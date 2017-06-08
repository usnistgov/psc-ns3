/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 CTTC
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
 *         Giuseppe Piro  <g.piro@poliba.it>
 * Modified by: Marco Miozzo <mmiozzo@cttc.es> (introduce physical error model)
 * Modified by: NIST
 */

#ifndef NIST_LTE_SPECTRUM_PHY_H
#define NIST_LTE_SPECTRUM_PHY_H

#include <ns3/event-id.h>
#include <ns3/spectrum-value.h>
#include <ns3/mobility-model.h>
#include <ns3/packet.h>
#include <ns3/nstime.h>
#include <ns3/net-device.h>
#include <ns3/spectrum-phy.h>
#include <ns3/spectrum-channel.h>
#include <ns3/spectrum-interference.h>
#include <ns3/data-rate.h>
#include <ns3/generic-phy.h>
#include <ns3/packet-burst.h>
#include <ns3/nist-lte-interference.h>
#include <ns3/nist-lte-sl-interference.h>
#include <ns3/nist-lte-phy-error-model.h>
#include "ns3/random-variable-stream.h"
#include <map>
#include <ns3/nist-ff-mac-common.h>
#include <ns3/nist-lte-harq-phy.h>
#include <ns3/nist-lte-common.h>
#include <ns3/nist-sl-pool.h>

namespace ns3 {

  /** Structure to identify and transport block */
struct NistTbId_t
{
  uint16_t m_rnti;
  uint8_t m_layer;
  
  public:
  NistTbId_t ();
  NistTbId_t (const uint16_t a, const uint8_t b);
  
  friend bool operator == (const NistTbId_t &a, const NistTbId_t &b);
  friend bool operator < (const NistTbId_t &a, const NistTbId_t &b);
};

  
struct NisttbInfo_t
{
  uint8_t ndi;
  uint16_t size;
  uint8_t mcs;
  std::vector<int> rbBitmap;
  uint8_t harqProcessId;
  uint8_t rv;
  double mi;
  bool downlink;
  bool corrupt;
  bool harqFeedbackSent;
  double sinr; //mean SINR
};

typedef std::map<NistTbId_t, NisttbInfo_t> expectedTbs_t;

struct NistSlTbId_t
{
  uint16_t m_rnti; //source SL-RNTI
  uint8_t m_l1dst; //layer 1 group Id
  
  public:
  NistSlTbId_t ();
  NistSlTbId_t (const uint16_t a, const uint8_t b);
  
  friend bool operator == (const NistSlTbId_t &a, const NistSlTbId_t &b);
  friend bool operator < (const NistSlTbId_t &a, const NistSlTbId_t &b);
};

  
struct NistSltbInfo_t
{
  uint8_t ndi;
  uint16_t size;
  uint8_t mcs;
  std::vector<int> rbBitmap;
  //uint8_t harqProcessId;
  uint8_t rv;
  double mi;
  //bool downlink;
  bool corrupt;
  bool harqFeedbackSent;
  double sinr; //mean SINR
};

typedef std::map<NistSlTbId_t, NistSltbInfo_t> expectedSlTbs_t;

struct NistDiscTbId_t
{
  uint16_t m_rnti; //source SL-RNTI
  uint8_t m_resPsdch; 
  
  public:
  NistDiscTbId_t ();
  NistDiscTbId_t (const uint16_t a, const uint8_t b);
  
  friend bool operator == (const NistDiscTbId_t &a, const NistDiscTbId_t &b);
  friend bool operator < (const NistDiscTbId_t &a, const NistDiscTbId_t &b);
  
};

struct NistDisctbInfo_t
{
  uint8_t ndi;
  uint8_t resPsdch;
  std::vector<int> rbBitmap;
  uint8_t rv;
  double mi;
  bool corrupt;
  bool harqFeedbackSent;
  double sinr; //mean SINR
};

typedef std::map<NistDiscTbId_t, NistDisctbInfo_t> expectedDiscTbs_t;

class NistLteNetDevice;
class AntennaModel;
class NistLteControlMessage;
struct NistLteSpectrumSignalParametersDataFrame;
struct NistLteSpectrumSignalParametersDlCtrlFrame;
struct NistLteSpectrumSignalParametersUlSrsFrame;
struct NistLteSpectrumSignalParametersSlFrame;

/**
 * Structure for sidelink packets being received
 */
struct NistSlRxPacketInfo_t
{
  std::vector<int> rbBitmap;
  Ptr<PacketBurst> m_rxPacketBurst;
  Ptr<NistLteControlMessage> m_rxControlMessage;
};

struct NistSlCtrlPacketInfo_t
{
  double sinr;
  int index;

