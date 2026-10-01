// Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2018 Fraunhofer ESK : RLF extensions
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Authors:
//   Nicola Baldo <nbaldo@cttc.es>
//   Marco Miozzo <mmiozzo@cttc.es>
//   Manuel Requena <manuel.requena@cttc.es>
// Modified by:
//   Danilo Abrignani <danilo.abrignani@unibo.it> (Carrier Aggregation - GSoC 2015)
//   Biljana Bojovic <biljana.bojovic@cttc.es> (Carrier Aggregation)
//   Vignesh Babu <ns3-dev@esk.fraunhofer.de> (RLF extensions)

#ifndef NR_GNB_RRC_H
#define NR_GNB_RRC_H

#include "nr-anr-sap.h"
#include "nr-ccm-rrc-sap.h"
#include "nr-component-carrier.h"
#include "nr-epc-gnb-s1-sap.h"
#include "nr-epc-x2-sap.h"
#include "nr-gnb-cmac-sap.h"
#include "nr-gnb-cphy-sap.h"
#include "nr-handover-management-sap.h"
#include "nr-mac-sap.h"
#include "nr-pdcp-sap.h"
#include "nr-rrc-sap.h"

#include <ns3/event-id.h>
#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/traced-callback.h>

#include <map>
#include <set>
#include <vector>

namespace ns3
{
class BandwidthPartGnb;

class NrRadioBearerInfo;
class NrSignalingRadioBearerInfo;
class NrDataRadioBearerInfo;
class NrGnbRrc;
class Packet;

/**
 * \ingroup nr
 * Manages all the radio bearer information possessed by the gNB RRC for a
 * single UE.
 */
class NrUeManager : public Object
{
    /// allow NrPdcpSpecificNrPdcpSapUser<NrUeManager> class friend access
    friend class NrPdcpSpecificNrPdcpSapUser<NrUeManager>;

  public:
    /**
     * The state of the NrUeManager at the gNB RRC
     *
     */
    enum State
    {
        INITIAL_RANDOM_ACCESS = 0,
        CONNECTION_SETUP,
        CONNECTION_REJECTED,
        ATTACH_REQUEST,
        CONNECTED_NORMALLY,
        CONNECTION_RECONFIGURATION,
        CONNECTION_REESTABLISHMENT,
        HANDOVER_PREPARATION,
        HANDOVER_JOINING,
        HANDOVER_PATH_SWITCH,
        HANDOVER_LEAVING,
        NUM_STATES
    };

    NrUeManager();

    /**
     * NrUeManager constructor
     *
     * \param rrc pointer to the NrGnbRrc holding this NrUeManager
     * \param rnti RNTI of the UE
     * \param s initial state of the NrUeManager
     * \param componentCarrierId primary component carrier ID
     */
    NrUeManager(Ptr<NrGnbRrc> rrc, uint16_t rnti, State s, uint8_t componentCarrierId);

    ~NrUeManager() override;

    /**
     * \brief Perform post-creation configuration steps
     *
     * This method is called after the NrUeManager is created and added
     * to the NrGnbRrc's UE map.  This method configures the gNB side of
     * the SRB0 and SRB1, the SAP objects, and propagates some information
     * to the MAC and PHY layers.
     */
    void Configure();

  protected:
    // inherited from Object
    void DoDispose() override;

  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Set the identifiers of the source gNB for the case where a UE
     * joins the current gNB as part of a handover procedure
     *
     * \param sourceCellId
     * \param sourceX2apId
     */
    void SetSource(uint16_t sourceCellId, uint16_t sourceX2apId);

    /**
     * Set the IMSI
     *
     * \param imsi the IMSI
     */
    void SetImsi(uint64_t imsi);

    /**
     * Process Initial context setup request message from the MME.
     * It triggers RRC connection reconfiguration.
     */
    void InitialContextSetupRequest();

    /**
     * \brief Initialize the SAP objects
     */
    void ConfigureSap();

    /**
     * \brief Initialize the gNB side of SRB0
     */
    void ConfigureSrb0();

    /**
     * \brief Initialize the gNB side of SRB1
     */
    void ConfigureSrb1();

    /**
     * \brief Configure MAC and PHY aspects
     *
     * Generate UeUpdateConfigurationReq() towards the MAC, with RNTI and
     * transmission mode. Generate SetSrsConfigurationIndex() towards
     * the PHY.  Schedule this NrUeManager instance to be deleted if the
     * UE does not give any sign of life by the RRC connection request
     * timeout duration.
     */
    void ConfigureMacPhy();

    /**
     * Setup a new data radio bearer, including both the configuration
     * within the gNB and the necessary RRC signaling with the UE
     *
     * \param bearer the QoS characteristics of the bearer
     * \param bearerId the EPS bearer identifier
     * \param gtpTeid S1-bearer GTP tunnel endpoint identifier, see 36.423 9.2.1
     * \param transportLayerAddress  IP Address of the SGW, see 36.423 9.2.1
     *
     */
    void SetupDataRadioBearer(NrEpsBearer bearer,
                              uint8_t bearerId,
                              uint32_t gtpTeid,
                              Ipv4Address transportLayerAddress);

    /**
     * Start all configured data radio bearers. It is safe to call this
     * method if any bearer had been already started previously.
     *
     */
    void RecordDataRadioBearersToBeStarted();

    /**
     * Start the data radio bearers that have been previously recorded
     * to be started using RecordDataRadioBearersToBeStarted()
     *
     */
    void StartDataRadioBearers();

    /**
     *
     * Release a given radio bearer
     *
     * \param drbid the data radio bearer id of the bearer to be released
     */
    void ReleaseDataRadioBearer(uint8_t drbid);

    /**
     * schedule an RRC Connection Reconfiguration procedure with the UE
     *
     */
    void ScheduleRrcConnectionReconfiguration();

    /**
     * Start the handover preparation and send the handover request
     *
     * \param cellId id of the target cell
     */
    void PrepareHandover(uint16_t cellId);

    /**
     * take the necessary actions in response to the reception of an X2 HANDOVER REQUEST ACK message
     *
     * \param params
     */
    void RecvHandoverRequestAck(NrEpcX2SapUser::HandoverRequestAckParams params);

    /**
     *
     * \return the HandoverPreparationInfo sent by the source gNB to the
     * target gNB in the X2-based handover procedure
     */
    NrRrcSap::RadioResourceConfigDedicated GetRadioResourceConfigForHandoverPreparationInfo();

    /**
     * \param componentCarrierId target component carrier ID
     *
     * \return retrieve the data that the target gNB needs to send to the source
     * gNB as the Handover Command in the X2-based handover
     * procedure.
     *
     * \note mobility control info is not expected to be filled in
     * (shall be filled in by the caller).
     */
    NrRrcSap::RrcConnectionReconfiguration GetRrcConnectionReconfigurationForHandover(
        uint8_t componentCarrierId);

    /**
     * Send a data packet over the appropriate Data Radio Bearer.
     * If state is HANDOVER_JOINING (i.e. target gNB has received the
     * Handover Request), the packet is buffered.
     * If state is HANDOVER_LEAVING (i.e. source gNB has received the
     * RRC Connection Reconfiguration, the packet is sent through the
     * X2 interface.
     *
     * \param bid the corresponding EPS Bearer ID
     * \param p the packet
     */
    void SendData(uint8_t bid, Ptr<Packet> p);

    /**
     *
     * \return a list of ERAB-to-be-setup items to be put in a X2 HO REQ message
     */
    std::vector<NrEpcX2Sap::ErabToBeSetupItem> GetErabList();

    /**
     * send the UE CONTEXT RELEASE X2 message to the source gNB, thus
     * successfully terminating an X2 handover procedure
     *
     */
    void SendUeContextRelease();

