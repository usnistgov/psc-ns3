// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-sl-ue-mac-harq.h"

#include <ns3/log.h>
#include <ns3/packet-burst.h>
#include <ns3/packet.h>
#include <ns3/simulator.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlUeMacHarq");
NS_OBJECT_ENSURE_REGISTERED(NrSlUeMacHarq);

TypeId
NrSlUeMacHarq::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::NrSlUeMacHarq")
            .SetParent<Object>()
            .AddConstructor<NrSlUeMacHarq>()
            .SetGroupName("nr")
            .AddTraceSource("HarqFeedbackReceived",
                            "One trace call per received HARQ feedback (per PSFCH event)",
                            MakeTraceSourceAccessor(&NrSlUeMacHarq::m_harqFeedbackReceived),
                            "ns3::NrSlUeMacHarq::HarqFeedbackReceivedTracedCallback")
            .AddTraceSource("HarqTbCompletion",
                            "One trace call per HARQ event that terminates a TB's chain",
                            MakeTraceSourceAccessor(&NrSlUeMacHarq::m_harqTbCompletion),
                            "ns3::NrSlUeMacHarq::HarqTbCompletionTracedCallback")
            .AddTraceSource("Allocate",
                            "Trace HARQ process ID allocation",
                            MakeTraceSourceAccessor(&NrSlUeMacHarq::m_allocateTrace),
                            "ns3::NrSlUeMacHarq::AllocateTracedCallback")
            .AddTraceSource("Deallocate",
                            "Trace HARQ process ID deallocation",
                            MakeTraceSourceAccessor(&NrSlUeMacHarq::m_deallocateTrace),
                            "ns3::NrSlUeMacHarq::DeallocateTracedCallback")
            .AddTraceSource("RequestPacketBurst",
                            "Trace requests for packet bursts (tx and retx)",
                            MakeTraceSourceAccessor(&NrSlUeMacHarq::m_packetBurstTrace),
                            "ns3::NrSlUeMacHarq::PacketBurstTracedCallback")
            .AddTraceSource("Timeout",
                            "Trace HARQ process timer expiry",
                            MakeTraceSourceAccessor(&NrSlUeMacHarq::m_timeoutTrace),
                            "ns3::NrSlUeMacHarq::TimeoutTracedCallback");
    return tid;
}

NrSlUeMacHarq::NrSlUeMacHarq()
{
    NS_LOG_FUNCTION(this);
}

NrSlUeMacHarq::~NrSlUeMacHarq()
{
    NS_LOG_FUNCTION(this);
}

void
NrSlUeMacHarq::DoDispose()
{
    NS_LOG_FUNCTION(this);
    for (auto it : m_pktBuffer)
    {
        it.pktBurst = nullptr;
    }
    m_pktBuffer.clear();
}

void
NrSlUeMacHarq::InitHarqBuffer(uint8_t maxSlProcessesMultiplePdu, uint8_t maxSlProcesses)
{
    NS_LOG_FUNCTION(this << +maxSlProcessesMultiplePdu << +maxSlProcesses);

    m_maxSlProcessesMultiplePdu = maxSlProcessesMultiplePdu;
    m_maxSlProcesses = maxSlProcesses;
    NS_ABORT_MSG_UNLESS(maxSlProcesses >= maxSlProcessesMultiplePdu, "Misconfiguration");
    m_pktBuffer.resize(maxSlProcesses);
    for (uint8_t i = 0; i < maxSlProcesses; i++)
    {
        ResetHarqBuffer(i);
        m_idBuffer.push_back(i);
    }
}

