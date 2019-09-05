/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public service. You may use,
 * copy and distribute copies of the software in any medium, provided that you
 * keep intact this entire notice. You may improve,modify and create derivative
 * works of the software or any portion of the software, and you may copy and
 * distribute such modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and nature of
 * any such change. Please explicitly acknowledge the National Institute of
 * Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
 * AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
 * OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
 * ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF,
 * INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY,
 * OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

/**
 * \file intel-http-model-test.cc
 * \ingroup psc-tests
 *
 * IntelHttpServer/IntelHttpClient test suite
 */

#include <ns3/core-module.h>
#include <ns3/internet-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/psc-module.h>
#include <ns3/simulator.h>
#include <ns3/test.h>
#include <ns3/log.h>

NS_LOG_COMPONENT_DEFINE ("IntelHttpModelTestSuite");

/**
 * \ingroup psc-tests
 * \defgroup intel-http-model-tests Tests for IntelHttpServer and IntelHttpClient
 */

namespace ns3 {
namespace tests {

Ptr<ConstantRandomVariable>
MakeConstantStream (double constant)
{
  auto stream = CreateObject<ConstantRandomVariable> ();
  stream->SetAttribute ("Constant", DoubleValue (constant));
  return stream;
}

/**
 * \ingroup intel-http-model-tests
 *
 * Base class for most test cases
 * set up two nodes connected via a point-to-point network
 */
class IntelHttpBaseTestCase : public TestCase
{
public:
  IntelHttpBaseTestCase (const std::string &name);
  virtual ~IntelHttpBaseTestCase () = default;

protected:
  const Time DEFAULT_STOP_TIME{Seconds (100)};

  NodeContainer nodes;
  NetDeviceContainer devices;
  Ipv4InterfaceContainer interfaces;

  Ptr<psc::IntelHttpServer> server;
  Ptr<psc::IntelHttpClient> client;

private:
  void DoSetup (void) override;
};

IntelHttpBaseTestCase::IntelHttpBaseTestCase (const std::string &name) : TestCase (name)
{
}

void
IntelHttpBaseTestCase::DoSetup (void)
{
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Gbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0ms"));

  devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  interfaces = address.Assign (devices);

  server = CreateObject<psc::IntelHttpServer> ();
  server->SetStartTime (Seconds (0));
  server->SetStopTime (DEFAULT_STOP_TIME);
  nodes.Get (0)->AddApplication (server);

  client = CreateObject<psc::IntelHttpClient> ();
  // Just to make sure the server is started
  client->SetStartTime (Seconds (1));
  client->SetStopTime (DEFAULT_STOP_TIME);
  client->SetAttribute ("RemoteAddress", AddressValue (interfaces.GetAddress (0)));
  nodes.Get (1)->AddApplication (client);
}

/**
 * \ingroup intel-http-model-tests
 *
 * Tests that the events in the model happen in the order defined by the
 * flow chart
 */
class IntelHttpTestCaseEventOrder : public IntelHttpBaseTestCase
{
  Time m_mainObjectRequest;
  Time m_mainObjectRequestReceived;
  Time m_mainObjectSent;
  Time m_mainObjectReceive;
  Time m_embeddedObjectRequest;
  Time m_embeddedObjectRequestReceived;
  Time m_embeddedObjectSent;
  Time m_embeddedObjectReceive;
  Time m_nextSessionMainObjectRequest;

public:
  IntelHttpTestCaseEventOrder (void);

  void RequestMainObject (Ptr<const Packet> /* unused */);
  void ReceiveMainObjectRequest (Ptr<const Packet> /* unused */, const Address & /* unused */);
  void SendMainObject (Ptr<const Packet> /* unused */);
  void ReceiveMainObject (Ptr<const Packet> /* unused */, const Address & /* unused */);
  void RequestEmbeddedObject (Ptr<const Packet> /* unused */);
  void ReceiveEmbeddedObjectRequest (Ptr<const Packet> /* unused */, const Address & /* unused */);
  void SendEmbeddedObject (Ptr<const Packet> /* unused */);
  void ReceiveEmbeddedObject (Ptr<const Packet> /* unused */, const Address & /* unused */);

  void DoRun (void) override;
};

IntelHttpTestCaseEventOrder::IntelHttpTestCaseEventOrder (void)
    : IntelHttpBaseTestCase ("Intel HTTP Model Test Case - Verify Event Order")
{
}

void
IntelHttpTestCaseEventOrder::RequestMainObject (Ptr<const Packet> /* unused */)
{
  if (m_mainObjectRequest.IsZero ())
    {
      // First session
      m_mainObjectRequest = Simulator::Now ();
    }
  else if (m_nextSessionMainObjectRequest.IsZero ())
    {
      m_nextSessionMainObjectRequest = Simulator::Now ();

      NS_TEST_ASSERT_MSG_GT (m_nextSessionMainObjectRequest, m_mainObjectRequest,
                             "Second session should start after the first");
      NS_TEST_ASSERT_MSG_GT_OR_EQ (
          m_nextSessionMainObjectRequest, m_embeddedObjectReceive,
          "Second session should start after the last event in the first session");
      Simulator::Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Client requested more than two main objects");
    }
}

void
IntelHttpTestCaseEventOrder::ReceiveMainObjectRequest (Ptr<const Packet> /* unused */,
                                                       const Address & /* unused */)
{
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectRequestReceived.IsZero (), true,
                         "Server received request for main object several times");
  m_mainObjectRequestReceived = Simulator::Now ();
  NS_TEST_ASSERT_MSG_LT_OR_EQ (m_mainObjectRequest, m_mainObjectRequestReceived,
                               "Client must have requested the main object before this event");
}

void
IntelHttpTestCaseEventOrder::SendMainObject (Ptr<const Packet> /* unused */)
{
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectSent.IsZero (), true,
                         "Server transmitted main object several times");
  m_mainObjectSent = Simulator::Now ();
  NS_TEST_ASSERT_MSG_LT_OR_EQ (
      m_mainObjectRequestReceived, m_mainObjectSent,
      "Server must have received a request the main object before this event");
}

void
IntelHttpTestCaseEventOrder::ReceiveMainObject (Ptr<const Packet> /* unused */,
                                                const Address & /* unused */)
{
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectReceive.IsZero (), true,
                         "Client received main object several times");
  m_mainObjectReceive = Simulator::Now ();
  NS_TEST_ASSERT_MSG_LT_OR_EQ (m_mainObjectSent, m_mainObjectReceive,
                               "Server must sent the main object before it could be received");
}

void
IntelHttpTestCaseEventOrder::RequestEmbeddedObject (Ptr<const Packet> /* unused */)
{
  NS_TEST_ASSERT_MSG_EQ (
      m_embeddedObjectRequest.IsZero (), true,
      "Client requested several embedded objects. Make sure the test is configured correctly");
  m_embeddedObjectRequest = Simulator::Now ();
  NS_TEST_ASSERT_MSG_LT_OR_EQ (
      m_mainObjectReceive, m_embeddedObjectRequest,
      "Client must have received the main object before it could request the embedded object");
}

void
IntelHttpTestCaseEventOrder::ReceiveEmbeddedObjectRequest (Ptr<const Packet> /* unused */,
                                                           const Address & /* unused */)
{
  NS_TEST_ASSERT_MSG_EQ (m_embeddedObjectRequestReceived.IsZero (), true,
                         "Server received several request for embedded objects. Make sure the test "
                         "is configured correctly");
  m_embeddedObjectRequestReceived = Simulator::Now ();
  NS_TEST_ASSERT_MSG_LT_OR_EQ (
      m_embeddedObjectRequest, m_embeddedObjectRequestReceived,
      "Client must have sent a request for the embedded object before the server could receive it");
}

