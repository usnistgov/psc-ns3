// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only AND NIST-Software

#ifndef NR_SL_UE_MAC_H
#define NR_SL_UE_MAC_H

#include "nr-sl-comm-resource-pool.h"
#include "nr-sl-mac-sap.h"
#include "nr-sl-phy-mac-common.h"
#include "nr-sl-ue-cmac-sap.h"
#include "nr-sl-ue-phy-sap.h"

#include "ns3/nr-ue-mac.h"

#include <map>
#include <unordered_map>
#include <unordered_set>

// test classes outside of namespace ns3
class NrSlSensingTestCase;
class NrSlRemoveOldSensingDataTest;
class NrSlSensingTransmitHistoryTest;

namespace ns3
{

class NrSlUeMacHarq;
class NrSlUeMacScheduler;

/**
 * \ingroup ue-mac
 * \brief Sidelink specialization of class `NrUeMac`
 *
 * This class implements sensing and HARQ procedures for sidelink as
 * standardized in TS 38.321.  One instance of this class is paired with
 * each instance of a `NrSlBwpManagerUe`.  Similar to the parent `NrUeMac`,
 * this class has interfaces defined with the RLC and RRC layers above and
 * the PHY below.  Scheduling operations are handled in a separate class.
 * A number of attributes are defined to parameterize the sensing operation,
 * and trace sources exporting scheduling and PDU receptions are available.
 */
class NrSlUeMac : public NrUeMac
{
    /// allow MemberNrSlMacSapProvider<NrSlUeMac> class friend access
    friend class MemberNrSlMacSapProvider<NrSlUeMac>;
    /// allow MemberNrSlUeCmacSapProvider<NrSlUeMac> class friend access
    friend class MemberNrSlUeCmacSapProvider<NrSlUeMac>;
    /// allow MemberNrSlUePhySapUser<NrSlUeMac> class friend access
    friend class MemberNrSlUePhySapUser<NrSlUeMac>;
    // Unit-test access to protected/private members
    friend class ::NrSlSensingTestCase;
    friend class ::NrSlRemoveOldSensingDataTest;
    friend class ::NrSlSensingTransmitHistoryTest;

  public:
    /**
     * \brief Get the Type id
     * \return the type id
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSlUeMac constructor
     */
    NrSlUeMac();

    // Structures/classes used in API

    /**
     * \brief Structure to pass grant from NrSlUeMacScheduler to this object
     */
    struct NrSlGrant
    {
        std::set<SlGrantResource>
            slotAllocations;     //!< List of all the slots available for transmission with the pool
        bool harqEnabled{false}; //!< Whether HARQ is enabled for the grant
        uint8_t harqId{std::numeric_limits<uint8_t>::max()}; //!< The HARQ process id assigned at
                                                             //!< the time of transmitting new data
        uint8_t nSelected{
            0}; //!< The number of slots selected by the scheduler for first reservation period
        uint8_t tbTxCounter{
            0}; //!< The counter to count the number of time a TB is tx/reTx in a reservation period
        uint32_t tbSize{0}; //!< Size of Transport Block in bytes
        Time rri{0}; //!< The resource reservation interval for the semi-persistent scheduled grant
        SidelinkInfo::CastType castType{SidelinkInfo::CastType::Invalid}; //!< Cast type
        uint8_t reselCounter{0}; //!< The grant reselection counter
    };

    /**
     * \brief Structure to pass parameters to trigger the selection of candidate
     * resources as per TR 38.214 Section 8.1.4
     */
    struct NrSlTransmissionParams
    {
        NrSlTransmissionParams() = default;
        NrSlTransmissionParams(uint8_t prio,
                               Time pdb,
                               uint16_t lSubch,
                               Time pRsvpTx,
                               uint16_t cResel);
        uint8_t m_priority{0};                //!< L1 priority prio_TX
        Time m_packetDelayBudget{Seconds(0)}; //!< remaining packet delay budget
        uint16_t m_lSubch{0};                 //!< L_subCH; number of subchannels to be used
        Time m_pRsvpTx{0};                    //!< resource reservation interval
        uint16_t m_cResel{0};                 //!< C_resel counter
    };

    /**
     * \brief Structure to pass parameters determined as part of the selection
     * of candidate resources as per TR 38.214 Section 8.1.4
     */
    struct NrSlSelectionParams
    {
        uint8_t m_t1;  //!< T1
        uint16_t m_t2; //!< T2
    };

