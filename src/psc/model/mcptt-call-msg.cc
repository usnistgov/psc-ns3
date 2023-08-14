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

#include <iostream>

#include <ns3/assert.h>
#include <ns3/buffer.h>
#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/type-id.h>

#include "mcptt-call-machine-grp-basic.h"
#include "mcptt-call-machine-grp-broadcast.h"
#include "mcptt-call-machine-private.h"
#include "mcptt-call-msg-field.h"
#include "mcptt-msg.h"

#include "mcptt-call-msg.h"

namespace ns3 {

namespace psc {

/** McpttCallMsg - begin **/
NS_LOG_COMPONENT_DEFINE ("McpttCallMsg");

NS_OBJECT_ENSURE_REGISTERED (McpttCallMsg);

TypeId
McpttCallMsg::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsg")
    .SetParent<McpttMsg> ()
  ;

  return tid;
}

McpttCallMsg::McpttCallMsg (void)
  : McpttMsg (),
    m_msgType (McpttCallMsgFieldMsgType ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsg::McpttCallMsg (const McpttCallMsgFieldMsgType& msgType)
  : McpttMsg (),
    m_msgType (msgType)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsg::~McpttCallMsg (void)
{}

uint32_t
McpttCallMsg::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this << &start);

  uint32_t bytesRead = 0;
  McpttCallMsgFieldMsgType msgType = GetMsgType ();

  bytesRead += msgType.Deserialize (start);
  SetMsgType (msgType);

  bytesRead += ReadData (start);

  return bytesRead;
}

TypeId
McpttCallMsg::GetInstanceTypeId (void) const
{
  return McpttCallMsg::GetTypeId ();
}

uint32_t
McpttCallMsg::GetSerializedSize (void) const
{
  uint32_t serializedSize = 0;
  McpttCallMsgFieldMsgType msgType = GetMsgType ();

  serializedSize += msgType.GetSerializedSize ();

  return serializedSize;
}

void
McpttCallMsg::Print (std::ostream& os) const
{
  McpttCallMsgFieldMsgType msgType = GetMsgType ();

  os << "McpttCallMsg(";
  os << "msgType=" << msgType;
  os << ")";
}

void
McpttCallMsg::Visit (McpttCallMachineGrpBasic& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);
}

void
McpttCallMsg::Visit (McpttCallMachineGrpBroadcast& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);
}

void
McpttCallMsg::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);
}

void
McpttCallMsg::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this << &start);

  McpttCallMsgFieldMsgType msgType = GetMsgType ();

  msgType.Serialize (start);

  WriteData (start);
}

uint32_t
McpttCallMsg::ReadData (Buffer::Iterator& start)
{
  NS_LOG_FUNCTION (this << &start);

  return 0;
}

void
McpttCallMsg::WriteData (Buffer::Iterator& start) const
{
  NS_LOG_FUNCTION (this << &start);
}

McpttCallMsgFieldMsgType
McpttCallMsg::GetMsgType (void) const
{
  return m_msgType;
}

void
McpttCallMsg::SetMsgType (const McpttCallMsgFieldMsgType& msgType)
{
  NS_LOG_FUNCTION (this << msgType);

  m_msgType = msgType;
}
/** McpttCallMsg - end **/

/** McpttCallMsgGrpProbe - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgGrpProbe);

const uint8_t McpttCallMsgGrpProbe::CODE = 1;

TypeId
McpttCallMsgGrpProbe::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgGrpProbe")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgGrpProbe> ()
  ;

  return tid;
}

McpttCallMsgGrpProbe::McpttCallMsgGrpProbe (void)
  : McpttCallMsg (McpttCallMsgGrpProbe::CODE),
    m_grpId (McpttCallMsgFieldGrpId  ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgGrpProbe::~McpttCallMsgGrpProbe (void)
{}

TypeId
McpttCallMsgGrpProbe::GetInstanceTypeId (void) const
{
  return McpttCallMsgGrpProbe::GetTypeId ();
}

uint32_t
McpttCallMsgGrpProbe::GetSerializedSize (void) const
{
  McpttCallMsgFieldGrpId grpId = GetGrpId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();
  size += grpId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgGrpProbe::Print (std::ostream& os) const
{
  McpttCallMsgFieldGrpId grpId = GetGrpId ();

  os << "McpttCallMsgGrpProbe(";

  McpttCallMsg::Print (os);

  os << ";grpId=" << grpId;
  os << ")";
}

void
McpttCallMsgGrpProbe::Visit (McpttCallMachineGrpBasic& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallProbe (*this);
}

uint32_t
McpttCallMsgGrpProbe::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += grpId.Deserialize (buff);

  SetGrpId (grpId);

  return bytesRead;
}

void
McpttCallMsgGrpProbe::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();

  McpttCallMsg::WriteData (buff);

  grpId.Serialize (buff);
}

McpttCallMsgFieldGrpId
McpttCallMsgGrpProbe::GetGrpId (void) const
{
  return m_grpId;
}

void
McpttCallMsgGrpProbe::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}
/** McpttCallMsgGrpProbe - end **/

