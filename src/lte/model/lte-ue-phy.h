/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 TELEMATICS LAB, DEE - Politecnico di Bari
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
 * Author: Giuseppe Piro  <g.piro@poliba.it>
 * Author: Marco Miozzo <mmiozzo@cttc.es>
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#ifndef LTE_UE_PHY_H
#define LTE_UE_PHY_H


#include <ns3/lte-phy.h>
#include <ns3/ff-mac-common.h>

#include <ns3/lte-control-messages.h>
#include <ns3/lte-amc.h>
#include <ns3/lte-ue-phy-sap.h>
#include <ns3/lte-ue-cphy-sap.h>
#include <ns3/ptr.h>
#include <ns3/lte-amc.h>
#include <set>
#include <ns3/lte-ue-power-control.h>


namespace ns3 {

class PacketBurst;
class LteEnbPhy;
class LteHarqPhy;


/**
 * \ingroup lte
 *
 * The LteSpectrumPhy models the physical layer of LTE
 */
class LteUePhy : public LtePhy
{

  /// allow UeMemberLteUePhySapProvider class friend access
  friend class UeMemberLteUePhySapProvider;
  /// allow MemberLteUeCphySapProvider<LteUePhy> class friend access
  friend class MemberLteUeCphySapProvider<LteUePhy>;

public:
  /**
   * \brief The states of the UE PHY entity
   */
  enum State
  {
    CELL_SEARCH = 0,
    SYNCHRONIZED,
    NUM_STATES
  };

  /**
   * @warning the default constructor should not be used
   */
  LteUePhy ();

  /**
   *
   * \param dlPhy the downlink LteSpectrumPhy instance
   * \param ulPhy the uplink LteSpectrumPhy instance
   */
  LteUePhy (Ptr<LteSpectrumPhy> dlPhy, Ptr<LteSpectrumPhy> ulPhy);

  virtual ~LteUePhy ();

  /**
   * \brief Get the type ID.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);
  // inherited from Object
  virtual void DoInitialize (void);
  virtual void DoDispose (void);

  /**
   * \brief Get the PHY SAP provider
   * \return a pointer to the SAP Provider 
   */
  LteUePhySapProvider* GetLteUePhySapProvider ();

  /**
  * \brief Set the PHY SAP User
  * \param s A pointer to the SAP user
  */
  void SetLteUePhySapUser (LteUePhySapUser* s);

  /**
   * \brief Get the CPHY SAP provider
   * \return A pointer to the SAP Provider
   */
  LteUeCphySapProvider* GetLteUeCphySapProvider ();

  /**
  * \brief Set the CPHY SAP User
  * \param s A pointer to the SAP user
  */
  void SetLteUeCphySapUser (LteUeCphySapUser* s);


  /**
   * \param pow The transmission power in dBm
   */
  void SetTxPower (double pow);

  /**
   * \return The transmission power in dBm
   */
  double GetTxPower () const;

  /**
   * \return The pointer to UE Uplink Power Control entity
   */
  Ptr<LteUePowerControl> GetUplinkPowerControl () const;

  /**
   * \param nf The noise figure in dB
   */
  void SetNoiseFigure (double nf);

  /**
   * \return The noise figure in dB
   */
  double GetNoiseFigure () const;

  /**
   * \return The TTI delay between MAC and channel
   */
  uint8_t GetMacChDelay (void) const;

  /**
   * \return A pointer to the LteSpectrumPhy instance relative to the downlink
   */
  Ptr<LteSpectrumPhy> GetDlSpectrumPhy () const;

  /**
   * \return A pointer to the LteSpectrumPhy instance relative to the uplink
   */
  Ptr<LteSpectrumPhy> GetUlSpectrumPhy () const;

  /**
   * \brief Create the PSD for the TX
   * \return The pointer to the PSD
   */
  virtual Ptr<SpectrumValue> CreateTxPowerSpectralDensity ();

  /**
   * \brief Set a list of sub channels to use in TX
   * \param mask A list of sub channels
   */
  void SetSubChannelsForTransmission (std::vector <int> mask);
  /**
   * \brief Get a list of sub channels to use in RX
   * \return A list of sub channels
   */
  std::vector <int> GetSubChannelsForTransmission (void);

  /**
   * \brief Get a list of sub channels to use in RX
   * \param mask A list of sub channels
   */
  void SetSubChannelsForReception (std::vector <int> mask);
  /**
   * \brief Get a list of sub channels to use in RX
   * \return A list of sub channels
   */
  std::vector <int> GetSubChannelsForReception (void);

