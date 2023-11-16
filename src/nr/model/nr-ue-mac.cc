/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#define NS_LOG_APPEND_CONTEXT                                            \
  do                                                                     \
    {                                                                    \
      std::clog << " [ CellId " << GetCellId() << ", bwpId "             \
                << GetBwpId () << ", rnti " << m_rnti << "] ";           \
    }                                                                    \
  while (false);

#include "nr-ue-mac.h"
#include <ns3/log.h>
#include <ns3/boolean.h>
#include <ns3/pointer.h>
#include <ns3/lte-radio-bearer-tag.h>
#include <ns3/random-variable-stream.h>
#include <ns3/integer.h>
#include <ns3/double.h>
#include "nr-phy-sap.h"
#include "nr-control-messages.h"
#include "nr-mac-header-vs.h"
#include "nr-mac-short-bsr-ce.h"
#include "nr-sl-ue-mac-csched-sap.h"
#include "nr-sl-ue-mac-harq.h"
#include "nr-sl-sci-f1a-header.h"
#include "nr-sl-sci-f2a-header.h"
#include "nr-sl-mac-pdu-tag.h"
#include "ns3/lte-rlc-tag.h"
#include <algorithm>
#include <bitset>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrUeMac");
NS_OBJECT_ENSURE_REGISTERED (NrUeMac);

uint8_t NrUeMac::g_raPreambleId = 0;

///////////////////////////////////////////////////////////
// SAP forwarders
///////////////////////////////////////////////////////////


class UeMemberNrUeCmacSapProvider : public LteUeCmacSapProvider
{
public:
  UeMemberNrUeCmacSapProvider (NrUeMac* mac);

  // inherited from LteUeCmacSapProvider
  virtual void ConfigureRach (RachConfig rc);
  virtual void StartContentionBasedRandomAccessProcedure ();
  virtual void StartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t preambleId, uint8_t prachMask);
  virtual void AddLc (uint8_t lcId, LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu);
  virtual void RemoveLc (uint8_t lcId);
  virtual void Reset ();
  virtual void SetRnti (uint16_t rnti);
  virtual void NotifyConnectionSuccessful ();
  virtual void SetImsi (uint64_t imsi);

private:
  NrUeMac* m_mac;
};


UeMemberNrUeCmacSapProvider::UeMemberNrUeCmacSapProvider (NrUeMac* mac)
  : m_mac (mac)
{
}

void
UeMemberNrUeCmacSapProvider::ConfigureRach (RachConfig rc)
{
  m_mac->DoConfigureRach (rc);
}

void
UeMemberNrUeCmacSapProvider::StartContentionBasedRandomAccessProcedure ()
{
  m_mac->DoStartContentionBasedRandomAccessProcedure ();
}

void
UeMemberNrUeCmacSapProvider::StartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t preambleId, uint8_t prachMask)
{
  m_mac->DoStartNonContentionBasedRandomAccessProcedure (rnti, preambleId, prachMask);
}


void
UeMemberNrUeCmacSapProvider::AddLc (uint8_t lcId, LogicalChannelConfig lcConfig, LteMacSapUser* msu)
{
  m_mac->AddLc (lcId, lcConfig, msu);
}

void
UeMemberNrUeCmacSapProvider::RemoveLc (uint8_t lcid)
{
  m_mac->DoRemoveLc (lcid);
}

void
UeMemberNrUeCmacSapProvider::Reset ()
{
  m_mac->DoReset ();
}

void
UeMemberNrUeCmacSapProvider::SetRnti (uint16_t rnti)
{
  m_mac->SetRnti (rnti);
}

void
UeMemberNrUeCmacSapProvider::NotifyConnectionSuccessful ()
{
  m_mac->DoNotifyConnectionSuccessful ();
}

void
UeMemberNrUeCmacSapProvider::SetImsi (uint64_t imsi)
 {
   m_mac->DoSetImsi (imsi);
 }

class UeMemberNrMacSapProvider : public LteMacSapProvider
{
public:
  UeMemberNrMacSapProvider (NrUeMac* mac);

  // inherited from LteMacSapProvider
  virtual void TransmitPdu (TransmitPduParameters params);
  virtual void ReportBufferStatus (ReportBufferStatusParameters params);

private:
  NrUeMac* m_mac;
};


UeMemberNrMacSapProvider::UeMemberNrMacSapProvider (NrUeMac* mac)
  : m_mac (mac)
{
}

void
UeMemberNrMacSapProvider::TransmitPdu (TransmitPduParameters params)
{
  m_mac->DoTransmitPdu (params);
}


void
UeMemberNrMacSapProvider::ReportBufferStatus (ReportBufferStatusParameters params)
{
  m_mac->DoReportBufferStatus (params);
}


class NrUePhySapUser;

class MacUeMemberPhySapUser : public NrUePhySapUser
{
public:
  MacUeMemberPhySapUser (NrUeMac* mac);

  virtual void ReceivePhyPdu (Ptr<Packet> p) override;

  virtual void ReceiveControlMessage (Ptr<NrControlMessage> msg) override;

  virtual void SlotIndication (SfnSf sfn) override;

  //virtual void NotifyHarqDeliveryFailure (uint8_t harqId);

  virtual uint8_t GetNumHarqProcess () const override;

private:
  NrUeMac* m_mac;
};

MacUeMemberPhySapUser::MacUeMemberPhySapUser (NrUeMac* mac)
  : m_mac (mac)
{

}
void
MacUeMemberPhySapUser::ReceivePhyPdu (Ptr<Packet> p)
{
  m_mac->DoReceivePhyPdu (p);
}

void
MacUeMemberPhySapUser::ReceiveControlMessage (Ptr<NrControlMessage> msg)
{
  m_mac->DoReceiveControlMessage (msg);
}

void
MacUeMemberPhySapUser::SlotIndication (SfnSf sfn)
{
  m_mac->DoSlotIndication (sfn);
}

uint8_t
MacUeMemberPhySapUser::GetNumHarqProcess () const
{
  return m_mac->GetNumHarqProcess();
}


class MemberNrSlUeMacSchedSapUser : public NrSlUeMacSchedSapUser
{

public:
  /**
   * \brief constructor
   * \param mac The pointer to the NrUeMac using this SAP
   */
  MemberNrSlUeMacSchedSapUser (NrUeMac* mac);

  virtual void SchedUeNrSlConfigInd (uint32_t dstL2Id, const NrSlGrant& grant);
  virtual uint8_t GetTotalSubCh () const;
  virtual uint8_t GetSlMaxTxTransNumPssch () const;
  virtual bool SlotHasPsfch (const SfnSf& sfn) const;

private:
  NrUeMac* m_mac; //!< The pointer to the NrUeMac using this SAP
};

MemberNrSlUeMacSchedSapUser::MemberNrSlUeMacSchedSapUser (NrUeMac* mac)
:m_mac (mac)
{
}

void
MemberNrSlUeMacSchedSapUser::SchedUeNrSlConfigInd (uint32_t dstL2Id, const NrSlGrant& grant)
{
  m_mac->DoSchedUeNrSlConfigInd (dstL2Id, grant);
}

uint8_t
MemberNrSlUeMacSchedSapUser::GetTotalSubCh () const
{
  return m_mac->DoGetTotalSubCh ();
}

uint8_t
MemberNrSlUeMacSchedSapUser::GetSlMaxTxTransNumPssch () const
{
  return m_mac->DoGetSlMaxTxTransNumPssch ();
}

bool
MemberNrSlUeMacSchedSapUser::SlotHasPsfch (const SfnSf& sfn) const
{
  return (m_mac->DoSlotHasPsfch (sfn));
}

class MemberNrSlUeMacCschedSapUser : public NrSlUeMacCschedSapUser
{

public:
  MemberNrSlUeMacCschedSapUser (NrUeMac* mac);
  virtual void  CschedUeNrSlLcConfigCnf (uint8_t lcg, uint8_t lcId);


private:
  NrUeMac* m_mac;
};

MemberNrSlUeMacCschedSapUser::MemberNrSlUeMacCschedSapUser (NrUeMac* mac)
:m_mac (mac)
{
}

void
MemberNrSlUeMacCschedSapUser::CschedUeNrSlLcConfigCnf (uint8_t lcg, uint8_t lcId)
{
  m_mac->DoCschedUeNrSlLcConfigCnf (lcg, lcId);
}

//-----------------------------------------------------------------------

TypeId
NrUeMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrUeMac")
    .SetParent<Object> ()
    .AddConstructor<NrUeMac> ()
    .AddAttribute ("NumHarqProcess",
                   "Number of concurrent stop-and-wait Hybrid ARQ processes per user",
                    UintegerValue (20),
                    MakeUintegerAccessor (&NrUeMac::SetNumHarqProcess,
                                          &NrUeMac::GetNumHarqProcess),
                    MakeUintegerChecker<uint8_t> ())
    .AddTraceSource ("UeMacRxedCtrlMsgsTrace",
                     "Ue MAC Control Messages Traces.",
                     MakeTraceSourceAccessor (&NrUeMac::m_macRxedCtrlMsgsTrace),
                     "ns3::NrMacRxTrace::RxedUeMacCtrlMsgsTracedCallback")
    .AddTraceSource ("UeMacTxedCtrlMsgsTrace",
                     "Ue MAC Control Messages Traces.",
                     MakeTraceSourceAccessor (&NrUeMac::m_macTxedCtrlMsgsTrace),
                     "ns3::NrMacRxTrace::TxedUeMacCtrlMsgsTracedCallback")
    .AddAttribute ("EnableSensing",
                   "Flag to enable NR Sidelink resource selection based on sensing; otherwise, use random selection",
                   BooleanValue (false),
                   MakeBooleanAccessor (&NrUeMac::EnableSensing),
                   MakeBooleanChecker ())
    .AddAttribute ("Tproc0",
                   "t_proc0 in slots",
                   UintegerValue (1),
                   MakeUintegerAccessor (&NrUeMac::SetTproc0,
                                         &NrUeMac::GetTproc0),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("T1",
                   "The start of the selection window in physical slots, accounting for physical layer processing delay",
                   UintegerValue (2),
                   MakeUintegerAccessor (&NrUeMac::SetT1,
                                         &NrUeMac::GetT1),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("T2",
                   "The end of the selection window in physical slots",
                   UintegerValue (33),
                   MakeUintegerAccessor (&NrUeMac::SetT2,
                                         &NrUeMac::GetT2),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ActivePoolId",
                   "The pool id of the active pool used for TX and RX",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NrUeMac::SetSlActivePoolId,
                                         &NrUeMac::GetSlActivePoolId),
                   MakeUintegerChecker<uint16_t> ())
     .AddAttribute ("NumSidelinkProcess",
                    "Number of concurrent stop-and-wait Sidelink processes for this UE",
                    UintegerValue (4),
                    MakeUintegerAccessor (&NrUeMac::SetNumSidelinkProcess,
                                          &NrUeMac::GetNumSidelinkProcess),
                    MakeUintegerChecker<uint8_t> ())
     .AddAttribute ("MinTimeGapProcessing",
                    "Minimum time (in slots) for processing PSFCH and preparing retransmission",
                    UintegerValue (2),
                    MakeUintegerAccessor (&NrUeMac::m_minTimeGapProcessing),
                    MakeUintegerChecker<uint8_t> ())
     .AddAttribute ("SlThresPsschRsrp",
                    "A threshold in dBm used for sensing based UE autonomous resource selection",
                    IntegerValue (-128),
                    MakeIntegerAccessor (&NrUeMac::SetSlThresPsschRsrp,
                                         &NrUeMac::GetSlThresPsschRsrp),
                    MakeIntegerChecker<int> ())
     .AddAttribute ("ResourcePercentage",
                    "The percentage threshold to indicate the minimum number of"
                    "candidate single-slot resources to be selected using sensing"
                    "procedure",
                    UintegerValue (20),
                    MakeUintegerAccessor (&NrUeMac::SetResourcePercentage,
                                          &NrUeMac::GetResourcePercentage),
                    MakeUintegerChecker<uint8_t> (1, 100))
    .AddAttribute ("NrSlUeMacHarq",
                   "Pointer accessor to the NrSlUeMacHarq object",
                    PointerValue (),
                    MakePointerAccessor (&NrUeMac::m_nrSlHarq),
                    MakePointerChecker<NrSlUeMacHarq> ())
    .AddTraceSource ("SlPscchScheduling",
                     "Information regarding NR SL PSCCH UE scheduling",
                     MakeTraceSourceAccessor (&NrUeMac::m_slPscchScheduling),
                     "ns3::SlPscchUeMacStatParameters::TracedCallback")
    .AddTraceSource ("SlPsschScheduling",
                     "Information regarding NR SL PSSCH UE scheduling",
                     MakeTraceSourceAccessor (&NrUeMac::m_slPsschScheduling),
                     "ns3::SlPsschUeMacStatParameters::TracedCallback")
    .AddTraceSource ("RxRlcPduWithTxRnti",
                     "PDU received trace also exporting TX UE RNTI in SL.",
                     MakeTraceSourceAccessor (&NrUeMac::m_rxRlcPduWithTxRnti),
                     "ns3::NrUeMac::ReceiveWithTxRntiTracedCallback")
    .AddTraceSource ("SensingAlgorithm",
                     "Candidates selected by the mode 2 sensing algorithm",
                     MakeTraceSourceAccessor (&NrUeMac::m_tracedSensingAlgorithm),
                     "ns3::NrUeMac::SensingAlgorithmTracedCallback")
          ;

  return tid;
}

NrUeMac::NrUeMac (void) : Object ()
{
  NS_LOG_FUNCTION (this);
  m_cmacSapProvider = new UeMemberNrUeCmacSapProvider (this);
  m_macSapProvider = new UeMemberNrMacSapProvider (this);
  m_phySapUser = new MacUeMemberPhySapUser (this);
  m_raPreambleUniformVariable = CreateObject<UniformRandomVariable> ();
  //NR SL
  m_nrSlMacSapProvider = new MemberNrSlMacSapProvider <NrUeMac> (this);
  m_nrSlUeCmacSapProvider = new MemberNrSlUeCmacSapProvider<NrUeMac> (this);
  m_nrSlUePhySapUser = new MemberNrSlUePhySapUser<NrUeMac> (this);
  m_nrSlUeMacCschedSapUser = new MemberNrSlUeMacCschedSapUser (this);
  m_nrSlUeMacSchedSapUser = new MemberNrSlUeMacSchedSapUser (this);
  m_nrSlHarq = CreateObject<NrSlUeMacHarq> ();
}

NrUeMac::~NrUeMac (void)
{
}

