/*
 * SPDX-License-Identifier: NIST-Software
 */

#include "ns3/application-container.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device-container.h"
#include "ns3/nhdp-client.h"
#include "ns3/nhdp-helper.h"
#include "ns3/nhdp-info-base.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/olsr-header.h"
#include "ns3/olsr-helper.h"
#include "ns3/olsrv2-header.h"
#include "ns3/olsrv2-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/onoff-application.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/position-allocator.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/simulator.h"
#include "ns3/snr-tag.h"
#include "ns3/ssid.h"
#include "ns3/string.h"
#include "ns3/test.h"
#include "ns3/vector.h"
#include "ns3/waypoint-mobility-model.h"
#include "ns3/wifi-helper.h"
#include "ns3/wifi-mac-helper.h"
#include "ns3/wifi-utils.h"
#include "ns3/yans-wifi-helper.h"

#include <map>
#include <vector>

using namespace ns3;
using namespace nhdp;

NS_LOG_COMPONENT_DEFINE("NhdpSystem");

/**
 * @defgroup nhdp-tests Tests for nhdp
 * @ingroup nhdp
 * @ingroup tests
 */

/**
 * @ingroup nhdp-tests
 *
 * Base class for common functions like trace sinks and counters
 */
class NhdpTestCase : public TestCase
{
  public:
    NhdpTestCase(std::string name);

    void Disable(Ptr<Node> a, Ptr<Node> b);
    void Enable(Ptr<Node> a, Ptr<Node> b);
    void Change(Ptr<Node> a, Ptr<Node> b, double lossDb);

    void NeighborChange(std::string context,
                        NeighborStatus neighborStatus,
                        const NeighborTuple& neighborTuple);
    void LinkChange(std::string context, LinkStatus oldLinkStatus, const LinkTuple& linkTuple);
    void TwoHopChange(std::string context, TwoHopStatus twoHopStatus, const TwoHopTuple& linkTuple);
    void ClearNeighborChanges();
    void CheckNeighbor(const NeighborTuple& tuple);
    void CheckNeighborSize(std::size_t expectedSize);

    void ClearLinkChanges();
    void CheckLink(const LinkTuple& tuple);
    void CheckLinkSize(std::size_t expectedSize);

    void CheckTwoHopSize(std::size_t expectedSize);

    void OlsrRoutingTableChange(std::string context, uint32_t tableSize);
    void OlsrTx(const olsr::PacketHeader& header, const olsr::MessageList& messages);
    void OlsrRx(const olsr::PacketHeader& header, const olsr::MessageList& messages);
    void Olsrv2Tx(const olsrv2::PacketHeader& header, const olsrv2::MessageList& messages);
    void Olsrv2Rx(const olsrv2::PacketHeader& header, const olsrv2::MessageList& messages);
    /**
     * Tests don't usually print but this can be useful when debugging the tests
     * @param client The NhdpClient to print
     */
    void Print(Ptr<NhdpClient> client);

  protected:
    std::map<Ipv4Address, NeighborTuple> m_neighborChanges;
    std::map<Ipv4Address, LinkTuple> m_linkChanges;
    std::map<Ipv4Address, TwoHopTuple> m_twoHopChanges;
    std::map<Ipv4Address, NeighborTuple> m_symmetricNeighbors;
    uint32_t m_txPacketsOlsrTrace;
    uint32_t m_txPacketsOlsrBytesTotal;
    uint32_t m_rxPacketsOlsrTrace;

    Ptr<MatrixPropagationLossModel> m_matrixLossModel;
    NodeContainer m_nodes;
    NetDeviceContainer m_devices;
};

NhdpTestCase::NhdpTestCase(std::string name)
    : TestCase(name)
{
}

void
NhdpTestCase::Disable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Disabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               1000,
                               true);
}

void
NhdpTestCase::Enable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Enabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               0,
                               true);
}

void
NhdpTestCase::Change(Ptr<Node> a, Ptr<Node> b, double lossDb)
{
    NS_LOG_INFO("Changing loss to " << lossDb << " on link between " << a->GetId() << " and "
                                    << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               lossDb,
                               true);
}

void
NhdpTestCase::Print(Ptr<NhdpClient> client)
{
    for (const auto& [addr, tuple] : client->GetNeighborInfoBase())
    {
        NS_LOG_INFO(client->GetNode()->GetId()
                    << " Neighbor tuple " << addr << " symmetric " << tuple.m_symmetric);
    }
    for (const auto& [addr, tuple] : client->GetLinkInfoBase())
    {
        NS_LOG_INFO(client->GetNode()->GetId()
                    << " Link tuple " << addr << " status " << tuple.GetLinkStatus() << " lost "
                    << tuple.m_lost << " expiration time " << tuple.m_expirationTime.As(Time::S));
    }
    for (const auto& [key, tuple] : client->GetTwoHopInfoBase())
    {
        NS_LOG_INFO(client->GetNode()->GetId()
                    << " Two hop neighbor " << tuple.m_twoHopAddr << " via " << key.first
                    << " expiration time " << tuple.m_expirationTime.As(Time::S));
    }
}

void
NhdpTestCase::NeighborChange(std::string context,
                             NeighborStatus neighborStatus,
                             const NeighborTuple& neighborTuple)
{
    if (neighborStatus == NeighborStatus::NEW)
    {
        NS_LOG_INFO(context << " New neighbor " << neighborTuple.m_neighborAddrList[0]);
    }
    else if (neighborStatus == NeighborStatus::MODIFIED && neighborTuple.m_symmetric)
    {
        NS_LOG_INFO(context << " Symmetric neighbor " << neighborTuple.m_neighborAddrList[0]);
        m_symmetricNeighbors.insert_or_assign(neighborTuple.m_neighborAddrList[0], neighborTuple);
    }
    else if (neighborStatus == NeighborStatus::REMOVED)
    {
        NS_LOG_INFO(context << " Removed neighbor " << neighborTuple.m_neighborAddrList[0]);
    }
    m_neighborChanges.insert_or_assign(neighborTuple.m_neighborAddrList[0], neighborTuple);
}

void
NhdpTestCase::CheckNeighbor(const NeighborTuple& checkTuple)
{
    NS_LOG_INFO("Checking tuple " << checkTuple.m_neighborAddrList[0]);
    bool found{false};
    for (auto& [neighborAddr, neighborTuple] : m_neighborChanges)
    {
        if (neighborAddr == checkTuple.m_neighborAddrList[0] &&
            neighborTuple.m_symmetric == checkTuple.m_symmetric)
        {
            found = true;
            break;
        }
    }
    NS_TEST_ASSERT_MSG_EQ(found, true, "CheckNeighbor() failed");
}

void
NhdpTestCase::CheckNeighborSize(std::size_t expectedSize)
{
    NS_TEST_ASSERT_MSG_EQ(m_neighborChanges.size(), expectedSize, "CheckNeighborSize() failed");
}

void
NhdpTestCase::ClearNeighborChanges()
{
    m_neighborChanges.clear();
}

