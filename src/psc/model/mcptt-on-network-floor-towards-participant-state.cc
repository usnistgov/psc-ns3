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

#include "mcptt-entity-id.h"
#include "mcptt-on-network-floor-arbitrator.h"
#include "mcptt-on-network-floor-arbitrator-state.h"
#include "mcptt-server-app.h"
#include "mcptt-server-call.h"
#include "mcptt-server-call-machine.h"
#include "mcptt-on-network-floor-towards-participant.h"
#include "mcptt-call-msg.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"

#include "mcptt-on-network-floor-towards-participant-state.h"

namespace ns3 {

namespace psc {

/** McpttOnNetworkFloorTowardsParticipantState - begin **/
NS_LOG_COMPONENT_DEFINE ("McpttOnNetworkFloorTowardsParticipantState");

McpttOnNetworkFloorTowardsParticipantState::~McpttOnNetworkFloorTowardsParticipantState (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkFloorTowardsParticipantState::CallInitialized (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring call initiated notification.");
}

void
McpttOnNetworkFloorTowardsParticipantState::CallRelease1 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this);

  //TODO: Shall stop sending floor control messages.

  //TODO: Shall request the network media interface to stop rendering RTP
  //      media packets towards to the associated MCPTT client

  //TODO: Shall ignore any floor control messages received from the associated
  //      floor participant

  machine->GetOwner ()->CallRelease1 ();

  machine->ChangeState (McpttOnNetworkFloorTowardsParticipantStateReleasing::GetInstance ());
}

void
McpttOnNetworkFloorTowardsParticipantState::CallRelease2 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring call release notification (part II).");
}

void
McpttOnNetworkFloorTowardsParticipantState::Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring enter.");
}

void
McpttOnNetworkFloorTowardsParticipantState::ExpiryOfT8 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T8.");
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantState::GetInstanceStateId (void) const
{
  return McpttEntityId ();
}

void
McpttOnNetworkFloorTowardsParticipantState::ReceiveFloorQueuePositionRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgQueuePositionRequest& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " storing " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorTowardsParticipantState::ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorTowardsParticipantState::ReceiveFloorRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorTowardsParticipantState::ReceiveMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorTowardsParticipantState::Selected (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

void
McpttOnNetworkFloorTowardsParticipantState::SendFloorDeny (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " sending " << msg.GetInstanceTypeId () << ".");

  machine->DoSend (msg);
}

void
McpttOnNetworkFloorTowardsParticipantState::SendFloorGranted (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " sending " << msg.GetInstanceTypeId () << ".");

  machine->DoSend (msg);
}

void
McpttOnNetworkFloorTowardsParticipantState::SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " sending " << msg.GetInstanceTypeId () << ".");

  machine->DoSend (msg);
}

void
McpttOnNetworkFloorTowardsParticipantState::SendFloorRevoke (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgRevoke& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " sending " << msg.GetInstanceTypeId () << ".");

  machine->DoSend (msg);
}

void
McpttOnNetworkFloorTowardsParticipantState::SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " sending " << msg.GetInstanceTypeId () << ".");

  machine->DoSend (msg);
}

void
McpttOnNetworkFloorTowardsParticipantState::SendMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " sending " << msg.GetInstanceTypeId () << ".");

  machine->DoSend (msg);
}

void
McpttOnNetworkFloorTowardsParticipantState::ClientRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring client release notification.");
}

void
McpttOnNetworkFloorTowardsParticipantState::Terminate (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring client release notification.");
}

