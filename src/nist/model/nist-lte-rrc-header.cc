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
 * Author: Lluis Parcerisa <lparcerisa@cttc.cat>
 * Modified by: NIST
 */

#include "ns3/log.h"
#include "ns3/nist-lte-rrc-header.h"

#include <stdio.h>
#include <sstream>

#define MAX_DRB 11 // According to section 6.4 3GPP TS 36.331
#define MAX_EARFCN 65535
#define MAX_RAT_CAPABILITIES 8
#define MAX_SI_MESSAGE 32
#define MAX_SIB 32

#define MAX_REPORT_CONFIG_ID 32
#define MAX_OBJECT_ID 32
#define MAX_MEAS_ID 32
#define MAX_CELL_MEAS 32
#define MAX_CELL_REPORT 8

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistRrcHeader");

//////////////////// NistRrcAsn1Header class ///////////////////////////////
NistRrcAsn1Header::NistRrcAsn1Header ()
{
}

TypeId
NistRrcAsn1Header::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistRrcAsn1Header")
    .SetParent<Header> ()
  ;
  return tid;
}

TypeId
NistRrcAsn1Header::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

int
NistRrcAsn1Header::GetMessageType ()
{
  return m_messageType;
}

void
NistRrcAsn1Header::SerializeNistDrbToAddModList (std::list<NistLteRrcSap::NistDrbToAddMod> drbToAddModList) const
{
  // Serialize DRB-ToAddModList sequence-of
  SerializeSequenceOf (drbToAddModList.size (),MAX_DRB,1);

  // Serialize the elements in the sequence-of list
  std::list<NistLteRrcSap::NistDrbToAddMod>::iterator it = drbToAddModList.begin ();
  for (; it != drbToAddModList.end (); it++)
    {
      // Serialize DRB-ToAddMod sequence
      // 5 otional fields. Extension marker is present.
      std::bitset<5> drbToAddModListOptionalFieldsPresent = std::bitset<5> ();
      drbToAddModListOptionalFieldsPresent.set (4,1); // eps-BearerIdentity present
      drbToAddModListOptionalFieldsPresent.set (3,0); // pdcp-Config not present
      drbToAddModListOptionalFieldsPresent.set (2,1); // rlc-Config present
      drbToAddModListOptionalFieldsPresent.set (1,1); // logicalChannelIdentity present
      drbToAddModListOptionalFieldsPresent.set (0,1); // logicalChannelConfig present
      SerializeSequence (drbToAddModListOptionalFieldsPresent,true);

      // Serialize eps-BearerIdentity::=INTEGER (0..15)
      SerializeInteger (it->epsBearerIdentity,0,15);

      // Serialize drb-Identity ::= INTEGER (1..32)
      SerializeInteger (it->drbIdentity,1,32);

      switch (it->rlcConfig.choice)
        {
        case NistLteRrcSap::NistRlcConfig::UM_BI_DIRECTIONAL:
          // Serialize rlc-Config choice
          SerializeChoice (4,1,true);

          // Serialize UL-UM-RLC
          SerializeSequence (std::bitset<0> (),false);
          SerializeEnum (2,0);  // sn-FieldLength

          // Serialize DL-UM-RLC
          SerializeSequence (std::bitset<0> (),false);
          SerializeEnum (2,0);  // sn-FieldLength
          SerializeEnum (32,0);  // t-Reordering
          break;

        case NistLteRrcSap::NistRlcConfig::UM_UNI_DIRECTIONAL_UL:
          // Serialize rlc-Config choice
          SerializeChoice (4,2,true);

          // Serialize UL-UM-RLC
          SerializeSequence (std::bitset<0> (),false);
          SerializeEnum (2,0);  // sn-FieldLength
          break;

        case NistLteRrcSap::NistRlcConfig::UM_UNI_DIRECTIONAL_DL:
          // Serialize rlc-Config choice
          SerializeChoice (4,3,true);

          // Serialize DL-UM-RLC
          SerializeSequence (std::bitset<0> (),false);
          SerializeEnum (2,0);  // sn-FieldLength
          SerializeEnum (32,0);  // t-Reordering
          break;

        case NistLteRrcSap::NistRlcConfig::AM:
        default:
          // Serialize rlc-Config choice
          SerializeChoice (4,0,true);

          // Serialize UL-AM-RLC
          SerializeSequence (std::bitset<0> (),false);
          SerializeEnum (64,0);  // t-PollRetransmit
          SerializeEnum (8,0);   // pollPDU
          SerializeEnum (16,0);  // pollByte
          SerializeEnum (8,0);   // maxRetxThreshold

          // Serialize DL-AM-RLC
          SerializeSequence (std::bitset<0> (),false);
          SerializeEnum (32,0);  // t-Reordering
          SerializeEnum (64,0);  // t-NistStatusProhibit
          break;
        }

      // Serialize logicalChannelIdentity ::=INTEGER (3..10)
      SerializeInteger (it->logicalChannelIdentity,3,10);

      // Serialize logicalChannelConfig
      SerializeNistLogicalChannelConfig (it->logicalChannelConfig);
    }
}

void
NistRrcAsn1Header::SerializeNistSrbToAddModList (std::list<NistLteRrcSap::NistSrbToAddMod> srbToAddModList) const
{
  // Serialize SRB-ToAddModList ::= SEQUENCE (SIZE (1..2)) OF SRB-ToAddMod
  SerializeSequenceOf (srbToAddModList.size (),2,1);

  // Serialize the elements in the sequence-of list
  std::list<NistLteRrcSap::NistSrbToAddMod>::iterator it = srbToAddModList.begin ();
  for (; it != srbToAddModList.end (); it++)
    {
      // Serialize SRB-ToAddMod sequence
      // 2 otional fields. Extension marker is present.
      std::bitset<2> srbToAddModListOptionalFieldsPresent = std::bitset<2> ();
      srbToAddModListOptionalFieldsPresent.set (1,0); // rlc-Config not present
      srbToAddModListOptionalFieldsPresent.set (0,1); // logicalChannelConfig present
      SerializeSequence (srbToAddModListOptionalFieldsPresent,true);

      // Serialize srb-Identity ::= INTEGER (1..2)
      SerializeInteger (it->srbIdentity,1,2);

      // Serialize logicalChannelConfig choice
      // 2 options, selected option 0 (var "explicitValue", of type NistLogicalChannelConfig)
      SerializeChoice (2,0,false);

      // Serialize NistLogicalChannelConfig 
      SerializeNistLogicalChannelConfig (it->logicalChannelConfig);
    }
}

void
NistRrcAsn1Header::SerializeNistLogicalChannelConfig (NistLteRrcSap::NistLogicalChannelConfig logicalChannelConfig) const
{
  // Serialize NistLogicalChannelConfig sequence
  // 1 optional field (ul-SpecificParameters), which is present. Extension marker present.
  SerializeSequence (std::bitset<1> (1),true);

  // Serialize ul-SpecificParameters sequence
  // 1 optional field (logicalChannelGroup), which is present. No extension marker.
  SerializeSequence (std::bitset<1> (1),false);

  // Serialize priority ::= INTEGER (1..16)
  SerializeInteger (logicalChannelConfig.priority,1,16);

  // Serialize prioritisedBitRate
  int prioritizedBitRate;
  switch (logicalChannelConfig.prioritizedBitRateKbps)
    {
    case 0:
      prioritizedBitRate = 0;
      break;
    case 8:
      prioritizedBitRate = 1;
      break;
    case 16:
      prioritizedBitRate = 2;
      break;
    case 32:
      prioritizedBitRate = 3;
      break;
    case 64:
      prioritizedBitRate = 4;
      break;
    case 128:
      prioritizedBitRate = 5;
      break;
    case 256:
      prioritizedBitRate = 6;
      break;
    default:
      prioritizedBitRate = 7;          // Infinity
    }
  SerializeEnum (16,prioritizedBitRate);

  // Serialize bucketSizeDuration
  int bucketSizeDuration;
  switch (logicalChannelConfig.bucketSizeDurationMs)
    {
    case 50:
      bucketSizeDuration = 0;
      break;
    case 100:
      bucketSizeDuration = 1;
      break;
    case 150:
      bucketSizeDuration = 2;
      break;
    case 300:
      bucketSizeDuration = 3;
      break;
    case 500:
      bucketSizeDuration = 4;
      break;
    case 1000:
      bucketSizeDuration = 5;
      break;
    default:
      bucketSizeDuration = 5;
    }
  SerializeEnum (8,bucketSizeDuration);

  // Serialize logicalChannelGroup ::= INTEGER (0..3)
  SerializeInteger (logicalChannelConfig.logicalChannelGroup,0,3);
}

void
NistRrcAsn1Header::SerializeNistPhysicalConfigDedicated (NistLteRrcSap::NistPhysicalConfigDedicated physicalConfigDedicated) const
{
  // Serialize NistPhysicalConfigDedicated Sequence
  std::bitset<10> optionalFieldsNistPhysicalConfigDedicated;
  optionalFieldsNistPhysicalConfigDedicated.set (9,physicalConfigDedicated.haveNistPdschConfigDedicated);  // pdsch-ConfigDedicated
  optionalFieldsNistPhysicalConfigDedicated.set (8,0);  // pucch-ConfigDedicated not present
  optionalFieldsNistPhysicalConfigDedicated.set (7,0);  // pusch-ConfigDedicated not present
  optionalFieldsNistPhysicalConfigDedicated.set (6,0);  // uplinkPowerControlDedicated not present
  optionalFieldsNistPhysicalConfigDedicated.set (5,0);  // tpc-PDCCH-ConfigPUCCH not present
  optionalFieldsNistPhysicalConfigDedicated.set (4,0);  // tpc-PDCCH-ConfigPUSCH not present
  optionalFieldsNistPhysicalConfigDedicated.set (3,0);  // cqi-ReportConfig not present
  optionalFieldsNistPhysicalConfigDedicated.set (2,physicalConfigDedicated.haveNistSoundingRsUlConfigDedicated);  // soundingRS-UL-ConfigDedicated
  optionalFieldsNistPhysicalConfigDedicated.set (1,physicalConfigDedicated.haveNistAntennaInfoDedicated);  // antennaInfo
  optionalFieldsNistPhysicalConfigDedicated.set (0,0);  // schedulingRequestConfig not present
  SerializeSequence (optionalFieldsNistPhysicalConfigDedicated,true);

  if (physicalConfigDedicated.haveNistPdschConfigDedicated)
    {
      // Serialize Pdsch-ConfigDedicated Sequence:
      // 0 optional / default fields, no extension marker.
      SerializeSequence (std::bitset<0> (),false);

      // Serialize  p-a
      // Assuming the value in the struct is the enum index
      SerializeEnum (8,physicalConfigDedicated.pdschConfigDedicated.pa);

      // Serialize release
      SerializeNull ();
    }

  if (physicalConfigDedicated.haveNistSoundingRsUlConfigDedicated)
    {
      // Serialize SoundingRS-UL-ConfigDedicated choice:
      switch (physicalConfigDedicated.soundingRsUlConfigDedicated.type)
        {
        case NistLteRrcSap::NistSoundingRsUlConfigDedicated::RESET:
          SerializeChoice (2,0,false);
          SerializeNull ();
          break;

        case NistLteRrcSap::NistSoundingRsUlConfigDedicated::SETUP:
        default:
          // 2 options, selected: 1 (setup)
          SerializeChoice (2,1,false);

          // Serialize setup sequence
          // 0 optional / default fields, no extension marker.
          SerializeSequence (std::bitset<0> (),false);

          // Serialize srs-Bandwidth
          SerializeEnum (4,physicalConfigDedicated.soundingRsUlConfigDedicated.srsBandwidth);

          // Serialize  srs-HoppingBandwidth
          SerializeEnum (4,0);

          // Serialize freqDomainPosition
          SerializeInteger (0,0,23);

          // Serialize duration
          SerializeBoolean (false);

          // Serialize srs-ConfigIndex
          SerializeInteger (physicalConfigDedicated.soundingRsUlConfigDedicated.srsConfigIndex,0,1023);

          // Serialize transmissionComb
          SerializeInteger (0,0,1);

          // Serialize cyclicShift
          SerializeEnum (8,0);

          break;
        }
    }

  if (physicalConfigDedicated.haveNistAntennaInfoDedicated)
    {
      // Serialize antennaInfo choice
      // 2 options. Selected: 0 ("explicitValue" of type "NistAntennaInfoDedicated")
      SerializeChoice (2,0,false);

      // Serialize NistAntennaInfoDedicated sequence
      // 1 optional parameter, not present. No extension marker.
      SerializeSequence (std::bitset<1> (0),false);

      // Serialize transmissionMode
      // Assuming the value in the struct is the enum index
      SerializeEnum (8,physicalConfigDedicated.antennaInfo.transmissionMode);

      // Serialize ue-TransmitAntennaSelection choice
      SerializeChoice (2,0,false);

      // Serialize release
      SerializeNull ();
    }
}

void
NistRrcAsn1Header::SerializeNistRadioResourceConfigDedicated (NistLteRrcSap::NistRadioResourceConfigDedicated radioResourceConfigDedicated) const
{
  bool isNistSrbToAddModListPresent = !radioResourceConfigDedicated.srbToAddModList.empty ();
  bool isNistDrbToAddModListPresent = !radioResourceConfigDedicated.drbToAddModList.empty ();
  bool isDrbToReleaseListPresent = !radioResourceConfigDedicated.drbToReleaseList.empty ();

  // 6 optional fields. Extension marker is present.
  std::bitset<6> optionalFieldsPresent = std::bitset<6> ();
  optionalFieldsPresent.set (5,isNistSrbToAddModListPresent);  // srb-ToAddModList present
  optionalFieldsPresent.set (4,isNistDrbToAddModListPresent);  // drb-ToAddModList present
  optionalFieldsPresent.set (3,isDrbToReleaseListPresent);  // drb-ToReleaseList present
  optionalFieldsPresent.set (2,0);  // mac-MainConfig not present
  optionalFieldsPresent.set (1,0);  // sps-Config not present
  optionalFieldsPresent.set (0,(radioResourceConfigDedicated.haveNistPhysicalConfigDedicated) ? 1 : 0);
  SerializeSequence (optionalFieldsPresent,true);

  // Serialize srbToAddModList
  if (isNistSrbToAddModListPresent)
    {
      SerializeNistSrbToAddModList (radioResourceConfigDedicated.srbToAddModList);
    }

  // Serialize drbToAddModList
  if (isNistDrbToAddModListPresent)
    {
      SerializeNistDrbToAddModList (radioResourceConfigDedicated.drbToAddModList);
    }

  // Serialize drbToReleaseList
  if (isDrbToReleaseListPresent)
    {
      SerializeSequenceOf (radioResourceConfigDedicated.drbToReleaseList.size (),MAX_DRB,1);
      std::list<uint8_t>::iterator it = radioResourceConfigDedicated.drbToReleaseList.begin ();
      for (; it != radioResourceConfigDedicated.drbToReleaseList.end (); it++)
        {
          // DRB-Identity ::= INTEGER (1..32)
          SerializeInteger (*it,1,32);
        }
    }

  if (radioResourceConfigDedicated.haveNistPhysicalConfigDedicated)
    {
      SerializeNistPhysicalConfigDedicated (radioResourceConfigDedicated.physicalConfigDedicated);
    }
}

void
NistRrcAsn1Header::SerializeNistSystemInformationBlockType1 (NistLteRrcSap::NistSystemInformationBlockType1 systemInformationBlockType1) const
{
  // 3 optional fields, no extension marker.
  std::bitset<3> sysInfoBlk1Opts;
  sysInfoBlk1Opts.set (2,0); // p-Max absent
  sysInfoBlk1Opts.set (1,0); // tdd-Config absent
  sysInfoBlk1Opts.set (0,0); // nonCriticalExtension absent
  SerializeSequence (sysInfoBlk1Opts,false);

  // Serialize cellAccessRelatedInfo
  // 1 optional field (csgIdentity) which is present, no extension marker.
  SerializeSequence (std::bitset<1> (1),false);

  // Serialize plmn-IdentityList
  SerializeSequenceOf (1,6,1);

  // PLMN-IdentityInfo 
  SerializeSequence (std::bitset<0> (),false);

  SerializePlmnIdentity (systemInformationBlockType1.cellAccessRelatedInfo.plmnIdentityInfo.plmnIdentity);

  // Serialize trackingAreaCode
  SerializeBitstring (std::bitset<16> (0));
  // Serialize cellIdentity
  SerializeBitstring (std::bitset<28> (systemInformationBlockType1.cellAccessRelatedInfo.cellIdentity));
  // Serialize cellBarred
  SerializeEnum (2,0);
  // Serialize intraFreqReselection
  SerializeEnum (2,0);
  // Serialize csg-Indication
  SerializeBoolean (systemInformationBlockType1.cellAccessRelatedInfo.csgIndication);
  // Serialize csg-Identity
  SerializeBitstring (std::bitset<27> (systemInformationBlockType1.cellAccessRelatedInfo.csgIdentity));

  // Serialize cellSelectionInfo
  SerializeSequence (std::bitset<1> (0),false);
  // Serialize q-RxLevMin
  SerializeInteger (-50,-70,-22);

  // Serialize freqBandIndicator
  SerializeInteger (1,1,64);

  // Serialize schedulingInfoList
  SerializeSequenceOf (1,MAX_SI_MESSAGE,1);
  // SchedulingInfo
  SerializeSequence (std::bitset<0> (),false);
  // si-Periodicity
  SerializeEnum (7,0);
  // sib-MappingInfo
  SerializeSequenceOf (0,MAX_SIB - 1,0);

  // Serialize si-WindowLength
  SerializeEnum (7,0);

  // Serialize systemInfoValueTag
  SerializeInteger (0,0,31);
}

void
NistRrcAsn1Header::SerializeNistRadioResourceConfigCommon (NistLteRrcSap::NistRadioResourceConfigCommon radioResourceConfigCommon) const
{
  // 9 optional fields. Extension marker yes.
  std::bitset<9> rrCfgCmmOpts;
  rrCfgCmmOpts.set (8,1); // rach-ConfigCommon is present
  rrCfgCmmOpts.set (7,0); // pdsch-ConfigCommon not present
  rrCfgCmmOpts.set (6,0); // phich-Config not present
  rrCfgCmmOpts.set (5,0); // pucch-ConfigCommon  not present
  rrCfgCmmOpts.set (4,0); // soundingRS-UL-ConfigCommon not present
  rrCfgCmmOpts.set (3,0); // uplinkPowerControlCommon not present
  rrCfgCmmOpts.set (2,0); // antennaInfoCommon not present
  rrCfgCmmOpts.set (1,0); // p-Max not present
  rrCfgCmmOpts.set (0,0); // tdd-Config not present

  SerializeSequence (rrCfgCmmOpts,true);

  if (rrCfgCmmOpts[8])
    {
      // Serialize RACH-ConfigCommon
      SerializeNistRachConfigCommon (radioResourceConfigCommon.rachConfigCommon);
    }

  // Serialize PRACH-Config
  // 1 optional, 0 extension marker.
  SerializeSequence (std::bitset<1> (0),false);

  // Serialize PRACH-Config rootSequenceIndex
  SerializeInteger (0,0,1023);

  // Serialize PUSCH-ConfigCommon
  SerializeSequence (std::bitset<0> (),false);

  // Serialize pusch-ConfigBasic
  SerializeSequence (std::bitset<0> (),false);
  SerializeInteger (1,1,4);
  SerializeEnum (2,0);
  SerializeInteger (0,0,98);
  SerializeBoolean (false);

  // Serialize UL-ReferenceSignalsPUSCH
  SerializeSequence (std::bitset<0> (),false);
  SerializeBoolean (false);
  SerializeInteger (0,0,29);
  SerializeBoolean (false);
  SerializeInteger (4,0,7);

  // Serialize UL-CyclicPrefixLength
  SerializeEnum (2,0);
}

void
NistRrcAsn1Header::SerializeNistRadioResourceConfigCommonSib (NistLteRrcSap::NistRadioResourceConfigCommonSib radioResourceConfigCommonSib) const
{
  SerializeSequence (std::bitset<0> (0),true);

  // rach-ConfigCommon
  SerializeNistRachConfigCommon (radioResourceConfigCommonSib.rachConfigCommon);

  // bcch-Config 
  SerializeSequence (std::bitset<0> (0),false);
  SerializeEnum (4,0); // modificationPeriodCoeff
  // pcch-Config 
  SerializeSequence (std::bitset<0> (0),false);
  SerializeEnum (4,0); // defaultPagingCycle
  SerializeEnum (8,0); // nB
  // prach-Config 
  SerializeSequence (std::bitset<1> (0),false);
  SerializeInteger (0,0,1023); // rootSequenceIndex
  // pdsch-ConfigCommon 
  SerializeSequence (std::bitset<0> (0),false);
  SerializeInteger (0,-60,50); // referenceSignalPower
  SerializeInteger (0,0,3); // p-b
  // pusch-ConfigCommon 
  SerializeSequence (std::bitset<0> (0),false);
  SerializeSequence (std::bitset<0> (0),false); // pusch-ConfigBasic
  SerializeInteger (1,1,4); // n-SB
  SerializeEnum (2,0); // hoppingMode
  SerializeInteger (0,0,98); // pusch-HoppingOffset
  SerializeBoolean (false); // enable64QAM
  SerializeSequence (std::bitset<0> (0),false); // UL-ReferenceSignalsPUSCH
  SerializeBoolean (false); // groupHoppingEnabled
  SerializeInteger (0,0,29); // groupAssignmentPUSCH
  SerializeBoolean (false); // sequenceHoppingEnabled
  SerializeInteger (0,0,7); // cyclicShift
  // pucch-ConfigCommon 
  SerializeSequence (std::bitset<0> (0),false);
  SerializeEnum (3,0); // deltaPUCCH-Shift
  SerializeInteger (0,0,98); // nRB-CQI
  SerializeInteger (0,0,7); // nCS-AN
  SerializeInteger (0,0,2047); // n1PUCCH-AN
  // soundingRS-UL-ConfigCommon 
  SerializeChoice (2,0,false);
  SerializeNull (); // release
  // uplinkPowerControlCommon 
  SerializeSequence (std::bitset<0> (0),false);
  SerializeInteger (0,-126,24); // p0-NominalPUSCH
  SerializeEnum (8,0); // alpha
  SerializeInteger (-50,-127,-96); // p0-NominalPUCCH
  SerializeSequence (std::bitset<0> (0),false); // deltaFList-PUCCH
  SerializeEnum (3,0); // deltaF-PUCCH-Format1
  SerializeEnum (3,0); // deltaF-PUCCH-Format1b
  SerializeEnum (4,0); // deltaF-PUCCH-Format2
  SerializeEnum (3,0); // deltaF-PUCCH-Format2a
  SerializeEnum (3,0); // deltaF-PUCCH-Format2b
  SerializeInteger (0,-1,6);
  // ul-CyclicPrefixLength 
  SerializeEnum (2,0);
}

void
NistRrcAsn1Header::SerializeNistSystemInformationBlockType2 (NistLteRrcSap::NistSystemInformationBlockType2 systemInformationBlockType2) const
{
  SerializeSequence (std::bitset<2> (0),true);

  // NistRadioResourceConfigCommonSib
  SerializeNistRadioResourceConfigCommonSib (systemInformationBlockType2.radioResourceConfigCommon);

  // ue-TimersAndConstants
  SerializeSequence (std::bitset<0> (0),true);
  SerializeEnum (8,0); // t300
  SerializeEnum (8,0); // t301
  SerializeEnum (7,0); // t310
  SerializeEnum (8,0); // n310
  SerializeEnum (7,0); // t311
  SerializeEnum (8,0); // n311

  // freqInfo
  SerializeSequence (std::bitset<2> (3),false);
  SerializeInteger ((int) systemInformationBlockType2.freqInfo.ulCarrierFreq, 0, MAX_EARFCN);
  switch (systemInformationBlockType2.freqInfo.ulBandwidth)
    {
    case 6:
      SerializeEnum (6,0);
      break;
    case 15:
      SerializeEnum (6,1);
      break;
    case 25:
      SerializeEnum (6,2);
      break;
    case 50:
      SerializeEnum (6,3);
      break;
    case 75:
      SerializeEnum (6,4);
      break;
    case 100:
      SerializeEnum (6,5);
      break;
    default:
      SerializeEnum (6,0);
    }

  SerializeInteger (29,1,32); // additionalSpectrumEmission
  // timeAlignmentTimerCommon
  SerializeEnum (8,0);
}

void
NistRrcAsn1Header::SerializeNistMeasResults (NistLteRrcSap::NistMeasResults measResults) const
{
  // Watchdog: if list has 0 elements, set boolean to false
  if (measResults.measResultListEutra.empty ())
    {
      measResults.haveMeasResultNeighCells = false;
    }

  // Serialize NistMeasResults sequence, 1 optional value, extension marker present
  SerializeSequence (std::bitset<1> (measResults.haveMeasResultNeighCells),true);

  // Serialize measId
  SerializeInteger (measResults.measId,1,MAX_MEAS_ID);

  // Serialize measResultServCell sequence
  SerializeSequence (std::bitset<0> (0),false);

  // Serialize rsrpResult
  SerializeInteger (measResults.rsrpResult,0,97);

  // Serialize rsrqResult
  SerializeInteger (measResults.rsrqResult,0,34);

  if (measResults.haveMeasResultNeighCells)
    {
      // Serialize Choice = 0 (MeasResultListEUTRA)
      SerializeChoice (4,0,false);

      // Serialize measResultNeighCells
      SerializeSequenceOf (measResults.measResultListEutra.size (),MAX_CELL_REPORT,1);

      // serialize NistMeasResultEutra elements in the list
      std::list<NistLteRrcSap::NistMeasResultEutra>::iterator it;
      for (it = measResults.measResultListEutra.begin (); it != measResults.measResultListEutra.end (); it++)
        {
          SerializeSequence (std::bitset<1> (it->haveNistCgiInfo),false);

          // Serialize PhysCellId
          SerializeInteger (it->physCellId, 0, 503);

          // Serialize NistCgiInfo
          if (it->haveNistCgiInfo)
            {
              SerializeSequence (std::bitset<1> (it->cgiInfo.plmnIdentityList.size ()),false);

              // Serialize cellGlobalId
              SerializeSequence (std::bitset<0> (0),false);
              SerializePlmnIdentity (it->cgiInfo.plmnIdentity);
              SerializeBitstring (std::bitset<28> (it->cgiInfo.cellIdentity));

              // Serialize trackingAreaCode
              SerializeBitstring (std::bitset<16> (it->cgiInfo.trackingAreaCode));

              // Serialize plmn-IdentityList
              if (!it->cgiInfo.plmnIdentityList.empty ())
                {
                  SerializeSequenceOf (it->cgiInfo.plmnIdentityList.size (),5,1);
                  std::list<uint32_t>::iterator it2;
                  for (it2 = it->cgiInfo.plmnIdentityList.begin (); it2 != it->cgiInfo.plmnIdentityList.end (); it2++)
                    {
                      SerializePlmnIdentity (*it2);
                    }
                }
            }

          // Serialize measResult
          std::bitset<2> measResultFieldsPresent;
          measResultFieldsPresent[1] = it->haveRsrpResult;
          measResultFieldsPresent[0] = it->haveRsrqResult;
          SerializeSequence (measResultFieldsPresent,true);

          if (it->haveRsrpResult)
            {
              SerializeInteger (it->rsrpResult,0,97);
            }

          if (it->haveRsrqResult)
            {
              SerializeInteger (it->rsrqResult,0,34);
            }
        }
    }
}

