/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef NR_UE_MAC_H
#define NR_UE_MAC_H

#include "nr-phy-mac-common.h"
#include "nr-mac-pdu-info.h"

#include <ns3/lte-ue-cmac-sap.h>
#include <ns3/lte-ccm-mac-sap.h>
#include <ns3/traced-callback.h>

#include <unordered_map>
#include <queue>

#include <ns3/nr-sl-mac-sap.h>
#include <ns3/nr-sl-ue-cmac-sap.h>
#include <ns3/nr-sl-comm-resource-pool.h>
#include "nr-sl-ue-phy-sap.h"
#include "nr-sl-ue-mac-sched-sap.h"
#include "nr-sl-phy-mac-common.h"
#include "nr-sl-ue-mac-scheduler.h"
#include <unordered_set>
#include <map>

// test classes outside of namespace ns3
class NrSensingTestCase;
class NrSlRemoveOldSensingDataTest;
class NrSensingTransmitHistoryTest;

namespace ns3 {

class NrUePhySapUser;
class NrPhySapProvider;
class NrControlMessage;
class UniformRandomVariable;
class PacketBurst;
class NrUlDciMessage;
class NrSlUeMacCschedSapProvider;
class NrSlUeMacCschedSapUser;
class NrSlUeMacHarq;

/**
 * \ingroup ue-mac
 * \brief The MAC class for the UE
 *
 * \section ue_mac_general General information
 *
 * The UE MAC has not much freedom, as it is directed by the GNB at which it
 * is attached to. For the attachment phase, we follow (more or less) what is
 * happening in LENA. After the UE is attached, the things become different.
 *
 * \section ue_mac_sr Scheduling Request
 *
 * When a RLC (please remember, there are as many RLC as many bearer the UE has)
 * tells the UE that there is some data in its queue, the UE MAC activates the
 * SR state machine. So, the UE sends a control message (called Scheduling Request)
 * to the PHY, that in turn has to deliver it to the GNB. This message says that
 * the UE has some data to transmit (without indicating the precise quantity).
 *
 * The GNB then allocates some data (a quantity that is implementation-defined)
 * to the UE, in which it can transmit data and a SHORT_BSR.
 *
 * \section ue_mac_response_to_dci Response to a DCI
 *
 * When the UE receives an UL_DCI, it can use a part of it to send a Control Element.
 * The most used control element (and, by the way, the only we support right now)
 * is SHORT_BSR, in which the UE informs the GNB of its buffer status. The rest
 * of the bytes can be used to send data.
 *
 * In the standard, the UE is allowed to send a single PDU in response to a
 * single UL DCI. Such PDU can be formed by one or more subPDU, each one consisting
 * in a header and a data (the data is optional). However, due to limitations in
 * serialization/deserialization of packets in the ns-3 simulator, we are bending
 * a little the definition. The MAC is allowed to send as many PDU as it wants,
 * but these PDU (that are, in fact, packets) should be enqueued in the PHY at
 * the same frame/subframe/slot/symbol. The PHY will use the concept of `PacketBurst`
 * to consider all these PDUs as a single, big, PDU. Practically speaking, the
 * result is the same as grouping these subPDU in a single PDU, just that the
 * single PDU is in reality a PacketBurst. As the order in a PacketBurst cannot
 * be maintained, it is impossible to respect the standard at the ordering part
 * (DL and UL PDU are formed in an opposite way, with CE at the beginning or
 * at the end of the PDU).
 *
 * The action of sending a subPDU to the PHY is done by the method DoTransmitPdu().
 * However, the MAC has to evaluate the received TBS, in light of how many
 * Logical Channel ID are active, and what data they have to transmit. In doing
 * all this, the MAC has to keep in consideration that each subPDU will have
 * a header prefixed, which is an overhead, using bytes that were originally
 * supposed to be assigned to data.
 *
 * The core of this small "scheduling" is done in method SendNewData(), in which
 * the MAC will try to send as many status-subPDUs as possible, then will try
 * to send as many as retx-subPDUs as possible, and finally as many as tx-subPDUs
 * as possible. At the end of all the subPDUs, it will be sent a SHORT_BSR, to
 * indicate to the GNB the new status of the RLC queues. As the SHORT_BSR is
 * a CE and is treated in the same way as data, it may be lost. Please note that
 * the code substract the amount of bytes devoted to the SHORT_BSR from the
 * available ones, so there will always be a space to send it. The only
 * exception (theoretically possible) is when the status PDUs use all the
 * available space; in this case, a rework of the code will be needed.
 *
 * The SHORT_BSR is not reflecting the standard, but it is the same data that
 * was sent in LENA, indicating the status of 4 LCG at once with an 8-bit value.
 * Making this part standard-compliant is a good novice exercise.
 *
 * \section ue_mac_configuration Configuration
 *
 * The user can configure the class using the method NrHelper::SetUeMacAttribute(),
 * or by directly calling `SetAttribute` on the MAC pointer. The list of
 * attributes is reported below, in the Attributes section.
 *
 * \section ue_mac_traces CTRL-trace Traces for CTRL messages
 *
 * The class has two attributes that signals to the eventual listener the
 * transmission or the reception of CTRL messages. One is UeMacRxedCtrlMsgsTrace,
 * and the other is UeMacTxedCtrlMsgsTrace. For what regards the PHY, you will
 * find more information in the NrUePhy class documentation.
 */
class NrUeMac : public Object
{
  friend class UeMemberNrUeCmacSapProvider;
  friend class UeMemberNrMacSapProvider;
  friend class MacUeMemberPhySapUser;
  //NR Sidelink
  /// allow MemberNrSlMacSapProvider<NrUeMac> class friend access
  friend class MemberNrSlMacSapProvider<NrUeMac>;
  /// allow MemberNrSlUeCmacSapProvider<NrUeMac> class friend access
  friend class MemberNrSlUeCmacSapProvider<NrUeMac>;
  /// allow MemberNrSlUePhySapUser<NrUeMac> class friend access
  friend class MemberNrSlUePhySapUser<NrUeMac>;
  /// allow MemberNrSlUeMacCschedSapUser<NrUeMac> class friend access
  friend class MemberNrSlUeMacCschedSapUser;
  /// allow MemberNrSlUeMacSchedSapUser<NrUeMac> class friend access
  friend class MemberNrSlUeMacSchedSapUser;
  // Unit-test access to protected/private members
  friend class ::NrSensingTestCase;
  friend class ::NrSlRemoveOldSensingDataTest;
  friend class ::NrSensingTransmitHistoryTest;

public:
  /**
   * \brief Get the Type id
   * \return the type id
   */
  static TypeId GetTypeId (void);