    /**
     * Take the necessary actions in response to the reception of an X2 HO preparation failure
     * message
     *
     * \param cellId id of the target cell
     */
    void RecvHandoverPreparationFailure(uint16_t cellId);

    /**
     * Take the necessary actions in response to the reception of an X2 SN STATUS TRANSFER message
     *
     * \param params the SN STATUS
     */
    void RecvSnStatusTransfer(NrEpcX2SapUser::SnStatusTransferParams params);

    /**
     * Take the necessary actions in response to the reception of an X2 UE CONTEXT RELEASE message
     *
     * \param params the SN STATUS
     */
    void RecvUeContextRelease(NrEpcX2SapUser::UeContextReleaseParams params);

    /**
     * Take the necessary actions in response to the reception of an X2 UE CONTEXT RELEASE message
     *
     * \param params the SN STATUS
     */
    void RecvHandoverCancel(NrEpcX2SapUser::HandoverCancelParams params);

    // METHODS FORWARDED FROM gNB RRC SAP ///////////////////////////////////////

    /**
     * Implement the NrGnbRrcSapProvider::CompleteSetupUe interface.
     * \param params CompleteSetupUeParameters
     */
    void CompleteSetupUe(NrGnbRrcSapProvider::CompleteSetupUeParameters params);
    /**
     * Implement the NrGnbRrcSapProvider::RecvRrcConnectionRequest interface.
     * \param msg the RRC connection request message
     */
    void RecvRrcConnectionRequest(NrRrcSap::RrcConnectionRequest msg);
    /**
     * Implement the NrGnbRrcSapProvider::RecvRrcConnectionSetupCompleted interface.
     * \param msg RRC connection setup completed message
     */
    void RecvRrcConnectionSetupCompleted(NrRrcSap::RrcConnectionSetupCompleted msg);
    /**
     * Implement the NrGnbRrcSapProvider::RecvRrcConnectionReconfigurationCompleted interface.
     * \param msg RRC connection reconfiguration completed message
     */
    void RecvRrcConnectionReconfigurationCompleted(
        NrRrcSap::RrcConnectionReconfigurationCompleted msg);
    /**
     * Implement the NrGnbRrcSapProvider::RecvRrcConnectionReestablishmentRequest interface.
     * \param msg the RRC connection reestablishment request message
     */
    void RecvRrcConnectionReestablishmentRequest(NrRrcSap::RrcConnectionReestablishmentRequest msg);
    /**
     * Implement the NrGnbRrcSapProvider::RecvRrcConnectionReestablishmentComplete interface.
     * \param msg the RRC connection reestablsihment complete message
     */
    void RecvRrcConnectionReestablishmentComplete(
        NrRrcSap::RrcConnectionReestablishmentComplete msg);
    /**
     * Implement the NrGnbRrcSapProvider::RecvMeasurementReport interface.
     * \param msg the measrurement report
     */
    void RecvMeasurementReport(NrRrcSap::MeasurementReport msg);
    /**
     * Implement the NrGnbRrcSapProvider::RecvIdealUeContextRemoveRequest interface.
     *
     * \param rnti the C-RNTI identifying the user
     */
    void RecvIdealUeContextRemoveRequest(uint16_t rnti);

    // METHODS FORWARDED FROM gNB CMAC SAP //////////////////////////////////////

    /**
     * CMAC UE config update indication function
     * \param cmacParams the UE config parameters
     */
    void CmacUeConfigUpdateInd(NrGnbCmacSapUser::UeConfig cmacParams);

    // METHODS FORWARDED FROM gNB PDCP SAP //////////////////////////////////////

    /**
     * Receive PDCP SDU function
     * \param params the receive PDCP SDU parameters
     */
    void DoReceivePdcpSdu(NrPdcpSapUser::ReceivePdcpSduParameters params);

    /**
     *
     * \return the RNTI, i.e., an UE identifier that is unique within
     * the cell
     */
    uint16_t GetRnti() const;

    /**
     *
     * \return the IMSI, i.e., a globally unique UE identifier
     */
    uint64_t GetImsi() const;

    /**
     *
     * \return the primary component carrier ID
     */
    uint8_t GetComponentCarrierId() const;

    /**
     *
     * \return the SRS Configuration Index
     */
    uint16_t GetSrsConfigurationIndex() const;

    /**
     * Set the SRS configuration index and do the necessary reconfiguration
     *
     * \param srsConfIndex
     */
    void SetSrsConfigurationIndex(uint16_t srsConfIndex);

    /**
     *
     * \return the current state
     */
    State GetState() const;

    /**
     * Configure PdschConfigDedicated (i.e. P_A value) for UE and start RrcConnectionReconfiguration
     * to inform UE about new PdschConfigDedicated
     *
     * \param pdschConfigDedicated new pdschConfigDedicated (i.e. P_A value) to be set
     */
    void SetPdschConfigDedicated(NrRrcSap::PdschConfigDedicated pdschConfigDedicated);

    /**
     * Cancel all timers which are running for the UE
     *
     */
    void CancelPendingEvents();

    /**
     *  \brief This function acts as an interface to trigger the connection
     *  release towards gNB, EPC and UE.
     *
     */
    void SendRrcConnectionRelease();

    /**
     * \brief build handover preparation failure message
     * \return the handover preparation failure message.
     */
    NrEpcX2Sap::HandoverPreparationFailureParams BuildHoPrepFailMsg();

    /**
     * \brief build handover cancel message
     * \return the handover cancel message.
     */
    NrEpcX2Sap::HandoverCancelParams BuildHoCancelMsg();

    /**
     * TracedCallback signature for state transition events.
     *
     * \param [in] imsi
     * \param [in] cellId
     * \param [in] rnti
     * \param [in] oldState
     * \param [in] newState
     */
    typedef void (*StateTracedCallback)(const uint64_t imsi,
                                        const uint16_t cellId,
                                        const uint16_t rnti,
                                        const State oldState,
                                        const State newState);

  private:
    /**
     * Add a new NrDataRadioBearerInfo structure to the NrUeManager
     *
     * \param radioBearerInfo
     *
     * \return the id of the newly added data radio bearer structure
     */
    uint8_t AddDataRadioBearerInfo(Ptr<NrDataRadioBearerInfo> radioBearerInfo);

    /**
     * \param drbid the Data Radio Bearer id
     *
     * \return the corresponding NrDataRadioBearerInfo
     */
    Ptr<NrDataRadioBearerInfo> GetDataRadioBearerInfo(uint8_t drbid);

    /**
     * remove the NrDataRadioBearerInfo corresponding to a bearer being released
     *
     * \param drbid the Data Radio Bearer id
     */
    void RemoveDataRadioBearerInfo(uint8_t drbid);

    /**
     *
     * \return an RrcConnectionReconfiguration struct built based on the
     * current configuration
     */
    NrRrcSap::RrcConnectionReconfiguration BuildRrcConnectionReconfiguration();

    /**
     *
     * \return an NonCriticalExtensionConfiguration struct built based on the
     * current configuration
     */
    NrRrcSap::NonCriticalExtensionConfiguration BuildNonCriticalExtensionConfigurationCa();

    /**
     *
     * \return a RadioResourceConfigDedicated struct built based on the
     * current configuration
     */
    NrRrcSap::RadioResourceConfigDedicated BuildRadioResourceConfigDedicated();

    /**
     *
     * \return a newly allocated identifier for a new RRC transaction
     */
    uint8_t GetNewRrcTransactionIdentifier();

