//
// SPDX-License-Identifier: NIST-Software

#include <ns3/boolean.h>
#include <ns3/config.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/data-rate.h>
#include <ns3/double.h>
#include <ns3/integer.h>
#include <ns3/internet-stack-helper.h>
#include <ns3/ipv4-address-helper.h>
#include <ns3/log.h>
#include <ns3/mobility-model.h>
#include <ns3/node.h>
#include <ns3/nr-rrc-sap.h>
#include <ns3/nr-sl-comm-resource-pool-factory.h>
#include <ns3/nr-sl-comm-resource-pool.h>
#include <ns3/nr-sl-helper.h>
#include <ns3/nr-sl-radio-bearer-info.h>
#include <ns3/nr-sl-rlc-um.h>
#include <ns3/nr-sl-spectrum-phy.h>
#include <ns3/nr-sl-tft.h>
#include <ns3/nr-sl-ue-mac-harq.h>
#include <ns3/nr-sl-ue-mac-scheduler-default.h>
#include <ns3/nr-sl-ue-mac.h>
#include <ns3/nr-sl-ue-rrc.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-ue-phy.h>
#include <ns3/nstime.h>
#include <ns3/on-off-helper.h>
#include <ns3/packet-sink-helper.h>
#include <ns3/pointer.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/random-variable-stream.h>
#include <ns3/rng-seed-manager.h>
#include <ns3/seq-ts-header.h>
#include <ns3/seq-ts-size-header.h>
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/test.h>
#include <ns3/udp-client.h>
#include <ns3/udp-server.h>
#include <ns3/uinteger.h>

#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrSlSchedulerTest");

/**
 * This test file can be used for defining tests of the NR SL schedulers
 * See also nr-sl-simple-multi-lc-example-test.cc which is a test of
 * multi-LC scheduling behavior.
 *
 * The test design is to use a base class, NrSlSchedulerTestCase, to provide
 * a simple two-node setup, and then to use subclasses to define traffic
 * and tests of the scheduler behavior
 */
class NrSlSchedulerTestCase : public TestCase
{
  public:
    /**
     * \brief Create NrSlSchedulerTestCase
     * \param name Name of the test
     */
    NrSlSchedulerTestCase(const std::string& name)
        : TestCase(name)
    {
    }

  protected:
    Ptr<NrUeNetDevice> m_senderDevice;          //!<  Sender net device
    Ptr<NrUeNetDevice> m_receiverDevice;        //!< Receiver net device
    Ptr<NrSlHelper> m_nrSlHelper;               //!< NR SL helper
    Ptr<NrSlUeMacSchedulerDefault> m_scheduler; //!< Pointer to sender-side scheduler
    double m_slProbResourceKeep{0.0}; //!< Probability of keeping SPS resources at reselection
    uint16_t m_maxNumPerReserve{3};   //!< Maximum PSSCH resources per reservation (1-3)
    uint8_t m_maxSlHarqProcesses{16}; //!< Total HARQ process pool size
    uint8_t m_maxSlHarqProcessesMultiplePdu{4}; //!< HARQ processes reserved for SPS grants

  private:
    void DoSetup() override;
    void DoTeardown() override;

    uint32_t m_rngSeedCache{1}; //!< Cached value of RngSeed
    uint64_t m_rngRunCache{1};  //!< Cached value of RngRun
};

void
NrSlSchedulerTestCase::DoSetup()
{
    NS_LOG_FUNCTION(this);

    // Lock down the RngSeed and RngRun values for this test; cache previous values
    m_rngSeedCache = RngSeedManager::GetSeed();
    m_rngRunCache = RngSeedManager::GetRun();
    Config::SetGlobal("RngSeed", UintegerValue(1));
    Config::SetGlobal("RngRun", UintegerValue(1));

    // This configuration is intended to be as simple as possible to get two SL nodes
    // configured on a channel, without transmission impairments
    auto channel = CreateObject<SingleModelSpectrumChannel>();
    auto lossModel = CreateObject<FriisPropagationLossModel>();
    lossModel->GetObject<FriisPropagationLossModel>()->SetAttribute("Frequency",
                                                                    DoubleValue(793e6));
    channel->AddPropagationLossModel(lossModel);

    auto senderNode = CreateObject<Node>();
    auto receiverNode = CreateObject<Node>();
    NodeContainer nodes;
    nodes.Add(senderNode);
    nodes.Add(receiverNode);

    auto senderMobility = CreateObject<ConstantPositionMobilityModel>();
    senderMobility->SetPosition(Vector(0, 0, 0));
    senderNode->AggregateObject(senderMobility);
    auto receiverMobility = CreateObject<ConstantPositionMobilityModel>();
    receiverMobility->SetPosition(Vector(0, 10, 0));
    receiverNode->AggregateObject(receiverMobility);

    m_nrSlHelper = CreateObject<NrSlHelper>();
    m_nrSlHelper->SetSlProbResourceKeep(m_slProbResourceKeep);
    m_nrSlHelper->SetSlMaxNumPerReserve(m_maxNumPerReserve);
    m_nrSlHelper->SetSlMaxHarqProcesses(m_maxSlHarqProcesses, m_maxSlHarqProcessesMultiplePdu);
    auto netDev = m_nrSlHelper->ConfigureSlNetwork(nodes, channel);
    m_senderDevice = netDev.Get(0)->GetObject<NrUeNetDevice>();
    m_receiverDevice = netDev.Get(1)->GetObject<NrUeNetDevice>();
    NS_ASSERT_MSG(m_senderDevice && m_receiverDevice, "Downcast failed");

    // Configure internet
    InternetStackHelper internet;
    internet.Install(nodes);
    Ipv4AddressHelper addrHelper;
    addrHelper.SetBase("7.0.0.0", "255.0.0.0");
    auto ueIpIface = addrHelper.Assign(netDev);

    // Assign m_scheduler
    auto senderMac = m_senderDevice->GetMac(0)->GetObject<NrSlUeMac>();
    PointerValue p;
    senderMac->GetAttribute("NrSlUeMacScheduler", p);
    m_scheduler = p.GetObject()->GetObject<NrSlUeMacSchedulerDefault>();
    // Set MCS to 0 for small TB sizes
    m_scheduler->SetAttribute("DefaultMcs", UintegerValue(0));

    // Assign some stream numbers to random variables used
    int64_t streamBase(1000);
    [[maybe_unused]] int64_t streamsUsed(0);
    streamsUsed = m_nrSlHelper->AssignStreams(netDev, streamBase);
    streamBase = 2000;
    streamsUsed = internet.AssignStreams(nodes, streamBase);
}

void
NrSlSchedulerTestCase::DoTeardown()
{
    NS_LOG_FUNCTION(this);
    // Restore the seed and run number that were in effect before this test
    Config::SetGlobal("RngSeed", UintegerValue(m_rngSeedCache));
    Config::SetGlobal("RngRun", UintegerValue(m_rngRunCache));
}

/**
 * This test case creates a test flow of ten packets, one per second,
 * starting around time 4 s.   The test checks that the MCS for
 * a destination can be configured to a non-default value even
 * before there is any traffic for that destination, and the test
 * also checks that the MCS for a destination can be changed
 * during the flow.
 */
class NrSlSchedulerMcsConfigTestCase : public NrSlSchedulerTestCase
{
  public:
    /**
     * \brief Create NrSlSchedulerMcsConfigTestCase
     * \param name Name of the test
     */
    NrSlSchedulerMcsConfigTestCase(const std::string& name)
        : NrSlSchedulerTestCase(name)
    {
    }

  private:
    struct TimeSeriesData
    {
        Time t;
        uint8_t mcs;
    };

    void DoRun() override;
    void NotifySlPsschRx(SlRxDataPacketTraceParams params);
    void ChangeMcs(uint8_t mcs);
    void CheckMcs(Time start, Time stop, uint8_t mcs);
    std::vector<TimeSeriesData> m_received;
};

void
NrSlSchedulerMcsConfigTestCase::ChangeMcs(uint8_t mcs)
{
    NS_LOG_FUNCTION(this << mcs);
    m_scheduler->SetMcs(2, mcs); // dstL2Id of receiver is 2
}

void
NrSlSchedulerMcsConfigTestCase::CheckMcs(Time start, Time stop, uint8_t mcs)
{
    NS_LOG_FUNCTION(this << start.As(Time::S) << stop.As(Time::S) << mcs);
    for (const auto& [timestamp, rxMcs] : m_received)
    {
        if (timestamp < start)
        {
            continue;
        }
        if (timestamp > stop)
        {
            break;
        }
        NS_LOG_DEBUG("Checking timestamp " << timestamp.As(Time::S) << " received MCS " << +rxMcs
                                           << " against expected MCS " << +mcs);
        NS_TEST_ASSERT_MSG_EQ(+mcs, +rxMcs, "MCS received " << +rxMcs << " not " << +mcs);
    }
}

void
NrSlSchedulerMcsConfigTestCase::NotifySlPsschRx(SlRxDataPacketTraceParams params)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("DstL2Id " << params.m_dstL2Id << " mcs " << +params.m_mcs);
    TimeSeriesData d{Now(), params.m_mcs};
    m_received.push_back(d);
}

void
NrSlSchedulerMcsConfigTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    // Configure a small amount (10 packets) of user traffic
    uint32_t maxPackets{10};
    Time udpStart(Seconds(2));
    Time udpPacketInterval = Seconds(1);
    Time udpStop(Seconds(2) + maxPackets * udpPacketInterval + Seconds(1));
    uint32_t udpPacketSize(200);
    uint16_t udpPort(8000);

    // Set up the logical channel properties: dynamic grants, no HARQ
    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = false;
    slInfo.m_pdb = MilliSeconds(20);
    slInfo.m_dynamic = true;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;
    slInfo.m_rri = MilliSeconds(20);

    // Configure and activate bearers
    Ipv4Address destination("7.0.0.2");
    auto txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPort, slInfo);
    auto rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPort, slInfo);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(1), NetDeviceContainer(m_senderDevice), txTft);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(1), NetDeviceContainer(m_receiverDevice), rxTft);

    // Send 10 packets at a rate of a packet every second
    Address remoteAddress = InetSocketAddress(destination, udpPort);
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    sidelinkClient.SetAttribute("MaxBytes", UintegerValue(maxPackets * udpPacketSize));
    DataRate dataRate{static_cast<uint64_t>(udpPacketSize * 8 / udpPacketInterval.GetSeconds())};
    sidelinkClient.SetConstantRate(dataRate, udpPacketSize);
    ApplicationContainer clientApps = sidelinkClient.Install(m_senderDevice->GetNode());
    auto uniformRv = CreateObject<UniformRandomVariable>();
    uniformRv->SetAttribute("Max", DoubleValue(0.1)); // 100 ms
    uniformRv->SetStream(3000);
    clientApps.StartWithJitter(udpStart, uniformRv);
    clientApps.Stop(udpStop);
    ApplicationContainer serverApps;
    Address localAddress = InetSocketAddress(Ipv4Address::GetAny(), udpPort);
    PacketSinkHelper sidelinkSink("ns3::UdpSocketFactory", localAddress);
    sidelinkSink.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    serverApps = sidelinkSink.Install(m_receiverDevice->GetNode());
    serverApps.Start(udpStart);

    // Hook the receive NrSlSpectrumPhy PSCCH trace which provides the TB MCS
    auto rxSpectrumPhy =
        m_receiverDevice->GetPhy(0)->GetSpectrumPhy()->GetObject<NrSlSpectrumPhy>();
    rxSpectrumPhy->TraceConnectWithoutContext(
        "RxPsschTraceUe",
        MakeCallback(&NrSlSchedulerMcsConfigTestCase::NotifySlPsschRx, this));

    // Change the MCS from default of 0 to 1 at time 0.  This means that all initial data packets
    // should have an MCS of 1 (packets sent at times 2, 3, 4, 5, 6, 7).  At time 7.5 seconds,
    // change the MCS to 2 for the rest of the simulation.
    Simulator::ScheduleWithContext(0,
                                   Seconds(0),
                                   &NrSlSchedulerMcsConfigTestCase::ChangeMcs,
                                   this,
                                   1);
    Simulator::ScheduleWithContext(0,
                                   Seconds(7.5),
                                   &NrSlSchedulerMcsConfigTestCase::ChangeMcs,
                                   this,
                                   2);

    Simulator::Stop(udpStop);
    Simulator::Run();

    // Check that between times 0 and 7.5 s, only MCS 1 was received
    CheckMcs(Seconds(0), Seconds(7.5), 1);
    // Check that between times 7.5 and 13 s, only MCS 2 was received
    CheckMcs(Seconds(10), Seconds(13), 2);
    Simulator::Destroy();
}

