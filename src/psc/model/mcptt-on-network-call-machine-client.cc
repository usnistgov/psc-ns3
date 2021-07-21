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
#include <ns3/sip-header.h>
#include <ns3/string.h>
#include <ns3/pointer.h>
#include <ns3/random-variable-stream.h>
#include "mcptt-ptt-app.h"
#include "mcptt-call.h"
#include "mcptt-floor-participant.h"

#include "mcptt-on-network-call-machine-client.h"
#include "mcptt-on-network-call-machine-client-state.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttOnNetworkCallMachineClient");

namespace psc {

/** McpttCallMachine - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttOnNetworkCallMachineClient);

TypeId
McpttOnNetworkCallMachineClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttOnNetworkCallMachineClient")
    .SetParent<McpttCallMachineGrp> ()
    .AddConstructor<McpttOnNetworkCallMachineClient> ()
    .AddAttribute ("FloorPort", "The port to use for floor control messages.",
                   UintegerValue (49150),
                   MakeUintegerAccessor (&McpttOnNetworkCallMachineClient::m_floorPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("MediaPort", "The port to use for media messages.",
                   UintegerValue (49151),
                   MakeUintegerAccessor (&McpttOnNetworkCallMachineClient::m_mediaPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("CallType", "The call type to use for this call machine.",
                   UintegerValue (McpttCallMsgFieldCallType::BASIC_GROUP),
                   MakeUintegerAccessor (&McpttOnNetworkCallMachineClient::m_callType),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("InvitePayloadSize",
                   "Random variable to fill out the INVITE packet to a typical size",
                   // Nemergent 1750 bytes on wire, minus headers
                   StringValue ("ns3::ConstantRandomVariable[Constant=1700]"),
                   MakePointerAccessor (&McpttOnNetworkCallMachineClient::m_invitePayloadSize),
                   MakePointerChecker<RandomVariableStream> ())
    .AddAttribute ("ByePayloadSize",
                   "Random variable to fill out the BYE packet to a typical size",
                   // Nemergent 868 bytes on wire, minus headers
                   StringValue ("ns3::ConstantRandomVariable[Constant=800]"),
                   MakePointerAccessor (&McpttOnNetworkCallMachineClient::m_byePayloadSize),
                   MakePointerChecker<RandomVariableStream> ())
    .AddAttribute ("ResponsePayloadSize",
                   "Random variable to fill out the 200 OK packet to a typical size",
                   // Nemergent 1305 bytes on wire, minus headers
                   StringValue ("ns3::ConstantRandomVariable[Constant=1250]"),
                   MakePointerAccessor (&McpttOnNetworkCallMachineClient::m_responsePayloadSize),
                   MakePointerChecker<RandomVariableStream> ())
    .AddTraceSource ("StateChangeTrace", "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttOnNetworkCallMachineClient::m_stateChangeTrace),
                     "ns3::psc::McpttCallMachine::StateChangeTracedCallback")
  ;
  return tid;
}

McpttOnNetworkCallMachineClient::McpttOnNetworkCallMachineClient (void)
  : McpttCallMachineGrp (),
    m_call (0),
    m_callId (0),
    m_started (false)
{
  NS_LOG_FUNCTION (this);
  m_state = McpttOnNetworkCallMachineClientStateS1::GetInstance ();
  m_stateChangeCb = MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ();
}

McpttOnNetworkCallMachineClient::~McpttOnNetworkCallMachineClient (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkCallMachineClient::SetState (Ptr<McpttOnNetworkCallMachineClientState>  state)
{
  NS_LOG_FUNCTION (this << &state);
  McpttEntityId stateId = state->GetInstanceStateId ();
  McpttEntityId currStateId = m_state->GetInstanceStateId ();
  std::string selected = "False";
  if (GetCall ()->GetCallId () == GetCall ()->GetOwner ()->GetSelectedCall ()->GetCallId ())
    {
      selected = "True";
    }
  if (m_started && (stateId != currStateId))
    {
      NS_LOG_LOGIC (GetCall ()->GetOwner ()->GetUserId () << " moving from state " << *m_state << " to state " << *state << ".");
      m_state = state;
      if (!m_stateChangeCb.IsNull ())
        {
          m_stateChangeCb (currStateId, stateId);
        }
      m_stateChangeTrace (m_call->GetOwner ()->GetUserId (), m_call->GetCallId (), selected, GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

Ptr<McpttOnNetworkCallMachineClientState>
McpttOnNetworkCallMachineClient::GetState (void) const
{
  return m_state;
}

void
McpttOnNetworkCallMachineClient::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this << &stateChangeCb);

  m_stateChangeCb = stateChangeCb;
}

void
McpttOnNetworkCallMachineClient::AcceptCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkCallMachineClient::BeginEmergAlert (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkCallMachineClient::CancelEmergAlert (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkCallMachineClient::DowngradeCallType (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttOnNetworkCallMachineClient::GetCallerUserId (void) const
{
  NS_LOG_FUNCTION (this);

  return 0;
}

McpttCallMsgFieldCallId
McpttOnNetworkCallMachineClient::GetCallId (void) const
{
  return m_callId;
}

McpttCallMsgFieldCallType
McpttOnNetworkCallMachineClient::GetCallType (void) const
{
  McpttCallMsgFieldCallType callTypeField;
  callTypeField.SetType (m_callType);

  return callTypeField;
}

TypeId
McpttOnNetworkCallMachineClient::GetInstanceTypeId (void) const
{
  return McpttOnNetworkCallMachineClient::GetTypeId ();
}

Ptr<McpttCall>
McpttOnNetworkCallMachineClient::GetCall (void) const
{
  return m_call;
}

McpttEntityId
McpttOnNetworkCallMachineClient::GetStateId (void) const
{
  return m_state->GetInstanceStateId ();
}

void
McpttOnNetworkCallMachineClient::InitiateCall (void)
{
  NS_LOG_FUNCTION (this);
  if (GetStateId () == McpttOnNetworkCallMachineClientStateS1::GetInstance ()->GetStateId ())
    {
      NS_LOG_DEBUG ("Initiating call from state " << GetStateId ());
      m_state->InitiateCall (*this);
    }
  else
    {
      NS_LOG_DEBUG ("Cannot initiate call from state " << GetStateId ());
    }
}

bool
McpttOnNetworkCallMachineClient::IsCallOngoing (void) const
{
  if (GetStateId () != McpttOnNetworkCallMachineClientStateS1::GetInstance ()->GetStateId ())
    {
      return true;
    }
  return false;
}

bool
McpttOnNetworkCallMachineClient::IsGrpCall (uint32_t grpId) const
{
  NS_LOG_FUNCTION (this << grpId);

  return false;
}

bool
McpttOnNetworkCallMachineClient::IsPrivateCall (uint32_t userId) const
{
  NS_LOG_FUNCTION (this << userId);

  return false;
}

void
McpttOnNetworkCallMachineClient::ReceiveCallPacket (Ptr<Packet> pkt, const sip::SipHeader& sipHeader)
{
  NS_LOG_FUNCTION (this << pkt);
  uint16_t callId = sipHeader.GetCallId ();
  NS_ASSERT_MSG (callId == m_callId, "mismatch of call ID");
  uint32_t from = sipHeader.GetFrom ();
  if (sipHeader.GetMessageType () == sip::SipHeader::SIP_REQUEST)
    {
      if (sipHeader.GetMethod () == sip::SipHeader::INVITE)
        {
          m_state->ReceiveInvite (*this, from, pkt, sipHeader);
        }
      else if (sipHeader.GetMethod () == sip::SipHeader::BYE)
        {
          m_state->ReceiveBye (*this, from, pkt, sipHeader);
        }
    }
  else if (sipHeader.GetMessageType () == sip::SipHeader::SIP_RESPONSE)
    {
      m_state->ReceiveResponse (*this, from, pkt, sipHeader);
    }
}

void
McpttOnNetworkCallMachineClient::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);
}

void
McpttOnNetworkCallMachineClient::Receive (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);
}

void
McpttOnNetworkCallMachineClient::ReleaseCall (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("Release call for call ID " << GetCall ()->GetCallId () << " in state " << GetStateId ());
  m_state->ReleaseCall (*this);
}

void
McpttOnNetworkCallMachineClient::RejectCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkCallMachineClient::Send (const McpttCallMsg& hdr)
{
  NS_LOG_FUNCTION (this << hdr);
  NS_FATAL_ERROR ("Unused in on-network operation");
}

void
McpttOnNetworkCallMachineClient::SetCallId (const McpttCallMsgFieldCallId& callId)
{
  NS_LOG_DEBUG (this << callId);
  m_callId = callId.GetCallId ();
}

void
McpttOnNetworkCallMachineClient::SetNewCallCb (const Callback<void, uint16_t>  newCallCb)
{
  NS_LOG_FUNCTION (this << &newCallCb);
}

void
McpttOnNetworkCallMachineClient::SetCall (Ptr<McpttCall> call)
{
  NS_LOG_FUNCTION (this << call);
  m_call = call;
}

void
McpttOnNetworkCallMachineClient::Start (void)
{
  NS_LOG_FUNCTION (this);

  AddressValue grpAddr;
  GetCall ()->GetAttribute ("PeerAddress", grpAddr);

  Ptr<McpttFloorParticipant> floorMachine = GetCall ()->GetFloorMachine ();

  GetCall ()->OpenFloorChannel (grpAddr.Get (), m_floorPort);
  GetCall ()->OpenMediaChannel (grpAddr.Get (), m_mediaPort);

  floorMachine->Start ();
  m_started = true;
}

void
McpttOnNetworkCallMachineClient::Stop (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttFloorParticipant> floorMachine = GetCall ()->GetFloorMachine ();

  GetCall ()->CloseFloorChannel ();
  GetCall ()->CloseMediaChannel ();

  floorMachine->Stop ();
  m_started = false;
  SetState (McpttOnNetworkCallMachineClientStateS1::GetInstance ());
}

void
McpttOnNetworkCallMachineClient::UpgradeCallType (uint8_t callType)
{
  NS_LOG_FUNCTION (this << (uint32_t)callType);
  m_callType = callType;
}

uint32_t
McpttOnNetworkCallMachineClient::GetInvitePayloadSize (void)
{
  double value = m_invitePayloadSize->GetValue ();
  NS_ASSERT_MSG (value > 0, "Invalid random variate");
  return (static_cast<uint32_t> (value));
}

uint32_t
McpttOnNetworkCallMachineClient::GetByePayloadSize (void)
{
  double value = m_byePayloadSize->GetValue ();
  NS_ASSERT_MSG (value > 0, "Invalid random variate");
  return (static_cast<uint32_t> (value));
}

uint32_t
McpttOnNetworkCallMachineClient::GetResponsePayloadSize (void)
{
  double value = m_responsePayloadSize->GetValue ();
  NS_ASSERT_MSG (value > 0, "Invalid random variate");
  return (static_cast<uint32_t> (value));
}

int64_t
McpttOnNetworkCallMachineClient::AssignStreams (int64_t stream)
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
McpttOnNetworkCallMachineClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_call = 0;
  McpttCallMachine::DoDispose ();
}

void
McpttOnNetworkCallMachineClient::SetGrpId (uint32_t grpId)
{
  NS_LOG_FUNCTION (this << grpId);
  McpttCallMsgFieldGrpId grpIdField;
  grpIdField.SetGrpId (grpId);
  m_grpId = grpIdField;
}

McpttCallMsgFieldGrpId
McpttOnNetworkCallMachineClient::GetGrpId (void) const
{
  return m_grpId;
}

} // namespace psc
} // namespace ns3

