/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Nicola Baldo <nbaldo@cttc.es>
 *          Lluis Parcerisa <lparcerisa@cttc.cat>
 */


#ifndef LTE_RRC_SAP_H
#define LTE_RRC_SAP_H

/// Sidelink Constraint values (from 6.4 RRC multiplicity and type constraint values 3GPP TS 36.331)
#define MAXSL_DEST 16
#define MAXSL_TF_INDEXPAIR 64
#define MAXSL_TXPOOL 4
#define MAXSL_RXPOOL 16
#define MAXSL_DEST 16

#define MAXSL_DISC_POWERCLASS 3

#define MAX_PLMN 6

#include <stdint.h>
#include <list>
#include <bitset>

#include <ns3/ptr.h>
#include <ns3/simulator.h>

namespace ns3 {

class LteRlcSapUser;
class LtePdcpSapUser;
class LteRlcSapProvider;
class LtePdcpSapProvider;
class Packet;

/**
 * \ingroup lte
 *
 * \brief Class holding definition common to all UE/eNodeB SAP Users/Providers.
 *
 * See 3GPP TS 36.331 for reference.
 *
 * Note that only those values that are (expected to be) used by the
 * ns-3 model are mentioned here. The naming of the variables that are
 * defined here is the same of 36.331, except for removal of "-" and
 * conversion to CamelCase or ALL_CAPS where needed in order to follow
 * the ns-3 coding style. Due to the 1-to-1 mapping with TS 36.331,
 * detailed doxygen documentation is omitted, so please refer to
 * 36.331 for the meaning of these data structures / fields.
 *
 */
class LteRrcSap
{
public:
  virtual ~LteRrcSap ();

  /// Constraint values
  static const uint8_t MaxReportCells = 8;

  // Information Elements
  /// PlmnIdentityInfo structure
  struct PlmnIdentityInfo
  {
    uint32_t plmnIdentity; ///< PLMN identity
  };

  /// CellAccessRelatedInfo structure
  struct CellAccessRelatedInfo
  {
    PlmnIdentityInfo plmnIdentityInfo; ///< PLMN identity info
    uint32_t cellIdentity; ///< cell identity
    bool csgIndication; ///< CSG indication
    uint32_t csgIdentity; ///< CSG identity
  };

  /// CellSelectionInfo structure
  struct CellSelectionInfo
  {
    int8_t qRxLevMin; ///< INTEGER (-70..-22), actual value = IE value * 2 [dBm].
    int8_t qQualMin; ///< INTEGER (-34..-3), actual value = IE value [dB].
  };

  /// FreqInfo structure
  struct FreqInfo
  {
    uint32_t ulCarrierFreq; ///< UL carrier frequency
    uint8_t ulBandwidth; ///< UL bandwidth
  };

  /// RlcConfig structure
  struct RlcConfig
  {
    /// the direction choice
    enum direction
    {
      AM,
      UM_BI_DIRECTIONAL,
      UM_UNI_DIRECTIONAL_UL,
      UM_UNI_DIRECTIONAL_DL
    } choice; ///< direction choice
  };

  /// LogicalChannelConfig structure
  struct LogicalChannelConfig
  {
    uint8_t priority; ///< priority
    uint16_t prioritizedBitRateKbps; ///< prioritized bit rate Kbps
    uint16_t bucketSizeDurationMs; ///< bucket size duration ms
    uint8_t logicalChannelGroup; ///< logical channel group
  };

  /// SoundingRsUlConfigCommon structure
  struct SoundingRsUlConfigCommon
  {
    /// the config action
    enum action
    {
      SETUP, RESET
    } type; ///< action type
    uint8_t srsBandwidthConfig; ///< SRS bandwidth config
    uint8_t srsSubframeConfig; ///< SRS subframe config
  };

  /// SoundingRsUlConfigDedicated structure
  struct SoundingRsUlConfigDedicated
  {
    /// the config action
    enum action
    {
      SETUP, RESET
    } type; ///< action type
    uint8_t srsBandwidth; ///< SRS bandwidth
    uint16_t srsConfigIndex; ///< SRS config index
  };

  /// AntennaInfoDedicated structure
  struct AntennaInfoDedicated
  {
    uint8_t transmissionMode; ///< transmission mode
  };

  /// PdschConfigCommon structure
  struct PdschConfigCommon
  {
    int8_t referenceSignalPower;  ///< INTEGER (-60..50),
    int8_t pb;                    ///< INTEGER (0..3),
  };

  /// PdschConfigDedicated structure
  struct PdschConfigDedicated
  {
    /**
     * P_A values, TS 36.331 6.3.2 PDSCH-Config
     * ENUMERATED { dB-6, dB-4dot77, dB-3, dB-1dot77, dB0, dB1, dB2, dB3 }
     */
    enum db
    {
      dB_6,
      dB_4dot77,
      dB_3,
      dB_1dot77,
      dB0,
      dB1,
      dB2,
      dB3
    };
    uint8_t pa; ///< P_A value
  };

  /**
   * Convert PDSCH config dedicated function
   *
   * \param pdschConfigDedicated PdschConfigDedicated
   * \returns double value
   */
  static double ConvertPdschConfigDedicated2Double (PdschConfigDedicated pdschConfigDedicated)
  {
    double pa = 0;
    switch (pdschConfigDedicated.pa)
      {
      case PdschConfigDedicated::dB_6:
        pa = -6;
        break;
      case PdschConfigDedicated::dB_4dot77:
        pa = -4.77;
        break;
      case PdschConfigDedicated::dB_3:
        pa = -3;
        break;
      case PdschConfigDedicated::dB_1dot77:
        pa = -1.77;
        break;
      case PdschConfigDedicated::dB0:
        pa = 0;
        break;
      case PdschConfigDedicated::dB1:
        pa = 1;
        break;
      case PdschConfigDedicated::dB2:
        pa = 2;
        break;
      case PdschConfigDedicated::dB3:
        pa = 3;
        break;
      default:
        break;
      }
    return pa;
  }

  /// PhysicalConfigDedicated structure
  struct PhysicalConfigDedicated
  {
    bool haveSoundingRsUlConfigDedicated; ///< have sounding RS UL config dedicated?
    SoundingRsUlConfigDedicated soundingRsUlConfigDedicated; ///< sounding RS UL config dedicated
    bool haveAntennaInfoDedicated; ///< have antenna info dedicated?
    AntennaInfoDedicated antennaInfo; ///< antenna info
    bool havePdschConfigDedicated; ///< have PDSCH config dedicated?
    PdschConfigDedicated pdschConfigDedicated; ///< PDSCH config dedicated
  };


  /// SrbToAddMod structure
  struct SrbToAddMod
  {
    uint8_t srbIdentity; ///< SB identity
    LogicalChannelConfig logicalChannelConfig; ///< logical channel config
  };

  /// DrbToAddMod structure
  struct DrbToAddMod
  {
    uint8_t epsBearerIdentity; ///< EPS bearer identity
    uint8_t drbIdentity; ///< DRB identity
    RlcConfig rlcConfig; ///< RLC config
    uint8_t logicalChannelIdentity; ///< logical channel identify
    LogicalChannelConfig logicalChannelConfig; ///< logical channel config
  };

  /// PreambleInfo structure
  struct PreambleInfo
  {
    uint8_t numberOfRaPreambles; ///< number of RA preambles
  };

  /// RaSupervisionInfo structure
  struct RaSupervisionInfo
  {
    uint8_t preambleTransMax; ///< preamble transmit maximum
    uint8_t raResponseWindowSize; ///< RA response window size
  };

  /// RachConfigCommon structure
  struct RachConfigCommon
  {
    PreambleInfo preambleInfo; ///< preamble info
    RaSupervisionInfo raSupervisionInfo; ///< RA supervision info
  };

  /// RadioResourceConfigCommon structure
  struct RadioResourceConfigCommon
  {
    RachConfigCommon rachConfigCommon; ///< RACH config common
  };

  /// RadioResourceConfigCommonSib structure
  struct RadioResourceConfigCommonSib
  {
    RachConfigCommon rachConfigCommon; ///< RACH config common
    PdschConfigCommon pdschConfigCommon; ///< PDSCH config common
  };

  /// RadioResourceConfigDedicated structure
  struct RadioResourceConfigDedicated
  {
    std::list<SrbToAddMod> srbToAddModList; ///< SRB to add mod list
    std::list<DrbToAddMod> drbToAddModList; ///< DRB to add mod list
    std::list<uint8_t> drbToReleaseList; ///< DRB to release list
    bool havePhysicalConfigDedicated; ///< have physical config dedicated?
    PhysicalConfigDedicated physicalConfigDedicated; ///< physical config dedicated
  };

  /// QuantityConfig structure
  struct QuantityConfig
  {
    uint8_t filterCoefficientRSRP; ///< filter coefficient RSRP
    uint8_t filterCoefficientRSRQ; ///< filter coefficient RSRQ
  };

  /// CellsToAddMod structure
  struct CellsToAddMod
  {
    uint8_t cellIndex; ///< cell index
    uint16_t physCellId; ///< Phy cell ID
    int8_t cellIndividualOffset; ///< cell individual offset
  };

  /// PhysCellIdRange structure
  struct PhysCellIdRange
  {
    uint16_t start; ///< starting cell ID
    bool haveRange; ///< has a range?
    uint16_t range; ///< the range
  };

  /// BlackCellsToAddMod structure
  struct BlackCellsToAddMod
  {
    uint8_t cellIndex; ///< cell index
    PhysCellIdRange physCellIdRange; ///< Phy cell ID range
  };

  /// MeasObjectEutra structure
  struct MeasObjectEutra
  {
    uint32_t carrierFreq; ///< carrier frequency
    uint8_t allowedMeasBandwidth; ///< allowed measure bandwidth
    bool presenceAntennaPort1; ///< antenna port 1 present?
    uint8_t neighCellConfig; ///< neighbor cell config
    int8_t offsetFreq; ///< offset frequency
    std::list<uint8_t> cellsToRemoveList; ///< cells to remove list
    std::list<CellsToAddMod> cellsToAddModList; ///< cells to add mod list
    std::list<uint8_t> blackCellsToRemoveList; ///< black cells to remove list
    std::list<BlackCellsToAddMod> blackCellsToAddModList; ///< black cells to add mod list
    bool haveCellForWhichToReportCGI; ///< have cell for which to report CGI?
    uint16_t cellForWhichToReportCGI; ///< cell for which to report CGI
  };

