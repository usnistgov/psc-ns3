// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef SRC_NR_MODEL_BEAMFORMING_VECTOR_H_
#define SRC_NR_MODEL_BEAMFORMING_VECTOR_H_

#include "beam-id.h"

#include <ns3/mobility-model.h>
#include <ns3/uniform-planar-array.h>

namespace ns3
{

/**
 * \ingroup utils
 * \brief Physical representation of a beam.
 *
 * Contains the vector of the antenna weight, as well as the beam id. These
 * values are stored as std::pair, and we provide utilities functions to
 * extract them.
 *
 * \see GetVector
 * \see GetBeamId
 */
typedef std::pair<PhasedArrayModel::ComplexVector, BeamId> BeamformingVector;

typedef std::pair<BeamformingVector, BeamformingVector> BeamformingVectorPair;

/**
 * \brief Create a quasi omni beamforming vector
 * \ingroup utils
 * \param antenna Antenna array for which the beamforming vector will be created
 * \return the beamforming vector
 */
PhasedArrayModel::ComplexVector CreateQuasiOmniBfv(const Ptr<const UniformPlanarArray>& antenna);

/**
 * \brief Creates a beamforming vector for a given sector and elevation
 * \ingroup utils
 * \param antenna Antenna array for which will be created the beamforming vector
 * \param sector sector to be used
 * \param elevation elevation to be used
 * \return the beamforming vector
 */
PhasedArrayModel::ComplexVector CreateDirectionalBfv(const Ptr<const UniformPlanarArray>& antenna,
                                                     uint16_t sector,
                                                     double elevation);

/**
 * \brief Creates a beamforming vector for a given azimuth and zenith
 * \ingroup utils
 * \param antenna Antenna array for which will be created the beamforming vector
 * \param azimuth azimuth to be used
 * \param zenith zenith to be used
 * \return the beamforming vector
 */
PhasedArrayModel::ComplexVector CreateDirectionalBfvAz(const Ptr<const UniformPlanarArray>& antenna,
                                                       double azimuth,
                                                       double zenith);

/**
 * \brief Get directs path beamforming vector bfv for a device with the mobility model
 * a for transmission toward device with a mobility model b, by using antenna aAntenna.
 * \param [in] a mobility model of the first device
 * \param [in] b mobility model of the second device
 * \param [in] antenna antenaArray of the first device
 * \return the resulting beamforming vector for antenna array for the first device
 */
PhasedArrayModel::ComplexVector CreateDirectPathBfv(const Ptr<MobilityModel>& a,
                                                    const Ptr<MobilityModel>& b,
                                                    const Ptr<const UniformPlanarArray>& antenna);
/**
 * \brief Creates a beamforming vector using Kronecker method for a given azimuth and zenith
 * \param [in] antenna Antenna array for which will be created the beamforming vector
 * \param [in] rowAngle row angle to be used
 * \param [in] colAngle column angle to be used
 * \return the beamforming vector
 */
PhasedArrayModel::ComplexVector CreateKroneckerBfv(const Ptr<const UniformPlanarArray>& antenna,
                                                   double rowAngle,
                                                   double colAngle);
} // namespace ns3

#endif /* SRC_NR_MODEL_BEAMFORMING_VECTOR_H_ */
