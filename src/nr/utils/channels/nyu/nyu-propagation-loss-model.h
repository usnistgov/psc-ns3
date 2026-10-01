// Copyright (c) 2023 New York University and NYU WIRELESS
// Users are encouraged to cite NYU WIRELESS publications regarding this work.
// Original source code is available in https://github.com/hiteshPoddar/NYUSIM_in_ns3
//
// SPDX-License-Identifier: MIT

#ifndef NYU_PROPAGATION_LOSS_MODEL_H
#define NYU_PROPAGATION_LOSS_MODEL_H

#include "nyu-channel-condition-model.h"

#include "ns3/propagation-loss-model.h"
#include "ns3/string.h"

namespace ns3
{

/**
 * \ingroup propagation
 *
 * \brief Base class for the NYU propagation models
 */

class NYUPropagationLossModel : public PropagationLossModel
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor
     */
    NYUPropagationLossModel();

    /**
     * Destructor
     */
    ~NYUPropagationLossModel() override;

    // Delete copy constructor and assignment operator to avoid misuse
    NYUPropagationLossModel(const NYUPropagationLossModel&) = delete;
    NYUPropagationLossModel& operator=(const NYUPropagationLossModel&) = delete;

    /**
     * \brief Set the channel condition model used to determine the channel
     *        state (e.g., the LOS/NLOS condition)
     * \param model pointer to the channel condition model
     */
    void SetChannelConditionModel(Ptr<ChannelConditionModel> model);

    /**
     * \brief Returns the associated channel condition model
     * \return the channel condition model
     */
    Ptr<ChannelConditionModel> GetChannelConditionModel() const;

    /**
     * \brief Set the central frequency of the model
     * \param frequency the central frequency in the range in Hz, between 500.0e6 and 100.0e9 Hz
     */
    void SetFrequency(double frequency);

    /**
     * \brief Return the current central frequency
     * \return The current central frequency
     */
    double GetFrequency() const;

    /**
     * \brief Set the Foliage Loss of the model
     * \param foliageLoss the Foliage loss in dB, between 0 to 10 dB per meter
     */
    void SetFoliageLoss(double foliageLoss);

    /**
     * \brief Return the Foliage Loss
     * \return the Foliage Loss in dB
     */
    double GetFoliageLoss() const;

    /**
     * \brief Set the atmospheric pressure
     * \param pressure the atmospheric pressure in mbar, between 1e-5 to 1013.25
     */
    void SetAtmosphericPressure(double pressure);

    /**
     * \brief Return the atmospheric pressure
     * \return the pressure in mbar
     */
    double GetAtmosphericPressure() const;

    /**
     * \brief Set the humidity
     * \param humidity the humidity in percentage, between 0 to 100
     */
    void SetHumidity(double humidity);

    /**
     * \brief Return the humidity
     * \return the humidity in percentage
     */
    double GetHumidity() const;

    /**
     * \brief Set the temperature
     * \param temperature the temperature in celsius, between -100 to 50 degrees celsius
     */
    void SetTemperature(double temperature);

    /**
     * \brief Return the temperature
     * \return the temperature in celsius
     */
    double GetTemperature() const;

    /**
     * \brief Set the rain rate
     * \param rainRate the rain rate in mm/hr, between 0 to 150 mm/hr
     */
    void SetRainRate(double rainRate);

    /**
     * \brief Return the rain rate
     * \return the rain rate in mm/hr
     */
    double GetRainRate() const;

    /**
     * \brief the atmospheric attenuation in dB
     * \param atmosphericAttenuationFactor the atmospheric attenuation factor in dB/m
     * \param distance2D the 2D distance between Tx and Rx
     * \return the atmoshperic attenuation
     */
    double GetAtmoshperticAttenuation(double atmosphericAttenuationFactor, double distance2D) const;

    /**
     * \brief the atmospheric attenuation factor in dB/m
     * \param frequency the frequency
     * \param pressure the atmospheric pressure in mbar
     * \param humidity the humidity in percentage
     * \param temperature the temperature in celsius
     * \param rainRate the rain rate in mm/hr
     * \return the atmoshperic attenuation factor
     */
    double GetAtmoshperticAttenuationFactor(double frequency,
                                            double pressure,
                                            double humidity,
                                            double temperature,
                                            double rainRate) const;

