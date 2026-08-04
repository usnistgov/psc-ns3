/*
 * Copyright (c) 2017 Jadavpur University, India
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manoj Kumar Rana <manoj24.rana@gmail.com>
 */

#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv6-static-routing.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-epc-helper.h"
#include "ns3/nr-helper.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/udp-echo-helper.h"

#include <algorithm>

/* *
   * Scenario:  3 UEs, 2 ENBs, 1 Remote Host, UE0<-->gNB0, UE1<-->gNB0, UE2<-->gNB1
                Servers: UE1, UE2, Remote Host
                Client: UE0 (3 clients)
                UDP Echo Packets transmitted between client and server

   * Pass criteria: 1) Every UDP Echo Request and Reply messages sent and received respectively
                       at UE0 must be matched by their UID, source address, destination address,
                       source port and destination port
                    2) Every request reply must follow proper route (e.g. In case of UE0->UE1,
                       packet must travel this route:
UE0->gNB0->PGW->gNB1->UE1->gNB1->PGW->gNB0->UE0) 3) The above check also ensures no redundancy of
the followed route for a packet
* */

using namespace ns3;

/**
 * \ingroup nr-test
 *
 * \brief Nr Ipv6 routing test case.
 */
class NrIpv6RoutingTestCase : public TestCase
{
  public:
    NrIpv6RoutingTestCase();
    ~NrIpv6RoutingTestCase() override;

    /**
     * \brief Initialize testing parameters.
     */
    void Checker();

    /**
     * \brief sent Packets from client's IPv6 interface.
     * \param p packet
     * \param ipv6 Ipv6 object
     * \param interface Ipv6interface from which the packet is transmitted
     */
    void SentAtClient(Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface);

    /**
     * \brief Received Packets at client's IPv6 interface.
     * \param p packet
     * \param ipv6 Ipv6 object
     * \param interface Ipv6interface at which the packet is received
     */
    void ReceivedAtClient(Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface);

    /**
     * \brief Received Packet at pgw from gnb.
     * \param p packet
     */
    void GnbToPgw(Ptr<Packet> p);

    /**
     * \brief Received Packet at pgw from gnb.
     * \param p packet
     */
    void TunToPgw(Ptr<Packet> p);

  private:
    void DoRun() override;
    Ipv6InterfaceContainer m_ueIpIface;   //!< IPv6 interface container for ue
    Ipv6Address m_remoteHostAddr;         //!< remote host address
    std::list<uint64_t> m_pgwUidRxFrmGnb; //!< list of uids of packets received at pgw from gnb
    std::list<uint64_t>
        m_pgwUidRxFrmTun; //!< list of uids of packets received at pgw from tunnel net device

    std::list<Ptr<Packet>> m_clientTxPkts; //!< list of sent packets from client
    std::list<Ptr<Packet>> m_clientRxPkts; //!< list of received packets at client
};

NrIpv6RoutingTestCase::NrIpv6RoutingTestCase()
    : TestCase("Test IPv6 Routing at LTE")
{
}

NrIpv6RoutingTestCase::~NrIpv6RoutingTestCase()
{
}

void
NrIpv6RoutingTestCase::SentAtClient(Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
    Ipv6Header ipv6Header;
    p->PeekHeader(ipv6Header);
    if (ipv6Header.GetNextHeader() == UdpL4Protocol::PROT_NUMBER)
    {
        m_clientTxPkts.push_back(p->Copy());
    }
}

void
NrIpv6RoutingTestCase::ReceivedAtClient(Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
    Ipv6Header ipv6Header;
    p->PeekHeader(ipv6Header);
    if (ipv6Header.GetNextHeader() == UdpL4Protocol::PROT_NUMBER)
    {
        m_clientRxPkts.push_back(p->Copy());
    }
}

void
NrIpv6RoutingTestCase::GnbToPgw(Ptr<Packet> p)
{
    Ipv6Header ipv6Header;
    p->PeekHeader(ipv6Header);
    if (ipv6Header.GetNextHeader() == UdpL4Protocol::PROT_NUMBER)
    {
        m_pgwUidRxFrmGnb.push_back(p->GetUid());
    }
}

void
NrIpv6RoutingTestCase::TunToPgw(Ptr<Packet> p)
{
    Ipv6Header ipv6Header;
    p->PeekHeader(ipv6Header);
    if (ipv6Header.GetNextHeader() == UdpL4Protocol::PROT_NUMBER)
    {
        m_pgwUidRxFrmTun.push_back(p->GetUid());
    }
}

