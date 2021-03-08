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

#include <ns3/abort.h>
#include <ns3/assert.h>
#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg-field.h"
#include "mcptt-entity-id.h"
#include "mcptt-on-network-floor-participant.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-media-src.h"
#include "mcptt-ptt-app.h"

#include "mcptt-on-network-floor-participant-state.h"

namespace ns3 {

namespace psc {

/** McpttOnNetworkFloorParticipantState - begin **/
NS_LOG_COMPONENT_DEFINE ("McpttOnNetworkFloorParticipantState");

McpttOnNetworkFloorParticipantState::~McpttOnNetworkFloorParticipantState (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkFloorParticipantState::AcceptGrant (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring accept grant.");
}

void
McpttOnNetworkFloorParticipantState::ExpiryOfT100 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T100.");
}

void
McpttOnNetworkFloorParticipantState::ExpiryOfT101 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T101.");
}

void
McpttOnNetworkFloorParticipantState::ExpiryOfT103 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T103.");
}

void
McpttOnNetworkFloorParticipantState::ExpiryOfT104 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T104.");
}

void
McpttOnNetworkFloorParticipantState::ExpiryOfT132 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T132.");
}

McpttEntityId
McpttOnNetworkFloorParticipantState::GetInstanceStateId (void) const
{
  return McpttEntityId ();
}

bool
McpttOnNetworkFloorParticipantState::HasFloor (const McpttOnNetworkFloorParticipant& machine) const
{
  return false;
}

void
McpttOnNetworkFloorParticipantState::MediaReady (McpttOnNetworkFloorParticipant& machine, McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_ABORT_MSG (GetInstanceStateId ().GetName () << "(" << this << ")" << " should not be sending media.");
}

void
McpttOnNetworkFloorParticipantState::CallInitiated (McpttOnNetworkFloorParticipant& machine) const
{
  NS_ABORT_MSG (GetInstanceStateId ().GetName () << "(" << this << ")" << " should not be initiating call.");
}

void
McpttOnNetworkFloorParticipantState::CallEstablished (McpttOnNetworkFloorParticipant& machine) const
{
  NS_ABORT_MSG (GetInstanceStateId ().GetName () << "(" << this << ")" << " should not be establishing call.");
}

void
McpttOnNetworkFloorParticipantState::CallRelease1 (McpttOnNetworkFloorParticipant& machine) const
{
  // Section 6.2.4.7.2
  if ((GetInstanceStateId () == McpttOnNetworkFloorParticipantStateStartStop::GetInstance ()->GetInstanceStateId ())
      || (GetInstanceStateId () == McpttOnNetworkFloorParticipantStateReleasing::GetInstance ()->GetInstanceStateId ()))
    {
      NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring call release (part I).");
    }
  else
    {
      // stop sending floor control and stop sending media should occur
      // as part of state change (Unselected ())
      machine.ChangeState (McpttOnNetworkFloorParticipantStateReleasing::GetInstance ());
    }
}

void
McpttOnNetworkFloorParticipantState::CallRelease2 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring call release (part II).");
}

void
McpttOnNetworkFloorParticipantState::ReceiveFloorAck (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgAck& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorParticipantState::ReceiveFloorDeny (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorParticipantState::ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorParticipantState::ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorParticipantState::ReceiveFloorQueuePositionInfo (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgQueuePositionInfo& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorParticipantState::ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorParticipantState::ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorParticipantState::ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttOnNetworkFloorParticipantState::SendFloorQueuePositionRequest (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring floor queue position request.");
}

void
McpttOnNetworkFloorParticipantState::ReleaseRequest (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring release request.");
}

void
McpttOnNetworkFloorParticipantState::Selected (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

void
McpttOnNetworkFloorParticipantState::PttPush (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring push notification.");
}

void
McpttOnNetworkFloorParticipantState::PttRelease (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring release notification.");
}

void
McpttOnNetworkFloorParticipantState::Unselected (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

std::string
McpttOnNetworkFloorParticipantState::GetName () const
{
  return GetInstanceStateId ().GetName ();
}

McpttOnNetworkFloorParticipantState::McpttOnNetworkFloorParticipantState (void)
  : SimpleRefCount<McpttOnNetworkFloorParticipantState> ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttOnNetworkFloorParticipantState - end **/

std::ostream&
operator<< (std::ostream& os, const McpttOnNetworkFloorParticipantState& state)
{
  return os << state.GetInstanceStateId ();
}

/** McpttOnNetworkFloorParticipantStateStartStop - begin **/
Ptr<McpttOnNetworkFloorParticipantStateStartStop>
McpttOnNetworkFloorParticipantStateStartStop::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorParticipantStateStartStop> instance = Create<McpttOnNetworkFloorParticipantStateStartStop> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorParticipantStateStartStop::GetStateId (void)
{
  static McpttEntityId id (0, "'Start-stop'");

  return id;
}

McpttOnNetworkFloorParticipantStateStartStop::McpttOnNetworkFloorParticipantStateStartStop (void)
  : McpttOnNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorParticipantStateStartStop::~McpttOnNetworkFloorParticipantStateStartStop (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorParticipantStateStartStop::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorParticipantStateStartStop::GetStateId ();
}

void
McpttOnNetworkFloorParticipantStateStartStop::PttPush (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
  machine.ReportEvent (McpttFloorParticipant::PTT_BUTTON_PUSHED);
}

void
McpttOnNetworkFloorParticipantStateStartStop::CallEstablished (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this);

  if (!machine.IsOriginator ())
    {
      machine.ChangeState (McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstance ());
    }
}

void
McpttOnNetworkFloorParticipantStateStartStop::CallInitiated (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this);

  if (machine.IsOriginator ())
    {
      if (!machine.IsImplicitRequest ())
        {
          Ptr<Packet> storedMsgs = machine.GetStoredMsgs ();
          storedMsgs->RemoveAtStart (storedMsgs->GetSize ());
          machine.ChangeState (McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstance ());
        }
      else
        {
          machine.GetC101 ()->Reset ();
          machine.GetT101 ()->Start ();
          machine.ReportEvent (McpttFloorParticipant::PTT_BUTTON_PUSHED);
          machine.ChangeState (McpttOnNetworkFloorParticipantStatePendingRequest::GetInstance ());
        }
    }
}

void
McpttOnNetworkFloorParticipantStateStartStop::ReceiveFloorAck (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgAck& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine.IsOriginator ())
    {
      NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " storing " << msg.GetInstanceTypeId () << ".");

      machine.GetStoredMsgs ()->AddHeader (msg);
    }
}

void
McpttOnNetworkFloorParticipantStateStartStop::ReceiveFloorDeny (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine.IsOriginator ())
    {
      NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " storing " << msg.GetInstanceTypeId () << ".");

      machine.GetStoredMsgs ()->AddHeader (msg);
    }
}

