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
 * Author: Nicola Baldo  <nbaldo@cttc.es>
 * Author: Marco Miozzo <mmiozzo@cttc.es>
 * Modified by: NIST
 */

#ifndef NIST_LTE_UE_MAC_ENTITY_H
#define NIST_LTE_UE_MAC_ENTITY_H




#include <map>

#include <ns3/nist-lte-mac-sap.h>
#include <ns3/nist-lte-ue-cmac-sap.h>
#include <ns3/nist-lte-ue-cphy-sap.h>
#include <ns3/nist-lte-ue-phy-sap.h>
#include <ns3/nist-lte-amc.h>
#include <ns3/nstime.h>
#include <ns3/event-id.h>
#include <vector>
#include <ns3/packet.h>
#include <ns3/packet-burst.h>
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"

namespace ns3 {

class UniformRandomVariable;

class NistLteUeMac :   public Object
{
  friend class NistUeMemberLteUeCmacSapProvider;
  friend class NistUeMemberLteMacSapProvider;
  friend class NistUeMemberLteUePhySapUser;

public:
  static TypeId GetTypeId (void);

  NistLteUeMac ();
  virtual ~NistLteUeMac ();
  virtual void DoDispose (void);

  NistLteMacSapProvider*  GetNistLteMacSapProvider (void);
  void  SetNistLteUeCmacSapUser (NistLteUeCmacSapUser* s);
  NistLteUeCmacSapProvider*  GetNistLteUeCmacSapProvider (void);

  
  /**
   * set the CPHY SAP this MAC should use to interact with the PHY
   *
   * \param s the CPHY SAP Provider
   */
  void SetNistLteUeCphySapProvider (NistLteUeCphySapProvider * s);
  //

  /**
  * \brief Get the PHY SAP user
  * \return a pointer to the SAP user of the PHY
  */
  NistLteUePhySapUser* GetNistLteUePhySapUser ();

  /**
  * \brief Set the PHY SAP Provider
  * \param s a pointer to the PHY SAP Provider
  */
  void SetNistLteUePhySapProvider (NistLteUePhySapProvider* s);
  
  /**
  * \brief Forwarded from NistLteUePhySapUser: trigger the start from a new frame
  *
  * \param frameNo frame number
  * \param subframeNo subframe number
  */
  void DoSubframeIndication (uint32_t frameNo, uint32_t subframeNo);

 /**
  * Assign a fixed random variable stream number to the random variables
  * used by this model.  Return the number of streams (possibly zero) that
  * have been assigned.
  *
  * \param stream first stream index to use
  * \return the number of stream indices assigned by this model
  */
  int64_t AssignStreams (int64_t stream);

  /**
   * \return the UE scheduler type
   *
   * This allows you to get access to the Ul scheduler value in the UE side.
   */
  std::string GetNistUlScheduler (void) const;

  /**
   * \set the UE scheduler type
   *
   * \param sched is the UE scheduler to set in the UE side 
   */
  void SetNistUlScheduler (std::string sched);

  /**
   * \return the UE selected resource mapping type
   *
   * This allows you to get access to the Sl UE selected resource value.
   */
  std::string GetNistSlUeSelResMapping (void) const;

  /**
   * \set the UE selected resource mapping type
   *
   * \param mapping is the UE selected mapping to set in the UE side
   */
  void SetNistSlUeSelResMapping (std::string mapping);
 
  /**
   * TracedCallback signature for transmission of discovery message.
   *
   * \param [in] rnti
   * \param [in] proSeAppCode
   */
  typedef void (* DiscoveryAnnouncementTracedCallback) 
    (const uint16_t rnti, const uint32_t proSeAppCode);

  /**
   *
   * \return discovery reception pools
   */
  std::list< Ptr<SidelinkRxDiscResourcePool> > GetDiscRxPools ();
 
  /**
   *
   * \return discovery transmission pools
   */
  Ptr<SidelinkTxDiscResourcePool> GetDiscTxPool ();


  /**
   *  TracedCallback signature for SL UL scheduling events.
   *
   * \param [in] frame Frame number
   * \param [in] subframe Subframe number
   * \param [in] rnti The C-RNTI identifying the UE
   * \param [in] mcs  The MCS for transport block
   * \param [in] pscch_ri The resource index in the PSCCH
   * \param [in] pssch_rb RB start in the PSSCH
   * \param [in] pssch_lenght Number of RBs in sub-channel in the PSSCH
   * \param [in] pssch_itrp TRP index used in PSSCH
   */
/*  typedef void (* SlUeSchedulingTracedCallback)
    (const uint32_t frame, const uint32_t subframe, const uint16_t rnti,     const uint8_t mcs, const uint16_t pscch_ri, const uint16_t pssch_rb, const uint16_t pssch_length, const uint16_t pssch_itrp);
*/

private:

  /**
   * The `DiscoveryMsgSent` trace source. Track the transmission of discovery message (announce)
   * Exporting RNTI, ProSe App Code.
   */
  TracedCallback<uint16_t, uint32_t> m_discoveryAnnouncementTrace;


