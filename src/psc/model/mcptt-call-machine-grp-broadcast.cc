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

#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/log.h>
#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/random-variable-stream.h>
#include <ns3/type-id.h>

#include "mcptt-call.h"
#include "mcptt-call-machine.h"
#include "mcptt-call-machine-grp-broadcast.h"
#include "mcptt-call-machine-grp-broadcast-state.h"
#include "mcptt-call-msg.h"
#include "mcptt-call-msg-field.h"
#include "mcptt-ptt-app.h"
#include "mcptt-timer.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallMachineGrpBroadcast");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttCallMachineGrpBroadcast);

TypeId
McpttCallMachineGrpBroadcast::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttCallMachineGrpBroadcast")
    .SetParent<McpttCallMachineGrp> ()
    .AddConstructor<McpttCallMachineGrpBroadcast> ()
    .AddAttribute ("TFB1", "The initial delay to use for timer TFB1 (Time value)",
                   TimeValue (Seconds (300)),
                   MakeTimeAccessor (&McpttCallMachineGrpBroadcast::SetDelayTfb1),
                   MakeTimeChecker ())
    .AddAttribute ("TFB2", "The initial delay to use for timer TFB2 (Time value)",
                   TimeValue (Seconds (3)),
                   MakeTimeAccessor (&McpttCallMachineGrpBroadcast::SetDelayTfb2),
                   MakeTimeChecker ())
    .AddAttribute ("TFB3", "The initial delay to use for timer TFB3 (Time value)",
                   TimeValue (Seconds (30)),
                   MakeTimeAccessor (&McpttCallMachineGrpBroadcast::SetDelayTfb3),
                   MakeTimeChecker ())
    .AddAttribute ("UserAckRequired", "The flag that indicates if user acknowledgements are required",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttCallMachineGrpBroadcast::m_userAckReq),
                   MakeBooleanChecker ())
    .AddTraceSource ("StateChangeTrace", "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttCallMachineGrpBroadcast::m_stateChangeTrace),
                     "ns3::psc::McpttCallMachine::StateChangeTracedCallback")
  ;

  return tid;
}

McpttCallMachineGrpBroadcast::McpttCallMachineGrpBroadcast (Ptr<McpttCall> call)
  : McpttCallMachineGrp (),
    m_callId (McpttCallMsgFieldCallId ()),
    m_callType (McpttCallMsgFieldCallType::BROADCAST_GROUP),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_newCallCb (MakeNullCallback<void, uint16_t> ()),
    m_origId (McpttCallMsgFieldUserId ()),
    m_call (call),
    m_priority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BROADCAST_GROUP)),
    m_randomCallIdGenerator (CreateObject<UniformRandomVariable> ()),
    m_sdp (McpttCallMsgFieldSdp ()),
    m_started (false),
    m_state (McpttCallMachineGrpBroadcastStateB1::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ()),
    m_tfb1 (CreateObject<McpttTimer> (McpttEntityId (1, "TFB1"))),
    m_tfb2 (CreateObject<McpttTimer> (McpttEntityId (2, "TFB2"))),
    m_tfb3 (CreateObject<McpttTimer> (McpttEntityId (3, "TFB3")))
{
  NS_LOG_FUNCTION (this);

  m_tfb1->Link (&McpttCallMachineGrpBroadcast::ExpiryOfTfb1, this);
  m_tfb2->Link (&McpttCallMachineGrpBroadcast::ExpiryOfTfb2, this);
  m_tfb3->Link (&McpttCallMachineGrpBroadcast::ExpiryOfTfb3, this);
}

McpttCallMachineGrpBroadcast::~McpttCallMachineGrpBroadcast (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBroadcast::AcceptCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " accepting call.");

  Ptr<McpttCallMachineGrpBroadcastState> state = GetState ();
  state->AcceptCall (*this);
}

void
McpttCallMachineGrpBroadcast::BeginEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Emergency alert not supported during broadcast call.");
}

void
McpttCallMachineGrpBroadcast::CancelEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Emergency alert not supported during broadcast call.");
}

