// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "cttc-nr-3gpp-calibration-utils-v2.h"

#include "flow-monitor-output-stats.h"
#include "power-output-stats.h"
#include "rb-output-stats.h"
#include "slot-output-stats.h"

#include "ns3/log.h"
#include <ns3/antenna-module.h>
#include <ns3/config.h>
#include <ns3/enum.h>
#include <ns3/nr-spectrum-value-helper.h>
#include <ns3/object-vector.h>
#include <ns3/pointer.h>

NS_LOG_COMPONENT_DEFINE("LenaV2Utils");

namespace ns3
{

void
LenaV2Utils::ReportSinrNr(SinrOutputStats* stats,
                          uint16_t cellId,
                          uint16_t rnti,
                          double avgSinr,
                          uint16_t bwpId)
{
    stats->SaveSinr(cellId, rnti, avgSinr, bwpId);
}

void
LenaV2Utils::ReportPowerNr(PowerOutputStats* stats,
                           const SfnSf& sfnSf,
                           Ptr<const SpectrumValue> txPsd,
                           const Time& t,
                           uint16_t rnti,
                           uint64_t imsi,
                           uint16_t bwpId,
                           uint16_t cellId)
{
    stats->SavePower(sfnSf, txPsd, t, rnti, imsi, bwpId, cellId);
}

void
LenaV2Utils::ReportSlotStatsNr(SlotOutputStats* stats,
                               const SfnSf& sfnSf,
                               uint32_t scheduledUe,
                               uint32_t usedReg,
                               uint32_t usedSym,
                               uint32_t availableRb,
                               uint32_t availableSym,
                               uint16_t bwpId,
                               uint16_t cellId)
{
    stats->SaveSlotStats(sfnSf,
                         scheduledUe,
                         usedReg,
                         usedSym,
                         availableRb,
                         availableSym,
                         bwpId,
                         cellId);
}

void
LenaV2Utils::ReportRbStatsNr(RbOutputStats* stats,
                             const SfnSf& sfnSf,
                             uint8_t sym,
                             const std::vector<int>& rbUsed,
                             uint16_t bwpId,
                             uint16_t cellId)
{
    stats->SaveRbStats(sfnSf, sym, rbUsed, bwpId, cellId);
}

void
LenaV2Utils::ReportGnbRxDataNr(PowerOutputStats* gnbRxDataStats,
                               const SfnSf& sfnSf,
                               Ptr<const SpectrumValue> rxPsd,
                               const Time& t,
                               uint16_t bwpId,
                               uint16_t cellId)
{
    gnbRxDataStats->SavePower(sfnSf, rxPsd, t, 0, 0, bwpId, cellId);
}

void
LenaV2Utils::ConfigureBwpTo(BandwidthPartInfoPtr& bwp, double centerFreq, double bwpBw)
{
    bwp->m_centralFrequency = centerFreq;
    bwp->m_higherFrequency = centerFreq + (bwpBw / 2);
    bwp->m_lowerFrequency = centerFreq - (bwpBw / 2);
    bwp->m_channelBandwidth = bwpBw;
}

//  unnamed namespace
namespace
{

void
ConfigurePhy(Ptr<NrHelper>& nrHelper,
             Ptr<NetDevice> gnb,
             double orientationRads,
             uint16_t numerology,
             double txPowerBs,
             const std::string& pattern,
             uint32_t bwpIndex,
             double gnbFirstSubArray,
             double gnbSecondSubArray,
             uint16_t beamConfSector,
             double beamConfElevation)
{
    // Change the antenna orientation
    Ptr<NrGnbPhy> phy0 = nrHelper->GetGnbPhy(gnb, 0); // BWP 0
    Ptr<UniformPlanarArray> antenna0 = ConstCast<UniformPlanarArray>(
        phy0->GetSpectrumPhy()->GetAntenna()->GetObject<UniformPlanarArray>());
    antenna0->SetAttribute("BearingAngle", DoubleValue(orientationRads));

    // configure the beam that points toward the center of hexagonal
    // In case of beamforming, it will be overwritten.
    phy0->GetSpectrumPhy()->GetBeamManager()->SetPredefinedBeam(beamConfSector, beamConfElevation);

    // Set numerology
    nrHelper->GetGnbPhy(gnb, 0)->SetAttribute("Numerology", UintegerValue(numerology)); // BWP

    // Set TX power
    nrHelper->GetGnbPhy(gnb, 0)->SetAttribute("TxPower", DoubleValue(txPowerBs));

    // Set TDD pattern
    nrHelper->GetGnbPhy(gnb, 0)->SetAttribute("Pattern", StringValue(pattern));
}

} // unnamed namespace

void
LenaV2Utils::SetLenaV2SimulatorParameters(const double sector0AngleRad,
                                          const std::string& scenario,
                                          const std::string& confType,
                                          const std::string& radioNetwork,
                                          std::string errorModel,
                                          const std::string& operationMode,
                                          const std::string& direction,
                                          uint16_t numerology,
                                          const std::string& pattern,
                                          const NodeContainer& gnbSector1Container,
                                          const NodeContainer& gnbSector2Container,
                                          const NodeContainer& gnbSector3Container,
                                          const NodeContainer& ueSector1Container,
                                          const NodeContainer& ueSector2Container,
                                          const NodeContainer& ueSector3Container,
                                          const Ptr<NrPointToPointEpcHelper>& baseEpcHelper,
                                          Ptr<NrHelper>& nrHelper,
                                          NetDeviceContainer& gnbSector1NetDev,
                                          NetDeviceContainer& gnbSector2NetDev,
                                          NetDeviceContainer& gnbSector3NetDev,
                                          NetDeviceContainer& ueSector1NetDev,
                                          NetDeviceContainer& ueSector2NetDev,
                                          NetDeviceContainer& ueSector3NetDev,
                                          bool enableFading,
                                          bool enableUlPc,
                                          std::string powerAllocation,
                                          SinrOutputStats* sinrStats,
                                          PowerOutputStats* ueTxPowerStats,
                                          PowerOutputStats* gnbRxPowerStats,
                                          SlotOutputStats* slotStats,
                                          RbOutputStats* rbStats,
                                          const std::string& scheduler,
                                          uint32_t bandwidthMHz,
                                          double startingFreq,
                                          uint32_t freqScenario,
                                          double gnbTxPower,
                                          double ueTxPower,
                                          double downtiltAngle,
                                          const uint32_t gnbNumRows,
                                          const uint32_t gnbNumColumns,
                                          const uint32_t ueNumRows,
                                          const uint32_t ueNumColumns,
                                          bool gnbEnable3gppElement,
                                          bool ueEnable3gppElement,
                                          const double gnbHSpacing,
                                          const double gnbVSpacing,
                                          const double ueHSpacing,
                                          const double ueVSpacing,
                                          const double gnbNoiseFigure,
                                          const double ueNoiseFigure,
                                          bool enableRealBF,
                                          bool enableShadowing,
                                          double o2iThreshold,
                                          double o2iLowLossThreshold,
                                          bool linkO2iConditionToAntennaHeight,
                                          bool crossPolarizedGnb,
                                          bool crossPolarizedUe,
                                          double polSlantAngleGnb1,
                                          double polSlantAngleGnb2,
                                          double polSlantAngleUe1,
                                          double polSlantAngleUe2,
                                          std::string bfMethod,
                                          uint16_t beamConfSector,
                                          double beamConfElevation,
                                          double isd,
                                          bool ueBearingAngle)
{
    /*
     * Create the radio network related parameters
     */
    uint8_t numScPerRb = 1; //!< The reference signal density is different in LTE and in NR
    double rbOverhead = 0.1;
    uint32_t harqProcesses = 20;
    uint32_t n1Delay = 2;
    uint32_t n2Delay = 2;
    uint8_t dlCtrlSymbols = 1;

    if (radioNetwork == "LTE")
    {
        rbOverhead = 0.1;
        harqProcesses = 8;
        n1Delay = 4;
        n2Delay = 4;
        // dlCtrlSymbols = 3;

        if (errorModel.empty())
        {
            errorModel = "ns3::LenaErrorModel";
        }
        else if (errorModel != "ns3::NrLteMiErrorModel" && errorModel != "ns3::LenaErrorModel")
        {
            NS_ABORT_MSG("The selected error model is not recommended for LTE");
        }
    }
    else if (radioNetwork == "NR")
    {
        rbOverhead = 0.04;
        harqProcesses = 20;
        if (errorModel.empty())
        {
            errorModel = "ns3::NrEesmIrT1";
        }
        else if (errorModel == "ns3::NrLteMiErrorModel")
        {
            NS_ABORT_MSG("The selected error model is not recommended for NR");
        }
    }
    else
    {
        NS_ABORT_MSG("Unrecognized radio network technology");
    }

    /*
     * Setup the NR module. We create the various helpers needed for the
     * NR simulation:
     * - IdealBeamformingHelper, which takes care of the beamforming part
     * - NrHelper, which takes care of creating and connecting the various
     * part of the NR stack
     */

    nrHelper = CreateObject<NrHelper>();

    Ptr<BeamformingHelperBase> beamformingHelper;

    // in LTE non-calibration we want to use predefined beams that we set directly
    // through beam manager. Hence, we do not need any ideal algorithm.
    // For other cases, we need it (and the beam will be overwritten)

    if (enableFading && bfMethod != "FixedBeam")
    {
        if (radioNetwork == "NR" && enableRealBF)
        {
            beamformingHelper = CreateObject<RealisticBeamformingHelper>();
        }
        else
        {
            beamformingHelper = CreateObject<IdealBeamformingHelper>();
        }
        nrHelper->SetBeamformingHelper(beamformingHelper);
    }

    Ptr<NrPointToPointEpcHelper> nrEpcHelper = DynamicCast<NrPointToPointEpcHelper>(baseEpcHelper);
    nrHelper->SetEpcHelper(nrEpcHelper);

    double txPowerBs = 0.0;

    NS_ABORT_MSG_UNLESS(scenario == "UMa" || scenario == "RMa" || scenario == "UMi",
                        "Unsupported scenario " << scenario << ". Supported values: UMa, RMa, UMi");
    txPowerBs = gnbTxPower;
    std::cout << "Scenario: " << scenario << "gnbTxPower: " << txPowerBs << std::endl;

    std::cout << "o2iThreshold: " << o2iThreshold << std::endl;
    std::cout << "o2iLowLossThreshold: " << o2iLowLossThreshold << std::endl;

    // Noise figure for the gNB
    nrHelper->SetGnbPhyAttribute("NoiseFigure", DoubleValue(gnbNoiseFigure));
    // Noise figure for the UE
    nrHelper->SetUePhyAttribute("NoiseFigure", DoubleValue(ueNoiseFigure));
    nrHelper->SetUePhyAttribute("EnableUplinkPowerControl", BooleanValue(enableUlPc));

    if (radioNetwork == "LTE" && confType == "calibrationConf" && enableUlPc)
    {
        Config::SetDefault("ns3::NrUePowerControl::ClosedLoop", BooleanValue(false));
        Config::SetDefault("ns3::NrUePowerControl::PoNominalPucch", IntegerValue(-106));
        Config::SetDefault("ns3::NrUePowerControl::PoNominalPusch", IntegerValue(-106));
        Config::SetDefault("ns3::NrUePowerControl::Alpha",
                           DoubleValue(1.0)); // well this is the default value also
    }

    Config::SetDefault("ns3::NrMacSchedulerSrsDefault::StartingPeriodicity", UintegerValue(16));
    nrHelper->SetSchedulerAttribute("SrsSymbols", UintegerValue(1));

    NrSpectrumValueHelper::PowerAllocationType powerAllocationEnum;
    if (powerAllocation == "UniformPowerAllocBw")
    {
        powerAllocationEnum = NrSpectrumValueHelper::UNIFORM_POWER_ALLOCATION_BW;
    }
    else if (powerAllocation == "UniformPowerAllocUsed")
    {
        powerAllocationEnum = NrSpectrumValueHelper::UNIFORM_POWER_ALLOCATION_USED;
    }
    else
    {
        NS_ABORT_MSG("Unsupported power allocation type "
                     << scenario
                     << ". Supported values: "
                        "UniformPowerAllocBw and UniformPowerAllocUsed.");
    }

    nrHelper->SetUePhyAttribute("PowerAllocationType", EnumValue(powerAllocationEnum));
    // to match LENA default settings
    nrHelper->SetGnbPhyAttribute("PowerAllocationType",
                                 EnumValue(NrSpectrumValueHelper::UNIFORM_POWER_ALLOCATION_BW));

    // Error Model: UE and GNB with same spectrum error model.
    nrHelper->SetUlErrorModel(errorModel);
    nrHelper->SetDlErrorModel(errorModel);

    // Both DL and UL AMC will have the same model behind.
    nrHelper->SetGnbDlAmcAttribute("AmcModel", EnumValue(NrAmc::ShannonModel));
    nrHelper->SetGnbUlAmcAttribute("AmcModel", EnumValue(NrAmc::ShannonModel));

    /*
     * Adjust the average number of Reference symbols per RB only for LTE case,
     * which is larger than in NR. We assume a value of 4 (could be 3 too).
     */
    nrHelper->SetGnbDlAmcAttribute("NumRefScPerRb", UintegerValue(numScPerRb));
    nrHelper->SetGnbUlAmcAttribute("NumRefScPerRb", UintegerValue(1)); // FIXME: Might change in LTE

    nrHelper->SetGnbPhyAttribute("RbOverhead", DoubleValue(rbOverhead));
    nrHelper->SetGnbPhyAttribute("N2Delay", UintegerValue(n2Delay));
    nrHelper->SetGnbPhyAttribute("N1Delay", UintegerValue(n1Delay));

    nrHelper->SetUeMacAttribute("NumHarqProcess", UintegerValue(harqProcesses));
    nrHelper->SetGnbMacAttribute("NumHarqProcess", UintegerValue(harqProcesses));

    /*
     * Create the necessary operation bands.
     *
     * In the 0 frequency scenario, each sector operates, in a separate band,
     * while for scenario 1 all the sectors are in the same band. Please note that
     * a single BWP in FDD is half the size of the corresponding TDD BWP, and the
     * parameter bandwidthMHz refers to the size of the FDD BWP.
     *
     * Scenario 0:  sectors NON_OVERLAPPING in frequency
     *
     * FDD scenario 0:
     *
     * |--------Band0--------|--------Band1--------|--------Band2--------|
     * |---------CC0---------|---------CC1---------|---------CC2---------|
     * |---BWP0---|---BWP1---|---BWP2---|---BWP3---|---BWP4---|---BWP5---|
     *
     *   Sector i will go in Bandi
     *   DL in the first BWP, UL in the second BWP
     *
     * TDD scenario 0:
     *
     * |--------Band0--------|--------Band1--------|--------Band2--------|
     * |---------CC0---------|---------CC2---------|---------CC2---------|
     * |---------BWP0--------|---------BWP1--------|---------BWP2--------|
     *
     *   Sector i will go in BWPi
     *
     *
     * Scenario 1:  sectors in OVERLAPPING bands
     *
     * Note that this configuration has 1/3 the total bandwidth of the
     * NON_OVERLAPPING configuration.
     *
     * FDD scenario 1:
     *
     * |--------Band0--------|
     * |---------CC0---------|
     * |---BWP0---|---BWP1---|
     *
     *   Sector i will go in BWPi
     *
     * TDD scenario 1:
     *
     * |--------Band0--------|
     * |---------CC0---------|
     * |---------BWP0--------|
     *
     * This is tightly coupled with what happens in lena-v1-utils.cc
     *
     */
    const double band0Start = startingFreq;
    double bandwidthBwp = bandwidthMHz * 1e6;

    OperationBandInfo band0;
    OperationBandInfo band1;
    OperationBandInfo band2;
    band0.m_bandId = 0;
    band1.m_bandId = 1;
    band2.m_bandId = 2;

    uint8_t numBwp = operationMode == "FDD" ? 2 : 1;

    if (freqScenario == 0) // NON_OVERLAPPING
    {
        double bandwidthCc = numBwp * bandwidthBwp;
        uint8_t numCcPerBand = 1;
        double bandwidthBand = numCcPerBand * bandwidthCc;
        double bandCenter = band0Start + bandwidthBand / 2.0;

        NS_LOG_LOGIC("NON_OVERLAPPING, " << operationMode << ": " << bandwidthBand << ":"
                                         << bandwidthCc << ":" << bandwidthBwp << ", "
                                         << (int)numCcPerBand << ", " << (int)numBwp);

        NS_LOG_LOGIC("bandConf0: " << bandCenter << " " << bandwidthBand);
        CcBwpCreator::SimpleOperationBandConf bandConf0(bandCenter, bandwidthBand, numCcPerBand);
        bandConf0.m_numBwp = numBwp;
        bandCenter += bandwidthBand;

        NS_LOG_LOGIC("bandConf1: " << bandCenter << " " << bandwidthBand);
        CcBwpCreator::SimpleOperationBandConf bandConf1(bandCenter, bandwidthBand, numCcPerBand);
        bandConf1.m_numBwp = numBwp;
        bandCenter += bandwidthBand;

        NS_LOG_LOGIC("bandConf2: " << bandCenter << " " << bandwidthBand);
        CcBwpCreator::SimpleOperationBandConf bandConf2(bandCenter, bandwidthBand, numCcPerBand);
        bandConf2.m_numBwp = numBwp;

        // Create, then configure
        CcBwpCreator ccBwpCreator;
        band0 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf0);
        band0.m_bandId = 0;
        band1 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf1);
        band1.m_bandId = 1;
        band2 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf2);
        band2.m_bandId = 2;
        bandCenter = band0Start + bandwidthBwp / 2.0;

        NS_LOG_LOGIC("band0[0][0]: " << bandCenter << " " << bandwidthBwp);
        ConfigureBwpTo(band0.m_cc[0]->m_bwp[0], bandCenter, bandwidthBwp);
        bandCenter += bandwidthBwp;

        if (operationMode == "FDD")
        {
            NS_LOG_LOGIC("band0[0][1]: " << bandCenter << " " << bandwidthBwp);
            ConfigureBwpTo(band0.m_cc[0]->m_bwp[1], bandCenter, bandwidthBwp);
            bandCenter += bandwidthBwp;
            Config::SetDefault("ns3::NrUeNetDevice::PrimaryUlIndex", UintegerValue(1));
        }

        NS_LOG_LOGIC("band1[0][0]: " << bandCenter << " " << bandwidthBwp);
        ConfigureBwpTo(band1.m_cc[0]->m_bwp[0], bandCenter, bandwidthBwp);
        bandCenter += bandwidthBwp;

        if (operationMode == "FDD")
        {
            NS_LOG_LOGIC("band1[0][1]: " << bandCenter << " " << bandwidthBwp);
            ConfigureBwpTo(band1.m_cc[0]->m_bwp[1], bandCenter, bandwidthBwp);
            bandCenter += bandwidthBwp;
        }

        NS_LOG_LOGIC("band2[0][0]: " << bandCenter << " " << bandwidthBwp);
        ConfigureBwpTo(band2.m_cc[0]->m_bwp[0], bandCenter, bandwidthBwp);
        bandCenter += bandwidthBwp;

        if (operationMode == "FDD")
        {
            NS_LOG_LOGIC("band2[0][1]: " << bandCenter << " " << bandwidthBwp);
            ConfigureBwpTo(band2.m_cc[0]->m_bwp[1], bandCenter, bandwidthBwp);
        }

        std::cout << "BWP Configuration for NON_OVERLAPPING case, mode " << operationMode << "\n"
                  << band0 << band1 << band2;
    }

    else if (freqScenario == 1) // OVERLAPPING
    {
        double bandwidthCc = numBwp * bandwidthBwp;
        uint8_t numCcPerBand = 1;
        double bandwidthBand = numCcPerBand * bandwidthCc;
        double bandCenter = band0Start + bandwidthBand / 2.0;

        NS_LOG_LOGIC("OVERLAPPING, " << operationMode << ": " << bandwidthBand << ":" << bandwidthCc
                                     << ":" << bandwidthBwp << ", " << (int)numCcPerBand << ", "
                                     << (int)numBwp);

        NS_LOG_LOGIC("bandConf0: " << bandCenter << " " << bandwidthBand);
        CcBwpCreator::SimpleOperationBandConf bandConf0(bandCenter, bandwidthBand, numCcPerBand);
        bandConf0.m_numBwp = numBwp;
        bandCenter += bandwidthBand;

        // Create, then configure
        CcBwpCreator ccBwpCreator;
        band0 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf0);
        band0.m_bandId = 0;
        bandCenter = band0Start + bandwidthBwp / 2.0;

        NS_LOG_LOGIC("band0[0][0]: " << bandCenter << " " << bandwidthBwp);
        ConfigureBwpTo(band0.m_cc[0]->m_bwp[0], bandCenter, bandwidthBwp);
        bandCenter += bandwidthBwp;

        if (operationMode == "FDD")
        {
            NS_LOG_LOGIC("band0[0][1]: " << bandCenter << " " << bandwidthBwp);
            ConfigureBwpTo(band0.m_cc[0]->m_bwp[1], bandCenter, bandwidthBwp);
        }

        std::cout << "BWP Configuration for OVERLAPPING case, mode " << operationMode << "\n"
                  << band0;
    }

    else
    {
        std::cerr << "unknown combination of freqScenario = " << freqScenario
                  << " and operationMode = " << operationMode << std::endl;
        exit(1);
    }
    // Create the NrChannelHelper, which takes care of the spectrum channel
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
    // Configure the spectrum channel with the scenario
    channelHelper->ConfigureFactories(scenario, "Default");
    // Set the channel condition attributes
    channelHelper->SetChannelConditionModelAttribute("UpdatePeriod", TimeValue(MilliSeconds(100)));
    channelHelper->SetChannelConditionModelAttribute("LinkO2iConditionToAntennaHeight",
                                                     BooleanValue(linkO2iConditionToAntennaHeight));
    channelHelper->SetChannelConditionModelAttribute("O2iThreshold", DoubleValue(o2iThreshold));
    channelHelper->SetChannelConditionModelAttribute("O2iLowLossThreshold",
                                                     DoubleValue(o2iLowLossThreshold));
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(enableShadowing));
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(100)));
    // Configure Distance-based spectrum manually because it is not possible to set it via
    // NrChannelHelper
    ObjectFactory distanceBasedChannelFactory;
    distanceBasedChannelFactory.SetTypeId(
        DistanceBasedThreeGppSpectrumPropagationLossModel::GetTypeId());
    distanceBasedChannelFactory.Set("MaxDistance", DoubleValue(2 * isd));
    for (size_t i = 0; i < band0.GetBwps().size(); i++)
    {
        auto distanceBased3gpp =
            distanceBasedChannelFactory.Create<DistanceBasedThreeGppSpectrumPropagationLossModel>();
        distanceBased3gpp->SetChannelModelAttribute(
            "Frequency",
            DoubleValue(band0.GetBwpAt(0, i)->m_centralFrequency));
        distanceBased3gpp->SetChannelModelAttribute("Scenario", StringValue(scenario));
        auto specChannelBand0 = channelHelper->CreateChannel(NrChannelHelper::INIT_PROPAGATION);
        // Create the channel considering only the propagation loss. Create the fading in
        // case of non-calibration
        if (enableFading)
        {
            PointerValue channelConditionModel0;
            specChannelBand0->GetPropagationLossModel()->GetAttribute("ChannelConditionModel",
                                                                      channelConditionModel0);
            distanceBased3gpp->SetChannelModelAttribute(
                "ChannelConditionModel",
                PointerValue(channelConditionModel0.Get<ChannelConditionModel>()));
            specChannelBand0->AddPhasedArraySpectrumPropagationLossModel(distanceBased3gpp);
        }
        band0.GetBwpAt(0, i)->SetChannel(specChannelBand0);
    }
    for (size_t i = 0; i < band1.GetBwps().size(); i++)
    {
        auto distanceBased3gpp =
            distanceBasedChannelFactory.Create<DistanceBasedThreeGppSpectrumPropagationLossModel>();
        distanceBased3gpp->SetChannelModelAttribute(
            "Frequency",
            DoubleValue(band1.GetBwpAt(0, i)->m_centralFrequency));
        distanceBased3gpp->SetChannelModelAttribute("Scenario", StringValue(scenario));
        auto specChannelBand1 = channelHelper->CreateChannel(NrChannelHelper::INIT_PROPAGATION);
        // Create the channel considering only the propagation loss. Create the fading in
        // case of non-calibration
        if (enableFading)
        {
            PointerValue channelConditionModel1;
            specChannelBand1->GetPropagationLossModel()->GetAttribute("ChannelConditionModel",
                                                                      channelConditionModel1);
            distanceBased3gpp->SetChannelModelAttribute(
                "ChannelConditionModel",
                PointerValue(channelConditionModel1.Get<ChannelConditionModel>()));
            specChannelBand1->AddPhasedArraySpectrumPropagationLossModel(distanceBased3gpp);
        }
        band1.GetBwpAt(0, i)->SetChannel(specChannelBand1);
    }
    for (size_t i = 0; i < band2.GetBwps().size(); i++)
    {
        auto distanceBased3gpp =
            distanceBasedChannelFactory.Create<DistanceBasedThreeGppSpectrumPropagationLossModel>();
        distanceBased3gpp->SetAttribute("MaxDistance", DoubleValue(2 * isd));
        distanceBased3gpp->SetChannelModelAttribute("Scenario", StringValue(scenario));
        distanceBased3gpp->SetChannelModelAttribute(
            "Frequency",
            DoubleValue(band2.GetBwpAt(0, i)->m_centralFrequency));
        auto specChannelBand2 = channelHelper->CreateChannel(NrChannelHelper::INIT_PROPAGATION);
        // Create the channel considering only the propagation loss. Create the fading in
        // case of non-calibration
        if (enableFading)
        {
            PointerValue channelConditionModel2;
            specChannelBand2->GetPropagationLossModel()->GetAttribute("ChannelConditionModel",
                                                                      channelConditionModel2);
            distanceBased3gpp->SetChannelModelAttribute(
                "ChannelConditionModel",
                PointerValue(channelConditionModel2.Get<ChannelConditionModel>()));
            specChannelBand2->AddPhasedArraySpectrumPropagationLossModel(distanceBased3gpp);
        }
        band2.GetBwpAt(0, i)->SetChannel(specChannelBand2);
    }
    BandwidthPartInfoPtrVector sector1Bwps;
    BandwidthPartInfoPtrVector sector2Bwps;
    BandwidthPartInfoPtrVector sector3Bwps;
    if (freqScenario == 0) // NON_OVERLAPPING
    {
        sector1Bwps = CcBwpCreator::GetAllBwps({band0});
        sector2Bwps = CcBwpCreator::GetAllBwps({band1});
        sector3Bwps = CcBwpCreator::GetAllBwps({band2});
    }
    else // OVERLAPPING
    {
        sector1Bwps = CcBwpCreator::GetAllBwps({band0});
        sector2Bwps = CcBwpCreator::GetAllBwps({band0});
        sector3Bwps = CcBwpCreator::GetAllBwps({band0});
    }

    RealisticBfManager::TriggerEvent realTriggerEvent{RealisticBfManager::SRS_COUNT};

    // TODO: Optimize this code (repeated code)
    //  if there is no fading, that means that there is no beamforming
    if (enableFading && bfMethod != "FixedBeam")
    {
        if (radioNetwork == "NR")
        {
            if (enableRealBF)
            {
                beamformingHelper->SetBeamformingMethod(RealisticBeamformingAlgorithm::GetTypeId());
                nrHelper->SetGnbBeamManagerTypeId(RealisticBfManager::GetTypeId());
                nrHelper->SetGnbBeamManagerAttribute("TriggerEvent", EnumValue(realTriggerEvent));
                nrHelper->SetGnbBeamManagerAttribute("UpdateDelay", TimeValue(MicroSeconds(0)));
            }
            else
            {
                if (bfMethod == "Omni")
                {
                    beamformingHelper->SetBeamformingMethod(
                        QuasiOmniDirectPathBeamforming::GetTypeId());
                }
                else if (bfMethod == "CellScan")
                {
                    beamformingHelper->SetBeamformingMethod(CellScanBeamforming::GetTypeId());
                    beamformingHelper->SetAttribute("BeamformingPeriodicity",
                                                    TimeValue(MilliSeconds(10)));
                }
                else if (bfMethod == "CellScanAzimuth")
                {
                    beamformingHelper->SetBeamformingMethod(
                        CellScanBeamformingAzimuthZenith::GetTypeId());
                }
                else
                {
                    NS_ABORT_MSG("We shouldn't be here. bfMethod is: " << bfMethod);
                }
            }
        }
        else if (radioNetwork == "LTE") // Omni for LTE
        {
            if (bfMethod == "Omni")
            {
                beamformingHelper->SetBeamformingMethod(
                    QuasiOmniDirectPathBeamforming::GetTypeId());
            }
            else if (bfMethod == "CellScan")
            {
                beamformingHelper->SetBeamformingMethod(CellScanBeamforming::GetTypeId());
                beamformingHelper->SetAttribute("BeamformingPeriodicity",
                                                TimeValue(MilliSeconds(10)));
            }
            else
            {
                NS_ABORT_MSG("We shouldn't be here. bfMethod is: " << bfMethod);
            }
        }
    }

    // Scheduler type

    if (radioNetwork == "NR")
    {
        if (scheduler == "PF")
        {
            nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerTdmaPF"));
        }
        else if (scheduler == "RR")
        {
            nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerTdmaRR"));
        }
    }
    else
    {
        if (scheduler == "PF")
        {
            nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerOfdmaPF"));
        }
        else if (scheduler == "RR")
        {
            nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerOfdmaRR"));
        }
    }
    nrHelper->SetSchedulerAttribute("EnableHarqReTx", BooleanValue(false));

    // configure SRS symbols
    nrHelper->SetSchedulerAttribute("SrsSymbols", UintegerValue(1));
    nrHelper->SetSchedulerAttribute("EnableSrsInUlSlots", BooleanValue(false));
    nrHelper->SetSchedulerAttribute("EnableSrsInFSlots", BooleanValue(false));

    // configure CTRL symbols
    nrHelper->SetSchedulerAttribute("DlCtrlSymbols", UintegerValue(dlCtrlSymbols));

    // Core latency
    nrEpcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    // Antennas for all the UEs
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(ueNumRows));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(ueNumColumns));

    if (ueEnable3gppElement)
    {
        nrHelper->SetUeAntennaAttribute("AntennaElement",
                                        PointerValue(CreateObject<ThreeGppAntennaModel>()));
    }
    else
    {
        nrHelper->SetUeAntennaAttribute("AntennaElement",
                                        PointerValue(CreateObject<IsotropicAntennaModel>()));
    }

    nrHelper->SetUeAntennaAttribute("AntennaHorizontalSpacing", DoubleValue(ueHSpacing));
    nrHelper->SetUeAntennaAttribute("AntennaVerticalSpacing", DoubleValue(ueVSpacing));

    // Antennas for all the gNbs
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(gnbNumRows));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(gnbNumColumns));

    nrHelper->SetGnbAntennaAttribute("AntennaHorizontalSpacing", DoubleValue(gnbHSpacing));
    nrHelper->SetGnbAntennaAttribute("AntennaVerticalSpacing", DoubleValue(gnbVSpacing));

    nrHelper->SetGnbAntennaAttribute("DowntiltAngle", DoubleValue(downtiltAngle * M_PI / 180.0));

    if (gnbEnable3gppElement)
    {
        nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                         PointerValue(CreateObject<ThreeGppAntennaModel>()));
    }
    else
    {
        nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                         PointerValue(CreateObject<IsotropicAntennaModel>()));
    }

    double gnbFirstSubArray = (polSlantAngleGnb1 * M_PI) / 180.0;  // converting to radians
    double gnbSecondSubArray = (polSlantAngleGnb2 * M_PI) / 180.0; // converting to radians
    double ueFirstSubArray = (polSlantAngleUe1 * M_PI) / 180.0;    // converting to radians
    double ueSecondSubArray = (polSlantAngleUe2 * M_PI) / 180.0;   // converting to radians

    // UE transmit power
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(ueTxPower));

    // Set LTE RBG size
    // TODO: What these values would be in TDD? bandwidthMhz refers to FDD.
    // for example, for TDD, if we have bandwidthMhz to 20, we will have a 40 MHz
    // BWP.
    if (radioNetwork == "LTE")
    {
        switch (bandwidthMHz)
        {
        case 40:
        case 20:
        case 15:
            nrHelper->SetGnbMacAttribute("NumRbPerRbg", UintegerValue(4));
            break;
        case 10:
            nrHelper->SetGnbMacAttribute("NumRbPerRbg", UintegerValue(3));
            break;
        case 5:
            nrHelper->SetGnbMacAttribute("NumRbPerRbg", UintegerValue(2));
            break;
        default:
            NS_ABORT_MSG(
                "Currently, only supported bandwidths are 5, 10, 15, 20 and 40MHz, you chose "
                << bandwidthMHz);
        }
    }
    else
    {
        nrHelper->SetGnbMacAttribute("NumRbPerRbg", UintegerValue(1));
    }

    // We assume a common traffic pattern for all UEs
    uint32_t bwpIdForLowLat = 0;
    if (operationMode == "FDD" && direction == "UL")
    {
        bwpIdForLowLat = 1;
    }

    // gNb routing between Bearer and bandwidth part
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB",
                                                 UintegerValue(bwpIdForLowLat));

    // Ue routing between Bearer and bandwidth part
    nrHelper->SetUeBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpIdForLowLat));

    //  NetDeviceContainer gnbNetDev = nrHelper->InstallGnbDevice (gridScenario.GetBaseStations (),
    //  allBwps);
    gnbSector1NetDev = nrHelper->InstallGnbDevice(gnbSector1Container, sector1Bwps);
    NetDeviceContainer gnbNetDevs(gnbSector1NetDev);
    gnbSector2NetDev = nrHelper->InstallGnbDevice(gnbSector2Container, sector2Bwps);
    gnbNetDevs.Add(gnbSector2NetDev);
    gnbSector3NetDev = nrHelper->InstallGnbDevice(gnbSector3Container, sector3Bwps);
    gnbNetDevs.Add(gnbSector3NetDev);
    ueSector1NetDev = nrHelper->InstallUeDevice(ueSector1Container, sector1Bwps);
    NetDeviceContainer ueNetDevs(ueSector1NetDev);
    ueSector2NetDev = nrHelper->InstallUeDevice(ueSector2Container, sector2Bwps);
    ueNetDevs.Add(ueSector2NetDev);
    ueSector3NetDev = nrHelper->InstallUeDevice(ueSector3Container, sector3Bwps);
    ueNetDevs.Add(ueSector3NetDev);

    int64_t randomStream = 1;
    randomStream += nrHelper->AssignStreams(gnbSector1NetDev, randomStream);
    randomStream += nrHelper->AssignStreams(gnbSector2NetDev, randomStream);
    randomStream += nrHelper->AssignStreams(gnbSector3NetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueSector1NetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueSector2NetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueSector3NetDev, randomStream);

    // Sectors (cells) of a site are pointing at different directions
    std::vector<double> sectorOrientationRad{
        sector0AngleRad,
        sector0AngleRad + 2.0 * M_PI / 3.0, // + 120 deg
        sector0AngleRad - 2.0 * M_PI / 3.0  // - 120 deg
    };

    for (uint32_t cellId = 0; cellId < gnbNetDevs.GetN(); ++cellId)
    {
        Ptr<NetDevice> gnb = gnbNetDevs.Get(cellId);
        uint32_t numBwps = nrHelper->GetNumberBwp(gnb);
        if (numBwps > 2)
        {
            NS_ABORT_MSG("Incorrect number of BWPs per CC");
        }

        uint32_t sector = cellId % (gnbSector3NetDev.GetN() == 0 ? 1 : 3);
        double orientation = sectorOrientationRad[sector];

        // First BWP (in case of FDD) or only BWP (in case of TDD)
        ConfigurePhy(nrHelper,
                     gnb,
                     orientation,
                     numerology,
                     txPowerBs,
                     pattern,
                     0,
                     gnbFirstSubArray,
                     gnbSecondSubArray,
                     beamConfSector,
                     beamConfElevation);

        if (numBwps == 2) // FDD
        {
            ConfigurePhy(nrHelper,
                         gnb,
                         orientation,
                         numerology,
                         txPowerBs,
                         pattern,
                         1,
                         gnbFirstSubArray,
                         gnbSecondSubArray,
                         beamConfSector,
                         beamConfElevation);
            // Link the two FDD BWP
            nrHelper->GetBwpManagerGnb(gnb)->SetOutputLink(1, 0);
        }
    }

    Ptr<UniformRandomVariable> m_uniformUeBearingAngle;
    m_uniformUeBearingAngle = CreateObject<UniformRandomVariable>();

    // Set the UE routing:
    for (auto nd = ueNetDevs.Begin(); nd != ueNetDevs.End(); ++nd)
    {
        auto uePhyFirst = nrHelper->GetUePhy(*nd, 0);
        auto uePhySecond{uePhyFirst};

        ObjectVectorValue ueSpectrumPhysFirstBwp;
        Ptr<NrSpectrumPhy> nrSpectrumPhy = uePhyFirst->GetSpectrumPhy();
        nrSpectrumPhy->GetAntenna()->GetObject<UniformPlanarArray>()->SetAttribute(
            "PolSlantAngle",
            DoubleValue(ueFirstSubArray));

        if (ueBearingAngle)
        {
            // For each UE throw a uniform random variable btw -180 and 180
            double ueBearingAngleValue = m_uniformUeBearingAngle->GetValue(-180, 180);
            ueBearingAngleValue = (ueBearingAngleValue * M_PI) / 180.0; // convert to radians
            nrSpectrumPhy->GetAntenna()->GetObject<UniformPlanarArray>()->SetAttribute(
                "BearingAngle",
                DoubleValue(ueBearingAngleValue));
        }
        if (ueSpectrumPhysFirstBwp.GetN() == 2)
        {
            nrSpectrumPhy = ueSpectrumPhysFirstBwp.Get(1)->GetObject<NrSpectrumPhy>();
            nrSpectrumPhy->GetAntenna()->GetObject<UniformPlanarArray>()->SetAttribute(
                "PolSlantAngle",
                DoubleValue(ueSecondSubArray));
        }

        if (operationMode == "FDD")
        {
            nrHelper->GetBwpManagerUe(*nd)->SetOutputLink(0, 1);
            uePhySecond = nrHelper->GetUePhy(*nd, 1);
            uePhySecond->SetUplinkPowerControl(uePhyFirst->GetUplinkPowerControl());

            ObjectVectorValue ueSpectrumPhysSecondBwp;
            nrSpectrumPhy = uePhySecond->GetSpectrumPhy();
            nrSpectrumPhy->GetAntenna()->GetObject<UniformPlanarArray>()->SetAttribute(
                "PolSlantAngle",
                DoubleValue(ueFirstSubArray));
        }
        uePhyFirst->TraceConnectWithoutContext("DlDataSinr",
                                               MakeBoundCallback(&ReportSinrNr, sinrStats));
        uePhySecond->TraceConnectWithoutContext("ReportPowerSpectralDensity",
                                                MakeBoundCallback(&ReportPowerNr, ueTxPowerStats));
    }

    for (auto nd = gnbNetDevs.Begin(); nd != gnbNetDevs.End(); ++nd)
    {
        uint32_t bwpId = 0;
        if (operationMode == "FDD" && direction == "UL")
        {
            bwpId = 1;
        }
        auto gnbPhy = nrHelper->GetGnbPhy(*nd, bwpId);
        gnbPhy->TraceConnectWithoutContext("SlotDataStats",
                                           MakeBoundCallback(&ReportSlotStatsNr, slotStats));
        gnbPhy->TraceConnectWithoutContext("RBDataStats",
                                           MakeBoundCallback(&ReportRbStatsNr, rbStats));
        gnbPhy->GetSpectrumPhy()->TraceConnectWithoutContext(
            "RxDataTrace",
            MakeBoundCallback(&ReportGnbRxDataNr, gnbRxPowerStats));
    }
}

} // namespace ns3
