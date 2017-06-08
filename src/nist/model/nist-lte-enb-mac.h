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
 * Author: Marco Miozzo  <marco.miozzo@cttc.es>
 *         Nicola Baldo  <nbaldo@cttc.es>
 * Modified by: NIST
 */

#ifndef NIST_LTE_ENB_MAC_H
#define NIST_LTE_ENB_MAC_H


#include <map>
#include <vector>
#include <ns3/nist-lte-common.h>
#include <ns3/nist-lte-mac-sap.h>
#include <ns3/nist-lte-enb-cmac-sap.h>
#include <ns3/nist-ff-mac-csched-sap.h>
#include <ns3/nist-ff-mac-sched-sap.h>
#include <ns3/nist-lte-enb-phy-sap.h>
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include <ns3/packet.h>
#include <ns3/packet-burst.h>

namespace ns3 {

class NistDlCqiLteControlMessage;
class NistUlCqiLteControlMessage;
class NistPdcchMapLteControlMessage;

typedef std::vector <std::vector < Ptr<PacketBurst> > > DlHarqProcessesBuffer_t;

/**
 * This class implements the MAC layer of the eNodeB device
 */
class NistLteEnbMac :   public Object
{
  friend class NistEnbMacMemberLteEnbCmacSapProvider;
  friend class NistEnbMacMemberLteMacSapProvider<NistLteEnbMac>;
  friend class NistEnbMacMemberFfMacSchedSapUser;
  friend class NistEnbMacMemberFfMacCschedSapUser;
  friend class NistEnbMacMemberLteEnbPhySapUser;

public:
  static TypeId GetTypeId (void);

  NistLteEnbMac (void);
  virtual ~NistLteEnbMac (void);
  virtual void DoDispose (void);


  /**
   * \brief Set the scheduler SAP provider
   * \param s a pointer SAP provider of the FF packet scheduler
   */
  void SetNistFfMacSchedSapProvider (NistFfMacSchedSapProvider* s);
  /**
   * \brief Get the scheduler SAP user
   * \return a pointer to the SAP user of the scheduler
   */
  NistFfMacSchedSapUser* GetNistFfMacSchedSapUser (void);
  /**
   * \brief Set the control scheduler SAP provider
   * \param s a pointer to the control scheduler SAP provider
   */
  void SetNistFfMacCschedSapProvider (NistFfMacCschedSapProvider* s);
  /**
   * \brief Get the control scheduler SAP user
   * \return a pointer to the control scheduler SAP user
   */
  NistFfMacCschedSapUser* GetNistFfMacCschedSapUser (void);



  /**
   * \brief Set the MAC SAP user
   * \param s a pointer to the MAC SAP user
   */
  void SetNistLteMacSapUser (NistLteMacSapUser* s);
  /**
   * \brief Get the MAC SAP provider
   * \return a pointer to the SAP provider of the MAC
   */
  NistLteMacSapProvider* GetNistLteMacSapProvider (void);
  /**
   * \brief Set the control MAC SAP user
   * \param s a pointer to the control MAC SAP user
   */
  void SetNistLteEnbCmacSapUser (NistLteEnbCmacSapUser* s);
  /**
   * \brief Get the control MAC SAP provider
   * \return a pointer to the control MAC SAP provider
   */
  NistLteEnbCmacSapProvider* GetNistLteEnbCmacSapProvider (void);


  /**
  * \brief Get the eNB-PHY SAP User
  * \return a pointer to the SAP User of the PHY
  */
  NistLteEnbPhySapUser* GetNistLteEnbPhySapUser ();

  /**
  * \brief Set the PHY SAP Provider
  * \param s a pointer to the PHY SAP provider
  */
  void SetNistLteEnbPhySapProvider (NistLteEnbPhySapProvider* s);

