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

#ifndef MCPTT_ON_NETWORK_FLOOR_DUAL_CONTROL_STATE_H
#define MCPTT_ON_NETWORK_FLOOR_DUAL_CONTROL_STATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-entity-id.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-queue.h"
#include "mcptt-media-msg.h"

namespace ns3 {

class McpttOnNetworkFloorDualControl;

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class represents a floor control state machine state as described in TS
 * 24.380 version 14.8.0.
 *
 * 'Start-stop'
 *  - The floor control server is in this state when a new instance of the
 *    'general floor control operation' state machine has been intitated, or
 *    when a call has been released.
 *
 * 'D: Floor Taken'
 *  - The floor controler server is in this state when it has permitted one of
 *    the MCPTT clients to override a current speaker.
 *
 * 'Releasing'
 *  - The floor control server is in this state while waiting for the
 *    application and signallying plane to finalize the disconnection of an
 *    MCPTT call.
 */
class McpttOnNetworkFloorDualControlState : public SimpleRefCount<McpttOnNetworkFloorDualControlState>
{
public:
  /**
   * \brief The destructor of the McpttOnNetworkFloorDualControlState class.
   */
  virtual ~McpttOnNetworkFloorDualControlState (void);
  /**
   * Indicates to the floor control server that the call has been released (part I).
   * \param machine The FSM.
   */
  virtual void CallRelease1 (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Indicates to the floor control server that the call has been released (part II).
   * \param machine The FSM.
   */
  virtual void CallRelease2 (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Indicates to the floor control server that the client has disconnected.
   * \param machine The FSM.
   */
  virtual void ClientRelease (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Enters the given state.
   */
  virtual void Enter (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Notifies the floor machine state that timer T11 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT11 (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Notifies the floor machine state that timer T12 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT12 (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Receives a floor release message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRelease& msg) const;
  /**
   * Receives a floor request message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRequest& msg) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOnNetworkFloorDualControl& machine, const McpttMediaMsg& msg) const;
  /**
   * Receives a preemptive floor request message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceivePreemptiveFloorRequest (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRequest& msg) const;
  /**
   * Notifies the machine state that it has been selected.
   * \param machine The FSM.
   */
  virtual void Selected (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Indicates to the floor control server to terminate.
   * \param machine The FSM.
   */
  virtual void Terminate (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Notifies the machine state that it has been unselected.
   * \param machine The FSM.
   */
  virtual void Unselected (McpttOnNetworkFloorDualControl& machine) const;

protected:
  /**
   * Creates an instance of the McpttOnNetworkFloorDualControlState class.
   */
  McpttOnNetworkFloorDualControlState (void);
};
/**
 * Overloaded output operator for the McpttOnNetworkFloorDualControlState class.
 * \param os The output stream to write to.
 * \param state The state to write to the output stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttOnNetworkFloorDualControlState& state);
/**
 * \ingroup mcptt
 * \brief A class that represents the 'Start-stop' state.
 */
class McpttOnNetworkFloorDualControlStateStartStop : public McpttOnNetworkFloorDualControlState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorDualControlStateStartStop.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorDualControlStateStartStop> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorDualControlStateStartStop class.
   */
  McpttOnNetworkFloorDualControlStateStartStop (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorDualControlStateStartStop class.
   */
  virtual ~McpttOnNetworkFloorDualControlStateStartStop (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Receives a preemptive floor request message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceivePreemptiveFloorRequest (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRequest& msg) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'G: Floor Taken' state.
 */
class McpttOnNetworkFloorDualControlStateTaken : public McpttOnNetworkFloorDualControlState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorDualControlStateTaken.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorDualControlStateTaken> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorDualControlStateTaken class.
   */
  McpttOnNetworkFloorDualControlStateTaken (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorDualControlStateTaken class.
   */
  virtual ~McpttOnNetworkFloorDualControlStateTaken (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Notifies the machine state that it has been selected.
   * \param machine The FSM.
   */
  virtual void Enter (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Notifies the floor machine state that timer T11 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT11 (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Notifies the floor machine state that timer T12 has expired.
   * \param machine The FSM.
   */
  virtual void ExpiryOfT12 (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Receives a media message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveMedia (McpttOnNetworkFloorDualControl& machine, const McpttMediaMsg& msg) const;
  /**
   * Receives a floor release message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRelease& msg) const;
  /**
   * Receives a floor request message.
   * \param machine The FSM.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (McpttOnNetworkFloorDualControl& machine, const McpttFloorMsgRequest& msg) const;
  /**
   * Indicates to the floor control server that the client has disconnected.
   * \param machine The FSM.
   */
  virtual void ClientRelease (McpttOnNetworkFloorDualControl& machine) const;
  /**
   * Indicates to the floor control server to terminate.
   * \param machine The FSM.
   */
  virtual void Terminate (McpttOnNetworkFloorDualControl& machine) const;
};
/**
 * \ingroup mcptt
 * \brief A class that represents the 'Releasing' state.
 */
class McpttOnNetworkFloorDualControlStateReleasing : public McpttOnNetworkFloorDualControlState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkFloorDualControlStateReleasing.
   * \returns An instance.
   */
  static Ptr<McpttOnNetworkFloorDualControlStateReleasing> GetInstance (void);
  /**
   * \brief Gets the ID of the state.
   * \returns The ID of the state.
   */
  static McpttEntityId GetStateId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorDualControlStateReleasing class.
   */
  McpttOnNetworkFloorDualControlStateReleasing (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorDualControlStateReleasing class.
   */
  virtual ~McpttOnNetworkFloorDualControlStateReleasing (void);
  /**
   * Gets the ID of the machine state.
   * \returns The ID of the state.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Notifies the machine state that the call has been released (part II).
   * \param machine The FSM.
   */
  virtual void CallRelease2 (McpttOnNetworkFloorDualControl& machine) const;
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_ON_NETWORK_FLOOR_DUAL_CONTROL_STATE_H */

