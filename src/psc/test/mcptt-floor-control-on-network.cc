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

namespace ns3 {
namespace psc {
namespace tests {

NS_LOG_COMPONENT_DEFINE ("McpttTestSuiteFloorControlOnNetwork");

/**
 * UE 1 releases the floor after initiating the call.
 */
class McpttTestCaseOnNetworkFloorRelease : public McpttTestCase
{
public:
  McpttTestCaseOnNetworkFloorRelease (const std::string& name = "Floor Release", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

private:
  bool m_ue1TxFloorRelease {false};
  bool m_ue1RxFloorAck {false};
  bool m_ue2RxFloorIdle {false};
};

/**
 * After UE 1 initiates the call and then release the floor, UE 2 will
 * request and then be granted the floor.
 */
class McpttTestCaseOnNetworkFloorGranted : public McpttTestCase
{
public:
  McpttTestCaseOnNetworkFloorGranted (const std::string& name = "Floor Granted", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

private:
  bool m_ue1TxFloorRelease {false};
  bool m_ue1RxFloorAck {false};
  bool m_ue2RxFloorIdle {false};
  bool m_ue2TxFloorAck1 {false};
  bool m_ue2TxFloorRequest {false};
  bool m_ue2RxFloorGranted {false};
  bool m_ue2TxFloorAck2 {false};
};

/**
 * After the call is initiated by UE 1, UE 2 will send a floor request. Since
 * UE 2 has a higher priority than UE 1 and audio cut-in is enabled this will
 * result in UE 1 having the floor revoked and then UE 2 being granted the
 * floor.
 */
class McpttTestCaseOnNetworkFloorRevoke : public McpttTestCase
{
public:
  McpttTestCaseOnNetworkFloorRevoke (const std::string& name = "Floor Revoke", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

private:
  bool m_ue2TxFloorRequest {false};
  bool m_ue1RxFloorRevoke {false};
  bool m_ue1TxFloorRelease {false};
  bool m_ue2RxFloorGranted {false};
  bool m_ue2TxFloorAck {false};
  bool m_ue1RxFloorTaken {false};
};

/**
 * After UE 1 initiates the call, UE 2 will request the floor. This will
 * result in UE 2 being denied the floor.
 */
class McpttTestCaseOnNetworkFloorDeny : public McpttTestCase
{
public:
  McpttTestCaseOnNetworkFloorDeny (const std::string& name = "Floor Deny", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

private:
  bool m_ue2TxFloorRequest {false};
  bool m_ue2RxFloorDeny {false};
};

/**
 * After UE 1 intiates the call UE 2 will request the floor, and then be
 * queued because UE 1 currently has the floor and queuing of floor requests
 * is enabled. After being queued UE 2 will request queue position
 * information. After receiving the information, the original request will
 * then be canceled by UE 2.
 */
class McpttTestCaseOnNetworkFloorQueueAndCancel : public McpttTestCase
{
public:
  McpttTestCaseOnNetworkFloorQueueAndCancel (const std::string& name = "Floor Queue and Cancel", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

private:
  bool m_ue2TxFloorRequest {false};
  bool m_ue2RxFloorQueuePositionInfo {false};
  bool m_ue2TxFloorQueuePositionRequest {false};
  bool m_ue2RxFloorQueuePositionInfoTwo {false};
  bool m_ue2TxFloorRelease {false};
  bool m_ue2RxFloorAck {false};
};

/**
 * After UE 1 intiates the call UE 2 will request the floor, and then be
 * queued because UE 1 currently has the floor and queuing of floor requests
 * is enabled. After being queued UE 2 will request queue position
 * information. Some time after UE 2 receives the information UE 1 will
 * release the floor, resulting in UE 2 being granted the floor.
 * */
class McpttTestCaseOnNetworkFloorQueueAndGranted : public McpttTestCase
{
public:
  McpttTestCaseOnNetworkFloorQueueAndGranted (const std::string& name = "Floor Queue and Granted", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

private:
  bool m_ue2TxFloorRequest {false};
  bool m_ue2RxFloorQueuePositionInfo {false};
  bool m_ue2TxFloorAck1 {false};
  bool m_ue1TxFloorRelease {false};
  bool m_ue1RxFloorAck {false};
  bool m_ue2RxFloorGranted {false};
  bool m_ue2TxFloorAck2 {false};
};
 
/**
 * After UE 1 initiates the call, UE 2 will request the floor. Because UE 2
 * does not have a higher priority then UE 1, UE 2 will be denied the floor.
 * Shortly after, UE 3 will request the floor. Because UE 3 has a higher
 * priority than UE 1, and dual floor control is enabled, UE 3 will be
 * be granted the floor. At this point UE 2 should be receiving media from
 * both UE 1 and UE 3 as they share the floor. Then UE 1 will release the
 * floor, followed by UE 3 releasing the floor.
 */
class McpttTestCaseOnNetworkDualFloorControl : public McpttTestCase
{
public:
  McpttTestCaseOnNetworkDualFloorControl (const std::string& name = "Dual Floor Control", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

 protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);
  virtual void Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

private:
  bool m_ue3RxFloorTaken {false};
  bool m_ue2RxFloorTaken {false};
  bool m_ue2TxFloorRequest {false};
  bool m_ue2RxFloorDeny {false};
  bool m_ue2TxFloorAck2 {false};
  bool m_ue3TxFloorRequest {false};
  bool m_ue3RxFloorGranted {false};
  bool m_ue3TxFloorAck {false};
  bool m_ue1RxFloorIdle {false}; 
  bool m_ue2RxFloorIdle {false};
  bool m_ue2TxFloorAck {false};
  bool m_ue1RxFloorTaken1 {false};
  bool m_ue2RxFloorTaken2 {false}; 
  bool m_ue2TxFloorAck3 {false}; 
  bool m_ue1TxFloorRelease {false};
  bool m_ue1RxFloorAck {false};
  bool m_ue2RxFloorIdle2 {false};
  bool m_ue2TxFloorAck4 {false};
  bool m_ue3RxFloorIdle {false};
  bool m_ue3TxFloorAck2 {false};
  bool m_ue3TxFloorRelease {false};
  bool m_ue3RxFloorAck {false};
  bool m_ue1RxFloorTaken2 {false};
  bool m_ue2RxFloorTaken3 {false};
  bool m_ue2TxFloorAck5 {false};
};

class McpttTestSuiteFloorControlOnNetwork : public TestSuite
{
public:
  McpttTestSuiteFloorControlOnNetwork (void);
};

/***************************************************************
 *  Implementation of the code declared above.
 ***************************************************************/

static McpttTestSuiteFloorControlOnNetwork suite;

McpttTestCaseOnNetworkFloorRelease::McpttTestCaseOnNetworkFloorRelease (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config)
{ }

void
McpttTestCaseOnNetworkFloorRelease::Configure (void)
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
                            "TxSsrc", UintegerValue (100),
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
  config->SetAppCount (3);
  config->SetClientHelper (clientHelper);
  config->SetCallHelper (callHelper);

