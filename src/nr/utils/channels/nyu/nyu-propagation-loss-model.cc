// Copyright (c) 2023 New York University and NYU WIRELESS
// Users are encouraged to cite NYU WIRELESS publications regarding this work.
// Original source code is available in https://github.com/hiteshPoddar/NYUSIM_in_ns3
//
// SPDX-License-Identifier: MIT

#include "nyu-propagation-loss-model.h"

#include "nyu-channel-condition-model.h"

#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include "ns3/node.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"
#include "ns3/string.h"

#include <cmath>
#include <complex>
#include <math.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NYUPropagationLossModel");

static const double M_C = 3.0e8;                // in m/s
static const double refdistance = 1;            // 1m free space reference distance in meters
static const double lowerLimitFrequency = 28;   // in GHz
static const double higherLimitFrequency = 140; // in GHz

static const double oxygen[44][7] = {
    {50.474238, 0.094, 9.694, 0.890, 0, 0.240, 0.790},
    {50.987749, 0.246, 8.694, 0.910, 0, 0.220, 0.780},
    {51.503350, 0.608, 7.744, 0.940, 0, 0.197, 0.774},
    {52.021410, 1.414, 6.844, 0.970, 0, 0.166, 0.764},
    {52.542394, 3.102, 6.004, 0.990, 0, 0.136, 0.751},
    {53.066907, 6.410, 5.224, 1.020, 0, 0.131, 0.714},
    {53.595749, 12.470, 4.484, 1.050, 0, 0.230, 0.584},
    {54.130000, 22.800, 3.814, 1.070, 0, 0.335, 0.431},
    {54.671159, 39.180, 3.194, 1.100, 0, 0.374, 0.305},
    {55.221367, 63.160, 2.624, 1.130, 0, 0.258, 0.339},
    {55.783802, 95.350, 2.119, 1.170, 0, -0.166, 0.705},
    {56.264775, 54.890, 0.015, 1.730, 0, 0.390, -0.113},
    {56.363389, 134.400, 1.660, 1.200, 0, -0.297, 0.753},
    {56.968206, 176.300, 1.260, 1.240, 0, -0.416, 0.742},
    {57.612484, 214.100, 0.915, 1.280, 0, -0.613, 0.697},
    {58.323877, 238.600, 0.626, 1.330, 0, -0.205, 0.051},
    {58.446590, 145.700, 0.084, 1.520, 0, 0.748, -0.146},
    {59.164207, 240.400, 0.391, 1.390, 0, -0.722, 0.266},
    {59.590983, 211.200, 0.212, 1.430, 0, 0.765, -0.090},
    {60.306061, 212.400, 0.212, 1.450, 0, -0.705, 0.081},
    {60.434776, 246.100, 0.391, 1.360, 0, 0.697, -0.324},
    {61.150560, 250.400, 0.626, 1.310, 0, 0.104, -0.067},
    {61.800154, 229.800, 0.915, 1.270, 0, 0.570, -0.761},
    {62.411215, 193.300, 1.260, 1.230, 0, 0.360, -0.777},
    {62.486260, 151.700, 0.083, 1.540, 0, -0.498, 0.097},
    {62.997977, 150.300, 1.665, 1.200, 0, 0.239, -0.768},
    {63.568518, 108.700, 2.115, 1.170, 0, 0.108, -0.706},
    {64.127767, 73.350, 2.620, 1.130, 0, -0.311, -0.332},
    {64.678903, 46.350, 3.195, 1.100, 0, -0.421, -0.298},
    {65.224071, 27.480, 3.815, 1.070, 0, -0.375, -0.423},
    {65.764772, 15.300, 4.485, 1.050, 0, -0.267, -0.575},
    {66.302091, 8.009, 5.225, 1.020, 0, -0.168, -0.700},
    {66.836830, 3.946, 6.005, 0.990, 0, -0.169, -0.735},
    {67.369598, 1.832, 6.845, 0.970, 0, -0.200, -0.744},
    {67.900867, 0.801, 7.745, 0.940, 0, -0.228, -0.753},
    {68.431005, 0.330, 8.695, 0.920, 0, -0.240, -0.760},
    {68.960311, 0.128, 9.695, 0.900, 0, -0.250, -0.765},
    {118.750343, 94.500, 0.009, 1.630, 0, -0.036, 0.009},
    {368.498350, 6.790, 0.049, 1.920, 0.6, 0, 0},
    {424.763124, 63.800, 0.044, 1.930, 0.6, 0, 0},
    {487.249370, 23.500, 0.049, 1.920, 0.6, 0, 0},
    {715.393150, 9.960, 0.145, 1.810, 0.6, 0, 0},
    {773.839675, 67.100, 0.130, 1.820, 0.6, 0, 0},
    {834.145330, 18.000, 0.147, 1.810, 0.6, 0, 0},
};

static const double water[35][7] = {
    {22.235080, 0.01130, 2.143, 2.811, 4.80, 0.69, 1.00},
    {67.803960, 0.00012, 8.735, 2.858, 4.93, 0.69, 0.82},
    {119.995940, 0.00008, 8.356, 2.948, 4.78, 0.70, 0.79},
    {183.310091, 0.24200, 0.668, 3.050, 5.30, 0.64, 0.85},
    {321.225644, 0.00483, 6.181, 2.303, 4.69, 0.67, 0.54},
    {325.152919, 0.14990, 1.540, 2.783, 4.85, 0.68, 0.74},
    {336.222601, 0.00011, 9.829, 2.693, 4.74, 0.69, 0.61},
    {380.197372, 1.15200, 1.048, 2.873, 5.38, 0.54, 0.89},
    {390.134508, 0.00046, 7.350, 2.152, 4.81, 0.63, 0.55},
    {437.346667, 0.00650, 5.050, 1.845, 4.23, 0.60, 0.48},
    {439.150812, 0.09218, 3.596, 2.100, 4.29, 0.63, 0.52},
    {443.018295, 0.01976, 5.050, 1.860, 4.23, 0.60, 0.50},
    {448.001075, 1.03200, 1.405, 2.632, 4.84, 0.66, 0.67},
    {470.888947, 0.03297, 3.599, 2.152, 4.57, 0.66, 0.65},
    {474.689127, 0.12620, 2.381, 2.355, 4.65, 0.65, 0.64},
    {488.491133, 0.02520, 2.853, 2.602, 5.04, 0.69, 0.72},
    {503.568532, 0.00390, 6.733, 1.612, 3.98, 0.61, 0.43},
    {504.482692, 0.00130, 6.733, 1.612, 4.01, 0.61, 0.45},
    {547.676440, 0.97010, 0.114, 2.600, 4.50, 0.70, 1.00},
    {552.020960, 1.47700, 0.114, 2.600, 4.50, 0.70, 1.00},
    {556.936002, 48.74000, 0.159, 3.210, 4.11, 0.69, 1.00},
    {620.700807, 0.50120, 2.200, 2.438, 4.68, 0.71, 0.68},
    {645.866155, 0.00713, 8.580, 1.800, 4.00, 0.60, 0.50},
    {658.005280, 0.03022, 7.820, 3.210, 4.14, 0.69, 1.00},
    {752.033227, 23.96000, 0.396, 3.060, 4.09, 0.68, 0.84},
    {841.053973, 0.00140, 8.180, 1.590, 5.76, 0.33, 0.45},
    {859.962313, 0.01472, 7.989, 3.060, 4.09, 0.68, 0.84},
    {899.306675, 0.00605, 7.917, 2.985, 4.53, 0.68, 0.90},
    {902.616173, 0.00426, 8.432, 2.865, 5.10, 0.70, 0.95},
    {906.207325, 0.01876, 5.111, 2.408, 4.70, 0.70, 0.53},
    {916.171582, 0.83400, 1.442, 2.670, 4.78, 0.70, 0.78},
    {923.118427, 0.00869, 10.220, 2.900, 5.00, 0.70, 0.80},
    {970.315022, 0.89720, 1.920, 2.550, 4.94, 0.64, 0.67},
    {987.926764, 13.21000, 0.258, 2.985, 4.55, 0.68, 0.90},
    {1780.00000, 2230.00000, 0.952, 17.620, 30.50, 2.00, 5.00},
};

