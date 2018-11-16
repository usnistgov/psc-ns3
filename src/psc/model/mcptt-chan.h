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

#ifndef MCPTT_CHAN_H
#define MCPTT_CHAN_H

#include <ns3/ipv4-address.h>
#include <ns3/node.h>
#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/socket.h>
#include <ns3/type-id.h>

namespace ns3 {

/**
 * \ingroup mcptt
 *
 * This class is used to represent a channel for the MCPTT application. This is
 * simply a wrapper around a socket so that this channel can be opened and
 * closed throughout the simulation.
 */
class McpttChan : public Object
{
public:
 /**
  * Gets the type ID of the McpttChan class.
  * \returns The type ID.
  */
 static TypeId GetTypeId (void);
 /**
  * Creates an instance of the McpttChan class.
  */
 McpttChan(void);
 /**
  * \brief The destructor of the McpttChan class.
  */
 virtual ~McpttChan (void);
 /**
  * Closes the channel.
  */
 virtual void Close (void);
 /**
  * Gets the type ID of this McpttChan instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
 /**
  * Indicates if the channel is open.
  * \returns True, if the channel is open.
  */
 virtual bool IsOpen (void) const;
 /**
  * Opens the channel.
  * \param node The node in which the socket will reside.
  * \param port The port number.
  * \param local The local address.
  * \param peer The peer address.
  * \returns 0 on success; otherwise the error number.
  */
 virtual int Open (Ptr<Node>  node, uint16_t port, const Ipv4Address& local, const Ipv4Address& peer);
 /**
  * Sends a packet.
  * \param pkt The packet to send.
  * \returns The number of bytes accepted for transmission if no error occurs, and -1 otherwise.
  */
 virtual int Send (Ptr<Packet>  pkt);
protected:
 /**
  * Disposes of this McpttChan instance.
  */
 virtual void DoDispose (void);
 /**
  * The function used to receive a single packet.
  * \param pkt The packet that was received.
  */
 virtual void ReceivePkt (Ptr<Packet>  pkt);
 /**
  * The sink for when packets are received.
  * \param socket The socket from which to receive packets.
  */
 virtual void ReceivePkts (Ptr<Socket> socket);
private:
 Callback<void, Ptr<Packet> > m_rxPktCb; //!< The sink used for receiving received packets.
 Ptr<Socket> m_socket; //!< The underlying socket of the channel.
protected:
 /**
  * Gets the underlying socket of the channel.
  * \returns The underlying socket.
  */
 virtual Ptr<Socket> GetSocket (void) const;
 /**
  * Sets the underlying socket of the channel.
  * \param socket The underlying socket.
  */
 virtual void SetSocket (Ptr<Socket>  socket);
public:
 /**
  * Sets the sink used for receiving received packets.
  * \param rxPktCb The callback.
  */
 virtual void SetRxPktCb (const Callback<void, Ptr<Packet> >  rxPktCb);
};

} // namespace ns3

#endif /* MCPTT_CHAN_H */

