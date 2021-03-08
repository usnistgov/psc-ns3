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

#ifndef MCPTT_MSG_H
#define MCPTT_MSG_H

#include <iostream>

#include <ns3/header.h>
#include <ns3/simple-ref-count.h>
#include <ns3/type-id.h>

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This is the base class of all MCPTT messages. And provides a simple
 * function that wraps around the ns3::TypeId::IsChildOf to determine if a
 * message is of a particular type using the "IsA" function.
 */
class McpttMsg : public Header
{
public:
  /**
   * Gets the type ID of the McpttMsg class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * The destructor of the McpttMsg class.
   */
  virtual ~McpttMsg (void);
  /**
   * Gets the type ID of this McpttMsg instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Indicates whether or not the message is a given type.
   * \param type The message type.
   * \returns True, if this message matches the given type, or false otherwise.
   */
  virtual bool IsA (const TypeId& type) const;
  virtual uint32_t        Deserialize (Buffer::Iterator start)
  {
    return 0;
  }

  virtual uint32_t        GetSerializedSize (void) const
  {
    return 0;
  }

  virtual void    Print (std::ostream &os) const
  { }

  virtual void    Serialize (Buffer::Iterator start) const
  { }

protected:
  /**
  * Creates an instance of the McpttMsg class.
  */
  McpttMsg (void);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_MSG_H */

