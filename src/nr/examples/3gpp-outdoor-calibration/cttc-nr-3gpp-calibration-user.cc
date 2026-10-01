// Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "cttc-nr-3gpp-calibration.h"

#include <ns3/command-line.h>
#include <ns3/show-progress.h>

using namespace ns3;

/**
 * \ingroup examples
 * \file cttc-nr-3gpp-calibration-user.cc
 * \brief A multi-cell network deployment with site sectorization
 *
 * This example describes how to setup a simulation using the 3GPP channel model
 * from TR 38.901. This example consists of an hexagonal grid deployment
 * consisting on a central site and a number of outer rings of sites around this
 * central site. Each site is sectorized, meaning that a number of three antenna
 * arrays or panels are deployed per gNB. These three antennas are pointing to
 * 30º, 150º and 270º w.r.t. the horizontal axis. We allocate a band to each
 * sector of a site, and the bands are contiguous in frequency.
 *
 * We provide a number of simulation parameters that can be configured in the
 * command line, such as the number of UEs per cell or the number of outer rings.
 * Please have a look at the possible parameters to know what you can configure
 * through the command line.
 *
 * With the default configuration, the example will create one DL flow per UE.
 * The example will print on-screen the end-to-end result of each flow,
 * as well as writing them on a file.
 *
 * \code{.unparsed}
$ ./ns3 run "cttc-nr-3gpp-calibration-user --PrintHelp"
    \endcode
 *
 */
