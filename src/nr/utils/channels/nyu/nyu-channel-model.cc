// Copyright (c) 2023 New York University and NYU WIRELESS
// Users are encouraged to cite NYU WIRELESS publications regarding this work.
// Original source code is available in https://github.com/hiteshPoddar/NYUSIM_in_ns3
//
// SPDX-License-Identifier: MIT
#include "nyu-channel-model.h"

#include "ns3/angles.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include "ns3/node.h"
#include "ns3/phased-array-model.h"
#include "ns3/pointer.h"
#include "ns3/string.h"
#include <ns3/simulator.h>

#include <algorithm>
#include <complex>
#include <math.h>
#include <random>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NYUChannelModel");

NS_OBJECT_ENSURE_REGISTERED(NYUChannelModel);

static const double M_C = 3.0e8;               // in m/s
static const double frequencyLowerBound = 28;  // in GHz
static const double frequencyUpperBound = 140; // in GHz

NYUChannelModel::NYUChannelModel()
{
    NS_LOG_FUNCTION(this);
    m_normalRv = CreateObject<NormalRandomVariable>();
    m_normalRv->SetAttribute("Mean", DoubleValue(0.0));
    m_normalRv->SetAttribute("Variance", DoubleValue(1.0));
    m_uniformRv = CreateObject<UniformRandomVariable>();
    m_expRv = CreateObject<ExponentialRandomVariable>();
    m_gammaRv = CreateObject<GammaRandomVariable>();
}

NYUChannelModel::~NYUChannelModel()
{
    NS_LOG_FUNCTION(this);
}

void
NYUChannelModel::DoDispose()
{
    NS_LOG_FUNCTION(this);
    if (m_channelConditionModel)
    {
        m_channelConditionModel->Dispose();
    }
    m_channelMatrixMap.clear();
    m_channelParamsMap.clear();
    m_channelConditionModel = nullptr;
}

TypeId
NYUChannelModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NYUChannelModel")
            .SetGroupName("Spectrum")
            .SetParent<MatrixBasedChannelModel>()
            .AddConstructor<NYUChannelModel>()
            .AddAttribute(
                "Frequency",
                "The operating Frequency in Hz",
                DoubleValue(140.0e9),
                MakeDoubleAccessor(&NYUChannelModel::SetFrequency, &NYUChannelModel::GetFrequency),
                MakeDoubleChecker<double>())
            .AddAttribute("RfBandwidth",
                          "The Bandwidth in Hz",
                          DoubleValue(500e6),
                          MakeDoubleAccessor(&NYUChannelModel::SetRfBandwidth,
                                             &NYUChannelModel::GetRfBandwidth),
                          MakeDoubleChecker<double>())
            .AddAttribute(
                "Scenario",
                "The NYU scenario (RMa,UMa,UMi-StreetCanyon,InH and InF))",
                StringValue("RMa"),
                MakeStringAccessor(&NYUChannelModel::SetScenario, &NYUChannelModel::GetScenario),
                MakeStringChecker())
            .AddAttribute("ChannelConditionModel",
                          "Pointer to the channel condition model",
                          PointerValue(),
                          MakePointerAccessor(&NYUChannelModel::SetChannelConditionModel,
                                              &NYUChannelModel::GetChannelConditionModel),
                          MakePointerChecker<ChannelConditionModel>())
            .AddAttribute("UpdatePeriod",
                          "Specify the channel coherence time",
                          TimeValue(MilliSeconds(0)),
                          MakeTimeAccessor(&NYUChannelModel::m_updatePeriod),
                          MakeTimeChecker())
            .AddAttribute("Blockage",
                          "Enable NYU blockage model",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NYUChannelModel::m_blockage),
                          MakeBooleanChecker());
    return tid;
}

void
NYUChannelModel::SetChannelConditionModel(Ptr<ChannelConditionModel> model)
{
    NS_LOG_FUNCTION(this);
    m_channelConditionModel = model;
}

Ptr<ChannelConditionModel>
NYUChannelModel::GetChannelConditionModel() const
{
    NS_LOG_FUNCTION(this);
    return m_channelConditionModel;
}

void
NYUChannelModel::SetFrequency(double freq)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(freq >= 500.0e6 && freq <= 150.0e9,
                  "Frequency should be between 0.5 and 150 GHz but is " << freq);
    m_frequency = freq;
}

double
NYUChannelModel::GetFrequency() const
{
    NS_LOG_FUNCTION(this);
    return m_frequency;
}

void
NYUChannelModel::SetRfBandwidth(double rfBandwidth)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(rfBandwidth >= 0 && rfBandwidth <= 1000e6,
                  "Bandwidth should be between 0 and 1000 MHz GHz but is " << rfBandwidth);
    m_rfBandwidth = rfBandwidth;
}

double
NYUChannelModel::GetRfBandwidth() const
{
    NS_LOG_FUNCTION(this);
    return m_rfBandwidth;
}

void
NYUChannelModel::SetScenario(const std::string& scenario)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(scenario == "RMa" || scenario == "UMa" || scenario == "UMi-StreetCanyon" ||
                      scenario == "InH" || scenario == "InF",
                  "Unknown scenario, choose between: RMa, UMa, UMi-StreetCanyon, InH, InF");
    m_scenario = scenario;
}

std::string
NYUChannelModel::GetScenario() const
{
    NS_LOG_FUNCTION(this);
    return m_scenario;
}

/* API which does a linear interpolation of channel parameters between 0.5 GHz - 150 GHz */
double
NYUChannelModel::GetCalibratedParameter(double val1, double val2, double frequency) const
{
    NS_LOG_FUNCTION(this << val1 << val2 << frequency);
    double output = 0;
    if (frequency < frequencyLowerBound)
    {
        output = val1;
    }
    else if (frequency > frequencyUpperBound)
    {
        output = val2;
    }
    else
    {
        output = frequency * (val2 - val1) / (frequencyUpperBound - frequencyLowerBound) +
                 (5 * val1 - val2) / 4;
    }
    NS_LOG_DEBUG("Interpolated parameter value:" << output << std::endl);
    return output;
}

