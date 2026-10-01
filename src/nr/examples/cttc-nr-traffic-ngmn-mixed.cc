// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

/**
 * \ingroup examples
 * \file cttc-nr-traffic-ngmn-mixed.cc
 * \brief A hegagonal topology example used to show how to configure different
 * NGMN types of traffics or NGMN mixed scenario
 *
 * The example consists of an hexagonal grid deployment
 * consisting on a central site and a number of outer rings of sites around this
 * central site. Each site is sectorized, meaning that a number of three antenna
 * arrays or panels are deployed per gNB. These three antennas are pointing to
 * 30º, 150º and 270º w.r.t. the horizontal axis.
 * We allocate a band to each sector of a site, and the bands are contiguous in frequency.
 * We provide a number of simulation parameters that can be configured in the
 * command line.
 *
 * Please have a look at the possible parameters to know what you can configure
 * through the command line.
 *
 * \code{.unparsed}
$ ./ns3 run "cttc-nr-traffic-ngmn-mixed --PrintHelp"
    \endcode
 *
 * The example will print on-screen the end-to-end result of each flow,
 * as well as writing them on a file.
 *
 */

/*
 * Include part. Often, you will have to include the headers for an entire module;
 * do that by including the name of the module you need with the suffix "-module.h".
 */

#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-module.h"
#include <ns3/antenna-module.h>
#include <ns3/nr-radio-environment-map-helper.h>
#include <ns3/ping-helper.h>
#include <ns3/rng-seed-manager.h>
#include <ns3/three-gpp-ftp-m1-helper.h>
#include <ns3/three-gpp-http-client.h>
#include <ns3/three-gpp-http-helper.h>
#include <ns3/three-gpp-http-server.h>
#include <ns3/three-gpp-http-variables.h>
#include <ns3/traffic-generator-ngmn-ftp-multi.h>
#include <ns3/traffic-generator-ngmn-gaming.h>
#include <ns3/traffic-generator-ngmn-video.h>
#include <ns3/traffic-generator-ngmn-voip.h>

#include <algorithm>
#include <iostream>

using namespace ns3;

/*
 * With this line, we will be able to see the logs of the file by enabling the
 * component "CttcTrafficExample", in this way:
 *
 * $ export NS_LOG="CttcTrafficExample=level_info|prefix_func|prefix_time"
 */
NS_LOG_COMPONENT_DEFINE("CttcNrTrafficNgmnMixed");

class RadioNetworkParametersHelper
{
  public:
    /**
     * \brief Set the main radio network parameters.
     * \param freqReuse The cell frequency reuse.
     */
    void SetNetworkParams(const std::string scenario,
                          const std::string operationMode,
                          uint16_t numCcs);
    /**
     * \brief Gets the BS transmit power
     * \return Transmit power in dBW
     */
    double GetTxPower() const;

    /**
     * \brief Gets the operation bandwidth
     * \return Bandwidth in Hz
     */
    double GetBandwidth() const;

    /**
     * \brief Gets the central frequency
     * \return Central frequency in Hz
     */
    double GetCentralFrequency() const;

  private:
    double m_txPower{-1.0};          //!< Transmit power in dBm
    double m_bandwidth{0.0};         //!< System bandwidth in Hz
    double m_centralFrequency{-1.0}; //!< Band central frequency in Hz
};

void
RadioNetworkParametersHelper::SetNetworkParams(const std::string scenario,
                                               const std::string operationMode,
                                               uint16_t numCcs)
{
    NS_ABORT_MSG_IF(scenario != "UMa" && scenario != "UMi", "Unsupported scenario");

    m_centralFrequency = 2e9;
    m_bandwidth = 20e6 * numCcs; // 100 RBs per CC (freqReuse)
    if (operationMode == "FDD")
    {
        m_bandwidth += m_bandwidth;
    }
    if (scenario == "UMa")
    {
        m_txPower = 43;
    }
    else
    {
        m_txPower = 30;
    }
}

double
RadioNetworkParametersHelper::GetTxPower() const
{
    return m_txPower;
}

double
RadioNetworkParametersHelper::GetBandwidth() const
{
    return m_bandwidth;
}

double
RadioNetworkParametersHelper::GetCentralFrequency() const
{
    return m_centralFrequency;
}

