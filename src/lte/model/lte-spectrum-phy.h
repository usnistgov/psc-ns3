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
 *              NIST // Contributions may not be subject to US copyright. (D2D extensions)
 */

#ifndef LTE_SPECTRUM_PHY_H
#define LTE_SPECTRUM_PHY_H

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
#include <ns3/lte-interference.h>
#include <ns3/lte-sl-interference.h>
#include <ns3/lte-nist-error-model.h>
#include "ns3/random-variable-stream.h"
#include <map>
#include <ns3/ff-mac-common.h>
#include <ns3/lte-harq-phy.h>
#include <ns3/lte-sl-harq-phy.h>
#include <ns3/lte-common.h>
#include <ns3/lte-sl-pool.h>

namespace ns3 {

/// TbId_t structure
struct TbId_t
{
  uint16_t m_rnti; ///< RNTI
  uint8_t m_layer; ///< layer

public:
  TbId_t ();
  /**
   * Constructor
   *
   * \param a RNTI
   * \param b Layer
   */
  TbId_t (const uint16_t a, const uint8_t b);

  /**
   * \brief Implements equal operator
   * \param a element to compare
   * \param b element to compare
   * \return true if the elements are equal
   */
  friend bool operator == (const TbId_t &a, const TbId_t &b);

  /**
   * \brief Implements less operator
   * \param a element to compare
   * \param b element to compare
   * \return true if a is less than b
   */
  friend bool operator < (const TbId_t &a, const TbId_t &b);
};


/// tbInfo_t structure
struct tbInfo_t
{
  uint8_t ndi; ///< New data indicator
  uint16_t size; ///< Transport block size
  uint8_t mcs; ///< MCS
  std::vector<int> rbBitmap; ///< Resource block bitmap
  uint8_t harqProcessId; ///< HARQ process id
  uint8_t rv; ///< Redundancy version
  double mi; ///< Mutual information
  bool downlink; ///< whether is downlink
  bool corrupt; ///< whether is corrupt
  bool harqFeedbackSent; ///< is HARQ feedback sent
  double sinr; ///< mean SINR
};

typedef std::map<TbId_t, tbInfo_t> expectedTbs_t; ///< expectedTbs_t typedef

/// SlTbId_t structure
struct SlTbId_t
{
  uint16_t m_rnti; ///< source SL-RNTI
  uint8_t m_l1dst; ///< layer 1 group Id

public:
  SlTbId_t ();
  /**
   * Constructor
   *
   * \param a The RNTI
   * \param b The l1dst
   */
  SlTbId_t (const uint16_t a, const uint8_t b);

  /**
   * \brief Implements equal operator
   * \param a element to compare
   * \param b element to compare
   * \return true if the elements are equal
   */
  friend bool operator == (const SlTbId_t &a, const SlTbId_t &b);

  /**
   * \brief Implements less operator
   * \param a element to compare
   * \param b element to compare
   * \return true if a is less than b
   */

  friend bool operator < (const SlTbId_t &a, const SlTbId_t &b);
};

/// SltbInfo_t structure
struct SltbInfo_t
{
  uint8_t ndi; ///< New data indicator
  uint16_t size; ///< Transport block size
  uint8_t mcs; ///< MCS
  std::vector<int> rbBitmap; ///< Resource block bitmap
  uint8_t rv; ///< The redundancy version
  double mi; ///< Mutual information
  bool corrupt; ///< whether is corrupt
  bool harqFeedbackSent; ///< is HARQ feedback sent
  double sinr; ///< mean SINR
};

/// Map to store Sidelink communication expected TBs
typedef std::map<SlTbId_t, SltbInfo_t> expectedSlTbs_t; ///< expectedSlTbs_t typedef.

/// SlDiscTbId_t structure
struct SlDiscTbId_t
{
  uint16_t m_rnti; ///< source SL-RNTI
  uint8_t m_resPsdch; ///< PSDCH resource number

public:
  SlDiscTbId_t ();
  /**
   * Constructor
   *
   * \param a The RNTI
   * \param b The resPsdch
   */
  SlDiscTbId_t (const uint16_t a, const uint8_t b);

  /**
   * \brief Implements equal operator
   * \param a element to compare
   * \param b element to compare
   * \return true if the elements are equal
   */
  friend bool operator == (const SlDiscTbId_t &a, const SlDiscTbId_t &b);