/**
 * This test case verifies that when a packet is larger than the maximum TB size
 * at the configured MCS, multiple grants (for multiple TBs) are created at the
 * same scheduling trigger time.
 *
 * At MCS 0 with the default subchannel configuration, a 300-byte UDP payload
 * should require two TBs (a single TB in this configuration can accommodate
 * a 148-byte UDP payload, fitting within a 183 byte RLC PDU).
 */
class NrSlSchedulerMultipleTbTestCase : public NrSlSchedulerTestCase
{
  public:
    /**
     * \brief Create NrSlSchedulerMultipleTbTestCase
     * \param name Name of the test
     */
    NrSlSchedulerMultipleTbTestCase(const std::string& name)
        : NrSlSchedulerTestCase(name)
    {
    }

  private:
    void DoRun() override;
    void NotifySchedulingReport(
        const NrSlUeMacSchedulerDefault::SchedulingReport& report,
        const std::list<SlResourceInfo>& candidateResources,
        const NrSlUeMac::NrSlTransmissionParams& txParams,
        const std::vector<SlGrantResource>& publishedGrants,
        const std::map<uint32_t, std::vector<NrSlUeMacScheduler::GrantInfo>>& unpublishedGrants,
        const NrSlUeMacScheduler::GrantInfo& grant);

    /**
     * Structure to record scheduling events
     */
    struct SchedulingEvent
    {
        SfnSf sfn;              //!< Scheduling trigger time
        uint8_t harqId;         //!< HARQ ID assigned
        uint16_t lSubch;        //!< Number of subchannels
        uint32_t numCandidates; //!< Number of candidate resources
    };

    std::vector<SchedulingEvent> m_events; //!< Recorded scheduling events
};

void
NrSlSchedulerMultipleTbTestCase::NotifySchedulingReport(
    const NrSlUeMacSchedulerDefault::SchedulingReport& report,
    const std::list<SlResourceInfo>& candidateResources,
    const NrSlUeMac::NrSlTransmissionParams& txParams,
    [[maybe_unused]] const std::vector<SlGrantResource>& publishedGrants,
    [[maybe_unused]] const std::map<uint32_t, std::vector<NrSlUeMacScheduler::GrantInfo>>&
        unpublishedGrants,
    const NrSlUeMacScheduler::GrantInfo& grant)
{
    NS_LOG_FUNCTION(this << report.m_sfn << +grant.harqId);
    if (grant.isDynamic)
    {
        SchedulingEvent event;
        event.sfn = report.m_sfn;
        event.harqId = grant.harqId;
        event.lSubch = txParams.m_lSubch;
        event.numCandidates = candidateResources.size();
        m_events.push_back(event);
        NS_LOG_DEBUG("Scheduling event at " << report.m_sfn << " HARQ ID " << +grant.harqId
                                            << " lSubch " << txParams.m_lSubch << " candidates "
                                            << candidateResources.size());
    }
}

void
NrSlSchedulerMultipleTbTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    // Configure traffic with a packet size larger than TB size at MCS 0
    // At MCS 0 and 5 subchannels with HARQ and PSFCH disabled, the TB size
    // is 185 bytes, so a 300-byte UDP payload should require 2 TBs
    Time udpStart(Seconds(3));
    Time udpStop(Seconds(5));
    uint32_t udpPacketSize(300);
    uint16_t udpPort(8000);

    // Set up the logical channel properties: dynamic grants, no HARQ
    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = false;
    slInfo.m_pdb = MilliSeconds(100);
    slInfo.m_dynamic = true;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;
    slInfo.m_rri = MilliSeconds(100);
    Ipv4Address destination("7.0.0.2");
    Address remoteAddress = InetSocketAddress(destination, udpPort);
    auto txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPort, slInfo);
    auto rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPort, slInfo);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_senderDevice), txTft);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_receiverDevice), rxTft);

    // Send a single packet
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    sidelinkClient.SetAttribute("MaxBytes", UintegerValue(udpPacketSize));
    sidelinkClient.SetAttribute("DataRate", DataRateValue(DataRate("1Mbps")));
    sidelinkClient.SetAttribute("PacketSize", UintegerValue(udpPacketSize));
    ApplicationContainer clientApps = sidelinkClient.Install(m_senderDevice->GetNode());
    clientApps.Start(udpStart);
    clientApps.Stop(udpStop);

    // Hook the SchedulingReport trace
    m_scheduler->TraceConnectWithoutContext(
        "SchedulingReport",
        MakeCallback(&NrSlSchedulerMultipleTbTestCase::NotifySchedulingReport, this));

    Simulator::Stop(udpStop);
    Simulator::Run();

    // Verify that we received scheduling events (assert since we cannot continue without events)
    NS_TEST_ASSERT_MSG_GT(m_events.size(), 0, "Expected at least one scheduling event");

    // Group events by trigger time (SfnSf)
    std::map<uint64_t, std::vector<SchedulingEvent>> eventsByTrigger;
    for (const auto& event : m_events)
    {
        eventsByTrigger[event.sfn.Normalize()].push_back(event);
    }
    NS_TEST_EXPECT_MSG_EQ(eventsByTrigger.size(),
                          1,
                          "Expected all scheduling to occur in the same slot");
    NS_TEST_EXPECT_MSG_EQ(m_events.size(), 2, "Expected two dynamic grants");

    // Check that candidate resources decreased between grants
    NS_TEST_EXPECT_MSG_LT(m_events[1].numCandidates,
                          m_events[0].numCandidates,
                          "Expected candidate resources to decrease after first grant");

    Simulator::Destroy();
}

/**
 * This test case verifies the CalculateNumTbNeeded() logic by sending
 * a packet that fits in a single TB and verifying only one grant is created.
 *
 * At MCS 0, five subchannels, and 12 symbols of PSSCH, TB size is 185 bytes.
 */
class NrSlSchedulerNumTbCalculationTestCase : public NrSlSchedulerTestCase
{
  public:
    NrSlSchedulerNumTbCalculationTestCase(const std::string& name)
        : NrSlSchedulerTestCase(name)
    {
    }

  private:
    void DoRun() override;
    void NotifyGrantCreated(const NrSlUeMacScheduler::GrantInfo& grant, uint16_t psfchPeriod);

    uint32_t m_grantCount{0};
};

void
NrSlSchedulerNumTbCalculationTestCase::NotifyGrantCreated(
    const NrSlUeMacScheduler::GrantInfo& grant,
    [[maybe_unused]] uint16_t psfchPeriod)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("Grant created at " << grant.allocationTime << " HARQ ID " << +grant.harqId
                                     << " TX slot " << grant.slotAllocations.begin()->sfn
                                     << " dynamic: " << grant.isDynamic);
    if (grant.isDynamic)
    {
        m_grantCount++;
    }
}

void
NrSlSchedulerNumTbCalculationTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    Time udpStart(Seconds(3));
    Time udpStop(Seconds(5));
    // Test case: 148-byte packet should expand to 183-byte RLC PDU, fitting exactly within
    // one 185-byte TB (that allows for 2 bytes MAC subheader overhead).
    // 148 byte UDP payload becomes 176 byte IP datagram: 8 bytes (UDP) + 20 bytes (IP)
    // 176 byte IP packet becomes 183 byte RLC PDU: 5 bytes PDCP header + 2 bytes RLC header
    uint32_t udpPacketSize(148);
    uint16_t udpPort(8000);

    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = false;
    slInfo.m_pdb = MilliSeconds(100);
    slInfo.m_dynamic = true;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;
    slInfo.m_rri = MilliSeconds(100);
    Ipv4Address destination("7.0.0.2");
    Address remoteAddress = InetSocketAddress(destination, udpPort);
    auto txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPort, slInfo);
    auto rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPort, slInfo);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_senderDevice), txTft);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_receiverDevice), rxTft);

    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    sidelinkClient.SetAttribute("MaxBytes", UintegerValue(udpPacketSize));
    sidelinkClient.SetAttribute("DataRate", DataRateValue(DataRate("1Mbps")));
    sidelinkClient.SetAttribute("PacketSize", UintegerValue(udpPacketSize));
    ApplicationContainer clientApps = sidelinkClient.Install(m_senderDevice->GetNode());
    clientApps.Start(udpStart);
    clientApps.Stop(udpStop);

    m_scheduler->TraceConnectWithoutContext(
        "GrantCreated",
        MakeCallback(&NrSlSchedulerNumTbCalculationTestCase::NotifyGrantCreated, this));

    Simulator::Stop(udpStop);
    Simulator::Run();

    NS_TEST_EXPECT_MSG_EQ(m_grantCount, 1, "Expected 1 grant for 185-byte TB at MCS 0");

    Simulator::Destroy();
}

/**
 * This test case verifies that when a very large packet requires more TBs than
 * available HARQ process IDs, the scheduler:
 * 1. Creates exactly 16 grants (all available HARQ IDs) at the first opportunity
 * 2. Creates additional grants one-by-one as HARQ processes free up
 *
 * At MCS 0 with TB size 185 bytes (payload 183 bytes), a 4000-byte packet
 * needs approximately 23 TBs. The buffer includes protocol overhead:
 * - UDP header: 8 bytes
 * - IP header: 20 bytes
 * - PDCP header: 5 bytes
 * - RLC header: 2 bytes
 * Total: approx. 4037 bytes RLC SDU, requiring ceil(4037/183) = 23 TBs.
 *
 * With 16 HARQ IDs available, the first trigger creates 16 grants, and the
 * remaining 7 are created as HARQ processes expire (one per slot when HARQ
 * retransmissions are disabled). Each grant uses all subchannels, so there
 * can be at most one grant per slot.
 */
class NrSlSchedulerHarqExhaustionTestCase : public NrSlSchedulerTestCase
{
  public:
    NrSlSchedulerHarqExhaustionTestCase(const std::string& name)
        : NrSlSchedulerTestCase(name)
    {
    }

  private:
    void DoRun() override;
    void NotifyGrantCreated(const NrSlUeMacScheduler::GrantInfo& grant, uint16_t psfchPeriod);

    struct GrantCreatedEvent
    {
        Time allocationTime;
        uint8_t harqId;
        SfnSf transmissionSlot; //!< First transmission slot for this grant
    };

    std::vector<GrantCreatedEvent> m_createdEvents;
};

void
NrSlSchedulerHarqExhaustionTestCase::NotifyGrantCreated(const NrSlUeMacScheduler::GrantInfo& grant,
                                                        [[maybe_unused]] uint16_t psfchPeriod)
{
    if (grant.isDynamic)
    {
        GrantCreatedEvent event;
        event.allocationTime = grant.allocationTime;
        event.harqId = grant.harqId;
        // Get the first (and typically only) transmission slot for this grant
        NS_ASSERT_MSG(!grant.slotAllocations.empty(), "Grant has no slot allocations");
        event.transmissionSlot = grant.slotAllocations.begin()->sfn;
        m_createdEvents.push_back(event);
        NS_LOG_DEBUG("Grant created at " << grant.allocationTime << " HARQ ID " << +grant.harqId
                                         << " TX slot " << event.transmissionSlot);
    }
}