  /**
   * \brief Threshold for event evaluation.
   *
   * For RSRP-based threshold, the actual value is (value - 140) dBm. While for
   * RSRQ-based threshold, the actual value is (value - 40) / 2 dB. This is in
   * accordance with section 9.1.4 and 9.1.7 of 3GPP TS 36.133.
   *
   * \sa ns3::EutranMeasurementMapping
   */
  struct ThresholdEutra
  {
    /// Threshold enumeration
    enum
    {
      THRESHOLD_RSRP, ///< RSRP is used for the threshold.
      THRESHOLD_RSRQ ///< RSRQ is used for the threshold.
    } choice;
    uint8_t range; ///< Value range used in RSRP/RSRQ threshold.
  };

  /// Specifies criteria for triggering of an E-UTRA measurement reporting event.
  struct ReportConfigEutra
  {
    /// Trigger enumeration
    enum
    {
      EVENT,      ///< event report
      PERIODICAL  ///< periodical report
    } triggerType; ///< trigger type

    /// Event enumeration
    enum
    {
      EVENT_A1, ///< Event A1: Serving becomes better than absolute threshold.
      EVENT_A2, ///< Event A2: Serving becomes worse than absolute threshold.
      EVENT_A3, ///< Event A3: Neighbour becomes amount of offset better than PCell.
      EVENT_A4, ///< Event A4: Neighbour becomes better than absolute threshold.
      EVENT_A5  ///< Event A5: PCell becomes worse than absolute `threshold1` AND Neighbour becomes better than another absolute `threshold2`.

    } eventId; ///< Choice of E-UTRA event triggered reporting criteria.

    ThresholdEutra threshold1; ///< Threshold for event A1, A2, A4, and A5.
    ThresholdEutra threshold2; ///< Threshold for event A5.

    /// Indicates whether or not the UE shall initiate the measurement reporting procedure when the leaving condition is met for a cell in `cellsTriggeredList`, as specified in 5.5.4.1 of 3GPP TS 36.331.
    bool reportOnLeave;

    /// Offset value for Event A3. An integer between -30 and 30. The actual value is (value * 0.5) dB.
    int8_t a3Offset;

    /// Parameter used within the entry and leave condition of an event triggered reporting condition. The actual value is (value * 0.5) dB.
    uint8_t hysteresis;

    /// Time during which specific criteria for the event needs to be met in order to trigger a measurement report.
    uint16_t timeToTrigger;

    /// the report purpose
    enum report
    {
      REPORT_STRONGEST_CELLS,
      REPORT_CGI
    } purpose; ///< purpose

    /// Trigger type enumeration
    enum
    {
      RSRP, ///< Reference Signal Received Power
      RSRQ ///< Reference Signal Received Quality
    } triggerQuantity; ///< The quantities used to evaluate the triggering condition for the event, see 3GPP TS 36.214.

    /// Report type enumeration
    enum
    {
      SAME_AS_TRIGGER_QUANTITY,
      BOTH ///< Both the RSRP and RSRQ quantities are to be included in the measurement report.
    } reportQuantity; ///< The quantities to be included in the measurement report, always assumed to be BOTH.

    /// Maximum number of cells, excluding the serving cell, to be included in the measurement report.
    uint8_t maxReportCells;

    /// Report interval enumeration
    enum
    {
      MS120,
      MS240,
      MS480,
      MS640,
      MS1024,
      MS2048,
      MS5120,
      MS10240,
      MIN1,
      MIN6,
      MIN12,
      MIN30,
      MIN60,
      SPARE3,
      SPARE2,
      SPARE1
    } reportInterval; ///< Indicates the interval between periodical reports.

    /// Number of measurement reports applicable, always assumed to be infinite.
    uint8_t reportAmount;

    /// Report config eutra function
    ReportConfigEutra ();

  }; // end of struct ReportConfigEutra

  /// MeasObjectToAddMod structure
  struct MeasObjectToAddMod
  {
    uint8_t measObjectId; ///< measure object ID
    MeasObjectEutra measObjectEutra; ///< measure object eutra
  };

  /// ReportConfigToAddMod structure
  struct ReportConfigToAddMod
  {
    uint8_t reportConfigId; ///< report config ID
    ReportConfigEutra reportConfigEutra; ///< report config eutra
  };

  /// MeasIdToAddMod structure
  struct MeasIdToAddMod
  {
    uint8_t measId; ///< measure ID
    uint8_t measObjectId; ///< measure object ID
    uint8_t reportConfigId; ///< report config ID
  };

  /// MeasGapConfig structure
  struct MeasGapConfig
  {
    /// the action type
    enum action
    {
      SETUP, RESET
    } type; ///< action type
    /// the gap offest
    enum gap
    {
      GP0, GP1
    } gapOffsetChoice; ///< gap offset
    uint8_t gapOffsetValue; ///< gap offset value
  };

  /// MobilityStateParameters structure
  struct MobilityStateParameters
  {
    uint8_t tEvaluation; ///< evaluation
    uint8_t tHystNormal; ///< hyst normal
    uint8_t nCellChangeMedium; ///< cell change medium
    uint8_t nCellChangeHigh; ///< cell change high
  };

  /// SpeedStateScaleFactors structure
  struct SpeedStateScaleFactors
  {
    // 25 = oDot25, 50 = oDot5, 75 = oDot75, 100 = lDot0
    uint8_t sfMedium; ///< scale factor medium
    uint8_t sfHigh; ///< scale factor high
  };

  /// SpeedStatePars structure
  struct SpeedStatePars
  {
    /// the action type
    enum action
    {
      SETUP,
      RESET
    } type; ///< action type
    MobilityStateParameters mobilityStateParameters; ///< mobility state parameters
    SpeedStateScaleFactors timeToTriggerSf; ///< time to trigger scale factors
  };

  /// MeasConfig structure
  struct MeasConfig
  {
    std::list<uint8_t> measObjectToRemoveList; ///< measure object to remove list
    std::list<MeasObjectToAddMod> measObjectToAddModList; ///< measure object to add mod list
    std::list<uint8_t> reportConfigToRemoveList; ///< report config to remove list
    std::list<ReportConfigToAddMod> reportConfigToAddModList; ///< report config to add mod list
    std::list<uint8_t> measIdToRemoveList; ///< measure ID to remove list
    std::list<MeasIdToAddMod> measIdToAddModList; ///< measure ID to add mod list
    bool haveQuantityConfig; ///< have quantity config?
    QuantityConfig quantityConfig; ///< quantity config
    bool haveMeasGapConfig; ///< have measure gap config?
    MeasGapConfig measGapConfig; ///< measure gap config
    bool haveSmeasure; ///< have S measure?
    uint8_t sMeasure; ///< S measure
    bool haveSpeedStatePars; ///< have speed state parameters?
    SpeedStatePars speedStatePars; ///< speed state parameters
  };

  /// CarrierFreqEutra structure
  struct CarrierFreqEutra
  {
    uint32_t dlCarrierFreq; ///< DL carrier frequency
    uint32_t ulCarrierFreq; ///< UL carrier frequency
  };

  /// CarrierBandwidthEutra structure
  struct CarrierBandwidthEutra
  {
    uint8_t dlBandwidth; ///< DL bandwidth
    uint8_t ulBandwidth; ///< UL bandwidth
  };

  /// RachConfigDedicated structure
  struct RachConfigDedicated
  {
    uint8_t raPreambleIndex; ///< RA preamble index
    uint8_t raPrachMaskIndex; ///< RA PRACH mask index
  };

  /// MobilityControlInfo structure
  struct MobilityControlInfo
  {
    uint16_t targetPhysCellId; ///< target Phy cell ID
    bool haveCarrierFreq; ///< have carrier frequency?
    CarrierFreqEutra carrierFreq; ///< carrier frequency
    bool haveCarrierBandwidth; ///< have carrier bandwidth?
    CarrierBandwidthEutra carrierBandwidth; ///< carrier bandwidth
    uint16_t newUeIdentity; ///< new UE identity
    RadioResourceConfigCommon radioResourceConfigCommon; ///< radio resource config common
    bool haveRachConfigDedicated; ///< Have RACH config dedicated?
    RachConfigDedicated rachConfigDedicated; ///< RACH config dedicated
  };

  /// ReestabUeIdentity structure
  struct ReestabUeIdentity
  {
    uint16_t cRnti; ///< RNTI
    uint16_t physCellId; ///< Phy cell ID
  };

  /// ReestablishmentCause enumeration
  enum ReestablishmentCause
  {
    RECONFIGURATION_FAILURE,
    HANDOVER_FAILURE,
    OTHER_FAILURE
  };

  /// MasterInformationBlock structure
  struct MasterInformationBlock
  {
    uint8_t dlBandwidth; ///< DL bandwidth
    uint8_t systemFrameNumber; ///< system frame number
  };

  /// SystemInformationBlockType1 structure
  struct SystemInformationBlockType1
  {
    CellAccessRelatedInfo cellAccessRelatedInfo; ///< cell access related info
    CellSelectionInfo cellSelectionInfo; ///< cell selection info
  };

  /// SystemInformationBlockType2 structure
  struct SystemInformationBlockType2
  {
    RadioResourceConfigCommonSib radioResourceConfigCommon; ///< radio resource config common
    FreqInfo freqInfo; ///< frequency info
  };

  /// Information Elements for SystemInformationBlockType18
  struct SlCpLen {
    enum {
      NORMAL,
      EXTENDED
    } cplen;
  };

  struct SlPeriodComm {
    enum
      {
        sf40,
        sf60,
        sf70,
        sf80,
        sf120,
        sf140,
        sf160,
        sf240,
        sf280,
        sf320
      } period;
  };


