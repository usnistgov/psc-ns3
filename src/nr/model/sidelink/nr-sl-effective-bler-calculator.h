//
// SPDX-License-Identifier: NIST-Software

#ifndef NR_SL_EFFECTIVE_BLER_CALCULATOR_H
#define NR_SL_EFFECTIVE_BLER_CALCULATOR_H

#include <array>
#include <cstdint>
#include <vector>

namespace ns3
{

/**
 * @ingroup error-models
 * @brief Accumulate per-attempt error probabilities for a sidelink HARQ
 *        chain and report the predicted effective BLER
 *
 * MCS controllers predict the outcome of a transport block delivered over
 * one or more transmissions whose failed attempts are soft-combined at the
 * receiver.  This class folds such a prediction one attempt at a time.
 * Each attempt contributes:
 *
 * - the conditional TB decoding failure probability returned by
 *   NrSlErrorModel::GetTbler() for that attempt, conditioned on all prior
 *   attempts having failed; and
 * - the SCI-2a decoding failure probability for that attempt.
 *
 * GetEffectiveBler() supports two effective BLER definitions:
 *
 * - TB-only (includeSci2a false): the product of the conditional values,
 *   which telescopes to the joint probability that every attempt fails to
 *   decode.
 * - Composed (includeSci2a true): additionally models per-attempt SCI-2a
 *   decoding.  Each attempt's SCI-2a is decoded independently; if it
 *   fails, the receiver cannot associate the transmission and the attempt
 *   contributes nothing to soft combining.  The calculator maintains a
 *   probability distribution over states (number of received attempts,
 *   whether a received attempt carried a self-decodable redundancy
 *   version), and the effective BLER is the expectation of the TB failure
 *   probability over that distribution.  A set of received attempts whose
 *   redundancy versions include neither RV 0 nor RV 3 carries no
 *   systematic bits and contributes failure probability 1.
 *
 * All attempts are assumed to be received at the same SINR, so the joint
 * TB failure probability for m received attempts equals the product of
 * the first m conditional values regardless of which attempts were
 * received.
 */
class NrSlEffectiveBlerCalculator
{
  public:
    /**
     * Fold one transmission attempt into the prediction.
     *
     * @param conditionalTbler The conditional probability that this
     *        attempt's TB decode fails given that all prior attempts
     *        failed (NrSlErrorModel::GetTbler() with the corresponding
     *        history)
     * @param sci2aBler The probability that this attempt's SCI-2a fails
     *        to decode (pass 0 when SCI-2a is not modeled)
     * @param rv The redundancy version of this attempt
     */
    void AddAttempt(double conditionalTbler, double sci2aBler, uint8_t rv);

    /**
     * Return the predicted effective BLER over the attempts added so far.
     *
     * @param includeSci2a Whether to include per-attempt SCI-2a decoding
     *        in the prediction
     * @return The predicted effective BLER (range [0, 1]); 1 if no
     *         attempts have been added
     */
    double GetEffectiveBler(bool includeSci2a) const;

  private:
    /**
     * Probability of each (received count, self-decodable flag) state.
     * m_stateProb[m][0] is the probability that m attempts were received
     * and none carried RV 0 or RV 3; m_stateProb[m][1] is the probability
     * that m attempts were received and at least one carried RV 0 or
     * RV 3.
     */
    std::vector<std::array<double, 2>> m_stateProb{{1.0, 0.0}};
    /**
     * Joint TB failure probability by number of received attempts:
     * m_jointTbler[m] is the product of the first m conditional values
     * (m_jointTbler[0] is 1).
     */
    std::vector<double> m_jointTbler{1.0};
};

} // namespace ns3
#endif // NR_SL_EFFECTIVE_BLER_CALCULATOR_H
