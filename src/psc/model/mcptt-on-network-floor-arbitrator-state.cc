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
#include "mcptt-server-call.h"
#include "mcptt-server-call-machine.h"
#include "mcptt-on-network-floor-arbitrator.h"
#include "mcptt-on-network-floor-towards-participant.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-media-src.h"
#include "mcptt-ptt-app.h"

#include "mcptt-on-network-floor-arbitrator-state.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttOnNetworkFloorArbitratorState");

namespace psc {

/** McpttOnNetworkFloorArbitratorState - begin **/
McpttOnNetworkFloorArbitratorState::~McpttOnNetworkFloorArbitratorState (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkFloorArbitratorState::CallInitialized (Ptr<McpttOnNetworkFloorArbitrator> machine, Ptr<McpttOnNetworkFloorTowardsParticipant> participant) const
{
  NS_LOG_FUNCTION (this << machine << participant);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring call initialization.");
}

void
McpttOnNetworkFloorArbitratorState::Enter (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring enter.");
}

void
McpttOnNetworkFloorArbitratorState::ExpiryOfT1 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T1.");
}

void
McpttOnNetworkFloorArbitratorState::ExpiryOfT2 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T2.");
}

void
McpttOnNetworkFloorArbitratorState::ExpiryOfT3 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T3.");
}

void
McpttOnNetworkFloorArbitratorState::ExpiryOfT4 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T4.");
}

void
McpttOnNetworkFloorArbitratorState::ExpiryOfT7 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << GetInstanceStateId ().GetName () << machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T7.");
}

void
McpttOnNetworkFloorArbitratorState::ExpiryOfT20 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T20.");
}

McpttEntityId
McpttOnNetworkFloorArbitratorState::GetInstanceStateId (void) const
{
  return McpttEntityId ();
}

bool
McpttOnNetworkFloorArbitratorState::IsFloorOccupied (void) const
{
  return false;
}

void
McpttOnNetworkFloorArbitratorState::ReceiveFloorRelease (Ptr<McpttOnNetworkFloorArbitrator> machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorArbitratorState::ReceiveFloorRequest (Ptr<McpttOnNetworkFloorArbitrator> machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorArbitratorState::ReceiveMedia (Ptr<McpttOnNetworkFloorArbitrator> machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorArbitratorState::Selected (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);
}

void
McpttOnNetworkFloorArbitratorState::ClientRelease (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring client release notification.");
}

void
McpttOnNetworkFloorArbitratorState::CallRelease1 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this);

  //TODO: shall request the media distributor in the MCPTT server to stop
  //      sending RTP media packets to MCPTT clients

  machine->ChangeState (McpttOnNetworkFloorArbitratorStateReleasing::GetInstance ());
}

void
McpttOnNetworkFloorArbitratorState::CallRelease2 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring call release (part II).");
}

void
McpttOnNetworkFloorArbitratorState::Unselected (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);
}

McpttOnNetworkFloorArbitratorState::McpttOnNetworkFloorArbitratorState (void)
  : SimpleRefCount<McpttOnNetworkFloorArbitratorState> ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttOnNetworkFloorArbitratorState - end **/

std::ostream&
operator<< (std::ostream& os, const McpttOnNetworkFloorArbitratorState& state)
{
  return os << state.GetInstanceStateId ();
}

/** McpttOnNetworkFloorArbitratorStateStartStop - begin **/
Ptr<McpttOnNetworkFloorArbitratorStateStartStop>
McpttOnNetworkFloorArbitratorStateStartStop::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorArbitratorStateStartStop> instance = Create<McpttOnNetworkFloorArbitratorStateStartStop> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateStartStop::GetStateId (void)
{
  static McpttEntityId id (0, "'Start-stop'");

  return id;
}

McpttOnNetworkFloorArbitratorStateStartStop::McpttOnNetworkFloorArbitratorStateStartStop (void)
  : McpttOnNetworkFloorArbitratorState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorArbitratorStateStartStop::~McpttOnNetworkFloorArbitratorStateStartStop (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateStartStop::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorArbitratorStateStartStop::GetStateId ();
}

