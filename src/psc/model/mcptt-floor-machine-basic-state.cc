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
#include "mcptt-floor-machine-basic.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-queue.h"
#include "mcptt-media-msg.h"
#include "mcptt-media-src.h"
#include "mcptt-ptt-app.h"

#include "mcptt-floor-machine-basic-state.h"

namespace ns3 {

/** McpttFloorMachineBasicState - begin **/
NS_LOG_COMPONENT_DEFINE ("McpttFloorMachineBasicState");

McpttFloorMachineBasicState::~McpttFloorMachineBasicState (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineBasicState::AcceptGrant (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring accept grant."); 
}

void
McpttFloorMachineBasicState::ExpiryOfT201 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T201."); 
}

void
McpttFloorMachineBasicState::ExpiryOfT203 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T203."); 
}

void
McpttFloorMachineBasicState::ExpiryOfT204 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T204."); 
}

void
McpttFloorMachineBasicState::ExpiryOfT205 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T205."); 
}

void
McpttFloorMachineBasicState::ExpiryOfT206 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T206."); 
}

void
McpttFloorMachineBasicState::ExpiryOfT207 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T207."); 
}

void
McpttFloorMachineBasicState::ExpiryOfT230 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T230."); 
}

void
McpttFloorMachineBasicState::ExpiryOfT233 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring expiry of T233."); 
}

McpttEntityId
McpttFloorMachineBasicState::GetInstanceStateId (void) const
{
  return McpttEntityId ();
}

bool
McpttFloorMachineBasicState::HasFloor (const McpttFloorMachineBasic& floorMachine) const
{
  return false;
}

void
McpttFloorMachineBasicState::ReceiveCallRelease (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this);
  McpttCall* callOwner = floorMachine.GetOwner ();
  McpttPttApp* appOwner = callOwner->GetOwner ();

  if (appOwner->IsPushed ())
    {
      appOwner->Released ();
    }

  floorMachine.StopTimers ();
  floorMachine.ResetCounters ();

  floorMachine.SetState (McpttFloorMachineBasicStateStartStop::GetInstance ());
}
void
McpttFloorMachineBasicState::ReceiveFloorDeny (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << "."); 
}

void
McpttFloorMachineBasicState::ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << "."); 
}

void
McpttFloorMachineBasicState::ReceiveFloorQueuePositionReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgQueuePosReq& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << "."); 
}

void
McpttFloorMachineBasicState::ReceiveFloorQueuePositionInfo (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgQueueInfo& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << "."); 
}

void
McpttFloorMachineBasicState::ReceiveFloorRelease (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRelease& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << "."); 
}

void
McpttFloorMachineBasicState::ReceiveFloorReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << "."); 
}

void
McpttFloorMachineBasicState::ReceiveFloorTaken (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << "."); 
}

void
McpttFloorMachineBasicState::ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring " << msg.GetInstanceTypeId () << "."); 
}

void
McpttFloorMachineBasicState::SendFloorQueuePosReq (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring floor queue position request."); 
}

void
McpttFloorMachineBasicState::MediaReady (McpttFloorMachineBasic& floorMachine, McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_ASSERT_MSG (false, "Should not be sending media here.");
}

void
McpttFloorMachineBasicState::ReleaseRequest (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring release request."); 
}

void
McpttFloorMachineBasicState::Selected (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);
}

void
McpttFloorMachineBasicState::Start (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);
}

void
McpttFloorMachineBasicState::Stop (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  ReceiveCallRelease (floorMachine);
}

void
McpttFloorMachineBasicState::TakePushNotification (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring push notification."); 
}

void
McpttFloorMachineBasicState::TakeReleaseNotification (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  NS_LOG_LOGIC (GetInstanceStateId ().GetName () << "(" << this << ")" << " ignoring release notification."); 
}

void
McpttFloorMachineBasicState::Unselected (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);
}