void
NrSlSchedulerHarqExhaustionTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    // Send a 4000-byte UDP payload in one packet that requires 23 TBs at MCS 0
    // With 16 HARQ IDs available, the first batch creates 16 grants, then 7 more later
    Time udpStart(Seconds(3));
    Time udpStop(Seconds(6));
    uint32_t udpPacketSize(4000);
    uint16_t udpPort(8000);

    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = false; // Disable HARQ retransmissions
    slInfo.m_pdb = MilliSeconds(100);
    slInfo.m_dynamic = true;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;
    slInfo.m_rri = MilliSeconds(100);
    Ipv4Address destination("7.0.0.2");
    Address remoteAddress = InetSocketAddress(destination, udpPort);
    auto txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPort, slInfo);
    auto rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPort, slInfo);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_senderDevice), txTft);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_receiverDevice), rxTft);

    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    sidelinkClient.SetAttribute("MaxBytes", UintegerValue(udpPacketSize));
    sidelinkClient.SetAttribute("DataRate", DataRateValue(DataRate("1Mbps")));
    sidelinkClient.SetAttribute("PacketSize", UintegerValue(udpPacketSize));
    ApplicationContainer clientApps = sidelinkClient.Install(m_senderDevice->GetNode());
    clientApps.Start(udpStart);
    clientApps.Stop(udpStop);

    m_scheduler->TraceConnectWithoutContext(
        "GrantCreated",
        MakeCallback(&NrSlSchedulerHarqExhaustionTestCase::NotifyGrantCreated, this));

    Simulator::Stop(udpStop);
    Simulator::Run();

    // Verify that grants were created
    NS_TEST_ASSERT_MSG_GT(m_createdEvents.size(), 0, "Expected at least one grant created");

    // Group created grants by allocation time
    std::map<Time, std::vector<GrantCreatedEvent>> eventsByAllocationTime;
    for (const auto& event : m_createdEvents)
    {
        eventsByAllocationTime[event.allocationTime].push_back(event);
    }

    // The first allocation time should have exactly 16 grants (using all HARQ process IDs).
    // The initial allocation computes ceil(4035/183) = 23 TBs.  After initially
    // allocated HARQ IDs are freed, and the scheduler subtracts already allocated
    // capacity, the additional grants account for per-segment RLC header overhead,
    // ensuring enough capacity to drain the buffer without an extra grant.
    constexpr uint32_t maxHarqIds = 16;
    constexpr uint32_t expectedTotalGrants = 23;
    constexpr uint32_t expectedLaterGrants = expectedTotalGrants - maxHarqIds;

    auto firstBatch = eventsByAllocationTime.begin();
    Time firstBatchTime = firstBatch->first;
    NS_TEST_EXPECT_MSG_EQ(firstBatch->second.size(),
                          maxHarqIds,
                          "First trigger should create exactly 16 grants");
    NS_LOG_INFO("First batch at " << firstBatchTime << " created " << firstBatch->second.size()
                                  << " grants");

    // Verify that all grants have unique transmission slots.
    // Since each TB occupies all subchannels, there can be at most one grant per slot.
    std::set<uint64_t> uniqueTransmissionSlots;
    for (const auto& event : m_createdEvents)
    {
        uint64_t slotNormalized = event.transmissionSlot.Normalize();
        NS_LOG_DEBUG("Grant HARQ ID " << +event.harqId << " uses TX slot " << slotNormalized);
        uniqueTransmissionSlots.insert(slotNormalized);
    }
    NS_TEST_EXPECT_MSG_EQ(uniqueTransmissionSlots.size(),
                          m_createdEvents.size(),
                          "All grants must have unique transmission slots");

    // Count grants and verify timing
    uint32_t totalGrants = 0;
    uint32_t laterGrants = 0;
    std::set<Time> laterAllocationTimes;

    for (const auto& [allocTime, events] : eventsByAllocationTime)
    {
        totalGrants += events.size();
        if (allocTime > firstBatchTime)
        {
            laterGrants += events.size();
            laterAllocationTimes.insert(allocTime);
            NS_LOG_INFO("Later grant(s) at " << allocTime << ": " << events.size() << " grant(s)");
        }
    }

    // Verify total grants matches expected
    NS_TEST_EXPECT_MSG_EQ(totalGrants,
                          expectedTotalGrants,
                          "Should create 23 total grants for single packet with 4000-byte payload");

    // Verify the number of later grants
    NS_TEST_EXPECT_MSG_EQ(laterGrants,
                          expectedLaterGrants,
                          "Should create 7 grants after HARQ processes become available");

    // Since each grant uses a unique slot and HARQ processes are freed one slot
    // after transmission, HARQ IDs should free up one at a time (staggered).
    // Therefore, each later grant should be created at a different allocation time.
    NS_TEST_EXPECT_MSG_EQ(laterAllocationTimes.size(),
                          expectedLaterGrants,
                          "Each later grant should have a unique allocation time "
                          "(HARQ IDs free up one per slot)");

    NS_LOG_INFO("Test completed: " << totalGrants << " grants across "
                                   << eventsByAllocationTime.size() << " allocation times");
    Simulator::Destroy();
}

/**
 * This test case checks baseline SPS grant behavior by sending 100
 * packets at 20 ms intervals on an SPS logical channel and confirming
 * that all packets are received with end-to-end latency under 20 ms.
 * Resources are reselected at three times.
 *
 * The test uses a 40-byte UDP payload (RLC PDU approx. 75 bytes), within
 * the single TB capacity at MCS 0 (183 bytes).  With an RRI of 20 ms
 * matching the packet interval, the test checks that no grant
 * opportunities are missed by looking at end-to-end latency.
 */
class NrSlSchedulerSpsTestCase : public NrSlSchedulerTestCase
{
  public:
    NrSlSchedulerSpsTestCase(const std::string& name)
        : NrSlSchedulerTestCase(name)
    {
    }

  private:
    void DoRun() override;
    void ReceivePacket(Ptr<const Packet> p,
                       const Address& from,
                       const Address& to,
                       const SeqTsSizeHeader& header);

    uint32_t m_rxCount{0};
    Time m_maxLatency{Seconds(0)};
    uint32_t m_latencyViolations{0};
};

void
NrSlSchedulerSpsTestCase::ReceivePacket([[maybe_unused]] Ptr<const Packet> p,
                                        [[maybe_unused]] const Address& from,
                                        [[maybe_unused]] const Address& to,
                                        const SeqTsSizeHeader& header)
{
    NS_LOG_FUNCTION(this << p);
    m_rxCount++;
    Time latency = Simulator::Now() - header.GetTs();
    if (latency > m_maxLatency)
    {
        m_maxLatency = latency;
    }
    NS_LOG_INFO("Rx packet seq=" << header.GetSeq() << " latency=" << latency.As(Time::MS));
    if (latency >= MilliSeconds(20))
    {
        m_latencyViolations++;
        NS_LOG_INFO("Latency violation: seq=" << header.GetSeq()
                                              << " latency=" << latency.As(Time::MS));
    }
}

void
NrSlSchedulerSpsTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    Time udpStart(Seconds(3));
    Time udpStop(Seconds(7));
    uint32_t udpPacketSize(40);
    uint16_t udpPort(8000);
    uint32_t numPackets(100);
    Time packetInterval(MilliSeconds(20));

    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = false;
    slInfo.m_pdb = MilliSeconds(20);
    slInfo.m_dynamic = false;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;
    slInfo.m_rri = MilliSeconds(20);
    Ipv4Address destination("7.0.0.2");
    Address remoteAddress = InetSocketAddress(destination, udpPort);
    auto txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPort, slInfo);
    auto rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPort, slInfo);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_senderDevice), txTft);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_receiverDevice), rxTft);

    // Sender: 1000 packets of 40 bytes at 20 ms intervals
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    sidelinkClient.SetAttribute("MaxBytes", UintegerValue(numPackets * udpPacketSize));
    DataRate dataRate{static_cast<uint64_t>(udpPacketSize * 8 / packetInterval.GetSeconds())};
    sidelinkClient.SetConstantRate(dataRate, udpPacketSize);
    ApplicationContainer clientApps = sidelinkClient.Install(m_senderDevice->GetNode());
    clientApps.Start(udpStart);
    clientApps.Stop(udpStop);

    // Receiver: PacketSink with SeqTsSize header for latency measurement
    Address localAddress = InetSocketAddress(Ipv4Address::GetAny(), udpPort);
    PacketSinkHelper sidelinkSink("ns3::UdpSocketFactory", localAddress);
    sidelinkSink.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    ApplicationContainer serverApps = sidelinkSink.Install(m_receiverDevice->GetNode());
    serverApps.Start(udpStart);

    serverApps.Get(0)->TraceConnectWithoutContext(
        "RxWithSeqTsSize",
        MakeCallback(&NrSlSchedulerSpsTestCase::ReceivePacket, this));

    Simulator::Stop(udpStop);
    Simulator::Run();

    NS_LOG_INFO("SPS baseline test: received "
                << m_rxCount << " of " << numPackets << " packets, max latency "
                << m_maxLatency.As(Time::MS) << ", violations " << m_latencyViolations);

    NS_TEST_EXPECT_MSG_EQ(m_rxCount,
                          numPackets,
                          "Expected " << numPackets << " packets but received " << m_rxCount);
    NS_TEST_EXPECT_MSG_EQ(m_latencyViolations,
                          0,
                          "Expected 0 latency violations but got "
                              << m_latencyViolations << " (max latency "
                              << m_maxLatency.As(Time::MS) << ")");

    Simulator::Destroy();
}

/**
 * This test case is a clone of NrSlSchedulerSpsTestCase that adds
 * instrumentation for the scheduling offset histogram (number of slots
 * past T1 that the actual TB transmission occurs) and will eventually
 * be extended with jittered packet arrivals to test supplemental
 * dynamic grant generation.
 */
class NrSlSchedulerSpsJitterTestCase : public NrSlSchedulerTestCase
{
  public:
    NrSlSchedulerSpsJitterTestCase(const std::string& name,
                                   uint32_t packetSize,
                                   bool expectSupplemental,
                                   uint8_t mcs = 0)
        : NrSlSchedulerTestCase(name),
          m_packetSize(packetSize),
          m_expectSupplemental(expectSupplemental),
          m_mcs(mcs)
    {
    }

  private:
    void DoRun() override;
    void ReceivePacket(Ptr<const Packet> p);
    void ChangeInterval(Time newInterval);
    void NotifySchedulingReport(
        const NrSlUeMacSchedulerDefault::SchedulingReport& report,
        const std::list<SlResourceInfo>& candidateResources,
        const NrSlUeMac::NrSlTransmissionParams& txParams,
        const std::vector<SlGrantResource>& publishedGrants,
        const std::map<uint32_t, std::vector<NrSlUeMacScheduler::GrantInfo>>& unpublishedGrants,
        const NrSlUeMacScheduler::GrantInfo& grant);

    uint32_t m_packetSize{40};        //!< UDP packet size
    bool m_expectSupplemental{false}; //!< Whether to expect supplemental dynamic grants
    uint8_t m_mcs{0};                 //!< MCS to use
    Ptr<UdpClient> m_udpClient;       //!< Sender app (held for later jitter injection)
    uint32_t m_rxCount{0};
    Time m_maxLatency{Seconds(0)};
    uint32_t m_latencyViolations{0};
    uint32_t m_supplementalGrantCount{0}; //!< Count of supplemental dynamic grants observed
    std::map<int64_t, uint32_t> m_txOffsetHistogram; //!< Histogram: slots past T1 -> count
};

void
NrSlSchedulerSpsJitterTestCase::ReceivePacket(Ptr<const Packet> p)
{
    SeqTsHeader seqTs;
    p->PeekHeader(seqTs);
    m_rxCount++;
    Time latency = Simulator::Now() - seqTs.GetTs();
    if (latency > m_maxLatency)
    {
        m_maxLatency = latency;
    }
    NS_LOG_DEBUG("Rx packet seq=" << seqTs.GetSeq() << " latency=" << latency.As(Time::MS));
    if (latency >= MilliSeconds(20))
    {
        m_latencyViolations++;
        NS_LOG_INFO("Latency violation: seq=" << seqTs.GetSeq()
                                              << " latency=" << latency.As(Time::MS));
    }
}

void
NrSlSchedulerSpsJitterTestCase::ChangeInterval(Time newInterval)
{
    NS_LOG_INFO("Changing UdpClient interval to " << newInterval.As(Time::MS));
    m_udpClient->SetAttribute("Interval", TimeValue(newInterval));
}

