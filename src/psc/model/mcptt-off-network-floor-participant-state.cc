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
#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg-field.h"
#include "mcptt-entity-id.h"
#include "mcptt-off-network-floor-participant.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-queue.h"
#include "mcptt-media-msg.h"
#include "mcptt-media-src.h"
#include "mcptt-ptt-app.h"

#include "mcptt-off-network-floor-participant.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttOffNetworkFloorParticipantState");

namespace psc {

/** McpttOffNetworkFloorParticipantState - begin **/
McpttOffNetworkFloorParticipantState::~McpttOffNetworkFloorParticipantState (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOffNetworkFloorParticipantState::AcceptGrant (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring accept grant.");
}

void
McpttOffNetworkFloorParticipantState::ExpiryOfT201 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T201.");
}

void
McpttOffNetworkFloorParticipantState::ExpiryOfT203 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T203.");
}

void
McpttOffNetworkFloorParticipantState::ExpiryOfT204 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T204.");
}

void
McpttOffNetworkFloorParticipantState::ExpiryOfT205 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T205.");
}

void
McpttOffNetworkFloorParticipantState::ExpiryOfT206 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T206.");
}

void
McpttOffNetworkFloorParticipantState::ExpiryOfT207 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T207.");
}

void
McpttOffNetworkFloorParticipantState::ExpiryOfT230 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T230.");
}

void
McpttOffNetworkFloorParticipantState::ExpiryOfT233 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T233.");
}

McpttEntityId
McpttOffNetworkFloorParticipantState::GetInstanceStateId (void) const
{
  return McpttEntityId ();
}

bool
McpttOffNetworkFloorParticipantState::HasFloor (const McpttOffNetworkFloorParticipant& floorMachine) const
{
  return false;
}

void
McpttOffNetworkFloorParticipantState::ReceiveCallRelease (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this);
  Ptr<McpttCall> callOwner = floorMachine.GetCall ();
  Ptr<McpttPttApp> appOwner = callOwner->GetOwner ();

  if (appOwner->IsPushed ())
    {
      appOwner->NotifyReleased ();
    }

  floorMachine.StopTimers ();
  floorMachine.ResetCounters ();

  floorMachine.SetState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ());
}
void
McpttOffNetworkFloorParticipantState::ReceiveFloorDeny (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOffNetworkFloorParticipantState::ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOffNetworkFloorParticipantState::ReceiveFloorQueuePositionRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgQueuePositionRequest& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOffNetworkFloorParticipantState::ReceiveFloorQueuePositionInfo (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgQueuePositionInfo& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOffNetworkFloorParticipantState::ReceiveFloorRelease (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOffNetworkFloorParticipantState::ReceiveFloorRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOffNetworkFloorParticipantState::ReceiveFloorTaken (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOffNetworkFloorParticipantState::ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOffNetworkFloorParticipantState::SendFloorQueuePositionRequest (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring floor queue position request.");
}

void
McpttOffNetworkFloorParticipantState::MediaReady (McpttOffNetworkFloorParticipant& floorMachine, McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_ASSERT_MSG (false, "Should not be sending media here.");
}

void
McpttOffNetworkFloorParticipantState::ReleaseRequest (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring release request.");
}

void
McpttOffNetworkFloorParticipantState::Selected (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);
}

void
McpttOffNetworkFloorParticipantState::Start (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);
}

void
McpttOffNetworkFloorParticipantState::Stop (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  ReceiveCallRelease (floorMachine);
}

void
McpttOffNetworkFloorParticipantState::PttPush (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring push notification.");
}

void
McpttOffNetworkFloorParticipantState::PttRelease (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring release notification.");
}

void
McpttOffNetworkFloorParticipantState::Unselected (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);
}

McpttOffNetworkFloorParticipantState::McpttOffNetworkFloorParticipantState (void)
  : SimpleRefCount<McpttOffNetworkFloorParticipantState> ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttOffNetworkFloorParticipantState - end **/

std::ostream&
operator<< (std::ostream& os, const McpttOffNetworkFloorParticipantState& state)
{
  return os << state.GetInstanceStateId ();
}

/** McpttOffNetworkFloorParticipantStateStartStop - begin **/
Ptr<McpttOffNetworkFloorParticipantStateStartStop>
McpttOffNetworkFloorParticipantStateStartStop::GetInstance (void)
{
  static Ptr<McpttOffNetworkFloorParticipantStateStartStop> instance = Create<McpttOffNetworkFloorParticipantStateStartStop> ();
  return instance;
}

McpttEntityId
McpttOffNetworkFloorParticipantStateStartStop::GetStateId (void)
{
  static McpttEntityId id (0, "'Start-stop'");

  return id;
}

McpttOffNetworkFloorParticipantStateStartStop::McpttOffNetworkFloorParticipantStateStartStop (void)
  : McpttOffNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOffNetworkFloorParticipantStateStartStop::~McpttOffNetworkFloorParticipantStateStartStop (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOffNetworkFloorParticipantStateStartStop::GetInstanceStateId (void) const
{
  return McpttOffNetworkFloorParticipantStateStartStop::GetStateId ();
}

void
McpttOffNetworkFloorParticipantStateStartStop::ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  //Provide floor taken notification.
  uint32_t grantedSsrc = msg.GetGrantedSsrc ().GetSsrc ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();

  floorMachine.SetCurrentSsrc (grantedSsrc);
  t203->Start ();

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
}

void
McpttOffNetworkFloorParticipantStateStartStop::ReceiveFloorTaken (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  //Provide floor taken notification to user.
  uint32_t grantedSsrc = msg.GetSsrc ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();

  floorMachine.SetCurrentSsrc (grantedSsrc);
  t203->Start ();

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
}

void
McpttOffNetworkFloorParticipantStateStartStop::ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  //Provide floor taken notification to user.
  //Request MCPTT client to start rendering received RTP packets.
  uint32_t rxSsrc = msg.GetHeader ().GetSsrc ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();

  floorMachine.SetCurrentSsrc (rxSsrc);
  t203->Restart ();

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
}

void
McpttOffNetworkFloorParticipantStateStartStop::Start (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  McpttCallMsgFieldCallType callType = floorMachine.GetCall ()->GetCallMachine ()->GetCallType ();
  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t userId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();

  uint8_t priority = floorMachine.GetPriority ();
  McpttFloorMsgFieldIndic indicator = floorMachine.GetIndicator ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();

  if (floorMachine.IsOriginator ())
    {
      floorMachine.SetCurrentSsrc (txSsrc);

      McpttFloorMsgGranted grantedMsg (txSsrc);
      grantedMsg.SetUserId (userId);
      grantedMsg.SetPriority (priority);
      grantedMsg.SetIndicator (indicator);

      floorMachine.Send (grantedMsg);

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
    }
  else if (callType.GetType () == McpttCallMsgFieldCallType::BASIC_GROUP)
    {
      t230->Start ();

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
    }
  else if (callType.GetType () == McpttCallMsgFieldCallType::PRIVATE)
    {
      t203->Start ();

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
    }
  else if (indicator.IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
    {
      t203->Start ();

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
    }
  else  //Assume this is a private call since there is no indicator for a private call.
    {
      NS_FATAL_ERROR ("Call type could not be determined.");
    }
}

void
McpttOffNetworkFloorParticipantStateStartStop::PttPush (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t userId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();

  uint8_t priority = floorMachine.GetPriority ();
  McpttFloorMsgFieldIndic indicator = floorMachine.GetIndicator ();
  Ptr<McpttTimer> t201 = floorMachine.GetT201 ();
  Ptr<McpttCounter> c201 = floorMachine.GetC201 ();

  McpttFloorMsgRequest reqMsg (txSsrc);
  reqMsg.SetPriority (priority);
  reqMsg.SetUserId (userId);
  reqMsg.SetIndicator (indicator);

  c201->Reset ();
  t201->Start ();

  floorMachine.Send (reqMsg);

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStatePendReq::GetInstance ());
}
/** McpttOffNetworkFloorParticipantStateStartStop - end **/