McpttFloorMachineBasicState::McpttFloorMachineBasicState (void)
  : SimpleRefCount<McpttFloorMachineBasicState> ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttFloorMachineBasicState - end **/

std::ostream&
operator<< (std::ostream& os, const ns3::McpttFloorMachineBasicState& state)
{
  return os << state.GetInstanceStateId ();
}

/** McpttFloorMachineBasicStateStartStop - begin **/
Ptr<McpttFloorMachineBasicStateStartStop>
McpttFloorMachineBasicStateStartStop::GetInstance (void)
{
  static Ptr<McpttFloorMachineBasicStateStartStop> instance = Create<McpttFloorMachineBasicStateStartStop> ();
  return instance;
}

McpttEntityId
McpttFloorMachineBasicStateStartStop::GetStateId (void)
{
  static McpttEntityId id (0, "'Start-stop'");

  return id;
}

McpttFloorMachineBasicStateStartStop::McpttFloorMachineBasicStateStartStop (void)
  : McpttFloorMachineBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMachineBasicStateStartStop::~McpttFloorMachineBasicStateStartStop (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttFloorMachineBasicStateStartStop::GetInstanceStateId (void) const
{
  return McpttFloorMachineBasicStateStartStop::GetStateId ();
}

void
McpttFloorMachineBasicStateStartStop::ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);
  
  //Provide floor taken notification.
  uint32_t grantedSsrc = msg.GetGrantedSsrc ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();

  floorMachine.SetCurrentSsrc (grantedSsrc);
  t203->Start ();

  floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
}

void
McpttFloorMachineBasicStateStartStop::ReceiveFloorTaken (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  //Provide floor taken notification to user.
  uint32_t grantedSsrc = msg.GetSsrc ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
 
  floorMachine.SetCurrentSsrc (grantedSsrc);
  t203->Start ();

  floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
}

void
McpttFloorMachineBasicStateStartStop::ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  //Provide floor taken notification to user.
  //Request MCPTT client to start rendering received RTP packets.
  uint32_t rxSsrc = msg.GetHead ().GetSsrc ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();

  floorMachine.SetCurrentSsrc (rxSsrc);
  t203->Restart ();

  floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
}

void
McpttFloorMachineBasicStateStartStop::Start (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  McpttCallMsgFieldCallType callType = floorMachine.GetOwner ()->GetCallMachine ()->GetCallType ();
  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t userId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();

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

      floorMachine.ChangeState (McpttFloorMachineBasicStateHasPerm::GetInstance ());
    }
  else if (callType.GetType () == McpttCallMsgFieldCallType::BASIC_GROUP)
    {
      t230->Start ();

      floorMachine.ChangeState (McpttFloorMachineBasicStateSilence::GetInstance ());
    }
  else if (callType.GetType () == McpttCallMsgFieldCallType::PRIVATE)
    {
      t203->Start ();

      floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
    }
  else if (indicator.IsIndicated (McpttFloorMsgFieldIndic::BROADCAST_CALL))
    {
      t203->Start ();

      floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
    }
   else //Assume this is a private call since there is no indicator for a private call.
    {
      NS_FATAL_ERROR ("Call type could not be determined.");
    }
}

void
McpttFloorMachineBasicStateStartStop::TakePushNotification (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t userId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();

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

  floorMachine.ChangeState (McpttFloorMachineBasicStatePendReq::GetInstance ());
}
/** McpttFloorMachineBasicStateStartStop - end **/

/** McpttFloorMachineBasicStateNoPerm - begin **/
Ptr<McpttFloorMachineBasicStateNoPerm>
McpttFloorMachineBasicStateNoPerm::GetInstance (void)
{
  static Ptr<McpttFloorMachineBasicStateNoPerm> instance = Create<McpttFloorMachineBasicStateNoPerm> ();

  return instance;
}