  /**
   * \brief Implements less operator
   * \param a element to compare
   * \param b element to compare
   * \return true if a is less than b
   */
  friend bool operator < (const SlDiscTbId_t &a, const SlDiscTbId_t &b);
};

/// SlDisctbInfo_t structure
struct SlDisctbInfo_t
{
  uint8_t ndi; ///< New data indicator
  uint8_t resPsdch; ///< PSDCH resource number
  std::vector<int> rbBitmap; ///< RB bitmap
  uint8_t rv; ///< The redundancy version
  double mi; ///< Mutual information
  bool corrupt; ///< whether is corrupt
  bool harqFeedbackSent; ///< is HARQ feedback sent
  double sinr; ///< mean SINR
  int index; ///< index of the packet received in the reception buffer
};

/// Map to store Sidelink discovery expected TBs
typedef std::map<SlDiscTbId_t, SlDisctbInfo_t> expectedDiscTbs_t; ///< expectedDiscTbs_t typedef.



class LteNetDevice;
class AntennaModel;
class LteControlMessage;
struct LteSpectrumSignalParametersDataFrame;
struct LteSpectrumSignalParametersDlCtrlFrame;
struct LteSpectrumSignalParametersUlSrsFrame;
struct LteSpectrumSignalParametersSlFrame;
struct LteSpectrumSignalParametersSlDataFrame;
struct LteSpectrumSignalParametersSlCtrlFrame;
struct LteSpectrumSignalParametersSlDiscFrame;
struct LteSpectrumSignalParametersSlMibFrame;

/**
 * Structure for Sidelink packets being received
 */
struct SlRxPacketInfo_t
{
  Ptr<LteSpectrumSignalParametersSlFrame> params; ///< Parameters of sidelink signal
  std::vector<int> rbBitmap;  ///< RB bitmap
};

/// SlCtrlPacketInfo_t structure
struct SlCtrlPacketInfo_t
{
  double sinr; ///< SINR
  int index; ///< index of the packet received in the reception buffer

  /**
   * \brief Implements equal operator
   * \param a element to compare
   * \param b element to compare
   * \return true if the elements are equal
   */
  friend bool operator == (const SlCtrlPacketInfo_t &a, const SlCtrlPacketInfo_t &b);

  /**
   * \brief Implements less operator
   * \param a element to compare
   * \param b element to compare
   * \return true if a is less than b
   */
  friend bool operator < (const SlCtrlPacketInfo_t &a, const SlCtrlPacketInfo_t &b);
};

/**
* This method is used by the LteSpectrumPhy to notify the PHY that a
* previously started RX attempt has terminated without success
*/
typedef Callback< void > LtePhyRxDataEndErrorCallback;
/**
* This method is used by the LteSpectrumPhy to notify the PHY that a
* previously started RX attempt has been successfully completed.
*
* \param packet The received Packet
*/
typedef Callback< void, Ptr<Packet> > LtePhyRxDataEndOkCallback;

/**
* This method is used by the LteSpectrumPhy to notify the PHY that a
* previously started RX attempt has been successfully completed.
*
* \param packet The received Packet
*/
typedef Callback< void, Ptr<Packet>, uint16_t > LtePhyRxPsbchEndOkCallback;

/**
* This method is used by the LteSpectrumPhy to notify the PHY that a
* previously started RX of a control frame attempt has been
* successfully completed.
*/
typedef Callback< void, std::list<Ptr<LteControlMessage> > > LtePhyRxCtrlEndOkCallback;

/**
* This method is used by the LteSpectrumPhy to notify the PHY that a
* previously started RX of a control frame attempt has terminated
* without success.
*/
typedef Callback< void > LtePhyRxCtrlEndErrorCallback;

/**
* This method is used by the LteSpectrumPhy to notify the UE PHY that a
* PSS has been received
*/
typedef Callback< void, uint16_t, Ptr<SpectrumValue> > LtePhyRxPssCallback;


/**
* This method is used by the LteSpectrumPhy to notify the PHY about
* the status of a certain DL HARQ process
*/
typedef Callback< void, DlInfoListElement_s > LtePhyDlHarqFeedbackCallback;

/**
* This method is used by the LteSpectrumPhy to notify the PHY about
* the status of a certain UL HARQ process
*/
typedef Callback< void, UlInfoListElement_s > LtePhyUlHarqFeedbackCallback;

/**
* This method is used by the LteSpectrumPhy to notify the UE PHY that a
* SLSS has been received
*/
typedef Callback< void, uint16_t, Ptr<SpectrumValue> > LtePhyRxSlssCallback;

/**
* This method is used by the LteSpectrumPhy to notify the UE PHY that a
* PSDCH has been received and measure the SD-RSRP using the corresponding RBs
*/
typedef Callback< void, Ptr<Packet>, Ptr<SpectrumValue>, const std::vector<int>& > LtePhyRxPsdchSdRsrpCallback;

/**
 * \ingroup lte
 * \class LteSpectrumPhy
 *
 * The LteSpectrumPhy models the physical layer of LTE
 *
 * It supports a single antenna model instance which is
 * used for both transmission and reception.
 */
class LteSpectrumPhy : public SpectrumPhy
{

public:
  LteSpectrumPhy ();
  virtual ~LteSpectrumPhy ();