    /**
     * \brief the saturation pressure depednds on temperature and ice
     * \param temperature the temperature in celsius
     * \param ice flag indicating if ice is present/absent
     * \return the saturation pressure
     */
    double GetSaturationPressure(double temperature, bool ice) const;

    /**
     * \brief calculates the permitivitty of water
     * \param v the reciprocal of temperature in Kelvins
     * \param ice flag indicating if ice is present/absent
     * \return the permitivitty of water
     */
    double GetH2oPermittivity(double v, bool ice) const;

    /**
     * \brief calculates the attenuation factor due to oxygen in atmosphere
     * \param freqGHz the frequency of operation in GHz
     * \param v the reciprocal of temperature in Kelvins
     * \param pd the partial pressure for dry air in mbar
     * \param e the partial pressure for water vapor (mb)
     * \return the attenuation factor due to oxygen
     */
    double GetO2Lines(double freqGHz, double v, double pd, double e) const;

    /**
     * \brief calculates the attenuation factor due to dry air in atmosphere
     * \param freqGHz the frequency of operation in GHz
     * \param v the reciprocal of temperature in Kelvins
     * \param pd the partial pressure for dry air in mbar
     * \param e the partial pressure for water vapor (mb)
     * \return the attenuation factor due to dry air
     */
    double GetDryCont(double freqGHz, double v, double pd, double e) const;

    /**
     * \brief calculates the attenuation factor due to water vapor in atmosphere
     * \param freqGHz the frequency of operation in GHz
     * \param v the reciprocal of temperature in Kelvins
     * \param pd the partial pressure for dry air in mbar
     * \param e the partial pressure for water vapor (mb)
     * \return the attenuation factor due to dry air
     */
    double GetH2oVapor(double freqGHz, double v, double pd, double e) const;

    /**
     * \brief calculates the attenuation factor due to liquid water in atmosphere
     * \param freqGHz the frequency of operation in GHz
     * \param v the reciprocal of temperature in Kelvins
     * \param w indicates if water droplets present or absent in the atmosphere
     * \param ice flag indicating if ice is present/absent
     * \param eps the permitivitty of water
     * \return the attenuation factor due to liquid water
     */
    double GetH2oLiquid(double freqGHz, double v, double w, bool ice, double eps) const;

    /**
     * \brief calculates the attenuation factor due to rain
     * \param freqGHz the frequency of operation in GHz
     * \param rainRate the rain rate in mm/hr
     * \return the attenuation factor due to rain
     */
    double GetRainAttenuation(double freqGHz, double rainRate) const;

    /**
     * \brief calculates the non dispersive refractivity
     * \param v the reciprocal of temperature in Kelvins
     * \param pd the partial pressure for dry air in mbar
     * \param e the partial pressure for water vapor (mb)
     * \param rainRate the rain rate in mm/hr
     * \param w indicates if water droplets present or absent in the atmosphere
     * \param eps the permitivitty of water
     * \return the attenuation factor due to liquid water
     */
    double GetNonDispRef(double v, double pd, double e, double rainRate, bool w, double eps) const;
    /**
     * \brief Set the Outdoor to Indoor (O2I) Loss Type
     * \param o2iLossType the O2I Loss Type - High Loss or Low Loss
     */
    void SetO2ILossType(const std::string& o2iLossType);

    /**
     * \brief Return the Outdoor to Indoor (O2I) Loss Type
     * \return the Outdoor to Indoor (O2I) Loss Type
     */
    std::string GetO2ILossType() const;

    /**
     * \brief Find Path Loss due to Outdoor to Indoor (O2I) penetration
     * \param o2iLossType the O2I Loss Type - High Loss or Low Loss
     * \param frequency the central frequency of operation
     * \return the pathloss value in dB
     */
    double GetO2IPathLoss(const std::string& o2ilosstype, double frequency) const;

    /**
     * \brief Find Path Loss due to Foliage Loss
     * \param distance2D the 2D distance between Tx and Rx
     * \return the pathloss value in dB
     */
    double GetFoliagePathLoss(double distance2D) const;

