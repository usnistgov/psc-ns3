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
#include <vector>

#include <ns3/buffer.h>
#include <ns3/log.h>
#include <ns3/object-base.h>
#include <ns3/type-id.h>

#include "mcptt-floor-msg-field.h"

namespace ns3 {

namespace psc {

/** McpttFloorMsgField - begin **/
NS_LOG_COMPONENT_DEFINE ("McpttFloorMsgField");

NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgField);

TypeId
McpttFloorMsgField::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgField")
    .SetParent<ObjectBase> ()
    .AddConstructor<McpttFloorMsgField> ()
  ;

  return tid;
}

McpttFloorMsgField::McpttFloorMsgField (void)
  : m_id (0),
    m_length (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgField::McpttFloorMsgField (uint8_t id, uint8_t length)
  : m_id (id),
    m_length (length)
{
  NS_LOG_FUNCTION (this << (uint32_t)id << (uint32_t) length);
}

McpttFloorMsgField::~McpttFloorMsgField (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgField::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgField deserializing...");

  uint32_t bytesRead = 0;

  uint8_t id = buff.ReadU8 ();
  bytesRead += 1;

  uint8_t length = buff.ReadU8 ();
  bytesRead += 1;

  NS_LOG_LOGIC ("Read " << bytesRead << " bytes (id=" << (uint32_t)id << ";length=" << (uint32_t)length << ").");

  SetId (id);
  SetLength (length);

  return bytesRead;
}

TypeId
McpttFloorMsgField::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgField::GetTypeId ();
}

uint32_t
McpttFloorMsgField::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);

  uint8_t length = GetLength ();

  // size = the length of the message plus the two additional bytes needed to
  // store the ID and length values.
  uint32_t size = length + 2;

  NS_LOG_LOGIC ("McpttFloorMsgField serialized size = " << size << " bytes.");

  return size;
}

void
McpttFloorMsgField::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  uint8_t id = GetId ();
  uint8_t length = GetLength ();

  os << "McpttFloorMsgField(";
  os << "id=" << (uint32_t)id << ",";
  os << "length=" << (uint32_t)length;
  os << ")";
}

void
McpttFloorMsgField::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("MsgField serializing...");

  uint8_t id = GetId ();
  uint8_t length = GetLength ();

  buff.WriteU8 (id);
  buff.WriteU8 (length);

  NS_LOG_LOGIC ("Wrote two bytes (id=" << (uint32_t)id << ";length=" << (uint32_t)length << ";).");
}

uint8_t
McpttFloorMsgField::GetId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_id;
}

uint8_t
McpttFloorMsgField::GetLength (void) const
{
  NS_LOG_FUNCTION (this);

  return m_length;
}

void
McpttFloorMsgField::SetId (uint8_t id)
{
  NS_LOG_FUNCTION (this << id);

  m_id = id;
}

void
McpttFloorMsgField::SetLength (uint8_t length)
{
  NS_LOG_FUNCTION (this << length);

  m_length = length;
}
/** McpttFloorMsgField - end **/

std::ostream&
operator<< (std::ostream& os, const McpttFloorMsgField& field)
{
  field.Print (os);

  return os;
}

/** McpttFloorMsgFieldPriority - being **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldPriority);

TypeId
McpttFloorMsgFieldPriority::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldPriority")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldPriority> ()
  ;

  return tid;
}

McpttFloorMsgFieldPriority::McpttFloorMsgFieldPriority (void)
  : McpttFloorMsgField (0, 2),
    m_priority (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldPriority::McpttFloorMsgFieldPriority (uint8_t priority)
  : McpttFloorMsgField (0, 2),
    m_priority (priority)
{
  NS_LOG_FUNCTION (this << (uint32_t)priority);
}

McpttFloorMsgFieldPriority::~McpttFloorMsgFieldPriority (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldPriority::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("FloorMsgFieldPriority deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint8_t priority = buff.ReadU8 ();
  bytesRead += 1;

  buff.ReadU8 (); // Read "spare" byte.
  bytesRead += 1;

  NS_LOG_LOGIC ("Read two bytes (priority=" << (uint32_t) priority << ").");

  SetPriority (priority);

  return bytesRead;
}

TypeId
McpttFloorMsgFieldPriority::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldPriority::GetTypeId ();
}

void
McpttFloorMsgFieldPriority::Print (std::ostream& os) const
{
  NS_LOG_LOGIC (this);

  os << "McpttFloorMsgFieldPriority(";

  McpttFloorMsgField::Print (os);

  uint8_t priority = GetPriority ();

  os << ";";
  os << "priority=" << (uint32_t)priority;
  os << ")";
}

void
McpttFloorMsgFieldPriority::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("FloorMsgFieldPriority serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint8_t priority = GetPriority ();

  buff.WriteU8 (priority);
  buff.WriteU8 (0); // Write "spare" byte.

  NS_LOG_LOGIC ("FloorMsgFieldPriority wrote two bytes (priority=" << (uint32_t)priority << ").");
}

uint8_t
McpttFloorMsgFieldPriority::GetPriority () const
{
  NS_LOG_FUNCTION (this);

  return m_priority;
}

void
McpttFloorMsgFieldPriority::SetPriority (uint8_t priority)
{
  NS_LOG_FUNCTION (this << (uint32_t)priority);

  m_priority = priority;
}

/** McpttFloorMsgFieldPriority - end **/

/** McpttFloorMsgFieldDuration - begin **/

NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldDuration);

TypeId
McpttFloorMsgFieldDuration::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldDuration")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldDuration> ()
  ;

  return tid;
}

