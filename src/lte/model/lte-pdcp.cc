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
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#include "ns3/log.h"
#include "ns3/simulator.h"

#include "ns3/lte-pdcp.h"
#include "ns3/lte-pdcp-header.h"
#include "ns3/lte-sl-pdcp-header.h"
#include "ns3/lte-pdcp-sap.h"
#include "ns3/lte-pdcp-tag.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LtePdcp");

/// LtePdcpSpecificLteRlcSapUser class
class LtePdcpSpecificLteRlcSapUser : public LteRlcSapUser
{
public:
  /**
   * Constructor
   *
   * \param pdcp PDCP
   */
  LtePdcpSpecificLteRlcSapUser (LtePdcp* pdcp);

  // Interface provided to lower RLC entity (implemented from LteRlcSapUser)
  virtual void ReceivePdcpPdu (Ptr<Packet> p);

private:
  LtePdcpSpecificLteRlcSapUser ();
  LtePdcp* m_pdcp; ///< the PDCP
};

LtePdcpSpecificLteRlcSapUser::LtePdcpSpecificLteRlcSapUser (LtePdcp* pdcp)
  : m_pdcp (pdcp)
{
}

LtePdcpSpecificLteRlcSapUser::LtePdcpSpecificLteRlcSapUser ()
{
}

void
LtePdcpSpecificLteRlcSapUser::ReceivePdcpPdu (Ptr<Packet> p)
{
  m_pdcp->DoReceivePdu (p);
}

///////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (LtePdcp);

LtePdcp::LtePdcp ()
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
  m_pdcpSapProvider = new LtePdcpSpecificLtePdcpSapProvider<LtePdcp> (this);
  m_rlcSapUser = new LtePdcpSpecificLteRlcSapUser (this);
}

LtePdcp::~LtePdcp ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
LtePdcp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LtePdcp")
    .SetParent<Object> ()
    .SetGroupName("Lte")
    .AddTraceSource ("TxPDU",
                     "PDU transmission notified to the RLC.",
                     MakeTraceSourceAccessor (&LtePdcp::m_txPdu),
                     "ns3::LtePdcp::PduTxTracedCallback")
    .AddTraceSource ("RxPDU",
                     "PDU received.",
                     MakeTraceSourceAccessor (&LtePdcp::m_rxPdu),
                     "ns3::LtePdcp::PduRxTracedCallback")
    ;
  return tid;
}

void
LtePdcp::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete (m_pdcpSapProvider);
  delete (m_rlcSapUser);
}


void
LtePdcp::SetRnti (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << (uint32_t) rnti);
  m_rnti = rnti;
}

void
LtePdcp::SetLcId (uint8_t lcId)
{
  NS_LOG_FUNCTION (this << (uint32_t) lcId);
  m_lcid = lcId;
}

void
LtePdcp::SetSourceL2Id (uint32_t src)
{
  NS_LOG_FUNCTION (this << src);
  m_srcL2Id = src;
}

void
LtePdcp::SetDestinationL2Id (uint32_t dst)
{
  NS_LOG_FUNCTION (this << dst);
  m_dstL2Id = dst;
}

void
LtePdcp::SetLtePdcpSapUser (LtePdcpSapUser * s)
{
  NS_LOG_FUNCTION (this << s);
  m_pdcpSapUser = s;
}

LtePdcpSapProvider*
LtePdcp::GetLtePdcpSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_pdcpSapProvider;
}

void
LtePdcp::SetLteRlcSapProvider (LteRlcSapProvider * s)
{
  NS_LOG_FUNCTION (this << s);
  m_rlcSapProvider = s;
}

LteRlcSapUser*
LtePdcp::GetLteRlcSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_rlcSapUser;
}

LtePdcp::Status 
LtePdcp::GetStatus ()
{
  Status s;
  s.txSn = m_txSequenceNumber;
  s.rxSn = m_rxSequenceNumber;
  return s;
}

void 
LtePdcp::SetStatus (Status s)
{
  m_txSequenceNumber = s.txSn;
  m_rxSequenceNumber = s.rxSn;
}

////////////////////////////////////////

