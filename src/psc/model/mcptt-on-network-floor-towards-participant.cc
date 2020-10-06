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

#include <limits>
#include <ns3/boolean.h>
#include <ns3/ipv4-address.h>
#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/simulator.h>
#include <ns3/type-id.h>
#include <ns3/uinteger.h>

#include "mcptt-server-call.h"
#include "mcptt-server-call-machine.h"
#include "mcptt-channel.h"
#include "mcptt-counter.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-on-network-floor-arbitrator.h"
#include "mcptt-server-app.h"
#include "mcptt-on-network-floor-towards-participant-state.h"
#include "mcptt-timer.h"

#include "mcptt-on-network-floor-towards-participant.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttOnNetworkFloorTowardsParticipant");

/** McpttOnNetworkFloorTowardsParticipant - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttOnNetworkFloorTowardsParticipant);

TypeId
McpttOnNetworkFloorTowardsParticipant::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttOnNetworkFloorTowardsParticipant")
    .SetParent<Object> ()
    .AddConstructor<McpttOnNetworkFloorTowardsParticipant>()
    .AddAttribute ("FloorPort", "The port to use for floor control messages.",
                   UintegerValue (49150),
                   MakeUintegerAccessor (&McpttOnNetworkFloorTowardsParticipant::GetFloorPort,
                                         &McpttOnNetworkFloorTowardsParticipant::SetFloorPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("MediaPort", "The port to use for media messages.",
                   UintegerValue (49151),
                   MakeUintegerAccessor (&McpttOnNetworkFloorTowardsParticipant::GetMediaPort,
                                         &McpttOnNetworkFloorTowardsParticipant::SetMediaPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PeerAddress", "The Ipv4 address of the node that the peer application is on.",
                   AddressValue (Ipv4Address ()),
                   MakeAddressAccessor (&McpttOnNetworkFloorTowardsParticipant::GetPeerAddress,
                                            &McpttOnNetworkFloorTowardsParticipant::SetPeerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("PeerUserId", "The MCPTT User ID of the node that the peer application is on.",
                   UintegerValue (std::numeric_limits<uint32_t>::max ()),
                   MakeUintegerAccessor (&McpttOnNetworkFloorTowardsParticipant::GetPeerUserId,
                                            &McpttOnNetworkFloorTowardsParticipant::SetPeerUserId),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("ReceiveOnly", "Indicates if the associated participant is \"receive only\"",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttOnNetworkFloorTowardsParticipant::m_receiveOnly),
                   MakeBooleanChecker ())
    .AddAttribute ("T8", "The delay to use for timer T8 (Time value)",
                   TimeValue (Seconds (1)),
                   MakeTimeAccessor (&McpttOnNetworkFloorTowardsParticipant::SetDelayT8),
                   MakeTimeChecker ())
    .AddTraceSource ("StateChangeTrace",
                   "The trace for capturing state changes.",
                   MakeTraceSourceAccessor (&McpttOnNetworkFloorTowardsParticipant::m_stateChangeTrace),
                   "ns3::McpttOnNetworkFloorTowardsParticipant::StateChangeTracedCallback")
    ;
  
  return tid;
}

McpttOnNetworkFloorTowardsParticipant::McpttOnNetworkFloorTowardsParticipant (void)
  : Object (),
    m_dualFloor (false),
    m_floorChannel (CreateObject<McpttChannel> ()),
    m_implicitRequest (false),
    m_mediaChannel (CreateObject<McpttChannel> ()),
    m_originator (false),
    m_overridden (false),
    m_overriding (false),
    m_owner (0),
    m_revokeMsg (McpttFloorMsgRevoke ()),
    m_rxCb (MakeNullCallback<void, Ptr<const McpttServerCall>, const Header&> ()),
    m_state (McpttOnNetworkFloorTowardsParticipantStateStartStop::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ()),
    m_t8 (CreateObject<McpttTimer> (McpttEntityId (8, "T8"))),
    m_txCb (MakeNullCallback<void, Ptr<const McpttServerCall>, const Header&> ())
{
  NS_LOG_FUNCTION (this);

  m_floorChannel->SetRxPktCb (MakeCallback (&McpttOnNetworkFloorTowardsParticipant::ReceiveFloorPkt, this));
  m_mediaChannel->SetRxPktCb (MakeCallback (&McpttOnNetworkFloorTowardsParticipant::ReceiveMediaPkt, this));

  m_t8->Link (&McpttOnNetworkFloorTowardsParticipant::ExpiryOfT8, this);
}

McpttOnNetworkFloorTowardsParticipant::~McpttOnNetworkFloorTowardsParticipant (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkFloorTowardsParticipant::CallInitialized (bool implicitRequest)
{
  NS_LOG_FUNCTION (this << implicitRequest);
  NS_LOG_LOGIC ("Taking call initiated notification towards participant " << GetPeerUserId ());
  m_implicitRequest = implicitRequest;
  m_state->CallInitialized (Ptr<McpttOnNetworkFloorTowardsParticipant> (this));
}

void
McpttOnNetworkFloorTowardsParticipant::CallRelease1 (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Taking call release notification (part I).");
  m_state->CallRelease1 (Ptr<McpttOnNetworkFloorTowardsParticipant> (this));
}

void
McpttOnNetworkFloorTowardsParticipant::CallRelease2 (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Taking call release notification (part II).");
  m_state->CallRelease2 (Ptr<McpttOnNetworkFloorTowardsParticipant> (this));
}

void
McpttOnNetworkFloorTowardsParticipant::ClientRelease (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Taking client release notification.");
  m_state->ClientRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> (this));
}

void
McpttOnNetworkFloorTowardsParticipant::ChangeState (Ptr<McpttOnNetworkFloorTowardsParticipantState>  state)
{
  NS_LOG_FUNCTION (this << state);
  McpttEntityId stateId = state->GetInstanceStateId ();
  McpttEntityId currStateId = GetStateId ();
  if (currStateId != stateId)
    {
      NS_LOG_LOGIC ("TowardsParticipant " << GetPeerUserId () << " moving from state " << *m_state << " to state " << *state);
      m_state->Unselected (Ptr<McpttOnNetworkFloorTowardsParticipant> (this));
      SetState (state);
      state->Selected (Ptr<McpttOnNetworkFloorTowardsParticipant> (this));
      if (!m_stateChangeCb.IsNull ())
        {
          m_stateChangeCb (currStateId, stateId);
        }
      std::string selected = "N/A"; // Selected call indicator not applicable
      m_stateChangeTrace (GetOwner ()->GetOwner ()->GetCallMachine ()->GetUserId (), GetOwner ()->GetOwner ()->GetCallId (), selected, GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

void
McpttOnNetworkFloorTowardsParticipant::DoSend (McpttMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);
  Ptr<Packet> pkt = Create<Packet> ();
  pkt->AddHeader (msg);

  if (!m_txCb.IsNull ())
    {
      m_txCb (GetOwner ()->GetOwner (), msg);
    }

  if (msg.IsA (McpttFloorMsg::GetTypeId ()))
    {
      NS_LOG_DEBUG ("Send floor msg towards participant " << GetPeerUserId ());
      GetFloorChannel ()->Send (pkt);
    }
  else if (msg.IsA (McpttMediaMsg::GetTypeId ()))
    {
      NS_LOG_DEBUG ("Send media msg towards participant " << GetPeerUserId ());
      GetMediaChannel ()->Send (pkt);
    }
}

TypeId
McpttOnNetworkFloorTowardsParticipant::GetInstanceTypeId (void) const
{
  return McpttOnNetworkFloorTowardsParticipant::GetTypeId ();
}

McpttEntityId
McpttOnNetworkFloorTowardsParticipant::GetStateId (void) const
{
  McpttEntityId stateId = m_state->GetInstanceStateId ();

  return stateId;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsDualFloor (void) const
{
  return m_dualFloor;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsImplicitRequest (void) const
{
  return m_implicitRequest;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsQueueing (void) const
{
  return m_owner->IsQueueingSupported ();
}

bool
McpttOnNetworkFloorTowardsParticipant::IsOriginator (void) const
{
  return m_originator;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsOverridden (void) const
{
  return m_overridden;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsOverriding (void) const
{
  return m_overriding;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsReceiveOnly (void) const
{
  return m_receiveOnly;
}

bool
McpttOnNetworkFloorTowardsParticipant::IsStarted (void) const
{
  return m_state->GetInstanceStateId () != McpttOnNetworkFloorTowardsParticipant::GetStateId ();
}

void
McpttOnNetworkFloorTowardsParticipant::Receive (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (GetOwner ()->GetOwner (), msg);
    }

  msg.Visit (*this);
}

void
McpttOnNetworkFloorTowardsParticipant::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (!m_rxCb.IsNull ())
    {
      m_rxCb (GetOwner ()->GetOwner (), msg);
    }

  msg.Visit (*this);
}

void
McpttOnNetworkFloorTowardsParticipant::ReceiveFloorQueuePositionRequest (const McpttFloorMsgQueuePositionRequest& msg)
{
  NS_LOG_FUNCTION (this << msg);
  NS_LOG_LOGIC ("Received " << msg.GetInstanceTypeId () << ".");
  m_state->ReceiveFloorQueuePositionRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> (this), msg);
}

void
McpttOnNetworkFloorTowardsParticipant::ReceiveFloorRelease (const McpttFloorMsgRelease& msg)
{
  NS_LOG_FUNCTION (this << msg);
  NS_LOG_LOGIC ("Received " << msg.GetInstanceTypeId () << ".");
  m_state->ReceiveFloorRelease (Ptr<McpttOnNetworkFloorTowardsParticipant> (this), msg);
}

void
McpttOnNetworkFloorTowardsParticipant::ReceiveFloorRequest (const McpttFloorMsgRequest& msg)
{
  NS_LOG_FUNCTION (this << msg);
  NS_LOG_LOGIC ("Received " << msg.GetInstanceTypeId () << ".");
  m_state->ReceiveFloorRequest (Ptr<McpttOnNetworkFloorTowardsParticipant> (this), msg);
}

void
McpttOnNetworkFloorTowardsParticipant::ReceiveMedia (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);
  NS_LOG_LOGIC ("Received " << msg.GetInstanceTypeId () << ".");
  m_state->ReceiveMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> (this), msg);
}

void
McpttOnNetworkFloorTowardsParticipant::Send (McpttMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC ("Send " << msg.GetInstanceTypeId () << ".");
  if (msg.IsA (McpttFloorMsgDeny::GetTypeId ()))
    {
      SendFloorDeny (static_cast<McpttFloorMsgDeny&>(msg));
    }
  else if (msg.IsA (McpttFloorMsgGranted::GetTypeId ()))
    {
      SendFloorGranted (static_cast<McpttFloorMsgGranted&>(msg));
    }
  else if (msg.IsA (McpttFloorMsgIdle::GetTypeId ()))
    {
      SendFloorIdle (static_cast<McpttFloorMsgIdle&>(msg));
    }
  else if (msg.IsA (McpttFloorMsgRevoke::GetTypeId ()))
    {
      SendFloorRevoke (static_cast<McpttFloorMsgRevoke&>(msg));
    }
  else if (msg.IsA (McpttFloorMsgTaken::GetTypeId ()))
    {
      SendFloorTaken (static_cast<McpttFloorMsgTaken&>(msg));
    }
  else if (msg.IsA (McpttMediaMsg::GetTypeId ()))
    {
      SendMedia (static_cast<McpttMediaMsg&>(msg));
    }
  else
    {
      DoSend (msg);
    }
}

void
McpttOnNetworkFloorTowardsParticipant::Terminate (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Taking terminate notification.");

  m_state->Terminate (Ptr<McpttOnNetworkFloorTowardsParticipant> (this));
}

void
McpttOnNetworkFloorTowardsParticipant::SetDelayT8 (const Time& delayT8)
{
  NS_LOG_FUNCTION (this << delayT8);

  GetT8 ()->SetDelay (delayT8);
}

void
McpttOnNetworkFloorTowardsParticipant::Start (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttChannel> floorChannel = GetFloorChannel ();
  Ptr<McpttChannel> mediaChannel = GetMediaChannel ();
  Ptr<McpttServerApp> app = GetOwner ()->GetOwner ()->GetOwner ();

  if (!floorChannel->IsOpen ())
    {
      floorChannel->Open (app->GetNode (), GetFloorPort (), app->GetLocalAddress (), GetPeerAddress ());
    }

  if (!mediaChannel->IsOpen ())
    {
      mediaChannel->Open (app->GetNode (), GetMediaPort (), app->GetLocalAddress (), GetPeerAddress ());
    }
}

void
McpttOnNetworkFloorTowardsParticipant::Stop (void)
{
  NS_LOG_FUNCTION (this);

  if (GetFloorChannel ()->IsOpen ())
    {
      GetFloorChannel ()->Close ();
    }

  if (GetMediaChannel ()->IsOpen ())
    {
      GetMediaChannel ()->Close ();
    }
}

void
McpttOnNetworkFloorTowardsParticipant::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_floorChannel->Dispose ();
  m_floorChannel = 0;
  m_mediaChannel->Dispose ();
  m_mediaChannel = 0;
  m_owner = 0;
  m_state = 0;
  m_t8 = 0;
  m_rxCb = MakeNullCallback<void, Ptr<const McpttServerCall>, const Header&> ();
  m_stateChangeCb = MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ();
  m_txCb = MakeNullCallback<void, Ptr<const McpttServerCall>, const Header&> ();
}

void
McpttOnNetworkFloorTowardsParticipant::ReceiveFloorPkt (Ptr<Packet>  pkt, Address from)
{
  NS_LOG_FUNCTION (this << &pkt << from);

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
  else if (subtype == McpttFloorMsgTaken::SUBTYPE
      || subtype == McpttFloorMsgTaken::SUBTYPE_ACK)
    {
      McpttFloorMsgTaken takenMsg;
      pkt->RemoveHeader (takenMsg);
      Receive (takenMsg);
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
      Receive(ackMsg);
    }
  else
    {
      NS_FATAL_ERROR ("Could not resolve message subtype = " << (uint32_t)subtype << ".");
    }
}

void
McpttOnNetworkFloorTowardsParticipant::ReceiveMediaPkt (Ptr<Packet>  pkt, Address from)
{
  NS_LOG_FUNCTION (this << &pkt << from);

  McpttMediaMsg msg;

  pkt->RemoveHeader (msg);

  Receive (msg);
}

void
McpttOnNetworkFloorTowardsParticipant::ExpiryOfT8 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << ": McpttOnNetworkFloorTowardsParticipant T8 expired.");

  m_state->ExpiryOfT8 (Ptr<McpttOnNetworkFloorTowardsParticipant> (this));
}

void
McpttOnNetworkFloorTowardsParticipant::SendFloorDeny (McpttFloorMsgDeny& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendFloorDeny (Ptr<McpttOnNetworkFloorTowardsParticipant> (this), msg);
}

void
McpttOnNetworkFloorTowardsParticipant::SendFloorGranted (McpttFloorMsgGranted& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendFloorGranted (Ptr<McpttOnNetworkFloorTowardsParticipant> (this), msg);
}

void
McpttOnNetworkFloorTowardsParticipant::SendFloorIdle (McpttFloorMsgIdle& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendFloorIdle (Ptr<McpttOnNetworkFloorTowardsParticipant> (this), msg);
}

void
McpttOnNetworkFloorTowardsParticipant::SendFloorRevoke (McpttFloorMsgRevoke& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendFloorRevoke (Ptr<McpttOnNetworkFloorTowardsParticipant> (this), msg);
}

void
McpttOnNetworkFloorTowardsParticipant::SendFloorTaken (McpttFloorMsgTaken& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendFloorTaken (Ptr<McpttOnNetworkFloorTowardsParticipant> (this), msg);
}

void
McpttOnNetworkFloorTowardsParticipant::SendMedia (McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttOnNetworkFloorTowardsParticipant(" << this << ") sending " << msg.GetInstanceTypeId () << ".");

  m_state->SendMedia (Ptr<McpttOnNetworkFloorTowardsParticipant> (this), msg);
}

Ptr<McpttChannel>
McpttOnNetworkFloorTowardsParticipant::GetFloorChannel (void) const
{
  return m_floorChannel;
}

uint16_t
McpttOnNetworkFloorTowardsParticipant::GetFloorPort (void) const
{
  return m_floorPort;
}

Ptr<McpttChannel>
McpttOnNetworkFloorTowardsParticipant::GetMediaChannel (void) const
{
  return m_mediaChannel;
}

uint16_t
McpttOnNetworkFloorTowardsParticipant::GetMediaPort (void) const
{
  return m_mediaPort;
}

Ptr<McpttOnNetworkFloorArbitrator>
McpttOnNetworkFloorTowardsParticipant::GetOwner (void) const
{
  return m_owner;
}

Address
McpttOnNetworkFloorTowardsParticipant::GetPeerAddress (void) const
{
  return m_peerAddress;
}

uint32_t
McpttOnNetworkFloorTowardsParticipant::GetPeerUserId (void) const
{
  return m_peerUserId;
}

McpttFloorMsgRevoke
McpttOnNetworkFloorTowardsParticipant::GetRevokeMsg (void) const
{
  return m_revokeMsg;
}

uint32_t
McpttOnNetworkFloorTowardsParticipant::GetStoredSsrc (void) const
{
  return m_storedSsrc;
}

uint8_t
McpttOnNetworkFloorTowardsParticipant::GetStoredPriority (void) const
{
  return m_storedPriority;
}

McpttFloorMsgFieldTrackInfo
McpttOnNetworkFloorTowardsParticipant::GetTrackInfo (void) const
{
  return m_trackInfo;
}

Ptr<McpttTimer>
McpttOnNetworkFloorTowardsParticipant::GetT8 (void) const
{
  return m_t8;
}

void
McpttOnNetworkFloorTowardsParticipant::SetDualFloor (const bool dualFloor)
{
  NS_LOG_FUNCTION (this);

  m_dualFloor = dualFloor;
}

void
McpttOnNetworkFloorTowardsParticipant::SetFloorChannel (const Ptr<McpttChannel> floorChannel)
{
  NS_LOG_FUNCTION (this);

  m_floorChannel = floorChannel;
}

void
McpttOnNetworkFloorTowardsParticipant::SetFloorPort (const uint16_t floorPort)
{
  NS_LOG_FUNCTION (this);

  m_floorPort = floorPort;
}

void
McpttOnNetworkFloorTowardsParticipant::SetMediaChannel (const Ptr<McpttChannel> mediaChannel)
{
  NS_LOG_FUNCTION (this);

  m_mediaChannel = mediaChannel;
}

void
McpttOnNetworkFloorTowardsParticipant::SetMediaPort (const uint16_t mediaPort)
{
  NS_LOG_FUNCTION (this);

  m_mediaPort = mediaPort;
}

void
McpttOnNetworkFloorTowardsParticipant::SetOriginator (const bool originator)
{
  NS_LOG_FUNCTION (this);

  m_originator = originator;
}

void
McpttOnNetworkFloorTowardsParticipant::SetOverridden (const bool overridden)
{
  NS_LOG_FUNCTION (this);

  m_overridden = overridden;
}

void
McpttOnNetworkFloorTowardsParticipant::SetOverriding (const bool overriding)
{
  NS_LOG_FUNCTION (this);

  m_overriding = overriding;
}

void
McpttOnNetworkFloorTowardsParticipant::SetOwner (Ptr<McpttOnNetworkFloorArbitrator> owner)
{
  NS_LOG_FUNCTION (this);

  m_owner = owner;
}

void
McpttOnNetworkFloorTowardsParticipant::SetPeerAddress (const Address& peerAddress)
{
  NS_LOG_FUNCTION (this);

  m_peerAddress = peerAddress;
}

void
McpttOnNetworkFloorTowardsParticipant::SetPeerUserId (uint32_t userId)
{
  NS_LOG_FUNCTION (this);
  m_peerUserId = userId;
}

void
McpttOnNetworkFloorTowardsParticipant::SetRevokeMsg (const McpttFloorMsgRevoke& revokeMsg)
{
  NS_LOG_FUNCTION (this);

  m_revokeMsg = revokeMsg;
}

void
McpttOnNetworkFloorTowardsParticipant::SetRxCb (const Callback<void, Ptr<const McpttServerCall>, const Header&>  rxCb)
{
  NS_LOG_FUNCTION (this);

  m_rxCb = rxCb;
}

void
McpttOnNetworkFloorTowardsParticipant::SetState (Ptr<McpttOnNetworkFloorTowardsParticipantState>  state)
{
  NS_LOG_FUNCTION (this << state);

  m_state = state;
}

void
McpttOnNetworkFloorTowardsParticipant::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this);

  m_stateChangeCb = stateChangeCb;
}

void
McpttOnNetworkFloorTowardsParticipant::SetStoredSsrc (const uint32_t storedSsrc)
{
  NS_LOG_FUNCTION (this);

  m_storedSsrc = storedSsrc;
}

void
McpttOnNetworkFloorTowardsParticipant::SetStoredPriority (uint8_t storedPriority)
{
  NS_LOG_FUNCTION (this);

  m_storedPriority = storedPriority;
}

void
McpttOnNetworkFloorTowardsParticipant::SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo)
{
  NS_LOG_FUNCTION (this);

  m_trackInfo = trackInfo;
}

void
McpttOnNetworkFloorTowardsParticipant::SetTxCb (const Callback<void, Ptr<const McpttServerCall>, const Header&>  txCb)
{
  NS_LOG_FUNCTION (this);

  m_txCb = txCb;
}
/** McpttOnNetworkFloorTowardsParticipant - end **/

} // namespace ns3

