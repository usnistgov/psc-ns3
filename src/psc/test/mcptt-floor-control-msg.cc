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

#include <sstream>
#include <string>

#include <ns3/core-module.h>
#include <ns3/network-module.h>

#include <ns3/mcptt-floor-msg.h>
#include <ns3/mcptt-floor-msg-field.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttFloorControlMsgTest");

namespace psc {
namespace tests {

class FloorMsgRequestTest : public TestCase
{
public:
  FloorMsgRequestTest (void);
  virtual void DoRun (void);
};

class FloorMsgGrantedTest : public TestCase
{
public:
  FloorMsgGrantedTest (void);
  virtual void DoRun (void);
};

class FloorMsgDenyTest : public TestCase
{
public:
  FloorMsgDenyTest (void);
  virtual void DoRun (void);
};

class FloorMsgReleaseTest : public TestCase
{
public:
  FloorMsgReleaseTest (void);
  virtual void DoRun (void);
};

class FloorMsgTakenTest : public TestCase
{
public:
  FloorMsgTakenTest (void);
  virtual void DoRun (void);
};

class FloorMsgQueuePositionRequestTest : public TestCase
{
public:
  FloorMsgQueuePositionRequestTest (void);
  virtual void DoRun (void);
};

class FloorMsgQueuePositionInfoTest : public TestCase
{
public:
  FloorMsgQueuePositionInfoTest (void);
  virtual void DoRun (void);
};

class McpttFloorControlMsgTestSuite : public TestSuite
{
public:
  McpttFloorControlMsgTestSuite (void);
};

/***************************************************************
 *  Implementation of the code declared above.
 ***************************************************************/

static McpttFloorControlMsgTestSuite suite;

FloorMsgRequestTest::FloorMsgRequestTest (void)
  : TestCase ("Floor Request")
{}

void
FloorMsgRequestTest::DoRun (void)
{
  McpttFloorMsgFieldIndic indic = McpttFloorMsgFieldIndic ();
  indic.Indicate (McpttFloorMsgFieldIndic::BROADCAST_CALL);

  McpttFloorMsgFieldPriority priority = McpttFloorMsgFieldPriority ();
  priority.SetPriority (1);

  McpttFloorMsgFieldTrackInfo trackInfo = McpttFloorMsgFieldTrackInfo ();
  trackInfo.SetQueueCap (1);
  trackInfo.AddRef (5);

  McpttFloorMsgFieldUserId id = McpttFloorMsgFieldUserId ();
  id.SetUserId (9);

  McpttFloorMsgRequest dstMsg;
  McpttFloorMsgRequest srcMsg;

  srcMsg.SetIndicator (indic);
  srcMsg.SetPriority (priority);
  srcMsg.UpdateTrackInfo (trackInfo);
  srcMsg.SetUserId (id);

  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (srcMsg);
  p->RemoveHeader (dstMsg);

  std::stringstream dstStr;
  std::stringstream srcStr;

  dstMsg.Print (dstStr);
  srcMsg.Print (srcStr);

  NS_LOG_LOGIC ("Serialized  : " << srcStr.str ());
  NS_LOG_LOGIC ("Deserialized: " << dstStr.str ());

  NS_TEST_ASSERT_MSG_EQ ((dstStr.str () == srcStr.str ()), true, "The serialized and deserialized messages do not match.");
  NS_TEST_ASSERT_MSG_EQ ((p->GetSize () == 0), true, "Bytes written/read do not match reported size.");
}

FloorMsgGrantedTest::FloorMsgGrantedTest (void)
  : TestCase ("Floor Granted")
{}

void
FloorMsgGrantedTest::DoRun (void)
{
  McpttFloorMsgFieldDuration duration;
  duration.SetDuration (50);

  uint32_t grantedSsrc = 100;

  McpttFloorMsgFieldIndic indic = McpttFloorMsgFieldIndic ();
  indic.Indicate (McpttFloorMsgFieldIndic::BROADCAST_CALL);

  McpttFloorMsgFieldPriority priority = McpttFloorMsgFieldPriority ();
  priority.SetPriority (1);

  McpttFloorMsgFieldTrackInfo trackInfo = McpttFloorMsgFieldTrackInfo ();
  trackInfo.SetQueueCap (1);

  McpttFloorMsgFieldUserId id = McpttFloorMsgFieldUserId ();
  id.SetUserId (9);

  McpttFloorMsgFieldQueuePositionInfo queuedInfoField;
  queuedInfoField.SetPosition (0);
  queuedInfoField.SetPriority (1);

  uint32_t queuedSsrc = 3;

  McpttFloorMsgFieldQueuedUserId queuedId;
  queuedId.SetUserId (22);

  McpttQueuedUserInfo queuedInfo (queuedSsrc, queuedId, queuedInfoField);

  McpttFloorMsgGranted dstMsg;
  McpttFloorMsgGranted srcMsg;
  srcMsg.SetDuration (duration);
  srcMsg.SetGrantedSsrc (grantedSsrc);
  srcMsg.SetIndicator (indic);
  srcMsg.SetPriority (priority);
  srcMsg.UpdateTrackInfo (trackInfo);
  srcMsg.SetUserId (id);
  srcMsg.AddUserInfo (queuedInfo);

  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (srcMsg);
  p->RemoveHeader (dstMsg);

  std::stringstream dstStr;
  std::stringstream srcStr;

  dstMsg.Print (dstStr);
  srcMsg.Print (srcStr);

  NS_LOG_LOGIC ("Serialized  : " << srcStr.str ());
  NS_LOG_LOGIC ("Deserialized: " << dstStr.str ());

  NS_TEST_ASSERT_MSG_EQ ((dstStr.str () == srcStr.str ()), true, "The serialized and deserialized messages do not match.");
  NS_TEST_ASSERT_MSG_EQ ((p->GetSize () == 0), true, "Bytes written/read do not match reported size.");
}

FloorMsgDenyTest::FloorMsgDenyTest (void)
  : TestCase ("Floor Deny")
{}

void
FloorMsgDenyTest::DoRun (void)
{
  McpttFloorMsgFieldRejectCause rejCause (McpttFloorMsgFieldRejectCause::CAUSE_255);

  McpttFloorMsgFieldUserId id = McpttFloorMsgFieldUserId ();
  id.SetUserId (9);

  McpttFloorMsgFieldTrackInfo trackInfo = McpttFloorMsgFieldTrackInfo ();
  trackInfo.SetQueueCap (1);
  trackInfo.AddRef (5);

  McpttFloorMsgDeny dstMsg;
  McpttFloorMsgDeny srcMsg;
  srcMsg.SetRejCause (rejCause);
  srcMsg.SetUserId (id);
  srcMsg.UpdateTrackInfo (trackInfo);

  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (srcMsg);
  p->RemoveHeader (dstMsg);

  std::stringstream dstStr;
  std::stringstream srcStr;

  dstMsg.Print (dstStr);
  srcMsg.Print (srcStr);

  NS_LOG_LOGIC ("Serialized  : " << srcStr.str ());
  NS_LOG_LOGIC ("Deserialized: " << dstStr.str ());

  NS_TEST_ASSERT_MSG_EQ ((dstStr.str () == srcStr.str ()), true, "The serialized and deserialized messages do not match.");
  NS_TEST_ASSERT_MSG_EQ ((p->GetSize () == 0), true, "Bytes written/read do not match reported size.");
}

FloorMsgReleaseTest::FloorMsgReleaseTest (void)
  : TestCase ("Floor Release")
{}

void
FloorMsgReleaseTest::DoRun (void)
{
  McpttFloorMsgFieldUserId id = McpttFloorMsgFieldUserId ();
  id.SetUserId (9);

  McpttFloorMsgFieldTrackInfo trackInfo = McpttFloorMsgFieldTrackInfo ();
  trackInfo.SetQueueCap (0);
  trackInfo.AddRef (5);

  McpttFloorMsgFieldIndic indic = McpttFloorMsgFieldIndic ();
  indic.Indicate (McpttFloorMsgFieldIndic::BROADCAST_CALL);

  McpttFloorMsgRelease dstMsg;
  McpttFloorMsgRelease srcMsg;
  srcMsg.SetUserId (id);
  srcMsg.UpdateTrackInfo (trackInfo);
  srcMsg.SetIndicator (indic);

  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (srcMsg);
  p->RemoveHeader (dstMsg);

  std::stringstream dstStr;
  std::stringstream srcStr;

  dstMsg.Print (dstStr);
  srcMsg.Print (srcStr);

  NS_LOG_LOGIC ("Serialized  : " << srcStr.str ());
  NS_LOG_LOGIC ("Deserialized: " << dstStr.str ());

  NS_TEST_ASSERT_MSG_EQ ((dstStr.str () == srcStr.str ()), true, "The serialized and deserialized messages do not match.");
  NS_TEST_ASSERT_MSG_EQ ((p->GetSize () == 0), true, "Bytes written/read do not match reported size.");
}

FloorMsgTakenTest::FloorMsgTakenTest (void)
  : TestCase ("Floor Taken")
{}

void
FloorMsgTakenTest::DoRun (void)
{
  McpttFloorMsgFieldIndic indic = McpttFloorMsgFieldIndic ();
  indic.Indicate (McpttFloorMsgFieldIndic::BROADCAST_CALL);

  McpttFloorMsgFieldGrantedPartyId partyId;
  partyId.SetPartyId (2);

  McpttFloorMsgFieldPermToReq permToReq;
  permToReq.SetPermission (1);

  McpttFloorMsgFieldSeqNum seqNum;
  seqNum.SetSeqNum (2);

  McpttFloorMsgFieldUserId id = McpttFloorMsgFieldUserId ();
  id.SetUserId (9);

  McpttFloorMsgFieldTrackInfo trackInfo = McpttFloorMsgFieldTrackInfo ();
  trackInfo.SetQueueCap (0);
  trackInfo.AddRef (5);

  McpttFloorMsgTaken dstMsg;
  McpttFloorMsgTaken srcMsg;
  srcMsg.UpdateTrackInfo (trackInfo);
  srcMsg.SetIndicator (indic);
  srcMsg.SetPartyId (partyId);
  srcMsg.SetPermission (permToReq);
  srcMsg.SetSeqNum (seqNum);

  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (srcMsg);
  p->RemoveHeader (dstMsg);

  std::stringstream dstStr;
  std::stringstream srcStr;

  dstMsg.Print (dstStr);
  srcMsg.Print (srcStr);

  NS_LOG_LOGIC ("Serialized  : " << srcStr.str ());
  NS_LOG_LOGIC ("Deserialized: " << dstStr.str ());

  NS_TEST_ASSERT_MSG_EQ ((dstStr.str () == srcStr.str ()), true, "The serialized and deserialized messages do not match.");
  NS_TEST_ASSERT_MSG_EQ ((p->GetSize () == 0), true, "Bytes written/read do not match reported size.");
}

FloorMsgQueuePositionRequestTest::FloorMsgQueuePositionRequestTest (void)
  : TestCase ("Queue Position Request")
{}

void
FloorMsgQueuePositionRequestTest::DoRun (void)
{
  McpttFloorMsgFieldUserId id = McpttFloorMsgFieldUserId ();
  id.SetUserId (9);

  McpttFloorMsgFieldTrackInfo trackInfo = McpttFloorMsgFieldTrackInfo ();
  trackInfo.SetQueueCap (0);
  trackInfo.AddRef (5);

  McpttFloorMsgQueuePositionRequest dstMsg;
  McpttFloorMsgQueuePositionRequest srcMsg;
  srcMsg.SetUserId (id);
  srcMsg.UpdateTrackInfo (trackInfo);

  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (srcMsg);
  p->RemoveHeader (dstMsg);

  std::stringstream dstStr;
  std::stringstream srcStr;

  dstMsg.Print (dstStr);
  srcMsg.Print (srcStr);

  NS_LOG_LOGIC ("Serialized  : " << srcStr.str ());
  NS_LOG_LOGIC ("Deserialized: " << dstStr.str ());

  NS_TEST_ASSERT_MSG_EQ ((dstStr.str () == srcStr.str ()), true, "The serialized and deserialized messages do not match.");
  NS_TEST_ASSERT_MSG_EQ ((p->GetSize () == 0), true, "Bytes written/read do not match reported size.");
}

FloorMsgQueuePositionInfoTest::FloorMsgQueuePositionInfoTest (void)
  : TestCase ("Queue Info")
{}

void
FloorMsgQueuePositionInfoTest::DoRun (void)
{
  McpttFloorMsgFieldUserId id = McpttFloorMsgFieldUserId ();
  id.SetUserId (9);

  McpttFloorMsgFieldQueuePositionInfo queuedInfoField;
  queuedInfoField.SetPosition (0);
  queuedInfoField.SetPriority (1);

  uint32_t queuedSsrc = 3;

  McpttFloorMsgFieldQueuedUserId queuedId;
  queuedId.SetUserId (22);

  McpttFloorMsgFieldTrackInfo trackInfo = McpttFloorMsgFieldTrackInfo ();
  trackInfo.AddRef (5);

  McpttFloorMsgQueuePositionInfo dstMsg;
  McpttFloorMsgQueuePositionInfo srcMsg;
  srcMsg.SetUserId (id);
  srcMsg.SetQueuedSsrc (queuedSsrc);
  srcMsg.SetQueuedUserId (queuedId);
  srcMsg.SetQueuePositionInfo (queuedInfoField);
  srcMsg.UpdateTrackInfo (trackInfo);

  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader (srcMsg);
  p->RemoveHeader (dstMsg);

  std::stringstream dstStr;
  std::stringstream srcStr;

  dstMsg.Print (dstStr);
  srcMsg.Print (srcStr);

  NS_LOG_LOGIC ("Serialized  : " << srcStr.str ());
  NS_LOG_LOGIC ("Deserialized: " << dstStr.str ());

  NS_TEST_ASSERT_MSG_EQ ((dstStr.str () == srcStr.str ()), true, "The serialized and deserialized messages do not match.");
  NS_TEST_ASSERT_MSG_EQ ((p->GetSize () == 0), true, "Bytes written/read do not match reported size.");
}

McpttFloorControlMsgTestSuite::McpttFloorControlMsgTestSuite (void)
  : TestSuite ("mcptt-floor-control-messages", TestSuite::UNIT)
{
  AddTestCase (new FloorMsgRequestTest (), TestCase::QUICK);
  AddTestCase (new FloorMsgGrantedTest (), TestCase::QUICK);
  AddTestCase (new FloorMsgDenyTest (), TestCase::QUICK);
  AddTestCase (new FloorMsgReleaseTest (), TestCase::QUICK);
  AddTestCase (new FloorMsgTakenTest (), TestCase::QUICK);
  AddTestCase (new FloorMsgQueuePositionRequestTest (), TestCase::QUICK);
  AddTestCase (new FloorMsgQueuePositionInfoTest (), TestCase::QUICK);
}

} // namespace tests
} // namespace psc
} // namespace ns3
