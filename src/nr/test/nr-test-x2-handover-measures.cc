// Copyright (c) 2013 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>
//         Manuel Requena <manuel.requena@cttc.es>/

#include <ns3/bulk-send-helper.h>
#include <ns3/core-module.h>
#include <ns3/internet-module.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>
#include <ns3/nr-module.h>
#include <ns3/packet-sink-helper.h>
#include <ns3/packet-sink.h>
#include <ns3/point-to-point-module.h>
#include <ns3/udp-client-server-helper.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrX2HandoverMeasuresTest");

/**
 * \ingroup nr-test
 *
 * \brief CheckPointEvent structure
 */
struct CheckPointEvent
{
    Time checkStartTime;     ///< check start time
    Time checkStopTime;      ///< check stop time
    Time checkInterval;      ///< check interval
    uint32_t ueDeviceIndex;  ///< UE device index
    uint32_t gnbDeviceIndex; ///< gNB device index

    /**
     *  Constructor
     *
     * \param start the start time
     * \param stop the stop time
     * \param interval the interval time
     * \param ueIndex the UE index
     * \param gnbIndex the gNB index
     */
    CheckPointEvent(Time start, Time stop, Time interval, uint32_t ueIndex, uint32_t gnbIndex)
        : checkStartTime(start),
          checkStopTime(stop),
          checkInterval(interval),
          ueDeviceIndex(ueIndex),
          gnbDeviceIndex(gnbIndex)
    {
    }
};

/**
 * \ingroup lte-test
 *
 * \brief Test different X2 handover measures and algorithms, e.g. NrA2A4RsrqHandoverAlgorithm and
 * NrA3RsrpHandoverAlgorithm. Test defines different handover parameters and scenario
 * configurations.
 */
class NrX2HandoverMeasuresTestCase : public TestCase
{
  public:
    /**
     * Constructor.
     *
     * \param nGnbs number of gNBs in the test
     * \param nUes number of UEs in the test
     * \param nDedicatedBearers number of bearers to be activated per UE
     * \param checkPointEventList list of check point events
     * \param checkPointEventListName name of check point event list
     * \param useUdp true if UDP is to be used, false if TCP is to be used
     * \param schedulerType type of scheduler to be used (e.g. "ns3::NrMacSchedulerTdmaPF")
     * \param handoverAlgorithmType type of handover algorithm to be used (e.g.
     * "ns3::NrA3RsrpHandoverAlgorithm")
     * \param admitHo true if Ho is admitted, false if it is not admitted
     * \param useIdealRrc true if ideal RRC is to be used, false if real RRC is to be used
     */
    NrX2HandoverMeasuresTestCase(uint32_t nGnbs,
                                 uint32_t nUes,
                                 uint32_t nDedicatedBearers,
                                 std::list<CheckPointEvent> checkPointEventList,
                                 std::string checkPointEventListName,
                                 bool useUdp,
                                 std::string schedulerType,
                                 std::string handoverAlgorithmType,
                                 bool admitHo,
                                 bool useIdealRrc);

  private:
    /**
     * Build name string
     * \param nGnbs number of gNBs in the test
     * \param nUes number of UEs in the test
     * \param nDedicatedBearers number of bearers to be activated per UE
     * \param checkPointEventListName name of check point event list
     * \param useUdp true if UDP is to be used, false if TCP is to be used
     * \param schedulerType the scheduler type
     * \param handoverAlgorithmType type of handover algorithm to be used (e.g.
     * "ns3::NrA3RsrpHandoverAlgorithm")
     * \param admitHo true if Ho is admitted, false if it is not admitted
     * \param useIdealRrc true if the ideal RRC should be used
     * \returns the name string
     */
    static std::string BuildNameString(uint32_t nGnbs,
                                       uint32_t nUes,
                                       uint32_t nDedicatedBearers,
                                       std::string checkPointEventListName,
                                       bool useUdp,
                                       std::string schedulerType,
                                       std::string handoverAlgorithmType,
                                       bool admitHo,
                                       bool useIdealRrc);
    void DoRun() override;
    /**
     * Check connected function
     * \param ueDevice the UE device
     * \param gnbDevice the gNB device
     */
    void CheckConnected(Ptr<NetDevice> ueDevice, Ptr<NetDevice> gnbDevice);

    uint32_t m_nGnbs;                                 ///< number of gNBs in the test
    uint32_t m_nUes;                                  ///< number of UEs in the test
    uint32_t m_nDedicatedBearers;                     ///< number of UEs in the test
    std::list<CheckPointEvent> m_checkPointEventList; ///< check point event list
    std::string m_checkPointEventListName;            ///< check point event list name
    bool m_epc;                                       ///< whether to use EPC
    bool m_useUdp;                                    ///< whether to use UDP traffic
    std::string m_schedulerType;                      ///< scheduler type
    std::string m_handoverAlgorithmType;              ///< handover algorithm type
    bool m_admitHo;                                   ///< whether to configure to admit handover
    bool m_useIdealRrc;                               ///< whether to use ideal RRC
    Ptr<NrHelper> m_nrHelper;                         ///< NR helper
    Ptr<NrPointToPointEpcHelper> m_epcHelper;         ///< EPC helper