void
NhdpTestCase::CheckLink(const LinkTuple& checkTuple)
{
    NS_LOG_INFO("Checking link tuple " << checkTuple.m_neighborAddrList[0]);
    bool found{false};
    // Allow some tolerance in the time values
    Time tolerance{MilliSeconds(1)};
    Time checkHeardTime = checkTuple.m_heardTime;
    Time checkHeardTimeMin = (checkHeardTime >= tolerance) ? (checkHeardTime - tolerance) : Seconds(0);
    Time checkHeardTimeMax = (checkHeardTime + tolerance);
    Time checkSymTime = checkTuple.m_symTime;
    Time checkSymTimeMin = (checkSymTime >= tolerance) ? (checkSymTime - tolerance) : Seconds(0);
    Time checkSymTimeMax = (checkSymTime + tolerance);
    Time checkLTime = checkTuple.m_expirationTime;
    Time checkLTimeMin = (checkLTime >= tolerance) ? (checkLTime - tolerance) : Seconds(0);
    Time checkLTimeMax = (checkLTime + tolerance);
    for (auto& [neighborAddr, linkTuple] : m_linkChanges)
    {
        if (neighborAddr == checkTuple.m_neighborAddrList[0] &&
            linkTuple.m_heardTime >= checkHeardTimeMin &&
            linkTuple.m_heardTime <= checkHeardTimeMax &&
            linkTuple.m_symTime >= checkSymTimeMin &&
            linkTuple.m_symTime <= checkSymTimeMax &&
            linkTuple.m_quality == checkTuple.m_quality &&
            linkTuple.m_pending == checkTuple.m_pending &&
            linkTuple.m_lost == checkTuple.m_lost &&
            linkTuple.m_expirationTime >= checkLTimeMin &&
            linkTuple.m_expirationTime <= checkLTimeMax)
        {
            found = true;
            break;
        }
    }
    NS_TEST_ASSERT_MSG_EQ(found, true, "CheckLink() failed");
}

void
NhdpTestCase::CheckLinkSize(std::size_t expectedSize)
{
    NS_TEST_ASSERT_MSG_EQ(m_linkChanges.size(), expectedSize, "CheckLinkSize() failed");
}

void
NhdpTestCase::ClearLinkChanges()
{
    m_linkChanges.clear();
}

void
NhdpTestCase::CheckTwoHopSize(std::size_t expectedSize)
{
    NS_TEST_ASSERT_MSG_EQ(m_twoHopChanges.size(), expectedSize, "CheckTwoHopSize() failed");
}

void
NhdpTestCase::LinkChange(std::string context, LinkStatus oldLinkStatus, const LinkTuple& linkTuple)
{
    NS_LOG_INFO(context << " Link old status " << oldLinkStatus << " new status "
                        << linkTuple.GetLinkStatus() << " " << linkTuple.m_neighborAddrList[0]
                        << " heard " << linkTuple.m_heardTime.As(Time::S) << " sym "
                        << linkTuple.m_symTime.As(Time::S) << " quality " << linkTuple.m_quality
                        << " pending " << linkTuple.m_pending << " lost " << linkTuple.m_lost
                        << " expiration " << linkTuple.m_expirationTime.As(Time::S));
    m_linkChanges.insert_or_assign(linkTuple.m_neighborAddrList[0], linkTuple);
}

void
NhdpTestCase::TwoHopChange(std::string context,
                           TwoHopStatus twoHopStatus,
                           const TwoHopTuple& twoHopTuple)
{
    if (twoHopStatus == TwoHopStatus::NEW)
    {
        NS_LOG_INFO(context << " New two-hop neighbor " << twoHopTuple.m_twoHopAddr << " from "
                            << twoHopTuple.m_neighborAddrList[0] << " status " << twoHopStatus);
    }
    else
    {
        NS_LOG_INFO(context << " Existing two-hop neighbor " << twoHopTuple.m_twoHopAddr << " from "
                            << twoHopTuple.m_neighborAddrList[0] << " status " << twoHopStatus);
    }
    m_twoHopChanges.insert_or_assign(twoHopTuple.m_neighborAddrList[0], twoHopTuple);
}

void
NhdpTestCase::OlsrRoutingTableChange(std::string context, uint32_t tableSize)
{
    NS_LOG_INFO(context << " routing table change to " << tableSize);
}

void
NhdpTestCase::OlsrTx(const olsr::PacketHeader& header, const olsr::MessageList&)
{
    m_txPacketsOlsrTrace++;
    m_txPacketsOlsrBytesTotal += header.GetPacketLength();
}

void
NhdpTestCase::OlsrRx(const olsr::PacketHeader&, const olsr::MessageList&)
{
    m_rxPacketsOlsrTrace++;
}

void
NhdpTestCase::Olsrv2Tx(const olsrv2::PacketHeader& header, const olsrv2::MessageList&)
{
    m_txPacketsOlsrTrace++;
    m_txPacketsOlsrBytesTotal += header.GetPacketLength();
}

void
NhdpTestCase::Olsrv2Rx(const olsrv2::PacketHeader&, const olsrv2::MessageList&)
{
    m_rxPacketsOlsrTrace++;
}


/**
 * @ingroup nhdp-tests
 * Use controlled topology based on MatrixPropagationLossModel, create a two-node topology
 * in which each node can possibly hear one neighbor, as follows:
 *
 * 1 <------> 2
 */
class NhdpTwoNodeTestCase : public NhdpTestCase
{
  public:
    NhdpTwoNodeTestCase(std::string name);

    double LinkQualityCallback(Ptr<Packet> packet) const;

  protected:
    void DoSetup() override;
};

NhdpTwoNodeTestCase::NhdpTwoNodeTestCase(std::string name)
    : NhdpTestCase(name)
{
}

double
NhdpTwoNodeTestCase::LinkQualityCallback(Ptr<Packet> packet) const
{
    SnrTag snrTag;
    auto found = packet->RemovePacketTag(snrTag);
    double threshold = -1; // dB
    double range = 6;      // dB
    if (!found)
    {
        NS_LOG_DEBUG("SnrTag not found");
        return 1;
    }
    double quality = 0;
    double snrDb = RatioToDb(snrTag.Get());
    if (snrDb > threshold)
    {
        quality = std::min(1.0, (snrDb - threshold) / range);
        quality = std::max(0.0, quality);
    }
    NS_LOG_DEBUG("Link quality: " << quality << " SNR_dB: " << snrDb);
    return quality;
}

void
NhdpTwoNodeTestCase::DoSetup()
{
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

    // Create node index zero but do not use it; this allows the subsequent
    // node IDs to align with the last octet of the IP address, for help
    // in correlating IP addresses to nodes
    Ptr<Node> unusedNode [[maybe_unused]] = CreateObject<Node>();

    m_nodes.Create(2);

    // Not that important because we are using a Matrix prop. loss model
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(m_nodes);

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    WifiMacHelper wifiMac;
    YansWifiPhyHelper wifiPhy;
    wifiPhy.DisablePreambleDetectionModel();
    YansWifiChannelHelper wifiChannel;
    auto channel = CreateObject<YansWifiChannel>();
    auto delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
    m_matrixLossModel = CreateObject<MatrixPropagationLossModel>();
    channel->SetPropagationDelayModel(delayModel);
    // By default, the loss is 'infinite' and must be changed by SetLoss() commands in DoRun()
    channel->SetPropagationLossModel(m_matrixLossModel);
    wifiPhy.SetChannel(channel);
    wifiMac.SetType("ns3::AdhocWifiMac");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("HeMcs0"));
    m_devices = wifi.Install(wifiPhy, wifiMac, m_nodes);
    wifi.AssignStreams(m_devices, 100);
    // Leave Internet configuration for subclasses
}

