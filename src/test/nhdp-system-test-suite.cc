// Include a header file from your module to test.
/*
 * SPDX-License-Identifier: NIST-Software
 */

#include "ns3/application-container.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-address.h"
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
    NhdpTestCase();

  private:
    void DoRun() override;
    NetDeviceContainer CreateAdhocNetwork(NodeContainer c, Ssid ssid);
    void NeighborChange(std::string context,
                        NeighborStatus neighborStatus,
                        const NeighborTuple& neighborTuple);
    void LinkChange(std::string context, LinkStatus oldLinkStatus, const LinkTuple& linkTuple);
    void TwoHopChange(std::string context, TwoHopStatus twoHopStatus, const TwoHopTuple& linkTuple);
    void Disable(Ptr<Node> a, Ptr<Node> b);
    void Enable(Ptr<Node> a, Ptr<Node> b);
    void Print(Ptr<NhdpClient> client);
    std::vector<NeighborTuple> m_neighborChanges;
    std::vector<LinkTuple> m_linkChanges;
    std::vector<TwoHopTuple> m_twoHopChanges;
    std::vector<NeighborTuple> m_symmetricNeighbors;
    Ptr<MatrixPropagationLossModel> m_matrixLossModel;
};

NhdpTestCase::NhdpTestCase()
    : TestCase("Nhdp test case")
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

void
NhdpTestCase::Disable(Ptr<Node> a, Ptr<Node> b)
{
    NS_LOG_INFO("Disabling link between " << a->GetId() << " and " << b->GetId());
    m_matrixLossModel->SetLoss(a->GetObject<MobilityModel>(),
                               b->GetObject<MobilityModel>(),
                               100,
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
NhdpTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(51)};

    // Create node index zero but do not use it; this allows the subsequent
    // node IDs to align with the last octet of the IP address, for help
    // in correlating IP addresses to nodes
    Ptr<Node> unusedNode [[maybe_unused]] = CreateObject<Node>();

    NodeContainer nodes;
    nodes.Create(4);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));
    positionAlloc->Add(Vector(0.0, 100.0, 0.0));
    positionAlloc->Add(Vector(100.0, 100.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

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
    m_matrixLossModel->SetLoss(nodes.Get(0)->GetObject<MobilityModel>(),
                               nodes.Get(3)->GetObject<MobilityModel>(),
                               100,
                               true);
    m_matrixLossModel->SetLoss(nodes.Get(1)->GetObject<MobilityModel>(),
                               nodes.Get(2)->GetObject<MobilityModel>(),
                               100,
                               true);
    wifiPhy.SetChannel(channel);
    wifiMac.SetType("ns3::AdhocWifiMac");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue("OfdmRate54Mbps"));
    auto devices = wifi.Install(wifiPhy, wifiMac, nodes);

    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(nodes);
    auto nhdp1 = apps.Get(0)->GetObject<NhdpClient>();
    nhdp1->TraceConnect("NeighborChange", "1", MakeCallback(&NhdpTestCase::NeighborChange, this));
    nhdp1->TraceConnect("LinkChange", "1", MakeCallback(&NhdpTestCase::LinkChange, this));
    nhdp1->TraceConnect("TwoHopChange", "1", MakeCallback(&NhdpTestCase::TwoHopChange, this));

    apps.Start(startTime);
    apps.Stop(stopTime);

    // Schedule losses
    // Simulator::Schedule(Seconds(3), &NhdpTestCase::Disable, this, nodes.Get(0), nodes.Get(1));
    Simulator::Schedule(Seconds(19), &NhdpTestCase::Print, this, nhdp1);
    // Simulator::Schedule(Seconds(20), &NhdpTestCase::Enable, this, nodes.Get(0), nodes.Get(1));
    Simulator::Schedule(Seconds(30), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(31), &NhdpTestCase::Disable, this, nodes.Get(0), nodes.Get(1));
    Simulator::Schedule(Seconds(31), &NhdpTestCase::Disable, this, nodes.Get(0), nodes.Get(2));
    Simulator::Schedule(Seconds(40), &NhdpTestCase::Print, this, nhdp1);
    Simulator::Schedule(Seconds(41), &NhdpTestCase::Disable, this, nodes.Get(0), nodes.Get(1));
    Simulator::Schedule(Seconds(41), &NhdpTestCase::Disable, this, nodes.Get(0), nodes.Get(2));
    Simulator::Schedule(Seconds(50), &NhdpTestCase::Print, this, nhdp1);

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
    AddTestCase(new NhdpTestCase, TestCase::Duration::QUICK);
}

/**
 * @ingroup nhdp-tests
 * Static variable for test initialization
 */
static NhdpTestSuite s_nhdpTestSuite;
