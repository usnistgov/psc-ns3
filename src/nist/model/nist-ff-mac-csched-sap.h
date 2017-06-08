/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#ifndef NIST_FF_MAC_CSCHED_SAP_H
#define NIST_FF_MAC_CSCHED_SAP_H

#include <stdint.h>
#include <vector>

#include "nist-ff-mac-common.h"
#include "nist-sl-pool.h"

namespace ns3 {

/**
 * \ingroup ff-api
 * \brief Provides the CSCHED SAP
 *
 * This abstract class defines the MAC Scheduler interface specified in the
 * Femto Forum Technical Document:
 *   - LTE MAC Scheduler Interface Specification v1.11
 *
 * The Technical Document contains a detailed description of the API.
 * The documentation of this class refers to sections of this Technical Document.
 *
 * You can found an example of the implementation of this interface
 * in the SampleNistFfMacCschedSapProvider and SampleFfMacCschedSapuser classes
 */
class NistFfMacCschedSapProvider
{
public:
  virtual ~NistFfMacCschedSapProvider ();

  /**
   * Parameters of the API primitives
   */

  /**
   * Parameters of the CSCHED_CELL_CONFIG_REQ primitive.
   * See section 4.1.1 for a detailed description of the parameters.
   */
  struct NistCschedCellConfigReqParameters
  {
    uint8_t m_puschHoppingOffset;

    enum HoppingMode_e
    {
      inter,
      interintra
    } m_hoppingMode;

    uint8_t m_nSb;

    enum PhichResource_e
    {
      PHICH_R_ONE_SIXTH,
      PHICH_R_HALF,
      PHICH_R_ONE,
      PHICH_R_TWO
    } m_phichResource;


    enum NormalExtended_e m_phichDuration;

    uint8_t m_initialNrOfPdcchOfdmSymbols;

    struct NistSiConfiguration_s m_siConfiguration;

    uint8_t m_ulBandwidth;
    uint8_t m_dlBandwidth;

    enum NormalExtended_e m_ulCyclicPrefixLength;
    enum NormalExtended_e m_dlCyclicPrefixLength;

    uint8_t m_antennaPortsCount;

    enum DuplexMode_e
    {
      DM_TDD,
      DM_FDD
    } m_duplexMode;

    uint8_t m_subframeAssignment;
    uint8_t m_specialSubframePatterns;
    std::vector <uint8_t> m_mbsfnSubframeConfigRfPeriod;
    std::vector <uint8_t> m_mbsfnSubframeConfigRfOffset;
    std::vector <uint8_t> m_mbsfnSubframeConfigSfAllocation;
    uint8_t m_prachConfigurationIndex;
    uint8_t m_prachFreqOffset;
    uint8_t m_raResponseWindowSize;
    uint8_t m_macContentionResolutionTimer;
    uint8_t m_maxHarqMsg3Tx;
    uint16_t m_n1PucchAn;
    uint8_t m_deltaPucchShift;
    uint8_t m_nrbCqi;
    uint8_t m_ncsAn;
    uint8_t m_srsSubframeConfiguration;
    uint8_t m_srsSubframeOffset;
    uint8_t m_srsBandwidthConfiguration;
    bool    m_srsMaxUpPts;

    enum Enable64Qam_e
    {
      MOD_16QAM,
      MOD_64QAM
    } m_enable64Qam;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;
  };

  /**
   * Parameters of the CSCHED_UE_CONFIG_REQ primitive.
   * See section 4.1.3 for a detailed description of the parameters.
   */
  struct NistCschedNistUeConfigReqParameters
  {
    uint16_t  m_rnti;
    bool      m_reconfigureFlag;
    bool      m_drxConfigPresent;
    struct NistDrxConfig_s m_drxConfig;
    uint16_t  m_timeAlignmentTimer;

    enum NistMeasGapConfigPattern_e
    {
      MGP_GP1,
      MGP_GP2,
      OFF
    } m_measGapConfigPattern;

