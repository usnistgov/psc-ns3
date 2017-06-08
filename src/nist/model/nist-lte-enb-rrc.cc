/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 * Authors: Nicola Baldo <nbaldo@cttc.es>
 *          Marco Miozzo <mmiozzo@cttc.es>
 *          Manuel Requena <manuel.requena@cttc.es>
 * Modified by: NIST
 */

#include "nist-lte-enb-rrc.h"

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/abort.h>

#include <ns3/pointer.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/simulator.h>

#include <ns3/nist-lte-radio-bearer-info.h>
#include <ns3/nist-eps-bearer-tag.h>
#include <ns3/packet.h>

#include <ns3/nist-lte-rlc.h>
#include <ns3/nist-lte-rlc-tm.h>
#include <ns3/nist-lte-rlc-um.h>
#include <ns3/nist-lte-rlc-am.h>
#include <ns3/nist-lte-pdcp.h>




namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteEnbRrc");

///////////////////////////////////////////
// CMAC SAP forwarder
///////////////////////////////////////////

/**
 * \brief Class for forwarding CMAC SAP User functions.
 */
class NistEnbRrcMemberLteEnbCmacSapUser : public NistLteEnbCmacSapUser
{
public:
  NistEnbRrcMemberLteEnbCmacSapUser (NistLteEnbRrc* rrc);

  virtual uint16_t AllocateTemporaryCellRnti ();
  virtual void NotifyLcConfigResult (uint16_t rnti, uint8_t lcid, bool success);
  virtual void RrcConfigurationUpdateInd (NistUeConfig params);

private:
  NistLteEnbRrc* m_rrc;
};

NistEnbRrcMemberLteEnbCmacSapUser::NistEnbRrcMemberLteEnbCmacSapUser (NistLteEnbRrc* rrc)
  : m_rrc (rrc)
{
}

uint16_t
NistEnbRrcMemberLteEnbCmacSapUser::AllocateTemporaryCellRnti ()
{
  return m_rrc->DoAllocateTemporaryCellRnti ();
}

void
NistEnbRrcMemberLteEnbCmacSapUser::NotifyLcConfigResult (uint16_t rnti, uint8_t lcid, bool success)
{
  m_rrc->DoNotifyLcConfigResult (rnti, lcid, success);
}

void
NistEnbRrcMemberLteEnbCmacSapUser::RrcConfigurationUpdateInd (NistUeConfig params)
{
  m_rrc->DoRrcConfigurationUpdateInd (params);
}

///////////////////////////////////////////
// LteEnbRrcSl
///////////////////////////////////////////
  
NS_OBJECT_ENSURE_REGISTERED (LteEnbRrcSl);

LteEnbRrcSl::LteEnbRrcSl ()
  : m_slEnabled (false)
{
  m_preconfigCommTxPoolExceptional.nbPools = 0;
}

void
LteEnbRrcSl::DoInitialize ()
{


}

LteEnbRrcSl::~LteEnbRrcSl (void)
{
}
  
void
LteEnbRrcSl::DoDispose ()
{
  
    
}

TypeId LteEnbRrcSl::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::LteEnbRrcSl")
    .SetParent<Object> ()
    .AddConstructor<LteEnbRrcSl> ()
    
    ;
  return tid;
}

/**
 * \brief makes a copy of the sidelink configuration
 * \return a copy of the sidelink configuration
 */
Ptr<LteEnbRrcSl>
LteEnbRrcSl::Copy ()
{
  NS_LOG_FUNCTION (this);
  Ptr<LteEnbRrcSl> copy = CreateObject<LteEnbRrcSl> ();
  //copy internal values
  copy->m_slEnabled = m_slEnabled;
  copy->m_sib18 = m_sib18;
  //copy->m_dedicatedPoolMap = m_dedicatedPoolMap;
  copy->m_preconfigDedicatedPoolMap = m_preconfigDedicatedPoolMap;
  copy->m_preconfigCommTxPoolExceptional = m_preconfigCommTxPoolExceptional;
  
  return copy;
}

  
// void
// LteEnbRrcSl::SetSystemInformationBlockType18 (NistLteRrcSap::SystemInformationBlockType18 sib18)
// {
//   NS_LOG_FUNCTION (this);
//   m_sib18 = sib18;
// }
  
/**
 * Gets the SIB 18 information
 * \return The SIB 18
 */
NistLteRrcSap::SystemInformationBlockType18
LteEnbRrcSl::GetSystemInformationType18 ()
{
  NS_LOG_FUNCTION (this);

  //build the content of SIB 18 by looking at pools allocated
  NistLteRrcSap::SystemInformationBlockType18 sib18;
  sib18.commConfig.commRxPool.nbPools = 0;
  sib18.commConfig.commTxPoolNormalCommon.nbPools = 0;
  sib18.commConfig.commTxPoolExceptional.nbPools = 0;
  
  std::map <uint32_t, LteEnbRrcSl::ActivePoolInfo>::iterator it;
  for (it = m_activePoolMap.begin() ; it != m_activePoolMap.end() ; it++) {
    //check if pool already listed
    if (it->second.m_poolSetup.setup == NistLteRrcSap::SlCommTxResourcesSetup::SCHEDULED) {
      //check single pool
      if (!IsPoolInList(it->second.m_poolSetup.scheduled.commTxConfig, sib18.commConfig.commRxPool.pools, sib18.commConfig.commRxPool.nbPools)) {
        //add pool
        sib18.commConfig.commRxPool.pools[sib18.commConfig.commRxPool.nbPools]=it->second.m_poolSetup.scheduled.commTxConfig;
        sib18.commConfig.commRxPool.nbPools++;
      }
    } else if (it->second.m_poolSetup.ueSelected.havePoolToAdd) {
      //check all pools
      for (int i = 0 ; i < it->second.m_poolSetup.ueSelected.poolToAddModList.nbPools ; i++) {
        if (!IsPoolInList(it->second.m_poolSetup.ueSelected.poolToAddModList.pools[i].pool, sib18.commConfig.commRxPool.pools, sib18.commConfig.commRxPool.nbPools)) {
          //add pool
        sib18.commConfig.commRxPool.pools[sib18.commConfig.commRxPool.nbPools]=it->second.m_poolSetup.ueSelected.poolToAddModList.pools[i].pool;
        sib18.commConfig.commRxPool.nbPools++;
        }
      }
    }
  }
  
  sib18.commConfig.commTxPoolExceptional = m_preconfigCommTxPoolExceptional;
  
  return sib18;
}

bool
LteEnbRrcSl::IsPoolInList (NistLteRrcSap::SlCommResourcePool pool, NistLteRrcSap::SlCommResourcePool *pools, int nbPool)
{
  bool found = false;
  for (int i = 0 ; i < nbPool && !found; i++) {
    found =  pool == pools[i];
  }
  return found;
}
  

void
LteEnbRrcSl::SetSlEnabled (bool status)
{
  NS_LOG_FUNCTION (this);
  m_slEnabled = status;
}
    
bool
LteEnbRrcSl::IsSlEnabled ()
{
  NS_LOG_FUNCTION (this);
  return m_slEnabled;
}

void
LteEnbRrcSl::AddPreconfiguredDedicatedPool (uint32_t group, NistLteRrcSap::SlCommTxResourcesSetup pool)
{
  NS_LOG_FUNCTION (this);
  m_preconfigDedicatedPoolMap.insert (std::pair<uint32_t, NistLteRrcSap::SlCommTxResourcesSetup> (group, pool));
}

void
LteEnbRrcSl::SetCommTxPoolExceptional (NistLteRrcSap::SlCommTxPoolList pool)
{
  NS_LOG_FUNCTION (this);
  m_preconfigCommTxPoolExceptional = pool;
}


  
///////////////////////////////////////////
// NistUeManager
///////////////////////////////////////////


/// Map each of UE Manager states to its string representation.
static const std::string g_ueManagerStateName[NistUeManager::NUM_STATES] =
{
  "INITIAL_RANDOM_ACCESS",
  "CONNECTION_SETUP",
  "CONNECTION_REJECTED",
  "CONNECTED_NORMALLY",
  "CONNECTION_RECONFIGURATION",
  "CONNECTION_REESTABLISHMENT",
  "HANDOVER_PREPARATION",
  "HANDOVER_JOINING",
  "HANDOVER_PATH_SWITCH",
  "HANDOVER_LEAVING",
};

/**
 * \param s The UE manager state.
 * \return The string representation of the given state.
 */
static const std::string & ToString (NistUeManager::State s)
{
  return g_ueManagerStateName[s];
}


NS_OBJECT_ENSURE_REGISTERED (NistUeManager);


NistUeManager::NistUeManager ()
{
  NS_FATAL_ERROR ("this constructor is not espected to be used");
}


NistUeManager::NistUeManager (Ptr<NistLteEnbRrc> rrc, uint16_t rnti, State s)
  : m_lastAllocatedDrbid (0),
    m_rnti (rnti),
    m_imsi (0),
    m_lastRrcTransactionIdentifier (0),
    m_rrc (rrc),
    m_state (s),
    m_pendingNistRrcConnectionReconfiguration (false),
    m_sourceX2apId (0),
    m_sourceCellId (0),
    m_needPhyMacConfiguration (false),
    m_slPoolChanged (false)
{ 
  NS_LOG_FUNCTION (this);
}

void
NistUeManager::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  m_drbPdcpSapUser = new NistLtePdcpSpecificLtePdcpSapUser<NistUeManager> (this);

  m_physicalConfigDedicated.haveNistAntennaInfoDedicated = true;
  m_physicalConfigDedicated.antennaInfo.transmissionMode = m_rrc->m_defaultTransmissionMode;
  m_physicalConfigDedicated.haveNistSoundingRsUlConfigDedicated = true;
  m_physicalConfigDedicated.soundingRsUlConfigDedicated.srsConfigIndex = m_rrc->GetNewSrsConfigurationIndex ();
  m_physicalConfigDedicated.soundingRsUlConfigDedicated.type = NistLteRrcSap::NistSoundingRsUlConfigDedicated::SETUP;
  m_physicalConfigDedicated.soundingRsUlConfigDedicated.srsBandwidth = 0;
  m_physicalConfigDedicated.haveNistPdschConfigDedicated = true;
  m_physicalConfigDedicated.pdschConfigDedicated.pa = NistLteRrcSap::NistPdschConfigDedicated::dB0;

  m_rrc->m_cmacSapProvider->AddUe (m_rnti);
  m_rrc->m_cphySapProvider->AddUe (m_rnti);

  // setup the eNB side of SRB0
  {
    uint8_t lcid = 0;

    Ptr<NistLteRlc> rlc = CreateObject<NistLteRlcTm> ()->GetObject<NistLteRlc> ();
    rlc->SetNistLteMacSapProvider (m_rrc->m_macSapProvider);
    rlc->SetRnti (m_rnti);
    rlc->SetLcId (lcid);

    m_srb0 = CreateObject<LteSignalingRadioNistBearerInfo> ();
    m_srb0->m_rlc = rlc;
    m_srb0->m_srbIdentity = 0;
    // no need to store logicalChannelConfig as SRB0 is pre-configured

    NistLteEnbCmacSapProvider::NistLcInfo lcinfo;
    lcinfo.rnti = m_rnti;
    lcinfo.lcId = lcid;
    // leave the rest of lcinfo empty as CCCH (LCID 0) is pre-configured
    m_rrc->m_cmacSapProvider->AddLc (lcinfo, rlc->GetNistLteMacSapUser ());

  }

  // setup the eNB side of SRB1; the UE side will be set up upon RRC connection establishment
  {
    uint8_t lcid = 1;

    Ptr<NistLteRlc> rlc = CreateObject<NistLteRlcAm> ()->GetObject<NistLteRlc> ();
    rlc->SetNistLteMacSapProvider (m_rrc->m_macSapProvider);
    rlc->SetRnti (m_rnti);
    rlc->SetLcId (lcid);

    Ptr<NistLtePdcp> pdcp = CreateObject<NistLtePdcp> ();
    pdcp->SetRnti (m_rnti);
    pdcp->SetLcId (lcid);
    pdcp->SetNistLtePdcpSapUser (m_drbPdcpSapUser);
    pdcp->SetNistLteRlcSapProvider (rlc->GetNistLteRlcSapProvider ());
    rlc->SetNistLteRlcSapUser (pdcp->GetNistLteRlcSapUser ());

    m_srb1 = CreateObject<LteSignalingRadioNistBearerInfo> ();
    m_srb1->m_rlc = rlc;
    m_srb1->m_pdcp = pdcp;
    m_srb1->m_srbIdentity = 1;
    m_srb1->m_logicalChannelConfig.priority = 0;
    m_srb1->m_logicalChannelConfig.prioritizedBitRateKbps = 100;
    m_srb1->m_logicalChannelConfig.bucketSizeDurationMs = 100;
    m_srb1->m_logicalChannelConfig.logicalChannelGroup = 0;

    NistLteEnbCmacSapProvider::NistLcInfo lcinfo;
    lcinfo.rnti = m_rnti;
    lcinfo.lcId = lcid;
    lcinfo.lcGroup = 0; // all SRBs always mapped to LCG 0
    lcinfo.qci = NistEpsBearer::GBR_CONV_VOICE; // not sure why the FF API requires a CQI even for SRBs...
    lcinfo.isGbr = true;
    lcinfo.mbrUl = 1e6;
    lcinfo.mbrDl = 1e6;
    lcinfo.gbrUl = 1e4;
    lcinfo.gbrDl = 1e4;
    m_rrc->m_cmacSapProvider->AddLc (lcinfo, rlc->GetNistLteMacSapUser ());
  }

  NistLteEnbRrcSapUser::NistSetupUeParameters ueParams;
  ueParams.srb0SapProvider = m_srb0->m_rlc->GetNistLteRlcSapProvider ();
  ueParams.srb1SapProvider = m_srb1->m_pdcp->GetNistLtePdcpSapProvider ();
  m_rrc->m_rrcSapUser->SetupUe (m_rnti, ueParams);

  // configure MAC (and scheduler)
  NistLteEnbCmacSapProvider::NistUeConfig req;
  req.m_rnti = m_rnti;
  req.m_transmissionMode = m_physicalConfigDedicated.antennaInfo.transmissionMode;
  m_rrc->m_cmacSapProvider->UeUpdateConfigurationReq (req);

  // configure PHY
  m_rrc->m_cphySapProvider->SetTransmissionMode (m_rnti, m_physicalConfigDedicated.antennaInfo.transmissionMode);
  m_rrc->m_cphySapProvider->SetSrsConfigurationIndex (m_rnti, m_physicalConfigDedicated.soundingRsUlConfigDedicated.srsConfigIndex);

  // schedule this NistUeManager instance to be deleted if the UE does not give any sign of life within a reasonable time
  Time maxConnectionDelay;
  switch (m_state)
    {
    case INITIAL_RANDOM_ACCESS:
      m_connectionRequestTimeout = Simulator::Schedule (m_rrc->m_connectionRequestTimeoutDuration,
                                                        &NistLteEnbRrc::ConnectionRequestTimeout,
                                                        m_rrc, m_rnti);
      break;

    case HANDOVER_JOINING:
      m_handoverJoiningTimeout = Simulator::Schedule (m_rrc->m_handoverJoiningTimeoutDuration,
                                                      &NistLteEnbRrc::HandoverJoiningTimeout,
                                                      m_rrc, m_rnti);
      break;

    default:
      NS_FATAL_ERROR ("unexpected state " << ToString (m_state));
      break;
    }

}