McpttFloorMsgFieldDuration::McpttFloorMsgFieldDuration ()
  : McpttFloorMsgField (1, 2),
    m_duration (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldDuration::McpttFloorMsgFieldDuration (uint16_t duration)
  : McpttFloorMsgField (1, 2),
    m_duration (duration)
{
  NS_LOG_FUNCTION (this << (uint32_t)duration);
}

McpttFloorMsgFieldDuration::~McpttFloorMsgFieldDuration (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldDuration::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldDuration deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint16_t duration = buff.ReadNtohU16 ();
  bytesRead += 2;

  NS_LOG_LOGIC ("McpttDuraionField read two bytes (duration=" << (uint32_t)duration << ").");

  SetDuration (duration);

  return bytesRead;
}

TypeId
McpttFloorMsgFieldDuration::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldDuration::GetTypeId ();
}

void
McpttFloorMsgFieldDuration::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldDuration(";

  McpttFloorMsgField::Print (os);

  uint16_t duration = GetDuration ();

  os << ";";
  os << "duration=" << (uint32_t)duration;
  os << ")";
}

void
McpttFloorMsgFieldDuration::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldDuration serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint16_t duration = GetDuration ();

  buff.WriteHtonU16 (duration);

  NS_LOG_LOGIC ("McpttFloorMsgFieldDuration wrote two bytes (duration=" << (uint32_t)duration << ").");
}

uint16_t
McpttFloorMsgFieldDuration::GetDuration (void) const
{
  NS_LOG_FUNCTION (this);

  return m_duration;
}

void
McpttFloorMsgFieldDuration::SetDuration (uint16_t duration)
{
  NS_LOG_FUNCTION (this << (uint32_t)duration);

  m_duration = duration;
}
/** McpttFloorMsgFieldDuration - end **/

/** McpttFloorMsgFieldRejectCause - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldRejectCause);

const uint16_t McpttFloorMsgFieldRejectCause::CAUSE_1 = 1;
const uint16_t McpttFloorMsgFieldRejectCause::CAUSE_2 = 2;
const uint16_t McpttFloorMsgFieldRejectCause::CAUSE_3 = 3;
const uint16_t McpttFloorMsgFieldRejectCause::CAUSE_4 = 4;
const uint16_t McpttFloorMsgFieldRejectCause::CAUSE_5 = 5;
const uint16_t McpttFloorMsgFieldRejectCause::CAUSE_6 = 6;
const uint16_t McpttFloorMsgFieldRejectCause::CAUSE_7 = 7;
const uint16_t McpttFloorMsgFieldRejectCause::CAUSE_255 = 255;

TypeId
McpttFloorMsgFieldRejectCause::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldRejectCause")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldRejectCause> ()
  ;

  return tid;
}

McpttFloorMsgFieldRejectCause::McpttFloorMsgFieldRejectCause (void)
  : McpttFloorMsgField (2, 2),
    m_cause (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldRejectCause::McpttFloorMsgFieldRejectCause (uint16_t cause)
  : McpttFloorMsgField (2, 2),
    m_cause (cause)
{
  NS_LOG_FUNCTION (this << (uint32_t)cause);
}

McpttFloorMsgFieldRejectCause::~McpttFloorMsgFieldRejectCause (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldRejectCause::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttRejectCause deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint16_t cause = buff.ReadNtohU16 ();
  bytesRead += 2;

  NS_LOG_LOGIC ("McpttRejectCause read two bytes (cause=" << (uint32_t)cause << ").");

  SetCause (cause);

  return bytesRead;
}

TypeId
McpttFloorMsgFieldRejectCause::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldRejectCause::GetTypeId ();
}

void
McpttFloorMsgFieldRejectCause::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldRejectCause(";

  McpttFloorMsgField::Print (os);

  uint16_t cause = GetCause ();

  os << ";";
  os << "cause=" << (uint32_t)cause;
  os << ")";
}

void
McpttFloorMsgFieldRejectCause::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldRejectCause serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint16_t cause = GetCause ();

  buff.WriteHtonU16 (cause);

  NS_LOG_LOGIC ("McpttFloorMsgFieldRejectCause wrote two bytes (cause=" << (uint32_t)cause << ").");
}

uint16_t
McpttFloorMsgFieldRejectCause::GetCause (void) const
{
  NS_LOG_FUNCTION (this);

  return m_cause;
}

void
McpttFloorMsgFieldRejectCause::SetCause (uint16_t cause)
{
  NS_LOG_FUNCTION (this << (uint32_t)cause);

  m_cause = cause;
}
/** McpttFloorMsgFieldRejectCause - end **/

/** McpttFloorMsgFieldQueuePositionInfo - begin **/

NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldQueuePositionInfo);

TypeId
McpttFloorMsgFieldQueuePositionInfo::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldQueuePositionInfo")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldQueuePositionInfo> ()
  ;

  return tid;
}

McpttFloorMsgFieldQueuePositionInfo::McpttFloorMsgFieldQueuePositionInfo (void)
  : McpttFloorMsgField (3, 2),
    m_position (255),
    m_priority (7)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldQueuePositionInfo::McpttFloorMsgFieldQueuePositionInfo (uint8_t position, uint8_t priority)
  : McpttFloorMsgField (3, 2),
    m_position (position),
    m_priority (priority)
{
  NS_LOG_FUNCTION (this << (uint32_t)position << (uint32_t)priority);
}

McpttFloorMsgFieldQueuePositionInfo::~McpttFloorMsgFieldQueuePositionInfo (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldQueuePositionInfo::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldQueuePositionInfo deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint8_t position = buff.ReadU8 ();
  bytesRead += 1;

  uint8_t priority = buff.ReadU8 ();
  bytesRead += 1;

  SetPosition (position);
  SetPriority (priority);

  return bytesRead;
}

TypeId
McpttFloorMsgFieldQueuePositionInfo::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldQueuePositionInfo::GetTypeId ();
}

