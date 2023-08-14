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

#include "mcptt-call-machine-grp-basic.h"
#include "mcptt-call-msg.h"
#include "mcptt-floor-participant.h"
#include "mcptt-ptt-app.h"

#include "mcptt-call-machine-grp-basic-state.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallMachineGrpBasicState");

namespace psc {

/** McpttCallMachineGrpBasicState - begin **/
McpttCallMachineGrpBasicState::~McpttCallMachineGrpBasicState (void)
{
  NS_LOG_FUNCTION (this);
}
void
McpttCallMachineGrpBasicState::AcceptCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);
  NS_LOG_LOGIC ("Ignoring call accept.");
}

void
McpttCallMachineGrpBasicState::ExpiryOfTfg1 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);
  NS_LOG_LOGIC ("Ignoring expiry of TFG1.");
}

void
McpttCallMachineGrpBasicState::ExpiryOfTfg2 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);
  NS_LOG_LOGIC ("Ignoring expiry of TFG2.");
}

void
McpttCallMachineGrpBasicState::ExpiryOfTfg3 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);
  NS_LOG_LOGIC ("Ignoring expiry of TFG3.");
}

void
McpttCallMachineGrpBasicState::ExpiryOfTfg4 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);
  NS_LOG_LOGIC ("Ignoring expiry of TFG4.");
}

void
McpttCallMachineGrpBasicState::ExpiryOfTfg5 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);
  NS_LOG_LOGIC ("Ignoring expiry of TFG5.");
}

void
McpttCallMachineGrpBasicState::ExpiryOfTfg6 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);
  NS_LOG_LOGIC ("Ignoring expiry of TFG6.");
}

McpttEntityId
McpttCallMachineGrpBasicState::GetInstanceStateId (void) const
{
  return McpttEntityId ();
}

bool
McpttCallMachineGrpBasicState::IsCallOngoing (const McpttCallMachineGrpBasic& machine) const
{
  return false;
}

void
McpttCallMachineGrpBasicState::InitiateCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);
  NS_LOG_LOGIC ("Ignoring initate call.");
}

