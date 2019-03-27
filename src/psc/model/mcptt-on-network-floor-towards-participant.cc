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

#include "mcptt-call.h"
#include "mcptt-counter.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-on-network-floor-arbitrator.h"
#include "mcptt-on-network-floor-towards-participant-state.h"
#include "mcptt-timer.h"

#include "mcptt-on-network-floor-towards-participant.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttOnNetworkFloorTowardsParticipant");

/** McpttOnNetworkFloorTowardsParticipant - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttOnNetworkFloorTowardsParticipant);

TypeId
McpttOnNetworkFloorTowardsParticipant::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttOnNetworkFloorTowardsParticipant")
    .SetParent<Object> ()
    .AddConstructor<McpttOnNetworkFloorTowardsParticipant>()
    .AddAttribute ("McImplicitRequest", "Indicates if the SDP offer included the \"mc_implicit_request\" fmtp attribute",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttOnNetworkFloorTowardsParticipant::m_mcImplicitRequest),
                   MakeBooleanChecker ())
    .AddAttribute ("McQueuing", "Indicates if the SDP offer included the \"mc_queuing\" fmtp attribute",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttOnNetworkFloorTowardsParticipant::m_mcQueuing),
                   MakeBooleanChecker ())
    .AddAttribute ("ReceiveOnly", "Indicates if the associated participant is \"receive only\"",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttOnNetworkFloorTowardsParticipant::m_receiveOnly),
                   MakeBooleanChecker ())
    .AddAttribute ("T8", "The delay to use for timer T8 (Time value)",
                   TimeValue (Seconds (1)),
                   MakeTimeAccessor (&McpttOnNetworkFloorTowardsParticipant::SetDelayT8),
                   MakeTimeChecker ())
    .AddTraceSource ("StateChangeTrace",
                   "The trace for capturing state changes.",
                   MakeTraceSourceAccessor (&McpttOnNetworkFloorTowardsParticipant::m_stateChangeTrace),
                   "ns3::McpttOnNetworkFloorTowardsParticipant::StateChangeTrace")
    ;
  
  return tid;
}

McpttOnNetworkFloorTowardsParticipant::McpttOnNetworkFloorTowardsParticipant (void)
  : Object (),
    m_dualFloor (false),
    m_mcImplicitRequest (false),
    m_mcQueuing (false),
    m_originator (false),
    m_overridden (false),
    m_overriding (false),
    m_owner (0),
    m_revokeMsg (McpttFloorMsgRevoke ()),
    m_rxCb (MakeNullCallback<void, const McpttFloorMsg&> ()),
    m_state (McpttOnNetworkFloorTowardsParticipantStateStartStop::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ()),
    m_t8 (CreateObject<McpttTimer> (McpttEntityId (8, "T8"))),
    m_txCb (MakeNullCallback<void, const McpttFloorMsg&> ())
{
  NS_LOG_FUNCTION (this);

  m_t8->Link (&McpttOnNetworkFloorTowardsParticipant::ExpiryOfT8, this);
}

McpttOnNetworkFloorTowardsParticipant::~McpttOnNetworkFloorTowardsParticipant (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkFloorTowardsParticipant::CallInitiated (void)
{
  NS_LOG_FUNCTION (this);
  
  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant (" << this << ") taking call initiated notification.");

  m_state->CallInitiated (*this);
}

void
McpttOnNetworkFloorTowardsParticipant::CallRelease1 (void)
{
  NS_LOG_FUNCTION (this);
  
  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant (" << this << ") taking call release notification (part I).");

  m_state->CallRelease1 (*this);
}

void
McpttOnNetworkFloorTowardsParticipant::CallRelease2 (void)
{
  NS_LOG_FUNCTION (this);
  
  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant (" << this << ") taking call release notification (part II).");

  m_state->CallRelease2 (*this);
}

void
McpttOnNetworkFloorTowardsParticipant::ClientRelease (void)
{
  NS_LOG_FUNCTION (this);
  
  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant (" << this << ") taking client release notification.");

  m_state->ClientRelease (*this);
}

void
McpttOnNetworkFloorTowardsParticipant::ChangeState (Ptr<McpttOnNetworkFloorTowardsParticipantState>  state)
{
  NS_LOG_FUNCTION (this << state);

  McpttEntityId stateId = state->GetInstanceStateId ();

  McpttEntityId currStateId = GetStateId ();

  if (currStateId != stateId)
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant (" << this << ") moving from state " << *m_state << " to state " << *state << ".");

      m_state->Unselected (*this);
      SetState (state);
      state->Selected (*this);

      m_stateChangeTrace (GetOwner ()->GetTxSsrc (), GetOwner ()->GetOwner ()->GetCallId (), GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

TypeId
McpttOnNetworkFloorTowardsParticipant::GetInstanceTypeId (void) const
{
  return McpttOnNetworkFloorTowardsParticipant::GetTypeId ();
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipant::GetStateId (void) const
{
  McpttEntityId stateId = m_state->GetInstanceStateId ();

  return stateId;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsDualFloor (void) const
{
  NS_LOG_FUNCTION (this);

  return m_dualFloor;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsMcImplicitRequest (void) const
{
  NS_LOG_FUNCTION (this);

  return m_mcImplicitRequest;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsMcQueuing (void) const
{
  NS_LOG_FUNCTION (this);

  return m_mcQueuing;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsOriginator (void) const
{
  NS_LOG_FUNCTION (this);

  return m_originator;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsOverridden (void) const
{
  NS_LOG_FUNCTION (this);

  return m_overridden;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsOverriding (void) const
{
  NS_LOG_FUNCTION (this);

  return m_overriding;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsReceiveOnly (void) const
{
  NS_LOG_FUNCTION (this);

  return m_receiveOnly;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsStarted (void) const
{
  return m_state->GetInstanceStateId () != McpttOnNetworkFloorTowardsParticipant::GetStateId ();
}

void
McpttOnNetworkFloorTowardsParticipant::Receive (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  msg.Visit (*this);
}

void
McpttOnNetworkFloorTowardsParticipant::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  msg.Visit (*this);
}

void
McpttOnNetworkFloorTowardsParticipant::ReceiveFloorQueuePositionRequest (const McpttFloorMsgQueuePositionRequest& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant (" << this << ") received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorQueuePositionRequest (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipant::ReceiveFloorRelease (const McpttFloorMsgRelease& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorRelease (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipant::ReceiveFloorRequest (const McpttFloorMsgRequest& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorRequest (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipant::ReceiveMedia (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveMedia (*this, msg);
}

void
McpttOnNetworkFloorTowardsParticipant::Send (McpttMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  //TODO: Actually send the message
}

void
McpttOnNetworkFloorTowardsParticipant::SendFloorDeny (McpttFloorMsgDeny& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendFloorDeny (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_txCb (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipant::SendFloorGranted (McpttFloorMsgGranted& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendFloorGranted (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_txCb (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipant::SendFloorIdle (McpttFloorMsgIdle& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendFloorIdle (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_txCb (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipant::SendFloorRevoke (McpttFloorMsgRevoke& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendFloorRevoke (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_txCb (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipant::SendFloorTaken (McpttFloorMsgTaken& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendFloorTaken (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      m_txCb (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipant::SendMedia (McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendMedia (*this, msg);

  if (!m_rxCb.IsNull ())
    {
      //m_txCb (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipant::Terminate (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant (" << this << ") taking terminate notification.");

  m_state->Terminate (*this);
}

void
McpttOnNetworkFloorTowardsParticipant::SetDelayT8 (const Time& delayT8)
{
  NS_LOG_FUNCTION (this << delayT8);

  GetT8 ()->SetDelay (delayT8);
}

void
McpttOnNetworkFloorTowardsParticipant::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_owner = 0;
  m_state = 0;
  m_t8 = 0;
}

void
McpttOnNetworkFloorTowardsParticipant::ExpiryOfT8 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttOnNetworkFloorTowardsParticipant T8 expired.");

  m_state->ExpiryOfT8 (*this);
}

McpttOnNetworkFloorArbitrator*
McpttOnNetworkFloorTowardsParticipant::GetOwner (void) const
{
  NS_LOG_FUNCTION (this);

  return m_owner;
}

McpttFloorMsgRevoke
McpttOnNetworkFloorTowardsParticipant::GetRevokeMsg (void) const
{
  NS_LOG_FUNCTION (this);

  return m_revokeMsg;
}

uint32_t
McpttOnNetworkFloorTowardsParticipant::GetStoredSsrc (void) const
{
  NS_LOG_FUNCTION (this);

  return m_storedSsrc;
}

uint8_t
McpttOnNetworkFloorTowardsParticipant::GetStoredPriority (void) const
{
  NS_LOG_FUNCTION (this);

  return m_storedPriority;
}

McpttFloorMsgFieldTrackInfo
McpttOnNetworkFloorTowardsParticipant::GetTrackInfo (void) const
{
  NS_LOG_FUNCTION (this);

  return m_trackInfo;
}

Ptr<McpttTimer>
McpttOnNetworkFloorTowardsParticipant::GetT8 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_t8;
}

void
McpttOnNetworkFloorTowardsParticipant::SetDualFloor (const bool dualFloor)
{
  NS_LOG_FUNCTION (this);

  m_dualFloor = dualFloor;
}

void
McpttOnNetworkFloorTowardsParticipant::SetOriginator (const bool originator)
{
  NS_LOG_FUNCTION (this);

  m_originator = originator;
}

void
McpttOnNetworkFloorTowardsParticipant::SetOverridden (const bool overridden)
{
  NS_LOG_FUNCTION (this);

  m_overridden = overridden;
}

void
McpttOnNetworkFloorTowardsParticipant::SetOverriding (const bool overriding)
{
  NS_LOG_FUNCTION (this);

  m_overriding = overriding;
}

void
McpttOnNetworkFloorTowardsParticipant::SetOwner (McpttOnNetworkFloorArbitrator* const& owner)
{
  NS_LOG_FUNCTION (this);

  m_owner = owner;
}

void
McpttOnNetworkFloorTowardsParticipant::SetRevokeMsg (const McpttFloorMsgRevoke& revokeMsg)
{
  NS_LOG_FUNCTION (this);

  m_revokeMsg = revokeMsg;
}

void
McpttOnNetworkFloorTowardsParticipant::SetRxCb (const Callback<void, const McpttFloorMsg&>  rxCb)
{
  NS_LOG_FUNCTION (this);

  m_rxCb = rxCb;
}

void
McpttOnNetworkFloorTowardsParticipant::SetState (Ptr<McpttOnNetworkFloorTowardsParticipantState>  state)
{
  NS_LOG_FUNCTION (this << state);

  m_state = state;
}

void
McpttOnNetworkFloorTowardsParticipant::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this);

  m_stateChangeCb = stateChangeCb;
}

void
McpttOnNetworkFloorTowardsParticipant::SetStoredSsrc (const uint32_t storedSsrc)
{
  NS_LOG_FUNCTION (this);

  m_storedSsrc = storedSsrc;
}

void
McpttOnNetworkFloorTowardsParticipant::SetStoredPriority (uint8_t storedPriority)
{
  NS_LOG_FUNCTION (this);

  m_storedPriority = storedPriority;
}

void
McpttOnNetworkFloorTowardsParticipant::SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo)
{
  NS_LOG_FUNCTION (this);

  m_trackInfo = trackInfo;
}

void
McpttOnNetworkFloorTowardsParticipant::SetTxCb (const Callback<void, const McpttFloorMsg&>  txCb)
{
  NS_LOG_FUNCTION (this);

  m_txCb = txCb;
}
/** McpttOnNetworkFloorTowardsParticipant - end **/

} // namespace ns3