NistUeManager::~NistUeManager (void)
{
}

void
NistUeManager::DoDispose ()
{
  delete m_drbPdcpSapUser;
  // delete eventual X2-U TEIDs
  for (std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator it = m_drbMap.begin ();
       it != m_drbMap.end ();
       ++it)
    {
      m_rrc->m_x2uTeidInfoMap.erase (it->second->m_gtpTeid);
    }

}

TypeId NistUeManager::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::NistUeManager")
    .SetParent<Object> ()
    .AddConstructor<NistUeManager> ()
    .AddAttribute ("DataRadioBearerMap", "List of UE DataRadioNistBearerInfo by DRBID.",
                   ObjectMapValue (),
                   MakeObjectMapAccessor (&NistUeManager::m_drbMap),
                   MakeObjectMapChecker<LteDataRadioNistBearerInfo> ())
    .AddAttribute ("Srb0", "SignalingRadioNistBearerInfo for SRB0",
                   PointerValue (),
                   MakePointerAccessor (&NistUeManager::m_srb0),
                   MakePointerChecker<LteSignalingRadioNistBearerInfo> ())
    .AddAttribute ("Srb1", "SignalingRadioNistBearerInfo for SRB1",
                   PointerValue (),
                   MakePointerAccessor (&NistUeManager::m_srb1),
                   MakePointerChecker<LteSignalingRadioNistBearerInfo> ())
    .AddAttribute ("C-RNTI",
                   "Cell Radio Network Temporary Identifier",
                   TypeId::ATTR_GET, // read-only attribute
                   UintegerValue (0), // unused, read-only attribute
                   MakeUintegerAccessor (&NistUeManager::m_rnti),
                   MakeUintegerChecker<uint16_t> ())
    .AddTraceSource ("StateTransition",
                     "fired upon every UE state transition seen by the "
                     "NistUeManager at the eNB RRC",
                     MakeTraceSourceAccessor (&NistUeManager::m_stateTransitionTrace),
                     "ns3::NistUeManager::StateTracedCallback")
  ;
  return tid;
}

void 
NistUeManager::SetSource (uint16_t sourceCellId, uint16_t sourceX2apId)
{
  m_sourceX2apId = sourceX2apId;
  m_sourceCellId = sourceCellId;
}

void 
NistUeManager::SetImsi (uint64_t imsi)
{
  m_imsi = imsi;
}

void
NistUeManager::SetupDataRadioBearer (NistEpsBearer bearer, uint8_t bearerId, uint32_t gtpTeid, Ipv4Address transportLayerAddress)
{
  NS_LOG_FUNCTION (this << (uint32_t) m_rnti);

  Ptr<LteDataRadioNistBearerInfo> drbInfo = CreateObject<LteDataRadioNistBearerInfo> ();
  uint8_t drbid = AddDataRadioNistBearerInfo (drbInfo);
  uint8_t lcid = Drbid2Lcid (drbid); 
  uint8_t bid = Drbid2Bid (drbid);
  NS_ASSERT_MSG ( bearerId == 0 || bid == bearerId, "bearer ID mismatch (" << (uint32_t) bid << " != " << (uint32_t) bearerId << ", the assumption that ID are allocated in the same way by MME and RRC is not valid any more");
  drbInfo->m_epsBearerIdentity = bid;
  drbInfo->m_drbIdentity = drbid;
  drbInfo->m_logicalChannelIdentity = lcid;
  drbInfo->m_gtpTeid = gtpTeid;
  drbInfo->m_transportLayerAddress = transportLayerAddress;

  if (m_state == HANDOVER_JOINING)
    {
      // setup TEIDs for receiving data eventually forwarded over X2-U 
      NistLteEnbRrc::NistX2uTeidInfo x2uTeidInfo;
      x2uTeidInfo.rnti = m_rnti;
      x2uTeidInfo.drbid = drbid;
      std::pair<std::map<uint32_t, NistLteEnbRrc::NistX2uTeidInfo>::iterator, bool>
      ret = m_rrc->m_x2uTeidInfoMap.insert (std::pair<uint32_t, NistLteEnbRrc::NistX2uTeidInfo> (gtpTeid, x2uTeidInfo));
      NS_ASSERT_MSG (ret.second == true, "overwriting a pre-existing entry in m_x2uTeidInfoMap");
    }

  TypeId rlcTypeId = m_rrc->GetRlcType (bearer);

  ObjectFactory rlcObjectFactory;
  rlcObjectFactory.SetTypeId (rlcTypeId);
  Ptr<NistLteRlc> rlc = rlcObjectFactory.Create ()->GetObject<NistLteRlc> ();
  rlc->SetNistLteMacSapProvider (m_rrc->m_macSapProvider);
  rlc->SetRnti (m_rnti);

  drbInfo->m_rlc = rlc;

  rlc->SetLcId (lcid);

  // we need PDCP only for real RLC, i.e., RLC/UM or RLC/AM
  // if we are using RLC/SM we don't care of anything above RLC
  if (rlcTypeId != NistLteRlcSm::GetTypeId ())
    {
      Ptr<NistLtePdcp> pdcp = CreateObject<NistLtePdcp> ();
      pdcp->SetRnti (m_rnti);
      pdcp->SetLcId (lcid);
      pdcp->SetNistLtePdcpSapUser (m_drbPdcpSapUser);
      pdcp->SetNistLteRlcSapProvider (rlc->GetNistLteRlcSapProvider ());
      rlc->SetNistLteRlcSapUser (pdcp->GetNistLteRlcSapUser ());
      drbInfo->m_pdcp = pdcp;
    }

  NistLteEnbCmacSapProvider::NistLcInfo lcinfo;
  lcinfo.rnti = m_rnti;
  lcinfo.lcId = lcid;
  lcinfo.lcGroup = m_rrc->GetLogicalChannelGroup (bearer);
  lcinfo.qci = bearer.qci;
  lcinfo.isGbr = bearer.IsGbr ();
  lcinfo.mbrUl = bearer.gbrQosInfo.mbrUl;
  lcinfo.mbrDl = bearer.gbrQosInfo.mbrDl;
  lcinfo.gbrUl = bearer.gbrQosInfo.gbrUl;
  lcinfo.gbrDl = bearer.gbrQosInfo.gbrDl;
  m_rrc->m_cmacSapProvider->AddLc (lcinfo, rlc->GetNistLteMacSapUser ());

  if (rlcTypeId == NistLteRlcAm::GetTypeId ())
    {
      drbInfo->m_rlcConfig.choice =  NistLteRrcSap::NistRlcConfig::AM;
    }
  else
    {
      drbInfo->m_rlcConfig.choice =  NistLteRrcSap::NistRlcConfig::UM_BI_DIRECTIONAL;
    }

  drbInfo->m_logicalChannelIdentity = lcid;
  drbInfo->m_logicalChannelConfig.priority =  m_rrc->GetLogicalChannelPriority (bearer);
  drbInfo->m_logicalChannelConfig.logicalChannelGroup = m_rrc->GetLogicalChannelGroup (bearer);
  if (bearer.IsGbr ())
    {
      drbInfo->m_logicalChannelConfig.prioritizedBitRateKbps = bearer.gbrQosInfo.gbrUl;
    }
  else
    {
      drbInfo->m_logicalChannelConfig.prioritizedBitRateKbps = 0;
    }
  drbInfo->m_logicalChannelConfig.bucketSizeDurationMs = 1000;

  ScheduleNistRrcConnectionReconfiguration ();
}

void
NistUeManager::RecordDataRadioBearersToBeStarted ()
{
  NS_LOG_FUNCTION (this << (uint32_t) m_rnti);
  for (std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator it = m_drbMap.begin ();
       it != m_drbMap.end ();
       ++it)
    {
      m_drbsToBeStarted.push_back (it->first);
    }
}

void
NistUeManager::StartDataRadioBearers ()
{
  NS_LOG_FUNCTION (this << (uint32_t) m_rnti);
  for (std::list <uint8_t>::iterator drbIdIt = m_drbsToBeStarted.begin ();
       drbIdIt != m_drbsToBeStarted.end ();
       ++drbIdIt)
    {
      std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator drbIt = m_drbMap.find (*drbIdIt);
      NS_ASSERT (drbIt != m_drbMap.end ());
      drbIt->second->m_rlc->Initialize ();
      if (drbIt->second->m_pdcp)
        {
          drbIt->second->m_pdcp->Initialize ();
        }
    }
  m_drbsToBeStarted.clear ();
}


void
NistUeManager::ReleaseDataRadioBearer (uint8_t drbid)
{
  NS_LOG_FUNCTION (this << (uint32_t) m_rnti << (uint32_t) drbid);
  uint8_t lcid = Drbid2Lcid (drbid);
  std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator it = m_drbMap.find (drbid);
  NS_ASSERT_MSG (it != m_drbMap.end (), "request to remove radio bearer with unknown drbid " << drbid);

  // first delete eventual X2-U TEIDs
  m_rrc->m_x2uTeidInfoMap.erase (it->second->m_gtpTeid);

  m_drbMap.erase (it);
  m_rrc->m_cmacSapProvider->ReleaseLc (m_rnti, lcid);

  NistLteRrcSap::NistRadioResourceConfigDedicated rrcd;
  rrcd.haveNistPhysicalConfigDedicated = false;
  rrcd.drbToReleaseList.push_back (drbid);
  //populating NistRadioResourceConfigDedicated information element as per 3GPP TS 36.331 version 9.2.0
  rrcd.haveNistPhysicalConfigDedicated = true;
  rrcd.physicalConfigDedicated = m_physicalConfigDedicated;
 
  //populating RRCConnectionReconfiguration message as per 3GPP TS 36.331 version 9.2.0 Release 9
  NistLteRrcSap::NistRrcConnectionReconfiguration msg;
  msg.haveNistMeasConfig = false;
  msg.haveNistMobilityControlInfo = false;
  msg.radioResourceConfigDedicated = rrcd;
  msg.haveNistRadioResourceConfigDedicated = true;
  //RRC Connection Reconfiguration towards UE
  m_rrc->m_rrcSapUser->SendNistRrcConnectionReconfiguration (m_rnti, msg);
}

void
NistLteEnbRrc::DoSendReleaseDataRadioBearer (uint64_t imsi, uint16_t rnti, uint8_t bearerId)
{
  Ptr<NistUeManager> ueManager = GetNistUeManager (rnti);
  // Bearer de-activation towards UE
  ueManager->ReleaseDataRadioBearer (bearerId);
  // Bearer de-activation indication towards epc-enb application
  m_s1SapProvider->DoSendReleaseIndication (imsi,rnti,bearerId);
}

void 
NistUeManager::ScheduleNistRrcConnectionReconfiguration ()
{
  NS_LOG_FUNCTION (this);
  switch (m_state)
    {
    case INITIAL_RANDOM_ACCESS:
    case CONNECTION_SETUP:
    case CONNECTION_RECONFIGURATION:
    case CONNECTION_REESTABLISHMENT:
    case HANDOVER_PREPARATION:
    case HANDOVER_JOINING:
    case HANDOVER_LEAVING:
      // a previous reconfiguration still ongoing, we need to wait for it to be finished
      m_pendingNistRrcConnectionReconfiguration = true;
      break;

    case CONNECTED_NORMALLY:
      {
        m_pendingNistRrcConnectionReconfiguration = false;
        NistLteRrcSap::NistRrcConnectionReconfiguration msg = BuildNistRrcConnectionReconfiguration ();
        m_rrc->m_rrcSapUser->SendNistRrcConnectionReconfiguration (m_rnti, msg);
        RecordDataRadioBearersToBeStarted ();
        SwitchToState (CONNECTION_RECONFIGURATION);
      }
      break;

    default:
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }
}

