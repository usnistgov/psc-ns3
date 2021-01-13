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

#include "mcptt-call-msg.h"
#include "mcptt-server-call.h"
#include "mcptt-on-network-floor-towards-participant.h"
#include "mcptt-on-network-floor-towards-participant-state.h"
#include "mcptt-on-network-floor-arbitrator.h"
#include "mcptt-ptt-app.h"
#include "mcptt-sdp-fmtp-header.h"

#include "mcptt-server-call-machine-group-prearranged.h"
#include "mcptt-server-call-machine-group-prearranged-state.h"

namespace ns3 {

/** McpttServerCallMachineGroupPrearrangedState - begin **/
NS_LOG_COMPONENT_DEFINE ("McpttServerCallMachineGroupPrearrangedState");

McpttServerCallMachineGroupPrearrangedState::McpttServerCallMachineGroupPrearrangedState (void)
  : SimpleRefCount<McpttServerCallMachineGroupPrearrangedState> ()
{
  NS_LOG_FUNCTION (this);
}

McpttServerCallMachineGroupPrearrangedState::~McpttServerCallMachineGroupPrearrangedState (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttServerCallMachineGroupPrearrangedState::GetInstanceStateId (void) const
{
  return McpttEntityId ();
}

bool
McpttServerCallMachineGroupPrearrangedState::IsCallOngoing (const McpttServerCallMachineGroupPrearranged& machine) const
{
  return false;
}

void
McpttServerCallMachineGroupPrearrangedState::ReceiveInvite (McpttServerCallMachineGroupPrearranged& machine, uint32_t from, Ptr<Packet> pkt, const SipHeader& sipHeader)
{
  NS_LOG_FUNCTION (this << &machine << from << pkt << sipHeader);
  NS_LOG_LOGIC ("Ignoring INVITE");
}

void
McpttServerCallMachineGroupPrearrangedState::ReceiveBye (McpttServerCallMachineGroupPrearranged& machine, uint32_t from, Ptr<Packet> pkt, const SipHeader& sipHeader)
{
  NS_LOG_FUNCTION (this << &machine << from << pkt << sipHeader);
  NS_LOG_LOGIC ("Ignoring BYE"); 
}

void
McpttServerCallMachineGroupPrearrangedState::ReceiveResponse (McpttServerCallMachineGroupPrearranged& machine, uint32_t from, Ptr<Packet> pkt, const SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << &machine << from << pkt << hdr);
  NS_LOG_LOGIC ("Ignoring response"); 
}


/** McpttServerCallMachineGroupPrearrangedState - end **/

std::ostream&
operator<< (std::ostream& os, const McpttServerCallMachineGroupPrearrangedState& inst)
{
  McpttEntityId stateId = inst.GetInstanceStateId ();

  os << stateId;

  return os;
}

/** McpttServerCallMachineGroupPrearrangedStateS1 - begin **/
Ptr<McpttServerCallMachineGroupPrearrangedStateS1>
McpttServerCallMachineGroupPrearrangedStateS1::GetInstance (void)
{
  static Ptr<McpttServerCallMachineGroupPrearrangedStateS1> instance = Create<McpttServerCallMachineGroupPrearrangedStateS1> ();
  return instance;
}

McpttEntityId
McpttServerCallMachineGroupPrearrangedStateS1::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (1, "'S1: start-stop'");
  return stateId;
}

McpttServerCallMachineGroupPrearrangedStateS1::McpttServerCallMachineGroupPrearrangedStateS1 (void)
  : McpttServerCallMachineGroupPrearrangedState ()
{
  NS_LOG_FUNCTION (this);
}