void
McpttCallMachineGrpBasicState::ReceiveGrpCallAccept (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAccept& msg)
{
  NS_LOG_FUNCTION (this << &machine << msg);
  NS_LOG_LOGIC ("Ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallMachineGrpBasicState::ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun & msg)
{
  NS_LOG_FUNCTION (this << &machine << msg);
  NS_LOG_LOGIC ("Ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallMachineGrpBasicState::ReceiveGrpCallProbe (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpProbe& msg)
{
  NS_LOG_FUNCTION (this << &machine << msg);
  NS_LOG_LOGIC ("Ignoring " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallMachineGrpBasicState::RejectCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);
  NS_LOG_LOGIC ("Ignoring reject call.");
}

void
McpttCallMachineGrpBasicState::ReleaseCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  NS_LOG_LOGIC ("Ignoring release call.");
}

McpttCallMachineGrpBasicState::McpttCallMachineGrpBasicState (void)
  : SimpleRefCount<McpttCallMachineGrpBasicState> ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttCallMachineGrpBasicState - end **/

std::ostream&
operator<< (std::ostream& os, const McpttCallMachineGrpBasicState& inst)
{
  McpttEntityId stateId = inst.GetInstanceStateId ();

  os << stateId;

  return os;
}

/** McpttCallMachineGrpBasicStateS1 - begin **/
Ptr<McpttCallMachineGrpBasicStateS1>
McpttCallMachineGrpBasicStateS1::GetInstance (void)
{
  static Ptr<McpttCallMachineGrpBasicStateS1> instance = Create<McpttCallMachineGrpBasicStateS1> ();
  return instance;
}

McpttEntityId
McpttCallMachineGrpBasicStateS1::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (1, "'S1: start-stop'");
  return stateId;
}

McpttCallMachineGrpBasicStateS1::McpttCallMachineGrpBasicStateS1 (void)
  : McpttCallMachineGrpBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachineGrpBasicStateS1::~McpttCallMachineGrpBasicStateS1 (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttCallMachineGrpBasicStateS1::GetInstanceStateId (void) const
{
  return McpttCallMachineGrpBasicStateS1::GetStateId ();
}

void
McpttCallMachineGrpBasicStateS1::InitiateCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = machine.GetTypeMachine ();

  uint32_t grpId = machine.GetGrpId ().GetGrpId ();
  uint16_t callId = machine.GenerateRandomCallId ();

  machine.SetCallId (callId);
  machine.SetGrpId (grpId);

  typeMachine->Start ();

  McpttCallMsgGrpProbe msg;
  msg.SetGrpId (grpId);

  tfg3->Start ();
  tfg1->Start ();

  machine.Send (msg);

  machine.ReportEvent (McpttCallMachine::CALL_INITIATED);

  machine.ChangeState (McpttCallMachineGrpBasicStateS2::GetInstance ());
}

void
McpttCallMachineGrpBasicStateS1::ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun& msg)
{
  NS_LOG_FUNCTION (this << &machine << msg);

  Time delayTfg2;
  Time delayTfg6;
  uint32_t myUserId = machine.GetCall ()->GetOwner ()->GetUserId ();
  bool userAckReq = machine.IsUserAckReq ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = machine.GetTypeMachine ();
  McpttCallMsgFieldCallType myCallType = typeMachine->GetCallType ();
  McpttCallMsgFieldSdp theirSdp = msg.GetSdp ();
  McpttCallMsgFieldGrpId theirGrpId = msg.GetGrpId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldUserId theirOrigId = msg.GetOrigId ();
  McpttCallMsgFieldRefreshInterval theirRefInt = msg.GetRefInt ();
  McpttCallMsgFieldStartTime theirStartTime = msg.GetStartTime ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  //TODO: The following check should make sure that the received group ID
  //TODO: does not match the group ID of any other call machines that may
  //TODO: exist in this application before executing.
  if (myGrpId.GetGrpId () == theirGrpId.GetGrpId ())
    {
      machine.SetSdp (theirSdp);
      machine.SetCallId (theirCallId);
      machine.SetOrigId (theirOrigId);
      machine.SetRefInt (theirRefInt);
      machine.SetGrpId (theirGrpId);
      machine.SetStartTime (theirStartTime);

      typeMachine->Start ();

      if (userAckReq == true)
        {
          tfg4->Start ();

          if (msg.IsConfirmModeIndicated ())
            {
              machine.ChangeState (McpttCallMachineGrpBasicStateS5::GetInstance ());
            }
          else
            {
              machine.ChangeState (McpttCallMachineGrpBasicStateS4::GetInstance ());
            }

          Simulator::Schedule (Seconds (0), &McpttCallMachineGrpBasic::NotifyNewCall, &machine, theirCallId.GetCallId ());
        }
      else
        {
          McpttCallMsgFieldSdp mySdp = machine.GetSdp ();

          uint16_t floorPort = mySdp.GetFloorPort ();
          Address grpAddr = mySdp.GetGrpAddr ();
          uint16_t speechPort = mySdp.GetSpeechPort ();

          call->OpenFloorChannel (grpAddr, floorPort);
          call->OpenMediaChannel (grpAddr, speechPort);

          floorMachine->SetOriginator (false);
          floorMachine->Start ();

          delayTfg6 = machine.CalcDelayForMaxDuration ();
          tfg6->SetDelay (delayTfg6);
          tfg6->Start ();

          delayTfg2 = machine.CalcDelayForPeriodicAnnoun ();
          tfg2->SetDelay (delayTfg2);
          tfg2->Start ();

          if (msg.IsConfirmModeIndicated ())
            {
              McpttCallMsgGrpAccept myMsg;
              myMsg.SetCallId (myCallId);
              myMsg.SetUserId (myUserId);
              myMsg.SetCallType (myCallType);
              myMsg.SetGrpId (myGrpId);

              machine.Send (myMsg);
            }

          machine.ChangeState (McpttCallMachineGrpBasicStateS3::GetInstance ());
        }
    }
}
/** McpttCallMachineGrpBasicStateS1 - end **/

/** McpttCallMachineGrpBasicStateS2 - begin **/
Ptr<McpttCallMachineGrpBasicStateS2>
McpttCallMachineGrpBasicStateS2::GetInstance (void)
{
  static Ptr<McpttCallMachineGrpBasicStateS2> instance = Create<McpttCallMachineGrpBasicStateS2> ();
  return instance;
}

McpttEntityId
McpttCallMachineGrpBasicStateS2::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (2, "'S2: waiting for call announcement'");
  return stateId;
}