    /**
     * Structure to pass trace information about the execution of the
     * mode 2 sensing algorithm.
     */
    struct SensingTraceReport
    {
        SfnSf m_sfn;                                 //!< Sfn
        uint16_t m_t0;                               //!< T0
        uint8_t m_tProc0;                            //!< T_proc0
        uint8_t m_t1;                                //!< T1
        uint16_t m_t2;                               //!< T2
        uint16_t m_subchannels;                      //!< Subchannels in the pool
        uint16_t m_lSubch;                           //!< Requested number of subchannels
        uint8_t m_resourcePercentage;                //!< Resource percentage value
        uint16_t m_initialCandidateSlotsSize;        //!< size of initial candidate slots
        uint16_t m_initialCandidateResourcesSize;    //!< S_A at step 4
        uint16_t m_candidateResourcesSizeAfterStep5; //!< S_A after step 5
        int m_initialRsrpThreshold;                  //!< Initial RSRP threshold
        int m_finalRsrpThreshold;                    //!< Final RSRP threshold used in algorithm
    };

    /**
     * Structure to maintain numConsecutiveDTX information per destination L2
     * ID
     */
    struct NumConsecutiveDtx
    {
        uint32_t m_count; //!< The numConsectutiveDTX count associated with the logical channels
        std::set<uint8_t> m_lcIds; //!< The IDs of the logical channels that the count pertains to
    };

    // SCHED API primitive for NR Sidelink
    // From FAPI 2.0.0 Small Cell Forum originated LTE MAC scheduler API

    /**
     * \brief Method to communicate NR SL grants from NR SL UE scheduler
     * \param dstL2Id destination L2 ID
     * \param grant The sidelink grant
     */
    void SchedNrSlConfigInd(uint32_t dstL2Id, const NrSlGrant& grant);

    // CSCHED API primitive for NR Sidelink
    /**
     * \brief Send the confirmation about the successful configuration of LC
     *        to the UE MAC.
     * \param lcg The logical group
     * \param lcId The Logical Channel id
     */
    void CschedNrSlLcConfigCnf(uint8_t lcg, uint8_t lcId);

    // Related primitives for LC removal

    /**
     * \brief Send the confirmation about the successful removal of LC ID
     *
     * \param lcId The Logical Channel id
     */
    void RemoveNrSlLcConfigCnf(uint8_t lcId);

    /**
     * TracedCallback signature for Receive with Tx RNTI
     *
     * \param [in] imsi The IMSI.
     * \param [in] rnti C-RNTI scheduled.
     * \param [in] txRnti C-RNTI of the transmitting UE.
     * \param [in] lcid The logical channel id corresponding to
     *             the sending RLC instance.
     * \param [in] bytes The packet size.
     * \param [in] delay Delay since sender timestamp, in sec.
     */
    typedef void (*ReceiveWithTxRntiTracedCallback)(uint64_t imsi,
                                                    uint16_t rnti,
                                                    uint16_t txRnti,
                                                    uint8_t lcid,
                                                    uint32_t bytes,
                                                    double delay);
    /**
     * TracedCallback signature for SensingAlgorithm
     * \param [in] report sensing report.
     * \param [in] candidateResources candidates found by the algorithm.
     * \param [in] sensingData sensing input data.
     * \param [in] transmitHistory transmit history.
     */
    typedef void (*SensingAlgorithmTracedCallback)(
        const struct SensingTraceReport& report,
        const std::list<SlResourceInfo>& candidateResources,
        const std::list<SensingData>& sensingData,
        const std::list<SfnSf>& transmitHistory);

    /**
     * \brief Get the NR Sidelink MAC SAP offered by MAC to RLC
     *
     * \return the NR Sidelink MAC SAP provider interface offered by
     *          MAC to RLC
     */
    NrSlMacSapProvider* GetNrSlMacSapProvider();

    /**
     * \brief Set the NR Sidelink MAC SAP offered by this RLC
     *
     * \param s the NR Sidelink MAC SAP user interface offered to the
     *          MAC by RLC
     */
    void SetNrSlMacSapUser(NrSlMacSapUser* s);

    /**
     * \brief Get the NR Sidelink UE Control MAC SAP offered by MAC to RRC
     *
     * \return the NR Sidelink UE Control MAC SAP provider interface offered by
     *         MAC to RRC
     */
    NrSlUeCmacSapProvider* GetNrSlUeCmacSapProvider();

    /**
     * \brief Set the NR Sidelink UE Control MAC SAP offered by RRC to MAC
     *
     * \param s the NR Sidelink UE Control MAC SAP user interface offered to the
     *          MAC by RRC
     */
    void SetNrSlUeCmacSapUser(NrSlUeCmacSapUser* s);

    /**
     * \brief Get the NR Sidelink UE Control MAC SAP offered by RRC to MAC
     *
     * \return the NR Sidelink UE Control MAC SAP user interface offered to the
     *          MAC by RRC
     */
    NrSlUeCmacSapUser* GetNrSlUeCmacSapUser() const;

