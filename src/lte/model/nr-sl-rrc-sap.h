/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef NR_SL_RRC_SAP_H
#define NR_SL_RRC_SAP_H

#include "lte-rrc-sap.h"

namespace ns3
{

class NrSlDataRadioBearerInfo;
class NrSlSignallingRadioBearerInfo;
class NrSlDiscoveryRadioBearerInfo;

/**
 * \ingroup lte
 *
 * \brief User part of the Service Access Point (SAP) between UE RRC and NR
 *        sidelink UE RRC.
 *
 * This class implements the service Access Point (SAP) for NR sidelink
 * UE RRC, i.e., the interface between the LteUeRrc and the NrSlUeRrc. In
 * particular, this class implements the User part of the
 * SAP, i.e., the sidelink related methods exported by the
 * NrSlUeRrc and called by the LteUeRrc.
 */
class NrSlUeRrcSapUser
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeRrcSapUser();
    /**
     * \brief Get NR sidelink preconfiguration
     *
     * \return The sidelink preconfiguration
     */
    virtual const LteRrcSap::SidelinkPreconfigNr GetNrSlPreconfiguration() = 0;
    /**
     * \brief Get the physical sidelink pool based on SL bitmap and the TDD pattern
     *
     * \param slBitMap The sidelink bitmap
     * \return A vector representing the physical sidelink pool
     */
    virtual const std::vector<std::bitset<1>> GetPhysicalSlPool(
        const std::vector<std::bitset<1>>& slBitMap) = 0;
    /**
     * \brief Get Bwp Id Container
     *
     * \return The container of SL BWP ids
     */
    virtual const std::set<uint8_t> GetBwpIdContainer() = 0;
    /**
     * \brief Add NR sidelink data radio bearer
     *
     * Attempts to add a sidelink radio bearer
     *
     * \param slTxDrb LteSidelinkRadioBearerInfo pointer
     */
    virtual void AddNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb) = 0;
    /**
     * \brief Add NR Reception sidelink data radio bearer
     *
     * Attempts to add a sidelink radio bearer for RX
     *
     * \param slRxDrb LteSidelinkRadioBearerInfo pointer
     */
    virtual void AddNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb) = 0;
    /**
     * \brief Get NR Sidelink data radio bearer
     *
     * \param dstL2Id The remote/destination layer 2 id
     * \param lcId The logical channel id
     * \return The NrSlDataRadioBearerInfo
     */
    virtual Ptr<NrSlDataRadioBearerInfo> GetSidelinkTxDataRadioBearer(uint32_t dstL2Id,
                                                                      uint8_t lcId) = 0;
    /**
     * \brief Get all NR Sidelink Tx data radio bearers to a destination L2 Id
     *
     * \param dstL2Id The remote/destination layer 2 id
     * \return A map containing all of the NrSlDataRadioBearerInfo, by logical channel ID
     */
    virtual std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
    GetAllSidelinkTxDataRadioBearers(uint32_t dstL2Id) = 0;
    /**
     * \brief Get NR Sidelink Rx data radio bearer
     *
     * \param srcL2Id The source layer 2 id
     * \param lcId The logical channel id
     * \return The NrSlDataRadioBearerInfo
     */
    virtual Ptr<NrSlDataRadioBearerInfo> GetSidelinkRxDataRadioBearer(uint32_t srcL2Id,
                                                                      uint8_t lcId) = 0;
    /**
     * \brief Get all NR Sidelink Rx data radio bearers from a source L2 ID
     *
     * \param srcL2Id The source layer 2 id
     * \return A map containing all of the NrSlDataRadioBearerInfo, by logical channel ID
     */
    virtual std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
    GetAllSidelinkRxDataRadioBearers(uint32_t srcL2Id) = 0;

    /**
     * \brief Remove NR SL data bearer transmission
     *
     * \param slTxdDb NrSlDataRadioBearerInfo pointer
     */
    virtual void RemoveNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb) = 0;

    /**
     * \brief Remove NR SL data bearer for reception
     *
     * \param slRxDrb NrSlDataRadioBearerInfo pointer
     */
    virtual void RemoveNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb) = 0;
    /**
     * \brief Get next LCID for setting up NR SL DRB towards the given destination
     *
     * As per, table 6.2.4-1 of 38.321 LCID for SL-SCH range from 4-19, i.e.,
     * total 16 LCIDs
     *
     * \param dstL2Id The destination layer 2 ID
     * \return the next available NR SL DRB LCID
     */
    virtual uint8_t GetNextLcid(uint32_t dstL2Id) = 0;
    /**
     * \brief Add NR sidelink signalling radio bearer for transmission
     *
     * \param slSrb NrSlSignallingRadioBearerInfo pointer
     */
    virtual void AddTxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb) = 0;
    /**
     * \brief Add NR sidelink signalling radio bearer for reception
     *
     * \param slSrb NrSlSignallingRadioBearerInfo pointer
     */
    virtual void AddRxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb) = 0;
    /**
     * \brief Get NR Sidelink signalling radio bearer for transmission
     *
     * \param dstL2Id The peer layer 2 id
     * \param lcId the logical channel id
     * \return The NrSlSignallingRadioBearerInfo
     */
    virtual Ptr<NrSlSignallingRadioBearerInfo> GetTxNrSlSignallingRadioBearer(uint32_t dstL2Id,
                                                                              uint8_t lcId) = 0;
    /**
     * \brief Get the UE layer 2 ID
     *
     * \return The layer 2 Id
     */
    virtual uint32_t GetSourceL2Id() = 0;
    /**
     * \brief Add NR sidelink discovery radio bearer for transmission
     *
     * \param slDiscRb NrSlDiscoveryRadioBearerInfo pointer
     */
    virtual void AddTxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRb) = 0;
    /**
     * \brief Add NR sidelink discovery radio bearer for reception
     *
     * \param slDiscRb NrSlDiscoveryRadioBearerInfo pointer
     */
    virtual void AddRxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRb) = 0;
    /**
     * \brief Get NR Sidelink discovery radio bearer for transmission
     *
     * \param dstL2Id The peer layer 2 id
     * \return The NrSlDiscoveryRadioBearerInfo
     */
    virtual Ptr<NrSlDiscoveryRadioBearerInfo> GetTxNrSlDiscoveryRadioBearer(uint32_t dstL2Id) = 0;
};

