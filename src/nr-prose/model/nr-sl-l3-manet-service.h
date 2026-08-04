//
// SPDX-License-Identifier: NIST-Software
//

#ifndef NR_SL_L3_MANET_SERVICE_H
#define NR_SL_L3_MANET_SERVICE_H

#include "nr-sl-discovery-header.h"
#include "nr-sl-ue-prose-direct-link.h"
#include "nr-sl-ue-service.h"

#include "ns3/net-device.h"
#include "ns3/nhdp-client.h"
#include "ns3/nr-sl-ue-prose-dir-lnk-sap.h"
#include "ns3/nr-sl-ue-svc-nas-sap.h"
#include "ns3/nr-sl-ue-svc-rrc-sap.h"
#include "ns3/traced-callback.h"

#include <bitset>
#include <map>
#include <optional>
#include <set>
#include <unordered_map>

namespace ns3
{

class NrPointToPointEpcHelper;
class NrSlUeProseDirLinkContext;

/**
 * @ingroup nr-prose
 *
 * This class implements an NR SL service layer providing Proximity Services (ProSe)
 */
class NrSlL3ManetService : public NrSlUeService
{
    /// allow MemberNrSlUeSvcRrcSapUser<NrSlL3ManetService> class friend access
    friend class MemberNrSlUeSvcRrcSapUser<NrSlL3ManetService>;
    /// allow MemberNrSlUeSvcNasSapUser<NrSlL3ManetService> class friend access
    friend class MemberNrSlUeSvcNasSapUser<NrSlL3ManetService>;
    /// allow MemberNrSlUeProseDirLnkSapUser<NrSlL3ManetService> class friend access
    friend class MemberNrSlUeProseDirLnkSapUser<NrSlL3ManetService>;

  public:
    NrSlL3ManetService();
    virtual ~NrSlL3ManetService(void);

    ///< The types of discovery role
    enum DiscoveryRole
    {
        Monitoring = 0, ///< Model A: The UE receiving discovery messages
        Announcing,     ///< Model A: The UE sending discovery messages
        Discoveree,     ///< Model B: The UE responding to requests
        Discoverer,     ///< Model B: The UE sending requests
        RemoteUE,
        RelayUE
    };

    ///< The discovery models supported
    enum DiscoveryModel
    {
        ModelA = 0, ///< announce
        ModelB      ///< request/response
    };

    ///< Information for application discovery
    struct DiscoveryInfo
    {
        DiscoveryModel model; ///< discovery model used
        DiscoveryRole role;   ///< role in the discovery
        uint32_t appCode;     ///< application code
        uint32_t dstL2Id;     ///< destination L2 ID
    };

    ///< Comparator for use in ordered containers
    struct DiscoveryInfoCmp
    {
        bool operator()(const DiscoveryInfo& a, const DiscoveryInfo& b) const
        {
            return std::tie(a.model, a.role, a.appCode, a.dstL2Id) <
                   std::tie(b.model, b.role, b.appCode, b.dstL2Id);
        }
    };

    ///< The RLF detection event
    enum RlfEvent
    {
        Harq = 0,  ///< HARQ failure resulted in RLF
        KeepAlive, ///< ProSe keep-alive procedure resulted in RLF
        SdRsrp,    ///< SD-RSRP measurement resulted in RLF
        SlRsrp     ///< SL-RSRP measurement resulted in RLF
    };

  protected:
    void DoDispose() override;
    void DoInitialize() override;

  public:
    /**
     *  @brief Register this type.
     *
     *  @return The object TypeId.
     */
    static TypeId GetTypeId(void);
    /**
     * @brief Get the pointer of the UE service layer SAP User interface
     *        offered to UE RRC by this class
     *
     * @return the pointer of type NrSlUeSvcRrcSapUser
     */
    NrSlUeSvcRrcSapUser* GetNrSlUeSvcRrcSapUser();
    /**
     * @brief Set the pointer for the UE service layer SAP Provider interface
     *        offered to this class by UE RRC class
     *
     * @param s the pointer of type NrSlUeSvcRrcSapProvider
     */
    void SetNrSlUeSvcRrcSapProvider(NrSlUeSvcRrcSapProvider* s);

