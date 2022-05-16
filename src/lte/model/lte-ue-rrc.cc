/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 * Copyright (c) 2018 Fraunhofer ESK : RLF extensions
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
 * Author: Nicola Baldo <nbaldo@cttc.es>
 *         Budiarto Herman <budiarto.herman@magister.fi>
 * Modified by:
 *          Danilo Abrignani <danilo.abrignani@unibo.it> (Carrier Aggregation - GSoC 2015)
 *          Biljana Bojovic <biljana.bojovic@cttc.es> (Carrier Aggregation)
 *          Vignesh Babu <ns3-dev@esk.fraunhofer.de> (RLF extensions)
 *          NIST // Contributions may not be subject to US copyright. (D2D extensions)
 */

#include "lte-ue-rrc.h"
#include "lte-rrc-header.h"

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/simulator.h>

#include <ns3/lte-rlc.h>
#include <ns3/lte-rlc-tm.h>
#include <ns3/lte-rlc-um.h>
#include <ns3/lte-rlc-am.h>
#include <ns3/lte-pdcp.h>
#include <ns3/lte-radio-bearer-info.h>
#include <ns3/lte-sl-pool.h>

#include <cmath>
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteUeRrc");

/////////////////////////////
// CMAC SAP forwarder
/////////////////////////////

/// UeMemberLteUeCmacSapUser class
class UeMemberLteUeCmacSapUser : public LteUeCmacSapUser
{
public:
  /**
   * Constructor
   *
   * \param rrc the RRC class
   */
  UeMemberLteUeCmacSapUser (LteUeRrc* rrc);

  virtual void SetTemporaryCellRnti (uint16_t rnti);
  virtual void NotifyRandomAccessSuccessful ();
  virtual void NotifyRandomAccessFailed ();
  //Sidelink communication
  virtual void NotifySidelinkReception (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id);
  virtual void NotifyMacHasSlDataToSend ();
  virtual void NotifyMacHasNoSlDataToSend ();
  //Sidelink discovery
  virtual void NotifyDiscoveryReception (Ptr<Packet> p);

private:
  LteUeRrc* m_rrc; ///< the RRC class
};

UeMemberLteUeCmacSapUser::UeMemberLteUeCmacSapUser (LteUeRrc* rrc)
  : m_rrc (rrc)
{}

void
UeMemberLteUeCmacSapUser::SetTemporaryCellRnti (uint16_t rnti)
{
  m_rrc->DoSetTemporaryCellRnti (rnti);
}


void
UeMemberLteUeCmacSapUser::NotifyRandomAccessSuccessful ()
{
  m_rrc->DoNotifyRandomAccessSuccessful ();
}

void
UeMemberLteUeCmacSapUser::NotifyRandomAccessFailed ()
{
  m_rrc->DoNotifyRandomAccessFailed ();
}
void
UeMemberLteUeCmacSapUser::NotifySidelinkReception (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id)
{
  m_rrc->DoNotifySidelinkReception (lcId, srcL2Id, dstL2Id);
}

void
UeMemberLteUeCmacSapUser::NotifyMacHasSlDataToSend ()
{
  m_rrc->DoNotifyMacHasSlDataToSend ();
}

void
UeMemberLteUeCmacSapUser::NotifyMacHasNoSlDataToSend ()
{
  m_rrc->DoNotifyMacHasNoSlDataToSend ();
}

void
UeMemberLteUeCmacSapUser::NotifyDiscoveryReception (Ptr<Packet> p)
{
  m_rrc->DoNotifyDiscoveryReception (p);
}






/// Map each of UE RRC states to its string representation.
static const std::string g_ueRrcStateName[LteUeRrc::NUM_STATES] =
{
  "IDLE_START",
  "IDLE_CELL_SEARCH",
  "IDLE_WAIT_MIB_SIB1",
  "IDLE_WAIT_MIB",
  "IDLE_WAIT_SIB1",
  "IDLE_CAMPED_NORMALLY",
  "IDLE_WAIT_SIB2",
  "IDLE_RANDOM_ACCESS",
  "IDLE_CONNECTING",
  "CONNECTED_NORMALLY",
  "CONNECTED_HANDOVER",
  "CONNECTED_PHY_PROBLEM",
  "CONNECTED_REESTABLISHING"
};


/////////////////////////////
// ue RRC methods
/////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (LteUeRrc);


LteUeRrc::LteUeRrc ()
  : m_cmacSapProvider (0),
  m_cphySapProvider (0),
  m_rrcSapUser (0),
  m_macSapProvider (0),
  m_asSapUser (0),
  m_ccmRrcSapProvider (0),
  m_state (IDLE_START),
  m_imsi (0),
  m_rnti (0),
  m_cellId (0),
  m_useRlcSm (true),
  m_connectionPending (false),
  m_hasReceivedMib (false),
  m_hasReceivedSib1 (false),
  m_hasReceivedSib2 (false),
  m_csgWhiteList (0),
  m_noOfSyncIndications (0),
  m_leaveConnectedMode (false),
  m_previousCellId (0),
  m_connEstFailCountLimit (0),
  m_connEstFailCount (0),
  m_sidelinkConfiguration (0),
  m_txPool (0),
  m_slssTransmissionActive (false),
  m_txSlSyncOffsetIndicator (0),
  m_hasSyncRef (false),
  m_inCoverage (false),
  m_slssId (0),
  m_currSubframeNo (0),
  m_currFrameNo (0),
  m_hasDataToTransmit (false),
  m_inInnerCellOfSyncRef (false),
  m_slssTxTime (Seconds (0)),
  m_numberOfComponentCarriers (MIN_NO_CC)
{
  NS_LOG_FUNCTION (this);
  m_cphySapUser.push_back (new MemberLteUeCphySapUser<LteUeRrc> (this));
  m_cmacSapUser.push_back (new UeMemberLteUeCmacSapUser (this));
  m_cphySapProvider.push_back (0);
  m_cmacSapProvider.push_back (0);
  m_rrcSapProvider = new MemberLteUeRrcSapProvider<LteUeRrc> (this);
  m_drbPdcpSapUser = new LtePdcpSpecificLtePdcpSapUser<LteUeRrc> (this);
  m_asSapProvider = new MemberLteAsSapProvider<LteUeRrc> (this);
  m_ccmRrcSapUser = new MemberLteUeCcmRrcSapUser<LteUeRrc> (this);
}

LteUeRrc::~LteUeRrc ()
{
  NS_LOG_FUNCTION (this);
}

void
LteUeRrc::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  if (m_sidelinkConfiguration)
    {
      m_sidelinkConfiguration->Dispose ();
    }
  for ( uint16_t i = 0; i < m_numberOfComponentCarriers; i++)
    {
      delete m_cphySapUser.at (i);
      delete m_cmacSapUser.at (i);
    }
  m_cphySapUser.clear ();
  m_cmacSapUser.clear ();
  delete m_rrcSapProvider;
  delete m_drbPdcpSapUser;
  delete m_asSapProvider;
  delete m_ccmRrcSapUser;
  m_cphySapProvider.erase (m_cphySapProvider.begin (), m_cphySapProvider.end ());
  m_cphySapProvider.clear ();
  m_cmacSapProvider.erase (m_cmacSapProvider.begin (), m_cmacSapProvider.end ());
  m_cmacSapProvider.clear ();
  m_drbMap.clear ();
}

TypeId
LteUeRrc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteUeRrc")
    .SetParent<Object> ()
    .SetGroupName ("Lte")
    .AddConstructor<LteUeRrc> ()
    .AddAttribute ("DataRadioBearerMap", "List of UE RadioBearerInfo for Data Radio Bearers by LCID.",
                   ObjectMapValue (),
                   MakeObjectMapAccessor (&LteUeRrc::m_drbMap),
                   MakeObjectMapChecker<LteDataRadioBearerInfo> ())
    .AddAttribute ("Srb0", "SignalingRadioBearerInfo for SRB0",
                   PointerValue (),
                   MakePointerAccessor (&LteUeRrc::m_srb0),
                   MakePointerChecker<LteSignalingRadioBearerInfo> ())
    .AddAttribute ("Srb1", "SignalingRadioBearerInfo for SRB1",
                   PointerValue (),
                   MakePointerAccessor (&LteUeRrc::m_srb1),
                   MakePointerChecker<LteSignalingRadioBearerInfo> ())
    .AddAttribute ("CellId",
                   "Serving cell identifier",
                   UintegerValue (0), // unused, read-only attribute
                   MakeUintegerAccessor (&LteUeRrc::GetCellId),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("C-RNTI",
                   "Cell Radio Network Temporary Identifier",
                   UintegerValue (0), // unused, read-only attribute
                   MakeUintegerAccessor (&LteUeRrc::GetRnti),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("T300",
                   "Timer for the RRC Connection Establishment procedure "
                   "(i.e., the procedure is deemed as failed if it takes longer than this). "
                   "Standard values: 100ms, 200ms, 300ms, 400ms, 600ms, 1000ms, 1500ms, 2000ms",
                   TimeValue (MilliSeconds (100)), //see 3GPP 36.331 UE-TimersAndConstants & RLF-TimersAndConstants
                   MakeTimeAccessor (&LteUeRrc::m_t300),
                   MakeTimeChecker (MilliSeconds (100), MilliSeconds (2000)))
    .AddAttribute ("T310",
                   "Timer for detecting the Radio link failure "
                   "(i.e., the radio link is deemed as failed if this timer expires). "
                   "Standard values: 0ms 50ms, 100ms, 200ms, 500ms, 1000ms, 2000ms",
                   TimeValue (MilliSeconds (1000)), //see 3GPP 36.331 UE-TimersAndConstants & RLF-TimersAndConstants
                   MakeTimeAccessor (&LteUeRrc::m_t310),
                   MakeTimeChecker (MilliSeconds (0), MilliSeconds (2000)))
    .AddAttribute ("N310",
                   "This specifies the maximum number of out-of-sync indications. "
                   "Standard values: 1, 2, 3, 4, 6, 8, 10, 20",
                   UintegerValue (6), //see 3GPP 36.331 UE-TimersAndConstants & RLF-TimersAndConstants
                   MakeUintegerAccessor (&LteUeRrc::m_n310),
                   MakeUintegerChecker<uint8_t> (1, 20))
    .AddAttribute ("N311",
                   "This specifies the maximum number of in-sync indications. "
                   "Standard values: 1, 2, 3, 4, 5, 6, 8, 10",
                   UintegerValue (2), //see 3GPP 36.331 UE-TimersAndConstants & RLF-TimersAndConstants
                   MakeUintegerAccessor (&LteUeRrc::m_n311),
                   MakeUintegerChecker<uint8_t> (1, 10))
    .AddTraceSource ("MibReceived",
                     "trace fired upon reception of Master Information Block",
                     MakeTraceSourceAccessor (&LteUeRrc::m_mibReceivedTrace),
                     "ns3::LteUeRrc::MibSibHandoverTracedCallback")
    .AddTraceSource ("Sib1Received",
                     "trace fired upon reception of System Information Block Type 1",
                     MakeTraceSourceAccessor (&LteUeRrc::m_sib1ReceivedTrace),
                     "ns3::LteUeRrc::MibSibHandoverTracedCallback")
    .AddTraceSource ("Sib2Received",
                     "trace fired upon reception of System Information Block Type 2",
                     MakeTraceSourceAccessor (&LteUeRrc::m_sib2ReceivedTrace),
                     "ns3::LteUeRrc::ImsiCidRntiTracedCallback")
    .AddAttribute ("SidelinkConfiguration",
                   "The Sidelink configuration for this UE",
                   PointerValue (),
                   MakePointerAccessor (&LteUeRrc::m_sidelinkConfiguration),
                   MakePointerChecker <LteSlUeRrc> ())
    .AddAttribute ("UeSlssTransmissionEnabled",
                   "If True, the UE transmits SLSSs when required as part of the Sidelink synchronization protocol",
                   BooleanValue (false),
                   MakeBooleanAccessor (&LteUeRrc::m_slssTransmissionEnabled),
                   MakeBooleanChecker ())
    .AddAttribute ("MinSrsrp",
                   "The minimum S-RSRP required to consider a SyncRef detectable [dBm]",
                   DoubleValue (-125),
                   MakeDoubleAccessor (&LteUeRrc::m_minSrsrp),
                   MakeDoubleChecker<double>())
    .AddAttribute ("MinSdRsrp",
                   "The minimum SD-RSRP required to consider a Relay UE detectable [dBm]",
                   DoubleValue (-125),
                   MakeDoubleAccessor (&LteUeRrc::m_minSdRsrp),
                   MakeDoubleChecker<double>())
    .AddTraceSource ("ChangeOfSyncRef",
                     "trace fired upon report of a change of SyncRef",
                     MakeTraceSourceAccessor (&LteUeRrc::m_changeOfSyncRefTrace),
                     "ns3::LteUeRrc::ChangeOfSyncRefTracedCallback")
    .AddTraceSource ("SendSLSS",
                     "trace fired upon send of a SLSS",
                     MakeTraceSourceAccessor (&LteUeRrc::m_SendSlssTrace),
                     "ns3::LteUeRrc::SendSLSSTracedCallback")
    .AddTraceSource ("DiscoveryMonitoring",
                     "trace to track the monitoring of discovery messages",
                     MakeTraceSourceAccessor (&LteUeRrc::m_discoveryMonitoringTrace),
                     "ns3::LteUeRrc::DiscoveryMonitoringTracedCallback")
    .AddTraceSource ("StateTransition",
                     "trace fired upon every UE RRC state transition",
                     MakeTraceSourceAccessor (&LteUeRrc::m_stateTransitionTrace),
                     "ns3::LteUeRrc::StateTracedCallback")
    .AddTraceSource ("InitialCellSelectionEndOk",
                     "trace fired upon successful initial cell selection procedure",
                     MakeTraceSourceAccessor (&LteUeRrc::m_initialCellSelectionEndOkTrace),
                     "ns3::LteUeRrc::CellSelectionTracedCallback")
    .AddTraceSource ("InitialCellSelectionEndError",
                     "trace fired upon failed initial cell selection procedure",
                     MakeTraceSourceAccessor (&LteUeRrc::m_initialCellSelectionEndErrorTrace),
                     "ns3::LteUeRrc::CellSelectionTracedCallback")
    .AddTraceSource ("RandomAccessSuccessful",
                     "trace fired upon successful completion of the random access procedure",
                     MakeTraceSourceAccessor (&LteUeRrc::m_randomAccessSuccessfulTrace),
                     "ns3::LteUeRrc::ImsiCidRntiTracedCallback")
    .AddTraceSource ("RandomAccessError",
                     "trace fired upon failure of the random access procedure",
                     MakeTraceSourceAccessor (&LteUeRrc::m_randomAccessErrorTrace),
                     "ns3::LteUeRrc::ImsiCidRntiTracedCallback")
    .AddTraceSource ("ConnectionEstablished",
                     "trace fired upon successful RRC connection establishment",
                     MakeTraceSourceAccessor (&LteUeRrc::m_connectionEstablishedTrace),
                     "ns3::LteUeRrc::ImsiCidRntiTracedCallback")
    .AddTraceSource ("ConnectionTimeout",
                     "trace fired upon timeout RRC connection establishment because of T300",
                     MakeTraceSourceAccessor (&LteUeRrc::m_connectionTimeoutTrace),
                     "ns3::LteUeRrc::ImsiCidRntiCountTracedCallback")
    .AddTraceSource ("ConnectionReconfiguration",
                     "trace fired upon RRC connection reconfiguration",
                     MakeTraceSourceAccessor (&LteUeRrc::m_connectionReconfigurationTrace),
                     "ns3::LteUeRrc::ImsiCidRntiTracedCallback")
    .AddTraceSource ("HandoverStart",
                     "trace fired upon start of a handover procedure",
                     MakeTraceSourceAccessor (&LteUeRrc::m_handoverStartTrace),
                     "ns3::LteUeRrc::MibSibHandoverTracedCallback")
    .AddTraceSource ("HandoverEndOk",
                     "trace fired upon successful termination of a handover procedure",
                     MakeTraceSourceAccessor (&LteUeRrc::m_handoverEndOkTrace),
                     "ns3::LteUeRrc::ImsiCidRntiTracedCallback")
    .AddTraceSource ("HandoverEndError",
                     "trace fired upon failure of a handover procedure",
                     MakeTraceSourceAccessor (&LteUeRrc::m_handoverEndErrorTrace),
                     "ns3::LteUeRrc::ImsiCidRntiTracedCallback")
    .AddTraceSource ("SCarrierConfigured",
                     "trace fired after configuring secondary carriers",
                     MakeTraceSourceAccessor (&LteUeRrc::m_sCarrierConfiguredTrace),
                     "ns3::LteUeRrc::SCarrierConfiguredTracedCallback")
    .AddTraceSource ("Srb1Created",
                     "trace fired after SRB1 is created",
                     MakeTraceSourceAccessor (&LteUeRrc::m_srb1CreatedTrace),
                     "ns3::LteUeRrc::ImsiCidRntiTracedCallback")
    .AddTraceSource ("DrbCreated",
                     "trace fired after DRB is created",
                     MakeTraceSourceAccessor (&LteUeRrc::m_drbCreatedTrace),
                     "ns3::LteUeRrc::ImsiCidRntiLcIdTracedCallback")
    .AddTraceSource ("RadioLinkFailure",
                     "trace fired upon failure of radio link",
                     MakeTraceSourceAccessor (&LteUeRrc::m_radioLinkFailureTrace),
                     "ns3::LteUeRrc::ImsiCidRntiTracedCallback")
    .AddTraceSource ("PhySyncDetection",
                     "trace fired upon receiving in Sync or out of Sync indications from UE PHY",
                     MakeTraceSourceAccessor (&LteUeRrc::m_phySyncDetectionTrace),
                     "ns3::LteUeRrc::PhySyncDetectionTracedCallback")
    .AddTraceSource ("RsrpMeasurement",
                     "Trace fired upon primary carrier L3 RSRP measurement storage",
                     MakeTraceSourceAccessor (&LteUeRrc::m_rsrpTrace),
                     "ns3::LteUeRrc::RsrpMeasurementTracedCallback")
  ;
  return tid;
}


void
LteUeRrc::SetLteUeCphySapProvider (LteUeCphySapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_cphySapProvider.at (0) = s;
}

void
LteUeRrc::SetLteUeCphySapProvider (LteUeCphySapProvider * s, uint8_t index)
{
  NS_LOG_FUNCTION (this << s);
  m_cphySapProvider.at (index) = s;
}

LteUeCphySapUser*
LteUeRrc::GetLteUeCphySapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_cphySapUser.at (0);
}

LteUeCphySapUser*
LteUeRrc::GetLteUeCphySapUser (uint8_t index)
{
  NS_LOG_FUNCTION (this);
  return m_cphySapUser.at (index);
}

void
LteUeRrc::SetLteUeCmacSapProvider (LteUeCmacSapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_cmacSapProvider.at (0) = s;
}

void
LteUeRrc::SetLteUeCmacSapProvider (LteUeCmacSapProvider * s, uint8_t index)
{
  NS_LOG_FUNCTION (this << s);
  m_cmacSapProvider.at (index) = s;
}

LteUeCmacSapUser*
LteUeRrc::GetLteUeCmacSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_cmacSapUser.at (0);
}

LteUeCmacSapUser*
LteUeRrc::GetLteUeCmacSapUser (uint8_t index)
{
  NS_LOG_FUNCTION (this);
  return m_cmacSapUser.at (index);
}

void
LteUeRrc::SetLteUeRrcSapUser (LteUeRrcSapUser * s)
{
  NS_LOG_FUNCTION (this << s);
  m_rrcSapUser = s;
}

LteUeRrcSapProvider*
LteUeRrc::GetLteUeRrcSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_rrcSapProvider;
}

void
LteUeRrc::SetLteMacSapProvider (LteMacSapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_macSapProvider = s;
}

void
LteUeRrc::SetLteCcmRrcSapProvider (LteUeCcmRrcSapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_ccmRrcSapProvider = s;
}

LteUeCcmRrcSapUser*
LteUeRrc::GetLteCcmRrcSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_ccmRrcSapUser;
}

void
LteUeRrc::SetAsSapUser (LteAsSapUser* s)
{
  m_asSapUser = s;
}

LteAsSapProvider*
LteUeRrc::GetAsSapProvider ()
{
  return m_asSapProvider;
}

void
LteUeRrc::SetImsi (uint64_t imsi)
{
  NS_LOG_FUNCTION (this << imsi);
  m_imsi = imsi;

  //Communicate the IMSI to MACs and PHYs for all the component carriers
  for (uint16_t i = 0; i < m_numberOfComponentCarriers; i++)
    {
      m_cmacSapProvider.at (i)->SetImsi (m_imsi);
      m_cphySapProvider.at (i)->SetImsi (m_imsi);
    }
}

void
LteUeRrc::StorePreviousCellId (uint16_t cellId)
{
  NS_LOG_FUNCTION (this << cellId);
  m_previousCellId = cellId;
}

uint64_t
LteUeRrc::GetImsi (void) const
{
  return m_imsi;
}

uint16_t
LteUeRrc::GetRnti () const
{
  NS_LOG_FUNCTION (this);
  return m_rnti;
}

uint16_t
LteUeRrc::GetCellId () const
{
  NS_LOG_FUNCTION (this);
  return m_cellId;
}

bool
LteUeRrc::IsServingCell (uint16_t cellId) const
{
  NS_LOG_FUNCTION (this);
  for (auto &cphySap: m_cphySapProvider)
    {
      if (cellId == cphySap->GetCellId ())
        {
          return true;
        }
    }
  return false;
}


uint8_t
LteUeRrc::GetUlBandwidth () const
{
  NS_LOG_FUNCTION (this);
  return m_ulBandwidth;
}

uint8_t
LteUeRrc::GetDlBandwidth () const
{
  NS_LOG_FUNCTION (this);
  return m_dlBandwidth;
}

uint32_t
LteUeRrc::GetDlEarfcn () const
{
  return m_dlEarfcn;
}

uint32_t
LteUeRrc::GetUlEarfcn () const
{
  NS_LOG_FUNCTION (this);
  return m_ulEarfcn;
}

LteUeRrc::State
LteUeRrc::GetState (void) const
{
  NS_LOG_FUNCTION (this);
  return m_state;
}

uint16_t
LteUeRrc::GetPreviousCellId () const
{
  NS_LOG_FUNCTION (this);
  return m_previousCellId;
}

void
LteUeRrc::SetUseRlcSm (bool val)
{
  NS_LOG_FUNCTION (this);
  m_useRlcSm = val;
}


void
LteUeRrc::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);

  m_currSyncRef.slssid = 0;
  m_currSyncRef.offset = 0;

  // setup the UE side of SRB0
  uint8_t lcid = 0;

  Ptr<LteRlc> rlc = CreateObject<LteRlcTm> ()->GetObject<LteRlc> ();
  rlc->SetLteMacSapProvider (m_macSapProvider);
  rlc->SetRnti (m_rnti);
  rlc->SetLcId (lcid);

  m_srb0 = CreateObject<LteSignalingRadioBearerInfo> ();
  m_srb0->m_rlc = rlc;
  m_srb0->m_srbIdentity = 0;
  LteUeRrcSapUser::SetupParameters ueParams;
  ueParams.srb0SapProvider = m_srb0->m_rlc->GetLteRlcSapProvider ();
  ueParams.srb1SapProvider = 0;
  m_rrcSapUser->Setup (ueParams);

  // CCCH (LCID 0) is pre-configured, here is the hardcoded configuration:
  LteUeCmacSapProvider::LogicalChannelConfig lcConfig;
  lcConfig.priority = 0; // highest priority
  lcConfig.prioritizedBitRateKbps = 65535; // maximum
  lcConfig.bucketSizeDurationMs = 65535; // maximum
  lcConfig.logicalChannelGroup = 0; // all SRBs mapped to LCG 0
  LteMacSapUser* msu = m_ccmRrcSapProvider->ConfigureSignalBearer (lcid, lcConfig, rlc->GetLteMacSapUser ());
  m_cmacSapProvider.at (0)->AddLc (lcid, lcConfig, msu);
}

void
LteUeRrc::InitializeSap (void)
{
  if (m_numberOfComponentCarriers < MIN_NO_CC || m_numberOfComponentCarriers > MAX_NO_CC)
    {
      // this check is needed in order to maintain backward compatibility with scripts and tests
      // if case lte-helper is not used (like in several tests) the m_numberOfComponentCarriers
      // is not set and then an error is raised
      // In this case m_numberOfComponentCarriers is set to 1
      m_numberOfComponentCarriers = MIN_NO_CC;
    }
  if (m_numberOfComponentCarriers > MIN_NO_CC )
    {
      for ( uint16_t i = 1; i < m_numberOfComponentCarriers; i++)
        {
          m_cphySapUser.push_back (new MemberLteUeCphySapUser<LteUeRrc> (this));
          m_cmacSapUser.push_back (new UeMemberLteUeCmacSapUser (this));
          m_cphySapProvider.push_back (0);
          m_cmacSapProvider.push_back (0);
        }
    }
}


void
LteUeRrc::DoSendData (Ptr<Packet> packet, uint8_t bid)
{
  NS_LOG_FUNCTION (this << packet);

  uint8_t drbid = Bid2Drbid (bid);

  if (drbid != 0)
    {
      std::map<uint8_t, Ptr<LteDataRadioBearerInfo> >::iterator it =   m_drbMap.find (drbid);
      NS_ASSERT_MSG (it != m_drbMap.end (), "could not find bearer with drbid == " << drbid);

      LtePdcpSapProvider::TransmitPdcpSduParameters params;
      params.pdcpSdu = packet;
      params.rnti = m_rnti;
      params.lcid = it->second->m_logicalChannelIdentity;
      params.dstL2Id = 0; //Only used for Sidelink
      params.srcL2Id = 0; //Only used for Sidelink
      params.sduType = LtePdcpSapUser::IP_SDU;

      NS_LOG_LOGIC (this << " RNTI=" << m_rnti << " sending packet " << packet
                         << " on DRBID " << (uint32_t) drbid
                         << " (LCID " << (uint32_t) params.lcid << ")"
                         << " (" << packet->GetSize () << " bytes)");
      it->second->m_pdcp->GetLtePdcpSapProvider ()->TransmitPdcpSdu (params);
    }
}

void
LteUeRrc::DoSendDataToGroup (Ptr<Packet> packet, uint32_t group)
{
  NS_LOG_FUNCTION (this << packet << "for Sidelink group " << group);
  //Find the PDCP for Sidelink transmission
  Ptr<LteSidelinkRadioBearerInfo> slrb = m_sidelinkConfiguration->GetSidelinkRadioBearer (group);
  //the NAS should be aware about the existence of the bearer or not
  NS_ASSERT_MSG (slrb, "could not find Sidelink bearer for group == " << group);

  LtePdcpSapProvider::TransmitPdcpSduParameters params;
  params.pdcpSdu = packet;
  params.rnti = m_rnti;
  params.lcid = slrb->m_logicalChannelIdentity;
  params.dstL2Id = 0; //It is set by PDCP
  params.srcL2Id = 0; //It is set by PDCP
  params.sduType = LtePdcpSapUser::IP_SDU;

  NS_LOG_LOGIC (this << " RNTI=" << m_rnti << " sending packet " << packet
                     << " on SLRBBID " << (uint32_t) group
                     << " (LCID " << (uint32_t) params.lcid << ")"
                     << " (" << packet->GetSize () << " bytes)");
  slrb->m_pdcp->GetLtePdcpSapProvider ()->TransmitPdcpSdu (params);
}

void
LteUeRrc::SendPc5Signaling (Ptr<Packet> packet, uint32_t destination)
{
  NS_LOG_FUNCTION (this << packet << "for sidelink " << destination);
  //Find the PDCP for sidelink transmission
  Ptr<LteSidelinkRadioBearerInfo> slrb = m_sidelinkConfiguration->GetSidelinkRadioBearer (destination);
  //the NAS should be aware about the existence of the bearer or not
  NS_ASSERT_MSG (slrb, "could not find sidelink bearer for destination == " << destination);

  LtePdcpSapProvider::TransmitPdcpSduParameters params;
  params.pdcpSdu = packet;
  params.rnti = m_rnti;
  params.lcid = slrb->m_logicalChannelIdentity;
  params.dstL2Id = 0; //It is set by PDCP
  params.srcL2Id = 0; //It is set by PDCP
  params.sduType = LtePdcpSapUser::PC5_SIGNALING_SDU;

  NS_LOG_LOGIC (this << " RNTI=" << m_rnti << " sending packet " << packet
                     << " on SLRBBID " << (uint32_t) destination
                     << " (LCID " << (uint32_t) params.lcid << ")"
                     << " (" << packet->GetSize () << " bytes)");
  slrb->m_pdcp->GetLtePdcpSapProvider ()->TransmitPdcpSdu (params);
}

void
LteUeRrc::DoDisconnect ()
{
  NS_LOG_FUNCTION (this);

  switch (m_state)
    {
      case IDLE_START:
      case IDLE_CELL_SEARCH:
      case IDLE_WAIT_MIB_SIB1:
      case IDLE_WAIT_MIB:
      case IDLE_WAIT_SIB1:
      case IDLE_CAMPED_NORMALLY:
        NS_LOG_INFO ("already disconnected");
        break;

      case IDLE_WAIT_SIB2:
      case IDLE_CONNECTING:
        NS_FATAL_ERROR ("cannot abort connection setup procedure");
        break;

      case CONNECTED_NORMALLY:
      case CONNECTED_HANDOVER:
      case CONNECTED_PHY_PROBLEM:
      case CONNECTED_REESTABLISHING:
        LeaveConnectedMode ();
        break;

      default: // i.e. IDLE_RANDOM_ACCESS
        NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
        break;
    }
}

