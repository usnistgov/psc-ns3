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

#include "nist-udp-groupecho-server.h"
#include <sstream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistUdpGroupEchoServerApplication");

NS_OBJECT_ENSURE_REGISTERED (NistUdpGroupEchoServer);

TypeId
NistUdpGroupEchoServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistUdpGroupEchoServer")
    .SetParent<Application> ()
    .AddConstructor<NistUdpGroupEchoServer> ()
    .AddAttribute ("Port", "Port on which we listen for incoming packets.",
                   UintegerValue (9),
                   MakeUintegerAccessor (&NistUdpGroupEchoServer::m_port),
                   MakeUintegerChecker<uint16_t> ())
	.AddAttribute ("EchoPort", "Port on which we echo packets to client.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NistUdpGroupEchoServer::m_port_client),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Timeout", "Inactive client session expiration time <seconds>.\nSessionTime  < 0 : Server echoes group clients indefinitely\nSessionTime == 0 : Server echoes single client (Default)\nSessionTime  > 0 : Server forwards packets to group. Session timeout units are seconds.",
                   DoubleValue (0),
                   MakeDoubleAccessor (&NistUdpGroupEchoServer::m_timeout),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Echo", "Server echoes back client. True (default) | False",
                   BooleanValue (true),
                   MakeBooleanAccessor (&NistUdpGroupEchoServer::m_echoback),
                   MakeBooleanChecker ())
    .AddTraceSource ("Rx", "A packet has been received",
				   MakeTraceSourceAccessor (&NistUdpGroupEchoServer::m_rxTrace),
				   "ns3::Packet::PacketAddressTracedCallback")
  ;
  
  return tid;
}

NistUdpGroupEchoServer::NistUdpGroupEchoServer ()
{
  NS_LOG_FUNCTION (this);
}

NistUdpGroupEchoServer::~NistUdpGroupEchoServer()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_socket6 = 0;
}

void
NistUdpGroupEchoServer::AddClient (const Address& address)
{
  client src_client;
  std::ostringstream os;
  InetSocketAddress clientAddress = InetSocketAddress::ConvertFrom (address);

  os << clientAddress.GetIpv4 () << ":" << clientAddress.GetPort ();
  std::string ipaddrskey = os.str ();

  src_client.addrs = address;
	src_client.echo_addrs = InetSocketAddress (clientAddress.GetIpv4 (), m_port_client);
  src_client.tstamp = Simulator::Now ();

  m_clients[ipaddrskey] = src_client;

  this->PrintClients ();
}

void
NistUdpGroupEchoServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void 
NistUdpGroupEchoServer::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_INFO("Starting NistUdpGroupEchoServer with SessionTime: " << m_timeout);
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

  m_socket->SetRecvCallback (MakeCallback (&NistUdpGroupEchoServer::HandleRead, this));
  m_socket6->SetRecvCallback (MakeCallback (&NistUdpGroupEchoServer::HandleRead, this));
}