void
NrSlSchedulerSpsJitterTestCase::NotifySchedulingReport(
    const NrSlUeMacSchedulerDefault::SchedulingReport& report,
    [[maybe_unused]] const std::list<SlResourceInfo>& candidateResources,
    [[maybe_unused]] const NrSlUeMac::NrSlTransmissionParams& txParams,
    [[maybe_unused]] const std::vector<SlGrantResource>& publishedGrants,
    [[maybe_unused]] const std::map<uint32_t, std::vector<NrSlUeMacScheduler::GrantInfo>>&
        unpublishedGrants,
    const NrSlUeMacScheduler::GrantInfo& grant)
{
    if (grant.isDynamic)
    {
        m_supplementalGrantCount++;
        NS_LOG_INFO("Supplemental dynamic grant detected, HARQ ID " << +grant.harqId);
        return;
    }
    if (grant.slotAllocations.empty())
    {
        return;
    }
    // Find the first NDI slot (ndi == 1)
    for (const auto& slot : grant.slotAllocations)
    {
        if (slot.ndi == 1)
        {
            int64_t txSlot = static_cast<int64_t>(slot.sfn.Normalize());
            int64_t earliestSlot = static_cast<int64_t>(report.m_sfn.Normalize()) + report.m_t1;
            int64_t offset = txSlot - earliestSlot;
            m_txOffsetHistogram[offset]++;
            NS_LOG_INFO("SPS grant: scheduling sfn=" << report.m_sfn << " T1=" << +report.m_t1
                                                     << " first TX sfn=" << slot.sfn
                                                     << " offset=" << offset << " slots past T1");
            break;
        }
    }
}

void
NrSlSchedulerSpsJitterTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    // Set a specific stream to achieve a certain scheduling outcome
    m_scheduler->AssignStreams(60);

    if (m_mcs > 0)
    {
        m_scheduler->SetMcs(2, m_mcs);
    }

    Time udpStart(Seconds(3));
    Time udpStop(Seconds(3.25));
    uint32_t udpPacketSize(m_packetSize);
    uint16_t udpPort(8000);
    uint32_t numPackets(10);
    Time packetInterval(MilliSeconds(20));

    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = false;
    slInfo.m_pdb = MilliSeconds(20);
    slInfo.m_dynamic = false;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;
    slInfo.m_rri = MilliSeconds(20);
    Ipv4Address destination("7.0.0.2");
    auto txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPort, slInfo);
    auto rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPort, slInfo);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_senderDevice), txTft);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_receiverDevice), rxTft);

    // Sender: UdpClient sends packets at 20 ms intervals
    // UdpClient prepends a SeqTsHeader (12 bytes) to each packet
    m_udpClient = CreateObject<UdpClient>();
    m_udpClient->SetAttribute("RemoteAddress", AddressValue(destination));
    m_udpClient->SetAttribute("RemotePort", UintegerValue(udpPort));
    m_udpClient->SetAttribute("MaxPackets", UintegerValue(numPackets));
    m_udpClient->SetAttribute("Interval", TimeValue(packetInterval));
    m_udpClient->SetAttribute("PacketSize", UintegerValue(udpPacketSize));
    m_senderDevice->GetNode()->AddApplication(m_udpClient);
    m_udpClient->SetStartTime(udpStart);
    m_udpClient->SetStopTime(udpStop);

    // Receiver: UdpServer listens on the same port
    auto udpServer = CreateObject<UdpServer>();
    udpServer->SetAttribute("Port", UintegerValue(udpPort));
    m_receiverDevice->GetNode()->AddApplication(udpServer);
    udpServer->SetStartTime(udpStart);
    udpServer->SetStopTime(udpStop);

    udpServer->TraceConnectWithoutContext(
        "Rx",
        MakeCallback(&NrSlSchedulerSpsJitterTestCase::ReceivePacket, this));

    m_scheduler->TraceConnectWithoutContext(
        "SchedulingReport",
        MakeCallback(&NrSlSchedulerSpsJitterTestCase::NotifySchedulingReport, this));

    // Delay one packet by 10ms to force a missed SPS TX opportunity.
    //
    // Packets sent starting at sequence number 0 at 3.000s, at 20ms intervals
    // The SPS grant transmission slot is set up to
    // The second SPS grant (offset=7, T1=2) has TX slots at 3.989, 4.009, 4.029, ...
    // and publication (T1=2ms before TX) at 3.987, 4.007, 4.027, ...
    //
    // Three interval changes create a delay-then-compensate pattern:
    //   t=3.055: interval=30ms -> seq 4 delayed to 3.060 + 0.030 = 3.090 (10ms late)
    //   t=3.085: interval=10ms -> seq 5 accelerated to 3.090 + 0.010 -> corrected to 4.000
    //   t=3.095: interval=20ms -> seq 6 is scheduled at  at 4.020+20 = 4.040 (normal)
    //
    // Effect on TX slots:
    // - TX 4.009 (pub 4.007): seq 50 not yet in buffer (arrives 4.010) -> LOST TX
    // - TX 4.029 (pub 4.027): seq 50 (4.010) and seq 51 (4.020) both in buffer,
    //   both fit in one TB (2*75 < 183 bytes), both transmitted
    // - seq 50 latency: 4.029 + prop - 4.010 ~= 20ms (violation)
    // - seq 51 latency: 4.029 + prop - 4.020 ~= 10ms (normal)
    // - seq 52 onward: back to normal 10ms latency
    Simulator::Schedule(Seconds(3.055),
                        &NrSlSchedulerSpsJitterTestCase::ChangeInterval,
                        this,
                        MilliSeconds(30));
    Simulator::Schedule(Seconds(3.085),
                        &NrSlSchedulerSpsJitterTestCase::ChangeInterval,
                        this,
                        MilliSeconds(10));
    Simulator::Schedule(Seconds(3.095),
                        &NrSlSchedulerSpsJitterTestCase::ChangeInterval,
                        this,
                        MilliSeconds(20));

    Simulator::Stop(udpStop);
    Simulator::Run();

    // Log the TX offset histogram
    NS_LOG_INFO("SPS TX offset histogram (slots past T1):");
    for (const auto& [offset, count] : m_txOffsetHistogram)
    {
        NS_LOG_INFO("  offset=" << offset << " slots: " << count << " grants");
    }

    NS_LOG_INFO("SPS jitter test: received "
                << m_rxCount << " of " << numPackets << " packets, max latency "
                << m_maxLatency.As(Time::MS) << ", violations " << m_latencyViolations);

    NS_TEST_EXPECT_MSG_EQ(m_rxCount,
                          numPackets,
                          "Expected " << numPackets << " packets but received " << m_rxCount);
    NS_TEST_EXPECT_MSG_EQ(m_latencyViolations,
                          0,
                          "Expected 0 latency violations (supplemental grant should clear "
                          "backlog) but got "
                              << m_latencyViolations << " (max latency "
                              << m_maxLatency.As(Time::MS) << ")");

    if (m_expectSupplemental)
    {
        NS_TEST_EXPECT_MSG_EQ(m_supplementalGrantCount,
                              1u,
                              "Expected 1 supplemental dynamic grant but got "
                                  << m_supplementalGrantCount);
    }
    else
    {
        NS_TEST_EXPECT_MSG_EQ(m_supplementalGrantCount,
                              0u,
                              "Expected no supplemental grants (TB capacity should absorb "
                              "backlog) but got "
                                  << m_supplementalGrantCount);
    }

    Simulator::Destroy();
}

/**
 * This test case verifies that TB expansion at grant publication time
 * allows a dynamic grant to carry more data than the original buffer
 * that triggered it.
 *
 * A 20-byte UdpClient packet (which includes the 12-byte SeqTsHeader)
 * expands to a 53-byte RLC SDU (20 + 8 UDP + 20 IP + 5 PDCP), producing
 * a 55-byte buffer status (53 + 2 estimated RLC header).  At MCS 14, the
 * scheduler fits this in 1 subchannel (TB 253 bytes, payload 251 bytes).
 * Two packets are sent 2 ms apart so that the first packet triggers a
 * grant sized for 55 bytes before the second packet arrives.  The second
 * packet arrives before the first grant publishes (T1=2 slots), and
 * triggers a second grant for the excess.  At publication of the first
 * grant, TB expansion gives the full 251-byte payload to the RLC, which
 * dequeues both packets (110 bytes total).  The test asserts that both
 * packets are received at the receiver before the second dynamic grant is
 * published, proving that the first grant carried both packets via TB
 * expansion.
 */
class NrSlSchedulerDynamicTbExpansionTestCase : public NrSlSchedulerTestCase
{
  public:
    NrSlSchedulerDynamicTbExpansionTestCase(const std::string& name)
        : NrSlSchedulerTestCase(name)
    {
    }

  private:
    void DoRun() override;
    void NotifyGrantCreated(const NrSlUeMacScheduler::GrantInfo& grant, uint16_t psfchPeriod);
    void NotifyGrantPublished(const NrSlUeMac::NrSlGrant& grant, uint16_t psfchPeriod);
    void ReceivePacket(Ptr<const Packet> p);

    uint32_t m_grantCount{0};             //!< Dynamic grants created (via GrantCreated)
    uint32_t m_publishCount{0};           //!< Grants published (via GrantPublished)
    uint32_t m_rxCount{0};                //!< Packets received (via UdpServer Rx)
    uint32_t m_rxCountAtSecondPublish{0}; //!< Rx count when second grant publishes
};

void
NrSlSchedulerDynamicTbExpansionTestCase::NotifyGrantCreated(
    const NrSlUeMacScheduler::GrantInfo& grant,
    [[maybe_unused]] uint16_t psfchPeriod)
{
    if (grant.isDynamic)
    {
        m_grantCount++;
        NS_LOG_INFO("Dynamic grant " << m_grantCount << " created at " << grant.allocationTime
                                     << " HARQ ID " << +grant.harqId);
    }
}

void
NrSlSchedulerDynamicTbExpansionTestCase::NotifyGrantPublished(
    [[maybe_unused]] const NrSlUeMac::NrSlGrant& grant,
    [[maybe_unused]] uint16_t psfchPeriod)
{
    m_publishCount++;
    NS_LOG_INFO("Grant published (count " << m_publishCount << ") HARQ ID " << +grant.harqId
                                          << " at " << Now().As(Time::MS)
                                          << ", rxCount=" << m_rxCount);
    if (m_publishCount == 2)
    {
        m_rxCountAtSecondPublish = m_rxCount;
    }
}

void
NrSlSchedulerDynamicTbExpansionTestCase::ReceivePacket(Ptr<const Packet> p)
{
    m_rxCount++;
    NS_LOG_INFO("Packet " << m_rxCount << " received at " << Now().As(Time::MS)
                          << " size=" << p->GetSize());
}

void
NrSlSchedulerDynamicTbExpansionTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    // Set MCS to 14 for dstL2Id=2 (receiver).  At MCS 14 with 1 subchannel,
    // TB size is 253 bytes (payload 251 bytes), which is large enough to
    // hold two 20-byte packets (110 bytes total) after TB expansion.
    m_scheduler->SetMcs(2, 14);

    Time udpStart(Seconds(3));
    Time udpStop(Seconds(3.1));
    uint32_t udpPacketSize(20);
    uint16_t udpPort(8000);

    // Set up the logical channel properties: dynamic grants, no HARQ
    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = false;
    slInfo.m_pdb = MilliSeconds(100);
    slInfo.m_dynamic = true;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;
    slInfo.m_rri = MilliSeconds(100);
    Ipv4Address destination("7.0.0.2");
    auto txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPort, slInfo);
    auto rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPort, slInfo);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_senderDevice), txTft);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_receiverDevice), rxTft);

    // Send two packets 2 ms apart.  The 2 ms gap ensures that the first
    // packet triggers a grant (at the next 1 ms slot boundary) before the
    // second packet arrives.  The second packet arrives before the first
    // grant publishes (publication is at least T1=2 slots after scheduling).
    auto client = CreateObject<UdpClient>();
    client->SetAttribute("RemoteAddress", AddressValue(destination));
    client->SetAttribute("RemotePort", UintegerValue(udpPort));
    client->SetAttribute("MaxPackets", UintegerValue(2));
    client->SetAttribute("Interval", TimeValue(MilliSeconds(2)));
    client->SetAttribute("PacketSize", UintegerValue(udpPacketSize));
    m_senderDevice->GetNode()->AddApplication(client);
    client->SetStartTime(udpStart);
    client->SetStopTime(udpStop);

    // Receiver
    auto udpServer = CreateObject<UdpServer>();
    udpServer->SetAttribute("Port", UintegerValue(udpPort));
    m_receiverDevice->GetNode()->AddApplication(udpServer);
    udpServer->SetStartTime(udpStart);
    udpServer->SetStopTime(udpStop);

    udpServer->TraceConnectWithoutContext(
        "Rx",
        MakeCallback(&NrSlSchedulerDynamicTbExpansionTestCase::ReceivePacket, this));

    // Connect scheduler traces
    m_scheduler->TraceConnectWithoutContext(
        "GrantCreated",
        MakeCallback(&NrSlSchedulerDynamicTbExpansionTestCase::NotifyGrantCreated, this));
    m_scheduler->TraceConnectWithoutContext(
        "GrantPublished",
        MakeCallback(&NrSlSchedulerDynamicTbExpansionTestCase::NotifyGrantPublished, this));

    Simulator::Stop(udpStop);
    Simulator::Run();

    NS_LOG_INFO("Dynamic TB expansion test: "
                << m_grantCount << " grants created, " << m_publishCount << " published, "
                << m_rxCount << " packets received, "
                << "rxCountAtSecondPublish=" << m_rxCountAtSecondPublish);

    NS_TEST_EXPECT_MSG_EQ(m_grantCount,
                          2,
                          "Expected 2 dynamic grants (one per scheduling trigger)");
    NS_TEST_EXPECT_MSG_EQ(m_rxCount, 2, "Expected both packets received");
    NS_TEST_EXPECT_MSG_EQ(m_rxCountAtSecondPublish,
                          2,
                          "Expected both packets received before second grant published "
                          "(TB expansion should drain both in first grant)");

    Simulator::Destroy();
}