// ------------------------------------------------------------------------- //
TypeId
NYUPropagationLossModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NYUPropagationLossModel")
            .SetParent<PropagationLossModel>()
            .SetGroupName("Propagation")
            .AddAttribute("Frequency",
                          "The centre frequency in Hz.",
                          DoubleValue(28.0e9),
                          MakeDoubleAccessor(&NYUPropagationLossModel::SetFrequency,
                                             &NYUPropagationLossModel::GetFrequency),
                          MakeDoubleChecker<double>())
            .AddAttribute("FoliageLoss",
                          "The Foilage Loss in dB",
                          DoubleValue(0.4),
                          MakeDoubleAccessor(&NYUPropagationLossModel::SetFoliageLoss,
                                             &NYUPropagationLossModel::GetFoliageLoss),
                          MakeDoubleChecker<double>())
            .AddAttribute("ShadowingEnabled",
                          "Enable/disable shadowing.",
                          BooleanValue(true),
                          MakeBooleanAccessor(&NYUPropagationLossModel::m_shadowingEnabled),
                          MakeBooleanChecker())
            .AddAttribute("O2ILosstype",
                          "Outdoor to indoor (O2I) penetration loss type - Low Loss / High Loss.",
                          StringValue("Low Loss"),
                          MakeStringAccessor(&NYUPropagationLossModel::SetO2ILossType,
                                             &NYUPropagationLossModel::GetO2ILossType),
                          MakeStringChecker())
            .AddAttribute("FoliageLossEnabled",
                          "Enable/disable foilage loss.",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NYUPropagationLossModel::m_foilageLossEnabled),
                          MakeBooleanChecker())
            .AddAttribute("AtmosphericLossEnabled",
                          "Enable/disable atnispheric loss.",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NYUPropagationLossModel::m_atmosphericLossEnabled),
                          MakeBooleanChecker())
            .AddAttribute("Pressure",
                          "The barometric pressure in mbar",
                          DoubleValue(1013.25),
                          MakeDoubleAccessor(&NYUPropagationLossModel::SetAtmosphericPressure,
                                             &NYUPropagationLossModel::GetAtmosphericPressure),
                          MakeDoubleChecker<double>())
            .AddAttribute("Humidity",
                          "The humidity in percentage",
                          DoubleValue(50),
                          MakeDoubleAccessor(&NYUPropagationLossModel::SetHumidity,
                                             &NYUPropagationLossModel::GetHumidity),
                          MakeDoubleChecker<double>())
            .AddAttribute("Temperature",
                          "The temperature in celsius",
                          DoubleValue(20),
                          MakeDoubleAccessor(&NYUPropagationLossModel::SetTemperature,
                                             &NYUPropagationLossModel::GetTemperature),
                          MakeDoubleChecker<double>())
            .AddAttribute("RainRate",
                          "The rain rate in mm/hr",
                          DoubleValue(0),
                          MakeDoubleAccessor(&NYUPropagationLossModel::SetRainRate,
                                             &NYUPropagationLossModel::GetRainRate),
                          MakeDoubleChecker<double>())
            .AddAttribute("ChannelConditionModel",
                          "Pointer to the channel condition model.",
                          PointerValue(),
                          MakePointerAccessor(&NYUPropagationLossModel::SetChannelConditionModel,
                                              &NYUPropagationLossModel::GetChannelConditionModel),
                          MakePointerChecker<ChannelConditionModel>());
    return tid;
}

NYUPropagationLossModel::NYUPropagationLossModel()
    : PropagationLossModel()
{
    NS_LOG_FUNCTION(this);
    m_uniformVar = CreateObject<UniformRandomVariable>();
    m_normRandomVariable = CreateObject<NormalRandomVariable>();
    m_normRandomVariable->SetAttribute("Mean", DoubleValue(0));
    m_normRandomVariable->SetAttribute("Variance", DoubleValue(1));
}

NYUPropagationLossModel::~NYUPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
}

void
NYUPropagationLossModel::DoDispose()
{
    m_channelConditionModel->Dispose();
    m_channelConditionModel = nullptr;
    m_shadowingMap.clear();
}

void
NYUPropagationLossModel::SetChannelConditionModel(Ptr<ChannelConditionModel> model)
{
    NS_LOG_FUNCTION(this);
    m_channelConditionModel = model;
}

Ptr<ChannelConditionModel>
NYUPropagationLossModel::GetChannelConditionModel() const
{
    NS_LOG_FUNCTION(this);
    return m_channelConditionModel;
}

int64_t
NYUPropagationLossModel::DoAssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this);
    m_uniformVar->SetStream(stream);
    return 1;
}

void
NYUPropagationLossModel::SetFrequency(double frequency)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(frequency >= 500.0e6 && frequency <= 150.0e9,
                  "Frequency should be between 0.5 and 150 GHz but is " << frequency);
    m_frequency = frequency;
}

double
NYUPropagationLossModel::GetFrequency() const
{
    NS_LOG_FUNCTION(this);
    return m_frequency;
}

void
NYUPropagationLossModel::SetFoliageLoss(double foliageLoss)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(foliageLoss >= 0 && foliageLoss <= 10,
                  "foilage loss should be between 0 dB/m and 10 dB/m but is " << foliageLoss);
    m_foliageLoss = foliageLoss;
}

double
NYUPropagationLossModel::GetFoliageLoss() const
{
    NS_LOG_FUNCTION(this);
    return m_foliageLoss;
}