McpttServerCallMachineGroupPrearrangedStateS1::~McpttServerCallMachineGroupPrearrangedStateS1 (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttServerCallMachineGroupPrearrangedStateS1::GetInstanceStateId (void) const
{
  return McpttServerCallMachineGroupPrearrangedStateS1::GetStateId ();
}

void
McpttServerCallMachineGroupPrearrangedStateS1::ReceiveInvite (McpttServerCallMachineGroupPrearranged& machine, uint32_t from, Ptr<Packet> pkt, const SipHeader& sipHeader)
{
  NS_LOG_FUNCTION (this << &machine << from << pkt << sipHeader);
  std::vector<uint32_t> pending;
  NS_ASSERT_MSG (machine.GetServerCall ()->GetOriginator () == std::numeric_limits<uint32_t>::max (), "Originator set in state S1");
  machine.GetServerCall ()->SetOriginator (from);
  McpttSdpFmtpHeader sdpHeader;
  pkt->RemoveHeader (sdpHeader);
  machine.SetState (McpttServerCallMachineGroupPrearrangedStateS2::GetInstance ());
  Ptr<McpttOnNetworkFloorTowardsParticipant> participant;
  participant = machine.GetServerCall ()->GetArbitrator ()->GetParticipantByUserId (from);
  machine.GetServerCall ()->GetArbitrator ()->CallInitialized (participant, sdpHeader.GetMcImplicitRequest ());
  sdpHeader.SetMcImplicitRequest (false);
  sdpHeader.SetMcGranted (false);
  for (uint32_t i = 0; i < machine.GetServerCall ()->GetClientUserIds ().size (); i++)
    {
      uint32_t userId = machine.GetServerCall ()->GetClientUserIds ().at (i);
      if (userId != from)
        {
          NS_LOG_DEBUG ("Forwarding invite to user ID: " << userId);
          Ptr<Packet> p = Create<Packet> ();
          p->AddHeader (sdpHeader);
          p->AddHeader (sipHeader);
          machine.SendSipRequest (userId, p, sipHeader);
          pending.push_back (userId);
        }
    }
  NS_ASSERT_MSG (pending.size (), "Error, no responses added to pending list");
  machine.SetPendingTransactionList (pending);
}

/** McpttServerCallMachineGroupPrearrangedStateS2 - begin **/
Ptr<McpttServerCallMachineGroupPrearrangedStateS2>
McpttServerCallMachineGroupPrearrangedStateS2::GetInstance (void)
{
  static Ptr<McpttServerCallMachineGroupPrearrangedStateS2> instance = Create<McpttServerCallMachineGroupPrearrangedStateS2> ();
  return instance;
}

McpttEntityId
McpttServerCallMachineGroupPrearrangedStateS2::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (2, "'S2: initiating'");
  return stateId;
}

McpttServerCallMachineGroupPrearrangedStateS2::McpttServerCallMachineGroupPrearrangedStateS2 (void)
  : McpttServerCallMachineGroupPrearrangedState ()
{
  NS_LOG_FUNCTION (this);
}