void
McpttOnNetworkFloorTowardsParticipantState::Unselected (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

McpttOnNetworkFloorTowardsParticipantState::McpttOnNetworkFloorTowardsParticipantState (void)
  : SimpleRefCount<McpttOnNetworkFloorTowardsParticipantState> ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttOnNetworkFloorTowardsParticipantState - end **/

std::ostream&
operator<< (std::ostream& os, const McpttOnNetworkFloorTowardsParticipantState& state)
{
  return os << state.GetInstanceStateId ();
}

/** McpttOnNetworkFloorTowardsParticipantStateStartStop - begin **/
Ptr<McpttOnNetworkFloorTowardsParticipantStateStartStop>
McpttOnNetworkFloorTowardsParticipantStateStartStop::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateStartStop> instance = Create<McpttOnNetworkFloorTowardsParticipantStateStartStop> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateStartStop::GetStateId (void)
{
  static McpttEntityId id (0, "'Start-stop'");

  return id;
}

McpttOnNetworkFloorTowardsParticipantStateStartStop::McpttOnNetworkFloorTowardsParticipantStateStartStop (void)
  : McpttOnNetworkFloorTowardsParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorTowardsParticipantStateStartStop::~McpttOnNetworkFloorTowardsParticipantStateStartStop (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateStartStop::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorTowardsParticipantStateStartStop::GetStateId ();
}

void
McpttOnNetworkFloorTowardsParticipantStateStartStop::CallInitialized (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
//  NS_LOG_FUNCTION (this);

  // Procedures are drawn from 24.380 section 6.3.5.2.2 SIP Session initiated
  NS_LOG_DEBUG ("IsTemporaryGroup: " << machine->GetOwner ()->GetOwner ()->IsTemporaryGroup ()
                                     << " IsAmbientListening: " << machine->GetOwner ()->GetOwner ()->IsAmbientListening ()
                                     << " IsStarted: " << machine->GetOwner ()->IsStarted ()
                                     << " IsImplicitRequest: " << machine->IsImplicitRequest ()
                                     << " IsFloorOccupied: " << machine->GetOwner ()->IsFloorOccupied ()
                                     << " IsEnabled: " << machine->GetOwner ()->IsQueueingSupported ()
                                     << " IsQueueing: " <<  machine->IsQueueing ()
                                     << " IsDualFloor: " <<  machine->IsDualFloor ());
  if (!machine->GetOwner ()->GetOwner ()->IsTemporaryGroup ()
      || !machine->GetOwner ()->GetOwner ()->IsAmbientListening ())
    {
      if (!machine->GetOwner ()->IsStarted ()
          && machine->IsImplicitRequest ())
        {
          NS_LOG_DEBUG ("Call not started and implicit request");
          McpttOnNetworkFloorTowardsParticipantStatePermitted::GetInstance ()->Enter (machine);
        }
      else if (machine->GetOwner ()->IsStarted ()
               && !machine->GetOwner ()->IsFloorOccupied ())
        {
          NS_LOG_DEBUG ("Call is started and floor is not occupied");
          McpttFloorMsgIdle idleMsg;
          idleMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->GetOwner ()->NextSeqNum ()));
          idleMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
          machine->DoSend (idleMsg);
          McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetInstance ()->Enter (machine);
        }
      else if (!machine->GetOwner ()->IsStarted ()
               && !machine->IsImplicitRequest ())
        {
          NS_LOG_DEBUG ("Call is not started and no implicit floor request");
          McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetInstance ()->Enter (machine);
        }
      else if (machine->GetOwner ()->IsStarted ()
               && machine->GetOwner ()->IsFloorOccupied ())
        {
          NS_LOG_DEBUG ("Call is started and floor is occupied");
          McpttFloorMsgTaken takenMsg;
          takenMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine->GetOwner ()->GetStoredSsrc ()));
          takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->GetOwner ()->NextSeqNum ()));
          takenMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
          if (machine->GetOwner ()->GetOwner ()->GetCallMachine ()->GetCallType ().GetType () == McpttCallMsgFieldCallType::BROADCAST_GROUP)
            {
              takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (false));
            }
          else
            {
              takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (true));
            }
          machine->DoSend (takenMsg);
          McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
        }
      else if (machine->GetOwner ()->IsStarted ()
               && machine->IsImplicitRequest ())
        {
          if (!machine->GetOwner ()->IsFloorOccupied ())
            {
              NS_LOG_DEBUG ("Call is started and implicit floor request and floor not occupied");
              McpttFloorMsgRequest reqMsg;
              reqMsg.SetSsrc (machine->GetStoredSsrc ());
              reqMsg.SetPriority (machine->GetStoredSsrc ());
              machine->GetOwner ()->ReceiveFloorRequest (reqMsg);
              McpttOnNetworkFloorTowardsParticipantStatePermitted::GetInstance ()->Enter (machine);
            }
          else if (machine->GetOwner ()->IsQueueingSupported ()
                   && machine->IsQueueing ())
            {
              //TODO: shall set the priority level to the negotated maximum priority level
              NS_LOG_DEBUG ("Call is started and implicit floor request and floor is occupied (with queueing enabled)");
              McpttQueuedUserInfo queueInfo = McpttQueuedUserInfo (machine->GetStoredSsrc (), McpttFloorMsgFieldQueuedUserId (machine->GetStoredSsrc ()), McpttFloorMsgFieldQueuePositionInfo (0, machine->GetStoredPriority ()));
              machine->GetOwner ()->GetQueue ()->Enqueue (queueInfo);
              McpttFloorMsgQueuePositionInfo queuedMsg;
              queuedMsg.SetSsrc (machine->GetOwner ()->GetStoredSsrc ());
              queuedMsg.SetQueuePositionInfo (queueInfo.GetInfo ());
              queuedMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
              machine->DoSend (queuedMsg);
              McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
            }
          else
            {
              NS_LOG_DEBUG ("Call is started and implicit floor request and floor is occupied (with queueing disabled)");
              McpttFloorMsgTaken takenMsg;
              takenMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
              takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine->GetOwner ()->GetStoredSsrc ()));
              takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->GetOwner ()->NextSeqNum ()));
              takenMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
              if (machine->GetOwner ()->GetOwner ()->GetCallMachine ()->GetCallType ().GetType () == McpttCallMsgFieldCallType::BROADCAST_GROUP)
                {
                  takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (false));
                }
              else
                {
                  takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (true));
                }
              machine->DoSend (takenMsg);
              McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
            }
        }
    }
  else // The client was invited
    {
      NS_LOG_DEBUG ("The client was invited");
      if (machine->GetOwner ()->IsFloorOccupied ())
        {
          NS_LOG_DEBUG ("Floor occupied; generate taken message");
          McpttFloorMsgTaken takenMsg;
          takenMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine->GetOwner ()->GetStoredSsrc ()));
          takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->GetOwner ()->NextSeqNum ()));
          if (machine->GetOwner ()->GetOwner ()->GetCallMachine ()->GetCallType ().GetType () == McpttCallMsgFieldCallType::BROADCAST_GROUP)
            {
              takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (false));
            }
          else
            {
              takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (true));
            }
          takenMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
          machine->DoSend (takenMsg);
          McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
        }
      else
        {
          NS_LOG_DEBUG ("Floor not occupied; generate idle message");
          McpttFloorMsgIdle idleMsg;
          idleMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->GetOwner ()->NextSeqNum ()));
          idleMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
          McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetInstance ()->Enter (machine);
        }
    }
  //TODO: Implement temporary group session procedures
}
/** McpttOnNetworkFloorTowardsParticipantStateStartStop - end **/

/** McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle - begin **/
Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle>
McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle> instance = Create<McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetStateId (void)
{
  static McpttEntityId id (1, "'U: not permitted and Floor Idle'");

  return id;
}

McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle (void)
  : McpttOnNetworkFloorTowardsParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::~McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetStateId ();
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine->SetTrackInfo (McpttFloorMsgFieldTrackInfo ());
  machine->ChangeState (McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetInstance ());
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgTaken::SUBTYPE_ACK);
    }

  machine->DoSend (msg);

  McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::ReceiveFloorRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine->GetOwner ()->GetOwner ()->GetCallMachine ()->GetCallType ().GetType () != McpttCallMsgFieldCallType::BROADCAST_GROUP)
    {
      machine->GetOwner ()->ReceiveFloorRequest (msg);
    }
  else //The call IS a Broadcast Group Call
    {
      if (machine->IsOriginator ())
        {
          machine->GetOwner ()->ReceiveFloorRequest (msg);
        }
      else
        {
          McpttFloorMsgDeny denyMsg;
          denyMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_5));
          if (machine->GetOwner ()->IsAckRequired ())
            {
              denyMsg.SetSubtype (McpttFloorMsgDeny::SUBTYPE_ACK);
            }
          denyMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
          machine->DoSend (denyMsg);
        }
    }
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::SendFloorGranted (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgGranted::SUBTYPE_ACK);
    }
  machine->DoSend (msg);
  McpttOnNetworkFloorTowardsParticipantStatePermitted::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::SendFloorDeny (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgDeny::SUBTYPE_ACK);
    }
  machine->DoSend (msg);
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgRelease::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgRelease::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::CONTROLLING_FUNCTION));
      machine->DoSend (ackMsg);
    }

  McpttFloorMsgIdle idleMsg;
  idleMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
  idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->GetOwner ()->NextSeqNum ()));

  if (machine->GetOwner ()->IsAckRequired ())
    {
      idleMsg.SetSubtype (McpttFloorMsgIdle::SUBTYPE_ACK);
    }

  idleMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
  machine->DoSend (idleMsg);

  machine->GetOwner ()->GetQueue ()->Pull (msg.GetSsrc ());
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::ReceiveMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttMediaMsg& msg) const
{
  //TODO: Shall request the MCPTT server not to foward the received RTP media packets

  McpttFloorMsgRevoke revokeMsg;
  revokeMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
  revokeMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_3));
  revokeMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
  machine->DoSend (revokeMsg);
  machine->SetRevokeMsg (revokeMsg);

  McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgIdle::SUBTYPE_ACK);
    }
  machine->DoSend (msg);
}
/** McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle - end **/

/** McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken - begin **/
Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken>
McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken> instance = Create<McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetStateId (void)
{
  static McpttEntityId id (2, "'U: not permitted and Floor Taken'");

  return id;
}

McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken (void)
  : McpttOnNetworkFloorTowardsParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::~McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetStateId ();
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine->SetTrackInfo (McpttFloorMsgFieldTrackInfo ());
  machine->ChangeState (McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ());
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgIdle::SUBTYPE_ACK);
    }

  machine->DoSend (msg);

  if (machine->IsDualFloor ())
    {
      machine->SetDualFloor (false);
    }
  else
    {
      McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetInstance ()->Enter (machine);
    }
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::ReceiveFloorRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine->GetOwner ()->IsAudioCutIn ())
    {
      machine->GetOwner ()->ReceiveFloorRequest (msg);
    }
  else if (!msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::EMERGENCY_CALL)
           && !msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::IMMINENT_CALL)
           && (!machine->GetOwner ()->IsQueueingSupported () || !machine->IsQueueing ())
           && msg.GetPriority ().GetPriority () == 0)
    {
      McpttFloorMsgDeny denyMsg;
      denyMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_1));
      denyMsg.UpdateTrackInfo (msg.GetTrackInfo ());
      denyMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
      if (machine->GetOwner ()->IsAckRequired ())
        {
          denyMsg.SetSubtype (McpttFloorMsgDeny::SUBTYPE_ACK);
        }
      machine->DoSend (denyMsg);
    }
  else if (machine->GetOwner ()->GetOwner ()->GetCallMachine ()->GetCallType ().GetType () == McpttCallMsgFieldCallType::BROADCAST_GROUP)
    {
      McpttFloorMsgDeny denyMsg;
      denyMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_5));
      denyMsg.UpdateTrackInfo (msg.GetTrackInfo ());
      denyMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
      if (machine->GetOwner ()->IsAckRequired ())
        {
          denyMsg.SetSubtype (McpttFloorMsgDeny::SUBTYPE_ACK);
        }
      machine->DoSend (denyMsg);
    }
  else if (machine->GetOwner ()->IsPreemptive (msg) == false)
    {
      uint16_t position;
      McpttQueuedUserInfo queueInfo;
      if (machine->IsReceiveOnly ())
        {
          McpttFloorMsgDeny denyMsg;
          denyMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_5));
          denyMsg.UpdateTrackInfo (msg.GetTrackInfo ());
          denyMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
          if (machine->GetOwner ()->IsAckRequired ())
            {
              denyMsg.SetSubtype (McpttFloorMsgDeny::SUBTYPE_ACK);
            }
          machine->DoSend (denyMsg);
        }
      else if (!machine->GetOwner ()->IsQueueingSupported ()
               || !machine->IsQueueing ())
        {
          McpttFloorMsgDeny denyMsg;
          denyMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_1));
          denyMsg.UpdateTrackInfo (msg.GetTrackInfo ());
          denyMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
          if (machine->GetOwner ()->IsAckRequired ())
            {
              denyMsg.SetSubtype (McpttFloorMsgDeny::SUBTYPE_ACK);
            }
          machine->DoSend (denyMsg);
        }
      else if (machine->GetOwner ()->GetQueue ()->View (msg.GetSsrc (), queueInfo, position))
        {
          McpttFloorMsgQueuePositionInfo queueInfoMsg;
          queueInfoMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          queueInfoMsg.SetQueuedUserId (queueInfo.GetUserId ());
          queueInfoMsg.SetQueuePositionInfo (queueInfo.GetInfo ());
          queueInfoMsg.UpdateTrackInfo (msg.GetTrackInfo ());
          queueInfoMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
          machine->DoSend (queueInfoMsg);
        }
      else
        {
          McpttFloorMsgFieldQueuePositionInfo queuePositionInfo;
          queuePositionInfo.SetPriority (msg.GetPriority ().GetPriority ());
          queueInfo.SetSsrc (msg.GetSsrc ());
          queueInfo.SetUserId (McpttFloorMsgFieldQueuedUserId (msg.GetSsrc ()));
          queueInfo.SetInfo (queuePositionInfo);
          machine->GetOwner ()->GetQueue ()->Enqueue (queueInfo);
          McpttFloorMsgQueuePositionInfo queueInfoMsg;
          queueInfoMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          queueInfoMsg.SetQueuedUserId (queueInfo.GetUserId ());
          queueInfoMsg.SetQueuePositionInfo (queueInfo.GetInfo ());
          queueInfoMsg.UpdateTrackInfo (msg.GetTrackInfo ());
          queueInfoMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
          if (machine->GetOwner ()->IsAckRequired ())
            {
              queueInfoMsg.SetSubtype (McpttFloorMsgQueuePositionInfo::SUBTYPE_ACK);
            }
          machine->DoSend (queueInfoMsg);
        }
    }
  else if (machine->GetOwner ()->IsPreemptive (msg) == true)
    {
      machine->GetOwner ()->ReceiveFloorRequest (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if ((!machine->GetOwner ()->IsQueueingSupported ()
       || !machine->IsQueueing ()))
    {
      if (msg.GetSubtype () == McpttFloorMsgRelease::SUBTYPE_ACK)
        {
          McpttFloorMsgAck ackMsg;
          ackMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgRelease::SUBTYPE));
          ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::CONTROLLING_FUNCTION));
          machine->DoSend (ackMsg);
        }

      McpttFloorMsgTaken takenMsg;
      takenMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine->GetOwner ()->GetStoredSsrc ()));
      takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->GetOwner ()->NextSeqNum ()));

      if (machine->GetOwner ()->GetOwner ()->GetCallMachine ()->GetCallType ().GetType () == McpttCallMsgFieldCallType::BROADCAST_GROUP)
        {
          takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (false));
        }
      else
        {
          takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (true));
        }

      takenMsg.UpdateTrackInfo (msg.GetTrackInfo ());

      if (machine->GetOwner ()->IsAckRequired ())
        {
          takenMsg.SetSubtype (McpttFloorMsgTaken::SUBTYPE_ACK);
        }

      takenMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
      machine->DoSend (takenMsg);
    }
  else
    {
      if (msg.GetSubtype () == McpttFloorMsgRelease::SUBTYPE_ACK)
        {
          McpttFloorMsgAck ackMsg;
          ackMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgRelease::SUBTYPE));
          ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::CONTROLLING_FUNCTION));
          machine->DoSend (ackMsg);
        }

      machine->GetOwner ()->GetQueue ()->Pull (msg.GetSsrc ());

      McpttFloorMsgTaken takenMsg;
      takenMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine->GetOwner ()->GetStoredSsrc ()));
      if (machine->GetOwner ()->GetOwner ()->GetCallMachine ()->GetCallType ().GetType () == McpttCallMsgFieldCallType::BROADCAST_GROUP)
        {
          takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (false));
        }
      else
        {
          takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (true));
        }

      takenMsg.UpdateTrackInfo (msg.GetTrackInfo ());
      takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->GetOwner ()->NextSeqNum ()));
      takenMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());

      if (machine->GetOwner ()->IsAckRequired ())
        {
          takenMsg.SetSubtype (McpttFloorMsgTaken::SUBTYPE_ACK);
        }

      machine->DoSend (takenMsg);
    }
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::ReceiveMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  //TODO: Request the network media interface to not forward the RTP media packets

  McpttFloorMsgRevoke revokeMsg;
  revokeMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
  revokeMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_3));
  revokeMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
  machine->DoSend (revokeMsg);

  McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::ReceiveFloorQueuePositionRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgQueuePositionRequest& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  uint16_t position;
  McpttQueuedUserInfo queuedInfo;

  machine->GetOwner ()->GetQueue ()->View (msg.GetSsrc (), queuedInfo, position);

  McpttFloorMsgQueuePositionInfo queueInfoMsg;
  queueInfoMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
  queueInfoMsg.SetQueuePositionInfo (queuedInfo.GetInfo ());
  queueInfoMsg.UpdateTrackInfo (msg.GetTrackInfo ());

  if (machine->GetOwner ()->IsAckRequired ())
    {
      queueInfoMsg.SetSubtype (McpttFloorMsgQueuePositionInfo::SUBTYPE_ACK);
    }

  queueInfoMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());

  machine->DoSend (queueInfoMsg);
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::SendFloorGranted (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgGranted::SUBTYPE_ACK);
    }

  machine->DoSend (msg);

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::DUAL_FLOOR))
    {
      machine->SetOverriding (true);
    }

  McpttOnNetworkFloorTowardsParticipantStatePermitted::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgTaken::SUBTYPE_ACK);
    }

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::DUAL_FLOOR))
    {
      machine->DoSend (msg);

      machine->SetDualFloor (true);
    }
}
/** McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken - end **/

