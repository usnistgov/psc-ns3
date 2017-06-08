/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Authors: Nicola Baldo <nbaldo@cttc.es>
 *          Marco Miozzo <mmiozzo@cttc.es>
 *          Manuel Requena <manuel.requena@cttc.es> 
 * Modified by: NIST
 */

#ifndef NIST_LTE_ENB_RRC_H
#define NIST_LTE_ENB_RRC_H

#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/traced-callback.h>
#include <ns3/event-id.h>

#include <ns3/nist-lte-enb-cmac-sap.h>
#include <ns3/nist-lte-mac-sap.h>
#include <ns3/nist-ff-mac-sched-sap.h>
#include <ns3/nist-ff-mac-csched-sap.h>
#include <ns3/nist-lte-pdcp-sap.h>
#include <ns3/nist-epc-x2-sap.h>
#include <ns3/nist-epc-enb-s1-sap.h>
#include <ns3/nist-lte-handover-management-sap.h>
#include <ns3/nist-lte-enb-cphy-sap.h>
#include <ns3/nist-lte-rrc-sap.h>
#include <ns3/nist-lte-anr-sap.h>
#include <ns3/nist-lte-ffr-rrc-sap.h>

#include <map>
#include <set>

namespace ns3 {

class LteRadioNistBearerInfo;
class LteSignalingRadioNistBearerInfo;
class LteDataRadioNistBearerInfo;
class NistLteEnbRrc;
class Packet;

  /**
   * \ingroup lte
   * Manages Sidelink information for this eNodeB
   */
  class LteEnbRrcSl : public Object
  {
    friend class NistLteUeRrc;
    friend class NistUeManager;
    
   public:
    LteEnbRrcSl ();

    virtual ~LteEnbRrcSl (void);

    /**
     * \brief makes a copy of the sidelink configuration
     * \return a copy of the sidelink configuration
     */
    Ptr<LteEnbRrcSl> Copy ();

    
    // inherited from Object
  protected:
    virtual void DoInitialize ();
    virtual void DoDispose ();

  public:
    /**
     *  Register this type.
     *  \return The object TypeId.
     */
    static TypeId GetTypeId (void);

    // /**
    //  * Sets the SIB 18 information
    //  * \param sib18 The content of the system information block to broadcast
    //  */
    // void SetSystemInformationBlockType18 (NistLteRrcSap::SystemInformationBlockType18 sib18);

    /**
     * Gets the SIB 18 information
     * \return The SIB 18
     */
    NistLteRrcSap::SystemInformationBlockType18 GetSystemInformationType18 ();

    void SetSlEnabled (bool status);
    
    bool IsSlEnabled ();

    /**
     * Utility function to preconfigure dedicated pools for UEs
     * \param group The group associated with the pool
     * \param pool The pool information
     */
    void AddPreconfiguredDedicatedPool (uint32_t group, NistLteRrcSap::SlCommTxResourcesSetup pool);

    /**
     * Sets the pool to be used in exceptional cases
     * \param pool The pool information
     */
    void SetCommTxPoolExceptional (NistLteRrcSap::SlCommTxPoolList pool);
    
  private:

    bool IsPoolInList (NistLteRrcSap::SlCommResourcePool pool, NistLteRrcSap::SlCommResourcePool *pools, int nbPool);
      
    //Add information about pools allocated for communication and discovery

    //Maps the UE's RNTI to its associated dedicated resources
    //std::map <uint16_t, NistLteRrcSap::SlCommTxResourcesSetup> m_dedicatedPoolMap;

    //Pre-provisioned dedicated pools using the group L2 address
    std::map <uint32_t, NistLteRrcSap::SlCommTxResourcesSetup> m_preconfigDedicatedPoolMap;

    //Pre-provisioned pool for exceptional cases
    NistLteRrcSap::SlCommTxPoolList m_preconfigCommTxPoolExceptional;
    
    ///NIST: System Information Block Type 18 currently broadcasted
    NistLteRrcSap::SystemInformationBlockType18 m_sib18;

    ///NIST: Indicates if ProSe is enabled
    bool m_slEnabled;

    struct ActivePoolInfo {
      Ptr<SidelinkCommResourcePool> m_pool; //pointer to the pool
      NistLteRrcSap::SlCommTxResourcesSetup m_poolSetup; //pool in a different format
      std::set<uint16_t> m_rntiSet; //list of UEs assigned to the pool
    };

    std::map <uint32_t, LteEnbRrcSl::ActivePoolInfo> m_activePoolMap;
    
  }; //end of 'class LteEnbRrcSl



/**
 * \ingroup lte
 * Manages all the radio bearer information possessed by the ENB RRC for a
 * single UE.
 */
class NistUeManager : public Object
{
  friend class NistLtePdcpSpecificLtePdcpSapUser<NistUeManager>;

public:


  /**
   * The state of the NistUeManager at the eNB RRC
   * 
   */
  enum State
  {
    INITIAL_RANDOM_ACCESS = 0,
    CONNECTION_SETUP,
    CONNECTION_REJECTED,
    CONNECTED_NORMALLY,
    CONNECTION_RECONFIGURATION,
    CONNECTION_REESTABLISHMENT,
    HANDOVER_PREPARATION,
    HANDOVER_JOINING,
    HANDOVER_PATH_SWITCH,
    HANDOVER_LEAVING,
    NUM_STATES
  };

  NistUeManager ();
 
  /** 
   * NistUeManager constructor
   * 
   * \param rrc pointer to the NistLteEnbRrc holding this NistUeManager
   * \param rnti RNTI of the UE
   * \param s initial state of the NistUeManager
   * 
   * \return 
   */
  NistUeManager (Ptr<NistLteEnbRrc> rrc, uint16_t rnti, State s);

