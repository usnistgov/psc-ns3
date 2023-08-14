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

#include <list>

#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/uinteger.h>

#include "mcptt-floor-queue.h"
#include "mcptt-queued-user-info.h"

namespace ns3 {

namespace psc {

NS_LOG_COMPONENT_DEFINE ("McpttFloorQueue");

NS_OBJECT_ENSURE_REGISTERED (McpttFloorQueue);

TypeId
McpttFloorQueue::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttFloorQueue")
    .SetParent<Object> ()
    .AddConstructor<McpttFloorQueue> ()
    .AddAttribute ("Capacity", "The maximum number of users that can be queued.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&McpttFloorQueue::m_capacity),
                   MakeUintegerChecker<uint16_t> ())
  ;

  return tid;
}

McpttFloorQueue::McpttFloorQueue (uint16_t capacity)
  : Object (),
    m_users (std::list<McpttQueuedUserInfo> ())
{
  NS_LOG_FUNCTION (this);
}

McpttFloorQueue::McpttFloorQueue (const std::list<McpttQueuedUserInfo>& users, uint16_t capacity)
  : Object (),
    m_capacity (capacity),
    m_users (users)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorQueue::~McpttFloorQueue (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorQueue::Clear (void)
{
  NS_LOG_FUNCTION (this);

  m_users.clear ();
}

bool
McpttFloorQueue::Contains (uint32_t userId) const
{
  uint16_t position = Find (userId);

  bool contained = (position > 0);

  return contained;
}

McpttQueuedUserInfo
McpttFloorQueue::Dequeue (void)
{
  NS_LOG_FUNCTION (this);

  McpttQueuedUserInfo user = Peek ();

  McpttFloorMsgFieldQueuePositionInfo queueInfo = user.GetInfo ();
  queueInfo.SetPosition (0);
  user.SetInfo (queueInfo);

  m_users.pop_front ();

  return user;
}

void
McpttFloorQueue::Enqueue (McpttQueuedUserInfo& user)
{
  NS_LOG_FUNCTION (this);

  uint16_t position = 1;
  std::list<McpttQueuedUserInfo>::iterator it = m_users.begin ();
  while (it != m_users.end () && user.GetInfo ().GetPriority () <= it->GetInfo ().GetPriority ())
    {
      it++;
    }
  McpttFloorMsgFieldQueuePositionInfo queuedInfo = user.GetInfo ();
  queuedInfo.SetPosition (position);
  user.SetInfo (queuedInfo);
  m_users.insert (it, user);
}

uint16_t
McpttFloorQueue::Find (uint32_t userId) const
{
  NS_LOG_FUNCTION (this << userId);

  uint16_t position = 0;
  McpttQueuedUserInfo temp;

  View (userId, temp, position);

  return position;
}

uint16_t
McpttFloorQueue::GetCount (void) const
{
  uint16_t count = m_users.size ();

  return count;
}

TypeId
McpttFloorQueue::GetInstanceTypeId (void) const
{
  return McpttFloorQueue::GetTypeId ();
}

bool
McpttFloorQueue::IsAtCapacity (void) const
{
  uint16_t count = GetCount ();

  bool isAtCapacity = (count >= m_capacity);

  return isAtCapacity;
}

bool
McpttFloorQueue::IsEnabled (void) const
{
  bool isEnabled = (m_capacity > 0);

  return isEnabled;
}

bool
McpttFloorQueue::HasNext (void) const
{
  uint16_t count = GetCount ();

  bool hasNext = (count > 0);

  return hasNext;
}

McpttQueuedUserInfo
McpttFloorQueue::Peek (void) const
{
  McpttQueuedUserInfo next = m_users.front ();

  McpttFloorMsgFieldQueuePositionInfo queueInfo = next.GetInfo ();
  queueInfo.SetPosition (1);
  next.SetInfo (queueInfo);

  return next;
}

bool
McpttFloorQueue::Pull (uint32_t userId)
{
  NS_LOG_FUNCTION (this << userId);

  for (std::list<McpttQueuedUserInfo>::iterator it = m_users.begin (); it != m_users.end (); it++)
    {
      McpttFloorMsgFieldUserId idField = it->GetUserId ();
      uint32_t tempUserId = idField.GetUserId ();

      if (tempUserId == userId)
        {
          m_users.erase (it);
          return true;
        }
    }

  return false;
}

void
McpttFloorQueue::UpdateUsers (const std::list<McpttQueuedUserInfo>& users)
{
  NS_LOG_FUNCTION (this << &users);
  m_users = users;
}

bool
McpttFloorQueue::View (uint32_t userId, McpttQueuedUserInfo& info, uint16_t& position) const
{
  NS_LOG_FUNCTION (this << userId);

  position = 0;
  uint16_t count = 1;
  for (std::list<McpttQueuedUserInfo>::const_iterator it = m_users.begin (); it != m_users.end () && position == 0; it++)
    {
      McpttFloorMsgFieldUserId idField = it->GetUserId ();
      uint32_t tempUserId = idField.GetUserId ();

      if (tempUserId == userId)
        {
          position = count;
          info = *it;

          McpttFloorMsgFieldQueuePositionInfo queueInfo = info.GetInfo ();
          queueInfo.SetPosition (position);
          info.SetInfo (queueInfo);
        }

      count++;
    }

  bool found = (position > 0);

  return found;
}

std::list<McpttQueuedUserInfo>
McpttFloorQueue::ViewUsers (void) const
{
  return m_users;
}

void
McpttFloorQueue::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_capacity = 0;
  m_users.clear ();
}

} // namespace psc
} // namespace ns3