    /**
     * \param lcid a Logical Channel Identifier
     *
     * \return the corresponding Data Radio Bearer Id
     */
    uint8_t Lcid2Drbid(uint8_t lcid);

    /**
     * \param drbid a Data Radio Bearer Id
     *
     * \return the corresponding  Logical Channel Identifier
     */
    uint8_t Drbid2Lcid(uint8_t drbid);

    /**
     * \param lcid a  Logical Channel Identifier
     *
     * \return the corresponding EPS Bearer Identifier
     */
    uint8_t Lcid2Bid(uint8_t lcid);

    /**
     * \param bid  an EPS Bearer Identifier
     *
     * \return the corresponding Logical Channel Identifier
     */
    uint8_t Bid2Lcid(uint8_t bid);

    /**
     * \param drbid Data Radio Bearer Id
     *
     * \return the corresponding EPS Bearer Identifier
     */
    uint8_t Drbid2Bid(uint8_t drbid);

    /**
     * \param bid an EPS Bearer Identifier
     *
     * \return the corresponding Data Radio Bearer Id
     */
    uint8_t Bid2Drbid(uint8_t bid);

    /**
     * Send a data packet over the appropriate Data Radio Bearer.
     * It is called by SendData if the UE is in a connected state
     * or when the RRC Connection Reconfiguration Complete message
     * is received and the packets are debuffered.
     *
     * \param bid the corresponding EPS Bearer ID
     * \param p the packet
     */
    void SendPacket(uint8_t bid, Ptr<Packet> p);

    /**
     * Switch the NrUeManager to the given state
     *
     * \param s the given state
     */
    void SwitchToState(State s);

    uint8_t m_lastAllocatedDrbid; ///< last allocated Data Radio Bearer ID

    /**
     * The `DataRadioBearerMap` attribute. List of UE DataRadioBearerInfo by
     * DRBID.
     */
    std::map<uint8_t, Ptr<NrDataRadioBearerInfo>> m_drbMap;

    /**
     * The `Srb0` attribute. SignalingRadioBearerInfo for SRB0.
     */
    Ptr<NrSignalingRadioBearerInfo> m_srb0;
    /**
     * The `Srb1` attribute. SignalingRadioBearerInfo for SRB1.
     */
    Ptr<NrSignalingRadioBearerInfo> m_srb1;

    /**
     * The `C-RNTI` attribute. Cell Radio Network Temporary Identifier.
     */
    uint16_t m_rnti;
    /**
     * International Mobile Subscriber Identity assigned to this UE. A globally
     * unique UE identifier.
     */
    uint64_t m_imsi;
    /**
     * ID of the primary CC for this UE
     */
    uint8_t m_componentCarrierId;

    uint8_t m_lastRrcTransactionIdentifier; ///< last RRC transaction identifier

    NrRrcSap::PhysicalConfigDedicated m_physicalConfigDedicated; ///< physical config dedicated
    /// Pointer to the parent eNodeB RRC.
    Ptr<NrGnbRrc> m_rrc;
    /// The current NrUeManager state.
    State m_state;

    NrPdcpSapUser* m_drbPdcpSapUser; ///< DRB PDCP SAP user

    bool m_pendingRrcConnectionReconfiguration; ///< pending RRC connection reconfiguration

    /**
     * The `StateTransition` trace source. Fired upon every UE state transition
     * seen by the NrUeManager at the gNB RRC. Exporting IMSI, cell ID, RNTI, old
     * state, and new state.
     */
    TracedCallback<uint64_t, uint16_t, uint16_t, State, State> m_stateTransitionTrace;

    /**
     * The `DrbCreated` trace source. Fired when DRB is created, i.e.
     * the RLC and PDCP entities are created for one logical channel.
     * Exporting IMSI, cell ID, RNTI, LCID.
     */
    TracedCallback<uint64_t, uint16_t, uint16_t, uint8_t> m_drbCreatedTrace;

    uint16_t m_sourceX2apId;              ///< source X2 ap ID
    uint16_t m_targetX2apId;              ///< target X2 ap ID
    uint16_t m_sourceCellId;              ///< source cell ID
    uint16_t m_targetCellId;              ///< target cell ID
    std::list<uint8_t> m_drbsToBeStarted; ///< DRBS to be started
    bool m_needPhyMacConfiguration;       ///< need Phy MAC configuration

    /**
     * Time limit before a _connection request timeout_ occurs. Set after a new
     * UE context is added after a successful Random Access. Calling
     * NrGnbRrc::ConnectionRequestTimeout() when it expires. Cancelled when RRC
     * CONNECTION REQUEST is received.
     */
    EventId m_connectionRequestTimeout;
    /**
     * Time limit before a _connection setup timeout_ occurs. Set after an RRC
     * CONNECTION SETUP is sent. Calling NrGnbRrc::ConnectionSetupTimeout() when
     * it expires. Cancelled when RRC CONNECTION SETUP COMPLETE is received.
     */
    EventId m_connectionSetupTimeout;
    /**
     * The delay before a _connection rejected timeout_ occurs. Set after an RRC
     * CONNECTION REJECT is sent. Calling NrGnbRrc::ConnectionRejectedTimeout()
     * when it expires.
     */
    EventId m_connectionRejectedTimeout;
    /**
     * Time limit before a _handover joining timeout_ occurs. Set after a new UE
     * context is added after receiving a handover request. Calling
     * NrGnbRrc::HandoverJoiningTimeout() when it expires. Cancelled when
     * RRC CONNECTION RECONFIGURATION COMPLETE is received.
     */
    EventId m_handoverJoiningTimeout;
    /**
     * Time limit before a _handover leaving timeout_ occurs. Set after a
     * handover command is sent. Calling NrGnbRrc::HandoverLeavingTimeout()
     * when it expires. Cancelled when RRC CONNECTION RE-ESTABLISHMENT or X2
     * UE CONTEXT RELEASE is received.
     */
    EventId m_handoverLeavingTimeout;

    /// Define if the Carrier Aggregation was already configure for the current UE on not
    bool m_caSupportConfigured;

    /// Pending start data radio bearers
    bool m_pendingStartDataRadioBearers;

    /**
     * Packet buffer for when UE is doing the handover.
     * The packets are stored with the bid (bearer ID).
     *
     * Source gNB starts forwarding data to target gNB through the X2 interface
     * when it sends RRC Connection Reconfiguration to the UE.
     * Target gNB buffers data until it receives RRC Connection Reconfiguration
     * Complete from the UE.
     */
    std::list<std::pair<uint8_t, Ptr<Packet>>> m_packetBuffer;

}; // end of `class NrUeManager`

/**
 * \ingroup nr
 *
 * The NR Radio Resource Control entity at the gNB
 */
class NrGnbRrc : public Object
{
    /// allow GnbRrcMemberNrGnbCmacSapUser class friend access
    friend class GnbRrcMemberNrGnbCmacSapUser;
    /// allow MemberNrHandoverManagementSapUser<NrGnbRrc> class friend access
    friend class MemberNrHandoverManagementSapUser<NrGnbRrc>;
    /// allow MemberNrAnrSapUser<NrGnbRrc> class friend access
    friend class MemberNrAnrSapUser<NrGnbRrc>;
    /// allow MemberNrGnbRrcSapProvider<NrGnbRrc> class friend access
    friend class MemberNrGnbRrcSapProvider<NrGnbRrc>;
    /// allow MemberNrGnbRrcSapProvider<NrGnbRrc> class friend access
    friend class NrMemberEpcGnbS1SapUser<NrGnbRrc>;
    /// allow NrMemberEpcGnbS1SapUser<NrGnbRrc> class friend access
    friend class NrEpcX2SpecificEpcX2SapUser<NrGnbRrc>;
    /// allow NrUeManager class friend access
    friend class NrUeManager;
    /// allow  MemberNrCcmRrcSapUser<NrGnbRrc> class friend access
    friend class MemberNrCcmRrcSapUser<NrGnbRrc>;

