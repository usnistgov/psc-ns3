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

#include <ns3/assert.h>
#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/uinteger.h>
#include <ns3/random-variable-stream.h>

#include "mcptt-call.h"
#include "mcptt-call-machine-private-state.h"
#include "mcptt-call-msg.h"
#include "mcptt-call-msg-field.h"
#include "mcptt-call-type-machine-private.h"
#include "mcptt-counter.h"
#include "mcptt-emerg-alert-machine-basic.h"
#include "mcptt-ptt-app.h"
#include "mcptt-timer.h"

#include "mcptt-call-machine-private.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallMachinePrivate");

namespace psc {

/** McpttCallMachinePrivate - begin **/

NS_OBJECT_ENSURE_REGISTERED (McpttCallMachinePrivate);

TypeId
McpttCallMachinePrivate::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttCallMachinePrivate")
    .SetParent<McpttCallMachineGrp> ()
    .AddConstructor<McpttCallMachinePrivate> ()
    .AddAttribute ("AutoCommencement", "The flag that indicates if auto commencement mode should be indicated.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttCallMachinePrivate::m_autoCommencement),
                   MakeBooleanChecker ())
    .AddAttribute ("CallTypeMachine", "The call type machine.",
                   PointerValue (),
                   MakePointerAccessor (&McpttCallMachinePrivate::m_typeMachine),
                   MakePointerChecker<McpttCallTypeMachinePrivate> ())
    .AddAttribute ("CFP1", "The initial limit to use for counter CFP1.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttCallMachinePrivate::SetLimitCfp1),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("CFP3", "The initial limit to use for counter CFP3.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttCallMachinePrivate::SetLimitCfp3),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("CFP4", "The initial limit to use for counter CFP4.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttCallMachinePrivate::SetLimitCfp4),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("CFP6", "The initial limit to use for counter CFP6.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttCallMachinePrivate::SetLimitCfp6),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("EmergAlertMachine", "The emergency alert machine.",
                   PointerValue (),
                   MakePointerAccessor (&McpttCallMachinePrivate::m_emergMachine),
                   MakePointerChecker<McpttEmergAlertMachine> ())
    .AddAttribute ("Failure", "The flag that indicates if the call should fail.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttCallMachinePrivate::m_failure),
                   MakeBooleanChecker ())
    .AddAttribute ("TargetId", "The MCPTT user ID of the user to call.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&McpttCallMachinePrivate::m_targetId),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("TFP1", "The initial delay to use for timer TFP1 (Time value).",
                   TimeValue (MilliSeconds (40)),
                   MakeTimeAccessor (&McpttCallMachinePrivate::SetDelayTfp1),
                   MakeTimeChecker ())
    .AddAttribute ("TFP2", "The initial delay to use for timer TFP2 (Time value).",
                   TimeValue (Seconds (30)),
                   MakeTimeAccessor (&McpttCallMachinePrivate::SetDelayTfp2),
                   MakeTimeChecker ())
    .AddAttribute ("TFP3", "The initial delay to use for timer TFP3 (Time value).",
                   TimeValue (MilliSeconds (40)),
                   MakeTimeAccessor (&McpttCallMachinePrivate::SetDelayTfp3),
                   MakeTimeChecker ())
    .AddAttribute ("TFP4", "The initial delay to use for timer TFP4 (Time value).",
                   TimeValue (MilliSeconds (40)),
                   MakeTimeAccessor (&McpttCallMachinePrivate::SetDelayTfp4),
                   MakeTimeChecker ())
    .AddAttribute ("TFP5", "The initial delay to use for timer TFP5 (Time value).",
                   TimeValue (Seconds (255)),
                   MakeTimeAccessor (&McpttCallMachinePrivate::SetDelayTfp5),
                   MakeTimeChecker ())
    .AddAttribute ("TFP6", "The initial delay to use for timer TFP6 (Time value).",
                   TimeValue (MilliSeconds (40)),
                   MakeTimeAccessor (&McpttCallMachinePrivate::SetDelayTfp6),
                   MakeTimeChecker ())
    .AddAttribute ("TFP7", "The initial delay to use for timer TFP7 (Time value).",
                   TimeValue (Seconds (1)),
                   MakeTimeAccessor (&McpttCallMachinePrivate::SetDelayTfp7),
                   MakeTimeChecker ())
    .AddAttribute ("TFP8", "The initial delay to use for timer TFP8 (Time value).",
                   TimeValue (Seconds (180)),
                   MakeTimeAccessor (&McpttCallMachinePrivate::SetDelayTfp8),
                   MakeTimeChecker ())
    .AddTraceSource ("StateChangeTrace", "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttCallMachinePrivate::m_stateChangeTrace),
                     "ns3::psc::McpttCallMachine::StateChangeTracedCallback")
  ;

