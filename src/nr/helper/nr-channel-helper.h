// Copyright (c) 2024 LASSE / Universidade Federal do Pará (UFPA)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
// Author: João Albuquerque <joao.barbosa.albuquerque@itec.ufpa.br>

#ifndef NR_CHANNEL_HELPER_H
#define NR_CHANNEL_HELPER_H

#include "cc-bwp-helper.h"

#include <ns3/object-factory.h>
#include <ns3/object.h>
#include <ns3/spectrum-channel.h>

namespace ns3
{

/**
 * \ingroup helper
 * \brief This class is a helper class to create a channel with a specific scenario, channel model
 * and channel condition model. You can also pass your own custom channel configuration without this
 * helper, using the following steps:
 *
 * \code
 * auto channel = CreateObject<MultiModelSpectrumChannel>();
 *
 * channel->AddPropagationLossModel(YourPropagationLossModel);
 *
 * channel->AddSpectrumPropagationLossModel(YourSpectrumModel);
 * \endcode
 *
 * The available features are:
 *
 * - Scenarios: RMa, UMa, InH-OfficeOpen, InH-OfficeMixed, V2V-Highway, V2V-Urban, UMi, InH, InF,
 *   NTN-DenseUrban, NTN-Urban, NTN-Suburban, NTN-Rural
 *
 * - Conditions: LOS, NLOS, Buildings, Default
 *
 * - Channel Models: ThreeGpp, TwoRay, NYU
 *
 * \note
 * The Default channel condition is defined by the selected scenario and can be mixed, LOS
 * or NLOS.
 */
class NrChannelHelper : public Object
{
  public:
    /**
     * \brief Flags for channel assignments
     */
    enum InitFlags : uint8_t
    {
        INIT_PROPAGATION = 0x01, //!< Initialize the propagation loss model
        INIT_FADING = 0x02,      //!< Initialize the fading model
    };

    /**
     * \brief Default constructor
     */
    NrChannelHelper() = default;

    /**
     * \brief Get the TypeId of the NrChannelHelper
     * \return The TypeId of the NrChannelHelper
     *
     */
    static TypeId GetTypeId();

    /**
     * \brief This function configures the object factories with the user-selected scenario,
     * channel, and channel condition models. It only allows the supported combinations, which have
     * only phased spectrum and propagation (NYUSIM, FTR, and 3GPP).
     * \param Scenario The scenario to be used in the band
     * \param Condition The channel condition to be used in the band
     * \param ChannelModel The channel model to be used in the band
     */
    void ConfigureFactories(std::string Scenario = "RMa",
                            std::string Condition = "Default",
                            std::string ChannelModel = "ThreeGpp");

    /**
     * \brief This function configures the spectrum object factory with the user selected spectrum
     * loss model. It is used to set the object factory manually, in case you want to use a custom
     * spectrum loss model. \param specTypeId The TypeId of the spectrum model
     */
    void ConfigureSpectrumFactory(TypeId specTypeId);

    /**
     * \brief This function configures the propagation loss object factory. It is used to set the
     * object factory manually, in case you want to use a custom propagation loss model.
     * \param propTypeId The TypeId of the propagation loss model
     */
    void ConfigurePropagationFactory(TypeId pathLossTypeId);

    /**
     * \brief This function creates a spectrum channel with the given flags
     * \param flags The flags to initialize the spectrum channel. By default, it initializes the
     * propagation and fading models, but you can choose to initialize only the propagation model or
     * only the fading model by using the following flags:
     *
     * - INIT_PROPAGATION: Initialize the propagation loss model
     *
     * - INIT_FADING: Initialize the fading model
     *
     * \return The created spectrum channel
     */
    Ptr<SpectrumChannel> CreateChannel(uint8_t flags = INIT_PROPAGATION | INIT_FADING);

    /**
     * \brief Set an attribute for the PhasedArraySpectrumPropagationLossModel, before the spectrum
     * channel has been created
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetPhasedArraySpectrumPropagationLossModelAttribute(const std::string& n,
                                                             const AttributeValue& v);

    /**
     * \brief Set an attribute for the ChannelConditionModel, before the spectrum channel has been
     * created
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetChannelConditionModelAttribute(const std::string& n, const AttributeValue& v);

    /**
     * \brief Set an attribute for the PathLossModel, before the spectrum channel has been created
     * \param n The name of the attribute
     * \param v The value of the attribute
     */
    void SetPathlossAttribute(const std::string& n, const AttributeValue& v);

    /**
     * \brief This helper function assists in creating multiple instances of spectrum channels with
     * the same scenario, channel condition, and channel model configuration. It is useful when you
     * want to create multiple spectrum channels and assign them to multiple BWPs of multiple bands.
     * \param bandInfos The operation bands
     * \param flags The flags to initialize the spectrum channel
     *
     * \note This function assigns spectrum channels to the provided bands by internally calling
     * CreateChannel(). It uses the central frequency of the BWPs to set the frequency
     * of the created spectrum channels.
     */
    void AssignChannelsToBands(
        const std::vector<std::reference_wrapper<OperationBandInfo>>& bandInfos,
        uint8_t flags = INIT_PROPAGATION | INIT_FADING);

