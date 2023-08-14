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

#define NS_LOG_APPEND_CONTEXT \
  if (m_owner) { std::clog << " [callId " << m_owner->GetCallId () << "] "; }

#include <unordered_map>
#include <sstream>

#include <ns3/boolean.h>
#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/object-vector.h>
#include <ns3/ptr.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>
#include <ns3/type-id.h>

#include "mcptt-counter.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-msg-sink.h"
#include "mcptt-media-msg.h"
#include "mcptt-on-network-floor-arbitrator-state.h"
#include "mcptt-server-app.h"
#include "mcptt-server-call.h"
#include "mcptt-server-call-machine.h"
#include "mcptt-on-network-floor-towards-participant.h"
#include "mcptt-ptt-app.h"
#include "mcptt-timer.h"

#include "mcptt-on-network-floor-arbitrator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttOnNetworkFloorArbitrator");

namespace psc {

/** McpttOnNetworkFloorArbitrator - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttOnNetworkFloorArbitrator);

TypeId
McpttOnNetworkFloorArbitrator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttOnNetworkFloorArbitrator")
    .SetParent<Object> ()
    .AddConstructor<McpttOnNetworkFloorArbitrator>()
    .AddAttribute ("AckRequired", "The flag that indicates if acknowledgement is required.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttOnNetworkFloorArbitrator::m_ackRequired),
                   MakeBooleanChecker ())
    .AddAttribute ("AudioCutIn", "The flag that indicates if the group is configured for audio cut-in.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttOnNetworkFloorArbitrator::m_audioCutIn),
                   MakeBooleanChecker ())
    .AddAttribute ("C7", "The initial limit of counter C7 (Floor Idle).",
                   UintegerValue (10),
                   MakeUintegerAccessor (&McpttOnNetworkFloorArbitrator::SetLimitC7),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("C20", "The initial limit of counter C20 (Floor Granted).",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttOnNetworkFloorArbitrator::SetLimitC20),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("DualFloorSupported", "The flag that indicates if dual floor control is supported.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttOnNetworkFloorArbitrator::m_dualFloorSupported),
                   MakeBooleanChecker ())
    .AddAttribute ("DualFloorControl", "The dual floor control state machine.",
                   PointerValue (CreateObject<McpttOnNetworkFloorDualControl> ()),
                   MakePointerAccessor (&McpttOnNetworkFloorArbitrator::GetDualControl,
                                        &McpttOnNetworkFloorArbitrator::SetDualControl),
                   MakePointerChecker<McpttOnNetworkFloorArbitrator> ())
    .AddAttribute ("FloorParticipants", "The list of floor participants associated with this arbitrator.",
                   ObjectVectorValue (),
                   MakeObjectVectorAccessor (&McpttOnNetworkFloorArbitrator::m_participants),
                   MakeObjectVectorChecker<McpttOnNetworkFloorTowardsParticipant> ())
    .AddAttribute ("QueueingSupported", "Whether queueing of floor requests is supported.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttOnNetworkFloorArbitrator::m_queueingSupported),
                   MakeBooleanChecker ())
    .AddAttribute ("TxSsrc", "The SSRC to use when transmitting a message.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&McpttOnNetworkFloorArbitrator::m_txSsrc),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("T1", "The delay to use for end-of-media timer T1 (Time value)",
                   TimeValue (Seconds (4)),
                   MakeTimeAccessor (&McpttOnNetworkFloorArbitrator::SetDelayT1),
                   MakeTimeChecker ())
    .AddAttribute ("T2", "The delay to use for stop talking timer T2 (Time value)",
                   TimeValue (Seconds (30)),
                   MakeTimeAccessor (&McpttOnNetworkFloorArbitrator::SetDelayT2),
                   MakeTimeChecker ())
    .AddAttribute ("T3", "The delay to use for stop talking grace timer T3 (Time value)",
                   TimeValue (Seconds (3)),
                   MakeTimeAccessor (&McpttOnNetworkFloorArbitrator::SetDelayT3),
                   MakeTimeChecker ())
    .AddAttribute ("T4", "The delay to use for inactivity timer T4 (Time value)",
                   TimeValue (Seconds (30)),
                   MakeTimeAccessor (&McpttOnNetworkFloorArbitrator::SetDelayT4),
                   MakeTimeChecker ())
    .AddAttribute ("T7", "The delay to use for Floor Idle timer T7 (Time value)",
                   TimeValue (Seconds (1)),
                   MakeTimeAccessor (&McpttOnNetworkFloorArbitrator::SetDelayT7),
                   MakeTimeChecker ())
    .AddAttribute ("T20", "The delay to use for Floor Granted timer T20 (Time value)",
                   TimeValue (Seconds (1)),
                   MakeTimeAccessor (&McpttOnNetworkFloorArbitrator::SetDelayT20),
                   MakeTimeChecker ())
    .AddTraceSource ("StateChangeTrace",
                     "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttOnNetworkFloorArbitrator::m_stateChangeTrace),
                     "ns3::psc::McpttOnNetworkFloorArbitrator::StateChangeTracedCallback")
  ;

  return tid;
}

McpttOnNetworkFloorArbitrator::McpttOnNetworkFloorArbitrator (void)
  : Object (),
    m_c7 (CreateObject<McpttCounter> (McpttEntityId (7, "C7"))),
    m_c20 (CreateObject<McpttCounter> (McpttEntityId (20, "C20"))),
    m_dualFloorSupported (false),
    m_owner (0),
    m_queueingSupported (false),
    m_queue (CreateObject<McpttFloorQueue> ()),
    m_rejectCause (0),
    m_seqNum (0),
    m_state (McpttOnNetworkFloorArbitratorStateStartStop::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ()),
    m_storedSsrc (0),
    m_storedPriority (0),
    m_trackInfo (McpttFloorMsgFieldTrackInfo ()),
    m_t1 (CreateObject<McpttTimer> (McpttEntityId (1, "T1"))),
    m_t2 (CreateObject<McpttTimer> (McpttEntityId (2, "T2"))),
    m_t3 (CreateObject<McpttTimer> (McpttEntityId (3, "T3"))),
    m_t4 (CreateObject<McpttTimer> (McpttEntityId (4, "T4"))),
    m_t7 (CreateObject<McpttTimer> (McpttEntityId (7, "T7"))),
    m_t20 (CreateObject<McpttTimer> (McpttEntityId (20, "T20")))
{
  NS_LOG_FUNCTION (this);

  m_t1->Link (&McpttOnNetworkFloorArbitrator::ExpiryOfT1, this);
  m_t2->Link (&McpttOnNetworkFloorArbitrator::ExpiryOfT2, this);
  m_t3->Link (&McpttOnNetworkFloorArbitrator::ExpiryOfT3, this);
  m_t4->Link (&McpttOnNetworkFloorArbitrator::ExpiryOfT4, this);
  m_t7->Link (&McpttOnNetworkFloorArbitrator::ExpiryOfT7, this);
  m_t20->Link (&McpttOnNetworkFloorArbitrator::ExpiryOfT20, this);
}

McpttOnNetworkFloorArbitrator::~McpttOnNetworkFloorArbitrator (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkFloorArbitrator::AddParticipant (Ptr<McpttOnNetworkFloorTowardsParticipant> participant)
{
  NS_LOG_FUNCTION (this);

  participant->SetOwner (this);
  m_participants.push_back (participant);
}

void
McpttOnNetworkFloorArbitrator::CallInitialized (Ptr<McpttOnNetworkFloorTowardsParticipant> participant, bool implicitRequest)
{
  NS_LOG_FUNCTION (this << participant << implicitRequest);
  if (m_state->GetInstanceStateId () == McpttOnNetworkFloorArbitratorStateStartStop::GetStateId ())
    {
      NS_LOG_LOGIC ("Call initialized");
      participant->SetOriginator (true);
      participant->CallInitialized (implicitRequest);
      m_state->CallInitialized (Ptr<McpttOnNetworkFloorArbitrator> (this), participant);
    }
  else
    {
      // If the state machine has already left StartStop state, such as when
      // a second INVITE is received, still respond to the implicit request.
      NS_LOG_LOGIC ("Attempt to initialize call but call already in " << m_state->GetInstanceStateId ().GetName ());
      participant->SetOriginator (false);
      participant->CallInitialized (implicitRequest);
      m_state->CallInitialized (Ptr<McpttOnNetworkFloorArbitrator> (this), participant);
    }
}

void
McpttOnNetworkFloorArbitrator::CallRelease1 (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Call released (part I).");
  m_state->CallRelease1 (Ptr<McpttOnNetworkFloorArbitrator> (this));
}

void
McpttOnNetworkFloorArbitrator::CallRelease2 (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Call released (part II).");
  m_state->CallRelease2 (Ptr<McpttOnNetworkFloorArbitrator> (this));
}

void
McpttOnNetworkFloorArbitrator::ClientRelease (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Taking client release notification.");
  m_state->ClientRelease (Ptr<McpttOnNetworkFloorArbitrator> (this));
}

void
McpttOnNetworkFloorArbitrator::ChangeState (Ptr<McpttOnNetworkFloorArbitratorState>  state)
{
  NS_LOG_FUNCTION (this << state);
  McpttEntityId stateId = state->GetInstanceStateId ();
  McpttEntityId currStateId = GetStateId ();
  NS_LOG_LOGIC ("Moving from state " << *m_state << " to state " << *state << ".");
  if (currStateId != stateId)
    {
      m_state->Unselected (Ptr<McpttOnNetworkFloorArbitrator> (this));
      SetState (state);
      state->Selected (Ptr<McpttOnNetworkFloorArbitrator> (this));

      if (!m_stateChangeCb.IsNull ())
        {
          m_stateChangeCb (currStateId, stateId);
        }
      std::string selected = "N/A"; // Selected call indicator not applicable
      m_stateChangeTrace (GetOwner ()->GetCallMachine ()->GetUserId (), GetOwner ()->GetCallId (), selected, GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

McpttFloorMsgFieldIndic
McpttOnNetworkFloorArbitrator::GetIndicator (void) const
{
  McpttFloorMsgFieldIndic indicator;
  uint8_t callTypeId = GetOwner ()->GetCallMachine ()->GetCallType ().GetType ();

  if (IsDualFloor ())
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::DUAL_FLOOR);
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
McpttOnNetworkFloorArbitrator::GetInstanceTypeId (void) const
{
  return McpttOnNetworkFloorArbitrator::GetTypeId ();
}

uint32_t
McpttOnNetworkFloorArbitrator::GetNParticipants (void) const
{
  return m_participants.size ();
}

Ptr<McpttOnNetworkFloorTowardsParticipant>
McpttOnNetworkFloorArbitrator::GetParticipant (uint32_t index) const
{
  NS_ASSERT_MSG (index < m_participants.size (), "Index out of bounds");
  return m_participants[index];
}

Ptr<McpttOnNetworkFloorTowardsParticipant>
McpttOnNetworkFloorArbitrator::GetParticipantByUserId (uint32_t userId) const
{
  Ptr<McpttOnNetworkFloorTowardsParticipant> participant = 0;
  std::vector<Ptr<McpttOnNetworkFloorTowardsParticipant> >::const_iterator it = m_participants.begin ();

  while (participant == 0 && it != m_participants.end ())
    {
      if ((*it)->GetPeerUserId () == userId)
        {
          participant = *it;
        }

      it++;
    }
  if (participant == 0)
    {
      NS_LOG_WARN ("Participant not found by user ID");
    }
  return participant;
}

Ptr<McpttOnNetworkFloorTowardsParticipant>
McpttOnNetworkFloorArbitrator::GetParticipantBySsrc (const uint32_t ssrc) const
{
  Ptr<McpttOnNetworkFloorTowardsParticipant> participant = 0;
  std::vector<Ptr<McpttOnNetworkFloorTowardsParticipant> >::const_iterator it = m_participants.begin ();
  while (participant == 0 && it != m_participants.end ())
    {
      if ((*it)->GetStoredSsrc () == ssrc)
        {
          participant = *it;
        }

      it++;
    }
  if (participant == 0)
    {
      NS_LOG_WARN ("Participant not found by SSRC");
    }
  return participant;
}
Ptr<McpttOnNetworkFloorTowardsParticipant>
McpttOnNetworkFloorArbitrator::GetOriginatingParticipant (void) const
{
  Ptr<McpttOnNetworkFloorTowardsParticipant> participant = 0;
  std::vector<Ptr<McpttOnNetworkFloorTowardsParticipant> >::const_iterator it = m_participants.begin ();
  while (participant == 0 && it != m_participants.end ())
    {
      if ((*it)->IsOriginator ())
        {
          participant = *it;
        }

      it++;
    }
  if (participant == 0)
    {
      NS_LOG_WARN ("Originating participant not found");
    }
  return participant;
}

McpttEntityId
McpttOnNetworkFloorArbitrator::GetStateId (void) const
{
  McpttEntityId stateId = m_state->GetInstanceStateId ();

  return stateId;
}

bool
McpttOnNetworkFloorArbitrator::IsAudioCutIn (void) const
{
  return m_audioCutIn;
}


bool
McpttOnNetworkFloorArbitrator::IsAckRequired (void) const
{
  return m_ackRequired;
}

bool
McpttOnNetworkFloorArbitrator::IsDualFloor (void) const
{
  return (GetDualControl ()->IsStarted ());
}

bool
McpttOnNetworkFloorArbitrator::IsDualFloorSupported (void) const
{
  return m_dualFloorSupported;
}

bool
McpttOnNetworkFloorArbitrator::IsFloorOccupied (void) const
{
  return m_state->IsFloorOccupied ();
}

bool
McpttOnNetworkFloorArbitrator::IsQueueingSupported (void) const
{
  return m_queueingSupported;
}

bool
McpttOnNetworkFloorArbitrator::IsPreemptive (const McpttFloorMsgRequest& msg) const
{
  bool preemptive = false;

  //if (!GetParticipantBySsrc (msg.GetSsrc ())->IsReceiveOnly ())
  //{
  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::NORMAL_CALL))
    {
      if (GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::NORMAL_CALL))
        {
          if (msg.GetPriority ().GetPriority () > GetStoredPriority ())
            {
              preemptive = true;
            }
        }
      else if (GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::IMMINENT_CALL)
               || GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::EMERGENCY_CALL))
        {
          preemptive = false;
        }
    }
  else if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::IMMINENT_CALL))
    {
      if (GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::NORMAL_CALL))
        {
          preemptive = true;
        }
      else if (GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::IMMINENT_CALL))
        {
          if (msg.GetPriority ().GetPriority () > GetStoredPriority ())
            {
              preemptive = true;
            }
        }
      else if (GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::EMERGENCY_CALL))
        {
          preemptive = false;
        }
    }
  else if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::EMERGENCY_CALL))
    {
      if (GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::NORMAL_CALL)
          || GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::IMMINENT_CALL))
        {
          preemptive = true;
        }
      else
        {
          if (msg.GetPriority ().GetPriority () > GetStoredPriority ())
            {
              preemptive = true;
            }
        }
    }
  else
    {
      NS_FATAL_ERROR ("No call type indicated.");
    }
  //}

  return preemptive;
}

bool
McpttOnNetworkFloorArbitrator::IsStarted (void) const
{
  return GetStateId () != McpttOnNetworkFloorArbitratorStateStartStop::GetStateId ();
}

uint16_t
McpttOnNetworkFloorArbitrator::NextSeqNum (void)
{
  NS_LOG_FUNCTION (this);

  m_seqNum++;

  NS_LOG_LOGIC (" McpttOnNetworkFloorArbitrator (" << this << ") next sequence number = " << m_seqNum << ".");

  return m_seqNum;
}

void
McpttOnNetworkFloorArbitrator::ReceiveFloorRelease (const McpttFloorMsgRelease& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC ("McpttOnNetworkFloorArbitrator (" << this << ") received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorRelease (Ptr<McpttOnNetworkFloorArbitrator> (this), msg);
}

void
McpttOnNetworkFloorArbitrator::ReceiveFloorRequest (const McpttFloorMsgRequest& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC ("McpttOnNetworkFloorArbitrator (" << this << ") received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorRequest (Ptr<McpttOnNetworkFloorArbitrator> (this), msg);
}

void
McpttOnNetworkFloorArbitrator::ReceiveMedia (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC ("McpttOnNetworkFloorArbitrator (" << this << ") received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveMedia (Ptr<McpttOnNetworkFloorArbitrator> (this), msg);
}

void
McpttOnNetworkFloorArbitrator::SendTo (McpttMsg& msg, const uint32_t ssrc)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC ("McpttOnNetworkFloorArbitrator (" << this << ") sending " << msg << " to " << ssrc << ".");

  bool found = false;
  std::vector<Ptr<McpttOnNetworkFloorTowardsParticipant> >::iterator it = m_participants.begin ();

  while (it != m_participants.end () && !found)
    {
      if ((*it)->GetStoredSsrc () == ssrc)
        {
          found = true;
          (*it)->Send (msg);
        }
      it++;
    }
}

void
McpttOnNetworkFloorArbitrator::SendToAll (McpttMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC ("Sending " << msg << " to " << m_participants.size () << " participants");

  std::vector<Ptr<McpttOnNetworkFloorTowardsParticipant> >::iterator it = m_participants.begin ();

  while (it != m_participants.end ())
    {
      (*it)->Send (msg);
      it++;
    }
}

void
McpttOnNetworkFloorArbitrator::SendToAllExcept (McpttMsg& msg, const uint32_t ssrc)
{
  NS_LOG_FUNCTION (this << msg);

  std::vector<Ptr<McpttOnNetworkFloorTowardsParticipant> >::iterator pit = m_participants.begin ();

  NS_LOG_LOGIC ("Sending " << msg << " to " << m_participants.size () << " except " << ssrc);

  while (pit != m_participants.end ())
    {
      if ((*pit)->GetStoredSsrc () != ssrc)
        {
          (*pit)->Send (msg);
        }
      pit++;
    }
}

void
McpttOnNetworkFloorArbitrator::SetDelayT1 (const Time& delayT1)
{
  NS_LOG_FUNCTION (this << delayT1);

  GetT1 ()->SetDelay (delayT1);
}

void
McpttOnNetworkFloorArbitrator::SetDelayT2 (const Time& delayT2)
{
  NS_LOG_FUNCTION (this << delayT2);

  GetT2 ()->SetDelay (delayT2);
}

void
McpttOnNetworkFloorArbitrator::SetDelayT3 (const Time& delayT3)
{
  NS_LOG_FUNCTION (this << delayT3);

  GetT3 ()->SetDelay (delayT3);
}

void
McpttOnNetworkFloorArbitrator::SetDelayT4 (const Time& delayT4)
{
  NS_LOG_FUNCTION (this << delayT4);

  GetT4 ()->SetDelay (delayT4);
}

void
McpttOnNetworkFloorArbitrator::SetDelayT7 (const Time& delayT7)
{
  NS_LOG_FUNCTION (this << delayT7);

  GetT7 ()->SetDelay (delayT7);
}

void
McpttOnNetworkFloorArbitrator::SetDelayT20 (const Time& delayT20)
{
  NS_LOG_FUNCTION (this << delayT20);

  GetT20 ()->SetDelay (delayT20);
}

void
McpttOnNetworkFloorArbitrator::SetLimitC7 (uint32_t limitC7)
{
  NS_LOG_FUNCTION (this << limitC7);

  GetC7 ()->SetLimit (limitC7);
}

void
McpttOnNetworkFloorArbitrator::SetLimitC20 (uint32_t limitC20)
{
  NS_LOG_FUNCTION (this << limitC20);

  GetC20 ()->SetLimit (limitC20);
}

void
McpttOnNetworkFloorArbitrator::Start (void)
{
  NS_LOG_FUNCTION (this);
  // Start arbitrator state machine

  // Start participant state machines
  for (std::vector<Ptr<McpttOnNetworkFloorTowardsParticipant> >::iterator it = m_participants.begin (); it != m_participants.end (); it++)
    {
      (*it)->Start ();
    }
}

void
McpttOnNetworkFloorArbitrator::Stop (void)
{
  NS_LOG_FUNCTION (this);

  for (std::vector<Ptr<McpttOnNetworkFloorTowardsParticipant> >::iterator it = m_participants.begin (); it != m_participants.end (); it++)
    {
      (*it)->Stop ();
    }

  GetDualControl ()->Stop ();

  if (GetT1 ()->IsRunning ())
    {
      GetT1 ()->Stop ();
    }

  if (GetT2 ()->IsRunning ())
    {
      GetT2 ()->Stop ();
    }

  if (GetT3 ()->IsRunning ())
    {
      GetT3 ()->Stop ();
    }

  if (GetT4 ()->IsRunning ())
    {
      GetT4 ()->Stop ();
    }

  if (GetT7 ()->IsRunning ())
    {
      GetT7 ()->Stop ();
    }

  if (GetT20 ()->IsRunning ())
    {
      GetT20 ()->Stop ();
    }

}

void
McpttOnNetworkFloorArbitrator::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_c7 = 0;
  m_c20 = 0;
  m_owner = 0;
  m_state = 0;
  m_t1 = 0;
  m_t2 = 0;
  m_t3 = 0;
  m_t4 = 0;
  m_t7 = 0;
  m_t20 = 0;
  if (m_dualControl)
    {
      m_dualControl->Dispose ();
      m_dualControl = 0;
    }

  for (std::vector<Ptr<McpttOnNetworkFloorTowardsParticipant> >::iterator it = m_participants.begin (); it != m_participants.end (); it++)
    {
      (*it)->Dispose ();
    }
  m_participants.clear ();
  m_stateChangeCb = MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ();
}

void
McpttOnNetworkFloorArbitrator::ExpiryOfT1 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorArbitrator T1 (end-of-media) expired.");

  m_state->ExpiryOfT1 (Ptr<McpttOnNetworkFloorArbitrator> (this));
}

void
McpttOnNetworkFloorArbitrator::ExpiryOfT2 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorArbitrator T2 (stop talking) expired.");

  m_state->ExpiryOfT2 (Ptr<McpttOnNetworkFloorArbitrator> (this));
}

void
McpttOnNetworkFloorArbitrator::ExpiryOfT3 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorArbitrator T3 (stop talking grace) expired.");

  m_state->ExpiryOfT3 (Ptr<McpttOnNetworkFloorArbitrator> (this));
}

void
McpttOnNetworkFloorArbitrator::ExpiryOfT4 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorArbitrator T4 (inactivity) expired.");

  m_state->ExpiryOfT4 (Ptr<McpttOnNetworkFloorArbitrator> (this));
}

void
McpttOnNetworkFloorArbitrator::ExpiryOfT7 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorArbitrator T7 (Floor Idle) expired.");

  m_state->ExpiryOfT7 (Ptr<McpttOnNetworkFloorArbitrator> (this));
}

void
McpttOnNetworkFloorArbitrator::ExpiryOfT20 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorArbitrator T20 (Floor Granted) expired.");

  m_state->ExpiryOfT20 (Ptr<McpttOnNetworkFloorArbitrator> (this));
}

Ptr<McpttCounter>
McpttOnNetworkFloorArbitrator::GetC7 (void) const
{
  return m_c7;
}

Ptr<McpttCounter>
McpttOnNetworkFloorArbitrator::GetC20 (void) const
{
  return m_c20;
}

Ptr<McpttOnNetworkFloorDualControl>
McpttOnNetworkFloorArbitrator::GetDualControl (void) const
{
  return m_dualControl;
}

Ptr<McpttServerCall>
McpttOnNetworkFloorArbitrator::GetOwner (void) const
{
  return m_owner;
}

uint8_t
McpttOnNetworkFloorArbitrator::GetStoredPriority (void) const
{
  return m_storedPriority;
}

Ptr<McpttFloorQueue>
McpttOnNetworkFloorArbitrator::GetQueue (void) const
{
  return m_queue;
}

uint16_t
McpttOnNetworkFloorArbitrator::GetRejectCause (void) const
{
  return m_rejectCause;
}

uint32_t
McpttOnNetworkFloorArbitrator::GetStoredSsrc (void) const
{
  return m_storedSsrc;
}

McpttFloorMsgFieldTrackInfo
McpttOnNetworkFloorArbitrator::GetTrackInfo (void) const
{
  return m_trackInfo;
}

uint32_t
McpttOnNetworkFloorArbitrator::GetTxSsrc (void) const
{
  return m_txSsrc;
}

Ptr<McpttTimer>
McpttOnNetworkFloorArbitrator::GetT1 (void) const
{
  return m_t1;
}

Ptr<McpttTimer>
McpttOnNetworkFloorArbitrator::GetT2 (void) const
{
  return m_t2;
}

Ptr<McpttTimer>
McpttOnNetworkFloorArbitrator::GetT3 (void) const
{
  return m_t3;
}

Ptr<McpttTimer>
McpttOnNetworkFloorArbitrator::GetT4 (void) const
{
  return m_t4;
}

Ptr<McpttTimer>
McpttOnNetworkFloorArbitrator::GetT7 (void) const
{
  return m_t7;
}

Ptr<McpttTimer>
McpttOnNetworkFloorArbitrator::GetT20 (void) const
{
  return m_t20;
}

void
McpttOnNetworkFloorArbitrator::SetDualControl (const Ptr<McpttOnNetworkFloorDualControl> dualControl)
{
  NS_LOG_FUNCTION (this);

  if (dualControl != 0)
    {
      dualControl->SetOwner (this);
    }

  m_dualControl = dualControl;
}

void
McpttOnNetworkFloorArbitrator::SetOwner (Ptr<McpttServerCall> owner)
{
  NS_LOG_FUNCTION (this);

  m_owner = owner;
}

void
McpttOnNetworkFloorArbitrator::SetRejectCause (const uint16_t rejectCause)
{
  NS_LOG_FUNCTION (this);

  m_rejectCause = rejectCause;
}

void
McpttOnNetworkFloorArbitrator::SetState (Ptr<McpttOnNetworkFloorArbitratorState>  state)
{
  NS_LOG_FUNCTION (this << state);

  m_state = state;
}

void
McpttOnNetworkFloorArbitrator::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this);

  m_stateChangeCb = stateChangeCb;
}

void
McpttOnNetworkFloorArbitrator::SetStoredSsrc (const uint32_t storedSsrc)
{
  NS_LOG_FUNCTION (this << storedSsrc);

  m_storedSsrc = storedSsrc;
}

void
McpttOnNetworkFloorArbitrator::SetStoredPriority (uint8_t storedPriority)
{
  NS_LOG_FUNCTION (this << +storedPriority);

  m_storedPriority = storedPriority;
}

void
McpttOnNetworkFloorArbitrator::SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo)
{
  NS_LOG_FUNCTION (this);

  m_trackInfo = trackInfo;
}

/** McpttOnNetworkFloorArbitrator - end **/

} // namespace psc
} // namespace ns3