void
NYUPropagationLossModel::SetAtmosphericPressure(double pressure)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(pressure >= 1e-5 && pressure <= 1013.25,
                  "Barometric pressure should be between 1e-5 mbar to 1013.5 mbar but is "
                      << pressure);
    m_pressure = pressure;
}

double
NYUPropagationLossModel::GetAtmosphericPressure() const
{
    NS_LOG_FUNCTION(this);
    return m_pressure;
}

void
NYUPropagationLossModel::SetHumidity(double humidity)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(humidity >= 0 && humidity <= 100,
                  "Humidity should be between 0 to 100 but is " << humidity);
    m_humidity = humidity;
}

double
NYUPropagationLossModel::GetHumidity() const
{
    NS_LOG_FUNCTION(this);
    return m_humidity;
}

void
NYUPropagationLossModel::SetTemperature(double temperature)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(temperature >= -100 && temperature <= 50,
                  "Temperature should be between -100 to 50 celsius but is " << temperature);
    m_temperature = temperature;
}

double
NYUPropagationLossModel::GetTemperature() const
{
    NS_LOG_FUNCTION(this);
    return m_temperature;
}

void
NYUPropagationLossModel::SetRainRate(double rainRate)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(rainRate >= 0 && rainRate <= 150,
                  "Rain rate should be between 0 to 150 mm/hr but is " << rainRate);
    m_rainRate = rainRate;
}

double
NYUPropagationLossModel::GetRainRate() const
{
    NS_LOG_FUNCTION(this);
    return m_rainRate;
}

void
NYUPropagationLossModel::SetO2ILossType(const std::string& o2iLossType)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(o2iLossType != "Low Loss" || o2iLossType != "High Loss",
                  "O2ILossType should be Low Loss or High Loss but is " << o2iLossType);
    m_o2iLossType = o2iLossType;
}

std::string
NYUPropagationLossModel::GetO2ILossType() const
{
    NS_LOG_FUNCTION(this);
    return m_o2iLossType;
}

double
NYUPropagationLossModel::DoCalcRxPower(double txPowerDbm,
                                       Ptr<MobilityModel> a,
                                       Ptr<MobilityModel> b) const
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(m_frequency != 0.0, "First set the centre frequency");
    // retrieve the channel condition
    NS_ASSERT_MSG(m_channelConditionModel, "First set the channel condition model");
    Ptr<ChannelCondition> cond = m_channelConditionModel->GetChannelCondition(a, b);

    // compute the 2D distance between a and b
    double distance2D = Calculate2dDistance(a->GetPosition(), b->GetPosition());

    // compute hUT and hBS
    std::pair<double, double> heights = GetUtAndBsHeights(a->GetPosition().z, b->GetPosition().z);

    double rxPow = txPowerDbm;
    double PL = 0;
    double atmosphericAttenuationFactor = 0;

    PL = GetLoss(cond, distance2D, heights.second);

    if (m_shadowingEnabled)
    {
        PL += GetShadowing(a, b, cond->GetLosCondition());
    }
    if (cond->GetO2iCondition() == ChannelCondition::O2I)
    {
        PL += GetO2IPathLoss(m_o2iLossType, m_frequency);
    }
    if (m_foilageLossEnabled)
    {
        PL += GetFoliagePathLoss(distance2D);
    }
    if (m_atmosphericLossEnabled)
    {
        atmosphericAttenuationFactor = GetAtmoshperticAttenuationFactor(m_frequency,
                                                                        GetAtmosphericPressure(),
                                                                        GetHumidity(),
                                                                        GetTemperature(),
                                                                        GetRainRate());
        PL += GetAtmoshperticAttenuation(atmosphericAttenuationFactor, distance2D);
    }
    rxPow -= PL;
    return rxPow;
}

double
NYUPropagationLossModel::GetAtmoshperticAttenuation(double atmosphericAttenuationFactor,
                                                    double distance2D) const
{
    double atmoshpericAttenuation;
    atmoshpericAttenuation = atmosphericAttenuationFactor * distance2D;
    return atmoshpericAttenuation;
}

double
NYUPropagationLossModel::GetAtmoshperticAttenuationFactor(double frequency,
                                                          double pressure,
                                                          double humidity,
                                                          double temperature,
                                                          double rainRate) const
{
    NS_LOG_FUNCTION(this << frequency << pressure << humidity << temperature << rainRate);
    double atmosphericAttenuationFactor = 0;
    double freqGHz = frequency / 1e9;
    bool ice = false;
    double w = 0;
    double es = 0;
    double e = 0;
    double pd = 0;
    double eps = 0;
    double v = 0;
    double o2Lines = 0;
    double dryAir = 0;
    double h2oVapor = 0;
    double h2oLiquid = 0;
    double rain = 0;
    double n0 = 0;

    if (freqGHz < 1)
    {
        freqGHz = 1;
    }

    if (humidity > 99.5)
    {
        w = 1;
    }
    else
    {
        NS_LOG_DEBUG("Haze model doesn't exist");
    }

    // temp less than 0 means snow
    if (temperature <= 0)
    {
        ice = true;
    }

    v = 300. / (temperature + 273.15);

    es = GetSaturationPressure(temperature, ice);

    e = es * humidity / 100;
    pd = pressure - e;

    if (pd < 0)
    {
        pd = 0;
        e = pressure;
    }

    eps = GetH2oPermittivity(v, ice);

    o2Lines = GetO2Lines(freqGHz, v, pd, e);

    dryAir = GetDryCont(freqGHz, v, pd, e);

    h2oVapor = GetH2oVapor(freqGHz, v, pd, e);

    h2oLiquid = GetH2oLiquid(freqGHz, v, w, ice, eps);

    rain = GetRainAttenuation(freqGHz, rainRate);

    n0 = GetNonDispRef(v, pd, e, rainRate, w, eps);

    atmosphericAttenuationFactor =
        0.182 * freqGHz * (o2Lines + dryAir + h2oVapor + h2oLiquid + rain) * 1e-3;

    NS_LOG_DEBUG("attenuation factor:" << atmosphericAttenuationFactor << " Es:" << es << " e:" << e
                                       << " pd:" << pd << " Eps:" << eps << " ice:" << ice
                                       << " W:" << w << " v:" << v << " O2Lines:" << o2Lines);
    NS_LOG_DEBUG("dryAir:" << dryAir << " h2oVapor:" << h2oVapor << " h2oLiquid:" << h2oLiquid
                           << " rain:" << rain << " n0:" << n0);

    return atmosphericAttenuationFactor;
}