  /**
   * TracedCallback signature for DL scheduling events.
   *
   * \param [in] frame Frame number.
   * \param [in] subframe Subframe number.
   * \param [in] rnti The C-RNTI identifying the UE.
   * \param [in] mcs0 The MCS for transport block.. 
   * \param [in] tbs0Size
   * \param [in] mcs1 The MCS for transport block.
   * \param [in] tbs1Size
   */
  typedef void (* DlSchedulingTracedCallback)
    (const uint32_t frame, const uint32_t subframe, const uint16_t rnti,
     const uint8_t mcs0, const uint16_t tbs0Size,
     const uint8_t mcs1, const uint16_t tbs1Size);

  /**
   *  TracedCallback signature for UL scheduling events.
   *
   * \param [in] frame Frame number.
   * \param [in] subframe Subframe number.
   * \param [in] rnti The C-RNTI identifying the UE.
   * \param [in] mcs  The MCS for transport block
   * \param [in] tbsSize
   */
  typedef void (* UlSchedulingTracedCallback)
    (const uint32_t frame, const uint32_t subframe, const uint16_t rnti,
     const uint8_t mcs, const uint16_t tbsSize);
  
private:

  /**
  * \brief Receive a DL CQI ideal control message
  * \param msg the DL CQI message
  */
  void ReceiveNistDlCqiLteControlMessage  (Ptr<NistDlCqiLteControlMessage> msg);

  void DoReceiveNistLteControlMessage (Ptr<NistLteControlMessage> msg);

  /**
  * \brief Receive a CE element containing the buffer status report
  * \param bsr the BSR message
  */
  void ReceiveBsrMessage  (NistMacCeListElement_s bsr);

 
  void DoUlCqiReport (NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters ulcqi);



  // forwarded from NistLteEnbCmacSapProvider
  void DoConfigureMac (uint8_t ulBandwidth, uint8_t dlBandwidth);
  void DoAddUe (uint16_t rnti);
  void DoRemoveUe (uint16_t rnti);
  void DoAddLc (NistLteEnbCmacSapProvider::NistLcInfo lcinfo, NistLteMacSapUser* msu);
  void DoReconfigureLc (NistLteEnbCmacSapProvider::NistLcInfo lcinfo);
  void DoReleaseLc (uint16_t  rnti, uint8_t lcid);
  void DoUeUpdateConfigurationReq (NistLteEnbCmacSapProvider::NistUeConfig params);
  void DoAddPool (uint32_t group, Ptr<SidelinkCommResourcePool> pool);
  void DoRemovePool (uint32_t group);

  NistLteEnbCmacSapProvider::NistRachConfig DoGetNistRachConfig ();
  NistLteEnbCmacSapProvider::NistAllocateNcRaPreambleReturnValue DoAllocateNcRaPreamble (uint16_t rnti);

  // forwarded from NistLteMacSapProvider
  void DoTransmitPdu (NistLteMacSapProvider::NistTransmitPduParameters);
  void DoReportBufferNistStatus (NistLteMacSapProvider::NistReportBufferNistStatusParameters);


  // forwarded from FfMacCchedSapUser
  void DoCschedCellConfigCnf (NistFfMacCschedSapUser::NistCschedCellConfigCnfParameters params);
  void DoCschedNistUeConfigCnf (NistFfMacCschedSapUser::NistCschedNistUeConfigCnfParameters params);
  void DoCschedLcConfigCnf (NistFfMacCschedSapUser::NistCschedLcConfigCnfParameters params);
  void DoCschedLcReleaseCnf (NistFfMacCschedSapUser::NistCschedLcReleaseCnfParameters params);
  void DoCschedUeReleaseCnf (NistFfMacCschedSapUser::NistCschedUeReleaseCnfParameters params);
  void DoCschedNistUeConfigUpdateInd (NistFfMacCschedSapUser::NistCschedNistUeConfigUpdateIndParameters params);
  void DoCschedCellConfigUpdateInd (NistFfMacCschedSapUser::NistCschedCellConfigUpdateIndParameters params);

  // forwarded from NistFfMacSchedSapUser
  void DoSchedDlConfigInd (NistFfMacSchedSapUser::NistSchedDlConfigIndParameters ind);
  void DoSchedUlConfigInd (NistFfMacSchedSapUser::NistSchedUlConfigIndParameters params);

