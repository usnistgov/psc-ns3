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

#include "mcptt-ptt-app.h"
#include "mcptt-floor-participant.h"
#include "mcptt-call-type-machine.h"
#include "mcptt-call-machine-grp-basic.h"
#include "mcptt-call-msg.h"
#include "mcptt-emerg-alert-machine.h"
#include "mcptt-entity-id.h"

#include "mcptt-call-type-machine-grp-basic.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallTypeMachineGrpBasic");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttCallTypeMachineGrpBasic);

const McpttEntityId McpttCallTypeMachineGrpBasic::T0 = McpttEntityId (0, "'T0: waiting for call to establish'");

const McpttEntityId McpttCallTypeMachineGrpBasic::T1 = McpttEntityId (1, "'T1: in-progress emergency group call'");

const McpttEntityId McpttCallTypeMachineGrpBasic::T2 = McpttEntityId (2, "'T2: in-progress basic group call'");

const McpttEntityId McpttCallTypeMachineGrpBasic::T3 = McpttEntityId (3, "'T3: in-progress imminent peril group call'");

TypeId
McpttCallTypeMachineGrpBasic::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttCallTypeMachineGrpBasic")
    .SetParent<McpttCallTypeMachine> ()
    .AddConstructor<McpttCallTypeMachineGrpBasic> ()
    .AddAttribute ("CFG11", "The limit of counter Cfg11.",
                   UintegerValue (5),
                   MakeUintegerAccessor (&McpttCallTypeMachineGrpBasic::SetLimitCfg11),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("CFG12", "The limit of counter Cfg12.",
                   UintegerValue (5),
                   MakeUintegerAccessor (&McpttCallTypeMachineGrpBasic::SetLimitCfg12),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("TFG11", "The delay to use for timer TFG11 (Time value)",
                   TimeValue (Seconds (1)),
                   MakeTimeAccessor (&McpttCallTypeMachineGrpBasic::SetDelayTfg11),
                   MakeTimeChecker ())
    .AddAttribute ("TFG12", "The delay to use for timer TFG12 (Time value)",
                   TimeValue (Seconds (1)),
                   MakeTimeAccessor (&McpttCallTypeMachineGrpBasic::SetDelayTfg12),
                   MakeTimeChecker ())
    .AddAttribute ("TFG13-X", "The X value to use in the delay calculation for TFG13 (Time value)",
                   TimeValue (Seconds (255)),
                   MakeTimeAccessor (&McpttCallTypeMachineGrpBasic::m_delayTfg13X),
                   MakeTimeChecker ())
    .AddAttribute ("TFG14-X", "The X value to use in the delay calculation for TFG14 (Time value)",
                   TimeValue (Seconds (255)),
                   MakeTimeAccessor (&McpttCallTypeMachineGrpBasic::m_delayTfg14X),
                   MakeTimeChecker ())
    .AddTraceSource ("StateChangeTrace", "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttCallTypeMachineGrpBasic::m_stateChangeTrace),
                     "ns3::psc::McpttCallMachine::StateChangeTracedCallback")
  ;

  return tid;
}

McpttCallTypeMachineGrpBasic::McpttCallTypeMachineGrpBasic (Ptr<McpttCallMachineGrpBasic> owner)
  : McpttCallTypeMachine (),
    m_callType (McpttCallMsgFieldCallType::BASIC_GROUP),
    m_cfg11 (CreateObject<McpttCounter> (McpttEntityId (11, "CFG11"))),
    m_cfg12 (CreateObject<McpttCounter> (McpttEntityId (12, "CFG12"))),
    m_lastChgTime (McpttCallMsgFieldLastChgTime ()),
    m_lastChgUserId (McpttCallMsgFieldUserId ()),
    m_owner (owner),
    m_priority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP)),
    m_started (false),
    m_stateId (McpttCallTypeMachineGrpBasic::T0),
    m_tfg11 (CreateObject<McpttTimer> (McpttEntityId (11, "TFG11"))),
    m_tfg12 (CreateObject<McpttTimer> (McpttEntityId (12, "TFG12"))),
    m_tfg13 (CreateObject<McpttTimer> (McpttEntityId (13, "TFG13"))),
    m_tfg14 (CreateObject<McpttTimer> (McpttEntityId (14, "TFG14"))),
    m_upgradeCb (MakeNullCallback<void, uint8_t> ())
{
  NS_LOG_FUNCTION (this);

  m_tfg11->Link (&McpttCallTypeMachineGrpBasic::ExpiryOfTfg11, this);
  m_tfg12->Link (&McpttCallTypeMachineGrpBasic::ExpiryOfTfg12, this);
  m_tfg13->Link (&McpttCallTypeMachineGrpBasic::ExpiryOfTfg13, this);
  m_tfg14->Link (&McpttCallTypeMachineGrpBasic::ExpiryOfTfg14, this);
}

McpttCallTypeMachineGrpBasic::~McpttCallTypeMachineGrpBasic (void)
{
  NS_LOG_FUNCTION (this);
}