void
IntelHttpTestCaseEventOrder::SendEmbeddedObject (Ptr<const Packet> /* unused */)
{
  NS_TEST_ASSERT_MSG_EQ (
      m_embeddedObjectSent.IsZero (), true,
      "Server transmitted several embedded objects. Make sure the test is configured correctly");
  m_embeddedObjectSent = Simulator::Now ();
  NS_TEST_ASSERT_MSG_LT_OR_EQ (
      m_embeddedObjectRequestReceived, m_embeddedObjectSent,
      "Server must have received a request for the embedded object before it may be sent");
}
void
IntelHttpTestCaseEventOrder::ReceiveEmbeddedObject (Ptr<const Packet> /* unused */,
                                                    const Address & /* unused */)
{
  NS_TEST_ASSERT_MSG_EQ (
      m_embeddedObjectReceive.IsZero (), true,
      "Client received several embedded objects. Make sure the test is configured correctly");
  m_embeddedObjectReceive = Simulator::Now ();
  NS_TEST_ASSERT_MSG_LT_OR_EQ (m_embeddedObjectSent, m_embeddedObjectReceive,
                               "Server must have sent embedded object before it may be received");
}

void
IntelHttpTestCaseEventOrder::DoRun (void)
{
  // Keep it simple so we don't have to track multiple Embedded Object requests
  const auto EMBEDDED_OBJECT_COUNT = 1.0;

  // Make sure we don't get a cache hit for our Embedded Object
  const auto CACHE_THRESHOLD = 0.0;

  // Make sure we at least sent one Embedded Object Request
  client->SetAttribute ("CacheThreshold", DoubleValue (CACHE_THRESHOLD));
  server->SetAttribute ("EmbeddedObjectAmountRvs",
                        PointerValue (MakeConstantStream (EMBEDDED_OBJECT_COUNT)));

  // Use the specific traces to track where we are in the flow chart
  client->TraceConnectWithoutContext (
      "TxRequestMainObject", MakeCallback (&IntelHttpTestCaseEventOrder::RequestMainObject, this));
  client->TraceConnectWithoutContext (
      "TxRequestEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseEventOrder::RequestEmbeddedObject, this));
  client->TraceConnectWithoutContext (
      "RxMainObject", MakeCallback (&IntelHttpTestCaseEventOrder::ReceiveMainObject, this));
  client->TraceConnectWithoutContext (
      "RxEmbeddedObject", MakeCallback (&IntelHttpTestCaseEventOrder::ReceiveEmbeddedObject, this));

  server->TraceConnectWithoutContext (
      "RxRequestMainObject",
      MakeCallback (&IntelHttpTestCaseEventOrder::ReceiveMainObjectRequest, this));
  server->TraceConnectWithoutContext (
      "RxRequestEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseEventOrder::ReceiveEmbeddedObjectRequest, this));
  server->TraceConnectWithoutContext (
      "TxMainObject", MakeCallback (&IntelHttpTestCaseEventOrder::SendMainObject, this));
  server->TraceConnectWithoutContext (
      "TxEmbeddedObject", MakeCallback (&IntelHttpTestCaseEventOrder::SendEmbeddedObject, this));

  // The delays should not have a significant impact on the test
  // but there's no need to leave them enabled
  auto constantZeroStream = MakeConstantStream (0);
  client->SetAttribute ("ParseTimeRvs", PointerValue (constantZeroStream));
  client->SetAttribute ("ReadTimeRvs", PointerValue (constantZeroStream));
  server->SetAttribute ("EmbeddedObjectIatRvs", PointerValue (constantZeroStream));

  Simulator::Stop (DEFAULT_STOP_TIME);
  Simulator::Run ();
  Simulator::Destroy ();

  // Make sure every event occurred
  // The asserts in each event callback will have handled the order
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectRequest.IsZero (), false,
                         "Client never requested main object");
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectRequestReceived.IsZero (), false,
                         "Server never received request for main object");
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectSent.IsZero (), false, "Server never sent main object");
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectReceive.IsZero (), false, "Client never received main object");
  NS_TEST_ASSERT_MSG_EQ (m_embeddedObjectRequest.IsZero (), false,
                         "Client never requested the embedded object");
  NS_TEST_ASSERT_MSG_EQ (m_embeddedObjectRequestReceived.IsZero (), false,
                         "Server never received request for the embedded object");
  NS_TEST_ASSERT_MSG_EQ (m_embeddedObjectSent.IsZero (), false,
                         "Server never sent the embedded object");
  NS_TEST_ASSERT_MSG_EQ (m_embeddedObjectReceive.IsZero (), false,
                         "Client never received the embedded object");
  NS_TEST_ASSERT_MSG_EQ (m_nextSessionMainObjectRequest.IsZero (), false,
                         "Client never started next session");
}

/**
 * \ingroup intel-http-model-tests
 *
 * Tests if the number of and the count of general Tx/Rx trace calls
 * matches the number of the specific trace types
 * (main and embedded object requests/responses)
 *
 * Also tests if each of the given message types is the correct size
 */
class IntelHttpTestCaseTxRxCountSizes : public IntelHttpBaseTestCase
{
  /**
   * Fixed size for the client's requests for both main
   * and embedded objects
   */
  const uint32_t OBJECT_REQUEST_SIZE{128u};

  /**
   * Fixed size of each main object
   */
  const uint32_t MAIN_OBJECT_SIZE{512u};

  /**
   * Fixed size of each embedded object
   */
  const uint32_t EMBEDDED_OBJECT_SIZE{2048u};

  /**
   * Total count of transmissions from the client.
   * Should match the number of Main and Embedded Object requests
   */
  int m_txCountClient{0};

  /**
   * Total count of transmissions received the client.
   * Should match the number of Main and Embedded Objects
   */
  int m_rxCountClient{0};

  /**
   * Total count of transmissions from the server.
   * Should match the number of Main and Embedded Object transmissions.
   */
  int m_txCountServer{0};

  /**
   * Total count of transmissions received the server.
   * Should match the number of Main and Embedded Object requests
   */
  int m_rxCountServer{0};

  /**
   * Total requests for Main Objects (pages) sent by the client
   */
  int m_mainObjectRequestsSent{0};

  /**
   * Total requests for Main Objects (pages) received by the server
   */
  int m_mainObjectRequestsReceived{0};

  /**
   * Total Main Objects (pages) sent by the server
   */
  int m_mainObjectsSent{0};

  /**
   * Total Main Objects (pages) received by the client
   */
  int m_mainObjectsReceived{0};

  /**
   * Total requests for Embedded Objects sent by the client
   */
  int m_embeddedObjectRequestsSent{0};

  /**
   * Total requests for Embedded Objects received by the server
   */
  int m_embeddedObjectRequestsReceived{0};

  /**
   * Total Embedded Objects sent by the server
   */
  int m_embeddedObjectsSent{0};

  /**
   * Total Embedded Objects received by the client
   */
  int m_embeddedObjectsReceived{0};

public:
  IntelHttpTestCaseTxRxCountSizes (void);
  void TxClient (Ptr<const Packet> /*unused*/);
  void RxClient (Ptr<const Packet> /*unused*/, const Address & /* unused */);

  void TxServer (Ptr<const Packet> /*unused*/);
  void RxServer (Ptr<const Packet> /*unused*/, const Address & /* unused */);

  void RequestMainObject (Ptr<const Packet> packet);
  void ReceiveMainObjectRequest (Ptr<const Packet> packet, const Address & /* unused */);

  void SendMainObject (Ptr<const Packet> packet);
  void ReceiveMainObject (Ptr<const Packet> packet, const Address & /* unused */);

  void RequestEmbeddedObject (Ptr<const Packet> packet);
  void ReceiveEmbeddedObjectRequest (Ptr<const Packet> packet, const Address & /* unused */);

  void SendEmbeddedObject (Ptr<const Packet> packet);
  void ReceiveEmbeddedObject (Ptr<const Packet> packet, const Address & /* unused */);

  void DoRun (void) override;
};

