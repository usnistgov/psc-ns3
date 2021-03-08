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

#ifndef MCPTT_EMERG_ALERT_MACHINE_H
#define MCPTT_EMERG_ALERT_MACHINE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg.h"
#include "mcptt-counter.h"
#include "mcptt-timer.h"

namespace ns3 {

namespace psc {

class McpttCallMachineGrp;

/**
 * \ingroup mcptt
 *
 * This interface provides an API common to the emergency alert state machine
 * described in TS 24.379 v14.4.0. This includes functions for starting and
 * canceling an emergency alert.
 */
class McpttEmergAlertMachine : public Object
{
public:
  /**
   * Gets the type ID of the McpttEmergAlertMachine class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the  class.
   */
  McpttEmergAlertMachine (void);
  /**
   * \brief The destructor of the McpttEmergAlertMachine class.
   */
  virtual ~McpttEmergAlertMachine (void);
  /**
   * Indicates that the emergency alert should be canceled.
   */
  virtual void CancelEmergAlert (void) = 0;
  /**
   * Gets the type ID of this McpttEmergAlertMachine instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Indicates if the machine is in the emergency state.
   * \returns True, if the machine is in the emergency state; otherwise, false.
   */
  virtual bool IsInEmergState (void) const = 0;
  /**
   * Indicates if the machine has been started.
   * \returns True, if the machine has been started; false, otherwise.
   */
  virtual bool IsStarted (void) const = 0;
  /**
   * Receive a "GROUP EMERGENCY ALERT" message.
   * \param msg The message.
   */
  virtual void ReceiveGrpCallEmergAlert (const McpttCallMsgGrpEmergAlert& msg) = 0;
  /**
   * Receive a "GROUP EMERGENCY ALERT CANCEL" message.
   * \param msg The message.
   */
  virtual void ReceiveGrpCallEmergAlertCancel (const McpttCallMsgGrpEmergAlertCancel& msg) = 0;
  /**
   * Indicates to the machine that an emergency alert should be sent.
   */
  virtual void SendEmergAlert (void) = 0;
  /**
   * Starts the emergency alert machine.
   */
  virtual void Start (void) = 0;
  /**
   * Stops the emergency alert machine.
   */
  virtual void Stop (void) = 0;
};

/**
 * \ingroup mcptt
 *
 * This class represents a nullMCPTT emergency alert state machine.
 */
class McpttEmergAlertMachineNull : public McpttEmergAlertMachine
{
public:
  /**
   * Gets the type ID of the McpttEmergAlertMachineNull class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the  class.
   */
  McpttEmergAlertMachineNull (void);
  /**
   * \brief The destructor of the McpttEmergAlertMachineNull class.
   */
  virtual ~McpttEmergAlertMachineNull (void);
  /**
   * Indicates that the emergency alert should be canceled.
   */
  virtual void CancelEmergAlert (void);
  /**
   * Gets the type ID of this McpttEmergAlertMachineNull instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Indicates if the machine is in the emergency state.
   * \returns True, if the machine is in the emergency state; otherwise, false.
   */
  virtual bool IsInEmergState (void) const;
  /**
   * Indicates if the machine has been started.
   * \returns True, if the machine has been started; false, otherwise.
   */
  virtual bool IsStarted (void) const;
  /**
   * Receive a "GROUP EMERGENCY ALERT" message.
   * \param msg The message.
   */
  virtual void ReceiveGrpCallEmergAlert (const McpttCallMsgGrpEmergAlert& msg);
  /**
   * Receive a "GROUP EMERGENCY ALERT CANCEL" message.
   * \param msg The message.
   */
  virtual void ReceiveGrpCallEmergAlertCancel (const McpttCallMsgGrpEmergAlertCancel& msg);
  /**
   * Indicates to the machine that an emergency alert should be sent.
   */
  virtual void SendEmergAlert (void);
  /**
   * Starts the emergency alert machine.
   */
  virtual void Start (void);
  /**
   * Stops the emergency alert machine.
   */
  virtual void Stop (void);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_EMERG_ALERT_MACHINE_H */