  static SlPeriodComm PeriodAsEnum (uint32_t p_length)
  {
    SlPeriodComm p;
    switch (p_length)
      {
      case 40:
        p.period = SlPeriodComm::sf40;
        break;
      case 60:
        p.period = SlPeriodComm::sf60;
        break;
      case 70:
        p.period = SlPeriodComm::sf70;
        break;
      case 80: 
        p.period = SlPeriodComm::sf80;
        break;
      case 120:
        p.period = SlPeriodComm::sf120;
        break;
      case 140:
        p.period = SlPeriodComm::sf140;
        break;
      case 160:
        p.period = SlPeriodComm::sf160;
        break;
      case 240:
        p.period = SlPeriodComm::sf240;
        break;
      case 280:
        p.period = SlPeriodComm::sf280;
        break;
      case 320:
        p.period = SlPeriodComm::sf320;
        break;
      default:
        NS_FATAL_ERROR("SL PERIOD LENGTH NOT SUPPORTED: "<< p_length);
      }

      return p;
  }
  static uint32_t PeriodAsInt (SlPeriodComm period)
  {
    uint32_t p = 0;
    switch (period.period)
      {
      case SlPeriodComm::sf40:
        p = 40;
        break;
      case SlPeriodComm::sf60:
        p = 60;
        break;
      case SlPeriodComm::sf70:
        p = 70;
        break;
      case SlPeriodComm::sf80:
        p = 80;
        break;
      case SlPeriodComm::sf120:
        p = 120;
        break;
      case SlPeriodComm::sf140:
        p = 140;
        break;
      case SlPeriodComm::sf160:
        p = 160;
        break;
      case SlPeriodComm::sf240:
        p = 240;
        break;
      case SlPeriodComm::sf280:
        p = 280;
        break;
      case SlPeriodComm::sf320:
        p = 320;
        break;        
      }

      return p;
  }

  //Offset of the pool of resource relative to SFN 0 of the cell or DFN 0 when out of coverage
  struct SlOffsetIndicator {
    uint16_t offset; //MAx is 319 for communication, 10239 for discovery
  };

  struct SubframeBitmapSl {
    std::bitset<40> bitmap; //40 bits for FDD
  };
  
  struct SlTfResourceConfig {
    uint8_t prbNum;
    uint8_t prbStart;
    uint8_t prbEnd;
    SlOffsetIndicator offsetIndicator;
    SubframeBitmapSl subframeBitmap;

    friend bool operator==(const SlTfResourceConfig& lhs, const SlTfResourceConfig& rhs)
    {
      return lhs.prbNum == rhs.prbNum && lhs.prbStart == rhs.prbStart && lhs.prbEnd == rhs.prbEnd
        && lhs.offsetIndicator.offset == rhs.offsetIndicator.offset && lhs.subframeBitmap.bitmap == rhs.subframeBitmap.bitmap;
    }
  };

  struct SlHoppingConfigComm {
    uint16_t hoppingParameters; //valid range 0..504
    enum {
      ns1,
      ns2,
      ns4,
    } numSubbands;
    uint8_t rbOffset; //valid range 0..110
    uint8_t hoppingInfo; //Information in hopping bits. Valid range [0,3]. Values 0..2 are used for Type 1 hopping.

    friend bool operator==(const SlHoppingConfigComm& lhs, const SlHoppingConfigComm& rhs)
    {
      return lhs.hoppingParameters == rhs.hoppingParameters && lhs.numSubbands == rhs.numSubbands && lhs.rbOffset == rhs.rbOffset && lhs.hoppingInfo == rhs.hoppingInfo;
    }
  };

  struct SlTrptSubset {
    std::bitset<3> subset; //3 bits for FDD to indicate the set of k values
  };

  struct SlTxParameters {
    enum {
      al0,
      al04,
      al05,
      al06,
      al07,
      al08,
      al09,
      al1
    } alpha;
    int16_t p0; //valid range -126..31

    friend bool operator==(const SlTxParameters& lhs, const SlTxParameters& rhs)
    {
      return lhs.alpha == rhs.alpha && lhs.p0 == rhs.p0;
    }
  };

  static double AlphaAsDouble (SlTxParameters param) {
    double alpha = 0;
    switch (param.alpha) {
    case SlTxParameters::al0:
      alpha = 0;
      break;
    case SlTxParameters::al04:
      alpha = 0.4;
      break;
    case SlTxParameters::al05:
      alpha = 0.5;
      break;
    case SlTxParameters::al06:
      alpha = 0.6;
      break;
    case SlTxParameters::al07:
      alpha = 0.7;
      break;
    case SlTxParameters::al08:
      alpha = 0.8;
      break;
    case SlTxParameters::al09:
      alpha = 0.9;
      break;
    case SlTxParameters::al1:
      alpha = 1;
      break;
    }
    return alpha;
  };
  
  struct SlCommResourcePool {
    SlCpLen scCpLen;
    SlPeriodComm scPeriod;
    SlTfResourceConfig scTfResourceConfig;
    SlCpLen dataCpLen;
    SlHoppingConfigComm dataHoppingConfig;
    bool haveUeSelectedResourceConfig;
    struct UeSelectedResourceConfig {
      SlTfResourceConfig dataTfResourceConfig;
      bool haveTrptSubset;
      SlTrptSubset trptSubset; //optional
    } ueSelectedResourceConfig;
    //rxParametersNCell not specified yet
    bool haveTxParameters; //mandatory present when included in commTxPoolNormalDedicated, commTxPoolNormalCommon or commTxPoolExceptional
    struct TxParameters {
      SlTxParameters scTxParameters;
      SlTxParameters dataTxParameters;
    } txParameters;

    friend bool operator==(const SlCommResourcePool& lhs, const SlCommResourcePool& rhs)
    {
      bool equal = lhs.scCpLen.cplen == rhs.scCpLen.cplen
        && lhs.scPeriod.period == rhs.scPeriod.period
        && lhs.scTfResourceConfig == rhs.scTfResourceConfig
        && lhs.dataCpLen.cplen == rhs.dataCpLen.cplen
        && lhs.dataHoppingConfig == rhs.dataHoppingConfig
        && lhs.haveUeSelectedResourceConfig == rhs.haveUeSelectedResourceConfig;
      if (equal && lhs.haveUeSelectedResourceConfig) {
        equal = equal && lhs.ueSelectedResourceConfig.dataTfResourceConfig == rhs.ueSelectedResourceConfig.dataTfResourceConfig
          && lhs.ueSelectedResourceConfig.haveTrptSubset == rhs.ueSelectedResourceConfig.haveTrptSubset;
        if (equal && lhs.ueSelectedResourceConfig.haveTrptSubset) {
          equal = equal && lhs.ueSelectedResourceConfig.trptSubset.subset == rhs.ueSelectedResourceConfig.trptSubset.subset;
        }
      }
      equal = equal && lhs.haveTxParameters == rhs.haveTxParameters;
      if (equal && lhs.haveTxParameters) {
        equal = equal && lhs.txParameters.scTxParameters == rhs.txParameters.scTxParameters
          && lhs.txParameters.dataTxParameters == rhs.txParameters.dataTxParameters;
      }
      return equal;
    }
    
  };
  
  struct SlCommTxPoolList {
    uint8_t nbPools;
    SlCommResourcePool pools[MAXSL_TXPOOL];
  };

  struct SlCommRxPoolList {
    uint8_t nbPools;
    SlCommResourcePool pools[MAXSL_RXPOOL];
  };

  struct SlSyncConfigList {
    uint8_t nbConfig;
  };

  struct Sib18CommConfig { //SlCommConfig struct used in RRC Connection
    SlCommRxPoolList commRxPool;
    SlCommTxPoolList commTxPoolNormalCommon; //Optional (number of pools may be 0)
    SlCommTxPoolList commTxPoolExceptional;  //Optional (number of pools may be 0)
    SlSyncConfigList commSyncConfig;         //Optional (number of pools may be 0)
  };

  /// SystemInformationBlockType2 structure
  struct SystemInformationBlockType18
  {
    Sib18CommConfig commConfig;
  };

  struct SlPreconfigGeneral {
    uint16_t carrierFreq; //ulEarfcn
    uint8_t slBandwidth;  //nb RBs
  };
  
  /*
   * \brief Structure representing the Master Information Block Sidelink to be sent by SyncRefs
   */
  struct MasterInformationBlockSL
  {
    uint16_t  slBandwidth;  	///< Sidelink bandwidth[RBs]
    uint16_t  directFrameNo;	///< Frame number of the frame in which is sent this MIB-SL
    uint16_t  directSubframeNo;	///< Subframe number of the subframe in which is sent this MIB-SL
    bool    inCoverage;			///< Indicates if the SyncRef sending the MIB-SL is in coverage
    uint64_t  slssid;			///< SLSSID of the SyncRef sending the MIB-SL
    Time rxTimestamp;			///< Reception timestamp filled upon reception
    Time creationTimestamp;		///< Creation timestamp filled when created
    uint16_t rxOffset;			///< Reception offset
  };

  /*
   * \brief Structure representing the Sidelink synchronization preconfigured parameters
   * to be used in the out-of-coverage case
   */
  struct SlPreconfigSync
  {
    SlCpLen syncCPLen; 				///< Cyclic prefix length
    uint16_t syncOffsetIndicator1;  ///< First offset indicator for transmission of SLSSs. Valid values: 0 ... 39
    uint16_t syncOffsetIndicator2;  ///< Second offset indicator for transmission of SLSSs. Valid values: 0 ... 39
    int16_t syncTxThreshOoC;  		///< Threshold representing inner cell boundaries of a detected SyncRef. Unit [dBm]. Valid values: -110, -105, -100, and so on (i.e. in steps of 5), -60 (+inf not represented)
    uint16_t filterCoefficient; 	///< Filter coefficient for L3 filtering. Valid values (k): 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 17, 19
    uint16_t syncRefMinHyst;    	///< Threshold representing how higher than the minimum required the SyncRef S-RSRP should be to consider it detected. Unit [dB] Valid values: 0, 3, 6, 9, 12
    uint16_t syncRefDiffHyst;   	///< Threshold representing how higher than the (currently) selected SyncRef S-RSRP the S-RSRP of a newly detected SyncRef should be to consider a change. Unit[dB] Valid values: 0, 3, 6, 9, 12 (+inf not represented)
  };

  struct SlPreconfigCommPool { //Same as SlCommResourcePool with rxParametersNCell absent
    SlCpLen scCpLen;
    SlPeriodComm scPeriod;
    SlTfResourceConfig scTfResourceConfig;
    SlTxParameters scTxParameters;
    SlCpLen dataCpLen;
    SlHoppingConfigComm dataHoppingConfig;
    SlTfResourceConfig dataTfResourceConfig;
    SlTrptSubset trptSubset; //optional
    SlTxParameters dataTxParameters;
  };
  
