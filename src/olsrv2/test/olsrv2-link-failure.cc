// SPDX-License-Identifier: GPL-2.0-only

#include "ns3/error-model.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/log.h"
#include "ns3/nhdp-client.h"
#include "ns3/nhdp-helper.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/olsrv2-helper.h"
#include "ns3/simple-channel.h"
#include "ns3/simple-net-device.h"
#include "ns3/simulator.h"
#include "ns3/test.h"

#include <iomanip>
#include <iostream>

using namespace ns3;
using namespace nhdp;

NS_LOG_COMPONENT_DEFINE("Olsrv2LinkFailureTest");

// Generation of trace files is disabled by default but can be enabled with the constants
// below for testing.
constexpr bool g_linkTrace = true;     // generates olsrv2-failure-nhdp-link-change-trace.txt
constexpr bool g_neighborTrace = true; // generates olsrv2-failure-nhdp-neighbor-change-trace.txt
constexpr bool g_twoHopTrace = true;   // generates olsrv2-failure-nhdp-two-hop-change-trace.txt
constexpr bool g_lostNeighborTrace =
    true; // generates olsrv2-failure-nhdp-lost-neighbor-change-trace.txt

/**
 * \defgroup olsrv2-tests Tests for NHDP
 * \ingroup olsrv2
 * \ingroup tests
 */

/**
 * \ingroup olsrv2-tests
 *
 * This test creates a three-node network, allows it to form, then breaks a link,
 * and finally restores the link.  This verifies basic NHDP operation (without any
 * link quality support).
 *
 *        n1 <--------------> n2 <--------------> n3
 *    7.0.0.1/32            7.0.0.2/32          7.0.0.3/32
 *                  |
 *                  |
 *          link breaks at 10 s
 *          link recovers at 25 s
 */
class Olsrv2LinkFailureTest : public TestCase
{
  public:
    Olsrv2LinkFailureTest();

  private:
    void DoRun() override;
    void LinkFailure(Ptr<SimpleChannel> channel, Ptr<SimpleNetDevice> d1, Ptr<SimpleNetDevice> d2);
    void LinkRecovery(Ptr<SimpleChannel> channel, Ptr<SimpleNetDevice> d1, Ptr<SimpleNetDevice> d2);
    void CheckNumLostNeighborAdded(Time start, Time stop, uint32_t numLostNeighbor);
    void CheckNumLostNeighborRemoved(Time start, Time stop, uint32_t numLostNeighbor);
};

Olsrv2LinkFailureTest::Olsrv2LinkFailureTest()
    : TestCase("OLSRv2 three node test that creates a link blockage from 10-25 s")
{
}

