// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>

#include "ns3/nr-epc-tft.h"
#include "ns3/nr-gnb-net-device.h"
#include "ns3/nr-gnb-rrc.h"
#include "ns3/nr-helper.h"
#include "ns3/nr-point-to-point-epc-helper.h"
#include "ns3/nr-radio-bearer-info.h"
#include "ns3/nr-ue-net-device.h"
#include "ns3/nr-ue-rrc.h"
#include <ns3/core-module.h>
#include <ns3/internet-module.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>
#include <ns3/packet-sink-helper.h>
#include <ns3/packet-sink.h>
#include <ns3/point-to-point-module.h>
#include <ns3/udp-client-server-helper.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrX2HandoverTest");

/**
 * \ingroup nr-test
 *
 * \brief HandoverEvent structure
 */
struct HandoverEvent
{
    Time startTime;                ///< start time
    uint32_t ueDeviceIndex;        ///< UE device index
    uint32_t sourceGnbDeviceIndex; ///< source gNB device index
    uint32_t targetGnbDeviceIndex; ///< target gNB device index
};

/**
 * \ingroup nr-test
 *
 * \brief Test X2 Handover. In this test is used NoOpHandoverAlgorithm and
 * the request for handover is generated manually, and it is not based on measurements.
 */
class NrX2HandoverTestCase : public TestCase
{
  public:
    /**
     *
     *
     * \param nUes number of UEs in the test
     * \param nDedicatedBearers number of bearers to be activated per UE
     * \param handoverEventList
     * \param handoverEventListName
     * \param schedulerType the scheduler type
     * \param admitHo
     * \param useIdealRrc true if the ideal RRC should be used
     */
    NrX2HandoverTestCase(uint32_t nUes,
                         uint32_t nDedicatedBearers,
                         std::list<HandoverEvent> handoverEventList,
                         std::string handoverEventListName,
                         std::string schedulerType,
                         bool admitHo,
                         bool useIdealRrc);

  private:
    /**
     * Build name string
     * \param nUes number of UEs in the test
     * \param nDedicatedBearers number of bearers to be activated per UE
     * \param handoverEventListName
     * \param schedulerType the scheduler type
     * \param admitHo
     * \param useIdealRrc true if the ideal RRC should be used
     * \returns the name string
     */
    static std::string BuildNameString(uint32_t nUes,
                                       uint32_t nDedicatedBearers,
                                       std::string handoverEventListName,
                                       std::string schedulerType,
                                       bool admitHo,
                                       bool useIdealRrc);
    void DoRun() override;
    /**
     * Check connected function
     * \param ueDevice the UE device
     * \param gnbDevice the gNB device
     */
    void CheckConnected(Ptr<NetDevice> ueDevice, Ptr<NetDevice> gnbDevice);

    /**
     * Teleport UE between both gNBs of the test
     * \param ueNode the UE node
     */
    void TeleportUeToMiddle(Ptr<Node> ueNode);

    /**
     * Teleport UE near the target gNB of the handover
     * \param ueNode the UE node
     * \param gnbNode the target gNB node
     */
    void TeleportUeNearTargetGnb(Ptr<Node> ueNode, Ptr<Node> gnbNode);

    uint32_t m_nUes;                              ///< number of UEs in the test
    uint32_t m_nDedicatedBearers;                 ///< number of UEs in the test
    std::list<HandoverEvent> m_handoverEventList; ///< handover event list
    std::string m_handoverEventListName;          ///< handover event list name
    bool m_epc;                                   ///< whether to use EPC
    std::string m_schedulerType;                  ///< scheduler type
    bool m_admitHo;                               ///< whether to admit the handover request
    bool m_useIdealRrc;                           ///< whether to use the ideal RRC
    Ptr<NrHelper> m_nrHelper;                     ///< NR helper
    Ptr<NrPointToPointEpcHelper> m_epcHelper;     ///< EPC helper