  return tid;
}

McpttCallMachinePrivate::McpttCallMachinePrivate (Ptr<McpttCall> call)
  : McpttCallMachineGrp (),
    m_callId (McpttCallMsgFieldCallId ()),
    m_calleeId (McpttCallMsgFieldUserId ()),
    m_callerId (McpttCallMsgFieldUserId ()),
    m_cfp1 (CreateObject<McpttCounter> (McpttEntityId (1, "CFP1"))),
    m_cfp3 (CreateObject<McpttCounter> (McpttEntityId (3, "CFP3"))),
    m_cfp4 (CreateObject<McpttCounter> (McpttEntityId (4, "CFP4"))),
    m_cfp6 (CreateObject<McpttCounter> (McpttEntityId (6, "CFP6"))),
    m_commMode (McpttCallMsgFieldCommMode ()),
    m_emergMachine (CreateObject<McpttEmergAlertMachineBasic> (this)),
    m_newCallCb (MakeNullCallback<void, uint16_t> ()),
    m_call (call),
    m_randomCallIdGenerator (CreateObject<UniformRandomVariable> ()),
    m_sdp (McpttCallMsgFieldSdp ()),
    m_sdpEmerg (McpttCallMsgFieldSdp ()),
    m_started (false),
    m_state (McpttCallMachinePrivateStateP0::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ()),
    m_tfp1 (CreateObject<McpttTimer> (McpttEntityId (1, "TFP1"))),
    m_tfp2 (CreateObject<McpttTimer> (McpttEntityId (2, "TFP2"))),
    m_tfp3 (CreateObject<McpttTimer> (McpttEntityId (3, "TFP3"))),
    m_tfp4 (CreateObject<McpttTimer> (McpttEntityId (4, "TFP4"))),
    m_tfp5 (CreateObject<McpttTimer> (McpttEntityId (5, "TFP5"))),
    m_tfp6 (CreateObject<McpttTimer> (McpttEntityId (6, "TFP6"))),
    m_tfp7 (CreateObject<McpttTimer> (McpttEntityId (7, "TFP7"))),
    m_tfp8 (CreateObject<McpttTimer> (McpttEntityId (8, "TFP8"))),
    m_typeMachine (CreateObject<McpttCallTypeMachinePrivate> (this))
{
  NS_LOG_FUNCTION (this);

  m_tfp1->Link (&McpttCallMachinePrivate::ExpiryOfTfp1, this);
  m_tfp2->Link (&McpttCallMachinePrivate::ExpiryOfTfp2, this);
  m_tfp3->Link (&McpttCallMachinePrivate::ExpiryOfTfp3, this);
  m_tfp4->Link (&McpttCallMachinePrivate::ExpiryOfTfp4, this);
  m_tfp5->Link (&McpttCallMachinePrivate::ExpiryOfTfp5, this);
  m_tfp6->Link (&McpttCallMachinePrivate::ExpiryOfTfp6, this);
  m_tfp7->Link (&McpttCallMachinePrivate::ExpiryOfTfp7, this);
  m_tfp8->Link (&McpttCallMachinePrivate::ExpiryOfTfp8, this);
}

McpttCallMachinePrivate::~McpttCallMachinePrivate (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachinePrivate::AcceptCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " accepting call.");

  Ptr<McpttCallMachinePrivateState> state = GetState ();

  state->AcceptCall (*this);
}

