//
// SPDX-License-Identifier: NIST-Software

#ifndef NR_SL_EPA_ERROR_MODEL_H
#define NR_SL_EPA_ERROR_MODEL_H

#include "nr-sl-error-model.h"

#include <ns3/object.h>

namespace ns3
{

/**
 * \ingroup error-models
 * \brief Extended Pedestrian A (EPA) error model
 *
 * This error model is based on NIST sidelink link-level simulation results
 * for an 'Extended Pedestrian A' channel in band n14 (center frequency
 * 793 MHz).  Reference curves exist for each MCS index (0 through 28),
 * number of transmissions of a transport block (1 through 5), subcarrier
 * spacing (15 kHz and 30 kHz), and two resource block allocations (10 and
 * 50 RBs at 15 kHz spacing; 10 and 20 RBs at 30 kHz spacing).  Each
 * curve's underlying raw data points are each based on simulations of
 * 40,000 transport blocks to estimate the block error rate.
 *
 * For transport block decoding, each reference curve is represented by an
 * erfc function, TBLER = 0.5 * erfc((SNR - mu) / (sigma * sqrt(2))), whose
 * (mu, sigma) parameters were fit offline to the reference data over the
 * TBLER window [0.01, 1.0].  Each curve is fit independently, with no
 * cross-MCS smoothing or ordering correction, so the model reproduces
 * measured TBLER inversions at modulation boundaries; callers must not
 * assume that TBLER is non-decreasing with MCS index at a given SNR.
 * TBLER for resource block allocations between the two reference
 * allocations is obtained by logarithmic interpolation, and allocations
 * beyond them by power-law extrapolation.  Computed TBLER values below the
 * ExtrapolationLimit attribute are treated as zero.
 *
 * For retransmissions, the SNR values of the current and prior
 * transmissions are averaged in the linear domain, and the curve for the
 * corresponding number of transmissions is evaluated at the mean (see the
 * error model documentation for this SNR combining rationale).  The
 * reference curves are unconditional (joint) error probabilities over all
 * transmissions of a transport block, while GetTbler() must return the
 * probability of failure conditioned on the failed attempts in the
 * history; the conditional value is formed as the quotient of the curves
 * for the current and previous numbers of transmissions, each evaluated
 * at its own combined SNR.  The reference data covers up to five
 * transmissions; the five-transmission curve is reused beyond that, so
 * additional attempts at unchanged SNR provide no further combining gain.
 *
 * SCI decoding uses linear interpolation between the tabulated SCI-2
 * reference data points.  Tabulated BLER values below the Threshold
 * attribute are not statistically significant with 40,000 transport
 * blocks and are not used directly; instead, the last two data points
 * above the threshold are used to extrapolate down to the
 * ExtrapolationLimit.
 *
 * The reference data is stored in CSV files under
 * model/sidelink/error-model-data/epa/, and the generated tables and erfc
 * parameters in the implementation file can be regenerated with
 * utils/sidelink/generate-error-model-data.py.
 */
class NrSlEpaErrorModel : public NrSlErrorModel
{
  public:
    /**
     * \brief GetTypeId
     * \return The TypeId of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSlEpaErrorModel default constructor
     */
    NrSlEpaErrorModel();

    // Documented in nr-sl-error-model.h
    void SetSpectrumModel(Ptr<const SpectrumModel> model) override;
    double GetSci1ErrorRate(uint8_t mcs, uint8_t numerology, double snrDb) const override;
    double GetSci2ErrorRate(uint8_t mcs, uint8_t numerology, double snrDb) const override;
    double GetTbler(uint8_t mcs,
                    uint8_t numerology,
                    uint8_t rv,
                    double snrDb,
                    uint32_t tbSize,
                    uint32_t numRbs,
                    const NrErrorModel::NrErrorModelHistory& history) const override;

  private:
    /**
     * \brief Look up the unconditional TBLER for a number of transmissions
     *
     * Returns the unconditional (joint) probability that all numTx
     * decoding attempts fail, by evaluating the erfc curves for numTx
     * transmissions at the combined SNR for both RB anchors and
     * interpolating to the requested RB allocation.  The reference data
     * covers up to five transmissions; the five-transmission curves are
     * reused beyond that.
     *
     * \param mcs MCS index
     * \param numerology Numerology (0 or 1)
     * \param numTx The number of transmissions (at least 1)
     * \param snrDb The combined SNR in dB over the numTx transmissions
     * \param numRbs The number of resource blocks
     * \return The unconditional TBLER value
     */
    double GetTblerForNumTx(uint8_t mcs,
                            uint8_t numerology,
                            size_t numTx,
                            double snrDb,
                            uint32_t numRbs) const;

    /**
     * \brief Compute TBLER from the erfc curve model
     *
     * Evaluates TBLER = 0.5 * erfc((SNR - mu) / (sigma * sqrt(2))), where
     * (mu, sigma) were fit offline to the reference data over the TBLER
     * window [0.01, 1.0].
     *
     * \param snrDb SNR in dB
     * \param mu erfc midpoint parameter (dB)
     * \param sigma erfc spread parameter (dB)
     * \return The TBLER value
     */
    double InterpolateWithErfc(double snrDb, double mu, double sigma) const;

    /**
     * \brief Calculate TBLER for a specific MCS from the erfc curve model
     *
     * \note  Callers must not assume that TBLER is non-decreasing with MCS index
     * at a given SNR.
     *
     * \param snrDb SNR in dB
     * \param mcs MCS index
     * \param erfcParams pointer to erfc parameter array indexed by MCS
     * \return The TBLER value
     */
    double GetTblerForMcs(double snrDb, uint8_t mcs, const double (*erfcParams)[2]) const;

    /**
     * \brief Interpolate or extrapolate TBLER based on number of resource blocks
     *
     * This method uses logarithmic interpolation between TBLER values for
     * different RB allocations. For extrapolation beyond available data,
     * power-law extrapolation is used when the single-subchannel TBLER is
     * below the reliability threshold.
     *
     * The reference points are from NIST link simulations:
     * - 10 RBs (1 subchannel)
     * - 50 RBs (5 subchannels) for numerology 0
     * - 20 RBs (2 subchannels) for numerology 1
     *
     * \param tblerOneSubch TBLER for single subchannel (10 RBs)
     * \param tblerAllSubch TBLER for all subchannels (50 RBs for num=0, 20 RBs for num=1)
     * \param numRbs The number of resource blocks
     * \param numerology The numerology (0 or 1)
     * \return The interpolated/extrapolated TBLER value
     */
    double InterpolateTblerByRbs(double tblerOneSubch,
                                 double tblerAllSubch,
                                 uint32_t numRbs,
                                 uint8_t numerology) const;

    Ptr<const SpectrumModel> m_spectrumModel; //!< SpectrumModel used by the Phy
    double m_threshold{0};                    //!< Threshold cutoff value for valid BLER
    double m_extrapolationLimit{0};           //!< Extrapolation cutoff value in dB
};

} // namespace ns3

#endif // NR_SL_EPA_ERROR_MODEL_H
