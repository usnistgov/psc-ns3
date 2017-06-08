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

#ifndef NIST_FF_MAC_COMMON_H
#define NIST_FF_MAC_COMMON_H

#include <ns3/simple-ref-count.h>
#include <ns3/ptr.h>
#include <vector>
#include<bitset>
#include <map>


/**
 * Constants. See section 4.4
 */
#define MAX_SCHED_CFG_LIST    10
#define MAX_LC_LIST           10

#define MAX_RACH_LIST         30
#define MAX_DL_INFO_LIST      30
#define MAX_BUILD_DATA_LIST   30
#define MAX_BUILD_RAR_LIST    10
#define MAX_BUILD_BC_LIST     3
#define MAX_UL_INFO_LIST      30
#define MAX_DCI_LIST          30
#define MAX_PHICH_LIST        30
#define MAX_TB_LIST           2
#define MAX_RLC_PDU_LIST      30
#define MAX_NR_LCG            4
#define MAX_MBSFN_CONFIG      5
#define MAX_SI_MSG_LIST       32
#define MAX_SI_MSG_SIZE       65535

#define MAX_CQI_LIST          30
#define MAX_UE_SELECTED_SB    6
#define MAX_HL_SB             25
#define MAX_SINR_RB_LIST      100
#define MAX_SR_LIST           30
#define MAX_MAC_CE_LIST       30

namespace ns3 {

enum Result_e
{
  SUCCESS,
  FAILURE
};

enum SetupRelease_e
{
  setup,
  release
};

enum CeBitmap_e
{
  TA,
  DRX,
  CR
};

enum NormalExtended_e
{
  normal,
  extended
};


/**
 * \brief See section 4.3.1 dlDciListElement
 */
struct NistDlDciListElement_s
{
  uint16_t  m_rnti;
  uint32_t  m_rbBitmap;
  uint8_t   m_rbShift;
  uint8_t   m_resAlloc;
  std::vector <uint16_t>  m_tbsSize;
  std::vector <uint8_t>   m_mcs;
  std::vector <uint8_t>   m_ndi;
  std::vector <uint8_t>   m_rv;
  uint8_t   m_cceIndex;
  uint8_t   m_aggrLevel;
  uint8_t   m_precodingInfo;
  enum Format_e
  {
    ONE, ONE_A, ONE_B, ONE_C, ONE_D, TWO, TWO_A, TWO_B
  } m_format;
  uint8_t   m_tpc;
  uint8_t   m_harqProcess;
  uint8_t   m_dai;
  enum VrbFormat_e
  {
    VRB_DISTRIBUTED,
    VRB_LOCALIZED
  } m_vrbFormat;
  bool      m_tbSwap;
  bool      m_spsRelease;
  bool      m_pdcchOrder;
  uint8_t   m_preambleIndex;
  uint8_t   m_prachMaskIndex;
  enum Ngap_e
  {
    GAP1, GAP2
  } m_nGap;
  uint8_t   m_tbsIdx;
  uint8_t   m_dlPowerOffset;
  uint8_t   m_pdcchPowerOffset;
};

/**
 * \brief See section 4.3.2 ulDciListElement
 */
struct NistUlDciListElement_s
{
  uint16_t  m_rnti;
  uint8_t   m_rbStart;
  uint8_t   m_rbLen;
  uint16_t  m_tbSize;
  uint8_t   m_mcs;
  uint8_t   m_ndi;
  uint8_t   m_cceIndex;
  uint8_t   m_aggrLevel;
  uint8_t   m_ueTxAntennaSelection;
  bool      m_hopping;
  uint8_t   m_n2Dmrs;
  int8_t    m_tpc;
  bool      m_cqiRequest;
  uint8_t   m_ulIndex;
  uint8_t   m_dai;
  uint8_t   m_freqHopping;
  int8_t    m_pdcchPowerOffset;
};
  
/**
 * \brief See section 5.3.3.1.9 Rel 12.4
 */
struct NistSlDciListElement_s
{
  uint16_t m_rnti;
  uint16_t m_resPscch;
  uint8_t m_tpc;
  uint8_t m_hopping;
  uint8_t m_rbStart; //models rb assignment
  uint8_t m_rbLen;   //models rb assignment
  uint8_t m_trp;
};

/**
 * \brief See section 5.4.3.1.1 Rel 12.4
 */
struct NistSciListElement_s
{
  uint16_t  m_rnti;
  uint8_t   m_resPscch;       //added for modeling
  uint8_t   m_hopping;
  uint8_t   m_rbStart; //models rb assignment
  uint8_t   m_rbLen;   //models rb assignment
  uint16_t  m_tbSize;  //added for modeling
  uint8_t   m_trp;
  uint8_t   m_mcs;
  uint16_t  m_timing;
  uint8_t   m_groupDstId;
};
 
struct NistSlDiscMsg
{
  uint16_t  m_rnti; //added for modeling
  uint8_t   m_resPsdch; //added for modeling
  uint8_t m_msgType; 
  std::bitset <184> m_proSeAppCode;
  uint32_t m_mic;
  uint8_t m_utcBasedCounter;
};

/**
* \brief Base class for storing the values of vendor specific parameters
*/
struct NistVendorSpecificValue : public SimpleRefCount<NistVendorSpecificValue>
{ 
  virtual ~NistVendorSpecificValue ();

};

/**
 * \brief See section 4.3.3 vendorSpecifiListElement
 */
struct NistVendorSpecificListElement_s
{
  uint32_t m_type;
  uint32_t m_length;
  Ptr<NistVendorSpecificValue> m_value;
};

/**
 * \brief See section 4.3.4 logicalChannelConfigListElement
 */
struct NistLogicalChannelConfigListElement_s
{
  uint8_t   m_logicalChannelIdentity;
  uint8_t   m_logicalChannelGroup;