    /**
     * \brief Get the NR Sidelink UE PHY SAP offered by UE MAC to UE PHY
     *
     * \return the NR Sidelink UE PHY SAP user interface offered by
     *         UE MAC to UE PHY
     */
    NrSlUePhySapUser* GetNrSlUePhySapUser();

    /**
     * \brief Set the NR Sidelink UE PHY SAP offered by UE PHY to UE MAC
     *
     * \param s the NR Sidelink UE PHY SAP provider interface offered to the
     *          UE MAC by UE PHY
     */
    void SetNrSlUePhySapProvider(NrSlUePhySapProvider* s);

    /**
     * \brief Enable sensing for NR Sidelink resource selection
     * \param enableSensing if True, sensing is used for resource selection. Otherwise, random
     * selection
     */
    void EnableSensing(bool enableSensing);

    /**
     * \brief Set the t_proc0 used for sensing window
     * \param tprocZero t_proc0 in number of slots
     */
    void SetTproc0(uint8_t tproc0);

    /**
     * \brief Get the t_proc0 used for sensing window
     * \return t_proc0 in number of slots
     */
    uint8_t GetTproc0() const;

    /**
     * \brief Set T1
     * \param t1 The start of the selection window in physical slots,
     *           accounting for physical layer processing delay.
     */
    void SetT1(uint8_t t1);

    /**
     * \brief Get T1
     * \return T1 The start of the selection window in physical slots, accounting
     *            for physical layer processing delay.
     */
    uint8_t GetT1() const;

    /**
     * \brief Set the pool id of the active pool
     * \param poolId The pool id
     */
    void SetSlActivePoolId(uint16_t poolId);

    /**
     * \brief Get the pool id of the active pool
     * \return the pool id
     */
    uint16_t GetSlActivePoolId() const;

    /**
     * \brief Gets the number of Sidelink processes of Sidelink HARQ
     *
     * The first item of the pair is the maximum number of processes
     * corresponding to multiple PDU transmissions.  The second item of
     * the pair is the maximum number of processes.
     *
     * \return The maximum number of Sidelink processes
     */
    std::pair<uint8_t, uint8_t> GetNumSidelinkProcess() const;

    /**
     * \brief Sets a threshold in dBm used for sensing based UE autonomous resource selection
     * \param thresRsrp The RSRP threshold in dBm
     */
    void SetSlThresPsschRsrp(int thresRsrp);

    /**
     * \brief Gets a threshold in dBm used for sensing based UE autonomous resource selection
     * \return The RSRP threshold in dBm
     */
    int GetSlThresPsschRsrp() const;

    /**
     * \brief Sets the percentage threshold to indicate the minimum number of
     *        candidate single-slot resources to be selected using sensing
     *        procedure.
     * \param percentage The percentage, e.g., 1, 2, 3,.. and so on.
     */
    void SetResourcePercentage(uint8_t percentage);

    /**
     * \brief Gets the percentage threshold to indicate the minimum number of
     *        candidate single-slot resources to be selected using sensing
     *        procedure.
     * \return The percentage, e.g., 1, 2, 3,.. and so on.
     */
    uint8_t GetResourcePercentage() const;

    /**
     * \brief Sets the maximum number of times a TB can be transmitted with
     *        no PSFCH feedback before RLF is declared
     * \parm max The maximum number { 1, 2, 3, 4, 6, 8, 16, 32 } or 0 to disable
     */
    void SetSlMaxNumConsecutiveDtx(uint32_t max);

    /**
     * \brief Gets the maximum number of times a TB can be transmitted with
     *        no PSFCH feedback before RLF is declared
     * \return The maximum number { 1, 2, 3, 4, 6, 8, 16, 32 } or 0 to disable
     */
    uint32_t GetSlMaxNumConsecutiveDtx() const;

    /**
     * \brief Return the number of NR SL sub-channels for the active BW pool
     * \return the total number of NR SL sub-channels
     */
    uint8_t GetTotalSubCh() const;

    /**
     * \brief Method through which the NR SL scheduler gets the maximum
     *        transmission number (including new transmission and retransmission)
     *        for PSSCH.
     *
     * \return The max number of PSSCH transmissions
     */
    uint8_t GetSlMaxTxTransNumPssch() const;

    /**
     * \brief Get the value of SlProbResourceKeep
     *
     * This value is set through RRC preconfiguration.
     *
     * \return The probability of keeping resources upon reselection
     */
    double GetSlProbResourceKeep() const;

    /**
     * \brief Method to check whether the provided slot has PSFCH
     *
     * \param sfn The slot to check
     * \return True if the slot is a sidelink slot and has PSFCH
     */
    bool SlotHasPsfch(const SfnSf& sfn) const;