  /**
   *  PHY states
   */
  enum State
  {
    IDLE, TX_DL_CTRL, TX_DATA, TX_UL_SRS, RX_DL_CTRL, RX_DATA, RX_UL_SRS
  };

  /**
   * \brief Get the type ID.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);
  // inherited from Object
  virtual void DoDispose ();

  // inherited from SpectrumPhy
  void SetChannel (Ptr<SpectrumChannel> c);
  void SetMobility (Ptr<MobilityModel> m);
  void SetDevice (Ptr<NetDevice> d);
  Ptr<MobilityModel> GetMobility () const;
  Ptr<NetDevice> GetDevice () const;
  Ptr<const SpectrumModel> GetRxSpectrumModel () const;
  Ptr<Object> GetAntenna () const;
  void StartRx (Ptr<SpectrumSignalParameters> params);
  /**
   * \brief Start receive data function
   * \param params Ptr<LteSpectrumSignalParametersDataFrame>
   */
  void StartRxData (Ptr<LteSpectrumSignalParametersDataFrame> params);
  /**
   * \brief Start receive DL control function
   * \param lteDlCtrlRxParams Ptr<LteSpectrumSignalParametersDlCtrlFrame>
   */
  void StartRxDlCtrl (Ptr<LteSpectrumSignalParametersDlCtrlFrame> lteDlCtrlRxParams);
  /**
   * \brief Start receive UL SRS function
   * \param lteUlSrsRxParams Ptr<LteSpectrumSignalParametersUlSrsFrame>
   */
  void StartRxUlSrs (Ptr<LteSpectrumSignalParametersUlSrsFrame> lteUlSrsRxParams);
  /**
   * \brief Start receive Sidelink data function
   * \param lteSlRxParams Ptr<LteSpectrumSignalParametersUlSrsFrame>
   */
  void StartRxSlFrame (Ptr<LteSpectrumSignalParametersSlFrame> lteSlRxParams);
  /**
   * \brief Set HARQ phy function
   * \param harq The HARQ phy module
   */
  void SetHarqPhyModule (Ptr<LteHarqPhy> harq);
  /**
   * \brief Set Sidelink HARQ phy function
   * \param lteSlHarq The Sidelink HARQ phy module
   */
  void SetSlHarqPhyModule (Ptr<LteSlHarqPhy> lteSlHarq);

  /**
   * set the Power Spectral Density of outgoing signals in W/Hz.
   *
   * \param txPsd The Transmit Power Spectral Density of outgoing signals in W/Hz.
   */
  void SetTxPowerSpectralDensity (Ptr<SpectrumValue> txPsd);

  /**
   * \brief set the noise power spectral density
   * \param noisePsd The Noise Power Spectral Density in power units
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
   * Clear expected discovery TBs
   *
   */
  void ClearExpectedDiscTb ();

  /**
   * set the AntennaModel to be used
   *
   * \param a The Antenna Model
   */
  void SetAntenna (Ptr<AntennaModel> a);

  /**
  * Start a transmission of data frame in DL and UL
  *
  *
  * \param pb The burst of packets to be transmitted in PDSCH/PUSCH
  * \param ctrlMsgList The list of LteControlMessage to send
  * \param duration The duration of the data frame
  *
  * \return true if an error occurred and the transmission was not
  * started, false otherwise.
  */
  bool StartTxDataFrame (Ptr<PacketBurst> pb, std::list<Ptr<LteControlMessage> > ctrlMsgList, Time duration);

  /**
   * Start a transmission of Sidelink data frame in DL and UL
   *
   *
   * \param pb The burst of packets to be transmitted in PSSCH/PSCCH
   * \param duration The duration of the data frame
   *
   * \return true if an error occurred and the transmission was not
   * started, false otherwise.
   */
  bool StartTxSlMibFrame (Ptr<PacketBurst> pb, Time duration);

