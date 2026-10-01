/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 */

#include "nr-test-entities.h"

#include "ns3/boolean.h"
#include "ns3/config.h"
#include "ns3/csma-helper.h"
#include "ns3/inet-socket-address.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/log.h"
#include "ns3/nr-epc-gnb-application.h"
#include "ns3/nr-eps-bearer.h"
#include "ns3/nr-point-to-point-epc-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/simulator.h"
#include "ns3/test.h"
#include "ns3/udp-echo-helper.h"
#include "ns3/uinteger.h"
#include <ns3/ipv4-static-routing-helper.h>
#include <ns3/ipv4-static-routing.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrEpcTestS1uDownlink");

/**
 * \ingroup nr-test
 *
 * \brief Custom structure for testing UE downlink data
 */
struct UeDlTestData
{
    /**
     * Constructor
     *
     * \param n number of packets
     * \param s packet size
     */
    UeDlTestData(uint32_t n, uint32_t s);

    uint32_t numPkts; ///< number of packets
    uint32_t pktSize; ///< packet size

    Ptr<PacketSink> serverApp;  ///< Server application
    Ptr<Application> clientApp; ///< Client application
};

UeDlTestData::UeDlTestData(uint32_t n, uint32_t s)
    : numPkts(n),
      pktSize(s)
{
}

/**
 * \ingroup nr-test
 *
 * \brief Custom structure for testing eNodeB downlink data, contains
 * the list of data structures for UEs
 */
struct GnbDlTestData
{
    std::vector<UeDlTestData> ues; ///< list of data structure for different UEs
};

/**
 * \ingroup nr-test
 *
 * \brief NrEpcS1uDlTestCase class
 */
class NrEpcS1uDlTestCase : public TestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the name of the test case instance
     * \param v list of eNodeB downlink test data information
     */
    NrEpcS1uDlTestCase(std::string name, std::vector<GnbDlTestData> v);
    ~NrEpcS1uDlTestCase() override;

  private:
    void DoRun() override;
    std::vector<GnbDlTestData> m_gnbDlTestData; ///< gNB DL test data
};

NrEpcS1uDlTestCase::NrEpcS1uDlTestCase(std::string name, std::vector<GnbDlTestData> v)
    : TestCase(name),
      m_gnbDlTestData(v)
{
}

NrEpcS1uDlTestCase::~NrEpcS1uDlTestCase()
{
}

void
NrEpcS1uDlTestCase::DoRun()
{
    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<Node> pgw = nrEpcHelper->GetPgwNode();

    // allow jumbo packets
    Config::SetDefault("ns3::CsmaNetDevice::Mtu", UintegerValue(30000));
    Config::SetDefault("ns3::PointToPointNetDevice::Mtu", UintegerValue(30000));
    nrEpcHelper->SetAttribute("S1uLinkMtu", UintegerValue(30000));

    // Create a single RemoteHost
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // Create the internet
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    ipv4h.Assign(internetDevices);

    // setup default gateway for the remote hosts
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());

    // hardcoded UE addresses for now
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"),
                                               Ipv4Mask("255.255.255.0"),
                                               1);

    NodeContainer gnbs;
    uint16_t cellIdCounter = 0;
    uint64_t imsiCounter = 0;

    for (auto gnbit = m_gnbDlTestData.begin(); gnbit < m_gnbDlTestData.end(); ++gnbit)
    {
        Ptr<Node> gnb = CreateObject<Node>();
        gnbs.Add(gnb);

        // we test EPC without LTE, hence we use:
        // 1) a CSMA network to simulate the cell
        // 2) a raw socket opened on the CSMA device to simulate the NR socket

        uint16_t cellId = ++cellIdCounter;

        NodeContainer ues;
        ues.Create(gnbit->ues.size());

        NodeContainer cell;
        cell.Add(ues);
        cell.Add(gnb);

        CsmaHelper csmaCell;
        NetDeviceContainer cellDevices = csmaCell.Install(cell);

        // the eNB's CSMA NetDevice acting as an NR NetDevice.
        Ptr<NetDevice> gnbDevice = cellDevices.Get(cellDevices.GetN() - 1);

        // Note that the NrEpcGnbApplication won't care of the actual NetDevice type
        std::vector<uint16_t> cellIds;
        cellIds.push_back(cellId);
        nrEpcHelper->AddGnb(gnb, gnbDevice, cellIds);

        // Plug test RRC entity
        Ptr<NrEpcGnbApplication> gnbApp = gnb->GetApplication(0)->GetObject<NrEpcGnbApplication>();
        NS_ASSERT_MSG(gnbApp, "cannot retrieve NrEpcGnbApplication");
        Ptr<NrEpcTestRrc> rrc = CreateObject<NrEpcTestRrc>();
        gnb->AggregateObject(rrc);
        rrc->SetS1SapProvider(gnbApp->GetS1SapProvider());
        gnbApp->SetS1SapUser(rrc->GetS1SapUser());

        // we install the IP stack on UEs only
        InternetStackHelper internet;
        internet.Install(ues);

        // assign IP address to UEs, and install applications
        for (uint32_t u = 0; u < ues.GetN(); ++u)
        {
            Ptr<NetDevice> ueNrDevice = cellDevices.Get(u);
            Ipv4InterfaceContainer ueIpIface =
                nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueNrDevice));

            Ptr<Node> ue = ues.Get(u);

            // disable IP Forwarding on the UE. This is because we use
            // CSMA broadcast MAC addresses for this test. The problem
            // won't happen with a NrUeNetDevice.
            ue->GetObject<Ipv4>()->SetAttribute("IpForward", BooleanValue(false));

            uint16_t port = 1234;
            PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory",
                                              InetSocketAddress(Ipv4Address::GetAny(), port));
            ApplicationContainer apps = packetSinkHelper.Install(ue);
            apps.Start(Seconds(1.0));
            apps.Stop(Seconds(10.0));
            gnbit->ues[u].serverApp = apps.Get(0)->GetObject<PacketSink>();

            Time interPacketInterval = Seconds(0.01);
            UdpEchoClientHelper client(ueIpIface.GetAddress(0), port);
            client.SetAttribute("MaxPackets", UintegerValue(gnbit->ues[u].numPkts));
            client.SetAttribute("Interval", TimeValue(interPacketInterval));
            client.SetAttribute("PacketSize", UintegerValue(gnbit->ues[u].pktSize));
            apps = client.Install(remoteHost);
            apps.Start(Seconds(2.0));
            apps.Stop(Seconds(10.0));
            gnbit->ues[u].clientApp = apps.Get(0);

            uint64_t imsi = ++imsiCounter;
            nrEpcHelper->AddUe(ueNrDevice, imsi);
            nrEpcHelper->ActivateEpsBearer(ueNrDevice,
                                           imsi,
                                           NrEpcTft::Default(),
                                           NrEpsBearer(NrEpsBearer::NGBR_VIDEO_TCP_DEFAULT));
            Simulator::Schedule(MilliSeconds(10),
                                &NrEpcGnbS1SapProvider::InitialUeMessage,
                                gnbApp->GetS1SapProvider(),
                                imsi,
                                (uint16_t)imsi);
        }
    }

    Simulator::Run();

    for (auto gnbit = m_gnbDlTestData.begin(); gnbit < m_gnbDlTestData.end(); ++gnbit)
    {
        for (auto ueit = gnbit->ues.begin(); ueit < gnbit->ues.end(); ++ueit)
        {
            NS_TEST_ASSERT_MSG_EQ(ueit->serverApp->GetTotalRx(),
                                  (ueit->numPkts) * (ueit->pktSize),
                                  "wrong total received bytes");
        }
    }

    Simulator::Destroy();
}

