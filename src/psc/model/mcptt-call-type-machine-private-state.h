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

#ifndef MCPTT_CALL_TYPE_MACHINE_PRIVATE_STATE_H
#define MCPTT_CALL_TYPE_MACHINE_PRIVATE_STATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg.h"
#include "mcptt-entity-id.h"

namespace ns3 {

namespace psc {

class McpttCallTypeMachinePrivate;

/**
 * \ingroup mcptt
 *
 * This class represents an MCPTT private call call control state machine
 * state as described in TS 24.379 version 14.4.0.
 *
 * Q0: waiting for the call to be established
 *  - This state is the start state.
 *
 * Q1: in-progress private call
 *  - This state exists for a UE, when the UE is part of an in-progress
 *    private call
 *
 * Q2: in-progress emergency private call
 *  - This state exists or a UE, when the UE is part of an in-progress
 *    emergency private call
 */
class McpttCallTypeMachinePrivateState : public SimpleRefCount<McpttCallTypeMachinePrivateState>
{
public:
  /**
   * \brief The destructor of the McpttCallTypeMachinePrivateState class.
   */
  virtual ~McpttCallTypeMachinePrivateState (void);
  /**
   * Downgrade the current call type.
   * \param machine The state machine that the action is for.
   */
  virtual void Downgrade (McpttCallTypeMachinePrivate& machine);
  /**
   * Indicates that the timer TFP1 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp1 (McpttCallTypeMachinePrivate& machine);
  /**
   * Indicates that the timer TFP6 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp6 (McpttCallTypeMachinePrivate& machine);
  /**
   * Indicates that the timer TFP8 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp8 (McpttCallTypeMachinePrivate& machine);
  /**
   * Gets the state ID of this McpttCallTypeMachinePrivateState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const = 0;
  /**
   * Indicates that a group call should be initiated.
   * \param machine The state machine that the action is for.
   */
  virtual void InitiateCall (McpttCallTypeMachinePrivate& machine);
  /**
   * Reception of a "PRIVATE CALL ACCEPT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveAccept (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg);
  /**
   * Reception of a "PRIVATE CALL ACCEPT ACK" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveAcceptAck (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAcceptAck& msg);
  /**
   * Reception of a "PRIVATE CALL EMERGENCY CANCEL" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveEmergencyCancel (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateEmergCancel& msg);
  /**
   * Reception of a "PRIVATE CALL EMERGENCY CANCEL ACK" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveEmergencyCancelAck (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateEmergCancelAck& msg);
  /**
   * Reception of a "PRIVATE CALL REJECT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveReject (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateReject& msg);
  /**
   * Reception of a "PRIVATE CALL RELEASE" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveRelease (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateRelease& msg);
  /**
   * Reception of a "PRIVATE CALL SETUP REQUEST" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveSetupRequest (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttCallTypeMachinePrivate& machine);
  /**
   * Upgrade the current call type.
   * \param machine The state machine that the action is for.
   * \param callType The call type to upgrade to.
   */
  virtual void UpgradeTo (McpttCallTypeMachinePrivate& machine, const McpttCallMsgFieldCallType& callType);

protected:
  /**
   * Creates an instance of the McpttCallTypeMachinePrivateState class.
   */
  McpttCallTypeMachinePrivateState (void);
};
/**
 * The output operator of the McpttCallTypeMachinePrivateState class.
 * \param os The output stream to write to.
 * \param inst The instance to write to the output stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttCallTypeMachinePrivateState& inst);
/**
 * \ingroup mcptt
 *
 * This class represents The 'Q0: waiting for the call to be established' state
 * of a private call type state machine.
 */
class McpttCallTypeMachinePrivateStateQ0 : public McpttCallTypeMachinePrivateState
{
public:
  /**
   * Gets an instance of the McpttCallTypeMachinePrivateStateQ0 class.
   * \returns The P0 instance.
   */
  static Ptr<McpttCallTypeMachinePrivateStateQ0> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallTypeMachinePrivateStateQ0 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallTypeMachinePrivateStateQ0 class.
   */
  McpttCallTypeMachinePrivateStateQ0 (void);
  /**
   * \brief The destructor of the McpttCallTypeMachinePrivateStateQ0 class.
   */
  virtual ~McpttCallTypeMachinePrivateStateQ0 (void);
  /**
   * Gets the state ID of this McpttCallTypeMachinePrivateState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates that a group call should be initiated.
   * \param machine The state machine that the action is for.
   */
  virtual void InitiateCall (McpttCallTypeMachinePrivate& machine);
  /**
   * Reception of a "PRIVATE CALL ACCEPT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveAccept (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg);
  /**
   * Reception of a "PRIVATE CALL ACCEPT ACK" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveAcceptAck (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAcceptAck& msg);
  /**
   * Reception of a "PRIVATE CALL SETUP REQUEST" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveSetupRequest (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'Q1: in-progress private call' state of a private
 * call type state machine.
 */
class McpttCallTypeMachinePrivateStateQ1 : public McpttCallTypeMachinePrivateState
{
public:
  /**
   * Gets an instance of the McpttCallTypeMachinePrivateStateQ1 class.
   * \returns The P1 instance.
   */
  static Ptr<McpttCallTypeMachinePrivateStateQ1> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallTypeMachinePrivateStateQ1 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallTypeMachinePrivateStateQ1 class.
   */
  McpttCallTypeMachinePrivateStateQ1 (void);
  /**
   * \brief The destructor of the McpttCallTypeMachinePrivateStateQ1 class.
   */
  virtual ~McpttCallTypeMachinePrivateStateQ1 (void);
  /**
   * Indicates that the timer TFP6 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp6 (McpttCallTypeMachinePrivate& machine);
  /**
   * Gets the state ID of this McpttCallTypeMachinePrivateState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Reception of a "PRIVATE CALL ACCEPT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveAccept (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg);
  /**
   * Reception of a "PRIVATE CALL EMERGENCY CANCEL" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveEmergencyCancel (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateEmergCancel& msg);
  /**
   * Reception of a "PRIVATE CALL EMERGENCY CANCEL ACK" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveEmergencyCancelAck (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateEmergCancelAck& msg);
  /**
   * Reception of a "PRIVATE CALL SETUP REQUEST" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveSetupRequest (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg);
  /**
   * Upgrade the current call type.
   * \param machine The state machine that the action is for.
   * \param callType The call type to upgrade to.
   */
  virtual void UpgradeTo (McpttCallTypeMachinePrivate& machine, const McpttCallMsgFieldCallType& callType);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttCallTypeMachinePrivate& machine);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'Q2: in-progress emergency private call' state of
 * a private call type state machine.
 */
class McpttCallTypeMachinePrivateStateQ2 : public McpttCallTypeMachinePrivateState
{
public:
  /**
   * Gets an instance of the McpttCallTypeMachinePrivateStateQ2 class.
   * \returns The P2 instance.
   */
  static Ptr<McpttCallTypeMachinePrivateStateQ2> GetInstance (void);
  /**
   * Indicates that the timer TFP1 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp1 (McpttCallTypeMachinePrivate& machine);
  /**
   * Indicates that the timer TFP8 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp8 (McpttCallTypeMachinePrivate& machine);
  /**
   * Gets the state ID of the McpttCallTypeMachinePrivateStateQ2 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallTypeMachinePrivateStateQ2 class.
   */
  McpttCallTypeMachinePrivateStateQ2 (void);
  /**
   * \brief The destructor of the McpttCallTypeMachinePrivateStateQ2 class.
   */
  virtual ~McpttCallTypeMachinePrivateStateQ2 (void);
  /**
   * Downgrade the current call type.
   * \param machine The state machine that the action is for.
   */
  virtual void Downgrade (McpttCallTypeMachinePrivate& machine);
  /**
   * Gets the state ID of this McpttCallTypeMachinePrivateState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Reception of a "PRIVATE CALL ACCEPT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveAccept (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg);
  /**
  * Reception of a "PRIVATE CALL EMERGENCY CANCEL" message.
  * \param machine The state machine that the action is for.
  * \param msg The received message.
  */
  virtual void ReceiveEmergencyCancel (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateEmergCancel& msg);
/**
  * Reception of a "PRIVATE CALL REJECT" message.
  * \param machine The state machine that the action is for.
  * \param msg The received message.
  */
  virtual void ReceiveReject (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateReject& msg);
  /**
   * Reception of a "PRIVATE CALL SETUP REQUEST" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveSetupRequest (McpttCallTypeMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttCallTypeMachinePrivate& machine);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_TYPE_MACHINE_PRIVATE_STATE_H */