void
McpttCallMachineGrpBroadcast::ChangeState (Ptr<McpttCallMachineGrpBroadcastState>  state)
{
  NS_LOG_FUNCTION (this << state);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }
  std::string selected = "False";
  if (GetCall ()->GetCallId () == GetCall ()->GetOwner ()->GetSelectedCall ()->GetCallId ())
    {
      selected = "True";
    }

  McpttEntityId stateId = state->GetInstanceStateId ();
  Ptr<McpttCallMachineGrpBroadcastState> curr = GetState ();

  McpttEntityId currStateId = curr->GetInstanceStateId ();

  if (currStateId != stateId)
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " moving from state " << *curr << " to state " << *state << ".");

      SetState (state);
      if (!m_stateChangeCb.IsNull ())
        {
          m_stateChangeCb (currStateId, stateId);
        }
      m_stateChangeTrace (m_call->GetOwner ()->GetUserId (), m_call->GetCallId (), selected, GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

void
McpttCallMachineGrpBroadcast::DowngradeCallType (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Downgrade of call type not supported during broadcast call.");
}

uint32_t
McpttCallMachineGrpBroadcast::GetCallerUserId (void) const
{
  NS_LOG_FUNCTION (this);

  McpttCallMsgFieldUserId origId = GetOrigId ();
  uint32_t callerUserId = origId.GetId ();

  return callerUserId;
}

TypeId
McpttCallMachineGrpBroadcast::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachineGrpBroadcast::GetTypeId ();
}

McpttEntityId
McpttCallMachineGrpBroadcast::GetStateId (void) const
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachineGrpBroadcastState> state = GetState ();
  McpttEntityId stateId = state->GetInstanceStateId ();

  return stateId;
}

void
McpttCallMachineGrpBroadcast::InitiateCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }
  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " initiating call.");

  Ptr<McpttCallMachineGrpBroadcastState> state = GetState ();
  state->InitiateCall (*this);
}

bool
McpttCallMachineGrpBroadcast::IsCallOngoing (void) const
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return false;
    }

  Ptr<McpttCallMachineGrpBroadcastState> state = GetState ();
  bool isOngoing = state->IsCallOngoing (*this);

  return isOngoing;
}

bool
McpttCallMachineGrpBroadcast::IsStarted (void) const
{
  NS_LOG_FUNCTION (this);

  bool isStarted = GetStarted ();

  return isStarted;
}

bool
McpttCallMachineGrpBroadcast::IsUserAckReq (void) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " does" << (m_userAckReq ? " " : " NOT ") << "require user acknowledgement.");

  return m_userAckReq;
}

void
McpttCallMachineGrpBroadcast::Receive (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  msg.Visit (*this);
}

void
McpttCallMachineGrpBroadcast::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");
}

void
McpttCallMachineGrpBroadcast::ReceiveGrpCallBroadcast (const McpttCallMsgGrpBroadcast& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  Ptr<McpttCallMachineGrpBroadcastState> state = GetState ();
  state->ReceiveGrpCallBroadcast (*this, msg);
}

void
McpttCallMachineGrpBroadcast::ReceiveGrpCallBroadcastEnd (const McpttCallMsgGrpBroadcastEnd& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  Ptr<McpttCallMachineGrpBroadcastState> state = GetState ();
  state->ReceiveGrpCallBroadcastEnd (*this, msg);
}

void
McpttCallMachineGrpBroadcast::ReleaseCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " releasing call.");

  Ptr<McpttCallMachineGrpBroadcastState> state = GetState ();
  state->ReleaseCall (*this);
}

void
McpttCallMachineGrpBroadcast::RejectCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " rejecting call.");

  Ptr<McpttCallMachineGrpBroadcastState> state = GetState ();
  state->RejectCall (*this);
}

void
McpttCallMachineGrpBroadcast::ReportEvent (const char* reason) const
{
  GetCall ()->GetOwner ()->ReportEvent (GetCall ()->GetCallId (), reason);
}

void
McpttCallMachineGrpBroadcast::Send (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " sending " << msg << ".");

  GetCall ()->Send (msg);
}

void
McpttCallMachineGrpBroadcast::SetDelayTfb1 (const Time& delayTfb1)
{
  NS_LOG_FUNCTION (this << &delayTfb1);

  GetTfb1 ()->SetDelay (delayTfb1);
}

