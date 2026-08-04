// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.cat>

#ifndef NR_EPC_S1AP_SAP_H
#define NR_EPC_S1AP_SAP_H

#include "nr-eps-bearer.h"

#include <ns3/address.h>
#include <ns3/ipv4-address.h>
#include <ns3/object.h>
#include <ns3/ptr.h>

#include <list>

namespace ns3
{

/**
 * \ingroup nr
 *
 * Base class that defines EPC S1-AP Service Access Point (SAP) interface.
 */
class NrEpcS1apSap
{
  public:
    virtual ~NrEpcS1apSap() = default;
};

/**
 * \ingroup nr
 *
 * MME side of the S1-AP Service Access Point (SAP), provides the MME
 * methods to be called when an S1-AP message is received by the MME.
 */
class NrEpcS1apSapMme : public NrEpcS1apSap
{
  public:
    /**
     * Initial UE message.
     *
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param stmsi in practice, the imsi
     * \param ecgi in practice, the cell Id
     */
    virtual void InitialUeMessage(uint64_t mmeUeS1Id,
                                  uint16_t gnbUeS1Id,
                                  uint64_t stmsi,
                                  uint16_t ecgi) = 0;

    /**
     *  E-RAB Release Indication Item IEs, 3GPP TS 36.413 version 9.8.0 section 9.1.3.7
     */
    struct ErabToBeReleasedIndication
    {
        uint8_t erabId; ///< E-RAB ID
    };

    /**
     * \brief As per 3GPP TS 36.413 version 9.8.0 section 8.2.3.2.2, the gNB
     * indicates bearer release by sending an E-RAB RELEASE INDICATION message
     * towards MME
     *
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param erabToBeReleaseIndication List of bearers to be deactivated
     */
    virtual void ErabReleaseIndication(
        uint64_t mmeUeS1Id,
        uint16_t gnbUeS1Id,
        std::list<ErabToBeReleasedIndication> erabToBeReleaseIndication) = 0;

    /**
     *  E-RAB Setup Item IEs, see 3GPP TS 36.413 9.1.4.2
     */
    struct ErabSetupItem
    {
        uint16_t erabId;                      ///< E-RAB ID
        Ipv4Address gnbTransportLayerAddress; ///< transport layer address
        uint32_t gnbTeid;                     ///< TEID
    };

    /**
     * INITIAL CONTEXT SETUP RESPONSE message,  see 3GPP TS 36.413 9.1.4.2
     *
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param erabSetupList List of ERAB setup
     *
     */
    virtual void InitialContextSetupResponse(uint64_t mmeUeS1Id,
                                             uint16_t gnbUeS1Id,
                                             std::list<ErabSetupItem> erabSetupList) = 0;

    /**
     * E-RABs Switched in Downlink Item IE, see 3GPP TS 36.413 9.1.5.8
     */
    struct ErabSwitchedInDownlinkItem
    {
        uint16_t erabId;                      ///< ERAB ID
        Ipv4Address gnbTransportLayerAddress; ///< address
        uint32_t gnbTeid;                     ///< TEID
    };

    /**
     * PATH SWITCH REQUEST message, see 3GPP TS 36.413 9.1.5.8
     *
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param gci GCI
     * \param erabToBeSwitchedInDownlinkList List of ERAB to be switched in downlink
     */
    virtual void PathSwitchRequest(
        uint64_t gnbUeS1Id,
        uint64_t mmeUeS1Id,
        uint16_t gci,
        std::list<ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList) = 0;
};

/**
 * \ingroup nr
 *
 * gNB side of the S1-AP Service Access Point (SAP), provides the gNB
 * methods to be called when an S1-AP message is received by the gNB.
 */
class NrEpcS1apSapGnb : public NrEpcS1apSap
{
  public:
    /// ErabToBeSetupItem structure
    struct ErabToBeSetupItem
    {
        uint8_t erabId;                     ///< ERAB iD
        NrEpsBearer erabLevelQosParameters; ///< Level QOS parameters
        Ipv4Address transportLayerAddress;  ///< transport layer address
        uint32_t sgwTeid;                   ///< TEID
    };

    /**
     * Initial context setup request
     *
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param erabToBeSetupList List of ERAB to be setup
     */
    virtual void InitialContextSetupRequest(uint64_t mmeUeS1Id,
                                            uint16_t gnbUeS1Id,
                                            std::list<ErabToBeSetupItem> erabToBeSetupList) = 0;

    /**
     * E-RABs Switched in Uplink Item IE, see 3GPP TS 36.413 9.1.5.9
     */
    struct ErabSwitchedInUplinkItem
    {
        uint8_t erabId;                    ///< E_RAB ID
        Ipv4Address transportLayerAddress; ///< transport layer address
        uint32_t gnbTeid;                  ///< TEID
    };

    /**
     * PATH SWITCH REQUEST ACKNOWLEDGE message, see 3GPP TS 36.413 9.1.5.9
     *
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param cgi CGI
     * \param erabToBeSwitchedInUplinkList List of ERAB to be switched in uplink
     */
    virtual void PathSwitchRequestAcknowledge(
        uint64_t gnbUeS1Id,
        uint64_t mmeUeS1Id,
        uint16_t cgi,
        std::list<ErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList) = 0;
};

/**
 * Template for the implementation of the NrEpcS1apSapMme as a member
 * of an owner class of type C to which all methods are forwarded
 */
template <class C>
class NrMemberEpcS1apSapMme : public NrEpcS1apSapMme
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    NrMemberEpcS1apSapMme(C* owner);