  virtual ~NistUeManager (void);

  // inherited from Object
protected:
  virtual void DoInitialize ();
  virtual void DoDispose ();
public: 
  static TypeId GetTypeId (void);

  /** 
   * Set the identifiers of the source eNB for the case where a UE
   * joins the current eNB as part of a handover procedure 
   * 
   * \param sourceCellId 
   * \param sourceX2apId 
   */
  void SetSource (uint16_t sourceCellId, uint16_t sourceX2apId);

  /** 
   * Set the IMSI
   * 
   * \param imsi the IMSI
   */
  void SetImsi (uint64_t imsi);

  /** 
   * Setup a new data radio bearer, including both the configuration
   * within the eNB and the necessary RRC signaling with the UE
   * 
   * \param bearer the QoS characteristics of the bearer
   * \param bearerId the EPS bearer identifier
   * \param gtpTeid S1-bearer GTP tunnel endpoint identifier, see 36.423 9.2.1
   * \param transportLayerAddress  IP Address of the SGW, see 36.423 9.2.1
   * 
   */
  void SetupDataRadioBearer (NistEpsBearer bearer, uint8_t bearerId, uint32_t gtpTeid, Ipv4Address transportLayerAddress);

  /** 
   * Start all configured data radio bearers. It is safe to call this
   * method if any bearer had been already started previously.
   * 
   */
  void RecordDataRadioBearersToBeStarted ();

  /** 
   * Start the data radio bearers that have been previously recorded
   * to be started using RecordDataRadioBearersToBeStarted() 
   * 
   */
  void StartDataRadioBearers ();

  /**
   *
   * Release a given radio bearer
   *
   * \param drbid the data radio bearer id of the bearer to be released
   */
  void ReleaseDataRadioBearer (uint8_t drbid);

  /** 
   * schedule an RRC Connection Reconfiguration procedure with the UE
   * 
   */
  void ScheduleNistRrcConnectionReconfiguration ();

  /** 
   * Start the handover preparation and send the handover request
   * 
   * \param cellId id of the target cell
   */
  void PrepareHandover (uint16_t cellId);

  /** 
   * take the necessary actions in response to the reception of an X2 HANDOVER REQUEST ACK message
   * 
   * \param params 
   */
  void RecvHandoverRequestAck (NistEpcX2SapUser::NistHandoverRequestAckParams params);

  /** 
   * 
   * \return the NistHandoverPreparationInfo sent by the source eNB to the
   * target eNB in the X2-based handover procedure
   */
  NistLteRrcSap::NistRadioResourceConfigDedicated GetRadioResourceConfigForNistHandoverPreparationInfo ();

  /** 
   * 
   * \return retrieve the data that the target eNB needs to send to the source
   * eNB as the Handover Command in the X2-based handover
   * procedure.
   *
   * \note mobility control info is not expected to be filled in
   * (shall be filled in by the caller). 
   */
  NistLteRrcSap::NistRrcConnectionReconfiguration GetNistRrcConnectionReconfigurationForHandover ();

  /** 
   * Send a data packet over the appropriate Data Radio Bearer
   * 
   * \param bid the corresponding EPS Bearer ID
   * \param p the packet
   */
  void SendData (uint8_t bid, Ptr<Packet> p);

  /** 
   * 
   * \return a list of ERAB-to-be-setup items to be put in a X2 HO REQ message
   */
  std::vector<NistEpcX2Sap::NistErabToBeSetupItem> GetErabList ();

  /** 
   * send the UE CONTEXT RELEASE X2 message to the source eNB, thus
   * successfully terminating an X2 handover procedure 
   * 
   */
  void SendUeContextRelease ();

  /** 
   * Take the necessary actions in response to the reception of an X2 HO preparation failure message
   * 
   * \param cellId id of the target cell
   */
  void RecvHandoverPreparationFailure (uint16_t cellId);

  /** 
   * Take the necessary actions in response to the reception of an X2 SN STATUS TRANSFER message
   * 
   * \param params the SN STATUS
   */
  void RecvSnNistStatusTransfer (NistEpcX2SapUser::NistSnNistStatusTransferParams params);
 
  /** 
   * Take the necessary actions in response to the reception of an X2 UE CONTEXT RELEASE message
   * 
   * \param params the SN STATUS
   */
  void RecvUeContextRelease (NistEpcX2SapUser::NistUeContextReleaseParams params); 


  // METHODS FORWARDED FROM ENB RRC SAP ///////////////////////////////////////

  /// Part of the RRC protocol. Implement the NistLteEnbRrcSapProvider::CompleteSetupUe interface.
  void CompleteSetupUe (NistLteEnbRrcSapProvider::CompleteNistSetupUeParameters params);
  /// Part of the RRC protocol. Implement the NistLteEnbRrcSapProvider::RecvNistRrcConnectionRequest interface.
  void RecvNistRrcConnectionRequest (NistLteRrcSap::NistRrcConnectionRequest msg);
  /// Part of the RRC protocol. Implement the NistLteEnbRrcSapProvider::RecvNistRrcConnectionSetupCompleted interface.
  void RecvNistRrcConnectionSetupCompleted (NistLteRrcSap::NistRrcConnectionSetupCompleted msg);
  /// Part of the RRC protocol. Implement the NistLteEnbRrcSapProvider::RecvNistRrcConnectionReconfigurationCompleted interface.
  void RecvNistRrcConnectionReconfigurationCompleted (NistLteRrcSap::NistRrcConnectionReconfigurationCompleted msg);
  /// Part of the RRC protocol. Implement the NistLteEnbRrcSapProvider::RecvNistRrcConnectionReestablishmentRequest interface.
  void RecvNistRrcConnectionReestablishmentRequest (NistLteRrcSap::NistRrcConnectionReestablishmentRequest msg);
  /// Part of the RRC protocol. Implement the NistLteEnbRrcSapProvider::RecvNistRrcConnectionReestablishmentComplete interface.
  void RecvNistRrcConnectionReestablishmentComplete (NistLteRrcSap::NistRrcConnectionReestablishmentComplete msg);
  /// Part of the RRC protocol. Implement the NistLteEnbRrcSapProvider::RecvNistMeasurementReport interface.
  void RecvNistMeasurementReport (NistLteRrcSap::NistMeasurementReport msg);
  /// Part of the RRC protocol. Implement the NistLteEnbRrcSapProvider::RecvSidelinkUeInformation interface.
  void RecvSidelinkUeInformation (NistLteRrcSap::SidelinkUeInformation msg);

