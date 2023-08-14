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

#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/type-id.h>
#include <ns3/uinteger.h>
#include <ns3/vector.h>
#include <ns3/sip-header.h>
#include <ns3/string.h>
#include <ns3/pointer.h>
#include <ns3/random-variable-stream.h>
#include "mcptt-server-app.h"
#include "mcptt-server-call.h"
#include "mcptt-on-network-floor-arbitrator.h"
#include "mcptt-on-network-floor-participant.h"
#include "mcptt-on-network-floor-towards-participant.h"

#include "mcptt-server-call-machine-group-prearranged.h"
#include "mcptt-server-call-machine-group-prearranged-state.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttServerCallMachineGroupPrearranged");

namespace psc {

/** McpttServerCallMachine - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttServerCallMachineGroupPrearranged);

TypeId
McpttServerCallMachineGroupPrearranged::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttServerCallMachineGroupPrearranged")
    .SetParent<McpttServerCallMachineGrp> ()
    .AddConstructor<McpttServerCallMachineGroupPrearranged> ()
    .AddAttribute ("FloorPort", "The port to use for floor control messages.",
                   UintegerValue (49150),
                   MakeUintegerAccessor (&McpttServerCallMachineGroupPrearranged::m_floorPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("MediaPort", "The port to use for media messages.",
                   UintegerValue (49151),
                   MakeUintegerAccessor (&McpttServerCallMachineGroupPrearranged::m_mediaPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("CallType", "The call type to use for this call machine.",
                   UintegerValue (McpttCallMsgFieldCallType::BASIC_GROUP),
                   MakeUintegerAccessor (&McpttServerCallMachineGroupPrearranged::m_callType),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("InvitePayloadSize",
                   "Random variable to fill out the INVITE packet to a typical size",
                   // Nemergent 1750 bytes on wire, minus headers
                   StringValue ("ns3::ConstantRandomVariable[Constant=1700]"),
                   MakePointerAccessor (&McpttServerCallMachineGroupPrearranged::m_invitePayloadSize),
                   MakePointerChecker<RandomVariableStream> ())
    .AddAttribute ("ByePayloadSize",
                   "Random variable to fill out the BYE packet to a typical size",
                   // Nemergent 868 bytes on wire, minus headers
                   StringValue ("ns3::ConstantRandomVariable[Constant=800]"),
                   MakePointerAccessor (&McpttServerCallMachineGroupPrearranged::m_byePayloadSize),
                   MakePointerChecker<RandomVariableStream> ())
    .AddAttribute ("ResponsePayloadSize",
                   "Random variable to fill out the 200 OK packet to a typical size",
                   // Nemergent 1305 bytes on wire, minus headers
                   StringValue ("ns3::ConstantRandomVariable[Constant=1250]"),
                   MakePointerAccessor (&McpttServerCallMachineGroupPrearranged::m_responsePayloadSize),
                   MakePointerChecker<RandomVariableStream> ())
    .AddTraceSource ("StateChangeTrace", "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttServerCallMachineGroupPrearranged::m_stateChangeTrace),
                     "ns3::psc::McpttServerCallMachine::StateChangeTracedCallback")
  ;
  return tid;
}

McpttServerCallMachineGroupPrearranged::McpttServerCallMachineGroupPrearranged (void)
  : McpttServerCallMachineGrp (),
    m_serverCall (0),
    m_callId (0),
    m_started (false)
{
  NS_LOG_FUNCTION (this);
  m_state = McpttServerCallMachineGroupPrearrangedStateS1::GetInstance ();
  m_stateChangeCb = MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ();
}

McpttServerCallMachineGroupPrearranged::~McpttServerCallMachineGroupPrearranged (void)
{
  NS_LOG_FUNCTION (this);
  m_pending.clear ();
  m_pending.shrink_to_fit ();
}

void
McpttServerCallMachineGroupPrearranged::SetState (Ptr<McpttServerCallMachineGroupPrearrangedState>  state)
{
  NS_LOG_FUNCTION (this << &state);
  McpttEntityId stateId = state->GetInstanceStateId ();
  McpttEntityId currStateId = m_state->GetInstanceStateId ();
  if (m_started && (stateId != currStateId))
    {
      NS_LOG_LOGIC ("Server moving from state " << *m_state << " to state " << *state << ".");
      m_state = state;
      if (!m_stateChangeCb.IsNull ())
        {
          m_stateChangeCb (currStateId, stateId);
        }
      std::string selected = "N/A"; // Selected call indicator not applicable
      m_stateChangeTrace (GetUserId (), m_serverCall->GetCallId (), selected, GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

Ptr<McpttServerCallMachineGroupPrearrangedState>
McpttServerCallMachineGroupPrearranged::GetState (void) const
{
  return m_state;
}

void
McpttServerCallMachineGroupPrearranged::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this << &stateChangeCb);

  m_stateChangeCb = stateChangeCb;
}

void
McpttServerCallMachineGroupPrearranged::SendSipRequest (uint32_t from, uint32_t to, Ptr<Packet> pkt, const sip::SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << from << to << pkt << hdr);
  Ptr<McpttOnNetworkFloorTowardsParticipant> participant = GetServerCall ()->GetArbitrator ()->GetParticipantByUserId (to);
  Ptr<sip::SipProxy> proxy = GetServerCall ()->GetOwner ()->GetSipProxy ();
  NS_ASSERT_MSG (participant, "Participant " << to << " not found");

  Address addr = participant->GetPeerAddress ();
  if (Ipv4Address::IsMatchingType (addr))
    {
      Ipv4Address ipv4Addr = Ipv4Address::ConvertFrom (addr);
      NS_LOG_DEBUG ("IPv4 address for SIP request: " << ipv4Addr);
      UintegerValue portValue;
      GetServerCall ()->GetOwner ()->GetAttribute ("CallPort", portValue);
      uint16_t callPort = portValue.Get ();
      InetSocketAddress inetAddr (ipv4Addr, callPort);
      if (hdr.GetMethod () == sip::SipHeader::INVITE)
        {
          proxy->SendInvite (pkt, inetAddr, hdr.GetRequestUri (),
                             from, to, hdr.GetCallId (),
                             MakeCallback (&McpttServerCall::SendCallControlPacket, GetServerCall ()));
        }
      else if (hdr.GetMethod () == sip::SipHeader::BYE)
        {
          proxy->SendBye (pkt, inetAddr, hdr.GetRequestUri (),
                          from, to, hdr.GetCallId (),
                          MakeCallback (&McpttServerCall::SendCallControlPacket, GetServerCall ()));
        }
      else
        {
          NS_FATAL_ERROR ("Unsupported SIP method type");
        }
    }
  else if (Ipv6Address::IsMatchingType (addr))
    {
      Ipv6Address ipv6Addr = Ipv6Address::ConvertFrom (addr);
      NS_LOG_DEBUG ("IPv6 address for SIP request: " << ipv6Addr);
      UintegerValue portValue;
      GetServerCall ()->GetOwner ()->GetAttribute ("CallPort", portValue);
      uint16_t callPort = portValue.Get ();
      Inet6SocketAddress inet6Addr (ipv6Addr, callPort);
      if (hdr.GetMethod () == sip::SipHeader::INVITE)
        {
          proxy->SendInvite (pkt, inet6Addr, hdr.GetRequestUri (),
                             from, to, hdr.GetCallId (),
                             MakeCallback (&McpttServerCall::SendCallControlPacket, GetServerCall ()));
        }
      else if (hdr.GetMethod () == sip::SipHeader::BYE)
        {
          proxy->SendBye (pkt, inet6Addr, hdr.GetRequestUri (),
                          from, to, hdr.GetCallId (),
                          MakeCallback (&McpttServerCall::SendCallControlPacket, GetServerCall ()));
        }
      else
        {
          NS_FATAL_ERROR ("Unsupported SIP method type");
        }
    }
  else
    {
      NS_FATAL_ERROR ("Address support not implemented");
    }
}

void
McpttServerCallMachineGroupPrearranged::SendSipResponse (uint32_t from, uint32_t to, Ptr<Packet> pkt, uint16_t statusCode, const sip::SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << from << to << pkt << statusCode << hdr);
  bool found = false;
  Ptr<sip::SipProxy> proxy = GetServerCall ()->GetOwner ()->GetSipProxy ();
  for (uint32_t i = 0; i < GetServerCall ()->GetArbitrator ()->GetNParticipants (); i++)
    {
      Ptr<McpttOnNetworkFloorTowardsParticipant> participant = GetServerCall ()->GetArbitrator ()->GetParticipant (i);
      // In SIP, if a response is needed, it means that this agent is
      // in a UAS (server) role, and the 'from' field will be the originating
      // (UAC) ID, and the 'to' field will be this agent's ID.
      if (participant->GetPeerUserId () == from)
        {
          Address addr = participant->GetPeerAddress ();
          if (Ipv4Address::IsMatchingType (addr))
            {
              Ipv4Address ipv4Addr = Ipv4Address::ConvertFrom (addr);
              UintegerValue portValue;
              GetServerCall ()->GetOwner ()->GetAttribute ("CallPort", portValue);
              uint16_t callPort = portValue.Get ();
              InetSocketAddress inetAddr (ipv4Addr, callPort);
              proxy->SendResponse (pkt, inetAddr, statusCode,
                                   from, to, hdr.GetCallId (),
                                   MakeCallback (&McpttServerCall::SendCallControlPacket, GetServerCall ()));
              found = true;
            }
          else if (Ipv6Address::IsMatchingType (addr))
            {
              Ipv6Address ipv6Addr = Ipv6Address::ConvertFrom (addr);
              UintegerValue portValue;
              GetServerCall ()->GetOwner ()->GetAttribute ("CallPort", portValue);
              uint16_t callPort = portValue.Get ();
              Inet6SocketAddress inet6Addr (ipv6Addr, callPort);
              proxy->SendResponse (pkt, inet6Addr, statusCode,
                                   from, to, hdr.GetCallId (),
                                   MakeCallback (&McpttServerCall::SendCallControlPacket, GetServerCall ()));
              found = true;
            }
          else
            {
              NS_FATAL_ERROR ("Address support not implemented");
            }
        }
    }
  NS_ASSERT_MSG (found, "Group member not found");
}

void
McpttServerCallMachineGroupPrearranged::AcceptCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttServerCallMachineGroupPrearranged::BeginEmergAlert (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttServerCallMachineGroupPrearranged::CancelEmergAlert (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttServerCallMachineGroupPrearranged::DowngradeCallType (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttServerCallMachineGroupPrearranged::GetCallerUserId (void) const
{
  NS_LOG_FUNCTION (this);

  return 0;
}

McpttCallMsgFieldCallId
McpttServerCallMachineGroupPrearranged::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldCallType
McpttServerCallMachineGroupPrearranged::GetCallType (void) const
{
  McpttCallMsgFieldCallType callTypeField;
  callTypeField.SetType (m_callType);

  return callTypeField;
}

TypeId
McpttServerCallMachineGroupPrearranged::GetInstanceTypeId (void) const
{
  return McpttServerCallMachineGroupPrearranged::GetTypeId ();
}

Ptr<McpttServerCall>
McpttServerCallMachineGroupPrearranged::GetServerCall (void) const
{
  return m_serverCall;
}

McpttEntityId
McpttServerCallMachineGroupPrearranged::GetStateId (void) const
{
  NS_LOG_FUNCTION (this);
  return m_state->GetInstanceStateId ();
}

bool
McpttServerCallMachineGroupPrearranged::IsCallOngoing (void) const
{
  if (GetStateId () != McpttServerCallMachineGroupPrearrangedStateS1::GetInstance ()->GetStateId ())
    {
      return true;
    }
  return false;
}

bool
McpttServerCallMachineGroupPrearranged::IsGrpCall (uint32_t grpId) const
{
  NS_LOG_FUNCTION (this << grpId);

  return false;
}

bool
McpttServerCallMachineGroupPrearranged::IsPrivateCall (uint32_t userId) const
{
  NS_LOG_FUNCTION (this << userId);

  return false;
}

void
McpttServerCallMachineGroupPrearranged::ReceiveCallPacket (Ptr<Packet> pkt, const sip::SipHeader& sipHeader)
{
  NS_LOG_FUNCTION (this << pkt << sipHeader);
  uint16_t callId = sipHeader.GetCallId ();
  NS_ASSERT_MSG (callId == m_serverCall->GetCallId (), "mismatch of call ID");
  if (sipHeader.GetMessageType () == sip::SipHeader::SIP_REQUEST)
    {
      if (sipHeader.GetMethod () == sip::SipHeader::INVITE)
        {
          m_state->ReceiveInvite (*this, pkt, sipHeader);
        }
      else if (sipHeader.GetMethod () == sip::SipHeader::BYE)
        {
          m_state->ReceiveBye (*this, pkt, sipHeader);
        }
    }
  else if (sipHeader.GetMessageType () == sip::SipHeader::SIP_RESPONSE)
    {
      m_state->ReceiveResponse (*this, pkt, sipHeader);
    }
}

void
McpttServerCallMachineGroupPrearranged::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);
}

void
McpttServerCallMachineGroupPrearranged::RejectCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttServerCallMachineGroupPrearranged::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_DEBUG (this << callId);
  m_callId = callId.GetCallId ();
}

void
McpttServerCallMachineGroupPrearranged::SetNewCallCb (const Callback<void, uint16_t>  newCallCb)
{
  NS_LOG_FUNCTION (this << &newCallCb);
}

void
McpttServerCallMachineGroupPrearranged::SetServerCall (Ptr<McpttServerCall> call)
{
  NS_LOG_FUNCTION (this << call);
  m_serverCall = call;
}

void
McpttServerCallMachineGroupPrearranged::Start (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttServerCall> call = GetServerCall ();
  Ptr<McpttServerApp> serverApp = call->GetOwner ();

  Ptr<McpttOnNetworkFloorArbitrator> arbitrator = call->GetArbitrator ();
  arbitrator->Start ();
  m_started = true;
  SetState (McpttServerCallMachineGroupPrearrangedStateS1::GetInstance ());
}

void
McpttServerCallMachineGroupPrearranged::Stop (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttServerCall> call = GetServerCall ();
  Ptr<McpttOnNetworkFloorArbitrator> arbitrator = call->GetArbitrator ();

  arbitrator->Stop ();
  m_started = false;
  SetState (McpttServerCallMachineGroupPrearrangedStateS1::GetInstance ());
}

void
McpttServerCallMachineGroupPrearranged::UpgradeCallType (uint8_t callType)
{
  NS_LOG_FUNCTION (this << (uint32_t)callType);
  m_callType = callType;
}

uint32_t
McpttServerCallMachineGroupPrearranged::GetInvitePayloadSize (void)
{
  double value = m_invitePayloadSize->GetValue ();
  NS_ASSERT_MSG (value > 0, "Invalid random variate");
  return (static_cast<uint32_t> (value));
}

uint32_t
McpttServerCallMachineGroupPrearranged::GetByePayloadSize (void)
{
  double value = m_byePayloadSize->GetValue ();
  NS_ASSERT_MSG (value > 0, "Invalid random variate");
  return (static_cast<uint32_t> (value));
}

uint32_t
McpttServerCallMachineGroupPrearranged::GetResponsePayloadSize (void)
{
  double value = m_responsePayloadSize->GetValue ();
  NS_ASSERT_MSG (value > 0, "Invalid random variate");
  return (static_cast<uint32_t> (value));
}

int64_t
McpttServerCallMachineGroupPrearranged::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  int64_t streamsUsed = 0;
  m_invitePayloadSize->SetStream (stream + streamsUsed);
  streamsUsed++;
  m_responsePayloadSize->SetStream (stream + streamsUsed);
  streamsUsed++;
  m_byePayloadSize->SetStream (stream + streamsUsed);
  streamsUsed++;
  return streamsUsed;
}

void
McpttServerCallMachineGroupPrearranged::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_serverCall = 0;
  m_state = 0;
  m_stateChangeCb = MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ();
  McpttServerCallMachine::DoDispose ();
}

void
McpttServerCallMachineGroupPrearranged::SetGrpId (uint32_t grpId)
{
  NS_LOG_FUNCTION (this << grpId);
  McpttCallMsgFieldGrpId grpIdField;
  grpIdField.SetGrpId (grpId);
  m_grpId = grpIdField;
}

void
McpttServerCallMachineGroupPrearranged::SetUserId (uint32_t userId)
{
  NS_LOG_FUNCTION (this << userId);
  m_userId = userId;
}

McpttCallMsgFieldGrpId
McpttServerCallMachineGroupPrearranged::GetGrpId (void) const
{
  return m_grpId;
}

uint32_t
McpttServerCallMachineGroupPrearranged::GetUserId (void) const
{
  return m_userId;
}


void
McpttServerCallMachineGroupPrearranged::SetPendingTransactionList (std::vector<uint32_t> pending)
{
  NS_LOG_FUNCTION (this);
  if (m_pending.size ())
    {
      NS_LOG_DEBUG ("Replacing existing pending transaction list with size " << m_pending.size ());
      m_pending.clear ();
      m_pending.shrink_to_fit ();
    }
  m_pending = pending;
}

bool
McpttServerCallMachineGroupPrearranged::RemoveFromPending (uint32_t userId)
{
  NS_LOG_FUNCTION (this << userId);
  bool found = false;
  for (auto it = m_pending.begin (); it != m_pending.end (); it++)
    {
      if (*it == userId)
        {
          NS_LOG_DEBUG ("Found entry for id " << userId);
          m_pending.erase (it);
          found = true;
          break;
        }
    }
  return found;
}

uint32_t
McpttServerCallMachineGroupPrearranged::GetNPendingTransactions (void) const
{
  return m_pending.size ();
}

} // namespace psc
} // namespace ns3

