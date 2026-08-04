// Copyright (c) 2023 New York University and NYU WIRELESS
// Users are encouraged to cite NYU WIRELESS publications regarding this work.
// Original source code is available in https://github.com/hiteshPoddar/NYUSIM_in_ns3
//
// SPDX-License-Identifier: MIT

#ifndef NYU_CHANNEL_CONDITION_MODEL_H
#define NYU_CHANNEL_CONDITION_MODEL_H

#include "ns3/channel-condition-model.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/random-variable-stream.h"
#include "ns3/vector.h"

#include <unordered_map>

namespace ns3
{

/**
 * \ingroup propagation
 *
 * \brief Base class for the NYU channel condition models
 *
 */
class NYUChannelConditionModel : public ChannelConditionModel
{
  public:
    /**
     * Get the type ID.
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor for the NYUChannelConditionModel class
     */
    NYUChannelConditionModel();

    /**
     * Destructor for the NYUChannelConditionModel class
     */
    ~NYUChannelConditionModel() override;

    /**
     * \brief Retrieve the condition of the channel between a and b.
     *
     * If the channel condition does not exists, the method computes it by calling
     * ComputeChannelCondition and stores it in a local cache, that will be updated
     * following the "UpdatePeriod" parameter.
     *
     * \param a mobility model
     * \param b mobility model
     * \return the condition of the channel between a and b
     */
    Ptr<ChannelCondition> GetChannelCondition(Ptr<const MobilityModel> a,
                                              Ptr<const MobilityModel> b) const override;
    /**
     * If this  model uses objects of type RandomVariableStream,
     * set the stream numbers to the integers starting with the offset
     * 'stream'. Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream the offset used to set the stream numbers
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream) override;

  protected:
    void DoDispose() override;

    /**
     * \brief Computes the 2D distance between two 3D vectors
     * \param a the first 3D vector
     * \param b the second 3D vector
     * \return the 2D distance between a and b
     */
    static double Calculate2dDistance(const Vector& a, const Vector& b);

    Ptr<UniformRandomVariable> m_uniformVar; //!< uniform random variable

  private:
    /**
     * This method computes the channel condition based on a probabilistic model
     * that is specific for the scenario of interest
     *
     * \param a tx mobility model
     * \param b rx mobility model
     * \return the channel condition
     */
    Ptr<ChannelCondition> ComputeChannelCondition(Ptr<const MobilityModel> a,
                                                  Ptr<const MobilityModel> b) const;

    /**
     * Compute the LOS probability.
     *
     * \param a tx mobility model
     * \param b rx mobility model
     * \return the LOS probability
     */
    virtual double ComputePlos(Ptr<const MobilityModel> a, Ptr<const MobilityModel> b) const = 0;

    /**
     * \brief Returns a unique and reciprocal key for the channel between a and b.
     * \param a tx mobility model
     * \param b rx mobility model
     * \return channel key
     */
    static uint32_t GetKey(Ptr<const MobilityModel> a, Ptr<const MobilityModel> b);

    /**
     * Struct to store the channel condition in the m_channelConditionMap
     */
    struct Item
    {
        Ptr<ChannelCondition> m_condition; //!< the channel condition
        Time m_generatedTime;              //!< the time when the condition was generated
    };

    std::unordered_map<uint32_t, Item>
        m_channelConditionMap; //!< map to store the channel conditions
    Time m_updatePeriod;       //!< the update period for the channel condition
};

/**
 * \ingroup propagation
 *
 * \brief Computes the channel condition for the RMa Scenario
 *
 * Computes the channel condition following the specifications for the RMa
 * scenario reported in Table 7.4.2-1 of 3GPP TR 38.901
 */
class NYURmaChannelConditionModel : public NYUChannelConditionModel
{
  public:
    /**
     * Get the type ID.
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor for the NYURmaChannelConditionModel class
     */
    NYURmaChannelConditionModel();

    /**
     * Destructor for the NYURmaChannelConditionModel class
     */
    ~NYURmaChannelConditionModel() override;