  /**
  * Start a transmission of Sidelink data frame in DL and UL
  *
  *
  * \param pb The burst of packets to be transmitted in PSCCH
  * \param duration The duration of the data frame
  *
  * \return true if an error occurred and the transmission was not
  * started, false otherwise.
  */
  bool StartTxSlCtrlFrame (Ptr<PacketBurst> pb, Time duration);

  /**
   * Start a transmission of Sidelink data frame in DL and UL
   *
   *
   * \param pb The burst of packets to be transmitted in PSSCH
   * \param duration The duration of the data frame
   * \param groupId The group id
   *
   * \return true if an error occurred and the transmission was not
   * started, false otherwise.
   */
  bool StartTxSlDataFrame (Ptr<PacketBurst> pb, Time duration, uint8_t groupId);

  /**
   * Start a transmission of Sidelink data frame in DL and UL
   *
   *
   * \param pb The burst of packets to be transmitted in PSDCH
   * \param resNo The resource number in the PSDCH pool
   * \param rv The redundancy version number
   * \param duration The duration of the data frame
   *
   * \return true if an error occurred and the transmission was not
   * started, false otherwise.
   */
  bool StartTxSlDiscFrame (Ptr<PacketBurst> pb, uint32_t resNo, uint8_t rv, Time duration);

  /**
  * Start a transmission of Sidelink data frame in DL and UL
  *
  *
  * \param pb The burst of packets to be transmitted in PSDCH
  * \param ctrlMsgList The list of LteControlMessage to send
  * \param duration The duration of the data frame
  *
  * \return true if an error occurred and the transmission was not
  * started, false otherwise.
  */
  bool StartTxSlMibFrame (Ptr<PacketBurst> pb, std::list<Ptr<LteControlMessage> > ctrlMsgList, Time duration);


  /**
  * Start a transmission of control frame in DL
  *
  *
  * \param ctrlMsgList The burst of control messages to be transmitted
  * \param pss The flag for transmitting the primary synchronization signal
  *
  * \return true if an error occurred and the transmission was not
  * started, false otherwise.
  */
  bool StartTxDlCtrlFrame (std::list<Ptr<LteControlMessage> > ctrlMsgList, bool pss);


  /**
  * Start a transmission of control frame in UL
  *
  * \return true if an error occurred and the transmission was not
  * started, false otherwise.
  */
  bool StartTxUlSrsFrame ();

  /**
   * set the callback for the end of a RX in error, as part of the
   * interconnections between the PHY and the MAC
   *
   * \param c The callback
   */
  void SetLtePhyRxDataEndErrorCallback (LtePhyRxDataEndErrorCallback c);

  /**
   * set the callback for the successful end of a RX, as part of the
   * interconnections between the PHY and the MAC
   *
   * \param c The callback
   */
  void SetLtePhyRxDataEndOkCallback (LtePhyRxDataEndOkCallback c);

  /**
   * set the callback for the successful end of a PSCCH RX, as part of the
   * interconnections between the PHY and the MAC
   *
   * \param c The callback
   */
  void SetLtePhyRxPscchEndOkCallback (LtePhyRxDataEndOkCallback c);

  /**
   * set the callback for the successful end of a PSDCH RX, as part of the
   * interconnections between the PHY and the MAC
   *
   * \param c The callback
   */
  void SetLtePhyRxPsdchEndOkCallback (LtePhyRxDataEndOkCallback c);

  /**
   * set the callback for the successful end of a PSBCH RX, as part of the
   * interconnections between the PHY and the MAC
   *
   * \param c The callback
   */
  void SetLtePhyRxPsbchEndOkCallback (LtePhyRxPsbchEndOkCallback c);

  /**
  * set the callback for the successful end of a RX ctrl frame, as part
  * of the interconnections between the LteSpectrumPhy and the PHY
  *
  * \param c The callback
  */
  void SetLtePhyRxCtrlEndOkCallback (LtePhyRxCtrlEndOkCallback c);

  /**
  * set the callback for the erroneous end of a RX ctrl frame, as part
  * of the interconnections between the LteSpectrumPhy and the PHY
  *
  * \param c The callback
  */
  void SetLtePhyRxCtrlEndErrorCallback (LtePhyRxCtrlEndErrorCallback c);

  /**
  * set the callback for the reception of the PSS as part
  * of the interconnections between the LteSpectrumPhy and the UE PHY
  *
  * \param c The callback
  */
  void SetLtePhyRxPssCallback (LtePhyRxPssCallback c);

  /**
  * set the callback for the DL HARQ feedback as part of the
  * interconnections between the LteSpectrumPhy and the PHY
  *
  * \param c The callback
  */
  void SetLtePhyDlHarqFeedbackCallback (LtePhyDlHarqFeedbackCallback c);

