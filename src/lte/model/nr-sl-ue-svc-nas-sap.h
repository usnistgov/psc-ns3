/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#ifndef NR_SL_UE_SVC_NAS_SAP_H
#define NR_SL_UE_SVC_NAS_SAP_H

#include "lte-sl-tft.h"
#include "nr-sl-ue-prose-dir-lnk-sap.h"

namespace ns3
{

/**
 * \ingroup lte
 *
 * \brief User part of the Service Access Point (SAP) between the UE service
 *        layer (e.g., the ProSe layer) and the UE NAS
 *
 * This class implements the service Access Point (SAP) for the UE service
 * layer (e.g., the ProSe layer) and the UE NAS. In particular, this class
 * implements the User part of the SAP, i.e., the methods exported by the
 * service layer and called by the UE NAS.
 */
class NrSlUeSvcNasSapUser
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeSvcNasSapUser();

    /**
     * \brief Notify the service layer that an NR SL data radio bearer was
     *        activated for a given peer
     *
     * \param peerL2Id the layer 2 ID of the peer UE
     */
    virtual void NotifySvcNrSlDataRadioBearerActivated(uint32_t peerL2Id) = 0;

    /**
     * \brief Notify the service layer that an NR SL data radio bearer was
     *        removed for a given peer
     *
     * \param peerL2Id the layer 2 ID of the peer UE
     */
    virtual void NotifySvcNrSlDataRadioBearerRemoved(uint32_t peerL2Id) = 0;
};

/**
 * \brief Provider part of the Service Access Point (SAP) between the UE
 *        service layer (e.g., the ProSe layer)  and the UE NAS
 *
 * This class implements the service Access Point (SAP) for the UE service
 * layer (e.g., the ProSe layer)  and the UE NAS. In particular, this class
 * implements the Provider part of the SAP, i.e., the methods exported by the
 * UE NAS and called by the service layer.
 *
 */
class NrSlUeSvcNasSapProvider
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeSvcNasSapProvider();

    /**
     * \brief Instruct the NAS to activate a NR SL data radio bearer with a given
     *        SL traffic template
     *
     * \param tft the SL traffic template used to route data from and to the NR SL
     *            data radio bearer to be activated
     */
    virtual void ActivateSvcNrSlDataRadioBearer(Ptr<LteSlTft> tft) = 0;

    /**
     * \brief Instruct the NAS to delete a NR SL data radio bearer with a given
     *       destination layer 2 ID
     *
     * \param dstL2Id destination layer 2 ID
     */
    virtual void DeleteSvcNrSlDataRadioBearer(Ptr<LteSlTft> tft) = 0;

    /**
     * Instruct the NAS to (re)configure the data bearers (UL and SL where it
     * applies) to have the data packets flowing in the appropriate path after
     * the UEs establish a connection for UE-to-Network (U2N) relay.
     *
     * \param peerL2Id the layer 2 ID of the peer UE
     * \param role the role of this UE in the U2N link (remote UE or relay UE)
     * \param ipInfo the IP configuration associated to the link
     * \param relayDrbId the UL data radio bearer ID used to relay data (used only when the UE has a
     * relay UE role)
     */
    virtual void ConfigureNrSlDataRadioBearersForU2nRelay(
        uint32_t peerL2Id,
        enum NrSlUeProseDirLnkSapUser::U2nRole role,
        NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo,
        uint8_t relayDrbId,
        const struct SidelinkInfo& slInfo) = 0;

    /**
     * Instruct the NAS to remove the data bearers (UL and SL where it
     * applies) to have the data packets flowing in the appropriate path after
     * the UEs release a connection for UE-to-Network (U2N) relay.
     *
     * \param peerL2Id the layer 2 ID of the peer UE
     * \param role the role of this UE in the U2N link (remote UE or relay UE)
     * \param ipInfo the IP configuration associated to the link
     * \param relayDrbId the UL data radio bearer ID used to relay data (used only when the UE has a
     * relay UE role)
     */
    virtual void RemoveNrSlDataRadioBearersForU2nRelay(
        uint32_t peerL2Id,
        enum NrSlUeProseDirLnkSapUser::U2nRole role,
        NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo,
        uint8_t relayDrbId) = 0;
};