  private:
    /**
     * \brief Different types for the propagation loss model
     */
    enum class Scenario
    {
        RMa,             //!< Rural Macro
        UMa,             //!< Urban Macro
        InH_OfficeOpen,  //!< Indoor Hotspot in an open plan office scenario
        InH_OfficeMixed, //!< Indoor Hotspot in a mixed plan office scenario
        V2V_Highway,     //!< Vehicle-to-vehicle in a highway scenario
        V2V_Urban,       //!< Vehicle-to-vehicle in an urban scenario
        UMi,             //!< Urban Micro
        InH,             //!< Indoor Hotspot
        InF,             //!< Indoor Factory
        NTN_DenseUrban,  //!< Non-Terrestrial Network in a dense urban scenario
        NTN_Urban,       //!< Non-Terrestrial Network in an urban scenario
        NTN_Suburban,    //!< Non-Terrestrial Network in a suburban scenario
        NTN_Rural,       //!< Non-Terrestrial Network in a rural scenario
        Custom           //!< User-defined custom scenario
    } m_scenario{Scenario::RMa};

    /**
     * \brief Different types for the channel model
     */
    enum class ChannelModel
    {
        ThreeGpp, //!< 3GPP
        TwoRay,   //!< TwoRay
        NYU,      //!< NYU
    } m_channelModel{ChannelModel::ThreeGpp};

    /**
     * \brief Different types for the channel condition
     */

    enum class Condition
    {
        LOS,       //!< Always Line-of-Sight
        NLOS,      //!< Never Line-of-Sight
        Buildings, //!< Buildings
        Default,   //!< Default is defined by the selected scenario and can be mixed, LOS or NLOS

    } m_condition{Condition::Default};

    /**
     * \brief Retrieve a string version of the scenario
     * \return the string version of the scenario
     */
    std::string GetScenario() const;

    /**
     * \brief Retrieve a tuple of TypeIds that contains, in the respective order, the propagation,
     * the spectrum and the channel condition models.
     * \return A tuple of propagation, spectrum, and channel condition TypeIds
     */
    std::tuple<TypeId, TypeId, TypeId> GetBandTypeIdInfo() const;

    /**
     * Save all the supported combinations that the user can use
     */
    std::set<std::tuple<ChannelModel, Scenario>> m_supportedCombinations = {
        // NYU-RMA
        {ChannelModel::NYU, Scenario::RMa},
        // NYU-UMA
        {ChannelModel::NYU, Scenario::UMa},
        // NYU-UMi
        {ChannelModel::NYU, Scenario::UMi},
        // NYU-InH
        {ChannelModel::NYU, Scenario::InH},
        // NYU-InF
        {ChannelModel::NYU, Scenario::InF},
        // 3GPP-RMa
        {ChannelModel::ThreeGpp, Scenario::RMa},
        // 3GPP-UMi
        {ChannelModel::ThreeGpp, Scenario::UMi},
        // 3GPP-UMa
        {ChannelModel::ThreeGpp, Scenario::UMa},
        // 3GPP-UMi
        {ChannelModel::ThreeGpp, Scenario::UMi},
        // 3GPP-UMa
        {ChannelModel::ThreeGpp, Scenario::UMa},
        // 3GPP-InH - OfficeMixed
        {ChannelModel::ThreeGpp, Scenario::InH_OfficeMixed},
        // 3GPP-InH - OfficeOpen
        {ChannelModel::ThreeGpp, Scenario::InH_OfficeOpen},
        // 3GPP-V2V - Highway
        {ChannelModel::ThreeGpp, Scenario::V2V_Highway},
        // 3GPP-V2V - Urban
        {ChannelModel::ThreeGpp, Scenario::V2V_Urban},
        // 3GPP-NTN - DenseUrban
        {ChannelModel::ThreeGpp, Scenario::NTN_DenseUrban},
        // 3GPP-NTN - Urban
        {ChannelModel::ThreeGpp, Scenario::NTN_Urban},
        // 3GPP-NTN - Suburban
        {ChannelModel::ThreeGpp, Scenario::NTN_Suburban},
        // 3GPP-NTN - Rural
        {ChannelModel::ThreeGpp, Scenario::NTN_Rural}};

    /**
     * \brief Use the channel model and channel condition to get the TypeId of the ChannelCondition
     * \return The TypeId of the ChannelCondition
     */
    TypeId GetConditionTypeId() const;

    /**
     * \brief Get the TypeId of the channel model
     * \return The TypeId of the channel model
     */
    TypeId GetChannelModelTypeId() const;

    /**
     * \brief Use the scenario and channel model to get the TypeId of the
     * propagation and channel condition model
     * \return A pair of TypeIds {PropagationTypeId, ChannelConditionTypeId}
     *
     * \note The channel condition TypeId will be used only if the user-selected
     * channel condition is Default
     */
    std::pair<TypeId, TypeId> GetPropagationTypeId() const;

    ObjectFactory m_pathLossModel;         //!< The path loss object factory
    ObjectFactory m_spectrumModel;         //!< The phased spectrum object factory
    ObjectFactory m_channelConditionModel; //!< The channel condition object factory
};
} // namespace ns3
#endif /* NR_CHANNEL_HELPER_H */
