/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
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

#include <ns3/callback.h>
#include <ns3/core-module.h>
#include <ns3/mcptt-on-network-floor-participant.h>

#include "mcptt-test-case.h"
#include "mcptt-test-case-config-on-network.h"
#include "ns3/mcptt-server-app.h"
#include "ns3/sip-header.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttTestSuiteCallControlOnNetwork");

namespace psc {
namespace tests {

/**
 * Test case corresponding to TS 36.579-2, Section 6.1.1.1, pre-arranged
 * group call, client originated, Table 6.1.1.1.3.2-1
 */
class McpttTestCasePreArrangedGroupCO : public McpttTestCase
{
public:
  McpttTestCasePreArrangedGroupCO (const std::string& name = "Floor Release", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void ServerRxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void ServerTxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

private:
  struct TestEvent
  {
    uint32_t m_step;
    std::string m_ueToServer;
    std::string m_message;
    TestEvent (uint32_t step, std::string ueToServer, std::string message)
      : m_step (step),
        m_ueToServer (ueToServer),
        m_message (message)
    {}
    bool operator != (const TestEvent& b) const
    {
      return (m_step != b.m_step || m_ueToServer != b.m_ueToServer || m_message != b.m_message);
    }
  };
  uint32_t m_step {0};  //!< Step number in test sequence
  std::list<TestEvent> m_expectedEvents; //!< Expected events
  std::list<TestEvent> m_observedEvents; //!< Observed events
};

class McpttTestSuiteCallControlOnNetwork : public TestSuite
{
public:
  McpttTestSuiteCallControlOnNetwork (void);
};

/***************************************************************
 *  Implementation of the code declared above.
 ***************************************************************/

McpttTestCasePreArrangedGroupCO::McpttTestCasePreArrangedGroupCO (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config)
{ }

void
McpttTestCasePreArrangedGroupCO::Configure (void)
{
  McpttHelper clientHelper;
  clientHelper.SetPttApp ("ns3::psc::McpttPttApp");
  clientHelper.SetMediaSrc ("ns3::psc::McpttMediaSrc",
                            "Bytes", UintegerValue (60),
                            "DataRate", DataRateValue (DataRate ("24kb/s")));
  clientHelper.SetPusher ("ns3::psc::McpttPusher",
                          "Automatic", BooleanValue (false));

  McpttCallHelper callHelper;
  callHelper.SetArbitrator ("ns3::psc::McpttOnNetworkFloorArbitrator",
                            "AckRequired", BooleanValue (true),
                            "AudioCutIn", BooleanValue (false),
                            "DualFloorSupported", BooleanValue (false),
                            "TxSsrc", UintegerValue (0),
                            "QueueingSupported", BooleanValue (true));
  callHelper.SetTowardsParticipant ("ns3::psc::McpttOnNetworkFloorTowardsParticipant",
                                    "ReceiveOnly", BooleanValue (false));
  callHelper.SetParticipant ("ns3::psc::McpttOnNetworkFloorParticipant",
                             "AckRequired", BooleanValue (true),
                             "GenMedia", BooleanValue (true));
  callHelper.SetServerCall ("ns3::psc::McpttServerCall",
                            "AmbientListening", BooleanValue (false),
                            "TemporaryGroup", BooleanValue (false));

  Ptr<McpttTestCaseConfigOnNetwork> config = Create<McpttTestCaseConfigOnNetwork> ();
  config->SetAppCount (2);
  config->SetClientHelper (clientHelper);
  config->SetCallHelper (callHelper);
  config->SetStop (Seconds (10));

  SetConfig (config);

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ue1App = GetApp (0);
  Ptr<McpttPttApp> ue2App = GetApp (1);
  Ptr<McpttServerApp> serverApp = config->GetServerApp ();

  ue1App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCasePreArrangedGroupCO::Ue1RxCb, this));
  ue1App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCasePreArrangedGroupCO::Ue1TxCb, this));
  serverApp->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCasePreArrangedGroupCO::ServerRxCb, this));
  serverApp->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCasePreArrangedGroupCO::ServerTxCb, this));

  m_step = 1;  // Advance step counter to first event
  // Step 1 in Table 6.1.1.1.3.2-1; this action is not logged by the test suite
  Ptr<McpttPusher> ue1Pusher = ue1App->GetPusher ();
  ue1Pusher->SchedulePush (Seconds (3.0));
  // Step 8 in Table 6.1.1.1.3.2-1; this action is not logged by the test suite
  ue1Pusher->ScheduleRelease (Seconds (4.0));
}