double
NYUPropagationLossModel::GetSaturationPressure(double temperature, bool ice) const
{
    double Y = 0;
    double x = 0;
    double Es = 0;
    if (!ice)
    {
        Y = 373.16 / (temperature + 273.16);
        x = -7.90298 * (Y - 1) + 5.02808 * log10(Y) -
            1.3816 * pow(10, -7) * (pow(10, (11.344 * (1 - (1 / Y)))) - 1) +
            8.1328 * pow(10, -3) * (pow(10, (-3.49149 * (Y - 1))) - 1) + log10(1013.246);
    }
    else
    {
        Y = 273.16 / (temperature + 273.16);
        x = -9.09718 * (Y - 1) - 3.56654 * log10(Y) + 0.876793 * (1 - (1 / Y)) + log10(6.1071);
    }
    Es = pow(10, x);
    return Es;
}

double
NYUPropagationLossModel::GetH2oPermittivity(double v, bool ice) const
{
    double Eps = 3.15;
    if (!ice)
    {
        Eps = 103.3 * (v - 1) + 77.66;
    }
    return Eps;
}

double
NYUPropagationLossModel::GetO2Lines(double freqGHz, double v, double pd, double e) const
{
    double freqO2[44];
    double a1[44];
    double a2[44];
    double a3[44];
    double a4[44];
    double a5[44];
    double a6[44];
    double p = 0;
    double s = 0;
    double gamma = 0;
    double delta = 0;
    std::complex<double> zf(0, 0);
    std::complex<double> zn(0, 0);

    for (int i = 0; i < 44; i++)
    {
        freqO2[i] = oxygen[i][0];
        a1[i] = oxygen[i][1];
        a2[i] = oxygen[i][2];
        a3[i] = oxygen[i][3];
        a4[i] = oxygen[i][4];
        a5[i] = oxygen[i][5];
        a6[i] = oxygen[i][6];
    }

    p = pd + e;

    for (int k = 0; k < 44; k++)
    {
        s = a1[k] * pd * pow(v, 3) * exp(a2[k] * (1 - v)) * pow(10, -6);
        gamma = a3[k] * (pd * pow(v, (0.8 - a4[k])) + 1.1 * e * v) * pow(10, -3);
        gamma = pow((pow(gamma, 2) + pow(25 * 0.6 * pow(10, -4), 2)), 0.5);
        delta = (a5[k] + a6[k] * v) * p * (pow(v, 0.8))*pow(10, -3);
        zf =
            freqGHz / freqO2[k] *
            ((std::complex<double>(1, 0) - std::complex<double>(0, delta)) /
                 (std::complex<double>((freqO2[k] - freqGHz), 0) - std::complex<double>(0, gamma)) -
             (std::complex<double>(1, 0) + std::complex<double>(0, delta)) /
                 (std::complex<double>((freqO2[k] + freqGHz), 0) + std::complex<double>(0, gamma)));
        zn = zn + s * zf;
    }
    return imag(zn);
}

double
NYUPropagationLossModel::GetDryCont(double freqGHz, double v, double pd, double e) const
{
    double p = 0;
    double so = 0;
    double gammao = 0;
    double sn = 0;
    std::complex<double> zfo(0, 0);
    std::complex<double> zfn(0, 0);
    std::complex<double> zn(0, 0);

    p = pd + e;
    so = 6.14 * pow(10, -5) * pd * pow(v, 2);
    gammao = 0.56 * pow(10, -3) * p * pow(v, 0.8);
    zfo = -freqGHz / (std::complex<double>(freqGHz, gammao));
    sn = 1.40 * pow(10, -12) * pow(pd, 2) * pow(v, 3.5);
    zfn = std::complex<double>(0, freqGHz) / (1.93 * pow(10, -5) * pow(freqGHz, 1.5) + 1);
    zn = so * zfo + sn * zfn;
    return imag(zn);
}

double
NYUPropagationLossModel::GetH2oVapor(double freqGHz, double v, double pd, double e) const
{
    double freqH2o[44];
    double b1[44];
    double b2[44];
    double b3[44];
    double b4[44];
    double b5[44];
    double b6[44];
    double s = 0;
    double gamh = 0;
    double gamd2 = 0;
    double delh = 0;
    std::complex<double> zf(0, 0);
    std::complex<double> zn(0, 0);

    for (int i = 0; i < 35; i++)
    {
        freqH2o[i] = water[i][0];
        b1[i] = water[i][1];
        b2[i] = water[i][2];
        b3[i] = water[i][3];
        b4[i] = water[i][4];
        b5[i] = water[i][5];
        b6[i] = water[i][6];
    }

    for (int k = 0; k < 35; k++)
    {
        s = b1[k] * e * pow(v, 3.5) * exp(b2[k] * (1 - v));
        gamh = b3[k] * (pd * pow(v, b5[k]) + b4[k] * e * pow(v, b6[k])) * pow(10, -3);
        gamd2 = pow(10, -12) / (v * pow(1.46 * freqH2o[k], 2));
        gamh = 0.535 * gamh + pow((0.217 * pow(gamh, 2) + gamd2), 0.5);
        delh = 0;
        zf = freqGHz / freqH2o[k] *
             ((std::complex<double>(1, -delh)) /
                  (std::complex<double>(freqH2o[k] - freqGHz, -gamh)) -
              (std::complex<double>(1, delh)) / (std::complex<double>(freqH2o[k] + freqGHz, gamh)));
        zn = zn + s * zf;
    }
    return imag(zn);
}

double
NYUPropagationLossModel::GetH2oLiquid(double freqGHz, double v, double w, bool ice, double eps)
    const
{
    NS_LOG_FUNCTION(this << freqGHz << v << w << ice << eps);
    double fd = 0;
    double fs = 0;
    double epinf = 0;
    double eopt = 0;
    double ai = 0;
    double bi = 0;
    double fice = 0;
    std::complex<double> zep(0, 0);
    std::complex<double> znw(0, 0);

    if (!ice)
    {
        fd = 20.20 - 146.4 * (v - 1) + 316 * pow((v - 1), 2);
        fs = 39.8 * fd;
        epinf = 0.0671 * eps;
        eopt = 3.52;
        zep = eps - freqGHz * ((eps - epinf) / (std::complex<double>(freqGHz, fd)) +
                               (epinf - eopt) / (std::complex<double>(freqGHz, fs)));
    }
    else
    {
        ai = (62 * v - 11.6) * exp(-22.1 * (v - 1)) * pow(10, -4);
        bi = 0.542 * pow(10, -6) * (-24.17 + 116.79 / v + pow((v / (v - 0.9927)), 2));
        if (freqGHz < 0.001)
        {
            fice = 0.001;
        }
        else
        {
            fice = freqGHz;
        }
        zep = std::complex<double>(3.15, (ai / fice + bi * fice));
    }
    znw = std::complex<double>((1.5 * w), 0) *
          ((zep - std::complex<double>(1, 0)) / (zep + std::complex<double>(2, 0)) -
           std::complex<double>(1, 0) +
           std::complex<double>(3, 0) / (std::complex<double>(eps + 2, 0)));
    return imag(znw);
}

