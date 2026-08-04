// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_3GPP_CALIBRATION_UTILS_V2_H
#define NR_3GPP_CALIBRATION_UTILS_V2_H

#include "sinr-output-stats.h"

#include "ns3/random-variable-stream.h"
#include <ns3/distance-based-three-gpp-spectrum-propagation-loss-model.h>
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
                                             bool ueBearingAngle);

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

#endif // NR_3GPP_CALIBRATION_UTILS_V2_H