    /**
     * \ingroup nr-test
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
     * \ingroup nr-test
     *
     * \brief UeData structure
     */
    struct UeData
    {
        uint32_t id;                          ///< ID
        std::list<BearerData> bearerDataList; ///< bearer ID list
    };

    /**
     * \brief Save stats after handover function
     * \param ueIndex the index of the UE
     */
    void SaveStatsAfterHandover(uint32_t ueIndex);
    /**
     * \brief Check stats a while after handover function
     * \param ueIndex the index of the UE
     */
    void CheckStatsAWhileAfterHandover(uint32_t ueIndex);

    std::vector<UeData> m_ueDataVector; ///< UE data vector

    const Time m_maxHoDuration;        ///< maximum HO duration
    const Time m_statsDuration;        ///< stats duration
    const Time m_udpClientInterval;    ///< UDP client interval
    const uint32_t m_udpClientPktSize; ///< UDP client packet size
};

std::string
NrX2HandoverTestCase::BuildNameString(uint32_t nUes,
                                      uint32_t nDedicatedBearers,
                                      std::string handoverEventListName,
                                      std::string schedulerType,
                                      bool admitHo,
                                      bool useIdealRrc)
{
    std::ostringstream oss;
    oss << " nUes=" << nUes << " nDedicatedBearers=" << nDedicatedBearers << " " << schedulerType
        << " admitHo=" << admitHo << " hoList: " << handoverEventListName;
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

NrX2HandoverTestCase::NrX2HandoverTestCase(uint32_t nUes,
                                           uint32_t nDedicatedBearers,
                                           std::list<HandoverEvent> handoverEventList,
                                           std::string handoverEventListName,
                                           std::string schedulerType,
                                           bool admitHo,
                                           bool useIdealRrc)
    : TestCase(BuildNameString(nUes,
                               nDedicatedBearers,
                               handoverEventListName,
                               schedulerType,
                               admitHo,
                               useIdealRrc)),
      m_nUes(nUes),
      m_nDedicatedBearers(nDedicatedBearers),
      m_handoverEventList(handoverEventList),
      m_handoverEventListName(handoverEventListName),
      m_epc(true),
      m_schedulerType(schedulerType),
      m_admitHo(admitHo),
      m_useIdealRrc(useIdealRrc),
      m_maxHoDuration(Seconds(0.1)),
      m_statsDuration(Seconds(0.1)),
      m_udpClientInterval(Seconds(0.01)),
      m_udpClientPktSize(100)

{
}

void
NrX2HandoverTestCase::DoRun()
{
    NS_LOG_FUNCTION(this << BuildNameString(m_nUes,
                                            m_nDedicatedBearers,
                                            m_handoverEventListName,
                                            m_schedulerType,
                                            m_admitHo,
                                            m_useIdealRrc));

    uint32_t previousSeed = RngSeedManager::GetSeed();
    uint64_t previousRun = RngSeedManager::GetRun();
    Config::Reset();
    // This test is sensitive to random variable stream assignments
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(3);
    Config::SetDefault("ns3::UdpClient::Interval", TimeValue(m_udpClientInterval));
    Config::SetDefault("ns3::UdpClient::MaxPackets", UintegerValue(1000000));
    Config::SetDefault("ns3::UdpClient::PacketSize", UintegerValue(m_udpClientPktSize));
    Config::SetDefault("ns3::NrGnbPhy::TxPower", DoubleValue(30));
    Config::SetDefault("ns3::NrUePhy::TxPower", DoubleValue(23));

    // Disable Uplink Power Control
    Config::SetDefault("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue(false));

    int64_t stream = 1;

    m_nrHelper = CreateObject<NrHelper>();
    // todo:
    // m_nrHelper->SetSchedulerType(m_schedulerType);
    m_nrHelper->SetHandoverAlgorithmType(
        "ns3::NoOpHandoverAlgorithm"); // disable automatic handover
    m_nrHelper->SetAttribute("UseIdealRrc", BooleanValue(m_useIdealRrc));

    NodeContainer gnbNodes;
    gnbNodes.Create(2);
    NodeContainer ueNodes;
    ueNodes.Create(m_nUes);

    if (m_epc)
    {
        m_epcHelper = CreateObject<NrPointToPointEpcHelper>();
        m_nrHelper->SetEpcHelper(m_epcHelper);
    }

    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(-3000, 0, 0)); // gnb0
    positionAlloc->Add(Vector(3000, 0, 0));  // gnb1
    for (uint32_t i = 0; i < m_nUes; i++)
    {
        positionAlloc->Add(Vector(-3000, 100, i));
    }
    MobilityHelper mobility;
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(gnbNodes);
    mobility.Install(ueNodes);

    auto bandwidthAndBWPPair = m_nrHelper->CreateBandwidthParts({{2.8e9, 5e6, 1}}, "UMa");
    auto bandwidthAndBWPPair2 = m_nrHelper->CreateBandwidthParts({{2.9e9, 5e6, 1}}, "UMa");
    NetDeviceContainer gnbDevices;
    gnbDevices.Add(m_nrHelper->InstallGnbDevice(gnbNodes.Get(0), bandwidthAndBWPPair.second));
    gnbDevices.Add(m_nrHelper->InstallGnbDevice(gnbNodes.Get(1), bandwidthAndBWPPair2.second));

    stream += m_nrHelper->AssignStreams(gnbDevices, stream);
    for (auto it = gnbDevices.Begin(); it != gnbDevices.End(); ++it)
    {
        Ptr<NrGnbRrc> gnbRrc = (*it)->GetObject<NrGnbNetDevice>()->GetRrc();
        gnbRrc->SetAttribute("AdmitHandoverRequest", BooleanValue(m_admitHo));
    }

    NetDeviceContainer ueDevices;
    ueDevices = m_nrHelper->InstallUeDevice(
        ueNodes,
        {bandwidthAndBWPPair.second.front(), bandwidthAndBWPPair2.second.front()});
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
        // always true: bool epcDl = true;
        // always true: bool epcUl = true;
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

                // always true: if (epcDl)
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
                // always true: if (epcUl)
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

                Ptr<NrEpcTft> tft = Create<NrEpcTft>();
                // always true: if (epcDl)
                {
                    NrEpcTft::PacketFilter dlpf;
                    dlpf.localPortStart = dlPort;
                    dlpf.localPortEnd = dlPort;
                    tft->Add(dlpf);
                }
                // always true: if (epcUl)
                {
                    NrEpcTft::PacketFilter ulpf;
                    ulpf.remotePortStart = ulPort;
                    ulpf.remotePortEnd = ulPort;
                    tft->Add(ulpf);
                }

                // always true: if (epcDl || epcUl)
                {
                    NrEpsBearer bearer(NrEpsBearer::NGBR_VIDEO_TCP_DEFAULT);
                    m_nrHelper->ActivateDedicatedEpsBearer(ueDevices.Get(u), bearer, tft);
                }
                double d = startTimeSeconds->GetValue();
                Time startTime = Seconds(d);
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
        Simulator::Schedule(maxRrcConnectionEstablishmentDuration,
                            &NrX2HandoverTestCase::CheckConnected,
                            this,
                            *it,
                            gnbDevices.Get(0));
    }

    // schedule handover events and corresponding checks

    Time stopTime = Seconds(0);
    for (auto hoEventIt = m_handoverEventList.begin(); hoEventIt != m_handoverEventList.end();
         ++hoEventIt)
    {
        // Teleport the UE between both gNBs just before the handover starts
        Simulator::Schedule(hoEventIt->startTime - MilliSeconds(10),
                            &NrX2HandoverTestCase::TeleportUeToMiddle,
                            this,
                            ueNodes.Get(hoEventIt->ueDeviceIndex));

        Simulator::Schedule(hoEventIt->startTime,
                            &NrX2HandoverTestCase::CheckConnected,
                            this,
                            ueDevices.Get(hoEventIt->ueDeviceIndex),
                            gnbDevices.Get(hoEventIt->sourceGnbDeviceIndex));

        m_nrHelper->HandoverRequest(hoEventIt->startTime,
                                    ueDevices.Get(hoEventIt->ueDeviceIndex),
                                    gnbDevices.Get(hoEventIt->sourceGnbDeviceIndex),
                                    gnbDevices.Get(hoEventIt->targetGnbDeviceIndex));

        // Once the handover is finished, teleport the UE near the target eNB
        Simulator::Schedule(hoEventIt->startTime + MilliSeconds(40),
                            &NrX2HandoverTestCase::TeleportUeNearTargetGnb,
                            this,
                            ueNodes.Get(hoEventIt->ueDeviceIndex),
                            gnbNodes.Get(m_admitHo ? hoEventIt->targetGnbDeviceIndex
                                                   : hoEventIt->sourceGnbDeviceIndex));

        Time hoEndTime = hoEventIt->startTime + m_maxHoDuration;
        Simulator::Schedule(hoEndTime,
                            &NrX2HandoverTestCase::CheckConnected,
                            this,
                            ueDevices.Get(hoEventIt->ueDeviceIndex),
                            gnbDevices.Get(m_admitHo ? hoEventIt->targetGnbDeviceIndex
                                                     : hoEventIt->sourceGnbDeviceIndex));
        Simulator::Schedule(hoEndTime,
                            &NrX2HandoverTestCase::SaveStatsAfterHandover,
                            this,
                            hoEventIt->ueDeviceIndex);

        Time checkStatsAfterHoTime = hoEndTime + m_statsDuration;
        Simulator::Schedule(checkStatsAfterHoTime,
                            &NrX2HandoverTestCase::CheckStatsAWhileAfterHandover,
                            this,
                            hoEventIt->ueDeviceIndex);
        if (stopTime <= checkStatsAfterHoTime)
        {
            stopTime = checkStatsAfterHoTime + MilliSeconds(1);
        }
    }

    // m_nrHelper->EnableRlcTraces ();
    // m_nrHelper->EnablePdcpTraces();

    Simulator::Stop(stopTime);

    Simulator::Run();

    Simulator::Destroy();

    // Undo changes to default settings
    Config::Reset();
    // Restore the previous settings of RngSeed and RngRun
    RngSeedManager::SetSeed(previousSeed);
    RngSeedManager::SetRun(previousRun);
}

