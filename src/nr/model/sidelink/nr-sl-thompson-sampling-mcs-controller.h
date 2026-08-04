// Copyright (c) 2021 IITP RAS (adapted from Wi-Fi Thompson Sampling implementation)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#ifndef NR_SL_THOMPSON_SAMPLING_MCS_CONTROLLER_H
#define NR_SL_THOMPSON_SAMPLING_MCS_CONTROLLER_H

#include "nr-sl-mcs-controller.h"

#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/random-variable-stream.h>

#include <limits>
#include <map>
#include <unordered_map>
#include <vector>

namespace ns3
{

/**
 * \ingroup scheduler
 *
 * A Thompson Sampling MCS controller for NR Sidelink.  This controller
 * adaptively selects the MCS for each unicast destination, in the context
 * of supporting sidelink mode 2 scheduling.  The controller maintains per-arm
 * success/failure statistics and uses Thompson Sampling to balance
 * exploration and exploitation when selecting MCS values.
 *
 * This implementation is adapted from the Wi-Fi Thompson Sampling rate
 * control algorithm (ThompsonSamplingWifiManager) for the NR Sidelink
 * context, where feedback is provided via HARQ ACK/NACK rather than
 * immediate frame acknowledgments.
 *
 * When GetGrantParams() is called, for each MCS arm, a success rate is sampled
 * from the Beta posterior.  The highest MCS whose sampled success rate
 * meets or exceeds the target success rate (1 - TargetBler) is selected.
 * If no arm qualifies, MCS 0 is returned.
 *
 * To suppress adjustment of broadcast or groupcast L2 IDs, this model
 * uses a MaximumL2Id attribute to indicate the maximum value of L2 ID
 * that will be controlled by this controller.
 */
class NrSlThompsonSamplingMcsController : public NrSlMcsController
{
  public:
    /**
     * \brief Get the type id
     * \return the type id of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSlThompsonSamplingMcsController constructor
     */
    NrSlThompsonSamplingMcsController();

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
     * Per-arm (per-MCS) statistics for the Thompson Sampling posterior
     */
    struct ArmStats
    {
        double success{0.0};        //!< Exponentially-weighted success count
        double fails{0.0};          //!< Exponentially-weighted failure count
        Time lastDecay{Seconds(0)}; //!< Last time decay was applied
    };

    /**
     * Callback sink to receive success or failure indications about HARQ
     * processes.
     *
     * \param slHarqInfo The HARQ information
     */
    void NotifyHarqFeedbackReceived(const SlHarqInfo& slHarqInfo);

    /**
     * Record the outcome of a transmission for the given destination and MCS.
     * Updates the Beta posterior for the corresponding arm.
     *
     * \param dstL2Id the destination L2 ID
     * \param mcs the MCS value that was used
     * \param success true if the transport block was decoded successfully
     */
    void RecordOutcome(uint32_t dstL2Id, uint8_t mcs, bool success);

    /**
     * Remove all arm statistics for a given destination.
     *
     * \param dstL2Id the destination L2 ID
     */
    void Flush(uint32_t dstL2Id);

    /**
     * TracedCallback signature for arm trial results.
     *
     * @param mcs the MCS of the ARM
     * @param dstL2Id the destination L2 ID
     * @param success Whether the trial was successful or not
     */
    typedef void (*TrialResultCallback)(uint8_t mcs, uint32_t dstL2Id, bool success);

  protected:
    void DoDispose() override;

  private:
    void DoSetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler) override;
    GrantParams DoGetGrantParams(uint32_t dstL2Id,
                                 SidelinkInfo::CastType castType,
                                 std::optional<uint32_t> numRbs,
                                 std::optional<uint32_t> tbSize) override;

    /**
     * Sample from a Beta variable that can be derived from two Gamma RVs:
     * if X ~ Gamma(alpha, 1) and Y ~ Gamma(beta, 1), then X/(X+Y) ~ Beta(alpha, beta).
     *
     * \param alpha the alpha parameter of the Beta distribution
     * \param beta the beta parameter of the Beta distribution
     * \return a sample from Beta(alpha, beta)
     */
    double SampleBeta(double alpha, double beta) const;

    /**
     * Apply geometric decay (multiplicatively, per-observation) to an arm's statistics.
     *
     * No-op when m_discountFactor is 1
     *
     * \param stats the arm statistics to decay
     * \param mcs the MCS value corresponding to the arm
     */
    void Decay(ArmStats& stats, uint8_t mcs);

    /**
     * Apply time-based decay to all arms for a destination.  For each arm,
     * the number of missed decay intervals since the arm was last decayed
     * is computed, and the discount factor is applied that many times.
     * This ensures that arms not visited by per-observation decay in
     * RecordOutcome() still have their stale evidence discounted.
     *
     * @param dstL2Id the destination L2 ID
     * @param arms the arm statistics map for the destination
     */
    void TimeBasedDecay(uint32_t dstL2Id, std::map<uint8_t, ArmStats>& arms);

    /**
     * Creates an intial set of arms to use for recording outcomes.
     *
     * \returns The set to use to record outcomes
     */
    std::map<uint8_t, ArmStats> CreateArms();

    static const std::vector<uint8_t> DEFAULT_MCS_INDEX; //!< Default MCS index (MCS 0-28)

    Ptr<GammaRandomVariable> m_gammaRandomVariable; //!< For sampling Beta-distributed RVs
    double m_discountFactor;                        //!< Geometric decay coefficient for arms
    double m_targetBler;                            //!< Target BLER for MCS selection
    uint32_t m_maximumL2Id{
        std::numeric_limits<uint32_t>::max()}; //!< Maximum L2 ID value to control
    std::unordered_map<uint32_t, std::map<uint8_t, ArmStats>>
        m_armStats;                  //!< Per-destination arm statistics
    bool m_propagateOutcomes;        //!< Indicates if outcomes apply to more than one arm
    std::vector<uint8_t> m_mcsIndex; //!< The index of MCS values / arms to use.
    Time m_decayInterval;            //!< Time interval for time-based catch-up decay
    TracedCallback<uint8_t, uint32_t, bool>
        m_trialResultTrace; //!< Trace source for MCS selection events
};

} // namespace ns3

#endif /* NR_SL_THOMPSON_SAMPLING_MCS_CONTROLLER_H */