/**
 * \brief Provider part of the Service Access Point (SAP) between UE RRC and NR
 *        sidelink UE RRC.
 *
 * This class implements the service Access Point (SAP) for NR sidelink
 * UE RRC, i.e., the interface between the LteUeRrc and the NrSlUeRrc. In
 * particular, this class implements the Provider part of the
 * SAP, i.e., the sidelink related methods exported by the
 * LteUeRrc and called by the NrSlUeRrc.
 *
 */
class NrSlUeRrcSapProvider
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeRrcSapProvider();
    /**
     * \brief Populate NR Sidelink pools
     *
     * After getting the pre-configuration
     * NrSlUeRrc instruct the LteUeRrc to
     * populate the pools.
     *
     */
    virtual void PopulatePools() = 0;
    /**
     * \brief Set Sidelink source layer 2 id
     *
     * \param srcL2Id The Sidelink layer 2 id of the source
     */
    virtual void SetSourceL2Id(uint32_t srcL2Id) = 0;
    /**
     * \brief Set NR Sidelink discovery and (re)selection requiremenmts for the relay UE
     *
     * \param relayConfig the relay UE config
     */
    virtual void SetRelayRequirements(const LteRrcSap::SlRelayUeConfig config) = 0;

    /**
     * \brief Set NR Sidelink discovery and (re)selection requiremenmts for the remote UE
     *
     * \param relayConfig the remote UE config
     */
    virtual void SetRemoteRequirements(const LteRrcSap::SlRemoteUeConfig config) = 0;
};

////////////////////////////////////
//   templates UE RRC side
////////////////////////////////////

