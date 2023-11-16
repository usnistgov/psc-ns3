/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
*   Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
*/
#ifndef NR_SL_UE_MAC_SCHED_SAP_PROVIDER_H
#define NR_SL_UE_MAC_SCHED_SAP_PROVIDER_H

#include "sfnsf.h"
#include "nr-sl-phy-mac-common.h"

#include <iostream>
#include <list>
#include <vector>
#include <memory>
#include <deque>


namespace ns3 {

/**
 * \ingroup scheduler
 *
 * \brief Structure to pass parameters to trigger the selection of candidate
 * resources as per TR 38.214 Section 8.1.4
 */
struct NrSlTransmissionParams
{
  NrSlTransmissionParams (uint8_t prio, Time pdb, uint16_t lSubch, Time pRsvpTx, uint16_t cResel);
  uint8_t m_priority {0};  //!< L1 priority prio_TX
  Time m_packetDelayBudget {Seconds (0)}; //!< remaining packet delay budget
  uint16_t m_lSubch {0}; //!< L_subCH; number of subchannels to be used
  Time m_pRsvpTx {0}; //!< resource reservation interval
  uint16_t m_cResel {0}; //!< C_resel counter
};

/**
 * \ingroup scheduler
 *
 * \brief The SAP interface between NR UE MAC and NR SL UE scheduler
 */
class NrSlUeMacSchedSapProvider
{
public:
  /**
   * ~NrSlUeMacSchedSapProvider
   */
  virtual ~NrSlUeMacSchedSapProvider () = default;

  /**
   * \brief NR Sidelink slot info.
   */
  struct NrSlSlotInfo
  {
    /**
     * \brief NrSlSlotInfo constructor
     *
     * \param numSlPscchRbs Indicates the number of PRBs for PSCCH in a resource pool where it is not greater than the number PRBs of the subchannel.
     * \param slPscchSymStart Indicates the starting symbol used for sidelink PSCCH in a slot
     * \param slPscchSymLength Indicates the total number of symbols available for sidelink PSCCH
     * \param slPsschSymStart Indicates the starting symbol used for sidelink PSSCH in a slot
     * \param slPsschSymLength Indicates the total number of symbols available for sidelink PSSCH
     * \param slHasPsfch Indicates whether PSFCH is present in the slot
     * \param slSubchannelSize Indicates the subchannel size in number of RBs
     // TODO four of the five parameters below should be collected into 
     // some kind of NrSlPoolInfo structure.  cast type
     // should be added to the logical channel configuration.
     * \param slMaxNumPerReserve Indicates the maximum number of reserved PSCCH/PSSCH resources that can be indicated by an SCI.
     * \param slPsfchPeriod Indicates PSFCH period for the pool
     * \param slMinTimeGapPsfch Indicates MinTimeGapPsfch for the pool
     * \param slMinTimeGapProcessing Indicates MinTimeGapProcessing value of NrUeMac
     * \param sfn The SfnSf
     * \param slSubchannelStart The starting subchannel index
     * \param slSubchannelLength The number of subchannels
     */
    NrSlSlotInfo (uint16_t numSlPscchRbs, uint16_t slPscchSymStart, uint16_t slPscchSymLength,
                      uint16_t slPsschSymStart, uint16_t slPsschSymLength, bool slHasPsfch, uint16_t slSubchannelSize,
                      uint16_t slMaxNumPerReserve, uint8_t slPsfchPeriod, uint8_t slMinTimeGapPsfch, uint8_t slMinTimeGapProcessing, SfnSf sfn, 
                      uint8_t slSubchannelStart, uint8_t slSubchannelLength)
    {
      this->numSlPscchRbs = numSlPscchRbs;
      this->slPscchSymStart = slPscchSymStart;
      this->slPscchSymLength = slPscchSymLength;
      this->slPsschSymStart = slPsschSymStart;
      this->slPsschSymLength = slPsschSymLength;
      this->slHasPsfch = slHasPsfch;
      this->slSubchannelSize = slSubchannelSize;
      this->slMaxNumPerReserve = slMaxNumPerReserve;
      this->slPsfchPeriod = slPsfchPeriod;
      this->slMinTimeGapPsfch = slMinTimeGapPsfch;
      this->slMinTimeGapProcessing = slMinTimeGapProcessing;
      this->sfn = sfn;
      this->slSubchannelStart = slSubchannelStart;
      this->slSubchannelLength = slSubchannelLength;
    }
    //PSCCH
    uint16_t numSlPscchRbs {0}; //!< Indicates the number of PRBs for PSCCH in a resource pool where it is not greater than the number PRBs of the subchannel.
    uint16_t slPscchSymStart {std::numeric_limits <uint16_t>::max ()}; //!< Indicates the starting symbol used for sidelink PSCCH in a slot
    uint16_t slPscchSymLength {std::numeric_limits <uint16_t>::max ()}; //!< Indicates the total number of symbols available for sidelink PSCCH
    //PSSCH
    uint16_t slPsschSymStart {std::numeric_limits <uint16_t>::max ()}; //!< Indicates the starting symbol used for sidelink PSSCH in a slot
    uint16_t slPsschSymLength {std::numeric_limits <uint16_t>::max ()}; //!< Indicates the total number of symbols available for sidelink PSSCH
    bool slHasPsfch {false}; //!< Indicates whether PSFCH is present in the slot
    //subchannel size in RBs
    uint16_t slSubchannelSize {std::numeric_limits <uint16_t>::max ()}; //!< Indicates the subchannel size in number of RBs
    uint16_t slMaxNumPerReserve {std::numeric_limits <uint16_t>::max ()}; //!< The maximum number of reserved PSCCH/PSSCH resources that can be indicated by an SCI.
    uint8_t slPsfchPeriod {std::numeric_limits <uint8_t>::max ()}; //!< The PSFCH period configured for this pool
    uint8_t slMinTimeGapPsfch {std::numeric_limits <uint8_t>::max ()}; //!< The MinTimeGapPsfch configured for this pool
    uint8_t slMinTimeGapProcessing {std::numeric_limits <uint8_t>::max ()}; //!< The MinTimeGapProcessing of NrUeMac
    SfnSf sfn {}; //!< The SfnSf
    uint8_t slSubchannelStart {std::numeric_limits <uint8_t>::max ()}; //!< Starting index of subchannel for this resource
    uint8_t slSubchannelLength {std::numeric_limits <uint8_t>::max ()}; //!< Number of continuous subchannels starting from the index