void
McpttCallMachineGrpBroadcast::SetDelayTfb2 (const Time& delayTfb2)
{
  NS_LOG_FUNCTION (this << &delayTfb2);

  GetTfb2 ()->SetDelay (delayTfb2);
}

void
McpttCallMachineGrpBroadcast::SetDelayTfb3 (const Time& delayTfb3)
{
  NS_LOG_FUNCTION (this << &delayTfb3);

  GetTfb3 ()->SetDelay (delayTfb3);
}

void
McpttCallMachineGrpBroadcast::SetGrpId (uint32_t grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  McpttCallMsgFieldGrpId grpIdField;
  grpIdField.SetGrpId (grpId);

  SetGrpId (grpIdField);
}

void
McpttCallMachineGrpBroadcast::Start (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " started.");

  SetStarted (true);
}

void
McpttCallMachineGrpBroadcast::Stop (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " stopped.");

  Ptr<McpttTimer> tfb1 = GetTfb1 ();
  Ptr<McpttTimer> tfb2 = GetTfb2 ();
  Ptr<McpttTimer> tfb3 = GetTfb3 ();
  Ptr<McpttFloorParticipant> floorMachine = GetCall ()->GetFloorMachine ();

  if (floorMachine->IsStarted ())
    {
      floorMachine->Stop ();

      GetCall ()->CloseFloorChannel ();
      GetCall ()->CloseMediaChannel ();
    }

  if (tfb1->IsRunning ())
    {
      tfb1->Stop ();
    }

  if (tfb2->IsRunning ())
    {
      tfb2->Stop ();
    }

  if (tfb3->IsRunning ())
    {
      tfb3->Stop ();
    }

  SetState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());

  SetStarted (false);
}

void
McpttCallMachineGrpBroadcast::TakeNewCallNotification (uint16_t callId)
{
  NS_LOG_FUNCTION (this << (uint32_t)callId);

  if (m_newCallCb.IsNull ())
    {
      AcceptCall (); //Accept call by default.
    }
  else
    {
      m_newCallCb (callId);
    }
}

void
McpttCallMachineGrpBroadcast::UpgradeCallType (uint8_t callType)
{
  NS_LOG_FUNCTION (this << (uint32_t)callType);

  NS_LOG_LOGIC ("Upgrade of call type not supported during broadcast call.");
}

void
McpttCallMachineGrpBroadcast::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetCallId (McpttCallMsgFieldCallId ());
  SetCallType (McpttCallMsgFieldCallType ());
  SetGrpId (McpttCallMsgFieldGrpId ());
  SetNewCallCb (MakeNullCallback<void, uint16_t> ());
  SetOrigId (McpttCallMsgFieldUserId ());
  SetCall (0);
  SetPriority (0);
  SetSdp (McpttCallMsgFieldSdp ());
  SetStarted (false);
  SetState (0);
  SetStateChangeCb (MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ());
  SetTfb1 (0);
  SetTfb2 (0);
  SetTfb3 (0);

  McpttCallMachineGrp::DoDispose ();
}

void
McpttCallMachineGrpBroadcast::ExpiryOfTfb1 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " TFB1 expired.");

  Ptr<McpttCallMachineGrpBroadcastState> state = GetState ();
  state->ExpiryOfTfb1 (*this);
}

void
McpttCallMachineGrpBroadcast::ExpiryOfTfb2 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " TFB2 expired.");

  Ptr<McpttCallMachineGrpBroadcastState> state = GetState ();
  state->ExpiryOfTfb2 (*this);
}

void
McpttCallMachineGrpBroadcast::ExpiryOfTfb3 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " TFB3 expired.");

  Ptr<McpttCallMachineGrpBroadcastState> state = GetState ();
  state->ExpiryOfTfb3 (*this);
}

bool
McpttCallMachineGrpBroadcast::GetStarted (void) const
{
  NS_LOG_FUNCTION (this);

  return m_started;
}

void
McpttCallMachineGrpBroadcast::SetStarted (const bool& started)
{
  NS_LOG_FUNCTION (this << &started);

  m_started = started;
}