class NhdpTwoNodeNhdpTestCase : public NhdpTwoNodeTestCase
{
  public:
    NhdpTwoNodeNhdpTestCase(std::string name);

  protected:
    void DoRun() override;
};

NhdpTwoNodeNhdpTestCase::NhdpTwoNodeNhdpTestCase(std::string name)
    : NhdpTwoNodeTestCase(name)
{
}

void
NhdpTwoNodeNhdpTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(21)};

    InternetStackHelper internet;
#if 0
    OlsrHelper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
#endif
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));
    nhdp1->AssignStreams(10);
    auto nhdp2 = apps.Get(1)->GetObject<NhdpClient>();
    nhdp2->AssignStreams(11);
#if 0
    nhdp1->RegisterLinkQualityCallback(MakeCallback(&NhdpTwoNodeTestCase::LinkQualityCallback, this));
#endif
    apps.Start(startTime);
    apps.Stop(stopTime);

    // Enable sets the matrix loss to zero dB.  The transmit power in dBm is 16.026.  The
    // noise power in 20 MHz (default 11ax channel width for group addressed frames)
    // is approximately -101 dBm.  The default Wi-Fi noise figure is 7 dB, so the expected
    // SNR if zero loss is configured is 16.026 - (-101) - 7 = 110 dB for NHDP HELLOs.
    //
    // Therefore, to set the SNR around 0 dB for NHDP frames requires around 110 dB of loss.
    // However, for data frames to reach 0 dB SNR, the loss should be set to 104 dB because
    // the channel width is 80 MHz and data will be sent in 80 MHz.
    //
    // A 110 dB loss will result in a PER of about 0.1 for NHDP hello
    // A 104 dB loss will result in a data PER of about 0.52 and a NHDP PER of zero
    //
    // In this test we are concerned with NHDP only.  Start NHDP with the channel disabled.
    //   - at 5 seconds, enable the channel  
    //   - at 6 and 8 seconds, check that HELLOs have been received and state is correct
    //   - at 10 seconds, disable the channel  
    //   - at 15 seconds, check that lost link status are being circulated
    //   - at 20 seconds, check that things are removed
    //
    Simulator::Schedule(Seconds(5),
                        &NhdpTestCase::Enable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1));

    // By time 6 seconds, we should have heard that there is a link and neighbor change;
    // the link state should be HEARD.  The Neighbor is not yet symmetric.
    NeighborTuple ntAt6s{Ipv4Address("7.0.0.2")};
    ntAt6s.m_symmetric = false;
    Simulator::Schedule(Seconds(6),
                        &NhdpTestCase::CheckNeighbor,
                        this,
                        ntAt6s);
    // at time 6, we should see heardTime of 111.9907s, symTime and expirationTime should be zero,
    // quality 1, m_pending false, m_lost false.  Some values are defaults and don't need setting
    LinkTuple ltAt6s{Ipv4Address("7.0.0.2")};
    ltAt6s.m_heardTime = Seconds(11.9907);  // arrival of HELLO at 5.9907 + 6 seconds
    ltAt6s.m_quality = 1;
    Simulator::Schedule(Seconds(6),
                        &NhdpTestCase::CheckLink,
                        this,
                        ltAt6s);
    // No 2-hop changes should be seen
    Simulator::Schedule(Seconds(6),
                        &NhdpTestCase::CheckTwoHopSize,
                        this,
                        0);

    // by time 8, the neighbor should be symmetric.  The heardTime and symTime should be now
    // 13.8889 seconds, and an expiration time of 19.8889 seconds (heard time + 6 sec).
    NeighborTuple ntAt8s{Ipv4Address("7.0.0.2")};
    ntAt8s.m_symmetric = true;
    Simulator::Schedule(Seconds(8),
                        &NhdpTestCase::CheckNeighbor,
                        this,
                        ntAt8s);
    LinkTuple ltAt8s{Ipv4Address("7.0.0.2")};
    ltAt8s.m_heardTime = Seconds(13.8889);  // arrival of last HELLO at 7.8889 + 6 seconds
    ltAt8s.m_symTime = Seconds(13.8889);
    ltAt8s.m_expirationTime = Seconds(19.8889); // heard time + 6 seconds
    ltAt8s.m_quality = 1;
    Simulator::Schedule(Seconds(8),
                        &NhdpTestCase::CheckLink,
                        this,
                        ltAt8s);

    Simulator::Schedule(Seconds(9),
                        &NhdpTestCase::ClearNeighborChanges,
                        this);

    // There should not be neighbor changes since they were cleared at time 9
    Simulator::Schedule(Seconds(10),
                        &NhdpTestCase::CheckNeighborSize,
                        this,
                        0);

    Simulator::Schedule(Seconds(10),
                        &NhdpTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1));

#if 0
    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.2"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(6));
    apps2.Stop(stopTime);

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(1));
    apps3.Start(Seconds(6));
    apps3.Stop(stopTime);
#endif

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
#if 0
    // Schedule losses
    // Simulator::Schedule(Seconds(3), &NhdpUnequalPathTestCase::Disable, this, m_nodes.Get(0),
    // m_nodes.Get(1));
    Simulator::Schedule(Seconds(19), &NhdpTestCase::Print, this, nhdp1);
    // Simulator::Schedule(Seconds(20), &NhdpUnequalPathTestCase::Enable, this, m_nodes.Get(0),
    // m_nodes.Get(1));
    Simulator::Schedule(Seconds(30), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(31),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1));
    Simulator::Schedule(Seconds(31),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(2));
    Simulator::Schedule(Seconds(40), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(41),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1));
    Simulator::Schedule(Seconds(41),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(2));
    Simulator::Schedule(Seconds(50), &NhdpUnequalPathTestCase::Print, this, nhdp1);

#endif
    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();
}

#if 0

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 */
class NhdpFourNodeTestCase : public NhdpTestCase
{
  public:
    NhdpFourNodeTestCase(std::string name);

    void Disable(Ptr<Node> a, Ptr<Node> b);
    void Enable(Ptr<Node> a, Ptr<Node> b);

  protected:
    void DoSetup() override;
    Ptr<MatrixPropagationLossModel> m_matrixLossModel;
    NodeContainer m_nodes;
    NetDeviceContainer m_devices;
};

NhdpFourNodeTestCase::NhdpFourNodeTestCase(std::string name)
    : NhdpTestCase(name)
{
}

void
NhdpFourNodeTestCase::Disable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Disabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               100,
                               true);
}

void
NhdpFourNodeTestCase::Enable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Enabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               0,
                               true);
}

