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

#ifndef MCPTT_ON_NETWORK_FLOOR_ARBITRATOR_H
#define MCPTT_ON_NETWORK_FLOOR_ARBITRATOR_H

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>

#include "mcptt-server-call.h"
#include "mcptt-counter.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-msg-sink.h"
#include "mcptt-media-msg.h"
#include "mcptt-on-network-floor-dual-control.h"
#include "mcptt-timer.h"

namespace ns3 {

namespace psc {

class McpttOnNetworkFloorArbitratorState;
class McpttOnNetworkFloorTowardsParticipant;
class McpttServerCall;

/**
 * \ingroup mcptt
 *
 * This class represents the general floor control operation state mahine
 * described in TS 24.380 v14.8.0.
 */
class McpttOnNetworkFloorArbitrator : public Object
{
public:
  /**
   * \brief Gets the ID of the McpttOnNetworkFloorArbitrator type.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorArbitrator class.
   */
  McpttOnNetworkFloorArbitrator (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorArbitrator class.
   */
  virtual ~McpttOnNetworkFloorArbitrator (void);
  /**
   * Adds a floor participant interface.
   * \param participant The interface for the associated participant.
   */
  virtual void AddParticipant (Ptr<McpttOnNetworkFloorTowardsParticipant> participant);
  /**
   * Notifies this machine that a call has been initialized.
   * \param participant The participant that initiated the call
   * \param implicitRequest whether the request includes an implicit floor request
   */
  virtual void CallInitialized (Ptr<McpttOnNetworkFloorTowardsParticipant> participant, bool implicitRequest);
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
  virtual void ChangeState (Ptr<McpttOnNetworkFloorArbitratorState>  state);
  /**
   * Gets the floor indicator to use when sending a message.
   * \returns The floor indicator field.
   */
  virtual McpttFloorMsgFieldIndic GetIndicator (void) const;
  /**
   * Gets the type ID of this McpttOnNetworkFloorArbitrator instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the number of floor participants.
   * \returns The number of floor participants.
   */
  virtual uint32_t GetNParticipants (void) const;
  /**
   * Gets the pointer for the indexed participant
   * \param index index of participant
   * \return The pointer of the participant
   */
  virtual Ptr<McpttOnNetworkFloorTowardsParticipant> GetParticipant (uint32_t index) const;
  /**
   * Gets the pointer for the associated participant with the given userId.
   * \param userId The userId of the associated participant.
   * \return The pointer of the participant, or 0 if the participant is not found.
   */
  virtual Ptr<McpttOnNetworkFloorTowardsParticipant> GetParticipantByUserId (uint32_t userId) const;
  /**
   * Gets the pointer for the associated participant with the given SSRC.
   * \param ssrc The SSRC of the associated participant.
   * \return The pointer, or 0 if the participant is not found.
   */
  virtual Ptr<McpttOnNetworkFloorTowardsParticipant> GetParticipantBySsrc (const uint32_t ssrc) const;
  /**
   * Gets the pointer for the originating participant
   * \return The pointer of the participant, or 0 if the participant is not found.
   */
  virtual Ptr<McpttOnNetworkFloorTowardsParticipant> GetOriginatingParticipant (void) const;
  /**
   * Gets the ID of the state.
   * \returns The state ID.
   */
  virtual McpttEntityId GetStateId (void) const;
  /**
   * Indicates if the audio cut-in is supported for the group.
   * \returns True, if audio cut-in is supported, otherwise false.
   */
  virtual bool IsAudioCutIn (void) const;
  /**
   * Indicates if acknowledgement is required.
   * \returns True, if acknowledgement is required; otherwise, false.
   */
  virtual bool IsAckRequired (void) const;
  /**
   * Indicates whether or not dual floor has been indicated.
   * \returns True, if the dual floor has been indicated; otherwise, false.
   */
  virtual bool IsDualFloor (void) const;
  /**
   * Indicates whether or not dual floor is supported.
   * \returns True, if dual floor is supported; otherwise, false.
   */
  virtual bool IsDualFloorSupported (void) const;
  /**
   * Indicates whether or not a client is currently permitted to send media.
   * \returns True, if a client is permitted to send media; otherwise, false.
   */
  virtual bool IsFloorOccupied (void) const;
  /**
   * Indicates whether or not queueing of floor requests is supported.
   * \returns True, if it has been negotiated.
   */
  virtual bool IsQueueingSupported (void) const;
  /**
   * Indicates if the given floor request is a preemptive floor request.
   * \returns True, if the given floor request is preemptive.
   */
  virtual bool IsPreemptive (const McpttFloorMsgRequest& msg) const;
  /**
   * Indicates whether or not the floor machine has been started.
   * \returns True, if the floor machine has been started.
   */
  virtual bool IsStarted (void) const;
  /**
   * Gets the next sequence number.
   * \returns The next sequence number.
   */
  virtual uint16_t NextSeqNum (void);
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
   * \brief Sends a message to the specified user.
   * \param msg The message to send.
   * \param ssrc The ID of the specified user.
   */
  virtual void SendTo (McpttMsg& msg, const uint32_t ssrc);
  /**
   * \brief Sends a message to all users.
   * \param msg The message to send.
   */
  virtual void SendToAll (McpttMsg& msg);
  /**
   * \brief Sends a message to all users except the specified user.
   * \param msg The message to send.
   * \param ssrc The ID of the excpted user.
   */
  virtual void SendToAllExcept (McpttMsg& msg, const uint32_t ssrc);
  /**
   * Sets the delay for timer T1.
   * \param delayT1 The delay to use.
   */
  virtual void SetDelayT1 (const Time& delayT1);
  /**
   * Sets the delay for timer T2.
   * \param delayT2 The delay to use.
   */
  virtual void SetDelayT2 (const Time& delayT2);
  /**
   * Sets the delay for timer T3.
   * \param delayT3 The delay to use.
   */
  virtual void SetDelayT3 (const Time& delayT3);
  /**
  * Sets the delay for timer T4.
  * \param delayT4 The delay to use.
  */
  virtual void SetDelayT4 (const Time& delayT4);
  /**
  * Sets the delay for timer T7.
  * \param delayT7 The delay to use.
  */
  virtual void SetDelayT7 (const Time& delayT7);
  /**
  * Sets the delay for timer T20.
  * \param delayT20 The delay to use.
  */
  virtual void SetDelayT20 (const Time& delayT20);
  /**
   * Sets the limit for counter C7.
   * \param limitC7 The limit to use.
   */
  virtual void SetLimitC7 (uint32_t limitC7);
  /**
   * Sets the limit for counter C20.
   * \param limitC20 The limit to use.
   */
  virtual void SetLimitC20 (uint32_t limitC20);
  /**
   * Starts the state machine.
   */
  virtual void Start (void);
  /**
   * Stops the state machine.
   */
  virtual void Stop (void);
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

protected:
  /**
   * \brief Disposes of the McpttLfloorMachine.
   */
  virtual void DoDispose (void);
  /**
   * Notifies the floor machine that timer T1 has expired.
   */
  virtual void ExpiryOfT1 (void);
  /**
   * Notifies the floor machine that timer T2 has expired.
   */
  virtual void ExpiryOfT2 (void);
  /**
   * Notifies the floor machine that timer T3 has expired.
   */
  virtual void ExpiryOfT3 (void);
  /**
   * Notifies the floor machine that timer T4 has expired.
   */
  virtual void ExpiryOfT4 (void);
  /**
   * Notifies the floor machine that timer T7 has expired.
   */
  virtual void ExpiryOfT7 (void);
  /**
   * Notifies the floor machine that timer T20 has expired.
   */
  virtual void ExpiryOfT20 (void);

private:
  bool m_ackRequired; //!< A flag that indicates if acknowledgement is required.
  bool m_audioCutIn; //!< The flag that indicates if audio cut-in is configured for the group.
  Ptr<McpttCounter> m_c7; //!< The counter associated with T7.
  Ptr<McpttCounter> m_c20; //!< The counter associated with T20.
  bool m_dualFloorSupported; //!< A flag that to indicate dual floor indication.
  Ptr<McpttOnNetworkFloorDualControl> m_dualControl; //!< The dual floor control state machine.
  Ptr<McpttServerCall> m_owner; //!< The call object that owns this floor machine.
  bool m_queueingSupported; //!<< The flag that indicates if queueing of floor control requests is supported.
  std::vector<Ptr<McpttOnNetworkFloorTowardsParticipant> > m_participants; //!< The associated floor participants.
  Ptr<McpttFloorQueue> m_queue; //!< The queue of floor requests.
  uint16_t m_rejectCause; //!< The reject cause to include when revoking the floor.
  uint16_t m_seqNum; //!< The sequence number.
  Ptr<McpttOnNetworkFloorArbitratorState> m_state; //!< The state of the floor machine.
  Callback<void, const McpttEntityId&, const McpttEntityId&> m_stateChangeCb; //!< The state change callback.
  TracedCallback<uint32_t, uint16_t, const std::string&, const std::string&, const std::string&, const std::string&> m_stateChangeTrace; //!< The state change traced callback.
  uint32_t m_storedSsrc; //!< The SSRC of the floor participant with permission to transmit media.
  uint8_t m_storedPriority; //!< The stored priority the floor machine.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.
  uint32_t m_txSsrc; //!< The SSRC to use when transmitting a message.
  Ptr<McpttTimer> m_t1; //!< The timer T1.
  Ptr<McpttTimer> m_t2; //!< The timer T2.
  Ptr<McpttTimer> m_t3; //!< The timer T3.
  Ptr<McpttTimer> m_t4; //!< The timer T4.
  Ptr<McpttTimer> m_t7; //!< The timer T7.
  Ptr<McpttTimer> m_t20; //!< The timer T20.

public:
  /**
   * Gets the counter C7
   * \returns The counter.
   */
  virtual Ptr<McpttCounter> GetC7 (void) const;
  /**
   * Gets the counter C20
   * \returns The counter.
   */
  virtual Ptr<McpttCounter> GetC20 (void) const;
  /**
   * Gets the dual floor control state machine.
   * \returns The dual floor control state machine.
   */
  virtual Ptr<McpttOnNetworkFloorDualControl> GetDualControl (void) const;
  /**
   * Gets the owner of the state machine.
   * \returns The owner.
   */
  virtual Ptr<McpttServerCall> GetOwner (void) const;
  /**
   * Gets the queue.
   * \returns The queue.
   */
  virtual Ptr<McpttFloorQueue> GetQueue (void) const;
  /**
   * Gets the reject cause to include in the revoke message when entering the 'G: pending Floor Revoke' state.
   * \returns The reject cause.
   */
  virtual uint16_t GetRejectCause (void) const;
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
   * Gets the SSRC to use when transmitting a message.
   * \returns The SSRC.
   */
  virtual uint32_t GetTxSsrc (void) const;
  /**
   * Gets the timer T1.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetT1 (void) const;
  /**
   * Gets the timer T2.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetT2 (void) const;
  /**
   * Gets the timer T3.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetT3 (void) const;
  /**
   * Gets the timer T4.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetT4 (void) const;
  /**
   * Gets the timer T7.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetT7 (void) const;
  /**
   * Gets the timer T20.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetT20 (void) const;
  /**
   * Sets the dual floor control state machine.
   * \param dualControl The state machine.
   */
  virtual void SetDualControl (const Ptr<McpttOnNetworkFloorDualControl> dualControl);
  /**
   * Sets the owner of the floor machine.
   * \param owner The owner.
   */
  virtual void SetOwner (Ptr<McpttServerCall> owner);
  /**
   * Sets the reject cause to include in the revoke message when entering the 'G: pending Floor Revoke' state.
   * \param rejectCause The reject cause.
   */
  virtual void SetRejectCause (const uint16_t rejectCause);
  /**
   * Sets the state of the floor machine.
   * \param state The state.
   */
  virtual void SetState (Ptr<McpttOnNetworkFloorArbitratorState>  state);
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

#endif /* MCPTT_ON_NETWORK_FLOOR_ARBITRATOR_H */