void
NrUeMac::DoDispose ()
{
  m_miUlHarqProcessesPacket.clear ();
  m_miUlHarqProcessesPacketTimer.clear ();
  m_ulBsrReceived.clear ();
  m_lcInfoMap.clear ();
  m_raPreambleUniformVariable = nullptr;
  delete m_macSapProvider;
  delete m_cmacSapProvider;
  delete m_phySapUser;
  delete m_nrSlMacSapProvider;
  delete m_nrSlUeCmacSapProvider;
  delete m_nrSlUePhySapUser;
  delete m_nrSlUeMacCschedSapUser;
  delete m_nrSlUeMacSchedSapUser;
  m_nrSlHarq->Dispose ();
  m_nrSlHarq = nullptr;
  if (m_nrSlUeMacScheduler)
    {
      m_nrSlUeMacScheduler->Dispose ();
      m_nrSlUeMacScheduler = nullptr;
    }
  m_slTxPool = nullptr;
  m_slRxPool = nullptr;
}

void
NrUeMac::SetRnti (uint16_t rnti)
{
  NS_LOG_FUNCTION (this);
  m_rnti = rnti;
}

void
NrUeMac::DoNotifyConnectionSuccessful ()
{
  NS_LOG_FUNCTION (this);
  m_phySapProvider->NotifyConnectionSuccessful ();
}

void
NrUeMac::DoSetImsi (uint64_t imsi)
{
  NS_LOG_FUNCTION (this);
  m_imsi = imsi;
}

uint16_t
NrUeMac::GetBwpId () const
{
  if (m_phySapProvider)
    {
      return m_phySapProvider->GetBwpId ();
    }
  else
    {
      return UINT16_MAX;
    }
}

uint16_t
NrUeMac::GetCellId () const
{
  if (m_phySapProvider)
    {
      return m_phySapProvider->GetCellId ();
    }
  else
    {
      return UINT16_MAX;
    }
}

uint32_t
NrUeMac::GetTotalBufSize () const
{
  uint32_t ret = 0;
  for (auto it = m_ulBsrReceived.cbegin (); it != m_ulBsrReceived.cend (); ++it)
    {
      ret += ((*it).second.txQueueSize + (*it).second.retxQueueSize + (*it).second.statusPduSize);
    }
  return ret;
}

/**
 * \brief Sets the number of HARQ processes
 * \param numHarqProcesses the maximum number of harq processes
 */
void
NrUeMac::SetNumHarqProcess (uint8_t numHarqProcess)
{
  NS_LOG_FUNCTION (this << numHarqProcess);
  m_numHarqProcess = numHarqProcess;

  m_miUlHarqProcessesPacket.resize (GetNumHarqProcess ());
  for (uint8_t i = 0; i < m_miUlHarqProcessesPacket.size (); i++)
    {
      if (m_miUlHarqProcessesPacket.at (i).m_pktBurst == nullptr)
        {
          Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
          m_miUlHarqProcessesPacket.at (i).m_pktBurst = pb;
        }
    }
  m_miUlHarqProcessesPacketTimer.resize (GetNumHarqProcess (), 0);
}

/**
 * \return number of HARQ processes
 */
uint8_t
NrUeMac::GetNumHarqProcess () const
{
  return m_numHarqProcess;
}

void
NrUeMac::SetNrSlUeMacScheduler (Ptr<NrSlUeMacScheduler> scheduler)
{
  NS_LOG_FUNCTION (this << scheduler);
  m_nrSlUeMacScheduler = scheduler;
}

// forwarded from MAC SAP
void
NrUeMac::DoTransmitPdu (LteMacSapProvider::TransmitPduParameters params)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_ulDci->m_harqProcess == params.harqProcessId);

  m_miUlHarqProcessesPacket.at (params.harqProcessId).m_lcidList.push_back (params.lcid);

  NrMacHeaderVs header;
  header.SetLcId (params.lcid);
  header.SetSize (params.pdu->GetSize ());

  params.pdu->AddHeader (header);

  LteRadioBearerTag bearerTag (params.rnti, params.lcid, params.layer);
  params.pdu->AddPacketTag (bearerTag);

  NS_LOG_INFO("Received packet from RLC for HARQ ID " << +params.harqProcessId << " LC ID " << +params.lcid << " size " << params.pdu->GetSize ());
  m_miUlHarqProcessesPacket.at (params.harqProcessId).m_pktBurst->AddPacket (params.pdu);
  m_miUlHarqProcessesPacketTimer.at (params.harqProcessId) = GetNumHarqProcess();

  m_ulDciTotalUsed += params.pdu->GetSize ();

  NS_ASSERT_MSG (m_ulDciTotalUsed <= m_ulDci->m_tbSize.at (0), "We used more data than the DCI allowed us.");

  m_phySapProvider->SendMacPdu (params.pdu, m_ulDciSfnsf, m_ulDci->m_symStart, params.layer);
}

void
NrUeMac::DoReportBufferStatus (LteMacSapProvider::ReportBufferStatusParameters params)
{
  NS_LOG_FUNCTION (this << static_cast<uint32_t> (params.lcid));

  auto it = m_ulBsrReceived.find (params.lcid);

  NS_LOG_INFO ("Received BSR for LC Id" << static_cast<uint32_t>(params.lcid));

  if (it != m_ulBsrReceived.end ())
    {
      // update entry
      (*it).second = params;
    }
  else
    {
      it = m_ulBsrReceived.insert (std::make_pair (params.lcid, params)).first;
    }

  if (m_srState == INACTIVE)
    {
      NS_LOG_INFO ("INACTIVE -> TO_SEND, bufSize " << GetTotalBufSize ());
      m_srState = TO_SEND;
    }
}


void
NrUeMac::SendReportBufferStatus (const SfnSf &dataSfn, uint8_t symStart)
{
  NS_LOG_FUNCTION (this);

  if (m_rnti == 0)
    {
      NS_LOG_INFO ("MAC not initialized, BSR deferred");
      return;
    }

  if (m_ulBsrReceived.size () == 0)
    {
      NS_LOG_INFO ("No BSR report to transmit");
      return;
    }
  MacCeElement bsr = MacCeElement ();
  bsr.m_rnti = m_rnti;
  bsr.m_macCeType = MacCeElement::BSR;

  // BSR is reported for each LCG
  std::unordered_map <uint8_t, LteMacSapProvider::ReportBufferStatusParameters>::iterator it;
  std::vector<uint32_t> queue (4, 0);   // one value per each of the 4 LCGs, initialized to 0
  for (it = m_ulBsrReceived.begin (); it != m_ulBsrReceived.end (); it++)
    {
      uint8_t lcid = it->first;
      std::unordered_map <uint8_t, LcInfo>::iterator lcInfoMapIt;
      lcInfoMapIt = m_lcInfoMap.find (lcid);
      NS_ASSERT (lcInfoMapIt !=  m_lcInfoMap.end ());
      NS_ASSERT_MSG ((lcid != 0) || (((*it).second.txQueueSize == 0)
                                     && ((*it).second.retxQueueSize == 0)
                                     && ((*it).second.statusPduSize == 0)),
                     "BSR should not be used for LCID 0");
      uint8_t lcg = lcInfoMapIt->second.lcConfig.logicalChannelGroup;
      queue.at (lcg) += ((*it).second.txQueueSize + (*it).second.retxQueueSize + (*it).second.statusPduSize);
    }

  NS_LOG_INFO ("Sending BSR with this info for the LCG: " << queue.at (0) << " " <<
               queue.at (1) << " " << queue.at(2) << " " << queue.at(3));
  // FF API says that all 4 LCGs are always present
  bsr.m_macCeValue.m_bufferStatus.push_back (NrMacShortBsrCe::FromBytesToLevel (queue.at (0)));
  bsr.m_macCeValue.m_bufferStatus.push_back (NrMacShortBsrCe::FromBytesToLevel (queue.at (1)));
  bsr.m_macCeValue.m_bufferStatus.push_back (NrMacShortBsrCe::FromBytesToLevel (queue.at (2)));
  bsr.m_macCeValue.m_bufferStatus.push_back (NrMacShortBsrCe::FromBytesToLevel (queue.at (3)));

  // create the message. It is used only for tracing, but we don't send it...
  Ptr<NrBsrMessage> msg = Create<NrBsrMessage> ();
  msg->SetSourceBwp (GetBwpId ());
  msg->SetBsr (bsr);

  m_macTxedCtrlMsgsTrace (m_currentSlot, GetCellId (), bsr.m_rnti, GetBwpId (), msg);

  // Here we send the real SHORT_BSR, as a subpdu.
  Ptr<Packet> p = Create<Packet> ();

  // Please note that the levels are defined from the standard. In this case,
  // we have 5 bit available, so use such standard levels. In the future,
  // when LONG BSR will be implemented, this have to change.
  NrMacShortBsrCe header;
  header.m_bufferSizeLevel_0 = NrMacShortBsrCe::FromBytesToLevel (queue.at (0));
  header.m_bufferSizeLevel_1 = NrMacShortBsrCe::FromBytesToLevel (queue.at (1));
  header.m_bufferSizeLevel_2 = NrMacShortBsrCe::FromBytesToLevel (queue.at (2));
  header.m_bufferSizeLevel_3 = NrMacShortBsrCe::FromBytesToLevel (queue.at (3));

  p->AddHeader (header);

  LteRadioBearerTag bearerTag (m_rnti, NrMacHeaderFsUl::SHORT_BSR, 0);
  p->AddPacketTag (bearerTag);

  m_ulDciTotalUsed += p->GetSize ();
  NS_ASSERT_MSG (m_ulDciTotalUsed <= m_ulDci->m_tbSize.at (0), "We used more data than the DCI allowed us.");

  //MIMO is not supported for UL yet.
  //Therefore, there will be only
  //one stream with stream Id 0.
  uint8_t streamId = 0;

  m_phySapProvider->SendMacPdu (p, dataSfn, symStart, streamId);
}

void
NrUeMac::SetUeCmacSapUser (LteUeCmacSapUser* s)
{
  m_cmacSapUser = s;
}

LteUeCmacSapProvider*
NrUeMac::GetUeCmacSapProvider (void)
{
  return m_cmacSapProvider;
}

void
NrUeMac::RefreshHarqProcessesPacketBuffer (void)
{
  NS_LOG_FUNCTION (this);

  for (uint16_t i = 0; i < m_miUlHarqProcessesPacketTimer.size (); i++)
    {
      if (m_miUlHarqProcessesPacketTimer.at (i) == 0 && m_miUlHarqProcessesPacket.at (i).m_pktBurst)
        {
          if (m_miUlHarqProcessesPacket.at (i).m_pktBurst->GetSize () > 0)
            {
              // timer expired: drop packets in buffer for this process
              NS_LOG_INFO ("HARQ Proc Id " << i << " packets buffer expired");
              Ptr<PacketBurst> emptyPb = CreateObject <PacketBurst> ();
              m_miUlHarqProcessesPacket.at (i).m_pktBurst = emptyPb;
              m_miUlHarqProcessesPacket.at (i).m_lcidList.clear ();
            }
        }
      else
        {
          //m_miUlHarqProcessesPacketTimer.at (i)--;  // ignore HARQ timeout
        }
    }
}

void
NrUeMac::DoSlotIndication (const SfnSf &sfn)
{
  NS_LOG_FUNCTION (this);
  m_currentSlot = sfn;
  NS_LOG_INFO ("Slot " << m_currentSlot);

  RefreshHarqProcessesPacketBuffer ();

  if (m_srState == TO_SEND)
    {
      NS_LOG_INFO ("Sending SR to PHY in slot " << sfn);
      SendSR ();
      m_srState = ACTIVE;
    }
  bool isSidelinkSlot = false;
  if (m_slTxPool)
    {
      isSidelinkSlot = m_slTxPool->IsSidelinkSlot (GetBwpId (), m_poolId, sfn.Normalize ());
    }
  if (m_nrSlUeMacSchedSapProvider)
    {
      if (m_enableSensing)
        {
          RemoveOldSensingData (sfn,
            m_slTxPool->GetNrSlSensWindInSlots (GetBwpId (), m_poolId, m_nrSlUePhySapProvider->GetSlotPeriod ()),
            m_sensingData, m_imsi);
          RemoveOldTransmitHistory (sfn,
            m_slTxPool->GetNrSlSensWindInSlots (GetBwpId (), m_poolId, m_nrSlUePhySapProvider->GetSlotPeriod ()),
            m_transmitHistory, m_imsi);
        }

      m_nrSlUeMacSchedSapProvider->SchedUeNrSlTriggerReq (sfn, m_nrSlHarq->GetAvailableHarqIds ());

    }
  if (m_nrSlUeCmacSapUser != nullptr)
    {
      //trigger SL only when it is a SL slot
      if (isSidelinkSlot)
        {
          DoNrSlSlotIndication (sfn);
        }
    }
  // Feedback missing
}

void
NrUeMac::SendSR () const
{
  NS_LOG_FUNCTION (this);

  if (m_rnti == 0)
    {
      NS_LOG_INFO ("MAC not initialized, SR deferred");
      return;
    }

  // create the SR to send to the gNB
  Ptr<NrSRMessage> msg = Create<NrSRMessage> ();
  msg->SetSourceBwp (GetBwpId ());
  msg->SetRNTI (m_rnti);

  m_macTxedCtrlMsgsTrace (m_currentSlot, GetCellId (), m_rnti, GetBwpId (), msg);
  m_phySapProvider->SendControlMessage (msg);
}

void
NrUeMac::DoReceivePhyPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);

  LteRadioBearerTag tag;
  p->RemovePacketTag (tag);

  if (tag.GetRnti() != m_rnti) // Packet is for another user
    {
      return;
    }

  NrMacHeaderVs header;
  p->RemoveHeader (header);

  LteMacSapUser::ReceivePduParameters rxParams;
  rxParams.p = p;
  rxParams.rnti = m_rnti;
  rxParams.lcid = header.GetLcId ();

  auto it = m_lcInfoMap.find (header.GetLcId());

  // p can be empty. Well, right now no, but when someone will add CE in downlink,
  // then p can be empty.
  if (rxParams.p->GetSize () > 0)
    {
      it->second.macSapUser->ReceivePdu (rxParams);
    }
}

void
NrUeMac::RecvRaResponse (BuildRarListElement_s raResponse)
{
  NS_LOG_FUNCTION (this);
  m_waitingForRaResponse = false;
  m_rnti = raResponse.m_rnti;
  m_cmacSapUser->SetTemporaryCellRnti (m_rnti);
  m_cmacSapUser->NotifyRandomAccessSuccessful ();
}