void
LteUeRrc::DoReceivePdcpSdu (LtePdcpSapUser::ReceivePdcpSduParameters params)
{
  NS_LOG_FUNCTION (this);

  switch (params.sduType)
    {
    case LtePdcpSapUser::IP_SDU:
      //if PC5 user plane data message, send indication to UE RRC SL to update timer
      if (params.dstL2Id && params.srcL2Id)
        {
          m_sidelinkConfiguration->RecvPc5DataMessage (params.srcL2Id, params.dstL2Id, params.pdcpSdu);
        }
      m_asSapUser->RecvData (params.pdcpSdu);
      break;
    case LtePdcpSapUser::PC5_SIGNALING_SDU:
      //pass to UE RRC SL entity for processing if it is for this node
      if (params.dstL2Id == m_sidelinkConfiguration->GetSourceL2Id ())
        {
          m_sidelinkConfiguration->RecvPc5SignalingMessage (params.srcL2Id, params.dstL2Id, params.pdcpSdu);
        }
      else
        {
          NS_LOG_DEBUG ("Dropping PC5 signaling packet for params.dstL2Id (my L2ID=" << m_sidelinkConfiguration->GetSourceL2Id ());
        }
      break;
    default:
      NS_FATAL_ERROR ("invalid pdcp SDU type " << (uint16_t) params.sduType);
    }
}


void
LteUeRrc::DoSetTemporaryCellRnti (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);
  m_rnti = rnti;
  m_srb0->m_rlc->SetRnti (m_rnti);
  //propagate to the MAC and PHY of primary Carrier (normally the MAC indicates the RNTI when receiving message from the eNodeB)
  m_cmacSapProvider.at (0)->SetRnti (m_rnti);
  m_cphySapProvider.at (0)->SetRnti (m_rnti);
}

void
LteUeRrc::DoNotifyRandomAccessSuccessful ()
{
  NS_LOG_FUNCTION (this << m_imsi << ToString (m_state));
  m_randomAccessSuccessfulTrace (m_imsi, m_cellId, m_rnti);

  switch (m_state)
    {
      case IDLE_RANDOM_ACCESS:
        {
          // we just received a RAR with a T-C-RNTI and an UL grant
          // send RRC connection request as message 3 of the random access procedure
          SwitchToState (IDLE_CONNECTING);
          LteRrcSap::RrcConnectionRequest msg;
          msg.ueIdentity = m_imsi;
          m_rrcSapUser->SendRrcConnectionRequest (msg);
          m_connectionTimeout = Simulator::Schedule (m_t300,
                                                     &LteUeRrc::ConnectionTimeout,
                                                     this);
        }
        break;

      case CONNECTED_HANDOVER:
        {
          LteRrcSap::RrcConnectionReconfigurationCompleted msg;
          msg.rrcTransactionIdentifier = m_lastRrcTransactionIdentifier;
          m_rrcSapUser->SendRrcConnectionReconfigurationCompleted (msg);

          // 3GPP TS 36.331 section 5.5.6.1 Measurements related actions upon handover
          std::map<uint8_t, LteRrcSap::MeasIdToAddMod>::iterator measIdIt;
          for (measIdIt = m_varMeasConfig.measIdList.begin ();
               measIdIt != m_varMeasConfig.measIdList.end ();
               ++measIdIt)
            {
              VarMeasReportListClear (measIdIt->second.measId);
            }
          //if target cell sending SIB 18 or 19 and SidelinkUEInformation was sent in the last second
          //with interest/resource request,
          //initiate transmission of the SidelinkUEInformation message in accordance with 5.X.2.3
          if (m_sidelinkConfiguration != nullptr)
            {
              if ((m_sidelinkConfiguration->IsCellBroadcastingSIB18 (m_cellId)
                   || m_sidelinkConfiguration->IsCellBroadcastingSIB19 (m_cellId))
                  && m_sidelinkConfiguration->GetTimeSinceLastTransmissionOfSidelinkUeInformation () < 1.0)
                {
                  SendSidelinkUeInformation (true, true, true, true);
                }
            }
          SwitchToState (CONNECTED_NORMALLY);
          m_cmacSapProvider.at (0)->NotifyConnectionSuccessful (); //RA successful during handover
          m_handoverEndOkTrace (m_imsi, m_cellId, m_rnti);
        }
        break;

      default:
        NS_FATAL_ERROR ("unexpected event in state " << ToString (m_state));
        break;
    }
}

void
LteUeRrc::DoNotifyRandomAccessFailed ()
{
  NS_LOG_FUNCTION (this << m_imsi << ToString (m_state));
  m_randomAccessErrorTrace (m_imsi, m_cellId, m_rnti);

  switch (m_state)
    {
      case IDLE_RANDOM_ACCESS:
        {
          SwitchToState (IDLE_CAMPED_NORMALLY);
          m_asSapUser->NotifyConnectionFailed ();
        }
        break;

      case CONNECTED_HANDOVER:
        {
          m_handoverEndErrorTrace (m_imsi, m_cellId, m_rnti);
          /**
           * \todo After a handover failure because of a random access failure,
           *       send an RRC Connection Re-establishment and switch to
           *       CONNECTED_REESTABLISHING state.
           */
        }
        break;

      default:
        NS_FATAL_ERROR ("unexpected event in state " << ToString (m_state));
        break;
    }
}


void
LteUeRrc::DoSetCsgWhiteList (uint32_t csgId)
{
  NS_LOG_FUNCTION (this << m_imsi << csgId);
  m_csgWhiteList = csgId;
}

void
LteUeRrc::DoStartCellSelection (uint32_t dlEarfcn)
{
  NS_LOG_FUNCTION (this << m_imsi << dlEarfcn);
  NS_ASSERT_MSG (m_state == IDLE_START,
                 "cannot start cell selection from state " << ToString (m_state));
  m_dlEarfcn = dlEarfcn;
  m_cphySapProvider.at (0)->StartCellSearch (dlEarfcn);
  SwitchToState (IDLE_CELL_SEARCH);
}

void
LteUeRrc::DoForceCampedOnEnb (uint16_t cellId, uint32_t dlEarfcn)
{
  NS_LOG_FUNCTION (this << m_imsi << cellId << dlEarfcn);

  switch (m_state)
    {
      case IDLE_START:
        m_cellId = cellId;
        m_dlEarfcn = dlEarfcn;
        m_cphySapProvider.at (0)->SynchronizeWithEnb (m_cellId, m_dlEarfcn);
        SwitchToState (IDLE_WAIT_MIB);
        break;

      case IDLE_CELL_SEARCH:
      case IDLE_WAIT_MIB_SIB1:
      case IDLE_WAIT_SIB1:
        NS_FATAL_ERROR ("cannot abort cell selection " << ToString (m_state));
        break;

      case IDLE_WAIT_MIB:
        NS_LOG_INFO ("already forced to camp to cell " << m_cellId);
        break;

      case IDLE_CAMPED_NORMALLY:
      case IDLE_WAIT_SIB2:
      case IDLE_RANDOM_ACCESS:
      case IDLE_CONNECTING:
        NS_LOG_INFO ("already camped to cell " << m_cellId);
        break;

      case CONNECTED_NORMALLY:
      case CONNECTED_HANDOVER:
      case CONNECTED_PHY_PROBLEM:
      case CONNECTED_REESTABLISHING:
        NS_LOG_INFO ("already connected to cell " << m_cellId);
        break;

      default:
        NS_FATAL_ERROR ("unexpected event in state " << ToString (m_state));
        break;
    }

}

void
LteUeRrc::DoConnect ()
{
  NS_LOG_FUNCTION (this << m_imsi);

  switch (m_state)
    {
      case IDLE_START:
      case IDLE_CELL_SEARCH:
      case IDLE_WAIT_MIB_SIB1:
      case IDLE_WAIT_SIB1:
      case IDLE_WAIT_MIB:
        m_connectionPending = true;
        break;

      case IDLE_CAMPED_NORMALLY:
        m_connectionPending = true;
        SwitchToState (IDLE_WAIT_SIB2);
        break;

      case IDLE_WAIT_SIB2:
      case IDLE_RANDOM_ACCESS:
      case IDLE_CONNECTING:
        NS_LOG_INFO ("already connecting");
        break;

      case CONNECTED_NORMALLY:
      case CONNECTED_REESTABLISHING:
      case CONNECTED_HANDOVER:
        NS_LOG_INFO ("already connected");
        break;

      default:
        NS_FATAL_ERROR ("unexpected event in state " << ToString (m_state));
        break;
    }
}



// CPHY SAP methods

void
LteUeRrc::DoRecvMasterInformationBlock (uint16_t cellId,
                                        LteRrcSap::MasterInformationBlock msg)
{
  m_dlBandwidth = msg.dlBandwidth;
  m_cphySapProvider.at (0)->SetDlBandwidth (msg.dlBandwidth);
  m_hasReceivedMib = true;
  m_mibReceivedTrace (m_imsi, m_cellId, m_rnti, cellId);

  switch (m_state)
    {
      case IDLE_WAIT_MIB:
        // manual attachment
        SwitchToState (IDLE_CAMPED_NORMALLY);
        break;

      case IDLE_WAIT_MIB_SIB1:
        // automatic attachment from Idle mode cell selection
        SwitchToState (IDLE_WAIT_SIB1);
        break;

      default:
        // do nothing extra
        break;
    }
}

void
LteUeRrc::DoRecvSystemInformationBlockType1 (uint16_t cellId,
                                             LteRrcSap::SystemInformationBlockType1 msg)
{
  NS_LOG_FUNCTION (this);
  switch (m_state)
    {
      case IDLE_WAIT_SIB1:
        NS_ASSERT_MSG (cellId == msg.cellAccessRelatedInfo.cellIdentity,
                       "Cell identity in SIB1 does not match with the originating cell");
        m_hasReceivedSib1 = true;
        m_lastSib1 = msg;
        m_sib1ReceivedTrace (m_imsi, m_cellId, m_rnti, cellId);
        EvaluateCellForSelection ();
        break;

      case IDLE_CAMPED_NORMALLY:
      case IDLE_RANDOM_ACCESS:
      case IDLE_CONNECTING:
      case CONNECTED_NORMALLY:
      case CONNECTED_HANDOVER:
      case CONNECTED_PHY_PROBLEM:
      case CONNECTED_REESTABLISHING:
        NS_ASSERT_MSG (cellId == msg.cellAccessRelatedInfo.cellIdentity,
                       "Cell identity in SIB1 does not match with the originating cell");
        m_hasReceivedSib1 = true;
        m_lastSib1 = msg;
        m_sib1ReceivedTrace (m_imsi, m_cellId, m_rnti, cellId);
        break;

      case IDLE_WAIT_MIB_SIB1:
        // MIB has not been received, so ignore this SIB1
        break;

      default: // e.g. IDLE_START, IDLE_CELL_SEARCH, IDLE_WAIT_MIB, IDLE_WAIT_SIB2
        // do nothing
        break;
    }
}

void
LteUeRrc::DoReportUeMeasurements (LteUeCphySapUser::UeMeasurementsParameters params)
{
  NS_LOG_FUNCTION (this);

  // layer 3 filtering does not apply in IDLE mode
  bool useLayer3Filtering = (m_state == CONNECTED_NORMALLY);
  bool triggering = true;
  std::vector <LteUeCphySapUser::UeMeasurementsElement>::iterator newMeasIt;
  for (newMeasIt = params.m_ueMeasurementsList.begin ();
       newMeasIt != params.m_ueMeasurementsList.end (); ++newMeasIt)
    {
      if (params.m_componentCarrierId != 0)
        {
          triggering = false; // report is triggered only when an event is on the primary carrier
          // in this case the measurement received is related to secondary carriers
        }
      SaveUeMeasurements (newMeasIt->m_cellId, newMeasIt->m_rsrp,
                          newMeasIt->m_rsrq, useLayer3Filtering,
                          params.m_componentCarrierId);
    }

  if (m_state == IDLE_CELL_SEARCH)
    {
      // start decoding BCH
      SynchronizeToStrongestCell ();
    }
  else
    {
      if (triggering)
        {
          std::map<uint8_t, LteRrcSap::MeasIdToAddMod>::iterator measIdIt;
          for (measIdIt = m_varMeasConfig.measIdList.begin ();
               measIdIt != m_varMeasConfig.measIdList.end (); ++measIdIt)
            {
              MeasurementReportTriggering (measIdIt->first);
            }
        }
    }

} // end of LteUeRrc::DoReportUeMeasurements



// RRC SAP methods

void
LteUeRrc::DoCompleteSetup (LteUeRrcSapProvider::CompleteSetupParameters params)
{
  NS_LOG_FUNCTION (this << " RNTI " << m_rnti);
  m_srb0->m_rlc->SetLteRlcSapUser (params.srb0SapUser);
  if (m_srb1)
    {
      m_srb1->m_pdcp->SetLtePdcpSapUser (params.srb1SapUser);
    }
}


void
LteUeRrc::DoRecvSystemInformation (LteRrcSap::SystemInformation msg)
{
  NS_LOG_FUNCTION (this << " RNTI " << m_rnti);

  if (msg.haveSib2)
    {
      switch (m_state)
        {
          case IDLE_CAMPED_NORMALLY:
          case IDLE_WAIT_SIB2:
          case IDLE_RANDOM_ACCESS:
          case IDLE_CONNECTING:
          case CONNECTED_NORMALLY:
          case CONNECTED_HANDOVER:
          case CONNECTED_PHY_PROBLEM:
          case CONNECTED_REESTABLISHING:
            m_hasReceivedSib2 = true;
            m_ulBandwidth = msg.sib2.freqInfo.ulBandwidth;
            m_ulEarfcn = msg.sib2.freqInfo.ulCarrierFreq;
            m_sib2ReceivedTrace (m_imsi, m_cellId, m_rnti);
            LteUeCmacSapProvider::RachConfig rc;
            rc.numberOfRaPreambles = msg.sib2.radioResourceConfigCommon.rachConfigCommon.preambleInfo.numberOfRaPreambles;
            rc.preambleTransMax = msg.sib2.radioResourceConfigCommon.rachConfigCommon.raSupervisionInfo.preambleTransMax;
            rc.raResponseWindowSize = msg.sib2.radioResourceConfigCommon.rachConfigCommon.raSupervisionInfo.raResponseWindowSize;
            rc.connEstFailCount = msg.sib2.radioResourceConfigCommon.rachConfigCommon.txFailParam.connEstFailCount;
            m_connEstFailCountLimit = rc.connEstFailCount;
            NS_ASSERT_MSG (m_connEstFailCountLimit > 0 && m_connEstFailCountLimit < 5,
                           "SIB2 msg contains wrong value "
                           << m_connEstFailCountLimit << "of connEstFailCount");
            m_cmacSapProvider.at (0)->ConfigureRach (rc);
            m_cphySapProvider.at (0)->ConfigureUplink (m_ulEarfcn, m_ulBandwidth);
            m_cphySapProvider.at (0)->ConfigureReferenceSignalPower (msg.sib2.radioResourceConfigCommon.pdschConfigCommon.referenceSignalPower);
            if (m_state == IDLE_WAIT_SIB2)
              {
                NS_ASSERT (m_connectionPending);
                StartConnection ();
              }
            break;

          default: // IDLE_START, IDLE_CELL_SEARCH, IDLE_WAIT_MIB, IDLE_WAIT_MIB_SIB1, IDLE_WAIT_SIB1
            // do nothing
            break;
        }
    }

  if (msg.haveSib18 && m_sidelinkConfiguration != nullptr && m_sidelinkConfiguration->IsSlEnabled ())
    {
      switch (m_state)
        {
        case IDLE_CAMPED_NORMALLY:
        case CONNECTED_NORMALLY:
          {
            NS_LOG_LOGIC (this << " processing SIB18 in state " << m_state);
            //update Sidelink information for the cell
            std::map <uint16_t, LteSlUeRrc::LteSlCellConfiguration>::iterator it = m_sidelinkConfiguration->m_slMap.find (m_cellId);
            if (it == m_sidelinkConfiguration->m_slMap.end ())
              {
                NS_LOG_LOGIC (this << " adding SIB18 for cell " << m_cellId);
                LteSlUeRrc::LteSlCellConfiguration cellConfig;
                cellConfig.cellId = m_cellId;
                cellConfig.haveSib18 = true;
                cellConfig.sib18 = msg.sib18;
                cellConfig.haveSib19 = false;
                m_sidelinkConfiguration->m_slMap.insert (std::pair<uint16_t, LteSlUeRrc::LteSlCellConfiguration> (m_cellId, cellConfig));
              }
            else
              {
                NS_LOG_LOGIC (this << " updating SIB18 for cell " << m_cellId);
                it->second.haveSib18 = true;
                it->second.sib18 = msg.sib18;
              }
            if (m_sidelinkConfiguration->IsTxInterested () || m_sidelinkConfiguration->IsRxInterested ())
              {
                //Inform MAC and PHY about the pools to be used
                std::list< Ptr<SidelinkRxCommResourcePool> > pools;
                for (int i = 0; i < msg.sib18.commConfig.commRxPool.nbPools; i++)
                  {
                    Ptr<SidelinkRxCommResourcePool> pool = nullptr;
                    for (uint32_t j = 0; j < m_sidelinkConfiguration->rxPools.size () && pool == nullptr; j++)
                      {
                        if (msg.sib18.commConfig.commRxPool.pools[i] == m_sidelinkConfiguration->rxPools.at (j).first)
                          {
                            pool = m_sidelinkConfiguration->rxPools.at (j).second;
                          }
                      }
                    if (pool == nullptr)
                      {
                        pool = CreateObject <SidelinkRxCommResourcePool> ();
                        pool->SetPool (msg.sib18.commConfig.commRxPool.pools[i]);
                        m_sidelinkConfiguration->rxPools.push_back (std::make_pair (msg.sib18.commConfig.commRxPool.pools[i], pool));
                      }
                    pools.push_back (pool);
                  }
                m_cmacSapProvider.at (0)->SetSlCommRxPools (pools);
                m_cphySapProvider.at (0)->SetSlCommRxPools (pools);
              }
          }
          break;
        default: // IDLE_START, IDLE_CELL_SEARCH, IDLE_WAIT_MIB, IDLE_WAIT_MIB_SIB1, IDLE_WAIT_SIB1
          // do nothing
          break;
        }//end switch
    }//end if

  if (msg.haveSib19 && m_sidelinkConfiguration != nullptr && m_sidelinkConfiguration->IsDiscEnabled ())
    {
      switch (m_state)
        {
        case IDLE_CAMPED_NORMALLY:
        case CONNECTED_NORMALLY:
          {
            NS_LOG_LOGIC (this << " processing SIB19 in state " << m_state);
            //update Sidelink information for the cell
            std::map <uint16_t, LteSlUeRrc::LteSlCellConfiguration>::iterator it = m_sidelinkConfiguration->m_slMap.find (m_cellId);
            if (it == m_sidelinkConfiguration->m_slMap.end ())
              {
                NS_LOG_LOGIC (this << " adding SIB19 for cell " << m_cellId);
                LteSlUeRrc::LteSlCellConfiguration cellConfig;
                cellConfig.cellId = m_cellId;
                cellConfig.haveSib19 = true;
                cellConfig.sib19 = msg.sib19;
                cellConfig.haveSib18 = false;
                m_sidelinkConfiguration->m_slMap.insert (std::pair<uint16_t, LteSlUeRrc::LteSlCellConfiguration> (m_cellId, cellConfig));
              }
            else
              {
                NS_LOG_LOGIC (this << " updating SIB19 for cell " << m_cellId);
                it->second.haveSib19 = true;
                it->second.sib19 = msg.sib19;
              }
            if (m_sidelinkConfiguration->IsMonitoringInterested () || m_sidelinkConfiguration->IsAnnouncingInterested ())
              {
                std::list< Ptr<SidelinkRxDiscResourcePool> > pools;
                for (int i = 0; i < msg.sib19.discConfig.discRxPool.nbPools; i++)
                  {
                    Ptr<SidelinkRxDiscResourcePool> pool = nullptr;
                    for (uint32_t j = 0; j < m_sidelinkConfiguration->monitorPools.size () && pool == nullptr; j++)
                      {
                        if (msg.sib19.discConfig.discRxPool.pools[i] == m_sidelinkConfiguration->monitorPools.at (j).first)
                          {
                            pool = m_sidelinkConfiguration->monitorPools.at (j).second;
                          }
                      }
                    if (pool == nullptr)
                      {
                        pool = CreateObject <SidelinkRxDiscResourcePool> ();
                        pool->SetPool (msg.sib19.discConfig.discRxPool.pools[i]);
                        m_sidelinkConfiguration->monitorPools.push_back (std::make_pair (msg.sib19.discConfig.discRxPool.pools[i], pool));
                      }
                    pools.push_back (pool);
                  }
                m_cmacSapProvider.at (0)->SetSlDiscRxPools (pools);
                m_cphySapProvider.at (0)->SetSlDiscRxPools (pools);
              }
          }//end case
          break;
        default: // IDLE_START, IDLE_CELL_SEARCH, IDLE_WAIT_MIB, IDLE_WAIT_MIB_SIB1, IDLE_WAIT_SIB1
          // do nothing
          break;
        }//end switch
    }//end if
}

void
LteUeRrc::DoRecvRrcConnectionSetup (LteRrcSap::RrcConnectionSetup msg)
{
  NS_LOG_FUNCTION (this << " RNTI " << m_rnti);
  switch (m_state)
    {
      case IDLE_CONNECTING:
        {
          ApplyRadioResourceConfigDedicated (msg.radioResourceConfigDedicated);
          m_connEstFailCount = 0;
          m_connectionTimeout.Cancel ();
          SwitchToState (CONNECTED_NORMALLY);
          m_leaveConnectedMode = false;
          LteRrcSap::RrcConnectionSetupCompleted msg2;
          msg2.rrcTransactionIdentifier = msg.rrcTransactionIdentifier;
          m_rrcSapUser->SendRrcConnectionSetupCompleted (msg2);
          m_asSapUser->NotifyConnectionSuccessful ();
          m_cmacSapProvider.at (0)->NotifyConnectionSuccessful ();
          m_connectionEstablishedTrace (m_imsi, m_cellId, m_rnti);
          NS_ABORT_MSG_IF (m_noOfSyncIndications > 0, "Sync indications should be zero "
                           "when a new RRC connection is established. Current value = " << (uint16_t) m_noOfSyncIndications);
        }
        break;

      default:
        NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
        break;
    }
}

void
LteUeRrc::DoRecvRrcConnectionReconfiguration (LteRrcSap::RrcConnectionReconfiguration msg)
{
  NS_LOG_FUNCTION (this << " RNTI " << m_rnti);
  NS_LOG_INFO ("DoRecvRrcConnectionReconfiguration haveNonCriticalExtension:" << msg.haveNonCriticalExtension);
  switch (m_state)
    {
      case CONNECTED_NORMALLY:
        if (msg.haveMobilityControlInfo)
          {
            NS_LOG_INFO ("haveMobilityControlInfo == true");
            SwitchToState (CONNECTED_HANDOVER);
            if (m_radioLinkFailureDetected.IsRunning ())
              {
                ResetRlfParams ();
              }
            const LteRrcSap::MobilityControlInfo& mci = msg.mobilityControlInfo;
            m_handoverStartTrace (m_imsi, m_cellId, m_rnti, mci.targetPhysCellId);
            //We should reset the MACs and PHYs for all the component carriers
            for (auto cmacSapProvider: m_cmacSapProvider)
              {
                cmacSapProvider->Reset ();
              }
            for (auto cphySapProvider: m_cphySapProvider)
              {
                cphySapProvider->Reset ();
              }
            m_ccmRrcSapProvider->Reset ();
            StorePreviousCellId (m_cellId);
            m_cellId = mci.targetPhysCellId;
            NS_ASSERT (mci.haveCarrierFreq);
            NS_ASSERT (mci.haveCarrierBandwidth);
            m_cphySapProvider.at (0)->SynchronizeWithEnb (m_cellId, mci.carrierFreq.dlCarrierFreq);
            m_cphySapProvider.at (0)->SetDlBandwidth ( mci.carrierBandwidth.dlBandwidth);
            m_cphySapProvider.at (0)->ConfigureUplink (mci.carrierFreq.ulCarrierFreq, mci.carrierBandwidth.ulBandwidth);
            m_rnti = msg.mobilityControlInfo.newUeIdentity;
            m_srb0->m_rlc->SetRnti (m_rnti);
            NS_ASSERT_MSG (mci.haveRachConfigDedicated, "handover is only supported with non-contention-based random access procedure");
            m_cmacSapProvider.at (0)->StartNonContentionBasedRandomAccessProcedure (m_rnti, mci.rachConfigDedicated.raPreambleIndex, mci.rachConfigDedicated.raPrachMaskIndex);
            m_cphySapProvider.at (0)->SetRnti (m_rnti);
            m_lastRrcTransactionIdentifier = msg.rrcTransactionIdentifier;
            NS_ASSERT (msg.haveRadioResourceConfigDedicated);

            // we re-establish SRB1 by creating a new entity
            // note that we can't dispose the old entity now, because
            // it's in the current stack, so we would corrupt the stack
            // if we did so. Hence we schedule it for later disposal
            m_srb1Old = m_srb1;
            Simulator::ScheduleNow (&LteUeRrc::DisposeOldSrb1, this);
            m_srb1 = 0; // new instance will be be created within ApplyRadioResourceConfigDedicated

            m_drbMap.clear (); // dispose all DRBs
            ApplyRadioResourceConfigDedicated (msg.radioResourceConfigDedicated);
            if (msg.haveNonCriticalExtension)
              {
                NS_LOG_DEBUG (this << "RNTI " << m_rnti << " Handover. Configuring secondary carriers");
                ApplyRadioResourceConfigDedicatedSecondaryCarrier (msg.nonCriticalExtension);
              }

            if (msg.haveMeasConfig)
              {
                ApplyMeasConfig (msg.measConfig);
              }
            // RRC connection reconfiguration completed will be sent
            // after handover is complete
          }
        else
          {
            NS_LOG_INFO ("haveMobilityControlInfo == false");
            if (msg.haveNonCriticalExtension)
              {
                ApplyRadioResourceConfigDedicatedSecondaryCarrier (msg.nonCriticalExtension);
                NS_LOG_DEBUG (this << "RNTI " << m_rnti << " Configured for CA" );
              }
            if (msg.haveRadioResourceConfigDedicated)
              {
                ApplyRadioResourceConfigDedicated (msg.radioResourceConfigDedicated);
              }
            if (msg.haveMeasConfig)
              {
                ApplyMeasConfig (msg.measConfig);
              }
            //check if it has Sidelink resource information
            if (msg.haveSlCommConfig)
              {
                ApplySidelinkDedicatedConfiguration (msg.slCommConfig);
              }
            //check if it has discovery resource information
            if (msg.haveSlDiscConfig)
              {
                ApplySidelinkDedicatedConfiguration (msg.slDiscConfig);
              }
            LteRrcSap::RrcConnectionReconfigurationCompleted msg2;
            msg2.rrcTransactionIdentifier = msg.rrcTransactionIdentifier;
            m_rrcSapUser->SendRrcConnectionReconfigurationCompleted (msg2);
            m_connectionReconfigurationTrace (m_imsi, m_cellId, m_rnti);
          }
        break;

      default:
        NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
        break;
    }
}

void
LteUeRrc::DoRecvRrcConnectionReestablishment (LteRrcSap::RrcConnectionReestablishment msg)
{
  NS_LOG_FUNCTION (this << " RNTI " << m_rnti);
  switch (m_state)
    {
      case CONNECTED_REESTABLISHING:
        {
          /**
           * \todo After receiving RRC Connection Re-establishment, stop timer
           *       T301, fire a new trace source, reply with RRC Connection
           *       Re-establishment Complete, and finally switch to
           *       CONNECTED_NORMALLY state. See Section 5.3.7.5 of 3GPP TS
           *       36.331.
           */
          //if target cell sending SIB 18 or 19 and SidelinkUEInformation was sent in the last second
          //with interest/resource request,
          //initiate transmission of the SidelinkUEInformation message in accordance with 5.X.2.3
          if ((m_sidelinkConfiguration->IsCellBroadcastingSIB18 (m_cellId)
               || m_sidelinkConfiguration->IsCellBroadcastingSIB19 (m_cellId))
              && m_sidelinkConfiguration->GetTimeSinceLastTransmissionOfSidelinkUeInformation () < 1.0)
            {
              SendSidelinkUeInformation (true, true, true, true);
            }
        }
        break;

      default:
        NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
        break;
    }
}

void
LteUeRrc::DoRecvRrcConnectionReestablishmentReject (LteRrcSap::RrcConnectionReestablishmentReject msg)
{
  NS_LOG_FUNCTION (this << " RNTI " << m_rnti);
  switch (m_state)
    {
      case CONNECTED_REESTABLISHING:
        {
          /**
           * \todo After receiving RRC Connection Re-establishment Reject, stop
           *       timer T301. See Section 5.3.7.8 of 3GPP TS 36.331.
           */
          m_asSapUser->NotifyConnectionReleased (); // Inform upper layers
        }
        break;

      default:
        NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
        break;
    }
}

