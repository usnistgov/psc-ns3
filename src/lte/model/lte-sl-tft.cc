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

LteSlTft::LteSlTft (Direction d, Ipv4Address groupIp, uint32_t groupL2)
{
  NS_LOG_FUNCTION (this);
  m_direction = d;
  m_groupAddress = groupIp;
  NS_ASSERT_MSG ((groupL2 & 0xFF000000) == 0, "Group L2 address must be 24 bits");
  m_groupL2Address = groupL2;
}

LteSlTft::LteSlTft (Direction d, Ipv6Address groupIp6, uint32_t groupL2)
{
  NS_LOG_FUNCTION (this);
  m_direction = d;
  m_groupAddress6 = groupIp6;
  NS_ASSERT_MSG ((groupL2 & 0xFF000000) == 0, "Group L2 address must be 24 bits");
  m_groupL2Address = groupL2;
}

LteSlTft::LteSlTft (Direction d, Ipv4Address groupIp, Ipv6Address groupIp6, uint32_t groupL2)
{
  NS_LOG_FUNCTION (this);
  m_direction = d;
  m_groupAddress = groupIp;
  m_groupAddress6 = groupIp6;
  NS_ASSERT_MSG ((groupL2 & 0xFF000000) == 0, "Group L2 address must be 24 bits");
  m_groupL2Address = groupL2;
}

Ptr<LteSlTft>
LteSlTft::Copy ()
{
  Ptr<LteSlTft> tft = Create<LteSlTft> (m_direction, m_groupAddress, m_groupAddress6, m_groupL2Address);
  return tft;
}

/**
 *
 * \param remoteAddress
 *
 * \return true if the TFT matches with the
 * parameters, false otherwise.
 */
bool
LteSlTft::Matches (Ipv4Address remoteAddress)
{
  return remoteAddress == m_groupAddress;

}

/**
 *
 * \param remoteAddress6
 *
 * \return true if the TFT matches with the
 * parameters, false otherwise.
 */
bool
LteSlTft::Matches (Ipv6Address remoteAddress6)
{
  return remoteAddress6 == m_groupAddress6;

}

uint32_t
LteSlTft::GetGroupL2Address ()
{
  return m_groupL2Address;
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

Ipv4Address
LteSlTft::GetGroupAddress ()
{
  return m_groupAddress;
}

Ipv6Address
LteSlTft::GetGroupAddress6 ()
{
  return m_groupAddress6;
}

}
