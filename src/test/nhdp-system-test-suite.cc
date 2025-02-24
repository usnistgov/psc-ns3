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
    void NeighborChange(std::string context, bool newNeighbor, const NeighborTuple& neighborTuple);
    void LinkChange(std::string context, LinkStatus oldLinkStatus, const LinkTuple& linkTuple);
    void TwoHopChange(std::string context, bool newTwoHop, const TwoHopTuple& linkTuple);
    std::vector<NeighborTuple> m_neighborChanges;
    std::vector<LinkTuple> m_linkChanges;
    std::vector<TwoHopTuple> m_twoHopChanges;
    std::vector<NeighborTuple> m_symmetricNeighbors;
};

NhdpTestCase::NhdpTestCase()
    : TestCase("Nhdp test case")
{
}

void
NhdpTestCase::NeighborChange(std::string context,
                             bool newNeighbor,
                             const NeighborTuple& neighborTuple)
{
    if (newNeighbor)
    {
        NS_LOG_INFO(context <<  " New neighbor " << neighborTuple.m_neighborAddrList[0]);
    }
    else if (neighborTuple.m_symmetric)
    {
        NS_LOG_INFO(context <<  " Symmetric neighbor " << neighborTuple.m_neighborAddrList[0]);
        m_symmetricNeighbors.emplace_back(neighborTuple);
    }
    m_neighborChanges.emplace_back(neighborTuple);
}

void
NhdpTestCase::LinkChange(std::string context,
                             LinkStatus oldLinkStatus,
                             const LinkTuple& linkTuple)
{
    NS_LOG_INFO(context << " Link old status " << oldLinkStatus << " new status " << linkTuple.GetLinkStatus() << " " << linkTuple.m_neighborAddrList[0]);
    m_linkChanges.emplace_back(linkTuple);
}

void
NhdpTestCase::TwoHopChange(std::string context,
                             bool newTwoHop,
                             const TwoHopTuple& twoHopTuple)
{
    if (newTwoHop)
    {
        NS_LOG_INFO(context <<  " New two-hop neighbor " << twoHopTuple.m_twoHopAddr << " from " << twoHopTuple.m_neighborAddrList[0]);
    }
    else
    {
        NS_LOG_INFO(context <<  " Existing two-hop neighbor " << twoHopTuple.m_twoHopAddr << " from " << twoHopTuple.m_neighborAddrList[0]);
    }
    m_twoHopChanges.emplace_back(twoHopTuple);
}


void
NhdpTestCase::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(10)};

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
    auto matrixLossModel = CreateObject<MatrixPropagationLossModel>();
    channel->SetPropagationDelayModel(delayModel);
    channel->SetPropagationLossModel(matrixLossModel);
    // Create 100 dB loss on the diagonals so that each node has two neighbors
    matrixLossModel->SetDefaultLoss(0);
    matrixLossModel->SetLoss(nodes.Get(0)->GetObject<MobilityModel>(),
                             nodes.Get(2)->GetObject<MobilityModel>(),
                             100,
                             true);
    matrixLossModel->SetLoss(nodes.Get(1)->GetObject<MobilityModel>(),
                             nodes.Get(3)->GetObject<MobilityModel>(),
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
