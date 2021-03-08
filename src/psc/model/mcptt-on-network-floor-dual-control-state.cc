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
#include "mcptt-on-network-floor-arbitrator.h"
#include "mcptt-on-network-floor-arbitrator-state.h"
#include "mcptt-on-network-floor-dual-control.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-media-src.h"
#include "mcptt-ptt-app.h"

#include "mcptt-on-network-floor-dual-control-state.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttOnNetworkFloorDualControlState");

namespace psc {

/** McpttOnNetworkFloorDualControlState - begin **/
McpttOnNetworkFloorDualControlState::~McpttOnNetworkFloorDualControlState (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkFloorDualControlState::CallRelease1 (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this);

  //TODO: Shall request the media distributor in the MCPTT server to stop
  //      rendering RTP media packets

  machine.ChangeState (McpttOnNetworkFloorDualControlStateReleasing::GetInstance ());
}

void
McpttOnNetworkFloorDualControlState::CallRelease2 (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring call release notification (part II).");
}

void
McpttOnNetworkFloorDualControlState::Enter (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring enter.");
}

void
McpttOnNetworkFloorDualControlState::ExpiryOfT11 (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T11.");
}

void
McpttOnNetworkFloorDualControlState::ExpiryOfT12 (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T12.");
}

McpttEntityId
McpttOnNetworkFloorDualControlState::GetInstanceStateId (void) const
{
  return McpttEntityId ();
}

void
McpttOnNetworkFloorDualControlState::ReceiveFloorRelease (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorDualControlState::ReceiveFloorRequest (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorDualControlState::ReceiveMedia (McpttOnNetworkFloorDualControl& machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorDualControlState::ReceivePreemptiveFloorRequest (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorDualControlState::Selected (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

void
McpttOnNetworkFloorDualControlState::ClientRelease (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring client release notification.");
}

void
McpttOnNetworkFloorDualControlState::Terminate (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring client release notification.");
}

void
McpttOnNetworkFloorDualControlState::Unselected (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

McpttOnNetworkFloorDualControlState::McpttOnNetworkFloorDualControlState (void)
  : SimpleRefCount<McpttOnNetworkFloorDualControlState> ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttOnNetworkFloorDualControlState - end **/

std::ostream&
operator<< (std::ostream& os, const McpttOnNetworkFloorDualControlState& state)
{
  return os << state.GetInstanceStateId ();
}

/** McpttOnNetworkFloorDualControlStateStartStop - begin **/
Ptr<McpttOnNetworkFloorDualControlStateStartStop>
McpttOnNetworkFloorDualControlStateStartStop::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorDualControlStateStartStop> instance = Create<McpttOnNetworkFloorDualControlStateStartStop> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorDualControlStateStartStop::GetStateId (void)
{
  static McpttEntityId id (0, "'Start-stop'");

  return id;
}

McpttOnNetworkFloorDualControlStateStartStop::McpttOnNetworkFloorDualControlStateStartStop (void)
  : McpttOnNetworkFloorDualControlState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorDualControlStateStartStop::~McpttOnNetworkFloorDualControlStateStartStop (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorDualControlStateStartStop::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorDualControlStateStartStop::GetStateId ();
}

void
McpttOnNetworkFloorDualControlStateStartStop::ReceivePreemptiveFloorRequest (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  machine.SetStoredSsrc (msg.GetSsrc ());
  machine.SetTrackInfo (msg.GetTrackInfo ());

  McpttOnNetworkFloorDualControlStateTaken::GetInstance ()->Enter (machine);
}

/** McpttOnNetworkFloorDualControlStateStartStop - end **/

/** McpttOnNetworkFloorDualControlStateTaken - begin **/
Ptr<McpttOnNetworkFloorDualControlStateTaken>
McpttOnNetworkFloorDualControlStateTaken::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorDualControlStateTaken> instance = Create<McpttOnNetworkFloorDualControlStateTaken> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorDualControlStateTaken::GetStateId (void)
{
  static McpttEntityId id (1, "'D: Floor Taken'");

  return id;
}

McpttOnNetworkFloorDualControlStateTaken::McpttOnNetworkFloorDualControlStateTaken (void)
  : McpttOnNetworkFloorDualControlState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorDualControlStateTaken::~McpttOnNetworkFloorDualControlStateTaken (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorDualControlStateTaken::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorDualControlStateTaken::GetStateId ();
}

void
McpttOnNetworkFloorDualControlStateTaken::Enter (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  //This must happen first so that the indictator fields G-bit to indicate dual floor is set
  machine.ChangeState (McpttOnNetworkFloorDualControlStateTaken::GetInstance ());

  McpttFloorMsgGranted grantedMsg;
  grantedMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
  grantedMsg.SetDuration (McpttFloorMsgFieldDuration (machine.GetT12 ()->GetTimeLeft ().GetSeconds ()));
  grantedMsg.SetPriority (McpttFloorMsgFieldPriority (machine.GetStoredPriority ()));
  grantedMsg.UpdateTrackInfo (machine.GetTrackInfo ());
  grantedMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
  //TODO: If non-controlling MCPTT functions are involved, shall include the
  //      SSRC of the granted floor participant
  machine.GetOwner ()->SendTo (grantedMsg, machine.GetStoredSsrc ());

  McpttFloorMsgIdle idleMsg;
  idleMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
  idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine.GetOwner ()->NextSeqNum ()));
  idleMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
  machine.GetOwner ()->SendToAllExcept (idleMsg, machine.GetStoredSsrc ());

  McpttFloorMsgTaken takenMsg;
  takenMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
  takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine.GetStoredSsrc ()));
  takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine.GetOwner ()->NextSeqNum ()));
  takenMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
  machine.GetOwner ()->SendToAllExcept (takenMsg, machine.GetStoredSsrc ());
  machine.GetT11 ()->Start ();
}

void
McpttOnNetworkFloorDualControlStateTaken::ExpiryOfT11 (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine.GetT12 ()->IsRunning ())
    {
      machine.GetT12 ()->Stop ();
    }