double
NYUPropagationLossModel::GetRainAttenuation(double freqGHz, double rainRate) const
{
    double atRain = 0;
    double ea = 0;
    double ga = 0;
    double eb = 0;
    double gb = 0;
    double arain = 0;
    double brain = 0;
    double fr = 0;
    double nro = 0;
    double x = 0;
    double nrp = 0;
    std::complex<double> znrp(0, 0);

    if (rainRate == 0)
    {
        atRain = 0;
        nrp = 0;
    }
    else
    {
        if (freqGHz < 2.9)
        {
            ga = 6.39e-5;
            ea = 2.03;
        }
        else if (freqGHz < 54)
        {
            ea = 2.42;
            ga = 4.21e-5;
        }
        else if (freqGHz < 180)
        {
            ea = 0.699;
            ga = 4.09e-2;
        }
        else
        {
            ea = -0.151;
            ga = 3.38;
        }
        arain = ga * pow(freqGHz, ea);

        if (freqGHz < 8.5)
        {
            eb = 0.158;
            gb = 0.851;
        }
        else if (freqGHz < 25)
        {
            eb = -0.0779;
            gb = 1.41;
        }
        else if (freqGHz < 164)
        {
            eb = -0.272;
            gb = 2.63;
        }
        else
        {
            eb = 0.0126;
            gb = 0.616;
        }
        brain = gb * pow(freqGHz, eb);
        atRain = arain * pow(rainRate, brain);

        fr = 53 - 0.37 * rainRate + 1.5e-3 * pow(rainRate, 2);
        nro = (rainRate * (3.68 - 0.012 * rainRate)) / fr;
        x = freqGHz / fr;
        nrp = -nro * (pow(x, 2.5) / (1 + pow(x, 2.5)));
    }

    znrp = nrp + std::complex<double>(0, (atRain / (0.182 * freqGHz)));
    return imag(znrp);
}

double
NYUPropagationLossModel::GetNonDispRef(double v,
                                       double pd,
                                       double e,
                                       double rainRate,
                                       bool w,
                                       double eps) const
{
    double n0 = 0;
    n0 = (0.2588 * pd + (4.163 * v + 0.239) * e) * v + 1.5 * w * (1 - 3 / (eps + 2)) +
         rainRate * (3.68 - 0.012 * rainRate) / (53 - 0.37 * rainRate + 0.0015 * pow(rainRate, 2));
    return n0;
}

double
NYUPropagationLossModel::GetLoss(Ptr<ChannelCondition> cond, double distance2D, double hBs) const
{
    NS_LOG_FUNCTION(this);
    double loss = 0;
    if (cond->GetLosCondition() == ChannelCondition::LosConditionValue::LOS)
    {
        loss = GetLossLos(distance2D, hBs);
    }
    else if (cond->GetLosCondition() == ChannelCondition::LosConditionValue::NLOS)
    {
        loss = GetLossNlos(distance2D, hBs);
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }
    return loss;
}

std::pair<double, double>
NYUPropagationLossModel::GetUtAndBsHeights(double za, double zb) const
{
    // The default implementation assumes that the tallest node is the BS and the
    // smallest is the UT.
    double hUt = std::min(za, zb);
    double hBs = std::max(za, zb);

    return std::pair<double, double>(hUt, hBs);
}

double
NYUPropagationLossModel::Calculate2dDistance(Vector a, Vector b)
{
    double x = a.x - b.x;
    double y = a.y - b.y;
    double distance2D = sqrt(x * x + y * y);

    return distance2D;
}

uint32_t
NYUPropagationLossModel::GetKey(Ptr<MobilityModel> a, Ptr<MobilityModel> b)
{
    // use the nodes ids to obtain an unique key for the channel between a and b
    // sort the nodes ids so that the key is reciprocal
    uint32_t x1 = std::min(a->GetObject<Node>()->GetId(), b->GetObject<Node>()->GetId());
    uint32_t x2 = std::max(a->GetObject<Node>()->GetId(), b->GetObject<Node>()->GetId());

    // use the cantor function to obtain the key
    uint32_t key = (((x1 + x2) * (x1 + x2 + 1)) / 2) + x2;

    return key;
}

Vector
NYUPropagationLossModel::GetVectorDifference(Ptr<MobilityModel> a, Ptr<MobilityModel> b)
{
    uint32_t x1 = a->GetObject<Node>()->GetId();
    uint32_t x2 = b->GetObject<Node>()->GetId();

    if (x1 < x2)
    {
        return b->GetPosition() - a->GetPosition();
    }
    else
    {
        return a->GetPosition() - b->GetPosition();
    }
}

double
NYUPropagationLossModel::GetShadowing(Ptr<MobilityModel> a,
                                      Ptr<MobilityModel> b,
                                      ChannelCondition::LosConditionValue cond) const
{
    NS_LOG_FUNCTION(this);
    double shadowingValue;

    // compute the channel key
    uint32_t key = GetKey(a, b);
    bool notFound = false;          // indicates if the shadowing value has not been computed yet
    bool newCondition = false;      // indicates if the channel condition has changed
    Vector newDistance;             // the distance vector, that is not a distance but a difference
    auto it = m_shadowingMap.end(); // the shadowing map iterator
    if (m_shadowingMap.find(key) != m_shadowingMap.end())
    {
        // found the shadowing value in the map
        it = m_shadowingMap.find(key);
        newDistance = GetVectorDifference(a, b);
        newCondition = (it->second.m_condition != cond); // true if the condition changed
    }
    else
    {
        notFound = true;
        // add a new entry in the map and update the iterator
        ShadowingMapItem newItem;
        it = m_shadowingMap.insert(it, std::make_pair(key, newItem));
    }
    if (notFound || newCondition)
    {
        // generate a new independent realization
        shadowingValue = GetShadowingStd(cond) * m_normRandomVariable->GetValue();
    }
    else
    {
        // compute a new correlated shadowing loss - as per 3GPP
        Vector2D displacement(newDistance.x - it->second.m_distance.x,
                              newDistance.y - it->second.m_distance.y);
        double R = exp(-1 * displacement.GetLength() / GetShadowingCorrelationDistance(cond));
        shadowingValue = R * it->second.m_shadowing +
                         sqrt(1 - R * R) * m_normRandomVariable->GetValue() * GetShadowingStd(cond);
    }

    // update the entry in the map
    it->second.m_shadowing = shadowingValue;
    it->second.m_distance = newDistance; // Save the (0,0,0) vector in case it's the first time we
                                         // are calculating this value
    it->second.m_condition = cond;

    NS_LOG_DEBUG("shadowingValue: " << shadowingValue);
    return shadowingValue;
}

