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

#include <ns3/buffer.h>
#include <ns3/ipv4-address.h>
#include <ns3/ipv6-address.h>
#include <ns3/log.h>
#include <ns3/nstime.h>
#include <ns3/object-base.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/vector.h>

#include "mcptt-call-msg-field.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallMsgField");

namespace psc {

/** McpttCallMsgField - begin **/

NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgField);

TypeId
McpttCallMsgField::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgField")
    .SetParent<ObjectBase> ()
  ;

  return tid;
}

McpttCallMsgField::~McpttCallMsgField (void)
{}

TypeId
McpttCallMsgField::GetInstanceTypeId (void) const
{
  return McpttCallMsgField::GetTypeId ();
}

McpttCallMsgField::McpttCallMsgField (void)
  : ObjectBase ()
{
  NS_LOG_FUNCTION (this);
}
/** McpttCallMsgField - end **/

std::ostream& operator<< (std::ostream& os, const McpttCallMsgField& field)
{
  field.Print (os);

  return os;
}

/** McpttCallMsgFieldType6 - begin **/
TypeId
McpttCallMsgFieldType6::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldType6")
    .SetParent<McpttCallMsgField> ()
  ;

  return tid;
}

McpttCallMsgFieldType6::~McpttCallMsgFieldType6 (void)
{}

uint32_t
McpttCallMsgFieldType6::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint16_t length = 0;
  uint32_t bytesRead = 0;

  length += buff.ReadNtohU16 ();
  bytesRead += 2;

  SetLength (length);

  return bytesRead;
}

TypeId
McpttCallMsgFieldType6::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldType6::GetTypeId ();
}

uint32_t
McpttCallMsgFieldType6::GetSerializedSize (void) const
{
  uint16_t length = GetLength ();
  // Two bytes for the length field, plus "length" bytes in the content.
  uint32_t serializedSize = 2 + length;

  return serializedSize;
}

void
McpttCallMsgFieldType6::Print (std::ostream& os) const
{
  uint16_t length = GetLength ();

  os << "McpttCallMsgFieldType6(";
  os << "length=" << (uint32_t)length;
  os << ")";
}

void
McpttCallMsgFieldType6::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  uint16_t length = GetLength ();

  buff.WriteHtonU16 (length);
}

McpttCallMsgFieldType6::McpttCallMsgFieldType6 (void)
  : McpttCallMsgField (),
    m_length (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldType6::McpttCallMsgFieldType6 (uint16_t length)
  : McpttCallMsgField (),
    m_length (length)
{
  NS_LOG_FUNCTION (this);
}

uint16_t
McpttCallMsgFieldType6::GetLength (void) const
{
  return m_length;
}

void
McpttCallMsgFieldType6::SetLength (uint16_t length)
{
  NS_LOG_FUNCTION (this << (uint32_t)length);

  m_length = length;
}
/** McpttCallMsgFieldType6 - end **/

/** McpttCallMsgFieldMsgType - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttCallMsgFieldMsgType);

TypeId
McpttCallMsgFieldMsgType::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldMsgType")
    .SetParent<McpttCallMsgField> ()
    .AddConstructor<McpttCallMsgFieldMsgType> ()
  ;

  return tid;
}

McpttCallMsgFieldMsgType::McpttCallMsgFieldMsgType (void)
  : McpttCallMsgField (),
    m_type (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldMsgType::McpttCallMsgFieldMsgType (uint8_t type)
  : McpttCallMsgField (),
    m_type (type)
{
  NS_LOG_FUNCTION (this << (uint32_t)type);
}

McpttCallMsgFieldMsgType::~McpttCallMsgFieldMsgType (void)
{}

uint32_t
McpttCallMsgFieldMsgType::Deserialize (Buffer::Iterator& start)
{
  NS_LOG_FUNCTION (this << &start);

  uint32_t bytesRead = 0;

  uint8_t type = start.ReadU8 ();
  bytesRead += 1;

  SetType (type);

  return bytesRead;
}

TypeId
McpttCallMsgFieldMsgType::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldMsgType::GetTypeId ();
}

uint32_t
McpttCallMsgFieldMsgType::GetSerializedSize (void) const
{
  return 1; // One byte for the message type.
}

void
McpttCallMsgFieldMsgType::Print (std::ostream& os) const
{
  uint8_t type = GetType ();

  os << "McpttCallMsgFieldMsgType(";
  os << "type=" << (uint32_t)type;
  os << ")";
}

void
McpttCallMsgFieldMsgType::Serialize (Buffer::Iterator& start) const
{
  NS_LOG_FUNCTION (this << &start);

  uint8_t type = GetType ();

  start.WriteU8 (type);
}

uint8_t
McpttCallMsgFieldMsgType::GetType (void) const
{
  return m_type;
}

void
McpttCallMsgFieldMsgType::SetType (uint8_t type)
{
  NS_LOG_FUNCTION (this);

  m_type = type;
}
/** McpttCallMsgFieldMsgType - end **/

/** McpttCallMsgFieldCallId - begin **/
TypeId
McpttCallMsgFieldCallId::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldCallId")
    .SetParent<McpttCallMsgField> ()
    .AddConstructor<McpttCallMsgFieldCallId> ()
  ;

  return tid;
}