    /**
     * \brief Get the reservation period in slots
     *
     * Wrapper method that calls NrSlCommResourcePool::GetResvPeriodInSlots
     *
     * \param resvPeriod The reservation period
     * \return The reservation period in slots
     */
    uint16_t GetResvPeriodInSlots(Time resvPeriod) const;

    /**
     * \brief Get NR Sidelink subchannel size
     *
     * Wrapper method that calls NrSlCommResourcePool::GetNrSlSubChSize
     *
     * \return The subchannel size in RBs
     */
    uint16_t GetNrSlSubChSize() const;

    /**
     * \brief Get the PSFCH period for the resource pool
     *
     * Wrapper method that calls NrSlCommResourcePool::GetPsfchPeriod
     *
     * \return value of the PsfchPeriod, in slots
     */
    uint8_t GetPsfchPeriod() const;

    /**
     * \brief Get the minimum time gap between PSSCH and PSFCH for the resource pool
     *
     * Wrapper method that calls NrSlCommResourcePool::GetMinTimeGapPsfch
     *
     * \return value of MinTimeGapPsfch, in slots
     */
    uint8_t GetMinTimeGapPsfch() const;

    /**
     * \brief Get NR sidelink available resources
     *
     * In resource allocation mode 2, as specified in TS 38.214 section 8,
     * the higher layer (MAC) can request the UE (PHY) to
     * determine a subset of resources from which the higher layer will select
     * resources for PSSCH/PSCCH transmission.  In ns-3, we implement
     * this as part of this UE MAC implementation, based on sensing data
     * provided by the PHY.
     *
     * This function first calls the GetAllCandidateResources function to
     * determine the set of candidate resources according to 3GPP TR 38.214
     * Section 8.1.4 step 1).  The size of the returned list of candidates
     * is defined as M_total in the standard.
     *
     * This function next performs steps 5-7 to possibly reduce the
     * candidate set to the list defined as S_A in the standard.
     *
     * The values of T1 and T2 that were used in the algorithm are exported
     * as an output NrSlSelectionParams parameter.
     *
     * \param sfn The current system frame, subframe, and slot number.
     * \param params The input transmission parameters for the algorithm
     * \param selectionParams The output selection parameters from the algorithm
     * \return The list of the transmit opportunities (slots) as per the TDD pattern
     *         and the NR SL bitmap
     */
    std::list<SlResourceInfo> GetCandidateResources(const SfnSf& sfn,
                                                    const NrSlTransmissionParams& params,
                                                    NrSlSelectionParams& selectionParams);

    /**
     * \brief Get the slot duration
     *
     * Wrapper method that calls NrSlUePhySapProvider::GetSlotPeriod
     *
     * \return value of the slot duration
     */
    Time GetSlotPeriod() const;

    /**
     * \brief Get the numerology
     *
     * Wrapper method that calls NrSlUePhySapProvider::GetNumerology
     *
     * \return the numerology
     */
    uint16_t GetNumerology() const;

    /**
     * \brief Get the TX resource pool
     *
     * \return pointer to the TX resource pool
     */
    Ptr<const NrSlCommResourcePool> GetTxPool() const;

  protected:
    /**
     * \brief Creates an LC ID entry in the data structure that tracks active
     *        destination L2 ID's with HARQ processes that are pending
     *        feedback.
     * \param dstL2Id The L2 destination ID to track numConsectutiveDTX
     *                for.
     * \param lcId The ID of the logical channel with outstanding HARQ FB.
     */
    void AddNumConsecutiveDtx(uint32_t dstL2Id, uint8_t lcId);

    /**
     * \brief Removes the LC ID entry in the data structure that tracks active
     *        destination L2 ID's with HARQ processes that are pending
     *        feedback.
     * \param dstL2Id The L2 destination ID to remove numConsectutiveDTX
     *                for.
     * \param lcId The ID of the logical channel that should be disregarded.
     */
    void RemoveNumConsecutiveDtx(uint32_t dstL2Id, uint8_t lcId);

    /**
     * \brief Increments numConsecutiveDTX for all destination L2 IDs which
     *        should be incremented each time PSFCH (HARQ feedback) can be
     *        received but was not.
     */
    void IncrementNumConsecutiveDtx();

    /**
     * \brief Clears numConsecutiveDTX entry from structure that tracks active
     *        destination L2 ID's with HARQ processes that are pending
     *        feedback.
     * \param dstL2Id The L2 destination ID to clear numConsectutiveDTX for.
     */
    void ClearNumConsecutiveDtx(uint32_t dstL2Id);

    // Inherited
    void DoDispose() override;
    int64_t DoAssignStreams(int64_t stream) override;