void
NrX2HandoverTestCase::CheckConnected(Ptr<NetDevice> ueDevice, Ptr<NetDevice> gnbDevice)
{
    Ptr<NrUeNetDevice> ueNrDevice = ueDevice->GetObject<NrUeNetDevice>();
    Ptr<NrUeRrc> ueRrc = ueNrDevice->GetRrc();
    NS_TEST_ASSERT_MSG_EQ(ueRrc->GetState(), NrUeRrc::CONNECTED_NORMALLY, "Wrong NrUeRrc state!");

    Ptr<NrGnbNetDevice> nrGnbDevice = gnbDevice->GetObject<NrGnbNetDevice>();
    Ptr<NrGnbRrc> gnbRrc = nrGnbDevice->GetRrc();
    uint16_t rnti = ueRrc->GetRnti();
    Ptr<NrUeManager> ueManager = gnbRrc->GetUeManager(rnti);
    NS_TEST_ASSERT_MSG_NE(ueManager, nullptr, "RNTI " << rnti << " not found in eNB");

    NrUeManager::State ueManagerState = ueManager->GetState();
    NS_TEST_ASSERT_MSG_EQ(ueManagerState,
                          NrUeManager::CONNECTED_NORMALLY,
                          "Wrong NrUeManager state!");
    NS_ASSERT_MSG(ueManagerState == NrUeManager::CONNECTED_NORMALLY, "Wrong NrUeManager state!");

    uint16_t ueCellId = ueRrc->GetCellId();
    std::vector<uint16_t> gnbCellId = nrGnbDevice->GetCellIds();
    bool gnbCellIdFound =
        std::find(gnbCellId.begin(), gnbCellId.end(), ueCellId) != gnbCellId.end();
    NS_TEST_ASSERT_MSG_EQ(gnbCellIdFound, true, "gNB does not contain UE cellId");
    uint8_t ueDlBandwidth = ueRrc->GetDlBandwidth();
    uint8_t gnbDlBandwidth = nrGnbDevice->GetCellIdDlBandwidth(ueCellId);
    uint8_t ueUlBandwidth = ueRrc->GetUlBandwidth();
    uint8_t gnbUlBandwidth = nrGnbDevice->GetCellIdUlBandwidth(ueCellId);
    uint8_t ueDlEarfcn = ueRrc->GetDlEarfcn();
    uint8_t gnbDlEarfcn = nrGnbDevice->GetCellIdDlEarfcn(ueCellId);
    uint8_t ueUlEarfcn = ueRrc->GetUlEarfcn();
    uint8_t gnbUlEarfcn = nrGnbDevice->GetCellIdUlEarfcn(ueCellId);
    uint64_t ueImsi = ueNrDevice->GetImsi();
    uint64_t gnbImsi = ueManager->GetImsi();

    NS_TEST_ASSERT_MSG_EQ(ueImsi, gnbImsi, "inconsistent IMSI");
    NS_TEST_ASSERT_MSG_EQ(ueDlBandwidth, gnbDlBandwidth, "inconsistent DlBandwidth");
    NS_TEST_ASSERT_MSG_EQ(ueUlBandwidth, gnbUlBandwidth, "inconsistent UlBandwidth");
    NS_TEST_ASSERT_MSG_EQ(ueDlEarfcn, gnbDlEarfcn, "inconsistent DlEarfcn");
    NS_TEST_ASSERT_MSG_EQ(ueUlEarfcn, gnbUlEarfcn, "inconsistent UlEarfcn");

    ObjectMapValue gnbDataRadioBearerMapValue;
    ueManager->GetAttribute("DataRadioBearerMap", gnbDataRadioBearerMapValue);
    NS_TEST_ASSERT_MSG_EQ(gnbDataRadioBearerMapValue.GetN(),
                          m_nDedicatedBearers + 1,
                          "wrong num bearers at eNB");

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
        // NS_TEST_ASSERT_MSG_EQ (gnbDrbInfo->m_epsBearer, ueDrbInfo->m_epsBearer, "epsBearer
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
    NS_ASSERT_MSG(gnbBearerIt == gnbDataRadioBearerMapValue.End(), "too many bearers at eNB");
    NS_ASSERT_MSG(ueBearerIt == ueDataRadioBearerMapValue.End(), "too many bearers at UE");
}

