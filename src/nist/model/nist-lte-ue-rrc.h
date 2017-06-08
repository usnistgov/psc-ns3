/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 *         Budiarto Herman <budiarto.herman@magister.fi>
 */

#ifndef NIST_LTE_UE_RRC_H
#define NIST_LTE_UE_RRC_H

#include <ns3/object.h>
#include <ns3/packet.h>
#include <ns3/nist-lte-ue-cmac-sap.h>
#include <ns3/nist-lte-pdcp-sap.h>
#include <ns3/nist-lte-as-sap.h>
#include <ns3/nist-lte-ue-cphy-sap.h>
#include <ns3/nist-lte-rrc-sap.h>
#include <ns3/traced-callback.h>
#include "ns3/random-variable-stream.h"
#include <map>
#include <set>

namespace ns3 {


/**
 * \brief Artificial delay of UE measurements procedure.
 *
 * i.e. the period between the time layer-1-filtered measurements from PHY
 * layer is received and the earliest time the actual measurement report
 * submission to the serving cell is invoked.
 *
 * This delay exists because of racing condition between several UE measurements
 * functions which happen to be scheduled at the same time. The delay ensures
 * that:
 *  - measurements (e.g., layer-3 filtering) are always performed before
 *    reporting, thus the latter always use the latest measured RSRP and RSRQ;
 *    and
 *  - time-to-trigger check is always performed before the reporting, so there
 *    would still be chance for it to cancel the reporting if necessary.
 */
static const Time UE_MEASUREMENT_REPORT_DELAY = MicroSeconds (1);


class NistLteRlc;
class NistLteMacSapProvider;
class NistLteUeCmacSapUser;
class NistLteUeCmacSapProvider;
class LteDataRadioNistBearerInfo;
class LteSignalingRadioNistBearerInfo;
class LteSidelinkRadioBearerInfo;

  class LteUeRrcSl: public Object
  {
    friend class NistLteUeRrc;
    
    // Structure to store Sidelink configuration for a given plmnid
    struct LteSlCellConfiguration {
      uint16_t cellId;
      bool haveSib18; //may need to be replaced with another struct containing frame information
      NistLteRrcSap::SystemInformationBlockType18 sib18;
      //Add sib19 and others as needed
      bool haveSib19;
      NistLteRrcSap::SystemInformationBlockType19 sib19;
    };

    
public:
    LteUeRrcSl ();

    virtual ~LteUeRrcSl (void);

    /**
     * \brief makes a copy of the sidelink configuration
     * \return a copy of the sidelink configuration
     */
    Ptr<LteUeRrcSl> Copy ();
    
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

    void SetSlEnabled (bool status);
    
    bool IsSlEnabled ();
    
    void SetDiscEnabled (bool status);
    
    bool IsDiscEnabled ();

    void SetSlPreconfiguration (NistLteRrcSap::SlPreconfiguration preconfiguration);

    NistLteRrcSap::SlPreconfiguration GetSlPreconfiguration ();

    void SetSourceL2Id (uint32_t src);

    /**
     * Set discTxResourceReq
     *
     */
    void SetDiscTxResources (uint8_t nb);

    /**
     * Set discInterFreq
     *
     */
    void SetDiscInterFreq (uint16_t nb);
    
    /**
     * Returns discTxResourceReq
     * \return discTxResourceReq
     */
    uint8_t GetDiscTxResources ();

    /***
     * Retuens discInterFreq
     * \return discInterFreq
     *
     */
    uint16_t GetDiscInterFreq ();
    
  protected:
    /**
     * Indicates if the UE is interested in sidelink transmission
     * \return True if the UE is interested in sidelink transmission
     */
    bool IsTxInterested ();

    /**
     * Indicates if the UE is interested in sidelink transmission
     * \return True if the UE is interested in sidelink transmission
     */    
    bool IsRxInterested ();
    
    /**
     * Indicates if the UE is interested in discovery Monitoring
     * \return True if the UE is interested in discovery Monitoring
     */
    bool IsMonitoringInterested ();
    /**
     * Indicates if the UE is interested in discovery Announcing
     * \return True if the UE is interested in discovery Announcing
     */
    bool IsAnnouncingInterested ();
    
    /**
     * Returns the list of destinations this UE is interested in transmitting
     * \return the list of destinations
     */
    std::list <uint32_t> GetTxDestinations ();

    /**
     * Attempts to add a sidelink radio bearer
     * \return True if the sidelink was successfully added, else false such as when
     * an identical bearer already exists
     */
    bool AddSidelinkRadioBearer (Ptr<LteSidelinkRadioBearerInfo> slb);

    /**
     * Deletes a sidelink radio bearer
     * \param group The group identifying the radio bearer to delete
     * \return true if the bearer was successfully deleted
     */
    bool DeleteSidelinkRadioBearer (uint32_t src, uint32_t group);
    
    /**
     * Return the radio bearer for the given group
     * \return The radio bearer for the given group
     */
    Ptr<LteSidelinkRadioBearerInfo> GetSidelinkRadioBearer (uint32_t src, uint32_t group);

    /**
     * Return the transmit radio bearer for the given group
     * \return The transmit radio bearer for the given group
     */
    Ptr<LteSidelinkRadioBearerInfo> GetSidelinkRadioBearer (uint32_t group);

    /**
     * Add applications depending on the interest (monitoring or announcing)
     * \param apps applicatiosn IDs to be added
     * \param rxtx 0 to monitor and 1 to announce
     */
    void AddDiscoveryApps (std::list<uint32_t> apps, bool rxtx);

   /**
    * Remove applications depending on the interest (monitoring or announcing)
    *
    * \param apps applicatiosn IDs to be removed
    * \param rxtx 0 to monitor and 1 to announce
    */
    void RemoveDiscoveryApps (std::list<uint32_t> apps, bool rxtx);

    /**
     * Records the time of transmission
     */
    void RecordTransmissionOfSidelinkUeInformation ();

    /**
     * Returns the time of last transmission
     * \return The time of last transmission
     */
    double GetTimeSinceLastTransmissionOfSidelinkUeInformation ();

    /**
     * Returns the next available LCID
     */
    uint8_t GetNextLcid ();

    /**
     * Indicates if cell is broadcasting SIB 18
     * \param The cell ID
     * \return true if the SIB 18 was received from the cell
     */
    bool IsCellBroadcastingSIB18 (uint16_t cellId);

    /**
     * Indicates if cell is broadcasting SIB 19
     * \param The cell ID
     * \return true if the SIB 19 was received from the cell
     */
    bool IsCellBroadcastingSIB19 (uint16_t cellId);
  private:

    /**
     * Indicates if sidelink is enabled
     */
    bool m_slEnabled;
    
    /**
     * Indicates if discovery is enabled
     */
    bool m_discEnabled;   

