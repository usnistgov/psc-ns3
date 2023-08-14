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
#include <ns3/uinteger.h>
#include <ns3/sip-header.h>

#include "mcptt-server-app.h"
#include "mcptt-server-call.h"
#include "mcptt-on-network-floor-arbitrator.h"

#include "mcptt-server-call-machine.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttServerCallMachine");

namespace psc {

/** McpttServerCallMachine - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttServerCallMachine);

TypeId
McpttServerCallMachine::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttServerCallMachine")
    .SetParent<Object> ()
  ;

  return tid;
}

McpttServerCallMachine::McpttServerCallMachine (void)
  : Object ()
{
  NS_LOG_FUNCTION (this);
}

McpttServerCallMachine::~McpttServerCallMachine (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttServerCallMachine::GetInstanceTypeId (void) const
{
  return McpttServerCallMachine::GetTypeId ();
}
/** McpttServerCallMachine - end **/

/** McpttServerCallMachineGrp - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttServerCallMachineGrp);

TypeId
McpttServerCallMachineGrp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttServerCallMachineGrp")
    .SetParent<McpttServerCallMachine> ()
    .AddAttribute ("GroupId",
                   "The group ID of the call.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&McpttServerCallMachineGrp::SetGrpId),
                   MakeUintegerChecker<uint32_t> ())
  ;

  return tid;
}

McpttServerCallMachineGrp::McpttServerCallMachineGrp (void)
  : McpttServerCallMachine ()
{
  NS_LOG_FUNCTION (this);
}

McpttServerCallMachineGrp::~McpttServerCallMachineGrp (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttServerCallMachineGrp::GetInstanceTypeId (void) const
{
  return McpttServerCallMachineGrp::GetTypeId ();
}

bool
McpttServerCallMachineGrp::IsGrpCall (uint32_t grpId) const
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
McpttServerCallMachineGrp::IsPrivateCall (uint32_t userId) const
{
  return false;
}
/** McpttServerCallMachineGrp - end **/

/** McpttServerCallMachineNull - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttServerCallMachineNull);

McpttEntityId
McpttServerCallMachineNull::GetNullStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (0, "Null");

  return stateId;
}

TypeId
McpttServerCallMachineNull::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttServerCallMachineNull")
    .SetParent<McpttServerCallMachine> ()
    .AddConstructor<McpttServerCallMachineNull> ()
    .AddAttribute ("FloorPort", "The port to use for floor control messages.",
                   UintegerValue (49150),
                   MakeUintegerAccessor (&McpttServerCallMachineNull::m_floorPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("MediaPort", "The port to use for media messages.",
                   UintegerValue (49151),
                   MakeUintegerAccessor (&McpttServerCallMachineNull::m_mediaPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("CallType", "The call type to use for this call machine.",
                   UintegerValue (McpttCallMsgFieldCallType::BASIC_GROUP),
                   MakeUintegerAccessor (&McpttServerCallMachineNull::m_callType),
                   MakeUintegerChecker<uint8_t> ())
  ;

  return tid;
}

McpttServerCallMachineNull::McpttServerCallMachineNull (void)
  : McpttServerCallMachine (),
    m_serverCall (0),
    m_userId (0)
{
  NS_LOG_FUNCTION (this);
}

McpttServerCallMachineNull::~McpttServerCallMachineNull (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttServerCallMachineNull::AcceptCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttServerCallMachineNull::BeginEmergAlert (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttServerCallMachineNull::CancelEmergAlert (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttServerCallMachineNull::DowngradeCallType (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttServerCallMachineNull::GetCallerUserId (void) const
{
  return 0;
}

McpttCallMsgFieldCallId
McpttServerCallMachineNull::GetCallId (void) const
{
  return McpttCallMsgFieldCallId ();
}

McpttCallMsgFieldCallType
McpttServerCallMachineNull::GetCallType (void) const
{
  McpttCallMsgFieldCallType callTypeField;
  callTypeField.SetType (m_callType);

  return callTypeField;
}

void
McpttServerCallMachineNull::SetUserId (uint32_t userId)
{
  m_userId = userId;
}

uint32_t
McpttServerCallMachineNull::GetUserId (void) const
{
  return m_userId;
}

TypeId
McpttServerCallMachineNull::GetInstanceTypeId (void) const
{
  return McpttServerCallMachineNull::GetTypeId ();
}

Ptr<McpttServerCall>
McpttServerCallMachineNull::GetServerCall (void) const
{
  return m_serverCall;
}

McpttEntityId
McpttServerCallMachineNull::GetStateId (void) const
{
  return McpttServerCallMachineNull::GetNullStateId ();
}

bool
McpttServerCallMachineNull::IsCallOngoing (void) const
{
  NS_LOG_FUNCTION (this);

  return true;
}

bool
McpttServerCallMachineNull::IsGrpCall (uint32_t grpId) const
{
  NS_LOG_FUNCTION (this << grpId);

  return false;
}

bool
McpttServerCallMachineNull::IsPrivateCall (uint32_t userId) const
{
  NS_LOG_FUNCTION (this << userId);

  return false;
}

void
McpttServerCallMachineNull::ReceiveCallPacket (Ptr<Packet> pkt, const sip::SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << pkt << hdr);
}

void
McpttServerCallMachineNull::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);
}

void
McpttServerCallMachineNull::RejectCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttServerCallMachineNull::SetNewCallCb (const Callback<void, uint16_t>  newCallCb)
{
  NS_LOG_FUNCTION (this << &newCallCb);
}

void
McpttServerCallMachineNull::SetServerCall (Ptr<McpttServerCall> call)
{
  NS_LOG_FUNCTION (this << call);

  m_serverCall = call;
}

void
McpttServerCallMachineNull::Start (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttServerCall> call = GetServerCall ();
  Ptr<McpttServerApp> serverApp = call->GetOwner ();

  Ptr<McpttOnNetworkFloorArbitrator> arbitrator = call->GetArbitrator ();
  arbitrator->Start ();
}

void
McpttServerCallMachineNull::Stop (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttServerCall> call = GetServerCall ();

  Ptr<McpttOnNetworkFloorArbitrator> arbitrator = call->GetArbitrator ();

  arbitrator->Stop ();
}

void
McpttServerCallMachineNull::UpgradeCallType (uint8_t callType)
{
  NS_LOG_FUNCTION (this << (uint32_t)callType);
  m_callType = callType;
}

void
McpttServerCallMachineNull::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_serverCall = 0;

  McpttServerCallMachine::DoDispose ();
}
/** McpttServerCallMachineNull - end **/

} // namespace psc
} // namespace ns3