/** McpttCallMsgGrpAnnoun - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgGrpAnnoun);

const uint8_t McpttCallMsgGrpAnnoun::CODE = 2;

TypeId
McpttCallMsgGrpAnnoun::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgGrpAnnoun")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgGrpAnnoun> ()
  ;

  return tid;
}

McpttCallMsgGrpAnnoun::McpttCallMsgGrpAnnoun (void)
  : McpttCallMsg (McpttCallMsgGrpAnnoun::CODE),
    m_callId (McpttCallMsgFieldCallId ()),
    m_callType (McpttCallMsgFieldCallType ()),
    m_confirmMode (false),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_lastChgTime (McpttCallMsgFieldLastChgTime ()),
    m_lastChgUserId (McpttCallMsgFieldUserId ()),
    m_origUserId (McpttCallMsgFieldUserId ()),
    m_probeResp (false),
    m_refInt (McpttCallMsgFieldRefreshInterval ()),
    m_sdp (McpttCallMsgFieldSdp ()),
    m_startTime (McpttCallMsgFieldStartTime ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgGrpAnnoun::~McpttCallMsgGrpAnnoun (void)
{}

TypeId
McpttCallMsgGrpAnnoun::GetInstanceTypeId (void) const
{
  return McpttCallMsgGrpAnnoun::GetTypeId ();
}

uint32_t
McpttCallMsgGrpAnnoun::GetSerializedSize (void) const
{
  McpttCallMsgFieldProbeResp probeRespField;
  McpttCallMsgFieldConfirmMode confirmModeField;

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  bool confirmMode = GetConfirmMode ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId origUserId = GetOrigId ();
  bool probeResp = GetProbeResp ();
  McpttCallMsgFieldRefreshInterval refInt = GetRefInt ();
  McpttCallMsgFieldSdp sdp = GetSdp ();
  McpttCallMsgFieldStartTime startTime = GetStartTime ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();
  size += callId.GetSerializedSize ();
  size += callType.GetSerializedSize ();
  size += grpId.GetSerializedSize ();
  size += lastChgTime.GetSerializedSize ();
  size += lastChgUserId.GetSerializedSize ();
  size += origUserId.GetSerializedSize ();
  size += refInt.GetSerializedSize ();
  size += sdp.GetSerializedSize ();
  size += startTime.GetSerializedSize ();

  if (confirmMode == true)
    {
      size += confirmModeField.GetSerializedSize ();
    }

  if (probeResp == true)
    {
      size += probeRespField.GetSerializedSize ();
    }

  return size;
}

bool
McpttCallMsgGrpAnnoun::IsConfirmModeIndicated (void) const
{
  NS_LOG_FUNCTION (this);

  bool isIndicated = GetConfirmMode ();

  return isIndicated;
}

bool
McpttCallMsgGrpAnnoun::IsProbeResp (void) const
{
  NS_LOG_FUNCTION (this);

  bool isProbeResp = GetProbeResp ();

  return isProbeResp;
}

void
McpttCallMsgGrpAnnoun::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  bool confirmMode = GetConfirmMode ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId origUserId = GetOrigId ();
  bool probeResp = GetProbeResp ();
  McpttCallMsgFieldRefreshInterval refInt = GetRefInt ();
  McpttCallMsgFieldSdp sdp = GetSdp ();
  McpttCallMsgFieldStartTime startTime = GetStartTime ();

  os << "McpttCallMsgGrpAnnoun(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";callType=" << callType;
  os << ";confirmMode=" << confirmMode;
  os << ";grpId=" << grpId;
  os << ";lastChgTime=" << lastChgTime;
  os << ";lastChgUserId=" << lastChgUserId;
  os << ";origUserId=" << origUserId;
  os << ";probeResp=" << probeResp;
  os << ";refInt=" << refInt;
  os << ";sdp=" << sdp;
  os << ";startTime=" << startTime;
  os << ")";
}

void
McpttCallMsgGrpAnnoun::Visit (McpttCallMachineGrpBasic& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallAnnoun (*this);
}

uint32_t
McpttCallMsgGrpAnnoun::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldProbeResp probeRespField;
  McpttCallMsgFieldConfirmMode confirmModeField;

  bool probeResp = false;
  bool confirmMode = false;
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId origUserId = GetOrigId ();
  McpttCallMsgFieldRefreshInterval refInt = GetRefInt ();
  McpttCallMsgFieldSdp sdp = GetSdp ();
  McpttCallMsgFieldStartTime startTime = GetStartTime ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);
  bytesRead += callId.Deserialize (buff);
  bytesRead += callType.Deserialize (buff);
  bytesRead += refInt.Deserialize (buff);
  bytesRead += grpId.Deserialize (buff);
  bytesRead += sdp.Deserialize (buff);
  bytesRead += origUserId.Deserialize (buff);
  bytesRead += startTime.Deserialize (buff);
  bytesRead += lastChgTime.Deserialize (buff);
  bytesRead += lastChgUserId.Deserialize (buff);

  if (!buff.IsEnd ())
    {
      uint8_t nextByte = buff.PeekU8 ();
      if (nextByte == McpttCallMsgFieldConfirmMode::GetIei ())
        {
          confirmMode = true;
          bytesRead += confirmModeField.Deserialize (buff);
        }
    }

  if (!buff.IsEnd ())
    {
      uint8_t nextByte = buff.PeekU8 ();
      if (nextByte == McpttCallMsgFieldProbeResp::GetIei ())
        {
          probeResp = true;
          bytesRead += probeRespField.Deserialize (buff);
        }
    }

  SetCallId (callId);
  SetCallType (callType);
  SetGrpId (grpId);
  SetLastChgTime (lastChgTime);
  SetLastChgUserId (lastChgUserId);
  SetOrigId (origUserId);
  SetRefInt (refInt);
  SetSdp (sdp);
  SetStartTime (startTime);
  SetConfirmMode (confirmMode);
  SetProbeResp (probeResp);

  return bytesRead;
}

void
McpttCallMsgGrpAnnoun::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldProbeResp probeRespField;
  McpttCallMsgFieldConfirmMode confirmModeField;

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  bool confirmMode = GetConfirmMode ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId origUserId = GetOrigId ();
  bool probeResp = GetProbeResp ();
  McpttCallMsgFieldRefreshInterval refInt = GetRefInt ();
  McpttCallMsgFieldSdp sdp = GetSdp ();
  McpttCallMsgFieldStartTime startTime = GetStartTime ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  callType.Serialize (buff);
  refInt.Serialize (buff);
  grpId.Serialize (buff);
  sdp.Serialize (buff);
  origUserId.Serialize (buff);
  startTime.Serialize (buff);
  lastChgTime.Serialize (buff);
  lastChgUserId.Serialize (buff);

  if (confirmMode == true)
    {
      confirmModeField.Serialize (buff);
    }

  if (probeResp == true)
    {
      probeRespField.Serialize (buff);
    }
}

McpttCallMsgFieldCallId
McpttCallMsgGrpAnnoun::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldCallType
McpttCallMsgGrpAnnoun::GetCallType (void) const
{
  return m_callType;
}

bool
McpttCallMsgGrpAnnoun::GetConfirmMode (void) const
{
  return m_confirmMode;
}

McpttCallMsgFieldGrpId
McpttCallMsgGrpAnnoun::GetGrpId (void) const
{
  return m_grpId;
}

McpttCallMsgFieldLastChgTime
McpttCallMsgGrpAnnoun::GetLastChgTime (void) const
{
  return m_lastChgTime;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpAnnoun::GetLastChgUserId (void) const
{
  return m_lastChgUserId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpAnnoun::GetOrigId (void) const
{
  return m_origUserId;
}

bool
McpttCallMsgGrpAnnoun::GetProbeResp (void) const
{
  return m_probeResp;
}

McpttCallMsgFieldRefreshInterval
McpttCallMsgGrpAnnoun::GetRefInt (void) const
{
  return m_refInt;
}

McpttCallMsgFieldSdp
McpttCallMsgGrpAnnoun::GetSdp (void) const
{
  return m_sdp;
}

McpttCallMsgFieldStartTime
McpttCallMsgGrpAnnoun::GetStartTime (void) const
{
  return m_startTime;
}

void
McpttCallMsgGrpAnnoun::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgGrpAnnoun::SetCallType (const McpttCallMsgFieldCallType& callType)
{
  NS_LOG_FUNCTION (this << callType);

  m_callType = callType;
}

void
McpttCallMsgGrpAnnoun::SetConfirmMode (const bool& confirmMode)
{
  NS_LOG_FUNCTION (this << &confirmMode);

  m_confirmMode = confirmMode;
}

void
McpttCallMsgGrpAnnoun::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}

void
McpttCallMsgGrpAnnoun::SetLastChgTime (const McpttCallMsgFieldLastChgTime& lastChgTime)
{
  NS_LOG_FUNCTION (this << lastChgTime);

  m_lastChgTime = lastChgTime;
}

void
McpttCallMsgGrpAnnoun::SetLastChgUserId (const McpttCallMsgFieldUserId& lastChgUserId)
{
  NS_LOG_FUNCTION (this << lastChgUserId);

  m_lastChgUserId = lastChgUserId;
}

void
McpttCallMsgGrpAnnoun::SetOrigId (const McpttCallMsgFieldUserId& origUserId)
{
  NS_LOG_FUNCTION (this << origUserId);

  m_origUserId = origUserId;
}

void
McpttCallMsgGrpAnnoun::SetProbeResp (const bool& probeResp)
{
  NS_LOG_FUNCTION (this << probeResp);

  m_probeResp = probeResp;
}

void
McpttCallMsgGrpAnnoun::SetRefInt (const McpttCallMsgFieldRefreshInterval& refInt)
{
  NS_LOG_FUNCTION (this << refInt);

  m_refInt = refInt;
}

void
McpttCallMsgGrpAnnoun::SetSdp (const McpttCallMsgFieldSdp& sdp)
{
  NS_LOG_FUNCTION (this << sdp);

  m_sdp = sdp;
}

void
McpttCallMsgGrpAnnoun::SetStartTime (const McpttCallMsgFieldStartTime& startTime)
{
  NS_LOG_FUNCTION (this << startTime);

  m_startTime = startTime;
}
/** McpttCallMsgGrpAnnoun - end **/

/** McpttCallMsgGrpAccept - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgGrpAccept);

const uint8_t McpttCallMsgGrpAccept::CODE = 3;

TypeId
McpttCallMsgGrpAccept::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgGrpAccept")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgGrpAccept> ()
  ;

  return tid;
}

McpttCallMsgGrpAccept::McpttCallMsgGrpAccept (void)
  : McpttCallMsg (McpttCallMsgGrpAccept::CODE),
    m_callId (McpttCallMsgFieldCallId ()),
    m_callType (McpttCallMsgFieldCallType ()),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_userId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgGrpAccept::~McpttCallMsgGrpAccept (void)
{}

TypeId
McpttCallMsgGrpAccept::GetInstanceTypeId (void) const
{
  return McpttCallMsgGrpAccept::GetTypeId ();
}

uint32_t
McpttCallMsgGrpAccept::GetSerializedSize () const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();
  size += callId.GetSerializedSize ();
  size += callType.GetSerializedSize ();
  size += grpId.GetSerializedSize ();
  size += userId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgGrpAccept::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  os << "McpttCallMsgGrpAccept(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";callType=" << callType;
  os << ";grpId=" << grpId;
  os << ";userId=" << userId;
  os << ")";
}

void
McpttCallMsgGrpAccept::Visit (McpttCallMachineGrpBasic& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallAccept (*this);
}

uint32_t
McpttCallMsgGrpAccept::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);
  bytesRead += callId.Deserialize (buff);
  bytesRead += callType.Deserialize (buff);
  bytesRead += grpId.Deserialize (buff);
  bytesRead += userId.Deserialize (buff);

  SetCallId (callId);
  SetCallType (callType);
  SetGrpId (grpId);
  SetUserId (userId);

  return bytesRead;
}

void
McpttCallMsgGrpAccept::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  callType.Serialize (buff);
  grpId.Serialize (buff);
  userId.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgGrpAccept::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldCallType
McpttCallMsgGrpAccept::GetCallType (void) const
{
  return m_callType;
}

McpttCallMsgFieldGrpId
McpttCallMsgGrpAccept::GetGrpId (void) const
{
  return m_grpId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpAccept::GetUserId (void) const
{
  return m_userId;
}

void
McpttCallMsgGrpAccept::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgGrpAccept::SetCallType (const McpttCallMsgFieldCallType& callType)
{
  NS_LOG_FUNCTION (this << callType);

  m_callType = callType;
}

void
McpttCallMsgGrpAccept::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}

void
McpttCallMsgGrpAccept::SetUserId (const McpttCallMsgFieldUserId& userId)
{
  NS_LOG_FUNCTION (this << userId);

  m_userId = userId;
}
/** McpttCallMsgGrpAccept - end **/

