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

#include <ns3/mcptt-msg.h>
#include <ns3/type-id.h>

#include "mcptt-msg-dropper.h"

namespace ns3 {
namespace psc {
namespace tests {

McpttMsgDropper::McpttMsgDropper (void)
{ }

McpttMsgDropper::~McpttMsgDropper (void)
{ }

McpttMsgDropperImpl::McpttMsgDropperImpl (void)
  : McpttMsgDropper (),
    m_limit (0),
    m_msgTypeId (TypeId ()),
    m_received (0)
{ }

McpttMsgDropperImpl::McpttMsgDropperImpl (const uint32_t& limit, const TypeId& msgTypeId)
  : McpttMsgDropper (),
    m_limit (limit),
    m_msgTypeId (msgTypeId),
    m_received (0)
{ }

McpttMsgDropperImpl::~McpttMsgDropperImpl (void)
{
  SetLimit (0);
  SetMsgTypeId (TypeId ());
  SetReceived (0);
}

bool
McpttMsgDropperImpl::ShouldDropMsg (const McpttMsg& msg)
{
  bool dropMsg = false;
  TypeId msgTypeId = GetMsgTypeId ();

  if (msg.IsA (msgTypeId))
    {
      IncrementReceived ();

      dropMsg = !IsLimitReached ();
    }

  return dropMsg;
}

void
McpttMsgDropperImpl::IncrementReceived (const uint32_t& amount)
{
  uint32_t received = GetReceived ();

  received += amount;

  SetReceived (received);
}

bool
McpttMsgDropperImpl::IsLimitReached (void) const
{
  uint32_t limit = GetLimit ();
  uint32_t received = GetReceived ();

  bool limitReached = (received >= limit);

  return limitReached;
}

uint32_t
McpttMsgDropperImpl::GetLimit (void) const
{
  return m_limit;
}

TypeId
McpttMsgDropperImpl::GetMsgTypeId (void) const
{
  return m_msgTypeId;
}

uint32_t
McpttMsgDropperImpl::GetReceived (void) const
{
  return m_received;
}

void
McpttMsgDropperImpl::SetLimit (const uint32_t& limit)
{
  m_limit = limit;
}

void
McpttMsgDropperImpl::SetMsgTypeId (const TypeId& msgTypeId)
{
  m_msgTypeId = msgTypeId;
}

void
McpttMsgDropperImpl::SetReceived (const uint32_t& received)
{
  m_received = received;
}

} // namespace tests
} // namespace psc
} // namespace ns3

