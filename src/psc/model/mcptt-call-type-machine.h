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

#ifndef MCPTT_CALL_TYPE_MACHINE_H
#define MCPTT_CALL_TYPE_MACHINE_H

#include <ns3/object.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg-field.h"
#include "mcptt-entity-id.h"

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This interface is used to construct a common API for the MCPTT call type
 * entities described in TS 24.379, version 14.4.0. This includes, upgrading
 * and downgrading the current call type, as well as getting the PPPP of the
 * current call based on the call type.
  */
class McpttCallTypeMachine : public Object
{
public:
  /**
   * Gets the type ID of the McpttCallTypeMachine class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallTypeMachine class.
   */
  McpttCallTypeMachine (void);
  /**
   * \brief The destructor of the McpttCallTypeMachineclass.
   */
  virtual ~McpttCallTypeMachine (void);
  /**
   * Gets the current call type.
   * \returns The call type.
   */
  virtual McpttCallMsgFieldCallType GetCallType (void) const = 0;
  /**
   * Gets the type ID of the object.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const = 0;
  /**
   * Gets the ProSe per-packet priority.
   * \returns The ProSe per-packet priority.
   */
  virtual uint8_t GetPriority (void) const = 0;
  /**
   * Gets the ID of the current state.
   * \returns The state ID.
   */
  virtual McpttEntityId GetStateId (void) const = 0;
  /**
   * Indicates if the call type machine has been started.
   * \returns True, if the machine has been started.
   */
  virtual bool IsStarted (void) const = 0;
  /**
   * Sets the current call type.
   * \param callType The current call type.
   */
  virtual void SetCallType (const McpttCallMsgFieldCallType& callType) = 0;
  /**
   * Sets the downgrade callback.
   * \param downgradeCb The downgrade callback.
   */
  virtual void SetDowngradeCb (const Callback<void>  downgradeCb) = 0;
  /**
   * Sets the ProSe per-packet priority.
   * \param priority The ProSe per-packet priority.
   */
  virtual void SetPriority (uint8_t priority) = 0;
  /**
   * Sets the upgrade callback.
   * \param upgradeCb The upgrade callback.
   */
  virtual void SetUpgradeCb (const Callback<void, uint8_t>  upgradeCb) = 0;
  /**
   * Starts the call type machine.
   */
  virtual void Start (void) = 0;
  /**
   * Stops the call type machine.
   */
  virtual void Stop (void) = 0;
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_TYPE_MACHINE_H */

