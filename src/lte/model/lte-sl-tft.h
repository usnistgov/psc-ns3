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

#ifndef LTE_SL_TFT_H
#define LTE_SL_TFT_H


#include <ns3/simple-ref-count.h>
#include <ns3/ipv4-address.h>
#include <ns3/ipv6-address.h>

#include <list>

namespace ns3 {

/**
 *
 * \brief  Traffic flow template used by sidelink bearers.
 *
 * This class implements a variant of LTE TFT for sidelink
 *
 */
class LteSlTft : public SimpleRefCount<LteSlTft>
{
public:
  /**
   * Indicates the direction of the traffic that is to be classified.
   */
  enum Direction
  {
    TRANSMIT = 1,
    RECEIVE = 2,
    BIDIRECTIONAL = 3
  };

  /**
   * Constructor
   * \param d The direction
   * \param groupIp The group IPv4 address
   * \param groupL2 The group layer 2 address
   */
  LteSlTft (Direction d, Ipv4Address groupIp, uint32_t groupL2);

  /**
   * Constructor
   * \param d The direction
   * \param groupIp6 The group IPv6 address
   * \param groupL2 The group layer 2 address
   */
  LteSlTft (Direction d, Ipv6Address groupIp6, uint32_t groupL2);

  /**
   * Constructor
   * \param d The direction
   * \param groupIp The group IPv4 address
   * \param groupIp6 The group IPv6 address
   * \param groupL2 The group layer 2 address
   */
  LteSlTft (Direction d, Ipv4Address groupIp, Ipv6Address groupIp6, uint32_t groupL2);

  /**
   * Clone the TFT
   * \return a copy of this SLTFT
   */
  Ptr<LteSlTft> Copy ();

  /**
   * Function to evaluate if the SL TFT matches the remote IPv4 address
   * \param remoteAddress
   *
   * \return true if the TFT matches with the
   * parameters, false otherwise.
   */
  bool Matches (Ipv4Address remoteAddress);

  /**
   * Function to evaluate if the SL TFT matches the remote IPv6 address
   * \param remoteAddress6
   *
   * \return true if the TFT matches with the
   * parameters, false otherwise.
   */
  bool Matches (Ipv6Address remoteAddress6);

  /**
   * Gets the Group L2 address associated with the TFT
   * \return the Group L2 address associated with the TFT
   */
  uint32_t GetGroupL2Address ();

  /**
   * Gets the Group IPv4 address associated with the TFT
   * \return the Group IPv4 address associated with the TFT
   */
  Ipv4Address GetGroupAddress ();

  /**
   * Gets the Group IPv6 address associated with the TFT
   * \return the Group IPv6 address associated with the TFT
   */
  Ipv6Address GetGroupAddress6 ();

  /**
   * Indicates if the TFT is for an incoming sidelink bearer
   * \return true if the TFT is for an incoming sidelink bearer
   */
  bool isReceive ();

  /**
   * Indicates if the TFT is for an outgoing sidelink bearer
   * \return true if the TFT is for an outgoing sidelink bearer
   */
  bool isTransmit ();

private:
  Direction m_direction; ///< whether the filter needs to be applied
  ///< to uplink / downlink only, or in both cases*/

  Ipv4Address m_groupAddress;     ///< IPv4 address of the remote host

  Ipv6Address m_groupAddress6;    ///< IPv6 address of the remote host

  uint32_t m_groupL2Address;      ///< 24 bit MAC address
};

} // namespace ns3

#endif /* LTE_SL_TFT_H */