void
LtePdcp::DoTransmitPdcpSdu (LtePdcpSapProvider::TransmitPdcpSduParameters params)
{
  NS_LOG_FUNCTION (this << m_rnti << static_cast <uint16_t> (m_lcid) << params.pdcpSdu->GetSize ());
  Ptr<Packet> p = params.pdcpSdu;
  
  // Sender timestamp
  PdcpTag pdcpTag (Simulator::Now ());
  
  if (IsSlrb()) /* SLRB has different PDCP headers than DRB */
    {
      LteSlPdcpHeader pdcpHeader;
      pdcpHeader.SetSequenceNumber (m_txSequenceNumber);
      
      m_txSequenceNumber++;
      if (m_txSequenceNumber > m_maxPdcpSlSn)
        {
          m_txSequenceNumber = 0;
        }
      
      pdcpHeader.SetSduType (params.sduType);
      
      NS_LOG_LOGIC ("PDCP header: " << pdcpHeader);
      p->AddHeader (pdcpHeader);
      p->AddByteTag (pdcpTag, 1, pdcpHeader.GetSerializedSize ());
    }
  else
    {
      LtePdcpHeader pdcpHeader;
      pdcpHeader.SetSequenceNumber (m_txSequenceNumber);
      
      m_txSequenceNumber++;
      if (m_txSequenceNumber > m_maxPdcpSn)
        {
          m_txSequenceNumber = 0;
        }
      
      pdcpHeader.SetDcBit (LtePdcpHeader::DATA_PDU);
      
      NS_LOG_LOGIC ("PDCP header: " << pdcpHeader);
      p->AddHeader (pdcpHeader);
      p->AddByteTag (pdcpTag, 1, pdcpHeader.GetSerializedSize ());
    }
  
  m_txPdu (m_rnti, m_lcid, p->GetSize ());

  LteRlcSapProvider::TransmitPdcpPduParameters txParams;
  txParams.rnti = m_rnti;
  txParams.lcid = m_lcid;
  txParams.srcL2Id = m_srcL2Id;
  txParams.dstL2Id = m_dstL2Id;
  txParams.pdcpPdu = p;

  m_rlcSapProvider->TransmitPdcpPdu (txParams);
}

void
LtePdcp::DoReceivePdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << m_rnti << (uint32_t) m_lcid << p->GetSize ());

  uint8_t sduType = 0;
  // Receiver timestamp
  PdcpTag pdcpTag;
  Time delay;
  p->FindFirstMatchingByteTag (pdcpTag);
  delay = Simulator::Now() - pdcpTag.GetSenderTimestamp ();
  m_rxPdu(m_rnti, m_lcid, p->GetSize (), delay.GetNanoSeconds ());

  if (IsSlrb ())
    {
      LteSlPdcpHeader pdcpHeader;
      p->RemoveHeader (pdcpHeader);
      NS_LOG_LOGIC ("PDCP header: " << pdcpHeader);
      
      m_rxSequenceNumber = pdcpHeader.GetSequenceNumber () + 1;
      if (m_rxSequenceNumber > m_maxPdcpSlSn)
        {
          m_rxSequenceNumber = 0;
        }

      sduType = pdcpHeader.GetSduType ();
    } 
  else
    {
      LtePdcpHeader pdcpHeader;
      p->RemoveHeader (pdcpHeader);
      NS_LOG_LOGIC ("PDCP header: " << pdcpHeader);
      
      m_rxSequenceNumber = pdcpHeader.GetSequenceNumber () + 1;
      if (m_rxSequenceNumber > m_maxPdcpSn)
        {
          m_rxSequenceNumber = 0;
        }
    }

  LtePdcpSapUser::ReceivePdcpSduParameters params;
  params.pdcpSdu = p;
  params.rnti = m_rnti;
  params.lcid = m_lcid;
  params.srcL2Id = m_srcL2Id;
  params.dstL2Id = m_dstL2Id;
  params.sduType = sduType;
  m_pdcpSapUser->ReceivePdcpSdu (params);
}

bool
LtePdcp::IsSlrb ()
{
  NS_LOG_FUNCTION (this << m_rnti << (uint32_t) (m_srcL2Id != 0 || m_dstL2Id !=0));
  return (m_srcL2Id != 0 || m_dstL2Id !=0);
}

} // namespace ns3