void 
NistUeManager::PrepareHandover (uint16_t cellId)
{
  NS_LOG_FUNCTION (this << cellId);
  switch (m_state)
    {
    case CONNECTED_NORMALLY:
      {
        m_targetCellId = cellId;
        NistEpcX2SapProvider::NistHandoverRequestParams params;
        params.oldEnbUeX2apId = m_rnti;
        params.cause          = NistEpcX2SapProvider::HandoverDesirableForRadioReason;
        params.sourceCellId   = m_rrc->m_cellId;
        params.targetCellId   = cellId;
        params.mmeUeS1apId    = m_imsi;
        params.ueAggregateMaxBitRateDownlink = 200 * 1000;
        params.ueAggregateMaxBitRateUplink = 100 * 1000;
        params.bearers = GetErabList ();

        NistLteRrcSap::NistHandoverPreparationInfo hpi;
        hpi.asConfig.sourceUeIdentity = m_rnti;
        hpi.asConfig.sourceDlCarrierFreq = m_rrc->m_dlEarfcn;
        hpi.asConfig.sourceNistMeasConfig = m_rrc->m_ueNistMeasConfig;
        hpi.asConfig.sourceRadioResourceConfig = GetRadioResourceConfigForNistHandoverPreparationInfo ();
        hpi.asConfig.sourceNistMasterInformationBlock.dlBandwidth = m_rrc->m_dlBandwidth;
        hpi.asConfig.sourceNistMasterInformationBlock.systemFrameNumber = 0;
        hpi.asConfig.sourceNistSystemInformationBlockType1.cellAccessRelatedInfo.plmnIdentityInfo.plmnIdentity = m_rrc->m_sib1.cellAccessRelatedInfo.plmnIdentityInfo.plmnIdentity;
        hpi.asConfig.sourceNistSystemInformationBlockType1.cellAccessRelatedInfo.cellIdentity = m_rrc->m_cellId;
        hpi.asConfig.sourceNistSystemInformationBlockType1.cellAccessRelatedInfo.csgIndication = m_rrc->m_sib1.cellAccessRelatedInfo.csgIndication;
        hpi.asConfig.sourceNistSystemInformationBlockType1.cellAccessRelatedInfo.csgIdentity = m_rrc->m_sib1.cellAccessRelatedInfo.csgIdentity;
        NistLteEnbCmacSapProvider::NistRachConfig rc = m_rrc->m_cmacSapProvider->GetNistRachConfig ();
        hpi.asConfig.sourceNistSystemInformationBlockType2.radioResourceConfigCommon.rachConfigCommon.preambleInfo.numberOfRaPreambles = rc.numberOfRaPreambles;
        hpi.asConfig.sourceNistSystemInformationBlockType2.radioResourceConfigCommon.rachConfigCommon.raSupervisionInfo.preambleTransMax = rc.preambleTransMax;
        hpi.asConfig.sourceNistSystemInformationBlockType2.radioResourceConfigCommon.rachConfigCommon.raSupervisionInfo.raResponseWindowSize = rc.raResponseWindowSize;
        hpi.asConfig.sourceNistSystemInformationBlockType2.freqInfo.ulCarrierFreq = m_rrc->m_ulEarfcn;
        hpi.asConfig.sourceNistSystemInformationBlockType2.freqInfo.ulBandwidth = m_rrc->m_ulBandwidth;
        params.rrcContext = m_rrc->m_rrcSapUser->EncodeNistHandoverPreparationInformation (hpi);

        NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
        NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
        NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
        NS_LOG_LOGIC ("mmeUeS1apId = " << params.mmeUeS1apId);
        NS_LOG_LOGIC ("rrcContext   = " << params.rrcContext);

        m_rrc->m_x2SapProvider->SendHandoverRequest (params);
        SwitchToState (HANDOVER_PREPARATION);
      }
      break;

    default:
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }

}

void 
NistUeManager::RecvHandoverRequestAck (NistEpcX2SapUser::NistHandoverRequestAckParams params)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT_MSG (params.notAdmittedBearers.empty (), "not admission of some bearers upon handover is not supported");
  NS_ASSERT_MSG (params.admittedBearers.size () == m_drbMap.size (), "not enough bearers in admittedBearers");

  // note: the Handover command from the target eNB to the source eNB
  // is expected to be sent transparently to the UE; however, here we
  // decode the message and eventually reencode it. This way we can
  // support both a real RRC protocol implementation and an ideal one
  // without actual RRC protocol encoding. 

  Ptr<Packet> encodedHandoverCommand = params.rrcContext;
  NistLteRrcSap::NistRrcConnectionReconfiguration handoverCommand = m_rrc->m_rrcSapUser->DecodeHandoverCommand (encodedHandoverCommand);
  m_rrc->m_rrcSapUser->SendNistRrcConnectionReconfiguration (m_rnti, handoverCommand);
  SwitchToState (HANDOVER_LEAVING);
  m_handoverLeavingTimeout = Simulator::Schedule (m_rrc->m_handoverLeavingTimeoutDuration, 
                                                  &NistLteEnbRrc::HandoverLeavingTimeout, 
                                                  m_rrc, m_rnti);
  NS_ASSERT (handoverCommand.haveNistMobilityControlInfo);
  m_rrc->m_handoverStartTrace (m_imsi, m_rrc->m_cellId, m_rnti, handoverCommand.mobilityControlInfo.targetPhysCellId);

  NistEpcX2SapProvider::NistSnNistStatusTransferParams sst;
  sst.oldEnbUeX2apId = params.oldEnbUeX2apId;
  sst.newEnbUeX2apId = params.newEnbUeX2apId;
  sst.sourceCellId = params.sourceCellId;
  sst.targetCellId = params.targetCellId;
  for ( std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator drbIt = m_drbMap.begin ();
        drbIt != m_drbMap.end ();
        ++drbIt)
    {
      // SN status transfer is only for AM RLC
      if (0 != drbIt->second->m_rlc->GetObject<NistLteRlcAm> ())
        {
          NistLtePdcp::NistStatus status = drbIt->second->m_pdcp->GetNistStatus ();
          NistEpcX2Sap::NistErabsSubjectToNistStatusTransferItem i;
          i.dlPdcpSn = status.txSn;
          i.ulPdcpSn = status.rxSn;
          sst.erabsSubjectToNistStatusTransferList.push_back (i);
        }
    }
  m_rrc->m_x2SapProvider->SendSnNistStatusTransfer (sst);
}


NistLteRrcSap::NistRadioResourceConfigDedicated
NistUeManager::GetRadioResourceConfigForNistHandoverPreparationInfo ()
{
  NS_LOG_FUNCTION (this);
  return BuildNistRadioResourceConfigDedicated ();
}

NistLteRrcSap::NistRrcConnectionReconfiguration 
NistUeManager::GetNistRrcConnectionReconfigurationForHandover ()
{
  NS_LOG_FUNCTION (this);
  return BuildNistRrcConnectionReconfiguration ();
}

void
NistUeManager::SendData (uint8_t bid, Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p << (uint16_t) bid);
  switch (m_state)
    {
    case INITIAL_RANDOM_ACCESS:
    case CONNECTION_SETUP:
      NS_LOG_WARN ("not connected, discarding packet");
      return;
      break;

    case CONNECTED_NORMALLY:
    case CONNECTION_RECONFIGURATION:
    case CONNECTION_REESTABLISHMENT:
    case HANDOVER_PREPARATION:
    case HANDOVER_JOINING:
    case HANDOVER_PATH_SWITCH:
      {
        NS_LOG_LOGIC ("queueing data on PDCP for transmission over the air");
        NistLtePdcpSapProvider::NistTransmitPdcpSduParameters params;
        params.pdcpSdu = p;
        params.rnti = m_rnti;
        params.lcid = Bid2Lcid (bid);
        uint8_t drbid = Bid2Drbid (bid);
        //Transmit PDCP sdu only if DRB ID found in drbMap
        std::map<uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator it = m_drbMap.find (drbid);
        if (it != m_drbMap.end ())
          {
            Ptr<LteDataRadioNistBearerInfo> bearerInfo = GetDataRadioNistBearerInfo (drbid);
            if (bearerInfo != NULL)
              {
                NistLtePdcpSapProvider* pdcpSapProvider = bearerInfo->m_pdcp->GetNistLtePdcpSapProvider ();
        pdcpSapProvider->TransmitPdcpSdu (params);
      }
          }
      }
      break;

    case HANDOVER_LEAVING:
      {
        NS_LOG_LOGIC ("forwarding data to target eNB over X2-U");
        uint8_t drbid = Bid2Drbid (bid);
        NistEpcX2Sap::NistUeDataParams params;
        params.sourceCellId = m_rrc->m_cellId;
        params.targetCellId = m_targetCellId;
        params.gtpTeid = GetDataRadioNistBearerInfo (drbid)->m_gtpTeid;
        params.ueData = p;
        m_rrc->m_x2SapProvider->SendUeData (params);
      }
      break;

    default:
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }
}

std::vector<NistEpcX2Sap::NistErabToBeSetupItem>
NistUeManager::GetErabList ()
{
  NS_LOG_FUNCTION (this);
  std::vector<NistEpcX2Sap::NistErabToBeSetupItem> ret;
  for (std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator it =  m_drbMap.begin ();
       it != m_drbMap.end ();
       ++it)
    {
      NistEpcX2Sap::NistErabToBeSetupItem etbsi;
      etbsi.erabId = it->second->m_epsBearerIdentity;
      etbsi.erabLevelQosParameters = it->second->m_epsBearer;
      etbsi.dlForwarding = false;
      etbsi.transportLayerAddress = it->second->m_transportLayerAddress;
      etbsi.gtpTeid = it->second->m_gtpTeid;
      ret.push_back (etbsi);
    }
  return ret;
}

void
NistUeManager::SendUeContextRelease ()
{
  NS_LOG_FUNCTION (this);
  switch (m_state)
    {
    case HANDOVER_PATH_SWITCH:
      NS_LOG_INFO ("Send UE CONTEXT RELEASE from target eNB to source eNB");
      NistEpcX2SapProvider::NistUeContextReleaseParams ueCtxReleaseParams;
      ueCtxReleaseParams.oldEnbUeX2apId = m_sourceX2apId;
      ueCtxReleaseParams.newEnbUeX2apId = m_rnti;
      ueCtxReleaseParams.sourceCellId = m_sourceCellId;
      m_rrc->m_x2SapProvider->SendUeContextRelease (ueCtxReleaseParams);
      SwitchToState (CONNECTED_NORMALLY);
      m_rrc->m_handoverEndOkTrace (m_imsi, m_rrc->m_cellId, m_rnti);
      break;

    default:
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }
}

void 
NistUeManager::RecvHandoverPreparationFailure (uint16_t cellId)
{
  NS_LOG_FUNCTION (this << cellId);
  switch (m_state)
    {
    case HANDOVER_PREPARATION:
      NS_ASSERT (cellId == m_targetCellId);
      NS_LOG_INFO ("target eNB sent HO preparation failure, aborting HO");
      SwitchToState (CONNECTED_NORMALLY);
      break;

    default:
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }
}

void 
NistUeManager::RecvSnNistStatusTransfer (NistEpcX2SapUser::NistSnNistStatusTransferParams params)
{
  NS_LOG_FUNCTION (this);
  for (std::vector<NistEpcX2Sap::NistErabsSubjectToNistStatusTransferItem>::iterator erabIt 
         = params.erabsSubjectToNistStatusTransferList.begin ();
       erabIt != params.erabsSubjectToNistStatusTransferList.end ();
       ++erabIt)
    {
      // NistLtePdcp::NistStatus status;
      // status.txSn = erabIt->dlPdcpSn;
      // status.rxSn = erabIt->ulPdcpSn;
      // uint8_t drbId = Bid2Drbid (erabIt->erabId);
      // std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator drbIt = m_drbMap.find (drbId);
      // NS_ASSERT_MSG (drbIt != m_drbMap.end (), "could not find DRBID " << (uint32_t) drbId);
      // drbIt->second->m_pdcp->SetNistStatus (status);
    }
}

void 
NistUeManager::RecvUeContextRelease (NistEpcX2SapUser::NistUeContextReleaseParams params)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (m_state == HANDOVER_LEAVING, "method unexpected in state " << ToString (m_state));
  m_handoverLeavingTimeout.Cancel ();
}


// methods forwarded from RRC SAP

void 
NistUeManager::CompleteSetupUe (NistLteEnbRrcSapProvider::CompleteNistSetupUeParameters params)
{
  NS_LOG_FUNCTION (this);
  m_srb0->m_rlc->SetNistLteRlcSapUser (params.srb0SapUser);
  m_srb1->m_pdcp->SetNistLtePdcpSapUser (params.srb1SapUser);
}

void
NistUeManager::RecvNistRrcConnectionRequest (NistLteRrcSap::NistRrcConnectionRequest msg)
{
  NS_LOG_FUNCTION (this);
  switch (m_state)
    {
    case INITIAL_RANDOM_ACCESS:
      {
        m_connectionRequestTimeout.Cancel ();

        if (m_rrc->m_admitNistRrcConnectionRequest == true)
          {
            m_imsi = msg.ueIdentity;
            if (m_rrc->m_s1SapProvider != 0)
              {
                m_rrc->m_s1SapProvider->InitialUeMessage (m_imsi, m_rnti);
              }

            // send RRC CONNECTION SETUP to UE
            NistLteRrcSap::NistRrcConnectionSetup msg2;
            msg2.rrcTransactionIdentifier = GetNewRrcTransactionIdentifier ();
            msg2.radioResourceConfigDedicated = BuildNistRadioResourceConfigDedicated ();
            m_rrc->m_rrcSapUser->SendNistRrcConnectionSetup (m_rnti, msg2);

            RecordDataRadioBearersToBeStarted ();
            m_connectionSetupTimeout = Simulator::Schedule (
                m_rrc->m_connectionSetupTimeoutDuration,
                &NistLteEnbRrc::ConnectionSetupTimeout, m_rrc, m_rnti);
            SwitchToState (CONNECTION_SETUP);
          }
        else
          {
            NS_LOG_INFO ("rejecting connection request for RNTI " << m_rnti);

            // send RRC CONNECTION REJECT to UE
            NistLteRrcSap::NistRrcConnectionReject rejectMsg;
            rejectMsg.waitTime = 3;
            m_rrc->m_rrcSapUser->SendNistRrcConnectionReject (m_rnti, rejectMsg);

            m_connectionRejectedTimeout = Simulator::Schedule (
                m_rrc->m_connectionRejectedTimeoutDuration,
                &NistLteEnbRrc::ConnectionRejectedTimeout, m_rrc, m_rnti);
            SwitchToState (CONNECTION_REJECTED);
          }
      }
      break;

    default:
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }
}

void
NistUeManager::RecvNistRrcConnectionSetupCompleted (NistLteRrcSap::NistRrcConnectionSetupCompleted msg)
{
  NS_LOG_FUNCTION (this);
  switch (m_state)
    {
    case CONNECTION_SETUP:
      m_connectionSetupTimeout.Cancel ();
      StartDataRadioBearers ();
      SwitchToState (CONNECTED_NORMALLY);
      m_rrc->m_connectionEstablishedTrace (m_imsi, m_rrc->m_cellId, m_rnti);
      break;

    default:
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }
}