McpttCallMsgFieldCallId::McpttCallMsgFieldCallId (void)
  : McpttCallMsgField (),
    m_callId (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldCallId::McpttCallMsgFieldCallId (uint16_t callId)
  : McpttCallMsgField (),
    m_callId (callId)
{
  NS_LOG_FUNCTION (this << (uint32_t)callId);
}

McpttCallMsgFieldCallId::~McpttCallMsgFieldCallId (void)
{}

uint32_t
McpttCallMsgFieldCallId::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint32_t bytesRead = 0;

  uint16_t callId = buff.ReadNtohU16 ();
  bytesRead += 2;

  SetCallId (callId);

  return bytesRead;
}

TypeId
McpttCallMsgFieldCallId::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldCallId::GetTypeId ();
}

uint32_t
McpttCallMsgFieldCallId::GetSerializedSize (void) const
{
  return 2;
}

void
McpttCallMsgFieldCallId::Print (std::ostream& os) const
{
  uint16_t callId = GetCallId ();

  os << "McpttCallMsgFieldCallId(";
  os << "callId=" << (uint32_t)callId;
  os << ")";
}

void
McpttCallMsgFieldCallId::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  uint16_t callId = GetCallId ();

  buff.WriteHtonU16 (callId);
}

uint16_t
McpttCallMsgFieldCallId::GetCallId (void) const
{
  return m_callId;
}

void
McpttCallMsgFieldCallId::SetCallId (uint16_t callId)
{
  NS_LOG_FUNCTION (this << (uint32_t)callId);

  m_callId = callId;
}
/** McpttCallMsgFieldCallId - end **/

/** McpttCallMsgFieldRefreshInterval - begin **/
TypeId
McpttCallMsgFieldRefreshInterval::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldRefreshInterval")
    .SetParent<McpttCallMsgField> ()
    .AddConstructor <McpttCallMsgFieldRefreshInterval> ()
  ;

  return tid;
}

McpttCallMsgFieldRefreshInterval::McpttCallMsgFieldRefreshInterval (void)
  : McpttCallMsgField (),
    m_interval (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldRefreshInterval::McpttCallMsgFieldRefreshInterval (uint16_t interval)
  : McpttCallMsgField (),
    m_interval (interval)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldRefreshInterval::~McpttCallMsgFieldRefreshInterval (void)
{}

uint32_t
McpttCallMsgFieldRefreshInterval::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  uint32_t bytesRead = 0;

  uint16_t interval = buff.ReadNtohU16 ();
  bytesRead += 2;

  SetInterval (interval);

  return bytesRead;
}

TypeId
McpttCallMsgFieldRefreshInterval::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldRefreshInterval::GetTypeId ();
}

uint32_t
McpttCallMsgFieldRefreshInterval::GetSerializedSize (void) const
{
  return 2;
}

void
McpttCallMsgFieldRefreshInterval::Print (std::ostream& os) const
{
  uint16_t interval = GetInterval ();

  os << "McpttCallMsgFieldRefreshInterval(";
  os << "interval=" << (uint32_t)interval;
  os << ")";
}

void
McpttCallMsgFieldRefreshInterval::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  uint16_t interval = GetInterval ();

  buff.WriteHtonU16 (interval);
}

uint16_t
McpttCallMsgFieldRefreshInterval::GetInterval (void) const
{
  return m_interval;
}

void
McpttCallMsgFieldRefreshInterval::SetInterval (uint16_t interval)
{
  NS_LOG_FUNCTION (this << (uint32_t)interval);

  m_interval = interval;
}
/** McpttCallMsgFieldRefreshInterval - end **/

/** McpttCallMsgFieldGrpId - begin **/
TypeId
McpttCallMsgFieldGrpId::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldGrpId")
    .SetParent<McpttCallMsgField> ()
    .AddConstructor<McpttCallMsgFieldGrpId> ()
  ;

  return tid;
}

McpttCallMsgFieldGrpId::McpttCallMsgFieldGrpId (void)
  : McpttCallMsgFieldType6 (4),
    m_grpId (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldGrpId::McpttCallMsgFieldGrpId (uint32_t grpId)
  : McpttCallMsgFieldType6 (4),
    m_grpId (grpId)
{
  NS_LOG_FUNCTION (this << grpId);
}

McpttCallMsgFieldGrpId::~McpttCallMsgFieldGrpId (void)
{}

uint32_t
McpttCallMsgFieldGrpId::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint32_t bytesRead = McpttCallMsgFieldType6::Deserialize (buff);

  uint32_t grpId = buff.ReadNtohU32 ();
  bytesRead += 4;

  SetGrpId (grpId);

  return bytesRead;
}

TypeId
McpttCallMsgFieldGrpId::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldGrpId::GetTypeId ();
}

void
McpttCallMsgFieldGrpId::Print (std::ostream& os) const
{
  uint32_t grpId = GetGrpId ();

  os << "McpttCallMsgFieldGrpId(";

  McpttCallMsgFieldType6::Print (os);

  os << "grpId=" << grpId;
  os << ")";
}

void
McpttCallMsgFieldGrpId::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldType6::Serialize (buff);

  uint32_t grpId = GetGrpId ();

  buff.WriteHtonU32 (grpId);
}

uint32_t
McpttCallMsgFieldGrpId::GetGrpId (void) const
{
  return m_grpId;
}

void
McpttCallMsgFieldGrpId::SetGrpId (uint32_t grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  m_grpId = grpId;
}
/** McpttCallMsgFieldGrpId - end **/