  //TODO: Shall request the media distributor to stop distributing RTP media
  //      packets received from the overriding MCPTT client to the other MCPTT
  //      clients.

  McpttFloorMsgIdle idleMsg;
  idleMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
  idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine.GetOwner ()->NextSeqNum ()));
  idleMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
  machine.GetOwner ()->SendToAllExcept (idleMsg, machine.GetStoredSsrc ());

  if (machine.GetOwner ()->GetStateId () == McpttOnNetworkFloorArbitratorStateTaken::GetStateId ())
    {
      McpttFloorMsgTaken takenMsg;
      takenMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
      takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine.GetOwner ()->GetStoredSsrc ()));
      takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine.GetOwner ()->NextSeqNum ()));
      takenMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
      machine.GetOwner ()->SendToAllExcept (takenMsg, machine.GetOwner ()->GetStoredSsrc ());
    }
  else if ((machine.GetOwner ()->GetStateId () == McpttOnNetworkFloorArbitratorStateIdle::GetStateId ()))
    {
      McpttFloorMsgIdle idleMsg;
      idleMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
      idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine.GetOwner ()->NextSeqNum ()));
      idleMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
      machine.GetOwner ()->SendToAll (idleMsg);
    }

  machine.ChangeState (McpttOnNetworkFloorDualControlStateStartStop::GetInstance ());
}

void
McpttOnNetworkFloorDualControlStateTaken::ExpiryOfT12 (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine.GetT11 ()->Stop ();

  //TODO: Shall request the media distributor in the MCPTT server to stop
  //      distributing RTP media packets to the other MCPTT clients.

  McpttFloorMsgRevoke revokeMsg;
  revokeMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
  revokeMsg.SetRejCause (McpttFloorMsgFieldRejectCause (McpttFloorMsgFieldRejectCause::CAUSE_2));
  revokeMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
  revokeMsg.UpdateTrackInfo (machine.GetTrackInfo ());
  machine.GetOwner ()->SendTo (revokeMsg, machine.GetStoredSsrc ());

  machine.SetTrackInfo (McpttFloorMsgFieldTrackInfo ());

  if (machine.GetOwner ()->GetStateId () == McpttOnNetworkFloorArbitratorStateTaken::GetStateId ())
    {
      McpttFloorMsgTaken takenMsg;
      takenMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
      takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine.GetOwner ()->GetStoredSsrc ()));
      takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine.GetOwner ()->NextSeqNum ()));
      machine.GetOwner ()->SendToAllExcept (takenMsg, machine.GetOwner ()->GetStoredSsrc ());
    }
  else if ((machine.GetOwner ()->GetStateId () == McpttOnNetworkFloorArbitratorStateIdle::GetStateId ()))
    {
      McpttFloorMsgIdle idleMsg;
      idleMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
      idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine.GetOwner ()->NextSeqNum ()));
      idleMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
      machine.GetOwner ()->SendToAll (idleMsg);
    }

  machine.ChangeState (McpttOnNetworkFloorDualControlStateStartStop::GetInstance ());
}

void
McpttOnNetworkFloorDualControlStateTaken::ReceiveMedia (McpttOnNetworkFloorDualControl& machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine.GetStoredSsrc () == msg.GetSsrc ())
    {
      if (!machine.GetT12 ()->IsRunning ())
        {
          machine.GetT12 ()->Start ();
        }

      machine.GetT11 ()->Restart ();

      //TODO: Shall instruct the media distributor to forward the received RTP
      //      media packets to any non-controlling MCPTT functions to the
      //      overriden MCPTT client and to those MCPTT clients receiving RTP
      //      media from the overriding MCPTT client controlled by the controlling
      //      MCPTT functions according to local policy.

      McpttMediaMsg copy = msg;
      machine.GetOwner ()->SendToAllExcept (copy, copy.GetSsrc ());
    }
}

