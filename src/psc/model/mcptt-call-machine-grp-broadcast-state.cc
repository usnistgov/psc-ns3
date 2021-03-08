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
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-call-machine-grp-broadcast.h"
#include "mcptt-call-machine-grp-broadcast-state.h"
#include "mcptt-ptt-app.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallMachineGrpBroadcastState");

namespace psc {

/** McpttCallMachineGrpBroadcastState - begin **/
McpttCallMachineGrpBroadcastState::~McpttCallMachineGrpBroadcastState (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBroadcastState::AcceptCall (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

void
McpttCallMachineGrpBroadcastState::ExpiryOfTfb1 (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

void
McpttCallMachineGrpBroadcastState::ExpiryOfTfb2 (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

void
McpttCallMachineGrpBroadcastState::ExpiryOfTfb3 (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

void
McpttCallMachineGrpBroadcastState::InitiateCall (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

bool
McpttCallMachineGrpBroadcastState::IsCallOngoing (const McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  return false;
}

void
McpttCallMachineGrpBroadcastState::ReceiveGrpCallBroadcast (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcast& msg) const
{
  NS_LOG_FUNCTION (this << &machine << &msg);
}

void
McpttCallMachineGrpBroadcastState::ReceiveGrpCallBroadcastEnd (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcastEnd& msg) const
{
  NS_LOG_FUNCTION (this << &machine << &msg);
}

void
McpttCallMachineGrpBroadcastState::RejectCall (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

void
McpttCallMachineGrpBroadcastState::ReleaseCall (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
}

McpttCallMachineGrpBroadcastState::McpttCallMachineGrpBroadcastState (void)
  : SimpleRefCount<McpttCallMachineGrpBroadcastState> ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttCallMachineGrpBroadcastState - end **/

std::ostream&
operator<< (std::ostream& os, const McpttCallMachineGrpBroadcastState& inst)
{
  McpttEntityId stateId = inst.GetInstanceStateId ();

  os << stateId;

  return os;
}

/** McpttCallMachineGrpBroadcastStateB1 - begin **/
Ptr<McpttCallMachineGrpBroadcastStateB1>
McpttCallMachineGrpBroadcastStateB1::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallMachineGrpBroadcastStateB1> inst = Create<McpttCallMachineGrpBroadcastStateB1> ();

  return inst;
}

McpttEntityId
McpttCallMachineGrpBroadcastStateB1::GetStateId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static McpttEntityId sid (1, "'B1: start-stop'");

  return sid;
}

McpttCallMachineGrpBroadcastStateB1::~McpttCallMachineGrpBroadcastStateB1 (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttCallMachineGrpBroadcastStateB1::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachineGrpBroadcastStateB1::GetStateId ();
}

void
McpttCallMachineGrpBroadcastStateB1::InitiateCall (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttPttApp> pttApp = call->GetOwner ();
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  McpttCallMsgFieldGrpId grpId = machine.GetGrpId ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  uint32_t myUserId = machine.GetCall ()->GetOwner ()->GetUserId ();
  uint16_t callId = machine.GenerateRandomCallId ();
  NS_LOG_DEBUG ("Generate call ID of " << callId);
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = pttApp->GetLocalAddress ();
  AddressValue grpAddress;
  call->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  machine.SetSdp (sdp);
  machine.SetCallId (callId);
  machine.SetOrigId (myUserId);
  machine.SetCallType (McpttCallMsgFieldCallType::BROADCAST_GROUP);

  McpttCallMsgGrpBroadcast msg;
  msg.SetCallId (callId);
  msg.SetCallType (McpttCallMsgFieldCallType::BROADCAST_GROUP);
  msg.SetOrigId (myUserId);
  msg.SetGrpId (grpId);
  msg.SetSdp (sdp);

  machine.SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BROADCAST_GROUP));

  machine.Send (msg);

  call->OpenFloorChannel (grpAddress.Get (), floorPort);
  call->OpenMediaChannel (grpAddress.Get (), speechPort);

  floorMachine->SetOriginator (true);
  floorMachine->Start ();

  tfb1->Start ();
  tfb2->Start ();

  machine.ReportEvent (McpttCallMachine::CALL_INITIATED);

  machine.ChangeState (McpttCallMachineGrpBroadcastStateB2::GetInstance ());
}

void
McpttCallMachineGrpBroadcastStateB1::ReceiveGrpCallBroadcast (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcast& msg) const
{
  NS_LOG_FUNCTION (this << &machine << &msg);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();
  McpttCallMsgFieldSdp theirSdp = msg.GetSdp ();
  McpttCallMsgFieldGrpId theirGrpId = msg.GetGrpId ();
  McpttCallMsgFieldUserId theirOrigId = msg.GetOrigId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldCallType theirCallType = msg.GetCallType ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  //TODO: Not in standard - should check that received call ID matches no other
  if (myGrpId.GetGrpId () == theirGrpId.GetGrpId ())
    {
      machine.SetCallId (theirCallId);
      machine.SetCallType (theirCallType);
      machine.SetSdp (theirSdp);
      machine.SetOrigId (theirOrigId);
      machine.SetGrpId (theirGrpId);

      if (machine.IsUserAckReq ())
        {
          tfb3->Start ();

          machine.ChangeState (McpttCallMachineGrpBroadcastStateB3::GetInstance ());

          machine.TakeNewCallNotification (theirCallId.GetCallId ());
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

          tfb1->Start ();

          machine.ChangeState (McpttCallMachineGrpBroadcastStateB2::GetInstance ());
        }
    }
}

McpttCallMachineGrpBroadcastStateB1::McpttCallMachineGrpBroadcastStateB1 (void)
  : McpttCallMachineGrpBroadcastState ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttCallMachineGrpBroadcastStateB1 - end **/

/** McpttCallMachineGrpBroadcastStateB2 - begin **/
Ptr<McpttCallMachineGrpBroadcastStateB2>
McpttCallMachineGrpBroadcastStateB2::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallMachineGrpBroadcastStateB2> inst = Create<McpttCallMachineGrpBroadcastStateB2> ();

  return inst;
}

McpttEntityId
McpttCallMachineGrpBroadcastStateB2::GetStateId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static McpttEntityId sid (2, "'B2: in-progress broadcast group call'");

  return sid;
}

McpttCallMachineGrpBroadcastStateB2::~McpttCallMachineGrpBroadcastStateB2 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBroadcastStateB2::ExpiryOfTfb1 (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  if (floorMachine->IsStarted ())
    {
      floorMachine->Stop ();
    }
  if (call->IsFloorChannelOpen ())
    {
      call->CloseFloorChannel ();
    }
  if (call->IsMediaChannelOpen ())
    {
      call->CloseMediaChannel ();
    }

  if (tfb2->IsRunning ())
    {
      tfb2->Stop ();
    }

  machine.SetCallId (McpttCallMsgFieldCallId (0));

  machine.ChangeState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());
}

void
McpttCallMachineGrpBroadcastStateB2::ExpiryOfTfb2 (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  McpttCallMsgFieldSdp mySdp = machine.GetSdp ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldUserId myOrigId = machine.GetOrigId ();
  McpttCallMsgFieldCallType myCallType = machine.GetCallType ();

  McpttCallMsgGrpBroadcast msg;
  msg.SetCallId (myCallId);
  msg.SetCallType (myCallType);
  msg.SetOrigId (myOrigId);
  msg.SetGrpId (myGrpId);
  msg.SetSdp (mySdp);

  machine.Send (msg);

  tfb2->Start ();
}

McpttEntityId
McpttCallMachineGrpBroadcastStateB2::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachineGrpBroadcastStateB2::GetStateId ();
}

