// Copyright (c) 2015 Danilo Abrignani
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Danilo Abrignani <danilo.abrignani@unibo.it>

#ifndef NR_CCM_RRC_SAP_H
#define NR_CCM_RRC_SAP_H

#include "nr-eps-bearer.h"
#include "nr-gnb-cmac-sap.h"
#include "nr-mac-sap.h"
#include "nr-rrc-sap.h"

#include <map>

namespace ns3
{
class NrUeCmacSapProvider;
class NrUeManager;
class NrGnbCmacSapProvider;
class NrMacSapUser;
class NrRrcSap;

/**
 * \brief Service Access Point (SAP) offered by the Component Carrier Manager (CCM)
 * instance to the eNodeB RRC instance.
 *
 * This is the *Component Carrier Manager SAP Provider*, i.e., the part of the SAP
 * that contains the CCM methods called by the eNodeB RRC instance.
 */
class NrCcmRrcSapProvider
{
    /// allow NrUeManager class friend access
    friend class NrUeManager;
    /// allow NrMacSapUser class friend access
    friend class NrMacSapUser;

  public:
    virtual ~NrCcmRrcSapProvider() = default;

    /// LcsConfig structure
    struct LcsConfig
    {
        uint16_t componentCarrierId;     ///< component carrier ID
        NrGnbCmacSapProvider::LcInfo lc; ///< LC info
        NrMacSapUser* msu;               ///< MSU
    };

    /**
     * \brief Reports UE measurements to the component carrier manager.
     * \param rnti Radio Network Temporary Identity, an integer identifying
     * the UE where the measurement report originates from.
     * \param measResults a single report of one measurement identity
     *
     * The received measurement report is a result of the UE measurements configuration
     * previously configured by calling
     * NrCcmRrcSapProvider::AddUeMeasReportConfigForComponentCarrier. The report may be stored and
     * utilized for the purpose of making decision if and when to use the secondary carriers.
     */
    virtual void ReportUeMeas(uint16_t rnti, NrRrcSap::MeasResults measResults) = 0;

    /**
     * \brief Add a new UE in the NrGnbComponentCarrierManager.
     * \param rnti Radio Network Temporary Identity, an integer identifying the UE.
     * \param state The current rrc state of the UE.
     */
    virtual void AddUe(uint16_t rnti, uint8_t state) = 0;

    /**
     * \brief Add a new logical channel.
     * \param lcInfo - information about newly created logical channel
     * \param msu - pointer to corresponding rlc interface
     *
     */
    virtual void AddLc(NrGnbCmacSapProvider::LcInfo lcInfo, NrMacSapUser* msu) = 0;

    /**
     * \brief Remove an existing UE.
     * \param rnti Radio Network Temporary Identity, an integer identifying the UE
     *             where the report originates from
     */
    virtual void RemoveUe(uint16_t rnti) = 0;

    /**
     * \brief Add a new Bearer for the Ue in the NrGnbComponentCarrierManager.
     * \param bearer a pointer to the NrEpsBearer object
     * \param bearerId a unique identifier for the bearer
     * \param rnti Radio Network Temporary Identity, an integer identifying the UE
     *             where the report originates from
     * \param lcid the Logical Channel id
     * \param lcGroup the Logical Channel group
     * \param msu a pointer to the NrMacSapUser, the NrGnbComponentCarrierManager
     *             has to store a NrMacSapUser for each Rlc instance, in order to
     *             properly redirect the packet
     * \return vector of LcsConfig contains the lc configuration for each Mac
     *                the size of the vector is equal to the number of component
     *                carrier enabled.
     *
     * The Logical Channel configurations for each component carrier depend on the
     * algorithm used to split the traffic between the component carriers themself.
     */
    virtual std::vector<NrCcmRrcSapProvider::LcsConfig> SetupDataRadioBearer(NrEpsBearer bearer,
                                                                             uint8_t bearerId,
                                                                             uint16_t rnti,
                                                                             uint8_t lcid,
                                                                             uint8_t lcGroup,
                                                                             NrMacSapUser* msu) = 0;

    /**
     * \brief Release an existing Data Radio Bearer for a Ue in the NrGnbComponentCarrierManager
     * \param rnti Radio Network Temporary Identity, an integer identifying the UE
     *             where the report originates from
     * \param lcid the Logical Channel Id
     * \return vector of integer the componentCarrierId of the NrComponentCarrier
     *                where the bearer is enabled
     */

    virtual std::vector<uint8_t> ReleaseDataRadioBearer(uint16_t rnti, uint8_t lcid) = 0;

