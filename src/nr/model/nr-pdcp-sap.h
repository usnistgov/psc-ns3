// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#ifndef NR_PDCP_SAP_H
#define NR_PDCP_SAP_H

#include "ns3/packet.h"

namespace ns3
{

/**
 * Service Access Point (SAP) offered by the PDCP entity to the RRC entity
 * See 3GPP 36.323 Packet Data Convergence Protocol (PDCP) specification
 *
 * This is the PDCP SAP Provider
 * (i.e. the part of the SAP that contains the PDCP methods called by the RRC)
 */
class NrPdcpSapProvider
{
  public:
    virtual ~NrPdcpSapProvider() = default;

    /**
     * Parameters for NrPdcpSapProvider::TransmitPdcpSdu
     */
    struct TransmitPdcpSduParameters
    {
        Ptr<Packet> pdcpSdu; /**< the RRC PDU */
        uint16_t rnti;       /**< the C-RNTI identifying the UE */
        uint8_t lcid; /**< the logical channel id corresponding to the sending RLC instance */
    };

    /**
     * Send RRC PDU parameters to the PDCP for transmission
     *
     * This method is to be called when upper RRC entity has a
     * RRC PDU ready to send
     *
     * \param params Parameters
     */
    virtual void TransmitPdcpSdu(TransmitPdcpSduParameters params) = 0;
};

/**
 * Service Access Point (SAP) offered by the PDCP entity to the RRC entity
 * See 3GPP 36.323 Packet Data Convergence Protocol (PDCP) specification
 *
 * This is the PDCP SAP User
 * (i.e. the part of the SAP that contains the RRC methods called by the PDCP)
 */
class NrPdcpSapUser
{
  public:
    virtual ~NrPdcpSapUser() = default;

    /**
     * Parameters for NrPdcpSapUser::ReceivePdcpSdu
     */
    struct ReceivePdcpSduParameters
    {
        Ptr<Packet> pdcpSdu; /**< the RRC PDU */
        uint16_t rnti;       /**< the C-RNTI identifying the UE */
        uint8_t lcid; /**< the logical channel id corresponding to the sending RLC instance */
    };

    /**
     * Called by the PDCP entity to notify the RRC entity of the reception of a new RRC PDU
     *
     * \param params Parameters
     */
    virtual void ReceivePdcpSdu(ReceivePdcpSduParameters params) = 0;
};

/// NrPdcpSpecificNrPdcpSapProvider class
template <class C>
class NrPdcpSpecificNrPdcpSapProvider : public NrPdcpSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param pdcp PDCP
     */
    NrPdcpSpecificNrPdcpSapProvider(C* pdcp);

    // Delete default constructor to avoid misuse
    NrPdcpSpecificNrPdcpSapProvider() = delete;

    // Interface implemented from NrPdcpSapProvider
    void TransmitPdcpSdu(TransmitPdcpSduParameters params) override;

  private:
    C* m_pdcp; ///< the PDCP
};

template <class C>
NrPdcpSpecificNrPdcpSapProvider<C>::NrPdcpSpecificNrPdcpSapProvider(C* pdcp)
    : m_pdcp(pdcp)
{
}

template <class C>
void
NrPdcpSpecificNrPdcpSapProvider<C>::TransmitPdcpSdu(TransmitPdcpSduParameters params)
{
    m_pdcp->DoTransmitPdcpSdu(params);
}

/// NrPdcpSpecificNrPdcpSapUser class
template <class C>
class NrPdcpSpecificNrPdcpSapUser : public NrPdcpSapUser
{
  public:
    /**
     * Constructor
     *
     * \param rrc RRC
     */
    NrPdcpSpecificNrPdcpSapUser(C* rrc);

    // Delete default constructor to avoid misuse
    NrPdcpSpecificNrPdcpSapUser() = delete;

    // Interface implemented from NrPdcpSapUser
    void ReceivePdcpSdu(ReceivePdcpSduParameters params) override;

  private:
    C* m_rrc; ///< RRC
};

template <class C>
NrPdcpSpecificNrPdcpSapUser<C>::NrPdcpSpecificNrPdcpSapUser(C* rrc)
    : m_rrc(rrc)
{
}

template <class C>
void
NrPdcpSpecificNrPdcpSapUser<C>::ReceivePdcpSdu(ReceivePdcpSduParameters params)
{
    m_rrc->DoReceivePdcpSdu(params);
}

} // namespace ns3

#endif // NR_PDCP_SAP_H