  /**
  * \brief Create the DL CQI feedback from SINR values perceived at
  * the physical layer with the signal received from eNB
  * \param sinr SINR values vector
  * \return A DL CQI control message containing the CQI feedback
  */
  Ptr<DlCqiLteControlMessage> CreateDlCqiFeedbackMessage (const SpectrumValue& sinr);

  /**
   * set the spectrum phy instance for Sidelink reception
   * \param phy The Sidelink spectrum phy
   */
  void SetSlSpectrumPhy (Ptr<LteSpectrumPhy> phy);

  /**
   * \return A pointer to the LteSpectrumPhy instance relative to the Sidelink reception
   */
  Ptr<LteSpectrumPhy> GetSlSpectrumPhy () const;



  // inherited from LtePhy
  virtual void GenerateCtrlCqiReport (const SpectrumValue& sinr);
  virtual void GenerateDataCqiReport (const SpectrumValue& sinr);
  /**
  * \brief Create the mixed CQI report
  *
  * \param sinr The SINR values vector
  */
  virtual void GenerateMixedCqiReport (const SpectrumValue& sinr);
  virtual void ReportInterference (const SpectrumValue& interf);
  /**
  * \brief Create the mixed CQI report
  *
  * \param interf The interference values vector
  */
  virtual void ReportDataInterference (const SpectrumValue& interf);
  virtual void ReportRsReceivedPower (const SpectrumValue& power);

  // callbacks for LteSpectrumPhy
  /**
  * \brief Receive LTE control message list function
  *
  * \param msgList The LTE control message list
  */
  virtual void ReceiveLteControlMessageList (std::list<Ptr<LteControlMessage> > msgList);
  /**
  * \brief Receive PSS function
  *
  * \param cellId The cell ID
  * \param p The PSS list
  */
  virtual void ReceivePss (uint16_t cellId, Ptr<SpectrumValue> p);


  /**
   * \brief PhySpectrum received a new PHY-PDU
   * \param p The packet received
   */
  void PhyPduReceived (Ptr<Packet> p);


  /**
  * \brief trigger from eNB the start from a new frame
  *
  * \param frameNo The frame number
  * \param subframeNo The subframe number
  */
  void SubframeIndication (uint32_t frameNo, uint32_t subframeNo);


  /**
   * \brief Send the SRS signal in the last symbols of the frame
   */
  void SendSrs ();

  /**
   * \brief PhySpectrum generated a new DL HARQ feedback
   * \param mes The DlInfoListElement_s
   */
  virtual void ReceiveLteDlHarqFeedback (DlInfoListElement_s mes);

  /**
   * \brief Set the HARQ PHY module
   * \param harq The HARQ PHY module
   */
  void SetHarqPhyModule (Ptr<LteHarqPhy> harq);

  /**
   * \return The current state
   */
  State GetState () const;

  /**
   * Set the time in which the first SyncRef selection will be performed by the UE
   *
   * \param t The time to perform the first SyncRef selection (relative to the
   *          simulation time when the function is called, i.e., relative to 0, if it is called
   *          before the start of the simulation)
   *
   * \note The UE will never perform the SyncRef selection process if this function
   * is not called before the start of the simulation
   */
  void SetFirstScanningTime(Time t);
  /**
   * Get the time in which the first SyncRef selection will be performed
   * \return The time in which the first SyncRef selection will be performed by the UE
   */
  Time GetFirstScanningTime();
  /**
   * Stores the S-RSRP of the detected SLSSs during the SyncRef selection  process
   * (i.e., during SyncRef scanning or measurement)
   *
   * \param slssid The SLSSID of the SyncRef
   * \param p The received signal
   *
   * \note The S-RSRP is not stored if: it is below the minimum required, or the UE
   *       is not performing the SyncRef selection process
   *       When a SlSS is received LteSpectrumPhy is informed through a callback
   *       hooked in LteHelper
   */
  virtual void ReceiveSlss (uint16_t slssid, Ptr<SpectrumValue> p);

  /**
   * TracedCallback signature for state transition events.
   *
   * \param [in] cellId
   * \param [in] rnti
   * \param [in] oldState
   * \param [in] newState
   */
  typedef void (* StateTracedCallback)
    (uint16_t cellId, uint16_t rnti, State oldState, State newState);

  /**
   * TracedCallback signature for cell RSRP and SINR report.
   *
   * \param [in] cellId
   * \param [in] rnti
   * \param [in] rsrp
   * \param [in] sinr
   * \param [in] componentCarrierId
   */
  typedef void (* RsrpSinrTracedCallback)
    (uint16_t cellId, uint16_t rnti,
     double rsrp, double sinr, uint8_t componentCarrierId);

