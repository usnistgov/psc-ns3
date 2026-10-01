// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_3GPP_CALIBRATION_UTILS_V1_H
#define NR_3GPP_CALIBRATION_UTILS_V1_H

#include "sinr-output-stats.h"

#include <ns3/hexagonal-grid-scenario-helper.h>
#include <ns3/lte-module.h>
#include <ns3/nr-module.h>

namespace ns3
{

class SinrOutputStats;
class PowerOutputStats;
class SlotOutputStats;

class LenaV1Utils
{
  public:
    static void SetLenaV1SimulatorParameters(const double sector0AngleRad,
                                             std::string scenario,
                                             const std::string& confType,
                                             NodeContainer enbSector1Container,
                                             NodeContainer enbSector2Container,
                                             NodeContainer enbSector3Container,
                                             NodeContainer ueSector1Container,
                                             NodeContainer ueSector2Container,
                                             NodeContainer ueSector3Container,
                                             Ptr<PointToPointEpcHelper>& epcHelper,
                                             Ptr<LteHelper>& lteHelper,
                                             NetDeviceContainer& enbSector1NetDev,
                                             NetDeviceContainer& enbSector2NetDev,
                                             NetDeviceContainer& enbSector3NetDev,
                                             NetDeviceContainer& ueSector1NetDev,
                                             NetDeviceContainer& ueSector2NetDev,
                                             NetDeviceContainer& ueSector3NetDev,
                                             bool calibration,
                                             bool enableUlPc,
                                             SinrOutputStats* sinrStats,
                                             PowerOutputStats* powerStats,
                                             const std::string& scheduler,
                                             uint32_t bandwidthMHz,
                                             double startingFreq,
                                             uint32_t freqScenario,
                                             double gnbTxPower,
                                             double ueTxPower,
                                             const double gnbNoiseFigure,
                                             const double ueNoiseFigure,
                                             bool enableShadowing);

    static void ReportSinrLena(SinrOutputStats* stats,
                               uint16_t cellId,
                               uint16_t rnti,
                               double power,
                               double avgSinr,
                               uint8_t bwpId);
    static void ReportPowerLena(PowerOutputStats* stats, uint16_t rnti, Ptr<SpectrumValue> txPsd);
};

} // namespace ns3

#endif // NR_3GPP_CALIBRATION_UTILS_V1_H
