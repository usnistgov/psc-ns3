// Copyright (c) 2015 Danilo Abrignani
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Danilo Abrignani <danilo.abrignani@unibo.it>
//
///

#ifndef NR_SIMPLE_UE_COMPONENT_CARRIER_MANAGER_H
#define NR_SIMPLE_UE_COMPONENT_CARRIER_MANAGER_H

#include "nr-rrc-sap.h"
#include "nr-ue-ccm-rrc-sap.h"
#include "nr-ue-component-carrier-manager.h"

#include <map>

namespace ns3
{
class NrUeCcmRrcSapProvider;

/**
 * \brief Component carrier manager implementation which simply does nothing.
 *
 * Selecting this component carrier selection algorithm is equivalent to disabling automatic
 * triggering of component carrier selection. This is the default choice.
 *
 */
class NrSimpleUeComponentCarrierManager : public NrUeComponentCarrierManager
{
  public:
    /// Creates a No-op CCS algorithm instance.
    NrSimpleUeComponentCarrierManager();

    ~NrSimpleUeComponentCarrierManager() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    // inherited from NrComponentCarrierManager
    NrMacSapProvider* GetNrMacSapProvider() override;

    /// let the forwarder class access the protected and private members
    friend class MemberNrUeCcmRrcSapProvider<NrSimpleUeComponentCarrierManager>;
    // friend class MemberNrUeCcmRrcSapUser<NrSimpleUeComponentCarrierManager>;

    /// allow NrSimpleUeCcmMacSapProvider class friend access
    friend class NrSimpleUeCcmMacSapProvider;
    /// allow NrSimpleUeCcmMacSapUser class friend access
    friend class NrSimpleUeCcmMacSapUser;

  protected:
    // inherited from Object
    void DoInitialize() override;
    void DoDispose() override;
    // inherited from NrCcsAlgorithm as a Component Carrier Management SAP implementation
    /**
     * \brief Report Ue Measure function
     * \param rnti the RNTI
     * \param measResults the measure results
     */
    void DoReportUeMeas(uint16_t rnti, NrRrcSap::MeasResults measResults);
    // forwarded from NrMacSapProvider
    /**
     * \brief Transmit PDU function
     * \param params NrMacSapProvider::TransmitPduParameters
     */
    void DoTransmitPdu(NrMacSapProvider::TransmitPduParameters params);
    /**
     * \brief Report buffer status function
     * \param params NrMacSapProvider::ReportBufferStatusParameters
     */
    virtual void DoReportBufferStatus(NrMacSapProvider::ReportBufferStatusParameters params);
    /// Notify HARQ deliver failure
    void DoNotifyHarqDeliveryFailure();
    // forwarded from NrMacSapUser
    /**
     * \brief Notify TX opportunity function
     *
     * \param txOpParams the NrMacSapUser::TxOpportunityParameters
     */
    void DoNotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams);
    /**
     * \brief Receive PDU function
     *
     * \param rxPduParams the NrMacSapUser::ReceivePduParameters
     */
    void DoReceivePdu(NrMacSapUser::ReceivePduParameters rxPduParams);
    // forwarded from NrUeCcmRrcSapProvider
    /**
     * \brief Add LC function
     * \param lcId the LCID
     * \param lcConfig the logical channel config
     * \param msu the MSU
     * \returns updated LC config list
     */
    virtual std::vector<NrUeCcmRrcSapProvider::LcsConfig> DoAddLc(
        uint8_t lcId,
        NrUeCmacSapProvider::LogicalChannelConfig lcConfig,
        NrMacSapUser* msu);
    /**
     * \brief Remove LC function
     * \param lcid the LCID
     * \returns updated LC list
     */
    std::vector<uint16_t> DoRemoveLc(uint8_t lcid);
    /**
     * \brief Configure signal bearer function
     * \param lcId the LCID
     * \param lcConfig the logical channel config
     * \param msu the MSU
     * \returns NrMacSapUser *
     */
    virtual NrMacSapUser* DoConfigureSignalBearer(
        uint8_t lcId,
        NrUeCmacSapProvider::LogicalChannelConfig lcConfig,
        NrMacSapUser* msu);
    /**
     * \brief Reset LC map
     *
     */
    void DoReset();

  protected:
    NrMacSapUser* m_ccmMacSapUser;         //!< Interface to the UE RLC instance.
    NrMacSapProvider* m_ccmMacSapProvider; //!< Receive API calls from the UE RLC instance

}; // end of class NrSimpleUeComponentCarrierManager

} // end of namespace ns3

#endif /* NR_SIMPLE_UE_COMPONENT_CARRIER_MANAGER_H */
