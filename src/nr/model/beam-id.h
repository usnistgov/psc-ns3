// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef SRC_NR_MODEL_BEAM_ID_H_
#define SRC_NR_MODEL_BEAM_ID_H_

#include <complex>
#include <stdint.h>

namespace ns3
{

/**
 * \brief Representation of a beam id
 *
 * A beam id in ns-3 is a pair that contains the sector, stored as a uint16_t,
 * and the elevation, stored as a double. Utilities functions are provided to
 * extract the values. This ID usually comes with the real physical representation
 * of a Beam, expressed by BeamformingVector.
 *
 * \see GetSector
 * \see GetElevation
 */
class BeamId
{
  public:
    /**
     * \brief Default constructor which created beamId with 0 sector and 0 elevation.
     */
    BeamId();

    /**
     * \constructor Constructor used to configure both sector and elevation.
     * \param sector species the sector of the beam
     * \param elevation specifies the elevation of the beam
     */
    BeamId(uint16_t sector, double elevation);

    /**
     * \brief Objects of this class are used as key in hash
     * table. This class must implement operator ==()
     * to handle collisions.
     * \param p BeamId with which we compare this object
     */
    bool operator==(const BeamId& p) const;

    /**
     * \brief Overrides != operator for the general use case
     * \param p BeamId with which we compare this object
     */
    bool operator!=(const BeamId& p) const;

    /**
     * \brief Extract the sector from the beam id
     * \return The sector of the beam
     */
    uint16_t GetSector() const;

    /**
     * \brief Extract the elevation from the beam id
     * \return the elevation of the beam
     */
    double GetElevation() const;

    /*
     * \brief Create BeamId with 0 sector and 0 elevation
     * \return BeamId (0,0)
     */
    static BeamId GetEmptyBeamId();

    /**
     * \return Returns the Cantor function value of this BeamId
     */
    uint32_t GetCantor() const;

  private:
    uint16_t m_sector{0};  //!< sector of the beam
    double m_elevation{0}; //!< elevation of the beam
};

// we reserve pair 65535, 65535 to identify the OMNI beam
/**
 * \brief Name of the OMNI beam
 * \ingroup utils
 */
extern const BeamId OMNI_BEAM_ID;

// we reserve pair 65534, 65534 to identify the directional predefined beam
/**
 * \brief Reserved ID for the predefined directional beam if it cannot be expressed
 * through sector and elevation
 * \ingroup utils
 */
extern const BeamId PREDEFINED_BEAM_ID;

/**
 * \brief Calculate the hash of a BeamId
 * \ingroup utils
 */
struct BeamIdHash
{
    /**
     * \brief operator ()
     * \param x beam id
     * \return the beam id hash
     */
    size_t operator()(const BeamId& x) const;
};

std::ostream& operator<<(std::ostream& os, const BeamId& item);

} /* namespace ns3 */

#endif /* SRC_NR_MODEL_ID_MANAGER_H_ */