/**
 * Template for the implementation of the NrSlUeRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeRrcSapUser : public NrSlUeRrcSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeRrcSapUser(C* owner);

    // inherited from NRSlUeRrcSapUser
    const LteRrcSap::SidelinkPreconfigNr GetNrSlPreconfiguration() override;
    const std::vector<std::bitset<1>> GetPhysicalSlPool(
        const std::vector<std::bitset<1>>& slBitMap) override;
    const std::set<uint8_t> GetBwpIdContainer() override;
    void AddNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb) override;
    void AddNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb) override;
    Ptr<NrSlDataRadioBearerInfo> GetSidelinkTxDataRadioBearer(uint32_t dstL2Id,
                                                              uint8_t lcid) override;
    std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>> GetAllSidelinkTxDataRadioBearers(
        uint32_t dstL2Id) override;

    Ptr<NrSlDataRadioBearerInfo> GetSidelinkRxDataRadioBearer(uint32_t srcL2Id,
                                                              uint8_t lcid) override;
    std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>> GetAllSidelinkRxDataRadioBearers(
        uint32_t srcL2Id) override;
    uint8_t GetNextLcid(uint32_t dstL2Id) override;
    void RemoveNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxTDrb) override;
    void RemoveNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb) override;
    void AddTxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb) override;
    void AddRxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb) override;
    Ptr<NrSlSignallingRadioBearerInfo> GetTxNrSlSignallingRadioBearer(uint32_t dstL2Id,
                                                                      uint8_t lcId) override;
    uint32_t GetSourceL2Id() override;
    void AddTxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRb) override;
    void AddRxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRb) override;
    Ptr<NrSlDiscoveryRadioBearerInfo> GetTxNrSlDiscoveryRadioBearer(uint32_t dstL2Id) override;

  private:
    MemberNrSlUeRrcSapUser();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeRrcSapUser<C>::MemberNrSlUeRrcSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlUeRrcSapUser<C>::MemberNrSlUeRrcSapUser()
{
}

template <class C>
const LteRrcSap::SidelinkPreconfigNr
MemberNrSlUeRrcSapUser<C>::GetNrSlPreconfiguration()
{
    return m_owner->DoGetNrSlPreconfiguration();
}

template <class C>
const std::vector<std::bitset<1>>
MemberNrSlUeRrcSapUser<C>::GetPhysicalSlPool(const std::vector<std::bitset<1>>& slBitMap)
{
    return m_owner->DoGetPhysicalSlPool(slBitMap);
}

template <class C>
const std::set<uint8_t>
MemberNrSlUeRrcSapUser<C>::GetBwpIdContainer()
{
    return m_owner->DoGetBwpIdContainer();
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slDrb)
{
    m_owner->DoAddNrSlTxDataRadioBearer(slDrb);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb)
{
    m_owner->DoAddNrSlRxDataRadioBearer(slRxDrb);
}

template <class C>
Ptr<NrSlDataRadioBearerInfo>
MemberNrSlUeRrcSapUser<C>::GetSidelinkTxDataRadioBearer(uint32_t dstL2Id, uint8_t lcId)
{
    return m_owner->DoGetSidelinkTxDataRadioBearer(dstL2Id, lcId);
}

template <class C>
std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
MemberNrSlUeRrcSapUser<C>::GetAllSidelinkTxDataRadioBearers(uint32_t dstL2Id)
{
    return m_owner->DoGetAllSidelinkTxDataRadioBearers(dstL2Id);
}

template <class C>
Ptr<NrSlDataRadioBearerInfo>
MemberNrSlUeRrcSapUser<C>::GetSidelinkRxDataRadioBearer(uint32_t srcL2Id, uint8_t lcId)
{
    return m_owner->DoGetSidelinkRxDataRadioBearer(srcL2Id, lcId);
}

template <class C>
std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
MemberNrSlUeRrcSapUser<C>::GetAllSidelinkRxDataRadioBearers(uint32_t srcL2Id)
{
    return m_owner->DoGetAllSidelinkRxDataRadioBearers(srcL2Id);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::RemoveNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb)
{
    m_owner->DoRemoveNrSlTxDataRadioBearer(slTxDrb);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::RemoveNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb)
{
    m_owner->DoRemoveNrSlRxDataRadioBearer(slRxDrb);
}

template <class C>
uint8_t
MemberNrSlUeRrcSapUser<C>::GetNextLcid(uint32_t dstL2Id)
{
    return m_owner->DoGetNextLcid(dstL2Id);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddTxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb)
{
    m_owner->DoAddTxNrSlSignallingRadioBearer(slSrb);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddRxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb)
{
    m_owner->DoAddRxNrSlSignallingRadioBearer(slSrb);
}

template <class C>
Ptr<NrSlSignallingRadioBearerInfo>
MemberNrSlUeRrcSapUser<C>::GetTxNrSlSignallingRadioBearer(uint32_t dstL2Id, uint8_t lcId)
{
    return m_owner->DoGetTxNrSlSignallingRadioBearer(dstL2Id, lcId);
}

template <class C>
uint32_t
MemberNrSlUeRrcSapUser<C>::GetSourceL2Id()
{
    return m_owner->DoGetSourceL2Id();
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddTxNrSlDiscoveryRadioBearer(
    Ptr<NrSlDiscoveryRadioBearerInfo> slTxDiscRb)
{
    m_owner->DoAddTxNrSlDiscoveryRadioBearer(slTxDiscRb);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddRxNrSlDiscoveryRadioBearer(
    Ptr<NrSlDiscoveryRadioBearerInfo> slRxDiscRb)
{
    m_owner->DoAddRxNrSlDiscoveryRadioBearer(slRxDiscRb);
}

template <class C>
Ptr<NrSlDiscoveryRadioBearerInfo>
MemberNrSlUeRrcSapUser<C>::GetTxNrSlDiscoveryRadioBearer(uint32_t dstL2Id)
{
    return m_owner->DoGetTxNrSlDiscoveryRadioBearer(dstL2Id);
}

/**
 * Template for the implementation of the NrSlUeRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeRrcSapProvider : public NrSlUeRrcSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeRrcSapProvider(C* owner);

    // inherited from NRSlUeRrcSapProvider
    void PopulatePools() override;
    void SetSourceL2Id(uint32_t srcL2Id) override;
    void SetRelayRequirements(const LteRrcSap::SlRelayUeConfig config) override;
    void SetRemoteRequirements(const LteRrcSap::SlRemoteUeConfig config) override;

  private:
    MemberNrSlUeRrcSapProvider();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeRrcSapProvider<C>::MemberNrSlUeRrcSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlUeRrcSapProvider<C>::MemberNrSlUeRrcSapProvider()
{
}

template <class C>
void
MemberNrSlUeRrcSapProvider<C>::PopulatePools()
{
    m_owner->DoPopulatePools();
}

template <class C>
void
MemberNrSlUeRrcSapProvider<C>::SetSourceL2Id(uint32_t srsL2Id)
{
    m_owner->DoSetSourceL2Id(srsL2Id);
}

template <class C>
void
MemberNrSlUeRrcSapProvider<C>::SetRelayRequirements(const LteRrcSap::SlRelayUeConfig config)
{
    m_owner->DoSetRelayRequirements(config);
}

template <class C>
void
MemberNrSlUeRrcSapProvider<C>::SetRemoteRequirements(const LteRrcSap::SlRemoteUeConfig config)
{
    m_owner->DoSetRemoteRequirements(config);
}

// eNB side SAPs

/**
 * \brief User part of the Service Access Point (SAP) between eNB RRC and NR
 *        sidelink eNB RRC.
 */