  public:
    /**
     * create an RRC instance for use within an gNB
     *
     */
    NrGnbRrc();

    /**
     * Destructor
     */
    ~NrGnbRrc() override;

    // inherited from Object
  protected:
    void DoDispose() override;

  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Set the X2 SAP this RRC should interact with
     * \param s the X2 SAP Provider to be used by this RRC entity
     */
    void SetEpcX2SapProvider(NrEpcX2SapProvider* s);

    /**
     * Get the X2 SAP offered by this RRC
     * \return s the X2 SAP User interface offered to the X2 entity by this RRC entity
     */
    NrEpcX2SapUser* GetEpcX2SapUser();

    /**
     * set the CMAC SAP this RRC should interact with
     *
     * \param s the CMAC SAP Provider to be used by this RRC
     */
    void SetNrGnbCmacSapProvider(NrGnbCmacSapProvider* s);

    /**
     * set the CMAC SAP this RRC should interact with
     *
     * \param s the CMAC SAP Provider to be used by this RRC
     * \param pos the position
     */
    void SetNrGnbCmacSapProvider(NrGnbCmacSapProvider* s, uint8_t pos);

    /**
     * Get the CMAC SAP offered by this RRC
     * \returns the CMAC SAP User interface offered to the MAC by this RRC
     */
    NrGnbCmacSapUser* GetNrGnbCmacSapUser();

    /**
     * Get the CMAC SAP offered by this RRC
     * \param pos the position
     * \returns the CMAC SAP User interface offered to the MAC by this RRC
     */
    NrGnbCmacSapUser* GetNrGnbCmacSapUser(uint8_t pos);

    /**
     * set the Handover Management SAP this RRC should interact with
     *
     * \param s the Handover Management SAP Provider to be used by this RRC
     */
    void SetNrHandoverManagementSapProvider(NrHandoverManagementSapProvider* s);

    /**
     * Get the Handover Management SAP offered by this RRC
     * \returns the Handover Management SAP User interface offered to the
     *           handover algorithm by this RRC
     */
    NrHandoverManagementSapUser* GetNrHandoverManagementSapUser();

    /**
     * set the Component Carrier Management SAP this RRC should interact with
     *
     * \param s the Component Carrier Management SAP Provider to be used by this RRC
     */
    void SetNrCcmRrcSapProvider(NrCcmRrcSapProvider* s);

    /**
     * Get the Component Carrier Management SAP offered by this RRC
     * \return s the Component Carrier Management SAP User interface offered to the
     *           carrier component selection algorithm by this RRC
     */
    NrCcmRrcSapUser* GetNrCcmRrcSapUser();

    /**
     * set the ANR SAP this RRC should interact with
     *
     * \param s the ANR SAP Provider to be used by this RRC
     */
    void SetNrAnrSapProvider(NrAnrSapProvider* s);

    /**
     * Get the ANR SAP offered by this RRC
     * \return s the ANR SAP User interface offered to the ANR instance by this
     *           RRC
     */
    NrAnrSapUser* GetNrAnrSapUser();

    /**
     * set the RRC SAP this RRC should interact with
     *
     * \param s the RRC SAP User to be used by this RRC
     */
    void SetNrGnbRrcSapUser(NrGnbRrcSapUser* s);

    /**
     *
     *
     * \return s the RRC SAP Provider interface offered to the MAC by this RRC
     */
    NrGnbRrcSapProvider* GetNrGnbRrcSapProvider();

    /**
     * set the MAC SAP provider. The gNB RRC does not use this
     * directly, but it needs to provide it to newly created RLC instances.
     *
     * \param s the MAC SAP provider that will be used by all
     * newly created RLC instances
     */
    void SetNrMacSapProvider(NrMacSapProvider* s);

    /**
     * Set the S1 SAP Provider
     *
     * \param s the S1 SAP Provider
     */
    void SetS1SapProvider(NrEpcGnbS1SapProvider* s);

    /**
     *
     * \return the S1 SAP user
     */
    NrEpcGnbS1SapUser* GetS1SapUser();

    /**
     * set the CPHY SAP this RRC should use to interact with the PHY
     *
     * \param s the CPHY SAP Provider
     */
    void SetNrGnbCphySapProvider(NrGnbCphySapProvider* s);

    /**
     * set the CPHY SAP this RRC should use to interact with the PHY
     *
     * \param s the CPHY SAP Provider
     * \param pos the position
     */
    void SetNrGnbCphySapProvider(NrGnbCphySapProvider* s, uint8_t pos);

    /**
     *
     *
     * \return s the CPHY SAP User interface offered to the PHY by this RRC
     */
    NrGnbCphySapUser* GetNrGnbCphySapUser();

    /**
     * Get the gNB CPhy SAP user
     *
     * \param pos the position
     * \return s the CPHY SAP User interface offered to the PHY by this RRC
     */
    NrGnbCphySapUser* GetNrGnbCphySapUser(uint8_t pos);

    /**
     *
     *
     * \param rnti the identifier of an UE
     *
     * \return true if the corresponding NrUeManager instance exists
     */
    bool HasUeManager(uint16_t rnti) const;

    /**
     *
     *
     * \param rnti the identifier of an UE
     *
     * \return the corresponding NrUeManager instance
     */
    Ptr<NrUeManager> GetUeManager(uint16_t rnti);

    /**
     * \brief Add a new UE measurement reporting configuration
     * \param config the new reporting configuration
     * \return the measurement IDs (measId) referring to the newly added
     *         reporting configuration
     *
     * Assuming intra-frequency environment, the new measurement reporting
     * configuration will be automatically associated to measurement
     * objects corresponding to serving cell frequencies.
     *
     * Can only be called before the start of simulation.
     */
    std::vector<uint8_t> AddUeMeasReportConfig(NrRrcSap::ReportConfigEutra config);

    /**
     * \brief Configure cell-specific parameters.
     *
     * Configure cell-specific parameters and propagate them to lower layers.
     * The parameters include bandwidth, EARFCN (E-UTRA Absolute Radio Frequency
     * Channel Number), and cell ID.
     *
     * In addition to parameter configuration, this function also performs several
     * other tasks:
     *  - Initializing UE measurement (i.e. measurement object and quantity
     *    configuration), which is expected to be further configured through
     *    `NrGnbRrc::AddUeMeasReportConfig`;
     *  - Enabling MIB (Master Information Block) broadcast transmission
     *  - Enabling SIB (System Information Block) broadcast transmission
     *
     * Typically runs when the eNodeB NetDevice is installed, for instance by
     * `NrHelper::InstallGnbDevice` (i.e. before the simulation starts).
     *
     * \warning Raises an error when executed more than once.
     *
     * \param ccPhyConf the component carrier configuration
     */
    void ConfigureCell(const std::map<uint8_t, Ptr<BandwidthPartGnb>>& ccPhyConf);

    /**
     * \brief Configure carriers.
     * \param ccPhyConf the component carrier configuration
     */
    void ConfigureCarriers(std::map<uint8_t, Ptr<BandwidthPartGnb>> ccPhyConf);

    /**
     * set the cell id of this gNB
     *
     * \param m_cellId
     */
    void SetCellId(uint16_t m_cellId);

    /**
     * set the cell id of this gNB
     *
     * \param m_cellId
     * \param ccIndex
     */
    void SetCellId(uint16_t m_cellId, uint8_t ccIndex);

