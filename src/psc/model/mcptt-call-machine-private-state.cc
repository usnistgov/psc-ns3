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

#include "mcptt-call-machine-private.h"
#include "mcptt-call-msg.h"
#include "mcptt-call-msg-field.h"
#include "mcptt-call-type-machine-private.h"
#include "mcptt-floor-participant.h"
#include "mcptt-ptt-app.h"

#include "mcptt-call-machine-private-state.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallMachinePrivateState");

namespace psc {

/** McpttCallMachinePrivateState - begin **/
McpttCallMachinePrivateState::~McpttCallMachinePrivateState (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachinePrivateState::AcceptCall (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring call accept.");
}

void
McpttCallMachinePrivateState::ExpiryOfTfp1 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of TFP1.");
}

void
McpttCallMachinePrivateState::ExpiryOfTfp2 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of TFP2.");
}

void
McpttCallMachinePrivateState::ExpiryOfTfp3 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of TFP3.");
}

void
McpttCallMachinePrivateState::ExpiryOfTfp4 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of TFP4.");
}

void
McpttCallMachinePrivateState::ExpiryOfTfp5 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of TFP5.");
}

void
McpttCallMachinePrivateState::ExpiryOfTfp6 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of TFP6.");
}

void
McpttCallMachinePrivateState::ExpiryOfTfp7 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of TFP7.");
}

void
McpttCallMachinePrivateState::InitiateCall (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring initate call.");
}

bool
McpttCallMachinePrivateState::IsCallOngoing (const McpttCallMachinePrivate& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  return false;
}

