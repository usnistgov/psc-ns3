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

#include "mcptt-call-machine.h"
#include "mcptt-call-msg.h"
#include "mcptt-chan.h"
#include "mcptt-floor-participant.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-on-network-call-machine-client.h"
#include "mcptt-ptt-app.h"

#include "mcptt-call.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("McpttCall");

NS_OBJECT_ENSURE_REGISTERED (McpttCall);

TypeId
McpttCall::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttCall")
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
   ;
  return tid;
}

McpttCall::McpttCall (void)
  : Object (),
    m_networkCallType (NetworkCallType::INVALID), // type must be set later
    m_floorChan (0),
    m_mediaChan (0),
    m_owner (0),
    m_pushOnSelect (false),
    m_startTime (Seconds (0)),
    m_stopTime (Seconds (0)),
    m_rxCb (MakeNullCallback<void, Ptr<const McpttCall>, const Header&> ()),
    m_txCb (MakeNullCallback<void, Ptr<const McpttCall>, const Header&> ())
{
  NS_LOG_FUNCTION (this);
}

McpttCall::McpttCall (NetworkCallType callType)
  : Object (),
    m_networkCallType (callType),
    m_floorChan (0),
    m_mediaChan (0),
    m_owner (0),
    m_pushOnSelect (false),
    m_startTime (Seconds (0)),
    m_stopTime (Seconds (0)),
    m_rxCb (MakeNullCallback<void, Ptr<const McpttCall>, const Header&> ()),
    m_txCb (MakeNullCallback<void, Ptr<const McpttCall>, const Header&> ())
{
  NS_LOG_FUNCTION (this);
}

McpttCall::~McpttCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttCall::CloseFloorChan (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttChan> floorChan = GetFloorChan ();
  floorChan->Close ();
}

void
McpttCall::CloseMediaChan (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttChan> mediaChan = GetMediaChan ();
  mediaChan->Close ();
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
McpttCall::IsFloorChanOpen (void) const
{
  Ptr<McpttChan> floorChan = GetFloorChan ();
  bool isOpen = floorChan->IsOpen ();

  return isOpen;
}

bool
McpttCall::IsMediaChanOpen (void) const
{
  Ptr<McpttChan> mediaChan = GetMediaChan ();
  bool isOpen = mediaChan->IsOpen ();

  return isOpen;
}

void
McpttCall::OpenFloorChan (const Address& peerAddr, const uint16_t port)
{
  NS_LOG_FUNCTION (this << peerAddr << port);

  Ptr<McpttPttApp> owner = GetOwner ();
  Ptr<Node> node = owner->GetNode ();
  Ptr<McpttChan> floorChan = GetFloorChan ();
  Address localAddr = owner->GetLocalAddress ();

  int result = floorChan->Open (node, port, localAddr, peerAddr);
  NS_ABORT_MSG_UNLESS (result == 0, "Unable to open floor channel on node " << GetOwner ()->GetNode ()->GetId ());
}

void
McpttCall::OpenMediaChan (const Address& peerAddr, const uint16_t port)
{
  NS_LOG_FUNCTION (this << peerAddr << port);

  Ptr<McpttPttApp> owner = GetOwner ();
  Ptr<Node> node = owner->GetNode ();
  Ptr<McpttChan> mediaChan = GetMediaChan ();
  Address localAddr = owner->GetLocalAddress ();

  int result = mediaChan->Open (node, port, localAddr, peerAddr);
  NS_ABORT_MSG_UNLESS (result == 0, "Unable to open media channel on node " << GetOwner ()->GetNode ()->GetId ());
}

void
McpttCall::Receive (Ptr<Packet> pkt, const SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << pkt);
  if (hdr.GetCallId () != GetCallId ())
    {
      NS_LOG_DEBUG ("Discarding SIP packet for call ID " << hdr.GetCallId ());
      return;
    }
  NS_LOG_DEBUG ("Received SIP packet for call ID " << GetCallId ());
  if (!m_rxCb.IsNull ())
    {
      m_rxCb (this, hdr);
    }
  GetCallMachine ()->GetObject<McpttOnNetworkCallMachineClient> ()->ReceiveCallPacket (pkt, hdr);
}

