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
 * is derived from udp-echo-server.cc originally authored by University of Washington.
 */

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include <iomanip>
#include "ns3/boolean.h"
#include "ns3/enum.h"

#include "udp-group-echo-server.h"
#include <sstream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UdpGroupEchoServerApplication");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (UdpGroupEchoServer);

// For pretty-printing of mode in logging statements below
std::string
ModeToString (UdpGroupEchoServer::Mode_t mode)
{
  if (mode == UdpGroupEchoServer::INF_SESSION)
    {
      return "INF_SESSION";
    }
  else if (mode == UdpGroupEchoServer::NO_GROUP_SESSION)
    {
      return "INF_SESSION";
    }
  else if (mode == UdpGroupEchoServer::TIMEOUT_LIMITED)
    {
      return "TIMEOUT_LIMITED";
    }
  else
    {
      return "UNKNOWN";
    }
}

TypeId
UdpGroupEchoServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::UdpGroupEchoServer")
    .SetParent<Application> ()
    .SetGroupName ("Psc")
    .AddConstructor<UdpGroupEchoServer> ()
    .AddAttribute ("Port", "Port on which we listen for incoming packets.",
                   UintegerValue (9),
                   MakeUintegerAccessor (&UdpGroupEchoServer::m_port),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("EchoPort", "Port on which we echo packets to client.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&UdpGroupEchoServer::m_port_client),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Mode", "Determines the mode of echo operation",
                   EnumValue (Mode_t::NO_GROUP_SESSION),
                   MakeEnumAccessor (&UdpGroupEchoServer::m_mode),
                   MakeEnumChecker (Mode_t::INF_SESSION, "InfSession",
                                    Mode_t::NO_GROUP_SESSION, "NoGroupSession",
                                    Mode_t::TIMEOUT_LIMITED, "TimeoutLimited"))
    .AddAttribute ("Timeout", "Inactive client session expiration time",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&UdpGroupEchoServer::m_timeout),
                   MakeTimeChecker ())
    .AddAttribute ("EchoClient", "Server echoes back to the sending client",

                   BooleanValue (true),
                   MakeBooleanAccessor (&UdpGroupEchoServer::m_echoClient),
                   MakeBooleanChecker ())
    .AddTraceSource ("Rx", "A packet has been received",
                     MakeTraceSourceAccessor (&UdpGroupEchoServer::m_rxTrace),
                     "ns3::Packet::PacketAddressTracedCallback")
  ;
  return tid;
}

UdpGroupEchoServer::UdpGroupEchoServer ()
{
  NS_LOG_FUNCTION (this);
}

UdpGroupEchoServer::~UdpGroupEchoServer ()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_socket6 = 0;
}

void
UdpGroupEchoServer::AddClient (const Address& address)
{
  UdpGroupEchoClient src_client;
  std::ostringstream os;
  std::string ipaddrskey;

  src_client.m_address = address;
  src_client.m_timestamp = Simulator::Now ();

  if (InetSocketAddress::IsMatchingType (address))
    {
      InetSocketAddress clientAddress = InetSocketAddress::ConvertFrom (address);

      os << clientAddress.GetIpv4 () << ":" << clientAddress.GetPort ();
      ipaddrskey = os.str ();

      src_client.m_echo_address = InetSocketAddress (clientAddress.GetIpv4 (), m_port_client);
    }
  else if (Inet6SocketAddress::IsMatchingType (address))
    {
      Inet6SocketAddress clientAddress = Inet6SocketAddress::ConvertFrom (address);

      os << clientAddress.GetIpv6 () << ":" << clientAddress.GetPort ();
      ipaddrskey = os.str ();

      src_client.m_echo_address = Inet6SocketAddress (clientAddress.GetIpv6 (), m_port_client);
    }
  else
    {
      NS_ABORT_MSG ("Invalid address type");
    }
  m_clients[ipaddrskey] = src_client;

  LogClients ();
}

void
UdpGroupEchoServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
UdpGroupEchoServer::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_INFO ("Starting UdpGroupEchoServer with mode " << ModeToString (m_mode) << " session time " << m_timeout.GetSeconds ());
  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
      m_socket->Bind (local);
      if (addressUtils::IsMulticast (m_local))
        {
          Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket);
          if (udpSocket)
            {
              // equivalent to setsockopt (MCAST_JOIN_GROUP)
              udpSocket->MulticastJoinGroup (0, m_local);
            }
          else
            {
              NS_FATAL_ERROR ("Error: Failed to join multicast group");
            }
        }
    }

  if (m_socket6 == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket6 = Socket::CreateSocket (GetNode (), tid);
      Inet6SocketAddress local6 = Inet6SocketAddress (Ipv6Address::GetAny (), m_port);
      m_socket6->Bind (local6);
      if (addressUtils::IsMulticast (local6))
        {
          Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket6);
          if (udpSocket)
            {
              // equivalent to setsockopt (MCAST_JOIN_GROUP)
              udpSocket->MulticastJoinGroup (0, local6);
            }
          else
            {
              NS_FATAL_ERROR ("Error: Failed to join multicast group");
            }
        }
    }

  m_socket->SetRecvCallback (MakeCallback (&UdpGroupEchoServer::HandleRead, this));
  m_socket6->SetRecvCallback (MakeCallback (&UdpGroupEchoServer::HandleRead, this));
}