    // Delete default constructor to avoid misuse
    NrMemberEpcS1apSapMme() = delete;

    // inherited from NrEpcS1apSapMme
    /**
     * Initial UE Message function
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param imsi the IMSI
     * \param ecgi ECGI
     */
    void InitialUeMessage(uint64_t mmeUeS1Id,
                          uint16_t gnbUeS1Id,
                          uint64_t imsi,
                          uint16_t ecgi) override;
    /**
     * ERAB Release Indiation function
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param erabToBeReleaseIndication List of ERAB to be release indication
     */
    void ErabReleaseIndication(
        uint64_t mmeUeS1Id,
        uint16_t gnbUeS1Id,
        std::list<ErabToBeReleasedIndication> erabToBeReleaseIndication) override;

    /**
     * Initial context setup response
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param erabSetupList List of ERAB setup
     */
    void InitialContextSetupResponse(uint64_t mmeUeS1Id,
                                     uint16_t gnbUeS1Id,
                                     std::list<ErabSetupItem> erabSetupList) override;
    /**
     * Path switch request
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param cgi CGI
     * \param erabToBeSwitchedInDownlinkList List of ERAB to be switched in downlink
     */
    void PathSwitchRequest(
        uint64_t gnbUeS1Id,
        uint64_t mmeUeS1Id,
        uint16_t cgi,
        std::list<ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList) override;

  private:
    C* m_owner; ///< owner class
};

template <class C>
NrMemberEpcS1apSapMme<C>::NrMemberEpcS1apSapMme(C* owner)
    : m_owner(owner)
{
}

template <class C>
void
NrMemberEpcS1apSapMme<C>::InitialUeMessage(uint64_t mmeUeS1Id,
                                           uint16_t gnbUeS1Id,
                                           uint64_t imsi,
                                           uint16_t ecgi)
{
    m_owner->DoInitialUeMessage(mmeUeS1Id, gnbUeS1Id, imsi, ecgi);
}

template <class C>
void
NrMemberEpcS1apSapMme<C>::ErabReleaseIndication(
    uint64_t mmeUeS1Id,
    uint16_t gnbUeS1Id,
    std::list<ErabToBeReleasedIndication> erabToBeReleaseIndication)
{
    m_owner->DoErabReleaseIndication(mmeUeS1Id, gnbUeS1Id, erabToBeReleaseIndication);
}

template <class C>
void
NrMemberEpcS1apSapMme<C>::InitialContextSetupResponse(uint64_t mmeUeS1Id,
                                                      uint16_t gnbUeS1Id,
                                                      std::list<ErabSetupItem> erabSetupList)
{
    m_owner->DoInitialContextSetupResponse(mmeUeS1Id, gnbUeS1Id, erabSetupList);
}

template <class C>
void
NrMemberEpcS1apSapMme<C>::PathSwitchRequest(
    uint64_t gnbUeS1Id,
    uint64_t mmeUeS1Id,
    uint16_t cgi,
    std::list<ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList)
{
    m_owner->DoPathSwitchRequest(gnbUeS1Id, mmeUeS1Id, cgi, erabToBeSwitchedInDownlinkList);
}

/**
 * Template for the implementation of the NrEpcS1apSapGnb as a member
 * of an owner class of type C to which all methods are forwarded
 */
template <class C>
class NrMemberEpcS1apSapGnb : public NrEpcS1apSapGnb
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    NrMemberEpcS1apSapGnb(C* owner);

    // Delete default constructor to avoid misuse
    NrMemberEpcS1apSapGnb() = delete;

    // inherited from NrEpcS1apSapGnb
    /**
     * Initial context setup request function
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param erabToBeSetupList List of ERAB to be setup
     */
    void InitialContextSetupRequest(uint64_t mmeUeS1Id,
                                    uint16_t gnbUeS1Id,
                                    std::list<ErabToBeSetupItem> erabToBeSetupList) override;
    /**
     * Path switch request acknowledge function
     * \param gnbUeS1Id in practice, we use the RNTI
     * \param mmeUeS1Id in practice, we use the IMSI
     * \param cgi CGI
     * \param erabToBeSwitchedInUplinkList List of ERAB to be switched in uplink
     */
    void PathSwitchRequestAcknowledge(
        uint64_t gnbUeS1Id,
        uint64_t mmeUeS1Id,
        uint16_t cgi,
        std::list<ErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList) override;

  private:
    C* m_owner; ///< owner class
};

template <class C>
NrMemberEpcS1apSapGnb<C>::NrMemberEpcS1apSapGnb(C* owner)
    : m_owner(owner)
{
}

template <class C>
void
NrMemberEpcS1apSapGnb<C>::InitialContextSetupRequest(uint64_t mmeUeS1Id,
                                                     uint16_t gnbUeS1Id,
                                                     std::list<ErabToBeSetupItem> erabToBeSetupList)
{
    m_owner->DoInitialContextSetupRequest(mmeUeS1Id, gnbUeS1Id, erabToBeSetupList);
}

template <class C>
void
NrMemberEpcS1apSapGnb<C>::PathSwitchRequestAcknowledge(
    uint64_t gnbUeS1Id,
    uint64_t mmeUeS1Id,
    uint16_t cgi,
    std::list<ErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList)
{
    m_owner->DoPathSwitchRequestAcknowledge(gnbUeS1Id,
                                            mmeUeS1Id,
                                            cgi,
                                            erabToBeSwitchedInUplinkList);
}

} // namespace ns3

#endif /* NR_EPC_S1AP_SAP_H */