McpttCallMachineGrpBasicStateS2::McpttCallMachineGrpBasicStateS2 (void)
  : McpttCallMachineGrpBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachineGrpBasicStateS2::~McpttCallMachineGrpBasicStateS2 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBasicStateS2::ExpiryOfTfg1 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Time delayTfg2;
  Time delayTfg6;
  Time callStartTime = Simulator::Now ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttPttApp> pttApp = call->GetOwner ();
  bool isConfirmMode = machine.IsConfirmMode ();
  McpttCallMsgFieldGrpId grpId = machine.GetGrpId ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = machine.GetTypeMachine ();

  uint32_t myUserId = machine.GetCall ()->GetOwner ()->GetUserId ();
  uint16_t callId = machine.GetCallId ().GetCallId ();
  NS_LOG_DEBUG ("Generate call ID of " << callId);
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  AddressValue grpAddress;
  Address origAddress = pttApp->GetLocalAddress ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();
  McpttCallMsgFieldRefreshInterval selectedRefInt = machine.GetRefInt ();
  McpttCallMsgFieldUserId lastChgUserId = typeMachine->GetLastChgUserId ();
  McpttCallMsgFieldLastChgTime lastChgTime = typeMachine->GetLastChgTime ();

  call->GetAttribute ("PeerAddress", grpAddress);

  //TODO: Not in standard - there is no check to stop TFG3.
  if (tfg3->IsRunning ())
    {
      tfg3->Stop ();
    }

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  machine.SetSdp (sdp);
  machine.SetCallId (callId);
  machine.SetRefInt (selectedRefInt);
  machine.SetOrigId (myUserId);
  machine.SetStartTime (callStartTime);

  McpttCallMsgGrpAnnoun msg;
  msg.SetCallId (callId);
  msg.SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  msg.SetRefInt (selectedRefInt);
  msg.SetSdp (sdp);
  msg.SetOrigId (myUserId);
  msg.SetGrpId (grpId);
  msg.SetStartTime (callStartTime);
  msg.SetLastChgTime (lastChgTime);
  msg.SetLastChgUserId (lastChgUserId);
  msg.SetConfirmMode (isConfirmMode);

  delayTfg6 = machine.CalcDelayForMaxDuration ();
  tfg6->SetDelay (delayTfg6);
  tfg6->Start ();

  delayTfg2 = machine.CalcDelayForPeriodicAnnoun ();
  tfg2->SetDelay (delayTfg2);
  tfg2->Start ();

  machine.Send (msg);

  call->OpenFloorChannel (grpAddress.Get (), floorPort);
  call->OpenMediaChannel (grpAddress.Get (), speechPort);

  floorMachine->SetOriginator (true);
  floorMachine->Start ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS3::GetInstance ());
}

void
McpttCallMachineGrpBasicStateS2::ExpiryOfTfg3 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCounter> cfg3 = machine.GetCfg3 ();
  McpttCallMsgFieldGrpId grpId = machine.GetGrpId ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();

  //TODO: Not in standard - there is no counter CFG3.
  if (cfg3->IsLimitReached ())
    {
      cfg3->Reset ();
    }
  else
    {
      McpttCallMsgGrpProbe msg;
      msg.SetGrpId (grpId);

      cfg3->Increment ();
      tfg3->Start ();

      machine.Send (msg);
    }
}

McpttEntityId
McpttCallMachineGrpBasicStateS2::GetInstanceStateId (void) const
{
  return McpttCallMachineGrpBasicStateS2::GetStateId ();
}

void
McpttCallMachineGrpBasicStateS2::ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun& msg)
{
  NS_LOG_FUNCTION (this << &machine << msg);

  Time delayTfg2;
  Time delayTfg6;
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCall> parent = machine.GetCall ();
  McpttCallMsgFieldSdp theirSdp = msg.GetSdp ();
  McpttCallMsgFieldGrpId theirGrpId = msg.GetGrpId ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();
  McpttCallMsgFieldUserId theirOrigId = msg.GetOrigId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldStartTime theirStartTime = msg.GetStartTime ();
  McpttCallMsgFieldRefreshInterval theirRefInt = msg.GetRefInt ();
  Ptr<McpttFloorParticipant> floorMachine = parent->GetFloorMachine ();
  uint16_t floorPort = theirSdp.GetFloorPort ();
  Address grpAddr = theirSdp.GetGrpAddr ();
  uint16_t speechPort = theirSdp.GetSpeechPort ();

  if (myGrpId.GetGrpId () == theirGrpId.GetGrpId ())
    {
      //TODO: Not in standard - there is no check to stop TFG3.
      if (tfg3->IsRunning ())
        {
          tfg3->Stop ();
        }

      tfg1->Stop ();

      machine.SetSdp (theirSdp);
      machine.SetCallId (theirCallId);
      machine.SetOrigId (theirOrigId);
      machine.SetRefInt (theirRefInt);
      machine.SetStartTime (theirStartTime);

      parent->OpenFloorChannel (grpAddr, floorPort);
      parent->OpenMediaChannel (grpAddr, speechPort);

      floorMachine->SetOriginator (false);
      floorMachine->Start ();

      delayTfg6 = machine.CalcDelayForMaxDuration ();
      tfg6->SetDelay (delayTfg6);
      tfg6->Start ();

      delayTfg2 = machine.CalcDelayForPeriodicAnnoun ();
      tfg2->SetDelay (delayTfg2);
      tfg2->Start ();

      machine.ChangeState (McpttCallMachineGrpBasicStateS3::GetInstance ());
    }
}