    /**
     * The preconfiguration for out of coverage scenarios
     */
    NistLteRrcSap::SlPreconfiguration m_preconfiguration;
    
    /**
     * Map between cell ID and sidelink configuration
     */
    std::map <uint16_t, LteSlCellConfiguration> m_slMap;

    /**
     * Map between source, group, and radio bearer for transmissions
     */
    std::map <uint32_t, std::map <uint32_t, Ptr<LteSidelinkRadioBearerInfo> > > m_slrbMap;

    /**
     * The time the last SidelinkUeInformation was sent
     */
    Time m_lastSidelinkUeInformationTime;

    /**
     * The source layer 2 ID to be used
     */
    uint32_t m_sourceL2Id;

    /*
     * List of groups for which the UE is interested in receiving and may or may not have
     * radio bearers yet
     */
    std::list<uint32_t> m_rxGroup;

    std::vector< std::pair <NistLteRrcSap::SlCommResourcePool, Ptr<SidelinkRxCommResourcePool> > > rxPools;
    
    /**
     * list of IDs of applications to monitor
     */
    std::list<uint32_t> m_monitorApps;

    /**
     * list of IDs of applications to announce
     */
    std::list<uint32_t> m_announceApps;

    /**
     * List of receiving pools
     */
    std::vector< std::pair <NistLteRrcSap::SlDiscResourcePool, Ptr<SidelinkRxDiscResourcePool> > > monitorPools;

    /*
     * discTxResourceReq : number of resources the UE requires every discovery period 
     * for transmitting sidelink direct discovery announcement
     * i.e. number of separate discovery message(s) the UE wants to transmit every discovery period
     * (to be set in the scenario)
     */
    uint8_t m_discTxResources;
    
    /*
     * frequency that the UE is supposed to monitor for discovery announcements
     */
    uint16_t m_discInterFreq;
    
    Ptr<UniformRandomVariable> m_rand;
  }; //end of LteUeRrcSl'class 
  
/**
 *
 *
 */
class NistLteUeRrc : public Object
{

  friend class NistUeMemberLteUeCmacSapUser;
  friend class NistUeRrcMemberLteEnbCmacSapUser;
  friend class NistLtePdcpSpecificLtePdcpSapUser<NistLteUeRrc>;
  friend class NistMemberLteAsSapProvider<NistLteUeRrc>;
  friend class NistMemberLteUeCphySapUser<NistLteUeRrc>;
  friend class NistMemberLteUeRrcSapProvider<NistLteUeRrc>;

public:

  /**
   * The states of the UE RRC entity
   * 
   */
  enum State
  {
    IDLE_START = 0,
    IDLE_CELL_SEARCH,
    IDLE_WAIT_MIB_SIB1,
    IDLE_WAIT_MIB,
    IDLE_WAIT_SIB1,
    IDLE_CAMPED_NORMALLY,
    IDLE_WAIT_SIB2,
    IDLE_RANDOM_ACCESS,
    IDLE_CONNECTING,
    CONNECTED_NORMALLY,
    CONNECTED_HANDOVER,
    CONNECTED_PHY_PROBLEM,
    CONNECTED_REESTABLISHING,
    NUM_STATES
  };


  /**
   * create an RRC instance for use within an ue
   *
   */
  NistLteUeRrc ();

  /**
   * Destructor
   */
  virtual ~NistLteUeRrc ();

  // inherited from Object
private:
  virtual void DoInitialize (void);
  virtual void DoDispose (void);
public:
  static TypeId GetTypeId (void);


  /**
   * set the CPHY SAP this RRC should use to interact with the PHY
   *
   * \param s the CPHY SAP Provider
   */
  void SetNistLteUeCphySapProvider (NistLteUeCphySapProvider * s);

  /**
   *
   *
   * \return s the CPHY SAP User interface offered to the PHY by this RRC
   */
  NistLteUeCphySapUser* GetNistLteUeCphySapUser ();

  /**
   * set the CMAC SAP this RRC should interact with
   *
   * \param s the CMAC SAP Provider to be used by this RRC
   */
  void SetNistLteUeCmacSapProvider (NistLteUeCmacSapProvider * s);

  /**
   *
   *
   * \return s the CMAC SAP User interface offered to the MAC by this RRC
   */
  NistLteUeCmacSapUser* GetNistLteUeCmacSapUser ();


  /**
   * set the RRC SAP this RRC should interact with
   *
   * \param s the RRC SAP User to be used by this RRC
   */
  void SetNistLteUeRrcSapUser (NistLteUeRrcSapUser * s);

  /**
   *
   *
   * \return s the RRC SAP Provider interface offered to the MAC by this RRC
   */
  NistLteUeRrcSapProvider* GetNistLteUeRrcSapProvider ();

  /**
   * set the MAC SAP provider. The ue RRC does not use this
   * directly, but it needs to provide it to newly created RLC instances.
   *
   * \param s the MAC SAP provider that will be used by all
   * newly created RLC instances
   */
  void SetNistLteMacSapProvider (NistLteMacSapProvider* s);

  /** 
   * Set the AS SAP user to interact with the NAS entity
   * 
   * \param s the AS SAP user
   */
  void SetAsSapUser (NistLteAsSapUser* s);

  /** 
   * 
   * 
   * \return the AS SAP provider exported by this RRC
   */
  NistLteAsSapProvider* GetAsSapProvider ();

  /** 
   * 
   * \param imsi the unique UE identifier
   */
  void SetImsi (uint64_t imsi);

  /**
   *
   * \return imsi the unique UE identifier
   */
  uint64_t GetImsi (void) const;

  /**
   *
   * \return the C-RNTI of the user
   */
  uint16_t GetRnti () const;

  /**
   *
   * \return the CellId of the attached Enb
   */
  uint16_t GetCellId () const;

  /** 
   * \return the uplink bandwidth in RBs
   */
  uint8_t GetUlBandwidth () const;

  /** 
   * \return the downlink bandwidth in RBs
   */
  uint8_t GetDlBandwidth () const;

  /**
   * \return the downlink carrier frequency (EARFCN)
   */
  uint16_t GetDlEarfcn () const;

  /** 
   * \return the uplink carrier frequency (EARFCN)
   */
  uint16_t GetUlEarfcn () const;

  /**
   *
   * \return the current state
   */
  State GetState () const;

  /** 
   * 
   * 
   * \param val true if RLC SM is to be used, false if RLC UM/AM are to be used
   */
  void SetUseRlcSm (bool val);


  /**
   * TracedCallback signature for imsi, cellId and rnti events.
   *
   * \param [in] imsi
   * \param [in] cellId
   */
  typedef void (* CellSelectionTracedCallback)
    (const uint64_t imsi, const uint16_t cellId);