/**
 * This test case verifies that the scheduler correctly estimates the number of
 * TBs needed when the RLC must segment SDUs across TB boundaries.
 *
 * The RLC buffer status includes an estimated 2-byte header per SDU. However,
 * when the RLC packs multiple SDUs into one PDU, additional Length Indicator
 * (LI) overhead is incurred that is not reflected in the buffer estimate.
 * This can cause the scheduler to underestimate the number of TBs needed,
 * leaving stray bytes in the RLC buffer.
 *
 * Setup: Three SDUs of 100, 100, and 160 bytes (UDP packet sizes 67, 67, 127).
 * Buffer status = (100 + 100 + 160) + 2 * 3 = 366 bytes.
 * At MCS 0 with TB size 185, capacity per TB = 183, so ceil(366 / 183) = 2 TBs.
 * But the LI overhead when packing the first SDU and part of the second SDU into TB1
 * causes the third SDU to not fully fit in TB2, leaving 2 stray bytes that require a 3rd TB.
 */
class NrSlSchedulerRlcSegmentationTestCase : public NrSlSchedulerTestCase
{
  public:
    NrSlSchedulerRlcSegmentationTestCase(const std::string& name)
        : NrSlSchedulerTestCase(name)
    {
    }

  private:
    void DoRun() override;
    void ConnectRlcTrace();
    void NotifyGrantCreated(const NrSlUeMacScheduler::GrantInfo& grant, uint16_t psfchPeriod);
    void NotifyTxBufferSize(uint32_t oldVal, uint32_t newVal);

    uint32_t m_grantCount{0};         //!< Number of dynamic grants created
    uint32_t m_lastTxBufferSize{0};   //!< Last observed RLC TX buffer size
    uint32_t m_strayBytesObserved{0}; //!< Non-zero buffer size after transmission started
};

void
NrSlSchedulerRlcSegmentationTestCase::NotifyGrantCreated(const NrSlUeMacScheduler::GrantInfo& grant,
                                                         [[maybe_unused]] uint16_t psfchPeriod)
{
    if (grant.isDynamic)
    {
        m_grantCount++;
        NS_LOG_INFO("Grant " << m_grantCount << " created at " << grant.allocationTime
                             << " HARQ ID " << +grant.harqId);
    }
}

void
NrSlSchedulerRlcSegmentationTestCase::NotifyTxBufferSize(uint32_t oldVal, uint32_t newVal)
{
    NS_LOG_INFO("RLC TxBufferSize: " << oldVal << " -> " << newVal);
    m_lastTxBufferSize = newVal;
    // Track the case where the buffer decreased but did not reach zero
    if (oldVal > newVal && newVal > 0 && newVal < 10)
    {
        m_strayBytesObserved = newVal;
    }
}

void
NrSlSchedulerRlcSegmentationTestCase::ConnectRlcTrace()
{
    NS_LOG_FUNCTION(this);
    auto rrc = m_senderDevice->GetRrc()->GetObject<NrSlUeRrc>();
    NS_TEST_ASSERT_MSG_NE(rrc, nullptr, "Failed to get NrSlUeRrc from sender device");
    auto bearers = rrc->GetAllSidelinkTxDataRadioBearers(2);
    NS_TEST_ASSERT_MSG_GT(bearers.size(), 0, "No TX bearers found for dstL2Id=2");
    auto rlc = bearers.begin()->second->m_rlc->GetObject<NrSlRlcUm>();
    NS_TEST_ASSERT_MSG_NE(rlc, nullptr, "Failed to get NrSlRlcUm from bearer");
    rlc->TraceConnectWithoutContext(
        "TxBufferSize",
        MakeCallback(&NrSlSchedulerRlcSegmentationTestCase::NotifyTxBufferSize, this));
    NS_LOG_INFO("Connected to RLC TxBufferSize trace");
}

void
NrSlSchedulerRlcSegmentationTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    Time udpStart(Seconds(3));
    Time udpStop(Seconds(3.1));
    uint16_t udpPort(8000);

    // SDU sizes: 100, 100, 160 bytes (UDP packet size + 33 bytes overhead = RLC SDU)
    // Buffer status: (100 + 100 + 160) + 2 * 3 = 366 = 183 * 2; scheduler estimates 2 TBs
    uint32_t smallPacketSize(67);  // 100-byte RLC SDU
    uint32_t largePacketSize(127); // 160-byte RLC SDU

    // Set up the logical channel properties: dynamic grants, no HARQ
    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = false;
    slInfo.m_pdb = MilliSeconds(100);
    slInfo.m_dynamic = true;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;
    slInfo.m_rri = MilliSeconds(100);
    Ipv4Address destination("7.0.0.2");
    auto txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPort, slInfo);
    auto rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPort, slInfo);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_senderDevice), txTft);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_receiverDevice), rxTft);

    // Client 1: sends two small packets at 10 ns apart
    auto client1 = CreateObject<UdpClient>();
    client1->SetAttribute("RemoteAddress", AddressValue(destination));
    client1->SetAttribute("RemotePort", UintegerValue(udpPort));
    client1->SetAttribute("MaxPackets", UintegerValue(2));
    client1->SetAttribute("Interval", TimeValue(NanoSeconds(10)));
    client1->SetAttribute("PacketSize", UintegerValue(smallPacketSize));
    m_senderDevice->GetNode()->AddApplication(client1);
    client1->SetStartTime(udpStart);
    client1->SetStopTime(udpStop);

    // Client 2: sends one large packet at the same time
    auto client2 = CreateObject<UdpClient>();
    client2->SetAttribute("RemoteAddress", AddressValue(destination));
    client2->SetAttribute("RemotePort", UintegerValue(udpPort));
    client2->SetAttribute("MaxPackets", UintegerValue(1));
    client2->SetAttribute("Interval", TimeValue(Seconds(1)));
    client2->SetAttribute("PacketSize", UintegerValue(largePacketSize));
    m_senderDevice->GetNode()->AddApplication(client2);
    client2->SetStartTime(udpStart);
    client2->SetStopTime(udpStop);

    // Connect to the GrantCreated trace
    m_scheduler->TraceConnectWithoutContext(
        "GrantCreated",
        MakeCallback(&NrSlSchedulerRlcSegmentationTestCase::NotifyGrantCreated, this));

    // Schedule RLC trace connection after bearer activation but before packets
    Simulator::Schedule(Seconds(2.5), &NrSlSchedulerRlcSegmentationTestCase::ConnectRlcTrace, this);

    Simulator::Stop(udpStop);
    Simulator::Run();

    NS_LOG_INFO("RLC segmentation test: " << m_grantCount << " grants created, "
                                          << "stray bytes observed: " << m_strayBytesObserved
                                          << ", final buffer: " << m_lastTxBufferSize);

    // The scheduler accounts for per-segment RLC header overhead when multiple
    // TBs are needed, so ceil(366/181) = 3 TBs are allocated upfront.  This
    // avoids stray bytes that would otherwise require an extra grant via the
    // RBS timer.
    NS_TEST_EXPECT_MSG_EQ(m_grantCount,
                          3,
                          "Expected 3 grants for 366-byte buffer (3 TBs at 181 effective bytes) "
                          "but got "
                              << m_grantCount << " (stray bytes: " << m_strayBytesObserved << ")");

    Simulator::Destroy();
}

/**
 * This test case verifies SPS grant reselection behavior by running SPS
 * traffic long enough for the reselection counter to expire multiple times.
 *
 * It tracks SPS grant creations via the GrantCreated trace.  The initial
 * grant creation accounts for one event.  Each reselection (counter reaches
 * zero with pendingReselection=true) erases the old grant and triggers a
 * new resource selection, producing another GrantCreated event.  In the
 * "keep" case, the grant is repopulated with resources without a new GrantCreated.
 *
 * The test is parameterized by slProbResourceKeep:
 * - SlProbResourceKeep = 0.0: always reselect
 * - SlProbResourceKeep = 0.8: usually keep
 */
class NrSlSchedulerSpsReselectionTestCase : public NrSlSchedulerTestCase
{
  public:
    NrSlSchedulerSpsReselectionTestCase(const std::string& name, double probResourceKeep)
        : NrSlSchedulerTestCase(name)
    {
        if (probResourceKeep > 0)
        {
            NS_TEST_EXPECT_MSG_EQ(probResourceKeep, 0.8, "Only the values 0 and 0.8 are supported");
            m_slProbResourceKeep = probResourceKeep;
        }
    }

  private:
    void DoRun() override;
    void NotifyGrantCreated(const NrSlUeMacScheduler::GrantInfo& grant, uint16_t psfchPeriod);
    void NotifyGrantReused(double slProbResourceKeep, double randomVariate);
    void NotifyGrantPublished(const NrSlUeMac::NrSlGrant& grant, uint16_t psfchPeriod);

    uint32_t m_spsGrantCreatedCount{0}; //!< Number of SPS GrantCreated events
    uint32_t m_spsGrantReusedCount{0};  //!< Number of SPS GrantReused events
    uint32_t m_spsNdiPublishCount{0};   //!< Number of SPS NDI publications
};

void
NrSlSchedulerSpsReselectionTestCase::NotifyGrantCreated(const NrSlUeMacScheduler::GrantInfo& grant,
                                                        [[maybe_unused]] uint16_t psfchPeriod)
{
    if (!grant.isDynamic)
    {
        m_spsGrantCreatedCount++;
        NS_LOG_INFO("SPS grant created (count "
                    << m_spsGrantCreatedCount << ") at " << grant.allocationTime.As(Time::S)
                    << " HARQ ID " << +grant.harqId << " counter " << +grant.slResoReselCounter);
    }
}

void
NrSlSchedulerSpsReselectionTestCase::NotifyGrantReused(double slProbResourceKeep,
                                                       double randomVariate)
{
    m_spsGrantReusedCount++;
    NS_LOG_INFO("SPS grant reused count incremented to " << m_spsGrantCreatedCount
                                                         << " from probability " << randomVariate);
    NS_TEST_EXPECT_MSG_LT_OR_EQ(randomVariate,
                                m_slProbResourceKeep,
                                "Expected the random variate to be <= the Sl-ProbResourceKeep");
}

void
NrSlSchedulerSpsReselectionTestCase::NotifyGrantPublished(const NrSlUeMac::NrSlGrant& grant,
                                                          [[maybe_unused]] uint16_t psfchPeriod)
{
    // Count NDI publications (ndi == 1 in first slot).
    // This test only creates SPS traffic, so all publications are SPS.
    if (!grant.slotAllocations.empty() && grant.slotAllocations.begin()->ndi == 1)
    {
        m_spsNdiPublishCount++;
    }
}