void
NistUeManager::RecvNistRrcConnectionReconfigurationCompleted (NistLteRrcSap::NistRrcConnectionReconfigurationCompleted msg)
{
  NS_LOG_FUNCTION (this);
  switch (m_state)
    {
    case CONNECTION_RECONFIGURATION:
      StartDataRadioBearers ();
      if (m_needPhyMacConfiguration)
        {
          // configure MAC (and scheduler)
          NistLteEnbCmacSapProvider::NistUeConfig req;
          req.m_rnti = m_rnti;
          req.m_transmissionMode = m_physicalConfigDedicated.antennaInfo.transmissionMode;
          req.m_slDestinations = m_slDestinations;
          m_rrc->m_cmacSapProvider->UeUpdateConfigurationReq (req);

          // configure PHY
          m_rrc->m_cphySapProvider->SetTransmissionMode (req.m_rnti, req.m_transmissionMode);

          double paDouble = NistLteRrcSap::ConvertNistPdschConfigDedicated2Double (m_physicalConfigDedicated.pdschConfigDedicated);
          m_rrc->m_cphySapProvider->SetPa (m_rnti, paDouble);

          m_needPhyMacConfiguration = false;
        }
      SwitchToState (CONNECTED_NORMALLY);
      m_rrc->m_connectionReconfigurationTrace (m_imsi, m_rrc->m_cellId, m_rnti);
      break;

    // This case is added to NS-3 in order to handle bearer de-activation scenario for CONNECTED state UE
    case CONNECTED_NORMALLY:
      if (!m_slPoolChanged) {
        NS_LOG_INFO ("ignoring RecvNistRrcConnectionReconfigurationCompleted in state " << ToString (m_state));
      } else {
        //inform about new sidelink pool if needed
        // configure MAC (and scheduler)
          NistLteEnbCmacSapProvider::NistUeConfig req;
          req.m_rnti = m_rnti;
          req.m_transmissionMode = m_physicalConfigDedicated.antennaInfo.transmissionMode;
          req.m_slDestinations = m_slDestinations;
          m_rrc->m_cmacSapProvider->UeUpdateConfigurationReq (req);
          
        m_slPoolChanged = false;
      }
      break;

    case HANDOVER_LEAVING:
      NS_LOG_INFO ("ignoring RecvNistRrcConnectionReconfigurationCompleted in state " << ToString (m_state));
      break;

    case HANDOVER_JOINING:
      {
        m_handoverJoiningTimeout.Cancel ();
        NS_LOG_INFO ("Send PATH SWITCH REQUEST to the MME");
        NistEpcEnbS1SapProvider::NistPathSwitchRequestParameters params;
        params.rnti = m_rnti;
        params.cellId = m_rrc->m_cellId;
        params.mmeUeS1Id = m_imsi;
        SwitchToState (HANDOVER_PATH_SWITCH);
        for (std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator it =  m_drbMap.begin ();
             it != m_drbMap.end ();
             ++it)
          {
            NistEpcEnbS1SapProvider::NistBearerToBeSwitched b;
            b.epsBearerId = it->second->m_epsBearerIdentity;
            b.teid =  it->second->m_gtpTeid;
            params.bearersToBeSwitched.push_back (b);
          }
        m_rrc->m_s1SapProvider->PathSwitchRequest (params);
      }
      break;

    default:
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }
}

void 
NistUeManager::RecvNistRrcConnectionReestablishmentRequest (NistLteRrcSap::NistRrcConnectionReestablishmentRequest msg)
{
  NS_LOG_FUNCTION (this);
  switch (m_state)
    {
    case CONNECTED_NORMALLY:
      break;

    case HANDOVER_LEAVING:
      m_handoverLeavingTimeout.Cancel ();
      break;

    default:
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }

  NistLteRrcSap::NistRrcConnectionReestablishment msg2;
  msg2.rrcTransactionIdentifier = GetNewRrcTransactionIdentifier ();
  msg2.radioResourceConfigDedicated = BuildNistRadioResourceConfigDedicated ();
  m_rrc->m_rrcSapUser->SendNistRrcConnectionReestablishment (m_rnti, msg2);
  SwitchToState (CONNECTION_REESTABLISHMENT);
}

void 
NistUeManager::RecvNistRrcConnectionReestablishmentComplete (NistLteRrcSap::NistRrcConnectionReestablishmentComplete msg)
{
  NS_LOG_FUNCTION (this);
  SwitchToState (CONNECTED_NORMALLY);
}

void 
NistUeManager::RecvNistMeasurementReport (NistLteRrcSap::NistMeasurementReport msg)
{
  uint8_t measId = msg.measResults.measId;
  NS_LOG_FUNCTION (this << (uint16_t) measId);
  NS_LOG_LOGIC ("measId " << (uint16_t) measId
                          << " haveMeasResultNeighCells " << msg.measResults.haveMeasResultNeighCells
                          << " measResultListEutra " << msg.measResults.measResultListEutra.size ());
  NS_LOG_LOGIC ("serving cellId " << m_rrc->m_cellId
                                  << " RSRP " << (uint16_t) msg.measResults.rsrpResult
                                  << " RSRQ " << (uint16_t) msg.measResults.rsrqResult);

  for (std::list <NistLteRrcSap::NistMeasResultEutra>::iterator it = msg.measResults.measResultListEutra.begin ();
       it != msg.measResults.measResultListEutra.end ();
       ++it)
    {
      NS_LOG_LOGIC ("neighbour cellId " << it->physCellId
                                        << " RSRP " << (it->haveRsrpResult ? (uint16_t) it->rsrpResult : 255)
                                        << " RSRQ " << (it->haveRsrqResult ? (uint16_t) it->rsrqResult : 255));
    }

  if ((m_rrc->m_handoverManagementSapProvider != 0)
      && (m_rrc->m_handoverMeasIds.find (measId) != m_rrc->m_handoverMeasIds.end ()))
    {
      // this measurement was requested by the handover algorithm
      m_rrc->m_handoverManagementSapProvider->ReportUeMeas (m_rnti,
                                                            msg.measResults);
    }

  if ((m_rrc->m_anrSapProvider != 0)
      && (m_rrc->m_anrMeasIds.find (measId) != m_rrc->m_anrMeasIds.end ()))
    {
      // this measurement was requested by the ANR function
      m_rrc->m_anrSapProvider->ReportUeMeas (msg.measResults);
    }

  if ((m_rrc->m_ffrRrcSapProvider != 0)
      && (m_rrc->m_ffrMeasIds.find (measId) != m_rrc->m_ffrMeasIds.end ()))
    {
      // this measurement was requested by the FFR function
      m_rrc->m_ffrRrcSapProvider->ReportUeMeas (m_rnti, msg.measResults);
    }

  // fire a trace source
  m_rrc->m_recvNistMeasurementReportTrace (m_imsi, m_rrc->m_cellId, m_rnti, msg);

} // end of NistUeManager::RecvNistMeasurementReport


void
NistUeManager::RecvSidelinkUeInformation (NistLteRrcSap::SidelinkUeInformation msg)
{
  NS_LOG_FUNCTION (this);
  //Parse message and decide on allocation
  //for now we only handle one group per UE
  if (msg.haveCommTxResourceReq)  {
    NS_ASSERT_MSG (msg.slCommTxResourceReq.slDestinationInfoList.nbDestinations=1, "Current implementation does not support more than 1 group per UE");

    //store destination list in the UE
    m_slDestinations.clear ();
    for (int i = 0 ; i < msg.slCommTxResourceReq.slDestinationInfoList.nbDestinations; i++) {
      m_slDestinations.push_back (msg.slCommTxResourceReq.slDestinationInfoList.SlDestinationIdentity[i]);
    }

    
    //populate dedicated resources
    NistLteRrcSap::SlCommConfig dedicatedResource;
    dedicatedResource.commTxResources = NistLteRrcSap::SlCommConfig::SETUP;
    
    //check if pool already in use
    std::map <uint32_t, LteEnbRrcSl::ActivePoolInfo>::iterator it2 = m_rrc->m_sidelinkConfiguration->m_activePoolMap.find (msg.slCommTxResourceReq.slDestinationInfoList.SlDestinationIdentity[0]);
    
    if (it2 == m_rrc->m_sidelinkConfiguration->m_activePoolMap.end())
      {
        //no active pool for this group, let's check if we have a preconfigured one
        //or later, how to create one dynamically
        std::map <uint32_t, NistLteRrcSap::SlCommTxResourcesSetup>::iterator it = m_rrc->m_sidelinkConfiguration->m_preconfigDedicatedPoolMap.find (msg.slCommTxResourceReq.slDestinationInfoList.SlDestinationIdentity[0]);

        if (it != m_rrc->m_sidelinkConfiguration->m_preconfigDedicatedPoolMap.end())
          {
            dedicatedResource.setup = it->second;
          } else
          {
            NS_LOG_INFO ("No pre-provisioned pool found for group " << msg.slCommTxResourceReq.slDestinationInfoList.SlDestinationIdentity[0]);
          }
      
        Ptr<SidelinkTxCommResourcePool> m_slPool = CreateObject<SidelinkTxCommResourcePool> ();
        if (dedicatedResource.setup.setup == NistLteRrcSap::SlCommTxResourcesSetup::SCHEDULED)
          {
            m_slPool->SetPool (dedicatedResource.setup.scheduled.commTxConfig);
            if (dedicatedResource.setup.scheduled.haveMcs) {
              m_slPool->SetScheduledTxParameters (m_rnti, dedicatedResource.setup.scheduled.macMainConfig, dedicatedResource.setup.scheduled.commTxConfig, 0, dedicatedResource.setup.scheduled.mcs);
            } else
              {
                m_slPool->SetScheduledTxParameters (m_rnti, dedicatedResource.setup.scheduled.macMainConfig, dedicatedResource.setup.scheduled.commTxConfig, 0);
              }
          } else
          {
            //right now, only use the first pool
            NS_ASSERT (dedicatedResource.setup.ueSelected.havePoolToAdd && dedicatedResource.setup.ueSelected.poolToAddModList.nbPools > 0);
            m_slPool->SetPool (dedicatedResource.setup.ueSelected.poolToAddModList.pools[0].pool);
          }

        LteEnbRrcSl::ActivePoolInfo newPoolInfo;
        newPoolInfo.m_pool = m_slPool;
        newPoolInfo.m_poolSetup = it->second;
        newPoolInfo.m_rntiSet.insert (m_rnti);
        //tell the MAC that there is a new pool
        m_rrc->m_cmacSapProvider->AddPool (msg.slCommTxResourceReq.slDestinationInfoList.SlDestinationIdentity[0], m_slPool);

        m_rrc->m_sidelinkConfiguration->m_activePoolMap.insert (std::pair<uint32_t, LteEnbRrcSl::ActivePoolInfo> (msg.slCommTxResourceReq.slDestinationInfoList.SlDestinationIdentity[0], newPoolInfo));
        m_slPoolChanged = true;
        
      } else
      {
        dedicatedResource.setup = it2->second.m_poolSetup;
        if (it2->second.m_rntiSet.find (m_rnti) == it2->second.m_rntiSet.end()) {
          //the pool is active but the UE was not on the list
          it2->second.m_rntiSet.insert (m_rnti);
          m_slPoolChanged = true;
        }
      } //else we already know this UE is using this pool
            
    //populating RRCConnectionReconfiguration message 
    NistLteRrcSap::NistRrcConnectionReconfiguration msg2;
    msg2.haveNistMeasConfig = false;
    msg2.haveNistMobilityControlInfo = false;
    msg2.haveNistRadioResourceConfigDedicated = false;
    msg2.haveSlCommConfig = true;
    msg2.slCommConfig = dedicatedResource;
    //RRC Connection Reconfiguration towards UE
    m_rrc->m_rrcSapUser->SendNistRrcConnectionReconfiguration (m_rnti, msg2);
    
    
  } else {
    //must release resources
    
    NistLteRrcSap::SlCommConfig dedicatedResource;
    dedicatedResource.commTxResources = NistLteRrcSap::SlCommConfig::RELEASE;
    //populating RRCConnectionReconfiguration message 
    NistLteRrcSap::NistRrcConnectionReconfiguration msg2;
    msg2.haveNistMeasConfig = false;
    msg2.haveNistMobilityControlInfo = false;
    msg2.haveNistRadioResourceConfigDedicated = false;
    msg2.haveSlCommConfig = true;
    msg2.slCommConfig = dedicatedResource;
    //RRC Connection Reconfiguration towards UE
    m_rrc->m_rrcSapUser->SendNistRrcConnectionReconfiguration (m_rnti, msg2);
      
    m_slDestinations.clear();
    m_slPoolChanged = true;
  }

  if (msg.haveCommRxInterestedFreq) {
    //interest from the UE
  } else if (0) {
    //no more interest from that UE
  }
}
  
// methods forwarded from CMAC SAP

void
NistUeManager::CmacNistUeConfigUpdateInd (NistLteEnbCmacSapUser::NistUeConfig cmacParams)
{
  NS_LOG_FUNCTION (this << m_rnti);
  // at this stage used only by the scheduler for updating txMode

  m_physicalConfigDedicated.antennaInfo.transmissionMode = cmacParams.m_transmissionMode;

  m_needPhyMacConfiguration = true;

  // reconfigure the UE RRC
  ScheduleNistRrcConnectionReconfiguration ();
}


// methods forwarded from PDCP SAP

void
NistUeManager::DoReceivePdcpSdu (NistLtePdcpSapUser::NistReceivePdcpSduParameters params)
{
  NS_LOG_FUNCTION (this);
  if (params.lcid > 2)
    {
      // data radio bearer
      NistEpsBearerTag tag;
      tag.SetRnti (params.rnti);
      tag.SetBid (Lcid2Bid (params.lcid));
      params.pdcpSdu->AddPacketTag (tag);
      m_rrc->m_forwardUpCallback (params.pdcpSdu);
    }
}


uint16_t
NistUeManager::GetRnti (void) const
{
  return m_rnti;
}

uint64_t
NistUeManager::GetImsi (void) const
{
  return m_imsi;
}

uint16_t
NistUeManager::GetSrsConfigurationIndex (void) const
{
  return m_physicalConfigDedicated.soundingRsUlConfigDedicated.srsConfigIndex;
}

void
NistUeManager::SetSrsConfigurationIndex (uint16_t srsConfIndex)
{
  NS_LOG_FUNCTION (this);
  m_physicalConfigDedicated.soundingRsUlConfigDedicated.srsConfigIndex = srsConfIndex;
  m_rrc->m_cphySapProvider->SetSrsConfigurationIndex (m_rnti, srsConfIndex);
  switch (m_state)
    {
    case INITIAL_RANDOM_ACCESS:
      // do nothing, srs conf index will be correctly enforced upon
      // RRC connection establishment
      break;

    default:
      ScheduleNistRrcConnectionReconfiguration ();
      break;
    }
}

NistUeManager::State
NistUeManager::GetState (void) const
{
  return m_state;
}

void
NistUeManager::SetNistPdschConfigDedicated (NistLteRrcSap::NistPdschConfigDedicated pdschConfigDedicated)
{
  NS_LOG_FUNCTION (this);
  m_physicalConfigDedicated.pdschConfigDedicated = pdschConfigDedicated;

  m_needPhyMacConfiguration = true;

  // reconfigure the UE RRC
  ScheduleNistRrcConnectionReconfiguration ();
}

