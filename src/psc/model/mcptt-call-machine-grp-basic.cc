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

// Override default context logging
#define NS_LOG_APPEND_CONTEXT \
  if (m_call && m_call->GetOwner ()) { std::clog << " [node " << m_call->GetOwner ()->GetNode ()->GetId () << "] "; }

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
#include "mcptt-call-machine-grp-basic-state.h"
#include "mcptt-call-msg.h"
#include "mcptt-call-msg-field.h"
#include "mcptt-call-type-machine-grp-basic.h"
#include "mcptt-counter.h"
#include "mcptt-emerg-alert-machine-basic.h"
#include "mcptt-ptt-app.h"
#include "mcptt-timer.h"

#include "mcptt-call-machine-grp-basic.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallMachineGrpBasic");

namespace psc {
/** McpttCallMachineGrpBasic - begin **/

NS_OBJECT_ENSURE_REGISTERED (McpttCallMachineGrpBasic);

TypeId
McpttCallMachineGrpBasic::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMachineGrpBasic")
    .SetParent<McpttCallMachineGrp> ()
    .AddConstructor<McpttCallMachineGrpBasic> ()
    .AddAttribute ("CallTypeMachine", "The call type machine.",
                   PointerValue (),
                   MakePointerAccessor (&McpttCallMachineGrpBasic::m_typeMachine),
                   MakePointerChecker<McpttCallTypeMachineGrpBasic> ())
    .AddAttribute ("CFG3", "The initial limit to use for counter CFG3.",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&McpttCallMachineGrpBasic::SetLimitCfg3),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("ConfirmMode", "The flag that indicates if confirm mode should be indicated.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttCallMachineGrpBasic::m_confirmMode),
                   MakeBooleanChecker ())
    .AddAttribute ("EmergAlertMachine", "The emergency alert machine.",
                   PointerValue (),
                   MakePointerAccessor (&McpttCallMachineGrpBasic::m_emergMachine),
                   MakePointerChecker<McpttEmergAlertMachine> ())
    .AddAttribute ("RefreshInterval", "The minimum amount of time (in milliseconds) between periodic call announcements.",
                   UintegerValue (10000),
                   MakeUintegerAccessor (&McpttCallMachineGrpBasic::SetRefIntVal),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("TFG1", "The initial delay to use for timer TFG1 (Time value).",
                   TimeValue (MilliSeconds (150)),
                   MakeTimeAccessor (&McpttCallMachineGrpBasic::SetDelayTfg1),
                   MakeTimeChecker ())
    .AddAttribute ("TFG3", "The initial delay to use for timer TFG3 (Time value).",
                   TimeValue (MilliSeconds (40)),
                   MakeTimeAccessor (&McpttCallMachineGrpBasic::SetDelayTfg3),
                   MakeTimeChecker ())
    .AddAttribute ("TFG4", "The initial delay to use for timer TFG4 (Time value).",
                   TimeValue (Seconds (30)),
                   MakeTimeAccessor (&McpttCallMachineGrpBasic::SetDelayTfg4),
                   MakeTimeChecker ())
    .AddAttribute ("TFG5", "The initial delay to use for timer TFG5 (Time value).",
                   TimeValue (Seconds (30)),
                   MakeTimeAccessor (&McpttCallMachineGrpBasic::SetDelayTfg5),
                   MakeTimeChecker ())
    .AddAttribute ("TFG6-X", "The X value to use when calculating the delay for TFG6 (Time value).",
                   TimeValue (Seconds (65535)),
                   MakeTimeAccessor (&McpttCallMachineGrpBasic::m_delayTfg6X),
                   MakeTimeChecker (Seconds (1), Seconds (65535)))
    .AddAttribute ("UserAckRequired", "The flag that indicates if user acknowledgements are required.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttCallMachineGrpBasic::m_userAckReq),
                   MakeBooleanChecker ())
    .AddTraceSource ("StateChangeTrace", "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttCallMachineGrpBasic::m_stateChangeTrace),
                     "ns3::psc::McpttCallMachine::StateChangeTracedCallback")
  ;

  return tid;
}

McpttCallMachineGrpBasic::McpttCallMachineGrpBasic (Ptr<McpttCall> call)
  : McpttCallMachineGrp (),
    m_callId (McpttCallMsgFieldCallId ()),
    m_cfg3 (CreateObject<McpttCounter> (McpttEntityId (3, "CFG3"))),
    m_emergMachine (CreateObject<McpttEmergAlertMachineBasic> (this)),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_mergeCb (MakeNullCallback<void, uint16_t> ()),
    m_newCallCb (MakeNullCallback<void, uint16_t> ()),
    m_origId (McpttCallMsgFieldUserId ()),
    m_call (call),
    m_probeRespVal (false),
    m_refInt (McpttCallMsgFieldRefreshInterval ()),
    m_randomCallIdGenerator (CreateObject<UniformRandomVariable> ()),
    m_rndDelays (CreateObjectWithAttributes<UniformRandomVariable> ("Min", DoubleValue (0.0), "Max", DoubleValue (1.0))),
    m_sdp (McpttCallMsgFieldSdp ()),
    m_started (false),
    m_startTime (McpttCallMsgFieldStartTime ()),
    m_state (McpttCallMachineGrpBasicStateS1::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ()),
    m_tfg1 (CreateObject<McpttTimer> (McpttEntityId (1, "TFG1"))),
    m_tfg2 (CreateObject<McpttTimer> (McpttEntityId (2, "TFG2"))),
    m_tfg3 (CreateObject<McpttTimer> (McpttEntityId (3, "TFG3"))),
    m_tfg4 (CreateObject<McpttTimer> (McpttEntityId (4, "TFG4"))),
    m_tfg5 (CreateObject<McpttTimer> (McpttEntityId (5, "TFG5"))),
    m_tfg6 (CreateObject<McpttTimer> (McpttEntityId (6, "TFG6"))),
    m_typeMachine (CreateObject<McpttCallTypeMachineGrpBasic> (this))
{
  NS_LOG_FUNCTION (this);

  m_tfg1->Link (&McpttCallMachineGrpBasic::ExpiryOfTfg1, this);
  m_tfg2->Link (&McpttCallMachineGrpBasic::ExpiryOfTfg2, this);
  m_tfg3->Link (&McpttCallMachineGrpBasic::ExpiryOfTfg3, this);
  m_tfg4->Link (&McpttCallMachineGrpBasic::ExpiryOfTfg4, this);
  m_tfg5->Link (&McpttCallMachineGrpBasic::ExpiryOfTfg5, this);
  m_tfg6->Link (&McpttCallMachineGrpBasic::ExpiryOfTfg6, this);
}

McpttCallMachineGrpBasic::~McpttCallMachineGrpBasic (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBasic::AcceptCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " accepting call.");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();