void
LteUeRrc::DoRecvRrcConnectionRelease (LteRrcSap::RrcConnectionRelease msg)
{
  NS_LOG_FUNCTION (this << " RNTI " << m_rnti);
  /// \todo Currently not implemented, see Section 5.3.8 of 3GPP TS 36.331.
}

void
LteUeRrc::DoRecvRrcConnectionReject (LteRrcSap::RrcConnectionReject msg)
{
  NS_LOG_FUNCTION (this);
  m_connectionTimeout.Cancel ();
  for (uint16_t i = 0; i < m_numberOfComponentCarriers; i++)
    {
      m_cmacSapProvider.at (i)->Reset (); // reset the MAC
    }
  m_hasReceivedSib2 = false;         // invalidate the previously received SIB2
  SwitchToState (IDLE_CAMPED_NORMALLY);
  m_asSapUser->NotifyConnectionFailed ();  // inform upper layer
}

void
LteUeRrc::DoSetNumberOfComponentCarriers (uint16_t noOfComponentCarriers)
{
  NS_LOG_FUNCTION (this);
  m_numberOfComponentCarriers = noOfComponentCarriers;
}



void
LteUeRrc::SynchronizeToStrongestCell ()
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_state == IDLE_CELL_SEARCH);

  uint16_t maxRsrpCellId = 0;
  double maxRsrp = -std::numeric_limits<double>::infinity ();
  double minRsrp = -140.0; // Minimum RSRP in dBm a UE can report

  std::map<uint16_t, MeasValues>::iterator it;
  for (it = m_storedMeasValues.begin (); it != m_storedMeasValues.end (); it++)
    {
      /*
       * This block attempts to find a cell with strongest RSRP and has not
       * yet been identified as "acceptable cell".
       */
      if (maxRsrp < it->second.rsrp && it->second.rsrp > minRsrp)
        {
          std::set<uint16_t>::const_iterator itCell;
          itCell = m_acceptableCell.find (it->first);
          if (itCell == m_acceptableCell.end ())
            {
              maxRsrpCellId = it->first;
              maxRsrp = it->second.rsrp;
            }
        }
    }

  if (maxRsrpCellId == 0)
    {
      NS_LOG_WARN (this << " Cell search is unable to detect surrounding cell to attach to");
    }
  else
    {
      NS_LOG_LOGIC (this << " cell " << maxRsrpCellId
                         << " is the strongest untried surrounding cell");
      m_cphySapProvider.at (0)->SynchronizeWithEnb (maxRsrpCellId, m_dlEarfcn);
      SwitchToState (IDLE_WAIT_MIB_SIB1);
    }

} // end of void LteUeRrc::SynchronizeToStrongestCell ()


void
LteUeRrc::EvaluateCellForSelection ()
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_state == IDLE_WAIT_SIB1);
  NS_ASSERT (m_hasReceivedMib);
  NS_ASSERT (m_hasReceivedSib1);
  uint16_t cellId = m_lastSib1.cellAccessRelatedInfo.cellIdentity;

  // Cell selection criteria evaluation

  bool isSuitableCell = false;
  bool isAcceptableCell = false;
  std::map<uint16_t, MeasValues>::iterator storedMeasIt = m_storedMeasValues.find (cellId);
  double qRxLevMeas = storedMeasIt->second.rsrp;
  double qRxLevMin = EutranMeasurementMapping::IeValue2ActualQRxLevMin (m_lastSib1.cellSelectionInfo.qRxLevMin);
  NS_LOG_LOGIC (this << " cell selection to cellId=" << cellId
                     << " qrxlevmeas=" << qRxLevMeas << " dBm"
                     << " qrxlevmin=" << qRxLevMin << " dBm");

  if (qRxLevMeas - qRxLevMin > 0)
    {
      isAcceptableCell = true;

      uint32_t cellCsgId = m_lastSib1.cellAccessRelatedInfo.csgIdentity;
      bool cellCsgIndication = m_lastSib1.cellAccessRelatedInfo.csgIndication;

      isSuitableCell = (cellCsgIndication == false) || (cellCsgId == m_csgWhiteList);

      NS_LOG_LOGIC (this << " csg(ue/cell/indication)=" << m_csgWhiteList << "/"
                         << cellCsgId << "/" << cellCsgIndication);
    }

  // Cell selection decision

  if (isSuitableCell)
    {
      m_cellId = cellId;
      m_cphySapProvider.at (0)->SynchronizeWithEnb (cellId, m_dlEarfcn);
      m_cphySapProvider.at (0)->SetDlBandwidth (m_dlBandwidth);
      m_initialCellSelectionEndOkTrace (m_imsi, cellId);
      // Once the UE is connected, m_connectionPending is
      // set to false. So, when RLF occurs and UE performs
      // cell selection upon leaving RRC_CONNECTED state,
      // the following call to DoConnect will make the
      // m_connectionPending to be true again. Thus,
      // upon calling SwitchToState (IDLE_CAMPED_NORMALLY)
      // UE state is instantly change to IDLE_WAIT_SIB2.
      // This will make the UE to read the SIB2 message
      // and start random access.
      if (!m_connectionPending)
        {
          NS_LOG_DEBUG ("Calling DoConnect in state = " << ToString (m_state));
          DoConnect ();
        }
      SwitchToState (IDLE_CAMPED_NORMALLY);
    }
  else
    {
      // ignore the MIB and SIB1 received from this cell
      m_hasReceivedMib = false;
      m_hasReceivedSib1 = false;

      m_initialCellSelectionEndErrorTrace (m_imsi, cellId);

      if (isAcceptableCell)
        {
          /*
           * The cells inserted into this list will not be considered for
           * subsequent cell search attempt.
           */
          m_acceptableCell.insert (cellId);
        }

      SwitchToState (IDLE_CELL_SEARCH);
      SynchronizeToStrongestCell (); // retry to a different cell
    }

} // end of void LteUeRrc::EvaluateCellForSelection ()


void
LteUeRrc::ApplyRadioResourceConfigDedicatedSecondaryCarrier (LteRrcSap::NonCriticalExtensionConfiguration nonCec)
{
  NS_LOG_FUNCTION (this);

  m_sCellToAddModList = nonCec.sCellToAddModList;

  for (uint32_t sCellIndex: nonCec.sCellToReleaseList)
    {
      m_cphySapProvider.at (sCellIndex)->Reset ();
      m_cmacSapProvider.at (sCellIndex)->Reset ();
    }

  for(auto &scell: nonCec.sCellToAddModList)
    {
      uint8_t ccId = scell.sCellIndex;


      uint16_t physCellId = scell.cellIdentification.physCellId;
      uint16_t ulBand = scell.radioResourceConfigCommonSCell.ulConfiguration.ulFreqInfo.ulBandwidth;
      uint32_t ulEarfcn = scell.radioResourceConfigCommonSCell.ulConfiguration.ulFreqInfo.ulCarrierFreq;
      uint16_t dlBand = scell.radioResourceConfigCommonSCell.nonUlConfiguration.dlBandwidth;
      uint32_t dlEarfcn = scell.cellIdentification.dlCarrierFreq;
      uint8_t txMode = scell.radioResourceConfigDedicatedSCell.physicalConfigDedicatedSCell.antennaInfo.transmissionMode;
      uint16_t srsIndex = scell.radioResourceConfigDedicatedSCell.physicalConfigDedicatedSCell.soundingRsUlConfigDedicated.srsConfigIndex;

      m_cphySapProvider.at (ccId)->SynchronizeWithEnb (physCellId, dlEarfcn);
      m_cphySapProvider.at (ccId)->SetDlBandwidth (dlBand);
      m_cphySapProvider.at (ccId)->ConfigureUplink (ulEarfcn, ulBand);
      m_cphySapProvider.at (ccId)->ConfigureReferenceSignalPower (scell.radioResourceConfigCommonSCell.nonUlConfiguration.pdschConfigCommon.referenceSignalPower);
      m_cphySapProvider.at (ccId)->SetTransmissionMode (txMode);
      m_cphySapProvider.at (ccId)->SetRnti (m_rnti);
      m_cmacSapProvider.at (ccId)->SetRnti (m_rnti);
      // update PdschConfigDedicated (i.e. P_A value)
      LteRrcSap::PdschConfigDedicated pdschConfigDedicated = scell.radioResourceConfigDedicatedSCell.physicalConfigDedicatedSCell.pdschConfigDedicated;
      double paDouble = LteRrcSap::ConvertPdschConfigDedicated2Double (pdschConfigDedicated);
      m_cphySapProvider.at (ccId)->SetPa (paDouble);
      m_cphySapProvider.at (ccId)->SetSrsConfigurationIndex (srsIndex);
    }

  m_sCarrierConfiguredTrace (this, m_sCellToAddModList);
}

void
LteUeRrc::ApplyRadioResourceConfigDedicated (LteRrcSap::RadioResourceConfigDedicated rrcd)
{
  NS_LOG_FUNCTION (this);
  const struct LteRrcSap::PhysicalConfigDedicated& pcd = rrcd.physicalConfigDedicated;

  if (pcd.haveAntennaInfoDedicated)
    {
      m_cphySapProvider.at (0)->SetTransmissionMode (pcd.antennaInfo.transmissionMode);
    }
  if (pcd.haveSoundingRsUlConfigDedicated)
    {
      m_cphySapProvider.at (0)->SetSrsConfigurationIndex (pcd.soundingRsUlConfigDedicated.srsConfigIndex);
    }

  if (pcd.havePdschConfigDedicated)
    {
      // update PdschConfigDedicated (i.e. P_A value)
      m_pdschConfigDedicated = pcd.pdschConfigDedicated;
      double paDouble = LteRrcSap::ConvertPdschConfigDedicated2Double (m_pdschConfigDedicated);
      m_cphySapProvider.at (0)->SetPa (paDouble);
    }

  std::list<LteRrcSap::SrbToAddMod>::const_iterator stamIt = rrcd.srbToAddModList.begin ();
  if (stamIt != rrcd.srbToAddModList.end ())
    {
      if (m_srb1 == 0)
        {
          // SRB1 not setup yet
          NS_ASSERT_MSG ((m_state == IDLE_CONNECTING) || (m_state == CONNECTED_HANDOVER),
                         "unexpected state " << ToString (m_state));
          NS_ASSERT_MSG (stamIt->srbIdentity == 1, "only SRB1 supported");

          const uint8_t lcid = 1; // fixed LCID for SRB1

          Ptr<LteRlc> rlc = CreateObject<LteRlcAm> ();
          rlc->SetLteMacSapProvider (m_macSapProvider);
          rlc->SetRnti (m_rnti);
          rlc->SetLcId (lcid);

          Ptr<LtePdcp> pdcp = CreateObject<LtePdcp> ();
          pdcp->SetRnti (m_rnti);
          pdcp->SetLcId (lcid);
          pdcp->SetLtePdcpSapUser (m_drbPdcpSapUser);
          pdcp->SetLteRlcSapProvider (rlc->GetLteRlcSapProvider ());
          rlc->SetLteRlcSapUser (pdcp->GetLteRlcSapUser ());

          m_srb1 = CreateObject<LteSignalingRadioBearerInfo> ();
          m_srb1->m_rlc = rlc;
          m_srb1->m_pdcp = pdcp;
          m_srb1->m_srbIdentity = 1;
          m_srb1CreatedTrace (m_imsi, m_cellId, m_rnti);

          m_srb1->m_logicalChannelConfig.priority = stamIt->logicalChannelConfig.priority;
          m_srb1->m_logicalChannelConfig.prioritizedBitRateKbps = stamIt->logicalChannelConfig.prioritizedBitRateKbps;
          m_srb1->m_logicalChannelConfig.bucketSizeDurationMs = stamIt->logicalChannelConfig.bucketSizeDurationMs;
          m_srb1->m_logicalChannelConfig.logicalChannelGroup = stamIt->logicalChannelConfig.logicalChannelGroup;

          struct LteUeCmacSapProvider::LogicalChannelConfig lcConfig;
          lcConfig.priority = stamIt->logicalChannelConfig.priority;
          lcConfig.prioritizedBitRateKbps = stamIt->logicalChannelConfig.prioritizedBitRateKbps;
          lcConfig.bucketSizeDurationMs = stamIt->logicalChannelConfig.bucketSizeDurationMs;
          lcConfig.logicalChannelGroup = stamIt->logicalChannelConfig.logicalChannelGroup;
          LteMacSapUser* msu = m_ccmRrcSapProvider->ConfigureSignalBearer (lcid, lcConfig, rlc->GetLteMacSapUser ());
          m_cmacSapProvider.at (0)->AddLc (lcid, lcConfig, msu);
          ++stamIt;
          NS_ASSERT_MSG (stamIt == rrcd.srbToAddModList.end (), "at most one SrbToAdd supported");

          LteUeRrcSapUser::SetupParameters ueParams;
          ueParams.srb0SapProvider = m_srb0->m_rlc->GetLteRlcSapProvider ();
          ueParams.srb1SapProvider = m_srb1->m_pdcp->GetLtePdcpSapProvider ();
          m_rrcSapUser->Setup (ueParams);
        }
      else
        {
          NS_LOG_INFO ("request to modify SRB1 (skipping as currently not implemented)");
          // would need to modify m_srb1, and then propagate changes to the MAC
        }
    }


  std::list<LteRrcSap::DrbToAddMod>::const_iterator dtamIt;
  for (dtamIt = rrcd.drbToAddModList.begin ();
       dtamIt != rrcd.drbToAddModList.end ();
       ++dtamIt)
    {
      NS_LOG_INFO (this << " IMSI " << m_imsi << " adding/modifying DRBID " << (uint32_t) dtamIt->drbIdentity << " LC " << (uint32_t) dtamIt->logicalChannelIdentity);
      NS_ASSERT_MSG (dtamIt->logicalChannelIdentity > 2, "LCID value " << dtamIt->logicalChannelIdentity << " is reserved for SRBs");

      std::map<uint8_t, Ptr<LteDataRadioBearerInfo> >::iterator drbMapIt = m_drbMap.find (dtamIt->drbIdentity);
      if (drbMapIt == m_drbMap.end ())
        {
          NS_LOG_INFO ("New Data Radio Bearer");

          TypeId rlcTypeId;
          if (m_useRlcSm)
            {
              rlcTypeId = LteRlcSm::GetTypeId ();
            }
          else
            {
              switch (dtamIt->rlcConfig.choice)
                {
                  case LteRrcSap::RlcConfig::AM:
                    rlcTypeId = LteRlcAm::GetTypeId ();
                    break;

                  case LteRrcSap::RlcConfig::UM_BI_DIRECTIONAL:
                    rlcTypeId = LteRlcUm::GetTypeId ();
                    break;

                  default:
                    NS_FATAL_ERROR ("unsupported RLC configuration");
                    break;
                }
            }

          ObjectFactory rlcObjectFactory;
          rlcObjectFactory.SetTypeId (rlcTypeId);
          Ptr<LteRlc> rlc = rlcObjectFactory.Create ()->GetObject<LteRlc> ();
          rlc->SetLteMacSapProvider (m_macSapProvider);
          rlc->SetRnti (m_rnti);
          rlc->SetLcId (dtamIt->logicalChannelIdentity);

          Ptr<LteDataRadioBearerInfo> drbInfo = CreateObject<LteDataRadioBearerInfo> ();
          drbInfo->m_rlc = rlc;
          drbInfo->m_epsBearerIdentity = dtamIt->epsBearerIdentity;
          drbInfo->m_logicalChannelIdentity = dtamIt->logicalChannelIdentity;
          drbInfo->m_drbIdentity = dtamIt->drbIdentity;

          // we need PDCP only for real RLC, i.e., RLC/UM or RLC/AM
          // if we are using RLC/SM we don't care of anything above RLC
          if (rlcTypeId != LteRlcSm::GetTypeId ())
            {
              Ptr<LtePdcp> pdcp = CreateObject<LtePdcp> ();
              pdcp->SetRnti (m_rnti);
              pdcp->SetLcId (dtamIt->logicalChannelIdentity);
              pdcp->SetLtePdcpSapUser (m_drbPdcpSapUser);
              pdcp->SetLteRlcSapProvider (rlc->GetLteRlcSapProvider ());
              rlc->SetLteRlcSapUser (pdcp->GetLteRlcSapUser ());
              drbInfo->m_pdcp = pdcp;
            }

          m_bid2DrbidMap[dtamIt->epsBearerIdentity] = dtamIt->drbIdentity;

          m_drbMap.insert (std::pair<uint8_t, Ptr<LteDataRadioBearerInfo> > (dtamIt->drbIdentity, drbInfo));

          m_drbCreatedTrace (m_imsi, m_cellId, m_rnti, dtamIt->drbIdentity);


          struct LteUeCmacSapProvider::LogicalChannelConfig lcConfig;
          lcConfig.priority = dtamIt->logicalChannelConfig.priority;
          lcConfig.prioritizedBitRateKbps = dtamIt->logicalChannelConfig.prioritizedBitRateKbps;
          lcConfig.bucketSizeDurationMs = dtamIt->logicalChannelConfig.bucketSizeDurationMs;
          lcConfig.logicalChannelGroup = dtamIt->logicalChannelConfig.logicalChannelGroup;

          NS_LOG_DEBUG (this << " UE RRC RNTI " << m_rnti << " Number Of Component Carriers " << m_numberOfComponentCarriers << " lcID " << (uint16_t) dtamIt->logicalChannelIdentity);
          //Call AddLc of UE component carrier manager
          std::vector <LteUeCcmRrcSapProvider::LcsConfig> lcOnCcMapping = m_ccmRrcSapProvider->AddLc (dtamIt->logicalChannelIdentity, lcConfig, rlc->GetLteMacSapUser ());

          NS_LOG_DEBUG ("Size of lcOnCcMapping vector " << lcOnCcMapping.size ());
          std::vector<LteUeCcmRrcSapProvider::LcsConfig>::iterator itLcOnCcMapping = lcOnCcMapping.begin ();
          NS_ASSERT_MSG (itLcOnCcMapping != lcOnCcMapping.end (), "Component carrier manager failed to add LC for data radio bearer");

          for (itLcOnCcMapping = lcOnCcMapping.begin (); itLcOnCcMapping != lcOnCcMapping.end (); ++itLcOnCcMapping)
            {
              NS_LOG_DEBUG ("RNTI " << m_rnti
                                    << " LCG id " << (uint16_t) itLcOnCcMapping->lcConfig.logicalChannelGroup
                                    << " ComponentCarrierId " << (uint16_t) itLcOnCcMapping->componentCarrierId);
              uint8_t index = itLcOnCcMapping->componentCarrierId;
              LteUeCmacSapProvider::LogicalChannelConfig lcConfigFromCcm = itLcOnCcMapping->lcConfig;
              LteMacSapUser *msu = itLcOnCcMapping->msu;
              m_cmacSapProvider.at (index)->AddLc (dtamIt->logicalChannelIdentity, lcConfigFromCcm, msu);
            }

          rlc->Initialize ();
        }
      else
        {
          NS_LOG_INFO ("request to modify existing DRBID");
          Ptr<LteDataRadioBearerInfo> drbInfo = drbMapIt->second;
          /// \todo currently not implemented. Would need to modify drbInfo, and then propagate changes to the MAC
        }
    }

  std::list<uint8_t>::iterator dtdmIt;
  for (dtdmIt = rrcd.drbToReleaseList.begin ();
       dtdmIt != rrcd.drbToReleaseList.end ();
       ++dtdmIt)
    {
      uint8_t drbid = *dtdmIt;
      NS_LOG_INFO (this << " IMSI " << m_imsi << " releasing DRB " << (uint32_t) drbid);
      std::map<uint8_t, Ptr<LteDataRadioBearerInfo> >::iterator it =   m_drbMap.find (drbid);
      NS_ASSERT_MSG (it != m_drbMap.end (), "could not find bearer with given lcid");
      m_drbMap.erase (it);
      m_bid2DrbidMap.erase (drbid);
      //Remove LCID
      for (uint32_t i = 0; i < m_numberOfComponentCarriers; i++)
        {
          m_cmacSapProvider.at (i)->RemoveLc (drbid + 2);
        }
    }
}


void
LteUeRrc::ApplyMeasConfig (LteRrcSap::MeasConfig mc)
{
  NS_LOG_FUNCTION (this);

  // perform the actions specified in 3GPP TS 36.331 section 5.5.2.1

  // 3GPP TS 36.331 section 5.5.2.4 Measurement object removal
  for (std::list<uint8_t>::iterator it = mc.measObjectToRemoveList.begin ();
       it !=  mc.measObjectToRemoveList.end ();
       ++it)
    {
      uint8_t measObjectId = *it;
      NS_LOG_LOGIC (this << " deleting measObjectId " << (uint32_t)  measObjectId);
      m_varMeasConfig.measObjectList.erase (measObjectId);
      std::map<uint8_t, LteRrcSap::MeasIdToAddMod>::iterator measIdIt = m_varMeasConfig.measIdList.begin ();
      while (measIdIt != m_varMeasConfig.measIdList.end ())
        {
          if (measIdIt->second.measObjectId == measObjectId)
            {
              uint8_t measId = measIdIt->second.measId;
              NS_ASSERT (measId == measIdIt->first);
              NS_LOG_LOGIC (this << " deleting measId " << (uint32_t) measId << " because referring to measObjectId " << (uint32_t)  measObjectId);
              // note: postfix operator preserves iterator validity
              m_varMeasConfig.measIdList.erase (measIdIt++);
              VarMeasReportListClear (measId);
            }
          else
            {
              ++measIdIt;
            }
        }

    }

  // 3GPP TS 36.331 section 5.5.2.5  Measurement object addition/ modification
  for (std::list<LteRrcSap::MeasObjectToAddMod>::iterator it = mc.measObjectToAddModList.begin ();
       it !=  mc.measObjectToAddModList.end ();
       ++it)
    {
      // simplifying assumptions
      NS_ASSERT_MSG (it->measObjectEutra.cellsToRemoveList.empty (), "cellsToRemoveList not supported");
      NS_ASSERT_MSG (it->measObjectEutra.cellsToAddModList.empty (), "cellsToAddModList not supported");
      NS_ASSERT_MSG (it->measObjectEutra.cellsToRemoveList.empty (), "blackCellsToRemoveList not supported");
      NS_ASSERT_MSG (it->measObjectEutra.blackCellsToAddModList.empty (), "blackCellsToAddModList not supported");
      NS_ASSERT_MSG (it->measObjectEutra.haveCellForWhichToReportCGI == false, "cellForWhichToReportCGI is not supported");

      uint8_t measObjectId = it->measObjectId;
      std::map<uint8_t, LteRrcSap::MeasObjectToAddMod>::iterator measObjectIt = m_varMeasConfig.measObjectList.find (measObjectId);
      if (measObjectIt != m_varMeasConfig.measObjectList.end ())
        {
          NS_LOG_LOGIC ("measObjectId " << (uint32_t) measObjectId << " exists, updating entry");
          measObjectIt->second = *it;
          for (std::map<uint8_t, LteRrcSap::MeasIdToAddMod>::iterator measIdIt
                 = m_varMeasConfig.measIdList.begin ();
               measIdIt != m_varMeasConfig.measIdList.end ();
               ++measIdIt)
            {
              if (measIdIt->second.measObjectId == measObjectId)
                {
                  uint8_t measId = measIdIt->second.measId;
                  NS_LOG_LOGIC (this << " found measId " << (uint32_t) measId << " referring to measObjectId " << (uint32_t)  measObjectId);
                  VarMeasReportListClear (measId);
                }
            }
        }
      else
        {
          NS_LOG_LOGIC ("measObjectId " << (uint32_t) measObjectId << " is new, adding entry");
          m_varMeasConfig.measObjectList[measObjectId] = *it;
        }

    }

  // 3GPP TS 36.331 section 5.5.2.6 Reporting configuration removal
  for (std::list<uint8_t>::iterator it = mc.reportConfigToRemoveList.begin ();
       it !=  mc.reportConfigToRemoveList.end ();
       ++it)
    {
      uint8_t reportConfigId = *it;
      NS_LOG_LOGIC (this << " deleting reportConfigId " << (uint32_t)  reportConfigId);
      m_varMeasConfig.reportConfigList.erase (reportConfigId);
      std::map<uint8_t, LteRrcSap::MeasIdToAddMod>::iterator measIdIt = m_varMeasConfig.measIdList.begin ();
      while (measIdIt != m_varMeasConfig.measIdList.end ())
        {
          if (measIdIt->second.reportConfigId == reportConfigId)
            {
              uint8_t measId = measIdIt->second.measId;
              NS_ASSERT (measId == measIdIt->first);
              NS_LOG_LOGIC (this << " deleting measId " << (uint32_t) measId << " because referring to reportConfigId " << (uint32_t)  reportConfigId);
              // note: postfix operator preserves iterator validity
              m_varMeasConfig.measIdList.erase (measIdIt++);
              VarMeasReportListClear (measId);
            }
          else
            {
              ++measIdIt;
            }
        }

    }

  // 3GPP TS 36.331 section 5.5.2.7 Reporting configuration addition/ modification
  for (std::list<LteRrcSap::ReportConfigToAddMod>::iterator it = mc.reportConfigToAddModList.begin ();
       it !=  mc.reportConfigToAddModList.end ();
       ++it)
    {
      // simplifying assumptions
      NS_ASSERT_MSG (it->reportConfigEutra.triggerType == LteRrcSap::ReportConfigEutra::EVENT,
                     "only trigger type EVENT is supported");

      uint8_t reportConfigId = it->reportConfigId;
      std::map<uint8_t, LteRrcSap::ReportConfigToAddMod>::iterator reportConfigIt = m_varMeasConfig.reportConfigList.find (reportConfigId);
      if (reportConfigIt != m_varMeasConfig.reportConfigList.end ())
        {
          NS_LOG_LOGIC ("reportConfigId " << (uint32_t) reportConfigId << " exists, updating entry");
          m_varMeasConfig.reportConfigList[reportConfigId] = *it;
          for (std::map<uint8_t, LteRrcSap::MeasIdToAddMod>::iterator measIdIt
                 = m_varMeasConfig.measIdList.begin ();
               measIdIt != m_varMeasConfig.measIdList.end ();
               ++measIdIt)
            {
              if (measIdIt->second.reportConfigId == reportConfigId)
                {
                  uint8_t measId = measIdIt->second.measId;
                  NS_LOG_LOGIC (this << " found measId " << (uint32_t) measId << " referring to reportConfigId " << (uint32_t)  reportConfigId);
                  VarMeasReportListClear (measId);
                }
            }
        }
      else
        {
          NS_LOG_LOGIC ("reportConfigId " << (uint32_t) reportConfigId << " is new, adding entry");
          m_varMeasConfig.reportConfigList[reportConfigId] = *it;
        }

    }

  // 3GPP TS 36.331 section 5.5.2.8 Quantity configuration
  if (mc.haveQuantityConfig)
    {
      NS_LOG_LOGIC (this << " setting quantityConfig");
      m_varMeasConfig.quantityConfig = mc.quantityConfig;
      //Convey the filter coefficient to PHY layer so it can configure the power control parameter
      for (uint16_t i = 0; i < m_numberOfComponentCarriers; i++)
        {
          m_cphySapProvider.at (i)->SetRsrpFilterCoefficient (mc.quantityConfig.filterCoefficientRSRP);
        }
      // we calculate here the coefficient a used for Layer 3 filtering, see 3GPP TS 36.331 section 5.5.3.2
      m_varMeasConfig.aRsrp = std::pow (0.5, mc.quantityConfig.filterCoefficientRSRP / 4.0);
      m_varMeasConfig.aRsrq = std::pow (0.5, mc.quantityConfig.filterCoefficientRSRQ / 4.0);
      NS_LOG_LOGIC (this << " new filter coefficients: aRsrp=" << m_varMeasConfig.aRsrp << ", aRsrq=" << m_varMeasConfig.aRsrq);

      for (std::map<uint8_t, LteRrcSap::MeasIdToAddMod>::iterator measIdIt
             = m_varMeasConfig.measIdList.begin ();
           measIdIt != m_varMeasConfig.measIdList.end ();
           ++measIdIt)
        {
          VarMeasReportListClear (measIdIt->second.measId);
        }
    }

  // 3GPP TS 36.331 section 5.5.2.2 Measurement identity removal
  for (std::list<uint8_t>::iterator it = mc.measIdToRemoveList.begin ();
       it !=  mc.measIdToRemoveList.end ();
       ++it)
    {
      uint8_t measId = *it;
      NS_LOG_LOGIC (this << " deleting measId " << (uint32_t) measId);
      m_varMeasConfig.measIdList.erase (measId);
      VarMeasReportListClear (measId);

      // removing time-to-trigger queues
      m_enteringTriggerQueue.erase (measId);
      m_leavingTriggerQueue.erase (measId);
    }

  // 3GPP TS 36.331 section 5.5.2.3 Measurement identity addition/ modification
  for (std::list<LteRrcSap::MeasIdToAddMod>::iterator it = mc.measIdToAddModList.begin ();
       it !=  mc.measIdToAddModList.end ();
       ++it)
    {
      NS_LOG_LOGIC (this << " measId " << (uint32_t) it->measId
                         << " (measObjectId=" << (uint32_t) it->measObjectId
                         << ", reportConfigId=" << (uint32_t) it->reportConfigId
                         << ")");
      NS_ASSERT (m_varMeasConfig.measObjectList.find (it->measObjectId)
                 != m_varMeasConfig.measObjectList.end ());
      NS_ASSERT (m_varMeasConfig.reportConfigList.find (it->reportConfigId)
                 != m_varMeasConfig.reportConfigList.end ());
      m_varMeasConfig.measIdList[it->measId] = *it; // side effect: create new entry if not exists
      std::map<uint8_t, VarMeasReport>::iterator measReportIt = m_varMeasReportList.find (it->measId);
      if (measReportIt != m_varMeasReportList.end ())
        {
          measReportIt->second.periodicReportTimer.Cancel ();
          m_varMeasReportList.erase (measReportIt);
        }
      NS_ASSERT (m_varMeasConfig.reportConfigList.find (it->reportConfigId)
                 ->second.reportConfigEutra.triggerType != LteRrcSap::ReportConfigEutra::PERIODICAL);

      // new empty queues for time-to-trigger
      std::list<PendingTrigger_t> s;
      m_enteringTriggerQueue[it->measId] = s;
      m_leavingTriggerQueue[it->measId] = s;
    }

  if (mc.haveMeasGapConfig)
    {
      NS_FATAL_ERROR ("measurement gaps are currently not supported");
    }

  if (mc.haveSmeasure)
    {
      NS_FATAL_ERROR ("s-measure is currently not supported");
    }

  if (mc.haveSpeedStatePars)
    {
      NS_FATAL_ERROR ("SpeedStatePars are currently not supported");
    }
}