void
NrSlSchedulerSpsReselectionTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    // Use RRI 100ms so counter range is [5, 15], giving reselection
    // every 500-1500ms.  Run for 5 seconds to see multiple cycles.
    Time udpStart(Seconds(3));
    Time udpStop(Seconds(8));
    uint32_t udpPacketSize(40);
    uint16_t udpPort(8000);
    Time packetInterval(MilliSeconds(100));

    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = false;
    slInfo.m_pdb = MilliSeconds(100);
    slInfo.m_dynamic = false;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;
    slInfo.m_rri = MilliSeconds(100);
    Ipv4Address destination("7.0.0.2");
    Address remoteAddress = InetSocketAddress(destination, udpPort);
    auto txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPort, slInfo);
    auto rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPort, slInfo);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_senderDevice), txTft);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_receiverDevice), rxTft);

    // Send packets at 100ms intervals (matching RRI)
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    uint32_t numPackets = 50;
    sidelinkClient.SetAttribute("MaxBytes", UintegerValue(numPackets * udpPacketSize));
    DataRate dataRate{static_cast<uint64_t>(udpPacketSize * 8 / packetInterval.GetSeconds())};
    sidelinkClient.SetConstantRate(dataRate, udpPacketSize);
    ApplicationContainer clientApps = sidelinkClient.Install(m_senderDevice->GetNode());
    clientApps.Start(udpStart);
    clientApps.Stop(udpStop);

    // Connect traces
    m_scheduler->TraceConnectWithoutContext(
        "GrantCreated",
        MakeCallback(&NrSlSchedulerSpsReselectionTestCase::NotifyGrantCreated, this));
    m_scheduler->TraceConnectWithoutContext(
        "GrantReused",
        MakeCallback(&NrSlSchedulerSpsReselectionTestCase::NotifyGrantReused, this));
    m_scheduler->TraceConnectWithoutContext(
        "GrantPublished",
        MakeCallback(&NrSlSchedulerSpsReselectionTestCase::NotifyGrantPublished, this));

    Simulator::Stop(udpStop);
    Simulator::Run();

    NS_LOG_INFO("SPS reselection test (probKeep=" << m_slProbResourceKeep
                                                  << "): grantCreated=" << m_spsGrantCreatedCount
                                                  << " ndiPublished=" << m_spsNdiPublishCount);

    if (m_slProbResourceKeep == 0.0)
    {
        // With prob=0, always reselect.  Counter range [5,15] with RRI 100 ms means that,
        // during 5 seconds, at least 3 reselection cycles should occur.
        // GrantCreated count = 1 (initial) + number of reselections.
        NS_LOG_DEBUG("Observed " << m_spsGrantCreatedCount
                                 << " SPS grants created with slProbResourceKeep of 0.0");
        NS_TEST_EXPECT_MSG_GT(m_spsGrantCreatedCount,
                              3,
                              "Expected at least 3 reselections (4 grant creations) "
                              "with slProbResourceKeep=0 over 5 seconds");
        NS_TEST_EXPECT_MSG_EQ(m_spsGrantReusedCount, 0, "Expected no grants to be reused");
    }
    else
    {
        // With the value 0.8, most cycles should keep.  The number of GrantCreated
        // events should be less than with slProbResourceKeep = 0, but at least the initial one.
        // With NDI publications continuing through keep cycles, there should
        // be more publications than grant creations.
        NS_LOG_DEBUG("Observed " << m_spsGrantCreatedCount
                                 << " SPS grants created with slProbResourceKeep of 0.8");
        NS_TEST_EXPECT_MSG_GT(m_spsGrantCreatedCount,
                              0,
                              "Expected at least the initial SPS grant creation");
        NS_LOG_DEBUG("Observed " << m_spsNdiPublishCount << " NDIs published");
        NS_TEST_EXPECT_MSG_GT(m_spsNdiPublishCount,
                              m_spsGrantCreatedCount,
                              "Expected more NDI publications than grant creations "
                              "(keep cycles should produce publications without new grants)");
        NS_TEST_EXPECT_MSG_GT(m_spsGrantReusedCount,
                              m_spsGrantCreatedCount,
                              "Expected more reused grants than reselected grants");
    }

    Simulator::Destroy();
}

/**
 * Test case for HARQ process ID lifecycle during SPS reselection.
 *
 * Verifies that HARQ process IDs are correctly managed across the SPS grant
 * lifecycle, including keep/reselect decisions and premature grant exhaustion
 * due to missing data. The test is parameterized to cover all combinations of:
 * - Probability of resource keep (0.0 vs 0.8)
 * - HARQ feedback enabled/disabled
 * - Retransmissions enabled/disabled (maxNumPerReserve 1 vs 3)
 * - Number of packets (50 for normal flow, 3 for premature exhaustion)
 */
class NrSlSchedulerHarqLifecycleTestCase : public NrSlSchedulerTestCase
{
  public:
    /**
     * Constructor.
     *
     * @param name Test case name.
     * @param probResourceKeep Probability of keeping SPS resources at reselection.
     * @param harqEnabled Whether HARQ feedback is enabled.
     * @param maxNumPerReserve Maximum PSSCH resources per reservation (1-3).
     * @param numPackets Number of packets to generate.
     */
    NrSlSchedulerHarqLifecycleTestCase(const std::string& name,
                                       double probResourceKeep,
                                       bool harqEnabled,
                                       uint16_t maxNumPerReserve,
                                       uint32_t numPackets);

  private:
    void DoRun() override;

    /**
     * Callback for HARQ Allocate trace.
     *
     * @param harqId The allocated HARQ process ID.
     * @param dstL2Id The destination L2 ID.
     * @param multiplePdu Whether this is a multiple-PDU (SPS) allocation.
     * @param timeout The HARQ timer timeout value.
     * @param available Number of HARQ IDs still available.
     */
    void NotifyHarqAllocate(uint8_t harqId,
                            uint32_t dstL2Id,
                            bool multiplePdu,
                            Time timeout,
                            std::size_t available);

    /**
     * Callback for HARQ Deallocate trace.
     *
     * @param harqId The deallocated HARQ process ID.
     * @param available Number of HARQ IDs still available.
     */
    void NotifyHarqDeallocate(uint8_t harqId, std::size_t available);

    /**
     * Callback for HARQ Timeout trace.
     *
     * @param harqId The HARQ process ID that timed out.
     */
    void NotifyHarqTimeout(uint8_t harqId);

    /**
     * Callback for scheduler GrantCreated trace.
     *
     * @param grant The created grant.
     * @param psfchPeriod The PSFCH period.
     */
    void NotifyGrantCreated(const NrSlUeMacScheduler::GrantInfo& grant, uint16_t psfchPeriod);

    /**
     * Callback for scheduler GrantReused trace.
     *
     * @param slProbResourceKeep The configured keep probability.
     * @param randomVariate The random variate that was drawn.
     */
    void NotifyGrantReused(double slProbResourceKeep, double randomVariate);

    /**
     * Callback for scheduler GrantPublished trace.
     *
     * @param grant The published grant.
     * @param psfchPeriod The PSFCH period.
     */
    void NotifyGrantPublished(const NrSlUeMac::NrSlGrant& grant, uint16_t psfchPeriod);

    bool m_harqEnabled;    //!< Whether HARQ feedback is enabled
    uint32_t m_numPackets; //!< Number of packets to generate

    uint32_t m_spsGrantCreatedCount{0}; //!< Number of SPS GrantCreated events
    uint32_t m_spsGrantReusedCount{0};  //!< Number of SPS GrantReused events
    uint32_t m_spsNdiPublishCount{0};   //!< Number of SPS NDI publications
    uint32_t m_harqAllocateCount{0};    //!< Number of HARQ Allocate events
    uint32_t m_harqDeallocateCount{0};  //!< Number of HARQ Deallocate events
    uint32_t m_harqTimeoutCount{0};     //!< Number of HARQ Timeout events
};

NrSlSchedulerHarqLifecycleTestCase::NrSlSchedulerHarqLifecycleTestCase(const std::string& name,
                                                                       double probResourceKeep,
                                                                       bool harqEnabled,
                                                                       uint16_t maxNumPerReserve,
                                                                       uint32_t numPackets)
    : NrSlSchedulerTestCase(name),
      m_harqEnabled(harqEnabled),
      m_numPackets(numPackets)
{
    if (probResourceKeep > 0)
    {
        NS_TEST_EXPECT_MSG_EQ(probResourceKeep, 0.8, "Only the values 0 and 0.8 are supported");
    }
    m_slProbResourceKeep = probResourceKeep;
    m_maxNumPerReserve = maxNumPerReserve;
}

void
NrSlSchedulerHarqLifecycleTestCase::NotifyHarqAllocate(uint8_t harqId,
                                                       [[maybe_unused]] uint32_t dstL2Id,
                                                       bool multiplePdu,
                                                       [[maybe_unused]] Time timeout,
                                                       [[maybe_unused]] std::size_t available)
{
    if (multiplePdu)
    {
        m_harqAllocateCount++;
        NS_LOG_INFO("HARQ allocate (SPS): ID " << +harqId << " count " << m_harqAllocateCount);
    }
}

void
NrSlSchedulerHarqLifecycleTestCase::NotifyHarqDeallocate(uint8_t harqId,
                                                         [[maybe_unused]] std::size_t available)
{
    m_harqDeallocateCount++;
    NS_LOG_INFO("HARQ deallocate: ID " << +harqId << " count " << m_harqDeallocateCount);
}

void
NrSlSchedulerHarqLifecycleTestCase::NotifyHarqTimeout(uint8_t harqId)
{
    m_harqTimeoutCount++;
    NS_LOG_INFO("HARQ timeout: ID " << +harqId << " count " << m_harqTimeoutCount);
}

void
NrSlSchedulerHarqLifecycleTestCase::NotifyGrantCreated(const NrSlUeMacScheduler::GrantInfo& grant,
                                                       [[maybe_unused]] uint16_t psfchPeriod)
{
    if (!grant.isDynamic)
    {
        m_spsGrantCreatedCount++;
        NS_LOG_INFO("SPS grant created (count " << m_spsGrantCreatedCount << ") HARQ ID "
                                                << +grant.harqId << " counter "
                                                << +grant.slResoReselCounter);
    }
}

void
NrSlSchedulerHarqLifecycleTestCase::NotifyGrantReused([[maybe_unused]] double slProbResourceKeep,
                                                      [[maybe_unused]] double randomVariate)
{
    m_spsGrantReusedCount++;
    NS_LOG_INFO("SPS grant reused (count " << m_spsGrantReusedCount << ")");
}

void
NrSlSchedulerHarqLifecycleTestCase::NotifyGrantPublished(const NrSlUeMac::NrSlGrant& grant,
                                                         [[maybe_unused]] uint16_t psfchPeriod)
{
    if (!grant.slotAllocations.empty() && grant.slotAllocations.begin()->ndi == 1)
    {
        m_spsNdiPublishCount++;
    }
}