void
McpttTestCasePreArrangedGroupCO::Execute (void)
{
  // Configure expected sequence according to TS 36.579-2, Table 6.1.1.1.3.2-1
  m_expectedEvents.push_back (TestEvent (2, "-->", "SIP INVITE"));
  // TODO: Step 3 'SIP 100 Trying' is occurring (in the logs) but this
  // test suite cannot hook the event until McpttCall::ReceiveSipEvent
  // is completed (and the TRYING_RECEIVED event is exported)
  m_expectedEvents.push_back (TestEvent (5, "<--", "SIP 200 (OK)"));
  m_expectedEvents.push_back (TestEvent (6, "-->", "SIP ACK"));
  m_expectedEvents.push_back (TestEvent (9, "-->", "Floor Release"));
  m_expectedEvents.push_back (TestEvent (10, "<--", "Floor Ack"));
  m_expectedEvents.push_back (TestEvent (11, "<--", "Floor Idle"));
  // TODO:  add events beyond step 11

  Simulator::Stop (Seconds (10));
  Simulator::Run ();
  Simulator::Destroy ();

//#define PRE_ARRANGED_GROUP_C0 1
#ifdef PRE_ARRANGED_GROUP_C0
  // Define this to print out the observed events, if needed for debugging
  // or to generate new expected events.
  std::cout << "Expected events:" << std::endl;
  for (auto it = m_expectedEvents.begin (); it != m_expectedEvents.end (); it++)
    {
      std::cout << "  m_expectedEvents.push_back (TestEvent (" << it->m_step << ", " << it->m_ueToServer << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Observed events: (these are printed out as 'expected' so they can be copied)" << std::endl;
  for (auto it = m_observedEvents.begin (); it != m_observedEvents.end (); it++)
    {
      std::cout << "  m_expectedEvents.push_back (TestEvent (" << it->m_step << ", " << it->m_ueToServer << ", \"" << it->m_message << "\"));" << std::endl;
    }
#endif

  NS_TEST_ASSERT_MSG_EQ (m_observedEvents.size (), m_expectedEvents.size (), "Did not observe all expected events");
  auto it1 = m_observedEvents.begin ();
  auto it2 = m_expectedEvents.begin ();
  while (it1 != m_observedEvents.end () && it2 != m_expectedEvents.end ())
    {
      if (*it1 != *it2)
        {
          NS_TEST_ASSERT_MSG_EQ (it1->m_step, it2->m_step, "Event steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_ueToServer, it2->m_ueToServer, "UE to server direction not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_message, it2->m_message, "Message not equal");
        }
      it1++;
      it2++;
    }
}

void
McpttTestCasePreArrangedGroupCO::Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  NS_LOG_DEBUG ("UE1 receive message");
  if (headerType == sip::SipHeader::GetTypeId ())
    {
      sip::SipHeader msg;
      pkt->PeekHeader (msg);
      sip::SipHeader::SipMessageType messageType = msg.GetMessageType ();
      uint16_t statusCode = 0;
      if (messageType == sip::SipHeader::SIP_RESPONSE)
        {
          statusCode = msg.GetStatusCode ();
          if (statusCode == 200 && m_step == 2)
            {
              NS_LOG_DEBUG ("Step 5 <-- SIP 200 (OK)");
              // Skip to step 5, since step 3 (Trying) is not observable
              // in this test, and step 4 is 'Void' in Table 6.1.1.1.3.2-1
              m_step = 5;
              m_observedEvents.push_back (TestEvent (m_step, "<--", "SIP 200 (OK)"));
            }
        }
      else if (messageType == sip::SipHeader::SIP_REQUEST)
        {
          // Placeholder
          NS_LOG_DEBUG ("SIP Request not yet expected");
        }
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ())
    {
      if (m_step == 9)
        {
          NS_LOG_DEBUG ("Step 10 <-- Floor Ack");
          m_observedEvents.push_back (TestEvent (10, "<--", "Floor Ack"));
          m_step = 10;
        }
    }
  else if (headerType == McpttFloorMsgIdle::GetTypeId ())
    {
      if (m_step == 10)
        {
          NS_LOG_DEBUG ("Step 11 <-- Floor Idle");
          m_observedEvents.push_back (TestEvent (11, "<--", "Floor Idle"));
          m_step = 11;
        }
    }
}

