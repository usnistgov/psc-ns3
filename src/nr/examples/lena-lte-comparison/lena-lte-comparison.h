// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include <ns3/nstime.h>

#include <ostream>
#include <string>

#ifndef LENA_LTE_COMPARISON_H
#define LENA_LTE_COMPARISON_H

namespace ns3
{

struct Parameters
{
    friend std::ostream& operator<<(std::ostream& os, const Parameters& parameters);

    bool Validate() const;

    uint16_t numOuterRings = 3;
    uint16_t ueNumPergNb = 2;
    bool logging = false;
    bool traces = false;
    std::string simulator = "5GLENA";
    std::string scenario = "UMa";
    std::string radioNetwork = "NR";   // LTE or NR
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

    // Where we will store the output files.
    std::string simTag = "default";
    std::string outputDir = "./";

    // Error models
    std::string errorModel = "";

    bool calibration = true;
    bool enableUlPc = false;
    std::string powerAllocation = "UniformPowerAllocUsed";

    uint32_t trafficScenario = 0;

    std::string scheduler = "PF";
    uint32_t freqScenario = 0;

    double downtiltAngle = 0;

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

    Time progressInterval = Seconds(1);
};

extern void LenaLteComparison(const Parameters& params);

} // namespace ns3

#endif // LENA_LTE_COMPARISON_H