/** McpttOnNetworkFloorTowardsParticipantStatePermitted - begin **/
Ptr<McpttOnNetworkFloorTowardsParticipantStatePermitted>
McpttOnNetworkFloorTowardsParticipantStatePermitted::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorTowardsParticipantStatePermitted> instance = Create<McpttOnNetworkFloorTowardsParticipantStatePermitted> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStatePermitted::GetStateId (void)
{
  static McpttEntityId id (3, "'U: permitted'");

  return id;
}

McpttOnNetworkFloorTowardsParticipantStatePermitted::McpttOnNetworkFloorTowardsParticipantStatePermitted (void)
  : McpttOnNetworkFloorTowardsParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorTowardsParticipantStatePermitted::~McpttOnNetworkFloorTowardsParticipantStatePermitted (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStatePermitted::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorTowardsParticipantStatePermitted::GetStateId ();
}

void
McpttOnNetworkFloorTowardsParticipantStatePermitted::Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine->ChangeState (McpttOnNetworkFloorTowardsParticipantStatePermitted::GetInstance ());
}

void
McpttOnNetworkFloorTowardsParticipantStatePermitted::ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgRelease::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgRelease::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::CONTROLLING_FUNCTION));

      machine->DoSend (ackMsg);
    }

  if (machine->IsOverriding ())
    {
      //TODO: May need to remove ACK bit
      //msg.SetSubtype (McpttFloorMsgRelease::SUBTYPE);
      machine->GetOwner ()->GetDualControl ()->ReceiveFloorRelease (msg);
      machine->SetOverriding (false);
      McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
    }
  else if (machine->IsOverridden ())
    {
      //TODO: May need to remove ACK bit
      //msg.SetSubtype (McpttFloorMsgRelease::SUBTYPE);
      machine->GetOwner ()->ReceiveFloorRelease (msg);
      machine->SetOverridden (false);
      McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
    }
  else
    {
      //TODO: May need to remove ACK bit
      //msg.SetSubtype (McpttFloorMsgRelease::SUBTYPE);
      machine->GetOwner ()->ReceiveFloorRelease (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipantStatePermitted::SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::DUAL_FLOOR
                                       && machine->IsOverridden ()))
    {
      machine->DoSend (msg);
      machine->SetOverridden (false);
    }
  else if (machine->IsOverriding ())
    {
      machine->DoSend (msg);
      machine->SetOverriding (false);
    }
  else
    {
      //Do nothing.
    }
}