    /**
     * @brief Get the pointer of the UE service layer SAP User interface
     *        offered to EPC UE NAS by this class
     *
     * @return the pointer of type NrSlUeSvcNasSapUser
     */
    NrSlUeSvcNasSapUser* GetNrSlUeSvcNasSapUser();
    /**
     * @brief Set the pointer for the UE service layer SAP Provider interface
     *        offered to this class by EPC UE NAS class
     *
     * @param s the pointer of type NrSlUeSvcNasSapProvider
     */
    void SetNrSlUeSvcNasSapProvider(NrSlUeSvcNasSapProvider* s);

    /**
     * @brief Get the pointer of the ProSe Direct Link SAP User interface
     *        offered to the Prose Direct Links by this class
     *
     * @return the pointer of type NrSlUeProseDirLnkSapUser
     */
    NrSlUeProseDirLnkSapUser* GetNrSlUeProseDirLnkSapUser();

    /**
     * @brief Set discovery interval
     *
     * @param val discovery interval
     */
    void SetDiscoveryInterval(Time val);

    /**
     * @brief Configure the parameters required by the UE to perform ProSe
     *        unicast communication
     */
    void ConfigureUnicast();

    /**
     * @brief Configure the monitoring of layer 2 IDs required by the UE to
     *        perform ProSe discovery
     *
     * @param dstL2Id a destination layer 2 ID to monitor
     */
    void ConfigureL2IdMonitoringForDiscovery(uint32_t dstL2Id);

    /**
     * @brief Add a new direct link connection with the given peer if possible
     * If a direct link between the pair of UEs already exists, it gets overwritten with the new
     * parameters (This can happen in the case of U2N relay reselection where an old link is still
     * stored in the m_unicastDirectLinks map but a new request to establish a new link between the
     * pair of UEs is triggered)
     *
     * @param selfL2Id the layer 2 ID of this UE
     * @param selfIp the IPv4 address used by this UE
     * @param peerL2Id the layer 2 UD of the peer UE
     * @param peerIp the IPv4 address used by the peer
     * @param isInitiating flag indicating if the UE is initiating the procedure (true)
     *                     or adding the link as result of a request by the peer UE (false)
     * @param relayServiceCode the relay service code associated to this direct link (if > 0, the
     * connection is for relay) \param slInfo the traffic profile parameters to be used for the
     * sidelink data radio bearer
     */
    void AddDirectLinkConnection(uint32_t selfL2Id,
                                 Ipv4Address selfIp,
                                 uint32_t peerL2Id,
                                 Ipv4Address peerIp,
                                 bool isInitiating,
                                 uint32_t relayServiceCode,
                                 const struct SidelinkInfo& slInfo);
    /**
     * Map to store direct link context instances indexed by direct link id
     */
    typedef std::map<uint32_t, Ptr<NrSlUeProseDirLinkContext>> NrSlDirectLinkContextMapPerPeerL2Id;

    /**
     * Map to keep track of the active SL-SRBs. A bit set of 4 bit representing each of the
     * 4 LcIds of the SL-SRBs is stored per peerL2Id.
     * Bit set to 1 means the SL-SRB of the corresponding LcId is active
     */
    typedef std::unordered_map<uint32_t, std::bitset<4>> NrSlSignalingRadioBearersPerPeerL2Id;

    /**
     * @brief Set the IMSI used by the UE
     *
     * @param imsi the IMSI of the UE
     */
    void SetImsi(uint64_t imsi);

    /**
     * @brief Set the Layer 2 ID used by the UE
     *
     * @param l2Id the Layer 2 ID  of the UE
     */
    void SetL2Id(uint32_t l2Id);

    /**
     * @brief Get the Layer 2 ID used by the UE
     *
     * @return l2Id the Layer 2 ID  of the UE
     */
    uint32_t GetL2Id() const;

    /**
     * Compare two DiscoveryInfo for std::set ordering
     */
    bool CompareDiscoveryInfo(const DiscoveryInfo& a, const DiscoveryInfo& b) const;

    /**
     * TracedCallback signature for transmission and reception of PC5-S messages.
     *
     * @param [in] srcL2Id the layer 2 ID of the source of the message
     * @param [in] dstL2Id the layer 2 ID of the destination of the message
     * @param [in] isTx flag indicating if the trace was called upon a transmission
     * @param [in] p the PC5-S message
     */
    typedef void (*PC5SignallingPacketTracedCallback)(uint32_t srcL2Id,
                                                      uint32_t dstL2Id,
                                                      bool isTx,
                                                      Ptr<Packet> p);

