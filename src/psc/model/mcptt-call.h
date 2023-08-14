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

#ifndef MCPTT_CALL_H
#define MCPTT_CALL_H

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/sip-header.h>
#include <ns3/sip-agent.h>

#include "mcptt-call-machine.h"
#include "mcptt-call-msg.h"
#include "mcptt-channel.h"
#include "mcptt-floor-participant.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"

namespace ns3 {

namespace psc {

class McpttPttApp;

/**
 * \ingroup mcptt
 *
 * This class used to represent an MCPTT call, which is just a logical channel
 * between MCPTT applications. Associated with a call is a call ID, to identify
 * the particular call, a call machine to perform call control, a floor
 * machine to do floor control, a floor control message channel, and a media
 * message channel. An MCPTT call should be subordinate of an MCPTT application.
 */
class McpttCall : public Object
{
public:
  /**
   * Gets the type ID of the McpttCall class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);

  /**
   * Types of MCPTT calls
   */
  enum NetworkCallType
  {
    ON_NETWORK,
    OFF_NETWORK,
    INVALID
  };
  /**
   * Creates an instance of the McpttCall class.
   */
  McpttCall ();
  /**
   * Creates an instance of the McpttCall class.
   * \param callType type of network call
   */
  McpttCall (NetworkCallType callType);
  /**
   * The destructor of the McpttCall class.
   */
  virtual ~McpttCall (void);
  /**
   * Closes the floor channel.
   */
  void CloseFloorChannel (void);
  /**
   * Closes the media channel.
   */
  void CloseMediaChannel (void);
  /**
   * Sets the ID of the call.
   * \param callId The call ID.
   */
  void SetCallId (uint16_t callId);
  /**
   * Gets the ID of the call.
   * \returns The call ID.
   */
  uint16_t GetCallId (void) const;
  /**
   * Set the type of network for the McpttCall
   * \param callType type of network call
   */
  void SetNetworkCallType (NetworkCallType callType);
  /**
   * Get the type of network for the McpttCall
   * \return the type of network call
   */
  NetworkCallType GetNetworkCallType (void) const;
  /**
   * Sets the push on select flag.
   * \param pushOnSelect true if call should start pusher upon select.
   */
  void SetPushOnSelect (bool pushOnSelect);
  /**
   * Gets the push on select flag.
   * \returns true if call should start pusher upon select.
   */
  bool GetPushOnSelect (void) const;
  /**
   * Indicates if the floor channel is open.
   * \returns True, if the channel is open.
   */
  bool IsFloorChannelOpen (void) const;
  /**
   * Indicates if the media channel is open.
   * \returns True, if the channel is open.
   */
  bool IsMediaChannelOpen (void) const;
  /**
   * Opens the floor channel.
   * \param peerAddr The peer address.
   * \param port The peer port.
   */
  void OpenFloorChannel (const Address& peerAddr, const uint16_t port);
  /**
   * Opens the media channel.
   * \param peerAddr The peer address.
   * \param port The peer port.
   */
  void OpenMediaChannel (const Address& peerAddr, const uint16_t port);
  /**
   * Receive an on-network SIP call message.
   * \param pkt The packet (without SIP header)
   * \param hdr A reference to the SIP header that has been deserialized
   * \param state The state of the transaction
   */
  virtual void ReceiveSipMessage (Ptr<Packet> pkt, const sip::SipHeader& hdr, sip::SipAgent::TransactionState state);
  /**
   * Receive notification of a SIP event
   * \param event The event description
   * \param state The state of the transaction
   */
  virtual void ReceiveSipEvent (const char* event, sip::SipAgent::TransactionState state);
  /**
   * Receive an on-network call message.
   * \param pkt The packet (without SIP header)
   * \param hdr A reference to the SIP header that has been deserialized
   */
  virtual void Receive (Ptr<Packet> pkt, const sip::SipHeader& hdr);
  /**
   * Receives a call message.
   * \param msg The message that was received.
   */
  virtual void Receive (const McpttCallMsg& msg);
  /**
   * Receives a floor message.
   * \param msg The message that was received.
   */
  virtual void Receive (const McpttFloorMsg& msg);
  /**
   * Receive a media message.
   * \param msg The message that was received.
   */
  virtual void Receive (const McpttMediaMsg& msg);
  /**
   * Return an InetSocketAddress or Inet6SocketAddress for the peer
   * \return the peer address
   */
  Address GetPeerSocketAddress (void) const;
  /**
   * Return the peer user ID (applicable mainly to on-network calls for which
   * it can be used as a SIP URI according to the ns-3 SIP model).
   */
  uint32_t GetPeerUserId (void) const;
  /**
   * Sends a SIP call control message.
   * \param pkt The packet (already serialized with SIP header)
   * \param addr The peer address to send to
   * \param hdr A reference to the SIP header that has been serialized
   */
  void SendSipMessage (Ptr<Packet> pkt, const Address& addr, const sip::SipHeader& hdr);
  /**
   * Sends a call message.
   * \param msg The message to send.
   */
  void Send (const McpttCallMsg& msg);
  /**
   * Sends a floor message.
   * \param msg The message to send.
   */
  void Send (const McpttFloorMsg& msg);
  /**
   * Sends a media message.
   * \param msg The media message.
   */
  void Send (const McpttMediaMsg& msg);
  /**
   * Starts the call.
   */
  void Start (void);
  /**
   * Stops the call.
   */
  void Stop (void);

protected:
  /**
   * Disposes of the McpttCall instance.
   */
  void DoDispose (void);
  /**
   * Handles the received floor control packet.
   * \param pkt The packet that was received.
   * \param from The source address of the packet
   */
  void ReceiveFloorPkt (Ptr<Packet>  pkt, Address from);
  /**
   * Handles the received media packet.
   * \param pkt The packet that was received.
   * \param from The source address of the packet
   */
  void ReceiveMediaPkt (Ptr<Packet>  pkt, Address from);

private:
  NetworkCallType m_networkCallType; //!< The network call type
  Ptr<McpttCallMachine> m_callMachine; //!< The call control state machine.
  Ptr<McpttChannel> m_callChannel; //!< The channel for call control messages.
  uint16_t m_defaultOnNetworkCallPort; //!< The default port for on-network call channels
  uint16_t m_defaultOffNetworkCallPort; //!< The default port for off-network call channels
  uint16_t m_callPort; //!< The port on which call control messages will flow.
  Ptr<McpttChannel> m_floorChannel; //!< The channel to use for floor control messages.
  Ptr<McpttFloorParticipant> m_floorMachine; //!< The floor state machine.
  Ptr<McpttChannel> m_mediaChannel; //!< The channel to use for media messages.
  Ptr<McpttPttApp> m_owner; //!< The owner of this call.
  Address m_peerAddress; //!< The address of the node that the peer application is on.
  uint32_t m_peerUserId; //!< The URI corresponding to the peer (on-network operation).
  bool m_pushOnSelect; //!< Whether to start pusher upon call select
  Time m_startTime; //!< The call start time.
  Time m_stopTime; //!< The call stop time.

public:
  /**
   * Gets the channel used for call control messages.
   * \returns The channel.
   */
  virtual Ptr<McpttChannel> GetCallChannel (void) const;
  /**
   * Gets the call control state machine.
   * \returns The call machine.
   */
  Ptr<McpttCallMachine> GetCallMachine (void) const;
  /**
   * Gets the channel to use for floor control messages.
   * \returns The channel.
   */
  Ptr<McpttChannel> GetFloorChannel (void) const;
  /**
   * Gets the floor machine.
   * \returns The floor machine.
   */
  Ptr<McpttFloorParticipant> GetFloorMachine (void) const;
  /**
   * Gets the channel to use for floor control messages.
   * \returns The channel.
   */
  Ptr<McpttChannel> GetMediaChannel (void) const;
  /**
   * Gets the owner of this call.
   * \returns The owner.
   */
  Ptr<McpttPttApp> GetOwner (void) const;
  /**
   * Gets the call start time
   * \return The call start time.
   */
  Time GetStartTime (void) const;
  /**
   * Gets the call stop time
   * \return The call stop time.
   */
  Time GetStopTime (void) const;
  /**
   * Sets the call control state machine.
   * \param callMachine The call control state machine.
   */
  void SetCallMachine (Ptr<McpttCallMachine>  callMachine);
  /**
   * Sets the channel to use for floor control messages.
   * \param floorChannel The channel.
   */
  void SetFloorChannel (Ptr<McpttChannel>  floorChannel);
  /**
   * Sets the floor machine.
   * \param floorMachine The floor machine.
   */
  void SetFloorMachine (Ptr<McpttFloorParticipant>  floorMachine);
  /**
   * Sets the channel to use for media messages.
   * \param mediaChannel The channel.
   */
  void SetMediaChannel (Ptr<McpttChannel>  mediaChannel);
  /**
   * Sets the owner of this call.
   * \param owner The owner.
   */
  void SetOwner (Ptr<McpttPttApp> owner);
  /**
   * Sets the call start time
   * \param startTime The start time.
   */
  void SetStartTime (Time startTime);
  /**
   * Sets the call stop time
   * \param stopTime The stop time.
   */
  void SetStopTime (Time stopTime);
};

/**
  * \brief Stream output operator
  * \param os output stream
  * \param obj NetworkCallType object
  * \return updated stream
  */
std::ostream& operator<< (std::ostream& os, const McpttCall::NetworkCallType& obj);

} // namespace psc
} // namespace ns3

#endif /* MCPTT_CALL_H */

