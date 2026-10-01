// Copyright (c) 2021 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef DISTANCE_BASED_THREE_GPP_SPECTRUM_PROPAGATION_LOSS_H
#define DISTANCE_BASED_THREE_GPP_SPECTRUM_PROPAGATION_LOSS_H

#include "ns3/three-gpp-spectrum-propagation-loss-model.h"

namespace ns3
{

/**
 * \ingroup nr-utils
 * \brief Distance based 3GPP Spectrum Propagation Loss Model
 *
 * This class inherits ThreeGppSpectrumPropagationLossModel
 * and calculates the fading and beamforming only for the signals
 * being transmitted among nodes whose distance is lower than the
 * max allowed distance that can be configured
 * through the attribute of this class.
 *
 * \see ThreeGppSpectrumPropagationLossModel
 */
class DistanceBasedThreeGppSpectrumPropagationLossModel
    : public ThreeGppSpectrumPropagationLossModel
{
  public:
    /**
     * Constructor
     */
    DistanceBasedThreeGppSpectrumPropagationLossModel();

    /**
     * Destructor
     */
    ~DistanceBasedThreeGppSpectrumPropagationLossModel() override;

    /**
     * Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /*
     * \brief Sets the max distance
     */
    void SetMaxDistance(double maxDistance);

    /*
     * \brief Gets the configured max distance
     */
    double GetMaxDistance() const;

    /**
     * \brief Computes the received PSD.
     *
     * This function computes the received PSD by applying the 3GPP fast fading
     * model and the beamforming gain. However, if the distance between a and b
     * is higher than allowed this class will return 0 PSD.
     *
     * \param params tx parameters
     * \param a first node mobility model
     * \param b second node mobility model
     * \param aPhasedArrayModel the antenna array of the first node
     * \param bPhasedArrayModel the antenna array of the second node
     * \return the received PSD
     */
    Ptr<SpectrumSignalParameters> DoCalcRxPowerSpectralDensity(
        Ptr<const SpectrumSignalParameters> params,
        Ptr<const MobilityModel> a,
        Ptr<const MobilityModel> b,
        Ptr<const PhasedArrayModel> aPhasedArrayModel,
        Ptr<const PhasedArrayModel> bPhasedArrayModel) const override;

  private:
    double m_maxDistance{1000}; //!< the maximum distance of the nodes a and b in order to calculate
                                //!< fast fading and the beamforming gain
};
} // namespace ns3

#endif /* THREE_GPP_SPECTRUM_PROPAGATION_LOSS_H */