    uint8_t   m_measGapConfigSubframeOffset;
    bool      m_spsConfigPresent;
    struct NistSpsConfig_s m_spsConfig;
    bool      m_srConfigPresent;
    struct NistSrConfig_s m_srConfig;
    bool      m_cqiConfigPresent;
    struct NistCqiConfig_s m_cqiConfig;
    uint8_t   m_transmissionMode;
    uint64_t  m_ueAggregatedMaximumBitrateUl;
    uint64_t  m_ueAggregatedMaximumBitrateDl;
    struct NistUeCapabilities_s m_ueCapabilities;

    enum OpenClosedLoop_e
    {
      noneloop,
      openloop,
      closedloop
    } m_ueTransmitAntennaSelection;

    bool      m_ttiBundling;
    uint8_t   m_maxHarqTx;
    uint8_t   m_betaOffsetAckIndex;
    uint8_t   m_betaOffsetRiIndex;
    uint8_t   m_betaOffsetCqiIndex;
    bool      m_ackNackSrsSimultaneousTransmission;
    bool      m_simultaneousAckNackAndCqi;

    enum RepMode_e
    {
      rm12, rm20, rm22, rm30, rm31, nonemode
    } m_aperiodicCqiRepMode;

    enum FeedbackMode_e
    {
      bundling,
      multiplexing
    } m_tddAckNackFeedbackMode;

    uint8_t   m_ackNackRepetitionFactor;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;

    std::vector <uint32_t> m_slDestinations;
  };

  /**
   * Parameters of the CSCHED_LC_CONFIG_REQ primitive.
   * See section 4.1.5 for a detailed description of the parameters.
   */
  struct NistCschedLcConfigReqParameters
  {
    uint16_t  m_rnti;
    bool      m_reconfigureFlag;

    std::vector <struct NistLogicalChannelConfigListElement_s> m_logicalChannelConfigList;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;
  };

  /**
   * Parameters of the CSCHED_LC_RELEASE_REQ primitive.
   * See section 4.1.7 for a detailed description of the parameters.
   */
  struct NistCschedLcReleaseReqParameters
  {
    uint16_t  m_rnti;

    std::vector <uint8_t> m_logicalChannelIdentity;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;
  };

  /**
   * Parameters of the CSCHED_UE_RELEASE_REQ primitive.
   * See section 4.1.9 for a detailed description of the parameters.
   */
  struct NistCschedUeReleaseReqParameters
  {
    uint16_t  m_rnti;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;
  };

  /**
   * Parameters to setup a pool
   */
  struct NistCschedPoolConfigReqParameters
  {
    uint32_t m_group;

    Ptr<SidelinkCommResourcePool> m_pool;
  };
  
  /**
   * Parameters to setup a pool
   */
  struct NistCschedPoolReleaseReqParameters
  {
    uint32_t m_group;
  };
  

  
  //
  // CSCHED - MAC Scheduler Control SAP primitives
  // (See 4.1 for description of the primitives)
  //

  /**
   * \brief CSCHED_CELL_CONFIG_REQ
   */
  virtual void CschedCellConfigReq (const struct NistCschedCellConfigReqParameters& params) = 0;

  virtual void CschedNistUeConfigReq (const struct NistCschedNistUeConfigReqParameters& params) = 0;

  virtual void CschedLcConfigReq (const struct NistCschedLcConfigReqParameters& params) = 0;

  virtual void CschedLcReleaseReq (const struct NistCschedLcReleaseReqParameters& params) = 0;

  virtual void CschedUeReleaseReq (const struct NistCschedUeReleaseReqParameters& params) = 0;

  //we are not making those function purely virtual so not all schedulers need to support them
  virtual void CschedPoolConfigReq (const struct NistCschedPoolConfigReqParameters& params) {};
  