void
NhdpFourNodeTestCase::DoSetup()
{
    // Create node index zero but do not use it; this allows the subsequent
    // node IDs to align with the last octet of the IP address, for help
    // in correlating IP addresses to nodes
    Ptr<Node> unusedNode [[maybe_unused]] = CreateObject<Node>();

    m_nodes.Create(4);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));
    positionAlloc->Add(Vector(0.0, 100.0, 0.0));
    positionAlloc->Add(Vector(100.0, 100.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(m_nodes);

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211a);
    WifiMacHelper wifiMac;
    YansWifiPhyHelper wifiPhy;
    wifiPhy.DisablePreambleDetectionModel();
    YansWifiChannelHelper wifiChannel;
    auto channel = CreateObject<YansWifiChannel>();
    auto delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
    m_matrixLossModel = CreateObject<MatrixPropagationLossModel>();
    channel->SetPropagationDelayModel(delayModel);
    channel->SetPropagationLossModel(m_matrixLossModel);
    // Create 100 dB loss on the diagonals so that each node has two neighbors
    m_matrixLossModel->SetDefaultLoss(0);
    m_matrixLossModel->SetLoss(m_nodes.Get(0)->GetObject<MobilityModel>(),
                               m_nodes.Get(3)->GetObject<MobilityModel>(),
                               100,
                               true);
    m_matrixLossModel->SetLoss(m_nodes.Get(1)->GetObject<MobilityModel>(),
                               m_nodes.Get(2)->GetObject<MobilityModel>(),
                               100,
                               true);
    wifiPhy.SetChannel(channel);
    wifiMac.SetType("ns3::AdhocWifiMac");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue("OfdmRate54Mbps"));
    m_devices = wifi.Install(wifiPhy, wifiMac, m_nodes);
}

/**
 * @ingroup nhdp-tests
 * Configure NHDP on four node topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class NhdpFourNodeNhdpTestCase : public NhdpFourNodeTestCase
{
  public:
    NhdpFourNodeNhdpTestCase(std::string name);

  protected:
    void DoRun() override;
};

NhdpFourNodeNhdpTestCase::NhdpFourNodeNhdpTestCase(std::string name)
    : NhdpFourNodeTestCase(name)
{
}

void
NhdpFourNodeNhdpTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(51)};

    InternetStackHelper internet;
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Schedule losses
    // Simulator::Schedule(Seconds(3), &NhdpFourNodeTestCase::Disable, this, m_nodes.Get(0),
    // m_nodes.Get(1));
    Simulator::Schedule(Seconds(19), &NhdpTestCase::Print, this, nhdp1);
    // Simulator::Schedule(Seconds(20), &NhdpFourNodeTestCase::Enable, this, m_nodes.Get(0),
    // m_nodes.Get(1));
    Simulator::Schedule(Seconds(30), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(31),
                        &NhdpFourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1));
    Simulator::Schedule(Seconds(31),
                        &NhdpFourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(2));
    Simulator::Schedule(Seconds(40), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(41),
                        &NhdpFourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1));
    Simulator::Schedule(Seconds(41),
                        &NhdpFourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(2));
    Simulator::Schedule(Seconds(50), &NhdpFourNodeTestCase::Print, this, nhdp1);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class NhdpFourNodeOlsrTestCase : public NhdpFourNodeTestCase
{
  public:
    NhdpFourNodeOlsrTestCase(std::string name);

  protected:
    void DoRun() override;
};

NhdpFourNodeOlsrTestCase::NhdpFourNodeOlsrTestCase(std::string name)
    : NhdpFourNodeTestCase(name)
{
}

void
NhdpFourNodeOlsrTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(35)};

    InternetStackHelper internet;
    OlsrHelper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Disable one path through the network before data transfer starts
    Simulator::Schedule(Seconds(5),
                        &NhdpFourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(14.99),
                        &NhdpFourNodeTestCase::Enable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(14.99),
                        &NhdpFourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(24.99),
                        &NhdpFourNodeTestCase::Enable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(24.99),
                        &NhdpFourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(34), &NhdpFourNodeTestCase::Print, this, nhdp1);

    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.4"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(10));
    apps2.Stop(stopTime);

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(3));
    apps3.Start(Seconds(10));
    apps3.Stop(stopTime);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class NhdpFourNodeOlsrv2TestCase : public NhdpFourNodeTestCase
{
  public:
    NhdpFourNodeOlsrv2TestCase(std::string name);

  protected:
    void DoRun() override;
};

NhdpFourNodeOlsrv2TestCase::NhdpFourNodeOlsrv2TestCase(std::string name)
    : NhdpFourNodeTestCase(name)
{
}

void
NhdpFourNodeOlsrv2TestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(35)};

    InternetStackHelper internet;
    Olsrv2Helper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Disable one path through the network before data transfer starts
    Simulator::Schedule(Seconds(5),
                        &NhdpFourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(14.99),
                        &NhdpFourNodeTestCase::Enable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(14.99),
                        &NhdpFourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(24.99),
                        &NhdpFourNodeTestCase::Enable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(24.99),
                        &NhdpFourNodeTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(34), &NhdpFourNodeTestCase::Print, this, nhdp1);

    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.4"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(10));
    apps2.Stop(stopTime);

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(3));
    apps3.Start(Seconds(10));
    apps3.Stop(stopTime);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a five-node topology with two unequal hop count paths
 *
 * 3<-->4 <-> 5
 * |          |
 * |          |
 * 1 <------> 2
 */
class NhdpUnequalPathTestCase : public NhdpTestCase
{
  public:
    NhdpUnequalPathTestCase(std::string name);

    double LinkQualityCallback(Ptr<Packet> packet) const;
    void Disable(Ptr<Node> a, Ptr<Node> b);
    void Enable(Ptr<Node> a, Ptr<Node> b);
    void Change(Ptr<Node> a, Ptr<Node> b, double lossDb);

  protected:
    void DoSetup() override;

    Ptr<MatrixPropagationLossModel> m_matrixLossModel;
    NodeContainer m_nodes;
    NetDeviceContainer m_devices;
    std::vector<NeighborTuple> m_neighborTupleVector;
};

NhdpUnequalPathTestCase::NhdpUnequalPathTestCase(std::string name)
    : NhdpTestCase(name)
{
}

double
NhdpUnequalPathTestCase::LinkQualityCallback(Ptr<Packet> packet) const
{
    SnrTag snrTag;
    auto found = packet->RemovePacketTag(snrTag);
    if (!found)
    {
        NS_LOG_DEBUG("SnrTag not found");
        return 1;
    }
    double snrDb = RatioToDb(snrTag.Get());
    NS_LOG_INFO("SnrTag " << snrDb);
    double quality = 0;
    if (snrDb > 1)
    {
        quality = 1;
    }
    else if (snrDb > 0.5)
    {
        quality = 0.7;
    }
    else if (snrDb > 0)
    {
        quality = 0.5;
    }
    return quality;

//    double threshold = -1; // dB
//    double range = 6; // dB
//    if (snrDb > threshold)
//    {
//       quality = std::min(1.0, (snrDb - threshold)/range);
//       quality = std::max(0.0, quality);
//    }
//    NS_LOG_DEBUG("Link quality: " << quality << " SNR_dB: " << snrDb);
//    return quality;
}