void
McpttCallMachinePrivate::BeginEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> emergMachine = GetEmergMachine ();

  emergMachine->SendEmergAlert ();
  typeMachine->UpgradeTo (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE);
}

void
McpttCallMachinePrivate::CancelEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> emergMachine = GetEmergMachine ();

  emergMachine->CancelEmergAlert ();

  //TODO: Not in standard - checking for another emergency state users before downgrading from the emergency call type.
  if (!emergMachine->IsOtherInEmergState ())
    {
      typeMachine->Downgrade ();
    }
}

void
McpttCallMachinePrivate::ChangeState (Ptr<McpttCallMachinePrivateState>  state)
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
  Ptr<McpttCallMachinePrivateState> curr = GetState ();

  McpttEntityId currStateId = curr->GetInstanceStateId ();

  if (currStateId != stateId)
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " moving from state " << *curr << " to state " << *state << ".");

      SetState (state);
      if (!m_stateChangeCb.IsNull ())
        {
          m_stateChangeCb (curr->GetInstanceStateId (), state->GetInstanceStateId ());
        }
      m_stateChangeTrace (m_call->GetOwner ()->GetUserId (), m_call->GetCallId (), selected, GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

void
McpttCallMachinePrivate::DowngradeCallType (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  typeMachine->Downgrade ();
}

uint32_t
McpttCallMachinePrivate::GetCallerUserId (void) const
{
  NS_LOG_FUNCTION (this);

  McpttCallMsgFieldUserId callerIdField = GetCallerId ();
  uint32_t callerId = callerIdField.GetId ();

  return callerId;
}

McpttCallMsgFieldCallType
McpttCallMachinePrivate::GetCallType (void) const
{
  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();
  McpttCallMsgFieldCallType typeField = typeMachine->GetCallType ();

  return typeField;
}

McpttCallMsgFieldGrpId
McpttCallMachinePrivate::GetGrpId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_grpId;
}

TypeId
McpttCallMachinePrivate::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachinePrivate::GetTypeId ();
}

McpttEntityId
McpttCallMachinePrivate::GetStateId (void) const
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  McpttEntityId stateId = state->GetInstanceStateId ();

  return stateId;
}

Ptr<McpttCallTypeMachine>
McpttCallMachinePrivate::GetCallTypeMachine (void) const
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallTypeMachinePrivate> grpTypeMachine = GetTypeMachine ();
  Ptr<McpttCallTypeMachine> typeMachine = DynamicCast<McpttCallTypeMachine, McpttCallTypeMachinePrivate> (grpTypeMachine);

  return typeMachine;
}

void
McpttCallMachinePrivate::InitiateCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " initiating private call.");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  typeMachine->Start ();
  typeMachine->InitiateCall ();
  state->InitiateCall (*this);
}

bool
McpttCallMachinePrivate::IsAutoCommencement (void) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " does " << (m_autoCommencement ? "" : "NOT ") << "desire auto commencement.");

  return m_autoCommencement;
}

bool
McpttCallMachinePrivate::IsCallOngoing (void) const
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  bool isCallOngoing = state->IsCallOngoing (*this);

  return isCallOngoing;
}

bool
McpttCallMachinePrivate::IsFailure (void) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " can " << (m_failure ? "NOT " : "") << "establish call.");

  return m_failure;
}

bool
McpttCallMachinePrivate::IsStarted (void) const
{
  NS_LOG_FUNCTION (this);

  bool isStarted = GetStarted ();

  return isStarted;
}

void
McpttCallMachinePrivate::NotifyNewCall (uint16_t callId)
{
  NS_LOG_FUNCTION (this << (uint32_t)callId);

  if (!m_newCallCb.IsNull ())
    {
      m_newCallCb (callId);
    }
}

void
McpttCallMachinePrivate::Receive (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  msg.Visit (*this);
}

void
McpttCallMachinePrivate::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  msg.Visit (*this);
}