  // METHODS FORWARDED FROM ENB CMAC SAP //////////////////////////////////////

  void CmacNistUeConfigUpdateInd (NistLteEnbCmacSapUser::NistUeConfig cmacParams);

  // METHODS FORWARDED FROM ENB PDCP SAP //////////////////////////////////////

  void DoReceivePdcpSdu (NistLtePdcpSapUser::NistReceivePdcpSduParameters params);

  /** 
   * 
   * \return the RNTI, i.e., an UE identifier that is unique within
   * the cell
   */
  uint16_t GetRnti (void) const;

  /** 
   *
   * \return the IMSI, i.e., a globally unique UE identifier
   */
  uint64_t GetImsi (void) const;

  /** 
   * 
   * \return the SRS Configuration Index
   */
  uint16_t GetSrsConfigurationIndex (void) const;

  /** 
   * Set the SRS configuration index and do the necessary reconfiguration
   * 
   * \param srsConfIndex 
   */
  void SetSrsConfigurationIndex (uint16_t srsConfIndex);

  /** 
   * 
   * \return the current state
   */
  State GetState () const;

  /**
   * Configure NistPdschConfigDedicated (i.e. P_A value) for UE and start NistRrcConnectionReconfiguration
   * to inform UE about new NistPdschConfigDedicated
   *
   * \param pdschConfigDedicated new pdschConfigDedicated (i.e. P_A value) to be set
   */
  void SetNistPdschConfigDedicated (NistLteRrcSap::NistPdschConfigDedicated pdschConfigDedicated);

  /**
   * TracedCallback signature for state transition events.
   *
   * \param [in] imsi
   * \param [in] cellId
   * \param [in] rnti
   * \param [in] oldState
   * \param [in] newState
   */
  typedef void (* StateTracedCallback)
    (const uint64_t imsi, const uint16_t cellId, const uint16_t rnti,
     const State oldState, const State newState);

private:

  /** 
   * Add a new LteDataRadioNistBearerInfo structure to the NistUeManager
   * 
   * \param radioNistBearerInfo 
   * 
   * \return the id of the newly added data radio bearer structure
   */
  uint8_t AddDataRadioNistBearerInfo (Ptr<LteDataRadioNistBearerInfo> radioNistBearerInfo);

  /** 
   * \param drbid the Data Radio Bearer id
   * 
   * \return the corresponding LteDataRadioNistBearerInfo
   */
  Ptr<LteDataRadioNistBearerInfo> GetDataRadioNistBearerInfo (uint8_t drbid);

  /** 
   * remove the LteDataRadioNistBearerInfo corresponding to a bearer being released
   *
   * \param drbid the Data Radio Bearer id
   */
  void RemoveDataRadioNistBearerInfo (uint8_t drbid);

  /** 
   * 
   * \return an NistRrcConnectionReconfiguration struct built based on the
   * current configuration
   */
  NistLteRrcSap::NistRrcConnectionReconfiguration BuildNistRrcConnectionReconfiguration ();

  /** 
   * 
   * \return a NistRadioResourceConfigDedicated struct built based on the
   * current configuration
   */
  NistLteRrcSap::NistRadioResourceConfigDedicated BuildNistRadioResourceConfigDedicated ();

  /** 
   * 
   * \return a newly allocated identifier for a new RRC transaction
   */
  uint8_t GetNewRrcTransactionIdentifier ();

  /** 
   * \param lcid a Logical Channel Identifier
   * 
   * \return the corresponding Data Radio Bearer Id
   */
  uint8_t Lcid2Drbid (uint8_t lcid);

  /** 
   * \param drbid a Data Radio Bearer Id
   * 
   * \return the corresponding  Logical Channel Identifier
   */
  uint8_t Drbid2Lcid (uint8_t drbid);

  /** 
   * \param lcid a  Logical Channel Identifier
   * 
   * \return the corresponding EPS Bearer Identifier
   */
  uint8_t Lcid2Bid (uint8_t lcid);

  /** 
   * \param bid  an EPS Bearer Identifier
   * 
   * \return the corresponding Logical Channel Identifier
   */
  uint8_t Bid2Lcid (uint8_t bid);

  /** 
   * \param drbid Data Radio Bearer Id
   * 
   * \return the corresponding EPS Bearer Identifier
   */
  uint8_t Drbid2Bid (uint8_t drbid);

  /** 
   * \param bid an EPS Bearer Identifier
   * 
   * \return the corresponding Data Radio Bearer Id
   */
  uint8_t Bid2Drbid (uint8_t bid);

  /** 
   * Switch the NistUeManager to the given state
   * 
   * \param s the given state
   */
  void SwitchToState (State s);

  uint8_t m_lastAllocatedDrbid;

  /**
   * The `DataRadioBearerMap` attribute. List of UE DataRadioNistBearerInfo by
   * DRBID.
   */
  std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> > m_drbMap;