Time
McpttCallTypeMachineGrpBasic::CalcDelayForTfg13 (void) const
{
  NS_LOG_FUNCTION (this);

  McpttCallMsgFieldLastChgTime lastChgTimeField = GetLastChgTime ();

  Time now = Simulator::Now ();
  Time lastChgTime = lastChgTimeField.GetTime ();

  double x = m_delayTfg13X.GetSeconds ();
  double y = now.GetSeconds ();
  double z = lastChgTime.GetSeconds ();
  double seconds = x - (y - z);

  NS_LOG_LOGIC ("TFG13= " << x << " - (" << y << " - " << z << ") = " << seconds);

  //TODO: Not in standard - making sure that the calculated time is at least zero.
  seconds = (seconds < 0.0 ? 0.0 : seconds);

  Time delay = Seconds (seconds);

  return delay;
}

Time
McpttCallTypeMachineGrpBasic::CalcDelayForTfg14 (void) const
{
  NS_LOG_FUNCTION (this);

  McpttCallMsgFieldLastChgTime lastChgTimeField = GetLastChgTime ();

  Time now = Simulator::Now ();
  Time lastChgTime = lastChgTimeField.GetTime ();

  double x = m_delayTfg14X.GetSeconds ();
  double y = now.GetSeconds ();
  double z = lastChgTime.GetSeconds ();
  double seconds = x - (y - z);

  NS_LOG_LOGIC ("TFG14= " << x << " - (" << y << " - " << z << ") = " << seconds);

  //TODO: Not in standard - making sure that the calculated time is at least zero.
  seconds = (seconds < 0.0 ? 0.0 : seconds);

  Time delay = Seconds (seconds);

  return delay;
}

void
McpttCallTypeMachineGrpBasic::CallStarted (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " call started.");

  Ptr<McpttTimer> tfg13 = GetTfg13 ();
  Ptr<McpttTimer> tfg14 = GetTfg14 ();
  McpttEntityId stateId = GetStateId ();
  McpttCallMsgFieldCallType callType = GetCallType ();

  if (stateId == McpttCallTypeMachineGrpBasic::T0)
    {
      //TODO: Not in standard - updating the last change time to NOW when starting a new call.
      SetLastChgTime (Simulator::Now ());
      if (callType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
        {
          Time delayTfg13 = CalcDelayForTfg13 ();
          tfg13->SetDelay (delayTfg13);
          tfg13->Start ();
          ChangeState (McpttCallTypeMachineGrpBasic::T1);
        }
      else if (callType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
        {
          Time delayTfg14 = CalcDelayForTfg14 ();
          tfg14->SetDelay (delayTfg14);
          tfg14->Start ();
          ChangeState (McpttCallTypeMachineGrpBasic::T3);
        }
      else if (callType.GetType () == McpttCallMsgFieldCallType::BASIC_GROUP)
        {
          ChangeState (McpttCallTypeMachineGrpBasic::T2);
        }
    }
}

void
McpttCallTypeMachineGrpBasic::ChangeState (const McpttEntityId& stateId)
{
  NS_LOG_FUNCTION (this << stateId);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }
  std::string selected = "0";
  if (GetOwner ()->GetCall ()->GetCallId () == GetOwner ()->GetCall ()->GetOwner ()->GetSelectedCall ()->GetCallId ())
    {
      selected = "1";
    }

  if (stateId != m_stateId)
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () << " moving from " << GetStateId ().GetName () << " to " << stateId.GetName () << ".");
      m_stateChangeTrace (m_owner->GetCall ()->GetOwner ()->GetUserId (), m_owner->GetCall ()->GetCallId (), selected, GetInstanceTypeId ().GetName (), GetStateId ().GetName (), stateId.GetName ());
      SetStateId (stateId);
    }
}

void
McpttCallTypeMachineGrpBasic::Downgrade (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " downgrading call.");

  Ptr<McpttCallMachineGrpBasic> owner = GetOwner ();
  uint32_t myUserId = owner->GetCall ()->GetOwner ()->GetUserId ();
  McpttCallMsgFieldCallId callId = owner->GetCallId ();
  McpttCallMsgFieldUserId origId = owner->GetOrigId ();
  McpttCallMsgFieldGrpId grpId = owner->GetGrpId ();
  McpttEntityId stateId = GetStateId ();
  McpttCallMsgFieldLastChgTime lastChgTime (Simulator::Now ());
  McpttCallMsgFieldUserId lastChgUserId (myUserId);
  Ptr<McpttCounter> cfg11 = GetCfg11 ();
  Ptr<McpttCounter> cfg12 = GetCfg12 ();
  Ptr<McpttTimer> tfg11 = GetTfg11 ();
  Ptr<McpttTimer> tfg12 = GetTfg12 ();
  Ptr<McpttTimer> tfg13 = GetTfg13 ();
  Ptr<McpttTimer> tfg14 = GetTfg14 ();

  if (stateId == McpttCallTypeMachineGrpBasic::T1)
    {
      SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
      SetLastChgTime (lastChgTime);
      SetLastChgUserId (lastChgUserId);

      McpttCallMsgGrpEmergEnd msg;
      msg.SetCallId (callId);
      msg.SetUserId (origId);
      msg.SetGrpId (grpId);
      msg.SetLastChgTime (lastChgTime);
      msg.SetLastChgUserId (lastChgUserId);

      tfg13->Stop ();
      cfg11->Reset ();
      tfg11->Start ();

      Send (msg);

      ChangeState (McpttCallTypeMachineGrpBasic::T2);

      if (!m_downgradeCb.IsNull ())
        {
          m_downgradeCb ();
        }
    }
  else if (stateId == McpttCallTypeMachineGrpBasic::T3)
    {
      SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
      SetLastChgTime (lastChgTime);
      SetLastChgUserId (lastChgUserId);

      McpttCallMsgGrpImmPerilEnd msg;
      msg.SetCallId (callId);
      msg.SetUserId (origId);
      msg.SetGrpId (grpId);
      msg.SetLastChgTime (lastChgTime);
      msg.SetLastChgUserId (lastChgUserId);

      tfg14->Stop ();
      cfg12->Reset ();
      tfg12->Start ();

      Send (msg);

      ChangeState (McpttCallTypeMachineGrpBasic::T2);

      if (!m_downgradeCb.IsNull ())
        {
          m_downgradeCb ();
        }
    }
}