  /**
   * \brief NrUeMac constructor
   */
  NrUeMac (void);
  /**
    * \brief Deconstructor
    */
  ~NrUeMac (void) override;


  /**
   * \brief Set the C MAC SAP user (AKA the RRC representation for the MAC)
   * \param s the SAP pointer
   */
  void  SetUeCmacSapUser (LteUeCmacSapUser* s);

  /**
   * \brief Get the C MAC SAP provider (AKA the MAC representation for the RRC)
   * \return  C MAC SAP provider (AKA the MAC representation for the RRC)
   */
  LteUeCmacSapProvider*  GetUeCmacSapProvider (void);

  /**
   * \brief Get the Mac SAP provider (AKA the MAC representation for the RLC)
   * \return the Mac SAP provider (AKA the MAC representation for the RLC)
   */
  LteMacSapProvider*  GetUeMacSapProvider (void);

  /**
   * \brief Get the PHY SAP User (AKA the MAC representation for the PHY)
   * \return the PHY SAP User (AKA the MAC representation for the PHY)
   */
  NrUePhySapUser* GetPhySapUser ();

  /**
   * \brief Set PHY SAP provider (AKA the PHY representation for the MAC)
   * \param ptr the PHY SAP provider (AKA the PHY representation for the MAC)
   */
  void SetPhySapProvider (NrPhySapProvider* ptr);

  /**
   *  TracedCallback signature for Ue Mac Received Control Messages.
   * \param [in] sfnSf Frame number, subframe number, slot number, VarTti
   * \param [in] nodeId the node ID
   * \param [in] rnti the RNTI
   * \param [in] bwpId the BWP ID
   * \param [in] ctrlMessage the pointer to msg to get the msg type
   */
  typedef void (* RxedUeMacCtrlMsgsTracedCallback)
    (const SfnSf sfnSf, const uint16_t nodeId, const uint16_t rnti,
     const uint8_t bwpId, Ptr<NrControlMessage> ctrlMessage);
  /**
   *  TracedCallback signature for Ue Mac Transmitted Control Messages.
   * \param [in] sfnSf the frame number, subframe number, slot number, VarTti
   * \param [in] nodeId the node ID
   * \param [in] rnti the RNTI
   * \param [in] bwpId the BWP ID
   * \param [in] ctrlMessage the pointer to msg to get the msg type
   */
  typedef void (* TxedUeMacCtrlMsgsTracedCallback)
    (const SfnSf sfnSf, const uint16_t nodeId, const uint16_t rnti,
     const uint8_t bwpId, Ptr<NrControlMessage> ctrlMessage);

  /**
   * \brief Sets the number of HARQ processes.
   * Called by the helper at the moment of UE attachment
   * \param numHarqProcesses the maximum number of harq processes
   */
  void SetNumHarqProcess (uint8_t numHarqProcesses);

  /**
   * \brief Please remember that this number is obtained by the GNB, the UE
   * cannot configure it.
   *
   * \return number of HARQ processes
   */
  uint8_t GetNumHarqProcess () const;

  /**
   * \brief Set pointer to NR sidelink scheduler
   * \param scheduler Pointer to scheduler
   */
  void SetNrSlUeMacScheduler (Ptr<NrSlUeMacScheduler> scheduler);

  /**
   * In resource allocation mode 2, the higher layer can request the UE to
   * determine a subset of resources from which the higher layer will select
   * resources for PSSCH/PSCCH transmission. This function first call the
   * GetNrSlCandidateResources function to determine the set of candidate
   * resources according to 3GPP TR 38.214 v16.7.0 Section 8.1.4 and then
   * it filters out the resources that are already part of a published grant
   * by calling the function FilterNrSlCandidateResources.
   *
   * \brief Get NR sidelink available resources
   *
   * \param sfn The current system frame, subframe, and slot number.
   * \param params The input transmission parameters for the algorithm
   * \return The list of the transmit opportunities (slots) as per the TDD pattern
   *         and the NR SL bitmap
   */
  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> GetNrSlAvailableResources (const SfnSf& sfn, const NrSlTransmissionParams& params);

  /**
   * This method implements the algorithm specified in 3GPP TR 38.214 v16.7.0
   * Section 8.1.4.
   *
   * \brief Get NR sidelink candidate single-slot resources
   * 
   * \param sfn The current system frame, subframe, and slot number.
   * \param params The input transmission parameters for the algorithm
   * \return The list of the transmit opportunities (slots) as per the TDD pattern
   *         and the NR SL bitmap
   */
  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> GetNrSlCandidateResources (const SfnSf& sfn, const NrSlTransmissionParams& params);

  /**
   * \brief Get the reservation period in slots
   *
   * Wrapper method that calls NrSlCommResourcePool::GetResvPeriodInSlots
   *
   * \param resvPeriod The reservation period
   * \return The reservation period in slots
   */
  uint16_t GetResvPeriodInSlots (Time resvPeriod) const;

  /**
   * \brief Get NR Sidelink subchannel size
   *
   * Wrapper method that calls NrSlCommResourcePool::GetNrSlSubChSize
   *
   * \return The subchannel size in RBs
   */
  uint16_t GetNrSlSubChSize () const;

  /**
   * \brief Get the PSFCH period for the resource pool
   *
   * Wrapper method that calls NrSlCommResourcePool::GetPsfchPeriod
   *
   * \return value of the PsfchPeriod, in slots
   */
  uint8_t GetPsfchPeriod () const;

  /**
   * \brief Get the number of PSSCH symbols per slot
   *
   * For further study:  The number of symbols per slot will be variable
   * depending  on whether PSFCH is present, but for the moment, PSFCH is
   * not modeled
   *
   * \return The number of PSSCH symbols per slot
   */
  uint16_t GetNrSlPsschSymbolsPerSlot () const;

  /**
   * \brief Assign a fixed random variable stream number to the random variables
   * used by this model. Returns the number of streams (possibly zero) that
   * have been assigned.
   *
   * \param stream first stream index to use
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

protected:
  /**
   * \brief DoDispose method inherited from Object
   */
  void virtual DoDispose () override;
  /**
   * \brief Get the bwp id of this MAC
   * \return the bwp id
   */
  uint16_t GetBwpId () const;

  /**
   * \brief Get the cell id of this MAC
   * \return the cell id
   */
  uint16_t GetCellId () const;

private:
  /**
   * \brief Received a RA response
   * \param raResponse the response
   */
  void RecvRaResponse (BuildRarListElement_s raResponse);
  /**
   * \brief Set the RNTI
   */
  void SetRnti (uint16_t);
  /**
   * \brief Do some work, we begin a new slot
   * \param sfn the new slot
   */
  void DoSlotIndication (const SfnSf &sfn);