/** McpttCallMsgFieldSdp - begin **/
TypeId
McpttCallMsgFieldSdp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldSdp")
    .SetParent<McpttCallMsgFieldType6> ()
    .AddConstructor<McpttCallMsgFieldSdp> ()
  ;

  return tid;
}

McpttCallMsgFieldSdp::McpttCallMsgFieldSdp (void)
  : McpttCallMsgFieldType6 (120),
    m_floorPort (0),
    m_grpAddr (Ipv4Address ()),
    m_origAddr (Ipv4Address ()),
    m_speechPort (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldSdp::~McpttCallMsgFieldSdp (void)
{}

uint32_t
McpttCallMsgFieldSdp::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint32_t bytesRead = McpttCallMsgFieldType6::Deserialize (buff);
  uint16_t length = GetLength ();

  char* characters = new char[length];
  for (uint32_t idx = 0; idx < length; idx++)
    {
      characters[idx] = (char)buff.ReadU8 ();

      bytesRead += 1;
    }

  std::string strRep = std::string (characters, length);

  FromStr (strRep);

  delete[] characters;

  return bytesRead;
}

void
McpttCallMsgFieldSdp::FromStr (const std::string& strRep)
{
  NS_LOG_FUNCTION (this << strRep);

  int idx = 0;
  int begIdx = 0;
  int endIdx = 0;
  int spaces = 0;
  int spaceLim = 4;
  char termChar = ' ';
  std::string grpAddrStr;
  std::string grpAddrTypeStr;
  std::string origAddrStr;
  std::string origAddrTypeStr;
  std::string floorPortStr;
  std::string speechPortStr;

  for (int step = 0; step < 6; step++)
    {
      while (spaces < spaceLim)
        {
          if (strRep[idx] == ' ')
            {
              spaces += 1;
            }

          idx += 1;
        }

      begIdx = idx;

      while (strRep[idx] != termChar)
        {
          idx += 1;
        }

      endIdx = idx;

      if (step == 0)
        {
          spaceLim = 5;
          termChar = ';';
          origAddrTypeStr = strRep.substr (begIdx, (endIdx - begIdx));
        }
      else if (step == 1)
        {
          spaceLim = 6;
          termChar = ' ';
          origAddrStr = strRep.substr (begIdx, (endIdx - begIdx));
        }
      else if (step == 2)
        {
          spaceLim = 7;
          termChar = '/';
          grpAddrTypeStr = strRep.substr (begIdx, (endIdx - begIdx));
        }
      else if (step == 3)
        {
          spaceLim = 8;
          termChar = ' ';
          grpAddrStr = strRep.substr (begIdx, (endIdx - begIdx));
        }
      else if (step == 4)
        {
          spaceLim = 11;
          termChar = ' ';
          speechPortStr = strRep.substr (begIdx, (endIdx - begIdx));
        }
      else if (step == 5)
        {
          floorPortStr = strRep.substr (begIdx, (endIdx - begIdx));
        }
    }

  if (origAddrTypeStr == "IP4")
    {
      SetOrigAddr (Ipv4Address (origAddrStr.c_str ()));
    }
  else if (origAddrTypeStr == "IP6")
    {
      SetOrigAddr (Ipv6Address (origAddrStr.c_str ()));
    }
  else
    {
      NS_ABORT_MSG ("Only Ipv4 and Ipv6 address are supported.");
    }

  if (grpAddrTypeStr == "IP4")
    {
      SetGrpAddr (Ipv4Address (grpAddrStr.c_str ()));
    }
  else if (grpAddrTypeStr == "IP6")
    {
      SetGrpAddr (Ipv6Address (grpAddrStr.c_str ()));
    }
  else
    {
      NS_ABORT_MSG ("Only Ipv4 and Ipv6 address are supported.");
    }

  SetSpeechPort ((uint16_t)atoi (speechPortStr.c_str ()));
  SetFloorPort ((uint16_t)atoi (floorPortStr.c_str ()));
}

TypeId
McpttCallMsgFieldSdp::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldSdp::GetTypeId ();
}

void
McpttCallMsgFieldSdp::Print (std::ostream& os) const
{
  std::string strRep = ToStr ();

  os << "McpttCallMsgFieldSdp(";

  McpttCallMsgFieldType6::Print (os);

  os << ";";
  os << strRep;
  os << ")";
}

void
McpttCallMsgFieldSdp::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldType6::Serialize (buff);

  std::string strRep = ToStr ();

  for (uint32_t idx = 0; idx < strRep.length (); idx++)
    {
      buff.WriteU8 ((uint8_t)strRep[idx]);
    }
}