void
McpttOnNetworkFloorArbitratorStateStartStop::CallInitialized (Ptr<McpttOnNetworkFloorArbitrator> machine, Ptr<McpttOnNetworkFloorTowardsParticipant> participant) const
{
  NS_LOG_FUNCTION (this << machine << participant);
  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " CallInitialized()");

  //TODO: Check if a confirmed indication is required and at least one invited
  //      MCPTT client has accepted the invitation; or check that a confirmed
  //      indication is not required; or If the initialized MCPTT call is a
  //      temporary group session.

  //TODO: Wait for the 'basic floor control operation towards the floor
  //      participant' to be initialized.

  if (machine->GetOwner ()->IsTemporaryGroup ())
    {
      NS_LOG_DEBUG ("machine IsTemporaryGroup () is true");
      McpttOnNetworkFloorArbitratorStateInitialising::GetInstance ()->Enter (machine);
    }
  else
    {
      if (participant->IsImplicitRequest ())
        {
          NS_LOG_DEBUG ("IsImplicitRequest is true");
          McpttFloorMsgRequest msg;
          msg.SetSsrc (participant->GetStoredSsrc ());
          msg.SetPriority (McpttFloorMsgFieldPriority (participant->GetStoredPriority ()));
          McpttOnNetworkFloorArbitratorStateIdle::GetInstance ()->ReceiveFloorRequest (machine, msg);
        }
      else
        {
          NS_LOG_DEBUG ("IsImplicitRequest is false");
          McpttOnNetworkFloorArbitratorStateIdle::GetInstance ()->Enter (machine);
        }
    }
}
/** McpttOnNetworkFloorArbitratorStateStartStop - end **/

/** McpttOnNetworkFloorArbitratorStateIdle - begin **/
Ptr<McpttOnNetworkFloorArbitratorStateIdle>
McpttOnNetworkFloorArbitratorStateIdle::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorArbitratorStateIdle> instance = Create<McpttOnNetworkFloorArbitratorStateIdle> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateIdle::GetStateId (void)
{
  static McpttEntityId id (1, "'G: Floor Idle'");

  return id;
}

McpttOnNetworkFloorArbitratorStateIdle::McpttOnNetworkFloorArbitratorStateIdle (void)
  : McpttOnNetworkFloorArbitratorState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorArbitratorStateIdle::~McpttOnNetworkFloorArbitratorStateIdle (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateIdle::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorArbitratorStateIdle::GetStateId ();
}

void
McpttOnNetworkFloorArbitratorStateIdle::Enter (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this);

  if (machine->GetStateId () != McpttOnNetworkFloorArbitratorStateStartStop::GetStateId ()
      && !machine->IsDualFloor ())
    {
      machine->SetTrackInfo (McpttFloorMsgFieldTrackInfo ());
      if (machine->GetQueue ()->HasNext ())
        {
          McpttQueuedUserInfo queueInfo = machine->GetQueue ()->Dequeue ();
          machine->SetStoredSsrc (queueInfo.GetSsrc ());
          machine->SetStoredPriority (queueInfo.GetInfo ().GetPriority ());
          McpttOnNetworkFloorArbitratorStateTaken::GetInstance ()->Enter (machine);
        }
      else
        {
          McpttFloorMsgIdle idleMsg;
          idleMsg.SetSsrc (machine->GetTxSsrc ());
          idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->NextSeqNum ()));
          idleMsg.SetIndicator (machine->GetIndicator ());
          machine->SendToAll (idleMsg);
          machine->GetC7 ()->Reset ();
          machine->GetT7 ()->Start ();
          machine->GetT4 ()->Start ();
          machine->ChangeState (McpttOnNetworkFloorArbitratorStateIdle::GetInstance ());
        }
    }
  else if (machine->GetStateId () != McpttOnNetworkFloorArbitratorStateStartStop::GetStateId ()
           && machine->IsDualFloor ())
    {
      machine->SetTrackInfo (McpttFloorMsgFieldTrackInfo ());

      McpttFloorMsgIdle idleMsg;
      idleMsg.SetSsrc (machine->GetTxSsrc ());
      idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->NextSeqNum ()));
      idleMsg.SetIndicator (machine->GetIndicator ());
      machine->SendToAllExcept (idleMsg, machine->GetStoredSsrc ());

      machine->GetDualControl ()->Terminate ();

      McpttFloorMsgTaken takenMsg;
      takenMsg.SetSsrc (machine->GetTxSsrc ());
      takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine->GetDualControl ()->GetStoredSsrc ()));
      takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->NextSeqNum ()));
      machine->SendToAllExcept (takenMsg, machine->GetStoredSsrc ());

      machine->ChangeState (McpttOnNetworkFloorArbitratorStateTaken::GetInstance ());
    }
}