void
McpttCallMachinePrivateState::ReceiveAccept (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallMachinePrivateState::ReceiveAcceptAck (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateAcceptAck& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallMachinePrivateState::ReceiveMedia (McpttCallMachinePrivate& machine, const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallMachinePrivateState::ReceiveReject (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateReject& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallMachinePrivateState::ReceiveRelease (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRelease& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallMachinePrivateState::ReceiveReleaseAck (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateReleaseAck& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallMachinePrivateState::ReceiveRinging (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRinging& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallMachinePrivateState::ReceiveSetupRequest (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetTypeId ().GetName () << " message.");
}

void
McpttCallMachinePrivateState::RejectCall (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring reject call.");
}

void
McpttCallMachinePrivateState::ReleaseCall (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring release call.");
}

McpttCallMachinePrivateState::McpttCallMachinePrivateState (void)
  : SimpleRefCount<McpttCallMachinePrivateState> ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttCallMachinePrivateState - end **/

std::ostream&
operator<< (std::ostream& os, const McpttCallMachinePrivateState& inst)
{
  McpttEntityId stateId = inst.GetInstanceStateId ();

  os << stateId;

  return os;
}

/** McpttCallMachinePrivateStateP0 - begin **/
Ptr<McpttCallMachinePrivateStateP0>
McpttCallMachinePrivateStateP0::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallMachinePrivateStateP0> instance = Create<McpttCallMachinePrivateStateP0> ();

  return instance;
}

McpttEntityId
McpttCallMachinePrivateStateP0::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (0, "'P0: start-stop'");

  return stateId;
}

McpttCallMachinePrivateStateP0::McpttCallMachinePrivateStateP0 (void)
  : McpttCallMachinePrivateState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachinePrivateStateP0::~McpttCallMachinePrivateStateP0 (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttCallMachinePrivateStateP0::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachinePrivateStateP0::GetStateId ();
}

void
McpttCallMachinePrivateStateP0::InitiateCall (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  UintegerValue targetId;
  AddressValue grpAddress;
  McpttCallMsgFieldCommMode commMode;
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttPttApp> pttApp = machine.GetCall ()->GetOwner ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  machine.GetAttribute ("TargetId", targetId);
  machine.GetCall ()->GetAttribute ("PeerAddress", grpAddress);

  Vector myLoc = pttApp->GetNodeLoc ();
  uint16_t callId = machine.GenerateRandomCallId ();
  NS_LOG_DEBUG ("Generate call ID of " << callId);
  uint32_t myUserId = pttApp->GetUserId ();
  uint32_t theirUserId = (uint32_t)targetId.Get ();
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  AddressValue origAddress = pttApp->GetLocalAddress ();
  McpttCallMsgFieldCallType callType = typeMachine->GetCallType ();

  machine.SetCallId (callId);
  machine.SetCallerId (myUserId);
  machine.SetCalleeId (theirUserId);

  if (machine.IsAutoCommencement ())
    {
      commMode.SetMode (McpttCallMsgFieldCommMode::AUTOMATIC_MODE);
    }
  else
    {
      commMode.SetMode (McpttCallMsgFieldCommMode::MANUAL_MODE);
    }

  machine.SetCommMode (commMode);

  typeMachine->Start ();

  //TODO: Check if an end-to-end security context needs to be established.

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress.Get ());
  sdp.SetSpeechPort (speechPort);

  machine.SetSdp (sdp);

  McpttCallMsgPrivateSetupReq reqMsg;
  reqMsg.SetCallId (callId);
  reqMsg.SetCallerId (myUserId);
  reqMsg.SetCalleeId (theirUserId);
  reqMsg.SetCommMode (commMode);
  reqMsg.SetCallType (callType);
  reqMsg.SetSdp (sdp);

  if (callType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_PRIVATE)
    {
      reqMsg.SetUserLoc (myLoc);
    }

  machine.Send (reqMsg);

  cfp1->Reset ();
  tfp1->Start ();

  if (tfp7->IsRunning ())
    {
      tfp7->Stop ();
    }

  machine.ReportEvent (McpttCallMachine::CALL_INITIATED);

  machine.ChangeState (McpttCallMachinePrivateStateP2::GetInstance ());
}

void
McpttCallMachinePrivateStateP0::ReceiveSetupRequest (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttPttApp> pttApp = call->GetOwner ();

  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  uint32_t myUserId = pttApp->GetUserId ();
  McpttCallMsgFieldSdp theirSdp = msg.GetSdp ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldUserId theirCallerId = msg.GetCallerId ();
  McpttCallMsgFieldUserId theirCalleeId = msg.GetCalleeId ();
  McpttCallMsgFieldCommMode commMode = msg.GetCommMode ();

  uint16_t floorPort = theirSdp.GetFloorPort ();
  uint16_t speechPort = theirSdp.GetSpeechPort ();
  Address grpAddress = theirSdp.GetGrpAddr ();

  if (myCallId.GetCallId () != theirCallId.GetCallId ())
    {
      if (theirCalleeId.GetId () == myUserId)
        {
          if (machine.IsFailure ()) //Media session CANNOT be established
            {
              machine.SetCallId (theirCallId);
              machine.SetCallerId (theirCallerId);
              machine.SetCalleeId (McpttCallMsgFieldUserId (myUserId));

              McpttCallMsgPrivateReject rejectMsg;
              rejectMsg.SetCallId (theirCallId);
              rejectMsg.SetCallerId (theirCallerId);
              rejectMsg.SetCalleeId (McpttCallMsgFieldUserId (myUserId));
              rejectMsg.SetReason (McpttCallMsgFieldReason (McpttCallMsgFieldReason::FAILED));

              machine.Send (rejectMsg);

              tfp7->Start ();

              if (machine.GetState ()->GetInstanceStateId () == McpttCallMachinePrivateStateP0::GetStateId ())
                {
                  machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
                }
            }
          else //Media session CAN be established
            {
              if (commMode.GetMode () == McpttCallMsgFieldCommMode::AUTOMATIC_MODE)
                {
                  machine.SetCallId (theirCallId);
                  machine.SetCallerId (theirCallerId);
                  machine.SetCalleeId (McpttCallMsgFieldUserId (myUserId));

                  typeMachine->Start ();

                  machine.SetSdp (theirSdp);

                  McpttCallMsgPrivateAccept acceptMsg;
                  acceptMsg.SetCallId (theirCallId);
                  acceptMsg.SetCallerId (theirCallerId);
                  acceptMsg.SetCalleeId (McpttCallMsgFieldUserId (myUserId));
                  acceptMsg.SetSdp (theirSdp);

                  machine.Send (acceptMsg);

                  call->OpenFloorChannel (grpAddress, floorPort);
                  call->OpenMediaChannel (grpAddress, speechPort);

                  if (tfp7->IsRunning ())
                    {
                      tfp7->Stop ();
                    }

                  cfp4->Reset ();
                  tfp4->Start ();

                  machine.ChangeState (McpttCallMachinePrivateStateP5::GetInstance ());
                }
              else if (commMode.GetMode () == McpttCallMsgFieldCommMode::MANUAL_MODE)
                {
                  machine.SetCallId (theirCallId);
                  machine.SetCallerId (theirCallerId);
                  machine.SetCalleeId (McpttCallMsgFieldUserId (myUserId));

                  typeMachine->Start ();

                  //TODO: Not in standard - When user accepts call need SDP to establish connection
                  machine.SetSdp (theirSdp);

                  McpttCallMsgPrivateRinging ringingMsg;
                  ringingMsg.SetCallId (theirCallId);
                  ringingMsg.SetCallerId (theirCallerId);
                  ringingMsg.SetCalleeId (McpttCallMsgFieldUserId (myUserId));

                  machine.Send (ringingMsg);

                  if (tfp7->IsRunning ())
                    {
                      tfp7->Stop ();
                    }

                  tfp2->Start ();

                  machine.ChangeState (McpttCallMachinePrivateStateP5::GetInstance ());

                  Simulator::Schedule (Seconds (0), &McpttCallMachinePrivate::NotifyNewCall, &machine, theirCallId.GetCallId ());
                }
            }
        }
    }
}
/** McpttCallMachinePrivateStateP0 - end **/

/** McpttCallMachinePrivateStateP1 - begin **/
Ptr<McpttCallMachinePrivateStateP1>
McpttCallMachinePrivateStateP1::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallMachinePrivateStateP1> instance = Create<McpttCallMachinePrivateStateP1> ();

  return instance;
}

McpttEntityId
McpttCallMachinePrivateStateP1::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (1, "'P1: ignoring same call id'");

  return stateId;
}

McpttCallMachinePrivateStateP1::McpttCallMachinePrivateStateP1 (void)
  : McpttCallMachinePrivateState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachinePrivateStateP1::~McpttCallMachinePrivateStateP1 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachinePrivateStateP1::ExpiryOfTfp7 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  machine.SetCallId (McpttCallMsgFieldCallId ());

  machine.ChangeState (McpttCallMachinePrivateStateP0::GetInstance ());
}

