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
#include <ns3/type-id.h>

#include "mcptt-call-msg.h"
#include "mcptt-call-machine-private.h"
#include "mcptt-call-type-machine-private.h"
#include "mcptt-floor-participant.h"
#include "mcptt-ptt-app.h"

#include "mcptt-call-type-machine-private-state.h"

namespace ns3 {

namespace psc {

/** McpttCallTypeMachinePrivateState - begin **/
NS_LOG_COMPONENT_DEFINE ("McpttCallTypeMachinePrivateState");

McpttCallTypeMachinePrivateState::~McpttCallTypeMachinePrivateState (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallTypeMachinePrivateState::Downgrade (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring downgrade.");
}

void
McpttCallTypeMachinePrivateState::ExpiryOfTfp1 (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of TFP1.");
}

void
McpttCallTypeMachinePrivateState::ExpiryOfTfp6 (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of TFP6.");
}

void
McpttCallTypeMachinePrivateState::ExpiryOfTfp8 (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of TFP8.");
}

void
McpttCallTypeMachinePrivateState::InitiateCall (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring initate call.");
}

void
McpttCallTypeMachinePrivateState::ReceiveAccept (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallTypeMachinePrivateState::ReceiveAcceptAck (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAcceptAck& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallTypeMachinePrivateState::ReceiveEmergencyCancel (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateEmergCancel& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallTypeMachinePrivateState::ReceiveEmergencyCancelAck (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateEmergCancelAck& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallTypeMachinePrivateState::ReceiveReject (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateReject& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallTypeMachinePrivateState::ReceiveRelease (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateRelease& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallTypeMachinePrivateState::ReceiveSetupRequest (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallTypeMachinePrivateState::ReleaseCall (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring release call.");
}

void
McpttCallTypeMachinePrivateState::UpgradeTo (McpttCallTypeMachinePrivate& machine, const McpttCallMsgFieldCallType& callType)
{
  NS_LOG_FUNCTION (this << &machine << &callType);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring upgrade to " << callType.GetType () << ".");
}

McpttCallTypeMachinePrivateState::McpttCallTypeMachinePrivateState (void)
  : SimpleRefCount<McpttCallTypeMachinePrivateState> ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttCallTypeMachinePrivateState - end **/

std::ostream&
operator<< (std::ostream& os, const McpttCallTypeMachinePrivateState& inst)
{
  McpttEntityId stateId = inst.GetInstanceStateId ();

  os << stateId;

  return os;
}

/** McpttCallTypeMachinePrivateStateQ0 - begin **/
Ptr<McpttCallTypeMachinePrivateStateQ0>
McpttCallTypeMachinePrivateStateQ0::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallTypeMachinePrivateStateQ0> instance = Create<McpttCallTypeMachinePrivateStateQ0> ();

  return instance;
}

McpttEntityId
McpttCallTypeMachinePrivateStateQ0::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (0, "'Q0: wiating for the call to be established'");

  return stateId;
}

McpttCallTypeMachinePrivateStateQ0::McpttCallTypeMachinePrivateStateQ0 (void)
  : McpttCallTypeMachinePrivateState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallTypeMachinePrivateStateQ0::~McpttCallTypeMachinePrivateStateQ0 (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttCallTypeMachinePrivateStateQ0::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallTypeMachinePrivateStateQ0::GetStateId ();
}

void
McpttCallTypeMachinePrivateStateQ0::InitiateCall (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCallMachinePrivate> callMachine = machine.GetOwner ();
  Ptr<McpttEmergAlertMachine> emergMachine = callMachine->GetEmergMachine ();

  if (emergMachine->IsInEmergState ())
    {
      machine.SetCallType (McpttCallMsgFieldCallType (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
      machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
    }
  else
    {
      machine.SetCallType (McpttCallMsgFieldCallType (McpttCallMsgFieldCallType::PRIVATE));
      machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
    }

}

void
McpttCallTypeMachinePrivateStateQ0::ReceiveAccept (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  McpttCallMsgFieldCallType myCallType = machine.GetCallType ();

  if (myCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_PRIVATE)
    {
      tfp8->Start ();

      machine.ChangeState (McpttCallTypeMachinePrivateStateQ2::GetInstance ());
    }
  else if (myCallType.GetType () == McpttCallMsgFieldCallType::PRIVATE)
    {
      machine.ChangeState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
    }
}

void
McpttCallTypeMachinePrivateStateQ0::ReceiveAcceptAck (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAcceptAck& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  McpttCallMsgFieldCallType myCallType = machine.GetCallType ();

  if (myCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_PRIVATE)
    {
      tfp8->Start ();

      machine.ChangeState (McpttCallTypeMachinePrivateStateQ2::GetInstance ());
    }
  else if (myCallType.GetType () == McpttCallMsgFieldCallType::PRIVATE)
    {
      machine.ChangeState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
    }
}

void
McpttCallTypeMachinePrivateStateQ0::ReceiveSetupRequest (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg)
{
  NS_LOG_FUNCTION (this << &machine << &msg);

  McpttCallMsgFieldCallType theirCallType = msg.GetCallType ();

  if (theirCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_PRIVATE)
    {
      machine.SetCallType (McpttCallMsgFieldCallType (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
      machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
    }
  else if (theirCallType.GetType () == McpttCallMsgFieldCallType::PRIVATE)
    {
      machine.SetCallType (McpttCallMsgFieldCallType (McpttCallMsgFieldCallType::PRIVATE));
      machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
    }

}
/** McpttCallTypeMachinePrivateStateQ0 - end **/

/** McpttCallTypeMachinePrivateStateQ1 - begin **/
Ptr<McpttCallTypeMachinePrivateStateQ1>
McpttCallTypeMachinePrivateStateQ1::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallTypeMachinePrivateStateQ1> instance = Create<McpttCallTypeMachinePrivateStateQ1> ();

  return instance;
}

McpttEntityId
McpttCallTypeMachinePrivateStateQ1::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (1, "'Q1: in-progress private call'");

  return stateId;
}

McpttCallTypeMachinePrivateStateQ1::McpttCallTypeMachinePrivateStateQ1 (void)
  : McpttCallTypeMachinePrivateState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallTypeMachinePrivateStateQ1::~McpttCallTypeMachinePrivateStateQ1 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallTypeMachinePrivateStateQ1::ExpiryOfTfp6 (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCallMachinePrivate> callMachine = machine.GetOwner ();

  Ptr<McpttTimer> tfp8 = callMachine->GetTfp8 ();
  Ptr<McpttTimer> tfp6 = callMachine->GetTfp6 ();
  Ptr<McpttCounter> cfp6 = callMachine->GetCfp6 ();

  McpttCallMsgFieldCallId myCallId = callMachine->GetCallId ();
  McpttCallMsgFieldUserId myCallerId = callMachine->GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = callMachine->GetCalleeId ();

  if (cfp6->IsLimitReached ())
    {
      machine.SetCallType (McpttCallMsgFieldCallType ());
      machine.SetPriority (0);
      machine.ChangeState (McpttCallTypeMachinePrivateStateQ0::GetInstance ());
    }
  else
    {
      McpttCallMsgPrivateEmergCancel emergCancelMsg;
      emergCancelMsg.SetCallId (myCallId);
      emergCancelMsg.SetCallerId (myCallerId);
      emergCancelMsg.SetCalleeId (myCalleeId);

      machine.Send (emergCancelMsg);

      cfp6->Increment ();

      tfp6->Start ();
    }
}

McpttEntityId
McpttCallTypeMachinePrivateStateQ1::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallTypeMachinePrivateStateQ1::GetStateId ();
}

void
McpttCallTypeMachinePrivateStateQ1::ReceiveAccept (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCallMachinePrivate> callMachine = machine.GetOwner ();
  Ptr<McpttCall> call = callMachine->GetCall ();

  Ptr<McpttTimer> tfp1 = callMachine->GetTfp1 ();
  Ptr<McpttTimer> tfp2 = callMachine->GetTfp2 ();
  Ptr<McpttTimer> tfp8 = callMachine->GetTfp8 ();

  McpttCallMsgFieldSdp theirSdp = msg.GetSdp ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldCallId myCallId = callMachine->GetCallId ();

  uint16_t floorPort = theirSdp.GetFloorPort ();
  uint16_t speechPort = theirSdp.GetSpeechPort ();
  Address grpAddress = theirSdp.GetGrpAddr ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      callMachine->SetSdpEmerg (theirSdp);

      McpttCallMsgPrivateAcceptAck ackMsg;
      ackMsg.SetCallId (myCallId);
      ackMsg.SetCallerId (callMachine->GetCallerId ());
      ackMsg.SetCalleeId (callMachine->GetCalleeId ());

      machine.Send (ackMsg);

      if (tfp1->IsRunning ())
        {
          tfp1->Stop ();
        }

      if (tfp2->IsRunning ())
        {
          tfp2->Stop ();
        }

      if (!tfp8->IsRunning ())
        {
          tfp8->Start ();
        }

      //TODO: Not in standard - does not require closing previous session.
      call->CloseFloorChannel ();
      call->CloseMediaChannel ();

      call->OpenFloorChannel (grpAddress, floorPort);
      call->OpenMediaChannel (grpAddress, speechPort);
    }
}

void
McpttCallTypeMachinePrivateStateQ1::ReceiveEmergencyCancel (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateEmergCancel& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCallMachinePrivate> callMachine = machine.GetOwner ();
  Ptr<McpttCall> call = callMachine->GetCall ();

  Ptr<McpttTimer> tfp8 = callMachine->GetTfp8 ();

  McpttCallMsgFieldSdp mySdp = callMachine->GetSdp ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldCallId myCallId = callMachine->GetCallId ();
  McpttCallMsgFieldUserId myCallerId = callMachine->GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = callMachine->GetCalleeId ();

  uint16_t floorPort = mySdp.GetFloorPort ();
  uint16_t speechPort = mySdp.GetSpeechPort ();
  Address grpAddress = mySdp.GetGrpAddr ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      McpttCallMsgPrivateEmergCancelAck emergCancelAckMsg;
      emergCancelAckMsg.SetCallId (myCallId);
      emergCancelAckMsg.SetCallerId (myCallerId);
      emergCancelAckMsg.SetCalleeId (myCalleeId);

      machine.Send (emergCancelAckMsg);

      tfp8->Stop ();

      //TODO: Not in standard - does not say to close media session
      call->CloseFloorChannel ();
      call->CloseMediaChannel ();

      call->OpenFloorChannel (grpAddress, floorPort);
      call->OpenMediaChannel (grpAddress, speechPort);

      machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));

      if (machine.GetState ()->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ())
        {
          machine.ChangeState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
          machine.SetCallType (McpttCallMsgFieldCallType (McpttCallMsgFieldCallType::PRIVATE));
        }
    }
}

void
McpttCallTypeMachinePrivateStateQ1::ReceiveEmergencyCancelAck (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateEmergCancelAck& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCallMachinePrivate> callMachine = machine.GetOwner ();
  Ptr<McpttCall> call = callMachine->GetCall ();

  Ptr<McpttTimer> tfp6 = callMachine->GetTfp6 ();

  McpttCallMsgFieldSdp mySdp = callMachine->GetSdp ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldCallId myCallId = callMachine->GetCallId ();

  uint16_t floorPort = mySdp.GetFloorPort ();
  uint16_t speechPort = mySdp.GetSpeechPort ();
  Address grpAddress = mySdp.GetGrpAddr ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      tfp6->Stop ();

      //TODO: Not in standard - does not close open media session
      call->CloseFloorChannel ();
      call->CloseMediaChannel ();

      call->OpenFloorChannel (grpAddress, floorPort);
      call->OpenMediaChannel (grpAddress, speechPort);

      machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
    }
}

void
McpttCallTypeMachinePrivateStateQ1::ReceiveSetupRequest (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCallMachinePrivate> callMachine = machine.GetOwner ();
  Ptr<McpttCall> call = callMachine->GetCall ();
  Ptr<McpttPttApp> pttApp = call->GetOwner ();

  uint32_t myUserId = pttApp->GetUserId ();
  Ptr<McpttTimer> tfp8 = callMachine->GetTfp8 ();

  McpttCallMsgFieldSdp theirSdp = callMachine->GetSdp ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldUserId theirCallerId = msg.GetCallerId ();
  McpttCallMsgFieldUserId theirCalleeId = msg.GetCalleeId ();
  McpttCallMsgFieldCallType theirCallType = msg.GetCallType ();
  McpttCallMsgFieldCallId myCallId = callMachine->GetCallId ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ()
      && theirCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_PRIVATE)
    {
      if (callMachine->IsFailure ()) //Media session CANNOT be established
        {
          McpttCallMsgPrivateReject rejectMsg;
          rejectMsg.SetCallId (myCallId);
          rejectMsg.SetCallerId (theirCallerId);
          rejectMsg.SetCalleeId (theirCalleeId);
          rejectMsg.SetReason (McpttCallMsgFieldReason (McpttCallMsgFieldReason::FAILED));

          machine.Send (rejectMsg);
        }
      else //Media sesssion CAN be established
        {
          callMachine->SetSdpEmerg (theirSdp);
          callMachine->SetCallerId (theirCallerId);
          callMachine->SetCalleeId (McpttCallMsgFieldUserId (myUserId));

          McpttCallMsgPrivateAccept acceptMsg;
          acceptMsg.SetCallId (callMachine->GetCallId ());
          acceptMsg.SetCalleeId (callMachine->GetCalleeId ());
          acceptMsg.SetCallerId (callMachine->GetCallerId ());
          acceptMsg.SetSdp (callMachine->GetSdp ());

          machine.Send (acceptMsg);

          if (machine.GetState ()->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ())
            {
              tfp8->Start ();
              machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
              machine.SetCallType (McpttCallMsgFieldCallType (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
              machine.ChangeState (McpttCallTypeMachinePrivateStateQ2::GetInstance ());
            }
        }
    }
}

void
McpttCallTypeMachinePrivateStateQ1::UpgradeTo (McpttCallTypeMachinePrivate& machine, const McpttCallMsgFieldCallType& callType)
{
  NS_LOG_FUNCTION (this << &machine << &callType);

  Ptr<McpttCallMachinePrivate> callMachine = machine.GetOwner ();
  Ptr<McpttCall> call = callMachine->GetCall ();
  Ptr<McpttPttApp> pttApp = call->GetOwner ();

  Ptr<McpttTimer> tfp1 = callMachine->GetTfp1 ();
  Ptr<McpttCounter> cfp1 = callMachine->GetCfp1 ();

  AddressValue grpAddress;
  Vector myLoc = pttApp->GetNodeLoc ();
  uint32_t myUserId = pttApp->GetUserId ();
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = pttApp->GetLocalAddress ();
  call->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldCallId myCallId = callMachine->GetCallId ();
  McpttCallMsgFieldUserId myCallerId = callMachine->GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = callMachine->GetCalleeId ();

  if (callType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_PRIVATE)
    {
      McpttCallMsgFieldSdp sdpEmerg;
      sdpEmerg.SetFloorPort (floorPort);
      sdpEmerg.SetGrpAddr (grpAddress.Get ());
      sdpEmerg.SetOrigAddr (origAddress);
      sdpEmerg.SetSpeechPort (speechPort);

      callMachine->SetSdpEmerg (sdpEmerg);

      if (myUserId == myCallerId.GetId ())
        {
          callMachine->SetCallerId (myCallerId);
          callMachine->SetCalleeId (myCalleeId);
        }
      else if (myUserId == myCalleeId.GetId ())
        {
          callMachine->SetCallerId (myCalleeId);
          callMachine->SetCalleeId (myCallerId);
        }
      else
        {
          NS_FATAL_ERROR ("Caller/Callee does not match.");
        }

      //TODO: Shall store the current user location as user location.

      machine.SetCallType (McpttCallMsgFieldCallType (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));

      McpttCallMsgPrivateSetupReq reqMsg;
      reqMsg.SetCallId (myCallId);
      reqMsg.SetCallerId (callMachine->GetCallerId ());
      reqMsg.SetCalleeId (callMachine->GetCalleeId ());
      reqMsg.SetCommMode (McpttCallMsgFieldCommMode (McpttCallMsgFieldCommMode::AUTOMATIC_MODE));
      reqMsg.SetCallType (machine.GetCallType ());
      reqMsg.SetSdp (sdpEmerg);
      reqMsg.SetUserLoc (myLoc);

      machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));

      machine.Send (reqMsg);

      cfp1->Reset ();

      tfp1->Start ();

      machine.ChangeState (McpttCallTypeMachinePrivateStateQ2::GetInstance ());
    }
}

void
McpttCallTypeMachinePrivateStateQ1::ReleaseCall (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  machine.SetCallType (McpttCallMsgFieldCallType ());
  machine.SetPriority (0);
  machine.ChangeState (McpttCallTypeMachinePrivateStateQ0::GetInstance ());
}
/** McpttCallTypeMachinePrivateStateQ1 - end **/

/** McpttCallTypeMachinePrivateStateQ2 - begin **/
Ptr<McpttCallTypeMachinePrivateStateQ2>
McpttCallTypeMachinePrivateStateQ2::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallTypeMachinePrivateStateQ2> instance = Create<McpttCallTypeMachinePrivateStateQ2> ();

  return instance;
}

McpttEntityId
McpttCallTypeMachinePrivateStateQ2::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (2, "'Q2: in-progress emergency private call'");