  /**
   * The `Srb0` attribute. SignalingRadioNistBearerInfo for SRB0.
   */
  Ptr<LteSignalingRadioNistBearerInfo> m_srb0;
  /**
   * The `Srb1` attribute. SignalingRadioNistBearerInfo for SRB1.
   */
  Ptr<LteSignalingRadioNistBearerInfo> m_srb1;

  /**
   * The `C-RNTI` attribute. Cell Radio Network Temporary Identifier.
   */
  uint16_t m_rnti;
  /**
   * International Mobile Subscriber Identity assigned to this UE. A globally
   * unique UE identifier.
   */
  uint64_t m_imsi;
  ///
  uint8_t m_lastRrcTransactionIdentifier;
  ///
  NistLteRrcSap::NistPhysicalConfigDedicated m_physicalConfigDedicated;
  /// Pointer to the parent eNodeB RRC.
  Ptr<NistLteEnbRrc> m_rrc;
  /// The current NistUeManager state.
  State m_state;
  ///
  NistLtePdcpSapUser* m_drbPdcpSapUser;
  ///
  bool m_pendingNistRrcConnectionReconfiguration;

  /**
   * The `StateTransition` trace source. Fired upon every UE state transition
   * seen by the NistUeManager at the eNB RRC. Exporting IMSI, cell ID, RNTI, old
   * state, and new state.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t, State, State> m_stateTransitionTrace;

  uint16_t m_sourceX2apId;
  uint16_t m_sourceCellId;
  uint16_t m_targetCellId;
  std::list<uint8_t> m_drbsToBeStarted;
  bool m_needPhyMacConfiguration;

  /**
   * Time limit before a _connection request timeout_ occurs. Set after a new
   * UE context is added after a successful Random Access. Calling
   * NistLteEnbRrc::ConnectionRequestTimeout() when it expires. Cancelled when RRC
   * CONNECTION REQUEST is received.
   */
  EventId m_connectionRequestTimeout;
  /**
   * Time limit before a _connection setup timeout_ occurs. Set after an RRC
   * CONNECTION SETUP is sent. Calling NistLteEnbRrc::ConnectionSetupTimeout() when
   * it expires. Cancelled when RRC CONNECTION SETUP COMPLETE is received.
   */
  EventId m_connectionSetupTimeout;
  /**
   * The delay before a _connection rejected timeout_ occurs. Set after an RRC
   * CONNECTION REJECT is sent. Calling NistLteEnbRrc::ConnectionRejectedTimeout()
   * when it expires.
   */
  EventId m_connectionRejectedTimeout;
  /**
   * Time limit before a _handover joining timeout_ occurs. Set after a new UE
   * context is added after receiving a handover request. Calling
   * NistLteEnbRrc::HandoverJoiningTimeout() when it expires. Cancelled when
   * RRC CONNECTION RECONFIGURATION COMPLETE is received.
   */
  EventId m_handoverJoiningTimeout;
  /**
   * Time limit before a _handover leaving timeout_ occurs. Set after a
   * handover command is sent. Calling NistLteEnbRrc::HandoverLeavingTimeout()
   * when it expires. Cancelled when RRC CONNECTION RE-ESTABLISHMENT or X2
   * UE CONTEXT RELEASE is received.
   */
  EventId m_handoverLeavingTimeout;

  /**
   * List of destinations for sidelink communications
   */
  std::vector<uint32_t> m_slDestinations;
  bool m_slPoolChanged;

}; // end of `class NistUeManager`



/**
 * \ingroup lte
 * 
 * The LTE Radio Resource Control entity at the eNB
 */
class NistLteEnbRrc : public Object
{

  friend class NistEnbRrcMemberLteEnbCmacSapUser;
  friend class MemberNistLteHandoverManagementSapUser<NistLteEnbRrc>;
  friend class MemberNistLteAnrSapUser<NistLteEnbRrc>;
  friend class MemberNistLteFfrRrcSapUser<NistLteEnbRrc>;
  friend class NistMemberLteEnbRrcSapProvider<NistLteEnbRrc>;
  friend class MemberNistEpcEnbS1SapUser<NistLteEnbRrc>;
  friend class NistEpcX2SpecificNistEpcX2SapUser<NistLteEnbRrc>;
  friend class NistUeManager;

public:
  /**
   * create an RRC instance for use within an eNB
   *
   */
  NistLteEnbRrc ();

  /**
   * Destructor
   */
  virtual ~NistLteEnbRrc ();


  // inherited from Object
protected:
  virtual void DoDispose (void);
public:
  static TypeId GetTypeId (void);


  /**
   * Set the X2 SAP this RRC should interact with
   * \param s the X2 SAP Provider to be used by this RRC entity
   */
  void SetNistEpcX2SapProvider (NistEpcX2SapProvider* s);

  /** 
   * Get the X2 SAP offered by this RRC
   * \return s the X2 SAP User interface offered to the X2 entity by this RRC entity
   */
  NistEpcX2SapUser* GetNistEpcX2SapUser ();


  /**
   * set the CMAC SAP this RRC should interact with
   *
   * \param s the CMAC SAP Provider to be used by this RRC
   */
  void SetNistLteEnbCmacSapProvider (NistLteEnbCmacSapProvider * s);

  /** 
   * Get the CMAC SAP offered by this RRC
   * \return s the CMAC SAP User interface offered to the MAC by this RRC
   */
  NistLteEnbCmacSapUser* GetNistLteEnbCmacSapUser ();


  /**
   * set the Handover Management SAP this RRC should interact with
   *
   * \param s the Handover Management SAP Provider to be used by this RRC
   */
  void SetNistLteHandoverManagementSapProvider (NistLteHandoverManagementSapProvider * s);