McpttEntityId
McpttFloorMachineBasicStateNoPerm::GetStateId (void)
{
  static McpttEntityId id (1, "'O: has no permission'");

  return id;
}

McpttFloorMachineBasicStateNoPerm::McpttFloorMachineBasicStateNoPerm (void)
  : McpttFloorMachineBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMachineBasicStateNoPerm::~McpttFloorMachineBasicStateNoPerm (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineBasicStateNoPerm::ExpiryOfT203 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  //Provide floor idle notification to user.
  //Request MCPTT client to stop rendering received RTP media packets.
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();

  t230->Start ();

  floorMachine.ClearCurrentSsrc ();

  floorMachine.ChangeState (McpttFloorMachineBasicStateSilence::GetInstance ());
}

McpttEntityId
McpttFloorMachineBasicStateNoPerm::GetInstanceStateId (void) const
{
  return McpttFloorMachineBasicStateNoPerm::GetStateId ();
}

void
McpttFloorMachineBasicStateNoPerm::ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  uint32_t grantedSsrc = msg.GetGrantedSsrc ();
  McpttFloorMsgFieldIndic rxIndicator = msg.GetIndicator ();

  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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
McpttFloorMachineBasicStateNoPerm::ReceiveFloorRelease (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRelease& msg) const
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

      floorMachine.ChangeState (McpttFloorMachineBasicStateSilence::GetInstance ());
    }
}

void
McpttFloorMachineBasicStateNoPerm::ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetHead ().GetSsrc ();
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
McpttFloorMachineBasicStateNoPerm::Selected (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  McpttPttApp* appOwner = floorMachine.GetOwner ()->GetOwner ();

  if (appOwner->IsPushed ())
    {
      appOwner->Released ();
    }
}

void
McpttFloorMachineBasicStateNoPerm::TakePushNotification (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t userId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

  floorMachine.ChangeState (McpttFloorMachineBasicStatePendReq::GetInstance ());
}
/** McpttFloorMachineBasicStateNoPerm - end **/

/** McpttFloorMachineBasicStateHasPerm - begin **/
Ptr<McpttFloorMachineBasicStateHasPerm>
McpttFloorMachineBasicStateHasPerm::GetInstance (void)
{
  static Ptr<McpttFloorMachineBasicStateHasPerm> instance = Create<McpttFloorMachineBasicStateHasPerm> ();

  return instance;
}

McpttEntityId
McpttFloorMachineBasicStateHasPerm::GetStateId (void)
{
  static McpttEntityId id (2, "'O: has permission'");

  return id;
}

McpttFloorMachineBasicStateHasPerm::McpttFloorMachineBasicStateHasPerm (void)
  : McpttFloorMachineBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMachineBasicStateHasPerm::~McpttFloorMachineBasicStateHasPerm (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineBasicStateHasPerm::ExpiryOfT206 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  //Notify the MCPTT user that the transmission time limit is about to be reached.

  Ptr<McpttTimer> t207 = floorMachine.GetT207 ();

  t207->Start ();
}

void
McpttFloorMachineBasicStateHasPerm::ExpiryOfT207 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

      floorMachine.ChangeState (McpttFloorMachineBasicStatePendGrant::GetInstance ());
    }
  else
    {
      McpttFloorMsgRelease releaseMsg (txSsrc);
      releaseMsg.SetUserId (myUserId);
      releaseMsg.SetIndicator (indicator);

      t230->Start ();

      floorMachine.ClearCurrentSsrc ();

      floorMachine.Send (releaseMsg);

      floorMachine.ChangeState (McpttFloorMachineBasicStateSilence::GetInstance ());
    }
}

McpttEntityId
McpttFloorMachineBasicStateHasPerm::GetInstanceStateId (void) const
{
  return McpttFloorMachineBasicStateHasPerm::GetStateId ();
}

bool
McpttFloorMachineBasicStateHasPerm::HasFloor (const McpttFloorMachineBasic& floorMachine) const
{
  return true;
}