void
NhdpUnequalPathTestCase::Disable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Disabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               100,
                               true);
}

void
NhdpUnequalPathTestCase::Enable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Enabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               0,
                               true);
}

void
NhdpUnequalPathTestCase::Change(Ptr<Node> a, Ptr<Node> b, double lossDb)
{
    NS_LOG_INFO("Changing loss to " << lossDb << " on link between " << a->GetId() << " and "
                                    << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               lossDb,
                               true);
}

void
NhdpUnequalPathTestCase::DoSetup()
{
    // Create node index zero but do not use it; this allows the subsequent
    // node IDs to align with the last octet of the IP address, for help
    // in correlating IP addresses to nodes
    Ptr<Node> unusedNode [[maybe_unused]] = CreateObject<Node>();

    m_nodes.Create(5);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));
    positionAlloc->Add(Vector(0.0, 100.0, 0.0));
    positionAlloc->Add(Vector(50.0, 100.0, 0.0));
    positionAlloc->Add(Vector(100.0, 100.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(m_nodes);

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    WifiMacHelper wifiMac;
    YansWifiPhyHelper wifiPhy;
    wifiPhy.DisablePreambleDetectionModel();
    YansWifiChannelHelper wifiChannel;
    auto channel = CreateObject<YansWifiChannel>();
    auto delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
    auto friisModel = CreateObject<FriisPropagationLossModel>();
    m_matrixLossModel = CreateObject<MatrixPropagationLossModel>();
    friisModel->SetNext(m_matrixLossModel);
    channel->SetPropagationDelayModel(delayModel);
    channel->SetPropagationLossModel(friisModel);
    // Isolate the two paths by adding 100 dB of loss for selected node pairs
    // * 3<-->4 <-> 5
    // * |          |
    // * |          |
    // * 1 <------> 2
    // node 1 (m_nodes.Get(0)) can talk to 2 and 3 but not 4 and 5
    m_matrixLossModel->SetDefaultLoss(0);
    m_matrixLossModel->SetLoss(m_nodes.Get(0)->GetObject<MobilityModel>(),
                               m_nodes.Get(3)->GetObject<MobilityModel>(),
                               100,
                               true);
    m_matrixLossModel->SetLoss(m_nodes.Get(0)->GetObject<MobilityModel>(),
                               m_nodes.Get(4)->GetObject<MobilityModel>(),
                               100,
                               true);
    // node 2 (m_nodes.Get(1)) can talk to 1 and 5 but not 3 and 4
    m_matrixLossModel->SetLoss(m_nodes.Get(1)->GetObject<MobilityModel>(),
                               m_nodes.Get(2)->GetObject<MobilityModel>(),
                               100,
                               true);
    m_matrixLossModel->SetLoss(m_nodes.Get(1)->GetObject<MobilityModel>(),
                               m_nodes.Get(3)->GetObject<MobilityModel>(),
                               100,
                               true);
    // node 3 (m_nodes.Get(2)) can talk to 1 and 4 but not 2 and 5
    m_matrixLossModel->SetLoss(m_nodes.Get(2)->GetObject<MobilityModel>(),
                               m_nodes.Get(4)->GetObject<MobilityModel>(),
                               100,
                               true);
    wifiPhy.SetChannel(channel);
    wifiMac.SetType("ns3::AdhocWifiMac");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("HeMcs0"));
    m_devices = wifi.Install(wifiPhy, wifiMac, m_nodes);
}