/** McpttOffNetworkFloorParticipantStateNoPerm - begin **/
Ptr<McpttOffNetworkFloorParticipantStateNoPerm>
McpttOffNetworkFloorParticipantStateNoPerm::GetInstance (void)
{
  static Ptr<McpttOffNetworkFloorParticipantStateNoPerm> instance = Create<McpttOffNetworkFloorParticipantStateNoPerm> ();

  return instance;
}

McpttEntityId
McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (void)
{
  static McpttEntityId id (1, "'O: has no permission'");

  return id;
}

McpttOffNetworkFloorParticipantStateNoPerm::McpttOffNetworkFloorParticipantStateNoPerm (void)
  : McpttOffNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOffNetworkFloorParticipantStateNoPerm::~McpttOffNetworkFloorParticipantStateNoPerm (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOffNetworkFloorParticipantStateNoPerm::ExpiryOfT203 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  //Provide floor idle notification to user.
  //Request MCPTT client to stop rendering received RTP media packets.
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();

  t230->Start ();

  floorMachine.ClearCurrentSsrc ();

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
}

McpttEntityId
McpttOffNetworkFloorParticipantStateNoPerm::GetInstanceStateId (void) const
{
  return McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ();
}

void
McpttOffNetworkFloorParticipantStateNoPerm::ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  uint32_t grantedSsrc = msg.GetGrantedSsrc ().GetSsrc ();
  McpttFloorMsgFieldIndic rxIndicator = msg.GetIndicator ();

  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  McpttFloorMsgFieldUserId grantedUserIdField = msg.GetUserId ();
  uint32_t grantedUserId = grantedUserIdField.GetUserId ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();

  if (myUserId != grantedUserId)
    {
      if (rxSsrc == floorMachine.GetCurrentSsrc ())
        {
          //Request MCPTT client to stop rendering received RTP media packets.
          t203->Restart ();
          floorMachine.SetCandidateSsrc (grantedSsrc);
          //Provide floor taken notification to the user.
          if (rxIndicator.IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
            {
              //Provide notification to user that this is a broadcast group call.
            }
        }
    }
}

void
McpttOffNetworkFloorParticipantStateNoPerm::ReceiveFloorRelease (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();

  if (floorMachine.GetCurrentSsrc () == rxSsrc
      || floorMachine.GetCandidateSsrc () == rxSsrc)
    {
      //Provide floor idle notification to user.
      //Request MCPTT client to stop rendering received RTP media.
      t203->Stop ();
      t230->Start ();
      floorMachine.ClearCandidateSsrc ();
      floorMachine.ClearCurrentSsrc ();

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
    }
}

void
McpttOffNetworkFloorParticipantStateNoPerm::ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetHeader ().GetSsrc ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();

  if (!floorMachine.HasCurrentSsrc ())
    {
      //Request the MCPTT client to render the received RTP media.
      floorMachine.SetCurrentSsrc (rxSsrc);
      floorMachine.ClearCandidateSsrc ();
      t203->Restart ();
    }
  else if (rxSsrc == floorMachine.GetCurrentSsrc ())
    {
      //Request the MCPTT client to render the received RTP media.
      floorMachine.ClearCandidateSsrc ();
      t203->Restart ();
    }
  else if (rxSsrc == floorMachine.GetCandidateSsrc ())
    {
      //Request the MCPTT client to render the received RTP media.
      floorMachine.SetCurrentSsrc (rxSsrc);
      floorMachine.ClearCandidateSsrc ();
      t203->Restart ();
    }
}

void
McpttOffNetworkFloorParticipantStateNoPerm::Selected (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttPttApp> appOwner = floorMachine.GetCall ()->GetOwner ();

  if (appOwner->IsPushed ())
    {
      appOwner->NotifyReleased ();
    }
}

void
McpttOffNetworkFloorParticipantStateNoPerm::PttPush (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t userId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t201 = floorMachine.GetT201 ();
  Ptr<McpttCounter> c201 = floorMachine.GetC201 ();

  uint8_t priority = floorMachine.GetPriority ();
  McpttFloorMsgFieldIndic indicator = floorMachine.GetIndicator ();

  McpttFloorMsgRequest reqMsg (txSsrc);
  reqMsg.SetPriority (priority);
  reqMsg.SetUserId (userId);
  reqMsg.SetIndicator (indicator);

  c201->Reset ();
  t201->Start ();

  floorMachine.Send (reqMsg);

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStatePendReq::GetInstance ());
}
/** McpttOffNetworkFloorParticipantStateNoPerm - end **/

/** McpttOffNetworkFloorParticipantStateHasPerm - begin **/
Ptr<McpttOffNetworkFloorParticipantStateHasPerm>
McpttOffNetworkFloorParticipantStateHasPerm::GetInstance (void)
{
  static Ptr<McpttOffNetworkFloorParticipantStateHasPerm> instance = Create<McpttOffNetworkFloorParticipantStateHasPerm> ();

  return instance;
}

McpttEntityId
McpttOffNetworkFloorParticipantStateHasPerm::GetStateId (void)
{
  static McpttEntityId id (2, "'O: has permission'");

  return id;
}

McpttOffNetworkFloorParticipantStateHasPerm::McpttOffNetworkFloorParticipantStateHasPerm (void)
  : McpttOffNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOffNetworkFloorParticipantStateHasPerm::~McpttOffNetworkFloorParticipantStateHasPerm (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOffNetworkFloorParticipantStateHasPerm::ExpiryOfT206 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  //Notify the MCPTT user that the transmission time limit is about to be reached.

  Ptr<McpttTimer> t207 = floorMachine.GetT207 ();

  t207->Start ();
}