void
McpttCallMachinePrivate::ReceiveAccept (const McpttCallMsgPrivateAccept& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  state->ReceiveAccept (*this, msg);

  //TODO: Not in standard - there is no check to see if call control messages should be ignored.
  if (GetState ()->GetInstanceStateId () != McpttCallMachinePrivateStateP1::GetStateId ())
    {
      typeMachine->ReceiveAccept (msg);
    }
}

void
McpttCallMachinePrivate::ReceiveAcceptAck (const McpttCallMsgPrivateAcceptAck& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  state->ReceiveAcceptAck (*this, msg);

  //TODO: Not in standard - there is no check to see if call control messages should be ignored.
  if (GetState ()->GetInstanceStateId () != McpttCallMachinePrivateStateP1::GetStateId ())
    {
      typeMachine->ReceiveAcceptAck (msg);
    }
}

void
McpttCallMachinePrivate::ReceiveEmergencyCancel (const McpttCallMsgPrivateEmergCancel& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  //TODO: Not in standard - there is no check to see if call control messages should be ignored.
  if (GetState ()->GetInstanceStateId () != McpttCallMachinePrivateStateP1::GetStateId ())
    {
      typeMachine->ReceiveEmergencyCancel (msg);
    }
}

void
McpttCallMachinePrivate::ReceiveEmergencyCancelAck (const McpttCallMsgPrivateEmergCancelAck& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  //TODO: Not in standard - there is no check to see if call control messages should be ignored.
  if (GetState ()->GetInstanceStateId () != McpttCallMachinePrivateStateP1::GetStateId ())
    {
      typeMachine->ReceiveEmergencyCancelAck (msg);
    }
}

void
McpttCallMachinePrivate::ReceiveGrpCallEmergAlert (const McpttCallMsgGrpEmergAlert& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttEmergAlertMachine> emergMachine = GetEmergMachine ();

  emergMachine->ReceiveGrpCallEmergAlert (msg);
}

void
McpttCallMachinePrivate::ReceiveGrpCallEmergAlertCancel (const McpttCallMsgGrpEmergAlertCancel& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttEmergAlertMachine> emergMachine = GetEmergMachine ();

  emergMachine->ReceiveGrpCallEmergAlertCancel (msg);
}

void
McpttCallMachinePrivate::ReceiveMedia (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  state->ReceiveMedia (*this, msg);
}

void
McpttCallMachinePrivate::ReceiveReject (const McpttCallMsgPrivateReject& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  state->ReceiveReject (*this, msg);

  //TODO: Not in standard - there is no check to see if call control messages should be ignored.
  if (GetState ()->GetInstanceStateId () != McpttCallMachinePrivateStateP1::GetStateId ())
    {
      typeMachine->ReceiveReject (msg);
    }
}

void
McpttCallMachinePrivate::ReceiveRelease (const McpttCallMsgPrivateRelease& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  state->ReceiveRelease (*this, msg);

  //TODO: Not in standard - there is no check to see if call control messages should be ignored.
  if (GetState ()->GetInstanceStateId () != McpttCallMachinePrivateStateP1::GetStateId ())
    {
      typeMachine->ReceiveRelease (msg);
    }
}

void
McpttCallMachinePrivate::ReportEvent (const char* reason) const
{
  GetCall ()->GetOwner ()->ReportEvent (GetCall ()->GetCallId (), reason);
}

void
McpttCallMachinePrivate::ReceiveReleaseAck (const McpttCallMsgPrivateReleaseAck& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  state->ReceiveReleaseAck (*this, msg);
}

void
McpttCallMachinePrivate::ReceiveRinging (const McpttCallMsgPrivateRinging& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  state->ReceiveRinging (*this, msg);
}

void
McpttCallMachinePrivate::ReceiveSetupRequest (const McpttCallMsgPrivateSetupReq& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  state->ReceiveSetupRequest (*this, msg);

  //TODO: Not in standard - there is no check to see if call control messages should be ignored.
  if (GetState ()->GetInstanceStateId () != McpttCallMachinePrivateStateP1::GetStateId ())
    {
      typeMachine->ReceiveSetupRequest (msg);
    }
}

