// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>
// Modified by: CTTC for NR Sidelink

#ifndef NR_SL_RLC_H
#define NR_SL_RLC_H

#include "nr-sl-mac-sap.h"
#include "nr-sl-rlc-sap.h"

#include "ns3/nr-rlc.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/traced-value.h"
#include "ns3/uinteger.h"
#include <ns3/packet.h>
#include <ns3/simple-ref-count.h>

namespace ns3
{

// class NrRlcSapProvider;
// class NrRlcSapUser;
//
// class NrMacSapProvider;
// class NrMacSapUser;

/**
 * This abstract base class defines the API to interact with the Radio Link Control
 * (NR_RLC) in LTE, see 3GPP TS 36.322
 *
 */
class NrSlRlc : public NrRlc
{
    /// let the forwarder class access the protected and private members
    friend class MemberNrSlMacSapUser<NrSlRlc>;
    /// let the forwarder class access the protected and private members
    friend class MemberNrSlRlcSapProvider<NrSlRlc>;

  public:
    NrSlRlc();
    ~NrSlRlc() override;
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    void DoDispose() override;

    // Sidelink
  public:
    /// Channel type enumeration
    enum ChannelType
    {
        DEFAULT = 0,
        STCH
    };

    /**
     * \brief Set the NR Sidelik MAC SAP offered by MAC to RLC
     *
     * \param s the NR Sidelik MAC SAP provider interface offered by
     *          MAC by this RLC
     */
    void SetNrSlMacSapProvider(NrSlMacSapProvider* s);

    /**
     * \brief Get the NR Sidelik MAC SAP offered by this RLC
     *
     * \return the NR Sidelik MAC SAP user interface offered to the
     *         MAC by this RLC
     */
    NrSlMacSapUser* GetNrSlMacSapUser();

    /**
     * \brief Get the NR Sidelik SAP offered by RLC to PDCP
     *
     * \return the NR Sidelink RLC SAP Provider interface offered by RLC to PDCP
     */
    NrSlRlcSapProvider* GetNrSlRlcSapProvider();

    /**
     * \brief Set the NR Sidelik SAP offered by PDCP to RLC
     *
     * \param s the NR Sidelink SAP user interface offered by PDCP to RLC
     */
    void SetNrSlRlcSapUser(NrSlRlcSapUser* s);

    /**
     * \brief Set the RLC logical channel type
     *
     * Currently this method is only used for a
     * Sidelink logical channel.
     *
     * \param channelType the logical channel type
     */
    void SetRlcChannelType(NrSlRlc::ChannelType channelType);

    /**
     * \brief Sets the source L2 Id for sidelink identification of the RLC UM and PDCP entity
     * \param src The Sidelink source layer 2 id
     */
    void SetSourceL2Id(uint32_t src);

    /**
     * \brief Sets the destination L2 Id for sidelink identification of the RLC UM and PDCP entity
     * \param dst The Sidelink destination layer 2 id
     */
    void SetDestinationL2Id(uint32_t dst);

  protected:
    /**
     * \brief Send a NR Sidelink PDCP PDU to the RLC for transmission
     *
     * This method is to be called when upper PDCP entity has a NR Sidelink PDCP
     * PDU ready to send
     *
     * \param params the NrSlTransmitPdcpPduParameters
     */
    virtual void DoTransmitNrSlPdcpPdu(
        const NrSlRlcSapProvider::NrSlTransmitPdcpPduParameters& params) = 0;

    /**
     * \brief Called by the MAC to notify the RLC that the scheduler granted a
     * transmission opportunity to this RLC instance.
     *
     * \param params the NrSlTxOpportunityParameters
     */
    virtual void DoNotifyNrSlTxOpportunity(
        const NrSlMacSapUser::NrSlTxOpportunityParameters& params) = 0;

    /**
     * \brief Called by the MAC to notify the RLC of the reception of a new PDU
     *
     * \param rxPduParams the NrSlReceiveRlcPduParameters
     */
    virtual void DoReceiveNrSlRlcPdu(NrSlMacSapUser::NrSlReceiveRlcPduParameters rxPduParams) = 0;

    NrSlMacSapProvider* m_nrSlMacSapProvider{nullptr}; //!< NR SL MAC SAP provider
    NrSlMacSapUser* m_nrSlMacSapUser;                  //!< NR SL MAC SAP user
    NrSlRlcSapProvider* m_nrSlRlcSapProvider; //!< SAP interface to receive calls from PDCP instance
    NrSlRlcSapUser* m_nrSlRlcSapUser{nullptr};   //!< SAP interface to call methods of PDCP instance
    uint32_t m_srcL2Id{0};                       ///< Source L2 ID (24 bits)
    uint32_t m_dstL2Id{0};                       ///< Destination L2 ID (24 bits)
    ChannelType m_channelType{NrSlRlc::DEFAULT}; ///< The logical channel type
};

/**
 * NR_RLC Saturation Mode (SM): simulation-specific mode used for
 * experiments that do not need to consider the layers above the NR_RLC.
 * The NR_RLC SM, unlike the standard NR_RLC modes, it does not provide
 * data delivery services to upper layers; rather, it just generates a
 * new NR_RLC PDU whenever the MAC notifies a transmission opportunity.
 *
 */
class NrSlRlcSm : public NrSlRlc
{
  public:
    NrSlRlcSm();
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    void DoTransmitPdcpPdu(Ptr<Packet> p) override;
    void DoNotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams) override;
    void DoNotifyHarqDeliveryFailure() override;
    void DoReceivePdu(NrMacSapUser::ReceivePduParameters rxPduParams) override;

  protected:
    void DoTransmitNrSlPdcpPdu(
        const NrSlRlcSapProvider::NrSlTransmitPdcpPduParameters& params) override;
    void DoNotifyNrSlTxOpportunity(
        const NrSlMacSapUser::NrSlTxOpportunityParameters& params) override;
    void DoReceiveNrSlRlcPdu(NrSlMacSapUser::NrSlReceiveRlcPduParameters rxPduParams) override;
};

} // namespace ns3

#endif // NR_SL_RLC_H