void
McpttOnNetworkFloorArbitratorStateIdle::ReceiveFloorRequest (Ptr<McpttOnNetworkFloorArbitrator> machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << machine << msg);

  bool isOnlyParticipant = machine->GetNParticipants () == 1;
  bool isRxOnly = machine->GetParticipantBySsrc (msg.GetSsrc ())->IsReceiveOnly ();

  if (isOnlyParticipant == true
      || isRxOnly == true)
    {
      McpttFloorMsgDeny denyMsg;
      denyMsg.SetSsrc (machine->GetTxSsrc ());
      if (isOnlyParticipant == true)
        {
          denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_3));
        }
      else if (isRxOnly == true)
        {
          denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_5));
        }
      denyMsg.UpdateTrackInfo (msg.GetTrackInfo ());
      machine->SendTo (denyMsg, msg.GetSsrc ());
    }
  else
    {
      if (machine->GetT4 ()->IsRunning ())
        {
          machine->GetT4 ()->Stop ();
        }
      if (machine->GetT7 ()->IsRunning ())
        {
          machine->GetT7 ()->Stop ();
        }
      machine->SetStoredSsrc (msg.GetSsrc ());
      machine->SetStoredPriority (msg.GetPriority ().GetPriority ());
      machine->SetTrackInfo (msg.GetTrackInfo ());
      McpttOnNetworkFloorArbitratorStateTaken::GetInstance ()->Enter (machine);
    }
}

void
McpttOnNetworkFloorArbitratorStateIdle::ExpiryOfT7 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << GetInstanceStateId ().GetName () << machine);

  if (machine->GetC7 ()->IsLimitReached ())
    {
      McpttFloorMsgIdle idleMsg;
      idleMsg.SetSsrc (machine->GetTxSsrc ());
      idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->NextSeqNum ()));
      machine->SendToAll (idleMsg);
    }
  else
    {
      machine->GetT7 ()->Start ();
      machine->GetC7 ()->Increment ();
    }
}

void
McpttOnNetworkFloorArbitratorStateIdle::ExpiryOfT4 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  //TODO: Shall indicate to the application and signaling plane that timer T4 has expired
  //TODO: If the application and signaling plane initiates MCPTT call release,
  //      shall enter the 'Releasing' state
  //      otherwise:
  machine->GetT4 ()->Start ();
}
/** McpttOnNetworkFloorArbitratorStateIdle - end **/

/** McpttOnNetworkFloorArbitratorStateTaken - begin **/
Ptr<McpttOnNetworkFloorArbitratorStateTaken>
McpttOnNetworkFloorArbitratorStateTaken::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorArbitratorStateTaken> instance = Create<McpttOnNetworkFloorArbitratorStateTaken> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateTaken::GetStateId (void)
{
  static McpttEntityId id (2, "'G: Floor Taken'");

  return id;
}

McpttOnNetworkFloorArbitratorStateTaken::McpttOnNetworkFloorArbitratorStateTaken (void)
  : McpttOnNetworkFloorArbitratorState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorArbitratorStateTaken::~McpttOnNetworkFloorArbitratorStateTaken (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateTaken::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorArbitratorStateTaken::GetStateId ();
}

bool
McpttOnNetworkFloorArbitratorStateTaken::IsFloorOccupied (void) const
{
  return true;
}

void
McpttOnNetworkFloorArbitratorStateTaken::Enter (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  McpttFloorMsgGranted grantedMsg;
  grantedMsg.SetSsrc (machine->GetTxSsrc ());
  grantedMsg.SetDuration (McpttFloorMsgFieldDuration (machine->GetT2 ()->GetTimeLeft ().GetSeconds ()));
  grantedMsg.SetPriority (McpttFloorMsgFieldPriority (machine->GetStoredPriority ()));
  grantedMsg.UpdateTrackInfo (machine->GetTrackInfo ());
  grantedMsg.SetIndicator (machine->GetIndicator ());
  machine->SendTo (grantedMsg, machine->GetStoredSsrc ());

  machine->GetC20 ()->Reset ();
  machine->GetT20 ()->Start ();

  McpttFloorMsgTaken takenMsg;
  takenMsg.SetSsrc (machine->GetTxSsrc ());
  takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine->GetStoredSsrc ()));
  takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine->NextSeqNum ()));

  if (machine->GetOwner ()->GetCallMachine ()->GetCallType ().GetType () == McpttCallMsgFieldCallType::BROADCAST_GROUP)
    {
      takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (0));
    }
  else
    {
      takenMsg.SetPermission (McpttFloorMsgFieldPermToReq (1));
    }

  takenMsg.SetIndicator (machine->GetIndicator ());
  machine->SendToAllExcept (takenMsg, machine->GetStoredSsrc ());
  machine->GetT1 ()->Start ();
  machine->ChangeState (McpttOnNetworkFloorArbitratorStateTaken::GetInstance ());
}