void
McpttCallMachinePrivate::RejectCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " rejected call.");

  Ptr<McpttCallMachinePrivateState> state = GetState ();

  state->RejectCall (*this);
}

void
McpttCallMachinePrivate::ReleaseCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " released call.");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  typeMachine->ReleaseCall ();
  state->ReleaseCall (*this);
}

void
McpttCallMachinePrivate::Send (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " trying to send " << msg << ".");

  GetCall ()->Send (msg);
}

void
McpttCallMachinePrivate::SetDelayTfp1 (const Time& delayTfp1)
{
  NS_LOG_FUNCTION (this << &delayTfp1);

  GetTfp1 ()->SetDelay (delayTfp1);
}

void
McpttCallMachinePrivate::SetDelayTfp2 (const Time& delayTfp2)
{
  NS_LOG_FUNCTION (this << &delayTfp2);

  GetTfp2 ()->SetDelay (delayTfp2);
}

void
McpttCallMachinePrivate::SetDelayTfp3 (const Time& delayTfp3)
{
  NS_LOG_FUNCTION (this << &delayTfp3);

  GetTfp3 ()->SetDelay (delayTfp3);
}

void
McpttCallMachinePrivate::SetDelayTfp4 (const Time& delayTfp4)
{
  NS_LOG_FUNCTION (this << &delayTfp4);

  GetTfp4 ()->SetDelay (delayTfp4);
}

void
McpttCallMachinePrivate::SetDelayTfp5 (const Time& delayTfp5)
{
  NS_LOG_FUNCTION (this << &delayTfp5);

  GetTfp5 ()->SetDelay (delayTfp5);
}

void
McpttCallMachinePrivate::SetDelayTfp6 (const Time& delayTfp6)
{
  NS_LOG_FUNCTION (this << &delayTfp6);

  GetTfp6 ()->SetDelay (delayTfp6);
}

void
McpttCallMachinePrivate::SetDelayTfp7 (const Time& delayTfp7)
{
  NS_LOG_FUNCTION (this << &delayTfp7);

  GetTfp7 ()->SetDelay (delayTfp7);
}

void
McpttCallMachinePrivate::SetDelayTfp8 (const Time& delayTfp8)
{
  NS_LOG_FUNCTION (this << &delayTfp8);

  GetTfp8 ()->SetDelay (delayTfp8);
}

void
McpttCallMachinePrivate::SetGrpId (uint32_t grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = McpttCallMsgFieldGrpId (grpId);
}

void
McpttCallMachinePrivate::SetLimitCfp1 (uint32_t limitCfp1)
{
  NS_LOG_FUNCTION (this << limitCfp1);

  GetCfp1 ()->SetLimit (limitCfp1);
}

void
McpttCallMachinePrivate::SetLimitCfp3 (uint32_t limitCfp3)
{
  NS_LOG_FUNCTION (this << limitCfp3);

  GetCfp3 ()->SetLimit (limitCfp3);
}

void
McpttCallMachinePrivate::SetLimitCfp4 (uint32_t limitCfp4)
{
  NS_LOG_FUNCTION (this << limitCfp4);

  GetCfp4 ()->SetLimit (limitCfp4);
}

void
McpttCallMachinePrivate::SetLimitCfp6 (uint32_t limitCfp6)
{
  NS_LOG_FUNCTION (this << limitCfp6);

  GetCfp6 ()->SetLimit (limitCfp6);
}

void
McpttCallMachinePrivate::Start (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " started.");

  Ptr<McpttEmergAlertMachineBasic> emergMachine = GetEmergMachine ();
  emergMachine->Start ();

  SetStarted (true);
}