void
McpttCallTypeMachineGrpBasic::ExpiryOfTfg1 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  "'s TFG1 timer expired.");

  CallStarted ();
}

TypeId
McpttCallTypeMachineGrpBasic::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallTypeMachineGrpBasic::GetTypeId ();
}

void
McpttCallTypeMachineGrpBasic::InitiateCall (uint8_t callType)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " initiating group call.");

  Ptr<McpttCallMachineGrpBasic> owner = GetOwner ();
  McpttEntityId stateId = GetStateId ();
  Ptr<McpttEmergAlertMachine> emergMachineGrpBasic = owner->GetEmergMachine ();
  uint32_t myUserId = owner->GetCall ()->GetOwner ()->GetUserId ();
  McpttCallMsgFieldLastChgTime lastChgTime (Simulator::Now ());
  McpttCallMsgFieldUserId lastChgUserId (myUserId);

  if (stateId == McpttCallTypeMachineGrpBasic::T0)
    {
      if (emergMachineGrpBasic->IsInEmergState ())
        {
          SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
          SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
        }
      else
        {
          if (callType == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
            {
              SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
              SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
            }
          else if (callType == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
            {
              SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
              SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
            }
          else if (callType == McpttCallMsgFieldCallType::BASIC_GROUP)
            {
              SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
              SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
            }
        }

      SetLastChgTime (lastChgTime);
      SetLastChgUserId (lastChgUserId);
    }
}

bool
McpttCallTypeMachineGrpBasic::IsStarted (void) const
{
  NS_LOG_FUNCTION (this);

  bool isStarted = GetStarted ();

  return isStarted;
}

void
McpttCallTypeMachineGrpBasic::ReceiveGrpCallAnnoun (const McpttCallMsgGrpAnnoun& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " received " << msg << ".");

  McpttEntityId stateId = GetStateId ();
  Ptr<McpttCallMachineGrpBasic> owner = GetOwner ();
  bool userAckRequired = owner->IsUserAckReq ();
  Ptr<McpttTimer> tfg13 = GetTfg13 ();
  Ptr<McpttTimer> tfg14 = GetTfg14 ();
  McpttCallMsgFieldCallId theirCallId = msg.GetCallId ();
  McpttCallMsgFieldGrpId theirGrpId = msg.GetGrpId ();
  McpttCallMsgFieldCallType theirCallType = msg.GetCallType ();
  McpttCallMsgFieldStartTime theirStartTime = msg.GetStartTime ();
  McpttCallMsgFieldLastChgTime theirLastChgTime = msg.GetLastChgTime ();
  McpttCallMsgFieldUserId theirLastChgUserId = msg.GetLastChgUserId ();
  McpttCallMsgFieldLastChgTime myLastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId myLastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldGrpId myGrpId = owner->GetGrpId ();
  McpttCallMsgFieldCallId myCallId = owner->GetCallId ();
  McpttCallMsgFieldCallType myCallType = GetCallType ();
  McpttCallMsgFieldStartTime myStartTime = owner->GetStartTime ();

  //TODO: Add to official model
  if (GetOwner ()->GetGrpId ().GetGrpId () != msg.GetGrpId ().GetGrpId ())
    {
      return;
    }

  if (stateId == McpttCallTypeMachineGrpBasic::T0)
    {
      if (msg.IsProbeResp ())
        {
          if (theirCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
            {
              SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
              SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
              SetLastChgTime (theirLastChgTime);
              SetLastChgUserId (theirLastChgUserId);
              Time delayTfg13 = CalcDelayForTfg13 ();
              tfg13->SetDelay (delayTfg13);
              tfg13->Start ();
              ChangeState (McpttCallTypeMachineGrpBasic::T1);
            }
          else if (theirCallType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
            {
              SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
              SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
              SetLastChgTime (theirLastChgTime);
              SetLastChgUserId (theirLastChgUserId);
              Time delayTfg14 = CalcDelayForTfg14 ();
              tfg14->SetDelay (delayTfg14);
              tfg14->Start ();
              ChangeState (McpttCallTypeMachineGrpBasic::T3);
            }
          else if (theirCallType.GetType () == McpttCallMsgFieldCallType::BASIC_GROUP)
            {
              SetLastChgTime (theirLastChgTime);
              SetLastChgUserId (theirLastChgUserId);
              ChangeState (McpttCallTypeMachineGrpBasic::T2);
            }
        }
      else if (userAckRequired == true)
        {
          if (theirCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
            {
              SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
              SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
            }
          else if (theirCallType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
            {
              SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
              SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
            }
          else if (theirCallType.GetType () == McpttCallMsgFieldCallType::BASIC_GROUP)
            {
              SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
              SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
            }
          SetLastChgTime (theirLastChgTime);
          SetLastChgUserId (theirLastChgUserId);
        }
      else if (userAckRequired == false)
        {
          SetLastChgTime (theirLastChgTime);
          SetLastChgUserId (theirLastChgUserId);

          if (theirCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
            {
              SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
              SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
              Time delayTfg13 = CalcDelayForTfg13 ();
              tfg13->SetDelay (delayTfg13);
              tfg13->Start ();
              ChangeState (McpttCallTypeMachineGrpBasic::T1);
            }
          else if (theirCallType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
            {
              SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
              SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
              Time delayTfg14 = CalcDelayForTfg14 ();
              tfg14->SetDelay (delayTfg14);
              tfg14->Start ();
              ChangeState (McpttCallTypeMachineGrpBasic::T3);
            }
          else if (theirCallType.GetType () == McpttCallMsgFieldCallType::BASIC_GROUP)
            {
              SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
              SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
              ChangeState (McpttCallTypeMachineGrpBasic::T2);
            }
        }
    }
  else if (stateId == McpttCallTypeMachineGrpBasic::T1
           || stateId == McpttCallTypeMachineGrpBasic::T2
           || stateId == McpttCallTypeMachineGrpBasic::T3)
    {
      //match group ID and call id.
      if (myGrpId.GetGrpId () == theirGrpId.GetGrpId ()
          && myCallId.GetCallId () == theirCallId.GetCallId ())
        {
          if (myLastChgUserId.GetId () == theirLastChgUserId.GetId ()
              && myLastChgTime.GetTime () < theirLastChgTime.GetTime ())
            {
              SetLastChgTime (theirLastChgTime);
              if (!(myCallType.GetType () == theirCallType.GetType ()))
                {
                  if (theirCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
                    {
                      SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
                      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
                      Time delayTfg13 = CalcDelayForTfg13 ();
                      tfg13->SetDelay (delayTfg13);
                      tfg13->Start ();
                      if (tfg14->IsRunning ())
                        {
                          tfg14->Stop ();
                        }
                      ChangeState (McpttCallTypeMachineGrpBasic::T1);
                    }
                  else if (theirCallType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
                    {
                      SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
                      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
                      Time delayTfg14 = CalcDelayForTfg14 ();
                      tfg14->SetDelay (delayTfg14);
                      tfg14->Start ();
                      ChangeState (McpttCallTypeMachineGrpBasic::T3);
                    }
                  else if (theirCallType.GetType () == McpttCallMsgFieldCallType::BASIC_GROUP)
                    {
                      SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
                      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
                      if (tfg13->IsRunning ())
                        {
                          tfg13->Stop ();
                        }
                      //TODO: Not in standard - Stopping timer TFG14 if it is running when merging to a basic group call.
                      if (tfg14->IsRunning ())
                        {
                          tfg14->Stop ();
                        }
                      ChangeState (McpttCallTypeMachineGrpBasic::T2);
                    }
                }
            }
          else if (myLastChgUserId.GetId () != theirLastChgUserId.GetId ())
            {
              if (myCallType.GetType () == theirCallType.GetType ())
                {
                  SetLastChgTime (theirLastChgTime);
                  SetLastChgUserId (theirLastChgUserId);
                }
              else // myCallType != theirCallType
                {
                  if (theirCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
                    {
                      SetLastChgTime (theirLastChgTime);
                      SetLastChgUserId (theirLastChgUserId);
                      SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
                      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
                      Time delayTfg13 = CalcDelayForTfg13 ();
                      tfg13->SetDelay (delayTfg13);
                      tfg13->Start ();
                      if (tfg14->IsRunning ())
                        {
                          tfg14->Stop ();
                        }
                      ChangeState (McpttCallTypeMachineGrpBasic::T1);
                    }
                  else if (theirCallType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP
                           && myCallType.GetType () == McpttCallMsgFieldCallType::BASIC_GROUP)
                    {
                      SetLastChgTime (theirLastChgTime);
                      SetLastChgUserId (theirLastChgUserId);
                      SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
                      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
                      Time delayTfg14 = CalcDelayForTfg14 ();
                      tfg14->SetDelay (delayTfg14);
                      tfg14->Start ();
                      ChangeState (McpttCallTypeMachineGrpBasic::T3);
                    }
                }
            }
        }
      else
        {
          bool clause1 = (myCallType.GetType () == McpttCallMsgFieldCallType::BASIC_GROUP)
            && (theirCallType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP
                || theirCallType.GetType () == (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
          bool clause2 = (myCallType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
            && (theirCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP);
          bool clause3 = (myCallType.GetType () == theirCallType.GetType ())
            && (theirStartTime.GetTime () < myStartTime.GetTime ());
          bool clause4 = (myCallType.GetType () == theirCallType.GetType ())
            && (theirStartTime.GetTime () == myStartTime.GetTime ())
            && (theirCallId.GetCallId () < myCallId.GetCallId ());

          if (clause1
              || clause2
              || clause3
              || clause4)
            {
              SetLastChgTime (theirLastChgTime);
              SetLastChgUserId (theirLastChgUserId);
              SetCallType (theirCallType);

              if (theirCallType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
                {
                  //TODO: Not in standard - Calculating the delay for TFG13 and starting it when merging to an 'EMERGENCY' call.
                  Time delayTfg13 = CalcDelayForTfg13 ();
                  tfg13->SetDelay (delayTfg13);
                  tfg13->Start ();
                  //TODO: Not in standard - Stopping TFG14 if it is running when merging to an 'EMERGENCY' call.
                  if (tfg14->IsRunning ())
                    {
                      tfg14->Stop ();
                    }
                  SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
                  ChangeState (McpttCallTypeMachineGrpBasic::T1);
                }
              else if (theirCallType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
                {
                  //TODO: Not in standard - Calculating the delay for TFG14 and starting it when merging to an 'IMMINENT PERIL' call.
                  Time delayTfg14 = CalcDelayForTfg14 ();
                  tfg14->SetDelay (delayTfg14);
                  tfg14->Start ();
                  SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
                  ChangeState (McpttCallTypeMachineGrpBasic::T3);
                }
            }
        }
    }
}

void
McpttCallTypeMachineGrpBasic::ReceiveGrpCallEmergEnd (const McpttCallMsgGrpEmergEnd& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  //TODO: Add to official model
  if (GetOwner ()->GetGrpId ().GetGrpId () != msg.GetGrpId ().GetGrpId ())
    {
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " received " << msg << ".");

  McpttEntityId stateId = GetStateId ();
  McpttCallMsgFieldUserId theirLastChgUserId = msg.GetLastChgUserId ();
  McpttCallMsgFieldLastChgTime theirLastChgTime = msg.GetLastChgTime ();
  Ptr<McpttTimer> tfg13 = GetTfg13 ();

  if (stateId == McpttCallTypeMachineGrpBasic::T1)
    {
      SetLastChgTime (theirLastChgTime);
      SetLastChgUserId (theirLastChgUserId);
      SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
      tfg13->Stop ();
      ChangeState (McpttCallTypeMachineGrpBasic::T2);
    }
}

void
McpttCallTypeMachineGrpBasic::ReceiveGrpCallImmPerilEnd (const McpttCallMsgGrpImmPerilEnd& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " received " << msg << ".");

  McpttEntityId stateId = GetStateId ();
  McpttCallMsgFieldUserId theirLastChgUserId = msg.GetLastChgUserId ();
  McpttCallMsgFieldLastChgTime theirLastChgTime = msg.GetLastChgTime ();
  Ptr<McpttTimer> tfg14 = GetTfg14 ();

  //TODO: Add to official model
  if (GetOwner ()->GetGrpId ().GetGrpId () != msg.GetGrpId ().GetGrpId ())
    {
      return;
    }

  if (stateId == McpttCallTypeMachineGrpBasic::T3)
    {
      SetLastChgTime (theirLastChgTime);
      SetLastChgUserId (theirLastChgUserId);
      SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
      tfg14->Stop ();
      ChangeState (McpttCallTypeMachineGrpBasic::T2);
    }
}

void
McpttCallTypeMachineGrpBasic::ReleaseCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " releasing call.");

  McpttEntityId stateId = GetStateId ();

  if (stateId == McpttCallTypeMachineGrpBasic::T0)
    {
      SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
      SetLastChgTime (Seconds (0));
      SetLastChgUserId (0);
    }
  else if (stateId == McpttCallTypeMachineGrpBasic::T1
           || stateId == McpttCallTypeMachineGrpBasic::T2
           || stateId == McpttCallTypeMachineGrpBasic::T3)
    {
      SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
      SetLastChgTime (Seconds (0));
      SetLastChgUserId (0);
      ChangeState (McpttCallTypeMachineGrpBasic::T0);
    }
}

void
McpttCallTypeMachineGrpBasic::RejectCall (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " rejecting call.");

  McpttEntityId stateId = GetStateId ();

  if (stateId == McpttCallTypeMachineGrpBasic::T0)
    {
      SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
      SetLastChgTime (Seconds (0));
      SetLastChgUserId (0);
    }
}

void
McpttCallTypeMachineGrpBasic::Send (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () << " trying to send " << msg << ".");

  Ptr<McpttCallMachineGrpBasic> owner = GetOwner ();
  owner->Send (msg);
}

void
McpttCallTypeMachineGrpBasic::SetDelayTfg11 (const Time& delayTfg11)
{
  NS_LOG_FUNCTION (this << &delayTfg11);

  GetTfg11 ()->SetDelay (delayTfg11);
}

void
McpttCallTypeMachineGrpBasic::SetDelayTfg12 (const Time& delayTfg12)
{
  NS_LOG_FUNCTION (this << &delayTfg12);

  GetTfg12 ()->SetDelay (delayTfg12);
}

void
McpttCallTypeMachineGrpBasic::SetLimitCfg11 (uint32_t limitCfg11)
{
  NS_LOG_FUNCTION (this << limitCfg11);

  GetCfg11 ()->SetLimit (limitCfg11);
}

void
McpttCallTypeMachineGrpBasic::SetLimitCfg12 (uint32_t limitCfg12)
{
  NS_LOG_FUNCTION (this << limitCfg12);

  GetCfg12 ()->SetLimit (limitCfg12);
}

void
McpttCallTypeMachineGrpBasic::Start (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " started.");

  SetStarted (true);
}

void
McpttCallTypeMachineGrpBasic::Stop (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " stopped.");

  Ptr<McpttTimer> tfg11 = GetTfg11 ();
  Ptr<McpttTimer> tfg12 = GetTfg12 ();
  Ptr<McpttTimer> tfg13 = GetTfg13 ();
  Ptr<McpttTimer> tfg14 = GetTfg14 ();
  Ptr<McpttCounter> cfg11 = GetCfg11 ();
  Ptr<McpttCounter> cfg12 = GetCfg12 ();

  SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  SetLastChgTime (Seconds (0));
  SetLastChgUserId (0);
  SetStateId (McpttCallTypeMachineGrpBasic::T0);

  cfg11->Reset ();
  cfg12->Reset ();

  if (tfg11->IsRunning ())
    {
      tfg11->Stop ();
    }

  if (tfg12->IsRunning ())
    {
      tfg12->Stop ();
    }

  if (tfg13->IsRunning ())
    {
      tfg13->Stop ();
    }

  if (tfg14->IsRunning ())
    {
      tfg14->Stop ();
    }

  SetStarted (false);
}

void
McpttCallTypeMachineGrpBasic::UpgradeTo (const McpttCallMsgFieldCallType& callType)
{
  NS_LOG_FUNCTION (this << callType);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () << " " << GetOwner ()->GetCall ()->GetOwner ()->GetUserId () <<  " upgrading call type to " << (uint32_t)callType.GetType () << ".");

  Ptr<McpttCallMachineGrpBasic> owner = GetOwner ();
  McpttEntityId stateId = GetStateId ();
  uint32_t myUserId = owner->GetCall ()->GetOwner ()->GetUserId ();
  McpttCallMsgFieldCallId callId = owner->GetCallId ();
  McpttCallMsgFieldSdp sdp = owner->GetSdp ();
  McpttCallMsgFieldRefreshInterval refInt = owner->GetRefInt ();
  McpttCallMsgFieldUserId origId = owner->GetOrigId ();
  McpttCallMsgFieldGrpId grpId = owner->GetGrpId ();
  McpttCallMsgFieldStartTime startTime = owner->GetStartTime ();
  Ptr<McpttTimer> tfg11 = GetTfg11 ();
  Ptr<McpttTimer> tfg12 = GetTfg12 ();
  Ptr<McpttTimer> tfg13 = GetTfg13 ();
  Ptr<McpttTimer> tfg14 = GetTfg14 ();
  McpttCallMsgFieldLastChgTime lastChgTime = McpttCallMsgFieldLastChgTime (Simulator::Now ());
  McpttCallMsgFieldUserId lastChgUserId = McpttCallMsgFieldUserId (myUserId);

  bool upgradeFromBasic = (stateId == McpttCallTypeMachineGrpBasic::T2
                           && (callType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP
                               || callType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  bool upgradeFromImmPeril = (stateId == McpttCallTypeMachineGrpBasic::T3
                              && callType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP);

  if (upgradeFromBasic
      || upgradeFromImmPeril)
    {
      //TODO: Not in standard - Making sure that the last change time and last changed user ID are updated before timer delay calculations.
      SetLastChgTime (lastChgTime);
      SetLastChgUserId (lastChgUserId);
      if (callType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
        {
          SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
          SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
          Time delayTfg13 = CalcDelayForTfg13 ();
          tfg13->SetDelay (delayTfg13);
          tfg13->Start ();
          if (tfg14->IsRunning ())
            {
              tfg14->Stop ();
            }
          //TODO: Not in standard - Stopping timer TFG11 if it is running when upgrading to an 'EMERGENCY' call.
          if (tfg11->IsRunning ())
            {
              tfg11->Stop ();
            }
        }
      else if (callType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
        {
          SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
          SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
          Time delayTfg14 = CalcDelayForTfg14 ();
          tfg14->SetDelay (delayTfg14);
          tfg14->Start ();
          //TODO: Not in standard - Stopping timer TFG12 if it is running when upgrading to an 'IMMINENT PERIL' call.
          if (tfg12->IsRunning ())
            {
              tfg12->Stop ();
            }
        }


      McpttCallMsgGrpAnnoun msg;
      msg.SetCallId (callId);
      msg.SetCallType (callType);
      msg.SetRefInt (refInt);
      msg.SetSdp (sdp);
      msg.SetOrigId (origId);
      msg.SetGrpId (grpId);
      msg.SetStartTime (startTime);
      msg.SetLastChgTime (lastChgTime);
      msg.SetLastChgUserId (lastChgUserId);

      Send (msg);

      if (callType.GetType () == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
        {
          ChangeState (McpttCallTypeMachineGrpBasic::T1);
        }
      else if (callType.GetType () == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
        {
          ChangeState (McpttCallTypeMachineGrpBasic::T3);
        }

      if (!m_upgradeCb.IsNull ())
        {
          m_upgradeCb (callType.GetType ());
        }
    }
}

void
McpttCallTypeMachineGrpBasic::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetOwner (0);
  SetCfg11 (0);
  SetCfg12 (0);
  SetTfg11 (0);
  SetTfg12 (0);
  SetTfg13 (0);
  SetTfg14 (0);
}

void
McpttCallTypeMachineGrpBasic::ExpiryOfTfg11 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  "'s TFG11 timer expired " << GetCfg11 ()->GetValue () << " times.");

  Ptr<McpttCallMachineGrpBasic> owner = GetOwner ();
  McpttCallMsgFieldGrpId grpId = owner->GetGrpId ();
  McpttCallMsgFieldCallId callId = owner->GetCallId ();
  McpttCallMsgFieldUserId origId = owner->GetOrigId ();
  McpttEntityId stateId = GetStateId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  Ptr<McpttCounter> cfg11 = GetCfg11 ();
  Ptr<McpttTimer> tfg11 = GetTfg11 ();

  if (stateId == McpttCallTypeMachineGrpBasic::T2)
    {
      McpttCallMsgGrpEmergEnd msg;
      msg.SetCallId (callId);
      msg.SetUserId (origId);
      msg.SetGrpId (grpId);
      msg.SetLastChgTime (lastChgTime);
      msg.SetLastChgUserId (lastChgUserId);

      cfg11->Increment ();

      if (!cfg11->IsLimitReached ())
        {
          tfg11->Start ();
        }

      Send (msg);
    }
}

void
McpttCallTypeMachineGrpBasic::ExpiryOfTfg12 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  "'s TFG12 timer expired " << GetCfg12 ()->GetValue () << " times.");

  Ptr<McpttCallMachineGrpBasic> owner = GetOwner ();
  McpttCallMsgFieldGrpId grpId = owner->GetGrpId ();
  McpttCallMsgFieldCallId callId = owner->GetCallId ();
  McpttCallMsgFieldUserId origId = owner->GetOrigId ();
  McpttEntityId stateId = GetStateId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  Ptr<McpttCounter> cfg12 = GetCfg12 ();
  Ptr<McpttTimer> tfg12 = GetTfg12 ();

  if (stateId == McpttCallTypeMachineGrpBasic::T2)
    {
      McpttCallMsgGrpImmPerilEnd msg;
      msg.SetCallId (callId);
      msg.SetUserId (origId);
      msg.SetGrpId (grpId);
      msg.SetLastChgTime (lastChgTime);
      msg.SetLastChgUserId (lastChgUserId);

      Send (msg);

      cfg12->Increment ();

      if (!cfg12->IsLimitReached ())
        {
          tfg12->Start ();
        }
    }
}

void
McpttCallTypeMachineGrpBasic::ExpiryOfTfg13 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  "'s TFG13 timer expired.");

  Ptr<McpttCallMachineGrpBasic> owner = GetOwner ();
  McpttEntityId stateId = GetStateId ();
  McpttCallMsgFieldUserId lastChgUserId = owner->GetOrigId ();
  McpttCallMsgFieldLastChgTime lastChgTime (Simulator::Now ());

  if (stateId == McpttCallTypeMachineGrpBasic::T1)
    {
      SetLastChgTime (lastChgTime);
      SetLastChgUserId (lastChgUserId);
      SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
      ChangeState (McpttCallTypeMachineGrpBasic::T2);
    }
}

void
McpttCallTypeMachineGrpBasic::ExpiryOfTfg14 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  "'s TFG14 timer expired.");

  Ptr<McpttCallMachineGrpBasic> owner = GetOwner ();
  McpttEntityId stateId = GetStateId ();
  McpttCallMsgFieldUserId lastChgUserId = owner->GetOrigId ();
  McpttCallMsgFieldLastChgTime lastChgTime (Simulator::Now ());

  if (stateId == McpttCallTypeMachineGrpBasic::T3)
    {
      SetLastChgTime (lastChgTime);
      SetLastChgUserId (lastChgUserId);
      SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
      SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
      ChangeState (McpttCallTypeMachineGrpBasic::T2);
    }
}

McpttCallMsgFieldCallType
McpttCallTypeMachineGrpBasic::GetCallType (void) const
{
  NS_LOG_FUNCTION (this);

  return m_callType;
}

Ptr<McpttCounter>
McpttCallTypeMachineGrpBasic::GetCfg11 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_cfg11;
}

Ptr<McpttCounter>
McpttCallTypeMachineGrpBasic::GetCfg12 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_cfg12;
}

McpttCallMsgFieldLastChgTime
McpttCallTypeMachineGrpBasic::GetLastChgTime (void) const
{
  NS_LOG_FUNCTION (this);

  return m_lastChgTime;
}

McpttCallMsgFieldUserId
McpttCallTypeMachineGrpBasic::GetLastChgUserId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_lastChgUserId;
}