void
NrIpv6RoutingTestCase::Checker()
{
    bool b = false;
    bool check = true;
    // Extract each received reply packet of the client
    for (auto it1 = m_clientRxPkts.begin(); it1 != m_clientRxPkts.end(); it1++)
    {
        Ipv6Header ipv6header1;
        UdpHeader udpHeader1;
        Ptr<Packet> p1 = (*it1)->Copy();
        p1->RemoveHeader(ipv6header1);
        uint64_t uid = p1->GetUid();
        p1->RemoveHeader(udpHeader1);
        // Search each packet in list of sent request packet of the client
        for (auto it2 = m_clientTxPkts.begin(); it2 != m_clientTxPkts.end(); it2++)
        {
            Ptr<Packet> p2 = (*it2)->Copy();
            Ipv6Header ipv6header2;
            p2->RemoveHeader(ipv6header2);
            Ipv6Address sourceAddress = ipv6header2.GetSource();
            Ipv6Address destinationAddress = ipv6header2.GetDestination();
            UdpHeader udpHeader2;
            p2->RemoveHeader(udpHeader2);
            uint16_t sourcePort;
            uint16_t destinationPort;
            sourcePort = udpHeader2.GetSourcePort();
            destinationPort = udpHeader2.GetDestinationPort();
            // Check whether the uids, addresses and ports match
            if ((p2->GetUid() == p1->GetUid()) && sourceAddress == ipv6header1.GetDestination() &&
                destinationAddress == ipv6header1.GetSource() &&
                sourcePort == udpHeader1.GetDestinationPort() &&
                destinationPort == udpHeader1.GetSourcePort())
            {
                b = true;
                break;
            }
        }
        check &= b;
        if (std::find(m_pgwUidRxFrmGnb.begin(), m_pgwUidRxFrmGnb.end(), uid) !=
            m_pgwUidRxFrmGnb.end())
        {
            check &= true;
            m_pgwUidRxFrmGnb.remove(uid);
        }
        if (std::find(m_pgwUidRxFrmTun.begin(), m_pgwUidRxFrmTun.end(), uid) !=
            m_pgwUidRxFrmTun.end())
        {
            check &= true;
            m_pgwUidRxFrmTun.remove(uid);
        }
        b = false;
    }

    NS_TEST_ASSERT_MSG_EQ(check, true, "Failure Happens IPv6 routing of LENA");
    NS_TEST_ASSERT_MSG_EQ(m_clientTxPkts.size(),
                          m_clientRxPkts.size(),
                          "No. of Request and Reply messages mismatch");
    NS_TEST_ASSERT_MSG_EQ(m_pgwUidRxFrmGnb.size(), 0, "Route is not Redundant in Nr IPv6 test");
    NS_TEST_ASSERT_MSG_EQ(m_pgwUidRxFrmTun.size(), 0, "Route is not Redundant in Nr IPv6 test");
}

