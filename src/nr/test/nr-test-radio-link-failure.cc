//
// Copyright (c) 2018 Fraunhofer ESK
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Vignesh Babu <ns3-dev@esk.fraunhofer.de>
// Modified by:
//         Zoraze Ali <zoraze.ali@cttc.es> (included both RRC protocol, two
//                                          gNB scenario and UE jump away
//                                          logic)
//

#include "nr-test-radio-link-failure.h"

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/udp-client-server-helper.h"

#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrRadioLinkFailureTest");

/*
 * Test Suite
 */
NrRadioLinkFailureTestSuite::NrRadioLinkFailureTestSuite()
    : TestSuite("nr-radio-link-failure", Type::SYSTEM)
{
    std::vector<Vector> uePositionList;
    std::vector<Vector> gnbPositionList;
    std::vector<Time> checkConnectedList;
    Vector ueJumpAwayPosition;

    uePositionList.emplace_back(10, 0, 0);
    gnbPositionList.emplace_back(0, 0, 0);
    ueJumpAwayPosition = Vector(7000.0, 0.0, 0.0);
    // check before jumping
    checkConnectedList.push_back(Seconds(0.3));
    // check connection after jumping but before T310 timer expiration.
    // This is to make sure that UE stays in connected mode
    // before the expiration of T310 timer.
    checkConnectedList.push_back(Seconds(1));

    // One gNB: Ideal RRC PROTOCOL
    //
    AddTestCase(new NrRadioLinkFailureTestCase(1,
                                               1,
                                               Seconds(2),
                                               true,
                                               uePositionList,
                                               gnbPositionList,
                                               ueJumpAwayPosition,
                                               checkConnectedList),
                TestCase::Duration::QUICK);

    // One eNB: Real RRC PROTOCOL
    AddTestCase(new NrRadioLinkFailureTestCase(1,
                                               1,
                                               Seconds(2),
                                               false,
                                               uePositionList,
                                               gnbPositionList,
                                               ueJumpAwayPosition,
                                               checkConnectedList),
                TestCase::Duration::QUICK);

    // Two eNBs: Ideal RRC PROTOCOL

    // We place the second gNB close to the position where the UE will jump
    gnbPositionList.emplace_back(7020, 0, 0);

    AddTestCase(new NrRadioLinkFailureTestCase(2,
                                               1,
                                               Seconds(2),
                                               true,
                                               uePositionList,
                                               gnbPositionList,
                                               ueJumpAwayPosition,
                                               checkConnectedList),
                TestCase::Duration::QUICK);

    // Two eNBs: Ideal RRC PROTOCOL
    AddTestCase(new NrRadioLinkFailureTestCase(2,
                                               1,
                                               Seconds(2),
                                               false,
                                               uePositionList,
                                               gnbPositionList,
                                               ueJumpAwayPosition,
                                               checkConnectedList),
                TestCase::Duration::QUICK);

} // end of NrRadioLinkFailureTestSuite::NrRadioLinkFailureTestSuite ()

/**
 * \ingroup nr-test
 * Static variable for test initialization
 */
static NrRadioLinkFailureTestSuite g_nrRadioLinkFailureTestSuite;

/*
 * Test Case
 */

std::string
NrRadioLinkFailureTestCase::BuildNameString(uint32_t numGnbs, uint32_t numUes, bool isIdealRrc)
{
    std::ostringstream oss;
    std::string rrcProtocol;
    if (isIdealRrc)
    {
        rrcProtocol = "RRC Ideal";
    }
    else
    {
        rrcProtocol = "RRC Real";
    }
    oss << numGnbs << " eNBs, " << numUes << " UEs, " << rrcProtocol << " Protocol";
    return oss.str();
}

NrRadioLinkFailureTestCase::NrRadioLinkFailureTestCase(uint32_t numGnbs,
                                                       uint32_t numUes,
                                                       Time simTime,
                                                       bool isIdealRrc,
                                                       std::vector<Vector> uePositionList,
                                                       std::vector<Vector> gnbPositionList,
                                                       Vector ueJumpAwayPosition,
                                                       std::vector<Time> checkConnectedList)
    : TestCase(BuildNameString(numGnbs, numUes, isIdealRrc)),
      m_numGnbs(numGnbs),
      m_numUes(numUes),
      m_simTime(simTime),
      m_isIdealRrc(isIdealRrc),
      m_uePositionList(uePositionList),
      m_gnbPositionList(gnbPositionList),
      m_checkConnectedList(checkConnectedList),
      m_ueJumpAwayPosition(ueJumpAwayPosition)
{
    NS_LOG_FUNCTION(this << GetName());
    m_lastState = NrUeRrc::NUM_STATES;
    m_radioLinkFailureDetected = false;
    m_numOfInSyncIndications = 0;
    m_numOfOutOfSyncIndications = 0;
}