  /**
   * \brief Get the total size of the RLC buffers.
   * \return The number of bytes that are in the RLC buffers
   */
  uint32_t GetTotalBufSize () const __attribute__((warn_unused_result));

  /**
   * \brief Send to the PHY a SR
   */
  void SendSR () const;
  /**
   * \brief Called by RLC to transmit a RLC PDU
   * \param params the RLC params
   *
   * Please note that this call is triggered by communicating to the RLC
   * that there is a new transmission opportunity with NotifyTxOpportunity().
   *
   * The method is called DoTransmitPdu, however, it may happen that multiple
   * PDUs need to be send in the same frame/subframe/slot/symbol, in this case,
   * they will be grouped (to imitate subPdus) by PHY into a PacketBurst that
   * represents a PDU.
   */
  void DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params);

  /**
   * \brief Called by CCM
   * \param params the BSR params
   *
   * The CCM is calling this function for all the MAC of the UE. This method
   * will send SR only for CC ID = 0 (BwpManagerGnb will take care of
   * routing the SR to the appropriate MAC).
   *
   * \see DoSlotIndication
   */
  void DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params);

  /**
   * Execute step 5 of the sensing algorithm in TS 38.214 Section 8.1.4
   * Candidate resources passed in may be excluded (modified) based on
   * transmit history and list of resource reservation periods.
   *
   * \param sfn The current system frame, subframe, and slot number.
   * \param transmitHistory List of transmission history
   * \param candidateList List of candidate resources (modified by this method)
   * \param slResourceReservePeriodList List of resource reservation periods (ms)
   */
  void ExcludeResourcesBasedOnHistory (const SfnSf& sfn, const std::list<SfnSf>& transmitHistory,
    std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>& candidateList,
    const std::list<uint16_t>& slResourceReservePeriodList) const;

  /**
   * Private, internal (const) method invoked by public
   * NrUeMac::GetNrSlCandidateResources()
   *
   * \sa ns3::NrUeMac::GetNrSlCandidateResources
   *
   * \param sfn The current system frame, subframe, and slot number.
   * \param params The input transmission parameters for the algorithm
   * \param txPool the transmit bandwidth pool
   * \param slotPeriod the slot period
   * \param imsi the IMSI
   * \param bwpId the bandwidth part ID 
   * \param poolId the pool ID
   * \param totalSubCh the total subchannels
   * \param sensingData the sensing data
   * \return The list of the transmit opportunities (slots) as per the TDD pattern
   *         and the NR SL bitmap
   */
  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> GetNrSlCandidateResourcesPrivate (const SfnSf& sfn,
    const NrSlTransmissionParams& params, Ptr<const NrSlCommResourcePool> txPool, Time slotPeriod,
    uint64_t imsi, uint8_t bwpId, uint16_t poolId, uint8_t totalSubCh,
    const std::list<SensingData>& sensingData, const std::list<SfnSf>& transmitHistory) const;

  // forwarded from PHY SAP
  void DoReceivePhyPdu (Ptr<Packet> p);
  void DoReceiveControlMessage  (Ptr<NrControlMessage> msg);
  //void DoNotifyHarqDeliveryFailure (uint8_t harqId);

  // forwarded from UE CMAC SAP
  void DoConfigureRach (LteUeCmacSapProvider::RachConfig rc);
  void DoStartContentionBasedRandomAccessProcedure ();
  void DoStartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t rapId, uint8_t prachMask);
  void AddLc (uint8_t lcId, LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu);
  void DoRemoveLc (uint8_t lcId);
  void DoReset ();
  void DoNotifyConnectionSuccessful ();
  void DoSetImsi (uint64_t imsi);

  void RandomlySelectAndSendRaPreamble ();
  void SendRaPreamble (bool contention);

  /**
   * \brief Send a Report Buffer Status
   * \param dataSfn data slot
   * \param symStart symStart
   *
   * Please note that the BSR is not saved in the HARQ buffer, so it will
   * not get retransmitted.
   */
  void SendReportBufferStatus (const SfnSf &dataSfn, uint8_t symStart);
  void RefreshHarqProcessesPacketBuffer (void);

  /**
   * \brief Process the received UL DCI
   * \param dciMsg the UL DCI received
   *
   * The method will call SendNewData() or TransmitRetx() (depending on the UL
   * DCI type), that will take care of sending data out taking into account the
   * header overhead. After sending new data, the method is allowed to enqueue
   * a BSR if there are still bytes in the queue.
   *
   */
  void ProcessUlDci (const Ptr<NrUlDciMessage> &dciMsg);

  /**
   * \brief Transmit a retransmission (good joke, eh?)
   *
   * The method uses the DCI stored in m_ulDci to take the HARQ process id,
   * preparing the subPDUs that are waiting in such HARQ process,
   * and sending them again.
   */
  void TransmitRetx ();

  /**
   * \brief Send data after an UL DCI
   *
   * The method takes care of checking how many subPDUs we have to send,
   * and with a very rough estimation, tries to allocate data to all the active
   * LCID.
   *
   * \see SendNewStatusData()
   * \see SendRetxData()
   * \see SendTxData()
   */
  void SendNewData ();

  /**
   * \brief Send STATUS PDUs
   *
   * This method will try to use the allocated resources by the UL_DCI to send
   * StatusPDU, if they are present, for all the LCID.
   */
  void SendNewStatusData ();

  /**
   * \brief Send RETX data
   *
   * \param usefulTbs TBS that we can use (data only)
   * \param activeRetx number of active LCID with some data in the retxQueue
   *
   * The method will try to use the allocated resources by the UL_DCI to send
   * data in the retxQueue of the various active LCID.
   *
   * \todo the code is similar to SendTxData, maybe they can be unified.
   */
  void SendRetxData (uint32_t usefulTbs, uint32_t activeRetx);

  /**
   * \brief Send TX data
   *
   * \param usefulTbs TBS that we can use (data only)
   * \param activeTx number of active LCID with some data in the txQueue
   *
   * The method will try to use the allocated resources by the UL_DCI to send
   * data in the txQueue of the various active LCID.
   *
   * \todo the code is similar to SendReTxData, maybe they can be unified.
   */
  void SendTxData (uint32_t usefulTbs, uint32_t activeTx);

