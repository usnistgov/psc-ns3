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

#include <stdint.h>
#include <list>
#include <array>
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
    uint16_t ulBandwidth; ///< UL bandwidth
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
    uint16_t srsBandwidthConfig; ///< SRS bandwidth config
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
    uint16_t srsBandwidth; ///< SRS bandwidth
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

  ///TxFailParams structure
  struct TxFailParam
  {
    uint8_t connEstFailCount {0}; ///< Number of times that the UE detects T300 expiry on the same cell
  };

  /// RachConfigCommon structure
  struct RachConfigCommon
  {
    PreambleInfo preambleInfo; ///< preamble info
    RaSupervisionInfo raSupervisionInfo; ///< RA supervision info
    TxFailParam txFailParam; ///< txFailParams
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
    uint16_t allowedMeasBandwidth; ///< allowed measure bandwidth
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
    uint16_t dlBandwidth; ///< DL bandwidth
    uint16_t ulBandwidth; ///< UL bandwidth
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
    uint16_t dlBandwidth; ///< DL bandwidth
    uint16_t systemFrameNumber; ///< system frame number
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

  /// SystemInformation structure
  struct SystemInformation
  {
    bool haveSib2; ///< have SIB2?
    SystemInformationBlockType2 sib2; ///< SIB2
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

  /// MeasResultPCell structure
  struct MeasResultPCell
  {
    uint8_t rsrpResult; ///< the RSRP result
    uint8_t rsrqResult; ///< the RSRQ result
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

  /// MeasResultSCell structure
  struct MeasResultSCell
  {
    uint8_t rsrpResult; ///< the RSRP result
    uint8_t rsrqResult; ///< the RSRQ result
  };

  /// MeasResultBestNeighCell structure
  struct MeasResultBestNeighCell
  {
    uint16_t physCellId; ///< physical cell ID
    uint8_t rsrpResult; ///< the RSRP result
    uint8_t rsrqResult; ///< the RSRQ result
  };

  /// MeasResultServFreq structure
  struct MeasResultServFreq {
    uint16_t servFreqId; ///< serving cell index
    bool haveMeasResultSCell; ///< have measResultSCell?
    MeasResultSCell measResultSCell; ///< SCell measurement results
    bool haveMeasResultBestNeighCell; ///< have measResultBestNeighCell?
    MeasResultBestNeighCell measResultBestNeighCell; ///< best neighbor cell measurement results
  };

  /// MeasResults structure
  struct MeasResults
  {
    uint8_t measId; ///< measure ID
    MeasResultPCell measResultPCell; ///< measurement result primary cell
    bool haveMeasResultNeighCells; ///< have measure result neighbor cells
    std::list<MeasResultEutra> measResultListEutra; ///< measure result list eutra
    bool haveMeasResultServFreqList; ///< has measResultServFreqList-r10
    std::list<MeasResultServFreq> measResultServFreqList; ///< MeasResultServFreqList-r10
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
    RadioResourceConfigDedicatedSCell radioResourceConfigDedicatedSCell; ///< radio resource config dedicated SCell
  };

  /// NonCriticalExtensionConfiguration structure
  struct NonCriticalExtensionConfiguration
  {
    std::list<SCellToAddMod> sCellToAddModList; ///< SCell to add mod list
    std::list<uint8_t> sCellToReleaseList; ///< SCell to release list
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



  // NR Sidelink IE TS 38.331


  /// Sidelink Constraint values (from 6.4 RRC multiplicity and type constraint values 3GPP TS 38.331)
//Since NR code emulate multiple carriers using BWP, we fixed the size of
//this list to 1 instead of 16.
#define MAX_NUM_OF_FREQ_SL 1 //!< Maximum number of carrier frequency for NR sidelink communication
#define MAX_SCSs 5 //!< Maximum number of subcarrier spacing specifications. Equal to the number of numerlogies supported (guessed) //TODO
#define MAX_NUM_OF_RX_POOL 16 //!< Maximum number of Rx resource poolfor NR sidelink communication
#define MAX_NUM_OF_TX_POOL 8 //!< Maximum number of Tx resource poolfor NR sidelink communication
#define MAX_NUM_OF_POOL_ID 16 //!< Maximum index of resource pool for NR sidelink communication
#define MAX_NUM_OF_SL_BWPs 4 //!< Maximum number of BWP for for NR sidelink communication

  /**
   * \brief Struct for SubcarrierSpacing enumeration
   */
  struct SubcarrierSpacing
  {
    /// SubcarrierSpacing enumeration
    enum
    {
      kHZ15,
      kHZ30,
      kHZ60,
      kHZ120,
      kHZ240,
      INVALID
    } spacing {INVALID}; //!< subcarrier spacing
  };

  /**
   * \brief Struct for sl-LengthSymbols enumeration
   */
  struct SlLengthSymbols
  {
    /// SlLengthSymbols enumeration
    enum
    {
      SYM7,
      SYM8,
      SYM9,
      SYM10,
      SYM11,
      SYM12,
      SYM13,
      SYM14,
      INVALID
    } symbols {INVALID}; //!< Number of symbols for Sidelink
  };

  /**
   * \brief Struct for sl-StartSymbol enumeration
   */
  struct SlStartSymbol
  {
    /// SlStartSymbol enumeration
    enum
    {
      SYM0,
      SYM1,
      SYM2,
      SYM3,
      SYM4,
      SYM5,
      SYM6,
      SYM7,
      INVALID
    } symbol {INVALID}; //!< Starting symbol index for Sidelink
  };

  /**
   * \brief Struct for CyclicPrefix enumeration
   */
  struct CyclicPrefix
  {
    /// CyclicPrefix enumeration
    enum
    {
      NORMAL,
      EXTENDED,
      INVALID
    } cp {INVALID}; //!< cyclic prefix
  };

  /**
   * \brief Struct for sl-TimeResourcePSCCH enumeration
   */
  struct SlTimeResourcePscch
  {
    /// SlTimeResourcePscch enumeration
    enum
    {
      N1, //!< 1 symbol //Added for modeling purpose
      N2, //!< 2 symbols
      N3, //!< 3 symbols
      INVALID
    } resources {INVALID}; //!< Number of symbols for PSCCH
  };

  /**
   * \brief Struct for sl-FreqResourcePSCCH enumeration
   */
  struct SlFreqResourcePscch
  {
    /// SlFreqResourcePscch enumeration
    enum
    {
      N10, //!< 10 PRBS
      N12,
      N15,
      N20,
      N25,
      INVALID
    } resources {INVALID}; //!< Number of RBs for PSCCH
  };

  /**
   * \brief Struct for sl-Scaling-r16 enumeration
   */
  struct SlScaling
  {
    /// SlScaling enumeration
    enum
    {
      F0P5,
      F0P65,
      F0P8,
      F1,
      INVALID
    } scaling {INVALID}; //!< scaling
  };

  /**
   * \brief Struct for sl-SubchannelSize enumeration
   */
  struct SlSubchannelSize
  {
    /// SlSubchannelSize enumeration
    enum
    {
      N10,
      N15,
      N20,
      N25,
      N50,
      N75,
      N100,
      INVALID
    } numPrbs {INVALID}; //!< Sidelink subchannel size in PRBs
  };

  /**
   * \brief Struct for sl-MCS-Table enumeration
   */
  struct SlMcsTable
  {
    /// SlMcsTable enumeration
    enum
    {
      QAM64,
      QAM256,
      //QAM64LOWSE, //TODO not supported by NR module
      INVALID
    } mcsTable {INVALID}; //!< MCS table to be used for Sidelink
  };

  /**
   * \brief Struct for sl-SensingWindow enumeration
   */
  struct SlSensingWindow
  {
    /// SlSensingWindow enumeration
    enum
    {
      MS100,
      MS1100,
      INVALID
    } windSize {INVALID}; //!< Sidelink sensing window length in milliseconds
  };

  /**
   * \brief Struct for sl-SelectionWindow
   */
  struct SlSelectionWindow
  {
    /// SlSelectionWindow enumeration
    enum
    {
      N1,
      N5,
      N10,
      N20,
      INVALID
    } windSize {INVALID}; //!< Sidelink selection window length in slots
  };

  /**
   * \brief Struct for SL-ResourceReservePeriod enumeration
   *
   * At the time of implementing this IE, 38.331 standard
   * seems confused. However, 38.214 (8.1.4) says the following:
   * The resource reservation interval, if provided, is converted
   * from units of ms to units of logical slots. So, the following
   * is ms
   */
  struct SlResourceReservePeriod
  {
    /// SlResourceReservePeriod enumeration
    enum
    {
      MS0, //!< Milliseconds
      MS10,
      MS20,
      MS30,
      MS40,
      MS50,
      MS60,
      MS70,
      MS80,
      MS90,
      MS100,
      MS150,
      MS200,
      MS250,
      MS300,
      MS350,
      MS400,
      MS450,
      MS500,
      MS550,
      MS600,
      MS650,
      MS700,
      MS750,
      MS800,
      MS850,
      MS900,
      MS950,
      MS1000,
      INVALID
    } period {INVALID}; //!< Sidelink resource reservation period in milliseconds.
  };

  /**
   * \brief struct for sl-MaxNumPerReserve-r16 enumeration
   */
  struct SlMaxNumPerReserve
  {
    /// SlMaxNumPerReserve enumeration
    enum
    {
      N1, //Added for modeling
      N2,
      N3,
      INVALID
    } maxNumPerRes {INVALID}; //!< Sidelink MaxNumPerReserve in slots
  };


  /**
   * \brief Get subcarrier spacing value
   *
   * This method converts the enum of type LteRrcSap::SubcarrierSpacing
   * to its unsigned integer representation.
   *
   * \param scs The object of type LteRrcSap::SubcarrierSpacing
   * \returns The unsigned integer representation of subcarrier spacing value in Hz
   */
  static uint32_t GetScSpacingValue (const LteRrcSap::SubcarrierSpacing &scs);

  /**
   * \brief Get Subcarrier spacing enum
   *
   * This method converts the subcarrier spacing unsigned value to
   * an enum of type LteRrcSap::SubcarrierSpacing
   *
   * \param scs The subcarrier spacing in Hz
   * \returns The object of type LteRrcSap::SubcarrierSpacing
   */
  static LteRrcSap::SubcarrierSpacing GetScSpacingEnum (uint32_t scs);


  /**
   * \brief Get sidelink length symbols value
   *
   * This method converts the enum of type LteRrcSap::SlLengthSymbols
   * to its unsigned integer representation.
   *
   * \param slLengthSyms The object of type LteRrcSap::SlLengthSymbols
   * \returns The unsigned integer representation of LteRrcSap::SlLengthSymbols
   */
  static uint16_t GetSlLengthSymbolsValue (const LteRrcSap::SlLengthSymbols &slLengthSyms);

  /**
   * \brief Get sidelink length symbols enum
   *
   * This method converts the sidelink length in symbols to
   * an enum of type LteRrcSap::SlLengthSymbols.
   *
   * \param slLengthSyms The total number of symbols allocated to SL in a slot
   * \returns The object of type LteRrcSap::SlLengthSymbols
   */
  static LteRrcSap::SlLengthSymbols GetSlLengthSymbolsEnum (uint16_t slLengthSyms);

  /**
   * \brief Get sidelink start symbol value
   *
   * This method converts the enum of type LteRrcSap::SlStartSymbol
   * to its unsigned integer representation.
   *
   * \param slStartSym The object of type LteRrcSap::SlStartSymbol
   * \returns The unsigned integer representation of LteRrcSap::SlStartSymbol
   */
  static uint16_t GetSlStartSymbolValue (const LteRrcSap::SlStartSymbol &slStartSym);

  /**
   * \brief Get sidelink start symbol enum
   *
   * This method converts the sidelink start symbol to
   * an enum of type LteRrcSap::SlStartSymbol.
   *
   * \param slStartSym The starting symbol of SL allocation in a slot
   * \returns The object of type LteRrcSap::SlStartSymbol
   */
  static LteRrcSap::SlStartSymbol GetSlStartSymbolEnum (uint16_t slStartSym);

  /**
   * \brief Get sidelink time resource PSCCH value
   *
   * This method converts the enum of type LteRrcSap::SlTimeResourcePscch
   * to its unsigned integer representation.
   *
   * \param slTResoPscch The object of type LteRrcSap::SlTimeResourcePscch
   * \returns The unsigned integer representation of LteRrcSap::SlTimeResourcePscch
   */
  static uint16_t GetSlTResoPscchValue (const LteRrcSap::SlTimeResourcePscch &slTResoPscch);

  /**
   * \brief Get sidelink time resource PSCCH enum
   *
   * This method converts the sidelink time resources for PSCCH length (in
   * symbols) to an enum of type LteRrcSap::SlTimeResourcePscch.
   *
   * \param slTResoPscch The number of symbols assigned for PSCCH transmission in a slot
   * \returns The object of type LteRrcSap::SlTimeResourcePscch
   */
  static LteRrcSap::SlTimeResourcePscch GetSlTResoPscchEnum (uint16_t slTResoPscch);

  /**
   * \brief Get sidelink frequency resource PSCCH value
   *
   * This method converts the enum of type LteRrcSap::SlFreqResourcePscch
   * to its unsigned integer representation.
   *
   * \param slFResoPscch The object of type LteRrcSap::SlFreqResourcePscch
   * \returns The unsigned integer representation of LteRrcSap::SlFreqResourcePscch
   */
  static uint16_t GetSlFResoPscchValue (const LteRrcSap::SlFreqResourcePscch &slFResoPscch);

  /**
   * \brief Get sidelink frequency resource PSCCH enum
   *
   * This method converts the sidelink frequency resources for PSCCH length (in
   * PRBs) to an enum of type LteRrcSap::SlFreqResourcePscch.
   *
   * \param slFResoPscch The number of symbols assigned for PSCCH transmission in a slot
   * \returns The object of type LteRrcSap::SlFreqResourcePscch
   */
  static LteRrcSap::SlFreqResourcePscch GetSlFResoPscchEnum (uint16_t slFResoPscch);

  /**
   * \brief Get sidelink scaling value
   *
   * This method converts the enum of type LteRrcSap::SlScaling
   * to its float representation.
   *
   * \param slScaling The object of type LteRrcSap::SlScaling
   * \returns The float representation of LteRrcSap::SlScaling
   */
  static float GetSlScalingValue (const LteRrcSap::SlScaling &slScaling);

  /**
   * \brief Get sidelink scaling enum.
   *
   * This method converts the sidelink scaling value to an enum of type
   * LteRrcSap::SlScaling.
   *
   *  <b>Only the first digit after decimal is considered</b>
   *
   *
   * \param slScaling The sidelink scaling factor
   * \returns The object of type LteRrcSap::SlScaling
   */
  static LteRrcSap::SlScaling GetSlScalingEnum (float slScaling);

  /**
   * \brief Get sidelink subchannel size value
   *
   * This method converts the enum of type LteRrcSap::SlSubchannelSize
   * to its unsigned integer representation.
   *
   * \param subChSize The object of type LteRrcSap::SlSubchannelSize
   * \returns The unsigned integer representation of LteRrcSap::SlSubchannelSize
   */
  static uint16_t GetNrSlSubChSizeValue (const LteRrcSap::SlSubchannelSize &subChSize);

  /**
   * \brief Get sidelink subchannel size enum.
   *
   * This method converts the sidelink subchannel size value to an enum of type
   * LteRrcSap::SlSubchannelSize.
   *
   * \param subChSize The sidelink subchannel size in PRBs
   * \returns The object of type LteRrcSap::SlSubchannelSize
   */
  static LteRrcSap::SlSubchannelSize GetNrSlSubChSizeEnum (uint16_t subChSize);

  /**
   * \brief Get sidelink sensing window value
   *
   * This method converts the enum of type LteRrcSap::SlSensingWindow
   * to its unsigned integer representation, such that, 100 is 100 ms,
   * 1100 is 1100 ms, and so on.
   *
   * \param windowSize The object of type LteRrcSap::SlSensingWindow
   * \returns The unsigned integer representation of LteRrcSap::SlSensingWindow
   */
  static uint16_t GetSlSensWindowValue (const LteRrcSap::SlSensingWindow &windowSize);

  /**
   * \brief Get sidelink sensing window enum.
   *
   * This method converts the sidelink sensing window size value to an enum of type
   * LteRrcSap::SlSensingWindow.
   *
   * \param windowSize The sidelink sensing window size in millisecond
   * \returns The object of type LteRrcSap::SlSensingWindow
   */
  static LteRrcSap::SlSensingWindow GetSlSensWindowEnum (uint16_t windowSize);


  /**
   * \brief Get sidelink selection window value
   *
   * This method converts the enum of type LteRrcSap::SlSelectionWindow
   * to its unsigned integer representation.
   *
   * \param windowSize The object of type LteRrcSap::SlSelectionWindow
   * \returns The unsigned integer representation of LteRrcSap::SlSelectionWindow
   */
  static uint16_t GetSlSelWindowValue (const LteRrcSap::SlSelectionWindow &windowSize);

  /**
   * \brief Get sidelink selection window enum.
   *
   * This method converts the sidelink selection window size value to an enum
   * of type LteRrcSap::SlSelectionWindow.
   *
   * \param windowSize The sidelink sensing window size in number of slots
   * \returns The object of type LteRrcSap::SlSelectionWindow
   */
  static LteRrcSap::SlSelectionWindow GetSlSelWindowEnum (uint16_t windowSize);


  /**
   * \brief Get sidelink resource reserve period value
   *
   * This method converts the enum of type LteRrcSap::SlResourceReservePeriod
   * to its unsigned integer representation, such that, 0 is 0 seconds,
   * 100 is 100 seconds, and so on.
   *
   * \param period The object of type LteRrcSap::SlResourceReservePeriod
   * \returns The unsigned integer representation of LteRrcSap::SlResourceReservePeriod
   */
  static uint16_t GetSlResoResvPrdValue (const LteRrcSap::SlResourceReservePeriod &period);

  /**
   * \brief Get sidelink resource reserve period enum.
   *
   * This method converts the sidelink resource reserve period value to an enum
   * of type LteRrcSap::SlResourceReservePeriod.
   *
   * \param period The sidelink resource reserve period in seconds
   * \returns The object of type LteRrcSap::SlResourceReservePeriod
   */
  static LteRrcSap::SlResourceReservePeriod GetSlResoResvPrdEnum (uint16_t period);

  /**
   * \brief Get sidelink maximum number per reserve value
   *
   * This method converts the enum of type LteRrcSap::SlMaxNumPerReserve
   * to its unsigned integer representation, such the, N2 is 2 slots,
   * N3 is 3 slots, and so on. This value is a maximum number of reserved
   * PSCCH/PSSCH resources that can be indicated by an SCI.
   *
   * \param slMaxReserve The object of type LteRrcSap::SlMaxNumPerReserve
   * \returns The unsigned integer representation of LteRrcSap::SlMaxNumPerReserve
   */
  static uint8_t GetSlMaxNumPerReserveValue (const LteRrcSap::SlMaxNumPerReserve &slMaxReserve);

  /**
   * \brief Get sidelink maximum number per reserve enum
   *
   * This method converts the sidelink maximum number per reserve value to
   * an enum of type LteRrcSap::SlMaxNumPerReserve.
   *
   * \param slMaxReserveInt The sidelink maximum number per reserve value in number of slots
   * \returns The object of type LteRrcSap::SlMaxNumPerReserve
   */
  static LteRrcSap::SlMaxNumPerReserve GetSlMaxNumPerReserveEnum (uint8_t slMaxReserveInt);


  /**
   * \brief SCS-SpecificCarrier information element
   */
  struct ScsSpecificCarrier
  {
    //uint16_t offsetToCarrier {3000}; //!< Offset in frequency domain between Point A and the lowest usable subcarrier on this carrier in number of PRBs (0..275*8-1).
    SubcarrierSpacing subcarrierSpacing; //!< Subcarrier spacing
    uint16_t carrierBandwidth {3000}; //!< carrier bandwidth in number of PRBs. Valid range is [1, 275]
  };

  /**
   * \brief ARFCN-ValueNR information element
   */
  struct ArfcnValueNR
  {
    uint32_t arfcn {4000000}; //!< ARFCN valid range is [0, 3279165] 38.331 sec 6.4
  };

  /**
   * \brief BWP information element
   */
  struct Bwp
  {
    //uint32_t locationAndBandwidth {3000}; //!< Resource Indicator value (RIV). 38.214 sec 5.1.2.2.2
    //SubcarrierSpacing subcarrierSpacing; //!< Subcarrier spacing
    //CyclicPrefix cyclicPrefix; //!< Cyclic prefix  //optional filed
    //Following parameters are not standard compliant. We use them to configure
    //UE in out-of-coverage scenarios
    uint16_t numerology {99}; //!< The numerology
    uint16_t symbolsPerSlots {0}; //!< Total number of symbols per slot
    uint32_t rbPerRbg {0}; //!< Resource block per resource block group
    uint16_t bandwidth {0}; //!< Bandwidth

  };

  /**
   * \brief Sl-Bwp-Generic information element
   */
  struct SlBwpGeneric
  {
    Bwp bwp; //!< BWP information element
    SlLengthSymbols slLengthSymbols; //!< This field indicates the number of symbols used for sidelink in a slot without SL-SSB
    SlStartSymbol slStartSymbol; //!< This field indicates the starting symbol used for sidelink in a slot without SL-SSB
    //slFilterCoefficient; //!< Filter coefficient used for L3 filtering [not supported]
  };

  /**
   * \brief SL-PSCCH-Config information element
   */
  struct SlPscchConfig
  {
    ///setuprelease enumeration
    enum
    {
      RELEASE,
      SETUP,
      INVALID
    } setupRelease {INVALID}; //!< Indicates if it is allocating or releasing resources
    SlTimeResourcePscch slTimeResourcePscch; //!< Indicates the number of symbols of PSCCH in a resource pool.
    SlFreqResourcePscch slFreqResourcePscch; //!< Indicates the number of PRBs for PSCCH in a resource pool where it is not greater than the number PRBs of the subchannel.
    //uint32_t slDmrsScreambleId {70000}; //TODO //!< Indicates the initialization value for PSCCH DMRS scrambling. Valid range [0, 65535]
    //uint16_t slNumReservedBits {0}; //TODO //!< Indicates the number of reserved bits in first stage SCI. Valid range [2, 4]
  };

  /**
   * \brief SL-BetaOffset information element
   */
  struct SlBetaOffsets
  {
    uint16_t offset {32}; //!< Beta-offset.
  };

  /**
   * \brief SL-PSSCH-Config information element
   */
  struct SlPsschConfig
  {
    //sl-PSSCH-DMRS-TimePattern-r16 //!< Indicates the set of PSSCH DMRS time domain patterns that can be used in the resource pool.
    std::array <SlBetaOffsets, 4> slBetaOffsets2ndSci; //!< Indicates candidates of beta-offset values to determine the number of coded modulation symbols for second stage SCI.
    SlScaling slScaling; //!< Indicates a scaling factor to limit the number of resource elements assigned to the second stage SCI on PSSCH
  };

  /**
   * \brief SL-UE-SelectedConfigRP information element
   */
  struct SlUeSelectedConfigRp
  {
    SlSensingWindow slSensingWindow; //!< Parameter that indicates the start of the sensing window.
    SlSelectionWindow slSelectionWindow; //!< Parameter that determines the end of the selection window in the resource selection for a TB with respect to priority indicated in SCI.
    bool slMultiReserveResource {false}; //!< Flag to enable the reservation of an initial transmission of a TB by an SCI associated with a different TB.
    std::array <SlResourceReservePeriod, 16> slResourceReservePeriodList; //!< Set of possible resource reservation period allowed in the resource pool.
    SlMaxNumPerReserve slMaxNumPerReserve; //!< Indicates the maximum number of reserved PSCCH/PSSCH resources that can be indicated by an SCI.

    //sl-CBR-Priority-TxConfigList-r16 //TODO
    //We probably going to simplify the RSRP threshold
    //by introducing an attribute directly in UE PHY class.
    //sl-ThresPSSCH-RSRP-List-r16 //TODO //!< Indicates a threshold used for sensing based UE autonomous resource selection.

    //sl-MultiReserveResource-r16 //TODO //!< Flag to enable the reservation of an initial transmission of a TB by an SCI associated with a different TB.
    //sl-ResourceReservePeriodList-r16 //TODO //!< Set of possible resource reservation period allowed in the resource pool.

     //since we do not support DMRS signals
     //we will rely on the PSCCH RSRP and SCI message
     //decodefication.
    //sl-RS-ForSensing-r16 //TODO //!< Flag to enable the use of DMRS of PSCCH or PSSCH for L1 RSRP measurement in the sensing operation.
  };

  /**
   * \brief SL-ResourcePool information element
   */
  struct SlResourcePoolNr
  {
    SlPscchConfig slPscchConfig; //!< SL-PSCCH field
    SlSubchannelSize slSubchannelSize; //!< Sidelink subchannel size in PRBs
    SlUeSelectedConfigRp slUeSelectedConfigRp; //!< SL-UE-SelectedConfigRP
    std::vector <std::bitset<1>> slTimeResource; //!< Sidelink bitmap
    //uint16_t slStartRbSubchannel {3000}; //!< First RB of a sidelink subchannel. Valid range [0, 265]
    //uint16_t slNumSubchannel {3000}; //!< Number of subchannels. Valid range [1, 27]
    //SlMcsTable slMcsTable; //It is configurable via error model attribute //!< Indicates the MCS table used for the resource pool.
    //SlPsschConfig slPsschHConfig; //!< SL-PSSCH field
    //sl-PSFCH-Config-r16 //TODO
    //sl-SyncAllowed-r16 //TODO
    //sl-Period-r16 [FFS]
    //sl-ThreshS-RSSI-CBR-r16 //TODO
    //sl-TimeWindowSizeCBR-r16 //TODO
    //sl-TimeWindowSizeCR-r16 //TODO
    //SL-PTRS-Config //TODO
    //SL-ConfiguredGrantConfigList //TODO
    //sl-RxParametersNcell-r16 //TODO
  };

  /**
   * \brief SL-ResourcePoolID information element
   */
  struct SlResourcePoolIdNr
  {
    uint16_t id {333}; //!< Sidelink pool id. Valid range [1, 16] by standard. In the simulator it is from [0, 15]
  };

  /**
   * \brief SL-ResourcePoolConfig information element
   */
  struct SlResourcePoolConfigNr
  {
    bool haveSlResourcePoolConfigNr {false}; //!< true if the fields in this IE are set, false otherwise
    SlResourcePoolIdNr slResourcePoolId; //!< Sidelink pool id
    SlResourcePoolNr slResourcePool; //!< Sidelink resource pool
  };

  /**
   * \brief SL-BWP-PoolConfigCommon information element
   */
  struct SlBwpPoolConfigCommonNr
  {
    //std::array <SlResourcePoolNr, MAX_NUM_OF_RX_POOL> slRxPool; //!< List of sidelink resource pools for RX
    std::array <SlResourcePoolConfigNr, MAX_NUM_OF_TX_POOL> slTxPoolSelectedNormal; //!< List of sidelink resource pools for TX
    //sl-TxPoolExceptional-r16 //TODO //!< Indicates the resources by which the UE is allowed to transmit NR sidelink communication in exceptional conditions on the configured BWP.
  };

  /**
   * \brief SL-BWP-ConfigCommon information element
   */
  struct SlBwpConfigCommonNr
  {
    bool haveSlBwpGeneric {false}; //!< true if slBwpGeneric is set, false otherwise.
    SlBwpGeneric slBwpGeneric; //!< This field indicates the generic parameters on the configured sidelink BWP
    bool haveSlBwpPoolConfigCommonNr {false}; //!< true if slBwpPoolConfigCommonNr is set, false otherwise.
    SlBwpPoolConfigCommonNr slBwpPoolConfigCommonNr; //!< This field indicates the resource pool configurations on the configured sidelink BWP
  };

  /**
   * \brief SL-FreqConfigCommon information element
   */
  struct SlFreqConfigCommonNr
  {
    //std::array <ScsSpecificCarrier, MAX_SCSs> slScsSpecificCarrierList; //!< A list per numerology for UE specific channel bandwidth and location configurations
    std::array <SlBwpConfigCommonNr, MAX_NUM_OF_SL_BWPs> slBwpList; //!< List of sidelink BWP(s) for NR sidelink communication
    //ArfcnValueNR slAbsoluteFrequencyPointA; //!< Absolute frequency of the reference resource block (Common RB 0).
    //ArfcnValueNR slAbsoluteFrequencySSB; //!< Frequency location of sidelink Synchronization Signal/PBCH block (SSB)
    //bool frequencyShift7p5khzSL {false}; //!< Enable the NR SL transmission with a 7.5 kHz shift to the LTE raster [we don't use this]
    //int valueN {2}; //!< Indicate the NR SL transmission with a valueN * 5kHz shift to the LTE raster. Valid range : [-1,1]
    //sl-SyncPriority-r16 //TODO //!< This field indicates synchronization priority order, as specified in sub-clause TS 38.331 5.X.6
    //bool sl-NbAsSync-r16 //TODO //!< If True, an out of coverage / mode 2 enabled UE is allowed to use an available network as sync source. Only present in SL-PreconfigurationNR
    //sl-SyncConfigList //TODO //!< List of configurations for Tx/RX synch information for NR SL communication.
    //sl-PowerControl //TODO //!< Sidelink power control related configuration
  };

  /**
   * \brief TDD-UL-DL-ConfigCommon
   *
   * This IE is not implemented as per the 3GPP standard
   * since in the simulator we support our own customized
   * TDD pattern.
   *
   */
  struct TddUlDlConfigCommon
  {
    std::string tddPattern {"F|F|F|F|F|F|F|F|F|F|"}; //!< TDD pattern

    //SubcarrierSpacing referenceSubcarrierSpacing //TODO
    //TDD-UL-DL-Pattern pattern1 //TODO
  };

  /**
   * \brief SL-PreconfigGeneral information element
   */
  struct SlPreconfigGeneralNr
  {
    TddUlDlConfigCommon slTddConfig; //!< TDD pattern
    //BIT STRING (SIZE (2)) reservedBits-r16 //TODO
  };

  /**
   * \brief SL-PSSCH-TxParameters information element
   */
  struct SlPsschTxParameters
  {
    uint8_t slMaxTxTransNumPssch; /**< Indicates the maximum transmission number
                                       (including new transmission and
                                       retransmission) for PSSCH.
                                       */
    //sl-MinMCS-PSSCH-r16  INTEGER (0..27); /TODO
    //sl-MaxMCS-PSSCH-r16  INTEGER (0..31); /TODO
    //sl-MinSubChannelNumPSSCH-r16 INTEGER (1..27); /TODO
    //sl-MaxSubchannelNumPSSCH-r16 INTEGER (1..27); /TODO
    //sl-MaxTxPower-r16 SL-TxPower-r16; /TODO
  };

  /**
   * \brief SL-PSSCH-TxConfigList information element
   */
  struct SlPsschTxConfigList
  {
    std::array <SlPsschTxParameters, 1> slPsschTxParameters; //!< List of SlPsschTxParameters
  };

  /**
   * \brief SL-UE-SelectedConfig-r16
   */
  struct SlUeSelectedConfig
  {
    double slProbResourceKeep {0.0}; /**< Indicates the probability with which
                                         the UE keeps the current resource when
                                         the resource reselection counter reaches
                                         zero for sensing based UE autonomous
                                         resource selection (see TS 38.321).
                                         Standard values for this parameter are
                                         0, 0.2, 0.4, 0.6, 0.8, however, in the
                                         simulator we are not restricting it to
                                         evaluate other values.
                                         */

    SlPsschTxConfigList slPsschTxConfigList; //!< Indicates PSSCH TX parameters

    //slPrioritizationThres; //TODO
    //SlReselAfter; //TODO
    //UlPrioritizationThres; //TODO
  };

  /**
   * \brief SL-PreconfigNR information element
   */
  struct SidelinkPreconfigNr
  {
    std::array <SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL> slPreconfigFreqInfoList; //!< List containing per carrier configuration for NR sidelink communication
    SlPreconfigGeneralNr slPreconfigGeneral; //!< SlPreconfigGeneralNr
    SlUeSelectedConfig slUeSelectedPreConfig; //!< SlUeSelectedConfig
    /*
    sl-PreconfigNR-AnchorCarrierFreqList-r16
    sl-PreconfigEUTRA-AnchorCarrierFreqList-r16
    sl-RadioBearerPreConfigList-r16
    sl-RLC-BearerPreConfigList-r16
    sl-MeasPreConfig-r16
    sl-OffsetDFN-r16
    t400-r16
    sl-SSB-PriorityNR-r16
    sl-UE-SelectedPreConfig-r16
    */
  };

  /**
   * \brief SL-PreconfigurationNR information element
   *
   * Even though the NR 38.331 specify the following
   * structure, we do not see the use of it. We
   * should use directly the SidelinkPreconfigNr
   * structure.
   */
 // struct SlPreconfigurationNr
 // {
 //   SidelinkPreconfigNr sidelinkPreconfigNr;
 // };


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
   * \brief Send UE context remove request function
   *
   * Request eNodeB to remove UE context once radio link failure or
   * random access failure is detected. It is needed since no RLF
   * detection mechanism at eNodeB is implemented.
   *
   * \param rnti the C-RNTI of the UE
   */
   virtual void SendIdealUeContextRemoveRequest (uint16_t rnti) = 0;

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
   * \brief Receive ideal UE context remove request from the UE RRC.
   *
   * Receive the notification from UE to remove the UE context
   * once radio link failure or random access failure is detected.
   * It is needed since no RLF detection mechanism at eNodeB is implemented.
   *
   * \param rnti the C-RNTI of the UE
   */
  virtual void RecvIdealUeContextRemoveRequest (uint16_t rnti) = 0;

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
  virtual void SendIdealUeContextRemoveRequest (uint16_t rnti);

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
MemberLteUeRrcSapUser<C>::SendIdealUeContextRemoveRequest (uint16_t rnti)
{
  m_owner->DoSendIdealUeContextRemoveRequest (rnti);
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
  virtual void RecvIdealUeContextRemoveRequest (uint16_t rnti);

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
void MemberLteEnbRrcSapProvider<C>::RecvIdealUeContextRemoveRequest (uint16_t rnti)
{
  Simulator::ScheduleNow (&C::DoRecvIdealUeContextRemoveRequest, m_owner, rnti);
}














} // namespace ns3


#endif // LTE_RRC_SAP_H




