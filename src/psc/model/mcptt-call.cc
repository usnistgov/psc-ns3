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

#include <ns3/callback.h>
#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/pointer.h>
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
#include "mcptt-on-network-call-machine-client.h"
#include "mcptt-ptt-app.h"

#include "mcptt-call.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCall");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttCall);

TypeId
McpttCall::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttCall")
    .SetParent<Object> ()
    .AddConstructor<McpttCall> ()
    .AddAttribute ("CallMachine", "The call machine of the call.",
                   PointerValue (0),
                   MakePointerAccessor (&McpttCall::m_callMachine),
                   MakePointerChecker<McpttCallMachine> ())
    .AddAttribute ("DefaultOnNetworkCallPort", "The default port for on-network call control messages.",
                   UintegerValue (5060), // standard SIP call control port
                   MakeUintegerAccessor (&McpttCall::m_defaultOnNetworkCallPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("DefaultOffNetworkCallPort", "The default port for off-network call control messages.",
                   UintegerValue (8809), // standard MONP IANA allocation
                   MakeUintegerAccessor (&McpttCall::m_defaultOffNetworkCallPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("CallPort", "The port to use for call control messages.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&McpttCall::m_callPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("FloorMachine", "The floor machine of the call.",
                   PointerValue (0),
                   MakePointerAccessor (&McpttCall::m_floorMachine),
                   MakePointerChecker<McpttFloorParticipant> ())
    .AddAttribute ("PeerAddress", "The address of the node that the peer application is on.",
                   AddressValue (Ipv4Address ("255.255.255.255")),
                   MakeAddressAccessor (&McpttCall::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("PeerUserId", "The user ID of the peer (or server) application (for on-network operation).",
                   UintegerValue (0),
                   MakeUintegerAccessor (&McpttCall::m_peerUserId),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}

McpttCall::McpttCall (void)
  : Object (),
    m_networkCallType (NetworkCallType::INVALID), // type must be set later
    m_floorChannel (0),
    m_mediaChannel (0),
    m_owner (0),
    m_pushOnSelect (false),
    m_startTime (Seconds (0)),
    m_stopTime (Seconds (0))
{
  NS_LOG_FUNCTION (this);
}

McpttCall::McpttCall (NetworkCallType callType)
  : Object (),
    m_networkCallType (callType),
    m_floorChannel (0),
    m_mediaChannel (0),
    m_owner (0),
    m_pushOnSelect (false),
    m_startTime (Seconds (0)),
    m_stopTime (Seconds (0))
{
  NS_LOG_FUNCTION (this);
}

McpttCall::~McpttCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCall::CloseFloorChannel (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttChannel> floorChannel = GetFloorChannel ();
  floorChannel->Close ();
}

void
McpttCall::CloseMediaChannel (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttChannel> mediaChannel = GetMediaChannel ();
  mediaChannel->Close ();
}

void
McpttCall::SetCallId (uint16_t callId)
{
  NS_LOG_DEBUG (this << callId);
  if (m_callMachine)
    {
      m_callMachine->SetCallId (callId);
    }
  else
    {
      NS_FATAL_ERROR ("Error:  setting call ID on a call without a call machine");
    }
}

uint16_t
McpttCall::GetCallId (void) const
{
  if (m_callMachine)
    {
      return m_callMachine->GetCallId ().GetCallId ();
    }
  else
    {
      NS_LOG_WARN ("CallId (CallMachine) not yet set");
      return 0;
    }
}

McpttCall::NetworkCallType
McpttCall::GetNetworkCallType (void) const
{
  return m_networkCallType;
}

bool
McpttCall::GetPushOnSelect (void) const
{
  return m_pushOnSelect;
}

void
McpttCall::SetPushOnSelect (bool pushOnSelect)
{
  m_pushOnSelect = pushOnSelect;
}

bool
McpttCall::IsFloorChannelOpen (void) const
{
  Ptr<McpttChannel> floorChannel = GetFloorChannel ();
  bool isOpen = floorChannel->IsOpen ();

  return isOpen;
}

bool
McpttCall::IsMediaChannelOpen (void) const
{
  Ptr<McpttChannel> mediaChannel = GetMediaChannel ();
  bool isOpen = mediaChannel->IsOpen ();

  return isOpen;
}

void
McpttCall::OpenFloorChannel (const Address& peerAddr, const uint16_t port)
{
  NS_LOG_FUNCTION (this << peerAddr << port);

  Ptr<McpttPttApp> owner = GetOwner ();
  Ptr<Node> node = owner->GetNode ();
  Ptr<McpttChannel> floorChannel = GetFloorChannel ();
  Address localAddr = owner->GetLocalAddress ();

  int result = floorChannel->Open (node, port, localAddr, peerAddr);
  NS_ABORT_MSG_UNLESS (result == 0, "Unable to open floor channel on node " << GetOwner ()->GetNode ()->GetId ());
}

void
McpttCall::OpenMediaChannel (const Address& peerAddr, const uint16_t port)
{
  NS_LOG_FUNCTION (this << peerAddr << port);

  Ptr<McpttPttApp> owner = GetOwner ();
  Ptr<Node> node = owner->GetNode ();
  Ptr<McpttChannel> mediaChannel = GetMediaChannel ();
  Address localAddr = owner->GetLocalAddress ();

  int result = mediaChannel->Open (node, port, localAddr, peerAddr);
  NS_ABORT_MSG_UNLESS (result == 0, "Unable to open media channel on node " << GetOwner ()->GetNode ()->GetId ());
}

void
McpttCall::ReceiveSipMessage (Ptr<Packet> pkt, const sip::SipHeader& hdr, sip::SipAgent::TransactionState state)
{
  NS_LOG_FUNCTION (this << pkt << hdr << state);
  // SIP layer removes SIP header; add it back for tracing
  Ptr<Packet> pktCopy = pkt->Copy ();
  pktCopy->AddHeader (hdr);
  GetOwner ()->TraceMessageReceive (GetCallId (), pktCopy, hdr.GetInstanceTypeId ());
  GetCallMachine ()->GetObject<McpttOnNetworkCallMachineClient> ()->ReceiveCallPacket (pkt, hdr);
}

void
McpttCall::ReceiveSipEvent (const char* event, sip::SipAgent::TransactionState state)
{
  NS_LOG_FUNCTION (this << event << state);
  // Not yet defined
}

void
McpttCall::Receive (Ptr<Packet> pkt, const sip::SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << pkt);
  // SIP layer removes SIP header; add it back for tracing
  Ptr<Packet> pktCopy = pkt->Copy ();
  pktCopy->AddHeader (hdr);
  if (hdr.GetCallId () != GetCallId ())
    {
      NS_LOG_DEBUG ("Discarding SIP packet for call ID " << hdr.GetCallId ());
      return;
    }
  NS_LOG_DEBUG ("Received SIP packet for call ID " << GetCallId ());
  GetOwner ()->TraceMessageReceive (GetCallId (), pktCopy, hdr.GetInstanceTypeId ());
  GetCallMachine ()->GetObject<McpttOnNetworkCallMachineClient> ()->ReceiveCallPacket (pkt, hdr);
}

void
McpttCall::Receive (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCallMachine> callMachine = GetCallMachine ();
  callMachine->Receive (msg);
}

void
McpttCall::Receive (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttFloorParticipant> floorMachine = GetFloorMachine ();
  floorMachine->Receive (msg);
}

void
McpttCall::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttCallMachine> callMachine = GetCallMachine ();
  Ptr<McpttFloorParticipant> floorMachine = GetFloorMachine ();

  callMachine->Receive (msg);
  floorMachine->Receive (msg);
}

Address
McpttCall::GetPeerSocketAddress (void) const
{
  if (Ipv4Address::IsMatchingType (m_peerAddress))
    {
      Ipv4Address peer = Ipv4Address::ConvertFrom (m_peerAddress);
      return InetSocketAddress (peer, m_callPort);
    }
  else if (Ipv6Address::IsMatchingType (m_peerAddress))
    {
      Ipv6Address peer = Ipv6Address::ConvertFrom (m_peerAddress);
      return Inet6SocketAddress (peer, m_callPort);
    }
  else
    {
      NS_FATAL_ERROR ("Address type unsupported");
    }
  return Address (); // unreachable
}

uint32_t
McpttCall::GetPeerUserId (void) const
{
  return m_peerUserId;
}

// on-network sends SIP-based messages
void
McpttCall::SendSipMessage (Ptr<Packet> pkt, const Address& addr, const sip::SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << pkt << hdr);
  if (!GetOwner ()->IsRunning ())
    {
      NS_LOG_DEBUG ("Not sending message because McpttPttApp is not running");
      return;
    }
  if (Ipv4Address::IsMatchingType (m_peerAddress))
    {
      GetCallChannel ()->SendTo (pkt, 0, InetSocketAddress (Ipv4Address::ConvertFrom (m_peerAddress), m_callPort));
      GetOwner ()->TraceMessageSend (GetCallId (), pkt, hdr.GetInstanceTypeId ());
    }
  else if (Ipv6Address::IsMatchingType (m_peerAddress))
    {
      GetCallChannel ()->SendTo (pkt, 0, Inet6SocketAddress (Ipv6Address::ConvertFrom (m_peerAddress), m_callPort));
      GetOwner ()->TraceMessageSend (GetCallId (), pkt, hdr.GetInstanceTypeId ());
    }
  else
    {
      NS_LOG_DEBUG ("Not sending message because m_peerAddress unrecognized: " << m_peerAddress);
    }
}

// off-network directly sends McpttCallMsg
void
McpttCall::Send (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<Packet> pkt = Create<Packet> ();

  pkt->AddHeader (msg);

  if (Ipv4Address::IsMatchingType (m_peerAddress))
    {
      Ipv4Address peer = Ipv4Address::ConvertFrom (m_peerAddress);
      GetCallChannel ()->SendTo (pkt, 0, InetSocketAddress (peer, m_callPort));
      GetOwner ()->TraceMessageSend (GetCallId (), pkt, msg.GetInstanceTypeId ());
    }
  else if (Ipv6Address::IsMatchingType (m_peerAddress))
    {
      Ipv6Address peer = Ipv6Address::ConvertFrom (m_peerAddress);
      GetCallChannel ()->SendTo (pkt, 0, Inet6SocketAddress (peer, m_callPort));
      GetOwner ()->TraceMessageSend (GetCallId (), pkt, msg.GetInstanceTypeId ());
    }
}

void
McpttCall::Send (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<Packet> pkt = Create<Packet> ();
  Ptr<McpttChannel> floorChannel = GetFloorChannel ();

  pkt->AddHeader (msg);

  GetOwner ()->TraceMessageSend (GetCallId (), pkt, msg.GetInstanceTypeId ());
  floorChannel->Send (pkt);
}

void
McpttCall::Send (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<Packet> pkt = Create<Packet> ();
  Ptr<McpttChannel> mediaChannel = GetMediaChannel ();
  Ptr<McpttFloorParticipant> floorMachine = GetFloorMachine ();

  McpttMediaMsg txMsg (msg);

  floorMachine->MediaReady (txMsg);
  pkt->AddHeader (txMsg);
  GetOwner ()->TraceMessageSend (GetCallId (), pkt, msg.GetInstanceTypeId ());

  mediaChannel->Send (pkt);
}

void
McpttCall::Start (void)
{
  NS_LOG_FUNCTION (this);
  if (m_callPort == 0)
    {
      // user has not provided a non-default port, so initialize to default
      if (m_networkCallType == McpttCall::NetworkCallType::ON_NETWORK)
        {
          m_callPort = m_defaultOnNetworkCallPort;
          NS_LOG_DEBUG ("Starting on-network call on port " << m_callPort);
        }
      else if (m_networkCallType == McpttCall::NetworkCallType::OFF_NETWORK)
        {
          m_callPort = m_defaultOffNetworkCallPort;
          NS_LOG_DEBUG ("Starting off-network call on port " << m_callPort);
        }
    }
  else
    {
      NS_LOG_DEBUG ("Starting call on port " << m_callPort);
    }
  GetOwner ()->OpenCallChannel (m_callPort, this, m_networkCallType);
  GetCallMachine ()->Start ();
}

void
McpttCall::Stop (void)
{
  NS_LOG_FUNCTION (this);

  GetCallMachine ()->Stop ();
  GetOwner ()->CloseCallChannel (m_callPort, this, m_networkCallType);
}

void
McpttCall::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  GetCallMachine ()->Dispose ();
  SetCallMachine (0);
  SetFloorChannel (0);
  SetFloorMachine (0);
  SetMediaChannel (0);
  SetOwner (0);

  Object::DoDispose ();
}

void
McpttCall::ReceiveFloorPkt (Ptr<Packet>  pkt, Address from)
{
  NS_LOG_FUNCTION (this << &pkt << from);

  McpttFloorMsg temp;

  pkt->PeekHeader (temp);
  uint8_t subtype = temp.GetSubtype ();

  if (subtype == McpttFloorMsgRequest::SUBTYPE)
    {
      McpttFloorMsgRequest reqMsg;
      GetOwner ()->TraceMessageReceive (GetCallId (), pkt, reqMsg.GetInstanceTypeId ());
      pkt->RemoveHeader (reqMsg);
      Receive (reqMsg);
    }
  else if (subtype == McpttFloorMsgGranted::SUBTYPE
           || subtype == McpttFloorMsgGranted::SUBTYPE_ACK)
    {
      McpttFloorMsgGranted grantedMsg;
      GetOwner ()->TraceMessageReceive (GetCallId (), pkt, grantedMsg.GetInstanceTypeId ());
      pkt->RemoveHeader (grantedMsg);
      Receive (grantedMsg);
    }
  else if (subtype == McpttFloorMsgDeny::SUBTYPE
           || subtype == McpttFloorMsgDeny::SUBTYPE_ACK)
    {
      McpttFloorMsgDeny denyMsg;
      GetOwner ()->TraceMessageReceive (GetCallId (), pkt, denyMsg.GetInstanceTypeId ());
      pkt->RemoveHeader (denyMsg);
      Receive (denyMsg);
    }
  else if (subtype == McpttFloorMsgRelease::SUBTYPE
           || subtype == McpttFloorMsgRelease::SUBTYPE_ACK)
    {
      McpttFloorMsgRelease releaseMsg;
      GetOwner ()->TraceMessageReceive (GetCallId (), pkt, releaseMsg.GetInstanceTypeId ());
      pkt->RemoveHeader (releaseMsg);
      Receive (releaseMsg);
    }
  else if (subtype == McpttFloorMsgIdle::SUBTYPE
           || subtype == McpttFloorMsgIdle::SUBTYPE_ACK)
    {
      McpttFloorMsgIdle idleMsg;
      GetOwner ()->TraceMessageReceive (GetCallId (), pkt, idleMsg.GetInstanceTypeId ());
      pkt->RemoveHeader (idleMsg);
      Receive (idleMsg);
    }
  else if (subtype == McpttFloorMsgTaken::SUBTYPE
           || subtype == McpttFloorMsgTaken::SUBTYPE_ACK)
    {
      McpttFloorMsgTaken takenMsg;
      GetOwner ()->TraceMessageReceive (GetCallId (), pkt, takenMsg.GetInstanceTypeId ());
      pkt->RemoveHeader (takenMsg);
      Receive (takenMsg);
    }
  else if (subtype == McpttFloorMsgRevoke::SUBTYPE)
    {
      McpttFloorMsgRevoke revokeMsg;
      GetOwner ()->TraceMessageReceive (GetCallId (), pkt, revokeMsg.GetInstanceTypeId ());
      pkt->RemoveHeader (revokeMsg);
      Receive (revokeMsg);
    }
  else if (subtype == McpttFloorMsgQueuePositionRequest::SUBTYPE)
    {
      McpttFloorMsgQueuePositionRequest queuePositionRequestMsg;
      GetOwner ()->TraceMessageReceive (GetCallId (), pkt, queuePositionRequestMsg.GetInstanceTypeId ());
      pkt->RemoveHeader (queuePositionRequestMsg);
      Receive (queuePositionRequestMsg);
    }
  else if (subtype == McpttFloorMsgQueuePositionInfo::SUBTYPE
           || subtype == McpttFloorMsgQueuePositionInfo::SUBTYPE_ACK)
    {
      McpttFloorMsgQueuePositionInfo queueInfoMsg;
      GetOwner ()->TraceMessageReceive (GetCallId (), pkt, queueInfoMsg.GetInstanceTypeId ());
      pkt->RemoveHeader (queueInfoMsg);
      Receive (queueInfoMsg);
    }
  else if (subtype == McpttFloorMsgAck::SUBTYPE)
    {
      McpttFloorMsgAck ackMsg;
      GetOwner ()->TraceMessageReceive (GetCallId (), pkt, ackMsg.GetInstanceTypeId ());
      pkt->RemoveHeader (ackMsg);
      Receive (ackMsg);
    }
  else
    {
      NS_FATAL_ERROR ("Could not resolve message subtype = " << (uint32_t)subtype << ".");
    }
}

void
McpttCall::ReceiveMediaPkt (Ptr<Packet>  pkt, Address from)
{
  NS_LOG_FUNCTION (this << &pkt << from);

  McpttMediaMsg msg;
  GetOwner ()->TraceMessageReceive (GetCallId (), pkt, msg.GetInstanceTypeId ());

  pkt->RemoveHeader (msg);

  Receive (msg);
}

Ptr<McpttChannel>
McpttCall::GetCallChannel (void) const
{
  return GetOwner ()->GetCallChannel (m_callPort);
}

Ptr<McpttCallMachine>
McpttCall::GetCallMachine (void) const
{
  return m_callMachine;
}

Ptr<McpttChannel>
McpttCall::GetFloorChannel (void) const
{
  return m_floorChannel;
}

Ptr<McpttFloorParticipant>
McpttCall::GetFloorMachine (void) const
{
  return m_floorMachine;
}

Ptr<McpttChannel>
McpttCall::GetMediaChannel (void) const
{
  return m_mediaChannel;
}

Ptr<McpttPttApp>
McpttCall::GetOwner (void) const
{
  return m_owner;
}

Time
McpttCall::GetStartTime (void) const
{
  return m_startTime;
}

Time
McpttCall::GetStopTime (void) const
{
  return m_stopTime;
}

void
McpttCall::SetCallMachine (Ptr<McpttCallMachine>  callMachine)
{
  NS_LOG_FUNCTION (this << &callMachine);

  if (callMachine != 0)
    {
      callMachine->SetCall (this);
    }

  m_callMachine = callMachine;
}

void
McpttCall::SetFloorChannel (Ptr<McpttChannel>  floorChannel)
{
  NS_LOG_FUNCTION (this << &floorChannel);

  if (floorChannel != 0)
    {
      floorChannel->SetRxPktCb (MakeCallback (&McpttCall::ReceiveFloorPkt, this));
    }

  m_floorChannel = floorChannel;
}

void
McpttCall::SetFloorMachine (Ptr<McpttFloorParticipant>  floorMachine)
{
  NS_LOG_FUNCTION (this << &floorMachine);

  if (floorMachine != 0)
    {
      floorMachine->SetCall (this);
    }

  m_floorMachine = floorMachine;
}

void
McpttCall::SetMediaChannel (Ptr<McpttChannel>  mediaChannel)
{
  NS_LOG_FUNCTION (this << &mediaChannel);

  if (mediaChannel != 0)
    {
      mediaChannel->SetRxPktCb (MakeCallback (&McpttCall::ReceiveMediaPkt, this));
    }

  m_mediaChannel = mediaChannel;
}

void
McpttCall::SetOwner (Ptr<McpttPttApp> owner)
{
  NS_LOG_FUNCTION (this << owner);

  m_owner = owner;
}

void
McpttCall::SetStartTime (Time startTime)
{
  NS_LOG_FUNCTION (this << startTime);
  m_startTime = startTime;
}

void
McpttCall::SetStopTime (Time stopTime)
{
  NS_LOG_FUNCTION (this << stopTime);
  m_stopTime = stopTime;
}

std::ostream& operator << (std::ostream& os, const McpttCall::NetworkCallType& obj)
{
  os << static_cast<std::underlying_type<McpttCall::NetworkCallType>::type> (obj);
  return os;
}

} // namespace psc

} // namespace ns3