void
McpttTestCasePreArrangedGroupCO::Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  NS_LOG_DEBUG ("UE1 send message");
  if (headerType == sip::SipHeader::GetTypeId ())
    {
      sip::SipHeader msg;
      pkt->PeekHeader (msg);
      sip::SipHeader::SipMessageType messageType = msg.GetMessageType ();
      sip::SipHeader::SipMethod method;
      if (messageType == sip::SipHeader::SIP_REQUEST)
        {
          method = msg.GetMethod ();
          if (method == sip::SipHeader::INVITE && m_step == 1)
            {
              NS_LOG_DEBUG ("Step 2 --> SIP INVITE");
              m_step++;
              m_observedEvents.push_back (TestEvent (m_step, "-->", "SIP INVITE"));
            }
          else if (method == sip::SipHeader::ACK && m_step == 5)
            {
              NS_LOG_DEBUG ("Step 6 --> SIP ACK");
              m_step++;
              m_observedEvents.push_back (TestEvent (m_step, "-->", "SIP ACK"));
            }
        }
      else if (messageType == sip::SipHeader::SIP_RESPONSE)
        {
          NS_LOG_DEBUG ("SIP Response not yet expected");
        }
    }
  else if (headerType == McpttFloorMsgRelease::GetTypeId ())
    {
      if (m_step == 6)
        {
          NS_LOG_DEBUG ("Step 9 --> Floor Release");
          m_observedEvents.push_back (TestEvent (9, "-->", "Floor Release"));
          m_step = 9;
        }
    }
}

void
McpttTestCasePreArrangedGroupCO::ServerRxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  NS_LOG_DEBUG ("Server receive message");
  // Note:  In TS 36.579-2, events are ordered from the perspective of
  // the UE, testing against a black box EPC, so only log observed events
  // on the UE, and just print debug log statements here
  if (headerType == sip::SipHeader::GetTypeId ())
    {
      NS_LOG_DEBUG ("Received SIP message on server after step " << m_step);
    }
  else
    {
      NS_LOG_DEBUG ("Received MCPTT message on server after step " << m_step);
    }
}

void
McpttTestCasePreArrangedGroupCO::ServerTxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  NS_LOG_DEBUG ("Server send message");
  // Note:  In TS 36.579-2, events are ordered from the perspective of
  // the UE, testing against a black box EPC, so only log observed events
  // on the UE, and just print debug log statements here
  if (headerType == sip::SipHeader::GetTypeId ())
    {
      NS_LOG_DEBUG ("Sent SIP message on server after step " << m_step);
    }
  else
    {
      NS_LOG_DEBUG ("Sent MCPTT message on server after step " << m_step);
    }
}

McpttTestSuiteCallControlOnNetwork::McpttTestSuiteCallControlOnNetwork (void)
  : TestSuite ("mcptt-call-control-on-network", TestSuite::SYSTEM)
{
  AddTestCase (new McpttTestCasePreArrangedGroupCO (), TestCase::QUICK);
}

static McpttTestSuiteCallControlOnNetwork suite;

} // namespace tests
} // namespace psc
} // namespace ns3