void
NrUeMac::ProcessUlDci (const Ptr<NrUlDciMessage> &dciMsg)
{
  NS_LOG_FUNCTION (this);

  SfnSf dataSfn = m_currentSlot;
  dataSfn.Add (dciMsg->GetKDelay ());

  // Saving the data we need in DoTransmitPdu
  m_ulDciSfnsf = dataSfn;
  m_ulDciTotalUsed = 0;
  m_ulDci = dciMsg->GetDciInfoElement ();

  m_macRxedCtrlMsgsTrace (m_currentSlot, GetCellId (), m_rnti, GetBwpId (), dciMsg);

  NS_LOG_INFO ("UL DCI received, transmit data in slot " << dataSfn <<
               " Harq Process " << +m_ulDci->m_harqProcess <<
               " TBS " << m_ulDci->m_tbSize.at (0) << " total queue " << GetTotalBufSize ());

  if (m_ulDci->m_ndi.at (0) == 0)
    {
      // This method will retransmit the data saved in the harq buffer
      TransmitRetx ();

      // This method will transmit a new BSR.
      SendReportBufferStatus (dataSfn, m_ulDci->m_symStart);
    }
  else if (m_ulDci->m_ndi.at (0) == 1)
    {
      SendNewData ();

      NS_LOG_INFO ("After sending NewData, bufSize " << GetTotalBufSize ());

      // Send a new BSR. SendNewData() already took into account the size of
      // the BSR.
      SendReportBufferStatus (dataSfn, m_ulDci->m_symStart);

      NS_LOG_INFO ("UL DCI processing done, sent to PHY a total of " << m_ulDciTotalUsed <<
                   " B out of " << m_ulDci->m_tbSize.at (0) << " allocated bytes ");

      if (GetTotalBufSize () == 0)
        {
          m_srState = INACTIVE;
          NS_LOG_INFO ("ACTIVE -> INACTIVE, bufSize " << GetTotalBufSize ());

          // the UE may have been scheduled, but we didn't use a single byte
          // of the allocation. So send an empty PDU. This happens because the
          // byte reporting in the BSR is not accurate, due to RLC and/or
          // BSR quantization.
          if (m_ulDciTotalUsed == 0)
            {
              NS_LOG_WARN ("No byte used for this UL-DCI, sending empty PDU");

              LteMacSapProvider::TransmitPduParameters txParams;

              txParams.pdu = Create<Packet> ();
              txParams.lcid = 3;
              txParams.rnti = m_rnti;
              txParams.layer = 0;
              txParams.harqProcessId = m_ulDci->m_harqProcess;
              txParams.componentCarrierId = GetBwpId ();

              DoTransmitPdu (txParams);
            }
        }
    }
}

void
NrUeMac::TransmitRetx ()
{
  NS_LOG_FUNCTION (this);

  Ptr<PacketBurst> pb = m_miUlHarqProcessesPacket.at (m_ulDci->m_harqProcess).m_pktBurst;

  if (pb == nullptr)
    {
      NS_LOG_WARN ("The previous transmission did not contain any new data; "
                   "probably it was BSR only. To not send an old BSR to the scheduler, "
                   "we don't send anything back in this allocation. Eventually, "
                   "the Harq timer at gnb will expire, and soon this allocation will be forgotten.");
      return;
    }

  NS_LOG_DEBUG ("UE MAC RETX HARQ " << + m_ulDci->m_harqProcess);

  NS_ASSERT (pb->GetNPackets() > 0);

  for (std::list<Ptr<Packet> >::const_iterator j = pb->Begin (); j != pb->End (); ++j)
    {
      Ptr<Packet> pkt = (*j)->Copy ();
      LteRadioBearerTag bearerTag;
      if (!pkt->PeekPacketTag (bearerTag))
        {
          NS_FATAL_ERROR ("No radio bearer tag");
        }
      //MIMO is not supported for UL yet.
      //Therefore, there will be only
      //one stream with stream Id 0.
      uint8_t streamId = 0;
      m_phySapProvider->SendMacPdu (pkt, m_ulDciSfnsf, m_ulDci->m_symStart, streamId);
    }

  m_miUlHarqProcessesPacketTimer.at (m_ulDci->m_harqProcess) = GetNumHarqProcess();
}

void
NrUeMac::SendRetxData (uint32_t usefulTbs, uint32_t activeLcsRetx)
{
  NS_LOG_FUNCTION (this);

  if (activeLcsRetx == 0)
    {
      return;
    }

  uint32_t bytesPerLcId = usefulTbs / activeLcsRetx;

  for (auto & itBsr : m_ulBsrReceived)
    {
      auto &bsr = itBsr.second;

      if (m_ulDciTotalUsed + bytesPerLcId <= usefulTbs)
        {
          LteMacSapUser::TxOpportunityParameters txParams;
          txParams.lcid = bsr.lcid;
          txParams.rnti = m_rnti;
          txParams.bytes = bytesPerLcId;
          txParams.layer = 0;
          txParams.harqId = m_ulDci->m_harqProcess;
          txParams.componentCarrierId = GetBwpId ();

          NS_LOG_INFO ("Notifying RLC of LCID " << +bsr.lcid << " of a TxOpp "
                       "of " << bytesPerLcId << " B for a RETX PDU");

          m_lcInfoMap.at (bsr.lcid).macSapUser->NotifyTxOpportunity (txParams);
          // After this call, m_ulDciTotalUsed has been updated with the
          // correct amount of bytes... but it is up to us in updating the BSR
          // value, substracting the amount of bytes transmitted

          // We need to use std::min here because bytesPerLcId can be
          // greater than bsr.txQueueSize because scheduler can assign
          // more bytes than needed due to how TB size is computed.
          bsr.retxQueueSize -= std::min (bytesPerLcId, bsr.retxQueueSize);
        }
      else
        {
          NS_LOG_DEBUG ("Something wrong with the calculation of overhead."
                        "Active LCS Retx: " << activeLcsRetx << " assigned to this: " <<
                        bytesPerLcId << ", with TBS of " << m_ulDci->m_tbSize.at (0) <<
                        " usefulTbs " << usefulTbs << " and total used " << m_ulDciTotalUsed);
        }
    }
}

void
NrUeMac::SendTxData(uint32_t usefulTbs, uint32_t activeTx)
{
  NS_LOG_FUNCTION (this);

  if (activeTx == 0)
    {
      return;
    }

  uint32_t bytesPerLcId = usefulTbs / activeTx;

  for (auto & itBsr : m_ulBsrReceived)
    {
      auto &bsr = itBsr.second;

      if (m_ulDciTotalUsed + bytesPerLcId <= usefulTbs)
        {
          LteMacSapUser::TxOpportunityParameters txParams;
          txParams.lcid = bsr.lcid;
          txParams.rnti = m_rnti;
          txParams.bytes = bytesPerLcId;
          txParams.layer = 0;
          txParams.harqId = m_ulDci->m_harqProcess;
          txParams.componentCarrierId = GetBwpId ();

          NS_LOG_INFO ("Notifying RLC of LCID " << +bsr.lcid << " of a TxOpp "
                       "of " << bytesPerLcId << " B for a TX PDU");

          m_lcInfoMap.at (bsr.lcid).macSapUser->NotifyTxOpportunity (txParams);
          // After this call, m_ulDciTotalUsed has been updated with the
          // correct amount of bytes... but it is up to us in updating the BSR
          // value, substracting the amount of bytes transmitted

          // We need to use std::min here because bytesPerLcId can be
          // greater than bsr.txQueueSize because scheduler can assign
          // more bytes than needed due to how TB size is computed.
          bsr.txQueueSize -= std::min (bytesPerLcId, bsr.txQueueSize);
        }
      else
        {
          NS_LOG_DEBUG ("Something wrong with the calculation of overhead."
                        "Active LCS Retx: " << activeTx << " assigned to this: " <<
                        bytesPerLcId << ", with TBS of " << m_ulDci->m_tbSize.at (0) <<
                        " usefulTbs " << usefulTbs << " and total used " << m_ulDciTotalUsed);
        }
    }
}

void
NrUeMac::SendNewData ()
{
  NS_LOG_FUNCTION (this);
  // New transmission -> empty pkt buffer queue (for deleting eventual pkts not acked )
  Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
  m_miUlHarqProcessesPacket.at (m_ulDci->m_harqProcess).m_pktBurst = pb;
  m_miUlHarqProcessesPacket.at (m_ulDci->m_harqProcess).m_lcidList.clear ();
  NS_LOG_INFO ("Reset HARQP " << +m_ulDci->m_harqProcess);

  // Sending the status data has no boundary: let's try to send the ACK as
  // soon as possible, filling the TBS, if necessary.
  SendNewStatusData ();

  // Let's count how many LC we have, that are waiting with some data
  uint16_t activeLcsRetx = 0;
  uint16_t activeLcsTx = 0;
  for (const auto & itBsr : m_ulBsrReceived)
    {
      if (itBsr.second.retxQueueSize > 0)
        {
          activeLcsRetx++;
        }
      if (itBsr.second.txQueueSize > 0)
        {
          activeLcsTx++;
        }
    }

  // Of the TBS we received in the DCI, one part is gone for the status pdu,
  // where we didn't check much as it is the most important data, that has to go
  // out. For the rest that we have left, we can use only a part of it because of
  // the overhead of the SHORT_BSR, which is 5 bytes.
  NS_ASSERT_MSG (m_ulDciTotalUsed + 5 <= m_ulDci->m_tbSize.at (0),
                 "The StatusPDU used " << m_ulDciTotalUsed << " B, we don't have any for the SHORT_BSR.");
  uint32_t usefulTbs = m_ulDci->m_tbSize.at (0) - m_ulDciTotalUsed - 5;

  // Now, we have 3 bytes of overhead for each subPDU. Let's try to serve all
  // the queues with some RETX data.
  if (activeLcsRetx * 3 > usefulTbs)
    {
      NS_LOG_DEBUG ("The overhead for transmitting retx data is greater than the space for transmitting it."
                    "Ignore the TBS of " << usefulTbs << " B.");
    }
  else
    {
      usefulTbs -= activeLcsRetx * 3;
      SendRetxData (usefulTbs, activeLcsRetx);
    }

  // Now we have to update our useful TBS for the next transmission.
  // Remember that m_ulDciTotalUsed keep count of data and overhead that we
  // used till now.
  NS_ASSERT_MSG (m_ulDciTotalUsed + 5 <= m_ulDci->m_tbSize.at (0),
                 "The StatusPDU sending required all space, we don't have any for the SHORT_BSR.");
  usefulTbs = m_ulDci->m_tbSize.at (0) - m_ulDciTotalUsed - 5; // Update the usefulTbs.

  // The last part is for the queues with some non-RETX data. If there is no space left,
  // then nothing.
  if (activeLcsTx * 3 > usefulTbs)
    {
      NS_LOG_DEBUG ("The overhead for transmitting new data is greater than the space for transmitting it."
                    "Ignore the TBS of " << usefulTbs << " B.");
    }
  else
    {
      usefulTbs -= activeLcsTx * 3;
      SendTxData (usefulTbs, activeLcsTx);
    }

  // If we did not used the packet burst, explicitly signal it to the HARQ
  // retx, if any.
  if (m_ulDciTotalUsed == 0)
    {
      m_miUlHarqProcessesPacket.at (m_ulDci->m_harqProcess).m_pktBurst = nullptr;
      m_miUlHarqProcessesPacket.at (m_ulDci->m_harqProcess).m_lcidList.clear ();
    }
}


void
NrUeMac::SendNewStatusData()
{
  NS_LOG_FUNCTION (this);

  bool hasStatusPdu = false;
  bool sentOneStatusPdu = false;

  for (auto & bsrIt : m_ulBsrReceived)
    {
      auto & bsr = bsrIt.second;

      if (bsr.statusPduSize > 0)
        {
          hasStatusPdu = true;

          // Check if we have room to transmit the statusPdu
          if (m_ulDciTotalUsed + bsr.statusPduSize <= m_ulDci->m_tbSize.at (0))
            {
              LteMacSapUser::TxOpportunityParameters txParams;
              txParams.lcid = bsr.lcid;
              txParams.rnti = m_rnti;
              txParams.bytes = bsr.statusPduSize;
              txParams.layer = 0;
              txParams.harqId = m_ulDci->m_harqProcess;
              txParams.componentCarrierId = GetBwpId ();

              NS_LOG_INFO ("Notifying RLC of LCID " << +bsr.lcid << " of a TxOpp "
                           "of " << bsr.statusPduSize << " B for a status PDU");

              m_lcInfoMap.at (bsr.lcid).macSapUser->NotifyTxOpportunity (txParams);
              // After this call, m_ulDciTotalUsed has been updated with the
              // correct amount of bytes... but it is up to us in updating the BSR
              // value, subtracting the amount of bytes transmitted
              bsr.statusPduSize = 0;
              sentOneStatusPdu = true;
            }
          else
            {
              NS_LOG_INFO ("Cannot send StatusPdu of " << bsr.statusPduSize <<
                           " B, we already used all the TBS");
            }
        }
    }

  NS_ABORT_MSG_IF (hasStatusPdu && !sentOneStatusPdu,
                   "The TBS of size " << m_ulDci->m_tbSize.at (0) << " doesn't allow us "
                   "to send one status PDU...");
}

void
NrUeMac::DoReceiveControlMessage  (Ptr<NrControlMessage> msg)
{
  NS_LOG_FUNCTION (this << msg);

  switch (msg->GetMessageType ())
    {
    case (NrControlMessage::UL_DCI):
      {
        ProcessUlDci (DynamicCast<NrUlDciMessage> (msg));
        break;
      }
    case (NrControlMessage::RAR):
      {
        NS_LOG_INFO ("Received RAR in slot " << m_currentSlot);

        m_macRxedCtrlMsgsTrace (m_currentSlot, GetCellId (), m_rnti, GetBwpId (), msg);

        if (m_waitingForRaResponse == true)
          {
            Ptr<NrRarMessage> rarMsg = DynamicCast<NrRarMessage> (msg);
            NS_LOG_LOGIC ("got RAR with RA-RNTI " << +rarMsg->GetRaRnti () <<
                          ", expecting " << +m_raRnti);
            for (auto it = rarMsg->RarListBegin (); it != rarMsg->RarListEnd (); ++it)
              {
                if (it->rapId == m_raPreambleId)
                  {
                    RecvRaResponse (it->rarPayload);
                  }
              }
          }
        break;
      }

    default:
      NS_LOG_LOGIC ("Control message not supported/expected");
    }
}

