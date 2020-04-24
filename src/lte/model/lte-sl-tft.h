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
   * Indicates if the address provided is for local or remote in case
   * the TFT does not use both
   */
  enum AddressType
  {
    LOCAL = 1,
    REMOTE = 2
  };

  /**
   * Constructor
   * \param d The direction
   * \param type The type of address provided
   * \param addr The IPv4 address
   * \param remoteL2 The group layer 2 address
   */
  LteSlTft (Direction d, AddressType type, Ipv4Address addr, uint32_t remoteL2);

/**
   * Constructor (sets remote address only, defined for backward compatibility)
   * \param d The direction
   * \param addr The IPv4 address
   * \param remoteL2 The group layer 2 address
   */
  LteSlTft (Direction d, Ipv4Address addr, uint32_t remoteL2);

  /**
   * Constructor
   * \param d The direction
   * \param type The type of address provided
   * \param addr The IPv4 address
   * \param mask The IPv4 mask
   * \param remoteL2 The group layer 2 address
   */
  LteSlTft (Direction d, AddressType type, Ipv4Address addr, Ipv4Mask mask, uint32_t remoteL2);

  /**
   * Constructor
   * \param d The direction
   * \param type The type of address provided
   * \param addr The IPv6 address
   * \param remoteL2 The group layer 2 address
   */
  LteSlTft (Direction d, AddressType type, Ipv6Address addr, uint32_t remoteL2);

  /**
   * Constructor (sets remote address only, defined for backward compatibility)
   * \param d The direction
   * \param addr The IPv6 address
   * \param remoteL2 The group layer 2 address
   */
  LteSlTft (Direction d, Ipv6Address addr, uint32_t remoteL2);

  /**
   * Constructor
   * \param d The direction
   * \param type The type of address provided
   * \param addr The IPv6 address
   * \param prefix The IPv6 prefix
   * \param remoteL2 The group layer 2 address
   */
  LteSlTft (Direction d, AddressType type, Ipv6Address addr, Ipv6Prefix prefix, uint32_t remoteL2);

  /**
   * Constructor for copy
   * \param tft The TFT to copy
   */
  LteSlTft (Ptr<LteSlTft> tft);

  /**
   * Function to evaluate if the SL TFT matches the remote IPv4 address
   * \param la the local address
   * \param ra the remote address
   * \return true if the TFT matches with the
   * parameters, false otherwise.
   */
  bool Matches (Ipv4Address la, Ipv4Address ra);

  /**
   * Function to evaluate if the SL TFT matches the remote IPv6 address
   * \param la the local address
   * \param ra the remote address
   *
   * \return true if the TFT matches with the
   * parameters, false otherwise.
   */
  bool Matches (Ipv6Address la, Ipv6Address ra);
  
  /**
   * Function to evaluate if the SL TFT is completely equal to another SL TFT
   * \param tft the tft to compare
   *
   * \return true if the provided SL TFT matches with the
   * actual SL TFT parameters, false otherwise.
   */
  bool Equals (Ptr<LteSlTft> tft);

  /**
   * Gets the Group L2 address associated with the TFT
   * \return the Group L2 address associated with the TFT
   */
  uint32_t GetRemoteL2Address ();

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
  ///< to sending or receiving only, or in both cases*/

  bool m_hasRemoteAddress;         /**< Indicates if the TFT has remoteAddress information */
  Ipv4Address m_remoteAddress;     /**< IPv4 address of the remote host  */
  Ipv6Address m_remoteAddress6;    /**< IPv6 address of the remote host  */
  Ipv4Mask m_remoteMask;           /**< IPv4 address mask of the remote host */
  Ipv6Prefix m_remoteMask6;        /**< IPv6 address mask of the remote host */
  bool m_hasLocalAddress;         /**< Indicates if the TFT has localeAddress information */
  Ipv4Address m_localAddress;      /**< IPv4 address of the UE */
  Ipv6Address m_localAddress6;     /**< IPv6 address of the UE */
  Ipv4Mask m_localMask;            /**< IPv4 address mask of the UE */
  Ipv6Prefix m_localMask6;         /**< IPv6 address mask of the remote host */

  uint32_t m_remoteL2Address;      ///< 24 bit MAC address of remote entity

};

} // namespace ns3

#endif /* LTE_SL_TFT_H */