McpttServerCallMachineGroupPrearrangedStateS2::~McpttServerCallMachineGroupPrearrangedStateS2 (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttServerCallMachineGroupPrearrangedStateS2::GetInstanceStateId (void) const
{
  return McpttServerCallMachineGroupPrearrangedStateS2::GetStateId ();
}

void
McpttServerCallMachineGroupPrearrangedStateS2::ReceiveInvite (McpttServerCallMachineGroupPrearranged& machine, uint32_t from, Ptr<Packet> pkt, const SipHeader& sipHeader)
{
  // Collision between INVITEs; this is the later arriving one
  NS_LOG_FUNCTION (this << &machine << from << pkt << sipHeader);
  if (machine.GetServerCall ()->GetOriginator () == from)
    {
      NS_LOG_LOGIC ("Possible retransmission of INVITE from same originator");
      Ptr<Packet> response = Create<Packet> ();
      McpttSdpFmtpHeader sdpHeader;
      Ptr<McpttOnNetworkFloorTowardsParticipant> participant;
      participant = machine.GetServerCall ()->GetArbitrator ()->GetParticipantByUserId (from);
      sdpHeader.SetMcQueueing (machine.GetServerCall ()->GetArbitrator ()->IsQueueingSupported ());
      sdpHeader.SetMcGranted (participant->IsImplicitRequest ());
      sdpHeader.SetMcPriority (participant->GetStoredPriority ());
      sdpHeader.SetMcImplicitRequest (participant->IsImplicitRequest ());
      response->AddHeader (sdpHeader);
      
      // response may have been lost; send TRYING
      SipHeader sipHeader;
      sipHeader.SetMessageType (SipHeader::SIP_RESPONSE);
      sipHeader.SetStatusCode (100);
      sipHeader.SetFrom (machine.GetUserId ());
      sipHeader.SetTo (machine.GetServerCall ()->GetOriginator ());
      sipHeader.SetCallId (machine.GetServerCall ()->GetCallId ());
      response->AddHeader (sipHeader);
      machine.SendSipResponse (from, response, sipHeader);
    }
  else
    {
      NS_LOG_LOGIC ("Collision between INVITE; send OK to complete transaction");
      // if this invite had an implicit request, notify floor machine.
      // Set towards participant to U: Not Permitted and Floor Taken
      // and handle the implicit floor request
      Ptr<McpttOnNetworkFloorTowardsParticipant> participant;
      participant = machine.GetServerCall ()->GetArbitrator ()->GetParticipantByUserId (from);
      NS_ASSERT_MSG (participant, "Participant " << from << " not found");
      bool found = machine.RemoveFromPending (from);
      if (!found)
        {
          NS_LOG_DEBUG ("Received response from " << from << " but not found in pending list");
        }
      McpttSdpFmtpHeader sdpHeader;
      pkt->RemoveHeader (sdpHeader);
      machine.GetServerCall ()->GetArbitrator ()->CallInitialized (participant, sdpHeader.GetMcImplicitRequest ());
      Ptr<Packet> response = Create<Packet> ();
      sdpHeader.SetMcQueueing (machine.GetServerCall ()->GetArbitrator ()->IsQueueingSupported ());
      sdpHeader.SetMcGranted (participant->IsImplicitRequest ());
      sdpHeader.SetMcPriority (participant->GetStoredPriority ());
      sdpHeader.SetMcImplicitRequest (participant->IsImplicitRequest ());
      response->AddHeader (sdpHeader);
      // Other outstanding (pending) INVITE will take precedence
      SipHeader sipHeader;
      sipHeader.SetMessageType (SipHeader::SIP_RESPONSE);
      sipHeader.SetStatusCode (200);
      sipHeader.SetFrom (machine.GetUserId ());
      sipHeader.SetTo (machine.GetServerCall ()->GetOriginator ());
      sipHeader.SetCallId (machine.GetServerCall ()->GetCallId ());
      response->AddHeader (sipHeader);
      machine.SendSipResponse (from, response, sipHeader);
    }
  // No change of state
}

void
McpttServerCallMachineGroupPrearrangedStateS2::ReceiveResponse (McpttServerCallMachineGroupPrearranged& machine, uint32_t from, Ptr<Packet> pkt, const SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << &machine << from << pkt << hdr);

  // For each response 'from', set 'towards participant floor machine' state.
  // If originator had implicit floor request, then set not permitted/taken,
  // else set not permitted/idle (TODO)
  Ptr<McpttOnNetworkFloorTowardsParticipant> participant;
  participant = machine.GetServerCall ()->GetArbitrator ()->GetParticipantByUserId (from);
  NS_ASSERT_MSG (participant, "Participant " << from << " not found");
  participant->ChangeState (McpttOnNetworkFloorTowardsParticipantStateNotPermittedTaken::GetInstance ());
  bool found = machine.RemoveFromPending (from);
  if (!found)
    {
      NS_LOG_DEBUG ("Received response from " << from << " but not found in pending list");
    }

  if (machine.GetNPendingTransactions () == 0)
    {
      NS_LOG_DEBUG ("Processed last pending response; send response to originator");
      participant = machine.GetServerCall ()->GetArbitrator ()->GetOriginatingParticipant ();
      Ptr<Packet> response = Create<Packet> ();
      McpttSdpFmtpHeader sdpHeader;
      sdpHeader.SetMcQueueing (machine.GetServerCall ()->GetArbitrator ()->IsQueueingSupported ());
      sdpHeader.SetMcGranted (participant->IsImplicitRequest ());
      sdpHeader.SetMcPriority (participant->GetStoredPriority ());
      sdpHeader.SetMcImplicitRequest (participant->IsImplicitRequest ());
      response->AddHeader (sdpHeader);
      SipHeader sipHeader;
      sipHeader.SetMessageType (SipHeader::SIP_RESPONSE);
      sipHeader.SetStatusCode (200);
      sipHeader.SetFrom (machine.GetUserId ());
      sipHeader.SetTo (machine.GetServerCall ()->GetOriginator ());
      sipHeader.SetCallId (machine.GetServerCall ()->GetCallId ());
      response->AddHeader (sipHeader);
      machine.SendSipResponse (machine.GetServerCall ()->GetOriginator (), response, sipHeader);
      // If originator had implicit floor request, it should transition to
      // permitted, else not permitted and floor idle (TODO)
      participant->ChangeState (McpttOnNetworkFloorTowardsParticipantStatePermitted::GetInstance ());
    
      machine.SetState (McpttServerCallMachineGroupPrearrangedStateS3::GetInstance ());
    }
}