    /**
     * \brief operator < (less than)
     * \param rhs other NrSlSlotInfo to compare
     * \return true if this NrSlSlotInfo is less than rhs
     *
     * The comparison is done on sfnSf
     */
    bool operator < (const NrSlSlotInfo& rhs) const;
  };

  /**
   * Parameters for NrSlMacSapProvider::ReportNrSlBufferStatus
   */
  struct SchedUeNrSlReportBufferStatusParams
  {
    /**
     * \brief NrSlReportBufferStatusParameters constructor
     *
     * \param rnti RNTI
     * \param lcId Logical Channel ID
     * \param txQueueSize The current size of the RLC transmission queue
     * \param txQueueHolDelay The Head Of Line delay of the transmission queue
     * \param retxQueueSize The current size of the RLC retransmission queue
     * \param retxQueueHolDelay The Head Of Line delay of the retransmission queue
     * \param statusPduSize The current size of the pending STATUS RLC  PDU message in bytes
     * \param srcL2Id Sidelink source L2 ID (24 bits)
     * \param dstL2Id Sidelink destination L2 ID (24 bits)
     */
    SchedUeNrSlReportBufferStatusParams (uint16_t rnti, uint8_t lcId,
                                         uint32_t txQueueSize, uint16_t txQueueHolDelay,
                                         uint32_t retxQueueSize, uint16_t retxQueueHolDelay,
                                         uint16_t statusPduSize, uint32_t srcL2Id, uint32_t dstL2Id)
    {
      this->rnti = rnti;
      this->lcid = lcId;
      this->txQueueSize = txQueueSize;
      this->txQueueHolDelay = txQueueHolDelay;
      this->retxQueueSize = retxQueueSize;
      this->retxQueueHolDelay = retxQueueHolDelay;
      this->statusPduSize = statusPduSize;
      this->srcL2Id = srcL2Id;
      this->dstL2Id = dstL2Id;
    }

    uint16_t rnti {std::numeric_limits <uint16_t>::max ()};  //!< the C-RNTI identifying the UE
    uint8_t lcid {std::numeric_limits <uint8_t>::max ()};  //!< the logical channel id corresponding to the sending RLC instance
    uint32_t txQueueSize {0};  //!< the current size of the RLC transmission queue
    uint16_t txQueueHolDelay {std::numeric_limits <uint16_t>::max ()};  //!< the Head Of Line delay of the transmission queue
    uint32_t retxQueueSize {0};  //!< the current size of the RLC retransmission queue in bytes
    uint16_t retxQueueHolDelay {std::numeric_limits <uint16_t>::max ()};  //!< the Head Of Line delay of the retransmission queue
    uint16_t statusPduSize {0};  //!< the current size of the pending STATUS RLC  PDU message in bytes
    uint32_t srcL2Id {0};  //!< Source L2 ID (24 bits)
    uint32_t dstL2Id {0};  //!< Destination L2 ID (24 bits)
  };

