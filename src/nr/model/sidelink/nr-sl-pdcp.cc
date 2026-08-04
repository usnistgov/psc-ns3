// Copyright (c) 2011-2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//

#include "nr-sl-pdcp.h"

#include "nr-sl-pdcp-header.h"

#include "ns3/log.h"
#include "ns3/nr-pdcp-tag.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlPdcp");

NS_OBJECT_ENSURE_REGISTERED(NrSlPdcp);

NrSlPdcp::NrSlPdcp()
{
    NS_LOG_FUNCTION(this);
    m_nrSlPdcpSapProvider = new MemberNrSlPdcpSapProvider<NrSlPdcp>(this);
    m_nrSlRlcSapUser = new MemberNrSlRlcSapUser<NrSlPdcp>(this);
}

TypeId
NrSlPdcp::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlPdcp").SetParent<NrPdcp>().SetGroupName("Nr");
    return tid;
}

void
NrSlPdcp::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_nrSlPdcpSapProvider;
    delete m_nrSlRlcSapUser;
    NrPdcp::DoDispose();
}

void
NrSlPdcp::DoTransmitNrSlPdcpSdu(const NrSlPdcpSapProvider::NrSlTransmitPdcpSduParameters& params)
{
    NS_LOG_FUNCTION(this << m_rnti << (uint32_t)m_lcid << params.pdcpSdu->GetSize());

    Ptr<Packet> p = params.pdcpSdu;
    // Sender timestamp
    NrPdcpTag pdcpTag(Simulator::Now());

    NS_ASSERT_MSG(IsSlRb(),
                  "Did you forget to set source layer 2 or destination layer 2 id in PDCP?");

    NrSlPdcpHeader pdcpHeader;
    pdcpHeader.SetSequenceNumber(m_txSequenceNumber);

    m_txSequenceNumber++;
    if (m_txSequenceNumber > m_maxPdcpSlSn)
    {
        m_txSequenceNumber = 0;
    }

    pdcpHeader.SetSduType(params.sduType);

    NS_LOG_LOGIC("PDCP header: " << pdcpHeader);
    p->AddHeader(pdcpHeader);
    p->AddByteTag(pdcpTag, 1, pdcpHeader.GetSerializedSize());

    m_txPdu(m_rnti, m_lcid, p->GetSize());

    auto txParams =
        NrSlRlcSapProvider::NrSlTransmitPdcpPduParameters(p, m_rnti, m_lcid, m_srcL2Id, m_dstL2Id);

    NS_LOG_INFO("Transmitting PDCP PDU on LC " << +m_lcid << " to dstL2Id " << m_dstL2Id
                                               << " with header: " << pdcpHeader);
    m_nrSlRlcSapProvider->TransmitNrSlPdcpPdu(txParams);
}

void
NrSlPdcp::DoReceiveNrSlPdcpPdu(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << m_rnti << (uint32_t)m_lcid << p->GetSize());

    // Receiver timestamp
    NrPdcpTag pdcpTag;
    Time delay;
    p->FindFirstMatchingByteTag(pdcpTag);
    delay = Simulator::Now() - pdcpTag.GetSenderTimestamp();
    m_rxPdu(m_rnti, m_lcid, p->GetSize(), delay.GetNanoSeconds());
    uint8_t sduType = 0;

    NS_ASSERT_MSG(IsSlRb(),
                  "Did you forget to set source layer 2 or destination layer 2 id in PDCP?");

    NrSlPdcpHeader pdcpHeader;
    p->RemoveHeader(pdcpHeader);
    NS_LOG_INFO("Receiving PDCP PDU with header: " << pdcpHeader
                                                   << " delay: " << delay.As(Time::US));

    m_rxSequenceNumber = pdcpHeader.GetSequenceNumber() + 1;
    if (m_rxSequenceNumber > m_maxPdcpSlSn)
    {
        m_rxSequenceNumber = 0;
    }

    sduType = pdcpHeader.GetSduType();

    auto params = NrSlPdcpSapUser::NrSlReceivePdcpSduParameters(p,
                                                                m_rnti,
                                                                m_lcid,
                                                                m_srcL2Id,
                                                                m_dstL2Id,
                                                                sduType);
    m_nrSlPdcpSapUser->ReceiveNrSlPdcpSdu(params);
}

NrSlPdcpSapProvider*
NrSlPdcp::GetNrSlPdcpSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlPdcpSapProvider;
}

void
NrSlPdcp::SetNrSlPdcpSapUser(NrSlPdcpSapUser* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlPdcpSapUser = s;
}

void
NrSlPdcp::SetNrSlRlcSapProvider(NrSlRlcSapProvider* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlRlcSapProvider = s;
}

NrSlRlcSapUser*
NrSlPdcp::GetNrSlRlcSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlRlcSapUser;
}

void
NrSlPdcp::SetSourceL2Id(uint32_t src)
{
    NS_LOG_FUNCTION(this << src);
    m_srcL2Id = src;
}

void
NrSlPdcp::SetDestinationL2Id(uint32_t dst)
{
    NS_LOG_FUNCTION(this << dst);
    m_dstL2Id = dst;
}

bool
NrSlPdcp::IsSlRb()
{
    NS_LOG_FUNCTION(this << m_rnti << m_srcL2Id << m_dstL2Id);
    return (m_srcL2Id != 0 || m_dstL2Id != 0);
}

} // namespace ns3