  SetConfig (config);

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ue1App = GetApp (0);
  Ptr<McpttPttApp> ue2App = GetApp (1);
  Ptr<McpttPttApp> ue3App = GetApp (2);

  ue1App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRelease::Ue1RxCb, this));
  ue1App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRelease::Ue1TxCb, this));
  ue2App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRelease::Ue2RxCb, this));
  ue2App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRelease::Ue2TxCb, this));
  ue3App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRelease::Ue3RxCb, this));
  ue3App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRelease::Ue3TxCb, this));

  Ptr<McpttPusher> ue1Pusher = ue1App->GetPusher ();
  ue1Pusher->SchedulePush (Seconds (3.0));
  ue1Pusher->ScheduleRelease (Seconds (4.0));
}

void
McpttTestCaseOnNetworkFloorRelease::Execute (void)
{
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor release.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive a floor Idle.");
}

void
McpttTestCaseOnNetworkFloorRelease::Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgAck::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send floor release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 already received floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 already received floor idle.");
      m_ue1RxFloorAck = true;
    }
}

void
McpttTestCaseOnNetworkFloorRelease::Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 already sent floor release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 already received floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 already received floor idle.");
      m_ue1TxFloorRelease = true;
    }
}

void
McpttTestCaseOnNetworkFloorRelease::Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgIdle::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send floor release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 already received floor idle.");
      m_ue2RxFloorIdle = true;
      Stop ();
    }
}

void
McpttTestCaseOnNetworkFloorRelease::Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

void
McpttTestCaseOnNetworkFloorRelease::Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

void
McpttTestCaseOnNetworkFloorRelease::Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

McpttTestCaseOnNetworkFloorGranted::McpttTestCaseOnNetworkFloorGranted (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config)
{ }

void
McpttTestCaseOnNetworkFloorGranted::Configure (void)
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
                            "TxSsrc", UintegerValue (100),
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
  config->SetAppCount (3);
  config->SetClientHelper (clientHelper);
  config->SetCallHelper (callHelper);

  SetConfig (config);

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ue1App = GetApp (0);
  Ptr<McpttPttApp> ue2App = GetApp (1);
  Ptr<McpttPttApp> ue3App = GetApp (2);

  ue1App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorGranted::Ue1RxCb, this));
  ue1App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorGranted::Ue1TxCb, this));
  ue2App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorGranted::Ue2RxCb, this));
  ue2App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorGranted::Ue2TxCb, this));
  ue3App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorGranted::Ue3RxCb, this));
  ue3App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorGranted::Ue3TxCb, this));

  Ptr<McpttPusher> ue1Pusher = ue1App->GetPusher ();
  ue1Pusher->SchedulePush (Seconds (3.0));
  ue1Pusher->ScheduleRelease (Seconds (4.0));

  Ptr<McpttPusher> ue2Pusher = ue2App->GetPusher ();
  ue2Pusher->SchedulePush (Seconds (6.0));
}