  enum Direction_e
  {
    DIR_UL,
    DIR_DL,
    DIR_BOTH
  } m_direction;

  enum QosBearerType_e
  {
    QBT_NON_GBR,
    QBT_GBR
  } m_qosBearerType;

  uint8_t   m_qci;
  uint64_t  m_eRabMaximulBitrateUl;
  uint64_t  m_eRabMaximulBitrateDl;
  uint64_t  m_eRabGuaranteedBitrateUl;
  uint64_t  m_eRabGuaranteedBitrateDl;
};

/**
 * \brief See section 4.3.6 rachListElement
 */
struct NistRachListElement_s
{
  uint16_t  m_rnti;
  uint16_t  m_estimatedSize;
};

/**
 * \brief See section 4.3.7 phichListElement
 */
struct NistPhichListElement_s
{
  uint16_t  m_rnti;
  enum Phich_e
  {
    ACK, NACK
  } m_phich;
};

/**
 * \brief See section 4.3.9 rlcPDU_ListElement
 */
struct NistRlcPduListElement_s
{
  uint8_t   m_logicalChannelIdentity;
  uint16_t  m_size;
};

/**
 * \brief See section 4.3.8 builDataListElement
 */
struct NistBuildDataListElement_s
{
  uint16_t  m_rnti;
  struct NistDlDciListElement_s m_dci;
  std::vector <enum CeBitmap_e> m_ceBitmap;
  std::vector < std::vector <struct NistRlcPduListElement_s> > m_rlcPduList;
};

/**
 * \brief Substitutive structure for specifying NistBuildNistRarListElement_s::m_grant field
 */
struct NistUlGrant_s
{
  uint16_t m_rnti;
  uint8_t m_rbStart;
  uint8_t m_rbLen;
  uint16_t m_tbSize;
  uint8_t m_mcs;
  bool m_hopping;
  int8_t m_tpc;
  bool m_cqiRequest;
  bool m_ulDelay;
}; 

/**
 * \brief See section 4.3.10 buildRARListElement
 */
struct NistBuildNistRarListElement_s
{
  uint16_t  m_rnti;
  //uint32_t  m_grant; // Substituted with type NistUlGrant_s
  NistUlGrant_s m_grant;
  struct NistDlDciListElement_s m_dci;
};

/**
 * \brief See section 4.3.11 buildBroadcastListElement
 */
struct NistBuildBroadcastListElement_s
{
  enum Type_e
  {
    BCCH, PCCH
  } m_type;
  uint8_t m_index;
  struct NistDlDciListElement_s m_dci;
};

/**
 * \brief See section 4.3.12 ulInfoListElement
 */
struct NistUlInfoListElement_s
{
  uint16_t  m_rnti;
  std::vector <uint16_t> m_ulReception;
  enum ReceptionNistStatus_e
  {
    Ok, NotOk, NotValid
  } m_receptionNistStatus;
  uint8_t   m_tpc;
};

/**
 * \brief See section 4.3.13 srListElement
 */
struct NistSrListElement_s
{
  uint16_t  m_rnti;
};

/**
 * \brief See section 4.3.15 macCEValue
 */
struct NistMacCeValue_u
{
  uint8_t   m_phr;
  uint8_t   m_crnti;
  std::vector <uint8_t> m_bufferNistStatus;
  std::map <uint8_t, std::vector <uint8_t> > Nist_m_bufferNistStatus;     //modified structure m_m_bufferStatus to handle UL different bearer scheduling
};

/**
 * \brief See section 4.3.14 macCEListElement
 */
struct NistMacCeListElement_s
{
  uint16_t  m_rnti;
  enum MacCeType_e
  {
    BSR, PHR, CRNTI, SLBSR
  } m_macCeType;
  struct NistMacCeValue_u m_macCeValue;
};

/**
 * \brief See section 4.3.16 drxConfig
 */
struct NistDrxConfig_s
{
  uint8_t   m_onDurationTimer;
  uint16_t  m_drxInactivityTimer;
  uint16_t  m_drxRetransmissionTimer;
  uint16_t  m_longDrxCycle;
  uint16_t  m_longDrxCycleStartOffset;
  uint16_t  m_shortDrxCycle;
  uint8_t   m_drxShortCycleTimer;
};

/**
 * \brief See section 4.3.17 spsConfig
 */
struct NistSpsConfig_s
{
  uint16_t  m_semiPersistSchedIntervalUl;
  uint16_t  m_semiPersistSchedIntervalDl;
  uint8_t   m_numberOfConfSpsProcesses;
  uint8_t   m_n1PucchAnPersistentListSize;
  std::vector <uint16_t> m_n1PucchAnPersistentList;
  uint8_t   m_implicitReleaseAfter;
};

/**
 * \brief See section 4.3.18 srConfig
 */
struct NistSrConfig_s
{
  enum SetupRelease_e m_action;
  uint8_t   m_schedInterval;
  uint8_t   m_dsrTransMax;
};

/**
 * \brief See section 4.3.19 cqiConfig
 */
struct NistCqiConfig_s
{
  enum SetupRelease_e m_action;
  uint16_t  m_cqiSchedInterval;
  uint8_t   m_riSchedInterval;
};

/**
 * \brief See section 4.3.20 ueCapabilities
 */
struct NistUeCapabilities_s
{
  bool      m_halfDuplex;
  bool      m_intraSfHopping;
  bool      m_type2Sb1;
  uint8_t   m_ueCategory;
  bool      m_resAllocType1;
};

/**
 * \brief See section 4.3.22 siMessageListElement
 */
struct NistSiMessageListElement_s
{
  uint16_t  m_periodicity;
  uint16_t  m_length;
};

/**
 * \brief See section 4.3.21 siConfiguration
 */
struct NistSiConfiguration_s
{
  uint16_t  m_sfn;
  uint16_t  m_sib1Length;
  uint8_t   m_siWindowLength;
  std::vector <struct NistSiMessageListElement_s> m_siMessageList;
};

/**
 * \brief See section 4.3.23 dlInfoListElement
 */
struct NistDlInfoListElement_s
{
  uint16_t  m_rnti;
  uint8_t   m_harqProcessId;
  enum HarqNistStatus_e
  {
    ACK, NACK, DTX
  };
  std::vector <enum HarqNistStatus_e> m_harqNistStatus;
};

/**
 * \brief See section 4.3.28 bwPart
 */
struct NistBwPart_s
{
  uint8_t   m_bwPartIndex;
  uint8_t   m_sb;
  uint8_t   m_cqi;
};

/**
 * \brief See section 4.3.27 higherLayerSelected
 */
struct NistHigherLayerSelected_s
{
  uint8_t   m_sbPmi;
  std::vector <uint8_t> m_sbCqi;
};

/**
 * \brief See section 4.3.26 ueSelected
 */
struct NistUeSelected_s
{
  std::vector <uint8_t> m_sbList;
  uint8_t   m_sbPmi;
  std::vector <uint8_t> m_sbCqi;
};

/**
 * \brief See section 4.3.25 sbMeasResult
 */
struct NistSbMeasResult_s
{
  struct NistUeSelected_s           m_ueSelected;
  std::vector <struct NistHigherLayerSelected_s> m_higherLayerSelected;
  struct NistBwPart_s               m_bwPart;
};

/**
 * \brief See section 4.3.24 cqiListElement
 */
struct NistCqiListElement_s
{
  uint16_t  m_rnti;
  uint8_t   m_ri;
  enum CqiType_e
  {
    P10, P11, P20, P21, A12, A22, A20, A30, A31
  } m_cqiType;
  std::vector <uint8_t> m_wbCqi;
  uint8_t   m_wbPmi;

  struct NistSbMeasResult_s m_sbMeasResult;
};

/**
 * \brief See section 4.3.29 ulCQI
 */
struct NistUlCqi_s
{
  std::vector <uint16_t> m_sinr;
  enum Type_e
  {
    SRS,
    PUSCH,
    PUCCH_1,
    PUCCH_2,
    PRACH
  } m_type;
};

/**
 * \brief See section 4.3.30 pagingInfoListElement
 */
struct NistPagingInfoListElement_s
{
  uint8_t   m_pagingIndex;
  uint16_t  m_pagingMessageSize;
  uint8_t   m_pagingSubframe;
};

} // namespace ns3

#endif /* NIST_FF_MAC_COMMON_H */