void
McpttOnNetworkFloorParticipantStateStartStop::ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine.IsOriginator ())
    {
      NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " storing " << msg.GetInstanceTypeId () << ".");

      machine.GetStoredMsgs ()->AddHeader (msg);
    }
}

void
McpttOnNetworkFloorParticipantStateStartStop::ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine.IsOriginator ())
    {
      NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " storing " << msg.GetInstanceTypeId () << ".");

      machine.GetStoredMsgs ()->AddHeader (msg);
    }
}

void
McpttOnNetworkFloorParticipantStateStartStop::ReceiveFloorQueuePositionInfo (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgQueuePositionInfo& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_QUEUE);
  if (machine.IsOriginator ())
    {
      NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " storing " << msg.GetInstanceTypeId () << ".");

      machine.GetStoredMsgs ()->AddHeader (msg);
    }
}

void
McpttOnNetworkFloorParticipantStateStartStop::ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine.IsOriginator ())
    {
      NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " storing " << msg.GetInstanceTypeId () << ".");

      machine.GetStoredMsgs ()->AddHeader (msg);
    }
}

void
McpttOnNetworkFloorParticipantStateStartStop::ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine.IsOriginator ())
    {
      NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " storing " << msg.GetInstanceTypeId () << ".");

      machine.GetStoredMsgs ()->AddHeader (msg);
    }
}

void
McpttOnNetworkFloorParticipantStateStartStop::ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine.IsOriginator ())
    {
      NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " storing " << msg.GetInstanceTypeId () << ".");

      machine.GetStoredMsgs ()->AddHeader (msg);
    }
}
/** McpttOnNetworkFloorParticipantStateStartStop - end **/

/** McpttOnNetworkFloorParticipantStateHasNoPermission - begin **/
Ptr<McpttOnNetworkFloorParticipantStateHasNoPermission>
McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorParticipantStateHasNoPermission> instance = Create<McpttOnNetworkFloorParticipantStateHasNoPermission> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorParticipantStateHasNoPermission::GetStateId (void)
{
  static McpttEntityId id (1, "'U: has no permission'");

  return id;
}

McpttOnNetworkFloorParticipantStateHasNoPermission::McpttOnNetworkFloorParticipantStateHasNoPermission (void)
  : McpttOnNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorParticipantStateHasNoPermission::~McpttOnNetworkFloorParticipantStateHasNoPermission (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorParticipantStateHasNoPermission::GetStateId ();
}

void
McpttOnNetworkFloorParticipantStateHasNoPermission::ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgIdle::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgIdle::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_IDLE);

  if (machine.GetT103 ()->IsRunning ())
    {
      machine.GetT103 ()->Stop ();
    }
}

void
McpttOnNetworkFloorParticipantStateHasNoPermission::ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgTaken::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgTaken::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_TAKEN);

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
    {
      //TODO: Provide notification to user indicating the type of call.
    }

  // It is possible for this timer to be running due to previous activity
  // of another MCPTT user, so issue Restart() instead of Start()
  machine.GetT103 ()->Restart ();
}

void
McpttOnNetworkFloorParticipantStateHasNoPermission::ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  //TODO: Request MCPTT client to start rendering received RTP media packets.

  machine.GetT103 ()->Restart ();
}

