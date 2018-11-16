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

#ifndef MCPTT_SOCKET_H
#define MCPTT_SOCKET_H


#include <ns3/ipv4-address.h>
#include <ns3/ptr.h>
#include <ns3/socket.h>
#include <ns3/type-id.h>

#include "mcptt-msg.h"

namespace ns3 {

/**
 * \ingroup mcptt
 *
 * This class is a wrapper for the ns3::Socket class to provide and API to
 * simplify the use of a socket based on actions requested in 3GPP standards
 * TS 24.379 an TS 24.380. This is also includes opening and clossing sockets
 * on the fly throughout the simulation.
 */
class McpttSocket : public Object
{
public:
 /**
  * Gets the type ID of the McpttSocket.
  * \returns The type ID.
  */
 static TypeId GetTypeId (void);
 /**
  * Creates an instance of the McpttSocket class.
  */
 McpttSocket (void);
 /**
  * The destructor of the McpttSocket class.
  */
 virtual ~McpttSocket (void);
 /**
  * Closes the socket.
  */
 virtual void Close (void);
 /**
  * Gets the type ID of this McpttSocket instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
 /**
  * Indicates if this wrapper is for the given socket.
  * \param socket The given socket.
  * \returns True, if this wrapper is for the given socket.
  */
 virtual bool IsFor (Ptr<Socket>  socket) const;
 /**
  * Indicates if this socket is open.
  * \returns True, if this socket is open.
  */
 virtual bool IsOpen (void) const;
 /**
  * Opens the socket.
  * \param owner The McpttApp instance that the socket is for.
  * \param socketFacTid The type ID of the socket factory to use for creation.
  * \param port The port to open the socket on.
  * \param local The local IP address of the application.
  * \param peer The remote IP address of the application.
  * \returns The error number (or zero on success).
  */
 virtual int Open (McpttApp* const& owner, const TypeId& socketFacTid, uint16_t port, const Ipv4Address& local, const Ipv4Address& peer);
 /**
  * Sends a packet.
  * \param pkt The packet to send.
  * \returns The error number (or zero on success).
  */
 virtual int Send (Ptr<Packet> pkt);
protected:
 /**
  * Disposes of the McpttSocket.
  */
 virtual void DoDispose (void);
private:
 bool m_open; //!< Flag used to indicate if the socket is open.
 Ptr<Socket> m_subject; //!< The socket being wrapped.
protected:
 /**
  * Gets the flag used to indicate if the socket is open.
  * \returns The flag.
  */
 virtual bool GetOpen (void) const;
 /**
  * Gets socket being wrapped.
  * \returns The socket being wrapped.
  */
 virtual Ptr<Socket> GetSubject (void) const;
 /**
  * Sets the flag used to indicate if the socket is open.
  * \param open The flag.
  */
 virtual void SetOpen (const bool& open);
 /**
  * Sets the socket being wrapped.
  * \param subject The socket being wrapped.
  */
 virtual void SetSubject (Ptr<Socket>  subject);
};

} // ns3 namespace

#endif /* MCPTT_SOCKET_H */