/** McpttCallMsgGrpImmPerilEnd - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgGrpImmPerilEnd);

const uint8_t McpttCallMsgGrpImmPerilEnd::CODE = 5;

TypeId
McpttCallMsgGrpImmPerilEnd::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgGrpImmPerilEnd")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgGrpImmPerilEnd> ()
  ;

  return tid;
}

McpttCallMsgGrpImmPerilEnd::McpttCallMsgGrpImmPerilEnd (void)
  : McpttCallMsg (McpttCallMsgGrpImmPerilEnd::CODE),
    m_callId (McpttCallMsgFieldCallId ()),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_lastChgTime (McpttCallMsgFieldLastChgTime ()),
    m_lastChgUserId (McpttCallMsgFieldUserId ()),
    m_userId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgGrpImmPerilEnd::~McpttCallMsgGrpImmPerilEnd (void)
{}

TypeId
McpttCallMsgGrpImmPerilEnd::GetInstanceTypeId (void) const
{
  return McpttCallMsgGrpImmPerilEnd::GetTypeId ();
}

uint32_t
McpttCallMsgGrpImmPerilEnd::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += callId.GetSerializedSize ();
  size += grpId.GetSerializedSize ();
  size += lastChgTime.GetSerializedSize ();
  size += lastChgUserId.GetSerializedSize ();
  size += userId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgGrpImmPerilEnd::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  os << "McpttCallMsgGrpImmPerilEnd(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";grpId=" << grpId;
  os << ";userId=" << userId;
  os << ";lastChgTime=" << lastChgTime;
  os << ";lastChgUserId=" << lastChgUserId;
  os << ")";
}

void
McpttCallMsgGrpImmPerilEnd::Visit (McpttCallMachineGrpBasic& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallImmPerilEnd (*this);
}

uint32_t
McpttCallMsgGrpImmPerilEnd::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += callId.Deserialize (buff);
  bytesRead += grpId.Deserialize (buff);
  bytesRead += lastChgTime.Deserialize (buff);
  bytesRead += lastChgUserId.Deserialize (buff);
  bytesRead += userId.Deserialize (buff);

  SetCallId (callId);
  SetGrpId (grpId);
  SetLastChgTime (lastChgTime);
  SetLastChgUserId (lastChgUserId);
  SetUserId (userId);

  return bytesRead;
}

void
McpttCallMsgGrpImmPerilEnd::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  grpId.Serialize (buff);
  lastChgTime.Serialize (buff);
  lastChgUserId.Serialize (buff);
  userId.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgGrpImmPerilEnd::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldGrpId
McpttCallMsgGrpImmPerilEnd::GetGrpId (void) const
{
  return m_grpId;
}

McpttCallMsgFieldLastChgTime
McpttCallMsgGrpImmPerilEnd::GetLastChgTime (void) const
{
  return m_lastChgTime;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpImmPerilEnd::GetLastChgUserId (void) const
{
  return m_lastChgUserId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpImmPerilEnd::GetUserId (void) const
{
  return m_userId;
}

void
McpttCallMsgGrpImmPerilEnd::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgGrpImmPerilEnd::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}

void
McpttCallMsgGrpImmPerilEnd::SetLastChgTime (const McpttCallMsgFieldLastChgTime& lastChgTime)
{
  NS_LOG_FUNCTION (this << lastChgTime);

  m_lastChgTime = lastChgTime;
}


void
McpttCallMsgGrpImmPerilEnd::SetLastChgUserId (const McpttCallMsgFieldUserId& lastChgUserId)
{
  NS_LOG_FUNCTION (this << lastChgUserId);

  m_lastChgUserId = lastChgUserId;
}

void
McpttCallMsgGrpImmPerilEnd::SetUserId (const McpttCallMsgFieldUserId& userId)
{
  NS_LOG_FUNCTION (this << userId);

  m_userId = userId;
}
/** McpttCallMsgGrpImmPerilEnd - end **/

/** McpttCallMsgGrpEmergEnd - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgGrpEmergEnd);

const uint8_t McpttCallMsgGrpEmergEnd::CODE = 4;

TypeId
McpttCallMsgGrpEmergEnd::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgGrpEmergEnd")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgGrpEmergEnd> ()
  ;

  return tid;
}

McpttCallMsgGrpEmergEnd::McpttCallMsgGrpEmergEnd (void)
  : McpttCallMsg (McpttCallMsgGrpEmergEnd::CODE),
    m_callId (McpttCallMsgFieldCallId ()),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_lastChgTime (McpttCallMsgFieldLastChgTime ()),
    m_lastChgUserId (McpttCallMsgFieldUserId ()),
    m_userId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgGrpEmergEnd::~McpttCallMsgGrpEmergEnd (void)
{}

TypeId
McpttCallMsgGrpEmergEnd::GetInstanceTypeId (void) const
{
  return McpttCallMsgGrpEmergEnd::GetTypeId ();
}

uint32_t
McpttCallMsgGrpEmergEnd::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += callId.GetSerializedSize ();
  size += grpId.GetSerializedSize ();
  size += lastChgTime.GetSerializedSize ();
  size += lastChgUserId.GetSerializedSize ();
  size += userId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgGrpEmergEnd::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  os << "McpttCallMsgGrpEmergEnd(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";grpId=" << grpId;
  os << ";userId=" << userId;
  os << ";lastChgTime=" << lastChgTime;
  os << ";lastChgUserId=" << lastChgUserId;
  os << ")";
}

void
McpttCallMsgGrpEmergEnd::Visit (McpttCallMachineGrpBasic& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallEmergEnd (*this);
}

uint32_t
McpttCallMsgGrpEmergEnd::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += callId.Deserialize (buff);
  bytesRead += grpId.Deserialize (buff);
  bytesRead += lastChgTime.Deserialize (buff);
  bytesRead += lastChgUserId.Deserialize (buff);
  bytesRead += userId.Deserialize (buff);

  SetCallId (callId);
  SetGrpId (grpId);
  SetLastChgTime (lastChgTime);
  SetLastChgUserId (lastChgUserId);
  SetUserId (userId);

  return bytesRead;
}

void
McpttCallMsgGrpEmergEnd::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldLastChgTime lastChgTime = GetLastChgTime ();
  McpttCallMsgFieldUserId lastChgUserId = GetLastChgUserId ();
  McpttCallMsgFieldUserId userId = GetUserId ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  grpId.Serialize (buff);
  lastChgTime.Serialize (buff);
  lastChgUserId.Serialize (buff);
  userId.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgGrpEmergEnd::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldGrpId
McpttCallMsgGrpEmergEnd::GetGrpId (void) const
{
  return m_grpId;
}

McpttCallMsgFieldLastChgTime
McpttCallMsgGrpEmergEnd::GetLastChgTime (void) const
{
  return m_lastChgTime;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpEmergEnd::GetLastChgUserId (void) const
{
  return m_lastChgUserId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpEmergEnd::GetUserId (void) const
{
  return m_userId;
}

void
McpttCallMsgGrpEmergEnd::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgGrpEmergEnd::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}

void
McpttCallMsgGrpEmergEnd::SetLastChgTime (const McpttCallMsgFieldLastChgTime& lastChgTime)
{
  NS_LOG_FUNCTION (this << lastChgTime);

  m_lastChgTime = lastChgTime;
}


void
McpttCallMsgGrpEmergEnd::SetLastChgUserId (const McpttCallMsgFieldUserId& lastChgUserId)
{
  NS_LOG_FUNCTION (this << lastChgUserId);

  m_lastChgUserId = lastChgUserId;
}

void
McpttCallMsgGrpEmergEnd::SetUserId (const McpttCallMsgFieldUserId& userId)
{
  NS_LOG_FUNCTION (this << userId);

  m_userId = userId;
}
/** McpttCallMsgGrpEmergEnd - end **/