void
McpttOnNetworkFloorParticipantStateHasNoPermission::PttPush (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine.ReportEvent (McpttFloorParticipant::PTT_BUTTON_PUSHED);
  McpttFloorMsgRequest reqMsg (machine.GetTxSsrc ());
  reqMsg.SetPriority (McpttFloorMsgFieldPriority (machine.GetPriority ()));
  reqMsg.SetIndicator (machine.GetIndicator ());
  machine.Send (reqMsg);

  machine.GetC101 ()->Reset ();
  machine.GetT101 ()->Start ();

  machine.ChangeState (McpttOnNetworkFloorParticipantStatePendingRequest::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStateHasNoPermission::ExpiryOfT103 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine.ReportEvent (McpttFloorParticipant::TIMER_T103_EXPIRED);
}

void
McpttOnNetworkFloorParticipantStateHasNoPermission::Selected (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttPttApp> app = machine.GetCall ()->GetOwner ();

  if (app->IsPushed ())
    {
      app->NotifyReleased ();
    }
}
/** McpttOnNetworkFloorParticipantStateHasNoPermission - end **/

/** McpttOnNetworkFloorParticipantStatePendingRequest - begin **/
Ptr<McpttOnNetworkFloorParticipantStatePendingRequest>
McpttOnNetworkFloorParticipantStatePendingRequest::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorParticipantStatePendingRequest> instance = Create<McpttOnNetworkFloorParticipantStatePendingRequest> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorParticipantStatePendingRequest::GetStateId (void)
{
  static McpttEntityId id (2, "'U: pending Request'");

  return id;
}

McpttOnNetworkFloorParticipantStatePendingRequest::McpttOnNetworkFloorParticipantStatePendingRequest (void)
  : McpttOnNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorParticipantStatePendingRequest::~McpttOnNetworkFloorParticipantStatePendingRequest (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorParticipantStatePendingRequest::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorParticipantStatePendingRequest::GetStateId ();
}

void
McpttOnNetworkFloorParticipantStatePendingRequest::Selected (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  McpttFloorMsg temp;
  Ptr<Packet> storedMsgs = Create<Packet>();
  Ptr<Packet> reversedStoredMsgs = machine.GetStoredMsgs ();

  while (reversedStoredMsgs->GetSize () > 0)
    {
      reversedStoredMsgs->PeekHeader (temp);
      uint8_t subtype = temp.GetSubtype ();

      if (subtype == McpttFloorMsgRequest::SUBTYPE)
        {
          McpttFloorMsgRequest reqMsg;
          reversedStoredMsgs->RemoveHeader (reqMsg);
          storedMsgs->AddHeader (reqMsg);
        }
      else if (subtype == McpttFloorMsgGranted::SUBTYPE
               || subtype == McpttFloorMsgGranted::SUBTYPE_ACK)
        {
          McpttFloorMsgGranted grantedMsg;
          reversedStoredMsgs->RemoveHeader (grantedMsg);
          storedMsgs->AddHeader (grantedMsg);
        }
      else if (subtype == McpttFloorMsgDeny::SUBTYPE
               || subtype == McpttFloorMsgDeny::SUBTYPE_ACK)
        {
          McpttFloorMsgDeny denyMsg;
          reversedStoredMsgs->RemoveHeader (denyMsg);
          storedMsgs->AddHeader (denyMsg);
        }
      else if (subtype == McpttFloorMsgRelease::SUBTYPE
               || subtype == McpttFloorMsgRelease::SUBTYPE_ACK)
        {
          McpttFloorMsgRelease releaseMsg;
          reversedStoredMsgs->RemoveHeader (releaseMsg);
          storedMsgs->AddHeader (releaseMsg);
        }
      else if (subtype == McpttFloorMsgRevoke::SUBTYPE)
        {
          McpttFloorMsgRevoke revokeMsg;
          reversedStoredMsgs->RemoveHeader (revokeMsg);
          storedMsgs->AddHeader (revokeMsg);
        }
      else if (subtype == McpttFloorMsgIdle::SUBTYPE
               || subtype == McpttFloorMsgIdle::SUBTYPE_ACK)
        {
          McpttFloorMsgIdle takenMsg;
          reversedStoredMsgs->RemoveHeader (takenMsg);
          storedMsgs->AddHeader (takenMsg);
        }
      else if (subtype == McpttFloorMsgTaken::SUBTYPE
               || subtype == McpttFloorMsgTaken::SUBTYPE_ACK)
        {
          McpttFloorMsgTaken takenMsg;
          reversedStoredMsgs->RemoveHeader (takenMsg);
          storedMsgs->AddHeader (takenMsg);
        }
      else if (subtype == McpttFloorMsgQueuePositionRequest::SUBTYPE)
        {
          McpttFloorMsgQueuePositionRequest queuePosReqMsg;
          reversedStoredMsgs->RemoveHeader (queuePosReqMsg);
          storedMsgs->AddHeader (queuePosReqMsg);
        }
      else if (subtype == McpttFloorMsgQueuePositionInfo::SUBTYPE
               || subtype == McpttFloorMsgQueuePositionInfo::SUBTYPE_ACK)
        {
          McpttFloorMsgQueuePositionInfo queueInfoMsg;
          reversedStoredMsgs->RemoveHeader (queueInfoMsg);
          storedMsgs->AddHeader (queueInfoMsg);
        }
      else if (subtype == McpttFloorMsgAck::SUBTYPE)
        {
          McpttFloorMsgAck queueInfoMsg;
          reversedStoredMsgs->RemoveHeader (queueInfoMsg);
          storedMsgs->AddHeader (queueInfoMsg);
        }
      else
        {
          NS_FATAL_ERROR ("Could not resolve message subtype = " << (uint32_t)subtype << ".");
        }

    }

  while (storedMsgs->GetSize () > 0)
    {
      storedMsgs->PeekHeader (temp);
      uint8_t subtype = temp.GetSubtype ();

      if (subtype == McpttFloorMsgRequest::SUBTYPE)
        {
          McpttFloorMsgRequest reqMsg;
          storedMsgs->RemoveHeader (reqMsg);
          machine.Receive (reqMsg);
        }
      else if (subtype == McpttFloorMsgGranted::SUBTYPE
               || subtype == McpttFloorMsgGranted::SUBTYPE_ACK)
        {
          McpttFloorMsgGranted grantedMsg;
          storedMsgs->RemoveHeader (grantedMsg);
          machine.Receive (grantedMsg);
        }
      else if (subtype == McpttFloorMsgDeny::SUBTYPE
               || subtype == McpttFloorMsgDeny::SUBTYPE_ACK)
        {
          McpttFloorMsgDeny denyMsg;
          storedMsgs->RemoveHeader (denyMsg);
          machine.Receive (denyMsg);
        }
      else if (subtype == McpttFloorMsgRelease::SUBTYPE
               || subtype == McpttFloorMsgRelease::SUBTYPE_ACK)
        {
          McpttFloorMsgRelease releaseMsg;
          storedMsgs->RemoveHeader (releaseMsg);
          machine.Receive (releaseMsg);
        }
      else if (subtype == McpttFloorMsgRevoke::SUBTYPE)
        {
          McpttFloorMsgRevoke revokeMsg;
          storedMsgs->RemoveHeader (revokeMsg);
          machine.Receive (revokeMsg);
        }
      else if (subtype == McpttFloorMsgIdle::SUBTYPE
               || subtype == McpttFloorMsgIdle::SUBTYPE_ACK)
        {
          McpttFloorMsgIdle takenMsg;
          storedMsgs->RemoveHeader (takenMsg);
          machine.Receive (takenMsg);
        }
      else if (subtype == McpttFloorMsgTaken::SUBTYPE
               || subtype == McpttFloorMsgTaken::SUBTYPE_ACK)
        {
          McpttFloorMsgTaken takenMsg;
          storedMsgs->RemoveHeader (takenMsg);
          machine.Receive (takenMsg);
        }
      else if (subtype == McpttFloorMsgQueuePositionRequest::SUBTYPE)
        {
          McpttFloorMsgQueuePositionRequest queuePosReqMsg;
          storedMsgs->RemoveHeader (queuePosReqMsg);
          machine.Receive (queuePosReqMsg);
        }
      else if (subtype == McpttFloorMsgQueuePositionInfo::SUBTYPE
               || subtype == McpttFloorMsgQueuePositionInfo::SUBTYPE_ACK)
        {
          McpttFloorMsgQueuePositionInfo queueInfoMsg;
          storedMsgs->RemoveHeader (queueInfoMsg);
          machine.Receive (queueInfoMsg);
        }
      else if (subtype == McpttFloorMsgAck::SUBTYPE)
        {
          McpttFloorMsgAck queueInfoMsg;
          storedMsgs->RemoveHeader (queueInfoMsg);
          machine.Receive (queueInfoMsg);
        }
      else
        {
          NS_FATAL_ERROR ("Could not resolve message subtype = " << (uint32_t)subtype << ".");
        }
    }
}

void
McpttOnNetworkFloorParticipantStatePendingRequest::ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  // This could occur if the user had previously been granted the floor, but
  // the grant message was lost, and in the meantime, another user preempted
  // this user, causing the server to issue a revoke.

  if (machine.GetT103 ()->IsRunning ())
    {
      NS_LOG_WARN ("T103 being stopped from PendingRequest state");
      machine.GetT103 ()->Stop ();
    }
  machine.GetT101 ()->Stop ();

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_REVOKED);

  McpttFloorMsgRelease releaseMsg;
  releaseMsg.SetSsrc (machine.GetTxSsrc ());
  releaseMsg.SetIndicator (msg.GetIndicator ());

  if (machine.IsAckRequired ())
    {
      releaseMsg.SetSubtype (McpttFloorMsgRelease::SUBTYPE_ACK);
    }

  machine.Send (releaseMsg);
  machine.GetC100 ()->Reset ();
  machine.GetT100 ()->Start ();
  machine.ChangeState (McpttOnNetworkFloorParticipantStatePendingRelease::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStatePendingRequest::ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgGranted::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgGranted::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_GRANTED);

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
    {
      //TODO: Provide notification to the user indicating the call type.
    }

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::DUAL_FLOOR))
    {
      machine.SetOverriding (true);
      machine.SetDualFloor (true);
    }

  if (machine.GetT103 ()->IsRunning ())
    {
      machine.GetT103 ()->Stop ();
    }

  machine.GetT101 ()->Stop ();
  machine.ChangeState (McpttOnNetworkFloorParticipantStateHasPermission::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStatePendingRequest::ReceiveFloorDeny (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgDeny::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgDeny::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_DENY);
  //TODO: Display floor deny reason to the user.

  machine.GetT101 ()->Stop ();
  machine.ChangeState (McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStatePendingRequest::ExpiryOfT101 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine.GetC101 ()->IsLimitReached ())
    {
      machine.ReportEvent (McpttFloorParticipant::TIMER_T101_EXPIRED_N_TIMES);
      machine.ChangeState (McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstance ());
    }
  else
    {
      McpttFloorMsgRequest reqMsg;
      reqMsg.SetSsrc (machine.GetTxSsrc ());
      reqMsg.SetPriority (McpttFloorMsgFieldPriority (machine.GetPriority ()));
      reqMsg.SetIndicator (machine.GetIndicator ());
      machine.Send (reqMsg);
      machine.GetT101 ()->Start ();
      machine.GetC101 ()->Increment ();
    }
}