void
McpttOffNetworkFloorParticipantStateHasPerm::ExpiryOfT207 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();

  McpttFloorMsgFieldIndic indicator = floorMachine.GetIndicator ();
  Ptr<McpttTimer> t205 = floorMachine.GetT205 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttCounter> c205 = floorMachine.GetC205 ();

  if (queue->HasNext ())
    {
      McpttQueuedUserInfo next = queue->Dequeue ();
      uint32_t nextSsrc = next.GetSsrc ();
      McpttFloorMsgFieldQueuedUserId nextUserIdField = next.GetUserId ();
      std::list<McpttQueuedUserInfo> queuedUsers = queue->ViewUsers ();

      McpttFloorMsgGranted grantedMsg (txSsrc);
      grantedMsg.SetUserId (nextUserIdField);
      grantedMsg.SetGrantedSsrc (nextSsrc);
      grantedMsg.UpdateUsers (queuedUsers);
      grantedMsg.SetIndicator (indicator);

      floorMachine.SetCurrentSsrc (nextSsrc);

      c205->Reset ();
      t205->Start ();

      floorMachine.Send (grantedMsg);

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStatePendGrant::GetInstance ());
    }
  else
    {
      McpttFloorMsgRelease releaseMsg (txSsrc);
      releaseMsg.SetUserId (myUserId);
      releaseMsg.SetIndicator (indicator);

      t230->Start ();

      floorMachine.ClearCurrentSsrc ();

      floorMachine.Send (releaseMsg);

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
    }
}

McpttEntityId
McpttOffNetworkFloorParticipantStateHasPerm::GetInstanceStateId (void) const
{
  return McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ();
}

bool
McpttOffNetworkFloorParticipantStateHasPerm::HasFloor (const McpttOffNetworkFloorParticipant& floorMachine) const
{
  return true;
}

void
McpttOffNetworkFloorParticipantStateHasPerm::ReceiveFloorQueuePositionRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgQueuePositionRequest& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();

  uint16_t position;
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  uint32_t msgUserId = msg.GetUserId ().GetUserId ();
  McpttQueuedUserInfo userInfo;

  queue->View (msgUserId, userInfo, position);

  uint32_t queuedUserSsrc = userInfo.GetSsrc ();
  McpttFloorMsgFieldQueuePositionInfo infoField = userInfo.GetInfo ();
  McpttFloorMsgFieldQueuedUserId queuedUserIdField = userInfo.GetUserId ();

  McpttFloorMsgQueuePositionInfo infoMsg (txSsrc);
  infoMsg.SetQueuedUserId (queuedUserIdField);
  infoMsg.SetQueuePositionInfo (infoField);
  infoMsg.SetQueuedSsrc (queuedUserSsrc);
  infoMsg.SetUserId (myUserId);

  floorMachine.Send (infoMsg);
}

void
McpttOffNetworkFloorParticipantStateHasPerm::ReceiveFloorRelease (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();
  McpttFloorMsgFieldUserId msgUserIdField = msg.GetUserId ();

  uint32_t msgUserIdVal = msgUserIdField.GetUserId ();

  if (queue->IsEnabled ())
    {
      queue->Pull (msgUserIdVal);
    }
}

void
McpttOffNetworkFloorParticipantStateHasPerm::ReceiveFloorRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  bool preemptive = false;
  uint32_t rxSsrc = msg.GetSsrc ();
  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  Ptr<McpttTimer> t205 = floorMachine.GetT205 ();
  Ptr<McpttTimer> t206 = floorMachine.GetT206 ();
  Ptr<McpttTimer> t207 = floorMachine.GetT207 ();
  Ptr<McpttCounter> c205 = floorMachine.GetC205 ();
  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();
  McpttFloorMsgFieldIndic myIndicator = floorMachine.GetIndicator ();

  McpttFloorMsgFieldPriority priorityField = msg.GetPriority ();
  McpttFloorMsgFieldUserId userIdField = msg.GetUserId ();
  McpttFloorMsgFieldIndic indicField = msg.GetIndicator ();

  uint8_t currPriority = floorMachine.GetPriority ();
  uint8_t msgPriority = priorityField.GetPriority ();


  if (indicField.IsIndicated (McpttFloorMsgFieldIndic::NORMAL_CALL))
    {
      if (myIndicator.IsIndicated (McpttFloorMsgFieldIndic::NORMAL_CALL))
        {
          if (msgPriority > currPriority)
            {
              preemptive = true;
            }
        }
      else if (myIndicator.IsIndicated (McpttFloorMsgFieldIndic::IMMINENT_CALL)
               || myIndicator.IsIndicated (McpttFloorMsgFieldIndic::EMERGENCY_CALL))
        {
          preemptive = false;
        }
    }
  else if (indicField.IsIndicated (McpttFloorMsgFieldIndic::IMMINENT_CALL))
    {
      if (myIndicator.IsIndicated (McpttFloorMsgFieldIndic::NORMAL_CALL))
        {
          preemptive = true;
        }
      else if (myIndicator.IsIndicated (McpttFloorMsgFieldIndic::IMMINENT_CALL))
        {
          if (msgPriority > currPriority)
            {
              preemptive = true;
            }
        }
      else if (myIndicator.IsIndicated (McpttFloorMsgFieldIndic::EMERGENCY_CALL))
        {
          preemptive = false;
        }
    }
  else if (indicField.IsIndicated (McpttFloorMsgFieldIndic::EMERGENCY_CALL))
    {
      if (myIndicator.IsIndicated (McpttFloorMsgFieldIndic::NORMAL_CALL)
          || myIndicator.IsIndicated (McpttFloorMsgFieldIndic::IMMINENT_CALL))
        {
          preemptive = true;
        }
      else
        {
          if (msgPriority > currPriority)
            {
              preemptive = true;
            }
        }
    }
  else
    {
      NS_FATAL_ERROR ("No call type indicated.");
    }

  if (preemptive == true)
    {
      //Request MCPTT client to stop sending RTP media packets.
      McpttFloorMsgGranted grantedMsg (txSsrc);
      grantedMsg.SetPriority (priorityField);
      grantedMsg.SetGrantedSsrc (rxSsrc);
      grantedMsg.SetUserId (userIdField);

      if (t206->IsRunning ())
        {
          t206->Stop ();
        }

      if (t207->IsRunning ())
        {
          t207->Stop ();
        }

      c205->Reset ();
      t205->Start ();

      if (queue->IsEnabled ())
        {
          std::list<McpttQueuedUserInfo> queuedUsers = queue->ViewUsers ();
          grantedMsg.UpdateUsers (queuedUsers);
        }

      floorMachine.SetCurrentSsrc (rxSsrc);

      floorMachine.Send (grantedMsg);

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStatePendGrant::GetInstance ());
    }
  else if (!queue->IsEnabled ()
           || !indicField.IsIndicated (McpttFloorMsgFieldIndic::QUEUING_SUPP))
    {
      McpttFloorMsgDeny denyMsg (txSsrc);
      denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_1));
      denyMsg.SetUserId (userIdField);

      floorMachine.Send (denyMsg);
    }
  //TODO: Not in standard - when a floor request is received for a user that is already queued, let them know that they are queued.
  else if (queue->Contains (userIdField.GetUserId ()))
    {
      uint16_t position;
      uint32_t msgUserId = msg.GetUserId ().GetUserId ();
      McpttQueuedUserInfo userInfo;

      queue->View (msgUserId, userInfo, position);

      uint32_t queuedUserSsrc = userInfo.GetSsrc ();
      McpttFloorMsgFieldQueuePositionInfo infoField = userInfo.GetInfo ();
      McpttFloorMsgFieldQueuedUserId queuedUserIdField = userInfo.GetUserId ();

      McpttFloorMsgQueuePositionInfo infoMsg (txSsrc);
      infoMsg.SetQueuedUserId (queuedUserIdField);
      infoMsg.SetQueuePositionInfo (infoField);
      infoMsg.SetQueuedSsrc (queuedUserSsrc);
      infoMsg.SetUserId (msgUserId);

      floorMachine.Send (infoMsg);
    }
  else if (queue->IsAtCapacity ())
    {
      McpttFloorMsgDeny denyMsg (txSsrc);
      denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_7));
      denyMsg.SetUserId (userIdField);

      floorMachine.Send (denyMsg);
    }
  else //Add the user to the queue
    {
      uint8_t position = queue->GetCount ();
      uint32_t queuedUserId = userIdField.GetUserId ();
      uint8_t sentPriority = priorityField.GetPriority ();
      McpttFloorMsgFieldQueuePositionInfo queueInfo (position, sentPriority);
      McpttFloorMsgFieldQueuedUserId queuedUserIdField (queuedUserId);

      McpttQueuedUserInfo userInfo (rxSsrc, queuedUserId, queueInfo);
      queue->Enqueue (userInfo);

      McpttFloorMsgQueuePositionInfo queueInfoMsg (txSsrc);
      queueInfoMsg.SetUserId (userIdField);
      queueInfoMsg.SetQueuedSsrc (rxSsrc);
      queueInfoMsg.SetQueuedUserId (queuedUserIdField);
      queueInfoMsg.SetQueuePositionInfo (queueInfo);

      floorMachine.Send (queueInfoMsg);
    }
}

