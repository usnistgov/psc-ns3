// Copyright (c) 2021 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "distance-based-three-gpp-spectrum-propagation-loss-model.h"

#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"
#include "ns3/spectrum-signal-parameters.h"
#include "ns3/string.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("DistanceBasedThreeGppSpectrumPropagationLossModel");
NS_OBJECT_ENSURE_REGISTERED(DistanceBasedThreeGppSpectrumPropagationLossModel);

DistanceBasedThreeGppSpectrumPropagationLossModel::
    DistanceBasedThreeGppSpectrumPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
}

DistanceBasedThreeGppSpectrumPropagationLossModel::
    ~DistanceBasedThreeGppSpectrumPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
}

TypeId
DistanceBasedThreeGppSpectrumPropagationLossModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::DistanceBasedThreeGppSpectrumPropagationLossModel")
            .SetParent<ThreeGppSpectrumPropagationLossModel>()
            .SetGroupName("Spectrum")
            .AddConstructor<DistanceBasedThreeGppSpectrumPropagationLossModel>()
            .AddAttribute(
                "MaxDistance",
                "The maximum distance in meters between nodes in order to calculate fast fading "
                "and beamforming."
                "For all signals for which nodes are at higher distance will be returned 0 PSD.",
                DoubleValue(1000),
                MakeDoubleAccessor(
                    &DistanceBasedThreeGppSpectrumPropagationLossModel::SetMaxDistance,
                    &DistanceBasedThreeGppSpectrumPropagationLossModel::GetMaxDistance),
                MakeDoubleChecker<double>());
    return tid;
}

void
DistanceBasedThreeGppSpectrumPropagationLossModel::SetMaxDistance(double maxDistance)
{
    m_maxDistance = maxDistance;
}

double
DistanceBasedThreeGppSpectrumPropagationLossModel::GetMaxDistance() const
{
    return m_maxDistance;
}

Ptr<SpectrumSignalParameters>
DistanceBasedThreeGppSpectrumPropagationLossModel::DoCalcRxPowerSpectralDensity(
    Ptr<const SpectrumSignalParameters> params,
    Ptr<const MobilityModel> a,
    Ptr<const MobilityModel> b,
    Ptr<const PhasedArrayModel> aPhasedArrayModel,
    Ptr<const PhasedArrayModel> bPhasedArrayModel) const
{
    NS_LOG_FUNCTION(this);
    uint32_t aId = a->GetObject<Node>()->GetId(); // id of the node a
    uint32_t bId = b->GetObject<Node>()->GetId(); // id of the node b

    Ptr<SpectrumSignalParameters> rxParams = Copy<SpectrumSignalParameters>(params);
    if (a->GetDistanceFrom(b) > m_maxDistance)
    {
        NS_LOG_LOGIC("Distance between a: "
                     << aId << "and  node b: " << bId
                     << " is higher than max allowed distance. Return 0 PSD.");
        *(rxParams->psd) = 0.0;
        return rxParams;
    }
    else
    {
        return ThreeGppSpectrumPropagationLossModel::DoCalcRxPowerSpectralDensity(
            params,
            a,
            b,
            aPhasedArrayModel,
            bPhasedArrayModel);
    }

    return rxParams;
}

} // namespace ns3