void
McpttCallMachineGrpBasicStateS2::ReleaseCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();

  tfg3->Stop ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS7::GetInstance ());
}
/** McpttCallMachineGrpBasicStateS2 - end **/

/** McpttCallMachineGrpBasicStateS3 - begin **/
Ptr<McpttCallMachineGrpBasicStateS3>
McpttCallMachineGrpBasicStateS3::GetInstance (void)
{
  static Ptr<McpttCallMachineGrpBasicStateS3> instance = Create<McpttCallMachineGrpBasicStateS3> ();

  return instance;
}

McpttEntityId
McpttCallMachineGrpBasicStateS3::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (3, "'S3: part of ongoing call'");

  return stateId;
}

McpttCallMachineGrpBasicStateS3::McpttCallMachineGrpBasicStateS3 (void)
  : McpttCallMachineGrpBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachineGrpBasicStateS3::~McpttCallMachineGrpBasicStateS3 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBasicStateS3::ExpiryOfTfg2 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this);

  Time delayTfg2;
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  bool probeRespVal = machine.GetProbeRespVal ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = machine.GetTypeMachine ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldCallType myCallType = typeMachine->GetCallType ();
  McpttCallMsgFieldRefreshInterval myRefInt = machine.GetRefInt ();
  McpttCallMsgFieldSdp mySdp = machine.GetSdp ();
  McpttCallMsgFieldUserId myOrigId = machine.GetOrigId ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();
  McpttCallMsgFieldLastChgTime myLastChgTime = typeMachine->GetLastChgTime ();
  McpttCallMsgFieldUserId myLastChgUserId = typeMachine->GetLastChgUserId ();
  McpttCallMsgFieldStartTime myStartTime = machine.GetStartTime ();

  McpttCallMsgGrpAnnoun msg;
  msg.SetCallId (myCallId);
  msg.SetCallType (myCallType);
  msg.SetRefInt (myRefInt);
  msg.SetSdp (mySdp);
  msg.SetOrigId (myOrigId);
  msg.SetGrpId (myGrpId);
  msg.SetLastChgTime (myLastChgTime);
  msg.SetLastChgUserId (myLastChgUserId);
  msg.SetStartTime (myStartTime);

  if (probeRespVal == true)
    {
      msg.SetProbeResp (true);
      machine.SetProbeRespVal (false);
    }

  delayTfg2 = machine.CalcDelayForPeriodicAnnoun ();
  tfg2->SetDelay (delayTfg2);
  tfg2->Start ();

  machine.Send (msg);
}

void
McpttCallMachineGrpBasicStateS3::ExpiryOfTfg6 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttCall> parent = machine.GetCall ();
  Ptr<McpttFloorParticipant> floorMachine = parent->GetFloorMachine ();

  floorMachine->Stop ();

  parent->CloseFloorChannel ();
  parent->CloseMediaChannel ();

  tfg2->Stop ();

  tfg5->Start ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS6::GetInstance ());
}

McpttEntityId
McpttCallMachineGrpBasicStateS3::GetInstanceStateId (void) const
{
  return McpttCallMachineGrpBasicStateS3::GetStateId ();
}

bool
McpttCallMachineGrpBasicStateS3::IsCallOngoing (const McpttCallMachineGrpBasic& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  return true;
}

void
McpttCallMachineGrpBasicStateS3::ReceiveGrpCallAccept (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAccept& msg)
{
  NS_LOG_FUNCTION (this << &machine << msg);

  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();
  McpttCallMsgFieldGrpId theirGrpId = msg.GetGrpId ();

  if (myGrpId.GetGrpId () == theirGrpId.GetGrpId ())
    {
      // Notify user about the call accept.
    }
}

