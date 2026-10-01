// Copyright (c) 2015 Danilo Abrignani
// Copyright (c) 2016 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Authors: Danilo Abrignani <danilo.abrignani@unibo.it>
//          Biljana Bojovic <biljana.bojovic@cttc.es>
///

#ifndef NR_NO_OP_COMPONENT_CARRIER_MANAGER_H
#define NR_NO_OP_COMPONENT_CARRIER_MANAGER_H

#include "nr-ccm-rrc-sap.h"
#include "nr-gnb-component-carrier-manager.h"
#include "nr-rrc-sap.h"

#include <map>

namespace ns3
{

class NrUeManager;
class NrCcmRrcSapProvider;

/**
 * \brief The default component carrier manager that forwards all traffic, the uplink and the
 * downlink, over the primary carrier, and will not use secondary carriers. To enable carrier
 * aggregation feature, select another component carrier manager class, i.e., some of child classes
 * of NrGnbComponentCarrierManager of NrNoOpComponentCarrierManager.
 */

class NrNoOpComponentCarrierManager : public NrGnbComponentCarrierManager
{
    /// allow GnbMacMemberNrMacSapProvider<NrNoOpComponentCarrierManager> class friend access
    friend class GnbMacMemberNrMacSapProvider<NrNoOpComponentCarrierManager>;
    /// allow MemberNrCcmRrcSapProvider<NrNoOpComponentCarrierManager> class friend access
    friend class MemberNrCcmRrcSapProvider<NrNoOpComponentCarrierManager>;
    /// allow MemberNrCcmRrcSapUser<NrNoOpComponentCarrierManager> class friend access
    friend class MemberNrCcmRrcSapUser<NrNoOpComponentCarrierManager>;
    /// allow MemberNrCcmMacSapUser<NrNoOpComponentCarrierManager> class friend access
    friend class MemberNrCcmMacSapUser<NrNoOpComponentCarrierManager>;

  public:
    NrNoOpComponentCarrierManager();
    ~NrNoOpComponentCarrierManager() override;
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

  protected:
    // Inherited methods
    void DoInitialize() override;
    void DoDispose() override;
    void DoReportUeMeas(uint16_t rnti, NrRrcSap::MeasResults measResults) override;
    /**
     * \brief Add UE.
     * \param rnti the RNTI
     * \param state the state
     */
    virtual void DoAddUe(uint16_t rnti, uint8_t state);
    /**
     * \brief Add LC.
     * \param lcInfo the LC info
     * \param msu the MSU
     */
    virtual void DoAddLc(NrGnbCmacSapProvider::LcInfo lcInfo, NrMacSapUser* msu);
    /**
     * \brief Setup data radio bearer.
     * \param bearer the radio bearer
     * \param bearerId the bearerID
     * \param rnti the RNTI
     * \param lcid the LCID
     * \param lcGroup the LC group
     * \param msu the MSU
     * \returns std::vector<NrCcmRrcSapProvider::LcsConfig>
     */
    virtual std::vector<NrCcmRrcSapProvider::LcsConfig> DoSetupDataRadioBearer(NrEpsBearer bearer,
                                                                               uint8_t bearerId,
                                                                               uint16_t rnti,
                                                                               uint8_t lcid,
                                                                               uint8_t lcGroup,
                                                                               NrMacSapUser* msu);
    /**
     * \brief Transmit PDU.
     * \param params the transmit PDU parameters
     */
    virtual void DoTransmitPdu(NrMacSapProvider::TransmitPduParameters params);
    /**
     * \brief Report buffer status.
     * \param params the report buffer status parameters
     */
    virtual void DoReportBufferStatus(NrMacSapProvider::ReportBufferStatusParameters params);
    /**
     * \brief Notify transmit opportunity.
     *
     * \param txOpParams the NrMacSapUser::TxOpportunityParameters
     */
    virtual void DoNotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams);
    /**
     * \brief Receive PDU.
     *
     * \param rxPduParams the NrMacSapUser::ReceivePduParameters
     */
    virtual void DoReceivePdu(NrMacSapUser::ReceivePduParameters rxPduParams);
    /// Notify HARQ delivery failure
    virtual void DoNotifyHarqDeliveryFailure();
    /**
     * \brief Remove UE.
     * \param rnti the RNTI
     */
    virtual void DoRemoveUe(uint16_t rnti);
    /**
     * \brief Release data radio bearer.
     * \param rnti the RNTI
     * \param lcid the LCID
     * \returns updated data radio bearer list
     */
    virtual std::vector<uint8_t> DoReleaseDataRadioBearer(uint16_t rnti, uint8_t lcid);
    /**
     * \brief Configure the signal bearer.
     * \param lcinfo the NrGnbCmacSapProvider::LcInfo
     * \param msu the MSU
     * \returns updated data radio bearer list
     */
    virtual NrMacSapUser* DoConfigureSignalBearer(NrGnbCmacSapProvider::LcInfo lcinfo,
                                                  NrMacSapUser* msu);
    /**
     * \brief Forwards uplink BSR to CCM, called by MAC through CCM SAP interface.
     * \param bsr the BSR
     * \param componentCarrierId the component carrier ID
     */
    virtual void DoUlReceiveMacCe(nr::MacCeListElement_s bsr, uint8_t componentCarrierId);
    /**
     * \brief Forward uplink SR to CCM, called by MAC through CCM SAP interface.
     * \param rnti RNTI of the UE that requested SR
     * \param componentCarrierId the component carrier ID that forwarded the SR
     */
    virtual void DoUlReceiveSr(uint16_t rnti, uint8_t componentCarrierId);
    /**
     * \brief Function implements the function of the SAP interface of CCM instance which is used by
     * MAC to notify the PRB occupancy reported by scheduler.
     * \param prbOccupancy the PRB occupancy
     * \param componentCarrierId the component carrier ID
     */
    virtual void DoNotifyPrbOccupancy(double prbOccupancy, uint8_t componentCarrierId);

  protected:
    std::map<uint8_t, double>
        m_ccPrbOccupancy; //!< The physical resource block occupancy per carrier.

}; // end of class NrNoOpComponentCarrierManager

/**
 * \brief Component carrier manager implementation that splits traffic equally among carriers.
 */
class NrRrComponentCarrierManager : public NrNoOpComponentCarrierManager
{
  public:
    NrRrComponentCarrierManager();
    ~NrRrComponentCarrierManager() override;
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

  protected:
    // Inherited methods
    void DoReportBufferStatus(NrMacSapProvider::ReportBufferStatusParameters params) override;
    void DoUlReceiveMacCe(nr::MacCeListElement_s bsr, uint8_t componentCarrierId) override;
    void DoUlReceiveSr(uint16_t rnti, uint8_t componentCarrierId) override;

  private:
    uint8_t m_lastCcIdForSr{0}; //!< Last CCID to which a SR was routed
}; // end of class NrRrComponentCarrierManager

} // end of namespace ns3

#endif /* NR_NO_OP_COMPONENT_CARRIER_MANAGER_H */
