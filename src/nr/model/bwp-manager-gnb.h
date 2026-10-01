// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef BWP_MANAGER_H
#define BWP_MANAGER_H

#include "nr-ccm-rrc-sap.h"
#include "nr-eps-bearer.h"
#include "nr-no-op-component-carrier-manager.h"
#include "nr-rlc.h"

#include <unordered_map>

namespace ns3
{
class NrUeManager;
class NrCcmRrcSapProvider;
class BwpManagerAlgorithm;
class NrControlMessage;

/**
 * \ingroup gnb-bwp
 * \brief Bandwidth part manager that coordinates traffic over different bandwidth parts.
 */
class BwpManagerGnb : public NrRrComponentCarrierManager
{
  public:
    BwpManagerGnb();
    ~BwpManagerGnb() override;
    static TypeId GetTypeId();

    /**
     * \brief Set the algorithm
     * \param algorithm pointer to the algorithm
     */
    void SetBwpManagerAlgorithm(const Ptr<BwpManagerAlgorithm>& algorithm);

    /**
     * \brief Get the bwp index for the RNTI and LCID
     * \param rnti The RNTI of the user
     * \param lcid The LCID of the flow that we want to know the bwp index
     * \return The index of the BWP in which that LCID should go
     */
    uint8_t GetBwpIndex(uint16_t rnti, uint8_t lcid);

    /**
     * \brief Get the bwp index for the RNTI and LCID
     * \param rnti The RNTI of the user
     * \param lcid The LCID of the flow that we want to know the bwp index
     * \return The index of the BWP in which that LCID should go
     *
     * Why this method? I wonder if you can imagine it... Hint: Think in a RR
     * algorithm that returns a different bwp index for every call...
     */
    uint8_t PeekBwpIndex(uint16_t rnti, uint8_t lcid) const;

    /**
     * \brief Decide the BWP for the control message received.
     * \param msg Message
     * \param sourceBwpId BWP Id from which this message come from.
     *
     * The routing is made following the bandwidth part reported in the message.
     *
     * \return the BWP Id to which this message should be routed to.
     */
    uint8_t RouteIngoingCtrlMsgs(const Ptr<NrControlMessage>& msg, uint8_t sourceBwpId) const;

    /**
     * \brief Route the outgoing messages to the right BWP
     * \param msg the message
     * \param sourceBwpId the source bwp of the message
     *
     * The routing is made by following the mapping provided through the function
     * SetOutputLink. If no mapping has been installed, or if the sourceBwpId
     * provided is not in the mapping, then forward the message back to the
     * originating BWP.
     *
     * \see SetOutputLink
     *
     * \return the bwp to which the ctrl messages should be redirected
     */
    uint8_t RouteOutgoingCtrlMsg(const Ptr<NrControlMessage>& msg, uint8_t sourceBwpId) const;

    /**
     * \brief Set a mapping between two BWP.
     * \param sourceBwp The messages that come from this value...
     * \param outputBwp ... will get routed in this bandwidth part.
     *
     * Call it for each mapping you want to install.
     */
    void SetOutputLink(uint32_t sourceBwp, uint32_t outputBwp);

  protected:
    /*
     * \brief This function contains most of the BwpManager logic.
     */
    void DoReportBufferStatus(NrMacSapProvider::ReportBufferStatusParameters params) override;

    /*
     * \brief Intercepts function calls from MAC of component carriers when it notifies RLC
     * of transmission opportunities. This function decides id the transmission opportunity
     * will be forwarded to the RLC.
     */
    void DoNotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams) override;

    /**
     * \brief Forwards uplink BSR to CCM, called by MAC through CCM SAP interface.
     * \param bsr the BSR
     * \param componentCarrierId the component carrier ID
     */
    void DoUlReceiveMacCe(nr::MacCeListElement_s bsr, uint8_t componentCarrierId) override;

    /**
     * \brief Forward SR to the right MAC instance through CCM SAP interface
     * \param rnti RNTI of the UE that requested the SR
     * \param componentCarrierId the component carrier ID which received the SR
     */
    void DoUlReceiveSr(uint16_t rnti, uint8_t componentCarrierId) override;

    /**
     * \brief Overload DoSetupBadaRadioBearer to connect directly to Rlc retransmission buffer size.
     */
    std::vector<NrCcmRrcSapProvider::LcsConfig> DoSetupDataRadioBearer(NrEpsBearer bearer,
                                                                       uint8_t bearerId,
                                                                       uint16_t rnti,
                                                                       uint8_t lcid,
                                                                       uint8_t lcGroup,
                                                                       NrMacSapUser* msu) override;

  private:
    /**
     * \brief Get the resource type of the flow.
     * \returns The resource type
     */
    uint8_t GetResourceType(NrMacSapProvider::ReportBufferStatusParameters params);

    Ptr<BwpManagerAlgorithm> m_algorithm; //!< The BWP selection algorithm.

    std::unordered_map<uint32_t, uint32_t> m_outputLinks; //!< Mapping between BWP.
};

} // end of namespace ns3

#endif /* BWP_MANAGER_H */
