/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 * Modified by: NIST
 */
#include "nist-udp-echo-helper.h"
#include "ns3/nist-udp-echo-server.h"
#include "ns3/nist-udp-echo-client.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

NistUdpEchoServerHelper::NistUdpEchoServerHelper (uint16_t port)
{
  m_factory.SetTypeId (NistUdpEchoServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
}

void 
NistUdpEchoServerHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
NistUdpEchoServerHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
NistUdpEchoServerHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
NistUdpEchoServerHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
NistUdpEchoServerHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<NistUdpEchoServer> ();
  node->AddApplication (app);

  return app;
}

NistUdpEchoClientHelper::NistUdpEchoClientHelper (Address address, uint16_t port)
{
  m_factory.SetTypeId (NistUdpEchoClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("RemotePort", UintegerValue (port));
}

NistUdpEchoClientHelper::NistUdpEchoClientHelper (Ipv4Address address, uint16_t port)
{
  m_factory.SetTypeId (NistUdpEchoClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (Address(address)));
  SetAttribute ("RemotePort", UintegerValue (port));
}

NistUdpEchoClientHelper::NistUdpEchoClientHelper (Ipv6Address address, uint16_t port)
{
  m_factory.SetTypeId (NistUdpEchoClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (Address(address)));
  SetAttribute ("RemotePort", UintegerValue (port));
}

void 
NistUdpEchoClientHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

void
NistUdpEchoClientHelper::SetFill (Ptr<Application> app, std::string fill)
{
  app->GetObject<NistUdpEchoClient>()->SetFill (fill);
}

void
NistUdpEchoClientHelper::SetFill (Ptr<Application> app, uint8_t fill, uint32_t dataLength)
{
  app->GetObject<NistUdpEchoClient>()->SetFill (fill, dataLength);
}

void
NistUdpEchoClientHelper::SetFill (Ptr<Application> app, uint8_t *fill, uint32_t fillLength, uint32_t dataLength)
{
  app->GetObject<NistUdpEchoClient>()->SetFill (fill, fillLength, dataLength);
}

ApplicationContainer
NistUdpEchoClientHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
NistUdpEchoClientHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
NistUdpEchoClientHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
NistUdpEchoClientHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<NistUdpEchoClient> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3