NrUePhySapUser*
NrUeMac::GetPhySapUser ()
{
  return m_phySapUser;
}

void
NrUeMac::SetPhySapProvider (NrPhySapProvider* ptr)
{
  m_phySapProvider = ptr;
}

void
NrUeMac::DoConfigureRach ([[maybe_unused]] LteUeCmacSapProvider::RachConfig rc)
{
  NS_LOG_FUNCTION (this);
}

void
NrUeMac::DoStartContentionBasedRandomAccessProcedure ()
{
  NS_LOG_FUNCTION (this);
  RandomlySelectAndSendRaPreamble ();
}

void
NrUeMac::RandomlySelectAndSendRaPreamble ()
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG (m_currentSlot << " Received System Information, send to PHY the RA preamble");
  SendRaPreamble (true);
}

void
NrUeMac::SendRaPreamble ([[maybe_unused]] bool contention)
{
  NS_LOG_INFO (this);
  //m_raPreambleId = m_raPreambleUniformVariable->GetInteger (0, 64 - 1);
  m_raPreambleId = g_raPreambleId++;
  /*raRnti should be subframeNo -1 */
  m_raRnti = 1;

  Ptr<NrRachPreambleMessage> rachMsg = Create<NrRachPreambleMessage> ();
  rachMsg->SetSourceBwp (GetBwpId ());
  m_macTxedCtrlMsgsTrace (m_currentSlot, GetCellId (), m_rnti, GetBwpId (), rachMsg);

  m_phySapProvider->SendRachPreamble (m_raPreambleId, m_raRnti);
}

void
NrUeMac::DoStartNonContentionBasedRandomAccessProcedure (uint16_t rnti, [[maybe_unused]] uint8_t preambleId, uint8_t prachMask)
{
  NS_LOG_FUNCTION (this << " rnti" << rnti);
  NS_ASSERT_MSG (prachMask == 0, "requested PRACH MASK = " << (uint32_t) prachMask << ", but only PRACH MASK = 0 is supported");
  m_rnti = rnti;
}

void
NrUeMac::AddLc (uint8_t lcId,  LteUeCmacSapProvider::LogicalChannelConfig lcConfig, LteMacSapUser* msu)
{
  NS_LOG_FUNCTION (this << " lcId" << (uint32_t) lcId);
  NS_ASSERT_MSG (m_lcInfoMap.find (lcId) == m_lcInfoMap.end (), "cannot add channel because LCID " << lcId << " is already present");

  LcInfo lcInfo;
  lcInfo.lcConfig = lcConfig;
  lcInfo.macSapUser = msu;
  m_lcInfoMap[lcId] = lcInfo;
}

void
NrUeMac::DoRemoveLc (uint8_t lcId)
{
  NS_LOG_FUNCTION (this << " lcId" << lcId);
}

LteMacSapProvider*
NrUeMac::GetUeMacSapProvider (void)
{
  return m_macSapProvider;
}

void
NrUeMac::DoReset ()
{
  NS_LOG_FUNCTION (this);
}

int64_t
NrUeMac::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_raPreambleUniformVariable ->SetStream (stream);
  return 1;
}
//NR SL

std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>
NrUeMac::GetNrSlCandidateResourcesFromSlots (const SfnSf& sfn, uint8_t psfchPeriod, uint8_t minTimeGapPsfch, uint16_t lSubCh, uint16_t totalSubCh, std::list <NrSlCommResourcePool::SlotInfo> slotInfo) const
{
  NS_LOG_FUNCTION (this << sfn.Normalize () << lSubCh << totalSubCh << slotInfo.size ());

  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> nrResourceList;
  for (const auto& it:slotInfo)
    {
      for (uint16_t i = 0; i + lSubCh <= totalSubCh; i++)
        {
          NrSlUeMacSchedSapProvider::NrSlSlotInfo info (it.numSlPscchRbs, it.slPscchSymStart,
                                                        it.slPscchSymLength, it.slPsschSymStart,
                                                        it.slPsschSymLength, it.slHasPsfch, it.slSubchannelSize,
                                                        it.slMaxNumPerReserve,
                                                        psfchPeriod,
                                                        minTimeGapPsfch, m_minTimeGapProcessing,
                                                        sfn.GetFutureSfnSf (it.slotOffset),
                                                        i, lSubCh);
          nrResourceList.emplace_back (info);
        }
    }

  return nrResourceList;
}

bool
NrUeMac::OverlappedResource (uint8_t firstStart, uint8_t firstLength, uint8_t secondStart, uint8_t secondLength) const
{
  NS_ASSERT_MSG (firstLength && secondLength, "Length should not be zero");
  if (std::max (firstStart, secondStart) < std::min (firstStart + firstLength, secondStart + secondLength))
    {
      return true;
    }
  else
    {
      return false;
    }
}

void
NrUeMac::ExcludeResourcesBasedOnHistory (const SfnSf& sfn, const std::list<SfnSf>& transmitHistory,
  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>& candidateList,
  const std::list<uint16_t>& slResourceReservePeriodList) const
{
  NS_LOG_FUNCTION (this << sfn.Normalize () << transmitHistory.size () << candidateList.size ()
    << slResourceReservePeriodList.size ()); 

  std::set<uint64_t> sfnToExclude; // SFN slot numbers (normalized) to exclude
  uint64_t firstSfnNorm = candidateList.front ().sfn.Normalize (); // lowest candidate SFN slot number
  uint64_t lastSfnNorm = candidateList.back ().sfn.Normalize (); // highest candidate SFN slot number
  NS_LOG_DEBUG ("Excluding resources between normalized SFNs (" << firstSfnNorm << ":" << lastSfnNorm << ")");

  // Iterate the resource reserve period list and the transmit history to
  // find all slot numbers such that multiples of the reserve period, when
  // added to the history's slot number, are within the candidate resource
  // slots lowest and highest numbers
  for (auto listIt : slResourceReservePeriodList)
    {
      if (listIt == 0) continue;  // 0ms value is ignored
      listIt *= (1 << sfn.GetNumerology ());  // Convert from ms to slots
      for (auto historyIt : transmitHistory)
        {
          uint16_t i = 1;
          uint64_t sfnToCheck = (historyIt).Normalize () + (listIt);
          while (sfnToCheck <= lastSfnNorm)
            {
              if (sfnToCheck >= firstSfnNorm)
                {
                  sfnToExclude.insert (sfnToCheck);
                }
              i++;
              sfnToCheck = (historyIt).Normalize () + (i) * (listIt);
            }
        }
    }
  // sfnToExclude is a set of SFN normalized slot numbers for which we need
  // to exclude (erase) any candidate resources that match
  for (auto i : sfnToExclude)
    {
      auto itCand = candidateList.begin ();
      while (itCand != candidateList.end ())
        {
          if ((*itCand).sfn.Normalize () == i)
            {
              NS_LOG_DEBUG ("Erasing candidate resource at " << i);
              itCand = candidateList.erase (itCand);
            }
          else
            {
              itCand++;
            }
        }
    }
}

std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>
NrUeMac::FilterNrSlCandidateResources (std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> candidateReso)
{
  NS_LOG_FUNCTION (this);

   if (candidateReso.empty () || m_slGrants.empty())
     {
       return candidateReso;
     }

   NrSlSlotAlloc dummyAlloc;
   for (const auto & itDst : m_slGrants)
     {
       for (auto itGrantVector = itDst.second.begin () ; itGrantVector != itDst.second.end (); ++itGrantVector)
         {
           for (auto itGrantAlloc = itGrantVector->slotAllocations.begin (); itGrantAlloc != itGrantVector->slotAllocations.end (); itGrantAlloc ++)
             {
               auto itCandReso = candidateReso.begin ();
               while (itCandReso != candidateReso.end ())
                 {
                   //Currently the PHY doesn't handle multiple PSSCH transmissions in the same slot
                   //Thus, we need to remove all candidate resources belonging to the same slot than a granted resource
                   if (itGrantAlloc->sfn == itCandReso->sfn)
                     {
                       itCandReso = candidateReso.erase (itCandReso);
                     }
                   else
                     {
                       ++itCandReso;
                     }
                 }
             }
         }
     }

   return candidateReso;
}

std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>
NrUeMac::GetNrSlAvailableResources (const SfnSf& sfn, const NrSlTransmissionParams& params)
{
  NS_LOG_FUNCTION (this << sfn.GetFrame() << +sfn.GetSubframe() << sfn.GetSlot () << params);

  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> availableResources;
  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> candidateResources;

  candidateResources = GetNrSlCandidateResources (sfn, params);
  availableResources = FilterNrSlCandidateResources (candidateResources);

  return availableResources;
}

std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>
NrUeMac::GetNrSlCandidateResources (const SfnSf& sfn, const NrSlTransmissionParams& params)
{
  return GetNrSlCandidateResourcesPrivate (sfn, params, m_slTxPool, m_nrSlUePhySapProvider->GetSlotPeriod (),
    m_imsi, GetBwpId (), m_poolId, GetTotalSubCh (m_poolId), m_sensingData, m_transmitHistory);
}