void
NistRrcAsn1Header::SerializePlmnIdentity (uint32_t plmnId) const
{
  // plmn-Identity sequence, mcc is optional, no extension marker
  SerializeSequence (std::bitset<1> (0), false);

  // Serialize mnc
  int nDig = (plmnId > 99) ? 3 : 2;

  SerializeSequenceOf (nDig,3,2);
  for (int i = nDig - 1; i >= 0; i--)
    {
      int n = floor (plmnId / pow (10,i));
      SerializeInteger (n,0,9);
      plmnId -= n * pow (10,i);
    }

  // cellReservedForOperatorUse 
  SerializeEnum (2,0);
}

void 
NistRrcAsn1Header::SerializeNistRachConfigCommon (NistLteRrcSap::NistRachConfigCommon rachConfigCommon) const
{
  // rach-ConfigCommon
  SerializeSequence (std::bitset<0> (0),true);

  // preambleInfo
  SerializeSequence (std::bitset<1> (0),false);

  // numberOfRA-Preambles
  switch (rachConfigCommon.preambleInfo.numberOfRaPreambles)
    {
    case 4:
      SerializeEnum (16,0);
      break;
    case 8:
      SerializeEnum (16,1);
      break;
    case 12:
      SerializeEnum (16,2);
      break;
    case 16:
      SerializeEnum (16,3);
      break;
    case 20:
      SerializeEnum (16,4);
      break;
    case 24:
      SerializeEnum (16,5);
      break;
    case 28:
      SerializeEnum (16,6);
      break;
    case 32:
      SerializeEnum (16,7);
      break;
    case 36:
      SerializeEnum (16,8);
      break;
    case 40:
      SerializeEnum (16,9);
      break;
    case 44:
      SerializeEnum (16,10);
      break;
    case 48:
      SerializeEnum (16,11);
      break;
    case 52:
      SerializeEnum (16,12);
      break;
    case 56:
      SerializeEnum (16,13);
      break;
    case 60:
      SerializeEnum (16,14);
      break;
    case 64:
      SerializeEnum (16,15);
      break;
    default:
      SerializeEnum (16,0);
    }

  SerializeSequence (std::bitset<0> (0),false); // powerRampingParameters
  SerializeEnum (4,0); // powerRampingStep
  SerializeEnum (16,0); // preambleInitialReceivedTargetPower
  SerializeSequence (std::bitset<0> (0),false); // ra-SupervisionInfo

  // preambleTransMax
  switch (rachConfigCommon.raSupervisionInfo.preambleTransMax)
    {
    case 3:
      SerializeEnum (11,0);
      break;
    case 4:
      SerializeEnum (11,1);
      break;
    case 5:
      SerializeEnum (11,2);
      break;
    case 6:
      SerializeEnum (11,3);
      break;
    case 7:
      SerializeEnum (11,4);
      break;
    case 8:
      SerializeEnum (11,5);
      break;
    case 10:
      SerializeEnum (11,6);
      break;
    case 20:
      SerializeEnum (11,7);
      break;
    case 50:
      SerializeEnum (11,8);
      break;
    case 100:
      SerializeEnum (11,9);
      break;
    case 200:
      SerializeEnum (11,10);
      break;
    default:
      SerializeEnum (11,0);
    }

  // ra-ResponseWindowSize
  switch (rachConfigCommon.raSupervisionInfo.raResponseWindowSize)
    {
    case 2:
      SerializeEnum (8,0);
      break;
    case 3:
      SerializeEnum (8,1);
      break;
    case 4:
      SerializeEnum (8,2);
      break;
    case 5:
      SerializeEnum (8,3);
      break;
    case 6:
      SerializeEnum (8,4);
      break;
    case 7:
      SerializeEnum (8,5);
      break;
    case 8:
      SerializeEnum (8,6);
      break;
    case 10:
      SerializeEnum (8,7);
      break;
    default:
      SerializeEnum (8,0);
    }

  SerializeEnum (8,0); // mac-ContentionResolutionTimer
  SerializeInteger (1,1,8); // maxHARQ-Msg3Tx
}

void
NistRrcAsn1Header::SerializeQoffsetRange (int8_t qOffsetRange) const
{
  switch (qOffsetRange)
    {
    case -24:
      SerializeEnum (31,0);
      break;
    case -22:
      SerializeEnum (31,1);
      break;
    case -20:
      SerializeEnum (31,2);
      break;
    case -18:
      SerializeEnum (31,3);
      break;
    case -16:
      SerializeEnum (31,4);
      break;
    case -14:
      SerializeEnum (31,5);
      break;
    case -12:
      SerializeEnum (31,6);
      break;
    case -10:
      SerializeEnum (31,7);
      break;
    case -8:
      SerializeEnum (31,8);
      break;
    case -6:
      SerializeEnum (31,9);
      break;
    case -5:
      SerializeEnum (31,10);
      break;
    case -4:
      SerializeEnum (31,11);
      break;
    case -3:
      SerializeEnum (31,12);
      break;
    case -2:
      SerializeEnum (31,13);
      break;
    case -1:
      SerializeEnum (31,14);
      break;
    case 0:
      SerializeEnum (31,15);
      break;
    case 1:
      SerializeEnum (31,16);
      break;
    case 2:
      SerializeEnum (31,17);
      break;
    case 3:
      SerializeEnum (31,18);
      break;
    case 4:
      SerializeEnum (31,19);
      break;
    case 5:
      SerializeEnum (31,20);
      break;
    case 6:
      SerializeEnum (31,21);
      break;
    case 8:
      SerializeEnum (31,22);
      break;
    case 10:
      SerializeEnum (31,23);
      break;
    case 12:
      SerializeEnum (31,24);
      break;
    case 14:
      SerializeEnum (31,25);
      break;
    case 16:
      SerializeEnum (31,26);
      break;
    case 18:
      SerializeEnum (31,27);
      break;
    case 20:
      SerializeEnum (31,28);
      break;
    case 22:
      SerializeEnum (31,29);
      break;
    case 24:
      SerializeEnum (31,30);
      break;
    default:
      SerializeEnum (31,15);
    }
}

void
NistRrcAsn1Header::SerializeNistThresholdEutra (NistLteRrcSap::NistThresholdEutra thresholdEutra) const
{
  switch (thresholdEutra.choice)
    {
    case NistLteRrcSap::NistThresholdEutra::THRESHOLD_RSRP:
      SerializeChoice (2,0,false);
      SerializeInteger (thresholdEutra.range, 0, 97);
      break;
    case NistLteRrcSap::NistThresholdEutra::THRESHOLD_RSRQ:
    default:
      SerializeChoice (2,1,false);
      SerializeInteger (thresholdEutra.range, 0, 34);
    }
}

void
NistRrcAsn1Header::SerializeNistMeasConfig (NistLteRrcSap::NistMeasConfig measConfig) const
{
  // Serialize NistMeasConfig sequence
  // 11 optional fields, extension marker present
  std::bitset<11> measConfigOptional;
  measConfigOptional.set (10, !measConfig.measObjectToRemoveList.empty () );
  measConfigOptional.set (9, !measConfig.measObjectToAddModList.empty () );
  measConfigOptional.set (8, !measConfig.reportConfigToRemoveList.empty () );
  measConfigOptional.set (7, !measConfig.reportConfigToAddModList.empty () );
  measConfigOptional.set (6, !measConfig.measIdToRemoveList.empty () );
  measConfigOptional.set (5, !measConfig.measIdToAddModList.empty () );
  measConfigOptional.set (4, measConfig.haveNistQuantityConfig ); 
  measConfigOptional.set (3, measConfig.haveNistMeasGapConfig ); 
  measConfigOptional.set (2, measConfig.haveSmeasure ); 
  measConfigOptional.set (1, false ); // preRegistrationInfoHRPD
  measConfigOptional.set (0, measConfig.haveNistSpeedStatePars ); 
  SerializeSequence (measConfigOptional,true);

  if (!measConfig.measObjectToRemoveList.empty ())
    {
      SerializeSequenceOf (measConfig.measObjectToRemoveList.size (),MAX_OBJECT_ID,1);
      for (std::list<uint8_t>::iterator it = measConfig.measObjectToRemoveList.begin (); it != measConfig.measObjectToRemoveList.end (); it++)
        {
          SerializeInteger (*it, 1, MAX_OBJECT_ID);
        }
    }

  if (!measConfig.measObjectToAddModList.empty ())
    {
      SerializeSequenceOf (measConfig.measObjectToAddModList.size (),MAX_OBJECT_ID,1);
      for (std::list<NistLteRrcSap::NistMeasObjectToAddMod>::iterator it = measConfig.measObjectToAddModList.begin (); it != measConfig.measObjectToAddModList.end (); it++)
        {
          SerializeSequence (std::bitset<0> (), false);
          SerializeInteger (it->measObjectId, 1, MAX_OBJECT_ID);
          SerializeChoice (4, 0, true); // Select MeasObjectEUTRA

          // Serialize measObjectEutra
          std::bitset<5> measObjOpts;
          measObjOpts.set (4,!it->measObjectEutra.cellsToRemoveList.empty () );
          measObjOpts.set (3,!it->measObjectEutra.cellsToAddModList.empty () );
          measObjOpts.set (2,!it->measObjectEutra.blackCellsToRemoveList.empty () );
          measObjOpts.set (1,!it->measObjectEutra.blackNistCellsToAddModList.empty () );
          measObjOpts.set (0,it->measObjectEutra.haveCellForWhichToReportCGI);
          SerializeSequence (measObjOpts, true);

          // Serialize carrierFreq
          SerializeInteger (it->measObjectEutra.carrierFreq, 0, MAX_EARFCN);

          // Serialize  allowedMeasBandwidth
          switch (it->measObjectEutra.allowedMeasBandwidth)
            {
            case 6:
              SerializeEnum (6,0);
              break;
            case 15:
              SerializeEnum (6,1);
              break;
            case 25:
              SerializeEnum (6,2);
              break;
            case 50:
              SerializeEnum (6,3);
              break;
            case 75:
              SerializeEnum (6,4);
              break;
            case 100:
              SerializeEnum (6,5);
              break;
            default:
              SerializeEnum (6,0);
            }

          SerializeBoolean (it->measObjectEutra.presenceAntennaPort1);
          SerializeBitstring (std::bitset<2> (it->measObjectEutra.neighCellConfig));
          SerializeQoffsetRange (it->measObjectEutra.offsetFreq);

          if (!it->measObjectEutra.cellsToRemoveList.empty ())
            {
              SerializeSequenceOf (it->measObjectEutra.cellsToRemoveList.size (),MAX_CELL_MEAS,1);
              for (std::list<uint8_t>::iterator it2 = it->measObjectEutra.cellsToRemoveList.begin (); it2 != it->measObjectEutra.cellsToRemoveList.end (); it2++)
                {
                  SerializeInteger (*it2, 1, MAX_CELL_MEAS);
                }
            }

          if (!it->measObjectEutra.cellsToAddModList.empty ())
            {
              SerializeSequenceOf (it->measObjectEutra.cellsToAddModList.size (), MAX_CELL_MEAS, 1);
              for (std::list<NistLteRrcSap::NistCellsToAddMod>::iterator it2 = it->measObjectEutra.cellsToAddModList.begin (); it2 != it->measObjectEutra.cellsToAddModList.end (); it2++)
                {
                  SerializeSequence (std::bitset<0> (), false);

                  // Serialize cellIndex
                  SerializeInteger (it2->cellIndex, 1, MAX_CELL_MEAS);

                  // Serialize PhysCellId
                  SerializeInteger (it2->physCellId,0,503);

                  // Serialize cellIndividualOffset
                  SerializeQoffsetRange (it2->cellIndividualOffset);
                }
            }

          if (!it->measObjectEutra.blackCellsToRemoveList.empty () )
            {
              SerializeSequenceOf (it->measObjectEutra.blackCellsToRemoveList.size (),MAX_CELL_MEAS,1);
              for (std::list<uint8_t>::iterator it2 = it->measObjectEutra.blackCellsToRemoveList.begin (); it2 != it->measObjectEutra.blackCellsToRemoveList.end (); it2++)
                {
                  SerializeInteger (*it2, 1, MAX_CELL_MEAS);
                }
            }

          if (!it->measObjectEutra.blackNistCellsToAddModList.empty () )
            {
              SerializeSequenceOf (it->measObjectEutra.blackNistCellsToAddModList.size (), MAX_CELL_MEAS, 1);
              for (std::list<NistLteRrcSap::BlackNistCellsToAddMod>::iterator it2 = it->measObjectEutra.blackNistCellsToAddModList.begin (); it2 != it->measObjectEutra.blackNistCellsToAddModList.end (); it2++)
                {
                  SerializeSequence (std::bitset<0> (),false);
                  SerializeInteger (it2->cellIndex, 1, MAX_CELL_MEAS);

                  // Serialize NistPhysCellIdRange
                  // range optional
                  std::bitset<1> rangePresent = std::bitset<1> (it2->physCellIdRange.haveRange);
                  SerializeSequence (rangePresent,false);
                  SerializeInteger (it2->physCellIdRange.start,0,503);
                  if (it2->physCellIdRange.haveRange)
                    {
                      switch (it2->physCellIdRange.range)
                        {
                        case 4:
                          SerializeEnum (16, 0);
                          break;
                        case 8:
                          SerializeEnum (16, 1);
                          break;
                        case 12:
                          SerializeEnum (16, 2);
                          break;
                        case 16:
                          SerializeEnum (16, 3);
                          break;
                        case 24:
                          SerializeEnum (16, 4);
                          break;
                        case 32:
                          SerializeEnum (16, 5);
                          break;
                        case 48:
                          SerializeEnum (16, 6);
                          break;
                        case 64:
                          SerializeEnum (16, 7);
                          break;
                        case 84:
                          SerializeEnum (16, 8);
                          break;
                        case 96:
                          SerializeEnum (16, 9);
                          break;
                        case 128:
                          SerializeEnum (16, 10);
                          break;
                        case 168:
                          SerializeEnum (16, 11);
                          break;
                        case 252:
                          SerializeEnum (16, 12);
                          break;
                        case 504:
                          SerializeEnum (16, 13);
                          break;
                        default:
                          SerializeEnum (16, 0);
                        }
                    }
                }

            }

          if (it->measObjectEutra.haveCellForWhichToReportCGI)
            {
              SerializeInteger (it->measObjectEutra.cellForWhichToReportCGI,0,503);
            }
        }
    }


  if (!measConfig.reportConfigToRemoveList.empty () )
    {
      SerializeSequenceOf (measConfig.reportConfigToRemoveList.size (),MAX_REPORT_CONFIG_ID,1);
      for (std::list<uint8_t>::iterator it = measConfig.reportConfigToRemoveList.begin (); it != measConfig.reportConfigToRemoveList.end (); it++)
        {
          SerializeInteger (*it, 1,MAX_REPORT_CONFIG_ID);
        }
    }

  if (!measConfig.reportConfigToAddModList.empty () )
    {
      SerializeSequenceOf (measConfig.reportConfigToAddModList.size (),MAX_REPORT_CONFIG_ID,1);
      for (std::list<NistLteRrcSap::NistReportConfigToAddMod>::iterator it = measConfig.reportConfigToAddModList.begin (); it != measConfig.reportConfigToAddModList.end (); it++)
        {
          SerializeSequence (std::bitset<0> (), false);
          SerializeInteger (it->reportConfigId,1,MAX_REPORT_CONFIG_ID);
          SerializeChoice (2,0,false); // reportConfigEUTRA

          // Serialize ReportConfigEUTRA
          SerializeSequence (std::bitset<0> (), true);
          switch (it->reportConfigEutra.triggerType)
            {
            case NistLteRrcSap::NistReportConfigEutra::PERIODICAL:
              SerializeChoice (2, 1, false); 
              SerializeSequence (std::bitset<0> (),false);
              switch (it->reportConfigEutra.purpose)
                {
                case NistLteRrcSap::NistReportConfigEutra::REPORT_CGI:
                  SerializeEnum (2,1);
                  break;
                case NistLteRrcSap::NistReportConfigEutra::REPORT_STRONGEST_CELLS:
                default:
                  SerializeEnum (2,0);
                }
              break;
            case NistLteRrcSap::NistReportConfigEutra::EVENT:
            default: 
              SerializeChoice (2, 0, false);
              SerializeSequence (std::bitset<0> (),false);
              switch (it->reportConfigEutra.eventId)
                {
                case NistLteRrcSap::NistReportConfigEutra::EVENT_A1:
                  SerializeChoice (5, 0, true);
                  SerializeSequence (std::bitset<0> (),false);
                  SerializeNistThresholdEutra (it->reportConfigEutra.threshold1);
                  break;
                case NistLteRrcSap::NistReportConfigEutra::EVENT_A2:
                  SerializeChoice (5, 1, true);
                  SerializeSequence (std::bitset<0> (),false);
                  SerializeNistThresholdEutra (it->reportConfigEutra.threshold1);
                  break;
                case NistLteRrcSap::NistReportConfigEutra::EVENT_A3:
                  SerializeChoice (5, 2, true);
                  SerializeSequence (std::bitset<0> (),false);
                  SerializeInteger (it->reportConfigEutra.a3Offset,-30,30);
                  SerializeBoolean (it->reportConfigEutra.reportOnLeave);
                  break;
                case NistLteRrcSap::NistReportConfigEutra::EVENT_A4:
                  SerializeChoice (5, 3, true);
                  SerializeSequence (std::bitset<0> (),false);
                  SerializeNistThresholdEutra (it->reportConfigEutra.threshold1);
                  break;
                case NistLteRrcSap::NistReportConfigEutra::EVENT_A5:
                default:
                  SerializeChoice (5, 4, true);
                  SerializeSequence (std::bitset<0> (),false);
                  SerializeNistThresholdEutra (it->reportConfigEutra.threshold1);
                  SerializeNistThresholdEutra (it->reportConfigEutra.threshold2);
                }

              SerializeInteger (it->reportConfigEutra.hysteresis, 0, 30);

              switch (it->reportConfigEutra.timeToTrigger)
                {
                case 0:
                  SerializeEnum (16, 0);
                  break;
                case 40:
                  SerializeEnum (16, 1);
                  break;
                case 64:
                  SerializeEnum (16, 2);
                  break;
                case 80:
                  SerializeEnum (16, 3);
                  break;
                case 100:
                  SerializeEnum (16, 4);
                  break;
                case 128:
                  SerializeEnum (16, 5);
                  break;
                case 160:
                  SerializeEnum (16, 6);
                  break;
                case 256:
                  SerializeEnum (16, 7);
                  break;
                case 320:
                  SerializeEnum (16, 8);
                  break;
                case 480:
                  SerializeEnum (16, 9);
                  break;
                case 512:
                  SerializeEnum (16, 10);
                  break;
                case 640:
                  SerializeEnum (16, 11);
                  break;
                case 1024:
                  SerializeEnum (16, 12);
                  break;
                case 1280:
                  SerializeEnum (16, 13);
                  break;
                case 2560:
                  SerializeEnum (16, 14);
                  break;
                case 5120:
                default:
                  SerializeEnum (16, 15);
                }
            } // end trigger type

          // Serialize triggerQuantity
          if (it->reportConfigEutra.triggerQuantity == NistLteRrcSap::NistReportConfigEutra::RSRP)
            {
              SerializeEnum (2, 0);
            }
          else
            {
              SerializeEnum (2, 1);
            }

          // Serialize reportQuantity
          if (it->reportConfigEutra.reportQuantity == NistLteRrcSap::NistReportConfigEutra::SAME_AS_TRIGGER_QUANTITY)
            {
              SerializeEnum (2, 0);
            }
          else
            {
              SerializeEnum (2, 1);
            }

          // Serialize maxReportCells
          SerializeInteger (it->reportConfigEutra.maxReportCells, 1, MAX_CELL_REPORT);

          // Serialize reportInterval
          switch (it->reportConfigEutra.reportInterval)
            {
            case NistLteRrcSap::NistReportConfigEutra::MS120:
              SerializeEnum (16, 0);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MS240:
              SerializeEnum (16, 1);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MS480:
              SerializeEnum (16, 2);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MS640:
              SerializeEnum (16, 3);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MS1024:
              SerializeEnum (16, 4);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MS2048:
              SerializeEnum (16, 5);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MS5120:
              SerializeEnum (16, 6);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MS10240:
              SerializeEnum (16, 7);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MIN1:
              SerializeEnum (16, 8);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MIN6:
              SerializeEnum (16, 9);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MIN12:
              SerializeEnum (16, 10);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MIN30:
              SerializeEnum (16, 11);
              break;
            case NistLteRrcSap::NistReportConfigEutra::MIN60:
              SerializeEnum (16, 12);
              break;
            case NistLteRrcSap::NistReportConfigEutra::SPARE3:
              SerializeEnum (16, 13);
              break;
            case NistLteRrcSap::NistReportConfigEutra::SPARE2:
              SerializeEnum (16, 14);
              break;
            case NistLteRrcSap::NistReportConfigEutra::SPARE1:
            default:
              SerializeEnum (16, 15);
            }

          // Serialize reportAmount
          switch (it->reportConfigEutra.reportAmount)
            {
            case 1:
              SerializeEnum (8, 0);
              break;
            case 2:
              SerializeEnum (8, 1);
              break;
            case 4:
              SerializeEnum (8, 2);
              break;
            case 8:
              SerializeEnum (8, 3);
              break;
            case 16:
              SerializeEnum (8, 4);
              break;
            case 32:
              SerializeEnum (8, 5);
              break;
            case 64:
              SerializeEnum (8, 6);
              break;
            default:
              SerializeEnum (8, 7);
            }
        }
    }

  if (!measConfig.measIdToRemoveList.empty () )
    {
      SerializeSequenceOf (measConfig.measIdToRemoveList.size (), MAX_MEAS_ID, 1);
      for (std::list<uint8_t>::iterator it = measConfig.measIdToRemoveList.begin (); it != measConfig.measIdToRemoveList.end (); it++)
        {
          SerializeInteger (*it, 1, MAX_MEAS_ID);
        }
    }

  if (!measConfig.measIdToAddModList.empty () )
    {
      SerializeSequenceOf ( measConfig.measIdToAddModList.size (), MAX_MEAS_ID, 1);
      for (std::list<NistLteRrcSap::NistMeasIdToAddMod>::iterator it = measConfig.measIdToAddModList.begin (); it != measConfig.measIdToAddModList.end (); it++)
        {
          SerializeInteger (it->measId, 1, MAX_MEAS_ID);
          SerializeInteger (it->measObjectId, 1, MAX_OBJECT_ID);
          SerializeInteger (it->reportConfigId, 1, MAX_REPORT_CONFIG_ID);
        }
    }

  if (measConfig.haveNistQuantityConfig )
    {
      // NistQuantityConfig sequence
      // 4 optional fields, only first (EUTRA) present. Extension marker yes.
      std::bitset<4> quantityConfigOpts (0);
      quantityConfigOpts.set (3,1);
      SerializeSequence (quantityConfigOpts, true);
      SerializeSequence (std::bitset<0> (), false);

      switch (measConfig.quantityConfig.filterCoefficientRSRP)
        {
        case 0:
          SerializeEnum (16, 0);
          break;
        case 1:
          SerializeEnum (16, 1);
          break;
        case 2:
          SerializeEnum (16, 2);
          break;
        case 3:
          SerializeEnum (16, 3);
          break;
        case 4:
          SerializeEnum (16, 4);
          break;
        case 5:
          SerializeEnum (16, 5);
          break;
        case 6:
          SerializeEnum (16, 6);
          break;
        case 7:
          SerializeEnum (16, 7);
          break;
        case 8:
          SerializeEnum (16, 8);
          break;
        case 9:
          SerializeEnum (16, 9);
          break;
        case 11:
          SerializeEnum (16, 10);
          break;
        case 13:
          SerializeEnum (16, 11);
          break;
        case 15:
          SerializeEnum (16, 12);
          break;
        case 17:
          SerializeEnum (16, 13);
          break;
        case 19:
          SerializeEnum (16, 14);
          break;
        default:
          SerializeEnum (16, 4);
        }

      switch (measConfig.quantityConfig.filterCoefficientRSRQ)
        {
        case 0:
          SerializeEnum (16, 0);
          break;
        case 1:
          SerializeEnum (16, 1);
          break;
        case 2:
          SerializeEnum (16, 2);
          break;
        case 3:
          SerializeEnum (16, 3);
          break;
        case 4:
          SerializeEnum (16, 4);
          break;
        case 5:
          SerializeEnum (16, 5);
          break;
        case 6:
          SerializeEnum (16, 6);
          break;
        case 7:
          SerializeEnum (16, 7);
          break;
        case 8:
          SerializeEnum (16, 8);
          break;
        case 9:
          SerializeEnum (16, 9);
          break;
        case 11:
          SerializeEnum (16, 10);
          break;
        case 13:
          SerializeEnum (16, 11);
          break;
        case 15:
          SerializeEnum (16, 12);
          break;
        case 17:
          SerializeEnum (16, 13);
          break;
        case 19:
          SerializeEnum (16, 14);
          break;
        default:
          SerializeEnum (16, 4);
        }
    }

  if (measConfig.haveNistMeasGapConfig )
    {
      switch (measConfig.measGapConfig.type)
        {
        case NistLteRrcSap::NistMeasGapConfig::RESET:
          SerializeChoice (2, 0, false);
          SerializeNull ();
          break;
        case NistLteRrcSap::NistMeasGapConfig::SETUP:
        default:
          SerializeChoice (2, 1, false);
          SerializeSequence (std::bitset<0> (),false);
          switch (measConfig.measGapConfig.gapOffsetChoice)
            {
            case NistLteRrcSap::NistMeasGapConfig::GP0:
              SerializeChoice (2, 0, true);
              SerializeInteger (measConfig.measGapConfig.gapOffsetValue, 0, 39);
              break;
            case NistLteRrcSap::NistMeasGapConfig::GP1:
            default:
              SerializeChoice (2, 1, true);
              SerializeInteger (measConfig.measGapConfig.gapOffsetValue, 0, 79);
            }
        }
    }

  if (measConfig.haveSmeasure )
    {
      SerializeInteger (measConfig.sMeasure, 0, 97);
    }

  // ...Here preRegistrationInfoHRPD would be serialized

  if (measConfig.haveNistSpeedStatePars )
    {
      switch (measConfig.speedStatePars.type)
        {
        case NistLteRrcSap::NistSpeedStatePars::RESET:
          SerializeChoice (2, 0, false);
          SerializeNull ();
          break;
        case NistLteRrcSap::NistSpeedStatePars::SETUP:
        default:
          SerializeChoice (2, 1, false);
          SerializeSequence (std::bitset<0> (), false);
          switch (measConfig.speedStatePars.mobilityStateParameters.tEvaluation)
            {
            case 30:
              SerializeEnum (8, 0);
              break;
            case 60:
              SerializeEnum (8, 1);
              break;
            case 120:
              SerializeEnum (8, 2);
              break;
            case 180:
              SerializeEnum (8, 3);
              break;
            case 240:
              SerializeEnum (8, 4);
              break;
            default:
              SerializeEnum (8, 5);
              break;
            }

          switch (measConfig.speedStatePars.mobilityStateParameters.tHystNormal)
            {
            case 30:
              SerializeEnum (8, 0);
              break;
            case 60:
              SerializeEnum (8, 1);
              break;
            case 120:
              SerializeEnum (8, 2);
              break;
            case 180:
              SerializeEnum (8, 3);
              break;
            case 240:
              SerializeEnum (8, 4);
              break;
            default:
              SerializeEnum (8, 5);
              break;
            }

          SerializeInteger (measConfig.speedStatePars.mobilityStateParameters.nCellChangeMedium, 1, 16);
          SerializeInteger (measConfig.speedStatePars.mobilityStateParameters.nCellChangeHigh, 1, 16);

          SerializeSequence (std::bitset<0> (), false);
          switch (measConfig.speedStatePars.timeToTriggerSf.sfMedium)
            {
            case 25:
              SerializeEnum (4, 0);
              break;
            case 50:
              SerializeEnum (4, 1);
              break;
            case 75:
              SerializeEnum (4, 2);
              break;
            case 100:
            default:
              SerializeEnum (4, 3);
            }

          switch (measConfig.speedStatePars.timeToTriggerSf.sfHigh)
            {
            case 25:
              SerializeEnum (4, 0);
              break;
            case 50:
              SerializeEnum (4, 1);
              break;
            case 75:
              SerializeEnum (4, 2);
              break;
            case 100:
            default:
              SerializeEnum (4, 3);
            }
        }
    }
}

