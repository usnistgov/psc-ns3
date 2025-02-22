// Include a header file from your module to test.
/*
 * SPDX-License-Identifier: NIST-Software
 */

#include "ns3/application-container.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/net-device-container.h"
#include "ns3/nhdp-client.h"
#include "ns3/nhdp-helper.h"
#include "ns3/nhdp-info-base.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/position-allocator.h"
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

NS_LOG_COMPONENT_DEFINE("NhdpSystemTestSuite");

/**
 * @defgroup nhdp-tests Tests for nhdp
 * @ingroup nhdp
 * @ingroup tests
 */

/**
 * @ingroup nhdp-tests
 * Use controlled topology
 */
class NhdpTestCase : public TestCase
{
  public:
    NhdpTestCase();

  private:
    void DoRun() override;
    NetDeviceContainer CreateAdhocNetwork(NodeContainer c, Ssid ssid);
};

NhdpTestCase::NhdpTestCase()
    : TestCase("Nhdp test case")
{
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
      
    NS_LOG_INFO("Creating nodes...");
    NodeContainer nodes;
    nodes.Create(2);

    NS_LOG_INFO("Creating mobility model...");
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(1.0, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    auto devices = CreateAdhocNetwork(nodes, Ssid("nhdp-example"));

    NS_LOG_INFO("Installing internet stack...");
    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper ipv4;
    auto ipInterfaces = ipv4.AssignManet(devices, Ipv4Address("7.0.0.1"));

    NS_LOG_INFO("Installing applications...");
    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(nodes);

    NS_LOG_INFO("Starting simulation...");
    apps.Start(startTime);
    apps.Stop(stopTime);

    Simulator::Stop(stopTime + Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    NS_TEST_ASSERT_MSG_EQ(true, true, "true doesn't equal true for some reason");
    NS_TEST_ASSERT_MSG_EQ_TOL(0.01, 0.01, 0.001, "Numbers are not equal within tolerance");
}

NetDeviceContainer
NhdpTestCase::CreateAdhocNetwork(NodeContainer c, Ssid ssid)
{
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211a);
    WifiMacHelper wifiMac;
    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    wifiPhy.SetChannel(wifiChannel.Create());
    wifiMac.SetType("ns3::AdhocWifiMac");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue("OfdmRate54Mbps"));
    return wifi.Install(wifiPhy, wifiMac, c);
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