void
McpttCall::Receive (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << & msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (this, msg);
    }

  Ptr<McpttCallMachine> callMachine = GetCallMachine ();
  callMachine->Receive (msg);
}

void
McpttCall::Receive (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (this, msg);
    }

  Ptr<McpttFloorParticipant> floorMachine = GetFloorMachine ();
  floorMachine->Receive (msg);
}

void
McpttCall::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (this, msg);
    }

  Ptr<McpttCallMachine> callMachine = GetCallMachine ();
  Ptr<McpttFloorParticipant> floorMachine = GetFloorMachine ();

  callMachine->Receive (msg);
  floorMachine->Receive (msg);
}

// on-network sends SIP-based messages
void
McpttCall::Send (Ptr<Packet> pkt, const SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << pkt);
  NS_LOG_LOGIC ("PttApp sending " << hdr << ".");
  if (Ipv4Address::IsMatchingType (m_peerAddress))
    {
      Ipv4Address peer = Ipv4Address::ConvertFrom (m_peerAddress);
      GetCallChan ()->SendTo (pkt, 0, InetSocketAddress (peer, m_callPort));
      if (!m_txCb.IsNull ())
        {
          m_txCb (this, hdr);
        }
    }
  else if (Ipv6Address::IsMatchingType (m_peerAddress))
    {
      Ipv6Address peer = Ipv6Address::ConvertFrom (m_peerAddress);
      GetCallChan ()->SendTo (pkt, 0, Inet6SocketAddress (peer, m_callPort));
      if (!m_txCb.IsNull ())
        {
          m_txCb (this, hdr);
        }
    }
}

// off-network directly sends McpttCallMsg
void
McpttCall::Send (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<Packet> pkt = Create<Packet> ();

  pkt->AddHeader (msg);

  NS_LOG_LOGIC ("PttApp sending " << msg << ".");

  if (Ipv4Address::IsMatchingType (m_peerAddress))
    {
      Ipv4Address peer = Ipv4Address::ConvertFrom (m_peerAddress);
      GetCallChan ()->SendTo (pkt, 0, InetSocketAddress (peer, m_callPort));
      if (!m_txCb.IsNull ())
        {
          m_txCb (this, msg);
        }
    }
  else if (Ipv6Address::IsMatchingType (m_peerAddress))
    {
      Ipv6Address peer = Ipv6Address::ConvertFrom (m_peerAddress);
      GetCallChan ()->SendTo (pkt, 0, Inet6SocketAddress (peer, m_callPort));
      if (!m_txCb.IsNull ())
        {
          m_txCb (this, msg);
        }
    }
}

void
McpttCall::Send (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!m_txCb.IsNull ())
    {
      m_txCb (this, msg);
    }

  Ptr<Packet> pkt = Create<Packet> ();
  Ptr<McpttChan> floorChan = GetFloorChan ();

  pkt->AddHeader (msg);

  floorChan->Send (pkt);
}

void
McpttCall::Send (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<Packet> pkt = Create<Packet> ();
  Ptr<McpttChan> mediaChan = GetMediaChan ();
  Ptr<McpttFloorParticipant> floorMachine = GetFloorMachine ();

  McpttMediaMsg txMsg (msg);

  floorMachine->MediaReady (txMsg);

  if (!m_txCb.IsNull ())
    {
      m_txCb (this, msg);
    }

  pkt->AddHeader (txMsg);

  mediaChan->Send (pkt);
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

  SetCallMachine (0);
  SetFloorChan (0);
  SetFloorMachine (0);
  SetMediaChan (0);
  SetOwner (0);

  Object::DoDispose ();
}