void
McpttCallMachinePrivate::Stop (void)
{
  Ptr<McpttCall> call = GetCall ();
  Ptr<McpttTimer> tfp1 = GetTfp1 ();
  Ptr<McpttTimer> tfp2 = GetTfp2 ();
  Ptr<McpttTimer> tfp3 = GetTfp3 ();
  Ptr<McpttTimer> tfp4 = GetTfp4 ();
  Ptr<McpttTimer> tfp5 = GetTfp5 ();
  Ptr<McpttTimer> tfp6 = GetTfp6 ();
  Ptr<McpttTimer> tfp7 = GetTfp7 ();
  Ptr<McpttTimer> tfp8 = GetTfp8 ();
  Ptr<McpttEmergAlertMachineBasic> emergMachine = GetEmergMachine ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " stopped.");

  if (floorMachine->IsStarted ())
    {
      floorMachine->Stop ();
    }

  if (call->IsFloorChannelOpen ())
    {
      call->CloseFloorChannel ();
    }

  if (call->IsMediaChannelOpen ())
    {
      call->CloseMediaChannel ();
    }

  if (emergMachine->IsStarted ())
    {
      emergMachine->Stop ();
    }

  if (typeMachine->IsStarted ())
    {
      typeMachine->Stop ();
    }

  if (tfp1->IsRunning ())
    {
      tfp1->Stop ();
    }

  if (tfp2->IsRunning ())
    {
      tfp2->Stop ();
    }

  if (tfp3->IsRunning ())
    {
      tfp3->Stop ();
    }

  if (tfp4->IsRunning ())
    {
      tfp4->Stop ();
    }

  if (tfp5->IsRunning ())
    {
      tfp5->Stop ();
    }

  if (tfp6->IsRunning ())
    {
      tfp6->Stop ();
    }

  if (tfp7->IsRunning ())
    {
      tfp7->Stop ();
    }

  if (tfp8->IsRunning ())
    {
      tfp8->Stop ();
    }

  SetCallId (0);
  SetState (McpttCallMachinePrivateStateP0::GetInstance ());
  SetStarted (false);
}

void
McpttCallMachinePrivate::UpgradeCallType (uint8_t callType)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  typeMachine->UpgradeTo (callType);
}

void
McpttCallMachinePrivate::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetEmergMachine (0);
  SetState (0);
  SetTfp1 (0);
  SetTfp2 (0);
  SetTfp3 (0);
  SetTfp4 (0);
  SetTfp5 (0);
  SetTfp6 (0);
  SetTfp7 (0);
  SetTfp8 (0);
  SetTypeMachine (0);
}

void
McpttCallMachinePrivate::ExpiryOfTfp1 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFP1 timer expired.");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  state->ExpiryOfTfp1 (*this);
  typeMachine->ExpiryOfTfp1 ();
}

void
McpttCallMachinePrivate::ExpiryOfTfp2 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFP2 timer expired.");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  state->ExpiryOfTfp2 (*this);
}

void
McpttCallMachinePrivate::ExpiryOfTfp3 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFP3 timer expired.");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  state->ExpiryOfTfp3 (*this);
}

void
McpttCallMachinePrivate::ExpiryOfTfp4 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFP4 timer expired.");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  state->ExpiryOfTfp4 (*this);
}

void
McpttCallMachinePrivate::ExpiryOfTfp5 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFP5 timer expired.");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  state->ExpiryOfTfp5 (*this);
}

void
McpttCallMachinePrivate::ExpiryOfTfp6 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFP6 timer expired.");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();

  state->ExpiryOfTfp6 (*this);
  typeMachine->ExpiryOfTfp6 ();
}

void
McpttCallMachinePrivate::ExpiryOfTfp7 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFP7 timer expired.");

  Ptr<McpttCallMachinePrivateState> state = GetState ();
  state->ExpiryOfTfp7 (*this);
}

void
McpttCallMachinePrivate::ExpiryOfTfp8 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFP8 timer expired.");

  Ptr<McpttCallTypeMachinePrivate> typeMachine = GetTypeMachine ();
  typeMachine->ExpiryOfTfp8 ();
}

bool
McpttCallMachinePrivate::GetStarted (void) const
{
  NS_LOG_FUNCTION (this);

  return m_started;
}

void
McpttCallMachinePrivate::SetStarted (const bool& started)
{
  NS_LOG_FUNCTION (this << started);

  m_started = started;
}