void
McpttOffNetworkFloorParticipantStateHasPerm::MediaReady (McpttOffNetworkFloorParticipant& floorMachine, McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  Ptr<McpttTimer> t206 = floorMachine.GetT206 ();
  Ptr<McpttTimer> t207 = floorMachine.GetT207 ();

  //Request MCPTT client to start sending RTP media packets.

  if (!t206->IsRunning ()
      && !t207->IsRunning ())
    {
      t206->Start ();
    }

  McpttRtpHeader header = msg.GetHeader ();
  header.SetSsrc (txSsrc);
  msg.SetHeader (header);
}

void
McpttOffNetworkFloorParticipantStateHasPerm::Selected (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttCall> callOwner = floorMachine.GetCall ();
  Ptr<McpttPttApp> appOwner = callOwner->GetOwner ();
  Ptr<McpttMediaSrc> mediaSrc = appOwner->GetMediaSrc ();

  if (!appOwner->IsPushed ())
    {
      appOwner->NotifyPushed ();
    }

  if (floorMachine.ShouldGenMedia ()
      && !mediaSrc->IsMakingReq ())
    {
      mediaSrc->StartMakingReq ();
    }
}

void
McpttOffNetworkFloorParticipantStateHasPerm::PttRelease (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttTimer> t205 = floorMachine.GetT205 ();
  Ptr<McpttTimer> t206 = floorMachine.GetT206 ();
  Ptr<McpttTimer> t207 = floorMachine.GetT207 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttCounter> c205 = floorMachine.GetC205 ();
  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  McpttFloorMsgFieldIndic indicator = floorMachine.GetIndicator ();

  if (t206->IsRunning ())
    {
      t206->Stop ();
    }

  if (t207->IsRunning ())
    {
      t207->Stop ();
    }

  if (queue->HasNext ())
    {
      McpttQueuedUserInfo next = queue->Dequeue ();
      uint32_t nextSsrc = next.GetSsrc ();
      McpttFloorMsgFieldQueuedUserId nextUserIdField = next.GetUserId ();
      std::list<McpttQueuedUserInfo> queuedUsers = queue->ViewUsers ();

      McpttFloorMsgGranted grantedMsg (txSsrc);
      grantedMsg.SetUserId (nextUserIdField);
      grantedMsg.SetGrantedSsrc (nextSsrc);
      grantedMsg.UpdateUsers (queuedUsers);
      grantedMsg.SetIndicator (indicator);

      floorMachine.SetCurrentSsrc (nextSsrc);

      c205->Reset ();
      t205->Start ();

      floorMachine.Send (grantedMsg);

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStatePendGrant::GetInstance ());
    }
  else
    {
      McpttFloorMsgRelease releaseMsg (txSsrc);
      releaseMsg.SetUserId (McpttFloorMsgFieldUserId (myUserId));
      releaseMsg.SetIndicator (indicator);

      t230->Start ();

      floorMachine.ClearCurrentSsrc ();

      floorMachine.Send (releaseMsg);

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
    }
}

void
McpttOffNetworkFloorParticipantStateHasPerm::Unselected (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttPttApp> appOwner = floorMachine.GetCall ()->GetOwner ();
  Ptr<McpttMediaSrc> mediaSrc = appOwner->GetMediaSrc ();

  if (appOwner->IsPushed ())
    {
      appOwner->NotifyReleased ();
    }

  if (mediaSrc->IsMakingReq ())
    {
      mediaSrc->StopMakingReq ();
    }
}
/** McpttOffNetworkFloorParticipantStateHasPerm - end **/

/** McpttOffNetworkFloorParticipantStatePendGrant - begin **/
Ptr<McpttOffNetworkFloorParticipantStatePendGrant>
McpttOffNetworkFloorParticipantStatePendGrant::GetInstance (void)
{
  static Ptr<McpttOffNetworkFloorParticipantStatePendGrant> instance = Create<McpttOffNetworkFloorParticipantStatePendGrant> ();

  return instance;
}

McpttEntityId
McpttOffNetworkFloorParticipantStatePendGrant::GetStateId (void)
{
  static McpttEntityId id (3, "'O: pending granted'");

  return id;
}

McpttOffNetworkFloorParticipantStatePendGrant::McpttOffNetworkFloorParticipantStatePendGrant (void)
  : McpttOffNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOffNetworkFloorParticipantStatePendGrant::~McpttOffNetworkFloorParticipantStatePendGrant (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOffNetworkFloorParticipantStatePendGrant::ExpiryOfT205 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttTimer> t205 = floorMachine.GetT205 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttTimer> t233 = floorMachine.GetT233 ();
  Ptr<McpttCounter> c205 = floorMachine.GetC205 ();
  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();

  if (c205->IsLimitReached ())
    {
      if (queue->HasNext ())
        {
          c205->Reset ();
          t233->Start ();
        }
      else
        {
          c205->Reset ();
          t230->Start ();

          floorMachine.ClearCurrentSsrc ();

          floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
        }
    }
  else
    {
      McpttFloorMsgGranted grantedMsg = floorMachine.GetLastGrantMsg ();

      c205->Increment ();
      t205->Restart ();

      floorMachine.Send (grantedMsg);
    }
}

