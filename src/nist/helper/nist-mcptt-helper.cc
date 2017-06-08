/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * 2016 National Institute of Standards and Technology (NIST)
 *
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
 *
 * Author: Wesley Garey <wesley.garey@nist.gov>
 */

#include <stdint.h>
#include <string>

#include <ns3/address.h>
#include <ns3/data-rate.h>
#include <ns3/double.h>
#include <ns3/names.h>
#include <ns3/nist-mcptt-floor-machine.h>
#include <ns3/nist-mcptt-floor-msg-parser.h>
#include <ns3/nist-mcptt-ptt-app.h>
#include <ns3/nist-mcptt-pusher.h>
#include <ns3/nist-mcptt-send-requester.h>
#include <ns3/packet-socket-address.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/string.h>
#include <ns3/udp-socket-factory.h>
#include <ns3/uinteger.h>

#include "nist-mcptt-helper.h"


namespace ns3 {

NistMcpttHelper::NistMcpttHelper ()
  : m_appFac (ObjectFactory ()),
    m_floorFac (ObjectFactory ()),
    m_msgParserFac (ObjectFactory ()),
    m_nextUserId (1),
    m_requesterFac (ObjectFactory ()),
    m_pusherFac (ObjectFactory ())
{ 
  this->SetAppTid (NistMcpttPttApp::GetTypeId ());
  this->SetFloorMachineTid (NistMcpttFloorMachine::GetTypeId ());
  this->SetMsgParserTid (NistMcpttMsgParser::GetTypeId ());
  this->SetPusherTid (NistMcpttPusher::GetTypeId ());
  this->SetRequesterTid (NistMcpttSendRequester::GetTypeId ());

  this->SetAppAttribute ("LocalPort", UintegerValue (49153));
  this->SetAppAttribute ("PeerAddress", AddressValue (Ipv4Address ("255.255.255.255")));
  this->SetAppAttribute ("PeerPort", UintegerValue (49153));
  this->SetAppAttribute ("SocketFactoryTid", TypeIdValue (UdpSocketFactory::GetTypeId ()));
  this->SetRequesterAttribute ("Bytes", UintegerValue (1));
  this->SetRequesterAttribute ("DataRate", DataRateValue (DataRate ("8b/s")));
  this->SetPusherAttribute ("Rnd", PointerValue (CreateObjectWithAttributes<NormalRandomVariable> ("Mean", DoubleValue(5.0), "Variance", DoubleValue (2.0))));

}

NistMcpttHelper::NistMcpttHelper (const Address& peerAddress, const uint16_t& peerPort)
  : m_appFac (ObjectFactory ()),
    m_floorFac (ObjectFactory ()),
    m_msgParserFac (ObjectFactory ()),
    m_nextUserId (1),
    m_requesterFac (ObjectFactory ()),
    m_pusherFac (ObjectFactory ())
{ 
  this->SetAppTid (NistMcpttPttApp::GetTypeId ());
  this->SetFloorMachineTid (NistMcpttFloorMachine::GetTypeId ());
  this->SetMsgParserTid (NistMcpttMsgParser::GetTypeId ());
  this->SetPusherTid (NistMcpttPusher::GetTypeId ());
  this->SetRequesterTid (NistMcpttSendRequester::GetTypeId ());

  this->SetAppAttribute ("LocalPort", UintegerValue (49153));
  this->SetAppAttribute ("PeerAddress", AddressValue (peerAddress));
  this->SetAppAttribute ("PeerPort", UintegerValue (peerPort));
  this->SetAppAttribute ("SocketFactoryTid", TypeIdValue (UdpSocketFactory::GetTypeId ()));
  this->SetRequesterAttribute ("Bytes", UintegerValue (1));
  this->SetRequesterAttribute ("DataRate", DataRateValue (DataRate ("8b/s")));
  this->SetPusherAttribute ("Rnd", PointerValue (CreateObjectWithAttributes<NormalRandomVariable> ("Mean", DoubleValue(5.0), "Variance", DoubleValue (2.0))));
}

NistMcpttHelper::~NistMcpttHelper ()
{ }

ApplicationContainer
NistMcpttHelper::Install (const Ptr<Node>& node)
{
  return ApplicationContainer (this->InstallPriv (node));
}

ApplicationContainer
NistMcpttHelper::Install (const std::string& nodeName)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  
  return ApplicationContainer (this->InstallPriv (node));
}

ApplicationContainer
NistMcpttHelper::Install (const NodeContainer& c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (this->InstallPriv (*i));
    }

  return apps;
}

