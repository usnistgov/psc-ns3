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

#include "mcptt-chan.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE ("McpttChan");

NS_OBJECT_ENSURE_REGISTERED (McpttChan);

TypeId
McpttChan::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttChan")
    .SetParent<Object> ()
    .AddConstructor<McpttChan> ()
  ;

  return tid;
}

McpttChan::McpttChan (void)
  : Object (),
    m_rxPktCb (MakeNullCallback<void, Ptr<Packet> > ()),
    m_socket (0)
{
  NS_LOG_FUNCTION (this);
}

McpttChan::~McpttChan (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttChan::Close (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<Socket> socket = GetSocket ();

  socket->Close ();
  socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());

  SetSocket (0);
}

TypeId
McpttChan::GetInstanceTypeId (void) const
{
  return McpttChan::GetTypeId ();
}

bool
McpttChan::IsOpen (void) const
{
  NS_LOG_FUNCTION (this);
  
  Ptr<Socket> socket = GetSocket ();

  bool isOpen = (socket != 0);

  return isOpen;
}

int
McpttChan::Open (Ptr<Node>  node, uint16_t port, const Ipv4Address& local, const Ipv4Address& peer)
{
  NS_LOG_FUNCTION (this << node << port << local << peer);

  Ptr<Socket> socket = Socket::CreateSocket (node, UdpSocketFactory::GetTypeId ());
  InetSocketAddress peerAddress = InetSocketAddress (peer, port);
  InetSocketAddress localAddress = InetSocketAddress (local, port);

  socket->Bind (localAddress);
  socket->SetAllowBroadcast (true);
  socket->SetRecvCallback (MakeCallback (&McpttChan::ReceivePkts, this));
  NS_LOG_DEBUG ("Create socket on " << node->GetId () << " to " << peer << " port " << port);
  int result = socket->Connect (peerAddress);
  bool success = (result == 0);

  if (success == true)
    {
      SetSocket (socket);
    }
  else
    {
      socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> >());
      socket = 0;
    }

  return result;
}

int
McpttChan::Send (Ptr<Packet>  pkt)
{
  NS_LOG_FUNCTION (this << pkt);

  Ptr<Socket> subject = GetSocket ();

  int result = subject->Send (pkt);

  return result;
}

void
McpttChan::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  if (IsOpen ())
    {
      Close ();
    }

  Object::DoDispose ();
}

void
McpttChan::ReceivePkt (Ptr<Packet>  pkt)
{
  NS_LOG_FUNCTION (this << &pkt);

  if (!m_rxPktCb.IsNull ())
    {
      m_rxPktCb (pkt);
    }
}

void
McpttChan::ReceivePkts (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << &socket);

  Address from;
  Ptr<Packet> pkt;

  while ((pkt = socket->RecvFrom (from)))
    {
      uint32_t pktSize = pkt->GetSize ();
      InetSocketAddress peer = InetSocketAddress::ConvertFrom (from);

      NS_LOG_INFO (Simulator::Now ().GetSeconds () << "s: McpttChan received " << pktSize << " byte packet from " << peer.GetIpv4 () << ":" << peer.GetPort () << ".");
      ReceivePkt (pkt);
   }
}

Ptr<Socket>
McpttChan::GetSocket (void) const
{
  NS_LOG_FUNCTION (this);

  return m_socket;
}

void
McpttChan::SetSocket (Ptr<Socket>  socket)
{
  NS_LOG_FUNCTION (this << &socket);

  m_socket = socket;
}

void
McpttChan::SetRxPktCb (const Callback<void, Ptr<Packet> >  rxPktCb)
{
  NS_LOG_FUNCTION (this << &rxPktCb);
  
  m_rxPktCb = rxPktCb;
}

} // namespace ns3

