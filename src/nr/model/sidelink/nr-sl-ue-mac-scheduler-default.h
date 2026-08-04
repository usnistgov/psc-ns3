// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#ifndef NR_SL_UE_MAC_SCHEDULER_DEFAULT_H
#define NR_SL_UE_MAC_SCHEDULER_DEFAULT_H

#include "nr-sl-comm-resource-pool.h"
#include "nr-sl-mcs-controller.h"
#include "nr-sl-phy-mac-common.h"
#include "nr-sl-ue-mac-harq.h"
#include "nr-sl-ue-mac-scheduler-dst-info.h"
#include "nr-sl-ue-mac-scheduler.h"
#include "nr-sl-ue-mac.h"

#include <ns3/callback.h>
#include <ns3/random-variable-stream.h>

#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <vector>

namespace ns3
{

struct AllocationInfo
{
    uint8_t m_priority{0};     //!< Priority
    bool m_isDynamic{false};   //!< Dynamic (per-PDU) scheduling indication (SPS when false)
    bool m_harqEnabled{false}; //!< Whether HARQ is enabled
    uint32_t m_tbSize{0};      //!< The transport block size
    uint32_t m_numTBs{1};      //!< Number of TBs needed for this allocation
    std::vector<SlRlcPduInfo> m_allocatedRlcPdus; //!< RLC PDUs
    Time m_rri{0};                                //!< Resource Reservation Interval (if SPS)
    SidelinkInfo::CastType m_castType{SidelinkInfo::CastType::Invalid}; //!< Cast type
    uint8_t m_reselCounter{0};              //!< The resource selection counter
    std::optional<uint8_t> m_selectedNumTx; //!< Per-grant numTx selected by the MCS
                                            //!< controller, clamped to pool maxNumTx.
                                            //!< std::nullopt = use pool maxNumTx.
};

/**
 * \brief Result of searching for a grant that serves a given logical channel
 */
struct GrantSearchResult
{
    bool found{false};        //!< Whether a grant was found for the LC
    bool foundForDest{false}; //!< Whether any grant exists for the destination
    std::vector<NrSlUeMacScheduler::GrantInfo>::iterator
        grant; //!< Iterator to the grant (valid only if found)
};

/**
 * \brief Result of selecting and filtering logical channels for a destination
 */
struct LcSelectionResult
{
    std::map<uint8_t, std::vector<uint8_t>> lcIdsByPrio; //!< LC IDs grouped by priority
    uint8_t lcIdOfRef{0};                                //!< Reference LC ID (highest priority,
                                                         //!< smallest LC ID)
    bool isDynamic{false};                               //!< Whether the grant is dynamic
    bool isSupplemental{false};                          //!< Whether this is a supplemental grant
};

/**
 * \ingroup scheduler
 *
 * \brief A general scheduler for NR SL UE that uses a fixed MCS by default, and supports multiple
 *        logical channels, prioritization, both dynamic and SPS grants, and the ability to
 *        change the MCS during runtime on a per-destination basis
 */
class NrSlUeMacSchedulerDefault : public NrSlUeMacScheduler
{
  public:
    /**
     * \brief GetTypeId
     *
     * \return The TypeId of the class
     */
    static TypeId GetTypeId(void);

    /**
     * \brief NrSlUeMacSchedulerDefault default constructor
     */
    NrSlUeMacSchedulerDefault();

    /**
     * \brief NrSlUeMacSchedulerDefault destructor
     */
    ~NrSlUeMacSchedulerDefault() override;

    /**
     * Set the MCS value to use for future grants scheduled towards a destination
     *
     * Any currently scheduled grant will retain the MCS that was configured
     * at the time of the grant scheduling.  In the case of an SPS grant, upon
     * grant reselection evaluation, a change of MCS during the last grant period
     * will force a reselection.
     *
     * \param dstL2Id The destination L2 Id
     * \param mcs The MCS value to use
     */
    void SetMcs(uint32_t dstL2Id, uint8_t mcs);

    /**
     * Structure to pass trace information about the execution of the
     * scheduling algorithm.
     */
    struct SchedulingReport
    {
        SfnSf m_sfn;
        uint16_t m_subchannels;
        uint16_t m_psfchPeriod;
        uint8_t m_t1;
        uint16_t m_t2;
    };

    /**
     * \brief Function to retrieve the unpublished grants.
     * \return map with unpublished grants
     */
    const std::map<uint32_t, std::vector<GrantInfo>> GetUnpublishedGrants() const;

