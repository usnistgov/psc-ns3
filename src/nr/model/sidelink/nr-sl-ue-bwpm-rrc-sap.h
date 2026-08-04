/*
 *   Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef NR_SL_UE_BWPM_RRC_SAP_H
#define NR_SL_UE_BWPM_RRC_SAP_H

#include "nr-sl-ue-cmac-sap.h"

#include "ns3/nr-eps-bearer.h"
#include "ns3/nr-mac-sap.h"
#include "ns3/nr-rrc-sap.h"
#include "ns3/nr-ue-ccm-rrc-sap.h"
#include "ns3/nr-ue-cmac-sap.h"

#include <map>

namespace ns3
{

class NrMacSapUser;

/**
 * \ingroup nr
 *
 * \brief Service Access Point (SAP) offered by the UE bandwidth part manager
 *  to the UE RRC for Sidelink.
 *
 * This is the *NR SL Bandwidth part Manager SAP Provider*, i.e., the part of
 * the SAP that contains the Sidelink bandwidth part manager methods called
 * by the UE RRC instance.
 */
class NrSlUeBwpmRrcSapProvider
{
  public:
    virtual ~NrSlUeBwpmRrcSapProvider();

    /// SlLcsInfo structure
    struct SlLcInfoBwpm
    {
        uint8_t bwpId;                                            ///< bandwidth part ID
        NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo lcInfo; ///< logical channel config
        NrSlMacSapUser* msu;                                      ///< MAC SAP User
    };

    /**
     * \brief Add a new NR Sidelink Data Radio Bearer Logical Channel (LC)
     *
     * \param lcInfo is the Sidelink Logical Channel Information
     * \param msu is the pointer to NrSlMacSapUser, which MAC uses to call RLC methods
     * \return vector of LcsConfig contains the lc configuration for each MAC
     *         the size of the vector is equal to the number of bandwidth part manager enabled.
     */
    virtual std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm> AddNrSlDrbLc(
        const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& lcInfo,
        NrSlMacSapUser* msu) = 0;

    /**
     * \brief Remove an existing NR Sidelink Data Radio Bearer Logical Channel for a UE in the
     * NrSlBwpManagerUe
     *
     * \param slLcId is the Sidelink Logical Channel Id
     * \param srcL2Id is the Source L2 ID
     * \param dstL2Id is the Destination L2 ID
     *
     * \return A vector containing the BWP ids for which the LC was removed.
     */
    virtual std::vector<uint8_t> RemoveNrSlDrbLc(uint8_t slLcId,
                                                 uint32_t srcL2Id,
                                                 uint32_t dstL2Id) = 0;
    /**
     * \brief Reset NR Sidelink Data Radio Bearer LC map
     *
     */
    virtual void ResetNrSlDrbLcMap() = 0;
    /**
     * \brief Set Bwp Id Container
     *
     * \param bwpIdVec The container of SL BWP ids
     */
    virtual void SetBwpIdContainer(const std::set<uint8_t>& bwpIdVec) = 0;

    /**
     * \brief Add a new NR Sidelink Signalling Radio Bearer Logical Channel (LC)
     *
     * \param lcInfo is the Sidelink Logical Channel Information
     * \param msu is the pointer to NrSlMacSapUser, which MAC uses to call RLC methods
     * \return vector of SlLcInfoBwpm contains the LC configuration for each MAC
     *         the size of the vector is equal to the number of bandwidth part manager enabled.
     */
    virtual std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm> AddNrSlSrbLc(
        const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& lcInfo,
        NrSlMacSapUser* msu) = 0;

    /**
     * \brief Add a new NR Sidelink Discovery Radio Bearer Logical Channel (LC)
     *
     * \param lcInfo is the Sidelink Logical Channel Information
     * \param msu is the pointer to NrSlMacSapUser, which MAC uses to call RLC methods
     * \return vector of SlLcInfoBwpm contains the LC configuration for each MAC
     *         the size of the vector is equal to the number of bandwidth part manager enabled.
     */
    virtual std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm> AddNrSlDiscoveryRbLc(
        const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& lcInfo,
        NrSlMacSapUser* msu) = 0;

}; // end of class NrSlUeBwpmRrcSapProvider

