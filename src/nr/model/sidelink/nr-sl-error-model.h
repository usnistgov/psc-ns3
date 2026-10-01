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
     * \brief Calculate the transport block error rate
     *
     * \param mcs MCS
     * \param numerology Numerology
     * \param rv The redundancy version
     * \param sinrDb The SINR in dB
     * \param tbSize The TB size in bytes
     * \param numRBs The number of RBs
     * \param history History of the previous transmissions
     * \return The probability (range [0,1]) that the TB was in error
     */
    virtual double GetTbler(uint8_t mcs,
                            uint8_t numerology,
                            uint8_t rv,
                            double sinrDb,
                            uint32_t tbSize,
                            uint32_t numRbs,
                            const NrErrorModel::NrErrorModelHistory& history) const = 0;

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
};

} // namespace ns3
#endif // NR_SL_ERROR_MODEL_H