    /**
     * TracedCallback signature for scheduling report
     *
     * \param [in] schedulingReport The scheduling report
     * \param [in] candidateResources The candidate resources considered
     * \param [in] transmissionParams The transmission parameters
     * \param [in] publishedGrants Vector of published grants
     * \param [in] unpublishedGrants Vector of unpublished grants by dstL2Id
     * \param [in] grant The GrantInfo for the scheduled grant
     */
    typedef void (*SchedulingReportCallback)(
        const struct SchedulingReport& schedulingReport,
        const std::list<SlResourceInfo>& candidateResources,
        const struct NrSlUeMac::NrSlTransmissionParams& transmissionParams,
        const std::vector<SlGrantResource>& publishedGrants,
        const std::map<uint32_t, std::vector<GrantInfo>>& unpublishedGrants,
        const struct GrantInfo& grant);

    /**
     * TracedCallback signature for MCS change
     *
     * \param [in] dstL2Id The destination L2 ID
     * \param [in] oldMcs The old MCS value
     * \param [in] newMcs The new MCS value
     */
    typedef void (*McsChangeCallback)(uint32_t dstL2Id, uint8_t oldMcs, uint8_t newMcs);

    // From parent class
    /**
     * \brief Assign a fixed random variable stream number to the random variables
     * used by this model. Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream The first stream index to use
     * \return The number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream) override;

    /**
     * Structure to indicate the level of ideal scheduling knowledge used when filtering candidate
     * resources.
     */
    enum IdealSchedLevel
    {
        IDEAL_SCHED_NONE = 0,
        IDEAL_SCHED_GLOBAL,
        IDEAL_SCHED_LOCAL_UNICAST
    };

  private:
    void DoRemoveNrSlLcConfigReq(uint8_t lcid, uint32_t dstL2Id) override;

    void DoSchedNrSlRlcBufferReq(
        const struct NrSlMacSapProvider::NrSlReportBufferStatusParameters& params) override;

    void DoSchedNrSlTriggerReq(const SfnSf& sfn) override;

    void DoCschedNrSlLcConfigReq(
        const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& params) override;

    void DoNotifyNrSlRlcPduDequeue(uint32_t dstL2Id, uint8_t lcId, uint32_t size) override;

    /**
     * \brief Perform the Tx resource (re-)selection check for the given destination and logical
     * channel
     *
     * \param sfn The SfnSf
     * \param dstL2Id The destination layer 2 ID
     * \param lcId The logical channel ID
     * \return True if the LC passes the check, false otherwise
     */
    bool TxResourceReselectionCheck(const SfnSf& sfn, uint32_t dstL2Id, uint8_t lcId);
    /**
     * \brief Select the destinations and logical channels that need scheduling
     *
     * The function fills the dstsAndLcsToSched map with the destinations and logical channels that
     * pass the transmission resource (re-)selection check in function TxResourceReselectionCheck
     *
     * \param sfn The SfnSf
     * \param dstsAndLcsToSched The map of destinations and logical channels IDs to be updated
     */
    void GetDstsAndLcsNeedingScheduling(
        const SfnSf& sfn,
        std::map<uint32_t, std::vector<uint8_t>>& dstsAndLcsToSched);
    /**
     * \brief Select the destination and logical channels to be allocated
     *
     * The selection and allocation is done according to TS 38.321 V16.11.0 Section 5.22.1.4.1
     * At the moment prioritized bitrate is not supported, thus the logic implemented in this
     * function assumes sPBR = infinity and sBj > 0 for all LCs.
     *
     * \param sfn The SfnSf
     * \param dstsAndLcsToSched The map of destinations and logical channels IDs to allocate
     * \param allocationInfo the allocation information to be updated
     * \param candResources the list of resources to be updated with the ones used for the
     * allocation
     *
     * \return destination layer 2 ID of the allocated destination, if any
     */
    std::optional<uint32_t> LogicalChannelPrioritization(
        const SfnSf& sfn,
        std::map<uint32_t, std::vector<uint8_t>> dstsAndLcsToSched,
        AllocationInfo& allocationInfo,
        std::list<SlResourceInfo>& candResources);
    /**
     * \brief Attempt to select new grant from the selection window
     *
     * If successful, CreateSpsGrant () will be called for SPS grants
     * or CreateSinglePduGrant () for dynamic grants
     *
     * \param sfn The SfnSf
     * \param dstL2Id The destination layer 2 id
     * \param candResources The list of candidate resources
     * \param allocationInfo the allocation information to use
     * \return true if a grant allocation was made
     */
    bool AttemptGrantAllocation(const SfnSf& sfn,
                                uint32_t dstL2Id,
                                const std::list<SlResourceInfo>& candResources,
                                const AllocationInfo& allocationInfo);
    /**
     * \brief Create future SPS grants based on slot allocation
     *
     * \param slotAllocList The slot allocation list
     * \param allocationInfo the allocation information to use
     * \return The grant info for a destination based on the scheduler allocation
     *
     * \see SlGrantResource
     * \see GrantInfo
     */
    GrantInfo CreateSpsGrantInfo(const std::set<SlGrantResource>& params,
                                 const AllocationInfo& allocationInfo) const;
    /**
     * \brief Create a single-PDU grant based on slot allocation
     *
     * \param slotAllocList The slot allocation list
     * \param allocationInfo the allocation information to use
     * \return The grant info for a destination based on the scheduler allocation
     *
     * \see SlGrantResource
     * \see GrantInfo
     */
    GrantInfo CreateSinglePduGrantInfo(const std::set<SlGrantResource>& params,
                                       const AllocationInfo& allocationInfo) const;