void
McpttOffNetworkFloorParticipantStatePendGrant::ExpiryOfT233 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t205 = floorMachine.GetT205 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttCounter> c205 = floorMachine.GetC205 ();
  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();
  McpttFloorMsgFieldIndic indicator = floorMachine.GetIndicator ();

  if (queue->HasNext ())
    {
      McpttQueuedUserInfo next = queue->Dequeue ();
      uint32_t nextSsrc = next.GetSsrc ();
      McpttFloorMsgFieldQueuedUserId nextUserIdField = next.GetUserId ();
      std::list<McpttQueuedUserInfo> queuedUsers = queue->ViewUsers ();

      McpttFloorMsgGranted grantedMsg (txSsrc);
      grantedMsg.SetUserId (nextUserIdField);
      grantedMsg.SetGrantedSsrc (nextSsrc);
      grantedMsg.UpdateUsers (queuedUsers);
      grantedMsg.SetIndicator (indicator);

      floorMachine.SetCurrentSsrc (nextSsrc);

      c205->Reset ();
      t205->Start ();

      floorMachine.Send (grantedMsg);
    }
  else
    {
      McpttFloorMsgRelease releaseMsg (txSsrc);
      releaseMsg.SetUserId (myUserId);

      t230->Start ();

      floorMachine.Send (releaseMsg);

      floorMachine.ClearCurrentSsrc ();

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
    }
}

McpttEntityId
McpttOffNetworkFloorParticipantStatePendGrant::GetInstanceStateId (void) const
{
  return McpttOffNetworkFloorParticipantStatePendGrant::GetStateId ();
}

bool
McpttOffNetworkFloorParticipantStatePendGrant::HasFloor (const McpttOffNetworkFloorParticipant& floorMachine) const
{
  return true;
}

void
McpttOffNetworkFloorParticipantStatePendGrant::ReceiveFloorRelease (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();
  McpttFloorMsgFieldUserId msgUserIdField = msg.GetUserId ();

  uint32_t msgUserIdVal = msgUserIdField.GetUserId ();

  if (queue->IsEnabled ())
    {
      queue->Pull (msgUserIdVal);
    }
}

void
McpttOffNetworkFloorParticipantStatePendGrant::ReceiveFloorRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  McpttFloorMsgFieldUserId userIdField = msg.GetUserId ();

  uint32_t txSsrc = floorMachine.GetTxSsrc ();

  if (floorMachine.GetCurrentSsrc () != msg.GetSsrc ())
    {
      McpttFloorMsgDeny denyMsg (txSsrc);
      denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause::CAUSE_1);
      denyMsg.SetUserId (userIdField);

      floorMachine.Send (denyMsg);
    }
}

void
McpttOffNetworkFloorParticipantStatePendGrant::ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  Ptr<McpttTimer> t205 = floorMachine.GetT205 ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttTimer> t233 = floorMachine.GetT233 ();

  if (t205->IsRunning ())
    {
      t205->Stop ();
    }

  if (t233->IsRunning ())
    {
      t233->Stop ();
    }

  t203->Start ();

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
}

void
McpttOffNetworkFloorParticipantStatePendGrant::Unselected (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();

  //TODO: Not in standard - clear the local queue of floor requests when leaving the 'O: pending granted' state.
  if (queue->IsEnabled ())
    {
      queue->Clear ();
    }
}
/** McpttOffNetworkFloorParticipantStatePendGrant - end **/

/** McpttOffNetworkFloorParticipantStatePendReq - begin **/
Ptr<McpttOffNetworkFloorParticipantStatePendReq>
McpttOffNetworkFloorParticipantStatePendReq::GetInstance (void)
{
  static Ptr<McpttOffNetworkFloorParticipantStatePendReq> instance = Create<McpttOffNetworkFloorParticipantStatePendReq> ();

  return instance;
}

McpttEntityId
McpttOffNetworkFloorParticipantStatePendReq::GetStateId (void)
{
  static McpttEntityId id (4, "'O: pending request'");

  return id;
}

McpttOffNetworkFloorParticipantStatePendReq::McpttOffNetworkFloorParticipantStatePendReq (void)
  : McpttOffNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOffNetworkFloorParticipantStatePendReq::~McpttOffNetworkFloorParticipantStatePendReq (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOffNetworkFloorParticipantStatePendReq::ExpiryOfT201 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t201 = floorMachine.GetT201 ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttCounter> c201 = floorMachine.GetC201 ();
  McpttFloorMsgFieldIndic indicator = floorMachine.GetIndicator ();

  uint8_t priority = floorMachine.GetPriority ();

  if (c201->IsLimitReached ())
    {
      floorMachine.SetCurrentSsrc (txSsrc);

      McpttFloorMsgTaken takenMsg (txSsrc);
      takenMsg.SetUserId (myUserId);
      takenMsg.SetIndicator (indicator);

      if (t203->IsRunning ())
        {
          t203->Stop ();
        }

      floorMachine.Send (takenMsg);

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
    }
  else
    {
      McpttFloorMsgRequest reqMsg (txSsrc);
      reqMsg.SetPriority (priority);
      reqMsg.SetUserId (myUserId);
      reqMsg.SetIndicator (indicator);

      t201->Restart ();
      c201->Increment ();

      floorMachine.Send (reqMsg);
    }
}

McpttEntityId
McpttOffNetworkFloorParticipantStatePendReq::GetInstanceStateId (void) const
{
  return McpttOffNetworkFloorParticipantStatePendReq::GetStateId ();
}

void
McpttOffNetworkFloorParticipantStatePendReq::ReceiveFloorDeny (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t201 = floorMachine.GetT201 ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();

  uint32_t rxSsrc = msg.GetSsrc ();
  McpttFloorMsgFieldUserId userIdField = msg.GetUserId ();
  uint32_t rxUserId = userIdField.GetUserId ();

  uint32_t currentSsrc = floorMachine.GetCurrentSsrc ();
  uint32_t candidateSsrc = floorMachine.GetCandidateSsrc ();

  if (myUserId == rxUserId)
    {
      if (floorMachine.HasCurrentSsrc ())
        {
          if (currentSsrc == rxSsrc)
            {
              t201->Stop ();
              //Provide floor deny notification to user.
              t203->Restart ();
              //Display floor deny reason to the user.
              floorMachine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_DENY);
              floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
            }
        }
      else if (floorMachine.HasCandidateSsrc ())
        {
          if (candidateSsrc == rxSsrc)
            {
              t201->Stop ();
              floorMachine.SetCurrentSsrc (rxSsrc);
              floorMachine.ClearCandidateSsrc ();
              t203->Restart ();
              //Provide floor deny notification to user.
              //Display floor deny reason to the user.
              floorMachine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_DENY);
              floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
            }
        }
      else
        {
          t201->Stop ();
          t203->Restart ();

          floorMachine.SetCurrentSsrc (rxSsrc);
          floorMachine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_DENY);
          floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
        }
    }
}

