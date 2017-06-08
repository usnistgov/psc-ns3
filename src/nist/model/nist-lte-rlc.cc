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
 * Author: Nicola Baldo <nbaldo@cttc.es>
 * Modified by: NIST
 */


#include "ns3/log.h"
#include "ns3/simulator.h"

#include "ns3/nist-lte-rlc.h"
#include "ns3/nist-lte-rlc-tag.h"
// #include "nist-lte-mac-sap.h"
#include "ns3/nist-lte-rlc-sap.h"
// #include "nist-ff-mac-sched-sap.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteRlc");


///////////////////////////////////////

class NistLteRlcSpecificLteMacSapUser : public NistLteMacSapUser
{
public:
  NistLteRlcSpecificLteMacSapUser (NistLteRlc* rlc);

  // Interface implemented from NistLteMacSapUser
  virtual void NotifyTxOpportunity (uint32_t bytes, uint8_t layer, uint8_t harqId);
  virtual void NotifyHarqDeliveryFailure ();
  virtual void ReceivePdu (Ptr<Packet> p);

private:
  NistLteRlcSpecificLteMacSapUser ();
  NistLteRlc* m_rlc;
};

NistLteRlcSpecificLteMacSapUser::NistLteRlcSpecificLteMacSapUser (NistLteRlc* rlc)
  : m_rlc (rlc)
{
}

NistLteRlcSpecificLteMacSapUser::NistLteRlcSpecificLteMacSapUser ()
{
}

void
NistLteRlcSpecificLteMacSapUser::NotifyTxOpportunity (uint32_t bytes, uint8_t layer, uint8_t harqId)
{
  m_rlc->DoNotifyTxOpportunity (bytes, layer, harqId);
}

void
NistLteRlcSpecificLteMacSapUser::NotifyHarqDeliveryFailure ()
{
  m_rlc->DoNotifyHarqDeliveryFailure ();
}

void
NistLteRlcSpecificLteMacSapUser::ReceivePdu (Ptr<Packet> p)
{
  m_rlc->DoReceivePdu (p);
}


///////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (NistLteRlc);

NistLteRlc::NistLteRlc ()
  : m_rlcSapUser (0),
    m_macSapProvider (0),
    m_rnti (0),
    m_lcid (0),
    m_srcL2Id (0),
    m_dstL2Id (0)
{
  NS_LOG_FUNCTION (this);
  m_rlcSapProvider = new NistLteRlcSpecificLteRlcSapProvider<NistLteRlc> (this);
  m_macSapUser = new NistLteRlcSpecificLteMacSapUser (this);
}

NistLteRlc::~NistLteRlc ()
{
  NS_LOG_FUNCTION (this);
}

TypeId NistLteRlc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLteRlc")
    .SetParent<Object> ()
    .AddTraceSource ("TxPDU",
                     "PDU transmission notified to the MAC.",
                     MakeTraceSourceAccessor (&NistLteRlc::m_txPdu),
                     "ns3::NistLteRlc::NotifyTxTracedCallback")
    .AddTraceSource ("RxPDU",
                     "PDU received.",
                     MakeTraceSourceAccessor (&NistLteRlc::m_rxPdu),
                     "ns3::NistLteRlc::ReceiveTracedCallback")
    ;
  return tid;
}

void
NistLteRlc::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete (m_rlcSapProvider);
  delete (m_macSapUser);
}

void
NistLteRlc::SetRnti (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << (uint32_t) rnti);
  m_rnti = rnti;
}

void
NistLteRlc::SetLcId (uint8_t lcId)
{
  NS_LOG_FUNCTION (this << (uint32_t) lcId);
  m_lcid = lcId;
}

void
NistLteRlc::SetSourceL2Id (uint32_t src)
{
  NS_LOG_FUNCTION (this << src);
  m_srcL2Id = src;
}
  
void
NistLteRlc::SetDestinationL2Id (uint32_t dst)
{
  NS_LOG_FUNCTION (this << dst);
  m_dstL2Id = dst;
}
  