std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>
NrUeMac::GetNrSlCandidateResourcesPrivate (const SfnSf& sfn, const NrSlTransmissionParams& params,
  Ptr<const NrSlCommResourcePool> txPool, Time slotPeriod, uint64_t imsi, uint8_t bwpId,
  uint16_t poolId, uint8_t totalSubCh, const std::list<SensingData>& sensingData,
  const std::list<SfnSf>& transmitHistory) const
{
  NS_LOG_FUNCTION (this << sfn.GetFrame() << +sfn.GetSubframe() << sfn.GetSlot () << params << txPool
   << slotPeriod << imsi << +bwpId << poolId << +totalSubCh);

  SensingTraceReport report;  // for tracing
  report.m_sfn = sfn;
  report.m_t0 = txPool->GetNrSlSensWindInSlots (bwpId, poolId, slotPeriod);
  report.m_tProc0 = m_tproc0;
  report.m_t1 = m_t1;
  report.m_t2 = m_t2;
  report.m_subchannels = totalSubCh;
  report.m_lSubch = params.m_lSubch;
  report.m_resourcePercentage = GetResourcePercentage ();

  NS_LOG_DEBUG ("Transmit  size: " << transmitHistory.size ()
    << "; sensing data size: " << sensingData.size ());

  // Input parameters (from params) are the priority, packet delay budget,
  // number of subchannels, the RRI, and the C_resel
  // - params.m_priority
  // - params.m_packetDelayBudget
  // - params.m_lSubch
  // - params.m_pRsvpTx
  // - params.m_cResel

  // TR 38.214 Section 8.1.4, return the set 'S_A' (candidate single slot
  // resources).  The size of this list is the algorithm parameter 'M_total'.

  // In this code, the list of candidateSlots is taken from the resource pool,
  // each SlotInfo doesn't have a list of subchannel (indices).
  // The NrUeMac copies each resource to the candidateResources list,
  // the only difference being that this NrSlUeMacSchedSapProvider::NrSlSlotInfo
  // exists at the mac/scheduler API.  Consolidating these two nearly
  // identical structures could be a topic of future work.

  // Probably what is needed is that the list of all candidate resources
  // should be passed from nr-sl-comm-resource-pool to begin with.

  std::list <NrSlCommResourcePool::SlotInfo> candidateSlots; // candidate single slots
  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> candidateResources;// S_A as per TS 38.214

  uint64_t absSlotIndex = sfn.Normalize ();
  uint16_t numerology = sfn.GetNumerology ();

  //Check the validity of the resource selection window configuration (T1 and T2) 
  //and the following parameters: numerology and reservation period.
  uint16_t nsMs = (m_t2-m_t1+1) * (1 / pow(2,numerology)); // number of slots mutiplied by the slot duration in ms
  uint16_t rsvpMs = static_cast <uint16_t> (params.m_pRsvpTx.GetMilliSeconds ()); // reservation period in ms
  NS_ABORT_MSG_IF (rsvpMs && nsMs > rsvpMs, 
      "An error may be generated due to the fact that the resource selection window size is higher than the resource reservation period value. Make sure that (T2-T1+1) x (1/(2^numerology)) < reservation period. Modify the values of T1, T2, numerology, and reservation period accordingly.");

  //step 4 as per TS 38.214 sec 8.1.4
  candidateSlots = txPool->GetNrSlCommOpportunities (absSlotIndex, bwpId, numerology, poolId, m_t1, m_t2);
  report.m_initialCandidateSlotsSize = candidateSlots.size ();
  uint8_t psfchPeriod = txPool->GetPsfchPeriod (bwpId, poolId);
  uint8_t minTimeGapPsfch = txPool->GetMinTimeGapPsfch (bwpId, poolId);
  if (candidateSlots.size () == 0)
    {
      //Since, all the parameters (i.e., T1, T2min, and T2) of the selection
      //window are in terms of physical slots, it may happen that there are no
      //slots available for Sidelink, which depends on the TDD pattern and the
      //Sidelink bitmap.
      return GetNrSlCandidateResourcesFromSlots (sfn, psfchPeriod, minTimeGapPsfch, params.m_lSubch, totalSubCh, candidateSlots);
    }

  candidateResources = GetNrSlCandidateResourcesFromSlots (sfn, psfchPeriod, minTimeGapPsfch, params.m_lSubch, totalSubCh, candidateSlots);
  uint32_t mTotal = candidateResources.size (); // total number of candidate single-slot resources
  report.m_initialCandidateResourcesSize = mTotal;
  if (!m_enableSensing)
    {
      NS_LOG_DEBUG ("No sensing: Total slots selected " << mTotal);
      return candidateResources;
    }

  // This is an optimization to skip further null processing below
  if (m_enableSensing && !sensingData.size () && !transmitHistory.size ())
    {
      NS_LOG_DEBUG ("No sensing or  data found: Total slots selected " << mTotal);
      return candidateResources;
    }

  //Copy the buffer so we can trim the buffer as per Tproc0.
  //Note, we do not need to delete the latest measurement
  //from the original buffer because it will be deleted
  //by RemoveOldSensingData method once it is outdated.

  auto updatedSensingData = sensingData;

  //latest sensing data is at the end of the list
  //now remove the latest sensing data as per the value of Tproc0. This would
  //keep the size of the buffer equal to [n – T0 , n – Tproc0)

  auto rvIt = updatedSensingData.crbegin ();
  if (rvIt != updatedSensingData.crend ())
    {
      while (sfn.Normalize () - rvIt->sfn.Normalize () <= GetTproc0 ())
        {
          NS_LOG_DEBUG ("IMSI " << m_imsi << " ignoring sensed SCI at sfn " << sfn << " received at " << rvIt->sfn);
          updatedSensingData.pop_back ();
          rvIt = updatedSensingData.crbegin ();
        }
    }

  // Perform a similar operation on the transmit .
  // latest  is at the end of the list
  // keep the size of the buffer equal to [n – T0 , n – Tproc0)
  auto updatedHistory = transmitHistory;

  auto rvIt2 = updatedHistory.crbegin ();
  if (rvIt2 != updatedHistory.crend ())
    {
      while (sfn.Normalize () - rvIt2->Normalize () <= GetTproc0 ())
        {
          NS_LOG_DEBUG ("IMSI " << m_imsi << " ignoring  at sfn " << sfn << " received at " << *rvIt2);
          updatedHistory.pop_back ();
          rvIt2 = updatedHistory.crbegin ();
        }
    }

  // step 5: filter candidateResources based on transmit history, if threshold
  // defined in step 5a) is met
  auto candidatesToCheck = candidateResources;
  ExcludeResourcesBasedOnHistory (sfn, updatedHistory, candidatesToCheck, txPool->GetSlResourceReservePeriodList (bwpId, poolId));
  if (candidatesToCheck.size () >= (GetResourcePercentage () / 100.0) * mTotal)
    {
      NS_LOG_DEBUG ("Step 5 filter results: original: " << candidateResources.size ()
        << " updated: " << candidatesToCheck.size () << " X: " << GetResourcePercentage ()/ 100.0);
      candidateResources = candidatesToCheck;
    }
    else
    {
      candidatesToCheck = candidateResources;
    }
  report.m_candidateResourcesSizeAfterStep5 = candidatesToCheck.size ();

  //step 6

  // calculate all possible transmissions based on sensed SCIs,
  // with past transmissions projected into the selection window.
  // Using a vector of SlotSensingData, since we need to check all the SCIs
  // and their possible future transmission that are received during the
  // above trimmed sensing window. Each element of the vector holds a
  // list that holds the info of each received SCI and its possible
  // future transmissions.
  std::vector<std::list<SlotSensingData>> sensingDataProjections;
  for (const auto &itSensedSlot:updatedSensingData)
    {
      uint16_t resvPeriodSlots = txPool->GetResvPeriodInSlots (bwpId, poolId, MilliSeconds (itSensedSlot.rsvp), slotPeriod);
      std::list<SlotSensingData> listFutureSensTx = GetFutSlotsBasedOnSens (itSensedSlot, slotPeriod, resvPeriodSlots);
      sensingDataProjections.push_back (listFutureSensTx);
    }

  NS_LOG_DEBUG ("Size of sensingDataProjections outer vector: " << sensingDataProjections.size ());

  int rsrpThreshold = m_thresRsrp;
  report.m_initialRsrpThreshold = m_thresRsrp;
  do
    {
      //following assignment is needed since we might have to perform
      //multiple do-while over the same list by increasing the rsrpThreshold
      candidateResources = candidatesToCheck;
      NS_LOG_DEBUG ("Step 6 loop iteration checking " << candidateResources.size () << " resources against threshold " << rsrpThreshold);
      auto itCandidate = candidateResources.begin ();
      // itCandidate is the candidate single-slot resource R_x,y
      while (itCandidate != candidateResources.end ())
        {
          bool erased = false;
          // calculate all proposed transmissions of current candidate resource within selection window
          std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> listFutureCands;
          uint16_t pPrimeRsvpTx = txPool->GetResvPeriodInSlots (bwpId,
                                                                    poolId,
                                                                    params.m_pRsvpTx,
                                                                    slotPeriod);
          for (uint16_t i = 0; i < params.m_cResel; i++)
            {
              auto slAlloc = *itCandidate;
              slAlloc.sfn.Add (i * pPrimeRsvpTx);
              listFutureCands.emplace_back (slAlloc);
            }
          // Traverse over all the possible transmissions of each sensed SCI
          for (const auto &itSensingDataProjections : sensingDataProjections)
            {
              // for all proposed transmissions of current candidate resource
              for (auto &itFutureCand : listFutureCands)
                {
                  // Traverse the list of future projected transmissions for the given sensed SCI
                  for (const auto &itSlotSensingDataProjection : itSensingDataProjections)
                    {
                      if (itFutureCand.sfn.Normalize () == itSlotSensingDataProjection.sfn.Normalize ())
                        {
                          if (itSlotSensingDataProjection.slRsrp > rsrpThreshold)
                            {
                              if (OverlappedResource (itSlotSensingDataProjection.sbChStart, itSlotSensingDataProjection.sbChLength, itCandidate->slSubchannelStart, itCandidate->slSubchannelLength))
                                {
                                  NS_LOG_DEBUG ("Overlapped resource " << itCandidate->sfn.Normalize () << " occupied " << +itSlotSensingDataProjection.sbChLength << " subchannels index " << +itSlotSensingDataProjection.sbChStart);
                                  itCandidate = candidateResources.erase (itCandidate);
                                  NS_LOG_DEBUG ("Resource " << itCandidate->sfn.Normalize () << ":[" << itCandidate->slSubchannelStart << "," << (itCandidate->slSubchannelStart + itCandidate->slSubchannelLength - 1) << "] erased. Its rsrp : " << itSlotSensingDataProjection.slRsrp << " Threshold : " << rsrpThreshold);
                                  erased = true;  // Used to break out of outer for loop of sensed data projections 
                                  break;  // Stop further evaluation because candidate is erased
                                }
                            }
                        }
                    }
                }
              if (erased)
                {
                  break; // break for itSensingDataProjections
                }
            }
          if (!erased)
            {
              // Only need to increment if not erased above; if erased, the erase()
              // action will point itCandidate to the next item
              itCandidate++;
            }
        }
      //step 7. If the following while will not break, start over do-while
      //loop with rsrpThreshold increased by 3dB
      rsrpThreshold += 3;
      if (rsrpThreshold > 0)
        {
          //0 dBm is the maximum RSRP threshold level so if we reach
          //it, that means all the available slots are overlapping
          //in time and frequency with the sensed slots, and the
          //RSRP of the sensed slots is very high.
          NS_LOG_DEBUG ("Reached maximum RSRP threshold, unable to select resources");
          candidateResources.erase (candidateResources.begin (), candidateResources.end ());
          break; //break do while
        }
    }
  while (candidateResources.size () < (GetResourcePercentage () / 100.0) * mTotal);

  NS_LOG_DEBUG (candidateResources.size () << " slots selected after sensing resource selection from " << mTotal << " slots");

  report.m_finalRsrpThreshold = (rsrpThreshold - 3); // undo the last increment
  m_tracedSensingAlgorithm (report, candidateResources, updatedSensingData, updatedHistory);
  return candidateResources;
}

uint16_t
NrUeMac::GetResvPeriodInSlots (Time resvPeriod) const
{
  // The following ValidateResvPeriod() method was moved here from the
  // SetReservationPeriod() method.  The code no longer configures the RRI
  // for the MAC as a whole, but sets it on a per-LC basis.  This checks that
  // only the standard compliant values, including their intermediate values
  // could be set. TS38.321 sec 5.22.1.1 instructs to select one of the
  // allowed values configured by RRC in sl-ResourceReservePeriodList and
  // set the resource reservation interval with the selected value.
  // Also, this method checks that the reservation period is a multiple of
  // the length of the physical sidelink pool (i.e., the resultant bitmap
  // after applying SL bitmap over the TDD pattern).
  m_slTxPool->ValidateResvPeriod (GetBwpId (), m_poolId, resvPeriod, m_nrSlUePhySapProvider->GetSlotPeriod ());
  return m_slTxPool->GetResvPeriodInSlots (GetBwpId (), m_poolId, resvPeriod, m_nrSlUePhySapProvider->GetSlotPeriod ());
}

uint16_t
NrUeMac::GetNrSlSubChSize () const
{
  return m_slTxPool->GetNrSlSubChSize (GetBwpId (), m_poolId);
}

uint16_t
NrUeMac::GetNrSlPsschSymbolsPerSlot () const
{
  return m_slTxPool->GetPsschSymbolsPerSlot (GetBwpId (), m_poolId);
}

uint8_t
NrUeMac::GetPsfchPeriod () const
{
  return m_slTxPool->GetPsfchPeriod (GetBwpId (), m_poolId);
}

// Calculates parameters including Q for step 6(c) of sensing algorithm
std::list<SlotSensingData>
NrUeMac::GetFutSlotsBasedOnSens (SensingData sensedData, Time slotPeriod, uint16_t resvPeriodSlots) const
{
  NS_LOG_FUNCTION (this << sensedData.sfn.Normalize () << slotPeriod << resvPeriodSlots);
  std::list<SlotSensingData> listFutureSensTx;

  double slotLenMiSec = slotPeriod.GetSeconds () * 1000.0;
  NS_ABORT_MSG_IF (slotLenMiSec > 1, "Slot length can not exceed 1 ms");
  uint16_t selecWindLen = (m_t2 - m_t1) + 1; //selection window length in physical slots
  double tScalMilSec = selecWindLen * slotLenMiSec;
  double pRsvpRxMilSec = static_cast<double> (sensedData.rsvp);
  uint16_t q = 0;
  if (sensedData.rsvp != 0)
    {
      //I am aware that two double variable are compared. I don't expect these two
      //numbers to be big floating-point numbers.
      if (pRsvpRxMilSec < tScalMilSec)
        {
          q = static_cast <uint16_t> (std::ceil (tScalMilSec / pRsvpRxMilSec));
        }
      else
        {
          q = 1;
        }
      NS_LOG_DEBUG ("tScalMilSec: " << tScalMilSec << " pRsvpRxMilSec: " << pRsvpRxMilSec);

    }
  uint16_t pPrimeRsvpRx = resvPeriodSlots;

  for (uint16_t i = 0; i <= q; i++)
    {
      SlotSensingData sensedSlotData (sensedData.sfn, sensedData.rsvp,
                                      sensedData.sbChLength, sensedData.sbChStart,
                                      sensedData.prio, sensedData.slRsrp);
      sensedSlotData.sfn.Add (i * pPrimeRsvpRx);
      listFutureSensTx.emplace_back (sensedSlotData);

      if (sensedData.gapReTx1 != std::numeric_limits <uint8_t>::max ())
        {
          auto reTx1Slot = sensedSlotData;
          reTx1Slot.sfn = sensedSlotData.sfn.GetFutureSfnSf (sensedData.gapReTx1);
          reTx1Slot.sbChLength = sensedData.sbChLength;
          reTx1Slot.sbChStart = sensedData.sbChStartReTx1;
          listFutureSensTx.emplace_back (reTx1Slot);
        }
      if (sensedData.gapReTx2 != std::numeric_limits <uint8_t>::max ())
        {
          auto reTx2Slot = sensedSlotData;
          reTx2Slot.sfn = sensedSlotData.sfn.GetFutureSfnSf (sensedData.gapReTx2);
          reTx2Slot.sbChLength = sensedData.sbChLength;
          reTx2Slot.sbChStart = sensedData.sbChStartReTx2;
          listFutureSensTx.emplace_back (reTx2Slot);
        }
    }
  NS_LOG_DEBUG ("q: " << q << " Size of listFutureSensTx: " << listFutureSensTx.size ());

  return listFutureSensTx;
}

void
NrUeMac::DoReceiveSensingData (SensingData sensingData)
{
  NS_LOG_FUNCTION (this);

  if (m_enableSensing)
    {
      //oldest data will be at the front of the queue
      m_sensingData.push_back (sensingData);
    }
}


void
NrUeMac::DoReceivePsfch (uint32_t sendingNodeId, SlHarqInfo harqInfo)
{
  NS_LOG_FUNCTION (this << sendingNodeId << harqInfo.m_rnti);
  if (harqInfo.m_txRnti == m_rnti && harqInfo.m_bwpIndex == GetBwpId ())
    {
      // This HARQ is for us.  If this is a HARQ ACK, check whether to cancel
      // a pending grant for retransmitting the associated TB.
      if (harqInfo.IsReceivedOk ())
        {
          // Look for the std::deque of NrSlGrant objects corresponding to
          // this dstL2Id
          auto itNrSlGrantMap = m_slGrants.find (harqInfo.m_dstL2Id);
          if (itNrSlGrantMap != m_slGrants.end ())
            {
              // Iterate the std::deque to find the NrSlGrant with a 
              // matching HARQ process ID
              for (auto itNrSlGrant = itNrSlGrantMap->second.begin (); itNrSlGrant != itNrSlGrantMap->second.end (); )
                {
                  if (itNrSlGrant->nrSlHarqId == harqInfo.m_harqProcessId)
                    {
                      NS_LOG_DEBUG ("HARQ ACK: erasing grant to " << harqInfo.m_dstL2Id << " with HARQ process ID " << +harqInfo.m_harqProcessId);
                      itNrSlGrant = itNrSlGrantMap->second.erase (itNrSlGrant);
                      break;
                    }
                  else
                    {
                      ++itNrSlGrant;
                    }
                }
            }
        }
      m_nrSlHarq->RecvNrSlHarqFeedback (harqInfo);
    }
}

void
NrUeMac::RemoveOldSensingData (const SfnSf& sfn, uint16_t sensingWindow, std::list<SensingData>& sensingData, [[maybe_unused]] uint64_t imsi) 
{
  NS_LOG_FUNCTION (this << sfn << sensingWindow << sensingData.size () << imsi);
  //oldest sensing data is on the top of the list
  auto it = sensingData.cbegin();
  while (it != sensingData.cend ())
    {
      if (it->sfn.Normalize () < sfn.Normalize () - sensingWindow)
        {
          NS_LOG_DEBUG ("IMSI " << imsi << " erasing SCI at sfn " << sfn << " received at " << it->sfn);
          it = sensingData.erase (it);
        }
      else
        {
          //once we reached the sensing data, which lies in the
          //sensing window, we break. If the last entry lies in the sensing
          //window rest of the entries as well.
          break;
        }
      ++it;
    }
}

void
NrUeMac::RemoveOldTransmitHistory (const SfnSf& sfn, uint16_t sensingWindow, std::list<SfnSf>& history, [[maybe_unused]] uint64_t imsi) 
{
  NS_LOG_FUNCTION (this << sfn << sensingWindow << history.size () << imsi);

  auto it = history.cbegin();
  while (it != history.cend ())
    {
      if (it->Normalize () < sfn.Normalize () - sensingWindow)
        {
          NS_LOG_DEBUG ("IMSI " << imsi << " erasing SFN history at sfn " << sfn << " sent at " << *it);
          it = history.erase (it);
        }
      else
        {
          //break upon reaching the edge of the sensing window
          break;
        }
      ++it;
    }
}