void
NistRrcAsn1Header::SerializeSidelinkUeInformation (NistLteRrcSap::SidelinkUeInformation slUeInfo) const
{

}


  
Buffer::Iterator
NistRrcAsn1Header::DeserializeNistThresholdEutra (NistLteRrcSap::NistThresholdEutra * thresholdEutra, Buffer::Iterator bIterator)
{
  int thresholdEutraChoice, range;
  bIterator = DeserializeChoice (2, false, &thresholdEutraChoice, bIterator);

  switch (thresholdEutraChoice)
    {
    case 0:
      thresholdEutra->choice = NistLteRrcSap::NistThresholdEutra::THRESHOLD_RSRP;
      bIterator = DeserializeInteger (&range, 0, 97, bIterator);
      thresholdEutra->range = range;
      break;
    case 1:
    default:
      thresholdEutra->choice = NistLteRrcSap::NistThresholdEutra::THRESHOLD_RSRQ;
      bIterator = DeserializeInteger (&range, 0, 34, bIterator);
      thresholdEutra->range = range;
    }

  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeQoffsetRange (int8_t * qOffsetRange, Buffer::Iterator bIterator)
{
  int n;
  bIterator = DeserializeEnum (31, &n, bIterator);
  switch (n)
    {
    case 0:
      *qOffsetRange = -24;
      break;
    case 1:
      *qOffsetRange = -22;
      break;
    case 2:
      *qOffsetRange = -20;
      break;
    case 3:
      *qOffsetRange = -18;
      break;
    case 4:
      *qOffsetRange = -16;
      break;
    case 5:
      *qOffsetRange = -14;
      break;
    case 6:
      *qOffsetRange = -12;
      break;
    case 7:
      *qOffsetRange = -10;
      break;
    case 8:
      *qOffsetRange = -8;
      break;
    case 9:
      *qOffsetRange = -6;
      break;
    case 10:
      *qOffsetRange = -5;
      break;
    case 11:
      *qOffsetRange = -4;
      break;
    case 12:
      *qOffsetRange = -3;
      break;
    case 13:
      *qOffsetRange = -2;
      break;
    case 14:
      *qOffsetRange = -1;
      break;
    case 15:
      *qOffsetRange = 0;
      break;
    case 16:
      *qOffsetRange = 1;
      break;
    case 17:
      *qOffsetRange = 2;
      break;
    case 18:
      *qOffsetRange = 3;
      break;
    case 19:
      *qOffsetRange = 4;
      break;
    case 20:
      *qOffsetRange = 5;
      break;
    case 21:
      *qOffsetRange = 6;
      break;
    case 22:
      *qOffsetRange = 8;
      break;
    case 23:
      *qOffsetRange = 10;
      break;
    case 24:
      *qOffsetRange = 12;
      break;
    case 25:
      *qOffsetRange = 14;
      break;
    case 26:
      *qOffsetRange = 16;
      break;
    case 27:
      *qOffsetRange = 18;
      break;
    case 28:
      *qOffsetRange = 20;
      break;
    case 29:
      *qOffsetRange = 22;
      break;
    case 30:
    default:
      *qOffsetRange = 24;
    }
  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeNistRadioResourceConfigDedicated (NistLteRrcSap::NistRadioResourceConfigDedicated *radioResourceConfigDedicated, Buffer::Iterator bIterator)
{
  // Deserialize NistRadioResourceConfigDedicated sequence
  std::bitset<6> optionalFieldsPresent = std::bitset<6> ();
  bIterator = DeserializeSequence (&optionalFieldsPresent,true,bIterator);

  if (optionalFieldsPresent[5])
    {
      // Deserialize srb-ToAddModList
      bIterator = DeserializeNistSrbToAddModList (&(radioResourceConfigDedicated->srbToAddModList),bIterator);
    }

  if (optionalFieldsPresent[4])
    {
      // Deserialize drb-ToAddModList
      bIterator = DeserializeNistDrbToAddModList (&(radioResourceConfigDedicated->drbToAddModList),bIterator);
    }

  if (optionalFieldsPresent[3])
    {
      // Deserialize drb-ToReleaseList
      int n;
      int val;
      bIterator = DeserializeSequenceOf (&n,MAX_DRB,1,bIterator);
      for (int i = 0; i < n; i++)
        {
          bIterator = DeserializeInteger (&val,1,32,bIterator);
          radioResourceConfigDedicated->drbToReleaseList.push_back (val);
        }
    }

  if (optionalFieldsPresent[2])
    {
      // Deserialize mac-MainConfig
      // ...
    }

  if (optionalFieldsPresent[1])
    {
      // Deserialize sps-Config
      // ...
    }

  radioResourceConfigDedicated->haveNistPhysicalConfigDedicated = optionalFieldsPresent[0];
  if (optionalFieldsPresent[0])
    {
      // Deserialize physicalConfigDedicated
      bIterator = DeserializeNistPhysicalConfigDedicated (&radioResourceConfigDedicated->physicalConfigDedicated,bIterator);
    }

  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeNistSrbToAddModList (std::list<NistLteRrcSap::NistSrbToAddMod> *srbToAddModList, Buffer::Iterator bIterator)
{
  int numElems;
  bIterator = DeserializeSequenceOf (&numElems,2,1,bIterator);

  srbToAddModList->clear ();

  // Deserialize SRB-ToAddMod elements
  for (int i = 0; i < numElems; i++)
    {
      NistLteRrcSap::NistSrbToAddMod srbToAddMod;
      // Deserialize SRB-ToAddMod sequence
      // 2 optional fields, extension marker present
      std::bitset<2> optionalFields;
      bIterator = DeserializeSequence (&optionalFields,true,bIterator);

      // Deserialize srbIdentity
      int n;
      bIterator = DeserializeInteger (&n,1,2,bIterator);
      srbToAddMod.srbIdentity = n;

      if (optionalFields[1])
        {
          // Deserialize rlcConfig choice
          // ...
        }

      if (optionalFields[0])
        {
          // Deserialize logicalChannelConfig choice
          int sel;
          bIterator = DeserializeChoice (2,false,&sel,bIterator);

          // Deserialize logicalChannelConfig defaultValue
          if (sel == 1)
            {
              bIterator = DeserializeNull (bIterator);
            }

          // Deserialize logicalChannelConfig explicitValue
          else if (sel == 0)
            {
              bIterator = DeserializeNistLogicalChannelConfig (&srbToAddMod.logicalChannelConfig,bIterator);
            }
        }
      srbToAddModList->insert (srbToAddModList->end (),srbToAddMod);
    }

  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeNistDrbToAddModList (std::list<NistLteRrcSap::NistDrbToAddMod> *drbToAddModList, Buffer::Iterator bIterator)
{
  int n;
  int val;
  bIterator = DeserializeSequenceOf (&n,MAX_DRB,1,bIterator);

  drbToAddModList->clear ();

  for (int i = 0; i < n; i++)
    {
      NistLteRrcSap::NistDrbToAddMod drbToAddMod;

      std::bitset<5> optionalFields;
      bIterator = DeserializeSequence (&optionalFields,true,bIterator);

      if (optionalFields[4])
        {
          // Deserialize epsBearerIdentity
          bIterator = DeserializeInteger (&val,0,15,bIterator);
          drbToAddMod.epsBearerIdentity = val;
        }

      bIterator = DeserializeInteger (&val,1,32,bIterator);
      drbToAddMod.drbIdentity = val;

      if (optionalFields[3])
        {
          // Deserialize pdcp-Config
          // ...
        }

      if (optionalFields[2])
        {
          // Deserialize RLC-Config
          int chosen;
          bIterator = DeserializeChoice (4,true,&chosen,bIterator);

          int sel;
          std::bitset<0> bitset0;
          switch (chosen)
            {
            case 0:
              drbToAddMod.rlcConfig.choice = NistLteRrcSap::NistRlcConfig::AM;

              // Deserialize UL-AM-RLC
              bIterator = DeserializeSequence (&bitset0,false, bIterator);
              bIterator = DeserializeEnum (64,&sel, bIterator); // t-PollRetransmit
              bIterator = DeserializeEnum (8,&sel, bIterator); // pollPDU
              bIterator = DeserializeEnum (16,&sel, bIterator); // pollByte
              bIterator = DeserializeEnum (8,&sel, bIterator); // maxRetxThreshold

              // Deserialize DL-AM-RLC
              bIterator = DeserializeSequence (&bitset0,false, bIterator);
              bIterator = DeserializeEnum (32,&sel, bIterator); // t-Reordering
              bIterator = DeserializeEnum (64,&sel, bIterator); // t-NistStatusProhibit
              break;

            case 1:
              drbToAddMod.rlcConfig.choice = NistLteRrcSap::NistRlcConfig::UM_BI_DIRECTIONAL;

              // Deserialize UL-UM-RLC
              bIterator = DeserializeSequence (&bitset0,false, bIterator);
              bIterator = DeserializeEnum (2,&sel, bIterator); // sn-FieldLength

              // Deserialize DL-UM-RLC
              bIterator = DeserializeSequence (&bitset0,false, bIterator);
              bIterator = DeserializeEnum (2,&sel, bIterator); // sn-FieldLength
              bIterator = DeserializeEnum (32,&sel, bIterator); // t-Reordering
              break;

            case 2:
              drbToAddMod.rlcConfig.choice = NistLteRrcSap::NistRlcConfig::UM_UNI_DIRECTIONAL_UL;

              // Deserialize UL-UM-RLC
              bIterator = DeserializeSequence (&bitset0,false, bIterator);
              bIterator = DeserializeEnum (2,&sel, bIterator); // sn-FieldLength
              break;

            case 3:
              drbToAddMod.rlcConfig.choice = NistLteRrcSap::NistRlcConfig::UM_UNI_DIRECTIONAL_DL;

              // Deserialize DL-UM-RLC
              bIterator = DeserializeSequence (&bitset0,false, bIterator);
              bIterator = DeserializeEnum (2,&sel, bIterator); // sn-FieldLength
              bIterator = DeserializeEnum (32,&sel, bIterator); // t-Reordering
              break;
            }

        }

      if (optionalFields[1])
        {
          bIterator = DeserializeInteger (&val,3,10,bIterator);
          drbToAddMod.logicalChannelIdentity = val;
        }

      if (optionalFields[0])
        {
          bIterator = DeserializeNistLogicalChannelConfig (&drbToAddMod.logicalChannelConfig,bIterator);
        }

      drbToAddModList->insert (drbToAddModList->end (),drbToAddMod);
    }
  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeNistLogicalChannelConfig (NistLteRrcSap::NistLogicalChannelConfig *logicalChannelConfig, Buffer::Iterator bIterator)
{
  int n;

  // Deserialize NistLogicalChannelConfig sequence
  // 1 optional field, extension marker is present.
  std::bitset<1> bitset1;
  bIterator = DeserializeSequence (&bitset1,true,bIterator);

  if (bitset1[0])
    {
      // Deserialize ul-SpecificParameters sequence
      bIterator = DeserializeSequence (&bitset1,false,bIterator);

      // Deserialize priority
      bIterator = DeserializeInteger (&n,1,16,bIterator);
      logicalChannelConfig->priority = n;

      // Deserialize prioritisedBitRate
      bIterator = DeserializeEnum (16,&n,bIterator);
      uint16_t prioritizedBitRateKbps;

      switch (n)
        {
        case 0:
          prioritizedBitRateKbps = 0;
          break;
        case 1:
          prioritizedBitRateKbps = 8;
          break;
        case 2:
          prioritizedBitRateKbps = 16;
          break;
        case 3:
          prioritizedBitRateKbps = 32;
          break;
        case 4:
          prioritizedBitRateKbps = 64;
          break;
        case 5:
          prioritizedBitRateKbps = 128;
          break;
        case 6:
          prioritizedBitRateKbps = 256;
          break;
        case 7:
          prioritizedBitRateKbps = 10000;
          break;
        default:
          prioritizedBitRateKbps = 10000;
        }
      logicalChannelConfig->prioritizedBitRateKbps = prioritizedBitRateKbps;

      // Deserialize bucketSizeDuration
      bIterator = DeserializeEnum (8,&n,bIterator);
      uint16_t bucketSizeDurationMs;
      switch (n)
        {
        case 0:
          bucketSizeDurationMs = 50;
          break;
        case 1:
          bucketSizeDurationMs = 100;
          break;
        case 2:
          bucketSizeDurationMs = 150;
          break;
        case 3:
          bucketSizeDurationMs = 300;
          break;
        case 4:
          bucketSizeDurationMs = 500;
          break;
        case 5:
          bucketSizeDurationMs = 1000;
          break;
        default:
          bucketSizeDurationMs = 1000;
        }
      logicalChannelConfig->bucketSizeDurationMs = bucketSizeDurationMs;

      if (bitset1[0])
        {
          // Deserialize logicalChannelGroup
          bIterator = DeserializeInteger (&n,0,3,bIterator);
          logicalChannelConfig->logicalChannelGroup = n;
        }
    }
  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeNistPhysicalConfigDedicated (NistLteRrcSap::NistPhysicalConfigDedicated *physicalConfigDedicated, Buffer::Iterator bIterator)
{
  std::bitset<10> optionalFieldPresent;
  bIterator = DeserializeSequence (&optionalFieldPresent,true,bIterator);

  physicalConfigDedicated->haveNistPdschConfigDedicated = optionalFieldPresent[9];
  if (optionalFieldPresent[9])
    {
      // Deserialize pdsch-ConfigDedicated
      std::bitset<0> bitset0;
      bIterator = DeserializeSequence (&bitset0,false,bIterator);

      int slct;

      // Deserialize p-a
      bIterator = DeserializeEnum (8,&slct,bIterator);
      physicalConfigDedicated->pdschConfigDedicated.pa = slct;

      bIterator = DeserializeNull (bIterator);

    }
  if (optionalFieldPresent[8])
    {
      // Deserialize pucch-ConfigDedicated
      // ...
    }
  if (optionalFieldPresent[7])
    {
      // Deserialize pusch-ConfigDedicated
      // ...
    }
  if (optionalFieldPresent[6])
    {
      // Deserialize uplinkPowerControlDedicated
      // ...
    }
  if (optionalFieldPresent[5])
    {
      // Deserialize tpc-PDCCH-ConfigPUCCH
      // ...
    }
  if (optionalFieldPresent[4])
    {
      // Deserialize tpc-PDCCH-ConfigPUSCH
      // ...
    }
  if (optionalFieldPresent[3])
    {
      // Deserialize cqi-ReportConfig
      // ...
    }
  physicalConfigDedicated->haveNistSoundingRsUlConfigDedicated = optionalFieldPresent[2];
  if (optionalFieldPresent[2])
    {
      // Deserialize soundingRS-UL-ConfigDedicated
      int sel;
      bIterator = DeserializeChoice (2,false,&sel,bIterator);

      if (sel == 0)
        {
          physicalConfigDedicated->soundingRsUlConfigDedicated.type = NistLteRrcSap::NistSoundingRsUlConfigDedicated::RESET;

          bIterator = DeserializeNull (bIterator);
        }

      else if (sel == 1)
        {
          physicalConfigDedicated->soundingRsUlConfigDedicated.type = NistLteRrcSap::NistSoundingRsUlConfigDedicated::SETUP;

          std::bitset<0> bitset0;
          bIterator = DeserializeSequence (&bitset0,false,bIterator);

          int slct;

          // Deserialize srs-Bandwidth
          bIterator = DeserializeEnum (4,&slct,bIterator);
          physicalConfigDedicated->soundingRsUlConfigDedicated.srsBandwidth = slct;

          // Deserialize srs-HoppingBandwidth
          bIterator = DeserializeEnum (4,&slct,bIterator);

          // Deserialize freqDomainPosition
          bIterator = DeserializeInteger (&slct,0,23,bIterator);

          // Deserialize duration
          bool duration;
          bIterator = DeserializeBoolean (&duration,bIterator);

          // Deserialize srs-ConfigIndex
          bIterator = DeserializeInteger (&slct,0,1023,bIterator);
          physicalConfigDedicated->soundingRsUlConfigDedicated.srsConfigIndex = slct;

          // Deserialize transmissionComb
          bIterator = DeserializeInteger (&slct,0,1,bIterator);

          // Deserialize cyclicShift
          bIterator = DeserializeEnum (8,&slct,bIterator);
        }
    }
  physicalConfigDedicated->haveNistAntennaInfoDedicated = optionalFieldPresent[1];
  if (optionalFieldPresent[1])
    {
      // Deserialize antennaInfo
      int sel;
      bIterator = DeserializeChoice (2,false,&sel,bIterator);
      if (sel == 1)
        {
          bIterator = DeserializeNull (bIterator);
        }
      else if (sel == 0)
        {
          std::bitset<1> codebookSubsetRestrictionPresent;
          bIterator = DeserializeSequence (&codebookSubsetRestrictionPresent,false,bIterator);

          int txmode;
          bIterator = DeserializeEnum (8,&txmode,bIterator);
          physicalConfigDedicated->antennaInfo.transmissionMode = txmode;

          if (codebookSubsetRestrictionPresent[0])
            {
              // Deserialize codebookSubsetRestriction
              // ...
            }

          int txantennaselchosen;
          bIterator = DeserializeChoice (2,false,&txantennaselchosen,bIterator);
          if (txantennaselchosen == 0)
            {
              // Deserialize ue-TransmitAntennaSelection release
              bIterator = DeserializeNull (bIterator);
            }
          else if (txantennaselchosen == 1)
            {
              // Deserialize ue-TransmitAntennaSelection setup
              // ...
            }
        }
    }
  if (optionalFieldPresent[0])
    {
      // Deserialize schedulingRequestConfig
      // ...
    }
  return bIterator;
}

void
NistRrcAsn1Header::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this << &os);
  NS_FATAL_ERROR ("NistRrcAsn1Header Print() function must also specify NistLteRrcSap::NistRadioResourceConfigDedicated as a second argument");
}

void
NistRrcAsn1Header::Print (std::ostream &os, NistLteRrcSap::NistRadioResourceConfigDedicated radioResourceConfigDedicated) const
{
  os << "   srbToAddModList: " << std::endl;
  std::list<NistLteRrcSap::NistSrbToAddMod>::iterator it = radioResourceConfigDedicated.srbToAddModList.begin ();
  for (; it != radioResourceConfigDedicated.srbToAddModList.end (); it++)
    {
      os << "      srbIdentity: " << (int)it->srbIdentity << std::endl;
      os << "      logicalChannelConfig: " << std::endl;
      os << "         priority: " <<  (int)it->logicalChannelConfig.priority << std::endl;
      os << "         prioritizedBitRateKbps: " <<  (int)it->logicalChannelConfig.prioritizedBitRateKbps << std::endl;
      os << "         bucketSizeDurationMs: " <<  (int)it->logicalChannelConfig.bucketSizeDurationMs << std::endl;
      os << "         logicalChannelGroup: " <<  (int)it->logicalChannelConfig.logicalChannelGroup << std::endl;
    }
  os << std::endl;

  os << "   drbToAddModList: " << std::endl;
  std::list<NistLteRrcSap::NistDrbToAddMod>::iterator it2 = radioResourceConfigDedicated.drbToAddModList.begin ();
  for (; it2 != radioResourceConfigDedicated.drbToAddModList.end (); it2++)
    {
      os << "      epsBearerIdentity: " << (int)it2->epsBearerIdentity << std::endl;
      os << "      drbIdentity: " << (int)it2->drbIdentity << std::endl;
      os << "      rlcConfig: " << it2->rlcConfig.choice << std::endl;
      os << "      logicalChannelIdentity: " << (int)it2->logicalChannelIdentity << std::endl;
      os << "      logicalChannelConfig: " << std::endl;
      os << "         priority: " <<  (int)it2->logicalChannelConfig.priority << std::endl;
      os << "         prioritizedBitRateKbps: " <<  (int)it2->logicalChannelConfig.prioritizedBitRateKbps << std::endl;
      os << "         bucketSizeDurationMs: " <<  (int)it2->logicalChannelConfig.bucketSizeDurationMs << std::endl;
      os << "         logicalChannelGroup: " <<  (int)it2->logicalChannelConfig.logicalChannelGroup << std::endl;
    }
  os << std::endl;

  os << "   drbToReleaseList: ";
  std::list<uint8_t>::iterator it3 = radioResourceConfigDedicated.drbToReleaseList.begin ();
  for (; it3 != radioResourceConfigDedicated.drbToReleaseList.end (); it3++)
    {
      os << (int)*it3 << ", ";
    }
  os << std::endl;

  os << "   haveNistPhysicalConfigDedicated: " << radioResourceConfigDedicated.haveNistPhysicalConfigDedicated << std::endl;

  if (radioResourceConfigDedicated.haveNistPhysicalConfigDedicated)
    {
      os << "   physicalConfigDedicated: " << std::endl;

      os << "      haveNistSoundingRsUlConfigDedicated: " << radioResourceConfigDedicated.physicalConfigDedicated.haveNistSoundingRsUlConfigDedicated << std::endl;
      if (radioResourceConfigDedicated.physicalConfigDedicated.haveNistSoundingRsUlConfigDedicated)
        {
          os << "      soundingRsUlConfigDedicated: " << std::endl;
          os << "         type: " << radioResourceConfigDedicated.physicalConfigDedicated.soundingRsUlConfigDedicated.type << std::endl;
          os << "         srsBandwidth: " << (int)radioResourceConfigDedicated.physicalConfigDedicated.soundingRsUlConfigDedicated.srsBandwidth << std::endl;
          os << "         srsConfigIndex: " << (int)radioResourceConfigDedicated.physicalConfigDedicated.soundingRsUlConfigDedicated.srsConfigIndex << std::endl;
        }

      os << "      haveNistAntennaInfoDedicated: " << radioResourceConfigDedicated.physicalConfigDedicated.haveNistAntennaInfoDedicated << std::endl;
      if (radioResourceConfigDedicated.physicalConfigDedicated.haveNistAntennaInfoDedicated)
        {
          os << "      antennaInfo Tx mode: " << (int)radioResourceConfigDedicated.physicalConfigDedicated.antennaInfo.transmissionMode << std::endl;
        }
    }
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeNistSystemInformationBlockType1 (NistLteRrcSap::NistSystemInformationBlockType1 *systemInformationBlockType1, Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  std::bitset<3> sysInfoBlkT1Opts;
  bIterator = DeserializeSequence (&sysInfoBlkT1Opts,false,bIterator);

  // Deserialize cellAccessRelatedInfo
  std::bitset<1> cellAccessRelatedInfoOpts;
  bIterator = DeserializeSequence (&cellAccessRelatedInfoOpts,false,bIterator);

  // Deserialize plmn-IdentityList
  int numNistPlmnIdentityInfoElements;
  bIterator = DeserializeSequenceOf (&numNistPlmnIdentityInfoElements,6,1,bIterator);
  for (int i = 0; i < numNistPlmnIdentityInfoElements; i++)
    {
      bIterator = DeserializeSequence (&bitset0,false,bIterator);

      // plmn-Identity
      bIterator = DeserializePlmnIdentity (&systemInformationBlockType1->cellAccessRelatedInfo.plmnIdentityInfo.plmnIdentity,bIterator);
    }

  // Deserialize trackingAreaCode
  std::bitset<16> trackingAreaCode;
  bIterator = DeserializeBitstring (&trackingAreaCode,bIterator);

  // Deserialize cellIdentity
  std::bitset<28> cellIdentity;
  bIterator = DeserializeBitstring (&cellIdentity,bIterator);
  systemInformationBlockType1->cellAccessRelatedInfo.cellIdentity = cellIdentity.to_ulong ();

  // Deserialize cellBarred
  bIterator = DeserializeEnum (2,&n,bIterator);

  // Deserialize intraFreqReselection
  bIterator = DeserializeEnum (2,&n,bIterator);

  // Deserialize csg-Indication
  bIterator = DeserializeBoolean (&systemInformationBlockType1->cellAccessRelatedInfo.csgIndication,bIterator);

  if (cellAccessRelatedInfoOpts[0])
    {
      // Deserialize csg-Identity
      std::bitset<27> csgIdentity;
      bIterator = DeserializeBitstring (&csgIdentity,bIterator);
      systemInformationBlockType1->cellAccessRelatedInfo.csgIdentity = csgIdentity.to_ulong ();
    }

  // Deserialize cellSelectionInfo
  std::bitset<1> qRxLevMinOffsetPresent;
  bIterator = DeserializeSequence (&qRxLevMinOffsetPresent,false,bIterator);
  bIterator = DeserializeInteger (&n,-70,-22,bIterator); //q-RxLevMin
  if (qRxLevMinOffsetPresent[0])
    {
      // Deserialize qRxLevMinOffset
      // ...
    }

  if (sysInfoBlkT1Opts[2])
    {
      // Deserialize p-Max
      // ...
    }

  // freqBandIndicator
  bIterator = DeserializeInteger (&n,1,64,bIterator);

  // schedulingInfoList
  int numSchedulingInfo;
  bIterator = DeserializeSequenceOf (&numSchedulingInfo,MAX_SI_MESSAGE,1,bIterator);
  for (int i = 0; i < numSchedulingInfo; i++)
    {
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
      bIterator = DeserializeEnum (7,&n,bIterator); // si-Periodicity
      int numSibType;
      bIterator = DeserializeSequenceOf (&numSibType,MAX_SIB - 1,0, bIterator); // sib-MappingInfo
      for (int j = 0; j < numSibType; j++)
        {
          bIterator = DeserializeEnum (16,&n,bIterator); // SIB-Type
        }
    }

  if (sysInfoBlkT1Opts[1])
    {
      // tdd-Config
      // ...
    }

  // si-WindowLength
  bIterator = DeserializeEnum (7,&n,bIterator);

  // systemInfoValueTag
  bIterator = DeserializeInteger (&n,0,31,bIterator);

  if (sysInfoBlkT1Opts[0])
    {
      // Deserialize nonCriticalExtension
      // ...
    }
  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeNistSystemInformationBlockType2 (NistLteRrcSap::NistSystemInformationBlockType2 *systemInformationBlockType2, Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  std::bitset<2> sysInfoBlkT2Opts;
  bIterator = DeserializeSequence (&sysInfoBlkT2Opts,true,bIterator);
  if (sysInfoBlkT2Opts[1])
    {
      // Deserialize ac-BarringInfo
      // ...
    }

  // Deserialize radioResourceConfigCommon
  bIterator = DeserializeNistRadioResourceConfigCommonSib (&systemInformationBlockType2->radioResourceConfigCommon, bIterator);

  // Deserialize ue-TimersAndConstants
  bIterator = DeserializeSequence (&bitset0,true,bIterator);
  bIterator = DeserializeEnum (8,&n,bIterator); // t300
  bIterator = DeserializeEnum (8,&n,bIterator); // t301
  bIterator = DeserializeEnum (7,&n,bIterator); // t310
  bIterator = DeserializeEnum (8,&n,bIterator); // n310
  bIterator = DeserializeEnum (7,&n,bIterator); // t311
  bIterator = DeserializeEnum (8,&n,bIterator); // n311

  // Deserialize freqInfo
  std::bitset<2> freqInfoOpts;
  bIterator = DeserializeSequence (&freqInfoOpts,false,bIterator);
  if (freqInfoOpts[1])
    {
      // Deserialize ul-CarrierFreq
      bIterator = DeserializeInteger (&n, 0, MAX_EARFCN, bIterator);
      systemInformationBlockType2->freqInfo.ulCarrierFreq = n;
    }
  if (freqInfoOpts[0])
    {
      // Deserialize ul-Bandwidth
      bIterator = DeserializeEnum (6, &n, bIterator);
      switch (n)
        {
        case 0:
          systemInformationBlockType2->freqInfo.ulBandwidth = 6;
          break;
        case 1:
          systemInformationBlockType2->freqInfo.ulBandwidth = 15;
          break;
        case 2:
          systemInformationBlockType2->freqInfo.ulBandwidth = 25;
          break;
        case 3:
          systemInformationBlockType2->freqInfo.ulBandwidth = 50;
          break;
        case 4:
          systemInformationBlockType2->freqInfo.ulBandwidth = 75;
          break;
        case 5:
          systemInformationBlockType2->freqInfo.ulBandwidth = 100;
          break;
        default:
          systemInformationBlockType2->freqInfo.ulBandwidth = 6;
        }
    }

  // additionalSpectrumEmission
  bIterator = DeserializeInteger (&n,1,32,bIterator);

  if (sysInfoBlkT2Opts[0])
    {
      // Deserialize mbsfn-SubframeConfigList
      // ...
    }

  // Deserialize timeAlignmentTimerCommon
  bIterator = DeserializeEnum (8,&n,bIterator);

  return bIterator;
}


Buffer::Iterator
NistRrcAsn1Header::DeserializeNistRadioResourceConfigCommon (NistLteRrcSap::NistRadioResourceConfigCommon * radioResourceConfigCommon, Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  std::bitset<9> rrCfgCommOptions;
  bIterator = DeserializeSequence (&rrCfgCommOptions,true,bIterator);

  // rach-ConfigCommon
  if (rrCfgCommOptions[8])
    {
      bIterator = DeserializeNistRachConfigCommon (&radioResourceConfigCommon->rachConfigCommon, bIterator);
    }

  // prach-Config
  std::bitset<1> prachConfigInfoPresent;
  bIterator = DeserializeSequence (&prachConfigInfoPresent,false,bIterator);

  // prach-Config -> rootSequenceIndex
  bIterator = DeserializeInteger (&n,0,1023,bIterator);

  // prach-Config -> prach-ConfigInfo
  if (prachConfigInfoPresent[0])
    {
      // ...
    }

  // pdsch-ConfigCommon
  if (rrCfgCommOptions[7])
    {
      // ...
    }

  // pusch-ConfigCommon
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // pusch-ConfigCommon -> pusch-ConfigBasic
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // pusch-ConfigCommon -> pusch-ConfigBasic -> n-SB
  bIterator = DeserializeInteger (&n,1,4,bIterator);

  // pusch-ConfigCommon -> pusch-ConfigBasic -> hoppingMode
  bIterator = DeserializeEnum (2,&n,bIterator);

  // pusch-ConfigCommon -> pusch-ConfigBasic -> pusch-HoppingOffset
  bIterator = DeserializeInteger (&n,0,98,bIterator);

  // pusch-ConfigCommon -> pusch-ConfigBasic -> enable64QAM
  bool enable64QAM;
  bIterator = DeserializeBoolean (&enable64QAM,bIterator);

  // ul-ReferenceSignalsPUSCH
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // groupHoppingEnabled
  bool dummyBool;
  bIterator = DeserializeBoolean (&dummyBool,bIterator);

  // groupAssignmentPUSCH
  bIterator = DeserializeInteger (&n,0,29,bIterator);

  // sequenceHoppingEnabled
  bIterator = DeserializeBoolean (&dummyBool,bIterator);

  // cyclicShift
  bIterator = DeserializeInteger (&n,0,7,bIterator);

  // phich-Config
  if (rrCfgCommOptions[6])
    {
      // ...
    }

  // pucch-ConfigCommon
  if (rrCfgCommOptions[5])
    {
      // ...
    }

  // soundingRS-UL-ConfigCommon
  if (rrCfgCommOptions[4])
    {
      // ...
    }

  // uplinkPowerControlCommon
  if (rrCfgCommOptions[3])
    {
      // ...
    }

  // antennaInfoCommon
  if (rrCfgCommOptions[2])
    {
      // ...
    }

  // p-Max
  if (rrCfgCommOptions[1])
    {
      // ...
    }

  // tdd-Config
  if (rrCfgCommOptions[0])
    {
      // ...
    }

  // ul-CyclicPrefixLength
  bIterator = DeserializeEnum (2,&n,bIterator);

  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeNistRachConfigCommon (NistLteRrcSap::NistRachConfigCommon * rachConfigCommon, Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeSequence (&bitset0,true,bIterator);

  // preambleInfo
  std::bitset<1> preamblesGroupAConfigPresent;
  bIterator = DeserializeSequence (&preamblesGroupAConfigPresent,false,bIterator);

  // numberOfRA-Preambles
  bIterator = DeserializeEnum (16,&n,bIterator);
  switch (n)
    {
    case 0:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 4;
      break;
    case 1:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 8;
      break;
    case 2:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 12;
      break;
    case 3:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 16;
      break;
    case 4:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 20;
      break;
    case 5:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 24;
      break;
    case 6:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 28;
      break;
    case 7:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 32;
      break;
    case 8:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 36;
      break;
    case 9:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 40;
      break;
    case 10:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 44;
      break;
    case 11:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 48;
      break;
    case 12:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 52;
      break;
    case 13:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 56;
      break;
    case 14:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 60;
      break;
    case 15:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 64;
      break;
    default:
      rachConfigCommon->preambleInfo.numberOfRaPreambles = 0;
    }

  rachConfigCommon->preambleInfo.numberOfRaPreambles = n;

  if (preamblesGroupAConfigPresent[0])
    {
      // Deserialize preamblesGroupAConfig
      // ...
    }

  // powerRampingParameters
  bIterator = DeserializeSequence (&bitset0,false,bIterator);
  bIterator = DeserializeEnum (4,&n,bIterator); // powerRampingStep
  bIterator = DeserializeEnum (16,&n,bIterator); // preambleInitialReceivedTargetPower

  // ra-SupervisionInfo
  bIterator = DeserializeSequence (&bitset0,false,bIterator);
  bIterator = DeserializeEnum (11,&n,bIterator); // preambleTransMax
  switch (n)
    {
    case 0:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 3;
      break;
    case 1:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 4;
      break;
    case 2:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 5;
      break;
    case 3:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 6;
      break;
    case 4:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 7;
      break;
    case 5:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 8;
      break;
    case 6:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 10;
      break;
    case 7:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 20;
      break;
    case 8:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 50;
      break;
    case 9:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 100;
      break;
    case 10:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 200;
      break;
    default:
      rachConfigCommon->raSupervisionInfo.preambleTransMax = 0;
    }

  // ra-ResponseWindowSize
  bIterator = DeserializeEnum (8,&n,bIterator);
  switch (n)
    {
    case 0:
      rachConfigCommon->raSupervisionInfo.raResponseWindowSize = 2;
      break;
    case 1:
      rachConfigCommon->raSupervisionInfo.raResponseWindowSize = 3;
      break;
    case 2:
      rachConfigCommon->raSupervisionInfo.raResponseWindowSize = 4;
      break;
    case 3:
      rachConfigCommon->raSupervisionInfo.raResponseWindowSize = 5;
      break;
    case 4:
      rachConfigCommon->raSupervisionInfo.raResponseWindowSize = 6;
      break;
    case 5:
      rachConfigCommon->raSupervisionInfo.raResponseWindowSize = 7;
      break;
    case 6:
      rachConfigCommon->raSupervisionInfo.raResponseWindowSize = 8;
      break;
    case 7:
      rachConfigCommon->raSupervisionInfo.raResponseWindowSize = 10;
      break;
    default:
      rachConfigCommon->raSupervisionInfo.raResponseWindowSize = 0;
    }

  bIterator = DeserializeEnum (8,&n,bIterator); // mac-ContentionResolutionTimer
  bIterator = DeserializeInteger (&n,1,8,bIterator); //maxHARQ-Msg3Tx
  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeNistRadioResourceConfigCommonSib (NistLteRrcSap::NistRadioResourceConfigCommonSib * radioResourceConfigCommonSib, Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeSequence (&bitset0,true,bIterator);

  // rach-ConfigCommon
  bIterator = DeserializeNistRachConfigCommon (&radioResourceConfigCommonSib->rachConfigCommon, bIterator);

  // bcch-Config 
  bIterator = DeserializeSequence (&bitset0,false,bIterator);
  bIterator = DeserializeEnum (4,&n,bIterator); // modificationPeriodCoeff

  // pcch-Config 
  bIterator = DeserializeSequence (&bitset0,false,bIterator);
  bIterator = DeserializeEnum (4,&n,bIterator); // defaultPagingCycle
  bIterator = DeserializeEnum (8,&n,bIterator); // nB

  // prach-Config 
  std::bitset<1> prachConfigInfoPresent;
  bIterator = DeserializeSequence (&prachConfigInfoPresent,false,bIterator);
  // prach-Config -> rootSequenceIndex
  bIterator = DeserializeInteger (&n,0,1023,bIterator);
  // prach-Config -> prach-ConfigInfo
  if (prachConfigInfoPresent[0])
    {
      // ...
    }

  // pdsch-ConfigCommon 
  bIterator = DeserializeSequence (&bitset0,false,bIterator);
  bIterator = DeserializeInteger (&n,-60,50,bIterator); // referenceSignalPower
  bIterator = DeserializeInteger (&n,0,3,bIterator); // p-b

  // pusch-ConfigCommon
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // pusch-ConfigCommon -> pusch-ConfigBasic
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // pusch-ConfigCommon -> pusch-ConfigBasic -> n-SB
  bIterator = DeserializeInteger (&n,1,4,bIterator);

  // pusch-ConfigCommon -> pusch-ConfigBasic -> hoppingMode
  bIterator = DeserializeEnum (2,&n,bIterator);

  // pusch-ConfigCommon -> pusch-ConfigBasic -> pusch-HoppingOffset
  bIterator = DeserializeInteger (&n,0,98,bIterator);

  // pusch-ConfigCommon -> pusch-ConfigBasic -> enable64QAM
  bool dummyBoolean;
  bIterator = DeserializeBoolean (&dummyBoolean,bIterator);

  // ul-ReferenceSignalsPUSCH 
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // groupHoppingEnabled 
  bIterator = DeserializeBoolean (&dummyBoolean,bIterator);

  // groupAssignmentPUSCH 
  bIterator = DeserializeInteger (&n,0,29,bIterator);

  // sequenceHoppingEnabled 
  bIterator = DeserializeBoolean (&dummyBoolean,bIterator);

  // cyclicShift 
  bIterator = DeserializeInteger (&n,0,7,bIterator);

  // pucch-ConfigCommon 
  bIterator = DeserializeEnum (3,&n,bIterator); // deltaPUCCH-Shift 
  bIterator = DeserializeInteger (&n,0,98,bIterator); // nRB-CQI 
  bIterator = DeserializeInteger (&n,0,7,bIterator); // nCS-AN 
  bIterator = DeserializeInteger (&n,0,2047,bIterator); // n1PUCCH-AN 

  // soundingRS-UL-ConfigCommon
  int choice;
  bIterator = DeserializeChoice (2,false,&choice,bIterator);
  if (choice == 0)
    {
      bIterator = DeserializeNull (bIterator); // release
    }
  if (choice == 1)
    {
      // setup
      // ...
    }

  // uplinkPowerControlCommon 
  bIterator = DeserializeSequence (&bitset0,false,bIterator);
  bIterator = DeserializeInteger (&n,-126,24,bIterator); // p0-NominalPUSCH
  bIterator = DeserializeEnum (8,&n,bIterator); // alpha 
  bIterator = DeserializeInteger (&n,-127,-96,bIterator); // p0-NominalPUCCH 
  //deltaFList-PUCCH 
  bIterator = DeserializeSequence (&bitset0,false,bIterator);
  bIterator = DeserializeEnum (3,&n,bIterator); // deltaF-PUCCH-Format1 
  bIterator = DeserializeEnum (3,&n,bIterator); // deltaF-PUCCH-Format1b 
  bIterator = DeserializeEnum (4,&n,bIterator); // deltaF-PUCCH-Format2 
  bIterator = DeserializeEnum (3,&n,bIterator); // deltaF-PUCCH-Format2a 
  bIterator = DeserializeEnum (3,&n,bIterator); // deltaF-PUCCH-Format2b
  bIterator = DeserializeInteger (&n,-1,6,bIterator); // deltaPreambleMsg3 
 
  // ul-CyclicPrefixLength
  bIterator = DeserializeEnum (2,&n,bIterator);

  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeNistMeasResults (NistLteRrcSap::NistMeasResults *measResults, Buffer::Iterator bIterator)
{
  int n;
  std::bitset<0> b0;
  std::bitset<1> measResultNeighCellsPresent;
  bIterator = DeserializeSequence (&measResultNeighCellsPresent,true,bIterator);

  // Deserialize measId
  bIterator = DeserializeInteger (&n, 1, MAX_MEAS_ID, bIterator);
  measResults->measId = n;

  // Deserialize measResultServCell
  bIterator = DeserializeSequence (&b0,false,bIterator);

  // Deserialize rsrpResult
  bIterator = DeserializeInteger (&n, 0, 97, bIterator);
  measResults->rsrpResult = n;

  // Deserialize rsrqResult
  bIterator = DeserializeInteger (&n, 0, 34, bIterator);
  measResults->rsrqResult = n;

  measResults->haveMeasResultNeighCells = measResultNeighCellsPresent[0];
  if ( measResults->haveMeasResultNeighCells)
    {
      int measResultNeighCellsChoice;

      // Deserialize measResultNeighCells
      bIterator = DeserializeChoice (4,false,&measResultNeighCellsChoice,bIterator);

      if (measResultNeighCellsChoice == 0)
        {
          // Deserialize measResultListEUTRA
          int numElems;
          bIterator = DeserializeSequenceOf (&numElems,MAX_CELL_REPORT,1,bIterator);

          for (int i = 0; i < numElems; i++)
            {
              NistLteRrcSap::NistMeasResultEutra measResultEutra;

              std::bitset<1> isNistCgiInfoPresent;
              bIterator = DeserializeSequence (&isNistCgiInfoPresent,false,bIterator);

              // PhysCellId
              bIterator = DeserializeInteger (&n,0,503,bIterator);
              measResultEutra.physCellId = n;

              measResultEutra.haveNistCgiInfo = isNistCgiInfoPresent[0];
              if (isNistCgiInfoPresent[0])
                {
                  std::bitset<1> havePlmnIdentityList;
                  bIterator = DeserializeSequence (&havePlmnIdentityList,false,bIterator);

                  // Deserialize cellGlobalId
                  bIterator = DeserializeSequence (&b0,false,bIterator);

                  // Deserialize plmn-Identity
                  bIterator = DeserializePlmnIdentity (&measResultEutra.cgiInfo.plmnIdentity,bIterator);

                  // Deserialize CellIdentity
                  std::bitset<28> cellId;
                  bIterator = DeserializeBitstring (&cellId,bIterator);
                  measResultEutra.cgiInfo.cellIdentity = cellId.to_ulong ();

                  // Deserialize trackingAreaCode
                  std::bitset<16> trArCo;
                  bIterator = DeserializeBitstring (&trArCo,bIterator);
                  measResultEutra.cgiInfo.trackingAreaCode = trArCo.to_ulong ();

                  // Deserialize plmn-IdentityList
                  if (havePlmnIdentityList[0])
                    {
                      int numPlmnElems;
                      bIterator = DeserializeSequenceOf (&numPlmnElems, 5, 1, bIterator);

                      for ( int j = 0; j < numPlmnElems; j++)
                        {
                          uint32_t plmnId;
                          bIterator = DeserializePlmnIdentity (&plmnId,bIterator);
                          measResultEutra.cgiInfo.plmnIdentityList.push_back (plmnId);
                        }
                    }
                }

              // Deserialize measResult
              std::bitset<2> measResultOpts;
              bIterator = DeserializeSequence (&measResultOpts, true, bIterator);

              measResultEutra.haveRsrpResult = measResultOpts[1];
              if (measResultOpts[1])
                {
                  // Deserialize rsrpResult
                  bIterator = DeserializeInteger (&n,0,97,bIterator);
                  measResultEutra.rsrpResult = n;
                }

              measResultEutra.haveRsrqResult = measResultOpts[0];
              if (measResultOpts[0])
                {
                  // Deserialize rsrqResult
                  bIterator = DeserializeInteger (&n,0,34,bIterator);
                  measResultEutra.rsrqResult = n;
                }

              measResults->measResultListEutra.push_back (measResultEutra);
            }
        }

      if (measResultNeighCellsChoice == 1)
        {
          // Deserialize measResultListUTRA
          // ...
        }

      if (measResultNeighCellsChoice == 2)
        {
          // Deserialize measResultListGERAN
          // ...
        }
      if (measResultNeighCellsChoice == 3)
        {
          // Deserialize measResultsCDMA2000
          // ...
        }
    }

  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializePlmnIdentity (uint32_t *plmnId, Buffer::Iterator bIterator)
{
  int n;
  std::bitset<1> isMccPresent;
  bIterator = DeserializeSequence (&isMccPresent,false,bIterator);

  if (isMccPresent[0])
    {
      // Deserialize mcc
      // ...
    }

  // Deserialize mnc
  int mncDigits;
  int mnc = 0;
  bIterator = DeserializeSequenceOf (&mncDigits,3,2,bIterator);

  for (int j = mncDigits - 1; j >= 0; j--)
    {
      bIterator = DeserializeInteger (&n,0,9,bIterator);
      mnc += n * pow (10,j);
    }

  *plmnId = mnc;

  // cellReservedForOperatorUse
  bIterator = DeserializeEnum (2,&n,bIterator);
  return bIterator;
}

Buffer::Iterator
NistRrcAsn1Header::DeserializeNistMeasConfig (NistLteRrcSap::NistMeasConfig * measConfig, Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  std::bitset<2> bitset2;
  std::bitset<11> bitset11;
  int n;

  // measConfig
  bIterator = DeserializeSequence (&bitset11,true,bIterator);

  if (bitset11[10])
    {
      // measObjectToRemoveList
      int measObjectToRemoveListElems;
      bIterator = DeserializeSequenceOf (&measObjectToRemoveListElems, MAX_OBJECT_ID, 1, bIterator);

      for (int i = 0; i < measObjectToRemoveListElems; i++)
        {
          bIterator = DeserializeInteger (&n, 1, MAX_OBJECT_ID, bIterator);
          measConfig->measObjectToRemoveList.push_back (n);
        }
    }

  if (bitset11[9])
    {
      // measObjectToAddModList
      int measObjectToAddModListElems;
      bIterator = DeserializeSequenceOf (&measObjectToAddModListElems, MAX_OBJECT_ID, 1, bIterator);

      for (int i = 0; i < measObjectToAddModListElems; i++)
        {
          NistLteRrcSap::NistMeasObjectToAddMod elem;

          bIterator = DeserializeSequence (&bitset0, false, bIterator);

          bIterator = DeserializeInteger (&n, 1, MAX_OBJECT_ID, bIterator);
          elem.measObjectId = n;

          int measObjectChoice;
          bIterator = DeserializeChoice (4, true, &measObjectChoice, bIterator);

          switch (measObjectChoice)
            {
            case 1:
              // Deserialize measObjectUTRA
              // ...
              break;

            case 2:
              // Deserialize measObjectGERAN
              // ...
              break;

            case 3:
              // Deserialize measObjectCDMA2000
              // ...
              break;

            case 0:
            default:
              // Deserialize measObjectEUTRA
              std::bitset<5> measObjectEutraOpts;
              bIterator = DeserializeSequence (&measObjectEutraOpts, true, bIterator);

              // carrierFreq
              bIterator = DeserializeInteger (&n, 0, MAX_EARFCN, bIterator);
              elem.measObjectEutra.carrierFreq = n;

              // allowedMeasBandwidth
              bIterator = DeserializeEnum (6, &n, bIterator);
              switch (n)
                {
                case 0:
                  elem.measObjectEutra.allowedMeasBandwidth = 6;
                  break;
                case 1:
                  elem.measObjectEutra.allowedMeasBandwidth = 15;
                  break;
                case 2:
                  elem.measObjectEutra.allowedMeasBandwidth = 25;
                  break;
                case 3:
                  elem.measObjectEutra.allowedMeasBandwidth = 50;
                  break;
                case 4:
                  elem.measObjectEutra.allowedMeasBandwidth = 75;
                  break;
                case 5:
                default:
                  elem.measObjectEutra.allowedMeasBandwidth = 100;
                  break;
                }

              // presenceAntennaPort1
              bIterator = DeserializeBoolean (&elem.measObjectEutra.presenceAntennaPort1, bIterator);

              // neighCellConfig
              bIterator = DeserializeBitstring (&bitset2, bIterator);
              elem.measObjectEutra.neighCellConfig = bitset2.to_ulong ();

              // offsetFreq
              bIterator = DeserializeQoffsetRange (&elem.measObjectEutra.offsetFreq, bIterator);

              if (measObjectEutraOpts[4])
                {
                  // cellsToRemoveList
                  int numElems;
                  bIterator = DeserializeSequenceOf (&numElems, MAX_CELL_MEAS, 1, bIterator);

                  for (int i = 0; i < numElems; i++)
                    {
                      bIterator = DeserializeInteger (&n, 1, MAX_CELL_MEAS, bIterator);
                      elem.measObjectEutra.cellsToRemoveList.push_back (n);
                    }
                }

              if (measObjectEutraOpts[3])
                {
                  // cellsToAddModList
                  int numElems;
                  bIterator = DeserializeSequenceOf (&numElems, MAX_CELL_MEAS, 1, bIterator);

                  for (int i = 0; i < numElems; i++)
                    {
                      NistLteRrcSap::NistCellsToAddMod cellsToAddMod;

                      bIterator = DeserializeSequence (&bitset0, false, bIterator);

                      // cellIndex
                      bIterator = DeserializeInteger (&n, 1, MAX_CELL_MEAS, bIterator);
                      cellsToAddMod.cellIndex = n;

                      // PhysCellId
                      bIterator = DeserializeInteger (&n, 0, 503, bIterator);
                      cellsToAddMod.physCellId = n;

                      // cellIndividualOffset
                      bIterator = DeserializeQoffsetRange ( &cellsToAddMod.cellIndividualOffset, bIterator);

                      elem.measObjectEutra.cellsToAddModList.push_back (cellsToAddMod);
                    }
                }

              if (measObjectEutraOpts[2])
                {
                  // blackCellsToRemoveList
                  int numElems;
                  bIterator = DeserializeSequenceOf (&numElems, MAX_CELL_MEAS, 1, bIterator);

                  for (int i = 0; i < numElems; i++)
                    {
                      bIterator = DeserializeInteger (&n, 1, MAX_CELL_MEAS, bIterator);
                      elem.measObjectEutra.blackCellsToRemoveList.push_back (n);
                    }
                }


              if (measObjectEutraOpts[1])
                {
                  // blackNistCellsToAddModList
                  int numElems;
                  bIterator = DeserializeSequenceOf (&numElems, MAX_CELL_MEAS, 1, bIterator);

                  for (int i = 0; i < numElems; i++)
                    {
                      NistLteRrcSap::BlackNistCellsToAddMod blackNistCellsToAddMod;
                      bIterator = DeserializeSequence (&bitset0, false, bIterator);

                      bIterator = DeserializeInteger (&n, 1, MAX_CELL_MEAS, bIterator);
                      blackNistCellsToAddMod.cellIndex = n;

                      // NistPhysCellIdRange
                      std::bitset<1> isRangePresent;
                      bIterator = DeserializeSequence (&isRangePresent, false, bIterator);

                      // start
                      bIterator = DeserializeInteger (&n, 0, 503, bIterator);
                      blackNistCellsToAddMod.physCellIdRange.start = n;

                      blackNistCellsToAddMod.physCellIdRange.haveRange = isRangePresent[0];
                      // initialize range to silence compiler warning
                      blackNistCellsToAddMod.physCellIdRange.range = 0;
                      if (blackNistCellsToAddMod.physCellIdRange.haveRange)
                        {
                          // range
                          bIterator = DeserializeEnum (16, &n, bIterator);
                          switch (n)
                            {
                            case 0:
                              blackNistCellsToAddMod.physCellIdRange.range = 4;
                              break;
                            case 1:
                              blackNistCellsToAddMod.physCellIdRange.range = 8;
                              break;
                            case 2:
                              blackNistCellsToAddMod.physCellIdRange.range = 12;
                              break;
                            case 3:
                              blackNistCellsToAddMod.physCellIdRange.range = 16;
                              break;
                            case 4:
                              blackNistCellsToAddMod.physCellIdRange.range = 24;
                              break;
                            case 5:
                              blackNistCellsToAddMod.physCellIdRange.range = 32;
                              break;
                            case 6:
                              blackNistCellsToAddMod.physCellIdRange.range = 48;
                              break;
                            case 7:
                              blackNistCellsToAddMod.physCellIdRange.range = 64;
                              break;
                            case 8:
                              blackNistCellsToAddMod.physCellIdRange.range = 84;
                              break;
                            case 9:
                              blackNistCellsToAddMod.physCellIdRange.range = 96;
                              break;
                            case 10:
                              blackNistCellsToAddMod.physCellIdRange.range = 128;
                              break;
                            case 11:
                              blackNistCellsToAddMod.physCellIdRange.range = 168;
                              break;
                            case 12:
                              blackNistCellsToAddMod.physCellIdRange.range = 252;
                              break;
                            case 13:
                              blackNistCellsToAddMod.physCellIdRange.range = 504;
                              break;
                            default:
                              blackNistCellsToAddMod.physCellIdRange.range = 0;
                            }
                        }

                      elem.measObjectEutra.blackNistCellsToAddModList.push_back (blackNistCellsToAddMod);
                    }
                }

              elem.measObjectEutra.haveCellForWhichToReportCGI = measObjectEutraOpts[0];
              if (measObjectEutraOpts[0])
                {
                  // cellForWhichToReportCGI
                  bIterator = DeserializeInteger (&n, 0, 503, bIterator);
                  elem.measObjectEutra.cellForWhichToReportCGI = n;
                }
            }
          measConfig->measObjectToAddModList.push_back (elem);
        }
    }

  if (bitset11[8])
    {
      // reportConfigToRemoveList
      int reportConfigToRemoveListElems;
      bIterator = DeserializeSequenceOf (&reportConfigToRemoveListElems, MAX_REPORT_CONFIG_ID, 1, bIterator);

      for (int i = 0; i < reportConfigToRemoveListElems; i++)
        {
          bIterator = DeserializeInteger (&n, 1, MAX_REPORT_CONFIG_ID, bIterator);
          measConfig->reportConfigToRemoveList.push_back (n);
        }
    }

  if (bitset11[7])
    {
      // reportConfigToAddModList
      int reportConfigToAddModListElems;
      bIterator = DeserializeSequenceOf (&reportConfigToAddModListElems, MAX_REPORT_CONFIG_ID, 1, bIterator);

      for (int i = 0; i < reportConfigToAddModListElems; i++)
        {
          NistLteRrcSap::NistReportConfigToAddMod elem;

          bIterator = DeserializeSequence (&bitset0, false, bIterator);
          bIterator = DeserializeInteger (&n, 1, MAX_REPORT_CONFIG_ID, bIterator);
          elem.reportConfigId = n;

          // Deserialize reportConfig
          int reportConfigChoice;
          bIterator = DeserializeChoice (2, false, &reportConfigChoice, bIterator);

          if (reportConfigChoice == 0)
            {
              // reportConfigEUTRA
              bIterator = DeserializeSequence (&bitset0, true, bIterator);

              // triggerType
              int triggerTypeChoice;
              bIterator = DeserializeChoice (2, false, &triggerTypeChoice, bIterator);

              if (triggerTypeChoice == 0)
                {
                  // event
                  elem.reportConfigEutra.triggerType = NistLteRrcSap::NistReportConfigEutra::EVENT;
                  bIterator = DeserializeSequence (&bitset0, false, bIterator);

                  // eventId
                  int eventIdChoice;
                  bIterator = DeserializeChoice (5, true, &eventIdChoice, bIterator);

                  switch (eventIdChoice)
                    {
                    case 0:
                      elem.reportConfigEutra.eventId = NistLteRrcSap::NistReportConfigEutra::EVENT_A1;
                      bIterator = DeserializeSequence (&bitset0, false, bIterator);
                      bIterator = DeserializeNistThresholdEutra (&elem.reportConfigEutra.threshold1, bIterator);
                      break;

                    case 1:
                      elem.reportConfigEutra.eventId = NistLteRrcSap::NistReportConfigEutra::EVENT_A2;
                      bIterator = DeserializeSequence (&bitset0, false, bIterator);
                      bIterator = DeserializeNistThresholdEutra (&elem.reportConfigEutra.threshold1, bIterator);
                      break;

                    case 2:
                      elem.reportConfigEutra.eventId = NistLteRrcSap::NistReportConfigEutra::EVENT_A3;
                      bIterator = DeserializeSequence (&bitset0, false, bIterator);
                      bIterator = DeserializeInteger (&n, -30, 30, bIterator);
                      elem.reportConfigEutra.a3Offset = n;
                      bIterator = DeserializeBoolean (&elem.reportConfigEutra.reportOnLeave, bIterator);
                      break;

                    case 3:
                      elem.reportConfigEutra.eventId = NistLteRrcSap::NistReportConfigEutra::EVENT_A4;
                      bIterator = DeserializeSequence (&bitset0, false, bIterator);
                      bIterator = DeserializeNistThresholdEutra (&elem.reportConfigEutra.threshold1, bIterator);
                      break;

                    case 4:
                    default:
                      elem.reportConfigEutra.eventId = NistLteRrcSap::NistReportConfigEutra::EVENT_A5;
                      bIterator = DeserializeSequence (&bitset0, false, bIterator);
                      bIterator = DeserializeNistThresholdEutra (&elem.reportConfigEutra.threshold1, bIterator);
                      bIterator = DeserializeNistThresholdEutra (&elem.reportConfigEutra.threshold2, bIterator);
                    }

                  bIterator = DeserializeInteger (&n, 0, 30, bIterator);
                  elem.reportConfigEutra.hysteresis = n;

                  bIterator = DeserializeEnum (16, &n, bIterator);
                  switch (n)
                    {
                    case 0:
                      elem.reportConfigEutra.timeToTrigger = 0;
                      break;
                    case 1:
                      elem.reportConfigEutra.timeToTrigger = 40;
                      break;
                    case 2:
                      elem.reportConfigEutra.timeToTrigger = 64;
                      break;
                    case 3:
                      elem.reportConfigEutra.timeToTrigger = 80;
                      break;
                    case 4:
                      elem.reportConfigEutra.timeToTrigger = 100;
                      break;
                    case 5:
                      elem.reportConfigEutra.timeToTrigger = 128;
                      break;
                    case 6:
                      elem.reportConfigEutra.timeToTrigger = 160;
                      break;
                    case 7:
                      elem.reportConfigEutra.timeToTrigger = 256;
                      break;
                    case 8:
                      elem.reportConfigEutra.timeToTrigger = 320;
                      break;
                    case 9:
                      elem.reportConfigEutra.timeToTrigger = 480;
                      break;
                    case 10:
                      elem.reportConfigEutra.timeToTrigger = 512;
                      break;
                    case 11:
                      elem.reportConfigEutra.timeToTrigger = 640;
                      break;
                    case 12:
                      elem.reportConfigEutra.timeToTrigger = 1024;
                      break;
                    case 13:
                      elem.reportConfigEutra.timeToTrigger = 1280;
                      break;
                    case 14:
                      elem.reportConfigEutra.timeToTrigger = 2560;
                      break;
                    case 15:
                    default:
                      elem.reportConfigEutra.timeToTrigger = 5120;
                      break;
                    }
                }

              if (triggerTypeChoice == 1)
                {
                  // periodical
                  elem.reportConfigEutra.triggerType = NistLteRrcSap::NistReportConfigEutra::PERIODICAL;

                  bIterator = DeserializeSequence (&bitset0, false, bIterator);
                  bIterator = DeserializeEnum (2, &n, bIterator);
                  if (n == 0)
                    {
                      elem.reportConfigEutra.purpose = NistLteRrcSap::NistReportConfigEutra::REPORT_STRONGEST_CELLS;
                    }
                  else
                    {
                      elem.reportConfigEutra.purpose = NistLteRrcSap::NistReportConfigEutra::REPORT_CGI;
                    }
                }

              // triggerQuantity
              bIterator = DeserializeEnum (2, &n, bIterator);
              if (n == 0)
                {
                  elem.reportConfigEutra.triggerQuantity = NistLteRrcSap::NistReportConfigEutra::RSRP;
                }
              else
                {
                  elem.reportConfigEutra.triggerQuantity = NistLteRrcSap::NistReportConfigEutra::RSRQ;
                }

              // reportQuantity
              bIterator = DeserializeEnum (2, &n, bIterator);
              if (n == 0)
                {
                  elem.reportConfigEutra.reportQuantity = NistLteRrcSap::NistReportConfigEutra::SAME_AS_TRIGGER_QUANTITY;
                }
              else
                {
                  elem.reportConfigEutra.reportQuantity = NistLteRrcSap::NistReportConfigEutra::BOTH;
                }

              // maxReportCells
              bIterator = DeserializeInteger (&n, 1, MAX_CELL_REPORT, bIterator);
              elem.reportConfigEutra.maxReportCells = n;

              // reportInterval
              bIterator = DeserializeEnum (16, &n, bIterator);
              switch (n)
                {
                case 0:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MS120;
                  break;
                case 1:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MS240;
                  break;
                case 2:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MS480;
                  break;
                case 3:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MS640;
                  break;
                case 4:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MS1024;
                  break;
                case 5:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MS2048;
                  break;
                case 6:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MS5120;
                  break;
                case 7:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MS10240;
                  break;
                case 8:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MIN1;
                  break;
                case 9:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MIN6;
                  break;
                case 10:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MIN12;
                  break;
                case 11:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MIN30;
                  break;
                case 12:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::MIN60;
                  break;
                case 13:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::SPARE3;
                  break;
                case 14:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::SPARE2;
                  break;
                case 15:
                default:
                  elem.reportConfigEutra.reportInterval = NistLteRrcSap::NistReportConfigEutra::SPARE1;
                }

              // reportAmount
              bIterator = DeserializeEnum (8, &n, bIterator);
              switch (n)
                {
                case 0:
                  elem.reportConfigEutra.reportAmount = 1;
                  break;
                case 1:
                  elem.reportConfigEutra.reportAmount = 2;
                  break;
                case 2:
                  elem.reportConfigEutra.reportAmount = 4;
                  break;
                case 3:
                  elem.reportConfigEutra.reportAmount = 8;
                  break;
                case 4:
                  elem.reportConfigEutra.reportAmount = 16;
                  break;
                case 5:
                  elem.reportConfigEutra.reportAmount = 32;
                  break;
                case 6:
                  elem.reportConfigEutra.reportAmount = 64;
                  break;
                default:
                  elem.reportConfigEutra.reportAmount = 0;
                }
            }

          if (reportConfigChoice == 1)
            {
              // ReportConfigInterRAT
              // ...
            }

          measConfig->reportConfigToAddModList.push_back (elem);
        }
    }

  if (bitset11[6])
    {
      // measIdToRemoveList
      int measIdToRemoveListElems;
      bIterator = DeserializeSequenceOf (&measIdToRemoveListElems, MAX_MEAS_ID, 1, bIterator);

      for (int i = 0; i < measIdToRemoveListElems; i++)
        {
          bIterator = DeserializeInteger (&n, 1, MAX_MEAS_ID, bIterator);
          measConfig->measIdToRemoveList.push_back (n);
        }
    }

  if (bitset11[5])
    {
      // measIdToAddModList
      int measIdToAddModListElems;
      bIterator = DeserializeSequenceOf (&measIdToAddModListElems, MAX_MEAS_ID, 1, bIterator);

      for (int i = 0; i < measIdToAddModListElems; i++)
        {
          NistLteRrcSap::NistMeasIdToAddMod elem;

          bIterator = DeserializeSequence (&bitset0, false, bIterator);

          bIterator = DeserializeInteger (&n, 1, MAX_MEAS_ID, bIterator);
          elem.measId = n;

          bIterator = DeserializeInteger (&n, 1, MAX_OBJECT_ID, bIterator);
          elem.measObjectId = n;

          bIterator = DeserializeInteger (&n, 1, MAX_REPORT_CONFIG_ID, bIterator);
          elem.reportConfigId = n;

          measConfig->measIdToAddModList.push_back (elem);
        }
    }

  measConfig->haveNistQuantityConfig = bitset11[4];
  if (measConfig->haveNistQuantityConfig)
    {
      // quantityConfig
      std::bitset<4> quantityConfigOpts;
      bIterator = DeserializeSequence (&quantityConfigOpts, true, bIterator);

      if (quantityConfigOpts[3])
        {
          // quantityConfigEUTRA
          bIterator = DeserializeSequence (&bitset0,false,bIterator);
          bIterator = DeserializeEnum (16, &n, bIterator);
          switch (n)
            {
            case 0:
              measConfig->quantityConfig.filterCoefficientRSRP = 0;
              break;
            case 1:
              measConfig->quantityConfig.filterCoefficientRSRP = 1;
              break;
            case 2:
              measConfig->quantityConfig.filterCoefficientRSRP = 2;
              break;
            case 3:
              measConfig->quantityConfig.filterCoefficientRSRP = 3;
              break;
            case 4:
              measConfig->quantityConfig.filterCoefficientRSRP = 4;
              break;
            case 5:
              measConfig->quantityConfig.filterCoefficientRSRP = 5;
              break;
            case 6:
              measConfig->quantityConfig.filterCoefficientRSRP = 6;
              break;
            case 7:
              measConfig->quantityConfig.filterCoefficientRSRP = 7;
              break;
            case 8:
              measConfig->quantityConfig.filterCoefficientRSRP = 8;
              break;
            case 9:
              measConfig->quantityConfig.filterCoefficientRSRP = 9;
              break;
            case 10:
              measConfig->quantityConfig.filterCoefficientRSRP = 11;
              break;
            case 11:
              measConfig->quantityConfig.filterCoefficientRSRP = 13;
              break;
            case 12:
              measConfig->quantityConfig.filterCoefficientRSRP = 15;
              break;
            case 13:
              measConfig->quantityConfig.filterCoefficientRSRP = 17;
              break;
            case 14:
              measConfig->quantityConfig.filterCoefficientRSRP = 19;
              break;
            case 15:
              measConfig->quantityConfig.filterCoefficientRSRP = 0;
              break;
            default:
              measConfig->quantityConfig.filterCoefficientRSRP = 4;
            }
          bIterator = DeserializeEnum (16, &n, bIterator);
          switch (n)
            {
            case 0:
              measConfig->quantityConfig.filterCoefficientRSRQ = 0;
              break;
            case 1:
              measConfig->quantityConfig.filterCoefficientRSRQ = 1;
              break;
            case 2:
              measConfig->quantityConfig.filterCoefficientRSRQ = 2;
              break;
            case 3:
              measConfig->quantityConfig.filterCoefficientRSRQ = 3;
              break;
            case 4:
              measConfig->quantityConfig.filterCoefficientRSRQ = 4;
              break;
            case 5:
              measConfig->quantityConfig.filterCoefficientRSRQ = 5;
              break;
            case 6:
              measConfig->quantityConfig.filterCoefficientRSRQ = 6;
              break;
            case 7:
              measConfig->quantityConfig.filterCoefficientRSRQ = 7;
              break;
            case 8:
              measConfig->quantityConfig.filterCoefficientRSRQ = 8;
              break;
            case 9:
              measConfig->quantityConfig.filterCoefficientRSRQ = 9;
              break;
            case 10:
              measConfig->quantityConfig.filterCoefficientRSRQ = 11;
              break;
            case 11:
              measConfig->quantityConfig.filterCoefficientRSRQ = 13;
              break;
            case 12:
              measConfig->quantityConfig.filterCoefficientRSRQ = 15;
              break;
            case 13:
              measConfig->quantityConfig.filterCoefficientRSRQ = 17;
              break;
            case 14:
              measConfig->quantityConfig.filterCoefficientRSRQ = 19;
              break;
            case 15:
              measConfig->quantityConfig.filterCoefficientRSRQ = 0;
              break;
            default:
              measConfig->quantityConfig.filterCoefficientRSRQ = 4;
            }
        }
      if (quantityConfigOpts[2])
        {
          // quantityConfigUTRA
          // ...
        }
      if (quantityConfigOpts[1])
        {
          // quantityConfigGERAN
          // ...
        }
      if (quantityConfigOpts[0])
        {
          // quantityConfigCDMA2000
          // ...
        }
    }

  measConfig->haveNistMeasGapConfig = bitset11[3];
  if (measConfig->haveNistMeasGapConfig)
    {
      // measGapConfig
      int measGapConfigChoice;
      bIterator = DeserializeChoice (2, false, &measGapConfigChoice, bIterator);
      switch (measGapConfigChoice)
        {
        case 0:
          measConfig->measGapConfig.type = NistLteRrcSap::NistMeasGapConfig::RESET;
          bIterator = DeserializeNull (bIterator);
          break;
        case 1:
        default:
          measConfig->measGapConfig.type = NistLteRrcSap::NistMeasGapConfig::SETUP;
          bIterator = DeserializeSequence (&bitset0, false, bIterator);

          int gapOffsetChoice;
          bIterator = DeserializeChoice (2, true, &gapOffsetChoice, bIterator);
          switch (gapOffsetChoice)
            {
            case 0:
              measConfig->measGapConfig.gapOffsetChoice = NistLteRrcSap::NistMeasGapConfig::GP0;
              bIterator = DeserializeInteger (&n, 0, 39, bIterator);
              measConfig->measGapConfig.gapOffsetValue = n;
              break;
            case 1:
            default:
              measConfig->measGapConfig.gapOffsetChoice = NistLteRrcSap::NistMeasGapConfig::GP1;
              bIterator = DeserializeInteger (&n, 0, 79, bIterator);
              measConfig->measGapConfig.gapOffsetValue = n;
            }
        }
    }

  measConfig->haveSmeasure = bitset11[2];
  if (measConfig->haveSmeasure)
    {
      // s-Measure
      bIterator = DeserializeInteger (&n, 0, 97, bIterator);
      measConfig->sMeasure = n;
    }

  if (bitset11[1])
    {
      // preRegistrationInfoHRPD
      // ...
    }

  measConfig->haveNistSpeedStatePars = bitset11[0];
  if (measConfig->haveNistSpeedStatePars)
    {
      // speedStatePars
      int speedStateParsChoice;
      bIterator = DeserializeChoice (2, false, &speedStateParsChoice, bIterator);
      switch (speedStateParsChoice)
        {
        case 0:
          measConfig->speedStatePars.type = NistLteRrcSap::NistSpeedStatePars::RESET;
          bIterator = DeserializeNull (bIterator);
          break;
        case 1:
        default:
          measConfig->speedStatePars.type = NistLteRrcSap::NistSpeedStatePars::SETUP;
          bIterator = DeserializeSequence (&bitset0, false, bIterator);

          // Deserialize mobilityStateParameters
          // Deserialize t-Evaluation
          bIterator = DeserializeEnum (8, &n, bIterator);
          switch (n)
            {
            case 0:
              measConfig->speedStatePars.mobilityStateParameters.tEvaluation = 30;
              break;
            case 1:
              measConfig->speedStatePars.mobilityStateParameters.tEvaluation = 60;
              break;
            case 2:
              measConfig->speedStatePars.mobilityStateParameters.tEvaluation = 120;
              break;
            case 3:
              measConfig->speedStatePars.mobilityStateParameters.tEvaluation = 180;
              break;
            case 4:
              measConfig->speedStatePars.mobilityStateParameters.tEvaluation = 240;
              break;
            default:
              measConfig->speedStatePars.mobilityStateParameters.tEvaluation = 0;
            }
          // Deserialize t-HystNormal
          bIterator = DeserializeEnum (8, &n, bIterator);
          switch (n)
            {
            case 0:
              measConfig->speedStatePars.mobilityStateParameters.tHystNormal = 30;
              break;
            case 1:
              measConfig->speedStatePars.mobilityStateParameters.tHystNormal = 60;
              break;
            case 2:
              measConfig->speedStatePars.mobilityStateParameters.tHystNormal = 120;
              break;
            case 3:
              measConfig->speedStatePars.mobilityStateParameters.tHystNormal = 180;
              break;
            case 4:
              measConfig->speedStatePars.mobilityStateParameters.tHystNormal = 240;
              break;
            default:
              measConfig->speedStatePars.mobilityStateParameters.tHystNormal = 0;
            }

          bIterator = DeserializeInteger (&n, 1, 16, bIterator);
          measConfig->speedStatePars.mobilityStateParameters.nCellChangeMedium = n;

          bIterator = DeserializeInteger (&n, 1, 16, bIterator);
          measConfig->speedStatePars.mobilityStateParameters.nCellChangeHigh = n;

          // Deserialize timeToTriggerSf
          bIterator = DeserializeEnum (4, &n, bIterator);
          measConfig->speedStatePars.timeToTriggerSf.sfMedium = (n + 1) * 25;
          bIterator = DeserializeEnum (4, &n, bIterator);
          measConfig->speedStatePars.timeToTriggerSf.sfHigh = (n + 1) * 25;
        }
    }
  return bIterator;
}
//////////////////// NistRrcConnectionRequest class ////////////////////////

// Constructor
NistRrcConnectionRequestHeader::NistRrcConnectionRequestHeader () : NistRrcUlCcchMessage ()
{
  m_mmec = std::bitset<8> (0ul);
  m_mTmsi = std::bitset<32> (0ul);
  m_establishmentCause = MO_SIGNALLING;
  m_spare = std::bitset<1> (0ul);
}

// Destructor
NistRrcConnectionRequestHeader::~NistRrcConnectionRequestHeader ()
{
}

TypeId
NistRrcConnectionRequestHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistRrcConnectionRequestHeader")
    .SetParent<Header> ()
  ;
  return tid;
}

void
NistRrcConnectionRequestHeader::Print (std::ostream &os) const
{
  os << "MMEC:" << m_mmec << std::endl;
  os << "MTMSI:" << m_mTmsi << std::endl;
  os << "EstablishmentCause:" << m_establishmentCause << std::endl;
  os << "Spare: " << m_spare << std::endl;
}

void
NistRrcConnectionRequestHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  SerializeUlCcchMessage (1);

  // Serialize RRCConnectionRequest sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize criticalExtensions choice:
  // 2 options, selected: 0 (option: rrcConnectionRequest-r8)
  SerializeChoice (2,0,false);

  // Serialize RRCConnectionRequest-r8-IEs sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize InitialUE-Identity choice:
  // 2 options, selected: 0 (option: s-TMSI)
  SerializeChoice (2,0,false);

  // Serialize S-TMSI sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize mmec : MMEC ::= BIT STRING (SIZE (8))
  SerializeBitstring (m_mmec);

  // Serialize m-TMSI ::= BIT STRING (SIZE (32))
  SerializeBitstring (m_mTmsi);

  // Serialize establishmentCause : EstablishmentCause ::= ENUMERATED
  SerializeEnum (8,m_establishmentCause);

  // Serialize spare : BIT STRING (SIZE (1))
  SerializeBitstring (std::bitset<1> ());

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistRrcConnectionRequestHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<1> dummy;
  std::bitset<0> optionalOrDefaultMask;
  int selectedOption;

  bIterator = DeserializeUlCcchMessage (bIterator);

  // Deserialize RCConnectionRequest sequence
  bIterator = DeserializeSequence (&optionalOrDefaultMask,false,bIterator);

  // Deserialize criticalExtensions choice:
  bIterator = DeserializeChoice (2,false,&selectedOption,bIterator);

  // Deserialize RRCConnectionRequest-r8-IEs sequence
  bIterator = DeserializeSequence (&optionalOrDefaultMask,false,bIterator);

  // Deserialize InitialUE-Identity choice
  bIterator = DeserializeChoice (2,false,&selectedOption,bIterator);

  // Deserialize S-TMSI sequence
  bIterator = DeserializeSequence (&optionalOrDefaultMask,false,bIterator);

  // Deserialize mmec
  bIterator = DeserializeBitstring (&m_mmec,bIterator);

  // Deserialize m-TMSI
  bIterator = DeserializeBitstring (&m_mTmsi,bIterator);

  // Deserialize establishmentCause
  bIterator = DeserializeEnum (8,&selectedOption,bIterator);

  // Deserialize spare
  bIterator = DeserializeBitstring (&dummy,bIterator);

  return GetSerializedSize ();
}

void
NistRrcConnectionRequestHeader::SetMessage (NistLteRrcSap::NistRrcConnectionRequest msg)
{
  m_mTmsi = std::bitset<32> ((uint32_t)msg.ueIdentity);
  m_mmec = std::bitset<8> ((uint32_t)(msg.ueIdentity >> 32));
  m_isDataSerialized = false;
}

NistLteRrcSap::NistRrcConnectionRequest
NistRrcConnectionRequestHeader::GetMessage () const
{
  NistLteRrcSap::NistRrcConnectionRequest msg;
  msg.ueIdentity = (((uint64_t) m_mmec.to_ulong ()) << 32) | (m_mTmsi.to_ulong ());

  return msg;
}

std::bitset<8>
NistRrcConnectionRequestHeader::GetMmec () const
{
  return m_mmec;
}

std::bitset<32>
NistRrcConnectionRequestHeader::GetMtmsi () const
{
  return m_mTmsi;
}


//////////////////// NistRrcConnectionSetup class ////////////////////////
NistRrcConnectionSetupHeader::NistRrcConnectionSetupHeader ()
{
}
 
NistRrcConnectionSetupHeader::~NistRrcConnectionSetupHeader ()
{
}
 
void
NistRrcConnectionSetupHeader::Print (std::ostream &os) const
{
  os << "rrcTransactionIdentifier: " << (int)m_rrcTransactionIdentifier << std::endl;
  os << "radioResourceConfigDedicated:" << std::endl;
  NistRrcAsn1Header::Print (os,m_radioResourceConfigDedicated);
}

void
NistRrcConnectionSetupHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  SerializeDlCcchMessage (3);

  SerializeInteger (15,0,15);

  // Serialize RRCConnectionSetup sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize rrc-TransactionIdentifier ::=INTEGER (0..3)
  SerializeInteger (m_rrcTransactionIdentifier,0,3);

  // Serialize criticalExtensions choice:
  // 2 options, selected: 0 (option: c1)
  SerializeChoice (2,0,false);

  // Serialize c1 choice:
  // 8 options, selected: 0 (option: rrcConnectionSetup-r8)
  SerializeChoice (8,0,false);

  // Serialize rrcConnectionSetup-r8 sequence
  // 1 optional fields (not present). Extension marker not present.
  SerializeSequence (std::bitset<1> (0),false);

  // Serialize NistRadioResourceConfigDedicated sequence
  SerializeNistRadioResourceConfigDedicated (m_radioResourceConfigDedicated);

  // Serialize nonCriticalExtension sequence
  // 2 optional fields, none present. No extension marker.
  SerializeSequence (std::bitset<2> (0),false);

  // Finish serialization
  FinalizeSerialization ();
}


uint32_t
NistRrcConnectionSetupHeader::Deserialize (Buffer::Iterator bIterator)
{
  int n;

  std::bitset<0> bitset0;
  std::bitset<1> bitset1;
  std::bitset<2> bitset2;

  bIterator = DeserializeDlCcchMessage (bIterator);

  bIterator = DeserializeInteger (&n,0,15,bIterator);

  // Deserialize RRCConnectionSetup sequence
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // Deserialize rrc-TransactionIdentifier ::=INTEGER (0..3)
  bIterator = DeserializeInteger (&n,0,3,bIterator);
  m_rrcTransactionIdentifier = n;

  // Deserialize criticalExtensions choice
  int criticalExtensionChoice;
  bIterator = DeserializeChoice (2,false,&criticalExtensionChoice,bIterator);
  if (criticalExtensionChoice == 1)
    {
      // Deserialize criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if (criticalExtensionChoice == 0)
    {
      // Deserialize c1
      int c1;
      bIterator = DeserializeChoice (8,false,&c1,bIterator);

      if (c1 > 0)
        {
          // Deserialize spareX , X:=7..1
          bIterator = DeserializeNull (bIterator);
        }
      else if (c1 == 0)
        {
          // Deserialize rrcConnectionSetup-r8
          // 1 optional fields, no extension marker.
          bIterator = DeserializeSequence (&bitset1,false,bIterator);

          // Deserialize radioResourceConfigDedicated
          bIterator = DeserializeNistRadioResourceConfigDedicated (&m_radioResourceConfigDedicated,bIterator);

          if (bitset1[0])
            {
              // Deserialize nonCriticalExtension
              // 2 optional fields, no extension marker.
              bIterator = DeserializeSequence (&bitset2,false,bIterator);

              // Deserialization of lateR8NonCriticalExtension and nonCriticalExtension
              // ...
            }
        }
    }
  return GetSerializedSize ();
}

void
NistRrcConnectionSetupHeader::SetMessage (NistLteRrcSap::NistRrcConnectionSetup msg)
{
  m_rrcTransactionIdentifier = msg.rrcTransactionIdentifier;
  m_radioResourceConfigDedicated = msg.radioResourceConfigDedicated;
  m_isDataSerialized = false;
}

NistLteRrcSap::NistRrcConnectionSetup
NistRrcConnectionSetupHeader::GetMessage () const
{
  NistLteRrcSap::NistRrcConnectionSetup msg;
  msg.rrcTransactionIdentifier = m_rrcTransactionIdentifier;
  msg.radioResourceConfigDedicated = m_radioResourceConfigDedicated; 
  return msg;
}

uint8_t
NistRrcConnectionSetupHeader::GetRrcTransactionIdentifier () const
{
  return m_rrcTransactionIdentifier;
}

bool
NistRrcConnectionSetupHeader::HaveNistPhysicalConfigDedicated () const
{
  return m_radioResourceConfigDedicated.haveNistPhysicalConfigDedicated;
}

std::list<NistLteRrcSap::NistSrbToAddMod>
NistRrcConnectionSetupHeader::GetNistSrbToAddModList () const
{
  return m_radioResourceConfigDedicated.srbToAddModList;
}

std::list<NistLteRrcSap::NistDrbToAddMod>
NistRrcConnectionSetupHeader::GetNistDrbToAddModList () const
{
  return m_radioResourceConfigDedicated.drbToAddModList;
} 

std::list<uint8_t>
NistRrcConnectionSetupHeader::GetDrbToReleaseList () const
{
  return m_radioResourceConfigDedicated.drbToReleaseList;
}

NistLteRrcSap::NistPhysicalConfigDedicated
NistRrcConnectionSetupHeader::GetNistPhysicalConfigDedicated () const
{
  return m_radioResourceConfigDedicated.physicalConfigDedicated;
}

NistLteRrcSap::NistRadioResourceConfigDedicated
NistRrcConnectionSetupHeader::GetNistRadioResourceConfigDedicated () const
{
  return m_radioResourceConfigDedicated;
}

//////////////////// NistRrcConnectionSetupCompleteHeader class ////////////////////////

NistRrcConnectionSetupCompleteHeader::NistRrcConnectionSetupCompleteHeader ()
{
}

NistRrcConnectionSetupCompleteHeader::~NistRrcConnectionSetupCompleteHeader ()
{
}

void
NistRrcConnectionSetupCompleteHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  // Serialize DCCH message
  SerializeUlDcchMessage (4);

  // Serialize RRCConnectionSetupComplete sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize rrc-TransactionIdentifier
  SerializeInteger (m_rrcTransactionIdentifier,0,3);

  // Serialize criticalExtensions choice
  // 2 options, selected 0 (c1)
  SerializeChoice (2,0,false);

  // Choose spare3 NULL
  SerializeChoice (4,1,false);

  // Serialize spare3 NULL
  SerializeNull ();

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistRrcConnectionSetupCompleteHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;

  bIterator = DeserializeUlDcchMessage (bIterator);

  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  int n;
  bIterator = DeserializeInteger (&n,0,3,bIterator);
  m_rrcTransactionIdentifier = n;

  bIterator = DeserializeChoice (2,false,&n,bIterator);

  if (n == 1)
    {
      // Deserialize criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if (n == 0)
    {
      // Deserialize c1
      int c1Chosen;
      bIterator = DeserializeChoice (4,false,&c1Chosen,bIterator);

      if (c1Chosen == 0)
        {
          // Deserialize rrcConnectionSetupComplete-r8
          // ...
        }
      else
        {
          bIterator = DeserializeNull (bIterator);
        }
    }

  return GetSerializedSize ();
}

void
NistRrcConnectionSetupCompleteHeader::Print (std::ostream &os) const
{
  os << "rrcTransactionIdentifier: " << (int) m_rrcTransactionIdentifier << std::endl;
}

void
NistRrcConnectionSetupCompleteHeader::SetMessage (NistLteRrcSap::NistRrcConnectionSetupCompleted msg)
{
  m_rrcTransactionIdentifier = msg.rrcTransactionIdentifier;
  m_isDataSerialized = false;
}

uint8_t
NistRrcConnectionSetupCompleteHeader::GetRrcTransactionIdentifier () const
{
  return m_rrcTransactionIdentifier;
}

NistLteRrcSap::NistRrcConnectionSetupCompleted
NistRrcConnectionSetupCompleteHeader::GetMessage () const
{
  NistLteRrcSap::NistRrcConnectionSetupCompleted msg;
  msg.rrcTransactionIdentifier = m_rrcTransactionIdentifier;
  return msg;
}

//////////////////// NistRrcConnectionReconfigurationCompleteHeader class ////////////////////////

NistRrcConnectionReconfigurationCompleteHeader::NistRrcConnectionReconfigurationCompleteHeader ()
{
}

NistRrcConnectionReconfigurationCompleteHeader::~NistRrcConnectionReconfigurationCompleteHeader ()
{
}

void
NistRrcConnectionReconfigurationCompleteHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  // Serialize DCCH message
  SerializeUlDcchMessage (2);

  // Serialize RRCConnectionSetupComplete sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize rrc-TransactionIdentifier
  SerializeInteger (m_rrcTransactionIdentifier,0,3);

  // Serialize criticalExtensions choice
  // 2 options, selected 1 (criticalExtensionsFuture)
  SerializeChoice (2,1,false);

  // Choose criticalExtensionsFuture
  SerializeSequence (std::bitset<0> (),false);

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistRrcConnectionReconfigurationCompleteHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeUlDcchMessage (bIterator);
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  bIterator = DeserializeInteger (&n,0,3,bIterator);
  m_rrcTransactionIdentifier = n;

  bIterator = DeserializeChoice (2,false,&n,bIterator);

  if (n == 1)
    {
      // Deserialize criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if (n == 0)
    {
      // Deserialize rrcConnectionReconfigurationComplete-r8
      // ...
    }

  return GetSerializedSize ();
}

void
NistRrcConnectionReconfigurationCompleteHeader::Print (std::ostream &os) const
{
  os << "rrcTransactionIdentifier: " << (int) m_rrcTransactionIdentifier << std::endl;
}

void
NistRrcConnectionReconfigurationCompleteHeader::SetMessage (NistLteRrcSap::NistRrcConnectionReconfigurationCompleted msg)
{
  m_rrcTransactionIdentifier = msg.rrcTransactionIdentifier;
  m_isDataSerialized = false;
}

NistLteRrcSap::NistRrcConnectionReconfigurationCompleted
NistRrcConnectionReconfigurationCompleteHeader::GetMessage () const
{
  NistLteRrcSap::NistRrcConnectionReconfigurationCompleted msg;
  msg.rrcTransactionIdentifier = m_rrcTransactionIdentifier;
  return msg;
}

uint8_t
NistRrcConnectionReconfigurationCompleteHeader::GetRrcTransactionIdentifier () const
{
  return m_rrcTransactionIdentifier;
}

//////////////////// NistRrcConnectionReconfigurationHeader class ////////////////////////

NistRrcConnectionReconfigurationHeader::NistRrcConnectionReconfigurationHeader ()
{
}

NistRrcConnectionReconfigurationHeader::~NistRrcConnectionReconfigurationHeader ()
{
}

void
NistRrcConnectionReconfigurationHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  SerializeDlDcchMessage (4);

  // Serialize RRCConnectionSetupComplete sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize rrc-TransactionIdentifier
  SerializeInteger (m_rrcTransactionIdentifier,0,3);

  // Serialize criticalExtensions choice
  // 2 options, selected 0 (c1)
  SerializeChoice (2,0,false);

  // Serialize c1 choice
  // 8 options, selected 0 (rrcConnectionReconfiguration-r8)
  SerializeChoice (8,0,false);

  // Serialize RRCConnectionReconfiguration-r8-IEs sequence:
  // 6 optional fields. Extension marker not present.
  std::bitset<6> options;
  options.set (5,m_haveNistMeasConfig);
  options.set (4,m_haveNistMobilityControlInfo);
  options.set (3,0); // No dedicatedInfoNASList
  options.set (2,m_haveNistRadioResourceConfigDedicated);
  options.set (1,0); // No securityConfigHO
  options.set (0,0); // No nonCriticalExtension
  SerializeSequence (options,false);

  if (m_haveNistMeasConfig)
    {
      SerializeNistMeasConfig (m_measConfig);
    }

  if (m_haveNistMobilityControlInfo)
    {
      // Serialize NistMobilityControlInfo

      // 4 optional fields, extension marker present.
      std::bitset<4> mobCtrlIntoOptional;
      mobCtrlIntoOptional.set (3,m_mobilityControlInfo.haveCarrierFreq);
      mobCtrlIntoOptional.set (2,m_mobilityControlInfo.haveCarrierBandwidth);
      mobCtrlIntoOptional.set (1,0); // No additionalSpectrumEmission
      mobCtrlIntoOptional.set (0,m_mobilityControlInfo.haveNistRachConfigDedicated);
      SerializeSequence (mobCtrlIntoOptional,true);

      // Serialize targetPhysCellId
      SerializeInteger (m_mobilityControlInfo.targetPhysCellId,0,503);

      if (m_mobilityControlInfo.haveCarrierFreq)
        {
          SerializeSequence (std::bitset<1> (1),false);
          SerializeInteger (m_mobilityControlInfo.carrierFreq.dlCarrierFreq,0,MAX_EARFCN);
          SerializeInteger (m_mobilityControlInfo.carrierFreq.ulCarrierFreq,0,MAX_EARFCN);
        }

      if (m_mobilityControlInfo.haveCarrierBandwidth)
        {
          SerializeSequence (std::bitset<1> (1),false);

          // Serialize dl-Bandwidth
          switch (m_mobilityControlInfo.carrierBandwidth.dlBandwidth)
            {
            case 6:
              SerializeEnum (16,0);
              break;
            case 15:
              SerializeEnum (16,1);
              break;
            case 25:
              SerializeEnum (16,2);
              break;
            case 50:
              SerializeEnum (16,3);
              break;
            case 75:
              SerializeEnum (16,4);
              break;
            case 100:
              SerializeEnum (16,5);
              break;
            default:
              SerializeEnum (16,6);
            }

          // Serialize ul-Bandwidth
          switch (m_mobilityControlInfo.carrierBandwidth.ulBandwidth)
            {
            case 6:
              SerializeEnum (16,0);
              break;
            case 15:
              SerializeEnum (16,1);
              break;
            case 25:
              SerializeEnum (16,2);
              break;
            case 50:
              SerializeEnum (16,3);
              break;
            case 75:
              SerializeEnum (16,4);
              break;
            case 100:
              SerializeEnum (16,5);
              break;
            default:
              SerializeEnum (16,6);
            }
        }

      // Serialize t304
      SerializeEnum (8,0);

      // Serialize newUE-Identitiy
      SerializeBitstring (std::bitset<16> (m_mobilityControlInfo.newUeIdentity));

      // Serialize radioResourceConfigCommon
      SerializeNistRadioResourceConfigCommon (m_mobilityControlInfo.radioResourceConfigCommon);

      if (m_mobilityControlInfo.haveNistRachConfigDedicated)
        {
          SerializeSequence (std::bitset<0> (),false);
          SerializeInteger (m_mobilityControlInfo.rachConfigDedicated.raPreambleIndex,0,63);
          SerializeInteger (m_mobilityControlInfo.rachConfigDedicated.raPrachMaskIndex,0,15);
        }
    }

  if (m_haveNistRadioResourceConfigDedicated)
    {
      // Serialize NistRadioResourceConfigDedicated
      SerializeNistRadioResourceConfigDedicated (m_radioResourceConfigDedicated);
    }

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistRrcConnectionReconfigurationHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;

  bIterator = DeserializeDlDcchMessage (bIterator);

  // RRCConnectionReconfiguration sequence
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // rrc-TransactionIdentifier
  int n;
  bIterator = DeserializeInteger (&n,0,3,bIterator);
  m_rrcTransactionIdentifier = n;

  // criticalExtensions
  int sel;
  bIterator = DeserializeChoice (2,false,&sel,bIterator);
  if (sel == 1)
    {
      // criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if (sel == 0)
    {
      // c1
      int c1Chosen;
      bIterator = DeserializeChoice (8,false,&c1Chosen,bIterator);
      if (c1Chosen > 0)
        {
          bIterator = DeserializeNull (bIterator);
        }
      else if (c1Chosen == 0)
        {
          // rrcConnectionReconfiguration-r8
          std::bitset<6> rrcConnRecOpts;
          bIterator = DeserializeSequence (&rrcConnRecOpts,false,bIterator);

          m_haveNistMeasConfig = rrcConnRecOpts[5];
          if (m_haveNistMeasConfig)
            {
              bIterator = DeserializeNistMeasConfig (&m_measConfig, bIterator);
            }

          m_haveNistMobilityControlInfo = rrcConnRecOpts[4];
          if (m_haveNistMobilityControlInfo)
            {
              // mobilityControlInfo
              std::bitset<4> mobCtrlOpts;
              bIterator = DeserializeSequence (&mobCtrlOpts,true,bIterator);

              // PhysCellId
              bIterator = DeserializeInteger (&n,0,503,bIterator);
              m_mobilityControlInfo.targetPhysCellId = n;

              // carrierFreq
              m_mobilityControlInfo.haveCarrierFreq = mobCtrlOpts[3];
              if (m_mobilityControlInfo.haveCarrierFreq)
                {
                  std::bitset<1> ulCarrierFreqPresent;
                  bIterator = DeserializeSequence (&ulCarrierFreqPresent,false,bIterator);

                  bIterator = DeserializeInteger (&n,0,MAX_EARFCN,bIterator);
                  m_mobilityControlInfo.carrierFreq.dlCarrierFreq = n;

                  if (ulCarrierFreqPresent[0])
                    {
                      bIterator = DeserializeInteger (&n,0,MAX_EARFCN,bIterator);
                      m_mobilityControlInfo.carrierFreq.ulCarrierFreq = n;
                    }
                }

              // carrierBandwidth
              m_mobilityControlInfo.haveCarrierBandwidth = mobCtrlOpts[2];
              if (m_mobilityControlInfo.haveCarrierBandwidth)
                {
                  std::bitset<1> ulBandwidthPresent;
                  bIterator = DeserializeSequence (&ulBandwidthPresent,false,bIterator);

                  bIterator = DeserializeEnum (16,&n,bIterator);
                  switch (n)
                    {
                    case 0:
                      m_mobilityControlInfo.carrierBandwidth.dlBandwidth = 6;
                      break;
                    case 1:
                      m_mobilityControlInfo.carrierBandwidth.dlBandwidth = 15;
                      break;
                    case 2:
                      m_mobilityControlInfo.carrierBandwidth.dlBandwidth = 25;
                      break;
                    case 3:
                      m_mobilityControlInfo.carrierBandwidth.dlBandwidth = 50;
                      break;
                    case 4:
                      m_mobilityControlInfo.carrierBandwidth.dlBandwidth = 75;
                      break;
                    case 5:
                      m_mobilityControlInfo.carrierBandwidth.dlBandwidth = 100;
                      break;
                    case 6:
                      m_mobilityControlInfo.carrierBandwidth.dlBandwidth = 0;
                      break;
                    }

                  if (ulBandwidthPresent[0])
                    {
                      bIterator = DeserializeEnum (16,&n,bIterator);
                      switch (n)
                        {
                        case 0:
                          m_mobilityControlInfo.carrierBandwidth.ulBandwidth = 6;
                          break;
                        case 1:
                          m_mobilityControlInfo.carrierBandwidth.ulBandwidth = 15;
                          break;
                        case 2:
                          m_mobilityControlInfo.carrierBandwidth.ulBandwidth = 25;
                          break;
                        case 3:
                          m_mobilityControlInfo.carrierBandwidth.ulBandwidth = 50;
                          break;
                        case 4:
                          m_mobilityControlInfo.carrierBandwidth.ulBandwidth = 75;
                          break;
                        case 5:
                          m_mobilityControlInfo.carrierBandwidth.ulBandwidth = 100;
                          break;
                        case 6:
                          m_mobilityControlInfo.carrierBandwidth.ulBandwidth = 0;
                          break;
                        }
                    }
                }

              // additionalSpectrumEmission
              if (mobCtrlOpts[1])
                {
                  // ...
                }

              // t304
              bIterator = DeserializeEnum (8,&n,bIterator);

              // newUE-Identity
              std::bitset<16> cRnti;
              bIterator = DeserializeBitstring (&cRnti, bIterator);
              m_mobilityControlInfo.newUeIdentity = cRnti.to_ulong ();

              // radioResourceConfigCommon
              bIterator = DeserializeNistRadioResourceConfigCommon (&m_mobilityControlInfo.radioResourceConfigCommon, bIterator);

              m_mobilityControlInfo.haveNistRachConfigDedicated = mobCtrlOpts[0];
              if (m_mobilityControlInfo.haveNistRachConfigDedicated)
                {
                  bIterator = DeserializeSequence (&bitset0, false, bIterator);
                  bIterator = DeserializeInteger (&n,0,63, bIterator);
                  m_mobilityControlInfo.rachConfigDedicated.raPreambleIndex = n;
                  bIterator = DeserializeInteger (&n,0,15, bIterator);
                  m_mobilityControlInfo.rachConfigDedicated.raPrachMaskIndex = n;
                }
            }

          // dedicatedInfoNASList
          if (rrcConnRecOpts[3])
            {
              // ...
            }

          // radioResourceConfigDedicated
          m_haveNistRadioResourceConfigDedicated = rrcConnRecOpts[2];
          if (m_haveNistRadioResourceConfigDedicated)
            {
              bIterator = DeserializeNistRadioResourceConfigDedicated (&m_radioResourceConfigDedicated,bIterator);
            }

          // securityConfigHO
          if (rrcConnRecOpts[1])
            {
              // ...
            }

          // nonCriticalExtension
          if (rrcConnRecOpts[0])
            {
              // ...
            }
        }
    }

  return GetSerializedSize ();
}

void
NistRrcConnectionReconfigurationHeader::Print (std::ostream &os) const
{
  os << "rrcTransactionIdentifier: " << (int) m_rrcTransactionIdentifier << std::endl;
  os << "haveNistMeasConfig: " << m_haveNistMeasConfig << std::endl;
  if (m_haveNistMeasConfig)
    {
      if (!m_measConfig.measObjectToRemoveList.empty ())
        {
          os << "  measObjectToRemoveList: ";
          std::list<uint8_t> auxList = m_measConfig.measObjectToRemoveList;
          std::list<uint8_t>::iterator it = auxList.begin ();
          for (; it != auxList.end (); it++)
            {
              os << (int) *it << ", ";
            }
          os << std::endl;
        }
      if (!m_measConfig.reportConfigToRemoveList.empty ())
        {
          os << "  reportConfigToRemoveList: ";
          std::list<uint8_t> auxList = m_measConfig.reportConfigToRemoveList;
          std::list<uint8_t>::iterator it = auxList.begin ();
          for (; it != auxList.end (); it++)
            {
              os << (int) *it << ", ";
            }
          os << std::endl;
        }
      if (!m_measConfig.measIdToRemoveList.empty ())
        {
          os << "  measIdToRemoveList: ";
          std::list<uint8_t> auxList = m_measConfig.measIdToRemoveList;
          std::list<uint8_t>::iterator it = auxList.begin ();
          for (; it != auxList.end (); it++)
            {
              os << (int) *it << ", ";
            }
          os << std::endl;
        }

      if (!m_measConfig.measObjectToAddModList.empty ())
        {
          os << "  measObjectToAddMod: " << std::endl;
          std::list<NistLteRrcSap::NistMeasObjectToAddMod> auxList = m_measConfig.measObjectToAddModList;
          std::list<NistLteRrcSap::NistMeasObjectToAddMod>::iterator it = auxList.begin ();
          for (; it != auxList.end (); it++)
            {
              os << "    measObjectId: " << (int)it->measObjectId << std::endl;
              os << "    carrierFreq: " << (int)it->measObjectEutra.carrierFreq << std::endl;
              os << "    allowedMeasBandwidth: " <<  (int)it->measObjectEutra.allowedMeasBandwidth << std::endl;
              os << "    presenceAntennaPort1: " <<  it->measObjectEutra.presenceAntennaPort1  << std::endl;
              os << "    neighCellConfig: " << (int) it->measObjectEutra.neighCellConfig  << std::endl;
              os << "    offsetFreq: " <<  (int)it->measObjectEutra.offsetFreq  << std::endl;


              if (!it->measObjectEutra.cellsToRemoveList.empty ())
                {
                  os << "    cellsToRemoveList: ";
                  std::list<uint8_t> auxList = it->measObjectEutra.cellsToRemoveList;
                  std::list<uint8_t>::iterator it = auxList.begin ();
                  for (; it != auxList.end (); it++)
                    {
                      os << (int) *it << ", ";
                    }
                  os << std::endl;
                }

              if (!it->measObjectEutra.blackCellsToRemoveList.empty ())
                {
                  os << "    blackCellsToRemoveList: ";
                  std::list<uint8_t> auxList = it->measObjectEutra.blackCellsToRemoveList;
                  std::list<uint8_t>::iterator it = auxList.begin ();
                  for (; it != auxList.end (); it++)
                    {
                      os << (int) *it << ", ";
                    }
                  os << std::endl;
                }

              if (!it->measObjectEutra.cellsToAddModList.empty ())
                {
                  os << "    cellsToAddModList: " << std::endl;
                  std::list<NistLteRrcSap::NistCellsToAddMod> auxList = it->measObjectEutra.cellsToAddModList;
                  std::list<NistLteRrcSap::NistCellsToAddMod>::iterator it = auxList.begin ();
                  for (; it != auxList.end (); it++)
                    {
                      os << "      cellIndex: " <<  (int)it->cellIndex << std::endl;
                      os << "      physCellId: " <<  (int)it->physCellId  << std::endl;
                      os << "      cellIndividualOffset: " <<  (int)it->cellIndividualOffset << std::endl;
                      os << "      ------ " << std::endl;
                    }
                }

              if (!it->measObjectEutra.blackNistCellsToAddModList.empty ())
                {
                  os << "    blackNistCellsToAddModList: " << std::endl;
                  std::list<NistLteRrcSap::BlackNistCellsToAddMod> auxList = it->measObjectEutra.blackNistCellsToAddModList;
                  std::list<NistLteRrcSap::BlackNistCellsToAddMod>::iterator it = auxList.begin ();
                  for (; it != auxList.end (); it++)
                    {
                      os << "      cellIndex: " <<  (int)it->cellIndex << std::endl;
                      os << "      physCellIdRange.start: " <<  (int)it->physCellIdRange.start  << std::endl;
                      os << "      physCellIdRange.haveRange: " <<  it->physCellIdRange.haveRange << std::endl;
                      os << "      physCellIdRange.range: " <<  (int)it->physCellIdRange.range << std::endl;
                      os << "      ------ " << std::endl;
                    }
                }

              os << "    haveCellForWhichToReportCGI: " <<  it->measObjectEutra.haveCellForWhichToReportCGI  << std::endl;
              os << "    cellForWhichToReportCGI: " <<  (int)it->measObjectEutra.cellForWhichToReportCGI  << std::endl;
              os << "    ------------- " << std::endl;
            }

        }

      if (!m_measConfig.reportConfigToAddModList.empty ())
        {
          os << "  reportConfigToAddModList: " << std::endl;
          std::list<NistLteRrcSap::NistReportConfigToAddMod> auxList = m_measConfig.reportConfigToAddModList;
          std::list<NistLteRrcSap::NistReportConfigToAddMod>::iterator it = auxList.begin ();
          for (; it != auxList.end (); it++)
            {
              os << "    reportConfigId: " << (int)it->reportConfigId << std::endl;
              os << "    reportConfigEutra.triggerType  " <<  (int)it->reportConfigEutra.triggerType << std::endl;
              if (it->reportConfigEutra.triggerType == NistLteRrcSap::NistReportConfigEutra::EVENT)
                {
                  os << "    reportConfigEutra.eventId  " <<  (int)it->reportConfigEutra.eventId << std::endl;
                  if (it->reportConfigEutra.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A3)
                    {
                      os << "    reportConfigEutra.reportOnLeave  " <<  (int)it->reportConfigEutra.reportOnLeave << std::endl;
                      os << "    reportConfigEutra.a3Offset  " <<  (int)it->reportConfigEutra.a3Offset << std::endl;
                    }
                  else
                    {
                      os << "    reportConfigEutra.threshold1.choice  " <<  (int)it->reportConfigEutra.threshold1.choice << std::endl;
                      os << "    reportConfigEutra.threshold1.range  " <<  (int)it->reportConfigEutra.threshold1.range << std::endl;
                      if (it->reportConfigEutra.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A5)
                        {
                          os << "    reportConfigEutra.threshold2.choice  " <<  (int)it->reportConfigEutra.threshold2.choice << std::endl;
                          os << "    reportConfigEutra.threshold2.range  " <<  (int)it->reportConfigEutra.threshold2.range << std::endl;
                        }
                    }
                  os << "    reportConfigEutra.hysteresis  " <<  (int)it->reportConfigEutra.hysteresis << std::endl;
                  os << "    reportConfigEutra.timeToTrigger  " <<  (int)it->reportConfigEutra.timeToTrigger << std::endl;
                }
              else
                {
                  os << "    reportConfigEutra.purpose  " <<  (int)it->reportConfigEutra.purpose << std::endl;
                }
              os << "    reportConfigEutra.triggerQuantity  " <<  (int)it->reportConfigEutra.triggerQuantity << std::endl;
              os << "    reportConfigEutra.reportQuantity  " <<  (int)it->reportConfigEutra.reportQuantity << std::endl;
              os << "    reportConfigEutra.maxReportCells  " <<  (int)it->reportConfigEutra.maxReportCells << std::endl;
              os << "    reportConfigEutra.reportInterval  " <<  (int)it->reportConfigEutra.reportInterval << std::endl;
              os << "    reportConfigEutra.reportAmount  " <<  (int)it->reportConfigEutra.reportAmount << std::endl;
            }
        }

      if (!m_measConfig.measIdToAddModList.empty ())
        {
          os << "  measIdToAddModList: " << std::endl;
          std::list<NistLteRrcSap::NistMeasIdToAddMod> auxList = m_measConfig.measIdToAddModList;
          std::list<NistLteRrcSap::NistMeasIdToAddMod>::iterator it = auxList.begin ();
          for (; it != auxList.end (); it++)
            {
              os << "    measId: " << (int)it->measId << std::endl;
              os << "    measObjectId: " << (int)it->measObjectId << std::endl;
              os << "    reportConfigId: " << (int)it->reportConfigId << std::endl;
              os << "    ------ " << std::endl;
            }
        }

      os << "  haveNistQuantityConfig: " << m_measConfig.haveNistQuantityConfig << std::endl;
      if (m_measConfig.haveNistQuantityConfig)
        {
          os << "    filterCoefficientRSRP: " << (int)m_measConfig.quantityConfig.filterCoefficientRSRP << std::endl;
          os << "    filterCoefficientRSRQ:" << (int)m_measConfig.quantityConfig.filterCoefficientRSRQ  << std::endl;
        }

      os << "  haveNistMeasGapConfig: " << m_measConfig.haveNistMeasGapConfig << std::endl;
      if (m_measConfig.haveNistMeasGapConfig)
        {
          os << "    measGapConfig.type: " << m_measConfig.measGapConfig.type << std::endl;
          os << "    measGapConfig.gap (gap0/1,value): (" << m_measConfig.measGapConfig.gapOffsetChoice
             << "," << (int) m_measConfig.measGapConfig.gapOffsetValue << ")" << std::endl;
        }

      os << "  haveSmeasure: " << m_measConfig.haveSmeasure << std::endl;
      if (m_measConfig.haveSmeasure)
        {
          os << "    sMeasure: " << (int) m_measConfig.sMeasure  << std::endl;
        }

      os << "  haveNistSpeedStatePars: " << m_measConfig.haveNistSpeedStatePars << std::endl;
      if (m_measConfig.haveNistSpeedStatePars)
        {
          os << "    speedStatePars.type: " << m_measConfig.speedStatePars.type << std::endl;
          os << "    speedStatePars.mobilityStateParameters.tEvaluation: " << (int)m_measConfig.speedStatePars.mobilityStateParameters.tEvaluation << std::endl;
          os << "    speedStatePars.mobilityStateParameters.tHystNormal: " << (int)m_measConfig.speedStatePars.mobilityStateParameters.tHystNormal << std::endl;
          os << "    speedStatePars.mobilityStateParameters.nCellChangeMedium: " << (int)m_measConfig.speedStatePars.mobilityStateParameters.nCellChangeMedium << std::endl;
          os << "    speedStatePars.mobilityStateParameters.nCellChangeHigh: " << (int)m_measConfig.speedStatePars.mobilityStateParameters.nCellChangeHigh << std::endl;
          os << "    speedStatePars.timeToTriggerSf.sfMedium: " << (int)m_measConfig.speedStatePars.timeToTriggerSf.sfMedium << std::endl;
          os << "    speedStatePars.timeToTriggerSf.sfHigh: " << (int)m_measConfig.speedStatePars.timeToTriggerSf.sfHigh << std::endl;
        }
    }

  os << "haveNistMobilityControlInfo: " << m_haveNistMobilityControlInfo << std::endl;
  if (m_haveNistMobilityControlInfo)
    {
      os << "targetPhysCellId: " << (int)m_mobilityControlInfo.targetPhysCellId << std::endl;
      os << "haveCarrierFreq: " << m_mobilityControlInfo.haveCarrierFreq << std::endl;
      if (m_mobilityControlInfo.haveCarrierFreq)
        {
          os << "  carrierFreq.dlCarrierFreq: " << (int) m_mobilityControlInfo.carrierFreq.dlCarrierFreq << std::endl;
          os << "  carrierFreq.dlCarrierFreq: " << (int) m_mobilityControlInfo.carrierFreq.ulCarrierFreq << std::endl;
        }
      os << "haveCarrierBandwidth: " << m_mobilityControlInfo.haveCarrierBandwidth << std::endl;
      if (m_mobilityControlInfo.haveCarrierBandwidth)
        {
          os << "  carrierBandwidth.dlBandwidth: " << (int) m_mobilityControlInfo.carrierBandwidth.dlBandwidth << std::endl;
          os << "  carrierBandwidth.ulBandwidth: " << (int) m_mobilityControlInfo.carrierBandwidth.ulBandwidth << std::endl;
        }
      os << "newUeIdentity: " << (int) m_mobilityControlInfo.newUeIdentity << std::endl;
      os << "haveNistRachConfigDedicated: " << m_mobilityControlInfo.haveNistRachConfigDedicated << std::endl;
      if (m_mobilityControlInfo.haveNistRachConfigDedicated)
        {
          os << "raPreambleIndex: " << (int) m_mobilityControlInfo.rachConfigDedicated.raPreambleIndex << std::endl;
          os << "raPrachMaskIndex: "  << (int) m_mobilityControlInfo.rachConfigDedicated.raPrachMaskIndex << std::endl;
        }
    }
  os << "haveNistRadioResourceConfigDedicated: " << m_haveNistRadioResourceConfigDedicated << std::endl;
  if (m_haveNistRadioResourceConfigDedicated)
    {
      NistRrcAsn1Header::Print (os,m_radioResourceConfigDedicated);
    }
}

void
NistRrcConnectionReconfigurationHeader::SetMessage (NistLteRrcSap::NistRrcConnectionReconfiguration msg)
{
  m_rrcTransactionIdentifier = msg.rrcTransactionIdentifier;
  m_haveNistMeasConfig = msg.haveNistMeasConfig;
  m_measConfig = msg.measConfig;
  m_haveNistMobilityControlInfo = msg.haveNistMobilityControlInfo;
  m_mobilityControlInfo = msg.mobilityControlInfo;
  m_haveNistRadioResourceConfigDedicated = msg.haveNistRadioResourceConfigDedicated;
  m_radioResourceConfigDedicated = msg.radioResourceConfigDedicated;

  m_isDataSerialized = false;
}

NistLteRrcSap::NistRrcConnectionReconfiguration
NistRrcConnectionReconfigurationHeader::GetMessage () const
{
  NistLteRrcSap::NistRrcConnectionReconfiguration msg;

  msg.rrcTransactionIdentifier = m_rrcTransactionIdentifier;
  msg.haveNistMeasConfig = m_haveNistMeasConfig;
  msg.measConfig = m_measConfig;
  msg.haveNistMobilityControlInfo = m_haveNistMobilityControlInfo;
  msg.mobilityControlInfo = m_mobilityControlInfo;
  msg.haveNistRadioResourceConfigDedicated = m_haveNistRadioResourceConfigDedicated;
  msg.radioResourceConfigDedicated = m_radioResourceConfigDedicated;

  return msg;
}

uint8_t
NistRrcConnectionReconfigurationHeader::GetRrcTransactionIdentifier () const
{
  return m_rrcTransactionIdentifier;
}

bool
NistRrcConnectionReconfigurationHeader::GetHaveNistMeasConfig ()
{
  return m_haveNistMeasConfig;
}

NistLteRrcSap::NistMeasConfig
NistRrcConnectionReconfigurationHeader::GetNistMeasConfig ()
{
  return m_measConfig;
}

bool
NistRrcConnectionReconfigurationHeader::GetHaveNistMobilityControlInfo ()
{
  return m_haveNistMobilityControlInfo;
}

NistLteRrcSap::NistMobilityControlInfo
NistRrcConnectionReconfigurationHeader::GetNistMobilityControlInfo ()
{
  return m_mobilityControlInfo;
}

bool
NistRrcConnectionReconfigurationHeader::GetHaveNistRadioResourceConfigDedicated ()
{
  return m_haveNistRadioResourceConfigDedicated;
}

NistLteRrcSap::NistRadioResourceConfigDedicated
NistRrcConnectionReconfigurationHeader::GetNistRadioResourceConfigDedicated ()
{
  return m_radioResourceConfigDedicated;
}

bool
NistRrcConnectionReconfigurationHeader::HaveNistPhysicalConfigDedicated () const
{
  return m_radioResourceConfigDedicated.haveNistPhysicalConfigDedicated;
}

std::list<NistLteRrcSap::NistSrbToAddMod> 
NistRrcConnectionReconfigurationHeader::GetNistSrbToAddModList () const
{
  return m_radioResourceConfigDedicated.srbToAddModList;
}

std::list<NistLteRrcSap::NistDrbToAddMod> 
NistRrcConnectionReconfigurationHeader::GetNistDrbToAddModList () const
{
  return m_radioResourceConfigDedicated.drbToAddModList;
}

std::list<uint8_t> 
NistRrcConnectionReconfigurationHeader::GetDrbToReleaseList () const
{
  return m_radioResourceConfigDedicated.drbToReleaseList;
}

NistLteRrcSap::NistPhysicalConfigDedicated 
NistRrcConnectionReconfigurationHeader::GetNistPhysicalConfigDedicated () const
{
  return m_radioResourceConfigDedicated.physicalConfigDedicated;
}


//////////////////// NistHandoverPreparationInfoHeader class ////////////////////////

NistHandoverPreparationInfoHeader::NistHandoverPreparationInfoHeader ()
{
}

void
NistHandoverPreparationInfoHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  // Serialize NistHandoverPreparationInformation sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize criticalExtensions choice
  // 2 options, selected 0 (c1)
  SerializeChoice (2,0,false);

  // Serialize c1 choice
  // 8 options, selected 0 (handoverPreparationInformation-r8)
  SerializeChoice (8,0,false);

  // Serialize NistHandoverPreparationInformation-r8-IEs sequence
  // 4 optional fields, no extension marker.
  std::bitset<4> handoverPrepInfoOpts;
  handoverPrepInfoOpts.set (3,1); // as-Config present
  handoverPrepInfoOpts.set (2,0); // rrm-Config not present
  handoverPrepInfoOpts.set (1,0); // as-Context not present
  handoverPrepInfoOpts.set (0,0); // nonCriticalExtension not present
  SerializeSequence (handoverPrepInfoOpts,false);

  // Serialize ue-RadioAccessCapabilityInfo
  SerializeSequenceOf (0,MAX_RAT_CAPABILITIES,0);

  // Serialize as-Config
  SerializeSequence (std::bitset<0> (),true);

  // Serialize sourceNistMeasConfig
  SerializeNistMeasConfig (m_asConfig.sourceNistMeasConfig);

  // Serialize sourceRadioResourceConfig
  SerializeNistRadioResourceConfigDedicated (m_asConfig.sourceRadioResourceConfig);

  // Serialize sourceSecurityAlgorithmConfig
  SerializeSequence (std::bitset<0> (),false);
  // cipheringAlgorithm
  SerializeEnum (8,0);
  // integrityProtAlgorithm
  SerializeEnum (8,0);

  // Serialize sourceUE-Identity
  SerializeBitstring (std::bitset<16> (m_asConfig.sourceUeIdentity));

  // Serialize sourceNistMasterInformationBlock
  SerializeSequence (std::bitset<0> (),false);
  SerializeEnum (6,m_asConfig.sourceNistMasterInformationBlock.dlBandwidth); // dl-Bandwidth
  SerializeSequence (std::bitset<0> (),false); // phich-Config sequence
  SerializeEnum (2,0); // phich-Duration
  SerializeEnum (4,0); // phich-Resource
  SerializeBitstring (std::bitset<8> (m_asConfig.sourceNistMasterInformationBlock.systemFrameNumber)); // systemFrameNumber
  SerializeBitstring (std::bitset<10> (321)); // spare

  // Serialize sourceNistSystemInformationBlockType1 sequence
  SerializeNistSystemInformationBlockType1 (m_asConfig.sourceNistSystemInformationBlockType1);

  // Serialize sourceNistSystemInformationBlockType2
  SerializeNistSystemInformationBlockType2 (m_asConfig.sourceNistSystemInformationBlockType2);

  // Serialize AntennaInfoCommon 
  SerializeSequence (std::bitset<0> (0),false);
  SerializeEnum (4,0); // antennaPortsCount 

  // Serialize sourceDlCarrierFreq
  SerializeInteger (m_asConfig.sourceDlCarrierFreq,0,MAX_EARFCN);

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistHandoverPreparationInfoHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  // Deserialize NistHandoverPreparationInformation sequence
  // 0 optional fields, no extension marker
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // Deserialize criticalExtensions choice
  int criticalExtensionsChosen;
  bIterator = DeserializeChoice (2,false,&criticalExtensionsChosen,bIterator);

  if (criticalExtensionsChosen == 1)
    {
      // Deserialize criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if (criticalExtensionsChosen == 0)
    {
      // Deserialize c1 choice
      int c1Chosen;
      bIterator = DeserializeChoice (8,false,&c1Chosen,bIterator);
      if (c1Chosen > 0)
        {
          bIterator = DeserializeNull (bIterator);
        }
      else if (c1Chosen == 0)
        {
          // Deserialize handoverPreparationInformation-r8
          std::bitset<4> handoverPrepInfoOpts;
          bIterator = DeserializeSequence (&handoverPrepInfoOpts,false,bIterator);

          // Deserialize ue-RadioAccessCapabilityInfo
          bIterator = DeserializeSequenceOf (&n,MAX_RAT_CAPABILITIES,0,bIterator);
          for (int i = 0; i < n; i++)
            {
              // Deserialize UE-CapabilityRAT-Container
              // ...
            }

          if (handoverPrepInfoOpts[3])
            {
              // Deserialize as-Config sequence
              bIterator = DeserializeSequence (&bitset0,true,bIterator);

              // Deserialize sourceNistMeasConfig
              bIterator = DeserializeNistMeasConfig (&m_asConfig.sourceNistMeasConfig, bIterator);

              // Deserialize sourceRadioResourceConfig
              bIterator = DeserializeNistRadioResourceConfigDedicated (&m_asConfig.sourceRadioResourceConfig,bIterator);

              // Deserialize sourceSecurityAlgorithmConfig
              bIterator = DeserializeSequence (&bitset0,false,bIterator);
              bIterator = DeserializeEnum (8,&n,bIterator); // cipheringAlgorithm
              bIterator = DeserializeEnum (8,&n,bIterator); // integrityProtAlgorithm

              // Deserialize sourceUE-Identity
              std::bitset<16> cRnti;
              bIterator = DeserializeBitstring (&cRnti,bIterator);
              m_asConfig.sourceUeIdentity = cRnti.to_ulong ();

              // Deserialize sourceNistMasterInformationBlock
              bIterator = DeserializeSequence (&bitset0,false,bIterator);
              bIterator = DeserializeEnum (6,&n,bIterator); // dl-Bandwidth
              m_asConfig.sourceNistMasterInformationBlock.dlBandwidth = n;

              // phich-Config
              bIterator = DeserializeSequence (&bitset0,false,bIterator);
              bIterator = DeserializeEnum (2,&n,bIterator); // phich-Duration
              bIterator = DeserializeEnum (4,&n,bIterator); // phich-Resource

              // systemFrameNumber
              std::bitset<8> systemFrameNumber;
              bIterator = DeserializeBitstring (&systemFrameNumber,bIterator);
              m_asConfig.sourceNistMasterInformationBlock.systemFrameNumber = systemFrameNumber.to_ulong ();
              // spare
              std::bitset<10> spare;
              bIterator = DeserializeBitstring (&spare,bIterator);

              // Deserialize sourceNistSystemInformationBlockType1
              bIterator = DeserializeNistSystemInformationBlockType1 (&m_asConfig.sourceNistSystemInformationBlockType1,bIterator);

              // Deserialize sourceNistSystemInformationBlockType2
              bIterator = DeserializeNistSystemInformationBlockType2 (&m_asConfig.sourceNistSystemInformationBlockType2,bIterator);

              // Deserialize antennaInfoCommon
              bIterator = DeserializeSequence (&bitset0,false,bIterator);
              bIterator = DeserializeEnum (4,&n,bIterator); // antennaPortsCount

              // Deserialize sourceDl-CarrierFreq
              bIterator = DeserializeInteger (&n,0,MAX_EARFCN,bIterator);
              m_asConfig.sourceDlCarrierFreq = n;
            }
          if (handoverPrepInfoOpts[2])
            {
              // Deserialize rrm-Config
              // ...
            }
          if (handoverPrepInfoOpts[1])
            {
              // Deserialize as-Context
              // ...
            }
          if (handoverPrepInfoOpts[0])
            {
              // Deserialize nonCriticalExtension
              // ...
            }
        }
    }

  return GetSerializedSize ();
}

void
NistHandoverPreparationInfoHeader::Print (std::ostream &os) const
{
  NistRrcAsn1Header::Print (os,m_asConfig.sourceRadioResourceConfig);
  os << "sourceUeIdentity: " << m_asConfig.sourceUeIdentity << std::endl;
  os << "dlBandwidth: " << (int)m_asConfig.sourceNistMasterInformationBlock.dlBandwidth << std::endl;
  os << "systemFrameNumber: " << (int)m_asConfig.sourceNistMasterInformationBlock.systemFrameNumber << std::endl;
  os << "plmnIdentityInfo.plmnIdentity: " << (int) m_asConfig.sourceNistSystemInformationBlockType1.cellAccessRelatedInfo.plmnIdentityInfo.plmnIdentity << std::endl;
  os << "cellAccessRelatedInfo.cellIdentity " << (int)m_asConfig.sourceNistSystemInformationBlockType1.cellAccessRelatedInfo.cellIdentity << std::endl;
  os << "cellAccessRelatedInfo.csgIndication: " << m_asConfig.sourceNistSystemInformationBlockType1.cellAccessRelatedInfo.csgIndication << std::endl;
  os << "cellAccessRelatedInfo.csgIdentity: " << (int)m_asConfig.sourceNistSystemInformationBlockType1.cellAccessRelatedInfo.csgIdentity << std::endl;
  os << "sourceDlCarrierFreq: " << m_asConfig.sourceDlCarrierFreq << std::endl;
}

void
NistHandoverPreparationInfoHeader::SetMessage (NistLteRrcSap::NistHandoverPreparationInfo msg)
{
  m_asConfig = msg.asConfig;
  m_isDataSerialized = false;
}

NistLteRrcSap::NistHandoverPreparationInfo
NistHandoverPreparationInfoHeader::GetMessage () const
{
  NistLteRrcSap::NistHandoverPreparationInfo msg;
  msg.asConfig = m_asConfig;

  return msg;
}

NistLteRrcSap::NistAsConfig
NistHandoverPreparationInfoHeader::GetNistAsConfig () const
{
  return m_asConfig;
}

//////////////////// NistRrcConnectionReestablishmentRequestHeader class ////////////////////////

NistRrcConnectionReestablishmentRequestHeader::NistRrcConnectionReestablishmentRequestHeader ()
{
}

NistRrcConnectionReestablishmentRequestHeader::~NistRrcConnectionReestablishmentRequestHeader ()
{
}

void
NistRrcConnectionReestablishmentRequestHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  SerializeUlCcchMessage (0);

  // Serialize NistRrcConnectionReestablishmentReques sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize criticalExtensions choice
  // chosen: rrcConnectionReestablishmentRequest-r8
  SerializeChoice (2,0,false);

  // Serialize RRCConnectionReestablishmentRequest-r8-IEs sequence
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize ue-Identity
  SerializeSequence (std::bitset<0> (),false);
  // Serialize c-RNTI
  SerializeBitstring (std::bitset<16> (m_ueIdentity.cRnti));
  // Serialize physCellId
  SerializeInteger (m_ueIdentity.physCellId,0,503);
  // Serialize shortMAC-I
  SerializeBitstring (std::bitset<16> (0));

  // Serialize ReestablishmentCause
  switch (m_reestablishmentCause)
    {
    case NistLteRrcSap::RECONFIGURATION_FAILURE:
      SerializeEnum (4,0);
      break;
    case NistLteRrcSap::HANDOVER_FAILURE:
      SerializeEnum (4,1);
      break;
    case NistLteRrcSap::OTHER_FAILURE:
      SerializeEnum (4,2);
      break;
    default:
      SerializeEnum (4,3);
    }

  // Serialize spare
  SerializeBitstring (std::bitset<2> (0));

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistRrcConnectionReestablishmentRequestHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeUlCcchMessage (bIterator);

  // Deserialize NistRrcConnectionReestablishmentRequest sequence
  // 0 optional fields, no extension marker
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // Deserialize criticalExtensions choice
  bIterator = DeserializeChoice (2,false,&n,bIterator);
  if ( n == 1)
    {
      // Deserialize criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if ( n == 0)
    {
      // Deserialize RRCConnectionReestablishmentRequest-r8-IEs
      bIterator = DeserializeSequence (&bitset0,false,bIterator);

      // Deserialize ReestabUE-Identity sequence
      bIterator = DeserializeSequence (&bitset0,false,bIterator);

      // Deserialize c-RNTI
      std::bitset<16> cRnti;
      bIterator = DeserializeBitstring (&cRnti,bIterator);
      m_ueIdentity.cRnti = cRnti.to_ulong ();

      // Deserialize physCellId
      int physCellId;
      bIterator = DeserializeInteger (&physCellId,0,503,bIterator);
      m_ueIdentity.physCellId = physCellId;

      // Deserialize shortMAC-I
      std::bitset<16> shortMacI;
      bIterator = DeserializeBitstring (&shortMacI,bIterator);

      // Deserialize ReestablishmentCause
      int reestCs;
      bIterator = DeserializeEnum (4,&reestCs,bIterator);
      switch (reestCs)
        {
        case 0:
          m_reestablishmentCause = NistLteRrcSap::RECONFIGURATION_FAILURE;
          break;
        case 1:
          m_reestablishmentCause = NistLteRrcSap::HANDOVER_FAILURE;
          break;
        case 2:
          m_reestablishmentCause = NistLteRrcSap::OTHER_FAILURE;
          break;
        case 3:
          break;
        }

      // Deserialize spare
      std::bitset<2> spare;
      bIterator = DeserializeBitstring (&spare,bIterator);
    }

  return GetSerializedSize ();
}

void
NistRrcConnectionReestablishmentRequestHeader::Print (std::ostream &os) const
{
  os << "ueIdentity.cRnti: " << (int)m_ueIdentity.cRnti << std::endl;
  os << "ueIdentity.physCellId: " << (int)m_ueIdentity.physCellId << std::endl;
  os << "m_reestablishmentCause: " << m_reestablishmentCause << std::endl;
}

void
NistRrcConnectionReestablishmentRequestHeader::SetMessage (NistLteRrcSap::NistRrcConnectionReestablishmentRequest msg)
{
  m_ueIdentity = msg.ueIdentity;
  m_reestablishmentCause = msg.reestablishmentCause;
  m_isDataSerialized = false;
}

NistLteRrcSap::NistRrcConnectionReestablishmentRequest
NistRrcConnectionReestablishmentRequestHeader::GetMessage () const
{
  NistLteRrcSap::NistRrcConnectionReestablishmentRequest msg;
  msg.ueIdentity = m_ueIdentity;
  msg.reestablishmentCause = m_reestablishmentCause;

  return msg;
}

NistLteRrcSap::NistReestabUeIdentity
NistRrcConnectionReestablishmentRequestHeader::GetUeIdentity () const
{
  return m_ueIdentity;
}

NistLteRrcSap::ReestablishmentCause
NistRrcConnectionReestablishmentRequestHeader::GetReestablishmentCause () const
{
  return m_reestablishmentCause;
}

//////////////////// NistRrcConnectionReestablishmentHeader class ////////////////////////

NistRrcConnectionReestablishmentHeader::NistRrcConnectionReestablishmentHeader ()
{
}

NistRrcConnectionReestablishmentHeader::~NistRrcConnectionReestablishmentHeader ()
{
}

void
NistRrcConnectionReestablishmentHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  SerializeDlCcchMessage (0);

  // Serialize NistRrcConnectionReestablishment sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize rrc-TransactionIdentifier
  SerializeInteger (m_rrcTransactionIdentifier,0,3);

  // Serialize criticalExtensions choice
  SerializeChoice (2,0,false);

  // Serialize c1 choice
  SerializeChoice (8,0,false);

  // Serialize RRCConnectionReestablishment-r8-IEs sequence
  // 1 optional field, no extension marker
  SerializeSequence (std::bitset<1> (0),false);

  // Serialize radioResourceConfigDedicated
  SerializeNistRadioResourceConfigDedicated (m_radioResourceConfigDedicated);

  // Serialize nextHopChainingCount
  SerializeInteger (0,0,7);

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistRrcConnectionReestablishmentHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeDlCcchMessage (bIterator);

  // Deserialize NistRrcConnectionReestablishment sequence
  // 0 optional fields, no extension marker
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // Deserialize rrc-TransactionIdentifier
  bIterator = DeserializeInteger (&n,0,3,bIterator);
  m_rrcTransactionIdentifier = n;

  // Deserialize criticalExtensions choice
  int criticalExtensionsChoice;
  bIterator = DeserializeChoice (2,false,&criticalExtensionsChoice,bIterator);
  if (criticalExtensionsChoice == 1)
    {
      // Deserialize criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if (criticalExtensionsChoice == 0)
    {
      // Deserialize c1
      int c1;
      bIterator = DeserializeChoice (8,false,&c1,bIterator);
      if (c1 > 0)
        {
          bIterator = DeserializeNull (bIterator);
        }
      else if (c1 == 0)
        {
          // Deserialize rrcConnectionReestablishment-r8
          // 1 optional field
          std::bitset<1> nonCriticalExtensionPresent;
          bIterator = DeserializeSequence (&nonCriticalExtensionPresent,false,bIterator);

          // Deserialize NistRadioResourceConfigDedicated
          bIterator = DeserializeNistRadioResourceConfigDedicated (&m_radioResourceConfigDedicated,bIterator);

          // Deserialize nextHopChainingCount
          bIterator = DeserializeInteger (&n,0,7,bIterator);
        }
    }

  return GetSerializedSize ();
}

void
NistRrcConnectionReestablishmentHeader::Print (std::ostream &os) const
{
  os << "rrcTransactionIdentifier: " << (int)m_rrcTransactionIdentifier << std::endl;
  os << "NistRadioResourceConfigDedicated: " << std::endl;
  NistRrcAsn1Header::Print (os,m_radioResourceConfigDedicated);
}

void
NistRrcConnectionReestablishmentHeader::SetMessage (NistLteRrcSap::NistRrcConnectionReestablishment msg)
{
  m_rrcTransactionIdentifier = msg.rrcTransactionIdentifier;
  m_radioResourceConfigDedicated = msg.radioResourceConfigDedicated;
  m_isDataSerialized = false;
}

NistLteRrcSap::NistRrcConnectionReestablishment
NistRrcConnectionReestablishmentHeader::GetMessage () const
{
  NistLteRrcSap::NistRrcConnectionReestablishment msg;
  msg.rrcTransactionIdentifier = m_rrcTransactionIdentifier;
  msg.radioResourceConfigDedicated = m_radioResourceConfigDedicated;
  return msg;
}

uint8_t
NistRrcConnectionReestablishmentHeader::GetRrcTransactionIdentifier () const
{
  return m_rrcTransactionIdentifier;
}

NistLteRrcSap::NistRadioResourceConfigDedicated
NistRrcConnectionReestablishmentHeader::GetNistRadioResourceConfigDedicated () const
{
  return m_radioResourceConfigDedicated;
}

//////////////////// NistRrcConnectionReestablishmentCompleteHeader class ////////////////////////

NistRrcConnectionReestablishmentCompleteHeader::NistRrcConnectionReestablishmentCompleteHeader ()
{
}

void
NistRrcConnectionReestablishmentCompleteHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  // Serialize DCCH message
  SerializeUlDcchMessage (3);

  // Serialize NistRrcConnectionReestablishmentComplete sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize rrc-TransactionIdentifier
  SerializeInteger (m_rrcTransactionIdentifier,0,3);

  // Serialize criticalExtensions choice
  SerializeChoice (2,0,false);

  // Serialize rrcConnectionReestablishmentComplete-r8 sequence
  // 1 optional field (not present), no extension marker.
  SerializeSequence (std::bitset<1> (0),false);

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistRrcConnectionReestablishmentCompleteHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeUlDcchMessage (bIterator);

  // Deserialize NistRrcConnectionReestablishmentComplete sequence
  // 0 optional fields, no extension marker
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // Deserialize rrc-TransactionIdentifier
  bIterator = DeserializeInteger (&n,0,3,bIterator);
  m_rrcTransactionIdentifier = n;

  // Deserialize criticalExtensions choice
  int criticalExtensionsChoice;
  bIterator = DeserializeChoice (2,false,&criticalExtensionsChoice,bIterator);
  if (criticalExtensionsChoice == 1)
    {
      // Deserialize criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if (criticalExtensionsChoice == 0)
    {
      // Deserialize rrcConnectionReestablishmentComplete-r8
      std::bitset<1> opts;
      bIterator = DeserializeSequence (&opts,false,bIterator);
      if (opts[0])
        {
          // Deserialize RRCConnectionReestablishmentComplete-v920-IEs
          // ...
        }
    }

  return GetSerializedSize ();
}

void
NistRrcConnectionReestablishmentCompleteHeader::Print (std::ostream &os) const
{
  os << "rrcTransactionIdentifier: " << (int)m_rrcTransactionIdentifier << std::endl;
}

void
NistRrcConnectionReestablishmentCompleteHeader::SetMessage (NistLteRrcSap::NistRrcConnectionReestablishmentComplete msg)
{
  m_rrcTransactionIdentifier = msg.rrcTransactionIdentifier;
  m_isDataSerialized = false;
}

NistLteRrcSap::NistRrcConnectionReestablishmentComplete
NistRrcConnectionReestablishmentCompleteHeader::GetMessage () const
{
  NistLteRrcSap::NistRrcConnectionReestablishmentComplete msg;
  msg.rrcTransactionIdentifier = m_rrcTransactionIdentifier;
  return msg;
}

uint8_t
NistRrcConnectionReestablishmentCompleteHeader::GetRrcTransactionIdentifier () const
{
  return m_rrcTransactionIdentifier;
}

//////////////////// NistRrcConnectionReestablishmentRejectHeader class ////////////////////////

NistRrcConnectionReestablishmentRejectHeader::NistRrcConnectionReestablishmentRejectHeader ()
{
}

NistRrcConnectionReestablishmentRejectHeader::~NistRrcConnectionReestablishmentRejectHeader ()
{
}

void
NistRrcConnectionReestablishmentRejectHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  // Serialize CCCH message
  SerializeDlCcchMessage (1);

  // Serialize NistRrcConnectionReestablishmentReject sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize criticalExtensions choice
  SerializeChoice (2,0,false);

  // Serialize RRCConnectionReestablishmentReject-r8-IEs sequence
  // 1 optional field (not present), no extension marker.
  SerializeSequence (std::bitset<1> (0),false);

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistRrcConnectionReestablishmentRejectHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;

  bIterator = DeserializeDlCcchMessage (bIterator);

  // Deserialize NistRrcConnectionReestablishmentReject sequence
  // 0 optional fields, no extension marker
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // Deserialize criticalExtensions choice
  int criticalExtensionsChoice;
  bIterator = DeserializeChoice (2,false,&criticalExtensionsChoice,bIterator);
  if (criticalExtensionsChoice == 1)
    {
      // Deserialize criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if (criticalExtensionsChoice == 0)
    {
      // Deserialize rrcConnectionReestablishmentReject-r8
      std::bitset<1> opts;
      bIterator = DeserializeSequence (&opts,false,bIterator);
      if (opts[0])
        {
          // Deserialize RRCConnectionReestablishmentReject-v8a0-IEs
          // ...
        }
    }

  return GetSerializedSize ();
}

void
NistRrcConnectionReestablishmentRejectHeader::Print (std::ostream &os) const
{
}

void
NistRrcConnectionReestablishmentRejectHeader::SetMessage (NistLteRrcSap::NistRrcConnectionReestablishmentReject msg)
{
  m_rrcConnectionReestablishmentReject = msg;
  m_isDataSerialized = false;
}

NistLteRrcSap::NistRrcConnectionReestablishmentReject
NistRrcConnectionReestablishmentRejectHeader::GetMessage () const
{
  return m_rrcConnectionReestablishmentReject;
}

//////////////////// NistRrcConnectionReleaseHeader class ////////////////////////

NistRrcConnectionReleaseHeader::NistRrcConnectionReleaseHeader ()
{
}

NistRrcConnectionReleaseHeader::~NistRrcConnectionReleaseHeader ()
{
}

void
NistRrcConnectionReleaseHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  // Serialize DCCH message
  SerializeDlDcchMessage (5);

  // Serialize NistRrcConnectionRelease sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize rrc-TransactionIdentifier
  SerializeInteger (m_rrcConnectionRelease.rrcTransactionIdentifier,0,3);

  // Serialize criticalExtensions choice
  SerializeChoice (2,0,false);

  // Serialize c1 choice
  SerializeChoice (4,0,false);

  // Serialize RRCConnectionRelease-r8-IEs sequence
  // 3 optional field (not present), no extension marker.
  SerializeSequence (std::bitset<3> (0),false);

  // Serialize ReleaseCause
  SerializeEnum (4,1);

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistRrcConnectionReleaseHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeDlDcchMessage (bIterator);

  // Deserialize NistRrcConnectionRelease sequence
  // 0 optional fields, no extension marker
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // Deserialize rrc-TransactionIdentifier
  bIterator = DeserializeInteger (&n,0,3,bIterator);
  m_rrcConnectionRelease.rrcTransactionIdentifier = n;

  // Deserialize criticalExtensions choice
  int criticalExtensionsChoice;
  bIterator = DeserializeChoice (2,false,&criticalExtensionsChoice,bIterator);
  if (criticalExtensionsChoice == 1)
    {
      // Deserialize criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if (criticalExtensionsChoice == 0)
    {
      // Deserialize c1
      int c1Choice;
      bIterator = DeserializeChoice (4,false,&c1Choice,bIterator);

      if (c1Choice == 0)
        {
          // Deserialize RRCConnectionRelease-r8-IEs
          std::bitset<3> opts;
          bIterator = DeserializeSequence (&opts,false,bIterator);

          // Deserialize releaseCause
          bIterator = DeserializeEnum (4,&n,bIterator);

          if (opts[2])
            {
              // Deserialize redirectedCarrierInfo
              // ...
            }
          if (opts[1])
            {
              // Deserialize idleModeNistMobilityControlInfo
              // ...
            }
          if (opts[0])
            {
              // Deserialize nonCriticalExtension
              // ...
            }
        }

      else
        {
          bIterator = DeserializeNull (bIterator);
        }
    }

  return GetSerializedSize ();
}

void
NistRrcConnectionReleaseHeader::Print (std::ostream &os) const
{
}

void
NistRrcConnectionReleaseHeader::SetMessage (NistLteRrcSap::NistRrcConnectionRelease msg)
{
  m_rrcConnectionRelease = msg;
  m_isDataSerialized = false;
}

NistLteRrcSap::NistRrcConnectionRelease
NistRrcConnectionReleaseHeader::GetMessage () const
{
  return m_rrcConnectionRelease;
}

//////////////////// NistRrcConnectionRejectHeader class ////////////////////////

NistRrcConnectionRejectHeader::NistRrcConnectionRejectHeader ()
{
}

NistRrcConnectionRejectHeader::~NistRrcConnectionRejectHeader ()
{
}

void
NistRrcConnectionRejectHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  // Serialize CCCH message
  SerializeDlCcchMessage (2);

  // Serialize NistRrcConnectionReject sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize criticalExtensions choice
  SerializeChoice (2,0,false);

  // Serialize c1 choice
  SerializeChoice (4,0,false);

  // Serialize rrcConnectionReject-r8 sequence
  // 1 optional field (not present), no extension marker.
  SerializeSequence (std::bitset<1> (0),false);

  // Serialize waitTime
  SerializeInteger (m_rrcConnectionReject.waitTime, 1, 16);

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistRrcConnectionRejectHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeDlCcchMessage (bIterator);

  // Deserialize NistRrcConnectionReject sequence
  // 0 optional fields, no extension marker
  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  // Deserialize criticalExtensions choice
  int criticalExtensionsChoice;
  bIterator = DeserializeChoice (2,false,&criticalExtensionsChoice,bIterator);
  if (criticalExtensionsChoice == 1)
    {
      // Deserialize criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if (criticalExtensionsChoice == 0)
    {
      // Deserialize c1 choice
      int c1Choice;
      bIterator = DeserializeChoice (4,false,&c1Choice,bIterator);

      if (c1Choice > 0)
        {
          bIterator = DeserializeNull (bIterator);
        }
      else if (c1Choice == 0)
        {
          // Deserialize rrcConnectionReject-r8
          std::bitset<1> opts;
          bIterator = DeserializeSequence (&opts,false,bIterator);

          bIterator = DeserializeInteger (&n,1,16,bIterator);
          m_rrcConnectionReject.waitTime = n;

          if (opts[0])
            {
              // Deserialize RRCConnectionReject-v8a0-IEs
              // ...
            }
        }
    }

  return GetSerializedSize ();
}

void
NistRrcConnectionRejectHeader::Print (std::ostream &os) const
{
  os << "wait time: " << (int)m_rrcConnectionReject.waitTime << std::endl;
}

void
NistRrcConnectionRejectHeader::SetMessage (NistLteRrcSap::NistRrcConnectionReject msg)
{
  m_rrcConnectionReject = msg;
  m_isDataSerialized = false;
}

NistLteRrcSap::NistRrcConnectionReject
NistRrcConnectionRejectHeader::GetMessage () const
{
  return m_rrcConnectionReject;
}

//////////////////// NistMeasurementReportHeader class ////////////////////////

NistMeasurementReportHeader::NistMeasurementReportHeader ()
{
}

NistMeasurementReportHeader::~NistMeasurementReportHeader ()
{
}

void
NistMeasurementReportHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  // Serialize DCCH message
  SerializeUlDcchMessage (1);

  // Serialize NistMeasurementReport sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);

  // Serialize criticalExtensions choice:
  // c1 chosen
  SerializeChoice (2,0,false);

  // Serialize c1 choice
  // measurementReport-r8 chosen
  SerializeChoice (8,0,false);

  // Serialize NistMeasurementReport-r8-IEs sequence:
  // 1 optional fields, not present. Extension marker not present.
  SerializeSequence (std::bitset<1> (0),false);

  // Serialize measResults
  SerializeNistMeasResults (m_measurementReport.measResults);

  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
NistMeasurementReportHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;

  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  bIterator = DeserializeUlDcchMessage (bIterator);

  int criticalExtensionsChoice;
  bIterator = DeserializeChoice (2,false,&criticalExtensionsChoice,bIterator);

  if (criticalExtensionsChoice == 1)
    {
      // Deserialize criticalExtensionsFuture
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
    }
  else if (criticalExtensionsChoice == 0)
    {
      // Deserialize c1
      int c1Choice;
      bIterator = DeserializeChoice (8,false,&c1Choice,bIterator);

      if (c1Choice > 0)
        {
          bIterator = DeserializeNull (bIterator);
        }
      else
        {
          // Deserialize measurementReport-r8
          std::bitset<1> isNonCriticalExtensionPresent;
          bIterator = DeserializeSequence (&isNonCriticalExtensionPresent,false,bIterator);

          // Deserialize measResults
          bIterator = DeserializeNistMeasResults (&m_measurementReport.measResults, bIterator);

          if (isNonCriticalExtensionPresent[0])
            {
              // Deserialize nonCriticalExtension NistMeasurementReport-v8a0-IEs
              // ...
            }

        }
    }

  return GetSerializedSize ();
}

void
NistMeasurementReportHeader::Print (std::ostream &os) const
{
  os << "measId = " << (int)m_measurementReport.measResults.measId << std::endl;
  os << "rsrpResult = " << (int)m_measurementReport.measResults.rsrpResult << std::endl;
  os << "rsrqResult = " << (int)m_measurementReport.measResults.rsrqResult << std::endl;
  os << "haveMeasResultNeighCells = " << (int)m_measurementReport.measResults.haveMeasResultNeighCells << std::endl;

  if (m_measurementReport.measResults.haveMeasResultNeighCells)
    {
      std::list<NistLteRrcSap::NistMeasResultEutra> measResultListEutra = m_measurementReport.measResults.measResultListEutra;
      std::list<NistLteRrcSap::NistMeasResultEutra>::iterator it = measResultListEutra.begin ();
      for (; it != measResultListEutra.end (); it++)
        {
          os << "   physCellId =" << (int) it->physCellId << std::endl;
          os << "   haveNistCgiInfo =" << it->haveNistCgiInfo << std::endl;
          if (it->haveNistCgiInfo)
            {
              os << "      plmnIdentity = " << (int) it->cgiInfo.plmnIdentity << std::endl;
              os << "      cellIdentity = " << (int) it->cgiInfo.cellIdentity << std::endl;
              os << "      trackingAreaCode = " << (int) it->cgiInfo.trackingAreaCode << std::endl;
              os << "      havePlmnIdentityList = " << !it->cgiInfo.plmnIdentityList.empty () << std::endl;
              if (!it->cgiInfo.plmnIdentityList.empty ())
                {
                  for (std::list<uint32_t>::iterator it2 = it->cgiInfo.plmnIdentityList.begin (); it2 != it->cgiInfo.plmnIdentityList.begin (); it2++)
                    {
                      os << "         plmnId : " << *it2 << std::endl;
                    }
                }
            }

          os << "   haveRsrpResult =" << it->haveRsrpResult << std::endl;
          if (it->haveRsrpResult)
            {
              os << "   rsrpResult =" << (int) it->rsrpResult << std::endl;
            }

          os << "   haveRsrqResult =" << it->haveRsrqResult << std::endl;
          if (it->haveRsrqResult)
            {
              os << "   rsrqResult =" << (int) it->rsrqResult << std::endl;
            }

        }
    }
}

void
NistMeasurementReportHeader::SetMessage (NistLteRrcSap::NistMeasurementReport msg)
{
  m_measurementReport = msg;
  m_isDataSerialized = false;
}

NistLteRrcSap::NistMeasurementReport
NistMeasurementReportHeader::GetMessage () const
{
  NistLteRrcSap::NistMeasurementReport msg;
  msg = m_measurementReport;
  return msg;
}

//////////////////// NistSidelinkInformationHeader class ////////////////////////


SidelinkUeInformationHeader::SidelinkUeInformationHeader ()
{
}

SidelinkUeInformationHeader::~SidelinkUeInformationHeader ()
{
}

void
SidelinkUeInformationHeader::PreSerialize () const
{
  m_serializationResult = Buffer ();

  // Serialize DCCH message
  SerializeUlDcchMessage (20);

  // Serialize NistMeasurementReport sequence:
  // no default or optional fields. Extension marker not present.
  SerializeSequence (std::bitset<0> (),false);



  // Finish serialization
  FinalizeSerialization ();
}

uint32_t
SidelinkUeInformationHeader::Deserialize (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;

  bIterator = DeserializeSequence (&bitset0,false,bIterator);

  bIterator = DeserializeUlDcchMessage (bIterator);



  return GetSerializedSize ();
}

void
SidelinkUeInformationHeader::Print (std::ostream &os) const
{
  //os << "measId = " << (int)m_measurementReport.measResults.measId << std::endl;

}

void
SidelinkUeInformationHeader::SetMessage (NistLteRrcSap::SidelinkUeInformation msg)
{
  m_sidelinkUeInformation = msg;
  m_isDataSerialized = false;
}

NistLteRrcSap::SidelinkUeInformation
SidelinkUeInformationHeader::GetMessage () const
{
  NistLteRrcSap::SidelinkUeInformation msg;
  msg = m_sidelinkUeInformation;
  return msg;
}


  
///////////////////  NistRrcUlDcchMessage //////////////////////////////////
NistRrcUlDcchMessage::NistRrcUlDcchMessage () : NistRrcAsn1Header ()
{
}

NistRrcUlDcchMessage::~NistRrcUlDcchMessage ()
{
}

uint32_t
NistRrcUlDcchMessage::Deserialize (Buffer::Iterator bIterator)
{
  DeserializeUlDcchMessage (bIterator);
  return 1;
}

void
NistRrcUlDcchMessage::Print (std::ostream &os) const
{
  std::cout << "UL DCCH MSG TYPE: " << m_messageType << std::endl;
}

void
NistRrcUlDcchMessage::PreSerialize () const
{
  SerializeUlDcchMessage (m_messageType);
}

Buffer::Iterator
NistRrcUlDcchMessage::DeserializeUlDcchMessage (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeSequence (&bitset0,false,bIterator);
  bIterator = DeserializeChoice (2,false,&n,bIterator);
  if (n == 1)
    {
      // Deserialize messageClassExtension
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
      m_messageType = -1;
    }
  else if (n == 0)
    {
      // Deserialize c1
      bIterator = DeserializeChoice (16,false,&m_messageType,bIterator);
    }

  return bIterator;
}

void
NistRrcUlDcchMessage::SerializeUlDcchMessage (int messageType) const
{
  SerializeSequence (std::bitset<0> (),false);
  // Choose c1
  SerializeChoice (2,0,false);
  // Choose message type
  SerializeChoice (16,messageType,false);
}

///////////////////  NistRrcDlDcchMessage //////////////////////////////////
NistRrcDlDcchMessage::NistRrcDlDcchMessage () : NistRrcAsn1Header ()
{
}

NistRrcDlDcchMessage::~NistRrcDlDcchMessage ()
{
}

uint32_t
NistRrcDlDcchMessage::Deserialize (Buffer::Iterator bIterator)
{
  DeserializeDlDcchMessage (bIterator);
  return 1;
}

void
NistRrcDlDcchMessage::Print (std::ostream &os) const
{
  std::cout << "DL DCCH MSG TYPE: " << m_messageType << std::endl;
}

void
NistRrcDlDcchMessage::PreSerialize () const
{
  SerializeDlDcchMessage (m_messageType);
}

Buffer::Iterator
NistRrcDlDcchMessage::DeserializeDlDcchMessage (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeSequence (&bitset0,false,bIterator);
  bIterator = DeserializeChoice (2,false,&n,bIterator);
  if (n == 1)
    {
      // Deserialize messageClassExtension
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
      m_messageType = -1;
    }
  else if (n == 0)
    {
      // Deserialize c1
      bIterator = DeserializeChoice (16,false,&m_messageType,bIterator);
    }

  return bIterator;
}

void
NistRrcDlDcchMessage::SerializeDlDcchMessage (int messageType) const
{
  SerializeSequence (std::bitset<0> (),false);
  // Choose c1
  SerializeChoice (2,0,false);
  // Choose message type
  SerializeChoice (16,messageType,false);
}

///////////////////  NistRrcUlCcchMessage //////////////////////////////////
NistRrcUlCcchMessage::NistRrcUlCcchMessage () : NistRrcAsn1Header ()
{
}

NistRrcUlCcchMessage::~NistRrcUlCcchMessage ()
{
}

uint32_t
NistRrcUlCcchMessage::Deserialize (Buffer::Iterator bIterator)
{
  DeserializeUlCcchMessage (bIterator);
  return 1;
}

void
NistRrcUlCcchMessage::Print (std::ostream &os) const
{
  std::cout << "UL CCCH MSG TYPE: " << m_messageType << std::endl;
}

void
NistRrcUlCcchMessage::PreSerialize () const
{
  SerializeUlCcchMessage (m_messageType);
}

Buffer::Iterator
NistRrcUlCcchMessage::DeserializeUlCcchMessage (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeSequence (&bitset0,false,bIterator);
  bIterator = DeserializeChoice (2,false,&n,bIterator);
  if (n == 1)
    {
      // Deserialize messageClassExtension
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
      m_messageType = -1;
    }
  else if (n == 0)
    {
      // Deserialize c1
      bIterator = DeserializeChoice (2,false,&m_messageType,bIterator);
    }

  return bIterator;
}

void
NistRrcUlCcchMessage::SerializeUlCcchMessage (int messageType) const
{
  SerializeSequence (std::bitset<0> (),false);
  // Choose c1
  SerializeChoice (2,0,false);
  // Choose message type
  SerializeChoice (2,messageType,false);
}

///////////////////  NistRrcDlCcchMessage //////////////////////////////////
NistRrcDlCcchMessage::NistRrcDlCcchMessage () : NistRrcAsn1Header ()
{
}

NistRrcDlCcchMessage::~NistRrcDlCcchMessage ()
{
}

uint32_t
NistRrcDlCcchMessage::Deserialize (Buffer::Iterator bIterator)
{
  DeserializeDlCcchMessage (bIterator);
  return 1;
}

void
NistRrcDlCcchMessage::Print (std::ostream &os) const
{
  std::cout << "DL CCCH MSG TYPE: " << m_messageType << std::endl;
}

void
NistRrcDlCcchMessage::PreSerialize () const
{
  SerializeDlCcchMessage (m_messageType);
}

Buffer::Iterator
NistRrcDlCcchMessage::DeserializeDlCcchMessage (Buffer::Iterator bIterator)
{
  std::bitset<0> bitset0;
  int n;

  bIterator = DeserializeSequence (&bitset0,false,bIterator);
  bIterator = DeserializeChoice (2,false,&n,bIterator);
  if (n == 1)
    {
      // Deserialize messageClassExtension
      bIterator = DeserializeSequence (&bitset0,false,bIterator);
      m_messageType = -1;
    }
  else if (n == 0)
    {
      // Deserialize c1
      bIterator = DeserializeChoice (4,false,&m_messageType,bIterator);
    }

  return bIterator;
}

void
NistRrcDlCcchMessage::SerializeDlCcchMessage (int messageType) const
{
  SerializeSequence (std::bitset<0> (),false);
  // Choose c1
  SerializeChoice (2,0,false);
  // Choose message type
  SerializeChoice (4,messageType,false);
}

} // namespace ns3