void
McpttFloorMachineBasicStateHasPerm::ReceiveFloorQueuePositionReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgQueuePosReq& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();

  uint16_t position;
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
  uint32_t msgUserId = msg.GetUserId ().GetUserId ();
  McpttQueuedUserInfo userInfo;

  queue->View (msgUserId, userInfo, position);

  uint32_t queuedUserSsrc = userInfo.GetSsrc ();
  McpttFloorMsgFieldQueueInfo infoField = userInfo.GetInfo ();
  McpttFloorMsgFieldQueuedUserId queuedUserIdField = userInfo.GetUserId ();

  McpttFloorMsgQueueInfo infoMsg (txSsrc);
  infoMsg.SetQueuedUserId (queuedUserIdField);
  infoMsg.SetQueueInfo (infoField);
  infoMsg.SetQueuedSsrc (queuedUserSsrc);
  infoMsg.SetUserId (myUserId);

  floorMachine.Send (infoMsg);
}

void
McpttFloorMachineBasicStateHasPerm::ReceiveFloorRelease (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRelease& msg) const
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
McpttFloorMachineBasicStateHasPerm::ReceiveFloorReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRequest& msg) const
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

      floorMachine.ChangeState (McpttFloorMachineBasicStatePendGrant::GetInstance ());
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
      McpttFloorMsgFieldQueueInfo infoField = userInfo.GetInfo ();
      McpttFloorMsgFieldQueuedUserId queuedUserIdField = userInfo.GetUserId ();

      McpttFloorMsgQueueInfo infoMsg (txSsrc);
      infoMsg.SetQueuedUserId (queuedUserIdField);
      infoMsg.SetQueueInfo (infoField);
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
      McpttFloorMsgFieldQueueInfo queueInfo (position, sentPriority);
      McpttFloorMsgFieldQueuedUserId queuedUserIdField (queuedUserId);
    
      McpttQueuedUserInfo userInfo (rxSsrc, queuedUserId, queueInfo);
      queue->Enqueue (userInfo);

      McpttFloorMsgQueueInfo queueInfoMsg (txSsrc);
      queueInfoMsg.SetUserId (userIdField);
      queueInfoMsg.SetQueuedSsrc (rxSsrc);
      queueInfoMsg.SetQueuedUserId (queuedUserIdField);
      queueInfoMsg.SetQueueInfo (queueInfo);

      floorMachine.Send (queueInfoMsg);
    }
}

void
McpttFloorMachineBasicStateHasPerm::MediaReady (McpttFloorMachineBasic& floorMachine, McpttMediaMsg& msg) const
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

    McpttRtpHeader head = msg.GetHead ();
    head.SetSsrc (txSsrc);
    msg.SetHead (head);
}

void
McpttFloorMachineBasicStateHasPerm::Selected (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  McpttCall* callOwner = floorMachine.GetOwner ();
  McpttPttApp* appOwner = callOwner->GetOwner (); 
  Ptr<McpttMediaSrc> mediaSrc = appOwner->GetMediaSrc ();

  if (!appOwner->IsPushed ())
    {
      appOwner->Pushed ();
    }

  if (floorMachine.ShouldGenMedia ()
      && !mediaSrc->IsMakingReq ())
    {
      mediaSrc->StartMakingReq ();
    }
}

void
McpttFloorMachineBasicStateHasPerm::TakeReleaseNotification (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttTimer> t205 = floorMachine.GetT205 ();
  Ptr<McpttTimer> t206 = floorMachine.GetT206 ();
  Ptr<McpttTimer> t207 = floorMachine.GetT207 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttCounter> c205 = floorMachine.GetC205 ();
  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

      floorMachine.ChangeState (McpttFloorMachineBasicStatePendGrant::GetInstance ());
    }
  else
    {
      McpttFloorMsgRelease releaseMsg (txSsrc);
      releaseMsg.SetUserId (McpttFloorMsgFieldUserId (myUserId));
      releaseMsg.SetIndicator (indicator);

      t230->Start ();

      floorMachine.ClearCurrentSsrc ();

      floorMachine.Send (releaseMsg);

      floorMachine.ChangeState (McpttFloorMachineBasicStateSilence::GetInstance ());
    }
}