/** McpttCallMsgGrpEmergAlert - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgGrpEmergAlert);

const uint8_t McpttCallMsgGrpEmergAlert::CODE = 17;

TypeId
McpttCallMsgGrpEmergAlert::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgGrpEmergAlert")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgGrpEmergAlert> ()
  ;

  return tid;
}

McpttCallMsgGrpEmergAlert::McpttCallMsgGrpEmergAlert (void)
  : McpttCallMsg (McpttCallMsgGrpEmergAlert::CODE),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_orgName (McpttCallMsgFieldOrgName ()),
    m_userId (McpttCallMsgFieldUserId ()),
    m_userLoc (McpttCallMsgFieldUserLoc ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgGrpEmergAlert::~McpttCallMsgGrpEmergAlert (void)
{}

TypeId
McpttCallMsgGrpEmergAlert::GetInstanceTypeId (void) const
{
  return McpttCallMsgGrpEmergAlert::GetTypeId ();
}

uint32_t
McpttCallMsgGrpEmergAlert::GetSerializedSize (void) const
{
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldOrgName orgName = GetOrgName ();
  McpttCallMsgFieldUserId userId = GetUserId ();
  McpttCallMsgFieldUserLoc userLoc = GetUserLoc ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += grpId.GetSerializedSize ();
  size += userId.GetSerializedSize ();
  size += orgName.GetSerializedSize ();
  size += userLoc.GetSerializedSize ();

  return size;
}

void
McpttCallMsgGrpEmergAlert::Print (std::ostream& os) const
{
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldOrgName orgName = GetOrgName ();
  McpttCallMsgFieldUserId userId = GetUserId ();
  McpttCallMsgFieldUserLoc userLoc = GetUserLoc ();

  os << "McpttCallMsgGrpEmergAlert(";

  McpttCallMsg::Print (os);

  os << ";grpId=" << grpId;
  os << ";userId=" << userId;
  os << ";orgName=" << orgName;
  os << ";userLoc=" << userLoc;
  os << ")";
}

void
McpttCallMsgGrpEmergAlert::Visit (McpttCallMachineGrpBasic& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallEmergAlert (*this);
}

void
McpttCallMsgGrpEmergAlert::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallEmergAlert (*this);
}

uint32_t
McpttCallMsgGrpEmergAlert::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldOrgName orgName = GetOrgName ();
  McpttCallMsgFieldUserId userId = GetUserId ();
  McpttCallMsgFieldUserLoc userLoc = GetUserLoc ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += grpId.Deserialize (buff);
  bytesRead += userId.Deserialize (buff);
  bytesRead += orgName.Deserialize (buff);
  bytesRead += userLoc.Deserialize (buff);

  SetGrpId (grpId);
  SetOrgName (orgName);
  SetUserId (userId);
  SetUserLoc (userLoc);

  return bytesRead;
}

void
McpttCallMsgGrpEmergAlert::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldOrgName orgName = GetOrgName ();
  McpttCallMsgFieldUserId userId = GetUserId ();
  McpttCallMsgFieldUserLoc userLoc = GetUserLoc ();

  McpttCallMsg::WriteData (buff);

  grpId.Serialize (buff);
  userId.Serialize (buff);
  orgName.Serialize (buff);
  userLoc.Serialize (buff);
}

McpttCallMsgFieldGrpId
McpttCallMsgGrpEmergAlert::GetGrpId (void) const
{
  return m_grpId;
}

McpttCallMsgFieldOrgName
McpttCallMsgGrpEmergAlert::GetOrgName (void) const
{
  return m_orgName;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpEmergAlert::GetUserId (void) const
{
  return m_userId;
}

McpttCallMsgFieldUserLoc
McpttCallMsgGrpEmergAlert::GetUserLoc (void) const
{
  return m_userLoc;
}

void
McpttCallMsgGrpEmergAlert::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}

void
McpttCallMsgGrpEmergAlert::SetOrgName (const McpttCallMsgFieldOrgName& orgName)
{
  NS_LOG_FUNCTION (this << orgName);

  m_orgName = orgName;
}

void
McpttCallMsgGrpEmergAlert::SetUserId (const McpttCallMsgFieldUserId& userId)
{
  NS_LOG_FUNCTION (this << userId);

  m_userId = userId;
}

void
McpttCallMsgGrpEmergAlert::SetUserLoc (const McpttCallMsgFieldUserLoc& userLoc)
{
  NS_LOG_FUNCTION (this << userLoc);

  m_userLoc = userLoc;
}
/** McpttCallMsgGrpEmergAlert - end **/

/** McpttCallMsgGrpEmergAlertAck - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgGrpEmergAlertAck);

const uint8_t McpttCallMsgGrpEmergAlertAck::CODE = 18;

TypeId
McpttCallMsgGrpEmergAlertAck::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgGrpEmergAlertAck")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgGrpEmergAlertAck> ()
  ;

  return tid;
}

McpttCallMsgGrpEmergAlertAck::McpttCallMsgGrpEmergAlertAck (void)
  : McpttCallMsg (McpttCallMsgGrpEmergAlertAck::CODE),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_origId (McpttCallMsgFieldUserId ()),
    m_sendingId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgGrpEmergAlertAck::~McpttCallMsgGrpEmergAlertAck (void)
{}

TypeId
McpttCallMsgGrpEmergAlertAck::GetInstanceTypeId (void) const
{
  return McpttCallMsgGrpEmergAlertAck::GetTypeId ();
}

uint32_t
McpttCallMsgGrpEmergAlertAck::GetSerializedSize (void) const
{
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += grpId.GetSerializedSize ();
  size += origId.GetSerializedSize ();
  size += sendingId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgGrpEmergAlertAck::Print (std::ostream& os) const
{
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  os << "McpttCallMsgGrpEmergAlertAck(";

  McpttCallMsg::Print (os);

  os << ";grpId=" << grpId;
  os << ";origId=" << origId;
  os << ";sendingId=" << sendingId;
  os << ")";
}

void
McpttCallMsgGrpEmergAlertAck::Visit (McpttCallMachineGrpBasic& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallEmergAlertAck (*this);
}

uint32_t
McpttCallMsgGrpEmergAlertAck::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += grpId.Deserialize (buff);
  bytesRead += origId.Deserialize (buff);
  bytesRead += sendingId.Deserialize (buff);

  SetGrpId (grpId);
  SetOrigId (origId);
  SetSendingId (sendingId);

  return bytesRead;
}

void
McpttCallMsgGrpEmergAlertAck::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  McpttCallMsg::WriteData (buff);

  grpId.Serialize (buff);
  origId.Serialize (buff);
  sendingId.Serialize (buff);
}

McpttCallMsgFieldGrpId
McpttCallMsgGrpEmergAlertAck::GetGrpId (void) const
{
  return m_grpId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpEmergAlertAck::GetOrigId (void) const
{
  return m_origId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpEmergAlertAck::GetSendingId (void) const
{
  return m_sendingId;
}

void
McpttCallMsgGrpEmergAlertAck::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}

void
McpttCallMsgGrpEmergAlertAck::SetOrigId (const McpttCallMsgFieldUserId& origId)
{
  NS_LOG_FUNCTION (this << origId);

  m_origId = origId;
}

void
McpttCallMsgGrpEmergAlertAck::SetSendingId (const McpttCallMsgFieldUserId& sendingId)
{
  NS_LOG_FUNCTION (this << sendingId);

  m_sendingId = sendingId;
}
/** McpttCallMsgGrpEmergAlertAck - end **/

/** McpttCallMsgGrpEmergAlertCancel - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgGrpEmergAlertCancel);

const uint8_t McpttCallMsgGrpEmergAlertCancel::CODE = 19;

TypeId
McpttCallMsgGrpEmergAlertCancel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgGrpEmergAlertCancel")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgGrpEmergAlertCancel> ()
  ;

  return tid;
}

McpttCallMsgGrpEmergAlertCancel::McpttCallMsgGrpEmergAlertCancel (void)
  : McpttCallMsg (McpttCallMsgGrpEmergAlertCancel::CODE),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_origId (McpttCallMsgFieldUserId ()),
    m_sendingId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgGrpEmergAlertCancel::~McpttCallMsgGrpEmergAlertCancel (void)
{}

TypeId
McpttCallMsgGrpEmergAlertCancel::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMsgGrpEmergAlertCancel::GetTypeId ();
}

uint32_t
McpttCallMsgGrpEmergAlertCancel::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += grpId.GetSerializedSize ();
  size += origId.GetSerializedSize ();
  size += sendingId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgGrpEmergAlertCancel::Print (std::ostream& os) const
{
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  os << "McpttCallMsgGrpEmergAlertCancel(";

  McpttCallMsg::Print (os);

  os << ";grpId=" << grpId;
  os << ";origId=" << origId;
  os << ";sendingId=" << sendingId;
  os << ")";
}

void
McpttCallMsgGrpEmergAlertCancel::Visit (McpttCallMachineGrpBasic& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallEmergAlertCancel (*this);
}

void
McpttCallMsgGrpEmergAlertCancel::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallEmergAlertCancel (*this);
}

uint32_t
McpttCallMsgGrpEmergAlertCancel::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += grpId.Deserialize (buff);
  bytesRead += origId.Deserialize (buff);
  bytesRead += sendingId.Deserialize (buff);

  SetGrpId (grpId);
  SetOrigId (origId);
  SetSendingId (sendingId);

  return bytesRead;
}

void
McpttCallMsgGrpEmergAlertCancel::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  McpttCallMsg::WriteData (buff);

  grpId.Serialize (buff);
  origId.Serialize (buff);
  sendingId.Serialize (buff);
}

McpttCallMsgFieldGrpId
McpttCallMsgGrpEmergAlertCancel::GetGrpId (void) const
{
  return m_grpId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpEmergAlertCancel::GetOrigId (void) const
{
  return m_origId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpEmergAlertCancel::GetSendingId (void) const
{
  return m_sendingId;
}

void
McpttCallMsgGrpEmergAlertCancel::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}

void
McpttCallMsgGrpEmergAlertCancel::SetOrigId (const McpttCallMsgFieldUserId& origId)
{
  NS_LOG_FUNCTION (this << origId);

  m_origId = origId;
}

void
McpttCallMsgGrpEmergAlertCancel::SetSendingId (const McpttCallMsgFieldUserId& sendingId)
{
  NS_LOG_FUNCTION (this << sendingId);

  m_sendingId = sendingId;
}
/** McpttCallMsgGrpEmergAlertCancel - end **/