std::string
McpttCallMsgFieldSdp::ToStr (void) const
{
  std::stringstream ss;
  std::string grpAddrType;
  std::string origAddrType;
  std::stringstream grpAddrStr;
  std::stringstream origAddrStr;
  uint16_t floorPort = GetFloorPort ();
  Address grpAddr = GetGrpAddr ();
  Address origAddr = GetOrigAddr ();
  uint16_t speechPort = GetSpeechPort ();

  if (Ipv4Address::IsMatchingType (grpAddr))
    {
      grpAddrType = "IP4";
      grpAddrStr << Ipv4Address::ConvertFrom (grpAddr);
    }
  else if (Ipv6Address::IsMatchingType (grpAddr))
    {
      grpAddrType = "IP6";
      grpAddrStr << Ipv6Address::ConvertFrom (grpAddr);
    }
  else
    {
      NS_ABORT_MSG ("Only Ipv4 and Ipv6 address are supported.");
    }

  if (Ipv4Address::IsMatchingType (origAddr))
    {
      origAddrType = "IP4";
      origAddrStr << Ipv4Address::ConvertFrom (origAddr);
    }
  else if (Ipv6Address::IsMatchingType (origAddr))
    {
      origAddrType = "IP6";
      origAddrStr << Ipv6Address::ConvertFrom (origAddr);
    }
  else
    {
      NS_ABORT_MSG ("Only Ipv4 and Ipv6 address are supported.");
    }

  ss << "v=0;"; // 4 characters
  ss << "o=- 0 0 IN " << origAddrType << " " << origAddrStr.str () << ";"; // 23 characters
  ss << "s=-;"; // 4 characters
  ss << "c=IN " << grpAddrType << " " << grpAddrStr.str () << "/255;"; // 21 characters
  ss << "m=audio " << (uint32_t)speechPort << " RTP/AVP 0;"; // 20 characters
  ss << "i=speech;"; // 9 characters
  ss << "m=application " << (uint32_t)floorPort << " udp MCPTT;"; // 26 characters
  ss << "a=fmtp:MCPTT;"; // 13 characters

  return ss.str ();
}

void
McpttCallMsgFieldSdp::UpdateLength (void)
{
  NS_LOG_FUNCTION (this);

  std::string rep = ToStr ();
  uint16_t length = rep.length ();

  SetLength (length);
}

uint16_t
McpttCallMsgFieldSdp::GetFloorPort (void) const
{
  return m_floorPort;
}

Address
McpttCallMsgFieldSdp::GetGrpAddr (void) const
{
  return m_grpAddr;
}

Address
McpttCallMsgFieldSdp::GetOrigAddr (void) const
{
  return m_origAddr;
}

uint16_t
McpttCallMsgFieldSdp::GetSpeechPort (void) const
{
  return m_speechPort;
}

void
McpttCallMsgFieldSdp::SetFloorPort (uint16_t floorPort)
{
  NS_LOG_FUNCTION (this << (uint32_t)floorPort);

  m_floorPort = floorPort;

  UpdateLength ();
}

void
McpttCallMsgFieldSdp::SetGrpAddr (const Address& grpAddr)
{
  NS_LOG_FUNCTION (this << grpAddr);

  m_grpAddr = grpAddr;

  UpdateLength ();
}

void
McpttCallMsgFieldSdp::SetOrigAddr (const Address& origAddr)
{
  NS_LOG_FUNCTION (this << origAddr);

  m_origAddr = origAddr;

  UpdateLength ();
}

void
McpttCallMsgFieldSdp::SetSpeechPort (uint16_t speechPort)
{
  NS_LOG_FUNCTION (this << (uint32_t)speechPort);

  m_speechPort = speechPort;

  UpdateLength ();
}
/** McpttCallMsgFieldSdp - end **/

/** McpttCallMsgFieldCommMode - begin **/
const uint8_t McpttCallMsgFieldCommMode::AUTOMATIC_MODE = 0;

const uint8_t McpttCallMsgFieldCommMode::MANUAL_MODE = 1;

TypeId
McpttCallMsgFieldCommMode::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldCommMode")
    .SetParent<McpttCallMsgField> ()
    .AddConstructor<McpttCallMsgFieldCommMode> ()
  ;

  return tid;
}

McpttCallMsgFieldCommMode::McpttCallMsgFieldCommMode (void)
  : McpttCallMsgField (),
    m_mode (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldCommMode::McpttCallMsgFieldCommMode (uint8_t commMode)
  : McpttCallMsgField (),
    m_mode (commMode)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldCommMode::~McpttCallMsgFieldCommMode (void)
{}

uint32_t
McpttCallMsgFieldCommMode::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint32_t bytesRead = 0;
  uint8_t mode = GetMode ();

  mode = buff.ReadU8 ();
  bytesRead += 1;

  SetMode (mode);

  return bytesRead;
}

TypeId
McpttCallMsgFieldCommMode::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldCommMode::GetTypeId ();
}

uint32_t
McpttCallMsgFieldCommMode::GetSerializedSize (void) const
{
  return 1; // One byte for mode value.
}

void
McpttCallMsgFieldCommMode::Print (std::ostream& os) const
{
  uint8_t mode = GetMode ();

  os << "McpttCallMsgFieldCommMode(";
  os << "mode=" << (uint32_t)mode;
  os << ")";
}

void
McpttCallMsgFieldCommMode::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  uint8_t mode = GetMode ();

  buff.WriteU8 (mode);
}

uint8_t
McpttCallMsgFieldCommMode::GetMode (void) const
{
  return m_mode;
}

void
McpttCallMsgFieldCommMode::SetMode (uint8_t mode)
{
  NS_LOG_FUNCTION (this << (uint8_t)mode);

  m_mode = mode;
}
/** McpttCallMsgFieldCommMode - end **/

/** McpttCallMsgFieldReason - begin **/
const uint8_t McpttCallMsgFieldReason::REJECT = 0;

const uint8_t McpttCallMsgFieldReason::NO_ANSWER = 1;

const uint8_t McpttCallMsgFieldReason::BUSY = 2;

const uint8_t McpttCallMsgFieldReason::SECURITY_FAILURE = 3;

const uint8_t McpttCallMsgFieldReason::FAILED = 4;