void
UdpGroupEchoServer::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
  if (m_socket6 != 0)
    {
      m_socket6->Close ();
      m_socket6->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
}

void
UdpGroupEchoServer::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from, echo_address;
  std::map<std::string, UdpGroupEchoClient>::iterator it, tempit;
  std::string ipaddrskey;
  std::ostringstream os;
  UdpGroupEchoClient src_client;
  Time lapse;

  while ((packet = socket->RecvFrom (from)))
    {
      m_rxTrace (packet, from);
      if (InetSocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("Server received " << packet->GetSize () << " bytes from " <<
                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                       InetSocketAddress::ConvertFrom (from).GetPort ());

          os << InetSocketAddress::ConvertFrom (from).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (from).GetPort ();

          InetSocketAddress inet_addrs = InetSocketAddress::ConvertFrom (from);
          inet_addrs.SetPort (m_port_client);
          echo_address = inet_addrs;
        }
      else if (Inet6SocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("Server received " << packet->GetSize () << " bytes from " <<
                       Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
                       Inet6SocketAddress::ConvertFrom (from).GetPort ());
          os << Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (from).GetPort ();

          Inet6SocketAddress inet6_addrs = Inet6SocketAddress::ConvertFrom (from);
          inet6_addrs.SetPort (m_port_client);
          echo_address = inet6_addrs;
        }


      packet->RemoveAllPacketTags ();
      packet->RemoveAllByteTags ();

      /* Update clients group with new/existing client.
       * If exist, update timestamp. Else, add client to group with new timestamp.
       * Echo packet.
       */

      ipaddrskey = os.str ();

      it = m_clients.find (ipaddrskey);
      if (it != m_clients.end ())
        {
          // Client is a group member. Udate timestamp.
          NS_LOG_DEBUG ("Client found; old timestamp: " << it->second.m_timestamp.GetSeconds ());
          it->second.m_timestamp = Simulator::Now ();
          NS_LOG_DEBUG ("New timestamp: " << it->second.m_timestamp.GetSeconds ());
        }
      else // Add client to group
        {
          src_client.m_address = from;
          src_client.m_echo_address = echo_address;
          src_client.m_timestamp = Simulator::Now ();
          m_clients[ipaddrskey] = src_client;
        }

      if (g_log.IsEnabled (LOG_DEBUG))
        {
          LogClients ();
        }

      /* Echoing packet to group.
       * Iterate through all the clients, check timestamp.
       * If timestamp has expired, remove client.
       * Else, echo/fwd packet to client.*/

      /* m_mode == INF_SESSION      : Server serves group clients indefinitely
       * m_mode == NO_GROUP_SESSION : Server behaves as single echo-client (no group echo)
       * m_mode == TIMEOUT_LIMITED  : Server group echo clients that have been active within
       *                 the m_timeout elapsed time.
       */
      Address addrs_dest;
      if (m_mode == INF_SESSION)
        {
          // Server serves group clients indefinitely.
          for (it = m_clients.begin ();
               it != m_clients.end (); ++it)
            {
              // If no echo back to client, neglect client source
              if (!m_echoClient && it->first == ipaddrskey)
                {
                  continue;
                }

              // Check elapsed time
              lapse = Simulator::Now () - it->second.m_timestamp;

              // Set destination address with the agreed client port.
              if (m_port_client != 0)
                {
                  addrs_dest = it->second.m_echo_address;
                }
              else
                {
                  addrs_dest = it->second.m_address;
                }

              // Forward packet
              NS_LOG_LOGIC ("Echoing packet");
              socket->SendTo (packet, 0, addrs_dest);

              if (InetSocketAddress::IsMatchingType (addrs_dest))
                {
                  NS_LOG_INFO ("Server sent "
                               << packet->GetSize ()
                               << " bytes to "
                               << InetSocketAddress::ConvertFrom (addrs_dest).GetIpv4 ()
                               << " port "
                               << InetSocketAddress::ConvertFrom (addrs_dest).GetPort ());
                }
              else if (Inet6SocketAddress::IsMatchingType (addrs_dest))
                {
                  NS_LOG_INFO ("Server sent "
                               << packet->GetSize ()
                               << " bytes to "
                               << Inet6SocketAddress::ConvertFrom (addrs_dest).GetIpv6 ()
                               << " port "
                               << Inet6SocketAddress::ConvertFrom (addrs_dest).GetPort ());
                }
            }
        }
      else if (m_mode == NO_GROUP_SESSION)
        {
          // Only one client allowed in group.
          it = m_clients.find (ipaddrskey);
          if (m_echoClient)
            {
              // Set destination address with the agreed client port.
              if (m_port_client != 0)
                {
                  addrs_dest = it->second.m_echo_address;
                }
              else
                {
                  addrs_dest = it->second.m_address;
                }

              // Forward packet
              NS_LOG_LOGIC ("Echoing packet");
              socket->SendTo (packet, 0, addrs_dest);

              if (InetSocketAddress::IsMatchingType (addrs_dest))
                {
                  NS_LOG_INFO ("Server sent "
                               << packet->GetSize ()
                               << " bytes to "
                               << InetSocketAddress::ConvertFrom (addrs_dest).GetIpv4 ()
                               << " port "
                               << InetSocketAddress::ConvertFrom (addrs_dest).GetPort ());
                }
              else if (Inet6SocketAddress::IsMatchingType (addrs_dest))
                {
                  NS_LOG_INFO ("Server sent "
                               << packet->GetSize ()
                               << " bytes to "
                               << Inet6SocketAddress::ConvertFrom (addrs_dest).GetIpv6 ()
                               << " port "
                               << Inet6SocketAddress::ConvertFrom (addrs_dest).GetPort ());
                }
            }
          // Remove client
          m_clients.erase (it);
        }
      else //if (m_mode == TIMEOUT_LIMITED)
        {
          for (it = m_clients.begin ();
               it != m_clients.end (); ++it)
            {
              // If no echo back, neglect client source
              if (!m_echoClient && it->first == ipaddrskey)
                {
                  continue;
                }

              // Check elapsed time
              lapse = Simulator::Now () - it->second.m_timestamp;

              if (lapse < m_timeout)
                {
                  // Set destination address with the agreed client port.
                  if (m_port_client != 0)
                    {
                      addrs_dest = it->second.m_echo_address;
                    }
                  else
                    {
                      addrs_dest = it->second.m_address;
                    }

                  // Forward packet
                  NS_LOG_LOGIC ("Echoing packet");
                  socket->SendTo (packet, 0, addrs_dest);


                  if (InetSocketAddress::IsMatchingType (addrs_dest))
                    {
                      NS_LOG_INFO ("Server sent "
                                   << packet->GetSize ()
                                   << " bytes to "
                                   << InetSocketAddress::ConvertFrom (addrs_dest).GetIpv4 ()
                                   << " port "
                                   << InetSocketAddress::ConvertFrom (addrs_dest).GetPort ());
                    }
                  else if (Inet6SocketAddress::IsMatchingType (addrs_dest))
                    {
                      NS_LOG_INFO ("Server sent "
                                   << packet->GetSize ()
                                   << " bytes to "
                                   << Inet6SocketAddress::ConvertFrom (addrs_dest).GetIpv6 ()
                                   << " port "
                                   << Inet6SocketAddress::ConvertFrom (addrs_dest).GetPort ());
                    }
                }
              else // Time has expired. Remove client.
                {
                  tempit = it;
                  --it;
                  m_clients.erase (tempit);
                }
            }
        } //end for
    } //end while
}

