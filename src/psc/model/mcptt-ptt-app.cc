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

#include <ns3/abort.h>
#include <ns3/application.h>
#include <ns3/boolean.h>
#include <ns3/callback.h>
#include <ns3/log.h>
#include <ns3/mobility-model.h>
#include <ns3/node.h>
#include <ns3/nstime.h>
#include <ns3/object-map.h>
#include <ns3/pointer.h>
#include <ns3/random-variable-stream.h>
#include <ns3/simulator.h>
#include <ns3/string.h>
#include <ns3/traced-callback.h>
#include <ns3/udp-socket-factory.h>
#include <ns3/uinteger.h>
#include <ns3/vector.h>
#include <ns3/sip-agent.h>

#include "mcptt-call-msg.h"
#include "mcptt-call-machine-grp-basic.h"
#include "mcptt-on-network-call-machine-client.h"
#include "mcptt-call-machine-grp-broadcast.h"
#include "mcptt-channel.h"
#include "mcptt-floor-participant.h"
#include "mcptt-off-network-floor-participant.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-ptt-app.h"
#include "mcptt-pusher.h"
#include "mcptt-media-src.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttPttApp");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttPttApp);

uint16_t McpttPttApp::s_portNumber = 9000; // Media ports typically 9000-10999

uint16_t
McpttPttApp::GetCurrentPortNumber (void)
{
  return s_portNumber;
}

uint16_t
McpttPttApp::AllocateNextPortNumber (void)
{
  return s_portNumber++;
}

TypeId
McpttPttApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttPttApp")
    .SetParent<Application> ()
    .AddConstructor<McpttPttApp>()
    .AddAttribute ("Calls", "The map of all calls created during the simulation.",
                   ObjectMapValue (),
                   MakeObjectMapAccessor (&McpttPttApp::m_calls),
                   MakeObjectMapChecker<McpttCall> ())
    .AddAttribute ("LocalAddress", "The local address of the node.",
                   AddressValue (Ipv4Address::GetAny ()),
                   MakeAddressAccessor (&McpttPttApp::GetLocalAddress,
                                        &McpttPttApp::SetLocalAddress),
                   MakeAddressChecker ())
    .AddAttribute ("PushOnStart", "The flag that indicates if the pusher should be started with the application.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttPttApp::m_pushOnStart),
                   MakeBooleanChecker ())
    .AddTraceSource ("RxTrace", "The trace for capturing received messages",
                     MakeTraceSourceAccessor (&McpttPttApp::m_rxTrace),
                     "ns3::psc::McpttPttApp::TxRxTracedCallback")
    .AddTraceSource ("TxTrace", "The trace for capturing sent messages",
                     MakeTraceSourceAccessor (&McpttPttApp::m_txTrace),
                     "ns3::psc::McpttPttApp::TxRxTracedCallback")
    .AddTraceSource ("EventTrace", "General event trace",
                     MakeTraceSourceAccessor (&McpttPttApp::m_eventTrace),
                     "ns3::psc::McpttPttApp::EventTracedCallback")
  ;
  return tid;
}

McpttPttApp::McpttPttApp (void)
  : Application (),
    m_isRunning (false),
    m_callIdAllocator (0),
    m_floorGrantedCb (MakeNullCallback<void> ()),
    m_mediaSrc (0),
    m_newCallCb (MakeNullCallback<void, uint16_t> ()),
    m_pusher (0),
    m_selectedCall (0),
    m_selectedCallChangeCb (MakeNullCallback<void, Ptr<McpttCall>, Ptr<McpttCall> > ()),
    m_userId (0)
{
  NS_LOG_FUNCTION (this);
  m_sipAgent = CreateObject<sip::SipAgent> ();
}

McpttPttApp::~McpttPttApp (void)
{
  NS_LOG_FUNCTION (this);
}

Ptr<sip::SipAgent>
McpttPttApp::GetSipAgent (void) const
{
  return m_sipAgent;
}

void
McpttPttApp::AcceptCall (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      callMachine = call->GetCallMachine ();

      callMachine->AcceptCall ();
    }
}