void
NrSlSchedulerHarqLifecycleTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    Time udpStart(Seconds(3));
    Time udpStop(Seconds(8));
    uint32_t udpPacketSize(40);
    uint16_t udpPort(8000);
    Time packetInterval(MilliSeconds(100));

    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = m_harqEnabled;
    slInfo.m_pdb = MilliSeconds(100);
    slInfo.m_dynamic = false;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;
    slInfo.m_rri = MilliSeconds(100);
    Ipv4Address destination("7.0.0.2");
    Address remoteAddress = InetSocketAddress(destination, udpPort);
    auto txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPort, slInfo);
    auto rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPort, slInfo);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_senderDevice), txTft);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_receiverDevice), rxTft);

    // Send packets at 100ms intervals (matching RRI)
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    sidelinkClient.SetAttribute("MaxBytes", UintegerValue(m_numPackets * udpPacketSize));
    DataRate dataRate{static_cast<uint64_t>(udpPacketSize * 8 / packetInterval.GetSeconds())};
    sidelinkClient.SetConstantRate(dataRate, udpPacketSize);
    ApplicationContainer clientApps = sidelinkClient.Install(m_senderDevice->GetNode());
    clientApps.Start(udpStart);
    clientApps.Stop(udpStop);

    // Connect scheduler traces
    m_scheduler->TraceConnectWithoutContext(
        "GrantCreated",
        MakeCallback(&NrSlSchedulerHarqLifecycleTestCase::NotifyGrantCreated, this));
    m_scheduler->TraceConnectWithoutContext(
        "GrantReused",
        MakeCallback(&NrSlSchedulerHarqLifecycleTestCase::NotifyGrantReused, this));
    m_scheduler->TraceConnectWithoutContext(
        "GrantPublished",
        MakeCallback(&NrSlSchedulerHarqLifecycleTestCase::NotifyGrantPublished, this));

    // Connect HARQ traces
    auto senderMac = m_senderDevice->GetMac(0)->GetObject<NrSlUeMac>();
    PointerValue harqPtrValue;
    senderMac->GetAttribute("NrSlUeMacHarq", harqPtrValue);
    auto harq = harqPtrValue.GetObject()->GetObject<NrSlUeMacHarq>();
    NS_ASSERT_MSG(harq, "Failed to get NrSlUeMacHarq from sender MAC");
    harq->TraceConnectWithoutContext(
        "Allocate",
        MakeCallback(&NrSlSchedulerHarqLifecycleTestCase::NotifyHarqAllocate, this));
    harq->TraceConnectWithoutContext(
        "Deallocate",
        MakeCallback(&NrSlSchedulerHarqLifecycleTestCase::NotifyHarqDeallocate, this));
    harq->TraceConnectWithoutContext(
        "Timeout",
        MakeCallback(&NrSlSchedulerHarqLifecycleTestCase::NotifyHarqTimeout, this));

    Simulator::Stop(udpStop);
    Simulator::Run();

    NS_LOG_INFO("HARQ lifecycle test: grantCreated="
                << m_spsGrantCreatedCount << " grantReused=" << m_spsGrantReusedCount
                << " ndiPublished=" << m_spsNdiPublishCount << " harqAlloc=" << m_harqAllocateCount
                << " harqDealloc=" << m_harqDeallocateCount
                << " harqTimeout=" << m_harqTimeoutCount);

    if (m_numPackets >= 50)
    {
        // Normal flow: enough packets for complete SPS cycles
        if (m_slProbResourceKeep == 0.8)
        {
            // resource keep scenario; resources should be kept sometimes if the probability is 0.8
            NS_TEST_EXPECT_MSG_GT(m_spsGrantReusedCount,
                                  0,
                                  "Expected at least one grant to be reused (kept)");
            // When resources are kept, the HARQ ID is renewed, not deallocated.
            // There should be fewer deallocations than total grant cycles.
            NS_TEST_EXPECT_MSG_GT(m_spsNdiPublishCount,
                                  m_harqDeallocateCount,
                                  "Expected more NDI publications than HARQ deallocations "
                                  "(keep cycles should not deallocate)");
        }
        else
        {
            // reselect scenario; resources should never be kept
            NS_TEST_EXPECT_MSG_EQ(m_spsGrantReusedCount, 0, "Expected no grants to be reused");
            NS_TEST_EXPECT_MSG_GT(m_spsGrantCreatedCount,
                                  1,
                                  "Expected more than one grant creation (reselections)");
            // Every old grant's HARQ ID should eventually be deallocated
            // (via a shortened timer after reselection)
            NS_TEST_EXPECT_MSG_GT(m_harqDeallocateCount,
                                  0,
                                  "Expected HARQ deallocations from reselection cycles");
        }
    }
    else
    {
        // Unused SPS grant opportunities: fewer packets than reselection counter
        if (m_slProbResourceKeep == 0.8)
        {
            // resources should never be kept despite high probKeep,
            // because the grant was not fully used
            NS_TEST_EXPECT_MSG_EQ(m_spsGrantReusedCount,
                                  0,
                                  "Expected no grants to be reused when packets < counter");
        }
        // grant should be erased, HARQ ID freed
        NS_TEST_EXPECT_MSG_GT(m_harqDeallocateCount,
                              0,
                              "Expected HARQ deallocation after premature grant exhaustion");
        NS_TEST_EXPECT_MSG_LT(m_spsNdiPublishCount,
                              m_numPackets + 1,
                              "Expected NDI publications bounded by packet count");
    }

    Simulator::Destroy();
}

/**
 * @ingroup tests
 *
 * Test case to detect incorrect HARQ process ID deallocation caused by
 * RenewHarqProcessIdTimer() being called on an ID that has already been freed
 * and reallocated to a different grant.
 *
 * This test uses two sidelink nodes and instantiates two SPS flows to different
 * destination L2 IDs.  Unicast is used for one flow and groupcast for the
 * other flow, to separate the handling of the LCs at the scheduler.
 * Flow A (dstL2Id=2) sends a burst of exactly N packets, where N matches the
 * reselection counter drawn from the fixed RNG (seed=1, run=1), so that all
 * N NDI slots are published with data and the counter decrements to 0
 * naturally.  After the last slot, the LC becomes empty; when
 * TxResourceReselectionCheck is called again, it returns early (LC empty),
 * leaving the grant lingering in m_grantInfo with counter==0 and
 * pendingReselection==true.  While the LC remains empty the HARQ timer for
 * Flow A's ID expires naturally, returning that ID to the free pool.
 * Flow B (dstL2Id=240) is continuous and will eventually reselect and
 * acquire Flow A's former HARQ process ID.
 *
 * With RngSeed and RngRun both fixed to 1, the reselection counter is
 * deterministic.  Flow A's HARQ timer expires at t=3.9 s (as confirmed by logs);
 * Flow B reselects (counter=15) at t=3.9 s, and reselects again at t=5.4 s,
 * acquiring Flow A's former HARQ ID.  Flow A's second burst is started at
 * a fixed time of t=5.405 s (5 ms after the process ID assignment event).
 * At that point Flow A's stale grant is still in memory (counter==0,
 * pendingReselection==true) but Flow B's timer is live (just allocated).
 *
 * Because the OnOff application operates at 3200 bps with 40-byte packets
 * (one packet per 100 ms), the first BSR from Flow A's second burst reaches
 * the scheduler at t=5.505 s, approximately 105 ms after Flow B allocated
 * HARQ ID 1.  TxResourceReselectionCheck then triggers the pendingReselection
 * path for Flow A's stale grant.  Unless the call to RenewHarqProcessIdTimer()
 * is suppressed, the timer renewal will affect Flow B's HARQ process, and will
 * force the incorrect deallocation of flow B's process.  Later, a bug can arise if
 * the NrSlUeMac attempts to access this HARQ process ID.
 *
 * To prevent this behavior, the scheduler should check if Flow A's
 * lastGrantedSlotSfn is in the past before renewing the HARQ process ID timer.
 *
 * This test creates the conditions for this situation to arise in the scheduler,
 * and will assert that no SPS HARQ ID is freed less than 200 ms after its
 * most recent allocation.  The minimum possible SPS timer is
 * rri * (counter_min + 1) = 100 * (5+1) = 600 ms, so any sub-200 ms
 * lifetime unambiguously indicates that RenewHarqProcessIdTimer() was
 * called on an ID belonging to a different grant. However, this test may
 * crash in the model code before reaching this test assert, so this test
 * assert is just a backstop to notify a problem if the model code silently
 * allows this sequence of events to pass.
 *
 * Note: the two flows must use different dstL2Id values so the scheduler
 * creates separate SPS grants for each.  If both share dstL2Id=2 they end up
 * in the same LCG and the same combined grant, which prevents the stale-grant
 * scenario from arising.  Another solution would be to introduce two unicast
 * flows to different nodes, requiring adding and configuring another node.
 */
class NrSlSchedulerHarqIdReuseTestCase : public NrSlSchedulerTestCase
{
  public:
    /**
     * Constructor.
     *
     * @param name Test case name.
     */
    NrSlSchedulerHarqIdReuseTestCase(const std::string& name);

  private:
    void DoRun() override;

    /**
     * Callback for HARQ Allocate trace.
     *
     * Records the allocation time of every SPS HARQ ID.  Also records
     * the first time Flow B (dstL2Id=240) acquires Flow A's original HARQ ID,
     * which is used post-run to assert that the scenario was actually exercised.
     *
     * @param harqId The allocated HARQ process ID.
     * @param dstL2Id The destination L2 ID.
     * @param multiplePdu Whether this is a multiple-PDU (SPS) allocation.
     * @param timeout The HARQ timer timeout value.
     * @param available Number of HARQ IDs still available.
     */
    void NotifyHarqAllocate(uint8_t harqId,
                            uint32_t dstL2Id,
                            bool multiplePdu,
                            Time timeout,
                            std::size_t available);

    /**
     * Callback for HARQ Timeout trace.
     *
     * @param harqId The HARQ process ID that timed out.
     */
    void NotifyHarqTimeout(uint8_t harqId);

    std::map<uint8_t, Time> m_harqAllocateTime; //!< Time of most recent SPS allocation per ID
    bool m_prematureTimeoutDetected{false};     //!< True if a premature timeout was observed
    uint8_t m_flowAOriginalHarqId{255};         //!< HARQ ID first assigned to Flow A (dstL2Id=2)
    bool m_flowAResumeScheduled{false};         //!< True once Flow B acquired Flow A's former ID
};

NrSlSchedulerHarqIdReuseTestCase::NrSlSchedulerHarqIdReuseTestCase(const std::string& name)
    : NrSlSchedulerTestCase(name)
{
    // Minimal HARQ pool: 2 total, 2 available for SPS -- forces ID reuse
    m_maxSlHarqProcesses = 2;
    m_maxSlHarqProcessesMultiplePdu = 2;
}

void
NrSlSchedulerHarqIdReuseTestCase::NotifyHarqAllocate(uint8_t harqId,
                                                     uint32_t dstL2Id,
                                                     bool multiplePdu,
                                                     [[maybe_unused]] Time timeout,
                                                     [[maybe_unused]] std::size_t available)
{
    if (!multiplePdu)
    {
        return;
    }
    m_harqAllocateTime[harqId] = Simulator::Now();
    NS_LOG_INFO("HARQ SPS allocate ID " << +harqId << " dstL2Id " << dstL2Id << " at "
                                        << Simulator::Now().As(Time::S));

    // Record the HARQ ID that the scheduler first assigns to Flow A (dstL2Id=2).
    if (dstL2Id == 2 && m_flowAOriginalHarqId == 255)
    {
        m_flowAOriginalHarqId = harqId;
        NS_LOG_INFO("Flow A original HARQ ID recorded as " << +harqId);
    }

    // Record the first time Flow B (dstL2Id=240) acquires Flow A's original HARQ
    // ID after the initial setup phase (Now() > 3.5 s).  This is used post-run
    // to assert that the scenario was actually exercised.
    if (dstL2Id == 240 && harqId == m_flowAOriginalHarqId && m_flowAOriginalHarqId != 255 &&
        !m_flowAResumeScheduled && Simulator::Now() > Seconds(3.5))
    {
        m_flowAResumeScheduled = true;
        NS_LOG_INFO("Flow B took Flow A's former ID " << +harqId << " at "
                                                      << Simulator::Now().As(Time::S));
    }
}

void
NrSlSchedulerHarqIdReuseTestCase::NotifyHarqTimeout(uint8_t harqId)
{
    auto it = m_harqAllocateTime.find(harqId);
    if (it != m_harqAllocateTime.end())
    {
        Time elapsed = Simulator::Now() - it->second;
        NS_LOG_INFO("HARQ timeout ID " << +harqId << " elapsed " << elapsed.As(Time::MS));
        // An SPS HARQ timer is always initialized to at least rri * (counter_min + 1).
        // For rri=100 ms and counter drawn from [5,15], the minimum possible timer is
        // 100 * (5+1) = 600 ms.  A timeout sooner than 200 ms therefore indicates that
        // RenewHarqProcessIdTimer() was called on an ID belonging to a different grant.
        if (elapsed < MilliSeconds(200))
        {
            NS_LOG_WARN(
                "Premature HARQ timeout: ID "
                << +harqId << " freed only " << elapsed.As(Time::MS)
                << " after last SPS allocation (expected >= 600 ms = rri * (counter_min + 1))");
            m_prematureTimeoutDetected = true;
        }
    }
}