  /**
   * TracedCallback signature for imsi, cellId and rnti events.
   *
   * \param [in] imsi
   * \param [in] cellId
   * \param [in] rnti
   */
  typedef void (* ImsiCidRntiTracedCallback)
    (const uint64_t imsi, const uint16_t cellId, const uint16_t rnti);

  /**
   * TracedCallback signature for MIBRecieved, Sib1Received and
   * HandoverStart events.
   *
   * \param [in] imsi
   * \param [in] cellId
   * \param [in] rnti
   * \param [in] otherCid
   */
  typedef void (* MibSibHandoverTracedCallback)
    (const uint64_t imsi, const uint16_t cellId, const uint16_t rnti,
     const uint16_t otherCid);

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

  /**
   * \brief Set the SLSSID for the UE
   * \param slssid
   */
  void SetSlssid(uint64_t slssid);
  /**
   * \brief Get the SLSSID of the UE
   * \return slssid
   */
  uint64_t GetSlssid();
  /**
   * \brief Get the current frame number reported to the RRC
   * \return the frame number value
   */
  uint64_t GetFrameNumber();
  /**
   * \brief Get the current subframe number reported to the RRC
   * \return the subframe number value
   */
  uint64_t GetSubFrameNumber();


  /**
     * Represents the values to be reported when the UE changes of SyncRef
     */
    struct SlChangeOfSyncRefStatParameters
    {
      uint64_t  imsi;			///< UE IMSI
      uint64_t  prevSlssid;		///< Previous SLSSID
      uint16_t  prevRxOffset;	///< Previous SLSS reception offset
      uint16_t  prevFrameNo; 	///< Previous frame number
      uint16_t  prevSubframeNo;	///< Previous subframe number
      uint64_t  currSlssid;		///< Current SLSSID (after the change)
      uint16_t  currRxOffset;	///< Current SLSS reception offset (after the change)
      uint16_t  currFrameNo;	///< Current frame number (after the change)
      uint16_t  currSubframeNo; ///< Current subframe number (after the change)
    };

  /**
   * TracedCallback signature for Change of SyncRef events
   *
   * \param [in] param (list of parameters to be reported, see SlChangeOfSyncRefStatParameters)
   */
    typedef void (* ChangeOfSyncRefTracedCallback)
          (const SlChangeOfSyncRefStatParameters param );

  /**
   * TracedCallback signature for Send SLSS events
   *
   * \param [in] imsi
   * \param [in] slssid
   * \param [in] txOffset (txSlSyncOffsetIndicator used)
   * \param [in] inCoverage (MIB-SL content)
   * \param [in] frame (MIB-SL content)
   * \param [in] subframe (MIB-SL content)
   */
  typedef void (* SendSLSSTracedCallback)
        (const uint64_t imsi, const uint64_t slssid, const uint16_t txOffset, const bool inCoverage,  const uint16_t frame, const uint16_t subframe);

    /**
   * TracedCallback signature for reception of discovery message.
   *
   * \param [in] imsi
   * \param [in] cellId
   * \param [in] rnti
   * \param [in] proSeAppCode
   */
  typedef void (* DiscoveryMonitoringTracedCallback)
    (const uint64_t imsi, const uint16_t cellId, const uint16_t rnti, const uint32_t proSeAppCode);



private:


  // PDCP SAP methods
  void DoReceivePdcpSdu (NistLtePdcpSapUser::NistReceivePdcpSduParameters params);

  // CMAC SAP methods
  void DoSetTemporaryCellRnti (uint16_t rnti);
  void DoNotifyRandomAccessSuccessful ();
  void DoNotifyRandomAccessFailed ();
  //communication
  void DoNotifySidelinkReception (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id);
  //discovery
  void DoNotifyDiscoveryReception (Ptr<NistLteControlMessage> msg);

  // LTE AS SAP methods
  void DoSetCsgWhiteList (uint32_t csgId);
  void DoForceCampedOnEnb (uint16_t cellId, uint16_t dlEarfcn);
  void DoStartCellSelection (uint16_t dlEarfcn);
  void DoConnect ();
  void DoSendData (Ptr<Packet> packet, uint8_t bid);
  void DoSendData (Ptr<Packet> packet, uint32_t group);
  void DoDisconnect ();
  // communication
  void DoActivateSidelinkRadioBearer (uint32_t group, bool tx, bool rx);
  void DoDeactivateSidelinkRadioBearer (uint32_t group);
  // discovery
  void DoAddDiscoveryApps (std::list<uint32_t> apps, bool rxtx);
  void DoRemoveDiscoveryApps (std::list<uint32_t> apps, bool rxtx);

  // CPHY SAP methods
  void DoRecvNistMasterInformationBlock (uint16_t cellId,
                                     NistLteRrcSap::NistMasterInformationBlock msg);
  void DoRecvNistSystemInformationBlockType1 (uint16_t cellId,
                                          NistLteRrcSap::NistSystemInformationBlockType1 msg);
  void DoReportNistUeMeasurements (NistLteUeCphySapUser::NistUeMeasurementsParameters params);

  // RRC SAP methods

  /// Part of the RRC protocol. Implement the NistLteUeRrcSapProvider::CompleteSetup interface.
  void DoCompleteSetup (NistLteUeRrcSapProvider::CompleteNistSetupParameters params);
  /// Part of the RRC protocol. Implement the NistLteUeRrcSapProvider::RecvNistSystemInformation interface.
  void DoRecvNistSystemInformation (NistLteRrcSap::NistSystemInformation msg);
  /// Part of the RRC protocol. Implement the NistLteUeRrcSapProvider::RecvNistRrcConnectionSetup interface.
  void DoRecvNistRrcConnectionSetup (NistLteRrcSap::NistRrcConnectionSetup msg);
  /// Part of the RRC protocol. Implement the NistLteUeRrcSapProvider::RecvNistRrcConnectionReconfiguration interface.
  void DoRecvNistRrcConnectionReconfiguration (NistLteRrcSap::NistRrcConnectionReconfiguration msg);
  /// Part of the RRC protocol. Implement the NistLteUeRrcSapProvider::RecvNistRrcConnectionReestablishment interface.
  void DoRecvNistRrcConnectionReestablishment (NistLteRrcSap::NistRrcConnectionReestablishment msg);
  /// Part of the RRC protocol. Implement the NistLteUeRrcSapProvider::RecvNistRrcConnectionReestablishmentReject interface.
  void DoRecvNistRrcConnectionReestablishmentReject (NistLteRrcSap::NistRrcConnectionReestablishmentReject msg);
  /// Part of the RRC protocol. Implement the NistLteUeRrcSapProvider::RecvNistRrcConnectionRelease interface.
  void DoRecvNistRrcConnectionRelease (NistLteRrcSap::NistRrcConnectionRelease msg);
  /// Part of the RRC protocol. Implement the NistLteUeRrcSapProvider::RecvNistRrcConnectionReject interface.
  void DoRecvNistRrcConnectionReject (NistLteRrcSap::NistRrcConnectionReject msg);

 
  // INTERNAL METHODS