void
McpttPttApp::AcceptFloorGrant (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttFloorParticipant> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      floorMachine = call->GetFloorMachine ();

      floorMachine->AcceptGrant ();
    }
}

void
McpttPttApp::BeginEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttFloorParticipant> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      callMachine = call->GetCallMachine ();
      floorMachine = call->GetFloorMachine ();
      floorMachine->SetPriority (7);
      callMachine->BeginEmergAlert ();
    }
}

void
McpttPttApp::CancelEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttFloorParticipant> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      callMachine = call->GetCallMachine ();
      floorMachine = call->GetFloorMachine ();
      floorMachine->SetPriority (1);
      callMachine->CancelEmergAlert ();
    }
}
Ptr<McpttCall>
McpttPttApp::CreateCall (ObjectFactory& callFac, ObjectFactory& floorFac, McpttCall::NetworkCallType callType)
{
  // User did not supply a call ID, so allocate one locally
  return CreateCall (callFac, floorFac, callType, m_callIdAllocator++);
}

Ptr<McpttCall>
McpttPttApp::CreateCall (ObjectFactory& callFac, ObjectFactory& floorFac, McpttCall::NetworkCallType callType, uint16_t callId)
{
  NS_LOG_FUNCTION (this << callId << callType);

  Ptr<McpttCall> call = CreateObject<McpttCall> (callType);
  Ptr<McpttChannel> floorChannel = CreateObject<McpttChannel> ();
  Ptr<McpttChannel> mediaChannel = CreateObject<McpttChannel> ();
  Ptr<McpttCallMachine> callMachine = callFac.Create<McpttCallMachine> ();
  Ptr<McpttFloorParticipant> floorMachine = floorFac.Create<McpttFloorParticipant> ();

  call->SetCallMachine (callMachine);
  call->SetFloorChannel (floorChannel);
  call->SetFloorMachine (floorMachine);
  call->SetMediaChannel (mediaChannel);
  call->SetCallId (callId);
  AddCall (call);
  return call;
}

void
McpttPttApp::AddCall (Ptr<McpttCall> call)
{
  NS_LOG_FUNCTION (this << call);
  call->SetOwner (this);
  NS_ABORT_MSG_UNLESS (call->GetStartTime () >= Simulator::Now (), "Call start time in the past");
  Simulator::ScheduleWithContext (GetNode ()->GetId (), call->GetStartTime () - Simulator::Now (), &McpttPttApp::SelectCall, this, call->GetCallId (), call->GetPushOnSelect ());
  NS_ABORT_MSG_UNLESS (call->GetStopTime () >= Simulator::Now (), "Call stop time in the past");
  Simulator::ScheduleWithContext (GetNode ()->GetId (), call->GetStopTime () - Simulator::Now (), &McpttPttApp::ReleaseCallByCallId, this, call->GetCallId ());
  m_calls.insert (std::pair<uint16_t, Ptr<McpttCall> > (call->GetCallId (), call));
}

void
McpttPttApp::DowngradeCallType (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      callMachine = call->GetCallMachine ();

      callMachine->DowngradeCallType ();
    }
}

Vector
McpttPttApp::GetNodeLoc (void) const
{
  Ptr<MobilityModel> mobility = GetNode ()->GetObject<MobilityModel> ();
  Vector current = mobility->GetPosition ();

  NS_LOG_LOGIC ("McpttPttApp located at " << current << ".");

  return current;
}

bool
McpttPttApp::HasGrpCallFor (uint32_t grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  bool found = false;
  Ptr<McpttCallMachine> callMachine = 0;

  for (auto it = m_calls.begin (); it != m_calls.end () && !found; it++)
    {
      callMachine = it->second->GetCallMachine ();
      found = (callMachine->IsGrpCall (grpId));
    }
  return found;
}

bool
McpttPttApp::HasPrivateCallFor (uint32_t userId)
{
  NS_LOG_FUNCTION (this << userId);

  bool found = false;
  Ptr<McpttCallMachine> callMachine = 0;

  for (auto it = m_calls.begin (); it != m_calls.end () && !found; it++)
    {
      callMachine = it->second->GetCallMachine ();
      found = (callMachine->IsPrivateCall (userId));
    }

  return found;
}