McpttEntityId
McpttCallMachinePrivateStateP1::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachinePrivateStateP1::GetStateId ();
}

void
McpttCallMachinePrivateStateP1::InitiateCall (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  // This procedure is defined for P0 and P1 so P0's procedure is used here
  // for maintenance.
  McpttCallMachinePrivateStateP0::GetInstance ()->InitiateCall (machine);
}

void
McpttCallMachinePrivateStateP1::ReceiveRelease (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRelease& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      McpttCallMsgPrivateReleaseAck ackMsg;
      ackMsg.SetCallId (machine.GetCallId ());
      ackMsg.SetCallerId (machine.GetCallerId ());
      ackMsg.SetCalleeId (machine.GetCalleeId ());

      machine.Send (ackMsg);

      if (tfp4->IsRunning ())
        {
          tfp4->Stop ();
        }

      if (machine.GetState ()->GetInstanceStateId () == McpttCallMachinePrivateStateP5::GetStateId ())
        {
          tfp7->Start ();

          typeMachine->Stop ();

          machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
        }
    }
}

void
McpttCallMachinePrivateStateP1::ReceiveSetupRequest (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  // This procedure is defined for P0 and P1 so P0's procedure is used here
  // for maintenance.
  McpttCallMachinePrivateStateP0::GetInstance ()->ReceiveSetupRequest (machine, msg);
}
/** McpttCallMachinePrivateStateP1 - end **/