  /**
   * Get the Handover Management SAP offered by this RRC
   * \return s the Handover Management SAP User interface offered to the
   *           handover algorithm by this RRC
   */
  NistLteHandoverManagementSapUser* GetNistLteHandoverManagementSapUser ();


  /**
   * set the ANR SAP this RRC should interact with
   *
   * \param s the ANR SAP Provider to be used by this RRC
   */
  void SetNistLteAnrSapProvider (NistLteAnrSapProvider * s);

  /**
   * Get the ANR SAP offered by this RRC
   * \return s the ANR SAP User interface offered to the ANR instance by this
   *           RRC
   */
  NistLteAnrSapUser* GetNistLteAnrSapUser ();



  /**
   * set the FFR SAP this RRC should interact with
   *
   * \param s the FFR SAP Provider to be used by this RRC
   */
  void SetNistLteFfrRrcSapProvider (NistLteFfrRrcSapProvider * s);

  /**
   * Get the FFR SAP offered by this RRC
   * \return s the FFR SAP User interface offered to the ANR instance by this
   *           RRC
   */
  NistLteFfrRrcSapUser* GetNistLteFfrRrcSapUser ();

  /**
   * set the RRC SAP this RRC should interact with
   *
   * \param s the RRC SAP User to be used by this RRC
   */
  void SetNistLteEnbRrcSapUser (NistLteEnbRrcSapUser * s);

  /**
   *
   *
   * \return s the RRC SAP Provider interface offered to the MAC by this RRC
   */
  NistLteEnbRrcSapProvider* GetNistLteEnbRrcSapProvider ();

  /**
   * set the MAC SAP provider. The eNB RRC does not use this
   * directly, but it needs to provide it to newly created RLC instances.
   *
   * \param s the MAC SAP provider that will be used by all
   * newly created RLC instances
   */
  void SetNistLteMacSapProvider (NistLteMacSapProvider* s);


  /** 
   * Set the S1 SAP Provider
   * 
   * \param s the S1 SAP Provider
   */
  void SetS1SapProvider (NistEpcEnbS1SapProvider * s);

  /** 
   * 
   * \return the S1 SAP user
   */
  NistEpcEnbS1SapUser* GetS1SapUser ();


  /**
   * set the CPHY SAP this RRC should use to interact with the PHY
   *
   * \param s the CPHY SAP Provider
   */
  void SetNistLteEnbCphySapProvider (NistLteEnbCphySapProvider * s);

  /**
   *
   *
   * \return s the CPHY SAP User interface offered to the PHY by this RRC
   */
  NistLteEnbCphySapUser* GetNistLteEnbCphySapUser ();

  /** 
   * 
   * 
   * \param rnti the identifier of an UE
   * 
   * \return true if the corresponding NistUeManager instance exists
   */
  bool HasNistUeManager (uint16_t rnti) const;

  /**
   *
   *
   * \param rnti the identifier of an UE
   *
   * \return the corresponding NistUeManager instance
   */
  Ptr<NistUeManager> GetNistUeManager (uint16_t rnti);

  /**
   * \brief Add a new UE measurement reporting configuration
   * \param config the new reporting configuration
   * \return the measurement ID (measId) referring to the newly added
   *         reporting configuration
   *
   * Assuming intra-frequency environment, the new measurement reporting
   * configuration will be automatically associated to the only measurement
   * object (i.e., a new measurement identity will be automatically created).
   *
   * Can only be called before the start of simulation.
   */
  uint8_t AddUeMeasReportConfig (NistLteRrcSap::NistReportConfigEutra config);

  /**
   * \brief Configure cell-specific parameters.
   * \param ulBandwidth the uplink bandwidth in number of RB
   * \param dlBandwidth the downlink bandwidth in number of RB
   * \param ulEarfcn the UL EARFCN
   * \param dlEarfcn the DL EARFCN
   * \param cellId the ID of the cell
   *
   * Configure cell-specific parameters and propagate them to lower layers.
   * The parameters include bandwidth, EARFCN (E-UTRA Absolute Radio Frequency
   * Channel Number), and cell ID.
   *
   * In addition to parameter configuration, this function also performs several
   * other tasks:
   *  - Initializing UE measurement (i.e. measurement object and quantity
   *    configuration), which is expected to be further configured through
   *    `NistLteEnbRrc::AddUeMeasReportConfig`;
   *  - Enabling MIB (Master Information Block) broadcast transmission
   *  - Enabling SIB (System Information Block) broadcast transmission
   *
   * Typically runs when the eNodeB NetDevice is installed, for instance by
   * `NistLteHelper::InstallEnbDevice` (i.e. before the simulation starts).
   *
   * \warning Raises an error when executed more than once.
   */
  void ConfigureCell (uint8_t ulBandwidth,
                      uint8_t dlBandwidth,
                      uint16_t ulEarfcn, 
                      uint16_t dlEarfcn,
                      uint16_t cellId);

  /** 
   * set the cell id of this eNB
   * 
   * \param m_cellId 
   */
  void SetCellId (uint16_t m_cellId);

  /** 
   * Enqueue an IP data packet on the proper bearer for downlink
   * transmission. Normally expected to be called by the NetDevice
   * forwarding a packet coming from the NistEpcEnbApplication 
   * 
   * \param p the packet
   * 
   * \return true if successful, false if an error occurred
   */
  bool SendData (Ptr<Packet> p);

  /** 
   * set the callback used to forward data packets up the stack
   * 
   * \param cb 
   */
  void SetForwardUpCallback (Callback <void, Ptr<Packet> > cb);