/** McpttCallMsgGrpEmergAlertCancelAck - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgGrpEmergAlertCancelAck);

const uint8_t McpttCallMsgGrpEmergAlertCancelAck::CODE = 20;

TypeId
McpttCallMsgGrpEmergAlertCancelAck::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgGrpEmergAlertCancelAck")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgGrpEmergAlertCancelAck> ()
  ;

  return tid;
}

McpttCallMsgGrpEmergAlertCancelAck::McpttCallMsgGrpEmergAlertCancelAck (void)
  : McpttCallMsg (McpttCallMsgGrpEmergAlertCancelAck::CODE),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_origId (McpttCallMsgFieldUserId ()),
    m_sendingId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgGrpEmergAlertCancelAck::~McpttCallMsgGrpEmergAlertCancelAck (void)
{}

TypeId
McpttCallMsgGrpEmergAlertCancelAck::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCallMsgGrpEmergAlertCancelAck::GetTypeId ();
}

uint32_t
McpttCallMsgGrpEmergAlertCancelAck::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += grpId.GetSerializedSize ();
  size += origId.GetSerializedSize ();
  size += sendingId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgGrpEmergAlertCancelAck::Print (std::ostream& os) const
{
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  os << "McpttCallMsgGrpEmergAlertCancelAck(";

  McpttCallMsg::Print (os);

  os << ";grpIdfield=" << grpId;
  os << ";origId=" << origId;
  os << ";sendingId=" << sendingId;
  os << ")";
}

void
McpttCallMsgGrpEmergAlertCancelAck::Visit (McpttCallMachineGrpBasic& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallEmergAlertCancelAck (*this);
}

uint32_t
McpttCallMsgGrpEmergAlertCancelAck::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += grpId.Deserialize (buff);
  bytesRead += origId.Deserialize (buff);
  bytesRead += sendingId.Deserialize (buff);

  SetGrpId (grpId);
  SetOrigId (origId);
  SetSendingId (sendingId);

  return bytesRead;
}

void
McpttCallMsgGrpEmergAlertCancelAck::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldUserId sendingId = GetSendingId ();

  McpttCallMsg::WriteData (buff);

  grpId.Serialize (buff);
  origId.Serialize (buff);
  sendingId.Serialize (buff);
}

McpttCallMsgFieldGrpId
McpttCallMsgGrpEmergAlertCancelAck::GetGrpId (void) const
{
  return m_grpId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpEmergAlertCancelAck::GetOrigId (void) const
{
  return m_origId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpEmergAlertCancelAck::GetSendingId (void) const
{
  return m_sendingId;
}

void
McpttCallMsgGrpEmergAlertCancelAck::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}

void
McpttCallMsgGrpEmergAlertCancelAck::SetOrigId (const McpttCallMsgFieldUserId& origId)
{
  NS_LOG_FUNCTION (this << origId);

  m_origId = origId;
}

void
McpttCallMsgGrpEmergAlertCancelAck::SetSendingId (const McpttCallMsgFieldUserId& sendingId)
{
  NS_LOG_FUNCTION (this << sendingId);

  m_sendingId = sendingId;
}
/** McpttCallMsgGrpEmergAlertCancelAck - end **/

/** McpttCallMsgGrpBroadcast - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgGrpBroadcast);

const uint8_t McpttCallMsgGrpBroadcast::CODE = 6;

TypeId
McpttCallMsgGrpBroadcast::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgGrpBroadcast")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgGrpBroadcast> ()
  ;

  return tid;
}

McpttCallMsgGrpBroadcast::McpttCallMsgGrpBroadcast (void)
  : McpttCallMsg (McpttCallMsgGrpBroadcast::CODE),
    m_callId (McpttCallMsgFieldCallId ()),
    m_callType (McpttCallMsgFieldCallType ()),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_origId (McpttCallMsgFieldUserId ()),
    m_sdp (McpttCallMsgFieldSdp ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgGrpBroadcast::~McpttCallMsgGrpBroadcast (void)
{}

TypeId
McpttCallMsgGrpBroadcast::GetInstanceTypeId (void) const
{
  return McpttCallMsgGrpBroadcast::GetTypeId ();
}

uint32_t
McpttCallMsgGrpBroadcast::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldSdp sdp = GetSdp ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();
  size += callId.GetSerializedSize ();
  size += callType.GetSerializedSize ();
  size += origId.GetSerializedSize ();
  size += grpId.GetSerializedSize ();
  size += sdp.GetSerializedSize ();

  return size;
}

void
McpttCallMsgGrpBroadcast::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldSdp sdp = GetSdp ();

  os << "McpttCallMsgGrpBroadcast(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";callType=" << callType;
  os << ";origId=" << origId;
  os << ";grpId=" << grpId;
  os << ";sdp=" << sdp;
  os << ")";
}

void
McpttCallMsgGrpBroadcast::Visit (McpttCallMachineGrpBroadcast& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallBroadcast (*this);
}

uint32_t
McpttCallMsgGrpBroadcast::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldSdp sdp = GetSdp ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);
  bytesRead += callId.Deserialize (buff);
  bytesRead += callType.Deserialize (buff);
  bytesRead += origId.Deserialize (buff);
  bytesRead += grpId.Deserialize (buff);
  bytesRead += sdp.Deserialize (buff);

  SetCallId (callId);
  SetCallType (callType);
  SetGrpId (grpId);
  SetOrigId (origId);
  SetSdp (sdp);

  return bytesRead;
}

void
McpttCallMsgGrpBroadcast::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();
  McpttCallMsgFieldSdp sdp = GetSdp ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  callType.Serialize (buff);
  origId.Serialize (buff);
  grpId.Serialize (buff);
  sdp.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgGrpBroadcast::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldCallType
McpttCallMsgGrpBroadcast::GetCallType (void) const
{
  return m_callType;
}

McpttCallMsgFieldGrpId
McpttCallMsgGrpBroadcast::GetGrpId (void) const
{
  return m_grpId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpBroadcast::GetOrigId (void) const
{
  return m_origId;
}

McpttCallMsgFieldSdp
McpttCallMsgGrpBroadcast::GetSdp (void) const
{
  return m_sdp;
}

void
McpttCallMsgGrpBroadcast::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgGrpBroadcast::SetCallType (const McpttCallMsgFieldCallType& callType)
{
  NS_LOG_FUNCTION (this << callType);

  m_callType = callType;
}

void
McpttCallMsgGrpBroadcast::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}

void
McpttCallMsgGrpBroadcast::SetOrigId (const McpttCallMsgFieldUserId& origId)
{
  NS_LOG_FUNCTION (this << origId);

  m_origId = origId;
}

void
McpttCallMsgGrpBroadcast::SetSdp (const McpttCallMsgFieldSdp& sdp)
{
  NS_LOG_FUNCTION (this << sdp);

  m_sdp = sdp;
}
/** McpttCallMsgGrpBroadcast - end **/

/** McpttCallMsgGrpBroadcastEnd - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgGrpBroadcastEnd);

const uint8_t McpttCallMsgGrpBroadcastEnd::CODE = 7;

TypeId
McpttCallMsgGrpBroadcastEnd::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgGrpBroadcastEnd")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgGrpBroadcastEnd> ()
  ;

  return tid;
}

McpttCallMsgGrpBroadcastEnd::McpttCallMsgGrpBroadcastEnd (void)
  : McpttCallMsg (McpttCallMsgGrpBroadcastEnd::CODE),
    m_callId (McpttCallMsgFieldCallId ()),
    m_grpId (McpttCallMsgFieldGrpId ()),
    m_origId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgGrpBroadcastEnd::~McpttCallMsgGrpBroadcastEnd (void)
{}

TypeId
McpttCallMsgGrpBroadcastEnd::GetInstanceTypeId (void) const
{
  return McpttCallMsgGrpBroadcastEnd::GetTypeId ();
}

uint32_t
McpttCallMsgGrpBroadcastEnd::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();
  size += callId.GetSerializedSize ();
  size += grpId.GetSerializedSize ();
  size += origId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgGrpBroadcastEnd::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();

  os << "McpttCallMsgGrpBroadcastEnd(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";grpId=" << grpId;
  os << ";origId=" << origId;
  os << ")";
}

void
McpttCallMsgGrpBroadcastEnd::Visit (McpttCallMachineGrpBroadcast& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveGrpCallBroadcastEnd (*this);
}

uint32_t
McpttCallMsgGrpBroadcastEnd::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);
  bytesRead += callId.Deserialize (buff);
  bytesRead += grpId.Deserialize (buff);
  bytesRead += origId.Deserialize (buff);

  SetCallId (callId);
  SetGrpId (grpId);
  SetOrigId (origId);

  return bytesRead;
}

void
McpttCallMsgGrpBroadcastEnd::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldGrpId grpId = GetGrpId ();
  McpttCallMsgFieldUserId origId = GetOrigId ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  grpId.Serialize (buff);
  origId.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgGrpBroadcastEnd::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldGrpId
McpttCallMsgGrpBroadcastEnd::GetGrpId (void) const
{
  return m_grpId;
}

McpttCallMsgFieldUserId
McpttCallMsgGrpBroadcastEnd::GetOrigId (void) const
{
  return m_origId;
}

void
McpttCallMsgGrpBroadcastEnd::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgGrpBroadcastEnd::SetGrpId (const McpttCallMsgFieldGrpId& grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}

void
McpttCallMsgGrpBroadcastEnd::SetOrigId (const McpttCallMsgFieldUserId& origId)
{
  NS_LOG_FUNCTION (this << origId);

  m_origId = origId;
}
/** McpttCallMsgGrpBroadcastEnd - end **/