    // forwarded from NR SL UE MAC SAP Provider
    /**
     * \brief send an NR SL RLC PDU to the MAC for transmission. This method is
     * to be called as a response to NrSlMacSapUser::NotifyNrSlTxOpportunity
     *
     * \param params NrSlRlcPduParameters
     */
    void DoTransmitNrSlRlcPdu(const NrSlMacSapProvider::NrSlRlcPduParameters& params);
    /**
     * \brief Report the RLC buffer status to the MAC
     *
     * \param params NrSlReportBufferStatusParameters
     */
    void DoReportNrSlBufferStatus(
        const NrSlMacSapProvider::NrSlReportBufferStatusParameters& params);
    /**
     * \brief Fire the trace for SL RLC Reception with Tx Rnti
     *
     * \param p the PDU
     * \param lcid The LCID
     */
    void FireTraceSlRlcRxPduWithTxRnti(const Ptr<Packet> p, uint8_t lcid);

    // forwarded from UE CMAC SAP
    /**
     * \brief Adds a new Logical Channel (LC) used for Sidelink
     *
     * \param slLcInfo The sidelink LC info
     * \param msu The corresponding NrSlMacSapUser
     */
    void DoAddNrSlLc(const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& slLcInfo,
                     NrSlMacSapUser* msu);
    /**
     * \brief Remove an existing NR Sidelink Logical Channel for a UE in the
     * NrUeComponentCarrierManager
     *
     * \param slLcId is the Sidelink Logical Channel Id
     * \param srcL2Id is the Source L2 ID
     * \param dstL2Id is the Destination L2 ID
     */
    void DoRemoveNrSlLc(uint8_t slLcId, uint32_t srcL2Id, uint32_t dstL2Id);
    /**
     * \brief Reset Nr Sidelink LC map
     *
     */
    void DoResetNrSlLcMap();
    /**
     * \brief Add NR Sidelink communication transmission pool
     *
     * Adds transmission pool for NR Sidelink communication
     *
     * \param txPool The pointer to the NrSlCommResourcePool
     */
    void DoAddNrSlCommTxPool(Ptr<const NrSlCommResourcePool> txPool);
    /**
     * \brief Add NR Sidelink communication reception pool
     *
     * Adds reception pool for NR Sidelink communication
     *
     * \param rxPool The pointer to the NrSlCommResourcePool
     */
    void DoAddNrSlCommRxPool(Ptr<const NrSlCommResourcePool> rxPool);
    /**
     * \brief Set Sidelink probability resource keep
     *
     * \param probability Indicates the probability with which the UE keeps the
     *        current resource when the resource reselection counter reaches zero
     *        for semi-persistent scheduling resource selection (see TS 38.321)
     */
    void DoSetSlProbResourceKeep(double probability);
    /**
     * \brief Set the maximum transmission number (including new transmission and
     *        retransmission) for PSSCH.
     *
     * \param maxTxPssch The max number of PSSCH transmissions
     */
    void DoSetSlMaxTxTransNumPssch(uint8_t maxTxPssch);
    /**
     * \brief Set Sidelink source layer 2 id
     *
     * \param srcL2Id The Sidelink layer 2 id of the source
     */
    void DoSetSourceL2Id(uint32_t srcL2Id);
    /**
     * \brief Add NR Sidelink destination layer 2 id for reception
     *
     * \param dstL2Id The Sidelink layer 2 id of the destination to listen to.
     */
    void DoAddNrSlRxDstL2Id(uint32_t dstL2Id);
    /**
     * \brief Remove NR Sidelink destination layer 2 id for reception
     *
     * \param dstL2Id The Sidelink layer 2 id of the destination to be removed.
     */
    void DoRemoveNrSlRxDstL2Id(uint32_t dstL2Id);

    // Forwarded from NR SL UE PHY SAP User
    /**
     * \brief Gets the active Sidelink pool id used for transmission for a
     *        destination.
     *
     * \return The active TX pool id
     */
    uint8_t DoGetSlActiveTxPoolId() const;
    /**
     * \brief Get the list of Sidelink destination for transmission from UE MAC
     * \return A vector holding Sidelink communication destinations and the highest priority value
     * among its LCs
     */
    std::vector<std::pair<uint32_t, uint8_t>> DoGetSlTxDestinations();
    /**
     * \brief Get the list of Sidelink destination for reception from UE MAC
     * \return A vector holding Sidelink communication destinations for reception
     */
    std::unordered_set<uint32_t> DoGetSlRxDestinations();
    /**
     * \brief Receive NR SL PSSCH PHY PDU
     * \param pdu The NR SL PSSCH PHY PDU
     */
    void DoReceivePsschPhyPdu(Ptr<PacketBurst> pdu);
    /**
     * \brief Receive the sensing information from PHY to MAC
     * \param sensingData The sensing data
     */
    void DoReceiveSensingData(SensingData sensingData);
    /**
     * \brief Receive the PSFCH from PHY to MAC
     * \param sendingNodeId The sending nodeId
     * \param harqInfo The HARQ info
     */
    void DoReceivePsfch(uint32_t sendingNodeId, SlHarqInfo harqInfo);

