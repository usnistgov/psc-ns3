// Copyright (c) 2011,2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>
//         Nicola Baldo <nbaldo@cttc.es>

#include "nr-rlc-tm.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrRlcTm");

NS_OBJECT_ENSURE_REGISTERED(NrRlcTm);

NrRlcTm::NrRlcTm()
    : m_maxTxBufferSize(0),
      m_txBufferSize(0)
{
    NS_LOG_FUNCTION(this);
}

NrRlcTm::~NrRlcTm()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrRlcTm::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrRlcTm")
                            .SetParent<NrRlc>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrRlcTm>()
                            .AddAttribute("MaxTxBufferSize",
                                          "Maximum Size of the Transmission Buffer (in Bytes)",
                                          UintegerValue(2 * 1024 * 1024),
                                          MakeUintegerAccessor(&NrRlcTm::m_maxTxBufferSize),
                                          MakeUintegerChecker<uint32_t>());
    return tid;
}

void
NrRlcTm::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_rbsTimer.Cancel();
    m_txBuffer.clear();

    NrRlc::DoDispose();
}

/**
 * RLC SAP
 */

void
NrRlcTm::DoTransmitPdcpPdu(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << m_rnti << (uint32_t)m_lcid << p->GetSize());

    if (m_txBufferSize + p->GetSize() <= m_maxTxBufferSize)
    {
        NS_LOG_LOGIC("Tx Buffer: New packet added");
        m_txBuffer.emplace_back(p, Simulator::Now());
        m_txBufferSize += p->GetSize();
        NS_LOG_LOGIC("NumOfBuffers = " << m_txBuffer.size());
        NS_LOG_LOGIC("txBufferSize = " << m_txBufferSize);
    }
    else
    {
        // Discard full RLC SDU
        NS_LOG_LOGIC("TxBuffer is full. RLC SDU discarded");
        NS_LOG_LOGIC("MaxTxBufferSize = " << m_maxTxBufferSize);
        NS_LOG_LOGIC("txBufferSize    = " << m_txBufferSize);
        NS_LOG_LOGIC("packet size     = " << p->GetSize());
    }

    /** Report Buffer Status */
    DoReportBufferStatus();
    m_rbsTimer.Cancel();
}

/**
 * MAC SAP
 */

void
NrRlcTm::DoNotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams)
{
    NS_LOG_FUNCTION(this << m_rnti << (uint32_t)m_lcid << txOpParams.bytes
                         << (uint32_t)txOpParams.layer << (uint32_t)txOpParams.harqId);

    // 5.1.1.1 Transmit operations
    // 5.1.1.1.1 General
    // When submitting a new TMD PDU to lower layer, the transmitting TM RLC entity shall:
    // - submit a RLC SDU without any modification to lower layer.

    if (m_txBuffer.empty())
    {
        NS_LOG_LOGIC("No data pending");
        return;
    }

    Ptr<Packet> packet = m_txBuffer.begin()->m_pdu->Copy();

    if (txOpParams.bytes < packet->GetSize())
    {
        NS_LOG_WARN("TX opportunity too small = " << txOpParams.bytes
                                                  << " (PDU size: " << packet->GetSize() << ")");
        return;
    }

    m_txBufferSize -= packet->GetSize();
    m_txBuffer.erase(m_txBuffer.begin());

    m_txPdu(m_rnti, m_lcid, packet->GetSize());

    // Send RLC PDU to MAC layer
    NrMacSapProvider::TransmitPduParameters params;
    params.pdu = packet;
    params.rnti = m_rnti;
    params.lcid = m_lcid;
    params.layer = txOpParams.layer;
    params.harqProcessId = txOpParams.harqId;
    params.componentCarrierId = txOpParams.componentCarrierId;

    m_macSapProvider->TransmitPdu(params);

    if (!m_txBuffer.empty())
    {
        m_rbsTimer.Cancel();
        m_rbsTimer = Simulator::Schedule(MilliSeconds(10), &NrRlcTm::ExpireRbsTimer, this);
    }
}

void
NrRlcTm::DoNotifyHarqDeliveryFailure()
{
    NS_LOG_FUNCTION(this);
}

void
NrRlcTm::DoReceivePdu(NrMacSapUser::ReceivePduParameters rxPduParams)
{
    NS_LOG_FUNCTION(this << m_rnti << (uint32_t)m_lcid << rxPduParams.p->GetSize());

    m_rxPdu(m_rnti, m_lcid, rxPduParams.p->GetSize(), 0);

    // 5.1.1.2 Receive operations
    // 5.1.1.2.1  General
    // When receiving a new TMD PDU from lower layer, the receiving TM RLC entity shall:
    // - deliver the TMD PDU without any modification to upper layer.

    m_rlcSapUser->ReceivePdcpPdu(rxPduParams.p);
}

void
NrRlcTm::DoReportBufferStatus()
{
    Time holDelay(0);
    uint32_t queueSize = 0;

    if (!m_txBuffer.empty())
    {
        holDelay = Simulator::Now() - m_txBuffer.front().m_waitingSince;

        queueSize = m_txBufferSize; // just data in tx queue (no header overhead for RLC TM)
    }

    NrMacSapProvider::ReportBufferStatusParameters r;
    r.rnti = m_rnti;
    r.lcid = m_lcid;
    r.txQueueSize = queueSize;
    r.txQueueHolDelay = holDelay.GetMilliSeconds();
    r.retxQueueSize = 0;
    r.retxQueueHolDelay = 0;
    r.statusPduSize = 0;

    NS_LOG_LOGIC("Send ReportBufferStatus = " << r.txQueueSize << ", " << r.txQueueHolDelay);
    m_macSapProvider->ReportBufferStatus(r);
}

void
NrRlcTm::ExpireRbsTimer()
{
    NS_LOG_LOGIC("RBS Timer expires");

    if (!m_txBuffer.empty())
    {
        DoReportBufferStatus();
        m_rbsTimer = Simulator::Schedule(MilliSeconds(10), &NrRlcTm::ExpireRbsTimer, this);
    }
}

} // namespace ns3