uint8_t
NistUeManager::AddDataRadioNistBearerInfo (Ptr<LteDataRadioNistBearerInfo> drbInfo)
{
  NS_LOG_FUNCTION (this);
  const uint8_t MAX_DRB_ID = 32;
  for (int drbid = (m_lastAllocatedDrbid + 1) % MAX_DRB_ID; 
       drbid != m_lastAllocatedDrbid; 
       drbid = (drbid + 1) % MAX_DRB_ID)
    {
      if (drbid != 0) // 0 is not allowed
        {
          if (m_drbMap.find (drbid) == m_drbMap.end ())
            {
              m_drbMap.insert (std::pair<uint8_t, Ptr<LteDataRadioNistBearerInfo> > (drbid, drbInfo));
              drbInfo->m_drbIdentity = drbid;
              m_lastAllocatedDrbid = drbid;
              return drbid;
            }
        }
    }
  NS_FATAL_ERROR ("no more data radio bearer ids available");
  return 0;
}

Ptr<LteDataRadioNistBearerInfo>
NistUeManager::GetDataRadioNistBearerInfo (uint8_t drbid)
{
  NS_LOG_FUNCTION (this << (uint32_t) drbid);
  NS_ASSERT (0 != drbid);
  std::map<uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator it = m_drbMap.find (drbid);
  NS_ABORT_IF (it == m_drbMap.end ());
  return it->second;
}


void
NistUeManager::RemoveDataRadioNistBearerInfo (uint8_t drbid)
{
  NS_LOG_FUNCTION (this << (uint32_t) drbid);
  std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator it = m_drbMap.find (drbid);
  NS_ASSERT_MSG (it != m_drbMap.end (), "request to remove radio bearer with unknown drbid " << drbid);
  m_drbMap.erase (it);
}


NistLteRrcSap::NistRrcConnectionReconfiguration
NistUeManager::BuildNistRrcConnectionReconfiguration ()
{
  NistLteRrcSap::NistRrcConnectionReconfiguration msg;
  msg.rrcTransactionIdentifier = GetNewRrcTransactionIdentifier ();
  msg.haveNistRadioResourceConfigDedicated = true;
  msg.radioResourceConfigDedicated = BuildNistRadioResourceConfigDedicated ();
  msg.haveNistMobilityControlInfo = false;
  msg.haveNistMeasConfig = true;
  msg.haveSlCommConfig = false;
  msg.measConfig = m_rrc->m_ueNistMeasConfig;

  return msg;
}

NistLteRrcSap::NistRadioResourceConfigDedicated
NistUeManager::BuildNistRadioResourceConfigDedicated ()
{
  NistLteRrcSap::NistRadioResourceConfigDedicated rrcd;

  if (m_srb1 != 0)
    {
      NistLteRrcSap::NistSrbToAddMod stam;
      stam.srbIdentity = m_srb1->m_srbIdentity;
      stam.logicalChannelConfig = m_srb1->m_logicalChannelConfig;
      rrcd.srbToAddModList.push_back (stam);
    }

  for (std::map <uint8_t, Ptr<LteDataRadioNistBearerInfo> >::iterator it = m_drbMap.begin ();
       it != m_drbMap.end ();
       ++it)
    {
      NistLteRrcSap::NistDrbToAddMod dtam;
      dtam.epsBearerIdentity = it->second->m_epsBearerIdentity;
      dtam.drbIdentity = it->second->m_drbIdentity;
      dtam.rlcConfig = it->second->m_rlcConfig;
      dtam.logicalChannelIdentity = it->second->m_logicalChannelIdentity;
      dtam.logicalChannelConfig = it->second->m_logicalChannelConfig;
      rrcd.drbToAddModList.push_back (dtam);
    }

  rrcd.haveNistPhysicalConfigDedicated = true;
  rrcd.physicalConfigDedicated = m_physicalConfigDedicated;
  return rrcd;
}

uint8_t 
NistUeManager::GetNewRrcTransactionIdentifier ()
{
  return ++m_lastRrcTransactionIdentifier;
}

uint8_t 
NistUeManager::Lcid2Drbid (uint8_t lcid)
{
  NS_ASSERT (lcid > 2);
  return lcid - 2;
}

uint8_t 
NistUeManager::Drbid2Lcid (uint8_t drbid)
{
  return drbid + 2;
}
uint8_t 
NistUeManager::Lcid2Bid (uint8_t lcid)
{
  NS_ASSERT (lcid > 2);
  return lcid - 2;
}

uint8_t 
NistUeManager::Bid2Lcid (uint8_t bid)
{
  return bid + 2;
}

uint8_t 
NistUeManager::Drbid2Bid (uint8_t drbid)
{
  return drbid;
}

uint8_t 
NistUeManager::Bid2Drbid (uint8_t bid)
{
  return bid;
}


void 
NistUeManager::SwitchToState (State newState)
{
  NS_LOG_FUNCTION (this << ToString (newState));
  State oldState = m_state;
  m_state = newState;
  NS_LOG_INFO (this << " IMSI " << m_imsi << " RNTI " << m_rnti << " NistUeManager "
                    << ToString (oldState) << " --> " << ToString (newState));
  m_stateTransitionTrace (m_imsi, m_rrc->m_cellId, m_rnti, oldState, newState);

  switch (newState)
    {
    case INITIAL_RANDOM_ACCESS:
    case HANDOVER_JOINING:
      NS_FATAL_ERROR ("cannot switch to an initial state");
      break;

    case CONNECTION_SETUP:
      break;

    case CONNECTED_NORMALLY:
      {
        if (m_pendingNistRrcConnectionReconfiguration == true)
          {
            ScheduleNistRrcConnectionReconfiguration ();
          }
      }
      break;

    case CONNECTION_RECONFIGURATION:
      break;

    case CONNECTION_REESTABLISHMENT:
      break;

    case HANDOVER_LEAVING:
      break;

    default:
      break;
    }
}



///////////////////////////////////////////
// eNB RRC methods
///////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (NistLteEnbRrc);

NistLteEnbRrc::NistLteEnbRrc ()
  : m_x2SapProvider (0),
    m_cmacSapProvider (0),
    m_handoverManagementSapProvider (0),
    m_anrSapProvider (0),
    m_ffrRrcSapProvider (0),
    m_rrcSapUser (0),
    m_macSapProvider (0),
    m_s1SapProvider (0),
    m_cphySapProvider (0),
    m_configured (false),
    m_lastAllocatedRnti (0),
    m_srsCurrentPeriodicityId (0),
    m_lastAllocatedConfigurationIndex (0),
    m_reconfigureUes (false),
    m_sidelinkConfiguration (0)
{
  NS_LOG_FUNCTION (this);
  m_cmacSapUser = new NistEnbRrcMemberLteEnbCmacSapUser (this);
  m_handoverManagementSapUser = new MemberNistLteHandoverManagementSapUser<NistLteEnbRrc> (this);
  m_anrSapUser = new MemberNistLteAnrSapUser<NistLteEnbRrc> (this);
  m_ffrRrcSapUser = new MemberNistLteFfrRrcSapUser<NistLteEnbRrc> (this);
  m_rrcSapProvider = new NistMemberLteEnbRrcSapProvider<NistLteEnbRrc> (this);
  m_x2SapUser = new NistEpcX2SpecificNistEpcX2SapUser<NistLteEnbRrc> (this);
  m_s1SapUser = new MemberNistEpcEnbS1SapUser<NistLteEnbRrc> (this);
  m_cphySapUser = new MemberNistLteEnbCphySapUser<NistLteEnbRrc> (this);
}


NistLteEnbRrc::~NistLteEnbRrc ()
{
  NS_LOG_FUNCTION (this);
}


void
NistLteEnbRrc::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_ueMap.clear ();
  delete m_cmacSapUser;
  delete m_handoverManagementSapUser;
  delete m_anrSapUser;
  delete m_ffrRrcSapUser;
  delete m_rrcSapProvider;
  delete m_x2SapUser;
  delete m_s1SapUser;
  delete m_cphySapUser;
}

TypeId
NistLteEnbRrc::GetTypeId (void)
{
  NS_LOG_FUNCTION ("NistLteEnbRrc::GetTypeId");
  static TypeId tid = TypeId ("ns3::NistLteEnbRrc")
    .SetParent<Object> ()
    .AddConstructor<NistLteEnbRrc> ()
    .AddAttribute ("UeMap", "List of NistUeManager by C-RNTI.",
                   ObjectMapValue (),
                   MakeObjectMapAccessor (&NistLteEnbRrc::m_ueMap),
                   MakeObjectMapChecker<NistUeManager> ())
    .AddAttribute ("DefaultTransmissionMode",
                   "The default UEs' transmission mode (0: SISO)",
                   UintegerValue (0),  // default tx-mode
                   MakeUintegerAccessor (&NistLteEnbRrc::m_defaultTransmissionMode),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("NistEpsBearerToRlcMapping", 
                   "Specify which type of RLC will be used for each type of EPS bearer. ",
                   EnumValue (RLC_SM_ALWAYS),
                   MakeEnumAccessor (&NistLteEnbRrc::m_epsBearerToRlcMapping),
                   MakeEnumChecker (RLC_SM_ALWAYS, "RlcSmAlways",
                                    RLC_UM_ALWAYS, "RlcUmAlways",
                                    RLC_AM_ALWAYS, "RlcAmAlways",
                                    PER_BASED,     "PacketErrorRateBased"))
    .AddAttribute ("NistSystemInformationPeriodicity",
                   "The interval for sending system information (Time value)",
                   TimeValue (MilliSeconds (80)),
                   MakeTimeAccessor (&NistLteEnbRrc::m_systemInformationPeriodicity),
                   MakeTimeChecker ())

    // SRS related attributes
    .AddAttribute ("SrsPeriodicity",
                   "The SRS periodicity in milliseconds",
                   UintegerValue (40),
                   MakeUintegerAccessor (&NistLteEnbRrc::SetSrsPeriodicity, 
                                         &NistLteEnbRrc::GetSrsPeriodicity),
                   MakeUintegerChecker<uint32_t> ())

    // Timeout related attributes
    .AddAttribute ("ConnectionRequestTimeoutDuration",
                   "After a RA attempt, if no RRC CONNECTION REQUEST is "
                   "received before this time, the UE context is destroyed. "
                   "Must account for reception of RAR and transmission of "
                   "RRC CONNECTION REQUEST over UL GRANT.",
                   TimeValue (MilliSeconds (15)),
                   MakeTimeAccessor (&NistLteEnbRrc::m_connectionRequestTimeoutDuration),
                   MakeTimeChecker ())
    .AddAttribute ("ConnectionSetupTimeoutDuration",
                   "After accepting connection request, if no RRC CONNECTION "
                   "SETUP COMPLETE is received before this time, the UE "
                   "context is destroyed. Must account for the UE's reception "
                   "of RRC CONNECTION SETUP and transmission of RRC CONNECTION "
                   "SETUP COMPLETE.",
                   TimeValue (MilliSeconds (150)),
                   MakeTimeAccessor (&NistLteEnbRrc::m_connectionSetupTimeoutDuration),
                   MakeTimeChecker ())
    .AddAttribute ("ConnectionRejectedTimeoutDuration",
                   "Time to wait between sending a RRC CONNECTION REJECT and "
                   "destroying the UE context",
                   TimeValue (MilliSeconds (30)),
                   MakeTimeAccessor (&NistLteEnbRrc::m_connectionRejectedTimeoutDuration),
                   MakeTimeChecker ())
    .AddAttribute ("HandoverJoiningTimeoutDuration",
                   "After accepting a handover request, if no RRC CONNECTION "
                   "RECONFIGURATION COMPLETE is received before this time, the "
                   "UE context is destroyed. Must account for reception of "
                   "X2 HO REQ ACK by source eNB, transmission of the Handover "
                   "Command, non-contention-based random access and reception "
                   "of the RRC CONNECTION RECONFIGURATION COMPLETE message.",
                   TimeValue (MilliSeconds (200)),
                   MakeTimeAccessor (&NistLteEnbRrc::m_handoverJoiningTimeoutDuration),
                   MakeTimeChecker ())
    .AddAttribute ("HandoverLeavingTimeoutDuration",
                   "After issuing a Handover Command, if neither RRC "
                   "CONNECTION RE-ESTABLISHMENT nor X2 UE Context Release has "
                   "been previously received, the UE context is destroyed.",
                   TimeValue (MilliSeconds (500)),
                   MakeTimeAccessor (&NistLteEnbRrc::m_handoverLeavingTimeoutDuration),
                   MakeTimeChecker ())

    // Cell selection related attribute
    .AddAttribute ("QRxLevMin",
                   "One of information transmitted within the SIB1 message, "
                   "indicating the required minimum RSRP level that any UE must "
                   "receive from this cell before it is allowed to camp to this "
                   "cell. The default value -70 corresponds to -140 dBm and is "
                   "the lowest possible value as defined by Section 6.3.4 of "
                   "3GPP TS 36.133. This restriction, however, only applies to "
                   "initial cell selection and EPC-enabled simulation.",
                   TypeId::ATTR_GET | TypeId::ATTR_CONSTRUCT,
                   IntegerValue (-70),
                   MakeIntegerAccessor (&NistLteEnbRrc::m_qRxLevMin),
                   MakeIntegerChecker<int8_t> (-70, -22))

    // Handover related attributes
    .AddAttribute ("AdmitHandoverRequest",
                   "Whether to admit an X2 handover request from another eNB",
                   BooleanValue (true),
                   MakeBooleanAccessor (&NistLteEnbRrc::m_admitHandoverRequest),
                   MakeBooleanChecker ())
    .AddAttribute ("AdmitNistRrcConnectionRequest",
                   "Whether to admit a connection request from a UE",
                   BooleanValue (true),
                   MakeBooleanAccessor (&NistLteEnbRrc::m_admitNistRrcConnectionRequest),
                   MakeBooleanChecker ())

    // UE measurements related attributes
    .AddAttribute ("RsrpFilterCoefficient",
                   "Determines the strength of smoothing effect induced by "
                   "layer 3 filtering of RSRP in all attached UE; "
                   "if set to 0, no layer 3 filtering is applicable",
                   // i.e. the variable k in 3GPP TS 36.331 section 5.5.3.2
                   UintegerValue (4),
                   MakeUintegerAccessor (&NistLteEnbRrc::m_rsrpFilterCoefficient),
                   MakeUintegerChecker<uint8_t> (0))
    .AddAttribute ("RsrqFilterCoefficient",
                   "Determines the strength of smoothing effect induced by "
                   "layer 3 filtering of RSRQ in all attached UE; "
                   "if set to 0, no layer 3 filtering is applicable",
                   // i.e. the variable k in 3GPP TS 36.331 section 5.5.3.2
                   UintegerValue (4),
                   MakeUintegerAccessor (&NistLteEnbRrc::m_rsrqFilterCoefficient),
                   MakeUintegerChecker<uint8_t> (0))
    //Add accessor to sidelink configuration
    .AddAttribute ("SidelinkConfiguration",
                   "The sidelink configuration associated to this NistLtePhy",
                   PointerValue (),
                   MakePointerAccessor (&NistLteEnbRrc::m_sidelinkConfiguration),
                   MakePointerChecker <LteEnbRrcSl> ())
    // Trace sources
    .AddTraceSource ("NewUeContext",
                     "Fired upon creation of a new UE context.",
                     MakeTraceSourceAccessor (&NistLteEnbRrc::m_newUeContextTrace),
                     "ns3::NistLteEnbRrc::NewUeContextTracedCallback")
    .AddTraceSource ("ConnectionEstablished",
                     "Fired upon successful RRC connection establishment.",
                     MakeTraceSourceAccessor (&NistLteEnbRrc::m_connectionEstablishedTrace),
                     "ns3::NistLteEnbRrc::ConnectionHandoverTracedCallback")
    .AddTraceSource ("ConnectionReconfiguration",
                     "trace fired upon RRC connection reconfiguration",
                     MakeTraceSourceAccessor (&NistLteEnbRrc::m_connectionReconfigurationTrace),
                     "ns3::NistLteEnbRrc::ConnectionHandoverTracedCallback")
    .AddTraceSource ("HandoverStart",
                     "trace fired upon start of a handover procedure",
                     MakeTraceSourceAccessor (&NistLteEnbRrc::m_handoverStartTrace),
                     "ns3::NistLteEnbRrc::HandoverStartTracedCallback")
    .AddTraceSource ("HandoverEndOk",
                     "trace fired upon successful termination of a handover procedure",
                     MakeTraceSourceAccessor (&NistLteEnbRrc::m_handoverEndOkTrace),
                     "ns3::NistLteEnbRrc::ConnectionHandoverTracedCallback")
    .AddTraceSource ("RecvNistMeasurementReport",
                     "trace fired when measurement report is received",
                     MakeTraceSourceAccessor (&NistLteEnbRrc::m_recvNistMeasurementReportTrace),
                     "ns3::NistLteEnbRrc::ReceiveReportTracedCallback")
  ;
  return tid;
}

void
NistLteEnbRrc::SetNistEpcX2SapProvider (NistEpcX2SapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_x2SapProvider = s;
}

NistEpcX2SapUser*
NistLteEnbRrc::GetNistEpcX2SapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_x2SapUser;
}

