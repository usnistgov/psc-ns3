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

LteSlTft::LteSlTft (Direction d, CastType castType, Ipv4Address remoteAddr, uint32_t dstL2Id, bool harqEnabled, Time delayBudget)
 :  m_direction (d),
    m_castType (castType),
    m_remoteAddress (remoteAddr),
    m_dstL2Id (dstL2Id),
    m_harqEnabled (harqEnabled),
    m_delayBudget (delayBudget)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (dstL2Id > 0, "Destination L2 id must be greater than zero");
  NS_ASSERT_MSG ((dstL2Id & 0xFF000000) == 0, "Destination L2 id must be 24 bits");
  m_hasRemoteAddress = true;
  m_remoteMask = Ipv4Mask::GetOnes ();
}

LteSlTft::LteSlTft (Direction d, CastType castType, Ipv6Address remoteAddr, uint32_t dstL2Id, bool harqEnabled, Time delayBudget)
 :  m_direction (d),
    m_castType (castType),
    m_remoteAddress6 (remoteAddr),
    m_dstL2Id (dstL2Id),
    m_harqEnabled (harqEnabled),
    m_delayBudget (delayBudget)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (dstL2Id > 0, "Destination L2 id must be greater than zero");
  NS_ASSERT_MSG ((dstL2Id & 0xFF000000) == 0, "Destination L2 id must be 24 bits");
  m_hasRemoteAddress = true;
  m_remoteMask6 = Ipv6Prefix::GetOnes ();
}

LteSlTft::LteSlTft (Direction d, CastType castType, Ipv4Address remoteAddr, uint32_t dstL2Id)
 :  m_direction (d),
    m_castType (castType),
    m_remoteAddress (remoteAddr),
    m_dstL2Id (dstL2Id)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (dstL2Id > 0, "Destination L2 id must be greater than zero");
  NS_ASSERT_MSG ((dstL2Id & 0xFF000000) == 0, "Destination L2 id must be 24 bits");
  m_harqEnabled = true;
  m_delayBudget = Seconds (0);
  m_hasRemoteAddress = true;
  m_remoteMask = Ipv4Mask::GetOnes ();
}

LteSlTft::LteSlTft (Direction d, CastType castType, Ipv6Address remoteAddr, uint32_t dstL2Id)
 :  m_direction (d),
    m_castType (castType),
    m_remoteAddress6 (remoteAddr),
    m_dstL2Id (dstL2Id)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (dstL2Id > 0, "Destination L2 id must be greater than zero");
  NS_ASSERT_MSG ((dstL2Id & 0xFF000000) == 0, "Destination L2 id must be 24 bits");
  m_harqEnabled = true;
  m_delayBudget = Seconds (0);
  m_hasRemoteAddress = true;
  m_remoteMask6 = Ipv6Prefix::GetOnes ();
}

LteSlTft::LteSlTft (Ptr<LteSlTft> tft)
{
  NS_LOG_FUNCTION (this);
  m_direction = tft->m_direction;
  m_castType = tft->m_castType;
  m_remoteAddress = tft->m_remoteAddress;
  m_remoteAddress6 = tft->m_remoteAddress6;
  m_remoteMask = tft->m_remoteMask;
  m_remoteMask6 = tft->m_remoteMask6;
  m_dstL2Id = tft->m_dstL2Id;
  m_harqEnabled = tft->m_harqEnabled;
  m_delayBudget = tft->m_delayBudget;
}

bool
LteSlTft::Matches (Ipv4Address ra) const
{
  NS_LOG_FUNCTION (this << ra);
  bool ok = false;
  //check remote address
  ok = m_hasRemoteAddress ? m_remoteMask.IsMatch (ra, m_remoteAddress) : true;
  return ok;
}

bool
LteSlTft::Matches (Ipv6Address ra) const
{
  NS_LOG_FUNCTION (this << ra);
  bool ok = false;
  //check remote address
  ok = m_hasRemoteAddress ? m_remoteMask6.IsMatch (ra, m_remoteAddress6) : true;
  return ok;
}

bool
LteSlTft::Equals (Ptr<LteSlTft> tft) const
{
  NS_LOG_FUNCTION (this);
  bool equals = true;

  if (m_direction != tft->m_direction
      || m_castType != tft->m_castType
      || m_remoteAddress != tft->m_remoteAddress
      || m_remoteAddress6 != tft->m_remoteAddress6
      || m_remoteMask != tft->m_remoteMask
      || m_remoteMask6 != tft->m_remoteMask6
      || m_dstL2Id != tft->m_dstL2Id
      || m_harqEnabled != tft->m_harqEnabled
      || m_delayBudget != tft->m_delayBudget)
    {
      equals = false;
    }

  return equals;
}

uint32_t
LteSlTft::GetDstL2Id() const
{
  return m_dstL2Id;
}

bool
LteSlTft::IsReceive () const
{
  //receiving if RECEIVE or BIDIRECTIONAL
  NS_ASSERT_MSG (m_direction != LteSlTft::Direction::INVALID, "Invalid TFT direction");
  return m_direction != LteSlTft::Direction::TRANSMIT;
}

bool
LteSlTft::IsTransmit () const
{
  //transmitting if TRANSMIT or BIDIRECTIONAL
  NS_ASSERT_MSG (m_direction != LteSlTft::Direction::INVALID, "Invalid TFT direction");
  return m_direction != LteSlTft::Direction::RECEIVE;
}

LteSlTft::CastType
LteSlTft::GetCastType () const
{
  NS_ASSERT_MSG (m_castType != LteSlTft::CastType::Invalid, "Invalid TFT communication type");
  return m_castType;
}

bool
LteSlTft::IsHarqEnabled () const
{
  return m_harqEnabled;
}

Time
LteSlTft::GetDelayBudget () const
{
  return m_delayBudget;
}

} // namespace ns3