void
McpttOffNetworkFloorParticipantStatePendReq::ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  uint32_t grantedSsrc = msg.GetGrantedSsrc ().GetSsrc ();
  McpttFloorMsgFieldUserId userIdField = msg.GetUserId ();

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t201 = floorMachine.GetT201 ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttCounter> c201 = floorMachine.GetC201 ();
  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();
  uint32_t rxUserId = userIdField.GetUserId ();
  uint32_t currentSsrc = floorMachine.GetCurrentSsrc ();
  uint32_t candidateSsrc = floorMachine.GetCandidateSsrc ();
  McpttFloorMsgFieldIndic rxIndicator = msg.GetIndicator ();

  if (myUserId == rxUserId)
    {
      if (floorMachine.HasCurrentSsrc ())
        {
          if (currentSsrc == rxSsrc)
            {
              floorMachine.SetCurrentSsrc (txSsrc);

              t203->Stop ();
              t201->Stop ();

              //Provide a floor granted notification to the MCPTT user.

              if (rxIndicator.IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
                {
                  //Provide a notification to the user indicating the type of call.
                }

              floorMachine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_GRANTED);
              floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
            }
        }
      else if (floorMachine.HasCandidateSsrc ())
        {
          if (candidateSsrc == rxSsrc)
            {
              floorMachine.SetCurrentSsrc (txSsrc);

              t203->Stop ();
              t201->Stop ();

              //Provide a floor granted notification to the MCPTT user.

              floorMachine.ClearCandidateSsrc ();

              if (rxIndicator.IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
                {
                  //Provide a notification to the user indicating the type of call.
                }

              floorMachine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_GRANTED);
              floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
            }
        }
      else
        {
          floorMachine.SetCurrentSsrc (txSsrc);

          if (t203->IsRunning ())
            {
              t203->Stop ();
            }

          t201->Stop ();

          //Provide a floor granted notification to the MCPTT user.

          if (rxIndicator.IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
            {
              //Provide a notification to the user indicating the type of call.
            }

          floorMachine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_GRANTED);
          floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
        }
      //TODO: Not in standard - update local queue of floor requests when a 'Floor Granted' message is received.
      if (queue->IsEnabled ())
        {
          std::list<McpttQueuedUserInfo> users = msg.GetUsers ();
          queue->UpdateUsers (users);
        }
    }
  else
    {
      if (floorMachine.HasCurrentSsrc ())
        {
          if (currentSsrc == rxSsrc)
            {
              floorMachine.SetCandidateSsrc (grantedSsrc);

              c201->Reset ();
              t201->Restart ();
              t203->Restart ();
              //Provide floor taken notification to the user.

              if (rxIndicator.IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
                {
                  //Provide a notification to the user indicating the type of call.
                }
            }
        }
      else
        {
          floorMachine.SetCandidateSsrc (grantedSsrc);

          c201->Reset ();
          t201->Restart ();
          t203->Restart ();

          //Provide floor taken notification to the user.

          if (rxIndicator.IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
            {
              //Provide a notification to the user indicating the type of call.
            }
        }
    }
}

void
McpttOffNetworkFloorParticipantStatePendReq::ReceiveFloorQueuePositionInfo (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgQueuePositionInfo& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  McpttFloorMsgFieldQueuedUserId userIdField = msg.GetQueuedUserId ();

  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t201 = floorMachine.GetT201 ();
  uint32_t rxUserId = userIdField.GetUserId ();
  uint32_t currentSsrc = floorMachine.GetCurrentSsrc ();
  uint32_t candidateSsrc = floorMachine.GetCandidateSsrc ();

  if (myUserId == rxUserId)
    {
      if (floorMachine.HasCurrentSsrc ())
        {
          if (currentSsrc == rxSsrc)
            {
              //Update the queue status.

              //Notify the MCPTT user about the queue position

              t201->Stop ();

              floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateQueued::GetInstance ());
            }
        }
      else if (floorMachine.HasCandidateSsrc ())
        {
          if (candidateSsrc == rxSsrc)
            {
              //Update the queue status.

              floorMachine.SetCurrentSsrc (rxSsrc);
              floorMachine.ClearCandidateSsrc ();

              //Notify the MCPTT user about the queue position

              t201->Stop ();

              floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateQueued::GetInstance ());
            }
        }
      else
        {
          //Update the queue status.

          floorMachine.SetCurrentSsrc (rxSsrc);

          //Notify the MCPTT user about the queue position

          t201->Stop ();

          floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateQueued::GetInstance ());
        }
    }
}

void
McpttOffNetworkFloorParticipantStatePendReq::ReceiveFloorRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  McpttFloorMsgFieldPriority rxMsgFieldPriority = msg.GetPriority ();

  uint32_t mySsrc = floorMachine.GetTxSsrc ();
  Ptr<McpttTimer> t201 = floorMachine.GetT201 ();
  Ptr<McpttCounter> c201 = floorMachine.GetC201 ();

  uint8_t myPriority = floorMachine.GetPriority ();
  uint8_t rxPriority = rxMsgFieldPriority.GetPriority ();

  if (rxPriority > myPriority) //i.e. they have a higher precessedence.
    {
      c201->Reset ();
      t201->Restart ();
    }
  else if (rxPriority == myPriority
           && rxSsrc > mySsrc) //i.e. they have a higher precessedence.
    {
      c201->Reset ();
      t201->Restart ();
    }
}

void
McpttOffNetworkFloorParticipantStatePendReq::ReceiveFloorTaken (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();

  Ptr<McpttTimer> t201 = floorMachine.GetT201 ();
  Ptr<McpttCounter> c201 = floorMachine.GetC201 ();

  floorMachine.SetCurrentSsrc (rxSsrc);

  c201->Reset ();
  t201->Restart ();
}

void
McpttOffNetworkFloorParticipantStatePendReq::ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetHeader ().GetSsrc ();

  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttCounter> c201 = floorMachine.GetC201 ();
  uint32_t currentSsrc = floorMachine.GetCurrentSsrc ();
  uint32_t candidateSsrc = floorMachine.GetCandidateSsrc ();

  if (floorMachine.HasCurrentSsrc ())
    {
      if (currentSsrc == rxSsrc)
        {
          c201->Reset ();
          t203->Restart ();
        }
    }
  else if (floorMachine.HasCandidateSsrc ())
    {
      if (candidateSsrc == rxSsrc)
        {
          c201->Reset ();

          floorMachine.SetCurrentSsrc (rxSsrc);
          floorMachine.ClearCandidateSsrc ();

          t203->Restart ();
        }
    }
  else
    {
      c201->Reset ();

      floorMachine.SetCurrentSsrc (rxSsrc);

      t203->Restart ();
    }
}

void
McpttOffNetworkFloorParticipantStatePendReq::PttRelease (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t201 = floorMachine.GetT201 ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  McpttFloorMsgFieldIndic indicator = floorMachine.GetIndicator ();

  McpttFloorMsgRelease releaseMsg (txSsrc);
  releaseMsg.SetUserId (myUserId);
  releaseMsg.SetIndicator (indicator);

  if (t203->IsRunning ())
    {
      t203->Stop ();
    }

  t201->Stop ();
  t230->Start ();

  floorMachine.ClearCurrentSsrc ();

  floorMachine.Send (releaseMsg);
  floorMachine.ReportEvent (McpttFloorParticipant::PTT_BUTTON_RELEASED);
  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
}
/** McpttOffNetworkFloorParticipantStatePendReq - end **/

/** McpttOffNetworkFloorParticipantStateQueued - begin **/
Ptr<McpttOffNetworkFloorParticipantStateQueued>
McpttOffNetworkFloorParticipantStateQueued::GetInstance (void)
{
  static Ptr<McpttOffNetworkFloorParticipantStateQueued> instance = Create<McpttOffNetworkFloorParticipantStateQueued> ();

  return instance;
}