  friend bool operator == (const NistSlCtrlPacketInfo_t &a, const NistSlCtrlPacketInfo_t &b);
  friend bool operator < (const NistSlCtrlPacketInfo_t &a, const NistSlCtrlPacketInfo_t &b);
};

/**
* this method is invoked by the NistLteSpectrumPhy to notify the PHY that the
* transmission of a given packet has been completed.
*
* @param packet the Packet whose TX has been completed.
*/
typedef Callback< void, Ptr<const Packet> > NistLtePhyTxEndCallback;

/**
* This method is used by the NistLteSpectrumPhy to notify the PHY that a
* previously started RX attempt has terminated without success
*/
typedef Callback< void > NistLtePhyRxDataEndErrorCallback;
/**
* This method is used by the NistLteSpectrumPhy to notify the PHY that a
* previously started RX attempt has been successfully completed.
*
* @param packet the received Packet
*/
typedef Callback< void, Ptr<Packet> > NistLtePhyRxDataEndOkCallback;


/**
* This method is used by the NistLteSpectrumPhy to notify the PHY that a
* previously started RX of a control frame attempt has been 
* successfully completed.
*
* @param packet the received Packet
*/
typedef Callback< void, std::list<Ptr<NistLteControlMessage> > > NistLtePhyRxCtrlEndOkCallback;

/**
* This method is used by the NistLteSpectrumPhy to notify the PHY that a
* previously started RX of a control frame attempt has terminated 
* without success.
*/
typedef Callback< void > NistLtePhyRxCtrlEndErrorCallback;

/**
* This method is used by the NistLteSpectrumPhy to notify the UE PHY that a
* PSS has been received
*/
typedef Callback< void, uint16_t, Ptr<SpectrumValue> > NistLtePhyRxPssCallback;

/**
* This method is used by the NistLteSpectrumPhy to notify the UE PHY that a
* SLSS has been received
*/
typedef Callback< void, uint16_t, Ptr<SpectrumValue> > NistLtePhyRxSlssCallback;

/**
* This method is used by the NistLteSpectrumPhy to notify the PHY about
* the status of a certain DL HARQ process
*/
typedef Callback< void, NistDlInfoListElement_s > NistLtePhyDlHarqFeedbackCallback;

/**
* This method is used by the NistLteSpectrumPhy to notify the PHY about
* the status of a certain UL HARQ process
*/
typedef Callback< void, NistUlInfoListElement_s > NistLtePhyUlHarqFeedbackCallback;



/**
 * \ingroup lte
 *
 * The NistLteSpectrumPhy models the physical layer of LTE
 *
 * It supports a single antenna model instance which is
 * used for both transmission and reception.  
 */
class NistLteSpectrumPhy : public SpectrumPhy
{

public:
  NistLteSpectrumPhy ();
  virtual ~NistLteSpectrumPhy ();

  /**
   *  PHY states
   */
  enum State
  {
    IDLE, TX, RX_DATA, RX_CTRL
  };

  // inherited from Object
  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  // inherited from SpectrumPhy
  void SetChannel (Ptr<SpectrumChannel> c);
  void SetMobility (Ptr<MobilityModel> m);
  void SetDevice (Ptr<NetDevice> d);
  Ptr<MobilityModel> GetMobility ();
  Ptr<NetDevice> GetDevice ();
  Ptr<const SpectrumModel> GetRxSpectrumModel () const;
  Ptr<AntennaModel> GetRxAntenna ();
  void StartRx (Ptr<SpectrumSignalParameters> params);
  void StartRxData (Ptr<NistLteSpectrumSignalParametersDataFrame> params);
  void StartRxCtrl (Ptr<SpectrumSignalParameters> params);
  void StartRxSlData (Ptr<NistLteSpectrumSignalParametersSlFrame> params);

  void SetHarqPhyModule (Ptr<NistLteHarqPhy> harq);

  /**
   * set the Power Spectral Density of outgoing signals in W/Hz.
   *
   * @param txPsd
   */
  void SetTxPowerSpectralDensity (Ptr<SpectrumValue> txPsd);