/**
 * \ingroup nr
 *
 * Template for the implementation of the NrSlUeBwpmRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to NR SL UE BWP manager class, which are
 * called by UE RRC to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlUeBwpmRrcSapProvider : public NrSlUeBwpmRrcSapProvider
{
  public:
    /**
     * \brief Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeBwpmRrcSapProvider(C* owner);

    // inherited from NrSlUeBwpmRrcSapProvider
    std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm> AddNrSlDrbLc(
        const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& lcInfo,
        NrSlMacSapUser* msu) override;
    std::vector<uint8_t> RemoveNrSlDrbLc(uint8_t slLcId,
                                         uint32_t srcL2Id,
                                         uint32_t dstL2Id) override;
    void ResetNrSlDrbLcMap() override;
    void SetBwpIdContainer(const std::set<uint8_t>& bwpIdVec) override;
    std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm> AddNrSlSrbLc(
        const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& lcInfo,
        NrSlMacSapUser* msu) override;
    std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm> AddNrSlDiscoveryRbLc(
        const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& lcInfo,
        NrSlMacSapUser* msu) override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeBwpmRrcSapProvider<C>::MemberNrSlUeBwpmRrcSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm>
MemberNrSlUeBwpmRrcSapProvider<C>::AddNrSlDrbLc(
    const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& lcInfo,
    NrSlMacSapUser* msu)
{
    return m_owner->DoAddNrSlDrbLc(lcInfo, msu);
}

template <class C>
std::vector<uint8_t>
MemberNrSlUeBwpmRrcSapProvider<C>::RemoveNrSlDrbLc(uint8_t slLcId,
                                                   uint32_t srcL2Id,
                                                   uint32_t dstL2Id)
{
    return m_owner->DoRemoveNrSlDrbLc(slLcId, srcL2Id, dstL2Id);
}

template <class C>
void
MemberNrSlUeBwpmRrcSapProvider<C>::ResetNrSlDrbLcMap()
{
    return m_owner->DoResetNrSlDrbLcMap();
}

template <class C>
void
MemberNrSlUeBwpmRrcSapProvider<C>::SetBwpIdContainer(const std::set<uint8_t>& bwpIdVec)
{
    return m_owner->DoSetBwpIdContainer(bwpIdVec);
}

template <class C>
std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm>
MemberNrSlUeBwpmRrcSapProvider<C>::AddNrSlSrbLc(
    const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& lcInfo,
    NrSlMacSapUser* msu)
{
    return m_owner->DoAddNrSlSrbLc(lcInfo, msu);
}

template <class C>
std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm>
MemberNrSlUeBwpmRrcSapProvider<C>::AddNrSlDiscoveryRbLc(
    const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& lcInfo,
    NrSlMacSapUser* msu)
{
    return m_owner->DoAddNrSlDiscoveryRbLc(lcInfo, msu);
}

/**
 * \ingroup nr
 *
 * \brief Service Access Point (SAP) offered by the UE RRC to the UE
 *        sidelink bandwidth part manager.
 *
 * This is the *Bandwidth part Manager SAP User*, i.e., the part of the SAP
 * that contains the UE RRC methods called by the Sidelink UE bandwidth part
 * manager instance.
 */
class NrSlUeBwpmRrcSapUser
{
  public:
    virtual ~NrSlUeBwpmRrcSapUser();

}; // end of class NrSlUeBwpmRrcSapUser

/**
 * \ingroup nr
 *
 * Template for the implementation of the NrSlUeBwpmRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to UE RRC class, which are
 * called by NR SL BWP manager class to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlUeBwpmRrcSapUser : public NrSlUeBwpmRrcSapUser
{
  public:
    /**
     * \brief Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeBwpmRrcSapUser(C* owner);

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeBwpmRrcSapUser<C>::MemberNrSlUeBwpmRrcSapUser(C* owner)
    : m_owner(owner)
{
}

} // end of namespace ns3

#endif /* NR_SL_UE_BWPM_RRC_SAP_H */