    /**
     * \brief Calibrate Parameters for frequency range 0.5 GHz - 150 GHz
     * \param ple1 value at 28 GHz
     * \param ple2 value at 140 GHz
     * \param frequency the center frequency of operation
     * \return the calibrated value at the center frequency of operation
     */
    double GetCalibratedParameter(double ple1, double ple2, double frequency) const;

  private:
    /**
     * \brief Assign a fixed random variable stream number to the random variables used by this
     * model. \param stream first stream index to use \return the number of stream indices assigned
     * by this model
     */
    int64_t DoAssignStreams(int64_t stream) override;

    /**
     * \param txPowerDbm tx power in dBm
     * \param a tx mobility model
     * \param b rx mobility model
     * \return the rx power in dBm
     */
    double DoCalcRxPower(double txPowerDbm,
                         Ptr<MobilityModel> a,
                         Ptr<MobilityModel> b) const override;

    /**
     * \brief Computes the pathloss between a and b
     * \param cond the channel condition
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    double GetLoss(Ptr<ChannelCondition> cond, double distance2D, double hBs) const;

    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is not obstructed
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    virtual double GetLossLos(double distance2D, double hBs) const = 0;

    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is obstructed
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    virtual double GetLossNlos(double distance2D, double hBs) const = 0;

    /**
     * \brief Determines hUT and hBS. The default implementation assumes that
     *        the tallest node is the BS and the smallest is the UT. The derived classes
     * can change the default behavior by overriding this method.
     * \param za the height of the first node in meters
     * \param zb the height of the second node in meters
     * \return std::pair of heights in meters, the first element is hUt and the second is hBs
     */
    virtual std::pair<double, double> GetUtAndBsHeights(double za, double zb) const;

    /**
     * \brief Retrieves the shadowing value by looking at m_shadowingMap.
     *        If not found or if the channel condition changed it generates a new
     *        independent realization and stores it in the map, otherwise it correlates
     *        the new value with the previous one using the autocorrelation function
     *        defined in 3GPP TR 38.901, Sec. 7.4.4.
     * \param a tx mobility model
     * \param b rx mobility model
     * \param cond the LOS/NLOS channel condition
     * \return shadowing loss in dB
     */
    double GetShadowing(Ptr<MobilityModel> a,
                        Ptr<MobilityModel> b,
                        ChannelCondition::LosConditionValue cond) const;

    /**
     * \brief Returns the shadow fading standard deviation
     * \param a tx mobility model
     * \param b rx mobility model
     * \param cond the LOS/NLOS channel condition
     * \return shadowing std in dB
     */
    virtual double GetShadowingStd(ChannelCondition::LosConditionValue cond) const = 0;

    /**
     * \brief Returns the shadow fading correlation distance
     * \param cond the LOS/NLOS channel condition
     * \return shadowing correlation distance in meters
     */
    virtual double GetShadowingCorrelationDistance(
        ChannelCondition::LosConditionValue cond) const = 0;

    /**
     * \brief Returns an unique key for the channel between a and b.
     *
     * The key is the value of the Cantor function calculated by using as
     * first parameter the lowest node ID, and as a second parameter the highest
     * node ID.
     *
     * \param a tx mobility model
     * \param b rx mobility model
     * \return channel key
     */
    static uint32_t GetKey(Ptr<MobilityModel> a, Ptr<MobilityModel> b);

    /**
     * \brief Get the difference between the node position
     *
     * The difference is calculated as (b-a) if Id(a) < Id (b), or
     * (a-b) if Id(b) <= Id(a).
     *
     * \param a First node
     * \param b Second node
     * \return the difference between the node vector position
     */
    static Vector GetVectorDifference(Ptr<MobilityModel> a, Ptr<MobilityModel> b);

  protected:
    void DoDispose() override;

    /**
     * \brief Computes the 2D distance between two 3D vectors
     * \param a the first 3D vector
     * \param b the second 3D vector
     * \return the 2D distance between a and b
     */
    static double Calculate2dDistance(Vector a, Vector b);

