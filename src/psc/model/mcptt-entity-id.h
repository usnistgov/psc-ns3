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

#ifndef MCPTT_ENTITY_ID_H
#define MCPTT_ENTITY_ID_H

#include <iostream>
#include <stdint.h>
#include <string>

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class is used to identify MCPTT objects that are of the same type.
 * There are no restrictions for the numbers or names used and it is up to the
 * user to use this class appropriately as an ID.
 */
class McpttEntityId
{
public:
  /**
   * Creates an instance of the McpttEntityId class.
   */
  McpttEntityId (void);
  /**
   * Creates an instance of the McpttEntityId class.
   * \param number The entity ID number.
   * \param name The entity ID name.
   */
  McpttEntityId (uint32_t number, const std::string& name);
  /**
   * The destructor of the McpttEntityId class.
   */
  virtual ~McpttEntityId (void);
  /**
   * Prints this instance of the McpttEntityId class to an output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;

private:
  std::string m_name; //!< The name of the entity.
  uint32_t m_number; //!< The number of the entity.

public:
  friend bool operator== (const McpttEntityId& rhs, const McpttEntityId& lhs);
  friend bool operator!= (const McpttEntityId& rhs, const McpttEntityId& lhs);
  /**
   * Gets the name of the entity.
   * \returns The name.
   */
  virtual std::string GetName (void) const;
  /**
   * Gets the number of the entity.
   * \returns The number.
   */
  virtual uint32_t GetNumber (void) const;
  /**
   * Sets the name of the entity.
   * \param name The name.
   */
  virtual void SetName (const std::string& name);
  /**
   * Sets the number of the entity.
   * \param number The number.
   */
  virtual void SetNumber (uint32_t number);

private:
  /**
   * Indicates if the given McpttEntityId instance is equal to this instance.
   * Used to implement operator== (client code should use the operator instead)
   * \param other The given instance.
   * \returns True, if this instance is equal to the given instance.
   */
  virtual bool IsEqualTo (const McpttEntityId& other) const;
};
/**
 * The overloaded equality operator for the McpttEntityId class.
 * \param lhs The instance to the left of the operator.
 * \param rhs The instance to the right of the operator.
 * \returns True, if the two arguments are equal.
 */
bool operator== (const McpttEntityId& rhs, const McpttEntityId& lhs);
/**
 * The overloaded inequality operator for the McpttEntityId class.
 * \param lhs The instance to the left of the operator.
 * \param rhs The instance to the right of the operator.
 * \returns True, if the two arguments are not equal.
 */
bool operator!= (const McpttEntityId& rhs, const McpttEntityId& lhs);
/**
 * The output operator for the McpttEntityId class.
 * \param os The output stream to write to.
 * \param id The McpttEntityId instance to output.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttEntityId& id);

} // namespace psc
} // namespace ns3

#endif /* MCPTT_ENTITY_ID_H */

