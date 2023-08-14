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

#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-ptt-app.h"
#include "mcptt-floor-participant.h"
#include "mcptt-call-machine-private.h"
#include "mcptt-call-msg.h"
#include "mcptt-call-msg-field.h"
#include "mcptt-call-type-machine.h"
#include "mcptt-call-type-machine-private-state.h"
#include "mcptt-emerg-alert-machine.h"
#include "mcptt-entity-id.h"

#include "mcptt-call-type-machine-private.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallTypeMachinePrivate");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttCallTypeMachinePrivate);

TypeId
McpttCallTypeMachinePrivate::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttCallTypeMachinePrivate")
    .SetParent<McpttCallTypeMachine> ()
    .AddConstructor<McpttCallTypeMachinePrivate> ()
    .AddTraceSource ("StateChangeTrace", "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttCallTypeMachinePrivate::m_stateChangeTrace),
                     "ns3::psc::McpttCallMachine::StateChangeTracedCallback")
  ;

  return tid;
}

McpttCallTypeMachinePrivate::McpttCallTypeMachinePrivate (Ptr<McpttCallMachinePrivate> owner)
  : McpttCallTypeMachine (),
    m_callType (McpttCallMsgFieldCallType::PRIVATE),
    m_owner (owner),
    m_priority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP)),
    m_state (McpttCallTypeMachinePrivateStateQ0::GetInstance ()),
    m_upgradeCb (MakeNullCallback<void, uint8_t> ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallTypeMachinePrivate::~McpttCallTypeMachinePrivate (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallTypeMachinePrivate::ChangeState (Ptr<McpttCallTypeMachinePrivateState>  state)
{
  NS_LOG_FUNCTION (this << state);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }
  std::string selected = "False";
  if (GetOwner ()->GetCall ()->GetCallId () == GetOwner ()->GetCall ()->GetOwner ()->GetSelectedCall ()->GetCallId ())
    {
      selected = "True";
    }

  McpttEntityId stateId = state->GetInstanceStateId ();
  Ptr<McpttCallTypeMachinePrivateState> curr = GetState ();

  McpttEntityId currStateId = curr->GetInstanceStateId ();

  if (currStateId != stateId)
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () << " moving from state " << *curr << " to state " << *state << ".");

      SetState (state);
      m_stateChangeTrace (GetOwner ()->GetCall ()->GetOwner ()->GetUserId (), GetOwner ()->GetCall ()->GetCallId (), selected, GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

void
McpttCallTypeMachinePrivate::Downgrade (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " downgrading call.");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->Downgrade (*this);
}

void
McpttCallTypeMachinePrivate::ExpiryOfTfp1 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  "'s TFP1 timer expired.");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->ExpiryOfTfp1 (*this);
}

void
McpttCallTypeMachinePrivate::ExpiryOfTfp6 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  "'s TFP6 timer expired.");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->ExpiryOfTfp6 (*this);
}

void
McpttCallTypeMachinePrivate::ExpiryOfTfp8 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  "'s TFP8 timer expired.");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->ExpiryOfTfp8 (*this);
}

TypeId
McpttCallTypeMachinePrivate::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallTypeMachinePrivate::GetTypeId ();
}

McpttEntityId
McpttCallTypeMachinePrivate::GetStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return GetState ()->GetInstanceStateId ();
}

void
McpttCallTypeMachinePrivate::InitiateCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " initiating private call.");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->InitiateCall (*this);
}

bool
McpttCallTypeMachinePrivate::IsStarted (void) const
{
  NS_LOG_FUNCTION (this);

  bool isStarted = GetStarted ();

  return isStarted;
}

void
McpttCallTypeMachinePrivate::ReceiveAccept (const McpttCallMsgPrivateAccept& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " received " << msg << ".");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->ReceiveAccept (*this, msg);
}

void
McpttCallTypeMachinePrivate::ReceiveAcceptAck (const McpttCallMsgPrivateAcceptAck& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " received " << msg << ".");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->ReceiveAcceptAck (*this, msg);
}

void
McpttCallTypeMachinePrivate::ReceiveEmergencyCancel (const McpttCallMsgPrivateEmergCancel& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " received " << msg << ".");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->ReceiveEmergencyCancel (*this, msg);
}