/**
 * @ingroup nhdp-tests
 * Configure NHDP on four node topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class NhdpUnequalPathNhdpTestCase : public NhdpUnequalPathTestCase
{
  public:
    NhdpUnequalPathNhdpTestCase(std::string name);

  protected:
    void DoRun() override;
};

NhdpUnequalPathNhdpTestCase::NhdpUnequalPathNhdpTestCase(std::string name)
    : NhdpUnequalPathTestCase(name)
{
}

void
NhdpUnequalPathNhdpTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(51)};

    InternetStackHelper internet;
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Schedule losses
    // Simulator::Schedule(Seconds(3), &NhdpUnequalPathTestCase::Disable, this, m_nodes.Get(0),
    // m_nodes.Get(1));
    Simulator::Schedule(Seconds(19), &NhdpTestCase::Print, this, nhdp1);
    // Simulator::Schedule(Seconds(20), &NhdpUnequalPathTestCase::Enable, this, m_nodes.Get(0),
    // m_nodes.Get(1));
    Simulator::Schedule(Seconds(30), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(31),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1));
    Simulator::Schedule(Seconds(31),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(2));
    Simulator::Schedule(Seconds(40), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(41),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1));
    Simulator::Schedule(Seconds(41),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(2));
    Simulator::Schedule(Seconds(50), &NhdpUnequalPathTestCase::Print, this, nhdp1);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class NhdpUnequalPathOlsrTestCase : public NhdpUnequalPathTestCase
{
  public:
    NhdpUnequalPathOlsrTestCase(std::string name);

  protected:
    void DoRun() override;
};

NhdpUnequalPathOlsrTestCase::NhdpUnequalPathOlsrTestCase(std::string name)
    : NhdpUnequalPathTestCase(name)
{
}

void
NhdpUnequalPathOlsrTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(90)};

    InternetStackHelper internet;
    OlsrHelper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));
    nhdp1->RegisterLinkQualityCallback(
        MakeCallback(&NhdpUnequalPathTestCase::LinkQualityCallback, this));
    // SNR above 1 will yield a quality of 1
    // SNR between 0 and 1 will yield a quality of 0.5
    // SNR below 0 will yield a quality of zero
    nhdp1->SetAttribute("HystAccept", DoubleValue(1));
    nhdp1->SetAttribute("HystReject", DoubleValue(0.5));
    nhdp1->SetAttribute("InitialPending", BooleanValue(true));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Disable alternate path
    Simulator::Schedule(Seconds(0),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(2));

    // The baseline SNR without additional path loss impairiment
    // between link 0 and 1 is 23.3027 dB
    // Force the SNR on the link from 0 to 1 to the value of -5 dB
    // at time 0 seconds.  This will force a routing change.
    Simulator::Schedule(Seconds(0),
                        &NhdpUnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        28.3027);

    // No neighbor relationships should be found before the link is enabled.
    Simulator::Schedule(Seconds(6.99), &NhdpUnequalPathTestCase::CheckNeighborSize, this, 0);

    // At time 7s, bring up the SNR to just under 0 dB.  Even though SNR
    // is strong enough, the link quality will block the HELLO adjacency
    Simulator::Schedule(Seconds(7),
                        &NhdpUnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        23.31);

    // At time 9s, after a neighbor HELLO was received, check that we created neighbor
    // state (not symmetric) to one neighbor.  There should be no link to address 7.0.0.2.
    Ipv4Address neighborAddr{"7.0.0.2"};
    NeighborTuple checkTuple{neighborAddr};
    checkTuple.m_symmetric = false;
    Simulator::Schedule(Seconds(9), &NhdpUnequalPathTestCase::CheckNeighbor, this, checkTuple);
    Simulator::Schedule(Seconds(9), &NhdpUnequalPathTestCase::CheckNeighborSize, this, 1);
    Simulator::Schedule(Seconds(9) + TimeStep(1),
                        &NhdpUnequalPathTestCase::ClearNeighborChanges,
                        this);
    Simulator::Schedule(Seconds(9), &NhdpUnequalPathTestCase::CheckLinkSize, this, 1);

    // At time 17s, bring up the SNR to  0.2 dB.  Even though SNR
    // is strong enough for reception, the link quality will block the HELLO adjacency
    Simulator::Schedule(Seconds(17),
                        &NhdpUnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        23.11);

    // At time 27s, bring up the SNR to  0.8 dB.  Even though quality is above
    // HYST_REJECT, the link quality will still block the HELLO adjacency
    Simulator::Schedule(Seconds(27),
                        &NhdpUnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        22.5);

    // At time 37s, bring up the SNR to  >1  dB.  This should form adjacency and add
    // a 2-hop neighbor
    Simulator::Schedule(Seconds(37),
                        &NhdpUnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        22);

    // At time 47s, drop the SNR to  0.8 dB.  Quality will still be above HYST_REJECT
    Simulator::Schedule(Seconds(47),
                        &NhdpUnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        22.5);

    // At time 57s, drop the SNR to  0 dB.  Quality falls below HYST_REJECT
    Simulator::Schedule(Seconds(57),
                        &NhdpUnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        23.3027);

    // At time 67s, up the SNR to  0.8 dB.  No HELLO adjacency.
    Simulator::Schedule(Seconds(67),
                        &NhdpUnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        22.5);

    // At time 77s, bring up the SNR to  >1  dB.  Even though SNR
    // is strong enough, the link quality will block the HELLO adjacency
    Simulator::Schedule(Seconds(77),
                        &NhdpUnequalPathTestCase::Change,
                        this,
                        m_nodes.Get(0),
                        m_nodes.Get(1),
                        22);

#endif
#if 0
    // Disable one path through the network before data transfer starts
    Simulator::Schedule(Seconds(5),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(14.99),
                        &NhdpUnequalPathTestCase::Enable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(14.99),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(24.99),
                        &NhdpUnequalPathTestCase::Enable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(24.99),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(34), &NhdpUnequalPathTestCase::Print, this, nhdp1);
#endif
#if 0

    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.5"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(8));
    apps2.Stop(stopTime);

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(4));
    apps3.Start(Seconds(8));
    apps3.Stop(stopTime);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class NhdpUnequalPathOlsrv2TestCase : public NhdpUnequalPathTestCase
{
  public:
    NhdpUnequalPathOlsrv2TestCase(std::string name);

  protected:
    void DoRun() override;
};

NhdpUnequalPathOlsrv2TestCase::NhdpUnequalPathOlsrv2TestCase(std::string name)
    : NhdpUnequalPathTestCase(name)
{
}

void
NhdpUnequalPathOlsrv2TestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(35)};

    InternetStackHelper internet;
    Olsrv2Helper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Disable one path through the network before data transfer starts
    Simulator::Schedule(Seconds(5),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(14.99),
                        &NhdpUnequalPathTestCase::Enable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(14.99),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    // Enable the disabled path, and disable the enabled path
    Simulator::Schedule(Seconds(24.99),
                        &NhdpUnequalPathTestCase::Enable,
                        this,
                        m_nodes.Get(1),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(24.99),
                        &NhdpUnequalPathTestCase::Disable,
                        this,
                        m_nodes.Get(2),
                        m_nodes.Get(3));

    Simulator::Schedule(Seconds(34), &NhdpUnequalPathTestCase::Print, this, nhdp1);

    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.4"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(10));
    apps2.Stop(stopTime);

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(3));
    apps3.Start(Seconds(10));
    apps3.Stop(stopTime);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using FriisPropagationLossModel, move two nodes together (outside of range
 * to in range), move them apart, and move them back, and observe the
 * state changes.  Should go from LOST to SYM to LOST to SYM
 *
 * 1 ------>>>        <<<---------- 2
 *   <<<------ 1    2 ------>>>
 * 1 ------>>>        <<<---------- 2
 */
class NhdpWaypointTestCase : public TestCase
{
  public:
    NhdpWaypointTestCase(std::string name);

    void NeighborChange(std::string context,
                        NeighborStatus neighborStatus,
                        const NeighborTuple& neighborTuple);
    void LinkChange(std::string context, LinkStatus oldLinkStatus, const LinkTuple& linkTuple);
    void TwoHopChange(std::string context, TwoHopStatus twoHopStatus, const TwoHopTuple& linkTuple);
    void HelloMessageSend(std::string context, Ptr<PbbMessage> helloMsg);
    void OlsrRoutingTableChange(std::string context, uint32_t tableSize);
    void OlsrTx(const olsr::PacketHeader& header, const olsr::MessageList& messages);
    void OlsrRx(const olsr::PacketHeader& header, const olsr::MessageList& messages);
    void Olsrv2Tx(const olsrv2::PacketHeader& header, const olsrv2::MessageList& messages);
    void Olsrv2Rx(const olsrv2::PacketHeader& header, const olsrv2::MessageList& messages);
    void Print(Ptr<NhdpClient> client);

  protected:
    std::map<Ipv4Address, NeighborTuple> m_neighborChanges;
    std::map<Ipv4Address, LinkTuple> m_linkChanges;
    std::map<Ipv4Address, TwoHopTuple> m_twoHopChanges;
    std::map<Ipv4Address, NeighborTuple> m_symmetricNeighbors;
    uint32_t m_txPacketsOlsrTrace;
    uint32_t m_txPacketsOlsrBytesTotal;
    uint32_t m_rxPacketsOlsrTrace;
};

NhdpWaypointTestCase::NhdpWaypointTestCase(std::string name)
    : TestCase(name)
{
}

void
NhdpWaypointTestCase::Print(Ptr<NhdpClient> client)
{
    for (const auto& [addr, tuple] : client->GetNeighborInfoBase())
    {
        NS_LOG_INFO(client->GetNode()->GetId()
                    << " Neighbor tuple " << addr << " symmetric " << tuple.m_symmetric);
    }
    for (const auto& [addr, tuple] : client->GetLinkInfoBase())
    {
        NS_LOG_INFO(client->GetNode()->GetId()
                    << " Link tuple " << addr << " status " << tuple.GetLinkStatus() << " lost "
                    << tuple.m_lost << " expiration time " << tuple.m_expirationTime.As(Time::S));
    }
    for (const auto& [key, tuple] : client->GetTwoHopInfoBase())
    {
        NS_LOG_INFO(client->GetNode()->GetId()
                    << " Two hop neighbor " << tuple.m_twoHopAddr << " via " << key.first
                    << " expiration time " << tuple.m_expirationTime.As(Time::S));
    }
}