private:

  LteUeCmacSapUser* m_cmacSapUser {nullptr};
  LteUeCmacSapProvider* m_cmacSapProvider {nullptr};
  NrPhySapProvider* m_phySapProvider {nullptr};
  NrUePhySapUser* m_phySapUser {nullptr};
  LteMacSapProvider* m_macSapProvider {nullptr};

  SfnSf m_currentSlot;  //!< The current slot
  uint8_t m_numHarqProcess {20}; //!< number of HARQ processes

  std::shared_ptr<DciInfoElementTdma> m_ulDci; //!< Received a DCI. While we process it, store it here.
  SfnSf m_ulDciSfnsf;             //!< Received a DCI for transmitting data in this slot.
  uint32_t m_ulDciTotalUsed {0};      //!< Received a DCI, put the total count of bytes we sent.

  std::unordered_map <uint8_t, LteMacSapProvider::ReportBufferStatusParameters> m_ulBsrReceived; //!< BSR received from RLC (the last one)

  /**
   * \brief States for the SR/BSR mechanism.
   *
   * The SR/BSR mechanism is based on a variable in which
   * it is saved the state (INACTIVE/ACTIVE).
   *
   * The machine is starting from the INACTIVE state. When the RLC notifies
   * to MAC that there are new bytes in its queue (DoReportBufferStatus()),
   * if the machine is in INACTIVE state, it enters the ACTIVE state.
   * Entering the ACTIVE state means to send a SR, which is enqueued in the PHY layer.
   * It will suffer slots of CTRL latency. If the state is already ACTIVE, then
   * the BSR can be sent in the same slot as data. It means that the MAC prepares
   * together the data and the BSR.
   *
   * If the BSR is not sent (we don't have any data in the queue) and we don't
   * have any more reserved space to send BSR, then the state goes back to the
   * INACTIVE state.
   */
  enum SrBsrMachine : uint8_t
  {
    INACTIVE,    //!< no SR nor BSR.. initial state
    TO_SEND,     //!< We have to send the BSR when possible
    ACTIVE       //!< SR or BSR sent; now the source of information is the vector m_bsrReservedSpace
  };
  SrBsrMachine m_srState {INACTIVE};       //!< Current state for the SR/BSR machine.

  Ptr<UniformRandomVariable> m_raPreambleUniformVariable;
  uint8_t m_raPreambleId {0}; //!< The RA Preamble ID
  uint8_t m_raRnti {0};       //!< The RA Rnti
  uint64_t m_imsi {0};        ///< IMSI

  // The HARQ part has to be reviewed
  struct UlHarqProcessInfo
  {
    Ptr<PacketBurst> m_pktBurst;
    // maintain list of LCs contained in this TB
    // used to signal HARQ failure to RLC handlers
    std::vector<uint8_t> m_lcidList;
  };

  //uint8_t m_harqProcessId;
  std::vector < UlHarqProcessInfo > m_miUlHarqProcessesPacket; //!< Packets under trasmission of the UL HARQ processes
  std::vector < uint8_t > m_miUlHarqProcessesPacketTimer;      //!< timer for packet life in the buffer

  struct LcInfo
  {
    LteUeCmacSapProvider::LogicalChannelConfig lcConfig;
    LteMacSapUser* macSapUser;
  };

  std::unordered_map <uint8_t, LcInfo> m_lcInfoMap;
  uint16_t m_rnti {0};

  bool m_waitingForRaResponse {true}; //!< Indicates if we are waiting for a RA response
  static uint8_t g_raPreambleId; //!< Preamble ID, fixed, the UEs will not have any collision

  /**
   * Trace information regarding Ue MAC Received Control Messages
   * Frame number, Subframe number, slot, VarTtti, nodeId, rnti, bwpId,
   * pointer to message in order to get the msg type
   */
  TracedCallback<SfnSf, uint16_t, uint16_t, uint8_t, Ptr<const NrControlMessage>> m_macRxedCtrlMsgsTrace;

  /**
   * Trace information regarding Ue MAC Transmitted Control Messages
   * Frame number, Subframe number, slot, VarTtti, nodeId, rnti, bwpId,
   * pointer to message in order to get the msg type
   */
  TracedCallback<SfnSf, uint16_t, uint16_t, uint8_t, Ptr<const NrControlMessage>> m_macTxedCtrlMsgsTrace;

  //NR SL