    /**
     * \brief Check if the resources indicated by two SFN/subchannel ranges overlap
     *
     * \param firstSfn The first SfnSf to compare
     * \param firstStart The starting subchannel index of the first resource
     * \param firstLength The subchannel length of the first resource
     * \param secondSfn The second  SfnSf to compare
     * \param secondStart The starting subchannel index of the second resource
     * \param secondLength The subchannel length of the second resource
     * \return Whether the two resources overlap
     */
    bool OverlappedResources(const SfnSf& firstSfn,
                             uint16_t firstStart,
                             uint16_t firstLength,
                             const SfnSf& secondSfn,
                             uint16_t secondStart,
                             uint16_t secondLength) const;

    /**
     * \brief Removes resources which are already part of an existing grant.
     *
     * \param sfn The current SfnSf
     * \param txOppr The list of available slots
     * \param rri The RRI for SPS grants
     * \param cResel The cResel value for SPS grants
     * \return The list of resources which are not used by any existing grant.
     */
    std::list<SlResourceInfo> FilterTxOpportunities(const SfnSf& sfn,
                                                    std::list<SlResourceInfo> txOppr,
                                                    Time rri,
                                                    uint16_t cResel);

    /**
     * \brief Calculate a timeout value for HARQ process ID deallocation.
     *
     * The SL HARQ entity will keep the HARQ process ID allocated until
     * the TB is ACKed or until after the last transmission.  This
     * method calculates the timeout time to pass to the HARQ entity.
     * If no HARQ FB is configured, the time corresponds to one slot
     * beyond the last granted slot.  If HARQ FB is configured, the
     * time corresponds to a time at which HARQ FB from the last scheduled
     * retransmission should have had a chance to have been returned.
     * If no slots have been granted (std::nullopt), a minimum timeout
     * of one slot is returned.
     *
     * @param sfn The current SfnSf
     * @param lastTxSlotSfn The SfnSf of the last slot granted for transmission
     *        (including retransmissions), or std::nullopt if no slots have been granted
     * @param harqEnabled Whether HARQ feedback is enabled
     * @param psfchPeriod The PSFCH period in slots
     * @param minTimeGapPsfch The minimum slot gap from PSSCH to its PSFCH feedback
     * @return the timeout value to recommend for recycling the process ID
     */
    Time CalculateProcessTimeout(const SfnSf& sfn,
                                 const std::optional<SfnSf>& lastTxSlotSfn,
                                 bool harqEnabled,
                                 uint16_t psfchPeriod,
                                 uint8_t minTimeGapPsfch) const;

