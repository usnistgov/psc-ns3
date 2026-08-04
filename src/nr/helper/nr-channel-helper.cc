// Copyright (c) 2024 LASSE / Universidade Federal do Pará (UFPA)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
// Author: João Albuquerque <joao.barbosa.albuquerque@itec.ufpa.br>

#include "nr-channel-helper.h"

#include <ns3/buildings-channel-condition-model.h>
#include <ns3/double.h>
#include <ns3/enum.h>
#include <ns3/multi-model-spectrum-channel.h>
#include <ns3/nyu-propagation-loss-model.h>
#include <ns3/nyu-spectrum-propagation-loss-model.h>
#include <ns3/object-factory.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>
#include <ns3/string.h>
#include <ns3/three-gpp-channel-model.h>
#include <ns3/three-gpp-propagation-loss-model.h>
#include <ns3/three-gpp-spectrum-propagation-loss-model.h>
#include <ns3/three-gpp-v2v-channel-condition-model.h>
#include <ns3/three-gpp-v2v-propagation-loss-model.h>
#include <ns3/two-ray-spectrum-propagation-loss-model.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrChannelHelper");

TypeId
NrChannelHelper::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrChannelHelper")
            .SetParent<Object>()
            .AddConstructor<NrChannelHelper>()
            .AddAttribute("Scenario",
                          "The spectrum channel scenario",
                          EnumValue(NrChannelHelper::Scenario::RMa),
                          MakeEnumAccessor<NrChannelHelper::Scenario>(&NrChannelHelper::m_scenario),
                          MakeEnumChecker(NrChannelHelper::Scenario::InF,
                                          "InF",
                                          NrChannelHelper::Scenario::InH,
                                          "InH",
                                          NrChannelHelper::Scenario::UMa,
                                          "UMa",
                                          NrChannelHelper::Scenario::UMi,
                                          "UMi",
                                          NrChannelHelper::Scenario::RMa,
                                          "RMa",
                                          NrChannelHelper::Scenario::InH_OfficeMixed,
                                          "InH-OfficeMixed",
                                          NrChannelHelper::Scenario::InH_OfficeOpen,
                                          "InH-OfficeOpen",
                                          NrChannelHelper::Scenario::V2V_Highway,
                                          "V2V-Highway",
                                          NrChannelHelper::Scenario::V2V_Urban,
                                          "V2V-Urban",
                                          NrChannelHelper::Scenario::NTN_DenseUrban,
                                          "NTN-DenseUrban",
                                          NrChannelHelper::Scenario::NTN_Urban,
                                          "NTN-Urban",
                                          NrChannelHelper::Scenario::NTN_Suburban,
                                          "NTN-Suburban",
                                          NrChannelHelper::Scenario::NTN_Rural,
                                          "NTN-Rural",
                                          NrChannelHelper::Scenario::Custom,
                                          "Custom"))
            .AddAttribute(
                "ChannelCondition",
                "The spectrum channel condition",
                EnumValue(NrChannelHelper::Condition::Default),
                MakeEnumAccessor<NrChannelHelper::Condition>(&NrChannelHelper::m_condition),
                MakeEnumChecker(NrChannelHelper::Condition::NLOS,
                                "NLOS",
                                NrChannelHelper::Condition::LOS,
                                "LOS",
                                NrChannelHelper::Condition::Buildings,
                                "Buildings",
                                NrChannelHelper::Condition::Default,
                                "Default"))
            .AddAttribute(
                "ChannelModel",
                "The spectrum channel fading model",
                EnumValue(NrChannelHelper::ChannelModel::ThreeGpp),
                MakeEnumAccessor<NrChannelHelper::ChannelModel>(&NrChannelHelper::m_channelModel),
                MakeEnumChecker(NrChannelHelper::ChannelModel::ThreeGpp,
                                "ThreeGpp",
                                NrChannelHelper::ChannelModel::NYU,
                                "NYU",
                                NrChannelHelper::ChannelModel::TwoRay,
                                "TwoRay"));
    return tid;
}