  /**
   * \brief set the noise power spectral density
   * @param noisePsd the Noise Power Spectral Density in power units
   * (Watt, Pascal...) per Hz.
   */
  void SetNoisePowerSpectralDensity (Ptr<const SpectrumValue> noisePsd);

  /** 
   * reset the internal state
   * 
   */
  void Reset ();

  /**
   * Clear expected SL TBs
   *
   */
  void ClearExpectedSlTb ();
 
  /**
   * set the AntennaModel to be used
   * 
   * \param a the Antenna Model
   */
  void SetAntenna (Ptr<AntennaModel> a);
  
  /**
  * Start a transmission of data frame in DL and UL
  *
  *
  * @param pb the burst of packets to be transmitted in PDSCH/PUSCH
  * @param ctrlMsgList the list of NistLteControlMessage to send
  * @param duration the duration of the data frame 
  *
  * @return true if an error occurred and the transmission was not
  * started, false otherwise.
  */
  bool StartTxDataFrame (Ptr<PacketBurst> pb, std::list<Ptr<NistLteControlMessage> > ctrlMsgList, Time duration);
  
  /**
  * Start a transmission of data frame in DL and UL
  *
  *
  * @param pb the burst of packets to be transmitted in PDSCH/PUSCH
  * @param ctrlMsgList the list of NistLteControlMessage to send
  * @param duration the duration of the data frame 
  *
  * @return true if an error occurred and the transmission was not
  * started, false otherwise.
  */
  bool StartTxSlDataFrame (Ptr<PacketBurst> pb, std::list<Ptr<NistLteControlMessage> > ctrlMsgList, Time duration, uint8_t groupId);

  /**
  * Start a transmission of control frame in DL
  *
  *
  * @param ctrlMsgList the burst of control messages to be transmitted
  * @param pss the flag for transmitting the primary synchronization signal
  *
  * @return true if an error occurred and the transmission was not
  * started, false otherwise.
  */
  bool StartTxDlCtrlFrame (std::list<Ptr<NistLteControlMessage> > ctrlMsgList, bool pss);
  
  /**
  * Start a transmission of control frame in UL
  *
  * @return true if an error occurred and the transmission was not
  * started, false otherwise.
  */
  bool StartTxUlSrsFrame ();


  /**
   * set the callback for the end of a TX, as part of the
   * interconnections between the PHY and the MAC
   *
   * @param c the callback
   */
  void SetNistLtePhyTxEndCallback (NistLtePhyTxEndCallback c);

  /**
   * set the callback for the end of a RX in error, as part of the
   * interconnections between the PHY and the MAC
   *
   * @param c the callback
   */
  void SetNistLtePhyRxDataEndErrorCallback (NistLtePhyRxDataEndErrorCallback c);

  /**
   * set the callback for the successful end of a RX, as part of the
   * interconnections between the PHY and the MAC
   *
   * @param c the callback
   */
  void SetNistLtePhyRxDataEndOkCallback (NistLtePhyRxDataEndOkCallback c);
  
  /**
  * set the callback for the successful end of a RX ctrl frame, as part 
  * of the interconnections between the NistLteSpectrumPhy and the PHY
  *
  * @param c the callback
  */
  void SetNistLtePhyRxCtrlEndOkCallback (NistLtePhyRxCtrlEndOkCallback c);
  
  /**
  * set the callback for the erroneous end of a RX ctrl frame, as part 
  * of the interconnections between the NistLteSpectrumPhy and the PHY
  *
  * @param c the callback
  */
  void SetNistLtePhyRxCtrlEndErrorCallback (NistLtePhyRxCtrlEndErrorCallback c);

  /**
  * set the callback for the reception of the PSS as part
  * of the interconnections between the NistLteSpectrumPhy and the UE PHY
  *
  * @param c the callback
  */
  void SetNistLtePhyRxPssCallback (NistLtePhyRxPssCallback c);

  /**
  * set the callback for the DL HARQ feedback as part of the 
  * interconnections between the NistLteSpectrumPhy and the PHY
  *
  * @param c the callback
  */
  void SetNistLtePhyDlHarqFeedbackCallback (NistLtePhyDlHarqFeedbackCallback c);

