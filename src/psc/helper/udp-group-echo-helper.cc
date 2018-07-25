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
 * Modified by: NIST // Contributions may not be subject to US copyright.
 *
 * This application helper, authored by Fernando J. Cintron <fernando.cintron@nist.gov>
 * is derived from udp-echo-helper.cc originally authored by
 * Mathieu Lacage <mathieu.lacage@sophia.inria.fr>.
 */

#include "udp-group-echo-helper.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"
#include "ns3/boolean.h"
#include "ns3/enum.h"

namespace ns3 {
namespace psc {

UdpGroupEchoServerHelper::UdpGroupEchoServerHelper (uint16_t port)
{
  m_factory.SetTypeId (UdpGroupEchoServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
}

UdpGroupEchoServerHelper::UdpGroupEchoServerHelper (uint16_t port, Time expirationTime)
{
  m_factory.SetTypeId (UdpGroupEchoServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
  SetAttribute ("Timeout", TimeValue (expirationTime));
}

UdpGroupEchoServerHelper::UdpGroupEchoServerHelper (uint16_t port, Time expirationTime, UdpGroupEchoServer::Mode_t mode, bool echoClient)
{
  m_factory.SetTypeId (UdpGroupEchoServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
  SetAttribute ("Timeout", TimeValue (expirationTime));
  SetAttribute ("Mode", EnumValue (mode));
  SetAttribute ("EchoClient", BooleanValue (echoClient));
}

void
UdpGroupEchoServerHelper::SetAttribute (
  std::string name,
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
UdpGroupEchoServerHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
UdpGroupEchoServerHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
UdpGroupEchoServerHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
UdpGroupEchoServerHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<UdpGroupEchoServer> ();
  node->AddApplication (app);

  return app;
}

} // namespace psc
} // namespace ns3