void
McpttOnNetworkFloorParticipantStatePendingRequest::ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  //TODO: Request the MCPTT client to start rendering the received RTP media packets.

  //TODO: Possibly a bug in standard - should be "restart/start T103" not just "start T103".
  machine.GetT103 ()->Restart ();
}

void
McpttOnNetworkFloorParticipantStatePendingRequest::PttRelease (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine.ReportEvent (McpttFloorParticipant::PTT_BUTTON_RELEASED);
  McpttFloorMsgRelease releaseMsg;
  releaseMsg.SetSsrc (machine.GetTxSsrc ());
  releaseMsg.SetIndicator (machine.GetIndicator ());

  if (machine.IsAckRequired ())
    {
      releaseMsg.SetSubtype (McpttFloorMsgRelease::SUBTYPE_ACK);
    }

  machine.Send (releaseMsg);
  machine.GetC100 ()->Reset ();
  machine.GetT100 ()->Start ();
  machine.GetT101 ()->Stop ();
  machine.ChangeState (McpttOnNetworkFloorParticipantStatePendingRelease::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStatePendingRequest::ReceiveFloorQueuePositionInfo (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgQueuePositionInfo& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_QUEUE);
  if (msg.GetSubtype () == McpttFloorMsgQueuePositionInfo::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgQueuePositionInfo::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }

  if (machine.GetT101 ()->IsRunning ())
    {
      machine.GetT101 ()->Stop ();
    }

  //TODO: Provide floor request queued response to the user.
  //TODO: Provide queue position and priority to the MCPTT user.

  machine.ChangeState (McpttOnNetworkFloorParticipantStateQueued::GetInstance ());
}
/** McpttOnNetworkFloorParticipantStatePendingRequest - end **/

/** McpttOnNetworkFloorParticipantStateHasPermission - begin **/
Ptr<McpttOnNetworkFloorParticipantStateHasPermission>
McpttOnNetworkFloorParticipantStateHasPermission::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorParticipantStateHasPermission> instance = Create<McpttOnNetworkFloorParticipantStateHasPermission> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorParticipantStateHasPermission::GetStateId (void)
{
  static McpttEntityId id (3, "'U: has permission'");

  return id;
}

McpttOnNetworkFloorParticipantStateHasPermission::McpttOnNetworkFloorParticipantStateHasPermission (void)
  : McpttOnNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorParticipantStateHasPermission::~McpttOnNetworkFloorParticipantStateHasPermission (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorParticipantStateHasPermission::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorParticipantStateHasPermission::GetStateId ();
}

bool
McpttOnNetworkFloorParticipantStateHasPermission::HasFloor (const McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  return true;
}

void
McpttOnNetworkFloorParticipantStateHasPermission::MediaReady (McpttOnNetworkFloorParticipant& machine, McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  //TODO: Shall request the MCPTT client to start forwarding encoded voice to the MCPTT server.
}

void
McpttOnNetworkFloorParticipantStateHasPermission::PttRelease (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine.ReportEvent (McpttFloorParticipant::PTT_BUTTON_RELEASED);
  McpttFloorMsgRelease releaseMsg;
  releaseMsg.SetSsrc (machine.GetTxSsrc ());
  releaseMsg.SetIndicator (machine.GetIndicator ());

  if (machine.IsAckRequired ())
    {
      releaseMsg.SetSubtype (McpttFloorMsgRelease::SUBTYPE_ACK);
    }

  machine.Send (releaseMsg);
  machine.SetOverriding (false);
  machine.SetOverridden (false);
  machine.SetDualFloor (false);
  machine.GetC100 ()->Reset ();
  machine.GetT100 ()->Start ();
  machine.ChangeState (McpttOnNetworkFloorParticipantStatePendingRelease::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStateHasPermission::ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  //TODO: Give reason the user about why the floor is being revoked.
  //TODO: Request the media mixer to discard any remaining buffered RTP media and stop fowarding conded voice to server.

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_REVOKED);

  McpttFloorMsgRelease releaseMsg;
  releaseMsg.SetSsrc (machine.GetTxSsrc ());
  releaseMsg.SetIndicator (msg.GetIndicator ());

  if (machine.IsAckRequired ())
    {
      releaseMsg.SetSubtype (McpttFloorMsgRelease::SUBTYPE_ACK);
    }

  machine.Send (releaseMsg);
  machine.GetC100 ()->Reset ();
  machine.GetT100 ()->Start ();
  machine.ChangeState (McpttOnNetworkFloorParticipantStatePendingRelease::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStateHasPermission::ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgGranted::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgGranted::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }
}

void
McpttOnNetworkFloorParticipantStateHasPermission::ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (machine.IsOverriding ())
    {
      //TODO: Request MCPTT client, based on the configuration to either, continue
      //      rendering or stop the rendering and start storing the recieved RTP
      //      media packets.
    }

  if (machine.IsOverridden ())
    {
      //TODO: Shall request the MCPTT client to render the recieved RTP media
      //      packets.
    }
}