void
McpttCallMachineGrpBasicStateS3::ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun& msg)
{
  NS_LOG_FUNCTION (this << &machine << msg);

  Time delayTfg2;
  Time delayTfg6;
  Ptr<McpttCall> parent = machine.GetCall ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  bool myProbeRespVal = machine.GetProbeRespVal ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();
  McpttCallMsgFieldUserId myOrigId = machine.GetOrigId ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldStartTime myStartTime = machine.GetStartTime ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = machine.GetTypeMachine ();
  McpttCallMsgFieldCallType myCallType = typeMachine->GetCallType ();
  McpttCallMsgFieldUserId myLastChgUserId = typeMachine->GetLastChgUserId ();
  McpttCallMsgFieldLastChgTime myLastChgTime = typeMachine->GetLastChgTime ();

  McpttCallMsgFieldSdp theirSdp = msg.GetSdp ();
  McpttCallMsgFieldGrpId theirGrpId = msg.GetGrpId ();
  McpttCallMsgFieldUserId theirOrigId = msg.GetOrigId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldCallType theirCallType = msg.GetCallType ();
  McpttCallMsgFieldRefreshInterval theirRefInt = msg.GetRefInt ();
  McpttCallMsgFieldStartTime theirStartTime = msg.GetStartTime ();
  McpttCallMsgFieldUserId theirLastChgUserId = msg.GetLastChgUserId ();
  McpttCallMsgFieldLastChgTime theirLastChgTime = msg.GetLastChgTime ();

  Ptr<McpttFloorParticipant> floorMachine = parent->GetFloorMachine ();
  uint16_t floorPort = theirSdp.GetFloorPort ();
  Address grpAddr = theirSdp.GetGrpAddr ();
  uint16_t speechPort = theirSdp.GetSpeechPort ();

  if (myGrpId.GetGrpId () == theirGrpId.GetGrpId ()
      && (myProbeRespVal == false
          || (myProbeRespVal == true && msg.IsProbeResp ()))
      && myGrpId.GetGrpId () == theirGrpId.GetGrpId ()
      && myStartTime.GetTime () == theirStartTime.GetTime ()
      && myLastChgTime.GetTime () == theirLastChgTime.GetTime ()
      && myLastChgUserId.GetId () == theirLastChgUserId.GetId ()
      && myCallId.GetCallId () == theirCallId.GetCallId ()
      && myCallType.GetType () == theirCallType.GetType ())
    {
      tfg2->Stop ();
      delayTfg2 = machine.CalcDelayForPeriodicAnnoun ();
      tfg2->SetDelay (delayTfg2);
      tfg2->Start ();
      machine.SetProbeRespVal (false);
    }
  else if (myGrpId.GetGrpId () == theirGrpId.GetGrpId ()
           && (myOrigId.GetId () != theirOrigId.GetId ()
               || myCallId.GetCallId () != theirCallId.GetCallId ()))
    {
      bool clause1 = (myCallType.GetType () == McpttCallMsgFieldCallType::BASIC_GROUP)
        && (theirCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP
            || theirCallType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
      bool clause2 = (myCallType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
        && (theirCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP);
      bool clause3 = (myCallType.GetType () == theirCallType.GetType ())
        && (theirStartTime.GetTime () < myStartTime.GetTime ());
      bool clause4 = (myCallType.GetType () == theirCallType.GetType ())
        && (myStartTime.GetTime () == theirStartTime.GetTime ())
        && (theirCallId.GetCallId () < myCallId.GetCallId ());

      if (clause1
          || clause2
          || clause3
          || clause4)
        {
          //TODO: Not in standard - just an indication that a call merge is taking place.
          Simulator::Schedule (Seconds (0), &McpttCallMachineGrpBasic::NotifyMerge, &machine, theirCallId.GetCallId ());

          machine.SetSdp (theirSdp);
          machine.SetCallId (theirCallId);
          machine.SetOrigId (theirOrigId);
          machine.SetRefInt (theirRefInt);
          machine.SetStartTime (theirStartTime);

          floorMachine->Stop ();

          parent->CloseFloorChannel ();
          parent->CloseMediaChannel ();

          parent->OpenFloorChannel (grpAddr, floorPort);
          parent->OpenMediaChannel (grpAddr, speechPort);

          floorMachine->SetOriginator (false);
          floorMachine->Start ();

          delayTfg6 = machine.CalcDelayForMaxDuration ();
          tfg6->Stop ();
          tfg6->SetDelay (delayTfg6);
          tfg6->Start ();

          delayTfg2 = machine.CalcDelayForPeriodicAnnoun ();
          tfg2->Stop ();
          tfg2->SetDelay (delayTfg2);
          tfg2->Start ();
        }
    }
}

void
McpttCallMachineGrpBasicStateS3::ReceiveGrpCallProbe (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpProbe& msg)
{
  NS_LOG_FUNCTION (this << &machine << msg);

  Time delayTfg2;
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  bool probeRespVal = machine.GetProbeRespVal ();
  McpttCallMsgFieldGrpId theirGrpId = msg.GetGrpId ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();

  if (myGrpId.GetGrpId () == theirGrpId.GetGrpId ()
      && probeRespVal == false)
    {
      tfg2->Stop ();
      delayTfg2 = machine.CalcDelayForAfterCallProbe ();
      tfg2->SetDelay (delayTfg2);
      tfg2->Start ();
      machine.SetProbeRespVal (true);
    }
}

void
McpttCallMachineGrpBasicStateS3::ReleaseCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCall> parent = machine.GetCall ();
  Ptr<McpttFloorParticipant> floorMachine = parent->GetFloorMachine ();

  floorMachine->Stop ();

  parent->CloseFloorChannel ();
  parent->CloseMediaChannel ();

  if (tfg4->IsRunning ())
    {
      tfg4->Stop ();
    }

  if (tfg2->IsRunning ())
    {
      tfg2->Stop ();
    }

  //TODO: Not in standard - stopping timer TFG6 when the call is released (i.e. when leaving the S3 state).
  tfg6->Stop ();

  tfg5->Start ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS6::GetInstance ());
}
/** McpttCallMachineGrpBasicStateS3 - end **/

