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

#include <ns3/ipv4-address.h>
#include <ns3/log.h>
#include <ns3/node.h>
#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/simulator.h>
#include <ns3/socket.h>
#include <ns3/type-id.h>
#include <ns3/udp-socket-factory.h>

#include "mcptt-channel.h"

namespace ns3 {

namespace psc {

NS_LOG_COMPONENT_DEFINE ("McpttChannel");

NS_OBJECT_ENSURE_REGISTERED (McpttChannel);

TypeId
McpttChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttChannel")
    .SetParent<Object> ()
    .AddConstructor<McpttChannel> ()
  ;

  return tid;
}

McpttChannel::McpttChannel (void)
  : Object (),
    m_rxPktCb (MakeNullCallback<void, Ptr<Packet>, Address> ()),
    m_socket (0)
{
  NS_LOG_FUNCTION (this);
}

McpttChannel::~McpttChannel (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttChannel::Close (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<Socket> socket = GetSocket ();

  socket->Close ();
  socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());

  SetSocket (0);
}

TypeId
McpttChannel::GetInstanceTypeId (void) const
{
  return McpttChannel::GetTypeId ();
}

bool
McpttChannel::IsOpen (void) const
{
  NS_LOG_FUNCTION (this);

  Ptr<Socket> socket = GetSocket ();

  bool isOpen = (socket != 0);

  return isOpen;
}

int
McpttChannel::Open (Ptr<Node>  node, uint16_t port, const Address& local, const Address& peer)
{
  NS_LOG_FUNCTION (this << node << port << local << peer);

  int result = -1;
  std::stringstream ssPeer;
  std::stringstream ssLocal;
  Ptr<Socket> socket = Socket::CreateSocket (node, UdpSocketFactory::GetTypeId ());

  if (Ipv4Address::IsMatchingType (local))
    {
      Ipv4Address localIpv4 = Ipv4Address::GetAny ();
      result = socket->Bind (InetSocketAddress (localIpv4, port));
      ssLocal << localIpv4;
    }
  else if (Ipv6Address::IsMatchingType (local))
    {
      Ipv6Address localIpv6 = Ipv6Address::GetAny ();
      result = socket->Bind (Inet6SocketAddress (localIpv6, port));
      ssLocal << localIpv6;
    }
  else
    {
      NS_ABORT_MSG ("Only Ipv4 and Ipv6 address are supported.");
    }
  if (result != 0)
    {
      return result;
    }

  socket->SetAllowBroadcast (true);
  socket->SetRecvCallback (MakeCallback (&McpttChannel::ReceivePkts, this));

  bool success = false;
  if (Ipv4Address::IsMatchingType (peer))
    {
      Ipv4Address peerIpv4 = Ipv4Address::ConvertFrom (peer);
      if (!peerIpv4.IsAny ())
        {
          result = socket->Connect (InetSocketAddress (peerIpv4, port));
          ssPeer << peerIpv4;
          NS_LOG_DEBUG ("Create socket on " << node->GetId () << " from " << ssLocal.str () << " port " << port << " to " << ssPeer.str () << " port " << port);
          success = (result == 0) ? true : false;
        }
      else
        {
          NS_LOG_DEBUG ("Create unconnected socket locally on " << node->GetId () << " address " << ssLocal.str () << " port " << port);
          success = true;
        }
    }
  else if (Ipv6Address::IsMatchingType (peer))
    {
      Ipv6Address peerIpv6 = Ipv6Address::ConvertFrom (peer);
      if (!peerIpv6.IsAny ())
        {
          result = socket->Connect (Inet6SocketAddress (peerIpv6, port));
          ssPeer << peerIpv6;
          NS_LOG_DEBUG ("Create socket on " << node->GetId () << " from " << ssLocal.str () << " port " << port << " to " << ssPeer.str () << " port " << port);
          success = (result == 0) ? true : false;
        }
      else
        {
          NS_LOG_DEBUG ("Create unconnected socket locally on " << node->GetId () << " address " << ssLocal.str () << " port " << port);
          success = true;
        }
    }
  else
    {
      NS_ABORT_MSG ("Only Ipv4 and Ipv6 address are supported.");
    }

  if (success == true)
    {
      SetSocket (socket);
    }
  else
    {
      NS_LOG_DEBUG ("Unable to create socket");
      socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> >());
      socket = 0;
    }

  return result;
}

int
McpttChannel::Send (Ptr<Packet>  pkt)
{
  NS_LOG_FUNCTION (this << pkt);

  Ptr<Socket> subject = GetSocket ();

  int result = subject->Send (pkt);

  return result;
}

int
McpttChannel::SendTo (Ptr<Packet> p, uint32_t flags, const Address &toAddress)
{
  NS_LOG_FUNCTION (this << p << flags << toAddress);
  return GetSocket ()->SendTo (p, flags, toAddress);
}

void
McpttChannel::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  if (IsOpen ())
    {
      Close ();
    }

  Object::DoDispose ();
}

void
McpttChannel::ReceivePkt (Ptr<Packet>  pkt, Address from)
{
  NS_LOG_FUNCTION (this << &pkt << from);

  if (!m_rxPktCb.IsNull ())
    {
      m_rxPktCb (pkt, from);
    }
}

void
McpttChannel::ReceivePkts (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << &socket);

  Address from;
  Ptr<Packet> pkt;
  std::stringstream ssAddress;
  std::stringstream ssPort;

  while ((pkt = socket->RecvFrom (from)))
    {
      uint32_t pktSize = pkt->GetSize ();
      if (InetSocketAddress::IsMatchingType (from))
        {
          InetSocketAddress address = InetSocketAddress::ConvertFrom (from);
          ssAddress << address.GetIpv4 ();
          ssPort << (uint32_t)address.GetPort ();
        }
      else if (Inet6SocketAddress::IsMatchingType (from))
        {
          Inet6SocketAddress address6 = Inet6SocketAddress::ConvertFrom (from);
          ssAddress << address6.GetIpv6 ();
          ssPort << (uint32_t)address6.GetPort ();
        }
      else
        {
          NS_ABORT_MSG ("Only Ipv4 and Ipv6 address are supported.");
        }

      NS_LOG_INFO ("McpttChannel received " << pktSize << " byte packet from " << ssAddress.str () << ":" << ssPort.str () << ".");
      ReceivePkt (pkt, from);
    }
}

Ptr<Socket>
McpttChannel::GetSocket (void) const
{
  NS_LOG_FUNCTION (this);

  return m_socket;
}

void
McpttChannel::SetSocket (Ptr<Socket>  socket)
{
  NS_LOG_FUNCTION (this << &socket);

  m_socket = socket;
}

void
McpttChannel::SetRxPktCb (const Callback<void, Ptr<Packet>, Address>  rxPktCb)
{
  NS_LOG_FUNCTION (this << &rxPktCb);

  m_rxPktCb = rxPktCb;
}

} // namespace psc
} // namespace ns3