  return stateId;
}

McpttCallTypeMachinePrivateStateQ2::McpttCallTypeMachinePrivateStateQ2 (void)
  : McpttCallTypeMachinePrivateState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallTypeMachinePrivateStateQ2::~McpttCallTypeMachinePrivateStateQ2 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallTypeMachinePrivateStateQ2::Downgrade (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCallMachinePrivate> callMachine = machine.GetOwner ();

  Ptr<McpttTimer> tfp8 = callMachine->GetTfp8 ();
  Ptr<McpttTimer> tfp6 = callMachine->GetTfp6 ();
  Ptr<McpttCounter> cfp6 = callMachine->GetCfp6 ();

  McpttCallMsgFieldCallId myCallId = callMachine->GetCallId ();
  McpttCallMsgFieldUserId myCallerId = callMachine->GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = callMachine->GetCalleeId ();

  McpttCallMsgPrivateEmergCancel emergCancelMsg;
  emergCancelMsg.SetCallId (myCallId);
  emergCancelMsg.SetCallerId (myCallerId);
  emergCancelMsg.SetCalleeId (myCalleeId);

  machine.Send (emergCancelMsg);

  tfp8->Stop ();

  cfp6->Reset ();

  tfp6->Start ();

  machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
  machine.SetCallType (McpttCallMsgFieldCallType (McpttCallMsgFieldCallType::PRIVATE));
  machine.ChangeState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
}

void
McpttCallTypeMachinePrivateStateQ2::ExpiryOfTfp1 (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCallMachinePrivate> callMachine = machine.GetOwner ();
  Ptr<McpttCall> call = callMachine->GetCall ();
  Ptr<McpttPttApp> pttApp = call->GetOwner ();

  Vector myLoc = pttApp->GetNodeLoc ();
  Ptr<McpttTimer> tfp1 = callMachine->GetTfp1 ();
  Ptr<McpttCounter> cfp1 = callMachine->GetCfp1 ();

  if (cfp1->IsLimitReached ())
    {
      machine.SetCallType (McpttCallMsgFieldCallType ());
      machine.SetPriority (0);
      machine.ChangeState (McpttCallTypeMachinePrivateStateQ0::GetInstance ());
    }
  else
    {
      //TODO: Update stored user location with current user location.

      cfp1->Increment ();

      McpttCallMsgPrivateSetupReq reqMsg;
      reqMsg.SetCallId (callMachine->GetCallId ());
      reqMsg.SetCallerId (callMachine->GetCallerId ());
      reqMsg.SetCalleeId (callMachine->GetCalleeId ());
      reqMsg.SetCommMode (McpttCallMsgFieldCommMode (McpttCallMsgFieldCommMode::AUTOMATIC_MODE));
      reqMsg.SetCallType (machine.GetCallType ());
      reqMsg.SetSdp (callMachine->GetSdpEmerg ());
      reqMsg.SetUserLoc (McpttCallMsgFieldUserLoc (myLoc));

      machine.Send (reqMsg);

      tfp1->Start ();
    }
}

void
McpttCallTypeMachinePrivateStateQ2::ExpiryOfTfp8 (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCallMachinePrivate> callMachine = machine.GetOwner ();
  Ptr<McpttCall> call = callMachine->GetCall ();

  McpttCallMsgFieldSdp mySdp = callMachine->GetSdp ();

  uint16_t floorPort = mySdp.GetFloorPort ();
  uint16_t speechPort = mySdp.GetSpeechPort ();
  Address grpAddress = mySdp.GetGrpAddr ();

  //TODO: Not in standard - does not require closing previous session
  call->CloseFloorChannel ();
  call->CloseMediaChannel ();

  call->OpenFloorChannel (grpAddress, floorPort);
  call->OpenMediaChannel (grpAddress, speechPort);

  machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  machine.SetCallType (McpttCallMsgFieldCallType (McpttCallMsgFieldCallType::PRIVATE));
  machine.ChangeState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
}

McpttEntityId
McpttCallTypeMachinePrivateStateQ2::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallTypeMachinePrivateStateQ2::GetStateId ();
}