/** McpttCallMachinePrivateStateP2 - begin **/
Ptr<McpttCallMachinePrivateStateP2>
McpttCallMachinePrivateStateP2::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallMachinePrivateStateP2> instance = Create<McpttCallMachinePrivateStateP2> ();

  return instance;
}

McpttEntityId
McpttCallMachinePrivateStateP2::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (2, "'P2: waiting for call response'");

  return stateId;
}

McpttCallMachinePrivateStateP2::McpttCallMachinePrivateStateP2 (void)
  : McpttCallMachinePrivateState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachinePrivateStateP2::~McpttCallMachinePrivateStateP2 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachinePrivateStateP2::ExpiryOfTfp1 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttPttApp> pttApp = machine.GetCall ()->GetOwner ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  Vector myLoc = pttApp->GetNodeLoc ();
  McpttCallMsgFieldSdp sdp = machine.GetSdp ();
  McpttCallMsgFieldCallId callId = machine.GetCallId ();
  McpttCallMsgFieldUserId callerId = machine.GetCallerId ();
  McpttCallMsgFieldUserId calleeId = machine.GetCalleeId ();
  McpttCallMsgFieldCommMode commMode = machine.GetCommMode ();
  McpttCallMsgFieldCallType callType = typeMachine->GetCallType ();

  if (cfp1->IsLimitReached ())
    {
      if (commMode.GetMode () == McpttCallMsgFieldCommMode::AUTOMATIC_MODE)
        {
          tfp7->Start ();

          machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
        }
      else if (commMode.GetMode () == McpttCallMsgFieldCommMode::MANUAL_MODE)
        {
          tfp2->Start ();

          machine.ChangeState (McpttCallMachinePrivateStateP2::GetInstance ());
        }
    }
  else
    {
      cfp1->Increment ();

      McpttCallMsgPrivateSetupReq reqMsg;
      reqMsg.SetCallId (callId);
      reqMsg.SetCallerId (callerId);
      reqMsg.SetCalleeId (calleeId);
      reqMsg.SetCommMode (commMode);
      reqMsg.SetCallType (callType);
      reqMsg.SetSdp (sdp);

      if (callType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_PRIVATE)
        {
          reqMsg.SetUserLoc (myLoc);
        }

      machine.Send (reqMsg);

      tfp1->Start ();
    }
}

void
McpttCallMachinePrivateStateP2::ExpiryOfTfp2 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  tfp7->Start ();

  typeMachine->Stop ();

  machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
}

McpttEntityId
McpttCallMachinePrivateStateP2::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachinePrivateStateP2::GetStateId ();
}

void
McpttCallMachinePrivateStateP2::ReceiveAccept (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  McpttCallMsgFieldSdp theirSdp = msg.GetSdp ();
  McpttCallMsgFieldUserId callerId = machine.GetCallerId ();
  McpttCallMsgFieldUserId calleeId = machine.GetCalleeId ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();

  uint16_t floorPort = theirSdp.GetFloorPort ();
  uint16_t speechPort = theirSdp.GetSpeechPort ();
  Address grpAddress = theirSdp.GetGrpAddr ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      machine.SetSdp (theirSdp);

      McpttCallMsgPrivateAcceptAck acceptAck;
      acceptAck.SetCallId (myCallId);
      acceptAck.SetCallerId (callerId);
      acceptAck.SetCalleeId (calleeId);

      machine.Send (acceptAck);

      if (tfp1->IsRunning ())
        {
          tfp1->Stop ();
        }

      if (tfp2->IsRunning ())
        {
          tfp2->Stop ();
        }

      call->OpenFloorChannel (grpAddress, floorPort);
      call->OpenMediaChannel (grpAddress, speechPort);

      floorMachine->SetOriginator (true);
      floorMachine->Start ();

      tfp5->Start ();

      machine.ChangeState (McpttCallMachinePrivateStateP4::GetInstance ());
    }
}