Ptr<SpectrumChannel>
NrChannelHelper::CreateChannel(uint8_t flags)
{
    auto channel = CreateObject<MultiModelSpectrumChannel>();
    Ptr<ChannelConditionModel> channelConditionModel;
    if (m_channelConditionModel.IsTypeIdSet())
    {
        channelConditionModel = m_channelConditionModel.Create<ChannelConditionModel>();
        NS_LOG_DEBUG(
            "Channel condition model: " << channelConditionModel->GetInstanceTypeId().GetName());
    }
    if (flags & INIT_FADING && m_spectrumModel.IsTypeIdSet())
    {
        auto spectrumLossModel = m_spectrumModel.Create();
        // Get the matrix-based channel model if it has one
        PointerValue matrixChannelClassPtr;
        bool isMatrixBased =
            spectrumLossModel->GetAttributeFailSafe("ChannelModel", matrixChannelClassPtr);
        Ptr<Object> channelObject;
        if (isMatrixBased)
        {
            channelObject = matrixChannelClassPtr.Get<MatrixBasedChannelModel>();
            channelObject->AggregateObject(spectrumLossModel);

            // Break the circular dependency between channel and spectrumLoss objects
            // before disposing the objects to avoid a memory leak until ns-3.44
            auto threeGppSpecProp =
                DynamicCast<ThreeGppSpectrumPropagationLossModel>(spectrumLossModel);
            if (threeGppSpecProp)
            {
                Simulator::ScheduleDestroy(&ThreeGppSpectrumPropagationLossModel::SetChannelModel,
                                           threeGppSpecProp,
                                           CreateObject<ThreeGppChannelModel>());
            }
        }
        else
        {
            channelObject = spectrumLossModel;
        }
        // Set the attributes of the channel model assuming both possible channel models
        channelObject->SetAttributeFailSafe("Scenario", StringValue(GetScenario()));
        channelObject->SetAttributeFailSafe("ChannelConditionModel",
                                            PointerValue(channelConditionModel));
        NS_LOG_DEBUG("Spectrum loss model: " << spectrumLossModel->GetInstanceTypeId().GetName());
        // Attempt to set both spectrum and phased-array spectrum propagation loss models.
        // If the user selects the phased-array spectrum model, the dynamic cast will fail for the
        // spectrum propagation loss model, and vice versa. If the user selects the spectrum model,
        // the dynamic cast will succeed for the spectrum propagation loss model, and the
        // phased-array spectrum propagation loss model will be set to nullptr.
        channel->AddSpectrumPropagationLossModel(
            DynamicCast<SpectrumPropagationLossModel>(spectrumLossModel));
        channel->AddPhasedArraySpectrumPropagationLossModel(
            DynamicCast<PhasedArraySpectrumPropagationLossModel>(spectrumLossModel));
    }
    if (flags & INIT_PROPAGATION && m_pathLossModel.IsTypeIdSet())
    {
        auto pathLoss = m_pathLossModel.Create<PropagationLossModel>();
        pathLoss->SetAttributeFailSafe("ChannelConditionModel",
                                       PointerValue(channelConditionModel));
        NS_LOG_DEBUG("Path loss model: " << pathLoss->GetInstanceTypeId().GetName());
        channel->AddPropagationLossModel(pathLoss);
    }
    return channel;
}

void
NrChannelHelper::ConfigureFactories(std::string Scenario,
                                    std::string Condition,
                                    std::string ChannelModel)
{
    NS_LOG_INFO("Setting the channel model: " << ChannelModel << " with the scenario " << Scenario
                                              << " and the condition " << Condition);
    SetAttribute("ChannelModel", StringValue(ChannelModel));
    SetAttribute("Scenario", StringValue(Scenario));
    SetAttribute("ChannelCondition", StringValue(Condition));
    auto bandInfo = GetBandTypeIdInfo();
    // Set the type ID of the factories
    m_pathLossModel.SetTypeId(std::get<0>(bandInfo));
    m_spectrumModel.SetTypeId(std::get<1>(bandInfo));
    m_channelConditionModel.SetTypeId(std::get<2>(bandInfo));
}

void
NrChannelHelper::SetPhasedArraySpectrumPropagationLossModelAttribute(const std::string& n,
                                                                     const AttributeValue& v)
{
    NS_ABORT_MSG_IF(!m_spectrumModel.IsTypeIdSet(), "Set the phased-array spectrum model first");
    m_spectrumModel.Set(n, v);
}

void
NrChannelHelper::SetChannelConditionModelAttribute(const std::string& n, const AttributeValue& v)
{
    NS_ABORT_MSG_IF(!m_channelConditionModel.IsTypeIdSet(),
                    "Set the channel condition model first");
    m_channelConditionModel.Set(n, v);
}

void
NrChannelHelper::SetPathlossAttribute(const std::string& n, const AttributeValue& v)
{
    NS_ABORT_MSG_IF(!m_pathLossModel.IsTypeIdSet(), "Set the propagation loss model first");
    m_pathLossModel.Set(n, v);
}

void
NrChannelHelper::ConfigurePropagationFactory(TypeId propTypeId)
{
    m_pathLossModel.SetTypeId(propTypeId);
}