Ptr<const NYUChannelModel::ParamsTable>
NYUChannelModel::GetNYUTable(Ptr<const ChannelCondition> channelCondition) const
{
    NS_LOG_FUNCTION(this);

    // Frequency in GHz
    double freq = m_frequency / 1e9;
    Ptr<ParamsTable> tablenyu = Create<ParamsTable>();
    bool los = channelCondition->IsLos();

    NS_LOG_DEBUG("Channel Condition is LOS: " << los << " Frequency" << freq << " Bandwidth:"
                                              << m_rfBandwidth << " Scenario:" << m_scenario);

    // XPD Values generated from NYU Channel Model is not based on scenario.
    if (los)
    {
        tablenyu->xpdMean = 11.5 + freq * 0.10; // frequency dependent XPD Mean value
        tablenyu->xpdSd = 1.6;                  // XPD standard deviation
    }
    else
    {
        tablenyu->xpdMean = 5.5 + freq * 0.13; // frequency dependent XPD Mean value
        tablenyu->xpdSd = 1.6;                 // XPD standard deviation
    }
    if ((m_scenario == "UMi-StreetCanyon" || m_scenario == "UMa") && los)
    {
        /* Currently values used are for 28-73 GHz */
        tablenyu->maxNumberOfTimeCluster =
            GetCalibratedParameter(6, 5, freq); // maximum number of time clusters
        tablenyu->maxNumberOfSubpaths = 30; // maximum number of subpaths for frequency < 100 GHz
        tablenyu->muS = 1.8;                // maximum number of subpaths for frequency >= 100 GHz
        tablenyu->muAod = GetCalibratedParameter(1.9, 1.4, freq); // number of AOD spatial Lobes
        tablenyu->muAoa = GetCalibratedParameter(1.8, 1.2, freq); // number of AOA spatial Lobes
        tablenyu->Xmax = 0.2;
        tablenyu->muRho = 30;                                                           // in ns
        tablenyu->muTau = GetCalibratedParameter(123, 80, freq);                        // in ns
        tablenyu->minimumVoidInterval = 25;                                             // in ns
        tablenyu->sigmaCluster = GetCalibratedParameter(1, 5.34, freq);                 // in dB
        tablenyu->timeClusterGamma = GetCalibratedParameter(25.9, 40, freq);            // in ns
        tablenyu->sigmaSubpath = GetCalibratedParameter(6, 3.48, freq);                 // in dB
        tablenyu->subpathGamma = GetCalibratedParameter(16.9, 20, freq);                // in ns
        tablenyu->meanZod = GetCalibratedParameter(-12.6, -3.2, freq);                  // in degree
        tablenyu->sigmaZod = GetCalibratedParameter(5.9, 1.2, freq);                    // in degree
        tablenyu->sdOfAodRmsLobeAzimuthSpread = GetCalibratedParameter(8.5, 4.3, freq); // degree
        tablenyu->aodRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAodRmsLobeElevationSpread = GetCalibratedParameter(2.5, 0.1, freq); // degree
        tablenyu->aodRmsLobeElevationSpread = "Gaussian";
        tablenyu->meanZoa = GetCalibratedParameter(10.8, 2, freq);   // in degree
        tablenyu->sigmaZoa = GetCalibratedParameter(5.3, 2.9, freq); // in degree
        tablenyu->sdOfAoaRmsLobeAzimuthSpread =
            GetCalibratedParameter(10.5, 7.3, freq); // in degree
        tablenyu->aoaRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAoaRmsLobeElevationSpread =
            GetCalibratedParameter(11.5, 3.2, freq); // in degree
        tablenyu->aoaRmsLobeElevationSpread = "Laplacian";
        tablenyu->los = true; // Flag indicating LOS/NLOS. true implies LOS and false implies NLOS
    }
    else if (m_scenario == "UMi-StreetCanyon" && !los)
    {
        tablenyu->maxNumberOfTimeCluster =
            GetCalibratedParameter(6, 3, freq); // maximum number of time clusters
        tablenyu->maxNumberOfSubpaths = 30; // maximum number of subpaths for frequency < 100 GHz
        tablenyu->muS = 3;                  // maximum number of subpaths for frequency >= 100 GHz
        tablenyu->muAod = GetCalibratedParameter(1.5, 1.3, freq); // number of AOD spatial Lobes
        tablenyu->muAoa = GetCalibratedParameter(2.1, 2.1, freq); // number of AOA spatial Lobes
        tablenyu->Xmax = 0.5;                                     // in ns
        tablenyu->muRho = 33;                                     // in ns
        tablenyu->muTau = GetCalibratedParameter(83, 58, freq);   // in ns
        tablenyu->minimumVoidInterval = 25;                       // in ns
        tablenyu->sigmaCluster = GetCalibratedParameter(3, 4.68, freq);    // in dB
        tablenyu->timeClusterGamma = GetCalibratedParameter(51, 49, freq); // in ns
        tablenyu->sigmaSubpath = GetCalibratedParameter(6, 3.48, freq);    // in dB
        tablenyu->subpathGamma = GetCalibratedParameter(15.5, 20, freq);   // in ns
        tablenyu->meanZod = GetCalibratedParameter(-4.9, -1.6, freq);      // in degree
        tablenyu->sigmaZod = GetCalibratedParameter(4.5, 0.5, freq);       // in degree
        tablenyu->sdOfAodRmsLobeAzimuthSpread = GetCalibratedParameter(11.0, 5.0, freq); // degree
        tablenyu->aodRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAodRmsLobeElevationSpread = GetCalibratedParameter(3.0, 2.3, freq); // degree
        tablenyu->aodRmsLobeElevationSpread = "Gaussian";
        tablenyu->meanZoa = GetCalibratedParameter(3.6, 1.6, freq);                     // in degree
        tablenyu->sigmaZoa = GetCalibratedParameter(4.8, 2, freq);                      // in degree
        tablenyu->sdOfAoaRmsLobeAzimuthSpread = GetCalibratedParameter(7.5, 7.5, freq); // in degree
        tablenyu->aoaRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAoaRmsLobeElevationSpread =
            GetCalibratedParameter(6.0, 0.0, freq); // in degree
        tablenyu->aoaRmsLobeElevationSpread = "Laplacian";
        tablenyu->los = false; // Flag indicating LOS/NLOS. LOS->1, NLOS->0
    }
    else if (m_scenario == "UMa" && !los)
    {
        tablenyu->maxNumberOfTimeCluster =
            GetCalibratedParameter(6, 3, freq); // maximum number of time clusters
        tablenyu->maxNumberOfSubpaths = 30; // maximum number of subpaths for frequency < 100 GHz
        tablenyu->muS = 3;                  // maximum number of subpaths for frequency >= 100 GHz
        tablenyu->muAod = GetCalibratedParameter(1.5, 1.3, freq); // number of AOD spatial Lobes
        tablenyu->muAoa = GetCalibratedParameter(2.1, 2.1, freq); // number of AOA spatial Lobes
        tablenyu->Xmax = 0.5;                                     // in ns
        tablenyu->muRho = 33;                                     // in ns
        tablenyu->muTau = GetCalibratedParameter(83, 58, freq);   // in ns
        tablenyu->minimumVoidInterval = 25;                       // in ns
        tablenyu->sigmaCluster = GetCalibratedParameter(3, 4.68, freq);        // in dB
        tablenyu->timeClusterGamma = GetCalibratedParameter(51.0, 49.0, freq); // in ns
        tablenyu->sigmaSubpath = GetCalibratedParameter(6, 3.48, freq);        // in dB
        tablenyu->subpathGamma = GetCalibratedParameter(15.5, 20, freq);       // in ns
        tablenyu->meanZod = GetCalibratedParameter(-4.9, -1.6, freq);          // in degree
        tablenyu->sigmaZod = GetCalibratedParameter(4.5, 0.5, freq);           // in degree
        tablenyu->sdOfAodRmsLobeAzimuthSpread = GetCalibratedParameter(11.0, 5.0, freq); // degree
        tablenyu->aodRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAodRmsLobeElevationSpread = GetCalibratedParameter(3, 2.3, freq); // degree
        tablenyu->aodRmsLobeElevationSpread = "Gaussian";
        tablenyu->meanZoa = GetCalibratedParameter(3.6, 1.6, freq);                     // in degree
        tablenyu->sigmaZoa = GetCalibratedParameter(4.8, 2, freq);                      // in degree
        tablenyu->sdOfAoaRmsLobeAzimuthSpread = GetCalibratedParameter(7.5, 7.5, freq); // in degree
        tablenyu->aoaRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAoaRmsLobeElevationSpread = GetCalibratedParameter(6, 0, freq); // in degree
        tablenyu->aoaRmsLobeElevationSpread = "Laplacian";
        tablenyu->los = false; // Flag indicating LOS/NLOS. true implies LOS and false implies NLOS
    }
    else if (m_scenario == "RMa" && los)
    {
        tablenyu->maxNumberOfTimeCluster =
            round(GetCalibratedParameter(1, 1, freq)); // maximum number of time clusters
        tablenyu->maxNumberOfSubpaths =
            round(GetCalibratedParameter(2, 2, freq));               // maximum number of subpaths
        tablenyu->muAod = round(GetCalibratedParameter(1, 1, freq)); // number of AOD spatial Lobes
        tablenyu->muAoa = round(GetCalibratedParameter(1, 1, freq)); // number of AOA spatial Lobes
        tablenyu->Xmax = 0.2;                                        // in ns
        tablenyu->muRho = 30;                                        // in ns
        tablenyu->muTau = GetCalibratedParameter(123, 80, freq);     // in ns
        tablenyu->minimumVoidInterval = 25;                          // in ns
        tablenyu->sigmaCluster = GetCalibratedParameter(1, 5.34, freq);                 // in dB
        tablenyu->timeClusterGamma = GetCalibratedParameter(25.9, 40, freq);            // in ns
        tablenyu->sigmaSubpath = GetCalibratedParameter(6, 3.48, freq);                 // in dB
        tablenyu->subpathGamma = GetCalibratedParameter(16.9, 20, freq);                // in ns
        tablenyu->meanZod = GetCalibratedParameter(-12.6, -3.2, freq);                  // in degree
        tablenyu->sigmaZod = GetCalibratedParameter(5.9, 1.2, freq);                    // in degree
        tablenyu->sdOfAodRmsLobeAzimuthSpread = GetCalibratedParameter(8.5, 4.3, freq); // degree
        tablenyu->aodRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAodRmsLobeElevationSpread = GetCalibratedParameter(2.5, 0.1, freq); // degree
        tablenyu->aodRmsLobeElevationSpread = "Gaussian";
        tablenyu->meanZoa = GetCalibratedParameter(10.8, 2, freq);   // in degree
        tablenyu->sigmaZoa = GetCalibratedParameter(5.3, 2.9, freq); // in degree
        tablenyu->sdOfAoaRmsLobeAzimuthSpread =
            GetCalibratedParameter(10.5, 7.3, freq); // in degree
        tablenyu->aoaRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAoaRmsLobeElevationSpread =
            GetCalibratedParameter(11.5, 3.2, freq); // in degree
        tablenyu->aoaRmsLobeElevationSpread = "Laplacian";
        tablenyu->los = true; // Flag indicating LOS/NLOS. true implies LOS and false implies NLOS
    }
    else if (m_scenario == "RMa" && !los)
    {
        tablenyu->maxNumberOfTimeCluster =
            round(GetCalibratedParameter(1, 1, freq)); // maximum number of time clusters
        tablenyu->maxNumberOfSubpaths =
            round(GetCalibratedParameter(2, 2, freq));               // maximum number of subpaths
        tablenyu->muAod = round(GetCalibratedParameter(1, 1, freq)); // number of AOD spatial Lobes
        tablenyu->muAoa = round(GetCalibratedParameter(1, 1, freq)); // number of AOA spatial Lobes
        tablenyu->Xmax = 0.5;                                        // in ns
        tablenyu->muRho = 33;                                        // in ns
        tablenyu->muTau = GetCalibratedParameter(83, 58, freq);      // in ns
        tablenyu->minimumVoidInterval = 25;                          // in ns
        tablenyu->sigmaCluster = GetCalibratedParameter(3, 4.68, freq);        // in dB
        tablenyu->timeClusterGamma = GetCalibratedParameter(51.0, 49.0, freq); // in ns
        tablenyu->sigmaSubpath = GetCalibratedParameter(6, 3.48, freq);        // in dB
        tablenyu->subpathGamma = GetCalibratedParameter(15.5, 20, freq);       // in ns
        tablenyu->meanZod = GetCalibratedParameter(-4.9, -1.6, freq);          // in degree
        tablenyu->sigmaZod = GetCalibratedParameter(4.5, 0.5, freq);           // in degree
        tablenyu->sdOfAodRmsLobeAzimuthSpread = GetCalibratedParameter(11.0, 5.0, freq); // degree
        tablenyu->aodRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAodRmsLobeElevationSpread = GetCalibratedParameter(3, 2.3, freq); // degree
        tablenyu->aodRmsLobeElevationSpread = "Gaussian";
        tablenyu->meanZoa = GetCalibratedParameter(3.6, 1.6, freq);                     // in degree
        tablenyu->sigmaZoa = GetCalibratedParameter(4.8, 2, freq);                      // in degree
        tablenyu->sdOfAoaRmsLobeAzimuthSpread = GetCalibratedParameter(7.5, 7.5, freq); // in degree
        tablenyu->aoaRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAoaRmsLobeElevationSpread =
            GetCalibratedParameter(6.0, 0.0, freq); // in degree
        tablenyu->aoaRmsLobeElevationSpread = "Laplacian";
        tablenyu->los = false; // Flag indicating LOS/NLOS. true implies LOS and false implies NLOS
    }
    else if (m_scenario == "InH" && los)
    {
        tablenyu->muAod = round(GetCalibratedParameter(3, 2, freq)); // in degree
        tablenyu->muAoa = round(GetCalibratedParameter(3, 2, freq)); // in degree
        tablenyu->lambdaC = GetCalibratedParameter(3.6, 0.9, freq);
        tablenyu->betaS = GetCalibratedParameter(0.7, 1.0, freq);
        tablenyu->muS = GetCalibratedParameter(3.7, 1.4, freq);
        tablenyu->muRho = GetCalibratedParameter(3.4, 1.1, freq);
        tablenyu->muTau = GetCalibratedParameter(17.3, 14.6, freq);            // in ns
        tablenyu->minimumVoidInterval = 6;                                     // in ns
        tablenyu->sigmaCluster = GetCalibratedParameter(10, 9, freq);          // in dB
        tablenyu->timeClusterGamma = GetCalibratedParameter(20.7, 18.2, freq); // in ns
        tablenyu->sigmaSubpath = GetCalibratedParameter(5, 5, freq);           // in dB
        tablenyu->subpathGamma = GetCalibratedParameter(2, 2, freq);           // in ns
        tablenyu->meanZod = GetCalibratedParameter(-7.3, -6.8, freq);          // in degree
        tablenyu->sigmaZod = GetCalibratedParameter(3.8, 4.9, freq);           // in degree
        tablenyu->sdOfAodRmsLobeAzimuthSpread = GetCalibratedParameter(20.6, 4.8, freq); // degree
        tablenyu->aodRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAodRmsLobeElevationSpread = GetCalibratedParameter(15.7, 4.3, freq); // degree
        tablenyu->aodRmsLobeElevationSpread = "Gaussian";
        tablenyu->meanZoa = GetCalibratedParameter(7.4, 7.4, freq);  // in degree
        tablenyu->sigmaZoa = GetCalibratedParameter(3.8, 4.5, freq); // in degree
        tablenyu->sdOfAoaRmsLobeAzimuthSpread =
            GetCalibratedParameter(17.7, 4.7, freq); // in degree
        tablenyu->aoaRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAoaRmsLobeElevationSpread =
            GetCalibratedParameter(14.4, 4.4, freq); // in degree
        tablenyu->aoaRmsLobeElevationSpread = "Gaussian";
        tablenyu->los = true; // Flag indicating LOS/NLOS. true implies LOS and false implies NLOS
    }
    else if (m_scenario == "InH" && !los)
    {
        tablenyu->muAod = round(GetCalibratedParameter(3, 3, freq)); // in degree
        tablenyu->muAoa = round(GetCalibratedParameter(3, 2, freq)); // in degree
        tablenyu->lambdaC = GetCalibratedParameter(5.1, 1.8, freq);
        tablenyu->betaS = GetCalibratedParameter(0.7, 1.0, freq);
        tablenyu->muS = GetCalibratedParameter(5.3, 1.2, freq);
        tablenyu->muRho = GetCalibratedParameter(22.7, 2.7, freq);             //
        tablenyu->muTau = GetCalibratedParameter(10.9, 21.0, freq);            // in ns
        tablenyu->minimumVoidInterval = 6;                                     // in ns
        tablenyu->sigmaCluster = GetCalibratedParameter(10, 10, freq);         // in dB
        tablenyu->timeClusterGamma = GetCalibratedParameter(23.6, 16.1, freq); // in ns
        tablenyu->sigmaSubpath = GetCalibratedParameter(6, 6, freq);           // in dB
        tablenyu->subpathGamma = GetCalibratedParameter(9.2, 2.4, freq);       // in ns
        tablenyu->meanZod = GetCalibratedParameter(-5.5, -2.5, freq);          // in degree
        tablenyu->sigmaZod = GetCalibratedParameter(2.9, 2.7, freq);           // in degree
        tablenyu->sdOfAodRmsLobeAzimuthSpread = GetCalibratedParameter(27.1, 4.8, freq); // degree
        tablenyu->aodRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAodRmsLobeElevationSpread = GetCalibratedParameter(16.2, 2.8, freq); // degree
        tablenyu->aodRmsLobeElevationSpread = "Gaussian";
        tablenyu->meanZoa = GetCalibratedParameter(5.5, 4.8, freq);  // in degree
        tablenyu->sigmaZoa = GetCalibratedParameter(2.9, 2.8, freq); // in degree
        tablenyu->sdOfAoaRmsLobeAzimuthSpread =
            GetCalibratedParameter(20.3, 6.6, freq); // in degree
        tablenyu->aoaRmsLobeAzimuthSpread = "Gaussian";
        tablenyu->sdOfAoaRmsLobeElevationSpread =
            GetCalibratedParameter(15.0, 4.5, freq); // in degree
        tablenyu->aoaRmsLobeElevationSpread = "Gaussian";
        tablenyu->los = false; // Flag indicating LOS/NLOS. true implies LOS and false implies NLOS
    }
    else if (m_scenario == "InF" && los)
    {
        tablenyu->muAod = 1.8; // number of AOD spatial Lobes
        tablenyu->muAoa = 1.9; // number of AOA spatial Lobes
        tablenyu->lambdaC = 2.4;
        tablenyu->betaS = 1;
        tablenyu->muS = 2.6;
        tablenyu->alphaTau = 0.7;
        tablenyu->betaTau = 26.9;
        tablenyu->alphaRho = 1.2;
        tablenyu->betaRho = 16.3;
        tablenyu->minimumVoidInterval = 8;           // in ns
        tablenyu->sigmaCluster = 10;                 // in dB
        tablenyu->timeClusterGamma = 16.2;           // in ns
        tablenyu->sigmaSubpath = 13;                 // in dB
        tablenyu->subpathGamma = 4.7;                // in ns
        tablenyu->meanZod = -4;                      // in degree
        tablenyu->sigmaZod = 4.3;                    // in degree
        tablenyu->sdOfAodRmsLobeAzimuthSpread = 6.7; // degree
        tablenyu->aodRmsLobeAzimuthSpread = "Laplacian";
        tablenyu->sdOfAodRmsLobeElevationSpread = 3; // degree
        tablenyu->aodRmsLobeElevationSpread = "Gaussian";
        tablenyu->meanZoa = 4;                        // in degree
        tablenyu->sigmaZoa = 4.3;                     // in degree
        tablenyu->sdOfAoaRmsLobeAzimuthSpread = 11.7; // in degree
        tablenyu->aoaRmsLobeAzimuthSpread = "Laplacian";
        tablenyu->sdOfAoaRmsLobeElevationSpread = 2.3; // in degree
        tablenyu->aoaRmsLobeElevationSpread = "Gaussian";
        tablenyu->los = true; // Flag indicating LOS/NLOS. true implies LOS and false implies NLOS
    }
    else if (m_scenario == "InF" && !los)
    {
        tablenyu->muAod = 1.8; // number of AOD spatial Lobes
        tablenyu->muAoa = 2.5; // number of AOA spatial Lobes
        tablenyu->lambdaC = 2.0;
        tablenyu->betaS = 1;
        tablenyu->muS = 7;
        tablenyu->alphaTau = 0.8;
        tablenyu->betaTau = 13.9;
        tablenyu->alphaRho = 1.6;
        tablenyu->betaRho = 9.0;
        tablenyu->minimumVoidInterval = 8;           // in ns
        tablenyu->sigmaCluster = 6;                  // in dB
        tablenyu->timeClusterGamma = 18.7;           // in ns
        tablenyu->sigmaSubpath = 11;                 // in dB
        tablenyu->subpathGamma = 7.3;                // in ns
        tablenyu->meanZod = -3;                      // in degree
        tablenyu->sigmaZod = 3.5;                    // in degree
        tablenyu->sdOfAodRmsLobeAzimuthSpread = 9.3; // degree
        tablenyu->aodRmsLobeAzimuthSpread = "Laplacian";
        tablenyu->sdOfAodRmsLobeElevationSpread = 4.5; // degree
        tablenyu->aodRmsLobeElevationSpread = "Gaussian";
        tablenyu->meanZoa = 3;                        // in degree
        tablenyu->sigmaZoa = 3.5;                     // in degree
        tablenyu->sdOfAoaRmsLobeAzimuthSpread = 14.1; // in degree
        tablenyu->aoaRmsLobeAzimuthSpread = "Laplacian";
        tablenyu->sdOfAoaRmsLobeElevationSpread = 3.2; // in degree
        tablenyu->aoaRmsLobeElevationSpread = "Gaussian";
        tablenyu->los = true; // Flag indicating LOS/NLOS. true implies LOS and false implies NLOS
    }
    else
    {
        NS_FATAL_ERROR("Unknown channel condition");
    }

    return tablenyu;
}