void
McpttPttApp::InitiateCall (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      call->GetFloorMachine ()->SetOriginator (true);
      call->GetCallMachine ()->InitiateCall ();
    }
}

void
McpttPttApp::SessionInitiateRequest (void)
{
  NS_LOG_FUNCTION (this);
  Ptr<McpttCall> call = GetSelectedCall ();
  if (call)
    {
      NS_LOG_DEBUG ("Starting pusher on call ID " << call->GetCallId ());
      call->GetFloorMachine ()->SetOriginator (false);
      if (!m_pusher->IsPushing ())
        {
          m_pusher->Start ();
        }
    }
  else
    {
      NS_LOG_DEBUG ("No call is selected");
    }
  // floor control start is handled from the McpttCall or call control
}

bool
McpttPttApp::IsRunning (void) const
{
  return m_isRunning;
}

bool
McpttPttApp::IsPushed (void) const
{
  return m_pusher->IsPushing ();
}

void
McpttPttApp::NotifyPushed (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_pusher->IsPushing ())
    {
      NS_LOG_LOGIC ("PttApp button pushed.");
      m_pusher->NotifyPushed ();
    }
}

void
McpttPttApp::RejectCall (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      callMachine = call->GetCallMachine ();

      callMachine->RejectCall ();
    }
}

void
McpttPttApp::ReleaseCall (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCall> call = GetSelectedCall ();
  Ptr<McpttCallMachine> callMachine = 0;

  if (call != 0)
    {
      callMachine = call->GetCallMachine ();
      callMachine->ReleaseCall ();
    }
  if (m_pusher && m_isRunning)
    {
      NS_LOG_DEBUG ("Stopping pusher on call ID " << call->GetCallId ());
      m_pusher->Stop ();
    }
}

void
McpttPttApp::ReleaseCallByCallId (uint32_t callId)
{
  NS_LOG_FUNCTION (this << callId);

  Ptr<McpttCall> call = m_calls.find (callId)->second;
  Ptr<McpttCallMachine> callMachine = 0;

  if (call != 0)
    {
      callMachine = call->GetCallMachine ();
      callMachine->ReleaseCall ();
    }
  if (m_pusher && m_isRunning)
    {
      NS_LOG_DEBUG ("Stopping pusher on call ID " << call->GetCallId ());
      m_pusher->Stop ();
    }
}

void
McpttPttApp::SessionReleaseRequest (void)
{
  NS_LOG_FUNCTION (this);
  Ptr<McpttCall> call = GetSelectedCall ();
  if (call)
    {
      NS_LOG_DEBUG ("Stopping pusher on call ID " << call->GetCallId ());
      m_pusher->Stop ();
    }
  else
    {
      NS_LOG_DEBUG ("No call is selected");
    }
  // floor control release is handled from the McpttCall or call control
}

void
McpttPttApp::ReleaseRequest (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttFloorParticipant> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      floorMachine = call->GetFloorMachine ();

      floorMachine->ReleaseRequest ();
    }
}

void
McpttPttApp::NotifyReleased (void)
{
  NS_LOG_FUNCTION (this);
  if (m_pusher->IsPushing ())
    {
      NS_LOG_LOGIC ("PttApp button released.");
      m_pusher->NotifyReleased ();
    }
}