void
NrSlSchedulerHarqIdReuseTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    Time simStop(Seconds(9));
    uint32_t udpPacketSize(40);
    uint16_t udpPortA(8000);
    uint16_t udpPortB(8001);
    Time packetInterval(MilliSeconds(100));

    // Flow A: SPS to dstL2Id=2, port 8000.  Sends a burst of 7 packets, then stops.
    // 7 matches the reselection counter drawn by the fixed RNG (seed=1, run=1).
    // Sending exactly counter packets ensures that every NDI slot is published with
    // data so the counter reaches 0 naturally; the LC is then empty and
    // TxResourceReselectionCheck leaves the stale grant (counter==0,
    // pendingReselection==true, LC empty).  Flow A resumes at t=5.405 s after
    // Flow B has acquired Flow A's former HARQ ID (see second burst below).
    SidelinkInfo slInfoA;
    slInfoA.m_harqEnabled = false;
    slInfoA.m_pdb = MilliSeconds(100);
    slInfoA.m_dynamic = false;
    slInfoA.m_dstL2Id = 2;
    slInfoA.m_castType = SidelinkInfo::CastType::Unicast;
    slInfoA.m_priority = 1;
    slInfoA.m_rri = MilliSeconds(100);

    // Flow B: groupcast SPS to dstL2Id=240, port 8001.  Sends continuously.
    // Using a distinct dstL2Id forces the scheduler to create a separate SPS
    // grant for Flow B.  If both flows share dstL2Id=2 they end up in the
    // same LCG and the same combined grant, which prevents the stale-grant
    // scenario from arising.
    SidelinkInfo slInfoB;
    slInfoB.m_harqEnabled = false;
    slInfoB.m_pdb = MilliSeconds(100);
    slInfoB.m_dynamic = false;
    slInfoB.m_dstL2Id = 240;
    slInfoB.m_castType = SidelinkInfo::CastType::Groupcast;
    slInfoB.m_priority = 1;
    slInfoB.m_rri = MilliSeconds(100);

    Ipv4Address destination("7.0.0.2");

    // Activate bearers for both flows on both sender and receiver
    auto txTftA = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPortA, slInfoA);
    auto rxTftA = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPortA, slInfoA);
    auto txTftB = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, udpPortB, slInfoB);
    auto rxTftB = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, udpPortB, slInfoB);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_senderDevice), txTftA);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_receiverDevice), rxTftA);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_senderDevice), txTftB);
    m_nrSlHelper->ActivateNrSlBearer(Seconds(2), NetDeviceContainer(m_receiverDevice), rxTftB);

    DataRate dataRate{static_cast<uint64_t>(udpPacketSize * 8 / packetInterval.GetSeconds())};
    Address remoteAddressA = InetSocketAddress(destination, udpPortA);
    Address remoteAddressB = InetSocketAddress(destination, udpPortB);

    // Flow A first burst: 7 packets starting at t=3 s (see comment above)
    OnOffHelper clientA1("ns3::UdpSocketFactory", remoteAddressA);
    clientA1.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    clientA1.SetAttribute("MaxBytes", UintegerValue(7 * udpPacketSize));
    clientA1.SetConstantRate(dataRate, udpPacketSize);
    ApplicationContainer appsA1 = clientA1.Install(m_senderDevice->GetNode());
    appsA1.Start(Seconds(3.0));
    appsA1.Stop(Seconds(3.8)); // safety stop (7th packet appears in LC at t=3.7 s)

    // Flow A second burst: fixed start at t=5.405 s.
    // With RNG seed=1/run=1 the scheduler draws counter=7 for both flows.  Flow A's
    // HARQ timer expires at t=3.9 s; Flow B reselects (counter=15) at t=3.9 s, timer
    // expires at t=5.4 s, at which point Flow B reselects again and acquires Flow A's
    // former HARQ ID.  Starting Flow A's second burst 5 ms after that (t=5.405 s)
    // ensures data is present in the LC when TxResourceReselectionCheck fires for the
    // stale grant, creating conditions to check for incorrect RenewHarqProcessIdTimer().
    OnOffHelper clientA2("ns3::UdpSocketFactory", remoteAddressA);
    clientA2.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    clientA2.SetConstantRate(dataRate, udpPacketSize);
    ApplicationContainer appsA2 = clientA2.Install(m_senderDevice->GetNode());
    appsA2.Start(Seconds(5.405));
    appsA2.Stop(simStop);

    // Flow B: continuous sending from t=3 s
    OnOffHelper clientB("ns3::UdpSocketFactory", remoteAddressB);
    clientB.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    clientB.SetConstantRate(dataRate, udpPacketSize);
    ApplicationContainer appsB = clientB.Install(m_senderDevice->GetNode());
    appsB.Start(Seconds(3.0));
    appsB.Stop(simStop);

    // Connect HARQ traces on the sender MAC
    auto senderMac = m_senderDevice->GetMac(0)->GetObject<NrSlUeMac>();
    PointerValue harqPtrValue;
    senderMac->GetAttribute("NrSlUeMacHarq", harqPtrValue);
    auto harq = harqPtrValue.GetObject()->GetObject<NrSlUeMacHarq>();
    NS_ASSERT_MSG(harq, "Failed to get NrSlUeMacHarq from sender MAC");
    harq->TraceConnectWithoutContext(
        "Allocate",
        MakeCallback(&NrSlSchedulerHarqIdReuseTestCase::NotifyHarqAllocate, this));
    harq->TraceConnectWithoutContext(
        "Timeout",
        MakeCallback(&NrSlSchedulerHarqIdReuseTestCase::NotifyHarqTimeout, this));

    Simulator::Stop(simStop);
    Simulator::Run();

    // Verify that the scenario was actually exercised.  If Flow B never acquired
    // Flow A's former HARQ ID before simStop, the test would pass.
    // Note that the simulator may crash (abort) in the model code before reaching here.
    NS_TEST_EXPECT_MSG_EQ(m_flowAResumeScheduled,
                          true,
                          "Test scenario was not exercised: Flow B never acquired Flow A's "
                          "former HARQ ID before simulation end.  Check RNG seed/run, simStop, "
                          "and that dstL2Id values are distinct and bearers are activated.");

    NS_TEST_EXPECT_MSG_EQ(m_prematureTimeoutDetected,
                          false,
                          "Premature HARQ timeout detected: RenewHarqProcessIdTimer() was "
                          "called on a HARQ ID owned by a different grant, shortening its "
                          "timer and causing the process to be freed while still in use");

    Simulator::Destroy();
}

class NrSlSchedulerTestSuite : public TestSuite
{
  public:
    NrSlSchedulerTestSuite()
        : TestSuite("nr-sl-scheduler", Type::SYSTEM)
    {
        AddTestCase(
            new NrSlSchedulerMcsConfigTestCase("Check ability to configure MCS during runtime"),
            Duration::QUICK);
        AddTestCase(
            new NrSlSchedulerMultipleTbTestCase("Check multiple TB scheduling for large packets"),
            Duration::QUICK);
        AddTestCase(new NrSlSchedulerNumTbCalculationTestCase("Check number of TBs calculation"),
                    Duration::QUICK);
        AddTestCase(new NrSlSchedulerHarqExhaustionTestCase("Check HARQ exhaustion handling"),
                    Duration::QUICK);
        AddTestCase(new NrSlSchedulerSpsTestCase("Check SPS baseline with 100 periodic packets"),
                    Duration::QUICK);
        AddTestCase(new NrSlSchedulerSpsJitterTestCase(
                        "Check SPS jitter with small packets (supplemental needed)",
                        20,
                        true),
                    Duration::QUICK);
        AddTestCase(new NrSlSchedulerSpsJitterTestCase(
                        "Check SPS jitter with large packets (supplemental needed)",
                        130,
                        true),
                    Duration::QUICK);
        AddTestCase(new NrSlSchedulerDynamicTbExpansionTestCase(
                        "Check dynamic grant TB expansion at MCS 14"),
                    Duration::QUICK);
        AddTestCase(new NrSlSchedulerSpsJitterTestCase(
                        "Check SPS jitter at MCS 14 with small packets (no supplemental)",
                        20,
                        false,
                        14),
                    Duration::QUICK);
        // TB size at MCS 14 with 1 subchannel is 309 bytes, but leave 2 bytes overhead for 307.
        // Each packet incurs 33 bytes overhead, so need 2 * (packetSize + 33) > 307 to
        // trigger a supplemental grant.  The minimum size (used below) is 121 bytes.
        AddTestCase(new NrSlSchedulerSpsJitterTestCase(
                        "Check SPS jitter at MCS 14 with large packets (supplemental needed)",
                        121,
                        true,
                        14),
                    Duration::QUICK);
        AddTestCase(new NrSlSchedulerRlcSegmentationTestCase("Check RLC segmentation stray bytes"),
                    Duration::QUICK);
        AddTestCase(new NrSlSchedulerSpsReselectionTestCase(
                        "Check SPS reselection with SlProbResourceKeep = 0 (always reselect)",
                        0.0),
                    Duration::QUICK);
        AddTestCase(new NrSlSchedulerSpsReselectionTestCase(
                        "Check SPS reselection with SlProbResourceKeep = 0.8 (mostly keep)",
                        0.8),
                    Duration::QUICK);

        // HARQ process ID (lifecycle) test cases

        // Keep resources, HARQ ID persists across keep cycles
        AddTestCase(
            new NrSlSchedulerHarqLifecycleTestCase("Keep resources (no retx)", 0.8, false, 1, 50),
            Duration::QUICK);
        // Reselect, HARQ disabled, no retransmissions
        AddTestCase(new NrSlSchedulerHarqLifecycleTestCase("Reselect resources (no HARQ, no retx)",
                                                           0.0,
                                                           false,
                                                           1,
                                                           50),
                    Duration::QUICK);
        // Reselect, HARQ disabled, retransmissions enabled
        AddTestCase(new NrSlSchedulerHarqLifecycleTestCase("Reselect resources (no HARQ, retx)",
                                                           0.0,
                                                           false,
                                                           3,
                                                           50),
                    Duration::QUICK);
        // Reselect, HARQ enabled, no retransmissions
        AddTestCase(new NrSlSchedulerHarqLifecycleTestCase("Reselect resources (HARQ, no retx)",
                                                           0.0,
                                                           true,
                                                           1,
                                                           50),
                    Duration::QUICK);
        // Reselect, HARQ enabled, retransmissions enabled
        AddTestCase(new NrSlSchedulerHarqLifecycleTestCase("Reselect resources (HARQ, retx)",
                                                           0.0,
                                                           true,
                                                           3,
                                                           50),
                    Duration::QUICK);
        // SPS grant opportunities unused, high probKeep, no HARQ, no retx
        AddTestCase(
            new NrSlSchedulerHarqLifecycleTestCase("Premature exhaustion (no HARQ, no retx)",
                                                   0.8,
                                                   false,
                                                   1,
                                                   3),
            Duration::QUICK);
        // SPS grant opportunities unused, high probKeep, no HARQ, retx
        AddTestCase(new NrSlSchedulerHarqLifecycleTestCase("Premature exhaustion (no HARQ, retx)",
                                                           0.8,
                                                           false,
                                                           3,
                                                           3),
                    Duration::QUICK);
        // SPS grant opportunities unused, high probKeep, HARQ, no retx
        AddTestCase(new NrSlSchedulerHarqLifecycleTestCase("Premature exhaustion (HARQ, no retx)",
                                                           0.8,
                                                           true,
                                                           1,
                                                           3),
                    Duration::QUICK);
        // SPS grant opportunities unused, high probKeep, HARQ, retx
        AddTestCase(new NrSlSchedulerHarqLifecycleTestCase("Premature exhaustion (HARQ, retx)",
                                                           0.8,
                                                           true,
                                                           3,
                                                           3),
                    Duration::QUICK);
        // SPS grant opportunities unused, no data at last slot
        AddTestCase(
            new NrSlSchedulerHarqLifecycleTestCase("No data at last slot", 0.0, false, 1, 3),
            Duration::QUICK);

        // HARQ ID reuse: stale SPS grant corrupts recycled ID timer
        AddTestCase(new NrSlSchedulerHarqIdReuseTestCase(
                        "Check HARQ ID reuse does not corrupt recycled process timer"),
                    Duration::QUICK);
    }
};

static NrSlSchedulerTestSuite nrSlSchedulerTestSuite; //!< NR SL scheduler test suite
