//
// SPDX-License-Identifier: NIST-Software
//

#include "nr-sl-test-mock-objects.h"

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/nr-pdcp-tag.h"
#include "ns3/nr-rlc-am-header.h"
#include "ns3/nr-rlc-header.h"
#include "ns3/nr-sl-mac-sap.h"
#include "ns3/nr-sl-pdcp-header.h"
#include "ns3/simulator.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrSlTestMockObjects");

TypeId
NrSlTestPdcp::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlTestPdcp").SetParent<Object>().AddConstructor<NrSlTestPdcp>();

    return tid;
}

NrSlTestPdcp::NrSlTestPdcp()
{
    NS_LOG_FUNCTION(this);
    m_nrSlRlcSapUser = new MemberNrSlRlcSapUser<NrSlTestPdcp>(this);
}

NrSlTestPdcp::~NrSlTestPdcp()
{
    NS_LOG_FUNCTION(this);
    delete m_nrSlRlcSapUser;
}

void
NrSlTestPdcp::SetNrSlRlcSapProvider(NrSlRlcSapProvider* s)
{
    m_nrSlRlcSapProvider = s;
}

NrSlRlcSapUser*
NrSlTestPdcp::GetNrSlRlcSapUser()
{
    return m_nrSlRlcSapUser;
}

void
NrSlTestPdcp::SendData(Time time, std::string dataToSend)
{
    NS_LOG_FUNCTION(this << time << dataToSend.length() << dataToSend);

    NS_LOG_LOGIC("Data(" << dataToSend.length() << ") = " << dataToSend.data());
    Ptr<Packet> pdcpPdu = Create<Packet>((uint8_t*)dataToSend.data(), dataToSend.length());

    NrSlRlcSapProvider::NrSlTransmitPdcpPduParameters p(pdcpPdu, 0, 0, 0, 0);

    NS_LOG_LOGIC("Packet(" << p.pdcpPdu->GetSize() << ")");
    Simulator::Schedule(time, &NrSlRlcSapProvider::TransmitNrSlPdcpPdu, m_nrSlRlcSapProvider, p);
}

void
NrSlTestPdcp::DoTransmitNrSlPdcpSdu(
    const NrSlPdcpSapProvider::NrSlTransmitPdcpSduParameters& params)
{
    NS_LOG_FUNCTION(this << params.pdcpSdu->GetSize());

    Ptr<Packet> p = params.pdcpSdu;
    NrPdcpTag pdcpTag(Simulator::Now());

    NrSlPdcpHeader pdcpHeader;
    pdcpHeader.SetSequenceNumber(++m_sn);
    pdcpHeader.SetSduType(params.sduType);

    NS_LOG_LOGIC("PDCP header: " << pdcpHeader);
    p->AddHeader(pdcpHeader);
    p->AddByteTag(pdcpTag, 1, pdcpHeader.GetSerializedSize());

    auto txParams = NrSlRlcSapProvider::NrSlTransmitPdcpPduParameters(p, 0, 0, 0, 0);

    m_nrSlRlcSapProvider->TransmitNrSlPdcpPdu(txParams);
}

void
NrSlTestPdcp::DoReceiveNrSlPdcpPdu(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << p->GetSize());
    NS_LOG_LOGIC("Data = " << (*p));

    uint32_t dataLen = p->GetSize();
    auto buf = new uint8_t[dataLen];
    p->CopyData(buf, dataLen);
    m_receivedData = std::string((char*)buf, dataLen);

    NS_LOG_LOGIC(m_receivedData);

    delete[] buf;
}

TypeId
NrSlTestMac::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlTestMac").SetParent<Object>().AddConstructor<NrSlTestMac>();

    return tid;
}

NrSlTestMac::NrSlTestMac()
{
    NS_LOG_FUNCTION(this);

    m_nrSlMacSapProvider = new MemberNrSlMacSapProvider<NrSlTestMac>(this);
}

NrSlTestMac::~NrSlTestMac()
{
    NS_LOG_FUNCTION(this);
    delete m_nrSlMacSapProvider;
}

void
NrSlTestMac::SetNrSlMacSapUser(NrSlMacSapUser* s)
{
    m_nrSlMacSapUser = s;
}

NrSlMacSapProvider*
NrSlTestMac::GetNrSlMacSapProvider()
{
    return m_nrSlMacSapProvider;
}

std::string
NrSlTestMac::GetDataReceived()
{
    return m_receivedData;
}

void
NrSlTestMac::SendTxOpportunity(Time time, uint32_t bytes)
{
    NS_LOG_FUNCTION(this << time << bytes);
    Ptr<Node> node;
    NrSlMacSapUser::NrSlTxOpportunityParameters txOpParams(bytes, // bytes
                                                           0,     // rnti
                                                           0,     // lcId
                                                           0,     // layer
                                                           0,     // harqId
                                                           0,     // bwpId
                                                           0,     // srcL2Id
                                                           0);    // dstL2Id

    Simulator::Schedule(time,
                        &NrSlMacSapUser::NotifyNrSlTxOpportunity,
                        m_nrSlMacSapUser,
                        txOpParams);
}

void
NrSlTestMac::SetPdcpHeaderPresent(bool present)
{
    m_pdcpHeaderPresent = present;
}

void
NrSlTestMac::SetRlcHeaderType(uint8_t rlcHeaderType)
{
    m_rlcHeaderType = rlcHeaderType;
}

void
NrSlTestMac::DoTransmitNrSlRlcPdu(const NrSlMacSapProvider::NrSlRlcPduParameters& params)
{
    NS_LOG_FUNCTION(this << params.pdu->GetSize());

    NrSlMacSapUser::NrSlReceiveRlcPduParameters rxPduParams(params.pdu,  // packet
                                                            params.rnti, // rnti
                                                            params.lcid, // lcid
                                                            0,           // srcL2Id
                                                            0);          // dstL2Id
    NrSlPdcpHeader pdcpHeader;
    if (m_rlcHeaderType == AM_RLC_HEADER)
    {
        // Remove AM RLC header
        NrRlcAmHeader rlcAmHeader;
        params.pdu->RemoveHeader(rlcAmHeader);
        NS_LOG_LOGIC("AM RLC header: " << rlcAmHeader);
    }
    else // if (m_rlcHeaderType == UM_RLC_HEADER)
    {
        // Remove UM RLC header
        NrRlcHeader rlcHeader;
        params.pdu->RemoveHeader(rlcHeader);
        NS_LOG_LOGIC("UM RLC header: " << rlcHeader);
    }

    // Remove PDCP header, if present
    if (m_pdcpHeaderPresent)
    {
        params.pdu->RemoveHeader(pdcpHeader);
        NS_LOG_LOGIC("PDCP header: " << pdcpHeader);
    }

    // Copy data to a string
    uint32_t dataLen = params.pdu->GetSize();
    auto buf = new uint8_t[dataLen];
    params.pdu->CopyData(buf, dataLen);
    m_receivedData = std::string((char*)buf, dataLen);

    NS_LOG_LOGIC("Data (" << dataLen << ") = " << m_receivedData);
    delete[] buf;
}

void
NrSlTestMac::DoReportNrSlBufferStatus(
    const NrSlMacSapProvider::NrSlReportBufferStatusParameters& params)
{
    NS_LOG_FUNCTION(this << params.txQueueSize << params.retxQueueSize << params.statusPduSize);
}
