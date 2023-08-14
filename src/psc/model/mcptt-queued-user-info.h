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

#ifndef MCPTT_QUEUED_USER_INFO_H
#define MCPTT_QUEUED_USER_INFO_H

#include <iostream>

#include <ns3/buffer.h>
#include <ns3/object-base.h>
#include <ns3/type-id.h>

#include "mcptt-floor-msg-field.h"

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class is used as a struct to store necessary information for a queued
 * MCPTT user. This includes the user's priority, queue position, SSRC, and
 * user ID.
 */
class McpttQueuedUserInfo : public ObjectBase
{
  friend std::ostream& operator<< (std::ostream& os, const McpttQueuedUserInfo& info);

public:
  /**
   * Gets the type ID of the McpttQueuedUserInfo class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttQueuedUserInfo class.
   */
  McpttQueuedUserInfo (void);
  /**
   * Creates an instance of the McpttQueuedUserInfo class.
   * \param ssrc The SSRC of the queued floor participant.
   * \param userId The queued user ID field.
   * \param info The queue info field.
   */
  McpttQueuedUserInfo (uint32_t ssrc, const McpttFloorMsgFieldQueuedUserId& userId, const McpttFloorMsgFieldQueuePositionInfo& info);
  /**
   * The destructor of the McpttQueuedUserInfo class.
   */
  virtual ~McpttQueuedUserInfo (void);
  /**
   * Reads the contents of the message from the byte stream.
   * \param buff The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator& buff);
  /**
   * Gets the serialized size (in bytes) of the queued user information.
   * \returns The serialized size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Gets the type ID of this instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the message to the byte stream.
   * \param buff The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator& buff) const;

private:
  McpttFloorMsgFieldQueuePositionInfo m_info; //!< The queue info field.
  uint32_t m_ssrc; //!< The SSRC of the queued floor participant.
  McpttFloorMsgFieldQueuedUserId m_userId; //!< The queued user ID field.

public:
  /**
   * Gets the queue info field.
   * \returns The queue info field.
   */
  virtual McpttFloorMsgFieldQueuePositionInfo GetInfo (void) const;
  /**
   * Gets the SSRC of the queued floor participant.
   * \returns The SSRC.
   */
  virtual uint32_t GetSsrc (void) const;
  /**
   * Gets the queued user ID field.
   * \returns The queued user ID field.
   */
  virtual McpttFloorMsgFieldQueuedUserId GetUserId (void) const;
  /**
   * Sets the queue info field.
   * \param info The queue info field.
   */
  virtual void SetInfo (const McpttFloorMsgFieldQueuePositionInfo& info);
  /**
   * Sets the SSRC of the queued floor particpant.
   * \param ssrc The SSRC.
   */
  virtual void SetSsrc (uint32_t ssrc);
  /**
   * Sets the queued user ID field.
   * \param userId The queued user ID field.
   */
  virtual void SetUserId (const McpttFloorMsgFieldQueuedUserId& userId);
};
/**
 * The output operator of the McpttQueuedUserInfo class.
 * \param os The output stream to write the given instance to.
 * \param info The McpttQueuedUserInfo instance to write to the stream.
 * \returns The stream that the the given instance was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttQueuedUserInfo& info);

} // namespace psc
} // namespace ns3

#endif /** MCPTT_QUEUED_USER_INFO_H **/

