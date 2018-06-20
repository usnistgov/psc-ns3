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
#include "psc-udp-groupecho-helper.h"
#include "ns3/psc-udp-groupecho-server.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"
#include "ns3/boolean.h"

namespace ns3 {

PscUdpGroupEchoServerHelper::PscUdpGroupEchoServerHelper (uint16_t port)
{
  m_factory.SetTypeId (PscUdpGroupEchoServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
}

  PscUdpGroupEchoServerHelper::PscUdpGroupEchoServerHelper (uint16_t port, double stime)
{
  m_factory.SetTypeId (PscUdpGroupEchoServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
  SetAttribute ("Timeout", DoubleValue (stime));
}

  PscUdpGroupEchoServerHelper::PscUdpGroupEchoServerHelper (uint16_t port, double stime, bool echoback)
{
  m_factory.SetTypeId (PscUdpGroupEchoServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
  SetAttribute ("Timeout", DoubleValue (stime));
  SetAttribute ("Echo", BooleanValue (echoback));
}

void 
PscUdpGroupEchoServerHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
PscUdpGroupEchoServerHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
PscUdpGroupEchoServerHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
PscUdpGroupEchoServerHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
PscUdpGroupEchoServerHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<PscUdpGroupEchoServer> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3
