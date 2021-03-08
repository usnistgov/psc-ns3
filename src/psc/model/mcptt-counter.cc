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

#include "mcptt-counter.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCounter");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttCounter);

TypeId
McpttCounter::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttCounter")
    .SetParent<Object> ()
    .AddConstructor<McpttCounter> ()
  ;

  return tid;
}

McpttCounter::McpttCounter (void)
  : Object (),
    m_id (McpttEntityId (0, "Counter")),
    m_limit (0),
    m_start (0),
    m_value (0)
{
  NS_LOG_FUNCTION (this);
}

McpttCounter::McpttCounter (const McpttEntityId& id, uint32_t start, uint32_t limit)
  : Object (),
    m_id (id),
    m_limit (limit),
    m_start (start),
    m_value (start)
{
  NS_LOG_FUNCTION (this << id << limit << start);
}

McpttCounter::~McpttCounter (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttCounter::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttCounter::GetTypeId ();
}

void
McpttCounter::Increment (uint32_t amount)
{
  NS_LOG_FUNCTION (this << amount);

  McpttEntityId id = GetId ();
  uint32_t value = GetValue ();

  NS_LOG_LOGIC ("Counter " << id << " incrementing " << value  << " by " << amount << " for total of " << (value + amount) << ".");

  value += amount;

  SetValue (value);
}

bool
McpttCounter::IsLimitExceeded (void) const
{
  NS_LOG_FUNCTION (this);

  McpttEntityId id = GetId ();
  uint32_t limit = GetLimit ();
  uint32_t value = GetValue ();

  bool isExceeded = (value > limit);

  NS_LOG_LOGIC ("Counter " << id << " limit is" << (isExceeded ? " " : " not ") << "exceeded " << "(" << value << "/" << limit << ").");

  return isExceeded;
}

bool
McpttCounter::IsLimitReached (void) const
{
  NS_LOG_FUNCTION (this);

  McpttEntityId id = GetId ();
  uint32_t limit = GetLimit ();
  uint32_t value = GetValue ();

  bool isReached = (value >= limit);

  NS_LOG_LOGIC ("Counter " << id << " limit is" << (isReached ? " " : " not ") << "reached " << "(" << value << "/" << limit << ").");

  return isReached;
}

void
McpttCounter::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this << &os);

  McpttEntityId id = GetId ();
  uint32_t limit = GetLimit ();
  uint32_t start = GetStart ();
  uint32_t value = GetValue ();

  os << id << "(";
  os << "start-value=" << start << ";";
  os << "current-value=" << value << ";";
  os << "limit=" << limit;
  os << ")";
}

void
McpttCounter::Reset ()
{
  NS_LOG_FUNCTION (this);

  McpttEntityId id = GetId ();
  uint32_t start = GetStart ();

  NS_LOG_LOGIC ("Reseting counter " << id << " to " << start << ".");

  SetValue (start);
}

McpttEntityId
McpttCounter::GetId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_id;
}

uint32_t
McpttCounter::GetLimit (void) const
{
  NS_LOG_FUNCTION (this);

  return m_limit;
}

uint32_t
McpttCounter::GetStart (void) const
{
  NS_LOG_FUNCTION (this);

  return m_start;
}

uint32_t
McpttCounter::GetValue (void) const
{
  NS_LOG_FUNCTION (this);

  return m_value;
}

void
McpttCounter::SetId (const McpttEntityId& id)
{
  NS_LOG_FUNCTION (this << id);

  m_id = id;
}

void
McpttCounter::SetLimit (uint32_t limit)
{
  NS_LOG_FUNCTION (this << limit);

  m_limit = limit;
}

void
McpttCounter::SetStart (uint32_t start)
{
  NS_LOG_FUNCTION (this << start);

  m_start = start;
}

void
McpttCounter::SetValue (uint32_t value)
{
  NS_LOG_FUNCTION (this << value);

  m_value = value;
}

std::ostream& operator<< (std::ostream& os, const McpttCounter& counter)
{
  counter.Print (os);

  return os;
}

} // namespace psc
} // namespace ns3