  state->AcceptCall (*this);
  typeMachine->CallStarted ();
}

void
McpttCallMachineGrpBasic::BeginEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> emergMachine = GetEmergMachine ();

  emergMachine->SendEmergAlert ();
  typeMachine->UpgradeTo (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
}

Time
McpttCallMachineGrpBasic::CalcDelayForAfterCallProbe (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<RandomVariableStream> rndDelays = GetRndDelays ();
  double x = rndDelays->GetValue ();

  double seconds = (1.0 / 12.0) * x;

  Time delay = Seconds (seconds);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " calculated " << delay.GetSeconds () << "s for TFG2 after call probe.");

  return delay;
}

Time
McpttCallMachineGrpBasic::CalcDelayForPeriodicAnnoun (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<RandomVariableStream> rndDelays = GetRndDelays ();
  McpttCallMsgFieldRefreshInterval refInt = GetRefInt ();

  double x = rndDelays->GetValue ();
  uint16_t interval = refInt.GetInterval ();

  double seconds = (interval / 1000.0) * ((2.0 / 3.0) + ((2.0 / 3.0) * x));

  Time delay = Seconds (seconds);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " calculated " << delay.GetSeconds () << "s for TFG2 periodic anouncement.");

  return delay;
}

Time
McpttCallMachineGrpBasic::CalcDelayForMaxDuration (void) const
{
  NS_LOG_FUNCTION (this);

  McpttCallMsgFieldStartTime startTimeField = GetStartTime ();
  Time startTime = startTimeField.GetTime ();

  double x = m_delayTfg6X.GetSeconds ();
  double y = Simulator::Now ().GetSeconds ();
  double z = startTime.GetSeconds ();

  double seconds = x - (y - z);
  //TODO: Not in standard - making sure that the calculated time is at least zero.
  seconds = (seconds < 0.0 ? 0.0 : seconds);

  Time delay = Seconds (seconds);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " calculated " << delay.GetSeconds () << "s for TFG6 max duration.");

  return delay;
}