  /**
  * set the callback for the UL HARQ feedback as part of the
  * interconnections between the LteSpectrumPhy and the PHY
  *
  * \param c The callback
  */
  void SetLtePhyUlHarqFeedbackCallback (LtePhyUlHarqFeedbackCallback c);

  /**
   * \brief Set the state of the phy layer
   * \param newState The state of the phy layer
   */
  void SetState (State newState);

  /**
   * \brief Set the Cell Identifier
   * \param cellId The Cell Identifier
   */
  void SetCellId (uint16_t cellId);

  /**
   * \brief Add a new L1 group for filtering
   *
   * \param groupId The L1 Group Identifier
   */
  void AddL1GroupId (uint8_t groupId);

  /**
   * \brief Remove a new L1 group for filtering
   *
   * \param groupId The L1 Group Identifier
   */
  void RemoveL1GroupId (uint8_t groupId);

  /**
   *
   * \param componentCarrierId The component carrier id
   */
  void SetComponentCarrierId (uint8_t componentCarrierId);

  /**
   *
   *
   * \param p The new LteChunkProcessor to be added to the RS power
   *          processing chain
   */
  void AddRsPowerChunkProcessor (Ptr<LteChunkProcessor> p);

  /**
   *
   *
   * \param p The new LteChunkProcessor to be added to the Data Channel power
   *          processing chain
   */
  void AddDataPowerChunkProcessor (Ptr<LteChunkProcessor> p);

  /**
   *
   *
   * \param p The new LteChunkProcessor to be added to the data processing chain
   */
  void AddDataSinrChunkProcessor (Ptr<LteChunkProcessor> p);

  /**
   *  LteChunkProcessor devoted to evaluate interference + noise power
   *  in control symbols of the subframe
   *
   * \param p The new LteChunkProcessor to be added to the data processing chain
   */
  void AddInterferenceCtrlChunkProcessor (Ptr<LteChunkProcessor> p);

  /**
   *  LteChunkProcessor devoted to evaluate interference + noise power
   *  in data symbols of the subframe
   *
   * \param p The new LteChunkProcessor to be added to the data processing chain
   */
  void AddInterferenceDataChunkProcessor (Ptr<LteChunkProcessor> p);


  /**
   *
   *
   * \param p The new LteChunkProcessor to be added to the ctrl processing chain
   */
  void AddCtrlSinrChunkProcessor (Ptr<LteChunkProcessor> p);

  /**
   *
   *
   * \param p The new LteSlChunkProcessor to be added to the Sidelink processing chain
   */
  void AddSlSinrChunkProcessor (Ptr<LteSlChunkProcessor> p);

  /**
   *
   *
   * \param p The new LteSlChunkProcessor to be added to the Sidelink processing chain
   */
  void AddSlSignalChunkProcessor (Ptr<LteSlChunkProcessor> p);

  /**
   *
   *
   * \param p The new LteSlChunkProcessor to be added to the Sidelink processing chain
   */
  void AddSlInterferenceChunkProcessor (Ptr<LteSlChunkProcessor> p);

  /**
   *
   *
   * \param rnti the RNTI of the source of the TB
   * \param ndi new data indicator flag
   * \param size the size of the TB
   * \param mcs the MCS of the TB
   * \param map the map of RB(s) used
   * \param layer the layer (in case of MIMO tx)
   * \param harqId the id of the HARQ process (valid only for DL)
   * \param rv the redundancy version
   * \param downlink true when the TB is for DL
   */
  void AddExpectedTb (uint16_t  rnti, uint8_t ndi, uint16_t size, uint8_t mcs, std::vector<int> map, uint8_t layer, uint8_t harqId, uint8_t rv, bool downlink);
  /**
   * \brief Remove expected transport block.
   *
   * When UE context at eNodeB is removed and if UL TB is expected to be received
   * but not transmitted due to break in radio link. The TB with different rnti or lcid
   * remains during the transmission of a new TB and causes problems with
   * m_ulPhyReception trace, since the UE context was already removed. TB has to be
   * removed when ue context at eNodeB is removed
   *
   * \param rnti The RNTI of the UE
   */
  void RemoveExpectedTb (uint16_t  rnti);
  
  /**
   * \brief Add Expected TB method for Sidelink communication
   * \param rnti The RNTI of the source of the TB
   * \param l1dst the layer 1 destination id of the TB
   * \param ndi The new data indicator flag
   * \param size the size of the TB
   * \param mcs The MCS of the TB
   * \param map The map of RB(s) used
   * \param rv The redundancy version
   */
  void AddExpectedTb (uint16_t  rnti, uint8_t l1dst, uint8_t ndi, uint16_t size, uint8_t mcs, std::vector<int> map, uint8_t rv);