    /**
     * \brief Calculate a process ID timeout value for a SPS grant allocation.
     *
     * For SPS grants, the SL HARQ entity will keep the HARQ process ID
     * allocated until the process is deallocated or a timeout occurs.
     * This scheduler typically deallocates and then reallocates SPS
     * grants every (ResourceReselCounter) * RRI time interval.
     *
     * This method calculates a failsafe timeout time to pass to the HARQ
     * entity, in case the scheduler does not explicitly deallocate the
     * HARQ process ID.  If an SPS grant is scheduled for 'ResourceReselCounter'
     * future iterations, with new transmissions separated by the RRI interval,
     * this method will schedule a timeout of the HARQ process ID at
     * (ResourceReselCounter + 1) * RRI time in the future.  Allowing one
     * extra RRI allows for some amount of jitter in the packet arrival
     * process.
     *
     * Note: this may be a candidate for refactoring and reuse of the preceding method
     *
     * \param sfn The SfnSf
     * \param resoReselCounter The resource reselection counter
     * \param rri The resource reservation interval
     * \return the timeout value to pass to the NrSlUeMac for recycling the process ID
     */
    Time CalculateSpsProcessTimeout(const SfnSf& sfn, uint8_t resoReselCounter, Time rri) const;

    /**
     * \brief Regenerate slot allocations for an SPS grant that is being kept
     * \param sfn The current SfnSf
     * \param grant The grant to regenerate slot allocations for
     *
     * When the reselection counter reaches 0 and the keep decision has been
     * made, this method regenerates slot allocations using the stored base
     * slot pattern (same subchannels, same NDI/retransmission structure)
     * projected forward in time from the last published NDI slot.
     */
    void RegenerateSpsSlotAllocations(const SfnSf& sfn, GrantInfo& grant);

    /**
     * \brief Make the keep/reselect decision for any SPS grant whose
     * reselection counter has reached 1
     */
    void MakeSpsReselectionDecisions();

    /**
     * \brief Search all grants for a destination to find one serving a given LC
     *
     * For SPS grants at counter == 0 whose slot allocations have been consumed,
     * the search falls back to the base slot pattern. For non-dynamic grants
     * with active slot allocations, the MCS query callback is invoked and MCS
     * staleness is detected.
     *
     * \param sfn The current SfnSf
     * \param dstL2Id The destination layer-2 ID
     * \param lcId The logical channel ID
     * \return The search result including grant iterator and MCS change flag
     */
    GrantSearchResult FindGrantForLc(const SfnSf& sfn, uint32_t dstL2Id, uint8_t lcId);

    /**
     * \brief Check whether a dynamic LC needs a new grant
     *
     * Compare the LC buffer size against the total already allocated to
     * existing dynamic grants for this LC. A new grant is needed when
     * the buffer exceeds the allocated amount.
     *
     * \param dstL2Id The destination layer-2 ID
     * \param lcId The logical channel ID
     * \param lcBufferSize The current LC buffer size in bytes
     * \param grantFoundForDest Whether any grant exists for the destination
     * \return true if a new dynamic grant should be scheduled
     */
    bool CheckDynamicLcNeedsScheduling(uint32_t dstL2Id,
                                       uint8_t lcId,
                                       uint32_t lcBufferSize,
                                       bool grantFoundForDest) const;

    /**
     * \brief Calculate the effective allocated capacity for a logical channel
     * across all existing grants for a destination
     *
     * For each grant, the effective capacity is the TB payload (TB size minus
     * MAC subheader overhead) minus the allocations for other LCs in the
     * same grant.
     *
     * \param dstL2Id The destination layer-2 ID
     * \param lcId The logical channel ID
     * \return The effective allocated capacity in bytes
     */
    uint32_t CalculateEffectiveAllocatedCapacityForLc(uint32_t dstL2Id, uint8_t lcId) const;

    /**
     * \brief Select the highest-priority destination from those needing scheduling
     *
     * Build a priority-keyed map of destinations, select the highest priority
     * level, and break ties randomly.
     *
     * \param dstsAndLcsToSched Map of destinations to their LCs needing scheduling
     * \return The selected destination layer-2 ID, or empty if no destinations
     */
    std::optional<uint32_t> SelectDestinationByPriority(
        const std::map<uint32_t, std::vector<uint8_t>>& dstsAndLcsToSched);

    /**
     * \brief Allocate TB payload capacity to logical channels by priority
     *
     * Drain the priority-ordered allocation queue, dividing capacity equally
     * among LCs at the same priority level. Populate allocationInfo with the
     * resulting RLC PDU allocations.
     *
     * \param allocQueue Priority-ordered queue of LC ID vectors
     * \param tbPayloadCapacity TB size available for RLC data
     * \param dstL2Id The destination layer-2 ID (for logging)
     * \param allocationInfo Allocation info to populate with RLC PDU assignments
     */
    void AllocateLcsToResources(std::queue<std::vector<uint8_t>>& allocQueue,
                                uint32_t tbPayloadCapacity,
                                uint32_t dstL2Id,
                                AllocationInfo& allocationInfo);