IntelHttpTestCaseTxRxCountSizes::IntelHttpTestCaseTxRxCountSizes (void)
    : IntelHttpBaseTestCase ("Intel HTTP Model Test Case - Verify Request and Response Sizes, "
                             "Overall and Specific counts")
{
}

void
IntelHttpTestCaseTxRxCountSizes::TxClient (Ptr<const Packet> /*unused*/)
{
  m_txCountClient++;
}

void
IntelHttpTestCaseTxRxCountSizes::RxClient (Ptr<const Packet> /*unused*/,
                                           const Address & /* unused */)
{
  m_rxCountClient++;
}

void
IntelHttpTestCaseTxRxCountSizes::TxServer (Ptr<const Packet> /*unused*/)
{
  m_txCountServer++;
}

void
IntelHttpTestCaseTxRxCountSizes::RxServer (Ptr<const Packet> /*unused*/,
                                           const Address & /* unused */)
{
  m_rxCountServer++;
}

void
IntelHttpTestCaseTxRxCountSizes::RequestMainObject (Ptr<const Packet> packet)
{
  NS_TEST_ASSERT_MSG_EQ (packet->GetSize (), OBJECT_REQUEST_SIZE,
                         "Client's request for main object is incorrect size");
  m_mainObjectRequestsSent++;
}

void
IntelHttpTestCaseTxRxCountSizes::ReceiveMainObjectRequest (Ptr<const Packet> packet,
                                                           const Address & /*unused*/)
{
  NS_TEST_ASSERT_MSG_EQ (packet->GetSize (), OBJECT_REQUEST_SIZE,
                         "Server received request for main object of incorrect size");
  m_mainObjectRequestsReceived++;
}

void
IntelHttpTestCaseTxRxCountSizes::SendMainObject (Ptr<const Packet> packet)
{
  NS_TEST_ASSERT_MSG_EQ (packet->GetSize (), MAIN_OBJECT_SIZE,
                         "Server's main object response is incorrect size");
  m_mainObjectsSent++;
}

void
IntelHttpTestCaseTxRxCountSizes::ReceiveMainObject (Ptr<const Packet> packet,
                                                    const Address & /*unused*/)
{
  NS_TEST_ASSERT_MSG_EQ (packet->GetSize (), MAIN_OBJECT_SIZE,
                         "Client received main object of incorrect size");
  m_mainObjectsReceived++;
}

void
IntelHttpTestCaseTxRxCountSizes::RequestEmbeddedObject (Ptr<const Packet> packet)
{
  NS_TEST_ASSERT_MSG_EQ (packet->GetSize (), OBJECT_REQUEST_SIZE,
                         "Client's request for embedded object is incorrect size");
  m_embeddedObjectRequestsSent++;
}

void
IntelHttpTestCaseTxRxCountSizes::ReceiveEmbeddedObjectRequest (Ptr<const Packet> packet,
                                                               const Address & /*unused*/)
{
  NS_TEST_ASSERT_MSG_EQ (packet->GetSize (), OBJECT_REQUEST_SIZE,
                         "Server received request for embedded object of incorrect size");
  m_embeddedObjectRequestsReceived++;
}

void
IntelHttpTestCaseTxRxCountSizes::SendEmbeddedObject (Ptr<const Packet> packet)
{
  NS_TEST_ASSERT_MSG_EQ (packet->GetSize (), EMBEDDED_OBJECT_SIZE,
                         "Server's embedded object response is incorrect size");
  m_embeddedObjectsSent++;
}

void
IntelHttpTestCaseTxRxCountSizes::ReceiveEmbeddedObject (Ptr<const Packet> packet,
                                                        const Address & /*unused*/)
{
  NS_TEST_ASSERT_MSG_EQ (packet->GetSize (), EMBEDDED_OBJECT_SIZE,
                         "Client received embedded object of incorrect size");
  m_embeddedObjectsReceived++;
}