    Ptr<ChannelConditionModel> m_channelConditionModel; //!< pointer to the channel condition model
    double m_frequency;                                 //!< operating frequency in Hz
    double m_foliageLoss;                               //!< loss due to foliage in dB/m
    double m_pressure;                                  //!< atmospheric pressure in mbar
    double m_humidity;                                  //!< humidity in percentage
    double m_temperature;                               //!< temperature in celsius
    double m_rainRate;                                  //!< rain rate in mm/hr
    std::string m_o2iLossType;                          //!< o2i loss type
    bool m_shadowingEnabled;                            //!< enable/disable shadowing
    bool m_foilageLossEnabled;                          //!< enable/disable foliage loss
    bool m_atmosphericLossEnabled;                      //!< enable/disable atmospheric loss
    Ptr<UniformRandomVariable> m_uniformVar;            //!< uniform random variable
    Ptr<NormalRandomVariable> m_normRandomVariable;     //!< normal random variable

    /** Define a struct for the m_shadowingMap entries */
    struct ShadowingMapItem
    {
        double m_shadowing;                              //!< the shadowing loss in dB
        ChannelCondition::LosConditionValue m_condition; //!< the LOS/NLOS condition
        Vector m_distance;                               //!< the vector AB
    };

    mutable std::unordered_map<uint32_t, ShadowingMapItem>
        m_shadowingMap; //!< map to store the shadowing values
};

/**
 * \ingroup propagation
 *
 * \brief Implements the pathloss model defined in
 * https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7999294 (equation 20 and equation 21) for
 * the RMa scenario.
 */
class NYURmaPropagationLossModel : public NYUPropagationLossModel
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor
     */
    NYURmaPropagationLossModel();

    /**
     * Destructor
     */
    ~NYURmaPropagationLossModel() override;

    // Delete copy constructor and assignment operator to avoid misuse
    NYURmaPropagationLossModel(const NYURmaPropagationLossModel&) = delete;
    NYURmaPropagationLossModel& operator=(const NYURmaPropagationLossModel&) = delete;

  private:
    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is not obstructed
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    double GetLossLos(double distance2D, double hBs) const override;

    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is obstructed
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    double GetLossNlos(double distance2D, double hBs) const override;

    /**
     * \brief Returns the shadow fading standard deviation
     * \param a tx mobility model
     * \param b rx mobility model
     * \param cond the LOS/NLOS channel condition
     * \return shadowing std in dB
     */
    double GetShadowingStd(ChannelCondition::LosConditionValue cond) const override;

    /**
     * \brief Returns the shadow fading correlation distance
     * \param cond the LOS/NLOS channel condition
     * \return shadowing correlation distance in meters
     */
    double GetShadowingCorrelationDistance(ChannelCondition::LosConditionValue cond) const override;
};

/**
 * \ingroup propagation
 *
 * \brief Implements the pathloss model defined in
 * https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7999294 (equation 2) for the UMa scenario.
 */
class NYUUmaPropagationLossModel : public NYUPropagationLossModel
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor
     */
    NYUUmaPropagationLossModel();

    /**
     * Destructor
     */
    ~NYUUmaPropagationLossModel() override;

    // Delete copy constructor and assignment operator to avoid misuse
    NYUUmaPropagationLossModel(const NYUUmaPropagationLossModel&) = delete;
    NYUUmaPropagationLossModel& operator=(const NYUUmaPropagationLossModel&) = delete;

  private:
    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is not obstructed
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    double GetLossLos(double distance2D, double hBs) const override;

    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is obstructed.
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    double GetLossNlos(double distance2D, double hBs) const override;

    /**
     * \brief Returns the shadow fading standard deviation
     * \param a tx mobility model
     * \param b rx mobility model
     * \param cond the LOS/NLOS channel condition
     * \return shadowing std in dB
     */
    double GetShadowingStd(ChannelCondition::LosConditionValue cond) const override;

    /**
     * \brief Returns the shadow fading correlation distance
     * \param cond the LOS/NLOS channel condition
     * \return shadowing correlation distance in meters
     */
    double GetShadowingCorrelationDistance(ChannelCondition::LosConditionValue cond) const override;
};

/**
 * \ingroup propagation
 *
 * \brief Implements the pathloss model defined in
 * https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7999294 (equation 2) for the UMi Scenario.
 */