void
McpttOnNetworkFloorTowardsParticipantStatePermitted::SendFloorRevoke (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgRevoke& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine->DoSend (msg);
  machine->SetRevokeMsg (msg);
  machine->SetTrackInfo (msg.GetTrackInfo ());

  McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorTowardsParticipantStatePermitted::ReceiveMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (!machine->IsOverriding ())
    {
      //TODO: Shall request the network media interface in the MCPTT server to foward RTP media packets.
    }

  //TODO: Not in standard
  machine->GetOwner ()->ReceiveMedia (msg);
}

void
McpttOnNetworkFloorTowardsParticipantStatePermitted::ReceiveFloorRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  machine->GetOwner ()->ReceiveFloorRequest (msg);

  //TODO: Shall instruct the media distributor to foward the RTP media packets
  //      to MCPTT clients.

  if (machine->IsOverriding ())
    {
      //TODO: Shall request the network media interface in the MCPTT server
      //      to foward RTP media packets to the media distributor.

      //TODO: Shall instruct the media distributor to forward the received RTP
      //      media packets to any non-controlling MCPTT functions, to the
      //      overridden MCPTT client and to those MCPTT clients receiving RTP
      //      media from the overriding MCPTT client controlled by the
      //      controlling MCPTT functions according to local policy.
    }
}

void
McpttOnNetworkFloorTowardsParticipantStatePermitted::SendMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine->IsOverridden ()
      || machine->IsOverriding ())
    {
      machine->DoSend (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipantStatePermitted::SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::DUAL_FLOOR))
    {
      machine->DoSend (msg);
      machine->SetOverridden (true);
    }
  else
    {
      machine->DoSend (msg);
      McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
    }
}
/** McpttOnNetworkFloorTowardsParticipantStatePermitted - end **/