  /**
  * set the callback for the UL HARQ feedback as part of the
  * interconnections between the NistLteSpectrumPhy and the PHY
  *
  * @param c the callback
  */
  void SetNistLtePhyUlHarqFeedbackCallback (NistLtePhyUlHarqFeedbackCallback c);

  /**
   * \brief Set the state of the phy layer
   * \param newState the state
   */
  void SetState (State newState);

  /** 
   * 
   * 
   * \param cellId the Cell Identifier
   */
  void SetCellId (uint16_t cellId);

  /** 
   * \brief Add a new L1 group for filtering
   * 
   * \param groupId the L1 Group Identifier
   */
  void AddL1GroupId (uint8_t groupId);

  /** 
   * \brief Remove a new L1 group for filtering
   * 
   * \param groupId the L1 Group Identifier
   */
  void RemoveL1GroupId (uint8_t groupId);

  /**
  *
  *
  * \param p the new NistLteChunkProcessor to be added to the RS power
  *          processing chain
  */
  void AddRsPowerChunkProcessor (Ptr<NistLteChunkProcessor> p);
  
  /**
  *
  *
  * \param p the new NistLteChunkProcessor to be added to the Data Channel power
  *          processing chain
  */
  void AddDataPowerChunkProcessor (Ptr<NistLteChunkProcessor> p);

  /** 
  * 
  * 
  * \param p the new NistLteChunkProcessor to be added to the data processing chain
  */
  void AddDataSinrChunkProcessor (Ptr<NistLteChunkProcessor> p);

  /**
  *  NistLteChunkProcessor devoted to evaluate interference + noise power
  *  in control symbols of the subframe
  *
  * \param p the new NistLteChunkProcessor to be added to the data processing chain
  */
  void AddInterferenceCtrlChunkProcessor (Ptr<NistLteChunkProcessor> p);

  /**
  *  NistLteChunkProcessor devoted to evaluate interference + noise power
  *  in data symbols of the subframe
  *
  * \param p the new NistLteChunkProcessor to be added to the data processing chain
  */
  void AddInterferenceDataChunkProcessor (Ptr<NistLteChunkProcessor> p);  
  
  /** 
  * 
  * 
  * \param p the new NistLteChunkProcessor to be added to the ctrl processing chain
  */
  void AddCtrlSinrChunkProcessor (Ptr<NistLteChunkProcessor> p);

  /** 
  * 
  * 
  * \param p the new NistLteSlChunkProcessor to be added to the sidelink processing chain
  */
  void AddSlSinrChunkProcessor (Ptr<NistLteSlChunkProcessor> p);

  /** 
  * 
  * 
  * \param p the new NistLteSlChunkProcessor to be added to the sidelink processing chain
  */
  void AddSlSignalChunkProcessor (Ptr<NistLteSlChunkProcessor> p);
 
  /** 
  * 
  * 
  * \param p the new NistLteSlChunkProcessor to be added to the sidelink processing chain
  */
  void AddSlInterferenceChunkProcessor (Ptr<NistLteSlChunkProcessor> p);
   
  /** 
  * 
  * 
  * \param rnti the rnti of the source of the TB
  * \param ndi new data indicator flag
  * \param size the size of the TB
  * \param mcs the MCS of the TB
  * \param map the map of RB(s) used
  * \param layer the layer (in case of MIMO tx)
  * \param harqId the id of the HARQ process (valid only for DL)
  * \param downlink true when the TB is for DL
  */
  void AddExpectedTb (uint16_t  rnti, uint8_t ndi, uint16_t size, uint8_t mcs, std::vector<int> map, uint8_t layer, uint8_t harqId, uint8_t rv, bool downlink);

  /** 
  * 
  * 
  * \param rnti the rnti of the source of the TB
  * \param l1dst the layer 1 destination id of the TB
  * \param ndi new data indicator flag
  * \param size the size of the TB
  * \param mcs the MCS of the TB
  * \param map the map of RB(s) used
  */
  void AddExpectedTb (uint16_t  rnti, uint8_t l1dst, uint8_t ndi, uint16_t size, uint8_t mcs, std::vector<int> map, uint8_t rv);

  /**
   * for discovery
   * no mcs, size fixe to 232, no l1dst
   * \param rnti the rnti of the source of the TB
   * \param resPsdch indetifier
   * \param ndi new data indicator flag
   * \param map map of RBs used
   * \param rv revision
   */
  void AddExpectedTb (uint16_t  rnti, uint8_t resPsdch, uint8_t ndi, std::vector<int> map, uint8_t rv);