std::optional<uint8_t>
NrSlUeMacHarq::AllocateHarqProcessId(uint32_t dstL2Id, bool multiplePdu, Time timeout, uint8_t mcs)
{
    NS_LOG_FUNCTION(this << dstL2Id << timeout);
    std::optional<uint8_t> harqId;
    if (!m_idBuffer.size())
    {
        NS_LOG_INFO("No HARQ process IDs available for " << dstL2Id);
        return harqId;
    }
    if (multiplePdu && m_numProcessesMultiplePdu == m_maxSlProcessesMultiplePdu)
    {
        NS_LOG_INFO("No HARQ process IDs for multiple PDUs available for " << dstL2Id);
        return harqId;
    }
    harqId = m_idBuffer.front();
    m_idBuffer.pop_front();
    if (multiplePdu)
    {
        m_numProcessesMultiplePdu++;
    }
    NS_LOG_INFO("Allocating HARQ ID " << +harqId.value() << " dstL2Id " << dstL2Id << " timeout "
                                      << timeout.As(Time::MS) << " multiple PDU " << multiplePdu
                                      << " mcs " << +mcs << " remaining " << m_idBuffer.size());
    m_allocateTrace(harqId.value(), dstL2Id, multiplePdu, timeout, m_idBuffer.size());
    // set the given destination in m_pktBuffer at the index equal to
    // availableHarqId so we can check it while adding the packet.
    m_pktBuffer.at(harqId.value()).dstL2Id = dstL2Id;
    NS_LOG_INFO("Scheduling HARQ process ID " << +harqId.value() << " timer to expire in "
                                              << timeout.As(Time::MS) << " at "
                                              << (Now() + timeout).As(Time::S));
    m_pktBuffer.at(harqId.value()).timer =
        Simulator::Schedule(timeout, &NrSlUeMacHarq::HarqProcessTimerExpiry, this, harqId.value());
    m_pktBuffer.at(harqId.value()).multiplePdu = multiplePdu;
    m_pktBuffer.at(harqId.value()).allocated = true;
    m_pktBuffer.at(harqId.value()).mcs = mcs;
    return harqId;
}

void
NrSlUeMacHarq::UpdateHarqProcess(uint8_t harqId, uint32_t numTx, bool harqEnabled, uint32_t tbSize)
{
    NS_LOG_FUNCTION(this << +harqId << numTx << harqEnabled << tbSize);
    NS_LOG_INFO("Updating process ID " << +harqId << " numTx " << numTx << " harqEnabled "
                                       << harqEnabled << " tbSize " << tbSize);
    m_pktBuffer.at(harqId).maxNumTx = numTx;
    m_pktBuffer.at(harqId).harqEnabled = harqEnabled;
    m_pktBuffer.at(harqId).tbSize = tbSize;
}

void
NrSlUeMacHarq::DeallocateHarqProcessId(uint8_t harqId)
{
    NS_LOG_FUNCTION(this << +harqId);
    if (m_pktBuffer.at(harqId).allocated)
    {
        if (m_pktBuffer.at(harqId).multiplePdu && m_numProcessesMultiplePdu)
        {
            m_numProcessesMultiplePdu--;
        }
        m_idBuffer.push_back(harqId);
        NS_LOG_INFO("Deallocating ID " << +harqId << " remaining IDs " << m_idBuffer.size());
        m_deallocateTrace(harqId, m_idBuffer.size());
        ResetHarqBuffer(harqId);
    }
}

bool
NrSlUeMacHarq::RenewHarqProcessIdTimer(uint8_t harqId, Time timeout)
{
    NS_LOG_FUNCTION(this << harqId << timeout);
    if (!m_pktBuffer.at(harqId).allocated)
    {
        NS_LOG_INFO("HARQ process ID " << +harqId << " is not allocated; not renewing timer");
        return false;
    }
    if (m_pktBuffer.at(harqId).timer.IsPending())
    {
        m_pktBuffer.at(harqId).timer.Cancel();
    }
    NS_LOG_INFO("Renewing HARQ process ID " << +harqId << " timer to expire in "
                                            << timeout.As(Time::MS) << " at "
                                            << (Now() + timeout).As(Time::S));
    m_pktBuffer.at(harqId).timer =
        Simulator::Schedule(timeout, &NrSlUeMacHarq::HarqProcessTimerExpiry, this, harqId);
    return true;
}

uint32_t
NrSlUeMacHarq::GetNumAvailableHarqIds() const
{
    return m_idBuffer.size();
}

bool
NrSlUeMacHarq::IsHarqIdAvailable(uint8_t harqId) const
{
    return !(m_pktBuffer.at(harqId).allocated);
}