void
McpttFloorMsgFieldQueuePositionInfo::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldQueuePositionInfo(";

  McpttFloorMsgField::Print (os);

  uint8_t position = GetPosition ();
  uint8_t priority = GetPriority ();

  os << ";";
  os << "position=" << (uint32_t)position << ",";
  os << "priority=" << (uint32_t)priority;
  os << ")";
}

void
McpttFloorMsgFieldQueuePositionInfo::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldQueuePositionInfo serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint8_t position = GetPosition ();
  uint8_t priority = GetPriority ();

  buff.WriteU8 (position);
  buff.WriteU8 (priority);

  NS_LOG_LOGIC ("McpttFloorMsgFieldQueuePositionInfo wrote two bytes (position=" << (uint32_t)position << ";priority=" << (uint32_t)priority << ";)");
}

uint8_t
McpttFloorMsgFieldQueuePositionInfo::GetPosition (void) const
{
  NS_LOG_FUNCTION (this);

  return m_position;
}

uint8_t
McpttFloorMsgFieldQueuePositionInfo::GetPriority (void) const
{
  NS_LOG_FUNCTION (this);

  return m_priority;
}

void
McpttFloorMsgFieldQueuePositionInfo::SetPosition (uint8_t position)
{
  NS_LOG_FUNCTION (this << (uint32_t)position);

  m_position = position;
}

void
McpttFloorMsgFieldQueuePositionInfo::SetPriority (uint8_t priority)
{
  NS_LOG_FUNCTION (this << (uint32_t)priority);

  NS_ASSERT_MSG (priority < 8, "Priority must be a value between 0 and 7 (inclusive).");

  m_priority = priority;
}
/** McpttFloorMsgFieldRejectCause - end **/

/** McpttFloorMsgFieldGrantedPartyId - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldGrantedPartyId);

TypeId
McpttFloorMsgFieldGrantedPartyId::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldGrantedPartyId")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldGrantedPartyId> ()
  ;

  return tid;
}

McpttFloorMsgFieldGrantedPartyId::McpttFloorMsgFieldGrantedPartyId (void)
  : McpttFloorMsgField (4, 4),
    m_partyId (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldGrantedPartyId::McpttFloorMsgFieldGrantedPartyId (uint32_t partyId)
  : McpttFloorMsgField (4, 4),
    m_partyId (partyId)
{
  NS_LOG_FUNCTION (this << partyId);
}

McpttFloorMsgFieldGrantedPartyId::~McpttFloorMsgFieldGrantedPartyId (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldGrantedPartyId::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldGrantedPartyId deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint32_t partyId = buff.ReadNtohU32 ();
  bytesRead += 4;

  SetPartyId (partyId);

  NS_LOG_LOGIC ("McpttFloorMsgFieldGrantedPartyId read four bytes (partyId=" << partyId << ";).");

  return bytesRead;
}

TypeId
McpttFloorMsgFieldGrantedPartyId::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldGrantedPartyId::GetTypeId ();
}

void
McpttFloorMsgFieldGrantedPartyId::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldGrantedPartyId(";

  McpttFloorMsgField::Print (os);

  uint32_t partyId = GetPartyId ();

  os << ";";
  os << "partyId=" << partyId;
  os << ")";
}

void
McpttFloorMsgFieldGrantedPartyId::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldGrantedPartyId serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint32_t partyId = GetPartyId ();

  buff.WriteHtonU32 (partyId);

  NS_LOG_LOGIC ("McpttFloorMsgFieldGrantedPartyId wrote four bytes (partyId=" << partyId << ";).");
}

uint32_t
McpttFloorMsgFieldGrantedPartyId::GetPartyId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_partyId;
}

void
McpttFloorMsgFieldGrantedPartyId::SetPartyId (uint32_t partyId)
{
  NS_LOG_FUNCTION (this << partyId);

  m_partyId = partyId;
}
/** McpttFloorMsgFieldGrantedPartyId - end **/

/** McpttFloorMsgFieldPermToReq - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldPermToReq);

TypeId
McpttFloorMsgFieldPermToReq::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldPermToReq")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldPermToReq> ()
  ;

  return tid;
}

McpttFloorMsgFieldPermToReq::McpttFloorMsgFieldPermToReq (void)
  : McpttFloorMsgField (5, 2),
    m_permission (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldPermToReq::McpttFloorMsgFieldPermToReq (uint16_t permission)
  : McpttFloorMsgField (5, 2),
    m_permission (permission)
{
  NS_LOG_FUNCTION (this << (uint32_t)permission);
}

McpttFloorMsgFieldPermToReq::~McpttFloorMsgFieldPermToReq (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldPermToReq::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldPermToReq deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint16_t permission = buff.ReadNtohU16 ();
  bytesRead += 2;

  NS_LOG_LOGIC ("McpttFloorMsgFieldPermToReq read two bytes (permission=" << permission << ";).");

  SetPermission (permission);

  return bytesRead;
}

TypeId
McpttFloorMsgFieldPermToReq::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldPermToReq::GetTypeId ();
}

void
McpttFloorMsgFieldPermToReq::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldPermToReq(";

  McpttFloorMsgField::Print (os);

  uint16_t permission = GetPermission ();

  os << ";";
  os << "permission=" << (uint32_t)permission;
  os << ")";
}

void
McpttFloorMsgFieldPermToReq::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldPermToReq serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint16_t permission = GetPermission ();

  buff.WriteHtonU16 (permission);

  NS_LOG_LOGIC ("McpttFloorMsgFieldPermToReq wrote two bytes (permission=" << (uint32_t)permission << ";).");
}

uint16_t
McpttFloorMsgFieldPermToReq::GetPermission (void) const
{
  NS_LOG_FUNCTION (this);

  return m_permission;
}

void
McpttFloorMsgFieldPermToReq::SetPermission (uint16_t permission)
{
  NS_LOG_FUNCTION (this << (uint32_t)permission);

  NS_ASSERT_MSG (permission < 2, "The permission value must be either a 0 or a 1.");

  m_permission = permission;
}
/** McpttFloorMsgFieldPermToReq - end **/