void
LteUeRrc::SaveUeMeasurements (uint16_t cellId, double rsrp, double rsrq,
                              bool useLayer3Filtering, uint8_t componentCarrierId)
{
  NS_LOG_FUNCTION (this << cellId << +componentCarrierId << rsrp << rsrq << useLayer3Filtering);

  std::map<uint16_t, MeasValues>::iterator storedMeasIt = m_storedMeasValues.find (cellId);

  if (storedMeasIt != m_storedMeasValues.end ())
    {
      if (useLayer3Filtering)
        {
          // F_n = (1-a) F_{n-1} + a M_n
          storedMeasIt->second.rsrp = (1 - m_varMeasConfig.aRsrp) * storedMeasIt->second.rsrp
            + m_varMeasConfig.aRsrp * rsrp;

          if (std::isnan (storedMeasIt->second.rsrq))
            {
              // the previous RSRQ measurements provided UE PHY are invalid
              storedMeasIt->second.rsrq = rsrq; // replace it with unfiltered value
            }
          else
            {
              storedMeasIt->second.rsrq = (1 - m_varMeasConfig.aRsrq) * storedMeasIt->second.rsrq
                + m_varMeasConfig.aRsrq * rsrq;
            }
        }
      else
        {
          storedMeasIt->second.rsrp = rsrp;
          storedMeasIt->second.rsrq = rsrq;
        }
    }
  else
    {
      // first value is always unfiltered
      MeasValues v;
      v.rsrp = rsrp;
      v.rsrq = rsrq;
      v.carrierFreq = m_cphySapProvider.at (componentCarrierId)->GetDlEarfcn ();

      std::pair<uint16_t, MeasValues> val (cellId, v);
      std::pair<std::map<uint16_t, MeasValues>::iterator, bool>
      ret = m_storedMeasValues.insert (val);
      NS_ASSERT_MSG (ret.second == true, "element already existed");
      storedMeasIt = ret.first;
    }

  NS_LOG_DEBUG (this << " IMSI " << m_imsi << " state " << ToString (m_state)
                     << ", measured cell " << cellId
                     << ", carrier component Id " << componentCarrierId
                     << ", new RSRP " << rsrp << " stored " << storedMeasIt->second.rsrp
                     << ", new RSRQ " << rsrq << " stored " << storedMeasIt->second.rsrq);

  m_rsrpTrace (m_imsi, m_cellId, storedMeasIt->second.rsrp );

} // end of void SaveUeMeasurements

void
LteUeRrc::MeasurementReportTriggering (uint8_t measId)
{
  NS_LOG_FUNCTION (this << (uint16_t) measId);

  std::map<uint8_t, LteRrcSap::MeasIdToAddMod>::iterator measIdIt =
    m_varMeasConfig.measIdList.find (measId);
  NS_ASSERT (measIdIt != m_varMeasConfig.measIdList.end ());
  NS_ASSERT (measIdIt->first == measIdIt->second.measId);

  std::map<uint8_t, LteRrcSap::ReportConfigToAddMod>::iterator
    reportConfigIt = m_varMeasConfig.reportConfigList.find (measIdIt->second.reportConfigId);
  NS_ASSERT (reportConfigIt != m_varMeasConfig.reportConfigList.end ());
  LteRrcSap::ReportConfigEutra& reportConfigEutra = reportConfigIt->second.reportConfigEutra;

  std::map<uint8_t, LteRrcSap::MeasObjectToAddMod>::iterator
    measObjectIt = m_varMeasConfig.measObjectList.find (measIdIt->second.measObjectId);
  NS_ASSERT (measObjectIt != m_varMeasConfig.measObjectList.end ());
  LteRrcSap::MeasObjectEutra& measObjectEutra = measObjectIt->second.measObjectEutra;

  std::map<uint8_t, VarMeasReport>::iterator
    measReportIt = m_varMeasReportList.find (measId);
  bool isMeasIdInReportList = (measReportIt != m_varMeasReportList.end ());

  // we don't check the purpose field, as it is only included for
  // triggerType == periodical, which is not supported
  NS_ASSERT_MSG (reportConfigEutra.triggerType
                 == LteRrcSap::ReportConfigEutra::EVENT,
                 "only triggerType == event is supported");
  // only EUTRA is supported, no need to check for it

  NS_LOG_LOGIC (this << " considering measId " << (uint32_t) measId);
  bool eventEntryCondApplicable = false;
  bool eventLeavingCondApplicable = false;
  ConcernedCells_t concernedCellsEntry;
  ConcernedCells_t concernedCellsLeaving;

  /*
   * Find which serving cell corresponds to measObjectEutra.carrierFreq
   * It is used, for example, by A1 event:
   * See TS 36.331 5.5.4.2: "for this measurement, consider the primary or
   * secondary cell that is configured on the frequency indicated in the
   * associated measObjectEUTRA to be the serving cell"
   */
  uint16_t servingCellId = 0;
  for (auto cphySapProvider: m_cphySapProvider)
    {
      if (cphySapProvider->GetDlEarfcn () == measObjectEutra.carrierFreq)
        {
          servingCellId = cphySapProvider->GetCellId ();
        }
    }

  if (servingCellId == 0)
    {
      return;
    }

  switch (reportConfigEutra.eventId)
    {
    case LteRrcSap::ReportConfigEutra::EVENT_A1:
      {
        /*
         * Event A1 (Serving becomes better than threshold)
         * Please refer to 3GPP TS 36.331 Section 5.5.4.2
         */

        double ms; // Ms, the measurement result of the serving cell
        double thresh; // Thresh, the threshold parameter for this event
        // Hys, the hysteresis parameter for this event.
        double hys = EutranMeasurementMapping::IeValue2ActualHysteresis (reportConfigEutra.hysteresis);

        switch (reportConfigEutra.triggerQuantity)
          {
          case LteRrcSap::ReportConfigEutra::RSRP:
            ms = m_storedMeasValues[servingCellId].rsrp;

            NS_ASSERT (reportConfigEutra.threshold1.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRP);
            thresh = EutranMeasurementMapping::RsrpRange2Dbm (reportConfigEutra.threshold1.range);
            break;
          case LteRrcSap::ReportConfigEutra::RSRQ:
            ms = m_storedMeasValues[servingCellId].rsrq;
            NS_ASSERT (reportConfigEutra.threshold1.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRQ);
            thresh = EutranMeasurementMapping::RsrqRange2Db (reportConfigEutra.threshold1.range);
            break;
          default:
            NS_FATAL_ERROR ("unsupported triggerQuantity");
            break;
          }

        // Inequality A1-1 (Entering condition): Ms - Hys > Thresh
        bool entryCond = ms - hys > thresh;

        if (entryCond)
          {
            if (!isMeasIdInReportList)
              {
                concernedCellsEntry.push_back (servingCellId);
                eventEntryCondApplicable = true;
              }
            else
              {
                /*
                 * This is to check that the triggered cell recorded in the
                 * VarMeasReportList is the serving cell.
                 */
                NS_ASSERT (measReportIt->second.cellsTriggeredList.find (servingCellId)
                           != measReportIt->second.cellsTriggeredList.end ());
              }
          }
        else if (reportConfigEutra.timeToTrigger > 0)
          {
            CancelEnteringTrigger (measId);
          }

        // Inequality A1-2 (Leaving condition): Ms + Hys < Thresh
        bool leavingCond = ms + hys < thresh;

        if (leavingCond)
          {
            if (isMeasIdInReportList)
              {
                /*
                 * This is to check that the triggered cell recorded in the
                 * VarMeasReportList is the serving cell.
                 */
                NS_ASSERT (measReportIt->second.cellsTriggeredList.find (m_cellId)
                           != measReportIt->second.cellsTriggeredList.end ());
                concernedCellsLeaving.push_back (m_cellId);
                eventLeavingCondApplicable = true;
              }
          }
        else if (reportConfigEutra.timeToTrigger > 0)
          {
            CancelLeavingTrigger (measId);
          }

        NS_LOG_LOGIC (this << " event A1: serving cell " << servingCellId
                           << " ms=" << ms << " thresh=" << thresh
                           << " entryCond=" << entryCond
                           << " leavingCond=" << leavingCond);

      } // end of case LteRrcSap::ReportConfigEutra::EVENT_A1

      break;

    case LteRrcSap::ReportConfigEutra::EVENT_A2:
      {
        /*
         * Event A2 (Serving becomes worse than threshold)
         * Please refer to 3GPP TS 36.331 Section 5.5.4.3
         */

        double ms; // Ms, the measurement result of the serving cell
        double thresh; // Thresh, the threshold parameter for this event
        // Hys, the hysteresis parameter for this event.
        double hys = EutranMeasurementMapping::IeValue2ActualHysteresis (reportConfigEutra.hysteresis);

        switch (reportConfigEutra.triggerQuantity)
          {
          case LteRrcSap::ReportConfigEutra::RSRP:
            ms = m_storedMeasValues[servingCellId].rsrp;
            NS_ASSERT (reportConfigEutra.threshold1.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRP);
            thresh = EutranMeasurementMapping::RsrpRange2Dbm (reportConfigEutra.threshold1.range);
            break;
          case LteRrcSap::ReportConfigEutra::RSRQ:
            ms = m_storedMeasValues[servingCellId].rsrq;
            NS_ASSERT (reportConfigEutra.threshold1.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRQ);
            thresh = EutranMeasurementMapping::RsrqRange2Db (reportConfigEutra.threshold1.range);
            break;
          default:
            NS_FATAL_ERROR ("unsupported triggerQuantity");
            break;
          }

        // Inequality A2-1 (Entering condition): Ms + Hys < Thresh
        bool entryCond = ms + hys < thresh;

        if (entryCond)
          {
            if (!isMeasIdInReportList)
              {
                concernedCellsEntry.push_back (servingCellId);
                eventEntryCondApplicable = true;
              }
            else
              {
                /*
                 * This is to check that the triggered cell recorded in the
                 * VarMeasReportList is the serving cell.
                 */
                NS_ASSERT (measReportIt->second.cellsTriggeredList.find (servingCellId)
                           != measReportIt->second.cellsTriggeredList.end ());
              }
          }
        else if (reportConfigEutra.timeToTrigger > 0)
          {
            CancelEnteringTrigger (measId);
          }

        // Inequality A2-2 (Leaving condition): Ms - Hys > Thresh
        bool leavingCond = ms - hys > thresh;

        if (leavingCond)
          {
            if (isMeasIdInReportList)
              {
                /*
                 * This is to check that the triggered cell recorded in the
                 * VarMeasReportList is the serving cell.
                 */
                NS_ASSERT (measReportIt->second.cellsTriggeredList.find (servingCellId)
                           != measReportIt->second.cellsTriggeredList.end ());
                concernedCellsLeaving.push_back (servingCellId);
                eventLeavingCondApplicable = true;
              }
          }
        else if (reportConfigEutra.timeToTrigger > 0)
          {
            CancelLeavingTrigger (measId);
          }

        NS_LOG_LOGIC (this << " event A2: serving cell " << servingCellId
                           << " ms=" << ms << " thresh=" << thresh
                           << " entryCond=" << entryCond
                           << " leavingCond=" << leavingCond);

      } // end of case LteRrcSap::ReportConfigEutra::EVENT_A2

      break;

    case LteRrcSap::ReportConfigEutra::EVENT_A3:
      {
        /*
         * Event A3 (Neighbour becomes offset better than PCell)
         * Please refer to 3GPP TS 36.331 Section 5.5.4.4
         */

        double mn; // Mn, the measurement result of the neighbouring cell
        double ofn = measObjectEutra.offsetFreq; // Ofn, the frequency specific offset of the frequency of the
        double ocn = 0.0; // Ocn, the cell specific offset of the neighbour cell
        double mp; // Mp, the measurement result of the PCell
        double ofp = measObjectEutra.offsetFreq; // Ofp, the frequency specific offset of the primary frequency
        double ocp = 0.0; // Ocp, the cell specific offset of the PCell
        // Off, the offset parameter for this event.
        double off = EutranMeasurementMapping::IeValue2ActualA3Offset (reportConfigEutra.a3Offset);
        // Hys, the hysteresis parameter for this event.
        double hys = EutranMeasurementMapping::IeValue2ActualHysteresis (reportConfigEutra.hysteresis);

        switch (reportConfigEutra.triggerQuantity)
          {
          case LteRrcSap::ReportConfigEutra::RSRP:
            mp = m_storedMeasValues[m_cellId].rsrp;
            NS_ASSERT (reportConfigEutra.threshold1.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRP);
            break;
          case LteRrcSap::ReportConfigEutra::RSRQ:
            mp = m_storedMeasValues[m_cellId].rsrq;
            NS_ASSERT (reportConfigEutra.threshold1.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRQ);
            break;
          default:
            NS_FATAL_ERROR ("unsupported triggerQuantity");
            break;
          }

        for (std::map<uint16_t, MeasValues>::iterator storedMeasIt = m_storedMeasValues.begin ();
             storedMeasIt != m_storedMeasValues.end ();
             ++storedMeasIt)
          {
            uint16_t cellId = storedMeasIt->first;
            if (cellId == m_cellId)
              {
                continue;
              }

            // Only cell(s) on the frequency indicated in the associated measObject can trigger event.
            if (m_storedMeasValues.at (cellId).carrierFreq != measObjectEutra.carrierFreq)
              {
                continue;
              }

            switch (reportConfigEutra.triggerQuantity)
              {
              case LteRrcSap::ReportConfigEutra::RSRP:
                mn = storedMeasIt->second.rsrp;
                break;
              case LteRrcSap::ReportConfigEutra::RSRQ:
                mn = storedMeasIt->second.rsrq;
                break;
              default:
                NS_FATAL_ERROR ("unsupported triggerQuantity");
                break;
              }

            bool hasTriggered = isMeasIdInReportList
              && (measReportIt->second.cellsTriggeredList.find (cellId)
                  != measReportIt->second.cellsTriggeredList.end ());

            // Inequality A3-1 (Entering condition): Mn + Ofn + Ocn - Hys > Mp + Ofp + Ocp + Off
            bool entryCond = mn + ofn + ocn - hys > mp + ofp + ocp + off;

            if (entryCond)
              {
                if (!hasTriggered)
                  {
                    concernedCellsEntry.push_back (cellId);
                    eventEntryCondApplicable = true;
                  }
              }
            else if (reportConfigEutra.timeToTrigger > 0)
              {
                CancelEnteringTrigger (measId, cellId);
              }

            // Inequality A3-2 (Leaving condition): Mn + Ofn + Ocn + Hys < Mp + Ofp + Ocp + Off
            bool leavingCond = mn + ofn + ocn + hys < mp + ofp + ocp + off;

            if (leavingCond)
              {
                if (hasTriggered)
                  {
                    concernedCellsLeaving.push_back (cellId);
                    eventLeavingCondApplicable = true;
                  }
              }
            else if (reportConfigEutra.timeToTrigger > 0)
              {
                CancelLeavingTrigger (measId, cellId);
              }

            NS_LOG_LOGIC (this << " event A3: neighbor cell " << cellId
                               << " mn=" << mn << " mp=" << mp << " offset=" << off
                               << " entryCond=" << entryCond
                               << " leavingCond=" << leavingCond);

          } // end of for (storedMeasIt)

      } // end of case LteRrcSap::ReportConfigEutra::EVENT_A3

      break;

    case LteRrcSap::ReportConfigEutra::EVENT_A4:
      {
        /*
         * Event A4 (Neighbour becomes better than threshold)
         * Please refer to 3GPP TS 36.331 Section 5.5.4.5
         */

        double mn; // Mn, the measurement result of the neighbouring cell
        double ofn = measObjectEutra.offsetFreq; // Ofn, the frequency specific offset of the frequency of the
        double ocn = 0.0; // Ocn, the cell specific offset of the neighbour cell
        double thresh; // Thresh, the threshold parameter for this event
        // Hys, the hysteresis parameter for this event.
        double hys = EutranMeasurementMapping::IeValue2ActualHysteresis (reportConfigEutra.hysteresis);

        switch (reportConfigEutra.triggerQuantity)
          {
          case LteRrcSap::ReportConfigEutra::RSRP:
            NS_ASSERT (reportConfigEutra.threshold1.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRP);
            thresh = EutranMeasurementMapping::RsrpRange2Dbm (reportConfigEutra.threshold1.range);
            break;
          case LteRrcSap::ReportConfigEutra::RSRQ:
            NS_ASSERT (reportConfigEutra.threshold1.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRQ);
            thresh = EutranMeasurementMapping::RsrqRange2Db (reportConfigEutra.threshold1.range);
            break;
          default:
            NS_FATAL_ERROR ("unsupported triggerQuantity");
            break;
          }

        for (std::map<uint16_t, MeasValues>::iterator storedMeasIt = m_storedMeasValues.begin ();
             storedMeasIt != m_storedMeasValues.end ();
             ++storedMeasIt)
          {
            uint16_t cellId = storedMeasIt->first;
            if (cellId == m_cellId)
              {
                continue;
              }

            switch (reportConfigEutra.triggerQuantity)
              {
              case LteRrcSap::ReportConfigEutra::RSRP:
                mn = storedMeasIt->second.rsrp;
                break;
              case LteRrcSap::ReportConfigEutra::RSRQ:
                mn = storedMeasIt->second.rsrq;
                break;
              default:
                NS_FATAL_ERROR ("unsupported triggerQuantity");
                break;
              }

            bool hasTriggered = isMeasIdInReportList
              && (measReportIt->second.cellsTriggeredList.find (cellId)
                  != measReportIt->second.cellsTriggeredList.end ());

            // Inequality A4-1 (Entering condition): Mn + Ofn + Ocn - Hys > Thresh
            bool entryCond = mn + ofn + ocn - hys > thresh;

            if (entryCond)
              {
                if (!hasTriggered)
                  {
                    concernedCellsEntry.push_back (cellId);
                    eventEntryCondApplicable = true;
                  }
              }
            else if (reportConfigEutra.timeToTrigger > 0)
              {
                CancelEnteringTrigger (measId, cellId);
              }

            // Inequality A4-2 (Leaving condition): Mn + Ofn + Ocn + Hys < Thresh
            bool leavingCond = mn + ofn + ocn + hys < thresh;

            if (leavingCond)
              {
                if (hasTriggered)
                  {
                    concernedCellsLeaving.push_back (cellId);
                    eventLeavingCondApplicable = true;
                  }
              }
            else if (reportConfigEutra.timeToTrigger > 0)
              {
                CancelLeavingTrigger (measId, cellId);
              }

            NS_LOG_LOGIC (this << " event A4: neighbor cell " << cellId
                               << " mn=" << mn << " thresh=" << thresh
                               << " entryCond=" << entryCond
                               << " leavingCond=" << leavingCond);

          } // end of for (storedMeasIt)

      } // end of case LteRrcSap::ReportConfigEutra::EVENT_A4

      break;

    case LteRrcSap::ReportConfigEutra::EVENT_A5:
      {
        /*
         * Event A5 (PCell becomes worse than threshold1 and neighbour
         * becomes better than threshold2)
         * Please refer to 3GPP TS 36.331 Section 5.5.4.6
         */

        double mp; // Mp, the measurement result of the PCell
        double mn; // Mn, the measurement result of the neighbouring cell
        double ofn = measObjectEutra.offsetFreq; // Ofn, the frequency specific offset of the frequency of the
        double ocn = 0.0; // Ocn, the cell specific offset of the neighbour cell
        double thresh1; // Thresh1, the threshold parameter for this event
        double thresh2; // Thresh2, the threshold parameter for this event
        // Hys, the hysteresis parameter for this event.
        double hys = EutranMeasurementMapping::IeValue2ActualHysteresis (reportConfigEutra.hysteresis);

        switch (reportConfigEutra.triggerQuantity)
          {
          case LteRrcSap::ReportConfigEutra::RSRP:
            mp = m_storedMeasValues[m_cellId].rsrp;
            NS_ASSERT (reportConfigEutra.threshold1.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRP);
            NS_ASSERT (reportConfigEutra.threshold2.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRP);
            thresh1 = EutranMeasurementMapping::RsrpRange2Dbm (reportConfigEutra.threshold1.range);
            thresh2 = EutranMeasurementMapping::RsrpRange2Dbm (reportConfigEutra.threshold2.range);
            break;
          case LteRrcSap::ReportConfigEutra::RSRQ:
            mp = m_storedMeasValues[m_cellId].rsrq;
            NS_ASSERT (reportConfigEutra.threshold1.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRQ);
            NS_ASSERT (reportConfigEutra.threshold2.choice
                       == LteRrcSap::ThresholdEutra::THRESHOLD_RSRQ);
            thresh1 = EutranMeasurementMapping::RsrqRange2Db (reportConfigEutra.threshold1.range);
            thresh2 = EutranMeasurementMapping::RsrqRange2Db (reportConfigEutra.threshold2.range);
            break;
          default:
            NS_FATAL_ERROR ("unsupported triggerQuantity");
            break;
          }

        // Inequality A5-1 (Entering condition 1): Mp + Hys < Thresh1
        bool entryCond = mp + hys < thresh1;

        if (entryCond)
          {
            for (std::map<uint16_t, MeasValues>::iterator storedMeasIt = m_storedMeasValues.begin ();
                 storedMeasIt != m_storedMeasValues.end ();
                 ++storedMeasIt)
              {
                uint16_t cellId = storedMeasIt->first;
                if (cellId == m_cellId)
                  {
                    continue;
                  }

                switch (reportConfigEutra.triggerQuantity)
                  {
                  case LteRrcSap::ReportConfigEutra::RSRP:
                    mn = storedMeasIt->second.rsrp;
                    break;
                  case LteRrcSap::ReportConfigEutra::RSRQ:
                    mn = storedMeasIt->second.rsrq;
                    break;
                  default:
                    NS_FATAL_ERROR ("unsupported triggerQuantity");
                    break;
                  }

                bool hasTriggered = isMeasIdInReportList
                  && (measReportIt->second.cellsTriggeredList.find (cellId)
                      != measReportIt->second.cellsTriggeredList.end ());

                // Inequality A5-2 (Entering condition 2): Mn + Ofn + Ocn - Hys > Thresh2

                entryCond = mn + ofn + ocn - hys > thresh2;

                if (entryCond)
                  {
                    if (!hasTriggered)
                      {
                        concernedCellsEntry.push_back (cellId);
                        eventEntryCondApplicable = true;
                      }
                  }
                else if (reportConfigEutra.timeToTrigger > 0)
                  {
                    CancelEnteringTrigger (measId, cellId);
                  }

                NS_LOG_LOGIC (this << " event A5: neighbor cell " << cellId
                                   << " mn=" << mn << " mp=" << mp
                                   << " thresh2=" << thresh2
                                   << " thresh1=" << thresh1
                                   << " entryCond=" << entryCond);

              } // end of for (storedMeasIt)

          } // end of if (entryCond)
        else
          {
            NS_LOG_LOGIC (this << " event A5: serving cell " << m_cellId
                               << " mp=" << mp << " thresh1=" << thresh1
                               << " entryCond=" << entryCond);

            if (reportConfigEutra.timeToTrigger > 0)
              {
                CancelEnteringTrigger (measId);
              }
          }

        if (isMeasIdInReportList)
          {
            // Inequality A5-3 (Leaving condition 1): Mp - Hys > Thresh1
            bool leavingCond = mp - hys > thresh1;

            if (leavingCond)
              {
                if (reportConfigEutra.timeToTrigger == 0)
                  {
                    // leaving condition #2 does not have to be checked

                    for (std::map<uint16_t, MeasValues>::iterator storedMeasIt = m_storedMeasValues.begin ();
                         storedMeasIt != m_storedMeasValues.end ();
                         ++storedMeasIt)
                      {
                        uint16_t cellId = storedMeasIt->first;
                        if (cellId == m_cellId)
                          {
                            continue;
                          }

                        if (measReportIt->second.cellsTriggeredList.find (cellId)
                            != measReportIt->second.cellsTriggeredList.end ())
                          {
                            concernedCellsLeaving.push_back (cellId);
                            eventLeavingCondApplicable = true;
                          }
                      }
                  } // end of if (reportConfigEutra.timeToTrigger == 0)
                else
                  {
                    // leaving condition #2 has to be checked to cancel time-to-trigger

                    for (std::map<uint16_t, MeasValues>::iterator storedMeasIt = m_storedMeasValues.begin ();
                         storedMeasIt != m_storedMeasValues.end ();
                         ++storedMeasIt)
                      {
                        uint16_t cellId = storedMeasIt->first;
                        if (cellId == m_cellId)
                          {
                            continue;
                          }

                        if (measReportIt->second.cellsTriggeredList.find (cellId)
                            != measReportIt->second.cellsTriggeredList.end ())
                          {
                            switch (reportConfigEutra.triggerQuantity)
                              {
                              case LteRrcSap::ReportConfigEutra::RSRP:
                                mn = storedMeasIt->second.rsrp;
                                break;
                              case LteRrcSap::ReportConfigEutra::RSRQ:
                                mn = storedMeasIt->second.rsrq;
                                break;
                              default:
                                NS_FATAL_ERROR ("unsupported triggerQuantity");
                                break;
                              }

                            // Inequality A5-4 (Leaving condition 2): Mn + Ofn + Ocn + Hys < Thresh2

                            leavingCond = mn + ofn + ocn + hys < thresh2;

                            if (!leavingCond)
                              {
                                CancelLeavingTrigger (measId, cellId);
                              }

                            /*
                             * Whatever the result of leaving condition #2, this
                             * cell is still "in", because leaving condition #1
                             * is already true.
                             */
                            concernedCellsLeaving.push_back (cellId);
                            eventLeavingCondApplicable = true;

                            NS_LOG_LOGIC (this << " event A5: neighbor cell " << cellId
                                               << " mn=" << mn << " mp=" << mp
                                               << " thresh2=" << thresh2
                                               << " thresh1=" << thresh1
                                               << " leavingCond=" << leavingCond);

                          } // end of if (measReportIt->second.cellsTriggeredList.find (cellId)
                            //            != measReportIt->second.cellsTriggeredList.end ())

                      } // end of for (storedMeasIt)

                  } // end of else of if (reportConfigEutra.timeToTrigger == 0)

                NS_LOG_LOGIC (this << " event A5: serving cell " << m_cellId
                                   << " mp=" << mp << " thresh1=" << thresh1
                                   << " leavingCond=" << leavingCond);

              } // end of if (leavingCond)
            else
              {
                if (reportConfigEutra.timeToTrigger > 0)
                  {
                    CancelLeavingTrigger (measId);
                  }

                // check leaving condition #2

                for (std::map<uint16_t, MeasValues>::iterator storedMeasIt = m_storedMeasValues.begin ();
                     storedMeasIt != m_storedMeasValues.end ();
                     ++storedMeasIt)
                  {
                    uint16_t cellId = storedMeasIt->first;
                    if (cellId == m_cellId)
                      {
                        continue;
                      }

                    if (measReportIt->second.cellsTriggeredList.find (cellId)
                        != measReportIt->second.cellsTriggeredList.end ())
                      {
                        switch (reportConfigEutra.triggerQuantity)
                          {
                          case LteRrcSap::ReportConfigEutra::RSRP:
                            mn = storedMeasIt->second.rsrp;
                            break;
                          case LteRrcSap::ReportConfigEutra::RSRQ:
                            mn = storedMeasIt->second.rsrq;
                            break;
                          default:
                            NS_FATAL_ERROR ("unsupported triggerQuantity");
                            break;
                          }

                        // Inequality A5-4 (Leaving condition 2): Mn + Ofn + Ocn + Hys < Thresh2
                        leavingCond = mn + ofn + ocn + hys < thresh2;

                        if (leavingCond)
                          {
                            concernedCellsLeaving.push_back (cellId);
                            eventLeavingCondApplicable = true;
                          }

                        NS_LOG_LOGIC (this << " event A5: neighbor cell " << cellId
                                           << " mn=" << mn << " mp=" << mp
                                           << " thresh2=" << thresh2
                                           << " thresh1=" << thresh1
                                           << " leavingCond=" << leavingCond);

                      } // end of if (measReportIt->second.cellsTriggeredList.find (cellId)
                        //            != measReportIt->second.cellsTriggeredList.end ())

                  } // end of for (storedMeasIt)

              } // end of else of if (leavingCond)

          } // end of if (isMeasIdInReportList)

      } // end of case LteRrcSap::ReportConfigEutra::EVENT_A5

      break;

    default:
      NS_FATAL_ERROR ("unsupported eventId " << reportConfigEutra.eventId);
      break;

    } // switch (event type)

  NS_LOG_LOGIC (this << " eventEntryCondApplicable=" << eventEntryCondApplicable
                     << " eventLeavingCondApplicable=" << eventLeavingCondApplicable);

  if (eventEntryCondApplicable)
    {
      if (reportConfigEutra.timeToTrigger == 0)
        {
          VarMeasReportListAdd (measId, concernedCellsEntry);
        }
      else
        {
          PendingTrigger_t t;
          t.measId = measId;
          t.concernedCells = concernedCellsEntry;
          t.timer = Simulator::Schedule (MilliSeconds (reportConfigEutra.timeToTrigger),
                                         &LteUeRrc::VarMeasReportListAdd, this,
                                         measId, concernedCellsEntry);
          std::map<uint8_t, std::list<PendingTrigger_t> >::iterator
            enteringTriggerIt = m_enteringTriggerQueue.find (measId);
          NS_ASSERT (enteringTriggerIt != m_enteringTriggerQueue.end ());
          enteringTriggerIt->second.push_back (t);
        }
    }

  if (eventLeavingCondApplicable)
    {
      // reportOnLeave will only be set when eventId = eventA3
      bool reportOnLeave = (reportConfigEutra.eventId == LteRrcSap::ReportConfigEutra::EVENT_A3)
        && reportConfigEutra.reportOnLeave;

      if (reportConfigEutra.timeToTrigger == 0)
        {
          VarMeasReportListErase (measId, concernedCellsLeaving, reportOnLeave);
        }
      else
        {
          PendingTrigger_t t;
          t.measId = measId;
          t.concernedCells = concernedCellsLeaving;
          t.timer = Simulator::Schedule (MilliSeconds (reportConfigEutra.timeToTrigger),
                                         &LteUeRrc::VarMeasReportListErase, this,
                                         measId, concernedCellsLeaving, reportOnLeave);
          std::map<uint8_t, std::list<PendingTrigger_t> >::iterator
            leavingTriggerIt = m_leavingTriggerQueue.find (measId);
          NS_ASSERT (leavingTriggerIt != m_leavingTriggerQueue.end ());
          leavingTriggerIt->second.push_back (t);
        }
    }

} // end of void LteUeRrc::MeasurementReportTriggering (uint8_t measId)