void
NhdpWaypointTestCase::NeighborChange(std::string context,
                                     NeighborStatus neighborStatus,
                                     const NeighborTuple& neighborTuple)
{
    if (neighborStatus == NeighborStatus::NEW)
    {
        NS_LOG_INFO(context << " New neighbor " << neighborTuple.m_neighborAddrList[0]);
    }
    else if (neighborStatus == NeighborStatus::MODIFIED && neighborTuple.m_symmetric)
    {
        NS_LOG_INFO(context << " Symmetric neighbor " << neighborTuple.m_neighborAddrList[0]);
        m_symmetricNeighbors.insert_or_assign(neighborTuple.m_neighborAddrList[0], neighborTuple);
    }
    else if (neighborStatus == NeighborStatus::REMOVED)
    {
        NS_LOG_INFO(context << " Removed neighbor " << neighborTuple.m_neighborAddrList[0]);
    }
    m_neighborChanges.insert_or_assign(neighborTuple.m_neighborAddrList[0], neighborTuple);
}

void
NhdpWaypointTestCase::LinkChange(std::string context,
                                 LinkStatus oldLinkStatus,
                                 const LinkTuple& linkTuple)
{
    NS_LOG_INFO(context << " Link old status " << oldLinkStatus << " new status "
                        << linkTuple.GetLinkStatus() << " " << linkTuple.m_neighborAddrList[0]);
    m_linkChanges.insert_or_assign(linkTuple.m_neighborAddrList[0], linkTuple);
}

void
NhdpWaypointTestCase::HelloMessageSend(std::string context, Ptr<PbbMessage> helloMsg)
{
    NS_LOG_INFO(context << " Hello message send " << helloMsg->GetSerializedSize());
}

void
NhdpWaypointTestCase::TwoHopChange(std::string context,
                                   TwoHopStatus twoHopStatus,
                                   const TwoHopTuple& twoHopTuple)
{
    if (twoHopStatus == TwoHopStatus::NEW)
    {
        NS_LOG_INFO(context << " New two-hop neighbor " << twoHopTuple.m_twoHopAddr << " from "
                            << twoHopTuple.m_neighborAddrList[0]);
    }
    else
    {
        NS_LOG_INFO(context << " Existing two-hop neighbor " << twoHopTuple.m_twoHopAddr << " from "
                            << twoHopTuple.m_neighborAddrList[0]);
    }
    m_twoHopChanges.insert_or_assign(twoHopTuple.m_neighborAddrList[0], twoHopTuple);
}

void
NhdpWaypointTestCase::OlsrRoutingTableChange(std::string context, uint32_t tableSize)
{
    NS_LOG_INFO(context << " routing table change to " << tableSize);
}

void
NhdpWaypointTestCase::OlsrTx(const olsr::PacketHeader& header, const olsr::MessageList&)
{
    m_txPacketsOlsrTrace++;
    m_txPacketsOlsrBytesTotal += header.GetPacketLength();
}

void
NhdpWaypointTestCase::OlsrRx(const olsr::PacketHeader&, const olsr::MessageList&)
{
    NS_LOG_DEBUG("Rx");
    m_rxPacketsOlsrTrace++;
}

void
NhdpWaypointTestCase::Olsrv2Tx(const olsrv2::PacketHeader& header, const olsrv2::MessageList&)
{
    m_txPacketsOlsrTrace++;
    m_txPacketsOlsrBytesTotal += header.GetPacketLength();
}

void
NhdpWaypointTestCase::Olsrv2Rx(const olsrv2::PacketHeader&, const olsrv2::MessageList&)
{
    m_rxPacketsOlsrTrace++;
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class NhdpTwoNodeWaypointTestCase : public NhdpWaypointTestCase
{
  public:
    NhdpTwoNodeWaypointTestCase(std::string name);

    void Disable(Ptr<Node> a, Ptr<Node> b);
    void Enable(Ptr<Node> a, Ptr<Node> b);
    double LinkQualityCallback(Ptr<Packet> packet) const;

  protected:
    void DoSetup() override;
    Ptr<MatrixPropagationLossModel> m_matrixLossModel;
    Ptr<FriisPropagationLossModel> m_lossModel;
    NodeContainer m_nodes;
    NetDeviceContainer m_devices;
};

NhdpTwoNodeWaypointTestCase::NhdpTwoNodeWaypointTestCase(std::string name)
    : NhdpWaypointTestCase(name)
{
}

double
NhdpTwoNodeWaypointTestCase::LinkQualityCallback(Ptr<Packet> packet) const
{
    SnrTag snrTag;
    auto found = packet->RemovePacketTag(snrTag);
    double threshold = -1; // dB
    double range = 6;      // dB
    if (!found)
    {
        NS_LOG_DEBUG("SnrTag not found");
        return 1;
    }
    double quality = 0;
    double snrDb = RatioToDb(snrTag.Get());
    if (snrDb > threshold)
    {
        quality = std::min(1.0, (snrDb - threshold) / range);
        quality = std::max(0.0, quality);
    }
    NS_LOG_DEBUG("Link quality: " << quality << " SNR_dB: " << snrDb);
    return quality;
}

void
NhdpTwoNodeWaypointTestCase::Disable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Disabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               100,
                               true);
}

void
NhdpTwoNodeWaypointTestCase::Enable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Enabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               0,
                               true);
}

void
NhdpTwoNodeWaypointTestCase::DoSetup()
{
    // Create node index zero but do not use it; this allows the subsequent
    // node IDs to align with the last octet of the IP address, for help
    // in correlating IP addresses to nodes
    Ptr<Node> unusedNode [[maybe_unused]] = CreateObject<Node>();

    m_nodes.Create(2);
    auto waypointMm = CreateObject<ConstantPositionMobilityModel>();
    m_nodes.Get(0)->AggregateObject(waypointMm);
    auto constantMm = CreateObject<ConstantPositionMobilityModel>();
    m_nodes.Get(1)->AggregateObject(constantMm);

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    WifiMacHelper wifiMac;
    YansWifiPhyHelper wifiPhy;
    wifiPhy.DisablePreambleDetectionModel();
    YansWifiChannelHelper wifiChannel;
    auto channel = CreateObject<YansWifiChannel>();
    auto delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
    m_lossModel = CreateObject<FriisPropagationLossModel>();
    channel->SetPropagationDelayModel(delayModel);
    channel->SetPropagationLossModel(m_lossModel);
    wifiPhy.SetChannel(channel);
    wifiMac.SetType("ns3::AdhocWifiMac");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue("HeMcs0"),
                                 "ControlMode",
                                 StringValue("HeMcs0"));
    wifiPhy.Set("TxPowerStart", DoubleValue(12));
    wifiPhy.Set("TxPowerEnd", DoubleValue(12));
    m_devices = wifi.Install(wifiPhy, wifiMac, m_nodes);
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class NhdpTwoNodeNhdpWaypointTestCase : public NhdpTwoNodeWaypointTestCase
{
  public:
    NhdpTwoNodeNhdpWaypointTestCase(std::string name);

  protected:
    void DoRun() override;
};

NhdpTwoNodeNhdpWaypointTestCase::NhdpTwoNodeNhdpWaypointTestCase(std::string name)
    : NhdpTwoNodeWaypointTestCase(name)
{
}