double
NYUPropagationLossModel::GetCalibratedParameter(double ple1, double ple2, double frequency) const
{
    NS_LOG_FUNCTION(this << ple1 << ple2 << frequency);

    double freqGHz = frequency / 1e9; // freq in GHz
    double calibrateValue = 0;

    if (freqGHz < lowerLimitFrequency)
    {
        calibrateValue = ple1;
    }
    else if (freqGHz > higherLimitFrequency)
    {
        calibrateValue = ple2;
    }
    else
    {
        calibrateValue = freqGHz * (ple2 - ple1) / (higherLimitFrequency - lowerLimitFrequency) +
                         (5 * ple1 - ple2) / 4;
    }
    return calibrateValue;
}

double
NYUPropagationLossModel::GetO2IPathLoss(const std::string& o2iLossType, double frequency) const
{
    NS_LOG_FUNCTION(this);

    double freqGHz = frequency / 1e9;
    double o2iLoss = 0;

    if (o2iLossType == "Low Loss")
    {
        o2iLoss = 10 * log10(5 + 0.03 * pow(freqGHz, 2)) + 4 * m_normRandomVariable->GetValue();
    }
    else if (o2iLossType == "High Loss")
    {
        o2iLoss = 10 * log10(10 + 5 * pow(freqGHz, 2)) + 6 * m_normRandomVariable->GetValue();
    }
    else
    {
        NS_FATAL_ERROR("Unknown O2I Loss Type");
    }
    return o2iLoss;
}

double
NYUPropagationLossModel::GetFoliagePathLoss(double distance2D) const
{
    NS_LOG_FUNCTION(this << distance2D);

    double foliagePathLoss = 0;
    foliagePathLoss = m_foliageLoss * m_uniformVar->GetValue(0, distance2D);
    return foliagePathLoss;
}

NS_OBJECT_ENSURE_REGISTERED(NYUUmiPropagationLossModel);

TypeId
NYUUmiPropagationLossModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NYUUmiPropagationLossModel")
                            .SetParent<NYUPropagationLossModel>()
                            .SetGroupName("Propagation")
                            .AddConstructor<NYUUmiPropagationLossModel>();
    return tid;
}

NYUUmiPropagationLossModel::NYUUmiPropagationLossModel()
    : NYUPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
    // set a default channel condition model
    m_channelConditionModel = CreateObject<NYUUmiChannelConditionModel>();
}

NYUUmiPropagationLossModel::~NYUUmiPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
}

double
NYUUmiPropagationLossModel::GetLossLos(double distance2D, double hBs) const
{
    NS_LOG_FUNCTION(this);

    double lambda;                                          // wavelength in meters
    double freeSpacePathLoss;                               // Free Space Path Loss (FSPL)
    double pathLossLos = 0;                                 // Path loss without SF (dB)
    double ple = GetCalibratedParameter(2, 2, m_frequency); // Path Loss Exponent (UMi LOS)

    lambda = M_C / (m_frequency);

    freeSpacePathLoss = 20 * log10(4 * M_PI * refdistance / lambda);

    pathLossLos = freeSpacePathLoss + 10 * ple * log10(distance2D);

    NS_LOG_DEBUG("m_frequency: " << m_frequency << " 2d-distance: " << distance2D
                                 << " labmda: " << lambda << " FSPL: " << freeSpacePathLoss
                                 << " pathLossLos: " << pathLossLos << " scenario:"
                                 << "Umi LOS");
    return pathLossLos;
}

double
NYUUmiPropagationLossModel::GetLossNlos(double distance2D, double hBs) const
{
    NS_LOG_FUNCTION(this);

    double lambda;                                              // wavelength in meters
    double freeSpacePathLoss;                                   // Free Space Path Loss (FSPL)
    double pathLossNlos = 0;                                    // Path loss without SF (dB)
    double ple = GetCalibratedParameter(3.2, 2.9, m_frequency); // Path Loss Exponent (UMi NLOS)

    lambda = M_C / (m_frequency);

    freeSpacePathLoss = 20 * log10(4 * M_PI * refdistance / lambda);

    pathLossNlos = freeSpacePathLoss + 10 * ple * log10(distance2D);

    NS_LOG_DEBUG("m_frequency: " << m_frequency << " 2d-distance: " << distance2D
                                 << " labmda: " << lambda << " FSPL: " << freeSpacePathLoss
                                 << " pathLossNlos: " << pathLossNlos << " scenario:"
                                 << "Umi NLOS");

    return pathLossNlos;
}

double
NYUUmiPropagationLossModel::GetShadowingStd(ChannelCondition::LosConditionValue cond) const
{
    NS_LOG_FUNCTION(this);
    double shadowingStd;
    if (cond == ChannelCondition::LosConditionValue::LOS)
    {
        shadowingStd = GetCalibratedParameter(4.0, 2.6, m_frequency);
    }
    else if (cond == ChannelCondition::LosConditionValue::NLOS)
    {
        shadowingStd = GetCalibratedParameter(7.0, 8.2, m_frequency);
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }

    NS_LOG_DEBUG("shadowingStd " << shadowingStd);
    return shadowingStd;
}

double
NYUUmiPropagationLossModel::GetShadowingCorrelationDistance(
    ChannelCondition::LosConditionValue cond) const
{
    NS_LOG_FUNCTION(this);
    double correlationDistance;

    // See 3GPP TR 38.901, Table 7.5-6
    if (cond == ChannelCondition::LosConditionValue::LOS)
    {
        correlationDistance = 10;
    }
    else if (cond == ChannelCondition::LosConditionValue::NLOS)
    {
        correlationDistance = 13;
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }

    return correlationDistance;
}

NS_OBJECT_ENSURE_REGISTERED(NYUInHPropagationLossModel);

TypeId
NYUInHPropagationLossModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NYUInHPropagationLossModel")
                            .SetParent<NYUPropagationLossModel>()
                            .SetGroupName("Propagation")
                            .AddConstructor<NYUInHPropagationLossModel>();
    return tid;
}

NYUInHPropagationLossModel::NYUInHPropagationLossModel()
    : NYUPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
    // set a default channel condition model
    m_channelConditionModel = CreateObject<NYUInHChannelConditionModel>();
}

NYUInHPropagationLossModel::~NYUInHPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
}

double
NYUInHPropagationLossModel::GetLossLos(double distance2D, double hBs) const
{
    NS_LOG_FUNCTION(this);

    double lambda;            // wavelength in meters
    double freeSpacePathLoss; // Free Space Path Loss (FSPL)
    double pathLossLos = 0;   // Path loss without SF (dB)
    double ple = 0;

    // Frequency dependent PLE for InH Los
    if (m_frequency < 28e9)
    {
        ple = m_frequency / 1e9 * (1.2 - 1.8) / (28 - 1) + (28 * 1.8 - 1.2) / 27;
    }
    else
    {
        ple = GetCalibratedParameter(1.2, 1.8, m_frequency); // Path Loss Exponent InH LOS
    }

    lambda = M_C / (m_frequency);

    freeSpacePathLoss = 20 * log10(4 * M_PI * refdistance / lambda);

    pathLossLos = freeSpacePathLoss + 10 * ple * log10(distance2D);

    NS_LOG_DEBUG("m_frequency: " << m_frequency << " 2d-distance: " << distance2D
                                 << " labmda: " << lambda << " FSPL: " << freeSpacePathLoss
                                 << " pathLossLos: " << pathLossLos << " scenario:"
                                 << "InH LOS");
    return pathLossLos;
}