void
McpttTestCaseOnNetworkFloorGranted::Execute (void)
{
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor release.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive a floor Idle.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, true, "UE 1 did not send a floor Ack1.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, true, "UE 2 did not receive a floor Granted.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send a floor ACK.");
}

void
McpttTestCaseOnNetworkFloorGranted::Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgAck::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive a floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, false, "UE 1 did send a floor Ack1.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor ACK.");
      m_ue1RxFloorAck = true;
    }
}

void
McpttTestCaseOnNetworkFloorGranted::Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send a floor release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive a floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, false, "UE 1 did send a floor Ack1.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor ACK.");
      m_ue1TxFloorRelease = true;
    }
}

void
McpttTestCaseOnNetworkFloorGranted::Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgIdle::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive a floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, false, "UE 1 did send a floor Ack1.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor ACK.");
      m_ue2RxFloorIdle = true;
    }
  else if (headerType == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive a floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, true, "UE 1 did not send a floor Ack1.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor ACK.");
      m_ue2RxFloorGranted = true;
    }
}

void
McpttTestCaseOnNetworkFloorGranted::Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgAck::GetTypeId ()
      && m_ue2TxFloorAck1 == false)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive a floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, false, "UE 1 did send a floor Ack1.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor ACK.");
      m_ue2TxFloorAck1 = true;
    }
  else if (headerType == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive a floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, true, "UE 1 did not send a floor Ack1.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor ACK.");
      m_ue2TxFloorRequest = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ()
           && m_ue2TxFloorAck1 == true)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive a floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, true, "UE 1 did not send a floor Ack1.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, true, "UE 2 did not receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor ACK.");
      m_ue2TxFloorAck2 = true;
      Stop ();
    }
}

void
McpttTestCaseOnNetworkFloorGranted::Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

void
McpttTestCaseOnNetworkFloorGranted::Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

McpttTestCaseOnNetworkFloorRevoke::McpttTestCaseOnNetworkFloorRevoke (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config)
{ }

void
McpttTestCaseOnNetworkFloorRevoke::Configure (void)
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
                            "AudioCutIn", BooleanValue (true),
                            "DualFloorSupported", BooleanValue (false),
                            "TxSsrc", UintegerValue (100),
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
  config->SetAppCount (3);
  config->SetClientHelper (clientHelper);
  config->SetCallHelper (callHelper);

  SetConfig (config);

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ue1App = GetApp (0);
  Ptr<McpttPttApp> ue2App = GetApp (1);
  Ptr<McpttPttApp> ue3App = GetApp (2);

  ue1App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRevoke::Ue1RxCb, this));
  ue1App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRevoke::Ue1TxCb, this));
  ue2App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRevoke::Ue2RxCb, this));
  ue2App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRevoke::Ue2TxCb, this));
  ue3App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRevoke::Ue3RxCb, this));
  ue3App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorRevoke::Ue3TxCb, this));

  Ptr<McpttPusher> ue1Pusher = ue1App->GetPusher ();
  ue1Pusher->SchedulePush (Seconds (3.0));
  ue1Pusher->ScheduleRelease (Seconds (6.0));

  Ptr<McpttPusher> ue2Pusher = ue2App->GetPusher ();
  ue2Pusher->SchedulePush (Seconds (5.0));

  std::map<uint16_t, Ptr<McpttCall> > calls = ue2App->GetCalls ();
  Ptr<McpttCall> ue2Call = calls.begin ()->second;
  Ptr<McpttFloorParticipant> ue2FloorParticipant = ue2Call->GetFloorMachine ();
  Ptr<McpttOnNetworkFloorParticipant> ue2OnNetworkFloorParticipant = DynamicCast<McpttOnNetworkFloorParticipant, McpttFloorParticipant> (ue2FloorParticipant);
  Simulator::Schedule (Seconds (4), &McpttOnNetworkFloorParticipant::SetPriority, ue2OnNetworkFloorParticipant, 2);
}

void McpttTestCaseOnNetworkFloorRevoke::Execute (void)
{
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorRevoke, true, "UE 1 did not receive a floor Revoke.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor Release.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, true, "UE 2 did not receive a floor Granted.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send a floor ACK.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken, true,"UE 1 did not receive a floor Taken.");

}


void
McpttTestCaseOnNetworkFloorRevoke::Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRevoke::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorRevoke, false, "UE 1 did receive a floor Revoke.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken, false,"UE 1 did not receive a floor Taken.");
      m_ue1RxFloorRevoke = true;
    }
  else if (headerType == McpttFloorMsgTaken::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorRevoke, true, "UE 1 did not receive a floor Revoke.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, true, "UE 2 did not receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken, false,"UE 1 did not receive a floor Taken.");
      m_ue1RxFloorTaken = true;
      Stop ();
    }
}