Ptr<McpttCallMachineGrpBasic>
McpttCallTypeMachineGrpBasic::GetOwner (void) const
{
  NS_LOG_FUNCTION (this);

  return m_owner;
}

uint8_t
McpttCallTypeMachineGrpBasic::GetPriority (void) const
{
  NS_LOG_FUNCTION (this);

  return m_priority;
}

bool
McpttCallTypeMachineGrpBasic::GetStarted (void) const
{
  NS_LOG_FUNCTION (this);

  return m_started;
}

McpttEntityId
McpttCallTypeMachineGrpBasic::GetStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_stateId;
}

Ptr<McpttTimer>
McpttCallTypeMachineGrpBasic::GetTfg11 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfg11;
}

Ptr<McpttTimer>
McpttCallTypeMachineGrpBasic::GetTfg12 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfg12;
}

Ptr<McpttTimer>
McpttCallTypeMachineGrpBasic::GetTfg13 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfg13;
}

Ptr<McpttTimer>
McpttCallTypeMachineGrpBasic::GetTfg14 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfg14;
}

void
McpttCallTypeMachineGrpBasic::SetDowngradeCb (const Callback<void>  downgradeCb)
{
  NS_LOG_FUNCTION (this << &downgradeCb);

  m_downgradeCb = downgradeCb;
}

