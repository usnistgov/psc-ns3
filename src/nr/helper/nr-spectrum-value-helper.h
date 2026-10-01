// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SPECTRUM_VALUE_HELPER_H
#define NR_SPECTRUM_VALUE_HELPER_H

#include <ns3/spectrum-value.h>

#include <vector>

namespace ns3
{

/**
 * \ingroup spectrum
 *
 * \brief This class provides a set of useful functions when working with spectrum model for NR
 */
class NrSpectrumValueHelper
{
  public:
    enum PowerAllocationType
    {
        UNIFORM_POWER_ALLOCATION_USED,
        UNIFORM_POWER_ALLOCATION_BW
    };

    static const uint8_t SUBCARRIERS_PER_RB = 12; //!< subcarriers per resource block

    /**
     * \brief Creates or obtains from a global map a spectrum model with a given number of RBs,
     * center frequency and subcarrier spacing.
     * \param numRbs bandwidth in number of RBs
     * \param centerFrequency the center frequency of this band
     * \return pointer to a spectrum model with defined characteristics
     */
    static Ptr<const SpectrumModel> GetSpectrumModel(uint32_t numRbs,
                                                     double centerFrequency,
                                                     double subcarrierSpacing);

    /**
     * \brief Create SpectrumValue that will represent transmit power spectral density,
     * and assuming that all RBs are active.
     * \param powerTx total power in dBm
     * \param rbIndexVector the list of active/used RBs for the current transmission
     * \param txSm spectrumModel to be used to create this SpectrumValue
     * \param allocationType power allocation type to be used
     * \return spectrum value representing power spectral density for given parameters
     */
    static Ptr<SpectrumValue> CreateTxPowerSpectralDensity(double powerTx,
                                                           const std::vector<int>& rbIndexVector,
                                                           const Ptr<const SpectrumModel>& txSm,
                                                           enum PowerAllocationType allocationType);

    /**
     * \brief Create a SpectrumValue that models the power spectral density of AWGN
     * \param noiseFigure the noise figure in dB  w.r.t. a reference temperature of 290K
     * \param spectrumModel the SpectrumModel instance to be used to create the output spectrum
     * value \return a pointer to a newly allocated SpectrumValue representing the noise Power
     * Spectral Density in W/Hz for each Resource Block
     */
    static Ptr<SpectrumValue> CreateNoisePowerSpectralDensity(
        double noiseFigure,
        const Ptr<const SpectrumModel>& spectrumModel);

    /**
     * \brief Returns the effective bandwidth for the total system bandwidth
     * \param bandwidth the total system bandwidth in Hz
     * \param numerology the numerology to be used over the whole bandwidth
     * \return effective bandwidth which is the sum of bandwidths of all sub-bands, in Hz
     */
    static uint64_t GetEffectiveBandwidth(double bandwidth, uint8_t numerology);

  protected:
    /**
     * \brief Create SpectrumValue that will represent transmit power spectral density, and
     * the total transmit power will be uniformly distributed only over active RBs
     * \param powerTx total power in dBm
     * \param activeRbs vector of RBs that are active for this transmission
     * \param spectrumModel spectrumModel to be used to create this SpectrumValue
     */
    static Ptr<SpectrumValue> CreateTxPsdOverActiveRbs(
        double powerTx,
        const std::vector<int>& activeRbs,
        const Ptr<const SpectrumModel>& spectrumModel);

    /**
     * \brief Create SpectrumValue that will represent transmit power spectral density, and
     * the total transmit power will divided among all RBs, and then it will be assigned to active
     * RBs \param powerTx total power in dBm \param activeRbs vector of RBs that are active for this
     * transmission \param spectrumModel spectrumModel to be used to create this SpectrumValue
     */
    static Ptr<SpectrumValue> CreateTxPsdOverAllRbs(double powerTx,
                                                    const std::vector<int>& activeRbs,
                                                    const Ptr<const SpectrumModel>& spectrumModel);

    /**
     * Delete SpectrumValues stored in g_nrSpectrumModelMap
     */
    static void DeleteSpectrumValues();
};

} // namespace ns3

#endif /*  NR_SPECTRUM_VALUE_HELPER_H */