void
IntelHttpTestCaseTxRxCountSizes::DoRun (void)
{
  // Fix request/response sizes
  client->SetAttribute ("RequestSizeRvs", PointerValue (MakeConstantStream (OBJECT_REQUEST_SIZE)));
  server->SetAttribute ("HtmlSizeRvs", PointerValue (MakeConstantStream (MAIN_OBJECT_SIZE)));
  server->SetAttribute ("EmbeddedObjectSizeRvs",
                        PointerValue (MakeConstantStream (EMBEDDED_OBJECT_SIZE)));

  // Make sure we don't miss our Embedded Object Request
  const auto CACHE_THRESHOLD = 0.0;
  client->SetAttribute ("CacheThreshold", DoubleValue (CACHE_THRESHOLD));

  // Connect ALL the Tx/Rx traces
  client->TraceConnectWithoutContext (
      "Tx", MakeCallback (&IntelHttpTestCaseTxRxCountSizes::TxClient, this));
  client->TraceConnectWithoutContext (
      "TxRequestMainObject",
      MakeCallback (&IntelHttpTestCaseTxRxCountSizes::RequestMainObject, this));
  client->TraceConnectWithoutContext (
      "TxRequestEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseTxRxCountSizes::RequestEmbeddedObject, this));

  client->TraceConnectWithoutContext (
      "Rx", MakeCallback (&IntelHttpTestCaseTxRxCountSizes::RxClient, this));
  client->TraceConnectWithoutContext (
      "RxMainObject", MakeCallback (&IntelHttpTestCaseTxRxCountSizes::ReceiveMainObject, this));
  client->TraceConnectWithoutContext (
      "RxEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseTxRxCountSizes::ReceiveEmbeddedObject, this));

  server->TraceConnectWithoutContext (
      "Tx", MakeCallback (&IntelHttpTestCaseTxRxCountSizes::TxServer, this));
  server->TraceConnectWithoutContext (
      "TxMainObject", MakeCallback (&IntelHttpTestCaseTxRxCountSizes::SendMainObject, this));
  server->TraceConnectWithoutContext (
      "TxEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseTxRxCountSizes::SendEmbeddedObject, this));

  server->TraceConnectWithoutContext (
      "Rx", MakeCallback (&IntelHttpTestCaseTxRxCountSizes::RxServer, this));
  server->TraceConnectWithoutContext (
      "RxRequestMainObject",
      MakeCallback (&IntelHttpTestCaseTxRxCountSizes::ReceiveMainObjectRequest, this));
  server->TraceConnectWithoutContext (
      "RxRequestEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseTxRxCountSizes::ReceiveEmbeddedObjectRequest, this));

  // This test does not depend on only one run occurring
  // so we will actually run to this stop time
  Simulator::Stop (DEFAULT_STOP_TIME);
  Simulator::Run ();
  Simulator::Destroy ();

  //  Make sure we got at least one transmission/receive of each type
  NS_TEST_ASSERT_MSG_GT (m_txCountServer, 0, "Server never transmitted");
  NS_TEST_ASSERT_MSG_GT (m_rxCountServer, 0, "Server never received anything");

  NS_TEST_ASSERT_MSG_GT (m_txCountClient, 0, "Client never transmitted");
  NS_TEST_ASSERT_MSG_GT (m_rxCountClient, 0, "Client never received anything");

  NS_TEST_ASSERT_MSG_GT (m_mainObjectRequestsSent, 0, "Client never requested a main object");
  NS_TEST_ASSERT_MSG_GT (m_mainObjectRequestsReceived, 0,
                         "Server never received a request for a main object");

  NS_TEST_ASSERT_MSG_GT (m_mainObjectsSent, 0, "Server never sent a main object");
  NS_TEST_ASSERT_MSG_GT (m_mainObjectsReceived, 0, "Client never received a main object");

  NS_TEST_ASSERT_MSG_GT (m_embeddedObjectRequestsSent, 0,
                         "Client never requested an embedded object");
  NS_TEST_ASSERT_MSG_GT (m_embeddedObjectRequestsSent, 0,
                         "Server never received a request for an embedded object");

  NS_TEST_ASSERT_MSG_GT (m_embeddedObjectsSent, 0, "Server never sent an embedded object");
  NS_TEST_ASSERT_MSG_GT (m_embeddedObjectsReceived, 0, "Client never received an embedded object");

  // Verify transmission/receive totals
  NS_TEST_ASSERT_MSG_EQ (
      m_mainObjectRequestsSent + m_embeddedObjectRequestsSent, m_txCountClient,
      "Client Tx Total should equal number of main + embedded objects requested");
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectsReceived + m_embeddedObjectsReceived, m_rxCountClient,
                         "Client Rx Total should equal number of main + embedded objects received");

  NS_TEST_ASSERT_MSG_EQ (
      m_mainObjectsSent + m_embeddedObjectsSent, m_txCountServer,
      "Server Tx Total should equal number of main + embedded objects transmitted");
  NS_TEST_ASSERT_MSG_EQ (
      m_mainObjectRequestsReceived + m_embeddedObjectRequestsReceived, m_rxCountServer,
      "Server Rx Total should equal number of main + embedded object requests received");
}

/**
 * \ingroup intel-http-model-tests
 *
 * Tests the Cache Hit/Miss traces for both the number of times their called
 * and the 'objectsLeft' parameter
 */
class IntelHttpTestCaseCacheTotal : public IntelHttpBaseTestCase
{
  // To not rely on the cache hit/miss traces
  // to determine if all of the embedded objects have
  // been received, stop when the model repeats
  // (since we've already verified event order)
  // So, flip this flag once we start the first run
  bool m_isFirstMainObject{true};

  // Set our count of embedded objects big enough to get a few cache hits/misses
  const uint16_t EMBEDDED_OBJECTS{40u};
  // Running count of received/cached embedded objects
  unsigned int m_countHit{0u};
  unsigned int m_countMiss{0u};
  unsigned int m_countRemaining{EMBEDDED_OBJECTS};

public:
  IntelHttpTestCaseCacheTotal (void);

  void RequestMainObject (Ptr<const Packet> /*unused*/);
  void CacheHit (uint16_t currentObject);
  void CacheMiss (uint16_t currentObject);

  void DoRun (void) override;
};

IntelHttpTestCaseCacheTotal::IntelHttpTestCaseCacheTotal (void)
    : IntelHttpBaseTestCase ("Intel HTTP Model Test Case - Verify Cache Hit Count Added With "
                             "Cache Miss Count is The Count of Embedded Objects")
{
}

void
IntelHttpTestCaseCacheTotal::RequestMainObject (Ptr<const Packet> /*unused*/)
{
  if (m_isFirstMainObject)
    {
      m_isFirstMainObject = false;
    }
  else
    {
      Simulator::Stop ();
    }
}

void
IntelHttpTestCaseCacheTotal::CacheHit (uint16_t currentObject)
{
  // Verify remainingObjects before adjusting count since it includes the current object
  // leave out the miss count, since they haven't been received yet (they've just been requested)
  // and replace it with the count we've actually received
  NS_TEST_ASSERT_MSG_EQ (currentObject, EMBEDDED_OBJECTS - m_countRemaining,
                         "remainingObjects has incorrect object count in CacheHit trace");
  m_countHit++;
  m_countRemaining--;
}

void
IntelHttpTestCaseCacheTotal::CacheMiss (uint16_t currentObject)
{
  // Verify remainingObjects before adjusting count since it includes the current object
  // include the hit count since all cache hits should have been processed by now
  NS_TEST_ASSERT_MSG_EQ (currentObject, EMBEDDED_OBJECTS - m_countRemaining,
                         "remainingObjects has incorrect object count in CacheHit trace");
  m_countMiss++;
  m_countRemaining--;
}

void
IntelHttpTestCaseCacheTotal::DoRun (void)
{
  // This cache threshold should be high enough to give us at least one
  // cache hit & one cache miss
  const auto CACHE_THRESHOLD = .5;
  client->SetAttribute ("CacheThreshold", DoubleValue (CACHE_THRESHOLD));

  client->TraceConnectWithoutContext (
      "TxRequestMainObject", MakeCallback (&IntelHttpTestCaseCacheTotal::RequestMainObject, this));

  client->TraceConnectWithoutContext ("CacheHit",
                                      MakeCallback (&IntelHttpTestCaseCacheTotal::CacheHit, this));
  client->TraceConnectWithoutContext ("CacheMiss",
                                      MakeCallback (&IntelHttpTestCaseCacheTotal::CacheMiss, this));

  server->SetAttribute ("EmbeddedObjectAmountRvs",
                        PointerValue (MakeConstantStream (EMBEDDED_OBJECTS)));

  // Remove unnecessary delays in the simulation
  auto constantZeroStream = MakeConstantStream (0);
  client->SetAttribute ("ParseTimeRvs", PointerValue (constantZeroStream));
  client->SetAttribute ("ReadTimeRvs", PointerValue (constantZeroStream));
  server->SetAttribute ("EmbeddedObjectIatRvs", PointerValue (constantZeroStream));

  Simulator::Stop (DEFAULT_STOP_TIME);
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_countHit + m_countMiss, EMBEDDED_OBJECTS,
                         "Count of cache hits & misses does not equal total embedded object");
}

/**
 * \ingroup intel-http-model-tests
 *
 * Tests that the client caches all embedded objects.
 */
class IntelHttpTestCaseAllCacheHits : public IntelHttpBaseTestCase
{
  const uint16_t EMBEDDED_OBJECTS{100u};
  unsigned int m_countHit{0u};
  bool m_isFirstMainObject{true};

public:
  IntelHttpTestCaseAllCacheHits (void);
  // Use this to stop after the first run
  void RequestMainObject (Ptr<const Packet> /*unused*/);
  void CacheHit (uint16_t /*unused*/);
  void CacheMiss (uint16_t /*unused*/);
  void DoRun (void) override;
};

IntelHttpTestCaseAllCacheHits::IntelHttpTestCaseAllCacheHits (void)
    : IntelHttpBaseTestCase (
          "Intel HTTP Model Test Case - Test that a cache threshold of 1 has all cache hits")
{
}

void
IntelHttpTestCaseAllCacheHits::RequestMainObject (Ptr<const Packet> /*unused*/)
{
  if (m_isFirstMainObject)
    {
      m_isFirstMainObject = false;
    }
  else
    {
      Simulator::Stop ();
    }
}

void IntelHttpTestCaseAllCacheHits::CacheHit (uint16_t /*unused*/)
{
  // We'll still verify the count just in case
  m_countHit++;
}

void IntelHttpTestCaseAllCacheHits::CacheMiss (uint16_t /*unused*/)
{
  // Since there doesn't seem to be a test fail macro,
  // then asserting equality on two unequal things should work...
  NS_TEST_ASSERT_MSG_EQ (true, false, "A cache miss should not occur");
}

void
IntelHttpTestCaseAllCacheHits::DoRun (void)
{
  // Should guarantee all cache hits
  const auto CACHE_THRESHOLD = 1.0;
  client->SetAttribute ("CacheThreshold", DoubleValue (CACHE_THRESHOLD));

  // This trace should stop the simulation
  client->TraceConnectWithoutContext (
      "TxRequestMainObject",
      MakeCallback (&IntelHttpTestCaseAllCacheHits::RequestMainObject, this));
  client->TraceConnectWithoutContext (
      "CacheHit", MakeCallback (&IntelHttpTestCaseAllCacheHits::CacheHit, this));
  client->TraceConnectWithoutContext (
      "CacheMiss", MakeCallback (&IntelHttpTestCaseAllCacheHits::CacheMiss, this));

  // Fix the embedded object count to a known value
  server->SetAttribute ("EmbeddedObjectAmountRvs",
                        PointerValue (MakeConstantStream (EMBEDDED_OBJECTS)));

  // The delays should not have a significant impact on the test
  // but there's no need to leave them enabled
  auto constantZeroStream = MakeConstantStream (0);
  client->SetAttribute ("ParseTimeRvs", PointerValue (constantZeroStream));
  client->SetAttribute ("ReadTimeRvs", PointerValue (constantZeroStream));
  server->SetAttribute ("EmbeddedObjectIatRvs", PointerValue (constantZeroStream));

  Simulator::Stop (DEFAULT_STOP_TIME);
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_countHit, EMBEDDED_OBJECTS,
                         "Cache hit count should match embedded object count");
}

/**
 * \ingroup intel-http-model-tests
 *
 * Tests that the client caches every embedded object.
 */
class IntelHttpTestCaseAllCacheMisses : public IntelHttpBaseTestCase
{
  const uint16_t EMBEDDED_OBJECTS{20u};
  unsigned int m_countMiss{0u};
  bool m_isFirstMainObject{true};

public:
  IntelHttpTestCaseAllCacheMisses (void);
  // Use this to stop after the first run
  void RequestMainObject (Ptr<const Packet> /*unused*/);
  void CacheHit (uint16_t /*unused*/);
  void CacheMiss (uint16_t /*unused*/);
  void DoRun (void) override;
};

IntelHttpTestCaseAllCacheMisses::IntelHttpTestCaseAllCacheMisses (void)
    : IntelHttpBaseTestCase (
          "Intel HTTP Model Test Case - Test that a cache threshold of 0 has no cache hits")
{
}

void
IntelHttpTestCaseAllCacheMisses::RequestMainObject (Ptr<const Packet> /*unused*/)
{
  if (m_isFirstMainObject)
    {
      m_isFirstMainObject = false;
    }
  else
    {
      Simulator::Stop ();
    }
}

void IntelHttpTestCaseAllCacheMisses::CacheHit (uint16_t /*unused*/)
{
  // Since there doesn't seem to be a test fail macro,
  // then asserting equality on two unequal things should work...
  NS_TEST_ASSERT_MSG_EQ (true, false, "A cache hit should not occur");
}

void IntelHttpTestCaseAllCacheMisses::CacheMiss (uint16_t /*unused*/)
{
  // We'll still verify the count just in case
  m_countMiss++;
}

void
IntelHttpTestCaseAllCacheMisses::DoRun (void)
{
  // Should guarantee all cache misses
  const auto CACHE_THRESHOLD = 0.0;
  client->SetAttribute ("CacheThreshold", DoubleValue (CACHE_THRESHOLD));

  // This trace should stop the simulation
  client->TraceConnectWithoutContext (
      "TxRequestMainObject",
      MakeCallback (&IntelHttpTestCaseAllCacheMisses::RequestMainObject, this));
  client->TraceConnectWithoutContext (
      "CacheHit", MakeCallback (&IntelHttpTestCaseAllCacheMisses::CacheHit, this));
  client->TraceConnectWithoutContext (
      "CacheMiss", MakeCallback (&IntelHttpTestCaseAllCacheMisses::CacheMiss, this));

  // Fix the embedded object count to a known value
  server->SetAttribute ("EmbeddedObjectAmountRvs",
                        PointerValue (MakeConstantStream (EMBEDDED_OBJECTS)));

  // The delays should not have a significant impact on the test
  // but there's no need to leave them enabled
  auto constantZeroStream = MakeConstantStream (0);
  client->SetAttribute ("ParseTimeRvs", PointerValue (constantZeroStream));
  client->SetAttribute ("ReadTimeRvs", PointerValue (constantZeroStream));
  server->SetAttribute ("EmbeddedObjectIatRvs", PointerValue (constantZeroStream));

  Simulator::Stop (DEFAULT_STOP_TIME);
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_countMiss, EMBEDDED_OBJECTS,
                         "Cache miss count should match embedded object count");
}

