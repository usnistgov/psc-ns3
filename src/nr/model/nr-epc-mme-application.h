// Copyright (c) 2017-2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#ifndef NR_EPC_MME_APPLICATION_H
#define NR_EPC_MME_APPLICATION_H

#include "nr-epc-gtpc-header.h"
#include "nr-epc-s1ap-sap.h"

#include "ns3/application.h"
#include "ns3/socket.h"

#include <map>

namespace ns3
{

/**
 * \ingroup nr
 *
 * This application implements the Mobility Management Entity (MME) according to
 * the 3GPP TS 23.401 document.
 *
 * This Application implements the MME side of the S1-MME interface between
 * the MME node and the gNB nodes and the MME side of the S11 interface between
 * the MME node and the SGW node. It supports the following functions and messages:
 *
 *  - Bearer management functions including dedicated bearer establishment
 *  - NAS signalling
 *  - Tunnel Management messages
 *
 * Others functions enumerated in section 4.4.2 of 3GPP TS 23.401 are not supported.
 */
class NrEpcMmeApplication : public Application
{
    /// allow NrMemberEpcS1apSapMme<EpcMme> class friend access
    friend class NrMemberEpcS1apSapMme<NrEpcMmeApplication>;

  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    void DoDispose() override;

    /** Constructor */
    NrEpcMmeApplication();

    /** Destructor */
    ~NrEpcMmeApplication() override;

    /**
     *
     * \return the MME side of the S1-AP SAP
     */
    NrEpcS1apSapMme* GetS1apSapMme();

    /**
     * Add a new SGW to the MME
     *
     * \param sgwS11Addr IPv4 address of the SGW S11 interface
     * \param mmeS11Addr IPv4 address of the MME S11 interface
     * \param mmeS11Socket socket of the MME S11 interface
     */
    void AddSgw(Ipv4Address sgwS11Addr, Ipv4Address mmeS11Addr, Ptr<Socket> mmeS11Socket);

    /**
     * Add a new gNB to the MME
     *
     * \param ecgi E-UTRAN Cell Global ID, the unique identifier of the eNodeB
     * \param gnbS1UAddr IPv4 address of the gNB for S1-U communications
     * \param gnbS1apSap the gNB side of the S1-AP SAP
     */
    void AddGnb(uint16_t ecgi, Ipv4Address gnbS1UAddr, NrEpcS1apSapGnb* gnbS1apSap);

    /**
     * Add a new UE to the MME. This is the equivalent of storing the UE
     * credentials before the UE is ever turned on.
     *
     * \param imsi the unique identifier of the UE
     */
    void AddUe(uint64_t imsi);

    /**
     * Add an EPS bearer to the list of bearers to be activated for this UE.
     * The bearer will be activated when the UE enters the ECM
     * connected state.
     *
     * \param imsi UE identifier
     * \param tft traffic flow template of the bearer
     * \param bearer QoS characteristics of the bearer
     * \returns bearer ID
     */
    uint8_t AddBearer(uint64_t imsi, Ptr<NrEpcTft> tft, NrEpsBearer bearer);

  private:
    // S1-AP SAP MME forwarded methods

    /**
     * Process the S1 Initial UE Message received from an gNB
     * \param mmeUeS1Id the MME UE S1 ID
     * \param gnbUeS1Id the gNB UE S1 ID
     * \param imsi the IMSI
     * \param ecgi the ECGI
     */
    void DoInitialUeMessage(uint64_t mmeUeS1Id, uint16_t gnbUeS1Id, uint64_t imsi, uint16_t ecgi);

    /**
     * Process the S1 Initial Context Setup Response received from an gNB
     * \param mmeUeS1Id the MME UE S1 ID
     * \param gnbUeS1Id the gNB UE S1 ID
     * \param erabSetupList the ERAB setup list
     */
    void DoInitialContextSetupResponse(uint64_t mmeUeS1Id,
                                       uint16_t gnbUeS1Id,
                                       std::list<NrEpcS1apSapMme::ErabSetupItem> erabSetupList);