McpttCallMsgFieldCallId
McpttCallMachinePrivate::GetCallId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_callId;
}

McpttCallMsgFieldUserId
McpttCallMachinePrivate::GetCalleeId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_calleeId;
}

McpttCallMsgFieldUserId
McpttCallMachinePrivate::GetCallerId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_callerId;
}

Ptr<McpttCounter>
McpttCallMachinePrivate::GetCfp1 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_cfp1;
}

Ptr<McpttCounter>
McpttCallMachinePrivate::GetCfp3 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_cfp3;
}

Ptr<McpttCounter>
McpttCallMachinePrivate::GetCfp4 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_cfp4;
}

Ptr<McpttCounter>
McpttCallMachinePrivate::GetCfp6 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_cfp6;
}

McpttCallMsgFieldCommMode
McpttCallMachinePrivate::GetCommMode (void) const
{
  NS_LOG_FUNCTION (this);

  return m_commMode;
}

Ptr<McpttEmergAlertMachineBasic>
McpttCallMachinePrivate::GetEmergMachine (void) const
{
  NS_LOG_FUNCTION (this);

  return m_emergMachine;
}

Ptr<McpttCall>
McpttCallMachinePrivate::GetCall (void) const
{
  return m_call;
}

uint16_t
McpttCallMachinePrivate::GenerateRandomCallId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_randomCallIdGenerator->GetInteger (0, 65535);
}

McpttCallMsgFieldSdp
McpttCallMachinePrivate::GetSdp (void) const
{
  NS_LOG_FUNCTION (this);

  return m_sdp;
}

McpttCallMsgFieldSdp
McpttCallMachinePrivate::GetSdpEmerg (void) const
{
  NS_LOG_FUNCTION (this);

  return m_sdp;
}

Ptr<McpttCallMachinePrivateState>
McpttCallMachinePrivate::GetState (void) const
{
  NS_LOG_FUNCTION (this);

  return m_state;
}

Ptr<McpttTimer>
McpttCallMachinePrivate::GetTfp1 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfp1;
}

Ptr<McpttTimer>
McpttCallMachinePrivate::GetTfp2 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfp2;
}

Ptr<McpttTimer>
McpttCallMachinePrivate::GetTfp3 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfp3;
}

Ptr<McpttTimer>
McpttCallMachinePrivate::GetTfp4 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfp4;
}

Ptr<McpttTimer>
McpttCallMachinePrivate::GetTfp5 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfp5;
}

Ptr<McpttTimer>
McpttCallMachinePrivate::GetTfp6 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfp6;
}

Ptr<McpttTimer>
McpttCallMachinePrivate::GetTfp7 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfp7;
}

Ptr<McpttTimer>
McpttCallMachinePrivate::GetTfp8 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfp8;
}

Ptr<McpttCallTypeMachinePrivate>
McpttCallMachinePrivate::GetTypeMachine (void) const
{
  NS_LOG_FUNCTION (this);

  return m_typeMachine;
}

void
McpttCallMachinePrivate::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMachinePrivate::SetCalleeId (const McpttCallMsgFieldUserId& calleeId)
{
  NS_LOG_FUNCTION (this << &calleeId);

  m_calleeId = calleeId;
}

void
McpttCallMachinePrivate::SetCallerId (const McpttCallMsgFieldUserId& callerId)
{
  NS_LOG_FUNCTION (this << &callerId);

  m_callerId = callerId;
}

void
McpttCallMachinePrivate::SetCfp1 (Ptr<McpttCounter>  cfp1)
{
  NS_LOG_FUNCTION (this << cfp1);

  m_cfp1 = cfp1;
}

void
McpttCallMachinePrivate::SetCfp3 (Ptr<McpttCounter>  cfp3)
{
  NS_LOG_FUNCTION (this << cfp3);

  m_cfp3 = cfp3;
}

void
McpttCallMachinePrivate::SetCfp4 (Ptr<McpttCounter>  cfp4)
{
  NS_LOG_FUNCTION (this << cfp4);

  m_cfp4 = cfp4;
}