  /** 
   * Method triggered when a UE is expected to request for connection but does
   * not do so in a reasonable time. The method will remove the UE context.
   * 
   * \param rnti the T-C-RNTI whose timeout expired
   */
  void ConnectionRequestTimeout (uint16_t rnti);

  /** 
   * Method triggered when a UE is expected to complete a connection setup
   * procedure but does not do so in a reasonable time. The method will remove
   * the UE context.
   *
   * \param rnti the T-C-RNTI whose timeout expired
   */
  void ConnectionSetupTimeout (uint16_t rnti);

  /**
   * Method triggered a while after sending RRC Connection Rejected. The method
   * will remove the UE context.
   * 
   * \param rnti the T-C-RNTI whose timeout expired
   */
  void ConnectionRejectedTimeout (uint16_t rnti);

  /** 
   * Method triggered when a UE is expected to join the cell for a handover 
   * but does not do so in a reasonable time. The method will remove the UE
   * context.
   * 
   * \param rnti the C-RNTI whose timeout expired
   */
  void HandoverJoiningTimeout (uint16_t rnti);

  /** 
   * Method triggered when a UE is expected to leave a cell for a handover
   * but no feedback is received in a reasonable time. The method will remove
   * the UE context.
   * 
   * \param rnti the C-RNTI whose timeout expired
   */
  void HandoverLeavingTimeout (uint16_t rnti);

  /** 
   * Send a HandoverRequest through the X2 SAP interface. This method will
   * trigger a handover which is started by the RRC by sending a handover
   * request to the target eNB over the X2 interface
   *
   * \param rnti the ID of the UE to be handed over
   * \param cellId the ID of the target eNB
   */
  void SendHandoverRequest (uint16_t rnti, uint16_t cellId);

  /**
   *  \brief This function acts as an interface to trigger Release indication messages towards eNB and EPC
   *  \param imsi the IMSI
   *  \param rnti the RNTI
   *  \param bearerId Bearer Identity which is to be de-activated
   */
  void DoSendReleaseDataRadioBearer (uint64_t imsi, uint16_t rnti, uint8_t bearerId);

  /**
   * Identifies how EPS Bearer parameters are mapped to different RLC types
   * 
   */
  enum LteNistEpsBearerToRlcMapping_t {RLC_SM_ALWAYS = 1,
                                   RLC_UM_ALWAYS = 2,
                                   RLC_AM_ALWAYS = 3,
                                   PER_BASED = 4};

  /**
   * TracedCallback signature for new Ue Context events.
   *
   * \param [in] cellId
   * \param [in] rnti
   */
  typedef void (* NewUeContextTracedCallback)
    (const uint16_t cellId, const uint16_t rnti);

  /**
   * TracedCallback signature for connection and handover end events.
   *
   * \param [in] imsi
   * \param [in] cellId
   * \param [in] rnti
   */
  typedef void (* ConnectionHandoverTracedCallback)
    (const uint64_t imsi, const uint16_t cellId, const uint16_t rnti);
  
  /**
   * TracedCallback signature for handover start events.
   *
   * \param [in] imsi
   * \param [in] cellId
   * \param [in] rnti
   * \param [in] targetCid
   */
  typedef void (* HandoverStartTracedCallback)
    (const uint64_t imsi, const uint16_t cellId, const uint16_t rnti,
     const uint16_t targetCid);

  /**
   * TracedCallback signature for receive measurement report events.
   *
   * \param [in] imsi
   * \param [in] cellId
   * \param [in] rnti
   * \param [in] report
   */
  typedef void (* ReceiveReportTracedCallback)
    (const uint64_t imsi, const uint16_t cellId, const uint16_t rnti,
     const NistLteRrcSap::NistMeasurementReport report);
  
private:


  // RRC SAP methods

  /// Part of the RRC protocol. Forwarding NistLteEnbRrcSapProvider::CompleteSetupUe interface to NistUeManager::CompleteSetupUe
  void DoCompleteSetupUe (uint16_t rnti, NistLteEnbRrcSapProvider::CompleteNistSetupUeParameters params);
  /// Part of the RRC protocol. Forwarding NistLteEnbRrcSapProvider::RecvNistRrcConnectionRequest interface to NistUeManager::RecvNistRrcConnectionRequest
  void DoRecvNistRrcConnectionRequest (uint16_t rnti, NistLteRrcSap::NistRrcConnectionRequest msg);
  /// Part of the RRC protocol. Forwarding NistLteEnbRrcSapProvider::RecvNistRrcConnectionSetupCompleted interface to NistUeManager::RecvNistRrcConnectionSetupCompleted
  void DoRecvNistRrcConnectionSetupCompleted (uint16_t rnti, NistLteRrcSap::NistRrcConnectionSetupCompleted msg);
  /// Part of the RRC protocol. Forwarding NistLteEnbRrcSapProvider::RecvNistRrcConnectionReconfigurationCompleted interface to NistUeManager::RecvNistRrcConnectionReconfigurationCompleted
  void DoRecvNistRrcConnectionReconfigurationCompleted (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReconfigurationCompleted msg);
  /// Part of the RRC protocol. Forwarding NistLteEnbRrcSapProvider::RecvNistRrcConnectionReestablishmentRequest interface to NistUeManager::RecvNistRrcConnectionReestablishmentRequest
  void DoRecvNistRrcConnectionReestablishmentRequest (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReestablishmentRequest msg);
  /// Part of the RRC protocol. Forwarding NistLteEnbRrcSapProvider::RecvNistRrcConnectionReestablishmentComplete interface to NistUeManager::RecvNistRrcConnectionReestablishmentComplete
  void DoRecvNistRrcConnectionReestablishmentComplete (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReestablishmentComplete msg);
  /// Part of the RRC protocol. Forwarding NistLteEnbRrcSapProvider::RecvNistMeasurementReport interface to NistUeManager::RecvNistMeasurementReport
  void DoRecvNistMeasurementReport (uint16_t rnti, NistLteRrcSap::NistMeasurementReport msg);
  /// Part of the RRC protocol. Forwarding NistLteEnbRrcSapProvider::RecvSidelinkUeInformation interface to NistUeManager::RecvSidelinkUeInformation.
  void DoRecvSidelinkUeInformation (uint16_t rnti, NistLteRrcSap::SidelinkUeInformation msg);