  private:
    /**
     * Compute the LOS probability for 0.5-150 GHz for the RMa scenario.
     *
     * \param a tx mobility model
     * \param b rx mobility model
     * \return the LOS probability
     */
    double ComputePlos(Ptr<const MobilityModel> a, Ptr<const MobilityModel> b) const override;
};

/**
 * \ingroup propagation
 *
 * \brief Computes the channel condition for the UMa Scenario
 *
 * Computes the channel condition(LOS/NLOS) for UMa in NYU Channel Model
 * as specified in https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7999294&tag=1
 * table II - NYU (squared) Model
 */
class NYUUmaChannelConditionModel : public NYUChannelConditionModel
{
  public:
    /**
     * Get the type ID.
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor for the NYUUmaChannelConditionModel class
     */
    NYUUmaChannelConditionModel();

    /**
     * Destructor for the NYUUmaChannelConditionModel class
     */
    ~NYUUmaChannelConditionModel() override;

  private:
    /**
     * Compute the LOS probability for 0.5-150 GHz for the UMa scenario.
     *
     * \param a tx mobility model
     * \param b rx mobility model
     * \return the LOS probability
     */
    double ComputePlos(Ptr<const MobilityModel> a, Ptr<const MobilityModel> b) const override;
};

/**
 * \ingroup propagation
 *
 * \brief Computes the channel condition for the UMi Scenario
 *
 * Computes the channel condition(LOS/NLOS) for the UMi scenario  in NYU Channel Model
 * as specified in https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7999294&tag=1
 * table I - NYU (squared) Model
 */
class NYUUmiChannelConditionModel : public NYUChannelConditionModel
{
  public:
    /**
     * Get the type ID.
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor for the NYUUmiChannelConditionModel class
     */
    NYUUmiChannelConditionModel();

    /**
     * Destructor for the NYUUmiChannelConditionModel class
     */
    ~NYUUmiChannelConditionModel() override;

  private:
    /**
     * Compute the LOS probability for 0.5 - 150 GHz for the UMi scenario.
     * \param a tx mobility model
     * \param b rx mobility model
     * \return the LOS probability
     */
    double ComputePlos(Ptr<const MobilityModel> a, Ptr<const MobilityModel> b) const override;
};

/**
 * \ingroup propagation
 *
 * \brief Computes the channel condition for the InH Scenario
 *
 * Computes the channel condition(LOS/NLOS) for InH in NYU Channel Model
 * as specified in https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7999294 (table III, row 2)
 */
class NYUInHChannelConditionModel : public NYUChannelConditionModel
{
  public:
    /**
     * Get the type ID.
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor for the NYUInHChannelConditionModel class
     */
    NYUInHChannelConditionModel();

    /**
     * Destructor for the NYUInHChannelConditionModel class
     */
    ~NYUInHChannelConditionModel() override;

  private:
    /**
     * Compute the LOS probability for 0.5-150 GHz for the InH scenario.
     *
     * \param a tx mobility model
     * \param b rx mobility model
     * \return the LOS probability
     */
    double ComputePlos(Ptr<const MobilityModel> a, Ptr<const MobilityModel> b) const override;
};

/**
 * \ingroup propagation
 *
 * \brief Computes the channel condition for the InF Scenario
 *
 * Computes the channel condition(LOS/NLOS) for InF in NYU Channel Model
 * by generating a random value between 0 and 1.
 */
class NYUInFChannelConditionModel : public NYUChannelConditionModel
{
  public:
    /**
     * Get the type ID.
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor for the NYUInFChannelConditionModel class
     */
    NYUInFChannelConditionModel();

    /**
     * Destructor for the NYUInFChannelConditionModel class
     */
    ~NYUInFChannelConditionModel() override;

  private:
    /**
     * Compute the LOS probability for 0.5-150 GHz for the InF scenario.
     * To be extended in future with the NYU LOS Probability model for above 100 GHz
     *
     * \param a tx mobility model
     * \param b rx mobility model
     * \return the LOS probability
     */
    double ComputePlos(Ptr<const MobilityModel> a, Ptr<const MobilityModel> b) const override;
};

} // namespace ns3

#endif /* CHANNEL_CONDITION_MODEL_H */