    /**
     * TracedCallback signature for transmission of discovery messages.
     *
     * @param [in] senderL2Id the layer 2 ID of the source of the message
     * @param [in] receiverL2Id the layer 2 ID of the destination of the message
     * @param [in] discHeader the discovery message
     */
    typedef void (*DiscoveryTracedCallback)(uint32_t senderL2Id,
                                            uint32_t receiverL2Id,
                                            NrSlDiscoveryHeader discHeader);

    /**
     * TracedCallback signature for direct link establishment and release
     *
     * @param [in] srcL2Id layer 2 ID of the source
     * @param [in] selfIpv4Addr Ipv4Address of the source
     * @param [in] peerL2Id layer 2 ID of the peer
     * @param [in] peerIpv4Addr Ipv4Address of the source
     */
    typedef void (*DirectLinkTracedCallback)(uint32_t srcL2Id,
                                             Ipv4Address selfIpv4Addr,
                                             uint32_t peerL2Id,
                                             Ipv4Address peerIpv4Addr);

    /**
     * TracedCallback signature for RLF detection
     *
     * @param [in] srcL2Id The source L2 ID
     * @param [in] dstL2Id The destination L2 ID
     * @param [in] event The event that lead to RLF detection
     */
    typedef void (*RadioLinkFailureTracedCallback)(uint32_t srcL2Id,
                                                   uint32_t dstL2Id,
                                                   RlfEvent event);

    /**
     * TracedCallback signature for L3 SD-RSRP measurement report
     *
     * @param [in] srcL2Id The source L2 ID
     * @param [in] peerL2Id The L2 ID of the peer UE
     * @param [in] sdRsrp The measured SD-RSRP (dBm)
     * @param [in] eligible Flag indicating whether the threshold conditions are met at the RRC or
     * not
     */
    typedef void (*L3SdRsrpReportTracedCallback)(uint32_t srcL2Id,
                                                 uint32_t peerL2Id,
                                                 double sdRsrp,
                                                 bool eligible);

    /**
     * @brief Add discovery application
     * Add payload depending on the interest (monitoring or announcing)
     * @param appCode application code to be announced or monitored
     * @param dstL2Id destination layer 2 ID to be set for this appCode
     * @param role Indicates if announcing or monitoring
     */
    void AddDiscoveryApp(uint32_t appCode, uint32_t dstL2Id, DiscoveryRole role);

    /**
     * @brief Remove Sidelink discovery applications
     * Remove application from discovery map
     * @param appCode application code to be removed
     * @param role Indicates if announcing or monitoring
     */
    void RemoveDiscoveryApp(uint32_t appCode, DiscoveryRole role);

    /**
     * Checks if the given app must be discovered
     * @param msgType The message type
     * @param appCode The application code
     * @return true if the node is monitoring for this message type and appCode
     */
    bool IsMonitoringApp(uint8_t msgType, uint32_t appCode);

    /**
     * @brief Send discovery message
     * @param appCode Application code
     * @param dstL2Id destination L2 ID
     */
    void SendDiscovery(uint32_t appCode, uint32_t dstL2Id);

    /**
     * Map to keep track of the active SL Discovery RBs.
     * A bit set of 4 bits representing each of the 4 LcIds of the SL-SRBs is stored per peerL2Id.
     * Bit set to 1 means the SL-SRB of the corresponding LcId is active
     */
    typedef std::list<uint32_t> NrSlDiscoveryRadioBearers;

    ///< Frequency of Discovery messages in seconds
    Time m_discoveryInterval;
    ///< Start time of discovery
    Time m_discoveryStartTime;
    ///< Stop time of discovery
    Time m_discoveryStopTime;

    /**
     * @brief Add a custom QoS rule
     * @param tft The TFT with the custom QoS rule
     */
    void AddQosRule(Ptr<NrSlTft> tft);

    /**
     * @brief Set the traffic profile for the default data radio bearers of unicast links
     * @param slDrbSlInfo The traffic profile
     */
    void SetDefaultSlDrbSlInfo(SidelinkInfo slDrbSlInfo);

