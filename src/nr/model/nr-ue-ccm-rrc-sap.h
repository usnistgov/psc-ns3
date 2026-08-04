// Copyright (c) 2015 Danilo Abrignani
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Danilo Abrignani <danilo.abrignani@unibo.it>

#ifndef NR_UE_CCM_RRC_SAP_H
#define NR_UE_CCM_RRC_SAP_H

#include "nr-mac-sap.h"
#include "nr-ue-cmac-sap.h"

#include <map>

namespace ns3
{
class NrUeCmacSapProvider;
class NrMacSapUser;

/**
 * \brief Service Access Point (SAP) offered by the UE component carrier manager
 *  to the UE RRC.
 *
 * This is the *Component Carrier Management SAP Provider*, i.e., the part of the SAP
 * that contains the component carrier manager methods called by the Ue RRC
 * instance.
 */
class NrUeCcmRrcSapProvider
{
    /// allow  NrMacSapUser class friend access
    friend class NrMacSapUser;

  public:
    virtual ~NrUeCcmRrcSapProvider() = default;

    /// LcsConfig structure
    struct LcsConfig
    {
        uint8_t componentCarrierId;                         ///< component carrier ID
        NrUeCmacSapProvider::LogicalChannelConfig lcConfig; ///< logical channel config
        NrMacSapUser* msu;                                  ///< MSU
    };

    /**
     * add a new Logical Channel (LC)
     *
     * \param lcId is the Logical Channel Id
     * \param lcConfig is a single structure contains logical Channel Id, Logical Channel config and
     * Component Carrier Id
     * \param msu is the pointer to NrMacSapUser related to the Rlc instance
     * \return vector of LcsConfig contains the lc configuration for each Mac
     *                the size of the vector is equal to the number of component
     *                carrier enabled.
     *
     * The Logical Channel configurations for each component carrier depend on the
     * algorithm used to split the traffic between the component carriers themself.
     */
    virtual std::vector<NrUeCcmRrcSapProvider::LcsConfig> AddLc(
        uint8_t lcId,
        NrUeCmacSapProvider::LogicalChannelConfig lcConfig,
        NrMacSapUser* msu) = 0;

    /**
     * \brief Remove an existing Logical Channel for a Ue in the NrUeComponentCarrierManager
     * \param lcid the Logical Channel Id
     * \return vector of integer the componentCarrierId of the NrComponentCarrier
     *                where the bearer is enabled
     */
    virtual std::vector<uint16_t> RemoveLc(uint8_t lcid) = 0;
    /**
     * \brief Reset LC maps
     *
     */
    virtual void Reset() = 0;
    /// Notify reconfiguration msg function
    virtual void NotifyConnectionReconfigurationMsg() = 0;

    /**
     * \brief Add the Signal Bearer for a specific Ue in NrUeComponenCarrierManager
     * \param lcid the Logical Channel Id
     * \param lcConfig this structure it is hard-coded in the NrGnbRrc
     * \param msu it is the MacSapUser of the Rlc instance
     * \return the NrMacSapUser of the ComponentCarrierManager
     *
     */
    virtual NrMacSapUser* ConfigureSignalBearer(uint8_t lcid,
                                                NrUeCmacSapProvider::LogicalChannelConfig lcConfig,
                                                NrMacSapUser* msu) = 0;

}; // end of class NrUeCcmRrcSapProvider

/// MemberNrUeCcmRrcSapProvider class
template <class C>
class MemberNrUeCcmRrcSapProvider : public NrUeCcmRrcSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrUeCcmRrcSapProvider(C* owner);

    // inherited from NrUeCcmRrcSapProvider
    std::vector<uint16_t> RemoveLc(uint8_t lcid) override;
    void Reset() override;
    std::vector<NrUeCcmRrcSapProvider::LcsConfig> AddLc(
        uint8_t lcId,
        NrUeCmacSapProvider::LogicalChannelConfig lcConfig,
        NrMacSapUser* msu) override;
    void NotifyConnectionReconfigurationMsg() override;
    NrMacSapUser* ConfigureSignalBearer(uint8_t lcid,
                                        NrUeCmacSapProvider::LogicalChannelConfig lcConfig,
                                        NrMacSapUser* msu) override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrUeCcmRrcSapProvider<C>::MemberNrUeCcmRrcSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
std::vector<uint16_t>
MemberNrUeCcmRrcSapProvider<C>::RemoveLc(uint8_t lcid)
{
    return m_owner->DoRemoveLc(lcid);
}

template <class C>
void
MemberNrUeCcmRrcSapProvider<C>::Reset()
{
    return m_owner->DoReset();
}

template <class C>
std::vector<NrUeCcmRrcSapProvider::LcsConfig>
MemberNrUeCcmRrcSapProvider<C>::AddLc(uint8_t lcId,
                                      NrUeCmacSapProvider::LogicalChannelConfig lcConfig,
                                      NrMacSapUser* msu)
{
    return m_owner->DoAddLc(lcId, lcConfig, msu);
}

template <class C>
void
MemberNrUeCcmRrcSapProvider<C>::NotifyConnectionReconfigurationMsg()
{
    NS_FATAL_ERROR("Function should not be called because it is not implemented.");
    // m_owner->DoNotifyConnectionReconfigurationMsg ();
}

template <class C>
NrMacSapUser*
MemberNrUeCcmRrcSapProvider<C>::ConfigureSignalBearer(
    uint8_t lcid,
    NrUeCmacSapProvider::LogicalChannelConfig lcConfig,
    NrMacSapUser* msu)
{
    return m_owner->DoConfigureSignalBearer(lcid, lcConfig, msu);
}

/**
 * \brief Service Access Point (SAP) offered by the UE RRC to the UE CCM.
 *
 * This is the *Component Carrier Management SAP User*, i.e., the part of the SAP
 * that contains the UE RRC methods called by the UE CCM instance.
 */
class NrUeCcmRrcSapUser
{
  public:
    virtual ~NrUeCcmRrcSapUser() = default;

    /**
     * this function will be used after the RRC notify to ComponentCarrierManager
     * that a reconfiguration message with Secondary component carrier (SCc) arrived or not
     * the method it is called only if the SCc wasn't set up
     * \param componentCarrierList component carrier list
     */
    virtual void ComponentCarrierEnabling(std::vector<uint8_t> componentCarrierList) = 0;
    /**
     * \brief Set the number of component carriers
     *
     * \param noOfComponentCarriers The number of component carriers
     */
    virtual void SetNumberOfComponentCarriers(uint16_t noOfComponentCarriers) = 0;

}; // end of class NrUeCcmRrcSapUser

/// MemberNrUeCcmRrcSapUser class
template <class C>
class MemberNrUeCcmRrcSapUser : public NrUeCcmRrcSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrUeCcmRrcSapUser(C* owner);
    // inherited from NrUeCcmRrcSapUser
    void ComponentCarrierEnabling(std::vector<uint8_t> componentCarrierList) override;
    void SetNumberOfComponentCarriers(uint16_t noOfComponentCarriers) override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrUeCcmRrcSapUser<C>::MemberNrUeCcmRrcSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
void
MemberNrUeCcmRrcSapUser<C>::ComponentCarrierEnabling(std::vector<uint8_t> componentCarrierList)
{
    NS_FATAL_ERROR("Function should not be called because it is not implemented.");
    // m_owner->DoComponentCarrierEnabling (componentCarrierList);
}

template <class C>
void
MemberNrUeCcmRrcSapUser<C>::SetNumberOfComponentCarriers(uint16_t noOfComponentCarriers)
{
    m_owner->DoSetNumberOfComponentCarriers(noOfComponentCarriers);
}

} // end of namespace ns3

#endif /* NR_UE_CCM_RRC_SAP_H */
