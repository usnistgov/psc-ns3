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
 * Modified by: NIST
 */


#ifndef NIST_LTE_RRC_SAP_H
#define NIST_LTE_RRC_SAP_H

#include <stdint.h>
#include <list>
#include <bitset>

#include <ns3/ptr.h>
#include <ns3/simulator.h>

namespace ns3 {

class NistLteRlcSapUser;
class NistLtePdcpSapUser;
class NistLteRlcSapProvider;
class NistLtePdcpSapProvider;
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
class NistLteRrcSap
{
public:
  virtual ~NistLteRrcSap ();

  // Constraint values

  static const uint8_t MaxReportCells = 255;

  // Information Elements

  struct NistPlmnIdentityInfo
  {
    uint32_t plmnIdentity;
  };

  struct NistCellAccessRelatedInfo
  {
    NistPlmnIdentityInfo plmnIdentityInfo;
    uint32_t cellIdentity;
    bool csgIndication;
    uint32_t csgIdentity;
  };

  struct NistCellSelectionInfo
  {
    int8_t qRxLevMin; ///< INTEGER (-70..-22), actual value = IE value * 2 [dBm].
    int8_t qQualMin; ///< INTEGER (-34..-3), actual value = IE value [dB].
  };

  struct NistFreqInfo
  {
    uint16_t ulCarrierFreq;
    uint8_t ulBandwidth;
  };

  struct NistRlcConfig
  {
    enum
    {
      AM,
      UM_BI_DIRECTIONAL,
      UM_UNI_DIRECTIONAL_UL,
      UM_UNI_DIRECTIONAL_DL
    } choice;
  };

  struct NistLogicalChannelConfig
  {
    uint8_t priority;
    uint16_t prioritizedBitRateKbps;
    uint16_t bucketSizeDurationMs;
    uint8_t logicalChannelGroup;
  };

  struct NistSoundingRsUlConfigCommon
  {
    enum
    {
      SETUP, RESET
    } type;
    uint8_t srsBandwidthConfig;
    uint8_t srsSubframeConfig;
  };

  struct NistSoundingRsUlConfigDedicated
  {
    enum
    {
      SETUP, RESET
    } type;
    uint8_t srsBandwidth;
    uint16_t srsConfigIndex;
  };

  struct NistAntennaInfoDedicated
  {
    uint8_t transmissionMode;
  };

  struct NistPdschConfigCommon
  {
	int8_t referenceSignalPower;  // INTEGER (-60..50),
    int8_t pb;                    // INTEGER (0..3),
  };

  struct NistPdschConfigDedicated
  {
    /*
     * P_A values, TS 36.331 6.3.2 PDSCH-Config
     * ENUMERATED { dB-6, dB-4dot77, dB-3, dB-1dot77, dB0, dB1, dB2, dB3 }
     */
    enum
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
    uint8_t pa;
  };

  static double ConvertNistPdschConfigDedicated2Double (NistPdschConfigDedicated pdschConfigDedicated)
  {
    double pa = 0;
    switch (pdschConfigDedicated.pa)
      {
      case NistPdschConfigDedicated::dB_6:
        pa = -6;
        break;
      case NistPdschConfigDedicated::dB_4dot77:
        pa = -4.77;
        break;
      case NistPdschConfigDedicated::dB_3:
        pa = -3;
        break;
      case NistPdschConfigDedicated::dB_1dot77:
        pa = -1.77;
        break;
      case NistPdschConfigDedicated::dB0:
        pa = 0;
        break;
      case NistPdschConfigDedicated::dB1:
        pa = 1;
        break;
      case NistPdschConfigDedicated::dB2:
        pa = 2;
        break;
      case NistPdschConfigDedicated::dB3:
        pa = 3;
        break;
      default:
        break;
      }
    return pa;
  }

  struct NistPhysicalConfigDedicated
  {
    bool haveNistSoundingRsUlConfigDedicated;
    NistSoundingRsUlConfigDedicated soundingRsUlConfigDedicated;
    bool haveNistAntennaInfoDedicated;
    NistAntennaInfoDedicated antennaInfo;
    bool haveNistPdschConfigDedicated;
    NistPdschConfigDedicated pdschConfigDedicated;
  };


  struct NistSrbToAddMod
  {
    uint8_t srbIdentity;
    NistLogicalChannelConfig logicalChannelConfig;
  };

  struct NistDrbToAddMod
  {
    uint8_t epsBearerIdentity;
    uint8_t drbIdentity;
    NistRlcConfig rlcConfig;
    uint8_t logicalChannelIdentity;
    NistLogicalChannelConfig logicalChannelConfig;
  };

  struct NistPreambleInfo
  {
    uint8_t numberOfRaPreambles;
  };

  struct NistRaSupervisionInfo
  {
    uint8_t preambleTransMax;
    uint8_t raResponseWindowSize;
  };

  struct NistRachConfigCommon
  {
    NistPreambleInfo preambleInfo;
    NistRaSupervisionInfo raSupervisionInfo;
  };

  struct NistRadioResourceConfigCommon
  {
    NistRachConfigCommon rachConfigCommon;
  };

  struct NistRadioResourceConfigCommonSib
  {
    NistRachConfigCommon rachConfigCommon;
    NistPdschConfigCommon pdschConfigCommon;
  };

  struct NistRadioResourceConfigDedicated
  {
    std::list<NistSrbToAddMod> srbToAddModList;
    std::list<NistDrbToAddMod> drbToAddModList;
    std::list<uint8_t> drbToReleaseList;
    bool haveNistPhysicalConfigDedicated;
    NistPhysicalConfigDedicated physicalConfigDedicated;
  };