void
McpttServerCallMachineGroupPrearrangedStateS2::ReceiveBye (McpttServerCallMachineGroupPrearranged& machine, uint32_t from, Ptr<Packet> pkt, const SipHeader& sipHeader)
{
  NS_LOG_FUNCTION (this << &machine << from << pkt << sipHeader);
  machine.SetState (McpttServerCallMachineGroupPrearrangedStateS1::GetInstance ());
}

/** McpttServerCallMachineGroupPrearrangedStateS2 - end **/

/** McpttServerCallMachineGroupPrearrangedStateS3 - begin **/
Ptr<McpttServerCallMachineGroupPrearrangedStateS3>
McpttServerCallMachineGroupPrearrangedStateS3::GetInstance (void)
{
  static Ptr<McpttServerCallMachineGroupPrearrangedStateS3> instance = Create<McpttServerCallMachineGroupPrearrangedStateS3> ();
  return instance;
}

McpttEntityId
McpttServerCallMachineGroupPrearrangedStateS3::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (3, "'S3: part of ongoing call'");

  return stateId;
}

McpttServerCallMachineGroupPrearrangedStateS3::McpttServerCallMachineGroupPrearrangedStateS3 (void)
  : McpttServerCallMachineGroupPrearrangedState ()
{
  NS_LOG_FUNCTION (this);
}

McpttServerCallMachineGroupPrearrangedStateS3::~McpttServerCallMachineGroupPrearrangedStateS3 (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttServerCallMachineGroupPrearrangedStateS3::GetInstanceStateId (void) const
{
  return McpttServerCallMachineGroupPrearrangedStateS3::GetStateId ();
}

bool
McpttServerCallMachineGroupPrearrangedStateS3::IsCallOngoing (const McpttServerCallMachineGroupPrearranged& machine) const
{
  NS_LOG_FUNCTION (this << &machine);
  return true;
}

void
McpttServerCallMachineGroupPrearrangedStateS3::ReceiveResponse (McpttServerCallMachineGroupPrearranged& machine, uint32_t from, Ptr<Packet> pkt, const SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << &machine << from << pkt << hdr);
  machine.SetState (McpttServerCallMachineGroupPrearrangedStateS1::GetInstance ());
}

void
McpttServerCallMachineGroupPrearrangedStateS3::ReceiveBye (McpttServerCallMachineGroupPrearranged& machine, uint32_t from, Ptr<Packet> pkt, const SipHeader& sipHeader)
{
  NS_LOG_FUNCTION (this << &machine << from << pkt << sipHeader);

  Ptr<McpttOnNetworkFloorTowardsParticipant> participant;
  std::vector<uint32_t> pending;
  participant = machine.GetServerCall ()->GetArbitrator ()->GetParticipantByUserId (from);
  NS_ASSERT_MSG (participant, "Participant " << from << " not found");
  participant->ChangeState (McpttOnNetworkFloorTowardsParticipantStateReleasing::GetInstance ());

  /// TODO:  allow non-originator to release
  if (machine.GetServerCall ()->GetOriginator () == from)
    {
      pkt->AddHeader (sipHeader);
      for (uint32_t i = 0; i < machine.GetServerCall ()->GetClientUserIds ().size (); i++)
        {
          uint32_t userId = machine.GetServerCall ()->GetClientUserIds ().at (i);
          if (userId != from)
            {
              NS_LOG_DEBUG ("Forwarding BYE to user ID: " << userId);
              machine.SendSipRequest (userId, pkt, sipHeader);
              pending.push_back (userId);
            }
        }
      machine.SetState (McpttServerCallMachineGroupPrearrangedStateS4::GetInstance ());
      machine.GetServerCall ()->GetArbitrator ()->CallRelease1 ();
      NS_ASSERT_MSG (pending.size (), "Error, no responses added to pending list");
      machine.SetPendingTransactionList (pending);
      NS_LOG_DEBUG ("Reply to BYE with 200 OK");
      Ptr<Packet> response = Create<Packet> ();
      SipHeader newHeader;
      newHeader.SetMessageType (SipHeader::SIP_RESPONSE);
      newHeader.SetStatusCode (200);
      newHeader.SetFrom (sipHeader.GetFrom ());
      newHeader.SetTo (sipHeader.GetTo ());
      newHeader.SetCallId (sipHeader.GetCallId ());
      response->AddHeader (newHeader);
      machine.SendSipResponse (machine.GetServerCall ()->GetOriginator (), response, newHeader);
    }
  else
    {
      NS_LOG_DEBUG ("Received BYE from non-originator " << from);
    }
}

