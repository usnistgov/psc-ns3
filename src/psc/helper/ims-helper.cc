/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 *
 */

#include <ns3/ims-helper.h>
#include <ns3/log.h>
#include <ns3/node.h>
#include <ns3/internet-stack-helper.h>
#include <ns3/point-to-point-helper.h>
#include <ns3/point-to-point-net-device.h>
#include <ns3/ipv4-address-helper.h>
#include <ns3/net-device-container.h>
#include <ns3/ipv4-interface-container.h>
#include <ns3/ipv4-static-routing-helper.h>
#include <ns3/ipv4-address.h>
#include <ns3/epc-tft.h>
#include <ns3/lte-ue-net-device.h>
#include <ns3/epc-ue-nas.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ImsHelper");

NS_OBJECT_ENSURE_REGISTERED (ImsHelper);

ImsHelper::ImsHelper () 
{
  NS_LOG_FUNCTION (this);
}

ImsHelper::~ImsHelper ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
ImsHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ImsHelper")
    .SetParent<Object> ()
    ;
  return tid;
}

void
ImsHelper::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_pgwNode = 0;
  Object::DoDispose ();
}

void
ImsHelper::ConnectPgw (Ptr<Node> pgw)
{
  NS_LOG_FUNCTION (this << pgw);

  NS_ABORT_MSG_IF (m_imsNode, "Error:  IMS node already created");
  m_imsNode = CreateObject<Node> ();
  InternetStackHelper internet;
  internet.Install (m_imsNode);

  NS_LOG_DEBUG ("IMS node (id " << m_imsNode->GetId () << ") connected to P-GW node (id " << pgw->GetId () << ")");
  m_pgwNode = pgw;

  // Setup point-to-point link to P-GW
  PointToPointHelper p2ph;
  Ipv4AddressHelper ipv4h;
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.001)));
  NetDeviceContainer imsDevices = p2ph.Install (m_pgwNode, m_imsNode);
  m_imsDevice = imsDevices.Get (1)->GetObject<PointToPointNetDevice> ();
  
  // Configure IP addresses across the SGi interface
  ipv4h.SetBase ("15.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer imsIpIfaces = ipv4h.Assign (imsDevices);

  // Configure static route from IMS back to the UEs
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> imsStaticRouting = ipv4RoutingHelper.GetStaticRouting (m_imsNode->GetObject<Ipv4> ());
  imsStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

}

Ptr<Node>
ImsHelper::GetImsNode (void) const
{
  return m_imsNode;
}

Address
ImsHelper::GetImsGmAddress (void) const
{
  Ptr<Ipv4> ipv4 = m_imsNode->GetObject<Ipv4> ();
  NS_ABORT_MSG_UNLESS (ipv4, "No IPv4 interface on IMS node");
  uint32_t numInterfaces = ipv4->GetNInterfaces ();
  // There should only be one non-loopback address on the IMS
  // When IPv6 is supported in the EPC network, support will need to be added
  for (uint32_t i = 0; i < numInterfaces; i++)
    {
      Ipv4InterfaceAddress addr = ipv4->GetAddress (i, 0);
      if (addr.GetLocal () != Ipv4Address::GetLoopback ())
        {
           NS_LOG_DEBUG ("Server address " << addr.GetLocal ());
           return addr.GetLocal ();
        }
    }
  NS_FATAL_ERROR ("Server address not found");
  return Address ();
}

Ptr<PointToPointNetDevice>
ImsHelper::GetImsGmDevice (void) const
{
  return m_imsDevice;
}

} // namespace ns3