bool
McpttCallMachineGrpBroadcastStateB2::IsCallOngoing (const McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  return true;
}

void
McpttCallMachineGrpBroadcastStateB2::ReceiveGrpCallBroadcastEnd (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcastEnd& msg) const
{
  NS_LOG_FUNCTION (this << &machine << &msg);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      if (floorMachine->IsStarted ())
        {
          floorMachine->Stop ();
        }
      if (call->IsFloorChannelOpen ())
        {
          call->CloseFloorChannel ();
        }
      if (call->IsMediaChannelOpen ())
        {
          call->CloseMediaChannel ();
        }

      tfb1->Stop ();

      machine.ChangeState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());
    }
}

void
McpttCallMachineGrpBroadcastStateB2::ReleaseCall (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttPttApp> pttApp = call->GetOwner ();
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  uint32_t myUserId = pttApp->GetUserId ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  McpttCallMsgFieldGrpId myGrpId = machine.GetGrpId ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldUserId myOrigId = machine.GetOrigId ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  if (myOrigId.GetId () == myUserId)
    {
      floorMachine->Stop ();

      call->CloseFloorChannel ();
      call->CloseMediaChannel ();

      McpttCallMsgGrpBroadcastEnd msg;
      msg.SetCallId (myCallId);
      msg.SetOrigId (myOrigId);
      msg.SetGrpId (myGrpId);

      machine.Send (msg);

      tfb1->Stop ();

      tfb2->Stop ();

      machine.ChangeState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());
    }
  else
    {
      floorMachine->Stop ();

      call->CloseFloorChannel ();
      call->CloseMediaChannel ();

      machine.ChangeState (McpttCallMachineGrpBroadcastStateB4::GetInstance ());
    }
}

McpttCallMachineGrpBroadcastStateB2::McpttCallMachineGrpBroadcastStateB2 (void)
  : McpttCallMachineGrpBroadcastState ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttCallMachineGrpBroadcastStateB2 - end **/