bool
NYUChannelModel::ChannelParamsNeedsUpdate(Ptr<const NYUChannelParams> channelParams,
                                          Ptr<const ChannelCondition> channelCondition) const
{
    NS_LOG_FUNCTION(this);

    bool update = false;

    // if the channel condition is different the channel has to be updated
    if (!channelCondition->IsEqual(channelParams->m_losCondition, channelParams->m_o2iCondition))
    {
        NS_LOG_DEBUG("Update the channel condition");
        update = true;
    }

    // if the coherence time is over the channel has to be updated
    if (!m_updatePeriod.IsZero() &&
        Simulator::Now() - channelParams->m_generatedTime > m_updatePeriod)
    {
        NS_LOG_DEBUG("Generation time " << channelParams->m_generatedTime.As(Time::NS) << " now "
                                        << Now().As(Time::NS));
        update = true;
    }

    return update;
}

bool
NYUChannelModel::ChannelMatrixNeedsUpdate(Ptr<const NYUChannelParams> channelParams,
                                          Ptr<const ChannelMatrix> channelMatrix)
{
    return channelParams->m_generatedTime > channelMatrix->m_generatedTime;
}

Ptr<const MatrixBasedChannelModel::ChannelMatrix>
NYUChannelModel::GetChannel(Ptr<const MobilityModel> aMob,
                            Ptr<const MobilityModel> bMob,
                            Ptr<const PhasedArrayModel> aAntenna,
                            Ptr<const PhasedArrayModel> bAntenna)
{
    NS_LOG_FUNCTION(this);

    // Compute the channel params key. The key is reciprocal, i.e., key (a, b) = key (b, a)
    uint64_t channelParamsKey =
        GetKey(aMob->GetObject<Node>()->GetId(), bMob->GetObject<Node>()->GetId());
    // Compute the channel matrix key. The key is reciprocal, i.e., key (a, b) = key (b, a)
    uint64_t channelMatrixKey = GetKey(aAntenna->GetId(), bAntenna->GetId());

    // retrieve the channel condition
    Ptr<const ChannelCondition> condition =
        m_channelConditionModel->GetChannelCondition(aMob, bMob);

    // Check if the channel is present in the map and return it, otherwise
    // generate a new channel
    bool updateParams = false;
    bool updateMatrix = false;
    bool notFoundParams = false;
    bool notFoundMatrix = false;
    Ptr<ChannelMatrix> channelMatrix;
    Ptr<NYUChannelParams> channelParams;

    if (m_channelParamsMap.find(channelParamsKey) != m_channelParamsMap.end())
    {
        channelParams = m_channelParamsMap[channelParamsKey];
        // check if it has to be updated
        updateParams = ChannelParamsNeedsUpdate(channelParams, condition);
    }
    else
    {
        NS_LOG_DEBUG("channel params not found");
        notFoundParams = true;
    }

    // get the NYU Channel parameters
    Ptr<const ParamsTable> tablenyu = GetNYUTable(condition);

    if (notFoundParams || updateParams)
    {
        // Step 1: Generate number of time clusters N, spatial AOD Lobes and spatial AOA Lobes, and
        // subpaths in each time cluster Step 2: Generate the intra-cluster subpath delays i.e.
        // Delay of each Subpath within a Time Cluster {rho_mn (ns)} Step 3: Generate the phases
        // (rad) for each Supath in a time cluster Step 4: Generate the cluster excess time delays
        // tau_n (ns) Step 5: Generate temporal cluster powers (mW) Step 6: Generate the cluster
        // subpath powers (mW) step 7: Recover absolute propagation times t_mn (ns) of each subpath
        // component in a time cluster Step 8: Recover AODs and AOAs of the multipath components
        // Step 9: Construct the multipath parameters (AOA,ZOD,AOA,ZOA)
        // Step 10: Adjust the multipath parameters (AOA,ZOD,AOA,ZOA) based on LOS/NLOS and
        // combine the Subpaths which cannot be resolved.
        // Step 11: Generate XPD values for each ray
        channelParams = GenerateChannelParameters(condition, tablenyu, aMob, bMob);
        // store or replace the channel parameters
        m_channelParamsMap[channelParamsKey] = channelParams;
    }

    if (m_channelMatrixMap.find(channelMatrixKey) != m_channelMatrixMap.end())
    {
        // channel matrix present in the map
        NS_LOG_DEBUG("channel matrix present in the map");
        channelMatrix = m_channelMatrixMap[channelMatrixKey];
        updateMatrix = ChannelMatrixNeedsUpdate(channelParams, channelMatrix);
    }
    else
    {
        NS_LOG_DEBUG("channel matrix not found");
        notFoundMatrix = true;
    }

    // If the channel is not present in the map or if it has to be updated
    // generate a new realization
    if (notFoundMatrix || updateMatrix)
    {
        // channel matrix not found or has to be updated, generate a new one
        channelMatrix = GetNewChannel(channelParams, tablenyu, aMob, bMob, aAntenna, bAntenna);
        channelMatrix->m_antennaPair =
            std::make_pair(aAntenna->GetId(),
                           bAntenna->GetId()); // save antenna pair, with the exact order of s and u
                                               // antennas at the moment of the channel generation

        // store or replace the channel matrix in the channel map
        m_channelMatrixMap[channelMatrixKey] = channelMatrix;
    }

    return channelMatrix;
}

Ptr<const MatrixBasedChannelModel::ChannelParams>
NYUChannelModel::GetParams(Ptr<const MobilityModel> aMob, Ptr<const MobilityModel> bMob) const
{
    NS_LOG_FUNCTION(this);

    // Compute the channel key. The key is reciprocal, i.e., key (a, b) = key (b, a)
    uint64_t channelParamsKey =
        GetKey(aMob->GetObject<Node>()->GetId(), bMob->GetObject<Node>()->GetId());

    if (m_channelParamsMap.find(channelParamsKey) != m_channelParamsMap.end())
    {
        return m_channelParamsMap.find(channelParamsKey)->second;
    }
    else
    {
        NS_LOG_WARN("Channel params map not found. Returning a nullptr.");
        return nullptr;
    }
}

// Main code to generate channel parameters
Ptr<NYUChannelModel::NYUChannelParams>
NYUChannelModel::GenerateChannelParameters(const Ptr<const ChannelCondition> channelCondition,
                                           const Ptr<const ParamsTable> tablenyu,
                                           const Ptr<const MobilityModel> aMob,
                                           const Ptr<const MobilityModel> bMob) const
{
    NS_LOG_FUNCTION(this);

    double x = aMob->GetPosition().x - bMob->GetPosition().x;
    double y = aMob->GetPosition().y - bMob->GetPosition().y;
    double distance2D = sqrt(x * x + y * y);

    double pwrthreshold = DynamicRange(distance2D);

    // create a channel matrix instance
    Ptr<NYUChannelParams> channelParams = Create<NYUChannelParams>();
    channelParams->m_generatedTime = Simulator::Now();
    channelParams->m_nodeIds =
        std::make_pair(aMob->GetObject<Node>()->GetId(), bMob->GetObject<Node>()->GetId());

    channelParams->m_losCondition = channelCondition->GetLosCondition();
    channelParams->m_o2iCondition = channelCondition->GetO2iCondition();

    // Step 1: Generate number of time clusters N, spatial AOD Lobes and spatial AOA Lobes, and
    // subpaths in each time cluster
    channelParams->numberOfTimeClusters =
        GetNumberOfTimeClusters(tablenyu->maxNumberOfTimeCluster, tablenyu->lambdaC);
    channelParams->numberOfAodSpatialLobes = GetNumberOfAodSpatialLobes(tablenyu->muAod);
    channelParams->numberOfAoaSpatialLobes = GetNumberOfAoaSpatialLobes(tablenyu->muAoa);
    channelParams->numberOfSubpathInTimeCluster =
        GetNumberOfSubpathsInTimeCluster(channelParams->numberOfTimeClusters,
                                         tablenyu->maxNumberOfSubpaths,
                                         tablenyu->betaS,
                                         tablenyu->muS,
                                         m_frequency);

    // Step 2: Generate the intra-cluster subpath delays i.e. Delay of each Subpath within a Time
    // Cluster {rho_mn (ns)}
    channelParams->subpathDelayInTimeCluster =
        GetIntraClusterDelays(channelParams->numberOfSubpathInTimeCluster,
                              tablenyu->Xmax,
                              tablenyu->muRho,
                              tablenyu->alphaRho,
                              tablenyu->betaRho,
                              m_frequency);

    // Step 3: Generate the phases (rad) for each Supath in a time cluster. 4 phases are generated
    // for each Subpath one for each polarization. Rows represent subpaths and col1,col2,col3,col4
    // represent the polarizations
    channelParams->subpathPhases = GetSubpathPhases(channelParams->numberOfSubpathInTimeCluster);

    // Step 4: Generate the cluster excess time delays tau_n (ns)
    channelParams->delayOfTimeCluster =
        GetClusterExcessTimeDelays(tablenyu->muTau,
                                   channelParams->subpathDelayInTimeCluster,
                                   tablenyu->minimumVoidInterval,
                                   tablenyu->alphaTau,
                                   tablenyu->betaTau);

    // Step 5: Generate temporal cluster powers (mW)
    channelParams->timeClusterPowers = GetClusterPowers(channelParams->delayOfTimeCluster,
                                                        tablenyu->sigmaCluster,
                                                        tablenyu->timeClusterGamma);

    // Step 6: Generate the cluster subpath powers (mW)
    channelParams->subpathPowers = GetSubpathPowers(channelParams->subpathDelayInTimeCluster,
                                                    channelParams->timeClusterPowers,
                                                    tablenyu->sigmaSubpath,
                                                    tablenyu->subpathGamma,
                                                    tablenyu->los);

    // step 7: Recover absolute propagation times t_mn (ns) of each subpath component in a time
    // cluster
    channelParams->absoluteSubpathDelayinTimeCluster =
        GetAbsolutePropagationTimes(distance2D,
                                    channelParams->delayOfTimeCluster,
                                    channelParams->subpathDelayInTimeCluster);

    // Step 8: Recover AODs and AOAs of the multipath components
    channelParams->subpathAodZod =
        GetSubpathMappingAndAngles(channelParams->numberOfAodSpatialLobes,
                                   channelParams->numberOfSubpathInTimeCluster,
                                   tablenyu->meanZod,
                                   tablenyu->sigmaZod,
                                   tablenyu->sdOfAodRmsLobeAzimuthSpread,
                                   tablenyu->sdOfAodRmsLobeElevationSpread,
                                   tablenyu->aodRmsLobeAzimuthSpread,
                                   tablenyu->aodRmsLobeElevationSpread);

    channelParams->subpathAoaZoa =
        GetSubpathMappingAndAngles(channelParams->numberOfAoaSpatialLobes,
                                   channelParams->numberOfSubpathInTimeCluster,
                                   tablenyu->meanZoa,
                                   tablenyu->sigmaZoa,
                                   tablenyu->sdOfAoaRmsLobeAzimuthSpread,
                                   tablenyu->sdOfAoaRmsLobeElevationSpread,
                                   tablenyu->aoaRmsLobeAzimuthSpread,
                                   tablenyu->aoaRmsLobeElevationSpread);

    // Step 9: Construct the multipath parameters (AOA,ZOD,AOA,ZOA)
    channelParams->powerSpectrumOld =
        GetPowerSpectrum(channelParams->numberOfSubpathInTimeCluster,
                         channelParams->absoluteSubpathDelayinTimeCluster,
                         channelParams->subpathPowers,
                         channelParams->subpathPhases,
                         channelParams->subpathAodZod,
                         channelParams->subpathAoaZoa);

    // Step 10: Adjust the multipath parameters (AOA,ZOD,AOA,ZOA) based on LOS/NLOS and
    // combine the Subpaths which cannot be resolved.
    channelParams->powerSpectrum = GetBWAdjustedtedPowerSpectrum(channelParams->powerSpectrumOld,
                                                                 m_rfBandwidth,
                                                                 tablenyu->los);

    // All subpaths whose power is above threshold is considered. The threshold is defined as Max
    // power of the subpath - 30 dB.
    channelParams->powerSpectrum = GetValidSubapths(channelParams->powerSpectrum, pwrthreshold);

    // Step 11: Generate XPD values for each ray in powerSpectrum
    channelParams->xpd =
        GetXpdPerSubpath(channelParams->powerSpectrum.size(), tablenyu->xpdMean, tablenyu->xpdSd);

    // The AOD,ZOD,AOA,ZOA generated by NYU channel model is in degrees and the coordinate system
    // used is phi w.r.t to y axis and theta w.r.t xy plane. This is different when compared to the
    // GCS where phi is w.r.t to x axis and theta is w.r.t z axis. This API converts NYU coordinate
    // system to GCS and also saves the angles in radians. So AOD,ZOD,AOA,ZOA are saved in radians.
    // m_angle is inherited from matrix-based-channel-model and is used in CalcBeamformingGain()
    channelParams->m_angle =
        NYUCoordinateSystemToGlobalCoordinateSystem(channelParams->powerSpectrum);

    channelParams->rayAodRadian.resize(channelParams->powerSpectrum.size());
    channelParams->rayZodRadian.resize(channelParams->powerSpectrum.size());
    channelParams->rayAoaRadian.resize(channelParams->powerSpectrum.size());
    channelParams->rayZoaRadian.resize(channelParams->powerSpectrum.size());

    // debug m_angle array and store the AOD,ZOD,AOA,ZOA in the rayAod,rayZod,rayAoa and rayZoa
    // vectors
    for (int i = 0; i < (int)channelParams->m_angle.size(); i++)
    {
        for (int j = 0; j < (int)channelParams->m_angle[i].size(); j++)
        {
            if (i == 0)
            {
                NS_LOG_DEBUG("m_angle sp id:" << j << " aoa:" << channelParams->m_angle[i][j]);
                channelParams->rayAoaRadian[j] = channelParams->m_angle[i][j];
            }
            else if (i == 1)
            {
                NS_LOG_DEBUG("m_angle sp id:" << j << " zoa:" << channelParams->m_angle[i][j]);
                channelParams->rayZoaRadian[j] = channelParams->m_angle[i][j];
            }
            else if (i == 2)
            {
                NS_LOG_DEBUG("m_angle sp id:" << j << " aod:" << channelParams->m_angle[i][j]);
                channelParams->rayAodRadian[j] = channelParams->m_angle[i][j];
            }
            else if (i == 3)
            {
                NS_LOG_DEBUG("m_angle sp id:" << j << " zod:" << channelParams->m_angle[i][j]);
                channelParams->rayZodRadian[j] = channelParams->m_angle[i][j];
            }
        }
    }

    // Save the delay of SP in m_delay. This is used later in CalcBeamformingGain() api present in
    // nyu-spectrum-propagation-loss-model.cc
    for (int i = 0; i < (int)channelParams->powerSpectrum.size(); i++)
    {
        channelParams->m_delay.push_back(channelParams->powerSpectrum[i][0]);
    }

    // debug ray delay stored in mdelay - same as powerSpectrum[i][0]
    for (int i = 0; i < (int)channelParams->m_delay.size(); i++)
    {
        NS_LOG_DEBUG(" Subpath id:" << i << " delay:" << channelParams->m_delay[i]);
    }

    // Stores the total number of subpaths after BW adjustment and excluding weak subpaths
    channelParams->totalSubpaths = channelParams->powerSpectrum.size();

    NS_LOG_DEBUG("Total Number of SP is:" << channelParams->totalSubpaths);

    return channelParams;
}