/** McpttCallMachineGrpBasicStateS4 - begin **/
Ptr<McpttCallMachineGrpBasicStateS4>
McpttCallMachineGrpBasicStateS4::GetInstance (void)
{
  static Ptr<McpttCallMachineGrpBasicStateS4> instance = Create<McpttCallMachineGrpBasicStateS4> ();

  return instance;
}

McpttEntityId
McpttCallMachineGrpBasicStateS4::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (4, "'S4: pending user action without confirm indication'");

  return stateId;
}

McpttCallMachineGrpBasicStateS4::McpttCallMachineGrpBasicStateS4 (void)
  : McpttCallMachineGrpBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachineGrpBasicStateS4::~McpttCallMachineGrpBasicStateS4 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBasicStateS4::AcceptCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Time delayTfg2;
  Time delayTfg6;
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCall> parent = machine.GetCall ();
  McpttCallMsgFieldSdp mySdp = machine.GetSdp ();
  Ptr<McpttFloorParticipant> floorMachine = parent->GetFloorMachine ();

  uint16_t floorPort = mySdp.GetFloorPort ();
  Address grpAddr = mySdp.GetGrpAddr ();
  uint16_t speechPort = mySdp.GetSpeechPort ();

  parent->OpenFloorChannel (grpAddr, floorPort);
  parent->OpenMediaChannel (grpAddr, speechPort);

  floorMachine->SetOriginator (false);
  floorMachine->Start ();

  delayTfg6 = machine.CalcDelayForMaxDuration ();
  tfg6->SetDelay (delayTfg6);
  tfg6->Start ();

  delayTfg2 = machine.CalcDelayForPeriodicAnnoun ();
  tfg2->SetDelay (delayTfg2);
  tfg2->Start ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS3::GetInstance ());
}

void
McpttCallMachineGrpBasicStateS4::ExpiryOfTfg4 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();

  tfg5->Start ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS6::GetInstance ());
}

McpttEntityId
McpttCallMachineGrpBasicStateS4::GetInstanceStateId (void) const
{
  return McpttCallMachineGrpBasicStateS4::GetStateId ();
}

bool
McpttCallMachineGrpBasicStateS4::IsCallOngoing (const McpttCallMachineGrpBasic& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  return true;
}

void
McpttCallMachineGrpBasicStateS4::RejectCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();

  tfg4->Stop ();
  tfg5->Start ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS6::GetInstance ());
}

void
McpttCallMachineGrpBasicStateS4::ReleaseCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttCall> parent = machine.GetCall ();
  Ptr<McpttFloorParticipant> floorMachine = parent->GetFloorMachine ();

  floorMachine->Stop ();

  parent->CloseFloorChannel ();
  parent->CloseMediaChannel ();

  if (tfg4->IsRunning ())
    {
      tfg4->Stop ();
    }

  if (tfg2->IsRunning ())
    {
      tfg2->Stop ();
    }

  tfg5->Start ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS6::GetInstance ());
}
/** McpttCallMachineGrpBasicStateS4 - end **/

/** McpttCallMachineGrpBasicStateS5 - begin **/
Ptr<McpttCallMachineGrpBasicStateS5>
McpttCallMachineGrpBasicStateS5::GetInstance (void)
{
  static Ptr<McpttCallMachineGrpBasicStateS5> instance = Create<McpttCallMachineGrpBasicStateS5> ();

  return instance;
}

McpttEntityId
McpttCallMachineGrpBasicStateS5::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (5, "'S5: pending user action with confirm indication'");

  return stateId;
}

