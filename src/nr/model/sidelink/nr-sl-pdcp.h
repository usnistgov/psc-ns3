// Copyright (c) 2011-2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//

#ifndef NR_SL_PDCP_H
#define NR_SL_PDCP_H

#include "nr-sl-pdcp-sap.h"
#include "nr-sl-rlc-sap.h"

#include "ns3/nr-pdcp.h"
#include "ns3/object.h"

namespace ns3
{

/**
 * LTE PDCP entity, see 3GPP TS 36.323
 */
class NrSlPdcp : public NrPdcp
{
    /// allow MemberNrSlPdcpSapProvider<NrPdcp> class friend access
    friend class MemberNrSlPdcpSapProvider<NrPdcp>;
    /// allow MemberNrSlRlcSapUser<NrPdcp> class friend access
    friend class MemberNrSlRlcSapUser<NrPdcp>;

  public:
    NrSlPdcp();
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    void DoDispose() override;

    /**
     * \brief Get the NR Sidelik SAP offered by PDCP to RRC
     *
     * \return the NR Sidelik UE PHY SAP provider interface offered by PDCP to RRC
     */
    NrSlPdcpSapProvider* GetNrSlPdcpSapProvider();

    /**
     * \brief Set the NR Sidelik SAP offered by RRC to PDCP
     *
     * \param s the NR Sidelik PDCP SAP user interface offered by RRC to PDCP
     */
    void SetNrSlPdcpSapUser(NrSlPdcpSapUser* s);

    /**
     * \brief Set the NR Sidelik SAP offered by RLC to PDCP
     *
     * \param s the NR Sidelink RLC SAP Provider interface offered by RLC to PDCP
     */
    void SetNrSlRlcSapProvider(NrSlRlcSapProvider* s);

    /**
     * \brief Get the NR Sidelik SAP offered by PDCP to RLC
     *
     * \return the NR Sidelink SAP user interface offered by PDCP to RLC
     */
    NrSlRlcSapUser* GetNrSlRlcSapUser();

    /**
     * \brief Send a RRC PDU to the RDCP for transmission
     *
     * This method is to be called when upper RRC entity has a NR SL RRC PDU
     * ready to send.
     *
     * \param params
     */
    void DoTransmitNrSlPdcpSdu(const NrSlPdcpSapProvider::NrSlTransmitPdcpSduParameters& params);

    /**
     * \brief Called by the RLC entity to notify the PDCP entity of the reception
     * of a new NR sidelink PDCP PDU
     *
     * \param p the PDCP PDU
     */
    void DoReceiveNrSlPdcpPdu(Ptr<Packet> p);

    /**
     * \brief Sets the source L2 Id for sidelink identification of the RLC and PDCP entity
     * \param src The Sidelink source layer 2 id
     */
    void SetSourceL2Id(uint32_t src);

    /**
     * \brief Sets the source L2 Id for sidelink identification of the RLC PDCP entity
     * \param dst The Sidelink destination layer 2 id
     */
    void SetDestinationL2Id(uint32_t dst);

  private:
    /**
     * \brief Indicates if this PDCP is for a sidelink radio bearer
     *
     * \return true if the PDCP instance is for a sidelink radio bearer
     */
    bool IsSlRb();
    uint32_t m_srcL2Id{0}; //!< Source L2 ID (24 bits)
    uint32_t m_dstL2Id{0}; //!< Destination L2 ID (24 bits)
    /**
     * Constants. See section 7.2 in TS 36.323
     */
    static const uint16_t m_maxPdcpSlSn = 65535;
    NrSlPdcpSapProvider*
        m_nrSlPdcpSapProvider; //!< SAP interface to receive calls from RRC instance
    NrSlPdcpSapUser* m_nrSlPdcpSapUser{
        nullptr}; //!< SAP interface to call the methods of RRC instance
    NrSlRlcSapProvider* m_nrSlRlcSapProvider{
        nullptr};                     //!< SAP interface to call the methods of RLC instance
    NrSlRlcSapUser* m_nrSlRlcSapUser; //!< SAP interface to receive calls from RLC instance
};

} // namespace ns3

#endif // NR_SL_PDCP_H