  /**
   * TracedCallback signature for cell RSRP and RSRQ.
   *
   * \param [in] rnti
   * \param [in] cellId
   * \param [in] rsrp
   * \param [in] rsrq
   * \param [in] isServingCell
   * \param [in] componentCarrierId
   */
  typedef void (* RsrpRsrqTracedCallback)
    (uint16_t rnti, uint16_t cellId, double rsrp, double rsrq,
     bool isServingCell, uint8_t componentCarrierId);

  /**
   * TracedCallback signature for transmission of discovery message.
   *
   * \param [in] cellId
   * \param [in] rnti
   * \param [in] proSeAppCode
   */
  typedef void (* DiscoveryAnnouncementTracedCallback)
      (const uint16_t cellId, const uint16_t rnti, const uint32_t proSeAppCode);

private:

  /**
   * Set transmit mode 1 gain function
   *
   * \param [in] gain
   */
  void SetTxMode1Gain (double gain);
  /**
   * Set transmit mode 2 gain function
   *
   * \param [in] gain
   */
  void SetTxMode2Gain (double gain);
  /**
   * Set transmit mode 3 gain function
   *
   * \param [in] gain
   */
  void SetTxMode3Gain (double gain);
  /**
   * Set transmit mode 4 gain function
   *
   * \param [in] gain
   */
  void SetTxMode4Gain (double gain);
  /**
   * Set transmit mode 5 gain function
   *
   * \param [in] gain
   */
  void SetTxMode5Gain (double gain);
  /**
   * Set transmit mode 6 gain function
   *
   * \param [in] gain
   */
  void SetTxMode6Gain (double gain);
  /**
   * Set transmit mode 7 gain function
   *
   * \param [in] gain
   */
  void SetTxMode7Gain (double gain);
  /**
   * Set Sidelink transmit gain function
   * Set the SIMO gain for Sidelink UE
   *
   * \param [in] gain
   */
  void SetSlRxGain (double gain);
  /**
   * Set transmit mode gain function
   *
   * \param [in] txMode
   * \param [in] gain
   */
  void SetTxModeGain (uint8_t txMode, double gain);

  /**
   * queue subchannels for transmission function
   *
   * \param [in] rbMap
   */
  void QueueSubChannelsForTransmission (std::vector <int> rbMap);


  /** 
   * internal method that takes care of generating CQI reports,
   * calculating the RSRP and RSRQ metrics, and generating RSRP+SINR traces
   * 
   * \param sinr 
   */
  void GenerateCqiRsrpRsrq (const SpectrumValue& sinr);


  /**
   * \brief Layer-1 filtering of RSRP and RSRQ measurements and reporting to
   *        the RRC entity.
   *
   * Initially executed at +0.200s, and then repeatedly executed with
   * periodicity as indicated by the *UeMeasurementsFilterPeriod* attribute.
   */
  void ReportUeMeasurements ();

  /**
   * Switch the UE PHY to the given state.
   * \param s The destination state
   */
  void SwitchToState (State s);

  /**
   * Get random system frame and subframe number
   * \param stream The first stream index to use
   * \return The number of stream indices assigned
   */
  int64_t AssignStreams (int64_t stream);



  // UE CPHY SAP methods
  /// Reset function
  void DoReset ();
  /**
   * Start the cell search function
   * \param dlEarfcn The DL EARFCN
   */
  void DoStartCellSearch (uint32_t dlEarfcn);
  /**
   * Synchronize with ENB function
   * \param cellId The cell ID
   */
  void DoSynchronizeWithEnb (uint16_t cellId);
  /**
   * Synchronize with ENB function
   * \param cellId The cell ID
   * \param dlEarfcn The DL EARFCN
   */
  void DoSynchronizeWithEnb (uint16_t cellId, uint32_t dlEarfcn);
  /**
   * Set DL bandwidth function
   * \param dlBandwidth The DL bandwidth
   */
  void DoSetDlBandwidth (uint8_t dlBandwidth);
  /**
   * Configure UL uplink function
   * \param ulEarfcn UL EARFCN
   * \param ulBandwidth The UL bandwidth
   */
  void DoConfigureUplink (uint32_t ulEarfcn, uint8_t ulBandwidth);
  /**
   * Configure reference signal power function
   * \param referenceSignalPower reference signal power in dBm
   */
  void DoConfigureReferenceSignalPower (int8_t referenceSignalPower);
  /**
   * Set RNTI function
   * \param rnti The RNTI
   */
  void DoSetRnti (uint16_t rnti);
  /**
   * Set transmission mode function
   * \param txMode The transmission mode
   */
  void DoSetTransmissionMode (uint8_t txMode);
  /**
   * Set SRS configuration index function
   * \param srcCi The SRS configuration index
   */
  void DoSetSrsConfigurationIndex (uint16_t srcCi);
  /**
   * Set PA function
   * \param pa The PA value
   */
  void DoSetPa (double pa);
  /**
   * \param rsrpFilterCoefficient value. Determines the strength of
   * smoothing effect induced by layer 3 filtering of RSRP
   * used for uplink power control in all attached UE.
   * If equals to 0, no layer 3 filtering is applicable.
   */
  void DoSetRsrpFilterCoefficient (uint8_t rsrpFilterCoefficient);