  // forwarded from MAC SAP
  void DoTransmitPdu (NistLteMacSapProvider::NistTransmitPduParameters params);
  void DoReportBufferNistStatus (NistLteMacSapProvider::NistReportBufferNistStatusParameters params);

  // forwarded from UE CMAC SAP
  void DoConfigureRach (NistLteUeCmacSapProvider::NistRachConfig rc);
  void DoStartContentionBasedRandomAccessProcedure ();
  void DoStartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t rapId, uint8_t prachMask);
  void DoAddLc (uint8_t lcId, NistLteUeCmacSapProvider::NistLogicalChannelConfig lcConfig, NistLteMacSapUser* msu);
  void DoAddLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id, NistLteUeCmacSapProvider::NistLogicalChannelConfig lcConfig, NistLteMacSapUser* msu);
  void DoRemoveLc (uint8_t lcId);
  void DoRemoveLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id);
  void DoReset ();
  //communication
  void DoAddSlTxPool (uint32_t dstL2Id, Ptr<SidelinkTxCommResourcePool> pool);
  void DoRemoveSlTxPool (uint32_t dstL2Id);
  void DoSetSlRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools);
  void DoAddSlDestination (uint32_t destination);
  void DoRemoveSlDestination (uint32_t destination);
  //discovery
  void DoAddSlTxPool (Ptr<SidelinkTxDiscResourcePool> pool);
  void DoRemoveSlTxPool ();
  void DoSetSlRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools);
  void DoModifyDiscTxApps (std::list<uint32_t> apps);
  void DoModifyDiscRxApps (std::list<uint32_t> apps);

  void DoSetRnti (uint16_t);
  
  // forwarded from PHY SAP
  void DoReceivePhyPdu (Ptr<Packet> p);
  void DoReceiveNistLteControlMessage (Ptr<NistLteControlMessage> msg);
  
  // internal methods
  void RandomlySelectAndSendRaPreamble ();
  void SendRaPreamble (bool contention);
  void StartWaitingForRaResponse ();
  void RecvRaResponse (NistBuildNistRarListElement_s raResponse);
  void RaResponseTimeout (bool contention);
  void SendReportBufferNistStatus (void);
  void SendSidelinkReportBufferStatus (void);
  void RefreshHarqProcessesPacketBuffer (void);

  // Added function to assign priority to each flow 
  void DoAddLCPriority(uint8_t rnti, uint8_t lcid ,uint8_t  priority);

  // added to handle the different schedulers for the UE 
  void DoReceiveNistPFLteControlMessage (Ptr<NistLteControlMessage> msg);
  void DoReceiveNistMTLteControlMessage (Ptr<NistLteControlMessage> msg);
  void DoReceiveNistPrLteControlMessage (Ptr<NistLteControlMessage> msg);
  void DoReceiveNistRrLteControlMessage (Ptr<NistLteControlMessage> msg);