void
McpttCallMachinePrivateStateP2::ReceiveReject (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateReject& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      if (tfp1->IsRunning ())
        {
          tfp1->Stop ();
        }

      if (tfp2->IsRunning ())
        {
          tfp2->Stop ();
        }

      tfp7->Start ();

      typeMachine->Stop ();

      machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
    }
}

void
McpttCallMachinePrivateStateP2::ReceiveRinging (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRinging& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();

  if (theirCallId.GetCallId () == myCallId.GetCallId ())
    {
      //Do nothing.
    }
}

void
McpttCallMachinePrivateStateP2::ReleaseCall (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldUserId myCallerId = machine.GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = machine.GetCalleeId ();

  McpttCallMsgPrivateRelease releaseMsg;
  releaseMsg.SetCallId (myCallId);
  releaseMsg.SetCallerId (myCallerId);
  releaseMsg.SetCalleeId (myCalleeId);

  machine.Send (releaseMsg);

  tfp3->Start ();

  machine.ChangeState (McpttCallMachinePrivateStateP3::GetInstance ());
}
/** McpttCallMachinePrivateStateP2 - end **/

/** McpttCallMachinePrivateStateP3 - begin **/
Ptr<McpttCallMachinePrivateStateP3>
McpttCallMachinePrivateStateP3::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallMachinePrivateStateP3> instance = Create<McpttCallMachinePrivateStateP3> ();

  return instance;
}

McpttEntityId
McpttCallMachinePrivateStateP3::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (3, "'P3: waiting for release response'");

  return stateId;
}

McpttCallMachinePrivateStateP3::McpttCallMachinePrivateStateP3 (void)
  : McpttCallMachinePrivateState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachinePrivateStateP3::~McpttCallMachinePrivateStateP3 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachinePrivateStateP3::ExpiryOfTfp3 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldUserId myCallerId = machine.GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = machine.GetCalleeId ();

  if (cfp3->IsLimitReached ())
    {
      call->CloseMediaChannel ();
      call->CloseFloorChannel ();

      tfp7->Start ();

      typeMachine->Stop ();

      machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
    }
  else
    {
      McpttCallMsgPrivateRelease releaseMsg;
      releaseMsg.SetCallId (myCallId);
      releaseMsg.SetCallerId (myCallerId);
      releaseMsg.SetCalleeId (myCalleeId);

      machine.Send (releaseMsg);

      cfp3->Increment ();

      tfp3->Start ();
    }
}

McpttEntityId
McpttCallMachinePrivateStateP3::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachinePrivateStateP3::GetStateId ();
}

void
McpttCallMachinePrivateStateP3::ReceiveReleaseAck (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateReleaseAck& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      tfp3->Stop ();

      if (call->IsMediaChannelOpen ())
        {
          call->CloseMediaChannel ();
        }
      if (call->IsFloorChannelOpen ())
        {
          call->CloseFloorChannel ();
        }

      tfp7->Start ();

      typeMachine->Stop ();

      machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
    }
}
/** McpttCallMachinePrivateStateP3 - end **/

/** McpttCallMachinePrivateStateP4 - begin **/
Ptr<McpttCallMachinePrivateStateP4>
McpttCallMachinePrivateStateP4::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallMachinePrivateStateP4> instance = Create<McpttCallMachinePrivateStateP4> ();

  return instance;
}

McpttEntityId
McpttCallMachinePrivateStateP4::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (4, "'P4: part of ongoing call'");

  return stateId;
}