  /**
   * \brief Go through the list of measurement results, choose the one with the
   *        strongest RSRP, and tell PHY to synchronize to it.
   *
   * \warning This function is a part of the *initial cell selection* procedure,
   *          hence must be only executed during IDLE mode.
   */
  void SynchronizeToStrongestCell ();

  /**
   * \brief Performs cell selection evaluation to the current serving cell.
   *
   * \warning This function is a part of the *initial cell selection* procedure,
   *          hence must be only executed during IDLE mode and specifically
   *          during the state when the UE just received the first SIB1 message
   *          from the serving cell.
   *
   * This function assumes that the required information for the evaluation
   * procedure have been readily gathered, such as *measurement results*, MIB,
   * and SIB1. Please refer to the LTE module's Design Documentation for more
   * details on the evaluation process.
   *
   * If the cell passes the evaluation, the UE will immediately camp to it.
   * Otherwise, the UE will pick another cell and restart the cell selection
   * procedure.
   */
  void EvaluateCellForSelection ();

  /**
   * \brief Update the current measurement configuration #m_varNistMeasConfig.
   * \param mc measurements to be performed by the UE
   *
   * Implements Section 5.5.2 "Measurement configuration" of 3GPP TS 36.331.
   * The supported subfunctions are:
   * - Measurement object removal
   * - Measurement object addition/ modification
   * - Reporting configuration removal
   * - Reporting configuration addition/ modification
   * - Quantity configuration
   * - Measurement identity removal
   * - Measurement identity addition/ modification
   *
   * The subfunctions that will be invoked are determined by the content of
   * the given measurement configuration.
   *
   * Note the existence of some chain reaction behaviours:
   * - Removal of measurement object or reporting configuration also removes any
   *   impacted measurement identities.
   * - Removal of measurement identity also removes any associated *reporting
   *   entry* from #m_varMeasReportList.
   * - Modification to measurement object or reporting configuration also
   *   removes any reporting entries of the impacted measurement identities
   *   from #m_varMeasReportList.
   * - Modification to quantity configuration also removes all existing
   *   reporting entries from #m_varMeasReportList, regardless of measurement
   *   identity.
   *
   * Some unsupported features:
   * - List of neighbouring cells
   * - List of black cells
   * - CGI reporting
   * - Periodical reporting configuration
   * - Measurement gaps
   * - s-Measure
   * - Speed-dependent scaling
   *
   * \warning There is a possibility that the input argument (of type
   *          NistLteRrcSap::NistMeasConfig) may contain information in fields related
   *          to the unsupported features. In such case, the function will raise
   *          an error.
   *
   * The measurement configuration given as an argument is typically provided by
   * the serving eNodeB. It is transmitted through the RRC protocol when the UE
   * joins the cell, e.g., by connection establishment or by incoming handover.
   * The information inside the argument can be configured from the eNodeB side,
   * which would then equally apply to all other UEs attached to the same
   * eNodeB. See the LTE module's User Documentation for more information on
   * configuring this.
   *
   * \sa NistLteRrcSap::NistMeasConfig, NistLteUeRrc::m_varMeasReportList
   */
  void ApplyNistMeasConfig (NistLteRrcSap::NistMeasConfig mc);

  /**
   * \brief Keep the given measurement result as the latest measurement figures,
   *        to be utilised by UE RRC functions.
   * \param cellId the cell ID of the measured cell
   * \param rsrp measured RSRP value to be saved (in dBm)
   * \param rsrq measured RSRQ value to be saved (in dB)
   * \param useLayer3Filtering
   * \todo Remove the useLayer3Filtering argument
   *
   * Implements Section 5.5.3.2 "Layer 3 filtering" of 3GPP TS 36.331. *Layer-3
   * filtering* is applied to the given measurement results before saved to
   * #m_storedNistMeasValues. The filtering is however disabled when the UE is in
   * IDLE mode, i.e., saving unfiltered values.
   *
   * Layer-3 filtering is influenced by a filter coefficient, which determines
   * the strength of the filtering. This coefficient is provided by the active
   * *quantity configuration* in #m_varNistMeasConfig, which is configured by the
   * NistLteUeRrc::ApplyNistMeasConfig. Details on how the coefficient works and how to
   * modify it can be found in LTE module's Design Documentation.
   *
   * \sa NistLteUeRrc::m_storedNistMeasValues
   */
  void SaveNistUeMeasurements (uint16_t cellId, double rsrp, double rsrq,
                           bool useLayer3Filtering);

  /**
   * \brief Evaluate the reporting criteria of a measurement identity and
   *        invoke some reporting actions based on the result.
   * \param measId the measurement identity to be evaluated
   *
   * Implements Section 5.5.4.1 "Measurement report triggering - General" of
   * 3GPP TS 36.331. This function take into use the latest measurement results
   * and evaluate them against the *entering condition* and the *leaving
   * condition* of the measurement identity's reporting criteria. The evaluation
   * also take into account other defined criteria, such as *hysteresis* and
   * *time-to-trigger*.
   *
   * The entering and leaving condition to be evaluated are determined by the
   * *event type* of the measurement identity's reporting criteria. As defined
   * in NistLteRrcSap::NistReportConfigEutra, there 5 supported events. The gore details
   * of these events can be found in Section 5.5.4 of 3GPP TS 36.331.
   *
   * An applicable entering condition (i.e., the condition evaluates to true)
   * will insert a new *reporting entry* to #m_varMeasReportList, so
   * *measurement reports* would be produced and submitted to eNodeB. On the
   * other hand, an applicable leaving condition will remove the related
   * reporting entry from #m_varMeasReportList, so submission of related
   * measurement reports to eNodeB will be suspended.
   */
  void NistMeasurementReportTriggering (uint8_t measId);

  /**
   * \brief Produce a proper measurement report from the given measurement
   *        identity's reporting entry in #m_varMeasReportList and then submit
   *        it to the serving eNodeB.
   * \param measId the measurement identity which report is to be submitted.
   *
   * Implements Section 5.5.5 "Measurement reporting" of 3GPP TS 36.331.
   * Producing a *measurement report* involves several tasks such as:
   * - including the measurement results of the serving cell into the report;
   * - selecting some neighbour cells which triggered the reporting (i.e., those
   *   in *cellsTriggeredList*) to be included in the report;
   * - sorting the order of neighbour cells in the report by their RSRP or RSRQ
   *   measurement results (the highest comes first); and
   * - ensuring the number of neighbour cells in the report is under the
   *   *maxReportCells* limit defined by the measurement identity's reporting
   *   configuration.
   *
   * The RSRP and RSRQ measurement results included in the report are expressed
   * in 3GPP-specified range format. They are converted from dBm and dB units
   * using NistEutranMeasurementMapping::Dbm2RsrpRange and
   * NistEutranMeasurementMapping::Db2RsrqRange functions.
   *
   * Measurement report is submitted to the serving eNodeB through the *RRC
   * protocol*. The NistLteUeRrcSapUser::SendNistMeasurementReport method of the *UE RRC
   * SAP* facilitates this submission.
   *
   * After the submission, the function will repeat itself after a certain
   * interval. The interval length may vary from 120 ms to 60 minutes and is
   * determined by the *report interval* parameter specified by the measurement
   * identity's reporting configuration.
   */
  void SendNistMeasurementReport (uint8_t measId);