void
McpttCallMachineGrpBasic::CancelEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> emergMachine = GetEmergMachine ();

  emergMachine->CancelEmergAlert ();

  //TODO: Not in standard - checking for another emergency state users before downgrading from the emergency call type.
  if (!emergMachine->IsOtherInEmergState ())
    {
      typeMachine->Downgrade ();
    }
}

void
McpttCallMachineGrpBasic::ChangeState (Ptr<McpttCallMachineGrpBasicState>  state)
{
  NS_LOG_FUNCTION (this << state);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC ("Not started yet.");
      return;
    }
  std::string selected = "False";
  if (GetCall ()->GetCallId () == GetCall ()->GetOwner ()->GetSelectedCall ()->GetCallId ())
    {
      selected = "True";
    }

  McpttEntityId stateId = state->GetInstanceStateId ();
  Ptr<McpttCallMachineGrpBasicState> curr = GetState ();

  McpttEntityId currStateId = curr->GetInstanceStateId ();

  if (currStateId != stateId)
    {
      NS_LOG_LOGIC (GetCall ()->GetOwner ()->GetUserId () << " moving from state " << *curr << " to state " << *state << ".");

      SetState (state);
      if (!m_stateChangeCb.IsNull ())
        {
          m_stateChangeCb (curr->GetInstanceStateId (), state->GetInstanceStateId ());
        }
      m_stateChangeTrace (m_call->GetOwner ()->GetUserId (), m_call->GetCallId (), selected, GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

void
McpttCallMachineGrpBasic::DowngradeCallType (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();

  typeMachine->Downgrade ();
}

uint32_t
McpttCallMachineGrpBasic::GetCallerUserId (void) const
{
  NS_LOG_FUNCTION (this);

  McpttCallMsgFieldUserId userIdField = GetOrigId ();
  uint32_t callerId = userIdField.GetId ();

  return callerId;
}

McpttCallMsgFieldCallType
McpttCallMachineGrpBasic::GetCallType (void) const
{
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();
  McpttCallMsgFieldCallType typeField = typeMachine->GetCallType ();

  return typeField;
}

TypeId
McpttCallMachineGrpBasic::GetInstanceTypeId (void) const
{
  return McpttCallMachineGrpBasic::GetTypeId ();
}

McpttEntityId
McpttCallMachineGrpBasic::GetStateId (void) const
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  McpttEntityId stateId = state->GetInstanceStateId ();

  return stateId;
}

Ptr<McpttCallTypeMachine>
McpttCallMachineGrpBasic::GetCallTypeMachine (void) const
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallTypeMachineGrpBasic> grpTypeMachine = GetTypeMachine ();
  Ptr<McpttCallTypeMachine> typeMachine = DynamicCast<McpttCallTypeMachine, McpttCallTypeMachineGrpBasic> (grpTypeMachine);

  return typeMachine;
}

void
McpttCallMachineGrpBasic::InitiateCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " initiating group call.");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();

  state->InitiateCall (*this);
  typeMachine->InitiateCall ();
}

bool
McpttCallMachineGrpBasic::IsCallOngoing (void) const
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  bool isCallOngoing = state->IsCallOngoing (*this);

  return isCallOngoing;
}

bool
McpttCallMachineGrpBasic::IsConfirmMode (void) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " is" << (m_confirmMode ? " " : " NOT ") << "indicating confirm mode.");

  return m_confirmMode;
}

bool
McpttCallMachineGrpBasic::IsStarted (void) const
{
  NS_LOG_FUNCTION (this);

  bool isStarted = GetStarted ();

  return isStarted;
}