void
McpttFloorMachineBasicStateHasPerm::Unselected (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  McpttPttApp* appOwner = floorMachine.GetOwner ()->GetOwner (); 
  Ptr<McpttMediaSrc> mediaSrc = appOwner->GetMediaSrc ();

  if (appOwner->IsPushed ())
    {
      appOwner->Released ();
    }

  if (mediaSrc->IsMakingReq ())
    {
      mediaSrc->StopMakingReq ();
    }
}
/** McpttFloorMachineBasicStateHasPerm - end **/

/** McpttFloorMachineBasicStatePendGrant - begin **/
Ptr<McpttFloorMachineBasicStatePendGrant>
McpttFloorMachineBasicStatePendGrant::GetInstance (void)
{
  static Ptr<McpttFloorMachineBasicStatePendGrant> instance = Create<McpttFloorMachineBasicStatePendGrant> ();

  return instance;
}

McpttEntityId
McpttFloorMachineBasicStatePendGrant::GetStateId (void)
{
  static McpttEntityId id (3, "'O: pending granted'");

  return id;
}

McpttFloorMachineBasicStatePendGrant::McpttFloorMachineBasicStatePendGrant (void)
  : McpttFloorMachineBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMachineBasicStatePendGrant::~McpttFloorMachineBasicStatePendGrant (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineBasicStatePendGrant::ExpiryOfT205 (McpttFloorMachineBasic& floorMachine) const
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

          floorMachine.ChangeState (McpttFloorMachineBasicStateSilence::GetInstance ());
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
McpttFloorMachineBasicStatePendGrant::ExpiryOfT233 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

      floorMachine.ChangeState (McpttFloorMachineBasicStateSilence::GetInstance ());
    }
}

McpttEntityId
McpttFloorMachineBasicStatePendGrant::GetInstanceStateId (void) const
{
  return McpttFloorMachineBasicStatePendGrant::GetStateId ();
}

bool
McpttFloorMachineBasicStatePendGrant::HasFloor (const McpttFloorMachineBasic& floorMachine) const
{
  return true;
}

void
McpttFloorMachineBasicStatePendGrant::ReceiveFloorRelease (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRelease& msg) const
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
McpttFloorMachineBasicStatePendGrant::ReceiveFloorReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRequest& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  McpttFloorMsgFieldUserId userIdField = msg.GetUserId ();

  uint32_t txSsrc = floorMachine.GetTxSsrc ();

  if (floorMachine.GetCurrentSsrc() != msg.GetSsrc())
    {
      McpttFloorMsgDeny denyMsg (txSsrc);
      denyMsg.SetRejCause (McpttFloorMsgFieldRejectCause::CAUSE_1);
      denyMsg.SetUserId (userIdField);

      floorMachine.Send (denyMsg);
    }
}

void
McpttFloorMachineBasicStatePendGrant::ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const
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

  floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
}

void
McpttFloorMachineBasicStatePendGrant::Unselected (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttFloorQueue> queue = floorMachine.GetQueue ();

  //TODO: Not in standard - clear the local queue of floor requests when leaving the 'O: pending granted' state.
  if (queue->IsEnabled ())
    {
      queue->Clear ();
    }
}
/** McpttFloorMachineBasicStatePendGrant - end **/

/** McpttFloorMachineBasicStatePendReq - begin **/
Ptr<McpttFloorMachineBasicStatePendReq>
McpttFloorMachineBasicStatePendReq::GetInstance (void)
{
  static Ptr<McpttFloorMachineBasicStatePendReq> instance = Create<McpttFloorMachineBasicStatePendReq> ();

  return instance;
}