public:

  /**
   * Structure to pass trace information about the execution of the
   * mode 2 sensing algorithm.
   */
  struct SensingTraceReport
  {
    SfnSf m_sfn; //!< Sfn
    uint16_t m_t0; //!< T0
    uint8_t m_tProc0; //!< T_proc0
    uint8_t m_t1; //!< T1
    uint16_t m_t2; //!< T2
    uint16_t m_subchannels; //!< Subchannels in the pool
    uint16_t m_lSubch; //!< Requested number of subchannels
    uint8_t m_resourcePercentage; //!< Resource percentage value
    uint16_t m_initialCandidateSlotsSize; //!< size of initial candidate slots
    uint16_t m_initialCandidateResourcesSize; //!< S_A at step 4
    uint16_t m_candidateResourcesSizeAfterStep5; //!< S_A after step 5
    int m_initialRsrpThreshold; //!< Initial RSRP threshold
    int m_finalRsrpThreshold; //!< Final RSRP threshold used in algorithm
  };

  /**
   * TracedCallback signature for
   *
   * \param [in] imsi The IMSI.
   * \param [in] rnti C-RNTI scheduled.
   * \param [in] txRnti C-RNTI of the transmitting UE.
   * \param [in] lcid The logical channel id corresponding to
   *             the sending RLC instance.
   * \param [in] bytes The packet size.
   * \param [in] delay Delay since sender timestamp, in sec.
   */
  typedef void (* ReceiveWithTxRntiTracedCallback)
    (uint64_t imsi, uint16_t rnti, uint16_t txRnti, uint8_t lcid, uint32_t bytes, double delay);
  // Comparator function to sort pairs
  // according to second value
  /**
   * \brief Comparator function to sort pairs according to second value
   * \param a The first pair
   * \param b The second pair
   * \return returns true if second value of first pair is less than the second
   *         pair second value, false otherwise.
   */
  static bool CompareSecond (std::pair<uint32_t, uint8_t>& a, std::pair<uint32_t, uint8_t>& b);

  /**
   * \brief Get the NR Sidelik MAC SAP offered by MAC to RLC
   *
   * \return the NR Sidelik MAC SAP provider interface offered by
   *          MAC to RLC
   */
  NrSlMacSapProvider* GetNrSlMacSapProvider ();

  /**
   * \brief Set the NR Sidelik MAC SAP offered by this RLC
   *
   * \param s the NR Sidelik MAC SAP user interface offered to the
   *          MAC by RLC
   */
  void SetNrSlMacSapUser (NrSlMacSapUser* s);

  /**
   * \brief Get the NR Sidelik UE Control MAC SAP offered by MAC to RRC
   *
   * \return the NR Sidelik UE Control MAC SAP provider interface offered by
   *         MAC to RRC
   */
  NrSlUeCmacSapProvider* GetNrSlUeCmacSapProvider ();

  /**
   * \brief Set the NR Sidelik UE Control MAC SAP offered by RRC to MAC
   *
   * \param s the NR Sidelik UE Control MAC SAP user interface offered to the
   *          MAC by RRC
   */
  void SetNrSlUeCmacSapUser (NrSlUeCmacSapUser* s);

  /**
   * \brief Get the NR Sidelik UE PHY SAP offered by UE MAC to UE PHY
   *
   * \return the NR Sidelik UE PHY SAP user interface offered by
   *         UE MAC to UE PHY
   */
  NrSlUePhySapUser* GetNrSlUePhySapUser ();

  /**
   * \brief Set the NR Sidelik UE PHY SAP offered by UE PHY to UE MAC
   *
   * \param s the NR Sidelik UE PHY SAP provider interface offered to the
   *          UE MAC by UE PHY
   */
  void SetNrSlUePhySapProvider (NrSlUePhySapProvider* s);

  /**
   * \brief Set the NR Sidelik SAP for Sched primitives offered by the scheduler
   *        to UE MAC.
   * \param s pointer of type NrSlUeMacSchedSapProvider
   */
  void SetNrSlUeMacSchedSapProvider (NrSlUeMacSchedSapProvider* s);

  /**
   * \brief Set the NR Sidelik SAP for Csched primitives offered by the scheduler
   *        to UE MAC.
   * \param s pointer of type NrSlUeMacCschedSapProvider
   */
  void SetNrSlUeMacCschedSapProvider (NrSlUeMacCschedSapProvider* s);

  /**
   * \brief Get the NR Sidelik SAP for Sched primitives offered by the UE MAC
   *        to the UE NR Sidelink scheduler
   * \return the pointer of type NrSlUeMacSchedSapUser
   */
  NrSlUeMacSchedSapUser* GetNrSlUeMacSchedSapUser ();

  /**
   * \brief Get the NR Sidelik SAP for Csched primitives offered by the UE MAC
   *        to the UE NR Sidelink scheduler
   * \return the pointer of type NrSlUeMacCschedSapUser
   */
  NrSlUeMacCschedSapUser* GetNrSlUeMacCschedSapUser ();

  /**
   * \brief Enable sensing for NR Sidelink resource selection
   * \param enableSensing if True, sensing is used for resource selection. Otherwise, random selection
   */
  void EnableSensing (bool enableSensing);

  /**
   * \brief Enable blind retransmissions for NR Sidelink
   * \param enableBlindReTx if True, blind re-transmissions, i.e.,
   *        retransmissions are done without HARQ feedback. If it is false,
   *        retransmissions are performed based on HARQ feedback from the
   *        receiving UE. The false value of this flag also means that a
   *        transmitting UE must indicate in the SCI message to a receiving
   *        UE to send the feedback.
   */
  void EnableBlindReTx (bool enableBlindReTx);

  /**
   * \brief Set the t_proc0 used for sensing window
   * \param tprocZero t_proc0 in number of slots
   */
  void SetTproc0 (uint8_t tproc0);

  /**
   * \brief Get the t_proc0 used for sensing window
   * \return t_proc0 in number of slots
   */
  uint8_t GetTproc0 () const;

  /**
   * \brief Set T1
   * \param t1 The start of the selection window in physical slots,
   *           accounting for physical layer processing delay.
   */
  void SetT1 (uint8_t t1);

  /**
   * \brief Get T1
   * \return T1 The start of the selection window in physical slots, accounting
   *            for physical layer processing delay.
   */
  uint8_t GetT1 () const;

  /**
   * \brief Set T2
   * \param t2 The end of the selection window in physical slots.
   */
  void SetT2 (uint16_t t2);

  /**
   * \brief Get T2
   * \return T2 The end of the selection window in physical slots.
   */
  uint16_t GetT2 () const;

  /**
   * \brief Set the pool id of the active pool
   * \param poolId The pool id
   */
  void SetSlActivePoolId (uint16_t poolId);

  /**
   * \brief Get the pool id of the active pool
   * \return the pool id
   */
  uint16_t GetSlActivePoolId () const;

  /**
   * \brief Set Reservation Period for NR Sidelink
   *
   * Only the standard compliant values, including their intermediate values
   * could be set. TS38.321 sec 5.22.1.1 instructs to select one of the
   * allowed values configured by RRC in sl-ResourceReservePeriodList and
   * set the resource reservation interval with the selected value. In the
   * simulator we made it an attribute of UE MAC so we can change it on
   * run time. Remember, this attribute value must be in the list of
   * resource reservations configured using pre-configuration. UE MAC calls
   * \link NrSlCommResourcePool::ValidateResvPeriod \endlink to validate its
   * value and also that this value is multiple of the length of the physical
   * sidelink pool (i.e., the resultant bitmap after applying SL bitmap over
   * the TDD pattern).
   *
   * \param rsvpInMs The reservation period in the milliseconds
   *
   * \see LteRrcSap::SlResourceReservePeriod
   */
  void SetReservationPeriod (const Time &rsvpInMs);

  /**
   * \brief Get Reservation Period for NR Sidelink
   *
   * \return The Reservation Period for NR Sidelink
   */
  Time GetReservationPeriod () const;

  /**
   * \brief Sets the number of Sidelink processes of Sidelink HARQ
   * \param numSidelinkProcess the maximum number of Sidelink processes
   */
  void SetNumSidelinkProcess (uint8_t numSidelinkProcess);

  /**
   * \brief Gets the number of Sidelink processes of Sidelink HARQ
   * \return The maximum number of Sidelink processes
   */
  uint8_t GetNumSidelinkProcess () const;

  /**
   * \brief Sets a threshold in dBm used for sensing based UE autonomous resource selection
   * \param thresRsrp The RSRP threshold in dBm
   */
  void SetSlThresPsschRsrp (int thresRsrp);

  /**
   * \brief Gets a threshold in dBm used for sensing based UE autonomous resource selection
   * \return The RSRP threshold in dBm
   */
  int GetSlThresPsschRsrp () const;

  /**
   * \brief Sets the percentage threshold to indicate the minimum number of
   *        candidate single-slot resources to be selected using sensing
   *        procedure.
   * \param percentage The percentage, e.g., 1, 2, 3,.. and so on.
   */
  void SetResourcePercentage (uint8_t percentage);

  /**
   * \brief Gets the percentage threshold to indicate the minimum number of
   *        candidate single-slot resources to be selected using sensing
   *        procedure.
   * \return The percentage, e.g., 1, 2, 3,.. and so on.
   */
  uint8_t GetResourcePercentage () const;