void
McpttCallMachinePrivate::SetCfp6 (Ptr<McpttCounter>  cfp6)
{
  NS_LOG_FUNCTION (this << cfp6);

  m_cfp6 = cfp6;
}

void
McpttCallMachinePrivate::SetCommMode (const McpttCallMsgFieldCommMode& commMode)
{
  NS_LOG_FUNCTION (this << &commMode);

  m_commMode = commMode;
}

void
McpttCallMachinePrivate::SetEmergMachine (Ptr<McpttEmergAlertMachineBasic>  emergMachine)
{
  NS_LOG_FUNCTION (this << &emergMachine);

  if (emergMachine != 0)
    {
      emergMachine->SetOwner (this);
    }

  m_emergMachine = emergMachine;
}

void
McpttCallMachinePrivate::SetNewCallCb (const Callback<void, uint16_t>  newCallCb)
{
  NS_LOG_FUNCTION (this << &newCallCb);

  m_newCallCb = newCallCb;
}

void
McpttCallMachinePrivate::SetCall (Ptr<McpttCall> call)
{
  m_call = call;
}

void
McpttCallMachinePrivate::SetSdp (const McpttCallMsgFieldSdp& sdp)
{
  NS_LOG_FUNCTION (this << sdp);

  m_sdp = sdp;
}

void
McpttCallMachinePrivate::SetSdpEmerg (const McpttCallMsgFieldSdp& sdpEmerg)
{
  NS_LOG_FUNCTION (this << sdpEmerg);

  m_sdpEmerg = sdpEmerg;
}

void
McpttCallMachinePrivate::SetState (Ptr<McpttCallMachinePrivateState>  state)
{
  NS_LOG_FUNCTION (this << &state);

  m_state = state;
}

void
McpttCallMachinePrivate::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this << &stateChangeCb);

  m_stateChangeCb = stateChangeCb;
}

void
McpttCallMachinePrivate::SetTfp1 (Ptr<McpttTimer>  tfp1)
{
  NS_LOG_FUNCTION (this << tfp1);

  m_tfp1 = tfp1;
}

void
McpttCallMachinePrivate::SetTfp2 (Ptr<McpttTimer>  tfp2)
{
  NS_LOG_FUNCTION (this << tfp2);

  m_tfp2 = tfp2;
}

void
McpttCallMachinePrivate::SetTfp3 (Ptr<McpttTimer>  tfp3)
{
  NS_LOG_FUNCTION (this << tfp3);

  m_tfp3 = tfp3;
}

void
McpttCallMachinePrivate::SetTfp4 (Ptr<McpttTimer>  tfp4)
{
  NS_LOG_FUNCTION (this << tfp4);

  m_tfp4 = tfp4;
}

void
McpttCallMachinePrivate::SetTfp5 (Ptr<McpttTimer>  tfp5)
{
  NS_LOG_FUNCTION (this << tfp5);

  m_tfp5 = tfp5;
}

void
McpttCallMachinePrivate::SetTfp6 (Ptr<McpttTimer>  tfp6)
{
  NS_LOG_FUNCTION (this << tfp6);

  m_tfp6 = tfp6;
}

void
McpttCallMachinePrivate::SetTfp7 (Ptr<McpttTimer>  tfp7)
{
  NS_LOG_FUNCTION (this << tfp7);

  m_tfp7 = tfp7;
}

void
McpttCallMachinePrivate::SetTfp8 (Ptr<McpttTimer>  tfp8)
{
  NS_LOG_FUNCTION (this << tfp8);

  m_tfp8 = tfp8;
}

void
McpttCallMachinePrivate::SetTypeMachine (Ptr<McpttCallTypeMachinePrivate>  typeMachine)
{
  NS_LOG_FUNCTION (this << &typeMachine);

  if (typeMachine != 0)
    {
      typeMachine->SetOwner (this);
    }

  m_typeMachine = typeMachine;
}
/** McpttCallMachinePrivate - end **/

} // namespace psc
} // namespace ns3