void
NrSlUeMacHarq::AddPacket(uint32_t dstL2Id, uint8_t lcId, uint8_t harqId, Ptr<Packet> pkt)
{
    NS_LOG_FUNCTION(this << dstL2Id << +lcId << +harqId);
    NS_ABORT_MSG_IF(m_pktBuffer.at(harqId).dstL2Id == std::numeric_limits<uint16_t>::max(),
                    "Trying to add packet but dstL2Id for HARQ ID " << +harqId << " is unassigned");
    NS_ABORT_MSG_IF(m_pktBuffer.at(harqId).dstL2Id != dstL2Id,
                    "the HARQ id " << +harqId << " does not belongs to the destination " << dstL2Id
                                   << "; instead belongs to: " << m_pktBuffer.at(harqId).dstL2Id);
    NS_ASSERT_MSG(m_pktBuffer.at(harqId).pktBurst != nullptr,
                  " Packet burst not initialized for HARQ id " << +harqId);
    if (m_pktBuffer.at(harqId).multiplePdu && m_pktBuffer.at(harqId).pktBurst->GetNPackets() &&
        (m_pktBuffer.at(harqId).numTx == m_pktBuffer.at(harqId).maxNumTx))
    {
        // If there is an SPS grant and no HARQ feedback, there is no way
        // to clear out the previous TB, so flush it here.
        NS_LOG_INFO("Flushing buffer for for dstL2Id " << dstL2Id << " LC ID " << +lcId
                                                       << " HARQ ID " << +harqId);
        // Synthesize a final TIMEOUT for trace consumers: SPS HARQ ID is
        // being reused with the prior TB still in-flight (numTx ==
        // maxNumTx, no ACK received).  The missing PSFCH would otherwise
        // be silently absorbed by the flush.  Mirrors the inline TIMEOUT
        // path in GetPacketBurst at numTx == maxNumTx.
        SlHarqInfo harqInfo;
        harqInfo.m_harqProcessId = harqId;
        harqInfo.m_dstL2Id = dstL2Id;
        harqInfo.m_mcs = m_pktBuffer.at(harqId).mcs;
        harqInfo.m_harqStatus = SlHarqInfo::TIMEOUT;
        harqInfo.m_txIndex = m_pktBuffer.at(harqId).numTx;
        harqInfo.m_selectedNumTx = m_pktBuffer.at(harqId).maxNumTx;
        harqInfo.m_isFinal = true;
        m_harqFeedbackReceived(harqInfo);
        m_harqTbCompletion(harqInfo);
        FlushHarqBuffer(harqId);
    }
    NS_LOG_INFO("Adding packet for dstL2Id " << dstL2Id << " LC ID " << +lcId << " HARQ ID "
                                             << +harqId);
    m_pktBuffer.at(harqId).lcidList.insert(lcId);
    m_pktBuffer.at(harqId).pktBurst->AddPacket(pkt);
    // Each LC have one MAC PDU in a TB. Packet burst here, imitates a TB, therefore,
    // the number of LCs inside lcidList and the packets inside the packet burst
    // must be equal.
    NS_ABORT_MSG_IF(m_pktBuffer.at(harqId).lcidList.size() !=
                        m_pktBuffer.at(harqId).pktBurst->GetNPackets(),
                    "Mismatch in number of LCIDs and the number of packets for SL HARQ ID "
                        << +harqId << " dest " << dstL2Id);
    NS_ABORT_MSG_IF(m_pktBuffer.at(harqId).pktBurst->GetSize() > m_pktBuffer.at(harqId).tbSize,
                    "Mismatch between TB size and size of packet burst");
}