class NYUUmiPropagationLossModel : public NYUPropagationLossModel
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor
     */
    NYUUmiPropagationLossModel();

    /**
     * Destructor
     */
    ~NYUUmiPropagationLossModel() override;

    // Delete copy constructor and assignment operator to avoid misuse
    NYUUmiPropagationLossModel(const NYUUmiPropagationLossModel&) = delete;
    NYUUmiPropagationLossModel& operator=(const NYUUmiPropagationLossModel&) = delete;

  private:
    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is not obstructed
     * \param distance2D the 3D distance between tx and rx in meters
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    double GetLossLos(double distance2D, double hBs) const override;

    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is obstructed
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    double GetLossNlos(double distance2D, double hBs) const override;

    /**
     * \brief Returns the shadow fading standard deviation
     * \param a tx mobility model
     * \param b rx mobility model
     * \param cond the LOS/NLOS channel condition
     * \return shadowing std in dB
     */
    double GetShadowingStd(ChannelCondition::LosConditionValue cond) const override;

    /**
     * \brief Returns the shadow fading correlation distance
     * \param cond the LOS/NLOS channel condition
     * \return shadowing correlation distance in meters
     */
    double GetShadowingCorrelationDistance(ChannelCondition::LosConditionValue cond) const override;
};

/**
 * \ingroup propagation
 *
 * \brief Implements the pathloss model defined in
 * https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7999294 (equation 2) for the InH scenario.
 */
class NYUInHPropagationLossModel : public NYUPropagationLossModel
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor
     */
    NYUInHPropagationLossModel();

    /**
     * Destructor
     */
    ~NYUInHPropagationLossModel() override;

    // Delete copy constructor and assignment operator to avoid misuse
    NYUInHPropagationLossModel(const NYUInHPropagationLossModel&) = delete;
    NYUInHPropagationLossModel& operator=(const NYUInHPropagationLossModel&) = delete;

  private:
    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is not obstructed
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    double GetLossLos(double distance2D, double hBs) const override;

    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is obstructed
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    double GetLossNlos(double distance2D, double hBs) const override;

    /**
     * \brief Returns the shadow fading standard deviation
     * \param a tx mobility model
     * \param b rx mobility model
     * \param cond the LOS/NLOS channel condition
     * \return shadowing std in dB
     */
    double GetShadowingStd(ChannelCondition::LosConditionValue cond) const override;

    /**
     * \brief Returns the shadow fading correlation distance
     * \param cond the LOS/NLOS channel condition
     * \return shadowing correlation distance in meters
     */
    double GetShadowingCorrelationDistance(ChannelCondition::LosConditionValue cond) const override;
};

/**
 * \ingroup propagation
 *
 * \brief Implements the pathloss model defined in
 * https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7999294 (equation 2) for the InF scenario.
 */
class NYUInFPropagationLossModel : public NYUPropagationLossModel
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor
     */
    NYUInFPropagationLossModel();

    /**
     * Destructor
     */
    ~NYUInFPropagationLossModel() override;

    // Delete copy constructor and assignment operator to avoid misuse
    NYUInFPropagationLossModel(const NYUInFPropagationLossModel&) = delete;
    NYUInFPropagationLossModel& operator=(const NYUInFPropagationLossModel&) = delete;

  private:
    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is not obstructed
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    double GetLossLos(double distance2D, double hBs) const override;

    /**
     * \brief Computes the pathloss between a and b considering that the line of
     *        sight is obstructed
     * \param distance2D the 2D distance between Tx and Rx
     * \param hBs the height of the BS in meters
     * \return pathloss value in dB
     */
    double GetLossNlos(double distance2D, double hBs) const override;

    /**
     * \brief Returns the shadow fading standard deviation
     * \param a tx mobility model
     * \param b rx mobility model
     * \param cond the LOS/NLOS channel condition
     * \return shadowing std in dB
     */
    double GetShadowingStd(ChannelCondition::LosConditionValue cond) const override;

    /**
     * \brief Returns the shadow fading correlation distance
     * \param cond the LOS/NLOS channel condition
     * \return shadowing correlation distance in meters
     */
    double GetShadowingCorrelationDistance(ChannelCondition::LosConditionValue cond) const override;
};

} // namespace ns3

#endif /* NYU_PROPAGATION_LOSS_MODEL_H */