double
NYUInHPropagationLossModel::GetLossNlos(double distance2D, double hBs) const
{
    NS_LOG_FUNCTION(this);

    double lambda;                                              // wavelength in meters
    double freeSpacePathLoss;                                   // Free Space Path Loss (FSPL)
    double pathLossNlos = 0;                                    // Path loss without SF (dB)
    double ple = GetCalibratedParameter(2.7, 2.7, m_frequency); // Path Loss Exponent InH NLOS

    lambda = M_C / (m_frequency);

    freeSpacePathLoss = 20 * log10(4 * M_PI * refdistance / lambda);

    pathLossNlos = freeSpacePathLoss + 10 * ple * log10(distance2D);

    NS_LOG_DEBUG("m_frequency: " << m_frequency << " 2d-distance: " << distance2D
                                 << " labmda: " << lambda << " FSPL: " << freeSpacePathLoss
                                 << " pathLossNlos: " << pathLossNlos << " scenario:"
                                 << "InH NLOS");

    return pathLossNlos;
}

double
NYUInHPropagationLossModel::GetShadowingStd(ChannelCondition::LosConditionValue cond) const
{
    NS_LOG_FUNCTION(this);
    double shadowingStd;
    if (cond == ChannelCondition::LosConditionValue::LOS)
    {
        shadowingStd = GetCalibratedParameter(3, 2.9, m_frequency);
    }
    else if (cond == ChannelCondition::LosConditionValue::NLOS)
    {
        shadowingStd = GetCalibratedParameter(9.8, 6.6, m_frequency);
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }
    return shadowingStd;
}

double
NYUInHPropagationLossModel::GetShadowingCorrelationDistance(
    ChannelCondition::LosConditionValue cond) const
{
    NS_LOG_FUNCTION(this);
    double correlationDistance;

    // See 3GPP TR 38.901, Table 7.5-6
    if (cond == ChannelCondition::LosConditionValue::LOS)
    {
        correlationDistance = 10;
    }
    else if (cond == ChannelCondition::LosConditionValue::NLOS)
    {
        correlationDistance = 6;
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }

    return correlationDistance;
}

NS_OBJECT_ENSURE_REGISTERED(NYUUmaPropagationLossModel);

TypeId
NYUUmaPropagationLossModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NYUUmaPropagationLossModel")
                            .SetParent<NYUPropagationLossModel>()
                            .SetGroupName("Propagation")
                            .AddConstructor<NYUUmaPropagationLossModel>();
    return tid;
}

NYUUmaPropagationLossModel::NYUUmaPropagationLossModel()
    : NYUPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
    // set a default channel condition model
    m_channelConditionModel = CreateObject<NYUUmaChannelConditionModel>();
}

NYUUmaPropagationLossModel::~NYUUmaPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
}

double
NYUUmaPropagationLossModel::GetLossLos(double distance2D, double hBs) const
{
    NS_LOG_FUNCTION(this);

    double lambda;                                          // wavelength in meters
    double freeSpacePathLoss;                               // Free Space Path Loss (FSPL)
    double pathLossLos = 0;                                 // Path loss without SF (dB)
    double ple = GetCalibratedParameter(2, 2, m_frequency); // Path Loss Exponent UMa LOS

    lambda = M_C / (m_frequency);

    freeSpacePathLoss = 20 * log10(4 * M_PI * refdistance / lambda);

    pathLossLos = freeSpacePathLoss + 10 * ple * log10(distance2D);

    NS_LOG_DEBUG("m_frequency: " << m_frequency << " 2d-distance: " << distance2D
                                 << " labmda: " << lambda << " FSPL: " << freeSpacePathLoss
                                 << " pathLossLos: " << pathLossLos << " scenario:"
                                 << "Uma LOS");
    return pathLossLos;
}

double
NYUUmaPropagationLossModel::GetLossNlos(double distance2D, double hBs) const
{
    NS_LOG_FUNCTION(this);

    double lambda;                                              // wavelength in meters
    double freeSpacePathLoss;                                   // Free Space Path Loss (FSPL)
    double pathLossNlos = 0;                                    // Path loss without SF (dB)
    double ple = GetCalibratedParameter(2.9, 2.9, m_frequency); // Path Loss Exponent UMa NLOS

    lambda = M_C / (m_frequency);

    freeSpacePathLoss = 20 * log10(4 * M_PI * refdistance / lambda);

    pathLossNlos = freeSpacePathLoss + 10 * ple * log10(distance2D);

    NS_LOG_DEBUG("m_frequency: " << m_frequency << " 2d-distance: " << distance2D
                                 << " labmda: " << lambda << " FSPL: " << freeSpacePathLoss
                                 << " pathLossNlos: " << pathLossNlos << " scenario:"
                                 << "Uma NLOS");

    return pathLossNlos;
}

double
NYUUmaPropagationLossModel::GetShadowingStd(ChannelCondition::LosConditionValue cond) const
{
    NS_LOG_FUNCTION(this);
    double shadowingStd;
    if (cond == ChannelCondition::LosConditionValue::LOS)
    {
        shadowingStd = GetCalibratedParameter(4.0, 2.6, m_frequency);
    }
    else if (cond == ChannelCondition::LosConditionValue::NLOS)
    {
        shadowingStd = GetCalibratedParameter(7.0, 8.2, m_frequency);
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }
    return shadowingStd;
}

double
NYUUmaPropagationLossModel::GetShadowingCorrelationDistance(
    ChannelCondition::LosConditionValue cond) const
{
    NS_LOG_FUNCTION(this);
    double correlationDistance;

    // See 3GPP TR 38.901, Table 7.5-6
    if (cond == ChannelCondition::LosConditionValue::LOS)
    {
        correlationDistance = 37;
    }
    else if (cond == ChannelCondition::LosConditionValue::NLOS)
    {
        correlationDistance = 50;
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }

    return correlationDistance;
}

NS_OBJECT_ENSURE_REGISTERED(NYURmaPropagationLossModel);

TypeId
NYURmaPropagationLossModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NYURmaPropagationLossModel")
                            .SetParent<NYUPropagationLossModel>()
                            .SetGroupName("Propagation")
                            .AddConstructor<NYURmaPropagationLossModel>();
    return tid;
}