NrRadioLinkFailureTestCase::~NrRadioLinkFailureTestCase()
{
    NS_LOG_FUNCTION(this << GetName());
}

void
NrRadioLinkFailureTestCase::DoRun()
{
    // LogLevel logLevel = (LogLevel) (LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_ALL);
    // LogComponentEnable ("NrUeRrc", logLevel);
    // LogComponentEnable ("NrGnbRrc", logLevel);
    // LogComponentEnable ("NrRadioLinkFailureTest", logLevel);

    Config::SetDefault("ns3::MacStatsCalculator::DlOutputFilename",
                       StringValue(CreateTempDirFilename("DlMacStats.txt")));
    Config::SetDefault("ns3::MacStatsCalculator::UlOutputFilename",
                       StringValue(CreateTempDirFilename("UlMacStats.txt")));
    Config::SetDefault("ns3::RadioBearerStatsCalculator::DlRlcOutputFilename",
                       StringValue(CreateTempDirFilename("DlRlcStats.txt")));
    Config::SetDefault("ns3::RadioBearerStatsCalculator::UlRlcOutputFilename",
                       StringValue(CreateTempDirFilename("UlRlcStats.txt")));
    Config::SetDefault("ns3::RadioBearerStatsCalculator::DlPdcpOutputFilename",
                       StringValue(CreateTempDirFilename("DlPdcpStats.txt")));
    Config::SetDefault("ns3::RadioBearerStatsCalculator::UlPdcpOutputFilename",
                       StringValue(CreateTempDirFilename("UlPdcpStats.txt")));
    Config::SetDefault("ns3::PhyStatsCalculator::DlRsrpSinrFilename",
                       StringValue(CreateTempDirFilename("DlRsrpSinrStats.txt")));
    Config::SetDefault("ns3::PhyStatsCalculator::UlSinrFilename",
                       StringValue(CreateTempDirFilename("UlSinrStats.txt")));
    Config::SetDefault("ns3::PhyStatsCalculator::UlInterferenceFilename",
                       StringValue(CreateTempDirFilename("UlInterferenceStats.txt")));
    Config::SetDefault("ns3::PhyRxStatsCalculator::DlRxOutputFilename",
                       StringValue(CreateTempDirFilename("DlRxPhyStats.txt")));
    Config::SetDefault("ns3::PhyRxStatsCalculator::UlRxOutputFilename",
                       StringValue(CreateTempDirFilename("UlRxPhyStats.txt")));
    Config::SetDefault("ns3::PhyTxStatsCalculator::DlTxOutputFilename",
                       StringValue(CreateTempDirFilename("DlTxPhyStats.txt")));
    Config::SetDefault("ns3::PhyTxStatsCalculator::UlTxOutputFilename",
                       StringValue(CreateTempDirFilename("UlTxPhyStats.txt")));

    NS_LOG_FUNCTION(this << GetName());
    uint16_t numBearersPerUe = 1;
    Time simTime = m_simTime;
    double eNodeB_txPower = 43;

    Config::SetDefault("ns3::NrHelper::UseIdealRrc", BooleanValue(m_isIdealRrc));

    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    nrHelper->SetEpcHelper(nrEpcHelper);

    //----power related (equal for all base stations)----
    Config::SetDefault("ns3::NrGnbPhy::TxPower", DoubleValue(eNodeB_txPower));
    Config::SetDefault("ns3::NrUePhy::TxPower", DoubleValue(23));
    Config::SetDefault("ns3::NrUePhy::NoiseFigure", DoubleValue(7));
    Config::SetDefault("ns3::NrGnbPhy::NoiseFigure", DoubleValue(2));
    Config::SetDefault("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue(true));
    Config::SetDefault("ns3::NrUePowerControl::ClosedLoop", BooleanValue(true));
    Config::SetDefault("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue(true));

    //----frequency related----
    auto bandwidthAndBWPPair = nrHelper->CreateBandwidthParts({{1.93e9, 5e6, 1}}, "UMa");

    //----others----
    nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerTdmaPF"));
    Config::SetDefault("ns3::NrAmc::AmcModel", EnumValue(NrAmc::ShannonModel));
    Config::SetDefault("ns3::NrAmc::Ber", DoubleValue(0.01));
    Config::SetDefault("ns3::PfFfMacScheduler::HarqEnabled", BooleanValue(true));

    // Radio link failure detection parameters
    Config::SetDefault("ns3::NrUeRrc::N310", UintegerValue(1));
    Config::SetDefault("ns3::NrUeRrc::N311", UintegerValue(1));
    Config::SetDefault("ns3::NrUeRrc::T310", TimeValue(Seconds(1)));

    // Create the internet
    Ptr<Node> pgw = nrEpcHelper->GetPgwNode();
    // Create a single RemoteHost0x18ab460
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(1500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.010)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    // Create Nodes: eNodeB and UE
    NodeContainer gnbNodes;
    NodeContainer ueNodes;
    gnbNodes.Create(m_numGnbs);
    ueNodes.Create(m_numUes);

    // Mobility
    Ptr<ListPositionAllocator> positionAllocGnb = CreateObject<ListPositionAllocator>();

    for (auto gnbPosIt = m_gnbPositionList.begin(); gnbPosIt != m_gnbPositionList.end(); ++gnbPosIt)
    {
        positionAllocGnb->Add(*gnbPosIt);
    }
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(positionAllocGnb);
    mobility.Install(gnbNodes);

    Ptr<ListPositionAllocator> positionAllocUe = CreateObject<ListPositionAllocator>();

    for (auto uePosIt = m_uePositionList.begin(); uePosIt != m_uePositionList.end(); ++uePosIt)
    {
        positionAllocUe->Add(*uePosIt);
    }

    mobility.SetPositionAllocator(positionAllocUe);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(ueNodes);
    m_ueMobility = ueNodes.Get(0)->GetObject<MobilityModel>();

    // Install NR Devices in gNB and UEs
    NetDeviceContainer gnbDevs;
    NetDeviceContainer ueDevs;

    int64_t randomStream = 1;
    gnbDevs = nrHelper->InstallGnbDevice(gnbNodes, bandwidthAndBWPPair.second);
    randomStream += nrHelper->AssignStreams(gnbDevs, randomStream);
    ueDevs = nrHelper->InstallUeDevice(ueNodes, bandwidthAndBWPPair.second);
    randomStream += nrHelper->AssignStreams(ueDevs, randomStream);

    // Install the IP stack on the UEs
    internet.Install(ueNodes);
    Ipv4InterfaceContainer ueIpIfaces;
    ueIpIfaces = nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueDevs));

    // Attach a UE to a eNB
    nrHelper->AttachToClosestGnb(ueDevs, gnbDevs);

    // Install and start applications on UEs and remote host
    uint16_t dlPort = 10000;
    uint16_t ulPort = 20000;

    DataRateValue dataRateValue = DataRate("18.6Mbps");
    uint64_t bitRate = dataRateValue.Get().GetBitRate();
    uint32_t packetSize = 1024; // bytes
    NS_LOG_DEBUG("bit rate " << bitRate);
    double interPacketInterval = static_cast<double>(packetSize * 8) / bitRate;
    Time udpInterval = Seconds(interPacketInterval);

    NS_LOG_DEBUG("UDP will use application interval " << udpInterval.As(Time::S));

    for (uint32_t u = 0; u < m_numUes; ++u)
    {
        Ptr<Node> ue = ueNodes.Get(u);
        // Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(ue->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(nrEpcHelper->GetUeDefaultGatewayAddress(), 1);

        for (uint32_t b = 0; b < numBearersPerUe; ++b)
        {
            ApplicationContainer ulClientApps;
            ApplicationContainer ulServerApps;
            ApplicationContainer dlClientApps;
            ApplicationContainer dlServerApps;

            ++dlPort;
            ++ulPort;

            NS_LOG_LOGIC("installing UDP DL app for UE " << u + 1);
            UdpClientHelper dlClientHelper(ueIpIfaces.GetAddress(u), dlPort);
            dlClientHelper.SetAttribute("Interval", TimeValue(udpInterval));
            dlClientHelper.SetAttribute("MaxPackets", UintegerValue(1000000));
            dlClientApps.Add(dlClientHelper.Install(remoteHost));

            PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                                InetSocketAddress(Ipv4Address::GetAny(), dlPort));
            dlServerApps.Add(dlPacketSinkHelper.Install(ue));

            NS_LOG_LOGIC("installing UDP UL app for UE " << u + 1);
            UdpClientHelper ulClientHelper(remoteHostAddr, ulPort);
            ulClientHelper.SetAttribute("Interval", TimeValue(udpInterval));
            ulClientHelper.SetAttribute("MaxPackets", UintegerValue(1000000));
            ulClientApps.Add(ulClientHelper.Install(ue));

            PacketSinkHelper ulPacketSinkHelper("ns3::UdpSocketFactory",
                                                InetSocketAddress(Ipv4Address::GetAny(), ulPort));
            ulServerApps.Add(ulPacketSinkHelper.Install(remoteHost));

            Ptr<NrEpcTft> tft = Create<NrEpcTft>();
            NrEpcTft::PacketFilter dlpf;
            dlpf.localPortStart = dlPort;
            dlpf.localPortEnd = dlPort;
            tft->Add(dlpf);
            NrEpcTft::PacketFilter ulpf;
            ulpf.remotePortStart = ulPort;
            ulpf.remotePortEnd = ulPort;
            tft->Add(ulpf);
            NrEpsBearer bearer(NrEpsBearer::NGBR_IMS);
            nrHelper->ActivateDedicatedEpsBearer(ueDevs.Get(u), bearer, tft);

            dlServerApps.Start(Seconds(0.27));
            dlClientApps.Start(Seconds(0.27));
            ulServerApps.Start(Seconds(0.27));
            ulClientApps.Start(Seconds(0.27));

        } // end for b
    }

    nrHelper->EnableTraces();

    for (uint32_t u = 0; u < m_numUes; ++u)
    {
        Simulator::Schedule(m_checkConnectedList.at(u),
                            &NrRadioLinkFailureTestCase::CheckConnected,
                            this,
                            ueDevs.Get(u),
                            gnbDevs);
    }

    Simulator::Schedule(Seconds(0.4),
                        &NrRadioLinkFailureTestCase::JumpAway,
                        this,
                        m_ueJumpAwayPosition);

    // connect custom trace sinks
    Config::Connect(
        "/NodeList/*/DeviceList/*/NrGnbRrc/ConnectionEstablished",
        MakeCallback(&NrRadioLinkFailureTestCase::ConnectionEstablishedGnbCallback, this));
    Config::Connect(
        "/NodeList/*/DeviceList/*/NrUeRrc/ConnectionEstablished",
        MakeCallback(&NrRadioLinkFailureTestCase::ConnectionEstablishedUeCallback, this));
    Config::Connect("/NodeList/*/DeviceList/*/NrUeRrc/StateTransition",
                    MakeCallback(&NrRadioLinkFailureTestCase::UeStateTransitionCallback, this));
    Config::Connect(
        "/NodeList/*/DeviceList/*/NrGnbRrc/NotifyConnectionRelease",
        MakeCallback(&NrRadioLinkFailureTestCase::ConnectionReleaseAtGnbCallback, this));
    Config::Connect("/NodeList/*/DeviceList/*/NrUeRrc/PhySyncDetection",
                    MakeCallback(&NrRadioLinkFailureTestCase::PhySyncDetectionCallback, this));
    Config::Connect("/NodeList/*/DeviceList/*/NrUeRrc/RadioLinkFailure",
                    MakeCallback(&NrRadioLinkFailureTestCase::RadioLinkFailureCallback, this));

    Simulator::Stop(simTime);

    Simulator::Run();
    for (uint32_t u = 0; u < m_numUes; ++u)
    {
        NS_TEST_ASSERT_MSG_EQ(
            m_radioLinkFailureDetected,
            true,
            "Error, UE transitions to idle state for other than radio link failure");
        CheckIdle(ueDevs.Get(u), gnbDevs);
    }
    Simulator::Destroy();
} // end of void NrRadioLinkFailureTestCase::DoRun ()

