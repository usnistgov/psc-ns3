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
#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/simulator.h>
#include <ns3/type-id.h>

#include "mcptt-counter.h"
#include "mcptt-call-machine.h"
#include "mcptt-call-type-machine.h"
#include "mcptt-floor-participant.h"
#include "mcptt-off-network-floor-participant-state.h"
#include "mcptt-floor-queue.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-ptt-app.h"
#include "mcptt-timer.h"

#include "mcptt-off-network-floor-participant.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttOffNetworkFloorParticipant");

namespace psc {

/** McpttOffNetworkFloorParticipant - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttOffNetworkFloorParticipant);

TypeId
McpttOffNetworkFloorParticipant::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttOffNetworkFloorParticipant")
    .SetParent<McpttFloorParticipant> ()
    .AddConstructor<McpttOffNetworkFloorParticipant>()
    .AddAttribute ("C201", "The initial limit of counter C201.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttOffNetworkFloorParticipant::SetLimitC201),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("C204", "The initial limit of counter C204.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttOffNetworkFloorParticipant::SetLimitC204),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("C205", "The initial limit of counter C205.",
                   UintegerValue (4),
                   MakeUintegerAccessor (&McpttOffNetworkFloorParticipant::SetLimitC205),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("GenMedia", "The flag that indicates if the floor machine should generate media when it has permission.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttOffNetworkFloorParticipant::m_genMedia),
                   MakeBooleanChecker ())
    .AddAttribute ("T201", "The delay to use for timer T201 (Time value)",
                   TimeValue (MilliSeconds (40)),
                   MakeTimeAccessor (&McpttOffNetworkFloorParticipant::SetDelayT201),
                   MakeTimeChecker ())
    .AddAttribute ("T203", "The delay to use for timer T203 (Time value)",
                   TimeValue (Seconds (4)),
                   MakeTimeAccessor (&McpttOffNetworkFloorParticipant::SetDelayT203),
                   MakeTimeChecker ())
    .AddAttribute ("T204", "The delay to use for timer T204 (Time value)",
                   TimeValue (MilliSeconds (80)),
                   MakeTimeAccessor (&McpttOffNetworkFloorParticipant::SetDelayT204),
                   MakeTimeChecker ())
    .AddAttribute ("T205", "The delay to use for timer T205 (Time value)",
                   TimeValue (MilliSeconds (80)),
                   MakeTimeAccessor (&McpttOffNetworkFloorParticipant::SetDelayT205),
                   MakeTimeChecker ())
    .AddAttribute ("T206", "The delay to use for timer T206 (Time value)",
                   TimeValue (Seconds (27)),
                   MakeTimeAccessor (&McpttOffNetworkFloorParticipant::SetDelayT206),
                   MakeTimeChecker ())
    .AddAttribute ("T207", "The delay to use for timer T207 (Time value)",
                   TimeValue (Seconds (3)),
                   MakeTimeAccessor (&McpttOffNetworkFloorParticipant::SetDelayT207),
                   MakeTimeChecker ())
    .AddAttribute ("T230", "The delay to use for timer T230 (Time value)",
                   TimeValue (Seconds (600)),
                   MakeTimeAccessor (&McpttOffNetworkFloorParticipant::SetDelayT230),
                   MakeTimeChecker ())
    .AddAttribute ("T233", "The delay to use for timer T233 (Time value)",
                   TimeValue (Seconds (3)),
                   MakeTimeAccessor (&McpttOffNetworkFloorParticipant::SetDelayT233),
                   MakeTimeChecker ())
    .AddTraceSource ("StateChangeTrace",
                     "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttOffNetworkFloorParticipant::m_stateChangeTrace),
                     "ns3::psc::McpttFloorParticipant::StateChangeTracedCallback")
  ;

  return tid;
}

McpttOffNetworkFloorParticipant::McpttOffNetworkFloorParticipant (void)
  : McpttFloorParticipant (),
    m_c201 (CreateObject<McpttCounter> (McpttEntityId (1, "C201"))),
    m_c204 (CreateObject<McpttCounter> (McpttEntityId (4, "C204"))),
    m_c205 (CreateObject<McpttCounter> (McpttEntityId (5, "C205"))),
    m_floorGrantedCb (MakeNullCallback<void> ()),
    m_lastGrantMsg (McpttFloorMsgGranted ()),
    m_originator (false),
    m_call (0),
    m_priority (1),
    m_queue (CreateObject<McpttFloorQueue> ()),
    m_setupDelayStartTime (Seconds (0)),
    m_setupDelayCb (MakeNullCallback<void, const Time&> ()),
    m_started (false),
    m_state (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ()),
    m_candidateSsrc (0),
    m_currentSsrc (0),
    m_t201 (CreateObject<McpttTimer> (McpttEntityId (1, "T201"))),
    m_t203 (CreateObject<McpttTimer> (McpttEntityId (3, "T203"))),
    m_t204 (CreateObject<McpttTimer> (McpttEntityId (4, "T204"))),
    m_t205 (CreateObject<McpttTimer> (McpttEntityId (5, "T205"))),
    m_t206 (CreateObject<McpttTimer> (McpttEntityId (6, "T206"))),
    m_t207 (CreateObject<McpttTimer> (McpttEntityId (7, "T207"))),
    m_t230 (CreateObject<McpttTimer> (McpttEntityId (30, "T230"))),
    m_t233 (CreateObject<McpttTimer> (McpttEntityId (33, "T233")))
{
  NS_LOG_FUNCTION (this);

  m_t201->Link (&McpttOffNetworkFloorParticipant::ExpiryOfT201, this);
  m_t203->Link (&McpttOffNetworkFloorParticipant::ExpiryOfT203, this);
  m_t204->Link (&McpttOffNetworkFloorParticipant::ExpiryOfT204, this);
  m_t205->Link (&McpttOffNetworkFloorParticipant::ExpiryOfT205, this);
  m_t206->Link (&McpttOffNetworkFloorParticipant::ExpiryOfT206, this);
  m_t207->Link (&McpttOffNetworkFloorParticipant::ExpiryOfT207, this);
  m_t230->Link (&McpttOffNetworkFloorParticipant::ExpiryOfT230, this);
  m_t233->Link (&McpttOffNetworkFloorParticipant::ExpiryOfT233, this);
}

McpttOffNetworkFloorParticipant::~McpttOffNetworkFloorParticipant (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOffNetworkFloorParticipant::AcceptGrant (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << " accepting grant" << ".");

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  state->AcceptGrant (*this);
}

void
McpttOffNetworkFloorParticipant::ChangeState (Ptr<McpttOffNetworkFloorParticipantState>  state)
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
  Ptr<McpttOffNetworkFloorParticipantState> curr = GetState ();

  McpttEntityId currStateId = curr->GetInstanceStateId ();

  if (currStateId != stateId)
    {
      uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

      NS_LOG_LOGIC ("UserId " << userId << " moving from state " << *curr << " to state " << *state << ".");

      curr->Unselected (*this);
      SetState (state);
      state->Selected (*this);

      if (state->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ())
        {
          Time start = GetSetupDelayStartTime ();
          Time stop = Simulator::Now ();
          Time setupDelay = stop - start;
          NS_LOG_LOGIC ("UserId " << userId << " has permission; passing " << setupDelay.As (Time::S) << " to setup delay callback");
          if (!m_setupDelayCb.IsNull ())
            {
              m_setupDelayCb (setupDelay);
            }
        }

      if (!m_stateChangeCb.IsNull ())
        {
          m_stateChangeCb (curr->GetInstanceStateId (), state->GetInstanceStateId ());
        }
      m_stateChangeTrace (m_call->GetOwner ()->GetUserId (), m_call->GetCallId (), selected, GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

void
McpttOffNetworkFloorParticipant::ClearCandidateSsrc (void)
{
  NS_LOG_FUNCTION (this);

  SetCandidateSsrc (0);
}

void
McpttOffNetworkFloorParticipant::ClearCurrentSsrc (void)
{
  NS_LOG_FUNCTION (this);

  SetCurrentSsrc (0);
}

uint8_t
McpttOffNetworkFloorParticipant::GetCallTypeId (void) const
{
  Ptr<McpttCallMachine> callMachine = GetCall ()->GetCallMachine ();
  McpttCallMsgFieldCallType callType = callMachine->GetCallType ();
  uint8_t callTypeId = callType.GetType ();

  return callTypeId;
}


McpttFloorMsgFieldIndic
McpttOffNetworkFloorParticipant::GetIndicator (void) const
{
  McpttFloorMsgFieldIndic indicator;
  Ptr<McpttFloorQueue> queue = GetQueue ();
  uint8_t callTypeId = GetCallTypeId ();

  if (queue->IsEnabled ())
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::QUEUING_SUPP);
    }

  if (callTypeId == McpttCallMsgFieldCallType::BASIC_GROUP)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::NORMAL_CALL);
    }
  else if (callTypeId == McpttCallMsgFieldCallType::BROADCAST_GROUP)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::BROADCAST_CALL);
    }
  else if (callTypeId == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::EMERGENCY_CALL);
    }
  else if (callTypeId == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::IMMINENT_CALL);
    }
  else if (callTypeId == McpttCallMsgFieldCallType::PRIVATE)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::NORMAL_CALL);
    }
  else if (callTypeId == McpttCallMsgFieldCallType::EMERGENCY_PRIVATE)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::EMERGENCY_CALL);
    }

  return indicator;
}

TypeId
McpttOffNetworkFloorParticipant::GetInstanceTypeId (void) const
{
  return McpttOffNetworkFloorParticipant::GetTypeId ();
}

McpttEntityId
McpttOffNetworkFloorParticipant::GetStateId (void) const
{
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  McpttEntityId stateId = state->GetInstanceStateId ();

  return stateId;
}

uint32_t
McpttOffNetworkFloorParticipant::GetTxSsrc () const
{
  uint32_t txSsrc = GetCall ()->GetOwner ()->GetUserId ();

  return txSsrc;
}

bool
McpttOffNetworkFloorParticipant::HasCandidateSsrc (void) const
{
  uint32_t candidateSsrc = GetCandidateSsrc ();

  bool isStored = (candidateSsrc > 0);

  return isStored;
}

bool
McpttOffNetworkFloorParticipant::HasCurrentSsrc (void) const
{
  uint32_t currentSsrc = GetCurrentSsrc ();

  bool isStored = (currentSsrc > 0);

  return isStored;
}

bool
McpttOffNetworkFloorParticipant::HasFloor (void) const
{
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();

  bool hasFloor = state->HasFloor (*this);

  // NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << GetCall ()->GetOwner ()-> GetUserId () << " does" << (hasFloor ? " " : " not ") << "have floor.");

  return hasFloor;
}

bool
McpttOffNetworkFloorParticipant::IsOriginator (void) const
{
  bool isOriginator = GetOriginator ();

  return isOriginator;
}

bool
McpttOffNetworkFloorParticipant::IsStarted (void) const
{
  bool isStarted = GetStarted ();

  return isStarted;
}

void
McpttOffNetworkFloorParticipant::MediaReady (McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << myUserId << "'s client is about to send media.");

  state->MediaReady (*this, msg);
}

void
McpttOffNetworkFloorParticipant::NotifyFloorGranted (void)
{
  NS_LOG_FUNCTION (this);

  if (!m_floorGrantedCb.IsNull ())
    {
      m_floorGrantedCb ();
    }
}

void
McpttOffNetworkFloorParticipant::Receive (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  msg.Visit (*this);
}

void
McpttOffNetworkFloorParticipant::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  msg.Visit (*this);
}

void
McpttOffNetworkFloorParticipant::ReceiveFloorAck (const McpttFloorMsgAck& msg)
{
  NS_LOG_FUNCTION (this);
  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_ABORT_MSG ("McpttOffNetworkFloorParticipant " << userId << " should never receive " << msg.GetInstanceTypeId () << ".");
}

void
McpttOffNetworkFloorParticipant::ReceiveFloorDeny (const McpttFloorMsgDeny& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  state->ReceiveFloorDeny (*this, msg);
}

void
McpttOffNetworkFloorParticipant::ReceiveFloorGranted (const McpttFloorMsgGranted& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  state->ReceiveFloorGranted (*this, msg);
}

void
McpttOffNetworkFloorParticipant::ReceiveFloorIdle (const McpttFloorMsgIdle& msg)
{
  NS_LOG_FUNCTION (this);
  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_ABORT_MSG ("McpttOffNetworkFloorParticipant " << userId << " should never receive " << msg.GetInstanceTypeId () << ".");
}

void
McpttOffNetworkFloorParticipant::ReceiveFloorQueuePositionRequest (const McpttFloorMsgQueuePositionRequest& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  state->ReceiveFloorQueuePositionRequest (*this, msg);
}

void
McpttOffNetworkFloorParticipant::ReceiveFloorQueuePositionInfo (const McpttFloorMsgQueuePositionInfo& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  state->ReceiveFloorQueuePositionInfo (*this, msg);
}

void
McpttOffNetworkFloorParticipant::ReceiveFloorRelease (const McpttFloorMsgRelease& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  state->ReceiveFloorRelease (*this, msg);
}

void
McpttOffNetworkFloorParticipant::ReceiveFloorRequest (const McpttFloorMsgRequest& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  state->ReceiveFloorRequest (*this, msg);
}

void
McpttOffNetworkFloorParticipant::ReceiveFloorRevoke (const McpttFloorMsgRevoke& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_ABORT_MSG ("McpttOffNetworkFloorParticipant " << userId << " should never receive " << msg.GetInstanceTypeId () << ".");
}

void
McpttOffNetworkFloorParticipant::ReceiveFloorTaken (const McpttFloorMsgTaken& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  state->ReceiveFloorTaken (*this, msg);
}

void
McpttOffNetworkFloorParticipant::ReceiveMedia (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  state->ReceiveMedia (*this, msg);
}

void
McpttOffNetworkFloorParticipant::ReleaseRequest (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << " releasing request" << ".");

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  state->ReleaseRequest (*this);
}

void
McpttOffNetworkFloorParticipant::ResetCounters (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCounter> c201 = GetC201 ();
  Ptr<McpttCounter> c204 = GetC204 ();
  Ptr<McpttCounter> c205 = GetC205 ();

  c201->Reset ();
  c204->Reset ();
  c205->Reset ();
}

void
McpttOffNetworkFloorParticipant::ReportEvent (const char* reason) const
{
  GetCall ()->GetOwner ()->ReportEvent (GetCall ()->GetCallId (), reason);
}

void
McpttOffNetworkFloorParticipant::Send (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  if (msg.IsA (McpttFloorMsgGranted::GetTypeId ()))
    {
      const McpttFloorMsgGranted& grantedMsg = dynamic_cast<const McpttFloorMsgGranted&> (msg);

      SetLastGrantMsg (grantedMsg);
    }

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << " sending " << msg << ".");

  GetCall ()->Send (msg);
}

void
McpttOffNetworkFloorParticipant::SendFloorQueuePositionRequest (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << " requesting queue position" << ".");

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  state->SendFloorQueuePositionRequest (*this);
}

void
McpttOffNetworkFloorParticipant::SetDelayT201 (const Time& delayT201)
{
  NS_LOG_FUNCTION (this << delayT201);

  GetT201 ()->SetDelay (delayT201);
}

void
McpttOffNetworkFloorParticipant::SetDelayT203 (const Time& delayT203)
{
  NS_LOG_FUNCTION (this << delayT203);

  GetT203 ()->SetDelay (delayT203);
}

void
McpttOffNetworkFloorParticipant::SetDelayT204 (const Time& delayT204)
{
  NS_LOG_FUNCTION (this << delayT204);

  GetT204 ()->SetDelay (delayT204);
}

void
McpttOffNetworkFloorParticipant::SetDelayT205 (const Time& delayT205)
{
  NS_LOG_FUNCTION (this << delayT205);

  GetT205 ()->SetDelay (delayT205);
}

void
McpttOffNetworkFloorParticipant::SetDelayT206 (const Time& delayT206)
{
  NS_LOG_FUNCTION (this << delayT206);

  GetT206 ()->SetDelay (delayT206);
}

void
McpttOffNetworkFloorParticipant::SetDelayT207 (const Time& delayT207)
{
  NS_LOG_FUNCTION (this << delayT207);

  GetT207 ()->SetDelay (delayT207);
}

void
McpttOffNetworkFloorParticipant::SetDelayT230 (const Time& delayT230)
{
  NS_LOG_FUNCTION (this << delayT230);

  GetT230 ()->SetDelay (delayT230);
}

void
McpttOffNetworkFloorParticipant::SetDelayT233 (const Time& delayT233)
{
  NS_LOG_FUNCTION (this << delayT233);

  GetT233 ()->SetDelay (delayT233);
}

void
McpttOffNetworkFloorParticipant::SetLimitC201 (uint32_t limitC201)
{
  NS_LOG_FUNCTION (this << limitC201);

  GetC201 ()->SetLimit (limitC201);
}

void
McpttOffNetworkFloorParticipant::SetLimitC204 (uint32_t limitC204)
{
  NS_LOG_FUNCTION (this << limitC204);

  GetC204 ()->SetLimit (limitC204);
}

void
McpttOffNetworkFloorParticipant::SetLimitC205 (uint32_t limitC205)
{
  NS_LOG_FUNCTION (this << limitC205);

  GetC205 ()->SetLimit (limitC205);
}

bool
McpttOffNetworkFloorParticipant::ShouldGenMedia (void) const
{
  return m_genMedia;
}

void
McpttOffNetworkFloorParticipant::Start (void)
{
  Stop ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << " started.");

  SetStarted (true);

  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();

  state->Selected (*this);

  state->Start (*this);
}

void
McpttOffNetworkFloorParticipant::Stop (void)
{
  NS_LOG_FUNCTION (this);
  if (IsStarted ())
    {
      Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();

      NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << " stopped.");

      state->Stop (*this);

      state->Unselected (*this);

      SetState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ());

      StopTimers ();
      ResetCounters ();

      ClearCandidateSsrc ();
      ClearCurrentSsrc ();

      SetStarted (false);
    }
}

void
McpttOffNetworkFloorParticipant::StopTimers (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttTimer> t201 = GetT201 ();
  Ptr<McpttTimer> t203 = GetT203 ();
  Ptr<McpttTimer> t204 = GetT204 ();
  Ptr<McpttTimer> t205 = GetT205 ();
  Ptr<McpttTimer> t206 = GetT206 ();
  Ptr<McpttTimer> t207 = GetT207 ();
  Ptr<McpttTimer> t230 = GetT230 ();
  Ptr<McpttTimer> t233 = GetT233 ();

  if (t201->IsRunning ())
    {
      t201->Stop ();
    }
  if (t203->IsRunning ())
    {
      t203->Stop ();
    }
  if (t204->IsRunning ())
    {
      t204->Stop ();
    }
  if (t205->IsRunning ())
    {
      t205->Stop ();
    }
  if (t206->IsRunning ())
    {
      t206->Stop ();
    }
  if (t207->IsRunning ())
    {
      t207->Stop ();
    }
  if (t230->IsRunning ())
    {
      t230->Stop ();
    }
  if (t233->IsRunning ())
    {
      t233->Stop ();
    }
}

void
McpttOffNetworkFloorParticipant::PttPush (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint8_t callTypeId = GetCallTypeId ();
  Ptr<McpttPttApp> pttApp = GetCall ()->GetOwner ();
  uint32_t userId = pttApp->GetUserId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " taking push notification.");

  if (callTypeId == McpttCallMsgFieldCallType::BROADCAST_GROUP)
    {
      //Provide local floor deny because PTT request are not allowed
      //from a terminaing user when part of a 'BROADCAST GROUP CALL'.
      //The originating user (or the user that started the call)
      //should have been given an implicit grant and thus should not
      //being making PTT request.
      NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " denied locally since termintating users can't make PTT request when part of a 'BROADCAST GROUP CALL'.");

      if (pttApp->IsPushed ())
        {
          pttApp->NotifyReleased ();
        }
    }
  else
    {
      SetSetupDelayStartTime (Simulator::Now ());
      state->PttPush (*this);
    }
}

void
McpttOffNetworkFloorParticipant::PttRelease (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " taking release notification.");

  state->PttRelease (*this);
}

void
McpttOffNetworkFloorParticipant::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetC201 (0);
  SetC204 (0);
  SetC205 (0);
  SetCall (0);
  SetQueue (0);
  SetState (0);
  SetT201 (0);
  SetT203 (0);
  SetT204 (0);
  SetT205 (0);
  SetT206 (0);
  SetT207 (0);
  SetT230 (0);
  SetT233 (0);
}

void
McpttOffNetworkFloorParticipant::ExpiryOfT201 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  Ptr<McpttCounter> c201 = GetC201 ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << myUserId << " T201 expired " << c201->GetValue () << " times.");

  state->ExpiryOfT201 (*this);
}

void
McpttOffNetworkFloorParticipant::ExpiryOfT203 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << myUserId << " T203 expired.");

  state->ExpiryOfT203 (*this);
}

void
McpttOffNetworkFloorParticipant::ExpiryOfT204 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  Ptr<McpttCounter> c204 = GetC204 ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << myUserId << " T204 expired " << c204->GetValue () << " times.");

  state->ExpiryOfT204 (*this);
}

void
McpttOffNetworkFloorParticipant::ExpiryOfT205 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();
  Ptr<McpttCounter> c205 = GetC205 ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " T205 expired " << c205->GetValue () << " times.");

  state->ExpiryOfT205 (*this);
}

void
McpttOffNetworkFloorParticipant::ExpiryOfT206 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << myUserId << " T206 expired.");

  state->ExpiryOfT206 (*this);
}

void
McpttOffNetworkFloorParticipant::ExpiryOfT207 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << myUserId << " T207 expired.");

  state->ExpiryOfT207 (*this);
}

void
McpttOffNetworkFloorParticipant::ExpiryOfT230 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << userId << " T230 expired.");

  state->ExpiryOfT230 (*this);
}

void
McpttOffNetworkFloorParticipant::ExpiryOfT233 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = GetState ();

  NS_LOG_LOGIC ("McpttOffNetworkFloorParticipant " << myUserId << " T233 expired.");

  state->ExpiryOfT233 (*this);
}

void
McpttOffNetworkFloorParticipant::SetStarted (const bool& started)
{
  NS_LOG_FUNCTION (this << started);

  m_started = started;
}

McpttFloorMsgGranted
McpttOffNetworkFloorParticipant::GetLastGrantMsg (void) const
{
  return m_lastGrantMsg;
}

bool
McpttOffNetworkFloorParticipant::GetStarted (void) const
{
  return m_started;
}

Ptr<McpttCounter>
McpttOffNetworkFloorParticipant::GetC201 (void) const
{
  return m_c201;
}

Ptr<McpttCounter>
McpttOffNetworkFloorParticipant::GetC204 (void) const
{
  return m_c204;
}

Ptr<McpttCounter>
McpttOffNetworkFloorParticipant::GetC205 (void) const
{
  return m_c205;
}

bool
McpttOffNetworkFloorParticipant::GetOriginator (void) const
{
  return m_originator;
}

Ptr<McpttCall>
McpttOffNetworkFloorParticipant::GetCall (void) const
{
  return m_call;
}

uint8_t
McpttOffNetworkFloorParticipant::GetPriority (void) const
{
  return m_priority;
}

Ptr<McpttFloorQueue>
McpttOffNetworkFloorParticipant::GetQueue (void) const
{
  return m_queue;
}

Time
McpttOffNetworkFloorParticipant::GetSetupDelayStartTime (void) const
{
  return m_setupDelayStartTime;
}

Ptr<McpttOffNetworkFloorParticipantState>
McpttOffNetworkFloorParticipant::GetState (void) const
{
  return m_state;
}

uint32_t
McpttOffNetworkFloorParticipant::GetCandidateSsrc (void) const
{
  return m_candidateSsrc;
}

uint32_t
McpttOffNetworkFloorParticipant::GetCurrentSsrc (void) const
{
  return m_currentSsrc;
}

Ptr<McpttTimer>
McpttOffNetworkFloorParticipant::GetT201 (void) const
{
  return m_t201;
}

Ptr<McpttTimer>
McpttOffNetworkFloorParticipant::GetT203 (void) const
{
  return m_t203;
}

Ptr<McpttTimer>
McpttOffNetworkFloorParticipant::GetT204 (void) const
{
  return m_t204;
}

Ptr<McpttTimer>
McpttOffNetworkFloorParticipant::GetT205 (void) const
{
  return m_t205;
}

Ptr<McpttTimer>
McpttOffNetworkFloorParticipant::GetT206 (void) const
{
  return m_t206;
}

Ptr<McpttTimer>
McpttOffNetworkFloorParticipant::GetT207 (void) const
{
  return m_t207;
}

Ptr<McpttTimer>
McpttOffNetworkFloorParticipant::GetT230 (void) const
{
  return m_t230;
}

Ptr<McpttTimer>
McpttOffNetworkFloorParticipant::GetT233 (void) const
{
  return m_t233;
}

void
McpttOffNetworkFloorParticipant::SetC201 (Ptr<McpttCounter>  c201)
{
  NS_LOG_FUNCTION (this << c201);

  m_c201 = c201;
}

void
McpttOffNetworkFloorParticipant::SetC204 (Ptr<McpttCounter>  c204)
{
  NS_LOG_FUNCTION (this << c204);

  m_c204 = c204;
}

void
McpttOffNetworkFloorParticipant::SetC205 (Ptr<McpttCounter>  c205)
{
  NS_LOG_FUNCTION (this << c205);

  m_c205 = c205;
}

void
McpttOffNetworkFloorParticipant::SetFloorGrantedCb (const Callback<void>  floorGrantedCb)
{
  NS_LOG_FUNCTION (this);

  m_floorGrantedCb = floorGrantedCb;
}

void
McpttOffNetworkFloorParticipant::SetLastGrantMsg (const McpttFloorMsgGranted& msg)
{
  NS_LOG_FUNCTION (this << msg);

  m_lastGrantMsg = msg;
}

void
McpttOffNetworkFloorParticipant::SetOriginator (const bool& originator)
{
  NS_LOG_FUNCTION (this << originator);

  m_originator = originator;
}

void
McpttOffNetworkFloorParticipant::SetCall (Ptr<McpttCall> call)
{
  NS_LOG_FUNCTION (this);

  m_call = call;
}

void
McpttOffNetworkFloorParticipant::SetPriority (uint8_t priority)
{
  NS_LOG_FUNCTION (this << +priority);

  m_priority = priority;
}

void
McpttOffNetworkFloorParticipant::SetQueue (Ptr<McpttFloorQueue>  queue)
{
  NS_LOG_FUNCTION (this << queue);

  m_queue = queue;
}

void
McpttOffNetworkFloorParticipant::SetSetupDelayStartTime (const Time& startTime)
{
  NS_LOG_FUNCTION (this << startTime);

  m_setupDelayStartTime = startTime;
}

void
McpttOffNetworkFloorParticipant::SetSetupDelayCb (const Callback<void, const Time&>  setupDelayCb)
{
  NS_LOG_FUNCTION (this);

  m_setupDelayCb = setupDelayCb;
}

void
McpttOffNetworkFloorParticipant::SetState (Ptr<McpttOffNetworkFloorParticipantState>  state)
{
  NS_LOG_FUNCTION (this << state);

  m_state = state;
}

void
McpttOffNetworkFloorParticipant::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this);

  m_stateChangeCb = stateChangeCb;
}

void
McpttOffNetworkFloorParticipant::SetCandidateSsrc (uint32_t candidateSsrc)
{
  NS_LOG_FUNCTION (this << candidateSsrc);

  m_candidateSsrc = candidateSsrc;
}

void
McpttOffNetworkFloorParticipant::SetCurrentSsrc (uint32_t currentSsrc)
{
  NS_LOG_FUNCTION (this << currentSsrc);

  m_currentSsrc = currentSsrc;
}

void
McpttOffNetworkFloorParticipant::SetT201 (Ptr<McpttTimer>  t201)
{
  NS_LOG_FUNCTION (this << t201);

  m_t201 = t201;
}

void
McpttOffNetworkFloorParticipant::SetT203 (Ptr<McpttTimer>  t203)
{
  NS_LOG_FUNCTION (this << t203);

  m_t203 = t203;
}

void
McpttOffNetworkFloorParticipant::SetT204 (Ptr<McpttTimer>  t204)
{
  NS_LOG_FUNCTION (this << t204);

  m_t204 = t204;
}

void
McpttOffNetworkFloorParticipant::SetT205 (Ptr<McpttTimer>  t205)
{
  NS_LOG_FUNCTION (this << t205);

  m_t205 = t205;
}

void
McpttOffNetworkFloorParticipant::SetT206 (Ptr<McpttTimer>  t206)
{
  NS_LOG_FUNCTION (this << t206);

  m_t206 = t206;
}

void
McpttOffNetworkFloorParticipant::SetT207 (Ptr<McpttTimer>  t207)
{
  NS_LOG_FUNCTION (this << t207);

  m_t207 = t207;
}

void
McpttOffNetworkFloorParticipant::SetT230 (Ptr<McpttTimer>  t230)
{
  NS_LOG_FUNCTION (this << t230);

  m_t230 = t230;
}

void
McpttOffNetworkFloorParticipant::SetT233 (Ptr<McpttTimer>  t233)
{
  NS_LOG_FUNCTION (this << t233);

  m_t233 = t233;
}

/** McpttOffNetworkFloorParticipant - end **/

} // namespace psc
} // namespace ns3