void
McpttOnNetworkFloorParticipantStateHasPermission::ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  //Check if duall floor bit is set.

  if (machine.IsOverriding ())
    {
      if (msg.GetSubtype () == McpttFloorMsgIdle::SUBTYPE_ACK)
        {
          McpttFloorMsgAck ackMsg;
          ackMsg.SetSsrc (machine.GetTxSsrc ());
          ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgIdle::SUBTYPE));
          ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
          machine.Send (ackMsg);
          machine.SetOverriding (false);
        }
    }
  else if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::DUAL_FLOOR)
           && machine.IsOverridden ())
    {
      if (msg.GetSubtype () == McpttFloorMsgIdle::SUBTYPE_ACK)
        {
          McpttFloorMsgAck ackMsg;
          ackMsg.SetSsrc (machine.GetTxSsrc ());
          ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgIdle::SUBTYPE));
          ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
          machine.Send (ackMsg);
          machine.SetOverridden (false);
        }
    }
}

void
McpttOnNetworkFloorParticipantStateHasPermission::ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_TAKEN);

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::DUAL_FLOOR))
    {
      //TODO: shall inform user that the call is overriden without revoke.
      machine.SetOverridden (true);
    }
}

void
McpttOnNetworkFloorParticipantStateHasPermission::Selected (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << " HasPermission::Selected " << &machine);

  Ptr<McpttPttApp> app = machine.GetCall ()->GetOwner ();
  Ptr<McpttMediaSrc> mediaSrc = app->GetMediaSrc ();

  if (!app->IsPushed ())
    {
      NS_LOG_DEBUG ("McpttPttApp is not pushed; notify that it is now pushed");
      app->NotifyPushed ();
    }

  if (machine.ShouldGenMedia ()
      && !mediaSrc->IsMakingReq ())
    {
      NS_LOG_DEBUG ("media src starting");
      mediaSrc->StartMakingReq ();
    }
}

