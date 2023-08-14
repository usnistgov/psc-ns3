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

#ifndef MCPTT_OFF_NETWORK_FLOOR_PARTICIPANT_STATE_H
#define MCPTT_OFF_NETWORK_FLOOR_PARTICIPANT_STATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-entity-id.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-queue.h"
#include "mcptt-media-msg.h"

namespace ns3 {

namespace psc {

class McpttOffNetworkFloorParticipant;

/**
 * \ingroup mcptt
 *
 * This class represents a floor control state machine state as described in TS
 * 24.380 version 14.4.0.
 *
 * 'Start-stop'
 *  - When a new instance of the state machine is created, before any floor
 *    control related input is applied, the state machne is in the
 *    'Start-stop' state. Similarly, when the call is relased or the floor
 *    control session is terminated, the state machine shall return to the
 *    'Start-stop' state.
 *
 * 'O: silence'
 *  - When in this state the MCPTT client for the session is unaware of any
 *    MCPTT client acting as a floor arbitrator, has not itself initated a
 *    floor control request and is not currently receiving RTP media packets.
 *
 * 'O: has no permission'
 *  - In this state the MCPTT client does not have permission to send media.
 *
 * 'O: has permission'
 *  - In this state the MCPTT client is acting as a floor control server
 *    (floor arbitrator) and has the permission to send media.
 *
 * 'O: pending request'
 *  - In this state the MCPTT client is waiting for a response to a "Floor
 *    request" message.
 *
 * 'O: pending granted'
 *  - In this state the MCPTT client is waiting for another client to take
 *    over the role of floor controller.
 *
 * 'O: queued'
 *  - In this state the MCPTT client is waiting for a response to a queued
 *    request.
 */
class McpttOffNetworkFloorParticipantState : public SimpleRefCount<McpttOffNetworkFloorParticipantState>
{
public:
  /**
   * \brief The destructor of the McpttOffNetworkFloorParticipantState class.
   */
  virtual ~McpttOffNetworkFloorParticipantState (void);
  /**
   * Accepts a grant.
   * \param floorMachine The FSM.
   */
  virtual void AcceptGrant (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T201 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT201 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T203 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT203 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T204 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT204 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T205 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT205 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T206 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT206 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T207 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT207 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T230 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT230 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T233 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT233 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates whether or not this machine state has the floor.
   * \param floorMachine The FSM.
   * \returns True, if this state has the floor.
   */
  virtual bool HasFloor (const McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that the call was released.
   * \param floorMachine The FSM.
   */
  virtual void ReceiveCallRelease (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Receives a floor deny message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorDeny (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgDeny& msg) const;
  /**
   * Receives a floor granted message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const;
  /**
   * Receives Floor Queue Position Request message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgQueuePositionRequest& msg) const;
  /**
   * Receives a floor queue position info message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionInfo (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgQueuePositionInfo& msg) const;
  /**
   * Receives a floor release message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRelease& msg) const;
  /**
   * Receives a floor request message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRequest& msg) const;
  /**
   * Receives a floor taken message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorTaken (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgTaken& msg) const;
  /**
   * Receives a media message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const;
  /**
   * Sends a floor queue position request message.
   * \param floorMachine The FSM.
   */
  virtual void SendFloorQueuePositionRequest (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine that media is ready to be sent.
   * \param floorMachine The FSM.
   * \param msg The media message to be sent.
   */
  virtual void MediaReady (McpttOffNetworkFloorParticipant& floorMachine, McpttMediaMsg& msg) const;
  /**
   * Releases a previously made request.
   * \param floorMachine The FSM.
   */
  virtual void ReleaseRequest (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the machine state that it has been selected.
   * \param floorMachine The FSM.
   */
  virtual void Selected (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Starts the floor machine state.
   * \param floorMachine The FSM.
   */
  virtual void Start (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Stops the floor machine state.
   * \param floorMachine The FSM.
   */
  virtual void Stop (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies this machine state that the button has been pushed.
   * \param floorMachine The FSM.
   */
  virtual void PttPush (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies this machine state that the button has been released.
   * \param floorMachine The FSM.
   */
  virtual void PttRelease (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the machine state that it has been unselected.
   * \param floorMachine The FSM.
   */
  virtual void Unselected (McpttOffNetworkFloorParticipant& floorMachine) const;

protected:
  /**
   * Creates an instance of the McpttOffNetworkFloorParticipantState class.
   */
  McpttOffNetworkFloorParticipantState (void);
};
/**
 * Overloaded output operator for the McpttOffNetworkFloorParticipantState class.
 * \param os The output stream to write to.
 * \param state The state to write to the output stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttOffNetworkFloorParticipantState& state);
/**
 * \ingroup mcptt
 * \brief A class that represents the 'Start-stop' state.
 */
class McpttOffNetworkFloorParticipantStateStartStop : public McpttOffNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOffNetworkFloorParticipantStateStartStop.
   * \returns An instance.
   */
  static Ptr<McpttOffNetworkFloorParticipantStateStartStop> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOffNetworkFloorParticipantStateStartStop class.
   */
  McpttOffNetworkFloorParticipantStateStartStop (void);
  /**
   * \brief The destructor of the McpttOffNetworkFloorParticipantStateStartStop class.
   */
  virtual ~McpttOffNetworkFloorParticipantStateStartStop (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Receives a floor granted message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const;
  /**
   * Receives a floor taken message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorTaken (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgTaken& msg) const;
  /**
   * Receives a media message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const;
  /**
   * Starts the floor machine state.
   * \param floorMachine The FSM.
   */
  virtual void Start (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies this machine state that the button has been pushed.
   * \param floorMachine The FSM.
   */
  virtual void PttPush (McpttOffNetworkFloorParticipant& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: has no permission' state.
 */
class McpttOffNetworkFloorParticipantStateNoPerm : public McpttOffNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOffNetworkFloorParticipantStateNoPerm.
   * \returns An instance.
   */
  static Ptr<McpttOffNetworkFloorParticipantStateNoPerm> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttOffNetworkFloorParticipantStateNoPerm class.
   */
  McpttOffNetworkFloorParticipantStateNoPerm (void);
  /**
   * \brief The destructor of the McpttOffNetworkFloorParticipantStateNoPerm class.
   */
  virtual ~McpttOffNetworkFloorParticipantStateNoPerm (void);
  /**
   * Notifies the floor machine state that timer T203 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT203 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Receives a floor granted message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const;
  /**
   * Receives a floor release message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRelease& msg) const;
  /**
   * Receives a media message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const;
  /**
   * Notifies the machine state that it has been selected.
   * \param floorMachine The FSM.
   */
  virtual void Selected (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies this machine state that the button has been pushed.
   * \param floorMachine The FSM.
   */
  virtual void PttPush (McpttOffNetworkFloorParticipant& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: has permission' state.
 */
class McpttOffNetworkFloorParticipantStateHasPerm : public McpttOffNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOffNetworkFloorParticipantStateHasPerm.
   * \returns An instance.
   */
  static Ptr<McpttOffNetworkFloorParticipantStateHasPerm> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOffNetworkFloorParticipantStateHasPerm class.
   */
  McpttOffNetworkFloorParticipantStateHasPerm (void);
  /**
   * \brief The destructor of the McpttOffNetworkFloorParticipantStateHasPerm class.
   */
  virtual ~McpttOffNetworkFloorParticipantStateHasPerm (void);
  /**
   * Notifies the floor machine state that timer T206 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT206 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T207 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT207 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates whether or not this machine state has the floor.
   * \param floorMachine The FSM.
   * \returns True, if this state has the floor.
   */
  virtual bool HasFloor (const McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Receives Floor Queue Position Request message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgQueuePositionRequest& msg) const;
  /**
   * Receives a floor release message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRelease& msg) const;
  /**
   * Receives a floor request message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRequest& msg) const;
  /**
   * Notifies the floor machine that media is ready to be sent.
   * \param floorMachine The FSM.
   * \param msg The received message
   */
  virtual void MediaReady (McpttOffNetworkFloorParticipant& floorMachine, McpttMediaMsg& msg) const;
  /**
   * Notifies the machine state that it has been selected.
   * \param floorMachine The FSM.
   */
  virtual void Selected (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies this machine state that the button has been released.
   * \param floorMachine The FSM.
   */
  virtual void PttRelease (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the machine state that it has been unselected.
   * \param floorMachine The FSM.
   */
  virtual void Unselected (McpttOffNetworkFloorParticipant& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: pending granted' state.
 */
class McpttOffNetworkFloorParticipantStatePendGrant : public McpttOffNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOffNetworkFloorParticipantStatePendGrant.
   * \returns An instance.
   */
  static Ptr<McpttOffNetworkFloorParticipantStatePendGrant> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOffNetworkFloorParticipantStatePendGrant class.
   */
  McpttOffNetworkFloorParticipantStatePendGrant (void);
  /**
   * \brief The destructor of the McpttOffNetworkFloorParticipantStatePendGrant class.
   */
  virtual ~McpttOffNetworkFloorParticipantStatePendGrant (void);
  /**
   * Notifies the floor machine state that timer T205 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT205 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T233 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT233 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates whether or not this machine state has the floor.
   * \param floorMachine The FSM.
   * \returns True, if this state has the floor.
   */
  virtual bool HasFloor (const McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Receives a floor release message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRelease& msg) const;
  /**
   * Receives a floor request message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRequest& msg) const;
  /**
   * Receives a media message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const;
  /**
   * Notifies the machine state that it has been unselected.
   * \param floorMachine The FSM.
   */
  virtual void Unselected (McpttOffNetworkFloorParticipant& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: pending request' state.
 */
class McpttOffNetworkFloorParticipantStatePendReq : public McpttOffNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOffNetworkFloorParticipantStatePendReq.
   * \returns An instance.
   */
  static Ptr<McpttOffNetworkFloorParticipantStatePendReq> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOffNetworkFloorParticipantStatePendReq class.
   */
  McpttOffNetworkFloorParticipantStatePendReq (void);
  /**
   * \brief The destructor of the McpttOffNetworkFloorParticipantStatePendReq class.
   */
  virtual ~McpttOffNetworkFloorParticipantStatePendReq (void);
  /**
   * Notifies the floor machine state that timer T201 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT201 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Receives a floor deny message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorDeny (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgDeny& msg) const;
  /**
   * Receives a floor granted message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const;
  /**
   * Receives a floor queue position info message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionInfo (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgQueuePositionInfo& msg) const;
  /**
   * Receives a floor request message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRequest& msg) const;
  /**
   * Receives a floor taken message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorTaken (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgTaken& msg) const;
  /**
   * Receives a media message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const;
  /**
   * Notifies this machine state that the button has been released.
   * \param floorMachine The FSM.
   */
  virtual void PttRelease (McpttOffNetworkFloorParticipant& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: queued' state.
 */
class McpttOffNetworkFloorParticipantStateQueued : public McpttOffNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOffNetworkFloorParticipantStateQueued.
   * \returns An instance.
   */
  static Ptr<McpttOffNetworkFloorParticipantStateQueued> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOffNetworkFloorParticipantStateQueued class.
   */
  McpttOffNetworkFloorParticipantStateQueued (void);
  /**
   * \brief The destructor of the McpttOffNetworkFloorParticipantStateQueued class.
   */
  virtual ~McpttOffNetworkFloorParticipantStateQueued (void);
  /**
   * Accepts a grant.
   * \param floorMachine The FSM.
   */
  virtual void AcceptGrant (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T203 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT203 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T204 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT204 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies the floor machine state that timer T233 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT233 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Receives a floor deny message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorDeny (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgDeny& msg) const;
  /**
   * Receives a floor granted message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const;
  /**
   * Receives a floor queue position info message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionInfo (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgQueuePositionInfo& msg) const;
  /**
   * Receives a media message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const;
  /**
   * Releases a previously made request.
   * \param floorMachine The FSM.
   */
  virtual void ReleaseRequest (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Sends a floor queue position request message.
   * \param floorMachine The FSM.
   */
  virtual void SendFloorQueuePositionRequest (McpttOffNetworkFloorParticipant& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: silence' state.
 */
class McpttOffNetworkFloorParticipantStateSilence : public McpttOffNetworkFloorParticipantState
{
public:
  /**
   * Gets an instance of the McpttOffNetworkFloorParticipantStateSilence.
   * \returns An instance.
   */
  static Ptr<McpttOffNetworkFloorParticipantStateSilence> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOffNetworkFloorParticipantStateSilence class.
   */
  McpttOffNetworkFloorParticipantStateSilence (void);
  /**
   * \brief The destructor of the McpttOffNetworkFloorParticipantStateSilence class.
   */
  virtual ~McpttOffNetworkFloorParticipantStateSilence (void);
  /**
   * Notifies the floor machine state that timer T230 has expired.
   * \param floorMachine The FSM.
   */
  virtual void ExpiryOfT230 (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Receives a floor granted message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorGranted (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgGranted& msg) const;
  /**
   * Receives a floor request message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   * \note
   */
  virtual void ReceiveFloorRequest (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgRequest& msg) const;
  /**
   * Receives a floor taken message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorTaken (McpttOffNetworkFloorParticipant& floorMachine, const McpttFloorMsgTaken& msg) const;
  /**
   * Receives a media message.
   * \param floorMachine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOffNetworkFloorParticipant& floorMachine, const McpttMediaMsg& msg) const;
  /**
   * Notifies the machine state that it has been selected.
   * \param floorMachine The FSM.
   */
  virtual void Selected (McpttOffNetworkFloorParticipant& floorMachine) const;
  /**
   * Notifies this machine state that the button has been pushed.
   * \param floorMachine The FSM.
   */
  virtual void PttPush (McpttOffNetworkFloorParticipant& floorMachine) const;
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_OFF_NETWORK_FLOOR_PARTICIPANT_STATE_H */