Ptr<MatrixBasedChannelModel::ChannelMatrix>
NYUChannelModel::GetNewChannel(Ptr<const NYUChannelParams> channelParams,
                               Ptr<const ParamsTable> tablenyu,
                               const Ptr<const MobilityModel> sMob,
                               const Ptr<const MobilityModel> uMob,
                               Ptr<const PhasedArrayModel> sAntenna,
                               Ptr<const PhasedArrayModel> uAntenna) const
{
    NS_LOG_FUNCTION(this);

    NS_ASSERT_MSG(m_frequency > 0.0, "Set the operating frequency first!");
    NS_ASSERT_MSG(m_rfBandwidth > 0.0, "Set the operating RF Bandwidth first!");

    Ptr<ChannelMatrix> channelMatrix = Create<ChannelMatrix>();
    channelMatrix->m_generatedTime = Simulator::Now();

    // save in which order is generated this matrix
    channelMatrix->m_nodeIds =
        std::make_pair(sMob->GetObject<Node>()->GetId(), uMob->GetObject<Node>()->GetId());
    // check if channelParams structure is generated in direction s-to-u or u-to-s
    bool isSameDirection = (channelParams->m_nodeIds == channelMatrix->m_nodeIds);

    MatrixBasedChannelModel::DoubleVector rayAodRadian;
    MatrixBasedChannelModel::DoubleVector rayZodRadian;
    MatrixBasedChannelModel::DoubleVector rayAoaRadian;
    MatrixBasedChannelModel::DoubleVector rayZoaRadian;

    // if channel params is generated in the same direction in which we
    // generate the channel matrix, angles and zenit od departure and arrival are ok,
    // just set them to corresponding variable that will be used for the generation
    // of channel matrix, otherwise we need to flip angles and zenits of departure and arrival
    if (isSameDirection)
    {
        rayAodRadian = channelParams->rayAodRadian;
        rayZodRadian = channelParams->rayZodRadian;
        rayAoaRadian = channelParams->rayAoaRadian;
        rayZoaRadian = channelParams->rayZoaRadian;
    }
    else
    {
        rayAodRadian = channelParams->rayAoaRadian;
        rayAoaRadian = channelParams->rayAodRadian;
        rayZodRadian = channelParams->rayZoaRadian;
        rayZoaRadian = channelParams->rayZodRadian;
    }

    // Step 11: Generate channel coefficients for each ray n and each receiver
    //  and transmitter element pair u,s.

    uint64_t uSize = uAntenna->GetNumElems();
    uint64_t sSize = sAntenna->GetNumElems();

    Complex3DVector hUsn(uSize,
                         sSize,
                         channelParams->totalSubpaths); // channel coffecient hUsn[u][s][n];

    // Geometrical direction used for LOS ray
    Angles sAngle(uMob->GetPosition(), sMob->GetPosition());
    Angles uAngle(sMob->GetPosition(), uMob->GetPosition());

    // The following for loops computes the channel coefficients
    for (uint64_t uIndex = 0; uIndex < uSize; uIndex++)
    {
        Vector uLoc = uAntenna->GetElementLocation(uIndex);
        for (uint64_t sIndex = 0; sIndex < sSize; sIndex++)
        {
            Vector sLoc = sAntenna->GetElementLocation(sIndex);
            for (uint8_t nIndex = 0; nIndex < channelParams->totalSubpaths; nIndex++)
            {
                std::complex<double> rays(0, 0);
                // if LOS then ray 1 is AOD and AOA , ZOD and ZOA are aligned
                if (tablenyu->los && nIndex == 0)
                {
                    double rxPhaseDiff =
                        2 * M_PI *
                        (sin(uAngle.GetInclination()) * cos(uAngle.GetAzimuth()) * uLoc.x +
                         sin(uAngle.GetInclination()) * sin(uAngle.GetAzimuth()) * uLoc.y +
                         cos(uAngle.GetInclination()) * uLoc.z);
                    double txPhaseDiff =
                        2 * M_PI *
                        (sin(sAngle.GetInclination()) * cos(sAngle.GetAzimuth()) * sLoc.x +
                         sin(sAngle.GetInclination()) * sin(sAngle.GetAzimuth()) * sLoc.y +
                         cos(sAngle.GetInclination()) * sLoc.z);
                    double rxFieldPatternPhi;
                    double rxFieldPatternTheta;
                    double txFieldPatternPhi;
                    double txFieldPatternTheta;
                    std::tie(rxFieldPatternPhi, rxFieldPatternTheta) =
                        uAntenna->GetElementFieldPattern(
                            Angles(uAngle.GetAzimuth(), uAngle.GetInclination()));
                    std::tie(txFieldPatternPhi, txFieldPatternTheta) =
                        sAntenna->GetElementFieldPattern(
                            Angles(sAngle.GetAzimuth(), sAngle.GetInclination()));
                    rays = (std::complex<double>(cos(channelParams->subpathPhases[nIndex][0]),
                                                 sin(channelParams->subpathPhases[nIndex][0])) *
                                rxFieldPatternTheta * txFieldPatternTheta +
                            std::complex<double>(cos(channelParams->subpathPhases[nIndex][1]),
                                                 sin(channelParams->subpathPhases[nIndex][1])) *
                                std::sqrt(1 / GetDbToPow(channelParams->xpd[nIndex][1])) *
                                rxFieldPatternTheta * txFieldPatternPhi +
                            std::complex<double>(cos(channelParams->subpathPhases[nIndex][2]),
                                                 sin(channelParams->subpathPhases[nIndex][2])) *
                                std::sqrt(1 / GetDbToPow(channelParams->xpd[nIndex][2])) *
                                rxFieldPatternPhi * txFieldPatternTheta +
                            std::complex<double>(cos(channelParams->subpathPhases[nIndex][3]),
                                                 sin(channelParams->subpathPhases[nIndex][3])) *
                                std::sqrt(1 / GetDbToPow(channelParams->xpd[nIndex][0])) *
                                rxFieldPatternPhi * txFieldPatternPhi) *
                           std::complex<double>(cos(rxPhaseDiff), sin(rxPhaseDiff)) *
                           std::complex<double>(cos(txPhaseDiff), sin(txPhaseDiff));
                    rays *= sqrt(channelParams->powerSpectrum[nIndex][1]);
                    hUsn(uIndex, sIndex, nIndex) = rays;
                }
                else
                {
                    double rxPhaseDiff =
                        2 * M_PI *
                        (sin(rayZoaRadian[nIndex]) * cos(rayAoaRadian[nIndex]) * uLoc.x +
                         sin(rayZoaRadian[nIndex]) * sin(rayAoaRadian[nIndex]) * uLoc.y +
                         cos(rayZoaRadian[nIndex]) * uLoc.z);

                    double txPhaseDiff =
                        2 * M_PI *
                        (sin(rayZodRadian[nIndex]) * cos(rayAodRadian[nIndex]) * sLoc.x +
                         sin(rayZodRadian[nIndex]) * sin(rayAodRadian[nIndex]) * sLoc.y +
                         cos(rayZodRadian[nIndex]) * sLoc.z);
                    double rxFieldPatternPhi;
                    double rxFieldPatternTheta;
                    double txFieldPatternPhi;
                    double txFieldPatternTheta;
                    std::tie(rxFieldPatternPhi, rxFieldPatternTheta) =
                        uAntenna->GetElementFieldPattern(
                            Angles(rayAoaRadian[nIndex], rayZoaRadian[nIndex]));
                    std::tie(txFieldPatternPhi, txFieldPatternTheta) =
                        sAntenna->GetElementFieldPattern(
                            Angles(rayAodRadian[nIndex], rayZodRadian[nIndex]));
                    rays = (std::complex<double>(cos(channelParams->subpathPhases[nIndex][0]),
                                                 sin(channelParams->subpathPhases[nIndex][0])) *
                                rxFieldPatternTheta * txFieldPatternTheta +
                            std::complex<double>(cos(channelParams->subpathPhases[nIndex][1]),
                                                 sin(channelParams->subpathPhases[nIndex][1])) *
                                std::sqrt(1 / GetDbToPow(channelParams->xpd[nIndex][1])) *
                                rxFieldPatternTheta * txFieldPatternPhi +
                            std::complex<double>(cos(channelParams->subpathPhases[nIndex][2]),
                                                 sin(channelParams->subpathPhases[nIndex][2])) *
                                std::sqrt(1 / GetDbToPow(channelParams->xpd[nIndex][2])) *
                                rxFieldPatternPhi * txFieldPatternTheta +
                            std::complex<double>(cos(channelParams->subpathPhases[nIndex][3]),
                                                 sin(channelParams->subpathPhases[nIndex][3])) *
                                std::sqrt(1 / GetDbToPow(channelParams->xpd[nIndex][0])) *
                                rxFieldPatternPhi * txFieldPatternPhi) *
                           std::complex<double>(cos(rxPhaseDiff), sin(rxPhaseDiff)) *
                           std::complex<double>(cos(txPhaseDiff), sin(txPhaseDiff));
                    rays *= sqrt(channelParams->powerSpectrum[nIndex][1]);
                    hUsn(uIndex, sIndex, nIndex) = rays;
                }
            }
        }
    }

    NS_LOG_DEBUG("Husn (sAntenna, uAntenna):" << sAntenna->GetId() << ", " << uAntenna->GetId());

    NS_LOG_DEBUG("Husn (sAntenna, uAntenna):" << sAntenna->GetId() << ", " << uAntenna->GetId());

    for (size_t cIndex = 0; cIndex < hUsn.GetNumPages(); cIndex++)
    {
        for (size_t rowIdx = 0; rowIdx < hUsn.GetNumRows(); rowIdx++)
        {
            for (size_t colIdx = 0; colIdx < hUsn.GetNumCols(); colIdx++)
            {
                NS_LOG_DEBUG(" " << hUsn(rowIdx, colIdx, cIndex) << ",");
            }
        }
    }

    NS_LOG_INFO("size of coefficient matrix (rows, columns, clusters) = ("
                << hUsn.GetNumRows() << ", " << hUsn.GetNumCols() << ", " << hUsn.GetNumPages()
                << ")");

    channelMatrix->m_channel = hUsn;

    return channelMatrix;
}