  struct NistQuantityConfig
  {
    uint8_t filterCoefficientRSRP;
    uint8_t filterCoefficientRSRQ;
  };

  struct NistCellsToAddMod
  {
    uint8_t cellIndex;
    uint16_t physCellId;
    int8_t cellIndividualOffset;
  };

  struct NistPhysCellIdRange
  {
    uint16_t start;
    bool haveRange;
    uint16_t range;
  };

  struct BlackNistCellsToAddMod
  {
    uint8_t cellIndex;
    NistPhysCellIdRange physCellIdRange;
  };

  struct NistMeasObjectEutra
  {
    uint16_t carrierFreq;
    uint8_t allowedMeasBandwidth;
    bool presenceAntennaPort1;
    uint8_t neighCellConfig;
    int8_t offsetFreq;
    std::list<uint8_t> cellsToRemoveList;
    std::list<NistCellsToAddMod> cellsToAddModList;
    std::list<uint8_t> blackCellsToRemoveList;
    std::list<BlackNistCellsToAddMod> blackNistCellsToAddModList;
    bool haveCellForWhichToReportCGI;
    uint8_t cellForWhichToReportCGI;
  };

  /**
   * \brief Threshold for event evaluation.
   *
   * For RSRP-based threshold, the actual value is (value - 140) dBm. While for
   * RSRQ-based threshold, the actual value is (value - 40) / 2 dB. This is in
   * accordance with section 9.1.4 and 9.1.7 of 3GPP TS 36.133.
   *
   * \sa ns3::NistEutranMeasurementMapping
   */
  struct NistThresholdEutra
  {
    enum
    {
      THRESHOLD_RSRP, ///< RSRP is used for the threshold.
      THRESHOLD_RSRQ ///< RSRQ is used for the threshold.
    } choice;
    uint8_t range; ///< Value range used in RSRP/RSRQ threshold.
  };

  /// Specifies criteria for triggering of an E-UTRA measurement reporting event.
  struct NistReportConfigEutra
  {
    enum
    {
      EVENT,
      PERIODICAL
    } triggerType;

    enum
    {
      EVENT_A1, ///< Event A1: Serving becomes better than absolute threshold.
      EVENT_A2, ///< Event A2: Serving becomes worse than absolute threshold.
      EVENT_A3, ///< Event A3: Neighbour becomes amount of offset better than PCell.
      EVENT_A4, ///< Event A4: Neighbour becomes better than absolute threshold.
      EVENT_A5 ///< Event A5: PCell becomes worse than absolute `threshold1` AND Neighbour becomes better than another absolute `threshold2`.

    } eventId; ///< Choice of E-UTRA event triggered reporting criteria.

    NistThresholdEutra threshold1; ///< Threshold for event A1, A2, A4, and A5.
    NistThresholdEutra threshold2; ///< Threshold for event A5.

    /// Indicates whether or not the UE shall initiate the measurement reporting procedure when the leaving condition is met for a cell in `cellsTriggeredList`, as specified in 5.5.4.1 of 3GPP TS 36.331.
    bool reportOnLeave;

    /// Offset value for Event A3. An integer between -30 and 30. The actual value is (value * 0.5) dB.
    int8_t a3Offset;

    /// Parameter used within the entry and leave condition of an event triggered reporting condition. The actual value is (value * 0.5) dB.
    uint8_t hysteresis;

    /// Time during which specific criteria for the event needs to be met in order to trigger a measurement report.
    uint16_t timeToTrigger;

    enum
    {
      REPORT_STRONGEST_CELLS,
      REPORT_CGI
    } purpose;

    enum
    {
      RSRP, ///< Reference Signal Received Power
      RSRQ ///< Reference Signal Received Quality
    } triggerQuantity; ///< The quantities used to evaluate the triggering condition for the event, see 3GPP TS 36.214.

    enum
    {
      SAME_AS_TRIGGER_QUANTITY,
      BOTH ///< Both the RSRP and RSRQ quantities are to be included in the measurement report.
    } reportQuantity; ///< The quantities to be included in the measurement report, always assumed to be BOTH.

    /// Maximum number of cells, excluding the serving cell, to be included in the measurement report.
    uint8_t maxReportCells;

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

    NistReportConfigEutra ();

  }; // end of struct NistReportConfigEutra

  struct NistMeasObjectToAddMod
  {
    uint8_t measObjectId;
    NistMeasObjectEutra measObjectEutra;
  };

  struct NistReportConfigToAddMod
  {
    uint8_t reportConfigId;
    NistReportConfigEutra reportConfigEutra;
  };

  struct NistMeasIdToAddMod
  {
    uint8_t measId;
    uint8_t measObjectId;
    uint8_t reportConfigId;
  };

  struct NistMeasGapConfig
  {
    enum
    {
      SETUP, RESET
    } type;
    enum
    {
      GP0, GP1
    } gapOffsetChoice;
    uint8_t gapOffsetValue;
  };

  struct NistMobilityStateParameters
  {
    uint8_t tEvaluation;
    uint8_t tHystNormal;
    uint8_t nCellChangeMedium;
    uint8_t nCellChangeHigh;
  };

  struct NistSpeedStateScaleFactors
  {
    // 25 = oDot25, 50 = oDot5, 75 = oDot75, 100 = lDot0
    uint8_t sfMedium;
    uint8_t sfHigh;
  };