protected:
  // forwarded from NR SL UE MAC SAP Provider
  /**
   * \brief send an NR SL RLC PDU to the MAC for transmission. This method is
   * to be called as a response to NrSlMacSapUser::NotifyNrSlTxOpportunity
   *
   * \param params NrSlRlcPduParameters
   */
  void DoTransmitNrSlRlcPdu (const NrSlMacSapProvider::NrSlRlcPduParameters &params);
  /**
   * \brief Report the RLC buffer status to the MAC
   *
   * \param params NrSlReportBufferStatusParameters
   */
  void DoReportNrSlBufferStatus (const NrSlMacSapProvider::NrSlReportBufferStatusParameters &params);
  /**
   * \brief Fire the trace for SL RLC Reception with Tx Rnti
   *
   * \param p the PDU
   * \param lcid The LCID
   */
  void FireTraceSlRlcRxPduWithTxRnti (const Ptr<Packet> p, uint8_t lcid);

  // forwarded from UE CMAC SAP
  /**
   * \brief Adds a new Logical Channel (LC) used for Sidelink
   *
   * \param slLcInfo The sidelink LC info
   * \param msu The corresponding LteMacSapUser
   */
  void DoAddNrSlLc (const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo &slLcInfo, NrSlMacSapUser* msu);
  /**
   * \brief Remove an existing NR Sidelink Logical Channel for a UE in the LteUeComponentCarrierManager
   *
   * \param slLcId is the Sidelink Logical Channel Id
   * \param srcL2Id is the Source L2 ID
   * \param dstL2Id is the Destination L2 ID
   */
  void DoRemoveNrSlLc (uint8_t slLcId, uint32_t srcL2Id, uint32_t dstL2Id);
  /**
   * \brief Reset Nr Sidelink LC map
   *
   */
  void DoResetNrSlLcMap ();
  /**
   * \brief Add NR Sidelink communication transmission pool
   *
   * Adds transmission pool for NR Sidelink communication
   *
   * \param txPool The pointer to the NrSlCommResourcePool
   */
  void DoAddNrSlCommTxPool (Ptr<const NrSlCommResourcePool> txPool);
  /**
   * \brief Add NR Sidelink communication reception pool
   *
   * Adds reception pool for NR Sidelink communication
   *
   * \param rxPool The pointer to the NrSlCommResourcePool
   */
  void DoAddNrSlCommRxPool (Ptr<const NrSlCommResourcePool> rxPool);
  /**
   * \brief Set Sidelink probability resource keep
   *
   * \param prob Indicates the probability with which the UE keeps the
   *        current resource when the resource reselection counter reaches zero
   *        for sensing based UE autonomous resource selection (see TS 38.321)
   */
  void DoSetSlProbResoKeep (double prob);
  /**
   * \brief Set the maximum transmission number (including new transmission and
   *        retransmission) for PSSCH.
   *
   * \param maxTxPssch The max number of PSSCH transmissions
   */
  void DoSetSlMaxTxTransNumPssch (uint8_t maxTxPssch);
  /**
   * \brief Set Sidelink source layer 2 id
   *
   * \param srcL2Id The Sidelink layer 2 id of the source
   */
  void DoSetSourceL2Id (uint32_t srcL2Id);
  /**
   * \brief Add NR Sidelink destination layer 2 id for reception
   *
   * \param dstL2Id The Sidelink layer 2 id of the destination to listen to.
   */
  void DoAddNrSlRxDstL2Id (uint32_t dstL2Id);

  //Forwarded from NR SL UE PHY SAP User
  /**
   * \brief Gets the active Sidelink pool id used for transmission for a
   *        destination.
   *
   * \return The active TX pool id
   */
  uint8_t DoGetSlActiveTxPoolId ();
  /**
   * \brief Get the list of Sidelink destination for transmission from UE MAC
   * \return A vector holding Sidelink communication destinations and the highest priority value among its LCs
   */
  std::vector <std::pair<uint32_t, uint8_t> > DoGetSlTxDestinations ();
  /**
   * \brief Get the list of Sidelink destination for reception from UE MAC
   * \return A vector holding Sidelink communication destinations for reception
   */
  std::unordered_set <uint32_t> DoGetSlRxDestinations ();
  /**
   * \brief Receive NR SL PSSCH PHY PDU
   * \param pdu The NR SL PSSCH PHY PDU
   */
  void DoReceivePsschPhyPdu (Ptr<PacketBurst> pdu);
  /**
   * \brief Receive the sensing information from PHY to MAC
   * \param sensingData The sensing data
   */
  void DoReceiveSensingData (SensingData sensingData);
  /**
   * \brief Receive the PSFCH from PHY to MAC
   * \param sendingNodeId The sending nodeId
   * \param harqInfo The HARQ info
   */
  void DoReceivePsfch (uint32_t sendingNodeId, SlHarqInfo harqInfo);

  // forwarded from MemberNrSlUeMacSchedSapUser
  /**
   * \brief Method to communicate NR SL grants from NR SL UE scheduler
   * \param dstL2Id destination L2 ID
   * \param grant The sidelink grant
   */
  void DoSchedUeNrSlConfigInd (uint32_t dstL2Id, const NrSlUeMacSchedSapUser::NrSlGrant& grant);

  /**
   * \brief Method through which the NR SL scheduler gets the total number of NR
   * SL sub-channels
   * \return the total number of NR SL sub-channels
   */
  uint8_t DoGetTotalSubCh () const;
  /**
   * \brief Method through which the NR SL scheduler gets the maximum
   *        transmission number (including new transmission and retransmission)
   *        for PSSCH.
   *
   * \return The max number of PSSCH transmissions
   */
  uint8_t DoGetSlMaxTxTransNumPssch () const;
  /**
   * \brief Method through which the NR SL scheduler checks whether the
   *        provided slot has PSFCH
   *
   * \param sfn The slot to check
   * \return True if the slot is a sidelink slot and has PSFCH
   */
  bool DoSlotHasPsfch (const SfnSf& sfn) const;

  // forwarded from MemberNrSlUeMacCschedSapUser
  /**
   * \brief Send the confirmation about the successful configuration of LC
   *        to the UE MAC.
   * \param lcg The logical group
   * \param lcId The Logical Channel id
   */
  void  DoCschedUeNrSlLcConfigCnf (uint8_t lcg, uint8_t lcId);