/** McpttCallMsgPrivateSetupReq - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgPrivateSetupReq);

const uint8_t McpttCallMsgPrivateSetupReq::CODE = 8;

TypeId
McpttCallMsgPrivateSetupReq::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgPrivateSetupReq")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgPrivateSetupReq> ()
  ;

  return tid;
}

McpttCallMsgPrivateSetupReq::McpttCallMsgPrivateSetupReq (void)
  : McpttCallMsg (8),
    m_callId (McpttCallMsgFieldCallId ()),
    m_callType (McpttCallMsgFieldCallType ()),
    m_calleeId (McpttCallMsgFieldUserId ()),
    m_callerId (McpttCallMsgFieldUserId ()),
    m_commMode (McpttCallMsgFieldCommMode ()),
    m_sdp (McpttCallMsgFieldSdp ()),
    m_userLoc (McpttCallMsgFieldUserLoc ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgPrivateSetupReq::~McpttCallMsgPrivateSetupReq (void)
{}

TypeId
McpttCallMsgPrivateSetupReq::GetInstanceTypeId (void) const
{
  return McpttCallMsgPrivateSetupReq::GetTypeId ();
}

uint32_t
McpttCallMsgPrivateSetupReq::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldCommMode commMode = GetCommMode ();
  McpttCallMsgFieldSdp sdp = GetSdp ();
  McpttCallMsgFieldUserLoc userLoc = GetUserLoc ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();
  size += callId.GetSerializedSize ();
  size += commMode.GetSerializedSize ();
  size += callType.GetSerializedSize ();
  size += callerId.GetSerializedSize ();
  size += calleeId.GetSerializedSize ();
  size += sdp.GetSerializedSize ();
  size += userLoc.GetSerializedSize ();

  return size;
}

void
McpttCallMsgPrivateSetupReq::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldCommMode commMode = GetCommMode ();
  McpttCallMsgFieldSdp sdp = GetSdp ();
  McpttCallMsgFieldUserLoc userLoc = GetUserLoc ();

  os << "McpttCallMsgPrivateSetupReq(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";commMode=" << commMode;
  os << ";callType=" << callType;
  os << ";callerId=" << callerId;
  os << ";calleeId=" << calleeId;
  os << ";sdp=" << sdp;
  os << ";userLoc=" << userLoc;
  os << ")";
}

void
McpttCallMsgPrivateSetupReq::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveSetupRequest (*this);
}

uint32_t
McpttCallMsgPrivateSetupReq::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldCommMode commMode = GetCommMode ();
  McpttCallMsgFieldSdp sdp = GetSdp ();
  McpttCallMsgFieldUserLoc userLoc = GetUserLoc ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);
  bytesRead += callId.Deserialize (buff);
  bytesRead += commMode.Deserialize (buff);
  bytesRead += callType.Deserialize (buff);
  bytesRead += callerId.Deserialize (buff);
  bytesRead += calleeId.Deserialize (buff);
  bytesRead += sdp.Deserialize (buff);
  bytesRead += userLoc.Deserialize (buff);

  SetCallId (callId);
  SetCallType (callType);
  SetCalleeId (calleeId);
  SetCallerId (callerId);
  SetCommMode (commMode);
  SetSdp (sdp);
  SetUserLoc (userLoc);

  return bytesRead;
}

void
McpttCallMsgPrivateSetupReq::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldCallType callType = GetCallType ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldCommMode commMode = GetCommMode ();
  McpttCallMsgFieldSdp sdp = GetSdp ();
  McpttCallMsgFieldUserLoc userLoc = GetUserLoc ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  commMode.Serialize (buff);
  callType.Serialize (buff);
  callerId.Serialize (buff);
  calleeId.Serialize (buff);
  sdp.Serialize (buff);
  userLoc.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgPrivateSetupReq::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldCallType
McpttCallMsgPrivateSetupReq::GetCallType (void) const
{
  return m_callType;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateSetupReq::GetCalleeId (void) const
{
  return m_calleeId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateSetupReq::GetCallerId (void) const
{
  return m_callerId;
}

McpttCallMsgFieldCommMode
McpttCallMsgPrivateSetupReq::GetCommMode (void) const
{
  return m_commMode;
}

McpttCallMsgFieldSdp
McpttCallMsgPrivateSetupReq::GetSdp (void) const
{
  return m_sdp;
}

McpttCallMsgFieldUserLoc
McpttCallMsgPrivateSetupReq::GetUserLoc (void) const
{
  return m_userLoc;
}

void
McpttCallMsgPrivateSetupReq::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgPrivateSetupReq::SetCallType (const McpttCallMsgFieldCallType& callType)
{
  NS_LOG_FUNCTION (this << callType);

  m_callType = callType;
}

void
McpttCallMsgPrivateSetupReq::SetCalleeId (const McpttCallMsgFieldUserId& calleeId)
{
  NS_LOG_FUNCTION (this << calleeId);

  m_calleeId = calleeId;
}

void
McpttCallMsgPrivateSetupReq::SetCallerId (const McpttCallMsgFieldUserId& callerId)
{
  NS_LOG_FUNCTION (this << callerId);

  m_callerId = callerId;
}

void
McpttCallMsgPrivateSetupReq::SetCommMode (const McpttCallMsgFieldCommMode& commMode)
{
  NS_LOG_FUNCTION (this << commMode);

  m_commMode = commMode;
}

void
McpttCallMsgPrivateSetupReq::SetSdp (const McpttCallMsgFieldSdp& sdp)
{
  NS_LOG_FUNCTION (this << sdp);

  m_sdp = sdp;
}

void
McpttCallMsgPrivateSetupReq::SetUserLoc (const McpttCallMsgFieldUserLoc& userLoc)
{
  NS_LOG_FUNCTION (this << &userLoc);

  m_userLoc = userLoc;
}
/** McpttCallMsgPrivateSetupReq - end **/

/** McpttCallMsgPrivateRinging - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgPrivateRinging);

const uint8_t McpttCallMsgPrivateRinging::CODE = 9;

TypeId
McpttCallMsgPrivateRinging::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgPrivateRinging")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgPrivateRinging> ()
  ;

  return tid;
}

McpttCallMsgPrivateRinging::McpttCallMsgPrivateRinging (void)
  : McpttCallMsg (9),
    m_callId (McpttCallMsgFieldCallId ()),
    m_calleeId (McpttCallMsgFieldUserId ()),
    m_callerId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgPrivateRinging::~McpttCallMsgPrivateRinging (void)
{}

TypeId
McpttCallMsgPrivateRinging::GetInstanceTypeId (void) const
{
  return McpttCallMsgPrivateRinging::GetTypeId ();
}

uint32_t
McpttCallMsgPrivateRinging::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();
  size += callId.GetSerializedSize ();
  size += callerId.GetSerializedSize ();
  size += calleeId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgPrivateRinging::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  os << "McpttCallMsgPrivateRinging(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";callerId=" << callerId;
  os << ";calleeId=" << calleeId;
  os << ")";
}

void
McpttCallMsgPrivateRinging::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveRinging (*this);
}

uint32_t
McpttCallMsgPrivateRinging::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += callId.Deserialize (buff);
  bytesRead += callerId.Deserialize (buff);
  bytesRead += calleeId.Deserialize (buff);

  SetCallId (callId);
  SetCallerId (callerId);
  SetCalleeId (calleeId);

  return bytesRead;
}

void
McpttCallMsgPrivateRinging::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  callerId.Serialize (buff);
  calleeId.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgPrivateRinging::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateRinging::GetCalleeId (void) const
{
  return m_calleeId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateRinging::GetCallerId (void) const
{
  return m_callerId;
}

void
McpttCallMsgPrivateRinging::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << &callId);

  m_callId = callId;
}

void
McpttCallMsgPrivateRinging::SetCalleeId (const McpttCallMsgFieldUserId& calleeId)
{
  NS_LOG_FUNCTION (this << calleeId);

  m_calleeId = calleeId;
}

void
McpttCallMsgPrivateRinging::SetCallerId (const McpttCallMsgFieldUserId& callerId)
{
  NS_LOG_FUNCTION (this << callerId);

  m_callerId = callerId;
}
/** McpttCallMsgPrivateRinging - end **/

