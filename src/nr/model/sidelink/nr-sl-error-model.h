//
// SPDX-License-Identifier: NIST-Software

#ifndef NR_SL_ERROR_MODEL_H
#define NR_SL_ERROR_MODEL_H

#include "ns3/nr-error-model.h"
#include <ns3/object.h>
#include <ns3/spectrum-model.h>
#include <ns3/spectrum-value.h>

#include <span>
#include <vector>

namespace ns3
{

/**
 * \brief The NrSlErrorModelOutput struct
 * Error model output returned by the class NrSlErrorModel
 * \see NrSlErrorModel
 */
struct NrSlErrorModelOutput : public NrErrorModelOutput
{
    /**
     * \brief NrSlErrorModelOutput default constructor (deleted)
     */
    NrSlErrorModelOutput() = delete;

    /**
     * \brief NrSlErrorModelOutput constructor with arguments
     * \param tbler the TBLER
     * \param snrDb the SNR in dB for the transmission
     * \param rv the redundancy vector for the transmission
     */
    NrSlErrorModelOutput(double tbler, double snrDb, uint8_t rv)
        : NrErrorModelOutput(tbler),
          m_snrDb(snrDb),
          m_rv(rv)
    {
    }

    double m_snrDb; //!< Average SNR in dB
    uint8_t m_rv;   //!< Redundancy vector
};

/**
 * \ingroup error-models
 * \brief Interface to error models for sidelink simulations
 */
class NrSlErrorModel : public Object
{
  public:
    /**
     * \brief GetTypeId
     * \return The TypeId of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSlErrorModel default constructor
     */
    NrSlErrorModel();

    /**
     * \brief Set the SpectrumModel
     *
     * For use to check that the error model is applicable to the frequency
     * band being used by the PHY.
     *
     * \param model The SpectrumModel used by the Phy
     */
    virtual void SetSpectrumModel(Ptr<const SpectrumModel> model) = 0;

    /**
     * \brief Calculate the SCI-1 error rate
     *
     * \param mcs MCS
     * \param numerology Numerology
     * \param sinrDb The SINR in dB
     * \return The probability (range [0,1]) that the SCI-1 was in error
     */
    virtual double GetSci1ErrorRate(uint8_t mcs, uint8_t numerology, double sinrDb) const = 0;

    /**
     * \brief Calculate the SCI-2 error rate
     *
     * \param mcs MCS
     * \param numerology Numerology
     * \param sinrDb The SINR in dB
     * \return The probability (range [0,1]) that the SCI-2 was in error
     */
    virtual double GetSci2ErrorRate(uint8_t mcs, uint8_t numerology, double sinrDb) const = 0;

    /**
     * \brief Calculate the transport block error rate for the current
     *        decoding attempt, conditioned on the failed attempts in the
     *        history
     *
     * The PHY only attempts to decode a transport block whose previous
     * transmissions all failed to decode, and it draws an independent
     * uniform random variable against the returned value for each attempt.
     * The returned value must therefore be the conditional probability
     * that the current decoding attempt fails given that every attempt
     * recorded in the history failed; i.e., for the n-th transmission,
     * Pr(En | E1, ..., En-1), where Ei is the event that the i-th
     * decoding attempt fails.  The product of the values returned across
     * successive attempts then reproduces the unconditional probability
     * Pr(E1, ..., En) that all n attempts fail.
     *
     * Implementations whose reference data provides unconditional
     * (joint) error probabilities must form this conditional probability
     * as the quotient Pr(E1, ..., En) / Pr(E1, ..., En-1); see
     * ComputeConditionalTbler().
     *
     * \param mcs MCS
     * \param numerology Numerology
     * \param rv The redundancy version
     * \param sinrDb The SINR in dB
     * \param tbSize The TB size in bytes
     * \param numRBs The number of RBs
     * \param history History of the previous (failed) transmissions
     * \return The conditional probability (range [0,1]) that the TB was in error
     */
    virtual double GetTbler(uint8_t mcs,
                            uint8_t numerology,
                            uint8_t rv,
                            double sinrDb,
                            uint32_t tbSize,
                            uint32_t numRbs,
                            const NrErrorModel::NrErrorModelHistory& history) const = 0;

