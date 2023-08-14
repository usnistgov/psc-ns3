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

#ifndef MCPTT_FLOOR_QUEUE_H
#define MCPTT_FLOOR_QUEUE_H

#include <list>

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-queued-user-info.h"

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class provides an interface for the floor queue that is described in TS
 * 24.380 v14.4.0. This class is only associated with a floor machine for use
 * when queuing is enabled in floor control. One can enable queuing in floor
 * control, simply by setting the capacity of this queue to a value greater
 * than zero.
 */
class McpttFloorQueue : public Object
{
public:
  /**
   * Gets the type ID of the McpttFloorQueue class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttFloorQueue class.
   * \param capacity The maximum number of users that can be queued.
   */
  McpttFloorQueue (uint16_t capacity = 0);
  /**
   * Creates an instance of the McpttFloorQueue class.
   * \param users The collection of queued users.
   * \param capacity The maximum number of users that can be queued.
   */
  McpttFloorQueue (const std::list<McpttQueuedUserInfo>& users, uint16_t capacity = 0);
  /**
   * \brief The destructor of the McpttFloorQueue class.
   */
  virtual ~McpttFloorQueue (void);
  /**
   * Clears the queue.
   */
  virtual void Clear (void);
  /**
   * Indicates whether or not the user with the given ID is in the queue.
   * \param userId The user ID.
   * \returns True, if the user is in the queue.
   */
  virtual bool Contains (uint32_t userId) const;
  /**
   * Removes and returns the next user from the queue.
   * \returns The next user.
   */
  virtual McpttQueuedUserInfo Dequeue (void);
  /**
   * Adds a user to the queue.
   * \param user The user to queue.
   */
  virtual void Enqueue (McpttQueuedUserInfo& user);
  /**
   * Finds the user with the given ID in the queue.
   * \param userId The user ID.
   * \returns The position in the queue, or 0 if the user is not found.
   */
  virtual uint16_t Find (uint32_t userId) const;
  /**
   * Gets the number of users that have been queue.
   * \returns The number of currently queued users.
   */
  virtual uint16_t GetCount (void) const;
  /**
   * Gets the type ID of this McpttFloorQueue instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Indicates whether or not the queue is full.
   * \returns True, if the queue is full.
   */
  virtual bool IsAtCapacity (void) const;
  /**
   * Indicates whether or not the queue is enabled.
   * \returns True, if the queue is enabled.
   */
  virtual bool IsEnabled (void) const;
  /**
   * Indicates whether or not there is at least one queued user.
   * \returns True, if there is at least one queued user.
   */
  virtual bool HasNext (void) const;
  /**
   * Gets the next user in the queue.
   * \returns The next user.
   */
  virtual McpttQueuedUserInfo Peek (void) const;
  /**
   * Pulls the user with the given ID out of the queue.
   * \param userId The user ID of the user to pull.
   * \returns True, if the user was in the queue.
   */
  virtual bool Pull (uint32_t userId);
  /**
   * Updates the collection of queued user info.
   * \param users The collection of queued user info.
   */
  virtual void UpdateUsers (const std::list<McpttQueuedUserInfo>& users);
  /**
   * Gets the information for a queued users.
   * \param [in] userId The ID of the user whose information should be collected.
   * \param [out] info The queued user's information.
   * \param [out] position The user's position in the queue.
   * \returns True, if the user was found and the information was retrieved.
   */
  virtual bool View (uint32_t userId, McpttQueuedUserInfo& info, uint16_t& position) const;
  /**
   * Gets a copy of the collection of queued user info.
   * \returns A copy of the collection of queued users.
   */
  virtual std::list<McpttQueuedUserInfo> ViewUsers (void) const;

protected:
  /**
   * Disposes of the McpttFloorQueue.
   */
  virtual void DoDispose (void);

private:
  uint16_t m_capacity; //!< The maximum number of users that can be queued.
  std::list<McpttQueuedUserInfo> m_users; //!< The collection of queued user info.
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_FLOOR_QUEUE_H */