void
McpttPttApp::SelectCall (uint32_t callId, bool pushOnSelect)
{
  NS_LOG_FUNCTION (this << callId << pushOnSelect);

  uint16_t oldCallId = 0;
  uint16_t newCallId = 0;
  Ptr<McpttCall> newCall = 0;
  Ptr<McpttCallMachine> oldCallMachine = 0;
  Ptr<McpttFloorParticipant> oldFloorMachine = 0;
  Ptr<McpttCallMachine> newCallMachine = 0;
  Ptr<McpttFloorParticipant> newFloorMachine = 0;
  Ptr<McpttCall> oldCall = GetSelectedCall ();

  if (oldCall && (callId == oldCall->GetCallId ()))
    {
      NS_LOG_DEBUG ("Trying to select existing selected call; take no action");
      return;
    }

  if (oldCall != 0)
    {
      oldCallId = oldCall->GetCallId ();
      oldCallMachine = oldCall->GetCallMachine ();
      oldFloorMachine = oldCall->GetFloorMachine ();
      oldCallMachine->SetNewCallCb (MakeNullCallback<void, uint16_t> ());
      oldFloorMachine->SetFloorGrantedCb (MakeNullCallback<void> ());
      if (m_pusher && m_isRunning)
        {
          if (IsPushed ())
            {
              oldFloorMachine->PttRelease ();
            }
          m_pusher->Stop ();
          NS_LOG_DEBUG ("Stopping pusher on call ID " << oldCallId);
        }
    }

  newCall = m_calls.find (callId)->second;
  if (newCall != 0)
    {
      newCallId = newCall->GetCallId ();
      newCallMachine = newCall->GetCallMachine ();
      newFloorMachine = newCall->GetFloorMachine ();
      newCallMachine->SetNewCallCb (MakeCallback (&McpttPttApp::NewCallCb, this));
      newFloorMachine->SetFloorGrantedCb (MakeCallback (&McpttPttApp::FloorGrantedCb, this));
      if (m_pusher && pushOnSelect)
        {
          if (!m_isRunning)
            {
              // If the call is selected before Start(), and pushOnSelect
              // is true, then carry this value forward into the pushOnStart
              // variable for when the app is started
              m_pushOnStart = true;
              NS_LOG_DEBUG ("Setting application pushOnStart for call ID " << newCallId);
            }
          else
            {
              NS_LOG_DEBUG ("Starting pusher on call ID " << newCallId);
              // Call will initiate once the first push notification arrives
              Simulator::ScheduleNow (&McpttPusher::Start, m_pusher);
            }
        }
      // if pushOnSelect is false, the pusher must be externally started
    }

  m_selectedCall = newCall;
  ReportEvent (newCall->GetCallId (), CALL_SELECTED);

  if (!m_selectedCallChangeCb.IsNull ())
    {
      m_selectedCallChangeCb (oldCall, newCall);
    }

  if (oldCall)
    {
      NS_LOG_LOGIC ("PttApp switching from call " << oldCallId << " to " << newCallId);
    }
  else
    {
      NS_LOG_LOGIC ("PttApp switching from no previous call to " << newCallId);
    }
}

void
McpttPttApp::TakePushNotification (void)
{
  NS_LOG_FUNCTION (this);
  Ptr<McpttCall> call = GetSelectedCall ();
  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttFloorParticipant> floorMachine = 0;
  if (call != 0)
    {
      callMachine = call->GetCallMachine ();
      floorMachine = call->GetFloorMachine ();
      if (callMachine->IsCallOngoing ())
        {
          // Request for floor if needed
          floorMachine->PttPush ();
        }
      else
        {
          InitiateCall ();
        }
    }
  else
    {
      NS_LOG_DEBUG ("No selected call for push notification");
    }
}

void
McpttPttApp::TakeReleaseNotification (void)
{
  NS_LOG_FUNCTION (this);

  uint32_t myUserId = GetUserId ();
  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttFloorParticipant> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      callMachine = call->GetCallMachine ();
      floorMachine = call->GetFloorMachine ();

      floorMachine->PttRelease ();

      //Broadcast calls should be released when the originator is done talking.
      if (callMachine->GetInstanceTypeId () == McpttCallMachineGrpBroadcast::GetTypeId ()
          && callMachine->GetCallerUserId () == myUserId)
        {
          ReleaseCall ();
        }
    }
}

bool
McpttPttApp::TakeSendReq (McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC ("PttApp taking request to send " << msg << ".");

  bool sent = false;
  Ptr<McpttFloorParticipant> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      floorMachine = call->GetFloorMachine ();

      floorMachine->MediaReady (msg);

      if (floorMachine->HasFloor ())
        {
          sent = true;
          call->Send (msg);
        }
    }

  return sent;
}

