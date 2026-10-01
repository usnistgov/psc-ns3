// Copyright (c) 2023 New York University and NYU WIRELESS
// Users are encouraged to cite NYU WIRELESS publications regarding this work.
// Original source code is available in https://github.com/hiteshPoddar/NYUSIM_in_ns3
//
// SPDX-License-Identifier: MIT

#include "nyu-spectrum-propagation-loss-model.h"

#include "nyu-channel-model.h"

#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"
#include "ns3/spectrum-signal-parameters.h"
#include "ns3/string.h"

#include <map>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NYUSpectrumPropagationLossModel");

NS_OBJECT_ENSURE_REGISTERED(NYUSpectrumPropagationLossModel);

NYUSpectrumPropagationLossModel::NYUSpectrumPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
}

NYUSpectrumPropagationLossModel::~NYUSpectrumPropagationLossModel()
{
    NS_LOG_FUNCTION(this);
}

void
NYUSpectrumPropagationLossModel::DoDispose()
{
    m_longTermMap.clear();
    m_channelModel->Dispose();
    m_channelModel = nullptr;
}

TypeId
NYUSpectrumPropagationLossModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NYUSpectrumPropagationLossModel")
            .SetParent<PhasedArraySpectrumPropagationLossModel>()
            .SetGroupName("Spectrum")
            .AddConstructor<NYUSpectrumPropagationLossModel>()
            .AddAttribute(
                "ChannelModel",
                "The channel model. It needs to implement the MatrixBasedChannelModel interface",
                StringValue("ns3::NYUChannelModel"),
                MakePointerAccessor(&NYUSpectrumPropagationLossModel::SetChannelModel,
                                    &NYUSpectrumPropagationLossModel::GetChannelModel),
                MakePointerChecker<MatrixBasedChannelModel>());
    return tid;
}

void
NYUSpectrumPropagationLossModel::SetChannelModel(Ptr<MatrixBasedChannelModel> channel)
{
    m_channelModel = channel;
}

Ptr<MatrixBasedChannelModel>
NYUSpectrumPropagationLossModel::GetChannelModel() const
{
    return m_channelModel;
}

double
NYUSpectrumPropagationLossModel::GetFrequency() const
{
    DoubleValue freq;
    m_channelModel->GetAttribute("Frequency", freq);
    return freq.Get();
}

void
NYUSpectrumPropagationLossModel::SetChannelModelAttribute(const std::string& name,
                                                          const AttributeValue& value)
{
    m_channelModel->SetAttribute(name, value);
}

void
NYUSpectrumPropagationLossModel::GetChannelModelAttribute(const std::string& name,
                                                          AttributeValue& value) const
{
    m_channelModel->GetAttribute(name, value);
}

PhasedArrayModel::ComplexVector
NYUSpectrumPropagationLossModel::CalcLongTerm(
    Ptr<const MatrixBasedChannelModel::ChannelMatrix> params,
    const PhasedArrayModel::ComplexVector& sW,
    const PhasedArrayModel::ComplexVector& uW) const
{
    NS_LOG_FUNCTION(this);

    size_t uAntennaNum = uW.GetSize();
    size_t sAntennaNum = sW.GetSize();

    NS_ASSERT(uAntennaNum == params->m_channel.GetNumRows());
    NS_ASSERT(sAntennaNum == params->m_channel.GetNumCols());

    NS_LOG_DEBUG("CalcLongTerm with " << uAntennaNum << " u antenna elements and " << sAntennaNum
                                      << " s antenna elements.");
    // store the long term part to reduce computation load
    // only the small scale fading needs to be updated if the large scale parameters and antenna
    // weights remain unchanged. here we calculate long term uW * Husn * sW, the result is an array
    // of values per cluster
    return params->m_channel.MultiplyByLeftAndRightMatrix(uW.Transpose(), sW);
}