    /**
     * \ingroup lte-test
     *
     * \brief BearerData structure
     */
    struct BearerData
    {
        uint32_t bid;           ///< BID
        Ptr<PacketSink> dlSink; ///< DL sink
        Ptr<PacketSink> ulSink; ///< UL sink
        uint32_t dlOldTotalRx;  ///< DL old total receive
        uint32_t ulOldTotalRx;  ///< UL old total receive
    };

    /**
     * \ingroup lte-test
     *
     * \brief UeData structure
     */
    struct UeData
    {
        uint32_t id;                          ///< ID
        std::list<BearerData> bearerDataList; ///< bearer ID list
    };

    /**
     * \brief Save stats  function
     * \param ueIndex the index of the UE
     */
    void SaveStats(uint32_t ueIndex);
    /**
     * \brief Check stats  function
     * \param ueIndex the index of the UE
     */
    void CheckStats(uint32_t ueIndex);

    std::vector<UeData> m_ueDataVector; ///< UE data vector

    const Time m_maxHoDuration;        ///< maximum HO duration
    const Time m_statsDuration;        ///< stats duration
    const Time m_udpClientInterval;    ///< UDP client interval
    const uint32_t m_udpClientPktSize; ///< UDP client packet size
};

std::string
NrX2HandoverMeasuresTestCase::BuildNameString(uint32_t nGnbs,
                                              uint32_t nUes,
                                              uint32_t nDedicatedBearers,
                                              std::string checkPointEventListName,
                                              bool useUdp,
                                              std::string schedulerType,
                                              std::string handoverAlgorithmType,
                                              bool admitHo,
                                              bool useIdealRrc)
{
    std::ostringstream oss;
    oss << "nGnbs=" << nGnbs << " nUes=" << nUes << " nDedicatedBearers=" << nDedicatedBearers
        << " udp=" << useUdp << " " << schedulerType << " " << handoverAlgorithmType
        << " admitHo=" << admitHo << " hoList: " << checkPointEventListName;
    if (useIdealRrc)
    {
        oss << ", ideal RRC";
    }
    else
    {
        oss << ", real RRC";
    }
    return oss.str();
}

NrX2HandoverMeasuresTestCase::NrX2HandoverMeasuresTestCase(
    uint32_t nGnbs,
    uint32_t nUes,
    uint32_t nDedicatedBearers,
    std::list<CheckPointEvent> checkPointEventList,
    std::string checkPointEventListName,
    bool useUdp,
    std::string schedulerType,
    std::string handoverAlgorithmType,
    bool admitHo,
    bool useIdealRrc)
    : TestCase(BuildNameString(nGnbs,
                               nUes,
                               nDedicatedBearers,
                               checkPointEventListName,
                               useUdp,
                               schedulerType,
                               handoverAlgorithmType,
                               admitHo,
                               useIdealRrc)),
      m_nGnbs(nGnbs),
      m_nUes(nUes),
      m_nDedicatedBearers(nDedicatedBearers),
      m_checkPointEventList(checkPointEventList),
      m_checkPointEventListName(checkPointEventListName),
      m_epc(true),
      m_useUdp(useUdp),
      m_schedulerType(schedulerType),
      m_handoverAlgorithmType(handoverAlgorithmType),
      m_admitHo(admitHo),
      m_useIdealRrc(useIdealRrc),
      m_maxHoDuration(Seconds(0.1)),
      m_statsDuration(Seconds(0.5)),
      m_udpClientInterval(Seconds(0.01)),
      m_udpClientPktSize(100)
{
}