void
McpttTestCaseOnNetworkFloorRevoke::Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorRevoke, true, "UE 1 did not receive a floor Revoke.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken, false,"UE 1 did not receive a floor Taken.");
      m_ue1TxFloorRelease = true;
    }
}


void
McpttTestCaseOnNetworkFloorRevoke::Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorRevoke, true, "UE 1 did not receive a floor Revoke.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken, false,"UE 1 did not receive a floor Taken.");
      m_ue2RxFloorGranted = true;
    }
}

void
McpttTestCaseOnNetworkFloorRevoke::Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorRevoke, false, "UE 1 did receive a floor Revoke.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken, false,"UE 1 did not receive a floor Taken.");
      m_ue2TxFloorRequest = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorRevoke, true, "UE 1 did not receive a floor Revoke.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, true, "UE 2 did not receive a floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken, false,"UE 1 did not receive a floor Taken.");
      m_ue2TxFloorAck = true;
    }
}

void
McpttTestCaseOnNetworkFloorRevoke::Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

void
McpttTestCaseOnNetworkFloorRevoke::Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

McpttTestCaseOnNetworkFloorDeny::McpttTestCaseOnNetworkFloorDeny (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config)
{ }

void
McpttTestCaseOnNetworkFloorDeny::Configure (void)
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
                            "TxSsrc", UintegerValue (100),
                            "QueueingSupported", BooleanValue (false));
  callHelper.SetTowardsParticipant ("ns3::psc::McpttOnNetworkFloorTowardsParticipant",
                                    "ReceiveOnly", BooleanValue (false));
  callHelper.SetParticipant ("ns3::psc::McpttOnNetworkFloorParticipant",
                             "AckRequired", BooleanValue (true),
                             "GenMedia", BooleanValue (true));
  callHelper.SetServerCall ("ns3::psc::McpttServerCall",
                            "AmbientListening", BooleanValue (false),
                            "TemporaryGroup", BooleanValue (false));

  Ptr<McpttTestCaseConfigOnNetwork> config = Create<McpttTestCaseConfigOnNetwork> ();
  config->SetAppCount (3);
  config->SetClientHelper (clientHelper);
  config->SetCallHelper (callHelper);

  SetConfig (config);

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ue1App = GetApp (0);
  Ptr<McpttPttApp> ue2App = GetApp (1);
  Ptr<McpttPttApp> ue3App = GetApp (2);

  ue1App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorDeny::Ue1RxCb, this));
  ue1App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorDeny::Ue1TxCb, this));
  ue2App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorDeny::Ue2RxCb, this));
  ue2App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorDeny::Ue2TxCb, this));
  ue3App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorDeny::Ue3RxCb, this));
  ue3App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorDeny::Ue3TxCb, this));

  Ptr<McpttPusher> ue1Pusher = ue1App->GetPusher ();
  ue1Pusher->SchedulePush (Seconds (3.0));

  Ptr<McpttPusher> ue2Pusher = ue2App->GetPusher ();
  ue2Pusher->SchedulePush (Seconds (4.0));
}

void
McpttTestCaseOnNetworkFloorDeny::Execute (void)
{
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor request.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive a floor deny.");
}

void
McpttTestCaseOnNetworkFloorDeny::Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

void
McpttTestCaseOnNetworkFloorDeny::Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

void
McpttTestCaseOnNetworkFloorDeny::Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, false, "UE 2 did receive a floor deny.");
      m_ue2TxFloorRequest = true;
    }
}

void
McpttTestCaseOnNetworkFloorDeny::Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgDeny::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, false, "UE 2 did receive a floor deny.");
      m_ue2RxFloorDeny = true;
      Stop ();
    }
}

void
McpttTestCaseOnNetworkFloorDeny::Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

void
McpttTestCaseOnNetworkFloorDeny::Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

McpttTestCaseOnNetworkFloorQueueAndCancel::McpttTestCaseOnNetworkFloorQueueAndCancel (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config)
{ }