  /** 
  * 
  * 
  * \param sinr vector of sinr perceived per each RB
  */
  void UpdateSinrPerceived (const SpectrumValue& sinr);

  /** 
  * 
  * 
  * \param sinr vector of sinr perceived per each RB per sidelink packet
  */
  void UpdateSlSinrPerceived (std::vector <SpectrumValue> sinr);
 
  /** 
  * 
  * 
  * \param sinr vector of signal perceived per each RB per sidelink packet
  */
  void UpdateSlSigPerceived (std::vector <SpectrumValue> signal);

  /** 
  * 
  * 
  * \param sinr vector of interference perceived per each RB per sidelink packet
  */
  void UpdateSlIntPerceived (std::vector <SpectrumValue> interference);
  
  /** 
  * 
  * 
  * \param txMode UE transmission mode (SISO, MIMO tx diversity, ...)
  */
  void SetTransmissionMode (uint8_t txMode);
  
  /** 
   * 
   * \return the previously set channel
   */
  Ptr<SpectrumChannel> GetChannel ();

   /**
    * Set the slssid of the SyncRef to which the UE is synchronized
    * \param slssid the SyncRef identifier
    */
   void SetSlssid (uint64_t slssid);
   /**
    * set the callback for the reception of the SLSS as part
    * of the interconnections between the NistLteSpectrumPhy and the UE PHY
    *
    * @param c the callback
    */
   void SetNistLtePhyRxSlssCallback (NistLtePhyRxSlssCallback c);

  friend class NistLteUePhy;
  
 /**
  * Assign a fixed random variable stream number to the random variables
  * used by this model.  Return the number of streams (possibly zero) that
  * have been assigned.
  *
  * \param stream first stream index to use
  * \return the number of stream indices assigned by this model
  */
  int64_t AssignStreams (int64_t stream);
  State GetState ();

  void SetRxPool (Ptr<SidelinkDiscResourcePool> newpool);

  void AddDiscTxApps (std::list<uint32_t> apps);
  void AddDiscRxApps (std::list<uint32_t> apps);
     
  void SetDiscNumRetx (uint8_t retx);

private:
  void ChangeState (State newState);
  void EndTx ();
  void EndRxData ();
  void EndRxDlCtrl ();
  void EndRxUlSrs ();
  void EndRxSlData ();
  
  void SetTxModeGain (uint8_t txMode, double gain);
  
  double GetMeanSinr (const SpectrumValue& sinr, const std::vector<int>& map);
  
  bool FilterRxApps (NistSlDiscMsg disc);

  Ptr<MobilityModel> m_mobility;
  Ptr<AntennaModel> m_antenna;
  Ptr<NetDevice> m_device;

  Ptr<SpectrumChannel> m_channel;

  Ptr<const SpectrumModel> m_rxSpectrumModel;
  Ptr<SpectrumValue> m_txPsd;
  Ptr<PacketBurst> m_txPacketBurst;
  std::list<Ptr<PacketBurst> > m_rxPacketBurstList;
  
  std::list<Ptr<NistLteControlMessage> > m_txControlMessageList;
  std::list<Ptr<NistLteControlMessage> > m_rxControlMessageList;
    
  State m_state;
  Time m_firstRxStart;
  Time m_firstRxDuration;

  TracedCallback<Ptr<const PacketBurst> > m_phyTxStartTrace;
  TracedCallback<Ptr<const PacketBurst> > m_phyTxEndTrace;
  TracedCallback<Ptr<const PacketBurst> > m_phyRxStartTrace;
  TracedCallback<Ptr<const Packet> > m_phyRxEndOkTrace;
  TracedCallback<Ptr<const Packet> > m_phyRxEndErrorTrace;

  NistLtePhyTxEndCallback        m_ltePhyTxEndCallback;
  NistLtePhyRxDataEndErrorCallback   m_ltePhyRxDataEndErrorCallback;
  NistLtePhyRxDataEndOkCallback      m_ltePhyRxDataEndOkCallback;
  
  NistLtePhyRxCtrlEndOkCallback     m_ltePhyRxCtrlEndOkCallback;
  NistLtePhyRxCtrlEndErrorCallback  m_ltePhyRxCtrlEndErrorCallback;
  NistLtePhyRxPssCallback  m_ltePhyRxPssCallback;

