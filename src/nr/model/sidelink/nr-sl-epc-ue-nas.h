// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_EPC_UE_NAS_H
#define NR_SL_EPC_UE_NAS_H

#include "nr-sl-as-sap.h"
#include "nr-sl-tft.h"
#include "nr-sl-ue-prose-dir-lnk-sap.h"
#include "nr-sl-ue-svc-nas-sap.h"

#include "ns3/nr-epc-ue-nas.h"
#include <ns3/ipv4-address.h>

namespace ns3
{

class NrSlEpcUeNas : public NrEpcUeNas
{
    /// allow MemberNrSlAsSapUser<NrSlEpcUeNas> class friend access
    friend class MemberNrSlAsSapUser<NrSlEpcUeNas>;
    /// allow MemberNrSlUeSvcNasSapProvider<NrSlEpcUeRrc> class friend access
    friend class MemberNrSlUeSvcNasSapProvider<NrSlEpcUeNas>;

  public:
    /**
     * Constructor
     */
    NrSlEpcUeNas();

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    // Inherited from NrEpcUeNas
    bool Send(Ptr<Packet> p, uint16_t protocolNumber) override;

    /**
     * Set the NR SL AS SAP provider to interact with the NAS entity
     *
     * \param s the NR SL AS SAP provider
     */
    void SetNrSlAsSapProvider(NrSlAsSapProvider* s);

    /**
     *
     *
     * \return the NR SL AS SAP user exported by this RRC
     */
    NrSlAsSapUser* GetNrSlAsSapUser();

    /**
     * \brief Get the pointer of the UE service layer SAP Provider interface
     *        offered to the service layer by this class
     *
     * \return the pointer of type NrSlUeSvcNasSapProvider
     */
    NrSlUeSvcNasSapProvider* GetNrSlUeSvcNasSapProvider();

    /**
     * \brief Set the pointer for the UE service layer SAP User interface
     *        offered to this class by service layer class
     *
     * \param s the pointer of type NrSlUeSvcNasSapUser
     */
    void SetNrSlUeSvcNasSapUser(NrSlUeSvcNasSapUser* s);

    /**
     * \brief Activate NR Sidelink bearer
     *
     * \param tft The bearer information
     */
    void ActivateNrSlBearer(Ptr<NrSlTft> tft);

    /**
     * \brief Delete NR Sidelink bearer
     *
     * \param tft The bearer information
     */
    void DeleteNrSlBearer(Ptr<NrSlTft> tft);

    /**
     * Add a PacketFilter entry on a bearer (indicated by L2ID) for the specified IPv4 destination
     *
     * \param dstL2Id Destination L2ID of the bearer
     * \param dest Ipv4Address of the destination for the packet filter
     */
    bool AddDestinationToPacketFilters(uint32_t dstL2Id, const Ipv4Address& dest);
    /**
     * Remove a PacketFilter entry on a bearer (indicated by L2ID) for the specified IPv4
     * destination
     *
     * \param dstL2Id Destination L2ID of the bearer
     * \param dest Ipv4Address of the destination for the packet filter
     */
    bool RemoveDestinationFromPacketFilters(uint32_t dstL2Id, const Ipv4Address& dest);

    /**
     * TracedCallback signature for reception of data packets by a UE acting as UE-to-Network relay
     *
     * \param [in] nodeIp the Ipv4 address of this UE
     * \param [in] srcIp the source Ipv4 address of the data packet
     * \param [in] dstIp the destination Ipv4 address of the data packet
     * \param [in] srcLink the link in which the data packet was received
     * \param [in] dstLink the link to which the data packet was relayed
     * \param [in] p the data packet
     */
    typedef void (*NrSlRelayNasRxPacketTracedCallback)(Ipv4Address nodeIp,
                                                       Ipv4Address srcIp,
                                                       Ipv4Address dstIp,
                                                       std::string srcLink,
                                                       std::string dstLink,
                                                       Ptr<Packet> p);

  private:
    // inherited from Object
    void DoDispose() override;

    // Inherited from NrEpcUeNas
    void DoRecvData(Ptr<Packet> packet) override;

    /**
     * \brief Notify Nr Sidelink radio bearer activated function
     *
     * \param slInfo The sidelink information
     */
    void DoNotifyNrSlRadioBearerActivated(const struct SidelinkInfo& slInfo);

    /**
     * \brief Notify Nr Sidelink radio bearer removed function
     *
     * \param slInfo The sidelink information
     */
    void DoNotifyNrSlRadioBearerRemoved(const struct SidelinkInfo& slInfo);

    std::list<Ptr<NrSlTft>> m_pendingSlBearersList; ///< pending NR Sidelink bearers list

    std::list<Ptr<NrSlTft>> m_slBearersActivatedList; ///< Sidelink NR bearers activated list

    std::list<Ptr<NrSlTft>> m_slBearersInactiveList; ///< Sidelink NR bearers inactive list