    /**
     * \brief Remove served LCs from the scheduling map after a successful allocation
     *
     * If all LCs for the destination were allocated, remove the entire
     * destination entry. Otherwise, remove only the individual LCs that
     * were served.
     *
     * \param dstL2Id The destination layer-2 ID
     * \param allocationInfo The allocation result containing served RLC PDUs
     * \param dstsAndLcsToSched The scheduling map to update
     */
    void RemoveServedLcsFromSchedulingMap(
        uint32_t dstL2Id,
        const AllocationInfo& allocationInfo,
        std::map<uint32_t, std::vector<uint8_t>>& dstsAndLcsToSched);

    /**
     * \brief Select and filter logical channels for a destination
     *
     * Group the LCs of a destination by priority, determine whether the grant
     * should be dynamic or SPS, handle supplemental override for SPS LCs
     * that need extra capacity, filter LCs by scheduling type, filter SPS
     * LCs by RRI to match the reference LC, and check SPS TB capacity.
     * Set allocationInfo fields for scheduling type, HARQ, RRI, reselection
     * counter, priority, and cast type.
     *
     * \param dstL2Id The selected destination layer-2 ID
     * \param dstsAndLcsToSched Map of destinations to their LCs needing scheduling
     * \param allocationInfo Allocation info to populate with scheduling parameters
     * \param minSymbolsPerSlot Minimum PSSCH symbols per slot
     * \param hasPsfch Whether PSFCH is configured
     * \return The filtered LC selection result
     */
    LcSelectionResult SelectAndFilterLcs(
        uint32_t dstL2Id,
        const std::map<uint32_t, std::vector<uint8_t>>& dstsAndLcsToSched,
        AllocationInfo& allocationInfo,
        uint16_t minSymbolsPerSlot,
        bool hasPsfch);

    /**
     * \brief Method to create future SPS grant repetitions
     * \param sfn The SfnSf
     * \param slotAllocList The slot allocation list from the selection window
     * \param allocationInfo the allocation information to use
     *
     * \see SlGrantResource
     */
    void CreateSpsGrant(const SfnSf& sfn,
                        const std::set<SlGrantResource>& slotAllocList,
                        const AllocationInfo& allocationInfo);
    /**
     * \brief Method to create a single-PDU grant
     * \param sfn The SfnSf
     * \param slotAllocList The slot allocation list from the selection window
     * \param allocationInfo the allocation information to use
     *
     * \see SlGrantResource
     */
    void CreateSinglePduGrant(const SfnSf& sfn,
                              const std::set<SlGrantResource>& slotAllocList,
                              const AllocationInfo& allocationInfo);

    /**
     * \brief Check whether any grants are at the processing delay deadline
     *        to send back to NrUeMac
     * \param sfn The current SfnSf
     */
    void CheckForGrantsToPublish(const SfnSf& sfn);

    /**
     * \brief Get Redundancy Version number
     *
     * We assume rvid = 0, so RV would take 0, 2, 3, 1. See TS 38.21 table 6.1.2.1-2
     *
     * \param txNumTb The transmission index of the TB, e.g., 0 for initial tx,
     *        1 for a first retransmission, and so on.
     * \return The Redundancy Version number
     */
    uint8_t GetRv(uint8_t txNumTb) const;

  protected:
    // Inherited from Object
    void DoDispose() override;

    /**
     * \brief Create a SlotInfo structure
     *
     * \param symbolsPerSlot number of symbols to assume in a slot
     * \param hasPsfch whether to assume slot has a PSFCH symbol
     * \return A SlotInfo structure according the the transmit pool
     */
    NrSlCommResourcePool::SlotInfo CreateSlotInfo(uint16_t symbolsPerSlot, bool hasPsfch) const;