  Ptr<NistLteInterference> m_interferenceData;
  Ptr<NistLteInterference> m_interferenceCtrl;

  uint16_t m_cellId;
  
  expectedTbs_t m_expectedTbs;
  expectedDiscTbs_t m_expectedDiscTbs;
  SpectrumValue m_sinrPerceived;

  // Information for sidelink communication
  Ptr<NistLteSlInterference> m_interferenceSl;
  std::set<uint8_t> m_l1GroupIds; // identifiers for D2D layer 1 filtering
  expectedSlTbs_t m_expectedSlTbs;  
  std::vector<SpectrumValue> m_slSinrPerceived; //SINR for each D2D packet received
  std::vector<SpectrumValue> m_slSignalPerceived; //Signal for each D2D packet received
  std::vector<SpectrumValue> m_slInterferencePerceived; //Interference for each D2D packet received
  //std::map<Ptr<NistLteControlMessage>, std::vector <int> > m_rxControlMessageRbMap;
  std::vector<NistSlRxPacketInfo_t> m_rxPacketInfo;

  /// Provides uniform random variables.
  Ptr<UniformRandomVariable> m_random;
  bool m_dataErrorModelEnabled; // when true (default) the phy error model is enabled
  bool m_ctrlErrorModelEnabled; // when true (default) the phy error model is enabled for DL ctrl frame
  
  bool m_ctrlFullDuplexEnabled; // when true the PSCCH operates in Full Duplexmode (disabled by default).
  
  bool m_dropRbOnCollisionEnabled; //when true, drop all receptions on colliding RBs regardless SINR value.

  /// NIST Physical error model
  bool m_nistErrorModelEnabled; // when true (not default) use NIST error model
  NistLtePhyErrorModel::NistLteFadingModel m_fadingModel;

  bool m_slBlerEnabled; //(true by default) when false BLER in the PSSCH is not modeled.
  
  uint8_t m_transmissionMode; // for UEs: store the transmission mode
  uint8_t m_layersNum;
  std::vector <double> m_txModeGain; // duplicate value of NistLteUePhy
  
  bool m_ulDataSlCheck;
  
  Ptr<NistLteHarqPhy> m_harqPhyModule;
  NistLtePhyDlHarqFeedbackCallback m_ltePhyDlHarqFeedbackCallback;
  NistLtePhyUlHarqFeedbackCallback m_ltePhyUlHarqFeedbackCallback;

  Ptr<NistLteSpectrumPhy> m_halfDuplexPhy;
  bool m_errorModelHarqD2dDiscoveryEnabled;
  
  std::list< Ptr<SidelinkDiscResourcePool> > m_discRxPools;
  
  std::list<uint32_t> m_discTxApps;
  std::list<uint32_t> m_discRxApps;
  
  /**
   * Trace information regarding PHY stats from DL Rx perspective
   * NistPhyReceptionStatParameters (see nist-lte-common.h)
   */
  TracedCallback<NistPhyReceptionStatParameters> m_dlPhyReception;

  
  /**
   * Trace information regarding PHY stats from UL Rx perspective
   * NistPhyReceptionStatParameters (see nist-lte-common.h)
   */
  TracedCallback<NistPhyReceptionStatParameters> m_ulPhyReception;

  /**
   * Trace information regarding PHY stats from UL Rx perspective
   * NistPhyReceptionStatParameters (see nist-lte-common.h)
   */
  TracedCallback<NistPhyReceptionStatParameters> m_slPhyReception;

  /**
   * Trace information regarding PHY stats from SL Rx PSCCH perspective
   * NistPhyReceptionStatParameters (see nist-lte-common.h)
   */
  TracedCallback<NistPhyReceptionStatParameters> m_slPscchReception;
  
  EventId m_endTxEvent;
  EventId m_endRxDataEvent;
  EventId m_endRxDlCtrlEvent;
  EventId m_endRxUlSrsEvent;

  /**
   * The Sidelink Synchronization Signal Identifier (SLSSID)
   */
  uint64_t m_slssId;
  /**
   * Callback used to notify the PHY about the reception of a SLSS
   */
  NistLtePhyRxSlssCallback  m_ltePhyRxSlssCallback;
};






}

#endif /* NIST_LTE_SPECTRUM_PHY_H */