void
McpttTestCaseOnNetworkFloorQueueAndCancel::Configure (void)
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
                            "TxSsrc", UintegerValue (100),
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
  config->SetAppCount (3);
  config->SetClientHelper (clientHelper);
  config->SetCallHelper (callHelper);

  SetConfig (config);

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ue1App = GetApp (0);
  Ptr<McpttPttApp> ue2App = GetApp (1);
  Ptr<McpttPttApp> ue3App = GetApp (2);

  ue1App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndCancel::Ue1RxCb, this));
  ue1App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndCancel::Ue1TxCb, this));
  ue2App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndCancel::Ue2RxCb, this));
  ue2App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndCancel::Ue2TxCb, this));
  ue3App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndCancel::Ue3RxCb, this));
  ue3App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndCancel::Ue3TxCb, this));

  Ptr<McpttPusher> ue1Pusher = ue1App->GetPusher ();
  ue1Pusher->SchedulePush (Seconds (3.0));

  Ptr<McpttPusher> ue2Pusher = ue2App->GetPusher ();
  ue2Pusher->SchedulePush (Seconds (4.0));
  ue2Pusher->ScheduleRelease (Seconds (6.0));

  std::map<uint16_t, Ptr<McpttCall> > calls = ue2App->GetCalls ();
  Ptr<McpttCall> ue2Call = calls.begin ()->second;
  Ptr<McpttFloorParticipant> ue2FloorParticipant = ue2Call->GetFloorMachine ();
  Ptr<McpttOnNetworkFloorParticipant> ue2OnNetworkFloorParticipant = DynamicCast<McpttOnNetworkFloorParticipant, McpttFloorParticipant> (ue2FloorParticipant);
  Simulator::Schedule (Seconds (5.0), &McpttOnNetworkFloorParticipant::SendFloorQueuePositionRequest, ue2OnNetworkFloorParticipant);
}

void
McpttTestCaseOnNetworkFloorQueueAndCancel::Execute (void)
{
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, true, "UE 2 did not receive queue posotion info");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorQueuePositionRequest, true, "UE 2 did not send queue posotion request.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfoTwo, true, "UE 2 did not receive queue posotion info");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRelease, true, "UE 2 did not send a floor Release.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorAck, true, "UE 2 did not receive a floor ACK.");
}

void
McpttTestCaseOnNetworkFloorQueueAndCancel::Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

void
McpttTestCaseOnNetworkFloorQueueAndCancel::Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}



void
McpttTestCaseOnNetworkFloorQueueAndCancel::Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, false, "UE 2 did receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorQueuePositionRequest, false, "UE 2 did send queue posotion request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfoTwo, false, "UE 2 did receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRelease, false, "UE 2 did send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorAck, false, "UE 2 did receive a floor ACK.");
      m_ue2TxFloorRequest = true;
    }
  else if (headerType == McpttFloorMsgQueuePositionRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, true, "UE 2 did not receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorQueuePositionRequest, false, "UE 2 did send queue posotion request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfoTwo, false, "UE 2 did receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRelease, false, "UE 2 did send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorAck, false, "UE 2 did receive a floor ACK.");
      m_ue2TxFloorQueuePositionRequest = true;
    }
  else if (headerType == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, true, "UE 2 did not receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorQueuePositionRequest, true, "UE 2 did not send queue posotion request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfoTwo, true, "UE 2 did not receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRelease, false, "UE 2 did send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorAck, false, "UE 2 did receive a floor ACK.");
      m_ue2TxFloorRelease = true;
    }
}

void
McpttTestCaseOnNetworkFloorQueueAndCancel::Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgQueuePositionInfo::GetTypeId ()&& m_ue2RxFloorQueuePositionInfo == false)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, false, "UE 2 did receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorQueuePositionRequest, false, "UE 2 did send queue posotion request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfoTwo, false, "UE 2 did receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRelease, false, "UE 1 did send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorAck, false, "UE 1 did receive a floor ACK.");
      m_ue2RxFloorQueuePositionInfo = true;
    }
  else if (headerType == McpttFloorMsgQueuePositionInfo::GetTypeId ()&& m_ue2RxFloorQueuePositionInfo == true)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, true, "UE 2 did not receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorQueuePositionRequest, true, "UE 2 did not send queue posotion request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfoTwo, false, "UE 2 did receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRelease, false, "UE 2 did send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorAck, false, "UE 2 did receive a floor ACK.");
      m_ue2RxFloorQueuePositionInfoTwo = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, true, "UE 2 did not receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorQueuePositionRequest, true, "UE 2 did not send queue posotion request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfoTwo, true, "UE 2 did not receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRelease, true, "UE 2 did not send a floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorAck, false, "UE 2 did receive a floor ACK.");
      m_ue2RxFloorAck = true;
      Stop ();
    }
}

void
McpttTestCaseOnNetworkFloorQueueAndCancel::Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

void
McpttTestCaseOnNetworkFloorQueueAndCancel::Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

McpttTestCaseOnNetworkFloorQueueAndGranted::McpttTestCaseOnNetworkFloorQueueAndGranted (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config)
{ }

