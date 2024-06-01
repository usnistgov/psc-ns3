/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 */

#ifndef EPC_UE_NAS_H
#define EPC_UE_NAS_H

#include "epc-tft-classifier.h"
#include "eps-bearer.h"
#include "lte-as-sap.h"
#include "nr-sl-ue-prose-dir-lnk-sap.h"
#include "nr-sl-ue-svc-nas-sap.h"

#include <ns3/object.h>
#include <ns3/traced-callback.h>

namespace ns3
{

class EpcHelper;
class LteSlTft;
class NetDevice;

class EpcUeNas : public Object
{
    /// allow MemberLteAsSapUser<EpcUeNas> class friend access
    friend class MemberLteAsSapUser<EpcUeNas>;
    /// allow MemberNrSlUeSvcNasSapProvider<LteUeRrc> class friend access
    friend class MemberNrSlUeSvcNasSapProvider<EpcUeNas>;

  public:
    /**
     * Constructor
     */
    EpcUeNas();

    /**
     * Destructor
     */
    ~EpcUeNas() override;

    // inherited from Object
    void DoDispose() override;
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     *
     * \param dev the UE NetDevice
     */
    void SetDevice(Ptr<NetDevice> dev);

    /**
     *
     *
     * \param imsi the unique UE identifier
     */
    void SetImsi(uint64_t imsi);

    /**
     *
     * \param csgId Closed Subscriber Group identity
     */
    void SetCsgId(uint32_t csgId);

    /**
     *
     * \return csgId Closed Subscriber Group identity
     */
    uint32_t GetCsgId() const;

    /**
     * Set the AS SAP provider to interact with the NAS entity
     *
     * \param s the AS SAP provider
     */
    void SetAsSapProvider(LteAsSapProvider* s);

    /**
     *
     *
     * \return the AS SAP user exported by this RRC
     */
    LteAsSapUser* GetAsSapUser();

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
     * set the callback used to forward data packets up the stack
     *
     * \param cb the callback
     */
    void SetForwardUpCallback(Callback<void, Ptr<Packet>> cb);

    /**
     * \brief Causes NAS to tell AS to find a suitable cell and camp to it.
     *
     * \param dlEarfcn the DL frequency of the eNB
     */
    void StartCellSelection(uint32_t dlEarfcn);

    /**
     * \brief Causes NAS to tell AS to go to ACTIVE state.
     *
     * The end result is equivalent with EMM Registered + ECM Connected states.
     */
    void Connect();

    /**
     * \brief Causes NAS to tell AS to camp to a specific cell and go to ACTIVE
     *        state.
     * \param cellId the id of the eNB to camp on
     * \param dlEarfcn the DL frequency of the eNB
     *
     * The end result is equivalent with EMM Registered + ECM Connected states.
     * Since RRC Idle Mode cell selection is not supported yet, we force the UE
     * RRC to be camped on a specific eNB.
     */
    void Connect(uint16_t cellId, uint32_t dlEarfcn);

    /**
     * instruct the NAS to disconnect
     *
     */
    void Disconnect();

    /**
     * Activate an EPS bearer
     *
     * \param bearer the characteristics of the bearer to be created
     * \param tft the TFT identifying the traffic that will go on this bearer
     */
    void ActivateEpsBearer(EpsBearer bearer, Ptr<EpcTft> tft);

    /**
     * Enqueue an IP packet on the proper bearer for uplink transmission
     *
     * \param p the packet
     * \param protocolNumber the protocol number of the packet
     *
     * \return true if successful, false if an error occurred
     */
    bool Send(Ptr<Packet> p, uint16_t protocolNumber);

    /**
     * Definition of NAS states as per "LTE - From theory to practice",
     * Section 3.2.3.2 "Connection Establishment and Release"
     *
     */
    enum State
    {
        OFF = 0,
        ATTACHING,
        IDLE_REGISTERED,
        CONNECTING_TO_EPC,
        ACTIVE,
        NUM_STATES
    };

    /**
     * \return The current state
     */
    State GetState() const;

    /**
     * TracedCallback signature for state change events.
     *
     * \param [in] oldState The old State.
     * \param [in] newState the new State.
     */
    typedef void (*StateTracedCallback)(const State oldState, const State newState);

  private:
    // LTE AS SAP methods
    /// Notify successful connection
    void DoNotifyConnectionSuccessful();
    /// Notify connection failed
    void DoNotifyConnectionFailed();
    /// Notify connection released
    void DoNotifyConnectionReleased();
    /**
     * Receive data
     * \param packet the packet
     */
    void DoRecvData(Ptr<Packet> packet);

    // internal methods
    /**
     * Activate EPS Bearer
     * \param bearer the EPS bearer
     * \param tft the EPC TFT
     */
    void DoActivateEpsBearer(EpsBearer bearer, Ptr<EpcTft> tft);
    /**
     * Switch the UE RRC to the given state.
     * \param s the destination state
     */
    void SwitchToState(State s);

    /// The current UE NAS state.
    State m_state;

    /**
     * The `StateTransition` trace source. Fired upon every UE NAS state
     * transition. Exporting old state and new state.
     * \todo This should be a TracedValue
     */
    TracedCallback<State, State> m_stateTransitionCallback;

    /// The UE NetDevice.
    Ptr<NetDevice> m_device;

    /// The unique UE identifier.
    uint64_t m_imsi;

    /// Closed Subscriber Group identity.
    uint32_t m_csgId;

    /// LTE SAP provider
    LteAsSapProvider* m_asSapProvider;
    /// LTE SAP user
    LteAsSapUser* m_asSapUser;

    uint8_t m_bidCounter;             ///< bid counter
    EpcTftClassifier m_tftClassifier; ///< tft classifier

    Callback<void, Ptr<Packet>> m_forwardUpCallback; ///< upward callback

    /// BearerToBeActivated structure
    struct BearerToBeActivated
    {
        EpsBearer bearer; ///< EPS bearer
        Ptr<EpcTft> tft;  ///< TFT
    };

    std::list<BearerToBeActivated> m_bearersToBeActivatedList; ///< bearers to be activated list

    /**
     * bearers to be activated list maintained and to be used for reconnecting
     * an out-of-sync UE
     *
     */
    std::list<BearerToBeActivated> m_bearersToBeActivatedListForReconnection;

    // NR SL

  public:
    /**
     * \brief Activate NR Sidelink bearer
     *
     * \param tft The bearer information
     */
    void ActivateNrSlBearer(Ptr<LteSlTft> tft);

    /**
     * \brief Delete NR Sidelink bearer
     *
     * \param tft The bearer information
     */
    void DeleteNrSlBearer(Ptr<LteSlTft> tft);

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

    std::list<Ptr<LteSlTft>> m_pendingSlBearersList; ///< pending NR Sidelink bearers list

    std::list<Ptr<LteSlTft>> m_slBearersActivatedList; ///< Sidelink NR bearers activated list

    // Service layer <-> NAS interfaces
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
    void DoActivateSvcNrSlDataRadioBearer(Ptr<LteSlTft> tft);

    /**
     * \ brief Instruct the NAS to delete the NR sidelink data bearer
     *
     * \param dstL2Id destination layer 2 ID
     */
    void DoDeleteSvcNrSlDataRadioBearer(Ptr<LteSlTft> tft);

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

#endif // EPC_UE_NAS_H
