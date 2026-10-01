// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include <ns3/nstime.h>

#include <ostream>
#include <string>

#ifndef NR_3GPP_CALIBRATION_H
#define NR_3GPP_CALIBRATION_H

namespace ns3
{

struct Parameters
{
    friend std::ostream& operator<<(std::ostream& os, const Parameters& parameters);

    bool Validate() const;

    std::string confType = "customConf";            // calibrationConf
    std::string nrConfigurationScenario = "DenseA"; // DenseA, DenseB, RuralA, RuralB
    uint16_t numOuterRings = 3;
    uint16_t ueNumPergNb = 10;
    double uesWithRandomUtHeight = 0;
    bool logging = false;
    bool basicTraces = false;
    bool extendedTraces = false;
    std::string simulator = "5GLENA";
    std::string scenario = "UMa";
    std::string radioNetwork = "";     // It must be set to NR
    std::string operationMode = "TDD"; // TDD or FDD
    std::string baseStationFile = "";  // path to file of tower/site coordinates
    bool useSiteFile = false;          // whether to use baseStationFile parameter,
                                       // or to use numOuterRings parameter to create a scenario

    // Simulation parameters. Please don't use double to indicate seconds, use
    // milliseconds and integers to avoid representation errors.
    Time appGenerationTime = MilliSeconds(1000);
    Time udpAppStartTime = MilliSeconds(400);
    std::string direction = "DL";

    // Spectrum parameters. We will take the input from the command line, and then
    //  we will pass them inside the NR module.
    uint16_t numerologyBwp = 0;
    std::string pattern =
        "F|F|F|F|F|F|F|F|F|F|"; // Pattern can be e.g. "DL|S|UL|UL|DL|DL|S|UL|UL|DL|"
    uint32_t bandwidthMHz = 20;
    double startingFreq = 2110e6;

    double gnbTxPower = 40;
    double ueTxPower = 23;

    double isd = 1732;
    double bsHeight = 30.0;
    double utHeight = 1.5;
    // uint32_t sectorization = 3;
    double minBsUtDistance = 10.0;
    double antennaOffset = 1.0;

    double o2iThreshold = 0;
    double o2iLowLossThreshold =
        1.0; // shows the percentage of low losses. Default value is 100% low
    bool linkO2iConditionToAntennaHeight = false;

    double speed = 0;

    double maxUeClosestSiteDistance = 1000;

    // Where we will store the output files.
    std::string simTag = "default";
    std::string outputDir = "./";

    // Error models
    std::string errorModel = "ns3::NrEesmIrT1";

    bool lenaCalibration = true;
    bool enableFading = true;
    bool enableShadowing = true;
    std::string bfMethod = "CellScan";

    uint16_t bfConfSector = 1;
    double bfConfElevation = 30;

    bool enableRealBF = false;

    bool enableUlPc = false;
    std::string powerAllocation = "UniformPowerAllocUsed";

    uint32_t trafficScenario = UINT32_MAX;

    std::string scheduler = "PF";
    uint32_t freqScenario = 0;
    bool attachToClosest = false;

    double gnbNoiseFigure = 5.0;
    double ueNoiseFigure = 7.0;

    double xMinRem = -2000.0;
    double xMaxRem = 2000.0;
    uint16_t xResRem = 100;
    double yMinRem = -2000.0;
    double yMaxRem = 2000.0;
    uint16_t yResRem = 100;
    double zRem = 1.5;
    bool dlRem = false;
    bool ulRem = false;
    uint32_t remSector = 0;
    bool useLastUeForRem = false;

    Time progressInterval = Seconds(1);

    // Antenna Parameters
    uint32_t gnbNumRows = 4;
    uint32_t gnbNumColumns = 4;
    uint32_t ueNumRows = 4;
    uint32_t ueNumColumns = 4;

    double gnbHSpacing = 0.5;
    double gnbVSpacing = 0.5;
    double ueHSpacing = 0.5;
    double ueVSpacing = 0.5;

    double downtiltAngle = 0.0;
    bool ueBearingAngle = false;

    // Whether gNB and UE antenna arrays support
    bool crossPolarizedGnb = false;
    bool crossPolarizedUe = false;
    // The polarization slant angle in degrees
    double polSlantAngleGnb1 = 0.0; // we can set to 45
    double polSlantAngleGnb2 = -45;
    // The polarization slant angle in degrees
    double polSlantAngleUe1 = 0.0;
    double polSlantAngleUe2 = 90;

    bool gnbEnable3gppElement = true;
    bool ueEnable3gppElement = false;

    bool checkUeMobility = false;
};

extern void Nr3gppCalibration(Parameters& params);
extern void ChooseCalibrationScenario(Parameters& params);

} // namespace ns3

#endif // NR_3GPP_CALIBRATION_H
