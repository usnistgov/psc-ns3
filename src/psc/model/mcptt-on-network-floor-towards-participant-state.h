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

#ifndef MCPTT_ON_NETWORK_FLOOR_TOWARDS_PARTICIPANT_STATE_H
#define MCPTT_ON_NETWORK_FLOOR_TOWARDS_PARTICIPANT_STATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-entity-id.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"

namespace ns3 {

namespace psc {

class McpttOnNetworkFloorTowardsParticipant;

/**
 * \ingroup mcptt
 *
 * This class represents a floor control state machine state as described in TS
 * 24.380 version 14.8.0.
 *
 * 'Start-stop'
 *  - The floor control interface is in this state when a new instance of the
 *    'basic floor control operations towards the floor participant' state
 *    machine has been intitated, or when a call has been released.
 *
 * 'U: not permitted and Floor Idle'
 *  - The floor control interface uses this state when the associated floor
 *    participant is not permitted to send media.
 *
 * 'U: not permitted and Floor Taken'
 *  - The floor control interface is in this state when another MCPTT client
 *    has been given permission to send media.
 *
 * 'U: permitted'
 *  - The floor control interface uses this state when the participant has
 *    been given permission to send media.
 *
 * 'U: pending Floor Revoke'
 *  - The floor control interface uses this state during the grace period
 *    after sending a Floor Revoke message.
 *
 * 'U: not permitted but sends media'
 *  - The floor control interface uses this state when it receives RTP media
 *    packets from the MCPTT client and the MCPTT client is not permitted to
 *    send media.
 *
 * 'Releasing'
 *  - The floor control interface is in this state while waiting for the
 *    application and signalling plane to finalize the disconnection of an
 *    MCPTT call or the removal of the participant.
 *
 * 'U: not permitted and initiating'
 *  - The floor control interface uses this state when waiting for the floor
 *    control arbitration logic to finalize the initialization of the state
 *    machine used for temporary group session.
 */
class McpttOnNetworkFloorTowardsParticipantState : public SimpleRefCount<McpttOnNetworkFloorTowardsParticipantState>
{
public:
  /**
   * \brief The destructor of the McpttOnNetworkFloorTowardsParticipantState class.
   */
  virtual ~McpttOnNetworkFloorTowardsParticipantState (void);
  /**
   * Indicates to the floor control server that the call has been initialized.
   * \param machine The FSM.
   */
  virtual void CallInitialized (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Indicates to the floor control server that the call has been released (part I).
   * \param machine The FSM.
   */
  virtual void CallRelease1 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Indicates to the floor control server that the call has been released (part II).
   * \param machine The FSM.
   */
  virtual void CallRelease2 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Indicates to the floor control server that the client has disconnected.
   * \param machine The FSM.
   */
  virtual void ClientRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Enters the given state.
   */
  virtual void Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Notifies the floor machine state that timer T8 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT8 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Receives Floor Queue Position Request message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgQueuePositionRequest& msg) const;
  /**
   * Receives a floor release message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const;
  /**
   * Receives a floor request message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRequest& msg) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttMediaMsg& msg) const;
  /**
   * Notifies the machine state that it has been selected.
   * \param machine The FSM.
   */
  virtual void Selected (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Sends a floor deny message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorDeny (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgDeny& msg) const;
  /**
   * Sends a floor granted message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorGranted (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgGranted& msg) const;
  /**
   * Sends a floor idle message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const;
  /**
   * Sends a floor revoke message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorRevoke (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgRevoke& msg) const;
  /**
   * Sends a floor taken message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const;
  /**
   * Sends a media message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttMediaMsg& msg) const;
  /**
   * Indicates to the floor control server to terminate.
   * \param machine The FSM.
   */
  virtual void Terminate (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Notifies the machine state that it has been unselected.
   * \param machine The FSM.
   */
  virtual void Unselected (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;

protected:
  /**
   * Creates an instance of the McpttOnNetworkFloorTowardsParticipantState class.
   */
  McpttOnNetworkFloorTowardsParticipantState (void);
};
/**
 * Overloaded output operator for the McpttOnNetworkFloorTowardsParticipantState class.
 * \param os The output stream to write to.
 * \param state The state to write to the output stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttOnNetworkFloorTowardsParticipantState& state);
/**
 * \ingroup mcptt
 * \brief A class that represents the 'Start-stop' state.
 */
class McpttOnNetworkFloorTowardsParticipantStateStartStop : public McpttOnNetworkFloorTowardsParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorTowardsParticipantStateStartStop.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateStartStop> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorTowardsParticipantStateStartStop class.
   */
  McpttOnNetworkFloorTowardsParticipantStateStartStop (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorTowardsParticipantStateStartStop class.
   */
  virtual ~McpttOnNetworkFloorTowardsParticipantStateStartStop (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates to the floor control server that the call has been initiated.
   * \param machine The FSM.
   */
  virtual void CallInitialized (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'U: not permitted and Floor Idle' state.
 */
class McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle : public McpttOnNetworkFloorTowardsParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle class.
   */
  McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle class.
   */
  virtual ~McpttOnNetworkFloorTowardsParticipantStateNotPermittedIdle (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Enters the given state.
   */
  virtual void Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Sends a floor taken message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const;
  /**
   * Receives a floor request message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRequest& msg) const;
  /**
   * Sends a floor granted message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorGranted (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgGranted& msg) const;
  /**
   * Sends a floor deny message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorDeny (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgDeny& msg) const;
  /**
   * Receives a floor release message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttMediaMsg& msg) const;
  /**
   * Sends a floor idle message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'U: not permitted and Floor Taken' state.
 */
class McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken : public McpttOnNetworkFloorTowardsParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken class.
   */
  McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken class.
   */
  virtual ~McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Enters the given state.
   */
  virtual void Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Sends a floor idle message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const;
  /**
   * Receives a floor request message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRequest& msg) const;
  /**
   * Receives a floor release message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttMediaMsg& msg) const;
  /**
   * Receives Floor Queue Position Request message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgQueuePositionRequest& msg) const;
  /**
   * Sends a floor granted message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorGranted (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgGranted& msg) const;
  /**
   * Sends a floor taken message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'U: permitted' state.
 */
class McpttOnNetworkFloorTowardsParticipantStatePermitted : public McpttOnNetworkFloorTowardsParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorTowardsParticipantStatePermitted.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorTowardsParticipantStatePermitted> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorTowardsParticipantStatePermitted class.
   */
  McpttOnNetworkFloorTowardsParticipantStatePermitted (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorTowardsParticipantStatePermitted class.
   */
  virtual ~McpttOnNetworkFloorTowardsParticipantStatePermitted (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Enters the given state.
   */
  virtual void Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Receives a floor release message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const;
  /**
   * Sends a floor idle message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const;
  /**
   * Sends a floor revoke message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorRevoke (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgRevoke& msg) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttMediaMsg& msg) const;
  /**
   * Receives a floor request message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRequest& msg) const;
  /**
   * Sends a media message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttMediaMsg& msg) const;
  /**
   * Sends a floor taken message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'U: pending Floor Revoke' state.
 */
class McpttOnNetworkFloorTowardsParticipantStatePendingRevoke : public McpttOnNetworkFloorTowardsParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorTowardsParticipantStatePendingRevoke.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorTowardsParticipantStatePendingRevoke> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorTowardsParticipantStatePendingRevoke class.
   */
  McpttOnNetworkFloorTowardsParticipantStatePendingRevoke (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorTowardsParticipantStatePendingRevoke class.
   */
  virtual ~McpttOnNetworkFloorTowardsParticipantStatePendingRevoke (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Enters the given state.
   */
  virtual void Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Notifies the floor machine state that timer T8 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT8 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttMediaMsg& msg) const;
  /**
   * Receives a floor release message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const;
  /**
   * Sends a floor idle message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const;
  /**
   * Sends a floor taken message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'U: not permitted but sends media' state.
 */
class McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia : public McpttOnNetworkFloorTowardsParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia class.
   */
  McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia class.
   */
  virtual ~McpttOnNetworkFloorTowardsParticipantStateNotPermittedMedia (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Enters the given state.
   */
  virtual void Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Notifies the floor machine state that timer T8 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT8 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Receives a floor release message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'Releasing' state.
 */
class McpttOnNetworkFloorTowardsParticipantStateReleasing : public McpttOnNetworkFloorTowardsParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorTowardsParticipantStateReleasing.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateReleasing> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorTowardsParticipantStateReleasing class.
   */
  McpttOnNetworkFloorTowardsParticipantStateReleasing (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorTowardsParticipantStateReleasing class.
   */
  virtual ~McpttOnNetworkFloorTowardsParticipantStateReleasing (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates to the floor control server that the call has been released (part II).
   * \param machine The FSM.
   */
  virtual void CallRelease2 (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'U: not permitted and initiating' state.
 */
class McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating : public McpttOnNetworkFloorTowardsParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating class.
   */
  McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating class.
   */
  virtual ~McpttOnNetworkFloorTowardsParticipantStateNotPermittedInitiating (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Enters the given state.
   */
  virtual void Enter (Ptr<McpttOnNetworkFloorTowardsParticipant> machine) const;
  /**
   * Sends a floor taken message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgTaken& msg) const;
  /**
   * Sends a floor idle message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgIdle& msg) const;
  /**
   * Receives a floor request message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRequest& msg) const;
  /**
   * Sends a floor granted message.
   * \param machine The FSM.
   * \param msg The message to send.
   */
  virtual void SendFloorGranted (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, McpttFloorMsgGranted& msg) const;
  /**
   * Receives a floor release message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> machine, const McpttFloorMsgRelease& msg) const;
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_ON_NETWORK_FLOOR_TOWARDS_PARTICIPANT_STATE_H */