  struct SlPreconfigCommPoolList {
    uint8_t nbPools;
    SlPreconfigCommPool pools[MAXSL_TXPOOL];
  };
   // Discovery period duration
  struct SlPeriodDisc {
    enum
      {
        rf32,
        rf64,
        rf128,
        rf256,
        rf512,
        rf1024
      } period;
  };

  static uint32_t DiscPeriodAsInt (SlPeriodDisc period)
  {
    uint32_t p = 0;
    switch (period.period)
      {
      case SlPeriodDisc::rf32:
        p = 320;
        break;
      case SlPeriodDisc::rf64:
        p = 640;
        break;
      case SlPeriodDisc::rf128:
        p = 1280;
        break;
      case SlPeriodDisc::rf256:
        p = 2560;
        break;
      case SlPeriodDisc::rf512:
        p = 5120;
        break;
      case SlPeriodDisc::rf1024:
        p = 10240;
        break;
      }

      return p;
  };

  // Probability of transmission for Discovery
  struct TxProbability {
    enum {
      p25,
      p50,
      p75,
      p100,
      pOptimal
    } probability;
  };

  static uint32_t TxProbabilityAsInt (TxProbability prob)
  {
    uint32_t p = 0;
    switch (prob.probability)
    {
      case TxProbability::p25:
        p = 25;
        break;
      case TxProbability::p50:
        p = 50;
        break;
      case TxProbability::p75:
        p = 75;
        break;
      case TxProbability::p100:
        p = 100;
        break;
      case TxProbability::pOptimal:
        p = 32;
        break;
    }
    return p;
  }

  static TxProbability TxProbabilityFromInt (uint32_t p)
  {
    TxProbability prob;
    switch (p)
    {
      case 25:
        prob.probability = TxProbability::p25;
        break;
      case 50:
        prob.probability = TxProbability::p50;
        break;
      case 75:
        prob.probability = TxProbability::p75;
        break;
      case 100:
        prob.probability = TxProbability::p100;
        break;
      case 32:
        prob.probability = TxProbability::pOptimal;
        break;
    }
    return prob;
  }


  struct SlPreconfigDiscPool { 
    SlCpLen cpLen;
    SlPeriodDisc discPeriod;
    int8_t numRetx; // 0..3
    int32_t numRepetition; // 1..50
    SlTfResourceConfig tfResourceConfig;
    struct TxParameters {
      SlTxParameters txParametersGeneral;
      TxProbability txProbability; 
    } txParameters;

  };
  
  struct SlPreconfigDiscPoolList {
    uint8_t nbPools;
    SlPreconfigDiscPool pools[MAXSL_TXPOOL];
  };

  struct SlPreconfiguration
  {
    SlPreconfigGeneral preconfigGeneral;
    SlPreconfigSync preconfigSync; ///< Synchronization configuration
    SlPreconfigCommPoolList preconfigComm;
    SlPreconfigDiscPoolList preconfigDisc;
  };

  struct PeriodicBsrTimer
  {

    enum {
      sf5,
      sf10,
      sf16,
      sf20,
      sf32,
      sf40,
      sf64,
      sf80,
      sf128,
      sf160,
      sf320,
      sf640,
      sf1280,
      sf2560,
      infinity
    } period;
  };

  struct RetxBsrTimer {
    enum {
      sf320,
      sf640,
      sf1280,
      sf2560,
      sf5120,
      sf10240
    } period;
  };
  
  struct SlMacMainConfigSl //MAC-MainConfigSL-r12
  {
    PeriodicBsrTimer periodicBsrTimer;
    RetxBsrTimer rtxBsrTimer;
  };

  struct SlCommTxPoolToAddMod
  {
    uint8_t poolIdentity;
    SlCommResourcePool pool;
  };
  
  struct SlCommTxPoolToAddModList
  {
    uint8_t nbPools;
    SlCommTxPoolToAddMod pools[MAXSL_TXPOOL];
  };

  struct SlTxPoolToReleaseList {
    uint8_t nbPools;
    uint8_t poolIdentities[MAXSL_TXPOOL];
  };
  
  struct SlCommConfigScheduled { //Indicates the configuration for the case E-UTRAN schedules the transmission resources based on sidelink specific BSR from the UE.
    uint16_t crnti;
    SlMacMainConfigSl macMainConfig;
    SlCommResourcePool commTxConfig;
    bool haveMcs; //indicates if MCS is being set
    uint8_t mcs; //0..28
  };
  
  struct SlCommConfigUeSelected { //commTxPoolNormalDedicated: Indicates a pool of transmission resources the UE is allowed to use while in RRC_CONNECTED.
    bool havePoolToRelease;
    SlTxPoolToReleaseList poolToRelease;
    bool havePoolToAdd;
    SlCommTxPoolToAddModList poolToAddModList;
  };

  struct SlCommTxResourcesSetup {//for dedicated configuration
    enum {
      SCHEDULED,
      UE_SELECTED
    } setup; //indicates which type of resources is being allocated
    SlCommConfigScheduled scheduled;
    SlCommConfigUeSelected ueSelected;
  };
  
  struct SlCommConfig {//for dedicated configuration
    enum {
      RELEASE,
      SETUP
    } commTxResources; //indicates if it is allocating or releasing resources
    SlCommTxResourcesSetup setup; //valid only if commTxResources = setup
  };

  struct SlDestinationInfoList {
    int nbDestinations;
    uint32_t SlDestinationIdentity[MAXSL_DEST]; //each destination is 24 bit long.
  };
  
  struct SlCommTxResourceReq {
    uint32_t carrierFreq;
    SlDestinationInfoList slDestinationInfoList;
  };
  
  /// end SystemInformationBlockType18

  /// Start SystemInformationBlockType19

  //Discovery period and probability of transmission are defined above 
  //in order to set the preconfigured pools
 
  struct PoolSelection 
  {
    enum {
      RSRPBASED,
      RANDOM
    } selection;
  };
  struct PoolSelectionRsrpBased {
    uint32_t threshLow; // 0..7
    uint32_t threshHigh; // 0..7
  }; // Value 0 corresponds to -infinity, value 1 to -110dBm, value 2 to -100dBm, and so on (i.e. in steps of 10dBm) until value 6, which corresponds to -60dBm, while value 7 corresponds to +infinity.

  static uint32_t RsrpValueDbm (uint32_t rsrp)
  {
    uint32_t r = 0;
    switch (rsrp)
    {
      case 1:
        r =- -110;
        break;
      case 2:
        r = -100;
        break;
      case 3:
        r = -90;
        break;
      case 4:
        r = -80;
        break;
      case 5:
        r = -70;
        break;
      case 6:
        r = -60;
        break;
    }
    return r;
  }

  struct SubframeAssignement {
    enum {
      sa0,
      sa1,
      sa2,
      sa3,
      sa4,
      sa5,
      sa6
    } sa;
  };
  struct SpecialSubframePatterns {
    enum {
      ssp0,
      ssp1,
      ssp2,
      ssp3,
      ssp4,
      ssp5,
      ssp6,
      ssp7,
      ssp8
    } ssp;
  };

  struct SlDiscResourcePool {
    SlCpLen cpLen; // defined for communication.
    SlPeriodDisc discPeriod;
    int8_t numRetx; // 0..3
    int32_t numRepetition; // 1..50
    SlTfResourceConfig tfResourceConfig; 
    bool haveTxParameters;
    struct TxParameters {
      SlTxParameters txParametersGeneral;
      struct UeSelectedResourceConfig{
        PoolSelection poolSelection;
        bool havePoolSelectionRsrpBased;
        PoolSelectionRsrpBased poolSelectionRsrpBased; 
        TxProbability txProbability; 
      } ueSelectedResourceConfig;
    } txParameters;
    bool haveRxParameters;
    struct RxParameters {
      struct TddConfig {
        SubframeAssignement subframeAssignement;
        SpecialSubframePatterns specialSubframePatterns;
      } tddConfig;
      uint32_t syncConfigIndex; // 0 .. 15
    } rxParameters;
  
    friend bool operator==(const SlDiscResourcePool& lhs, const SlDiscResourcePool& rhs)
    {
      bool equal = lhs.cpLen.cplen == rhs.cpLen.cplen
        && lhs.discPeriod.period == rhs.discPeriod.period
        && lhs.numRetx == rhs.numRetx
        && lhs.numRepetition == rhs.numRepetition
        && lhs.tfResourceConfig == rhs.tfResourceConfig
        && lhs.haveTxParameters == rhs.haveTxParameters;
      if (equal && lhs.haveTxParameters) {
        equal = equal && lhs.txParameters.txParametersGeneral == rhs.txParameters.txParametersGeneral
          && lhs.txParameters.ueSelectedResourceConfig.poolSelection.selection == rhs.txParameters.ueSelectedResourceConfig.poolSelection.selection;
      }
      equal = equal && lhs.txParameters.ueSelectedResourceConfig.havePoolSelectionRsrpBased == rhs.txParameters.ueSelectedResourceConfig.havePoolSelectionRsrpBased;
      if (equal && lhs.txParameters.ueSelectedResourceConfig.havePoolSelectionRsrpBased) {
        equal = equal && lhs.txParameters.ueSelectedResourceConfig.poolSelectionRsrpBased.threshHigh == rhs.txParameters.ueSelectedResourceConfig.poolSelectionRsrpBased.threshHigh
          && lhs.txParameters.ueSelectedResourceConfig.poolSelectionRsrpBased.threshLow == rhs.txParameters.ueSelectedResourceConfig.poolSelectionRsrpBased.threshLow
          && lhs.txParameters.ueSelectedResourceConfig.txProbability.probability == rhs.txParameters.ueSelectedResourceConfig.txProbability.probability;
      }
      equal = equal && lhs.haveRxParameters == rhs.haveRxParameters;
      if (equal && lhs.haveRxParameters) {
        equal = equal && lhs.rxParameters.tddConfig.subframeAssignement.sa == rhs.rxParameters.tddConfig.subframeAssignement.sa
          && lhs.rxParameters.tddConfig.specialSubframePatterns.ssp == rhs.rxParameters.tddConfig.specialSubframePatterns.ssp
          && lhs.rxParameters.syncConfigIndex == rhs.rxParameters.syncConfigIndex;
      }
      return equal;
    }
  };

