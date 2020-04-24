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
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#ifndef LTE_UE_MAC_ENTITY_H
#define LTE_UE_MAC_ENTITY_H



#include <map>

#include <ns3/lte-mac-sap.h>
#include <ns3/lte-ue-cmac-sap.h>
#include <ns3/lte-ue-phy-sap.h>
#include <ns3/lte-amc.h>
#include <ns3/nstime.h>
#include <ns3/event-id.h>
#include <vector>
#include <ns3/packet.h>
#include <ns3/packet-burst.h>
#include <ns3/traced-callback.h>
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/lte-sl-header.h"

namespace ns3 {

class UniformRandomVariable;

class LteUeMac :   public Object
{
  /// allow UeMemberLteUeCmacSapProvider class friend access
  friend class UeMemberLteUeCmacSapProvider;
  /// allow UeMemberLteMacSapProvider class friend access
  friend class UeMemberLteMacSapProvider;
  /// allow UeMemberLteUePhySapUser class friend access
  friend class UeMemberLteUePhySapUser;

public:
 
 /// Scheduling grant metric used for UE_SELECTED scheduling
  enum SlSchedulingGrantMetric
  {
      FIXED = 0, //Default; Use values provided to UE MAC
      RANDOM, //Random selection among valid <MCS,PRB> pairs
      MIN_PRB, //Minimum number of PRBs
      MAX_COVERAGE //Maximum coverage, based on BLER curves
  };

  /**
   * \brief Get the type ID.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);

  LteUeMac ();
  virtual ~LteUeMac ();
  virtual void DoDispose (void);

  /**
   * \brief TracedCallback signature for RA response timeout events
   * exporting IMSI, contention flag, preamble transmission counter
   * and the max limit of preamble transmission
   *
   * \param [in] imsi
   * \param [in] contention
   * \param [in] preambleTxCounter
   * \param [in] maxPreambleTxLimit
   */
  typedef void (* RaResponseTimeoutTracedCallback)
    (uint64_t imsi, bool contention, uint8_t preambleTxCounter, uint8_t maxPreambleTxLimit);

  /**
  * \brief Get the LTE MAC SAP provider
  * \return a pointer to the LTE MAC SAP provider
  */
  LteMacSapProvider*  GetLteMacSapProvider (void);
  /**
  * \brief Set the LTE UE CMAC SAP user
  * \param s The LTE UE CMAC SAP User
  */
  void  SetLteUeCmacSapUser (LteUeCmacSapUser* s);
  /**
  * \brief Get the LTE CMAC SAP provider
  * \return a pointer to the LTE CMAC SAP provider
  */
  LteUeCmacSapProvider*  GetLteUeCmacSapProvider (void);

  /**
  * \brief Set the component carried ID
  * \param index The component carrier ID
  */
  void SetComponentCarrierId (uint8_t index);

  /**
  * \brief Get the PHY SAP user
  * \return A pointer to the SAP user of the PHY
  */
  LteUePhySapUser* GetLteUePhySapUser ();

  /**
  * \brief Set the PHY SAP Provider
  * \param s A pointer to the PHY SAP Provider
  */
  void SetLteUePhySapProvider (LteUePhySapProvider* s);

  /**
  * \brief Forwarded from LteUePhySapUser: trigger the start from a new frame
  *
  * \param frameNo The frame number
  * \param subframeNo The subframe number
  */
  void DoSubframeIndication (uint32_t frameNo, uint32_t subframeNo);

  /**
   * Execute the Sidelink subframe indication. Expected to occur after a
   * given delay from the start of the subframe in order to wait to know
   * if there are Uplink transmissions before processing the Sidelink
   * transmissions
   *
   * \param frameNo The frame number
   * \param subframeNo The subframe number
   */
  void DoSlDelayedSubframeIndication (uint32_t frameNo, uint32_t subframeNo);

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
   *\brief Get the discovery Rx pools
   *
   * \return list of discovery reception pools
   */
  std::list< Ptr<SidelinkRxDiscResourcePool> > GetDiscRxPools ();