  /**
   * \brief Add Expected TB method for Sidelink Discovery
   * 
   * no mcs, size fixed to 232 bits, no l1dst
   *
   * \param rnti The RNTI of the source of the TB
   * \param resPsdch The PSDCH resource identifier
   * \param ndi The new data indicator flag
   * \param map map of RBs used
   * \param rv The redundancy version
   * \param index The packet index in the list of discovery messages received
   */
  void AddExpectedTb (uint16_t  rnti, uint8_t resPsdch, uint8_t ndi, std::vector<int> map, uint8_t rv, int index);


  /**
   *
   *
   * \param sinr vector of SINR perceived per each RB
   */
  void UpdateSinrPerceived (const SpectrumValue& sinr);

  /**
   *
   *
   * \param sinr vector of SINR perceived per each RB per Sidelink packet
   */
  void UpdateSlSinrPerceived (std::vector <SpectrumValue> sinr);

  /**
   *
   *
   * \param signal vector of signal perceived per each RB per Sidelink packet
   */
  void UpdateSlSigPerceived (std::vector <SpectrumValue> signal);

  /**
   *
   *
   * \param interference vector of interference perceived per each RB per Sidelink packet
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
   * \return The previously set channel
   */
  Ptr<SpectrumChannel> GetChannel ();

  /**
   * Sets the SLSSID of the SyncRef to which the UE is synchronized
   * \param slssid the SyncRef identifier
   */
  void SetSlssid (uint64_t slssid);
  /**
   * Sets the callback for the reception of the SLSS as part
   * of the interconnections between the LteSpectrumPhy and the UE PHY
   *
   * \param c The callback
   */
  void SetLtePhyRxSlssCallback (LtePhyRxSlssCallback c);

  /**
   * Sets the callback for the measurement of the SD-RSRP as part
   * of the interconnections between the LteSpectrumPhy and the UE PHY
   *
   * \param c The callback
   */
  void SetLtePhyRxPsdchSdRsrpCallback (LtePhyRxPsdchSdRsrpCallback c);


  /// allow LteUePhy class friend access
  friend class LteUePhy;

  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model.  Return the number of streams (possibly zero) that
   * have been assigned.
   *
   * \param stream The first stream index to use
   * \return The number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

  /**
   *
   * \return The state of the PHY
   */
  State GetState ();

  /**
   * Set Rx pool function
   * Sets the Sidelink Discovery reception pool
   *
   * \param newpool The Sidelink discovery resource pool
   */
  void SetRxPool (Ptr<SidelinkDiscResourcePool> newpool);

  /**
   * Set Discovery number of retransmission function
   * Sets the number of retransmissions for Sidelink Discovery messages
   *
   * \param retx The number of retransmissions
   */
  void SetDiscNumRetx (uint8_t retx);

  /**
  * TracedCallback signature for TB drop.
  *
  * \param [in] TB The transport block index
  */
  typedef void (* DropSlTbTracedCallback)(uint64_t);
  /**
  * TracedCallback signature for Sidelink start Rx.
  *
  * \param [in] pointer The pointer to LteSpectrumPhy
  */
  typedef void (* SlStartRxTracedCallback)(Ptr<LteSpectrumPhy>);

private:
  /**
   * \brief Change state function
   *
   * \param newState The new state to set
   */
  void ChangeState (State newState);
  /// End transmit data function
  void EndTxData ();
  /// End transmit DL control function
  void EndTxDlCtrl ();
  /// End transmit UL SRS function
  void EndTxUlSrs ();
  /// End receive data function
  void EndRxData ();
  /// End receive DL control function
  void EndRxDlCtrl ();
  /// End receive UL SRS function
  void EndRxUlSrs ();
  /// End receive Sidelink Data function
  void EndRxSlFrame ();

  /**
   * \brief Set transmit mode gain function
   *
   * \param txMode The transmit mode
   * \param gain The gain to set
   */
  void SetTxModeGain (uint8_t txMode, double gain);

  /**
   * \brief Set Sidelink transmit mode gain function
   * Average gain for SIMO based on [CatreuxMIMO]
   *
   * \param gain The gain to set
   */
  void SetSlRxGain (double gain);

  /**
   * \brief Get mean SINR function
   *
   * \param sinr The SINR values
   * \param rbBitMap The vector whose size is equal to the number active RBs
   * \return The average SINR per RB in linear scale
   */
  double GetMeanSinr (const SpectrumValue& sinr, const std::vector<int>& rbBitMap);