  //Sidelink discovery
  /**
   * Set Sidelink Tx Pool function
   *
   * \param pool The Sidelink Tx discovery resource pool
   */
  void DoSetSlDiscTxPool (Ptr<SidelinkTxDiscResourcePool> pool);
  /**
   * Set Sidelink Rx Pool function
   *
   * \param pools The Sidelink Rx discovery resource pools
   */
  void DoSetSlDiscRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools);
  /**
   * Remove Sidelink Discovery Tx Pool function
   */
  void DoRemoveSlDiscTxPool ();
  /**
   * Set discovery grant information function
   *
   * \param resPsdch A randomly chosen resource index from the PSDCH resource pool by UE MAC
   */
  void DoSetDiscGrantInfo (uint8_t resPsdch);
  /**
   * Add discovery transmission applications function
   *
   * \param apps The applications we are interested in announcing
   */
  void DoAddDiscTxApps (std::list<uint32_t> apps);
  /**
   * Add discovery reception applications function
   *
   * \param apps The applications we are interested in monitoring
   */
  void DoAddDiscRxApps (std::list<uint32_t> apps);

    //communication
  /**
   * Set Sidelink Tx Pool function
   *
   * \param pool The Sidelink Tx communication resource pool
   */
  void DoSetSlCommTxPool (Ptr<SidelinkTxCommResourcePool> pool);
  /**
   * Set Sidelink Rx Pool function
   *
   * \param pools The Sidelink Rx communication resource pools
   */
  void DoSetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools);
  /**
   * Remove Sidelink Communication Tx Pool function
   */
  void DoRemoveSlCommTxPool ();
  /**
   * Add Sidelink destination function
   *
   * \param destination The destination id
   */
  void DoAddSlDestination (uint32_t destination);
  /**
   * Remove Sidelink destination function
   *
   * \param destination The destination id
   */
    void DoRemoveSlDestination (uint32_t destination);

  // UE PHY SAP methods 
  virtual void DoSendMacPdu (Ptr<Packet> p);
  /**
   * Send LTE control message function
   * \param msg the LTE control message
   */
  virtual void DoSendLteControlMessage (Ptr<LteControlMessage> msg);
  /**
   * Send RACH preamble function
   * \param prachId The RACH preamble ID
   * \param raRnti The rnti
   */
  virtual void DoSendRachPreamble (uint32_t prachId, uint32_t raRnti);

  /// A list of sub channels to use in TX.
  std::vector <int> m_subChannelsForTransmission;
  /// A list of sub channels to use in RX.
  std::vector <int> m_subChannelsForReception;

  std::vector< std::vector <int> > m_subChannelsForTransmissionQueue; ///< subchannels for transmission queue


  Ptr<LteAmc> m_amc; ///< AMC

  /**
   * The `EnableUplinkPowerControl` attribute. If true, Uplink Power Control
   * will be enabled.
   */
  bool m_enableUplinkPowerControl;
  /// Pointer to UE Uplink Power Control entity.
  Ptr<LteUePowerControl> m_powerControl;

  /// Wideband Periodic CQI. 2, 5, 10, 16, 20, 32, 40, 64, 80 or 160 ms.
  Time m_p10CqiPeriodicity;
  Time m_p10CqiLast; ///< last periodic CQI

  /**
   * SubBand Aperiodic CQI. Activated by DCI format 0 or Random Access Response
   * Grant.
   * \note Defines a periodicity for academic studies.
   */
  Time m_a30CqiPeriodicity;
  Time m_a30CqiLast; ///< last aperiodic CQI

  LteUePhySapProvider* m_uePhySapProvider; ///< UE Phy SAP provider
  LteUePhySapUser* m_uePhySapUser; ///< UE Phy SAP user

  LteUeCphySapProvider* m_ueCphySapProvider; ///< UE CPhy SAP provider
  LteUeCphySapUser* m_ueCphySapUser; ///< UE CPhy SAP user

  uint16_t  m_rnti; ///< the RNTI
 
  uint8_t m_transmissionMode; ///< the transmission mode
  std::vector <double> m_txModeGain; ///< the transmit mode gain
  double m_slRxGain; ///< the Sidelink SIMO gain

  uint16_t m_srsPeriodicity; ///< SRS periodicity
  uint16_t m_srsSubframeOffset; ///< SRS subframe offset
  uint16_t m_srsConfigured; ///< SRS configured
  Time     m_srsStartTime; ///< SRS start time

  double m_paLinear; ///< PA linear

  bool m_dlConfigured; ///< DL configured?
  bool m_ulConfigured; ///< UL configured?

  /// The current UE PHY state.
  State m_state;
  Ptr<UniformRandomVariable> m_uniformRandomVariable; ///< Provides uniform random variables.
  /**
   * The `StateTransition` trace source. Fired upon every UE PHY state
   * transition. Exporting the serving cell ID, RNTI, old state, and new state.
   */
  TracedCallback<uint16_t, uint16_t, State, State> m_stateTransitionTrace;
  /**
   * The `DiscoveryMsgSent` trace source. Track the transmission of discovery message (announce)
   * Exporting the cellId, RNTI and ProSe App Code.
   */
  TracedCallback<uint16_t, uint16_t, uint32_t> m_discoveryAnnouncementTrace;

  /// \todo Can be removed.
  uint8_t m_subframeNo;

  bool m_rsReceivedPowerUpdated; ///< RS receive power updated?
  SpectrumValue m_rsReceivedPower; ///< RS receive power

  bool m_rsInterferencePowerUpdated; ///< RS interference power updated?
  SpectrumValue m_rsInterferencePower; ///< RS interference power

  bool m_dataInterferencePowerUpdated; ///< data interference power updated?
  SpectrumValue m_dataInterferencePower; ///< data interference power

  bool m_pssReceived; ///< PSS received?
  /// PssElement structure
  struct PssElement 
  {
    uint16_t cellId; ///< cell ID
    double pssPsdSum; ///< PSS PSD sum
    uint16_t nRB; ///< number of RB
  };
  std::list <PssElement> m_pssList; ///< PSS list

  /**
   * The `RsrqUeMeasThreshold` attribute. Receive threshold for PSS on RSRQ
   * in dB.
   */
  double m_pssReceptionThreshold;
  /**
   * The `RsrpUeMeasThreshold` attribute. Receive threshold for RSRP
   * in dB.
   */
  double m_rsrpReceptionThreshold;

  /// Summary results of measuring a specific cell. Used for layer-1 filtering.
  struct UeMeasurementsElement
  {
    double rsrpSum;   ///< Sum of RSRP sample values in linear unit.
    uint8_t rsrpNum;  ///< Number of RSRP samples.
    double rsrqSum;   ///< Sum of RSRQ sample values in linear unit.
    uint8_t rsrqNum;  ///< Number of RSRQ samples.
  };

  /**
   * Store measurement results during the last layer-1 filtering period.
   * Indexed by the cell ID where the measurements come from.
   */
  std::map <uint16_t, UeMeasurementsElement> m_ueMeasurementsMap;
  /**
   * The `UeMeasurementsFilterPeriod` attribute. Time period for reporting UE
   * measurements, i.e., the length of layer-1 filtering (default 200 ms).
   */
  Time m_ueMeasurementsFilterPeriod;
  /// \todo Can be removed.
  Time m_ueMeasurementsFilterLast;

  Ptr<LteHarqPhy> m_harqPhyModule; ///< HARQ phy module

  uint32_t m_raPreambleId; ///< RA preamble ID
  uint32_t m_raRnti; ///< RA rnti

  /**
   * The `ReportCurrentCellRsrpSinr` trace source. Trace information regarding
   * RSRP and average SINR (see TS 36.214). Exporting cell ID, RNTI, RSRP, and
   * SINR. Moreover it reports the m_componentCarrierId.
   */
  TracedCallback<uint16_t, uint16_t, double, double, uint8_t> m_reportCurrentCellRsrpSinrTrace;
  /**
   * The `RsrpSinrSamplePeriod` attribute. The sampling period for reporting
   * RSRP-SINR stats.
   */
  uint16_t m_rsrpSinrSamplePeriod;
  /**
   * The `RsrpSinrSampleCounter` attribute. The sampling counter for reporting
   * RSRP-SINR stats.
   */
  uint16_t m_rsrpSinrSampleCounter;

  /**
   * The `ReportUeMeasurements` trace source. Contains trace information
   * regarding RSRP and RSRQ measured from a specific cell (see TS 36.214).
   * Exporting RNTI, the ID of the measured cell, RSRP (in dBm), RSRQ (in dB),
   * and whether the cell is the serving cell. Moreover it report the m_componentCarrierId.
   */
  TracedCallback<uint16_t, uint16_t, double, double, bool, uint8_t> m_reportUeMeasurements;

  EventId m_sendSrsEvent; ///< send SRS event

  /**
   * The `UlPhyTransmission` trace source. Contains trace information regarding
   * PHY stats from UL Tx perspective. Exporting a structure with type
   * PhyTransmissionStatParameters.
   */
  TracedCallback<PhyTransmissionStatParameters> m_ulPhyTransmission;

  
  Ptr<SpectrumValue> m_noisePsd; ///< Noise power spectral density for
                                 ///the configured bandwidth 

  /**
   * The Sidelink LteSpectrumPhy associated to this LteUePhy.
   */
  Ptr<LteSpectrumPhy> m_sidelinkSpectrumPhy;

  Ptr<SpectrumValue> m_slNoisePsd; ///< Sidelink noise power spectral density for
                                   ///the configured bandwidth

  /**
   * If true the values of frameNo and subframeNo are selected randomly,
   * if false, frameNo=1 and subframeNo=1
   */
  bool m_chooseFrameAndSubframeRandomly;

  /// Sidelink communication grant related  parameters
 struct SidelinkGrant
 {
   //fields common with SL_DCI
   uint16_t m_rnti; ///< RNTI
   uint16_t m_resPscch; ///< PSCCH resource index
   uint8_t m_tpc; ///< TPC
   uint8_t m_hopping; ///< Hopping flag
   uint8_t m_rbStart; ///< models rb assignment
   uint8_t m_rbLen;   ///< models rb assignment
   uint8_t m_hoppingInfo; ///< models rb assignment when hopping is enabled
   uint8_t m_trp; ///< Time resourse pattern (TRP)
   uint8_t m_groupDstId; ///< Group destination id

   //other fields
   uint8_t m_mcs; ///< MCS
   uint32_t m_tbSize; ///< TB size

   uint32_t frameNo; ///< Frame number
   uint32_t subframeNo; ///< Subframe number
 };

 /// SidelinkGrantInfo structure
 struct SidelinkGrantInfo
 {
   SidelinkGrant m_grant; ///< Sidelink communication grant
   std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> m_pscchTx; ///< list of PSCCH transmissions within the pool
   std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> m_psschTx; ///< list of PSSCH transmissions within the pool
   bool m_grantReceived; ///< Flag to indicate the reception of Sidelink communication grant
 };

 /// Sidelink pool information
 struct PoolInfo
 {
   Ptr<SidelinkCommResourcePool> m_pool; ///< the pool
   SidelinkCommResourcePool::SubframeInfo m_currentScPeriod; ///< start of current period
   SidelinkCommResourcePool::SubframeInfo m_nextScPeriod; ///< start of next period
   uint32_t m_npscch; ///< number of PSCCH available in the pool
   std::map<uint16_t, SidelinkGrantInfo> m_currentGrants; ///< Current Sidelink communication grants
 };

 PoolInfo m_slTxPoolInfo; ///< Sidelink pool information
 std::list <PoolInfo> m_sidelinkRxPools; ///< List of Sidelink communication Rx pools
 std::list <uint32_t> m_destinations; ///< List of destinations for Sidelink communication

 /// Sidelink discovery grant
 struct DiscGrant
 {
   uint16_t m_rnti; ///< RNTI
   uint8_t m_resPsdch; ///< m_resPsdch A randomly chosen resource index from the PSDCH resource pool
 };

 /// Sidelink discovery grant info
 struct DiscGrantInfo
 {
   DiscGrant m_grant; ///< Sidelink discovery grant
   std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo> m_psdchTx; ///< List of PSDCH transmissions within the pool
   bool m_grantReceived; ///< Flag to indicate the reception of Sidelink discovery grant
 };

 /// Sidelink discovery pool information
 struct DiscPoolInfo
 {
   Ptr<SidelinkDiscResourcePool> m_pool; ///< the discover resource pool
   SidelinkDiscResourcePool::SubframeInfo m_currentDiscPeriod; ///< start of current period
   SidelinkDiscResourcePool::SubframeInfo m_nextDiscPeriod; ///< start of next period
   uint32_t m_npsdch; ///< number of PSDCH available in the pool
   std::map<uint16_t, DiscGrantInfo> m_currentGrants; ///< Current Sidelink discovery grants
 };

 DiscPoolInfo m_discTxPools; ///< Discovery Tx pool information
 std::list <DiscPoolInfo> m_discRxPools; ///< List of Sidelink discovery Rx pools

 uint8_t m_discResPsdch; ///< A randomly chosen resource index from the PSDCH resource pool by UE MAC

 std::list<uint32_t> m_discTxApps; ///< List of discovery Tx applications
 std::list<uint32_t> m_discRxApps; ///< List of discovery Rx applications

 /**
  * Summary results of measuring a specific SyncRef. Used for layer-1 filtering.
  */
 struct UeSlssMeasurementsElement
 {
   double srsrpSum;   ///< Sum of S-RSRP sample values in linear unit.
   uint8_t srsrpNum;  ///< Number of S-RSRP samples.
 };
 /**
  * Stores the S-RSRP information of the SyncRefs detected during the scanning process,
  * indexed by the SLSSID of the SyncRef and the offset it uses for transmitting the SLSSs
  */
 std::map <std::pair<uint16_t,uint16_t>, UeSlssMeasurementsElement> m_ueSlssDetectionMap;
 /**
  * Represents the S-RSRP measurement schedule for the current measurement process.
  * It is used for knowing when the UE needs to take samples of the detected SyncRefs S-RSRP.
  * The index is the simulation time and the elements are the SyncRef identifiers (SLSSID and offset)
  */
 std::map <int64_t, std::pair<uint16_t,uint16_t> > m_ueSlssMeasurementsSched;
 /**
  * Stores the S-RSRP information of the SyncRefs in measurement during the measurement process
  * indexed by the SLSSID of the SyncRef and the offset it uses for transmitting the SLSSs
  */
 std::map <std::pair<uint16_t,uint16_t>, UeSlssMeasurementsElement> m_ueSlssMeasurementsMap;
 /**
  * Time period for searching/scanning to detect available SyncRefs (supporting SyncRef selection)
  */
 Time m_ueSlssScanningPeriod;
 /**
  * Time period for taking S-RSRP samples of the detected SyncRefs (supporting SyncRef selection)
  */
 Time m_ueSlssMeasurementPeriod;
 /**
  * Time period for taking S-RSRP samples of the selected SyncRef (supporting SLSS transmission decision)
  */
 Time m_ueSlssEvaluationPeriod;
 /**
  *  The number of samples the UE takes during the measurement and evaluation periods
  */
 uint16_t m_nSamplesSrsrpMeas;
 /**
  * Time for the first SyncRef selection period
  */
 Time m_tFirstScanning;
 /**
  * Random number generator used for determining the time between SyncRef selection processes
  */
 Ptr<UniformRandomVariable> m_nextScanRdm;
 /**
  * True if a SyncRef selection is in progress and the UE is performing the SyncRef search/scanning
  */
 bool m_ueSlssScanningInProgress;
 /**
  * True if a SyncRef selection is in progress and the UE is performing the S-RSRP measurements,
  * either of the detected SyncRefs (measurement sub-process) or the selected one (evaluation sub-process)
  */
 bool m_ueSlssMeasurementInProgress;
 /**
  * Number of S-RSRP measurement periods already performed in SyncRef selection process in progress
  * (1 = measurement, 2 = measurement + evaluation)
  */
 uint16_t m_currNMeasPeriods;
 /**
  * The minimum S-RSRP value for considering a SyncRef S-RSRP sample valid
  */
 double m_minSrsrp;
 /**
  * Stores the received MIB-SL during the SyncRef search/scanning.
  * Used to determine the detected SyncRefs, as SyncRefs with valid S-RSRP but without
  * successfully decoded/received MIB-SL are not considered detected
  */
 std::map <std::pair<uint16_t,uint16_t>, LteRrcSap::MasterInformationBlockSL> m_detectedMibSl;
 /**
  * Initial frame number
  */
 uint32_t m_initFrameNo;
 /**
  * Initial subframe number
  */
 uint32_t m_initSubframeNo;
 /**
  * Current frame number
  */
 uint32_t m_currFrameNo;
 /**
  * Current subframe number
  */
 uint32_t m_currSubframeNo;
 /**
  * Configuration needed for the timely change of subframe indication upon synchronization to
  * a different SyncRef
  */
 struct ResyncParams
 {
   uint16_t newSubframeNo; ///< Subframe number
   uint16_t newFrameNo; ///< frame number
   LteRrcSap::MasterInformationBlockSL syncRefMib; ///< MIB
 };
 /**
  * Parameters to be used for the change of subframe indication upon synchronization to
  * a different SyncRef
  */
 ResyncParams m_resyncParams;
 /**
  * True if the RRC instructed to synchronize to a different SyncRef
  */
 bool m_resyncRequested;
 /**
  * True if the UE changed of timing (synchronized to a different SyncRef) and have to
  * wait the start of a new sideliink communication period for transmitting the data
  * (the subframe indication changed and the data indication in the SCI of the current
  * period is not valid anymore)
  */
 bool m_waitingNextScPeriod;

 /**
  * Set the upper limit for the random values generated by m_nextScanRdm
  *
  * \param t The upper limit for m_nextScanRdm
  */
 void SetUeSlssInterScanningPeriodMax(Time t);
 /**
  * Set the lower limit for the random values generated by m_nextScanRdm
  *
  * \param t The lower limit for m_nextScanRdm
  */
 void SetUeSlssInterScanningPeriodMin(Time t);
 /**
  * Notify the start of a new SyncRef selection process, starting with the
  * SyncRef search/scanning
  */
 void StartSlssScanning ();
 /**
  * Notify the end of the SyncRef search/scanning,
  * keep only the six detected SyncRef (with received MIB-SL) with highest S-RSRP,
  * create the S-RSRP measurement schedule for each of them and start the measurement
  * process
  */
 void EndSlssScanning ();
 /**
  * Notify the start of a S-RSRP measurement process.
  * The S-RSRP measurement is used for two sub-processes:
  * 1. Measurement: collect the S-RSRP samples of the detected SyncRefs for determining
  *    the suitable SyncRef to select and synchronize with, and
  * 2. Evaluation: collect the S-RSRP of the selected SyncRef (if any) to determine if the UE needs
  *    to become itself a SyncRef and start transmitting SLSS
  *
  * \param slssid The SLSSID of the selected SyncRef if the function is called for Evaluation,
  *               0 if it is called for Measurement
  * \param offset The offset in which the selected SyncRef sends SLSS if the function is called
  *               for Evaluation, 0 if it is called for Measurement
  */
 void StartSlssMeasurements (uint64_t slssid, uint16_t offset);
 /**
  * Perform L1 filtering of the S-RSRP samples collected during the measurement process
  * for each SyncRef, and report them to the RRC
  *
  * \param slssid The SLSSID of the selected SyncRef if the measurement process was called for
  *               Evaluation, 0 if was is called for Measurement
  * \param offset The offset in which the selected SyncRef sends SLSS if the process was called
  *               for Evaluation, 0 if it was called for Measurement
  */
 void ReportSlssMeasurements (uint64_t slssid,uint16_t offset);
 /**
  * Schedule the next SyncRef selection process.
  * The function is called at the end of the SyncRef selection process in progress
  *
  * \param endOfPrevious This indicates after which sub-process the SyncRef selection process ended:
  *                      0 if it ended after scanning, 1 if it ended after measurement,
  *                      or 2 if it ends after evaluation
  */
 void ScheduleNextSyncRefReselection(uint16_t endOfPrevious);
 /**
  * Apply the change of timing (change of frame/subframe indication) when appropriate.
  * The change of timing is instructed when the UE selected and wants to synchronize to a given
  * SyncRef. The change is applied immediately upon resynchronization request if the UE is
  * not transmitting Sidelink communication at the moment. Otherwise, the change is delayed until
  * the end of the current Sidelink communication period to avoid the loss of already scheduled
  * transmissions (the subframe indication will change, and the data indication in the SCI
  * of the current period will not be valid anymore).
  *
  * \param frameNo The current frame number
  * \param subframeNo The current subframe number
  * \return True if change of timings performed successfully
  */
 bool ChangeOfTiming(uint32_t frameNo, uint32_t subframeNo);

 // UE CPHY SAP methods related to synchronization
 /**
  * Do Set Sidelink synchronization id function
  * The RRC set the SLSSID value for lower layers
  *
  * \param slssid The Sidelink synchronization id
  */
 void DoSetSlssId(uint64_t slssid);
 /**
  * Do Send Sidelink synchronization signal function
  * The RRC instructs the PHY to send a MIB-SL in the PSBCH
  *
  * \param mibSl The Sidelink Master Information Block
  */
 void DoSendSlss (LteRrcSap::MasterInformationBlockSL mibSl);
 /**
  * Do synchronize to the SyncRef function
  * The RRC instructs the PHY to synchronize to a given SyncRef and apply the corresponding change of timing
  *
  * \param mibSl The Sidelink Master Information Block
  */
 void DoSynchronizeToSyncRef (LteRrcSap::MasterInformationBlockSL mibSl);

}; // end of `class LteUePhy`


}

#endif /* LTE_UE_PHY_H */