  // S1 SAP methods

  void DoDataRadioBearerSetupRequest (NistEpcEnbS1SapUser::NistDataRadioBearerSetupRequestParameters params);
  void DoPathSwitchRequestAcknowledge (NistEpcEnbS1SapUser::NistPathSwitchRequestAcknowledgeParameters params);

  // X2 SAP methods

  void DoRecvHandoverRequest (NistEpcX2SapUser::NistHandoverRequestParams params);
  void DoRecvHandoverRequestAck (NistEpcX2SapUser::NistHandoverRequestAckParams params);
  void DoRecvHandoverPreparationFailure (NistEpcX2SapUser::NistHandoverPreparationFailureParams params);
  void DoRecvSnNistStatusTransfer (NistEpcX2SapUser::NistSnNistStatusTransferParams params);
  void DoRecvUeContextRelease (NistEpcX2SapUser::NistUeContextReleaseParams params);
  void DoRecvLoadInformation (NistEpcX2SapUser::NistLoadInformationParams params);
  void DoRecvResourceNistStatusUpdate (NistEpcX2SapUser::NistResourceNistStatusUpdateParams params);
  void DoRecvUeData (NistEpcX2SapUser::NistUeDataParams params);

  // CMAC SAP methods

  uint16_t DoAllocateTemporaryCellRnti ();
  void DoNotifyLcConfigResult (uint16_t rnti, uint8_t lcid, bool success);
  void DoRrcConfigurationUpdateInd (NistLteEnbCmacSapUser::NistUeConfig params);

  // Handover Management SAP methods

  uint8_t DoAddUeMeasReportConfigForHandover (NistLteRrcSap::NistReportConfigEutra reportConfig);
  void DoTriggerHandover (uint16_t rnti, uint16_t targetCellId);

  // ANR SAP methods

  uint8_t DoAddUeMeasReportConfigForAnr (NistLteRrcSap::NistReportConfigEutra reportConfig);

  // FFR RRC SAP methods
  uint8_t DoAddUeMeasReportConfigForFfr (NistLteRrcSap::NistReportConfigEutra reportConfig);
  void DoSetNistPdschConfigDedicated (uint16_t rnti, NistLteRrcSap::NistPdschConfigDedicated pa);
  void DoSendLoadInformation (NistEpcX2Sap::NistLoadInformationParams params);

  // Internal methods

  /**
   * Allocate a new RNTI for a new UE. This is done in the following cases:
   *   * T-C-RNTI allocation upon contention-based MAC Random Access procedure
   *   * target cell RNTI allocation upon handover
   *
   * \param state the initial state of the NistUeManager
   *
   * \return the newly allocated RNTI
   */
  uint16_t AddUe (NistUeManager::State state);

  /**
   * remove a UE from the cell
   *
   * \param rnti the C-RNTI identiftying the user
   */
  void RemoveUe (uint16_t rnti);


  /** 
   * 
   * \param bearer the specification of an EPS bearer
   * 
   * \return the type of RLC that is to be created for the given EPS bearer
   */
  TypeId GetRlcType (NistEpsBearer bearer);



public:

  /** 
   * Add a neighbour with an X2 interface
   *
   * \param cellId neighbouring cell id
   */
  void AddX2Neighbour (uint16_t cellId);

  /** 
   * 
   * \param p the SRS periodicity in num TTIs
   */
  void SetSrsPeriodicity (uint32_t p);

  /** 
   *
   * \return the current SRS periodicity
   */
  uint32_t GetSrsPeriodicity () const;

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
  void SetCsgId (uint32_t csgId, bool csgIndication);

private:

  /** 
   * Allocate a new SRS configuration index for a new UE. 
   *
   * \note this method can have the side effect of updating the SRS
   * configuration index of all UEs
   * 
   * \return the newly allocated SRS configuration index
   */
  uint16_t GetNewSrsConfigurationIndex (void);

  /** 
   * remove a previously allocated SRS configuration index
   *
   * \note this method can have the side effect of updating the SRS
   * configuration index of all UEs
   * 
   * \param srcCi the indext to be removed
   */
  void RemoveSrsConfigurationIndex (uint16_t srcCi);

 

  /** 
   * 
   * \param bearer the characteristics of the bearer
   * 
   * \return the Logical Channel Group in a bearer with these
   * characteristics is put. Used for MAC Buffer NistStatus Reporting purposes. 
   */
  uint8_t GetLogicalChannelGroup (NistEpsBearer bearer);

  /** 
   * 
   * \param bearer the characteristics of the bearer
   * 
   * \return the priority level of a bearer with these
   * characteristics is put. Used for the part of UL MAC Scheduling
   * carried out by the UE
   */
  uint8_t GetLogicalChannelPriority (NistEpsBearer bearer);


  /** 
   * method used to periodically send System Information
   * 
   */
  void SendNistSystemInformation ();


  Callback <void, Ptr<Packet> > m_forwardUpCallback;

  /// Interface to receive messages from neighbour eNodeB over the X2 interface.
  NistEpcX2SapUser* m_x2SapUser;
  /// Interface to send messages to neighbour eNodeB over the X2 interface.
  NistEpcX2SapProvider* m_x2SapProvider;