void
NistLteRlc::SetNistLteRlcSapUser (NistLteRlcSapUser * s)
{
  NS_LOG_FUNCTION (this << s);
  m_rlcSapUser = s;
}

NistLteRlcSapProvider*
NistLteRlc::GetNistLteRlcSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_rlcSapProvider;
}

void
NistLteRlc::SetNistLteMacSapProvider (NistLteMacSapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_macSapProvider = s;
}

NistLteMacSapUser*
NistLteRlc::GetNistLteMacSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_macSapUser;
}



////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (NistLteRlcSm);

NistLteRlcSm::NistLteRlcSm ()
{
  NS_LOG_FUNCTION (this);
}

NistLteRlcSm::~NistLteRlcSm ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
NistLteRlcSm::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLteRlcSm")
    .SetParent<NistLteRlc> ()
    .AddConstructor<NistLteRlcSm> ()
    ;
  return tid;
}

void
NistLteRlcSm::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  ReportBufferNistStatus ();
}

void
NistLteRlcSm::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  NistLteRlc::DoDispose ();
}

void
NistLteRlcSm::DoTransmitPdcpPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
}

void
NistLteRlcSm::DoReceivePdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
  // RLC Performance evaluation
  NistRlcTag rlcTag;
  Time delay;
  if (p->FindFirstMatchingByteTag(rlcTag))
    {
      delay = Simulator::Now() - rlcTag.GetSenderTimestamp ();
    }
  NS_LOG_LOGIC (" RNTI=" << m_rnti 
                << " LCID=" << (uint32_t) m_lcid 
                << " size=" << p->GetSize () 
                << " delay=" << delay.GetNanoSeconds ());
  m_rxPdu(m_rnti, m_lcid, p->GetSize (), delay.GetNanoSeconds () );
}

void
NistLteRlcSm::DoNotifyTxOpportunity (uint32_t bytes, uint8_t layer, uint8_t harqId)
{
  NS_LOG_FUNCTION (this << bytes);
  NistLteMacSapProvider::NistTransmitPduParameters params;
  params.pdu = Create<Packet> (bytes);
  params.rnti = m_rnti;
  params.lcid = m_lcid;
  params.srcL2Id = m_srcL2Id;
  params.dstL2Id = m_dstL2Id;
  params.layer = layer;
  params.harqProcessId = harqId;

  // RLC Performance evaluation
  NistRlcTag tag (Simulator::Now());
  params.pdu->AddByteTag (tag);
  NS_LOG_LOGIC (" RNTI=" << m_rnti 
                << " LCID=" << (uint32_t) m_lcid 
                << " size=" << bytes);
  m_txPdu(m_rnti, m_lcid, bytes);

  m_macSapProvider->TransmitPdu (params);
  ReportBufferNistStatus ();
}

void
NistLteRlcSm::DoNotifyHarqDeliveryFailure ()
{
  NS_LOG_FUNCTION (this);
}

void
NistLteRlcSm::ReportBufferNistStatus ()
{
  NS_LOG_FUNCTION (this);
  NistLteMacSapProvider::NistReportBufferNistStatusParameters p;
  p.rnti = m_rnti;
  p.lcid = m_lcid;
  p.srcL2Id = m_srcL2Id;
  p.dstL2Id = m_dstL2Id;
  p.txQueueSize = 80000;
  p.txQueueHolDelay = 10;
  p.retxQueueSize = 0;
  p.retxQueueHolDelay = 0;
  p.statusPduSize = 0;
  m_macSapProvider->ReportBufferNistStatus (p);
}




//////////////////////////////////////////

// NistLteRlcTm::~NistLteRlcTm ()
// {
// }

//////////////////////////////////////////

// NistLteRlcUm::~NistLteRlcUm ()
// {
// }

//////////////////////////////////////////

// NistLteRlcAm::~NistLteRlcAm ()
// {
// }


} // namespace ns3