void
NistMcpttHelper::SetAppAttribute (const std::string& name, const AttributeValue& value)
{
  this->m_appFac.Set (name, value);
}

void
NistMcpttHelper::SetAppTid (const TypeId& appTid)
{
  TypeId pttAppTid = NistMcpttPttApp::GetTypeId ();

  NS_ASSERT_MSG ((appTid == pttAppTid || appTid.IsChildOf (pttAppTid)), "The given type ID is not that of a " << pttAppTid.GetName () << ".");

  this->m_appFac.SetTypeId (appTid);
}

void
NistMcpttHelper::SetFloorMachineAttribute (const std::string& name, const AttributeValue& value)
{
  this->m_floorFac.Set (name, value);
}

void
NistMcpttHelper::SetFloorMachineTid (const TypeId& floorTid)
{
  TypeId floorMachineTid = NistMcpttFloorMachine::GetTypeId ();

  NS_ASSERT_MSG ((floorTid == floorMachineTid || floorTid.IsChildOf (floorMachineTid)), "The given type ID is not that of a " << floorMachineTid.GetName () << ".");

  this->m_floorFac.SetTypeId (floorTid);
}

void
NistMcpttHelper::SetMsgParserAttribute (const std::string& name, const AttributeValue& value)
{
  this->m_msgParserFac.Set (name, value);
}

void
NistMcpttHelper::SetMsgParserTid (const TypeId& parserTid)
{
  TypeId mcpttParserTid = NistMcpttMsgParser::GetTypeId ();

  NS_ASSERT_MSG ((parserTid == mcpttParserTid || parserTid.IsChildOf (mcpttParserTid)), "The given type ID is not that of a " << mcpttParserTid.GetName () << ".");

  this->m_msgParserFac.SetTypeId (parserTid);
}

void
NistMcpttHelper::SetPusherAttribute (const std::string& name, const AttributeValue& value)
{
  this->m_pusherFac.Set (name, value);
}

void
NistMcpttHelper::SetPusherTid (const TypeId& pusherTid)
{
  TypeId mcpttPusherTid = NistMcpttPusher::GetTypeId ();

  NS_ASSERT_MSG ((pusherTid == mcpttPusherTid || pusherTid.IsChildOf (mcpttPusherTid)), "The given type ID is not that of a " << mcpttPusherTid.GetName () << ".");

  this->m_pusherFac.SetTypeId (pusherTid);
}

void
NistMcpttHelper::SetRequesterAttribute (const std::string& name, const AttributeValue& value)
{
  this->m_requesterFac.Set (name, value);
}

void
NistMcpttHelper::SetRequesterTid (const TypeId& requesterTid)
{
  TypeId mcpttRequesterTid = NistMcpttSendRequester::GetTypeId ();

  NS_ASSERT_MSG ((mcpttRequesterTid == requesterTid || requesterTid.IsChildOf (mcpttRequesterTid)), "The given type ID is not that of a " << mcpttRequesterTid.GetName () << ".");

  this->m_requesterFac.SetTypeId (requesterTid);
}

Ptr<Application>
NistMcpttHelper::InstallPriv (const Ptr<Node>& node)
{
  uint32_t userId = this->NextUserId ();

  Ptr<NistMcpttPttApp> app = this->m_appFac.Create<NistMcpttPttApp> ();
  Ptr<NistMcpttFloorMachine> floorMachine = this->m_floorFac.Create<NistMcpttFloorMachine> ();
  Ptr<NistMcpttMsgParser> msgParser = this->m_msgParserFac.Create<NistMcpttMsgParser> ();
  Ptr<NistMcpttSendRequester> requester = this->m_requesterFac.Create<NistMcpttSendRequester> ();
  Ptr<NistMcpttPusher> pusher = this->m_pusherFac.Create<NistMcpttPusher> ();

  app->SetUserId (userId);
  app->SetDataSrc (requester);
  app->SetFloorMachine (floorMachine);
  app->SetMsgParser (msgParser);
  app->SetPusher (pusher);

  node->AddApplication (app);

  return app;
}

uint32_t
NistMcpttHelper::NextUserId (void)
{
  uint32_t current = this->GetNextUserId ();
  uint32_t next = current + 1;

  this->SetNextUserId (next);

  return current;
}

uint32_t
NistMcpttHelper::GetNextUserId (void) const
{
  return this->m_nextUserId;
}

void
NistMcpttHelper::SetNextUserId (const uint32_t& nextUserId)
{
  this->m_nextUserId = nextUserId;
}

} // namespace ns3