/** McpttOnNetworkFloorTowardsParticipantStatePendingRevoke - begin **/
Ptr<McpttOnNetworkFloorTowardsParticipantStatePendingRevoke>
McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorTowardsParticipantStatePendingRevoke> instance = Create<McpttOnNetworkFloorTowardsParticipantStatePendingRevoke> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::GetStateId (void)
{
  static McpttEntityId id (4, "'U: pending Floor Revoke'");

  return id;
}

McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::McpttOnNetworkFloorTowardsParticipantStatePendingRevoke (void)
  : McpttOnNetworkFloorTowardsParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::~McpttOnNetworkFloorTowardsParticipantStatePendingRevoke (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::GetStateId ();
}

void
McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine->GetT8 ()->Start ();
  machine->ChangeState (McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::GetInstance ());
}

void
McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::ExpiryOfT8 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  McpttFloorMsgRevoke revokeMsg = machine->GetRevokeMsg ();

  machine->DoSend (revokeMsg);
  machine->GetT8 ()->Start ();

  //TODO: How many times before giving up is up to implementation but it is
  //      recommnded that the client is disconnected once the floor control
  //      server has given up
}

void
McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::ReceiveMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  //TODO: Foward RTP media packets to the media distributor
  //TODO: Not in standard
  machine->GetOwner ()->ReceiveMedia (msg);
}

void
McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgRelease::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgRelease::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::CONTROLLING_FUNCTION));

      machine->DoSend (ackMsg);
    }

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::DUAL_FLOOR))
    {
      if (machine->GetOwner ()->GetStateId () == McpttOnNetworkFloorArbitratorStateTaken::GetStateId ())
        {
          McpttFloorMsgTaken takenMsg;
          takenMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine->GetOwner ()->GetStoredSsrc ()));
          takenMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());

          machine->DoSend (takenMsg);

          McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
        }
      else if (machine->GetOwner ()->GetStateId () == McpttOnNetworkFloorArbitratorStateIdle::GetStateId ())
        {
          McpttFloorMsgIdle idleMsg;
          idleMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
          idleMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());

          machine->DoSend (idleMsg);

          McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetInstance ()->Enter (machine);
        }
    }
  else
    {
      machine->GetOwner ()->ReceiveFloorRelease (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgIdle::SUBTYPE_ACK);
    }

  machine->DoSend (msg);

  McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorTowardsParticipantStatePendingRevoke::SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgTaken::SUBTYPE_ACK);
    }

  machine->DoSend (msg);

  McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
}
/** McpttOnNetworkFloorTowardsParticipantStatePendingRevoke - end **/

/** McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia - begin **/
Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia>
McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia> instance = Create<McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::GetStateId (void)
{
  static McpttEntityId id (5, "'U: not permitted but sends media'");

  return id;
}

McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia (void)
  : McpttOnNetworkFloorTowardsParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::~McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::GetStateId ();
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine->GetT8 ()->Start ();
  machine->ChangeState (McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::GetInstance ());

  //TODO: Shall not request the network media interface in the MCPTT server to
  //      forward RTP media packets from the MCPTT client to the media
  //      distributor in the MCPTT server
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::ExpiryOfT8 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  McpttFloorMsgRevoke revokeMsg;
  revokeMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
  revokeMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_3));
  revokeMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());

  machine->DoSend (revokeMsg);

  machine->GetT8 ()->Start ();

  //TODO: How many times before giving up is up to implementation but it is
  //      recommnded that the client is disconnected once the floor control
  //      server has given up
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia::ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgRelease::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgRelease::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::CONTROLLING_FUNCTION));
      machine->DoSend (ackMsg);
    }

  if (machine->GetOwner ()->GetStateId () == McpttOnNetworkFloorArbitratorStateIdle::GetStateId ())
    {
      McpttFloorMsgIdle idleMsg;
      idleMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->GetOwner ()->NextSeqNum ()));
      idleMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());

      machine->DoSend (idleMsg);

      McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetInstance ()->Enter (machine);
    }
  else if (machine->GetOwner ()->GetStateId () == McpttOnNetworkFloorArbitratorStateTaken::GetStateId ())
    {
      McpttFloorMsgTaken takenMsg;
      takenMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine->GetOwner ()->GetStoredSsrc ()));
      takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->GetOwner ()->NextSeqNum ()));

      if (machine->GetOwner ()->GetOwner ()->GetCallMachine ()->GetCallType ().GetType () == McpttCallMsgFieldCallType::BROADCAST_GROUP)
        {
          takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (false));
        }
      else
        {
          takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (true));
        }

      if (machine->GetOwner ()->IsAckRequired ())
        {
          takenMsg.SetSubtype (McpttFloorMsgTaken::SUBTYPE_ACK);
        }

      takenMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());

      machine->DoSend (takenMsg);

      McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
    }
}
/** McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia - end **/