Ptr<SpectrumValue>
NYUSpectrumPropagationLossModel::CalcBeamformingGain(
    Ptr<SpectrumValue> txPsd,
    PhasedArrayModel::ComplexVector longTerm,
    Ptr<const MatrixBasedChannelModel::ChannelMatrix> channelMatrix,
    Ptr<const MatrixBasedChannelModel::ChannelParams> channelParams,
    const ns3::Vector& sSpeed,
    const ns3::Vector& uSpeed) const
{
    NS_LOG_FUNCTION(this);

    Ptr<SpectrumValue> tempPsd = Copy<SpectrumValue>(txPsd);

    // channel[rx][tx][cluster]
    uint16_t numRays = channelMatrix->m_channel.GetNumPages();

    // compute the doppler term
    // NOTE the update of Doppler is simplified by only taking the center angle of
    // each cluster in to consideration.
    double slotTime = Simulator::Now().GetSeconds();
    double factor = 2 * M_PI * slotTime * GetFrequency() / 3e8;
    PhasedArrayModel::ComplexVector doppler(numRays);

    // check if channelParams structure is generated in direction s-to-u or u-to-s
    bool isSameDirection = (channelParams->m_nodeIds == channelMatrix->m_nodeIds);

    MatrixBasedChannelModel::DoubleVector zoa;
    MatrixBasedChannelModel::DoubleVector zod;
    MatrixBasedChannelModel::DoubleVector aoa;
    MatrixBasedChannelModel::DoubleVector aod;

    // if channel params is generated in the same direction in which we
    // generate the channel matrix, angles and zenit od departure and arrival are ok,
    // just set them to corresponding variable that will be used for the generation
    // of channel matrix, otherwise we need to flip angles and zenits of departure and arrival
    if (isSameDirection)
    {
        zoa = channelParams->m_angle[MatrixBasedChannelModel::ZOA_INDEX];
        zod = channelParams->m_angle[MatrixBasedChannelModel::ZOD_INDEX];
        aoa = channelParams->m_angle[MatrixBasedChannelModel::AOA_INDEX];
        aod = channelParams->m_angle[MatrixBasedChannelModel::AOD_INDEX];
    }
    else
    {
        zod = channelParams->m_angle[MatrixBasedChannelModel::ZOA_INDEX];
        zoa = channelParams->m_angle[MatrixBasedChannelModel::ZOD_INDEX];
        aod = channelParams->m_angle[MatrixBasedChannelModel::AOA_INDEX];
        aoa = channelParams->m_angle[MatrixBasedChannelModel::AOD_INDEX];
    }

    for (size_t cIndex = 0; cIndex < numRays; cIndex++)
    {
        // Compute alpha and D as described in 3GPP TR 37.885 v15.3.0, Sec. 6.2.3
        // These terms account for an additional Doppler contribution due to the
        // presence of moving objects in the surrounding environment, such as in
        // vehicular scenarios.
        // This contribution is applied only to the delayed (reflected) paths and
        // must be properly configured by setting the value of
        // m_vScatt, which is defined as "maximum speed of the vehicle in the
        // layout".
        // By default, m_vScatt is set to 0, so there is no additional Doppler
        // contribution.

        // cluster angle angle[direction][n], where direction = 0(aoa), 1(zoa).
        double tempDoppler =
            factor *
            ((sin(zoa[cIndex]) * cos(aoa[cIndex]) * uSpeed.x +
              sin(zoa[cIndex]) * sin(aoa[cIndex]) * uSpeed.y + cos(zoa[cIndex]) * uSpeed.z) +
             (sin(zod[cIndex]) * cos(aod[cIndex]) * sSpeed.x +
              sin(zod[cIndex]) * sin(aod[cIndex]) * sSpeed.y + cos(zod[cIndex]) * sSpeed.z));
        doppler[cIndex] = std::complex<double>(cos(tempDoppler), sin(tempDoppler));
    }

    NS_ASSERT(numRays <= doppler.GetSize());

    // apply the doppler term and the propagation delay to the long term component
    // to obtain the beamforming gain
    auto vit = tempPsd->ValuesBegin();      // psd iterator
    auto sbit = tempPsd->ConstBandsBegin(); // band iterator
    while (vit != tempPsd->ValuesEnd())
    {
        if ((*vit) != 0.00)
        {
            std::complex<double> subsbandGain(0.0, 0.0);
            double fsb = (*sbit).fc; // center frequency of the sub-band
            for (size_t cIndex = 0; cIndex < numRays; cIndex++)
            {
                double delay = -2 * M_PI * fsb * (channelParams->m_delay[cIndex]) * 1e-9;
                subsbandGain = subsbandGain + longTerm[cIndex] *
                                                  std::complex<double>(cos(delay), sin(delay)) *
                                                  doppler[cIndex];
            }
            *vit = (*vit) * (norm(subsbandGain));
        }
        vit++;
        sbit++;
    }
    return tempPsd;
}

