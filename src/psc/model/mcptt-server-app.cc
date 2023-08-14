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
#include <ns3/ipv4-address.h>
#include <ns3/pointer.h>
#include <ns3/uinteger.h>
#include <ns3/object-map.h>
#include <ns3/sip-header.h>
#include <ns3/sip-proxy.h>

#include "mcptt-on-network-floor-arbitrator.h"
#include "mcptt-server-call.h"
#include "mcptt-call-msg.h"
#include "mcptt-server-call-machine.h"
#include "mcptt-channel.h"

#include "mcptt-server-app.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttServerApp");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttServerApp);

uint16_t McpttServerApp::s_callId = 1;

TypeId
McpttServerApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttServerApp")
    .SetParent<Application> ()
    .AddConstructor<McpttServerApp>()
    .AddAttribute ("CallPort", "The port that the application will use for call control messages.",
                   UintegerValue (5060), // standard SIP call control port
                   MakeUintegerAccessor (&McpttServerApp::m_callPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Calls", "The map of all calls created during the simulation.",
                   ObjectMapValue (),
                   MakeObjectMapAccessor (&McpttServerApp::m_calls),
                   MakeObjectMapChecker<McpttServerCall> ())
    .AddAttribute ("LocalAddress", "The local address of the server.",
                   AddressValue (Ipv4Address::GetAny ()),
                   MakeAddressAccessor (&McpttServerApp::m_localAddress),
                   MakeAddressChecker ())
    .AddAttribute ("PeerAddress", "The peer address of the server.",
                   AddressValue (Ipv4Address::GetAny ()),
                   MakeAddressAccessor (&McpttServerApp::m_peerAddress),
                   MakeAddressChecker ())
    .AddTraceSource ("RxTrace", "The trace for capturing received messages",
                     MakeTraceSourceAccessor (&McpttServerApp::m_rxTrace),
                     "ns3::psc::McpttServerApp::RxTrace")
    .AddTraceSource ("TxTrace", "The trace for capturing sent messages",
                     MakeTraceSourceAccessor (&McpttServerApp::m_txTrace),
                     "ns3::psc::McpttServerApp::TxTrace")
  ;

  return tid;
}

McpttServerApp::McpttServerApp (void)
  : Application (),
    m_callChannel (0)
{
  NS_LOG_FUNCTION (this);
  m_sipProxy = CreateObject<sip::SipProxy> ();
}

McpttServerApp::~McpttServerApp (void)
{
  NS_LOG_FUNCTION (this);
}

Ptr<sip::SipProxy>
McpttServerApp::GetSipProxy (void) const
{
  return m_sipProxy;
}

uint16_t
McpttServerApp::AllocateCallId (void)
{
  return s_callId++;
}

void
McpttServerApp::AddCall (Ptr<McpttServerCall> call)
{
  NS_LOG_FUNCTION (this);
  NS_ABORT_MSG_IF (call->GetCallId () > s_callId, "CallID out of range");
  NS_LOG_DEBUG ("Inserting call with callId " << call->GetCallId () << " to list");
  call->SetOwner (this);
  m_calls.insert ({call->GetCallId (), call});
}

Ptr<McpttServerCall>
McpttServerApp::GetCall (uint16_t callId)
{
  NS_LOG_FUNCTION (this);
  Ptr<McpttServerCall> call = nullptr;
  std::map<uint16_t, Ptr<McpttServerCall> >::iterator it = m_calls.find (callId);
  if (it != m_calls.end ())
    {
      call = it->second;
    }
  return call;
}

void
McpttServerApp::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  for (auto it = m_calls.begin (); it != m_calls.end (); it++)
    {
      it->second->Dispose ();
    }
  m_calls.clear ();
  if (m_callChannel)
    {
      m_callChannel->Dispose ();
      m_callChannel = 0;
    }
  m_sipProxy->Dispose ();
  m_sipProxy = 0;
  Object::DoDispose ();
}

void
McpttServerApp::TraceMessageReceive (uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  m_rxTrace (this, callId, pkt, headerType);
}

void
McpttServerApp::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
  m_callChannel = CreateObject<McpttChannel> ();
  // Set the McpttChan object to deliver incoming packets to SipProxy::Receive
  m_callChannel->SetRxPktCb (MakeCallback (&sip::SipProxy::Receive, m_sipProxy));
  NS_LOG_DEBUG ("Open socket for incoming call control on port " << m_callPort);
  m_callChannel->Open (GetNode (), m_callPort, m_localAddress, m_peerAddress);
  for (auto it = m_calls.begin (); it != m_calls.end (); it++)
    {
      NS_LOG_DEBUG ("Starting call for id " << it->first);
      it->second->GetCallMachine ()->Start ();
      // Set the SipProxy to deliver received packets back to
      // McpttServerCall::ReceiveSipMessage and events to
      // McpttServerCall::ReceiveSipEvent
      m_sipProxy->SetCallbacks (it->first, MakeCallback (&McpttServerCall::ReceiveSipMessage, it->second), MakeCallback (&McpttServerCall::ReceiveSipEvent, it->second));
    }
  m_isRunning = true;
}

void
McpttServerApp::StopApplication (void)
{
  NS_LOG_FUNCTION (this);

  for (auto it = m_calls.begin (); it != m_calls.end (); it++)
    {
      NS_LOG_DEBUG ("Stopping call for id " << it->first);
      it->second->GetCallMachine ()->Stop ();
    }
  m_isRunning = false;
}

void
McpttServerApp::SendCallControlPacket (Ptr<Packet> pkt, const Address& toAddr)
{
  NS_LOG_FUNCTION (this << pkt << toAddr);
  m_callChannel->SendTo (pkt, 0, toAddr);
}

void
McpttServerApp::TraceMessageSend (uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  m_txTrace (this, callId, pkt, headerType);
}

Address
McpttServerApp::GetLocalAddress (void) const
{
  return m_localAddress;
}

void
McpttServerApp::SetLocalAddress (const Address& localAddress)
{
  NS_LOG_FUNCTION (this);

  m_localAddress = localAddress;
}

bool
McpttServerApp::IsRunning (void) const
{
  return m_isRunning;
}

} // namespace psc
} // namespace ns3