void
LteUeRrc::CancelEnteringTrigger (uint8_t measId)
{
  NS_LOG_FUNCTION (this << (uint16_t) measId);

  std::map<uint8_t, std::list<PendingTrigger_t> >::iterator
    it1 = m_enteringTriggerQueue.find (measId);
  NS_ASSERT (it1 != m_enteringTriggerQueue.end ());

  if (!it1->second.empty ())
    {
      std::list<PendingTrigger_t>::iterator it2;
      for (it2 = it1->second.begin (); it2 != it1->second.end (); ++it2)
        {
          NS_ASSERT (it2->measId == measId);
          NS_LOG_LOGIC (this << " canceling entering time-to-trigger event at "
                             << Simulator::GetDelayLeft (it2->timer).GetSeconds ());
          Simulator::Cancel (it2->timer);
        }

      it1->second.clear ();
    }
}

void
LteUeRrc::CancelEnteringTrigger (uint8_t measId, uint16_t cellId)
{
  NS_LOG_FUNCTION (this << (uint16_t) measId << cellId);

  std::map<uint8_t, std::list<PendingTrigger_t> >::iterator
    it1 = m_enteringTriggerQueue.find (measId);
  NS_ASSERT (it1 != m_enteringTriggerQueue.end ());

  std::list<PendingTrigger_t>::iterator it2 = it1->second.begin ();
  while (it2 != it1->second.end ())
    {
      NS_ASSERT (it2->measId == measId);

      ConcernedCells_t::iterator it3;
      for (it3 = it2->concernedCells.begin ();
           it3 != it2->concernedCells.end (); ++it3)
        {
          if (*it3 == cellId)
            {
              it3 = it2->concernedCells.erase (it3);
            }
        }

      if (it2->concernedCells.empty ())
        {
          NS_LOG_LOGIC (this << " canceling entering time-to-trigger event at "
                             << Simulator::GetDelayLeft (it2->timer).GetSeconds ());
          Simulator::Cancel (it2->timer);
          it2 = it1->second.erase (it2);
        }
      else
        {
          it2++;
        }
    }
}

void
LteUeRrc::CancelLeavingTrigger (uint8_t measId)
{
  NS_LOG_FUNCTION (this << (uint16_t) measId);

  std::map<uint8_t, std::list<PendingTrigger_t> >::iterator
    it1 = m_leavingTriggerQueue.find (measId);
  NS_ASSERT (it1 != m_leavingTriggerQueue.end ());

  if (!it1->second.empty ())
    {
      std::list<PendingTrigger_t>::iterator it2;
      for (it2 = it1->second.begin (); it2 != it1->second.end (); ++it2)
        {
          NS_ASSERT (it2->measId == measId);
          NS_LOG_LOGIC (this << " canceling leaving time-to-trigger event at "
                             << Simulator::GetDelayLeft (it2->timer).GetSeconds ());
          Simulator::Cancel (it2->timer);
        }

      it1->second.clear ();
    }
}

void
LteUeRrc::CancelLeavingTrigger (uint8_t measId, uint16_t cellId)
{
  NS_LOG_FUNCTION (this << (uint16_t) measId << cellId);

  std::map<uint8_t, std::list<PendingTrigger_t> >::iterator
    it1 = m_leavingTriggerQueue.find (measId);
  NS_ASSERT (it1 != m_leavingTriggerQueue.end ());

  std::list<PendingTrigger_t>::iterator it2 = it1->second.begin ();
  while (it2 != it1->second.end ())
    {
      NS_ASSERT (it2->measId == measId);

      ConcernedCells_t::iterator it3;
      for (it3 = it2->concernedCells.begin ();
           it3 != it2->concernedCells.end (); ++it3)
        {
          if (*it3 == cellId)
            {
              it3 = it2->concernedCells.erase (it3);
            }
        }

      if (it2->concernedCells.empty ())
        {
          NS_LOG_LOGIC (this << " canceling leaving time-to-trigger event at "
                             << Simulator::GetDelayLeft (it2->timer).GetSeconds ());
          Simulator::Cancel (it2->timer);
          it2 = it1->second.erase (it2);
        }
      else
        {
          it2++;
        }
    }
}

void
LteUeRrc::VarMeasReportListAdd (uint8_t measId, ConcernedCells_t enteringCells)
{
  NS_LOG_FUNCTION (this << (uint16_t) measId);
  NS_ASSERT (!enteringCells.empty ());

  std::map<uint8_t, VarMeasReport>::iterator
    measReportIt = m_varMeasReportList.find (measId);

  if (measReportIt == m_varMeasReportList.end ())
    {
      VarMeasReport r;
      r.measId = measId;
      std::pair<uint8_t, VarMeasReport> val (measId, r);
      std::pair<std::map<uint8_t, VarMeasReport>::iterator, bool>
      ret = m_varMeasReportList.insert (val);
      NS_ASSERT_MSG (ret.second == true, "element already existed");
      measReportIt = ret.first;
    }

  NS_ASSERT (measReportIt != m_varMeasReportList.end ());

  for (ConcernedCells_t::const_iterator it = enteringCells.begin ();
       it != enteringCells.end ();
       ++it)
    {
      measReportIt->second.cellsTriggeredList.insert (*it);
    }

  NS_ASSERT (!measReportIt->second.cellsTriggeredList.empty ());

  // #issue 224, schedule only when there is no periodic event scheduled already
  if (!measReportIt->second.periodicReportTimer.IsRunning ())
    {
      measReportIt->second.numberOfReportsSent = 0;
      measReportIt->second.periodicReportTimer
        = Simulator::Schedule (UE_MEASUREMENT_REPORT_DELAY,
                               &LteUeRrc::SendMeasurementReport,
                               this, measId);
    }

  std::map<uint8_t, std::list<PendingTrigger_t> >::iterator
    enteringTriggerIt = m_enteringTriggerQueue.find (measId);
  NS_ASSERT (enteringTriggerIt != m_enteringTriggerQueue.end ());
  if (!enteringTriggerIt->second.empty ())
    {
      /*
       * Assumptions at this point:
       *  - the call to this function was delayed by time-to-trigger;
       *  - the time-to-trigger delay is fixed (not adaptive/dynamic); and
       *  - the first element in the list is associated with this function call.
       */
      enteringTriggerIt->second.pop_front ();

      if (!enteringTriggerIt->second.empty ())
        {
          /*
           * To prevent the same set of cells triggering again in the future,
           * we clean up the time-to-trigger queue. This case might occur when
           * time-to-trigger > 200 ms.
           */
          for (ConcernedCells_t::const_iterator it = enteringCells.begin ();
               it != enteringCells.end (); ++it)
            {
              CancelEnteringTrigger (measId, *it);
            }
        }

    } // end of if (!enteringTriggerIt->second.empty ())

} // end of LteUeRrc::VarMeasReportListAdd

void
LteUeRrc::VarMeasReportListErase (uint8_t measId, ConcernedCells_t leavingCells,
                                  bool reportOnLeave)
{
  NS_LOG_FUNCTION (this << (uint16_t) measId);
  NS_ASSERT (!leavingCells.empty ());

  std::map<uint8_t, VarMeasReport>::iterator
    measReportIt = m_varMeasReportList.find (measId);
  NS_ASSERT (measReportIt != m_varMeasReportList.end ());

  for (ConcernedCells_t::const_iterator it = leavingCells.begin ();
       it != leavingCells.end ();
       ++it)
    {
      measReportIt->second.cellsTriggeredList.erase (*it);
    }

  if (reportOnLeave)
    {
      // runs immediately without UE_MEASUREMENT_REPORT_DELAY
      SendMeasurementReport (measId);
    }

  if (measReportIt->second.cellsTriggeredList.empty ())
    {
      measReportIt->second.periodicReportTimer.Cancel ();
      m_varMeasReportList.erase (measReportIt);
    }

  std::map<uint8_t, std::list<PendingTrigger_t> >::iterator
    leavingTriggerIt = m_leavingTriggerQueue.find (measId);
  NS_ASSERT (leavingTriggerIt != m_leavingTriggerQueue.end ());
  if (!leavingTriggerIt->second.empty ())
    {
      /*
       * Assumptions at this point:
       *  - the call to this function was delayed by time-to-trigger; and
       *  - the time-to-trigger delay is fixed (not adaptive/dynamic); and
       *  - the first element in the list is associated with this function call.
       */
      leavingTriggerIt->second.pop_front ();

      if (!leavingTriggerIt->second.empty ())
        {
          /*
           * To prevent the same set of cells triggering again in the future,
           * we clean up the time-to-trigger queue. This case might occur when
           * time-to-trigger > 200 ms.
           */
          for (ConcernedCells_t::const_iterator it = leavingCells.begin ();
               it != leavingCells.end (); ++it)
            {
              CancelLeavingTrigger (measId, *it);
            }
        }

    } // end of if (!leavingTriggerIt->second.empty ())

} // end of LteUeRrc::VarMeasReportListErase

void
LteUeRrc::VarMeasReportListClear (uint8_t measId)
{
  NS_LOG_FUNCTION (this << (uint16_t) measId);

  // remove the measurement reporting entry for this measId from the VarMeasReportList
  std::map<uint8_t, VarMeasReport>::iterator
    measReportIt = m_varMeasReportList.find (measId);
  if (measReportIt != m_varMeasReportList.end ())
    {
      NS_LOG_LOGIC (this << " deleting existing report for measId " << (uint16_t) measId);
      measReportIt->second.periodicReportTimer.Cancel ();
      m_varMeasReportList.erase (measReportIt);
    }

  CancelEnteringTrigger (measId);
  CancelLeavingTrigger (measId);
}

void
LteUeRrc::SendMeasurementReport (uint8_t measId)
{
  NS_LOG_FUNCTION (this << (uint16_t) measId);
  //  3GPP TS 36.331 section 5.5.5 Measurement reporting

  std::map<uint8_t, LteRrcSap::MeasIdToAddMod>::iterator
    measIdIt = m_varMeasConfig.measIdList.find (measId);
  NS_ASSERT (measIdIt != m_varMeasConfig.measIdList.end ());

  std::map<uint8_t, LteRrcSap::ReportConfigToAddMod>::iterator
    reportConfigIt = m_varMeasConfig.reportConfigList.find (measIdIt->second.reportConfigId);
  NS_ASSERT (reportConfigIt != m_varMeasConfig.reportConfigList.end ());
  LteRrcSap::ReportConfigEutra& reportConfigEutra = reportConfigIt->second.reportConfigEutra;

  LteRrcSap::MeasurementReport measurementReport;
  LteRrcSap::MeasResults& measResults = measurementReport.measResults;
  measResults.measId = measId;

  std::map<uint8_t, VarMeasReport>::iterator measReportIt = m_varMeasReportList.find (measId);
  if (measReportIt == m_varMeasReportList.end ())
    {
      NS_LOG_ERROR ("no entry found in m_varMeasReportList for measId " << (uint32_t) measId);
    }
  else
    {
      std::map<uint16_t, MeasValues>::iterator servingMeasIt = m_storedMeasValues.find (m_cellId);
      NS_ASSERT (servingMeasIt != m_storedMeasValues.end ());
      measResults.measResultPCell.rsrpResult = EutranMeasurementMapping::Dbm2RsrpRange (servingMeasIt->second.rsrp);
      measResults.measResultPCell.rsrqResult = EutranMeasurementMapping::Db2RsrqRange (servingMeasIt->second.rsrq);
      NS_LOG_INFO (this << " reporting serving cell "
                   "RSRP " << +measResults.measResultPCell.rsrpResult << " (" << servingMeasIt->second.rsrp << " dBm) "
                   "RSRQ " << +measResults.measResultPCell.rsrqResult << " (" << servingMeasIt->second.rsrq << " dB)");

      measResults.haveMeasResultServFreqList = false;
      for (uint8_t componentCarrierId = 1; componentCarrierId < m_numberOfComponentCarriers; componentCarrierId++)
        {
          const uint16_t cellId = m_cphySapProvider.at (componentCarrierId)->GetCellId ();
          auto measValuesIt = m_storedMeasValues.find (cellId);
          if (measValuesIt != m_storedMeasValues.end ())
            {
              measResults.haveMeasResultServFreqList = true;
              LteRrcSap::MeasResultServFreq measResultServFreq;
              measResultServFreq.servFreqId = componentCarrierId;
              measResultServFreq.haveMeasResultSCell = true;
              measResultServFreq.measResultSCell.rsrpResult = EutranMeasurementMapping::Dbm2RsrpRange (measValuesIt->second.rsrp);
              measResultServFreq.measResultSCell.rsrqResult = EutranMeasurementMapping::Db2RsrqRange (measValuesIt->second.rsrq);
              measResultServFreq.haveMeasResultBestNeighCell = false;
              measResults.measResultServFreqList.push_back (measResultServFreq);
            }
        }

      measResults.haveMeasResultNeighCells = false;

      if (!(measReportIt->second.cellsTriggeredList.empty ()))
        {
          std::multimap<double, uint16_t> sortedNeighCells;
          for (std::set<uint16_t>::iterator cellsTriggeredIt = measReportIt->second.cellsTriggeredList.begin ();
               cellsTriggeredIt != measReportIt->second.cellsTriggeredList.end ();
               ++cellsTriggeredIt)
            {
              uint16_t cellId = *cellsTriggeredIt;
              if (cellId != m_cellId)
                {
                  std::map<uint16_t, MeasValues>::iterator neighborMeasIt = m_storedMeasValues.find (cellId);
                  double triggerValue;
                  switch (reportConfigEutra.triggerQuantity)
                    {
                      case LteRrcSap::ReportConfigEutra::RSRP:
                        triggerValue = neighborMeasIt->second.rsrp;
                        break;
                      case LteRrcSap::ReportConfigEutra::RSRQ:
                        triggerValue = neighborMeasIt->second.rsrq;
                        break;
                      default:
                        NS_FATAL_ERROR ("unsupported triggerQuantity");
                        break;
                    }
                  sortedNeighCells.insert (std::pair<double, uint16_t> (triggerValue, cellId));
                }
            }

          std::multimap<double, uint16_t>::reverse_iterator sortedNeighCellsIt;
          uint32_t count;
          for (sortedNeighCellsIt = sortedNeighCells.rbegin (), count = 0;
               sortedNeighCellsIt != sortedNeighCells.rend () && count < reportConfigEutra.maxReportCells;
               ++sortedNeighCellsIt, ++count)
            {
              uint16_t cellId = sortedNeighCellsIt->second;
              std::map<uint16_t, MeasValues>::iterator neighborMeasIt = m_storedMeasValues.find (cellId);
              NS_ASSERT (neighborMeasIt != m_storedMeasValues.end ());
              LteRrcSap::MeasResultEutra measResultEutra;
              measResultEutra.physCellId = cellId;
              measResultEutra.haveCgiInfo = false;
              measResultEutra.haveRsrpResult = true;
              measResultEutra.rsrpResult = EutranMeasurementMapping::Dbm2RsrpRange (neighborMeasIt->second.rsrp);
              measResultEutra.haveRsrqResult = true;
              measResultEutra.rsrqResult = EutranMeasurementMapping::Db2RsrqRange (neighborMeasIt->second.rsrq);
              NS_LOG_INFO (this << " reporting neighbor cell " << (uint32_t) measResultEutra.physCellId
                                << " RSRP " << (uint32_t) measResultEutra.rsrpResult
                                << " (" << neighborMeasIt->second.rsrp << " dBm)"
                                << " RSRQ " << (uint32_t) measResultEutra.rsrqResult
                                << " (" << neighborMeasIt->second.rsrq << " dB)");
              measResults.measResultListEutra.push_back (measResultEutra);
              measResults.haveMeasResultNeighCells = true;
            }
        }
      else
        {
          NS_LOG_WARN (this << " cellsTriggeredList is empty");
        }

      /*
       * The current LteRrcSap implementation is broken in that it does not
       * allow for infinite values of reportAmount, which is probably the most
       * reasonable setting. So we just always assume infinite reportAmount.
       */
      measReportIt->second.numberOfReportsSent++;
      measReportIt->second.periodicReportTimer.Cancel ();

      Time reportInterval;
      switch (reportConfigEutra.reportInterval)
        {
          case LteRrcSap::ReportConfigEutra::MS120:
            reportInterval = MilliSeconds (120);
            break;
          case LteRrcSap::ReportConfigEutra::MS240:
            reportInterval = MilliSeconds (240);
            break;
          case LteRrcSap::ReportConfigEutra::MS480:
            reportInterval = MilliSeconds (480);
            break;
          case LteRrcSap::ReportConfigEutra::MS640:
            reportInterval = MilliSeconds (640);
            break;
          case LteRrcSap::ReportConfigEutra::MS1024:
            reportInterval = MilliSeconds (1024);
            break;
          case LteRrcSap::ReportConfigEutra::MS2048:
            reportInterval = MilliSeconds (2048);
            break;
          case LteRrcSap::ReportConfigEutra::MS5120:
            reportInterval = MilliSeconds (5120);
            break;
          case LteRrcSap::ReportConfigEutra::MS10240:
            reportInterval = MilliSeconds (10240);
            break;
          case LteRrcSap::ReportConfigEutra::MIN1:
            reportInterval = Seconds (60);
            break;
          case LteRrcSap::ReportConfigEutra::MIN6:
            reportInterval = Seconds (360);
            break;
          case LteRrcSap::ReportConfigEutra::MIN12:
            reportInterval = Seconds (720);
            break;
          case LteRrcSap::ReportConfigEutra::MIN30:
            reportInterval = Seconds (1800);
            break;
          case LteRrcSap::ReportConfigEutra::MIN60:
            reportInterval = Seconds (3600);
            break;
          default:
            NS_FATAL_ERROR ("Unsupported reportInterval " << (uint16_t) reportConfigEutra.reportInterval);
            break;
        }

      // schedule the next measurement reporting
      measReportIt->second.periodicReportTimer
        = Simulator::Schedule (reportInterval,
                               &LteUeRrc::SendMeasurementReport,
                               this, measId);

      // send the measurement report to eNodeB
      m_rrcSapUser->SendMeasurementReport (measurementReport);
    }
}

void
LteUeRrc::StartConnection ()
{
  NS_LOG_FUNCTION (this << m_imsi);
  NS_ASSERT (m_hasReceivedMib);
  NS_ASSERT (m_hasReceivedSib2);
  m_connectionPending = false; // reset the flag
  SwitchToState (IDLE_RANDOM_ACCESS);
  m_cmacSapProvider.at (0)->StartContentionBasedRandomAccessProcedure ();
}

void
LteUeRrc::LeaveConnectedMode ()
{
  NS_LOG_FUNCTION (this << m_imsi);
  m_leaveConnectedMode = true;
  m_storedMeasValues.clear ();
  ResetRlfParams ();

  std::map<uint8_t, LteRrcSap::MeasIdToAddMod>::iterator measIdIt;
  for (measIdIt = m_varMeasConfig.measIdList.begin ();
       measIdIt != m_varMeasConfig.measIdList.end (); ++measIdIt)
    {
      VarMeasReportListClear (measIdIt->second.measId);
    }
  m_varMeasConfig.measIdList.clear ();

  m_ccmRrcSapProvider->Reset ();

  for (uint32_t i = 0; i < m_numberOfComponentCarriers; i++)
    {
      m_cmacSapProvider.at (i)->Reset ();  // reset the MAC
    }

  m_drbMap.clear ();
  m_bid2DrbidMap.clear ();
  m_srb1 = nullptr;
  m_hasReceivedMib = false;
  m_hasReceivedSib1 = false;
  m_hasReceivedSib2 = false;

  for (uint32_t i = 0; i < m_numberOfComponentCarriers; i++)
    {
      m_cphySapProvider.at (i)->ResetPhyAfterRlf ();  //reset the PHY
    }
  SwitchToState (IDLE_START);
  DoStartCellSelection (m_dlEarfcn);
  //Save the cell id UE was attached to
  StorePreviousCellId (m_cellId);
  m_cellId = 0;
  m_rnti = 0;
  m_srb0->m_rlc->SetRnti (m_rnti);
}

void
LteUeRrc::ConnectionTimeout ()
{
  NS_LOG_FUNCTION (this << m_imsi);
  ++m_connEstFailCount;
  if (m_connEstFailCount >= m_connEstFailCountLimit)
    {
      m_connectionTimeoutTrace (m_imsi, m_cellId, m_rnti, m_connEstFailCount);
      SwitchToState (CONNECTED_PHY_PROBLEM);
      //Assumption: The eNB connection request timer would expire
      //before the expiration of T300 at UE. Upon which, the eNB deletes
      //the UE context. Therefore, here we don't need to send the UE context
      //deletion request to the eNB.
      m_asSapUser->NotifyConnectionReleased ();
      m_connEstFailCount = 0;
    }
  else
    {
      for (uint16_t i = 0; i < m_numberOfComponentCarriers; i++)
        {
          m_cmacSapProvider.at (i)->Reset ();  // reset the MAC
        }
      m_hasReceivedSib2 = false;           // invalidate the previously received SIB2
      SwitchToState (IDLE_CAMPED_NORMALLY);
      m_connectionTimeoutTrace (m_imsi, m_cellId, m_rnti, m_connEstFailCount);
      //Following call to UE NAS will force the UE to immediately
      //perform the random access to the same cell again.
      m_asSapUser->NotifyConnectionFailed ();    // inform upper layer
    }
}

void
LteUeRrc::DisposeOldSrb1 ()
{
  NS_LOG_FUNCTION (this);
  m_srb1Old = 0;
}

uint8_t
LteUeRrc::Bid2Drbid (uint8_t bid)
{
  std::map<uint8_t, uint8_t>::iterator it = m_bid2DrbidMap.find (bid);
  if (it == m_bid2DrbidMap.end ())
    {
      return 0;
    }
  else
    {
      return it->second;
    }
}

void
LteUeRrc::SwitchToState (State newState)
{
  NS_LOG_FUNCTION (this << ToString (newState));
  State oldState = m_state;
  m_state = newState;
  NS_LOG_INFO (this << " IMSI " << m_imsi << " RNTI " << m_rnti << " UeRrc "
                    << ToString (oldState) << " --> " << ToString (newState));
  m_stateTransitionTrace (m_imsi, m_cellId, m_rnti, oldState, newState);

  switch (newState)
    {
      case IDLE_START:
        if (m_leaveConnectedMode)
          {
            NS_LOG_INFO ("Starting initial cell selection after RLF");
          }
        else
          {
            NS_FATAL_ERROR ("cannot switch to an initial state");
          }
        break;

      case IDLE_CELL_SEARCH:
      case IDLE_WAIT_MIB_SIB1:
      case IDLE_WAIT_MIB:
      case IDLE_WAIT_SIB1:
        break;

      case IDLE_CAMPED_NORMALLY:
        if (m_connectionPending)
          {
            SwitchToState (IDLE_WAIT_SIB2);
          }
        break;

      case IDLE_WAIT_SIB2:
        if (m_hasReceivedSib2)
          {
            NS_ASSERT (m_connectionPending);
            StartConnection ();
          }
        break;

      case IDLE_RANDOM_ACCESS:
      case IDLE_CONNECTING:
      case CONNECTED_NORMALLY:
      case CONNECTED_HANDOVER:
      case CONNECTED_PHY_PROBLEM:
      case CONNECTED_REESTABLISHING:
        break;

      default:
        break;
    }
}



void
LteUeRrc::RadioLinkFailureDetected ()
{
  NS_LOG_FUNCTION (this << m_imsi << m_rnti);
  m_radioLinkFailureTrace (m_imsi, m_cellId, m_rnti);
  SwitchToState (CONNECTED_PHY_PROBLEM);
  m_rrcSapUser->SendIdealUeContextRemoveRequest (m_rnti);
  m_asSapUser->NotifyConnectionReleased ();
}

void
LteUeRrc::DoNotifyInSync ()
{
  NS_LOG_FUNCTION (this << m_imsi);
  m_noOfSyncIndications++;
  NS_LOG_INFO ("noOfSyncIndications " << (uint16_t) m_noOfSyncIndications);
  m_phySyncDetectionTrace (m_imsi, m_rnti, m_cellId, "Notify in sync", m_noOfSyncIndications);
  if (m_noOfSyncIndications == m_n311)
    {
      ResetRlfParams ();
    }
}

void
LteUeRrc::DoNotifyOutOfSync ()
{
  NS_LOG_FUNCTION (this << m_imsi);
  m_noOfSyncIndications++;
  NS_LOG_INFO (this << " Total Number of Sync indications from PHY "
                    << (uint16_t) m_noOfSyncIndications << "N310 value : " << (uint16_t) m_n310);
  m_phySyncDetectionTrace (m_imsi, m_rnti, m_cellId, "Notify out of sync", m_noOfSyncIndications);
  if (m_noOfSyncIndications == m_n310)
    {
      m_radioLinkFailureDetected = Simulator::Schedule (m_t310, &LteUeRrc::RadioLinkFailureDetected, this);
      if (m_radioLinkFailureDetected.IsRunning ())
        {
          NS_LOG_INFO ("t310 started");
        }
      m_cphySapProvider.at (0)->StartInSnycDetection ();
      m_noOfSyncIndications = 0;
    }
}

void
LteUeRrc::DoResetSyncIndicationCounter ()
{
  NS_LOG_FUNCTION (this << m_imsi);

  NS_LOG_DEBUG ("The number of sync indication received by RRC from PHY: " << (uint16_t) m_noOfSyncIndications);
  m_noOfSyncIndications = 0;
}

void
LteUeRrc::ResetRlfParams ()
{
  NS_LOG_FUNCTION (this << m_imsi);
  m_radioLinkFailureDetected.Cancel ();
  m_noOfSyncIndications = 0;
  m_cphySapProvider.at (0)->ResetRlfParams ();
}

const std::string
LteUeRrc::ToString (LteUeRrc::State s)
{
  return g_ueRrcStateName[s];
}

void
LteUeRrc::ActivateSidelinkRadioBearer (uint32_t destination, bool tx, bool rx)
{
  NS_LOG_FUNCTION (this << destination << tx << rx);
  //setup bearer to be able to transmit to the given destination (peer UE)
  DoActivateSidelinkRadioBearer (destination, tx, rx);
}