/** McpttCallMsgPrivateAccept - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgPrivateAccept);

const uint8_t McpttCallMsgPrivateAccept::CODE = 10;

TypeId
McpttCallMsgPrivateAccept::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgPrivateAccept")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgPrivateAccept> ()
  ;

  return tid;
}

McpttCallMsgPrivateAccept::McpttCallMsgPrivateAccept (void)
  : McpttCallMsg (10),
    m_callId (McpttCallMsgFieldCallId ()),
    m_calleeId (McpttCallMsgFieldUserId ()),
    m_callerId (McpttCallMsgFieldUserId ()),
    m_sdp (McpttCallMsgFieldSdp ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgPrivateAccept::~McpttCallMsgPrivateAccept (void)
{}

TypeId
McpttCallMsgPrivateAccept::GetInstanceTypeId (void) const
{
  return McpttCallMsgPrivateAccept::GetTypeId ();
}

uint32_t
McpttCallMsgPrivateAccept::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldSdp sdp = GetSdp ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();
  size += callId.GetSerializedSize ();
  size += callerId.GetSerializedSize ();
  size += calleeId.GetSerializedSize ();
  size += sdp.GetSerializedSize ();

  return size;
}

void
McpttCallMsgPrivateAccept::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldSdp sdp = GetSdp ();

  os << "McpttCallMsgPrivateAccept(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";callerId=" << callerId;
  os << ";calleeId=" << calleeId;
  os << ";sdp=" << sdp;
  os << ")";
}

void
McpttCallMsgPrivateAccept::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveAccept (*this);
}

uint32_t
McpttCallMsgPrivateAccept::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldSdp sdp = GetSdp ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += callId.Deserialize (buff);
  bytesRead += callerId.Deserialize (buff);
  bytesRead += calleeId.Deserialize (buff);
  bytesRead += sdp.Deserialize (buff);

  SetCallId (callId);
  SetCallerId (callerId);
  SetCalleeId (calleeId);
  SetSdp (sdp);

  return bytesRead;
}

void
McpttCallMsgPrivateAccept::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldSdp sdp = GetSdp ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  callerId.Serialize (buff);
  calleeId.Serialize (buff);
  sdp.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgPrivateAccept::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateAccept::GetCalleeId (void) const
{
  return m_calleeId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateAccept::GetCallerId (void) const
{
  return m_callerId;
}

McpttCallMsgFieldSdp
McpttCallMsgPrivateAccept::GetSdp (void) const
{
  return m_sdp;
}

void
McpttCallMsgPrivateAccept::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgPrivateAccept::SetCalleeId (const McpttCallMsgFieldUserId& calleeId)
{
  NS_LOG_FUNCTION (this << calleeId);

  m_calleeId = calleeId;
}

void
McpttCallMsgPrivateAccept::SetCallerId (const McpttCallMsgFieldUserId& callerId)
{
  NS_LOG_FUNCTION (this << callerId);

  m_callerId = callerId;
}

void
McpttCallMsgPrivateAccept::SetSdp (const McpttCallMsgFieldSdp& sdp)
{
  NS_LOG_FUNCTION (this << sdp);

  m_sdp = sdp;
}
/** McpttCallMsgPrivateAccept - end **/

/** McpttCallMsgPrivateReject - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgPrivateReject);

const uint8_t McpttCallMsgPrivateReject::CODE = 11;

TypeId
McpttCallMsgPrivateReject::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgPrivateReject")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgPrivateReject> ()
  ;

  return tid;
}

McpttCallMsgPrivateReject::McpttCallMsgPrivateReject (void)
  : McpttCallMsg (11),
    m_callId (McpttCallMsgFieldCallId ()),
    m_calleeId (McpttCallMsgFieldUserId ()),
    m_callerId (McpttCallMsgFieldUserId ()),
    m_reason (McpttCallMsgFieldReason ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgPrivateReject::~McpttCallMsgPrivateReject (void)
{}

TypeId
McpttCallMsgPrivateReject::GetInstanceTypeId (void) const
{
  return McpttCallMsgPrivateReject::GetTypeId ();
}

uint32_t
McpttCallMsgPrivateReject::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldReason reason = GetReason ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += callId.GetSerializedSize ();
  size += reason.GetSerializedSize ();
  size += callerId.GetSerializedSize ();
  size += calleeId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgPrivateReject::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldReason reason = GetReason ();

  os << "McpttCallMsgPrivateReject(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";reason=" << reason;
  os << ";callerId=" << callerId;
  os << ";calleeId=" << calleeId;
  os << ")";
}

void
McpttCallMsgPrivateReject::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveReject (*this);
}

uint32_t
McpttCallMsgPrivateReject::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldReason reason = GetReason ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += callId.Deserialize (buff);
  bytesRead += reason.Deserialize (buff);
  bytesRead += callerId.Deserialize (buff);
  bytesRead += calleeId.Deserialize (buff);

  SetCallId (callId);
  SetCalleeId (calleeId);
  SetCallerId (callerId);
  SetReason (reason);

  return bytesRead;
}

void
McpttCallMsgPrivateReject::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();
  McpttCallMsgFieldReason reason = GetReason ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  reason.Serialize (buff);
  callerId.Serialize (buff);
  calleeId.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgPrivateReject::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateReject::GetCalleeId (void) const
{
  return m_calleeId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateReject::GetCallerId (void) const
{
  return m_callerId;
}

McpttCallMsgFieldReason
McpttCallMsgPrivateReject::GetReason (void) const
{
  return m_reason;
}

void
McpttCallMsgPrivateReject::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgPrivateReject::SetCalleeId (const McpttCallMsgFieldUserId& calleeId)
{
  NS_LOG_FUNCTION (this << calleeId);

  m_calleeId = calleeId;
}

void
McpttCallMsgPrivateReject::SetCallerId (const McpttCallMsgFieldUserId& callerId)
{
  NS_LOG_FUNCTION (this << callerId);

  m_callerId = callerId;
}

void
McpttCallMsgPrivateReject::SetReason (const McpttCallMsgFieldReason& reason)
{
  NS_LOG_FUNCTION (this << reason);

  m_reason = reason;
}
/** McpttCallMsgPrivateReject - end **/

/** McpttCallMsgPrivateRelease - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgPrivateRelease);

const uint8_t McpttCallMsgPrivateRelease::CODE = 12;

TypeId
McpttCallMsgPrivateRelease::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgPrivateRelease")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgPrivateRelease> ()
  ;

  return tid;
}

McpttCallMsgPrivateRelease::McpttCallMsgPrivateRelease (void)
  : McpttCallMsg (12),
    m_callId (McpttCallMsgFieldCallId ()),
    m_calleeId (McpttCallMsgFieldUserId ()),
    m_callerId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgPrivateRelease::~McpttCallMsgPrivateRelease (void)
{}

TypeId
McpttCallMsgPrivateRelease::GetInstanceTypeId (void) const
{
  return McpttCallMsgPrivateRelease::GetTypeId ();
}

uint32_t
McpttCallMsgPrivateRelease::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += callId.GetSerializedSize ();
  size += callerId.GetSerializedSize ();
  size += calleeId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgPrivateRelease::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  os << "McpttCallMsgPrivateRelease(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";callerId=" << callerId;
  os << ";calleeId=" << calleeId;
  os << ")";
}

void
McpttCallMsgPrivateRelease::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveRelease (*this);
}

uint32_t
McpttCallMsgPrivateRelease::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += callId.Deserialize (buff);
  bytesRead += callerId.Deserialize (buff);
  bytesRead += calleeId.Deserialize (buff);

  SetCallId (callId);
  SetCalleeId (calleeId);
  SetCallerId (callerId);

  return bytesRead;
}

void
McpttCallMsgPrivateRelease::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  callerId.Serialize (buff);
  calleeId.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgPrivateRelease::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateRelease::GetCalleeId (void) const
{
  return m_calleeId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateRelease::GetCallerId (void) const
{
  return m_callerId;
}

void
McpttCallMsgPrivateRelease::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgPrivateRelease::SetCalleeId (const McpttCallMsgFieldUserId& calleeId)
{
  NS_LOG_FUNCTION (this << calleeId);

  m_calleeId = calleeId;
}

void
McpttCallMsgPrivateRelease::SetCallerId (const McpttCallMsgFieldUserId& callerId)
{
  NS_LOG_FUNCTION (this << callerId);

  m_callerId = callerId;
}
/** McpttCallMsgPrivateRelease - end **/

/** McpttCallMsgPrivateReleaseAck - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgPrivateReleaseAck);

const uint8_t McpttCallMsgPrivateReleaseAck::CODE = 13;

TypeId
McpttCallMsgPrivateReleaseAck::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgPrivateReleaseAck")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgPrivateReleaseAck> ()
  ;

  return tid;
}

McpttCallMsgPrivateReleaseAck::McpttCallMsgPrivateReleaseAck (void)
  : McpttCallMsg (13),
    m_callId (McpttCallMsgFieldCallId ()),
    m_calleeId (McpttCallMsgFieldUserId ()),
    m_callerId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgPrivateReleaseAck::~McpttCallMsgPrivateReleaseAck (void)
{}

TypeId
McpttCallMsgPrivateReleaseAck::GetInstanceTypeId (void) const
{
  return McpttCallMsgPrivateReleaseAck::GetTypeId ();
}

uint32_t
McpttCallMsgPrivateReleaseAck::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += callId.GetSerializedSize ();
  size += callerId.GetSerializedSize ();
  size += calleeId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgPrivateReleaseAck::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  os << "McpttCallMsgPrivateReleaseAck(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";callerId=" << callerId;
  os << ";calleeId=" << calleeId;
  os << ")";
}

void
McpttCallMsgPrivateReleaseAck::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveReleaseAck (*this);
}

uint32_t
McpttCallMsgPrivateReleaseAck::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += callId.Deserialize (buff);
  bytesRead += callerId.Deserialize (buff);
  bytesRead += calleeId.Deserialize (buff);

  SetCallId (callId);
  SetCallerId (callerId);
  SetCalleeId (calleeId);

  return bytesRead;
}

void
McpttCallMsgPrivateReleaseAck::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  callerId.Serialize (buff);
  calleeId.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgPrivateReleaseAck::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateReleaseAck::GetCalleeId (void) const
{
  return m_calleeId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateReleaseAck::GetCallerId (void) const
{
  return m_callerId;
}

void
McpttCallMsgPrivateReleaseAck::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgPrivateReleaseAck::SetCalleeId (const McpttCallMsgFieldUserId& calleeId)
{
  NS_LOG_FUNCTION (this << calleeId);

  m_calleeId = calleeId;
}

void
McpttCallMsgPrivateReleaseAck::SetCallerId (const McpttCallMsgFieldUserId& callerId)
{
  NS_LOG_FUNCTION (this << callerId);

  m_callerId = callerId;
}
/** McpttCallMsgPrivateReleaseAck - end **/