void
NrX2HandoverMeasuresTestCase::DoRun()
{
    NS_LOG_FUNCTION(this << BuildNameString(m_nGnbs,
                                            m_nUes,
                                            m_nDedicatedBearers,
                                            m_checkPointEventListName,
                                            m_useUdp,
                                            m_schedulerType,
                                            m_handoverAlgorithmType,
                                            m_admitHo,
                                            m_useIdealRrc));

    Config::Reset();
    Config::SetDefault("ns3::UdpClient::Interval", TimeValue(m_udpClientInterval));
    Config::SetDefault("ns3::UdpClient::MaxPackets", UintegerValue(1000000));
    Config::SetDefault("ns3::UdpClient::PacketSize", UintegerValue(m_udpClientPktSize));
    Config::SetDefault("ns3::NrGnbRrc::HandoverJoiningTimeoutDuration",
                       TimeValue(MilliSeconds(200)));
    Config::SetDefault("ns3::NrGnbPhy::TxPower", DoubleValue(20));

    // Disable Uplink Power Control
    Config::SetDefault("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue(false));

    int64_t stream = 1;

    m_nrHelper = CreateObject<NrHelper>();
    m_nrHelper->SetAttribute("UseIdealRrc", BooleanValue(m_useIdealRrc));
    m_nrHelper->SetSchedulerTypeId(TypeId::LookupByName(m_schedulerType));

    if (m_handoverAlgorithmType == "ns3::NrA2A4RsrqHandoverAlgorithm")
    {
        m_nrHelper->SetHandoverAlgorithmType("ns3::NrA2A4RsrqHandoverAlgorithm");
        m_nrHelper->SetHandoverAlgorithmAttribute("ServingCellThreshold", UintegerValue(30));
        m_nrHelper->SetHandoverAlgorithmAttribute("NeighbourCellOffset", UintegerValue(1));
    }
    else if (m_handoverAlgorithmType == "ns3::NrA3RsrpHandoverAlgorithm")
    {
        m_nrHelper->SetHandoverAlgorithmType("ns3::NrA3RsrpHandoverAlgorithm");
        m_nrHelper->SetHandoverAlgorithmAttribute("Hysteresis", DoubleValue(1.5));
        m_nrHelper->SetHandoverAlgorithmAttribute("TimeToTrigger", TimeValue(MilliSeconds(128)));
    }
    else
    {
        NS_FATAL_ERROR("Unknown handover algorithm " << m_handoverAlgorithmType);
    }

    double distance = 1000.0; // m
    double speed = 150;       // m/s

    NodeContainer gnbNodes;
    gnbNodes.Create(m_nGnbs);
    NodeContainer ueNodes;
    ueNodes.Create(m_nUes);

    if (m_epc)
    {
        m_epcHelper = CreateObject<NrPointToPointEpcHelper>();
        m_nrHelper->SetEpcHelper(m_epcHelper);
    }

    // Install Mobility Model in gNBs
    // gNBs are located along a line in the X axis
    Ptr<ListPositionAllocator> gnbPositionAlloc = CreateObject<ListPositionAllocator>();
    for (uint32_t i = 0; i < m_nGnbs; i++)
    {
        Vector gnbPosition(distance * (i + 1), 0, 0);
        gnbPositionAlloc->Add(gnbPosition);
    }
    MobilityHelper gnbMobility;
    gnbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    gnbMobility.SetPositionAllocator(gnbPositionAlloc);
    gnbMobility.Install(gnbNodes);

    // Install Mobility Model in UE
    // UE moves with a constant speed along the X axis
    MobilityHelper ueMobility;
    ueMobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    ueMobility.Install(ueNodes);
    for (uint32_t i = 0; i < m_nUes; i++)
    {
        ueNodes.Get(i)->GetObject<MobilityModel>()->SetPosition(Vector(0, 0, 0));
        ueNodes.Get(i)->GetObject<ConstantVelocityMobilityModel>()->SetVelocity(
            Vector(speed, 0, 0));
    }

    auto bandwidthAndBWPPair = m_nrHelper->CreateBandwidthParts({{2.8e9, 5e6, 1}}, "UMa");

    NetDeviceContainer gnbDevices;
    gnbDevices = m_nrHelper->InstallGnbDevice(gnbNodes, bandwidthAndBWPPair.second);
    stream += m_nrHelper->AssignStreams(gnbDevices, stream);
    for (auto it = gnbDevices.Begin(); it != gnbDevices.End(); ++it)
    {
        Ptr<NrGnbRrc> gnbRrc = (*it)->GetObject<NrGnbNetDevice>()->GetRrc();
        gnbRrc->SetAttribute("AdmitHandoverRequest", BooleanValue(m_admitHo));
    }

    NetDeviceContainer ueDevices;
    ueDevices = m_nrHelper->InstallUeDevice(ueNodes, bandwidthAndBWPPair.second);
    stream += m_nrHelper->AssignStreams(ueDevices, stream);

    Ipv4Address remoteHostAddr;
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ipv4InterfaceContainer ueIpIfaces;
    Ptr<Node> remoteHost;
    if (m_epc)
    {
        // Create a single RemoteHost
        NodeContainer remoteHostContainer;
        remoteHostContainer.Create(1);
        remoteHost = remoteHostContainer.Get(0);
        InternetStackHelper internet;
        internet.Install(remoteHostContainer);

        // Create the Internet
        PointToPointHelper p2ph;
        p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
        p2ph.SetDeviceAttribute("Mtu", UintegerValue(1500));
        p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.010)));
        Ptr<Node> pgw = m_epcHelper->GetPgwNode();
        NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
        Ipv4AddressHelper ipv4h;
        ipv4h.SetBase("1.0.0.0", "255.0.0.0");
        Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
        // in this container, interface 0 is the pgw, 1 is the remoteHost
        remoteHostAddr = internetIpIfaces.GetAddress(1);

        Ipv4StaticRoutingHelper ipv4RoutingHelper;
        Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
        remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"),
                                                   Ipv4Mask("255.0.0.0"),
                                                   1);

        // Install the IP stack on the UEs
        internet.Install(ueNodes);
        ueIpIfaces = m_epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueDevices));
    }

    // attachment (needs to be done after IP stack configuration)
    // all UEs attached to gNB 0 at the beginning
    for (uint32_t i = 0; i < ueDevices.GetN(); i++)
    {
        m_nrHelper->AttachToGnb(ueDevices.Get(i), gnbDevices.Get(0));
    }

    if (m_epc)
    {
        bool epcDl = true;
        bool epcUl = false;
        // the rest of this block is copied from lena-dual-stripe

        // Install and start applications on UEs and remote host
        uint16_t dlPort = 10000;
        uint16_t ulPort = 20000;

        // randomize a bit start times to avoid simulation artifacts
        // (e.g., buffer overflows due to packet transmissions happening
        // exactly at the same time)
        Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable>();
        startTimeSeconds->SetAttribute("Min", DoubleValue(0));
        startTimeSeconds->SetAttribute("Max", DoubleValue(0.010));
        startTimeSeconds->SetStream(stream++);

        for (uint32_t u = 0; u < ueNodes.GetN(); ++u)
        {
            Ptr<Node> ue = ueNodes.Get(u);
            // Set the default gateway for the UE
            Ptr<Ipv4StaticRouting> ueStaticRouting =
                ipv4RoutingHelper.GetStaticRouting(ue->GetObject<Ipv4>());
            ueStaticRouting->SetDefaultRoute(m_epcHelper->GetUeDefaultGatewayAddress(), 1);

            UeData ueData;

            for (uint32_t b = 0; b < m_nDedicatedBearers; ++b)
            {
                ++dlPort;
                ++ulPort;

                ApplicationContainer clientApps;
                ApplicationContainer serverApps;
                BearerData bearerData = BearerData();

                if (m_useUdp)
                {
                    if (epcDl)
                    {
                        UdpClientHelper dlClientHelper(ueIpIfaces.GetAddress(u), dlPort);
                        clientApps.Add(dlClientHelper.Install(remoteHost));
                        PacketSinkHelper dlPacketSinkHelper(
                            "ns3::UdpSocketFactory",
                            InetSocketAddress(Ipv4Address::GetAny(), dlPort));
                        ApplicationContainer sinkContainer = dlPacketSinkHelper.Install(ue);
                        bearerData.dlSink = sinkContainer.Get(0)->GetObject<PacketSink>();
                        serverApps.Add(sinkContainer);
                    }
                    if (epcUl)
                    {
                        UdpClientHelper ulClientHelper(remoteHostAddr, ulPort);
                        clientApps.Add(ulClientHelper.Install(ue));
                        PacketSinkHelper ulPacketSinkHelper(
                            "ns3::UdpSocketFactory",
                            InetSocketAddress(Ipv4Address::GetAny(), ulPort));
                        ApplicationContainer sinkContainer = ulPacketSinkHelper.Install(remoteHost);
                        bearerData.ulSink = sinkContainer.Get(0)->GetObject<PacketSink>();
                        serverApps.Add(sinkContainer);
                    }
                }
                else // use TCP
                {
                    if (epcDl)
                    {
                        BulkSendHelper dlClientHelper(
                            "ns3::TcpSocketFactory",
                            InetSocketAddress(ueIpIfaces.GetAddress(u), dlPort));
                        dlClientHelper.SetAttribute("MaxBytes", UintegerValue(0));
                        clientApps.Add(dlClientHelper.Install(remoteHost));
                        PacketSinkHelper dlPacketSinkHelper(
                            "ns3::TcpSocketFactory",
                            InetSocketAddress(Ipv4Address::GetAny(), dlPort));
                        ApplicationContainer sinkContainer = dlPacketSinkHelper.Install(ue);
                        bearerData.dlSink = sinkContainer.Get(0)->GetObject<PacketSink>();
                        serverApps.Add(sinkContainer);
                    }
                    if (epcUl)
                    {
                        BulkSendHelper ulClientHelper("ns3::TcpSocketFactory",
                                                      InetSocketAddress(remoteHostAddr, ulPort));
                        ulClientHelper.SetAttribute("MaxBytes", UintegerValue(0));
                        clientApps.Add(ulClientHelper.Install(ue));
                        PacketSinkHelper ulPacketSinkHelper(
                            "ns3::TcpSocketFactory",
                            InetSocketAddress(Ipv4Address::GetAny(), ulPort));
                        ApplicationContainer sinkContainer = ulPacketSinkHelper.Install(remoteHost);
                        bearerData.ulSink = sinkContainer.Get(0)->GetObject<PacketSink>();
                        serverApps.Add(sinkContainer);
                    }
                } // end if (useUdp)

                Ptr<NrEpcTft> tft = Create<NrEpcTft>();
                if (epcDl)
                {
                    NrEpcTft::PacketFilter dlpf;
                    dlpf.localPortStart = dlPort;
                    dlpf.localPortEnd = dlPort;
                    tft->Add(dlpf);
                }
                if (epcUl)
                {
                    NrEpcTft::PacketFilter ulpf;
                    ulpf.remotePortStart = ulPort;
                    ulpf.remotePortEnd = ulPort;
                    tft->Add(ulpf);
                }

                if (epcDl || epcUl)
                {
                    NrEpsBearer bearer(NrEpsBearer::NGBR_VIDEO_TCP_DEFAULT);
                    m_nrHelper->ActivateDedicatedEpsBearer(ueDevices.Get(u), bearer, tft);
                }
                Time startTime = Seconds(startTimeSeconds->GetValue());
                serverApps.Start(startTime);
                clientApps.Start(startTime);

                ueData.bearerDataList.push_back(bearerData);

            } // end for b

            m_ueDataVector.push_back(ueData);
        }
    }
    else // (epc == false)
    {
        // for radio bearer activation purposes, consider together home UEs and macro UEs
        for (uint32_t u = 0; u < ueDevices.GetN(); ++u)
        {
            Ptr<NetDevice> ueDev = ueDevices.Get(u);
            for (uint32_t b = 0; b < m_nDedicatedBearers; ++b)
            {
                NrEpsBearer::Qci q = NrEpsBearer::NGBR_VIDEO_TCP_DEFAULT;
                NrEpsBearer bearer(q);
                m_nrHelper->ActivateDataRadioBearer(ueDev, bearer);
            }
        }
    }

    m_nrHelper->AddX2Interface(gnbNodes);

    // check initial RRC connection
    const Time maxRrcConnectionEstablishmentDuration = Seconds(0.080);
    for (auto it = ueDevices.Begin(); it != ueDevices.End(); ++it)
    {
        NS_LOG_FUNCTION(maxRrcConnectionEstablishmentDuration);
        Simulator::Schedule(maxRrcConnectionEstablishmentDuration,
                            &NrX2HandoverMeasuresTestCase::CheckConnected,
                            this,
                            *it,
                            gnbDevices.Get(0));
    }

    // schedule the checkpoint events

    Time stopTime = Seconds(0);
    for (auto checkPointEventIt = m_checkPointEventList.begin();
         checkPointEventIt != m_checkPointEventList.end();
         ++checkPointEventIt)
    {
        for (Time checkPointTime = checkPointEventIt->checkStartTime;
             checkPointTime < checkPointEventIt->checkStopTime;
             checkPointTime += checkPointEventIt->checkInterval)
        {
            Simulator::Schedule(checkPointTime,
                                &NrX2HandoverMeasuresTestCase::CheckConnected,
                                this,
                                ueDevices.Get(checkPointEventIt->ueDeviceIndex),
                                gnbDevices.Get(checkPointEventIt->gnbDeviceIndex));

            Simulator::Schedule(checkPointTime,
                                &NrX2HandoverMeasuresTestCase::SaveStats,
                                this,
                                checkPointEventIt->ueDeviceIndex);

            Time checkStats = checkPointTime + m_statsDuration;
            Simulator::Schedule(checkStats,
                                &NrX2HandoverMeasuresTestCase::CheckStats,
                                this,
                                checkPointEventIt->ueDeviceIndex);

            if (stopTime <= checkStats)
            {
                stopTime = checkStats + Seconds(1);
            }
        }
    }

    Simulator::Stop(stopTime);
    Simulator::Run();
    Simulator::Destroy();
}