void
NrRadioLinkFailureTestCase::JumpAway(Vector UeJumpAwayPosition)
{
    NS_LOG_FUNCTION(this);
    // move to a far away location so that transmission errors occur

    m_ueMobility->SetPosition(UeJumpAwayPosition);
}

void
NrRadioLinkFailureTestCase::CheckConnected(Ptr<NetDevice> ueDevice, NetDeviceContainer gnbDevices)
{
    NS_LOG_FUNCTION(ueDevice);

    Ptr<NrUeNetDevice> ueNrDevice = ueDevice->GetObject<NrUeNetDevice>();
    Ptr<NrUeRrc> ueRrc = ueNrDevice->GetRrc();
    NS_TEST_ASSERT_MSG_EQ(ueRrc->GetState(), NrUeRrc::CONNECTED_NORMALLY, "Wrong NrUeRrc state!");
    uint16_t cellId = ueRrc->GetCellId();

    Ptr<NrGnbNetDevice> nrGnbDevice;

    for (auto gnbDevIt = gnbDevices.Begin(); gnbDevIt != gnbDevices.End(); ++gnbDevIt)
    {
        if (((*gnbDevIt)->GetObject<NrGnbNetDevice>())->GetRrc()->HasCellId(cellId))
        {
            nrGnbDevice = (*gnbDevIt)->GetObject<NrGnbNetDevice>();
        }
    }

    NS_TEST_ASSERT_MSG_NE(nrGnbDevice, nullptr, "LTE gNB device not found");
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
    NS_TEST_ASSERT_MSG_EQ(gnbDataRadioBearerMapValue.GetN(), 1 + 1, "wrong num bearers at eNB");

    ObjectMapValue ueDataRadioBearerMapValue;
    ueRrc->GetAttribute("DataRadioBearerMap", ueDataRadioBearerMapValue);
    NS_TEST_ASSERT_MSG_EQ(ueDataRadioBearerMapValue.GetN(), 1 + 1, "wrong num bearers at UE");

    auto gnbBearerIt = gnbDataRadioBearerMapValue.Begin();
    auto ueBearerIt = ueDataRadioBearerMapValue.Begin();
    while (gnbBearerIt != gnbDataRadioBearerMapValue.End() &&
           ueBearerIt != ueDataRadioBearerMapValue.End())
    {
        Ptr<NrDataRadioBearerInfo> gnbDrbInfo =
            gnbBearerIt->second->GetObject<NrDataRadioBearerInfo>();
        Ptr<NrDataRadioBearerInfo> ueDrbInfo =
            ueBearerIt->second->GetObject<NrDataRadioBearerInfo>();
        NS_TEST_ASSERT_MSG_EQ((uint32_t)gnbDrbInfo->m_epsBearerIdentity,
                              (uint32_t)ueDrbInfo->m_epsBearerIdentity,
                              "epsBearerIdentity differs");
        NS_TEST_ASSERT_MSG_EQ((uint32_t)gnbDrbInfo->m_drbIdentity,
                              (uint32_t)ueDrbInfo->m_drbIdentity,
                              "drbIdentity differs");
        NS_TEST_ASSERT_MSG_EQ((uint32_t)gnbDrbInfo->m_logicalChannelIdentity,
                              (uint32_t)ueDrbInfo->m_logicalChannelIdentity,
                              "logicalChannelIdentity differs");

        ++gnbBearerIt;
        ++ueBearerIt;
    }
    NS_ASSERT_MSG(gnbBearerIt == gnbDataRadioBearerMapValue.End(), "too many bearers at eNB");
    NS_ASSERT_MSG(ueBearerIt == ueDataRadioBearerMapValue.End(), "too many bearers at UE");
}