    /**
     * convert the cell id to component carrier id
     *
     * \param cellId Cell ID
     *
     * \return corresponding component carrier id
     */
    uint8_t CellToComponentCarrierId(uint16_t cellId);

    /**
     * convert the component carrier id to cell id
     *
     * \param componentCarrierId component carrier ID
     *
     * \return corresponding cell ID
     */
    uint16_t ComponentCarrierToCellId(uint8_t componentCarrierId);

    /**
     * \param cellId cell ID
     * \return true if cellId is served by this gNB
     */
    bool HasCellId(uint16_t cellId) const;

    /**
     * Enqueue an IP data packet on the proper bearer for downlink
     * transmission. Normally expected to be called by the NetDevice
     * forwarding a packet coming from the NrEpcGnbApplication
     *
     * \param p the packet
     *
     * \return true if successful, false if an error occurred
     */
    bool SendData(Ptr<Packet> p);

    /**
     * set the callback used to forward data packets up the stack
     *
     * \param cb
     */
    void SetForwardUpCallback(Callback<void, Ptr<Packet>> cb);

    /**
     * Method triggered when a UE is expected to request for connection but does
     * not do so in a reasonable time. The method will remove the UE context.
     *
     * \param rnti the T-C-RNTI whose timeout expired
     */
    void ConnectionRequestTimeout(uint16_t rnti);

    /**
     * Method triggered when a UE is expected to complete a connection setup
     * procedure but does not do so in a reasonable time. The method will remove
     * the UE context.
     *
     * \param rnti the T-C-RNTI whose timeout expired
     */
    void ConnectionSetupTimeout(uint16_t rnti);

    /**
     * Method triggered a while after sending RRC Connection Rejected. The method
     * will remove the UE context.
     *
     * \param rnti the T-C-RNTI whose timeout expired
     */
    void ConnectionRejectedTimeout(uint16_t rnti);

    /**
     * Method triggered when a UE is expected to join the cell for a handover
     * but does not do so in a reasonable time. The method will remove the UE
     * context.
     *
     * \param rnti the C-RNTI whose timeout expired
     */
    void HandoverJoiningTimeout(uint16_t rnti);

    /**
     * Method triggered when a UE is expected to leave a cell for a handover
     * but no feedback is received in a reasonable time. The method will remove
     * the UE context.
     *
     * \param rnti the C-RNTI whose timeout expired
     */
    void HandoverLeavingTimeout(uint16_t rnti);

    /**
     * Send a HandoverRequest through the X2 SAP interface. This method will
     * trigger a handover which is started by the RRC by sending a handover
     * request to the target gNB over the X2 interface
     *
     * \param rnti the ID of the UE to be handed over
     * \param cellId the ID of the target gNB
     */
    void SendHandoverRequest(uint16_t rnti, uint16_t cellId);

    /**
     *  \brief This function acts as an interface to trigger Release indication messages towards gNB
     * and EPC
     * \param imsi the IMSI
     * \param rnti the RNTI
     * \param bearerId Bearer Identity which is to be de-activated
     */
    void DoSendReleaseDataRadioBearer(uint64_t imsi, uint16_t rnti, uint8_t bearerId);

    /**
     *  \brief Send RRC connection release function
     *
     *  This function acts as an interface to trigger the connection
     *  release towards gNB, EPC and UE.
     */
    void SendRrcConnectionRelease();

    /**
     * Identifies how EPS Bearer parameters are mapped to different RLC types
     *
     */
    enum NrEpsBearerToRlcMapping_t
    {
        RLC_SM_ALWAYS = 1,
        RLC_UM_ALWAYS = 2,
        RLC_AM_ALWAYS = 3,
        PER_BASED = 4
    };

    /**
     * TracedCallback signature for new Ue Context events.
     *
     * \param [in] cellId
     * \param [in] rnti
     */
    typedef void (*NewUeContextTracedCallback)(const uint16_t cellId, const uint16_t rnti);

    /**
     * TracedCallback signature for connection and handover end events.
     *
     * \param [in] imsi
     * \param [in] cellId
     * \param [in] rnti
     */
    typedef void (*ConnectionHandoverTracedCallback)(const uint64_t imsi,
                                                     const uint16_t cellId,
                                                     const uint16_t rnti);

    /**
     * TracedCallback signature for handover start events.
     *
     * \param [in] imsi
     * \param [in] cellId
     * \param [in] rnti
     * \param [in] targetCid
     */
    typedef void (*HandoverStartTracedCallback)(const uint64_t imsi,
                                                const uint16_t cellId,
                                                const uint16_t rnti,
                                                const uint16_t targetCid);

    /**
     * TracedCallback signature for receive measurement report events.
     *
     * \param [in] imsi
     * \param [in] cellId
     * \param [in] rnti
     * \param [in] report
     * \todo The \c NrRrcSap::MeasurementReport argument should be
     * changed to a const reference since the argument is large.
     */
    typedef void (*ReceiveReportTracedCallback)(const uint64_t imsi,
                                                const uint16_t cellId,
                                                const uint16_t rnti,
                                                const NrRrcSap::MeasurementReport report);

    /**
     * TracedCallback signature for timer expiry events
     *
     * \param [in] imsi
     * \param [in] rnti
     * \param [in] cellId
     * \param [in] cause
     */
    typedef void (*TimerExpiryTracedCallback)(const uint64_t imsi,
                                              const uint16_t rnti,
                                              const uint16_t cellId,
                                              const std::string cause);

    /**
     * TracedCallback signature for handover failure events.
     *
     * \param [in] imsi
     * \param [in] rnti
     * \param [in] cellId
     */
    typedef void (*HandoverFailureTracedCallback)(const uint64_t imsi,
                                                  const uint16_t rnti,
                                                  const uint16_t cellId);

  private:
    // RRC SAP methods

    /**
     * Part of the RRC protocol. Forwarding NrGnbRrcSapProvider::CompleteSetupUe interface to
     * NrUeManager::CompleteSetupUe
     * \param rnti the RNTI
     * \param params the NrGnbRrcSapProvider::CompleteSetupUeParameters
     */
    void DoCompleteSetupUe(uint16_t rnti, NrGnbRrcSapProvider::CompleteSetupUeParameters params);
    /**
     * Part of the RRC protocol. Forwarding NrGnbRrcSapProvider::RecvRrcConnectionRequest interface
     * to NrUeManager::RecvRrcConnectionRequest
     *
     * \param rnti the RNTI
     * \param msg the NrRrcSap::RrcConnectionRequest
     */
    void DoRecvRrcConnectionRequest(uint16_t rnti, NrRrcSap::RrcConnectionRequest msg);
    /**
     * Part of the RRC protocol. Forwarding NrGnbRrcSapProvider::RecvRrcConnectionSetupCompleted
     * interface to NrUeManager::RecvRrcConnectionSetupCompleted
     *
     * \param rnti the RNTI
     * \param msg the NrRrcSap::RrcConnectionSetupCompleted
     */
    void DoRecvRrcConnectionSetupCompleted(uint16_t rnti,
                                           NrRrcSap::RrcConnectionSetupCompleted msg);
    /**
     * Part of the RRC protocol. Forwarding
     * NrGnbRrcSapProvider::RecvRrcConnectionReconfigurationCompleted interface to
     * NrUeManager::RecvRrcConnectionReconfigurationCompleted
     *
     * \param rnti the RNTI
     * \param msg the NrRrcSap::RrcConnectionReconfigurationCompleted
     */
    void DoRecvRrcConnectionReconfigurationCompleted(
        uint16_t rnti,
        NrRrcSap::RrcConnectionReconfigurationCompleted msg);
    /**
     * Part of the RRC protocol. Forwarding
     * NrGnbRrcSapProvider::RecvRrcConnectionReestablishmentRequest interface to
     * NrUeManager::RecvRrcConnectionReestablishmentRequest
     *
     * \param rnti the RNTI
     * \param msg the NrRrcSap::RrcConnectionReestablishmentRequest
     */
    void DoRecvRrcConnectionReestablishmentRequest(
        uint16_t rnti,
        NrRrcSap::RrcConnectionReestablishmentRequest msg);
    /**
     * Part of the RRC protocol. Forwarding
     * NrGnbRrcSapProvider::RecvRrcConnectionReestablishmentComplete interface to
     * NrUeManager::RecvRrcConnectionReestablishmentComplete
     *
     * \param rnti the RNTI
     * \param msg the NrRrcSap::RrcConnectionReestablishmentComplete
     */
    void DoRecvRrcConnectionReestablishmentComplete(
        uint16_t rnti,
        NrRrcSap::RrcConnectionReestablishmentComplete msg);
    /**
     * Part of the RRC protocol. Forwarding NrGnbRrcSapProvider::RecvMeasurementReport interface to
     * NrUeManager::RecvMeasurementReport
     *
     * \param rnti the RNTI
     * \param msg the NrRrcSap::MeasurementReport
     */
    void DoRecvMeasurementReport(uint16_t rnti, NrRrcSap::MeasurementReport msg);
    /**
     * \brief Part of the RRC protocol. Forwarding
     * NrGnbRrcSapProvider::RecvIdealUeContextRemoveRequest interface to
     * NrUeManager::RecvIdealUeContextRemoveRequest.
     *
     * Remove the UE context at eNodeB and also remove the bearers established
     * at SGW/PGW node. Bearer info at MME is not deleted since they are added at
     * MME only at the beginning of simulation and if they are removed,
     * the bearers cannot be activated again.
     *
     * \param rnti the C-RNTI identifying the user
     */
    void DoRecvIdealUeContextRemoveRequest(uint16_t rnti);