bool
NrSlUeMacHarq::RecvHarqFeedback(SlHarqInfo harqInfo)
{
    NS_LOG_FUNCTION(this << harqInfo.m_dstL2Id << +harqInfo.m_harqProcessId
                         << harqInfo.IsReceivedOk());
    // Populate per-tx context fields when the HARQ process is live, so trace
    // consumers and controllers can tell final events from intermediate ones
    // without having to look up MAC state themselves.  Stale feedback (arriving
    // after deallocation) leaves these fields as std::nullopt.
    if (!IsHarqIdAvailable(harqInfo.m_harqProcessId))
    {
        const auto& pkt = m_pktBuffer.at(harqInfo.m_harqProcessId);
        harqInfo.m_txIndex = pkt.numTx;
        harqInfo.m_selectedNumTx = pkt.maxNumTx;
        harqInfo.m_isFinal = harqInfo.IsReceivedOk() || (pkt.numTx == pkt.maxNumTx);
    }
    m_harqFeedbackReceived(harqInfo);
    if (harqInfo.m_isFinal.value_or(false))
    {
        m_harqTbCompletion(harqInfo);
    }
    if (IsHarqIdAvailable(harqInfo.m_harqProcessId))
    {
        NS_LOG_DEBUG("Feedback (possibly stale) received for unused HARQ ID "
                     << +harqInfo.m_harqProcessId);
        return false;
    }
    if (harqInfo.IsReceivedOk() &&
        (m_pktBuffer.at(harqInfo.m_harqProcessId).dstL2Id != harqInfo.m_dstL2Id))
    {
        NS_LOG_DEBUG("Feedback (possibly stale) received for different dstL2Id "
                     << harqInfo.m_dstL2Id << " on HARQ ID " << +harqInfo.m_harqProcessId);
        return false;
    }
    // Received HARQ feedback but there are no packets in the PacketBurst
    // buffer (possibly feedback for previous use of this HARQ ID)
    if (harqInfo.IsReceivedOk() &&
        !m_pktBuffer.at(harqInfo.m_harqProcessId).pktBurst->GetNPackets())
    {
        NS_LOG_DEBUG("Feedback (possibly stale) received for ID " << +harqInfo.m_harqProcessId
                                                                  << " with no transmissions");
        return false;
    }
    // Received HARQ feedback but there have been no transmissions of this
    // packet burst yet (possibly feedback for previous use of this HARQ ID)
    if (harqInfo.IsReceivedOk() &&
        m_pktBuffer.at(harqInfo.m_harqProcessId).pktBurst->GetNPackets() &&
        !m_pktBuffer.at(harqInfo.m_harqProcessId).numTx)
    {
        NS_LOG_DEBUG("Feedback (possibly stale) received for ID " << +harqInfo.m_harqProcessId
                                                                  << " with no transmissions");
        return false;
    }

    // If transmission is ACKed, and it is a dynamic grant, free both the
    // packet buffer and the HARQ ID.  If transmission is ACKed and it is an
    // SPS grant, do not free the HARQ ID but free the packet buffer
    if (harqInfo.IsReceivedOk())
    {
        NS_LOG_INFO("Positive feedback for dstL2Id " << harqInfo.m_dstL2Id << " on HARQ ID "
                                                     << +harqInfo.m_harqProcessId);
        if (m_pktBuffer.at(harqInfo.m_harqProcessId).pktBurst->GetSize())
        {
            // Only deallocate process IDs for dynamic grants upon ACK feedback
            if (m_pktBuffer.at(harqInfo.m_harqProcessId).multiplePdu)
            {
                FlushHarqBuffer(harqInfo.m_harqProcessId);
            }
            else
            {
                DeallocateHarqProcessId(harqInfo.m_harqProcessId);
            }
            return true;
        }
    }
    else
    {
        NS_LOG_INFO("Negative feedback for dstL2Id " << harqInfo.m_dstL2Id << " on HARQ ID "
                                                     << +harqInfo.m_harqProcessId);
        // Terminal NACK on an SPS bearer: reset the packet buffer so the
        // next SPS arrival's AddPacket() does not trip the silent-flush
        // path, which would synthesize a duplicate TIMEOUT and feed a
        // second negative-feedback event to controllers that subscribe
        // to m_harqFeedbackReceived.  The ACK path already flushes for
        // the same reason; mirror it here for the terminal-NACK case.
        const auto& pkt = m_pktBuffer.at(harqInfo.m_harqProcessId);
        if (pkt.multiplePdu && pkt.numTx == pkt.maxNumTx)
        {
            FlushHarqBuffer(harqInfo.m_harqProcessId);
        }
    }
    return false;
}

void
NrSlUeMacHarq::FlushHarqBuffer(uint8_t harqId)
{
    NS_LOG_FUNCTION(this << harqId);
    if (m_pktBuffer.at(harqId).pktBurst)
    {
        auto nPackets = m_pktBuffer.at(harqId).pktBurst->GetNPackets();
        if (nPackets)
        {
            NS_LOG_INFO("Flush packet buffer with " << nPackets << " packets for HARQ ID "
                                                    << +harqId);
        }
    }
    Ptr<PacketBurst> pb = CreateObject<PacketBurst>();
    m_pktBuffer.at(harqId).pktBurst = pb;
    m_pktBuffer.at(harqId).lcidList.clear();
    m_pktBuffer.at(harqId).numTx = 0;
}

