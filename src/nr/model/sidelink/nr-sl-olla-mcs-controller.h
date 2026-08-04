// SPDX-License-Identifier: NIST-Software

#ifndef NR_SL_OLLA_MCS_CONTROLLER_H
#define NR_SL_OLLA_MCS_CONTROLLER_H

#include "nr-sl-comm-resource-pool.h"
#include "nr-sl-error-model.h"
#include "nr-sl-mcs-controller.h"
#include "nr-sl-phy-mac-common.h"

#include "ns3/event-id.h"
#include "ns3/random-variable-stream.h"
#include "ns3/timer.h"
#include <ns3/object.h>

#include <deque>
#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns3
{

/**
 * \ingroup scheduler
 * TODO: Description
 *
 *
 * To suppress adjustment of broadcast or groupcast L2 IDs, this model uses a MaximumL2Id
 * attribute to indicate the maximum value of L2 ID that will be controlled by this controller.
 * For values greater than this maximum, this controller will not attempt any MCS changes.
 * The value is a 32 bit unsigned integer initialized to the value 200.
 */
class NrSlOllaMcsController : public NrSlMcsController
{
  public:
    /**
     * \brief Get the type id
     * \return the type id of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSlOllaMcsController constructor
     */
    NrSlOllaMcsController();

    /**
     * \brief Destructor.  Cancels any pending SaveSinr and CQI update events.
     */
    ~NrSlOllaMcsController() override;

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model.  Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream) override;

    /**
     * Remove history value(s) for a given L2 ID
     */
    void Flush(uint32_t dstL2Id);

    /**
     * Use binary search to find the highest MCS that provides a BLER less than or
     * equal to the target BLER.  Returns std::nullopt if no MCS meets the target.
     *
     * This is a static utility method that can be used independently of an
     * NrSlOllaMcsController instance; for example, in unit tests that need
     * a selection of MCS from an idealized oracle.
     *
     * \param errorModel The sidelink error model to query
     * \param numerology The numerology
     * \param rv The redundancy version
     * \param snrDb The SNR in dB
     * \param targetBler The target block error rate
     * \param tbSize The transport block size in bytes
     * \param numRbs The number of resource blocks
     * \return The highest MCS with BLER <= targetBler, or std::nullopt
     */
    static std::optional<uint8_t> BinarySearch(Ptr<const NrSlErrorModel> errorModel,
                                               uint16_t numerology,
                                               uint8_t rv,
                                               double snrDb,
                                               double targetBler,
                                               uint32_t tbSize,
                                               uint32_t numRbs);

    [[nodiscard]] double GetTargetBler() const;
    void SetTargetBler(double tbler);

    [[nodiscard]] double GetNackPenaltyStep() const;
    void SetNackPenaltyStep(double nackPenalty);

    [[nodiscard]] double GetAckStep() const;

    /**
     * Get the last SINR the model received
     * for a given L2 ID.
     *
     * @param l2Id
     * The L2 ID to lookup
     *
     * @return The Last SINR for `l2Id` or an empty `std::optional`
     * if there isn't one
     */
    [[nodiscard]] std::optional<double> GetLastSinr(uint32_t l2Id) const;

  protected:
    void DoDispose() override;

  private:
    using L2Id = uint32_t;

    /**
     * Information recorded every time
     * `NotifyRxPssch()` is triggered
     */
    struct SinrTraceEntry
    {
        Time receivedAt;
        double sinrDb;    ///< wideband SINR in dB, normalized to full band occupancy
        uint32_t poolRbs; ///< total RBs in the resource pool (for de-normalization)
    };

    struct SinrDeltaEntry
    {
        Time updatedAt;
        double sinrOffsetDelta;
    };

    /**
     * CQI estimate for a destination, exported to DoGetGrantParams.
     * The sinrDb is the max over the per-destination ring buffer of
     * recent raw SINR samples, periodically gated by `CqiPeriod`.
     */
    struct FilteredSinrEntry
    {
        Time timestamp;
        double sinrDb{std::numeric_limits<double>::lowest()}; ///< max-filtered SINR in dB
                                                              ///< (wideband, full band occupancy)
    };

    void DoSetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler) override;
    GrantParams DoGetGrantParams(uint32_t dstL2Id,
                                 SidelinkInfo::CastType castType,
                                 std::optional<uint32_t> numRbs,
                                 std::optional<uint32_t> tbSize) override;
    GrantParams DoGetLargestFeasibleTb(uint32_t dstL2Id,
                                       SidelinkInfo::CastType castType,
                                       uint16_t maxSubCh) override;
    void NotifyRxPssch(std::string context, SlRxDataPacketTraceParams traceParams);
    void RxHarqFeedback(const SlHarqInfo& harqInfo);
    void ConfigureTraces();
    void SaveSinr(L2Id destination, SinrTraceEntry entry);

    /**
     * Sample the current max over the raw-sample buffer into
     * m_filteredSinrTraces for the given destination, and schedule the
     * next CQI update.  Invoked periodically every `CqiPeriod`.
     *
     * @param destination the destination L2 ID
     */
    void UpdateFilteredSinr(L2Id destination);

    /**
     * Compute the max over the per-destination raw-sample ring buffer
     * and store the result in m_filteredSinrTraces as the current CQI
     * estimate.  The buffer must be non-empty.
     *
     * @param destination the destination L2 ID
     */
    void EmitFilteredSinr(L2Id destination);

    /**
     * Calculate the max of the values in the buffer.  Used as the
     * receiver-side filter operator: the cleanest recent sample drives
     * the CQI estimate, suppressing collision-corrupted samples that
     * would otherwise pull the estimate down.
     *
     * @param buffer non-empty deque of SINR values
     * @return the maximum value
     */
    static double Max(const std::deque<double>& buffer);

    /**
     * Build a SlotInfo from the MAC pool configuration.
     *
     * @return the SlotInfo for the transmitter pool
     */
    NrSlCommResourcePool::SlotInfo CreateSlotInfo() const;

    std::unordered_map<L2Id, SinrTraceEntry> m_sinrTraces;
    std::unordered_map<L2Id, FilteredSinrEntry>
        m_filteredSinrTraces; ///< CQI estimate (max over raw samples, periodically gated)
    std::unordered_map<L2Id, std::deque<double>>
        m_sinrBuffer; ///< ring buffer of recent raw SINR samples per destination
    std::unordered_map<L2Id, SinrDeltaEntry> m_sinrDeltas;
    std::unordered_map<L2Id, std::vector<EventId>> m_saveSinrEvents; ///< pending SaveSinr events
    std::unordered_map<L2Id, EventId> m_cqiUpdateEvents;             ///< pending CQI update events

    Ptr<NrSlErrorModel> m_slErrorModel;
    Ptr<RandomVariableStream> m_rxPsschDelay; ///< `RxPsschDelay` attribute
    Ptr<RandomVariableStream> m_cqiPeriod;    ///< `CqiPeriod` attribute
    L2Id m_srcL2Id{};                         ///< source L2 id;
    L2Id m_maximumL2Id{};                     ///< maximum L2 id value to control;
    /// `TargetBler` attribute,
    /// Initialized to `1.0` to avoid violating
    /// the precondition of `m_targetBler != 0`
    /// for the `PenaltyStep` attribute
    double m_targetBler{1.0};
    double m_nackPenaltyStep;    /// `PenaltyStep` attribute
    double m_ackStep;            /// derived from the `PenaltyStep` attribute
    double m_minSinrOffsetDelta; ///< `MinSinrDelta` attribute (a clamping value)
    double m_maxSinrOffsetDelta; ///< `MaxSinrDelta` attribute (a clamping value)
    bool m_enableClamping;       ///< `EnableClamping` attribute
    uint32_t m_medianFilterSize; ///< `MedianFilterSize` attribute (ring-buffer length)
    bool m_harqAwareTarget;      ///< `HarqAwareTarget` attribute
    bool m_dynamicNumTx;         ///< `DynamicNumTx` attribute
    /// When true, each predicted effective BLER composes per-attempt
    /// SCI-2a decoding with TB decoding; when false, predictions use TB
    /// decoding alone.  Not bound to an attribute; edit the initializer
    /// to change the behavior.
    bool m_useSci2aComposition{true};

    /**
     * TracedCallback signature for the outcome of a HARQ feedback trial.
     *
     * @param mcs the MCS used for the transmission
     * @param dstL2Id the destination L2 ID
     * @param success true if the transmission was received successfully
     */
    typedef void (*TrialResultCallback)(uint8_t mcs, uint32_t dstL2Id, bool success);

    /**
     * TracedCallback signature for the SINR estimate used for MCS selection.
     *
     * Called in DoGetGrantParams() with the unfiltered and filtered channel SINR,
     * the cumulative OLLA offset, and the combined SINR estimate
     * (filteredSinrDb + sinrOffsetDb).
     *
     * @param dstL2Id the destination L2 ID
     * @param unfilteredSinrDb the unfiltered (latest observed) channel SINR in dB
     * @param filteredSinrDb the filtered (CQI-gated) channel SINR in dB
     * @param sinrOffsetDb the OLLA offset Delta in dB
     * @param sinrEstimateDb the SINR estimate in dB (filteredSinrDb + sinrOffsetDb)
     */
    TracedCallback<uint32_t, double, double, double, double> m_sinrEstimateTrace;

    TracedCallback<uint8_t, uint32_t, bool> m_trialResultTrace; ///< Trace for trial results
};

} // namespace ns3

#endif /* NR_SL_OLLA_MCS_CONTROLLER_H */
