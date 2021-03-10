/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <iostream>
#include <string>
#include <sstream>
#include "ns3/test.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/simple-net-device-helper.h"
#include "ns3/simple-net-device.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/inet-socket-address.h"
#include "ns3/error-model.h"

#include "ns3/sip-agent.h"
#include "ns3/sip-proxy.h"
#include "ns3/sip-header.h"

using namespace ns3;
using namespace sip;

NS_LOG_COMPONENT_DEFINE ("SipTestSuite");

/**
 * \ingroup tests
 */
class SipTestCase : public TestCase
{
public:
  SipTestCase (std::string name);
  // Scheduled events
  void Client1SendSipInvite (void);
  void Client1SendSipBye (void);

protected:
  // Promoted from private in case it needs to be called by subclasses
  virtual void DoSetup (void);
  // Member variables
  Ptr<SipAgent> m_sipAgent1;
  Ptr<SipAgent> m_sipAgent2;
  Ptr<SipAgent> m_sipAgent3;
  Ptr<SipProxy> m_sipProxy;
  Ptr<Socket> m_client1Socket;
  Ptr<Socket> m_client2Socket;
  Ptr<Socket> m_client3Socket;
  Ptr<Socket> m_serverSocket;
  Ptr<Node> m_server;
  Ptr<Node> m_client1;
  Ptr<Node> m_client2;
  Ptr<Node> m_client3;
  Ptr<SimpleNetDevice> m_serverDeviceTowardsClient1;
  Ptr<SimpleNetDevice> m_serverDeviceTowardsClient2;
  Ptr<SimpleNetDevice> m_serverDeviceTowardsClient3;
  Ptr<SimpleNetDevice> m_client1DeviceTowardsServer;
  Ptr<SimpleNetDevice> m_client2DeviceTowardsServer;
  Ptr<SimpleNetDevice> m_client3DeviceTowardsServer;
  // Test event recording
  struct TestEvent
  {
    uint32_t m_step;
    uint32_t m_userId;
    std::string m_message;
    TestEvent (uint32_t step, uint32_t userId, std::string message)
      : m_step (step),
        m_userId (userId),
        m_message (message)
    {}
    bool operator != (const TestEvent& b) const
    {
      return (m_step != b.m_step || m_userId != b.m_userId || m_message != b.m_message);
    }
  };
  uint32_t m_step {0};                   //!< Step number in test sequence
  std::list<TestEvent> m_expectedEvents; //!< Expected events
  std::list<TestEvent> m_observedEvents; //!< Observed events

  // Maintain per-node event lists for state change events
  uint32_t m_client1Step {0};
  std::list<TestEvent> m_client1ExpectedStateChanges; //!< Expected state changes
  std::list<TestEvent> m_client1ObservedStateChanges; //!< Observed state changes
  uint32_t m_client2Step {0};
  std::list<TestEvent> m_client2ExpectedStateChanges; //!< Expected state changes
  std::list<TestEvent> m_client2ObservedStateChanges; //!< Observed state changes
  uint32_t m_client3Step {0};
  std::list<TestEvent> m_client3ExpectedStateChanges; //!< Expected state changes
  std::list<TestEvent> m_client3ObservedStateChanges; //!< Observed state changes
  uint32_t m_proxyStep {0};
  std::list<TestEvent> m_proxyExpectedStateChanges; //!< Expected state changes
  std::list<TestEvent> m_proxyObservedStateChanges; //!< Observed state changes
  uint32_t m_proxyTimerBCount {0};                  //!< Count Timer B expiry

  InetSocketAddress m_client1Address;
  InetSocketAddress m_client2Address;
  InetSocketAddress m_client3Address;
  InetSocketAddress m_client1ServerAddress;
  InetSocketAddress m_client2ServerAddress;
  InetSocketAddress m_client3ServerAddress;
  uint16_t m_callId;
  bool m_receivedTerminatingReply;
  sip::SipHeader m_proxiedInviteHeader;

  // Callbacks for SIP proxy
  void ServerReceiveSipMessage (Ptr<Packet> pkt, const sip::SipHeader& hdr, sip::SipElement::TransactionState state);
  virtual void ServerReceiveSipEvent (const char* event, sip::SipElement::TransactionState state);
  virtual void ServerReceive (Ptr<Socket> socket);
  void ServerSendSipMessage (Ptr<Packet> pkt, const Address& addr, const sip::SipHeader& hdr);
  // Callbacks for SIP agents
  void Client1ReceiveSipMessage (Ptr<Packet> pkt, const sip::SipHeader& hdr, sip::SipElement::TransactionState state);
  virtual void Client1ReceiveSipEvent (const char* event, sip::SipElement::TransactionState state);
  virtual void Client1Receive (Ptr<Socket> socket);
  void Client1SendSipMessage (Ptr<Packet> pkt, const Address& addr, const sip::SipHeader& hdr);
  void Client2ReceiveSipMessage (Ptr<Packet> pkt, const sip::SipHeader& hdr, sip::SipElement::TransactionState state);
  virtual void Client2ReceiveSipEvent (const char* event, sip::SipElement::TransactionState state);
  virtual void Client2Receive (Ptr<Socket> socket);
  void Client2SendSipMessage (Ptr<Packet> pkt, const Address& addr, const sip::SipHeader& hdr);
  void Client3ReceiveSipMessage (Ptr<Packet> pkt, const sip::SipHeader& hdr, sip::SipElement::TransactionState state);
  virtual void Client3ReceiveSipEvent (const char* event, sip::SipElement::TransactionState state);
  virtual void Client3Receive (Ptr<Socket> socket);
  void Client3SendSipMessage (Ptr<Packet> pkt, const Address& addr, const sip::SipHeader& hdr);
  // Traces for state change events
  void ProxyDialogStateTrace (std::string context, uint16_t callId, uint32_t lowerUri, uint32_t higherUri, SipElement::DialogState state);
  void AgentDialogStateTrace (std::string context, uint16_t callId, uint32_t lowerUri, uint32_t higherUri, SipElement::DialogState state);
  void ProxyTransactionStateTrace (std::string context, uint16_t callId, uint32_t from, uint32_t to, SipElement::TransactionState state);
  void AgentTransactionStateTrace (std::string context, uint16_t callId, uint32_t from, uint32_t to, SipElement::TransactionState state);
  // Traces for IPv4 TX/RX events
  // These are just used for debug logging
  void ServerRxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface);
  void ServerTxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface);
  void Client1RxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface);
  void Client1TxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface);
  void Client2RxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface);
  void Client2TxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface);
  void Client3RxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface);
  void Client3TxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface);

private:
  virtual void DoRun (void) = 0;
};

SipTestCase::SipTestCase (std::string name)
  : TestCase (name),
    m_client1Address (Ipv4Address::GetAny (), 0),
    m_client2Address (Ipv4Address::GetAny (), 0),
    m_client3Address (Ipv4Address::GetAny (), 0),
    m_client1ServerAddress (Ipv4Address::GetAny (), 0),
    m_client2ServerAddress (Ipv4Address::GetAny (), 0),
    m_client3ServerAddress (Ipv4Address::GetAny (), 0),
    m_callId (0),
    m_receivedTerminatingReply (false)
{}