void
NrRadioLinkFailureTestCase::CheckIdle(Ptr<NetDevice> ueDevice, NetDeviceContainer gnbDevices)
{
    NS_LOG_FUNCTION(ueDevice);

    Ptr<NrUeNetDevice> ueNrDevice = ueDevice->GetObject<NrUeNetDevice>();
    Ptr<NrUeRrc> ueRrc = ueNrDevice->GetRrc();
    uint16_t rnti = ueRrc->GetRnti();
    uint32_t numGnbDevices = gnbDevices.GetN();
    bool ueManagerFound = false;

    switch (numGnbDevices)
    {
    // 1 eNB
    case 1:
        NS_TEST_ASSERT_MSG_EQ(ueRrc->GetState(), NrUeRrc::IDLE_CELL_SEARCH, "Wrong NrUeRrc state!");
        ueManagerFound = CheckUeExistAtGnb(rnti, gnbDevices.Get(0));
        NS_TEST_ASSERT_MSG_EQ(ueManagerFound,
                              false,
                              "Unexpected RNTI with value " << rnti << " found in eNB");
        break;
    // 2 eNBs
    case 2:
        NS_TEST_ASSERT_MSG_EQ(ueRrc->GetState(),
                              NrUeRrc::CONNECTED_NORMALLY,
                              "Wrong NrUeRrc state!");
        ueManagerFound = CheckUeExistAtGnb(rnti, gnbDevices.Get(1));
        NS_TEST_ASSERT_MSG_EQ(ueManagerFound,
                              true,
                              "RNTI " << rnti << " is not attached to the eNB");
        break;
    default:
        NS_FATAL_ERROR("The RRC state of the UE in more then 2 gNB scenario is not defined. "
                       "Consider creating more cases");
        break;
    }
}