void
NistLteEnbRrc::SetNistLteEnbCmacSapProvider (NistLteEnbCmacSapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_cmacSapProvider = s;
}

NistLteEnbCmacSapUser*
NistLteEnbRrc::GetNistLteEnbCmacSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_cmacSapUser;
}

void
NistLteEnbRrc::SetNistLteHandoverManagementSapProvider (NistLteHandoverManagementSapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_handoverManagementSapProvider = s;
}

NistLteHandoverManagementSapUser*
NistLteEnbRrc::GetNistLteHandoverManagementSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_handoverManagementSapUser;
}

void
NistLteEnbRrc::SetNistLteAnrSapProvider (NistLteAnrSapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_anrSapProvider = s;
}

NistLteAnrSapUser*
NistLteEnbRrc::GetNistLteAnrSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_anrSapUser;
}

void
NistLteEnbRrc::SetNistLteFfrRrcSapProvider (NistLteFfrRrcSapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_ffrRrcSapProvider = s;
}

NistLteFfrRrcSapUser*
NistLteEnbRrc::GetNistLteFfrRrcSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_ffrRrcSapUser;
}

void
NistLteEnbRrc::SetNistLteEnbRrcSapUser (NistLteEnbRrcSapUser * s)
{
  NS_LOG_FUNCTION (this << s);
  m_rrcSapUser = s;
}

NistLteEnbRrcSapProvider*
NistLteEnbRrc::GetNistLteEnbRrcSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_rrcSapProvider;
}

void
NistLteEnbRrc::SetNistLteMacSapProvider (NistLteMacSapProvider * s)
{
  NS_LOG_FUNCTION (this);
  m_macSapProvider = s;
}

void 
NistLteEnbRrc::SetS1SapProvider (NistEpcEnbS1SapProvider * s)
{
  m_s1SapProvider = s;
}


NistEpcEnbS1SapUser* 
NistLteEnbRrc::GetS1SapUser ()
{
  return m_s1SapUser;
}

void
NistLteEnbRrc::SetNistLteEnbCphySapProvider (NistLteEnbCphySapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_cphySapProvider = s;
}

NistLteEnbCphySapUser*
NistLteEnbRrc::GetNistLteEnbCphySapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_cphySapUser;
}

bool
NistLteEnbRrc::HasNistUeManager (uint16_t rnti) const
{
  NS_LOG_FUNCTION (this << (uint32_t) rnti);
  std::map<uint16_t, Ptr<NistUeManager> >::const_iterator it = m_ueMap.find (rnti);
  return (it != m_ueMap.end ());
}

Ptr<NistUeManager>
NistLteEnbRrc::GetNistUeManager (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << (uint32_t) rnti);
  NS_ASSERT (0 != rnti);
  std::map<uint16_t, Ptr<NistUeManager> >::iterator it = m_ueMap.find (rnti);
  NS_ASSERT_MSG (it != m_ueMap.end (), "RNTI " << rnti << " not found in eNB with cellId " << m_cellId);
  return it->second;
}

uint8_t
NistLteEnbRrc::AddUeMeasReportConfig (NistLteRrcSap::NistReportConfigEutra config)
{
  NS_LOG_FUNCTION (this);

  // SANITY CHECK

  NS_ASSERT_MSG (m_ueNistMeasConfig.measIdToAddModList.size () == m_ueNistMeasConfig.reportConfigToAddModList.size (),
                 "Measurement identities and reporting configuration should not have different quantity");

  if (Simulator::Now () != Seconds (0))
    {
      NS_FATAL_ERROR ("AddUeMeasReportConfig may not be called after the simulation has run");
    }

  // INPUT VALIDATION

  switch (config.triggerQuantity)
    {
    case NistLteRrcSap::NistReportConfigEutra::RSRP:
      if ((config.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A5)
          && (config.threshold2.choice != NistLteRrcSap::NistThresholdEutra::THRESHOLD_RSRP))
        {
          NS_FATAL_ERROR ("The given triggerQuantity (RSRP) does not match with the given threshold2.choice");
        }

      if (((config.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A1)
           || (config.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A2)
           || (config.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A4)
           || (config.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A5))
          && (config.threshold1.choice != NistLteRrcSap::NistThresholdEutra::THRESHOLD_RSRP))
        {
          NS_FATAL_ERROR ("The given triggerQuantity (RSRP) does not match with the given threshold1.choice");
        }
      break;

    case NistLteRrcSap::NistReportConfigEutra::RSRQ:
      if ((config.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A5)
          && (config.threshold2.choice != NistLteRrcSap::NistThresholdEutra::THRESHOLD_RSRQ))
        {
          NS_FATAL_ERROR ("The given triggerQuantity (RSRQ) does not match with the given threshold2.choice");
        }

      if (((config.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A1)
           || (config.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A2)
           || (config.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A4)
           || (config.eventId == NistLteRrcSap::NistReportConfigEutra::EVENT_A5))
          && (config.threshold1.choice != NistLteRrcSap::NistThresholdEutra::THRESHOLD_RSRQ))
        {
          NS_FATAL_ERROR ("The given triggerQuantity (RSRQ) does not match with the given threshold1.choice");
        }
      break;

    default:
      NS_FATAL_ERROR ("unsupported triggerQuantity");
      break;
    }

  if (config.purpose != NistLteRrcSap::NistReportConfigEutra::REPORT_STRONGEST_CELLS)
    {
      NS_FATAL_ERROR ("Only REPORT_STRONGEST_CELLS purpose is supported");
    }

  if (config.reportQuantity != NistLteRrcSap::NistReportConfigEutra::BOTH)
    {
      NS_LOG_WARN ("reportQuantity = BOTH will be used instead of the given reportQuantity");
    }

  uint8_t nextId = m_ueNistMeasConfig.reportConfigToAddModList.size () + 1;

  // create the reporting configuration
  NistLteRrcSap::NistReportConfigToAddMod reportConfig;
  reportConfig.reportConfigId = nextId;
  reportConfig.reportConfigEutra = config;

  // create the measurement identity
  NistLteRrcSap::NistMeasIdToAddMod measId;
  measId.measId = nextId;
  measId.measObjectId = 1;
  measId.reportConfigId = nextId;

  // add both to the list of UE measurement configuration
  m_ueNistMeasConfig.reportConfigToAddModList.push_back (reportConfig);
  m_ueNistMeasConfig.measIdToAddModList.push_back (measId);

  return nextId;
}

void
NistLteEnbRrc::ConfigureCell (uint8_t ulBandwidth, uint8_t dlBandwidth,
                          uint16_t ulEarfcn, uint16_t dlEarfcn, uint16_t cellId)
{
  NS_LOG_FUNCTION (this << (uint16_t) ulBandwidth << (uint16_t) dlBandwidth
                        << ulEarfcn << dlEarfcn << cellId);
  NS_ASSERT (!m_configured);
  m_cmacSapProvider->ConfigureMac (ulBandwidth, dlBandwidth);
  m_cphySapProvider->SetBandwidth (ulBandwidth, dlBandwidth);
  m_cphySapProvider->SetEarfcn (ulEarfcn, dlEarfcn);
  m_dlEarfcn = dlEarfcn;
  m_ulEarfcn = ulEarfcn;
  m_dlBandwidth = dlBandwidth;
  m_ulBandwidth = ulBandwidth;
  m_cellId = cellId;
  m_cphySapProvider->SetCellId (cellId);
  m_ffrRrcSapProvider->SetCellId (cellId);
  m_ffrRrcSapProvider->SetBandwidth(ulBandwidth, dlBandwidth);

  /*
   * Initializing the list of UE measurement configuration (m_ueNistMeasConfig).
   * Only intra-frequency measurements are supported, so only one measurement
   * object is created.
   */

  NistLteRrcSap::NistMeasObjectToAddMod measObject;
  measObject.measObjectId = 1;
  measObject.measObjectEutra.carrierFreq = m_dlEarfcn;
  measObject.measObjectEutra.allowedMeasBandwidth = m_dlBandwidth;
  measObject.measObjectEutra.presenceAntennaPort1 = false;
  measObject.measObjectEutra.neighCellConfig = 0;
  measObject.measObjectEutra.offsetFreq = 0;
  measObject.measObjectEutra.haveCellForWhichToReportCGI = false;

  m_ueNistMeasConfig.measObjectToAddModList.push_back (measObject);
  m_ueNistMeasConfig.haveNistQuantityConfig = true;
  m_ueNistMeasConfig.quantityConfig.filterCoefficientRSRP = m_rsrpFilterCoefficient;
  m_ueNistMeasConfig.quantityConfig.filterCoefficientRSRQ = m_rsrqFilterCoefficient;
  m_ueNistMeasConfig.haveNistMeasGapConfig = false;
  m_ueNistMeasConfig.haveSmeasure = false;
  m_ueNistMeasConfig.haveNistSpeedStatePars = false;

  // Enabling MIB transmission
  NistLteRrcSap::NistMasterInformationBlock mib;
  mib.dlBandwidth = m_dlBandwidth;
  m_cphySapProvider->SetNistMasterInformationBlock (mib);

  // Enabling SIB1 transmission with default values
  m_sib1.cellAccessRelatedInfo.cellIdentity = cellId;
  m_sib1.cellAccessRelatedInfo.csgIndication = false;
  m_sib1.cellAccessRelatedInfo.csgIdentity = 0;
  m_sib1.cellAccessRelatedInfo.plmnIdentityInfo.plmnIdentity = 0; // not used
  m_sib1.cellSelectionInfo.qQualMin = -34; // not used, set as minimum value
  m_sib1.cellSelectionInfo.qRxLevMin = m_qRxLevMin; // set as minimum value
  m_cphySapProvider->SetNistSystemInformationBlockType1 (m_sib1);

  /*
   * Enabling transmission of other SIB. The first time System Information is
   * transmitted is arbitrarily assumed to be at +0.016s, and then it will be
   * regularly transmitted every 80 ms by default (set the
   * NistSystemInformationPeriodicity attribute to configure this).
   */
  Simulator::Schedule (MilliSeconds (16), &NistLteEnbRrc::SendNistSystemInformation, this);

  m_configured = true;

}


void
NistLteEnbRrc::SetCellId (uint16_t cellId)
{
  m_cellId = cellId;

  // update SIB1 too
  m_sib1.cellAccessRelatedInfo.cellIdentity = cellId;
  m_cphySapProvider->SetNistSystemInformationBlockType1 (m_sib1);
}

bool
NistLteEnbRrc::SendData (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  NistEpsBearerTag tag;
  bool found = packet->RemovePacketTag (tag);
  NS_ASSERT_MSG (found, "no NistEpsBearerTag found in packet to be sent");
  Ptr<NistUeManager> ueManager = GetNistUeManager (tag.GetRnti ());
  ueManager->SendData (tag.GetBid (), packet);

  return true;
}

void 
NistLteEnbRrc::SetForwardUpCallback (Callback <void, Ptr<Packet> > cb)
{
  m_forwardUpCallback = cb;
}

void
NistLteEnbRrc::ConnectionRequestTimeout (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  NS_ASSERT_MSG (GetNistUeManager (rnti)->GetState () == NistUeManager::INITIAL_RANDOM_ACCESS,
                 "ConnectionRequestTimeout in unexpected state " << ToString (GetNistUeManager (rnti)->GetState ()));
  RemoveUe (rnti);
}

void
NistLteEnbRrc::ConnectionSetupTimeout (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  NS_ASSERT_MSG (GetNistUeManager (rnti)->GetState () == NistUeManager::CONNECTION_SETUP,
                 "ConnectionSetupTimeout in unexpected state " << ToString (GetNistUeManager (rnti)->GetState ()));
  RemoveUe (rnti);
}