    /**
     * \brief Do the NE Sidelink allocation
     *
     * This function selects resources from the candidate list and associate them
     * to the allocation parameters selected by the scheduler.
     * The SCI 1-A is Txed with every new transmission and after the transmission
     * for, which \c txNumTb mod MaxNumPerReserved == 0 \c , where the txNumTb
     * is the transmission index of the TB, e.g., 0 for initial tx, 1 for a first
     * retransmission, and so on.
     * Finally, the function updates the logical channels with the corresponding
     * assigned data.
     *
     * \param candResources The list of candidate resources received from the UE MAC
     * \param dstInfo The pointer to the NrSlUeMacSchedulerDstInfo of the destination
     *        for which UE MAC asked the scheduler to allocate the resourses
     * \param slotAllocList The slot allocation list to be updated by the scheduler
     * \param allocationInfo the allocation parameters to be associated to the selected
     *        resources
     * \return The status of the resource allocation, true if the destination has been
     *         allocated some resources; false otherwise.
     */
    virtual bool DoNrSlAllocation(const std::list<SlResourceInfo>& candResources,
                                  const std::shared_ptr<NrSlUeMacSchedulerDstInfo>& dstInfo,
                                  std::set<SlGrantResource>& slotAllocList,
                                  const AllocationInfo& allocationInfo);
    /**
     * \brief Method to get total number of sub-channels.
     *
     * \return the total number of sub-channels.
     */
    uint8_t GetTotalSubCh() const;

    /**
     * \brief Method to get the maximum transmission number
     *        (including new transmission and retransmission) for PSSCH.
     *
     * \return The max number of PSSCH transmissions
     */
    uint8_t GetSlMaxTxTransNumPssch() const;

    Ptr<UniformRandomVariable> m_grantSelectionUniformVariable; //!< Used for grant selection
    Ptr<UniformRandomVariable> m_destinationUniformVariable; //!< Used for destination randomization

  private:
    /**
     * \brief Create destination info
     *
     * If the scheduler does not have the destination info then it creates it,
     * and then save its pointer in the m_dstMap map.
     *
     * If the scheduler already have the destination info, it does noting. This
     * could happen when we are trying add more than one logical channels
     * for a destination.
     *
     * \param params params of the UE
     * \return A std::shared_ptr to newly created NrSlUeMacSchedulerDstInfo
     */
    std::shared_ptr<NrSlUeMacSchedulerDstInfo> CreateDstInfo(
        const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& params);

    /**
     * \brief Remove destination info
     *
     * \param lcid LC ID
     * \param dstL2Id destination L2 ID
     */
    void RemoveDstInfo(uint8_t lcid, uint32_t dstL2Id);

    /**
     * \brief Create a NR Sidelink logical channel group
     *
     * A subclass can return its own representation of a logical channel by
     * implementing a proper subclass of NrSlUeMacSchedulerLCG and returning a
     * pointer to a newly created instance.
     *
     * \param lcGroup The logical channel group id
     * \return a pointer to the representation of a logical channel group
     */
    NrSlLCGPtr CreateLCG(uint8_t lcGroup) const;

    /**
     * \brief Create a NR Sidelink logical channel
     *
     * A subclass can return its own representation of a logical channel by
     * implementing a proper subclass of NrSlUeMacSchedulerLC and returning a
     * pointer to a newly created instance.
     *
     * \param params configuration of the logical channel
     * \return a pointer to the representation of a logical channel
     */

    NrSlLCPtr CreateLC(const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& params) const;

    /**
     * \brief Return true if candidate resource overlaps in time (slot)
     *        with any resource on the list.
     *
     * \param resources List of resources to check
     * \param candidate candidate to check
     * \return true if candidate overlaps any slots in the list of resources
     */
    bool OverlappedSlots(const std::list<SlResourceInfo>& resources,
                         const SlResourceInfo& candidate) const;

    /**
     * \brief Randomly select resources for a grant from the candidate resources
     *
     * If K denotes the candidate resources, and N_PSSCH_maxTx is the
     * maximum number of PSSCH configured transmissions, then:
     *
     * N_Selected = N_PSSCH_maxTx , if K >= N_PSSCH_maxTx
     * otherwise;
     * N_Selected = K
     *
     * If HARQ is disabled, N_Selected = 1 resource.
     *
     * \param txOpps The list of the candidate resources
     * \return The list of randomly selected resources
     */
    std::list<SlResourceInfo> SelectResourcesForBlindRetransmissions(
        std::list<SlResourceInfo> txOpps);