  struct SlDiscTxPoolList {
    uint8_t nbPools;
    SlDiscResourcePool pools[MAXSL_TXPOOL];
  };

  struct SlDiscRxPoolList {
    uint8_t nbPools;
    SlDiscResourcePool pools[MAXSL_RXPOOL];
  };

  struct SlDiscTxPowerInfo {
    uint32_t discMaxTxPower; //-30..30
  };

  // The first entry in SLDiscTxPowerInfoList corresponds to UE range class "short", the second entry corresponds to "medium" and the third entry corresponds to "long"
  struct SlDiscTxPowerInfoList {
    uint8_t nbPowerInfo;
    SlDiscTxPowerInfo power[MAXSL_DISC_POWERCLASS];
  };

  struct Sib19DiscConfig { // SlDiscConfig struct used in RRC Connection
    SlDiscRxPoolList discRxPool;
    SlDiscTxPoolList discTxPoolCommon; // Optional 
    SlDiscTxPowerInfoList discTxPowerInfo;  // Optional 
    SlSyncConfigList discSyncConfig;         // Optional 
  };
  
  struct PlmnIdentityList
  {
    int nbPlmn;
    PlmnIdentityInfo plmnIdentityInfo[MAX_PLMN]; 
  };

  struct SlCarrierFreqInfoList
  {
    uint16_t carrierFreq;
    PlmnIdentityList plmnIdentityList;
  };

  struct SystemInformationBlockType19
  {
    Sib19DiscConfig discConfig;
    SlCarrierFreqInfoList discInterFreqList;
  };
  
  struct SlDiscTxPoolToAddMod
  {
    uint8_t poolIdentity;
    SlDiscResourcePool pool;
  };
  
  struct SlDiscTxPoolToAddModList
  {
    uint8_t nbPools;
    SlDiscTxPoolToAddMod pools[MAXSL_TXPOOL];
  };

  struct SlTfIndexPair {
    uint32_t discSfIndex; //1..200
    uint32_t discPrbIndex; //1..50
  };

  struct SlTfIndexPairList {
    uint8_t nbPair;
    SlTfIndexPair pair[MAXSL_TF_INDEXPAIR];
  };

  struct SlHoppingConfigDisc {
    uint32_t a; //1..200
    uint32_t b; //1..10
    enum {
      n1,
      n5
    } c;
  };

  struct SlDiscConfigScheduled { 
    SlDiscResourcePool discTxConfig;
    SlTfIndexPairList discTfIndexList;
    SlHoppingConfigDisc discHoppingConfigDisc;
  };

  struct SlDiscConfigUeSelected { 
    bool havePoolToRelease;
    SlTxPoolToReleaseList poolToRelease;
    bool havePoolToAdd;
    SlDiscTxPoolToAddModList poolToAddModList;
  };

  struct SlDiscTxResourcesSetup {
    enum {
      SCHEDULED,
      UE_SELECTED
    } setup;
    SlDiscConfigScheduled scheduled;
    SlDiscConfigUeSelected ueSelected;
  };
  
  struct SlDiscConfig {
    enum {
      RELEASE,
      SETUP
    } discTxResources; 
    SlDiscTxResourcesSetup setup;
  };

  /// End SystemInformationBlockType19

  /// SystemInformation structure
  struct SystemInformation
  {
    bool haveSib2; ///< have SIB2?
    SystemInformationBlockType2 sib2; ///< SIB2
    bool haveSib18; ///< have SIB18
    SystemInformationBlockType18 sib18; ///< SIB18 
    bool haveSib19; ///< have SIB19
    SystemInformationBlockType19 sib19; ///< SIB19
  };

  /// AsConfig structure
  struct AsConfig
  {
    MeasConfig sourceMeasConfig; ///< source measure config
    RadioResourceConfigDedicated sourceRadioResourceConfig; ///< source radio resource config
    uint16_t sourceUeIdentity; ///< source UE identity
    MasterInformationBlock sourceMasterInformationBlock; ///< source master information block
    SystemInformationBlockType1 sourceSystemInformationBlockType1; ///< source system information block type 1
    SystemInformationBlockType2 sourceSystemInformationBlockType2; ///< source system information block type 2
    uint32_t sourceDlCarrierFreq; ///< source DL carrier frequency
  };

  /// CgiInfo structure
  struct CgiInfo
  {
    uint32_t plmnIdentity; ///< PLMN identity
    uint32_t cellIdentity; ///< cell identity
    uint16_t trackingAreaCode; ///< tracking area code
    std::list<uint32_t> plmnIdentityList; ///< PLMN identity list
  };

  /// MeasResultEutra structure
  struct MeasResultEutra
  {
    uint16_t physCellId; ///< Phy cell ID
    bool haveCgiInfo; ///< have CGI info?
    CgiInfo cgiInfo; ///< CGI info
    bool haveRsrpResult; ///< have RSRP result
    uint8_t rsrpResult; ///< RSRP result
    bool haveRsrqResult; ///< have RSRQ result?
    uint8_t rsrqResult; ///< RSRQ result
  };

  /// MeasResultScell structure
  struct MeasResultScell
  {
    uint16_t servFreqId; ///< service frequency ID
    bool haveRsrpResult; ///< have RSRP result?
    uint8_t rsrpResult; ///< the RSRP result
    bool haveRsrqResult; ///< have RSRQ result?
    uint8_t rsrqResult; ///< the RSRQ result
  };

  /// MeasResultBestNeighCell structure
  struct MeasResultBestNeighCell
  {
    uint16_t servFreqId; ///< service frequency ID
    uint16_t physCellId; ///< physical cell ID
    bool haveRsrpResult; ///< have RSRP result?
    uint8_t rsrpResult; ///< the RSRP result
    bool haveRsrqResult; ///< have RSRQ result?
    uint8_t rsrqResult; ///< the RSRQ result
  };

  /// MeasResultServFreqList
  struct MeasResultServFreqList
  {
    bool haveMeasurementResultsServingSCells; ///< have measure results serving Scells
    std::list<MeasResultScell> measResultScell; ///< measure results Scells
    bool haveMeasurementResultsNeighCell; ///< always false since not implemented
    std::list<MeasResultBestNeighCell> measResultBestNeighCell; ///< measure result best neighbor cell
  };

  /// MeasResults structure
  struct MeasResults
  {
    uint8_t measId; ///< measure ID
    uint8_t rsrpResult; ///< RSRP result
    uint8_t rsrqResult; ///< RSRQ result
    bool haveMeasResultNeighCells; ///< have measure result neighbor cells
    std::list<MeasResultEutra> measResultListEutra; ///< measure result list eutra
    bool haveScellsMeas; ///< has SCells measure
    MeasResultServFreqList measScellResultList; ///< measure SCell result list
  };

  // Messages

  /// RrcConnectionRequest structure
  struct RrcConnectionRequest
  {
    uint64_t ueIdentity; ///< UE identity
  };

  /// RrcConnectionSetup structure
  struct RrcConnectionSetup
  {
    uint8_t rrcTransactionIdentifier; ///< RRC transaction identifier
    RadioResourceConfigDedicated radioResourceConfigDedicated; ///< radio resource config dedicated
  };

  /// RrcConnectionSetupCompleted structure
  struct RrcConnectionSetupCompleted
  {
    uint8_t rrcTransactionIdentifier; ///< RRC transaction identifier
  };


  /// CellIdentification structure
  struct CellIdentification
  {
    uint32_t physCellId; ///< physical cell ID
    uint32_t dlCarrierFreq; ///<  ARFCN - valueEUTRA
  };

  /// AntennaInfoCommon structure
  struct AntennaInfoCommon
  {
    uint16_t antennaPortsCount; ///< antenna ports count
  };

  /// UlPowerControlCommonSCell structure
  struct UlPowerControlCommonSCell
  {
    uint16_t alpha; ///< alpha value
  };
  
  /// PrachConfigSCell structure
  struct PrachConfigSCell
  {
    uint16_t index; ///< the index
  };

  /// NonUlConfiguration structure
  struct NonUlConfiguration
  {
    // 3GPP TS 36.311 v.11.10 R11 pag.220
    /// 1: Cell characteristics
    uint16_t dlBandwidth;
    /// 2: Physical configuration, general antennaInfoCommon-r10
    AntennaInfoCommon antennaInfoCommon;
    // 3: Physical configuration, control phich-Config-r10
    // Not Implemented
    /// 4: Physical configuration, physical channels pdsch-ConfigCommon-r10
    PdschConfigCommon pdschConfigCommon;
    // 5: tdd-Config-r10
    //Not Implemented
  };

  /// UlConfiguration structure
  struct UlConfiguration 
  { 
    FreqInfo ulFreqInfo; ///< UL frequency info
    UlPowerControlCommonSCell ulPowerControlCommonSCell; ///< 3GPP TS 36.331 v.11.10 R11 pag.223 
    SoundingRsUlConfigCommon soundingRsUlConfigCommon; ///< sounding RS UL config common
    PrachConfigSCell prachConfigSCell; ///< PRACH config SCell
    //PushConfigCommon pushConfigCommon; //NOT IMPLEMENTED!
  };

  /// AntennaInfoUl structure
  struct AntennaInfoUl
  {
    uint8_t transmissionMode; ///< transmission mode
  };

  /// PuschConfigDedicatedSCell structure
  struct PuschConfigDedicatedSCell
  {
    /// 3GPP TS 36.331 v.11.10 R11 page 216
    uint16_t nPuschIdentity;
  };

  /// UlPowerControlDedicatedSCell structure
  struct UlPowerControlDedicatedSCell
  {
    /// 3GPP TS 36.331 v.11.10 R11 page 234
    uint16_t pSrsOffset;
  };

  /// PhysicalConfigDedicatedSCell structure
  struct PhysicalConfigDedicatedSCell
  {
    // Non-Ul Configuration
    bool haveNonUlConfiguration; ///< have non UL configuration?
    bool haveAntennaInfoDedicated; ///< have antenna info dedicated?
    AntennaInfoDedicated antennaInfo; ///< antenna info dedicated
    bool crossCarrierSchedulingConfig; ///< currently implemented as boolean variable --> implementing crossCarrierScheduling is out of the scope of this GSoC proposal
    bool havePdschConfigDedicated; ///< have PDSCH config dedicated?
    PdschConfigDedicated pdschConfigDedicated; ///< PDSCH config dedicated