McpttCallMachineGrpBasicStateS5::McpttCallMachineGrpBasicStateS5 (void)
  : McpttCallMachineGrpBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachineGrpBasicStateS5::~McpttCallMachineGrpBasicStateS5 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBasicStateS5::AcceptCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Time delayTfg2;
  Time delayTfg6;
  uint32_t myUserId = machine.GetCall ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCall> parent = machine.GetCall ();
  McpttCallMsgFieldSdp mySdp = machine.GetSdp ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  Ptr<McpttFloorParticipant> floorMachine = parent->GetFloorMachine ();
  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = machine.GetTypeMachine ();
  McpttCallMsgFieldCallType myCallType = typeMachine->GetCallType ();

  uint16_t floorPort = mySdp.GetFloorPort ();
  Address grpAddr = mySdp.GetGrpAddr ();
  uint16_t speechPort = mySdp.GetSpeechPort ();

  parent->OpenFloorChannel (grpAddr, floorPort);
  parent->OpenMediaChannel (grpAddr, speechPort);

  floorMachine->SetOriginator (false);
  floorMachine->Start ();

  McpttCallMsgGrpAccept msg;
  msg.SetCallId (myCallId);
  msg.SetUserId (myUserId);
  msg.SetCallType (myCallType);
  msg.SetGrpId (myGrpId);

  delayTfg6 = machine.CalcDelayForMaxDuration ();
  tfg6->SetDelay (delayTfg6);
  tfg6->Start ();

  delayTfg2 = machine.CalcDelayForPeriodicAnnoun ();
  tfg2->SetDelay (delayTfg2);
  tfg2->Start ();

  machine.Send (msg);

  machine.ChangeState (McpttCallMachineGrpBasicStateS3::GetInstance ());
}

void
McpttCallMachineGrpBasicStateS5::ExpiryOfTfg4 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();

  tfg5->Start ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS6::GetInstance ());
}

McpttEntityId
McpttCallMachineGrpBasicStateS5::GetInstanceStateId (void) const
{
  return McpttCallMachineGrpBasicStateS5::GetStateId ();
}

bool
McpttCallMachineGrpBasicStateS5::IsCallOngoing (const McpttCallMachineGrpBasic& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  return true;
}

void
McpttCallMachineGrpBasicStateS5::RejectCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();

  tfg4->Stop ();
  tfg5->Start ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS6::GetInstance ());
}

void
McpttCallMachineGrpBasicStateS5::ReleaseCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttCall> parent = machine.GetCall ();
  Ptr<McpttFloorParticipant> floorMachine = parent->GetFloorMachine ();

  floorMachine->Stop ();

  if (parent->IsMediaChannelOpen ())
    {
      parent->CloseMediaChannel ();
    }

  if (parent->IsFloorChannelOpen ())
    {
      parent->CloseFloorChannel ();
    }

  if (tfg4->IsRunning ())
    {
      tfg4->Stop ();
    }

  if (tfg2->IsRunning ())
    {
      tfg2->Stop ();
    }

  tfg5->Start ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS6::GetInstance ());
}
/** McpttCallMachineGrpBasicStateS5 - end **/

/** McpttCallMachineGrpBasicStateS6 - begin **/
Ptr<McpttCallMachineGrpBasicStateS6>
McpttCallMachineGrpBasicStateS6::GetInstance (void)
{
  static Ptr<McpttCallMachineGrpBasicStateS6> instance = Create<McpttCallMachineGrpBasicStateS6> ();

  return instance;
}

McpttEntityId
McpttCallMachineGrpBasicStateS6::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (6, "'S6: ignoring incoming call announcements'");

  return stateId;
}

McpttCallMachineGrpBasicStateS6::McpttCallMachineGrpBasicStateS6 (void)
  : McpttCallMachineGrpBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachineGrpBasicStateS6::~McpttCallMachineGrpBasicStateS6 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBasicStateS6::ExpiryOfTfg5 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = machine.GetTypeMachine ();

  machine.SetSdp (McpttCallMsgFieldSdp ());
  machine.SetCallId (McpttCallMsgFieldCallId ());
  machine.SetOrigId (McpttCallMsgFieldUserId ());
  //TODO: In standard this should be released but since the same call machine
  //is reused for a new call for the same group, this will be left in place
  //machine.SetRefInt (McpttCallMsgFieldRefreshInterval ());
  //TODO: In standard this should be released but since the same call machine
  //is reused for a new call for the same group, this will be left in place
  //machine.SetGrpId (McpttCallMsgFieldGrpId ());
  machine.SetStartTime (McpttCallMsgFieldStartTime ());

  typeMachine->ReleaseCall ();
  typeMachine->Stop ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS1::GetInstance ());
}

McpttEntityId
McpttCallMachineGrpBasicStateS6::GetInstanceStateId (void) const
{
  return McpttCallMachineGrpBasicStateS6::GetStateId ();
}