bool
McpttCallMachineGrpBasic::IsUserAckReq (void) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " does" << (m_userAckReq ? " " : " NOT ") << "require user acknowledgement.");

  return m_userAckReq;
}

void
McpttCallMachineGrpBasic::NotifyMerge (uint16_t callId)
{
  NS_LOG_FUNCTION (this << (uint32_t)callId);

  if (!m_mergeCb.IsNull ())
    {
      m_mergeCb (callId);
    }
}

void
McpttCallMachineGrpBasic::NotifyNewCall (uint16_t callId)
{
  NS_LOG_FUNCTION (this << (uint32_t)callId);

  if (!m_newCallCb.IsNull ())
    {
      m_newCallCb (callId);
    }
}

void
McpttCallMachineGrpBasic::Receive (const McpttCallMsg& msg)
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
McpttCallMachineGrpBasic::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }
}

void
McpttCallMachineGrpBasic::ReceiveGrpCallAccept (const McpttCallMsgGrpAccept& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  state->ReceiveGrpCallAccept (*this, msg);
}

void
McpttCallMachineGrpBasic::ReceiveGrpCallAnnoun (const McpttCallMsgGrpAnnoun& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();

  state->ReceiveGrpCallAnnoun (*this, msg);
  typeMachine->ReceiveGrpCallAnnoun (msg);
}

void
McpttCallMachineGrpBasic::ReceiveGrpCallEmergAlert (const McpttCallMsgGrpEmergAlert& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttEmergAlertMachineBasic> emergMachine = GetEmergMachine ();
  emergMachine->ReceiveGrpCallEmergAlert (msg);
}

void
McpttCallMachineGrpBasic::ReceiveGrpCallEmergAlertAck (const McpttCallMsgGrpEmergAlertAck& msg)
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
McpttCallMachineGrpBasic::ReceiveGrpCallEmergAlertCancel (const McpttCallMsgGrpEmergAlertCancel& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttEmergAlertMachineBasic> emergMachine = GetEmergMachine ();
  emergMachine->ReceiveGrpCallEmergAlertCancel (msg);
}

void
McpttCallMachineGrpBasic::ReceiveGrpCallEmergAlertCancelAck (const McpttCallMsgGrpEmergAlertCancelAck& msg)
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
McpttCallMachineGrpBasic::ReceiveGrpCallEmergEnd (const McpttCallMsgGrpEmergEnd& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();
  typeMachine->ReceiveGrpCallEmergEnd (msg);
}

void
McpttCallMachineGrpBasic::ReceiveGrpCallImmPerilEnd (const McpttCallMsgGrpImmPerilEnd& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();
  typeMachine->ReceiveGrpCallImmPerilEnd (msg);
}

void
McpttCallMachineGrpBasic::ReceiveGrpCallProbe (const McpttCallMsgGrpProbe& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " received " << msg << ".");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  state->ReceiveGrpCallProbe (*this, msg);
}

void
McpttCallMachineGrpBasic::RejectCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " rejected call.");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();

  state->RejectCall (*this);
  typeMachine->RejectCall ();
}

void
McpttCallMachineGrpBasic::ReleaseCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " released call.");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();

  state->ReleaseCall (*this);
  typeMachine->ReleaseCall ();
}

void
McpttCallMachineGrpBasic::ReportEvent (const char* reason) const
{
  GetCall ()->GetOwner ()->ReportEvent (GetCall ()->GetCallId (), reason);
}

void
McpttCallMachineGrpBasic::Send (const McpttCallMsg& msg)
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
McpttCallMachineGrpBasic::SetDelayTfg1 (const Time& delayTfg1)
{
  NS_LOG_FUNCTION (this << &delayTfg1);

  GetTfg1 ()->SetDelay (delayTfg1);
}
void
McpttCallMachineGrpBasic::SetDelayTfg3 (const Time& delayTfg3)
{
  NS_LOG_FUNCTION (this << &delayTfg3);

  GetTfg3 ()->SetDelay (delayTfg3);
}
void
McpttCallMachineGrpBasic::SetDelayTfg4 (const Time& delayTfg4)
{
  NS_LOG_FUNCTION (this << &delayTfg4);

  GetTfg4 ()->SetDelay (delayTfg4);
}
void
McpttCallMachineGrpBasic::SetDelayTfg5 (const Time& delayTfg5)
{
  NS_LOG_FUNCTION (this << &delayTfg5);

  GetTfg5 ()->SetDelay (delayTfg5);
}