McpttCallMachinePrivateStateP4::McpttCallMachinePrivateStateP4 (void)
  : McpttCallMachinePrivateState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachinePrivateStateP4::~McpttCallMachinePrivateStateP4 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachinePrivateStateP4::ExpiryOfTfp1 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  if (cfp1->IsLimitReached ())
    {
      tfp7->Start ();

      typeMachine->Stop ();

      tfp5->Stop ();

      machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
    }
}

void
McpttCallMachinePrivateStateP4::ExpiryOfTfp5 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  call->CloseMediaChannel ();
  call->CloseFloorChannel ();

  tfp7->Start ();

  typeMachine->Stop ();

  machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
}

void
McpttCallMachinePrivateStateP4::ExpiryOfTfp6 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  if (cfp6->IsLimitReached ())
    {
      tfp7->Start ();

      typeMachine->Stop ();

      tfp5->Stop ();

      machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
    }
}

McpttEntityId
McpttCallMachinePrivateStateP4::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachinePrivateStateP4::GetStateId ();
}

bool
McpttCallMachinePrivateStateP4::IsCallOngoing (const McpttCallMachinePrivate& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  return true;
}

void
McpttCallMachinePrivateStateP4::ReceiveRelease (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRelease& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldUserId myCallerId = machine.GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = machine.GetCalleeId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      McpttCallMsgPrivateReleaseAck releaseAckMsg;
      releaseAckMsg.SetCallId (myCallId);
      releaseAckMsg.SetCallerId (myCallerId);
      releaseAckMsg.SetCalleeId (myCalleeId);

      machine.Send (releaseAckMsg);

      call->CloseMediaChannel ();
      call->CloseFloorChannel ();

      tfp7->Start ();

      typeMachine->Stop ();

      tfp5->Stop ();

      machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
    }
}

void
McpttCallMachinePrivateStateP4::ReleaseCall (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldUserId myCallerId = machine.GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = machine.GetCalleeId ();

  McpttCallMsgPrivateRelease releaseMsg;
  releaseMsg.SetCallId (myCallId);
  releaseMsg.SetCallerId (myCallerId);
  releaseMsg.SetCalleeId (myCalleeId);

  machine.Send (releaseMsg);

  cfp3->Reset ();

  tfp3->Start ();

  tfp5->Stop ();

  machine.ChangeState (McpttCallMachinePrivateStateP3::GetInstance ());
}
/** McpttCallMachinePrivateStateP4 - end **/

/** McpttCallMachinePrivateStateP5 - begin **/
Ptr<McpttCallMachinePrivateStateP5>
McpttCallMachinePrivateStateP5::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallMachinePrivateStateP5> instance = Create<McpttCallMachinePrivateStateP5> ();

  return instance;
}

McpttEntityId
McpttCallMachinePrivateStateP5::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (5, "'P5: pending'");

  return stateId;
}

McpttCallMachinePrivateStateP5::McpttCallMachinePrivateStateP5 (void)
  : McpttCallMachinePrivateState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachinePrivateStateP5::~McpttCallMachinePrivateStateP5 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachinePrivateStateP5::AcceptCall (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldUserId myCallerId = machine.GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = machine.GetCalleeId ();
  McpttCallMsgFieldSdp mySdp = machine.GetSdp ();

  uint16_t floorPort = mySdp.GetFloorPort ();
  uint16_t speechPort = mySdp.GetSpeechPort ();
  Address grpAddress = mySdp.GetGrpAddr ();

  McpttCallMsgPrivateAccept acceptMsg;
  acceptMsg.SetCallId (myCallId);
  acceptMsg.SetCallerId (myCallerId);
  acceptMsg.SetCalleeId (myCalleeId);
  acceptMsg.SetSdp (mySdp);

  machine.Send (acceptMsg);

  call->OpenFloorChannel (grpAddress, floorPort);
  call->OpenMediaChannel (grpAddress, speechPort);

  tfp2->Stop ();

  cfp4->Reset ();

  tfp4->Start ();
}