void
McpttOnNetworkFloorArbitratorStateTaken::ExpiryOfT1 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  if (machine->GetT2 ()->IsRunning ())
    {
      machine->GetT2 ()->Stop ();
    }

  if (machine->GetT20 ()->IsRunning ())
    {
      machine->GetT20 ()->Stop ();
    }

  //TODO: Shall request the media distributor in the MCPTT server to stop
  //      distributing RTP media packets (with the expception of RTP media
  //      packets already in the buffer (if RTP media buffering is ongoing))
  //      to other MCPTT clients
  McpttOnNetworkFloorArbitratorStateIdle::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorArbitratorStateTaken::ExpiryOfT2 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  machine->GetT1 ()->Stop ();

  //TODO: Communicate to the revoke state that Cause 2 should be sent
  McpttOnNetworkFloorArbitratorStateRevoke::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorArbitratorStateTaken::ReceiveMedia (Ptr<McpttOnNetworkFloorArbitrator> machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << machine << msg);

  if (machine->GetStoredSsrc () == msg.GetSsrc ())
    {
      if (!machine->GetT2 ()->IsRunning ())
        {
          machine->GetT2 ()->Start ();
        }

      machine->GetT1 ()->Restart ();

      if (machine->GetT20 ()->IsRunning ())
        {
          machine->GetT20 ()->Stop ();
        }

      //TODO: shall instruct the media distibutor to foward the RTP media packets
      //      to MCPTT clients according to local policy
      McpttMediaMsg copy = msg;
      machine->SendToAllExcept (copy, copy.GetSsrc ());
    }
  else //TODO: Not in standard
    {
      if (machine->GetDualControl ()->IsStarted ())
        {
          machine->GetDualControl ()->ReceiveMedia (msg);
        }
    }
}

void
McpttOnNetworkFloorArbitratorStateTaken::ReceiveFloorRelease (Ptr<McpttOnNetworkFloorArbitrator> machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << machine << msg);

  if (machine->GetStoredSsrc () == msg.GetSsrc ())
    {
      //TODO: Request the media distributor in the MCPTT server to stop fowarding
      //      RTP media packets

      //TODO: Not in standard - stop T1 if running
      if (machine->GetT1 ()->IsRunning ())
        {
          machine->GetT1 ()->Stop ();
        }

      if (machine->GetT2 ()->IsRunning ())
        {
          machine->GetT2 ()->Stop ();
        }

      if (machine->GetT20 ()->IsRunning ())
        {
          machine->GetT20 ()->Stop ();
        }

      McpttOnNetworkFloorArbitratorStateIdle::GetInstance ()->Enter (machine);
    }
}