void
NrIpv6RoutingTestCase::DoRun()
{
    double distance = 60.0;

    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    nrHelper->SetEpcHelper(nrEpcHelper);

    ConfigStore inputConfig;
    inputConfig.ConfigureDefaults();

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
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.010)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);

    NodeContainer ueNodes;
    NodeContainer gnbNodes;
    gnbNodes.Create(2);
    ueNodes.Create(3);

    // Install Mobility Model
    Ptr<ListPositionAllocator> positionAlloc1 = CreateObject<ListPositionAllocator>();
    Ptr<ListPositionAllocator> positionAlloc2 = CreateObject<ListPositionAllocator>();

    positionAlloc1->Add(Vector(distance * 0, 0, 0));
    positionAlloc1->Add(Vector(distance * 0 + 5, 0, 0));
    positionAlloc1->Add(Vector(distance * 1, 0, 0));

    positionAlloc2->Add(Vector(distance * 0, 0.1, 0));
    positionAlloc2->Add(Vector(distance * 1, 0.1, 0));

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(positionAlloc1);
    mobility.Install(ueNodes);

    mobility.SetPositionAllocator(positionAlloc2);
    mobility.Install(gnbNodes);

    // Install the IP stack on the UEs
    internet.Install(ueNodes);

    // Create bandwidth part
    auto bandwidthAndBWPPair = nrHelper->CreateBandwidthParts({{2.8e9, 5e6, 1}}, "UMa");

    // Install NR Devices to the nodes
    NetDeviceContainer nrGnbDevs = nrHelper->InstallGnbDevice(gnbNodes, bandwidthAndBWPPair.second);
    NetDeviceContainer ueNrDevs = nrHelper->InstallUeDevice(ueNodes, bandwidthAndBWPPair.second);

    // Assign IP address to UEs, and install applications
    m_ueIpIface = nrEpcHelper->AssignUeIpv6Address(NetDeviceContainer(ueNrDevs));

    Ipv6StaticRoutingHelper ipv6RoutingHelper;

    for (uint32_t u = 0; u < ueNodes.GetN(); ++u)
    {
        Ptr<Node> ueNode = ueNodes.Get(u);
        // Set the default gateway for the UE
        Ptr<Ipv6StaticRouting> ueStaticRouting =
            ipv6RoutingHelper.GetStaticRouting(ueNode->GetObject<Ipv6>());
        ueStaticRouting->SetDefaultRoute(nrEpcHelper->GetUeDefaultGatewayAddress6(), 1);
    }

    // Attach two UEs at first eNodeB and one UE at second eNodeB
    nrHelper->AttachToGnb(ueNrDevs.Get(0), nrGnbDevs.Get(0));
    nrHelper->AttachToGnb(ueNrDevs.Get(1), nrGnbDevs.Get(0));
    nrHelper->AttachToGnb(ueNrDevs.Get(2), nrGnbDevs.Get(1));

    Ipv6AddressHelper ipv6h;
    ipv6h.SetBase(Ipv6Address("6001:db80::"), Ipv6Prefix(64));
    Ipv6InterfaceContainer internetIpIfaces = ipv6h.Assign(internetDevices);

    internetIpIfaces.SetForwarding(0, true);
    internetIpIfaces.SetDefaultRouteInAllNodes(0);

    Ptr<Ipv6StaticRouting> remoteHostStaticRouting =
        ipv6RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv6>());
    remoteHostStaticRouting
        ->AddNetworkRouteTo("7777:f00d::", Ipv6Prefix(64), internetIpIfaces.GetAddress(0, 1), 1, 0);

    // interface 0 is localhost, 1 is the p2p device
    m_remoteHostAddr = internetIpIfaces.GetAddress(1, 1);

    // Install and start applications on UEs and remote host
    UdpEchoServerHelper echoServer1(10);
    UdpEchoServerHelper echoServer2(11);
    UdpEchoServerHelper echoServer3(12);

    ApplicationContainer serverApps = echoServer1.Install(remoteHost);
    serverApps.Add(echoServer2.Install(ueNodes.Get(1)));
    serverApps.Add(echoServer3.Install(ueNodes.Get(2)));

    serverApps.Start(Seconds(4.0));
    serverApps.Stop(Seconds(12.0));

    UdpEchoClientHelper echoClient1(m_remoteHostAddr, 10);
    UdpEchoClientHelper echoClient2(m_ueIpIface.GetAddress(1, 1), 11);
    UdpEchoClientHelper echoClient3(m_ueIpIface.GetAddress(2, 1), 12);

    echoClient1.SetAttribute("MaxPackets", UintegerValue(1000));
    echoClient1.SetAttribute("Interval", TimeValue(Seconds(0.2)));
    echoClient1.SetAttribute("PacketSize", UintegerValue(1024));

    echoClient2.SetAttribute("MaxPackets", UintegerValue(1000));
    echoClient2.SetAttribute("Interval", TimeValue(Seconds(0.2)));
    echoClient2.SetAttribute("PacketSize", UintegerValue(1024));

    echoClient3.SetAttribute("MaxPackets", UintegerValue(1000));
    echoClient3.SetAttribute("Interval", TimeValue(Seconds(0.2)));
    echoClient3.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps1 = echoClient1.Install(ueNodes.Get(0));
    ApplicationContainer clientApps2 = echoClient2.Install(ueNodes.Get(0));
    ApplicationContainer clientApps3 = echoClient3.Install(ueNodes.Get(0));

    clientApps1.Start(Seconds(4.0));
    clientApps1.Stop(Seconds(6.0));

    clientApps2.Start(Seconds(6.1));
    clientApps2.Stop(Seconds(8.0));

    clientApps3.Start(Seconds(8.1));
    clientApps3.Stop(Seconds(10.0));

    // Set Cllback for Client Sent and Received packets
    Ptr<Ipv6L3Protocol> ipL3 = (ueNodes.Get(0))->GetObject<Ipv6L3Protocol>();
    ipL3->TraceConnectWithoutContext("Tx",
                                     MakeCallback(&NrIpv6RoutingTestCase::SentAtClient, this));
    ipL3->TraceConnectWithoutContext("Rx",
                                     MakeCallback(&NrIpv6RoutingTestCase::ReceivedAtClient, this));

    // Set Callback at SgwPgWApplication of epc to get the packets from gnb and from tunnel net
    // device
    Ptr<Application> appPgw = pgw->GetApplication(0);
    appPgw->TraceConnectWithoutContext("RxFromS1u",
                                       MakeCallback(&NrIpv6RoutingTestCase::GnbToPgw, this));
    appPgw->TraceConnectWithoutContext("RxFromTun",
                                       MakeCallback(&NrIpv6RoutingTestCase::TunToPgw, this));

    Simulator::Schedule(Time(Seconds(12.0)), &NrIpv6RoutingTestCase::Checker, this);

    Simulator::Stop(Seconds(14));
    Simulator::Run();

    Simulator::Destroy();
}

/**
 * \brief test suite 1
 */
class NrIpv6RoutingTestSuite : public TestSuite
{
  public:
    NrIpv6RoutingTestSuite();
};

NrIpv6RoutingTestSuite::NrIpv6RoutingTestSuite()
    : TestSuite("nr-ipv6-routing-test", Type::UNIT)
{
    AddTestCase(new NrIpv6RoutingTestCase, TestCase::Duration::QUICK);
}

/**
 * \ingroup nr-test
 * Static variable for test initialization
 */
static NrIpv6RoutingTestSuite g_nripv6testsuite;
