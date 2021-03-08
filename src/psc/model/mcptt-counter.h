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

#ifndef MCPTT_COUNTER_H
#define MCPTT_COUNTER_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-entity-id.h"

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class is used to represent a counter, providing an interface similar to
 * the actions requested of counters from TS 24.379 and TS 24.380 v14.4.0.
 *
 * NOTE: the initial value of this counter, if not specified, is one NOT zero.
 */
class McpttCounter : public Object
{
  friend std::ostream& operator<< (std::ostream& os, const McpttCounter& counter);

public:
  /**
   * Gets the type ID of the  class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCounter class.
   */
  McpttCounter (void);
  /**
   * Creates an instance of the McpttCounter class.
   * \param id The id of the counter.
   * \param start The starting value of the counter.
   * \param limit The limit of the counter.
   */
  McpttCounter (const McpttEntityId& id, uint32_t start = 1, uint32_t limit = 3);
  /**
   * \brief The destructor of the McpttCounter class.
   */
  virtual ~McpttCounter (void);
  /**
   * Gets the type ID of this McpttCounter instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Increments the counter.
   * \param amount The amount by which to increment the counter.
   */
  virtual void Increment (uint32_t amount = 1);
  /**
   * Indicates whether or not the limit has been exceeded.
   * \returns True, if the limit has been exceeed, or false otherwise.
   */
  virtual bool IsLimitExceeded (void) const;
  /**
   * Indicates whether or not the limit has been reached.
   * \returns True, if the limit has been reached, or false otherwise.
   */
  virtual bool IsLimitReached (void) const;
  /**
   * Prints the counter to the givne output stream.
   * \param os The given output stream.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Resets the the counter.
   */
  virtual void Reset ();

private:
  McpttEntityId m_id; //!< The ID of the counter.
  uint32_t m_limit; //!< The limit of the counter.
  uint32_t m_start; //!< The starting value.
  uint32_t m_value; //!< The value of the counter.

public:
  /**
   * Gets the ID of the counter.
   * \returns The ID.
   */
  virtual McpttEntityId GetId (void) const;
  /**
   * Gets the limit of the counter.
   * \returns limit The limit.
   */
  virtual uint32_t GetLimit (void) const;
  /**
   * Gets the start value.
   * \returns The start value.
   */
  virtual uint32_t GetStart (void) const;
  /**
   * Gets the value of the counter.
   * \returns The value.
   */
  virtual uint32_t GetValue (void) const;
  /**
   * Sets the ID of the counter.
   * \param id The ID of the counter.
   */
  virtual void SetId (const McpttEntityId& id);
  /**
   * Sets the limit of the counter.
   * \param limit The limit.
   */
  virtual void SetLimit (uint32_t limit);
  /**
   * Sets the start value.
   * \param start The start value.
   */
  virtual void SetStart (uint32_t start);
  /**
   * Sets the value of the counter.
   * \param value The value.
   */
  virtual void SetValue (uint32_t value);
};
/**
 * The output operator for the McpttCounter class.
 * \param os The output stream to write to.
 * \param counter the counter to write.
 * \returns The outputstream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttCounter& counter);

} // namespace psc
} // namespace ns3

#endif /* MCPTT_COUNTER_H */