  /// Receive API calls from the eNodeB MAC instance.
  NistLteEnbCmacSapUser* m_cmacSapUser;
  /// Interface to the eNodeB MAC instance.
  NistLteEnbCmacSapProvider* m_cmacSapProvider;

  /// Receive API calls from the handover algorithm instance.
  NistLteHandoverManagementSapUser* m_handoverManagementSapUser;
  /// Interface to the handover algorithm instance.
  NistLteHandoverManagementSapProvider* m_handoverManagementSapProvider;

  /// Receive API calls from the ANR instance.
  NistLteAnrSapUser* m_anrSapUser;
  /// Interface to the ANR instance.
  NistLteAnrSapProvider* m_anrSapProvider;

  /// Receive API calls from the FFR algorithm instance.
  NistLteFfrRrcSapUser* m_ffrRrcSapUser;
  /// Interface to the FFR algorithm instance.
  NistLteFfrRrcSapProvider* m_ffrRrcSapProvider;

  /// Interface to send messages to UE over the RRC protocol.
  NistLteEnbRrcSapUser* m_rrcSapUser;
  /// Interface to receive messages from UE over the RRC protocol.
  NistLteEnbRrcSapProvider* m_rrcSapProvider;

  /// Interface to the eNodeB MAC instance, to be used by RLC instances.
  NistLteMacSapProvider* m_macSapProvider;

  /// Interface to send messages to core network over the S1 protocol.
  NistEpcEnbS1SapProvider* m_s1SapProvider;
  /// Interface to receive messages from core network over the S1 protocol.
  NistEpcEnbS1SapUser* m_s1SapUser;

  /// Receive API calls from the eNodeB PHY instance.
  NistLteEnbCphySapUser* m_cphySapUser;
  /// Interface to the eNodeB PHY instance.
  NistLteEnbCphySapProvider* m_cphySapProvider;

  /// True if ConfigureCell() has been completed.
  bool m_configured;
  /// Cell identifier. Must be unique across the simulation.
  uint16_t m_cellId;
  /// Downlink E-UTRA Absolute Radio Frequency Channel Number.
  uint16_t m_dlEarfcn;
  /// Uplink E-UTRA Absolute Radio Frequency Channel Number.
  uint16_t m_ulEarfcn;
  /// Downlink transmission bandwidth configuration in number of Resource Blocks.
  uint16_t m_dlBandwidth;
  /// Uplink transmission bandwidth configuration in number of Resource Blocks.
  uint16_t m_ulBandwidth;
  ///
  uint16_t m_lastAllocatedRnti;

  /// The System Information Block Type 1 that is currently broadcasted over BCH.
  NistLteRrcSap::NistSystemInformationBlockType1 m_sib1;

  /**
   * The `UeMap` attribute. List of NistUeManager by C-RNTI.
   */
  std::map<uint16_t, Ptr<NistUeManager> > m_ueMap;

  /**
   * List of measurement configuration which are active in every UE attached to
   * this eNodeB instance.
   */
  NistLteRrcSap::NistMeasConfig m_ueNistMeasConfig;

  /// List of measurement identities which are intended for handover purpose.
  std::set<uint8_t> m_handoverMeasIds;
  /// List of measurement identities which are intended for ANR purpose.
  std::set<uint8_t> m_anrMeasIds;
  /// List of measurement identities which are intended for FFR purpose.
  std::set<uint8_t> m_ffrMeasIds;

  struct NistX2uTeidInfo
  {
    uint16_t rnti;
    uint8_t drbid;
  };

  //       TEID      RNTI, DRBID
  std::map<uint32_t, NistX2uTeidInfo> m_x2uTeidInfoMap;

  /**
   * The `DefaultTransmissionMode` attribute. The default UEs' transmission
   * mode (0: SISO).
   */
  uint8_t m_defaultTransmissionMode;
  /**
   * The `NistEpsBearerToRlcMapping` attribute. Specify which type of RLC will be
   * used for each type of EPS bearer.
   */
  enum LteNistEpsBearerToRlcMapping_t m_epsBearerToRlcMapping;
  /**
   * The `NistSystemInformationPeriodicity` attribute. The interval for sending
   * system information.
   */
  Time m_systemInformationPeriodicity;
  /**
   * The `SrsPeriodicity` attribute. The SRS periodicity in milliseconds.
   */
  uint16_t m_srsCurrentPeriodicityId;
  std::set<uint16_t> m_ueSrsConfigurationIndexSet;
  uint16_t m_lastAllocatedConfigurationIndex;
  bool m_reconfigureUes;

  /**
   * Sidelink configuration
   */
  Ptr<LteEnbRrcSl> m_sidelinkConfiguration;
  
  /**
   * The `QRxLevMin` attribute. One of information transmitted within the SIB1
   * message, indicating the required minimum RSRP level that any UE must
   * receive from this cell before it is allowed to camp to this cell.
   */
  int8_t m_qRxLevMin;
  /**
   * The `AdmitHandoverRequest` attribute. Whether to admit an X2 handover
   * request from another eNB.
   */
  bool m_admitHandoverRequest;
  /**
   * The `AdmitNistRrcConnectionRequest` attribute. Whether to admit a connection
   * request from a UE.
   */
  bool m_admitNistRrcConnectionRequest;
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
   * HO REQ ACK by source eNB, transmission of the Handover Command,
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
   * The `RecvNistMeasurementReport` trace source. Fired when measurement report is
   * received. Exporting IMSI, cell ID, and RNTI.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t, NistLteRrcSap::NistMeasurementReport> m_recvNistMeasurementReportTrace;

}; // end of `class NistLteEnbRrc`


} // namespace ns3

#endif // NIST_LTE_ENB_RRC_H
