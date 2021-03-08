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

#ifndef MCPTT_CALL_MACHINE_GRP_BROADCAST_STATE_H
#define MCPTT_CALL_MACHINE_GRP_BROADCAST_STATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

namespace ns3 {

namespace psc {

class McpttCallMachineGrpBroadcast;

/**
 * \ingroup mcptt
 *
 * This class represents a broadcast MCPTT group call call control state
 * machine state as described in TS 24.379 version 14.4.0.
 *
 * B1: start-stop
 *  - This state exists for UE, when the UE is not part of an ongoing
 *    broadcast group call.
 *
 * B2: in-progress broadcast group call
 *  - This state exists for UE, when the UE is part of an ongoing broadcast
 *    group call.
 *
 * B3: pending user action
 *  - This state exists for the UE, when the UE has persented a notification
 *    to the MCPTT user for the received "GROUP CALL BROADCAST" message, is
 *    waiting for a response and is not expected to send confirm indication.
 *
 * B4: ignoring same call ID
 *  - This state exists for UE, when the group call was rejected or released
 *    and "GROUP CALL BROADCAST" messages continue being received.
 */
class McpttCallMachineGrpBroadcastState : public SimpleRefCount<McpttCallMachineGrpBroadcastState>
{
public:
  /**
   * \brief The destructor of the McpttCallMachineGrpBroadcastState class.
   */
  virtual ~McpttCallMachineGrpBroadcastState (void);
  /**
   * Accepts the call.
   * \param machine The FSM.
   */
  virtual void AcceptCall (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Expirty of timer TFB1.
   * \param machine The FSM.
   */
  virtual void ExpiryOfTfb1 (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Expiry of timer TFB2.
   * \param machine The FSM.
   */
  virtual void ExpiryOfTfb2 (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Expiry of timer TFB3.
   * \param machine The FSM.
   */
  virtual void ExpiryOfTfb3 (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Gets the state ID of this McpttCallMachineGrpBroadcastState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const = 0;
  /**
   * Initiates a call.
   * \param machine The FSM.
   */
  virtual void InitiateCall (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Indicates if there is an ongoing call.
   * \param machine The machine that the action is for.
   * \returns True, if there is an ongoing call.
   */
  virtual bool IsCallOngoing (const McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Receives a "GROUP CALL BROADCAST" message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallBroadcast (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcast& msg) const;
  /**
   * Receives a "GROUP CALL BROADCAST END" message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallBroadcastEnd (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcastEnd& msg) const;
  /**
   * Rejects the call.
   * \param machine The FSM.
   */
  virtual void RejectCall (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Releases the call.
   * \param machine The FSM.
   */
  virtual void ReleaseCall (McpttCallMachineGrpBroadcast& machine) const;

protected:
  /**
   * Creates an instance of the McpttCallMachineGrpBroadcastState class.
   */
  McpttCallMachineGrpBroadcastState (void);
};
/**
 * The output operator of the McpttCallMachineGrpBroadcastState class.
 * \param os The output stream to write to.
 * \param inst The instance to write to the output stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttCallMachineGrpBroadcastState& inst);
/**
 * \ingroup mcptt
 *
 * Represents the B1 state of the Broadcast Group Call state machine.
 */
class McpttCallMachineGrpBroadcastStateB1 : public McpttCallMachineGrpBroadcastState
{
public:
  /**
   * Gets an instance of the McpttCallMachineGrpBroadcastStateB1 class.
   * \returns The instance.
   */
  static Ptr<McpttCallMachineGrpBroadcastStateB1> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachineGrpBroadcastStateB1 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachineGrpBroadcastStateB1 class.
   */
  McpttCallMachineGrpBroadcastStateB1 (void);
  /**
   * \brief The destructor of the McpttCallMachineGrpBroadcastStateB1 class.
   */
  virtual ~McpttCallMachineGrpBroadcastStateB1 (void);
  /**
   * Gets the state ID of this McpttCallMachineGrpBroadcastState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Initiates a call.
   * \param machine The FSM.
   */
  virtual void InitiateCall (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Receives a "GROUP CALL BROADCAST" message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallBroadcast (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcast& msg) const;
};
/**
 * \ingroup mcptt
 *
 * Represents the B2 state of the Broadcast Group Call state machine.
 */
class McpttCallMachineGrpBroadcastStateB2 : public McpttCallMachineGrpBroadcastState
{
public:
  /**
   * Gets an instance of the McpttCallMachineGrpBroadcastStateB2 class.
   * \returns The instance.
   */
  static Ptr<McpttCallMachineGrpBroadcastStateB2> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachineGrpBroadcastStateB2 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachineGrpBroadcastStateB2 class.
   */
  McpttCallMachineGrpBroadcastStateB2 (void);
  /**
   * \brief The destructor of the McpttCallMachineGrpBroadcastStateB2 class.
   */
  virtual ~McpttCallMachineGrpBroadcastStateB2 (void);
  /**
   * Expirty of timer TFB1.
   * \param machine The FSM.
   */
  virtual void ExpiryOfTfb1 (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Expiry of timer TFB2.
   * \param machine The FSM.
   */
  virtual void ExpiryOfTfb2 (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Gets the state ID of this McpttCallMachineGrpBroadcastState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates if there is an ongoing call.
   * \param machine The machine that the action is for.
   * \returns True, if there is an ongoing call.
   */
  virtual bool IsCallOngoing (const McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Receives a "GROUP CALL BROADCAST END" message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallBroadcastEnd (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcastEnd& msg) const;
  /**
   * Releases the call.
   * \param machine The FSM.
   */
  virtual void ReleaseCall (McpttCallMachineGrpBroadcast& machine) const;
};
/**
 * \ingroup mcptt
 *
 * Represents the B3 state of the Broadcast Group Call state machine.
 */
class McpttCallMachineGrpBroadcastStateB3 : public McpttCallMachineGrpBroadcastState
{
public:
  /**
   * Gets an instance of the McpttCallMachineGrpBroadcastStateB3 class.
   * \returns The instance.
   */
  static Ptr<McpttCallMachineGrpBroadcastStateB3> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachineGrpBroadcastStateB3 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachineGrpBroadcastStateB3 class.
   */
  McpttCallMachineGrpBroadcastStateB3 (void);
  /**
   * \brief The destructor of the McpttCallMachineGrpBroadcastStateB3 class.
   */
  virtual ~McpttCallMachineGrpBroadcastStateB3 (void);
  /**
   * Accepts the call.
   * \param machine The FSM.
   */
  virtual void AcceptCall (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Expiry of timer TFB3.
   * \param machine The FSM.
   */
  virtual void ExpiryOfTfb3 (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Gets the state ID of this McpttCallMachineGrpBroadcastState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Rejects the call.
   * \param machine The FSM.
   */
  virtual void RejectCall (McpttCallMachineGrpBroadcast& machine) const;
};
/**
 * \ingroup mcptt
 *
 * Represents the B4 state of the Broadcast Group Call state machine.
 */
class McpttCallMachineGrpBroadcastStateB4 : public McpttCallMachineGrpBroadcastState
{
public:
  /**
   * Gets an instance of the McpttCallMachineGrpBroadcastStateB4 class.
   * \returns The instance.
   */
  static Ptr<McpttCallMachineGrpBroadcastStateB4> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachineGrpBroadcastStateB4 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachineGrpBroadcastStateB4 class.
   */
  McpttCallMachineGrpBroadcastStateB4 (void);
  /**
   * \brief The destructor of the McpttCallMachineGrpBroadcastStateB4 class.
   */
  virtual ~McpttCallMachineGrpBroadcastStateB4 (void);
  /**
   * Expirty of timer TFB1.
   * \param machine The FSM.
   */
  virtual void ExpiryOfTfb1 (McpttCallMachineGrpBroadcast& machine) const;
  /**
   * Gets the state ID of this McpttCallMachineGrpBroadcastState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Receives a "GROUP CALL BROADCAST" message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallBroadcast (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcast& msg) const;
  /**
   * Receives a "GROUP CALL BROADCAST END" message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallBroadcastEnd (McpttCallMachineGrpBroadcast& machine, const McpttCallMsgGrpBroadcastEnd& msg) const;
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_MACHINE_GRP_BROADCAST_STATE_H */