/** McpttFloorMsgFieldUserId - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldUserId);

TypeId
McpttFloorMsgFieldUserId::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldUserId")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldUserId> ()
  ;

  return tid;
}

McpttFloorMsgFieldUserId::McpttFloorMsgFieldUserId (void)
  : McpttFloorMsgField (6, 4),
    m_userId (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldUserId::McpttFloorMsgFieldUserId (uint32_t userId)
  : McpttFloorMsgField (6, 4),
    m_userId (userId)
{
  NS_LOG_FUNCTION (this << userId);
}

McpttFloorMsgFieldUserId::~McpttFloorMsgFieldUserId (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldUserId::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldUserId deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint32_t userId = buff.ReadNtohU32 ();
  bytesRead += 4;

  SetUserId (userId);

  NS_LOG_LOGIC ("McpttFloorMsgFieldUserId read four bytes (userId=" << userId << ";).");

  return bytesRead;
}

TypeId
McpttFloorMsgFieldUserId::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldUserId::GetTypeId ();
}

void
McpttFloorMsgFieldUserId::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldUserId(";

  McpttFloorMsgField::Print (os);

  uint32_t userId = GetUserId ();

  os << ";";
  os << "userId=" << userId;
  os << ")";
}

void
McpttFloorMsgFieldUserId::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldUserId serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint32_t userId = GetUserId ();

  buff.WriteHtonU32 (userId);

  NS_LOG_LOGIC ("McpttFloorMsgFieldUserId wrote four bytes (userId=" << userId << ";).");
}

McpttFloorMsgFieldUserId::McpttFloorMsgFieldUserId (uint8_t id, uint8_t length, uint16_t userId)
  : McpttFloorMsgField (id, length),
    m_userId (0)
{
  NS_LOG_FUNCTION (this << (uint32_t)id << (uint32_t)length << (uint32_t)userId);

  SetUserId (userId);
}

uint32_t
McpttFloorMsgFieldUserId::GetUserId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_userId;
}

void
McpttFloorMsgFieldUserId::SetUserId (uint32_t userId)
{
  NS_LOG_FUNCTION (this << userId);

  m_userId = userId;
}
/** McpttFloorMsgFieldUserId - end **/

/** McpttFloorMsgFieldQueueSize - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldQueueSize);

TypeId
McpttFloorMsgFieldQueueSize::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldQueueSize")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldQueueSize> ()
  ;

  return tid;
}

McpttFloorMsgFieldQueueSize::McpttFloorMsgFieldQueueSize (void)
  : McpttFloorMsgField (7, 2),
    m_queueSize (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldQueueSize::McpttFloorMsgFieldQueueSize (uint16_t queueSize)
  : McpttFloorMsgField (7, 2),
    m_queueSize (queueSize)
{
  NS_LOG_FUNCTION (this << (uint32_t)queueSize);
}

McpttFloorMsgFieldQueueSize::~McpttFloorMsgFieldQueueSize (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldQueueSize::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldQueueSize deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint16_t queueSize = buff.ReadNtohU16 ();
  bytesRead += 2;

  NS_LOG_LOGIC ("McpttFloorMsgFieldQueueSize read two bytes (queueSize=" << (uint32_t)queueSize << ";).");

  SetQueueSize (queueSize);

  return bytesRead;
}

TypeId
McpttFloorMsgFieldQueueSize::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldQueueSize::GetTypeId ();
}

void
McpttFloorMsgFieldQueueSize::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldQueueSize(";

  McpttFloorMsgField::Print (os);

  uint16_t queueSize = GetQueueSize ();

  os << ";";
  os << "queueSize=" << (uint32_t)queueSize;
  os << ")";
}

void
McpttFloorMsgFieldQueueSize::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldQueueSize serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint16_t queueSize = GetQueueSize ();

  buff.WriteHtonU16 (queueSize);

  NS_LOG_LOGIC ("McpttFloorMsgFieldQueueSize wrote two bytes (queueSize=" << (uint32_t)queueSize << ").");
}

uint16_t
McpttFloorMsgFieldQueueSize::GetQueueSize (void) const
{
  NS_LOG_FUNCTION (this);

  return m_queueSize;
}

void
McpttFloorMsgFieldQueueSize::SetQueueSize (uint16_t queueSize)
{
  NS_LOG_FUNCTION (this << (uint32_t)queueSize);

  m_queueSize = queueSize;
}
/** McpttFloorMsgFieldQueueSize - end **/