void
SipTestCase::DoSetup (void)
{
  NodeContainer n;
  m_server = CreateObject<Node> ();
  n.Add (m_server);
  m_client1 = CreateObject<Node> ();
  n.Add (m_client1);
  m_client2 = CreateObject<Node> ();
  n.Add (m_client2);
  m_client3 = CreateObject<Node> ();
  n.Add (m_client3);

  NodeContainer n1;
  n1.Add (m_server);
  n1.Add (m_client1);
  NodeContainer n2;
  n2.Add (m_server);
  n2.Add (m_client2);
  NodeContainer n3;
  n3.Add (m_server);
  n3.Add (m_client3);

  SimpleNetDeviceHelper helper;
  helper.SetNetDevicePointToPointMode (true);
  // Set the delays at slightly different values to make message sequences
  // deterministic
  helper.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  NetDeviceContainer nd1 = helper.Install (n1);
  m_serverDeviceTowardsClient1 = nd1.Get (0)->GetObject<SimpleNetDevice> ();
  m_client1DeviceTowardsServer = nd1.Get (1)->GetObject<SimpleNetDevice> ();
  helper.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (11)));
  NetDeviceContainer nd2 = helper.Install (n2);
  m_serverDeviceTowardsClient2 = nd2.Get (0)->GetObject<SimpleNetDevice> ();
  m_client2DeviceTowardsServer = nd2.Get (1)->GetObject<SimpleNetDevice> ();
  helper.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (12)));
  NetDeviceContainer nd3 = helper.Install (n3);
  m_serverDeviceTowardsClient3 = nd3.Get (0)->GetObject<SimpleNetDevice> ();
  m_client3DeviceTowardsServer = nd3.Get (1)->GetObject<SimpleNetDevice> ();

  InternetStackHelper internet;
  internet.Install (n);

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.252");
  Ipv4InterfaceContainer i1 = ipv4.Assign (nd1);
  m_client1ServerAddress = InetSocketAddress (Ipv4Address ("10.1.1.1"), 5060);
  m_client1Address = InetSocketAddress (Ipv4Address ("10.1.1.2"), 5060);
  ipv4.SetBase ("10.1.2.0", "255.255.255.252");
  Ipv4InterfaceContainer i2 = ipv4.Assign (nd2);
  m_client2ServerAddress = InetSocketAddress (Ipv4Address ("10.1.2.1"), 5060);
  m_client2Address = InetSocketAddress (Ipv4Address ("10.1.2.2"), 5060);
  ipv4.SetBase ("10.1.3.0", "255.255.255.252");
  Ipv4InterfaceContainer i3 = ipv4.Assign (nd3);
  m_client3ServerAddress = InetSocketAddress (Ipv4Address ("10.1.3.1"), 5060);
  m_client3Address = InetSocketAddress (Ipv4Address ("10.1.3.2"), 5060);

  Ptr<Ipv4L3Protocol> ipv4Server = m_server->GetObject<Ipv4L3Protocol> ();
  ipv4Server->TraceConnectWithoutContext ("Tx", MakeCallback (&SipTestCase::ServerTxTrace, this));
  ipv4Server->TraceConnectWithoutContext ("Rx", MakeCallback (&SipTestCase::ServerRxTrace, this));
  Ptr<Ipv4L3Protocol> ipv4Client1 = m_client1->GetObject<Ipv4L3Protocol> ();
  ipv4Client1->TraceConnectWithoutContext ("Tx", MakeCallback (&SipTestCase::Client1TxTrace, this));
  ipv4Client1->TraceConnectWithoutContext ("Rx", MakeCallback (&SipTestCase::Client1RxTrace, this));
  Ptr<Ipv4L3Protocol> ipv4Client2 = m_client2->GetObject<Ipv4L3Protocol> ();
  ipv4Client2->TraceConnectWithoutContext ("Tx", MakeCallback (&SipTestCase::Client2TxTrace, this));
  ipv4Client2->TraceConnectWithoutContext ("Rx", MakeCallback (&SipTestCase::Client2RxTrace, this));
  Ptr<Ipv4L3Protocol> ipv4Client3 = n.Get (3)->GetObject<Ipv4L3Protocol> ();
  ipv4Client3->TraceConnectWithoutContext ("Tx", MakeCallback (&SipTestCase::Client3TxTrace, this));
  ipv4Client3->TraceConnectWithoutContext ("Rx", MakeCallback (&SipTestCase::Client3RxTrace, this));

  m_client1Socket = Socket::CreateSocket (n.Get (1), UdpSocketFactory::GetTypeId ());
  m_client1Socket->Bind (InetSocketAddress (Ipv4Address::GetAny (), 5060));
  m_client1Socket->Connect (m_client1ServerAddress);
  m_client1Socket->SetRecvCallback (MakeCallback (&SipTestCase::Client1Receive, this));
  m_client2Socket = Socket::CreateSocket (n.Get (2), UdpSocketFactory::GetTypeId ());
  m_client2Socket->Bind (InetSocketAddress (Ipv4Address::GetAny (), 5060));
  m_client2Socket->Connect (m_client2ServerAddress);
  m_client2Socket->SetRecvCallback (MakeCallback (&SipTestCase::Client2Receive, this));
  m_client3Socket = Socket::CreateSocket (n.Get (3), UdpSocketFactory::GetTypeId ());
  m_client3Socket->Bind (InetSocketAddress (Ipv4Address::GetAny (), 5060));
  m_client3Socket->Connect (m_client3ServerAddress);
  m_client3Socket->SetRecvCallback (MakeCallback (&SipTestCase::Client3Receive, this));
  m_serverSocket = Socket::CreateSocket (n.Get (0), UdpSocketFactory::GetTypeId ());
  m_serverSocket->Bind (InetSocketAddress (Ipv4Address::GetAny (), 5060));
  // Do not connect the socket to any individual client
  m_serverSocket->SetRecvCallback (MakeCallback (&SipTestCase::ServerReceive, this));

  m_callId = 1000; // Arbitrary integer
  m_sipAgent1 = CreateObject<SipAgent> ();
  m_sipAgent1->SetCallbacks (m_callId, MakeCallback (&SipTestCase::Client1ReceiveSipMessage, this), MakeCallback (&SipTestCase::Client1ReceiveSipEvent, this));
  m_sipAgent2 = CreateObject<SipAgent> ();
  m_sipAgent2->SetCallbacks (m_callId, MakeCallback (&SipTestCase::Client2ReceiveSipMessage, this), MakeCallback (&SipTestCase::Client2ReceiveSipEvent, this));
  m_sipAgent3 = CreateObject<SipAgent> ();
  m_sipAgent3->SetCallbacks (m_callId, MakeCallback (&SipTestCase::Client3ReceiveSipMessage, this), MakeCallback (&SipTestCase::Client3ReceiveSipEvent, this));

  m_sipProxy = CreateObject<SipProxy> ();
  m_sipProxy->SetCallbacks (m_callId, MakeCallback (&SipTestCase::ServerReceiveSipMessage, this), MakeCallback (&SipTestCase::ServerReceiveSipEvent, this));
  m_sipProxy->SetDefaultSendCallback (MakeCallback (&SipTestCase::ServerSendSipMessage, this));

  m_sipProxy->TraceConnect ("DialogState", "0", MakeCallback (&SipTestCase::ProxyDialogStateTrace, this));
  m_sipProxy->TraceConnect ("TransactionState", "0", MakeCallback (&SipTestCase::ProxyTransactionStateTrace, this));
  m_sipAgent1->TraceConnect ("DialogState", "1", MakeCallback (&SipTestCase::AgentDialogStateTrace, this));
  m_sipAgent1->TraceConnect ("TransactionState", "1", MakeCallback (&SipTestCase::AgentTransactionStateTrace, this));
  m_sipAgent2->TraceConnect ("DialogState", "2", MakeCallback (&SipTestCase::AgentDialogStateTrace, this));
  m_sipAgent2->TraceConnect ("TransactionState", "2", MakeCallback (&SipTestCase::AgentTransactionStateTrace, this));
  m_sipAgent3->TraceConnect ("DialogState", "3", MakeCallback (&SipTestCase::AgentDialogStateTrace, this));
  m_sipAgent3->TraceConnect ("TransactionState", "3", MakeCallback (&SipTestCase::AgentTransactionStateTrace, this));
}

// Traces for IPv4 TX/RX events
void
SipTestCase::ServerRxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface)
{
  Ipv4Header header;
  packet->PeekHeader (header);
  NS_LOG_DEBUG ("Server received IP header: " << header);
}

void
SipTestCase::ServerTxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface)
{
  Ipv4Header header;
  packet->PeekHeader (header);
  NS_LOG_DEBUG ("Server sent IP header: " << header);
}

void
SipTestCase::Client1RxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface)
{
  Ipv4Header header;
  packet->PeekHeader (header);
  NS_LOG_DEBUG ("Client1 received IP header: " << header);
}

void
SipTestCase::Client1TxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface)
{
  Ipv4Header header;
  packet->PeekHeader (header);
  NS_LOG_DEBUG ("Client1 sent IP header: " << header);
}

void
SipTestCase::Client2RxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface)
{
  Ipv4Header header;
  packet->PeekHeader (header);
  NS_LOG_DEBUG ("Client2 received IP header: " << header);
}

void
SipTestCase::Client2TxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface)
{
  Ipv4Header header;
  packet->PeekHeader (header);
  NS_LOG_DEBUG ("Client2 sent IP header: " << header);
}

void
SipTestCase::Client3RxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface)
{
  Ipv4Header header;
  packet->PeekHeader (header);
  NS_LOG_DEBUG ("Client2 received IP header: " << header);
}

void
SipTestCase::Client3TxTrace (Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interface)
{
  Ipv4Header header;
  packet->PeekHeader (header);
  NS_LOG_DEBUG ("Client2 sent IP header: " << header);
}

void
SipTestCase::ServerReceiveSipMessage (Ptr<Packet> pkt, const sip::SipHeader& hdr, sip::SipElement::TransactionState state)
{
  NS_LOG_DEBUG ("Server received SIP message: " << hdr << " state: " << SipElement::TransactionStateToString (state));
  if (hdr.GetMessageType () == sip::SipHeader::SIP_REQUEST)
    {
      if (hdr.GetMethod () == sip::SipHeader::INVITE)
        {
          m_observedEvents.push_back (TestEvent (m_step, 0, "Server receive SIP INVITE"));
          m_step++;
          // SIP has delivered an invite to the proxy.  Check that this is
          // from client 1.  Send '100 Trying' to client 1, and then forward
          // the INVITE to client 2 and client 3.
          if (hdr.GetFrom () == 1)
            {
              m_observedEvents.push_back (TestEvent (m_step, 0, "Server send 100 Trying"));
              m_step++;
              Ptr<Packet> p1 = Create<Packet> ();
              // From: and To: fields remain the same in the response
              m_sipProxy->SendResponse (p1, m_client1Address, 100, hdr.GetFrom (), hdr.GetTo (), hdr.GetCallId (),
                                        MakeCallback (&SipTestCase::ServerSendSipMessage, this));
              // Save header for later responses
              m_proxiedInviteHeader = hdr;
              m_observedEvents.push_back (TestEvent (m_step, 0, "Server send SIP INVITE"));
              m_step++;
              Ptr<Packet> p2 = Create<Packet> ();
              // From: server URI, to:  client URI
              m_sipProxy->SendInvite (p2, m_client2Address, 2, 0, 2, hdr.GetCallId (),
                                      MakeCallback (&SipTestCase::ServerSendSipMessage, this));
              m_observedEvents.push_back (TestEvent (m_step, 0, "Server send SIP INVITE"));
              m_step++;
              Ptr<Packet> p3 = Create<Packet> ();
              // From: server URI, to:  client URI
              m_sipProxy->SendInvite (p3, m_client3Address, 3, 0, 3, hdr.GetCallId (),
                                      MakeCallback (&SipTestCase::ServerSendSipMessage, this));
            }
        }
      else if (hdr.GetMethod () == sip::SipHeader::BYE)
        {
          m_observedEvents.push_back (TestEvent (m_step, 0, "Server receive SIP BYE"));
          m_step++;
          // SIP has delivered a bye  to the proxy.  Check that this is
          // from client 1.  Send 'OK' to client 1, and then forward
          // the BYE to client 2 and client 3.
          if (hdr.GetFrom () == 1)
            {
              m_observedEvents.push_back (TestEvent (m_step, 0, "Server send 200 OK"));
              m_step++;
              Ptr<Packet> p1 = Create<Packet> ();
              m_sipProxy->SendResponse (p1, m_client1Address, 200, hdr.GetFrom (), hdr.GetTo (), hdr.GetCallId (),
                                        MakeCallback (&SipTestCase::ServerSendSipMessage, this));
              m_observedEvents.push_back (TestEvent (m_step, 0, "Server send SIP BYE"));
              m_step++;
              Ptr<Packet> p2 = Create<Packet> ();
              m_sipProxy->SendBye (p2, m_client2Address, 2, 0, 2, hdr.GetCallId (),
                                   MakeCallback (&SipTestCase::ServerSendSipMessage, this));
              m_observedEvents.push_back (TestEvent (m_step, 0, "Server send SIP BYE"));
              m_step++;
              Ptr<Packet> p3 = Create<Packet> ();
              m_sipProxy->SendBye (p3, m_client3Address, 3, 0, 3, hdr.GetCallId (),
                                   MakeCallback (&SipTestCase::ServerSendSipMessage, this));
            }
        }
    }
  else if (hdr.GetMessageType () == sip::SipHeader::SIP_RESPONSE)
    {
      if (hdr.GetStatusCode () == 100)
        {
          m_observedEvents.push_back (TestEvent (m_step, 0, "Server receive 100 Trying"));
          m_step++;
        }
      else if (hdr.GetStatusCode () == 200)
        {
          m_observedEvents.push_back (TestEvent (m_step, 0, "Server receive 200 OK"));
          m_step++;
          if (m_receivedTerminatingReply)
            {
              // Server has already sent the 200 OK to the originating client
            }
          else
            {
              // This is the first 200 OK received from client 2 or 3, so
              // send 200 OK to the originating client
              m_receivedTerminatingReply = true;
              m_observedEvents.push_back (TestEvent (m_step, 0, "Server send 200 OK"));
              m_step++;
              Ptr<Packet> p1 = Create<Packet> ();
              m_sipProxy->SendResponse (p1, m_client1Address, 200, m_proxiedInviteHeader.GetFrom (), m_proxiedInviteHeader.GetTo (), hdr.GetCallId (),
                                        MakeCallback (&SipTestCase::ServerSendSipMessage, this));
            }
        }
    }
}