  /**
   * \brief Send NR Sidelink RLC buffer status report from UE MAC to the UE scheduler
   *
   * \param params NrSlUeMacSchedSapProvider::SchedUeNrSlReportBufferStatusParams
   */
  virtual void SchedUeNrSlRlcBufferReq (const struct SchedUeNrSlReportBufferStatusParams& params) = 0;
  /**
   * \brief Send NR Sidleink trigger request from UE MAC to the UE scheduler
   *
   * \param sfn The SfnSf
   * \param ids available HARQ process IDs
   */
  virtual void SchedUeNrSlTriggerReq (const SfnSf& sfn, const std::deque<uint8_t>& ids) = 0;
  /**
   * \brief Tell the scheduler that a new slot has started
   * \param sfn Ths current SfnSf
   * \param isSidelinkSlot Whether the slot is a sidelink slot
   */
  virtual void SlotIndication (SfnSf sfn, bool isSidelinkSlot) = 0;
  /**
   * \brief Tell the scheduler that an RLC PDU packet has been dequeue and is now on the HARQ buffer
   *
   * \param dstL2Id The destination layer 2 ID
   * \param lcId The logical channel ID
   * \param size The size of the RLC PDU
   */
  virtual void NotifyNrSlRlcPduDequeue (uint32_t dstL2Id, uint8_t lcId, uint32_t size) = 0;
};

/**
 * \ingroup scheduler
 *
 * \brief The Interface between NR SL UE Scheduler and NR UE MAC
 */
class NrSlUeMacSchedSapUser
{
public:
  /**
   * \brief ~NrSlUeMacSchedSapUser
   */
  virtual ~NrSlUeMacSchedSapUser () = default;

  ///NR Sidelink grant Information
  struct NrSlGrantInfo
  {
    uint16_t cReselCounter {std::numeric_limits <uint8_t>::max ()}; //!< The Cresel counter for the semi-persistently scheduled resources as per TS 38.214
    uint8_t slResoReselCounter {std::numeric_limits <uint8_t>::max ()}; //!< The Sidelink resource re-selection counter for the semi-persistently scheduled resources as per TS 38.214
    std::set <NrSlSlotAlloc> slotAllocations; //!< List of all the slots available for transmission with the pool
    uint8_t prevSlResoReselCounter {std::numeric_limits <uint8_t>::max ()}; //!< Previously drawn Sidelink resource re-selection counter
    uint8_t nrSlHarqId {std::numeric_limits <uint8_t>::max ()}; //!< The NR SL HARQ process id assigned at the time of transmitting new data
    uint8_t nSelected {0}; //!< The number of slots selected by the scheduler for first reservation period
    uint8_t tbTxCounter {0}; //!< The counter to count the number of time a TB is tx/reTx in a reservation period
    bool isDynamic {false}; //!< true if the grant is for dynamic scheduling (single-PDU), false if it is for semi-persistent scheduling
    bool harqEnabled {false}; //!< true if the grant should use HARQ
    Time rri {0}; //!< The resource reservation interval for the semi-persistent scheduled grant
  };

  struct NrSlGrant
  {
    std::set <NrSlSlotAlloc> slotAllocations; //!< List of all the slots available for transmission with the pool
    bool harqEnabled {false}; //!< Whether HARQ is enabled for the grant
    uint8_t nrSlHarqId {std::numeric_limits <uint8_t>::max ()}; //!< The NR SL HARQ process id assigned at the time of transmitting new data
    uint8_t nSelected {0}; //!< The number of slots selected by the scheduler for first reservation period
    uint8_t tbTxCounter {0}; //!< The counter to count the number of time a TB is tx/reTx in a reservation period
    uint32_t tbSize {0}; //!< Size of Transport Block in bytes
    Time rri {0}; //!< The resource reservation interval for the semi-persistent scheduled grant
  };

  /**
   * \brief Method to communicate NR SL grants from NR SL UE scheduler
   * \param dstL2Id destination L2 ID
   * \param grant The sidelink grant
   */
  virtual void SchedUeNrSlConfigInd (uint32_t dstL2Id, const NrSlGrant& grant) = 0;

  /**
   * \brief Method to get total number of sub-channels.
   *
   * \return the total number of sub-channels.
   */
  virtual uint8_t GetTotalSubCh () const = 0;

  /**
   * \brief Method to get the maximum transmission number
   *        (including new transmission and retransmission) for PSSCH.
   *
   * \return The max number of PSSCH transmissions
   */
  virtual uint8_t GetSlMaxTxTransNumPssch () const = 0;

  /**
   * \brief Method to check whether a given slot has PSFCH
   * \param sfn The slot to check
   * \return true if that slot has PSFCH
   */
  virtual bool SlotHasPsfch (const SfnSf& sfn) const = 0;
};

/**
 * \brief Stream output operator
 * \param os output stream
 * \param p struct whose parameter to output
 * \return updated stream
 */
std::ostream & operator<< (std::ostream & os, NrSlTransmissionParams const & p);

/**
 * \brief Stream output operator
 * \param os output stream
 * \param p struct whose parameter to output
 * \return updated stream
 */
std::ostream & operator<< (std::ostream & os, NrSlUeMacSchedSapProvider::SchedUeNrSlReportBufferStatusParams const & p);

/**
 * \brief Stream output operator
 * \param os output stream
 * \param p struct whose parameter to output
 * \return updated stream
 */
std::ostream & operator<< (std::ostream & os, NrSlUeMacSchedSapProvider::NrSlSlotInfo const & p);

/**
 * \brief Stream output operator
 * \param os output stream
 * \param p struct whose parameter to output
 * \return updated stream
 */
std::ostream & operator<< (std::ostream & os, NrSlUeMacSchedSapUser::NrSlGrantInfo const & p);

} // namespace ns3

#endif /* NR_SL_UE_MAC_SCHED_SAP_PROVIDER_H */