void
NrX2HandoverTestCase::TeleportUeToMiddle(Ptr<Node> ueNode)
{
    Ptr<MobilityModel> ueMobility = ueNode->GetObject<MobilityModel>();
    ueMobility->SetPosition(Vector(0.0, 0.0, ueMobility->GetPosition().z));
}

void
NrX2HandoverTestCase::TeleportUeNearTargetGnb(Ptr<Node> ueNode, Ptr<Node> gnbNode)
{
    Ptr<MobilityModel> gnbMobility = gnbNode->GetObject<MobilityModel>();
    Vector pos = gnbMobility->GetPosition();

    Ptr<MobilityModel> ueMobility = ueNode->GetObject<MobilityModel>();
    ueMobility->SetPosition(pos + Vector(0.0, 100.0, 0.0));
}

void
NrX2HandoverTestCase::SaveStatsAfterHandover(uint32_t ueIndex)
{
    for (auto it = m_ueDataVector.at(ueIndex).bearerDataList.begin();
         it != m_ueDataVector.at(ueIndex).bearerDataList.end();
         ++it)
    {
        it->dlOldTotalRx = it->dlSink->GetTotalRx();
        it->ulOldTotalRx = it->ulSink->GetTotalRx();
    }
}

void
NrX2HandoverTestCase::CheckStatsAWhileAfterHandover(uint32_t ueIndex)
{
    uint32_t b = 1;
    for (auto it = m_ueDataVector.at(ueIndex).bearerDataList.begin();
         it != m_ueDataVector.at(ueIndex).bearerDataList.end();
         ++it)
    {
        uint32_t dlRx = it->dlSink->GetTotalRx() - it->dlOldTotalRx;
        uint32_t ulRx = it->ulSink->GetTotalRx() - it->ulOldTotalRx;
        uint32_t expectedBytes =
            m_udpClientPktSize * (m_statsDuration / m_udpClientInterval).GetDouble();

        NS_TEST_ASSERT_MSG_EQ(dlRx,
                              expectedBytes,
                              "too few RX bytes in DL, ue=" << ueIndex << ", b=" << b);
        NS_TEST_ASSERT_MSG_EQ(ulRx,
                              expectedBytes,
                              "too few RX bytes in UL, ue=" << ueIndex << ", b=" << b);
        ++b;
    }
}

