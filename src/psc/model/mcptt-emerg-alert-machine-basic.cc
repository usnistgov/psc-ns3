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

#include "mcptt-call-machine-grp-basic.h"
#include "mcptt-call-msg.h"
#include "mcptt-counter.h"
#include "mcptt-emerg-alert-machine.h"
#include "mcptt-ptt-app.h"
#include "mcptt-timer.h"

#include "mcptt-emerg-alert-machine-basic.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttEmergAlertMachineBasic");

namespace psc {

/** McpttEmergAlertMachineBasic - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttEmergAlertMachineBasic);

McpttEmergAlertMachineBasic::EmergUser::~EmergUser (void)
{
  NS_LOG_FUNCTION (this);

  Id = 0;
  Loc = Vector ();
  Tfe1 = 0;
}


TypeId
McpttEmergAlertMachineBasic::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttEmergAlertMachineBasic")
    .SetParent<McpttEmergAlertMachine> ()
    .AddConstructor<McpttEmergAlertMachineBasic> ()
    .AddAttribute ("TFE1", "The initial delay to use for timer TFE1 (Time value)",
                   TimeValue (Seconds (30)),
                   MakeTimeAccessor (&McpttEmergAlertMachineBasic::m_delayTfe1),
                   MakeTimeChecker ())
    .AddAttribute ("TFE2", "The initial delay to use for timer TFE2 (Time value)",
                   TimeValue (Seconds (5)),
                   MakeTimeAccessor (&McpttEmergAlertMachineBasic::SetDelayTfe2),
                   MakeTimeChecker ())
    .AddTraceSource ("StateChangeTrace", "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttEmergAlertMachineBasic::m_stateChangeTrace),
                     "ns3::psc::McpttCallMachine::StateChangeTracedCallback")
  ;

  return tid;
}

McpttEmergAlertMachineBasic::McpttEmergAlertMachineBasic (Ptr<McpttCallMachineGrp> owner)
  : McpttEmergAlertMachine (),
    m_emerg (false),
    m_emergUsers (std::list<McpttEmergAlertMachineBasic::EmergUser> ()),
    m_orgName (McpttCallMsgFieldOrgName ()),
    m_owner (owner),
    m_started (false),
    m_stateChangeCb (MakeNullCallback<void, bool> ()),
    m_tfe2 (CreateObject<McpttTimer> (McpttEntityId (2, "TFE2")))
{
  NS_LOG_FUNCTION (this);

  m_tfe2->Link (&McpttEmergAlertMachineBasic::ExpiryOfTfe2, this);
}

McpttEmergAlertMachineBasic::~McpttEmergAlertMachineBasic (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttEmergAlertMachineBasic::CancelEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }
  std::string selected = "False";
  if (GetOwner ()->GetCall ()->GetCallId () == GetOwner ()->GetCall ()->GetOwner ()->GetSelectedCall ()->GetCallId ())
    {
      selected = "True";
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " canceling emergency alert.");

  Ptr<McpttTimer> tfe2 = GetTfe2 ();
  McpttCallMsgFieldGrpId grpId = GetOwner ()->GetGrpId ();
  uint32_t myId = GetOwner ()->GetCall ()->GetOwner ()->GetUserId ();

  if (IsInEmergState ())
    {
      SetEmerg (false);

      McpttCallMsgGrpEmergAlertCancel msg;
      msg.SetGrpId (grpId);
      msg.SetOrigId (myId);
      msg.SetSendingId (myId);

      tfe2->Stop ();

      Send (msg);

      if (!m_stateChangeCb.IsNull ())
        {
          m_stateChangeCb (false);
        }
      m_stateChangeTrace (GetOwner ()->GetCall ()->GetOwner ()->GetUserId (), GetOwner ()->GetCall ()->GetCallId (), selected, GetInstanceTypeId ().GetName (), "'E2: Emergency State'", "'E1: Not in emergency state'");
    }
}

TypeId
McpttEmergAlertMachineBasic::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttEmergAlertMachineBasic::GetTypeId ();
}

bool
McpttEmergAlertMachineBasic::IsInEmergState (void) const
{
  NS_LOG_FUNCTION (this);

  bool isInEmergState = false;

  if (IsStarted ())
    {
      isInEmergState = GetEmerg ();
    }

  // NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " is" << (isInEmergState ? " " : " not ") << "in emergency state.");

  return isInEmergState;
}

bool
McpttEmergAlertMachineBasic::IsStarted (void) const
{
  NS_LOG_FUNCTION (this);

  bool isStarted = GetStarted ();

  return isStarted;
}

bool
McpttEmergAlertMachineBasic::IsOtherInEmergState (void) const
{
  NS_LOG_FUNCTION (this);

  bool isOther = !m_emergUsers.empty ();

  return isOther;
}

void
McpttEmergAlertMachineBasic::ReceiveGrpCallEmergAlert (const McpttCallMsgGrpEmergAlert& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " received " << msg << ".");

  uint32_t theirId = msg.GetUserId ().GetId ();
  Vector theirLoc = msg.GetUserLoc ().GetLoc ();
  std::list<McpttEmergAlertMachineBasic::EmergUser>::iterator it;
  uint32_t myId = GetOwner ()->GetCall ()->GetOwner ()->GetUserId ();

  if (FindEmergUser (theirId, it))
    {
      it->Loc = theirLoc;
      it->Tfe1->Restart ();
    }
  else
    {
      McpttEmergAlertMachineBasic::EmergUser emergUser;
      emergUser.Id = msg.GetUserId ().GetId ();
      emergUser.Loc = msg.GetUserLoc ().GetLoc ();
      emergUser.Tfe1 = CreateObject<McpttTimer> (McpttEntityId (1, "TFE1"));
      emergUser.Tfe1->Link (&McpttEmergAlertMachineBasic::ExpiryOfTfe1, this);
      emergUser.Tfe1->SetDelay (m_delayTfe1);
      emergUser.Tfe1->SetArgument (theirId);
      emergUser.Tfe1->Start ();

      AddEmergUser (emergUser);

      McpttCallMsgGrpEmergAlertAck ackMsg;
      ackMsg.SetGrpId (msg.GetGrpId ());
      ackMsg.SetSendingId (myId);
      ackMsg.SetOrigId (msg.GetUserId ());

      Send (ackMsg);
    }
}

void
McpttEmergAlertMachineBasic::ReceiveGrpCallEmergAlertCancel (const McpttCallMsgGrpEmergAlertCancel& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " received " << msg << ".");

  uint32_t theirId = msg.GetSendingId ().GetId ();
  uint32_t myId = GetOwner ()->GetCall ()->GetOwner ()->GetUserId ();

  if (RemoveEmergUser (theirId))
    {
      McpttCallMsgGrpEmergAlertCancelAck ackMsg;
      ackMsg.SetGrpId (msg.GetGrpId ());
      ackMsg.SetSendingId (myId);
      ackMsg.SetOrigId (msg.GetOrigId ());

      Send (ackMsg);
    }
}

void
McpttEmergAlertMachineBasic::Send (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " trying to send " << msg << ".");

  Ptr<McpttCallMachineGrp> owner = GetOwner ();
  owner->Send (msg);
}

void
McpttEmergAlertMachineBasic::SendEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }
  std::string selected = "False";
  if (GetOwner ()->GetCall ()->GetCallId () == GetOwner ()->GetCall ()->GetOwner ()->GetSelectedCall ()->GetCallId ())
    {
      selected = "True";
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " sending emergency alert.");

  Ptr<McpttTimer> tfe2 = GetTfe2 ();
  Ptr<McpttCallMachineGrp> owner = GetOwner ();
  McpttCallMsgFieldGrpId grpId = owner->GetGrpId ();
  uint32_t userId = owner->GetCall ()->GetOwner ()->GetUserId ();
  McpttCallMsgFieldOrgName orgName = GetOrgName ();
  Vector nodeLoc = owner->GetCall ()->GetOwner ()->GetNodeLoc ();

  if (!IsInEmergState ())
    {
      SetEmerg (true);

      McpttCallMsgGrpEmergAlert msg;
      msg.SetGrpId (grpId);
      msg.SetUserId (userId);
      msg.SetOrgName (orgName);
      msg.SetUserLoc (nodeLoc);

      tfe2->Start ();

      Send (msg);

      if (!m_stateChangeCb.IsNull ())
        {
          m_stateChangeCb (true);
        }
      m_stateChangeTrace (GetOwner ()->GetCall ()->GetOwner ()->GetUserId (), GetOwner ()->GetCall ()->GetCallId (), selected, GetInstanceTypeId ().GetName (), "'E1: Not in emergency state'", "'E2: Emergency State'");
    }
}

void
McpttEmergAlertMachineBasic::SetDelayTfe2 (const Time& delayTfe2)
{
  NS_LOG_FUNCTION (this << &delayTfe2);

  GetTfe2 ()->SetDelay (delayTfe2);
}

void
McpttEmergAlertMachineBasic::Start (void)
{
  NS_LOG_FUNCTION (this);

  if (IsStarted ())
    {
      NS_LOG_LOGIC ("McpttEmergAlertMachineBasic has already been started.");
    }
  else
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " started.");

      SetEmerg (false);
      SetStarted (true);
    }
}

void
McpttEmergAlertMachineBasic::Stop (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " stopped.");

  Ptr<McpttTimer> tfe2 = GetTfe2 ();

  if (tfe2->IsRunning ())
    {
      tfe2->Stop ();
    }

  for (std::list<McpttEmergAlertMachineBasic::EmergUser>::iterator it = m_emergUsers.begin (); it != m_emergUsers.end (); it++)
    {
      if (it->Tfe1->IsRunning ())
        {
          it->Tfe1->Stop ();
          it->Tfe1 = 0;
        }
    }

  m_emergUsers.clear ();

  SetEmerg (false);
  SetStarted (false);
}

void
McpttEmergAlertMachineBasic::AddEmergUser (const McpttEmergAlertMachineBasic::EmergUser& user)
{
  NS_LOG_FUNCTION (this << &user);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " adding user " << user.Id << " to emergency alert list.");

  m_emergUsers.push_back (user);
}

void
McpttEmergAlertMachineBasic::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetOwner (0);
  SetTfe2 (0);
}

void
McpttEmergAlertMachineBasic::ExpiryOfTfe1 (uint32_t userId)
{
  NS_LOG_FUNCTION (this << userId);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  "'s TFE1 timer expired for user " << userId << " to emergency alert list.");

  RemoveEmergUser (userId);
}

void
McpttEmergAlertMachineBasic::ExpiryOfTfe2 (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsStarted ())
    {
      NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " not started yet.");
      return;
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  "'s TFE2 timer expired.");

  Ptr<McpttTimer> tfe2 = GetTfe2 ();
  Ptr<McpttCallMachineGrp> owner = GetOwner ();
  McpttCallMsgFieldGrpId grpId = owner->GetGrpId ();
  uint32_t userId = owner->GetCall ()->GetOwner ()->GetUserId ();
  McpttCallMsgFieldOrgName orgName = GetOrgName ();
  Vector nodeLoc = owner->GetCall ()->GetOwner ()->GetNodeLoc ();

  if (IsInEmergState ())
    {
      McpttCallMsgGrpEmergAlert msg;
      msg.SetGrpId (grpId);
      msg.SetUserId (userId);
      msg.SetOrgName (orgName);
      msg.SetUserLoc (nodeLoc);

      tfe2->Start ();

      Send (msg);
    }
}

bool
McpttEmergAlertMachineBasic::FindEmergUser (uint32_t userId, std::list<McpttEmergAlertMachineBasic::EmergUser>::iterator& it)
{
  NS_LOG_FUNCTION (this << userId);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " looking for user " << userId << " in emergency list.");

  bool found = false;
  it = m_emergUsers.begin ();
  while (it != m_emergUsers.end () && !found)
    {
      if (it->Id == userId)
        {
          found = true;
        }
      else
        {
          it++;
        }
    }

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " did" << (found ? " " : " NOT ") << "find user " << userId << " in emergency list.");

  return found;
}

bool
McpttEmergAlertMachineBasic::RemoveEmergUser (uint32_t userId)
{
  NS_LOG_FUNCTION (this << userId);

  NS_LOG_LOGIC (GetInstanceTypeId ().GetName () <<  " removing user " << userId << " from emergency list.");

  std::list<McpttEmergAlertMachineBasic::EmergUser>::iterator it;
  bool found = FindEmergUser (userId, it);

  if (found == true)
    {
      if (it->Tfe1->IsRunning ())
        {
          it->Tfe1->Stop ();
        }

      m_emergUsers.erase (it);
    }

  return found;
}

std::list<McpttEmergAlertMachineBasic::EmergUser>
McpttEmergAlertMachineBasic::GetEmergUsers (void) const
{
  NS_LOG_FUNCTION (this);

  return m_emergUsers;
}

Ptr<McpttTimer>
McpttEmergAlertMachineBasic::GetTfe2 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_tfe2;
}

void
McpttEmergAlertMachineBasic::SetEmergUsers (const std::list<McpttEmergAlertMachineBasic::EmergUser>& emergUsers)
{
  NS_LOG_FUNCTION (this << &emergUsers);

  m_emergUsers = emergUsers;
}

void
McpttEmergAlertMachineBasic::SetTfe2 (Ptr<McpttTimer>  tfe2)
{
  NS_LOG_FUNCTION (this << tfe2);

  m_tfe2 = tfe2;
}

bool
McpttEmergAlertMachineBasic::GetEmerg (void) const
{
  NS_LOG_FUNCTION (this);

  return m_emerg;
}

McpttCallMsgFieldOrgName
McpttEmergAlertMachineBasic::GetOrgName (void) const
{
  NS_LOG_FUNCTION (this);

  return m_orgName;
}

Ptr<McpttCallMachineGrp>
McpttEmergAlertMachineBasic::GetOwner (void) const
{
  NS_LOG_FUNCTION (this);

  return m_owner;
}

bool
McpttEmergAlertMachineBasic::GetStarted (void) const
{
  NS_LOG_FUNCTION (this);

  return m_started;
}

void
McpttEmergAlertMachineBasic::SetEmerg (bool emerg)
{
  NS_LOG_FUNCTION (this << emerg);

  m_emerg = emerg;
}

void
McpttEmergAlertMachineBasic::SetOrgName (const McpttCallMsgFieldOrgName& orgName)
{
  NS_LOG_FUNCTION (this << orgName);

  m_orgName = orgName;
}

void
McpttEmergAlertMachineBasic::SetOwner (Ptr<McpttCallMachineGrp> owner)
{
  NS_LOG_FUNCTION (this << owner);

  m_owner = owner;
}

void
McpttEmergAlertMachineBasic::SetStarted (bool started)
{
  NS_LOG_FUNCTION (this << started);

  m_started = started;
}

void
McpttEmergAlertMachineBasic::SetStateChangeCb (const Callback<void, bool>  stateChangeCb)
{
  NS_LOG_FUNCTION (this << &stateChangeCb);

  m_stateChangeCb = stateChangeCb;
}
/** McpttEmergAlertMachineBasic - end **/

} // namespace psc
} // namespace ns3