void
McpttCallMachinePrivateStateP5::ExpiryOfTfp2 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldUserId myCallerId = machine.GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = machine.GetCalleeId ();

  McpttCallMsgPrivateReject rejectMsg;
  rejectMsg.SetCallId (myCallId);
  rejectMsg.SetCallerId (myCallerId);
  rejectMsg.SetCalleeId (myCalleeId);
  rejectMsg.SetReason (McpttCallMsgFieldReason (McpttCallMsgFieldReason::FAILED));

  machine.Send (rejectMsg);

  tfp7->Start ();

  typeMachine->Stop ();

  machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
}

void
McpttCallMachinePrivateStateP5::ExpiryOfTfp4 (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  McpttCallMsgFieldSdp mySdp = machine.GetSdp ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldUserId myCallerId = machine.GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = machine.GetCalleeId ();

  if (cfp4->IsLimitReached ())
    {
      tfp7->Start ();

      typeMachine->Stop ();

      machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
    }
  else
    {
      McpttCallMsgPrivateAccept acceptMsg;
      acceptMsg.SetCallId (myCallId);
      acceptMsg.SetCallerId (myCallerId);
      acceptMsg.SetCalleeId (myCalleeId);
      acceptMsg.SetSdp (mySdp);

      machine.Send (acceptMsg);

      cfp4->Increment ();
      tfp4->Start ();
    }
}

McpttEntityId
McpttCallMachinePrivateStateP5::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachinePrivateStateP5::GetStateId ();
}

void
McpttCallMachinePrivateStateP5::ReceiveAcceptAck (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateAcceptAck& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldUserId myCallerId = machine.GetCallerId ();
  McpttCallMsgFieldUserId theirCallerId = msg.GetCallerId ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ()
      && myCallerId.GetId () == theirCallerId.GetId ())
    {
      tfp4->Stop ();

      floorMachine->SetOriginator (false);
      floorMachine->Start ();

      tfp5->Start ();

      machine.ChangeState (McpttCallMachinePrivateStateP4::GetInstance ());
    }
}

void
McpttCallMachinePrivateStateP5::ReceiveMedia (McpttCallMachinePrivate& machine, const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();

  uint32_t theirSsrc = msg.GetSsrc ();
  McpttCallMsgFieldUserId myCallerId = machine.GetCallerId ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  //Check if the call is from the originator
  if (myCallerId.GetId () == theirSsrc)
    {
      tfp4->Stop ();

      floorMachine->SetOriginator (false);
      floorMachine->Start ();

      tfp5->Start ();

      machine.ChangeState (McpttCallMachinePrivateStateP4::GetInstance ());
    }
}

void
McpttCallMachinePrivateStateP5::ReceiveRelease (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRelease& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  McpttCallMachinePrivateStateP1::GetInstance ()->ReceiveRelease (machine, msg);
}

void
McpttCallMachinePrivateStateP5::RejectCall (McpttCallMachinePrivate& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttCallTypeMachinePrivate> typeMachine = machine.GetTypeMachine ();

  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldUserId myCallerId = machine.GetCallerId ();
  McpttCallMsgFieldUserId myCalleeId = machine.GetCalleeId ();

  McpttCallMsgPrivateReject rejectMsg;
  rejectMsg.SetCallId (myCallId);
  rejectMsg.SetCallerId (myCallerId);
  rejectMsg.SetCalleeId (myCalleeId);
  rejectMsg.SetReason (McpttCallMsgFieldReason (McpttCallMsgFieldReason::FAILED));

  machine.Send (rejectMsg);

  tfp7->Start ();

  typeMachine->Stop ();

  machine.ChangeState (McpttCallMachinePrivateStateP1::GetInstance ());
}
/** McpttCallMachinePrivateStateP5 - end **/

} // namespace psc
} // namespace ns3

