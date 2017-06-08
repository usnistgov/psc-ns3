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
 * Author: Marco Miozzo <marco.miozzo@cttc.es>
 */

#ifndef NIST_ENB_LTE_PHY_H
#define NIST_ENB_LTE_PHY_H


#include <ns3/nist-lte-control-messages.h>
#include <ns3/nist-lte-enb-phy-sap.h>
#include <ns3/nist-lte-enb-cphy-sap.h>
#include <ns3/nist-lte-phy.h>
#include <ns3/nist-lte-harq-phy.h>

#include <map>
#include <set>



namespace ns3 {

class PacketBurst;
class NistLteNetDevice;
class NistLteUePhy;

/**
 * \ingroup lte
 * NistLteEnbPhy models the physical layer for the eNodeB
 */
class NistLteEnbPhy : public NistLtePhy
{
  friend class NistEnbMemberLteEnbPhySapProvider;
  friend class MemberNistLteEnbCphySapProvider<NistLteEnbPhy>;

public:
  /**
   * @warning the default constructor should not be used
   */
  NistLteEnbPhy ();

  /**
   *
   * \param dlPhy the downlink NistLteSpectrumPhy instance
   * \param ulPhy the uplink NistLteSpectrumPhy instance
   */
  NistLteEnbPhy (Ptr<NistLteSpectrumPhy> dlPhy, Ptr<NistLteSpectrumPhy> ulPhy, bool EnbEnablePhyLayer = true);

  virtual ~NistLteEnbPhy ();

  // inherited from Object
  static TypeId GetTypeId (void);
  virtual void DoInitialize (void);
  virtual void DoDispose (void);


  /**
  * \brief Get the PHY SAP provider
  * \return a pointer to the SAP Provider of the PHY
  */
  NistLteEnbPhySapProvider* GetNistLteEnbPhySapProvider ();

  /**
  * \brief Set the PHY SAP User
  * \param s a pointer to the PHY SAP user
  */
  void SetNistLteEnbPhySapUser (NistLteEnbPhySapUser* s);

  /**
   * \brief Get the CPHY SAP provider
   * \return a pointer to the SAP Provider
   */
  NistLteEnbCphySapProvider* GetNistLteEnbCphySapProvider ();

  /**
  * \brief Set the CPHY SAP User
  * \param s a pointer to the SAP user
  */
  void SetNistLteEnbCphySapUser (NistLteEnbCphySapUser* s);

  /**
   * \param pow the transmission power in dBm
   */
  void SetTxPower (double pow);

  /**
   * \return the transmission power in dBm
   */
  double GetTxPower () const;

  /**
   * \return the transmission power in dBm
   */
  int8_t DoGetReferenceSignalPower () const;

  /**
   * \param pow the noise figure in dB
   */
  void SetNoiseFigure (double pow);

  /**
   * \return the noise figure in dB
   */
  double GetNoiseFigure () const;

  /**
   * \param delay the TTI delay between MAC and channel
   */
  void SetMacChDelay (uint8_t delay);

  /**
   * \returns the TTI delay between MAC and channel
   */
  uint8_t GetMacChDelay (void) const;

  /**
   * \return a pointer to the NistLteSpectrumPhy instance relative to the downlink
   */
  Ptr<NistLteSpectrumPhy> GetDlSpectrumPhy () const;

  /**
   * \return a pointer to the NistLteSpectrumPhy instance relative to the uplink
   */
  Ptr<NistLteSpectrumPhy> GetUlSpectrumPhy () const;


  /**
   * \brief set the resource blocks (a.k.a. sub channels) to be used in the downlink for transmission
   * 
   * \param mask a vector of integers, if the i-th value is j it means
   * that the j-th resource block is used for transmission in the
   * downlink. If there is no i such that the value of the i-th
   * element is j, it means that RB j is not used.
   */
  void SetDownlinkSubChannels (std::vector<int> mask );

  /**
   * \brief set the resource blocks (a.k.a. sub channels) and its power
   * to be used in the downlink for transmission
   *
   * \param mask a vector of integers, if the i-th value is j it means
   * that the j-th resource block is used for transmission in the
   * downlink. If there is no i such that the value of the i-th
   * element is j, it means that RB j is not used.
   */
  void SetDownlinkSubChannelsWithPowerAllocation (std::vector<int> mask);
  /**
   * 
   * \return  a vector of integers, if the i-th value is j it means
   * that the j-th resource block is used for transmission in the
   * downlink. If there is no i such that the value of the i-th
   * element is j, it means that RB j is not used.
   */
  std::vector<int> GetDownlinkSubChannels (void);


  /**
   * \brief Generate power allocation map (i.e. tx power level for each RB)
   *
   * \param rnti indicates which UE will occupy this RB
   * \param rbId indicates which RB UE is using,
   * power level for this RB is power level of UE
   */
  void GeneratePowerAllocationMap (uint16_t rnti, int rbId);