TypeId
McpttCallMsgFieldReason::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldReason")
    .SetParent<McpttCallMsgField> ()
    .AddConstructor<McpttCallMsgFieldReason> ()
  ;

  return tid;
}

McpttCallMsgFieldReason::McpttCallMsgFieldReason (void)
  : McpttCallMsgField (),
    m_reason (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldReason::McpttCallMsgFieldReason (uint8_t reason)
  : McpttCallMsgField (),
    m_reason (reason)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldReason::~McpttCallMsgFieldReason (void)
{}

uint32_t
McpttCallMsgFieldReason::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint32_t bytesRead = 0;

  uint8_t reason = buff.ReadU8 ();
  bytesRead += 1;

  SetReason (reason);

  return bytesRead;
}

TypeId
McpttCallMsgFieldReason::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldReason::GetTypeId ();
}

uint32_t
McpttCallMsgFieldReason::GetSerializedSize (void) const
{
  return 1; // One byte for the reason value.
}

void
McpttCallMsgFieldReason::Print (std::ostream& os) const
{
  uint8_t reason = GetReason ();

  os << "McpttCallMsgFieldReason(";
  os << "reason=" << (uint32_t)reason;
  os << ")";
}

void
McpttCallMsgFieldReason::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  uint8_t reason = GetReason ();

  buff.WriteU8 (reason);
}

uint8_t
McpttCallMsgFieldReason::GetReason (void) const
{
  return m_reason;
}

void
McpttCallMsgFieldReason::SetReason (uint8_t reason)
{
  NS_LOG_FUNCTION (this << (uint32_t)reason);

  m_reason = reason;
}
/** McpttCallMsgFieldReason - end **/

/** McpttCallMsgFieldConfirmMode - begin **/
uint8_t
McpttCallMsgFieldConfirmMode::GetIei (void)
{
  return 80;
}

TypeId
McpttCallMsgFieldConfirmMode::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldConfirmMode")
    .SetParent<McpttCallMsgField> ()
    .AddConstructor <McpttCallMsgFieldConfirmMode> ()
  ;

  return tid;
}

McpttCallMsgFieldConfirmMode::McpttCallMsgFieldConfirmMode (void)
  : McpttCallMsgField ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldConfirmMode::~McpttCallMsgFieldConfirmMode (void)
{}

uint32_t
McpttCallMsgFieldConfirmMode::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint32_t bytesRead = 0;

  uint8_t iei = buff.ReadU8 (); // Read IEI.
  bytesRead += 1;

  NS_ASSERT_MSG (iei == McpttCallMsgFieldConfirmMode::GetIei (), "IEI \"" << (uint32_t)iei << "\" is not for " << GetInstanceTypeId ().GetName () << ".");

  return bytesRead;
}

TypeId
McpttCallMsgFieldConfirmMode::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldConfirmMode::GetTypeId ();
}

uint32_t
McpttCallMsgFieldConfirmMode::GetSerializedSize (void) const
{
  return 1; // One byte for the indication.
}

void
McpttCallMsgFieldConfirmMode::Print (std::ostream& os) const
{
  os << "McpttCallMsgFieldConfirmMode(";
  os << "iei=" << (uint32_t)McpttCallMsgFieldConfirmMode::GetIei ();
  os << ")";
}

void
McpttCallMsgFieldConfirmMode::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  uint8_t iei = McpttCallMsgFieldConfirmMode::GetIei ();

  buff.WriteU8 (iei);
}
/** McpttCallMsgFieldConfirmMode - end **/

/** McpttCallMsgFieldUserId - begin **/
TypeId
McpttCallMsgFieldUserId::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldUserId")
    .SetParent<McpttCallMsgFieldType6> ()
    .AddConstructor<McpttCallMsgFieldUserId> ()
  ;

  return tid;
}

McpttCallMsgFieldUserId::McpttCallMsgFieldUserId (void)
  : McpttCallMsgFieldType6 (4),
    m_id (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldUserId::McpttCallMsgFieldUserId (uint32_t id)
  : McpttCallMsgFieldType6 (4),
    m_id (id)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldUserId::~McpttCallMsgFieldUserId (void)
{}

uint32_t
McpttCallMsgFieldUserId::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint32_t bytesRead = McpttCallMsgFieldType6::Deserialize (buff);

  uint32_t id = buff.ReadNtohU32 ();
  bytesRead += 4;

  SetId (id);

  return bytesRead;
}

TypeId
McpttCallMsgFieldUserId::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldUserId::GetTypeId ();
}

void
McpttCallMsgFieldUserId::Print (std::ostream& os) const
{
  uint32_t id = GetId ();

  os << "McpttCallMsgFieldUserId(";

  McpttCallMsgFieldType6::Print (os);

  os << "id=" << id;
  os << ")";
}

void
McpttCallMsgFieldUserId::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldType6::Serialize (buff);

  uint32_t id = GetId ();

  buff.WriteHtonU32 (id);
}

uint32_t
McpttCallMsgFieldUserId::GetId (void) const
{
  return m_id;
}

void
McpttCallMsgFieldUserId::SetId (uint32_t id)
{
  NS_LOG_FUNCTION (this << id);

  m_id = id;
}
/** McpttCallMsgFieldUserId - end **/

/** McpttCallMsgFieldCallType - begin **/
const uint8_t McpttCallMsgFieldCallType::BASIC_GROUP = 1;

const uint8_t McpttCallMsgFieldCallType::BROADCAST_GROUP = 2;