void
UdpGroupEchoServer::LogClients (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_INFO ("Number of clients: " << m_clients.size ());
  if ( m_clients.size () > 0 )
    {
      // Check time lapse
      Time tstamp = Simulator::Now ();
      Time lapse;
      NS_LOG_INFO (std::setfill ('-') << std::setw (57) << "-" << std::setfill (' '));
      NS_LOG_INFO (std::setw (23) << "Client  " << std::setw (10) << "Session");
      NS_LOG_INFO (std::setfill ('-') << std::setw (57) << "-" << std::setfill (' '));
      for (std::map<std::string, UdpGroupEchoClient>::iterator it = m_clients.begin ();
           it != m_clients.end (); ++it)
        {
          lapse = tstamp - it->second.m_timestamp;
          if (m_mode == INF_SESSION
              || m_mode == NO_GROUP_SESSION
              || (m_mode == TIMEOUT_LIMITED && lapse < m_timeout))
            {
              NS_LOG_INFO (std::setw (23) << it->first << " " << std::setw (10) << lapse.GetSeconds ());
            }
          else
            {
              NS_LOG_INFO (std::setw (23) << it->first << " " << std::setw (10) << lapse.GetSeconds () << " **Session Expired!**");
            }
        }
      NS_LOG_INFO (std::setfill ('=') << std::setw (57) << "=" << std::setfill (' '));
    }
}

} // Namespace psc
} // Namespace ns3