    /**
     * \brief Add the Signal Bearer for a specific Ue in NrGnbComponenCarrierManager
     * \param lcInfo this structure it is hard-coded in the NrGnbRrc
     * \param rlcMacSapUser it is the MacSapUser of the Rlc instance
     * \return the NrMacSapUser of the ComponentCarrierManager
     *
     */
    virtual NrMacSapUser* ConfigureSignalBearer(NrGnbCmacSapProvider::LcInfo lcInfo,
                                                NrMacSapUser* rlcMacSapUser) = 0;

}; // end of class NrCcmRrcSapProvider

/**
 * \brief Service Access Point (SAP) offered by the eNodeB RRC instance to the
 *        component carrier manager (CCM) instance.
 *
 * This is the *Component Carrier Management SAP User*, i.e., the part of the SAP that
 * contains the eNodeB RRC methods called by the CCM.
 */
class NrCcmRrcSapUser
{
    /// allow NrGnbRrc class friend access
    friend class NrGnbRrc;

  public:
    virtual ~NrCcmRrcSapUser() = default;

    /**
     * \brief Request a certain reporting configuration to be fulfilled by the UEs
     *        attached to the eNodeB entity.
     * \param reportConfig the UE measurement reporting configuration
     * \return the measurement identity associated with this newly added
     *         reporting configuration
     *
     * The eNodeB RRC entity is expected to configure the same reporting
     * configuration in each of the attached UEs. When later in the simulation a
     * UE measurement report is received from a UE as a result of this
     * configuration, the eNodeB RRC entity shall forward this report to the
     * NrComponentCarrier algorithm through the NrCcmRrcSapProvider::ReportUeMeas
     * SAP function.
     *
     * \note This function is only valid before the simulation begins.
     */
    virtual uint8_t AddUeMeasReportConfigForComponentCarrier(
        NrRrcSap::ReportConfigEutra reportConfig) = 0;

    /**
     * \brief Instruct the eNodeB RRC entity to prepare a component carrier.
     * \param rnti Radio Network Temporary Identity, an integer identifying the
     *             UE which shall perform the NrComponentCarrier
     * \param targetCellId the cell ID of the target eNodeB
     *
     * This function is used by the NrComponentCarrier manager when a decision on
     * component carriers configurations.
     *
     * The process to produce the decision is up to the implementation of NrComponentCarrier
     * algorithm. It is typically based on the reported UE measurements, which are
     * received through the NrCcmRrcSapProvider::ReportUeMeas function.
     */
    virtual void TriggerComponentCarrier(uint16_t rnti, uint16_t targetCellId) = 0;

    /**
     * add a new Logical Channel (LC)
     *
     * \param lcConfig is a single structure contains logical Channel Id, Logical Channel config and
     * Component Carrier Id
     */
    virtual void AddLcs(std::vector<NrGnbRrcSapProvider::LogicalChannelConfig> lcConfig) = 0;

    /**
     * remove an existing LC
     *
     * \param rnti
     * \param lcid
     */
    virtual void ReleaseLcs(uint16_t rnti, uint8_t lcid) = 0;

    /**
     * Get UE manager by RNTI
     *
     * \param rnti RNTI
     * \return UE manager
     */
    virtual Ptr<NrUeManager> GetUeManager(uint16_t rnti) = 0;

    /**
     * \brief Set the number of component carriers
     *
     * \param noOfComponentCarriers The number of component carriers
     */
    virtual void SetNumberOfComponentCarriers(uint16_t noOfComponentCarriers) = 0;

}; // end of class NrCcmRrcSapUser

/// MemberNrCcmRrcSapProvider class
template <class C>
class MemberNrCcmRrcSapProvider : public NrCcmRrcSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrCcmRrcSapProvider(C* owner);

