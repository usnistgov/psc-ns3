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

#ifndef MCPTT_ON_NETWORK_FLOOR_TOWARDS_PARTICIPANT_H
#define MCPTT_ON_NETWORK_FLOOR_TOWARDS_PARTICIPANT_H

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/type-id.h>

#include "mcptt-channel.h"
#include "mcptt-counter.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-msg-sink.h"
#include "mcptt-media-msg.h"
#include "mcptt-timer.h"

namespace ns3 {

namespace psc {

class McpttServerCall;
class McpttOnNetworkFloorTowardsParticipantState;
class McpttOnNetworkFloorArbitrator;

/**
 * \ingroup mcptt
 *
 * This class represents the MCPTT server state machine for basic floor control
 * operation towards the floor participant, as described in TS 24.380 v14.8.0.
 */
class McpttOnNetworkFloorTowardsParticipant : public Object,
                                              public McpttFloorMsgSink
{
public:
  /**
   * \brief Gets the ID of the McpttOnNetworkFloorTowardsParticipant type.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Creates an instance of the McpttOnNetworkFloorTowardsParticipant class.
   */
  McpttOnNetworkFloorTowardsParticipant (void);
  /**
   * \brief The destructor of the McpttOnNetworkFloorTowardsParticipant class.
   */
  virtual ~McpttOnNetworkFloorTowardsParticipant (void);
  /**
   * Notifies the floor towards participant machine that the call has been initiated.
   * \param implicitRequest whether the floor is taken by an implicit request
   */
  virtual void CallInitialized (bool implicitRequest);
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
  virtual void ChangeState (Ptr<McpttOnNetworkFloorTowardsParticipantState>  state);
  /**
   * Fowards a message to the participant.
   * \param msg The message to forward.
   */
  virtual void DoSend (McpttMsg& msg);
  /**
   * Gets the type ID of this McpttOnNetworkFloorTowardsParticipant instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the ID of the state.
   * \returns The state ID.
   */
  virtual McpttEntityId GetStateId (void) const;
  /**
   * Indicates whether or not the associated participant is listening to two sources.
   * \returns True, if the associated participant is listening to two sources; otherwise, false.
   */
  virtual bool IsDualFloor (void) const;
  /**
   * Indicates if the SDP offer included the "mc_implicit_request" fmtp attribute.
   * \returns True, if the attribute was included; otherwise, false.
   */
  virtual bool IsImplicitRequest (void) const;
  /**
   * Indicates if the floor arbitrator supports queueing of floor control requests.
   * Whether the SDP offer included the "mc_queueing" fmtp attribute is not considered
   * by the current model; it is assumed to be supported by participants.
   * \returns True, if the queueing is supported; otherwise, false.
   */
  virtual bool IsQueueing (void) const;
  /**
   * Indicates whether or not the associated floor participant is the originator.
   * \returns True, if the associated participant is the originator.
   */
  virtual bool IsOriginator (void) const;
  /**
   * Indicates if the associated participant is overridden without revoke.
   * \returns True, if the associated participant is overridden without revoke; othwerwise, false.
   */
  virtual bool IsOverridden (void) const;
  /**
   * Indicates if the associated participant is overriding without revoke.
   * \returns True, if the associated participant is overriding without revoke; othwerwise, false.
   */
  virtual bool IsOverriding (void) const;
  /**
   * Indicates if the associated participant is "receive only".
   * \returns True, if the associated participant is "receive only".
   */
  virtual bool IsReceiveOnly (void) const;
  /**
   * Indicates whether or not the floor machine has been started.
   * \returns True, if the floor machine has been started.
   */
  virtual bool IsStarted (void) const;
  /**
   * \brief Receives a message.
   * \param msg The message that was received.
   */
  virtual void Receive (const McpttFloorMsg& msg);
  /**
   * \brief Receives a message.
   * \param msg The message that was received.
   */
  virtual void Receive (const McpttMediaMsg& msg);
  /**
   * Receives Floor Queue Position Request message.
   * \param msg The received message.
   */
  virtual void ReceiveFloorQueuePositionRequest (const McpttFloorMsgQueuePositionRequest& msg);
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
   * Sends an MCPTT message.
   * \param msg The message to send.
   */
  virtual void Send (McpttMsg& msg);
  /**
   * Indicates to the floor control server to terminate.
   */
  virtual void Terminate (void);
  /**
   * Sets the delay for timer T8.
   * \param delayT8 The delay to use.
   */
  virtual void SetDelayT8 (const Time& delayT8);
  /**
   * Starts the FSM.
   */
  virtual void Start (void);
  /**
   * Stops the FSM.
   */
  virtual void Stop (void);

protected:
  /**
   * \brief Disposes of the McpttFloorMachine.
   */
  virtual void DoDispose (void);
  /**
   * Notifies the floor machine that timer T8 has expired.
   */
  virtual void ExpiryOfT8 (void);
  /**
   * Handles the received floor control packet.
   * \param pkt The packet that was received.
   * \param from The source address of the packet.
   */
  virtual void ReceiveFloorPkt (Ptr<Packet>  pkt, Address from);
  /**
   * Handles the received media packet.
   * \param pkt The packet that was received.
   * \param from The source address of the packet.
   */
  virtual void ReceiveMediaPkt (Ptr<Packet>  pkt, Address from);
  /**
   * Sends a floor deny message.
   * \param msg The message to send.
   */
  virtual void SendFloorDeny (McpttFloorMsgDeny& msg);
  /**
   * Sends a floor granted message.
   * \param msg The message to send.
   */
  virtual void SendFloorGranted (McpttFloorMsgGranted& msg);
  /**
   * Sends a floor idle message.
   * \param msg The message to send.
   */
  virtual void SendFloorIdle (McpttFloorMsgIdle& msg);
  /**
   * Sends a floor revoke message.
   * \param msg The message to send.
   */
  virtual void SendFloorRevoke (McpttFloorMsgRevoke& msg);
  /**
   * Sends a floor taken message.
   * \param msg The message to send.
   */
  virtual void SendFloorTaken (McpttFloorMsgTaken& msg);
  /**
   * Sends an RTP media message.
   * \param msg The message to send.
   */
  virtual void SendMedia (McpttMediaMsg& msg);
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
  bool m_dualFloor; //!< The flag that indicates if the associated participant is listening to two sources.
  Ptr<McpttChannel> m_floorChannel; //!< The channel to use for floor control messages.
  uint16_t m_floorPort; //!< The port to use for the floor control channel.
  bool m_implicitRequest; //!< The flag that indicates if SDP offer contains the "mc_implicit_request" fmtp.
  Ptr<McpttChannel> m_mediaChannel; //!< The channel to use for media messages.
  uint16_t m_mediaPort; //!< The port to use for media messages.
  bool m_originator; //!< Flag that indicates if the associated floor participant is the originator.
  bool m_overridden; //!< Flag that indicates if associated participant is overridden without revoke.
  bool m_overriding; //!< Flag that indicates if associated participant is overriding without revoke.
  Ptr<McpttOnNetworkFloorArbitrator> m_owner; //!< The floor arbitration server.
  Address m_peerAddress; //!< The address of the node that the peer application is on.
  uint32_t m_peerUserId; //!< The MCPTT user ID of the node that the peer application is on.
  bool m_receiveOnly; //!< Flag that indicates if the associated participant is "receive only".
  McpttFloorMsgRevoke m_revokeMsg; //!< The Floor Revoke message to retransmit when T8 expires.
  Ptr<McpttOnNetworkFloorTowardsParticipantState> m_state; //!< The state of the floor machine.
  Callback<void, const McpttEntityId&, const McpttEntityId&> m_stateChangeCb; //!< The state change callback.
  TracedCallback<uint32_t, uint16_t, const std::string&, const std::string&, const std::string&, const std::string&> m_stateChangeTrace; //!< The state change traced callback.
  uint32_t m_storedSsrc; //!< The SSRC of the floor participant with permission to transmit media.
  uint8_t m_storedPriority; //!< The stored priority the floor machine.
  McpttFloorMsgFieldTrackInfo m_trackInfo; //!< The track info field.
  Ptr<McpttTimer> m_t8; //!< The timer T8.

public:
  /**
   * Gets the channel to use for floor control messages.
   * \returns The channel.
   */
  virtual Ptr<McpttChannel> GetFloorChannel (void) const;
  /**
   * Gets the port to use for the floor control channel.
   * \returns The port number.
   */
  virtual uint16_t GetFloorPort (void) const;
  /**
   * Gets the channel to use for floor control messages.
   * \returns The channel.
   */
  virtual Ptr<McpttChannel> GetMediaChannel (void) const;
  /**
   * Gets the port to use for the media channel.
   * \returns The port number.
   */
  virtual uint16_t GetMediaPort (void) const;
  /**
   * Gets the owner of the state machine.
   * \returns The owner.
   */
  virtual Ptr<McpttOnNetworkFloorArbitrator> GetOwner (void) const;
  /**
   * Gets the peer address.
   * \returns The peer address.
   */
  virtual Address GetPeerAddress (void) const;
  /**
   * Gets the peer user ID
   * \returns The peer user ID
   */
  virtual uint32_t GetPeerUserId (void) const;
  /**
   * Gets the floor revoke message to retransmit.
   * \returns The revoke message.
   */
  virtual McpttFloorMsgRevoke GetRevokeMsg (void) const;
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
   * Gets the timer T8.
   * \returns The timer.
   */
  virtual Ptr<McpttTimer> GetT8 (void) const;
  /**
   * Sets the flag that indicates if the associated participant is listening to two sources.
   * \param dualFloor true if listening to two sources
   */
  virtual void SetDualFloor (const bool dualFloor);
  /**
   * Sets the channel to use for floor control messages.
   * \param floorChannel The channel.
   */
  virtual void SetFloorChannel (const Ptr<McpttChannel> floorChannel);
  /**
   * Sets the port to use for the floor control channel.
   * \param floorPort The port number.
   */
  virtual void SetFloorPort (const uint16_t floorPort);
  /**
   * Sets the channel to use for media messages.
   * \param mediaChannel The channel.
   */
  virtual void SetMediaChannel (const Ptr<McpttChannel> mediaChannel);
  /**
   * Sets the port to use for the media channel.
   * \param mediaPort The port number.
   */
  virtual void SetMediaPort (const uint16_t mediaPort);
  /**
   * Sets the flag that indicates if the associated floor participant is the originator.
   * \param originator The flag.
   */
  virtual void SetOriginator (const bool originator);
  /**
   * Sets the flag that indicates if the assoicated participant is overridden without revoke.
   * \param overridden The flag.
   */
  virtual void SetOverridden (const bool overridden);
  /**
   * Sets the flag that indicates if the assoicated participant is overriding without revoke.
   * \param overriding The flag.
   */
  virtual void SetOverriding (const bool overriding);
  /**
   * Sets the owner of the floor machine.
   * \param owner The owner.
   */
  virtual void SetOwner (Ptr<McpttOnNetworkFloorArbitrator> owner);
  /**
   * Sets the address of the participant.
   * \param peerAddress The address of the participant.
   */
  virtual void SetPeerAddress (const Address& peerAddress);
  /**
   * Sets the user ID of the peer.
   * \param userId The user ID of the peer.
   */
  virtual void SetPeerUserId (uint32_t userId);
  /**
   * Sets the floor revoke message to retransmit when the T8 expires.
   * \param revokeMsg The message to retransmit.
   */
  virtual void SetRevokeMsg (const McpttFloorMsgRevoke& revokeMsg);
  /**
   * Sets the state of the floor machine.
   * \param state The state.
   */
  virtual void SetState (Ptr<McpttOnNetworkFloorTowardsParticipantState>  state);
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

#endif /* MCPTT_ON_NETWORK_FLOOR_TOWARDS_PARTICIPANT_H */