void
Set5gLenaSimulatorParameters(HexagonalGridScenarioHelper gridScenario,
                             std::string scenario,
                             std::string radioNetwork,
                             std::string operationMode,
                             std::string direction,
                             NodeContainer gnbSector1Container,
                             NodeContainer gnbSector2Container,
                             NodeContainer gnbSector3Container,
                             NodeContainer ueSector1Container,
                             NodeContainer ueSector2Container,
                             NodeContainer ueSector3Container,
                             Ptr<NrPointToPointEpcHelper>& baseNrEpcHelper,
                             Ptr<NrHelper>& nrHelper,
                             NetDeviceContainer& gnbSector1NetDev,
                             NetDeviceContainer& gnbSector2NetDev,
                             NetDeviceContainer& gnbSector3NetDev,
                             NetDeviceContainer& ueSector1NetDev,
                             NetDeviceContainer& ueSector2NetDev,
                             NetDeviceContainer& ueSector3NetDev,
                             bool uniformLambda)
{
    /*
     * Create the radio network related parameters
     */
    RadioNetworkParametersHelper ranHelper;
    ranHelper.SetNetworkParams(scenario, operationMode, 1);

    /*
     * Setup the NR module. We create the various helpers needed for the
     * NR simulation:
     * - IdealBeamformingHelper, which takes care of the beamforming part
     * - NrHelper, which takes care of creating and connecting the various
     * part of the NR stack
     * - NrChannelHelper, which takes care of the spectrum channel creation and configuration
     */

    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    nrHelper = CreateObject<NrHelper>();

    // Put the pointers inside nrHelper
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);

    Ptr<NrPointToPointEpcHelper> nrEpcHelper =
        DynamicCast<NrPointToPointEpcHelper>(baseNrEpcHelper);
    nrHelper->SetEpcHelper(nrEpcHelper);

    /*
     * Spectrum division. We create one operational band containing three
     * component carriers, and each CC containing a single bandwidth part
     * centered at the frequency specified by the input parameters.
     * Each spectrum part length is, as well, specified by the input parameters.
     * The operational band will use StreetCanyon channel or UrbanMacro modeling.
     */
    BandwidthPartInfoPtrVector allBwps;
    BandwidthPartInfoPtrVector bwps1;
    BandwidthPartInfoPtrVector bwps2;
    BandwidthPartInfoPtrVector bwps3;
    CcBwpCreator ccBwpCreator;
    // Create the configuration for the CcBwpHelper. SimpleOperationBandConf creates
    // a single BWP per CC. Get the spectrum values from the RadioNetworkParametersHelper
    double centralFrequencyBand = ranHelper.GetCentralFrequency();
    double bandwidthBand = ranHelper.GetBandwidth();
    const uint8_t numCcPerBand = 1; // In this example, each cell will have one CC with one BWP
    NS_ABORT_MSG_UNLESS(scenario == "UMa" || scenario == "UMi", "Unsupported scenario");

    std::string errorModel = "";

    if (radioNetwork == "LTE")
    {
        errorModel = "ns3::LenaErrorModel";
    }
    else if (radioNetwork == "NR")
    {
        errorModel = "ns3::NrEesmIrT2";
    }

    // Error Model: UE and GNB with same spectrum error model.
    nrHelper->SetUlErrorModel(errorModel);
    nrHelper->SetDlErrorModel(errorModel);

    // Both DL and UL AMC will have the same model behind.
    nrHelper->SetGnbDlAmcAttribute(
        "AmcModel",
        EnumValue(NrAmc::ErrorModel)); // NrAmc::ShannonModel or NrAmc::ErrorModel
    nrHelper->SetGnbUlAmcAttribute(
        "AmcModel",
        EnumValue(NrAmc::ErrorModel)); // NrAmc::ShannonModel or NrAmc::ErrorModel

    /*
     * Create the necessary operation bands. In this example, each sector operates
     * in a separate band. Each band contains a single component carrier (CC),
     * which is made of one BWP in TDD operation mode or two BWPs in FDD mode.
     * Note that BWPs have the same bandwidth. Therefore, CCs and bands in FDD are
     * twice larger than in TDD.
     *
     * The configured spectrum division for TDD operation is:
     * |---Band1---|---Band2---|---Band3---|
     * |----CC1----|----CC2----|----CC3----|
     * |----BWP1---|----BWP2---|----BWP3---|
     *
     * And the configured spectrum division for FDD operation is:
     * |---------Band1---------|---------Band2---------|---------Band3---------|
     * |----------CC1----------|----------CC2----------|----------CC3----------|
     * |----BWP1---|----BWP2---|----BWP3---|----BWP4---|----BWP5---|----BWP6---|
     */
    double centralFrequencyBand1 = centralFrequencyBand - bandwidthBand;
    double centralFrequencyBand2 = centralFrequencyBand;
    double centralFrequencyBand3 = centralFrequencyBand + bandwidthBand;
    double bandwidthBand1 = bandwidthBand;
    double bandwidthBand2 = bandwidthBand;
    double bandwidthBand3 = bandwidthBand;

    uint8_t numBwpPerCc = 1;
    if (operationMode == "FDD")
    {
        numBwpPerCc = 2; // FDD will have 2 BWPs per CC
        Config::SetDefault("ns3::NrUeNetDevice::PrimaryUlIndex", UintegerValue(1));
    }

    CcBwpCreator::SimpleOperationBandConf bandConf1(centralFrequencyBand1,
                                                    bandwidthBand1,
                                                    numCcPerBand);
    bandConf1.m_numBwp = numBwpPerCc; // FDD will have 2 BWPs per CC
    CcBwpCreator::SimpleOperationBandConf bandConf2(centralFrequencyBand2,
                                                    bandwidthBand2,
                                                    numCcPerBand);
    bandConf2.m_numBwp = numBwpPerCc; // FDD will have 2 BWPs per CC
    CcBwpCreator::SimpleOperationBandConf bandConf3(centralFrequencyBand3,
                                                    bandwidthBand3,
                                                    numCcPerBand);
    bandConf3.m_numBwp = numBwpPerCc; // FDD will have 2 BWPs per CC

    // By using the configuration created, it is time to make the operation bands
    OperationBandInfo band1 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf1);
    OperationBandInfo band2 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf2);
    OperationBandInfo band3 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf3);
    // Use the channel helper to configure the spectrum channel
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
    /**
     * Set the spectrum channel using the selected scenario
     */
    channelHelper->ConfigureFactories(scenario, "Default", "ThreeGpp");
    // Set attributes to the channel
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(0)));
    channelHelper->SetChannelConditionModelAttribute("UpdatePeriod", TimeValue(MilliSeconds(0)));
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));
    // Assign the channel to all created bands
    channelHelper->AssignChannelsToBands({band1, band2, band3});

    allBwps = CcBwpCreator::GetAllBwps({band1, band2, band3});
    bwps1 = CcBwpCreator::GetAllBwps({band1});
    bwps2 = CcBwpCreator::GetAllBwps({band2});
    bwps3 = CcBwpCreator::GetAllBwps({band3});

    double txPower = ranHelper.GetTxPower(); // Convert to mW

    /*
     * allBwps contains all the spectrum configuration needed for the nrHelper.
     *
     * Now, we can setup the attributes. We can have three kind of attributes:
     * (i) parameters that are valid for all the bandwidth parts and applies to
     * all nodes, (ii) parameters that are valid for all the bandwidth parts
     * and applies to some node only, and (iii) parameters that are different for
     * every bandwidth parts. The approach is:
     *
     * - for (i): Configure the attribute through the helper, and then install;
     * - for (ii): Configure the attribute through the helper, and then install
     * for the first set of nodes. Then, change the attribute through the helper,
     * and install again;
     * - for (iii): Install, and then configure the attributes by retrieving
     * the pointer needed, and calling "SetAttribute" on top of such pointer.
     *
     */

    Packet::EnableChecking();
    Packet::EnablePrinting();

    /*
     *  Case (i): Attributes valid for all the nodes
     */
    // Beamforming method
    if (radioNetwork == "LTE")
    {
        idealBeamformingHelper->SetAttribute(
            "BeamformingMethod",
            TypeIdValue(QuasiOmniDirectPathBeamforming::GetTypeId()));
    }
    else
    {
        idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                             TypeIdValue(DirectPathBeamforming::GetTypeId()));
    }

    // Scheduler type
    if (radioNetwork == "LTE")
    {
        nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerOfdmaPF"));
        nrHelper->SetSchedulerAttribute("DlCtrlSymbols", UintegerValue(1));
    }
    // Core latency
    nrEpcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    // Antennas for all the UEs
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(1));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(1));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));

    // Antennas for all the gNbs
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(8));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(8));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<ThreeGppAntennaModel>()));

    // Set numerology
    nrHelper->SetGnbPhyAttribute("Numerology", UintegerValue(1));
    // Set gNB TX power
    nrHelper->SetGnbPhyAttribute("TxPower", DoubleValue(txPower));
    // UE transmit power
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(20.0));

    // Set LTE RBG size
    if (radioNetwork == "LTE")
    {
        nrHelper->SetGnbMacAttribute("NumRbPerRbg", UintegerValue(4));
    }

    // We assume a common traffic pattern for all UEs
    uint32_t bwpIdForLowLat = 0;
    if (operationMode == "FDD" && direction == "UL")
    {
        bwpIdForLowLat = 1;
    }

    // TODO check later when QoS scheduler is in place, that the type of bearer corresponds to the
    // type of traffic gNb routing between Bearer and bandwidth part
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_VIDEO_TCP_DEFAULT",
                                                 UintegerValue(bwpIdForLowLat));

    // Ue routing between Bearer and bandwidth part
    nrHelper->SetUeBwpManagerAlgorithmAttribute("NGBR_VIDEO_TCP_DEFAULT",
                                                UintegerValue(bwpIdForLowLat));

    /*
     * We have configured the attributes we needed. Now, install and get the pointers
     * to the NetDevices, which contains all the NR stack:
     */

    //  NetDeviceContainer gnbNetDev = nrHelper->InstallGnbDevice (gridScenario.GetBaseStations (),
    //  allBwps);
    gnbSector1NetDev = nrHelper->InstallGnbDevice(gnbSector1Container, bwps1);
    gnbSector2NetDev = nrHelper->InstallGnbDevice(gnbSector2Container, bwps2);
    gnbSector3NetDev = nrHelper->InstallGnbDevice(gnbSector3Container, bwps3);
    ueSector1NetDev = nrHelper->InstallUeDevice(ueSector1Container, bwps1);
    ueSector2NetDev = nrHelper->InstallUeDevice(ueSector2Container, bwps2);
    ueSector3NetDev = nrHelper->InstallUeDevice(ueSector3Container, bwps3);

    int64_t randomStream = 1;
    randomStream += nrHelper->AssignStreams(gnbSector1NetDev, randomStream);
    randomStream += nrHelper->AssignStreams(gnbSector2NetDev, randomStream);
    randomStream += nrHelper->AssignStreams(gnbSector3NetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueSector1NetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueSector2NetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueSector3NetDev, randomStream);

    /*
     * Case (iii): Go node for node and change the attributes we have to setup
     * per-node.
     */

    // Sectors (cells) of a site are pointing at different directions
    double orientationRads = gridScenario.GetAntennaOrientationRadians(0);
    for (uint32_t numCell = 0; numCell < gnbSector1NetDev.GetN(); ++numCell)
    {
        Ptr<NetDevice> gnb = gnbSector1NetDev.Get(numCell);
        uint32_t numBwps = nrHelper->GetNumberBwp(gnb);
        if (numBwps == 1) // TDD
        {
            // Change the antenna orientation
            Ptr<NrGnbPhy> phy = nrHelper->GetGnbPhy(gnb, 0);
            Ptr<UniformPlanarArray> antenna =
                DynamicCast<UniformPlanarArray>(phy->GetSpectrumPhy()->GetAntenna());
            antenna->SetAttribute("BearingAngle", DoubleValue(orientationRads));
            // Set TDD pattern
            nrHelper->GetGnbPhy(gnb, 0)->SetAttribute("Pattern",
                                                      StringValue("F|F|F|F|F|F|F|F|F|F|"));
        }

        else if (numBwps == 2) // FDD
        {
            // Change the antenna orientation
            Ptr<NrGnbPhy> phy0 = nrHelper->GetGnbPhy(gnb, 0);
            Ptr<UniformPlanarArray> antenna0 =
                DynamicCast<UniformPlanarArray>(phy0->GetSpectrumPhy()->GetAntenna());
            antenna0->SetAttribute("BearingAngle", DoubleValue(orientationRads));
            Ptr<NrGnbPhy> phy1 = nrHelper->GetGnbPhy(gnb, 1);
            Ptr<UniformPlanarArray> antenna1 =
                DynamicCast<UniformPlanarArray>(phy1->GetSpectrumPhy()->GetAntenna());
            antenna1->SetAttribute("BearingAngle", DoubleValue(orientationRads));
            // Set TDD pattern
            nrHelper->GetGnbPhy(gnb, 0)->SetAttribute(
                "Pattern",
                StringValue("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));
            nrHelper->GetGnbPhy(gnb, 1)->SetAttribute(
                "Pattern",
                StringValue("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));

            // Link the two FDD BWP
            nrHelper->GetBwpManagerGnb(gnb)->SetOutputLink(1, 0);
        }

        else
        {
            NS_ABORT_MSG("Incorrect number of BWPs per CC");
        }
    }

    orientationRads = gridScenario.GetAntennaOrientationRadians(1);
    for (uint32_t numCell = 0; numCell < gnbSector2NetDev.GetN(); ++numCell)
    {
        Ptr<NetDevice> gnb = gnbSector2NetDev.Get(numCell);
        uint32_t numBwps = nrHelper->GetNumberBwp(gnb);
        if (numBwps == 1) // TDD
        {
            // Change the antenna orientation
            Ptr<NrGnbPhy> phy = nrHelper->GetGnbPhy(gnb, 0);
            Ptr<UniformPlanarArray> antenna =
                DynamicCast<UniformPlanarArray>(phy->GetSpectrumPhy()->GetAntenna());
            antenna->SetAttribute("BearingAngle", DoubleValue(orientationRads));
            // Set TDD pattern
            nrHelper->GetGnbPhy(gnb, 0)->SetAttribute("Pattern",
                                                      StringValue("F|F|F|F|F|F|F|F|F|F|"));
        }

        else if (numBwps == 2) // FDD
        {
            // Change the antenna orientation
            Ptr<NrGnbPhy> phy0 = nrHelper->GetGnbPhy(gnb, 0);
            Ptr<UniformPlanarArray> antenna0 =
                DynamicCast<UniformPlanarArray>(phy0->GetSpectrumPhy()->GetAntenna());
            antenna0->SetAttribute("BearingAngle", DoubleValue(orientationRads));
            Ptr<NrGnbPhy> phy1 = nrHelper->GetGnbPhy(gnb, 1);
            Ptr<UniformPlanarArray> antenna1 =
                DynamicCast<UniformPlanarArray>(phy1->GetSpectrumPhy()->GetAntenna());
            antenna1->SetAttribute("BearingAngle", DoubleValue(orientationRads));
            // Set TDD pattern
            nrHelper->GetGnbPhy(gnb, 0)->SetAttribute(
                "Pattern",
                StringValue("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));
            nrHelper->GetGnbPhy(gnb, 1)->SetAttribute(
                "Pattern",
                StringValue("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));
            // Link the two FDD BWP
            nrHelper->GetBwpManagerGnb(gnb)->SetOutputLink(1, 0);
        }

        else
        {
            NS_ABORT_MSG("Incorrect number of BWPs per CC");
        }
    }

    orientationRads = gridScenario.GetAntennaOrientationRadians(2);
    for (uint32_t numCell = 0; numCell < gnbSector3NetDev.GetN(); ++numCell)
    {
        Ptr<NetDevice> gnb = gnbSector3NetDev.Get(numCell);
        uint32_t numBwps = nrHelper->GetNumberBwp(gnb);
        if (numBwps == 1) // TDD
        {
            // Change the antenna orientation
            Ptr<NrGnbPhy> phy = nrHelper->GetGnbPhy(gnb, 0);
            Ptr<UniformPlanarArray> antenna =
                DynamicCast<UniformPlanarArray>(phy->GetSpectrumPhy()->GetAntenna());
            antenna->SetAttribute("BearingAngle", DoubleValue(orientationRads));
            // Set TDD pattern
            nrHelper->GetGnbPhy(gnb, 0)->SetAttribute("Pattern",
                                                      StringValue("F|F|F|F|F|F|F|F|F|F|"));
        }

        else if (numBwps == 2) // FDD
        {
            // Change the antenna orientation
            Ptr<NrGnbPhy> phy0 = nrHelper->GetGnbPhy(gnb, 0);
            Ptr<UniformPlanarArray> antenna0 =
                DynamicCast<UniformPlanarArray>(phy0->GetSpectrumPhy()->GetAntenna());
            antenna0->SetAttribute("BearingAngle", DoubleValue(orientationRads));
            Ptr<NrGnbPhy> phy1 = nrHelper->GetGnbPhy(gnb, 1);
            Ptr<UniformPlanarArray> antenna1 =
                DynamicCast<UniformPlanarArray>(phy1->GetSpectrumPhy()->GetAntenna());
            antenna1->SetAttribute("BearingAngle", DoubleValue(orientationRads));
            // Set TDD pattern
            nrHelper->GetGnbPhy(gnb, 0)->SetAttribute(
                "Pattern",
                StringValue("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));
            nrHelper->GetGnbPhy(gnb, 1)->SetAttribute(
                "Pattern",
                StringValue("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));
            // Link the two FDD BWP
            nrHelper->GetBwpManagerGnb(gnb)->SetOutputLink(1, 0);
        }

        else
        {
            NS_ABORT_MSG("Incorrect number of BWPs per CC");
        }
    }

    // Set the UE routing:

    if (operationMode == "FDD")
    {
        for (uint32_t i = 0; i < ueSector1NetDev.GetN(); i++)
        {
            nrHelper->GetBwpManagerUe(ueSector1NetDev.Get(i))->SetOutputLink(0, 1);
        }

        for (uint32_t i = 0; i < ueSector2NetDev.GetN(); i++)
        {
            nrHelper->GetBwpManagerUe(ueSector2NetDev.Get(i))->SetOutputLink(0, 1);
        }

        for (uint32_t i = 0; i < ueSector3NetDev.GetN(); i++)
        {
            nrHelper->GetBwpManagerUe(ueSector3NetDev.Get(i))->SetOutputLink(0, 1);
        }
    }
}

enum TrafficTypeConf
{
    UDP_CBR,     // 0
    FTP_3GPP_M1, // 1
    NGMN_FTP,    // 2
    NGMN_VIDEO,  // 3
    NGMN_HTTP,   // 4
    NGMN_GAMING, // 5
    NGMN_VOIP,   // 6
    NGMN_MIXED   // 7
};

/**
 * \brief operator << for TrafficTypeConf
 * \param os output stream
 * \param item TrafficType to print
 * \return a copy of the output stream
 */
static inline std::ostream&
operator<<(std::ostream& os, const TrafficTypeConf& item)
{
    switch (item)
    {
    case UDP_CBR:
        os << "UDP CBR";
        break;
    case FTP_3GPP_M1:
        os << "FTP 3GPP M1";
        break;
    case NGMN_FTP:
        os << "NGMN FTP";
        break;
    case NGMN_VIDEO:
        os << "NGMN VIDEO";
        break;
    case NGMN_HTTP:
        os << "NGMN HTTP";
        break;
    case NGMN_GAMING:
        os << "NGMN GAMING";
        break;
    case NGMN_VOIP:
        os << "NGMN VOIP";
        break;
    case NGMN_MIXED:
        os << "NGMN MIXED";
        break;
    default:
        NS_ABORT_MSG("Unknown traffic type");
    }
    return os;
}

static inline std::istream&
operator>>(std::istream& is, TrafficTypeConf& item)
{
    uint32_t inputValue;
    is >> inputValue;
    item = (TrafficTypeConf)inputValue;
    return is;
}

int
main(int argc, char* argv[])
{
    /*
     * Variables that represent the parameters we will accept as input by the
     * command line. Each of them is initialized with a default value.
     */

    TrafficTypeConf trafficTypeConf = FTP_3GPP_M1;
    // Traffic parameters (that we will use inside this script:)
    uint32_t udpPacketSize = 600; // bytes
    // 4000*600*8 = 19.2 Mbps/UE,
    // 3000*600*8 = 14.4 Mbps/UE,
    // 2000*600*8 = 9.6 Mbps/UE
    // 1500*600*8 = 7.2 Mbps/UE
    // 1000*600*8 = 4.8 Mbps/UE
    uint32_t udpLambda = 2000;
    double ftpM1Lambda = 5;
    uint32_t ftpM1FileSize = 512000; // in bytes
    Time clientAppStartTime = MilliSeconds(400);
    Time serverAppStartTime = MilliSeconds(400);
    // Simulation parameters. Please don't use double to indicate seconds, use
    // milliseconds and integers to avoid representation errors.
    uint32_t simTimeMs = 1400;
    Time appStartTime = MilliSeconds(400);
    std::string direction = "DL";
    bool uniformLambda = true;

    // topology
    uint16_t numOuterRings = 0;
    uint16_t uesPerGnb = 10;
    std::string scenario = "UMi";
    std::string radioNetwork = "NR";   // LTE or NR
    std::string operationMode = "TDD"; // TDD or FDD

    // Where we will store the output files.
    std::string simTag = "default";
    std::string outputDir = "./";
    bool logging = false;
    bool traces = true;
    bool useUdp = true;

    uint8_t ngmnMixedFtpPercentage = 10;
    uint8_t ngmnMixedHttpPercentage = 20;
    uint8_t ngmnMixedVideoPercentage = 20;
    uint8_t ngmnMixedVoipPercentage = 30;
    uint8_t ngmnMixedGamingPercentage = 20;

    /*
     * From here, we instruct the ns3::CommandLine class of all the input parameters
     * that we may accept as input, as well as their description, and the storage
     * variable.
     */
    CommandLine cmd(__FILE__);

    cmd.AddValue("trafficTypeConf",
                 "The traffic type to be configured. Currently the following options are "
                 "available: 0 - UDP CBR, 1 - FTP Model 1, 2 - NGMN FTP, 3 - NGMN VIDEO, 4 - HTTP, "
                 "5-NGMN GAMING, 6 - NGMN VOIP, 7 - NGMN MIXED (e.g., "
                 "10% FTP, 20% HTTP, 20% VIDEO STREAMING, 30% VoIP, 20% GAMING)",
                 trafficTypeConf);
    cmd.AddValue(
        "ngmnMixedFtpPercentage",
        "If trafficTypeConf selected is NGMN MIXED this value can be configured to determine the "
        "percentage of the FTP traffic. Percentage should be multiply of 10.",
        ngmnMixedFtpPercentage);
    cmd.AddValue(
        "ngmnMixedHttpPercentage",
        "If trafficTypeConf selected is NGMN MIXED this value can be configured to determine the "
        "percentage of the HTTP traffic. Percentage should be multiply of 10.",
        ngmnMixedHttpPercentage);
    cmd.AddValue(
        "ngmnMixedVideoPercentage",
        "If trafficTypeConf selected is NGMN MIXED this value can be configured to determine the "
        "percentage of the VIDEO traffic. Percentage should be multiply of 10.",
        ngmnMixedVideoPercentage);
    cmd.AddValue(
        "ngmnMixedVoipPercentage",
        "If trafficTypeConf selected is NGMN MIXED this value can be configured to determine the "
        "percentage of the VoIP traffic. Percentage should be multiply of 10.",
        ngmnMixedVoipPercentage);
    cmd.AddValue(
        "ngmnMixedGamingPercentage",
        "If trafficTypeConf selected is NGMN MIXED this value can be configured to determine the "
        "percentage of the GAMING traffic. Percentage should be multiply of 10.",
        ngmnMixedGamingPercentage);
    cmd.AddValue("useUdp",
                 "if true, the NGMN applications will run over UDP connection, otherwise a TCP "
                 "connection will be used. "
                 "Notice that HTTP application as it is present in ns-3 simulator is implemented "
                 "as typical HTTP application, i.e., "
                 "based on the TCP protocol and as such cannot be reconfigured to use UDP.",
                 useUdp);
    cmd.AddValue("ftpM1Lambda",
                 "The lambda to be used for FTP M1 traffic model (Typical values are 2.5, 5). ",
                 ftpM1Lambda);
    cmd.AddValue("udpLambda", "Number of UDP packets generated in one second per UE", udpLambda);
    cmd.AddValue("uniformLambda",
                 "1: Use same lambda (packets/s) for all UEs and cells (equal to 'lambda' input), "
                 "0: use different packet arrival rates (lambdas) among cells",
                 uniformLambda);
    cmd.AddValue("scenario", "The urban scenario string (UMa or UMi)", scenario);
    cmd.AddValue("numRings", "The number of rings around the central site", numOuterRings);
    cmd.AddValue("uesPerGnb",
                 "The number of UE per gNB, should be multiply of 10 so that the mixed traffic "
                 "works correctly.",
                 uesPerGnb);
    cmd.AddValue("logging", "Enable logging", logging);
    cmd.AddValue("traces", "Enable output traces", traces);
    cmd.AddValue("packetSize", "packet size in bytes to be used by UE traffic", udpPacketSize);
    cmd.AddValue("simTimeMs", "Simulation time", simTimeMs);
    cmd.AddValue("direction", "The flow direction (DL or UL)", direction);
    cmd.AddValue("technology", "The radio access network technology", radioNetwork);
    cmd.AddValue("operationMode", "The network operation mode can be TDD or FDD", operationMode);
    cmd.AddValue("simTag",
                 "tag to be appended to output filenames to distinguish simulation campaigns",
                 simTag);
    cmd.AddValue("outputDir", "directory where to store simulation results", outputDir);

    // Parse the command line
    cmd.Parse(argc, argv);

    /*
     * Check if the parameter values provided by the user are correct.
     */
    //  NS_ABORT_IF (centralFrequencyBand > 100e9);]
    NS_ABORT_MSG_IF(
        trafficTypeConf > 7,
        "Currently only supported values for traffic type are 0, 1, 2. Meaning: 0 -UDP CBR, 1 - "
        "FTP Model 1, 2 - NGMN FTP, 3 - NGMN VIDEO, 4- HTTP, 5- NGMN GAMING, 6 - VOIP, 7 - NGMN "
        "MIXED (e.g., 10% FTP, 20% HTTP, 20% VIDEO STREAMING, 30% VoIP, 20% GAMING");
    NS_ABORT_MSG_IF(direction != "DL" && direction != "UL", "Flow direction can only be DL or UL");
    NS_ABORT_MSG_IF(operationMode != "TDD" && operationMode != "FDD",
                    "Operation mode can only be TDD or FDD");
    NS_ABORT_MSG_IF(radioNetwork != "LTE" && radioNetwork != "NR",
                    "Unrecognized radio network technology");

    NS_ABORT_MSG_IF(
        trafficTypeConf == 7 &&
            (ngmnMixedFtpPercentage + ngmnMixedHttpPercentage + ngmnMixedVideoPercentage +
             ngmnMixedVoipPercentage + ngmnMixedGamingPercentage) != 100,
        "If trafficTypeConf selected is the NGMN mixed, then the sum of the percentages of FTP, "
        "VOIP, HTTP, VIDEO STREAMING and GAMING traffic should give 100.");
    /*
     * If the logging variable is set to true, enable the log of some components
     * through the code. The same effect can be obtained through the use
     * of the NS_LOG environment variable:
     *
     * export NS_LOG="UdpClient=level_info|prefix_time|prefix_func|prefix_node:UdpServer=..."
     *
     * Usually, the environment variable way is preferred, as it is more customizable,
     * and more expressive.
     */
    if (logging)
    {
        LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
        LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
        //      LogComponentEnable ("NrMacSchedulerOfdma", LOG_LEVEL_ALL);
    }

    // configure the transport protocol to be used
    std::string transportProtocol;
    if (useUdp)
    {
        transportProtocol = "ns3::UdpSocketFactory";
    }
    else
    {
        transportProtocol = "ns3::TcpSocketFactory";
    }

    Time simTime = MilliSeconds(simTimeMs);

    std::cout << "\n  Traffic configuration selected is: " << trafficTypeConf << std::endl;

    /*
     * Create the scenario. In our examples, we heavily use helpers that setup
     * the gnbs and ue following a pre-defined pattern. Please have a look at the
     * GridScenarioHelper documentation to see how the nodes will be distributed.
     */
    HexagonalGridScenarioHelper gridScenario;
    gridScenario.SetSectorization(HexagonalGridScenarioHelper::TRIPLE);
    gridScenario.SetNumRings(numOuterRings);
    gridScenario.SetScenarioParameters(scenario);
    uint16_t gNbNum = gridScenario.GetNumCells();
    std::cout << "  GNB num: " << gNbNum << std::endl;
    uint32_t ueNum = uesPerGnb * gNbNum;
    std::cout << "  UE num: " << ueNum << std::endl;
    gridScenario.SetUtNumber(ueNum);
    gridScenario.AssignStreams(RngSeedManager::GetRun());
    gridScenario.CreateScenario(); //!< Creates and plots the network deployment
    const uint16_t ffr =
        3; // Fractional Frequency Reuse scheme to mitigate intra-site inter-sector interferences

    /*
     * Create different gNB NodeContainer for the different sectors.
     */
    NodeContainer gnbSector1Container;
    NodeContainer gnbSector2Container;
    NodeContainer gnbSector3Container;
    for (uint32_t j = 0; j < gridScenario.GetBaseStations().GetN(); ++j)
    {
        Ptr<Node> gnb = gridScenario.GetBaseStations().Get(j);
        switch (j % ffr)
        {
        case 0:
            gnbSector1Container.Add(gnb);
            break;
        case 1:
            gnbSector2Container.Add(gnb);
            break;
        case 2:
            gnbSector3Container.Add(gnb);
            break;
        default:
            NS_ABORT_MSG("ffr param cannot be larger than 3");
            break;
        }
    }

    /*
     * Create different UE NodeContainer for the different sectors.
     */
    NodeContainer ueSector1Container;
    NodeContainer ueSector2Container;
    NodeContainer ueSector3Container;

    for (uint32_t j = 0; j < gridScenario.GetUserTerminals().GetN(); ++j)
    {
        Ptr<Node> ue = gridScenario.GetUserTerminals().Get(j);
        switch (j % ffr)
        {
        case 0:
            ueSector1Container.Add(ue);
            break;
        case 1:
            ueSector2Container.Add(ue);
            break;
        case 2:
            ueSector3Container.Add(ue);
            break;
        default:
            NS_ABORT_MSG("ffr param cannot be larger than 3");
            break;
        }
    }

    /*
     * Setup the 5G-LENA scenario
     */
    Ptr<NrPointToPointEpcHelper> nrEpcHelper;

    NetDeviceContainer gnbSector1NetDev;
    NetDeviceContainer gnbSector2NetDev;
    NetDeviceContainer gnbSector3NetDev;
    NetDeviceContainer ueSector1NetDev;
    NetDeviceContainer ueSector2NetDev;
    NetDeviceContainer ueSector3NetDev;

    Ptr<NrHelper> nrHelper = nullptr;

    nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    Set5gLenaSimulatorParameters(gridScenario,
                                 scenario,
                                 radioNetwork,
                                 operationMode,
                                 direction,
                                 gnbSector1Container,
                                 gnbSector2Container,
                                 gnbSector3Container,
                                 ueSector1Container,
                                 ueSector2Container,
                                 ueSector3Container,
                                 nrEpcHelper,
                                 nrHelper,
                                 gnbSector1NetDev,
                                 gnbSector2NetDev,
                                 gnbSector3NetDev,
                                 ueSector1NetDev,
                                 ueSector2NetDev,
                                 ueSector3NetDev,
                                 uniformLambda);

    // From here, it is standard NS3. In the future, we will create helpers
    // for this part as well.

    // create the internet and install the IP stack on the UEs
    // get SGW/PGW and create a single RemoteHost
    Ptr<Node> pgw = nrEpcHelper->GetPgwNode();
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // connect a remoteHost to pgw. Setup routing too
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(2500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.000)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);
    internet.Install(gridScenario.GetUserTerminals());

    // if the mixed traffic type selected then determine for each which container IDs correposnd to
    // each traffic type

    std::set<uint16_t> ngmnFtpIds;
    std::set<uint16_t> ngmnVideoIds;
    std::set<uint16_t> ngmnVoipIds;
    std::set<uint16_t> ngmnHttpIds;
    std::set<uint16_t> ngmnGamingIds;

    // configure indexes of UEs per traffic type

    if (trafficTypeConf == NGMN_MIXED)
    {
        // check if there is enough UEs to configure NGMN_MIXED traffic type
        NS_ABORT_MSG_UNLESS((ueSector1NetDev.GetN() % 10) == 0,
                            "The number of UEs per sector must be mupliply of 10 when NGMN MIXED "
                            "traffic configured");

        std::cout << "\n ueSector1NetDev:" << ueSector1NetDev.GetN() / 10 << std::endl;
        NS_ABORT_MSG_UNLESS((ueSector1NetDev.GetN() / 10) >= 1,
                            "The number of UEs per sector must be at least 10 when NGMN MIXED "
                            "traffic is configured");

        uint16_t ftp = (ueSector1NetDev.GetN() / 10) * ngmnMixedFtpPercentage / 10;
        uint16_t http = (ueSector1NetDev.GetN() / 10) * ngmnMixedHttpPercentage / 10;
        uint16_t video = (ueSector1NetDev.GetN() / 10) * ngmnMixedVideoPercentage / 10;
        uint16_t voip = (ueSector1NetDev.GetN() / 10) * ngmnMixedVoipPercentage / 10;
        uint16_t gaming = (ueSector1NetDev.GetN() / 10) * ngmnMixedGamingPercentage / 10;
        uint16_t index = 0;

        std::cout << "\n Each sector has:" << std::endl;
        std::cout << ftp << " UEs with NGMN FTP traffic" << std::endl;
        std::cout << http << " UEs with NGMN HTTP traffic" << std::endl;
        std::cout << video << " UEs with NGMN VIDEO traffic" << std::endl;
        std::cout << voip << " UEs with NGMN VOIP traffic" << std::endl;
        std::cout << gaming << " UEs with NGMN GAMING traffic" << std::endl;

        for (uint16_t i = 0; i < ftp; i++)
        {
            ngmnFtpIds.insert(index++);
        }

        for (uint16_t i = 0; i < http; i++)
        {
            ngmnHttpIds.insert(index++);
        }

        for (uint16_t i = 0; i < video; i++)
        {
            ngmnVideoIds.insert(index++);
        }

        for (uint16_t i = 0; i < voip; i++)
        {
            ngmnVoipIds.insert(index++);
        }

        for (uint16_t i = 0; i < gaming; i++)
        {
            ngmnGamingIds.insert(index++);
        }
    }

    Ipv4InterfaceContainer ueSector1IpIface =
        nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueSector1NetDev));
    Ipv4InterfaceContainer ueSector2IpIface =
        nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueSector2NetDev));
    Ipv4InterfaceContainer ueSector3IpIface =
        nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueSector3NetDev));

    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);

    // Set the default gateway for the UEs
    for (uint32_t j = 0; j < gridScenario.GetUserTerminals().GetN(); ++j)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(
            gridScenario.GetUserTerminals().Get(j)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(nrEpcHelper->GetUeDefaultGatewayAddress(), 1);
    }

    // attach UEs to their gNB. Try to attach them per cellId order
    for (uint32_t u = 0; u < ueNum; ++u)
    {
        uint32_t sector = u % ffr;
        uint32_t i = u / ffr;
        if (sector == 0)
        {
            Ptr<NetDevice> gnbNetDev = gnbSector1NetDev.Get(i % gridScenario.GetNumSites());
            Ptr<NetDevice> ueNetDev = ueSector1NetDev.Get(i);

            nrHelper->AttachToGnb(ueNetDev, gnbNetDev);

            if (logging)
            {
                Vector gnbpos = gnbNetDev->GetNode()->GetObject<MobilityModel>()->GetPosition();
                Vector uepos = ueNetDev->GetNode()->GetObject<MobilityModel>()->GetPosition();
                double distance = CalculateDistance(gnbpos, uepos);
                std::cout << "Distance = " << distance << " meters" << std::endl;
            }
        }
        else if (sector == 1)
        {
            Ptr<NetDevice> gnbNetDev = gnbSector2NetDev.Get(i % gridScenario.GetNumSites());
            Ptr<NetDevice> ueNetDev = ueSector2NetDev.Get(i);
            nrHelper->AttachToGnb(ueNetDev, gnbNetDev);
            if (logging)
            {
                Vector gnbpos = gnbNetDev->GetNode()->GetObject<MobilityModel>()->GetPosition();
                Vector uepos = ueNetDev->GetNode()->GetObject<MobilityModel>()->GetPosition();
                double distance = CalculateDistance(gnbpos, uepos);
                std::cout << "Distance = " << distance << " meters" << std::endl;
            }
        }
        else if (sector == 2)
        {
            Ptr<NetDevice> gnbNetDev = gnbSector3NetDev.Get(i % gridScenario.GetNumSites());
            Ptr<NetDevice> ueNetDev = ueSector3NetDev.Get(i);
            nrHelper->AttachToGnb(ueNetDev, gnbNetDev);
            if (logging)
            {
                Vector gnbpos = gnbNetDev->GetNode()->GetObject<MobilityModel>()->GetPosition();
                Vector uepos = ueNetDev->GetNode()->GetObject<MobilityModel>()->GetPosition();
                double distance = CalculateDistance(gnbpos, uepos);
                std::cout << "Distance = " << distance << " meters" << std::endl;
            }
        }
        else
        {
            NS_ABORT_MSG("Number of sector cannot be larger than 3");
        }
    }

    /*
     * Traffic part. Install two kind of traffic: low-latency and voice, each
     * identified by a particular source port.
     */
    uint16_t dlPortLowLat = 1234;

    ApplicationContainer serverApps;

    // The sink will always listen to the specified ports
    UdpServerHelper dlPacketSinkLowLat(dlPortLowLat);

    // The server, that is the application which is listening, is installed in the UE
    if (direction == "DL")
    {
        serverApps.Add(dlPacketSinkLowLat.Install(
            {ueSector1Container, ueSector2Container, ueSector3Container}));
    }
    else
    {
        serverApps.Add(dlPacketSinkLowLat.Install(remoteHost));
    }

    /*
     * Configure attributes for the different generators, using user-provided
     * parameters for generating a CBR traffic
     *
     * Low-Latency configuration and object creation:
     */
    UdpClientHelper dlClientLowLat;
    dlClientLowLat.SetAttribute("RemotePort", UintegerValue(dlPortLowLat));
    dlClientLowLat.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
    dlClientLowLat.SetAttribute("PacketSize", UintegerValue(udpPacketSize));
    // dlClientLowLat.SetAttribute ("Interval", TimeValue (Seconds (1.0/lambda)));

    // The bearer that will carry low latency traffic
    NrEpsBearer lowLatBearer(NrEpsBearer::NGBR_VIDEO_TCP_DEFAULT);

    // The filter for the low-latency traffic
    Ptr<NrEpcTft> lowLatTft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpfLowLat;
    if (direction == "DL")
    {
        dlpfLowLat.localPortStart = dlPortLowLat;
        dlpfLowLat.localPortEnd = dlPortLowLat;
        dlpfLowLat.direction = NrEpcTft::DOWNLINK;
    }
    else
    {
        dlpfLowLat.remotePortStart = dlPortLowLat;
        dlpfLowLat.remotePortEnd = dlPortLowLat;
        dlpfLowLat.direction = NrEpcTft::UPLINK;
    }
    lowLatTft->Add(dlpfLowLat);

    std::vector<uint32_t> lambdaPerCell(gridScenario.GetNumCells());

    if (trafficTypeConf == UDP_CBR)
    {
        if (uniformLambda)
        {
            for (uint32_t bs = 0; bs < gridScenario.GetNumCells(); ++bs)
            {
                lambdaPerCell[bs] = udpLambda;
                std::cout << "Cell: " << bs << " lambda (same lambda): " << lambdaPerCell[bs]
                          << std::endl;
            }
        }
        else // non-uniform lambda values among the cells!
        {
            for (uint32_t bs = 0; bs < gridScenario.GetNumCells(); ++bs)
            {
                lambdaPerCell[bs] = 1000 + bs * 2000;
                std::cout << "Cell: " << bs << " lambda (diff lambda): " << lambdaPerCell[bs]
                          << std::endl;
            }
        }
    }

    /*
     * Let's install the applications!
     */
    ApplicationContainer clientApps;
    ApplicationContainer ftpClientAppsSector1;
    ApplicationContainer ftpServerAppsSector1;
    ApplicationContainer ftpClientAppsSector2;
    ApplicationContainer ftpServerAppsSector2;
    ApplicationContainer ftpClientAppsSector3;
    ApplicationContainer ftpServerAppsSector3;
    Ptr<ThreeGppFtpM1Helper> ftpHelperSector1;
    Ptr<ThreeGppFtpM1Helper> ftpHelperSector2;
    Ptr<ThreeGppFtpM1Helper> ftpHelperSector3;
    uint32_t port1 = 2001;
    uint32_t port2 = 2002;
    uint32_t port3 = 2003;
    // Seed the ARP cache by pinging early in the simulation
    // This is a workaround until a static ARP capability is provided
    ApplicationContainer pingApps;

    if (trafficTypeConf == FTP_3GPP_M1)
    {
        // sector 1 FTP M1 applications configuration
        ftpHelperSector1 = CreateObject<ThreeGppFtpM1Helper>(&ftpServerAppsSector1,
                                                             &ftpClientAppsSector1,
                                                             &ueSector1Container,
                                                             &remoteHostContainer,
                                                             &ueSector1IpIface);
        ftpHelperSector1->Configure(port1,
                                    serverAppStartTime,
                                    clientAppStartTime,
                                    simTime,
                                    ftpM1Lambda,
                                    ftpM1FileSize);
        ftpHelperSector1->Start();

        // sector 2 FTP M1 applications configuration
        ftpHelperSector2 = CreateObject<ThreeGppFtpM1Helper>(&ftpServerAppsSector2,
                                                             &ftpClientAppsSector2,
                                                             &ueSector2Container,
                                                             &remoteHostContainer,
                                                             &ueSector2IpIface);
        ftpHelperSector2->Configure(port2,
                                    serverAppStartTime,
                                    clientAppStartTime,
                                    simTime,
                                    ftpM1Lambda,
                                    ftpM1FileSize);
        ftpHelperSector2->Start();

        // sector 3 FTP M1 applications configuration
        ftpHelperSector3 = CreateObject<ThreeGppFtpM1Helper>(&ftpServerAppsSector3,
                                                             &ftpClientAppsSector3,
                                                             &ueSector3Container,
                                                             &remoteHostContainer,
                                                             &ueSector3IpIface);
        ftpHelperSector3->Configure(port3,
                                    serverAppStartTime,
                                    clientAppStartTime,
                                    simTime,
                                    ftpM1Lambda,
                                    ftpM1FileSize);
        ftpHelperSector3->Start();

        clientApps.Add(ftpClientAppsSector1);
        clientApps.Add(ftpClientAppsSector2);
        clientApps.Add(ftpClientAppsSector3);

        serverApps.Add(ftpServerAppsSector1);
        serverApps.Add(ftpServerAppsSector2);
        serverApps.Add(ftpServerAppsSector3);
    }

    if (trafficTypeConf == NGMN_FTP || (trafficTypeConf == NGMN_MIXED && !ngmnFtpIds.empty()))
    {
        uint32_t portFtpNgmn = 2000;

        if (direction == "DL")
        {
            // configure FTP clients with file transfer application that generates multiple file
            // transfers
            TrafficGeneratorHelper ftpHelper(transportProtocol,
                                             Address(),
                                             TrafficGeneratorNgmnFtpMulti::GetTypeId());
            ftpHelper.SetAttribute("PacketSize", UintegerValue(1448));
            ftpHelper.SetAttribute("MaxFileSize", UintegerValue(5e6));

            // configure clients on sector 1
            for (uint32_t i = 0; i < ueSector1IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and (ngmnFtpIds.find(i) == ngmnFtpIds.end()))
                {
                    continue;
                }

                Ipv4Address ipAddress = ueSector1IpIface.GetAddress(i, 0);
                AddressValue ueAddress(InetSocketAddress(ipAddress, portFtpNgmn));
                ftpHelper.SetAttribute("Remote", ueAddress);
                clientApps.Add(ftpHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }
            // configure clients on sector 2
            for (uint32_t i = 0; i < ueSector2IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and (ngmnFtpIds.find(i) == ngmnFtpIds.end()))
                {
                    continue;
                }
                Ipv4Address ipAddress = ueSector2IpIface.GetAddress(i, 0);
                AddressValue ueAddress(InetSocketAddress(ipAddress, portFtpNgmn));
                ftpHelper.SetAttribute("Remote", ueAddress);
                clientApps.Add(ftpHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }
            // configure clients on sector 3
            for (uint32_t i = 0; i < ueSector3IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and (ngmnFtpIds.find(i) == ngmnFtpIds.end()))
                {
                    continue;
                }

                Ipv4Address ipAddress = ueSector3IpIface.GetAddress(i, 0);
                AddressValue ueAddress(InetSocketAddress(ipAddress, portFtpNgmn));
                ftpHelper.SetAttribute("Remote", ueAddress);
                clientApps.Add(ftpHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }

            // configure FTP servers
            InetSocketAddress localAddress(Ipv4Address::GetAny(), portFtpNgmn);
            PacketSinkHelper packetSinkHelper(transportProtocol, localAddress);

            for (uint32_t index = 0; index < ueSector1IpIface.GetN(); index++)
            {
                // in case of NGMN traffic we install packet sink for the subset of the nodes
                if ((trafficTypeConf == NGMN_MIXED) and
                    (ngmnFtpIds.find(index) == ngmnFtpIds.end()))
                {
                    continue;
                }
                serverApps.Add(packetSinkHelper.Install(ueSector1Container.Get(index)));
                serverApps.Add(packetSinkHelper.Install(ueSector2Container.Get(index)));
                serverApps.Add(packetSinkHelper.Install(ueSector3Container.Get(index)));
            }
        }
        else
        {
            NS_ABORT_MSG("Not yet supported option of FTP NGMN traffic with the UL traffic in this "
                         "example. If you need it implement this else block");
        }
    }

    if (trafficTypeConf == NGMN_VIDEO || (trafficTypeConf == NGMN_MIXED && !ngmnVideoIds.empty()))
    {
        uint32_t portNgmnVideo = 4000;

        if (direction == "DL")
        {
            // configure FTP clients with file transfer application that generates multiple file
            // transfers
            TrafficGeneratorHelper trafficGeneratorHelper(transportProtocol,
                                                          Address(),
                                                          TrafficGeneratorNgmnVideo::GetTypeId());
            trafficGeneratorHelper.SetAttribute("NumberOfPacketsInFrame", UintegerValue(8));
            trafficGeneratorHelper.SetAttribute("InterframeIntervalTime",
                                                TimeValue(Seconds(0.100)));

            // configure clients on sector 1
            for (uint32_t i = 0; i < ueSector1IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and
                    (ngmnVideoIds.find(i) == ngmnVideoIds.end()))
                {
                    continue;
                }

                Ipv4Address ipAddress = ueSector1IpIface.GetAddress(i, 0);
                AddressValue remoteAddress(InetSocketAddress(ipAddress, portNgmnVideo));
                trafficGeneratorHelper.SetAttribute("Remote", remoteAddress);
                clientApps.Add(trafficGeneratorHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }
            // configure clients on sector 2
            for (uint32_t i = 0; i < ueSector2IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and
                    (ngmnVideoIds.find(i) == ngmnVideoIds.end()))
                {
                    continue;
                }

                Ipv4Address ipAddress = ueSector2IpIface.GetAddress(i, 0);
                AddressValue remoteAddress(InetSocketAddress(ipAddress, portNgmnVideo));
                trafficGeneratorHelper.SetAttribute("Remote", remoteAddress);
                clientApps.Add(trafficGeneratorHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }
            // configure clients on sector 3
            for (uint32_t i = 0; i < ueSector3IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and
                    (ngmnVideoIds.find(i) == ngmnVideoIds.end()))
                {
                    continue;
                }

                Ipv4Address ipAddress = ueSector3IpIface.GetAddress(i, 0);
                AddressValue remoteAddress(InetSocketAddress(ipAddress, portNgmnVideo));
                trafficGeneratorHelper.SetAttribute("Remote", remoteAddress);
                clientApps.Add(trafficGeneratorHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }

            // configure servers
            InetSocketAddress localAddress(Ipv4Address::GetAny(), portNgmnVideo);
            PacketSinkHelper packetSinkHelper(transportProtocol, localAddress);

            for (uint32_t index = 0; index < ueSector1IpIface.GetN(); index++)
            {
                // in case of NGMN traffic we install packet sink for the subset of the nodes
                if ((trafficTypeConf == NGMN_MIXED) and
                    (ngmnVideoIds.find(index) == ngmnVideoIds.end()))
                {
                    continue;
                }

                Ptr<PacketSink> ps1 = packetSinkHelper.Install(ueSector1Container.Get(index))
                                          .Get(0)
                                          ->GetObject<PacketSink>();
                Ptr<PacketSink> ps2 = packetSinkHelper.Install(ueSector2Container.Get(index))
                                          .Get(0)
                                          ->GetObject<PacketSink>();
                Ptr<PacketSink> ps3 = packetSinkHelper.Install(ueSector3Container.Get(index))
                                          .Get(0)
                                          ->GetObject<PacketSink>();
                serverApps.Add(ps1);
                serverApps.Add(ps2);
                serverApps.Add(ps3);
            }
        }
        else
        {
            NS_ABORT_MSG("Not yet supported option of FTP NGMN traffic with the UL traffic in this "
                         "example. If you need it implement this else block");
        }
    }

    if (trafficTypeConf == NGMN_GAMING || (trafficTypeConf == NGMN_MIXED && !ngmnGamingIds.empty()))
    {
        uint32_t portNgmnGaming = 5000;
        if (direction == "DL")
        {
            // configure FTP clients with file transfer application that generates multiple file
            // transfers
            TrafficGeneratorHelper trafficGeneratorHelper(transportProtocol,
                                                          Address(),
                                                          TrafficGeneratorNgmnGaming::GetTypeId());
            trafficGeneratorHelper.SetAttribute("IsDownlink", BooleanValue(true));
            trafficGeneratorHelper.SetAttribute("aParamPacketSizeDl", UintegerValue(120));
            trafficGeneratorHelper.SetAttribute("bParamPacketSizeDl", DoubleValue(36));
            trafficGeneratorHelper.SetAttribute("aParamPacketArrivalDl", DoubleValue(45));
            trafficGeneratorHelper.SetAttribute("bParamPacketArrivalDl", DoubleValue(5.7));
            trafficGeneratorHelper.SetAttribute("InitialPacketArrivalMin", UintegerValue(0));
            trafficGeneratorHelper.SetAttribute("InitialPacketArrivalMax", UintegerValue(40));

            // configure clients on sector 1
            for (uint32_t i = 0; i < ueSector1IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and
                    (ngmnGamingIds.find(i) == ngmnGamingIds.end()))
                {
                    continue;
                }

                Ipv4Address ipAddress = ueSector1IpIface.GetAddress(i, 0);
                AddressValue remoteAddress(InetSocketAddress(ipAddress, portNgmnGaming));
                trafficGeneratorHelper.SetAttribute("Remote", remoteAddress);
                clientApps.Add(trafficGeneratorHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }
            // configure clients on sector 2
            for (uint32_t i = 0; i < ueSector2IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and
                    (ngmnGamingIds.find(i) == ngmnGamingIds.end()))
                {
                    continue;
                }

                Ipv4Address ipAddress = ueSector2IpIface.GetAddress(i, 0);
                AddressValue remoteAddress(InetSocketAddress(ipAddress, portNgmnGaming));
                trafficGeneratorHelper.SetAttribute("Remote", remoteAddress);
                clientApps.Add(trafficGeneratorHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }
            // configure clients on sector 3
            for (uint32_t i = 0; i < ueSector3IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and
                    (ngmnGamingIds.find(i) == ngmnGamingIds.end()))
                {
                    continue;
                }

                Ipv4Address ipAddress = ueSector3IpIface.GetAddress(i, 0);
                AddressValue remoteAddress(InetSocketAddress(ipAddress, portNgmnGaming));
                trafficGeneratorHelper.SetAttribute("Remote", remoteAddress);
                clientApps.Add(trafficGeneratorHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }

            // configure GAMING servers
            InetSocketAddress localAddress(Ipv4Address::GetAny(), portNgmnGaming);
            PacketSinkHelper packetSinkHelper(transportProtocol, localAddress);

            for (uint32_t index = 0; index < ueSector1IpIface.GetN(); index++)
            {
                // in case of NGMN traffic we install packet sink for the subset of the nodes
                if ((trafficTypeConf == NGMN_MIXED) and
                    (ngmnGamingIds.find(index) == ngmnGamingIds.end()))
                {
                    continue;
                }
                serverApps.Add(packetSinkHelper.Install(ueSector1Container.Get(index)));
                serverApps.Add(packetSinkHelper.Install(ueSector2Container.Get(index)));
                serverApps.Add(packetSinkHelper.Install(ueSector3Container.Get(index)));
            }
        }
        else
        {
            NS_ABORT_MSG("Not yet supported option of FTP NGMN traffic with the UL traffic in this "
                         "example. If you need it implement this else block");

            // TODO extend
            // configure FTP clients with file transfer application that generates multiple file
            // transfers
            // TrafficGeneratorHelper trafficGeneratorHelper ("ns3::UdpSocketFactory", Address (),
            // TrafficGeneratorVideo::GetTypeId ()); trafficGeneratorHelper.SetAttribute
            // ("IsDownlink", BooleanValue (false)); trafficGeneratorHelper.SetAttribute
            // ("aParamPacketSizeUl", UintegerValue (45)); trafficGeneratorHelper.SetAttribute
            // ("bParamPacketSizeUl", DoubleValue (5.7)); trafficGeneratorHelper.SetAttribute
            // ("PacketArrivalUl", UintegerValue (40)); trafficGeneratorHelper.SetAttribute
            // ("InitialPacketArrivalMin", UintegerValue (0)); trafficGeneratorHelper.SetAttribute
            // ("InitialPacketArrivalMax", UintegerValue (40));
        }
    }

    if (trafficTypeConf == NGMN_VOIP || (trafficTypeConf == NGMN_MIXED && !ngmnVoipIds.empty()))
    {
        uint32_t portNgmnVoip = 5000;
        if (direction == "DL")
        {
            // configure FTP clients with file transfer application that generates multiple file
            // transfers
            TrafficGeneratorHelper trafficGeneratorHelper(transportProtocol,
                                                          Address(),
                                                          TrafficGeneratorNgmnVoip::GetTypeId());

            trafficGeneratorHelper.SetAttribute("EncoderFrameLength", UintegerValue(20));
            trafficGeneratorHelper.SetAttribute("MeanTalkSpurtDuration", UintegerValue(2000));
            trafficGeneratorHelper.SetAttribute("VoiceActivityFactor", DoubleValue(0.5));
            trafficGeneratorHelper.SetAttribute("VoicePayload", UintegerValue(40));
            trafficGeneratorHelper.SetAttribute("SIDPeriodicity", UintegerValue(160));
            trafficGeneratorHelper.SetAttribute("SIDPayload", UintegerValue(15));

            // configure clients on sector 1
            for (uint32_t i = 0; i < ueSector1IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and (ngmnVoipIds.find(i) == ngmnVoipIds.end()))
                {
                    continue;
                }

                Ipv4Address ipAddress = ueSector1IpIface.GetAddress(i, 0);
                AddressValue remoteAddress(InetSocketAddress(ipAddress, portNgmnVoip));
                trafficGeneratorHelper.SetAttribute("Remote", remoteAddress);
                clientApps.Add(trafficGeneratorHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }
            // configure clients on sector 2
            for (uint32_t i = 0; i < ueSector2IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and (ngmnVoipIds.find(i) == ngmnVoipIds.end()))
                {
                    continue;
                }

                Ipv4Address ipAddress = ueSector2IpIface.GetAddress(i, 0);
                AddressValue remoteAddress(InetSocketAddress(ipAddress, portNgmnVoip));
                trafficGeneratorHelper.SetAttribute("Remote", remoteAddress);
                clientApps.Add(trafficGeneratorHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }
            // configure clients on sector 3
            for (uint32_t i = 0; i < ueSector3IpIface.GetN(); i++)
            {
                // in case of NGMN traffic allow installation of the specific traffic type only on
                // the specific nodes
                if ((trafficTypeConf == NGMN_MIXED) and (ngmnVoipIds.find(i) == ngmnVoipIds.end()))
                {
                    continue;
                }

                Ipv4Address ipAddress = ueSector3IpIface.GetAddress(i, 0);
                AddressValue remoteAddress(InetSocketAddress(ipAddress, portNgmnVoip));
                trafficGeneratorHelper.SetAttribute("Remote", remoteAddress);
                clientApps.Add(trafficGeneratorHelper.Install(remoteHostContainer));
                // Seed the ARP cache by pinging early in the simulation
                // This is a workaround until a static ARP capability is provided
                PingHelper ping(ipAddress);
                pingApps.Add(ping.Install(remoteHostContainer));
            }

            // configure servers
            InetSocketAddress localAddress(Ipv4Address::GetAny(), portNgmnVoip);
            PacketSinkHelper packetSinkHelper(transportProtocol, localAddress);

            for (uint32_t index = 0; index < ueSector1IpIface.GetN(); index++)
            {
                // in case of NGMN traffic we install packet sink for the subset of the nodes
                if ((trafficTypeConf == NGMN_MIXED) and
                    (ngmnVoipIds.find(index) == ngmnVoipIds.end()))
                {
                    continue;
                }
                serverApps.Add(packetSinkHelper.Install(ueSector1Container.Get(index)));
                serverApps.Add(packetSinkHelper.Install(ueSector2Container.Get(index)));
                serverApps.Add(packetSinkHelper.Install(ueSector3Container.Get(index)));
            }
        }
        else
        {
            NS_ABORT_MSG("Not yet supported option of NGMN VOIP traffic with the UL traffic in "
                         "this example. If you need it implement this else block");

            // TODO extend
            // configure FTP clients with file transfer application that generates multiple file
            // transfers
            // TrafficGeneratorHelper trafficGeneratorHelper ("ns3::UdpSocketFactory", Address (),
            // TrafficGeneratorVideo::GetTypeId ()); trafficGeneratorHelper.SetAttribute
            // ("IsDownlink", BooleanValue (false)); trafficGeneratorHelper.SetAttribute
            // ("aParamPacketSizeUl", UintegerValue (45)); trafficGeneratorHelper.SetAttribute
            // ("bParamPacketSizeUl", DoubleValue (5.7)); trafficGeneratorHelper.SetAttribute
            // ("PacketArrivalUl", UintegerValue (40)); trafficGeneratorHelper.SetAttribute
            // ("InitialPacketArrivalMin", UintegerValue (0)); trafficGeneratorHelper.SetAttribute
            // ("InitialPacketArrivalMax", UintegerValue (40));
        }
    }

    if (trafficTypeConf == NGMN_HTTP || (trafficTypeConf == NGMN_MIXED && !ngmnHttpIds.empty()))
    {
        // uint32_t portNgmnHttp = 7000;
        //  the way how ThreeGppHttpClient and ThreeGppHttpServer are implemented in ns-3
        //  it seems that the client should be installed on UEs and server on remote host

        NodeContainer httpUeContainer;

        for (uint32_t i = 0; i < ueSector1Container.GetN(); i++)
        {
            // in case of NGMN traffic allow installation of the specific traffic type only on the
            // specific nodes
            if ((trafficTypeConf == NGMN_MIXED) and (ngmnHttpIds.find(i) == ngmnHttpIds.end()))
            {
                continue;
            }
            httpUeContainer.Add(ueSector1Container.Get(i));
            httpUeContainer.Add(ueSector2Container.Get(i));
            httpUeContainer.Add(ueSector3Container.Get(i));
        }

        // 1. Create HTTP client applications
        ThreeGppHttpClientHelper clientHelper(remoteHostAddr);
        // Install HTTP clients on UEs
        ApplicationContainer clientApps = clientHelper.Install(httpUeContainer);

        // 2. Create HTTP server applications
        ThreeGppHttpServerHelper serverHelper(remoteHostAddr);
        // Install HTTP server on a remote host node
        ApplicationContainer serverApps = serverHelper.Install(remoteHostContainer.Get(0));
        Ptr<ThreeGppHttpServer> httpServer = serverApps.Get(0)->GetObject<ThreeGppHttpServer>();

        // 3. Setup HTTP variables for the server according to NGMN white paper
        PointerValue ptrVal;
        httpServer->GetAttribute("Variables", ptrVal);
        Ptr<ThreeGppHttpVariables> httpParameters = ptrVal.Get<ThreeGppHttpVariables>();
        httpParameters->SetMainObjectSizeMean(10710);        // according to NGMN white paper
        httpParameters->SetMainObjectSizeStdDev(25032);      // according to NGMN white paper
        httpParameters->SetEmbeddedObjectSizeMean(7758);     // according to NGMN white paper
        httpParameters->SetEmbeddedObjectSizeStdDev(126168); /// according to NGMN white paper
        httpParameters->SetNumOfEmbeddedObjectsMax(55);      // according to NGMN white paper
        httpParameters->SetNumOfEmbeddedObjectsScale(2);     // according to NGMN white paper
        httpParameters->SetNumOfEmbeddedObjectsShape(1.1);   // according to NGMN white paper
        httpParameters->SetReadingTimeMean(Seconds(30));     // according to NGMN white paper
        httpParameters->SetParsingTimeMean(Seconds(0.13));   // according to NGMN white paper

        for (uint32_t i = 0; i < ueSector1IpIface.GetN(); i++)
        {
            // in case of NGMN traffic allow installation of the specific traffic type only on the
            // specific nodes
            if ((trafficTypeConf == NGMN_MIXED) and (ngmnHttpIds.find(i) == ngmnHttpIds.end()))
            {
                continue;
            }

            Ipv4Address ipAddress = ueSector1IpIface.GetAddress(i, 0);
            PingHelper ping(ipAddress);
            pingApps.Add(ping.Install(remoteHostContainer));
        }
        // configure clients on sector 2
        for (uint32_t i = 0; i < ueSector2IpIface.GetN(); i++)
        {
            // in case of NGMN traffic allow installation of the specific traffic type only on the
            // specific nodes
            if ((trafficTypeConf == NGMN_MIXED) and (ngmnHttpIds.find(i) == ngmnHttpIds.end()))
            {
                continue;
            }

            Ipv4Address ipAddress = ueSector2IpIface.GetAddress(i, 0);
            PingHelper ping(ipAddress);
            pingApps.Add(ping.Install(remoteHostContainer));
        }
        // configure clients on sector 3
        for (uint32_t i = 0; i < ueSector3IpIface.GetN(); i++)
        {
            // in case of NGMN traffic allow installation of the specific traffic type only on the
            // specific nodes
            if ((trafficTypeConf == NGMN_MIXED) and (ngmnHttpIds.find(i) == ngmnHttpIds.end()))
            {
                continue;
            }

            Ipv4Address ipAddress = ueSector3IpIface.GetAddress(i, 0);
            PingHelper ping(ipAddress);
            pingApps.Add(ping.Install(remoteHostContainer));
        }
    }

    if (trafficTypeConf == UDP_CBR)
    {
        for (uint32_t i = 0; i < ueSector1Container.GetN(); ++i)
        {
            dlClientLowLat.SetAttribute(
                "Interval",
                TimeValue(Seconds(1.0 / lambdaPerCell[(i % gridScenario.GetNumSites()) *
                                                      gridScenario.GetNumSectorsPerSite()])));
            std::cout << "ue (sector1): " << i << " index: "
                      << (i % gridScenario.GetNumSites()) * gridScenario.GetNumSectorsPerSite()
                      << " lambda: "
                      << lambdaPerCell[(i % gridScenario.GetNumSites()) *
                                       gridScenario.GetNumSectorsPerSite()]
                      << std::endl;
            Ptr<Node> ue = ueSector1Container.Get(i);
            Ptr<NetDevice> ueDevice = ueSector1NetDev.Get(i);
            Address ueAddress = ueSector1IpIface.GetAddress(i);

            // The client, who is transmitting, is installed in the remote host,
            // with destination address set to the address of the UE
            if (direction == "DL")
            {
                dlClientLowLat.SetAttribute("RemoteAddress", AddressValue(ueAddress));
                clientApps.Add(dlClientLowLat.Install(remoteHost));
            }
            else
            {
                dlClientLowLat.SetAttribute("RemoteAddress", AddressValue(remoteHostAddr));
                clientApps.Add(dlClientLowLat.Install(ue));
            }
            // Activate a dedicated bearer for the traffic type
            nrHelper->ActivateDedicatedEpsBearer(ueDevice, lowLatBearer, lowLatTft);
        }

        for (uint32_t i = 0; i < ueSector2Container.GetN(); ++i)
        {
            dlClientLowLat.SetAttribute(
                "Interval",
                TimeValue(Seconds(1.0 / lambdaPerCell[(i % gridScenario.GetNumSites()) *
                                                          gridScenario.GetNumSectorsPerSite() +
                                                      1])));
            std::cout << "ue (sector2): " << i << " index: "
                      << (i % gridScenario.GetNumSites()) * gridScenario.GetNumSectorsPerSite() + 1
                      << " lambda: "
                      << lambdaPerCell[(i % gridScenario.GetNumSites()) *
                                           gridScenario.GetNumSectorsPerSite() +
                                       1]
                      << std::endl;
            Ptr<Node> ue = ueSector2Container.Get(i);
            Ptr<NetDevice> ueDevice = ueSector2NetDev.Get(i);
            Address ueAddress = ueSector2IpIface.GetAddress(i);

            // The client, who is transmitting, is instaviso entonces pronto, sualled in the remote
            // host, with destination address set to the address of the UE
            if (direction == "DL")
            {
                dlClientLowLat.SetAttribute("RemoteAddress", AddressValue(ueAddress));
                clientApps.Add(dlClientLowLat.Install(remoteHost));
            }
            else
            {
                dlClientLowLat.SetAttribute("RemoteAddress", AddressValue(remoteHostAddr));
                clientApps.Add(dlClientLowLat.Install(ue));
            }
            // Activate a dedicated bearer for the traffic type
            nrHelper->ActivateDedicatedEpsBearer(ueDevice, lowLatBearer, lowLatTft);
        }

        for (uint32_t i = 0; i < ueSector3Container.GetN(); ++i)
        {
            dlClientLowLat.SetAttribute(
                "Interval",
                TimeValue(Seconds(1.0 / lambdaPerCell[(i % gridScenario.GetNumSites()) *
                                                          gridScenario.GetNumSectorsPerSite() +
                                                      2])));
            std::cout << "ue (sector3): " << i << " index: "
                      << (i % gridScenario.GetNumSites()) * gridScenario.GetNumSectorsPerSite() + 2
                      << " lambda: "
                      << lambdaPerCell[(i % gridScenario.GetNumSites()) *
                                           gridScenario.GetNumSectorsPerSite() +
                                       2]
                      << std::endl;
            Ptr<Node> ue = ueSector3Container.Get(i);
            Ptr<NetDevice> ueDevice = ueSector3NetDev.Get(i);
            Address ueAddress = ueSector3IpIface.GetAddress(i);

            // The client, who is transmitting, is installed in the remote host,
            // with destination address set to the address of the UE
            if (direction == "DL")
            {
                dlClientLowLat.SetAttribute("RemoteAddress", AddressValue(ueAddress));
                clientApps.Add(dlClientLowLat.Install(remoteHost));
            }
            else
            {
                dlClientLowLat.SetAttribute("RemoteAddress", AddressValue(remoteHostAddr));
                clientApps.Add(dlClientLowLat.Install(ue));
            }
            // Activate a dedicated bearer for the traffic type
            nrHelper->ActivateDedicatedEpsBearer(ueDevice, lowLatBearer, lowLatTft);
        }
    }

    // Add one or two pings for ARP at the beginning of the simulation
    pingApps.Start(Seconds(0.300));
    pingApps.Stop(Seconds(0.500));
    serverApps.Start(serverAppStartTime);
    serverApps.Stop(simTime - MilliSeconds(400));
    clientApps.Start(clientAppStartTime);
    clientApps.Stop(simTime - MilliSeconds(400));

    // enable the traces provided by the nr module
    if (traces)
    {
        nrHelper->EnableTraces();
    }

    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    endpointNodes.Add(remoteHost);
    endpointNodes.Add(gridScenario.GetUserTerminals());

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install(endpointNodes);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));

    Simulator::Stop(simTime);
    Simulator::Run();

    // Print per-flow statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    std::ofstream outFile;
    std::string filename = outputDir + "/" + simTag;
    std::vector<double> delayValues(stats.size());
    uint64_t cont = 0;

    outFile.open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!outFile.is_open())
    {
        std::cerr << "Can't open file " << filename << std::endl;
        return 1;
    }

    outFile.setf(std::ios_base::fixed);

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
         i != stats.end();
         ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        std::stringstream protoStream;
        protoStream << (uint16_t)t.protocol;
        if (t.protocol == 6)
        {
            protoStream.str("TCP");
        }
        if (t.protocol == 17)
        {
            protoStream.str("UDP");
        }
        std::cout << "Flow " << i->first << " (" << t.sourceAddress << ":" << t.sourcePort << " -> "
                  << t.destinationAddress << ":" << t.destinationPort << ") proto "
                  << protoStream.str() << "\n";
        std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
        std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
        std::cout << "  TxOffered:  "
                  << i->second.txBytes * 8.0 / (simTime - appStartTime).GetSeconds() / 1000.0 /
                         1000.0
                  << " Mbps\n";
        std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
        std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        if (i->second.rxPackets > 0)
        {
            // Measure the duration of the flow from receiver's perspective
            // double rxDuration = i->second.timeLastRxPacket.GetSeconds () -
            // i->second.timeFirstTxPacket.GetSeconds ();
            double rxDuration = (simTime - appStartTime).GetSeconds();

            averageFlowThroughput += i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
            averageFlowDelay += 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;
            delayValues[cont] = 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets;
            cont++;

            std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / rxDuration / 1000 / 1000
                      << " Mbps\n";
            std::cout << "  Mean delay:  "
                      << double(1000 * i->second.delaySum.GetSeconds()) / (i->second.rxPackets)
                      << " ms\n";
            std::cout << "  Last packet delay: " << i->second.lastDelay.As(Time::MS) << "\n";
            // std::cout << "  Mean upt:  " << i->second.uptSum / i->second.rxPackets / 1000/1000 <<
            // " Mbps \n";
            std::cout << "  Mean jitter:  "
                      << 1000 * i->second.jitterSum.GetSeconds() / i->second.rxPackets << " ms\n";
        }
        else
        {
            // outFile << "  Throughput:  0 Mbps\n";
            // outFile << "  Mean delay:  0 ms\n";
            // outFile << "  Mean jitter: 0 ms\n";
        }
        // outFile << "  Rx Packets: " << i->second.rxPackets << "\n";
    }
    std::stable_sort(delayValues.begin(), delayValues.end());
    // for (uint32_t i = 0; i < stats.size(); i++)
    //   {
    //     std::cout << delayValues[i] << " ";
    //   }
    // double FiftyTileFlowDelay = (delayValues[stats.size()/2] + delayValues[stats.size()/2 -1])/2;
    double FiftyTileFlowDelay = delayValues[stats.size() / 2];

    outFile << "\n\n  Mean flow throughput: " << averageFlowThroughput / stats.size() << "\n";
    outFile << "  Mean flow delay: " << averageFlowDelay / stats.size() << "\n";
    outFile << "  Median flow delay: " << FiftyTileFlowDelay << "\n";

    outFile.close();

    std::ifstream f(filename.c_str());

    if (f.is_open())
    {
        std::cout << f.rdbuf();
    }

    Simulator::Destroy();
    return 0;
}