/**
 * \ingroup nr-test
 *
 * \brief NR X2 Handover Test Suite.
 *
 * In this test suite, we use NoOpHandoverAlgorithm, i.e. "handover algorithm which does nothing"
 * is used and handover is triggered manually. The automatic handover algorithms (A2A4, A3Rsrp)
 * are not tested.
 *
 * The tests are designed to check that eNB-buffered data received while a handover is in progress
 * is not lost but successfully forwarded. But the test suite doesn't test for possible loss of
 * RLC-buffered data because "lossless" handover is not implemented, and there are other application
 * send patterns (outside of the range tested here) that may incur losses.
 */
class NrX2HandoverTestSuite : public TestSuite
{
  public:
    NrX2HandoverTestSuite();
};

NrX2HandoverTestSuite::NrX2HandoverTestSuite()
    : TestSuite("nr-x2-handover", Type::SYSTEM)
{
    // in the following:
    // fwd means handover from gnb 0 to gnb 1
    // bwd means handover from gnb 1 to gnb 0

    HandoverEvent ue1fwd;
    ue1fwd.startTime = MilliSeconds(100);
    ue1fwd.ueDeviceIndex = 0;
    ue1fwd.sourceGnbDeviceIndex = 0;
    ue1fwd.targetGnbDeviceIndex = 1;

    HandoverEvent ue1bwd;
    ue1bwd.startTime = MilliSeconds(400);
    ue1bwd.ueDeviceIndex = 0;
    ue1bwd.sourceGnbDeviceIndex = 1;
    ue1bwd.targetGnbDeviceIndex = 0;

    HandoverEvent ue1fwdagain;
    ue1fwdagain.startTime = MilliSeconds(700);
    ue1fwdagain.ueDeviceIndex = 0;
    ue1fwdagain.sourceGnbDeviceIndex = 0;
    ue1fwdagain.targetGnbDeviceIndex = 1;

    HandoverEvent ue2fwd;
    ue2fwd.startTime = MilliSeconds(110);
    ue2fwd.ueDeviceIndex = 1;
    ue2fwd.sourceGnbDeviceIndex = 0;
    ue2fwd.targetGnbDeviceIndex = 1;

    HandoverEvent ue2bwd;
    ue2bwd.startTime = MilliSeconds(350);
    ue2bwd.ueDeviceIndex = 1;
    ue2bwd.sourceGnbDeviceIndex = 1;
    ue2bwd.targetGnbDeviceIndex = 0;

    std::string handoverEventList0name("none");
    std::list<HandoverEvent> handoverEventList0;

    std::string handoverEventList1name("1 fwd");
    const std::list<HandoverEvent> handoverEventList1{
        ue1fwd,
    };

    std::string handoverEventList2name("1 fwd & bwd");
    const std::list<HandoverEvent> handoverEventList2{
        ue1fwd,
        ue1bwd,
    };

    std::string handoverEventList3name("1 fwd & bwd & fwd");
    const std::list<HandoverEvent> handoverEventList3{
        ue1fwd,
        ue1bwd,
        ue1fwdagain,
    };

    std::string handoverEventList4name("1+2 fwd");
    const std::list<HandoverEvent> handoverEventList4{
        ue1fwd,
        ue2fwd,
    };

    std::string handoverEventList5name("1+2 fwd & bwd");
    const std::list<HandoverEvent> handoverEventList5{
        ue1fwd,
        ue1bwd,
        ue2fwd,
        ue2bwd,
    };

    // std::string handoverEventList6name("2 fwd");
    // const std::list<HandoverEvent> handoverEventList6{
    //     ue2fwd,
    // };

    // std::string handoverEventList7name("2 fwd & bwd");
    // const std::list<HandoverEvent> handoverEventList7{
    //     ue2fwd,
    //     ue2bwd,
    // };

    std::vector<std::string> schedulers{
        "ns3::NrMacSchedulerTdmaRR",
        "ns3::NrMacSchedulerTdmaPF",
    };

    for (auto schedIt = schedulers.begin(); schedIt != schedulers.end(); ++schedIt)
    {
        for (auto useIdealRrc : {true, false})
        {
            // nUes, nDBearers, helist, name, sched, admitHo, idealRrc
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 0,
                                                 handoverEventList0,
                                                 handoverEventList0name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 0,
                                                 handoverEventList0,
                                                 handoverEventList0name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 5,
                                                 handoverEventList0,
                                                 handoverEventList0name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 5,
                                                 handoverEventList0,
                                                 handoverEventList0name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 0,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 1,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 2,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 0,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 false,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 1,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 false,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 2,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 false,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 0,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 1,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 2,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 0,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 false,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 1,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 false,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 2,
                                                 handoverEventList1,
                                                 handoverEventList1name,
                                                 *schedIt,
                                                 false,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 0,
                                                 handoverEventList2,
                                                 handoverEventList2name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 1,
                                                 handoverEventList2,
                                                 handoverEventList2name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 2,
                                                 handoverEventList2,
                                                 handoverEventList2name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 0,
                                                 handoverEventList3,
                                                 handoverEventList3name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 1,
                                                 handoverEventList3,
                                                 handoverEventList3name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(1,
                                                 2,
                                                 handoverEventList3,
                                                 handoverEventList3name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 0,
                                                 handoverEventList3,
                                                 handoverEventList3name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 1,
                                                 handoverEventList3,
                                                 handoverEventList3name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 2,
                                                 handoverEventList3,
                                                 handoverEventList3name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::QUICK);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 0,
                                                 handoverEventList4,
                                                 handoverEventList4name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 1,
                                                 handoverEventList4,
                                                 handoverEventList4name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 2,
                                                 handoverEventList4,
                                                 handoverEventList4name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 0,
                                                 handoverEventList5,
                                                 handoverEventList5name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 1,
                                                 handoverEventList5,
                                                 handoverEventList5name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(2,
                                                 2,
                                                 handoverEventList5,
                                                 handoverEventList5name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(3,
                                                 0,
                                                 handoverEventList3,
                                                 handoverEventList3name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(3,
                                                 1,
                                                 handoverEventList3,
                                                 handoverEventList3name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(3,
                                                 2,
                                                 handoverEventList3,
                                                 handoverEventList3name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(3,
                                                 0,
                                                 handoverEventList4,
                                                 handoverEventList4name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(3,
                                                 1,
                                                 handoverEventList4,
                                                 handoverEventList4name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(3,
                                                 2,
                                                 handoverEventList4,
                                                 handoverEventList4name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(3,
                                                 0,
                                                 handoverEventList5,
                                                 handoverEventList5name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(3,
                                                 1,
                                                 handoverEventList5,
                                                 handoverEventList5name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrX2HandoverTestCase(3,
                                                 2,
                                                 handoverEventList5,
                                                 handoverEventList5name,
                                                 *schedIt,
                                                 true,
                                                 useIdealRrc),
                        TestCase::Duration::QUICK);
        }
    }
}

/**
 * \ingroup nr-test
 * Static variable for test initialization
 */
static NrX2HandoverTestSuite g_nrX2HandoverTestSuiteInstance;