void
McpttCallMachineGrpBasic::SetGrpId (uint32_t grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  McpttCallMsgFieldGrpId grpIdField;
  grpIdField.SetGrpId (grpId);

  SetGrpId (grpIdField);
}

void
McpttCallMachineGrpBasic::SetLimitCfg3 (uint32_t limitCfg3)
{
  NS_LOG_FUNCTION (this << limitCfg3);

  GetCfg3 ()->SetLimit (limitCfg3);
}

void
McpttCallMachineGrpBasic::SetRefIntVal (uint16_t refIntVal)
{
  NS_LOG_FUNCTION (this << (uint32_t)refIntVal);

  SetRefInt (McpttCallMsgFieldRefreshInterval (refIntVal));
}

void
McpttCallMachineGrpBasic::Start (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << " started.");

  Ptr<McpttEmergAlertMachineBasic> emergMachine = GetEmergMachine ();
  emergMachine->Start ();

  SetStarted (true);
}

void
McpttCallMachineGrpBasic::Stop (void)
{
  Ptr<McpttCall> call = GetCall ();
  Ptr<McpttTimer> tfg1 = GetTfg1 ();
  Ptr<McpttTimer> tfg2 = GetTfg2 ();
  Ptr<McpttTimer> tfg3 = GetTfg3 ();
  Ptr<McpttTimer> tfg4 = GetTfg4 ();
  Ptr<McpttTimer> tfg5 = GetTfg5 ();
  Ptr<McpttTimer> tfg6 = GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> emergMachine = GetEmergMachine ();
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

  if (tfg1->IsRunning ())
    {
      tfg1->Stop ();
    }

  if (tfg2->IsRunning ())
    {
      tfg2->Stop ();
    }

  if (tfg3->IsRunning ())
    {
      tfg3->Stop ();
    }

  if (tfg4->IsRunning ())
    {
      tfg4->Stop ();
    }

  if (tfg5->IsRunning ())
    {
      tfg5->Stop ();
    }

  if (tfg6->IsRunning ())
    {
      tfg6->Stop ();
    }

  SetCallId (0);
  //TODO: his should be released but since the same call machine is reused for
  //a new call for the same group, this will be left in place
  //SetGrpId (0);
  SetOrigId (0);
  SetProbeRespVal (false);
  //TODO: his should be released but since the same call machine is reused for
  //a new call for the same group, this will be left in place
  //SetRefInt (0);
  SetStartTime (Seconds (0));
  SetState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  SetStarted (false);
}

void
McpttCallMachineGrpBasic::UpgradeCallType (uint8_t callType)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();

  typeMachine->UpgradeTo (callType);
}

void
McpttCallMachineGrpBasic::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetEmergMachine (0);
  SetRndDelays (0);
  SetState (0);
  SetTfg1 (0);
  SetTfg2 (0);
  SetTfg3 (0);
  SetTfg4 (0);
  SetTfg5 (0);
  SetTfg6 (0);
  SetTypeMachine (0);
}

void
McpttCallMachineGrpBasic::ExpiryOfTfg1 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFG1 timer expired.");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = GetTypeMachine ();

  state->ExpiryOfTfg1 (*this);
  typeMachine->ExpiryOfTfg1 ();
}

void
McpttCallMachineGrpBasic::ExpiryOfTfg2 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFG2 timer expired.");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  state->ExpiryOfTfg2 (*this);
}

void
McpttCallMachineGrpBasic::ExpiryOfTfg3 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFG3 timer expired.");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  state->ExpiryOfTfg3 (*this);
}

void
McpttCallMachineGrpBasic::ExpiryOfTfg4 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFG4 timer expired.");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  state->ExpiryOfTfg4 (*this);
}