void
Olsrv2LinkFailureTest::DoRun()
{
    Time startTime{Seconds(1)};
    Time stopTime{Seconds(30)};

    // Create node index zero but do not use it; this allows the subsequent
    // node IDs to align with the last octet of the IP address, for help
    // in correlating IP addresses to nodes
    Ptr<Node> unusedNode [[maybe_unused]] = CreateObject<Node>();

    NodeContainer nodes;
    nodes.Create(3);

    auto d1 = CreateObject<SimpleNetDevice>();
    nodes.Get(0)->AddDevice(d1);
    auto d2 = CreateObject<SimpleNetDevice>();
    nodes.Get(1)->AddDevice(d2);
    auto d3 = CreateObject<SimpleNetDevice>();
    nodes.Get(2)->AddDevice(d3);

    auto channel = CreateObject<SimpleChannel>();
    d1->SetChannel(channel);
    d1->SetNode(nodes.Get(0));
    d2->SetChannel(channel);
    d2->SetNode(nodes.Get(1));
    d3->SetChannel(channel);
    d3->SetNode(nodes.Get(2));
    NetDeviceContainer devices;
    devices.Add(d1);
    devices.Add(d2);
    devices.Add(d3);

    // Use the SimpleChannel::BlackList feature to block n1 from communicating with n3
    channel->BlackList(d1, d3);
    channel->BlackList(d3, d1);

    Olsrv2Helper olsr;
    Ipv4StaticRoutingHelper staticRouting;
    Ipv4ListRoutingHelper list;
    list.Add(staticRouting, 0);
    list.Add(olsr, 10);

    InternetStackHelper internet;
    internet.SetRoutingHelper(list);
    internet.Install(nodes);

    Ipv4AddressHelper addressHelper;
    addressHelper.AssignManet(devices, Ipv4Address("7.0.0.1"));

    NhdpHelper nhdpHelper;
    ApplicationContainer apps = nhdpHelper.Install(nodes);

    Simulator::Schedule(Seconds(10), &Olsrv2LinkFailureTest::LinkFailure, this, channel, d1, d2);
    Simulator::Schedule(Seconds(25), &Olsrv2LinkFailureTest::LinkRecovery, this, channel, d1, d2);

    AsciiTraceHelper ascii;
    // Enable extra tracing if needed (see variables declared above)
    if (g_linkTrace)
    {
        auto linkChangeStream = ascii.CreateFileStream("olsrv2-failure-nhdp-link-change-trace.txt");
        nhdpHelper.EnableLinkChangeTrace(nodes, linkChangeStream);
    }
    if (g_neighborTrace)
    {
        auto neighborChangeStream =
            ascii.CreateFileStream("olsrv2-failure-nhdp-neighbor-change-trace.txt");
        nhdpHelper.EnableNeighborChangeTrace(nodes, neighborChangeStream);
    }
    if (g_twoHopTrace)
    {
        auto twoHopChangeStream =
            ascii.CreateFileStream("olsrv2-failure-nhdp-two-hop-change-trace.txt");
        nhdpHelper.EnableTwoHopChangeTrace(nodes, twoHopChangeStream);
    }
    if (g_lostNeighborTrace)
    {
        auto lostNeighborChangeStream =
            ascii.CreateFileStream("olsrv2-failure-nhdp-lost-neighbor-change-trace.txt");
        nhdpHelper.EnableLostNeighborChangeTrace(nodes, lostNeighborChangeStream);
    }
    Simulator::Stop(stopTime);
    Simulator::Run();

    // Check that each of nodes 1 and 2 added a lost neighbor around time 15 s and
    // removed a lost neighbor around time 21 s
    CheckNumLostNeighborAdded(Seconds(15), Seconds(16), 2);
    CheckNumLostNeighborRemoved(Seconds(22), Seconds(23), 2);
}

void
Olsrv2LinkFailureTest::LinkFailure(Ptr<SimpleChannel> channel,
                                   Ptr<SimpleNetDevice> d1,
                                   Ptr<SimpleNetDevice> d2)
{
    NS_LOG_DEBUG("Blocking link from " << d1->GetNode()->GetId() << " to "
                                       << d2->GetNode()->GetId());
    channel->BlackList(d1, d2);
    channel->BlackList(d2, d1);
}

void
Olsrv2LinkFailureTest::LinkRecovery(Ptr<SimpleChannel> channel,
                                    Ptr<SimpleNetDevice> d1,
                                    Ptr<SimpleNetDevice> d2)
{
    NS_LOG_DEBUG("Unblocking link from " << d1->GetNode()->GetId() << " to "
                                         << d2->GetNode()->GetId());
    channel->UnBlackList(d1, d2);
    channel->UnBlackList(d2, d1);
}

void
Olsrv2LinkFailureTest::CheckNumLostNeighborAdded(Time start, Time stop, uint32_t numLostNeighbor)
{
    // To be completed
}

void
Olsrv2LinkFailureTest::CheckNumLostNeighborRemoved(Time start, Time stop, uint32_t numLostNeighbor)
{
    // To be completed
}

/**
 * \ingroup olsrv2-tests
 * TestSuite for module olsrv2
 */
class Olsrv2LinkFailureTestSuite : public TestSuite
{
  public:
    Olsrv2LinkFailureTestSuite();
};

Olsrv2LinkFailureTestSuite::Olsrv2LinkFailureTestSuite()
    : TestSuite("olsrv2-link-failure", Type::UNIT)
{
    AddTestCase(new Olsrv2LinkFailureTest, TestCase::Duration::QUICK);
}

/**
 * \ingroup olsrv2-tests
 * Static variable for test initialization
 */
static Olsrv2LinkFailureTestSuite g_olsrv2TestSuite;