  /**
   * \brief Process received PSCCH messages function
   * \param pktIndexes Indexes of PSCCH messages received
   */
  void RxSlPscch (std::vector<uint32_t> pktIndexes);

  /**
   * \brief Process received PSSCH messages function
   * \param pktIndexes Indexes of PSSCH messages received
   */
  void RxSlPssch (std::vector<uint32_t> pktIndexes);

  /**
   * \brief Process received PSDCH messages function
   * \param pktIndexes Indexes of PSDCH messages received
   */
  void RxSlPsdch (std::vector<uint32_t> pktIndexes);

  /**
   * \brief Process received PSBCH messages function
   * \param pktIndexes Indexes of PSBCH messages received
   */
  void RxSlPsbch (std::vector<uint32_t> pktIndexes);

  Ptr<MobilityModel> m_mobility; ///< the mobility model
  Ptr<AntennaModel> m_antenna; ///< the antenna model
  Ptr<NetDevice> m_device; ///< the device

  Ptr<SpectrumChannel> m_channel; ///< the channel

  Ptr<const SpectrumModel> m_rxSpectrumModel; ///< the spectrum model
  Ptr<SpectrumValue> m_txPsd; ///< the transmit PSD
  Ptr<PacketBurst> m_txPacketBurst; ///< the transmit packet burst
  std::list<Ptr<PacketBurst> > m_rxPacketBurstList; ///< the receive burst list

  std::list<Ptr<LteControlMessage> > m_txControlMessageList; ///< the transmit control message list
  std::list<Ptr<LteControlMessage> > m_rxControlMessageList; ///< the receive control message list


  State m_state; ///< the state
  Time m_firstRxStart; ///< the first receive start
  Time m_firstRxDuration; ///< the first receive duration

  TracedCallback<Ptr<const PacketBurst> > m_phyTxStartTrace; ///< the phy transmit start trace callback
  TracedCallback<Ptr<const PacketBurst> > m_phyTxEndTrace; ///< the phy transmit end trace callback
  TracedCallback<Ptr<const PacketBurst> > m_phyRxStartTrace; ///< the phy receive start trace callback
  TracedCallback<Ptr<const Packet> >      m_phyRxEndOkTrace; ///< the phy receive end ok trace callback
  TracedCallback<Ptr<const Packet> >      m_phyRxEndErrorTrace; ///< the phy receive end error trace callback

  LtePhyRxDataEndErrorCallback   m_ltePhyRxDataEndErrorCallback; ///< the LTE phy receive data end error callback
  LtePhyRxDataEndOkCallback      m_ltePhyRxDataEndOkCallback; ///< the LTE phy receive data end ok callback
  LtePhyRxDataEndOkCallback      m_ltePhyRxPscchEndOkCallback; ///< the LTE phy receive PSCCH end ok callback
  LtePhyRxDataEndOkCallback      m_ltePhyRxPsdchEndOkCallback; ///< the LTE phy receive PSDCH end ok callback
  LtePhyRxPsbchEndOkCallback      m_ltePhyRxPsbchEndOkCallback; ///< the LTE phy receive PSBCH end ok callback

  LtePhyRxCtrlEndOkCallback     m_ltePhyRxCtrlEndOkCallback; ///< the LTE phy receive control end ok callback
  LtePhyRxCtrlEndErrorCallback  m_ltePhyRxCtrlEndErrorCallback; ///< the LTE phy receive control end error callback
  LtePhyRxPssCallback  m_ltePhyRxPssCallback; ///< the LTE phy receive PSS callback

  Ptr<LteInterference> m_interferenceData; ///< the data interference
  Ptr<LteInterference> m_interferenceCtrl; ///< the control interference

  uint16_t m_cellId; ///< the cell ID

  uint8_t m_componentCarrierId; ///< the component carrier ID
  expectedTbs_t m_expectedTbs; ///< the expected TBS
  expectedDiscTbs_t m_expectedDiscTbs; ///< the expected Sidelink Discovery TBS
  SpectrumValue m_sinrPerceived; ///< the perceived SINR

  // Information for Sidelink Communication
  Ptr<LteSlInterference> m_interferenceSl; ///< the Sidelink interference
  std::set<uint8_t> m_l1GroupIds; ///< identifiers for D2D layer 1 filtering
  expectedSlTbs_t m_expectedSlTbs; ///< the expected Sidelink Communication TBS
  std::vector<SpectrumValue> m_slSinrPerceived; ///< SINR for each D2D packet received
  std::vector<SpectrumValue> m_slSignalPerceived; ///< Signal for each D2D packet received
  std::vector<SpectrumValue> m_slInterferencePerceived; ///< interference for each D2D packet received
  std::vector< SlRxPacketInfo_t > m_rxPacketInfo; ///< Sidelink received packet information

