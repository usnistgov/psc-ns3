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

#include "mcptt-ptt-app.h"
#include "mcptt-call.h"
#include "mcptt-floor-machine.h"

#include "mcptt-call-machine.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("McpttCallMachine");

/** McpttCallMachine - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMachine);

TypeId
McpttCallMachine::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::McpttCallMachine")
    .SetParent<Object> ()
  ;

  return tid;
}

McpttCallMachine::McpttCallMachine (void)
  : Object ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachine::~McpttCallMachine (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttCallMachine::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachine::GetTypeId ();
}
/** McpttCallMachine - end **/

/** McpttCallMachineGrp - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMachineGrp);

TypeId
McpttCallMachineGrp::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::McpttCallMachineGrp")
    .SetParent<McpttCallMachine> ()
    .AddAttribute ("GroupId",
                   "The group ID of the call.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&McpttCallMachineGrp::SetGrpId),
                   MakeUintegerChecker<uint32_t> ())
  ;

  return tid;
}

McpttCallMachineGrp::McpttCallMachineGrp (void)
  : McpttCallMachine ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachineGrp::~McpttCallMachineGrp (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttCallMachineGrp::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachineGrp::GetTypeId ();
}

bool
McpttCallMachineGrp::IsGrpCall (uint32_t grpId) const
{
  NS_LOG_FUNCTION (this << grpId);

  bool isGrpCall = true;
  McpttCallMsgFieldGrpId myGrpIdField = GetGrpId ();
  uint32_t myGrpId = myGrpIdField.GetGrpId ();

  if (grpId > 0)
    {
      isGrpCall = (myGrpId == grpId);
    }

  return isGrpCall;
}

bool
McpttCallMachineGrp::IsPrivateCall (uint32_t userId) const
{
  return false;
}
/** McpttCallMachineGrp - end **/

/** McpttCallMachineNull - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMachineNull);

McpttEntityId
McpttCallMachineNull::GetNullStateId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static McpttEntityId stateId = McpttEntityId (0, "Null");

  return stateId;
}

TypeId
McpttCallMachineNull::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::McpttCallMachineNull")
    .SetParent<McpttCallMachine> ()
    .AddConstructor<McpttCallMachineNull> ()
    .AddAttribute ("FloorPort", "The port to use for floor control messages.",
                   UintegerValue (49150),
                   MakeUintegerAccessor (&McpttCallMachineNull::m_floorPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("MediaPort", "The port to use for media messages.",
                   UintegerValue (49151),
                   MakeUintegerAccessor (&McpttCallMachineNull::m_mediaPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("CallType", "The call type to use for this call machine.",
                   UintegerValue (McpttCallMsgFieldCallType::BASIC_GROUP),
                   MakeUintegerAccessor (&McpttCallMachineNull::m_callType),
                   MakeUintegerChecker<uint8_t> ())
   ;

  return tid;
}

McpttCallMachineNull::McpttCallMachineNull (void)
  : McpttCallMachine (),
    m_owner (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMachineNull::~McpttCallMachineNull (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineNull::AcceptCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineNull::BeginEmergAlert (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineNull::CancelEmergAlert (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineNull::DowngradeCallType (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttCallMachineNull::GetCallerUserId (void) const
{
  NS_LOG_FUNCTION (this);

  return 0;
}

McpttCallMsgFieldCallId
McpttCallMachineNull::GetCallId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMsgFieldCallId ();
}

McpttCallMsgFieldCallType
McpttCallMachineNull::GetCallType (void) const
{
  McpttCallMsgFieldCallType callTypeField;
  callTypeField.SetType (m_callType);

  return callTypeField;
}

TypeId
McpttCallMachineNull::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachineNull::GetTypeId ();
}

McpttCall*
McpttCallMachineNull::GetOwner (void) const
{
  NS_LOG_FUNCTION (this);

  return m_owner;
}

McpttEntityId
McpttCallMachineNull::GetStateId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMachineNull::GetNullStateId ();
}

void
McpttCallMachineNull::InitiateCall (void)
{
  NS_LOG_FUNCTION (this);
}

bool
McpttCallMachineNull::IsCallOngoing (void) const
{
  NS_LOG_FUNCTION (this);

  return true;
}

bool
McpttCallMachineNull::IsGrpCall (uint32_t grpId) const
{
  NS_LOG_FUNCTION (this << grpId);

  return false;
}

bool
McpttCallMachineNull::IsPrivateCall (uint32_t userId) const
{
  NS_LOG_FUNCTION (this << userId);

  return false;
}

void
McpttCallMachineNull::Receive (const McpttCallMsg& msg) 
{
  NS_LOG_FUNCTION (this << &msg);
}

void
McpttCallMachineNull::Receive (const McpttMediaMsg& msg) 
{
  NS_LOG_FUNCTION (this << &msg);
}

void
McpttCallMachineNull::ReleaseCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineNull::RejectCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCallMachineNull::Send (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);
}

void
McpttCallMachineNull::SetNewCallCb (const Callback<void, uint16_t>  newCallCb)
{
  NS_LOG_FUNCTION (this << &newCallCb);
}

void
McpttCallMachineNull::SetOwner (McpttCall* const& owner)
{
  NS_LOG_FUNCTION (this << owner);

  m_owner = owner;
}

void
McpttCallMachineNull::Start (void)
{
  NS_LOG_FUNCTION (this);

  McpttCall* call = GetOwner ();
  McpttPttApp* pttApp = call->GetOwner ();
  Ipv4AddressValue grpAddr;

  pttApp->GetAttribute ("PeerAddress", grpAddr);

  Ptr<McpttFloorMachine> floorMachine = call->GetFloorMachine ();

  call->OpenFloorChan (grpAddr.Get (), m_floorPort);
  call->OpenMediaChan (grpAddr.Get (), m_mediaPort);

  floorMachine->Start ();
}

void
McpttCallMachineNull::Stop (void)
{
  NS_LOG_FUNCTION (this);

  McpttCall* call = GetOwner ();

  Ptr<McpttFloorMachine> floorMachine = call->GetFloorMachine ();

  call->CloseFloorChan ();
  call->CloseMediaChan ();

  floorMachine->Stop ();
}

void
McpttCallMachineNull::UpgradeCallType (uint8_t callType)
{
  NS_LOG_FUNCTION (this << (uint32_t)callType);
  m_callType = callType;
}

void
McpttCallMachineNull::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetOwner (0);

  McpttCallMachine::DoDispose ();
}
/** McpttCallMachineNull - end **/

} // namespace ns3