void
LteUeRrc::DoActivateSidelinkRadioBearer (uint32_t group, bool tx, bool rx)
{
  NS_LOG_FUNCTION (this << group << tx << rx);

  //NS_ASSERT_MSG (m_sidelinkConfiguration->GetSidelinkRadioBearer (m_sidelinkConfiguration->m_sourceL2Id, group) == NULL,
  //                 "Sidelink bearer with source L2 id = "<< m_sidelinkConfiguration->m_sourceL2Id << " and group id = "
  //                 << group <<" is already established.");
  if (m_sidelinkConfiguration->GetSidelinkRadioBearer (m_sidelinkConfiguration->m_sourceL2Id, group) != NULL)
    {
      NS_LOG_INFO ("Sidelink bearer with source L2 id = " << m_sidelinkConfiguration->m_sourceL2Id << " and group id = "
                                                          << group << " is already established.");
      m_asSapUser->NotifySidelinkRadioBearerActivated (group);
      return;
    }

  switch (m_state)
    {
    case IDLE_START:
    case IDLE_CELL_SEARCH:
    case IDLE_WAIT_MIB_SIB1:
    case IDLE_WAIT_MIB:
    case IDLE_WAIT_SIB1:
    case IDLE_CAMPED_NORMALLY:

      NS_LOG_INFO ("Considering IMSI " << m_imsi << " out of network");

      if (m_rnti == 0)
        {
          //the RNTI was not configured, this is the first call to Sidelink configuration
          NS_LOG_INFO (this << " Setting RNTI to " <<  (uint16_t) (m_imsi & 0xFFFF));
          //preconfigure the RNTI to the IMSI's 16 LSB for uniqueness
          NS_LOG_DEBUG ("Sidelink assigning RNTI " << (uint16_t) (m_imsi & 0xFFFF));
          DoSetTemporaryCellRnti ( (uint16_t) (m_imsi & 0xFFFF));
          //since it is first time, configure the physical layer of the primary carrier as well
          m_cphySapProvider.at (0)->ConfigureUplink (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigGeneral.carrierFreq,
                                                     m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigGeneral.slBandwidth);
        }

      if (tx)
        {
          Ptr<LteSidelinkRadioBearerInfo> slbInfo = AddSlrb (m_sidelinkConfiguration->m_sourceL2Id, group, m_sidelinkConfiguration->GetNextLcid (group));
          NS_LOG_INFO ("Created new TX SLRB for group " << group << " LCID=" << (slbInfo->m_logicalChannelIdentity & 0xF));
        }
      if (rx)
        {
          //Add to the list of group to monitor for Sidelink
          m_sidelinkConfiguration->m_rxGroup.push_back (group);
          //tell the PHY and MAC of primary carrier to listen for the group
          m_cphySapProvider.at (0)->AddSlDestination (group);
          m_cmacSapProvider.at (0)->AddSlDestination (group);
        }

      NS_ASSERT (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigComm.nbPools > 0);
      //Activate bearer using preconfiguration if available
      if (tx)
        {
          NS_LOG_INFO ("Configuring Tx pool");
          Ptr<SidelinkTxCommResourcePool> txPool = CreateObject<SidelinkTxCommResourcePool>();
          txPool->SetPool (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigComm.pools[0]);
          txPool->SetUeSelectedTxParameters (0);
          std::list <uint32_t>::iterator it;
          std::list <uint32_t> destinations = m_sidelinkConfiguration->GetTxDestinations ();

          m_cmacSapProvider.at (0)->AddSlCommTxPool (group, txPool);
          //if this is the first group setup, register pool with physical layer, otherwise it has already been done
          if (destinations.size () == 1)
            {
              //inform PHY of primary carrier about the tx pool
              m_cphySapProvider.at (0)->SetSlCommTxPool (txPool);
            }
        }
      if ((tx && rx) || rx) // only populate Rx pool info if rx == true
        {
          NS_LOG_INFO ("Configuring Rx pool");
          //Configure receiving pool
          std::list< Ptr<SidelinkRxCommResourcePool> > pools;
          Ptr<SidelinkRxCommResourcePool> pool = CreateObject <SidelinkRxCommResourcePool> ();
          pool->SetPool (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigComm.pools[0]);
          //must find ways to store Rx pool though it is in different format
          //m_sidelinkConfiguration->rxPools.push_back (std::make_pair(msg.sib18.commConfig.commRxPool.pools[i], pool));
          pools.push_back (pool);
          m_cmacSapProvider.at (0)->SetSlCommRxPools (pools);
          m_cphySapProvider.at (0)->SetSlCommRxPools (pools);
        }
      //for testing, just indicate it is ok
      m_asSapUser->NotifySidelinkRadioBearerActivated (group);
      m_sidelinkConfiguration->NotifySidelinkRadioBearerActivated (group);
      break;

    case IDLE_WAIT_SIB2:
    case IDLE_CONNECTING:
      NS_LOG_INFO ("Connecting, must wait to send message");
      break;

    case CONNECTED_NORMALLY:
    case CONNECTED_HANDOVER:
    case CONNECTED_PHY_PROBLEM:
    case CONNECTED_REESTABLISHING:
      NS_LOG_INFO ("Considering in coverage");
      if (tx)
        {
          Ptr<LteSidelinkRadioBearerInfo> slbInfo = AddSlrb (m_sidelinkConfiguration->m_sourceL2Id, group, m_sidelinkConfiguration->GetNextLcid (group));
          NS_LOG_INFO ("Created new TX SLRB for group " << group << " LCID=" << (slbInfo->m_logicalChannelIdentity & 0xF));
        }
      if (rx)
        {
          //Add to the list of group to monitor for Sidelink
          m_sidelinkConfiguration->m_rxGroup.push_back (group);
          //tell the phy to listen for the group
          m_cphySapProvider.at (0)->AddSlDestination (group);
          m_cmacSapProvider.at (0)->AddSlDestination (group);
        }
      //Try to send to eNodeB
      SendSidelinkUeInformation (tx, rx, false, false);
      break;

    default: // i.e. IDLE_RANDOM_ACCESS
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }
}

void
LteUeRrc::DoDeactivateSidelinkRadioBearer (uint32_t group)
{
  NS_LOG_FUNCTION (this);

  //TODO: need handling of SLRBs for the group coming from other UEs as well.

  if (m_sidelinkConfiguration->DeleteSidelinkRadioBearer (m_sidelinkConfiguration->m_sourceL2Id, group))
    {
      switch (m_state)
        {
        case IDLE_START:
        case IDLE_CELL_SEARCH:
        case IDLE_WAIT_MIB_SIB1:
        case IDLE_WAIT_MIB:
        case IDLE_WAIT_SIB1:
        case IDLE_CAMPED_NORMALLY:
          NS_LOG_INFO ("Considering out of network");
          //Activate bearer using preconfiguration if available
          //TBD
          break;
        case CONNECTED_NORMALLY:
        case CONNECTED_HANDOVER:
        case CONNECTED_PHY_PROBLEM:
        case CONNECTED_REESTABLISHING:
          NS_LOG_INFO ("Considering in coverage");
          //Try to send to eNodeB
          //TODO: check if the communication for for tx only, rx only, or both
          SendSidelinkUeInformation (true, false, false, false);
          break;

        default: // i.e. IDLE_RANDOM_ACCESS
          NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
          break;
        }
    }
}

Ptr<LteSidelinkRadioBearerInfo>
LteUeRrc::AddSlrb (uint32_t source, uint32_t destination, uint8_t lcid)
{
  NS_LOG_FUNCTION (this);

  NS_ABORT_MSG_IF ((source == 0 || destination == 0), "ProSe L2 source or destination Id shouldn't be 0");
  Ptr<LteSidelinkRadioBearerInfo> slbInfo = CreateObject <LteSidelinkRadioBearerInfo> ();
  slbInfo->m_sourceL2Id = source;
  slbInfo->m_destinationL2Id = destination;
  slbInfo->m_logicalChannelIdentity = lcid;
  m_sidelinkConfiguration->AddSidelinkRadioBearer (slbInfo);

  //create PDCP/RLC stack
  ObjectFactory rlcObjectFactory;
  rlcObjectFactory.SetTypeId (LteRlcUm::GetTypeId ());
  Ptr<LteRlc> rlc = rlcObjectFactory.Create ()->GetObject<LteRlc> ();
  rlc->SetLteMacSapProvider (m_macSapProvider);
  rlc->SetRnti (m_rnti);
  rlc->SetLcId (slbInfo->m_logicalChannelIdentity);
  rlc->SetSourceL2Id (slbInfo->m_sourceL2Id);
  rlc->SetDestinationL2Id (slbInfo->m_destinationL2Id);
  rlc->SetRlcChannelType (LteRlc::STCH);

  slbInfo->m_rlc = rlc;

  Ptr<LtePdcp> pdcp = CreateObject<LtePdcp> ();
  pdcp->SetRnti (m_rnti);
  pdcp->SetLcId (slbInfo->m_logicalChannelIdentity);
  pdcp->SetSourceL2Id (slbInfo->m_sourceL2Id);
  pdcp->SetDestinationL2Id (slbInfo->m_destinationL2Id);
  pdcp->SetLtePdcpSapUser (m_drbPdcpSapUser);
  pdcp->SetLteRlcSapProvider (rlc->GetLteRlcSapProvider ());
  rlc->SetLteRlcSapUser (pdcp->GetLteRlcSapUser ());
  slbInfo->m_pdcp = pdcp;


  struct LteUeCmacSapProvider::LogicalChannelConfig lcConfig;
  lcConfig.priority = 9;
  lcConfig.prioritizedBitRateKbps = 65535;
  lcConfig.bucketSizeDurationMs = 65535;
  lcConfig.logicalChannelGroup = 3; // as per 36.331 9.1.1.6

  NS_LOG_DEBUG (this << " UE RRC RNTI " << m_rnti << " Number Of Component Carriers "
                     << m_numberOfComponentCarriers << " lcID " << (uint16_t) slbInfo->m_logicalChannelIdentity);

  //Call AddSlLc of UE component carrier manager
  std::vector <LteUeCcmRrcSapProvider::LcsConfig> slLcOnCcMapping = m_ccmRrcSapProvider->AddSlLc (slbInfo->m_logicalChannelIdentity,
                                                                                                  slbInfo->m_sourceL2Id,
                                                                                                  slbInfo->m_destinationL2Id,
                                                                                                  lcConfig, rlc->GetLteMacSapUser ());

  NS_LOG_DEBUG ("Size of lcOnCcMapping vector " << slLcOnCcMapping.size ());
  std::vector<LteUeCcmRrcSapProvider::LcsConfig>::iterator itLcOnCcMapping = slLcOnCcMapping.begin ();
  NS_ASSERT_MSG (itLcOnCcMapping != slLcOnCcMapping.end (), "Component carrier manager failed to add Sl LC for SL radio bearer");

  NS_LOG_DEBUG ("RNTI " << m_rnti << " LCG id " << (uint16_t) itLcOnCcMapping->lcConfig.logicalChannelGroup
                        << " ComponentCarrierId " << (uint16_t) itLcOnCcMapping->componentCarrierId);
  uint8_t componentCarrierId = itLcOnCcMapping->componentCarrierId;

  NS_ABORT_MSG_IF (componentCarrierId != 0, "CA is not supported for Sidelink. Component carrier id should be 0");

  LteUeCmacSapProvider::LogicalChannelConfig lcConfigFromCcm = itLcOnCcMapping->lcConfig;
  LteMacSapUser *msu = itLcOnCcMapping->msu;
  m_cmacSapProvider.at (componentCarrierId)->AddSlLc (slbInfo->m_logicalChannelIdentity,
                                                      slbInfo->m_sourceL2Id,
                                                      slbInfo->m_destinationL2Id,
                                                      lcConfigFromCcm,
                                                      msu);
  return slbInfo;
}

void
LteUeRrc::StartDiscoveryApps (std::list<uint32_t> appCodes, LteSlUeRrc::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);

  bool wasAnnouncingInterested = m_sidelinkConfiguration->IsAnnouncingInterested ();
  bool wasMonitoringInterested = m_sidelinkConfiguration->IsMonitoringInterested ();

  switch (m_state)
    {
    case IDLE_START:
    case IDLE_CELL_SEARCH:
    case IDLE_WAIT_MIB_SIB1:
    case IDLE_WAIT_MIB:
    case IDLE_WAIT_SIB1:
    case IDLE_CAMPED_NORMALLY:
      NS_LOG_INFO ("Considering out of network");

      if (m_rnti == 0)
        {
          NS_LOG_INFO (this << " Setting RNTI to " <<  (uint16_t) (m_imsi & 0xFFFF));
          //preconfigure the RNTI to the IMSI's 16 LSB for uniqueness
          DoSetTemporaryCellRnti ( (uint16_t) (m_imsi & 0xFFFF));
          //propagate to the MAC (normally the MAC indicates the RNTI when receiving message from the eNodeB)
          m_cmacSapProvider.at (0)->SetRnti (m_rnti);
          //since it is first time, configure the physical layer as well
          m_cphySapProvider.at (0)->ConfigureUplink (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigGeneral.carrierFreq, m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigGeneral.slBandwidth);
        }
      m_sidelinkConfiguration->StartDiscoveryApps (appCodes, role);
      NS_LOG_INFO ("Created new discovery app for UE " << m_rnti );

      //Set pool using preconfigured one if it exits
      if (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigDisc.nbPools > 0)
        {
          //announce
          if (!wasAnnouncingInterested && m_sidelinkConfiguration->IsAnnouncingInterested ())
            {
              NS_LOG_INFO ("Configuring Tx preconfigured pool");
              Ptr<SidelinkTxDiscResourcePool> txPool = CreateObject<SidelinkTxDiscResourcePool>();
              txPool->SetPool (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigDisc.pools[0]);
              txPool->SetUeSelectedTxParameters (0);
              m_sidelinkConfiguration->SetActiveTxDiscoveryPool (txPool);
              //inform MAC and PHY about the pool
              m_cmacSapProvider.at (0)->SetSlDiscTxPool (txPool);
              m_sidelinkConfiguration->StartAnnouncing ();
            }
          //monitor
          if (!wasMonitoringInterested && m_sidelinkConfiguration->IsMonitoringInterested ())
            {
              NS_LOG_INFO ("Configuring Rx preconfigured pool");
              std::list< Ptr<SidelinkRxDiscResourcePool> > pools;
              Ptr<SidelinkRxDiscResourcePool> pool = CreateObject <SidelinkRxDiscResourcePool> ();
              pool->SetPool (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigDisc.pools[0]);
              pools.push_back (pool);
              m_cmacSapProvider.at (0)->SetSlDiscRxPools (pools);
              m_cphySapProvider.at (0)->SetSlDiscRxPools (pools);
            }
        }

      break;

    case IDLE_WAIT_SIB2:
    case IDLE_CONNECTING:
      NS_LOG_INFO ("Connecting, must wait to send message");
      break;

    case CONNECTED_NORMALLY:
    case CONNECTED_HANDOVER:
    case CONNECTED_PHY_PROBLEM:
    case CONNECTED_REESTABLISHING:
      NS_LOG_INFO ("Considering in coverage");

      m_sidelinkConfiguration->StartDiscoveryApps (appCodes, role);
      NS_LOG_INFO ("Created new discovery Payloads for UE " << m_rnti );
      //Inform MAC about new disc payloads
      //m_cmacSapProvider.at (0)->ModifyDiscTxPayloads (m_sidelinkConfiguration->m_announcePayloads);
      //m_cmacSapProvider.at (0)->ModifyDiscRxPayloads (m_sidelinkConfiguration->m_monitorPayloads);
      //Try to send to eNodeB
      SendSidelinkUeInformation (false, false, wasAnnouncingInterested != m_sidelinkConfiguration->IsAnnouncingInterested (), wasMonitoringInterested != m_sidelinkConfiguration->IsMonitoringInterested () );
      break;

    default: // i.e. IDLE_RANDOM_ACCESS
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }

}

void
LteUeRrc::StopDiscoveryApps (std::list<uint32_t> appCodes, LteSlUeRrc::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);

  bool wasAnnouncingInterested = m_sidelinkConfiguration->IsAnnouncingInterested ();
  bool wasMonitoringInterested = m_sidelinkConfiguration->IsMonitoringInterested ();

  m_sidelinkConfiguration->StopDiscoveryApps (appCodes, role);
  NS_LOG_INFO ("deleting new discovery Payloads for UE " << m_rnti );
  // Inform MAC: if no longer interested in announcing or monitoring, remove discovery pools
  //m_cmacSapProvider.at (0)->ModifyDiscTxPayloads (m_sidelinkConfiguration->m_announcePayloads);
  //m_cmacSapProvider.at (0)->ModifyDiscRxPayloads (m_sidelinkConfiguration->m_monitorPayloads);

  switch (m_state)
    {
    case IDLE_START:
    case IDLE_CELL_SEARCH:
    case IDLE_WAIT_MIB_SIB1:
    case IDLE_WAIT_MIB:
    case IDLE_WAIT_SIB1:
    case IDLE_CAMPED_NORMALLY:
      NS_LOG_INFO ("Considering out of network");
      //Activate bearer using preconfiguration if available
      //TBD
      break;
    case CONNECTED_NORMALLY:
    case CONNECTED_HANDOVER:
    case CONNECTED_PHY_PROBLEM:
    case CONNECTED_REESTABLISHING:
      NS_LOG_INFO ("Considering in coverage");
      //Try to send to eNodeB
      SendSidelinkUeInformation (false, false, wasAnnouncingInterested != m_sidelinkConfiguration->IsAnnouncingInterested (), wasMonitoringInterested != m_sidelinkConfiguration->IsMonitoringInterested ());
      break;

    default:   // i.e. IDLE_RANDOM_ACCESS
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }
}

void
LteUeRrc::StartRelayService (uint32_t serviceCode, LteSlUeRrc::DiscoveryModel model, LteSlUeRrc::RelayRole role)
{
  NS_LOG_FUNCTION (this);

  bool wasAnnouncingInterested = m_sidelinkConfiguration->IsAnnouncingInterested ();
  bool wasMonitoringInterested = m_sidelinkConfiguration->IsMonitoringInterested ();
  bool rxSelf = (std::find (m_sidelinkConfiguration->m_rxGroup.begin (), m_sidelinkConfiguration->m_rxGroup.end (), m_sidelinkConfiguration->m_sourceL2Id) != m_sidelinkConfiguration->m_rxGroup.end ());

  switch (m_state)
    {
    case IDLE_START:
    case IDLE_CELL_SEARCH:
    case IDLE_WAIT_MIB_SIB1:
    case IDLE_WAIT_MIB:
    case IDLE_WAIT_SIB1:
    case IDLE_CAMPED_NORMALLY:
      NS_LOG_INFO ("Considering out of network");

      if (m_rnti == 0)
        {
          NS_LOG_INFO (this << " Setting RNTI to " <<  (uint16_t) (m_imsi & 0xFFFF));
          //preconfigure the RNTI to the IMSI's 16 LSB for uniqueness
          DoSetTemporaryCellRnti ( (uint16_t) (m_imsi & 0xFFFF));
          //propagate to the MAC (normally the MAC indicates the RNTI when receiving message from the eNodeB)
          m_cmacSapProvider.at (0)->SetRnti (m_rnti);
          //since it is first time, configure the physical layer as well
          m_cphySapProvider.at (0)->ConfigureUplink (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigGeneral.carrierFreq, m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigGeneral.slBandwidth);
        }
      m_sidelinkConfiguration->StartRelayService (serviceCode, model, role);
      NS_LOG_INFO ("Created new discovery app for UE " << m_rnti );

      //Set pool using preconfigured one if it exits
      if (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigDisc.nbPools > 0)
        {
          //announce
          if (!wasAnnouncingInterested && m_sidelinkConfiguration->IsAnnouncingInterested ())
            {
              NS_LOG_INFO ("Configuring Tx preconfigured pool");
              Ptr<SidelinkTxDiscResourcePool> txPool = CreateObject<SidelinkTxDiscResourcePool>();
              txPool->SetPool (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigDisc.pools[0]);
              txPool->SetUeSelectedTxParameters (0);
              m_sidelinkConfiguration->SetActiveTxDiscoveryPool (txPool);
              //inform MAC and PHY about the pool
              m_cmacSapProvider.at (0)->SetSlDiscTxPool (txPool);
            }
          //monitor
          if (!wasMonitoringInterested && m_sidelinkConfiguration->IsMonitoringInterested ())
            {
              std::list< Ptr<SidelinkRxDiscResourcePool> > pools;
              Ptr<SidelinkRxDiscResourcePool> pool = CreateObject <SidelinkRxDiscResourcePool> ();
              pool->SetPool (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigDisc.pools[0]);
              pools.push_back (pool);
              m_cmacSapProvider.at (0)->SetSlDiscRxPools (pools);
              m_cphySapProvider.at (0)->SetSlDiscRxPools (pools);

              if (role == LteSlUeRrc::RemoteUE)
                {
                  // Enable SD-RSRP measurement for Relay UE selection
                  m_cphySapProvider.at (0)->EnableUeSdRsrpMeasurements ();
                }
            }
          //must enable reception of direct communication messages
          if (!rxSelf)
            {
              NS_LOG_INFO ("Notify lower layers to accept incoming message");
              //Add to the list of group to monitor for sidelink
              m_sidelinkConfiguration->m_rxGroup.push_back (m_sidelinkConfiguration->m_sourceL2Id);
              //tell the phy to listen for the group
              m_cphySapProvider.at (0)->AddSlDestination (m_sidelinkConfiguration->m_sourceL2Id);
              m_cmacSapProvider.at (0)->AddSlDestination (m_sidelinkConfiguration->m_sourceL2Id);
            }
        }

      break;

    case IDLE_WAIT_SIB2:
    case IDLE_CONNECTING:
      NS_LOG_INFO ("Connecting, must wait to send message");
      break;

    case CONNECTED_NORMALLY:
    case CONNECTED_HANDOVER:
    case CONNECTED_PHY_PROBLEM:
    case CONNECTED_REESTABLISHING:
      NS_LOG_INFO ("Considering in coverage");

      m_sidelinkConfiguration->StartRelayService (serviceCode, model, role);
      //must enable reception of direct communication messages
      if (!rxSelf)
        {
          NS_LOG_INFO ("Notify lower layers to accept incoming message");
          //Add to the list of group to monitor for sidelink
          m_sidelinkConfiguration->m_rxGroup.push_back (m_sidelinkConfiguration->m_sourceL2Id);
          //tell the phy to listen for the group
          m_cphySapProvider.at (0)->AddSlDestination (m_sidelinkConfiguration->m_sourceL2Id);
          m_cmacSapProvider.at (0)->AddSlDestination (m_sidelinkConfiguration->m_sourceL2Id);
        }
      if (role == LteSlUeRrc::RemoteUE)
        {
          // Enable SD-RSRP measurement for Relay UE selection
          m_cphySapProvider.at (0)->EnableUeSdRsrpMeasurements ();
        }
      //Try to send to eNodeB
      SendSidelinkUeInformation (false, !rxSelf, wasAnnouncingInterested != m_sidelinkConfiguration->IsAnnouncingInterested (), wasMonitoringInterested != m_sidelinkConfiguration->IsMonitoringInterested ());
      break;

    default: // i.e. IDLE_RANDOM_ACCESS
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }

}

void
LteUeRrc::StopRelayService (uint32_t serviceCode)
{
  NS_LOG_FUNCTION (this);

  bool wasAnnouncingInterested = m_sidelinkConfiguration->IsAnnouncingInterested ();
  bool wasMonitoringInterested = m_sidelinkConfiguration->IsMonitoringInterested ();

  m_sidelinkConfiguration->StopRelayService (serviceCode);
  NS_LOG_INFO ("deleting new relay service for UE " << m_rnti );
  // Inform MAC: if no longer interested in announcing or monitoring, remove discovery pools
  //m_cmacSapProvider.at (0)->ModifyDiscTxPayloads (m_sidelinkConfiguration->m_announcePayloads);
  //m_cmacSapProvider.at (0)->ModifyDiscRxPayloads (m_sidelinkConfiguration->m_monitorPayloads);

  // Disable SD-RSRP measurement for Relay UE selection if it was enable
  // TODO: How to keep measuring if at least one service is still active?
  // To be addapted when the code support multiple services
  m_cphySapProvider.at (0)->DisableUeSdRsrpMeasurements ();


  switch (m_state)
    {
    case IDLE_START:
    case IDLE_CELL_SEARCH:
    case IDLE_WAIT_MIB_SIB1:
    case IDLE_WAIT_MIB:
    case IDLE_WAIT_SIB1:
    case IDLE_CAMPED_NORMALLY:
      NS_LOG_INFO ("Considering out of network");
      //Activate bearer using preconfiguration if available
      //TBD
      break;
    case CONNECTED_NORMALLY:
    case CONNECTED_HANDOVER:
    case CONNECTED_PHY_PROBLEM:
    case CONNECTED_REESTABLISHING:
      NS_LOG_INFO ("Considering in coverage");
      //Try to send to eNodeB
      SendSidelinkUeInformation (false, false, wasAnnouncingInterested != m_sidelinkConfiguration->IsAnnouncingInterested (), wasMonitoringInterested != m_sidelinkConfiguration->IsMonitoringInterested ());
      break;

    default:   // i.e. IDLE_RANDOM_ACCESS
      NS_FATAL_ERROR ("method unexpected in state " << ToString (m_state));
      break;
    }
}

void
LteUeRrc::ApplySidelinkDedicatedConfiguration (LteRrcSap::SlCommConfig config)
{
  NS_LOG_FUNCTION (this);

  //TODO: implement procedures described in 5.3.10.X
  // 1> if the RRCConnectionReconfiguration message includes the sl-CommConfig:
  //   2>       if commTxResources is included and set to setup:
  //     3>     from the next SC period use the resources indicated by commTxResources for Sidelink direct communication transmission, as specified in 5.X.4;
  // 2> else if commTxResources is included and set to release:
  //     3>     from the next SC period, release the resources allocated for Sidelink direct communication transmission previously configured by commTxResources;

  //pools are allocated per UE, not per destinations. A UE may have multiple pools
  //and we need to maintain the list of pools
  //pass information to MAC to change scheduling
  if (config.commTxResources == LteRrcSap::SlCommConfig::SETUP)
    {
      NS_LOG_INFO ("Received dedicated resources " << config.setup.setup);

      Ptr<SidelinkTxCommResourcePool> txPool = CreateObject<SidelinkTxCommResourcePool>();
      if (config.setup.setup == LteRrcSap::SlCommTxResourcesSetup::SCHEDULED)
        {
          txPool->SetPool (config.setup.scheduled.commTxConfig);
          txPool->SetScheduledTxParameters (config.setup.scheduled.crnti, config.setup.scheduled.macMainConfig, config.setup.scheduled.commTxConfig, 0, config.setup.scheduled.mcs);
        }
      else
        {
          NS_ASSERT (config.setup.ueSelected.havePoolToAdd);
          //configure lower layers to transmit the Sidelink control information and the corresponding data using the pool of resources indicated by the first entry in commTxPoolNormalDedicated;
          txPool->SetPool (config.setup.ueSelected.poolToAddModList.pools[0].pool);
          txPool->SetUeSelectedTxParameters (config.setup.ueSelected.poolToAddModList.pools[0].poolIdentity);
        }
      if (!m_txPool)
        {
          //previous pool was configure, let's see if it changed configuration
          if (m_txPool == txPool)
            { //TODO: check if we can avoid pushing pool again if not changed
              NS_LOG_DEBUG ("Pool configuration unchanged");
            }
        }
      m_txPool = txPool;

      std::list <uint32_t>::iterator it;
      std::list <uint32_t> destinations = m_sidelinkConfiguration->GetTxDestinations ();
      //int index = 0;
      //currently we can only use one pool so all groups will use the same one
      for (it = destinations.begin (); it != destinations.end (); it++)
        {
          m_cmacSapProvider.at (0)->AddSlCommTxPool (*it, txPool);
        }
      //inform PHY about pool
      m_cphySapProvider.at (0)->SetSlCommTxPool (txPool);

      //indicate NAS that bearer was established
      //TODO: we should only indicate this once per bearer
      for (std::list <uint32_t>::iterator it = destinations.begin (); it != destinations.end (); it++)
        {
          m_asSapUser->NotifySidelinkRadioBearerActivated (*it);
          m_sidelinkConfiguration->NotifySidelinkRadioBearerActivated (*it);
        }
    }
  else
    {
      //release all transmission pools
      m_cphySapProvider.at (0)->RemoveSlCommTxPool ();
    }
}

