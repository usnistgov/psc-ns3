// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "xr-traffic-mixer-helper.h"

#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("XrTrafficMixerHelper");
NS_OBJECT_ENSURE_REGISTERED(XrTrafficMixerHelper);

const std::map<NrXrConfig, std::list<TypeId>> XrPreconfig = {
    {AR_M3,
     {TrafficGenerator3gppPoseControl::GetTypeId(),
      TrafficGenerator3gppGenericVideo::GetTypeId(),
      TrafficGenerator3gppAudioData::GetTypeId()}},
    {AR_M3_V2,
     {TrafficGenerator3gppPoseControl::GetTypeId(),
      TrafficGeneratorNgmnVideo::GetTypeId(),
      TrafficGenerator3gppAudioData::GetTypeId()}},
    {VR_DL1, {TrafficGenerator3gppGenericVideo::GetTypeId()}},
    {VR_DL2,
     {TrafficGenerator3gppGenericVideo::GetTypeId(), TrafficGenerator3gppAudioData::GetTypeId()}},
    {VR_UL, {TrafficGenerator3gppPoseControl::GetTypeId()}},
    {CG_DL1, {TrafficGenerator3gppGenericVideo::GetTypeId()}},
    {CG_DL2,
     {TrafficGenerator3gppGenericVideo::GetTypeId(), TrafficGenerator3gppAudioData::GetTypeId()}},
    {CG_UL, {TrafficGenerator3gppPoseControl::GetTypeId()}},
    {NGMN_VOICE, {TrafficGeneratorNgmnVoip::GetTypeId()}},
};

enum NrXrConfig
GetXrTrafficType(const std::string& item)
{
    if (item == "AR_M3")
    {
        return AR_M3;
    }
    else if (item == "AR_M3_V2")
    {
        return AR_M3_V2;
    }
    else if (item == "VR_DL1")
    {
        return VR_DL1;
    }
    else if (item == "VR_DL2")
    {
        return VR_DL2;
    }
    else if (item == "VR_UL")
    {
        return VR_UL;
    }
    else if (item == "CG_DL1")
    {
        return CG_DL1;
    }
    else if (item == "CG_DL2")
    {
        return CG_DL2;
    }
    else if (item == "CG_UL")
    {
        return CG_UL;
    }
    else if (item == "NGMN_VOICE")
    {
        return NGMN_VOICE;
    }
    else
    {
        NS_ABORT_MSG("Unknown traffic type");
    }
}

std::string
GetXrTrafficName(const NrXrConfig& item)
{
    switch (item)
    {
    case AR_M3:
        return "AR_M3";
    case AR_M3_V2:
        return "AR_M3_V2";
    case VR_DL1:
        return "VR_DL1";
    case VR_DL2:
        return "VR_DL2";
    case VR_UL:
        return "VR_UL";
    case CG_DL1:
        return "CG_DL1";
    case CG_DL2:
        return "CG_DL2";
    case CG_UL:
        return "CG_UL";
    case NGMN_VOICE:
        return "NGMN_VOICE";
    default:
        NS_ABORT_MSG("Unknown traffic type");
    };
}

TypeId
XrTrafficMixerHelper::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::XrTrafficMixerHelper").SetParent<Object>().SetGroupName("Applications");
    return tid;
}

XrTrafficMixerHelper::XrTrafficMixerHelper()
{
    NS_LOG_FUNCTION(this);
}

void
XrTrafficMixerHelper::AddStream(TypeId trafficGenerator)
{
    NS_LOG_FUNCTION(this);
    m_trafficStreams.push_back(trafficGenerator);
}

XrTrafficMixerHelper::~XrTrafficMixerHelper()
{
    NS_LOG_FUNCTION(this);
    m_trafficStreams.clear();
}

void
XrTrafficMixerHelper::ConfigureXr(NrXrConfig xrTrafficType)
{
    NS_LOG_FUNCTION(this);
    auto it = XrPreconfig.find(xrTrafficType);
    NS_ASSERT_MSG(it != XrPreconfig.end(), "Unknown NrXrConfig configuration.");
    NS_ASSERT_MSG(m_trafficStreams.empty(),
                  "Some traffic streams were already set. Default XR configuration failed.");
    for (const auto& streamType : it->second)
    {
        m_trafficStreams.push_back(streamType);
    }
}

ApplicationContainer
XrTrafficMixerHelper::Install(std::string transportProtocol,
                              std::vector<Address>& remoteAddresses,
                              Ptr<Node> trafficGeneratorNode)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(!m_trafficStreams.empty() && m_trafficStreams.size() <= 3);
    NS_ASSERT(!remoteAddresses.empty() && remoteAddresses.size() <= 3);
    NS_ASSERT(remoteAddresses.size() >= m_trafficStreams.size());
    uint16_t index = 0;

    ApplicationContainer trafficGeneratorApps;
    for (const auto& trafficTypeId : m_trafficStreams)
    {
        TrafficGeneratorHelper trafficHelper(transportProtocol,
                                             remoteAddresses[index],
                                             trafficTypeId);
        trafficGeneratorApps.Add(trafficHelper.Install(trafficGeneratorNode));
        index++;
    }
    return trafficGeneratorApps;
}

} // Namespace ns3
