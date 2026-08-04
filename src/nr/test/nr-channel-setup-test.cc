// Copyright (c) 2024 LASSE / Universidade Federal do Pará (UFPA)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
// Author: João Albuquerque <joao.barbosa.albuquerque@itec.ufpa.br>

#include <ns3/channel-condition-model.h>
#include <ns3/log.h>
#include <ns3/multi-model-spectrum-channel.h>
#include <ns3/nr-channel-helper.h>
#include <ns3/nyu-channel-condition-model.h>
#include <ns3/nyu-propagation-loss-model.h>
#include <ns3/nyu-spectrum-propagation-loss-model.h>
#include <ns3/pointer.h>
#include <ns3/test.h>
#include <ns3/three-gpp-propagation-loss-model.h>
#include <ns3/three-gpp-spectrum-propagation-loss-model.h>
#include <ns3/three-gpp-v2v-channel-condition-model.h>
#include <ns3/three-gpp-v2v-propagation-loss-model.h>
#include <ns3/two-ray-spectrum-propagation-loss-model.h>

using namespace ns3;

/**
 * \ingroup test
 * \file nr-channel-setup-test.cc
 *
 * \brief This test aims to check if the NrChannelHelper API can correctly create a specified
 * channel, which will be defined by the:
 *
 * - Scenarios: RMa, UMa, InH-OfficeOpen, InH-OfficeMixed, V2V-Highway, V2V-Urban, UMi, InH, InF,
 *   NTN-DenseUrban, NTN-Urban, NTN-Suburban, NTN-Rural
 *
 * - Channel Conditions: LOS, NLOS, Buildings, Default
 *
 * - Channel Models: ThreeGpp, TwoRay, NYU
 *
 *  The test will fail if the created channel does not represent the one expected to be created.
 *
 * \note This test code was produced during the Google Summer of Code 2024 program. The main author
 * is João Albuquerque, under the supervision of Biljana Bojovic, Amir Ashtari, Gabriel Ferreira, in
 * project: 5G NR Module Benchmark and Analysis for Distinct Channel Models
 *
 * <joao.barbosa.albuquerque@itec.ufpa.br>
 */

class NrChannelSetupTest : public TestSuite
{
  public:
    /**
     * Structure to hold the channel configuration
     */
    struct ChannelConfig
    {
        std::string scenario;
        std::string condition;
        std::string channelModel;
    };

    /**
     * Constructor
     */
    NrChannelSetupTest();
    /**
     * Destructor
     */
    ~NrChannelSetupTest() override;
    /**
     * Validate if the created channel is the one expected
     */
    void ValidateCreatedChannel(const Ptr<MultiModelSpectrumChannel> channel,
                                const ChannelConfig& config);

  private:
    /**
     * Run the test
     */
    void DoRun() override;

    // All supported scenarios for each channel model
    std::map<std::string, std::vector<std::string>> supportedScenarios{
        // ThreeGpp supported scenarios
        {"ThreeGpp",
         {"RMa",
          "UMa",
          "UMi",
          "InH-OfficeOpen",
          "InH-OfficeMixed",
          "V2V-Highway",
          "V2V-Urban",
          "UMi",
          "NTN-DenseUrban",
          "NTN-Urban",
          "NTN-Suburban",
          "NTN-Rural"}},
        // TwoRay supported scenarios (V2V-Highway and V2V-Urban are not yet calibrated)
        {"TwoRay",
         {
             "RMa",
             "UMa",
             "UMi",
             "InH-OfficeOpen",
             "InH-OfficeMixed" // V2V-Highway, "V2V-Urban"
         }},
        // NYU supported scenarios
        {"NYU", {"RMa", "UMa", "UMi", "InF", "InH"}}};

    // All channel conditions
    std::vector<std::string> channelConditions = {"LOS", "NLOS", "Buildings", "Default"};
    // All channel models
    std::vector<std::string> channelModels = {"ThreeGpp", "TwoRay", "NYU"};

