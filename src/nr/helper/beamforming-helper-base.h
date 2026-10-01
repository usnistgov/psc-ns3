// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include <ns3/beamforming-vector.h>
#include <ns3/object-factory.h>
#include <ns3/object.h>
#include <ns3/vector.h>

#ifndef SRC_NR_HELPER_BEAMFORMING_HELPER_BASE_H_
#define SRC_NR_HELPER_BEAMFORMING_HELPER_BASE_H_

namespace ns3
{

class NrSpectrumPhy;
class NrGnbNetDevice;
class NrUeNetDevice;

/**
 * \ingroup helper
 * \brief The BeamformingHelperBase class that is being
 * used as the general interface for beamforming helper
 * classes. Currently, there are two beamforming helper classes:
 * `IdealBeamformingHelper` and `RealisticBeamformingHelper`
 * that inherit this base beamforming helper class
 */
class BeamformingHelperBase : public Object
{
  public:
    /**
     * \brief BeamformingHelperBase constructor
     */
    BeamformingHelperBase();
    /**
     * \brief ~BeamformingHelperBase destructor
     */
    ~BeamformingHelperBase() override;

    /**
     * \brief Get the Type ID
     * \return the TypeId of the instance
     */
    static TypeId GetTypeId();

    /**
     * \brief Creates a new beamforming task, which means the pair of
     * devices for which the configured algorithm for updating the
     * beamforming vectors will be run either periodically or
     * as specified by the algorithm.
     * \param gNbDev gNb device
     * \param ueDev UE device
     */
    virtual void AddBeamformingTask(const Ptr<NrGnbNetDevice>& gNbDev,
                                    const Ptr<NrUeNetDevice>& ueDev) = 0;

    /**
     * \brief Set the beamforming method that will be executed each
     * time when is necessary to update the beamforming algorithms
     * \param beamformingMethod the beamforming method to be set
     */
    virtual void SetBeamformingMethod(const TypeId& beamformingMethod) = 0;

    /**
     * \brief Set an attribute for the beafmorming algorithm that will be created.
     * \param n the name of the attribute
     * \param v the value of the attribute
     */
    void SetBeamformingAlgorithmAttribute(const std::string& n, const AttributeValue& v);

  protected:
    /**
     * \brief This function runs the beamforming algorithm among the provided gNB and UE
     * device, and for a specified bwp index
     * \param gNbSpectrumPhy a pointer to SpectrumPhy of gNb device
     * \param ueSpectrumPhy a pointer to SpectrumPhy of UE device
     */
    virtual void RunTask(const Ptr<NrSpectrumPhy>& gnbSpectrumPhy,
                         const Ptr<NrSpectrumPhy>& ueSpectrumPhy) const;

    /**
     * \brief Function that will call the configured algorithm for the specified devices and obtain
     * the beamforming vectors for each of them.
     * \param gnbSpectrumPhy a pointer to SpectrumPhy of gNb device
     * \param ueSpectrumPhy a pointer to SpectrumPhy of UE device
     * \return the pair of beamforming vectors, one corresponding to the gNB device, and one
     * corresponding to the UE device
     */
    virtual BeamformingVectorPair GetBeamformingVectors(
        const Ptr<NrSpectrumPhy>& gnbSpectrumPhy,
        const Ptr<NrSpectrumPhy>& ueSpectrumPhy) const = 0;

    typedef std::pair<Ptr<NrSpectrumPhy>, Ptr<NrSpectrumPhy>> SpectrumPhyPair;

    ObjectFactory
        m_algorithmFactory; //!< Object factory that will be used to create beamforming algorithms
};

}; // namespace ns3

#endif /* SRC_NR_HELPER_BEAMFORMING_HELPER_BASE_H_ */