  /// Provides uniform random variables.
  Ptr<UniformRandomVariable> m_random; ///< Uniform random variable used to toss for the reception of the TB
  bool m_dataErrorModelEnabled; ///< when true (default) the phy error model is enabled
  bool m_ctrlErrorModelEnabled; ///< when true (default) the phy error model is enabled for DL ctrl frame

  bool m_ctrlFullDuplexEnabled; ///< when true the PSCCH operates in Full Duplex mode (disabled by default).

  bool m_dropRbOnCollisionEnabled; ///< when true, drop all receptions on colliding RBs regardless SINR value.
  bool m_slDataErrorModelEnabled; ///< when true (default) the PSSCH phy error model is enabled
  bool m_slCtrlErrorModelEnabled; ///< when true (default) the PSCCH phy error model is enabled
  bool m_slDiscoveryErrorModelEnabled; ///< when true (default) the PSDCH phy error model is enabled
  LteNistErrorModel::LteFadingModel m_fadingModel; ///< Type of fading model

  uint8_t m_transmissionMode; ///< for UEs: store the transmission mode
  uint8_t m_layersNum; ///< layers num
  bool m_ulDataSlCheck; ///< Flag used to indicate the transmission of data in uplink
  std::vector <double> m_txModeGain; ///< duplicate value of LteUePhy

  Ptr<LteHarqPhy> m_harqPhyModule; ///< the HARQ phy module
  Ptr<LteSlHarqPhy> m_slHarqPhyModule; ///< the Sidelink HARQ phy module
  LtePhyDlHarqFeedbackCallback m_ltePhyDlHarqFeedbackCallback; ///< the LTE phy DL HARQ feedback callback
  LtePhyUlHarqFeedbackCallback m_ltePhyUlHarqFeedbackCallback; ///< the LTE phy UL HARQ feedback callback

  Ptr<LteSpectrumPhy> m_halfDuplexPhy; ///< Pointer to a UL LteSpectrumPhy object

  std::list< Ptr<SidelinkDiscResourcePool> > m_discRxPools; ///< List of discovery Rx pools

  uint64_t m_slssId; ///< the Sidelink Synchronization Signal Identifier (SLSSID)

  double m_slRxGain; ///< Sidelink Rx gain (Linear units)
  std::map <uint16_t, uint16_t> m_slDiscTxCount; ///< Map to store the count of discovery transmissions
  ///< by a UE. The RNTI of a UE is used as the key of this map

  LtePhyRxSlssCallback  m_ltePhyRxSlssCallback; ///< Callback used to notify the PHY about the reception of a SLSS

  LtePhyRxPsdchSdRsrpCallback m_ltePhyRxPsdchSdRsrpCallback; ///< Callback used to notify the PHY about the reception of PSDCH and SD-RSRP measurement

  /**
   * Trace information regarding PHY stats from DL Rx perspective
   * PhyReceptionStatParameters (see lte-common.h)
   */
  TracedCallback<PhyReceptionStatParameters> m_dlPhyReception;


  /**
   * Trace information regarding PHY stats from UL Rx perspective
   * PhyReceptionStatParameters (see lte-common.h)
   */
  TracedCallback<PhyReceptionStatParameters> m_ulPhyReception;

  /**
   * Trace information regarding PHY stats from Sidelink Rx perspective
   * PhyReceptionStatParameters (see lte-common.h)
   */
  TracedCallback<PhyReceptionStatParameters> m_slPhyReception;

  /**
   * Trace information regarding PHY stats from SL Rx PSCCH perspective
   * PhyReceptionStatParameters (see lte-common.h)
   */
  TracedCallback<SlPhyReceptionStatParameters> m_slPscchReception;

  /**
   * The `SlStartRx` trace source. Trace fired when reception at Sidelink starts.
   */
  TracedCallback<Ptr<LteSpectrumPhy> > m_slStartRx;

  EventId m_endTxEvent; ///< end transmit event
  EventId m_endRxDataEvent; ///< end receive data event
  EventId m_endRxDlCtrlEvent; ///< end receive DL control event
  EventId m_endRxUlSrsEvent; ///< end receive UL SRS event


};


}

#endif /* LTE_SPECTRUM_PHY_H */
