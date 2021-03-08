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
#include <stdint.h>
#include <string>

#include "mcptt-entity-id.h"

namespace ns3 {

namespace psc {

McpttEntityId::McpttEntityId (void)
  : m_name (""),
    m_number (0)
{}

McpttEntityId::McpttEntityId (uint32_t number, const std::string& name)
  : m_name (name),
    m_number (number)
{}

McpttEntityId::~McpttEntityId (void)
{}

bool
McpttEntityId::IsEqualTo (const McpttEntityId& other) const
{
  std::string myName = GetName ();
  uint32_t myNumber = GetNumber ();
  std::string theirName = other.GetName ();
  uint32_t theirNumber = other.GetNumber ();

  bool isEqual = (myNumber == theirNumber && myName == theirName);

  return isEqual;
}

void
McpttEntityId::Print (std::ostream& os) const
{
  std::string myName = GetName ();

  os << myName;
}

std::string
McpttEntityId::GetName (void) const
{
  return m_name;
}

uint32_t
McpttEntityId::GetNumber (void) const
{
  return m_number;
}

void
McpttEntityId::SetName (const std::string& name)
{
  m_name = name;
}

void
McpttEntityId::SetNumber (uint32_t number)
{
  m_number = number;
}
bool
operator== (const McpttEntityId& rhs, const McpttEntityId& lhs)
{
  bool isEqual = rhs.IsEqualTo (lhs);

  return isEqual;
}

bool
operator!= (const McpttEntityId& rhs, const McpttEntityId& lhs)
{
  bool notEqual = !rhs.IsEqualTo (lhs);

  return notEqual;
}

std::ostream&
operator<< (std::ostream& os, const McpttEntityId& id)
{
  id.Print (os);

  return os;
}

} // namespace psc
} // namespace ns3

