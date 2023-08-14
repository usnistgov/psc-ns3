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
#include <ns3/callback.h>
#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/fatal-error.h>
#include <ns3/boolean.h>
#include <ns3/sip-header.h>

#include "mcptt-call-msg.h"
#include "mcptt-channel.h"
#include "mcptt-on-network-floor-arbitrator.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-server-app.h"

#include "mcptt-server-call.h"
#include "mcptt-server-call-machine.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttServerCall");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttServerCall);

TypeId
McpttServerCall::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttServerCall")
    .SetParent<Object> ()
    .AddConstructor<McpttServerCall> ()
    .AddAttribute ("CallMachine", "The call machine of the call.",
                   PointerValue (0),
                   MakePointerAccessor (&McpttServerCall::m_callMachine),
                   MakePointerChecker<McpttServerCallMachine> ())
    .AddAttribute ("FloorArbitrator", "The floor arbitrator of the call.",
                   PointerValue (0),
                   MakePointerAccessor (&McpttServerCall::m_arbitrator),
                   MakePointerChecker<McpttOnNetworkFloorArbitrator> ())
    .AddAttribute ("AmbientListening", "Indicates if the call is configured for ambient listening.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttServerCall::m_ambientListening),
                   MakeBooleanChecker ())
    .AddAttribute ("TemporaryGroup", "Indicates if the call is configured for a temporary group session.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttServerCall::m_temporaryGroup),
                   MakeBooleanChecker ())
  ;
  return tid;
}

McpttServerCall::McpttServerCall (void)
  : Object (),
    m_callId (std::numeric_limits<uint16_t>::max ()),
    m_originator (std::numeric_limits<uint32_t>::max ()),
    m_owner (0)
{
  NS_LOG_FUNCTION (this);
}

McpttServerCall::~McpttServerCall (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttServerCall::SetCallId (uint16_t callId)
{
  NS_LOG_DEBUG (this << callId);
  m_callId = callId;
}

uint16_t
McpttServerCall::GetCallId (void) const
{
  return m_callId;
}

bool
McpttServerCall::IsAmbientListening (void) const
{
  return m_ambientListening;
}

bool
McpttServerCall::IsTemporaryGroup (void) const
{
  return m_temporaryGroup;
}

void
McpttServerCall::ReceiveSipMessage (Ptr<Packet> pkt, const sip::SipHeader& hdr, sip::SipProxy::TransactionState state)
{
  NS_LOG_FUNCTION (this << pkt << hdr << state);
  // SIP layer removes SIP header; add it back for tracing
  Ptr<Packet> pktCopy = pkt->Copy ();
  pktCopy->AddHeader (hdr);
  GetOwner ()->TraceMessageReceive (GetCallId (), pktCopy, hdr.GetInstanceTypeId ());
  GetCallMachine ()->ReceiveCallPacket (pkt, hdr);
}

void
McpttServerCall::ReceiveSipEvent (const char* event, sip::SipProxy::TransactionState state)
{
  NS_LOG_FUNCTION (this << event << state);
  // Not yet defined
}

void
McpttServerCall::SendCallControlPacket (Ptr<Packet> pkt, const Address& toAddr, const sip::SipHeader &hdr)
{
  NS_LOG_FUNCTION (this << pkt << toAddr << hdr);
  if (m_owner->IsRunning ())
    {
      m_owner->SendCallControlPacket (pkt, toAddr);
      GetOwner ()->TraceMessageSend (GetCallId (), pkt, hdr.GetInstanceTypeId ());
    }
  else
    {
      NS_LOG_DEBUG ("Not sending message because McpttServerApp is not running");
    }
}

void
McpttServerCall::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  if (GetArbitrator ())
    {
      GetArbitrator ()->Dispose ();
      SetArbitrator (0);
    }
  if (GetCallMachine ())
    {
      GetCallMachine ()->Dispose ();
      SetCallMachine (0);
    }
  SetOwner (0);

  Object::DoDispose ();
}

Ptr<McpttServerCallMachine>
McpttServerCall::GetCallMachine (void) const
{
  return m_callMachine;
}

Ptr<McpttOnNetworkFloorArbitrator>
McpttServerCall::GetArbitrator (void) const
{
  return m_arbitrator;
}

Ptr<McpttServerApp>
McpttServerCall::GetOwner (void) const
{
  return m_owner;
}

void
McpttServerCall::SetCallMachine (Ptr<McpttServerCallMachine>  callMachine)
{
  NS_LOG_FUNCTION (this << &callMachine);

  if (callMachine != 0)
    {
      callMachine->SetServerCall (this);
    }

  m_callMachine = callMachine;
}

void
McpttServerCall::SetArbitrator (Ptr<McpttOnNetworkFloorArbitrator>  arbitrator)
{
  NS_LOG_FUNCTION (this << &arbitrator);

  if (arbitrator != 0)
    {
      arbitrator->SetOwner (this);
    }

  m_arbitrator = arbitrator;
}

void
McpttServerCall::SetOwner (Ptr<McpttServerApp> owner)
{
  NS_LOG_FUNCTION (this << owner);

  m_owner = owner;
}

void
McpttServerCall::SetClientUserIds (std::vector<uint32_t> clientUserIds)
{
  NS_LOG_FUNCTION (this);
  m_clientUserIds = clientUserIds;
}

std::vector<uint32_t>
McpttServerCall::GetClientUserIds (void) const
{
  return m_clientUserIds;
}

void
McpttServerCall::SetOriginator (uint32_t originator)
{
  NS_LOG_FUNCTION (this << originator);
  m_originator = originator;
}

uint32_t
McpttServerCall::GetOriginator (void) const
{
  return m_originator;
}

} // namespace psc
} // namespace ns3