void
McpttTestCaseOnNetworkFloorQueueAndGranted::Configure (void)
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
                            "TxSsrc", UintegerValue (100),
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
  config->SetAppCount (3);
  config->SetClientHelper (clientHelper);
  config->SetCallHelper (callHelper);

  SetConfig (config);

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ue1App = GetApp (0);
  Ptr<McpttPttApp> ue2App = GetApp (1);
  Ptr<McpttPttApp> ue3App = GetApp (2);

  ue1App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndGranted::Ue1RxCb, this));
  ue1App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndGranted::Ue1TxCb, this));
  ue2App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndGranted::Ue2RxCb, this));
  ue2App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndGranted::Ue2TxCb, this));
  ue3App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndGranted::Ue3RxCb, this));
  ue3App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkFloorQueueAndGranted::Ue3TxCb, this));

  Ptr<McpttPusher> ue1Pusher = ue1App->GetPusher ();
  ue1Pusher->SchedulePush (Seconds (3.0));
  ue1Pusher->ScheduleRelease (Seconds (6.0));

  Ptr<McpttPusher> ue2Pusher = ue2App->GetPusher ();
  ue2Pusher->SchedulePush (Seconds (5.0));

}

void
McpttTestCaseOnNetworkFloorQueueAndGranted::Execute (void)
{
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, true, "UE 2 did not receive queue posotion info");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, true, "UE 2 did not send a floor Ack.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a Floor Release.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive Ack.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, true, "UE 2 did not receive Floor Granted");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send a floor Ack.");

}


void
McpttTestCaseOnNetworkFloorQueueAndGranted::Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgAck::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, true, "UE 2 did not receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, true, "UE 2 did not send a floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive Floor Granted");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor Ack.");
      m_ue1RxFloorAck = true;
    }
}


void
McpttTestCaseOnNetworkFloorQueueAndGranted::Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, true, "UE 2 did not receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, true, "UE 2 did not send a floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send a Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive Floor Granted");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor Ack.");
      m_ue1TxFloorRelease = true;
    }
}

void
McpttTestCaseOnNetworkFloorQueueAndGranted::Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, false, "UE 2 did receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, false, "UE 2 did send a floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send a Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive Floor Granted");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor Ack.");
      m_ue2TxFloorRequest = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ()
           && m_ue2TxFloorAck1 == false)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, true, "UE 2 did not receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, false, "UE 2 did send a floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send a Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive Floor Granted");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor Ack.");
      m_ue2TxFloorAck1 = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ()
           && m_ue2TxFloorAck1 == true)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, true, "UE 2 did not receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, true, "UE 2 did not send a floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, true, "UE 2 did not receive Floor Granted");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor Ack.");
      m_ue2TxFloorAck2 = true;
      Stop ();
    }
}

void
McpttTestCaseOnNetworkFloorQueueAndGranted::Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgQueuePositionInfo::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, false, "UE 2 did receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, false, "UE 2 did send a floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send a Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive Floor Granted");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor Ack.");
      m_ue2RxFloorQueuePositionInfo = true;
    }
  else if (headerType == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorQueuePositionInfo, true, "UE 2 did not receive queue posotion info");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck1, true, "UE 2 did not send a floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send a Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorGranted, false, "UE 2 did receive Floor Granted");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send a floor Ack.");
      m_ue2RxFloorGranted = true;
    }
}

void
McpttTestCaseOnNetworkFloorQueueAndGranted::Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

void
McpttTestCaseOnNetworkFloorQueueAndGranted::Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{}

McpttTestCaseOnNetworkDualFloorControl::McpttTestCaseOnNetworkDualFloorControl (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config)
{}