void
LteUeRrc::ApplySidelinkDedicatedConfiguration (LteRrcSap::SlDiscConfig config)
{
  NS_LOG_FUNCTION (this);
  if (config.discTxResources == LteRrcSap::SlDiscConfig::SETUP)
    {
      NS_LOG_INFO ("Received dedicated resources (discovery) " << config.setup.setup);

      Ptr<SidelinkTxDiscResourcePool> txPool = CreateObject<SidelinkTxDiscResourcePool>();

      if (config.setup.setup == LteRrcSap::SlDiscTxResourcesSetup::SCHEDULED)
        {
          txPool->SetPool (config.setup.scheduled.discTxConfig);
          txPool->SetScheduledTxParameters (config.setup.scheduled.discTxConfig, config.setup.scheduled.discTfIndexList, config.setup.scheduled.discHoppingConfigDisc);
        }
      else   //UE Selected
        {
          NS_ASSERT (config.setup.ueSelected.havePoolToAdd && config.setup.ueSelected.poolToAddModList.nbPools > 0);

          // all pools in the same list have the same criteria for poolSelection
          if (config.setup.ueSelected.poolToAddModList.pools[0].pool.txParameters.ueSelectedResourceConfig.poolSelection.selection == LteRrcSap::PoolSelection::RSRPBASED)
            {
              uint8_t i = 0;
              bool poolFound = false;
              while ((i < config.setup.ueSelected.poolToAddModList.nbPools)and (!poolFound))
                {
                  // retrieve upper and lower RSRP bounds
                  // make sure that the current rsrp vlaue is in-between threshLow and threshHigh;
                  // otherwise go to the next pool
                  uint32_t lowRsrp = config.setup.ueSelected.poolToAddModList.pools[i].pool.txParameters.ueSelectedResourceConfig.poolSelectionRsrpBased.threshLow;
                  uint32_t highRsrp = config.setup.ueSelected.poolToAddModList.pools[i].pool.txParameters.ueSelectedResourceConfig.poolSelectionRsrpBased.threshHigh;

                  // check the rsrp values : Value 0 corresponds to -infinity, value 1 to -110dBm,
                  // value 2 to -100dBm, and so on (i.e. in steps of 10dBm) until value 6,
                  // which corresponds to -60dBm, while value 7 corresponds to +infinity.
                  NS_ASSERT_MSG (lowRsrp <= highRsrp, "Invalid Rsrp limits : lower bound is greater than upper bound");
                  NS_ASSERT_MSG ((lowRsrp != 7)and (highRsrp != 0), "invalid RSRP limits values");

                  // apply the layer 3 filter before checking the pool
                  uint8_t componentCarrierId = 0; // Assumes only single carrier
                  SaveUeMeasurements (m_cellId, m_storedMeasValues[m_cellId].rsrp, m_storedMeasValues[m_cellId].rsrq, true, componentCarrierId);

                  //low = -infinity
                  if (lowRsrp == 0)
                    {
                      //high = + infinity
                      if (highRsrp == 7)
                        {
                          // any rsrp value would be ok : select first pool
                          txPool->SetPool (config.setup.ueSelected.poolToAddModList.pools[0].pool);
                          txPool->SetUeSelectedTxParameters (config.setup.ueSelected.poolToAddModList.pools[0].poolIdentity);
                          poolFound = true;
                        }
                      //high could be : -110 dBm to -60 dBm
                      else
                        {
                          // check if rsrp <= highRsrp
                          if (m_storedMeasValues[m_cellId].rsrp <= LteRrcSap::RsrpValueDbm (highRsrp))
                            {
                              txPool->SetPool (config.setup.ueSelected.poolToAddModList.pools[i].pool);
                              txPool->SetUeSelectedTxParameters (config.setup.ueSelected.poolToAddModList.pools[i].poolIdentity);
                              poolFound = true;
                            }
                        }
                    }
                  // low could be : -110 dBm to -60 dBm
                  else
                    {
                      //high = + infinity
                      if (highRsrp == 7)
                        {
                          // check if rsrp >= lowRsrp
                          if (m_storedMeasValues[m_cellId].rsrp >= LteRrcSap::RsrpValueDbm (lowRsrp))
                            {
                              txPool->SetPool (config.setup.ueSelected.poolToAddModList.pools[i].pool);
                              poolFound = true;
                            }
                        }
                      //high could be : -110 dBm to -60 dBm
                      else
                        {
                          // check if lowRsrp <= rsrp <= highRsrp
                          if (((m_storedMeasValues[m_cellId].rsrp >= LteRrcSap::RsrpValueDbm (lowRsrp))and (m_storedMeasValues[m_cellId].rsrp <= LteRrcSap::RsrpValueDbm (highRsrp))))
                            {
                              txPool->SetPool (config.setup.ueSelected.poolToAddModList.pools[i].pool);
                              poolFound = true;
                            }
                        }
                    }
                  //increment; so if the pool is not good, go to the next one
                  i++;
                }  //end while
              NS_ASSERT_MSG (poolFound, "No pool match the RSRP-based selection");
            }  //end if (rsrp)
          else
            {
              // ue selected : randomly selected using a uniform distribution
              txPool->SetPool (config.setup.ueSelected.poolToAddModList.pools[m_sidelinkConfiguration->m_rand->GetInteger (0, config.setup.ueSelected.poolToAddModList.nbPools - 1)].pool);
              txPool->SetUeSelectedTxParameters (config.setup.ueSelected.poolToAddModList.pools[m_sidelinkConfiguration->m_rand->GetInteger (0, config.setup.ueSelected.poolToAddModList.nbPools - 1)].poolIdentity);
            }
        }  //end else (UE Selected)

      m_sidelinkConfiguration->SetActiveTxDiscoveryPool (txPool);
      m_sidelinkConfiguration->StartAnnouncing ();
      //inform MAC about the pool
      m_cmacSapProvider.at (0)->SetSlDiscTxPool (txPool);

    }//end if (setup)
  else
    {
      //release all transmission pools
      //To be implemented
    }
}

void
LteUeRrc::SendSidelinkUeInformation (bool txComm, bool rxComm, bool txDisc, bool rxDisc)
{
  NS_LOG_FUNCTION (this << txComm << rxComm << txDisc << rxDisc);

  LteRrcSap::SidelinkUeInformation sidelinkUeInformation;
  sidelinkUeInformation.haveCommRxInterestedFreq = false;
  sidelinkUeInformation.haveCommTxResourceReq = false;
  sidelinkUeInformation.haveDiscRxInterest = false;
  sidelinkUeInformation.haveDiscTxResourceReq = false;

  std::map <uint16_t, LteSlUeRrc::LteSlCellConfiguration>::iterator it = m_sidelinkConfiguration->m_slMap.find (m_cellId);
  if (it != m_sidelinkConfiguration->m_slMap.end ())
    {
      //if SIB 18 received
      //  if configured to receive Sidelink communication
      //    set commRxInterestedFreq with Sidelink frequency

      //  if configuration to transmit Sidelink communication
      //    set commTxResourceReq with Sidelink frequency and list of destination
      if (m_sidelinkConfiguration->IsSlEnabled ())
        {
          if (it->second.haveSib18)
            {
              if (rxComm)
                {
                  sidelinkUeInformation.haveCommRxInterestedFreq = true;
                  sidelinkUeInformation.commRxInterestedFreq = GetUlEarfcn ();
                }
              if (txComm)
                {
                  std::list <uint32_t> destinations = m_sidelinkConfiguration->GetTxDestinations ();
                  sidelinkUeInformation.haveCommTxResourceReq = true;
                  sidelinkUeInformation.slCommTxResourceReq.carrierFreq = GetUlEarfcn ();
                  sidelinkUeInformation.slCommTxResourceReq.slDestinationInfoList.nbDestinations = destinations.size ();
                  std::list <uint32_t>::iterator it;
                  int index = 0;
                  NS_ASSERT_MSG (destinations.size () <= MAXSL_DEST, "Maximum number of destinations (" << MAXSL_DEST << ") reached");
                  for (it = destinations.begin (); it != destinations.end (); it++)
                    {
                      sidelinkUeInformation.slCommTxResourceReq.slDestinationInfoList.SlDestinationIdentity[index++] = *it;
                    }
                }
            }
        }

      // similar for SIB 19
      if (m_sidelinkConfiguration->IsDiscEnabled ())
        {
          if (it->second.haveSib19)
            {
              // UE interested in monitoring discovery announcements
              if ((rxDisc) && (m_sidelinkConfiguration->GetDiscInterFreq () ==  GetUlEarfcn ()))
                {
                  sidelinkUeInformation.haveDiscRxInterest = true;
                  sidelinkUeInformation.discRxInterest = true;
                }
              // UE interested in transmit discovery announcements
              if (txDisc)
                {
                  sidelinkUeInformation.haveDiscTxResourceReq = true;
                  NS_ASSERT_MSG (m_sidelinkConfiguration->GetDiscTxResources () > 0, "can't have 0 or negative resources for the discovery announcement. Check if DiscTxResources is defined for in-coverage or eNBs disabled for ou-of-coverage");
                  sidelinkUeInformation.discTxResourceReq = m_sidelinkConfiguration->GetDiscTxResources ();
                }
            }
        }
      // Record time
      m_sidelinkConfiguration->RecordTransmissionOfSidelinkUeInformation ();
      // send the message to eNodeB
      m_rrcSapUser->SendSidelinkUeInformation (sidelinkUeInformation);
    }
}

void LteUeRrc::DoNotifySidelinkReception (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this << (uint16_t)lcId << srcL2Id << dstL2Id);
  //add LC
  Ptr<LteSidelinkRadioBearerInfo> slbInfo = AddSlrb (srcL2Id, dstL2Id, lcId);
  NS_LOG_INFO ("Created new RX SLRB for group " << dstL2Id << " LCID=" << (slbInfo->m_logicalChannelIdentity & 0xF));
}

void LteUeRrc::DoNotifyDiscoveryReception (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);

  LteSlDiscHeader discHeader;
  p->RemoveHeader (discHeader);

  uint8_t msgType = discHeader.GetDiscoveryMsgType ();
  if (msgType == LteSlDiscHeader::DISC_OPEN_ANNOUNCEMENT || msgType == LteSlDiscHeader::DISC_RESTRICTED_QUERY || msgType == LteSlDiscHeader::DISC_RESTRICTED_RESPONSE)
    { //open or restricted announcement
      if (m_sidelinkConfiguration->IsMonitoringApp (msgType, discHeader.GetApplicationCode ()))
        {
          NS_LOG_INFO ("discovery message received by " << m_rnti);
          m_discoveryMonitoringTrace (m_imsi, m_cellId, m_rnti, discHeader);
          m_sidelinkConfiguration->RecvApplicationServiceDiscovery (msgType, discHeader.GetApplicationCode ());
        }
    }
  else if (msgType == LteSlDiscHeader::DISC_RELAY_ANNOUNCEMENT || msgType == LteSlDiscHeader::DISC_RELAY_RESPONSE)
    {
      if (m_sidelinkConfiguration->IsMonitoringRelayServiceCode (msgType, discHeader.GetRelayServiceCode ()))
        {
          NS_LOG_INFO ("Relay announcement message received by " << m_rnti);
          m_discoveryMonitoringTrace (m_imsi, m_cellId, m_rnti, discHeader);
          m_sidelinkConfiguration->RecvRelayServiceDiscovery (discHeader.GetRelayServiceCode (), discHeader.GetInfo (), discHeader.GetRelayUeId (), discHeader.GetStatusIndicator ());
        }
    }
  else if (msgType == LteSlDiscHeader::DISC_RELAY_SOLICITATION)
    {
      if (m_sidelinkConfiguration->IsMonitoringRelayServiceCode (msgType, discHeader.GetRelayServiceCode ()))
        {
          NS_LOG_INFO ("Relay request message received by " << m_rnti);
          m_discoveryMonitoringTrace (m_imsi, m_cellId, m_rnti, discHeader);
          m_sidelinkConfiguration->RecvRelayServiceDiscovery (discHeader.GetRelayServiceCode (), discHeader.GetInfo (), discHeader.GetURDSComposition (), discHeader.GetRelayUeId ());
        }
    }
}

void
LteUeRrc::SetSlssid (uint64_t slssid)
{
  NS_LOG_FUNCTION (this << slssid);
  m_slssId = slssid;
  //Pass it to lower layers as well
  m_cphySapProvider.at (0)->SetSlssId (slssid);
}

uint64_t
LteUeRrc::GetSlssid ()
{
  NS_LOG_FUNCTION (this);
  return m_slssId;
}

uint64_t
LteUeRrc::GetFrameNumber ()
{
  NS_LOG_FUNCTION (this);
  return m_currFrameNo;
}

uint64_t
LteUeRrc::GetSubFrameNumber ()
{
  NS_LOG_FUNCTION (this);
  return m_currSubframeNo;
}

void
LteUeRrc::InitiateSlssTransmission ()
{
  NS_LOG_FUNCTION (this);

  //Try to initiate SLSS transmissions only once ending an off-data period (didn't have data to transmit before)
  if (!m_hasDataToTransmit)
    {
      NS_LOG_LOGIC (this << " the UE has data to transmit");
      m_hasDataToTransmit = true;
      if (m_hasSyncRef && IsInTheInnerPartOfTheSyncRefCell (m_currSyncRef.slssid,m_currSyncRef.offset))
        {
          m_inInnerCellOfSyncRef = true;
          NS_LOG_LOGIC (this << " the UE is in the inner cell of the selected SyncRef, no SLSS transmissions initiated");
        }
      else
        {
          NS_LOG_LOGIC (this << " the UE is in the outer cell of the selected SyncRef, SLSS transmissions initiated");
          ActivateSlssTransmission ();
        }
    }
}

void
LteUeRrc::StopSlssTransmission ()
{
  NS_LOG_FUNCTION (this);

  //Try to stop SLSS transmissions only once ending an on-data period (had data to transmit before)
  if (m_hasDataToTransmit)
    {
      m_hasDataToTransmit = false;
      NS_LOG_LOGIC (this << " the UE has no data to transmit, stopping SLSS transmission");
      DeactivateSlssTransmission ();
    }
}

void LteUeRrc::ActivateSlssTransmission ()
{
  NS_LOG_FUNCTION (this);

  if (!m_slssTransmissionActive)
    {
      NS_LOG_LOGIC (this << " the SLSS transmission was not active, configuring it and activating it");
      m_slssTransmissionActive = true;
      LteRrcSap::SlPreconfiguration preconf = m_sidelinkConfiguration->GetSlPreconfiguration ();
      //Configure own  SLSSID and SyncOffsetIndicator
      if (m_hasSyncRef || m_currSyncRef.slssid != 0)
        {
          NS_LOG_LOGIC (this << " the UE has a selected SyncRef, using its SLSSID and the other offset indicator");
          //Use the values from the SyncRef
          m_slssId = m_currSyncRef.slssid;
          uint16_t currSyncRefSyncOffsetIndicator = (10 * m_currSyncRef.syncRefMib.directFrameNo + m_currSyncRef.syncRefMib.directSubframeNo) % 40;
          if (currSyncRefSyncOffsetIndicator == preconf.preconfigSync.syncOffsetIndicator1 )
            {
              m_txSlSyncOffsetIndicator = preconf.preconfigSync.syncOffsetIndicator2;
            }
          else
            {
              m_txSlSyncOffsetIndicator = preconf.preconfigSync.syncOffsetIndicator1;
            }
        }
      else
        {
          NS_LOG_LOGIC (this << " the UE does not have a selected SyncRef, selecting SLSSID and offset indicator randomly");
          //Choose SLSSID Randomly between 168 and 335
          SlChangeOfSyncRefStatParameters param;

          param.imsi = m_imsi;
          param.prevSlssid = m_slssId;
          param.prevRxOffset = 0;
          param.prevFrameNo = m_currFrameNo;
          param.prevSubframeNo = m_currSubframeNo;

          m_slssId = m_imsi * 10;

          param.currSlssid = m_slssId;
          param.currRxOffset = 0;
          param.currFrameNo = m_currFrameNo;
          param.currSubframeNo = m_currSubframeNo;

          //Notify lower layers about the new SlSSID
          SetSlssid (m_slssId);

          m_changeOfSyncRefTrace (param);

          //Choose SyncOffsetIndicator randomly between the two preconfigured values
          Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
          if (x->GetInteger (0,1))
            {
              m_txSlSyncOffsetIndicator = preconf.preconfigSync.syncOffsetIndicator1;
            }
          else
            {
              m_txSlSyncOffsetIndicator = preconf.preconfigSync.syncOffsetIndicator2;
            }
        }
      NS_LOG_DEBUG ("Randomly chosen offset = " << m_txSlSyncOffsetIndicator);
      //Schedule the sending of the first MIB-SL according the selected SyncOffsetIndicator
      uint16_t nextSLSS = 0;
      uint16_t currOffset = (m_currFrameNo * 10 + m_currSubframeNo) % 40;
      if ( currOffset < m_txSlSyncOffsetIndicator)
        {
          nextSLSS = m_txSlSyncOffsetIndicator - currOffset;
        }
      else
        {
          nextSLSS = 40 - currOffset + m_txSlSyncOffsetIndicator;
        }

      double offset = 1e6 - ((Simulator::Now ().GetNanoSeconds ()) - Simulator::Now ().GetMilliSeconds () * 1e6);
      double nextSLSSns = nextSLSS * 1e6 + offset;
      NS_LOG_INFO (this << " UE IMSI " << m_imsi << " activating SLSS transmission with SLSSID "
                        << m_slssId << ", first SLSS in " << NanoSeconds (nextSLSSns));
      m_slssTxEvent = Simulator::Schedule (NanoSeconds (nextSLSSns), &LteUeRrc::SendSlss, this);

      //m_slssTxEvent = Simulator::Schedule (MilliSeconds (nextSLSS), &LteUeRrc::SendSlss, this);
      //m_slssTxTime = MilliSeconds (Simulator::Now ().GetMilliSeconds () + nextSLSS);
    }
}

void LteUeRrc::DeactivateSlssTransmission ()
{
  NS_LOG_FUNCTION (this);

  if (m_slssTransmissionActive)
    {
      m_slssTransmissionActive = false;
      //cancel next transmission
      m_slssTxEvent.Cancel ();
    }
}

void LteUeRrc::SaveSubframeIndication (uint16_t frameNo, uint16_t subFrameNo)
{
  //NS_LOG_FUNCTION (this << frameNo << subFrameNo ); //To much overhead as it is called every ms
  m_currFrameNo = frameNo;
  m_currSubframeNo = subFrameNo;
}

void
LteUeRrc::SendSlss ()
{
  NS_LOG_FUNCTION (this);

  if (m_slssTransmissionActive)
    {
      NS_LOG_LOGIC (this << " the SLSS transmission is active");
      //Send a SLSS if the UE is supposed to send it now (e.g. it didn't change of timing/SyncRef)
      if ((m_currFrameNo * 10 + m_currSubframeNo) % 40 == m_txSlSyncOffsetIndicator)
        {
          NS_LOG_LOGIC (this << " the UE is supposed to send the SLSS now");

          LteRrcSap::MasterInformationBlockSL mibSl;
          LteRrcSap::SlPreconfiguration preconf = m_sidelinkConfiguration->GetSlPreconfiguration ();

          mibSl.slBandwidth = preconf.preconfigGeneral.slBandwidth;
          mibSl.inCoverage = m_inCoverage;

          //Calculate the Subframe indication when the MIB-SL will be received
          //There is a delay of 4 ms
          SidelinkCommResourcePool::SubframeInfo currentTime;
          currentTime.frameNo = m_currFrameNo - 1;
          currentTime.subframeNo = m_currSubframeNo - 1;
          SidelinkCommResourcePool::SubframeInfo delay;
          delay.frameNo = 0;
          /**
           * Note, we have still the frame and subframe number from the previous
           * subframe indication because the event triggering this method
           * happen before (but at the same time) the subframe indication event
           * at the PHY. Therefore, we are adding 5 subframes. Thus, adding 5
           * does not mean that this MIB-SL will be received after 5 ms of the
           * delay. The delay is still 4 ms.
           */
          delay.subframeNo = 5;
          SidelinkCommResourcePool::SubframeInfo mibTime = currentTime + delay;

          mibSl.directFrameNo = mibTime.frameNo + 1;
          mibSl.directSubframeNo = mibTime.subframeNo + 1;

          uint16_t slssid = (m_hasSyncRef ? m_currSyncRef.slssid : m_slssId );

          //Send the SLSS
          NS_LOG_INFO ("UE RRC with IMSI " << m_imsi << " has frame/subframe number = " << m_currFrameNo << "/" << m_currSubframeNo);
          NS_LOG_INFO ("mibSl parameters : slBandwidth = " << mibSl.slBandwidth
                                                           << ", inCoverage = " << mibSl.inCoverage
                                                           << ", directFrameNo = " << mibSl.directFrameNo
                                                           << ", directSubframeNo = " << mibSl.directSubframeNo
                                                           << ", slssid = " << slssid);

          MasterInformationBlockSlHeader mibslHeader;
          mibslHeader.SetMessage (mibSl);
          Ptr<Packet> p = Create<Packet>();
          p->AddHeader (mibslHeader);

          LteMacSapProvider::TransmitPduParameters params;
          params.rnti = m_rnti;
          params.srcL2Id = 0; //not used for MIB-SL messages
          params.dstL2Id = 0; //not used for MIB-SL messages
          params.lcid = 0; //not used for MIB-SL messages
          params.harqProcessId = 0; //not used for MIB-SL messages
          params.layer = 0; //not used for MIB-SL messages
          params.discMsg = false;
          params.mibslMsg = true;
          params.componentCarrierId = 0;
          params.pdu = p;

          m_SendSlssTrace (m_imsi,slssid,m_txSlSyncOffsetIndicator,mibSl.inCoverage,mibSl.directFrameNo,mibSl.directSubframeNo);

          m_macSapProvider->TransmitPdu (params);

        }

      //Calculate when the next SLSS is supposed to be send
      uint16_t nextSLSS = 0;
      uint16_t currOffset = (m_currFrameNo * 10 + m_currSubframeNo) % 40;

      if (currOffset < m_txSlSyncOffsetIndicator)
        {
          nextSLSS = m_txSlSyncOffsetIndicator - currOffset;
        }
      else
        {
          nextSLSS = 40 - currOffset + m_txSlSyncOffsetIndicator;
        }

      m_slssTxEvent = Simulator::Schedule (MilliSeconds (nextSLSS), &LteUeRrc::SendSlss, this);
      NS_LOG_LOGIC (this << " UE IMSI " << m_imsi << " scheduled a SLSS to be sent in " << nextSLSS << " ms");
    }
}

void
LteUeRrc::SaveSlssMeasurements (uint16_t slssid, uint16_t offset, double srsrp, bool useLayer3Filtering)
{
  NS_LOG_FUNCTION (this << slssid << offset << srsrp << useLayer3Filtering);

  std::map <std::pair<uint16_t,uint16_t>, SlssMeasValues>::iterator storedMeasIt = m_storedSlssMeasValues.find (std::pair<uint16_t,uint16_t> (slssid,offset));

  if (storedMeasIt != m_storedSlssMeasValues.end ())
    {
      NS_LOG_LOGIC (this << " This SyncRef was measured in the past");

      if (useLayer3Filtering)
        {
          LteRrcSap::SlPreconfiguration preconf = m_sidelinkConfiguration->GetSlPreconfiguration ();
          NS_LOG_LOGIC (this << " Using L3 filtering with preconfigured filterCoefficient: " << preconf.preconfigSync.filterCoefficient);

          bool valid = false;

          //Converting stored S-RSRP to linear units
          double storedSrsrp_W = std::pow (10.0,storedMeasIt->second.srsrp / 10.0) / 1000.0;

          double Fn_W;
          if (srsrp > m_minSrsrp)
            {
              NS_LOG_LOGIC (this << " The new SyncRef S-RSRP is above the minimum required, applying filter");

              double newSrsrp_W = std::pow (10.0,srsrp / 10.0) / 1000.0;

              //The filter to be used according to //TS 36.331 5.5.3.2 is
              //      F_n = (1-a)*F_{n-1} + a*M_n
              //and it is defined for an input rate of 200 ms.
              //If the input rate is different, the filter needs to be adapted such that
              //its time characteristics are preserved.
              //This is achieved using:
              //      F_n = (1-a)^(nP) F_{n-1} + a M_n
              //where nP is the number of 200 ms periods that have elapsed since last time
              //the quantity was filtered and stored

              double aSrsrp = std::pow (0.5, preconf.preconfigSync.filterCoefficient / 4.0); //TS 36.331 5.5.3.2 is
              double nP = (Simulator::Now ().GetMilliSeconds () -  storedMeasIt->second.timestamp.GetMilliSeconds ()) / 200;

              NS_LOG_DEBUG (Simulator::Now ().GetMilliSeconds () << " IMSI: " << m_imsi << " SL-SSID: " << slssid << " offset: " << offset << " S-RSRP: " << srsrp <<
                            " timestamp: " << storedMeasIt->second.timestamp.GetMilliSeconds ());

              Fn_W = std::pow ((1 - aSrsrp),nP) * storedSrsrp_W + aSrsrp * newSrsrp_W;
              valid = true;
            }
          else
            {
              NS_LOG_LOGIC (this << " The new SyncRef S-RSRP is below or equal to the minimum required... Ignoring measurement");
            }

          if (valid)
            {
              //Converting filtered value to dBm
              double Fn_dBm = 10 * log10 (1000 * (Fn_W));

              //If after the filtering, it decays below the minimum required, use the minimum required
              if (Fn_dBm <= m_minSrsrp)
                {
                  NS_LOG_LOGIC (this << " The L3 filtered SyncRef S-RSRP is below or equal to the minimum required, storing minimum required");
                  storedMeasIt->second.srsrp = m_minSrsrp;
                  storedMeasIt->second.timestamp = Simulator::Now ();
                }
              else
                {
                  NS_LOG_LOGIC (this << " The L3 filtered SyncRef S-RSRP is above the minimum required, storing it");
                  storedMeasIt->second.srsrp = Fn_dBm;
                  storedMeasIt->second.timestamp = Simulator::Now ();
                }
            }
        }
      else
        {
          NS_LOG_LOGIC (this << " Not using L3 filtering");
          storedMeasIt->second.srsrp = srsrp;
          storedMeasIt->second.timestamp = Simulator::Now ();
        }
    }
  else
    {
      NS_LOG_LOGIC (this << " First time measuring this SyncRef, storing first value");
      SlssMeasValues v;
      v.srsrp = srsrp;
      v.timestamp = Simulator::Now ();
      m_storedSlssMeasValues.insert (std::pair< std::pair<uint16_t,uint16_t>, SlssMeasValues> (std::pair<uint16_t,uint16_t> (slssid,offset), v));
      m_knownSlssidList.push_back (std::pair<uint16_t,uint16_t> (slssid,offset));
    }
  NS_LOG_INFO (this << " IMSI " << m_imsi << " measured SyncRef with slssid " << slssid << " and offset " << offset << " New S-RSRP "
                    << srsrp << " Stored S-RSRP " << storedMeasIt->second.srsrp);
}

void
LteUeRrc::SynchronizeToStrongestSyncRef ()
{
  NS_LOG_FUNCTION (this);

  uint64_t maxSrsrpSlssid = 0;
  uint64_t maxSrsrpOffset = 0;

  double maxSrsrp = -std::numeric_limits<double>::infinity ();

  //Determine the strongest SyncRef
  std::vector <std::pair<uint16_t,uint16_t> >::iterator RepIt;
  for (RepIt = m_lastReportedSlssidList.begin (); RepIt != m_lastReportedSlssidList.end (); ++RepIt)
    {
      std::map <std::pair<uint16_t,uint16_t>, SlssMeasValues>::iterator it = m_storedSlssMeasValues.find (std::pair<uint16_t,uint16_t> (*RepIt));
      if (it == m_storedSlssMeasValues.end ())
        {
          NS_LOG_ERROR (" The UE cannot find the S-RSRP measurements for the detected SyncRef SLSSID " << RepIt->first << " offset " << RepIt->second );
        }
      else
        {
          if (maxSrsrp < it->second.srsrp)
            {
              maxSrsrpSlssid = it->first.first;
              maxSrsrpOffset = it->first.second;
              maxSrsrp = it->second.srsrp;
            }
        }
    }

  NS_LOG_INFO (this << " The strongest SyncRef SLSSID " << maxSrsrpSlssid << " offset " << maxSrsrpOffset << " has a S-RSRP of " << maxSrsrp << " dBm" );

  LteRrcSap::SlPreconfiguration preconf = m_sidelinkConfiguration->GetSlPreconfiguration ();

  if (maxSrsrpSlssid == 0 || maxSrsrp - m_minSrsrp <  preconf.preconfigSync.syncRefMinHyst)
    {
      NS_LOG_LOGIC (this << " Unable to find suitable SyncRef");
      if (!m_slssTransmissionActive && m_hasDataToTransmit)
        {
          NS_LOG_LOGIC (this << " The UE is transmitting Sidelink communication... Activating own SLSS transmission");
          ActivateSlssTransmission ();
        }
    }
  else
    {
      NS_LOG_LOGIC (this << " The strongest SyncRef is suitable");

      //Verify we have the mib for the strongest SyncRef
      std::map <std::pair<uint16_t,uint16_t>, LteSlSyncParams>::iterator itMap
        = m_latestMibSlReceived.find (std::pair<uint16_t,uint16_t> (maxSrsrpSlssid,maxSrsrpOffset ));
      if (itMap == m_latestMibSlReceived.end ())
        {
          NS_LOG_ERROR (" The UE is unable to find the MIB-SL of the strongest SyncRef");
        }
      else
        {
          NS_LOG_LOGIC (this << " The MIB-SL of the strongest SyncRef was found... Instructing synchronization");

          NS_LOG_INFO (this << " UE IMSI " << m_imsi << " selected SyncRef slssid " << maxSrsrpSlssid << " offset " << maxSrsrpOffset);
          NS_LOG_INFO (this << " mibSl.slBandwidth " << (*itMap).second.syncRefMib.slBandwidth
                            << " mibSl.inCoverage " << (*itMap).second.syncRefMib.inCoverage
                            << " mibSl.directFrameNo " << (*itMap).second.syncRefMib.directFrameNo
                            << " mibSl.directSubframeNo " << (*itMap).second.syncRefMib.directSubframeNo
                            << " mibSl.rxFrameNo " << (*itMap).second.rxSubframe.frameNo
                            << " mibSl.rxSubframeNo " << (*itMap).second.rxSubframe.subframeNo);

          //Instruct the phy to synchronize with maxSrsrpSlssid
          m_cphySapProvider.at (0)->SynchronizeToSyncRef ((*itMap).second);
        }
    }
}