void 
NistUdpGroupEchoServer::StopApplication ()
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
NistUdpGroupEchoServer::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from, echo_addrs;
  std::map<std::string,client>::iterator it, tempit;
  std::string ipaddrskey;
  std::ostringstream os;
  client src_client;
  double lapse;
  
  while ((packet = socket->RecvFrom (from)))
    {
	  m_rxTrace (packet, from);
      if (InetSocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO (Simulator::Now ().GetSeconds () << " server received " << packet->GetSize () << " bytes from " <<
                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                       InetSocketAddress::ConvertFrom (from).GetPort ());

          os << InetSocketAddress::ConvertFrom (from).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (from).GetPort ();
		  
		  InetSocketAddress inet_addrs = InetSocketAddress::ConvertFrom (from);
		  inet_addrs.SetPort(m_port_client);
		  echo_addrs = inet_addrs;
        }
      else if (Inet6SocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO (Simulator::Now ().GetSeconds () << " server received " << packet->GetSize () << " bytes from " <<
                       Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
                       Inet6SocketAddress::ConvertFrom (from).GetPort ());
          os << Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (from).GetPort ();
		  
		  Inet6SocketAddress inet6_addrs = Inet6SocketAddress::ConvertFrom (from);
		  inet6_addrs.SetPort(m_port_client);
		  echo_addrs = inet6_addrs;
        }
      

      packet->RemoveAllPacketTags ();
      packet->RemoveAllByteTags ();
      
      /* Update clients group with new/existing client.
       * If exist, update timestamp. Else, add client to group with new timestamp.
       * Echo packet.
       */

      ipaddrskey = os.str ();
      
      it = m_clients.find(ipaddrskey);
      if (it != m_clients.end ())
        {
          // Client is a group member. Udate timestamp.
          //std::cout<<"Client found; old timestamp: " << it->second.tstamp.GetSeconds ()<<std::endl;
          it->second.tstamp = Simulator::Now ();
          //std::cout<<"New timestamp: " << it->second.tstamp.GetSeconds ()<<std::endl;
        }
      else // Add client to group
        {
		  //client src_client;
          src_client.addrs = from;
		  src_client.echo_addrs = echo_addrs;
          src_client.tstamp = Simulator::Now ();
          m_clients[ipaddrskey] = src_client;
        }
      
      if(g_log.IsEnabled (LOG_DEBUG))
        this->PrintClients ();

      /* Echoing packet to group.
       * Iterate through all the clients, check timestamp.
       * If timestamp has expired, remove client.
       * Else, echo/fwd packet to client.*/

      /* m_timeout  < 0 : Server serves group clients indefinitely
       * m_timeout == 0 : Server behaves as single echo-client (no group echo)
       * m_timeout  > 0 : Server group echo clients that have been active within 
       *                 the m_timeout elapsed time. 
       */
      Address addrs_dest; 
      if (m_timeout < 0)
        { 
          // Server serves group clients indefinitely.
          for (it = m_clients.begin ();
               it!=m_clients.end (); ++it)
            { 
			  // If no echo back, neglect client source
			  if (!m_echoback && it->first == ipaddrskey)
				  continue;
			  
              // Check elapsed time
              lapse = Simulator::Now ().GetSeconds () - it->second.tstamp.GetSeconds ();
              
			  // Set destination address with the agreed client port.
			  if (m_port_client !=0)
				addrs_dest = it->second.echo_addrs;
			  else
				addrs_dest = it->second.addrs;
			  
			  // Forward packet
			  NS_LOG_LOGIC ("Echoing packet");
              socket->SendTo (packet, 0, addrs_dest);
              
              if (InetSocketAddress::IsMatchingType (addrs_dest))
                {
                  NS_LOG_INFO (Simulator::Now ().GetSeconds () 
                               << " server sent " 
                               << packet->GetSize () 
                               << " bytes to "
                               << InetSocketAddress::ConvertFrom (addrs_dest).GetIpv4 () 
                               << " port " 
                               << InetSocketAddress::ConvertFrom (addrs_dest).GetPort ());
                }
              else if (Inet6SocketAddress::IsMatchingType (addrs_dest))
                {
                  NS_LOG_INFO (Simulator::Now ().GetSeconds () 
                               << " server sent " 
                               << packet->GetSize () 
                               << " bytes to " 
                               << Inet6SocketAddress::ConvertFrom (addrs_dest).GetIpv6 () 
                               << " port " 
                               << Inet6SocketAddress::ConvertFrom (addrs_dest).GetPort ());
                }
            }
        }
      else if (m_timeout == 0)
        { 
		  // Only one client allowed in group.
          it = m_clients.find(ipaddrskey);
          if (m_echoback)
			{
			  // Set destination address with the agreed client port.
			  if (m_port_client !=0)
				addrs_dest = it->second.echo_addrs;
			  else
				addrs_dest = it->second.addrs;
			  
			  // Forward packet
			  NS_LOG_LOGIC ("Echoing packet");
              socket->SendTo (packet, 0, addrs_dest);
				  
			  if (InetSocketAddress::IsMatchingType (addrs_dest))
				{
				  NS_LOG_INFO (Simulator::Now ().GetSeconds () 
							   << " server sent " 
							   << packet->GetSize () 
							   << " bytes to "
							   << InetSocketAddress::ConvertFrom (addrs_dest).GetIpv4 () 
							   << " port " 
							   << InetSocketAddress::ConvertFrom (addrs_dest).GetPort ());
				}
			  else if (Inet6SocketAddress::IsMatchingType (addrs_dest))
				{
				  NS_LOG_INFO (Simulator::Now ().GetSeconds () 
							   << " server sent " 
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
      else //if (m_timeout > 0)
        {
          for (it = m_clients.begin ();
               it!=m_clients.end (); ++it)
            { 
			  // If no echo back, neglect client source
			  if (!m_echoback && it->first == ipaddrskey)
				  continue;
			  
              // Check elapsed time
              lapse = Simulator::Now ().GetSeconds () - it->second.tstamp.GetSeconds ();
              
              if (lapse < m_timeout)
                {
                  // Set destination address with the agreed client port.
				  if (m_port_client !=0)
					addrs_dest = it->second.echo_addrs;
				  else
					addrs_dest = it->second.addrs;
				  
				  // Forward packet
				  NS_LOG_LOGIC ("Echoing packet");
				  socket->SendTo (packet, 0, addrs_dest);
				  
              
                  if (InetSocketAddress::IsMatchingType (addrs_dest))
                    {
                      NS_LOG_INFO (Simulator::Now ().GetSeconds () 
                                   << " server sent " 
                                   << packet->GetSize () 
                                   << " bytes to "
                                   << InetSocketAddress::ConvertFrom (addrs_dest).GetIpv4 () 
                                   << " port " 
                                   << InetSocketAddress::ConvertFrom (addrs_dest).GetPort ());
                    }
                  else if (Inet6SocketAddress::IsMatchingType (addrs_dest))
                    {
                      NS_LOG_INFO (Simulator::Now ().GetSeconds () 
                                   << " server sent " 
                                   << packet->GetSize () 
                                   << " bytes to " 
                                   << Inet6SocketAddress::ConvertFrom (addrs_dest).GetIpv6 () 
                                   << " port " 
                                   << Inet6SocketAddress::ConvertFrom (addrs_dest).GetPort ());
                    }
                }
              else // Time has expired. Remove client.
                {
                  //NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () <<" client " << it->first << " session time expired! ");
                  tempit = it;
                  --it;
                  m_clients.erase (tempit);
                }
            }
        }//end for
    }//end while
}

void
NistUdpGroupEchoServer::PrintClients (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_INFO (Simulator::Now ().GetSeconds ()
               << " number of clients: " << m_clients.size ());
  if( m_clients.size () > 0 )
    {
      // Check time lapse
      double tstamp = Simulator::Now ().GetSeconds ();
      double lapse;
      NS_LOG_INFO (std::setfill ('-') << std::setw(57) << "-" << std::setfill (' '));
      NS_LOG_INFO (std::setw(23) << "Client  " << std::setw(10) << "Session");
      NS_LOG_INFO (std::setfill ('-') << std::setw(57) << "-" << std::setfill (' '));
      for (std::map<std::string,client>::iterator it = m_clients.begin ();
           it!=m_clients.end (); ++it)
        {
          lapse = tstamp - it->second.tstamp.GetSeconds ();
          if (m_timeout < 0 || lapse < m_timeout)
            NS_LOG_INFO (std::setw(23) << it->first << " " << std::setw(10) << lapse);
          else
            NS_LOG_INFO (std::setw(23) << it->first << " " << std::setw(10) << lapse << " **Session Expired!**");
        }
      NS_LOG_INFO (std::setfill ('=') << std::setw(57) << "=" << std::setfill (' '));
    }
}

} // Namespace ns3