/** McpttCallMsgPrivateAcceptAck - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgPrivateAcceptAck);

const uint8_t McpttCallMsgPrivateAcceptAck::CODE = 14;

TypeId
McpttCallMsgPrivateAcceptAck::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgPrivateAcceptAck")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgPrivateAcceptAck> ()
  ;

  return tid;
}

McpttCallMsgPrivateAcceptAck::McpttCallMsgPrivateAcceptAck (void)
  : McpttCallMsg (14),
    m_callId (McpttCallMsgFieldCallId ()),
    m_calleeId (McpttCallMsgFieldUserId ()),
    m_callerId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgPrivateAcceptAck::~McpttCallMsgPrivateAcceptAck (void)
{}

TypeId
McpttCallMsgPrivateAcceptAck::GetInstanceTypeId (void) const
{
  return McpttCallMsgPrivateAcceptAck::GetTypeId ();
}

uint32_t
McpttCallMsgPrivateAcceptAck::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += callId.GetSerializedSize ();
  size += callerId.GetSerializedSize ();
  size += calleeId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgPrivateAcceptAck::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  os << "McpttCallMsgPrivateAcceptAck(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";callerId=" << callerId;
  os << ";calleeId=" << calleeId;
  os << ")";
}

void
McpttCallMsgPrivateAcceptAck::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveAcceptAck (*this);
}

uint32_t
McpttCallMsgPrivateAcceptAck::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += callId.Deserialize (buff);
  bytesRead += callerId.Deserialize (buff);
  bytesRead += calleeId.Deserialize (buff);

  SetCallId (callId);
  SetCallerId (callerId);
  SetCalleeId (calleeId);

  return bytesRead;
}

void
McpttCallMsgPrivateAcceptAck::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  callerId.Serialize (buff);
  calleeId.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgPrivateAcceptAck::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateAcceptAck::GetCalleeId (void) const
{
  return m_calleeId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateAcceptAck::GetCallerId (void) const
{
  return m_callerId;
}

void
McpttCallMsgPrivateAcceptAck::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgPrivateAcceptAck::SetCalleeId (const McpttCallMsgFieldUserId& calleeId)
{
  NS_LOG_FUNCTION (this << calleeId);

  m_calleeId = calleeId;
}

void
McpttCallMsgPrivateAcceptAck::SetCallerId (const McpttCallMsgFieldUserId& callerId)
{
  NS_LOG_FUNCTION (this << callerId);

  m_callerId = callerId;
}
/** McpttCallMsgPrivateAcceptAck - end **/

/** McpttCallMsgPrivateEmergCancel - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgPrivateEmergCancel);

const uint8_t McpttCallMsgPrivateEmergCancel::CODE = 15;

TypeId
McpttCallMsgPrivateEmergCancel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgPrivateEmergCancel")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgPrivateEmergCancel> ()
  ;

  return tid;
}

McpttCallMsgPrivateEmergCancel::McpttCallMsgPrivateEmergCancel (void)
  : McpttCallMsg (15),
    m_callId (McpttCallMsgFieldCallId ()),
    m_calleeId (McpttCallMsgFieldUserId ()),
    m_callerId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgPrivateEmergCancel::~McpttCallMsgPrivateEmergCancel (void)
{}

TypeId
McpttCallMsgPrivateEmergCancel::GetInstanceTypeId (void) const
{
  return McpttCallMsgPrivateEmergCancel::GetTypeId ();
}

uint32_t
McpttCallMsgPrivateEmergCancel::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += callId.GetSerializedSize ();
  size += callerId.GetSerializedSize ();
  size += calleeId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgPrivateEmergCancel::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  os << "McpttCallMsgPrivateEmergCancel(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";callerId=" << callerId;
  os << ";calleeId=" << calleeId;
  os << ")";
}

void
McpttCallMsgPrivateEmergCancel::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveEmergencyCancel (*this);
}

uint32_t
McpttCallMsgPrivateEmergCancel::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += callId.Deserialize (buff);
  bytesRead += callerId.Deserialize (buff);
  bytesRead += calleeId.Deserialize (buff);

  SetCallId (callId);
  SetCallerId (callerId);
  SetCalleeId (calleeId);

  return bytesRead;
}

void
McpttCallMsgPrivateEmergCancel::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  callerId.Serialize (buff);
  calleeId.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgPrivateEmergCancel::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateEmergCancel::GetCalleeId (void) const
{
  return m_calleeId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateEmergCancel::GetCallerId (void) const
{
  return m_callerId;
}

void
McpttCallMsgPrivateEmergCancel::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgPrivateEmergCancel::SetCalleeId (const McpttCallMsgFieldUserId& calleeId)
{
  NS_LOG_FUNCTION (this << calleeId);

  m_calleeId = calleeId;
}

void
McpttCallMsgPrivateEmergCancel::SetCallerId (const McpttCallMsgFieldUserId& callerId)
{
  NS_LOG_FUNCTION (this << callerId);

  m_callerId = callerId;
}
/** McpttCallMsgPrivateEmergCancel - end **/

/** McpttCallMsgPrivateEmergCancelAck - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgPrivateEmergCancelAck);

const uint8_t McpttCallMsgPrivateEmergCancelAck::CODE = 16;

TypeId
McpttCallMsgPrivateEmergCancelAck::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgPrivateEmergCancelAck")
    .SetParent<McpttCallMsg> ()
    .AddConstructor<McpttCallMsgPrivateEmergCancelAck> ()
  ;

  return tid;
}

McpttCallMsgPrivateEmergCancelAck::McpttCallMsgPrivateEmergCancelAck (void)
  : McpttCallMsg (16),
    m_callId (McpttCallMsgFieldCallId ()),
    m_calleeId (McpttCallMsgFieldUserId ()),
    m_callerId (McpttCallMsgFieldUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgPrivateEmergCancelAck::~McpttCallMsgPrivateEmergCancelAck (void)
{}

TypeId
McpttCallMsgPrivateEmergCancelAck::GetInstanceTypeId (void) const
{
  return McpttCallMsgPrivateEmergCancelAck::GetTypeId ();
}

uint32_t
McpttCallMsgPrivateEmergCancelAck::GetSerializedSize (void) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t size = McpttCallMsg::GetSerializedSize ();

  size += callId.GetSerializedSize ();
  size += callerId.GetSerializedSize ();
  size += calleeId.GetSerializedSize ();

  return size;
}

void
McpttCallMsgPrivateEmergCancelAck::Print (std::ostream& os) const
{
  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  os << "McpttCallMsgPrivateEmergCancelAck(";

  McpttCallMsg::Print (os);

  os << ";callId=" << callId;
  os << ";callerId=" << callerId;
  os << ";calleeId=" << calleeId;
  os << ")";
}

void
McpttCallMsgPrivateEmergCancelAck::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveEmergencyCancelAck (*this);
}

uint32_t
McpttCallMsgPrivateEmergCancelAck::ReadData (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  uint32_t bytesRead = McpttCallMsg::ReadData (buff);

  bytesRead += callId.Deserialize (buff);
  bytesRead += callerId.Deserialize (buff);
  bytesRead += calleeId.Deserialize (buff);

  SetCallId (callId);
  SetCallerId (callerId);
  SetCalleeId (calleeId);

  return bytesRead;
}

void
McpttCallMsgPrivateEmergCancelAck::WriteData (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldCallId callId = GetCallId ();
  McpttCallMsgFieldUserId calleeId = GetCalleeId ();
  McpttCallMsgFieldUserId callerId = GetCallerId ();

  McpttCallMsg::WriteData (buff);

  callId.Serialize (buff);
  callerId.Serialize (buff);
  calleeId.Serialize (buff);
}

McpttCallMsgFieldCallId
McpttCallMsgPrivateEmergCancelAck::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateEmergCancelAck::GetCalleeId (void) const
{
  return m_calleeId;
}

McpttCallMsgFieldUserId
McpttCallMsgPrivateEmergCancelAck::GetCallerId (void) const
{
  return m_callerId;
}

void
McpttCallMsgPrivateEmergCancelAck::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_FUNCTION (this << callId);

  m_callId = callId;
}

void
McpttCallMsgPrivateEmergCancelAck::SetCalleeId (const McpttCallMsgFieldUserId& calleeId)
{
  NS_LOG_FUNCTION (this << calleeId);

  m_calleeId = calleeId;
}

void
McpttCallMsgPrivateEmergCancelAck::SetCallerId (const McpttCallMsgFieldUserId& callerId)
{
  NS_LOG_FUNCTION (this << callerId);

  m_callerId = callerId;
}
/** McpttCallMsgPrivateEmergCancelAck - end **/

} // namespace psc
} // namespace ns3