void
McpttOnNetworkFloorDualControlStateTaken::ReceiveFloorRelease (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  //TODO: Request the media distributor in the MCPTT server to stop
  //      distributing RTP media packets received from the overriding MCPTT
  //      client to other MCPTT clients.

  if (machine.GetT12 ()->IsRunning ())
    {
      machine.GetT12 ()->Stop ();
    }

  machine.GetT11 ()->Stop ();

  if (msg.GetSubtype () == McpttFloorMsgRelease::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::CONTROLLING_FUNCTION));
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgRelease::SUBTYPE));
      machine.GetOwner ()->SendTo (ackMsg, machine.GetStoredSsrc ());
    }

  McpttFloorMsgIdle idleMsg;
  idleMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
  idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine.GetOwner ()->NextSeqNum ()));
  idleMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
  machine.GetOwner ()->SendToAllExcept (idleMsg, machine.GetStoredSsrc ());

  if (machine.GetOwner ()->GetStateId () == McpttOnNetworkFloorArbitratorStateTaken::GetStateId ())
    {
      McpttFloorMsgTaken takenMsg;
      takenMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
      takenMsg.SetPartyId (McpttFloorMsgFieldGrantedPartyId (machine.GetOwner ()->GetStoredSsrc ()));
      takenMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine.GetOwner ()->NextSeqNum ()));
      takenMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
      machine.GetOwner ()->SendToAllExcept (takenMsg, machine.GetOwner ()->GetStoredSsrc ());
    }
  else if ((machine.GetOwner ()->GetStateId () == McpttOnNetworkFloorArbitratorStateIdle::GetStateId ()))
    {
      McpttFloorMsgIdle idleMsg;
      idleMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
      idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine.GetOwner ()->NextSeqNum ()));
      idleMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
      machine.GetOwner ()->SendToAll (idleMsg);
    }

  machine.ChangeState (McpttOnNetworkFloorDualControlStateStartStop::GetInstance ());
}

void
McpttOnNetworkFloorDualControlStateTaken::ReceiveFloorRequest (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine.GetStoredSsrc () == msg.GetSsrc ())
    {
      McpttFloorMsgGranted grantedMsg;
      grantedMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
      grantedMsg.SetDuration (McpttFloorMsgFieldDuration (machine.GetT12 ()->GetTimeLeft ().GetSeconds ()));
      grantedMsg.SetPriority (machine.GetStoredPriority ());
      grantedMsg.UpdateTrackInfo (msg.GetTrackInfo ());
      grantedMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
    }
}

void
McpttOnNetworkFloorDualControlStateTaken::ClientRelease (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this);

  //TODO: Shall request the media distributor in the MCPTT server to stop
  //      distributing RTP media packets received from the overriding MCPTT
  //      client to the other MCPTT clients.

  McpttFloorMsgIdle idleMsg;
  idleMsg.SetSsrc (machine.GetOwner ()->GetTxSsrc ());
  idleMsg.SetSeqNum (McpttFloorMsgFieldSeqNum (machine.GetOwner ()->NextSeqNum ()));
  idleMsg.SetIndicator (machine.GetOwner ()->GetIndicator ());
  machine.GetOwner ()->SendToAllExcept (idleMsg, machine.GetStoredSsrc ());

  machine.ChangeState (McpttOnNetworkFloorDualControlStateStartStop::GetInstance ());
}

void
McpttOnNetworkFloorDualControlStateTaken::Terminate (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this);

  if (machine.GetT11 ()->IsRunning ())
    {
      machine.GetOwner ()->GetT1 ()->StartWith (machine.GetT11 ()->GetTimeLeft ());
    }

  if (machine.GetT12 ()->IsRunning ())
    {
      machine.GetOwner ()->GetT2 ()->StartWith (machine.GetT12 ()->GetTimeLeft ());
    }

  machine.GetOwner ()->SetStoredSsrc (machine.GetStoredSsrc ());
  machine.GetOwner ()->SetStoredPriority (machine.GetStoredPriority ());

  machine.ChangeState (McpttOnNetworkFloorDualControlStateStartStop::GetInstance ());
}

/** McpttOnNetworkFloorDualControlStateTaken - end **/

/** McpttOnNetworkFloorDualControlStateReleasing - begin **/
Ptr<McpttOnNetworkFloorDualControlStateReleasing>
McpttOnNetworkFloorDualControlStateReleasing::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorDualControlStateReleasing> instance = Create<McpttOnNetworkFloorDualControlStateReleasing> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorDualControlStateReleasing::GetStateId (void)
{
  static McpttEntityId id (2, "'Releasing'");

  return id;
}

McpttOnNetworkFloorDualControlStateReleasing::McpttOnNetworkFloorDualControlStateReleasing (void)
  : McpttOnNetworkFloorDualControlState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorDualControlStateReleasing::~McpttOnNetworkFloorDualControlStateReleasing (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorDualControlStateReleasing::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorDualControlStateReleasing::GetStateId ();
}

void
McpttOnNetworkFloorDualControlStateReleasing::CallRelease2 (McpttOnNetworkFloorDualControl& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine.ChangeState (McpttOnNetworkFloorDualControlStateStartStop::GetInstance ());
}
/** McpttOnNetworkFloorDualControlStateReleasing - end **/

} //namespace psc
} //namespace ns3

