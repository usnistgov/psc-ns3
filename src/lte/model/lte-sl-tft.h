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
 * \ingroup Lte
 *
 * \brief  Traffic flow template used by sidelink bearers.
 *
 * This class implements a variant of LTE TFT for sidelink
 *
 * It is the simple version of NIST LteSlTft class in D2D repo.
 * It uses only remote address.
 *
 */
class LteSlTft : public SimpleRefCount<LteSlTft>
{
public:
  /**
   * \brief Indicates the direction of the traffic that is to be classified.
   */
  enum class Direction
  {
    TRANSMIT = 1,
    RECEIVE = 2,
    BIDIRECTIONAL = 3,
    INVALID = 4
  };

  /**
   * \brief Indicates the type of communication.
   */
  enum class CommType
  {
    Broadcast = 1,
    GroupCast = 2,
    Uincast = 3,
    INVALID = 4
  };

  /**
   * \brief Constructor (sets remote address only)
   *
   * \param d The direction
   * \param commType The communication type
   * \param remoteAddr The IPv4 address of the remote
   * \param dstL2Id The destination layer 2 id
   */
  LteSlTft (Direction d, CommType commType, Ipv4Address remoteAddr, uint32_t dstL2Id);

  /**
   * \brief Constructor (sets remote address only)
   *
   * \param d The direction
   * \param commType The communication type
   * \param remoteAddr The IPv6 address of the remote
   * \param dstL2Id The destination layer 2 id
   */
  LteSlTft (Direction d, CommType commType, Ipv6Address remoteAddr, uint32_t dstL2Id);

  /**
   * \brief Constructor for copy
   *
   * \param tft The TFT to copy
   */
  LteSlTft (Ptr<LteSlTft> tft);

  /**
   * \brief Function to evaluate if the SL TFT matches the remote IPv4 address
   *
   * \param ra the remote address
   * \return true if the TFT matches with the parameters, false otherwise.
   */
  bool Matches (Ipv4Address ra);

  /**
   * \brief Function to evaluate if the SL TFT matches the remote IPv6 address
   *
   * \param ra the remote address
   *
   * \return true if the TFT matches with the
   * parameters, false otherwise.
   */
  bool Matches (Ipv6Address ra);

  /**
   * \brief Function to evaluate if the SL TFT is completely equal to another SL TFT
   *
   * \param tft the tft to compare
   * \return true if the provided SL TFT matches with the
   *         actual SL TFT parameters, false otherwise.
   */
  bool Equals (Ptr<LteSlTft> tft);

  /**
   * \brief Gets the Destination L2 id associated with the TFT
   * \return The Destination L2 address associated with the TFT
   */
  uint32_t GetDstL2Id ();

  /**
   * \brief Indicates if the TFT is for an incoming sidelink bearer
   * \return true if the TFT is for an incoming sidelink bearer
   */
  bool isReceive ();

  /**
   * \brief Indicates if the TFT is for an outgoing sidelink bearer
   * \return true if the TFT is for an outgoing sidelink bearer
   */
  bool isTransmit ();

  /**
   * \brief Indicates if the TFT is for unicast communication
   * \return true if the TFT is for the unicast communication
   */
  bool isUnicast ();

private:
  Direction m_direction {Direction::INVALID}; /**< whether the filter needs to be applied
                                               * to sending or receiving only, or in both cases
                                               */
  CommType m_commType {CommType::INVALID}; //!< The type of communication
  bool m_hasRemoteAddress {false};      //!< Indicates if the TFT has remoteAddress information
  Ipv4Address m_remoteAddress {Ipv4Address::GetZero ()};        //!< IPv4 address of the remote host
  Ipv6Address m_remoteAddress6 {Ipv6Address::GetZero ()};       //!< IPv6 address of the remote host
  Ipv4Mask m_remoteMask {Ipv4Mask::GetZero ()};         //!< IPv4 address mask of the remote host
  Ipv6Prefix m_remoteMask6 {Ipv6Prefix::GetZero ()};    //!< IPv6 address mask of the remote host
  uint32_t m_dstL2Id {0};      //!< 24 bit L2 id of remote entity

};

} // namespace ns3

#endif /* LTE_SL_TFT_H */