class NrSlEnbRrcSapUser
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlEnbRrcSapUser();
};

/**
 * \brief Provider part of the Service Access Point (SAP) between eNB RRC and NR
 *        sidelink eNB RRC.
 */
class NrSlEnbRrcSapProvider
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlEnbRrcSapProvider();
};

////////////////////////////////////
//   templates eNB RRC side
////////////////////////////////////

/**
 * Template for the implementation of the NrSlEnbRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlEnbRrcSapUser : public NrSlEnbRrcSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlEnbRrcSapUser(C* owner);

    // inherited from NRSlUeRrcSapUser
    //  virtual void Setup (SetupParameters params);

  private:
    MemberNrSlEnbRrcSapUser();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlEnbRrcSapUser<C>::MemberNrSlEnbRrcSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlEnbRrcSapUser<C>::MemberNrSlEnbRrcSapUser()
{
}

/*
template <class C>
void
MemberNrSlEnbRrcSapUser<C>::Setup (SetupParameters params)
{
  m_owner->DoSetup (params);
}
*/

/**
 * Template for the implementation of the NrSlEnbRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlEnbRrcSapProvider : public NrSlEnbRrcSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlEnbRrcSapProvider(C* owner);

    // inherited from NRSlUeRrcSapUser
    //  virtual void Setup (SetupParameters params);

  private:
    MemberNrSlEnbRrcSapProvider();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlEnbRrcSapProvider<C>::MemberNrSlEnbRrcSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlEnbRrcSapProvider<C>::MemberNrSlEnbRrcSapProvider()
{
}

/*
template <class C>
void
MemberNrSlEnbRrcSapProvider<C>::Setup (SetupParameters params)
{
  m_owner->DoSetup (params);
}
*/

} // namespace ns3

#endif /* NR_SL_RRC_SAP_H */