    /**
     * @brief Set the traffic profile for the signaling radio bearers of unicast links
     * @param slSrbSlInfo The traffic profile
     */
    void SetDefaultSlSrbSlInfo(SidelinkInfo slSrbSlInfo);
    /*
     * @brief Notifies the MANET service that a ProSe keep-alive procedure has
     *        failed for a direct link
     * @param [in] srcL2Id The L2 ID of the source
     * @param [in] dstL2Id The L2 ID of the destination
     */
    void NotifyKeepAliveFailure(uint32_t srcL2Id, uint32_t dstL2Id);

  private:
    // NrSlUeSvcRrcSapUser methods
    void DoReceiveNrSlSignalling(Ptr<Packet> packet, uint32_t srcL2Id);
    void DoNotifySvcNrSlDataRadioBearerActivated(uint32_t peerL2Id);
    void DoNotifySvcNrSlDataRadioBearerRemoved(uint32_t peerL2Id);
    void DoNotifyNrSlHarqProcessMaxTransmissionsWithNoFeedback(uint32_t peerL2Id);
    void DoReceiveNrSlDiscovery(Ptr<Packet> packet, uint32_t srcL2Id);
    void DoReceiveNrSdRsrpMeasurements(uint32_t l2Id, double value, bool eligible);
    void DoReceiveNrSlRsrpMeasurements(uint32_t l2Id, double value, bool eligible);
    void DoNotifyDataReceived(uint32_t srcL2Id);
    bool DoConfirmSendRequest(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId);

    // NrSlUeProseDirLnkSapUser methods
    void DoSendNrSlPc5SMessage(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId);
    void DoNotifyChangeOfDirectLinkState(uint32_t peerL2Id,
                                         NrSlUeProseDirLnkSapUser::ChangeOfStateNotification info);
    void DoSendNrSlDiscovery(Ptr<Packet> packet, uint32_t dstL2Id);

    /**
     * Trace information upon transmission and reception of PC5-S messages
     */
    TracedCallback<uint32_t, uint32_t, bool, Ptr<Packet>> m_pc5SignallingPacketTrace;

    /**
     * Track the transmission of discovery message
     * Exporting sender L2 ID, receiver L2 ID, transmission or not flag, and discovery message.
     */
    TracedCallback<uint32_t, uint32_t, bool, NrSlDiscoveryHeader> m_discoveryTrace;

    /**
     * Trace fired when a Prose direct link is established
     */
    TracedCallback<uint32_t, Ipv4Address, uint32_t, Ipv4Address> m_directLinkEstablishedTrace;

    /**
     * Trace fired when a Prose direct link enters releasing state
     */
    TracedCallback<uint32_t, Ipv4Address, uint32_t, Ipv4Address> m_directLinkReleasingTrace;

    /**
     * Trace fired when RLF is detected
     */
    TracedCallback<uint32_t, uint32_t, RlfEvent> m_rlfTrace;

    /**
     * Trace fired when a SD-RSRP measurement report is received from the RRC
     */
    TracedCallback<uint32_t, uint32_t, double, bool> m_l3SdRsrpReportTrace;

    // SAP pointers
    NrSlUeSvcNasSapUser* m_nrSlUeSvcNasSapUser; ///< NR SL UE SERVICE NAS SAP user
    NrSlUeSvcNasSapProvider* m_nrSlUeSvcNasSapProvider{
        nullptr}; ///< NR SL UE SERVICE NAS SAP Provider

    NrSlUeSvcRrcSapUser* m_nrSlUeSvcRrcSapUser;                  ///< NR SL UE SERVICE SAP User
    NrSlUeSvcRrcSapProvider* m_nrSlUeSvcRrcSapProvider{nullptr}; ///< NR SL UE SERVICE SAP Provider

    NrSlUeProseDirLnkSapUser* m_nrSlUeProseDirLnkSapUser; ///< ProSe Direct Link SAP User

    // Class internal private methods and member variables
    NrSlDirectLinkContextMapPerPeerL2Id m_unicastDirectLinks; ///< Active direct link contexts map
    NrSlSignalingRadioBearersPerPeerL2Id m_activeSlSrbs; ///< Active SL signalling radio bearers