void
McpttCallTypeMachinePrivate::ReceiveEmergencyCancelAck (const McpttCallMsgPrivateEmergCancelAck& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " received " << msg << ".");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->ReceiveEmergencyCancelAck (*this, msg);
}

void
McpttCallTypeMachinePrivate::ReceiveReject (const McpttCallMsgPrivateReject& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " received " << msg << ".");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->ReceiveReject (*this, msg);
}

void
McpttCallTypeMachinePrivate::ReceiveRelease (const McpttCallMsgPrivateRelease& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " received " << msg << ".");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->ReceiveRelease (*this, msg);
}

void
McpttCallTypeMachinePrivate::ReceiveSetupRequest (const McpttCallMsgPrivateSetupReq& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " received " << msg << ".");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->ReceiveSetupRequest (*this, msg);
}

void
McpttCallTypeMachinePrivate::ReleaseCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " releasing call.");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->ReleaseCall (*this);
}

void
McpttCallTypeMachinePrivate::Send (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () << " trying to send " << msg << ".");

  Ptr<McpttCallMachinePrivate> owner = GetOwner ();
  owner->Send (msg);
}

void
McpttCallTypeMachinePrivate::Start (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " started.");

  SetStarted (true);
}

void
McpttCallTypeMachinePrivate::Stop (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " stopped.");

  SetStarted (false);
}

void
McpttCallTypeMachinePrivate::UpgradeTo (const McpttCallMsgFieldCallType& callType)
{
  NS_LOG_FUNCTION (this << callType);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " upgrading call type to " << (uint32_t)callType.GetType () << ".");

  Ptr<McpttCallTypeMachinePrivateState> state = GetState ();

  state->UpgradeTo (*this, callType);
}

void
McpttCallTypeMachinePrivate::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetOwner (0);
}

McpttCallMsgFieldCallType
McpttCallTypeMachinePrivate::GetCallType (void) const
{
  NS_LOG_FUNCTION (this);

  return m_callType;
}

Ptr<McpttCallMachinePrivate>
McpttCallTypeMachinePrivate::GetOwner (void) const
{
  NS_LOG_FUNCTION (this);

  return m_owner;
}

uint8_t
McpttCallTypeMachinePrivate::GetPriority (void) const
{
  NS_LOG_FUNCTION (this);

  return m_priority;
}

bool
McpttCallTypeMachinePrivate::GetStarted (void) const
{
  NS_LOG_FUNCTION (this);

  return m_started;
}

Ptr<McpttCallTypeMachinePrivateState>
McpttCallTypeMachinePrivate::GetState (void) const
{
  NS_LOG_FUNCTION (this);

  return m_state;
}

void
McpttCallTypeMachinePrivate::SetDowngradeCb (const Callback<void>  downgradeCb)
{
  NS_LOG_FUNCTION (this << &downgradeCb);

  m_downgradeCb = downgradeCb;
}

void
McpttCallTypeMachinePrivate::SetCallType (const McpttCallMsgFieldCallType& callType)
{
  NS_LOG_FUNCTION (this << callType);

  m_callType = callType;
}

void
McpttCallTypeMachinePrivate::SetOwner (Ptr<McpttCallMachinePrivate> owner)
{
  NS_LOG_FUNCTION (this << owner);

  m_owner = owner;
}

void
McpttCallTypeMachinePrivate::SetPriority (uint8_t priority)
{
  NS_LOG_FUNCTION (this << (uint32_t)priority);

  m_priority = priority;
}

void
McpttCallTypeMachinePrivate::SetStarted (const bool& started)
{
  NS_LOG_FUNCTION (this << started);

  m_started = started;
}

void
McpttCallTypeMachinePrivate::SetState (Ptr<McpttCallTypeMachinePrivateState>  state)
{
  NS_LOG_FUNCTION (this << &state);

  m_state = state;
}

void
McpttCallTypeMachinePrivate::SetUpgradeCb (const Callback<void, uint8_t>  upgradeCb)
{
  NS_LOG_FUNCTION (this << &upgradeCb);

  m_upgradeCb = upgradeCb;
}

} // namespace psc
} // namespace ns3