int
main(int argc, char* argv[])
{
    Parameters params;
    /*
     * From here, we instruct the ns3::CommandLine class of all the input parameters
     * that we may accept as input, as well as their description, and the storage
     * variable.
     */
    CommandLine cmd(__FILE__);

    cmd.AddValue("configurationType",
                 "Choose among a) customConf and b) calibrationConf."
                 "a) allows custom configuration through the command line,"
                 "while b) allows user to select one of the predefined"
                 "calibration scenarios. Please notice that if b) is selected"
                 "custom parameters should not be set through the command line",
                 params.confType);
    cmd.AddValue("nrConfigurationScenario",
                 "The NR calibration scenario string. Choose among:"
                 "DenseA, DenseB, RuralA, RuralB"
                 "This variable must be set when calibrationConf is chosen",
                 params.nrConfigurationScenario);
    cmd.AddValue("scenario",
                 "The urban scenario string (UMa, RMa, UMi_StreeCanyon)",
                 params.scenario);
    cmd.AddValue("numRings", "The number of rings", params.numOuterRings);
    cmd.AddValue("ueNumPergNb",
                 "The number of UE per cell or gNB in multiple-ue topology",
                 params.ueNumPergNb);
    cmd.AddValue("appGenerationTime",
                 "Duration applications will generate traffic.",
                 params.appGenerationTime);
    cmd.AddValue("numerologyBwp", "The numerology to be used (NR only)", params.numerologyBwp);
    cmd.AddValue("pattern", "The TDD pattern to use", params.pattern);
    cmd.AddValue("direction", "The flow direction (DL or UL)", params.direction);
    cmd.AddValue("simulator",
                 "The cellular network simulator to use: LENA or 5GLENA",
                 params.simulator);
    cmd.AddValue("technology",
                 "The radio access network technology (LTE or NR)",
                 params.radioNetwork);
    cmd.AddValue("operationMode",
                 "The network operation mode can be TDD or FDD",
                 params.operationMode);
    cmd.AddValue("simTag",
                 "tag to be appended to output filenames to distinguish simulation campaigns",
                 params.simTag);
    cmd.AddValue("outputDir", "directory where to store simulation results", params.outputDir);
    cmd.AddValue("errorModelType",
                 "Error model type: ns3::NrEesmCcT1, ns3::NrEesmCcT2, "
                 "ns3::NrEesmIrT1, ns3::NrEesmIrT2, ns3::NrLteMiErrorModel",
                 params.errorModel);
    cmd.AddValue("lenaCalibration",
                 "whether to configure 4G LENA in calibration mode",
                 params.lenaCalibration);
    cmd.AddValue("enableFading",
                 "If false, Fading (and consequently beamforming) will be disabled "
                 "when simulator is 5GLENA. Default value true (enabled). Notice "
                 "that if fading is disabled, also Shadowing must be disabled",
                 params.enableFading);
    cmd.AddValue("enableShadowing", "If true, it enables Shadowing", params.enableShadowing);
    cmd.AddValue("bfMethod",
                 "The BF method string. Can be a) Omni, b) CellScan c) fixedBeam"
                 "d) CellScanAzimuth."
                 "Notice that if Shadowing and Fading are disabled, fixedBeam will"
                 "be used. Default value is CellScan",
                 params.bfMethod);
    cmd.AddValue("trafficScenario",
                 "0: saturation (80 Mbps/20 MHz), 1: latency (1 pkt of 12 bytes), "
                 "2: low-load (1 Mbps), 3: medium-load (20Mbps), 4: extremely high "
                 "load (50% more than saturation mode)",
                 params.trafficScenario);
    cmd.AddValue("scheduler", "PF: Proportional Fair, RR: Round-Robin", params.scheduler);
    cmd.AddValue("bandwidth",
                 "BW in MHz for each BWP (integer value): valid values are 20, 10, 5",
                 params.bandwidthMHz);
    cmd.AddValue("startingFreq",
                 "Frequency for the first band. Rest of the bands will be configured"
                 "accordingly based on the configured BW",
                 params.startingFreq);
    cmd.AddValue("freqScenario",
                 "0: NON_OVERLAPPING (each sector in different freq), "
                 "1: OVERLAPPING (same freq for all sectors)",
                 params.freqScenario);
    cmd.AddValue("attachToClosest",
                 "When freqScenario is set to 1 (OVERLAPPING) then attachToClosest "
                 "can be set to true to allow the attachment to closest gNBs",
                 params.attachToClosest);
    cmd.AddValue("downtiltAngle",
                 "Base station antenna downtilt angle (deg)",
                 params.downtiltAngle);
    cmd.AddValue("enableUlPc", "Whether to enable or disable UL power control", params.enableUlPc);
    cmd.AddValue("powerAllocation",
                 "Power allocation can be a)UniformPowerAllocBw or b)UniformPowerAllocUsed.",
                 params.powerAllocation);
    cmd.AddValue("xMin", "The min x coordinate of the rem map", params.xMinRem);
    cmd.AddValue("xMax", "The max x coordinate of the rem map", params.xMaxRem);
    cmd.AddValue("xRes", "The resolution on the x axis of the rem map", params.xResRem);
    cmd.AddValue("yMin", "The min y coordinate of the rem map", params.yMinRem);
    cmd.AddValue("yMax", "The max y coordinate of the rem map", params.yMaxRem);
    cmd.AddValue("yRes", "The resolution on the y axis of the rem map", params.yResRem);
    cmd.AddValue("z", "The z coordinate of the rem map", params.zRem);
    cmd.AddValue("dlRem", "Generates DL REM without executing simulation", params.dlRem);
    cmd.AddValue("ulRem", "Generates UL REM without executing simulation", params.ulRem);
    cmd.AddValue("remSector", "For which sector to generate the rem", params.remSector);
    cmd.AddValue("progressInterval", "Progress reporting interval", params.progressInterval);
    cmd.AddValue("gnbTxPower", "The transmit power of the gNB", params.gnbTxPower);
    cmd.AddValue("ueTxPower", "The transmit power of the UE", params.ueTxPower);
    cmd.AddValue("gnbNumRows",
                 "The number of rows of the phased array of the gNB",
                 params.gnbNumRows);
    cmd.AddValue("gnbNumColumns",
                 "The number of columns of the phased array of the gNB",
                 params.gnbNumColumns);
    cmd.AddValue("ueNumRows", "The number of rows of the phased array of the UE", params.ueNumRows);
    cmd.AddValue("ueNumColumns",
                 "The number of columns of the phased array of the UE",
                 params.ueNumColumns);
    cmd.AddValue("gnbHSpacing",
                 "Horizontal spacing between antenna elements, "
                 "in multiples of wave length, for the gNB",
                 params.gnbHSpacing);
    cmd.AddValue("gnbVSpacing",
                 "Vertical spacing between antenna elements, "
                 "in multiples of wave length for the gNB",
                 params.gnbVSpacing);
    cmd.AddValue("ueHSpacing",
                 "Horizontal spacing between antenna elements, "
                 "in multiples of wave length, for the UE",
                 params.ueHSpacing);
    cmd.AddValue("ueVSpacing",
                 "Vertical spacing between antenna elements, "
                 "in multiples of wave length, for the UE",
                 params.ueVSpacing);
    cmd.AddValue("crossPolarizedGnb",
                 "Whether the gNB antenna array has the cross polarized antenna "
                 "elements. If yes, gNB supports 2 streams, otherwise only 1 stream",
                 params.crossPolarizedGnb);
    cmd.AddValue("crossPolarizedUe",
                 "Whether the UE antenna array has the cross polarized antenna "
                 "elements. If yes, UE supports 2 streams, otherwise only 1 stream",
                 params.crossPolarizedUe);
    cmd.AddValue("polSlantAngleGnb1",
                 "Polarization slant angle of the first panel of gNB in degrees",
                 params.polSlantAngleGnb1);
    cmd.AddValue("polSlantAngleGnb2",
                 "Polarization slant angle of the second panel of gNB in degrees",
                 params.polSlantAngleGnb2);
    cmd.AddValue("polSlantAngleUe1",
                 "Polarization slant angle of the first panel of UE in degrees",
                 params.polSlantAngleUe1);
    cmd.AddValue("ueBearingAngle",
                 "UE Bearing Angle. If true it sets to a random value among 0 and"
                 "360 the UE bearing angle",
                 params.ueBearingAngle);
    cmd.AddValue("polSlantAngleUe2",
                 "Polarization slant angle of the second panel of UE in degrees",
                 params.polSlantAngleUe2);
    cmd.AddValue("gnbNoiseFigure", "gNB Noise Figure", params.gnbNoiseFigure);
    cmd.AddValue("ueNoiseFigure", "UE Noise Figure", params.ueNoiseFigure);
    cmd.AddValue("enableRealBF",
                 "If true, Real BeamForming method is configured (must be disabled "
                 "for calibration)",
                 params.enableRealBF);
    cmd.AddValue("gnbEnable3gppElement",
                 "If true, it enables 3GPP Antenna element configuration in the gNB",
                 params.gnbEnable3gppElement);
    cmd.AddValue("ueEnable3gppElement",
                 "If true, it enables 3GPP Antenna element configuration in the UE",
                 params.ueEnable3gppElement);
    cmd.AddValue("checkUeMobility",
                 "If true, it enables printing of UE position every 100 ms",
                 params.checkUeMobility);
    cmd.AddValue("basicTraces",
                 "If true, it enables printing of the PHY traces. If by mistake is "
                 "enabled along with extendedTraces, it all traces will be enabled",
                 params.basicTraces);
    cmd.AddValue("extendedTraces",
                 "If true, it enables printing all traces",
                 params.extendedTraces);
    cmd.AddValue("maxUeClosestSiteDistance",
                 "Max distance between UE and the closest site",
                 params.maxUeClosestSiteDistance);
    cmd.AddValue("bfConfSector", "Beamforming fixed beam sector value.", params.bfConfSector);
    cmd.AddValue("bfConfElevation",
                 "Beamforming fixed beam elevation value.",
                 params.bfConfElevation);
    cmd.AddValue("useLastUeForRem",
                 "If true, last UE will be passed as rrd to REM. This can be useful"
                 "when there are UEs with random antenna heights, so instead of "
                 "passing to REM a UE with random hUT, we pass one with 1.5m.",
                 params.useLastUeForRem);
    cmd.AddValue("isd", "The Inter-Site Distance value.", params.isd);
    cmd.AddValue("bsHeight", "The antenna height of the BS.", params.bsHeight);
    cmd.AddValue("utHeight", "The antenna height of the UE.", params.utHeight);
    cmd.AddValue("minBsUtDistance",
                 "The minimum distance between UE and BS.",
                 params.minBsUtDistance);
    cmd.AddValue("o2iThreshold",
                 "The ratio of O2I channel conditions. Default value is 0 that "
                 "corresponds to 0 O2I losses.",
                 params.o2iThreshold);
    cmd.AddValue("o2iLowLossThreshold",
                 "The ratio of O2I low - high penetration losses. Default value "
                 "is 1.0 meaning that all losses will be low.",
                 params.o2iLowLossThreshold);
    cmd.AddValue("linkO2iConditionToAntennaHeight",
                 "Specifies whether the O2I condition will be determined based on "
                 "the UE height, i.e. if the UE height is 1.5 then it is O2O,"
                 "otherwise it is O2I.",
                 params.linkO2iConditionToAntennaHeight);
    cmd.AddValue("ueSpeed", "The UE speed.", params.speed);

    // Parse the command line
    cmd.Parse(argc, argv);
    params.Validate();

    // in case calibrationConf is chosen, it sets the parameters of one
    // of the NR pre-defined scenarios

    if (params.confType == "calibrationConf")
    {
        ChooseCalibrationScenario(params);
    }

    std::cout << params;

    ShowProgress spinner(params.progressInterval);

    Nr3gppCalibration(params);

    return 0;
}