    // S1 SAP methods

    /**
     * Initial context setup request function
     *
     * \param params NrEpcGnbS1SapUser::InitialContextSetupRequestParameters
     */
    void DoInitialContextSetupRequest(
        NrEpcGnbS1SapUser::InitialContextSetupRequestParameters params);
    /**
     * Data radio beaerer setup request function
     *
     * \param params NrEpcGnbS1SapUser::DataRadioBearerSetupRequestParameters
     */
    void DoDataRadioBearerSetupRequest(
        NrEpcGnbS1SapUser::DataRadioBearerSetupRequestParameters params);
    /**
     * Path switch request acknowledge function
     *
     * \param params NrEpcGnbS1SapUser::PathSwitchRequestAcknowledgeParameters
     */
    void DoPathSwitchRequestAcknowledge(
        NrEpcGnbS1SapUser::PathSwitchRequestAcknowledgeParameters params);

    // X2 SAP methods

    /**
     * Receive handover request function
     *
     * \param params NrEpcX2SapUser::HandoverRequestParams
     */
    void DoRecvHandoverRequest(NrEpcX2SapUser::HandoverRequestParams params);
    /**
     * Receive handover request acknowledge function
     *
     * \param params NrEpcX2SapUser::HandoverRequestAckParams
     */
    void DoRecvHandoverRequestAck(NrEpcX2SapUser::HandoverRequestAckParams params);
    /**
     * Receive handover preparation failure function
     *
     * \param params NrEpcX2SapUser::HandoverPreparationFailureParams
     */
    void DoRecvHandoverPreparationFailure(NrEpcX2SapUser::HandoverPreparationFailureParams params);
    /**
     * Receive SN status transfer function
     *
     * \param params NrEpcX2SapUser::SnStatusTransferParams
     */
    void DoRecvSnStatusTransfer(NrEpcX2SapUser::SnStatusTransferParams params);
    /**
     * Receive UE context release function
     *
     * \param params NrEpcX2SapUser::UeContextReleaseParams
     */
    void DoRecvUeContextRelease(NrEpcX2SapUser::UeContextReleaseParams params);
    /**
     * Receive load information function
     *
     * \param params NrEpcX2SapUser::LoadInformationParams
     */
    void DoRecvLoadInformation(NrEpcX2SapUser::LoadInformationParams params);
    /**
     * Receive resource status update function
     *
     * \param params NrEpcX2SapUser::ResourceStatusUpdateParams
     */
    void DoRecvResourceStatusUpdate(NrEpcX2SapUser::ResourceStatusUpdateParams params);
    /**
     * Receive UE data function
     *
     * \param params NrEpcX2SapUser::UeDataParams
     */
    void DoRecvUeData(NrEpcX2SapUser::UeDataParams params);
    /**
     * Receive Handover Cancel function.
     *
     * \param params NrEpcX2SapUser::HandoverCancelParams
     */
    void DoRecvHandoverCancel(NrEpcX2SapUser::HandoverCancelParams params);

    // CMAC SAP methods

    /**
     * Allocate temporary cell RNTI function
     *
     * \param componentCarrierId ID of the primary component carrier
     * \return temporary RNTI
     */
    uint16_t DoAllocateTemporaryCellRnti(uint8_t componentCarrierId);
    /**
     * Notify LC config result function
     *
     * \param rnti RNTI
     * \param lcid LCID
     * \param success the success indicator
     */
    void DoNotifyLcConfigResult(uint16_t rnti, uint8_t lcid, bool success);
    /**
     * RRC configuration update indication function
     *
     * \param params NrGnbCmacSapUser::UeConfig
     */
    void DoRrcConfigurationUpdateInd(NrGnbCmacSapUser::UeConfig params);

    // Handover Management SAP methods

    /**
     * Add UE measure report config for handover function
     *
     * \param reportConfig NrRrcSap::ReportConfigEutra
     * \returns measure ID
     */
    std::vector<uint8_t> DoAddUeMeasReportConfigForHandover(
        NrRrcSap::ReportConfigEutra reportConfig);
    /**
     * Add UE measure report config for component carrier function
     *
     * \param reportConfig NrRrcSap::ReportConfigEutra
     * \returns measure ID
     */
    uint8_t DoAddUeMeasReportConfigForComponentCarrier(NrRrcSap::ReportConfigEutra reportConfig);
    /**
     * \brief Set number of component carriers
     * \param numberOfComponentCarriers the number of component carriers
     */
    void DoSetNumberOfComponentCarriers(uint16_t numberOfComponentCarriers);

    /**
     * Trigger handover function
     *
     * \param rnti RNTI
     * \param targetCellId target cell ID
     */
    void DoTriggerHandover(uint16_t rnti, uint16_t targetCellId);

    // ANR SAP methods

    /**
     * Add UE measure report config for ANR function
     *
     * \param reportConfig NrRrcSap::ReportConfigEutra
     * \returns measure ID
     */
    uint8_t DoAddUeMeasReportConfigForAnr(NrRrcSap::ReportConfigEutra reportConfig);

    /**
     * Set PDSCH config dedicated function
     *
     * \param rnti the RNTI
     * \param pa NrRrcSap::PdschConfigDedicated
     */
    void DoSetPdschConfigDedicated(uint16_t rnti, NrRrcSap::PdschConfigDedicated pa);
    /**
     * Send load information function
     *
     * \param params NrEpcX2Sap::LoadInformationParams
     */
    void DoSendLoadInformation(NrEpcX2Sap::LoadInformationParams params);

    // Internal methods