  struct NistSpeedStatePars
  {
    enum
    {
      SETUP,
      RESET
    } type;
    NistMobilityStateParameters mobilityStateParameters;
    NistSpeedStateScaleFactors timeToTriggerSf;
  };

  struct NistMeasConfig
  {
    std::list<uint8_t> measObjectToRemoveList;
    std::list<NistMeasObjectToAddMod> measObjectToAddModList;
    std::list<uint8_t> reportConfigToRemoveList;
    std::list<NistReportConfigToAddMod> reportConfigToAddModList;
    std::list<uint8_t> measIdToRemoveList;
    std::list<NistMeasIdToAddMod> measIdToAddModList;
    bool haveNistQuantityConfig;
    NistQuantityConfig quantityConfig;
    bool haveNistMeasGapConfig;
    NistMeasGapConfig measGapConfig;
    bool haveSmeasure;
    uint8_t sMeasure;
    bool haveNistSpeedStatePars;
    NistSpeedStatePars speedStatePars;
  };

  struct NistCarrierFreqEutra
  {
    uint16_t dlCarrierFreq;
    uint16_t ulCarrierFreq;
  };

  struct NistCarrierBandwidthEutra
  {
    uint8_t dlBandwidth;
    uint8_t ulBandwidth;
  };

  struct NistRachConfigDedicated
  {
    uint8_t raPreambleIndex;
    uint8_t raPrachMaskIndex;
  };

  struct NistMobilityControlInfo
  {
    uint16_t targetPhysCellId;
    bool haveCarrierFreq;
    NistCarrierFreqEutra carrierFreq;
    bool haveCarrierBandwidth;
    NistCarrierBandwidthEutra carrierBandwidth;
    uint16_t newUeIdentity;
    NistRadioResourceConfigCommon radioResourceConfigCommon;
    bool haveNistRachConfigDedicated;
    NistRachConfigDedicated rachConfigDedicated;
  };

  struct NistReestabUeIdentity
  {
    uint16_t cRnti;
    uint16_t physCellId;
  };

  enum ReestablishmentCause
  {
    RECONFIGURATION_FAILURE,
    HANDOVER_FAILURE,
    OTHER_FAILURE
  };

  struct NistMasterInformationBlock
  {
    uint8_t dlBandwidth;
    uint8_t systemFrameNumber;
  };

  struct NistSystemInformationBlockType1
  {
    NistCellAccessRelatedInfo cellAccessRelatedInfo;
    NistCellSelectionInfo cellSelectionInfo;
  };

  struct NistSystemInformationBlockType2
  {
    NistRadioResourceConfigCommonSib radioResourceConfigCommon;
    NistFreqInfo freqInfo;
  };

    /* NIST: Information Elements for SystemInformationBlockType18 */
#define MAXSL_DEST 16
#define MAXSL_TF_INDEXPAIR 64
#define MAXSL_TXPOOL 4
#define MAXSL_RXPOOL 16
#define MAXSL_DEST 16

#define MAXSL_DISC_POWERCLASS 3

#define MAX_PLMN 6
  
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
    uint8_t hoppingParameters; //valid range 0..504
    enum {
      ns1,
      ns2,
      ns4,
    } numSubbands;
    uint8_t rbOffset; //valid range 0..110