void
NrUeMac::DoReceivePsschPhyPdu (Ptr<PacketBurst> pdu)
{
  NS_LOG_FUNCTION (this << "Received Sidelink PDU from PHY");

  LteRadioBearerTag tag;
  NrSlSciF2aHeader sciF2a;
  //Separate SCI stage 2 packet from data packets
  std::list <Ptr<Packet> > dataPkts;
  bool foundSci2 = false;
  for (auto packet : pdu->GetPackets ())
    {
      LteRadioBearerTag tag;
      if (packet->PeekPacketTag (tag) == false)
        {
          //SCI stage 2 is the only packet in the packet burst, which does
          //not have the tag
          packet->RemoveHeader (sciF2a);
          foundSci2 = true;
        }
      else
        {
          dataPkts.push_back (packet);
        }
    }

  NS_ABORT_MSG_IF (foundSci2 == false, "Did not find SCI stage 2 in PSSCH packet burst");
  NS_ASSERT_MSG (dataPkts.size () > 0, "Received PHY PDU with not data packets");

  auto it = m_sidelinkRxDestinations.find (sciF2a.GetDstId ());
  if (it == m_sidelinkRxDestinations.end ())
    {
      // It can be the case that the PHY allows a packet burst with an
      // unknown destination to be delivered, if the slot also contained a
      // packet burst with a valid destination.  If so, just drop this burst.
      NS_LOG_DEBUG ("Received PHY PDU with unknown destination " << sciF2a.GetDstId () << "; dropping");
      return;
    }

  for (auto &pktIt : dataPkts)
    {
      pktIt->RemovePacketTag (tag);
      //Even though all the packets in the packet burst are for the same
      //destination, they can belong to different Logical Channels (LC),
      //therefore, we have to build the identifier and find the LC of the
      //packet.
      SidelinkLcIdentifier identifier;
      identifier.lcId = tag.GetLcid ();
      identifier.srcL2Id = sciF2a.GetSrcId ();
      identifier.dstL2Id = sciF2a.GetDstId ();

      std::map <SidelinkLcIdentifier, SlLcInfoUeMac>::iterator it = m_nrSlLcInfoMap.find (identifier);
      if (it == m_nrSlLcInfoMap.end ())
        {
          //notify RRC to setup bearer.  We pass the castType and harqEnabled up to the RRC so that
          // additional bearer configuration is aligned
          m_nrSlUeCmacSapUser->NotifySidelinkReception (tag.GetLcid (), identifier.srcL2Id, identifier.dstL2Id, sciF2a.GetCastType (), sciF2a.GetHarqFeedbackIndicator ());

          //should be setup now
          it = m_nrSlLcInfoMap.find (identifier);
          if (it == m_nrSlLcInfoMap.end ())
            {
              NS_FATAL_ERROR ("Failure to setup Sidelink radio bearer for reception");
            }
        }
      NrSlMacSapUser::NrSlReceiveRlcPduParameters rxPduParams (pktIt, m_rnti, tag.GetLcid (),
                                                               identifier.srcL2Id, identifier.dstL2Id);

      FireTraceSlRlcRxPduWithTxRnti (pktIt->Copy (), tag.GetLcid ());
      it->second.macSapUser->ReceiveNrSlRlcPdu (rxPduParams);
    }
}


void
NrUeMac::DoNrSlSlotIndication (const SfnSf& sfn)
{
  NS_LOG_FUNCTION (this << " Frame " << sfn.GetFrame() << " Subframe " << +sfn.GetSubframe()
                        << " slot " << sfn.GetSlot () << " Normalized slot number " << sfn.Normalize ());

  bool atLeastOneTransmissionInSlot = false;
  // If a grant is scheduled for this slot, code below will provide the
  // TTI indication to the PHY (for either PSCCH or PSSCH).  However,
  // PSFCH may be scheduled in slots independent of the grants.  Provide
  // those TTI indications to the PHY here so that it can determine whether
  // to check its buffer for HARQ messages to send in this slot.
  if (m_slTxPool->SlotHasPsfch (sfn.Normalize (), GetBwpId (), m_poolId))
    {
      NS_LOG_DEBUG ("Slot " << sfn.Normalize () << " has PSFCH");
      NrSlVarTtiAllocInfo feedbackVarTtiInfo;
      feedbackVarTtiInfo.SlVarTtiType = NrSlVarTtiAllocInfo::FEEDBACK;
      feedbackVarTtiInfo.symStart = 12; // PSFCH is always in slot 12
      feedbackVarTtiInfo.symLength = 1;
      // Current NR sidelink code assumes that all of the RBs in a BWP
      // are used; so the rbStart will always be zero and the rbLength
      // will be the number of RBs in the BWP
      feedbackVarTtiInfo.rbStart = 0;
      feedbackVarTtiInfo.rbLength = GetTotalSubCh (m_poolId) * m_slTxPool->GetNrSlSubChSize (GetBwpId (), m_poolId);
      m_nrSlUePhySapProvider->SetNrSlVarTtiAllocInfo (sfn, feedbackVarTtiInfo);
      NS_LOG_INFO ("PSFCH at : Frame = " << sfn.GetFrame ()
                   << " SF = " << +sfn.GetSubframe ()
                   << " slot = " << +sfn.GetSlot ());
    }

  //check if we need to transmit PSCCH + PSSCH
  //We are starting with the transmission of data packets because if the buffer
  //at the RLC would be empty we just erase the grant of the current slot
  //without transmitting SCI 1 and SCI 2 message, and data. Therefore,
  //even we had the grant we will not put anything in the queues at the PHY.
  for (auto &itGrantMap : m_slGrants)
    {
      if (itGrantMap.second.empty ())
        {
          continue;
        }
      for (auto itGrant =  itGrantMap.second.begin () ; itGrant != itGrantMap.second.end ();)
        {
          bool removeGrant = false;
          NrSlUeMacSchedSapUser::NrSlGrant currentGrant = *itGrant;
          // Rename use of *itGrant (and currentGrant) below with "nrSlGrant"
          NS_ASSERT_MSG (itGrant->slotAllocations.size() > 0, "Empty grant in m_slGrants when iterated in NrUeMac::DoNrSlSlotIndication, rnti: " << m_rnti << " harqId: " << +itGrant->nrSlHarqId);
          auto currentSlotIt = itGrant->slotAllocations.begin ();
          NrSlSlotAlloc currentSlot = *currentSlotIt;
          // Rename use of currentSlot below with "nrSlSlotAlloc"
          if (currentGrant.slotAllocations.begin ()->sfn == sfn)
            {
              NS_LOG_INFO ("Grant at : Frame = " << currentSlot.sfn.GetFrame ()
                           << " SF = " << +currentSlot.sfn.GetSubframe ()
                           << " slot = " << +currentSlot.sfn.GetSlot ());
              if (currentSlot.ndi)
                {
                  Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
                  pb = m_nrSlHarq->GetPacketBurst (currentSlot.dstL2Id, currentGrant.nrSlHarqId);
                  if (pb->GetNPackets () > 0)
                    {
                      m_nrSlMacPduTxed = true;

                      NS_ASSERT_MSG (pb->GetNPackets () > 0, "Packet burst for HARQ id " << +currentGrant.nrSlHarqId << " is empty");
                      for (const auto & itPkt : pb->GetPackets ())
                        {
                          NS_LOG_DEBUG ("Sending PSSCH MAC PDU (1st Tx) dstL2Id: " << currentSlot.dstL2Id
                                        << " harqId: " << +currentGrant.nrSlHarqId
                                        << " Packet Size: " << itPkt->GetSize ());
                          m_nrSlUePhySapProvider->SendPsschMacPdu (itPkt, currentSlot.dstL2Id);
                        }
                    }
                  else
                    {
                      // A grant with NDI has been published but there is
                      // no data in the HARQ buffer.  This can occur if the
                      // application supported by SPS has stopped.
                      m_nrSlMacPduTxed = false;
                      NS_LOG_DEBUG ("Wasted grant opportunity ");
                      // Free up HARQ ID
                      m_nrSlHarq->FlushNrSlHarqBuffer (itGrant->nrSlHarqId);
                      itGrant = itGrantMap.second.erase (itGrant);
                      continue;
                    }
                  itGrant->tbTxCounter++;
                }
              else
                {
                  //retx from MAC HARQ buffer
                  //we might want to match the LC ids in currentGrant.slRlcPduInfo and
                  //the LC ids whose packets are in the packet burst in the HARQ
                  //buffer. I am not doing it at the moment as it might slow down
                  //the simulation.
                  itGrant->tbTxCounter++;
                  Ptr<PacketBurst> pb = CreateObject <PacketBurst> ();
                  pb = m_nrSlHarq->GetPacketBurst (currentSlot.dstL2Id, currentGrant.nrSlHarqId);
                  if (pb && pb->GetNPackets () > 0)
                    {
                      m_nrSlMacPduTxed = true;
                      NS_ASSERT_MSG (pb->GetNPackets () > 0, "Packet burst for HARQ id " << +currentGrant.nrSlHarqId << " is empty");
                      for (const auto & itPkt : pb->GetPackets ())
                        {
                          NS_LOG_DEBUG ("Sending PSSCH MAC PDU (Rtx) dstL2Id: " << currentSlot.dstL2Id
                                        << " harqId: " << +currentGrant.nrSlHarqId
                                        << " Packet Size: " << itPkt->GetSize ());
                          m_nrSlUePhySapProvider->SendPsschMacPdu (itPkt, currentSlot.dstL2Id);
                        }
                    }
                  else
                    {
                      NS_LOG_DEBUG ("Wasted Retx opportunity");
                    }
                }
              // Remove current slot allocation from this grant
              if (currentGrant.tbTxCounter == currentGrant.nSelected)
                {
                  // Remove this grant from the queue before continuing to next grant
                  removeGrant = true;
                  NS_LOG_DEBUG ("No slot allocations remain for grant to " << currentSlot.dstL2Id);
                  // Blind retransmissions if HARQ enabled and PSFCH period == 0                  
                  if (itGrant->harqEnabled && (m_slTxPool->GetPsfchPeriod (GetBwpId (), m_poolId) == 0))
                    {
                      // Do not wait for feedback, just flush the buffer and
                      // make the HARQ process ID available again
                      m_nrSlHarq->FlushNrSlHarqBuffer (itGrant->nrSlHarqId);
                    }
                  if (!itGrant->harqEnabled)
                    {
                      // There will not be feedback, just flush the buffer and
                      // make the HARQ process ID available again
                      m_nrSlHarq->FlushNrSlHarqBuffer (itGrant->nrSlHarqId);
                    }
                }
              itGrant->slotAllocations.erase (currentSlotIt);
              if (!m_nrSlMacPduTxed)
                {
                  //NR SL MAC PDU was not txed. It can happen if RLC buffer was empty
                  NS_LOG_DEBUG ("Slot wasted at : Frame = " << currentSlot.sfn.GetFrame () << " SF = " << +currentSlot.sfn.GetSubframe () << " slot = " << currentSlot.sfn.GetSlot ());
                  if (!currentGrant.tbTxCounter)
                    {
                      // Restore the previously allocated HarqID
                      m_nrSlHarq->FlushNrSlHarqBuffer (itGrant->nrSlHarqId);
                    }
                  continue;
                }
              atLeastOneTransmissionInSlot = true;

              //prepare and send SCI format 2A message
              NrSlSciF2aHeader sciF2a;
              sciF2a.SetHarqId (currentGrant.nrSlHarqId);
              sciF2a.SetNdi (currentSlot.ndi);
              sciF2a.SetRv (currentSlot.rv);
              sciF2a.SetSrcId (m_srcL2Id);
              sciF2a.SetDstId (currentSlot.dstL2Id);
              //fields which are not used yet that is why we set them to 0
              sciF2a.SetCsiReq (0);
              SidelinkLcIdentifier slLcId;
              // If multiple TB and LC are handled by this grant, they should
              // all share the same cast type, so it should suffice to fetch the
              // cast type from the first LC ID associated with the grant
              NS_ASSERT_MSG (currentSlot.slRlcPduInfo.size () > 0, "No SlRlcPduInfo available");
              slLcId.lcId = currentSlot.slRlcPduInfo.front ().lcid;
              slLcId.srcL2Id = m_srcL2Id;
              slLcId.dstL2Id = currentSlot.dstL2Id;
              const auto & itLc = m_nrSlLcInfoMap.find (slLcId);
              NS_ASSERT_MSG (itLc != m_nrSlLcInfoMap.end (), "No LC with id " << +slLcId.lcId << " found for destination " << currentSlot.dstL2Id);
              sciF2a.SetCastType (static_cast<uint8_t> (itLc->second.lcInfo.castType));
              // Request HARQ feedback if HARQ enabled and PSFCH period > 0
              if (currentGrant.harqEnabled && (m_slTxPool->GetPsfchPeriod (GetBwpId (), m_poolId) > 0))
                {
                  sciF2a.SetHarqFbIndicator (1);
                }
              else
                {
                  sciF2a.SetHarqFbIndicator (0);
                }

              Ptr<Packet> pktSciF02 = Create<Packet> ();
              pktSciF02->AddHeader (sciF2a);
              //put SCI stage 2 in PSSCH queue
              m_nrSlUePhySapProvider->SendPsschMacPdu (pktSciF02, currentSlot.dstL2Id);

              //set the VarTti allocation info for PSSCH
              NrSlVarTtiAllocInfo dataVarTtiInfo;
              dataVarTtiInfo.SlVarTtiType = NrSlVarTtiAllocInfo::DATA;
              dataVarTtiInfo.symStart = currentSlot.slPsschSymStart;
              dataVarTtiInfo.symLength = currentSlot.slPsschSymLength;
              dataVarTtiInfo.rbStart = currentSlot.slPsschSubChStart * m_slTxPool->GetNrSlSubChSize (GetBwpId (), m_poolId);
              dataVarTtiInfo.rbLength = currentSlot.slPsschSubChLength * m_slTxPool->GetNrSlSubChSize (GetBwpId (), m_poolId);
              m_nrSlUePhySapProvider->SetNrSlVarTtiAllocInfo (sfn, dataVarTtiInfo);

              // Collect statistics for NR SL PSCCH UE MAC scheduling trace
              SlPsschUeMacStatParameters psschStatsParams;
              psschStatsParams.timeMs = Simulator::Now ().GetSeconds () * 1000.0;
              psschStatsParams.imsi = m_imsi;
              psschStatsParams.rnti = m_rnti;
              psschStatsParams.frameNum = currentSlot.sfn.GetFrame ();
              psschStatsParams.subframeNum = currentSlot.sfn.GetSubframe ();
              psschStatsParams.slotNum = currentSlot.sfn.GetSlot ();
              psschStatsParams.symStart = currentSlot.slPsschSymStart;
              psschStatsParams.symLength = currentSlot.slPsschSymLength;
              psschStatsParams.rbStart = currentSlot.slPsschSubChStart * m_slTxPool->GetNrSlSubChSize (GetBwpId (), m_poolId);
              psschStatsParams.subChannelSize = m_slTxPool->GetNrSlSubChSize (GetBwpId (), m_poolId);
              psschStatsParams.rbLength = currentSlot.slPsschSubChLength * m_slTxPool->GetNrSlSubChSize (GetBwpId (), m_poolId);
              psschStatsParams.harqId = currentGrant.nrSlHarqId;
              psschStatsParams.ndi = currentSlot.ndi;
              psschStatsParams.rv = currentSlot.rv;
              psschStatsParams.srcL2Id = m_srcL2Id;
              psschStatsParams.dstL2Id = currentSlot.dstL2Id;
              psschStatsParams.csiReq = sciF2a.GetCsiReq ();
              psschStatsParams.castType = sciF2a.GetCastType ();
#ifdef NOTYET
              psschStatsParams.resoReselCounter = itGrantInfo.second.slResoReselCounter;
              psschStatsParams.cReselCounter = itGrantInfo.second.cReselCounter;
#endif


              m_slPsschScheduling (psschStatsParams); //Trace

              if (currentSlot.txSci1A)
                {
                  //prepare and send SCI format 1A message
                  NrSlSciF1aHeader sciF1a;
                  sciF1a.SetPriority (currentSlot.priority);
                  sciF1a.SetMcs (currentSlot.mcs);
                  sciF1a.SetSciStage2Format (NrSlSciF1aHeader::SciFormat2A);
                  sciF1a.SetSlResourceReservePeriod (static_cast <uint16_t> (currentGrant.rri.GetMilliSeconds ()));
                  sciF1a.SetTotalSubChannels (GetTotalSubCh (m_poolId));
                  sciF1a.SetIndexStartSubChannel (currentSlot.slPsschSubChStart);
                  sciF1a.SetLengthSubChannel (currentSlot.slPsschSubChLength);
                  sciF1a.SetSlMaxNumPerReserve (currentSlot.maxNumPerReserve);
                  if (currentSlot.slotNumInd > 1)
                    {
                      //itGrant->slotAllocations.cbegin () points to
                      //the next slot allocation this slot has to indicate
                      std::vector<uint8_t> gaps = ComputeGaps (currentSlot.sfn,
                                                               itGrant->slotAllocations.cbegin (),
                                                               currentSlot.slotNumInd);
                      std::vector<uint8_t> sbChIndex = GetStartSbChOfReTx (itGrant->slotAllocations.cbegin (),
                                                                           currentSlot.slotNumInd);
                      sciF1a.SetGapReTx1 (gaps.at (0));
                      sciF1a.SetIndexStartSbChReTx1 (sbChIndex.at (0));
                      if (gaps.size () > 1)
                        {
                          sciF1a.SetGapReTx2 (gaps.at (1));
                          NS_ASSERT_MSG (gaps.at (0) < gaps.at (1), "Incorrect computation of ReTx slot gaps");
                          sciF1a.SetIndexStartSbChReTx2 (sbChIndex.at (1));
                        }
                    }

                  Ptr<Packet> pktSciF1a = Create<Packet> ();
                  pktSciF1a->AddHeader (sciF1a);
                  NrSlMacPduTag tag (m_rnti, currentSlot.sfn, currentSlot.slPsschSymStart, currentSlot.slPsschSymLength, currentGrant.tbSize, currentSlot.dstL2Id);
                  pktSciF1a->AddPacketTag (tag);

                  NS_LOG_DEBUG ("Sending PSCCH MAC PDU dstL2Id: " << currentSlot.dstL2Id
                                << " harqId: " << +currentGrant.nrSlHarqId);
                  m_nrSlUePhySapProvider->SendPscchMacPdu (pktSciF1a);

                  //set the VarTti allocation info for PSCCH
                  NrSlVarTtiAllocInfo ctrlVarTtiInfo;
                  ctrlVarTtiInfo.SlVarTtiType = NrSlVarTtiAllocInfo::CTRL;
                  ctrlVarTtiInfo.symStart = currentSlot.slPscchSymStart;
                  ctrlVarTtiInfo.symLength = currentSlot.slPscchSymLength;
                  ctrlVarTtiInfo.rbStart = currentSlot.slPsschSubChStart * m_slTxPool->GetNrSlSubChSize (GetBwpId (), m_poolId);
                  ctrlVarTtiInfo.rbLength = currentSlot.numSlPscchRbs;
                  m_nrSlUePhySapProvider->SetNrSlVarTtiAllocInfo (sfn, ctrlVarTtiInfo);

                  // Collect statistics for NR SL PSCCH UE MAC scheduling trace
                  SlPscchUeMacStatParameters pscchStatsParams;
                  pscchStatsParams.timeMs = Simulator::Now ().GetSeconds () * 1000.0;
                  pscchStatsParams.imsi = m_imsi;
                  pscchStatsParams.rnti = m_rnti;
                  pscchStatsParams.frameNum = currentSlot.sfn.GetFrame ();
                  pscchStatsParams.subframeNum = currentSlot.sfn.GetSubframe ();
                  pscchStatsParams.slotNum = currentSlot.sfn.GetSlot ();
                  pscchStatsParams.symStart = currentSlot.slPscchSymStart;
                  pscchStatsParams.symLength = currentSlot.slPscchSymLength;
                  pscchStatsParams.rbStart = currentSlot.slPsschSubChStart * m_slTxPool->GetNrSlSubChSize (GetBwpId (), m_poolId);
                  pscchStatsParams.rbLength = currentSlot.numSlPscchRbs;
                  pscchStatsParams.priority = currentSlot.priority;
                  pscchStatsParams.mcs = currentSlot.mcs;
                  pscchStatsParams.tbSize = currentGrant.tbSize;
                  pscchStatsParams.slResourceReservePeriod = static_cast <uint16_t> (currentGrant.rri.GetMilliSeconds ());
                  pscchStatsParams.totalSubChannels = GetTotalSubCh (m_poolId);
                  pscchStatsParams.slPsschSubChStart = currentSlot.slPsschSubChStart;
                  pscchStatsParams.slPsschSubChLength = currentSlot.slPsschSubChLength;
                  pscchStatsParams.slMaxNumPerReserve = currentSlot.maxNumPerReserve;
                  pscchStatsParams.gapReTx1 = sciF1a.GetGapReTx1 ();
                  pscchStatsParams.gapReTx2 = sciF1a.GetGapReTx2 ();
                  m_slPscchScheduling (pscchStatsParams); //Trace
                }
            }
          else
            {
              //When there are no resources it may happen that the re-selection
              //counter of already existing destination remains zero. In this case,
              //we just go the next destination, if any.
            }

          if (removeGrant)
            {
              // The grant may be removed either when all slot allocations
              // have been used, or the TB has been positively acknowledged
              itGrant = itGrantMap.second.erase (itGrant);
            }
          else
            {
              ++itGrant;
            }

          //make this false before processing the grant for next destination
          m_nrSlMacPduTxed = false;
        }
    }
  if (atLeastOneTransmissionInSlot)
    {
      NS_LOG_DEBUG ("IMSI " << m_imsi << " adding SFN history at sfn " << sfn);
      m_transmitHistory.push_back (sfn);
    }
}