/**
 * Template for the implementation of the NrSlUeSvcNasSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeSvcNasSapUser : public NrSlUeSvcNasSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeSvcNasSapUser(C* owner);
    // inherited from NrSlUeSvcNasSapUser
    virtual void NotifySvcNrSlDataRadioBearerActivated(uint32_t peerL2Id);
    virtual void NotifySvcNrSlDataRadioBearerRemoved(uint32_t peerL2Id);

  private:
    MemberNrSlUeSvcNasSapUser();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeSvcNasSapUser<C>::MemberNrSlUeSvcNasSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlUeSvcNasSapUser<C>::MemberNrSlUeSvcNasSapUser()
{
}

template <class C>
void
MemberNrSlUeSvcNasSapUser<C>::NotifySvcNrSlDataRadioBearerActivated(uint32_t peerL2Id)
{
    m_owner->DoNotifySvcNrSlDataRadioBearerActivated(peerL2Id);
}

template <class C>
void
MemberNrSlUeSvcNasSapUser<C>::NotifySvcNrSlDataRadioBearerRemoved(uint32_t peerL2Id)
{
    m_owner->DoNotifySvcNrSlDataRadioBearerRemoved(peerL2Id);
}

/**
 * Template for the implementation of the NrSlUeSvcNasSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeSvcNasSapProvider : public NrSlUeSvcNasSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeSvcNasSapProvider(C* owner);

    // inherited from NrSlUeSvcNasSapProvider
    virtual void ActivateSvcNrSlDataRadioBearer(Ptr<LteSlTft> tft);
    virtual void DeleteSvcNrSlDataRadioBearer(Ptr<LteSlTft> tft);
    virtual void ConfigureNrSlDataRadioBearersForU2nRelay(
        uint32_t peerL2Id,
        enum NrSlUeProseDirLnkSapUser::U2nRole role,
        NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo,
        uint8_t relayDrbId,
        const struct SidelinkInfo& slInfo);
    virtual void RemoveNrSlDataRadioBearersForU2nRelay(
        uint32_t peerL2Id,
        enum NrSlUeProseDirLnkSapUser::U2nRole role,
        NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo,
        uint8_t relayDrbId);

  private:
    MemberNrSlUeSvcNasSapProvider();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeSvcNasSapProvider<C>::MemberNrSlUeSvcNasSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlUeSvcNasSapProvider<C>::MemberNrSlUeSvcNasSapProvider()
{
}

template <class C>
void
MemberNrSlUeSvcNasSapProvider<C>::ActivateSvcNrSlDataRadioBearer(Ptr<LteSlTft> tft)
{
    m_owner->DoActivateSvcNrSlDataRadioBearer(tft);
}

template <class C>
void
MemberNrSlUeSvcNasSapProvider<C>::DeleteSvcNrSlDataRadioBearer(Ptr<LteSlTft> tft)
{
    m_owner->DoDeleteSvcNrSlDataRadioBearer(tft);
}

template <class C>
void
MemberNrSlUeSvcNasSapProvider<C>::ConfigureNrSlDataRadioBearersForU2nRelay(
    uint32_t peerL2Id,
    enum NrSlUeProseDirLnkSapUser::U2nRole role,
    NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo,
    uint8_t relayDrbId,
    const struct SidelinkInfo& slInfo)
{
    m_owner->DoConfigureNrSlDataRadioBearersForU2nRelay(peerL2Id, role, ipInfo, relayDrbId, slInfo);
}

template <class C>
void
MemberNrSlUeSvcNasSapProvider<C>::RemoveNrSlDataRadioBearersForU2nRelay(
    uint32_t peerL2Id,
    enum NrSlUeProseDirLnkSapUser::U2nRole role,
    NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo,
    uint8_t relayDrbId)
{
    m_owner->DoRemoveNrSlDataRadioBearersForU2nRelay(peerL2Id, role, ipInfo, relayDrbId);
}

} // namespace ns3

#endif /* NR_SL_UE_SVC_NAS_SAP_H */