void
SipTestCase::ServerReceiveSipEvent (const char* event, sip::SipElement::TransactionState state)
{
  NS_LOG_DEBUG ("Server received SIP event: " << event << "; state: " << SipElement::
                TransactionStateToString (state));
  if (strcmp (event, SipElement::ACK_RECEIVED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 0, "Server receive ACK"));
      m_step++;
    }
  else if (strcmp (event, SipElement::TRYING_RECEIVED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 0, "Server receive 100 Trying"));
      m_step++;
    }
  else if (strcmp (event, SipElement::TIMER_B_EXPIRED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 0, "Server Timer B expired"));
      m_step++;
      m_proxyTimerBCount++;
      if (m_proxyTimerBCount == 2)
        {
          // Both clients 2 and 3 failed; send 408 Request Timeout to client 1
          Ptr<Packet> p1 = Create<Packet> ();
          m_sipProxy->SendResponse (p1, m_client1Address, 408, m_proxiedInviteHeader.GetFrom (), m_proxiedInviteHeader.GetTo (), m_proxiedInviteHeader.GetCallId (),
                                    MakeCallback (&SipTestCase::ServerSendSipMessage, this));
          m_observedEvents.push_back (TestEvent (m_step, 0, "Server send 408 Request Timeout"));
        }
    }
}

void
SipTestCase::ServerReceive (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () == 0)
        {
          break;
        }
      NS_LOG_DEBUG ("Server received packet from UDP socket");
      m_observedEvents.push_back (TestEvent (m_step, 0, "Server receive SIP message"));
      m_step++;
      m_sipProxy->Receive (packet, from);
    }
}

void
SipTestCase::ServerSendSipMessage (Ptr<Packet> pkt, const Address& addr, const sip::SipHeader& hdr)
{
  NS_LOG_DEBUG ("Server sending SIP message: " << hdr);
  m_observedEvents.push_back (TestEvent (m_step, 0, "Server send SIP message"));
  m_step++;
  m_serverSocket->SendTo (pkt, 0, addr);
}

void
SipTestCase::Client1ReceiveSipMessage (Ptr<Packet> pkt, const sip::SipHeader& hdr, sip::SipElement::TransactionState state)
{
  NS_LOG_DEBUG ("Client1 received SIP message: " << hdr << " state: " << SipElement::TransactionStateToString (state));
  if (hdr.GetMessageType () == sip::SipHeader::SIP_RESPONSE)
    {
      if (hdr.GetStatusCode () == 100)
        {
          m_observedEvents.push_back (TestEvent (m_step, 1, "Client1 receive 100 Trying"));
          m_step++;
        }
      else if (hdr.GetStatusCode () == 200)
        {
          m_observedEvents.push_back (TestEvent (m_step, 1, "Client1 receive 200 OK"));
          m_step++;
        }
      else if (hdr.GetStatusCode () == 408)
        {
          m_observedEvents.push_back (TestEvent (m_step, 1, "Client1 receive 408 Request Timeout"));
          m_step++;
        }
    }
}

void
SipTestCase::Client1ReceiveSipEvent (const char* event, sip::SipElement::TransactionState state)
{
  NS_LOG_DEBUG ("Client1 received SIP event: " << event << "; state: " << SipElement::TransactionStateToString (state));
  if (strcmp (event, SipElement::ACK_RECEIVED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 1, "Client1 receive ACK"));
      m_step++;
    }
  else if (strcmp (event, SipElement::TRYING_RECEIVED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 1, "Client1 receive 100 Trying"));
      m_step++;
    }
  else if (strcmp (event, SipElement::TIMER_A_EXPIRED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 1, "Client1 Timer A expired"));
      m_step++;
    }
  else if (strcmp (event, SipElement::TIMER_B_EXPIRED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 1, "Client1 Timer B expired"));
      m_step++;
    }
  else if (strcmp (event, SipElement::TIMER_E_EXPIRED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 1, "Client1 Timer E expired"));
      m_step++;
    }
  else if (strcmp (event, SipElement::TIMER_F_EXPIRED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 1, "Client1 Timer F expired"));
      m_step++;
    }
}

void
SipTestCase::Client1Receive (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () == 0)
        {
          break;
        }
      NS_LOG_DEBUG ("Client1 received packet from UDP socket");
      m_observedEvents.push_back (TestEvent (m_step, 1, "Client1 receive SIP message"));
      m_step++;
      m_sipAgent1->Receive (packet, from);
    }
}

void
SipTestCase::Client1SendSipMessage (Ptr<Packet> pkt, const Address& addr, const sip::SipHeader& hdr)
{
  NS_LOG_DEBUG ("Client1 sending SIP message: " << hdr);
  m_observedEvents.push_back (TestEvent (m_step, 1, "Client1 send SIP message"));
  m_step++;
  m_client1Socket->SendTo (pkt, 0, addr);
}

void
SipTestCase::Client2ReceiveSipMessage (Ptr<Packet> pkt, const sip::SipHeader& hdr, sip::SipElement::TransactionState state)
{
  NS_LOG_DEBUG ("Client2 received SIP message: " << hdr << " state: " << SipElement::TransactionStateToString (state));
  if (hdr.GetMessageType () == sip::SipHeader::SIP_REQUEST)
    {
      if (hdr.GetMethod () == sip::SipHeader::INVITE)
        {
          m_observedEvents.push_back (TestEvent (m_step, 2, "Client2 receive SIP INVITE"));
          m_step++;
          m_observedEvents.push_back (TestEvent (m_step, 2, "Client2 send 200 OK"));
          m_step++;
          Ptr<Packet> p2 = Create<Packet> ();
          m_sipAgent2->SendResponse (p2, m_client2ServerAddress, 200, hdr.GetFrom (), hdr.GetTo (), hdr.GetCallId (),
                                     MakeCallback (&SipTestCase::Client2SendSipMessage, this));
        }
      else if (hdr.GetMethod () == sip::SipHeader::BYE)
        {
          m_observedEvents.push_back (TestEvent (m_step, 2, "Client2 receive SIP BYE"));
          m_step++;
          m_observedEvents.push_back (TestEvent (m_step, 2, "Client2 send 200 OK"));
          m_step++;
          Ptr<Packet> p2 = Create<Packet> ();
          m_sipAgent2->SendResponse (p2, m_client2ServerAddress, 200, hdr.GetFrom (), hdr.GetTo (), hdr.GetCallId (),
                                     MakeCallback (&SipTestCase::Client2SendSipMessage, this));
        }
    }
}

void
SipTestCase::Client2ReceiveSipEvent (const char* event, sip::SipElement::TransactionState state)
{
  NS_LOG_DEBUG ("Client2 received SIP event: " << event << "; state: " << SipElement::TransactionStateToString (state));
  if (strcmp (event, SipElement::ACK_RECEIVED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 2, "Client2 receive ACK"));
      m_step++;
    }
  else if (strcmp (event, SipElement::TRYING_RECEIVED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 2, "Client2 receive 100 Trying"));
      m_step++;
    }
}

void
SipTestCase::Client2Receive (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () == 0)
        {
          break;
        }
      NS_LOG_DEBUG ("Client2 received packet from UDP socket");
      m_observedEvents.push_back (TestEvent (m_step, 2, "Client2 receive SIP message"));
      m_step++;
      m_sipAgent2->Receive (packet, from);
    }
}

void
SipTestCase::Client2SendSipMessage (Ptr<Packet> pkt, const Address& addr, const sip::SipHeader& hdr)
{
  NS_LOG_DEBUG ("Client2 sending SIP message: " << hdr);
  m_observedEvents.push_back (TestEvent (m_step, 2, "Client2 send SIP message"));
  m_step++;
  m_client2Socket->SendTo (pkt, 0, addr);
}

void
SipTestCase::Client3ReceiveSipMessage (Ptr<Packet> pkt, const sip::SipHeader& hdr, sip::SipElement::TransactionState state)
{
  NS_LOG_DEBUG ("Client3 received SIP message: " << hdr << " state: " << SipElement::TransactionStateToString (state));
  if (hdr.GetMessageType () == sip::SipHeader::SIP_REQUEST)
    {
      if (hdr.GetMethod () == sip::SipHeader::INVITE)
        {
          m_observedEvents.push_back (TestEvent (m_step, 3, "Client3 receive SIP INVITE"));
          m_step++;
          m_observedEvents.push_back (TestEvent (m_step, 3, "Client3 send 200 OK"));
          m_step++;
          Ptr<Packet> p3 = Create<Packet> ();
          m_sipAgent3->SendResponse (p3, m_client3ServerAddress, 200, hdr.GetFrom (), hdr.GetTo (), hdr.GetCallId (),
                                     MakeCallback (&SipTestCase::Client3SendSipMessage, this));
        }
      else if (hdr.GetMethod () == sip::SipHeader::BYE)
        {
          m_observedEvents.push_back (TestEvent (m_step, 3, "Client3 receive SIP BYE"));
          m_step++;
          m_observedEvents.push_back (TestEvent (m_step, 3, "Client3 send 200 OK"));
          m_step++;
          Ptr<Packet> p2 = Create<Packet> ();
          m_sipAgent3->SendResponse (p2, m_client3ServerAddress, 200, hdr.GetFrom (), hdr.GetTo (), hdr.GetCallId (),
                                     MakeCallback (&SipTestCase::Client3SendSipMessage, this));
        }
    }
}

void
SipTestCase::Client3ReceiveSipEvent (const char* event, sip::SipElement::TransactionState state)
{
  NS_LOG_DEBUG ("Client3 received SIP event: " << event << "; state: " << SipElement::TransactionStateToString (state));
  if (strcmp (event, SipElement::ACK_RECEIVED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 2, "Client2 receive ACK"));
      m_step++;
    }
  else if (strcmp (event, SipElement::TRYING_RECEIVED) == 0)
    {
      m_observedEvents.push_back (TestEvent (m_step, 2, "Client2 receive 100 Trying"));
      m_step++;
    }
}

void
SipTestCase::Client3Receive (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () == 0)
        {
          break;
        }
      NS_LOG_DEBUG ("Client3 received packet from UDP socket");
      m_observedEvents.push_back (TestEvent (m_step, 3, "Client3 receive SIP message"));
      m_step++;
      m_sipAgent3->Receive (packet, from);
    }
}

void
SipTestCase::Client3SendSipMessage (Ptr<Packet> pkt, const Address& addr, const sip::SipHeader& hdr)
{
  NS_LOG_DEBUG ("Client3 sending SIP message: " << hdr);
  m_observedEvents.push_back (TestEvent (m_step, 3, "Client3 send SIP message"));
  m_step++;
  m_client3Socket->SendTo (pkt, 0, addr);
}


// This is specialized to client 1
void
SipTestCase::Client1SendSipInvite (void)
{
  Ptr<Packet> p = Create<Packet> ();
  uint32_t clientUri = 1; // representing client URI
  uint32_t requestUri = 0; // representing server URI
  NS_LOG_DEBUG ("Send SIP INVITE to request URI " << requestUri << " clientUri " << clientUri);
  m_observedEvents.push_back (TestEvent (m_step, clientUri, "Client1 send SIP INVITE"));
  m_step++;
  m_sipAgent1->SendInvite (p, m_client1ServerAddress,
                           requestUri, // request URI
                           clientUri,  // from
                           requestUri, // to
                           m_callId,   // Call ID
                           MakeCallback (&SipTestCase::Client1SendSipMessage, this));
}

// This is specialized to client 1
void
SipTestCase::Client1SendSipBye (void)
{
  Ptr<Packet> p = Create<Packet> ();
  uint32_t clientUri = 1; // representing client URI
  uint32_t requestUri = 0; // representing server URI
  NS_LOG_DEBUG ("Send SIP BYE to request URI " << requestUri << " clientUri " << clientUri);
  m_observedEvents.push_back (TestEvent (m_step, clientUri, "Client1 send SIP BYE"));
  m_step++;
  m_sipAgent1->SendBye (p, m_client1ServerAddress,
                        requestUri, // request URI
                        clientUri,  // from
                        requestUri, // to
                        m_callId,   // Call ID
                        MakeCallback (&SipTestCase::Client1SendSipMessage, this));
}