/** McpttFloorMsgFieldSeqNum - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldSeqNum);

TypeId
McpttFloorMsgFieldSeqNum::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttSeqNumField")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldSeqNum> ()
  ;

  return tid;
}

McpttFloorMsgFieldSeqNum::McpttFloorMsgFieldSeqNum (void)
  : McpttFloorMsgField (8, 2),
    m_seqNum (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldSeqNum::McpttFloorMsgFieldSeqNum (uint16_t seqNum)
  : McpttFloorMsgField (8, 2),
    m_seqNum (seqNum)
{
  NS_LOG_FUNCTION (this << (uint32_t)seqNum);

  SetSeqNum (seqNum);
}

McpttFloorMsgFieldSeqNum::~McpttFloorMsgFieldSeqNum (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldSeqNum::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldSeqNum deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint16_t seqNum = buff.ReadNtohU16 ();
  bytesRead += 2;

  NS_LOG_LOGIC ("McpttFloorMsgFieldSeqNum read two bytes (seqNum=" << (uint32_t)seqNum << ";).");

  SetSeqNum (seqNum);

  return bytesRead;
}

TypeId
McpttFloorMsgFieldSeqNum::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldSeqNum::GetTypeId ();
}

void
McpttFloorMsgFieldSeqNum::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldSeqNum(";

  McpttFloorMsgField::Print (os);

  uint16_t seqNum = GetSeqNum ();

  os << ";";
  os << "seqNum=" << (uint32_t)seqNum;
  os << ")";
}

void
McpttFloorMsgFieldSeqNum::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldSeqNum serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint16_t seqNum = GetSeqNum ();

  buff.WriteHtonU16 (seqNum);

  NS_LOG_LOGIC ("McpttFloorMsgFieldSeqNum wrote two bytes (seqNum=" << (uint32_t)seqNum << ";).");
}

uint16_t
McpttFloorMsgFieldSeqNum::GetSeqNum (void) const
{
  NS_LOG_FUNCTION (this);

  return m_seqNum;
}

void
McpttFloorMsgFieldSeqNum::SetSeqNum (uint16_t seqNum)
{
  NS_LOG_FUNCTION (this << (uint32_t)seqNum);

  m_seqNum = seqNum;
}
/** McpttFloorMsgFieldSeqNum - end **/

/** McpttFloorMsgFieldQueuedUserId - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldQueuedUserId);

TypeId
McpttFloorMsgFieldQueuedUserId::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldQueuedUserId")
    .SetParent<McpttFloorMsgFieldUserId> ()
    .AddConstructor<McpttFloorMsgFieldQueuedUserId> ()
  ;

  return tid;
}

McpttFloorMsgFieldQueuedUserId::McpttFloorMsgFieldQueuedUserId (void)
  : McpttFloorMsgFieldUserId (9, 4, 0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldQueuedUserId::McpttFloorMsgFieldQueuedUserId (uint32_t userId)
  : McpttFloorMsgFieldUserId (9, 4, userId)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldQueuedUserId::~McpttFloorMsgFieldQueuedUserId (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldQueuedUserId::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldQueuedUserId deserializing...");

  uint32_t bytesRead = McpttFloorMsgFieldUserId::Deserialize (buff);

  return bytesRead;
}

TypeId
McpttFloorMsgFieldQueuedUserId::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldQueuedUserId::GetTypeId ();
}

void
McpttFloorMsgFieldQueuedUserId::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldQueuedUserId(";

  McpttFloorMsgFieldUserId::Print (os);

  os << ")";
}

void
McpttFloorMsgFieldQueuedUserId::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldQueuedUserId serializing...");

  McpttFloorMsgFieldUserId::Serialize (buff);
}
/** McpttFloorMsgFieldQueuedUserId - end **/

/** McpttFloorMsgFieldSource - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldSource);

const uint16_t McpttFloorMsgFieldSource::FLOOR_PARTICIPANT = 0;
const uint16_t McpttFloorMsgFieldSource::PARTICIPATING_FUNCTION = 1;
const uint16_t McpttFloorMsgFieldSource::CONTROLLING_FUNCTION = 2;
const uint16_t McpttFloorMsgFieldSource::NONCONTROLLING_FUNCTION = 3;

TypeId
McpttFloorMsgFieldSource::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldSource")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldSource> ()
  ;

  return tid;
}

McpttFloorMsgFieldSource::McpttFloorMsgFieldSource (void)
  : McpttFloorMsgField (10, 2),
    m_source (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldSource::McpttFloorMsgFieldSource (uint16_t source)
  : McpttFloorMsgField (10, 2),
    m_source (source)
{
  NS_LOG_FUNCTION (this << (uint32_t)source);
}

McpttFloorMsgFieldSource::~McpttFloorMsgFieldSource (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldSource::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldSource deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint16_t source = buff.ReadNtohU16 ();
  bytesRead += 2;

  SetSource (source);

  NS_LOG_LOGIC ("McpttFloorMsgFieldSource read two bytes (source=" << (uint32_t)source << ";).");

  return bytesRead;
}

TypeId
McpttFloorMsgFieldSource::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldSource::GetTypeId ();
}

void
McpttFloorMsgFieldSource::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldSource(";

  McpttFloorMsgField::Print (os);

  uint16_t source = GetSource ();

  os << ";";
  os << "source=" << (uint32_t)source;
  os << ")";
}

void
McpttFloorMsgFieldSource::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldSource serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint16_t source = GetSource ();

  buff.WriteHtonU16 (source);

  NS_LOG_LOGIC ("McpttFloorMsgFieldSource wrote two bytes (source=" << (uint32_t)source << ";).");
}

uint16_t
McpttFloorMsgFieldSource::GetSource (void) const
{
  NS_LOG_FUNCTION (this);

  return m_source;
}

void
McpttFloorMsgFieldSource::SetSource (uint16_t source)
{
  NS_LOG_FUNCTION (this << (uint32_t)source);

  NS_ASSERT_MSG (source < 4, "The source value must be less than 4.");

  m_source = source;
}
/** McpttFloorMsgFieldSource - end **/