/**
 * Test that the S1-U interface implementation works correctly
 */
class NrEpcS1uDlTestSuite : public TestSuite
{
  public:
    NrEpcS1uDlTestSuite();

} g_NrEpcS1uDlTestSuiteInstance;

NrEpcS1uDlTestSuite::NrEpcS1uDlTestSuite()
    : TestSuite("nr-epc-s1u-downlink", Type::SYSTEM)
{
    std::vector<GnbDlTestData> v1;
    GnbDlTestData e1;
    UeDlTestData f1(1, 100);
    e1.ues.push_back(f1);
    v1.push_back(e1);
    AddTestCase(new NrEpcS1uDlTestCase("1 eNB, 1UE", v1), TestCase::Duration::QUICK);

    std::vector<GnbDlTestData> v2;
    GnbDlTestData e2;
    UeDlTestData f2_1(1, 100);
    e2.ues.push_back(f2_1);
    UeDlTestData f2_2(2, 200);
    e2.ues.push_back(f2_2);
    v2.push_back(e2);
    AddTestCase(new NrEpcS1uDlTestCase("1 eNB, 2UEs", v2), TestCase::Duration::QUICK);

    std::vector<GnbDlTestData> v3;
    v3.push_back(e1);
    v3.push_back(e2);
    AddTestCase(new NrEpcS1uDlTestCase("2 eNBs", v3), TestCase::Duration::QUICK);

    GnbDlTestData e3;
    UeDlTestData f3_1(3, 50);
    e3.ues.push_back(f3_1);
    UeDlTestData f3_2(5, 1472);
    e3.ues.push_back(f3_2);
    UeDlTestData f3_3(1, 1);
    e3.ues.push_back(f3_2);
    std::vector<GnbDlTestData> v4;
    v4.push_back(e3);
    v4.push_back(e1);
    v4.push_back(e2);
    AddTestCase(new NrEpcS1uDlTestCase("3 eNBs", v4), TestCase::Duration::QUICK);

    std::vector<GnbDlTestData> v5;
    GnbDlTestData e5;
    UeDlTestData f5(10, 3000);
    e5.ues.push_back(f5);
    v5.push_back(e5);
    AddTestCase(new NrEpcS1uDlTestCase("1 eNB, 10 pkts 3000 bytes each", v5),
                TestCase::Duration::QUICK);

    std::vector<GnbDlTestData> v6;
    GnbDlTestData e6;
    UeDlTestData f6(50, 3000);
    e6.ues.push_back(f6);
    v6.push_back(e6);
    AddTestCase(new NrEpcS1uDlTestCase("1 eNB, 50 pkts 3000 bytes each", v6),
                TestCase::Duration::QUICK);

    std::vector<GnbDlTestData> v7;
    GnbDlTestData e7;
    UeDlTestData f7(10, 15000);
    e7.ues.push_back(f7);
    v7.push_back(e7);
    AddTestCase(new NrEpcS1uDlTestCase("1 eNB, 10 pkts 15000 bytes each", v7),
                TestCase::Duration::QUICK);

    std::vector<GnbDlTestData> v8;
    GnbDlTestData e8;
    UeDlTestData f8(100, 15000);
    e8.ues.push_back(f8);
    v8.push_back(e8);
    AddTestCase(new NrEpcS1uDlTestCase("1 eNB, 100 pkts 15000 bytes each", v8),
                TestCase::Duration::QUICK);
}