const uint8_t McpttCallMsgFieldCallType::EMERGENCY_GROUP = 3;

const uint8_t McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP = 4;

const uint8_t McpttCallMsgFieldCallType::PRIVATE = 5;

const uint8_t McpttCallMsgFieldCallType::EMERGENCY_PRIVATE = 6;

std::string
McpttCallMsgFieldCallType::GetCallTypeName (uint8_t type)
{
  std::string callTypeName = "N/A";

  if (type == McpttCallMsgFieldCallType::BASIC_GROUP)
    {
      callTypeName = "Basic";
    }
  else if (type == McpttCallMsgFieldCallType::BROADCAST_GROUP)
    {
      callTypeName = "Broadcast";
    }
  else if (type == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
    {
      callTypeName = "Emergency";
    }
  else if (type == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
    {
      callTypeName = "Imminent Peril";
    }
  else if (type == McpttCallMsgFieldCallType::PRIVATE)
    {
      callTypeName = "Private'";
    }
  else if (type == McpttCallMsgFieldCallType::EMERGENCY_PRIVATE)
    {
      callTypeName = "Private Emergency";
    }

  return callTypeName;
}

uint8_t
McpttCallMsgFieldCallType::GetCallTypePriority (uint8_t type)
{
  uint8_t priority = 0;

  if (type == McpttCallMsgFieldCallType::BASIC_GROUP)
    {
      priority = 1;
    }
  else if (type == McpttCallMsgFieldCallType::BROADCAST_GROUP)
    {
      priority = 1;
    }
  else if (type == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
    {
      priority = 1;
    }
  else if (type == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
    {
      priority = 1;
    }
  else if (type == McpttCallMsgFieldCallType::PRIVATE)
    {
      priority = 1;
    }
  else if (type == McpttCallMsgFieldCallType::EMERGENCY_PRIVATE)
    {
      priority = 1;
    }

  return priority;
}

TypeId
McpttCallMsgFieldCallType::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldCallType")
    .SetParent<McpttCallMsgField> ()
    .AddConstructor<McpttCallMsgFieldCallType> ()
  ;

  return tid;
}

McpttCallMsgFieldCallType::McpttCallMsgFieldCallType (void)
  : McpttCallMsgField (),
    m_type (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldCallType::McpttCallMsgFieldCallType (uint8_t type)
  : McpttCallMsgField (),
    m_type (type)
{
  NS_LOG_FUNCTION (this << (uint32_t)type);
}

McpttCallMsgFieldCallType::~McpttCallMsgFieldCallType (void)
{}

uint32_t
McpttCallMsgFieldCallType::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  uint32_t bytesRead = 0;

  uint8_t type = buff.ReadU8 ();
  bytesRead += 1;

  SetType (type);

  return bytesRead;
}

TypeId
McpttCallMsgFieldCallType::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldCallType::GetTypeId ();
}

uint32_t
McpttCallMsgFieldCallType::GetSerializedSize (void) const
{
  return 1; // One byte for the call type representation byte.
}

void
McpttCallMsgFieldCallType::Print (std::ostream& os) const
{
  uint8_t type = GetType ();

  os << "McpttCallMsgFieldCallType(";
  os << "type=" << (uint32_t)type;
  os << ")";
}

void
McpttCallMsgFieldCallType::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  uint8_t type = GetType ();

  buff.WriteU8 (type);
}

uint8_t
McpttCallMsgFieldCallType::GetType (void) const
{
  return m_type;
}

void
McpttCallMsgFieldCallType::SetType (uint8_t type)
{
  NS_LOG_FUNCTION (this << (uint32_t)type);

  m_type = type;
}
/** McpttCallMsgFieldCallType - end **/

/** McpttCallMsgFieldUserLoc - begin **/
uint8_t
McpttCallMsgFieldUserLoc::GetIei (void)
{
  return 78;
}

TypeId
McpttCallMsgFieldUserLoc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldUserLoc")
    .SetParent<McpttCallMsgFieldType6> ()
    .AddConstructor<McpttCallMsgFieldUserLoc> ()
  ;

  return tid;
}

McpttCallMsgFieldUserLoc::McpttCallMsgFieldUserLoc (void)
  : McpttCallMsgFieldType6 (sizeof (double) * 3),
    m_loc (Vector ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldUserLoc::McpttCallMsgFieldUserLoc (const Vector& loc)
  : McpttCallMsgFieldType6 (sizeof (double) * 3),
    m_loc (loc)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldUserLoc::~McpttCallMsgFieldUserLoc (void)
{}

uint32_t
McpttCallMsgFieldUserLoc::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  Vector loc;
  double temp;
  uint8_t bytesRead = 0;
  char bytes[sizeof (double)];

  uint8_t iei = buff.ReadU8 (); // Read IEI.
  bytesRead += 1;

  NS_ASSERT_MSG (iei == McpttCallMsgFieldUserLoc::GetIei (), "IEI \"" << (uint32_t)iei << "\" is not for " << GetInstanceTypeId ().GetName () << ".");

  bytesRead += McpttCallMsgFieldType6::Deserialize (buff);

  for (uint32_t i = 0; i < 3; i++)
    {
      for (uint32_t idx = 0; idx < sizeof (double); idx++)
        {
          bytes[idx] = (char)buff.ReadU8 ();
          bytesRead += 1;
        }

      memcpy (&temp, bytes, sizeof (double));

      if (i == 0)
        {
          loc.x = temp;
        }
      else if (i == 1)
        {
          loc.y = temp;
        }
      else if (i == 2)
        {
          loc.z = temp;
        }
    }

  SetLoc (loc);

  return bytesRead;
}