/** McpttFloorMsgFieldTrackInfo - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldTrackInfo);

TypeId
McpttFloorMsgFieldTrackInfo::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldTrackInfo")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldTrackInfo> ()
  ;

  return tid;
}

McpttFloorMsgFieldTrackInfo::McpttFloorMsgFieldTrackInfo (void)
  : McpttFloorMsgField (11, 6),
    m_ptLength (4),
    m_queueCap (0),
    m_refs (std::vector<uint32_t> ())
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldTrackInfo::McpttFloorMsgFieldTrackInfo (uint8_t queueCap)
  : McpttFloorMsgField (11, 6),
    m_ptLength (4),
    m_queueCap (queueCap),
    m_refs (std::vector<uint32_t> ())
{
  NS_LOG_FUNCTION (this << (uint32_t)queueCap);
}

McpttFloorMsgFieldTrackInfo::~McpttFloorMsgFieldTrackInfo (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMsgFieldTrackInfo::AddRef (uint32_t ref)
{
  NS_LOG_FUNCTION (this << ref);

  // Since we are adding a reference the overall length of the header will be
  // increased, so get the overall length for later use.
  uint8_t length = GetLength ();
  std::vector<uint32_t> refs = GetRefs ();

  refs.push_back (ref);
  length += 4; // Add the length of a reference to the ovarall length.

  NS_LOG_LOGIC ("McpttFloorMsgFieldTrackInfo added ref = " << ref << " for a total of " << refs.size () << " reference(s).");
  // Update the value stored for the overall length of the header.
  SetLength (length);

  SetRefs (refs);
}

void
McpttFloorMsgFieldTrackInfo::ClearRefs (void)
{
  NS_LOG_FUNCTION (this);

  uint8_t length = GetLength ();
  std::vector<uint32_t> refs = GetRefs ();

  length -= (refs.size () * 4);

  refs.clear ();

  SetLength (length);
  SetRefs (refs);
}

uint32_t
McpttFloorMsgFieldTrackInfo::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldTrackInfo deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint8_t queueCap = buff.ReadU8 ();
  bytesRead += 1;
  SetQueueCap (queueCap);

  uint8_t ptLength = buff.ReadU8 ();
  bytesRead += 1;
  SetPtLength (ptLength);

  NS_LOG_LOGIC ("McpttFloorMsgFieldTrackInfo read two bytes (queueCap=" << (uint32_t)queueCap << ";ptLength=" << (uint32_t)ptLength << ";).");

  // Since the standard does not specify what the participant type value is we
  // just place "ptLength" bytes with value 0 in it's place with some padding
  // if needed since it's serialized size/length must be divisible by 4.
  uint8_t ptLengthWithPadding = GetPtLengthWithPadding ();
  for (uint8_t ptValIndex = 0; ptValIndex < ptLengthWithPadding; ptValIndex++)
    {
      buff.ReadU8 ();

      bytesRead += 1;
    }

  NS_LOG_LOGIC ("McpttFloorMsgFieldTrackInfo read " << (uint32_t)ptLengthWithPadding << " bytes (" << (ptLengthWithPadding - ptLength) << " were for padding).");

  // This line of code must be called after "SetPtLength" has been called
  // because this method calls "GetPtLengthWithPadding" which relies on
  // "ptLength" which was just read.
  uint32_t totalBytes = GetSerializedSize ();

  // Now that we have read all the bytes for the previous fields, we must now
  // compute the number of floor participant references that are recorded in the
  // header. Since each reference value is 4 bytes long, the remaining number of
  // bytes must be divisible by 4 (this includes 0 which would mean no
  // references were recorded).
  uint32_t bytesLeft = (totalBytes - bytesRead);
  uint32_t refCount = (bytesLeft / 4);

  NS_LOG_LOGIC ("McpttFloorMsgFieldTrackInfo read (" << bytesRead << "/" << totalBytes << ") bytes, which leaves " << bytesLeft << " bytes left and " << refCount << " reference(s) to read.");

  NS_ASSERT_MSG (bytesLeft >= 0 && bytesLeft % 4 == 0, "Something went wrong deserializing a McpttFloorMsgFieldTrackInfo.");

  std::vector<uint32_t> refs;
  // Now that we know how many references have been recorded, we will read each
  // reference value (which is represented by 4 bytes) and store it.
  for (uint32_t refIndex = 0; refIndex < refCount; refIndex++)
    {
      uint32_t ref = buff.ReadNtohU32 ();
      bytesRead += 4;

      refs.push_back (ref);
    }

  SetRefs (refs);

  NS_LOG_LOGIC ("McpttFloorMsgFieldTrackInfo read " << refCount << " reference(s) (" << (refCount * 4) << " bytes).");

  return bytesRead;
}

uint8_t
McpttFloorMsgFieldTrackInfo::GetPtLengthWithPadding (void) const
{
  NS_LOG_FUNCTION (this);

  uint8_t ptLength = GetPtLength ();

  uint8_t ptLengthWithPadding = ptLength + (ptLength % 4);

  return ptLengthWithPadding;
}

TypeId
McpttFloorMsgFieldTrackInfo::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldTrackInfo::GetTypeId ();
}

uint32_t
McpttFloorMsgFieldTrackInfo::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);

  uint32_t size = McpttFloorMsgField::GetSerializedSize ();

  uint8_t ptLength = GetPtLength ();
  uint8_t ptLengthWithPadding = GetPtLengthWithPadding ();

  size -= ptLength;
  size += ptLengthWithPadding;

  NS_LOG_LOGIC ("McpttFloorMsgFieldTrackInfo serialized size = " << size << " bytes.");

  return size;
}

void
McpttFloorMsgFieldTrackInfo::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldTrackInfo(";

  McpttFloorMsgField::Print (os);

  uint8_t ptLength = GetPtLength ();
  uint8_t queueCap = GetQueueCap ();
  std::vector<uint32_t> refs = GetRefs ();

  os << ";";
  os << "ptLength=" << (uint32_t)ptLength << ",";
  os << "queueCap=" << (uint32_t)queueCap << ",";
  os << "Refs(";

  for (uint32_t refIndex = 0; refIndex < refs.size (); refIndex++)
    {
      uint32_t ref = refs.at (refIndex);

      os << ref << ";";
    }

  os << ")";
  os << ")";
}

void
McpttFloorMsgFieldTrackInfo::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldTrackInfo serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint8_t ptLength = GetPtLength ();
  uint8_t queueCap = GetQueueCap ();
  std::vector<uint32_t> refs = GetRefs ();
  uint8_t ptLengthWithPadding = GetPtLengthWithPadding ();

  buff.WriteU8 (queueCap);
  buff.WriteU8 (ptLength);

  NS_LOG_LOGIC ("McpttFloorMsgFieldTrackInfo wrote two bytes (queueCap=" << (uint32_t)queueCap << ";ptLength=" << (uint32_t)ptLength << ";).");

  for (uint8_t ptValIndex = 0; ptValIndex < ptLengthWithPadding; ptValIndex++)
    {
      buff.WriteU8 (0);
    }

  NS_LOG_LOGIC ("McpttFloorMsgFieldTrackInfo wrote " << (uint32_t)ptLengthWithPadding << " bytes (" << (ptLengthWithPadding - ptLength) << " were for padding).");

  for (uint32_t refIndex = 0; refIndex < refs.size (); refIndex++)
    {
      uint32_t ref = refs.at (refIndex);

      buff.WriteHtonU32 (ref);
    }

  NS_LOG_LOGIC ("McpttFloorMsgFieldTrackInfo wrote " << refs.size () << " reference(s) (" << (refs.size () * 4) << " bytes).");
}

void
McpttFloorMsgFieldTrackInfo::UpdatePtLength (uint8_t ptLength)
{
  NS_LOG_FUNCTION (this << ptLength);

  // Since the length of the participant type is changing so is the length of
  // the overall header, so get the length for future use.
  uint8_t length = GetLength ();
  uint8_t currPtLength = GetPtLength ();

  // Deduct the current value stored for the participant type length.
  length -= currPtLength;

  // Update the current value stored for the participant type length.
  SetPtLength (ptLength);

  // Added the updated value to the length of the overall header.
  length += ptLength;

  // Store the updated length of the overall header.
  SetLength (length);
}

uint8_t
McpttFloorMsgFieldTrackInfo::GetPtLength (void) const
{
  NS_LOG_FUNCTION (this);

  return m_ptLength;
}

uint8_t
McpttFloorMsgFieldTrackInfo::GetQueueCap (void) const
{
  NS_LOG_FUNCTION (this);

  return m_queueCap;
}

void
McpttFloorMsgFieldTrackInfo::SetPtLength (uint8_t ptLength)
{
  NS_LOG_FUNCTION (this << (uint32_t)ptLength);

  m_ptLength = ptLength;
}

void
McpttFloorMsgFieldTrackInfo::SetQueueCap (uint8_t queueCap)
{
  NS_LOG_FUNCTION (this << (uint32_t)queueCap);

  NS_ASSERT_MSG (queueCap < 2, "The queuing capability value must be 0 or 1.");

  m_queueCap = queueCap;
}

std::vector<uint32_t>
McpttFloorMsgFieldTrackInfo::GetRefs (void) const
{
  NS_LOG_FUNCTION (this);

  return m_refs;
}

void
McpttFloorMsgFieldTrackInfo::SetRefs (const std::vector<uint32_t>  refs)
{
  NS_LOG_FUNCTION (this << &refs);

  m_refs = refs;
}

/** McpttFloorMsgFieldTrackInfo - end **/