/**
 * \ingroup intel-http-model-tests
 *
 * Tests that embedded objects are requested and transmitted in order
 * given a constant Embedded Object Inter-Arrival Time.
 *
 * Note: Embedded objects will only be transmitted in order
 * given a fixed Embedded Object IAT. If the IAT varies then
 * a later embedded object may transmit while an earlier one
 * is still waiting
 */
class IntelHttpTestCaseEmbeddedObjectSequence : public IntelHttpBaseTestCase
{
  const double EMBEDDED_OBJECTS{10.0};
  bool m_isFirstMainObject{true};

  // Track the number of times the Embedded Object traces are called
  // so we can verify sequence number 0 occurs on the first call
  uint16_t m_requestCalls{0u};
  uint16_t m_transmitCalls{0u};

  // Sequence Number of the last embedded object
  // Should start with 0 so we can
  // special case the first object
  uint16_t m_lastRequestedObjectNumber{0u};
  uint16_t m_lastTransmittedObjectNumber{0u};

public:
  IntelHttpTestCaseEmbeddedObjectSequence (void);
  void RequestMainObject (Ptr<const Packet> /*unused*/);
  void RequestEmbeddedObject (Ptr<const Packet> packet);
  void TransmitEmbeddedObject (Ptr<const Packet> packet);
  void DoRun (void) override;
};

IntelHttpTestCaseEmbeddedObjectSequence::IntelHttpTestCaseEmbeddedObjectSequence (void)
    : IntelHttpBaseTestCase (
          "Intel HTTP Model Test Case - Verify the Sequence Order of Embedded Objects")
{
}

void
IntelHttpTestCaseEmbeddedObjectSequence::RequestMainObject (Ptr<const Packet> /*unused*/)
{
  if (m_isFirstMainObject)
    {
      m_isFirstMainObject = false;
    }
  else
    {
      Simulator::Stop ();
    }
}

void
IntelHttpTestCaseEmbeddedObjectSequence::RequestEmbeddedObject (Ptr<const Packet> packet)
{
  psc::IntelHttpHeader header;
  packet->PeekHeader (header);

  if (header.GetNumberEmbeddedObjects () == 0u)
    {
      NS_TEST_ASSERT_MSG_EQ (
          m_requestCalls, 0,
          "Embedded object with sequence number 0 requested after first embedded object");
      return;
    }

  NS_TEST_ASSERT_MSG_EQ (header.GetNumberEmbeddedObjects (), m_lastRequestedObjectNumber + 1,
                         "Current embedded object number does not match expected number");

  m_lastRequestedObjectNumber++;
  m_requestCalls++;
}

void
IntelHttpTestCaseEmbeddedObjectSequence::TransmitEmbeddedObject (Ptr<const Packet> packet)
{
  psc::IntelHttpHeader header;
  packet->PeekHeader (header);

  // Special case the (hopefully) first packet
  // Since we don't have a previous one in the sequence
  // to compare it with, use the number of times this
  // trace was called
  if (header.GetNumberEmbeddedObjects () == 0u)
    {
      NS_TEST_ASSERT_MSG_EQ (
          m_transmitCalls, 0,
          "Embedded object with sequence number 0 received after first embedded object");
      return;
    }

  NS_TEST_ASSERT_MSG_EQ (header.GetNumberEmbeddedObjects (), m_lastTransmittedObjectNumber + 1,
                         "Current embedded object number does not match expected number");

  m_lastTransmittedObjectNumber++;
  m_transmitCalls++;
}