uint32_t
McpttCallMsgFieldUserLoc::GetSerializedSize (void) const
{
  uint32_t size = McpttCallMsgFieldType6::GetSerializedSize ();
  size += 1; // Add one byte for IEI

  return size;
}

TypeId
McpttCallMsgFieldUserLoc::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldUserLoc::GetTypeId ();
}

void
McpttCallMsgFieldUserLoc::Print (std::ostream& os) const
{
  Vector loc = GetLoc ();

  os << "McpttCallMsgFieldUserLoc(";
  os << "iei=" << (uint32_t)McpttCallMsgFieldUserLoc::GetIei ();
  os << ";";

  McpttCallMsgFieldType6::Print (os);

  os << ";loc=" << loc;
  os << ")";
}

void
McpttCallMsgFieldUserLoc::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  double temp = 0;
  char bytes[sizeof(double)];
  Vector loc = GetLoc ();
  uint8_t iei = McpttCallMsgFieldUserLoc::GetIei ();

  buff.WriteU8 (iei);

  McpttCallMsgFieldType6::Serialize (buff);

  for (uint32_t i = 0; i < 3; i++)
    {
      if (i == 0)
        {
          temp = loc.x;
        }
      else if (i == 1)
        {
          temp = loc.y;
        }
      else if (i == 2)
        {
          temp = loc.z;
        }

      memcpy (bytes, &temp, sizeof (double));

      for (uint32_t idx = 0; idx < sizeof (double); idx++)
        {
          buff.WriteU8 ((uint8_t)bytes[idx]);
        }
    }
}

Vector
McpttCallMsgFieldUserLoc::GetLoc (void) const
{
  return m_loc;
}

void
McpttCallMsgFieldUserLoc::SetLoc (const Vector& loc)
{
  NS_LOG_FUNCTION (this << loc);

  m_loc = loc;
}
/** McpttCallMsgFieldUserLoc - end **/

/** McpttCallMsgFieldOrgName - begin **/
TypeId
McpttCallMsgFieldOrgName::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldOrgName")
    .SetParent<McpttCallMsgFieldType6> ()
    .AddConstructor<McpttCallMsgFieldOrgName> ()
  ;

  return tid;
}

McpttCallMsgFieldOrgName::McpttCallMsgFieldOrgName (void)
  : McpttCallMsgFieldType6 (),
    m_name ("")
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldOrgName::McpttCallMsgFieldOrgName (const std::string& name)
  : McpttCallMsgFieldType6 (),
    m_name (name)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldOrgName::~McpttCallMsgFieldOrgName (void)
{}

uint32_t
McpttCallMsgFieldOrgName::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  std::string name ("");
  uint32_t bytesRead = McpttCallMsgFieldType6::Deserialize (buff);
  uint16_t length = GetLength ();

  for (uint32_t idx = 0; idx < length; idx++)
    {
      name += (char)buff.ReadU8 ();
    }

  UpdateName (name);

  return bytesRead;
}

TypeId
McpttCallMsgFieldOrgName::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldOrgName::GetTypeId ();
}

void
McpttCallMsgFieldOrgName::Print (std::ostream& os) const
{
  std::string name = GetName ();

  os << "McpttCallMsgFieldOrgName(";

  McpttCallMsgFieldType6::Print (os);

  os << "name=" << name;
  os << ")";
}

void
McpttCallMsgFieldOrgName::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  McpttCallMsgFieldType6::Serialize (buff);

  std::string name = GetName ();

  for (uint32_t idx = 0; idx < name.length (); idx++)
    {
      buff.WriteU8 ((uint8_t)name[idx]);
    }
}

void
McpttCallMsgFieldOrgName::UpdateName (const std::string& name)
{
  NS_LOG_FUNCTION (this << name);

  m_name = name;
}

std::string
McpttCallMsgFieldOrgName::GetName (void) const
{
  return m_name;
}

void
McpttCallMsgFieldOrgName::SetName (const std::string& name)
{
  NS_LOG_FUNCTION (this << name);

  uint16_t length = GetLength ();
  length -= m_name.length ();
  length += name.length ();

  SetLength (length);
  UpdateName (name);
}
/** McpttCallMsgFieldOrgName - end **/

/** McpttCallMsgFieldStartTime - begin **/
TypeId
McpttCallMsgFieldStartTime::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldStartTime")
    .SetParent<McpttCallMsgField> ()
    .AddConstructor<McpttCallMsgFieldStartTime> ()
  ;

  return tid;
}

McpttCallMsgFieldStartTime::McpttCallMsgFieldStartTime (void)
  : McpttCallMsgField (),
    m_time (Time ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldStartTime::McpttCallMsgFieldStartTime (const Time& time)
  : McpttCallMsgField (),
    m_time (time)
{
  NS_LOG_FUNCTION (this << time);
}

McpttCallMsgFieldStartTime::~McpttCallMsgFieldStartTime (void)
{}

uint32_t
McpttCallMsgFieldStartTime::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint32_t bytesRead = 0;

  uint32_t seconds = buff.ReadNtohU32 ();
  bytesRead += 4;
  buff.ReadU8 (); // dummy byte
  bytesRead++;

  SetTime (MilliSeconds (seconds));

  return bytesRead;
}

