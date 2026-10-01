// Copyright (c) 2011-2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#include "nr-pdcp.h"

#include "nr-pdcp-header.h"
#include "nr-pdcp-sap.h"
#include "nr-pdcp-tag.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrPdcp");

/// NrPdcpSpecificNrRlcSapUser class
class NrPdcpSpecificNrRlcSapUser : public NrRlcSapUser
{
  public:
    /**
     * Constructor
     *
     * \param pdcp PDCP
     */
    NrPdcpSpecificNrRlcSapUser(NrPdcp* pdcp);

    // Interface provided to lower RLC entity (implemented from NrRlcSapUser)
    void ReceivePdcpPdu(Ptr<Packet> p) override;

  private:
    NrPdcpSpecificNrRlcSapUser();
    NrPdcp* m_pdcp; ///< the PDCP
};

NrPdcpSpecificNrRlcSapUser::NrPdcpSpecificNrRlcSapUser(NrPdcp* pdcp)
    : m_pdcp(pdcp)
{
}

NrPdcpSpecificNrRlcSapUser::NrPdcpSpecificNrRlcSapUser()
{
}

void
NrPdcpSpecificNrRlcSapUser::ReceivePdcpPdu(Ptr<Packet> p)
{
    m_pdcp->DoReceivePdu(p);
}

///////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(NrPdcp);

NrPdcp::NrPdcp()
    : m_pdcpSapUser(nullptr),
      m_rlcSapProvider(nullptr),
      m_rnti(0),
      m_lcid(0),
      m_txSequenceNumber(0),
      m_rxSequenceNumber(0)
{
    NS_LOG_FUNCTION(this);
    m_pdcpSapProvider = new NrPdcpSpecificNrPdcpSapProvider<NrPdcp>(this);
    m_rlcSapUser = new NrPdcpSpecificNrRlcSapUser(this);
}

NrPdcp::~NrPdcp()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrPdcp::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrPdcp")
                            .SetParent<Object>()
                            .SetGroupName("Nr")
                            .AddTraceSource("TxPDU",
                                            "PDU transmission notified to the RLC.",
                                            MakeTraceSourceAccessor(&NrPdcp::m_txPdu),
                                            "ns3::NrPdcp::PduTxTracedCallback")
                            .AddTraceSource("RxPDU",
                                            "PDU received.",
                                            MakeTraceSourceAccessor(&NrPdcp::m_rxPdu),
                                            "ns3::NrPdcp::PduRxTracedCallback");
    return tid;
}

void
NrPdcp::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete (m_pdcpSapProvider);
    delete (m_rlcSapUser);
}

void
NrPdcp::SetRnti(uint16_t rnti)
{
    NS_LOG_FUNCTION(this << (uint32_t)rnti);
    m_rnti = rnti;
}

void
NrPdcp::SetLcId(uint8_t lcId)
{
    NS_LOG_FUNCTION(this << (uint32_t)lcId);
    m_lcid = lcId;
}

void
NrPdcp::SetNrPdcpSapUser(NrPdcpSapUser* s)
{
    NS_LOG_FUNCTION(this << s);
    m_pdcpSapUser = s;
}

NrPdcpSapProvider*
NrPdcp::GetNrPdcpSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_pdcpSapProvider;
}

void
NrPdcp::SetNrRlcSapProvider(NrRlcSapProvider* s)
{
    NS_LOG_FUNCTION(this << s);
    m_rlcSapProvider = s;
}

NrRlcSapUser*
NrPdcp::GetNrRlcSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_rlcSapUser;
}

NrPdcp::Status
NrPdcp::GetStatus() const
{
    Status s;
    s.txSn = m_txSequenceNumber;
    s.rxSn = m_rxSequenceNumber;
    return s;
}

void
NrPdcp::SetStatus(Status s)
{
    m_txSequenceNumber = s.txSn;
    m_rxSequenceNumber = s.rxSn;
}

////////////////////////////////////////

void
NrPdcp::DoTransmitPdcpSdu(NrPdcpSapProvider::TransmitPdcpSduParameters params)
{
    NS_LOG_FUNCTION(this << m_rnti << static_cast<uint16_t>(m_lcid) << params.pdcpSdu->GetSize());
    Ptr<Packet> p = params.pdcpSdu;

    // Sender timestamp
    NrPdcpTag pdcpTag(Simulator::Now());

    NrPdcpHeader pdcpHeader;
    pdcpHeader.SetSequenceNumber(m_txSequenceNumber);

    m_txSequenceNumber++;
    if (m_txSequenceNumber > m_maxPdcpSn)
    {
        m_txSequenceNumber = 0;
    }

    pdcpHeader.SetDcBit(NrPdcpHeader::DATA_PDU);
    p->AddHeader(pdcpHeader);
    p->AddByteTag(pdcpTag, 1, pdcpHeader.GetSerializedSize());

    m_txPdu(m_rnti, m_lcid, p->GetSize());

    NrRlcSapProvider::TransmitPdcpPduParameters txParams;
    txParams.rnti = m_rnti;
    txParams.lcid = m_lcid;
    txParams.pdcpPdu = p;

    NS_LOG_INFO("Transmitting PDCP PDU with header: " << pdcpHeader);
    m_rlcSapProvider->TransmitPdcpPdu(txParams);
}

void
NrPdcp::DoReceivePdu(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << m_rnti << (uint32_t)m_lcid << p->GetSize());

    // Receiver timestamp
    NrPdcpTag pdcpTag;
    Time delay;
    p->FindFirstMatchingByteTag(pdcpTag);
    delay = Simulator::Now() - pdcpTag.GetSenderTimestamp();
    m_rxPdu(m_rnti, m_lcid, p->GetSize(), delay.GetNanoSeconds());

    NrPdcpHeader pdcpHeader;
    p->RemoveHeader(pdcpHeader);
    NS_LOG_LOGIC("PDCP header: " << pdcpHeader);

    m_rxSequenceNumber = pdcpHeader.GetSequenceNumber() + 1;
    if (m_rxSequenceNumber > m_maxPdcpSn)
    {
        m_rxSequenceNumber = 0;
    }

    NrPdcpSapUser::ReceivePdcpSduParameters params;
    params.pdcpSdu = p;
    params.rnti = m_rnti;
    params.lcid = m_lcid;
    m_pdcpSapUser->ReceivePdcpSdu(params);
}

} // namespace ns3