void
NhdpTwoNodeNhdpWaypointTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(300)};

    auto mm1 = m_nodes.Get(0)->GetObject<WaypointMobilityModel>();
    NS_ASSERT_MSG(mm1, "Waypoint mobility model not found");
    auto mm2 = m_nodes.Get(1)->GetObject<WaypointMobilityModel>();
    NS_ASSERT_MSG(mm2, "Waypoint mobility model not found");
    mm1->AddWaypoint(Waypoint(Seconds(0), Vector(0, 0, 0)));
    mm1->AddWaypoint(Waypoint(Seconds(100), Vector(480, 0, 0)));
    mm1->AddWaypoint(Waypoint(Seconds(200), Vector(0, 0, 0)));
    mm1->AddWaypoint(Waypoint(Seconds(300), Vector(480, 0, 0)));
    mm2->AddWaypoint(Waypoint(Seconds(0), Vector(1000, 0, 0)));
    mm2->AddWaypoint(Waypoint(Seconds(100), Vector(520, 0, 0)));
    mm2->AddWaypoint(Waypoint(Seconds(200), Vector(1000, 0, 0)));
    mm2->AddWaypoint(Waypoint(Seconds(300), Vector(520, 0, 0)));

    InternetStackHelper internet;
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange",
                        "1",
                        MakeCallback(&NhdpWaypointTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpWaypointTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange",
                        "1",
                        MakeCallback(&NhdpWaypointTestCase::TwoHopChange, this));
    nhdp1->RegisterLinkQualityCallback(
        MakeCallback(&NhdpTwoNodeWaypointTestCase::LinkQualityCallback, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class NhdpOlsrWaypointTestCase : public NhdpTwoNodeWaypointTestCase
{
  public:
    NhdpOlsrWaypointTestCase(std::string name);

  protected:
    void DoRun() override;
};

NhdpOlsrWaypointTestCase::NhdpOlsrWaypointTestCase(std::string name)
    : NhdpTwoNodeWaypointTestCase(name)
{
}

void
NhdpOlsrWaypointTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(999)};

    auto mm1 = m_nodes.Get(0)->GetObject<ConstantPositionMobilityModel>();
    NS_ASSERT_MSG(mm1, "Waypoint mobility model not found");
    auto cm2 = m_nodes.Get(1)->GetObject<ConstantPositionMobilityModel>();
    NS_ASSERT_MSG(cm2, "Constant mobility model not found");
    cm2->SetPosition(Vector(0, 0, 0));
    cm2->SetPosition(Vector(800, 0, 0));
    InternetStackHelper internet;
    OlsrHelper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange",
                        "1",
                        MakeCallback(&NhdpWaypointTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpWaypointTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange",
                        "1",
                        MakeCallback(&NhdpWaypointTestCase::TwoHopChange, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    Simulator::Schedule(Seconds(34), &NhdpTwoNodeWaypointTestCase::Print, this, nhdp1);

    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.2"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(10));
    apps2.Stop(stopTime);

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(1));
    apps3.Start(Seconds(10));
    apps3.Stop(stopTime);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 * Using MatrixPropagationLossModel, create a four-node topology in which each node can hear
 * two neighbors, as follows:
 *
 * 3 <------> 4
 * |          |
 * |          |
 * 1 <------> 2
 *
 * Start the simulation and check that node 1 ends up with two symmetric neighbors
 */
class NhdpOlsrv2WaypointTestCase : public NhdpTwoNodeWaypointTestCase
{
  public:
    NhdpOlsrv2WaypointTestCase(std::string name);

  protected:
    void DoRun() override;
};

NhdpOlsrv2WaypointTestCase::NhdpOlsrv2WaypointTestCase(std::string name)
    : NhdpTwoNodeWaypointTestCase(name)
{
}

void
NhdpOlsrv2WaypointTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(1000)};

    auto mm1 = m_nodes.Get(0)->GetObject<ConstantPositionMobilityModel>();
    NS_ASSERT_MSG(mm1, "Waypoint mobility model not found");
    auto cm2 = m_nodes.Get(1)->GetObject<ConstantPositionMobilityModel>();
    NS_ASSERT_MSG(cm2, "Constant mobility model not found");
    cm2->SetPosition(Vector(0, 0, 0));
    cm2->SetPosition(Vector(1050, 0, 0));

    InternetStackHelper internet;
    Olsrv2Helper olsr;
    Ipv4ListRoutingHelper list;
    list.Add(olsr, 100);
    internet.SetRoutingHelper(list);
    internet.Install(m_nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(m_devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(m_nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange",
                        "1",
                        MakeCallback(&NhdpWaypointTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpWaypointTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange",
                        "1",
                        MakeCallback(&NhdpWaypointTestCase::TwoHopChange, this));
    nhdp1->RegisterLinkQualityCallback(
        MakeCallback(&NhdpTwoNodeWaypointTestCase::LinkQualityCallback, this));
    auto nhdp2 = apps.Get(1)->GetObject<NhdpClient>();
    nhdp2->TraceConnect("HelloMessageSend",
                        "2",
                        MakeCallback(&NhdpWaypointTestCase::HelloMessageSend, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // 1 packet every 200 ms
    uint16_t port = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("7.0.0.2"), port)));
    onoff.SetConstantRate(DataRate(20480));
    ApplicationContainer apps2 = onoff.Install(m_nodes.Get(0));
    apps2.Start(Seconds(10));
    apps2.Stop(stopTime);

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer apps3 = sink.Install(m_nodes.Get(1));
    apps3.Start(Seconds(10));
    apps3.Stop(stopTime);

    NS_LOG_INFO("Start simulation for " << (stopTime + Seconds(1)).As(Time::S) << " duration");
    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}
#endif

/**
 * @ingroup nhdp-tests
 * TestSuite for module nhdp
 */
class NhdpTestSuite : public TestSuite
{
  public:
    NhdpTestSuite();
};

NhdpTestSuite::NhdpTestSuite()
    : TestSuite("nhdp-system", Type::SYSTEM)
{
    AddTestCase(new NhdpTwoNodeNhdpTestCase("Two node test checking state transitions"),
                TestCase::Duration::QUICK);
#if 0
    AddTestCase(new NhdpFourNodeNhdpTestCase("Four node matrix test with losses"),
                TestCase::Duration::QUICK);
    AddTestCase(new NhdpFourNodeOlsrTestCase("Four node matrix test with OLSRv1"),
                TestCase::Duration::QUICK);
    AddTestCase(new NhdpFourNodeOlsrv2TestCase("Four node matrix test with OLSRv2"),
                TestCase::Duration::QUICK);
    AddTestCase(new NhdpUnequalPathOlsrTestCase("Unequal path matrix test with OLSRv1"),
                TestCase::Duration::QUICK);
    AddTestCase(new NhdpTwoNodeNhdpWaypointTestCase("Two node waypoint mobility with NHDP"), TestCase::Duration::QUICK);
    AddTestCase(new NhdpOlsrWaypointTestCase("Two node waypoint mobility with OLSR"), TestCase::Duration::QUICK);
    AddTestCase(new NhdpOlsrv2WaypointTestCase("Two node waypoint mobility with NHDP and OLSR"), TestCase::Duration::QUICK);
#endif
}

/**
 * @ingroup nhdp-tests
 * Static variable for test initialization
 */
static NhdpTestSuite s_nhdpTestSuite;