/** McpttServerCallMachineGroupPrearrangedStateS3 - end **/

/** McpttServerCallMachineGroupPrearrangedStateS4 - begin **/
Ptr<McpttServerCallMachineGroupPrearrangedStateS4>
McpttServerCallMachineGroupPrearrangedStateS4::GetInstance (void)
{
  static Ptr<McpttServerCallMachineGroupPrearrangedStateS4> instance = Create<McpttServerCallMachineGroupPrearrangedStateS4> ();
  return instance;
}

McpttEntityId
McpttServerCallMachineGroupPrearrangedStateS4::GetStateId (void)
{
  static McpttEntityId stateId = McpttEntityId (4, "'S4: releasing'");

  return stateId;
}

McpttServerCallMachineGroupPrearrangedStateS4::McpttServerCallMachineGroupPrearrangedStateS4 (void)
  : McpttServerCallMachineGroupPrearrangedState ()
{
  NS_LOG_FUNCTION (this);
}

McpttServerCallMachineGroupPrearrangedStateS4::~McpttServerCallMachineGroupPrearrangedStateS4 (void)
{
  NS_LOG_FUNCTION (this);
}

McpttEntityId
McpttServerCallMachineGroupPrearrangedStateS4::GetInstanceStateId (void) const
{
  return McpttServerCallMachineGroupPrearrangedStateS4::GetStateId ();
}
void
McpttServerCallMachineGroupPrearrangedStateS4::ReceiveBye (McpttServerCallMachineGroupPrearranged& machine, uint32_t from, Ptr<Packet> pkt, const SipHeader& sipHeader)
{
  // colliding BYEs
  NS_LOG_FUNCTION (this << &machine << from << pkt << sipHeader);
  machine.SetState (McpttServerCallMachineGroupPrearrangedStateS1::GetInstance ());
}

void
McpttServerCallMachineGroupPrearrangedStateS4::ReceiveResponse (McpttServerCallMachineGroupPrearranged& machine, uint32_t from, Ptr<Packet> pkt, const SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << &machine << from << pkt << hdr);

  Ptr<McpttOnNetworkFloorTowardsParticipant> participant;
  participant = machine.GetServerCall ()->GetArbitrator ()->GetParticipantByUserId (from);
  NS_ASSERT_MSG (participant, "Participant " << from << " not found");
  participant->ChangeState (McpttOnNetworkFloorTowardsParticipantStateStartStop::GetInstance ());
  bool found = machine.RemoveFromPending (from);
  if (!found)
    {
      NS_LOG_DEBUG ("Received response from " << from << " but not found in pending list");
    }

  if (machine.GetNPendingTransactions () == 0)
    {
      NS_LOG_DEBUG ("Processed last pending response; send response to originator");
      participant = machine.GetServerCall ()->GetArbitrator ()->GetOriginatingParticipant ();
      participant->ChangeState (McpttOnNetworkFloorTowardsParticipantStateStartStop::GetInstance ());
      Ptr<Packet> response = Create<Packet> ();
      McpttSdpFmtpHeader sdpHeader;
      sdpHeader.SetMcQueueing (machine.GetServerCall ()->GetArbitrator ()->IsQueueingSupported ());
      sdpHeader.SetMcGranted (participant->IsImplicitRequest ());
      sdpHeader.SetMcPriority (participant->GetStoredPriority ());
      sdpHeader.SetMcImplicitRequest (participant->IsImplicitRequest ());
      response->AddHeader (sdpHeader);
      SipHeader sipHeader;
      sipHeader.SetMessageType (SipHeader::SIP_RESPONSE);
      sipHeader.SetStatusCode (200);
      sipHeader.SetFrom (machine.GetUserId ());
      sipHeader.SetTo (machine.GetServerCall ()->GetOriginator ());
      sipHeader.SetCallId (machine.GetServerCall ()->GetCallId ());
      response->AddHeader (sipHeader);
      machine.SendSipResponse (machine.GetServerCall ()->GetOriginator (), response, sipHeader);
    
      machine.GetServerCall ()->GetArbitrator ()->CallRelease2 ();
      machine.SetState (McpttServerCallMachineGroupPrearrangedStateS1::GetInstance ());
    }
}

/** McpttServerCallMachineGroupPrearrangedStateS4 - end **/

} // namespace ns3
