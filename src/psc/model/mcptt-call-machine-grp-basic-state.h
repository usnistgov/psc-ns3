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

#ifndef MCPTT_CALL_MACHINE_GRP_BASIC_STATE_H
#define MCPTT_CALL_MACHINE_GRP_BASIC_STATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg.h"
#include "mcptt-entity-id.h"

namespace ns3 {

namespace psc {

class McpttCallMachineGrpBasic;

/**
 * \ingroup mcptt
 *
 * This class represents a basic MCPTT group call call control state machine
 * state as described in TS 24.379 version 14.4.0.
 *
 * S1: start-stop
 *  - This state exists for UE, when the UE is not part of an ongoing call
 *  - This state is the start state
 *  - This state is the stop state
 *
 * S2: waiting for call announcement
 *  - This state exists of UE, when the Ue has sent a "GROUP CALL PROBE"
 *    message and is waiting for a "GROUP CALL ANNOUNCEMENT" message
 *
 * S3: part of ongoing call
 *  - This state exists for UE, when the UE is part of an ongoing group call
 *
 * S4: pending user action without confirm indication
 *  - This state exists for UE, when the UE has presented a notification to
 *    the MCPTT user for the received "GROUP CALL ANNOUNCEMENT" message, is
 *    waiting for a response and is not expected to send confirm indication
 *
 * S5: pending user action with confirm indication
 *  - This state exists for UE, when the UE has presented a notification to
 *    the MCPTT user for the received "GROUP CALL ANNOUNCEMENT" message, is
 *    waiting for a response and is expcted to send confirm notification
 *
 * S6: ignoring incoming call announcements
 *  - This state exists for UE, when the group call was rejected or released,
 *    "GROUP CALL ANNOUNCEMENT" message was sent or received and "GROUP CALL
 *    ANNOUNCEMENT" messages continue being received
 *
 * S7: waiting for call anouncement after call release
 *  - This state exists for UE, when the group call was released, "GROUP CALL
 *    ANNOUNCEMENT" message was neither sent nor received and "GROUP CALL
 *    PROBE" was sent
 */
class McpttCallMachineGrpBasicState : public SimpleRefCount<McpttCallMachineGrpBasicState>
{
public:
  /**
   * \brief The destructor of the McpttCallMachineGrpBasicState class.
   */
  virtual ~McpttCallMachineGrpBasicState (void);
  /**
   * Indicates that the call should be accepted.
   * \param machine The state machine that the action is for.
   */
  virtual void AcceptCall (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the timer TFG1 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg1 (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the timer TFG2 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg2 (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the timer TFG3 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg3 (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the timer TFG4 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg4 (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the timer TFG5 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg5 (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the timer TFG6 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg6 (McpttCallMachineGrpBasic& machine);
  /**
   * Gets the state ID of this McpttCallMachineGrpBasicState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates that a group call should be initiated.
   * \param machine The state machine that the action is for.
   */
  virtual void InitiateCall (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates if the call is active.
   * \param machine The machine that the action is for.
   * \returns True, if the call is active.
   */
  virtual bool IsCallOngoing (const McpttCallMachineGrpBasic& machine) const;
  /**
   * Reception of a "GROUP CALL ACCEPT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallAccept (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAccept& msg);
  /**
   * Reception of a "GROUP CALL ANNOUNCEMENT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun& msg);
  /**
   * Reception of a "GROUP CALL PROBE" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallProbe (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpProbe& msg);
  /**
   * Indicates that the call should be rejected.
   * \param machine The state machine that the action is for.
   */
  virtual void RejectCall (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttCallMachineGrpBasic& machine);

protected:
  /**
   * Creates an instance of the McpttCallMachineGrpBasicState class.
   */
  McpttCallMachineGrpBasicState (void);
};
/**
 * The output operator of the McpttCallMachineGrpBasicState class.
 * \param os The output stream to write to.
 * \param inst The instance to write to the output stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttCallMachineGrpBasicState& inst);
/**
 * \ingroup mcptt
 *
 * This class represents The 'S1: start-stop' state of a basic call machine.
 */
class McpttCallMachineGrpBasicStateS1 : public McpttCallMachineGrpBasicState
{
public:
  /**
   * Gets an instance of the McpttCallMachineGrpBasicStateS1 class.
   * \returns The S1 instance.
   */
  static Ptr<McpttCallMachineGrpBasicStateS1> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachineGrpBasicStateS1 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachineGrpBasicStateS1 class.
   */
  McpttCallMachineGrpBasicStateS1 (void);
  /**
   * \brief The destructor of the McpttCallMachineGrpBasicStateS1 class.
   */
  virtual ~McpttCallMachineGrpBasicStateS1 (void);
  /**
   * Gets the state ID of this McpttCallMachineGrpBasicState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates that a group call should be initiated.
   * \param machine The state machine that the action is for.
   */
  virtual void InitiateCall (McpttCallMachineGrpBasic& machine);
  /**
   * Reception of a "GROUP CALL ANNOUNCEMENT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun& msg);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'S2: waiting for call announcement' state of a basic call machine.
 */
class McpttCallMachineGrpBasicStateS2 : public McpttCallMachineGrpBasicState
{
public:
  /**
   * Gets an instance of the McpttCallMachineGrpBasicStateS2 class.
   * \returns The S2 instance.
   */
  static Ptr<McpttCallMachineGrpBasicStateS2> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachineGrpBasicStateS2 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachineGrpBasicStateS2 class.
   */
  McpttCallMachineGrpBasicStateS2 (void);
  /**
   * \brief The destructor of the McpttCallMachineGrpBasicStateS2 class.
   */
  virtual ~McpttCallMachineGrpBasicStateS2 (void);
  /**
   * Indicates that the timer TFG1 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg1 (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the timer TFG3 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg3 (McpttCallMachineGrpBasic& machine);
  /**
   * Gets the state ID of this McpttCallMachineGrpBasicState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Reception of a "GROUP CALL ANNOUNCEMENT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun& msg);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttCallMachineGrpBasic& machine);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'S3: part of ongoing call' state of a basic call
 * machine.
 */
class McpttCallMachineGrpBasicStateS3 : public McpttCallMachineGrpBasicState
{
public:
  /**
   * Gets an instance of the McpttCallMachineGrpBasicStateS3 class.
   * \returns An instance of the S3 state.
   */
  static Ptr<McpttCallMachineGrpBasicStateS3> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachineGrpBasicStateS3 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachineGrpBasicStateS3 class.
   */
  McpttCallMachineGrpBasicStateS3 (void);
  /**
   * \brief The destructor of the McpttCallMachineGrpBasicStateS3 class.
   */
  virtual ~McpttCallMachineGrpBasicStateS3 (void);
  /**
   * Indicates that the timer TFG2 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg2 (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the timer TFG6 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg6 (McpttCallMachineGrpBasic& machine);
  /**
   * Gets the state ID of this McpttCallMachineGrpBasicState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates if the call is active.
   * \param machine The state machine that the action is for.
   * \return True, if the call is active.
   */
  virtual bool IsCallOngoing (const McpttCallMachineGrpBasic& machine) const;
  /**
   * Reception of a "GROUP CALL ACCEPT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallAccept (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAccept& msg);
  /**
   * Reception of a "GROUP CALL ANNOUNCEMENT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun& msg);
  /**
   * Reception of a "GROUP CALL PROBE" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallProbe (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpProbe& msg);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttCallMachineGrpBasic& machine);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'S4: pending user action without confirm
 * indication' state of a basic call machine.
 */
class McpttCallMachineGrpBasicStateS4 : public McpttCallMachineGrpBasicState
{
public:
  /**
   * Gets an instance of the McpttCallMachineGrpBasicStateS4 class.
   * \returns An instance of the S4 state.
   */
  static Ptr<McpttCallMachineGrpBasicStateS4> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachineGrpBasicStateS4 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachineGrpBasicStateS4 class.
   */
  McpttCallMachineGrpBasicStateS4 (void);
  /**
   * \brief The destructor of the McpttCallMachineGrpBasicStateS4 class.
   */
  virtual ~McpttCallMachineGrpBasicStateS4 (void);
  /**
   * Indicates that the call should be accepted.
   * \param machine The state machine that the action is for.
   */
  virtual void AcceptCall (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the timer TFG4 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg4 (McpttCallMachineGrpBasic& machine);
  /**
   * Gets the state ID of this McpttCallMachineGrpBasicState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates if the call is active.
   * \param machine The state machine that the action is for.
   * \returns True, if the call is active.
   */
  virtual bool IsCallOngoing (const McpttCallMachineGrpBasic& machine) const;
  /**
   * Indicates that the call should be rejected.
   * \param machine The state machine that the action is for.
   */
  virtual void RejectCall (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttCallMachineGrpBasic& machine);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'S5: pending user action with confirm indication'
 * state of a basic call machine.
 */
class McpttCallMachineGrpBasicStateS5 : public McpttCallMachineGrpBasicState
{
public:
  /**
   * Gets an instance of the McpttCallMachineGrpBasicStateS5 class.
   * \returns An instance of the S5 state.
   */
  static Ptr<McpttCallMachineGrpBasicStateS5> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachineGrpBasicStateS5 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachineGrpBasicStateS5 class.
   */
  McpttCallMachineGrpBasicStateS5 (void);
  /**
   * \brief The destructor of the McpttCallMachineGrpBasicStateS5 class.
   */
  virtual ~McpttCallMachineGrpBasicStateS5 (void);
  /**
   * Indicates that the call should be accepted.
   * \param machine The state machine that the action is for.
   */
  virtual void AcceptCall (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the timer TFG4 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg4 (McpttCallMachineGrpBasic& machine);
  /**
   * Gets the state ID of this McpttCallMachineGrpBasicState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates if the call is active.
   * \param machine The state machine that the action is for.
   * \returns True, if the call is active.
   */
  virtual bool IsCallOngoing (const McpttCallMachineGrpBasic& machine) const;
  /**
   * Indicates that the call should be rejected.
   * \param machine The state machine that the action is for.
   */
  virtual void RejectCall (McpttCallMachineGrpBasic& machine);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttCallMachineGrpBasic& machine);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'S6: ignoring incoming call announcements' state
 * of a basic call machine.
 */
class McpttCallMachineGrpBasicStateS6 : public McpttCallMachineGrpBasicState
{
public:
  /**
   * Gets an instance of the McpttCallMachineGrpBasicStateS6 class.
   * \returns An instance of the S6 state.
   */
  static Ptr<McpttCallMachineGrpBasicStateS6> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachineGrpBasicStateS6 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachineGrpBasicStateS6 class.
   */
  McpttCallMachineGrpBasicStateS6 (void);
  /**
   * \brief The destructor of the McpttCallMachineGrpBasicStateS6 class.
   */
  virtual ~McpttCallMachineGrpBasicStateS6 (void);
  /**
   * Indicates that the timer TFG5 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg5 (McpttCallMachineGrpBasic& machine);
  /**
   * Gets the state ID of this McpttCallMachineGrpBasicState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates that a group call should be initiated.
   * \param machine The state machine that the action is for.
   */
  virtual void InitiateCall (McpttCallMachineGrpBasic& machine);
  /**
   * Reception of a "GROUP CALL ANNOUNCEMENT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun& msg);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'S7: waiting for call announcement after call
 * release' state of a basic call machine.
 */
class McpttCallMachineGrpBasicStateS7 : public McpttCallMachineGrpBasicState
{
public:
  /**
   * Gets an instance of the McpttCallMachineGrpBasicStateS7 class.
   * \returns An instance of the S7 state.
   */
  static Ptr<McpttCallMachineGrpBasicStateS7> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachineGrpBasicStateS7 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachineGrpBasicStateS7 class.
   */
  McpttCallMachineGrpBasicStateS7 (void);
  /**
   * \brief The destructor of the McpttCallMachineGrpBasicStateS7 class.
   */
  virtual ~McpttCallMachineGrpBasicStateS7 (void);
  /**
   * Indicates that the timer TFG1 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfg1 (McpttCallMachineGrpBasic& machine);
  /**
   * Gets the state ID of this McpttCallMachineGrpBasicState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates that a group call should be initiated.
   * \param machine The state machine that the action is for.
   */
  virtual void InitiateCall (McpttCallMachineGrpBasic& machine);
  /**
   * Reception of a "GROUP CALL ANNOUNCEMENT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveGrpCallAnnoun (McpttCallMachineGrpBasic& machine, const McpttCallMsgGrpAnnoun& msg);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_MACHINE_GRP_BASIC_STATE_H */