void
McpttOnNetworkFloorParticipantStateHasPermission::Unselected (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << " HasPermission::Unselected " << &machine);

  Ptr<McpttPttApp> app = machine.GetCall ()->GetOwner ();
  Ptr<McpttMediaSrc> mediaSrc = app->GetMediaSrc ();

  if (app->IsPushed ())
    {
      NS_LOG_DEBUG ("IsPushed true; notifying McpttPttApp of release");
      app->NotifyReleased ();
    }

  if (mediaSrc->IsMakingReq ())
    {
      NS_LOG_DEBUG ("media src is making requests; stopping it");
      mediaSrc->StopMakingReq ();
    }
}
/** McpttOnNetworkFloorParticipantStateHasPermission - end **/

/** McpttOnNetworkFloorParticipantStatePendingRelease - begin **/
Ptr<McpttOnNetworkFloorParticipantStatePendingRelease>
McpttOnNetworkFloorParticipantStatePendingRelease::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorParticipantStatePendingRelease> instance = Create<McpttOnNetworkFloorParticipantStatePendingRelease> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorParticipantStatePendingRelease::GetStateId (void)
{
  static McpttEntityId id (4, "'U: pending Release'");

  return id;
}

McpttOnNetworkFloorParticipantStatePendingRelease::McpttOnNetworkFloorParticipantStatePendingRelease (void)
  : McpttOnNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorParticipantStatePendingRelease::~McpttOnNetworkFloorParticipantStatePendingRelease (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorParticipantStatePendingRelease::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorParticipantStatePendingRelease::GetStateId ();
}

void
McpttOnNetworkFloorParticipantStatePendingRelease::ExpiryOfT100 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine.GetC100 ()->IsLimitReached ())
    {
      machine.ReportEvent (McpttFloorParticipant::TIMER_T100_EXPIRED_N_TIMES);
      machine.ChangeState (McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstance ());
    }
  else
    {
      McpttFloorMsgRelease releaseMsg;
      releaseMsg.SetSsrc (machine.GetTxSsrc ());
      if (machine.IsAckRequired ())
        {
          releaseMsg.SetSubtype (McpttFloorMsgRelease::SUBTYPE_ACK);
        }
      machine.Send (releaseMsg);
      machine.GetT100 ()->Start ();
      machine.GetC100 ()->Increment ();
    }
}

void
McpttOnNetworkFloorParticipantStatePendingRelease::ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgIdle::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgIdle::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_IDLE);

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
    {
      //TODO: shall provide a notification to the user indicating the type of call.
    }

  machine.GetT100 ()->Stop ();

  if (machine.GetCallTypeId () != McpttCallMsgFieldCallType::BROADCAST_GROUP
      || msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::NORMAL_CALL))
    {
      machine.ChangeState (McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstance ());
    }

  if (machine.GetCallTypeId () == McpttCallMsgFieldCallType::BROADCAST_GROUP)
    {
      //TODO: shall indicate to the MCPTT client the media transmission is completed.
      machine.ChangeState (McpttOnNetworkFloorParticipantStateReleasing::GetInstance ());
    }
}