  /**
   *\brief Get the discovery Tx pool
   *
   * \return pointer to the discovery transmission pool
   */
  Ptr<SidelinkTxDiscResourcePool> GetDiscTxPool ();

private:
  // forwarded from MAC SAP
  /**
   * Transmit PDU function
   *
   * \param params LteMacSapProvider::TransmitPduParameters
   */
  void DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params);
  /**
   * Report buffers status function
   *
   * \param params LteMacSapProvider::ReportBufferStatusParameters
   */
  void DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params);

  // forwarded from UE CMAC SAP
  /**
   * Configure RACH function
   *
   * \param rc LteUeCmacSapProvider::RachConfig
   */
  void DoConfigureRach (LteUeCmacSapProvider::RachConfig rc);
  /**
   * Start contention based random access procedure function
   */
  void DoStartContentionBasedRandomAccessProcedure ();
  /**
   * Set RNTI
   *
   * \param rnti The RNTI of the UE
   */
  void DoSetRnti (uint16_t rnti);
  /**
   * Start non contention based random access procedure function
   *
   * \param rnti The RNTI
   * \param rapId The RAPID
   * \param prachMask The PRACH mask
   */
  void DoStartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t rapId, uint8_t prachMask);
  /**
   * Add LC function
   *
   * \param lcId The logical channel id
   * \param lcConfig The logical channel config
   * \param msu The corresponding LteMacSapUser
   */
  void DoAddLc (uint8_t lcId, LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu);
  /**
   * Remove LC function
   *
   * \param lcId The logical channel id
   */
  void DoRemoveLc (uint8_t lcId);
  /**
   * \brief Reset function
   */
  void DoReset ();
  /**
   * \brief Notify MAC about the successful RRC connection
   * establishment.
   */
  void DoNotifyConnectionSuccessful ();
  /**
   * Set IMSI
   *
   * \param imsi the IMSI of the UE
   */
  void DoSetImsi (uint64_t imsi);
  /**
   * Adds a new Logical Channel (LC) used for Sidelink
   *
   * \param lcId The ID of the logical channel
   * \param srcL2Id The L2 id of the source
   * \param dstL2Id The L2 id of the destination
   * \param lcConfig The LC configuration provided by the RRC
   * \param msu The corresponding LteMacSapUser
   */
  void DoAddSlLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id, LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu);
  /**
   * Removes an existing Sidelink LC
   *
   * \param lcId The LC ID
   * \param srcL2Id The L2 id of the source
   * \param dstL2Id The L2 id of the destination
   */
  void DoRemoveSlLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id);

  //Sidelink communication

  /**
   * Add Sidelink communication transmission pool function
   * Adds transmission pool for Sidelink communication
   *
   * \param dstL2Id The destination Layer 2 ID
   * \param pool The pointer to the SidelinkTxCommResourcePool
   */
  void DoAddSlCommTxPool (uint32_t dstL2Id, Ptr<SidelinkTxCommResourcePool> pool);
  /**
   * Remove Sidelink communication transmission pool function
   * Removes transmission pool for Sidelink communication
   *
   * \param dstL2Id The destination Layer 2 ID
   */
  void DoRemoveSlCommTxPool (uint32_t dstL2Id);
  /**
   * Set Sidelink communication pool function
   * Sets reception pools for Sidelink communication
   *
   * \param pools The list of reception pools for Sidelink communication
   */
  void DoSetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools);
  /**
   * Add Sidelink destination function
   * Adds a new destination for Sidelink communication to listen
   *
   * \param destination The destination Layer 2 ID
   */
  void DoAddSlDestination (uint32_t destination);
  /**
   * Remove Sidelink destination function
   * Removes a destination from the list destinations of Sidelink communication
   *
   * \param destination The destination Layer 2 ID
   */
  void DoRemoveSlDestination (uint32_t destination);

  //Sidelink discovery

  /**
   * Set Sidelink discovery transmission pool function
   * Sets transmission pool for Sidelink discovery
   *
   * \param pool The pointer to the SidelinkTxDiscResourcePool
   */
  void DoSetSlDiscTxPool (Ptr<SidelinkTxDiscResourcePool> pool);
  /**
   * Remove Sidelink discovery transmission pool function
   * Removes transmission pool for Sidelink discovery
   */
  void DoRemoveSlDiscTxPool ();
  /**
   * Set Sidelink discovery reception pool function
   * Sets reception pool for Sidelink discovery
   *
   * \param pools The pointer to the SidelinkRxDiscResourcePool
   */
  void DoSetSlDiscRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools);

  // forwarded from PHY SAP
  /**
   * Receive Phy PDU function
   *
   * \param p The packet
   */
  void DoReceivePhyPdu (Ptr<Packet> p);
  /**
   * Receive Phy PDU function
   *
   * \param p The packet
   */
  void DoReceiveSlDiscPhyPdu (Ptr<Packet> p);
  /**
   * Receive Phy PDU function
   *
   * \param p The packet
   */
  void DoReceiveSlSciPhyPdu (Ptr<Packet> p);
  /**
   * Receive LTE control message function
   *
   * \param msg The LTE control message
   */
  void DoReceiveLteControlMessage (Ptr<LteControlMessage> msg);
  /**
   * Notify change of timing function
   * The PHY notifies the change of timing as consequence of a change of SyncRef, the MAC adjust its timing
   *
   * \param frameNo The current PHY frame number
   * \param subframeNo The current PHY subframe number
   */
  void DoNotifyChangeOfTiming (uint32_t frameNo, uint32_t subframeNo);
  /**
   * Notify Sidelink enabled function
   * The PHY notifies the MAC the Sidelink is activated
   */
  void DoNotifySidelinkEnabled ();

  /**
   * Notify uplink transmission generated by PHY
   * The PHY notifies the MAC it scheduled a transmission
   */
  void DoNotifyUlTransmission ();

  // internal methods
  /// Randomly select and send RA preamble function
  void RandomlySelectAndSendRaPreamble ();
  /**
   * Send RA preamble function
   *
   * \param contention if true randomly select and send the RA preamble
   */
  void SendRaPreamble (bool contention);
  /**
   * Start waiting for RA response function
   */
  void StartWaitingForRaResponse ();
  /**
   * Receive the RA response function
   *
   * \param raResponse The random access response received
   */
  void RecvRaResponse (BuildRarListElement_s raResponse);
  /**
   * RA response timeout function
   *
   * \param contention if true randomly select and send the RA preamble
   */
  void RaResponseTimeout (bool contention);
  /**
   * Send report buffer status
   */
  void SendReportBufferStatus (void);
  /**
   * Send Sidelink report buffer status
   */
  void SendSidelinkReportBufferStatus (void);
  /**
   * Refresh HARQ processes packet buffer function
   */
  void RefreshHarqProcessesPacketBuffer (void);

  /// component carrier Id --> used to address sap
  uint8_t m_componentCarrierId;
  
  /// Sidelink grant related variables
  struct SidelinkGrant
  {
    //fields common with SL_DCI
    uint16_t m_resPscch; ///< Resource for PSCCH
    uint8_t m_tpc; ///< TPC
    uint8_t m_hopping; ///< hopping flag
    uint8_t m_rbStart; ///< models RB assignment
    uint8_t m_rbLen; ///< models RB assignment
    uint8_t m_hoppingInfo; ///< models RB assignment when hopping is enabled
    uint8_t m_iTrp; ///< Index of Time recourse pattern (TRP)

    //other fields
    uint8_t m_mcs; ///< Modulation and Coding Scheme
    uint32_t m_tbSize; ///< Transport Block Size
    
    //the destination of the grant
    uint32_t m_dst; ///< Current destination L2 ID 
  };

  /// Sidelink communication pool information
  struct PoolInfo
  {
    Ptr<SidelinkTxCommResourcePool> m_pool; ///< The Sidelink communication resource pool
    std::set<uint32_t> m_destinations; ///< list of destinations that use this pool
    SidelinkCommResourcePool::SubframeInfo m_currentScPeriod; ///< Start of the current Sidelink Control (SC) period
    SidelinkGrant m_currentGrant; ///< Grant for the next SC period
    SidelinkCommResourcePool::SubframeInfo m_nextScPeriod; ///< Start of next SC period

    uint32_t m_npscch; ///< Number of PSCCH available in the pool

    bool m_grantReceived; ///< True if we receive the grant
    SidelinkGrant m_nextGrant; ///< Grant received for the next SC period

    std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> m_pscchTx; ///< List of PSCCH transmissions within the pool
    std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> m_psschTx; ///< List of PSSCH transmissions within the pool

    Ptr<PacketBurst> m_miSlHarqProcessPacket; ///< Packets under transmission of the SL HARQ process
  };

  /**
   * Compute the next grant for the given pool
   * \param poolIt The iterator to the pool to schedule
   * \return The grant for the next SC period of the pool
   */
  SidelinkGrant GetSlUeSelectedGrant (std::list<PoolInfo>::iterator poolIt);

  /**
   * Get the amount of data in the queue for the given destination
   * \param dstL2Id The destination L2 ID
   * \return The amount of data in the queue
   */
  uint32_t GetQueueSize (uint32_t dstL2Id);
  
  /**
   * Get an iterator to the pool that must be used for a given destination
   * \param dstL2Id
   * \return an iterator to the pool that must be used for a given destination
   */
  std::list <PoolInfo>::iterator GetPoolForDestination (uint32_t dstL2Id);
  
