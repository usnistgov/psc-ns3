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

#ifndef IMS_HELPER_H
#define IMS_HELPER_H

#include <ns3/object.h>
#include <ns3/ipv4-address.h>
#include <ns3/ipv6-address.h>
#include <ns3/point-to-point-net-device.h>

namespace ns3 {

class Node;

namespace psc {

/**
 * \ingroup psc
 *
 * \brief Helper class to handle IMS creation
 */
class ImsHelper : public Object
{
public:
  
  /** 
   * Constructor
   */
  ImsHelper ();

  /** 
   * Destructor
   */  
  virtual ~ImsHelper ();
  
  // inherited from Object
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  // inherited from Object
  virtual void DoDispose ();

  /**
   * This method should be called once per simulation.  It creates an IMS
   * node, adds an Internet stack to it, connects the PGW to it with a 
   * point-to-point link, assigns IP addresses to the link, and installs
   * a default route in the IMS node to reach the UE network.
   *
   * \param pgwNode PGW node pointer
   */
  void ConnectPgw (Ptr<Node> pgwNode);

  /**
   *
   * \return a pointer to the node implementing IMS functionality.
   */
  Ptr<Node> GetImsNode () const;

  /**
   * Return the address of the IMS that connects to the UE over the Gm
   * reference point.   This should be the destination address for packets
   * sent to the CSCF (IMS).
   *
   * \return Address of the IMS at Gm reference point; reachable by UEs
   */
  Address GetImsGmAddress (void) const;

  /**
   * Return the device pointer for the device connecting the IMS to the PGW
   * at the Gm reference point.
   *
   * \return Device pointer of IMS device at Gm reference point
   */
  Ptr<PointToPointNetDevice> GetImsGmDevice (void) const;

  /**
   * Override default IPv4 network configuration for IMS network
   * \param baseAddress base IPv4 address
   * \param baseMask base IPv4 mask
   */
  void SetImsIpv4Network (Ipv4Address baseAddress, Ipv4Mask baseMask);

  /**
   * Override default IPv6 network configuration for IMS network
   * \param baseAddress base IPv6 address
   * \param basePrefix base IPv6 prefix
   */
  void SetImsIpv6Network (Ipv6Address baseAddress, Ipv6Prefix basePrefix);

private:
  bool m_useIpv6; //!< A flag that indicates if IPv6 should be used.
  Ptr<Node> m_imsNode;
  Ipv4Address m_ipv4BaseAddress {"15.0.0.0"};
  Ipv4Mask m_ipv4BaseMask {"255.0.0.0"};
  Ipv6Address m_ipv6BaseAddress {"6001:db80::"};
  Ipv6Prefix m_ipv6BasePrefix {64};
  Ptr<Node> m_pgwNode;
  Ptr<PointToPointNetDevice> m_imsDevice;
};

} // namespace psc
} // namespace ns3

#endif // IMS_HELPER_H