    friend bool operator==(const SlHoppingConfigComm& lhs, const SlHoppingConfigComm& rhs)
    {
      return lhs.numSubbands == rhs.numSubbands && lhs.rbOffset == rhs.rbOffset;
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
    struct {
      SlTfResourceConfig dataTfResourceConfig;
      bool haveTrptSubset;
      SlTrptSubset trptSubset; //optional
    } ueSelectedResourceConfig;
    //rxParametersNCell not specified yet
    bool haveTxParameters; //mandatory present when included in commTxPoolNormalDedicated, commTxPoolNormalCommon or commTxPoolExceptional
    struct {
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
    struct {
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
  
  
  /* end SystemInformationBlockType18 */


 ///////////////* Start SystemInformationBlockType19 *///////////////

  //Discovery period and probability of transmission are defined above 
  //in order to set the preconfigured pools
 
  struct PoolSelection {
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
    struct {
      SlTxParameters txParametersGeneral;
      struct {
        PoolSelection poolSelection;
        bool havePoolSelectionRsrpBased;
        PoolSelectionRsrpBased poolSelectionRsrpBased; 
        TxProbability txProbability; 
      } ueSelectedResourceConfig;
    } txParameters;
    bool haveRxParameters;
    struct {
      struct {
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
  
  struct PlmnIdendityList
  {
    int nbPlmn;
    NistPlmnIdentityInfo plmnIdentityInfo[MAX_PLMN]; 
  };

  struct SlCarrierFreqInfoList
  {
    uint16_t carrierFreq;
    PlmnIdendityList plmnIdentityList;
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

  ///////////////* End SystemInformationBlockType19 *///////////////


  struct NistSystemInformation
  {
    bool haveSib2;
    NistSystemInformationBlockType2 sib2;
    bool haveSib18; //NIST: add SIB 18
    SystemInformationBlockType18 sib18; //NIST: add SIB 18
    bool haveSib19; // add SIB 19
    SystemInformationBlockType19 sib19;
  };

  struct NistAsConfig
  {
    NistMeasConfig sourceNistMeasConfig;
    NistRadioResourceConfigDedicated sourceRadioResourceConfig;
    uint16_t sourceUeIdentity;
    NistMasterInformationBlock sourceNistMasterInformationBlock;
    NistSystemInformationBlockType1 sourceNistSystemInformationBlockType1;
    NistSystemInformationBlockType2 sourceNistSystemInformationBlockType2;
    uint16_t sourceDlCarrierFreq;
  };

  struct NistCgiInfo
  {
    uint32_t plmnIdentity;
    uint32_t cellIdentity;
    uint16_t trackingAreaCode;
    std::list<uint32_t> plmnIdentityList;
  };

  struct NistMeasResultEutra
  {
    uint16_t physCellId;
    bool haveNistCgiInfo;
    NistCgiInfo cgiInfo;
    bool haveRsrpResult;
    uint8_t rsrpResult;
    bool haveRsrqResult;
    uint8_t rsrqResult;
  };

  struct NistMeasResults
  {
    uint8_t measId;
    uint8_t rsrpResult;
    uint8_t rsrqResult;
    bool haveMeasResultNeighCells;
    std::list<NistMeasResultEutra> measResultListEutra;
  };
  
  // Messages

  struct NistRrcConnectionRequest
  {
    uint64_t ueIdentity;
  };

  struct NistRrcConnectionSetup
  {
    uint8_t rrcTransactionIdentifier;
    NistRadioResourceConfigDedicated radioResourceConfigDedicated;
  };

  struct NistRrcConnectionSetupCompleted
  {
    uint8_t rrcTransactionIdentifier;
  };

  struct NistRrcConnectionReconfiguration
  {
    uint8_t rrcTransactionIdentifier;
    bool haveNistMeasConfig;
    NistMeasConfig measConfig;
    bool haveNistMobilityControlInfo;
    NistMobilityControlInfo mobilityControlInfo;
    bool haveNistRadioResourceConfigDedicated;
    NistRadioResourceConfigDedicated radioResourceConfigDedicated;
    bool haveSlCommConfig; //presence of dedicated sidelink communication configuration
    SlCommConfig slCommConfig;    
    bool haveSlDiscConfig; // sidelink discovery configuration
    SlDiscConfig slDiscConfig;
  };

  struct NistRrcConnectionReconfigurationCompleted
  {
    uint8_t rrcTransactionIdentifier;
  };


  struct NistRrcConnectionReestablishmentRequest
  {
    NistReestabUeIdentity ueIdentity;
    ReestablishmentCause reestablishmentCause;
  };

  struct NistRrcConnectionReestablishment
  {
    uint8_t rrcTransactionIdentifier;
    NistRadioResourceConfigDedicated radioResourceConfigDedicated;
  };

  struct NistRrcConnectionReestablishmentComplete
  {
    uint8_t rrcTransactionIdentifier;
  };

  struct NistRrcConnectionReestablishmentReject
  {
  };

  struct NistRrcConnectionRelease
  {
    uint8_t rrcTransactionIdentifier;
  };

  struct NistRrcConnectionReject
  {
    uint8_t waitTime;
  };

  struct NistHandoverPreparationInfo
  {
    NistAsConfig asConfig;
  };

  struct NistMeasurementReport
  {
    NistMeasResults measResults;
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
class NistLteUeRrcSapUser : public NistLteRrcSap
{
public:
  struct NistSetupParameters
  {
    NistLteRlcSapProvider* srb0SapProvider;
    NistLtePdcpSapProvider* srb1SapProvider;
  };

  virtual void Setup (NistSetupParameters params) = 0;

  /**
   * \brief Send an _RRCConnectionRequest message to the serving eNodeB
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param msg the message
   */
  virtual void SendNistRrcConnectionRequest (NistRrcConnectionRequest msg) = 0;

  /**
   * \brief Send an _RRCConnectionSetupComplete_ message to the serving eNodeB
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param msg the message
   */
  virtual void SendNistRrcConnectionSetupCompleted (NistRrcConnectionSetupCompleted msg) = 0;

  /**
   * \brief Send an _RRCConnectionReconfigurationComplete_ message to the serving eNodeB
   *        during an RRC connection reconfiguration procedure
   *        (Section 5.3.5 of TS 36.331).
   * \param msg the message
   */
  virtual void SendNistRrcConnectionReconfigurationCompleted (NistRrcConnectionReconfigurationCompleted msg) = 0;

  /**
   * \brief Send an _RRCConnectionReestablishmentRequest_ message to the serving eNodeB
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param msg the message
   */
  virtual void SendNistRrcConnectionReestablishmentRequest (NistRrcConnectionReestablishmentRequest msg) = 0;

  /**
   * \brief Send an _RRCConnectionReestablishmentComplete_ message to the serving eNodeB
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param msg the message
   */
  virtual void SendNistRrcConnectionReestablishmentComplete (NistRrcConnectionReestablishmentComplete msg) = 0;

  /**
   * \brief Send a _NistMeasurementReport_ message to the serving eNodeB
   *        during a measurement reporting procedure
   *        (Section 5.5.5 of TS 36.331).
   * \param msg the message
   */
  virtual void SendNistMeasurementReport (NistMeasurementReport msg) = 0;

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
class NistLteUeRrcSapProvider : public NistLteRrcSap
{
public:
  struct CompleteNistSetupParameters
  {
    NistLteRlcSapUser* srb0SapUser;
    NistLtePdcpSapUser* srb1SapUser;
  };

  virtual void CompleteSetup (CompleteNistSetupParameters params) = 0;

  /**
   * \brief Receive a _NistSystemInformation_ message from the serving eNodeB
   *        during a system information acquisition procedure
   *        (Section 5.2.2 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvNistSystemInformation (NistSystemInformation msg) = 0;

  /**
   * \brief Receive an _RRCConnectionSetup_ message from the serving eNodeB
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvNistRrcConnectionSetup (NistRrcConnectionSetup msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReconfiguration_ message from the serving eNodeB
   *        during an RRC connection reconfiguration procedure
   *        (Section 5.3.5 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvNistRrcConnectionReconfiguration (NistRrcConnectionReconfiguration msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReestablishment_ message from the serving eNodeB
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvNistRrcConnectionReestablishment (NistRrcConnectionReestablishment msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReestablishmentReject_ message from the serving eNodeB
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvNistRrcConnectionReestablishmentReject (NistRrcConnectionReestablishmentReject msg) = 0;

  /**
   * \brief Receive an _RRCConnectionRelease_ message from the serving eNodeB
   *        during an RRC connection release procedure
   *        (Section 5.3.8 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvNistRrcConnectionRelease (NistRrcConnectionRelease msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReject_ message from the serving eNodeB
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param msg the message
   */
  virtual void RecvNistRrcConnectionReject (NistRrcConnectionReject msg) = 0;

};


/**
 * \brief Part of the RRC protocol. This Service Access Point (SAP) is used by
 *        the eNB RRC to send messages to the UE RRC.  Each method defined in
 *        this class corresponds to the transmission of a message that is
 *        defined in Section 6.2.2 of TS 36.331.
 */
class NistLteEnbRrcSapUser : public NistLteRrcSap
{
public:
  struct NistSetupUeParameters
  {
    NistLteRlcSapProvider* srb0SapProvider;
    NistLtePdcpSapProvider* srb1SapProvider;
  };

  virtual void SetupUe (uint16_t rnti, NistSetupUeParameters params) = 0;
  virtual void RemoveUe (uint16_t rnti) = 0;

  /**
   * \brief Send a _NistSystemInformation_ message to all attached UEs
   *        during a system information acquisition procedure
   *        (Section 5.2.2 of TS 36.331).
   * \param msg the message
   */
  virtual void SendNistSystemInformation (NistSystemInformation msg) = 0;

  /**
   * \brief Send an _RRCConnectionSetup_ message to a UE
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendNistRrcConnectionSetup (uint16_t rnti, NistRrcConnectionSetup msg) = 0;

  /**
   * \brief Send an _RRCConnectionReconfiguration_ message to a UE
   *        during an RRC connection reconfiguration procedure
   *        (Section 5.3.5 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendNistRrcConnectionReconfiguration (uint16_t rnti, NistRrcConnectionReconfiguration msg) = 0;

  /**
   * \brief Send an _RRCConnectionReestablishment_ message to a UE
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendNistRrcConnectionReestablishment (uint16_t rnti, NistRrcConnectionReestablishment msg) = 0;

  /**
   * \brief Send an _RRCConnectionReestablishmentReject_ message to a UE
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendNistRrcConnectionReestablishmentReject (uint16_t rnti, NistRrcConnectionReestablishmentReject msg) = 0;

  /**
   * \brief Send an _RRCConnectionRelease_ message to a UE
   *        during an RRC connection release procedure
   *        (Section 5.3.8 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendNistRrcConnectionRelease (uint16_t rnti, NistRrcConnectionRelease msg) = 0;

  /**
   * \brief Send an _RRCConnectionReject_ message to a UE
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param rnti the RNTI of the destination UE
   * \param msg the message
   */
  virtual void SendNistRrcConnectionReject (uint16_t rnti, NistRrcConnectionReject msg) = 0;

  virtual Ptr<Packet> EncodeNistHandoverPreparationInformation (NistHandoverPreparationInfo msg) = 0;
  virtual NistHandoverPreparationInfo DecodeNistHandoverPreparationInformation (Ptr<Packet> p) = 0;
  virtual Ptr<Packet> EncodeHandoverCommand (NistRrcConnectionReconfiguration msg) = 0;
  virtual NistRrcConnectionReconfiguration DecodeHandoverCommand (Ptr<Packet> p) = 0;

};


/**
 * \brief Part of the RRC protocol. This Service Access Point (SAP) is used to
 *        let the eNB RRC receive a message from a UE RRC.  Each method defined
 *        in this class corresponds to the reception of a message that is
 *        defined in Section 6.2.2 of TS 36.331.
 */
class NistLteEnbRrcSapProvider : public NistLteRrcSap
{
public:
  struct CompleteNistSetupUeParameters
  {
    NistLteRlcSapUser* srb0SapUser;
    NistLtePdcpSapUser* srb1SapUser;
  };

  virtual void CompleteSetupUe (uint16_t rnti, CompleteNistSetupUeParameters params) = 0;

  /**
   * \brief Receive an _RRCConnectionRequest_ message from a UE
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvNistRrcConnectionRequest (uint16_t rnti,
                                         NistRrcConnectionRequest msg) = 0;

  /**
   * \brief Receive an _RRCConnectionSetupComplete_ message from a UE
   *        during an RRC connection establishment procedure
   *        (Section 5.3.3 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvNistRrcConnectionSetupCompleted (uint16_t rnti,
                                                NistRrcConnectionSetupCompleted msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReconfigurationComplete_ message from a UE
   *        during an RRC connection reconfiguration procedure
   *        (Section 5.3.5 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvNistRrcConnectionReconfigurationCompleted (uint16_t rnti,
                                                          NistRrcConnectionReconfigurationCompleted msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReestablishmentRequest_ message from a UE
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvNistRrcConnectionReestablishmentRequest (uint16_t rnti,
                                                        NistRrcConnectionReestablishmentRequest msg) = 0;

  /**
   * \brief Receive an _RRCConnectionReestablishmentComplete_ message from a UE
   *        during an RRC connection re-establishment procedure
   *        (Section 5.3.7 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvNistRrcConnectionReestablishmentComplete (uint16_t rnti,
                                                         NistRrcConnectionReestablishmentComplete msg) = 0;

  /**
   * \brief Receive a _NistMeasurementReport_ message from a UE
   *        during a measurement reporting procedure
   *        (Section 5.5.5 of TS 36.331).
   * \param rnti the RNTI of UE which sent the message
   * \param msg the message
   */
  virtual void RecvNistMeasurementReport (uint16_t rnti, NistMeasurementReport msg) = 0;

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
 * Template for the implementation of the NistLteUeRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class NistMemberLteUeRrcSapUser : public NistLteUeRrcSapUser
{
public:
  NistMemberLteUeRrcSapUser (C* owner);

  // inherited from NistLteUeRrcSapUser
  virtual void Setup (NistSetupParameters params);
  virtual void SendNistRrcConnectionRequest (NistRrcConnectionRequest msg);
  virtual void SendNistRrcConnectionSetupCompleted (NistRrcConnectionSetupCompleted msg);
  virtual void SendNistRrcConnectionReconfigurationCompleted (NistRrcConnectionReconfigurationCompleted msg);
  virtual void SendNistRrcConnectionReestablishmentRequest (NistRrcConnectionReestablishmentRequest msg);
  virtual void SendNistRrcConnectionReestablishmentComplete (NistRrcConnectionReestablishmentComplete msg);
  virtual void SendNistMeasurementReport (NistMeasurementReport msg);
  virtual void SendSidelinkUeInformation (SidelinkUeInformation msg);

private:
  NistMemberLteUeRrcSapUser ();
  C* m_owner;
};

template <class C>
NistMemberLteUeRrcSapUser<C>::NistMemberLteUeRrcSapUser (C* owner)
  : m_owner (owner)
{
}

template <class C>
NistMemberLteUeRrcSapUser<C>::NistMemberLteUeRrcSapUser ()
{
}

template <class C>
void
NistMemberLteUeRrcSapUser<C>::Setup (NistSetupParameters params)
{
  m_owner->DoSetup (params);
}

template <class C>
void
NistMemberLteUeRrcSapUser<C>::SendNistRrcConnectionRequest (NistRrcConnectionRequest msg)
{
  m_owner->DoSendNistRrcConnectionRequest (msg);
}

template <class C>
void
NistMemberLteUeRrcSapUser<C>::SendNistRrcConnectionSetupCompleted (NistRrcConnectionSetupCompleted msg)
{
  m_owner->DoSendNistRrcConnectionSetupCompleted (msg);
}

template <class C>
void
NistMemberLteUeRrcSapUser<C>::SendNistRrcConnectionReconfigurationCompleted (NistRrcConnectionReconfigurationCompleted msg)
{
  m_owner->DoSendNistRrcConnectionReconfigurationCompleted (msg);
}

template <class C>
void
NistMemberLteUeRrcSapUser<C>::SendNistRrcConnectionReestablishmentRequest (NistRrcConnectionReestablishmentRequest msg)
{
  m_owner->DoSendNistRrcConnectionReestablishmentRequest (msg);
}

template <class C>
void
NistMemberLteUeRrcSapUser<C>::SendNistRrcConnectionReestablishmentComplete (NistRrcConnectionReestablishmentComplete msg)
{
  m_owner->DoSendNistRrcConnectionReestablishmentComplete (msg);
}

template <class C>
void
NistMemberLteUeRrcSapUser<C>::SendNistMeasurementReport (NistMeasurementReport msg)
{
  m_owner->DoSendNistMeasurementReport (msg);
}

template <class C>
void
NistMemberLteUeRrcSapUser<C>::SendSidelinkUeInformation (SidelinkUeInformation msg)
{
  m_owner->DoSendSidelinkUeInformation (msg);
}

/**
 * Template for the implementation of the NistLteUeRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class NistMemberLteUeRrcSapProvider : public NistLteUeRrcSapProvider
{
public:
  NistMemberLteUeRrcSapProvider (C* owner);

  // methods inherited from NistLteUeRrcSapProvider go here
  virtual void CompleteSetup (CompleteNistSetupParameters params);
  virtual void RecvNistSystemInformation (NistSystemInformation msg);
  virtual void RecvNistRrcConnectionSetup (NistRrcConnectionSetup msg);
  virtual void RecvNistRrcConnectionReconfiguration (NistRrcConnectionReconfiguration msg);
  virtual void RecvNistRrcConnectionReestablishment (NistRrcConnectionReestablishment msg);
  virtual void RecvNistRrcConnectionReestablishmentReject (NistRrcConnectionReestablishmentReject msg);
  virtual void RecvNistRrcConnectionRelease (NistRrcConnectionRelease msg);
  virtual void RecvNistRrcConnectionReject (NistRrcConnectionReject msg);

private:
  NistMemberLteUeRrcSapProvider ();
  C* m_owner;
};

template <class C>
NistMemberLteUeRrcSapProvider<C>::NistMemberLteUeRrcSapProvider (C* owner)
  : m_owner (owner)
{
}

template <class C>
NistMemberLteUeRrcSapProvider<C>::NistMemberLteUeRrcSapProvider ()
{
}

template <class C>
void
NistMemberLteUeRrcSapProvider<C>::CompleteSetup (CompleteNistSetupParameters params)
{
  m_owner->DoCompleteSetup (params);
}

template <class C>
void
NistMemberLteUeRrcSapProvider<C>::RecvNistSystemInformation (NistSystemInformation msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistSystemInformation, m_owner, msg);
}

template <class C>
void
NistMemberLteUeRrcSapProvider<C>::RecvNistRrcConnectionSetup (NistRrcConnectionSetup msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistRrcConnectionSetup, m_owner, msg);
}

template <class C>
void
NistMemberLteUeRrcSapProvider<C>::RecvNistRrcConnectionReconfiguration (NistRrcConnectionReconfiguration msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistRrcConnectionReconfiguration, m_owner, msg);
}

template <class C>
void
NistMemberLteUeRrcSapProvider<C>::RecvNistRrcConnectionReestablishment (NistRrcConnectionReestablishment msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistRrcConnectionReestablishment, m_owner, msg);
}

template <class C>
void
NistMemberLteUeRrcSapProvider<C>::RecvNistRrcConnectionReestablishmentReject (NistRrcConnectionReestablishmentReject msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistRrcConnectionReestablishmentReject, m_owner, msg);
}

template <class C>
void
NistMemberLteUeRrcSapProvider<C>::RecvNistRrcConnectionRelease (NistRrcConnectionRelease msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistRrcConnectionRelease, m_owner, msg);
}

template <class C>
void
NistMemberLteUeRrcSapProvider<C>::RecvNistRrcConnectionReject (NistRrcConnectionReject msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistRrcConnectionReject, m_owner, msg);
}


/**
 * Template for the implementation of the NistLteEnbRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class NistMemberLteEnbRrcSapUser : public NistLteEnbRrcSapUser
{
public:
  NistMemberLteEnbRrcSapUser (C* owner);

  // inherited from NistLteEnbRrcSapUser

  virtual void SetupUe (uint16_t rnti, NistSetupUeParameters params);
  virtual void RemoveUe (uint16_t rnti);
  virtual void SendNistSystemInformation (NistSystemInformation msg);
  virtual void SendNistRrcConnectionSetup (uint16_t rnti, NistRrcConnectionSetup msg);
  virtual void SendNistRrcConnectionReconfiguration (uint16_t rnti, NistRrcConnectionReconfiguration msg);
  virtual void SendNistRrcConnectionReestablishment (uint16_t rnti, NistRrcConnectionReestablishment msg);
  virtual void SendNistRrcConnectionReestablishmentReject (uint16_t rnti, NistRrcConnectionReestablishmentReject msg);
  virtual void SendNistRrcConnectionRelease (uint16_t rnti, NistRrcConnectionRelease msg);
  virtual void SendNistRrcConnectionReject (uint16_t rnti, NistRrcConnectionReject msg);
  virtual Ptr<Packet> EncodeNistHandoverPreparationInformation (NistHandoverPreparationInfo msg);
  virtual NistHandoverPreparationInfo DecodeNistHandoverPreparationInformation (Ptr<Packet> p);
  virtual Ptr<Packet> EncodeHandoverCommand (NistRrcConnectionReconfiguration msg);
  virtual NistRrcConnectionReconfiguration DecodeHandoverCommand (Ptr<Packet> p);

private:
  NistMemberLteEnbRrcSapUser ();
  C* m_owner;
};

template <class C>
NistMemberLteEnbRrcSapUser<C>::NistMemberLteEnbRrcSapUser (C* owner)
  : m_owner (owner)
{
}

template <class C>
NistMemberLteEnbRrcSapUser<C>::NistMemberLteEnbRrcSapUser ()
{
}

template <class C>
void
NistMemberLteEnbRrcSapUser<C>::SetupUe (uint16_t rnti, NistSetupUeParameters params)
{
  m_owner->DoSetupUe (rnti, params);
}

template <class C>
void
NistMemberLteEnbRrcSapUser<C>::RemoveUe (uint16_t rnti)
{
  m_owner->DoRemoveUe (rnti);
}

template <class C>
void
NistMemberLteEnbRrcSapUser<C>::SendNistSystemInformation (NistSystemInformation msg)
{
  m_owner->DoSendNistSystemInformation (msg);
}

template <class C>
void
NistMemberLteEnbRrcSapUser<C>::SendNistRrcConnectionSetup (uint16_t rnti, NistRrcConnectionSetup msg)
{
  m_owner->DoSendNistRrcConnectionSetup (rnti, msg);
}

template <class C>
void
NistMemberLteEnbRrcSapUser<C>::SendNistRrcConnectionReconfiguration (uint16_t rnti, NistRrcConnectionReconfiguration msg)
{
  m_owner->DoSendNistRrcConnectionReconfiguration (rnti, msg);
}

template <class C>
void
NistMemberLteEnbRrcSapUser<C>::SendNistRrcConnectionReestablishment (uint16_t rnti, NistRrcConnectionReestablishment msg)
{
  m_owner->DoSendNistRrcConnectionReestablishment (rnti, msg);
}

template <class C>
void
NistMemberLteEnbRrcSapUser<C>::SendNistRrcConnectionReestablishmentReject (uint16_t rnti, NistRrcConnectionReestablishmentReject msg)
{
  m_owner->DoSendNistRrcConnectionReestablishmentReject (rnti, msg);
}

template <class C>
void
NistMemberLteEnbRrcSapUser<C>::SendNistRrcConnectionRelease (uint16_t rnti, NistRrcConnectionRelease msg)
{
  m_owner->DoSendNistRrcConnectionRelease (rnti, msg);
}

template <class C>
void
NistMemberLteEnbRrcSapUser<C>::SendNistRrcConnectionReject (uint16_t rnti, NistRrcConnectionReject msg)
{
  m_owner->DoSendNistRrcConnectionReject (rnti, msg);
}

template <class C>
Ptr<Packet>
NistMemberLteEnbRrcSapUser<C>::EncodeNistHandoverPreparationInformation (NistHandoverPreparationInfo msg)
{
  return m_owner->DoEncodeNistHandoverPreparationInformation (msg);
}

template <class C>
NistLteRrcSap::NistHandoverPreparationInfo
NistMemberLteEnbRrcSapUser<C>::DecodeNistHandoverPreparationInformation (Ptr<Packet> p)
{
  return m_owner->DoDecodeNistHandoverPreparationInformation (p);
}


template <class C>
Ptr<Packet>
NistMemberLteEnbRrcSapUser<C>::EncodeHandoverCommand (NistRrcConnectionReconfiguration msg)
{
  return m_owner->DoEncodeHandoverCommand (msg);
}

template <class C>
NistLteRrcSap::NistRrcConnectionReconfiguration
NistMemberLteEnbRrcSapUser<C>::DecodeHandoverCommand (Ptr<Packet> p)
{
  return m_owner->DoDecodeHandoverCommand (p);
}

/**
 * Template for the implementation of the NistLteEnbRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class NistMemberLteEnbRrcSapProvider : public NistLteEnbRrcSapProvider
{
public:
  NistMemberLteEnbRrcSapProvider (C* owner);

  // methods inherited from NistLteEnbRrcSapProvider go here

  virtual void CompleteSetupUe (uint16_t rnti, CompleteNistSetupUeParameters params);
  virtual void RecvNistRrcConnectionRequest (uint16_t rnti, NistRrcConnectionRequest msg);
  virtual void RecvNistRrcConnectionSetupCompleted (uint16_t rnti, NistRrcConnectionSetupCompleted msg);
  virtual void RecvNistRrcConnectionReconfigurationCompleted (uint16_t rnti, NistRrcConnectionReconfigurationCompleted msg);
  virtual void RecvNistRrcConnectionReestablishmentRequest (uint16_t rnti, NistRrcConnectionReestablishmentRequest msg);
  virtual void RecvNistRrcConnectionReestablishmentComplete (uint16_t rnti, NistRrcConnectionReestablishmentComplete msg);
  virtual void RecvNistMeasurementReport (uint16_t rnti, NistMeasurementReport msg);
  virtual void RecvSidelinkUeInformation (uint16_t rnti, SidelinkUeInformation msg);

private:
  NistMemberLteEnbRrcSapProvider ();
  C* m_owner;
};

template <class C>
NistMemberLteEnbRrcSapProvider<C>::NistMemberLteEnbRrcSapProvider (C* owner)
  : m_owner (owner)
{
}

template <class C>
NistMemberLteEnbRrcSapProvider<C>::NistMemberLteEnbRrcSapProvider ()
{
}

template <class C>
void
NistMemberLteEnbRrcSapProvider<C>::CompleteSetupUe (uint16_t rnti, CompleteNistSetupUeParameters params)
{
  m_owner->DoCompleteSetupUe (rnti, params);
}

template <class C>
void
NistMemberLteEnbRrcSapProvider<C>::RecvNistRrcConnectionRequest (uint16_t rnti, NistRrcConnectionRequest msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistRrcConnectionRequest, m_owner, rnti, msg);
}

template <class C>
void
NistMemberLteEnbRrcSapProvider<C>::RecvNistRrcConnectionSetupCompleted (uint16_t rnti, NistRrcConnectionSetupCompleted msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistRrcConnectionSetupCompleted, m_owner, rnti, msg);
}

template <class C>
void
NistMemberLteEnbRrcSapProvider<C>::RecvNistRrcConnectionReconfigurationCompleted (uint16_t rnti, NistRrcConnectionReconfigurationCompleted msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistRrcConnectionReconfigurationCompleted, m_owner, rnti, msg);
}

template <class C>
void
NistMemberLteEnbRrcSapProvider<C>::RecvNistRrcConnectionReestablishmentRequest (uint16_t rnti, NistRrcConnectionReestablishmentRequest msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistRrcConnectionReestablishmentRequest, m_owner, rnti, msg);
}

template <class C>
void
NistMemberLteEnbRrcSapProvider<C>::RecvNistRrcConnectionReestablishmentComplete (uint16_t rnti, NistRrcConnectionReestablishmentComplete msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistRrcConnectionReestablishmentComplete, m_owner, rnti, msg);
}

template <class C>
void
NistMemberLteEnbRrcSapProvider<C>::RecvNistMeasurementReport (uint16_t rnti, NistMeasurementReport msg)
{
  Simulator::ScheduleNow (&C::DoRecvNistMeasurementReport, m_owner, rnti, msg);
}

template <class C>
void
NistMemberLteEnbRrcSapProvider<C>::RecvSidelinkUeInformation (uint16_t rnti, SidelinkUeInformation msg)
{
  Simulator::ScheduleNow (&C::DoRecvSidelinkUeInformation, m_owner, rnti, msg);
}













} // namespace ns3


#endif // NIST_LTE_RRC_SAP_H