/** McpttFloorMsgFieldType - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldType);

TypeId
McpttFloorMsgFieldType::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldType")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldType> ()
  ;

  return tid;
}

McpttFloorMsgFieldType::McpttFloorMsgFieldType (void)
  : McpttFloorMsgField (12, 2),
    m_msgType (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldType::McpttFloorMsgFieldType (uint8_t msgType)
  : McpttFloorMsgField (12, 2),
    m_msgType (msgType)
{
  NS_LOG_FUNCTION (this << (uint32_t)msgType);
}

McpttFloorMsgFieldType::~McpttFloorMsgFieldType (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldType::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldType deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint8_t msgType = buff.ReadU8 ();
  bytesRead += 1;

  buff.ReadU8 (); // Read the "spare" byte.
  bytesRead += 1;

  SetMsgType (msgType);

  NS_LOG_LOGIC ("McpttFloorMsgFieldType read two bytes (msgType=" << (uint32_t)msgType << ";).");

  return bytesRead;
}

TypeId
McpttFloorMsgFieldType::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldType::GetTypeId ();
}

void
McpttFloorMsgFieldType::Print (std::ostream& os)  const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldType(";

  McpttFloorMsgField::Print (os);

  uint8_t msgType = GetMsgType ();

  os << ";";
  os << "msgType=" << (uint32_t)msgType;
  os << ")";
}

void
McpttFloorMsgFieldType::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldType serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint8_t msgType = GetMsgType ();

  buff.WriteU8 (msgType);
  buff.WriteU8 (0); // Write "spare" byte.

  NS_LOG_LOGIC ("McpttFloorMsgFieldType wrote two bytes (msgType=" << (uint32_t)msgType << ";)");
}

uint8_t
McpttFloorMsgFieldType::GetMsgType (void) const
{
  NS_LOG_FUNCTION (this);

  return m_msgType;
}

void
McpttFloorMsgFieldType::SetMsgType (uint8_t msgType)
{
  NS_LOG_FUNCTION (this << (uint32_t)msgType);

  m_msgType = msgType;
}
/** McpttFloorMsgFieldType - end **/

