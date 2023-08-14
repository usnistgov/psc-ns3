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

#ifndef MCPTT_ON_NETWORK_FLOOR_DUAL_CONTROL_H
#define MCPTT_ON_NETWORK_FLOOR_DUAL_CONTROL_H

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>

#include "mcptt-counter.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-msg-sink.h"
#include "mcptt-media-msg.h"
#include "mcptt-timer.h"

namespace ns3 {

namespace psc {

class McpttCall;
class McpttOnNetworkFloorDualControlState;
class McpttOnNetworkFloorArbitrator;

/**
 * \ingroup mcptt
 *
 * This class represents the MCPTT dual floor control state machine
 * described in TS 24.380 v14.8.0.  As specified there, dual floor control
 * is used when a floor participant requests floor with a pre-emptive
 * floor priority allowing the floor control to continue without
 * revoking the floor from the MCPTT client permitted to send media.
 *
 * This class declaration is for an optional object that can be added
 * to class McpttOnNetworkFloorArbitrator if dual floor control is desired.
 */
class McpttOnNetworkFloorDualControl : public Object,
                                       public McpttFloorMsgSink
{
public:
  /**
   * \brief Gets the ID of the McpttOnNetworkFloorDualControl type.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorDualControl class.
   */
  McpttOnNetworkFloorDualControl (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorDualControl class.
   */
  virtual ~McpttOnNetworkFloorDualControl (void);
  /**
   * Notifies the floor machine that the call has been released (part I).
   */
  virtual void CallRelease1 (void);
  /**
   * Notifies the floor machine that the call has been released (part II).
   */
  virtual void CallRelease2 (void);
  /**
   * Notifies the floor machine that the client has been released.
   */
  virtual void ClientRelease (void);
  /**
   * Changes the state of the floor machine.
   * \param state The state to change to.
   */
  virtual void ChangeState (Ptr<McpttOnNetworkFloorDualControlState>  state);
  /**
   * Gets the type ID of this McpttOnNetworkFloorDualControl instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the ID of the state.
   * \returns The state ID.
   */
  virtual McpttEntityId GetStateId (void) const;
  /**
   * Indicates whether or not the floor machine has been started.
   * \returns True, if the floor machine has been started.
   */
  virtual bool IsStarted (void) const;
  /**
   * Receives a floor release message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRelease (const McpttFloorMsgRelease& msg);
  /**
   * Receives a floor request message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorRequest (const McpttFloorMsgRequest& msg);
  /**
  * Receives an RTP media message.
  * \param msg The received message.
  */
  virtual void ReceiveMedia (const McpttMediaMsg& msg);
  /**
   * Receives a preemptive floor request message.
   * \param msg The received message.
   */
  virtual void ReceivePreemptiveFloorRequest (const McpttFloorMsgRequest& msg);
  /**
   * Sets the delay for timer T11.
   * \param delayT11 The delay to use.
   */
  virtual void SetDelayT11 (const Time& delayT11);
  /**
  * Sets the delay for timer T12.
  * \param delayT12 The delay to use.
  */
  virtual void SetDelayT12 (const Time& delayT12);
  /**
   * Stops the FSM.
   */
  virtual void Stop (void);
  /**
   * Indicates to the floor control server to terminate.
   */
  virtual void Terminate (void);

protected:
  /**
   * \brief Disposes of the McpttLfloorMachine.
   */
  virtual void DoDispose (void);
  /**
   * Notifies the floor machine that timer T11 has expired.
   */
  virtual void ExpiryOfT11 (void);
  /**
   * Notifies the floor machine that timer T12 has expired.
   */
  virtual void ExpiryOfT12 (void);
  /**
   * TracedCallback signature for state change traces
   * \param [in] userId User ID
   * \param [in] callId Call ID
   * \param [in] selected Indicator whether the call is selected by the client
   * \param [in] typeId Type ID of the class reporting the change
   * \param [in] oldStateName Name of the old state
   * \param [in] newStateName Name of the new state
   */
  typedef void (* StateChangeTracedCallback) (uint32_t userId, uint16_t callId, const std::string& selected, const std::string& typeId, const std::string& oldStateName, const std::string& newStateName);

private:
  Ptr<McpttOnNetworkFloorArbitrator> m_owner; //!< The floor abitration server.
  Ptr<McpttOnNetworkFloorDualControlState> m_state; //!< The state of the floor machine.
  Callback<void, const McpttEntityId&, const McpttEntityId&> m_stateChangeCb; //!< The state change callback.
  TracedCallback<uint32_t, uint16_t, const std::string&, const std::string&, const std::string&, const std::string&> m_stateChangeTrace; //!< The state change traced callback.
  uint32_t m_storedSsrc; //!< The SSRC of the floor participant with permission to transmit media.
  uint8_t m_storedPriority; //!< The stored priority the floor machine.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.
  Ptr<McpttTimer> m_t11; //!< The timer T11.
  Ptr<McpttTimer> m_t12; //!< The timer T12.

public:
  /**
   * Gets the owner of the state machine.
   * \returns The owner.
   */
  virtual Ptr<McpttOnNetworkFloorArbitrator> GetOwner (void) const;
  /**
   * Gets the SSRC of the participant who currently has permission to send media.
   * \returns The SSRC.
   */
  virtual uint32_t GetStoredSsrc (void) const;
  /**
  * Gets the stored of the state machine.
  * \returns The stored priority.
  */
  virtual uint8_t GetStoredPriority (void) const;
  /**
   * Gets the track info field.
   * \returns The track info field.
   */
  virtual McpttFloorMsgFieldTrackInfo GetTrackInfo (void) const;
  /**
   * Gets the timer T11.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetT11 (void) const;
  /**
   * Gets the timer T12.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetT12 (void) const;
  /**
   * Sets the owner of the floor machine.
   * \param owner The owner.
   */
  virtual void SetOwner (Ptr<McpttOnNetworkFloorArbitrator> owner);
  /**
   * Sets the state of the floor machine.
   * \param state The state.
   */
  virtual void SetState (Ptr<McpttOnNetworkFloorDualControlState>  state);
  /**
   * Sets the state change call back.
   * \param stateChangeCb The setup delay callback.
   */
  virtual void SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb);
  /**
   * Sets the SSRC of the participant that has permission to send media.
   * \param storedSsrc The SSRC.
   */
  virtual void SetStoredSsrc (const uint32_t storedSsrc);
  /**
   * Sets the stored priority of the floor.
   * \param storedPriority The stored priority.
   */
  virtual void SetStoredPriority (uint8_t storedPriority);
  /**
  * Sets the track info field.
  * \param trackInfo The track info field.
  */
  virtual void SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_ON_NETWORK_FLOOR_DUAL_CONTROL_H */