bool
NrRadioLinkFailureTestCase::CheckUeExistAtGnb(uint16_t rnti, Ptr<NetDevice> gnbDevice)
{
    NS_LOG_FUNCTION(this << rnti);
    Ptr<NrGnbNetDevice> nrGnbDevice = DynamicCast<NrGnbNetDevice>(gnbDevice);
    NS_ABORT_MSG_IF(!nrGnbDevice, "LTE gNB device not found");
    Ptr<NrGnbRrc> gnbRrc = nrGnbDevice->GetRrc();
    bool ueManagerFound = gnbRrc->HasNrUeManager(rnti);
    return ueManagerFound;
}

void
NrRadioLinkFailureTestCase::UeStateTransitionCallback(std::string context,
                                                      uint64_t imsi,
                                                      uint16_t cellId,
                                                      uint16_t rnti,
                                                      NrUeRrc::State oldState,
                                                      NrUeRrc::State newState)
{
    NS_LOG_FUNCTION(this << imsi << cellId << rnti << oldState << newState);
    m_lastState = newState;
}

void
NrRadioLinkFailureTestCase::ConnectionEstablishedGnbCallback(std::string context,
                                                             uint64_t imsi,
                                                             uint16_t cellId,
                                                             uint16_t rnti)
{
    NS_LOG_FUNCTION(this << imsi << cellId << rnti);
}