  virtual void CschedPoolReleaseReq (const struct NistCschedPoolReleaseReqParameters& params) {};

private:
};


class NistFfMacCschedSapUser
{
public:
  virtual ~NistFfMacCschedSapUser ();

  /**
   * Parameters of the API primitives
   */

  /**
   * Parameters of the CSCHED_CELL_CONFIG_CNF primitive.
   * See section 4.1.2 for a detailed description of the parameters.
   */
  struct NistCschedCellConfigCnfParameters
  {
    enum Result_e m_result;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;
  };

  /**
   * Parameters of the CSCHED_UE_CONFIG_CNF primitive.
   * See section 4.1.4 for a detailed description of the parameters.
   */
  struct NistCschedNistUeConfigCnfParameters
  {
    uint16_t  m_rnti;
    enum Result_e m_result;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;
  };

  /**
   * Parameters of the CSCHED_LC_CONFIG_CNF primitive.
   * See section 4.1.6 for a detailed description of the parameters.
   */
  struct NistCschedLcConfigCnfParameters
  {
    uint16_t  m_rnti;
    enum Result_e m_result;

    std::vector <uint8_t> m_logicalChannelIdentity;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;
  };

  /**
   * Parameters of the CSCHED_LC_RELEASE_CNF primitive.
   * See section 4.1.8 for a detailed description of the parameters.
   */
  struct NistCschedLcReleaseCnfParameters
  {
    uint16_t  m_rnti;
    enum Result_e m_result;

    std::vector <uint8_t> m_logicalChannelIdentity;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;
  };

  /**
   * Parameters of the CSCHED_UE_RELEASE_CNF primitive.
   * See section 4.1.10 for a detailed description of the parameters.
   */
  struct NistCschedUeReleaseCnfParameters
  {
    uint16_t  m_rnti;
    enum Result_e m_result;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;
  };

  /**
   * Parameters of the CSCHED_UE_CONFIG_UPDATE_IND primitive.
   * See section 4.1.11 for a detailed description of the parameters.
   */
  struct NistCschedNistUeConfigUpdateIndParameters
  {
    uint16_t  m_rnti;
    uint8_t   m_transmissionMode;
    bool      m_spsConfigPresent;
    struct NistSpsConfig_s m_spsConfig;
    bool      m_srConfigPresent;
    struct NistSrConfig_s m_srConfig;
    bool      m_cqiConfigPresent;
    struct NistCqiConfig_s m_cqiConfig;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;
  };

  /**
   * Parameters of the CSCHED_CELL_CONFIG_UPDATE_IND primitive.
   * See section 4.1.12 for a detailed description of the parameters.
   */
  struct NistCschedCellConfigUpdateIndParameters
  {
    uint8_t   m_prbUtilizationDl;
    uint8_t   m_prbUtilizationUl;

    std::vector <struct NistVendorSpecificListElement_s> m_vendorSpecificList;
  };

  //
  // CSCHED - MAC Scheduler Control SAP primitives
  // (See 4.1 for description of the primitives)
  //

  virtual void CschedCellConfigCnf (const struct NistCschedCellConfigCnfParameters& params) = 0;

  virtual void CschedNistUeConfigCnf (const struct NistCschedNistUeConfigCnfParameters& params) = 0;

  virtual void CschedLcConfigCnf (const struct NistCschedLcConfigCnfParameters& params) = 0;

  virtual void CschedLcReleaseCnf (const struct NistCschedLcReleaseCnfParameters& params) = 0;

  virtual void CschedUeReleaseCnf (const struct NistCschedUeReleaseCnfParameters& params) = 0;

  virtual void CschedNistUeConfigUpdateInd (const struct NistCschedNistUeConfigUpdateIndParameters& params) = 0;

  virtual void CschedCellConfigUpdateInd (const struct NistCschedCellConfigUpdateIndParameters& params) = 0;

private:
};

} // end namespace ns3

#endif /* NIST_FF_MAC_CSCHED_SAP_H */
