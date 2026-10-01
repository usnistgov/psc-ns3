// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo  <nbaldo@cttc.es>

#ifndef NR_EPC_TFT_H
#define NR_EPC_TFT_H

#include <ns3/ipv4-address.h>
#include <ns3/ipv6-address.h>
#include <ns3/simple-ref-count.h>

#include <list>

namespace ns3
{

/**
 * This class implements the EPS bearer Traffic Flow Template (TFT),
 * which is the set of all packet filters associated with an EPS bearer.
 *
 */
class NrEpcTft : public SimpleRefCount<NrEpcTft>
{
  public:
    /**
     * creates a TFT matching any traffic
     *
     * \return a newly created TFT that will match any traffic
     */
    static Ptr<NrEpcTft> Default();

    /**
     * Indicates the direction of the traffic that is to be classified.
     */
    enum Direction
    {
        DOWNLINK = 1,
        UPLINK = 2,
        BIDIRECTIONAL = 3
    };

    /**
     * Implement the data structure representing a TrafficFlowTemplate
     * Packet Filter.
     * See 3GPP TS 24.008 version 8.7.0 Release 8, Table 10.5.162/3GPP TS
     * 24.008: Traffic flow template information element
     *
     * With respect to the Packet Filter specification in the above doc,
     * the following features are NOT supported:
     *  - IPv6 filtering (including flow labels)
     *  - IPSec filtering
     *  - filter precedence field is not evaluated, hence it is recommended to setup
     *    the TFTs within a PDP context such that TFTs are mutually exclusive
     */
    struct PacketFilter
    {
        PacketFilter();

        /**
         *
         * \param d the direction
         * \param ra the remote address
         * \param la the local address
         * \param rp the remote port
         * \param lp the local port
         * \param tos the type of service
         *
         * \return true if the parameters match with the PacketFilter,
         * false otherwise.
         */
        bool Matches(Direction d,
                     Ipv4Address ra,
                     Ipv4Address la,
                     uint16_t rp,
                     uint16_t lp,
                     uint8_t tos) const;

        /**
         *
         * \param d the direction
         * \param ra the remote address
         * \param la the local address
         * \param rp the remote port
         * \param lp the local port
         * \param tos the type of service
         *
         * \return true if the parameters match with the PacketFilter,
         * false otherwise.
         */
        bool Matches(Direction d,
                     Ipv6Address ra,
                     Ipv6Address la,
                     uint16_t rp,
                     uint16_t lp,
                     uint8_t tos) const;

        /// Used to specify the precedence for the packet filter among all packet filters in the
        /// TFT; higher values will be evaluated last.
        uint8_t precedence{255};

        /// Whether the filter needs to be applied to uplink / downlink only, or in both cases
        Direction direction{BIDIRECTIONAL};

        Ipv4Address remoteAddress;      //!< IPv4 address of the remote host
        Ipv4Mask remoteMask{"0.0.0.0"}; //!< IPv4 address mask of the remote host
        Ipv4Address localAddress;       //!< IPv4 address of the UE
        Ipv4Mask localMask{"0.0.0.0"};  //!< IPv4 address mask of the UE

        Ipv6Address remoteIpv6Address; //!< IPv6 address of the remote host
        Ipv6Prefix remoteIpv6Prefix;   //!< IPv6 address prefix of the remote host
        Ipv6Address localIpv6Address;  //!< IPv6 address of the UE
        Ipv6Prefix localIpv6Prefix;    //!< IPv6 address prefix of the UE

        uint16_t remotePortStart{0};   //!< start of the port number range of the remote host
        uint16_t remotePortEnd{65535}; //!< end of the port number range of the remote host
        uint16_t localPortStart{0};    //!< start of the port number range of the UE
        uint16_t localPortEnd{65535};  //!< end of the port number range of the UE

        uint8_t typeOfService{0};     //!< type of service field
        uint8_t typeOfServiceMask{0}; //!< type of service field mask
    };

    NrEpcTft();

    virtual ~NrEpcTft();

    /**
     * add a PacketFilter to the Traffic Flow Template
     *
     * \param f the PacketFilter to be added
     *
     * \return the id( 0 <= id < 16) of the newly added filter, if the addition was successful. Will
     * fail if you try to add more than 15 filters. This is to be compliant with TS 24.008.
     */
    uint8_t Add(PacketFilter f);

    /**
     * remove a PacketFilter from the Traffic Flow Template
     *
     * \param f the PacketFilter to be removed
     *
     * \return whether the specified filter was found (and removed) or not
     */
    bool Remove(PacketFilter f);

    /**
     *
     * \param direction
     * \param remoteAddress
     * \param localAddress
     * \param remotePort
     * \param localPort
     * \param typeOfService
     *
     * \return true if any PacketFilter in the TFT matches with the
     * parameters, false otherwise.
     */
    bool Matches(Direction direction,
                 Ipv4Address remoteAddress,
                 Ipv4Address localAddress,
                 uint16_t remotePort,
                 uint16_t localPort,
                 uint8_t typeOfService) const;

    /**
     *
     * \param direction
     * \param remoteAddress
     * \param localAddress
     * \param remotePort
     * \param localPort
     * \param typeOfService
     *
     * \return true if any PacketFilter in the TFT matches with the
     * parameters, false otherwise.
     */
    bool Matches(Direction direction,
                 Ipv6Address remoteAddress,
                 Ipv6Address localAddress,
                 uint16_t remotePort,
                 uint16_t localPort,
                 uint8_t typeOfService) const;

    /**
     * Get the packet filters
     * \return a container of packet filters
     */
    std::list<PacketFilter> GetPacketFilters() const;

  protected:
    std::list<PacketFilter> m_filters; ///< packet filter list
    uint8_t m_numFilters;              ///< number of packet filters applied to this TFT
};

/**
 * Output stream operator for EPC TFT direction
 *
 * \param os output stream
 * \param d EPC TFT direction
 * \return ostream
 */
std::ostream& operator<<(std::ostream& os, const NrEpcTft::Direction& d);

/**
 * \brief operator ==, compares PacketFilter
 * \param lhs filter to compare
 * \param rhs other filter to compare
 * \return true if this instance and the other filter are equal
 */
bool operator==(const NrEpcTft::PacketFilter& lhs, const NrEpcTft::PacketFilter& rhs);

/**
 * Output stream for EPC TFT packet filter
 *
 * \param os output stream
 * \param f EPC TFT packet filter
 * \return ostream
 */
std::ostream& operator<<(std::ostream& os, const NrEpcTft::PacketFilter& f);

} // namespace ns3

#endif /* NR_EPC_TFT_H */