  private:
    void DoSlotIndication(const SfnSf& sfn) override;

    /**
     * \brief Selection window exclusion based on half-duplex considerations.
     *
     * Execute step 5 of the sensing algorithm in TS 38.214 Section 8.1.4
     * Candidate resources passed in may be excluded (removed based on
     * transmit history and list of possible resource reservation periods.
     *
     * \param sfn The current system frame, subframe, and slot number.
     * \param transmitHistory List of transmission history
     * \param candidateList List of candidate resources (modified by this method)
     * \param slResourceReservePeriodList List of resource reservation periods (ms)
     */
    void ExcludeResourcesBasedOnHistory(
        const SfnSf& sfn,
        const std::list<SfnSf>& transmitHistory,
        std::list<SlResourceInfo>& candidateList,
        const std::list<uint16_t>& slResourceReservePeriodList) const;

    /**
     * \brief Return all of the candidate single-slot resources (step 1 of
     *        TS 38.214 Section 8.1.4)
     *
     * Method to convert the list of NrSlCommResourcePool::SlotInfo (slots)
     * SlResourceInfo (with widths of subchannels)
     *
     * NrSlCommResourcePool class exists in the LTE module, therefore, we can not
     * have an object of NR SfnSf class there due to dependency issue. The use of
     * SfnSf class makes our life easier since it already implements the necessary
     * arithmetic of adding slots, constructing new SfnSf given the slot offset,
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
    std::list<SlResourceInfo> GetCandidateResourcesFromSlots(
        const SfnSf& sfn,
        uint8_t psfchPeriod,
        uint8_t minTimeGapPsfch,
        uint16_t lSubch,
        uint16_t numSubch,
        std::list<NrSlCommResourcePool::SlotInfo> slotInfo) const;

    /// Sidelink Logical Channel Identifier
    struct SidelinkLcIdentifier
    {
        uint8_t lcId;     //!< Sidelink LCID
        uint32_t srcL2Id; //!< Source L2 ID
        uint32_t dstL2Id; //!< Destination L2 ID
    };

    /**
     * \brief Less than operator
     *
     * \param l first SidelinkLcIdentifier
     * \param r second SidelinkLcIdentifier
     * \returns true if first SidelinkLcIdentifier parameter values are less than the second
     * SidelinkLcIdentifier parameters"
     */
    friend bool operator<(const SidelinkLcIdentifier& l, const SidelinkLcIdentifier& r)
    {
        return l.lcId < r.lcId || (l.lcId == r.lcId && l.srcL2Id < r.srcL2Id) ||
               (l.lcId == r.lcId && l.srcL2Id == r.srcL2Id && l.dstL2Id < r.dstL2Id);
    }

    /// Sidelink Logical Channel Information
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
    void AddNrSlDstL2Id(uint32_t dstL2Id, uint8_t lcPriority);

    /**
     * \brief NR sidelink slot indication
     * \param sfn
     */
    void DoNrSlSlotIndication(const SfnSf& sfn);

    /**
     * \brief Get the list of the future reserved resources based on sensed data.
     * \param sensedData The data extracted from the sensed SCI 1-A.
     * \param slotPeriod Slot period
     * \param resvPeriodSlots Reservation period in slots
     * \param t1 Left edge of selection window (T1)
     * \param t2 Right edge of selection window (T2)
     * \return The list of the future transmission slots based on sensed data.
     */
    std::list<ReservedResource> ExcludeReservedResources(SensingData sensedData,
                                                         Time slotPeriod,
                                                         uint16_t resvPeriodSlots,
                                                         uint16_t t1,
                                                         uint16_t t2) const;