void
McpttCallTypeMachineGrpBasic::SetCallType (const McpttCallMsgFieldCallType& callType)
{
  NS_LOG_FUNCTION (this << callType);

  m_callType = callType;
}

void
McpttCallTypeMachineGrpBasic::SetCfg11 (Ptr<McpttCounter>  cfg11)
{
  NS_LOG_FUNCTION (this << &cfg11);

  m_cfg11 = cfg11;
}

void
McpttCallTypeMachineGrpBasic::SetCfg12 (Ptr<McpttCounter>  cfg12)
{
  NS_LOG_FUNCTION (this << &cfg12);

  m_cfg12 = cfg12;
}

void
McpttCallTypeMachineGrpBasic::SetLastChgTime (const McpttCallMsgFieldLastChgTime& lastChgTime)
{
  NS_LOG_FUNCTION (this << lastChgTime);

  m_lastChgTime = lastChgTime;
}

void
McpttCallTypeMachineGrpBasic::SetLastChgUserId (const McpttCallMsgFieldUserId& lastChgUserId)
{
  NS_LOG_FUNCTION (this << lastChgUserId);

  m_lastChgUserId = lastChgUserId;
}

void
McpttCallTypeMachineGrpBasic::SetOwner (Ptr<McpttCallMachineGrpBasic> owner)
{
  NS_LOG_FUNCTION (this << owner);

  m_owner = owner;
}