    // Ul Configuration
    bool haveUlConfiguration; ///< have UL configuration?
    bool haveAntennaInfoUlDedicated; ///< have antenna info UL dedicated?
    AntennaInfoDedicated antennaInfoUl; ///< antenna info UL
    PuschConfigDedicatedSCell pushConfigDedicatedSCell; ///< PUSCH config dedicated SCell
    UlPowerControlDedicatedSCell  ulPowerControlDedicatedSCell; ///< UL power control dedicated SCell
    bool haveSoundingRsUlConfigDedicated; ///< have sounding RS UL config dedicated?
    SoundingRsUlConfigDedicated soundingRsUlConfigDedicated; ///< sounding RS UL config dedicated
  };

  /// RadioResourceConfigCommonSCell
  struct RadioResourceConfigCommonSCell
  {
    bool haveNonUlConfiguration; ///< have non UL configuration?
    NonUlConfiguration nonUlConfiguration; ///< non UL configuration
    bool haveUlConfiguration; ///< have UL configuration
    UlConfiguration ulConfiguration; ///< UL configuration
  };

  /// RadioResourceConfigDedicatedSCell structure
  struct RadioResourceConfigDedicatedSCell
  {
    PhysicalConfigDedicatedSCell physicalConfigDedicatedSCell; ///< physical config dedicated SCell
  };

  /// SCellToAddMod structure
  struct SCellToAddMod
  {
    uint32_t sCellIndex; ///< SCell index
    CellIdentification cellIdentification; ///< cell identification
    RadioResourceConfigCommonSCell radioResourceConfigCommonSCell; ///< radio resource config common SCell
    bool haveRadioResourceConfigDedicatedSCell; ///< have radio resource config dedicated SCell?
    RadioResourceConfigDedicatedSCell radioResourceConfigDedicateSCell; ///< radio resource config dedicated SCell
  };

  /// NonCriticalExtensionConfiguration structure
  struct NonCriticalExtensionConfiguration
  {
    std::list<SCellToAddMod> sCellsToAddModList; ///< SCell to add mod list
    std::list<uint32_t> sCellToReleaseList; ///< SCell to release list
  };

  /// RrcConnectionReconfiguration structure
  struct RrcConnectionReconfiguration
  {
    uint8_t rrcTransactionIdentifier; ///< RRC transaction identifier
    bool haveMeasConfig; ///< have measure config
    MeasConfig measConfig; ///< measure config
    bool haveMobilityControlInfo; ///< have mobility control info
    MobilityControlInfo mobilityControlInfo; ///< mobility control info
    bool haveRadioResourceConfigDedicated; ///< have radio resource config dedicated
    RadioResourceConfigDedicated radioResourceConfigDedicated; ///< radio resource config dedicated
    bool haveNonCriticalExtension; ///< have critical extension?
    /// 3GPP TS 36.331 v.11.10 R11 Sec. 6.2.2 pag. 147 (also known as ETSI TS 136 331 v.11.10 Feb-2015)
    NonCriticalExtensionConfiguration nonCriticalExtension;
    bool haveSlCommConfig; ///< have dedicated sidelink communication configuration
    SlCommConfig slCommConfig; ///< dedicated configuration for sidelink direct communication  
    bool haveSlDiscConfig; ///< have dedicated sidelink discovery configuration
    SlDiscConfig slDiscConfig; ///< dedicated configuration for sidelink direct discovery
 };

  /// RrcConnectionReconfigurationCompleted structure
  struct RrcConnectionReconfigurationCompleted
  {
    uint8_t rrcTransactionIdentifier; ///< RRC transaction identifier
  };


  /// RrcConnectionReestablishmentRequest structure
  struct RrcConnectionReestablishmentRequest
  {
    ReestabUeIdentity ueIdentity; ///< UE identity
    ReestablishmentCause reestablishmentCause; ///< reestablishment cause
  };

  /// RrcConnectionReestablishment structure
  struct RrcConnectionReestablishment
  {
    uint8_t rrcTransactionIdentifier; ///< RRC transaction identifier
    RadioResourceConfigDedicated radioResourceConfigDedicated; ///< radio resource config dedicated
  };

  /// RrcConnectionReestablishmentComplete structure
  struct RrcConnectionReestablishmentComplete
  {
    uint8_t rrcTransactionIdentifier; ///< RRC transaction identifier
  };

  /// RrcConnectionReestablishmentReject structure
  struct RrcConnectionReestablishmentReject
  {
  };

  /// RrcConnectionRelease structure
  struct RrcConnectionRelease
  {
    uint8_t rrcTransactionIdentifier; ///< RRC transaction identifier
  };

  /// RrcConnectionReject structure
  struct RrcConnectionReject
  {
    uint8_t waitTime; ///< wait time
  };

  /// HandoverPreparationInfo structure
  struct HandoverPreparationInfo
  {
    AsConfig asConfig; ///< AS config
  };

  /// MeasurementReport structure
  struct MeasurementReport
  {
    MeasResults measResults; ///< measure results
  };

  struct SidelinkUeInformation {
    bool haveCommRxInterestedFreq;
    uint32_t commRxInterestedFreq; //max value=262143
    bool haveCommTxResourceReq;
    SlCommTxResourceReq slCommTxResourceReq;
    bool haveDiscRxInterest;
    bool discRxInterest;
    bool haveDiscTxResourceReq;
    uint8_t discTxResourceReq; //1..63, use 0 as invalid
  };

};



/**
 * \brief Part of the RRC protocol. This Service Access Point (SAP) is used by
 *        the UE RRC to send messages to the eNB. Each method defined in this
 *        class corresponds to the transmission of a message that is defined in
 *        Section 6.2.2 of TS 36.331.
 */
class LteUeRrcSapUser : public LteRrcSap
{
public:
  /// SetupParameters structure
  struct SetupParameters
  {
    LteRlcSapProvider* srb0SapProvider; ///< SRB0 SAP provider
    LtePdcpSapProvider* srb1SapProvider; ///< SRB1 SAP provider
  };

  /**
   * \brief Setup function
   * \param params the setup parameters
   */
  virtual void Setup (SetupParameters params) = 0;

  /**
   * \brief Send an _RRCConnectionRequest message to the serving eNodeB
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param msg the message
   */
  virtual void SendRrcConnectionRequest (RrcConnectionRequest msg) = 0;

  /**
   * \brief Send an _RRCConnectionSetupComplete_ message to the serving eNodeB
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param msg the message
   */
  virtual void SendRrcConnectionSetupCompleted (RrcConnectionSetupCompleted msg) = 0;

  /**
   * \brief Send an _RRCConnectionReconfigurationComplete_ message to the serving eNodeB
   *        during an RRC connection reconfiguration procedure
   *        (Section 5.3.5 of TS 36.331).
   * \param msg the message
   */
  virtual void SendRrcConnectionReconfigurationCompleted (RrcConnectionReconfigurationCompleted msg) = 0;

  /**
   * \brief Send an _RRCConnectionReestablishmentRequest_ message to the serving eNodeB
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param msg the message
   */
  virtual void SendRrcConnectionReestablishmentRequest (RrcConnectionReestablishmentRequest msg) = 0;

  /**
   * \brief Send an _RRCConnectionReestablishmentComplete_ message to the serving eNodeB
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param msg the message
   */
  virtual void SendRrcConnectionReestablishmentComplete (RrcConnectionReestablishmentComplete msg) = 0;

  /**
   * \brief Send a _MeasurementReport_ message to the serving eNodeB
   *        during a measurement reporting procedure
   *        (Section 5.5.5 of TS 36.331).
   * \param msg the message
   */
  virtual void SendMeasurementReport (MeasurementReport msg) = 0;

  /**
   * \brief Send a _SidelinkUeInformation_ message to the serving eNodeB
   *        to indicate interest in sidelink transmission/reception
   * \param msg the message
   */
  virtual void SendSidelinkUeInformation (SidelinkUeInformation msg) = 0;

};


/**
 * \brief Part of the RRC protocol. This Service Access Point (SAP) is used to
 *        let the UE RRC receive a message from the eNB RRC. Each method defined
 *        in this class corresponds to the reception of a message that is
 *        defined in Section 6.2.2 of TS 36.331.
 */
class LteUeRrcSapProvider : public LteRrcSap
{
public:
  /// CompleteSetupParameters structure
  struct CompleteSetupParameters
  {
    LteRlcSapUser* srb0SapUser; ///< SRB0 SAP user
    LtePdcpSapUser* srb1SapUser; ///< SRB1 SAP user
  };

  /**
   * \brief Complete setup function
   * \param params the complete setup parameters
   */
  virtual void CompleteSetup (CompleteSetupParameters params) = 0;

  /**
   * \brief Receive a _SystemInformation_ message from the serving eNodeB
   *        during a system information acquisition procedure
   *        (Section 5.2.2 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvSystemInformation (SystemInformation msg) = 0;

  /**
   * \brief Receive an _RRCConnectionSetup_ message from the serving eNodeB
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvRrcConnectionSetup (RrcConnectionSetup msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReconfiguration_ message from the serving eNodeB
   *        during an RRC connection reconfiguration procedure
   *        (Section 5.3.5 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvRrcConnectionReconfiguration (RrcConnectionReconfiguration msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReestablishment_ message from the serving eNodeB
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvRrcConnectionReestablishment (RrcConnectionReestablishment msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReestablishmentReject_ message from the serving eNodeB
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvRrcConnectionReestablishmentReject (RrcConnectionReestablishmentReject msg) = 0;

  /**
   * \brief Receive an _RRCConnectionRelease_ message from the serving eNodeB
   *        during an RRC connection release procedure
   *        (Section 5.3.8 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvRrcConnectionRelease (RrcConnectionRelease msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReject_ message from the serving eNodeB
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvRrcConnectionReject (RrcConnectionReject msg) = 0;

};


/**
 * \brief Part of the RRC protocol. This Service Access Point (SAP) is used by
 *        the eNB RRC to send messages to the UE RRC.  Each method defined in
 *        this class corresponds to the transmission of a message that is
 *        defined in Section 6.2.2 of TS 36.331.
 */
class LteEnbRrcSapUser : public LteRrcSap
{
public:
  /// SetupUeParameters structure
  struct SetupUeParameters
  {
    LteRlcSapProvider* srb0SapProvider; ///< SRB0 SAP provider
    LtePdcpSapProvider* srb1SapProvider; ///< SRB1 SAP provider
  };