/** McpttOnNetworkFloorTowardsParticipantStateReleasing - begin **/
Ptr<McpttOnNetworkFloorTowardsParticipantStateReleasing>
McpttOnNetworkFloorTowardsParticipantStateReleasing::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateReleasing> instance = Create<McpttOnNetworkFloorTowardsParticipantStateReleasing> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateReleasing::GetStateId (void)
{
  static McpttEntityId id (6, "'Releasing'");

  return id;
}

McpttOnNetworkFloorTowardsParticipantStateReleasing::McpttOnNetworkFloorTowardsParticipantStateReleasing (void)
  : McpttOnNetworkFloorTowardsParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorTowardsParticipantStateReleasing::~McpttOnNetworkFloorTowardsParticipantStateReleasing (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateReleasing::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorTowardsParticipantStateReleasing::GetStateId ();
}

void
McpttOnNetworkFloorTowardsParticipantStateReleasing::CallRelease2 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this);

  //TODO: Shall request the network media interface to release all resources
  //      associated with this MCPTT client for this MCPTT call.

  machine->ChangeState (McpttOnNetworkFloorTowardsParticipantStateStartStop::GetInstance ());
}
/** McpttOnNetworkFloorTowardsParticipantStateReleasing - end **/

/** McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating - begin **/
Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating>
McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating> instance = Create<McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::GetStateId (void)
{
  static McpttEntityId id (7, "'U: not permitted and initiating'");

  return id;
}

McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating (void)
  : McpttOnNetworkFloorTowardsParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::~McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::GetStateId ();
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine->ChangeState (McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::GetInstance ());
}
void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgTaken::SUBTYPE_ACK);
    }

  machine->DoSend (msg);

  McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgIdle::SUBTYPE_ACK);
    }

  machine->DoSend (msg);

  McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::ReceiveFloorRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine->GetOwner ()->IsQueueingSupported ()
      && machine->IsQueueing ())
    {
      McpttQueuedUserInfo queueInfo;
      queueInfo.SetSsrc (msg.GetSsrc ());
      queueInfo.SetUserId (McpttFloorMsgFieldQueuedUserId (msg.GetSsrc ()));

      McpttFloorMsgFieldQueuePositionInfo queuePositionInfo;
      queuePositionInfo.SetPriority (msg.GetPriority ().GetPriority ());

      queueInfo.SetInfo (queuePositionInfo);

      machine->GetOwner ()->GetQueue ()->Enqueue (queueInfo);

      McpttFloorMsgQueuePositionInfo queueInfoMsg;
      queueInfoMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      queueInfoMsg.SetQueuedUserId (queueInfo.GetUserId ());
      queueInfoMsg.SetQueuePositionInfo (queueInfo.GetInfo ());
      queueInfoMsg.UpdateTrackInfo (msg.GetTrackInfo ());
      queueInfoMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());

      if (machine->GetOwner ()->IsAckRequired ())
        {
          queueInfoMsg.SetSubtype (McpttFloorMsgQueuePositionInfo::SUBTYPE_ACK);
        }

      machine->DoSend (queueInfoMsg);
    }
  else
    {
      McpttFloorMsgDeny denyMsg;
      denyMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_1));
      denyMsg.UpdateTrackInfo (msg.GetTrackInfo ());
      denyMsg.SetIndicator (machine->GetOwner ()->GetIndicator ());
    }
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::SendFloorGranted (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine->GetOwner ()->IsAckRequired ())
    {
      msg.SetSubtype (McpttFloorMsgGranted::SUBTYPE_ACK);
    }

  machine->DoSend (msg);

  McpttOnNetworkFloorTowardsParticipantStatePermitted::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating::ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgRelease::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine->GetOwner ()->GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgRelease::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::CONTROLLING_FUNCTION));

      machine->DoSend (ackMsg);
    }

  machine->GetOwner ()->GetQueue ()->Pull (msg.GetSsrc ());
}
/** McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating - end **/

} //namespace psc
} //namespace ns3