void
McpttCallTypeMachineGrpBasic::SetPriority (uint8_t priority)
{
  NS_LOG_FUNCTION (this << (uint32_t)priority);

  m_priority = priority;
}

void
McpttCallTypeMachineGrpBasic::SetStarted (const bool& started)
{
  NS_LOG_FUNCTION (this << started);

  m_started = started;
}

void
McpttCallTypeMachineGrpBasic::SetStateId (const McpttEntityId& stateId)
{
  NS_LOG_FUNCTION (this << stateId);

  m_stateId = stateId;
}

void
McpttCallTypeMachineGrpBasic::SetTfg11 (Ptr<McpttTimer>  tfg11)
{
  NS_LOG_FUNCTION (this << &tfg11);

  m_tfg11 = tfg11;
}

void
McpttCallTypeMachineGrpBasic::SetTfg12 (Ptr<McpttTimer>  tfg12)
{
  NS_LOG_FUNCTION (this << &tfg12);

  m_tfg12 = tfg12;
}

void
McpttCallTypeMachineGrpBasic::SetTfg13 (Ptr<McpttTimer>  tfg13)
{
  NS_LOG_FUNCTION (this << &tfg13);

  m_tfg13 = tfg13;
}

void
McpttCallTypeMachineGrpBasic::SetTfg14 (Ptr<McpttTimer>  tfg14)
{
  NS_LOG_FUNCTION (this << &tfg14);

  m_tfg14 = tfg14;
}

void
McpttCallTypeMachineGrpBasic::SetUpgradeCb (const Callback<void, uint8_t>  upgradeCb)
{
  NS_LOG_FUNCTION (this << &upgradeCb);

  m_upgradeCb = upgradeCb;
}

} // namespace psc
} // namespace ns3

