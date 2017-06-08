/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011-2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Modified by: NIST
 */

#include "ns3/log.h"
#include "ns3/simulator.h"

#include "ns3/nist-lte-pdcp.h"
#include "ns3/nist-lte-pdcp-header.h"
#include "ns3/nist-lte-pdcp-sap.h"
#include "ns3/nist-lte-pdcp-tag.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLtePdcp");

class NistLtePdcpSpecificLteRlcSapUser : public NistLteRlcSapUser
{
public:
  NistLtePdcpSpecificLteRlcSapUser (NistLtePdcp* pdcp);

  // Interface provided to lower RLC entity (implemented from NistLteRlcSapUser)
  virtual void ReceivePdcpPdu (Ptr<Packet> p);

private:
  NistLtePdcpSpecificLteRlcSapUser ();
  NistLtePdcp* m_pdcp;
};

NistLtePdcpSpecificLteRlcSapUser::NistLtePdcpSpecificLteRlcSapUser (NistLtePdcp* pdcp)
  : m_pdcp (pdcp)
{
}

NistLtePdcpSpecificLteRlcSapUser::NistLtePdcpSpecificLteRlcSapUser ()
{
}

void
NistLtePdcpSpecificLteRlcSapUser::ReceivePdcpPdu (Ptr<Packet> p)
{
  m_pdcp->DoReceivePdu (p);
}

///////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (NistLtePdcp);

NistLtePdcp::NistLtePdcp ()
  : m_pdcpSapUser (0),
    m_rlcSapProvider (0),
    m_rnti (0),
    m_lcid (0),
    m_srcL2Id (0),
    m_dstL2Id (0),
    m_txSequenceNumber (0),
    m_rxSequenceNumber (0)
{
  NS_LOG_FUNCTION (this);
  m_pdcpSapProvider = new NistLtePdcpSpecificLtePdcpSapProvider<NistLtePdcp> (this);
  m_rlcSapUser = new NistLtePdcpSpecificLteRlcSapUser (this);
}

NistLtePdcp::~NistLtePdcp ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
NistLtePdcp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLtePdcp")
    .SetParent<Object> ()
    .AddTraceSource ("TxPDU",
                     "PDU transmission notified to the RLC.",
                     MakeTraceSourceAccessor (&NistLtePdcp::m_txPdu),
                     "ns3::NistLtePdcp::PduTxTracedCallback")
    .AddTraceSource ("RxPDU",
                     "PDU received.",
                     MakeTraceSourceAccessor (&NistLtePdcp::m_rxPdu),
                     "ns3::NistLtePdcp::PduRxTracedCallback")
    ;
  return tid;
}

void
NistLtePdcp::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete (m_pdcpSapProvider);
  delete (m_rlcSapUser);
}


void
NistLtePdcp::SetRnti (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << (uint32_t) rnti);
  m_rnti = rnti;
}

void
NistLtePdcp::SetLcId (uint8_t lcId)
{
  NS_LOG_FUNCTION (this << (uint32_t) lcId);
  m_lcid = lcId;
}

void
NistLtePdcp::SetSourceL2Id (uint32_t src)
{
  NS_LOG_FUNCTION (this << src);
  m_srcL2Id = src;
}

void
NistLtePdcp::SetDestinationL2Id (uint32_t dst)
{
  NS_LOG_FUNCTION (this << dst);
  m_dstL2Id = dst;
}

void
NistLtePdcp::SetNistLtePdcpSapUser (NistLtePdcpSapUser * s)
{
  NS_LOG_FUNCTION (this << s);
  m_pdcpSapUser = s;
}

NistLtePdcpSapProvider*
NistLtePdcp::GetNistLtePdcpSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_pdcpSapProvider;
}

void
NistLtePdcp::SetNistLteRlcSapProvider (NistLteRlcSapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_rlcSapProvider = s;
}

NistLteRlcSapUser*
NistLtePdcp::GetNistLteRlcSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_rlcSapUser;
}

NistLtePdcp::NistStatus 
NistLtePdcp::GetNistStatus ()
{
  NistStatus s;
  s.txSn = m_txSequenceNumber;
  s.rxSn = m_rxSequenceNumber;
  return s;
}

void 
NistLtePdcp::SetNistStatus (NistStatus s)
{
  m_txSequenceNumber = s.txSn;
  m_rxSequenceNumber = s.rxSn;
}

////////////////////////////////////////

void
NistLtePdcp::DoTransmitPdcpSdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << m_rnti << (uint32_t) m_lcid << p->GetSize ());

  NistLtePdcpHeader pdcpHeader;
  pdcpHeader.SetSequenceNumber (m_txSequenceNumber);

  m_txSequenceNumber++;
  if (m_txSequenceNumber > m_maxPdcpSn)
    {
      m_txSequenceNumber = 0;
    }

  pdcpHeader.SetDcBit (NistLtePdcpHeader::DATA_PDU);

  NS_LOG_LOGIC ("PDCP header: " << pdcpHeader);
  p->AddHeader (pdcpHeader);

  // Sender timestamp
  NistPdcpTag pdcpTag (Simulator::Now ());
  p->AddByteTag (pdcpTag);
  m_txPdu (m_rnti, m_lcid, p->GetSize ());

  NistLteRlcSapProvider::NistTransmitPdcpPduParameters params;
  params.rnti = m_rnti;
  params.lcid = m_lcid;
  params.srcL2Id = m_srcL2Id;
  params.dstL2Id = m_dstL2Id;
  params.pdcpPdu = p;

  m_rlcSapProvider->TransmitPdcpPdu (params);
}

void
NistLtePdcp::DoReceivePdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << m_rnti << (uint32_t) m_lcid << p->GetSize ());

  // Receiver timestamp
  NistPdcpTag pdcpTag;
  Time delay;
  if (p->FindFirstMatchingByteTag (pdcpTag))
    {
      delay = Simulator::Now() - pdcpTag.GetSenderTimestamp ();
    }
  m_rxPdu(m_rnti, m_lcid, p->GetSize (), delay.GetNanoSeconds ());

  NistLtePdcpHeader pdcpHeader;
  p->RemoveHeader (pdcpHeader);
  NS_LOG_LOGIC ("PDCP header: " << pdcpHeader);

  m_rxSequenceNumber = pdcpHeader.GetSequenceNumber () + 1;
  if (m_rxSequenceNumber > m_maxPdcpSn)
    {
      m_rxSequenceNumber = 0;
    }

  NistLtePdcpSapUser::NistReceivePdcpSduParameters params;
  params.pdcpSdu = p;
  params.rnti = m_rnti;
  params.lcid = m_lcid;
  params.srcL2Id = m_srcL2Id;
  params.dstL2Id = m_dstL2Id;
  m_pdcpSapUser->ReceivePdcpSdu (params);
}


} // namespace ns3