McpttEntityId
McpttFloorMachineBasicStatePendReq::GetStateId (void)
{
  static McpttEntityId id (4, "'O: pending request'");

  return id;
}

McpttFloorMachineBasicStatePendReq::McpttFloorMachineBasicStatePendReq (void)
  : McpttFloorMachineBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMachineBasicStatePendReq::~McpttFloorMachineBasicStatePendReq (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineBasicStatePendReq::ExpiryOfT201 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

      floorMachine.ChangeState (McpttFloorMachineBasicStateHasPerm::GetInstance ());
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
McpttFloorMachineBasicStatePendReq::GetInstanceStateId (void) const
{
  return McpttFloorMachineBasicStatePendReq::GetStateId ();
}

void
McpttFloorMachineBasicStatePendReq::ReceiveFloorDeny (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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
              floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
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
              floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
            }
         }
      else
        {
          t201->Stop ();
          t203->Restart ();

          floorMachine.SetCurrentSsrc (rxSsrc);

          floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
        }
    }
}

void
McpttFloorMachineBasicStatePendReq::ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  uint32_t grantedSsrc = msg.GetGrantedSsrc ();
  McpttFloorMsgFieldUserId userIdField = msg.GetUserId ();

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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
              
              floorMachine.ChangeState (McpttFloorMachineBasicStateHasPerm::GetInstance ());
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
              
              floorMachine.ChangeState (McpttFloorMachineBasicStateHasPerm::GetInstance ());
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
 
          floorMachine.ChangeState (McpttFloorMachineBasicStateHasPerm::GetInstance ());
        }
      //TODO: Not in standard - update local queue of floor requests when a 'Floor Granted' message is received.
      if (queue->IsEnabled ())
        {
          std::list<McpttQueuedUserInfo> users = msg.GetUsers();
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
McpttFloorMachineBasicStatePendReq::ReceiveFloorQueuePositionInfo (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgQueueInfo& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  McpttFloorMsgFieldQueuedUserId userIdField = msg.GetQueuedUserId ();

  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

              floorMachine.ChangeState (McpttFloorMachineBasicStateQueued::GetInstance ());
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

              floorMachine.ChangeState (McpttFloorMachineBasicStateQueued::GetInstance ());
            }
        }
      else
        {
          //Update the queue status.

          floorMachine.SetCurrentSsrc (rxSsrc);
          
          //Notify the MCPTT user about the queue position
          
          t201->Stop ();

          floorMachine.ChangeState (McpttFloorMachineBasicStateQueued::GetInstance ());
        }
    }
}

void
McpttFloorMachineBasicStatePendReq::ReceiveFloorReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRequest& msg) const
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
McpttFloorMachineBasicStatePendReq::ReceiveFloorTaken (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgTaken& msg) const
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
McpttFloorMachineBasicStatePendReq::ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetHead ().GetSsrc ();

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
McpttFloorMachineBasicStatePendReq::TakeReleaseNotification (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

  floorMachine.ChangeState (McpttFloorMachineBasicStateSilence::GetInstance ());
}
/** McpttFloorMachineBasicStatePendReq - end **/

/** McpttFloorMachineBasicStateQueued - begin **/
Ptr<McpttFloorMachineBasicStateQueued>
McpttFloorMachineBasicStateQueued::GetInstance (void)
{
  static Ptr<McpttFloorMachineBasicStateQueued> instance = Create<McpttFloorMachineBasicStateQueued> ();

  return instance;
}

McpttEntityId
McpttFloorMachineBasicStateQueued::GetStateId (void)
{
  static McpttEntityId id (5, "'O: queued'");

  return id;
}

