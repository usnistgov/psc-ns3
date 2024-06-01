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

#include <ns3/ipv4-address.h>
#include <ns3/ipv6-address.h>
#include <ns3/nstime.h>
#include <ns3/simple-ref-count.h>

#include <list>

namespace ns3
{

/**
 * \brief structure to hold sidelink information about a logical channel
 *
 * Corresponds to "Sidelink Transmission/Identification/Other Information"
 * define in TS 38.321.
 */
struct SidelinkInfo
{
    /**
     * \brief Indicates the type of communication.  Corresponds to
     * TS 38.212 Table 8.4.1.1-1: Cast type indicator
     */
    enum class CastType
    {
        Broadcast = 0,
        Groupcast = 1,
        Unicast = 2,
        GroupcastNegativeOnly = 3,
        Invalid = 4
    };

    CastType m_castType{CastType::Invalid}; //!< Cast type
    uint32_t m_srcL2Id{0};                  //!< Source L2 ID
    uint32_t m_dstL2Id{0};                  //!< 24 bit L2 id of remote entity
    bool m_harqEnabled{false};              //!< Whether HARQ is enabled
    Time m_pdb;                             //!< Packet Delay Budget
    bool m_dynamic{false};                  //!< flag for whether LC is dynamic or SPS
    Time m_rri{0};                          //!< Resource Reservation Interval
    uint8_t m_lcId{0};                      //!< Logical channel ID
    uint8_t m_priority{0};                  //!< Priority
};

/**
 * \brief Equality operator.
 *
 * \param a lhs
 * \param b rhs
 * \returns true if operands are equal, false otherwise
 */
bool operator==(const SidelinkInfo a, const SidelinkInfo b);

/**
 * \ingroup Lte
 *
 * \brief  Traffic flow template used by sidelink bearers.
 *
 * This class implements a variant of LTE TFT for sidelink
 *
 * It is the simple version of NIST LteSlTft class in D2D repo.
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
     * \brief Indicates the type of communication.  Corresponds to
     * TS 38.212 Table 8.4.1.1-1: Cast type indicator
     */
    enum class CastType
    {
        Broadcast = 0,
        Groupcast = 1,
        Unicast = 2,
        GroupcastNegativeOnly = 3,
        Invalid = 4
    };

    /**
     * \brief Constructor
     *
     * \param d The direction
     * \param remoteAddr The IPv4 address of the remote
     * \param slInfo SidelinkInfo structure
     */
    LteSlTft(Direction d, Ipv4Address remoteAddr, const struct SidelinkInfo& slInfo);

    /**
     * \brief Constructor
     *
     * \param d The direction
     * \param remoteAddr The IPv6 address of the remote
     * \param slInfo SidelinkInfo structure
     */
    LteSlTft(Direction d, Ipv6Address remoteAddr, const struct SidelinkInfo& slInfo);

    /**
     * \brief Constructor
     *
     * \param d The direction
     * \param remoteAddr The IPv4 address of the remote
     * \param remotePort The port number of the remote
     * \param slInfo SidelinkInfo structure
     */
    LteSlTft(Direction d,
             Ipv4Address remoteAddr,
             uint16_t remotePort,
             const struct SidelinkInfo& slInfo);

    /**
     * \brief Constructor
     *
     * \param d The direction
     * \param remoteAddr The IPv6 address of the remote
     * \param remotePort The port number of the remote
     * \param slInfo SidelinkInfo structure
     */
    LteSlTft(Direction d,
             Ipv6Address remoteAddr,
             uint16_t remotePort,
             const struct SidelinkInfo& slInfo);

    /**
     * \brief Constructor for copy
     *
     * \param tft The TFT to copy
     */
    LteSlTft(Ptr<LteSlTft> tft);

    /**
     * \brief Function to evaluate if the SL TFT matches the remote IPv4 address
     *
     * \param ra the remote address
     * \return true if the TFT matches with the parameters, false otherwise.
     */
    bool Matches(Ipv4Address ra) const;

    /**
     * \brief Function to evaluate if the SL TFT matches the remote IPv6 address
     *
     * \param ra the remote address
     *
     * \return true if the TFT matches with the
     * parameters, false otherwise.
     */
    bool Matches(Ipv6Address ra) const;

    /**
     * \brief Function to evaluate if the SL TFT matches the remote IPv6 address
     *        and the remote port
     *
     * \param ra the remote address
     * \param rp the remote port
     *
     * \return true if the TFT matches with the parameters, false otherwise.
     */
    bool Matches(Ipv6Address ra, uint16_t rp);

    /**
     * \brief Function to evaluate if the SL TFT matches the remote IPv4 address
     *        and the remote port
     *
     * \param ra the remote address
     * \param rp the remote port
     *
     * \return true if the TFT matches with the parameters, false otherwise.
     */
    bool Matches(Ipv4Address ra, uint16_t rp);

    /**
     * \brief Function to evaluate if the SL TFT is completely equal to another SL TFT
     *
     * \param tft the tft to compare
     * \return true if the provided SL TFT matches with the
     *         actual SL TFT parameters, false otherwise.
     */
    bool Equals(Ptr<LteSlTft> tft) const;

    /**
     * \brief Gets the SidelinkInfo associated with the TFT
     * \return The SidelinkInfo associated with the TFT
     */
    struct SidelinkInfo GetSidelinkInfo() const;

    /**
     * \brief Set the lcId of the SidelinkInfo associated with the TFT
     *
     * \param lcId The logical channel id to be set
     */
    void SetSidelinkInfoLcId(uint8_t lcId);

    /**
     * \brief Indicates if the TFT is for an incoming sidelink bearer
     * \return true if the TFT is for an incoming sidelink bearer
     */
    bool IsReceive() const;

    /**
     * \brief Indicates if the TFT is for an outgoing sidelink bearer
     * \return true if the TFT is for an outgoing sidelink bearer
     */
    bool IsTransmit() const;

    /**
     * \brief Indicates if the TFT is for unicast communication
     * \return true if the TFT is for the unicast communication
     */
    bool IsUnicast() const;

    /**
     * \brief Indicates if the TFT is for HARQ feedback-enabled communication
     * \return true if the TFT is for HARQ feedback-enabled communication
     */
    bool IsHarqEnabled() const;

    /**
     * \brief Return the packet delay budget
     * \return packet delay budget
     */
    Time GetDelayBudget() const;

  private:
    Direction m_direction{Direction::INVALID}; /**< whether the filter needs to be applied
                                                * to sending or receiving only, or in both cases
                                                */
    bool m_hasRemoteAddress{false}; //!< Indicates if the TFT has remoteAddress information
    Ipv4Address m_remoteAddress{Ipv4Address::GetZero()};  //!< IPv4 address of the remote host
    Ipv6Address m_remoteAddress6{Ipv6Address::GetZero()}; //!< IPv6 address of the remote host
    Ipv4Mask m_remoteMask{Ipv4Mask::GetZero()};           //!< IPv4 address mask of the remote host
    Ipv6Prefix m_remoteMask6{Ipv6Prefix::GetZero()};      //!< IPv6 address mask of the remote host
    bool m_harqEnabled{false};                            //!< Whether HARQ is enabled
    Time m_delayBudget{Seconds(0)};                       //!< Packet delay budget
    bool m_hasRemotePort{false};        //!< Indicates if the TFT has remotePort information
    uint16_t m_remotePort{0};           /**< Port number of the remote host */
    struct SidelinkInfo m_sidelinkInfo; //!< SidelinkInfo struct
};

} // namespace ns3

#endif /* LTE_SL_TFT_H */