void
IntelHttpTestCaseEmbeddedObjectSequence::DoRun (void)
{
  // Fix the embedded object count to a known value
  server->SetAttribute ("EmbeddedObjectAmountRvs",
                        PointerValue (MakeConstantStream (EMBEDDED_OBJECTS)));

  // Make sure that none of our objects are cached
  const auto CACHE_THRESHOLD = 0.0;
  client->SetAttribute ("CacheThreshold", DoubleValue (CACHE_THRESHOLD));

  client->TraceConnectWithoutContext (
      "TxRequestMainObject",
      MakeCallback (&IntelHttpTestCaseEmbeddedObjectSequence::RequestMainObject, this));
  client->TraceConnectWithoutContext (
      "TxRequestEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseEmbeddedObjectSequence::RequestEmbeddedObject, this));

  server->TraceConnectWithoutContext (
      "TxEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseEmbeddedObjectSequence::TransmitEmbeddedObject, this));

  // Make sure no object is held for longer than another
  const auto WAIT_TIME = 10.0;
  server->SetAttribute ("EmbeddedObjectIatRvs", PointerValue (MakeConstantStream (WAIT_TIME)));

  Simulator::Stop (DEFAULT_STOP_TIME);
  Simulator::Run ();
  Simulator::Destroy ();

  // Make sure we send a request/response for every object
  NS_TEST_ASSERT_MSG_EQ (m_lastRequestedObjectNumber, EMBEDDED_OBJECTS - 1,
                         "Incorrect number of embedded objects requested");
  NS_TEST_ASSERT_MSG_EQ (m_lastTransmittedObjectNumber, EMBEDDED_OBJECTS - 1,
                         "Incorrect number of embedded objects transmitted");
}

/**
 * \ingroup intel-http-model-tests
 *
 * Tests that the client waits the parse time.
 */
class IntelHttpTestCaseParseTime : public IntelHttpBaseTestCase
{
  bool m_isFirstRun{true};

  // Track the start and end times
  // to find the duration of each event
  Time m_mainObjectStart;
  Time m_mainObjectEnd;

  Time m_embeddedObjectStart;
  Time m_embeddedObjectEnd;

public:
  void RequestMainObject (Ptr<const Packet> /* unused */);
  void ReceiveMainObject (Ptr<const Packet> /* unused */, const Address & /* unused */);
  void RequestEmbeddedObject (Ptr<const Packet> /* unused */);
  void ReceiveEmbeddedObject (Ptr<const Packet> packet, const Address & /* unused */);

  IntelHttpTestCaseParseTime (void);
  void DoRun (void) override;
};

void
IntelHttpTestCaseParseTime::RequestMainObject (Ptr<const Packet> /* unused */)
{
  if (m_isFirstRun)
    {
      m_isFirstRun = false;
    }
  else
    {
      // Since we have no trace for when a cycle is complete
      // use the beginning of the next cycle
      m_embeddedObjectEnd = Simulator::Now ();
      Simulator::Stop ();
    }
}

void
IntelHttpTestCaseParseTime::ReceiveMainObject (Ptr<const Packet> /* unused */,
                                               const Address & /* unused */)
{
  m_mainObjectStart = Simulator::Now ();
}

void
IntelHttpTestCaseParseTime::RequestEmbeddedObject (Ptr<const Packet> /* unused */)
{
  m_mainObjectEnd = Simulator::Now ();
}

void
IntelHttpTestCaseParseTime::ReceiveEmbeddedObject (Ptr<const Packet> /* unused */,
                                                   const Address & /* unused */)
{
  m_embeddedObjectStart = Simulator::Now ();
}
IntelHttpTestCaseParseTime::IntelHttpTestCaseParseTime (void)
    : IntelHttpBaseTestCase ("Intel HTTP Model Test Case - Verify the Client Parse Times")
{
}

void
IntelHttpTestCaseParseTime::DoRun (void)
{
  // Fix the parse time to an expected value
  const auto PARSE_TIME = 20;
  client->SetAttribute ("ParseTimeRvs", PointerValue (MakeConstantStream (PARSE_TIME)));

  // Clear the read time so we can test only the parse time
  const auto READ_TIME = 0;
  client->SetAttribute ("ReadTimeRvs", PointerValue (MakeConstantStream (READ_TIME)));

  // Make sure we don't miss our Embedded Object Request
  const auto CACHE_THRESHOLD = 0.0;
  client->SetAttribute ("CacheThreshold", DoubleValue (CACHE_THRESHOLD));

  const auto EMBEDDED_OBJECTS = 1;
  server->SetAttribute ("EmbeddedObjectAmountRvs",
                        PointerValue (MakeConstantStream (EMBEDDED_OBJECTS)));

  // Connect to traces for our timing
  client->TraceConnectWithoutContext (
      "TxRequestMainObject", MakeCallback (&IntelHttpTestCaseParseTime::RequestMainObject, this));
  client->TraceConnectWithoutContext (
      "RxMainObject", MakeCallback (&IntelHttpTestCaseParseTime::ReceiveMainObject, this));

  client->TraceConnectWithoutContext (
      "TxRequestEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseParseTime::RequestEmbeddedObject, this));
  client->TraceConnectWithoutContext (
      "RxEmbeddedObject", MakeCallback (&IntelHttpTestCaseParseTime::ReceiveEmbeddedObject, this));

  // We'll abort before this
  Simulator::Stop (DEFAULT_STOP_TIME);
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_mainObjectEnd - m_mainObjectStart, Seconds (PARSE_TIME),
                         "Main object parsing took an unexpected amount of time");
  NS_TEST_ASSERT_MSG_EQ (m_embeddedObjectEnd - m_embeddedObjectStart, Seconds (PARSE_TIME),
                         "Embedded object parsing took an unexpected amount of time");
}

/**
 * \ingroup intel-http-model-tests
 *
 * Tests that the client waits the read time.
 */
class IntelHttpTestCaseReadTime : public IntelHttpBaseTestCase
{
  bool m_isFirstRun{true};

  // Track the start and end times
  // to find the duration of each event
  Time m_readStart;
  Time m_readEnd;

public:
  void RequestMainObject (Ptr<const Packet> /* unused */);
  void ReceiveEmbeddedObject (Ptr<const Packet> /* unused */, const Address & /* unused */);

  IntelHttpTestCaseReadTime (void);
  void DoRun (void) override;
};

void
IntelHttpTestCaseReadTime::RequestMainObject (Ptr<const Packet> /* unused */)
{
  if (m_isFirstRun)
    {
      m_isFirstRun = false;
    }
  else
    {
      // Since we have no trace for when a cycle is complete
      // use the beginning of the next cycle
      m_readEnd = Simulator::Now ();
      Simulator::Stop ();
    }
}

void
IntelHttpTestCaseReadTime::ReceiveEmbeddedObject (Ptr<const Packet> /* unused */,
                                                  const Address & /* unused */)
{
  m_readStart = Simulator::Now ();
}

IntelHttpTestCaseReadTime::IntelHttpTestCaseReadTime (void)
    : IntelHttpBaseTestCase ("Intel HTTP Model Test Case - Verify the Client Read Time")
{
}

void
IntelHttpTestCaseReadTime::DoRun (void)
{
  // Fix the read time to an expected value
  const auto READ_TIME = 20;
  client->SetAttribute ("ReadTimeRvs", PointerValue (MakeConstantStream (READ_TIME)));

  // Clear the parse time so we can test only the read time.
  const auto PARSE_TIME = 0;
  client->SetAttribute ("ParseTimeRvs", PointerValue (MakeConstantStream (PARSE_TIME)));

  // Make sure we don't miss our Embedded Object Request
  const auto CACHE_THRESHOLD = 0.0;
  client->SetAttribute ("CacheThreshold", DoubleValue (CACHE_THRESHOLD));

  const auto EMBEDDED_OBJECTS = 1;
  server->SetAttribute ("EmbeddedObjectAmountRvs",
                        PointerValue (MakeConstantStream (EMBEDDED_OBJECTS)));

  // Connect to traces for our timing
  client->TraceConnectWithoutContext (
      "TxRequestMainObject", MakeCallback (&IntelHttpTestCaseReadTime::RequestMainObject, this));
  client->TraceConnectWithoutContext (
      "RxEmbeddedObject", MakeCallback (&IntelHttpTestCaseReadTime::ReceiveEmbeddedObject, this));

  // We'll abort before this
  Simulator::Stop (DEFAULT_STOP_TIME);
  Simulator::Run ();
  Simulator::Destroy ();
  NS_TEST_ASSERT_MSG_EQ (m_readEnd - m_readStart, Seconds (READ_TIME),
                         "Embedded object parsing took an unexpected amount of time");
}

/**
 * \ingroup intel-http-model-tests
 *
 * Tests that the server waits the Embedded Object Inter-Arrival Time
 */
class IntelHttpTestCaseObjectInterArrival : public IntelHttpBaseTestCase
{
  Time m_IatStart;
  Time m_IatEnd;

public:
  void RequestEmbeddedObject (Ptr<const Packet> /* unused */, const Address & /* unused */);
  void TransmitEmbeddedObject (Ptr<const Packet> /* unused */);

  IntelHttpTestCaseObjectInterArrival (void);
  void DoRun (void) override;
};

void
IntelHttpTestCaseObjectInterArrival::RequestEmbeddedObject (Ptr<const Packet> /* unused */,
                                                            const Address & /* unused */)
{
  m_IatStart = Simulator::Now ();
}

void
IntelHttpTestCaseObjectInterArrival::TransmitEmbeddedObject (Ptr<const Packet> /* unused */)
{
  m_IatEnd = Simulator::Now ();
  Simulator::Stop ();
}

IntelHttpTestCaseObjectInterArrival::IntelHttpTestCaseObjectInterArrival (void)
    : IntelHttpBaseTestCase ("Intel HTTP Model Test Case - Verify the Server IAT")
{
}

void
IntelHttpTestCaseObjectInterArrival::DoRun (void)
{
  const auto INTER_ARRIVAL_TIME = 15.0;
  server->SetAttribute ("EmbeddedObjectIatRvs",
                        PointerValue (MakeConstantStream (INTER_ARRIVAL_TIME)));

  // Fix us to one embedded object since we'd  end up with the last time
  // for each
  const auto EMBEDDED_OBJECTS = 1.0;
  server->SetAttribute ("EmbeddedObjectAmountRvs",
                        PointerValue (MakeConstantStream (EMBEDDED_OBJECTS)));

  server->TraceConnectWithoutContext (
      "RxRequestEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseObjectInterArrival::RequestEmbeddedObject, this));
  server->TraceConnectWithoutContext (
      "TxEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseObjectInterArrival::TransmitEmbeddedObject, this));

  Simulator::Stop (DEFAULT_STOP_TIME);
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_IatEnd - m_IatStart, Seconds (INTER_ARRIVAL_TIME),
                         "Server IAT took longer than expected");
}

/**
 * \ingroup intel-http-model-tests
 *
 * Verify that the model appropriately sends messages
 * provided IPv6 Addresses (since all other tests were using IPv4)
 */
class IntelHttpTestCaseIpv6 : public TestCase
{
  // Test a few events
  // Should be enough to see
  // a back and forth
  bool m_mainObjectRequested{false};
  bool m_mainObjectRequestReceived{false};
  bool m_embeddedObjectReceived{false};

public:
  void RequestMainObject (Ptr<const Packet> /* unused */);
  void ReceiveMainObjectRequest (Ptr<const Packet> /* unused */, const Address & /* unused */);
  void ReceiveEmbeddedObject (Ptr<const Packet> /* unused */, const Address & /* unused */);

  IntelHttpTestCaseIpv6 (void);
  void DoRun (void) override;
};

void
IntelHttpTestCaseIpv6::RequestMainObject (Ptr<const Packet> /* unused */)
{
  if (!m_mainObjectRequested)
    {
      m_mainObjectRequested = true;
    }
  else
    {
      Simulator::Stop ();
    }
}

void
IntelHttpTestCaseIpv6::ReceiveMainObjectRequest (Ptr<const Packet> /* unused */,
                                                 const Address & /* unused */)
{
  m_mainObjectRequestReceived = true;
}

void
IntelHttpTestCaseIpv6::ReceiveEmbeddedObject (Ptr<const Packet> /* unused */,
                                              const Address & /* unused */)
{
  m_embeddedObjectReceived = true;
}

IntelHttpTestCaseIpv6::IntelHttpTestCaseIpv6 (void)
    : TestCase ("Intel HTTP Model Test Case - Verify IPv6 Addresses Are Accepted By the Model")
{
}

void
IntelHttpTestCaseIpv6::DoRun (void)
{
  // Run through the setup
  // the base case would normally handle
  const auto STOP_TIME = Seconds (100);
  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Gbps"));

  auto devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv6AddressHelper address;
  auto interfaces = address.Assign (devices);

  auto server = CreateObject<psc::IntelHttpServer> ();
  server->SetStartTime (Seconds (0));
  server->SetStopTime (STOP_TIME);
  nodes.Get (0)->AddApplication (server);

  const auto EMBEDDED_OBJECTS = 1.0;
  server->SetAttribute ("EmbeddedObjectAmountRvs",
                        PointerValue (MakeConstantStream (EMBEDDED_OBJECTS)));

  auto client = CreateObject<psc::IntelHttpClient> ();
  client->SetStartTime (Seconds (1));
  client->SetStopTime (STOP_TIME);
  client->SetAttribute ("RemoteAddress", AddressValue (interfaces.GetAddress (0, 0)));
  nodes.Get (1)->AddApplication (client);

  const auto CACHE_THRESHOLD = 0.0;
  client->SetAttribute ("CacheThreshold", DoubleValue (CACHE_THRESHOLD));

  client->TraceConnectWithoutContext (
      "TxRequestMainObject", MakeCallback (&IntelHttpTestCaseIpv6::RequestMainObject, this));
  server->TraceConnectWithoutContext (
      "RxRequestMainObject", MakeCallback (&IntelHttpTestCaseIpv6::ReceiveMainObjectRequest, this));
  client->TraceConnectWithoutContext (
      "RxEmbeddedObject", MakeCallback (&IntelHttpTestCaseIpv6::ReceiveEmbeddedObject, this));

  Simulator::Stop (STOP_TIME);
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_mainObjectRequested, true, "Client never requested a Main Object");
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectRequestReceived, true,
                         "Server never received a request for a Main Object");
  NS_TEST_ASSERT_MSG_EQ (m_embeddedObjectReceived, true,
                         "Client never received an embedded object");
}

/**
 * \ingroup intel-http-model-tests
 *
 * Verify the HTTP header for request types and overall embedded object count
 */
class IntelHttpTestCaseHeaderContent : public IntelHttpBaseTestCase
{
  // Use a fixed number (not 1 since we've used that plenty of times)
  // so we can test the request for embedded objects has the correct count
  const uint16_t EMBEDDED_OBJECTS = 5u;

  // Make sure we test each possible type at least once
  bool m_mainObjectRequest{false};
  bool m_mainObjectRequestReceived{false};
  bool m_mainObject{false};
  bool m_mainObjectReceived{false};
  bool m_embeddedObjectRequest{false};
  bool m_embeddedObjectRequestReceived{false};
  bool m_embeddedObject{false};
  bool m_embeddedObjectReceived{false};

public:
  IntelHttpTestCaseHeaderContent (void);

  void RequestMainObject (Ptr<const Packet> packet);
  void ReceiveMainObjectRequest (Ptr<const Packet> packet, const Address & /* unused */);
  void SendMainObject (Ptr<const Packet> packet);
  void ReceiveMainObject (Ptr<const Packet> packet, const Address & /* unused */);
  void RequestEmbeddedObject (Ptr<const Packet> packet);
  void ReceiveEmbeddedObjectRequest (Ptr<const Packet> packet, const Address & /* unused */);
  void SendEmbeddedObject (Ptr<const Packet> packet);
  void ReceiveEmbeddedObject (Ptr<const Packet> packet, const Address & /* unused */);

  void DoRun (void) override;
};

IntelHttpTestCaseHeaderContent::IntelHttpTestCaseHeaderContent (void)
    : IntelHttpBaseTestCase (
          "Intel HTTP Model Test Case - Verify HTTP Headers Report the Correct Information")
{
}

void
IntelHttpTestCaseHeaderContent::RequestMainObject (Ptr<const Packet> packet)
{
  psc::IntelHttpHeader header;
  packet->PeekHeader (header);

  NS_TEST_ASSERT_MSG_EQ (
      header.GetRequestType (), psc::IntelHttpHeader::Type::Main,
      "Callback mismatch: Client request for Main object has Embedded object header");

  m_mainObjectRequest = true;
}

void
IntelHttpTestCaseHeaderContent::ReceiveMainObjectRequest (Ptr<const Packet> packet, const Address &)
{
  psc::IntelHttpHeader header;
  packet->PeekHeader (header);

  NS_TEST_ASSERT_MSG_EQ (
      header.GetRequestType (), psc::IntelHttpHeader::Type::Main,
      "Callback mismatch: Server received request for Main object that has Embedded object header");

  m_mainObjectRequestReceived = true;
}

void
IntelHttpTestCaseHeaderContent::SendMainObject (Ptr<const Packet> packet)
{
  psc::IntelHttpHeader header;
  packet->PeekHeader (header);

  NS_TEST_ASSERT_MSG_EQ (header.GetRequestType (), psc::IntelHttpHeader::Type::Main,
                         "Callback mismatch: Server sent Main object with Embedded object header");
  NS_TEST_ASSERT_MSG_EQ (header.GetNumberEmbeddedObjects (), EMBEDDED_OBJECTS,
                         "Embedded object count in header does not match expected count");

  m_mainObject = true;
}

void
IntelHttpTestCaseHeaderContent::ReceiveMainObject (Ptr<const Packet> packet, const Address &)
{
  psc::IntelHttpHeader header;
  packet->PeekHeader (header);

  NS_TEST_ASSERT_MSG_EQ (
      header.GetRequestType (), psc::IntelHttpHeader::Type::Main,
      "Callback mismatch: Client received Main object with Embedded object header");
  NS_TEST_ASSERT_MSG_EQ (header.GetNumberEmbeddedObjects (), EMBEDDED_OBJECTS,
                         "Embedded object count in header does not match expected count");
  m_mainObjectReceived = true;
}

void
IntelHttpTestCaseHeaderContent::RequestEmbeddedObject (Ptr<const Packet> packet)
{
  psc::IntelHttpHeader header;
  packet->PeekHeader (header);

  NS_TEST_ASSERT_MSG_EQ (
      header.GetRequestType (), psc::IntelHttpHeader::Type::Embedded,
      "Callback mismatch: Client requested Embedded object with Main object header");

  // We've already verified the embedded object count in
  // IntelHttpTestCaseEmbeddedObjectSequence

  m_embeddedObjectRequest = true;
}

void
IntelHttpTestCaseHeaderContent::ReceiveEmbeddedObjectRequest (Ptr<const Packet> packet,
                                                              const Address &)
{
  psc::IntelHttpHeader header;
  packet->PeekHeader (header);

  NS_TEST_ASSERT_MSG_EQ (
      header.GetRequestType (), psc::IntelHttpHeader::Type::Embedded,
      "Callback mismatch: Server received request for Embedded object with Main object header");

  // We've already verified the embedded object count in
  // IntelHttpTestCaseEmbeddedObjectSequence

  m_embeddedObjectRequestReceived = true;
}

void
IntelHttpTestCaseHeaderContent::SendEmbeddedObject (Ptr<const Packet> packet)
{
  psc::IntelHttpHeader header;
  packet->PeekHeader (header);

  NS_TEST_ASSERT_MSG_EQ (header.GetRequestType (), psc::IntelHttpHeader::Type::Embedded,
                         "Callback mismatch: Server sent Embedded object with Main object header");

  m_embeddedObject = true;
}

void
IntelHttpTestCaseHeaderContent::ReceiveEmbeddedObject (Ptr<const Packet> packet, const Address &)
{
  psc::IntelHttpHeader header;
  packet->PeekHeader (header);

  NS_TEST_ASSERT_MSG_EQ (
      header.GetRequestType (), psc::IntelHttpHeader::Type::Embedded,
      "Callback mismatch: Client received Main object with Embedded object header");

  m_embeddedObjectReceived = true;
}

void
IntelHttpTestCaseHeaderContent::DoRun (void)
{
  // Fix us to a guaranteed count of embedded objects
  const auto CACHE_THRESHOLD = 0.0;
  client->SetAttribute ("CacheThreshold", DoubleValue (CACHE_THRESHOLD));
  server->SetAttribute ("EmbeddedObjectAmountRvs",
                        PointerValue (MakeConstantStream (EMBEDDED_OBJECTS)));

  client->TraceConnectWithoutContext (
      "TxRequestMainObject",
      MakeCallback (&IntelHttpTestCaseHeaderContent::RequestMainObject, this));
  client->TraceConnectWithoutContext (
      "TxRequestEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseHeaderContent::RequestEmbeddedObject, this));
  client->TraceConnectWithoutContext (
      "RxMainObject", MakeCallback (&IntelHttpTestCaseHeaderContent::ReceiveMainObject, this));
  client->TraceConnectWithoutContext (
      "RxEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseHeaderContent::ReceiveEmbeddedObject, this));

  server->TraceConnectWithoutContext (
      "RxRequestMainObject",
      MakeCallback (&IntelHttpTestCaseHeaderContent::ReceiveMainObjectRequest, this));
  server->TraceConnectWithoutContext (
      "RxRequestEmbeddedObject",
      MakeCallback (&IntelHttpTestCaseHeaderContent::ReceiveEmbeddedObjectRequest, this));
  server->TraceConnectWithoutContext (
      "TxMainObject", MakeCallback (&IntelHttpTestCaseHeaderContent::SendMainObject, this));
  server->TraceConnectWithoutContext (
      "TxEmbeddedObject", MakeCallback (&IntelHttpTestCaseHeaderContent::SendEmbeddedObject, this));

  // This one will run the whole length
  // since each test assert is independent
  // of the others
  Simulator::Stop (DEFAULT_STOP_TIME);
  Simulator::Run ();
  Simulator::Destroy ();

  // Make sure we actually hit each of the test cases
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectRequest, true, "Client never requested a main object");
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectRequestReceived, true,
                         "Server never received a main object request");
  NS_TEST_ASSERT_MSG_EQ (m_mainObject, true, "Server never sent a main object");
  NS_TEST_ASSERT_MSG_EQ (m_mainObjectReceived, true, "Client never received a main object");
  NS_TEST_ASSERT_MSG_EQ (m_embeddedObjectRequest, true,
                         "Client never requested an embedded object");
  NS_TEST_ASSERT_MSG_EQ (m_embeddedObjectRequestReceived, true,
                         "Server never received a request for an embedded object");
  NS_TEST_ASSERT_MSG_EQ (m_embeddedObject, true, "Server never sent an embedded object");
  NS_TEST_ASSERT_MSG_EQ (m_embeddedObjectReceived, true,
                         "Client never received an embedded object");
}

/**
 * \ingroup intel-http-model-tests
 */
class IntelHttpTestSuite : public TestSuite
{
public:
  IntelHttpTestSuite (void);
};
IntelHttpTestSuite::IntelHttpTestSuite (void) : TestSuite ("intel-http-model")
{
  AddTestCase (new IntelHttpTestCaseEventOrder ());
  AddTestCase (new IntelHttpTestCaseTxRxCountSizes ());
  AddTestCase (new IntelHttpTestCaseCacheTotal ());
  AddTestCase (new IntelHttpTestCaseAllCacheHits ());
  AddTestCase (new IntelHttpTestCaseAllCacheMisses ());
  AddTestCase (new IntelHttpTestCaseEmbeddedObjectSequence ());
  AddTestCase (new IntelHttpTestCaseParseTime ());
  AddTestCase (new IntelHttpTestCaseReadTime ());
  AddTestCase (new IntelHttpTestCaseObjectInterArrival ());
  AddTestCase (new IntelHttpTestCaseIpv6 ());
  AddTestCase (new IntelHttpTestCaseHeaderContent ());
}

/**
 * \ingroup intel-http-model-tests
 */
static IntelHttpTestSuite g_intelHttpTestSuite;

} // namespace tests
} // namespace ns3