McpttFloorMachineBasicStateQueued::McpttFloorMachineBasicStateQueued (void)
  : McpttFloorMachineBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMachineBasicStateQueued::~McpttFloorMachineBasicStateQueued (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineBasicStateQueued::AcceptGrant (McpttFloorMachineBasic& floorMachine) const
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
      std::list<McpttQueuedUserInfo> users = grantedMsg.GetUsers();
      queue->UpdateUsers (users);
    }

  floorMachine.ChangeState (McpttFloorMachineBasicStateHasPerm::GetInstance ());
}

void
McpttFloorMachineBasicStateQueued::ExpiryOfT203 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

  floorMachine.ChangeState (McpttFloorMachineBasicStatePendReq::GetInstance ());
}

void
McpttFloorMachineBasicStateQueued::ExpiryOfT204 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

      floorMachine.ChangeState (McpttFloorMachineBasicStateSilence::GetInstance ());
    }
  else
    {   
      McpttFloorMsgQueuePosReq reqMsg (txSsrc);
      reqMsg.SetUserId (myUserId);

      c204->Increment ();
      t204->Start ();

      floorMachine.Send (reqMsg);
    }
}

void
McpttFloorMachineBasicStateQueued::ExpiryOfT233 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();

  t203->Stop ();
  t230->Start ();

  floorMachine.ChangeState (McpttFloorMachineBasicStateSilence::GetInstance ());
}

McpttEntityId
McpttFloorMachineBasicStateQueued::GetInstanceStateId (void) const
{
  return McpttFloorMachineBasicStateQueued::GetStateId ();
}

void
McpttFloorMachineBasicStateQueued::ReceiveFloorDeny (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgDeny& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  McpttFloorMsgFieldUserId rxIdField = msg.GetUserId ();
  uint32_t rxUserId = rxIdField.GetUserId ();

  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

          floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
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

          floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
         }
    }
}

void
McpttFloorMachineBasicStateQueued::ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  uint32_t grantedSsrc = msg.GetGrantedSsrc ();
  McpttFloorMsgFieldUserId rxIdField = msg.GetUserId ();
  uint32_t rxUserId = rxIdField.GetUserId ();
  McpttFloorMsgFieldIndic rxIndicator = msg.GetIndicator ();

  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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
              Simulator::Schedule (Seconds (0), &McpttFloorMachineBasic::NotifyFloorGranted, &floorMachine);
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
              Simulator::Schedule (Seconds (0), &McpttFloorMachineBasic::NotifyFloorGranted, &floorMachine);
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
McpttFloorMachineBasicStateQueued::ReceiveFloorQueuePositionInfo (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgQueueInfo& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  McpttFloorMsgFieldUserId rxIdField = msg.GetQueuedUserId ();
  uint32_t rxUserId = rxIdField.GetUserId ();

  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
  Ptr<McpttTimer> t204 = floorMachine.GetT204 ();
  uint32_t currenSsrc = floorMachine.GetCurrentSsrc ();
  uint32_t candidateSsrc = floorMachine.GetCandidateSsrc ();

  if (myUserId == rxUserId)
    {
      if (currenSsrc == rxSsrc)
        {
          //Update the queue position.
          //Notify the user abou tthe queue position received.
          if (t204->IsRunning())
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
          if (t204->IsRunning())
            {
              t204->Stop ();
            }
        }
    }
}

void
McpttFloorMachineBasicStateQueued::ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const
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
McpttFloorMachineBasicStateQueued::ReleaseRequest (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttTimer> t233 = floorMachine.GetT233 ();

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();

  McpttFloorMsgRelease releaseMsg (txSsrc);
  releaseMsg.SetUserId (McpttFloorMsgFieldUserId (myUserId));

  if (t233->IsRunning ())
    {
      t233->Stop ();
    }

  floorMachine.Send (releaseMsg);

  floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
}

void
McpttFloorMachineBasicStateQueued::SendFloorQueuePosReq (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  Ptr<McpttTimer> t204 = floorMachine.GetT204 ();
  Ptr<McpttCounter> c204 = floorMachine.GetC204 ();

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();

  McpttFloorMsgQueuePosReq queuePosReqMsg (txSsrc);
  queuePosReqMsg.SetUserId (McpttFloorMsgFieldUserId (myUserId));

  c204->Reset ();
  t204->Start ();

  floorMachine.Send (queuePosReqMsg);
}
/** McpttFloorMachineBasicStateQueued - end **/