    /**
     * Private, internal (const) method invoked by public
     * NrSlUeMac::GetCandidateResources()
     *
     * \sa ns3::NrUeMac::GetCandidateResources
     *
     * \param sfn The current system frame, subframe, and slot number.
     * \param params The input transmission parameters for the algorithm
     * \param selectionParams The output selection parameters from the algorithm
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
    std::list<SlResourceInfo> GetCandidateResourcesPrivate(
        const SfnSf& sfn,
        const NrSlTransmissionParams& params,
        NrSlSelectionParams& selectionParams,
        Ptr<const NrSlCommResourcePool> txPool,
        Time slotPeriod,
        uint64_t imsi,
        uint8_t bwpId,
        uint16_t poolId,
        uint8_t totalSubCh,
        const std::list<SensingData>& sensingData,
        const std::list<SfnSf>& transmitHistory) const;

    /**
     * \brief Check if subchannels ranges in two resources overlap
     * \param firstStart Starting subchannel index of first resource
     * \param firstLength Number of subchannels of first resource (must be greater than 0)
     * \param secondStart Starting subchannel index of candidate resource
     * \param secondLength Number of subchannels of second resource (must be greater than 0)
     * \return True if the two resources overlap, false otherwise
     */
    bool OverlappedResource(uint8_t firstStart,
                            uint8_t firstLength,
                            uint8_t secondStart,
                            uint8_t secondLength) const;

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
    void RemoveOldSensingData(const SfnSf& sfn,
                              uint16_t sensingWindow,
                              std::list<SensingData>& sensingData,
                              [[maybe_unused]] uint64_t imsi);
    /**
     * \brief Remove transmit history older than T0 (sl-SensingWindow)
     * \param sfn The current system frame, subframe, and slot number. This SfnSf
     *        is aligned with the SfnSf of the physical layer.
     * \param sensingWindow The window length in slots (parameter sl-SensingWindow)
     * \param history Reference to the transmit history to be updated
     * \param imsi The IMSI of this instance
     */
    void RemoveOldTransmitHistory(const SfnSf& sfn,
                                  uint16_t sensingWindow,
                                  std::list<SfnSf>& history,
                                  [[maybe_unused]] uint64_t imsi);
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
    std::vector<uint8_t> ComputeGaps(const SfnSf& sfn,
                                     std::set<SlGrantResource>::const_iterator it,
                                     uint8_t slotNumInd);
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
    std::vector<uint8_t> GetStartSbChOfReTx(std::set<SlGrantResource>::const_iterator it,
                                            uint8_t slotNumInd);

    // Comparator function to sort pairs
    // according to second value
    /**
     * \brief Comparator function to sort pairs according to second value
     * \param a The first pair
     * \param b The second pair
     * \return returns true if second value of first pair is less than the second
     *         pair second value, false otherwise.
     */
    static bool CompareSecond(std::pair<uint32_t, uint8_t>& a, std::pair<uint32_t, uint8_t>& b);

    /**
     * \brief Check that T1 value is <= Tproc1
     *
     * \param sfn The SfnSf
     * \param t1Slots The value of T1 in slots
     * \return True if the condition is satisfied
     */
    bool CheckT1WithinTproc1(const SfnSf& sfn, uint16_t t1Slots) const;

    /**
     * \brief Convert a time quantity to slots based on numerology
     *
     * The time quantity must be less than 4000 ms to avoid integer overflow
     * if the maximum numerology shift is applied.
     *
     * \param sfn The SfnSf corresponding to numerology
     * \param timeVal The Time value to convert
     * \return the number of slots
     *
     * \note This utility function could possibly be moved to SfnSf
     */
    uint16_t TimeToSlots(const SfnSf& sfn, Time timeVal) const;

    /**
     * Called after all attributes are set on a newly constructed object.
     * Initializes the HARQ buffer using the configured pool size attributes.
     */
    void NotifyConstructionCompleted() override;