void
McpttCallMachineGrpBasic::ExpiryOfTfg5 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFG5 timer expired.");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  state->ExpiryOfTfg5 (*this);
}

void
McpttCallMachineGrpBasic::ExpiryOfTfg6 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetCall ()->GetOwner ()->GetUserId () << "'s TFG6 timer expired.");

  Ptr<McpttCallMachineGrpBasicState> state = GetState ();
  state->ExpiryOfTfg6 (*this);
}

bool
McpttCallMachineGrpBasic::GetStarted (void) const
{
  NS_LOG_FUNCTION (this);

  return m_started;
}

void
McpttCallMachineGrpBasic::SetStarted (const bool& started)
{
  NS_LOG_FUNCTION (this << started);

  m_started = started;
}

McpttCallMsgFieldCallId
McpttCallMachineGrpBasic::GetCallId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_callId;
}

Ptr<McpttCounter>
McpttCallMachineGrpBasic::GetCfg3 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_cfg3;
}

Ptr<McpttEmergAlertMachineBasic>
McpttCallMachineGrpBasic::GetEmergMachine (void) const
{
  NS_LOG_FUNCTION (this);

  return m_emergMachine;
}

McpttCallMsgFieldGrpId
McpttCallMachineGrpBasic::GetGrpId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_grpId;
}

McpttCallMsgFieldUserId
McpttCallMachineGrpBasic::GetOrigId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_origId;
}

Ptr<McpttCall>
McpttCallMachineGrpBasic::GetCall (void) const
{
  return m_call;
}

bool
McpttCallMachineGrpBasic::GetProbeRespVal (void) const
{
  NS_LOG_FUNCTION (this);

  return m_probeRespVal;
}

McpttCallMsgFieldRefreshInterval
McpttCallMachineGrpBasic::GetRefInt (void) const
{
  NS_LOG_FUNCTION (this);

  return m_refInt;
}

uint16_t
McpttCallMachineGrpBasic::GenerateRandomCallId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_randomCallIdGenerator->GetInteger (0, 65535);
}

Ptr<RandomVariableStream>
McpttCallMachineGrpBasic::GetRndDelays (void) const
{
  NS_LOG_FUNCTION (this);

  return m_rndDelays;
}

McpttCallMsgFieldSdp
McpttCallMachineGrpBasic::GetSdp (void) const
{
  NS_LOG_FUNCTION (this);

  return m_sdp;
}

McpttCallMsgFieldStartTime
McpttCallMachineGrpBasic::GetStartTime (void) const
{
  NS_LOG_FUNCTION (this);

  return m_startTime;
}

Ptr<McpttCallMachineGrpBasicState>
McpttCallMachineGrpBasic::GetState (void) const
{
  NS_LOG_FUNCTION (this);

  return m_state;
}

Ptr<McpttTimer>
McpttCallMachineGrpBasic::GetTfg1 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfg1;
}

Ptr<McpttTimer>
McpttCallMachineGrpBasic::GetTfg2 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfg2;
}

Ptr<McpttTimer>
McpttCallMachineGrpBasic::GetTfg3 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfg3;
}

Ptr<McpttTimer>
McpttCallMachineGrpBasic::GetTfg4 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfg4;
}

Ptr<McpttTimer>
McpttCallMachineGrpBasic::GetTfg5 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfg5;
}

Ptr<McpttTimer>
McpttCallMachineGrpBasic::GetTfg6 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfg6;
}

Ptr<McpttCallTypeMachineGrpBasic>
McpttCallMachineGrpBasic::GetTypeMachine (void) const
{
  NS_LOG_FUNCTION (this);

  return m_typeMachine;
}

void
McpttCallMachineGrpBasic::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMachineGrpBasic::SetCfg3 (Ptr<McpttCounter>  cfg3)
{
  NS_LOG_FUNCTION (this << cfg3);

  m_cfg3 = cfg3;
}

void
McpttCallMachineGrpBasic::SetEmergMachine (Ptr<McpttEmergAlertMachineBasic>  emergMachine)
{
  NS_LOG_FUNCTION (this << &emergMachine);

  if (emergMachine != 0)
    {
      emergMachine->SetOwner (this);
    }

  m_emergMachine = emergMachine;
}