  void ApplyNistRadioResourceConfigDedicated (NistLteRrcSap::NistRadioResourceConfigDedicated rrcd);
  void StartConnection ();
  void LeaveConnectedMode ();
  void DisposeOldSrb1 ();
  uint8_t Bid2Drbid (uint8_t bid);
  /**
   * Switch the UE RRC to the given state.
   * \param s the destination state
   */
  void SwitchToState (State s);

  /**
   * Process dedicated sidelink configuration
   * \param config The sidelink configuration
   */
  void ApplySidelinkDedicatedConfiguration (NistLteRrcSap::SlCommConfig config);

  /**
   * Process dedicated sidelink configuration (discovery)
   * \param config The sidelink configuration (discovery)
   */
  void ApplySidelinkDedicatedConfiguration (NistLteRrcSap::SlDiscConfig config);

  /**
   * Transmits a SidelinkUEInformation message to the eNodeB
   */
  void SendSidelinkUeInformation ();

  Ptr<LteSidelinkRadioBearerInfo> AddSlrb (uint32_t source, uint32_t group, uint8_t lcid);
  
  std::map<uint8_t, uint8_t> m_bid2DrbidMap;

  NistLteUeCphySapUser* m_cphySapUser;
  NistLteUeCphySapProvider* m_cphySapProvider;

  NistLteUeCmacSapUser* m_cmacSapUser;
  NistLteUeCmacSapProvider* m_cmacSapProvider;

  NistLteUeRrcSapUser* m_rrcSapUser;
  NistLteUeRrcSapProvider* m_rrcSapProvider;

  NistLteMacSapProvider* m_macSapProvider;
  NistLtePdcpSapUser* m_drbPdcpSapUser;

  NistLteAsSapProvider* m_asSapProvider;
  NistLteAsSapUser* m_asSapUser;

  /// The current UE RRC state.
  State m_state;

  /// The unique UE identifier.
  uint64_t m_imsi;
  /**
   * The `C-RNTI` attribute. Cell Radio Network Temporary Identifier.
   */
  uint16_t m_rnti;
  /**
   * The `CellId` attribute. Serving cell identifier.
   */
  uint16_t m_cellId;

  /**
   * The `Srb0` attribute. SignalingRadioNistBearerInfo for SRB0.
   */
  Ptr<LteSignalingRadioNistBearerInfo> m_srb0;
  /**
   * The `Srb1` attribute. SignalingRadioNistBearerInfo for SRB1.
   */
  Ptr<LteSignalingRadioNistBearerInfo> m_srb1;
  /**
   * SRB1 configuration before RRC connection reconfiguration. To be deleted
   * soon by DisposeOldSrb1().
   */
  Ptr<LteSignalingRadioNistBearerInfo> m_srb1Old;
  /**
   * The `DataRadioBearerMap` attribute. List of UE RadioNistBearerInfo for Data
   * Radio Bearers by LCID.
   */
  std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> > m_drbMap;

  /**
   * True if RLC SM is to be used, false if RLC UM/AM are to be used.
   * Can be modified using SetUseRlcSm().
   */
  bool m_useRlcSm;

  uint8_t m_lastRrcTransactionIdentifier;

  NistLteRrcSap::NistPdschConfigDedicated m_pdschConfigDedicated;

  uint8_t m_dlBandwidth; /**< Downlink bandwidth in RBs. */
  uint8_t m_ulBandwidth; /**< Uplink bandwidth in RBs. */

  uint16_t m_dlEarfcn;  /**< Downlink carrier frequency. */
  uint16_t m_ulEarfcn;  /**< Uplink carrier frequency. */

  /**
   * The `MibReceived` trace source. Fired upon reception of Master Information
   * Block. Exporting IMSI, the serving cell ID, RNTI, and the source cell ID.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t, uint16_t> m_mibReceivedTrace;
  /**
   * The `Sib1Received` trace source. Fired upon reception of System
   * Information Block Type 1. Exporting IMSI, the serving cell ID, RNTI, and
   * the source cell ID.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t, uint16_t> m_sib1ReceivedTrace;
  /**
   * The `Sib2Received` trace source. Fired upon reception of System
   * Information Block Type 2. Exporting IMSI, the serving cell ID, RNTI.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t> m_sib2ReceivedTrace;
  /**
   * The `StateTransition` trace source. Fired upon every UE RRC state
   * transition. Exporting IMSI, the serving cell ID, RNTI, old state, and new
   * state.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t, State, State> m_stateTransitionTrace;
  /**
   * The `InitialCellSelectionEndOk` trace source. Fired upon successful
   * initial cell selection procedure. Exporting IMSI and the selected cell ID.
   */
  TracedCallback<uint64_t, uint16_t> m_initialCellSelectionEndOkTrace;
  /**
   * The `InitialCellSelectionEndError` trace source. Fired upon failed initial
   * cell selection procedure. Exporting IMSI and the cell ID under evaluation.
   */
  TracedCallback<uint64_t, uint16_t> m_initialCellSelectionEndErrorTrace;
  /**
   * The `RandomAccessSuccessful` trace source. Fired upon successful
   * completion of the random access procedure. Exporting IMSI, cell ID, and
   * RNTI.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t> m_randomAccessSuccessfulTrace;
  /**
   * The `RandomAccessError` trace source. Fired upon failure of the random
   * access procedure. Exporting IMSI, cell ID, and RNTI.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t> m_randomAccessErrorTrace;
  /**
   * The `ConnectionEstablished` trace source. Fired upon successful RRC
   * connection establishment. Exporting IMSI, cell ID, and RNTI.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t> m_connectionEstablishedTrace;
  /**
   * The `ConnectionTimeout` trace source. Fired upon timeout RRC connection
   * establishment because of T300. Exporting IMSI, cell ID, and RNTI.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t> m_connectionTimeoutTrace;
  /**
   * The `ConnectionReconfiguration` trace source. Fired upon RRC connection
   * reconfiguration. Exporting IMSI, cell ID, and RNTI.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t> m_connectionReconfigurationTrace;
  /**
   * The `HandoverStart` trace source. Fired upon start of a handover
   * procedure. Exporting IMSI, source cell ID, RNTI, and target cell ID.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t, uint16_t> m_handoverStartTrace;
  /**
   * The `HandoverEndOk` trace source. Fired upon successful termination of a
   * handover procedure. Exporting IMSI, cell ID, and RNTI.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t> m_handoverEndOkTrace;
  /**
   * The `HandoverEndError` trace source. Fired upon failure of a handover
   * procedure. Exporting IMSI, cell ID, and RNTI.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t> m_handoverEndErrorTrace;
  
  /**
   * The `m_discoveryMsgReceived` trace source. Track the reception of discovery message (monitor)
   * Exporting IMSI, cell ID, RNTI, ProSe App Code.
   */
  TracedCallback<uint64_t, uint16_t, uint16_t, uint32_t> m_discoveryMonitoringTrace;