bool
LteUeRrc::IsInTheInnerPartOfTheSyncRefCell (uint16_t slssid, uint16_t offset)
{
  NS_LOG_FUNCTION (this << slssid << offset);

  LteRrcSap::SlPreconfiguration preconf = m_sidelinkConfiguration->GetSlPreconfiguration ();

  std::map <std::pair<uint16_t,uint16_t>, SlssMeasValues>::iterator itMap = m_storedSlssMeasValues.find (std::pair<uint16_t,uint16_t> (slssid,offset));
  if (itMap == m_storedSlssMeasValues.end ())
    {
      NS_LOG_ERROR (" The UE cannot find the S-RSRP measurements for the selected SyncRef SLSSID " << slssid << " offset " << offset);
      return false;
    }
  else
    {
      if ((*itMap).second.srsrp >= preconf.preconfigSync.syncTxThreshOoC)
        {
          NS_LOG_LOGIC (this << " The SyncRef S-RSRP is above or equal to the configured syncTxThreshOoC, i.e., the UE is in the inner part of the SyncRef cell");
          return true;
        }
      else
        {
          NS_LOG_LOGIC (this << " The SyncRef S-RSRP is below the configured syncTxThreshOoC, i.e., the UE is in the outer part of the SyncRef cell");
          return false;
        }
    }
}

void LteUeRrc::DoReceiveMibSL (Ptr<Packet> p, uint16_t slssid)
{
  NS_LOG_FUNCTION (this);

  MasterInformationBlockSlHeader mibSlHeader;
  p->PeekHeader (mibSlHeader);
  LteRrcSap::MasterInformationBlockSL mibSl = mibSlHeader.GetMessage ();

  uint16_t rxOffset = Simulator::Now ().GetMilliSeconds () % 40;

  NS_LOG_INFO (" UE IMSI " << m_imsi << " received MIB-SL at " << m_currFrameNo << "/" << m_currSubframeNo);
  NS_LOG_INFO (" mibSl.slBandwidth " << mibSl.slBandwidth
                                     << ", mibSl.inCoverage " << mibSl.inCoverage
                                     << ", mibSl.directFrameNo " << mibSl.directFrameNo
                                     << ", mibSl.directSubframeNo " << mibSl.directSubframeNo
                                     << ", slssid " << slssid
                                     << ", rxOffset " << rxOffset);

  //Store the mib
  std::map <std::pair<uint16_t,uint16_t>, LteSlSyncParams>::iterator itMap
    = m_latestMibSlReceived.find (std::pair<uint16_t,uint16_t> (slssid, rxOffset));

  if (itMap == m_latestMibSlReceived.end ())
    {
      //Insert new entry
      NS_LOG_LOGIC (this << " First received MIB-SL for SyncRef with SLSSID " << slssid << " offset " << rxOffset);
      LteSlSyncParams synchParams;
      synchParams.rxSubframe.frameNo = m_currFrameNo;
      synchParams.rxSubframe.subframeNo = m_currSubframeNo;
      synchParams.slssid = slssid;
      synchParams.offset = rxOffset;
      synchParams.syncRefMib = mibSl;


      m_latestMibSlReceived.insert (std::pair <std::pair<uint16_t,uint16_t>, LteSlSyncParams> (std::pair<uint16_t,uint16_t> (slssid, rxOffset), synchParams));
    }
  else
    {
      //Replace the entry
      NS_LOG_LOGIC (this << " Updating stored MIB-SL for SyncRef with SLSSID " << slssid << " offset " << rxOffset);
      (*itMap).second.syncRefMib.slBandwidth = mibSl.slBandwidth;
      (*itMap).second.syncRefMib.inCoverage = mibSl.inCoverage;
      (*itMap).second.syncRefMib.directFrameNo = mibSl.directFrameNo;
      (*itMap).second.syncRefMib.directSubframeNo = mibSl.directSubframeNo;
      (*itMap).second.offset = rxOffset;
      (*itMap).second.rxSubframe.frameNo = m_currFrameNo;
      (*itMap).second.rxSubframe.subframeNo = m_currSubframeNo;
    }
  //Verify if it is a MIB-SL from the current SyncRef
  if (m_hasSyncRef && slssid == m_currSyncRef.slssid)
    {
      NS_LOG_LOGIC (this << " The received MIB-SL is from the selected SyncRef (SLSSID " << slssid << " offset " << rxOffset << ")");
    }

}

void
LteUeRrc::DoReportSlssMeasurements (LteUeCphySapUser::UeSlssMeasurementsParameters params, uint16_t slssid, uint16_t offset)
{
  NS_LOG_FUNCTION (this);

  bool useLayer3Filtering = false;
  LteRrcSap::SlPreconfiguration preconf = m_sidelinkConfiguration->GetSlPreconfiguration ();
  if (preconf.preconfigSync.filterCoefficient != 0)
    {
      NS_LOG_LOGIC (this << " Using L3 filtering");
      useLayer3Filtering = true;
    }

  if (m_knownSlssidList.size () > 0 && slssid == 0)
    {
      //Measurement for SyncRef selection report
      NS_LOG_LOGIC (this << " The report corresponds to the measurement sub-process");

      //Storing the measurements of the known SLSSIDs
      std::vector <std::pair<uint16_t,uint16_t> >::iterator knownSlssidIit;
      for (knownSlssidIit = m_knownSlssidList.begin ();
           knownSlssidIit != m_knownSlssidList.end (); ++knownSlssidIit)
        {
          bool found = false;
          std::vector <LteUeCphySapUser::UeSlssMeasurementsElement>::iterator newMeasIt;
          for (newMeasIt = params.m_ueSlssMeasurementsList.begin ();
               newMeasIt != params.m_ueSlssMeasurementsList.end (); newMeasIt++)
            {
              if ( (*knownSlssidIit).first ==  newMeasIt->m_slssid && (*knownSlssidIit).second ==  newMeasIt->m_offset)
                {
                  NS_LOG_LOGIC (this << " UE IMSI " << m_imsi << " reported measurements from known SyncRef with SLSSID " << newMeasIt->m_slssid << " offset " << newMeasIt->m_offset << " S-RSRP " << newMeasIt->m_srsrp );
                  SaveSlssMeasurements (newMeasIt->m_slssid,newMeasIt->m_offset,newMeasIt->m_srsrp,useLayer3Filtering);
                  m_lastReportedSlssidList.push_back (std::pair<uint16_t,uint16_t> (newMeasIt->m_slssid,newMeasIt->m_offset));
                  found = true;
                  break;
                }
            }
          if (!found)
            {
              NS_LOG_LOGIC (this << " UE IMSI " << m_imsi << " did not report measurements from known SyncRef with SLSSID" << (*knownSlssidIit).first << " offset " << (*knownSlssidIit).second);
            }
        }
      //Adding the measures of the not known SLSSIDs
      std::vector <LteUeCphySapUser::UeSlssMeasurementsElement>::iterator newMeasIt;
      for (newMeasIt = params.m_ueSlssMeasurementsList.begin ();
           newMeasIt != params.m_ueSlssMeasurementsList.end (); newMeasIt++)
        {
          bool found = false;
          std::vector <std::pair<uint16_t,uint16_t> >::iterator knownSlssidIit;
          for (knownSlssidIit = m_knownSlssidList.begin ();
               knownSlssidIit != m_knownSlssidList.end (); ++knownSlssidIit)
            {
              if ( (*knownSlssidIit).first ==  newMeasIt->m_slssid && (*knownSlssidIit).second ==  newMeasIt->m_offset)
                {
                  found = true;
                  break;
                }
            }
          if (!found)
            {
              NS_LOG_LOGIC (this << " UE IMSI " << m_imsi << " reported measurements from not known SyncRef with SLSSID " << newMeasIt->m_slssid << " offset " << newMeasIt->m_offset << " S-RSRP " << newMeasIt->m_srsrp);
              SaveSlssMeasurements (newMeasIt->m_slssid,newMeasIt->m_offset, newMeasIt->m_srsrp,useLayer3Filtering);
              m_lastReportedSlssidList.push_back (std::pair<uint16_t,uint16_t> (newMeasIt->m_slssid,newMeasIt->m_offset));
              found = true;
            }
        }
    }
  else if (m_knownSlssidList.size () == 0) //The first report with detected SyncRefs
    {
      NS_LOG_LOGIC (this << " The report corresponds to the first measurement sub-process with detected SyncRefs");

      std::vector <LteUeCphySapUser::UeSlssMeasurementsElement>::iterator newMeasIt;
      for (newMeasIt = params.m_ueSlssMeasurementsList.begin ();
           newMeasIt != params.m_ueSlssMeasurementsList.end (); ++newMeasIt)
        {
          NS_LOG_LOGIC (this << " UE IMSI " << m_imsi << " reported measurements from not known SyncRef with SLSSID " << newMeasIt->m_slssid << " offset " << newMeasIt->m_offset << " S-RSRP " << newMeasIt->m_srsrp);
          SaveSlssMeasurements (newMeasIt->m_slssid,newMeasIt->m_offset, newMeasIt->m_srsrp,useLayer3Filtering);
          m_lastReportedSlssidList.push_back (std::pair<uint16_t,uint16_t> (newMeasIt->m_slssid,newMeasIt->m_offset));

        }
    }
  else if (slssid > 0)//Specific SLSSID report (i.e., evaluation of selected SyncRef)
    {
      NS_LOG_LOGIC (this << " The report corresponds to the evaluation sub-process");

      std::vector <LteUeCphySapUser::UeSlssMeasurementsElement>::iterator newMeasIt;
      if (params.m_ueSlssMeasurementsList.size () == 1)
        {
          newMeasIt = params.m_ueSlssMeasurementsList.begin ();
          if (newMeasIt->m_slssid == slssid)
            {
              NS_LOG_LOGIC (this << " UE IMSI " << m_imsi << " reported measurements from selected SyncRef with SLSSID " << newMeasIt->m_slssid << " offset " << newMeasIt->m_offset << " S-RSRP " << newMeasIt->m_srsrp);
              SaveSlssMeasurements (newMeasIt->m_slssid,newMeasIt->m_offset, newMeasIt->m_srsrp,useLayer3Filtering);
              m_lastReportedSlssidList.push_back (std::pair<uint16_t,uint16_t> (slssid,offset));
            }
          else
            {
              NS_LOG_ERROR (" The S-RSRP measurements does not correspond to the intended SyncRef with SLSSID " << slssid << " offset " << offset);
            }
        }
      else if (params.m_ueSlssMeasurementsList.size () == 0) //The SyncRef disappeared
        {
          NS_LOG_LOGIC (this << " The UE is unable to find the S-RSRP measurements of the selected SyncRef with SLSSID " << slssid << " offset " << offset);
        }
      else if (params.m_ueSlssMeasurementsList.size () > 1)
        {
          NS_LOG_ERROR (" The S-RSRP measurement report intended for evaluation of selected SyncRef has more than one SyncRef reported ");
        }
    }

  //Selection of the SyncRef
  if (m_hasSyncRef && slssid == 0)
    {
      NS_LOG_LOGIC (this << " Evaluating validity of current SyncRef");

      LteRrcSap::SlPreconfiguration preconf = m_sidelinkConfiguration->GetSlPreconfiguration ();
      //Select strongest candidate
      uint16_t maxSrsrpSlssid = 0;
      uint64_t maxSrsrpOffset = 0;
      bool syncRefDetected = false;
      double maxSrsrp = -std::numeric_limits<double>::infinity ();
      std::vector <std::pair<uint16_t,uint16_t> >::iterator RepIt;
      for (RepIt = m_lastReportedSlssidList.begin (); RepIt != m_lastReportedSlssidList.end (); ++RepIt)
        {
          if ( (*RepIt).first == m_currSyncRef.slssid && (*RepIt).second == m_currSyncRef.offset)
            {
              syncRefDetected = true;
            }
          std::map <std::pair<uint16_t,uint16_t>, SlssMeasValues>::iterator it = m_storedSlssMeasValues.find (std::pair<uint16_t,uint16_t> (*RepIt));
          if (it == m_storedSlssMeasValues.end ())
            {
              NS_LOG_ERROR (" The UE cannot find the S-RSRP measurements for the detected SyncRef SLSSID " << RepIt->first << " offset " << RepIt->second );
            }
          else
            {
              if (maxSrsrp < it->second.srsrp)
                {
                  maxSrsrpSlssid = it->first.first;
                  maxSrsrpOffset = it->first.second;
                  maxSrsrp = it->second.srsrp;
                }
            }
        }

      NS_LOG_INFO (this << " The strongest SyncRef SLSSID " << maxSrsrpSlssid << " offset " << maxSrsrpOffset << " has a S-RSRP of " << maxSrsrp << " dBm" );
      //Strongest candidate exceeds the minimum required?
      if (maxSrsrpSlssid != 0 && maxSrsrp - m_minSrsrp >  preconf.preconfigSync.syncRefMinHyst)
        {
          //Yes
          NS_LOG_LOGIC (this << " The strongest candidate S-RSRP exceeds the minimum required by the corresponding threshold syncRefMinHyst" );

          //The selected SyncRef was detected?
          if (!syncRefDetected)
            {
              //No... consider the selected SyncRef not valid
              NS_LOG_LOGIC (this << " The selected SyncRef was not detected... Considering it not valid" );
              m_hasSyncRef = false;
            }
          else
            {
              //Yes
              std::map <std::pair<uint16_t,uint16_t>, SlssMeasValues>::iterator itMap = m_storedSlssMeasValues.find (std::pair<uint16_t,uint16_t> (m_currSyncRef.slssid,m_currSyncRef.offset));
              if (itMap == m_storedSlssMeasValues.end ())
                {
                  NS_LOG_LOGIC (this << " The UE cannot find the S-RSRP measurements for the selected SyncRef... Considering it not valid"  );
                  m_hasSyncRef = false;
                }
              else
                {
                  //Strongest candidate better than the selected SyncRef?
                  if (maxSrsrp - (*itMap).second.srsrp > preconf.preconfigSync.syncRefDiffHyst)
                    {
                      //Yes... Consider not valid the selected SyncRef
                      NS_LOG_LOGIC (this << " The strongest candidate S-RSRP exceeds the selected SyncRef one by the corresponding threshold syncRefDiffHyst... Considering the selected SyncRef not valid" );
                      m_hasSyncRef = false;
                    }
                  else
                    {
                      //No... Keep the selected SyncRef
                      NS_LOG_LOGIC (this << " The strongest candidate S-RSRP does not exceed the selected SyncRef one by the corresponding threshold syncRefDiffHyst... Keeping the selected SyncRef" );
                    }
                }
            }
        }
      else
        {
          //No... Keep the selected SyncRef
          NS_LOG_LOGIC (this << " The strongest candidate S-RSRP does not exceed the minimum required by the corresponding threshold syncRefMinHyst" );
        }
    }
  if (!m_hasSyncRef && slssid == 0)
    {
      NS_LOG_LOGIC (this << " The UE does not have a (valid) selected SyncRef... Selecting the strongest candidate if appropriate" );
      //Evaluate candidates for selection of SyncRef
      SynchronizeToStrongestSyncRef ();
    }

  if (m_hasSyncRef && slssid > 0)
    {
      NS_LOG_LOGIC (this << " Evaluating S-RSRP measurements of the selected SyncRef" );

      if (IsInTheInnerPartOfTheSyncRefCell (m_currSyncRef.slssid, m_currSyncRef.offset))
        {
          NS_LOG_LOGIC (this << " the UE is in the inner cell of the selected SyncRef");
          if (m_slssTransmissionActive == true )
            {
              NS_LOG_LOGIC (this << " the UE was transmitting SLSS... Deactivating the SLSS transmission");
              DeactivateSlssTransmission ();
            }
          m_inInnerCellOfSyncRef = true;
        }
      else
        {
          NS_LOG_LOGIC (this << " the UE is in the outer cell of the selected SyncRef");

          if (m_inInnerCellOfSyncRef && m_hasDataToTransmit)
            {
              // It was in the inner cell, but not anymore

              NS_LOG_LOGIC (this << " the UE was in the inner cell of the selected SyncRef (but not anymore) "
                            "and it is transmitting Sidelink communication... Activating SLSS transmission");
              ActivateSlssTransmission ();
            }
          m_inInnerCellOfSyncRef = false;
        }
    }
  m_lastReportedSlssidList.clear ();
}

void
LteUeRrc::DoReportChangeOfSyncRef (LteSlSyncParams params)
{
  NS_LOG_FUNCTION (this);

  m_hasSyncRef = true;

  SlChangeOfSyncRefStatParameters traceParam;
  traceParam.imsi = m_imsi;

  //Old parameters
  traceParam.prevSlssid = m_slssId;
  traceParam.prevRxOffset = m_currSyncRef.offset;
  traceParam.prevFrameNo = m_currFrameNo;
  traceParam.prevSubframeNo = m_currSubframeNo;

  //Update the parameters
  //Update the current subframe indication (it updates m_currFrameNo and m_currSubframeNo)
  SaveSubframeIndication (params.newSubframe.frameNo,params.newSubframe.subframeNo);

  //Update current SyncRef info
  m_currSyncRef = params;
  m_slssId = params.slssid;

  traceParam.currSlssid = m_slssId;
  traceParam.currRxOffset = params.offset;
  traceParam.currFrameNo = m_currFrameNo;
  traceParam.currSubframeNo = m_currSubframeNo;

  m_changeOfSyncRefTrace (traceParam);

  NS_LOG_INFO (this << " UE IMSI " << m_imsi << " reported successful change of SyncRef, selected SyncRef SLSSID " << params.slssid << "offset " << params.offset);
}

void
LteUeRrc::DoReportSubframeIndication (uint16_t frameNo, uint16_t subFrameNo)
{
  //NS_LOG_FUNCTION (this << frameNo << subFrameNo ); // Too much overhead as it is called every ms
  SaveSubframeIndication (frameNo, subFrameNo);
}

void
LteUeRrc::DoNotifyMacHasSlDataToSend ()
{
  NS_LOG_FUNCTION (this);
  if (m_slssTransmissionEnabled)
    {
      NS_LOG_LOGIC (this << " The MAC notified that it has data to send, and SLSS transmissions are enabled... Initiating SLSS transmissions" );
      InitiateSlssTransmission ();
    }
}

void
LteUeRrc::DoNotifyMacHasNoSlDataToSend ()
{
  NS_LOG_FUNCTION (this);
  if (m_slssTransmissionEnabled)
    {
      NS_LOG_LOGIC (this << " The MAC notified that it does not have anymore data to send, and SLSS transmissions are enabled... Stoping SLSS transmissions" );
      StopSlssTransmission ();
    }
}

void
LteUeRrc::TransmitDiscoveryMessage (LteSlDiscHeader discHeader)
{
  NS_LOG_FUNCTION (this << m_rnti);
  //create message
  Ptr<Packet> p = Create<Packet>();

  p->AddHeader (discHeader);

  LteMacSapProvider::TransmitPduParameters params;
  params.rnti = m_rnti;
  params.srcL2Id = 0; //not used for discovery messages
  params.dstL2Id = 0; //not used for discovery messages
  params.lcid = 0; //not used  for discovery messages
  params.layer = 0; //not used  for discovery messages
  params.harqProcessId = 0; //not used for discovery messages
  params.discMsg = true;
  params.mibslMsg = false;
  params.componentCarrierId = 0;
  params.pdu = p;
  m_macSapProvider->TransmitPdu (params);
}


void
LteUeRrc::DoReportUeSdRsrpMeasurements (LteUeCphySapUser::UeSdRsrpMeasurementsParameters params)
{
  NS_LOG_FUNCTION (this << m_imsi);

  // Obtain Relay UE (re)selection parameters
  uint16_t layer3FilterCoefficient = 0;
  int16_t qRxLevMin = m_minSdRsrp;
  uint16_t minHyst = 0;
  std::map <uint16_t, LteSlUeRrc::LteSlCellConfiguration>::iterator it;

  switch (m_state)
    {
    case IDLE_START:
    case IDLE_CELL_SEARCH:
    case IDLE_WAIT_MIB_SIB1:
    case IDLE_WAIT_MIB:
    case IDLE_WAIT_SIB1:
    case IDLE_CAMPED_NORMALLY:
    case IDLE_WAIT_SIB2:
    case IDLE_CONNECTING:
    case IDLE_RANDOM_ACCESS:
      NS_LOG_INFO ("Considering out of network");
      //Obtain parameters from preconfiguration
      if (m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigRelay.haveReselectionInfoOoc == true)
        {
          layer3FilterCoefficient = m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigRelay.reselectionInfoOoc.filterCoefficient;
          qRxLevMin = m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigRelay.reselectionInfoOoc.qRxLevMin;
          minHyst = m_sidelinkConfiguration->GetSlPreconfiguration ().preconfigRelay.reselectionInfoOoc.minHyst;
        }
      else
        {
          NS_FATAL_ERROR ("Unable to find out-of-coverage Relay UE reselection info in the sidelink preconfiguration."
                          " Please make sure to properly configure the Relay UE reselection parameters while configuring the sidelink preconfiguration");
        }
      break;

    case CONNECTED_NORMALLY:
    case CONNECTED_HANDOVER:
    case CONNECTED_PHY_PROBLEM:
    case CONNECTED_REESTABLISHING:
      NS_LOG_INFO ("Considering in coverage");
      //Obtain parameters from SIB 19
      it = m_sidelinkConfiguration->m_slMap.find (m_cellId);
      if (it != m_sidelinkConfiguration->m_slMap.end () && it->second.haveSib19)
        {
          if (it->second.sib19.discConfigRelay.haveRemoteUeConfig && it->second.sib19.discConfigRelay.remoteUeConfig.haveReselectionInfoIc)
            {
              layer3FilterCoefficient = it->second.sib19.discConfigRelay.remoteUeConfig.reselectionInfoIc.filterCoefficient;
              qRxLevMin = it->second.sib19.discConfigRelay.remoteUeConfig.reselectionInfoIc.qRxLevMin;
              minHyst = it->second.sib19.discConfigRelay.remoteUeConfig.reselectionInfoIc.minHyst;
            }
          else
            {
              NS_FATAL_ERROR ("Unable to find in-coverage Relay UE reselection info in the stored SIB 19."
                              " Please make sure to properly set the Remote UE configuration and the Relay UE reselection parameters while configuring the sidelink parameters in the eNodeB");
            }
        }
      else
        {
          NS_LOG_DEBUG ("SIB 19 not found. Unable to Save SD-RSRP Measurements and perform Relay UE selection");
          return;
        }
      break;

    default:
      NS_FATAL_ERROR ("Unexpected state!");
      break;
    }

  NS_LOG_INFO (this << "Relay UE (re)selection parameters: "
                    << " filterCoefficient " << layer3FilterCoefficient
                    << " qRxLevMin " << qRxLevMin << " minHyst " <<  minHyst );

  //Store values (Applying L3 filter if needed)
  std::vector <LteUeCphySapUser::UeSdRsrpMeasurementsElement>::iterator newMeasIt;
  for (newMeasIt = params.m_ueSdRsrpMeasurementsList.begin ();
       newMeasIt != params.m_ueSdRsrpMeasurementsList.end (); newMeasIt++)
    {
      NS_LOG_INFO (this << " UE IMSI " << m_imsi
                        << " reported measurements from Relay ID " << newMeasIt->m_relayId
                        << " Service Code " << newMeasIt->m_serviceCode
                        << " SD-RSRP " << newMeasIt->m_sdRsrp << " dBm ");

      SaveSdRsrpMeasurements (newMeasIt->m_relayId, newMeasIt->m_serviceCode, newMeasIt->m_sdRsrp, layer3FilterCoefficient);
    }

  // Create a list of valid Relay UEs and their (l3 filtered) SD-RSRP
  // A Relay UE is valid if:
  // - Reported in this report
  // - (l3 filtered) SD-RSRP - q-RsLevMin > minHyst
  std::map <std::pair<uint64_t,uint32_t>, double> validRelays;
  for (newMeasIt = params.m_ueSdRsrpMeasurementsList.begin ();
       newMeasIt != params.m_ueSdRsrpMeasurementsList.end (); newMeasIt++)
    {
      std::map <std::pair<uint64_t,uint32_t>, SdRsrpMeasValue>::iterator storedMeasIt = m_storedSdRsrpMeasValues.find (std::pair<uint64_t,uint32_t> (newMeasIt->m_relayId,newMeasIt->m_serviceCode));
      if (storedMeasIt != m_storedSdRsrpMeasValues.end ()
          && (storedMeasIt->second.sdRsrp - qRxLevMin) > minHyst )
        {
          validRelays.insert (std::pair< std::pair<uint64_t,uint32_t>, double> (std::pair<uint64_t,uint32_t> (storedMeasIt->first.first, storedMeasIt->first.second), storedMeasIt->second.sdRsrp));
        }
    }

  // Evaluate Relay UE (re)selection
  m_sidelinkConfiguration->RelayUeSelection (validRelays);
}

void
LteUeRrc::SaveSdRsrpMeasurements (uint64_t relayId, uint32_t serviceCode, double sdRsrp, uint16_t layer3FilterCoefficient)
{
  NS_LOG_FUNCTION (this << m_imsi << relayId << serviceCode << sdRsrp << layer3FilterCoefficient);

  std::map <std::pair<uint64_t,uint32_t>, SdRsrpMeasValue>::iterator storedMeasIt =
    m_storedSdRsrpMeasValues.find (std::pair<uint64_t,uint32_t> (relayId,serviceCode));
  if (storedMeasIt != m_storedSdRsrpMeasValues.end ())
    {
      NS_LOG_LOGIC (this << " This Relay UE was measured in the past");
      if (layer3FilterCoefficient != 0)
        {
          NS_LOG_LOGIC (this << " Using L3 filtering with filterCoefficient: " << layer3FilterCoefficient);
          bool valid = false;

          //Converting stored SD-RSRP to linear units
          double storedSdRsrp_W = std::pow (10.0,storedMeasIt->second.sdRsrp / 10.0) / 1000.0;
          double Fn_W;
          if (sdRsrp > m_minSdRsrp)
            {
              NS_LOG_LOGIC (this << " The fresh SD-RSRP is above the minimum required, applying filter");
              double newSdRsrp_W = std::pow (10.0, sdRsrp / 10.0) / 1000.0;

              //The filter to be used according to TS 36.331 5.5.3.2 is
              //      F_n = (1-a)*F_{n-1} + a*M_n
              //and it is defined for an input rate of 200 ms.
              //If the input rate is different, the filter needs to be adapted such that
              //its time characteristics are preserved.
              //This is achieved using:
              //      F_n = (1-a)^(nP) F_{n-1} + a M_n
              //where nP is the number of 200 ms periods that have elapsed since last time
              //the quantity was filtered and stored
              double aSdRsrp = std::pow (0.5, layer3FilterCoefficient / 4.0); //TS 36.331 5.5.3.2
              double nP = (Simulator::Now ().GetMilliSeconds () -  storedMeasIt->second.timestamp.GetMilliSeconds ()) / 200;
              Fn_W = std::pow ((1 - aSdRsrp),nP) * storedSdRsrp_W + aSdRsrp * newSdRsrp_W;
              valid = true;
            }
          else
            {
              NS_LOG_LOGIC (this << " The fresh Relay UE S-RSRP is below or equal to the minimum required... Ignoring measurement");
            }

          if (valid)
            {
              //Converting filtered value to dBm
              double Fn_dBm = 10 * log10 (1000 * (Fn_W));

              //If after the filtering, it decays below the minimum required, use the minimum required
              if (Fn_dBm <= m_minSdRsrp)
                {
                  NS_LOG_LOGIC (this << " The L3 filtered SD-RSRP is below or equal to the minimum required, storing minimum required");
                  storedMeasIt->second.sdRsrp = m_minSdRsrp;
                  storedMeasIt->second.timestamp = Simulator::Now ();
                }
              else
                {
                  NS_LOG_LOGIC (this << " The L3 filtered SD-RSRP is above the minimum required, storing it");
                  storedMeasIt->second.sdRsrp = Fn_dBm;
                  storedMeasIt->second.timestamp = Simulator::Now ();
                }
            }
        }
      else
        {
          NS_LOG_LOGIC (this << " Not using L3 filtering, storing fresh value");
          storedMeasIt->second.sdRsrp = sdRsrp;
          storedMeasIt->second.timestamp = Simulator::Now ();
        }

      NS_LOG_INFO (this << " IMSI " << m_imsi << " measured Relay UE ID " << relayId
                        << " Service Code " << serviceCode
                        << " Fresh SD-RSRP " << sdRsrp << " Processed SD-RSRP " << storedMeasIt->second.sdRsrp);
    }
  else
    {
      NS_LOG_LOGIC (this << " First time measuring this Relay UE, storing first SD-RSRP value");
      SdRsrpMeasValue v;
      v.sdRsrp = sdRsrp;
      v.timestamp = Simulator::Now ();
      m_storedSdRsrpMeasValues.insert (std::pair< std::pair<uint64_t,uint32_t>, SdRsrpMeasValue> (std::pair<uint64_t,uint32_t> (relayId,serviceCode), v));

      NS_LOG_INFO (this << " IMSI " << m_imsi << " measured Relay UE ID " << relayId
                        << " Service Code " << serviceCode
                        << " Fresh SD-RSRP " << sdRsrp << " Processed SD-RSRP " << v.sdRsrp);
    }
}

} // namespace ns3