void
McpttOnNetworkFloorArbitratorStateTaken::ReceiveFloorRequest (Ptr<McpttOnNetworkFloorArbitrator> machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << machine << msg);

  if (msg.GetSsrc () == machine->GetStoredSsrc ())
    {
      McpttFloorMsgGranted grantedMsg;
      grantedMsg.SetSsrc (machine->GetTxSsrc ());
      grantedMsg.SetDuration (McpttFloorMsgFieldDuration (machine->GetT2 ()->GetTimeLeft ().GetSeconds ()));
      grantedMsg.SetPriority (McpttFloorMsgFieldPriority (machine->GetStoredPriority ()));
      grantedMsg.UpdateTrackInfo (msg.GetTrackInfo ());
      machine->SendTo (grantedMsg, machine->GetStoredSsrc ());
    }
  else if (machine->IsPreemptive (msg) == true)
    {
      if (machine->IsDualFloorSupported ())
        {
          machine->GetDualControl ()->ReceivePreemptiveFloorRequest (msg);
        }
      else if (machine->IsAudioCutIn ())
        {
          if (machine->GetT1 ()->IsRunning ())
            {
              machine->GetT1 ()->Stop ();
            }

          //TODO: Not in standard - stop T2 if running.
          if (machine->GetT2 ()->IsRunning ())
            {
              machine->GetT2 ()->Stop ();
            }

          if (machine->GetT20 ()->IsRunning ())
            {
              machine->GetT20 ()->Stop ();
            }

          machine->SetRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_4);

          McpttFloorMsgFieldQueuedUserId queuedUserId;
          queuedUserId.SetUserId (msg.GetSsrc ());
          McpttFloorMsgFieldQueuePositionInfo queueInfo (0, msg.GetPriority ().GetPriority ());
          McpttQueuedUserInfo userInfo (msg.GetSsrc (), queuedUserId, queueInfo);

          machine->GetQueue ()->Pull (msg.GetSsrc ());
          machine->GetQueue ()->Enqueue (userInfo);

          if (machine->IsQueueingSupported ()
              && machine->GetParticipantBySsrc (msg.GetSsrc ())->IsQueueing ())
            {
              McpttFloorMsgQueuePositionInfo queuedMsg;
              queuedMsg.SetSsrc (machine->GetTxSsrc ());
              queuedMsg.SetQueuePositionInfo (queueInfo);
              queuedMsg.UpdateTrackInfo (msg.GetTrackInfo ());
              machine->SendTo (queuedMsg, msg.GetSsrc ());
            }

          McpttOnNetworkFloorArbitratorStateRevoke::GetInstance ()->Enter (machine);
        }
    }
}

void
McpttOnNetworkFloorArbitratorStateTaken::ExpiryOfT20 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  if (!machine->GetC20 ()->IsLimitReached ())
    {
      McpttFloorMsgGranted grantedMsg;
      grantedMsg.SetSsrc (machine->GetTxSsrc ());
      grantedMsg.SetDuration (McpttFloorMsgFieldDuration (machine->GetT2 ()->GetTimeLeft ().GetSeconds ()));
      grantedMsg.SetPriority (McpttFloorMsgFieldPriority (machine->GetStoredPriority ()));
      grantedMsg.SetIndicator (machine->GetIndicator ());
      machine->SendTo (grantedMsg, machine->GetStoredSsrc ());

      machine->GetC20 ()->Increment ();
      machine->GetT20 ()->Start ();
    }
}

void
McpttOnNetworkFloorArbitratorStateTaken::ClientRelease (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this);

  McpttOnNetworkFloorArbitratorStateIdle::GetInstance ()->Enter (machine);
}
/** McpttOnNetworkFloorArbitratorStateTaken - end **/

/** McpttOnNetworkFloorArbitratorStateRevoke - begin **/
Ptr<McpttOnNetworkFloorArbitratorStateRevoke>
McpttOnNetworkFloorArbitratorStateRevoke::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorArbitratorStateRevoke> instance = Create<McpttOnNetworkFloorArbitratorStateRevoke> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateRevoke::GetStateId (void)
{
  static McpttEntityId id (3, "'G: Pending Floor Revoke'");

  return id;
}

McpttOnNetworkFloorArbitratorStateRevoke::McpttOnNetworkFloorArbitratorStateRevoke (void)
  : McpttOnNetworkFloorArbitratorState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorArbitratorStateRevoke::~McpttOnNetworkFloorArbitratorStateRevoke (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateRevoke::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorArbitratorStateRevoke::GetStateId ();
}

bool
McpttOnNetworkFloorArbitratorStateRevoke::IsFloorOccupied (void) const
{
  return true;
}

void
McpttOnNetworkFloorArbitratorStateRevoke::Enter (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  McpttFloorMsgRevoke revokeMsg;
  revokeMsg.SetSsrc (machine->GetTxSsrc ());
  revokeMsg.SetRejCause (McpttFloorMsgFieldRejectCause (machine->GetRejectCause ()));
  revokeMsg.SetIndicator (machine->GetIndicator ());
  machine->SendTo (revokeMsg, machine->GetStoredSsrc ());

  if (machine->IsAudioCutIn ())
    {
      machine->GetT3 ()->StartWith (Seconds (0));
    }
  else
    {
      machine->GetT3 ()->Start ();
    }

  machine->ChangeState (McpttOnNetworkFloorArbitratorStateRevoke::GetInstance ());
}