  /**
   * \brief Setup UE function
   * \param rnti the RNTI
   * \param params the setup UE parameters
   */
  virtual void SetupUe (uint16_t rnti, SetupUeParameters params) = 0;
  /**
   * \brief Remove UE function
   * \param rnti the RNTI
   */
  virtual void RemoveUe (uint16_t rnti) = 0;

  /**
   * \brief Send a _SystemInformation_ message to all attached UEs
   *        during a system information acquisition procedure
   *        (Section 5.2.2 of TS 36.331).
   * \param cellId cell ID
   * \param msg the message
   */
  virtual void SendSystemInformation (uint16_t cellId, SystemInformation msg) = 0;

  /**
   * \brief Send an _RRCConnectionSetup_ message to a UE
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendRrcConnectionSetup (uint16_t rnti, RrcConnectionSetup msg) = 0;

  /**
   * \brief Send an _RRCConnectionReconfiguration_ message to a UE
   *        during an RRC connection reconfiguration procedure
   *        (Section 5.3.5 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendRrcConnectionReconfiguration (uint16_t rnti, RrcConnectionReconfiguration msg) = 0;

  /**
   * \brief Send an _RRCConnectionReestablishment_ message to a UE
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendRrcConnectionReestablishment (uint16_t rnti, RrcConnectionReestablishment msg) = 0;

  /**
   * \brief Send an _RRCConnectionReestablishmentReject_ message to a UE
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendRrcConnectionReestablishmentReject (uint16_t rnti, RrcConnectionReestablishmentReject msg) = 0;

  /**
   * \brief Send an _RRCConnectionRelease_ message to a UE
   *        during an RRC connection release procedure
   *        (Section 5.3.8 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendRrcConnectionRelease (uint16_t rnti, RrcConnectionRelease msg) = 0;

  /**
   * \brief Send an _RRCConnectionReject_ message to a UE
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendRrcConnectionReject (uint16_t rnti, RrcConnectionReject msg) = 0;

  /**
   * \brief Encode handover prepration information
   * \param msg HandoverPreparationInfo
   * \returns the packet
   */
  virtual Ptr<Packet> EncodeHandoverPreparationInformation (HandoverPreparationInfo msg) = 0;
  /**
   * \brief Decode handover prepration information
   * \param p the packet
   * \returns HandoverPreparationInfo
   */
  virtual HandoverPreparationInfo DecodeHandoverPreparationInformation (Ptr<Packet> p) = 0;
  /**
   * \brief Encode handover command
   * \param msg RrcConnectionReconfiguration
   * \returns the packet
   */
  virtual Ptr<Packet> EncodeHandoverCommand (RrcConnectionReconfiguration msg) = 0;
  /**
   * \brief Decode handover command
   * \param p the packet
   * \returns RrcConnectionReconfiguration
   */
  virtual RrcConnectionReconfiguration DecodeHandoverCommand (Ptr<Packet> p) = 0;

};


/**
 * \brief Part of the RRC protocol. This Service Access Point (SAP) is used to
 *        let the eNB RRC receive a message from a UE RRC.  Each method defined
 *        in this class corresponds to the reception of a message that is
 *        defined in Section 6.2.2 of TS 36.331.
 */
class LteEnbRrcSapProvider : public LteRrcSap
{
public:
  /// CompleteSetupUeParameters structure
  struct CompleteSetupUeParameters
  {
    LteRlcSapUser* srb0SapUser; ///< SRB0 SAP user
    LtePdcpSapUser* srb1SapUser; ///< SRB1 SAP user
  };

  /**
   * \brief Complete setup UE function
   * \param rnti the RNTI of UE which sent the message
   * \param params CompleteSetupUeParameters
   */
  virtual void CompleteSetupUe (uint16_t rnti, CompleteSetupUeParameters params) = 0;

  /**
   * \brief Receive an _RRCConnectionRequest_ message from a UE
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvRrcConnectionRequest (uint16_t rnti,
                                         RrcConnectionRequest msg) = 0;

  /**
   * \brief Receive an _RRCConnectionSetupComplete_ message from a UE
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvRrcConnectionSetupCompleted (uint16_t rnti,
                                                RrcConnectionSetupCompleted msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReconfigurationComplete_ message from a UE
   *        during an RRC connection reconfiguration procedure
   *        (Section 5.3.5 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvRrcConnectionReconfigurationCompleted (uint16_t rnti,
                                                          RrcConnectionReconfigurationCompleted msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReestablishmentRequest_ message from a UE
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvRrcConnectionReestablishmentRequest (uint16_t rnti,
                                                        RrcConnectionReestablishmentRequest msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReestablishmentComplete_ message from a UE
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvRrcConnectionReestablishmentComplete (uint16_t rnti,
                                                         RrcConnectionReestablishmentComplete msg) = 0;

  /**
   * \brief Receive a _MeasurementReport_ message from a UE
   *        during a measurement reporting procedure
   *        (Section 5.5.5 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvMeasurementReport (uint16_t rnti, MeasurementReport msg) = 0;

  /**
   * \brief Receive a _SidelinkUeInformation_ message from a UE
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvSidelinkUeInformation (uint16_t rnti, SidelinkUeInformation msg) = 0;

};






////////////////////////////////////
//   templates
////////////////////////////////////


/**
 * Template for the implementation of the LteUeRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberLteUeRrcSapUser : public LteUeRrcSapUser
{
public:
  /**
   * Constructor
   *
   * \param owner the owner class
   */
  MemberLteUeRrcSapUser (C* owner);