void
McpttTestCaseOnNetworkDualFloorControl::Configure (void)
{
  McpttHelper clientHelper;
  clientHelper.SetPttApp ("ns3::psc::McpttPttApp");
  clientHelper.SetMediaSrc ("ns3::psc::McpttMediaSrc",
                         "Bytes", UintegerValue (6),
                         "DataRate", DataRateValue (DataRate ("24kb/s")));
  clientHelper.SetPusher ("ns3::psc::McpttPusher",
                         "Automatic", BooleanValue (false));

  McpttCallHelper callHelper;
  callHelper.SetArbitrator ("ns3::psc::McpttOnNetworkFloorArbitrator",
                         "AckRequired", BooleanValue (true),
                         "AudioCutIn", BooleanValue (false),
                         "DualFloorSupported", BooleanValue (true),
                         "TxSsrc", UintegerValue (100),
                         "QueueingSupported", BooleanValue (false));
  callHelper.SetTowardsParticipant ("ns3::psc::McpttOnNetworkFloorTowardsParticipant",
                         "ReceiveOnly", BooleanValue (false));
  callHelper.SetParticipant ("ns3::psc::McpttOnNetworkFloorParticipant",
                         "AckRequired", BooleanValue (true),
                         "GenMedia", BooleanValue (true));
  callHelper.SetServerCall ("ns3::psc::McpttServerCall",
                         "AmbientListening", BooleanValue (false),
                         "TemporaryGroup", BooleanValue (false));


  Ptr<McpttTestCaseConfigOnNetwork> config = Create<McpttTestCaseConfigOnNetwork> ();
  config->SetAppCount (3);
  config->SetClientHelper (clientHelper);
  config->SetCallHelper (callHelper);

  SetConfig (config);

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ue1App = GetApp (0);
  Ptr<McpttPttApp> ue2App = GetApp (1);
  Ptr<McpttPttApp> ue3App = GetApp (2);

  ue1App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkDualFloorControl::Ue1RxCb, this));
  ue1App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkDualFloorControl::Ue1TxCb, this));
  ue2App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkDualFloorControl::Ue2RxCb, this));
  ue2App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkDualFloorControl::Ue2TxCb, this));
  ue3App->TraceConnectWithoutContext ("RxTrace", MakeCallback (&McpttTestCaseOnNetworkDualFloorControl::Ue3RxCb, this));
  ue3App->TraceConnectWithoutContext ("TxTrace", MakeCallback (&McpttTestCaseOnNetworkDualFloorControl::Ue3TxCb, this));

  Ptr<McpttPusher> ue1Pusher = ue1App->GetPusher ();
  ue1Pusher->SchedulePush (Seconds (3.0));
  ue1Pusher->ScheduleRelease (Seconds (7.0));
  
  Ptr<McpttPusher> ue2Pusher = ue2App->GetPusher ();
  ue2Pusher->SchedulePush (Seconds (4.0));
  
  Ptr<McpttPusher> ue3Pusher = ue3App->GetPusher ();
  ue3Pusher->SchedulePush (Seconds (6.0));
  ue3Pusher->ScheduleRelease (Seconds (8.0));

  std::map<uint16_t, Ptr<McpttCall> > calls = ue3App->GetCalls ();
  Ptr<McpttCall> ue3Call = calls.begin ()->second;
  Ptr<McpttFloorParticipant> ue3FloorParticipant = ue3Call->GetFloorMachine ();
  Ptr<McpttOnNetworkFloorParticipant> ue3OnNetworkFloorParticipant = DynamicCast<McpttOnNetworkFloorParticipant, McpttFloorParticipant> (ue3FloorParticipant);
  Simulator::Schedule (Seconds (5), &McpttOnNetworkFloorParticipant::SetPriority, ue3OnNetworkFloorParticipant, 2);
}

void McpttTestCaseOnNetworkDualFloorControl::Execute (void)
{
  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
  NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
  NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
  NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not receive Floor Ack.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Idle.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Idle.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send Floor Release.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, true, "UE 2 did not receive Floor Idle.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, true, "UE 2 did not send floor Ack.");
  NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, true, "UE 2 did not receive Floor Idle.");
  NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, true, "UE 2 did not send floor Ack.");
  NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, true, "UE 1 did not receive Floor Taken.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, true, "UE 2 did not receive Floor Taken.");
  NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, true, "UE 2 did not send floor Ack.");
  NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, true, "UE 3 did not send Floor Release");
  NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, true, "UE 3 did not send Floor ACK.");
}

void
McpttTestCaseOnNetworkDualFloorControl::Ue1RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgIdle::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue1RxFloorIdle = true;
    }
  else if (headerType == McpttFloorMsgTaken::GetTypeId ()
           && !m_ue1RxFloorTaken1)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue1RxFloorTaken1 = true;  
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue1RxFloorAck = true;
    }
  else if (headerType == McpttFloorMsgTaken::GetTypeId ()
           && !m_ue1RxFloorTaken2)
    {  
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, true, "UE 2 did not receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, true, "UE 3 did not receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, true, "UE 3 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue1RxFloorTaken2 = true;
    }
}

void
McpttTestCaseOnNetworkDualFloorControl::Ue1TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue1TxFloorRelease = true;
    }
}
void
McpttTestCaseOnNetworkDualFloorControl::Ue2TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRequest::GetTypeId ())
    {    
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, false, "UE 2 did receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, false, "UE 3 did send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, false, "UE 3 did receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, false, "UE 3 did send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2TxFloorRequest = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ()
           && m_ue2TxFloorAck2 == false)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, false, "UE 3 did send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, false, "UE 3 did receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, false, "UE 3 did send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2TxFloorAck2 = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ()
           && m_ue2TxFloorAck == false)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2TxFloorAck = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ()
           && m_ue2TxFloorAck3 == false)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2TxFloorAck3 = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ()
           && m_ue2TxFloorAck4 == false)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, true, "UE 2 did not receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2TxFloorAck4 = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ()
           && m_ue2TxFloorAck5 == false)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, true, "UE 2 did not receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, true, "UE 3 did not receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, true, "UE 3 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2TxFloorAck5 = true;
    }
}