void
NrRadioLinkFailureTestCase::ConnectionEstablishedUeCallback(std::string context,
                                                            uint64_t imsi,
                                                            uint16_t cellId,
                                                            uint16_t rnti)
{
    NS_LOG_FUNCTION(this << imsi << cellId << rnti);
    NS_TEST_ASSERT_MSG_EQ(m_numOfOutOfSyncIndications,
                          0,
                          "radio link failure detection should start only in RRC CONNECTED state");
    NS_TEST_ASSERT_MSG_EQ(m_numOfInSyncIndications,
                          0,
                          "radio link failure detection should start only in RRC CONNECTED state");
}

void
NrRadioLinkFailureTestCase::ConnectionReleaseAtGnbCallback(std::string context,
                                                           uint64_t imsi,
                                                           uint16_t cellId,
                                                           uint16_t rnti)
{
    NS_LOG_FUNCTION(this << imsi << cellId << rnti);
}

void
NrRadioLinkFailureTestCase::PhySyncDetectionCallback(std::string context,
                                                     uint64_t imsi,
                                                     uint16_t rnti,
                                                     uint16_t cellId,
                                                     std::string type,
                                                     uint8_t count)
{
    NS_LOG_FUNCTION(this << imsi << cellId << rnti);
    if (type == "Notify out of sync")
    {
        m_numOfOutOfSyncIndications = count;
    }
    else if (type == "Notify in sync")
    {
        m_numOfInSyncIndications = count;
    }
}

void
NrRadioLinkFailureTestCase::RadioLinkFailureCallback(std::string context,
                                                     uint64_t imsi,
                                                     uint16_t cellId,
                                                     uint16_t rnti)
{
    NS_LOG_FUNCTION(this << imsi << cellId << rnti);
    NS_LOG_DEBUG("RLF at " << Simulator::Now());
    m_radioLinkFailureDetected = true;
    // The value of N310 is hard coded to the default value 1
    NS_TEST_ASSERT_MSG_EQ(
        m_numOfOutOfSyncIndications,
        1,
        "wrong number of out-of-sync indications detected, check configured value for N310");
    // The value of N311 is hard coded to the default value 1
    NS_TEST_ASSERT_MSG_LT(
        m_numOfInSyncIndications,
        1,
        "wrong number of out-of-sync indications detected, check configured value for N311");
    // Reset the counter for the next RRC connection establishment.
    m_numOfOutOfSyncIndications = 0;
}