void
McpttPttApp::UpgradeCallType (uint8_t callType)
{
  NS_LOG_FUNCTION (this << (uint32_t)callType);

  Ptr<McpttCall> call = GetSelectedCall ();
  Ptr<McpttCallMachine> callMachine = 0;

  if (call != 0)
    {
      callMachine = call->GetCallMachine ();

      callMachine->UpgradeCallType (callType);
    }
}

void
McpttPttApp::ReportEvent (uint16_t callId, const char* reason)
{
  NS_LOG_FUNCTION (this << callId << reason);
  std::string selected = "True";
  if (callId != GetSelectedCall ()->GetCallId ())
    {
      selected = "False";
    }
  m_eventTrace (m_userId, callId, selected, reason);
}

void
McpttPttApp::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetMediaSrc (0);
  SetLocalAddress (Address ());
  if (m_pusher)
    {
      m_pusher->Dispose ();
    }
  SetPusher (0);
  m_selectedCall = 0;
  m_callChannels.clear ();
  m_callChannelReferenceCount.clear ();
  for (auto it = m_calls.begin (); it != m_calls.end (); it++)
    {
      it->second->Dispose ();
    }
  m_calls.clear ();
  m_onNetworkCalls.clear ();
  m_offNetworkCalls.clear ();
  m_sipAgent->Dispose ();
  m_sipAgent = 0;

  Object::DoDispose ();
}

void
McpttPttApp::FloorGrantedCb (void)
{
  NS_LOG_FUNCTION (this);

  if (m_floorGrantedCb.IsNull ())
    {
      AcceptFloorGrant ();
    }
  else
    {
      m_floorGrantedCb ();
    }
}

void
McpttPttApp::NewCallCb (uint16_t callId)
{
  NS_LOG_FUNCTION (this);

  if (m_newCallCb.IsNull ())
    {
      AcceptCall ();
    }
  else
    {
      m_newCallCb (callId);
    }
}

