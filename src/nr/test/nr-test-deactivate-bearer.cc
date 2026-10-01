// Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
// Author:Gaurav Sathe <gaurav.sathe@tcs.com>

#include "nr-test-deactivate-bearer.h"

#include "ns3/double.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/nr-point-to-point-epc-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/radio-bearer-stats-calculator.h"
#include "ns3/string.h"
#include "ns3/udp-client-server-helper.h"
#include <ns3/boolean.h>
#include <ns3/config.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/enum.h>
#include <ns3/log.h>
#include <ns3/mobility-helper.h>
#include <ns3/net-device-container.h>
#include <ns3/node-container.h>
#include <ns3/nr-bearer-stats-calculator.h>
#include <ns3/nr-eps-bearer.h>
#include <ns3/nr-gnb-net-device.h>
#include <ns3/nr-gnb-phy.h>
#include <ns3/nr-helper.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-ue-phy.h>
#include <ns3/nr-ue-rrc.h>
#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/rng-seed-manager.h>
#include <ns3/simulator.h>
#include <ns3/test.h>
#include <ns3/type-id.h>

#include <iostream>
#include <sstream>
#include <string>

NS_LOG_COMPONENT_DEFINE("NrTestDeactivateBearer");

namespace ns3
{

NrTestBearerDeactivateSuite::NrTestBearerDeactivateSuite()
    : TestSuite("nr-test-deactivate-bearer", Type::SYSTEM)
{
    NS_LOG_INFO("creating NrTestPssFfMacSchedulerSuite");

    bool errorModel = false;
    std::vector<uint16_t> dist_1{1, 2, 3}; // 3 nearby UEs

    std::vector<uint16_t> packetSize_1;

    packetSize_1.push_back(100); // 1
    packetSize_1.push_back(100); // 2
    packetSize_1.push_back(100); // 3

    std::vector<uint32_t> estThrPssDl_1;

    estThrPssDl_1.push_back(0);      // User 0, IMSI 1, will have this bearer disconnected
    estThrPssDl_1.push_back(198000); // User 1 will share the channel
    estThrPssDl_1.push_back(198000); // User 2 will share the channel

    AddTestCase(
        new NrDeactivateBearerTestCase(dist_1, estThrPssDl_1, packetSize_1, 1, errorModel, true),
        TestCase::Duration::QUICK);
}

/**
 * \ingroup nr-test
 * Static variable for test initialization
 */
static NrTestBearerDeactivateSuite lenaTestBearerDeactivateSuite;

std::string
NrDeactivateBearerTestCase::BuildNameString(uint16_t nUser, std::vector<uint16_t> dist)
{
    std::ostringstream oss;
    oss << "distances (m) = [ ";
    for (auto it = dist.begin(); it != dist.end(); ++it)
    {
        oss << *it << " ";
    }
    oss << "]";
    return oss.str();
}

NrDeactivateBearerTestCase::NrDeactivateBearerTestCase(std::vector<uint16_t> dist,
                                                       std::vector<uint32_t> estThrPssDl,
                                                       std::vector<uint16_t> packetSize,
                                                       uint16_t interval,
                                                       bool errorModelEnabled,
                                                       bool useIdealRrc)
    : TestCase(BuildNameString(dist.size(), dist)),
      m_nUser(dist.size()),
      m_dist(dist),
      m_packetSize(packetSize),
      m_interval(interval),
      m_estThrPssDl(estThrPssDl),
      m_errorModelEnabled(errorModelEnabled)
{
}

NrDeactivateBearerTestCase::~NrDeactivateBearerTestCase()
{
}

void
NrDeactivateBearerTestCase::DoRun()
{
    uint32_t originalSeed = RngSeedManager::GetSeed();
    uint32_t originalRun = RngSeedManager::GetRun();
    RngSeedManager::SetSeed(1);
    RngSeedManager::SetRun(1);

    if (!m_errorModelEnabled)
    {
        Config::SetDefault("ns3::NrSpectrumPhy::DataErrorModelEnabled", BooleanValue(false));
    }

    Config::SetDefault("ns3::NrHelper::UseIdealRrc", BooleanValue(true));
    Config::SetDefault("ns3::RadioBearerStatsCalculator::DlRlcOutputFilename",
                       StringValue(CreateTempDirFilename("DlRlcStats.txt")));
    Config::SetDefault("ns3::RadioBearerStatsCalculator::UlRlcOutputFilename",
                       StringValue(CreateTempDirFilename("UlRlcStats.txt")));

    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    nrHelper->SetEpcHelper(nrEpcHelper);

    Ptr<Node> pgw = nrEpcHelper->GetPgwNode();

    // Create a single RemoteHost
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // Create the Internet
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(1500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.001)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    // interface 0 is localhost, 1 is the p2p device
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    // Create Nodes: eNodeB and UE
    NodeContainer gnbNodes;
    NodeContainer ueNodes;
    gnbNodes.Create(1);
    ueNodes.Create(m_nUser);

    // Install Mobility Model
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(gnbNodes);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(ueNodes);

    auto bandwidthAndBWPPair = nrHelper->CreateBandwidthParts({{2.8e9, 5e6, 1}}, "UMa");
    // Create Devices and install them in the Nodes (gNB and UE)
    NetDeviceContainer gnbDevs;
    NetDeviceContainer ueDevs;
    int64_t stream = 1;

    nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerTdmaRR"));
    gnbDevs = nrHelper->InstallGnbDevice(gnbNodes, bandwidthAndBWPPair.second);
    stream += nrHelper->AssignStreams(gnbDevs, stream);

    ueDevs = nrHelper->InstallUeDevice(ueNodes, bandwidthAndBWPPair.second);
    stream += nrHelper->AssignStreams(ueDevs, stream);

    Ptr<NrGnbNetDevice> nrGnbDev = gnbDevs.Get(0)->GetObject<NrGnbNetDevice>();
    Ptr<NrGnbPhy> gnbPhy = nrGnbDev->GetPhy(0);
    gnbPhy->SetAttribute("TxPower", DoubleValue(30.0));
    gnbPhy->SetAttribute("NoiseFigure", DoubleValue(5.0));

    // Set UEs' position and power
    for (int i = 0; i < m_nUser; i++)
    {
        Ptr<ConstantPositionMobilityModel> mm =
            ueNodes.Get(i)->GetObject<ConstantPositionMobilityModel>();
        mm->SetPosition(Vector(m_dist.at(i), 0.0, 0.0));
        Ptr<NrUeNetDevice> nrUeDev = ueDevs.Get(i)->GetObject<NrUeNetDevice>();
        Ptr<NrUePhy> uePhy = nrUeDev->GetPhy(0);
        uePhy->SetAttribute("TxPower", DoubleValue(23.0));
        uePhy->SetAttribute("NoiseFigure", DoubleValue(9.0));
    }

    // Install the IP stack on the UEs
    internet.Install(ueNodes);
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueDevs));

    // Assign IP address to UEs
    for (uint32_t u = 0; u < ueNodes.GetN(); ++u)
    {
        Ptr<Node> ueNode = ueNodes.Get(u);
        // Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(ueNode->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(nrEpcHelper->GetUeDefaultGatewayAddress(), 1);
    }

    // Attach a UE to a gNB
    for (uint32_t i = 0; i < ueDevs.GetN(); i++)
    {
        nrHelper->AttachToGnb(ueDevs.Get(i), gnbDevs.Get(0));
    }
    // Activate an EPS bearer on all UEs

    for (uint32_t u = 0; u < ueNodes.GetN(); ++u)
    {
        Ptr<NetDevice> ueDevice = ueDevs.Get(u);
        NrGbrQosInformation qos;
        qos.gbrDl = (m_packetSize.at(u) + 32) * (1000 / m_interval) *
                    8; // bit/s, considering IP, UDP, RLC, PDCP header size
        qos.gbrUl = (m_packetSize.at(u) + 32) * (1000 / m_interval) * 8;
        qos.mbrDl = qos.gbrDl;
        qos.mbrUl = qos.gbrUl;

        NrEpsBearer::Qci q = NrEpsBearer::GBR_CONV_VOICE;
        NrEpsBearer bearer(q, qos);
        bearer.arp.priorityLevel = 15 - (u + 1);
        bearer.arp.preemptionCapability = true;
        bearer.arp.preemptionVulnerability = true;
        nrHelper->ActivateDedicatedEpsBearer(ueDevice, bearer, NrEpcTft::Default());
    }

    // Install downlink and uplink applications
    uint16_t dlPort = 1234;
    uint16_t ulPort = 2000;
    PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                        InetSocketAddress(Ipv4Address::GetAny(), dlPort));
    ApplicationContainer clientApps;
    ApplicationContainer serverApps;

    for (uint32_t u = 0; u < ueNodes.GetN(); ++u)
    {
        ++ulPort;
        serverApps.Add(
            dlPacketSinkHelper.Install(ueNodes.Get(u))); // receive packets from remotehost
        PacketSinkHelper ulPacketSinkHelper("ns3::UdpSocketFactory",
                                            InetSocketAddress(Ipv4Address::GetAny(), ulPort));
        serverApps.Add(ulPacketSinkHelper.Install(remoteHost)); // receive packets from UEs

        UdpClientHelper dlClient(ueIpIface.GetAddress(u), dlPort); // uplink packets generator
        dlClient.SetAttribute("Interval", TimeValue(MilliSeconds(m_interval)));
        dlClient.SetAttribute("MaxPackets", UintegerValue(1000000));
        dlClient.SetAttribute("PacketSize", UintegerValue(m_packetSize.at(u)));

        UdpClientHelper ulClient(remoteHostAddr, ulPort); // downlink packets generator
        ulClient.SetAttribute("Interval", TimeValue(MilliSeconds(m_interval)));
        ulClient.SetAttribute("MaxPackets", UintegerValue(1000000));
        ulClient.SetAttribute("PacketSize", UintegerValue(m_packetSize.at(u)));

        clientApps.Add(dlClient.Install(remoteHost));
        clientApps.Add(ulClient.Install(ueNodes.Get(u)));
    }

    serverApps.Start(Seconds(0.030));
    clientApps.Start(Seconds(0.030));

    double statsDuration = 1.0;
    double tolerance = 0.1;

    // get ue device pointer for UE-ID 0 IMSI 1 and gnb device pointer
    Ptr<NetDevice> ueDevice = ueDevs.Get(0);
    Ptr<NetDevice> gnbDevice = gnbDevs.Get(0);

    /*
     *   Instantiate De-activation using Simulator::Schedule() method which will initiate bearer
     * de-activation after deActivateTime Instantiate De-activation in sequence (Time const &time,
     * MEM mem_ptr, OBJ obj, T1 a1, T2 a2, T3 a3)
     */
    Time deActivateTime(Seconds(1.5));
    Simulator::Schedule(deActivateTime,
                        &NrHelper::DeActivateDedicatedEpsBearer,
                        nrHelper,
                        ueDevice,
                        gnbDevice,
                        2);

    // enable rlc traffic measurements and start measuring after the bearer from
    // imsi 1 has been disabled (a.k.a. there should no traffic)
    nrHelper->EnableRlcE2eTraces();
    Ptr<NrBearerStatsCalculator> rlcStats = nrHelper->GetRlcStatsCalculator();
    rlcStats->SetAttribute("StartTime", TimeValue(deActivateTime));
    rlcStats->SetAttribute("EpochDuration", TimeValue(Seconds(statsDuration)));

    // stop simulation after 3 seconds
    Simulator::Stop(Seconds(3.0));

    Simulator::Run();

    NS_LOG_INFO("DL - Test with " << m_nUser << " user(s)");
    std::vector<uint64_t> dlDataRxed;
    std::vector<uint64_t> dlDataTxed;
    for (int i = 0; i < m_nUser; i++)
    {
        // get the imsi
        uint64_t imsi = ueDevs.Get(i)->GetObject<NrUeNetDevice>()->GetImsi();
        // get the lcId
        // lcId is hard-coded, since only one dedicated bearer is added
        uint8_t lcId = 4;
        dlDataRxed.push_back(rlcStats->GetDlRxData(imsi, lcId));
        dlDataTxed.push_back(rlcStats->GetDlTxData(imsi, lcId));
        NS_LOG_INFO("\tUser " << i << " dist " << m_dist.at(i) << " imsi " << imsi << " bytes rxed "
                              << (double)dlDataRxed.at(i) << "  thr "
                              << (double)dlDataRxed.at(i) / statsDuration << " ref "
                              << m_estThrPssDl.at(i));
        NS_LOG_INFO("\tUser " << i << " imsi " << imsi << " bytes txed " << (double)dlDataTxed.at(i)
                              << "  thr " << (double)dlDataTxed.at(i) / statsDuration);
    }

    for (int i = 0; i < m_nUser; i++)
    {
        uint64_t imsi = ueDevs.Get(i)->GetObject<NrUeNetDevice>()->GetImsi();

        /*
         * For UE ID-0 IMSI 1, LCID=4 is deactivated hence If traffic seen on it, test case should
         * fail Else For other UE's, test case should validate throughput
         */
        if (imsi == 1)
        {
            NS_TEST_ASSERT_MSG_EQ((double)dlDataTxed.at(i), 0, "Invalid LCID in Statistics ");
        }
        else
        {
            NS_TEST_ASSERT_MSG_EQ_TOL((double)dlDataTxed.at(i) / statsDuration,
                                      m_estThrPssDl.at(i),
                                      m_estThrPssDl.at(i) * tolerance,
                                      " Unfair Throughput!");
        }
    }

    Simulator::Destroy();

    RngSeedManager::SetSeed(originalSeed);
    RngSeedManager::SetRun(originalRun);
}
} // namespace ns3