std::vector<uint8_t>
NrUeMac::ComputeGaps (const SfnSf& sfn,
                      std::set <NrSlSlotAlloc>::const_iterator it, uint8_t slotNumInd)
{
  NS_LOG_FUNCTION (this);
  std::vector<uint8_t> gaps;
  //slotNumInd is the number including the first TX. Gaps are computed only for
  //the ReTxs
  for (uint8_t i = 0; i < slotNumInd - 1; i++)
    {
      std::advance (it, i);
      gaps.push_back (static_cast<uint8_t>(it->sfn.Normalize () - sfn.Normalize ()));
    }

  return gaps;
}

std::vector<uint8_t>
NrUeMac::GetStartSbChOfReTx (std::set <NrSlSlotAlloc>::const_iterator it, uint8_t slotNumInd)
{
  NS_LOG_FUNCTION (this);
  std::vector<uint8_t> startSbChIndex;
  //slotNumInd is the number including the first TX. Start sub-channel index or
  //indices are retrieved only for the ReTxs
  for (uint8_t i = 0; i < slotNumInd - 1; i++)
    {
      std::advance (it, i);
      startSbChIndex.push_back (it->slPsschSubChStart);
    }

  return startSbChIndex;
}

void
NrUeMac::DoSchedUeNrSlConfigInd (uint32_t dstL2Id, const NrSlUeMacSchedSapUser::NrSlGrant& grant)
{
  NS_LOG_FUNCTION (this << dstL2Id);

  NS_LOG_INFO("Received grant to dstL2Id " << dstL2Id << " on HARQ ID " << +grant.nrSlHarqId << " containing " << grant.slotAllocations.size () << " slots and RRI " << grant.rri.As (Time::MS));
  // This grant will lead to dequeue of a new transport block.  There should
  // be a HARQ process ID available (otherwise the later enqueue will fail)
  NS_ABORT_MSG_UNLESS (m_nrSlHarq->IsHarqIdAvailable (grant.nrSlHarqId), "Unable to assign granted HARQ ID");
  // To ensure that the scheduler's choice of HARQ ID is available on a recurring
  // basis, a timeout value is necessary to return the HARQ ID before the next
  // scheduled grant can arrive (otherwise, the above ABORT may trigger)
  if (grant.rri > Seconds (0))
    {
      // SPS grant-- use the RRI as the timeout value
      m_nrSlHarq->AssignNrSlHarqProcessId (grant.nrSlHarqId, dstL2Id, grant.rri);
    }
  else
    { 
      // Dynamic grant-- allow some time after the last scheduled Tx
      // XXX Need a less hacky way to compute this
      std::vector<uint64_t> slots;
      uint8_t numerology = 0;
      for (const auto &itSlotAlloc : grant.slotAllocations)
        {
          slots.push_back(itSlotAlloc.sfn.Normalize ());
          numerology = itSlotAlloc.sfn.GetNumerology ();
        }
      auto minValue = std::min_element(slots.begin(), slots.end());
      auto maxValue = std::max_element(slots.begin(), slots.end());
      // Assume that the first slot in the grant is T1 time from now.  Wait for
      // T1 + (max - min) slots, plus some small number (4) of additional slots,
      // as a timeout value on the HARQ ID  
      auto timeoutInSlots = (*maxValue - *minValue) + GetT1 () + 4;
      auto slotsPerMs = 1 << numerology;
      m_nrSlHarq->AssignNrSlHarqProcessId (grant.nrSlHarqId, dstL2Id, MilliSeconds ((timeoutInSlots / slotsPerMs) + 1));
    }
  auto it = m_slGrants.find (dstL2Id);
  if (it == m_slGrants.end ())
    {
      NS_LOG_LOGIC ("Adding new grant structure for " << dstL2Id);
      std::deque<NrSlUeMacSchedSapUser::NrSlGrant> q;
      q.push_back (grant);
      m_slGrants.emplace (dstL2Id, q);
    }
  else
    {
      NS_LOG_LOGIC ("Inserting new grant to " << dstL2Id);
      if (m_nrSlHarq->IsHarqIdAvailable (grant.nrSlHarqId))
        {
          // XXX need to convey the timeout value across this API
          // Consider to have the scheduler make this assignment directly instead
          // of asking NrUeMac to make the assignment
           m_nrSlHarq->AssignNrSlHarqProcessId (grant.nrSlHarqId, dstL2Id, MilliSeconds (40));
        }
      it->second.push_back  (grant);
    }
  // The grant has a set of NrSlSlotAlloc.  One of these slots will be for
  // new data and some for retransmissions.  For the new data slots, notify
  // the RLC layer of transmission opportunities.
  for (const auto &itSlotAlloc : grant.slotAllocations)
    {
      if (itSlotAlloc.ndi == 0)
        {
          continue;
        }
      for (const auto & itLcRlcPduInfo : itSlotAlloc.slRlcPduInfo)
        {
          SidelinkLcIdentifier slLcId;
          slLcId.lcId = itLcRlcPduInfo.lcid;
          slLcId.srcL2Id = m_srcL2Id;
          slLcId.dstL2Id = dstL2Id;
          const auto & itLc = m_nrSlLcInfoMap.find (slLcId);
          NS_ASSERT_MSG (itLc != m_nrSlLcInfoMap.end (), "No LC with id " << +itLcRlcPduInfo.lcid << " found for destination " << itSlotAlloc.dstL2Id);
          NS_LOG_INFO ("Notifying NR SL RLC of TX opportunity for LC id " << +itLcRlcPduInfo.lcid << " for TB size " << itLcRlcPduInfo.size);
          itLc->second.macSapUser->NotifyNrSlTxOpportunity (NrSlMacSapUser::NrSlTxOpportunityParameters (itLcRlcPduInfo.size, m_rnti, itLcRlcPduInfo.lcid,
             0, grant.nrSlHarqId, GetBwpId (),
             m_srcL2Id, dstL2Id));
        }
    }
}

NrSlMacSapProvider*
NrUeMac::GetNrSlMacSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_nrSlMacSapProvider;
}

void
NrUeMac::SetNrSlMacSapUser (NrSlMacSapUser* s)
{
  NS_LOG_FUNCTION (this);
  m_nrSlMacSapUser = s;
}