private:

  ///Sidelink Logical Channel Identifier
  struct SidelinkLcIdentifier
  {
    uint8_t lcId; //!< Sidelink LCID
    uint32_t srcL2Id; //!< Source L2 ID
    uint32_t dstL2Id; //!< Destination L2 ID
  };

  /**
   * \brief Less than operator
   *
   * \param l first SidelinkLcIdentifier
   * \param r second SidelinkLcIdentifier
   * \returns true if first SidelinkLcIdentifier parameter values are less than the second SidelinkLcIdentifier parameters"
   */
  friend bool operator < (const SidelinkLcIdentifier &l, const SidelinkLcIdentifier &r)
  {
   return l.lcId < r.lcId || (l.lcId == r.lcId && l.srcL2Id < r.srcL2Id) || (l.lcId == r.lcId && l.srcL2Id == r.srcL2Id && l.dstL2Id < r.dstL2Id);
  }

  ///Sidelink Logical Channel Information
  struct SlLcInfoUeMac
  {
   NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo lcInfo; //!< LC info
   NrSlMacSapUser* macSapUser; //!< SAP pointer to the RLC instance of the LC
  };

  /**
   * \brief Add NR Sidelink destination layer 2 Id
   *
   * Adds destination layer 2 id to the list of destinations.
   * The destinations in this map are sorted w.r.t their
   * logical channel priority. That is, the destination
   * with a logical channel with a highest priority
   * comes first.
   *
   * \param dstL2Id The destination layer 2 ID
   * \param lcPriority The LC priority
   */
  void AddNrSlDstL2Id (uint32_t dstL2Id, uint8_t lcPriority);

  /**
   * \brief NR sidelink slot indication
   * \param sfn
   */
  void DoNrSlSlotIndication (const SfnSf& sfn);
  /**
   * \brief Get the list of the future transmission slots based on sensed data.
   * \param sensedData The data extracted from the sensed SCI 1-A.
   * \param slotPeriod Slot period
   * \param resvPeriodSlots Reservation period in slots
   * \return The list of the future transmission slots based on sensed data.
   */
  std::list<SlotSensingData> GetFutSlotsBasedOnSens (SensingData sensedData,
Time slotPeriod, uint16_t resvPeriodSlots) const;
#if 0
  /**
   * \brief Method to convert the list of NrSlCommResourcePool::SlotInfo to
   *        NrSlUeMacSchedSapProvider::NrSlSlotInfo
   *
   * NrSlCommResourcePool class exists in the LTE module, therefore, we can not
   * have an object of NR SfnSf class there due to dependency issue. The use of
   * SfnSf class makes our life easier since it already implements the necessary
   * arithmetics of adding slots, constructing new SfnSf given the slot offset,
   * and e.t.c. In this method, we use the slot offset value, which is the
   * offset in number of slots from the current slot to construct the object of
   * SfnSf class.
   *
   * \param sfn The current system frame, subframe, and slot number. This SfnSf
   *        is aligned with the SfnSf of the physical layer.
   * \param slotInfo the list of LTE module compatible slot info
   * \return The list of NR compatible slot info
   */
  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> GetNrSupportedList (const SfnSf& sfn, std::list <NrSlCommResourcePool::SlotInfo> slotInfo);