    /**
     * Process the S1 Path Switch Request received from an gNB
     * \param mmeUeS1Id the MME UE S1 ID
     * \param gnbUeS1Id the gNB UE S1 ID
     * \param cgi the CGI
     * \param erabToBeSwitchedInDownlinkList the ERAB to be switched in downlink list
     */
    void DoPathSwitchRequest(
        uint64_t gnbUeS1Id,
        uint64_t mmeUeS1Id,
        uint16_t cgi,
        std::list<NrEpcS1apSapMme::ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList);

    /**
     * Process ERAB Release Indication received from an gNB
     * \param mmeUeS1Id the MME UE S1 ID
     * \param gnbUeS1Id the gNB UE S1 ID
     * \param erabToBeReleaseIndication the ERAB to be release indication list
     */
    void DoErabReleaseIndication(
        uint64_t mmeUeS1Id,
        uint16_t gnbUeS1Id,
        std::list<NrEpcS1apSapMme::ErabToBeReleasedIndication> erabToBeReleaseIndication);

    // Methods to read/process GTP-C messages of the S11 interface

    /**
     * Reads the S11 messages from a socket
     * \param socket the socket
     */
    void RecvFromS11Socket(Ptr<Socket> socket);

    /**
     * Process GTP-C Create Session Response message
     * \param header the GTP-C header
     * \param packet the packet containing the message
     */
    void DoRecvCreateSessionResponse(NrGtpcHeader& header, Ptr<Packet> packet);

    /**
     * Process GTP-C Modify Bearer Response message
     * \param header the GTP-C header
     * \param packet the packet containing the message
     */
    void DoRecvModifyBearerResponse(NrGtpcHeader& header, Ptr<Packet> packet);

    /**
     * Process GTP-C Delete Bearer Request message
     * \param header the GTP-C header
     * \param packet the packet containing the message
     */
    void DoRecvDeleteBearerRequest(NrGtpcHeader& header, Ptr<Packet> packet);

    /**
     * Hold info on an EPS bearer to be activated
     */
    struct BearerInfo
    {
        Ptr<NrEpcTft> tft;  ///< traffic flow template
        NrEpsBearer bearer; ///< bearer QOS characteristics
        uint8_t bearerId;   ///< bearer ID
    };

    /**
     * Hold info on a UE
     */
    struct NrUeInfo : public SimpleRefCount<NrUeInfo>
    {
        uint64_t imsi;                              ///< UE identifier
        uint64_t mmeUeS1Id;                         ///< mmeUeS1Id
        uint16_t gnbUeS1Id;                         ///< gnbUeS1Id
        uint16_t cellId;                            ///< cell ID
        uint16_t bearerCounter;                     ///< bearer counter
        std::list<BearerInfo> bearersToBeActivated; ///< list of bearers to be activated
    };

    /**
     * NrUeInfo stored by IMSI
     */
    std::map<uint64_t, Ptr<NrUeInfo>> m_ueInfoMap;

    /**
     * \brief This Function erases all contexts of bearer from MME side
     * \param ueInfo UE information pointer
     * \param epsBearerId Bearer Id which need to be removed corresponding to UE
     */
    void RemoveBearer(Ptr<NrUeInfo> ueInfo, uint8_t epsBearerId);

    /**
     * Hold info on an gNB
     */
    struct GnbInfo : public SimpleRefCount<GnbInfo>
    {
        uint16_t gci;                ///< GCI
        Ipv4Address s1uAddr;         ///< IP address of the S1-U interface
        NrEpcS1apSapGnb* s1apSapGnb; ///< NrEpcS1apSapGnb
    };

    /**
     * GnbInfo stored by EGCI
     */
    std::map<uint16_t, Ptr<GnbInfo>> m_gnbInfoMap;

    NrEpcS1apSapMme* m_s1apSapMme; ///< NrEpcS1apSapMme

    Ptr<Socket> m_s11Socket;  ///< Socket to send/receive messages in the S11 interface
    Ipv4Address m_mmeS11Addr; ///< IPv4 address of the MME S11 interface
    Ipv4Address m_sgwS11Addr; ///< IPv4 address of the SGW S11 interface
    uint16_t m_gtpcUdpPort;   ///< UDP port for GTP-C protocol. Fixed by the standard to port 2123
};

} // namespace ns3

#endif // NR_EPC_MME_APPLICATION_H