void
NrX2HandoverMeasuresTestCase::CheckConnected(Ptr<NetDevice> ueDevice, Ptr<NetDevice> gnbDevice)
{
    NS_LOG_FUNCTION(ueDevice << gnbDevice);

    Ptr<NrUeNetDevice> ueLteDevice = ueDevice->GetObject<NrUeNetDevice>();
    Ptr<NrUeRrc> ueRrc = ueLteDevice->GetRrc();
    NS_TEST_ASSERT_MSG_EQ(ueRrc->GetState(), NrUeRrc::CONNECTED_NORMALLY, "Wrong NrUeRrc state!");

    Ptr<NrGnbNetDevice> gnbLteDevice = gnbDevice->GetObject<NrGnbNetDevice>();
    Ptr<NrGnbRrc> gnbRrc = gnbLteDevice->GetRrc();
    uint16_t rnti = ueRrc->GetRnti();
    Ptr<NrUeManager> NrUeManager = gnbRrc->GetUeManager(rnti);
    NS_TEST_ASSERT_MSG_NE(NrUeManager, nullptr, "RNTI " << rnti << " not found in gNB");

    NrUeManager::State ueManagerState = NrUeManager->GetState();
    NS_TEST_ASSERT_MSG_EQ(ueManagerState,
                          NrUeManager::CONNECTED_NORMALLY,
                          "Wrong NrUeManager state!");
    NS_ASSERT_MSG(ueManagerState == NrUeManager::CONNECTED_NORMALLY, "Wrong NrUeManager state!");

    uint16_t ueCellId = ueRrc->GetCellId();
    uint16_t gnbCellId = gnbLteDevice->GetCellId();
    uint8_t ueDlBandwidth = ueRrc->GetDlBandwidth();
    uint8_t gnbDlBandwidth = gnbLteDevice->GetCellIdDlBandwidth(gnbCellId);
    uint8_t ueUlBandwidth = ueRrc->GetUlBandwidth();
    uint8_t gnbUlBandwidth = gnbLteDevice->GetCellIdUlBandwidth(gnbCellId);
    uint8_t ueDlEarfcn = ueRrc->GetDlEarfcn();
    uint8_t gnbDlEarfcn = gnbLteDevice->GetCellIdDlEarfcn(gnbCellId);
    uint8_t ueUlEarfcn = ueRrc->GetUlEarfcn();
    uint8_t gnbUlEarfcn = gnbLteDevice->GetCellIdUlEarfcn(gnbCellId);
    uint64_t ueImsi = ueLteDevice->GetImsi();
    uint64_t gnbImsi = NrUeManager->GetImsi();

    NS_TEST_ASSERT_MSG_EQ(ueImsi, gnbImsi, "inconsistent IMSI");
    NS_TEST_ASSERT_MSG_EQ(ueCellId, gnbCellId, "inconsistent CellId");
    NS_TEST_ASSERT_MSG_EQ(ueDlBandwidth, gnbDlBandwidth, "inconsistent DlBandwidth");
    NS_TEST_ASSERT_MSG_EQ(ueUlBandwidth, gnbUlBandwidth, "inconsistent UlBandwidth");
    NS_TEST_ASSERT_MSG_EQ(ueDlEarfcn, gnbDlEarfcn, "inconsistent DlEarfcn");
    NS_TEST_ASSERT_MSG_EQ(ueUlEarfcn, gnbUlEarfcn, "inconsistent UlEarfcn");

    ObjectMapValue gnbDataRadioBearerMapValue;
    NrUeManager->GetAttribute("DataRadioBearerMap", gnbDataRadioBearerMapValue);
    NS_TEST_ASSERT_MSG_EQ(gnbDataRadioBearerMapValue.GetN(),
                          m_nDedicatedBearers + 1,
                          "wrong num bearers at gNB");

    ObjectMapValue ueDataRadioBearerMapValue;
    ueRrc->GetAttribute("DataRadioBearerMap", ueDataRadioBearerMapValue);
    NS_TEST_ASSERT_MSG_EQ(ueDataRadioBearerMapValue.GetN(),
                          m_nDedicatedBearers + 1,
                          "wrong num bearers at UE");

    auto gnbBearerIt = gnbDataRadioBearerMapValue.Begin();
    auto ueBearerIt = ueDataRadioBearerMapValue.Begin();
    while (gnbBearerIt != gnbDataRadioBearerMapValue.End() &&
           ueBearerIt != ueDataRadioBearerMapValue.End())
    {
        Ptr<NrDataRadioBearerInfo> gnbDrbInfo =
            gnbBearerIt->second->GetObject<NrDataRadioBearerInfo>();
        Ptr<NrDataRadioBearerInfo> ueDrbInfo =
            ueBearerIt->second->GetObject<NrDataRadioBearerInfo>();
        // NS_TEST_ASSERT_MSG_EQ (gnbDrbInfo->m_epsBearer, ueDrbInfo->m_epsBearer, "NrEpsBearer
        // differs");
        NS_TEST_ASSERT_MSG_EQ((uint32_t)gnbDrbInfo->m_epsBearerIdentity,
                              (uint32_t)ueDrbInfo->m_epsBearerIdentity,
                              "epsBearerIdentity differs");
        NS_TEST_ASSERT_MSG_EQ((uint32_t)gnbDrbInfo->m_drbIdentity,
                              (uint32_t)ueDrbInfo->m_drbIdentity,
                              "drbIdentity differs");
        // NS_TEST_ASSERT_MSG_EQ (gnbDrbInfo->m_rlcConfig, ueDrbInfo->m_rlcConfig, "rlcConfig
        // differs");
        NS_TEST_ASSERT_MSG_EQ((uint32_t)gnbDrbInfo->m_logicalChannelIdentity,
                              (uint32_t)ueDrbInfo->m_logicalChannelIdentity,
                              "logicalChannelIdentity differs");
        // NS_TEST_ASSERT_MSG_EQ (gnbDrbInfo->m_logicalChannelConfig,
        // ueDrbInfo->m_logicalChannelConfig, "logicalChannelConfig differs");

        ++gnbBearerIt;
        ++ueBearerIt;
    }
    NS_ASSERT_MSG(gnbBearerIt == gnbDataRadioBearerMapValue.End(), "too many bearers at gNB");
    NS_ASSERT_MSG(ueBearerIt == ueDataRadioBearerMapValue.End(), "too many bearers at UE");
}

