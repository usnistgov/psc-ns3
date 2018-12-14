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

#ifndef MCPTT_FLOOR_MACHINE_BASIC_STATE_H
#define MCPTT_FLOOR_MACHINE_BASIC_STATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-entity-id.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-queue.h"
#include "mcptt-media-msg.h"

namespace ns3 {

class McpttFloorMachineBasic;

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
class McpttFloorMachineBasicState : public SimpleRefCount<McpttFloorMachineBasicState>
{
public:
 /**
  * \brief The destructor of the McpttFloorMachineBasicState class.
  */
 virtual ~McpttFloorMachineBasicState (void);
 /**
  * Accepts a grant.
  * \param floorMachine The FSM.
  */
 virtual void AcceptGrant (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T201 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT201 (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T203 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT203 (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T204 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT204 (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T205 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT205 (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T206 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT206 (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T207 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT207 (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T230 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT230 (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T233 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT233 (McpttFloorMachineBasic& floorMachine) const;
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
 virtual bool HasFloor (const McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that the call was released.
  * \param floorMachine The FSM.
  */
 virtual void ReceiveCallRelease (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Receives a floor deny message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorDeny (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgDeny& msg) const;
 /**
  * Receives a floor granted message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const;
 /**
  * Receives Floor Queue Position Request message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorQueuePositionReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgQueuePosReq& msg) const;
 /**
  * Receives a floor queue position info message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorQueuePositionInfo (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgQueueInfo& msg) const;
 /**
  * Receives a floor release message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorRelease (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRelease& msg) const;
 /**
  * Receives a floor request message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRequest& msg) const;
 /**
  * Receives a floor taken message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorTaken (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgTaken& msg) const;
 /**
  * Receives a media message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const;
 /**
  * Sends a floor queue position request message.
  * \param floorMachine The FSM.
  */
 virtual void SendFloorQueuePosReq (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine that media is ready to be sent.
  * \param floorMachine The FSM.
  * \param msg The media message to be sent.
  */
 virtual void MediaReady (McpttFloorMachineBasic& floorMachine, McpttMediaMsg& msg) const;
 /**
  * Releases a previously made request.
  * \param floorMachine The FSM.
  */
 virtual void ReleaseRequest (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the machine state that it has been selected.
  * \param floorMachine The FSM.
  */
 virtual void Selected (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Starts the floor machine state.
  * \param floorMachine The FSM.
  */
 virtual void Start (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Stops the floor machine state.
  * \param floorMachine The FSM.
  */
 virtual void Stop (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies this machine state that the button has been pushed.
  * \param floorMachine The FSM.
  */
 virtual void TakePushNotification (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies this machine state that the button has been released.
  * \param floorMachine The FSM.
  */
 virtual void TakeReleaseNotification (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the machine state that it has been unselected.
  * \param floorMachine The FSM.
  */
 virtual void Unselected (McpttFloorMachineBasic& floorMachine) const;
protected:
 /**
  * Creates an instance of the McpttFloorMachineBasicState class.
  */
 McpttFloorMachineBasicState (void);
};
/**
 * Overloaded output operator for the ns3::McpttFloorMachineBasicState class.
 * \param os The output stream to write to.
 * \param state The state to write to the output stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const ns3::McpttFloorMachineBasicState& state);
/**
 * \ingroup mcptt
 * \brief A class that represents the 'Start-stop' state.
 */
class McpttFloorMachineBasicStateStartStop : public McpttFloorMachineBasicState
{
public:
 /**
  * Gets an instance of the McpttFloorMachineBasicStateStartStop.
  * \returns An instance.
  */
 static Ptr<McpttFloorMachineBasicStateStartStop> GetInstance (void);
 /**
  * \brief Gets the ID of the state.
  * \returns The ID of the state.
  */
 static McpttEntityId GetStateId (void);
 /**
  * \brief Creates an instance of the McpttFloorMachineBasicStateStartStop class.
  */
 McpttFloorMachineBasicStateStartStop (void);
 /**
  * \brief The destructor of the McpttFloorMachineBasicStateStartStop class.
  */
 virtual ~McpttFloorMachineBasicStateStartStop (void);
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
 virtual void ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const;
 /**
  * Receives a floor taken message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorTaken (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgTaken& msg) const;
 /**
  * Receives a media message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const;
 /**
  * Starts the floor machine state.
  * \param floorMachine The FSM.
  */
 virtual void Start (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies this machine state that the button has been pushed.
  * \param floorMachine The FSM.
  */
 virtual void TakePushNotification (McpttFloorMachineBasic& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: has no permission' state.
 */
class McpttFloorMachineBasicStateNoPerm : public McpttFloorMachineBasicState
{
public:
 /**
  * Gets an instance of the McpttFloorMachineBasicStateNoPerm.
  * \returns An instance.
  */
 static Ptr<McpttFloorMachineBasicStateNoPerm> GetInstance (void);
 /**
  * \brief Gets the ID of the state.
  * \returns The ID of the state.
  */
 static McpttEntityId GetStateId (void);
 /**
  * Creates an instance of the McpttFloorMachineBasicStateNoPerm class.
  */
 McpttFloorMachineBasicStateNoPerm (void);
 /**
  * \brief The destructor of the McpttFloorMachineBasicStateNoPerm class.
  */
 virtual ~McpttFloorMachineBasicStateNoPerm (void);
 /**
  * Notifies the floor machine state that timer T203 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT203 (McpttFloorMachineBasic& floorMachine) const;
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
 virtual void ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const;
 /**
  * Receives a floor release message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorRelease (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRelease& msg) const;
 /**
  * Receives a media message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const;
 /**
  * Notifies the machine state that it has been selected.
  * \param floorMachine The FSM.
  */
 virtual void Selected (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies this machine state that the button has been pushed.
  * \param floorMachine The FSM.
  */
 virtual void TakePushNotification (McpttFloorMachineBasic& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: has permission' state.
 */
class McpttFloorMachineBasicStateHasPerm : public McpttFloorMachineBasicState
{
public:
 /**
  * Gets an instance of the McpttFloorMachineBasicStateHasPerm.
  * \returns An instance.
  */
 static Ptr<McpttFloorMachineBasicStateHasPerm> GetInstance (void);
 /**
  * \brief Gets the ID of the state.
  * \returns The ID of the state.
  */
 static McpttEntityId GetStateId (void);
 /**
  * \brief Creates an instance of the McpttFloorMachineBasicStateHasPerm class.
  */
 McpttFloorMachineBasicStateHasPerm (void);
 /**
  * \brief The destructor of the McpttFloorMachineBasicStateHasPerm class.
  */
 virtual ~McpttFloorMachineBasicStateHasPerm (void);
 /**
  * Notifies the floor machine state that timer T206 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT206 (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T207 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT207 (McpttFloorMachineBasic& floorMachine) const;
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
 virtual bool HasFloor (const McpttFloorMachineBasic& floorMachine) const;
 /**
  * Receives Floor Queue Position Request message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorQueuePositionReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgQueuePosReq& msg) const;
 /**
  * Receives a floor release message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorRelease (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRelease& msg) const;
 /**
  * Receives a floor request message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRequest& msg) const;
 /**
  * Notifies the floor machine that media is ready to be sent.
  * \param floorMachine The FSM.
  * \param msg The received message
  */
 virtual void MediaReady (McpttFloorMachineBasic& floorMachine, McpttMediaMsg& msg) const;
 /**
  * Notifies the machine state that it has been selected.
  * \param floorMachine The FSM.
  */
 virtual void Selected (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies this machine state that the button has been released.
  * \param floorMachine The FSM.
  */
 virtual void TakeReleaseNotification (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the machine state that it has been unselected.
  * \param floorMachine The FSM.
  */
 virtual void Unselected (McpttFloorMachineBasic& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: pending granted' state.
 */
class McpttFloorMachineBasicStatePendGrant : public McpttFloorMachineBasicState
{
public:
 /**
  * Gets an instance of the McpttFloorMachineBasicStatePendGrant.
  * \returns An instance.
  */
 static Ptr<McpttFloorMachineBasicStatePendGrant> GetInstance (void);
 /**
  * \brief Gets the ID of the state.
  * \returns The ID of the state.
  */
 static McpttEntityId GetStateId (void);
 /**
  * \brief Creates an instance of the McpttFloorMachineBasicStatePendGrant class.
  */
 McpttFloorMachineBasicStatePendGrant (void);
 /**
  * \brief The destructor of the McpttFloorMachineBasicStatePendGrant class.
  */
 virtual ~McpttFloorMachineBasicStatePendGrant (void);
 /**
  * Notifies the floor machine state that timer T205 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT205 (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T233 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT233 (McpttFloorMachineBasic& floorMachine) const;
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
 virtual bool HasFloor (const McpttFloorMachineBasic& floorMachine) const;
 /**
  * Receives a floor release message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorRelease (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRelease& msg) const;
 /**
  * Receives a floor request message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRequest& msg) const;
 /**
  * Receives a media message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const;
 /**
  * Notifies the machine state that it has been unselected.
  * \param floorMachine The FSM.
  */
 virtual void Unselected (McpttFloorMachineBasic& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: pending request' state.
 */
class McpttFloorMachineBasicStatePendReq : public McpttFloorMachineBasicState
{
public:
 /**
  * Gets an instance of the McpttFloorMachineBasicStatePendReq.
  * \returns An instance.
  */
 static Ptr<McpttFloorMachineBasicStatePendReq> GetInstance (void);
 /**
  * \brief Gets the ID of the state.
  * \returns The ID of the state.
  */
 static McpttEntityId GetStateId (void);
 /**
  * \brief Creates an instance of the McpttFloorMachineBasicStatePendReq class.
  */
 McpttFloorMachineBasicStatePendReq (void);
 /**
  * \brief The destructor of the McpttFloorMachineBasicStatePendReq class.
  */
 virtual ~McpttFloorMachineBasicStatePendReq (void);
 /**
  * Notifies the floor machine state that timer T201 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT201 (McpttFloorMachineBasic& floorMachine) const;
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
 virtual void ReceiveFloorDeny (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgDeny& msg) const;
 /**
  * Receives a floor granted message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const;
 /**
  * Receives a floor queue position info message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorQueuePositionInfo (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgQueueInfo& msg) const;
 /**
  * Receives a floor request message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRequest& msg) const;
 /**
  * Receives a floor taken message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorTaken (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgTaken& msg) const;
 /**
  * Receives a media message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const;
 /**
  * Notifies this machine state that the button has been released.
  * \param floorMachine The FSM.
  */
 virtual void TakeReleaseNotification (McpttFloorMachineBasic& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: queued' state.
 */
class McpttFloorMachineBasicStateQueued : public McpttFloorMachineBasicState
{
public:
 /**
  * Gets an instance of the McpttFloorMachineBasicStateQueued.
  * \returns An instance.
  */
 static Ptr<McpttFloorMachineBasicStateQueued> GetInstance (void);
 /**
  * \brief Gets the ID of the state.
  * \returns The ID of the state.
  */
 static McpttEntityId GetStateId (void);
 /**
  * \brief Creates an instance of the McpttFloorMachineBasicStateQueued class.
  */
 McpttFloorMachineBasicStateQueued (void);
 /**
  * \brief The destructor of the McpttFloorMachineBasicStateQueued class.
  */
 virtual ~McpttFloorMachineBasicStateQueued (void);
 /**
  * Accepts a grant.
  * \param floorMachine The FSM.
  */
 virtual void AcceptGrant (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T203 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT203 (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T204 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT204 (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies the floor machine state that timer T233 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT233 (McpttFloorMachineBasic& floorMachine) const;
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
 virtual void ReceiveFloorDeny (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgDeny& msg) const;
 /**
  * Receives a floor granted message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const;
 /**
  * Receives a floor queue position info message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorQueuePositionInfo (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgQueueInfo& msg) const;
 /**
  * Receives a media message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const;
 /**
  * Releases a previously made request.
  * \param floorMachine The FSM.
  */
 virtual void ReleaseRequest (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Sends a floor queue position request message.
  * \param floorMachine The FSM.
  */
 virtual void SendFloorQueuePosReq (McpttFloorMachineBasic& floorMachine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'O: silence' state.
 */
class McpttFloorMachineBasicStateSilence : public McpttFloorMachineBasicState
{
public:
 /**
  * Gets an instance of the McpttFloorMachineBasicStateSilence.
  * \returns An instance.
  */
 static Ptr<McpttFloorMachineBasicStateSilence> GetInstance (void);
 /**
  * \brief Gets the ID of the state.
  * \returns The ID of the state.
  */
 static McpttEntityId GetStateId (void);
 /**
  * \brief Creates an instance of the McpttFloorMachineBasicStateSilence class.
  */
 McpttFloorMachineBasicStateSilence (void);
 /**
  * \brief The destructor of the McpttFloorMachineBasicStateSilence class.
  */
 virtual ~McpttFloorMachineBasicStateSilence (void);
 /**
  * Notifies the floor machine state that timer T230 has expired.
  * \param floorMachine The FSM.
  */
 virtual void ExpiryOfT230 (McpttFloorMachineBasic& floorMachine) const;
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
 virtual void ReceiveFloorGranted (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgGranted& msg) const;
 /**
  * Receives a floor request message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  * \note
  */
 virtual void ReceiveFloorReq (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgRequest& msg) const;
 /**
  * Receives a floor taken message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveFloorTaken (McpttFloorMachineBasic& floorMachine, const McpttFloorMsgTaken& msg) const;
 /**
  * Receives a media message.
  * \param floorMachine The FSM.
  * \param msg The received message.
  */
 virtual void ReceiveMedia (McpttFloorMachineBasic& floorMachine, const McpttMediaMsg& msg) const;
 /**
  * Notifies the machine state that it has been selected.
  * \param floorMachine The FSM.
  */
 virtual void Selected (McpttFloorMachineBasic& floorMachine) const;
 /**
  * Notifies this machine state that the button has been pushed.
  * \param floorMachine The FSM.
  */
 virtual void TakePushNotification (McpttFloorMachineBasic& floorMachine) const;
};

} // namespace ns3

#endif /* MCPTT_FLOOR_MACHINE_BASIC_STATE_H */