void
NistLteEnbRrc::ConnectionRejectedTimeout (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  NS_ASSERT_MSG (GetNistUeManager (rnti)->GetState () == NistUeManager::CONNECTION_REJECTED,
                 "ConnectionRejectedTimeout in unexpected state " << ToString (GetNistUeManager (rnti)->GetState ()));
  RemoveUe (rnti);
}

void
NistLteEnbRrc::HandoverJoiningTimeout (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  NS_ASSERT_MSG (GetNistUeManager (rnti)->GetState () == NistUeManager::HANDOVER_JOINING,
                 "HandoverJoiningTimeout in unexpected state " << ToString (GetNistUeManager (rnti)->GetState ()));
  RemoveUe (rnti);
}

void
NistLteEnbRrc::HandoverLeavingTimeout (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  NS_ASSERT_MSG (GetNistUeManager (rnti)->GetState () == NistUeManager::HANDOVER_LEAVING,
                 "HandoverLeavingTimeout in unexpected state " << ToString (GetNistUeManager (rnti)->GetState ()));
  RemoveUe (rnti);
}

void
NistLteEnbRrc::SendHandoverRequest (uint16_t rnti, uint16_t cellId)
{
  NS_LOG_FUNCTION (this << rnti << cellId);
  NS_LOG_LOGIC ("Request to send HANDOVER REQUEST");
  NS_ASSERT (m_configured);

  Ptr<NistUeManager> ueManager = GetNistUeManager (rnti);
  ueManager->PrepareHandover (cellId);
 
}

void 
NistLteEnbRrc::DoCompleteSetupUe (uint16_t rnti, NistLteEnbRrcSapProvider::CompleteNistSetupUeParameters params)
{
  NS_LOG_FUNCTION (this << rnti);
  GetNistUeManager (rnti)->CompleteSetupUe (params);
}

void
NistLteEnbRrc::DoRecvNistRrcConnectionRequest (uint16_t rnti, NistLteRrcSap::NistRrcConnectionRequest msg)
{
  NS_LOG_FUNCTION (this << rnti);
  GetNistUeManager (rnti)->RecvNistRrcConnectionRequest (msg);
}

void
NistLteEnbRrc::DoRecvNistRrcConnectionSetupCompleted (uint16_t rnti, NistLteRrcSap::NistRrcConnectionSetupCompleted msg)
{
  NS_LOG_FUNCTION (this << rnti);
  GetNistUeManager (rnti)->RecvNistRrcConnectionSetupCompleted (msg);
}

void
NistLteEnbRrc::DoRecvNistRrcConnectionReconfigurationCompleted (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReconfigurationCompleted msg)
{
  NS_LOG_FUNCTION (this << rnti);
  GetNistUeManager (rnti)->RecvNistRrcConnectionReconfigurationCompleted (msg);
}

void 
NistLteEnbRrc::DoRecvNistRrcConnectionReestablishmentRequest (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReestablishmentRequest msg)
{
  NS_LOG_FUNCTION (this << rnti);
  GetNistUeManager (rnti)->RecvNistRrcConnectionReestablishmentRequest (msg);
}

void 
NistLteEnbRrc::DoRecvNistRrcConnectionReestablishmentComplete (uint16_t rnti, NistLteRrcSap::NistRrcConnectionReestablishmentComplete msg)
{
  NS_LOG_FUNCTION (this << rnti);
  GetNistUeManager (rnti)->RecvNistRrcConnectionReestablishmentComplete (msg);
}

void 
NistLteEnbRrc::DoRecvNistMeasurementReport (uint16_t rnti, NistLteRrcSap::NistMeasurementReport msg)
{
  NS_LOG_FUNCTION (this << rnti);
  GetNistUeManager (rnti)->RecvNistMeasurementReport (msg);
}

void 
NistLteEnbRrc::DoRecvSidelinkUeInformation (uint16_t rnti, NistLteRrcSap::SidelinkUeInformation msg)
{
  NS_LOG_FUNCTION (this << rnti);
  //TODO
  GetNistUeManager (rnti)->RecvSidelinkUeInformation (msg);
}

  
void 
NistLteEnbRrc::DoDataRadioBearerSetupRequest (NistEpcEnbS1SapUser::NistDataRadioBearerSetupRequestParameters request)
{
  Ptr<NistUeManager> ueManager = GetNistUeManager (request.rnti);
  ueManager->SetupDataRadioBearer (request.bearer, request.bearerId, request.gtpTeid, request.transportLayerAddress);
}

void 
NistLteEnbRrc::DoPathSwitchRequestAcknowledge (NistEpcEnbS1SapUser::NistPathSwitchRequestAcknowledgeParameters params)
{
  Ptr<NistUeManager> ueManager = GetNistUeManager (params.rnti);
  ueManager->SendUeContextRelease ();
}

void
NistLteEnbRrc::DoRecvHandoverRequest (NistEpcX2SapUser::NistHandoverRequestParams req)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Recv X2 message: HANDOVER REQUEST");

  NS_LOG_LOGIC ("oldEnbUeX2apId = " << req.oldEnbUeX2apId);
  NS_LOG_LOGIC ("sourceCellId = " << req.sourceCellId);
  NS_LOG_LOGIC ("targetCellId = " << req.targetCellId);
  NS_LOG_LOGIC ("mmeUeS1apId = " << req.mmeUeS1apId);

  NS_ASSERT (req.targetCellId == m_cellId);

  if (m_admitHandoverRequest == false)
    {
      NS_LOG_INFO ("rejecting handover request from cellId " << req.sourceCellId);
      NistEpcX2Sap::NistHandoverPreparationFailureParams res;
      res.oldEnbUeX2apId =  req.oldEnbUeX2apId;
      res.sourceCellId = req.sourceCellId;
      res.targetCellId = req.targetCellId;
      res.cause = 0;
      res.criticalityDiagnostics = 0;
      m_x2SapProvider->SendHandoverPreparationFailure (res);
      return;
    }

  uint16_t rnti = AddUe (NistUeManager::HANDOVER_JOINING);
  NistLteEnbCmacSapProvider::NistAllocateNcRaPreambleReturnValue anrcrv = m_cmacSapProvider->AllocateNcRaPreamble (rnti);
  if (anrcrv.valid == false)
    {
      NS_LOG_INFO (this << " failed to allocate a preamble for non-contention based RA => cannot accept HO");
      RemoveUe (rnti);
      NS_FATAL_ERROR ("should trigger HO Preparation Failure, but it is not implemented");
      return;
    }

  Ptr<NistUeManager> ueManager = GetNistUeManager (rnti);
  ueManager->SetSource (req.sourceCellId, req.oldEnbUeX2apId);
  ueManager->SetImsi (req.mmeUeS1apId);

  NistEpcX2SapProvider::NistHandoverRequestAckParams ackParams;
  ackParams.oldEnbUeX2apId = req.oldEnbUeX2apId;
  ackParams.newEnbUeX2apId = rnti;
  ackParams.sourceCellId = req.sourceCellId;
  ackParams.targetCellId = req.targetCellId;

  for (std::vector <NistEpcX2Sap::NistErabToBeSetupItem>::iterator it = req.bearers.begin ();
       it != req.bearers.end ();
       ++it)
    {
      ueManager->SetupDataRadioBearer (it->erabLevelQosParameters, it->erabId, it->gtpTeid, it->transportLayerAddress);
      NistEpcX2Sap::NistErabAdmittedItem i;
      i.erabId = it->erabId;
      ackParams.admittedBearers.push_back (i);
    }

  NistLteRrcSap::NistRrcConnectionReconfiguration handoverCommand = ueManager->GetNistRrcConnectionReconfigurationForHandover ();
  handoverCommand.haveNistMobilityControlInfo = true;
  handoverCommand.mobilityControlInfo.targetPhysCellId = m_cellId;
  handoverCommand.mobilityControlInfo.haveCarrierFreq = true;
  handoverCommand.mobilityControlInfo.carrierFreq.dlCarrierFreq = m_dlEarfcn;
  handoverCommand.mobilityControlInfo.carrierFreq.ulCarrierFreq = m_ulEarfcn;
  handoverCommand.mobilityControlInfo.haveCarrierBandwidth = true;
  handoverCommand.mobilityControlInfo.carrierBandwidth.dlBandwidth = m_dlBandwidth;
  handoverCommand.mobilityControlInfo.carrierBandwidth.ulBandwidth = m_ulBandwidth;
  handoverCommand.mobilityControlInfo.newUeIdentity = rnti;
  handoverCommand.mobilityControlInfo.haveNistRachConfigDedicated = true;
  handoverCommand.mobilityControlInfo.rachConfigDedicated.raPreambleIndex = anrcrv.raPreambleId;
  handoverCommand.mobilityControlInfo.rachConfigDedicated.raPrachMaskIndex = anrcrv.raPrachMaskIndex;

  NistLteEnbCmacSapProvider::NistRachConfig rc = m_cmacSapProvider->GetNistRachConfig ();
  handoverCommand.mobilityControlInfo.radioResourceConfigCommon.rachConfigCommon.preambleInfo.numberOfRaPreambles = rc.numberOfRaPreambles;
  handoverCommand.mobilityControlInfo.radioResourceConfigCommon.rachConfigCommon.raSupervisionInfo.preambleTransMax = rc.preambleTransMax;
  handoverCommand.mobilityControlInfo.radioResourceConfigCommon.rachConfigCommon.raSupervisionInfo.raResponseWindowSize = rc.raResponseWindowSize;

  Ptr<Packet> encodedHandoverCommand = m_rrcSapUser->EncodeHandoverCommand (handoverCommand);

  ackParams.rrcContext = encodedHandoverCommand;

  NS_LOG_LOGIC ("Send X2 message: HANDOVER REQUEST ACK");

  NS_LOG_LOGIC ("oldEnbUeX2apId = " << ackParams.oldEnbUeX2apId);
  NS_LOG_LOGIC ("newEnbUeX2apId = " << ackParams.newEnbUeX2apId);
  NS_LOG_LOGIC ("sourceCellId = " << ackParams.sourceCellId);
  NS_LOG_LOGIC ("targetCellId = " << ackParams.targetCellId);

  m_x2SapProvider->SendHandoverRequestAck (ackParams);
}

void
NistLteEnbRrc::DoRecvHandoverRequestAck (NistEpcX2SapUser::NistHandoverRequestAckParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Recv X2 message: HANDOVER REQUEST ACK");

  NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
  NS_LOG_LOGIC ("newEnbUeX2apId = " << params.newEnbUeX2apId);
  NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
  NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);

  uint16_t rnti = params.oldEnbUeX2apId;
  Ptr<NistUeManager> ueManager = GetNistUeManager (rnti);
  ueManager->RecvHandoverRequestAck (params);
}

void
NistLteEnbRrc::DoRecvHandoverPreparationFailure (NistEpcX2SapUser::NistHandoverPreparationFailureParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Recv X2 message: HANDOVER PREPARATION FAILURE");

  NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
  NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
  NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
  NS_LOG_LOGIC ("cause = " << params.cause);
  NS_LOG_LOGIC ("criticalityDiagnostics = " << params.criticalityDiagnostics);

  uint16_t rnti = params.oldEnbUeX2apId;
  Ptr<NistUeManager> ueManager = GetNistUeManager (rnti);
  ueManager->RecvHandoverPreparationFailure (params.targetCellId);
}

void
NistLteEnbRrc::DoRecvSnNistStatusTransfer (NistEpcX2SapUser::NistSnNistStatusTransferParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Recv X2 message: SN STATUS TRANSFER");

  NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
  NS_LOG_LOGIC ("newEnbUeX2apId = " << params.newEnbUeX2apId);
  NS_LOG_LOGIC ("erabsSubjectToNistStatusTransferList size = " << params.erabsSubjectToNistStatusTransferList.size ());

  uint16_t rnti = params.newEnbUeX2apId;
  Ptr<NistUeManager> ueManager = GetNistUeManager (rnti);
  ueManager->RecvSnNistStatusTransfer (params);
}

void
NistLteEnbRrc::DoRecvUeContextRelease (NistEpcX2SapUser::NistUeContextReleaseParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Recv X2 message: UE CONTEXT RELEASE");

  NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
  NS_LOG_LOGIC ("newEnbUeX2apId = " << params.newEnbUeX2apId);

  uint16_t rnti = params.oldEnbUeX2apId;
  GetNistUeManager (rnti)->RecvUeContextRelease (params);
  RemoveUe (rnti);
}

void
NistLteEnbRrc::DoRecvLoadInformation (NistEpcX2SapUser::NistLoadInformationParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Recv X2 message: LOAD INFORMATION");

  NS_LOG_LOGIC ("Number of cellInformationItems = " << params.cellInformationList.size ());

  m_ffrRrcSapProvider->RecvLoadInformation(params);
}

void
NistLteEnbRrc::DoRecvResourceNistStatusUpdate (NistEpcX2SapUser::NistResourceNistStatusUpdateParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Recv X2 message: RESOURCE STATUS UPDATE");

  NS_LOG_LOGIC ("Number of cellMeasurementResultItems = " << params.cellMeasurementResultList.size ());

  NS_ASSERT ("Processing of RESOURCE STATUS UPDATE X2 message IS NOT IMPLEMENTED");
}

void
NistLteEnbRrc::DoRecvUeData (NistEpcX2SapUser::NistUeDataParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Recv UE DATA FORWARDING through X2 interface");
  NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
  NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
  NS_LOG_LOGIC ("gtpTeid = " << params.gtpTeid);
  NS_LOG_LOGIC ("ueData = " << params.ueData);
  NS_LOG_LOGIC ("ueData size = " << params.ueData->GetSize ());

  std::map<uint32_t, NistX2uTeidInfo>::iterator 
    teidInfoIt = m_x2uTeidInfoMap.find (params.gtpTeid);
  if (teidInfoIt != m_x2uTeidInfoMap.end ())
    {
      GetNistUeManager (teidInfoIt->second.rnti)->SendData (teidInfoIt->second.drbid, params.ueData);
    }
  else
    {
      NS_FATAL_ERROR ("X2-U data received but no NistX2uTeidInfo found");
    }
}


uint16_t 
NistLteEnbRrc::DoAllocateTemporaryCellRnti ()
{
  NS_LOG_FUNCTION (this);
  return AddUe (NistUeManager::INITIAL_RANDOM_ACCESS);
}

void
NistLteEnbRrc::DoRrcConfigurationUpdateInd (NistLteEnbCmacSapUser::NistUeConfig cmacParams)
{
  Ptr<NistUeManager> ueManager = GetNistUeManager (cmacParams.m_rnti);
  ueManager->CmacNistUeConfigUpdateInd (cmacParams);
}