McpttCallMsgFieldCallId
McpttCallMachineGrpBroadcast::GetCallId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_callId;
}

McpttCallMsgFieldCallType
McpttCallMachineGrpBroadcast::GetCallType (void) const
{
  NS_LOG_FUNCTION (this);

  return m_callType;
}

McpttCallMsgFieldGrpId
McpttCallMachineGrpBroadcast::GetGrpId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_grpId;
}

McpttCallMsgFieldUserId
McpttCallMachineGrpBroadcast::GetOrigId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_origId;
}

Ptr<McpttCall>
McpttCallMachineGrpBroadcast::GetCall (void) const
{
  NS_LOG_FUNCTION (this);

  return m_call;
}

uint8_t
McpttCallMachineGrpBroadcast::GetPriority (void) const
{
  NS_LOG_FUNCTION (this);

  return m_priority;
}

uint16_t
McpttCallMachineGrpBroadcast::GenerateRandomCallId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_randomCallIdGenerator->GetInteger (0, 65535);
}

McpttCallMsgFieldSdp
McpttCallMachineGrpBroadcast::GetSdp (void) const
{
  NS_LOG_FUNCTION (this);

  return m_sdp;
}

Ptr<McpttCallMachineGrpBroadcastState>
McpttCallMachineGrpBroadcast::GetState (void) const
{
  NS_LOG_FUNCTION (this);

  return m_state;
}

Ptr<McpttTimer>
McpttCallMachineGrpBroadcast::GetTfb1 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfb1;
}

Ptr<McpttTimer>
McpttCallMachineGrpBroadcast::GetTfb2 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfb2;
}

Ptr<McpttTimer>
McpttCallMachineGrpBroadcast::GetTfb3 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfb3;
}

void
McpttCallMachineGrpBroadcast::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << &callId);

  m_callId = callId;
}

void
McpttCallMachineGrpBroadcast::SetCallType (const McpttCallMsgFieldCallType& callType)
{
  NS_LOG_FUNCTION (this << &callType);

  m_callType = callType;
}

void
McpttCallMachineGrpBroadcast::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << &grpId);

  m_grpId = grpId;
}

void
McpttCallMachineGrpBroadcast::SetNewCallCb (const Callback<void, uint16_t>  newCallCb)
{
  NS_LOG_FUNCTION (this << &newCallCb);

  m_newCallCb = newCallCb;
}

void
McpttCallMachineGrpBroadcast::SetOrigId (const McpttCallMsgFieldUserId& origId)
{
  NS_LOG_FUNCTION (this << &origId);

  m_origId = origId;
}

void
McpttCallMachineGrpBroadcast::SetCall (Ptr<McpttCall> call)
{
  NS_LOG_FUNCTION (this << call);
  m_call = call;
}

void
McpttCallMachineGrpBroadcast::SetPriority (uint8_t priority)
{
  NS_LOG_FUNCTION (this << (uint32_t)priority);

  m_priority = priority;
}

void
McpttCallMachineGrpBroadcast::SetSdp (const McpttCallMsgFieldSdp& sdp)
{
  NS_LOG_FUNCTION (this << &sdp);

  m_sdp = sdp;
}

void
McpttCallMachineGrpBroadcast::SetState (Ptr<McpttCallMachineGrpBroadcastState>  state)
{
  NS_LOG_FUNCTION (this << state);

  m_state = state;
}

void
McpttCallMachineGrpBroadcast::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this << &stateChangeCb);

  m_stateChangeCb = stateChangeCb;
}

void
McpttCallMachineGrpBroadcast::SetTfb1 (Ptr<McpttTimer>  tfb1)
{
  NS_LOG_FUNCTION (this << tfb1);

  m_tfb1 = tfb1;
}

void
McpttCallMachineGrpBroadcast::SetTfb2 (Ptr<McpttTimer>  tfb2)
{
  NS_LOG_FUNCTION (this << tfb2);

  m_tfb2 = tfb2;
}

void
McpttCallMachineGrpBroadcast::SetTfb3 (Ptr<McpttTimer>  tfb3)
{
  NS_LOG_FUNCTION (this << tfb3);

  m_tfb3 = tfb3;
}

} // namespace psc
} // namespace ns3