void
NrChannelHelper::ConfigureSpectrumFactory(TypeId spectrumTypeId)
{
    m_spectrumModel.SetTypeId(spectrumTypeId);
}

std::tuple<TypeId, TypeId, TypeId>
NrChannelHelper::GetBandTypeIdInfo() const
{
    auto currentChannel =
        (m_channelModel == ChannelModel::TwoRay) ? ChannelModel::ThreeGpp : m_channelModel;
    if (m_supportedCombinations.find(std::make_tuple(currentChannel, m_scenario)) ==
        m_supportedCombinations.end())
    {
        NS_ABORT_MSG(
            "The combination of propagation, channel model and condition is not supported.");
    }

    auto propagationTypeId = GetPropagationTypeId();
    auto channelConditionTypeId = GetConditionTypeId();
    auto phasedSpectrumTypeId = GetChannelModelTypeId();
    if (!channelConditionTypeId.GetUid())
    {
        channelConditionTypeId = propagationTypeId.second;
    }

    return std::make_tuple(propagationTypeId.first, phasedSpectrumTypeId, channelConditionTypeId);
}

std::string
NrChannelHelper::GetScenario() const
{
    static std::map<Scenario, std::string> lookupTable{
        {Scenario::RMa, "RMa"},
        {Scenario::UMa, "UMa"},
        {Scenario::InH_OfficeOpen, "InH-OfficeOpen"},
        {Scenario::InH_OfficeMixed, "InH-OfficeMixed"},
        {Scenario::V2V_Highway, "V2V-Highway"},
        {Scenario::V2V_Urban, "V2V-Urban"},
        {Scenario::UMi, "UMi-StreetCanyon"},
        {Scenario::InH, "InH"},
        {Scenario::InF, "InF"},
        {Scenario::NTN_DenseUrban, "NTN-DenseUrban"},
        {Scenario::NTN_Urban, "NTN-Urban"},
        {Scenario::NTN_Suburban, "NTN-Suburban"},
        {Scenario::NTN_Rural, "NTN-Rural"},
        {Scenario::Custom, "Custom"},
    };

    return lookupTable[m_scenario];
}

TypeId
NrChannelHelper::GetChannelModelTypeId() const
{
    static std::map<ChannelModel, TypeId> lookupTable{
        {ChannelModel::ThreeGpp, ThreeGppSpectrumPropagationLossModel::GetTypeId()},
        {ChannelModel::TwoRay, TwoRaySpectrumPropagationLossModel::GetTypeId()},
        {ChannelModel::NYU, NYUSpectrumPropagationLossModel::GetTypeId()},
    };
    return lookupTable[m_channelModel];
}

TypeId
NrChannelHelper::GetConditionTypeId() const
{
    static std::map<Condition, TypeId> lookupTable{
        {Condition::NLOS, NeverLosChannelConditionModel::GetTypeId()},
        {Condition::LOS, AlwaysLosChannelConditionModel::GetTypeId()},
        {Condition::Buildings, BuildingsChannelConditionModel::GetTypeId()},
        {Condition::Default, TypeId()},
    };
    return lookupTable[m_condition];
}

