// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef SRC_NR_MODEL_BEAM_MANAGER_H_
#define SRC_NR_MODEL_BEAM_MANAGER_H_

#include "beamforming-vector.h"

#include "ns3/event-id.h"
#include <ns3/net-device.h>
#include <ns3/nstime.h>

#include <map>

namespace ns3
{

class NrUeNetDevice;
class NrGnbNetDevice;
class BeamformingHelperBase;

/**
 * \ingroup gnb-phy
 *
 * \brief Antenna array management
 *
 * BeamManager is responsible of installation and configuration of antenna
 * array. Additionally, in the case of gNB it saves the map of beamforming
 * vectors per device.
 */
class BeamManager : public Object
{
    friend BeamformingHelperBase;

  public:
    /**
     * \brief BeamManager constructor
     */
    BeamManager();

    /**
     * \brief ~BeamManager
     */
    ~BeamManager() override;

    /**
     * \brief GetTypeId
     * \return the TypeId of this instance
     */
    static TypeId GetTypeId();

    /*
     * \brief configures a predefined beam to use for directional transmissions
     */
    void SetPredefinedBeam(PhasedArrayModel::ComplexVector predefinedBeam);

    /**
     * \brief configures a predefined beam to use for directional transmissions
     * \param sector specifies the sector of the beam to be configured
     * \param elevation specifies the elevation angle of the beam to be configured
     */
    void SetPredefinedBeam(uint16_t sector, double elevation);

    /**
     * \brief Configures quasi-omni beamforming vector
     *
     * \param antennaArray the antenna array
     */
    void Configure(const Ptr<UniformPlanarArray>& antennaArray);

    /**
     * \brief Get weight vector from a BeamformingVector
     * \param v the BeamformingVector
     * \return the weight vector
     */
    PhasedArrayModel::ComplexVector GetVector(const BeamformingVector& v) const;

    /**
     * \brief Extract the beam id from the beamforming vector specified
     * \return the beam id
     * \param v the beamforming vector
     */
    BeamId GetBeamId(const BeamformingVector& v) const;

    /**
     * \brief Get a pointer to the current antenna
     * \return the antenna
     */
    Ptr<const UniformPlanarArray> GetAntenna() const;

    typedef std::map<const Ptr<const NetDevice>, BeamformingVector>
        BeamformingStorage; //!< BeamformingStorage type used to save the map of beamforming vectors
                            //!< per device

    /**
     * \brief Function that saves the beamforming weights of the antenna
     * for transmission or reception to/from a specified connected device.
     * \param bfv the unique identifier of the beam
     * \param device device to which it is being transmitted, or from which is
     * being received
     */
    virtual void SaveBeamformingVector(const BeamformingVector& bfv,
                                       const Ptr<const NetDevice>& device);
    /**
     * \brief Change the beamforming vector for tx/rx to/from specified device
     * \param device Device to change the beamforming vector for
     */
    virtual void ChangeBeamformingVector(const Ptr<const NetDevice>& device);

    /**
     * \brief Change current beamforming vector to quasi-omni beamforming vector
     */
    virtual void ChangeToQuasiOmniBeamformingVector();

    /**
     * \brief Function that returns the beamforming vector that is currently being
     * used by the antenna.
     * \return the current beamforming vector
     */
    virtual PhasedArrayModel::ComplexVector GetCurrentBeamformingVector();

    /**
     * \brief Function that returns the beamforming vector weights that is used to
     * communicated with a specified device
     * \return the current beamforming vector
     */
    virtual PhasedArrayModel::ComplexVector GetBeamformingVector(
        const Ptr<NetDevice>& device) const;

    /**
     * \brief Function that returns the beamId of the beam that is used to
     * communicated with a specified device
     * \return the current beamforming vector
     */
    virtual BeamId GetBeamId(const Ptr<NetDevice>& device) const;

    /**
     * \brief Set the Sector
     * \param sector sector
     * \param elevation elevation
     */
    void SetSector(uint16_t sector, double elevation) const;

    /**
     * \brief Set the Sector
     * \param azimuth azimuth
     * \param zenith zenith
     */
    void SetSectorAz(double azimuth, double zenith) const;

  private:
    Ptr<UniformPlanarArray>
        m_antennaArray;    //!< the antenna array instance for which is responsible this BeamManager
    uint32_t m_numRows{0}; //!< Number of rows of antenna array for which is calculated current
                           //!< quasi omni beamforming vector
    uint32_t m_numColumns{0};      //!< Number of columns of antenna array for which is calculated
                                   //!< current quasi omni beamforming vector
    uint32_t m_numPortElems{0};    //!< Number of analog antenna elements per antenna port
    bool m_isPolDual{false};       //!< True if the antenna array has dual polarized antennas
    BeamformingVector m_omniTxRxW; //!< Beamforming vector that emulates omnidirectional
                                   //!< transmission and reception
    BeamformingStorage m_beamformingVectorMap; //!< device to beamforming vector mapping
    BeamformingVector m_predefinedDirTxRxW;    //!< A predefined vector that is used for directional
                                               //!< transmission and reception to any device
};

} /* namespace ns3 */

#endif /* SRC_NR_MODEL_BEAM_MANAGER_H_ */