McpttEntityId
McpttOffNetworkFloorParticipantStateQueued::GetStateId (void)
{
  static McpttEntityId id (5, "'O: queued'");

  return id;
}

McpttOffNetworkFloorParticipantStateQueued::McpttOffNetworkFloorParticipantStateQueued (void)
  : McpttOffNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOffNetworkFloorParticipantStateQueued::~McpttOffNetworkFloorParticipantStateQueued (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOffNetworkFloorParticipantStateQueued::AcceptGrant (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttTimer> t233 = floorMachine.GetT233 ();
  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();
  McpttFloorMsgGranted grantedMsg = floorMachine.GetLastGrantMsg ();

  t203->Stop ();
  t233->Stop ();

  floorMachine.SetCurrentSsrc (txSsrc);

  //TODO: Not in standard - update local queue of floor requests upon accepting the grant.
  if (queue->IsEnabled ())
    {
      std::list<McpttQueuedUserInfo> users = grantedMsg.GetUsers ();
      queue->UpdateUsers (users);
    }

  floorMachine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_GRANTED);

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
}

void
McpttOffNetworkFloorParticipantStateQueued::ExpiryOfT203 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t201 = floorMachine.GetT201 ();
  Ptr<McpttCounter> c201 = floorMachine.GetC201 ();
  uint8_t priority = floorMachine.GetPriority ();
  McpttFloorMsgFieldIndic indicator = floorMachine.GetIndicator ();

  McpttFloorMsgRequest requestMsg (txSsrc);
  requestMsg.SetPriority (priority);
  requestMsg.SetUserId (myUserId);
  requestMsg.SetIndicator (indicator);

  c201->Reset ();
  t201->Start ();

  floorMachine.ClearCurrentSsrc ();

  floorMachine.Send (requestMsg);

  floorMachine.ReportEvent (McpttFloorParticipant::TIMER_T203_EXPIRED);

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStatePendReq::GetInstance ());
}

void
McpttOffNetworkFloorParticipantStateQueued::ExpiryOfT204 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t204 = floorMachine.GetT204 ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttCounter> c204 = floorMachine.GetC204 ();

  if (c204->IsLimitReached ())
    {
      t203->Stop ();

      c204->Reset ();
      t230->Start ();

      floorMachine.ClearCurrentSsrc ();

      floorMachine.ReportEvent (McpttFloorParticipant::TIMER_T204_EXPIRED_N_TIMES);

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
    }
  else
    {
      McpttFloorMsgQueuePositionRequest reqMsg (txSsrc);
      reqMsg.SetUserId (myUserId);

      c204->Increment ();
      t204->Start ();

      floorMachine.Send (reqMsg);
    }
}

void
McpttOffNetworkFloorParticipantStateQueued::ExpiryOfT233 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();

  t203->Stop ();
  t230->Start ();

  floorMachine.ReportEvent (McpttFloorParticipant::TIMER_T233_EXPIRED);

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
}

McpttEntityId
McpttOffNetworkFloorParticipantStateQueued::GetInstanceStateId (void) const
{
  return McpttOffNetworkFloorParticipantStateQueued::GetStateId ();
}

void
McpttOffNetworkFloorParticipantStateQueued::ReceiveFloorDeny (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  McpttFloorMsgFieldUserId rxIdField = msg.GetUserId ();
  uint32_t rxUserId = rxIdField.GetUserId ();

  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t233 = floorMachine.GetT233 ();
  uint32_t currentSsrc = floorMachine.GetCurrentSsrc ();
  uint32_t candidateSsrc = floorMachine.GetCandidateSsrc ();

  if (myUserId == rxUserId)
    {
      if (currentSsrc == rxSsrc)
        {
          if (t233->IsRunning ())
            {
              t233->Stop ();
            }

          //Provide floor deny notification to the user.
          //Display floor deny reason.

          floorMachine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_DENY);

          floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
        }
      else if (candidateSsrc == rxSsrc)
        {
          if (t233->IsRunning ())
            {
              t233->Stop ();
            }

          floorMachine.SetCurrentSsrc (rxSsrc);
          floorMachine.ClearCandidateSsrc ();

          //Provide floor deny notification to the user.
          //Display floor deny reason.

          floorMachine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_DENY);

          floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
        }
    }
}

void
McpttOffNetworkFloorParticipantStateQueued::ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  uint32_t grantedSsrc = msg.GetGrantedSsrc ().GetSsrc ();
  McpttFloorMsgFieldUserId rxIdField = msg.GetUserId ();
  uint32_t rxUserId = rxIdField.GetUserId ();
  McpttFloorMsgFieldIndic rxIndicator = msg.GetIndicator ();

  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttTimer> t233 = floorMachine.GetT233 ();
  uint32_t currentSsrc = floorMachine.GetCurrentSsrc ();
  uint32_t candidateSsrc = floorMachine.GetCandidateSsrc ();

  if (myUserId == rxUserId)
    {
      if (floorMachine.HasCurrentSsrc ())
        {
          if (currentSsrc == rxSsrc)
            {
              if (!t233->IsRunning ())
                {
                  t233->Start ();
                }

              //Notify user about the grant.

              if (rxIndicator.IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
                {
                  //Provide broadcast group call notification if indicated.
                }

              floorMachine.SetLastGrantMsg (msg);
              Simulator::Schedule (Seconds (0), &McpttOffNetworkFloorParticipant::NotifyFloorGranted, &floorMachine);
            }
        }
      else if (floorMachine.HasCandidateSsrc ())
        {
          if (candidateSsrc == rxSsrc)
            {
              if (!t233->IsRunning ())
                {
                  t233->Start ();
                }

              //Notify user about the grant.

              floorMachine.SetCurrentSsrc (rxSsrc);
              floorMachine.ClearCandidateSsrc ();

              if (rxIndicator.IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
                {
                  //Provide broadcast group call notification if indicated.
                }

              floorMachine.SetLastGrantMsg (msg);
              Simulator::Schedule (Seconds (0), &McpttOffNetworkFloorParticipant::NotifyFloorGranted, &floorMachine);
            }
        }
    }
  else
    {
      if (floorMachine.HasCurrentSsrc ())
        {
          if (currentSsrc == rxSsrc)
            {
              t203->Restart ();

              floorMachine.SetCandidateSsrc (grantedSsrc);
            }
        }
    }
}

void
McpttOffNetworkFloorParticipantStateQueued::ReceiveFloorQueuePositionInfo (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgQueuePositionInfo& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  McpttFloorMsgFieldUserId rxIdField = msg.GetQueuedUserId ();
  uint32_t rxUserId = rxIdField.GetUserId ();

  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t204 = floorMachine.GetT204 ();
  uint32_t currenSsrc = floorMachine.GetCurrentSsrc ();
  uint32_t candidateSsrc = floorMachine.GetCandidateSsrc ();

  if (myUserId == rxUserId)
    {
      if (currenSsrc == rxSsrc)
        {
          //Update the queue position.
          //Notify the user abou tthe queue position received.
          if (t204->IsRunning ())
            {
              t204->Stop ();
            }
        }
      else if (candidateSsrc == rxSsrc)
        {
          //Update the queue position.
          floorMachine.SetCurrentSsrc (rxSsrc);
          floorMachine.ClearCandidateSsrc ();
          //Notify the user about the queue position received.
          if (t204->IsRunning ())
            {
              t204->Stop ();
            }
        }
    }
}

void
McpttOffNetworkFloorParticipantStateQueued::ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  uint32_t currentSsrc = floorMachine.GetCurrentSsrc ();
  uint32_t candidateSsrc = floorMachine.GetCandidateSsrc ();

  if (floorMachine.HasCurrentSsrc ())
    {
      if (rxSsrc == currentSsrc)
        {
          t203->Restart ();
        }
    }
  else if (floorMachine.HasCandidateSsrc ())
    {
      if (rxSsrc == candidateSsrc)
        {
          t203->Restart ();
          floorMachine.SetCurrentSsrc (rxSsrc);
          floorMachine.ClearCurrentSsrc ();
        }
    }
}

