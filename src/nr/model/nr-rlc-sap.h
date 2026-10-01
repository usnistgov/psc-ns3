// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#ifndef NR_RLC_SAP_H
#define NR_RLC_SAP_H

#include "ns3/packet.h"

namespace ns3
{

/**
 * Service Access Point (SAP) offered by the UM-RLC and AM-RLC entities to the PDCP entity
 * See 3GPP 36.322 Radio Link Control (RLC) protocol specification
 *
 * This is the RLC SAP Provider
 * (i.e. the part of the SAP that contains the RLC methods called by the PDCP)
 */
class NrRlcSapProvider
{
  public:
    virtual ~NrRlcSapProvider() = default;

    /**
     * Parameters for NrRlcSapProvider::TransmitPdcpPdu
     */
    struct TransmitPdcpPduParameters
    {
        Ptr<Packet> pdcpPdu; /**< the PDCP PDU */
        uint16_t rnti;       /**< the C-RNTI identifying the UE */
        uint8_t lcid; /**< the logical channel id corresponding to the sending RLC instance */
    };

    /**
     * Send a PDCP PDU to the RLC for transmission
     * This method is to be called
     * when upper PDCP entity has a PDCP PDU ready to send
     * \param params the TransmitPdcpPduParameters
     */
    virtual void TransmitPdcpPdu(TransmitPdcpPduParameters params) = 0;
};

/**
 * Service Access Point (SAP) offered by the UM-RLC and AM-RLC entities to the PDCP entity
 * See 3GPP 36.322 Radio Link Control (RLC) protocol specification
 *
 * This is the RLC SAP User
 * (i.e. the part of the SAP that contains the PDCP methods called by the RLC)
 */
class NrRlcSapUser
{
  public:
    virtual ~NrRlcSapUser() = default;

    /**
     * Called by the RLC entity to notify the PDCP entity of the reception of a new PDCP PDU
     *
     * \param p the PDCP PDU
     */
    virtual void ReceivePdcpPdu(Ptr<Packet> p) = 0;
};

/// NrRlcSpecificNrRlcSapProvider
template <class C>
class NrRlcSpecificNrRlcSapProvider : public NrRlcSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param rlc the RLC
     */
    NrRlcSpecificNrRlcSapProvider(C* rlc);

    // Delete default constructor to avoid misuse
    NrRlcSpecificNrRlcSapProvider() = delete;

    /**
     * Interface implemented from NrRlcSapProvider
     * \param params the TransmitPdcpPduParameters
     */
    void TransmitPdcpPdu(TransmitPdcpPduParameters params) override;

  private:
    C* m_rlc; ///< the RLC
};

template <class C>
NrRlcSpecificNrRlcSapProvider<C>::NrRlcSpecificNrRlcSapProvider(C* rlc)
    : m_rlc(rlc)
{
}

template <class C>
void
NrRlcSpecificNrRlcSapProvider<C>::TransmitPdcpPdu(TransmitPdcpPduParameters params)
{
    m_rlc->DoTransmitPdcpPdu(params.pdcpPdu);
}

/// NrRlcSpecificNrRlcSapUser class
template <class C>
class NrRlcSpecificNrRlcSapUser : public NrRlcSapUser
{
  public:
    /**
     * Constructor
     *
     * \param pdcp the PDCP
     */
    NrRlcSpecificNrRlcSapUser(C* pdcp);

    // Delete default constructor to avoid misuse
    NrRlcSpecificNrRlcSapUser() = delete;

    // Interface implemented from NrRlcSapUser
    void ReceivePdcpPdu(Ptr<Packet> p) override;

  private:
    C* m_pdcp; ///< the PDCP
};

template <class C>
NrRlcSpecificNrRlcSapUser<C>::NrRlcSpecificNrRlcSapUser(C* pdcp)
    : m_pdcp(pdcp)
{
}

template <class C>
void
NrRlcSpecificNrRlcSapUser<C>::ReceivePdcpPdu(Ptr<Packet> p)
{
    m_pdcp->DoReceivePdcpPdu(p);
}

} // namespace ns3

#endif // NR_RLC_SAP_H