/** McpttFloorMachineBasicStateSilence - begin **/
Ptr<McpttFloorMachineBasicStateSilence>
McpttFloorMachineBasicStateSilence::GetInstance (void)
{
  static Ptr<McpttFloorMachineBasicStateSilence> instance = Create<McpttFloorMachineBasicStateSilence> ();

  return instance;
}

McpttEntityId
McpttFloorMachineBasicStateSilence::GetStateId (void)
{
  static McpttEntityId id (6, "'O: silence'");

  return id;
}

McpttFloorMachineBasicStateSilence::McpttFloorMachineBasicStateSilence (void)
  : McpttFloorMachineBasicState ()
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMachineBasicStateSilence::~McpttFloorMachineBasicStateSilence (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineBasicStateSilence::ExpiryOfT230 (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  //Shall indicate to call control that T230 expired.
  //TODO: Indicate to call control tha T230 has expired.

  floorMachine.ChangeState (McpttFloorMachineBasicStateStartStop::GetInstance ());
}

McpttEntityId
McpttFloorMachineBasicStateSilence::GetInstanceStateId (void) const
{
  return McpttFloorMachineBasicStateSilence::GetStateId ();
}

void
McpttFloorMachineBasicStateSilence::ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  uint32_t rxSsrc = msg.GetSsrc ();
  uint32_t myUserId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

      floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
    }
}

void
McpttFloorMachineBasicStateSilence::ReceiveFloorReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRequest& msg) const
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

      floorMachine.ChangeState (McpttFloorMachineBasicStatePendGrant::GetInstance ());
    }
}

void
McpttFloorMachineBasicStateSilence::ReceiveFloorTaken (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgTaken& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  //Provide floor taken notification to user.
  uint32_t rxSsrc = msg.GetSsrc ();

  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();

  floorMachine.SetCurrentSsrc (rxSsrc);
  t230->Stop ();
  t203->Start ();

  floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
}

void
McpttFloorMachineBasicStateSilence::ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const
{
  NS_LOG_FUNCTION (this << &floorMachine << msg);

  //Provide floor taken notification.
  //Request client to start rendering received RTP media packets.
  uint32_t rxSsrc = msg.GetHead ().GetSsrc ();

  Ptr<McpttTimer> t230 = floorMachine.GetT230 ();
  Ptr<McpttTimer> t203 = floorMachine.GetT203 ();
  uint32_t storedSsrc = floorMachine.GetCurrentSsrc ();

  if (floorMachine.HasCurrentSsrc ())
    {
      if (storedSsrc == rxSsrc)
        {
          t203->Restart ();
          t230->Stop ();

          floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
        }
    }
  else
    {
      t230->Stop ();

      floorMachine.SetCurrentSsrc (rxSsrc);

      t203->Restart ();

      floorMachine.ChangeState (McpttFloorMachineBasicStateNoPerm::GetInstance ());
    }
}

void
McpttFloorMachineBasicStateSilence::Selected (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  McpttPttApp* appOwner = floorMachine.GetOwner ()->GetOwner ();

  if (appOwner->IsPushed ())
    {
      appOwner->Released ();
    }
}

void
McpttFloorMachineBasicStateSilence::TakePushNotification (McpttFloorMachineBasic& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  uint32_t txSsrc = floorMachine.GetTxSsrc ();
  uint32_t userId = floorMachine.GetOwner ()->GetOwner ()->GetUserId ();
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

  floorMachine.ChangeState (McpttFloorMachineBasicStatePendReq::GetInstance ());
}
/** McpttFloorMachineBasicStateSilence - end **/

} //namespace ns3