private:
  /// LcInfo structure
  struct LcInfo
  {
    LteUeCmacSapProvider::LogicalChannelConfig lcConfig; ///< logical channel config
    LteMacSapUser* macSapUser; ///< MAC SAP user
  };

  std::map <uint8_t, LcInfo> m_lcInfoMap; ///< logical channel info map

  LteMacSapProvider* m_macSapProvider; ///< MAC SAP provider

  LteUeCmacSapUser* m_cmacSapUser; ///< CMAC SAP user
  LteUeCmacSapProvider* m_cmacSapProvider; ///< CMAC SAP provider

  LteUePhySapProvider* m_uePhySapProvider; ///< UE Phy SAP provider
  LteUePhySapUser* m_uePhySapUser; ///< UE Phy SAP user

  std::map <uint8_t, LteMacSapProvider::ReportBufferStatusParameters> m_ulBsrReceived; ///< BSR received from RLC (the last one)


  Time m_bsrPeriodicity; ///< BSR periodicity
  Time m_bsrLast; ///< BSR last

  bool m_freshUlBsr; ///< true when a BSR has been received in the last TTI

  uint8_t m_harqProcessId; ///< HARQ process ID
  std::vector < Ptr<PacketBurst> > m_miUlHarqProcessesPacket; ///< Packets under transmission of the UL HARQ processes
  std::vector < uint8_t > m_miUlHarqProcessesPacketTimer; ///< timer for packet life in the buffer

  uint16_t m_rnti; ///< RNTI
  uint16_t m_imsi; ///< IMSI

  bool m_rachConfigured; ///< is RACH configured?
  LteUeCmacSapProvider::RachConfig m_rachConfig; ///< RACH configuration
  uint8_t m_raPreambleId; ///< RA preamble ID
  uint8_t m_preambleTransmissionCounter; ///< preamble transmission counter
  uint16_t m_backoffParameter; ///< backoff parameter
  EventId m_noRaResponseReceivedEvent; ///< no RA response received event ID
  Ptr<UniformRandomVariable> m_raPreambleUniformVariable; ///< RA preamble random variable

  uint32_t m_frameNo; ///< frame number
  uint32_t m_subframeNo; ///< subframe number
  uint8_t m_raRnti; ///< RA RNTI
  bool m_waitingForRaResponse; ///< waiting for RA response

  /**
   * \brief The `RaResponseTimeout` trace source. Fired RA response timeout.
   * Exporting IMSI, contention flag, preamble transmission counter
   * and the max limit of preamble transmission.
   */
  TracedCallback<uint64_t, bool, uint8_t, uint8_t> m_raResponseTimeoutTrace;
  /// Sidelink Communication related variables
  struct SidelinkLcIdentifier
  {
    uint8_t lcId; ///< Sidelink LCID
    uint32_t srcL2Id; ///< Source L2 ID
    uint32_t dstL2Id; ///< Destination L2 ID
  };

  /**
   * Less than operator
   *
   * \param l first SidelinkLcIdentifier
   * \param r second SidelinkLcIdentifier
   * \returns true if first SidelinkLcIdentifier parameter values are less than the second SidelinkLcIdentifier parameters"
   */
  friend bool operator < (const SidelinkLcIdentifier &l, const SidelinkLcIdentifier &r)
  {
    return l.lcId < r.lcId || (l.lcId == r.lcId && l.srcL2Id < r.srcL2Id) || (l.lcId == r.lcId && l.srcL2Id == r.srcL2Id && l.dstL2Id < r.dstL2Id);
  }

  std::map <SidelinkLcIdentifier, LcInfo> m_slLcInfoMap; ///< Sidelink logical channel info map
  Time m_slBsrPeriodicity; ///< Sidelink buffer status report periodicity
  Time m_slBsrLast; ///< Time of last transmitted Sidelink buffer status report
  bool m_freshSlBsr; ///< true when a BSR has been received in the last TTI
  std::map <SidelinkLcIdentifier, LteMacSapProvider::ReportBufferStatusParameters> m_slBsrReceived; ///< Sidelink BSR received from RLC (the last one)

  std::list <PoolInfo> m_sidelinkTxPools; ///< List of Sidelink communication reception pools
  std::list <Ptr<SidelinkRxCommResourcePool> > m_sidelinkRxPools; ///< List of Sidelink communication reception pools
  std::list <uint32_t> m_sidelinkDestinations; ///< List of Sidelink communication destinations
  std::set <SidelinkCommResourcePool::SubframeInfo> m_psschRxSet; ///< List of reception opportunities on PSSCH

  Ptr<LteAmc> m_amc; ///< Pointer to LteAmc class; needed now since UE is doing scheduling
  Ptr<UniformRandomVariable> m_ueSelectedUniformVariable;  ///<  A uniform random variable used to choose random resources, RB start
  ///<  and iTrp values in UE selected mode
  //fields for fixed UE_SELECTED pools
  uint8_t m_slKtrp; ///< Number of active resource blocks in the TRP used.
  uint8_t m_setTrpIndex; ///< TRP index to be used
  bool m_useSetTrpIndex; ///< True if TRP index set, i.e., m_setTrpIndex to be used
  uint8_t m_slGrantMcs; ///< Sidelink grant MCS
  uint8_t m_slGrantSize; ///< The number of RBs allocated per UE for Sidelink

  /// Sidelink discovery grant related variables
  struct DiscGrant
  {
    uint8_t m_resPsdch; ///< A randomly chosen resource index from the PSDCH resource pool
    Ptr<Packet> m_discMsg; ///< The discovery message to send in the selected resource
    std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo> m_psdchTx; ///< List of PSDCH transmissions within the pool
  };

  /// Sidelink discovery pool information
  struct DiscPoolInfo
  {
    Ptr<SidelinkTxDiscResourcePool> m_pool; ///< The Sidelink discovery transmission pool
    SidelinkDiscResourcePool::SubframeInfo m_currentDiscPeriod; ///< Start of the current discovery period
    std::list <DiscGrant> m_currentGrants; ///< Grants for the current discovery period
    SidelinkDiscResourcePool::SubframeInfo m_nextDiscPeriod; ///< Start of next discovery period

    uint32_t m_npsdch; ///< Number of PSDCH available in the pool

    std::list <DiscGrant> m_nextGrants; ///< Grants received for the next discovery period
  };

  DiscPoolInfo m_discTxPool; ///< Sidelink discovery transmission pool
  std::list <Ptr<SidelinkRxDiscResourcePool> > m_discRxPools; ///< Sidelink discovery reception pool

  std::list< Ptr<Packet> > m_discPendingTxMsgs; ///< List of discovery messages to send

  Ptr<UniformRandomVariable> m_p1UniformVariable; ///< A uniform random variable to compare with the Tx probability of UE selected pool
  Ptr<UniformRandomVariable> m_resUniformVariable; ///< A uniform random variable to randomly choose the resource index from the PSDCH resource pool

  Ptr<Packet> m_slSynchPendingTxMsg; ///< MIB-SL message to send


  /**
   * Trace information regarding Sidelink PSCCH UE scheduling.
   * SlUeMacStatParameters (see lte-common.h)
   */
  TracedCallback<SlUeMacStatParameters> m_slPscchScheduling; //m_slUeScheduling

  /**
   * Trace information regarding Sidelink PSSCH UE scheduling.
   * SlUeMacStatParameters (see lte-common.h)
   */
  TracedCallback<SlUeMacStatParameters> m_slPsschScheduling; //m_slSharedChUeScheduling

  /**
   * Trace information regarding Sidelink PSDCH UE scheduling
   * SlUeMacStatParameters (see lte-common.h)
   */
  TracedCallback<SlUeMacStatParameters, LteSlDiscHeader> m_slPsdchScheduling; //m_slPsdchScheduling

  bool m_slHasDataToTxInPeriod; ///< True if there is data to transmit in the PSSCH

  bool m_sidelinkEnabled; ///< True if Sidelink is used

  bool m_hasUlToTx; ///< True if uplink sent data to the phy in the current TTI/subframe

  bool m_hasSlMibToTx; ///< True if sidelink MIB sent to the phy in the current TTI/subframe

  bool m_hasSlCommToTx; ///< True if sidelink communication (PSCCH or PSSCH) sent to the phy in the current TTI/subframe

  bool m_hasSlCommToRx; ///< True if sidelink communication is expected in the current TTI/subframe

  SidelinkCommResourcePool::SubframeInfo m_slSchedTime; ///< Current time regarding sidelink scheduling
  
  SlSchedulingGrantMetric m_schedulingGrantMetric; ///< Metrics to allocate resource in sidelink
};

} // namespace ns3

#endif // LTE_UE_MAC_ENTITY
