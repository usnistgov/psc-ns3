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
#include "mcptt-floor-machine.h"
#include "mcptt-floor-machine-basic-state.h"
#include "mcptt-floor-queue.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-ptt-app.h"
#include "mcptt-timer.h"

#include "mcptt-floor-machine-basic.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttFloorMachineBasic");

/** McpttFloorMachineBasic - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMachineBasic);

TypeId
McpttFloorMachineBasic::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttFloorMachineBasic")
    .SetParent<Object> ()
    .AddConstructor<McpttFloorMachineBasic>()
    .AddAttribute ("C201", "The initial limit of counter C201.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttFloorMachineBasic::SetLimitC201),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("C204", "The initial limit of counter C204.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttFloorMachineBasic::SetLimitC204),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("C205", "The initial limit of counter C205.",
                   UintegerValue (4),
                   MakeUintegerAccessor (&McpttFloorMachineBasic::SetLimitC205),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("GenMedia", "The flag that indicates if the floor machine should generate media when it has permission.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttFloorMachineBasic::m_genMedia),
                   MakeBooleanChecker ())
    .AddAttribute ("T201", "The delay to use for timer T201 (Time value)",
                   TimeValue (MilliSeconds (40)),
                   MakeTimeAccessor (&McpttFloorMachineBasic::SetDelayT201),
                   MakeTimeChecker ())
    .AddAttribute ("T203", "The delay to use for timer T203 (Time value)",
                   TimeValue (Seconds (4)),
                   MakeTimeAccessor (&McpttFloorMachineBasic::SetDelayT203),
                   MakeTimeChecker ())
    .AddAttribute ("T204", "The delay to use for timer T204 (Time value)",
                   TimeValue (MilliSeconds (80)),
                   MakeTimeAccessor (&McpttFloorMachineBasic::SetDelayT204),
                   MakeTimeChecker ())
    .AddAttribute ("T205", "The delay to use for timer T205 (Time value)",
                   TimeValue (MilliSeconds (80)),
                   MakeTimeAccessor (&McpttFloorMachineBasic::SetDelayT205),
                   MakeTimeChecker ())
    .AddAttribute ("T206", "The delay to use for timer T206 (Time value)",
                   TimeValue (Seconds (27)),
                   MakeTimeAccessor (&McpttFloorMachineBasic::SetDelayT206),
                   MakeTimeChecker ())
    .AddAttribute ("T207", "The delay to use for timer T207 (Time value)",
                   TimeValue (Seconds (3)),
                   MakeTimeAccessor (&McpttFloorMachineBasic::SetDelayT207),
                   MakeTimeChecker ())
    .AddAttribute ("T230", "The delay to use for timer T230 (Time value)",
                   TimeValue (Seconds (600)),
                   MakeTimeAccessor (&McpttFloorMachineBasic::SetDelayT230),
                   MakeTimeChecker ())
    .AddAttribute ("T233", "The delay to use for timer T233 (Time value)",
                   TimeValue (Seconds (3)),
                   MakeTimeAccessor (&McpttFloorMachineBasic::SetDelayT233),
                   MakeTimeChecker ())
    .AddTraceSource ("StateChangeTrace",
                      "The trace for capturing state changes.",
                      MakeTraceSourceAccessor (&McpttFloorMachineBasic::m_stateChangeTrace),
                      "ns3::McpttFloorMachineBasic::StateChangeTrace")
    ;
  
  return tid;
}

McpttFloorMachineBasic::McpttFloorMachineBasic (void)
  : McpttFloorMachine (),
    m_c201 (CreateObject<McpttCounter> (McpttEntityId (1, "C201"))),
    m_c204 (CreateObject<McpttCounter> (McpttEntityId (4, "C204"))),
    m_c205 (CreateObject<McpttCounter> (McpttEntityId (5, "C205"))),
    m_floorGrantedCb (MakeNullCallback<void> ()),
    m_lastGrantMsg (McpttFloorMsgGranted ()),
    m_originator (false),
    m_owner (0),
    m_priority (1),
    m_queue (CreateObject<McpttFloorQueue> ()),
    m_rxCb (MakeNullCallback<void, const McpttFloorMsg&> ()),
    m_setupDelayStartTime (Seconds (0)),
    m_setupDelayCb (MakeNullCallback<void, const Time&> ()),
    m_started (false),
    m_state (McpttFloorMachineBasicStateStartStop::GetInstance ()),
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
    m_t233 (CreateObject<McpttTimer> (McpttEntityId (33, "T233"))),
    m_txCb (MakeNullCallback<void, const McpttFloorMsg&> ())
{
  NS_LOG_FUNCTION (this);

  m_t201->Link (&McpttFloorMachineBasic::ExpiryOfT201, this);
  m_t203->Link (&McpttFloorMachineBasic::ExpiryOfT203, this);
  m_t204->Link (&McpttFloorMachineBasic::ExpiryOfT204, this);
  m_t205->Link (&McpttFloorMachineBasic::ExpiryOfT205, this);
  m_t206->Link (&McpttFloorMachineBasic::ExpiryOfT206, this);
  m_t207->Link (&McpttFloorMachineBasic::ExpiryOfT207, this);
  m_t230->Link (&McpttFloorMachineBasic::ExpiryOfT230, this);
  m_t233->Link (&McpttFloorMachineBasic::ExpiryOfT233, this);
}

McpttFloorMachineBasic::~McpttFloorMachineBasic (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineBasic::AcceptGrant (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << GetOwner ()->GetOwner ()->GetUserId () << " accepting grant" << ".");

  Ptr<McpttFloorMachineBasicState> state = GetState ();
  state->AcceptGrant (*this);
}

void
McpttFloorMachineBasic::ChangeState (Ptr<McpttFloorMachineBasicState>  state)
{
  NS_LOG_FUNCTION (this << state);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  McpttEntityId stateId = state->GetInstanceStateId ();
  Ptr<McpttFloorMachineBasicState> curr = GetState ();

  McpttEntityId currStateId = curr->GetInstanceStateId ();

  if (currStateId != stateId)
    {
      uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();

      NS_LOG_LOGIC ("UserId " << userId << " moving from state " << *curr << " to state " << *state << ".");

      curr->Unselected (*this);
      SetState (state);
      state->Selected (*this);

      if (state->GetInstanceStateId () == McpttFloorMachineBasicStateHasPerm::GetStateId ())
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
      m_stateChangeTrace (m_owner->GetOwner ()->GetUserId (), m_owner->GetCallId (), GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

void
McpttFloorMachineBasic::ClearCandidateSsrc (void)
{
  NS_LOG_FUNCTION (this);

  SetCandidateSsrc (0);
}

void
McpttFloorMachineBasic::ClearCurrentSsrc (void)
{
  NS_LOG_FUNCTION (this);

  SetCurrentSsrc (0);
}

uint8_t
McpttFloorMachineBasic::GetCallTypeId (void) const
{
  McpttCall* owner = GetOwner ();
  Ptr<McpttCallMachine> callMachine = owner->GetCallMachine ();
  McpttCallMsgFieldCallType callType = callMachine->GetCallType ();
  uint8_t callTypeId = callType.GetType ();

  return callTypeId;
}
 

McpttFloorMsgFieldIndic
McpttFloorMachineBasic::GetIndicator (void) const
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
McpttFloorMachineBasic::GetInstanceTypeId (void) const
{
  return McpttFloorMachineBasic::GetTypeId ();
}

McpttEntityId
McpttFloorMachineBasic::GetStateId (void) const
{
  Ptr<McpttFloorMachineBasicState> state = GetState ();
  McpttEntityId stateId = state->GetInstanceStateId ();

  return stateId;
}

uint32_t
McpttFloorMachineBasic::GetTxSsrc () const
{
  uint32_t txSsrc = GetOwner ()->GetOwner ()-> GetUserId ();

  return txSsrc;
}

bool
McpttFloorMachineBasic::HasCandidateSsrc (void) const
{
  uint32_t candidateSsrc = GetCandidateSsrc ();

  bool isStored = (candidateSsrc > 0);

  return isStored;
}

bool
McpttFloorMachineBasic::HasCurrentSsrc (void) const
{
  uint32_t currentSsrc = GetCurrentSsrc ();

  bool isStored = (currentSsrc > 0);

  return isStored;
}

bool
McpttFloorMachineBasic::HasFloor (void) const
{
  Ptr<McpttFloorMachineBasicState> state = GetState ();

  bool hasFloor = state->HasFloor (*this);

  // NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttFloorMachineBasic " << GetOwner ()->GetOwner ()-> GetUserId () << " does" << (hasFloor ? " " : " not ") << "have floor.");

  return hasFloor;
}

bool
McpttFloorMachineBasic::IsOriginator (void) const
{
  bool isOriginator = GetOriginator ();

  return isOriginator;
}

bool
McpttFloorMachineBasic::IsStarted (void) const
{
  bool isStarted = GetStarted ();

  return isStarted;
}

void
McpttFloorMachineBasic::MediaReady (McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetOwner ()->GetOwner ()-> GetUserId ();
  Ptr<McpttFloorMachineBasicState> state = GetState ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << myUserId << "'s client is about to send media.");

  state->MediaReady (*this, msg);
}

void
McpttFloorMachineBasic::NotifyFloorGranted (void)
{
  NS_LOG_FUNCTION (this);

  if (!m_floorGrantedCb.IsNull ())
    {
      m_floorGrantedCb ();
    }
}

void
McpttFloorMachineBasic::Receive (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  msg.Visit (*this);
}

void
McpttFloorMachineBasic::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  msg.Visit (*this);
}

void
McpttFloorMachineBasic::ReceiveFloorDeny (const McpttFloorMsgDeny& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttFloorMachineBasicState> state = GetState ();
  state->ReceiveFloorDeny (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (msg);
    }
}

void
McpttFloorMachineBasic::ReceiveFloorGranted (const McpttFloorMsgGranted& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttFloorMachineBasicState> state = GetState ();
  state->ReceiveFloorGranted (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (msg);
    }
}

void
McpttFloorMachineBasic::ReceiveFloorQueuePositionReq (const McpttFloorMsgQueuePosReq& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttFloorMachineBasicState> state = GetState ();
  state->ReceiveFloorQueuePositionReq (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (msg);
    }
}

void
McpttFloorMachineBasic::ReceiveFloorQueuePositionInfo (const McpttFloorMsgQueueInfo& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttFloorMachineBasicState> state = GetState ();
  state->ReceiveFloorQueuePositionInfo (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (msg);
    }
}

void
McpttFloorMachineBasic::ReceiveFloorRelease (const McpttFloorMsgRelease& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttFloorMachineBasicState> state = GetState ();
  state->ReceiveFloorRelease (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (msg);
    }
}

void
McpttFloorMachineBasic::ReceiveFloorReq (const McpttFloorMsgRequest& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttFloorMachineBasicState> state = GetState ();
  state->ReceiveFloorReq (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (msg);
    }
}

void
McpttFloorMachineBasic::ReceiveFloorTaken (const McpttFloorMsgTaken& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttFloorMachineBasicState> state = GetState ();
  state->ReceiveFloorTaken (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (msg);
    }
}

void
McpttFloorMachineBasic::ReceiveMedia (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << userId << " received " << msg.GetInstanceTypeId () << ".");

  Ptr<McpttFloorMachineBasicState> state = GetState ();
  state->ReceiveMedia (*this, msg);
}

void
McpttFloorMachineBasic::ReleaseRequest (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << GetOwner ()->GetOwner ()->GetUserId () << " releasing request" << ".");

  Ptr<McpttFloorMachineBasicState> state = GetState ();
  state->ReleaseRequest (*this);
}

void
McpttFloorMachineBasic::ResetCounters (void)
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
McpttFloorMachineBasic::Send (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  McpttCall* owner = GetOwner ();

  if (msg.IsA (McpttFloorMsgGranted::GetTypeId ()))
    {
      const McpttFloorMsgGranted& grantedMsg = dynamic_cast<const McpttFloorMsgGranted&> (msg);

      SetLastGrantMsg (grantedMsg);
    }

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << GetOwner ()->GetOwner ()->GetUserId () << " sending " << msg << ".");

  owner->Send (msg);

  if (!m_txCb.IsNull ())
    {
      m_txCb (msg);
    }
}

void
McpttFloorMachineBasic::SendFloorQueuePosReq (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << GetOwner ()->GetOwner ()->GetUserId () << " requesting queue position" << ".");

  Ptr<McpttFloorMachineBasicState> state = GetState ();
  state->SendFloorQueuePosReq (*this);
}

void
McpttFloorMachineBasic::SetDelayT201 (const Time& delayT201)
{
  NS_LOG_FUNCTION (this << delayT201);

  GetT201 ()->SetDelay (delayT201);
}

void
McpttFloorMachineBasic::SetDelayT203 (const Time& delayT203)
{
  NS_LOG_FUNCTION (this << delayT203);

  GetT203 ()->SetDelay (delayT203);
}

void
McpttFloorMachineBasic::SetDelayT204 (const Time& delayT204)
{
  NS_LOG_FUNCTION (this << delayT204);

  GetT204 ()->SetDelay (delayT204);
}

void
McpttFloorMachineBasic::SetDelayT205 (const Time& delayT205)
{
  NS_LOG_FUNCTION (this << delayT205);

  GetT205 ()->SetDelay (delayT205);
}

void
McpttFloorMachineBasic::SetDelayT206 (const Time& delayT206)
{
  NS_LOG_FUNCTION (this << delayT206);

  GetT206 ()->SetDelay (delayT206);
}

void
McpttFloorMachineBasic::SetDelayT207 (const Time& delayT207)
{
  NS_LOG_FUNCTION (this << delayT207);

  GetT207 ()->SetDelay (delayT207);
}

void
McpttFloorMachineBasic::SetDelayT230 (const Time& delayT230)
{
  NS_LOG_FUNCTION (this << delayT230);

  GetT230 ()->SetDelay (delayT230);
}

void
McpttFloorMachineBasic::SetDelayT233 (const Time& delayT233)
{
  NS_LOG_FUNCTION (this << delayT233);

  GetT233 ()->SetDelay (delayT233);
}

void
McpttFloorMachineBasic::SetLimitC201 (uint32_t limitC201)
{
  NS_LOG_FUNCTION (this << limitC201);

  GetC201 ()->SetLimit (limitC201);
}

void
McpttFloorMachineBasic::SetLimitC204 (uint32_t limitC204)
{
  NS_LOG_FUNCTION (this << limitC204);

  GetC204 ()->SetLimit (limitC204);
}

void
McpttFloorMachineBasic::SetLimitC205 (uint32_t limitC205)
{
  NS_LOG_FUNCTION (this << limitC205);

  GetC205 ()->SetLimit (limitC205);
}

bool
McpttFloorMachineBasic::ShouldGenMedia (void) const
{
  return m_genMedia;
}

void
McpttFloorMachineBasic::Start (void)
{
  Stop ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << GetOwner ()->GetOwner ()->GetUserId () << " started.");

  SetStarted (true);

  Ptr<McpttFloorMachineBasicState> state = GetState ();

  state->Selected (*this);

  state->Start (*this);
}

void
McpttFloorMachineBasic::Stop (void)
{
  NS_LOG_FUNCTION (this);
  if (IsStarted ())
    {
      Ptr<McpttFloorMachineBasicState> state = GetState ();

      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttFloorMachineBasic " << GetOwner ()->GetOwner ()->GetUserId () << " stopped.");

      state->Stop (*this);

      state->Unselected (*this);

      SetState (McpttFloorMachineBasicStateStartStop::GetInstance ());

      StopTimers ();
      ResetCounters ();

      ClearCandidateSsrc ();
      ClearCurrentSsrc ();

      SetStarted (false);
    }
}

void
McpttFloorMachineBasic::StopTimers (void)
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
McpttFloorMachineBasic::TakePushNotification (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint8_t callTypeId = GetCallTypeId ();
  Ptr<McpttCall> call = GetOwner ();
  Ptr<McpttPttApp> pttApp = call->GetOwner ();
  uint32_t userId = pttApp->GetUserId ();
  Ptr<McpttFloorMachineBasicState> state = GetState ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttFloorMachineBasic " << userId << " taking push notification.");

  if (callTypeId == McpttCallMsgFieldCallType::BROADCAST_GROUP)
    {
      //Provide local floor deny because PTT request are not allowed
      //from a terminaing user when part of a 'BROADCAST GROUP CALL'.
      //The originating user (or the user that started the call)
      //should have been given an implicit grant and thus should not
      //being making PTT request.
      NS_LOG_LOGIC ("McpttFloorMachineBasic " << userId << " denied locally since termintating users can't make PTT request when part of a 'BROADCAST GROUP CALL'.");

      if (pttApp->IsPushed ())
        {
          pttApp->Released ();
        }
    }
  else
    {
      SetSetupDelayStartTime (Simulator::Now ());
      state->TakePushNotification (*this);
    }
}

void
McpttFloorMachineBasic::TakeReleaseNotification (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();
  Ptr<McpttFloorMachineBasicState> state = GetState ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttFloorMachineBasic " << userId << " taking release notification.");

  state->TakeReleaseNotification (*this);
}

void
McpttFloorMachineBasic::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetC201 (0);
  SetC204 (0);
  SetC205 (0);
  SetOwner (0);
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
McpttFloorMachineBasic::ExpiryOfT201 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetOwner ()->GetOwner ()-> GetUserId ();
  Ptr<McpttFloorMachineBasicState> state = GetState ();
  Ptr<McpttCounter> c201 = GetC201 ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttFloorMachineBasic " << myUserId << " T201 expired " << c201->GetValue () << " times.");

  state->ExpiryOfT201 (*this);
}

void
McpttFloorMachineBasic::ExpiryOfT203 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetOwner ()->GetOwner ()-> GetUserId ();
  Ptr<McpttFloorMachineBasicState> state = GetState ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttFloorMachineBasic " << myUserId << " T203 expired.");

  state->ExpiryOfT203 (*this);
}

void
McpttFloorMachineBasic::ExpiryOfT204 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetOwner ()->GetOwner ()-> GetUserId ();
  Ptr<McpttFloorMachineBasicState> state = GetState ();
  Ptr<McpttCounter> c204 = GetC204 ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttFloorMachineBasic " << myUserId << " T204 expired " << c204->GetValue () << " times.");

  state->ExpiryOfT204 (*this);
}

void
McpttFloorMachineBasic::ExpiryOfT205 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();
  Ptr<McpttFloorMachineBasicState> state = GetState ();
  Ptr<McpttCounter> c205 = GetC205 ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttFloorMachineBasic " << userId << " T205 expired " << c205->GetValue () << " times.");

  state->ExpiryOfT205 (*this);
}

void
McpttFloorMachineBasic::ExpiryOfT206 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetOwner ()->GetOwner ()-> GetUserId ();
  Ptr<McpttFloorMachineBasicState> state = GetState ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttFloorMachineBasic " << myUserId << " T206 expired.");

  state->ExpiryOfT206 (*this);
}

void
McpttFloorMachineBasic::ExpiryOfT207 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetOwner ()->GetOwner ()-> GetUserId ();
  Ptr<McpttFloorMachineBasicState> state = GetState ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttFloorMachineBasic " << myUserId << " T207 expired.");

  state->ExpiryOfT207 (*this);
}

void
McpttFloorMachineBasic::ExpiryOfT230 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t userId = GetOwner ()->GetOwner ()-> GetUserId ();
  Ptr<McpttFloorMachineBasicState> state = GetState ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttFloorMachineBasic " << userId << " T230 expired.");

  state->ExpiryOfT230 (*this);
}

void
McpttFloorMachineBasic::ExpiryOfT233 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": " << GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  uint32_t myUserId = GetOwner ()->GetOwner ()-> GetUserId ();
  Ptr<McpttFloorMachineBasicState> state = GetState ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttFloorMachineBasic " << myUserId << " T233 expired.");

  state->ExpiryOfT233 (*this);
}

void
McpttFloorMachineBasic::SetStarted (const bool& started)
{
  NS_LOG_FUNCTION (this << started);

  m_started = started;
}

McpttFloorMsgGranted
McpttFloorMachineBasic::GetLastGrantMsg (void) const
{
  return m_lastGrantMsg;
}

bool
McpttFloorMachineBasic::GetStarted (void) const
{
  return m_started;
}

Ptr<McpttCounter>
McpttFloorMachineBasic::GetC201 (void) const
{
  return m_c201;
}

Ptr<McpttCounter>
McpttFloorMachineBasic::GetC204 (void) const
{
  return m_c204;
}

Ptr<McpttCounter>
McpttFloorMachineBasic::GetC205 (void) const
{
  return m_c205;
}

bool
McpttFloorMachineBasic::GetOriginator (void) const
{
  return m_originator;
}

McpttCall*
McpttFloorMachineBasic::GetOwner (void) const
{
  return m_owner;
}

uint8_t
McpttFloorMachineBasic::GetPriority (void) const
{
  return m_priority;
}

Ptr<McpttFloorQueue>
McpttFloorMachineBasic::GetQueue (void) const
{
  return m_queue;
}

Time
McpttFloorMachineBasic::GetSetupDelayStartTime (void) const
{
  return m_setupDelayStartTime;
}

Ptr<McpttFloorMachineBasicState>
McpttFloorMachineBasic::GetState (void) const
{
  return m_state;
}

uint32_t
McpttFloorMachineBasic::GetCandidateSsrc (void) const
{
  return m_candidateSsrc;
}

uint32_t
McpttFloorMachineBasic::GetCurrentSsrc (void) const
{
  return m_currentSsrc;
}

Ptr<McpttTimer>
McpttFloorMachineBasic::GetT201 (void) const
{
  return m_t201;
}

Ptr<McpttTimer>
McpttFloorMachineBasic::GetT203 (void) const
{
  return m_t203;
}

Ptr<McpttTimer>
McpttFloorMachineBasic::GetT204 (void) const
{
  return m_t204;
}

Ptr<McpttTimer>
McpttFloorMachineBasic::GetT205 (void) const
{
  return m_t205;
}

Ptr<McpttTimer>
McpttFloorMachineBasic::GetT206 (void) const
{
  return m_t206;
}

Ptr<McpttTimer>
McpttFloorMachineBasic::GetT207 (void) const
{
  return m_t207;
}

Ptr<McpttTimer>
McpttFloorMachineBasic::GetT230 (void) const
{
  return m_t230;
}

Ptr<McpttTimer>
McpttFloorMachineBasic::GetT233 (void) const
{
  return m_t233;
}

void
McpttFloorMachineBasic::SetC201 (Ptr<McpttCounter>  c201)
{
  NS_LOG_FUNCTION (this << c201);

  m_c201 = c201;
}

void
McpttFloorMachineBasic::SetC204 (Ptr<McpttCounter>  c204)
{
  NS_LOG_FUNCTION (this << c204);

  m_c204 = c204;
}

void
McpttFloorMachineBasic::SetC205 (Ptr<McpttCounter>  c205)
{
  NS_LOG_FUNCTION (this << c205);

  m_c205 = c205;
}

void
McpttFloorMachineBasic::SetFloorGrantedCb (const Callback<void>  floorGrantedCb)
{
  NS_LOG_FUNCTION (this);

  m_floorGrantedCb = floorGrantedCb;
}

void
McpttFloorMachineBasic::SetLastGrantMsg (const McpttFloorMsgGranted& msg)
{
  NS_LOG_FUNCTION (this << msg);

  m_lastGrantMsg = msg;
}

void
McpttFloorMachineBasic::SetOriginator (const bool& originator)
{
  NS_LOG_FUNCTION (this << originator);

  m_originator = originator;
}

void
McpttFloorMachineBasic::SetOwner (McpttCall* const& owner)
{
  NS_LOG_FUNCTION (this);

  m_owner = owner;
}

void
McpttFloorMachineBasic::SetPriority (uint8_t priority)
{
  NS_LOG_FUNCTION (this << +priority);

  m_priority = priority;
}

void
McpttFloorMachineBasic::SetQueue (Ptr<McpttFloorQueue>  queue)
{
  NS_LOG_FUNCTION (this << queue);

  m_queue = queue;
}

void
McpttFloorMachineBasic::SetRxCb (const Callback<void, const McpttFloorMsg&>  rxCb)
{
  NS_LOG_FUNCTION (this);

  m_rxCb = rxCb;
}

void
McpttFloorMachineBasic::SetSetupDelayStartTime (const Time& startTime)
{
  NS_LOG_FUNCTION (this << startTime);

  m_setupDelayStartTime = startTime;
}

void
McpttFloorMachineBasic::SetSetupDelayCb (const Callback<void, const Time&>  setupDelayCb)
{
  NS_LOG_FUNCTION (this);

  m_setupDelayCb = setupDelayCb;
}

void
McpttFloorMachineBasic::SetState (Ptr<McpttFloorMachineBasicState>  state)
{
  NS_LOG_FUNCTION (this << state);

  m_state = state;
}

void
McpttFloorMachineBasic::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this);

  m_stateChangeCb = stateChangeCb;
}

void
McpttFloorMachineBasic::SetCandidateSsrc (uint32_t candidateSsrc)
{
  NS_LOG_FUNCTION (this << candidateSsrc);

  m_candidateSsrc = candidateSsrc;
}

void
McpttFloorMachineBasic::SetCurrentSsrc (uint32_t currentSsrc)
{
  NS_LOG_FUNCTION (this << currentSsrc);

  m_currentSsrc = currentSsrc;
}

void
McpttFloorMachineBasic::SetT201 (Ptr<McpttTimer>  t201)
{
  NS_LOG_FUNCTION (this << t201);

  m_t201 = t201;
}

void
McpttFloorMachineBasic::SetT203 (Ptr<McpttTimer>  t203)
{
  NS_LOG_FUNCTION (this << t203);

  m_t203 = t203;
}

void
McpttFloorMachineBasic::SetT204 (Ptr<McpttTimer>  t204)
{
  NS_LOG_FUNCTION (this << t204);

  m_t204 = t204;
}

void
McpttFloorMachineBasic::SetT205 (Ptr<McpttTimer>  t205)
{
  NS_LOG_FUNCTION (this << t205);

  m_t205 = t205;
}

void
McpttFloorMachineBasic::SetT206 (Ptr<McpttTimer>  t206)
{
  NS_LOG_FUNCTION (this << t206);

  m_t206 = t206;
}

void
McpttFloorMachineBasic::SetT207 (Ptr<McpttTimer>  t207)
{
  NS_LOG_FUNCTION (this << t207);

  m_t207 = t207;
}

void
McpttFloorMachineBasic::SetT230 (Ptr<McpttTimer>  t230)
{
  NS_LOG_FUNCTION (this << t230);

  m_t230 = t230;
}

void
McpttFloorMachineBasic::SetT233 (Ptr<McpttTimer>  t233)
{
  NS_LOG_FUNCTION (this << t233);

  m_t233 = t233;
}

void
McpttFloorMachineBasic::SetTxCb (const Callback<void, const McpttFloorMsg&>  txCb)
{
  NS_LOG_FUNCTION (this);

  m_txCb = txCb;
}
/** McpttFloorMachineBasic - end **/

} // namespace ns3