  // inherited from LteUeRrcSapUser
  virtual void Setup (SetupParameters params);
  virtual void SendRrcConnectionRequest (RrcConnectionRequest msg);
  virtual void SendRrcConnectionSetupCompleted (RrcConnectionSetupCompleted msg);
  virtual void SendRrcConnectionReconfigurationCompleted (RrcConnectionReconfigurationCompleted msg);
  virtual void SendRrcConnectionReestablishmentRequest (RrcConnectionReestablishmentRequest msg);
  virtual void SendRrcConnectionReestablishmentComplete (RrcConnectionReestablishmentComplete msg);
  virtual void SendMeasurementReport (MeasurementReport msg);
  virtual void SendSidelinkUeInformation (SidelinkUeInformation msg);

private:
  MemberLteUeRrcSapUser ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberLteUeRrcSapUser<C>::MemberLteUeRrcSapUser (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberLteUeRrcSapUser<C>::MemberLteUeRrcSapUser ()
{
}

template <class C>
void
MemberLteUeRrcSapUser<C>::Setup (SetupParameters params)
{
  m_owner->DoSetup (params);
}

template <class C>
void
MemberLteUeRrcSapUser<C>::SendRrcConnectionRequest (RrcConnectionRequest msg)
{
  m_owner->DoSendRrcConnectionRequest (msg);
}

template <class C>
void
MemberLteUeRrcSapUser<C>::SendRrcConnectionSetupCompleted (RrcConnectionSetupCompleted msg)
{
  m_owner->DoSendRrcConnectionSetupCompleted (msg);
}

template <class C>
void
MemberLteUeRrcSapUser<C>::SendRrcConnectionReconfigurationCompleted (RrcConnectionReconfigurationCompleted msg)
{
  m_owner->DoSendRrcConnectionReconfigurationCompleted (msg);
}

template <class C>
void
MemberLteUeRrcSapUser<C>::SendRrcConnectionReestablishmentRequest (RrcConnectionReestablishmentRequest msg)
{
  m_owner->DoSendRrcConnectionReestablishmentRequest (msg);
}

template <class C>
void
MemberLteUeRrcSapUser<C>::SendRrcConnectionReestablishmentComplete (RrcConnectionReestablishmentComplete msg)
{
  m_owner->DoSendRrcConnectionReestablishmentComplete (msg);
}

template <class C>
void
MemberLteUeRrcSapUser<C>::SendMeasurementReport (MeasurementReport msg)
{
  m_owner->DoSendMeasurementReport (msg);
}

template <class C>
void
MemberLteUeRrcSapUser<C>::SendSidelinkUeInformation (SidelinkUeInformation msg)
{
  m_owner->DoSendSidelinkUeInformation (msg);
}

/**
 * Template for the implementation of the LteUeRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberLteUeRrcSapProvider : public LteUeRrcSapProvider
{
public:
  /**
   * Constructor
   *
   * \param owner the owner class
   */
  MemberLteUeRrcSapProvider (C* owner);

  // methods inherited from LteUeRrcSapProvider go here
  virtual void CompleteSetup (CompleteSetupParameters params);
  virtual void RecvSystemInformation (SystemInformation msg);
  virtual void RecvRrcConnectionSetup (RrcConnectionSetup msg);
  virtual void RecvRrcConnectionReconfiguration (RrcConnectionReconfiguration msg);
  virtual void RecvRrcConnectionReestablishment (RrcConnectionReestablishment msg);
  virtual void RecvRrcConnectionReestablishmentReject (RrcConnectionReestablishmentReject msg);
  virtual void RecvRrcConnectionRelease (RrcConnectionRelease msg);
  virtual void RecvRrcConnectionReject (RrcConnectionReject msg);

private:
  MemberLteUeRrcSapProvider ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberLteUeRrcSapProvider<C>::MemberLteUeRrcSapProvider (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberLteUeRrcSapProvider<C>::MemberLteUeRrcSapProvider ()
{
}

template <class C>
void
MemberLteUeRrcSapProvider<C>::CompleteSetup (CompleteSetupParameters params)
{
  m_owner->DoCompleteSetup (params);
}

template <class C>
void
MemberLteUeRrcSapProvider<C>::RecvSystemInformation (SystemInformation msg)
{
  Simulator::ScheduleNow (&C::DoRecvSystemInformation, m_owner, msg);
}

template <class C>
void
MemberLteUeRrcSapProvider<C>::RecvRrcConnectionSetup (RrcConnectionSetup msg)
{
  Simulator::ScheduleNow (&C::DoRecvRrcConnectionSetup, m_owner, msg);
}

template <class C>
void
MemberLteUeRrcSapProvider<C>::RecvRrcConnectionReconfiguration (RrcConnectionReconfiguration msg)
{
  Simulator::ScheduleNow (&C::DoRecvRrcConnectionReconfiguration, m_owner, msg);
}

template <class C>
void
MemberLteUeRrcSapProvider<C>::RecvRrcConnectionReestablishment (RrcConnectionReestablishment msg)
{
  Simulator::ScheduleNow (&C::DoRecvRrcConnectionReestablishment, m_owner, msg);
}

template <class C>
void
MemberLteUeRrcSapProvider<C>::RecvRrcConnectionReestablishmentReject (RrcConnectionReestablishmentReject msg)
{
  Simulator::ScheduleNow (&C::DoRecvRrcConnectionReestablishmentReject, m_owner, msg);
}

template <class C>
void
MemberLteUeRrcSapProvider<C>::RecvRrcConnectionRelease (RrcConnectionRelease msg)
{
  Simulator::ScheduleNow (&C::DoRecvRrcConnectionRelease, m_owner, msg);
}

template <class C>
void
MemberLteUeRrcSapProvider<C>::RecvRrcConnectionReject (RrcConnectionReject msg)
{
  Simulator::ScheduleNow (&C::DoRecvRrcConnectionReject, m_owner, msg);
}


/**
 * Template for the implementation of the LteEnbRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberLteEnbRrcSapUser : public LteEnbRrcSapUser
{
public:
  /**
   * Constructor
   *
   * \param owner the owner class
   */
  MemberLteEnbRrcSapUser (C* owner);

  // inherited from LteEnbRrcSapUser

  virtual void SetupUe (uint16_t rnti, SetupUeParameters params);
  virtual void RemoveUe (uint16_t rnti);
  virtual void SendSystemInformation (uint16_t cellId, SystemInformation msg);
  virtual void SendRrcConnectionSetup (uint16_t rnti, RrcConnectionSetup msg);
  virtual void SendRrcConnectionReconfiguration (uint16_t rnti, RrcConnectionReconfiguration msg);
  virtual void SendRrcConnectionReestablishment (uint16_t rnti, RrcConnectionReestablishment msg);
  virtual void SendRrcConnectionReestablishmentReject (uint16_t rnti, RrcConnectionReestablishmentReject msg);
  virtual void SendRrcConnectionRelease (uint16_t rnti, RrcConnectionRelease msg);
  virtual void SendRrcConnectionReject (uint16_t rnti, RrcConnectionReject msg);
  virtual Ptr<Packet> EncodeHandoverPreparationInformation (HandoverPreparationInfo msg);
  virtual HandoverPreparationInfo DecodeHandoverPreparationInformation (Ptr<Packet> p);
  virtual Ptr<Packet> EncodeHandoverCommand (RrcConnectionReconfiguration msg);
  virtual RrcConnectionReconfiguration DecodeHandoverCommand (Ptr<Packet> p);

private:
  MemberLteEnbRrcSapUser ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberLteEnbRrcSapUser<C>::MemberLteEnbRrcSapUser (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberLteEnbRrcSapUser<C>::MemberLteEnbRrcSapUser ()
{
}

template <class C>
void
MemberLteEnbRrcSapUser<C>::SetupUe (uint16_t rnti, SetupUeParameters params)
{
  m_owner->DoSetupUe (rnti, params);
}

template <class C>
void
MemberLteEnbRrcSapUser<C>::RemoveUe (uint16_t rnti)
{
  m_owner->DoRemoveUe (rnti);
}

template <class C>
void
MemberLteEnbRrcSapUser<C>::SendSystemInformation (uint16_t cellId, SystemInformation msg)
{
  m_owner->DoSendSystemInformation (cellId, msg);
}

template <class C>
void
MemberLteEnbRrcSapUser<C>::SendRrcConnectionSetup (uint16_t rnti, RrcConnectionSetup msg)
{
  m_owner->DoSendRrcConnectionSetup (rnti, msg);
}

template <class C>
void
MemberLteEnbRrcSapUser<C>::SendRrcConnectionReconfiguration (uint16_t rnti, RrcConnectionReconfiguration msg)
{
  m_owner->DoSendRrcConnectionReconfiguration (rnti, msg);
}

template <class C>
void
MemberLteEnbRrcSapUser<C>::SendRrcConnectionReestablishment (uint16_t rnti, RrcConnectionReestablishment msg)
{
  m_owner->DoSendRrcConnectionReestablishment (rnti, msg);
}

template <class C>
void
MemberLteEnbRrcSapUser<C>::SendRrcConnectionReestablishmentReject (uint16_t rnti, RrcConnectionReestablishmentReject msg)
{
  m_owner->DoSendRrcConnectionReestablishmentReject (rnti, msg);
}

template <class C>
void
MemberLteEnbRrcSapUser<C>::SendRrcConnectionRelease (uint16_t rnti, RrcConnectionRelease msg)
{
  m_owner->DoSendRrcConnectionRelease (rnti, msg);
}

template <class C>
void
MemberLteEnbRrcSapUser<C>::SendRrcConnectionReject (uint16_t rnti, RrcConnectionReject msg)
{
  m_owner->DoSendRrcConnectionReject (rnti, msg);
}

template <class C>
Ptr<Packet>
MemberLteEnbRrcSapUser<C>::EncodeHandoverPreparationInformation (HandoverPreparationInfo msg)
{
  return m_owner->DoEncodeHandoverPreparationInformation (msg);
}

template <class C>
LteRrcSap::HandoverPreparationInfo
MemberLteEnbRrcSapUser<C>::DecodeHandoverPreparationInformation (Ptr<Packet> p)
{
  return m_owner->DoDecodeHandoverPreparationInformation (p);
}


template <class C>
Ptr<Packet>
MemberLteEnbRrcSapUser<C>::EncodeHandoverCommand (RrcConnectionReconfiguration msg)
{
  return m_owner->DoEncodeHandoverCommand (msg);
}

template <class C>
LteRrcSap::RrcConnectionReconfiguration
MemberLteEnbRrcSapUser<C>::DecodeHandoverCommand (Ptr<Packet> p)
{
  return m_owner->DoDecodeHandoverCommand (p);
}

/**
 * Template for the implementation of the LteEnbRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberLteEnbRrcSapProvider : public LteEnbRrcSapProvider
{
public:
  /**
   * Constructor
   *
   * \param owner
   */
  MemberLteEnbRrcSapProvider (C* owner);

  // methods inherited from LteEnbRrcSapProvider go here

  virtual void CompleteSetupUe (uint16_t rnti, CompleteSetupUeParameters params);
  virtual void RecvRrcConnectionRequest (uint16_t rnti, RrcConnectionRequest msg);
  virtual void RecvRrcConnectionSetupCompleted (uint16_t rnti, RrcConnectionSetupCompleted msg);
  virtual void RecvRrcConnectionReconfigurationCompleted (uint16_t rnti, RrcConnectionReconfigurationCompleted msg);
  virtual void RecvRrcConnectionReestablishmentRequest (uint16_t rnti, RrcConnectionReestablishmentRequest msg);
  virtual void RecvRrcConnectionReestablishmentComplete (uint16_t rnti, RrcConnectionReestablishmentComplete msg);
  virtual void RecvMeasurementReport (uint16_t rnti, MeasurementReport msg);
  virtual void RecvSidelinkUeInformation (uint16_t rnti, SidelinkUeInformation msg);

private:
  MemberLteEnbRrcSapProvider ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberLteEnbRrcSapProvider<C>::MemberLteEnbRrcSapProvider (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberLteEnbRrcSapProvider<C>::MemberLteEnbRrcSapProvider ()
{
}

template <class C>
void
MemberLteEnbRrcSapProvider<C>::CompleteSetupUe (uint16_t rnti, CompleteSetupUeParameters params)
{
  m_owner->DoCompleteSetupUe (rnti, params);
}

template <class C>
void
MemberLteEnbRrcSapProvider<C>::RecvRrcConnectionRequest (uint16_t rnti, RrcConnectionRequest msg)
{
  Simulator::ScheduleNow (&C::DoRecvRrcConnectionRequest, m_owner, rnti, msg);
}

template <class C>
void
MemberLteEnbRrcSapProvider<C>::RecvRrcConnectionSetupCompleted (uint16_t rnti, RrcConnectionSetupCompleted msg)
{
  Simulator::ScheduleNow (&C::DoRecvRrcConnectionSetupCompleted, m_owner, rnti, msg);
}

template <class C>
void
MemberLteEnbRrcSapProvider<C>::RecvRrcConnectionReconfigurationCompleted (uint16_t rnti, RrcConnectionReconfigurationCompleted msg)
{
  Simulator::ScheduleNow (&C::DoRecvRrcConnectionReconfigurationCompleted, m_owner, rnti, msg);
}

template <class C>
void
MemberLteEnbRrcSapProvider<C>::RecvRrcConnectionReestablishmentRequest (uint16_t rnti, RrcConnectionReestablishmentRequest msg)
{
  Simulator::ScheduleNow (&C::DoRecvRrcConnectionReestablishmentRequest, m_owner, rnti, msg);
}

template <class C>
void
MemberLteEnbRrcSapProvider<C>::RecvRrcConnectionReestablishmentComplete (uint16_t rnti, RrcConnectionReestablishmentComplete msg)
{
  Simulator::ScheduleNow (&C::DoRecvRrcConnectionReestablishmentComplete, m_owner, rnti, msg);
}

template <class C>
void
MemberLteEnbRrcSapProvider<C>::RecvMeasurementReport (uint16_t rnti, MeasurementReport msg)
{
  Simulator::ScheduleNow (&C::DoRecvMeasurementReport, m_owner, rnti, msg);
}

template <class C>
void
MemberLteEnbRrcSapProvider<C>::RecvSidelinkUeInformation (uint16_t rnti, SidelinkUeInformation msg)
{
  Simulator::ScheduleNow (&C::DoRecvSidelinkUeInformation, m_owner, rnti, msg);
}














} // namespace ns3


#endif // LTE_RRC_SAP_H