NYURmaPropagationLossModel::NYURmaPropagationLossModel()
    : NYUPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
    // set a default channel condition model
    m_channelConditionModel = CreateObject<NYURmaChannelConditionModel>();
}

NYURmaPropagationLossModel::~NYURmaPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
}

double
NYURmaPropagationLossModel::GetLossLos(double distance2D, double hBs) const
{
    NS_LOG_FUNCTION(this);

    double lambda;            // wavelength in meters
    double freeSpacePathLoss; // Free Space Path Loss (FSPL)
    double pathLossLos = 0;   // Path loss without SF (dB)

    lambda = M_C / (m_frequency);

    freeSpacePathLoss = 20 * log10(4 * M_PI * refdistance / lambda);

    pathLossLos = freeSpacePathLoss + 23.1 * (1 - 0.03 * ((hBs - 35) / 35)) * log10(distance2D);

    NS_LOG_DEBUG("m_frequency: " << m_frequency << " 2d-distance: " << distance2D
                                 << " labmda: " << lambda << " FSPL: " << freeSpacePathLoss
                                 << " pathLossLos: " << pathLossLos << " scenario:"
                                 << "Rma LOS");
    return pathLossLos;
}

double
NYURmaPropagationLossModel::GetLossNlos(double distance2D, double hBs) const
{
    NS_LOG_FUNCTION(this);

    double lambda;            // wavelength in meters
    double freeSpacePathLoss; // Free Space Path Loss (FSPL)
    double pathLossNlos = 0;  // Path loss without SF (dB)

    lambda = M_C / (m_frequency);

    freeSpacePathLoss = 20 * log10(4 * M_PI * refdistance / lambda);

    pathLossNlos = freeSpacePathLoss + 30.7 * (1 - 0.049 * ((hBs - 35) / 35)) * log10(distance2D);

    NS_LOG_DEBUG("m_frequency: " << m_frequency << " 2d-distance: " << distance2D
                                 << " labmda: " << lambda << " FSPL: " << freeSpacePathLoss
                                 << " pathLossNlos: " << pathLossNlos << " scenario:"
                                 << "Rma NLOS");

    return pathLossNlos;
}

double
NYURmaPropagationLossModel::GetShadowingStd(ChannelCondition::LosConditionValue cond) const
{
    NS_LOG_FUNCTION(this);
    double shadowingStd;
    if (cond == ChannelCondition::LosConditionValue::LOS)
    {
        shadowingStd = GetCalibratedParameter(1.7, 1.7, m_frequency);
    }
    else if (cond == ChannelCondition::LosConditionValue::NLOS)
    {
        shadowingStd = GetCalibratedParameter(6.7, 6.7, m_frequency);
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }
    return shadowingStd;
}

double
NYURmaPropagationLossModel::GetShadowingCorrelationDistance(
    ChannelCondition::LosConditionValue cond) const
{
    NS_LOG_FUNCTION(this);
    double correlationDistance;

    // See 3GPP TR 38.901, Table 7.5-6
    if (cond == ChannelCondition::LosConditionValue::LOS)
    {
        correlationDistance = 37;
    }
    else if (cond == ChannelCondition::LosConditionValue::NLOS)
    {
        correlationDistance = 120;
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }
    return correlationDistance;
}

NS_OBJECT_ENSURE_REGISTERED(NYUInFPropagationLossModel);

TypeId
NYUInFPropagationLossModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NYUInFPropagationLossModel")
                            .SetParent<NYUPropagationLossModel>()
                            .SetGroupName("Propagation")
                            .AddConstructor<NYUInFPropagationLossModel>();
    return tid;
}

NYUInFPropagationLossModel::NYUInFPropagationLossModel()
    : NYUPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
    // set a default channel condition model
    m_channelConditionModel = CreateObject<NYUInFChannelConditionModel>();
}

NYUInFPropagationLossModel::~NYUInFPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
}

double
NYUInFPropagationLossModel::GetLossLos(double distance2D, double hBs) const
{
    NS_LOG_FUNCTION(this);

    double lambda;                                              // wavelength in meters
    double freeSpacePathLoss;                                   // Free Space Path Loss (FSPL)
    double pathLossLos = 0;                                     // Path loss without SF (dB)
    double ple = GetCalibratedParameter(1.7, 1.7, m_frequency); // Path Loss Exponent InF LOS

    lambda = M_C / (m_frequency);

    freeSpacePathLoss = 20 * log10(4 * M_PI * refdistance / lambda);

    pathLossLos = freeSpacePathLoss + 10 * ple * log10(distance2D);

    NS_LOG_DEBUG("m_frequency: " << m_frequency << " 2d-distance: " << distance2D
                                 << " labmda: " << lambda << " FSPL: " << freeSpacePathLoss
                                 << " pathLossLos: " << pathLossLos << " scenario:"
                                 << "InF LOS");
    return pathLossLos;
}

double
NYUInFPropagationLossModel::GetLossNlos(double distance2D, double hBs) const
{
    NS_LOG_FUNCTION(this);

    double lambda;                                              // wavelength in meters
    double freeSpacePathLoss;                                   // Free Space Path Loss (FSPL)
    double pathLossNlos = 0;                                    // Path loss without SF (dB)
    double ple = GetCalibratedParameter(3.1, 3.1, m_frequency); // Path Loss Exponent InF NLOS

    lambda = M_C / (m_frequency);

    freeSpacePathLoss = 20 * log10(4 * M_PI * refdistance / lambda);

    pathLossNlos = freeSpacePathLoss + 10 * ple * log10(distance2D);

    NS_LOG_DEBUG("m_frequency: " << m_frequency << " 2d-distance: " << distance2D
                                 << " labmda: " << lambda << " FSPL: " << freeSpacePathLoss
                                 << " pathLossNlos: " << pathLossNlos << " scenario:"
                                 << "InF NLOS");

    return pathLossNlos;
}

double
NYUInFPropagationLossModel::GetShadowingStd(ChannelCondition::LosConditionValue cond) const
{
    NS_LOG_FUNCTION(this);
    double shadowingStd;
    if (cond == ChannelCondition::LosConditionValue::LOS)
    {
        shadowingStd = GetCalibratedParameter(3.0, 3.0, m_frequency);
    }
    else if (cond == ChannelCondition::LosConditionValue::NLOS)
    {
        shadowingStd = GetCalibratedParameter(7.0, 7.0, m_frequency);
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }
    return shadowingStd;
}

double
NYUInFPropagationLossModel::GetShadowingCorrelationDistance(
    ChannelCondition::LosConditionValue cond) const
{
    NS_LOG_FUNCTION(this);
    double correlationDistance;

    // See 3GPP TR 38.901, Table 7.5-6
    if (cond == ChannelCondition::LosConditionValue::LOS ||
        cond == ChannelCondition::LosConditionValue::NLOS)
    {
        correlationDistance = 10;
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }

    return correlationDistance;
}

} // namespace ns3