void
NrX2HandoverMeasuresTestCase::SaveStats(uint32_t ueIndex)
{
    NS_LOG_FUNCTION(ueIndex);
    for (auto it = m_ueDataVector.at(ueIndex).bearerDataList.begin();
         it != m_ueDataVector.at(ueIndex).bearerDataList.end();
         ++it)
    {
        if (it->dlSink)
        {
            it->dlOldTotalRx = it->dlSink->GetTotalRx();
        }
        if (it->ulSink)
        {
            it->ulOldTotalRx = it->ulSink->GetTotalRx();
        }
    }
}

void
NrX2HandoverMeasuresTestCase::CheckStats(uint32_t ueIndex)
{
    NS_LOG_FUNCTION(ueIndex);
    uint32_t b = 1;
    for (auto it = m_ueDataVector.at(ueIndex).bearerDataList.begin();
         it != m_ueDataVector.at(ueIndex).bearerDataList.end();
         ++it)
    {
        uint32_t dlRx = 0;
        uint32_t ulRx = 0;

        if (it->dlSink)
        {
            dlRx = it->dlSink->GetTotalRx() - it->dlOldTotalRx;
        }

        if (it->ulSink)
        {
            ulRx = it->ulSink->GetTotalRx() - it->ulOldTotalRx;
        }
        double expectedBytes =
            m_udpClientPktSize * (m_statsDuration / m_udpClientInterval).GetDouble();

        NS_LOG_LOGIC("expBytes " << expectedBytes << " dlRx " << dlRx << " ulRx " << ulRx);

        // tolerance
        if (it->dlSink)
        {
            NS_TEST_ASSERT_MSG_GT(dlRx,
                                  0.500 * expectedBytes,
                                  "too few RX bytes in DL, ue=" << ueIndex << ", b=" << b);
        }
        if (it->ulSink)
        {
            NS_TEST_ASSERT_MSG_GT(ulRx,
                                  0.500 * expectedBytes,
                                  "too few RX bytes in UL, ue=" << ueIndex << ", b=" << b);
        }
        ++b;
    }
}

