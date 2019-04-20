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

#include "mcptt-call-machine.h"
#include "mcptt-call-msg.h"
#include "mcptt-chan.h"
#include "mcptt-floor-machine.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"

namespace ns3 {

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
  * Creates an instance of the McpttCall class.
  */
  McpttCall (void);
 /**
  * The destructor of the McpttCall class.
  */
 virtual ~McpttCall (void);
 /**
  * Closes the floor channel.
  */
 void CloseFloorChan (void);
 /**
  * Closes the media channel.
  */
 void CloseMediaChan (void);
 /**
  * Gets the ID of the call.
  * \returns The call ID.
  */
 uint16_t GetCallId (void) const;
 /**
  * Indicates if the floor channel is open.
  * \returns True, if the channel is open.
  */
 bool IsFloorChanOpen (void) const;
 /**
  * Indicates if the media channel is open.
  * \returns True, if the channel is open.
  */
 bool IsMediaChanOpen (void) const;
 /**
  * Opens the floor channel.
  * \param peerAddr The peer address.
  * \param port The peer port.
  */
 void OpenFloorChan (const Ipv4Address& peerAddr, const uint16_t port);
 /**
  * Opens the media channel.
  * \param peerAddr The peer address.
  * \param port The peer port.
  */
 void OpenMediaChan (const Ipv4Address& peerAddr, const uint16_t port);
 /**
  * Receives a call message.
  * \param msg The message that was received.
  */
 void Receive (const McpttCallMsg& msg);
 /**
  * Receives a floor message.
  * \param msg The message that was received.
  */
 void Receive (const McpttFloorMsg& msg);
 /**
  * Receive a media message.
  * \param msg The message that was received.
  */
 void Receive (const McpttMediaMsg& msg);
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
protected:
 /**
  * Disposes of the McpttCall instance.
  */
 void DoDispose (void);
 /**
  * Handles the receieved floor control packet.
  * \param pkt The packet that was received.
  */
 void ReceiveFloorPkt (Ptr<Packet>  pkt);
 /**
  * Handles the received media packet.
  * \param pkt The packet that was received.
  */
 void ReceiveMediaPkt (Ptr<Packet>  pkt);
 private:
 Ptr<McpttCallMachine> m_callMachine; //!< The call control state machine.
 Ptr<McpttChan> m_floorChan; //!< The channel to use for floor control messages.
 Ptr<McpttFloorMachine> m_floorMachine; //!< The floor state machine.
 Ptr<McpttChan> m_mediaChan; //!< The channel to use for media messages.
 McpttPttApp* m_owner; //!< The owner of this call.
 Callback<void, const McpttCall&, const McpttMsg&> m_rxCb; //!< The received message callback.
 Callback<void, const McpttCall&, const McpttMsg&> m_txCb; //!< The transmitted message callback.
public:
 /**
  * Gets the call control state machine.
  * \returns The call machine.
  */
 Ptr<McpttCallMachine> GetCallMachine (void) const;
 /**
  * Gets the channel to use for floor control messages.
  * \returns The channel.
  */
 Ptr<McpttChan> GetFloorChan (void) const;
 /**
  * Gets the floor machine.
  * \returns The floor machine.
  */
 Ptr<McpttFloorMachine> GetFloorMachine (void) const;
 /**
  * Gets the channel to use for floor control messages.
  * \returns The channel.
  */
 Ptr<McpttChan> GetMediaChan (void) const;
 /**
  * Gets the owner of this call.
  * \returns The owner.
  */
 McpttPttApp* GetOwner (void) const;
 /**
  * Sets the call control state machine.
  * \param callMachine The call control state machine.
  */
 void SetCallMachine (Ptr<McpttCallMachine>  callMachine);
 /**
  * Sets the channel to use for floor control messages.
  * \param floorChan The channel.
  */
 void SetFloorChan (Ptr<McpttChan>  floorChan);
 /**
  * Sets the floor machine.
  * \param floorMachine The floor machine.
  */
 void SetFloorMachine (Ptr<McpttFloorMachine>  floorMachine);
 /**
  * Sets the channel to use for media messages.
  * \param mediaChan The channel.
  */
 void SetMediaChan (Ptr<McpttChan>  mediaChan);
 /**
  * Sets the owner of this call.
  * \param owner The owner.
  */
 void SetOwner (McpttPttApp* const& owner);
 /**
  * Sets the received message callback.
  * \param rxCb The callback.
  */
 void SetRxCb (const Callback<void, const McpttCall&, const McpttMsg&>  rxCb);
 /**
  * Sets the transmitted message callback.
  * \param txCb The callback.
  */
 void SetTxCb (const Callback<void, const McpttCall&, const McpttMsg&>  txCb);
};

} // namespace ns3

#endif /* MCPTT_CALL_H */

