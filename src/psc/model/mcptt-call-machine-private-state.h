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

#ifndef MCPTT_CALL_MACHINE_PRIVATE_STATE_H
#define MCPTT_CALL_MACHINE_PRIVATE_STATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg.h"
#include "mcptt-entity-id.h"

namespace ns3 {

namespace psc {

class McpttCallMachinePrivate;

/**
 * \ingroup mcptt
 *
 * This class represents an MCPTT private call call control state machine
 * state as described in TS 24.379 version 14.4.0.
 *
 * P0: start-stop
 *  - In this state no private call control entity exists
 *
 * P1: ignoring same call id
 *  - This state exists for UE, when the UE is not part of an ongoing private
 *    call
 *
 * P2: waiting for call response
 *  - This state exists for UE, when the UE has sent a "PRIVATE CALL SETUP
 *    REQUEST" message and is waiting for a response, "PRIVATE CALL ACCEPT" or
 *    "PRIVATE CALL REJECT" message
 *
 * P3: waiting for release response
 *  - This state exists for UE, when the UE has sent a "PRIVATE CALL RELEASE"
 *    message and is waiting for a "PRIVATE CALL RELEASE ACK" message
 *
 * P4: part of ongoing call
 *  - This state exists for UE, when the UE is part of an ongoing private call
 *
 * P5: pending
 *  - This state exists for UE, when the UE has presented a notification to the
 *    user for the received "PRIVATE CALL SETUP REQUEST" message and is waiting
 *    for a user indication
 */
class McpttCallMachinePrivateState : public SimpleRefCount<McpttCallMachinePrivateState>
{
public:
  /**
   * \brief The destructor of the McpttCallMachinePrivateState class.
   */
  virtual ~McpttCallMachinePrivateState (void);
  /**
   * Indicates that the call should be accepted.
   * \param machine The state machine that the action is for.
   */
  virtual void AcceptCall (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the timer TFP1 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp1 (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the timer TFP2 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp2 (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the timer TFP3 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp3 (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the timer TFP4 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp4 (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the timer TFP5 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp5 (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the timer TFP6 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp6 (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the timer TFP7 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp7 (McpttCallMachinePrivate& machine);
  /**
   * Gets the state ID of this McpttCallMachinePrivateState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const = 0;
  /**
   * Indicates that a group call should be initiated.
   * \param machine The state machine that the action is for.
   */
  virtual void InitiateCall (McpttCallMachinePrivate& machine);
  /**
   * Indicates if the call is active.
   * \param machine The machine that the action is for.
   * \returns True, if the call is active.
   */
  virtual bool IsCallOngoing (const McpttCallMachinePrivate& machine) const;
  /**
   * Reception of a "PRIVATE CALL ACCEPT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveAccept (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg);
  /**
   * Reception of a "PRIVATE CALL ACCEPT ACK" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveAcceptAck (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateAcceptAck& msg);
  /**
   * Reception of a media message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttCallMachinePrivate& machine, const McpttMediaMsg& msg);
  /**
   * Reception of a "PRIVATE CALL REJECT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveReject (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateReject& msg);
  /**
   * Reception of a "PRIVATE CALL RELEASE" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveRelease (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRelease& msg);
  /**
   * Reception of a "PRIVATE CALL RELEASE ACK" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveReleaseAck (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateReleaseAck& msg);
  /**
   * Reception of a "PRIVATE CALL RINGING" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveRinging (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRinging& msg);
  /**
   * Reception of a "PRIVATE CALL SETUP REQUEST" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveSetupRequest (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg);
  /**
   * Indicates that the call should be rejected.
   * \param machine The state machine that the action is for.
   */
  virtual void RejectCall (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttCallMachinePrivate& machine);

protected:
  /**
   * Creates an instance of the McpttCallMachinePrivateState class.
   */
  McpttCallMachinePrivateState (void);
};
/**
 * The output operator of the McpttCallMachinePrivateState class.
 * \param os The output stream to write to.
 * \param inst The instance to write to the output stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttCallMachinePrivateState& inst);
/**
 * \ingroup mcptt
 *
 * This class represents The 'P0: start-stop' state of a private call state
 * machine.
 */
class McpttCallMachinePrivateStateP0 : public McpttCallMachinePrivateState
{
public:
  /**
   * Gets an instance of the McpttCallMachinePrivateStateP0 class.
   * \returns The P0 instance.
   */
  static Ptr<McpttCallMachinePrivateStateP0> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachinePrivateStateP0 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachinePrivateStateP0 class.
   */
  McpttCallMachinePrivateStateP0 (void);
  /**
   * \brief The destructor of the McpttCallMachinePrivateStateP0 class.
   */
  virtual ~McpttCallMachinePrivateStateP0 (void);
  /**
   * Gets the state ID of this McpttCallMachinePrivateState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates that a group call should be initiated.
   * \param machine The state machine that the action is for.
   */
  virtual void InitiateCall (McpttCallMachinePrivate& machine);
  /**
   * Reception of a "PRIVATE CALL SETUP REQUEST" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveSetupRequest (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'P1: ignoring same call id' state of a private
 * call state machine.
 */
class McpttCallMachinePrivateStateP1 : public McpttCallMachinePrivateState
{
public:
  /**
   * Gets an instance of the McpttCallMachinePrivateStateP1 class.
   * \returns The P1 instance.
   */
  static Ptr<McpttCallMachinePrivateStateP1> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachinePrivateStateP1 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachinePrivateStateP1 class.
   */
  McpttCallMachinePrivateStateP1 (void);
  /**
   * \brief The destructor of the McpttCallMachinePrivateStateP1 class.
   */
  virtual ~McpttCallMachinePrivateStateP1 (void);
  /**
   * Indicates that the timer TFP7 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp7 (McpttCallMachinePrivate& machine);
  /**
   * Gets the state ID of this McpttCallMachinePrivateState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates that a group call should be initiated.
   * \param machine The state machine that the action is for.
   */
  virtual void InitiateCall (McpttCallMachinePrivate& machine);
  /**
   * Reception of a "PRIVATE CALL RELEASE" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveRelease (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRelease& msg);
  /**
   * Reception of a "PRIVATE CALL SETUP REQUEST" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveSetupRequest (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateSetupReq& msg);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'P2: waiting for call response' state of a private
 * call state machine.
 */
class McpttCallMachinePrivateStateP2 : public McpttCallMachinePrivateState
{
public:
  /**
   * Gets an instance of the McpttCallMachinePrivateStateP2 class.
   * \returns The P2 instance.
   */
  static Ptr<McpttCallMachinePrivateStateP2> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachinePrivateStateP2 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachinePrivateStateP2 class.
   */
  McpttCallMachinePrivateStateP2 (void);
  /**
   * \brief The destructor of the McpttCallMachinePrivateStateP2 class.
   */
  virtual ~McpttCallMachinePrivateStateP2 (void);
  /**
   * Indicates that the timer TFP1 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp1 (McpttCallMachinePrivate& machine);
/**
  * Indicates that the timer TFP2 has expired.
  * \param machine The state machine that the action is for.
  */
  virtual void ExpiryOfTfp2 (McpttCallMachinePrivate& machine);
  /**
   * Gets the state ID of this McpttCallMachinePrivateState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Reception of a "PRIVATE CALL ACCEPT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveAccept (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateAccept& msg);
  /**
   * Reception of a "PRIVATE CALL REJECT" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveReject (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateReject& msg);
  /**
   * Reception of a "PRIVATE CALL RINGING" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveRinging (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRinging& msg);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttCallMachinePrivate& machine);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'P3: waiting for release response' state of a
 * private call state machine.
 */
class McpttCallMachinePrivateStateP3 : public McpttCallMachinePrivateState
{
public:
  /**
   * Gets an instance of the McpttCallMachinePrivateStateP3 class.
   * \returns The P3 instance.
   */
  static Ptr<McpttCallMachinePrivateStateP3> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachinePrivateStateP3 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachinePrivateStateP3 class.
   */
  McpttCallMachinePrivateStateP3 (void);
  /**
   * \brief The destructor of the McpttCallMachinePrivateStateP3 class.
   */
  virtual ~McpttCallMachinePrivateStateP3 (void);
  /**
   * Indicates that the timer TFP3 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp3 (McpttCallMachinePrivate& machine);
  /**
   * Gets the state ID of this McpttCallMachinePrivateState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Reception of a "PRIVATE CALL RELEASE ACK" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveReleaseAck (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateReleaseAck& msg);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'P4: part of ongoing call' state of a private call
 * state machine.
 */
class McpttCallMachinePrivateStateP4 : public McpttCallMachinePrivateState
{
public:
  /**
   * Gets an instance of the McpttCallMachinePrivateStateP4 class.
   * \returns The P4 instance.
   */
  static Ptr<McpttCallMachinePrivateStateP4> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachinePrivateStateP4 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachinePrivateStateP4 class.
   */
  McpttCallMachinePrivateStateP4 (void);
  /**
   * \brief The destructor of the McpttCallMachinePrivateStateP4 class.
   */
  virtual ~McpttCallMachinePrivateStateP4 (void);
  /**
   * Indicates that the timer TFP1 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp1 (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the timer TFP5 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp5 (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the timer TFP6 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp6 (McpttCallMachinePrivate& machine);
  /**
   * Gets the state ID of this McpttCallMachinePrivateState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates if the call is active.
   * \param machine The machine that the action is for.
   * \returns True, if the call is active.
   */
  virtual bool IsCallOngoing (const McpttCallMachinePrivate& machine) const;
  /**
   * Reception of a "PRIVATE CALL RELEASE" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveRelease (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRelease& msg);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttCallMachinePrivate& machine);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'P5: pending' state of a private call state
 * machine.
 */
class McpttCallMachinePrivateStateP5 : public McpttCallMachinePrivateState
{
public:
  /**
   * Gets an instance of the McpttCallMachinePrivateStateP5 class.
   * \returns The P5 instance.
   */
  static Ptr<McpttCallMachinePrivateStateP5> GetInstance (void);
  /**
   * Gets the state ID of the McpttCallMachinePrivateStateP5 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttCallMachinePrivateStateP5 class.
   */
  McpttCallMachinePrivateStateP5 (void);
  /**
   * \brief The destructor of the McpttCallMachinePrivateStateP5 class.
   */
  virtual ~McpttCallMachinePrivateStateP5 (void);
  /**
   * Indicates that the call should be accepted.
   * \param machine The state machine that the action is for.
   */
  virtual void AcceptCall (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the timer TFP2 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp2 (McpttCallMachinePrivate& machine);
  /**
   * Indicates that the timer TFP4 has expired.
   * \param machine The state machine that the action is for.
   */
  virtual void ExpiryOfTfp4 (McpttCallMachinePrivate& machine);
  /**
   * Gets the state ID of this McpttCallMachinePrivateState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Reception of a "PRIVATE CALL ACCEPT ACK" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveAcceptAck (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateAcceptAck& msg);
  /**
   * Reception of a media message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttCallMachinePrivate& machine, const McpttMediaMsg& msg);
  /**
   * Reception of a "PRIVATE CALL RELEASE" message.
   * \param machine The state machine that the action is for.
   * \param msg The received message.
   */
  virtual void ReceiveRelease (McpttCallMachinePrivate& machine, const McpttCallMsgPrivateRelease& msg);
  /**
   * Indicates that the call should be rejected.
   * \param machine The state machine that the action is for.
   */
  virtual void RejectCall (McpttCallMachinePrivate& machine);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_MACHINE_PRIVATE_STATE_H */