  /// True if a connection request by upper layers is pending.
  bool m_connectionPending;
  /// True if MIB was received for the current cell.
  bool m_hasReceivedMib;
  /// True if SIB1 was received for the current cell.
  bool m_hasReceivedSib1;
  /// True if SIB2 was received for the current cell.
  bool m_hasReceivedSib2;

  /// Stored content of the last SIB1 received.
  NistLteRrcSap::NistSystemInformationBlockType1 m_lastSib1;

  /// List of cell ID of acceptable cells for cell selection that have been detected.
  std::set<uint16_t> m_acceptableCell;

  /// List of CSG ID which this UE entity has access to.
  uint32_t m_csgWhiteList;

  /**
   * Sidelink configuration
   */
  Ptr<LteUeRrcSl> m_sidelinkConfiguration;

  // INTERNAL DATA STRUCTURE RELATED TO UE MEASUREMENTS

  /**
   * \brief Includes the accumulated configuration of the measurements to be
   *        performed by the UE.
   *
   * Based on 3GPP TS 36.331 section 7.1. Also note that some optional variables
   * in the specification are omitted.
   */
  struct VarNistMeasConfig
  {
    std::map<uint8_t, NistLteRrcSap::NistMeasIdToAddMod> measIdList;
    std::map<uint8_t, NistLteRrcSap::NistMeasObjectToAddMod> measObjectList;
    std::map<uint8_t, NistLteRrcSap::NistReportConfigToAddMod> reportConfigList;
    NistLteRrcSap::NistQuantityConfig quantityConfig; 
    double aRsrp;
    double aRsrq;
  };

  /**
   * \brief Includes the accumulated configuration of the measurements to be
   *        performed by the UE.
   *
   * Based on 3GPP TS 36.331 section 7.1.
   */
  VarNistMeasConfig m_varNistMeasConfig;

  /**
   * \brief Represents a single measurement reporting entry., which includes
   *        information about a measurement for which the triggering conditions
   *        have been met.
   *
   * Based on 3GPP TS 36.331 section 7.1.
   */
  struct NistVarMeasReport
  {
    uint8_t measId;
    std::set<uint16_t> cellsTriggeredList; // note: only E-UTRA is supported.
    uint32_t numberOfReportsSent;
    EventId periodicReportTimer;
  };

  /**
   * \brief The list of active reporting entries, indexed by the measurement
   *        identity which triggered the reporting. Includes information about
   *        measurements for which the triggering conditions have been met.
   */
  std::map<uint8_t, NistVarMeasReport> m_varMeasReportList;

  /**
   * \brief List of cell IDs which are responsible for a certain trigger.
   */
  typedef std::list<uint16_t> ConcernedCells_t;

  /**
   * \brief Compose a new reporting entry of the given measurement identity,
   *        insert it into #m_varMeasReportList, and set it up for submission
   *        to eNodeB.
   * \param measId the measurement identity which the new reporting entry will
   *               be based upon
   * \param enteringCells the cells which are responsible for triggering the
   *                      reporting (i.e., successfully fulfilling the entering
   *                      condition of the measurement identity) and will be
   *                      included in the measurement report.
   *
   * \note If an existing reporting entry with the same measurement identity has
   *       already existed in #m_varMeasReportList, the function will update it
   *       by adding the entering cells into the existing reporting entry.
   * \note When time-to-trigger is enabled for this measurement identity, the
   *       function will also remove the related trigger from the
   *       #m_enteringTriggerQueue.
   */
  void NistVarMeasReportListAdd (uint8_t measId, ConcernedCells_t enteringCells);

  /**
   * \brief Remove some cells from an existing reporting entry in
   *        #m_varMeasReportList.
   * \param measId the measurement identity to be removed from
   *               #m_varMeasReportList, must already exists there, otherwise
   *               an error would be raised
   * \param leavingCells the cells which are about to be removed
   * \param reportOnLeave when true, will make the function send one last
   *                      measurement report to eNodeB before removing it
   *
   * \note If a given cell is not found in the reporting entry, the function
   *       will quietly continue.
   * \note If the removal has removed all the cells in the reporting entry, the
   *       function will remove the reporting entry as well.
   * \note When time-to-trigger is enabled for this measurement identity, the
   *       function will also remove the related trigger from the
   *       #m_leavingTriggerQueue.
   */
  void NistVarMeasReportListErase (uint8_t measId, ConcernedCells_t leavingCells,
                               bool reportOnLeave);

  /**
   * \brief Remove the reporting entry of the given measurement identity from
   *        #m_varMeasReportList.
   * \param measId the measurement identity to be removed from
   *               #m_varMeasReportList, must already exists there, otherwise
   *               an error would be raised
   *
   * Any events or triggers related with this measurement identity will be
   * canceled as well.
   */
  void NistVarMeasReportListClear (uint8_t measId);

  /**
   * \brief Represents a measurement result from a certain cell.
   */
  struct NistMeasValues
  {
    double rsrp; ///< Measured RSRP in dBm.
    double rsrq; ///< Measured RSRQ in dB.
    Time timestamp; ///< Not used. \todo Should be removed.
  };

  /**
   * \brief Internal storage of the latest measurement results from all detected
   *        detected cells, indexed by the cell ID where the measurement was
   *        taken from.
   *
   * Each *measurement result* comprises of RSRP (in dBm) and RSRQ (in dB).
   *
   * In IDLE mode, the measurement results are used by the *initial cell
   * selection* procedure. While in CONNECTED mode, *layer-3 filtering* is
   * applied to the measurement results and they are used by *UE measurements*
   * function (NistLteUeRrc::NistMeasurementReportTriggering and
   * NistLteUeRrc::SendNistMeasurementReport).
   */
  std::map<uint16_t, NistMeasValues> m_storedNistMeasValues;