  /**
   * \brief Create the PSD for TX
   */
  virtual Ptr<SpectrumValue> CreateTxPowerSpectralDensity ();

  /**
   * \brief Create the PSD for TX with power allocation for each RB
   */
  virtual Ptr<SpectrumValue> CreateTxPowerSpectralDensityWithPowerAllocation ();

  /**
   * \brief Calculate the channel quality for a given UE
   * \param sinr a list of computed SINR
   * \param ue the UE
   */
  void CalcChannelQualityForUe (std::vector <double> sinr, Ptr<NistLteSpectrumPhy> ue);

  /**
   * \brief Receive the control message
   * \param msg the received message
   */
  virtual void ReceiveNistLteControlMessage (Ptr<NistLteControlMessage> msg);

  /**
  * \brief Create the UL CQI feedback from SINR values perceived at
  * the physical layer with the PUSCH signal received from eNB
  * \param sinr SINR values vector
  * \return UL CQI feedback in the format usable by an FF MAC scheduler
  */
  NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters CreatePuschCqiReport (const SpectrumValue& sinr);

  /**
  * \brief Create the UL CQI feedback from SINR values perceived at
  * the physical layer with the SRS signal received from eNB
  * \param sinr SINR values vector
  * \return UL CQI feedback in the format usable by an FF MAC scheduler
  */
  NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters CreateSrsCqiReport (const SpectrumValue& sinr);

  /**
  * \brief Send the PDCCH and PCFICH in the first 3 symbols
  * \param ctrlMsgList the list of control messages of PDCCH
  */
  void SendControlChannels (std::list<Ptr<NistLteControlMessage> > ctrlMsgList);

  /**
  * \brief Send the PDSCH
  * \param pb the PacketBurst to be sent
  */
  void SendDataChannels (Ptr<PacketBurst> pb);

  /**
  * \param m the UL-CQI to be queued
  */
  void QueueUlDci (NistUlDciLteControlMessage m);

  /**
  * \returns the list of UL-CQI to be processed
  */
  std::list<NistUlDciLteControlMessage> DequeueUlDci (void);


  /**
   * \brief Start a LTE frame
   */
  void StartFrame (void);
  /**
   * \brief Start a LTE sub frame
   */
  void StartSubFrame (void);
  /**
   * \brief End a LTE sub frame
   */
  void EndSubFrame (void);
  /**
   * \brief End a LTE frame
   */
  void EndFrame (void);

  /**
   * \brief PhySpectrum received a new PHY-PDU
   */
  void PhyPduReceived (Ptr<Packet> p);

  /**
  * \brief PhySpectrum received a new list of NistLteControlMessage
  */
  virtual void ReceiveNistLteControlMessageList (std::list<Ptr<NistLteControlMessage> >);

  // inherited from NistLtePhy
  virtual void GenerateCtrlCqiReport (const SpectrumValue& sinr);
  virtual void GenerateDataCqiReport (const SpectrumValue& sinr);
  virtual void ReportInterference (const SpectrumValue& interf);
  virtual void ReportRsReceivedPower (const SpectrumValue& power);



  /**
  * \brief PhySpectrum generated a new UL HARQ feedback
  */
  virtual void ReceiveLteUlHarqFeedback (NistUlInfoListElement_s mes);

  void SetHarqPhyModule (Ptr<NistLteHarqPhy> harq);

  /**
   * TracedCallback signature for the linear average of SRS SINRs.
   *
   * \param [in] cellId
   * \param [in] rnti
   * \param [in] sinrLinear
   */
  typedef void (* ReportUeSinrTracedCallback)
    (const uint16_t cellId, const uint16_t rnti, const double sinrLinear);

  /**
   * TracedCallback signature for the linear average of SRS SINRs.
   *
   * \param [in] cellId
   * \param [in] spectrumValue
   */
  typedef void (* ReportInterferenceTracedCallback)
    (const uint16_t cellId, const Ptr<const SpectrumValue> spectrumValue);


private:

  // NistLteEnbCphySapProvider forwarded methods
  void DoSetBandwidth (uint8_t ulBandwidth, uint8_t dlBandwidth);
  void DoSetEarfcn (uint16_t dlEarfcn, uint16_t ulEarfcn);
  void DoAddUe (uint16_t rnti);
  void DoRemoveUe (uint16_t rnti);
  void DoSetPa (uint16_t rnti, double pa);
  void DoSetTransmissionMode (uint16_t  rnti, uint8_t txMode);
  void DoSetSrsConfigurationIndex (uint16_t  rnti, uint16_t srcCi);
  void DoSetNistMasterInformationBlock (NistLteRrcSap::NistMasterInformationBlock mib);
  void DoSetNistSystemInformationBlockType1 (NistLteRrcSap::NistSystemInformationBlockType1 sib1);