    // inherited from NrCcmRrcSapProvider
    void ReportUeMeas(uint16_t rnti, NrRrcSap::MeasResults measResults) override;
    void AddUe(uint16_t rnti, uint8_t state) override;
    void AddLc(NrGnbCmacSapProvider::LcInfo lcInfo, NrMacSapUser* msu) override;
    void RemoveUe(uint16_t rnti) override;
    std::vector<NrCcmRrcSapProvider::LcsConfig> SetupDataRadioBearer(NrEpsBearer bearer,
                                                                     uint8_t bearerId,
                                                                     uint16_t rnti,
                                                                     uint8_t lcid,
                                                                     uint8_t lcGroup,
                                                                     NrMacSapUser* msu) override;
    std::vector<uint8_t> ReleaseDataRadioBearer(uint16_t rnti, uint8_t lcid) override;
    NrMacSapUser* ConfigureSignalBearer(NrGnbCmacSapProvider::LcInfo lcInfo,
                                        NrMacSapUser* rlcMacSapUser) override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrCcmRrcSapProvider<C>::MemberNrCcmRrcSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
void
MemberNrCcmRrcSapProvider<C>::ReportUeMeas(uint16_t rnti, NrRrcSap::MeasResults measResults)
{
    m_owner->DoReportUeMeas(rnti, measResults);
}

template <class C>
void
MemberNrCcmRrcSapProvider<C>::AddUe(uint16_t rnti, uint8_t state)
{
    m_owner->DoAddUe(rnti, state);
}

template <class C>
void
MemberNrCcmRrcSapProvider<C>::AddLc(NrGnbCmacSapProvider::LcInfo lcInfo, NrMacSapUser* msu)
{
    m_owner->DoAddLc(lcInfo, msu);
}

template <class C>
void
MemberNrCcmRrcSapProvider<C>::RemoveUe(uint16_t rnti)
{
    m_owner->DoRemoveUe(rnti);
}

template <class C>
std::vector<NrCcmRrcSapProvider::LcsConfig>
MemberNrCcmRrcSapProvider<C>::SetupDataRadioBearer(NrEpsBearer bearer,
                                                   uint8_t bearerId,
                                                   uint16_t rnti,
                                                   uint8_t lcid,
                                                   uint8_t lcGroup,
                                                   NrMacSapUser* msu)
{
    return m_owner->DoSetupDataRadioBearer(bearer, bearerId, rnti, lcid, lcGroup, msu);
}

template <class C>
std::vector<uint8_t>
MemberNrCcmRrcSapProvider<C>::ReleaseDataRadioBearer(uint16_t rnti, uint8_t lcid)
{
    return m_owner->DoReleaseDataRadioBearer(rnti, lcid);
}

template <class C>
NrMacSapUser*
MemberNrCcmRrcSapProvider<C>::ConfigureSignalBearer(NrGnbCmacSapProvider::LcInfo lcInfo,
                                                    NrMacSapUser* rlcMacSapUser)
{
    return m_owner->DoConfigureSignalBearer(lcInfo, rlcMacSapUser);
}

/// MemberNrCcmRrcSapUser class
template <class C>
class MemberNrCcmRrcSapUser : public NrCcmRrcSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrCcmRrcSapUser(C* owner);

    // inherited from NrCcmRrcSapUser
    void AddLcs(std::vector<NrGnbRrcSapProvider::LogicalChannelConfig> lcConfig) override;
    void ReleaseLcs(uint16_t rnti, uint8_t lcid) override;
    uint8_t AddUeMeasReportConfigForComponentCarrier(
        NrRrcSap::ReportConfigEutra reportConfig) override;
    void TriggerComponentCarrier(uint16_t rnti, uint16_t targetCellId) override;
    Ptr<NrUeManager> GetUeManager(uint16_t rnti) override;
    void SetNumberOfComponentCarriers(uint16_t noOfComponentCarriers) override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrCcmRrcSapUser<C>::MemberNrCcmRrcSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
void
MemberNrCcmRrcSapUser<C>::AddLcs(std::vector<NrGnbRrcSapProvider::LogicalChannelConfig> lcConfig)
{
    NS_FATAL_ERROR("Function should not be called because it is not implemented.");
    // m_owner->DoAddLcs (lcConfig);
}

template <class C>
void
MemberNrCcmRrcSapUser<C>::ReleaseLcs(uint16_t rnti, uint8_t lcid)
{
    NS_FATAL_ERROR("Function should not be called because it is not implemented.");
    // m_owner->DoReleaseLcs (rnti, lcid);
}

template <class C>
uint8_t
MemberNrCcmRrcSapUser<C>::AddUeMeasReportConfigForComponentCarrier(
    NrRrcSap::ReportConfigEutra reportConfig)
{
    return m_owner->DoAddUeMeasReportConfigForComponentCarrier(reportConfig);
}

template <class C>
void
MemberNrCcmRrcSapUser<C>::TriggerComponentCarrier(uint16_t rnti, uint16_t targetCellId)
{
    NS_FATAL_ERROR("Function should not be called because it is not implemented.");
}

template <class C>
Ptr<NrUeManager>
MemberNrCcmRrcSapUser<C>::GetUeManager(uint16_t rnti)
{
    return m_owner->GetUeManager(rnti);
}

template <class C>
void
MemberNrCcmRrcSapUser<C>::SetNumberOfComponentCarriers(uint16_t noOfComponentCarriers)
{
    return m_owner->DoSetNumberOfComponentCarriers(noOfComponentCarriers);
}

} // end of namespace ns3

#endif /* NR_CCM_RRC_SAP_H */
