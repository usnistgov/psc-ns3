/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#include "lte-sl-tft.h"
#include "ns3/abort.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlTft");

LteSlTft::LteSlTft (Direction d, AddressType type, Ipv4Address addr, uint32_t remoteL2)
{
  NS_LOG_FUNCTION (this);
  m_direction = d;
  m_hasLocalAddress = ( type == LteSlTft::LOCAL );
  m_hasRemoteAddress = ( type == LteSlTft::REMOTE );
  if (type == LteSlTft::LOCAL)
    {
      m_localAddress = addr;
      m_localMask = Ipv4Mask::GetOnes ();
    }
  else
    {
      m_remoteAddress = addr;
      m_remoteMask = Ipv4Mask::GetOnes ();
    }
  NS_ASSERT_MSG ((remoteL2 & 0xFF000000) == 0, "Group L2 address must be 24 bits");
  m_remoteL2Address = remoteL2;
}

LteSlTft::LteSlTft (Direction d, Ipv4Address addr, uint32_t remoteL2) : LteSlTft (d, LteSlTft::REMOTE, addr, remoteL2) {}

LteSlTft::LteSlTft (Direction d, AddressType type, Ipv4Address addr, Ipv4Mask mask, uint32_t remoteL2)
{
  NS_LOG_FUNCTION (this);
  m_direction = d;
  m_hasLocalAddress = ( type == LteSlTft::LOCAL );
  m_hasRemoteAddress = ( type == LteSlTft::REMOTE );
  if (type == LteSlTft::LOCAL)
    {
      m_localAddress = addr;
      m_localMask = mask;
    }
  else
    {
      m_remoteAddress = addr;
      m_remoteMask = mask;
    }
  NS_ASSERT_MSG ((remoteL2 & 0xFF000000) == 0, "Group L2 address must be 24 bits");
  m_remoteL2Address = remoteL2; 
}

LteSlTft::LteSlTft (Direction d, AddressType type, Ipv6Address addr, uint32_t remoteL2)
{
  NS_LOG_FUNCTION (this);
  m_direction = d;
  m_hasLocalAddress = ( type == LteSlTft::LOCAL );
  m_hasRemoteAddress = ( type == LteSlTft::REMOTE );
  if (type == LteSlTft::LOCAL)
    {
      m_localAddress6 = addr;
      m_localMask6 = Ipv6Prefix::GetOnes ();
    }
  else
    {
      m_remoteAddress6 = addr;
      m_remoteMask6 = Ipv6Prefix::GetOnes ();
    }
  NS_ASSERT_MSG ((remoteL2 & 0xFF000000) == 0, "Group L2 address must be 24 bits");
  m_remoteL2Address = remoteL2;
}

LteSlTft::LteSlTft (Direction d, Ipv6Address addr, uint32_t remoteL2) : LteSlTft (d, LteSlTft::REMOTE, addr, remoteL2) {}

LteSlTft::LteSlTft (Direction d, AddressType type, Ipv6Address addr, Ipv6Prefix prefix, uint32_t remoteL2)
{
  NS_LOG_FUNCTION (this);
  m_direction = d;
  m_hasLocalAddress = ( type == LteSlTft::LOCAL );
  m_hasRemoteAddress = ( type == LteSlTft::REMOTE );
  if (type == LteSlTft::LOCAL)
    {
      m_localAddress6 = addr;
      m_localMask6 = prefix;
    }
  else
    {
      m_remoteAddress6 = addr;
      m_remoteMask6 = prefix;
    }
  NS_ASSERT_MSG ((remoteL2 & 0xFF000000) == 0, "Group L2 address must be 24 bits");
  m_remoteL2Address = remoteL2; 
}

LteSlTft::LteSlTft (Ptr<LteSlTft> tft)
{
  NS_LOG_FUNCTION (this);

  m_direction = tft->m_direction;
  m_hasLocalAddress = tft->m_hasLocalAddress;
  m_hasRemoteAddress = tft->m_hasRemoteAddress;
  m_localAddress = tft->m_localAddress;
  m_localAddress6 = tft->m_localAddress6;
  m_localMask = tft->m_localMask;
  m_localMask6 = tft->m_localMask6;
  m_remoteAddress = tft->m_remoteAddress;
  m_remoteAddress6 = tft->m_remoteAddress6;
  m_remoteMask = tft->m_remoteMask;
  m_remoteMask6 = tft->m_remoteMask6;
  m_remoteL2Address = tft->m_remoteL2Address;
}

bool
LteSlTft::Matches (Ipv4Address la, Ipv4Address ra)
{
  NS_LOG_FUNCTION (this << la << ra);
  bool ok = true;
  //check local address
  ok = m_hasLocalAddress ? m_localMask.IsMatch (la, m_localAddress): true;
  //check remote address
  ok = m_hasRemoteAddress ? m_remoteMask.IsMatch (ra, m_remoteAddress): true;
  return ok;
}

bool
LteSlTft::Matches (Ipv6Address la, Ipv6Address ra)
{
  NS_LOG_FUNCTION (this << la << ra);
  bool ok = true;
  //check local address
  ok = m_hasLocalAddress ? m_localMask6.IsMatch (la, m_localAddress6): true;
  //check remote address
  ok = m_hasRemoteAddress ? m_remoteMask6.IsMatch (ra, m_remoteAddress6): true;
  return ok;
}

bool
LteSlTft::Equals (Ptr<LteSlTft> tft)
{
  NS_LOG_FUNCTION (this);
  bool equals = true;

  if (m_direction != tft->m_direction
      || m_hasLocalAddress != tft->m_hasLocalAddress
      || m_hasRemoteAddress != tft->m_hasRemoteAddress
      || m_localAddress != tft->m_localAddress
      || m_localAddress6 != tft->m_localAddress6
      || m_localMask != tft->m_localMask
      || m_localMask6 != tft->m_localMask6
      || m_remoteAddress != tft->m_remoteAddress
      || m_remoteAddress6 != tft->m_remoteAddress6
      || m_remoteMask != tft->m_remoteMask
      || m_remoteMask6 != tft->m_remoteMask6
      || m_remoteL2Address != tft->m_remoteL2Address)
    {
      equals = false;
    }

  return equals;
}

uint32_t
LteSlTft::GetRemoteL2Address ()
{
  return m_remoteL2Address;
}

bool
LteSlTft::isReceive ()
{
  //receiving if RECEIVE or BIDIRECTIONAL
  return m_direction != LteSlTft::TRANSMIT;
}

bool
LteSlTft::isTransmit ()
{
  //transmitting if TRANSMIT or BIDIRECTIONAL
  return m_direction != LteSlTft::RECEIVE;
}

} // namespace ns3