int64_t
NYUChannelModel::AssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    m_normalRv->SetStream(stream);
    m_uniformRv->SetStream(stream + 1);
    m_expRv->SetStream(stream + 2);
    return 3;
}

int
NYUChannelModel::GetPoissionDist(double lambda) const
{
    NS_LOG_FUNCTION(this << lambda);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::poisson_distribution<int> distribution(lambda);
    double value = distribution(gen);
    NS_LOG_DEBUG(" Value in Pois Dist is:" << value);
    return value;
}

int
NYUChannelModel::GetDiscreteUniformDist(const double min, const double max) const
{
    NS_LOG_FUNCTION(this << min << max);
    int value = m_uniformRv->GetInteger(min, max);
    NS_LOG_DEBUG(" Value in Uniform Dist is:" << (double)value << ",min is:" << min
                                              << ", max is:" << max);
    return value;
}

double
NYUChannelModel::GetUniformDist(const double min, const double max) const
{
    NS_LOG_FUNCTION(this << min << max);
    double value = m_uniformRv->GetValue(min, max);
    NS_LOG_DEBUG(" Value in Uniform Dist is:" << (double)value << ",min is:" << min
                                              << ", max is:" << max);
    return value;
}

double
NYUChannelModel::GetExponentialDist(double lambda) const
{
    NS_LOG_FUNCTION(this << lambda);
    m_expRv->SetAttribute("Mean", DoubleValue(lambda));
    double value = m_expRv->GetValue();
    NS_LOG_DEBUG("Value in Exp Dist is:" << value);
    return value;
}

double
NYUChannelModel::GetGammaDist(double alpha, double beta) const
{
    double value = 0;
    NS_LOG_FUNCTION(this << alpha << beta);
    m_gammaRv->SetAttribute("Alpha", DoubleValue(alpha));
    m_gammaRv->SetAttribute("Beta", DoubleValue(beta));
    value = m_gammaRv->GetValue();
    NS_LOG_DEBUG("Value in Gamma Dist is:" << value);
    return value;
}

int
NYUChannelModel::GetBinomialDist(double trials, double success) const
{
    NS_LOG_FUNCTION(this << trials << success);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::binomial_distribution<int> distribution(trials, success);
    double value = distribution(gen);
    NS_LOG_DEBUG(" Value in Binomial Dist is:" << value);
    return value;
}

double
NYUChannelModel::GetMinimumValue(double val1, double val2) const
{
    NS_LOG_FUNCTION(this << val1 << val2);
    double output = 0;
    if (val1 > val2)
    {
        output = val2;
    }
    else
    {
        output = val1;
    }
    NS_LOG_DEBUG("Min Value is:" << output << std::endl);
    return output;
}

double
NYUChannelModel::GetMaximumValue(double val1, double val2) const
{
    NS_LOG_FUNCTION(this << val1 << val2);
    double output = 0;
    if (val1 > val2)
    {
        output = val1;
    }
    else
    {
        output = val2;
    }
    NS_LOG_DEBUG("Max Value is:" << output << std::endl);
    return output;
}

int
NYUChannelModel::GetSignum(double value) const
{
    NS_LOG_FUNCTION(this << value);
    int output = 0;
    if (value > 0)
    {
        output = 1;
    }
    else if (value < 0)
    {
        output = -1;
    }
    else
    {
        output = 0;
    }
    NS_LOG_DEBUG("Signum Function output value is:" << output << std::endl);
    return output;
}

double
NYUChannelModel::GetDbToPow(double pwr_dB) const
{
    double pwr_lin = 0;
    pwr_lin = std::pow(10, (pwr_dB * 0.10));
    return pwr_lin;
}

// Dynamic range of NYU Channel Sounder
double
NYUChannelModel::DynamicRange(double distance2D) const
{
    // distance is in meters and dynamicRange is in dB
    double dynamicRange = 0;
    if (distance2D <= 500)
    {
        dynamicRange = 190;
    }
    else
    {
        dynamicRange = 220;
    }
    return dynamicRange;
}

int
NYUChannelModel::GetNumberOfTimeClusters(double maxNumberOfTimeCluster, double lambdaC) const
{
    NS_LOG_FUNCTION(this << maxNumberOfTimeCluster << lambdaC);
    int numberOfTimeCluster = 0;
    if (m_scenario == "InH" || m_scenario == "InF")
    {
        // InH and InF
        numberOfTimeCluster = GetPoissionDist(lambdaC) + 1;
    }
    else
    {
        // UMi-StreetCanyon,UMa and RMa scenario
        numberOfTimeCluster = GetDiscreteUniformDist(1, maxNumberOfTimeCluster);
    }
    NS_LOG_DEBUG(" Scenario:" << m_scenario
                              << " number of Time Cluster is:" << numberOfTimeCluster);
    return numberOfTimeCluster;
}

int
NYUChannelModel::GetNumberOfAoaSpatialLobes(double muAoa) const
{
    NS_LOG_FUNCTION(this << muAoa);

    int numAOALobes = 0;
    if (m_scenario == "InH")
    {
        numAOALobes = GetDiscreteUniformDist(1, muAoa);
    }
    else if (m_scenario == "InF")
    {
        numAOALobes = GetPoissionDist(muAoa) + 1;
    }
    else if (m_scenario == "RMa")
    {
        numAOALobes = 1;
    }
    else
    {
        /* UMi-StreetCanyon or UMa */
        int aoa_instance = 0;
        aoa_instance = GetPoissionDist(muAoa);
        numAOALobes = GetMaximumValue(1, GetMinimumValue(5, aoa_instance));
    }
    NS_LOG_DEBUG(" Scenario:" << m_scenario << " number of AOA Spatial Lobes is:" << numAOALobes);
    return numAOALobes;
}

int
NYUChannelModel::GetNumberOfAodSpatialLobes(double muAod) const
{
    NS_LOG_FUNCTION(this << muAod);

    int numAODLobes = 0;
    if (m_scenario == "InH")
    {
        numAODLobes = GetDiscreteUniformDist(1, muAod);
    }
    else if (m_scenario == "InF")
    {
        numAODLobes = GetPoissionDist(muAod) + 1;
    }
    else if (m_scenario == "RMa")
    {
        numAODLobes = 1;
    }
    else
    {
        /* UMi-StreetCanyon or UMa */
        int aod_instance = 0;
        aod_instance = GetPoissionDist(muAod);
        numAODLobes = GetMaximumValue(1, GetMinimumValue(5, aod_instance));
    }
    NS_LOG_DEBUG(" Scenario:" << m_scenario << " number of AOD Spatial Lobes is:" << numAODLobes);
    return numAODLobes;
}

MatrixBasedChannelModel::DoubleVector
NYUChannelModel::GetNumberOfSubpathsInTimeCluster(int numberOfTimeClusters,
                                                  double maxNumberOfSubpaths,
                                                  double betaS,
                                                  double muS,
                                                  double frequency) const
{
    NS_LOG_FUNCTION(this << numberOfTimeClusters << maxNumberOfSubpaths << betaS << muS
                         << frequency);

    int i;
    int k;
    MatrixBasedChannelModel::DoubleVector subpathPerTimeCluster;
    double freqGHz = frequency / 1e9;

    if (m_scenario == "InH" || m_scenario == "InF")
    {
        for (k = 0; k < numberOfTimeClusters; k++)
        {
            if (numberOfTimeClusters == 1)
            {
                i = 1;
            }
            else
            {
                i = GetBinomialDist(1, betaS);
            }
            if (i == 1)
            {
                subpathPerTimeCluster.push_back(round(GetExponentialDist(muS)) + 1);
            }
            else
            {
                subpathPerTimeCluster.push_back(1);
            }
        }
        while (numberOfTimeClusters == 1 && subpathPerTimeCluster.size() == 1)
        {
            subpathPerTimeCluster.push_back(round(GetExponentialDist(muS)) + 1);
        }
    }
    else
    {
        // UMi-StreetCanyon,UMa and RMa
        if (freqGHz < 100 || m_scenario == "RMa")
        {
            for (i = 0; i < numberOfTimeClusters; i++)
            {
                subpathPerTimeCluster.push_back(GetDiscreteUniformDist(1, maxNumberOfSubpaths));
            }
        }
        else
        {
            for (i = 0; i < numberOfTimeClusters; i++)
            {
                subpathPerTimeCluster.push_back(round(GetExponentialDist(muS)) + 1);
            }
        }
    }
    for (long unsigned int i = 0; i < subpathPerTimeCluster.size(); i++)
    {
        NS_LOG_DEBUG("Time Cluster:" << i << " Number of Subpaths:" << subpathPerTimeCluster[i]
                                     << std::endl);
    }
    return subpathPerTimeCluster;
}

MatrixBasedChannelModel::Double2DVector
NYUChannelModel::GetIntraClusterDelays(
    MatrixBasedChannelModel::DoubleVector numberOfSubpathInTimeCluster,
    double Xmax,
    double muRho,
    double alphaRho,
    double betaRho,
    double frequency) const
{
    NS_LOG_FUNCTION(this << Xmax << muRho << m_rfBandwidth << alphaRho << betaRho << frequency);

    int i = 0;
    int j = 0;
    int numSP = 0;
    double tmp = 0;
    double min_delay = 0;
    double x = 0;
    double freqGHz = frequency / 1e9;

    MatrixBasedChannelModel::DoubleVector arrayTemp;
    MatrixBasedChannelModel::Double2DVector SPdelaysinTC;

    for (i = 0; i < (int)numberOfSubpathInTimeCluster.size(); i++)
    {
        // SP in each time cluster
        numSP = numberOfSubpathInTimeCluster[i];
        // generating delay in ns for each SP in a TC.
        for (j = 0; j < numSP; j++)
        {
            if (m_scenario == "InH")
            {
                arrayTemp.push_back(GetExponentialDist(muRho));
            }
            else if (m_scenario == "InF")
            {
                arrayTemp.push_back(GetGammaDist(alphaRho, betaRho));
            }
            else
            {
                if (freqGHz < 100)
                {
                    tmp = (1 / (m_rfBandwidth / 2)) * 1e9 * (j + 1);
                    arrayTemp.push_back(tmp);
                }
                else
                {
                    arrayTemp.push_back(GetExponentialDist(muRho));
                }
            }
        }
        // finding min_delay
        min_delay = *min_element(arrayTemp.begin(), arrayTemp.end());

        for (j = 0; j < (int)arrayTemp.size(); j++)
        {
            arrayTemp[j] = arrayTemp[j] - min_delay;
        }

        // sorting the delay generated
        sort(arrayTemp.begin(), arrayTemp.end());

        if ((!(m_scenario == "InH" || m_scenario == "InF")) && freqGHz < 100)
        {
            // For UMa, UMi-StreetCanyon, RMa only multiply the sorted array
            x = Xmax * GetUniformDist(0, 1);
            for (j = 0; j < (int)arrayTemp.size(); j++)
            {
                arrayTemp[j] = pow(arrayTemp[j], (1 + x));
            }
        }
        SPdelaysinTC.push_back(arrayTemp);
        // clear vector arrayTemp for each TC
        arrayTemp.clear();
    }

    // Displaying the subpath delay generated for each time cluster for debugging
    for (i = 0; i < (int)SPdelaysinTC.size(); i++)
    {
        for (j = 0; j < (int)SPdelaysinTC[i].size(); j++)
        {
            NS_LOG_DEBUG("Time Cluster: " << i << " Subpath:" << j
                                          << " Delay:" << SPdelaysinTC[i][j] << std::endl);
        }
    }
    return SPdelaysinTC;
}