    /**
     * \brief Randomly select resources for a grant from the candidate resources,
     *        subject to the constraint of a minimum time gap betweeen resources
     *
     * Select one or more resources, such that retransmission resources are
     * separated by a minimum time gap configured for this resource pool, and
     * such that a retransmission resource can be indicated by the time resource
     * assignment of a prior SCI according to clause 8.3.1.1 of TS 38.212
     *
     * If K denotes the candidate resources, and N_PSSCH_maxTx is the
     * maximum number of PSSCH configured transmissions, then:
     *
     * N_Selected <= N_PSSCH_maxTx , if K >= N_PSSCH_maxTx
     * otherwise;
     * N_Selected <= K
     *
     * If HARQ is disabled, N_Selected = 1 resource.
     *
     * \param txOpps The list of the candidate resources
     * \param harqEnabled Whether HARQ retransmission resources should be selected
     * \param selectedNumTx Optional per-grant cap on N_Selected.  When present,
     *        it overrides N_PSSCH_maxTx as the upper bound on the number of
     *        resources selected (the caller is responsible for clamping to the
     *        pool's configured maximum).  When absent, N_PSSCH_maxTx is used.
     * \return The list of randomly selected resources
     */
    std::list<SlResourceInfo> SelectResourcesWithConstraint(
        std::list<SlResourceInfo> txOpps,
        bool harqEnabled,
        std::optional<uint8_t> selectedNumTx = std::nullopt);

    /**
     * Check if the time difference between the two slots meets or exceeds the
     * minimum time gap for retransmission.
     *
     * \param first First slot
     * \param second Second slot (possible retransmission)
     * \param minTimeGapPsfch MinTimeGapPsfch value (slots)
     * \param minTimeGapProcessing MinTimeGapProcessing value (slots)
     * \return true if minimum time gap is satisfied
     */
    bool IsMinTimeGapSatisfied(const SfnSf& first,
                               const SfnSf& second,

                               uint8_t minTimeGapPsfch,
                               uint8_t minTimeGapProcessing) const;
    /**
     * Check that the candidate resource conforms to the minimum time gap between
     * any two selected resources as specified in TS 38.321 Section 5.22.1.1
     *
     * \param txOpps list of currently selected resources
     * \param resourceInfo information of the candidate resource
     * \return true if minimum time gap of candidate resource is satisfied
     */
    bool IsCandidateResourceEligible(const std::list<SlResourceInfo>& txOpps,
                                     const SlResourceInfo& resourceInfo) const;

    /**
     * \brief Get the random selection counter
     * \param rri The RRI value
     * \return The randomly selected reselection counter
     *
     * See 38.321 section 5.22.1.1 V16
     *
     * For 50 ms we use the range as per 36.321 section 5.14.1.1
     */
    uint8_t GetRandomReselectionCounter(Time rri) const;
    /**
     * \brief Get the lower bound for the Sidelink resource re-selection
     *        counter when the resource reservation period is less than
     *        100 ms. It is as per the Change Request (CR) R2-2005970
     *        to TS 38.321.
     * \param pRsrv The resource reservation period
     * \return The lower bound of the range from which Sidelink resource re-selection
     *         counter will be drawn.
     */
    uint8_t GetLowerBoundReselCounter(uint16_t pRsrv) const;
    /**
     * \brief Get the upper bound for the Sidelink resource re-selection
     *        counter when the resource reservation period is less than
     *        100 ms. It is as per the Change Request (CR) R2-2005970
     *        to TS 38.321.
     * \param pRsrv The resource reservation period
     * \return The upper bound of the range from which Sidelink resource re-selection
     *         counter will be drawn.
     */
    uint8_t GetUpperBoundReselCounter(uint16_t pRsrv) const;
    /**
     * \brief utility function to retrieve and cache a pointer to NrSlUeMacHarq object
     * \return pointer to NrSlUeMacHarq
     */
    Ptr<NrSlUeMacHarq> GetMacHarq(void) const;
    /**
     * \brief Remove unpublished grants for a given logical channel
     * \param lcid The logical channel id
     * \param dstL2Id The destination layer-2 id
     */
    void RemoveUnpublishedGrants(uint8_t lcid, uint32_t dstL2Id);

    /**
     * \brief Function to retrieve external grants from other nodes. Used for scheduling with global
     * knowledge.
     * \param map with external grants to be updated by the function
     */
    void GetExternalGrants(std::map<uint32_t, std::vector<GrantInfo>>& externalGrants) const;

    /**
     * @brief Inidicates if there is a unicast logical channel towards the peer L2 ID
     *
     * @return peerL2Id the Layer 2 ID of the peer UE
     */
    bool HasUnicastLogicalChannelTo(uint32_t peerL2Id) const;