private:

  struct NistLcInfo
  {
    NistLteUeCmacSapProvider::NistLogicalChannelConfig lcConfig;
    NistLteMacSapUser* macSapUser;
  };

  std::map <uint8_t, NistLcInfo> m_lcInfoMap;
  NistLteUeCphySapProvider* m_cphySapProvider;
  NistLteMacSapProvider* m_macSapProvider;

  NistLteUeCmacSapUser* m_cmacSapUser;
  NistLteUeCmacSapProvider* m_cmacSapProvider;

  NistLteUePhySapProvider* m_uePhySapProvider;
  NistLteUePhySapUser* m_uePhySapUser;
  
  std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters> m_ulBsrReceived; // BSR received from RLC (the last one)
  // NIST: new structure for m_ulBsrReceived to handle UL per flow scheduling 
  std::map <uint8_t, std::map <uint8_t, NistLteMacSapProvider::NistReportBufferNistStatusParameters > > Nist_m_ulBsrReceived;
 
  // added map to handle priority  
  std::map <uint8_t, std::map <uint8_t, uint8_t> > PriorityMap;
  
  
  Time m_bsrPeriodicity;
  Time m_bsrLast;
  
  bool m_freshUlBsr; // true when a BSR has been received in the last TTI

  uint8_t m_harqProcessId;
  std::vector < Ptr<PacketBurst> > m_miUlHarqProcessesPacket; // Packets under trasmission of the UL HARQ processes
  std::vector < uint8_t > m_miUlHarqProcessesPacketTimer; // timer for packet life in the buffer

  uint16_t m_rnti;

  bool m_rachConfigured;
  NistLteUeCmacSapProvider::NistRachConfig m_rachConfig;
  uint8_t m_raPreambleId;
  uint8_t m_preambleTransmissionCounter;
  uint16_t m_backoffParameter;
  EventId m_noRaResponseReceivedEvent;
  Ptr<UniformRandomVariable> m_raPreambleUniformVariable;

  uint32_t m_frameNo;
  uint32_t m_subframeNo;
  uint8_t m_raRnti;
  bool m_waitingForRaResponse;
  
  //sidelink variables
  struct SidelinkLcIdentifier
  {
    uint8_t lcId;
    uint32_t srcL2Id;
    uint32_t dstL2Id;
  };
  
  friend bool operator < (const SidelinkLcIdentifier &l, const SidelinkLcIdentifier &r) { return l.lcId < r.lcId || (l.lcId == r.lcId && l.srcL2Id < r.srcL2Id) || (l.lcId == r.lcId && l.srcL2Id == r.srcL2Id && l.dstL2Id < r.dstL2Id); }
  
  std::map <SidelinkLcIdentifier, NistLcInfo> m_slLcInfoMap;
  Time m_slBsrPeriodicity;
  Time m_slBsrLast;
  bool m_freshSlBsr; // true when a BSR has been received in the last TTI
  std::map <SidelinkLcIdentifier, NistLteMacSapProvider::NistReportBufferNistStatusParameters> m_slBsrReceived; // BSR received from RLC (the last one)

  struct SidelinkGrant
  {
    //fields common with SL_DCI
    uint16_t m_resPscch;
    uint8_t m_tpc;
    uint8_t m_hopping;
    uint8_t m_rbStart; //models rb assignment
    uint8_t m_rbLen;   //models rb assignment
    uint8_t m_trp;

    //other fields
    uint8_t m_mcs;
    uint32_t m_tbSize;
  };

  struct PoolInfo
  {
    Ptr<SidelinkCommResourcePool> m_pool; //the pool
    SidelinkCommResourcePool::SubframeInfo m_currentScPeriod; //start of current period
    SidelinkGrant m_currentGrant; //grant for the next SC period
    SidelinkCommResourcePool::SubframeInfo m_nextScPeriod; //start of next period

    uint32_t m_npscch; // number of PSCCH available in the pool

    bool m_grant_received;
    SidelinkGrant m_nextGrant; //grant received for the next SC period

    std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> m_pscchTx;//list of PSCCH transmissions within the pool
    std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> m_psschTx; //list of PSSCH transmissions within the pool

    Ptr<PacketBurst> m_miSlHarqProcessPacket; // Packets under trasmission of the SL HARQ process
  };

  
  std::map <uint32_t, PoolInfo > m_sidelinkTxPoolsMap;
  std::list <Ptr<SidelinkRxCommResourcePool> > m_sidelinkRxPools;
  std::list <uint32_t> m_sidelinkDestinations;

  Ptr<NistLteAmc> m_amc; //needed now since UE is doing scheduling
  Ptr<UniformRandomVariable> m_ueSelectedUniformVariable;
  //fields for fixed UE_SELECTED pools
  uint8_t m_slKtrp;
  uint8_t m_slGrantMcs;
  uint8_t m_slGrantSize;
  uint8_t m_pucchSize;

  std::string m_NistUlScheduler;      // the UE scheduler attribute 

  //discovery
  struct DiscGrant
  {
    uint16_t m_rnti;
    uint8_t m_resPsdch;
    
    //uint32_t m_tbSize; //232 bits (fixed) 
  };

  struct DiscPoolInfo
  {
    Ptr<SidelinkTxDiscResourcePool> m_pool; //the pool
    SidelinkDiscResourcePool::SubframeInfo m_currentDiscPeriod; //start of current period
    DiscGrant m_currentGrant; //grant for the next discovery period
    SidelinkDiscResourcePool::SubframeInfo m_nextDiscPeriod; //start of next period

    uint32_t m_npsdch; // number of PSDCH available in the pool

    bool m_grant_received;
    DiscGrant m_nextGrant; //grant received for the next discovery period

    std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo> m_psdchTx;//list of PSDCH transmissions within the pool

    //HARQ is not considered for now
    //Ptr<PacketBurst> m_miSlHarqProcessPacket; // Packets under trasmission of the SL HARQ process
  };

  DiscPoolInfo m_discTxPools;
  std::list <Ptr<SidelinkRxDiscResourcePool> > m_discRxPools;

  std::list<uint32_t> m_discTxApps;
  std::list<uint32_t> m_discRxApps;

  Ptr<UniformRandomVariable> m_p1UniformVariable;
  Ptr<UniformRandomVariable> m_resUniformVariable;


  /**
   * Trace information regarding SL UE scheduling.
   * Frame number, Subframe number, RNTI, MCS, PSCCH resource index, PSSCH RB start, PSSCH lenght in RBs, PSSCH TRP index
   */
 TracedCallback<NistSlUeMacStatParameters> m_slUeScheduling;
 
 TracedCallback<NistSlUeMacStatParameters> m_slSharedChUeScheduling;

 /**
  * True if there is data to transmit in the PSSCH
  */
 bool m_slHasDataToTx;
 //The PHY notifies the change of timing as consequence of a change of SyncRef, the MAC adjust its timing
 void DoNotifyChangeOfTiming (uint32_t frameNo, uint32_t subframeNo);

};

} // namespace ns3

#endif // LTE_UE_MAC_ENTITY