  // forwarded from NistLteEnbPhySapUser
  void DoSubframeIndication (uint32_t frameNo, uint32_t subframeNo);
  void DoReceiveRachPreamble (uint8_t prachId);

public:
  // legacy public for use the Phy callback
  void DoReceivePhyPdu (Ptr<Packet> p);

private:
  void DoUlInfoListElementHarqFeeback (NistUlInfoListElement_s params);
  void DoDlInfoListElementHarqFeeback (NistDlInfoListElement_s params);

  //            rnti,             lcid, SAP of the RLC instance
  std::map <uint16_t, std::map<uint8_t, NistLteMacSapUser*> > m_rlcAttached;

  std::vector <NistCqiListElement_s> m_dlCqiReceived; // DL-CQI received
  std::vector <NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters> m_ulCqiReceived; // UL-CQI received
  std::vector <NistMacCeListElement_s> m_ulCeReceived; // CE received (BSR up to now)

  std::vector <NistDlInfoListElement_s> m_dlInfoListReceived; // DL HARQ feedback received

  std::vector <NistUlInfoListElement_s> m_ulInfoListReceived; // UL HARQ feedback received


  /*
  * Map of UE's info element (see 4.3.12 of FF MAC Scheduler API)
  */
//   std::map <uint16_t,NistUlInfoListElement_s> m_ulInfoListElements; 



  NistLteMacSapProvider* m_macSapProvider;
  NistLteEnbCmacSapProvider* m_cmacSapProvider;
  NistLteMacSapUser* m_macSapUser;
  NistLteEnbCmacSapUser* m_cmacSapUser;


  NistFfMacSchedSapProvider* m_schedSapProvider;
  NistFfMacCschedSapProvider* m_cschedSapProvider;
  NistFfMacSchedSapUser* m_schedSapUser;
  NistFfMacCschedSapUser* m_cschedSapUser;

  // PHY-SAP
  NistLteEnbPhySapProvider* m_enbPhySapProvider;
  NistLteEnbPhySapUser* m_enbPhySapUser;

  uint32_t m_frameNo;
  uint32_t m_subframeNo;
  /**
   * Trace information regarding DL scheduling
   * Frame number, Subframe number, RNTI, MCS of TB1, size of TB1,
   * MCS of TB2 (0 if not present), size of TB2 (0 if not present)
   */
  TracedCallback<uint32_t, uint32_t, uint16_t, uint8_t, uint16_t, uint8_t, uint16_t> m_dlScheduling;
  /**
   * Trace information regarding UL scheduling
   * Frame number, Subframe number, RNTI, MCS of TB, size of TB
   */
  TracedCallback<uint32_t, uint32_t, uint16_t, uint8_t, uint16_t> m_ulScheduling;
  
  uint8_t m_macChTtiDelay; // delay of MAC, PHY and channel in terms of TTIs


  std::map <uint16_t, DlHarqProcessesBuffer_t> m_miDlHarqProcessesPackets; // Packet under trasmission of the DL HARQ process
  
  uint8_t m_numberOfRaPreambles;
  uint8_t m_preambleTransMax;
  uint8_t m_raResponseWindowSize;

  /**
   * info associated with a preamble allocated for non-contention based RA
   * 
   */
  struct NistNcRaNistPreambleInfo
  {   
    uint16_t rnti; ///< rnti previously allocated for this non-contention based RA procedure
    Time expiryTime; ///< value the expiration time of this allocation (so that stale preambles can be reused)
  };

  /**
   * map storing as key the random acccess preamble IDs allocated for
   * non-contention based access, and as value the associated info
   * 
   */
  std::map<uint8_t, NistNcRaNistPreambleInfo> m_allocatedNcRaPreambleMap;
 
  std::map<uint8_t, uint32_t> m_receivedRachPreambleCount;

  std::map<uint8_t, uint32_t> m_rapIdRntiMap;
};

} // end namespace ns3

#endif /* LTE_ENB_MAC_ENTITY_H */