std::pair<TypeId, TypeId>
NrChannelHelper::GetPropagationTypeId() const
{
    // FTR uses the same propagation model as 3GPP
    auto currentModel =
        (m_channelModel == ChannelModel::TwoRay) ? ChannelModel::ThreeGpp : m_channelModel;
    static std::map<std::pair<ChannelModel, Scenario>, std::pair<TypeId, TypeId>> lookupTable{
        {{ChannelModel::ThreeGpp, Scenario::RMa},
         {ThreeGppRmaPropagationLossModel::GetTypeId(),
          ThreeGppRmaChannelConditionModel::GetTypeId()}},
        {{ChannelModel::ThreeGpp, Scenario::UMa},
         {ThreeGppUmaPropagationLossModel::GetTypeId(),
          ThreeGppUmaChannelConditionModel::GetTypeId()}},
        {{ChannelModel::ThreeGpp, Scenario::InH_OfficeOpen},
         {ThreeGppIndoorOfficePropagationLossModel::GetTypeId(),
          ThreeGppIndoorOpenOfficeChannelConditionModel::GetTypeId()}},
        {{ChannelModel::ThreeGpp, Scenario::InH_OfficeMixed},
         {ThreeGppIndoorOfficePropagationLossModel::GetTypeId(),
          ThreeGppIndoorMixedOfficeChannelConditionModel::GetTypeId()}},
        {{ChannelModel::ThreeGpp, Scenario::V2V_Highway},
         {ThreeGppV2vHighwayPropagationLossModel::GetTypeId(),
          ThreeGppV2vHighwayChannelConditionModel::GetTypeId()}},
        {{ChannelModel::ThreeGpp, Scenario::V2V_Urban},
         {ThreeGppV2vUrbanPropagationLossModel::GetTypeId(),
          ThreeGppV2vUrbanChannelConditionModel::GetTypeId()}},
        {{ChannelModel::ThreeGpp, Scenario::UMi},
         {ThreeGppUmiStreetCanyonPropagationLossModel::GetTypeId(),
          ThreeGppUmiStreetCanyonChannelConditionModel::GetTypeId()}},
        {{ChannelModel::ThreeGpp, Scenario::NTN_DenseUrban},
         {ThreeGppNTNDenseUrbanPropagationLossModel::GetTypeId(),
          ThreeGppNTNDenseUrbanChannelConditionModel::GetTypeId()}},
        {{ChannelModel::ThreeGpp, Scenario::NTN_Urban},
         {ThreeGppNTNUrbanPropagationLossModel::GetTypeId(),
          ThreeGppNTNUrbanChannelConditionModel::GetTypeId()}},
        {{ChannelModel::ThreeGpp, Scenario::NTN_Suburban},
         {ThreeGppNTNSuburbanPropagationLossModel::GetTypeId(),
          ThreeGppNTNSuburbanChannelConditionModel::GetTypeId()}},
        {{ChannelModel::ThreeGpp, Scenario::NTN_Rural},
         {ThreeGppNTNRuralPropagationLossModel::GetTypeId(),
          ThreeGppNTNRuralChannelConditionModel::GetTypeId()}},
        {{ChannelModel::NYU, Scenario::RMa},
         {NYURmaPropagationLossModel::GetTypeId(), NYURmaChannelConditionModel::GetTypeId()}},
        {{ChannelModel::NYU, Scenario::UMa},
         {NYUUmaPropagationLossModel::GetTypeId(), NYUUmaChannelConditionModel::GetTypeId()}},
        {{ChannelModel::NYU, Scenario::UMi},
         {NYUUmiPropagationLossModel::GetTypeId(), NYUUmiChannelConditionModel::GetTypeId()}},
        {{ChannelModel::NYU, Scenario::InH},
         {NYUInHPropagationLossModel::GetTypeId(), NYUInHChannelConditionModel::GetTypeId()}},
        {{ChannelModel::NYU, Scenario::InF},
         {NYUInFPropagationLossModel::GetTypeId(), NYUInFChannelConditionModel::GetTypeId()}}};
    return lookupTable[std::make_pair(currentModel, m_scenario)];
}

void
NrChannelHelper::AssignChannelsToBands(
    const std::vector<std::reference_wrapper<OperationBandInfo>>& bandInfos,
    uint8_t flags)
{
    for (auto& band : bandInfos)
    {
        for (auto& cc : band.get().m_cc)
        {
            for (auto& bwp : cc->m_bwp)
            {
                auto spectrumChannel = CreateChannel(flags);
                // Set the frequency of the phased array spectrum propagation loss model if it
                // exists, we leave it like this until we have a better way to set the frequency
                auto phasedArrayChannel =
                    spectrumChannel->GetPhasedArraySpectrumPropagationLossModel();
                if (phasedArrayChannel)
                {
                    auto matrixChannel = phasedArrayChannel->GetObject<MatrixBasedChannelModel>();
                    matrixChannel->SetAttributeFailSafe("Frequency",
                                                        DoubleValue(bwp->m_centralFrequency));
                    phasedArrayChannel->SetAttributeFailSafe("Frequency",
                                                             DoubleValue(bwp->m_centralFrequency));
                    // Set the bandwidth of the matrix-based channel model in case of NYUSIM
                    // channel model
                    matrixChannel->SetAttributeFailSafe("RfBandwidth",
                                                        DoubleValue(bwp->m_channelBandwidth));
                }
                // Set the frequency of the spectrum propagation loss model if it exists
                auto nonPhasedArrayChannel = spectrumChannel->GetSpectrumPropagationLossModel();
                if (nonPhasedArrayChannel)
                {
                    nonPhasedArrayChannel->SetAttributeFailSafe(
                        "Frequency",
                        DoubleValue(bwp->m_centralFrequency));
                }
                // Set the frequency of the propagation loss model if it exists
                auto propagationLoss = spectrumChannel->GetPropagationLossModel();
                if (propagationLoss)
                {
                    propagationLoss->SetAttributeFailSafe("Frequency",
                                                          DoubleValue(bwp->m_centralFrequency));
                }
                bwp->SetChannel(spectrumChannel);
            }
        }
    }
}

} // namespace ns3