    /**
     * Allocate a new RNTI for a new UE. This is done in the following cases:
     *   * T-C-RNTI allocation upon contention-based MAC Random Access procedure
     *   * target cell RNTI allocation upon handover
     *
     * \param state the initial state of the NrUeManager
     * \param componentCarrierId primary component carrier ID of the NrUeManager
     *
     * \return the newly allocated RNTI
     */
    uint16_t AddUe(NrUeManager::State state, uint8_t componentCarrierId);

    /**
     * remove a UE from the cell
     *
     * \param rnti the C-RNTI identiftying the user
     */
    void RemoveUe(uint16_t rnti);

    /**
     *
     * \param bearer the specification of an EPS bearer
     *
     * \return the type of RLC that is to be created for the given EPS bearer
     */
    TypeId GetRlcType(NrEpsBearer bearer);

    /**
     * \brief Is random access completed function
     *
     * This method is executed to decide if the non contention based
     * preamble has to reused or not upon preamble expiry. If the random access
     * in connected mode is completed, then the preamble can be reused by other UEs.
     * If not, the same UE retains the preamble and other available preambles is
     * assigned to the required UEs.
     *
     * \param rnti the C-RNTI identifying the user
     * \return true if the random access in connected mode is completed
     */
    bool IsRandomAccessCompleted(uint16_t rnti);

  public:
    /**
     * Add a neighbour with an X2 interface
     *
     * \param cellId neighbouring cell id
     */
    void AddX2Neighbour(uint16_t cellId);

    /**
     *
     * \param p the SRS periodicity in num TTIs
     */
    void SetSrsPeriodicity(uint32_t p);

    /**
     *
     * \return the current SRS periodicity
     */
    uint32_t GetSrsPeriodicity() const;

    /**
     * \brief Associate this RRC entity with a particular CSG information.
     * \param csgId the intended Closed Subscriber Group identity
     * \param csgIndication if TRUE, only CSG members are allowed to access the
     *                      cell
     *
     * CSG identity is a number identifying a Closed Subscriber Group which the
     * cell belongs to. eNodeB is associated with a single CSG identity.
     *
     * The same CSG identity can also be associated to several UEs, which is
     * equivalent as enlisting these UEs as the members of this particular CSG.
     * When the CSG indication field is set to TRUE, only UEs which are members of
     * the CSG (i.e. same CSG ID) can gain access to the eNodeB, therefore
     * enforcing closed access mode. Otherwise, the eNodeB operates as a non-CSG
     * cell and implements open access mode.
     *
     * This restriction only applies to initial cell selection and EPC-enabled
     * simulation.
     */
    void SetCsgId(uint32_t csgId, bool csgIndication);

  private:
    /**
     * Allocate a new SRS configuration index for a new UE.
     *
     * \note this method can have the side effect of updating the SRS
     * configuration index of all UEs
     *
     * \return the newly allocated SRS configuration index
     */
    uint16_t GetNewSrsConfigurationIndex();

    /**
     * remove a previously allocated SRS configuration index
     *
     * \note this method can have the side effect of updating the SRS
     * configuration index of all UEs
     *
     * \param srcCi the index to be removed
     */
    void RemoveSrsConfigurationIndex(uint16_t srcCi);

    /**
     * \return true if all the SRS indices are assigned to UEs
     */
    bool IsMaxSrsReached();

    /**
     *
     * \param bearer the characteristics of the bearer
     *
     * \return the Logical Channel Group in a bearer with these
     * characteristics is put. Used for MAC Buffer Status Reporting purposes.
     */
    uint8_t GetLogicalChannelGroup(NrEpsBearer bearer);

    /**
     *
     * \param bearer the characteristics of the bearer
     *
     * \return the priority level of a bearer with these
     * characteristics is put. Used for the part of UL MAC Scheduling
     * carried out by the UE
     */
    uint8_t GetLogicalChannelPriority(NrEpsBearer bearer);

    /**
     * method used to periodically send System Information
     *
     */
    void SendSystemInformation();

    Callback<void, Ptr<Packet>> m_forwardUpCallback; ///< forward up callback function

    /// Interface to receive messages from neighbour eNodeB over the X2 interface.
    NrEpcX2SapUser* m_x2SapUser;
    /// Interface to send messages to neighbour eNodeB over the X2 interface.
    NrEpcX2SapProvider* m_x2SapProvider;

    /// Receive API calls from the eNodeB MAC instance.
    std::vector<NrGnbCmacSapUser*> m_cmacSapUser;
    /// Interface to the eNodeB MAC instance.
    std::vector<NrGnbCmacSapProvider*> m_cmacSapProvider;

    /// Receive API calls from the handover algorithm instance.
    NrHandoverManagementSapUser* m_handoverManagementSapUser;
    /// Interface to the handover algorithm instance.
    NrHandoverManagementSapProvider* m_handoverManagementSapProvider;

    /// Receive API calls from the NrGnbComponentCarrierManager instance.
    NrCcmRrcSapUser* m_ccmRrcSapUser;
    /// Interface to the NrGnbComponentCarrierManager instance.
    NrCcmRrcSapProvider* m_ccmRrcSapProvider;

    /// Receive API calls from the ANR instance.
    NrAnrSapUser* m_anrSapUser;
    /// Interface to the ANR instance.
    NrAnrSapProvider* m_anrSapProvider;

    /// Interface to send messages to UE over the RRC protocol.
    NrGnbRrcSapUser* m_rrcSapUser;
    /// Interface to receive messages from UE over the RRC protocol.
    NrGnbRrcSapProvider* m_rrcSapProvider;

    /// Interface to the eNodeB MAC instance, to be used by RLC instances.
    NrMacSapProvider* m_macSapProvider;

    /// Interface to send messages to core network over the S1 protocol.
    NrEpcGnbS1SapProvider* m_s1SapProvider;
    /// Interface to receive messages from core network over the S1 protocol.
    NrEpcGnbS1SapUser* m_s1SapUser;

    /// Receive API calls from the eNodeB PHY instances.
    std::vector<NrGnbCphySapUser*> m_cphySapUser;
    /// Interface to the eNodeB PHY instances.
    std::vector<NrGnbCphySapProvider*> m_cphySapProvider;

    /// True if ConfigureCell() has been completed.
    bool m_configured;
    /// Downlink E-UTRA Absolute Radio Frequency Channel Number.
    uint32_t m_dlEarfcn;
    /// Uplink E-UTRA Absolute Radio Frequency Channel Number.
    uint32_t m_ulEarfcn;
    /// Downlink transmission bandwidth configuration in number of Resource Blocks.
    uint16_t m_dlBandwidth;
    /// Uplink transmission bandwidth configuration in number of Resource Blocks.
    uint16_t m_ulBandwidth;
    /// Last allocated RNTI
    uint16_t m_lastAllocatedRnti;

    /// The System Information Block Type 1 that is currently broadcasted over BCH.
    std::vector<NrRrcSap::SystemInformationBlockType1> m_sib1;

    /**
     * The `UeMap` attribute. List of NrUeManager by C-RNTI.
     */
    std::map<uint16_t, Ptr<NrUeManager>> m_ueMap;

    /**
     * List of measurement configuration which are active in every UE attached to
     * this eNodeB instance.
     */
    NrRrcSap::MeasConfig m_ueMeasConfig;

    /// List of measurement identities which are intended for handover purpose.
    std::set<uint8_t> m_handoverMeasIds;
    /// List of measurement identities which are intended for ANR purpose.
    std::set<uint8_t> m_anrMeasIds;
    /// List of measurement identities which are intended for component carrier management purposes.
    std::set<uint8_t> m_componentCarrierMeasIds;

    /// X2uTeidInfo structure
    struct X2uTeidInfo
    {
        uint16_t rnti; ///< RNTI
        uint8_t drbid; ///< DRBID
    };

