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

#ifndef MCPTT_ON_NETWORK_FLOOR_PARTICIPANT_STATE_H
#define MCPTT_ON_NETWORK_FLOOR_PARTICIPANT_STATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-entity-id.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-queue.h"
#include "mcptt-media-msg.h"

namespace ns3 {

namespace psc {

class McpttOnNetworkFloorParticipant;

/**
 * \ingroup mcptt
 *
 * This class represents a floor control state machine state as described in TS
 * 24.380 version 14.8.0.
 *
 * 'Start-stop'
 *  - When a new instance of the state machine is created, before any floor
 *    control related input is applied, the state machine is in the
 *    'Start-stop' state. Similarly, when the call is relased or the floor
 *    control session is terminated, the state machine shall return to the
 *    'Start-stop' state.
 *
 * 'U: has no permission'
 *  - In this state the MCPTT client does not have permission to send media or
 *    is not waiting for a floor control response.
 *
 * 'U: has permission'
 *  - In this state the MCPTT client is permitted to send media.
 *
 * 'U: pending request'
 *  - In this state the MCPTT client is waiting for a response to a "Floor
 *    request" message.
 *
 * 'U: pending release'
 *  - In this state the MCPTT client is waiting for response to a "Floor
 *    Release" message.
 *
 * 'Releasing'
 *  - In this state the MCPTT client is waiting for application and signaling
 *    plane to finalized disconnection of call.
 *
 * 'U: queued'
 *  - In this state the MCPTT client has sent a "Floor Request", which was
 *    queued by the server and is now awaiting the "Floor Granted" message.
 */
class McpttOnNetworkFloorParticipantState : public SimpleRefCount<McpttOnNetworkFloorParticipantState>
{
public:
  /**
   * \brief The destructor of the McpttOnNetworkFloorParticipantState class.
   */
  virtual ~McpttOnNetworkFloorParticipantState (void);
  /**
   * Accepts a grant.
   * \param machine The FSM.
   */
  virtual void AcceptGrant (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that the call was initiated.
   * \param machine The FSM.
   */
  virtual void CallInitiated (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that the call was established.
   * \param machine The FSM.
   */
  virtual void CallEstablished (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that the call was released (part I).
   * \param machine The FSM.
   */
  virtual void CallRelease1 (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that the call was released (part II).
   * \param machine The FSM.
   */
  virtual void CallRelease2 (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that timer T100 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT100 (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that timer T101 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT101 (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that timer T103 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT103 (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that timer T104 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT104 (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that timer T132 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT132 (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates whether or not this machine state has the floor.
   * \param machine The FSM.
   * \returns True, if this state has the floor.
   */
  virtual bool HasFloor (const McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine that media is ready to be sent.
   * \param machine The FSM.
   * \param msg The media message to be sent.
   */
  virtual void MediaReady (McpttOnNetworkFloorParticipant& machine, McpttMediaMsg& msg) const;
  /**
   * Receives a floor ack message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorAck (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgAck& msg) const;
  /**
   * Receives a floor deny message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorDeny (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgDeny& msg) const;
  /**
   * Receives a floor granted message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const;
  /**
   * Receives a floor granted message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const;
  /**
   * Receives a floor queue position info message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionInfo (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgQueuePositionInfo& msg) const;
  /**
   * Receives a floor revoke message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const;
  /**
   * Receives a floor taken message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const;
  /**
   * Sends a floor queue position request message.
   * \param machine The FSM.
   */
  virtual void SendFloorQueuePositionRequest (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Releases a previously made request.
   * \param machine The FSM.
   */
  virtual void ReleaseRequest (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Take actions (if any) to enter the new state, upon a state transition.
   * \param machine The FSM.
   */
  virtual void Selected (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies this machine state that the button has been pushed.
   * \param machine The FSM.
   */
  virtual void PttPush (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies this machine state that the button has been released.
   * \param machine The FSM.
   */
  virtual void PttRelease (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Take actions (if any) to leave the current state, upon a state transition.
   * \param machine The FSM.
   */
  virtual void Unselected (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Syntactic sugar for printing the name (equivalent to
   * GetInstanceStateId ().GetName ())
   * \returns The name of the state.
   */
  virtual std::string GetName (void) const;

protected:
  /**
   * Creates an instance of the McpttOnNetworkFloorParticipantState class.
   */
  McpttOnNetworkFloorParticipantState (void);
};
/**
 * Overloaded output operator for the McpttOnNetworkFloorParticipantState class.
 * \param os The output stream to write to.
 * \param state The state to write to the output stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttOnNetworkFloorParticipantState& state);
/**
 * \ingroup mcptt
 * \brief A class that represents the 'Start-stop' state.
 */
class McpttOnNetworkFloorParticipantStateStartStop : public McpttOnNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorParticipantStateStartStop.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorParticipantStateStartStop> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorParticipantStateStartStop class.
   */
  McpttOnNetworkFloorParticipantStateStartStop (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorParticipantStateStartStop class.
   */
  virtual ~McpttOnNetworkFloorParticipantStateStartStop (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Notifies this machine state that the button has been pushed.
   * \param machine The FSM.
   */
  virtual void PttPush (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that the call was established.
   * \param machine The FSM.
   */
  virtual void CallEstablished (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that the call was initiated.
   * \param machine The FSM.
   */
  virtual void CallInitiated (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Receives a floor ack message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorAck (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgAck& msg) const;
  /**
   * Receives a floor deny message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorDeny (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgDeny& msg) const;
  /**
   * Receives a floor granted message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const;
  /**
   * Receives a floor granted message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const;
  /**
   * Receives a floor queue position info message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionInfo (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgQueuePositionInfo& msg) const;
  /**
   * Receives a floor revoke message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const;
  /**
   * Receives a floor taken message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'U: has no permission' state.
 */
class McpttOnNetworkFloorParticipantStateHasNoPermission : public McpttOnNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorParticipantStateHasNoPermission.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorParticipantStateHasNoPermission> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorParticipantStateHasNoPermission class.
   */
  McpttOnNetworkFloorParticipantStateHasNoPermission (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorParticipantStateHasNoPermission class.
   */
  virtual ~McpttOnNetworkFloorParticipantStateHasNoPermission (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Receives a floor granted message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const;
  /**
   * Receives a floor taken message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const;
  /**
   * Notifies this machine state that the button has been pushed.
   * \param machine The FSM.
   */
  virtual void PttPush (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that timer T103 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT103 (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that it has been selected.
   * \param machine The FSM.
   */
  virtual void Selected (McpttOnNetworkFloorParticipant& machine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'U: pending request' state.
 */
class McpttOnNetworkFloorParticipantStatePendingRequest : public McpttOnNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorParticipantStatePendingRequest.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorParticipantStatePendingRequest> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorParticipantStatePendingRequest class.
   */
  McpttOnNetworkFloorParticipantStatePendingRequest (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorParticipantStatePendingRequest class.
   */
  virtual ~McpttOnNetworkFloorParticipantStatePendingRequest (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Notifies the machine state that it has been selected.
   * \param machine The FSM.
   */
  virtual void Selected (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Receives a floor revoke message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const;
  /**
   * Receives a floor granted message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const;
  /**
   * Receives a floor deny message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorDeny (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgDeny& msg) const;
  /**
   * Notifies the floor machine state that timer T101 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT101 (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const;
  /**
   * Notifies this machine state that the button has been released.
   * \param machine The FSM.
   */
  virtual void PttRelease (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Receives a floor queue position info message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionInfo (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgQueuePositionInfo& msg) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'U: has permission' state.
 */
class McpttOnNetworkFloorParticipantStateHasPermission : public McpttOnNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorParticipantStateHasPermission.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorParticipantStateHasPermission> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorParticipantStateHasPermission class.
   */
  McpttOnNetworkFloorParticipantStateHasPermission (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorParticipantStateHasPermission class.
   */
  virtual ~McpttOnNetworkFloorParticipantStateHasPermission (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates whether or not this machine state has the floor.
   * \param machine The FSM.
   * \returns True, if this state has the floor.
   */
  virtual bool HasFloor (const McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine that media is ready to be sent.
   * \param machine The FSM.
   * \param msg The media message to be sent.
   */
  virtual void MediaReady (McpttOnNetworkFloorParticipant& machine, McpttMediaMsg& msg) const;
  /**
   * Notifies this machine state that the button has been released.
   * \param machine The FSM.
   */
  virtual void PttRelease (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Receives a floor revoke message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const;
  /**
   * Receives a floor granted message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const;
  /**
   * Receives a floor granted message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const;
  /**
   * Receives a floor taken message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const;
  /**
   * Notifies the floor machine state that it has been selected.
   * \param machine The FSM.
   */
  virtual void Selected (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that it has been unselected.
   * \param machine The FSM.
   */
  virtual void Unselected (McpttOnNetworkFloorParticipant& machine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'U: pending release' state.
 */
class McpttOnNetworkFloorParticipantStatePendingRelease : public McpttOnNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorParticipantStatePendingRelease.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorParticipantStatePendingRelease> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorParticipantStatePendingRelease class.
   */
  McpttOnNetworkFloorParticipantStatePendingRelease (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorParticipantStatePendingRelease class.
   */
  virtual ~McpttOnNetworkFloorParticipantStatePendingRelease (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Notifies the floor machine state that timer T100 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT100 (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Receives a floor idle message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const;
  /**
   * Receives a floor taken message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const;
  /**
   * Receives a floor revoke message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const;
  /**
   * Receives a floor granted message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'Releasing' state.
 */
class McpttOnNetworkFloorParticipantStateReleasing : public McpttOnNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorParticipantStateReleasing.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorParticipantStateReleasing> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorParticipantStateReleasing class.
   */
  McpttOnNetworkFloorParticipantStateReleasing (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorParticipantStateReleasing class.
   */
  virtual ~McpttOnNetworkFloorParticipantStateReleasing (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Notifies the floor machine state that the call was released (part II).
   * \param machine The FSM.
   */
  virtual void CallRelease2 (McpttOnNetworkFloorParticipant& machine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'U: queued' state.
 */
class McpttOnNetworkFloorParticipantStateQueued : public McpttOnNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorParticipantStateQueued.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorParticipantStateQueued> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorParticipantStateQueued class.
   */
  McpttOnNetworkFloorParticipantStateQueued (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorParticipantStateQueued class.
   */
  virtual ~McpttOnNetworkFloorParticipantStateQueued (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOnNetworkFloorParticipant& machine, const McpttMediaMsg& msg) const;
  /**
   * Receives a floor taken message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorTaken (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgTaken& msg) const;
  /**
   * Receives a floor granted message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgGranted& msg) const;
  /**
   * Receives a floor revoke message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRevoke (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgRevoke& msg) const;
  /**
   * Receives a floor deny message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorDeny (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgDeny& msg) const;
  /**
   * Notifies this machine state that the button has been released.
   * \param machine The FSM.
   */
  virtual void PttRelease (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Receives a floor queue position info message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionInfo (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgQueuePositionInfo& msg) const;
  /**
   * Receives a floor idle message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorIdle (McpttOnNetworkFloorParticipant& machine, const McpttFloorMsgIdle& msg) const;
  /**
   * Sends a floor queue position request message.
   * \param machine The FSM.
   */
  virtual void SendFloorQueuePositionRequest (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that timer T104 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT104 (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Accepts a grant.
   * \param machine The FSM.
   */
  virtual void AcceptGrant (McpttOnNetworkFloorParticipant& machine) const;
  /**
   * Notifies the floor machine state that timer T1 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT132 (McpttOnNetworkFloorParticipant& machine) const;
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_ON_NETWORK_FLOOR_PARTICIPANT_STATE_H */