    // FTR uses almost the same scenarios of 3GPP so we can use the same TypeIds for both 3GPP and
    // FTR. These are the expected TypeIds for the channel conditions and propagation loss models
    std::map<std::pair<std::string, std::string>, std::pair<TypeId, TypeId>> channelInfoTypeId = {
        {{"ThreeGpp", "RMa"},
         std::make_pair(ThreeGppRmaChannelConditionModel::GetTypeId(),
                        ThreeGppRmaPropagationLossModel::GetTypeId())},
        {{"ThreeGpp", "UMa"},
         std::make_pair(ThreeGppUmaChannelConditionModel::GetTypeId(),
                        ThreeGppUmaPropagationLossModel::GetTypeId())},
        {{"ThreeGpp", "UMi"},
         std::make_pair(ThreeGppUmiStreetCanyonChannelConditionModel::GetTypeId(),
                        ThreeGppUmiStreetCanyonPropagationLossModel::GetTypeId())},
        {{"ThreeGpp", "InH-OfficeOpen"},
         std::make_pair(ThreeGppIndoorOpenOfficeChannelConditionModel::GetTypeId(),
                        ThreeGppIndoorOfficePropagationLossModel::GetTypeId())},
        {{"ThreeGpp", "InH-OfficeMixed"},
         std::make_pair(ThreeGppIndoorMixedOfficeChannelConditionModel::GetTypeId(),
                        ThreeGppIndoorOfficePropagationLossModel::GetTypeId())},
        {{"ThreeGpp", "V2V-Highway"},
         std::make_pair(ThreeGppV2vHighwayChannelConditionModel::GetTypeId(),
                        ThreeGppV2vHighwayPropagationLossModel::GetTypeId())},
        {{"ThreeGpp", "V2V-Urban"},
         std::make_pair(ThreeGppV2vUrbanChannelConditionModel::GetTypeId(),
                        ThreeGppV2vUrbanPropagationLossModel::GetTypeId())},
        {{"ThreeGpp", "NTN-DenseUrban"},
         std::make_pair(ThreeGppNTNDenseUrbanChannelConditionModel::GetTypeId(),
                        ThreeGppNTNDenseUrbanPropagationLossModel::GetTypeId())},
        {{"ThreeGpp", "NTN-Urban"},
         std::make_pair(ThreeGppNTNUrbanChannelConditionModel::GetTypeId(),
                        ThreeGppNTNUrbanPropagationLossModel::GetTypeId())},
        {{"ThreeGpp", "NTN-Suburban"},
         std::make_pair(ThreeGppNTNSuburbanChannelConditionModel::GetTypeId(),
                        ThreeGppNTNSuburbanPropagationLossModel::GetTypeId())},
        {{"ThreeGpp", "NTN-Rural"},
         std::make_pair(ThreeGppNTNRuralChannelConditionModel::GetTypeId(),
                        ThreeGppNTNRuralPropagationLossModel::GetTypeId())},
        {{"NYU", "RMa"},
         std::make_pair(NYURmaChannelConditionModel::GetTypeId(),
                        NYURmaPropagationLossModel::GetTypeId())},
        {{"NYU", "UMa"},
         std::make_pair(NYUUmaChannelConditionModel::GetTypeId(),
                        NYUUmaPropagationLossModel::GetTypeId())},
        {{"NYU", "UMi"},
         std::make_pair(NYUUmiChannelConditionModel::GetTypeId(),
                        NYUUmiPropagationLossModel::GetTypeId())},
        {{"NYU", "InF"},
         std::make_pair(NYUInFChannelConditionModel::GetTypeId(),
                        NYUInFPropagationLossModel::GetTypeId())},
        {{"NYU", "InH"},
         std::make_pair(NYUInHChannelConditionModel::GetTypeId(),
                        NYUInHPropagationLossModel::GetTypeId())}};

    // TypeIds for the channel models
    std::map<std::string, TypeId> channelModelTypeId = {
        {"ThreeGpp", ThreeGppSpectrumPropagationLossModel::GetTypeId()},
        {"TwoRay", TwoRaySpectrumPropagationLossModel::GetTypeId()},
        {"NYU", NYUSpectrumPropagationLossModel::GetTypeId()}};
    // TypeIds for the channel conditions
    std::map<std::string, TypeId> channelConditionTypeId = {
        {"LOS", AlwaysLosChannelConditionModel::GetTypeId()},
        {"NLOS", NeverLosChannelConditionModel::GetTypeId()},
        {"Buildings", BuildingsChannelConditionModel::GetTypeId()}};
};

NrChannelSetupTest::NrChannelSetupTest()
    : TestSuite("nr-channel-setup-test")
{
}

NrChannelSetupTest::~NrChannelSetupTest()
{
}

void
NrChannelSetupTest::DoRun()
{
    auto channelHelper = CreateObject<NrChannelHelper>();
    for (auto& channel : channelModels)
    {
        for (auto& condition : channelConditions)
        {
            for (auto& scenario : supportedScenarios[channel])
            {
                channelHelper->ConfigureFactories(scenario, condition, channel);
                auto specChannel = channelHelper->CreateChannel();
                ValidateCreatedChannel(DynamicCast<MultiModelSpectrumChannel>(specChannel),
                                       {scenario, condition, channel});
            }
        }
    }
}

void
NrChannelSetupTest::ValidateCreatedChannel(const Ptr<MultiModelSpectrumChannel> channel,
                                           const ChannelConfig& config)
{
    auto channelModel = channel->GetPhasedArraySpectrumPropagationLossModel();
    auto propagationLossModel = channel->GetPropagationLossModel();
    // Get the channel conidtion model, using the propagation object
    PointerValue channelConditionModelPtr;
    propagationLossModel->GetAttribute("ChannelConditionModel", channelConditionModelPtr);
    auto channelConditionModel = channelConditionModelPtr.Get<ChannelConditionModel>();

    // Check if the channel model is the one expected
    NS_TEST_ASSERT_MSG_EQ(channelModelTypeId[config.channelModel],
                          channelModel->GetInstanceTypeId(),
                          "Channel model is not the one expected");

    // If LOS, NLOS or Buildings, it maps to the expected channel condition model using the
    // channelConditionTypeId map. Instead, if Default, it uses the channelInfoTypeId map to get the
    // expected channel condition model
    TypeId expectedChannelConditionTypeId = channelConditionTypeId[config.condition];
    // If FTR is used, the channel model is the same as 3GPP
    auto currentChannel = (config.channelModel == "TwoRay") ? "ThreeGpp" : config.channelModel;
    if (config.condition == "Default")
    {
        expectedChannelConditionTypeId = channelInfoTypeId[{currentChannel, config.scenario}].first;
    }

    // Check if the channel condition model is the one expected
    NS_TEST_ASSERT_MSG_EQ(expectedChannelConditionTypeId,
                          channelConditionModel->GetInstanceTypeId(),
                          "Channel condition is not the one expected");

    // Check if the propagation loss model is the one expected
    NS_TEST_ASSERT_MSG_EQ(channelInfoTypeId[std::make_pair(currentChannel, config.scenario)].second,
                          propagationLossModel->GetInstanceTypeId(),
                          "Propagation loss model is not the one expected");
}

static NrChannelSetupTest g_NrChannelSetupTest;