Ptr<PacketBurst>
NrSlUeMacHarq::GetPacketBurst(uint32_t dstL2Id, uint8_t harqId)
{
    NS_LOG_FUNCTION(this << dstL2Id << +harqId);
    NS_ASSERT_MSG(m_pktBuffer.at(harqId).pktBurst, "Error, no PacketBurst object");
    if (m_pktBuffer.at(harqId).dstL2Id != dstL2Id || !m_pktBuffer.at(harqId).allocated)
    {
        // This operation can fail to return a packet burst if retransmissions
        // have been completed on this HARQ Process ID
        NS_LOG_DEBUG("No packet to return");
        return nullptr;
    }
    if (!m_pktBuffer.at(harqId).pktBurst->GetNPackets())
    {
        NS_LOG_INFO("No packets to retrieve for dstL2Id " << dstL2Id << " HARQ ID " << +harqId);
        return nullptr;
    }
    if (m_pktBuffer.at(harqId).numTx == m_pktBuffer.at(harqId).maxNumTx)
    {
        NS_LOG_INFO(
            "Maximum number of transmissions has been reached for packet in buffer, for dstL2Id "
            << dstL2Id << " HARQ ID " << +harqId);
        Simulator::ScheduleNow(&NrSlUeMacHarq::FlushHarqBuffer, this, harqId);
        // Provide an implicit NACK notification.  The TIMEOUT is emitted when
        // the scheduler attempts to retransmit past the selected cap, so this
        // event is final by construction (txIndex == selectedNumTx).
        SlHarqInfo harqInfo;
        harqInfo.m_harqProcessId = harqId;
        harqInfo.m_dstL2Id = dstL2Id;
        harqInfo.m_mcs = m_pktBuffer.at(harqId).mcs;
        harqInfo.m_harqStatus = SlHarqInfo::TIMEOUT;
        harqInfo.m_txIndex = m_pktBuffer.at(harqId).numTx;
        harqInfo.m_selectedNumTx = m_pktBuffer.at(harqId).maxNumTx;
        harqInfo.m_isFinal = true;
        m_harqFeedbackReceived(harqInfo);
        m_harqTbCompletion(harqInfo);
        return nullptr;
    }
    m_pktBuffer.at(harqId).numTx++;
    NS_LOG_INFO("Packet burst retrieved for dstL2Id " << dstL2Id << " HARQ ID " << +harqId
                                                      << " numTx " << m_pktBuffer.at(harqId).numTx);
    // If HARQ FB is disabled, there will be no feedback to free the
    // resources after the last transmission is made, so free them here
    if (!m_pktBuffer.at(harqId).harqEnabled &&
        (m_pktBuffer.at(harqId).numTx == m_pktBuffer.at(harqId).maxNumTx))
    {
        // Only deallocate process IDs for dynamic grants upon ACK feedback
        if (m_pktBuffer.at(harqId).multiplePdu)
        {
            // Calling this method directly leads to an assert in NrSlUeMac
            // because the pktBurst needs to be returned below before it is
            // flushed, so append this event for this sim time, but later
            Simulator::ScheduleNow(&NrSlUeMacHarq::FlushHarqBuffer, this, harqId);
        }
        else
        {
            Simulator::ScheduleNow(&NrSlUeMacHarq::DeallocateHarqProcessId, this, harqId);
        }
    }
    NS_ASSERT_MSG(m_pktBuffer.at(harqId).numTx <= m_pktBuffer.at(harqId).maxNumTx,
                  "Number of transmissions " << m_pktBuffer.at(harqId).numTx << " for ID "
                                             << +harqId << " exceeded "
                                             << m_pktBuffer.at(harqId).maxNumTx);
    m_packetBurstTrace(dstL2Id, harqId);
    return m_pktBuffer.at(harqId).pktBurst;
}

void
NrSlUeMacHarq::HarqProcessTimerExpiry(uint8_t harqId)
{
    NS_LOG_FUNCTION(this << +harqId);
    NS_LOG_INFO("HARQ process ID " << +harqId << " timed out");
    DeallocateHarqProcessId(harqId);
    m_timeoutTrace(harqId);
}

void
NrSlUeMacHarq::ResetHarqBuffer(uint8_t harqId)
{
    NS_LOG_FUNCTION(this << +harqId);
    FlushHarqBuffer(harqId);
    if (m_pktBuffer.at(harqId).timer.IsPending())
    {
        m_pktBuffer.at(harqId).timer.Cancel();
    }
    m_pktBuffer.at(harqId).dstL2Id = std::numeric_limits<uint16_t>::max();
    m_pktBuffer.at(harqId).multiplePdu = false;
    m_pktBuffer.at(harqId).allocated = false;
    m_pktBuffer.at(harqId).harqEnabled = false;
    m_pktBuffer.at(harqId).numTx = 0;
    m_pktBuffer.at(harqId).maxNumTx = 0;
    m_pktBuffer.at(harqId).tbSize = 0;
}

} // namespace ns3