    /**
     * \brief Calculate the unconditional transport block error rate after
     *        a number of transmissions at equal SNR
     *
     * Returns the unconditional (joint) probability Pr(E1, ..., En) that
     * none of numTx decoding attempts succeeds, with all transmissions
     * received at the given SNR.  This is the quantity measured by
     * link-level simulation reference curves, in contrast to the
     * per-attempt conditional probability returned by GetTbler().  It is
     * intended for model validation, curve plotting, and offline analysis.
     *
     * The value is computed by composing GetTbler(): the conditional
     * error probabilities of numTx successive attempts (using the
     * standard redundancy version sequence 0, 2, 3, 1) are multiplied
     * together, which telescopes to the joint probability.  It therefore
     * works for any subclass, whether its reference data is unconditional
     * or conditional.
     *
     * \param mcs MCS
     * \param numerology Numerology
     * \param numTx The number of transmissions (at least 1)
     * \param sinrDb The SINR in dB of each transmission
     * \param tbSize The TB size in bytes
     * \param numRBs The number of RBs
     * \return The probability (range [0,1]) that all numTx attempts fail
     */
    double GetUnconditionalTbler(uint8_t mcs,
                                 uint8_t numerology,
                                 size_t numTx,
                                 double sinrDb,
                                 uint32_t tbSize,
                                 uint32_t numRbs) const;

  protected:
    /**
     * \brief Calculate the transport block error rate by interpolating
     *        or extrapolating as required
     *
     * This method performs linear interpolation on the curve represented by
     * the snrVector and blerVector, by using the input SNR in dB units.
     * If an optional threshold value is passed in, the method will not
     * perform linear interpolation that results in a BLER value below the
     * threshold.  Instead, in this case, the last two data points above the
     * BLER threshold are used for a linear extrapolation below the threshold.
     * The linear extrapolation is truncated by the extrapolationLimit BLER
     * value, below which the BLER value returned is zero.
     *
     * \param snrDb The SNR in dB for the current transmission
     * \param snrVector The vector of SNR values for the curve
     * \param blerVector The vector of TBLER values for the curve
     * \param threshold The BLER threshold (linear value) above which the curves can be interpolated
     * as needed \param extrapolationLimit Below this BLER value, return zero (truncate the
     * extrapolation)
     */
    double Interpolate(double snrDb,
                       std::span<const double> snrVector,
                       std::span<const double> blerVector,
                       double threshold = 0,
                       double extrapolationLimit = 0) const;
    /**
     * \brief Return the average of the SNR values passed in the first argument
     *        and the error model history.
     *
     * The input SNR value and the values stored in the history are converted
     * to linear values, and the arithmetic mean is calculated, and then the
     * result is converted back to decibel units and returned.  If the history
     * is empty, the method returns the value passed in the first argument.
     *
     * \param snrDb The SNR in dB for the current transmission
     * \param history The error model history
     */
    double GetAverageSnr(double snrDb, const NrErrorModel::NrErrorModelHistory& history) const;
    /**
     * \brief Return the average of the SNR values stored in the error
     *        model history
     *
     * Behaves as the two-argument GetAverageSnr() but averages only the
     * values stored in the history, which must not be empty.  This is
     * the combined SNR of the previous transmissions, used to evaluate
     * the reference curve for the previous decoding attempt.
     *
     * \param history The error model history (must not be empty)
     */
    double GetAverageSnr(const NrErrorModel::NrErrorModelHistory& history) const;
    /**
     * \brief Form a conditional error probability from two unconditional
     *        (joint) error probabilities
     *
     * Given the joint probability that the current and all previous
     * decoding attempts fail, Pr(E1, ..., En), and the joint probability
     * that the previous attempts fail, Pr(E1, ..., En-1), returns the
     * conditional probability Pr(En | E1, ..., En-1) as their quotient.
     * The quotient is clamped to at most 1, since independently measured
     * or fitted reference curves for adjacent numbers of transmissions
     * may cross slightly (typically where both are near 1).  If the
     * prior joint probability is zero (a degenerate history, since an
     * attempt with zero modeled error probability cannot have failed),
     * the joint probability is returned unmodified.
     *
     * \param jointTbler Pr(E1, ..., En), evaluated at the combined SNR of all n transmissions
     * \param priorJointTbler Pr(E1, ..., En-1), evaluated at the combined SNR of the previous n-1
     * transmissions
     * \return The conditional probability (range [0,1]) that the current attempt fails
     */
    double ComputeConditionalTbler(double jointTbler, double priorJointTbler) const;
};

} // namespace ns3
#endif // NR_SL_ERROR_MODEL_H