    /// TEID, RNTI, DRBID
    std::map<uint32_t, X2uTeidInfo> m_x2uTeidInfoMap;

    /**
     * The `DefaultTransmissionMode` attribute. The default UEs' transmission
     * mode (0: SISO).
     */
    uint8_t m_defaultTransmissionMode;
    /**
     * The `EpsBearerToRlcMapping` attribute. Specify which type of RLC will be
     * used for each type of EPS bearer.
     */
    NrEpsBearerToRlcMapping_t m_epsBearerToRlcMapping;
    /**
     * The `SystemInformationPeriodicity` attribute. The interval for sending
     * system information.
     */
    Time m_systemInformationPeriodicity;
    /**
     * The `SrsPeriodicity` attribute. The SRS periodicity in milliseconds.
     */
    uint16_t m_srsCurrentPeriodicityId;              ///< SRS current periodicity ID
    std::set<uint16_t> m_ueSrsConfigurationIndexSet; ///< UE SRS configuration index set
    uint16_t m_lastAllocatedConfigurationIndex;      ///< last allocated configuration index
    bool m_reconfigureUes;                           ///< reconfigure UEs?

    /**
     * The `QRxLevMin` attribute. One of information transmitted within the SIB1
     * message, indicating the required minimum RSRP level that any UE must
     * receive from this cell before it is allowed to camp to this cell.
     */
    int8_t m_qRxLevMin;
    /**
     * The `AdmitHandoverRequest` attribute. Whether to admit an X2 handover
     * request from another gNB.
     */
    bool m_admitHandoverRequest;
    /**
     * The `AdmitRrcConnectionRequest` attribute. Whether to admit a connection
     * request from a UE.
     */
    bool m_admitRrcConnectionRequest;
    /**
     * The `RsrpFilterCoefficient` attribute. Determines the strength of
     * smoothing effect induced by layer 3 filtering of RSRP in all attached UE.
     * If equals to 0, no layer 3 filtering is applicable.
     */
    uint8_t m_rsrpFilterCoefficient;
    /**
     * The `RsrqFilterCoefficient` attribute. Determines the strength of
     * smoothing effect induced by layer 3 filtering of RSRQ in all attached UE.
     * If equals to 0, no layer 3 filtering is applicable.
     */
    uint8_t m_rsrqFilterCoefficient;
    /**
     * The `ConnectionRequestTimeoutDuration` attribute. After a RA attempt, if
     * no RRC CONNECTION REQUEST is received before this time, the UE context is
     * destroyed. Must account for reception of RAR and transmission of RRC
     * CONNECTION REQUEST over UL GRANT.
     */
    Time m_connectionRequestTimeoutDuration;
    /**
     * The `ConnectionSetupTimeoutDuration` attribute. After accepting connection
     * request, if no RRC CONNECTION SETUP COMPLETE is received before this time,
     * the UE context is destroyed. Must account for the UE's reception of RRC
     * CONNECTION SETUP and transmission of RRC CONNECTION SETUP COMPLETE.
     */
    Time m_connectionSetupTimeoutDuration;
    /**
     * The `ConnectionRejectedTimeoutDuration` attribute. Time to wait between
     * sending a RRC CONNECTION REJECT and destroying the UE context.
     */
    Time m_connectionRejectedTimeoutDuration;
    /**
     * The `HandoverJoiningTimeoutDuration` attribute. After accepting a handover
     * request, if no RRC CONNECTION RECONFIGURATION COMPLETE is received before
     * this time, the UE context is destroyed. Must account for reception of X2
     * HO REQ ACK by source gNB, transmission of the Handover Command,
     * non-contention-based random access and reception of the RRC CONNECTION
     * RECONFIGURATION COMPLETE message.
     */
    Time m_handoverJoiningTimeoutDuration;
    /**
     * The `HandoverLeavingTimeoutDuration` attribute. After issuing a Handover
     * Command, if neither RRC CONNECTION RE-ESTABLISHMENT nor X2 UE Context
     * Release has been previously received, the UE context is destroyed.
     */
    Time m_handoverLeavingTimeoutDuration;

    /**
     * The `NewUeContext` trace source. Fired upon creation of a new UE context.
     * Exporting cell ID and RNTI.
     */
    TracedCallback<uint16_t, uint16_t> m_newUeContextTrace;
    /**
     * The `ConnectionEstablished` trace source. Fired upon successful RRC
     * connection establishment. Exporting IMSI, cell ID, and RNTI.
     */
    TracedCallback<uint64_t, uint16_t, uint16_t> m_connectionEstablishedTrace;
    /**
     * The `ConnectionReconfiguration` trace source. Fired upon RRC connection
     * reconfiguration. Exporting IMSI, cell ID, and RNTI.
     */
    TracedCallback<uint64_t, uint16_t, uint16_t> m_connectionReconfigurationTrace;
    /**
     * The `HandoverStart` trace source. Fired upon start of a handover
     * procedure. Exporting IMSI, cell ID, RNTI, and target cell ID.
     */
    TracedCallback<uint64_t, uint16_t, uint16_t, uint16_t> m_handoverStartTrace;
    /**
     * The `HandoverEndOk` trace source. Fired upon successful termination of a
     * handover procedure. Exporting IMSI, cell ID, and RNTI.
     */
    TracedCallback<uint64_t, uint16_t, uint16_t> m_handoverEndOkTrace;
    /**
     * The `RecvMeasurementReport` trace source. Fired when measurement report is
     * received. Exporting IMSI, cell ID, and RNTI.
     */
    TracedCallback<uint64_t, uint16_t, uint16_t, NrRrcSap::MeasurementReport>
        m_recvMeasurementReportTrace;
    /**
     * The `NotifyConnectionRelease` trace source. Fired when an UE leaves the gNB.
     * Exporting IMSI, cell ID, RNTI.
     *
     */
    TracedCallback<uint64_t, uint16_t, uint16_t> m_connectionReleaseTrace;
    /**
     * The 'TimerExpiry' Trace source. Fired when any of the RRC timers maintained
     * at gNB expires. Exporting IMSI, cell ID, and RNTI and name of timer
     * which expired.
     */
    TracedCallback<uint64_t, uint16_t, uint16_t, std::string> m_rrcTimeoutTrace;
    /**
     * The 'HandoverFailureNoPreamble' Trace source. Fired upon handover failure due to
     * non-allocation of non-contention based preamble at gNB for UE to handover due to max count
     * reached
     *
     */
    TracedCallback<uint64_t, uint16_t, uint16_t> m_handoverFailureNoPreambleTrace;
    /**
     * The 'HandoverFailureMaxRach' Trace source. Fired upon handover failure due to max RACH
     * attempts from UE to target gNB
     *
     */
    TracedCallback<uint64_t, uint16_t, uint16_t> m_handoverFailureMaxRachTrace;
    /**
     * The 'HandoverFailureLeaving' Trace source. Fired upon handover failure due to handover
     * leaving timeout at source gNB
     *
     */
    TracedCallback<uint64_t, uint16_t, uint16_t> m_handoverFailureLeavingTrace;
    /**
     * The 'HandoverFailureJoining' Trace source. Fired upon handover failure due to handover
     * joining timeout at target gNB
     *
     */
    TracedCallback<uint64_t, uint16_t, uint16_t> m_handoverFailureJoiningTrace;

    uint16_t m_numberOfComponentCarriers; ///< number of component carriers

    bool m_carriersConfigured; ///< are carriers configured

    std::map<uint8_t, Ptr<BandwidthPartGnb>>
        m_componentCarrierPhyConf; ///< component carrier phy configuration

}; // end of `class NrGnbRrc`

} // namespace ns3

#endif // NR_GNB_RRC_H