  // NistLteEnbPhySapProvider forwarded methods
  void DoSendMacPdu (Ptr<Packet> p);
  void DoSendNistLteControlMessage (Ptr<NistLteControlMessage> msg);
  uint8_t DoGetMacChTtiDelay ();

  /**
   * Add the given RNTI to the list of attached UE #m_ueAttached.
   * \param rnti RNTI of a UE
   * \return true if the RNTI has _not_ existed before, or false otherwise.
   */
  bool AddUePhy (uint16_t rnti);
  /**
   * Remove the given RNTI from the list of attached UE #m_ueAttached.
   * \param rnti RNTI of a UE
   * \return true if the RNTI has existed before, or false otherwise.
   */
  bool DeleteUePhy (uint16_t rnti);

  void CreateSrsReport (uint16_t rnti, double srs);

  /**
   * List of RNTI of attached UEs. Used for quickly determining whether a UE is
   * attached to this eNodeB or not.
   */
  std::set <uint16_t> m_ueAttached;


  /// P_A per UE RNTI.
  std::map <uint16_t,double> m_paMap;

  /// DL power allocation map.
  std::map <int, double> m_dlPowerAllocationMap;

  /**
   * A vector of integers, if the i-th value is j it means that the j-th
   * resource block is used for transmission in the downlink. If there is
   * no i such that the value of the i-th element is j, it means that RB j
   * is not used.
   */
  std::vector <int> m_listOfDownlinkSubchannel;

  std::vector <int> m_dlDataRbMap;

  /// For storing info on future receptions.
  std::vector< std::list<NistUlDciLteControlMessage> > m_ulDciQueue;

  NistLteEnbPhySapProvider* m_enbPhySapProvider;
  NistLteEnbPhySapUser* m_enbPhySapUser;

  NistLteEnbCphySapProvider* m_enbCphySapProvider;
  NistLteEnbCphySapUser* m_enbCphySapUser;

  /**
   * The frame number currently served. In ns-3, frame number starts from 1.
   * In contrast, the 3GPP standard's frame number starts from 0.
   */
  uint32_t m_nrFrames;
  /**
   * The subframe number currently served. In ns-3, subframe number starts
   * from 1. In contrast, the 3GPP standard's subframe number starts from 0.
   * The number resets to the beginning again after 10 subframes.
   */
  uint32_t m_nrSubFrames;

  uint16_t m_srsPeriodicity;
  Time m_srsStartTime;
  std::map <uint16_t,uint16_t> m_srsCounter;
  std::vector <uint16_t> m_srsUeOffset;
  uint16_t m_currentSrsOffset;

  /**
   * The Master Information Block message to be broadcasted every frame.
   * The message content is specified by the upper layer through the RRC SAP.
   */
  NistLteRrcSap::NistMasterInformationBlock m_mib;
  /**
   * The System Information Block Type 1 message to be broadcasted. SIB1 is
   * broadcasted every 6th subframe of every odd-numbered radio frame.
   * The message content is specified by the upper layer through the RRC SAP.
   */
  NistLteRrcSap::NistSystemInformationBlockType1 m_sib1;

  Ptr<NistLteHarqPhy> m_harqPhyModule;

  /**
   * The `ReportUeSinr` trace source. Reporting the linear average of SRS SINR.
   * Exporting cell ID, RNTI, and SINR in linear unit.
   */
  TracedCallback<uint16_t, uint16_t, double> m_reportUeSinr;
  /**
   * The `UeSinrSamplePeriod` trace source. The sampling period for reporting
   * UEs' SINR stats.
   */
  uint16_t m_srsSamplePeriod;
  std::map <uint16_t,uint16_t> m_srsSampleCounterMap;

  /**
   * The `ReportInterference` trace source. Reporting the interference per PHY
   * RB (TS 36.214 section 5.2.2, measured on DATA). Exporting cell ID and
   * interference linear power per RB basis.
   */
  TracedCallback<uint16_t, Ptr<SpectrumValue> > m_reportInterferenceTrace;
  /**
   * The `InterferenceSamplePeriod` attribute. The sampling period for
   * reporting interference stats.
   * \todo In what unit is this?
   */
  uint16_t m_interferenceSamplePeriod;
  uint16_t m_interferenceSampleCounter;

  /**
   * The `DlPhyTransmission` trace source. Contains trace information regarding
   * PHY stats from DL Tx perspective. Exporting a structure with type
   * NistPhyTransmissionStatParameters.
   */
  TracedCallback<NistPhyTransmissionStatParameters> m_dlPhyTransmission;


}; // end of `class NistLteEnbPhy`


}

#endif /* LTE_ENB_PHY_H */