/**
 * \ingroup lte-test
 *
 * \brief NR X2 Handover Measures Test Suite
 */
class NrX2HandoverMeasuresTestSuite : public TestSuite
{
  public:
    NrX2HandoverMeasuresTestSuite();
};

NrX2HandoverMeasuresTestSuite::NrX2HandoverMeasuresTestSuite()
    : TestSuite("nr-x2-handover-measures", Type::SYSTEM)
{
    Time checkInterval = Seconds(1);

    std::string cel1name("ho: 0 -> 1");
    const std::list<CheckPointEvent> cel1{
        CheckPointEvent(Seconds(1), Seconds(10.1), checkInterval, 0, 0),
        CheckPointEvent(Seconds(11), Seconds(17), checkInterval, 0, 1),
    };

    std::string cel2name("ho: 0 -> 1 -> 2");
    const std::list<CheckPointEvent> cel2{
        CheckPointEvent(Seconds(1), Seconds(10.1), checkInterval, 0, 0),
        CheckPointEvent(Seconds(11), Seconds(17.1), checkInterval, 0, 1),
        CheckPointEvent(Seconds(18), Seconds(24), checkInterval, 0, 2),
    };

    std::string cel3name("ho: 0 -> 1 -> 2 -> 3");
    const std::list<CheckPointEvent> cel3{
        CheckPointEvent(Seconds(1), Seconds(10.1), checkInterval, 0, 0),
        CheckPointEvent(Seconds(11), Seconds(17.1), checkInterval, 0, 1),
        CheckPointEvent(Seconds(18), Seconds(24.1), checkInterval, 0, 2),
        CheckPointEvent(Seconds(25), Seconds(37), checkInterval, 0, 3),
    };

    std::string sched = "ns3::NrMacSchedulerTdmaPF";
    std::string ho = "ns3::NrA2A4RsrqHandoverAlgorithm";
    for (auto useIdealRrc : {true, false})
    {
        // nGnbs, nUes, nDBearers, celist, name, useUdp, sched, ho, admitHo, idealRrc
        AddTestCase(new NrX2HandoverMeasuresTestCase(2,
                                                     1,
                                                     0,
                                                     cel1,
                                                     cel1name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::TAKES_FOREVER);
        AddTestCase(new NrX2HandoverMeasuresTestCase(2,
                                                     1,
                                                     1,
                                                     cel1,
                                                     cel1name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::QUICK);
        AddTestCase(new NrX2HandoverMeasuresTestCase(2,
                                                     1,
                                                     2,
                                                     cel1,
                                                     cel1name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::TAKES_FOREVER);
        AddTestCase(new NrX2HandoverMeasuresTestCase(3,
                                                     1,
                                                     0,
                                                     cel2,
                                                     cel2name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::TAKES_FOREVER);
        AddTestCase(new NrX2HandoverMeasuresTestCase(3,
                                                     1,
                                                     1,
                                                     cel2,
                                                     cel2name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::TAKES_FOREVER);
        AddTestCase(new NrX2HandoverMeasuresTestCase(3,
                                                     1,
                                                     2,
                                                     cel2,
                                                     cel2name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::EXTENSIVE);
        AddTestCase(new NrX2HandoverMeasuresTestCase(4,
                                                     1,
                                                     0,
                                                     cel3,
                                                     cel3name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::EXTENSIVE);
        AddTestCase(new NrX2HandoverMeasuresTestCase(4,
                                                     1,
                                                     1,
                                                     cel3,
                                                     cel3name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::TAKES_FOREVER);
        AddTestCase(new NrX2HandoverMeasuresTestCase(4,
                                                     1,
                                                     2,
                                                     cel3,
                                                     cel3name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::TAKES_FOREVER);
    }

    sched = "ns3::NrMacSchedulerTdmaRR";
    for (auto useIdealRrc : {true, false})
    {
        // nGnbs, nUes, nDBearers, celist, name, useUdp, sched, admitHo, idealRrc
        AddTestCase(new NrX2HandoverMeasuresTestCase(2,
                                                     1,
                                                     0,
                                                     cel1,
                                                     cel1name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::EXTENSIVE);
        AddTestCase(new NrX2HandoverMeasuresTestCase(3,
                                                     1,
                                                     0,
                                                     cel2,
                                                     cel2name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::TAKES_FOREVER);
        AddTestCase(new NrX2HandoverMeasuresTestCase(4,
                                                     1,
                                                     0,
                                                     cel3,
                                                     cel3name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::TAKES_FOREVER);
    }

    ho = "ns3::NrA3RsrpHandoverAlgorithm";
    sched = "ns3::NrMacSchedulerTdmaPF";
    for (auto useIdealRrc : {true, false})
    {
        // nGnbs, nUes, nDBearers, celist, name, useUdp, sched, admitHo, idealRrc
        AddTestCase(new NrX2HandoverMeasuresTestCase(2,
                                                     1,
                                                     0,
                                                     cel1,
                                                     cel1name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::EXTENSIVE);
        AddTestCase(new NrX2HandoverMeasuresTestCase(3,
                                                     1,
                                                     0,
                                                     cel2,
                                                     cel2name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::TAKES_FOREVER);
        AddTestCase(new NrX2HandoverMeasuresTestCase(4,
                                                     1,
                                                     0,
                                                     cel3,
                                                     cel3name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::TAKES_FOREVER);
    }

    sched = "ns3::NrMacSchedulerTdmaRR";
    for (auto useIdealRrc : {true, false})
    {
        // nGnbs, nUes, nDBearers, celist, name, useUdp, sched, admitHo, idealRrc
        AddTestCase(new NrX2HandoverMeasuresTestCase(2,
                                                     1,
                                                     0,
                                                     cel1,
                                                     cel1name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::QUICK);
        AddTestCase(new NrX2HandoverMeasuresTestCase(3,
                                                     1,
                                                     0,
                                                     cel2,
                                                     cel2name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::TAKES_FOREVER);
        AddTestCase(new NrX2HandoverMeasuresTestCase(4,
                                                     1,
                                                     0,
                                                     cel3,
                                                     cel3name,
                                                     true,
                                                     sched,
                                                     ho,
                                                     true,
                                                     useIdealRrc),
                    TestCase::Duration::EXTENSIVE);
    }

} // end of NrX2HandoverMeasuresTestSuite ()

/**
 * \ingroup lte-test
 * Static variable for test initialization
 */
static NrX2HandoverMeasuresTestSuite g_NrX2HandoverMeasuresTestSuiteInstance;