void
SipTestCase::ProxyDialogStateTrace (std::string context, uint16_t callId, uint32_t lowerUri, uint32_t higherUri, SipElement::DialogState state)
{
  std::stringstream message;
  message << "Dialog (" << callId << "," << lowerUri << "," << higherUri << ") enter " << SipElement::DialogStateToString (state);
  m_proxyObservedStateChanges.push_back (TestEvent (m_proxyStep, 0, message.str ()));
  m_proxyStep++;
}

void
SipTestCase::AgentDialogStateTrace (std::string context, uint16_t callId, uint32_t lowerUri, uint32_t higherUri, SipElement::DialogState state)
{
  std::stringstream message;
  message << "Dialog (" << callId << "," << lowerUri << "," << higherUri << ") enter " << SipElement::DialogStateToString (state);
  if (context.compare ("1") == 0)
    {
      m_client1ObservedStateChanges.push_back (TestEvent (m_client1Step, 0, message.str ()));
      m_client1Step++;
    }
  else if (context.compare ("2") == 0)
    {
      m_client2ObservedStateChanges.push_back (TestEvent (m_client2Step, 0, message.str ()));
      m_client2Step++;
    }
  else if (context.compare ("3") == 0)
    {
      m_client3ObservedStateChanges.push_back (TestEvent (m_client3Step, 0, message.str ()));
      m_client3Step++;
    }
}

void
SipTestCase::ProxyTransactionStateTrace (std::string context, uint16_t callId, uint32_t from, uint32_t to, SipElement::TransactionState state)
{
  std::stringstream message;
  message << "Transaction (" << callId << "," << from << "," << to << ") enter " << SipElement::TransactionStateToString (state);
  m_proxyObservedStateChanges.push_back (TestEvent (m_proxyStep, 0, message.str ()));
  m_proxyStep++;
}

void
SipTestCase::AgentTransactionStateTrace (std::string context, uint16_t callId, uint32_t from, uint32_t to, SipElement::TransactionState state)
{
  std::stringstream message;
  message << "Transaction (" << callId << "," << from << "," << to << ") enter " << SipElement::TransactionStateToString (state);
  if (context.compare ("1") == 0)
    {
      m_client1ObservedStateChanges.push_back (TestEvent (m_client1Step, 0, message.str ()));
      m_client1Step++;
    }
  else if (context.compare ("2") == 0)
    {
      m_client2ObservedStateChanges.push_back (TestEvent (m_client2Step, 0, message.str ()));
      m_client2Step++;
    }
  else if (context.compare ("3") == 0)
    {
      m_client3ObservedStateChanges.push_back (TestEvent (m_client3Step, 0, message.str ()));
      m_client3Step++;
    }
}

/**
 * \ingroup tests
 * Test a full dialog with no message losses (normal operation)
 */
class SipDialogTest : public SipTestCase
{
public:
  SipDialogTest (void);

private:
  virtual void DoRun (void);
};

SipDialogTest::SipDialogTest (void)
  : SipTestCase ("SIP dialog test")
{}