void
McpttOnNetworkFloorParticipantStatePendingRelease::ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgTaken::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgTaken::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_TAKEN);

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
    {
      //TODO: Provide notification to user indicating the type of call.
    }

  // It is possible for this timer to be running due to previous activity
  // of another MCPTT user, so issue Restart() instead of Start()
  machine.GetT103 ()->Restart ();
  machine.GetT100 ()->Stop ();
  machine.ChangeState (McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStatePendingRelease::ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  //TODO: Request MCPTT client to start rendering received RTP media packets.

  // It is possible for this timer to be running due to previous activity
  // of another MCPTT user, so issue Restart() instead of Start()
  machine.GetT103 ()->Restart ();
  machine.GetT100 ()->Stop ();
  machine.ChangeState (McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStatePendingRelease::ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  //TODO: May give information to the user that permission to send RTP media is being revoked.
  //TODO: May inform the user of the reason contained in the Floor Revoke message.
}

void
McpttOnNetworkFloorParticipantStatePendingRelease::ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgGranted::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgGranted::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }
}
/** McpttOnNetworkFloorParticipantStatePendingRelease - end **/

/** McpttOnNetworkFloorParticipantStateReleasing - begin **/
Ptr<McpttOnNetworkFloorParticipantStateReleasing>
McpttOnNetworkFloorParticipantStateReleasing::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorParticipantStateReleasing> instance = Create<McpttOnNetworkFloorParticipantStateReleasing> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorParticipantStateReleasing::GetStateId (void)
{
  static McpttEntityId id (5, "'Releasing'");

  return id;
}

McpttOnNetworkFloorParticipantStateReleasing::McpttOnNetworkFloorParticipantStateReleasing (void)
  : McpttOnNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorParticipantStateReleasing::~McpttOnNetworkFloorParticipantStateReleasing (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorParticipantStateReleasing::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorParticipantStateReleasing::GetStateId ();
}

void
McpttOnNetworkFloorParticipantStateReleasing::CallRelease2 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this);

  // TODO:  check that all timers (not just T103) are all stopped (sec 6.2.4.8.2)
  if (machine.GetT103 ()->IsRunning ())
    {
      machine.GetT103 ()->Stop ();
    }
  machine.ChangeState (McpttOnNetworkFloorParticipantStateStartStop::GetInstance ());
}
/** McpttOnNetworkFloorParticipantStateReleasing - end **/

/** McpttOnNetworkFloorParticipantStateQueued - begin **/
Ptr<McpttOnNetworkFloorParticipantStateQueued>
McpttOnNetworkFloorParticipantStateQueued::GetInstance (void)
{
  static Ptr<McpttOnNetworkFloorParticipantStateQueued> instance = Create<McpttOnNetworkFloorParticipantStateQueued> ();
  return instance;
}

McpttEntityId
McpttOnNetworkFloorParticipantStateQueued::GetStateId (void)
{
  static McpttEntityId id (6, "'U: queued'");

  return id;
}

McpttOnNetworkFloorParticipantStateQueued::McpttOnNetworkFloorParticipantStateQueued (void)
  : McpttOnNetworkFloorParticipantState ()
{
  NS_LOG_FUNCTION (this);
}

McpttOnNetworkFloorParticipantStateQueued::~McpttOnNetworkFloorParticipantStateQueued (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttOnNetworkFloorParticipantStateQueued::GetInstanceStateId (void) const
{
  return McpttOnNetworkFloorParticipantStateQueued::GetStateId ();
}

void
McpttOnNetworkFloorParticipantStateQueued::ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  //TODO: shall request to the media mixer to start rendering received RTP media packets.

  machine.GetT103 ()->Restart ();
}

void
McpttOnNetworkFloorParticipantStateQueued::ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_TAKEN);

  if (msg.GetSubtype () == McpttFloorMsgTaken::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgTaken::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }
}

void
McpttOnNetworkFloorParticipantStateQueued::ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  // This could occur if the user had previously been granted the floor, but
  // the grant message was lost, and in the meantime, another user preempted
  // this user, causing the server to issue a revoke.

  if (machine.GetT104 ()->IsRunning ())
    {
      machine.GetT104 ()->Stop ();
    }

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_REVOKED);

  McpttFloorMsgRelease releaseMsg;
  releaseMsg.SetSsrc (machine.GetTxSsrc ());
  releaseMsg.SetIndicator (msg.GetIndicator ());

  if (machine.IsAckRequired ())
    {
      releaseMsg.SetSubtype (McpttFloorMsgRelease::SUBTYPE_ACK);
    }

  machine.Send (releaseMsg);
  machine.GetC100 ()->Reset ();
  machine.GetT100 ()->Start ();
  machine.ChangeState (McpttOnNetworkFloorParticipantStatePendingRelease::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStateQueued::ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgGranted::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgGranted::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_GRANTED);

  if (msg.GetIndicator ().IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
    {
      //TODO: Shall provide a notification to the user indicating the type of call.
    }

  if (machine.GetT104 ()->IsRunning ())
    {
      machine.GetT104 ()->Stop ();
    }

  machine.GetT132 ()->Start ();

  machine.FloorGranted ();
}