PhasedArrayModel::ComplexVector
NYUSpectrumPropagationLossModel::GetLongTerm(
    Ptr<const MatrixBasedChannelModel::ChannelMatrix> channelMatrix,
    Ptr<const PhasedArrayModel> aPhasedArrayModel,
    Ptr<const PhasedArrayModel> bPhasedArrayModel) const
{
    PhasedArrayModel::ComplexVector
        longTerm; // vector containing the long term component for each cluster

    // check if the channel matrix was generated considering a as the s-node and
    // b as the u-node or vice-versa
    PhasedArrayModel::ComplexVector sW;
    PhasedArrayModel::ComplexVector uW;
    if (!channelMatrix->IsReverse(aPhasedArrayModel->GetId(), bPhasedArrayModel->GetId()))
    {
        sW = aPhasedArrayModel->GetBeamformingVector();
        uW = bPhasedArrayModel->GetBeamformingVector();
    }
    else
    {
        sW = bPhasedArrayModel->GetBeamformingVector();
        uW = aPhasedArrayModel->GetBeamformingVector();
    }

    bool update = false;   // indicates whether the long term has to be updated
    bool notFound = false; // indicates if the long term has not been computed yet

    // compute the long term key, the key is unique for each tx-rx pair
    uint64_t longTermId =
        MatrixBasedChannelModel::GetKey(aPhasedArrayModel->GetId(), bPhasedArrayModel->GetId());

    // look for the long term in the map and check if it is valid
    if (m_longTermMap.find(longTermId) != m_longTermMap.end())
    {
        NS_LOG_DEBUG("found the long term component in the map");
        longTerm = m_longTermMap[longTermId]->m_longTerm;

        // check if the channel matrix has been updated
        // or the s beam has been changed
        // or the u beam has been changed
        update = (m_longTermMap[longTermId]->m_channel->m_generatedTime !=
                      channelMatrix->m_generatedTime ||
                  m_longTermMap[longTermId]->m_sW != sW || m_longTermMap[longTermId]->m_uW != uW);
    }
    else
    {
        NS_LOG_DEBUG("long term component NOT found");
        notFound = true;
    }

    if (update || notFound)
    {
        NS_LOG_DEBUG("compute the long term");
        // compute the long term component
        longTerm = CalcLongTerm(channelMatrix, sW, uW);

        // store the long term
        Ptr<LongTerm> longTermItem = Create<LongTerm>();
        longTermItem->m_longTerm = longTerm;
        longTermItem->m_channel = channelMatrix;
        longTermItem->m_sW = sW;
        longTermItem->m_uW = uW;

        m_longTermMap[longTermId] = longTermItem;
    }

    return longTerm;
}

Ptr<SpectrumSignalParameters>
NYUSpectrumPropagationLossModel::DoCalcRxPowerSpectralDensity(
    Ptr<const SpectrumSignalParameters> params,
    Ptr<const MobilityModel> a,
    Ptr<const MobilityModel> b,
    Ptr<const PhasedArrayModel> aPhasedArrayModel,
    Ptr<const PhasedArrayModel> bPhasedArrayModel) const
{
    NS_LOG_FUNCTION(this);
    uint32_t aId = a->GetObject<Node>()->GetId(); // id of the node a
    uint32_t bId = b->GetObject<Node>()->GetId(); // id of the node b

    NS_ASSERT(aId != bId);
    NS_ASSERT_MSG(a->GetDistanceFrom(b) > 0.0,
                  "The position of a and b devices cannot be the same");

    auto rxParams = params->Copy();

    // retrieve the antenna of device a
    NS_ASSERT_MSG(aPhasedArrayModel, "Antenna not found for node " << aId);

    // retrieve the antenna of the device b
    NS_ASSERT_MSG(bPhasedArrayModel, "Antenna not found for device " << bId);

    Ptr<const MatrixBasedChannelModel::ChannelMatrix> channelMatrix =
        m_channelModel->GetChannel(a, b, aPhasedArrayModel, bPhasedArrayModel);
    Ptr<const MatrixBasedChannelModel::ChannelParams> channelParams =
        m_channelModel->GetParams(a, b);

    // retrieve the long term component
    PhasedArrayModel::ComplexVector longTerm =
        GetLongTerm(channelMatrix, aPhasedArrayModel, bPhasedArrayModel);

    // apply the beamforming gain
    rxParams->psd = CalcBeamformingGain(rxParams->psd,
                                        longTerm,
                                        channelMatrix,
                                        channelParams,
                                        a->GetVelocity(),
                                        b->GetVelocity());

    return rxParams;
}

int64_t
NYUSpectrumPropagationLossModel::DoAssignStreams(int64_t stream)
{
    return 0;
}

} // namespace ns3
