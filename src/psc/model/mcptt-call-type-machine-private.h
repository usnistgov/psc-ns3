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

#ifndef MCPTT_CALL_TYPE_MACHINE_PRIVATE_H
#define MCPTT_CALL_TYPE_MACHINE_PRIVATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/traced-callback.h>

#include "mcptt-call-msg.h"
#include "mcptt-call-type-machine.h"
#include "mcptt-call-type-machine-private-state.h"
#include "mcptt-emerg-alert-machine.h"
#include "mcptt-entity-id.h"

namespace ns3 {

namespace psc {

class McpttCallMachinePrivate;

/**
 * \ingroup mcptt
 *
 * This class represents the basic MCPTT call control call type state machine
 * described in TS 24.379 v14.4.0.
 */
class McpttCallTypeMachinePrivate : public McpttCallTypeMachine
{
public:
  /**
   * Gets the type ID of the McpttCallTypeMachinePrivate class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttCallTypeMachinePrivate class.
   * \param owner The owner of this machine.
   */
  McpttCallTypeMachinePrivate (Ptr<McpttCallMachinePrivate> owner = 0);
  /**
   * \brief The destructor of the McpttCallTypeMachinePrivate class.
   */
  virtual ~McpttCallTypeMachinePrivate (void);
  /**
   * Changes the state of the call machine.
   * \param state The state to change to.
   */
  virtual void ChangeState (Ptr<McpttCallTypeMachinePrivateState>  state);
  /**
   * Downgrade the current call type.
   */
  virtual void Downgrade (void);
  /**
   * The timer Tfp1 expired.
   */
  virtual void ExpiryOfTfp1 (void);
  /**
   * The timer Tfp6 expired.
   */
  virtual void ExpiryOfTfp6 (void);
  /**
   * The timer Tfp8 expired.
   */
  virtual void ExpiryOfTfp8 (void);
  /**
   * Gets the type ID of this McpttCallTypeMachinePrivate instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the ID of the current state.
   * \returns The state ID.
   */
  virtual McpttEntityId GetStateId (void) const;
  /**
   * Initiate a private call.
   */
  virtual void InitiateCall (void);
  /**
   * Indicates if the call type machine has been started.
   * \returns True, if the call type machine has been started, otherwise false.
   */
  virtual bool IsStarted (void) const;
  /**
   * Reception of a "PRIVATE CALL ACCEPT" message.
   * \param msg The received message.
   */
  virtual void ReceiveAccept (const McpttCallMsgPrivateAccept& msg);
  /**
   * Reception of a "PRIVATE CALL ACCEPT ACK" message.
   * \param msg The received message.
   */
  virtual void ReceiveAcceptAck (const McpttCallMsgPrivateAcceptAck& msg);
  /**
   * Reception of a "PRIVATE CALL EMERGENCY CANCEL" message.
   * \param msg The received message.
   */
  virtual void ReceiveEmergencyCancel (const McpttCallMsgPrivateEmergCancel& msg);
  /**
   * Reception of a "PRIVATE CALL EMERGENCY CANCEL ACK" message.
   * \param msg The received message.
   */
  virtual void ReceiveEmergencyCancelAck (const McpttCallMsgPrivateEmergCancelAck& msg);
  /**
   * Reception of a "PRIVATE CALL REJECT" message.
   * \param msg The received message.
   */
  virtual void ReceiveReject (const McpttCallMsgPrivateReject& msg);
  /**
   * Reception of a "PRIVATE CALL RELEASE" message.
   * \param msg The received message.
   */
  virtual void ReceiveRelease (const McpttCallMsgPrivateRelease& msg);
  /**
   * Reception of a "PRIVATE CALL SETUP REQUEST" message.
   * \param msg The received message.
   */
  virtual void ReceiveSetupRequest (const McpttCallMsgPrivateSetupReq& msg);
  /**
   * Release a group call.
   */
  virtual void ReleaseCall (void);
  /**
   * Sends a call control message.
   * \param msg The message to send.
   */
  virtual void Send (const McpttCallMsg& msg);
  /**
   * Starts the call type machine.
   */
  virtual void Start (void);
  /**
   * Stops the call type machine.
   */
  virtual void Stop (void);
  /**
   * Upgrade the current call type.
   * \param callType The call type to upgrade to.
   */
  virtual void UpgradeTo (const McpttCallMsgFieldCallType& callType);

protected:
  /**
   * Disposes of the McpttCallTypeMachinePrivate.
   */
  virtual void DoDispose (void);

private:
  McpttCallMsgFieldCallType m_callType; //!< The current call type.
  Callback<void> m_downgradeCb; //!< The downgrade callback.
  /* NOTE:
   * Using a raw pointer here because this type represents
   * an interface that does not inherit from any of the ns3
   * base classes. Therefore, this class should not allocate
   * or dispose of the memory referenced by this pointer, and
   * it is up to the user to provide a valid object. This
   * pointer's sole purpose is to provide access to the
   * object that the user has provided.
   */
  Ptr<McpttCallMachinePrivate> m_owner; //!< The owner of this call type machine
  uint8_t m_priority; //!< The ProSe per-packet priority.
  bool m_started; //!< The flag that indicates if the machine has been started.
  Ptr<McpttCallTypeMachinePrivateState> m_state; //!< The current state of the machine.
  TracedCallback<uint32_t, uint16_t, const std::string&, const std::string&, const std::string&, const std::string&> m_stateChangeTrace; //!< The state change traced callback.
  Callback<void, uint8_t> m_upgradeCb; //!< The downgrade callback.

public:
  /**
   * Gets the current call type.
   * \returns The call type.
   */
  virtual McpttCallMsgFieldCallType GetCallType (void) const;
  /**
   * Gets the owner of this call type machine.
   * \returns The owner.
   */
  virtual Ptr<McpttCallMachinePrivate> GetOwner (void) const;
  /**
   * Gets the ProSe per-packet priority.
   * \returns The ProSe per-packet priority.
   */
  virtual uint8_t GetPriority (void) const;
  /**
   * Gets the flag that indicates if the call type machine has been started.
   * \returns The flag.
   */
  virtual bool GetStarted (void) const;
  /**
   * Gets the current state of the call machine.
   * \returns The current state.
   */
  virtual Ptr<McpttCallTypeMachinePrivateState> GetState (void) const;
  /**
   * Sets the downgrade callback.
   * \param downgradeCb The downgrade callback.
   */
  virtual void SetDowngradeCb (const Callback<void>  downgradeCb);
  /**
   * Sets the current call type.
   * \param callType The current call type.
   */
  virtual void SetCallType (const McpttCallMsgFieldCallType& callType);
  /**
   * Sets the owner of this call type machine.
   * \param owner The owner.
   */
  virtual void SetOwner (Ptr<McpttCallMachinePrivate> owner);
  /**
   * Sets the ProSe per-packet priority.
   * \param priority The ProSe per-packet priority.
   */
  virtual void SetPriority (uint8_t priority);
  /**
   * Sets the flag that indicates if the call type machine has been started.
   * \param started The flag.
   */
  virtual void SetStarted (const bool& started);
  /**
   * Sets the current state of the call machine.
   * \param state The current state.
   */
  virtual void SetState (Ptr<McpttCallTypeMachinePrivateState>  state);
  /**
   * Sets the upgrade callback.
   * \param upgradeCb The upgrade callback.
   */
  virtual void SetUpgradeCb (const Callback<void, uint8_t>  upgradeCb);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_TYPE_MACHINE_PRIVATE_H */

