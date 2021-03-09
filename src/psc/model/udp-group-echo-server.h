/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Modified by: NIST // Contributions may not be subject to US copyright.
 *
 * This application, authored by Fernando J. Cintron <fernando.cintron@nist.gov>
 * is derived from udp-echo-server.h originally authored by University of Washington.
 */

#ifndef UDP_GROUP_ECHO_SERVER_H
#define UDP_GROUP_ECHO_SERVER_H

#include "ns3/application.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/nstime.h"
#include "ns3/traced-callback.h"
#include <map>
#include <string>

namespace ns3 {

class Socket;
class Packet;

namespace psc {

/**
 * Structure to store information about the client
 */
struct UdpGroupEchoClient
{
  Address m_address; //!< The remote address of the client
  Address m_echo_address; //!< The address where to send a response
  Time m_timestamp; //!< Last time the server heard from the client
};

/**
 * \ingroup psc
 * \brief A Udp Group Echo server
 *
 * Every packet received is sent back to active group members.
 */
class UdpGroupEchoServer : public Application
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Mode of echo operation
   */
  typedef enum
  {
    INF_SESSION,       /**<  Group echo with no session expiration time */
    NO_GROUP_SESSION,  /**<  Server echoes single client only           */
    TIMEOUT_LIMITED    /**<  Server forwards to group within timeout    */
  } Mode_t;

  UdpGroupEchoServer ();
  virtual ~UdpGroupEchoServer ();
  /**
   * Adds a new client to the list of clients to echo messages
   * \param client The new client address to add
   */
  virtual void AddClient (const Address& client);

protected:
  virtual void DoDispose (void);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  /**
   * \brief Handle a packet reception.
   *
   * This function is called by lower layers.
   *
   * \param socket the socket the packet was received to.
   */
  void HandleRead (Ptr<Socket> socket);
  /**
   * \brief Log client information.
   *
   * This function logs the number of clients and their session information.
   * To log the output, at least NS_LOG_INFO must be enabled for this class.
   *
   */
  void LogClients (void);

  Mode_t m_mode; ///< Mode of echo operation
  uint16_t m_port; ///< Port on which we listen for incoming packets.
  uint16_t m_port_client; ///< Port on which we echo packets to client.
  Ptr<Socket> m_socket; ///< IPv4 Socket
  Ptr<Socket> m_socket6; ///< IPv6 Socket
  Address m_local; ///< local multicast address
  std::map<std::string, UdpGroupEchoClient> m_clients; ///< Group of clients
  Time m_timeout; ///< Inactive client session expiration time
  bool m_echoClient; ///< Set server to echo back to the client.
  TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace; ///< Callbacks for tracing the packet Rx events

};

} // namespace psc
} // namespace ns3

#endif /* UDP_GROUP_ECHO_SERVER_H */