  /**
   * \brief Represents a single triggered event from a measurement identity
   *        which reporting criteria have been fulfilled, but delayed by
   *        time-to-trigger.
   */
  struct NistPendingTrigger_t
  {
    uint8_t measId; ///< The measurement identity which raised the trigger.
    ConcernedCells_t concernedCells; ///< The list of cells responsible for this trigger.
    EventId timer; ///< The pending reporting event, scheduled at the end of the time-to-trigger.
  };

  /**
   * \brief List of triggers that were raised because entering condition have
   *        been true, but are still delayed from reporting it by
   *        time-to-trigger.
   *
   * The list is indexed by the measurement identity where the trigger
   * originates from. The enclosed event will run at the end of the
   * time-to-trigger and insert a *reporting entry* to #m_varMeasReportList.
   */
  std::map<uint8_t, std::list<NistPendingTrigger_t> > m_enteringTriggerQueue;

  /**
   * \brief List of triggers that were raised because leaving condition have
   *        been true, but are still delayed from stopping the reporting by
   *        time-to-trigger.
   *
   * The list is indexed by the measurement identity where the trigger
   * originates from. The enclosed event will run at the end of the
   * time-to-trigger and remove the associated *reporting entry* from
   * #m_varMeasReportList.
   */
  std::map<uint8_t, std::list<NistPendingTrigger_t> > m_leavingTriggerQueue;

  /**
   * \brief Clear all the waiting triggers in #m_enteringTriggerQueue which are
   *        associated with the given measurement identity.
   * \param measId the measurement identity to be processed, must already exists
   *               in #m_enteringTriggerQueue, otherwise an error would be
   *               raised
   *
   * \note The function may conclude that there is nothing to be removed. In
   *       this case, the function will simply ignore quietly.
   *
   * This function is used when the entering condition of the measurement
   * identity becomes no longer true. Therefore all the waiting triggers for
   * this measurement identity in #m_enteringTriggerQueue have become invalid
   * and must be canceled.
   *
   * \sa NistLteUeRrc::m_enteringTriggerQueue
   */
  void CancelEnteringTrigger (uint8_t measId);

  /**
   * \brief Remove a specific cell from the waiting triggers in
   *        #m_enteringTriggerQueue which belong to the given measurement
   *        identity.
   * \param measId the measurement identity to be processed, must already exists
   *               in #m_enteringTriggerQueue, otherwise an error would be
   *               raised
   * \param cellId the cell ID to be removed from the waiting triggers
   *
   * \note The function may conclude that there is nothing to be removed. In
   *       this case, the function will simply ignore quietly.
   *
   * This function is used when a specific neighbour cell no longer fulfills
   * the entering condition of the measurement identity. Thus the cell must be
   * removed from all the waiting triggers for this measurement identity in
   * #m_enteringTriggerQueue.
   *
   * \sa NistLteUeRrc::m_enteringTriggerQueue
   */
  void CancelEnteringTrigger (uint8_t measId, uint16_t cellId);

  /**
   * \brief Clear all the waiting triggers in #m_leavingTriggerQueue which are
   *        associated with the given measurement identity.
   * \param measId the measurement identity to be processed, must already exists
   *               in #m_leavingTriggerQueue, otherwise an error would be
   *               raised
   *
   * \note The function may conclude that there is nothing to be removed. In
   *       this case, the function will simply ignore quietly.
   *
   * This function is used when the leaving condition of the measurement
   * identity becomes no longer true. Therefore all the waiting triggers for
   * this measurement identity in #m_leavingTriggerQueue have become invalid
   * and must be canceled.
   *
   * \sa NistLteUeRrc::m_leavingTriggerQueue
   */
  void CancelLeavingTrigger (uint8_t measId);

  /**
   * \brief Remove a specific cell from the waiting triggers in
   *        #m_leavingTriggerQueue which belong to the given measurement
   *        identity.
   * \param measId the measurement identity to be processed, must already exists
   *               in #m_leavingTriggerQueue, otherwise an error would be
   *               raised
   * \param cellId the cell ID to be removed from the waiting triggers
   *
   * \note The function may conclude that there is nothing to be removed. In
   *       this case, the function will simply ignore quietly.
   *
   * This function is used when a specific neighbour cell no longer fulfills
   * the leaving condition of the measurement identity. Thus the cell must be
   * removed from all the waiting triggers for this measurement identity in
   * #m_leavingTriggerQueue.
   *
   * \sa NistLteUeRrc::m_leavingTriggerQueue
   */
  void CancelLeavingTrigger (uint8_t measId, uint16_t cellId);

  /**
   * The `T300` attribute. Timer for RRC connection establishment procedure
   * (i.e., the procedure is deemed as failed if it takes longer than this).
   * See Section 7.3 of 3GPP TS 36.331.
   */
  Time m_t300;

  /**
   * \brief Invokes ConnectionEstablishmentTimeout() if RRC connection
   *        establishment procedure for this UE takes longer than T300.
   */
  EventId m_connectionTimeout;

  /**
   * \brief Invoked after timer T300 expires, notifying upper layers that RRC
   *        connection establishment procedure has failed.
   */
  void ConnectionTimeout ();

  /**
   * True if the transmission of SLSS is enabled for the UE
   * (out of coverage sidelink synchronization procedure is enabled)
   */
  bool m_slssTransmissionEnabled;
  /**
   * True if the UE should transmit SLSSs
   * i.e, UE fulfill the conditions in TS36331 5.10.7
   */
  bool m_slssTransmissionActive;
  /**
   * The offset indicator used for the transmission of SLSSs
   */
  uint16_t m_txSlSyncOffsetIndicator;
  /**
   * True if the UE has a selected SyncRef
   */
  bool m_hasSyncRef;
  /**
   * True if the UE is in coverage
   */
  bool m_inCoverage;
  /**
   * The Sidelink Synchronization Signal Identifier (SLSSID) of the UE
   */
  uint64_t m_slssId;
  /**
   * The subframe number reported to the RRC
   */
  uint16_t m_currSubframeNo;
  /**
   * The frame number reported to the RRC
   */
  uint16_t m_currFrameNo;
  /**
   * True if upper layers indicated that the UE has Sidelink Communication
   * data to transmit
   */
  bool m_hasDataToTransmit;
  /**
   * True if the selected SyncRef measured S-RSRP is above the corresponding
   * threshold (syncTxThreshOoC)
   */
  bool m_inInnerCellOfSyncRef;
  /**
   * The minimum value of S-RSRP to consider a SyncRef detected
   */
  double m_minSrsrp;
  /**
   * The time in which the (next) SLSS should be sent. Keep track to avoid duplicates
   */
  Time m_slssTxTime;

  /**
   * \brief Internal storage of the most recent MIB-SL for each SyncRef,
   * indexed by the SyncRef SLSSID and the time offset indicator in which it was received
   */
  std::map <std::pair<uint16_t,uint16_t>, NistLteRrcSap::MasterInformationBlockSL> m_latestMibSlReceived;

