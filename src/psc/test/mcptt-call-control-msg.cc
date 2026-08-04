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

#include <ns3/core-module.h>
#include <ns3/mcptt-call-msg-field.h>
#include <ns3/mcptt-call-msg.h>
#include <ns3/network-module.h>

#include <sstream>
#include <string>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("McpttCallControlMsgTest");

namespace psc
{
namespace tests
{

class McpttCallMsgGrpProbeTest : public TestCase
{
  public:
    McpttCallMsgGrpProbeTest();
    void DoRun() override;
};

class McpttCallMsgGrpAnnounTest : public TestCase
{
  public:
    McpttCallMsgGrpAnnounTest();
    void DoRun() override;
};

class McpttCallMsgGrpAcceptTest : public TestCase
{
  public:
    McpttCallMsgGrpAcceptTest();
    void DoRun() override;
};

class McpttCallMsgGrpImmPerilEndTest : public TestCase
{
  public:
    McpttCallMsgGrpImmPerilEndTest();
    void DoRun() override;
};

class McpttCallMsgGrpEmergEndTest : public TestCase
{
  public:
    McpttCallMsgGrpEmergEndTest();
    void DoRun() override;
};

class McpttCallMsgGrpEmergAlertTest : public TestCase
{
  public:
    McpttCallMsgGrpEmergAlertTest();
    void DoRun() override;
};

class McpttCallMsgGrpEmergAlertAckTest : public TestCase
{
  public:
    McpttCallMsgGrpEmergAlertAckTest();
    void DoRun() override;
};

class McpttCallMsgGrpEmergAlertCancelTest : public TestCase
{
  public:
    McpttCallMsgGrpEmergAlertCancelTest();
    void DoRun() override;
};

class McpttCallMsgGrpEmergAlertCancelAckTest : public TestCase
{
  public:
    McpttCallMsgGrpEmergAlertCancelAckTest();
    void DoRun() override;
};

class McpttCallControlMsgTestSuite : public TestSuite
{
  public:
    McpttCallControlMsgTestSuite();
};

/***************************************************************
 *  Implementation of the code declared above.
 ***************************************************************/

static McpttCallControlMsgTestSuite suite;

McpttCallMsgGrpProbeTest::McpttCallMsgGrpProbeTest()
    : TestCase("GROUP CALL PROBE")
{
}

void
McpttCallMsgGrpProbeTest::DoRun()
{
    McpttCallMsgFieldGrpId grpId = McpttCallMsgFieldGrpId();
    grpId.SetGrpId(10);

    McpttCallMsgGrpProbe dstMsg;
    McpttCallMsgGrpProbe srcMsg;
    srcMsg.SetGrpId(grpId);

    std::stringstream dstStr;
    std::stringstream srcStr;

    Ptr<Packet> p = Create<Packet>();
    p->AddHeader(srcMsg);
    p->RemoveHeader(dstMsg);

    dstMsg.Print(dstStr);
    srcMsg.Print(srcStr);

    NS_LOG_LOGIC("Serialized  : " << srcStr.str());
    NS_LOG_LOGIC("Deserialized: " << dstStr.str());

    NS_TEST_ASSERT_MSG_EQ((dstStr.str() == srcStr.str()),
                          true,
                          "The serialized and deserialized messages do not match.");
}

McpttCallMsgGrpAnnounTest::McpttCallMsgGrpAnnounTest()
    : TestCase("GROUP CALL ANNOUNCEMENT")
{
}

void
McpttCallMsgGrpAnnounTest::DoRun()
{
    McpttCallMsgFieldCallId callId;
    callId.SetCallId(12);

    McpttCallMsgFieldCallType callType;
    callType.SetType(McpttCallMsgFieldCallType::EMERGENCY_GROUP);

    bool confirmMode = false;

    McpttCallMsgFieldGrpId grpId;
    grpId.SetGrpId(13);

    McpttCallMsgFieldLastChgTime lastChgTime;
    lastChgTime.SetTime(Seconds(4));

    McpttCallMsgFieldUserId lastChgUserId;
    lastChgUserId.SetId(15);

    McpttCallMsgFieldUserId origUserId;
    origUserId.SetId(16);

    bool probeResp = true;

    McpttCallMsgFieldRefreshInterval refInt;
    refInt.SetInterval(1000);

    McpttCallMsgFieldSdp sdp;
    sdp.SetFloorPort(17);
    sdp.SetSpeechPort(18);
    sdp.SetGrpAddr(Ipv4Address("129.6.142.230"));
    sdp.SetOrigAddr(Ipv4Address("192.168.1.1"));

    McpttCallMsgFieldStartTime startTime;
    startTime.SetTime(Seconds(1));

    McpttCallMsgGrpAnnoun dstMsg;
    McpttCallMsgGrpAnnoun srcMsg;
    srcMsg.SetCallId(callId);
    srcMsg.SetCallType(callType);
    srcMsg.SetConfirmMode(confirmMode);
    srcMsg.SetGrpId(grpId);
    srcMsg.SetLastChgTime(lastChgTime);
    srcMsg.SetLastChgUserId(lastChgUserId);
    srcMsg.SetOrigId(origUserId);
    srcMsg.SetProbeResp(probeResp);
    srcMsg.SetRefInt(refInt);
    srcMsg.SetSdp(sdp);
    srcMsg.SetStartTime(startTime);

    std::stringstream dstStr;
    std::stringstream srcStr;

    Ptr<Packet> p = Create<Packet>();
    p->AddHeader(srcMsg);
    p->RemoveHeader(dstMsg);

    dstMsg.Print(dstStr);
    srcMsg.Print(srcStr);

    NS_LOG_LOGIC("Serialized  : " << srcStr.str());
    NS_LOG_LOGIC("Deserialized: " << dstStr.str());

    NS_TEST_ASSERT_MSG_EQ((dstStr.str() == srcStr.str()),
                          true,
                          "The serialized and deserialized messages do not match.");
}

McpttCallMsgGrpAcceptTest::McpttCallMsgGrpAcceptTest()
    : TestCase("GROUP CALL ACCEPT")
{
}

void
McpttCallMsgGrpAcceptTest::DoRun()
{
    McpttCallMsgFieldCallId callId;
    callId.SetCallId(2);

    McpttCallMsgFieldCallType callType;
    callType.SetType(McpttCallMsgFieldCallType::PRIVATE);

    McpttCallMsgFieldGrpId grpId;
    grpId.SetGrpId(101);

    McpttCallMsgFieldUserId userId;
    userId.SetId(11);

    McpttCallMsgGrpAccept dstMsg;
    McpttCallMsgGrpAccept srcMsg;
    srcMsg.SetCallId(callId);
    srcMsg.SetCallType(callType);
    srcMsg.SetGrpId(grpId);
    srcMsg.SetUserId(userId);

    std::stringstream dstStr;
    std::stringstream srcStr;

    Ptr<Packet> p = Create<Packet>();
    p->AddHeader(srcMsg);
    p->RemoveHeader(dstMsg);

    dstMsg.Print(dstStr);
    srcMsg.Print(srcStr);

    NS_LOG_LOGIC("Serialized  : " << srcStr.str());
    NS_LOG_LOGIC("Deserialized: " << dstStr.str());

    NS_TEST_ASSERT_MSG_EQ((dstStr.str() == srcStr.str()),
                          true,
                          "The serialized and deserialized messages do not match.");
}

McpttCallMsgGrpImmPerilEndTest::McpttCallMsgGrpImmPerilEndTest()
    : TestCase("GROUP CALL IMMINENT PERIL END")
{
}

void
McpttCallMsgGrpImmPerilEndTest::DoRun()
{
    McpttCallMsgFieldCallId callId;
    callId.SetCallId(5);

    McpttCallMsgFieldGrpId grpId;
    grpId.SetGrpId(6);

    McpttCallMsgFieldLastChgTime lastChgTime;
    lastChgTime.SetTime(Seconds(3));

    McpttCallMsgFieldUserId lastChgUserId;
    lastChgUserId.SetId(7);

    McpttCallMsgFieldUserId userId;
    userId.SetId(8);

    McpttCallMsgGrpImmPerilEnd dstMsg;
    McpttCallMsgGrpImmPerilEnd srcMsg;
    srcMsg.SetCallId(callId);
    srcMsg.SetGrpId(grpId);
    srcMsg.SetLastChgTime(lastChgTime);
    srcMsg.SetLastChgUserId(lastChgUserId);
    srcMsg.SetUserId(userId);

    std::stringstream dstStr;
    std::stringstream srcStr;

    Ptr<Packet> p = Create<Packet>();
    p->AddHeader(srcMsg);
    p->RemoveHeader(dstMsg);

    dstMsg.Print(dstStr);
    srcMsg.Print(srcStr);

    NS_LOG_LOGIC("Serialized  : " << srcStr.str());
    NS_LOG_LOGIC("Deserialized: " << dstStr.str());

    NS_TEST_ASSERT_MSG_EQ((dstStr.str() == srcStr.str()),
                          true,
                          "The serialized and deserialized messages do not match.");
}

McpttCallMsgGrpEmergEndTest::McpttCallMsgGrpEmergEndTest()
    : TestCase("GROUP CALL EMERGENCY END")
{
}

void
McpttCallMsgGrpEmergEndTest::DoRun()
{
    McpttCallMsgFieldCallId callId;
    callId.SetCallId(5);

    McpttCallMsgFieldGrpId grpId;
    grpId.SetGrpId(6);

    McpttCallMsgFieldLastChgTime lastChgTime;
    lastChgTime.SetTime(Seconds(3));

    McpttCallMsgFieldUserId lastChgUserId;
    lastChgUserId.SetId(7);

    McpttCallMsgFieldUserId userId;
    userId.SetId(8);

    McpttCallMsgGrpEmergEnd dstMsg;
    McpttCallMsgGrpEmergEnd srcMsg;
    srcMsg.SetCallId(callId);
    srcMsg.SetGrpId(grpId);
    srcMsg.SetLastChgTime(lastChgTime);
    srcMsg.SetLastChgUserId(lastChgUserId);
    srcMsg.SetUserId(userId);

    std::stringstream dstStr;
    std::stringstream srcStr;

    Ptr<Packet> p = Create<Packet>();
    p->AddHeader(srcMsg);
    p->RemoveHeader(dstMsg);

    dstMsg.Print(dstStr);
    srcMsg.Print(srcStr);

    NS_LOG_LOGIC("Serialized  : " << srcStr.str());
    NS_LOG_LOGIC("Deserialized: " << dstStr.str());

    NS_TEST_ASSERT_MSG_EQ((dstStr.str() == srcStr.str()),
                          true,
                          "The serialized and deserialized messages do not match.");
}

McpttCallMsgGrpEmergAlertTest::McpttCallMsgGrpEmergAlertTest()
    : TestCase("GROUP CALL EMERGENCY ALERT")
{
}

void
McpttCallMsgGrpEmergAlertTest::DoRun()
{
    McpttCallMsgFieldGrpId grpId;
    grpId.SetGrpId(1);

    McpttCallMsgFieldOrgName orgName;
    orgName.SetName("NIST");

    McpttCallMsgFieldUserId userId;
    userId.SetId(2);

    McpttCallMsgFieldUserLoc userLoc;
    userLoc.SetLoc(Vector(1.0, 2.0, 3.0));

    McpttCallMsgGrpEmergAlert dstMsg;
    McpttCallMsgGrpEmergAlert srcMsg;
    srcMsg.SetGrpId(grpId);
    srcMsg.SetOrgName(orgName);
    srcMsg.SetUserId(userId);
    srcMsg.SetUserLoc(userLoc);

    std::stringstream dstStr;
    std::stringstream srcStr;

    Ptr<Packet> p = Create<Packet>();
    p->AddHeader(srcMsg);
    p->RemoveHeader(dstMsg);

    dstMsg.Print(dstStr);
    srcMsg.Print(srcStr);

    NS_LOG_LOGIC("Serialized  : " << srcStr.str());
    NS_LOG_LOGIC("Deserialized: " << dstStr.str());

    NS_TEST_ASSERT_MSG_EQ((dstStr.str() == srcStr.str()),
                          true,
                          "The serialized and deserialized messages do not match.");
}

McpttCallMsgGrpEmergAlertAckTest::McpttCallMsgGrpEmergAlertAckTest()
    : TestCase("GROUP CALL EMERGENCY ALERT ACK")
{
}

void
McpttCallMsgGrpEmergAlertAckTest::DoRun()
{
    McpttCallMsgFieldGrpId grpId;
    grpId.SetGrpId(95);

    McpttCallMsgFieldUserId origId;
    origId.SetId(96);

    McpttCallMsgFieldUserId sendingId;
    sendingId.SetId(97);

    McpttCallMsgGrpEmergAlertAck dstMsg;
    McpttCallMsgGrpEmergAlertAck srcMsg;
    srcMsg.SetGrpId(grpId);
    srcMsg.SetOrigId(origId);
    srcMsg.SetSendingId(sendingId);

    std::stringstream dstStr;
    std::stringstream srcStr;

    Ptr<Packet> p = Create<Packet>();
    p->AddHeader(srcMsg);
    p->RemoveHeader(dstMsg);

    dstMsg.Print(dstStr);
    srcMsg.Print(srcStr);

    NS_LOG_LOGIC("Serialized  : " << srcStr.str());
    NS_LOG_LOGIC("Deserialized: " << dstStr.str());

    NS_TEST_ASSERT_MSG_EQ((dstStr.str() == srcStr.str()),
                          true,
                          "The serialized and deserialized messages do not match.");
}

McpttCallMsgGrpEmergAlertCancelTest::McpttCallMsgGrpEmergAlertCancelTest()
    : TestCase("GROUP CALL EMERGENCY ALERT CANCEL")
{
}

void
McpttCallMsgGrpEmergAlertCancelTest::DoRun()
{
    McpttCallMsgFieldGrpId grpId;
    grpId.SetGrpId(95);

    McpttCallMsgFieldUserId origId;
    origId.SetId(96);

    McpttCallMsgFieldUserId sendingId;
    sendingId.SetId(97);

    McpttCallMsgGrpEmergAlertCancel dstMsg;
    McpttCallMsgGrpEmergAlertCancel srcMsg;
    srcMsg.SetGrpId(grpId);
    srcMsg.SetOrigId(origId);
    srcMsg.SetSendingId(sendingId);

    std::stringstream dstStr;
    std::stringstream srcStr;

    Ptr<Packet> p = Create<Packet>();
    p->AddHeader(srcMsg);
    p->RemoveHeader(dstMsg);

    dstMsg.Print(dstStr);
    srcMsg.Print(srcStr);

    NS_LOG_LOGIC("Serialized  : " << srcStr.str());
    NS_LOG_LOGIC("Deserialized: " << dstStr.str());

    NS_TEST_ASSERT_MSG_EQ((dstStr.str() == srcStr.str()),
                          true,
                          "The serialized and deserialized messages do not match.");
}

McpttCallMsgGrpEmergAlertCancelAckTest::McpttCallMsgGrpEmergAlertCancelAckTest()
    : TestCase("GROUP CALL EMERGENCY ALERT CANCEL ACK")
{
}

void
McpttCallMsgGrpEmergAlertCancelAckTest::DoRun()
{
    McpttCallMsgFieldGrpId grpId;
    grpId.SetGrpId(95);

    McpttCallMsgFieldUserId origId;
    origId.SetId(96);

    McpttCallMsgFieldUserId sendingId;
    sendingId.SetId(97);

    McpttCallMsgGrpEmergAlertCancelAck dstMsg;
    McpttCallMsgGrpEmergAlertCancelAck srcMsg;
    srcMsg.SetGrpId(grpId);
    srcMsg.SetOrigId(origId);
    srcMsg.SetSendingId(sendingId);

    std::stringstream dstStr;
    std::stringstream srcStr;

    Ptr<Packet> p = Create<Packet>();
    p->AddHeader(srcMsg);
    p->RemoveHeader(dstMsg);

    dstMsg.Print(dstStr);
    srcMsg.Print(srcStr);

    NS_LOG_LOGIC("Serialized  : " << srcStr.str());
    NS_LOG_LOGIC("Deserialized: " << dstStr.str());

    NS_TEST_ASSERT_MSG_EQ((dstStr.str() == srcStr.str()),
                          true,
                          "The serialized and deserialized messages do not match.");
}

McpttCallControlMsgTestSuite::McpttCallControlMsgTestSuite()
    : TestSuite("mcptt-call-control-messages", TestSuite::Type::UNIT)
{
    AddTestCase(new McpttCallMsgGrpProbeTest(), TestCase::Duration::QUICK);
    AddTestCase(new McpttCallMsgGrpAnnounTest(), TestCase::Duration::QUICK);
    AddTestCase(new McpttCallMsgGrpAcceptTest(), TestCase::Duration::QUICK);
    AddTestCase(new McpttCallMsgGrpImmPerilEndTest(), TestCase::Duration::QUICK);
    AddTestCase(new McpttCallMsgGrpEmergEndTest(), TestCase::Duration::QUICK);
    AddTestCase(new McpttCallMsgGrpEmergAlertTest(), TestCase::Duration::QUICK);
    AddTestCase(new McpttCallMsgGrpEmergAlertAckTest(), TestCase::Duration::QUICK);
    AddTestCase(new McpttCallMsgGrpEmergAlertCancelTest(), TestCase::Duration::QUICK);
    AddTestCase(new McpttCallMsgGrpEmergAlertCancelAckTest(), TestCase::Duration::QUICK);
}

} // namespace tests
} // namespace psc
} // namespace ns3