void
McpttCall::ReceiveCallPkt (Ptr<Packet> pkt)
{
  NS_LOG_FUNCTION (this << pkt);

  NS_LOG_LOGIC ("Call received " << pkt->GetSize () << " byte(s).");

  if (GetCallMachine ()->GetObject<McpttOnNetworkCallMachineClient> ())
    {
      NS_LOG_LOGIC ("Handling on-network call control packet");
      SipHeader sipHeader;
      pkt->PeekHeader (sipHeader);
      if (GetCallId () == sipHeader.GetCallId ())
        {
          NS_LOG_DEBUG ("Received packet for call ID " << GetCallId ());
          pkt->RemoveHeader (sipHeader);
          Receive (pkt, sipHeader);
        }
      else
        {
          NS_LOG_DEBUG ("Call ID " << GetCallId () << " does not match " << sipHeader.GetCallId ());
        }
      return;
    }

  NS_LOG_LOGIC ("Handling off-network call control packet");
  McpttCallMsg temp;
  pkt->PeekHeader (temp);

  McpttCallMsgFieldMsgType msgType = temp.GetMsgType ();
  uint8_t code = msgType.GetType ();

  if (code == McpttCallMsgGrpProbe::CODE)
    {
      McpttCallMsgGrpProbe probeMsg;
      pkt->RemoveHeader (probeMsg);
      Receive (probeMsg);
    }
  else if (code == McpttCallMsgGrpAnnoun::CODE)
    {
      McpttCallMsgGrpAnnoun grpAnnounMsg;
      pkt->RemoveHeader (grpAnnounMsg);
      Receive (grpAnnounMsg);
    }
  else if (code == McpttCallMsgGrpAccept::CODE)
    {
      McpttCallMsgGrpAccept grpAcceptMsg;
      pkt->RemoveHeader (grpAcceptMsg);
      Receive (grpAcceptMsg);
    }
  else if (code == McpttCallMsgGrpImmPerilEnd::CODE)
    {
      McpttCallMsgGrpImmPerilEnd grpImmPerilEndMsg;
      pkt->RemoveHeader (grpImmPerilEndMsg);
      Receive (grpImmPerilEndMsg);
    }
  else if (code == McpttCallMsgGrpEmergEnd::CODE)
    {
      McpttCallMsgGrpEmergEnd grpEmergEndMsg;
      pkt->RemoveHeader (grpEmergEndMsg);
      Receive (grpEmergEndMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlert::CODE)
    {
      McpttCallMsgGrpEmergAlert grpEmergAlertMsg;
      pkt->RemoveHeader (grpEmergAlertMsg);
      Receive (grpEmergAlertMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlertAck::CODE)
    {
      McpttCallMsgGrpEmergAlertAck grpEmergAlertAckMsg;
      pkt->RemoveHeader (grpEmergAlertAckMsg);
      Receive (grpEmergAlertAckMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlertCancel::CODE)
    {
      McpttCallMsgGrpEmergAlertCancel grpEmergAlertCancelMsg;
      pkt->RemoveHeader (grpEmergAlertCancelMsg);
      Receive (grpEmergAlertCancelMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlertCancelAck::CODE)
    {
      McpttCallMsgGrpEmergAlertCancelAck emergAlertCancelAckMsg;
      pkt->RemoveHeader (emergAlertCancelAckMsg);
      Receive (emergAlertCancelAckMsg);
    }
  else if (code == McpttCallMsgGrpBroadcast::CODE)
    {
      McpttCallMsgGrpBroadcast grpBroadcastMsg;
      pkt->RemoveHeader (grpBroadcastMsg);
      Receive (grpBroadcastMsg);
    }
  else if (code == McpttCallMsgGrpBroadcastEnd::CODE)
    {
      McpttCallMsgGrpBroadcastEnd grpBroadcastEndMsg;
      pkt->RemoveHeader (grpBroadcastEndMsg);
      Receive (grpBroadcastEndMsg);
    }
  else if (code == McpttCallMsgPrivateSetupReq::CODE)
    {
      McpttCallMsgPrivateSetupReq privateSetupReqMsg;
      pkt->RemoveHeader (privateSetupReqMsg);
      Receive (privateSetupReqMsg);
    }
  else if (code == McpttCallMsgPrivateRinging::CODE)
    {
      McpttCallMsgPrivateRinging privateRingingMsg;
      pkt->RemoveHeader (privateRingingMsg);
      Receive (privateRingingMsg);
    }
  else if (code == McpttCallMsgPrivateAccept::CODE)
    {
      McpttCallMsgPrivateAccept privateAcceptMsg;
      pkt->RemoveHeader (privateAcceptMsg);
      Receive (privateAcceptMsg);
    }
  else if (code == McpttCallMsgPrivateReject::CODE)
    {
      McpttCallMsgPrivateReject privateRejectMsg;
      pkt->RemoveHeader (privateRejectMsg);
      Receive (privateRejectMsg);
    }
  else if (code == McpttCallMsgPrivateRelease::CODE)
    {
      McpttCallMsgPrivateRelease privateReleaseMsg;
      pkt->RemoveHeader (privateReleaseMsg);
      Receive (privateReleaseMsg);
    }
  else if (code == McpttCallMsgPrivateReleaseAck::CODE)
    {
      McpttCallMsgPrivateReleaseAck privateReleaseAckMsg;
      pkt->RemoveHeader (privateReleaseAckMsg);
      Receive (privateReleaseAckMsg);
    }
  else if (code == McpttCallMsgPrivateAcceptAck::CODE)
    {
      McpttCallMsgPrivateAcceptAck privateAcceptAckMsg;
      pkt->RemoveHeader (privateAcceptAckMsg);
      Receive (privateAcceptAckMsg);
    }
  else if (code == McpttCallMsgPrivateEmergCancel::CODE)
    {
      McpttCallMsgPrivateEmergCancel privateEmergCancelMsg;
      pkt->RemoveHeader (privateEmergCancelMsg);
      Receive (privateEmergCancelMsg);
    }
  else if (code == McpttCallMsgPrivateEmergCancelAck::CODE)
    {
      McpttCallMsgPrivateEmergCancelAck privateEmergCancelAckMsg;
      pkt->RemoveHeader (privateEmergCancelAckMsg);
      Receive (privateEmergCancelAckMsg);
    }
  else
    {
      NS_FATAL_ERROR ("Could not resolve message code = " << (uint32_t)code << ".");
    }
}

void
McpttCall::ReceiveFloorPkt (Ptr<Packet>  pkt)
{
  NS_LOG_FUNCTION (this << &pkt);

  McpttFloorMsg temp;

  pkt->PeekHeader (temp);
  uint8_t subtype = temp.GetSubtype ();

  if (subtype == McpttFloorMsgRequest::SUBTYPE)
    {
      McpttFloorMsgRequest reqMsg;
      pkt->RemoveHeader (reqMsg);
      Receive (reqMsg);
    }
  else if (subtype == McpttFloorMsgGranted::SUBTYPE
      || subtype == McpttFloorMsgGranted::SUBTYPE_ACK)
    {
      McpttFloorMsgGranted grantedMsg;
      pkt->RemoveHeader (grantedMsg);
      Receive (grantedMsg);
    }
  else if (subtype == McpttFloorMsgDeny::SUBTYPE
      || subtype == McpttFloorMsgDeny::SUBTYPE_ACK)
    {
      McpttFloorMsgDeny denyMsg;
      pkt->RemoveHeader (denyMsg);
      Receive (denyMsg);
    }
  else if (subtype == McpttFloorMsgRelease::SUBTYPE
      || subtype == McpttFloorMsgRelease::SUBTYPE_ACK)
    {
      McpttFloorMsgRelease releaseMsg;
      pkt->RemoveHeader (releaseMsg);
      Receive (releaseMsg);
    }
  else if (subtype == McpttFloorMsgIdle::SUBTYPE
      || subtype == McpttFloorMsgIdle::SUBTYPE_ACK)
    {
      McpttFloorMsgIdle idleMsg;
      pkt->RemoveHeader (idleMsg);
      Receive (idleMsg);
    }
  else if (subtype == McpttFloorMsgTaken::SUBTYPE
      || subtype == McpttFloorMsgTaken::SUBTYPE_ACK)
    {
      McpttFloorMsgTaken takenMsg;
      pkt->RemoveHeader (takenMsg);
      Receive (takenMsg);
    }
  else if (subtype == McpttFloorMsgRevoke::SUBTYPE)
    {
      McpttFloorMsgRevoke revokeMsg;
      pkt->RemoveHeader (revokeMsg);
      Receive (revokeMsg);
    }
  else if (subtype == McpttFloorMsgQueuePositionRequest::SUBTYPE)
    {
      McpttFloorMsgQueuePositionRequest queuePositionRequestMsg;
      pkt->RemoveHeader (queuePositionRequestMsg);
      Receive (queuePositionRequestMsg);
    }
  else if (subtype == McpttFloorMsgQueuePositionInfo::SUBTYPE
      || subtype == McpttFloorMsgQueuePositionInfo::SUBTYPE_ACK)
    {
      McpttFloorMsgQueuePositionInfo queueInfoMsg;
      pkt->RemoveHeader (queueInfoMsg);
      Receive (queueInfoMsg);
    }
  else if (subtype == McpttFloorMsgAck::SUBTYPE)
    {
      McpttFloorMsgAck ackMsg;
      pkt->RemoveHeader (ackMsg);
      Receive (ackMsg);
    }
  else
    {
      NS_FATAL_ERROR ("Could not resolve message subtype = " << (uint32_t)subtype << ".");
    }
}

void
McpttCall::ReceiveMediaPkt (Ptr<Packet>  pkt)
{
  NS_LOG_FUNCTION (this << &pkt);

  McpttMediaMsg msg;

  pkt->RemoveHeader (msg);

  Receive (msg);
}

Ptr<McpttChan>
McpttCall::GetCallChan (void) const
{
  return GetOwner ()->GetCallChannel (m_callPort);
}

Ptr<McpttCallMachine>
McpttCall::GetCallMachine (void) const
{
  return m_callMachine;
}

Ptr<McpttChan>
McpttCall::GetFloorChan (void) const
{
  return m_floorChan;
}

Ptr<McpttFloorParticipant>
McpttCall::GetFloorMachine (void) const
{
  return m_floorMachine;
}

Ptr<McpttChan>
McpttCall::GetMediaChan (void) const
{
  return m_mediaChan;
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
McpttCall::SetFloorChan (Ptr<McpttChan>  floorChan)
{
  NS_LOG_FUNCTION (this << &floorChan);

  if (floorChan != 0)
    {
      floorChan->SetRxPktCb (MakeCallback (&McpttCall::ReceiveFloorPkt, this));
    }

  m_floorChan = floorChan;
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
McpttCall::SetMediaChan (Ptr<McpttChan>  mediaChan)
{
  NS_LOG_FUNCTION (this << &mediaChan);

  if (mediaChan != 0)
    {
      mediaChan->SetRxPktCb (MakeCallback (&McpttCall::ReceiveMediaPkt, this));
    }

  m_mediaChan = mediaChan;
}

void
McpttCall::SetOwner (Ptr<McpttPttApp> owner)
{
  NS_LOG_FUNCTION (this << owner);

  m_owner = owner;
}

void
McpttCall::SetRxCb (const Callback<void, Ptr<const McpttCall>, const Header&>  rxCb)
{
  NS_LOG_FUNCTION (this);

  m_rxCb = rxCb;
}

void
McpttCall::SetTxCb (const Callback<void, Ptr<const McpttCall>, const Header&>  txCb)
{
  NS_LOG_FUNCTION (this << &txCb);

  m_txCb = txCb;
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

} // namespace ns3