void
McpttCallMachineGrpBasicStateS6::InitiateCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Time delayTfg2;
  Time delayTfg6;
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCall> parent = machine.GetCall ();
  McpttCallMsgFieldSdp mySdp = machine.GetSdp ();
  Ptr<McpttFloorParticipant> floorMachine = parent->GetFloorMachine ();

  uint16_t floorPort = mySdp.GetFloorPort ();
  Address grpAddr = mySdp.GetGrpAddr ();
  uint16_t speechPort = mySdp.GetSpeechPort ();

  tfg5->Stop ();

  parent->OpenFloorChannel (grpAddr, floorPort);
  parent->OpenMediaChannel (grpAddr, speechPort);

  floorMachine->SetOriginator (false);
  floorMachine->Start ();

  delayTfg6 = machine.CalcDelayForMaxDuration ();
  tfg6->SetDelay (delayTfg6);
  tfg6->Start ();

  delayTfg2 = machine.CalcDelayForPeriodicAnnoun ();
  tfg2->SetDelay (delayTfg2);
  tfg2->Start ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS3::GetInstance ());
}

void
McpttCallMachineGrpBasicStateS6::ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun& msg)
{
  NS_LOG_FUNCTION (this << &machine << msg);

  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();

  McpttCallMsgFieldSdp theirSdp = msg.GetSdp ();
  McpttCallMsgFieldGrpId theirGrpId = msg.GetGrpId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldUserId theirOrigId = msg.GetOrigId ();
  McpttCallMsgFieldRefreshInterval theirRefInt = msg.GetRefInt ();
  McpttCallMsgFieldStartTime theirStartTime = msg.GetStartTime ();

  if (myGrpId.GetGrpId () == theirGrpId.GetGrpId ())
    {
      machine.SetSdp (theirSdp);
      machine.SetCallId (theirCallId);
      machine.SetOrigId (theirOrigId);
      machine.SetRefInt (theirRefInt);
      machine.SetStartTime (theirStartTime);

      tfg5->Stop ();
      tfg5->Start ();
    }
}
/** McpttCallMachineGrpBasicStateS6 - end **/

/** McpttCallMachineGrpBasicStateS7 - begin **/
Ptr<McpttCallMachineGrpBasicStateS7>
McpttCallMachineGrpBasicStateS7::GetInstance (void)
{
  static Ptr<McpttCallMachineGrpBasicStateS7> instance = Create<McpttCallMachineGrpBasicStateS7> ();

  return instance;
}

McpttEntityId
McpttCallMachineGrpBasicStateS7::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (7, "'S7: waiting for call announcement after call release'");

  return stateId;
}

McpttCallMachineGrpBasicStateS7::McpttCallMachineGrpBasicStateS7 (void)
  : McpttCallMachineGrpBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachineGrpBasicStateS7::~McpttCallMachineGrpBasicStateS7 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBasicStateS7::ExpiryOfTfg1 (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCallTypeMachineGrpBasic> typeMachine = machine.GetTypeMachine ();

  machine.SetGrpId (McpttCallMsgFieldGrpId ());

  typeMachine->ReleaseCall ();
  typeMachine->Stop ();

  machine.ChangeState (McpttCallMachineGrpBasicStateS1::GetInstance ());
}

McpttEntityId
McpttCallMachineGrpBasicStateS7::GetInstanceStateId (void) const
{
  return McpttCallMachineGrpBasicStateS7::GetStateId ();
}

void
McpttCallMachineGrpBasicStateS7::InitiateCall (McpttCallMachineGrpBasic& machine)
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();

  tfg1->Stop ();

  McpttCallMsgGrpProbe msg;
  msg.SetGrpId (myGrpId);

  tfg3->Start ();

  tfg1->Start ();

  machine.Send (msg);

  machine.ChangeState (McpttCallMachineGrpBasicStateS2::GetInstance ());
}

void
McpttCallMachineGrpBasicStateS7::ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun& msg)
{
  NS_LOG_FUNCTION (this << &machine << msg);

  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  McpttCallMsgFieldSdp theirSdp = msg.GetSdp ();
  McpttCallMsgFieldGrpId theirGrpId = msg.GetGrpId ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldUserId theirOrigId = msg.GetOrigId ();
  McpttCallMsgFieldStartTime theirStartTime = msg.GetStartTime ();
  McpttCallMsgFieldRefreshInterval theirRefInt = msg.GetRefInt ();

  if (myGrpId.GetGrpId () == theirGrpId.GetGrpId ())
    {
      machine.SetSdp (theirSdp);
      machine.SetCallId (theirCallId);
      machine.SetOrigId (theirOrigId);
      machine.SetRefInt (theirRefInt);
      machine.SetStartTime (theirStartTime);

      tfg1->Stop ();

      tfg5->Start ();

      machine.ChangeState (McpttCallMachineGrpBasicStateS6::GetInstance ());
    }
}
/** McpttCallMachineGrpBasicStateS7 - end **/

} // namespace psc
} // namespace ns3
