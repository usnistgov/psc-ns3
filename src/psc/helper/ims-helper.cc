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
#include <ns3/boolean.h>
#include <ns3/log.h>
#include <ns3/node.h>
#include <ns3/internet-stack-helper.h>
#include <ns3/point-to-point-helper.h>
#include <ns3/point-to-point-net-device.h>
#include <ns3/ipv4-address-helper.h>
#include <ns3/ipv6-address-helper.h>
#include <ns3/net-device-container.h>
#include <ns3/ipv4-interface-container.h>
#include <ns3/ipv6-interface-container.h>
#include <ns3/ipv4-static-routing-helper.h>
#include <ns3/ipv6-static-routing-helper.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ImsHelper");

namespace psc {

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
  static TypeId tid = TypeId ("ns3::psc::ImsHelper")
    .SetParent<Object> ()
    .AddAttribute ("UseIPv6",
                   "The flag that indicates if IPv6 should be used.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&ImsHelper::m_useIpv6),
                   MakeBooleanChecker ())
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
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.001)));
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("1Gbps")));
  NetDeviceContainer imsDevices = p2ph.Install (m_pgwNode, m_imsNode);
  m_imsDevice = imsDevices.Get (1)->GetObject<PointToPointNetDevice> ();

  if (!m_useIpv6)
    {
      Ipv4AddressHelper ipv4h;
      // Configure IP addresses across the SGi interface
      ipv4h.SetBase (m_ipv4BaseAddress, m_ipv4BaseMask);
      Ipv4InterfaceContainer imsIpIfaces = ipv4h.Assign (imsDevices);

      // Configure static route from IMS back to the UEs
      Ipv4StaticRoutingHelper ipv4RoutingHelper;
      Ptr<Ipv4StaticRouting> imsStaticRouting = ipv4RoutingHelper.GetStaticRouting (m_imsNode->GetObject<Ipv4> ());
      imsStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);
    }
  else
    {
      Ipv6AddressHelper ipv6h;
      // Configure IP addresses across the SGi interface
      ipv6h.SetBase (m_ipv6BaseAddress, m_ipv6BasePrefix);
      Ipv6InterfaceContainer imsIpIfaces = ipv6h.Assign (imsDevices);

      imsIpIfaces.SetForwarding (0, true);
      imsIpIfaces.SetDefaultRouteInAllNodes (0);

      Ipv6StaticRoutingHelper ipv6RoutingHelper;
      Ptr<Ipv6StaticRouting> imsStaticRouting = ipv6RoutingHelper.GetStaticRouting (m_imsNode->GetObject<Ipv6> ());
      imsStaticRouting->AddNetworkRouteTo ("7777:f000::", Ipv6Prefix (60), imsIpIfaces.GetAddress (0, 1), 1, 0);
    }
}

Ptr<Node>
ImsHelper::GetImsNode (void) const
{
  return m_imsNode;
}

Address
ImsHelper::GetImsGmAddress (void) const
{
  if (!m_useIpv6)
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
    }
  else
    {
      Ptr<Ipv6> ipv6 = m_imsNode->GetObject<Ipv6> ();
      NS_ABORT_MSG_UNLESS (ipv6, "No IPv6 interface on IMS node");
      Ipv6InterfaceAddress addr = ipv6->GetAddress (1, 1);
      return addr.GetAddress ();
     }
  NS_FATAL_ERROR ("Server address not found");
  return Address ();
}

Ptr<PointToPointNetDevice>
ImsHelper::GetImsGmDevice (void) const
{
  return m_imsDevice;
}

void
ImsHelper::SetImsIpv4Network (Ipv4Address baseAddress, Ipv4Mask baseMask)
{
  NS_LOG_FUNCTION (this << baseAddress << baseMask);
  m_ipv4BaseAddress = baseAddress;
  m_ipv4BaseMask = baseMask;
}

void
ImsHelper::SetImsIpv6Network (Ipv6Address baseAddress, Ipv6Prefix basePrefix)
{
  NS_LOG_FUNCTION (this << baseAddress << basePrefix);
  m_ipv6BaseAddress = baseAddress;
  m_ipv6BasePrefix = basePrefix;
}

} // namespace psc
} // namespace ns3