void
McpttTestCaseOnNetworkDualFloorControl::Ue2RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  
  if (headerType == McpttFloorMsgTaken::GetTypeId ()
      && m_ue2RxFloorTaken == false )
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, false, "UE 3 already received Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, false, "UE 2 did receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, false, "UE 3 did send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, false, "UE 3 did receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, false, "UE 3 did send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2RxFloorTaken = true;
    }
  else if (headerType == McpttFloorMsgDeny::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, false, "UE 2 did receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, false, "UE 3 did send Floor Request");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, false, "UE 3 did receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, false, "UE 3 did send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease,false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2RxFloorDeny = true;
    }
  else if (headerType == McpttFloorMsgIdle::GetTypeId ()
           && m_ue2RxFloorIdle == false)
    {  
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2RxFloorIdle = true;
    }
  else if (headerType == McpttFloorMsgTaken::GetTypeId ()
           && m_ue2RxFloorTaken2 == false)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2RxFloorTaken2 = true;
    }
  else if (headerType == McpttFloorMsgIdle::GetTypeId ()
           && m_ue2RxFloorIdle == true)
    {  
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2RxFloorIdle2 = true;
    }
  else if (headerType == McpttFloorMsgTaken::GetTypeId ()
           && m_ue2RxFloorTaken3 == false)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, true, "UE 3 did not receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, true, "UE 3 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue2RxFloorTaken3 = true;  
    }
}

void
McpttTestCaseOnNetworkDualFloorControl::Ue3RxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgTaken ::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, false, "UE 3 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, false, "UE 2 did send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, false, "UE 2 did receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, false, "UE 3 did send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, false, "UE 3 did receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, false, "UE 3 did send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue3RxFloorTaken = true;
    }
  else if (headerType == McpttFloorMsgGranted ::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, false, "UE 3 did receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, false, "UE 3 did send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue3RxFloorGranted = true;    
    }
  else if (headerType == McpttFloorMsgIdle::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue3RxFloorIdle = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, true, "UE 3 did not receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, true, "UE 3 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, true, "UE 3 did not send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck,false , "UE 3 did not send Floor ACK.");
      m_ue3RxFloorAck = true;
      Stop ();
    }
}

void
McpttTestCaseOnNetworkDualFloorControl::Ue3TxCb (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");  
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, false, "UE 3 did send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, false, "UE 3 did receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, false, "UE 3 did send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue3TxFloorRequest = true;
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ()
           && m_ue3TxFloorAck == false)
    {  
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, false, "UE 3 did send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, false, "UE 1 did send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, false, "UE 1 did receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, false, "UE 3 did receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue3TxFloorAck = true;  
    }
  else if (headerType == McpttFloorMsgAck::GetTypeId ()
           && m_ue3TxFloorAck2 == false )
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, true, "UE 3 did not receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, false, "UE 3 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, false, "UE 1 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, false, "UE 2 did receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, false, "UE 2 did send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue3TxFloorAck2 = true;
    }  
  else if (headerType == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorTaken, true, "UE 3 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorRequest, true, "UE 2 did not send a floor Request.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorDeny, true, "UE 2 did not receive floor Deny");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck2, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRequest, true, "UE 3 did not send Floor Request"); 
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorGranted, true, "UE 3 did not receive Floor Granted.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck, true, "UE 3 did not send Floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorIdle, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken1, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken2, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck3, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1TxFloorRelease, true, "UE 1 did not send Floor Release.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorAck, true, "UE 1 did not receive a floor ACK.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorIdle2, true, "UE 2 did not receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck4, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorIdle, true, "UE 3 did not receive Floor Idle.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorAck2, true, "UE 3 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue1RxFloorTaken2, true, "UE 1 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2RxFloorTaken3, true, "UE 2 did not receive Floor Taken.");
      NS_TEST_ASSERT_MSG_EQ (m_ue2TxFloorAck5, true, "UE 2 did not send floor Ack.");
      NS_TEST_ASSERT_MSG_EQ (m_ue3TxFloorRelease, false, "UE 3 did send Floor Release");
      NS_TEST_ASSERT_MSG_EQ (m_ue3RxFloorAck, false, "UE 3 did send Floor ACK.");
      m_ue3TxFloorRelease = true;
    }
}

McpttTestSuiteFloorControlOnNetwork::McpttTestSuiteFloorControlOnNetwork (void)
  : TestSuite ("mcptt-floor-control-on-network", TestSuite::SYSTEM)
{
  AddTestCase (new McpttTestCaseOnNetworkFloorRelease (), TestCase::QUICK);
  AddTestCase (new McpttTestCaseOnNetworkFloorGranted (), TestCase::QUICK);
  AddTestCase (new McpttTestCaseOnNetworkFloorRevoke (), TestCase::QUICK);
  AddTestCase (new McpttTestCaseOnNetworkFloorDeny (), TestCase::QUICK);
  AddTestCase (new McpttTestCaseOnNetworkFloorQueueAndCancel (), TestCase::QUICK);
  AddTestCase (new McpttTestCaseOnNetworkFloorQueueAndGranted (), TestCase::QUICK);
  AddTestCase (new McpttTestCaseOnNetworkDualFloorControl (), TestCase::QUICK);
}

} // namespace tests
} // namespace psc
} // namespace ns3