void
McpttOnNetworkFloorParticipantStateQueued::ReceiveFloorDeny (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgDeny::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgDeny::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
    }

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_DENY);
  //TODO: Shall provide a floor deny notification to the MCPTT user.
  //TODO: May display the deny reason to the user using information in the Reject Cause field.

  if (machine.GetT104 ()->IsRunning ())
    {
      machine.GetT104 ()->Stop ();
    }

  machine.ChangeState (McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStateQueued::PttRelease (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine.ReportEvent (McpttFloorParticipant::PTT_BUTTON_RELEASED);
  McpttFloorMsgRelease releaseMsg;
  releaseMsg.SetSsrc (machine.GetTxSsrc ());
  releaseMsg.SetIndicator (machine.GetIndicator ());

  if (machine.IsAckRequired ())
    {
      releaseMsg.SetSubtype (McpttFloorMsgRelease::SUBTYPE_ACK);
    }

  machine.Send (releaseMsg);

  machine.GetC100 ()->Reset ();
  machine.GetT100 ()->Start ();

  if (machine.GetT104 ()->IsRunning ())
    {
      machine.GetT104 ()->Stop ();
    }

  machine.ChangeState (McpttOnNetworkFloorParticipantStatePendingRelease::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStateQueued::ReceiveFloorQueuePositionInfo (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgQueuePositionInfo& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_QUEUE);
  //TODO: May provide the queue position and priority (if available) to the MCPTT user.

  if (msg.GetSubtype () == McpttFloorMsgQueuePositionInfo::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgQueuePositionInfo::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }

  if (machine.GetT104 ()->IsRunning ())
    {
      machine.GetT104 ()->Stop ();
    }
}

void
McpttOnNetworkFloorParticipantStateQueued::ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const
{
  NS_LOG_FUNCTION (this << &machine << msg);

  if (msg.GetSubtype () == McpttFloorMsgIdle::SUBTYPE_ACK)
    {
      McpttFloorMsgAck ackMsg;
      ackMsg.SetSsrc (machine.GetTxSsrc ());
      ackMsg.SetMsgType (McpttFloorMsgFieldType (McpttFloorMsgIdle::SUBTYPE));
      ackMsg.SetSource (McpttFloorMsgFieldSource (McpttFloorMsgFieldSource::FLOOR_PARTICIPANT));
      machine.Send (ackMsg);
    }

  machine.ReportEvent (McpttFloorParticipant::RECEIVED_FLOOR_IDLE);

  if (machine.GetT104 ()->IsRunning ())
    {
      machine.GetT104 ()->Stop ();
    }

  machine.ChangeState (McpttOnNetworkFloorParticipantStateHasNoPermission::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStateQueued::SendFloorQueuePositionRequest (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  McpttFloorMsgQueuePositionRequest requestMsg;
  requestMsg.SetSsrc (machine.GetTxSsrc ());

  machine.Send (requestMsg);
  machine.GetC104 ()->Reset ();
  machine.GetT104 ()->Start ();
}

void
McpttOnNetworkFloorParticipantStateQueued::ExpiryOfT104 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  if (machine.GetC104 ()->IsLimitReached ())
    {
      machine.ReportEvent (McpttFloorParticipant::TIMER_T104_EXPIRED_N_TIMES);
      McpttFloorMsgQueuePositionRequest requestMsg;
      requestMsg.SetSsrc (machine.GetTxSsrc ());

      machine.Send (requestMsg);
      machine.GetC104 ()->Increment ();
      machine.GetT104 ()->Start ();
    }
  else
    {
      //TODO: Shall provide a floor queued timeout to the MCPTT client.

      McpttFloorMsgRelease releaseMsg;
      releaseMsg.SetSsrc (machine.GetTxSsrc ());

      if (machine.IsAckRequired ())
        {
          releaseMsg.SetSubtype (McpttFloorMsgRelease::SUBTYPE_ACK);
        }

      machine.ChangeState (McpttOnNetworkFloorParticipantStatePendingRelease::GetInstance ());
    }
}

void
McpttOnNetworkFloorParticipantStateQueued::AcceptGrant (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine.GetT132 ()->Stop ();
  machine.ChangeState (McpttOnNetworkFloorParticipantStateHasPermission::GetInstance ());
}

void
McpttOnNetworkFloorParticipantStateQueued::ExpiryOfT132 (McpttOnNetworkFloorParticipant& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  McpttFloorMsgRelease releaseMsg;
  releaseMsg.SetSsrc (machine.GetTxSsrc ());

  if (machine.IsAckRequired ())
    {
      releaseMsg.SetSubtype (McpttFloorMsgRelease::SUBTYPE_ACK);
    }

  //TODO: May indicate the user that the floor is no more available.

  machine.ChangeState (McpttOnNetworkFloorParticipantStatePendingRelease::GetInstance ());
}

/** McpttOnNetworkFloorParticipantStateQueued - end **/

} //namespace psc
} //namespace ns3