void
McpttOffNetworkFloorParticipantStateQueued::ReleaseRequest (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttTimer> t233 = floorMachine.GetT233 ();

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();

  McpttFloorMsgRelease releaseMsg (txSsrc);
  releaseMsg.SetUserId (McpttFloorMsgFieldUserId (myUserId));

  if (t233->IsRunning ())
    {
      t233->Stop ();
    }

  floorMachine.Send (releaseMsg);

  floorMachine.ReportEvent (McpttFloorParticipant::PTT_BUTTON_RELEASED);

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
}

void
McpttOffNetworkFloorParticipantStateQueued::SendFloorQueuePositionRequest (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttTimer> t204 = floorMachine.GetT204 ();
  Ptr<McpttCounter> c204 = floorMachine.GetC204 ();

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();

  McpttFloorMsgQueuePositionRequest queuePosReqMsg (txSsrc);
  queuePosReqMsg.SetUserId (McpttFloorMsgFieldUserId (myUserId));

  c204->Reset ();
  t204->Start ();

  floorMachine.Send (queuePosReqMsg);
}
/** McpttOffNetworkFloorParticipantStateQueued - end **/

/** McpttOffNetworkFloorParticipantStateSilence - begin **/
Ptr<McpttOffNetworkFloorParticipantStateSilence>
McpttOffNetworkFloorParticipantStateSilence::GetInstance (void)
{
  static Ptr<McpttOffNetworkFloorParticipantStateSilence> instance = Create<McpttOffNetworkFloorParticipantStateSilence> ();

  return instance;
}

McpttEntityId
McpttOffNetworkFloorParticipantStateSilence::GetStateId (void)
{
  static McpttEntityId id (6, "'O: silence'");

  return id;
}

McpttOffNetworkFloorParticipantStateSilence::McpttOffNetworkFloorParticipantStateSilence (void)
  : McpttOffNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOffNetworkFloorParticipantStateSilence::~McpttOffNetworkFloorParticipantStateSilence (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOffNetworkFloorParticipantStateSilence::ExpiryOfT230 (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  //Shall indicate to call control that T230 expired.
  //TODO: Indicate to call control tha T230 has expired.

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ());
}

McpttEntityId
McpttOffNetworkFloorParticipantStateSilence::GetInstanceStateId (void) const
{
  return McpttOffNetworkFloorParticipantStateSilence::GetStateId ();
}

void
McpttOffNetworkFloorParticipantStateSilence::ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  uint32_t myUserId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  uint32_t rxUserId = msg.GetUserId ().GetUserId ();
  McpttFloorMsgFieldIndic indicator = msg.GetIndicator ();

  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();

  if (rxUserId != myUserId)
    {
      //Provide floor taken notification to user.
      if (indicator.IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
        {
          //Provide broadcast group call notification if indicated.
        }

      floorMachine.SetCandidateSsrc (rxSsrc);
      t230->Stop ();
      t203->Start ();

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
    }
}

void
McpttOffNetworkFloorParticipantStateSilence::ReceiveFloorRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  McpttFloorMsgFieldDuration durationField;
  Ptr<McpttTimer> t205 = floorMachine.GetT205 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  uint8_t callTypeId = floorMachine.GetCallTypeId ();
  McpttFloorMsgFieldUserId rxUserId = msg.GetUserId ();
  McpttFloorMsgFieldPriority rxPriority = msg.GetPriority ();

  if (callTypeId == McpttCallMsgFieldCallType::PRIVATE)
    {
      McpttFloorMsgGranted grantedMsg;
      grantedMsg.SetUserId (rxUserId);
      grantedMsg.SetSsrc (rxSsrc);
      grantedMsg.SetDuration (durationField);
      grantedMsg.SetPriority (rxPriority);

      t230->Stop ();
      t205->Start ();

      floorMachine.Send (grantedMsg);

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStatePendGrant::GetInstance ());
    }
}

void
McpttOffNetworkFloorParticipantStateSilence::ReceiveFloorTaken (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  //Provide floor taken notification to user.
  uint32_t rxSsrc = msg.GetSsrc ();

  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();

  floorMachine.SetCurrentSsrc (rxSsrc);
  t230->Stop ();
  t203->Start ();

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
}

void
McpttOffNetworkFloorParticipantStateSilence::ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  //Provide floor taken notification.
  //Request client to start rendering received RTP media packets.
  uint32_t rxSsrc = msg.GetHeader ().GetSsrc ();

  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  uint32_t storedSsrc = floorMachine.GetCurrentSsrc ();

  if (floorMachine.HasCurrentSsrc ())
    {
      if (storedSsrc == rxSsrc)
        {
          t203->Restart ();
          t230->Stop ();

          floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
        }
    }
  else
    {
      t230->Stop ();

      floorMachine.SetCurrentSsrc (rxSsrc);

      t203->Restart ();

      floorMachine.ChangeState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
    }
}

void
McpttOffNetworkFloorParticipantStateSilence::Selected (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttPttApp> appOwner = floorMachine.GetCall ()->GetOwner ();

  if (appOwner->IsPushed ())
    {
      appOwner->NotifyReleased ();
    }
}

void
McpttOffNetworkFloorParticipantStateSilence::PttPush (McpttOffNetworkFloorParticipant& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t userId = floorMachine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t201 = floorMachine.GetT201 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttCounter> c201 = floorMachine.GetC201 ();
  uint8_t priority = floorMachine.GetPriority ();
  McpttFloorMsgFieldIndic indicator = floorMachine.GetIndicator ();

  McpttFloorMsgRequest reqMsg (txSsrc);
  reqMsg.SetIndicator (indicator);
  reqMsg.SetUserId (userId);
  reqMsg.SetPriority (priority);

  c201->Reset ();
  t201->Start ();
  t230->Stop ();

  floorMachine.Send (reqMsg);

  floorMachine.ChangeState (McpttOffNetworkFloorParticipantStatePendReq::GetInstance ());
}
/** McpttOffNetworkFloorParticipantStateSilence - end **/

} //namespace psc
} //namespace ns3