/** McpttFloorMsgFieldIndic - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldIndic);

const uint16_t McpttFloorMsgFieldIndic::BROADCAST_CALL = 0x4000;
const uint16_t McpttFloorMsgFieldIndic::DUAL_FLOOR = 0x0200;
const uint16_t McpttFloorMsgFieldIndic::EMERGENCY_CALL = 0x1000;
const uint16_t McpttFloorMsgFieldIndic::IMMINENT_CALL = 0x0800;
const uint16_t McpttFloorMsgFieldIndic::NORMAL_CALL = 0x8000;
const uint16_t McpttFloorMsgFieldIndic::QUEUING_SUPP = 0x0400;
const uint16_t McpttFloorMsgFieldIndic::SYSTEM_CALL = 0x2000;
const uint16_t McpttFloorMsgFieldIndic::TEMPORARY_GROUP_CALL = 0x0100;

TypeId
McpttFloorMsgFieldIndic::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldIndic")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldIndic> ()
  ;

  return tid;
}

McpttFloorMsgFieldIndic::McpttFloorMsgFieldIndic (void)
  : McpttFloorMsgField (13, 2),
    m_flags (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldIndic::~McpttFloorMsgFieldIndic (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldIndic::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldIndic deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint16_t flags = buff.ReadNtohU16 ();
  bytesRead  += 2;

  SetFlags (flags);

  NS_LOG_LOGIC ("McpttFloorMsgFieldIndic read two bytes (flags=" << (uint32_t)flags << ";).");

  return bytesRead;
}

TypeId
McpttFloorMsgFieldIndic::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldIndic::GetTypeId ();
}

void
McpttFloorMsgFieldIndic::Indicate (uint16_t pattern)
{
  NS_LOG_FUNCTION (this << (uint32_t)pattern);

  uint16_t flags = GetFlags ();

  flags |= pattern;

  SetFlags (flags);
}

bool
McpttFloorMsgFieldIndic::IsIndicated (uint16_t pattern) const
{
  NS_LOG_FUNCTION (this << (uint32_t)pattern);

  uint16_t flags = GetFlags ();

  bool isIndicated = ((flags & pattern) == pattern);

  return isIndicated;
}

void
McpttFloorMsgFieldIndic::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldIndic(";

  McpttFloorMsgField::Print (os);

  uint16_t flags = GetFlags ();

  os << ";";
  os << "flags=" << (uint32_t)flags;
  os << ")";
}

void
McpttFloorMsgFieldIndic::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldIndic serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint16_t flags = GetFlags ();

  buff.WriteHtonU16 (flags);

  NS_LOG_LOGIC ("McpttFloorMsgFieldIndic wrote two bytes (flags=" << (uint32_t)flags << ";).");
}

void
McpttFloorMsgFieldIndic::Unindicate (uint16_t pattern)
{
  NS_LOG_FUNCTION (this << (uint32_t)pattern);

  uint16_t flags = GetFlags ();

  flags &= (~pattern);

  SetFlags (flags);
}

uint16_t
McpttFloorMsgFieldIndic::GetFlags (void) const
{
  NS_LOG_FUNCTION (this);

  return m_flags;
}

void
McpttFloorMsgFieldIndic::SetFlags (uint16_t flags)
{
  NS_LOG_FUNCTION (this << (uint32_t)flags);

  m_flags = flags;
}
/** McpttFloorMsgFieldIndic - end **/

/** McpttFloorMsgFieldSsrc - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMsgFieldSsrc);

TypeId
McpttFloorMsgFieldSsrc::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttFloorMsgFieldSsrc")
    .SetParent<McpttFloorMsgField> ()
    .AddConstructor<McpttFloorMsgFieldSsrc> ()
  ;

  return tid;
}

McpttFloorMsgFieldSsrc::McpttFloorMsgFieldSsrc (void)
  : McpttFloorMsgField (14, 6),
    m_ssrc (0)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMsgFieldSsrc::McpttFloorMsgFieldSsrc (uint32_t ssrc)
  : McpttFloorMsgField (14, 6),
    m_ssrc (ssrc)
{
  NS_LOG_FUNCTION (this << ssrc);
}

McpttFloorMsgFieldSsrc::~McpttFloorMsgFieldSsrc (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMsgFieldSsrc::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldSsrc deserializing...");

  uint32_t bytesRead = McpttFloorMsgField::Deserialize (buff);

  uint32_t ssrc = buff.ReadNtohU32 ();
  bytesRead += 4;

  buff.ReadNtohU16 (); // Read two spare bytes.
  bytesRead += 2;

  SetSsrc (ssrc);

  NS_LOG_LOGIC ("McpttFloorMsgFieldSsrc read six bytes (ssrc=" << ssrc << ";spare=0).");

  return bytesRead;
}

TypeId
McpttFloorMsgFieldSsrc::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMsgFieldSsrc::GetTypeId ();
}

void
McpttFloorMsgFieldSsrc::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << "McpttFloorMsgFieldSsrc(";

  McpttFloorMsgField::Print (os);

  uint32_t ssrc = GetSsrc ();

  os << ";";
  os << "ssrc=" << ssrc;
  os << ")";
}

void
McpttFloorMsgFieldSsrc::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttFloorMsgFieldSsrc serializing...");

  McpttFloorMsgField::Serialize (buff);

  uint32_t ssrc = GetSsrc ();

  buff.WriteHtonU32 (ssrc);
  buff.WriteHtonU16 (0); // Write two spare bytes.

  NS_LOG_LOGIC ("McpttFloorMsgFieldSsrc wrote six bytes (ssrc=" << ssrc << ";).");
}

McpttFloorMsgFieldSsrc::McpttFloorMsgFieldSsrc (uint8_t id, uint8_t length, uint32_t ssrc)
  : McpttFloorMsgField (id, length),
    m_ssrc (ssrc)
{
  NS_LOG_FUNCTION (this << (uint32_t)id << (uint32_t)length << (uint32_t)ssrc);
}

uint32_t
McpttFloorMsgFieldSsrc::GetSsrc (void) const
{
  NS_LOG_FUNCTION (this);

  return m_ssrc;
}

void
McpttFloorMsgFieldSsrc::SetSsrc (uint32_t ssrc)
{
  NS_LOG_FUNCTION (this << ssrc);

  m_ssrc = ssrc;
}
/** McpttFloorMsgFieldSsrc - end **/


} // namespace psc
} // namespace ns3

