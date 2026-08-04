//
// SPDX-License-Identifier: NIST-Software

#ifndef NR_SL_IDEAL_MCS_CONTROLLER_H
#define NR_SL_IDEAL_MCS_CONTROLLER_H

#include "nr-sl-comm-resource-pool.h"
#include "nr-sl-error-model.h"
#include "nr-sl-mcs-controller.h"
#include "nr-sl-phy-mac-common.h"

#include <ns3/object.h>
#include <ns3/traced-callback.h>

#include <limits>
#include <optional>
#include <string>
#include <unordered_map>

namespace ns3
{

/**
 * \ingroup scheduler
 *
 * An ideal MCS controller that learns the most recent SNR of a previous transmission and,
 * if necessary, converts it to an equivalent wideband SNR (for the whole pool bandwidth). It
 * uses the wideband SNR and the currently configured error model to select the highest MCS
 * that will provide a BLER  greater than a target BLER.  It can also be configured with a margin
 * that will apply an additional offset to the SNR value provided to the error model.
 * In the absence of recent information, MCS 0 will be used.
 *
 * To suppress adjustment of broadcast or groupcast L2 IDs, this model uses a MaximumL2Id
 * attribute to indicate the maximum value of L2 ID that will be controlled by this controller.
 * For values greater than this maximum, this controller will not attempt any MCS changes.
 * The value is a 32 bit unsigned integer initialized to the value 200.
 */
class NrSlIdealMcsController : public NrSlMcsController
{
  public:
    /**
     * \brief Get the type id
     * \return the type id of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSlIdealMcsController constructor
     */
    NrSlIdealMcsController();

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
     * Callback sink to receive success or failure indications about HARQ
     * processes.
     *
     * @param slHarqInfo The HARQ information
     */
    void NotifyHarqFeedbackReceived(const SlHarqInfo& slHarqInfo);

    /**
     * TracedCallback signature for trial results.
     *
     * @param mcs the MCS of the trial
     * @param dstL2Id the destination L2 ID
     * @param success whether the trial was successful or not
     */
    typedef void (*TrialResultCallback)(uint8_t mcs, uint32_t dstL2Id, bool success);

    /**
     * Use binary search to find the highest MCS that provides a BLER less than or
     * equal to the target BLER.  Returns std::nullopt if no MCS meets the target.
     *
     * This is a static utility method that can be used independently of an
     * NrSlIdealMcsController instance; for example, in unit tests that need
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

  protected:
    // override base class methods
    void DoDispose() override;

  private:
    // override base class methods
    void DoSetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler) override;
    GrantParams DoGetGrantParams(uint32_t dstL2Id,
                                 SidelinkInfo::CastType castType,
                                 std::optional<uint32_t> numRbs,
                                 std::optional<uint32_t> tbSize) override;
    GrantParams DoGetLargestFeasibleTb(uint32_t dstL2Id,
                                       SidelinkInfo::CastType castType,
                                       uint16_t maxSubCh) override;

    void NotifyRxPssch(std::string context, SlRxDataPacketTraceParams traceParams);
    void NotifyRxPscch(std::string context, SlRxCtrlPacketTraceParams traceParams);
    void ConfigureTraces();

    /**
     * Build a SlotInfo struct from the current MAC and pool configuration.
     *
     * @return a SlotInfo populated from the MAC's active pool
     */
    NrSlCommResourcePool::SlotInfo CreateSlotInfo() const;

    /**
     * Information recorded every time
     * `NotifyRxPssch()` is triggered
     */
    struct TraceEntry
    {
        Time time;
        double sinrDb;    //!< Sub-band SINR in dB (no wideband correction applied)
        uint32_t rbsUsed; //!< Number of RBs used in the observed transmission
    };

    Ptr<const NrSlErrorModel> m_slErrorModel;
    uint32_t m_srcL2Id{std::numeric_limits<uint32_t>::max()}; //!< source L2 id
    uint16_t m_srcRnti{std::numeric_limits<uint16_t>::max()}; //!< source RNTI
    uint32_t m_maximumL2Id{
        std::numeric_limits<uint32_t>::max()}; //!< maximum L2 id value to control;
    std::unordered_map<uint32_t, TraceEntry> m_traceMap;
    double m_tblerTarget;   // `TargetBler` attribute
    double m_sinrMargin;    // `Margin` attribute
    bool m_harqAwareTarget; // `HarqAwareTarget` attribute
    bool m_dynamicNumTx;    // `DynamicNumTx` attribute
    /// When true, each predicted effective BLER composes per-attempt
    /// SCI-2a decoding with TB decoding; when false, predictions use TB
    /// decoding alone.  Not bound to an attribute; edit the initializer
    /// to change the behavior.
    bool m_useSci2aComposition{true};
    TracedCallback<uint8_t, uint32_t, bool> m_trialResultTrace; //!< Trace for trial results
};

} // namespace ns3

#endif /* NR_SL_IDEAL_MCS_CONTROLLER_H */