MatrixBasedChannelModel::Double2DVector
NYUChannelModel::GetSubpathPhases(
    MatrixBasedChannelModel::DoubleVector numberOfSubpathInTimeCluster) const
{
    NS_LOG_FUNCTION(this);
    int i;
    int j;
    int k;
    int numSP = 0;
    double SubpathPhases = 0;

    MatrixBasedChannelModel::DoubleVector polarizationPhases;
    MatrixBasedChannelModel::Double2DVector SubpathPhases_db;

    // Number of TC is the size of the vector numberOfSubpathInTimeCluster
    for (i = 0; i < (int)numberOfSubpathInTimeCluster.size(); i++)
    {
        // SP in each time cluster
        numSP = numberOfSubpathInTimeCluster[i];
        NS_LOG_DEBUG("TC" << i << "numSP:" << numSP);

        for (j = 0; j < numSP; j++)
        {
            NS_LOG_DEBUG("TC" << i << "SP:" << j);
            // generate four phases for each ray. One phase for each polarization
            // phases: {theta_theta (V-V), theta_phi (V-H), phi_theta (H-V), phi-phi (H-H)}
            for (k = 0; k < 4; k++)
            {
                SubpathPhases = GetUniformDist(-1 * M_PI, M_PI);
                polarizationPhases.push_back(SubpathPhases);
            }
            SubpathPhases_db.push_back(polarizationPhases);
            polarizationPhases.clear();
        }
    }
    return SubpathPhases_db;
}

MatrixBasedChannelModel::DoubleVector
NYUChannelModel::GetClusterExcessTimeDelays(
    double muTau,
    MatrixBasedChannelModel::Double2DVector subpathDelayInTimeCluster,
    double minimumVoidInterval,
    double alphaTau,
    double betaTau) const
{
    NS_LOG_FUNCTION(this << muTau << minimumVoidInterval << alphaTau << betaTau);

    int i;
    int numberOfSubpathInTimeCluster;
    int numTC = 0;
    double min_delay = 0;
    double clusterVoidInterval = 0;
    double LastSPTC = 0;
    double delay = 0;

    MatrixBasedChannelModel::DoubleVector tau_n_prime;
    MatrixBasedChannelModel::DoubleVector tau_n;

    tau_n.resize(1);
    numTC = subpathDelayInTimeCluster.size();
    clusterVoidInterval = minimumVoidInterval;

    if (m_scenario == "InF")
    {
        for (i = 0; i < numTC; i++)
        {
            tau_n_prime.push_back(GetGammaDist(alphaTau, betaTau));
        }
    }
    else
    {
        // UMi-StreetCanyon, UMa, RMa and InH. For each TC generate a delay based on an exponential
        // distribution
        for (i = 0; i < numTC; i++)
        {
            tau_n_prime.push_back(GetExponentialDist(muTau));
        }
    }

    min_delay = *min_element(tau_n_prime.begin(), tau_n_prime.end());
    for (i = 0; i < (int)tau_n_prime.size(); i++)
    {
        tau_n_prime[i] = tau_n_prime[i] - min_delay;
    }
    sort(tau_n_prime.begin(), tau_n_prime.end());
    // Fetch the delay of the last SP of TC1.
    numberOfSubpathInTimeCluster = subpathDelayInTimeCluster[0].size();
    LastSPTC = subpathDelayInTimeCluster[0][numberOfSubpathInTimeCluster - 1];
    // First TC delay is 0 ns. For the other TC need to compute the excess delay
    for (i = 1; i < numTC; i++)
    {
        delay = tau_n_prime[i] + LastSPTC + clusterVoidInterval;
        tau_n.push_back(delay);
        numberOfSubpathInTimeCluster = subpathDelayInTimeCluster[i].size();
        LastSPTC = tau_n[i] + subpathDelayInTimeCluster[i][numberOfSubpathInTimeCluster - 1];
    }
    // display the computed excess delay values for each time cluster
    for (i = 0; i < (int)tau_n.size(); i++)
    {
        NS_LOG_DEBUG("Mean Excess Delay of TC " << i << " is:" << tau_n[i] << std::endl);
    }
    return tau_n;
}

MatrixBasedChannelModel::DoubleVector
NYUChannelModel::GetClusterPowers(MatrixBasedChannelModel::DoubleVector getClusterExcessTimeDelays,
                                  double sigmaCluster,
                                  double timeclusterGamma) const
{
    NS_LOG_FUNCTION(this << sigmaCluster << timeclusterGamma);

    int i;
    int numTC = 0;                 // num of time clusters
    double shadowing = 0;          // shadowing in each time cluster
    double Pwr = 0;                // power in time cluster
    double sum_of_cluster_pwr = 0; // sum of powers in all time clusters
    double NormalizedPwr = 0;      // each cluster power divided by sum of all cluster power

    MatrixBasedChannelModel::DoubleVector z; // Vector storing shadowing in each Time Cluster
    MatrixBasedChannelModel::DoubleVector ClusterPwr; // Vector storing power of each Time Cluster
    MatrixBasedChannelModel::DoubleVector
        NormalizedClusterPwr; // Vector storing normalized power of each Time Cluster

    numTC = getClusterExcessTimeDelays.size();

    for (i = 0; i < numTC; i++)
    {
        shadowing = sigmaCluster * m_normalRv->GetValue();
        z.push_back(shadowing);
    }

    // debugging: to check shadowing power in each time cluster
    for (i = 0; i < (int)z.size(); i++)
    {
        NS_LOG_DEBUG("Shadowing power in TC: " << i << " is:" << z[i]);
    }

    for (i = 0; i < numTC; i++)
    {
        Pwr = exp(-getClusterExcessTimeDelays[i] / timeclusterGamma) * (pow(10, (z[i] / 10)));
        ClusterPwr.push_back(Pwr);
    }

    // debugging: to check power distribution as per exponential distribution in each time cluster
    for (i = 0; i < (int)ClusterPwr.size(); i++)
    {
        NS_LOG_DEBUG("Exponential Power distribution in TC: " << i << " is:" << ClusterPwr[i]);
    }

    // sum cluster power
    sum_of_cluster_pwr = std::accumulate(ClusterPwr.begin(), ClusterPwr.end(), 0.0);

    // debugging: Sum of the total power of all Time Cluster
    NS_LOG_DEBUG("Sum of Powers in all TC is:" << sum_of_cluster_pwr);

    // normalize cluster ratios
    for (i = 0; i < (int)ClusterPwr.size(); i++)
    {
        NormalizedPwr = ClusterPwr[i] / sum_of_cluster_pwr;
        NormalizedClusterPwr.push_back(NormalizedPwr);
    }

    // debugging: check the normalized cluster power
    for (i = 0; i < (int)ClusterPwr.size(); i++)
    {
        NS_LOG_DEBUG("Normalized Cluster Power for TC " << i << " is:" << NormalizedClusterPwr[i]);
    }
    return NormalizedClusterPwr;
}

MatrixBasedChannelModel::Double2DVector
NYUChannelModel::GetSubpathPowers(MatrixBasedChannelModel::Double2DVector subpathDelayInTimeCluster,
                                  MatrixBasedChannelModel::DoubleVector timeClusterPowers,
                                  double sigmaSubpath,
                                  double subpathGamma,
                                  bool los) const
{
    NS_LOG_FUNCTION(this << sigmaSubpath << subpathGamma << los);

    int i;                                // loop iteration
    int j;                                // loop iteration
    int numTC = 0;                        // Number of Time Clusters
    int numberOfSubpathInTimeCluster = 0; // Number of SP in each Time Cluster
    int maxElementIndex =
        0; // Find the index of the strongest subpath power in time cluster one for LOS
    double shadowing = 0;         // shadowing power for each subpath
    double subPathRatios_tmp = 0; // subpath power w.r.t distributions
    double maxElement = 0;        // maximum value of subpath power for TC1 in LOS
    double tmp;                   // used for swapping powers for TC1 in LOS condition
    double sum_of_sp_pwr = 0;     // sum of subpath powers in a particular time cluster
    double subPathRatios = 0;     // final power of each subpath in a time cluster

    numTC = timeClusterPowers.size();        // Number of Time Clusters
    MatrixBasedChannelModel::DoubleVector u; // shadowing values for all SP in a one TC
    MatrixBasedChannelModel::DoubleVector
        SubPathRatios_tmp_vector; // Subpath power from distribution
    MatrixBasedChannelModel::DoubleVector
        subPathRatios_vect; // normalized subpath for each time cluster
    MatrixBasedChannelModel::Double2DVector
        subPathPowers; // Vector storing the final computed power of all subpaths in a time cluster

    // Each time cluster
    for (i = 0; i < numTC; i++)
    {
        numberOfSubpathInTimeCluster = subpathDelayInTimeCluster[i].size();

        // Shadowing values for all SP in a TC
        for (j = 0; j < numberOfSubpathInTimeCluster; j++)
        {
            shadowing = sigmaSubpath * m_normalRv->GetValue();
            u.push_back(shadowing);
        }

        // debugging: to shadowing power for all SP in a time cluster
        for (j = 0; j < (int)u.size(); j++)
        {
            NS_LOG_DEBUG("TC:" << i << " Shadowing Power for SP:" << j << " is:" << u[j]);
        }

        // Each SP of a time cluster store the power
        for (j = 0; j < numberOfSubpathInTimeCluster; j++)
        {
            subPathRatios_tmp =
                exp(-subpathDelayInTimeCluster[i][j] / subpathGamma) * (pow(10, u[j] / 10));
            SubPathRatios_tmp_vector.push_back(subPathRatios_tmp);
        }

        // debugging: exponential power distribution for SPs in a TC
        for (j = 0; j < (int)SubPathRatios_tmp_vector.size(); j++)
        {
            NS_LOG_DEBUG("TC:" << i << " Exponential Distributed Power for SP:" << j
                               << " is:" << SubPathRatios_tmp_vector[j]);
        }

        // For 1st Time Cluster the First SP is the SP with the strongest power
        if (i == 1 && los)
        {
            NS_LOG_DEBUG("In LOS condition first SP of 1st TC has the strongest power");
            maxElementIndex =
                std::max_element(SubPathRatios_tmp_vector.begin(), SubPathRatios_tmp_vector.end()) -
                SubPathRatios_tmp_vector.begin();
            maxElement =
                *std::max_element(SubPathRatios_tmp_vector.begin(), SubPathRatios_tmp_vector.end());
            tmp = SubPathRatios_tmp_vector[0];
            SubPathRatios_tmp_vector[0] = maxElement;
            SubPathRatios_tmp_vector[maxElementIndex] = tmp;
        }

        // sum supath powers in one time cluster
        sum_of_sp_pwr =
            std::accumulate(SubPathRatios_tmp_vector.begin(), SubPathRatios_tmp_vector.end(), 0.0);
        NS_LOG_DEBUG("Sum of SP Power in TC" << i << " is:" << sum_of_sp_pwr);

        for (j = 0; j < numberOfSubpathInTimeCluster; j++)
        {
            subPathRatios = (SubPathRatios_tmp_vector[j] / sum_of_sp_pwr) * timeClusterPowers[i];
            subPathRatios_vect.push_back(subPathRatios);
        }

        subPathPowers.push_back(subPathRatios_vect);
        u.clear();
        SubPathRatios_tmp_vector.clear();
        subPathRatios_vect.clear();
    }

    // Displaying the subpath powers generated for each time cluster for debugging
    for (i = 0; i < (int)subPathPowers.size(); i++)
    {
        for (j = 0; j < (int)subPathPowers[i].size(); j++)
        {
            NS_LOG_DEBUG("Time Cluster: " << i << " Subpath:" << j
                                          << " Power:" << subPathPowers[i][j] << std::endl);
        }
    }

    return subPathPowers;
}

MatrixBasedChannelModel::Double2DVector
NYUChannelModel::GetAbsolutePropagationTimes(
    double distance2D,
    MatrixBasedChannelModel::DoubleVector delayOfTimeCluster,
    MatrixBasedChannelModel::Double2DVector subpathDelayInTimeCluster) const
{
    NS_LOG_FUNCTION(this << distance2D);

    int numTC = 0;
    int i;
    int j;
    double time = 0;
    double absdelay_tmp = 0;

    MatrixBasedChannelModel::DoubleVector absdelay;
    MatrixBasedChannelModel::Double2DVector abssubpathDelayInTimeCluster;

    numTC = delayOfTimeCluster.size();
    time = (distance2D / M_C) * 1e9;

    NS_LOG_DEBUG("Absolute Propagation is:" << time);

    for (i = 0; i < numTC; i++)
    {
        for (j = 0; j < (int)subpathDelayInTimeCluster[i].size(); j++)
        {
            absdelay_tmp = time + delayOfTimeCluster[i] + subpathDelayInTimeCluster[i][j];
            absdelay.push_back(absdelay_tmp);
        }
        abssubpathDelayInTimeCluster.push_back(absdelay);
        absdelay.clear();
    }

    // Displaying the absolute subpath delays generated for each time cluster for debugging
    for (i = 0; i < (int)abssubpathDelayInTimeCluster.size(); i++)
    {
        for (j = 0; j < (int)abssubpathDelayInTimeCluster[i].size(); j++)
        {
            NS_LOG_DEBUG("Time Cluster: " << i << " Subpath:" << j << " Delay:"
                                          << abssubpathDelayInTimeCluster[i][j] << std::endl);
        }
    }
    return abssubpathDelayInTimeCluster;
}