TypeId
McpttCallMsgFieldStartTime::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldStartTime::GetTypeId ();
}

uint32_t
McpttCallMsgFieldStartTime::GetSerializedSize (void) const
{
  // Even though only four bytes are actually stored here, the size of the
  // field is supposed to be five byes, so an extra dummy byte will be
  // included in serialization and deserialization.
  return 5;
}

void
McpttCallMsgFieldStartTime::Print (std::ostream& os) const
{
  Time t = GetTime ();

  os << "McpttCallMsgFieldStartTime(";
  os << "time=" << t;
  os << ")";
}

void
McpttCallMsgFieldStartTime::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  Time t = GetTime ();

  uint32_t seconds = (uint32_t)t.GetMilliSeconds ();

  buff.WriteHtonU32 (seconds);
  buff.WriteU8 (0); // Extra dummy byte
}

Time
McpttCallMsgFieldStartTime::GetTime (void) const
{
  return m_time;
}

void
McpttCallMsgFieldStartTime::SetTime (const Time& time)
{
  NS_LOG_FUNCTION (this << time);

  m_time = time;
}
/** McpttCallMsgFieldStartTime - end **/

/** McpttCallMsgFieldLastChgTime - begin **/
TypeId
McpttCallMsgFieldLastChgTime::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldLastChgTime")
    .SetParent<McpttCallMsgField> ()
    .AddConstructor<McpttCallMsgFieldLastChgTime> ()
  ;

  return tid;
}

McpttCallMsgFieldLastChgTime::McpttCallMsgFieldLastChgTime (void)
  : McpttCallMsgField (),
    m_time (Time ())
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldLastChgTime::McpttCallMsgFieldLastChgTime (const Time& time)
  : McpttCallMsgField (),
    m_time (time)
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldLastChgTime::~McpttCallMsgFieldLastChgTime (void)
{}

uint32_t
McpttCallMsgFieldLastChgTime::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint32_t bytesRead = 0;

  uint32_t seconds = buff.ReadNtohU32 ();
  bytesRead += 4;
  buff.ReadU8 (); // dummy byte
  bytesRead++;

  SetTime (MilliSeconds (seconds));

  return bytesRead;
}

TypeId
McpttCallMsgFieldLastChgTime::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldLastChgTime::GetTypeId ();
}

uint32_t
McpttCallMsgFieldLastChgTime::GetSerializedSize (void) const
{
  // Even though only four bytes are actually stored here, the size of the
  // field is supposed to be five byes, so an extra dummy byte will be
  // included in serialization and deserialization.
  return 5;
}

void
McpttCallMsgFieldLastChgTime::Print (std::ostream& os) const
{
  Time t = GetTime ();

  os << "McpttCallMsgFieldLastChgTime(";
  os << "time=" << t;
  os << ")";
}

void
McpttCallMsgFieldLastChgTime::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  Time t = GetTime ();

  uint32_t seconds = (uint32_t)t.GetMilliSeconds ();

  buff.WriteHtonU32 (seconds);
  buff.WriteU8 (0); // Extra dummy byte
}

Time
McpttCallMsgFieldLastChgTime::GetTime (void) const
{
  return m_time;
}

void
McpttCallMsgFieldLastChgTime::SetTime (const Time& time)
{
  NS_LOG_FUNCTION (this << time);

  m_time = time;
}
/** McpttCallMsgFieldLastChgTime - end **/

/** McpttCallMsgFieldProbeResp - begin **/
uint8_t
McpttCallMsgFieldProbeResp::GetIei (void)
{
  return 81;
}

TypeId
McpttCallMsgFieldProbeResp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCallMsgFieldProbeResp")
    .SetParent<McpttCallMsgField> ()
    .AddConstructor<McpttCallMsgFieldProbeResp> ()
  ;

  return tid;
}

McpttCallMsgFieldProbeResp::McpttCallMsgFieldProbeResp (void)
  : McpttCallMsgField ()
{
  NS_LOG_FUNCTION (this);
}

McpttCallMsgFieldProbeResp::~McpttCallMsgFieldProbeResp (void)
{}

uint32_t
McpttCallMsgFieldProbeResp::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint32_t bytesRead = 0;

  uint8_t iei = buff.ReadU8 (); // Read IEI.
  bytesRead += 1;

  NS_ASSERT_MSG (iei == McpttCallMsgFieldProbeResp::GetIei (), "IEI \"" << (uint32_t)iei << "\" is not for " << GetInstanceTypeId ().GetName () << ".");

  return bytesRead;
}

TypeId
McpttCallMsgFieldProbeResp::GetInstanceTypeId (void) const
{
  return McpttCallMsgFieldProbeResp::GetTypeId ();
}

uint32_t
McpttCallMsgFieldProbeResp::GetSerializedSize (void) const
{
  return 1; // One byte for the IEI.
}

void
McpttCallMsgFieldProbeResp::Print (std::ostream& os) const
{
  os << "McpttCallMsgFieldProbeResp(";
  os << "iei=" << (uint32_t)McpttCallMsgFieldProbeResp::GetIei ();
  os << ")";
}

void
McpttCallMsgFieldProbeResp::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  uint8_t iei = McpttCallMsgFieldProbeResp::GetIei ();

  buff.WriteU8 (iei);
}

/** McpttCallMsgFieldProbeResp - end **/

} // namespace psc
} // namespace ns3

