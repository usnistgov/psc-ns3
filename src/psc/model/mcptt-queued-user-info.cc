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
#include <ns3/log.h>
#include <ns3/object-base.h>
#include <ns3/type-id.h>

#include "mcptt-floor-msg-field.h"
#include "mcptt-queued-user-info.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttQueuedUserInfo");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttQueuedUserInfo);

TypeId
McpttQueuedUserInfo::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttQueuedUserInfo")
    .SetParent<ObjectBase> ()
    .AddConstructor<McpttQueuedUserInfo> ()
  ;

  return tid;
}

McpttQueuedUserInfo::McpttQueuedUserInfo (void)
  : ObjectBase (),
    m_info (McpttFloorMsgFieldQueuePositionInfo ()),
    m_ssrc (0),
    m_userId (McpttFloorMsgFieldQueuedUserId ())
{
  NS_LOG_FUNCTION (this);
}

McpttQueuedUserInfo::McpttQueuedUserInfo (uint32_t ssrc, const McpttFloorMsgFieldQueuedUserId& userId, const McpttFloorMsgFieldQueuePositionInfo& info)
  : ObjectBase (),
    m_info (McpttFloorMsgFieldQueuePositionInfo ()),
    m_ssrc (0),
    m_userId (McpttFloorMsgFieldQueuedUserId ())
{
  NS_LOG_FUNCTION (this << ssrc << userId << info);

  SetSsrc (ssrc);
  SetUserId (userId);
  SetInfo (info);
}

McpttQueuedUserInfo::~McpttQueuedUserInfo (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttQueuedUserInfo::Deserialize (Buffer::Iterator& buff)
{
  NS_LOG_FUNCTION (this << &buff);

  uint32_t bytesRead = 0;

  NS_LOG_LOGIC ("McpttQueuedUserInfo deserializing...");

  McpttFloorMsgFieldQueuedUserId userId;
  McpttFloorMsgFieldQueuePositionInfo info;

  uint32_t ssrc = buff.ReadNtohU32 ();
  bytesRead += 4;

  NS_LOG_LOGIC ("McpttQueuedUserInfo read four bytes (ssrc=" << ssrc << ").");

  bytesRead += userId.Deserialize (buff);
  bytesRead += info.Deserialize (buff);

  SetSsrc (ssrc);
  SetUserId (userId);
  SetInfo (info);

  return bytesRead;
}

uint32_t
McpttQueuedUserInfo::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);

  uint32_t size = 0;
  McpttFloorMsgFieldQueuedUserId userId = GetUserId ();
  McpttFloorMsgFieldQueuePositionInfo info = GetInfo ();

  size += 4; // 4 bytes for the SSRC.
  size += userId.GetSerializedSize ();
  size += info.GetSerializedSize ();

  NS_LOG_LOGIC ("McpttQueuedUserInfo serialized size = " << size << " bytes.");

  return size;
}

TypeId
McpttQueuedUserInfo::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttQueuedUserInfo::GetTypeId ();
}

void
McpttQueuedUserInfo::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this << &os);

  uint32_t ssrc = GetSsrc ();
  McpttFloorMsgFieldQueuedUserId userId = GetUserId ();
  McpttFloorMsgFieldQueuePositionInfo info = GetInfo ();

  os << "McpttQueuedUserInfo(";
  os << "ssrc=" << ssrc << ";";
  os << userId << ";";
  os << info;
  os << ")";
}

void
McpttQueuedUserInfo::Serialize (Buffer::Iterator& buff) const
{
  NS_LOG_FUNCTION (this << &buff);

  NS_LOG_LOGIC ("McpttQueuedUserInfo serializing...");

  uint32_t ssrc = GetSsrc ();
  McpttFloorMsgFieldQueuedUserId userId = GetUserId ();
  McpttFloorMsgFieldQueuePositionInfo info = GetInfo ();

  buff.WriteHtonU32 (ssrc);

  NS_LOG_LOGIC ("McpttQueuedUserInfo wrote four bytes (ssrc=" << ssrc << ").");

  userId.Serialize (buff);
  info.Serialize (buff);
}

McpttFloorMsgFieldQueuePositionInfo
McpttQueuedUserInfo::GetInfo (void) const
{
  NS_LOG_FUNCTION (this);

  return m_info;
}

uint32_t
McpttQueuedUserInfo::GetSsrc (void) const
{
  NS_LOG_FUNCTION (this);

  return m_ssrc;
}

McpttFloorMsgFieldQueuedUserId
McpttQueuedUserInfo::GetUserId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_userId;
}

void
McpttQueuedUserInfo::SetInfo (const McpttFloorMsgFieldQueuePositionInfo& info)
{
  NS_LOG_FUNCTION (this << info);

  m_info = info;
}

void
McpttQueuedUserInfo::SetSsrc (uint32_t ssrc)
{
  NS_LOG_FUNCTION (this << ssrc);

  m_ssrc = ssrc;
}

void
McpttQueuedUserInfo::SetUserId (const McpttFloorMsgFieldQueuedUserId& userId)
{
  NS_LOG_FUNCTION (this << userId);

  m_userId = userId;
}

std::ostream&
operator<< (std::ostream& os, const McpttQueuedUserInfo& info)
{
  NS_LOG_FUNCTION (&os << &info);

  info.Print (os);

  return os;
}

} // namespace psc
} // namespace ns3

