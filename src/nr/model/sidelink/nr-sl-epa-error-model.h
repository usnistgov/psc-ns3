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
 * This error model is based on the NIST sidelink simulator results for
 * an 'Extended Pedestrian A' channel in band n14 (center frequency 793 MHz).
 * It is based on link simulations of 40000 transmission for each MCS and
 * number of transmissions (1 through 5) for a transport block.  The number of
 * transport blocks simulated depends on the number of transmissions
 * configured per transport block; e.g., if two transmissions are scheduled
 * for each transport block, the curve for that outcome is based on
 * 20,000 transport blocks (with two transmissions each).
 *
 * The simulations were performed using a transport block size of XX
 * bits, corresponding to one subchannel at a subcarrier spacing of 15
 * kHz.
 *
 * Each transport block error rate (TBLER) curve consists of roughly
 * twenty (SNR, TBLER) data points.  In the ns-3 simulation, the SNR
 * will typically not align exactly with the data points, so this model
 * will use linear interpolation between the relevant data points to
 * return an interpolated TBLER.
 *
 * Some data points stored may be for TBLER rates that are too low to
 * be statistically significant.  For example, a TBLER rate of 1e-6 cannot
 * be estimated by 40,000 transmissions.  This model therefore defines
 * a Tolerance attribute, below which the curve data will not be directly
 * used.  Instead, the last two data points above the tolerance limit
 * will be used to linearly extrapolate for BLER values below the limit.
 *
 * This error model is insensitive to numerology (which primarily affects
 * fading channels).
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
    Ptr<const SpectrumModel> m_spectrumModel; //!< SpectrumModel used by the Phy
    double m_threshold{0};                    //!< Threshold cutoff value for valid BLER
    double m_extrapolationLimit{0};           //!< Extrapolation cutoff value in dB
};

} // namespace ns3

#endif // NR_SL_EPA_ERROR_MODEL_H