/** McpttCallMachineGrpBroadcastStateB3 - begin **/
Ptr<McpttCallMachineGrpBroadcastStateB3>
McpttCallMachineGrpBroadcastStateB3::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallMachineGrpBroadcastStateB3> inst = Create<McpttCallMachineGrpBroadcastStateB3> ();

  return inst;
}

McpttEntityId
McpttCallMachineGrpBroadcastStateB3::GetStateId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static McpttEntityId sid (3, "'B3: pending user action'");

  return sid;
}

McpttCallMachineGrpBroadcastStateB3::~McpttCallMachineGrpBroadcastStateB3 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBroadcastStateB3::AcceptCall (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  McpttCallMsgFieldSdp mySdp = machine.GetSdp ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  uint16_t floorPort = mySdp.GetFloorPort ();
  Address grpAddr = mySdp.GetGrpAddr ();
  uint16_t speechPort = mySdp.GetSpeechPort ();

  call->OpenFloorChannel (grpAddr, floorPort);
  call->OpenMediaChannel (grpAddr, speechPort);

  floorMachine->SetOriginator (false);
  floorMachine->Start ();

  tfb3->Stop ();

  tfb1->Start ();

  machine.ChangeState (McpttCallMachineGrpBroadcastStateB2::GetInstance ());
}

void
McpttCallMachineGrpBroadcastStateB3::ExpiryOfTfb3 (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  machine.ChangeState (McpttCallMachineGrpBroadcastStateB4::GetInstance ());
}


McpttEntityId
McpttCallMachineGrpBroadcastStateB3::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachineGrpBroadcastStateB3::GetStateId ();
}

void
McpttCallMachineGrpBroadcastStateB3::RejectCall (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  tfb3->Stop ();
  tfb1->Start ();

  machine.ChangeState (McpttCallMachineGrpBroadcastStateB4::GetInstance ());
}

McpttCallMachineGrpBroadcastStateB3::McpttCallMachineGrpBroadcastStateB3 (void)
  : McpttCallMachineGrpBroadcastState ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttCallMachineGrpBroadcastStateB3 - end **/

/** McpttCallMachineGrpBroadcastStateB4 - begin **/
Ptr<McpttCallMachineGrpBroadcastStateB4>
McpttCallMachineGrpBroadcastStateB4::GetInstance (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static Ptr<McpttCallMachineGrpBroadcastStateB4> inst = Create<McpttCallMachineGrpBroadcastStateB4> ();

  return inst;
}

McpttEntityId
McpttCallMachineGrpBroadcastStateB4::GetStateId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static McpttEntityId sid (4, "'B4: ignoring same call ID'");

  return sid;
}

McpttCallMachineGrpBroadcastStateB4::~McpttCallMachineGrpBroadcastStateB4 (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineGrpBroadcastStateB4::ExpiryOfTfb1 (McpttCallMachineGrpBroadcast& machine) const
{
  NS_LOG_FUNCTION (this << &machine);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  if (floorMachine->IsStarted ())
    {
      floorMachine->Stop ();
    }
  if (call->IsFloorChannelOpen ())
    {
      call->CloseFloorChannel ();
    }
  if (call->IsMediaChannelOpen ())
    {
      call->CloseMediaChannel ();
    }

  machine.SetCallId (McpttCallMsgFieldCallId (0));

  machine.ChangeState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());
}

McpttEntityId
McpttCallMachineGrpBroadcastStateB4::GetInstanceStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachineGrpBroadcastStateB4::GetStateId ();
}

void
McpttCallMachineGrpBroadcastStateB4::ReceiveGrpCallBroadcast (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcast& msg) const
{
  NS_LOG_FUNCTION (this << &machine << &msg);

  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldCallId  theirCallId = msg.GetCallId ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      tfb1->Restart ();
    }
}

void
McpttCallMachineGrpBroadcastStateB4::ReceiveGrpCallBroadcastEnd (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcastEnd& msg) const
{
  NS_LOG_FUNCTION (this << &machine << &msg);

  Ptr<McpttCall> call = machine.GetCall ();
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  McpttCallMsgFieldCallId myCallId = machine.GetCallId ();
  McpttCallMsgFieldCallId  theirCallId = msg.GetCallId ();
  Ptr<McpttFloorParticipant> floorMachine = call->GetFloorMachine ();

  if (myCallId.GetCallId () == theirCallId.GetCallId ())
    {
      if (floorMachine->IsStarted ())
        {
          floorMachine->Stop ();
        }
      if (call->IsFloorChannelOpen ())
        {
          call->CloseFloorChannel ();
        }
      if (call->IsMediaChannelOpen ())
        {
          call->CloseMediaChannel ();
        }

      if (tfb1->IsRunning ())
        {
          tfb1->Stop ();
        }

      machine.ChangeState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());
    }
}

McpttCallMachineGrpBroadcastStateB4::McpttCallMachineGrpBroadcastStateB4 (void)
  : McpttCallMachineGrpBroadcastState ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttCallMachineGrpBroadcastStateB4 - end **/

} // namespace psc
} // namespace ns3