void
McpttOnNetworkFloorArbitratorStateRevoke::ReceiveMedia (Ptr<McpttOnNetworkFloorArbitrator> machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << machine << msg);

  if (msg.GetSsrc () == machine->GetStoredSsrc ())
    {
      machine->GetT1 ()->Restart ();

      //TODO: Shall instruct the emdia distributor to foward the RTP media
      //      packets to MCPTT clients according to local policy.
      McpttMediaMsg copy = msg;
      machine->SendToAllExcept (copy, copy.GetSsrc ());
    }
  else //TODO: Not in standard
    {
      if (machine->GetDualControl ()->IsStarted ())
        {
          machine->GetDualControl ()->ReceiveMedia (msg);
        }
    }
}

void
McpttOnNetworkFloorArbitratorStateRevoke::ReceiveFloorRelease (Ptr<McpttOnNetworkFloorArbitrator> machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << machine << msg);

  //TODO: Shall request the media distrubtor in the MCPTT server to stop
  //      forwarding RTP media packets.

  if (machine->GetT1 ()->IsRunning ())
    {
      machine->GetT1 ()->Stop ();
    }

  machine->GetT3 ()->Stop ();

  McpttOnNetworkFloorArbitratorStateIdle::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorArbitratorStateRevoke::ExpiryOfT3 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  //TODO: Shall indicate to the interface twoards the MCPTT client that the
  //      general state machine is now in 'G: Floor Idle'

  McpttOnNetworkFloorArbitratorStateIdle::GetInstance ()->Enter (machine);
}

void
McpttOnNetworkFloorArbitratorStateRevoke::ExpiryOfT1 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  machine->GetT3 ()->Stop ();

  McpttOnNetworkFloorArbitratorStateIdle::GetInstance ()->Enter (machine);
}
/** McpttOnNetworkFloorArbitratorStateRevoke - end **/

/** McpttOnNetworkFloorArbitratorStateReleasing - begin **/
Ptr<McpttOnNetworkFloorArbitratorStateReleasing>
McpttOnNetworkFloorArbitratorStateReleasing::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorArbitratorStateReleasing> instance = Create<McpttOnNetworkFloorArbitratorStateReleasing> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateReleasing::GetStateId (void)
{
  static McpttEntityId id (4, "'Releasing'");

  return id;
}

McpttOnNetworkFloorArbitratorStateReleasing::McpttOnNetworkFloorArbitratorStateReleasing (void)
  : McpttOnNetworkFloorArbitratorState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorArbitratorStateReleasing::~McpttOnNetworkFloorArbitratorStateReleasing (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateReleasing::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorArbitratorStateReleasing::GetStateId ();
}

void
McpttOnNetworkFloorArbitratorStateReleasing::CallRelease2 (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this);

  //TODO: Shall release all resources reserved in the media plane.

  machine->ChangeState (McpttOnNetworkFloorArbitratorStateStartStop::GetInstance ());
}
/** McpttOnNetworkFloorArbitratorStateReleasing - end **/

/** McpttOnNetworkFloorArbitratorStateInitialising - begin **/
Ptr<McpttOnNetworkFloorArbitratorStateInitialising>
McpttOnNetworkFloorArbitratorStateInitialising::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorArbitratorStateInitialising> instance = Create<McpttOnNetworkFloorArbitratorStateInitialising> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateInitialising::GetStateId (void)
{
  static McpttEntityId id (5, "'G: Floor Initialising'");

  return id;
}

McpttOnNetworkFloorArbitratorStateInitialising::McpttOnNetworkFloorArbitratorStateInitialising (void)
  : McpttOnNetworkFloorArbitratorState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorArbitratorStateInitialising::~McpttOnNetworkFloorArbitratorStateInitialising (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorArbitratorStateInitialising::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorArbitratorStateInitialising::GetStateId ();
}

void
McpttOnNetworkFloorArbitratorStateInitialising::Enter (Ptr<McpttOnNetworkFloorArbitrator> machine) const
{
  NS_LOG_FUNCTION (this << machine);

  NS_ABORT_MSG ("'G: Floor Initialising' state not fully supported");
  //TODO: Still need the following sections implemented for the 'G: Initialising' state
  //        - 6.3.4.8.3 Receiving a floor request from a constituent MCPTT group (R: mcptt-floor-request)
  //        - 6.3.4.8.4 All final SIP responses received (R: final SIP responses)
}

} //namespace psc
} //namespace ns3

