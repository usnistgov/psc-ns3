// Include a header file from your module to test.
/*
 * SPDX-License-Identifier: NIST-Software
 */

#include "ns3/application-container.h"
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
#include "ns3/olsr-helper.h"
#include "ns3/olsrv2-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/onoff-application.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/position-allocator.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/simulator.h"
#include "ns3/ssid.h"
#include "ns3/string.h"
#include "ns3/test.h"
#include "ns3/vector.h"
#include "ns3/wifi-helper.h"
#include "ns3/wifi-mac-helper.h"
#include "ns3/yans-wifi-helper.h"

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
class NhdpTestCase : public TestCase
{
  public:
    NhdpTestCase(std::string name);

    void NeighborChange(std::string context,
                        NeighborStatus neighborStatus,
                        const NeighborTuple& neighborTuple);
    void LinkChange(std::string context, LinkStatus oldLinkStatus, const LinkTuple& linkTuple);
    void TwoHopChange(std::string context, TwoHopStatus twoHopStatus, const TwoHopTuple& linkTuple);
    void Print(Ptr<NhdpClient> client);

  protected:
    std::vector<NeighborTuple> m_neighborChanges;
    std::vector<LinkTuple> m_linkChanges;
    std::vector<TwoHopTuple> m_twoHopChanges;
    std::vector<NeighborTuple> m_symmetricNeighbors;
};

NhdpTestCase::NhdpTestCase(std::string name)
    : TestCase(name)
{
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
    for (const auto& [addr, tuple] : client->GetTwoHopInfoBase())
    {
        NS_LOG_INFO(client->GetNode()->GetId()
                    << " Two hop neighbor " << tuple.m_twoHopAddr << " via " << addr
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
        m_symmetricNeighbors.emplace_back(neighborTuple);
    }
    else if (neighborStatus == NeighborStatus::REMOVED)
    {
        NS_LOG_INFO(context << " Removed neighbor " << neighborTuple.m_neighborAddrList[0]);
    }
    m_neighborChanges.emplace_back(neighborTuple);
}

void
NhdpTestCase::LinkChange(std::string context, LinkStatus oldLinkStatus, const LinkTuple& linkTuple)
{
    NS_LOG_INFO(context << " Link old status " << oldLinkStatus << " new status "
                        << linkTuple.GetLinkStatus() << " " << linkTuple.m_neighborAddrList[0]);
    m_linkChanges.emplace_back(linkTuple);
}

void
NhdpTestCase::TwoHopChange(std::string context,
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
    m_twoHopChanges.emplace_back(twoHopTuple);
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
    AddTestCase(new NhdpFourNodeNhdpTestCase("Four node matrix test with losses"),
                TestCase::Duration::QUICK);
    AddTestCase(new NhdpFourNodeOlsrTestCase("Four node matrix test with OLSRv1"),
                TestCase::Duration::QUICK);
    AddTestCase(new NhdpFourNodeOlsrv2TestCase("Four node matrix test with OLSRv2"),
                TestCase::Duration::QUICK);
}

/**
 * @ingroup nhdp-tests
 * Static variable for test initialization
 */
static NhdpTestSuite s_nhdpTestSuite;