void
NistLteEnbRrc::DoNotifyLcConfigResult (uint16_t rnti, uint8_t lcid, bool success)
{
  NS_LOG_FUNCTION (this << (uint32_t) rnti);
  NS_FATAL_ERROR ("not implemented");
}


uint8_t
NistLteEnbRrc::DoAddUeMeasReportConfigForHandover (NistLteRrcSap::NistReportConfigEutra reportConfig)
{
  NS_LOG_FUNCTION (this);
  uint8_t measId = AddUeMeasReportConfig (reportConfig);
  m_handoverMeasIds.insert (measId);
  return measId;
}

void
NistLteEnbRrc::DoTriggerHandover (uint16_t rnti, uint16_t targetCellId)
{
  NS_LOG_FUNCTION (this << rnti << targetCellId);

  bool isHandoverAllowed = true;

  if (m_anrSapProvider != 0)
    {
      // ensure that proper neighbour relationship exists between source and target cells
      bool noHo = m_anrSapProvider->GetNoHo (targetCellId);
      bool noX2 = m_anrSapProvider->GetNoX2 (targetCellId);
      NS_LOG_DEBUG (this << " cellId=" << m_cellId
                         << " targetCellId=" << targetCellId
                         << " NRT.NoHo=" << noHo << " NRT.NoX2=" << noX2);

      if (noHo || noX2)
        {
          isHandoverAllowed = false;
          NS_LOG_LOGIC (this << " handover to cell " << targetCellId
                             << " is not allowed by ANR");
        }
    }

  Ptr<NistUeManager> ueManager = GetNistUeManager (rnti);
  NS_ASSERT_MSG (ueManager != 0, "Cannot find UE context with RNTI " << rnti);

  if (ueManager->GetState () != NistUeManager::CONNECTED_NORMALLY)
    {
      isHandoverAllowed = false;
      NS_LOG_LOGIC (this << " handover is not allowed because the UE"
                         << " rnti=" << rnti << " is in "
                         << ToString (ueManager->GetState ()) << " state");
    }

  if (isHandoverAllowed)
    {
      // initiate handover execution
      ueManager->PrepareHandover (targetCellId);
    }
}

uint8_t
NistLteEnbRrc::DoAddUeMeasReportConfigForAnr (NistLteRrcSap::NistReportConfigEutra reportConfig)
{
  NS_LOG_FUNCTION (this);
  uint8_t measId = AddUeMeasReportConfig (reportConfig);
  m_anrMeasIds.insert (measId);
  return measId;
}

uint8_t
NistLteEnbRrc::DoAddUeMeasReportConfigForFfr (NistLteRrcSap::NistReportConfigEutra reportConfig)
{
  NS_LOG_FUNCTION (this);
  uint8_t measId = AddUeMeasReportConfig (reportConfig);
  m_ffrMeasIds.insert (measId);
  return measId;
}

void
NistLteEnbRrc::DoSetNistPdschConfigDedicated (uint16_t rnti, NistLteRrcSap::NistPdschConfigDedicated pdschConfigDedicated)
{
  NS_LOG_FUNCTION (this);
  Ptr<NistUeManager> ueManager = GetNistUeManager (rnti);
  ueManager->SetNistPdschConfigDedicated (pdschConfigDedicated);
}

void
NistLteEnbRrc::DoSendLoadInformation (NistEpcX2Sap::NistLoadInformationParams params)
{
  NS_LOG_FUNCTION (this);

  m_x2SapProvider->SendLoadInformation(params);
}

uint16_t
NistLteEnbRrc::AddUe (NistUeManager::State state)
{
  NS_LOG_FUNCTION (this);
  bool found = false;
  uint16_t rnti;
  for (rnti = m_lastAllocatedRnti + 1; 
       (rnti != m_lastAllocatedRnti - 1) && (!found);
       ++rnti)
    {
      if ((rnti != 0) && (m_ueMap.find (rnti) == m_ueMap.end ()))
        {
          found = true;
          break;
        }
    }

  NS_ASSERT_MSG (found, "no more RNTIs available (do you have more than 65535 UEs in a cell?)");
  m_lastAllocatedRnti = rnti;
  Ptr<NistUeManager> ueManager = CreateObject<NistUeManager> (this, rnti, state);
  m_ueMap.insert (std::pair<uint16_t, Ptr<NistUeManager> > (rnti, ueManager));
  ueManager->Initialize ();
  NS_LOG_DEBUG (this << " New UE RNTI " << rnti << " cellId " << m_cellId << " srs CI " << ueManager->GetSrsConfigurationIndex ());
  m_newUeContextTrace (m_cellId, rnti);
  return rnti;
}

void
NistLteEnbRrc::RemoveUe (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << (uint32_t) rnti);
  std::map <uint16_t, Ptr<NistUeManager> >::iterator it = m_ueMap.find (rnti);
  NS_ASSERT_MSG (it != m_ueMap.end (), "request to remove UE info with unknown rnti " << rnti);
  uint16_t srsCi = (*it).second->GetSrsConfigurationIndex ();
  m_ueMap.erase (it);
  m_cmacSapProvider->RemoveUe (rnti);
  m_cphySapProvider->RemoveUe (rnti);
  if (m_s1SapProvider != 0)
    {
      m_s1SapProvider->UeContextRelease (rnti);
    }
  // need to do this after NistUeManager has been deleted
  RemoveSrsConfigurationIndex (srsCi); 
}

TypeId
NistLteEnbRrc::GetRlcType (NistEpsBearer bearer)
{
  switch (m_epsBearerToRlcMapping)
    {
    case RLC_SM_ALWAYS:
      return NistLteRlcSm::GetTypeId ();
      break;

    case  RLC_UM_ALWAYS:
      return NistLteRlcUm::GetTypeId ();
      break;

    case RLC_AM_ALWAYS:
      return NistLteRlcAm::GetTypeId ();
      break;

    case PER_BASED:
      if (bearer.GetPacketErrorLossRate () > 1.0e-5)
        {
          return NistLteRlcUm::GetTypeId ();
        }
      else
        {
          return NistLteRlcAm::GetTypeId ();
        }
      break;

    default:
      return NistLteRlcSm::GetTypeId ();
      break;
    }
}


void
NistLteEnbRrc::AddX2Neighbour (uint16_t cellId)
{
  NS_LOG_FUNCTION (this << cellId);

  if (m_anrSapProvider != 0)
    {
      m_anrSapProvider->AddNeighbourRelation (cellId);
    }
}

void
NistLteEnbRrc::SetCsgId (uint32_t csgId, bool csgIndication)
{
  NS_LOG_FUNCTION (this << csgId << csgIndication);
  m_sib1.cellAccessRelatedInfo.csgIdentity = csgId;
  m_sib1.cellAccessRelatedInfo.csgIndication = csgIndication;
  m_cphySapProvider->SetNistSystemInformationBlockType1 (m_sib1);
}


/// Number of distinct SRS periodicity plus one.
static const uint8_t SRS_ENTRIES = 9;
/**
 * Sounding Reference Symbol (SRS) periodicity (TSRS) in milliseconds. Taken
 * from 3GPP TS 36.213 Table 8.2-1. Index starts from 1.
 */
static const uint16_t g_srsPeriodicity[SRS_ENTRIES] = {0, 2, 5, 10, 20, 40,  80, 160, 320};
/**
 * The lower bound (inclusive) of the SRS configuration indices (ISRS) which
 * use the corresponding SRS periodicity (TSRS). Taken from 3GPP TS 36.213
 * Table 8.2-1. Index starts from 1.
 */
static const uint16_t g_srsCiLow[SRS_ENTRIES] =       {0, 0, 2,  7, 17, 37,  77, 157, 317};
/**
 * The upper bound (inclusive) of the SRS configuration indices (ISRS) which
 * use the corresponding SRS periodicity (TSRS). Taken from 3GPP TS 36.213
 * Table 8.2-1. Index starts from 1.
 */
static const uint16_t g_srsCiHigh[SRS_ENTRIES] =      {0, 1, 6, 16, 36, 76, 156, 316, 636};

void 
NistLteEnbRrc::SetSrsPeriodicity (uint32_t p)
{
  NS_LOG_FUNCTION (this << p);
  for (uint32_t id = 1; id < SRS_ENTRIES; ++id)
    {
      if (g_srsPeriodicity[id] == p)
        {
          m_srsCurrentPeriodicityId = id;
          return;
        }
    }
  // no match found
  std::ostringstream allowedValues;
  for (uint32_t id = 1; id < SRS_ENTRIES; ++id)
    {
      allowedValues << g_srsPeriodicity[id] << " ";
    }
  NS_FATAL_ERROR ("illecit SRS periodicity value " << p << ". Allowed values: " << allowedValues.str ());
}

uint32_t 
NistLteEnbRrc::GetSrsPeriodicity () const
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_srsCurrentPeriodicityId > 0);
  NS_ASSERT (m_srsCurrentPeriodicityId < SRS_ENTRIES);
  return g_srsPeriodicity[m_srsCurrentPeriodicityId];
}


uint16_t
NistLteEnbRrc::GetNewSrsConfigurationIndex ()
{
  NS_LOG_FUNCTION (this << m_ueSrsConfigurationIndexSet.size ());
  // SRS
  NS_ASSERT (m_srsCurrentPeriodicityId > 0);
  NS_ASSERT (m_srsCurrentPeriodicityId < SRS_ENTRIES);
  NS_LOG_DEBUG (this << " SRS p " << g_srsPeriodicity[m_srsCurrentPeriodicityId] << " set " << m_ueSrsConfigurationIndexSet.size ());
  if (m_ueSrsConfigurationIndexSet.size () >= g_srsPeriodicity[m_srsCurrentPeriodicityId])
    {
      NS_FATAL_ERROR ("too many UEs (" << m_ueSrsConfigurationIndexSet.size () + 1 
                                       << ") for current SRS periodicity "
                                       <<  g_srsPeriodicity[m_srsCurrentPeriodicityId]
                                       << ", consider increasing the value of ns3::NistLteEnbRrc::SrsPeriodicity");
    }

  if (m_ueSrsConfigurationIndexSet.empty ())
    {
      // first entry
      m_lastAllocatedConfigurationIndex = g_srsCiLow[m_srsCurrentPeriodicityId];
      m_ueSrsConfigurationIndexSet.insert (m_lastAllocatedConfigurationIndex);
    }
  else
    {
      // find a CI from the available ones
      std::set<uint16_t>::reverse_iterator rit = m_ueSrsConfigurationIndexSet.rbegin ();
      NS_ASSERT (rit != m_ueSrsConfigurationIndexSet.rend ());
      NS_LOG_DEBUG (this << " lower bound " << (*rit) << " of " << g_srsCiHigh[m_srsCurrentPeriodicityId]);
      if ((*rit) < g_srsCiHigh[m_srsCurrentPeriodicityId])
        {
          // got it from the upper bound
          m_lastAllocatedConfigurationIndex = (*rit) + 1;
          m_ueSrsConfigurationIndexSet.insert (m_lastAllocatedConfigurationIndex);
        }
      else
        {
          // look for released ones
          for (uint16_t srcCi = g_srsCiLow[m_srsCurrentPeriodicityId]; srcCi < g_srsCiHigh[m_srsCurrentPeriodicityId]; srcCi++) 
            {
              std::set<uint16_t>::iterator it = m_ueSrsConfigurationIndexSet.find (srcCi);
              if (it == m_ueSrsConfigurationIndexSet.end ())
                {
                  m_lastAllocatedConfigurationIndex = srcCi;
                  m_ueSrsConfigurationIndexSet.insert (srcCi);
                  break;
                }
            }
        } 
    }
  return m_lastAllocatedConfigurationIndex;

}


void
NistLteEnbRrc::RemoveSrsConfigurationIndex (uint16_t srcCi)
{
  NS_LOG_FUNCTION (this << srcCi);
  std::set<uint16_t>::iterator it = m_ueSrsConfigurationIndexSet.find (srcCi);
  NS_ASSERT_MSG (it != m_ueSrsConfigurationIndexSet.end (), "request to remove unkwown SRS CI " << srcCi);
  m_ueSrsConfigurationIndexSet.erase (it);
}

uint8_t 
NistLteEnbRrc::GetLogicalChannelGroup (NistEpsBearer bearer)
{
  if (bearer.IsGbr ())
    {
      return 1;
    }
  else
    {
      return 2;
    }
}

uint8_t 
NistLteEnbRrc::GetLogicalChannelPriority (NistEpsBearer bearer)
{
  return bearer.qci;
}

void
NistLteEnbRrc::SendNistSystemInformation ()
{
  // NS_LOG_FUNCTION (this);

  /*
   * For simplicity, we use the same periodicity for all SIBs. Note that in real
   * systems the periodicy of each SIBs could be different.
   */
  NistLteRrcSap::NistSystemInformation si;
  si.haveSib2 = true;
  si.sib2.freqInfo.ulCarrierFreq = m_ulEarfcn;
  si.sib2.freqInfo.ulBandwidth = m_ulBandwidth;
  si.sib2.radioResourceConfigCommon.pdschConfigCommon.referenceSignalPower = m_cphySapProvider->GetReferenceSignalPower();
  si.sib2.radioResourceConfigCommon.pdschConfigCommon.pb = 0;

  NistLteEnbCmacSapProvider::NistRachConfig rc = m_cmacSapProvider->GetNistRachConfig ();
  NistLteRrcSap::NistRachConfigCommon rachConfigCommon;
  rachConfigCommon.preambleInfo.numberOfRaPreambles = rc.numberOfRaPreambles;
  rachConfigCommon.raSupervisionInfo.preambleTransMax = rc.preambleTransMax;
  rachConfigCommon.raSupervisionInfo.raResponseWindowSize = rc.raResponseWindowSize;
  si.sib2.radioResourceConfigCommon.rachConfigCommon = rachConfigCommon;

  /* 
   * Send information for SIB 18
   */
  if (m_sidelinkConfiguration && m_sidelinkConfiguration->IsSlEnabled()) {
    si.haveSib18 = true;
    si.sib18 = m_sidelinkConfiguration->GetSystemInformationType18();
  } else {
    si.haveSib18 = false;
  }
    
  m_rrcSapUser->SendNistSystemInformation (si);
  Simulator::Schedule (m_systemInformationPeriodicity, &NistLteEnbRrc::SendNistSystemInformation, this);
}

 
} // namespace ns3