    uint64_t m_imsi;           //!< the IMSI used by the UE
    uint32_t m_l2Id;           ///< the L2Id used by this UE
    Ptr<NetDevice> m_ueDevice; //< the net device of the this UE

    Ptr<NrPointToPointEpcHelper> m_epcHelper; //!< pointer to the EPC helper

    ///< List of active discovery RBs
    NrSlDiscoveryRadioBearers m_activeSlDiscoveryRbs;

    ///< list of IDs of applications to announce/response
    std::set<DiscoveryInfo, DiscoveryInfoCmp> m_discoverySet;

    SidelinkInfo
        m_slSrbSlInfo; ///< Default values for traffic profile used for signaling radio bearers

    bool m_useSdRsrpForRlf; //!< Indicates if SD-RSRP should be used for RLF detection
    bool m_useSlRsrpForRlf; //!< Indicates if SL-RSRP should be used for RLF detection

    Ptr<nhdp::NhdpClient> m_nhdp; //!< The NHDP layer of the MANET

    /**
     * @brief Return a pointer to the direct link to peerL2Id for the provided app code.
     *
     * Return a null value if there is no known corresponding direct link.
     *
     * @param appCode the relevant app code
     * @param peerL2Id the destination layer 2 ID of interest
     */
    Ptr<NrSlUeProseDirectLink> GetDirectLink(uint32_t appCode, uint32_t peerL2Id);

    SidelinkInfo
        m_slDrbSlInfo; ///< Traffic profile used for unicast links default data radio bearers

    std::list<Ptr<NrSlTft>> m_qosRules; ///< List of configured custom QoS rules

    std::map<uint32_t, uint16_t>
        m_countNrSdRsrpMeas; ///< Current number of received SD-RSRP Measurements indexed by
                             ///< peer L2 ID. Used for threshold condition evaluation

    /**
     * @brief Creates the TFT and instructs the activation of the corresponding
     *        data radior bearer for a direct link
     *
     * @param peerL2Id the L2 ID of the peer UE in the link
     * @param ipInfo the IP configuration to use
     */
    void ActivateDirectLinkDataRadioBearer(uint32_t peerL2Id,
                                           NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo);

    /**
     * @brief Instruct the removal of data radio bearer for the direct link
     *
     * @param dstL2Id destination layer 2 ID
     * @param ipIndo destination ip info
     *
     */
    void DeleteDirectLinkDataRadioBearer(uint32_t dstL2Id,
                                         NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo);

    /**
     * Trace sink for OLSR route additions
     * @param dest Destination address
     * @param nextHop Next hop address
     * @param interface Interface
     * @param distance Distance to destination
     */
    void OlsrAddRoute(const Ipv4Address& dest,
                      const Ipv4Address& nextHop,
                      uint32_t interface,
                      uint32_t distance);

    /**
     * Trace sink for OLSR route removals
     * @param dest Destination address
     * @param nextHop Next hop address
     * @param interface Interface
     * @param distance Distance to destination
     */
    void OlsrRemoveRoute(const Ipv4Address& dest,
                         const Ipv4Address& nextHop,
                         uint32_t interface,
                         uint32_t distance);

    /**
     * Look into the unicast direct link data structure to find the peer
     * L2 ID corresponding to the next hop Ipv4Address
     *
     * @param nextHop the address to search
     * @return L2Id corresponding to the next hop
     */
    std::optional<uint32_t> FindL2IdForNextHopIpv4Address(const Ipv4Address& nextHop);

    /**
     * Get the Ipv4Address for the NetDevice to which this object is aggregated
     * @return Ipv4Address for the NetDevice to which this object is aggregated
     */
    Ipv4Address GetIpv4Address() const;

    /**
     * Get the Ipv4Address for the peer associated with the provided L2 ID
     * @param peerL2Id the peer L2 ID
     * @return Ipv4Address for the peer associated with the provided L2 ID
     */
    Ipv4Address GetIpv4AddressFromPeerL2Id(uint32_t peerL2Id) const;

    /**
     * @brief Try to setup a unicast link with a peer
     * @param [in] targetL2Id The L2 ID of the peer
     */
    void InitiateLink(uint32_t targetL2Id);
};

} // namespace ns3

#endif /* NR_SL_L3_MANET_SERVICE_H */
