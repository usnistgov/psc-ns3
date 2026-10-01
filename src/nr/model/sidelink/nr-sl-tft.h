//
// SPDX-License-Identifier: NIST-Software
//

#ifndef NR_SL_TFT_H
#define NR_SL_TFT_H

#include "ns3/nr-epc-tft.h"
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
 * \ingroup nr
 *
 * \brief  Traffic flow template used by sidelink bearers.
 *
 * This class implements a variant of NR TFT for sidelink
 */
class NrSlTft : public NrEpcTft
{
  public:
    /**
     * \brief Indicates the type of bearer (direction of traffic)
     */
    enum class BearerType
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
     * \brief Construct a bearer of type bearerType with an associated SidelinkInfo.
     *
     * \param bearerType The bearer type
     * \param slInfo SidelinkInfo structure
     */
    NrSlTft(BearerType bearerType, const struct SidelinkInfo& slInfo);

    /**
     * \brief Construct a bearer of type bearerType with an associated SidelinkInfo,
     * and add an IPv4 packet filter (allowing any port) to the bearer.
     *
     * \param bearerType The bearer type
     * \param remoteAddr The IPv4 address of the remote
     * \param slInfo SidelinkInfo structure
     */
    NrSlTft(BearerType bearerType, Ipv4Address remoteAddr, const struct SidelinkInfo& slInfo);

    /**
     * \brief Construct a bearer of type bearerType with an associated SidelinkInfo,
     * and add an IPv6 packet filter (allowing any port) to the bearer.
     *
     * \param bearerType The bearer type
     * \param remoteAddr The IPv6 address of the remote
     * \param slInfo SidelinkInfo structure
     */
    NrSlTft(BearerType bearerType, Ipv6Address remoteAddr, const struct SidelinkInfo& slInfo);

    /**
     * \brief Construct a bearer of type bearerType with an associated SidelinkInfo,
     * and add an IPv4 packet filter (specific to a port) to the bearer.
     *
     * \param bearerType The bearer type
     * \param remoteAddr The IPv4 address of the remote
     * \param remotePort The port number of the remote
     * \param slInfo SidelinkInfo structure
     */
    NrSlTft(BearerType bearerType,
            Ipv4Address remoteAddr,
            uint16_t remotePort,
            const struct SidelinkInfo& slInfo);

    /**
     * \brief Construct a bearer of type bearerType with an associated SidelinkInfo,
     * and add an IPv6 packet filter (specific to a port) to the bearer.
     *
     * \param bearerType The bearer type
     * \param remoteAddr The IPv6 address of the remote
     * \param remotePort The port number of the remote
     * \param slInfo SidelinkInfo structure
     */
    NrSlTft(BearerType bearerType,
            Ipv6Address remoteAddr,
            uint16_t remotePort,
            const struct SidelinkInfo& slInfo);

    /**
     * \brief Constructor for copy
     *
     * \param tft The TFT to copy
     */
    NrSlTft(Ptr<NrSlTft> tft);

    ~NrSlTft() override;

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

    BearerType m_bearerType{BearerType::INVALID}; /**< whether the TFT applies to transmit or
                                                   * receive bearers, or both
                                                   */
    bool m_harqEnabled{false};                    //!< Whether HARQ is enabled
    Time m_delayBudget{Seconds(0)};               //!< Packet delay budget
    struct SidelinkInfo m_sidelinkInfo;           //!< SidelinkInfo struct
};

} // namespace ns3

#endif /* NR_SL_TFT_H */