    std::map<SidelinkLcIdentifier, SlLcInfoUeMac>
        m_nrSlLcInfoMap; //!< Sidelink logical channel info map
    NrSlMacSapProvider*
        m_nrSlMacSapProvider; //!< SAP interface to receive calls from the UE RLC instance
    NrSlMacSapUser* m_nrSlMacSapUser{
        nullptr}; //!< SAP interface to call the methods of UE RLC instance
    NrSlUeCmacSapProvider* m_nrSlUeCmacSapProvider; //!< Control SAP interface to receive calls from
                                                    //!< the UE RRC instance
    NrSlUeCmacSapUser* m_nrSlUeCmacSapUser{
        nullptr}; //!< Control SAP interface to call the methods of UE RRC instance
    NrSlUePhySapProvider* m_nrSlUePhySapProvider{
        nullptr}; //!< SAP interface to call the methods of UE PHY instance
    NrSlUePhySapUser*
        m_nrSlUePhySapUser; //!< SAP interface to receive calls from the UE PHY instance
    Ptr<const NrSlCommResourcePool> m_slTxPool; //!< Sidelink communication transmission pools
    Ptr<const NrSlCommResourcePool> m_slRxPool; //!< Sidelink communication reception pools
    std::vector<std::pair<uint32_t, uint8_t>>
        m_sidelinkTxDestinations; //!< vector holding Sidelink communication destinations for
                                  //!< transmission and the highest priority value among its LCs
    std::unordered_set<uint32_t>
        m_sidelinkRxDestinations; //!< vector holding Sidelink communication destinations for
                                  //!< reception
    bool m_enableSensing{false};  //!< Flag to enable NR Sidelink resource selection based on
                                  //!< sensing; otherwise, use random selection
    uint8_t m_minTimeGapProcessing{
        std::numeric_limits<uint8_t>::max()}; //!< Minimum time for PSFCH processing
    uint8_t m_tproc0{0};                      //!< t_proc0 in slots
    uint8_t m_t1{0};  //!< The offset in number of slots between the slot in which the resource
                      //!< selection is triggered and the start of the selection window
    uint16_t m_t2{0}; //!< The configured value of T2 (end of selection window)
    std::map<SidelinkLcIdentifier, NrSlMacSapProvider::NrSlReportBufferStatusParameters>
        m_nrSlBsrReceived;                                   ///< NR Sidelink BSR received from RLC
    uint16_t m_poolId{std::numeric_limits<uint16_t>::max()}; //!< Sidelink active pool id
    Ptr<UniformRandomVariable>
        m_ueSelectedUniformVariable; //!< uniform random variable used for NR Sidelink
    std::map<uint32_t, std::deque<NrSlGrant>> m_slGrants;
    double m_slProbResourceKeep{0.0}; //!< Sidelink probability of keeping a resource after resource
                                      //!< re-selection counter reaches zero
    uint8_t m_slMaxTxTransNumPssch{0};    /**< Indicates the maximum transmission number
                                         (including new transmission and
                                         retransmission) for PSSCH.
                                         */
    uint8_t m_slMaxNumConsecutiveDtx;     //!< Max num consecutive HARQ FB failures to declare RLF
    Ptr<NrSlUeMacHarq> m_nrSlHarq;        //!< Pointer to the NR SL UE MAC HARQ object
    uint8_t m_maxSlProcess{16};           //!< MaxSidelinkProcess attribute value
    uint8_t m_maxSlProcessMultiplePdu{4}; //!< MaxSidelinkProcessMultiplePdu attribute value
    Ptr<NrSlUeMacScheduler> m_nrSlUeMacScheduler; //!< Pointer to the NR SL UE MAC scheduler
    uint32_t m_srcL2Id{std::numeric_limits<uint32_t>::max()}; //!< The NR Sidelink Source L2 id;
    bool m_nrSlMacPduTxed{false};         //!< Flag to indicate the TX of SL MAC PDU to PHY
    std::list<SensingData> m_sensingData; //!< List to store sensing data
    int m_thresRsrp{
        -128}; //!< A threshold in dBm used for sensing based UE autonomous resource selection
    uint8_t m_resPercentage{0}; /**< The percentage threshold to indicate the
                                     minimum number of candidate single-slot
                                     resources to be selected using sensing procedure.
                                     */

    std::list<SfnSf> m_transmitHistory; //!< History of slots used for transmission

    std::map<uint32_t, NumConsecutiveDtx>
        m_numConsecutiveDtx; //!< Tracks consecutive DTX for an L2 destination

    /**
     * Trace information regarding NR Sidelink PSCCH UE scheduling.
     * SlPscchUeMacStatParameters (see nr-sl-phy-mac-common.h)
     */
    TracedCallback<SlPscchUeMacStatParameters>
        m_slPscchScheduling; //!< NR SL PSCCH scheduling trace source
    /**
     * Trace information regarding NR Sidelink PSSCH UE scheduling.
     * SlPsschUeMacStatParameters (see nr-sl-phy-mac-common.h)
     */
    TracedCallback<SlPsschUeMacStatParameters>
        m_slPsschScheduling; //!< NR SL PSCCH scheduling trace source

    /**
     * Trace information regarding RLC PDU reception from MAC
     */
    TracedCallback<uint64_t, uint16_t, uint16_t, uint8_t, uint32_t, double> m_rxRlcPduWithTxRnti;

    /**
     * Trace information regarding sensing operation
     */
    TracedCallback<const struct SensingTraceReport&,
                   const std::list<SlResourceInfo>&,
                   const std::list<SensingData>&,
                   const std::list<SfnSf>&>
        m_tracedSensingAlgorithm;
};

/**
 * \brief Stream output operator
 * \param os output stream
 * \param p struct whose parameter to output
 * \return updated stream
 */
std::ostream& operator<<(std::ostream& os, const NrSlUeMac::NrSlTransmissionParams& p);

/**
 * \brief Stream output operator
 * \param os output stream
 * \param p struct whose parameter to output
 * \return updated stream
 */
std::ostream& operator<<(std::ostream& os, const struct NrSlUeMac::SensingTraceReport& report);

} // namespace ns3

#endif /* NR_SL_UE_MAC_H */