MatrixBasedChannelModel::Double2DVector
NYUChannelModel::GetSubpathMappingAndAngles(
    int numberOfSpatialLobes,
    MatrixBasedChannelModel::DoubleVector numberOfSubpathInTimeCluster,
    double mean,
    double sigma,
    double stdRMSLobeElevationSpread,
    double stdRMSLobeAzimuthSpread,
    std::string azimuthDistributionType,
    std::string elevationDistributionType) const
{
    NS_LOG_FUNCTION(this << numberOfSpatialLobes << mean << sigma << stdRMSLobeElevationSpread
                         << stdRMSLobeAzimuthSpread << azimuthDistributionType
                         << elevationDistributionType);

    int i;
    int j;
    int numTC = 0;           // Number of Time Clusters
    int numSP = 0;           // Number of SubPaths
    int randomLobeIndex = 0; // Assign SP to a Spatial Lobe
    double az_min;
    double az_max; // Splitting azimuth planes
    double tmp_mean_azi_angle;
    double tmp_mean_elev_angle; // Assign angles to lobes and Subpaths in elevation and azimuth
    double deltaElev;
    double deltaAzi; // standard deviation of subpath w.r.t to mean angles of lobes in
                     // azimuth and elevation
    double subpathAzi;
    double subpathElev; // computed azimuth and elevation angles of the subpath
    double z;
    double b;

    MatrixBasedChannelModel::DoubleVector
        lobeindices; // contains lobe index from 1 to numberOfSpatialLobes
    MatrixBasedChannelModel::DoubleVector
        theta_min_array; // min splitting angles for azimuth plane.
    MatrixBasedChannelModel::DoubleVector
        theta_max_array; // max splitting angles for azimuth plane.
    MatrixBasedChannelModel::DoubleVector
        mean_ElevationAngles; // mean elevation angles for spatial lobes
    MatrixBasedChannelModel::DoubleVector
        mean_AzimuthAngles; // mean azimuth angles for spatial lobes
    MatrixBasedChannelModel::Double2DVector
        cluster_subpath_lobe_az_elev_angles; // stores the sp->tc->lobe mapping and Azimuth and
                                             // Elevation angles of each SP

    numTC = numberOfSubpathInTimeCluster.size();

    // Skip the first index i.e. index 0 as Lobe indices start from 1 to numberOfSpatialLobes.
    mean_ElevationAngles.resize(1);
    mean_AzimuthAngles.resize(1);

    // Lobe indices
    for (i = 0; i < numberOfSpatialLobes; i++)
    {
        lobeindices.push_back(i + 1);
    }

    // debugging: Lobe indices
    for (i = 0; i < (int)lobeindices.size(); i++)
    {
        NS_LOG_DEBUG("Lobe index generated is:" << lobeindices[i] << std::endl);
    }

    // Discretize azimuth plane
    for (i = 0; i < (int)lobeindices.size(); i++)
    {
        az_min = 360 * (lobeindices[i] - 1) / numberOfSpatialLobes;
        az_max = 360 * (lobeindices[i]) / numberOfSpatialLobes;
        theta_min_array.push_back(az_min);
        theta_max_array.push_back(az_max);
    }

    // debugging: theta min and theta max values
    for (i = 0; i < (int)theta_min_array.size(); i++)
    {
        NS_LOG_DEBUG("Theta min value:" << theta_min_array[i] << std::endl);
        NS_LOG_DEBUG("Theta max value:" << theta_max_array[i] << std::endl);
    }

    // compute mean elevation and azimuth angles
    for (i = 0; i < numberOfSpatialLobes; i++)
    {
        tmp_mean_elev_angle = mean + sigma * m_normalRv->GetValue();
        tmp_mean_azi_angle =
            theta_min_array[i] + (theta_max_array[i] - theta_min_array[i]) * GetUniformDist(0, 1);
        mean_ElevationAngles.push_back(tmp_mean_elev_angle);
        mean_AzimuthAngles.push_back(tmp_mean_azi_angle);
    }

    for (i = 1; i < (int)mean_ElevationAngles.size(); i++)
    {
        NS_LOG_DEBUG("Mean Elevation Angle:" << mean_ElevationAngles[i] << std::endl);
        NS_LOG_DEBUG("Mean Azimuth Angle:" << mean_AzimuthAngles[i] << std::endl);
    }

    // main code to compute SP angles and do mapping
    for (i = 0; i < numTC; i++)
    {
        numSP = numberOfSubpathInTimeCluster[i];
        for (j = 0; j < numSP; j++)
        {
            randomLobeIndex = GetDiscreteUniformDist(1, numberOfSpatialLobes);
            tmp_mean_elev_angle = mean_ElevationAngles[randomLobeIndex];
            tmp_mean_azi_angle = mean_AzimuthAngles[randomLobeIndex];

            // Azimuth Distribution Spread
            if (azimuthDistributionType == "Gaussian")
            {
                deltaAzi = stdRMSLobeAzimuthSpread * m_normalRv->GetValue();
            }
            else if (azimuthDistributionType == "Laplacian")
            {
                z = -0.5 + GetUniformDist(0, 1);
                b = stdRMSLobeAzimuthSpread / sqrt(2);
                deltaAzi = -b * GetSignum(z) * log(1 - 2 * abs(z));
            }
            else
            {
                NS_FATAL_ERROR("Invalid Azimuth Distribution Type");
            }

            // Elevation Distribution Spread
            if (elevationDistributionType == "Gaussian")
            {
                deltaElev = stdRMSLobeElevationSpread * m_normalRv->GetValue();
            }
            else if (elevationDistributionType == "Laplacian")
            {
                z = -0.5 + GetUniformDist(0, 1);
                b = stdRMSLobeElevationSpread / sqrt(2);
                deltaElev = -b * GetSignum(z) * log(1 - 2 * abs(z));
            }
            else
            {
                NS_FATAL_ERROR("Invalid Elevation Distribution Type");
            }

            subpathAzi = WrapTo360(tmp_mean_azi_angle + deltaAzi);
            subpathElev =
                GetMinimumValue((GetMaximumValue(tmp_mean_elev_angle + deltaElev, -60)), 60);

            cluster_subpath_lobe_az_elev_angles.push_back(
                {(double)i, (double)j, (double)randomLobeIndex, subpathAzi, subpathElev});
        }
    }

    // For debugging the Generated Subpaths Azimuth and Elevation angles
    for (i = 0; i < (int)cluster_subpath_lobe_az_elev_angles.size(); i++)
    {
        for (j = 0; j < (int)cluster_subpath_lobe_az_elev_angles[i].size(); j++)
        {
            if (j == 0)
            {
                NS_LOG_DEBUG("TC Id:" << cluster_subpath_lobe_az_elev_angles[i][j]);
            }
            else if (j == 1)
            {
                NS_LOG_DEBUG("SP Id:" << cluster_subpath_lobe_az_elev_angles[i][j]);
            }
            else if (j == 2)
            {
                NS_LOG_DEBUG("Lobe Id:" << cluster_subpath_lobe_az_elev_angles[i][j]);
            }
            else if (j == 3)
            {
                NS_LOG_DEBUG("azimuth:" << cluster_subpath_lobe_az_elev_angles[i][j]);
            }
            else if (j == 4)
            {
                NS_LOG_DEBUG("elevation:" << cluster_subpath_lobe_az_elev_angles[i][j]);
            }
            else
            {
                NS_FATAL_ERROR("Invalid Index Accessed");
            }
        }
    }
    return cluster_subpath_lobe_az_elev_angles;
}

MatrixBasedChannelModel::Double2DVector
NYUChannelModel::GetPowerSpectrum(
    MatrixBasedChannelModel::DoubleVector numberOfSubpathInTimeCluster,
    MatrixBasedChannelModel::Double2DVector absolutesubpathDelayInTimeCluster,
    MatrixBasedChannelModel::Double2DVector subpathPower,
    MatrixBasedChannelModel::Double2DVector subpathPhases,
    MatrixBasedChannelModel::Double2DVector subpathAodZod,
    MatrixBasedChannelModel::Double2DVector subpathAoaZoa) const
{
    NS_LOG_FUNCTION(this);
    int i;
    int j;
    int numTC = 0;
    int numSP = 0;

    double subpathDelay;
    double subpathPower_tmp;
    double subpathPhase;
    double subpath_AOD_Azi;
    double subpath_AOD_EL;
    double subpath_AOA_Azi;
    double subpath_AOA_EL;
    double subpath_AOD_Lobe;
    double subpath_AOA_Lobe;

    MatrixBasedChannelModel::Double2DVector powerSpectrum;

    numTC = numberOfSubpathInTimeCluster.size();

    for (i = 0; i < numTC; i++)
    {
        for (j = 0; j < numberOfSubpathInTimeCluster[i]; j++)
        {
            subpathDelay = absolutesubpathDelayInTimeCluster[i][j];
            subpathPower_tmp = subpathPower[i][j];
            subpathPhase = subpathPhases[numSP][0];
            subpath_AOD_Azi = subpathAodZod[j][3];
            subpath_AOD_EL = subpathAodZod[j][4];
            subpath_AOA_Azi = subpathAoaZoa[j][3];
            subpath_AOA_EL = subpathAoaZoa[j][4];
            subpath_AOD_Lobe = subpathAodZod[j][2];
            subpath_AOA_Lobe = subpathAoaZoa[j][2];

            powerSpectrum.push_back({subpathDelay,
                                     subpathPower_tmp,
                                     subpathPhase,
                                     subpath_AOD_Azi,
                                     subpath_AOD_EL,
                                     subpath_AOA_Azi,
                                     subpath_AOA_EL,
                                     subpath_AOD_Lobe,
                                     subpath_AOA_Lobe});
            numSP++;
        }
    }

    NS_LOG_DEBUG("Total Number of SP is:" << numSP);

    // Displaying the absolute subpath delays generated for each time cluster for debugging
    for (i = 0; i < (int)powerSpectrum.size(); i++)
    {
        NS_LOG_DEBUG("Subpath id:" << i << std::endl);
        for (j = 0; j < (int)powerSpectrum[i].size(); j++)
        {
            if (j == 0)
            {
                NS_LOG_DEBUG("SubpathDelay:" << powerSpectrum[i][j]);
            }
            else if (j == 1)
            {
                NS_LOG_DEBUG("SubpathPower:" << powerSpectrum[i][j]);
            }
            else if (j == 2)
            {
                NS_LOG_DEBUG("SubpathPhase:" << powerSpectrum[i][j]);
            }
            else if (j == 3)
            {
                NS_LOG_DEBUG("Subpath_AOD_Azi:" << powerSpectrum[i][j]);
            }
            else if (j == 4)
            {
                NS_LOG_DEBUG("subpath_AOD_EL:" << powerSpectrum[i][j]);
            }
            else if (j == 5)
            {
                NS_LOG_DEBUG("subpath_AOA_Azi:" << powerSpectrum[i][j]);
            }
            else if (j == 6)
            {
                NS_LOG_DEBUG("subpath_AOA_EL:" << powerSpectrum[i][j]);
            }
            else if (j == 7)
            {
                NS_LOG_DEBUG("subpath_AOD_Lobe:" << powerSpectrum[i][j]);
            }
            else if (j == 8)
            {
                NS_LOG_DEBUG("subpath_AOA_Lobe:" << powerSpectrum[i][j]);
            }
            else
            {
                NS_FATAL_ERROR("Invalid Index Accessed");
            }
        }
    }
    return powerSpectrum;
}