void
SipDialogTest::DoRun ()
{
  // The first events are triggered by client 1 sending a SIP INVITE
  Simulator::Schedule (Seconds (1), &SipTestCase::Client1SendSipInvite, this);

  // The final events are triggered by client 1 sending a SIP BYE
  Simulator::Schedule (Seconds (10), &SipTestCase::Client1SendSipBye, this);

  // Define the expected events
  // Client 1 sends INVITE
  m_expectedEvents.push_back (TestEvent (0, 1, "Client1 send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (1, 1, "Client1 send SIP message"));
  // Server receives invite, sends trying to client 1, sends INVITE to others
  m_expectedEvents.push_back (TestEvent (2, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (3, 0, "Server receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (4, 0, "Server send 100 Trying"));
  m_expectedEvents.push_back (TestEvent (5, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (6, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (7, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (8, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (9, 0, "Server send SIP message"));
  // Client 1 receives Trying
  m_expectedEvents.push_back (TestEvent (10, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (11, 1, "Client1 receive 100 Trying"));
  // Clients 2 and 3 receive INVITE and automatically commence
  m_expectedEvents.push_back (TestEvent (12, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (13, 2, "Client2 receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (14, 2, "Client2 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (15, 2, "Client2 send SIP message"));
  m_expectedEvents.push_back (TestEvent (16, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (17, 3, "Client3 receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (18, 3, "Client3 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (19, 3, "Client3 send SIP message"));
  // Server receives the first OK (from client 2), and returns an OK
  // to the originating client
  m_expectedEvents.push_back (TestEvent (20, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (21, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (22, 0, "Server send 200 OK"));
  m_expectedEvents.push_back (TestEvent (23, 0, "Server send SIP message"));
  // Server sends ACK for 200 OK
  m_expectedEvents.push_back (TestEvent (24, 0, "Server send SIP message"));
  // Server receives the second OK from client 3, and returns an ACK
  m_expectedEvents.push_back (TestEvent (25, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (26, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (27, 0, "Server send SIP message"));
  // Client 1 receives OK and sends ACK
  m_expectedEvents.push_back (TestEvent (28, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (29, 1, "Client1 receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (30, 1, "Client1 send SIP message"));
  // Clients 2 and 3 receive ACK
  m_expectedEvents.push_back (TestEvent (31, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (32, 2, "Client2 receive ACK"));
  m_expectedEvents.push_back (TestEvent (33, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (34, 2, "Client2 receive ACK"));
  // Server receives ACK
  m_expectedEvents.push_back (TestEvent (35, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (36, 0, "Server receive ACK"));
  // Client 1 sends a BYE
  m_expectedEvents.push_back (TestEvent (37, 1, "Client1 send SIP BYE"));
  m_expectedEvents.push_back (TestEvent (38, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (39, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (40, 0, "Server receive SIP BYE"));
  // Server sends 200 OK back to Client 1, and forwards BYE to clients 2 and 3
  m_expectedEvents.push_back (TestEvent (41, 0, "Server send 200 OK"));
  m_expectedEvents.push_back (TestEvent (42, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (43, 0, "Server send SIP BYE"));
  m_expectedEvents.push_back (TestEvent (44, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (45, 0, "Server send SIP BYE"));
  m_expectedEvents.push_back (TestEvent (46, 0, "Server send SIP message"));
  // Client 1 receives the OK and does not reply with any message
  m_expectedEvents.push_back (TestEvent (47, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (48, 1, "Client1 receive 200 OK"));
  // Clients 2 and 3 receive the BYE and reply with OK
  m_expectedEvents.push_back (TestEvent (49, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (50, 2, "Client2 receive SIP BYE"));
  m_expectedEvents.push_back (TestEvent (51, 2, "Client2 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (52, 2, "Client2 send SIP message"));
  m_expectedEvents.push_back (TestEvent (53, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (54, 3, "Client3 receive SIP BYE"));
  m_expectedEvents.push_back (TestEvent (55, 3, "Client3 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (56, 3, "Client3 send SIP message"));
  // Server receives both OK responses
  m_expectedEvents.push_back (TestEvent (57, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (58, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (59, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (60, 0, "Server receive 200 OK"));

  // Define the expected state transitions
  // Client 1 plays the UAC role in sending an INVITE
  m_client1ExpectedStateChanges.push_back (TestEvent (0, 0, "Dialog (1000,0,1) enter TRYING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (1, 0, "Transaction (1000,1,0) enter CALLING"));
  // receives 100 Trying
  m_client1ExpectedStateChanges.push_back (TestEvent (2, 0, "Dialog (1000,0,1) enter PROCEEDING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (3, 0, "Transaction (1000,1,0) enter PROCEEDING"));
  // receives OK
  m_client1ExpectedStateChanges.push_back (TestEvent (4, 0, "Dialog (1000,0,1) enter CONFIRMED"));
  m_client1ExpectedStateChanges.push_back (TestEvent (5, 0, "Transaction (1000,1,0) enter TERMINATED"));
  // sends BYE
  m_client1ExpectedStateChanges.push_back (TestEvent (6, 0, "Dialog (1000,0,1) enter TERMINATED"));
  m_client1ExpectedStateChanges.push_back (TestEvent (7, 0, "Transaction (1000,1,0) enter TRYING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (8, 0, "Transaction (1000,1,0) enter COMPLETED"));
  // After Timer K expires, move to TERMINATED
  m_client1ExpectedStateChanges.push_back (TestEvent (9, 0, "Transaction (1000,1,0) enter TERMINATED"));

  // Clients 2  and 3 plays the UAS role in receiving both INVITE and BYE
  // Client 2 receives INVITE, and the transaction ends in CONFIRMED upon ACK
  m_client2ExpectedStateChanges.push_back (TestEvent (0, 0, "Dialog (1000,0,2) enter TRYING"));
  m_client2ExpectedStateChanges.push_back (TestEvent (1, 0, "Transaction (1000,0,2) enter TRYING"));
  m_client2ExpectedStateChanges.push_back (TestEvent (2, 0, "Dialog (1000,0,2) enter CONFIRMED"));
  m_client2ExpectedStateChanges.push_back (TestEvent (3, 0, "Transaction (1000,0,2) enter COMPLETED"));
  m_client2ExpectedStateChanges.push_back (TestEvent (4, 0, "Transaction (1000,0,2) enter CONFIRMED"));
  // After Timer I expires, move to TERMINATED
  m_client2ExpectedStateChanges.push_back (TestEvent (5, 0, "Transaction (1000,0,2) enter TERMINATED"));
  // Client 2 later receives BYE, and the transaction ends up in TERMINATED
  // after Timer J expires
  m_client2ExpectedStateChanges.push_back (TestEvent (6, 0, "Dialog (1000,0,2) enter TERMINATED"));
  m_client2ExpectedStateChanges.push_back (TestEvent (7, 0, "Transaction (1000,0,2) enter TRYING"));
  m_client2ExpectedStateChanges.push_back (TestEvent (8, 0, "Transaction (1000,0,2) enter COMPLETED"));
  m_client2ExpectedStateChanges.push_back (TestEvent (9, 0, "Transaction (1000,0,2) enter TERMINATED"));

  // Client 3's transitions should be identical to Client 2's transitions
  m_client3ExpectedStateChanges.push_back (TestEvent (0, 0, "Dialog (1000,0,3) enter TRYING"));
  m_client3ExpectedStateChanges.push_back (TestEvent (1, 0, "Transaction (1000,0,3) enter TRYING"));
  m_client3ExpectedStateChanges.push_back (TestEvent (2, 0, "Dialog (1000,0,3) enter CONFIRMED"));
  m_client3ExpectedStateChanges.push_back (TestEvent (3, 0, "Transaction (1000,0,3) enter COMPLETED"));
  m_client3ExpectedStateChanges.push_back (TestEvent (4, 0, "Transaction (1000,0,3) enter CONFIRMED"));
  m_client3ExpectedStateChanges.push_back (TestEvent (5, 0, "Transaction (1000,0,3) enter TERMINATED"));
  m_client3ExpectedStateChanges.push_back (TestEvent (6, 0, "Dialog (1000,0,3) enter TERMINATED"));
  m_client3ExpectedStateChanges.push_back (TestEvent (7, 0, "Transaction (1000,0,3) enter TRYING"));
  m_client3ExpectedStateChanges.push_back (TestEvent (8, 0, "Transaction (1000,0,3) enter COMPLETED"));
  m_client3ExpectedStateChanges.push_back (TestEvent (9, 0, "Transaction (1000,0,3) enter TERMINATED"));

  // The proxy plays the role of UAS towards Client 1 and UAC towards 2 and 3
  // Handle the INVITE from client 1
  m_proxyExpectedStateChanges.push_back (TestEvent (0, 0, "Dialog (1000,0,1) enter TRYING"));
  m_proxyExpectedStateChanges.push_back (TestEvent (1, 0, "Transaction (1000,1,0) enter TRYING"));
  m_proxyExpectedStateChanges.push_back (TestEvent (2, 0, "Dialog (1000,0,1) enter PROCEEDING"));
  m_proxyExpectedStateChanges.push_back (TestEvent (3, 0, "Transaction (1000,1,0) enter PROCEEDING"));
  // Send INVITE towards clients 2 and 3; transaction ends in TERMINATED
  m_proxyExpectedStateChanges.push_back (TestEvent (4, 0, "Dialog (1000,0,2) enter TRYING"));
  m_proxyExpectedStateChanges.push_back (TestEvent (5, 0, "Transaction (1000,0,2) enter CALLING"));
  m_proxyExpectedStateChanges.push_back (TestEvent (6, 0, "Dialog (1000,0,3) enter TRYING"));
  m_proxyExpectedStateChanges.push_back (TestEvent (7, 0, "Transaction (1000,0,3) enter CALLING"));
  m_proxyExpectedStateChanges.push_back (TestEvent (8, 0, "Dialog (1000,0,2) enter CONFIRMED"));
  m_proxyExpectedStateChanges.push_back (TestEvent (9, 0, "Transaction (1000,0,2) enter TERMINATED"));
  m_proxyExpectedStateChanges.push_back (TestEvent (10, 0, "Dialog (1000,0,1) enter CONFIRMED"));
  m_proxyExpectedStateChanges.push_back (TestEvent (11, 0, "Transaction (1000,1,0) enter COMPLETED"));
  m_proxyExpectedStateChanges.push_back (TestEvent (12, 0, "Dialog (1000,0,3) enter CONFIRMED"));
  m_proxyExpectedStateChanges.push_back (TestEvent (13, 0, "Transaction (1000,0,3) enter TERMINATED"));
  // upon ACK from 1, enter CONFIRMED state
  // send the 200 OK towards Client 1
  m_proxyExpectedStateChanges.push_back (TestEvent (14, 0, "Transaction (1000,1,0) enter CONFIRMED"));
  // Timer I expires towards UAC (node 1).  This ends INVITE state changes
  m_proxyExpectedStateChanges.push_back (TestEvent (15, 0, "Transaction (1000,1,0) enter TERMINATED"));
  // upon receipt of BYE, the Dialog goes to TERMINATED
  m_proxyExpectedStateChanges.push_back (TestEvent (16, 0, "Dialog (1000,0,1) enter TERMINATED"));
  m_proxyExpectedStateChanges.push_back (TestEvent (17, 0, "Transaction (1000,1,0) enter TRYING"));
  // send OK to client 1 and enter COMPLETED
  m_proxyExpectedStateChanges.push_back (TestEvent (18, 0, "Transaction (1000,1,0) enter COMPLETED"));
  // close the Dialog from 0 to 2 and 0 to 3
  m_proxyExpectedStateChanges.push_back (TestEvent (19, 0, "Dialog (1000,0,2) enter TERMINATED"));
  m_proxyExpectedStateChanges.push_back (TestEvent (20, 0, "Transaction (1000,0,2) enter TRYING"));
  m_proxyExpectedStateChanges.push_back (TestEvent (21, 0, "Dialog (1000,0,3) enter TERMINATED"));
  m_proxyExpectedStateChanges.push_back (TestEvent (22, 0, "Transaction (1000,0,3) enter TRYING"));
  // Non-INVITE client transaction moves to COMPLETED upon 200 OK
  m_proxyExpectedStateChanges.push_back (TestEvent (23, 0, "Transaction (1000,0,2) enter COMPLETED"));
  m_proxyExpectedStateChanges.push_back (TestEvent (24, 0, "Transaction (1000,0,3) enter COMPLETED"));
  // After timers J (towards client 1) and K (towards clients 2 and 3) expire,
  // transactions move to TERMINATED
  m_proxyExpectedStateChanges.push_back (TestEvent (25, 0, "Transaction (1000,0,2) enter TERMINATED"));
  m_proxyExpectedStateChanges.push_back (TestEvent (26, 0, "Transaction (1000,0,3) enter TERMINATED"));
  m_proxyExpectedStateChanges.push_back (TestEvent (27, 0, "Transaction (1000,1,0) enter TERMINATED"));

  Simulator::Stop (Seconds (60));  // greater than 45 seconds to cover Timer J
  Simulator::Run ();
  Simulator::Destroy ();

//#define VERBOSE_DIALOG 1
#ifdef VERBOSE_DIALOG
  // Define this to print out the observed events, if needed for debugging
  // or to generate new expected events.
  for (auto it = m_observedEvents.begin (); it != m_observedEvents.end (); it++)
    {
      std::cout << "  m_expectedEvents.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client1 state changes:" << std::endl;
  for (auto it = m_client1ObservedStateChanges.begin (); it != m_client1ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client1ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client2 state changes:" << std::endl;
  for (auto it = m_client2ObservedStateChanges.begin (); it != m_client2ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client2ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client3 state changes:" << std::endl;
  for (auto it = m_client3ObservedStateChanges.begin (); it != m_client3ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client3ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Proxy state changes:" << std::endl;
  for (auto it = m_proxyObservedStateChanges.begin (); it != m_proxyObservedStateChanges.end (); it++)
    {
      std::cout << "  m_proxyExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
#endif

  // Check that all observed events match expected events
  NS_TEST_ASSERT_MSG_EQ (m_observedEvents.size (), m_expectedEvents.size (), "Did not observe all expected events");
  auto it1 = m_observedEvents.begin ();
  auto it2 = m_expectedEvents.begin ();
  while (it1 != m_observedEvents.end () && it2 != m_expectedEvents.end ())
    {
      if (*it1 != *it2)
        {
          NS_TEST_ASSERT_MSG_EQ (it1->m_step, it2->m_step, "Event steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_userId, it2->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_message, it2->m_message, "Message not equal");
        }
      it1++;
      it2++;
    }
  // Check that all observed state transitions match expected transitions
  NS_TEST_ASSERT_MSG_EQ (m_client1ExpectedStateChanges.size (), m_client1ObservedStateChanges.size (), "Did not observe all expected state changes");
  auto it3 = m_client1ExpectedStateChanges.begin ();
  auto it4 = m_client1ObservedStateChanges.begin ();
  while (it3 != m_client1ExpectedStateChanges.end () && it4 != m_client1ObservedStateChanges.end ())
    {
      if (*it3 != *it4)
        {
          NS_TEST_ASSERT_MSG_EQ (it3->m_step, it4->m_step, "State steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it3->m_userId, it4->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it3->m_message, it4->m_message, "Message not equal");
        }
      it3++;
      it4++;
    }
  NS_TEST_ASSERT_MSG_EQ (m_client2ExpectedStateChanges.size (), m_client2ObservedStateChanges.size (), "Did not observe all expected state changes");
  auto it5 = m_client2ExpectedStateChanges.begin ();
  auto it6 = m_client2ObservedStateChanges.begin ();
  while (it5 != m_client2ExpectedStateChanges.end () && it6 != m_client2ObservedStateChanges.end ())
    {
      if (*it5 != *it6)
        {
          NS_TEST_ASSERT_MSG_EQ (it5->m_step, it6->m_step, "State steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it5->m_userId, it6->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it5->m_message, it6->m_message, "Message not equal");
        }
      it5++;
      it6++;
    }
  NS_TEST_ASSERT_MSG_EQ (m_client3ExpectedStateChanges.size (), m_client3ObservedStateChanges.size (), "Did not observe all expected state changes");
  auto it7 = m_client3ExpectedStateChanges.begin ();
  auto it8 = m_client3ObservedStateChanges.begin ();
  while (it7 != m_client3ExpectedStateChanges.end () && it8 != m_client3ObservedStateChanges.end ())
    {
      if (*it7 != *it8)
        {
          NS_TEST_ASSERT_MSG_EQ (it7->m_step, it8->m_step, "State steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it7->m_userId, it8->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it7->m_message, it8->m_message, "Message not equal");
        }
      it7++;
      it8++;
    }
  NS_TEST_ASSERT_MSG_EQ (m_proxyExpectedStateChanges.size (), m_proxyObservedStateChanges.size (), "Did not observe all expected state changes");
  auto it9 = m_proxyExpectedStateChanges.begin ();
  auto it10 = m_proxyObservedStateChanges.begin ();
  while (it9 != m_proxyExpectedStateChanges.end () && it10 != m_proxyObservedStateChanges.end ())
    {
      if (*it9 != *it10)
        {
          NS_TEST_ASSERT_MSG_EQ (it9->m_step, it10->m_step, "State steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it9->m_userId, it10->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it9->m_message, it10->m_message, "Message not equal");
        }
      it9++;
      it10++;
    }
}

/**
 * \ingroup tests
 * Test the recovery from the loss of initial INVITE from Client 1
 */
class SipInitiatorInviteLossTest : public SipTestCase
{
public:
  SipInitiatorInviteLossTest (void);

private:
  virtual void DoRun (void);
};

SipInitiatorInviteLossTest::SipInitiatorInviteLossTest (void)
  : SipTestCase ("SIP initiator INVITE loss test")
{}

void
SipInitiatorInviteLossTest::DoRun ()
{
  // Add an error model to force the loss of the first INVITE from client 1
  // to the server
  Ptr<ReceiveListErrorModel> em = CreateObject<ReceiveListErrorModel> ();
  std::list<uint32_t> errorList;
  errorList.push_back (0);
  em->SetList (errorList);
  m_serverDeviceTowardsClient1->SetReceiveErrorModel (em);

  // The event is triggered by client 1 sending a SIP INVITE
  Simulator::Schedule (Seconds (1), &SipTestCase::Client1SendSipInvite, this);

  // Expected events
  // Client 1 sends INVITE
  m_expectedEvents.push_back (TestEvent (0, 1, "Client1 send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (1, 1, "Client1 send SIP message"));
  // The message is lost; Timer A expired
  m_expectedEvents.push_back (TestEvent (2, 1, "Client1 Timer A expired"));
  m_expectedEvents.push_back (TestEvent (3, 1, "Client1 send SIP message"));
  // Server receives invite, sends trying to client 1, sends INVITE to others
  m_expectedEvents.push_back (TestEvent (4, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (5, 0, "Server receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (6, 0, "Server send 100 Trying"));
  m_expectedEvents.push_back (TestEvent (7, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (8, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (9, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (10, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (11, 0, "Server send SIP message"));
  // Client 1 receives Trying
  m_expectedEvents.push_back (TestEvent (12, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (13, 1, "Client1 receive 100 Trying"));
  // Clients 2 and 3 receive INVITE and automatically commence
  m_expectedEvents.push_back (TestEvent (14, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (15, 2, "Client2 receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (16, 2, "Client2 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (17, 2, "Client2 send SIP message"));
  m_expectedEvents.push_back (TestEvent (18, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (19, 3, "Client3 receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (20, 3, "Client3 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (21, 3, "Client3 send SIP message"));
  // Server receives the first OK (from client 2), and returns an OK
  // to the originating client
  m_expectedEvents.push_back (TestEvent (22, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (23, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (24, 0, "Server send 200 OK"));
  m_expectedEvents.push_back (TestEvent (25, 0, "Server send SIP message"));
  // Server sends ACK for 200 OK
  m_expectedEvents.push_back (TestEvent (26, 0, "Server send SIP message"));
  // Server receives the second OK from client 3, and returns an ACK
  m_expectedEvents.push_back (TestEvent (27, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (28, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (29, 0, "Server send SIP message"));
  // Client 1 receives OK and sends ACK
  m_expectedEvents.push_back (TestEvent (30, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (31, 1, "Client1 receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (32, 1, "Client1 send SIP message"));
  // Clients 2 and 3 receive ACK
  m_expectedEvents.push_back (TestEvent (33, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (34, 2, "Client2 receive ACK"));
  m_expectedEvents.push_back (TestEvent (35, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (36, 2, "Client2 receive ACK"));
  // Server receives ACK
  m_expectedEvents.push_back (TestEvent (37, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (38, 0, "Server receive ACK"));

  Simulator::Run ();
  Simulator::Destroy ();

// #define VERBOSE_INITIATOR_INVITE_LOSS 1
#ifdef VERBOSE_INITIATOR_INVITE_LOSS
  // Define this to print out the observed events, if needed for debugging
  // or to generate new expected events.
  for (auto it = m_observedEvents.begin (); it != m_observedEvents.end (); it++)
    {
      std::cout << "  m_expectedEvents.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client1 state changes:" << std::endl;
  for (auto it = m_client1ObservedStateChanges.begin (); it != m_client1ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client1ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client2 state changes:" << std::endl;
  for (auto it = m_client2ObservedStateChanges.begin (); it != m_client2ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client2ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client3 state changes:" << std::endl;
  for (auto it = m_client3ObservedStateChanges.begin (); it != m_client3ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client3ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Proxy state changes:" << std::endl;
  for (auto it = m_proxyObservedStateChanges.begin (); it != m_proxyObservedStateChanges.end (); it++)
    {
      std::cout << "  m_proxyExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
#endif

  NS_TEST_ASSERT_MSG_EQ (m_observedEvents.size (), m_expectedEvents.size (), "Did not observe all expected events");
  auto it1 = m_observedEvents.begin ();
  auto it2 = m_expectedEvents.begin ();
  while (it1 != m_observedEvents.end () && it2 != m_expectedEvents.end ())
    {
      if (*it1 != *it2)
        {
          NS_TEST_ASSERT_MSG_EQ (it1->m_step, it2->m_step, "Event steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_userId, it2->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_message, it2->m_message, "Message not equal");
        }
      it1++;
      it2++;
    }

  // Do not replicate DialogTestCase checks on state transitions
}

/**
 * \ingroup tests
 * Test the outcome from the failure of INVITE (all attempts) from Client 1
 */
class SipInitiatorInviteFailureTest : public SipTestCase
{
public:
  SipInitiatorInviteFailureTest (void);

private:
  virtual void DoRun (void);
};

SipInitiatorInviteFailureTest::SipInitiatorInviteFailureTest (void)
  : SipTestCase ("SIP initiator INVITE failure test")
{}

void
SipInitiatorInviteFailureTest::DoRun ()
{
  // Add an error model to force the failure of all INVITEs from client 1
  // to the server
  Ptr<ReceiveListErrorModel> em = CreateObject<ReceiveListErrorModel> ();
  std::list<uint32_t> errorList;
  // 7 losses will lead to a Timer B
  errorList.push_back (0);
  errorList.push_back (1);
  errorList.push_back (2);
  errorList.push_back (3);
  errorList.push_back (4);
  errorList.push_back (5);
  errorList.push_back (6);
  em->SetList (errorList);
  m_serverDeviceTowardsClient1->SetReceiveErrorModel (em);

  // The event is triggered by client 1 sending a SIP INVITE
  Simulator::Schedule (Seconds (1), &SipTestCase::Client1SendSipInvite, this);

  // Expected events
  // Client 1 sends INVITE at time 1 second
  m_expectedEvents.push_back (TestEvent (0, 1, "Client1 send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (1, 1, "Client1 send SIP message"));
  // all retransmissions are lost (sent at 1.5, 2.5, 4.5, 8.5, 16.5,i 32.5s)
  m_expectedEvents.push_back (TestEvent (2, 1, "Client1 Timer A expired"));
  m_expectedEvents.push_back (TestEvent (3, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (4, 1, "Client1 Timer A expired"));
  m_expectedEvents.push_back (TestEvent (5, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (6, 1, "Client1 Timer A expired"));
  m_expectedEvents.push_back (TestEvent (7, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (8, 1, "Client1 Timer A expired"));
  m_expectedEvents.push_back (TestEvent (9, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (10, 1, "Client1 Timer A expired"));
  m_expectedEvents.push_back (TestEvent (11, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (12, 1, "Client1 Timer A expired"));
  m_expectedEvents.push_back (TestEvent (13, 1, "Client1 send SIP message"));
  // At time 33 s (64 * 0.5 sec + 1 sec), Timer B expires
  m_expectedEvents.push_back (TestEvent (14, 1, "Client1 Timer B expired"));
  // No further timer A expiry

  // We expect at time 1s for the dialog to enter TRYING and transaction to
  // enter CALLING
  m_client1ExpectedStateChanges.push_back (TestEvent (0, 0, "Dialog (1000,0,1) enter TRYING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (1, 0, "Transaction (1000,1,0) enter CALLING"));
  // We expect at time 33s for the dialog to enter TERMINATED and transaction
  // to enter FAILED
  m_client1ExpectedStateChanges.push_back (TestEvent (2, 0, "Transaction (1000,1,0) enter FAILED"));
  m_client1ExpectedStateChanges.push_back (TestEvent (3, 0, "Dialog (1000,0,1) enter TERMINATED"));

  Simulator::Run ();
  Simulator::Destroy ();

//#define VERBOSE_INITIATOR_INVITE_FAILURE 1
#ifdef VERBOSE_INITIATOR_INVITE_FAILURE
  // Define this to print out the observed events, if needed for debugging
  // or to generate new expected events.
  for (auto it = m_observedEvents.begin (); it != m_observedEvents.end (); it++)
    {
      std::cout << "  m_expectedEvents.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client1 state changes:" << std::endl;
  for (auto it = m_client1ObservedStateChanges.begin (); it != m_client1ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client1ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
#endif

  NS_TEST_ASSERT_MSG_EQ (m_observedEvents.size (), m_expectedEvents.size (), "Did not observe all expected events");
  auto it1 = m_observedEvents.begin ();
  auto it2 = m_expectedEvents.begin ();
  while (it1 != m_observedEvents.end () && it2 != m_expectedEvents.end ())
    {
      if (*it1 != *it2)
        {
          NS_TEST_ASSERT_MSG_EQ (it1->m_step, it2->m_step, "Event steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_userId, it2->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_message, it2->m_message, "Message not equal");
        }
      it1++;
      it2++;
    }
  // Check that client observed state transitions match expected transitions
  NS_TEST_ASSERT_MSG_EQ (m_client1ExpectedStateChanges.size (), m_client1ObservedStateChanges.size (), "Did not observe all expected state changes");
  auto it3 = m_client1ExpectedStateChanges.begin ();
  auto it4 = m_client1ObservedStateChanges.begin ();
  while (it3 != m_client1ExpectedStateChanges.end () && it4 != m_client1ObservedStateChanges.end ())
    {
      if (*it3 != *it4)
        {
          NS_TEST_ASSERT_MSG_EQ (it3->m_step, it4->m_step, "State steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it3->m_userId, it4->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it3->m_message, it4->m_message, "Message not equal");
        }
      it3++;
      it4++;
    }
}

/**
 * \ingroup tests
 * Test the recovery from the loss of initial BYE from Client 1
 */
class SipInitiatorByeLossTest : public SipTestCase
{
public:
  SipInitiatorByeLossTest (void);

private:
  virtual void DoRun (void);
};

SipInitiatorByeLossTest::SipInitiatorByeLossTest (void)
  : SipTestCase ("SIP initiator BYE loss test")
{}

void
SipInitiatorByeLossTest::DoRun ()
{
  // Add an error model to force the loss of the first BYE from client 1
  // to the server
  Ptr<ReceiveListErrorModel> em = CreateObject<ReceiveListErrorModel> ();
  std::list<uint32_t> errorList;
  errorList.push_back (2); // 0 is INVITE, 1 is ACK of 200
  em->SetList (errorList);
  m_serverDeviceTowardsClient1->SetReceiveErrorModel (em);

  // The event is triggered by client 1 sending a SIP INVITE then later a BYE
  Simulator::Schedule (Seconds (1), &SipTestCase::Client1SendSipInvite, this);
  Simulator::Schedule (Seconds (10), &SipTestCase::Client1SendSipBye, this);

  // Expected events
  // Client 1 sends INVITE
  m_expectedEvents.push_back (TestEvent (0, 1, "Client1 send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (1, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (2, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (3, 0, "Server receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (4, 0, "Server send 100 Trying"));
  m_expectedEvents.push_back (TestEvent (5, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (6, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (7, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (8, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (9, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (10, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (11, 1, "Client1 receive 100 Trying"));
  m_expectedEvents.push_back (TestEvent (12, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (13, 2, "Client2 receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (14, 2, "Client2 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (15, 2, "Client2 send SIP message"));
  m_expectedEvents.push_back (TestEvent (16, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (17, 3, "Client3 receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (18, 3, "Client3 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (19, 3, "Client3 send SIP message"));
  m_expectedEvents.push_back (TestEvent (20, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (21, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (22, 0, "Server send 200 OK"));
  m_expectedEvents.push_back (TestEvent (23, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (24, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (25, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (26, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (27, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (28, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (29, 1, "Client1 receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (30, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (31, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (32, 2, "Client2 receive ACK"));
  m_expectedEvents.push_back (TestEvent (33, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (34, 2, "Client2 receive ACK"));
  m_expectedEvents.push_back (TestEvent (35, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (36, 0, "Server receive ACK"));
  m_expectedEvents.push_back (TestEvent (37, 1, "Client1 send SIP BYE"));
  m_expectedEvents.push_back (TestEvent (38, 1, "Client1 send SIP message"));
  // The message is lost; Timer E expires, and the BYE is sent again
  m_expectedEvents.push_back (TestEvent (39, 1, "Client1 Timer E expired"));
  m_expectedEvents.push_back (TestEvent (40, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (41, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (42, 0, "Server receive SIP BYE"));
  m_expectedEvents.push_back (TestEvent (43, 0, "Server send 200 OK"));
  m_expectedEvents.push_back (TestEvent (44, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (45, 0, "Server send SIP BYE"));
  m_expectedEvents.push_back (TestEvent (46, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (47, 0, "Server send SIP BYE"));
  m_expectedEvents.push_back (TestEvent (48, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (49, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (50, 1, "Client1 receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (51, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (52, 2, "Client2 receive SIP BYE"));
  m_expectedEvents.push_back (TestEvent (53, 2, "Client2 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (54, 2, "Client2 send SIP message"));
  m_expectedEvents.push_back (TestEvent (55, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (56, 3, "Client3 receive SIP BYE"));
  m_expectedEvents.push_back (TestEvent (57, 3, "Client3 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (58, 3, "Client3 send SIP message"));
  m_expectedEvents.push_back (TestEvent (59, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (60, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (61, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (62, 0, "Server receive 200 OK"));

  Simulator::Run ();
  Simulator::Destroy ();

//#define VERBOSE_INITIATOR_BYE_LOSS 1
#ifdef VERBOSE_INITIATOR_BYE_LOSS
  // Define this to print out the observed events, if needed for debugging
  // or to generate new expected events.
  for (auto it = m_observedEvents.begin (); it != m_observedEvents.end (); it++)
    {
      std::cout << "  m_expectedEvents.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client1 state changes:" << std::endl;
  for (auto it = m_client1ObservedStateChanges.begin (); it != m_client1ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client1ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client2 state changes:" << std::endl;
  for (auto it = m_client2ObservedStateChanges.begin (); it != m_client2ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client2ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client3 state changes:" << std::endl;
  for (auto it = m_client3ObservedStateChanges.begin (); it != m_client3ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client3ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Proxy state changes:" << std::endl;
  for (auto it = m_proxyObservedStateChanges.begin (); it != m_proxyObservedStateChanges.end (); it++)
    {
      std::cout << "  m_proxyExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
#endif

  NS_TEST_ASSERT_MSG_EQ (m_observedEvents.size (), m_expectedEvents.size (), "Did not observe all expected events");
  auto it1 = m_observedEvents.begin ();
  auto it2 = m_expectedEvents.begin ();
  while (it1 != m_observedEvents.end () && it2 != m_expectedEvents.end ())
    {
      if (*it1 != *it2)
        {
          NS_TEST_ASSERT_MSG_EQ (it1->m_step, it2->m_step, "Event steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_userId, it2->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_message, it2->m_message, "Message not equal");
        }
      it1++;
      it2++;
    }

  // Do not replicate DialogTestCase checks on per-client state transitions
}

/**
 * \ingroup tests
 * Test the outcome from the failure of BYE (all attempts) from Client 1
 */
class SipInitiatorByeFailureTest : public SipTestCase
{
public:
  SipInitiatorByeFailureTest (void);

private:
  virtual void DoRun (void);
};

SipInitiatorByeFailureTest::SipInitiatorByeFailureTest (void)
  : SipTestCase ("SIP initiator BYE failure test")
{}

void
SipInitiatorByeFailureTest::DoRun ()
{
  // Add an error model to force the failure of all BYEs from client 1
  // to the server
  Ptr<ReceiveListErrorModel> em = CreateObject<ReceiveListErrorModel> ();
  std::list<uint32_t> errorList;
  // 7 losses from message 2 onward (the first BYE) will lead to a Timer F
  errorList.push_back (2);
  errorList.push_back (3);
  errorList.push_back (4);
  errorList.push_back (5);
  errorList.push_back (6);
  errorList.push_back (7);
  errorList.push_back (8);
  em->SetList (errorList);
  m_serverDeviceTowardsClient1->SetReceiveErrorModel (em);

  // The event is triggered by client 1 sending a SIP INVITE then later a BYE
  Simulator::Schedule (Seconds (1), &SipTestCase::Client1SendSipInvite, this);
  Simulator::Schedule (Seconds (10), &SipTestCase::Client1SendSipBye, this);


  // Expected events
  // Client 1 sends INVITE at time 1 second, and first transaction completes
  m_expectedEvents.push_back (TestEvent (0, 1, "Client1 send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (1, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (2, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (3, 0, "Server receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (4, 0, "Server send 100 Trying"));
  m_expectedEvents.push_back (TestEvent (5, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (6, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (7, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (8, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (9, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (10, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (11, 1, "Client1 receive 100 Trying"));
  m_expectedEvents.push_back (TestEvent (12, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (13, 2, "Client2 receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (14, 2, "Client2 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (15, 2, "Client2 send SIP message"));
  m_expectedEvents.push_back (TestEvent (16, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (17, 3, "Client3 receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (18, 3, "Client3 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (19, 3, "Client3 send SIP message"));
  m_expectedEvents.push_back (TestEvent (20, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (21, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (22, 0, "Server send 200 OK"));
  m_expectedEvents.push_back (TestEvent (23, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (24, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (25, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (26, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (27, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (28, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (29, 1, "Client1 receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (30, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (31, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (32, 2, "Client2 receive ACK"));
  m_expectedEvents.push_back (TestEvent (33, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (34, 2, "Client2 receive ACK"));
  m_expectedEvents.push_back (TestEvent (35, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (36, 0, "Server receive ACK"));
  // Client 1 sends BYE at 10 seconds
  // all retransmissions are lost (sent at 10.5, 11.5, 13.5, 17.5, 25.5, 41.5s)
  m_expectedEvents.push_back (TestEvent (37, 1, "Client1 send SIP BYE"));
  m_expectedEvents.push_back (TestEvent (38, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (39, 1, "Client1 Timer E expired"));
  m_expectedEvents.push_back (TestEvent (40, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (41, 1, "Client1 Timer E expired"));
  m_expectedEvents.push_back (TestEvent (42, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (43, 1, "Client1 Timer E expired"));
  m_expectedEvents.push_back (TestEvent (44, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (45, 1, "Client1 Timer E expired"));
  m_expectedEvents.push_back (TestEvent (46, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (47, 1, "Client1 Timer E expired"));
  m_expectedEvents.push_back (TestEvent (48, 1, "Client1 send SIP message"));
  m_expectedEvents.push_back (TestEvent (49, 1, "Client1 Timer E expired"));
  m_expectedEvents.push_back (TestEvent (50, 1, "Client1 send SIP message"));
  // After 6th retransmission, Timer F fires
  m_expectedEvents.push_back (TestEvent (51, 1, "Client1 Timer F expired"));
  // No further timer E expiry

  // We expect at time 1s for the dialog to enter TRYING and transaction to
  // enter CALLING
  m_client1ExpectedStateChanges.push_back (TestEvent (0, 0, "Dialog (1000,0,1) enter TRYING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (1, 0, "Transaction (1000,1,0) enter CALLING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (2, 0, "Dialog (1000,0,1) enter PROCEEDING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (3, 0, "Transaction (1000,1,0) enter PROCEEDING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (4, 0, "Dialog (1000,0,1) enter CONFIRMED"));
  m_client1ExpectedStateChanges.push_back (TestEvent (5, 0, "Transaction (1000,1,0) enter TERMINATED"));
  // We expect at time 10s for the dialog to enter TERMINATED and transaction to
  // enter TRYING, then later to FAILED
  m_client1ExpectedStateChanges.push_back (TestEvent (6, 0, "Dialog (1000,0,1) enter TERMINATED"));
  m_client1ExpectedStateChanges.push_back (TestEvent (7, 0, "Transaction (1000,1,0) enter TRYING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (8, 0, "Transaction (1000,1,0) enter FAILED"));

  Simulator::Run ();
  Simulator::Destroy ();

//#define VERBOSE_INITIATOR_BYE_FAILURE 1
#ifdef VERBOSE_INITIATOR_BYE_FAILURE
  // Define this to print out the observed events, if needed for debugging
  // or to generate new expected events.
  for (auto it = m_observedEvents.begin (); it != m_observedEvents.end (); it++)
    {
      std::cout << "  m_expectedEvents.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client1 state changes:" << std::endl;
  for (auto it = m_client1ObservedStateChanges.begin (); it != m_client1ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client1ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
#endif

  NS_TEST_ASSERT_MSG_EQ (m_observedEvents.size (), m_expectedEvents.size (), "Did not observe all expected events");
  auto it1 = m_observedEvents.begin ();
  auto it2 = m_expectedEvents.begin ();
  while (it1 != m_observedEvents.end () && it2 != m_expectedEvents.end ())
    {
      if (*it1 != *it2)
        {
          NS_TEST_ASSERT_MSG_EQ (it1->m_step, it2->m_step, "Event steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_userId, it2->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_message, it2->m_message, "Message not equal");
        }
      it1++;
      it2++;
    }
  // Check that client observed state transitions match expected transitions
  NS_TEST_ASSERT_MSG_EQ (m_client1ExpectedStateChanges.size (), m_client1ObservedStateChanges.size (), "Did not observe all expected state changes");
  auto it3 = m_client1ExpectedStateChanges.begin ();
  auto it4 = m_client1ObservedStateChanges.begin ();
  while (it3 != m_client1ExpectedStateChanges.end () && it4 != m_client1ObservedStateChanges.end ())
    {
      if (*it3 != *it4)
        {
          NS_TEST_ASSERT_MSG_EQ (it3->m_step, it4->m_step, "State steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it3->m_userId, it4->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it3->m_message, it4->m_message, "Message not equal");
        }
      it3++;
      it4++;
    }
}

/**
 * \ingroup tests
 * Test the recovery from the loss of initial INVITE to Clients 2 and 3
 */
class SipProxyInviteLossTest : public SipTestCase
{
public:
  SipProxyInviteLossTest (void);

private:
  virtual void DoRun (void);
};

SipProxyInviteLossTest::SipProxyInviteLossTest (void)
  : SipTestCase ("SIP proxy INVITE loss test")
{}

void
SipProxyInviteLossTest::DoRun ()
{
  // Add error models to force the loss of the first INVITE from proxy
  // to clients 2 and 3
  Ptr<ReceiveListErrorModel> em2 = CreateObject<ReceiveListErrorModel> ();
  std::list<uint32_t> errorList;
  errorList.push_back (0);
  em2->SetList (errorList);
  m_client2DeviceTowardsServer->SetReceiveErrorModel (em2);
  Ptr<ReceiveListErrorModel> em3 = CreateObject<ReceiveListErrorModel> ();
  em3->SetList (errorList);
  m_client3DeviceTowardsServer->SetReceiveErrorModel (em3);

  // The event is triggered by client 1 sending a SIP INVITE
  Simulator::Schedule (Seconds (1), &SipTestCase::Client1SendSipInvite, this);

  // Expected events
  // Client 1 sends INVITE
  m_expectedEvents.push_back (TestEvent (0, 1, "Client1 send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (1, 1, "Client1 send SIP message"));
  // Server receives invite, sends trying to client 1, sends INVITE to others
  m_expectedEvents.push_back (TestEvent (2, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (3, 0, "Server receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (4, 0, "Server send 100 Trying"));
  m_expectedEvents.push_back (TestEvent (5, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (6, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (7, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (8, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (9, 0, "Server send SIP message"));
  // Client 1 receives Trying
  m_expectedEvents.push_back (TestEvent (10, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (11, 1, "Client1 receive 100 Trying"));
  // Retransmissions occur towards both client 2 and 3 after Timer A
  m_expectedEvents.push_back (TestEvent (12, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (13, 0, "Server send SIP message"));
  // Clients 2 and 3 receive INVITE and automatically commence
  m_expectedEvents.push_back (TestEvent (14, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (15, 2, "Client2 receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (16, 2, "Client2 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (17, 2, "Client2 send SIP message"));
  m_expectedEvents.push_back (TestEvent (18, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (19, 3, "Client3 receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (20, 3, "Client3 send 200 OK"));
  m_expectedEvents.push_back (TestEvent (21, 3, "Client3 send SIP message"));
  // Server receives the first OK (from client 2), and returns an OK
  // to the originating client
  m_expectedEvents.push_back (TestEvent (22, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (23, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (24, 0, "Server send 200 OK"));
  m_expectedEvents.push_back (TestEvent (25, 0, "Server send SIP message"));
  // Server sends ACK for 200 OK
  m_expectedEvents.push_back (TestEvent (26, 0, "Server send SIP message"));
  // Server receives the second OK from client 3, and returns an ACK
  m_expectedEvents.push_back (TestEvent (27, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (28, 0, "Server receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (29, 0, "Server send SIP message"));
  // Client 1 receives OK and sends ACK
  m_expectedEvents.push_back (TestEvent (30, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (31, 1, "Client1 receive 200 OK"));
  m_expectedEvents.push_back (TestEvent (32, 1, "Client1 send SIP message"));
  // Clients 2 and 3 receive ACK
  m_expectedEvents.push_back (TestEvent (33, 2, "Client2 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (34, 2, "Client2 receive ACK"));
  m_expectedEvents.push_back (TestEvent (35, 3, "Client3 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (36, 2, "Client2 receive ACK"));
  // Server receives ACK
  m_expectedEvents.push_back (TestEvent (37, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (38, 0, "Server receive ACK"));

  Simulator::Run ();
  Simulator::Destroy ();

//#define VERBOSE_PROXY_INVITE_LOSS 1
#ifdef VERBOSE_PROXY_INVITE_LOSS
  // Define this to print out the observed events, if needed for debugging
  // or to generate new expected events.
  for (auto it = m_observedEvents.begin (); it != m_observedEvents.end (); it++)
    {
      std::cout << "  m_expectedEvents.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client1 state changes:" << std::endl;
  for (auto it = m_client1ObservedStateChanges.begin (); it != m_client1ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client1ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client2 state changes:" << std::endl;
  for (auto it = m_client2ObservedStateChanges.begin (); it != m_client2ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client2ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client3 state changes:" << std::endl;
  for (auto it = m_client3ObservedStateChanges.begin (); it != m_client3ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client3ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Proxy state changes:" << std::endl;
  for (auto it = m_proxyObservedStateChanges.begin (); it != m_proxyObservedStateChanges.end (); it++)
    {
      std::cout << "  m_proxyExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
#endif

  NS_TEST_ASSERT_MSG_EQ (m_observedEvents.size (), m_expectedEvents.size (), "Did not observe all expected events");
  auto it1 = m_observedEvents.begin ();
  auto it2 = m_expectedEvents.begin ();
  while (it1 != m_observedEvents.end () && it2 != m_expectedEvents.end ())
    {
      if (*it1 != *it2)
        {
          NS_TEST_ASSERT_MSG_EQ (it1->m_step, it2->m_step, "Event steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_userId, it2->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_message, it2->m_message, "Message not equal");
        }
      it1++;
      it2++;
    }

  // Do not replicate DialogTestCase checks on state transitions
}

/**
 * \ingroup tests
 * Test the outcome from the failure of INVITE (all attempts) to clients 2 and 3
 */
class SipProxyInviteFailureTest : public SipTestCase
{
public:
  SipProxyInviteFailureTest (void);

private:
  virtual void DoRun (void);
};

SipProxyInviteFailureTest::SipProxyInviteFailureTest (void)
  : SipTestCase ("SIP proxy INVITE failure test")
{}

void
SipProxyInviteFailureTest::DoRun ()
{
  // Add an error model to force the failure of all INVITEs from client 1
  // to the server
  Ptr<ReceiveListErrorModel> em2 = CreateObject<ReceiveListErrorModel> ();
  std::list<uint32_t> errorList;
  // 7 losses will lead to a Timer B
  errorList.push_back (0);
  errorList.push_back (1);
  errorList.push_back (2);
  errorList.push_back (3);
  errorList.push_back (4);
  errorList.push_back (5);
  errorList.push_back (6);
  em2->SetList (errorList);
  m_client2DeviceTowardsServer->SetReceiveErrorModel (em2);
  Ptr<ReceiveListErrorModel> em3 = CreateObject<ReceiveListErrorModel> ();
  em3->SetList (errorList);
  m_client3DeviceTowardsServer->SetReceiveErrorModel (em3);

  // The event is triggered by client 1 sending a SIP INVITE
  Simulator::Schedule (Seconds (1), &SipTestCase::Client1SendSipInvite, this);

  // Expected events
  // Client 1 sends INVITE at time 1 second
  m_expectedEvents.push_back (TestEvent (0, 1, "Client1 send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (1, 1, "Client1 send SIP message"));
  // Server sends 100 Trying back to client 1, and INVITE to clients 2 and 3
  m_expectedEvents.push_back (TestEvent (2, 0, "Server receive SIP message"));
  m_expectedEvents.push_back (TestEvent (3, 0, "Server receive SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (4, 0, "Server send 100 Trying"));
  m_expectedEvents.push_back (TestEvent (5, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (6, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (7, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (8, 0, "Server send SIP INVITE"));
  m_expectedEvents.push_back (TestEvent (9, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (10, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (11, 1, "Client1 receive 100 Trying"));
  // Server retransmits until Timer B expires (towards both client 2 and 3)
  m_expectedEvents.push_back (TestEvent (12, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (13, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (14, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (15, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (16, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (17, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (18, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (19, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (20, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (21, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (22, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (23, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (24, 0, "Server Timer B expired"));
  m_expectedEvents.push_back (TestEvent (25, 0, "Server Timer B expired"));
  // At this point, we have failed transactions towards client 2 and client 3
  // and the server has issued a 100 TRYING towards client 1
  // In this model, the server should send a 408 (Request Timeout), not a
  // CANCEL, back to client 1
  m_expectedEvents.push_back (TestEvent (26, 0, "Server send SIP message"));
  m_expectedEvents.push_back (TestEvent (27, 0, "Server send 408 Request Timeout"));
  m_expectedEvents.push_back (TestEvent (27, 1, "Client1 receive SIP message"));
  m_expectedEvents.push_back (TestEvent (28, 1, "Client1 receive 408 Request Timeout"));
  // Corresponding client 1 state transitions
  m_client1ExpectedStateChanges.push_back (TestEvent (0, 0, "Dialog (1000,0,1) enter TRYING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (1, 0, "Transaction (1000,1,0) enter CALLING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (2, 0, "Dialog (1000,0,1) enter PROCEEDING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (3, 0, "Transaction (1000,1,0) enter PROCEEDING"));
  m_client1ExpectedStateChanges.push_back (TestEvent (4, 0, "Dialog (1000,0,1) enter TERMINATED"));
  m_client1ExpectedStateChanges.push_back (TestEvent (5, 0, "Transaction (1000,1,0) enter FAILED"));

  Simulator::Run ();
  Simulator::Destroy ();

//#define VERBOSE_PROXY_INVITE_FAILURE 1
#ifdef VERBOSE_PROXY_INVITE_FAILURE
  // Define this to print out the observed events, if needed for debugging
  // or to generate new expected events.
  for (auto it = m_observedEvents.begin (); it != m_observedEvents.end (); it++)
    {
      std::cout << "  m_expectedEvents.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
  std::cout << "Client1 state changes:" << std::endl;
  for (auto it = m_client1ObservedStateChanges.begin (); it != m_client1ObservedStateChanges.end (); it++)
    {
      std::cout << "  m_client1ExpectedStateChanges.push_back (TestEvent (" << it->m_step << ", " << it->m_userId << ", \"" << it->m_message << "\"));" << std::endl;
    }
#endif

  NS_TEST_ASSERT_MSG_EQ (m_observedEvents.size (), m_expectedEvents.size (), "Did not observe all expected events");
  auto it1 = m_observedEvents.begin ();
  auto it2 = m_expectedEvents.begin ();
  while (it1 != m_observedEvents.end () && it2 != m_expectedEvents.end ())
    {
      if (*it1 != *it2)
        {
          NS_TEST_ASSERT_MSG_EQ (it1->m_step, it2->m_step, "Event steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_userId, it2->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it1->m_message, it2->m_message, "Message not equal");
        }
      it1++;
      it2++;
    }
  // Check that client 1 observed state transitions match expected transitions
  NS_TEST_ASSERT_MSG_EQ (m_client1ExpectedStateChanges.size (), m_client1ObservedStateChanges.size (), "Did not observe all expected state changes");
  auto it3 = m_client1ExpectedStateChanges.begin ();
  auto it4 = m_client1ObservedStateChanges.begin ();
  while (it3 != m_client1ExpectedStateChanges.end () && it4 != m_client1ObservedStateChanges.end ())
    {
      if (*it3 != *it4)
        {
          NS_TEST_ASSERT_MSG_EQ (it3->m_step, it4->m_step, "State steps not equal");
          NS_TEST_ASSERT_MSG_EQ (it3->m_userId, it4->m_userId, "User IDs not equal");
          NS_TEST_ASSERT_MSG_EQ (it3->m_message, it4->m_message, "Message not equal");
        }
      it3++;
      it4++;
    }
}

/**
 * \ingroup sip
 * \ingroup tests
 *
 * \brief SIP test suite.  The emphasis of these tests is checking a full
 * dialog in a group call setting, without any message loss, and then
 * checking various message loss cases and how timers handle the losses
 */
class SipTestSuite : public TestSuite
{
public:
  SipTestSuite () : TestSuite ("sip", UNIT)
  {
    // Test a full dialog with no message losses (normal operation)
    AddTestCase (new SipDialogTest, TestCase::QUICK);
    // Test the recovery from the loss of initial INVITE from client 1
    AddTestCase (new SipInitiatorInviteLossTest, TestCase::QUICK);
    // Test the outcome from the failure of INVITE from client 1
    AddTestCase (new SipInitiatorInviteFailureTest, TestCase::QUICK);
    // Test the recovery from the loss of initial BYE from client 1
    AddTestCase (new SipInitiatorByeLossTest, TestCase::QUICK);
    // Test the outcome from the failure of BYE from client 1
    AddTestCase (new SipInitiatorByeFailureTest, TestCase::QUICK);
    // Test the outcome from the loss of INVITEs from proxy to clients 2, 3
    AddTestCase (new SipProxyInviteLossTest, TestCase::QUICK);
    // Test the outcome from the failure of INVITEs from proxy to clients 2, 3
    AddTestCase (new SipProxyInviteFailureTest, TestCase::QUICK);
  }
};

static SipTestSuite g_sipTestSuite; //!< Static variable for test initialization