#endif
  /**
   * \brief Return all of the candidate single-slot resources (step 1 of
   *        TS 38.214 Section 8.1.4)
   *
   * Method to convert the list of NrSlCommResourcePool::SlotInfo (slots)
   * NrSlUeMacSchedSapProvider::NrSlSlotInfo (with widths of subchannels)
   *
   * NrSlCommResourcePool class exists in the LTE module, therefore, we can not
   * have an object of NR SfnSf class there due to dependency issue. The use of
   * SfnSf class makes our life easier since it already implements the necessary
   * arithmetics of adding slots, constructing new SfnSf given the slot offset,
   * and e.t.c. In this method, we use the slot offset value, which is the
   * offset in number of slots from the current slot to construct the object of
   * SfnSf class.
   *
   * \param sfn The current system frame, subframe, and slot number. This SfnSf
   *        is aligned with the SfnSf of the physical layer.
   * \param psfchPeriod The PSFCH period in slots
   * \param minTimeGapPsfch The MinTimeGapPsfch parameter (in slots)
   * \param lSubch Width of candidate resource (number of subchannels)
   * \param numSubch Number of contiguous subchannels
   * \param slotInfo the list of LTE module compatible slot info
   * \return The list of NR compatible slot info
   */
  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> GetNrSlCandidateResourcesFromSlots (const SfnSf& sfn, uint8_t psfchPeriod, uint8_t minTimeGapPsfch, uint16_t lSubch, uint16_t numSubch, std::list <NrSlCommResourcePool::SlotInfo> slotInfo) const;

  /**
   * \brief Removes resources which are already part of an existing published grant.
   *
   * \param txOppr The list of available slots
   * \return The list of resources which are not used by any existing published grant.
   */
  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> FilterNrSlCandidateResources (std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> txOppr);


  /**
   * \brief Get the total number of subchannels based on the system UL bandwidth
   * \param poolId The pool id of the active pool to retrieve the sub-channel size in RBs
   * \return The total number of subchannels
   */
  uint8_t GetTotalSubCh (uint16_t poolId) const;

  /**
   * \brief Remove sensed data older than T0 (sl-SensingWindow)
   * Sensing data outside of the other window edge (Tproc0) is not removed but
   * will be ignored later by the resource selection algorithm.
   * \param sfn The current system frame, subframe, and slot number. This SfnSf
   *        is aligned with the SfnSf of the physical layer.
   * \param sensingWindow The window length in slots (parameter sl-SensingWindow)
   * \param sensingData Reference to the list of SensingData items to be updated
   * \param imsi The IMSI of this instance 
   */
  void RemoveOldSensingData (const SfnSf& sfn, uint16_t sensingWindow, std::list<SensingData>& sensingData, [[maybe_unused]] uint64_t imsi);
  /**
   * \brief Remove transmit history older than T0 (sl-SensingWindow)
   * \param sfn The current system frame, subframe, and slot number. This SfnSf
   *        is aligned with the SfnSf of the physical layer.
   * \param sensingWindow The window length in slots (parameter sl-SensingWindow)
   * \param history Reference to the transmit history to be updated
   * \param imsi The IMSI of this instance 
   */
  void RemoveOldTransmitHistory (const SfnSf& sfn, uint16_t sensingWindow, std::list<SfnSf>& history, [[maybe_unused]] uint64_t imsi);
  /**
   * \brief Compute the gaps in slots for the possible retransmissions
   *        indicated by an SCI 1-A.
   * \param sfn The SfnSf of the current slot which will carry the SCI 1-A.
   * \param it The iterator to a slot allocation list, which is pointing to the
   *        first possible retransmission.   *
   * \param slotNumInd The parameter indicating how many gaps we need to compute.
   * \return A vector containing the value gaps in slots for the possible retransmissions
   *        indicated by an SCI 1-A.
   */
  std::vector<uint8_t> ComputeGaps (const SfnSf& sfn, std::set <NrSlSlotAlloc>::const_iterator it, uint8_t slotNumInd);
  /**
   * \brief Get the start subchannel index of the possible retransmissions
   *        which would be indicated by an SCI 1-A.
   * \param it The iterator to a slot allocation list, which is pointing to the
   *        first possible retransmission.
   * \param slotNumInd The parameter indicating how many
   *        slots (first TX plus one or two ReTx) an SCI 1-A is indicating.
   * \return A vector containing the starting subchannel index of the possible
   *         retransmissions which would be indicated by an SCI 1-A.
   */
  std::vector<uint8_t> GetStartSbChOfReTx (std::set <NrSlSlotAlloc>::const_iterator it, uint8_t slotNumInd);

  /**
   * \brief Check if subchannels ranges in two resources overlap
   * \param firstStart Starting subchannel index of first resource
   * \param firstLength Number of subchannels of first resource (must be greater than 0)
   * \param secondStart Starting subchannel index of candidate resource
   * \param secondLength Number of subchannels of second resource (must be greater than 0)
   * \return True if the two resources overlap, false otherwise
   */
  bool OverlappedResource (uint8_t firstStart, uint8_t firstLength, uint8_t secondStart, uint8_t secondLength) const;

  std::list<SfnSf> m_transmitHistory; //!< History of slots used for transmission

  std::map <SidelinkLcIdentifier, SlLcInfoUeMac> m_nrSlLcInfoMap; //!< Sidelink logical channel info map
  NrSlMacSapProvider* m_nrSlMacSapProvider; //!< SAP interface to receive calls from the UE RLC instance
  NrSlMacSapUser* m_nrSlMacSapUser {nullptr}; //!< SAP interface to call the methods of UE RLC instance
  NrSlUeCmacSapProvider* m_nrSlUeCmacSapProvider; //!< Control SAP interface to receive calls from the UE RRC instance
  NrSlUeCmacSapUser* m_nrSlUeCmacSapUser {nullptr}; //!< Control SAP interface to call the methods of UE RRC instance
  NrSlUePhySapProvider* m_nrSlUePhySapProvider {nullptr}; //!< SAP interface to call the methods of UE PHY instance
  NrSlUePhySapUser* m_nrSlUePhySapUser; //!< SAP interface to receive calls from the UE PHY instance
  Ptr<const NrSlCommResourcePool> m_slTxPool; //!< Sidelink communication transmission pools
  Ptr<const NrSlCommResourcePool> m_slRxPool; //!< Sidelink communication reception pools
  std::vector <std::pair<uint32_t, uint8_t> > m_sidelinkTxDestinations; //!< vector holding Sidelink communication destinations for transmission and the highest priority value among its LCs
  std::unordered_set <uint32_t> m_sidelinkRxDestinations; //!< vector holding Sidelink communication destinations for reception
  bool m_enableSensing {false}; //!< Flag to enable NR Sidelink resource selection based on sensing; otherwise, use random selection
  uint8_t m_minTimeGapProcessing {std::numeric_limits <uint8_t>::max ()}; //!< Minimum time for PSFCH processing
  uint8_t m_tproc0 {0}; //!< t_proc0 in slots
  uint8_t m_t1 {0}; //!< The offset in number of slots between the slot in which the resource selection is triggered and the start of the selection window
  uint16_t m_t2 {0}; //!< The offset in number of slots between T1 and the end of the selection window
  std::map <SidelinkLcIdentifier, NrSlMacSapProvider::NrSlReportBufferStatusParameters> m_nrSlBsrReceived; ///< NR Sidelink BSR received from RLC
  uint16_t m_poolId {std::numeric_limits <uint16_t>::max ()}; //!< Sidelink active pool id
  NrSlUeMacSchedSapUser* m_nrSlUeMacSchedSapUser           {nullptr};  //!< SAP user
  NrSlUeMacCschedSapUser* m_nrSlUeMacCschedSapUser         {nullptr};  //!< SAP User
  NrSlUeMacCschedSapProvider* m_nrSlUeMacCschedSapProvider {nullptr};  //!< SAP Provider
  NrSlUeMacSchedSapProvider* m_nrSlUeMacSchedSapProvider   {nullptr};  //!< SAP Provider
  Ptr<NrSlUeMacScheduler> m_nrSlUeMacScheduler {nullptr}; //!< Pointer to scheduler
  std::map<uint32_t, std::deque<NrSlUeMacSchedSapUser::NrSlGrant> > m_slGrants; //!< Grants provided by the sidelink scheduler; map index is dstL2Id

  double m_slProbResourceKeep {0.0}; //!< Sidelink probability of keeping a resource after resource re-selection counter reaches zero
  uint8_t m_slMaxTxTransNumPssch {0}; /**< Indicates the maximum transmission number
                                     (including new transmission and
                                     retransmission) for PSSCH.
                                     */
  uint8_t m_numSidelinkProcess {0}; //!< Maximum number of Sidelink processes
  Ptr <NrSlUeMacHarq> m_nrSlHarq; //!< Pointer to the NR SL UE MAC HARQ object
  uint32_t m_srcL2Id {std::numeric_limits <uint32_t>::max ()}; //!< The NR Sidelink Source L2 id;
  bool m_nrSlMacPduTxed {false}; //!< Flag to indicate the TX of SL MAC PDU to PHY
  std::list<SensingData> m_sensingData; //!< List to store sensing data
  std::list<SensingData> m_nrSlUnmonSlotsData; //!< List to store info about the slots that were not monitored during sensing

  int m_thresRsrp {-128}; //!< A threshold in dBm used for sensing based UE autonomous resource selection
  uint8_t m_resPercentage {0}; /**< The percentage threshold to indicate the
                                    minimum number of candidate single-slot
                                    resources to be selected using sensing procedure.
                                    */

  /**
   * Trace information regarding NR Sidelink PSCCH UE scheduling.
   * SlPscchUeMacStatParameters (see nr-sl-phy-mac-common.h)
   */
  TracedCallback<SlPscchUeMacStatParameters> m_slPscchScheduling; //!< NR SL PSCCH scheduling trace source
  /**
   * Trace information regarding NR Sidelink PSSCH UE scheduling.
   * SlPsschUeMacStatParameters (see nr-sl-phy-mac-common.h)
   */
  TracedCallback<SlPsschUeMacStatParameters> m_slPsschScheduling; //!< NR SL PSCCH scheduling trace source

  /**
   * Trace information regarding RLC PDU reception from MAC
   */
  TracedCallback<uint64_t, uint16_t, uint16_t, uint8_t, uint32_t, double> m_rxRlcPduWithTxRnti;
  /**
   * Trace information regarding sensing operation
   */
  TracedCallback<const struct SensingTraceReport&, const std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>&,
    const std::list<SensingData>&, const std::list<SfnSf>& > m_tracedSensingAlgorithm;

};

}


#endif /* NR_UE_MAC_H */
