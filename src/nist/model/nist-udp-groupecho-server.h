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
 * Modified by: NIST
 * Author: Fernando J. Cintron <fernando.cintron@nist.gov>
 */

#ifndef NIST_UDP_GROUPECHO_SERVER_H
#define NIST_UDP_GROUPECHO_SERVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/timer.h"
#include "ns3/nstime.h"
#include <map>
#include <string>
#include "ns3/traced-callback.h"
 
#define INF_SESSION -1
#define NO_GROUP_SESSION 0
 
namespace ns3 {

class Socket;
class Packet;

  /**
   * Structure to store information about the client
   */
struct client
{
  Address addrs; //!< The remote address of the client 
  Address echo_addrs; //!< The address where to send a response
  Time tstamp; //!< Last time the server heard from the client 
 };

/**
 * \ingroup applications 
 * \defgroup groupudpecho GroupUdpEcho
 */

/**
 * \ingroup groupudpecho
 * \brief A Udp Group Echo server
 *
 * Every packet received is sent back to active group members.
 */
class NistUdpGroupEchoServer : public Application 
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  NistUdpGroupEchoServer ();
  virtual ~NistUdpGroupEchoServer ();
  /**
   * Adds a new client to the list of clients to echo messages
   * \param client The new client to add
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
  void PrintClients (void);

  uint16_t m_port; //!< Port on which we listen for incoming packets.
  uint16_t m_port_client; //!< Port on which we echo packets to client.
  Ptr<Socket> m_socket; //!< IPv4 Socket
  Ptr<Socket> m_socket6; //!< IPv6 Socket
  Address m_local; //!< local multicast address
  std::map<std::string,client> m_clients; //!< Group of clients
  double  m_timeout; //!< Inactive client session expiration time <seconds>.
  bool m_echoback; //!< Set server to echo back the client.
  /// Callbacks for tracing the packet Rx events
  TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;
  
};

} // namespace ns3

#endif /* NIST_UDP_GROUPECHO_SERVER_H */