void
McpttCallTypeMachinePrivateStateQ2::ReceiveAccept (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  // This procedure is defined for q1 and Q2 so Q1's procedure is used here
  // for maintenance.
  McpttCallTypeMachinePrivateStateQ1::GetInstance ()->ReceiveAccept (machine, msg);
}

void
McpttCallTypeMachinePrivateStateQ2::ReceiveEmergencyCancel (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateEmergCancel& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  // This procedure is defined for q1 and Q2 so Q1's procedure is used here
  // for maintenance.
  McpttCallTypeMachinePrivateStateQ1::GetInstance ()->ReceiveEmergencyCancel (machine, msg);
}

void
McpttCallTypeMachinePrivateStateQ2::ReceiveReject (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateReject& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCallMachinePrivate> callMachine = machine.GetOwner ();

  Ptr<McpttTimer> tfp1 = callMachine->GetTfp1 ();

  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldCallId myCallId = callMachine->GetCallId ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      if (tfp1->IsRunning ())
        {
          tfp1->Stop ();
        }

      machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
      machine.SetCallType (McpttCallMsgFieldCallType (McpttCallMsgFieldCallType::PRIVATE));
      machine.ChangeState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
    }
}

void
McpttCallTypeMachinePrivateStateQ2::ReceiveSetupRequest (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  // This procedure is defined for q1 and Q2 so Q1's procedure is used here
  // for maintenance.
  McpttCallTypeMachinePrivateStateQ1::GetInstance ()->ReceiveSetupRequest (machine, msg);
}

void
McpttCallTypeMachinePrivateStateQ2::ReleaseCall (McpttCallTypeMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  // This procedure is defined for q1 and Q2 so Q1's procedure is used here
  // for maintenance.
  McpttCallTypeMachinePrivateStateQ1::GetInstance ()->ReleaseCall (machine);
}
/** McpttCallTypeMachinePrivateStateQ2 - end **/

} // namespace psc
} // namespace ns3