MatrixBasedChannelModel::Double2DVector
NYUChannelModel::GetBWAdjustedtedPowerSpectrum(
    MatrixBasedChannelModel::Double2DVector powerSpectrumOld,
    double rfBandwidth,
    bool los) const
{
    NS_LOG_FUNCTION(this << rfBandwidth << los);
    int numSP = 0; // number of Subpaths
    int i = 0;
    int j = 0;
    int index = 0;

    double MinTimeSP =
        0; // Subpaths within this duration cannot be resolved. They appear as one subpath.
    double BoundaryTime = 0;  // All Subpaths <= boundary time are combined together
    double SPcombinedPwr = 0; // Combined complex power of the SP

    std::complex<double> sum_sp = 0; // add the subpath amplitude and phase together

    // bool SetBoundaryTime = true; // indicated a new boundary time is being set.
    bool isSubpathCombined = false;
    MatrixBasedChannelModel::Double2DVector powerSpectrum;

    MinTimeSP = (1 / (m_rfBandwidth / 2)) * 1e9;
    NS_LOG_DEBUG("SP Resolution Time:" << MinTimeSP);

    numSP = powerSpectrumOld.size();

    while (i < numSP - 1)
    {
        NS_LOG_DEBUG("Subpath Id: " << i);
        powerSpectrum.push_back(powerSpectrumOld[i]);
        BoundaryTime = powerSpectrumOld[i][0] + MinTimeSP;
        NS_LOG_DEBUG("BoundaryTime:" << BoundaryTime);
        while (powerSpectrumOld[index][0] <= BoundaryTime && index < numSP - 1)
        {
            sum_sp = sum_sp + sqrt(powerSpectrumOld[index][1]) *
                                  exp(std::complex<double>(0, powerSpectrumOld[index][2]));
            isSubpathCombined = true;
            index++;
        }
        if (isSubpathCombined)
        {
            SPcombinedPwr = pow(abs(sum_sp), 2);
            powerSpectrum[powerSpectrum.size() - 1][1] = SPcombinedPwr;
            sum_sp = 0;
            i = index;
        }
    }

    if (numSP == 1)
    {
        powerSpectrum = GetLosAlignedPowerSpectrum(powerSpectrumOld, los);
    }
    else
    {
        powerSpectrum = GetLosAlignedPowerSpectrum(powerSpectrum, los);
    }

    NS_LOG_DEBUG(
        "Final powerSpectrum values after BW Adjustment, Total SP:" << powerSpectrum.size());

    // Displaying the absolute subpath delays generated for each time cluster for debugging
    for (i = 0; i < (int)powerSpectrum.size(); i++)
    {
        NS_LOG_DEBUG("Subpath ID:" << i);
        for (j = 0; j < (int)powerSpectrum[i].size(); j++)
        {
            if (j == 0)
            {
                NS_LOG_DEBUG("SubpathDelay:" << powerSpectrum[i][j]);
            }
            else if (j == 1)
            {
                NS_LOG_DEBUG("SubpathPower:" << powerSpectrum[i][j]);
            }
            else if (j == 2)
            {
                NS_LOG_DEBUG("SubpathPhase:" << powerSpectrum[i][j]);
            }
            else if (j == 3)
            {
                NS_LOG_DEBUG("Subpath_AOD_Azi:" << powerSpectrum[i][j]);
            }
            else if (j == 4)
            {
                NS_LOG_DEBUG("subpath_AOD_EL:" << powerSpectrum[i][j]);
            }
            else if (j == 5)
            {
                NS_LOG_DEBUG("subpath_AOA_Azi:" << powerSpectrum[i][j]);
            }
            else if (j == 6)
            {
                NS_LOG_DEBUG("subpath_AOA_EL:" << powerSpectrum[i][j]);
            }
            else if (j == 7)
            {
                NS_LOG_DEBUG("subpath_AOD_Lobe:" << powerSpectrum[i][j]);
            }
            else if (j == 8)
            {
                NS_LOG_DEBUG("subpath_AOA_Lobe:" << powerSpectrum[i][j]);
            }
            else
            {
                NS_FATAL_ERROR("Invalid Index Accessed");
            }
        }
    }

    return powerSpectrum;
}

MatrixBasedChannelModel::Double2DVector
NYUChannelModel::GetLosAlignedPowerSpectrum(MatrixBasedChannelModel::Double2DVector& powerSpectrum,
                                            bool los) const
{
    NS_LOG_FUNCTION(this << los << m_scenario);

    double correctAzAOA = 0;
    double diffAzAOA = 0;
    double correctELAOA = 0;
    double diffElAOA = 0;

    int i;

    // In LOS the first Subpath i.e. Subpath 0 in AOD and AOA , ZOD and ZOA should be aligned.
    if (los)
    {
        // Subpath 0 - Azimuth AOD
        if (powerSpectrum[0][3] - 180 > 0)
        {
            correctAzAOA = powerSpectrum[0][3] - 180;
        }
        else
        {
            correctAzAOA = powerSpectrum[0][3] + 180;
        }
        NS_LOG_DEBUG("Corrected Az AOA is:" << correctAzAOA << std::endl);

        // Calculate the difference between generated azimuth AOA and correct Azimuth AOA.
        diffAzAOA = powerSpectrum[0][5] - correctAzAOA;
        NS_LOG_DEBUG("Diff between generated Az AOA and corrected Az AOA is:" << diffAzAOA
                                                                              << std::endl);

        // Correct all AOA w.r.t to the AOA of the first LOS subpath
        for (i = 0; i < (int)powerSpectrum.size(); i++)
        {
            powerSpectrum[i][5] = powerSpectrum[i][5] - diffAzAOA;
            powerSpectrum[i][5] = WrapTo360(powerSpectrum[i][5]);
        }

        // Debug SP AOA alignment
        for (i = 0; i < (int)powerSpectrum.size(); i++)
        {
            NS_LOG_DEBUG("Adjusted AOA for Subpath" << i << " is:" << powerSpectrum[i][5]
                                                    << std::endl);
        }

        // Fetch the ZOD elevation
        correctELAOA = -powerSpectrum[0][4];
        NS_LOG_DEBUG("Corrected Az ZOA is:" << correctELAOA << std::endl);

        // Calculate the difference between generated ZOA and correct Azimuth ZOD.
        diffElAOA = powerSpectrum[0][6] - correctELAOA;
        NS_LOG_DEBUG("Diff between generated Az ZOA and corrected Az ZOA is:" << diffElAOA
                                                                              << std::endl);

        // Correct all ZOA w.r.t to the ZOA of the first LOS subpath
        for (i = 0; i < (int)powerSpectrum.size(); i++)
        {
            powerSpectrum[i][6] = powerSpectrum[i][6] - diffElAOA;
            if (powerSpectrum[i][6] > 90)
            {
                powerSpectrum[i][6] = 180 - powerSpectrum[i][6];
            }
            else if (powerSpectrum[i][6] < -90)
            {
                powerSpectrum[i][6] = -180 - powerSpectrum[i][6];
            }
        }

        // Debug SP AOA alignment
        for (i = 0; i < (int)powerSpectrum.size(); i++)
        {
            NS_LOG_DEBUG("Adjusted ZOA for Subpath" << i << " is:" << powerSpectrum[i][6]
                                                    << std::endl);
        }
    }
    else
    {
        NS_LOG_DEBUG("powerSpectrum alignment not needed, scnario is:" << m_scenario << std::endl);
    }
    return powerSpectrum;
}

MatrixBasedChannelModel::Double2DVector
NYUChannelModel::GetValidSubapths(MatrixBasedChannelModel::Double2DVector powerSpectrum,
                                  double pwrthreshold) const
{
    NS_LOG_FUNCTION(this << pwrthreshold);

    MatrixBasedChannelModel::Double2DVector powerSpectrumOptimized;
    double maxSubpathPower = 0;
    double maxSubpathPowerID = 500; // 500 is a dummy subpath id
    double threshold = 0;           // in dB
    double subpathPower = 0;        // subpath Power in dB

    for (int i = 0; i < (int)powerSpectrum.size(); i++)
    {
        if (powerSpectrum[i][1] > maxSubpathPower)
        {
            maxSubpathPower = powerSpectrum[i][1];
            maxSubpathPowerID = i;
        }
    }

    threshold = 10 * log10(maxSubpathPower) - pwrthreshold;
    NS_LOG_DEBUG("Max Subpath Power lin_scale:" << maxSubpathPower << " Max Subpath Power ID:"
                                                << maxSubpathPowerID << " threshold:" << threshold);

    // for all subpaths above the threshold save the Power spectrum
    for (int i = 0; i < (int)powerSpectrum.size(); i++)
    {
        subpathPower = 10 * log10(powerSpectrum[i][1]);
        if (subpathPower > threshold)
        {
            powerSpectrumOptimized.push_back(powerSpectrum[i]);
        }
    }

    NS_LOG_DEBUG("Total Number of Subpath after removing weak subpaths is: "
                 << powerSpectrumOptimized.size());

    return powerSpectrumOptimized;
}

MatrixBasedChannelModel::Double2DVector
NYUChannelModel::GetXpdPerSubpath(double totalNumberOfSubpaths, double xpdMean, double xpdSd) const
{
    NS_LOG_FUNCTION(this << totalNumberOfSubpaths << xpdMean << xpdSd);
    MatrixBasedChannelModel::Double2DVector XPD;
    int i;
    int j;
    // Polarization values for HH (phi_phi), VH(theta_phi), HV (phi_theta)
    double phi_phi;
    double theta_phi;
    double phi_theta;

    for (i = 0; i < totalNumberOfSubpaths; i++)
    {
        phi_phi = m_normalRv->GetValue() * xpdSd;
        theta_phi = xpdMean;
        phi_theta = xpdMean + m_normalRv->GetValue() * xpdSd;
        XPD.push_back({phi_phi, theta_phi, phi_theta});
    }

    // debugging XPD values for each Ray
    for (i = 0; i < (int)XPD.size(); i++)
    {
        for (j = 0; j < (int)XPD[i].size(); j++)
        {
            if (j == 0)
            {
                NS_LOG_DEBUG(" HH XPD value for ray" << i << " is:" << XPD[i][j]);
            }
            else if (j == 1)
            {
                NS_LOG_DEBUG(" VH XPD value for ray" << i << " is:" << XPD[i][j]);
            }
            else if (j == 2)
            {
                NS_LOG_DEBUG(" HV XPD value for ray" << i << " is:" << XPD[i][j]);
            }
        }
    }
    return XPD;
}

MatrixBasedChannelModel::Double2DVector
NYUChannelModel::NYUCoordinateSystemToGlobalCoordinateSystem(
    MatrixBasedChannelModel::Double2DVector powerSpectrum) const
{
    NS_LOG_FUNCTION(this);

    MatrixBasedChannelModel::DoubleVector rayAodDegree;
    MatrixBasedChannelModel::DoubleVector rayZodDegree;
    MatrixBasedChannelModel::DoubleVector rayAoaDegree;
    MatrixBasedChannelModel::DoubleVector rayZoaDegree;

    MatrixBasedChannelModel::DoubleVector rayAodRadian;
    MatrixBasedChannelModel::DoubleVector rayZodRadian;
    MatrixBasedChannelModel::DoubleVector rayAoaRadian;
    MatrixBasedChannelModel::DoubleVector rayZoaRadian;

    MatrixBasedChannelModel::Double2DVector m_angle;

    // Storing the AOD,ZOD,AOA,ZOA values and changing it from NYU coordinates to 3GPP GCS
    // Col 3 - AOD , Col 4 - ZOD, Col 5 - AOA, Col 6 - ZOA (all values in degrees)
    for (int i = 0; i < (int)powerSpectrum.size(); i++)
    {
        rayAodDegree.push_back(powerSpectrum[i][3]);
        rayZodDegree.push_back(powerSpectrum[i][4]);
        rayAoaDegree.push_back(powerSpectrum[i][5]);
        rayZoaDegree.push_back(powerSpectrum[i][6]);
    }

    // Transforming NYU coordinate system to GCS. Subtract (90-theta) for elevation and (90-phi)%360
    // for azimuth to change NYU measurement coordinate system to GCS.
    for (int i = 0; i < (int)powerSpectrum.size(); i++)
    {
        rayAodDegree[i] = WrapTo360(90 - rayAodDegree[i]);
        rayZodDegree[i] = 90 - rayZodDegree[i];
        rayAoaDegree[i] = WrapTo360(90 - rayAoaDegree[i]);
        rayZoaDegree[i] = 90 - rayZoaDegree[i];
    }

    // Debug for NYU to GCS converted Ray characteristics in degrees (AOD,ZOD,AOA,ZOA)
    for (int i = 0; i < (int)powerSpectrum.size(); i++)
    {
        NS_LOG_DEBUG("Subpath:" << i << " GCS AOD:" << rayAodDegree[i] << " degree");
        NS_LOG_DEBUG("Subpath:" << i << " GCS ZOD:" << rayZodDegree[i] << " degree");
        NS_LOG_DEBUG("Subpath:" << i << " GCS AOA:" << rayAoaDegree[i] << " degree");
        NS_LOG_DEBUG("Subpath:" << i << " GCS ZOA:" << rayZoaDegree[i] << " degree");
    }

    // Store the AOD,ZOD,AOA,ZOA in radians for each ray according to GCS.
    rayAodRadian = DegreesToRadians(rayAodDegree);
    rayZodRadian = DegreesToRadians(rayZodDegree);
    rayAoaRadian = DegreesToRadians(rayAoaDegree);
    rayZoaRadian = DegreesToRadians(rayZoaDegree);

    // Debug for NYU to GCS converted Ray characteristics in radians (AOD,ZOD,AOA,ZOA)
    for (int i = 0; i < (int)powerSpectrum.size(); i++)
    {
        NS_LOG_DEBUG("Subpath:" << i << " GCS AOD:" << rayAodRadian[i] << " radian");
        NS_LOG_DEBUG("Subpath:" << i << " GCS ZOD:" << rayZodRadian[i] << " radian");
        NS_LOG_DEBUG("Subpath:" << i << " GCS AOA:" << rayAoaRadian[i] << " radian");
        NS_LOG_DEBUG("Subpath:" << i << " GCS ZOA:" << rayZoaRadian[i] << " radian");
    }

    // m_angle is in matrix-based-channel-model.h and we populate the value in radians
    // for AOD,ZOD,AOA,ZOA in m_angle. This is then used in CalcBeamformingGain() in
    // nyu-spectrum-propagation-loss-model.cc m_angle row 0 - aoa , row 1 - zoa, row 2 -aod, row 3
    // -zod (for all SP)
    m_angle.push_back(rayAoaRadian);
    m_angle.push_back(rayZoaRadian);
    m_angle.push_back(rayAodRadian);
    m_angle.push_back(rayZodRadian);

    return m_angle;
}

} // namespace ns3