NrSlUeCmacSapProvider*
NrUeMac::GetNrSlUeCmacSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_nrSlUeCmacSapProvider;
}

 void
 NrUeMac::SetNrSlUeCmacSapUser (NrSlUeCmacSapUser* s)
 {
   NS_LOG_FUNCTION (this);
   m_nrSlUeCmacSapUser = s;
 }

 NrSlUePhySapUser*
 NrUeMac::GetNrSlUePhySapUser ()
 {
   NS_LOG_FUNCTION (this);
   return m_nrSlUePhySapUser;
 }

 void
 NrUeMac::SetNrSlUePhySapProvider (NrSlUePhySapProvider* s)
 {
   NS_LOG_FUNCTION (this);
   m_nrSlUePhySapProvider = s;
 }

 void
 NrUeMac::SetNrSlUeMacSchedSapProvider (NrSlUeMacSchedSapProvider* s)
 {
   NS_LOG_FUNCTION (this);
   m_nrSlUeMacSchedSapProvider = s;
 }

 NrSlUeMacSchedSapUser*
 NrUeMac::GetNrSlUeMacSchedSapUser ()
 {
   NS_LOG_FUNCTION (this);
   return m_nrSlUeMacSchedSapUser;
 }

 void
 NrUeMac::SetNrSlUeMacCschedSapProvider (NrSlUeMacCschedSapProvider* s)
 {
   NS_LOG_FUNCTION (this);
   m_nrSlUeMacCschedSapProvider = s;
 }

 NrSlUeMacCschedSapUser*
 NrUeMac::GetNrSlUeMacCschedSapUser ()
 {
   NS_LOG_FUNCTION (this);
   return m_nrSlUeMacCschedSapUser;
 }

void
NrUeMac::DoTransmitNrSlRlcPdu (const NrSlMacSapProvider::NrSlRlcPduParameters &params)
{
  NS_LOG_FUNCTION (this << +params.lcid << +params.harqProcessId);
  LteRadioBearerTag bearerTag (params.rnti, params.lcid, 0);
  params.pdu->AddPacketTag (bearerTag);
  NS_LOG_DEBUG ("Adding packet in HARQ buffer for HARQ ID " << +params.harqProcessId << " pkt size " << params.pdu->GetSize ());
  m_nrSlHarq->AddPacket (params.dstL2Id, params.lcid, params.harqProcessId, params.pdu);
  m_nrSlUeMacSchedSapProvider->NotifyNrSlRlcPduDequeue (params.dstL2Id, params.lcid, params.pdu->GetSize ());
}

void
NrUeMac::DoReportNrSlBufferStatus (const NrSlMacSapProvider::NrSlReportBufferStatusParameters &params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_INFO ("Reporting for Sidelink. Tx Queue size = " << params.txQueueSize);
  //Sidelink BSR
  std::map <SidelinkLcIdentifier, NrSlMacSapProvider::NrSlReportBufferStatusParameters>::iterator it;

  SidelinkLcIdentifier slLcId;
  slLcId.lcId = params.lcid;
  slLcId.srcL2Id = params.srcL2Id;
  slLcId.dstL2Id = params.dstL2Id;

  it = m_nrSlBsrReceived.find (slLcId);
  if (it != m_nrSlBsrReceived.end ())
    {
      // update entry
      (*it).second = params;
    }
  else
    {
      m_nrSlBsrReceived.insert (std::make_pair (slLcId, params));
    }

  auto report = NrSlUeMacSchedSapProvider::SchedUeNrSlReportBufferStatusParams (params.rnti, params.lcid,
                                                                                params.txQueueSize, params.txQueueHolDelay,
                                                                                params.retxQueueSize, params.retxQueueHolDelay,
                                                                                params.statusPduSize, params.srcL2Id, params.dstL2Id);

  m_nrSlUeMacSchedSapProvider->SchedUeNrSlRlcBufferReq (report);
}

void
NrUeMac::DoAddNrSlLc (const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo &slLcInfo, NrSlMacSapUser* msu)
{
  NS_LOG_FUNCTION (this << +slLcInfo.lcId << slLcInfo.srcL2Id << slLcInfo.dstL2Id);

  // The following ValidateResvPeriod() method was moved here from the old
  // SetReservationPeriod() method.  The code no longer configures the RRI
  // for the MAC as a whole, but sets it on a per-LC basis.  This checks that
  // only the standard compliant values, including their intermediate values
  // could be set. TS38.321 sec 5.22.1.1 instructs to select one of the
  // allowed values configured by RRC in sl-ResourceReservePeriodList and
  // set the resource reservation interval with the selected value.
  // Also, this method checks that the reservation period is a multiple of
  // the length of the physical sidelink pool (i.e., the resultant bitmap
  // after applying SL bitmap over the TDD pattern).
  m_slTxPool->ValidateResvPeriod (GetBwpId (), m_poolId, slLcInfo.rri, m_nrSlUePhySapProvider->GetSlotPeriod ());

  SidelinkLcIdentifier slLcIdentifier;
  slLcIdentifier.lcId = slLcInfo.lcId;
  slLcIdentifier.srcL2Id = slLcInfo.srcL2Id;
  slLcIdentifier.dstL2Id = slLcInfo.dstL2Id;

  NS_ASSERT_MSG (m_nrSlLcInfoMap.find (slLcIdentifier) == m_nrSlLcInfoMap.end (), "cannot add LCID " << +slLcInfo.lcId
                                                                    << ", srcL2Id " << slLcInfo.srcL2Id << ", dstL2Id " << slLcInfo.dstL2Id << " is already present");

  SlLcInfoUeMac slLcInfoUeMac;
  slLcInfoUeMac.lcInfo = slLcInfo;
  slLcInfoUeMac.macSapUser = msu;
  m_nrSlLcInfoMap.insert (std::make_pair (slLcIdentifier, slLcInfoUeMac));

  auto lcInfo = NrSlUeMacCschedSapProvider::SidelinkLogicalChannelInfo (slLcInfo.dstL2Id, slLcInfo.lcId,
                                                                        slLcInfo.lcGroup, slLcInfo.pqi,
                                                                        slLcInfo.priority, slLcInfo.isGbr,
                                                                        slLcInfo.mbr, slLcInfo.gbr, slLcInfo.castType,
                                                                        slLcInfo.harqEnabled, slLcInfo.pdb,
                                                                        slLcInfo.dynamic, slLcInfo.rri);

  //Following if is needed because this method is called for both
  //TX and RX LCs addition into m_nrSlLcInfoMap. In case of RX LC, the
  //destination is this UE MAC.
  if (slLcInfo.srcL2Id == m_srcL2Id)
    {
      NS_LOG_DEBUG ("UE MAC with src id " << m_srcL2Id << " giving info of LC to the scheduler");
      m_nrSlUeMacCschedSapProvider->CschedUeNrSlLcConfigReq (lcInfo);
      AddNrSlDstL2Id (slLcInfo.dstL2Id, slLcInfo.priority);
    }
}

void
NrUeMac::DoRemoveNrSlLc (uint8_t slLcId, uint32_t srcL2Id, uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this << +slLcId << srcL2Id << dstL2Id);
  NS_ASSERT_MSG (slLcId > 3, "Hey! I can delete only the LC for data radio bearers.");
  SidelinkLcIdentifier slLcIdentifier;
  slLcIdentifier.lcId = slLcId;
  slLcIdentifier.srcL2Id = srcL2Id;
  slLcIdentifier.dstL2Id = dstL2Id;
  NS_ASSERT_MSG (m_nrSlLcInfoMap.find (slLcIdentifier) != m_nrSlLcInfoMap.end (), "could not find Sidelink LCID " << slLcId);
  m_nrSlLcInfoMap.erase (slLcIdentifier);
}

void
NrUeMac::DoResetNrSlLcMap ()
{
  NS_LOG_FUNCTION (this);

  auto it = m_nrSlLcInfoMap.begin ();

  while (it != m_nrSlLcInfoMap.end ())
    {
      if (it->first.lcId > 3) //SL DRB LC starts from 4
        {
          m_nrSlLcInfoMap.erase (it);
        }
      else
        {
          it++;
        }
    }
}

void
NrUeMac::AddNrSlDstL2Id (uint32_t dstL2Id, uint8_t lcPriority)
{
  NS_LOG_FUNCTION (this << dstL2Id << lcPriority);
  bool foundDst = false;
  for (auto& it : m_sidelinkTxDestinations)
    {
      if (it.first == dstL2Id)
        {
          foundDst = true;
          if (lcPriority < it.second)
            {
              it.second = lcPriority;
            }
          break;
        }
    }

  if (!foundDst)
    {
      m_sidelinkTxDestinations.push_back (std::make_pair (dstL2Id, lcPriority));
    }

  std::sort (m_sidelinkTxDestinations.begin (), m_sidelinkTxDestinations.end (), CompareSecond);
}

bool
NrUeMac::CompareSecond (std::pair<uint32_t, uint8_t>& a, std::pair<uint32_t, uint8_t>& b)
{
  return a.second < b.second;
}

void
NrUeMac::DoAddNrSlCommTxPool (Ptr<const NrSlCommResourcePool> txPool)
{
  NS_LOG_FUNCTION (this << txPool);
  m_slTxPool = txPool;
}

void
NrUeMac::DoAddNrSlCommRxPool (Ptr<const NrSlCommResourcePool> rxPool)
{
  NS_LOG_FUNCTION (this);
  m_slRxPool = rxPool;
}

void
NrUeMac::DoSetSlProbResoKeep (double prob)
{
  NS_LOG_FUNCTION (this << prob);
  NS_ASSERT_MSG (prob <= 1.0, "Probability value must be between 0 and 1");
  m_slProbResourceKeep = prob;
}

void
NrUeMac::DoSetSlMaxTxTransNumPssch (uint8_t maxTxPssch)
{
  NS_LOG_FUNCTION (this << +maxTxPssch);
  NS_ASSERT_MSG (maxTxPssch <= 32, "Number of PSSCH transmissions can not exceed 32");
  m_slMaxTxTransNumPssch = maxTxPssch;
}

void
NrUeMac::DoSetSourceL2Id (uint32_t srcL2Id)
{
  NS_LOG_FUNCTION (this << srcL2Id);
  m_srcL2Id = srcL2Id;
}

void
NrUeMac::DoAddNrSlRxDstL2Id (uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this << dstL2Id);
  m_sidelinkRxDestinations.insert (dstL2Id);
}

uint8_t
NrUeMac::DoGetSlActiveTxPoolId ()
{
  return GetSlActivePoolId ();
}

std::vector <std::pair<uint32_t, uint8_t> >
NrUeMac::DoGetSlTxDestinations ()
{
  return m_sidelinkTxDestinations;
}

std::unordered_set <uint32_t>
NrUeMac::DoGetSlRxDestinations ()
{
  return m_sidelinkRxDestinations;
}

uint8_t
NrUeMac::DoGetTotalSubCh () const
{
  NS_LOG_FUNCTION (this);
  return GetTotalSubCh (m_poolId);
}

uint8_t
NrUeMac::DoGetSlMaxTxTransNumPssch () const
{
  NS_LOG_FUNCTION (this);
  return m_slMaxTxTransNumPssch;
}

bool
NrUeMac::DoSlotHasPsfch (const SfnSf& sfn) const
{
  if (m_slTxPool->IsSidelinkSlot (GetBwpId (), GetSlActivePoolId (), sfn.Normalize ()))
    {
      return m_slTxPool->SlotHasPsfch (sfn.Normalize (), GetBwpId (), GetSlActivePoolId ());
    }
  else
    {
      return false;
    }
}

void
NrUeMac::DoCschedUeNrSlLcConfigCnf (uint8_t lcg, uint8_t lcId)
{
  NS_LOG_FUNCTION (this << +lcg << +lcId);
  NS_LOG_INFO ("SL UE scheduler successfully added LCG " << +lcg << " LC id " << +lcId);
}

void
NrUeMac::EnableSensing (bool enableSensing)
{
  NS_LOG_FUNCTION (this << enableSensing);
  NS_ASSERT_MSG (m_enableSensing == false, " Once the sensing is enabled, it can not be enabled or disabled again");
  m_enableSensing = enableSensing;
}

void
NrUeMac::SetTproc0 (uint8_t tproc0)
{
  NS_LOG_FUNCTION (this << +tproc0);
  m_tproc0 = tproc0;
}

uint8_t
NrUeMac::GetTproc0 () const
{
  return m_tproc0;
}

uint8_t
NrUeMac::GetT1 () const
{
  return m_t1;
}

void
NrUeMac::SetT1 (uint8_t t1)
{
  NS_LOG_FUNCTION (this << +t1);
  m_t1 = t1;
}

uint16_t
NrUeMac::GetT2 () const
{
  return m_t2;
}

void
NrUeMac::SetT2 (uint16_t t2)
{
  NS_LOG_FUNCTION (this << t2);
  m_t2 = t2;
}

uint16_t
NrUeMac::GetSlActivePoolId () const
{
  return m_poolId;
}

void
NrUeMac::SetSlActivePoolId (uint16_t poolId)
{
  m_poolId =  poolId;
}

uint8_t
NrUeMac::GetTotalSubCh (uint16_t poolId) const
{
  NS_LOG_FUNCTION (this << poolId);

  uint16_t subChSize = m_slTxPool->GetNrSlSubChSize (static_cast <uint8_t> (GetBwpId ()), poolId);

  uint8_t totalSubChanels = static_cast <uint8_t> (std::floor (m_nrSlUePhySapProvider->GetBwInRbs () / subChSize));

  return totalSubChanels;
}

void
NrUeMac::SetNumSidelinkProcess (uint8_t numSidelinkProcess)
{
  NS_LOG_FUNCTION (this << numSidelinkProcess);
  m_numSidelinkProcess = numSidelinkProcess;
  m_nrSlHarq->InitHarqBuffer (m_numSidelinkProcess);
}

uint8_t
NrUeMac::GetNumSidelinkProcess () const
{
  NS_LOG_FUNCTION (this);
  return m_numSidelinkProcess;
}

void
NrUeMac::SetSlThresPsschRsrp (int thresRsrp)
{
  NS_LOG_FUNCTION (this);
  m_thresRsrp = thresRsrp;
}

int
NrUeMac::GetSlThresPsschRsrp () const
{
  NS_LOG_FUNCTION (this);
  return m_thresRsrp;
}

void
NrUeMac::SetResourcePercentage (uint8_t percentage)
{
  NS_LOG_FUNCTION (this);
  m_resPercentage = percentage;
}

uint8_t
NrUeMac::GetResourcePercentage () const
{
  NS_LOG_FUNCTION (this);
  return m_resPercentage;
}

void
NrUeMac::FireTraceSlRlcRxPduWithTxRnti (const Ptr<Packet> p, uint8_t lcid)
{
  NS_LOG_FUNCTION (this);
  // Receiver timestamp
  RlcTag rlcTag;
  Time delay;

  bool ret = p->FindFirstMatchingByteTag (rlcTag);
  NS_ASSERT_MSG (ret, "RlcTag is missing for NR SL");

  delay = Simulator::Now () - rlcTag.GetSenderTimestamp ();
  m_rxRlcPduWithTxRnti (m_imsi, m_rnti, rlcTag.GetTxRnti (), lcid, p->GetSize (), delay.GetSeconds ());
}

//////////////////////////////////////////////

}