    // Service layer <-> NAS interfaces
    NrSlAsSapProvider* m_nrSlAsSapProvider;
    NrSlAsSapUser* m_nrSlAsSapUser;
    NrSlUeSvcNasSapProvider* m_nrSlUeSvcNasSapProvider; //!< SAP interface to receive calls from the
                                                        //!< service layer instance
    NrSlUeSvcNasSapUser* m_nrSlUeSvcNasSapUser{
        nullptr}; //!< SAP interface to call methods of the service layer instance

    // NrSlUeSvcNasSapProvider functions
    /**
     * \brief Implementation of the method called by the service layer (e.g.,
     *        ProSe layer) to instruct the NAS to activate a sidelink data
     *        radio bearer (SL-DRB)
     *
     * \param tft the SL TFT identifying the traffic that will go on this bearer
     */
    void DoActivateSvcNrSlDataRadioBearer(Ptr<NrSlTft> tft);

    /**
     * \brief Implementation of the method called by the service layer (e.g.,
     *        ProSe layer) to instruct the NAS to deactivate a sidelink data
     *        radio bearer (SL-DRB)
     *
     * \param tft the SL TFT identifying the traffic that will go on this bearer
     */
    void DoDeactivateSvcNrSlDataRadioBearer(Ptr<NrSlTft> tft);

    /**
     * \ brief Instruct the NAS to delete the NR sidelink data bearer
     *
     * \param dstL2Id destination layer 2 ID
     */
    void DoDeleteSvcNrSlDataRadioBearer(Ptr<NrSlTft> tft);

    /**
     * \brief Method that verifies if the two TFT have at least one TFT filter
     *        that matches remote address and port
     *
     * \param tft1 First TFT to compare
     * \param tft2 Second TFT to compare
     *
     * \return true if at least one TFT filter matches, false otherwise.
     */
    bool CheckTftFilterMatch(Ptr<NrSlTft> tft1, Ptr<NrSlTft> tft2);

    /**
     * Implementation of the method called by the service layer (e.g., ProSe
     * layer) to instruct the NAS to (re)configure the data bearers (UL and SL
     * where it applies) to have the data packets flowing in the appropriate path
     * after the UEs establish a connection for UE-to-Network (U2N) relay.
     *
     * \param peerL2Id the layer 2 ID of the peer UE
     * \param role the role of this UE in the U2N link (remote UE or relay UE)
     * \param ipInfo the IP configuration associated to the link
     * \param relayDrbId the UL data radio bearer ID used to relay data (used only when the UE has a
     * relay UE role)
     * \param slInfo the parameters to be used for the sidelink data radio bearer
     */
    void DoConfigureNrSlDataRadioBearersForU2nRelay(
        uint32_t peerL2Id,
        enum NrSlUeProseDirLnkSapUser::U2nRole role,
        NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo,
        uint8_t relayDrbId,
        const struct SidelinkInfo& slInfo);

    /**
     * Implementation of the method called by the service layer (e.g., ProSe
     * layer) to instruct the NAS to remove the data bearers (UL and SL
     * where it applies) to have the data packets flowing in the appropriate path
     * after the UEs release a connection for UE-to-Network (U2N) relay.
     *
     * \param peerL2Id the layer 2 ID of the peer UE
     * \param role the role of this UE in the U2N link (remote UE or relay UE)
     * \param ipInfo the IP configuration associated to the link
     * \param relayDrbId the UL data radio bearer ID used to relay data (used only when the UE has a
     * relay UE role)
     */
    void DoRemoveNrSlDataRadioBearersForU2nRelay(uint32_t peerL2Id,
                                                 enum NrSlUeProseDirLnkSapUser::U2nRole role,
                                                 NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo,
                                                 uint8_t relayDrbId);
    /**
     * \brief Function that moves the received packet through the correct path
     *        when the UE is a UE-to-Network (U2N) relay UE
     *
     * \param packet the received packet
     */
    void ClassifyRecvPacketForU2nRelay(Ptr<Packet> packet);

    /**
     * Parameters related to the UE-to-Network relay configuration used in this
     * layer
     */
    struct U2nRelayNasConfig
    {
        Ipv4Address selfIpv4Addr; ///< Ipv4 address of this UE
        bool relaying = false; ///< True if the UE is acting as UE-to-Network Relay, false otherwise
        uint8_t relayDrbId = 0; ///< The data radio bearer ID used for relay
    };

    U2nRelayNasConfig m_u2nRelayConfig; ///< UE-to-Network Relay parameters

    /**
     * Trace for reception of data packets by a UE acting as UE-to-Network relay
     */
    TracedCallback<Ipv4Address, Ipv4Address, Ipv4Address, std::string, std::string, Ptr<Packet>>
        m_relayRxPacketTrace;
};

} // namespace ns3

#endif // NR_SL_EPC_UE_NAS_H