void
McpttCallMachineGrpBasic::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}

void
McpttCallMachineGrpBasic::SetMergeCb (const Callback<void, uint16_t>  mergeCb)
{
  NS_LOG_FUNCTION (this << &mergeCb);

  m_mergeCb = mergeCb;
}

void
McpttCallMachineGrpBasic::SetNewCallCb (const Callback<void, uint16_t>  newCallCb)
{
  NS_LOG_FUNCTION (this << &newCallCb);

  m_newCallCb = newCallCb;
}

void
McpttCallMachineGrpBasic::SetOrigId (const McpttCallMsgFieldUserId& origId)
{
  NS_LOG_FUNCTION (this << origId);

  m_origId = origId;
}

void
McpttCallMachineGrpBasic::SetCall (Ptr<McpttCall> call)
{
  NS_LOG_FUNCTION (this << call);
  m_call = call;
}

void
McpttCallMachineGrpBasic::SetProbeRespVal (const bool& probeRespVal)
{
  NS_LOG_FUNCTION (this << probeRespVal);

  m_probeRespVal = probeRespVal;
}

void
McpttCallMachineGrpBasic::SetRefInt (const McpttCallMsgFieldRefreshInterval& refInt)
{
  NS_LOG_FUNCTION (this << refInt);

  m_refInt = refInt;
}

void
McpttCallMachineGrpBasic::SetRndDelays (Ptr<RandomVariableStream>  rndDelays)
{
  NS_LOG_FUNCTION (this << &rndDelays);

  m_rndDelays = rndDelays;
}

void
McpttCallMachineGrpBasic::SetSdp (const McpttCallMsgFieldSdp& sdp)
{
  NS_LOG_FUNCTION (this << sdp);

  m_sdp = sdp;
}

void
McpttCallMachineGrpBasic::SetStartTime (const McpttCallMsgFieldStartTime& startTime)
{
  NS_LOG_FUNCTION (this << startTime);

  m_startTime = startTime;
}

void
McpttCallMachineGrpBasic::SetState (Ptr<McpttCallMachineGrpBasicState>  state)
{
  NS_LOG_FUNCTION (this << &state);

  m_state = state;
}

void
McpttCallMachineGrpBasic::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this << &stateChangeCb);

  m_stateChangeCb = stateChangeCb;
}

void
McpttCallMachineGrpBasic::SetTfg1 (Ptr<McpttTimer>  tfg1)
{
  NS_LOG_FUNCTION (this << tfg1);

  m_tfg1 = tfg1;
}

void
McpttCallMachineGrpBasic::SetTfg2 (Ptr<McpttTimer>  tfg2)
{
  NS_LOG_FUNCTION (this << tfg2);

  m_tfg2 = tfg2;
}

void
McpttCallMachineGrpBasic::SetTfg3 (Ptr<McpttTimer>  tfg3)
{
  NS_LOG_FUNCTION (this << tfg3);

  m_tfg3 = tfg3;
}

void
McpttCallMachineGrpBasic::SetTfg4 (Ptr<McpttTimer>  tfg4)
{
  NS_LOG_FUNCTION (this << tfg4);

  m_tfg4 = tfg4;
}

void
McpttCallMachineGrpBasic::SetTfg5 (Ptr<McpttTimer>  tfg5)
{
  NS_LOG_FUNCTION (this << tfg5);

  m_tfg5 = tfg5;
}

void
McpttCallMachineGrpBasic::SetTfg6 (Ptr<McpttTimer>  tfg6)
{
  NS_LOG_FUNCTION (this << tfg6);

  m_tfg6 = tfg6;
}

void
McpttCallMachineGrpBasic::SetTypeMachine (Ptr<McpttCallTypeMachineGrpBasic>  typeMachine)
{
  NS_LOG_FUNCTION (this << &typeMachine);

  if (typeMachine != 0)
    {
      typeMachine->SetOwner (this);
    }

  m_typeMachine = typeMachine;
}
/** McpttCallMachineGrpBasic - end **/

} // namespace psc
} // namespace ns3

