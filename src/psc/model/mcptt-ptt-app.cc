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
#include <ns3/sip-header.h>

#include "mcptt-call-msg.h"
#include "mcptt-call-machine-grp-basic.h"
#include "mcptt-on-network-call-machine-client.h"
#include "mcptt-call-machine-grp-broadcast.h"
#include "mcptt-chan.h"
#include "mcptt-floor-participant.h"
#include "mcptt-off-network-floor-participant.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-ptt-app.h"
#include "mcptt-pusher.h"
#include "mcptt-media-src.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("McpttPttApp");

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
  static TypeId tid = TypeId ("ns3::McpttPttApp")
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
                     "ns3::McpttPttApp::TxRxTracedCallback")
    .AddTraceSource ("TxTrace", "The trace for capturing sent messages",
                     MakeTraceSourceAccessor (&McpttPttApp::m_txTrace),
                     "ns3::McpttPttApp::TxRxTracedCallback")
    .AddTraceSource ("EventTrace", "General event trace",
                     MakeTraceSourceAccessor (&McpttPttApp::m_eventTrace),
                     "ns3::McpttPttApp::EventTracedCallback")
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
    m_selectedCallChangeCb (MakeNullCallback<void, Ptr<McpttCall> , Ptr<McpttCall> > ()),
    m_userId (0)
{  
  NS_LOG_FUNCTION (this);
}

McpttPttApp::~McpttPttApp (void)
{
  NS_LOG_FUNCTION (this);
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
McpttPttApp::CreateCall (ObjectFactory& callFac, ObjectFactory& floorFac)
{
  // User did not supply a call ID, so allocate one locally
  return CreateCall (callFac, floorFac, m_callIdAllocator++);
}

Ptr<McpttCall>
McpttPttApp::CreateCall (ObjectFactory& callFac, ObjectFactory& floorFac, uint16_t callId)
{
  NS_LOG_FUNCTION (this << callId);

  Ptr<McpttCall> call = CreateObject<McpttCall> ();
  Ptr<McpttChan> floorChan = CreateObject<McpttChan> ();
  Ptr<McpttChan> mediaChan = CreateObject<McpttChan> ();
  Ptr<McpttCallMachine> callMachine = callFac.Create<McpttCallMachine> ();
  Ptr<McpttFloorParticipant> floorMachine = floorFac.Create<McpttFloorParticipant> ();

  call->SetCallMachine (callMachine);
  call->SetFloorChan (floorChan);
  call->SetFloorMachine (floorMachine);
  call->SetMediaChan (mediaChan);
  call->SetCallId (callId);
  AddCall (call);
  return call;
}

void
McpttPttApp::AddCall (Ptr<McpttCall> call)
{
  NS_LOG_FUNCTION (this << call);
  call->SetOwner (this);
  call->SetRxCb (MakeCallback (&McpttPttApp::RxCb, this));
  call->SetTxCb (MakeCallback (&McpttPttApp::TxCb, this));
  NS_ABORT_MSG_UNLESS (call->GetStartTime () >= Simulator::Now (), "Call start time in the past");
  Simulator::ScheduleWithContext (GetNode ()->GetId (), call->GetStartTime () - Simulator::Now (), &McpttPttApp::SelectCall, this, call->GetCallId (), call->GetPushOnSelect ());
  NS_ABORT_MSG_UNLESS (call->GetStopTime () >= Simulator::Now (), "Call stop time in the past");
  Simulator::ScheduleWithContext (GetNode ()->GetId (), call->GetStopTime () - Simulator::Now (), &McpttPttApp::ReleaseCall, this);
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
  Vector current = mobility->GetPosition();

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
          NS_LOG_DEBUG ("Stopping pusher on call ID " << oldCallId);
          m_pusher->Stop ();
        }
      if (m_isRunning)
        {
          oldCallMachine->ReleaseCall ();
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
  m_eventTrace (m_userId, callId, reason);
}

void
McpttPttApp::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetMediaSrc (0);
  SetLocalAddress (Address ());
  SetPusher (0);
  m_selectedCall = 0;

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
McpttPttApp::RxCb (Ptr<const McpttCall> call, const Header& msg)
{
  NS_LOG_FUNCTION (this << call << &msg);

  m_rxTrace (this, call->GetCallId (), msg);
}

void
McpttPttApp::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  for (auto it = m_calls.begin (); it != m_calls.end (); it++)
    {
      if (it->second->GetCallId () == 4)
        {
          m_isRunning = !m_isRunning;
          m_isRunning = !m_isRunning;
        }
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

  pusher->Stop();

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
McpttPttApp::TxCb (Ptr<const McpttCall> call, const Header& msg)
{
  NS_LOG_FUNCTION (this << call << &msg);

  m_txTrace (this, call->GetCallId (), msg);
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
McpttPttApp::SetCalls (const std::map<uint16_t, Ptr<McpttCall> >  calls)
{
  NS_LOG_FUNCTION (this);

  m_calls = calls;
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
      pusher->SetPushable (this);
    }

  m_pusher = pusher;
}

void
McpttPttApp::SetSelectedCallChangeCb (const Callback<void, Ptr<McpttCall> , Ptr<McpttCall> >  selectedCallChangeCb)
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

} // namespace ns3