    /**
     * \brief Calculate the number of TBs needed to transmit a buffer
     *
     * Accounts for 2-byte RLC segment header overhead per segment.
     *
     * \param bufferSize The total buffer size in bytes
     * \param tbSize The transport block size in bytes
     * \return The number of TBs required
     * \note Returns 0 if bufferSize is 0
     */
    uint32_t CalculateNumTbNeeded(uint32_t bufferSize, uint32_t tbSize) const;

    /**
     * \brief Remove resources that overlap with allocated slots from the candidate list
     *
     * \param candidates List of candidate resources
     * \param usedSlots Set of slot allocations from a created grant
     * \return Filtered list of remaining candidate resources
     */
    std::list<SlResourceInfo> FilterUsedResources(const std::list<SlResourceInfo>& candidates,
                                                  const std::set<SlGrantResource>& usedSlots) const;

    IdealSchedLevel m_idealSchedLevel{
        IDEAL_SCHED_NONE}; //!< Level of ideal scheduling knowledge used when filtering candidate
                           //!< resources

    uint32_t m_minimumSpsGrantSize{0}; //!< Minimum SPS grant size, in bytes

    uint32_t m_spsReselectionThreshold{0}; //!< SPS grant reselection threshold, in bytes

    std::list<SlResourceInfo>
        m_candidateResources; //!< Saved result of calling GetCandidateResources()

    NrSlUeMac::NrSlTransmissionParams
        m_transmissionParams; //!< Saved result of parameters used for m_candidateResources

    NrSlUeMac::NrSlSelectionParams
        m_selectionParams; //!< Saved result of parameters output from selection algorithm

    std::map<uint32_t, std::shared_ptr<NrSlUeMacSchedulerDstInfo>>
        m_dstMap; //!< The map of between destination layer 2 id and the destination info

    Ptr<NrAmc> m_nrSlAmc; //!< AMC pointer for NR SL

    uint8_t m_defaultMcs{0}; //!< default value for MCS

    std::map<uint32_t, std::vector<GrantInfo>>
        m_grantInfo; //!< (unpublished) grants, indexed by dstL2Id

    std::vector<SlGrantResource> m_publishedGrants; //!< published grants

    Ptr<UniformRandomVariable>
        m_ueSelectedUniformVariable; //!< uniform random variable used for NR Sidelink
    uint8_t m_t1{2}; //!< The offset in number of slots between the slot in which the resource
                     //!< selection is triggered and the start of the selection window

    bool m_prioToSps{true}; //!< Flag to give scheduling priority to logical channels that are
                            //!< configured with SPS in case of priority tie
    bool m_wholeSlotExclusion{false}; //!< Flag to exclude scheduling in a slot for which
                                      //!< reception on at least one resource is sensed
    bool m_allowMultipleDestinationsPerSlot{
        false}; //!< Allow scheduling of multiple destinations in same slot
    mutable Ptr<NrSlUeMacHarq> m_nrSlUeMacHarq{nullptr}; //!< Pointer to cache object

    std::set<uint8_t> m_supplementalLcIds; //!< LC IDs needing supplemental dynamic grants

    bool m_allowSupplementalGrants{
        true}; //!< Allow supplemental dynamic grants for overloaded SPS grants

    Ptr<NrSlMcsController> m_mcsController{nullptr}; //!< Pointer to optional MCS controller

    bool m_slotSkipOnMcsControllerNullopt{true}; //!< If true, skip the slot when the MCS
                                                 //!< controller returns nullopt; if false,
                                                 //!< fall back to MCS 0 at the current
                                                 //!< allocation.

    std::map<uint32_t, uint8_t> m_mcsCache; //!< Cache calls to SetMcs() before a dstInfo exists

    TracedCallback<const struct SchedulingReport&,
                   const std::list<SlResourceInfo>&,
                   const struct NrSlUeMac::NrSlTransmissionParams&,
                   const std::vector<SlGrantResource>&,
                   const std::map<uint32_t, std::vector<GrantInfo>>&,
                   const struct GrantInfo&>
        m_schedulingTrace; //!< Trace source for scheduling report

    TracedCallback<uint32_t, uint8_t, uint8_t> m_mcsChangeTrace; //!< Trace source for MCS change
};

} // namespace ns3

#endif /* NR_SL_UE_MAC_SCHEDULER_DEFAULT_H */