  /**
   * Represents the value of S-RSRP to be used in the RRC layer for SyncRef selection
   */
  struct SlssMeasValues
  {
    double srsrp; ///< Measured S-RSRP in dBm.
    Time timestamp;
  };

  /**
   * The MIB-SL used by the UE to obtain the synchronization information once it
   * has selected the SyncRef and successfully synchronized to it
   */
  NistLteRrcSap::MasterInformationBlockSL m_currSyncRef;

  /**
   * \brief Internal storage of each SyncRef S-RSRP measurement to be used for SyncRef selection
   * and SyncRef inner cell criteria,
   * indexed by the SyncRef SLSSID and the time offset indicator in which it was received
   */
  std::map <std::pair<uint16_t,uint16_t>, SlssMeasValues> m_storedSlssMeasValues;

  /**
   * \brief Internal storage of the SyncRefs detected and measured by the UE
   * in the SyncRef selection process in progress.
   * Used also to support L3 filtering by determining missing SyncRefs in conjunction
   * with m_knownSlssidList
   */
  std::vector <std::pair<uint16_t,uint16_t> > m_lastReportedSlssidList;

  /**
   * \brief Internal storage of the SyncRefs detected and measured by the UE in all simulation.
   * Used to support L3 filtering by determining missing SyncRefs in conjunction
   * with m_lastReportedSlssidList
   */
  std::vector <std::pair<uint16_t,uint16_t> > m_knownSlssidList;

  /**
   * The `ChangeOfSyncRef` trace source. Fired upon successful change of SyncRef.
   * Exporting SlChangeOfSyncRefStatParameters containing IMSI, prevSLSSID, prevRxOffset, prevFrameNo, prevSubframeNo,
   * currSLSSID, currRxOffset, currFrameNo and currSubframeNo.
   */
  TracedCallback<SlChangeOfSyncRefStatParameters> m_ChangeOfSyncRefTrace;

  /**
   * The `SendSlss` trace source. Fired upon transmission of a SLSS.
   * Exporting IMSI, SLSSID, txSlSyncOffsetIndicator, inCoverage, FrameNo and SubframeNo.
   */
  TracedCallback<uint64_t, uint64_t, uint16_t, bool, uint16_t, uint16_t> m_SendSlssTrace;


  /**
   * \brief Initiate if appropriate the transmission of Sidelink Synchronization Signals (SLSSs).
   * The function is triggered by upper layers when the UE has sidelink communication
   * data to transmit.
   * \note The function may conclude that the UE was already in a on-data period
   * and nothing is needed to be done.
   */
  void InitiateSlssTransmission();
  /**
   * \brief Stop the transmission of Sidelink Synchronization Signals (SLSSs).
   * The function is triggered by upper layers when the UE does not have anymore
   * sidelink communication data to transmit.
   * \note The function may conclude that the UE was already in a off-data period
   * and nothing is needed to be done.
   */
  void StopSlssTransmission();
  /**
   * \brief Activate the transmission of Sidelink Synchronization Signals (SLSSs).
   * The function decides the value of the SLSSID, when to send the MIB-SL (offsetIndicator),
   * and schedules it to be send for the first time.
   * \note The function may conclude that the transmission of SLSS was already active
   * and nothing is needed to be done.
   */
  void ActivateSlssTransmission ();
  /**
   * \brief Deactivate the transmission of Sidelink Synchronization Signals (SLSSs).
   * \note The function may conclude that the transmission of SLSS was not active
   * and nothing is needed to be done.
   */
  void DeactivateSlssTransmission ();
  /**
   * Store the values of the frameNo and subframeNo
   * \param frameNo current frameNo
   * \param subFrameNo current subframeNo
   */
  void SaveSubframeIndication(uint16_t frameNo, uint16_t subFrameNo);

  /**
   * The function fills the MIB-SL with the appropriate parameters, pass it to
   * lower layers to be sent, and schedule the next transmission according to the
   * SLSS frequency (every 40 ms).
   * \note The function may conclude that the UE is not  supposed to send a SLSS in the
   * current timeslot (e.g. if the UE changed of timing/SyncRef in the meantime)
   * and nothing is needed to be done.
   */
  void SendSlss();
  /**
   * Store the S-RSRP values to be used in the synchronization protocol decision processes
   * If the L3 filtering is active, the values are processed before being stored.
   * \param slssid the SLSSID of the SyncRef
   * \param offset the offset in which the SyncRef is sending SLSSs
   * \param srsrp the value of the S-RSRP
   * \param useLayer3Filtering true if the UE needs to use L3 filtering before storing the values
   */
  void SaveSlssMeasurements (uint16_t slssid, uint16_t offset, double srsrp, bool useLayer3Filtering);
  /**
   * Determine the SyncRef with highest S-RSRP, select it and start synchronization process
   * if suitable
   * \param
   */
  void SynchronizeToStrongestSyncRef ();
  /**
   * Determine if the UE is in the inner part of the cell of its SyncRef by
   * comparing the perceived S-RSRP with a predefined threshold (syncTxThreshOoC)
   * \param slssid the SLSSID of the SyncRef
   * \param offset the offset in which the SyncRef is sending SLSSs
   * \return true if the UE is in the inner part of the SyncRef cell, or false otherwise
   */
  bool IsInTheInnerPartOfTheSyncRefCell(uint16_t slssid, uint16_t offset);

  // CPHY SAP methods elated to Sidelink synchronization
  // Store the latest MIB-SL for each SyncRef
  void DoReceiveMibSL (NistLteRrcSap::MasterInformationBlockSL mibSL);
  //Process the S-RSRP measurement reported by the PHY
  void DoReportSlssMeasurements (NistLteUeCphySapUser::NistUeSlssMeasurementsParameters params,  uint64_t slssid, uint16_t offset);
  //Notify the successful change of timing/SyncRef, and store the selected/current SyncRef information
  void DoReportChangeOfSyncRef(NistLteRrcSap::MasterInformationBlockSL mibSL, uint16_t frameNo, uint16_t subFrameNo);
  //Store the current frameNo and subframeNo
  void DoReportSubframeIndication(uint16_t FrameNo, uint16_t subFrameNo);

  // CMAC SAP methods related to Sidelink synchronization
  // The MAC notifies it has data to send in order that RRC activate the SLSS transmission if appropriate
  void DoNotifyMacHasSlDataToSend();
  // The MAC notifies it does not have data to send in order that RRC deactivate the SLSS transmission if appropriate
  void DoNotifyMacHasNotSlDataToSend();


}; // end of class NistLteUeRrc


} // namespace ns3

#endif // NIST_LTE_UE_RRC_H