void
McpttPttApp::ReceiveOffNetworkCallPacket (Ptr<Packet> pkt, Address from)
{
  NS_LOG_FUNCTION (this << pkt << from);
  NS_LOG_LOGIC ("PttApp received " << pkt->GetSize () << " byte(s).");
  McpttCallMsg temp;
  pkt->PeekHeader (temp);

  McpttCallMsgFieldMsgType msgType = temp.GetMsgType ();
  uint8_t code = msgType.GetType ();

  if (code == McpttCallMsgGrpProbe::CODE)
    {
      McpttCallMsgGrpProbe probeMsg;
      pkt->PeekHeader (probeMsg);
      Receive (pkt, probeMsg);
    }
  else if (code == McpttCallMsgGrpAnnoun::CODE)
    {
      McpttCallMsgGrpAnnoun grpAnnounMsg;
      pkt->PeekHeader (grpAnnounMsg);
      Receive (pkt, grpAnnounMsg);
    }
  else if (code == McpttCallMsgGrpAccept::CODE)
    {
      McpttCallMsgGrpAccept grpAcceptMsg;
      pkt->PeekHeader (grpAcceptMsg);
      Receive (pkt, grpAcceptMsg);
    }
  else if (code == McpttCallMsgGrpImmPerilEnd::CODE)
    {
      McpttCallMsgGrpImmPerilEnd grpImmPerilEndMsg;
      pkt->PeekHeader (grpImmPerilEndMsg);
      Receive (pkt, grpImmPerilEndMsg);
    }
  else if (code == McpttCallMsgGrpEmergEnd::CODE)
    {
      McpttCallMsgGrpEmergEnd grpEmergEndMsg;
      pkt->PeekHeader (grpEmergEndMsg);
      Receive (pkt, grpEmergEndMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlert::CODE)
    {
      McpttCallMsgGrpEmergAlert grpEmergAlertMsg;
      pkt->PeekHeader (grpEmergAlertMsg);
      Receive (pkt, grpEmergAlertMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlertAck::CODE)
    {
      McpttCallMsgGrpEmergAlertAck grpEmergAlertAckMsg;
      pkt->PeekHeader (grpEmergAlertAckMsg);
      Receive (pkt, grpEmergAlertAckMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlertCancel::CODE)
    {
      McpttCallMsgGrpEmergAlertCancel grpEmergAlertCancelMsg;
      pkt->PeekHeader (grpEmergAlertCancelMsg);
      Receive (pkt, grpEmergAlertCancelMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlertCancelAck::CODE)
    {
      McpttCallMsgGrpEmergAlertCancelAck emergAlertCancelAckMsg;
      pkt->PeekHeader (emergAlertCancelAckMsg);
      Receive (pkt, emergAlertCancelAckMsg);
    }
  else if (code == McpttCallMsgGrpBroadcast::CODE)
    {
      McpttCallMsgGrpBroadcast grpBroadcastMsg;
      pkt->PeekHeader (grpBroadcastMsg);
      Receive (pkt, grpBroadcastMsg);
    }
  else if (code == McpttCallMsgGrpBroadcastEnd::CODE)
    {
      McpttCallMsgGrpBroadcastEnd grpBroadcastEndMsg;
      pkt->PeekHeader (grpBroadcastEndMsg);
      Receive (pkt, grpBroadcastEndMsg);
    }
  else if (code == McpttCallMsgPrivateSetupReq::CODE)
    {
      McpttCallMsgPrivateSetupReq privateSetupReqMsg;
      pkt->PeekHeader (privateSetupReqMsg);
      Receive (pkt, privateSetupReqMsg);
    }
  else if (code == McpttCallMsgPrivateRinging::CODE)
    {
      McpttCallMsgPrivateRinging privateRingingMsg;
      pkt->PeekHeader (privateRingingMsg);
      Receive (pkt, privateRingingMsg);
    }
  else if (code == McpttCallMsgPrivateAccept::CODE)
    {
      McpttCallMsgPrivateAccept privateAcceptMsg;
      pkt->PeekHeader (privateAcceptMsg);
      Receive (pkt, privateAcceptMsg);
    }
  else if (code == McpttCallMsgPrivateReject::CODE)
    {
      McpttCallMsgPrivateReject privateRejectMsg;
      pkt->PeekHeader (privateRejectMsg);
      Receive (pkt, privateRejectMsg);
    }
  else if (code == McpttCallMsgPrivateRelease::CODE)
    {
      McpttCallMsgPrivateRelease privateReleaseMsg;
      pkt->PeekHeader (privateReleaseMsg);
      Receive (pkt, privateReleaseMsg);
    }
  else if (code == McpttCallMsgPrivateReleaseAck::CODE)
    {
      McpttCallMsgPrivateReleaseAck privateReleaseAckMsg;
      pkt->PeekHeader (privateReleaseAckMsg);
      Receive (pkt, privateReleaseAckMsg);
    }
  else if (code == McpttCallMsgPrivateAcceptAck::CODE)
    {
      McpttCallMsgPrivateAcceptAck privateAcceptAckMsg;
      pkt->PeekHeader (privateAcceptAckMsg);
      Receive (pkt, privateAcceptAckMsg);
    }
  else if (code == McpttCallMsgPrivateEmergCancel::CODE)
    {
      McpttCallMsgPrivateEmergCancel privateEmergCancelMsg;
      pkt->PeekHeader (privateEmergCancelMsg);
      Receive (pkt, privateEmergCancelMsg);
    }
  else if (code == McpttCallMsgPrivateEmergCancelAck::CODE)
    {
      McpttCallMsgPrivateEmergCancelAck privateEmergCancelAckMsg;
      pkt->PeekHeader (privateEmergCancelAckMsg);
      Receive (pkt, privateEmergCancelAckMsg);
    }
  else
    {
      NS_FATAL_ERROR ("Could not resolve message code = " << (uint32_t)code << ".");
    }
}

void
McpttPttApp::Receive (Ptr<Packet> pkt, const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  for (auto it = m_offNetworkCalls.begin (); it != m_offNetworkCalls.end (); it++)
    {
      TraceMessageReceive (it->second->GetCallId (), pkt, msg.GetInstanceTypeId ());
      it->second->Receive (msg);
    }
}

void
McpttPttApp::TraceMessageReceive (uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  NS_LOG_FUNCTION (this << callId << pkt << headerType);
  m_rxTrace (this, callId, pkt, headerType);
}

void
McpttPttApp::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  for (auto it = m_calls.begin (); it != m_calls.end (); it++)
    {
      Simulator::ScheduleNow (&McpttCall::Start, it->second);
    }

  if (m_pushOnStart == true && GetSelectedCall ())
    {
      // Start activity on the selected call (if one is selected)
      Simulator::ScheduleNow (&McpttPusher::Start, m_pusher);
      NS_LOG_DEBUG ("m_pushOnStart " << m_pushOnStart);
    }
  m_isRunning = true;
}

void
McpttPttApp::StopApplication (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("PttApp application stopping.");

  Ptr<McpttPusher> pusher = GetPusher ();
  Ptr<McpttMediaSrc> mediaSrc = GetMediaSrc ();

  pusher->Stop ();

  if (mediaSrc->IsMakingReq ())
    {
      mediaSrc->StopMakingReq ();
    }

  for (auto it = m_calls.begin (); it != m_calls.end (); it++)
    {
      it->second->Stop ();
    }

  m_isRunning = false;
  m_selectedCall = 0;
}

void
McpttPttApp::TraceMessageSend (uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  NS_LOG_FUNCTION (this << callId << pkt << headerType);
  m_txTrace (this, callId, pkt, headerType);
}

void
McpttPttApp::OpenCallChannel (uint16_t port, Ptr<McpttCall> call, McpttCall::NetworkCallType callType)
{
  NS_LOG_FUNCTION (port << call << callType);

  // Check if a call channel exists to the port.  If so,
  // increase its reference count.  If not, create it.
  auto it = m_callChannels.find (port);
  if (it != m_callChannels.end ())
    {
      NS_LOG_DEBUG ("Call channel exists for port " << port << "; increment reference count");
      m_callChannelReferenceCount[it->second]++;
      m_sipAgent->SetCallbacks (call->GetCallId (), MakeCallback (&McpttCall::ReceiveSipMessage, call), MakeCallback (&McpttCall::ReceiveSipEvent, call));
    }
  else
    {
      NS_LOG_DEBUG ("Call channel does not exist for port " << port << "; create it");
      Ptr<McpttChannel> channel = CreateObject<McpttChannel> ();
      int result = channel->Open (GetNode (), port, GetLocalAddress (), Ipv4Address::GetAny ());
      NS_ABORT_MSG_UNLESS (result == 0, "Unable to open call channel on node " << GetNode ()->GetId ());
      m_callChannels.emplace (port, channel);
      if (callType == McpttCall::NetworkCallType::ON_NETWORK)
        {
          // The next two statements do the following.  The McpttChan
          // object will send incoming packets to the SipAgent::Receive
          // method.  The SIP agent, after processing (removing the
          // header), will send the packet back to McpttCall::ReceiveSipMessage
          // and any non-packet events to McpttCall::ReceiveSipEvent
          channel->SetRxPktCb (MakeCallback (&sip::SipAgent::Receive, m_sipAgent));
          m_sipAgent->SetCallbacks (call->GetCallId (), MakeCallback (&McpttCall::ReceiveSipMessage, call), MakeCallback (&McpttCall::ReceiveSipEvent, call));
        }
      else if (callType == McpttCall::NetworkCallType::OFF_NETWORK)
        {
          channel->SetRxPktCb (MakeCallback (&McpttPttApp::ReceiveOffNetworkCallPacket, this));
        }
      else
        {
          NS_FATAL_ERROR ("Call type unsupported");
        }
      m_callChannelReferenceCount.emplace (channel, 1);
    }

  // Add this call to the list of on-network or off-network calls, respectively
  if (callType == McpttCall::NetworkCallType::ON_NETWORK)
    {
      m_onNetworkCalls.insert (std::pair<uint16_t, Ptr<McpttCall> > (call->GetCallId (), call));
    }
  else if (callType == McpttCall::NetworkCallType::OFF_NETWORK)
    {
      m_offNetworkCalls.insert (std::pair<uint16_t, Ptr<McpttCall> > (call->GetCallId (), call));
    }
}

void
McpttPttApp::CloseCallChannel (uint16_t port, Ptr<McpttCall> call, McpttCall::NetworkCallType callType)
{
  NS_LOG_FUNCTION (port << call << callType);
  auto it = m_callChannels.find (port);
  if (it != m_callChannels.end ())
    {
      if (m_callChannelReferenceCount[it->second] == 1)
        {
          NS_LOG_DEBUG ("Closing call channel");
          it->second->Close ();
          it->second->SetRxPktCb (MakeNullCallback<void, Ptr<Packet>, Address> ());
          m_callChannelReferenceCount.erase (it->second);
          m_callChannels.erase (it);
        }
      else
        {
          NS_LOG_DEBUG ("Decrementing reference count on the call channel");
          m_callChannelReferenceCount[it->second]--;
        }
    }
  // Remove call from the list of on-network or off-network calls, respectively
  if (callType == McpttCall::NetworkCallType::ON_NETWORK)
    {
      m_onNetworkCalls.erase (call->GetCallId ());
    }
  else if (callType == McpttCall::NetworkCallType::OFF_NETWORK)
    {
      m_offNetworkCalls.erase (call->GetCallId ());
    }
}

Ptr<McpttChannel>
McpttPttApp::GetCallChannel (uint16_t port) const
{
  auto it = m_callChannels.find (port);
  if (it != m_callChannels.end ())
    {
      return it->second;
    }
  return nullptr;
}

std::map<uint16_t, Ptr<McpttCall> >
McpttPttApp::GetCalls (void) const
{
  return m_calls;
}

Address
McpttPttApp::GetLocalAddress (void) const
{
  return m_localAddress;
}

Ptr<McpttMediaSrc>
McpttPttApp::GetMediaSrc (void) const
{
  return m_mediaSrc;
}

Ptr<McpttPusher>
McpttPttApp::GetPusher (void) const
{
  return m_pusher;
}

Ptr<McpttCall>
McpttPttApp::GetSelectedCall (void) const
{
  return m_selectedCall;
}

uint32_t
McpttPttApp::GetUserId (void) const
{
  return m_userId;
}

void
McpttPttApp::SetFloorGrantedCb (const Callback<void>  floorGrantedCb)
{
  NS_LOG_FUNCTION (this << &floorGrantedCb);

  m_floorGrantedCb = floorGrantedCb;
}

void
McpttPttApp::SetLocalAddress (const Address& localAddress)
{
  NS_LOG_FUNCTION (this << &localAddress);

  m_localAddress = localAddress;
}

void
McpttPttApp::SetMediaSrc (Ptr<McpttMediaSrc>  mediaSrc)
{
  NS_LOG_FUNCTION (this << mediaSrc);

  if (mediaSrc != 0)
    {
      mediaSrc->SetSink (this);
    }

  m_mediaSrc = mediaSrc;
}

void
McpttPttApp::SetNewCallCb (const Callback<void, uint16_t>  newCallCb)
{
  NS_LOG_FUNCTION (this << &newCallCb);

  m_newCallCb = newCallCb;
}

void
McpttPttApp::SetPusher (Ptr<McpttPusher>  pusher)
{
  NS_LOG_FUNCTION (this << pusher);

  if (pusher != 0)
    {
      pusher->SetPttApp (GetObject<McpttPttApp> ());
    }

  m_pusher = pusher;
}

void
McpttPttApp::SetSelectedCallChangeCb (const Callback<void, Ptr<McpttCall>, Ptr<McpttCall> >  selectedCallChangeCb)
{
  NS_LOG_FUNCTION (this << &selectedCallChangeCb);

  m_selectedCallChangeCb = selectedCallChangeCb;
}

void
McpttPttApp::SetUserId (uint32_t userId)
{
  NS_LOG_FUNCTION (this << userId);

  m_userId = userId;
}

} // namespace psc
} // namespace ns3

