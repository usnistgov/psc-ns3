// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef LENA_V2_UTILS_H
#define LENA_V2_UTILS_H

#include "sinr-output-stats.h"

#include <ns3/hexagonal-grid-scenario-helper.h>
#include <ns3/nr-module.h>

namespace ns3
{

class SinrOutputStats;
class PowerOutputStats;
class SlotOutputStats;
class RbOutputStats;

class LenaV2Utils
{
  public:
    static void SetLenaV2SimulatorParameters(const double sector0AngleRad,
                                             const std::string& scenario,
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
                                             bool calibration,
                                             bool enableUlPc,
                                             std::string powerAllocation,
                                             SinrOutputStats* sinrStats,
                                             PowerOutputStats* ueTxPowerStats,
                                             PowerOutputStats* gnbRxPowerStats,
                                             SlotOutputStats* slotStats,
                                             RbOutputStats* rbStats,
                                             const std::string& scheduler,
                                             uint32_t bandwidthMHz,
                                             uint32_t freqScenario,
                                             double downtiltAngle);
    static void ReportSinrNr(SinrOutputStats* stats,
                             uint16_t cellId,
                             uint16_t rnti,
                             double avgSinr,
                             uint16_t bwpId);
    static void ReportPowerNr(PowerOutputStats* stats,
                              const SfnSf& sfnSf,
                              Ptr<const SpectrumValue> txPsd,
                              const Time& t,
                              uint16_t rnti,
                              uint64_t imsi,
                              uint16_t bwpId,
                              uint16_t cellId);
    static void ReportSlotStatsNr(SlotOutputStats* stats,
                                  const SfnSf& sfnSf,
                                  uint32_t scheduledUe,
                                  uint32_t usedReg,
                                  uint32_t usedSym,
                                  uint32_t availableRb,
                                  uint32_t availableSym,
                                  uint16_t bwpId,
                                  uint16_t cellId);
    static void ReportRbStatsNr(RbOutputStats* stats,
                                const SfnSf& sfnSf,
                                uint8_t sym,
                                const std::vector<int>& rbUsed,
                                uint16_t bwpId,
                                uint16_t cellId);
    static void ReportGnbRxDataNr(PowerOutputStats* gnbRxDataStats,
                                  const SfnSf& sfnSf,
                                  Ptr<const SpectrumValue> rxPsd,
                                  const Time& t,
                                  uint16_t bwpId,
                                  uint16_t cellId);

    static void ConfigureBwpTo(BandwidthPartInfoPtr& bwp, double centerFreq, double bwpBw);
};

} // namespace ns3

#endif // LENA_V2_UTILS_H
