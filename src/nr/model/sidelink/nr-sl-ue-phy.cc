// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2015 NYU WIRELESS, Tandon School of Engineering, New York University
// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#define NS_LOG_APPEND_CONTEXT                                                                      \
    do                                                                                             \
    {                                                                                              \
        std::clog << " [ CellId " << GetCellId() << ", bwpId " << GetBwpId() << "] ";              \
    } while (false);

#include "nr-sl-ue-phy.h"

#include "nr-sl-comm-resource-pool.h"
#include "nr-sl-spectrum-phy.h"

#include "ns3/nr-radio-bearer-tag.h"
#include "ns3/nr-ue-net-device.h"
#include <ns3/node.h>
#include <ns3/object-vector.h>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrSlUePhy);

NS_LOG_COMPONENT_DEFINE("NrSlUePhy");

NrSlUePhy::NrSlUePhy()
    : NrUePhy()
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeCphySapProvider = new MemberNrSlUeCphySapProvider<NrSlUePhy>(this);
    m_nrSlUePhySapProvider = new MemberNrSlUePhySapProvider<NrSlUePhy>(this);
    DoReset();
}

NrSlUePhy::~NrSlUePhy()
{
    NS_LOG_FUNCTION(this);
    m_slHarqFbList.clear();
}

void
NrSlUePhy::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_nrSlUeCphySapProvider;
    delete m_nrSlUePhySapProvider;
    m_slTxPool = nullptr;
    m_slRxPool = nullptr;
    NrUePhy::DoDispose();
}

TypeId
NrSlUePhy::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlUePhy")
            .SetParent<NrUePhy>()
            .AddConstructor<NrSlUePhy>()
            .AddAttribute("RsrpFilterPeriod",
                          "L1 Filter period for SD-/SL-RSRP."
                          "The default value is arbitrary as it is left to UE implementation",
                          TimeValue(MilliSeconds(200)),
                          MakeTimeAccessor(&NrSlUePhy::m_rsrpFilterPeriod),
                          MakeTimeChecker())
            .AddTraceSource("SdRsrpMeasurement",
                            "Trace for reporting SD-RSRP measurements",
                            MakeTraceSourceAccessor(&NrSlUePhy::m_sdRsrpMeasurementsTrace),
                            "ns3::NrSlUePhy::SdRsrpMeasurementsTracedCallback")
            .AddTraceSource("SlRsrpMeasurement",
                            "Trace for reporting SL-RSRP measurements",
                            MakeTraceSourceAccessor(&NrSlUePhy::m_slRsrpMeasurementsTrace),
                            "ns3::NrSlUePhy::SlRsrpMeasurementsTracedCallback");
    return tid;
}

void
NrSlUePhy::StartSlot(const SfnSf& s)
{
    NS_LOG_FUNCTION(this);
    SetCurrentSfnSf(s);
    SetLastSlotStart(Simulator::Now());

    // Call MAC before doing anything in PHY
    GetPhySapUser()->SlotIndication(GetCurrentSfnSf()); // trigger mac

    /*
     * Clear SL expected TB not received in previous slot.
     * It may happen that a UE is expecting to receive a TB in a slot, however,
     * in the same slot it decided to transmit. In this case, due to the half-duplex
     * nature of the Sidelink it will not receive that TB. Thus, the information
     * inserted in the m_slTransportBlocks buffer will be out-dated in the next slot,
     * hence, must be removed at the beginning of the next slot. This is also due
     * to the fact that in current implementation we always prioritize transmission
     * over reception without looking at the priority of the two TBs, i.e, the one
     * which needs to be transmitted and the one which need to be received.
     * As per the 3GPP standard, a device might prioritize RX over TX as per
     * the priority or vice versa.
     */
    auto slSpectrumPhy = m_spectrumPhy->GetObject<NrSlSpectrumPhy>();
    NS_ASSERT_MSG(slSpectrumPhy, "Did not find NrSlSpectrumPhy object");
    slSpectrumPhy->ClearExpectedSlTb();

    // If an SCI-2/TB was indicated for this slot by a past SCI-1, the information
    // would have been cached in m_assignedResources.  The next statement causes
    // an 'expected TB' to be generated for this SCI-2/TB' from that cache.
    SendSlExpectedTbInfo(s);

    if (NrSlSlotAllocInfoExists(GetCurrentSfnSf()))
    {
        StartNrSlSlot(s);
    }
    else
    {
        // Call a base class method to perform control allocations on slots
        // that are not sidelink slots
        bool nrAllocationExists = false;
        FinishSlotProcessing(s, nrAllocationExists);
    }
}

void
NrSlUePhy::EnqueueSlHarqFeedback(const SlHarqInfo& m)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO("Enqueued SL HARQ " << (m.IsReceivedOk() ? "ACK" : "NACK") << " in slot "
                                    << GetCurrentSfnSf().Normalize() << " for process "
                                    << +m.m_harqProcessId);
    Ptr<NrSlHarqFeedbackMessage> msg = Create<NrSlHarqFeedbackMessage>();
    msg->SetSlHarqFeedback(m);
    m_slHarqFbList.emplace_back(GetCurrentSfnSf(), msg);
}

void
NrSlUePhy::DoReset()
{
    NS_LOG_FUNCTION(this);
    // initialize NR SL PSCCH packet queue
    m_nrSlPscchPacketBurstQueue.clear();
    Ptr<PacketBurst> pbPscch = CreateObject<PacketBurst>();
    m_nrSlPscchPacketBurstQueue.push_back(pbPscch);

    // initialize NR SL PSSCH packet queue
    m_nrSlPsschPacketBurstQueue.clear();
    // initialize NR SL PSFCH feedback queue
    m_slHarqFbList.clear();
}

void
NrSlUePhy::ReportUeSdRsrpMeasurements()
{
    NS_LOG_FUNCTION(this << GetRnti());
    if (m_ueSdRsrpMeasurementsEnabled)
    {
        NrSlUeCphySapUser::RsrpElementsList rsrpList;
        // Perform the L1 filtering
        for (auto it = m_ueSdRsrpMeasurementsMap.begin(); it != m_ueSdRsrpMeasurementsMap.end();
             it++)
        {
            // L1 filtering: linear average
            double avgRsrpW = it->second.rsrpSum / static_cast<double>(it->second.rsrpNum);
            // The stored values are in W, the report to the MAC/RRC should be in dBm
            double avgRsrpDbm = 10 * log10(1000 * (avgRsrpW));

            NS_LOG_INFO("L2Id " << it->first << " averaged SD-RSRP (dBm) " << avgRsrpDbm
                                << " number of measurements " << it->second.rsrpNum);
            NrSlUeCphySapUser::RsrpElement elt;
            elt.l2Id = it->first;
            elt.rsrp = avgRsrpDbm;
            rsrpList.rsrpMeasurementsList.push_back(elt);
        }

        // Notify RRC
        m_nrSlUeCphySapUser->ReceiveUeSdRsrpMeasurements(rsrpList);

        // Schedule next L1 filtering
        Simulator::ScheduleWithContext(GetRnti(),
                                       m_rsrpFilterPeriod,
                                       &NrSlUePhy::ReportUeSdRsrpMeasurements,
                                       this);

        // Clear map after finishing the L1 filtering
        m_ueSdRsrpMeasurementsMap.clear();
    }
}

void
NrSlUePhy::ReportUeSlRsrpMeasurements()
{
    NS_LOG_FUNCTION(this << GetRnti());
    if (m_ueSlRsrpMeasurementsEnabled)
    {
        NrSlUeCphySapUser::RsrpElementsList rsrpList;
        // Perform the L1 filtering
        for (auto it = m_ueSlRsrpMeasurementsMap.begin(); it != m_ueSlRsrpMeasurementsMap.end();
             it++)
        {
            // L1 filtering: linear average
            double avgRsrpW = it->second.rsrpSum / static_cast<double>(it->second.rsrpNum);
            // The stored values are in W, the report to the MAC/RRC should be in dBm
            double avgRsrpDbm = 10 * log10(1000 * (avgRsrpW));

            NS_LOG_INFO("L2Id " << it->first << " averaged SL-RSRP (dBm) " << avgRsrpDbm
                                << " number of measurements " << it->second.rsrpNum);
            NrSlUeCphySapUser::RsrpElement elt;
            elt.l2Id = it->first;
            elt.rsrp = avgRsrpDbm;
            rsrpList.rsrpMeasurementsList.push_back(elt);
        }

        // Notify RRC
        m_nrSlUeCphySapUser->ReceiveUeSlRsrpMeasurements(rsrpList);

        // Schedule next L1 filtering
        Simulator::ScheduleWithContext(GetRnti(),
                                       m_rsrpFilterPeriod,
                                       &NrSlUePhy::ReportUeSlRsrpMeasurements,
                                       this);

        // Clear map after finishing the L1 filtering
        m_ueSlRsrpMeasurementsMap.clear();
    }
}

void
NrSlUePhy::PreConfigSlBandwidth(uint16_t slBandwidth)
{
    NS_LOG_FUNCTION(this << slBandwidth);
    if (GetChannelBandwidth() != slBandwidth)
    {
        SetChannelBandwidth(slBandwidth);
    }
}

void
NrSlUePhy::RegisterSlBwpId(uint16_t bwpId)
{
    NS_LOG_FUNCTION(this);

    // we initialize queues in DoReset;

    SetBwpId(bwpId);
}

NrSlUeCphySapProvider*
NrSlUePhy::GetNrSlUeCphySapProvider()
{
    return m_nrSlUeCphySapProvider;
}

void
NrSlUePhy::SetNrSlUeCphySapUser(NrSlUeCphySapUser* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeCphySapUser = s;
}

void
NrSlUePhy::SetNrSlUePhySapUser(NrSlUePhySapUser* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUePhySapUser = s;
}

void
NrSlUePhy::DoAddNrSlCommTxPool(Ptr<const NrSlCommResourcePool> txPool)
{
    NS_LOG_FUNCTION(this);
    m_slTxPool = txPool;
}

void
NrSlUePhy::DoAddNrSlCommRxPool(Ptr<const NrSlCommResourcePool> rxPool)
{
    NS_LOG_FUNCTION(this);
    m_slRxPool = rxPool;
}

NrSlUePhySapProvider*
NrSlUePhy::GetNrSlUePhySapProvider()
{
    return m_nrSlUePhySapProvider;
}

Time
NrSlUePhy::DoGetSlotPeriod() const
{
    return GetSlotPeriod();
}

uint32_t
NrSlUePhy::DoGetBwInRbs() const
{
    return GetRbNum();
}

void
NrSlUePhy::DoSendPscchMacPdu(Ptr<Packet> p)
{
    SetPscchMacPdu(p);
}

void
NrSlUePhy::DoSendPsschMacPdu(Ptr<Packet> p, uint32_t dstL2Id)
{
    SetPsschMacPdu(p, dstL2Id);
}

void
NrSlUePhy::SetPscchMacPdu(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this);
    // Since we must send one SCI msg at a given time, initially the queue size must
    // be 1 with an empty packet burst
    NS_ASSERT_MSG(m_nrSlPscchPacketBurstQueue.size() == 1 &&
                      m_nrSlPscchPacketBurstQueue.at(0)->GetNPackets() == 0,
                  "Error in Pscch queue size and packet burst size (should be 1 and 0): "
                      << m_nrSlPscchPacketBurstQueue.size() << " "
                      << m_nrSlPscchPacketBurstQueue.at(0)->GetNPackets());
    m_nrSlPscchPacketBurstQueue.at(0)->AddPacket(p);
}

void
NrSlUePhy::SetPsschMacPdu(Ptr<Packet> p, uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    // The packets in this packet burst would be equal to the number of LCs
    // multiplexed together plus one SCI format 2 packet
    auto it = m_nrSlPsschPacketBurstQueue.find(dstL2Id);
    if (it == m_nrSlPsschPacketBurstQueue.end())
    {
        Ptr<PacketBurst> pb = CreateObject<PacketBurst>();
        pb->AddPacket(p);
        m_nrSlPsschPacketBurstQueue.insert({dstL2Id, pb});
    }
    else
    {
        it->second->AddPacket(p);
    }
}

Ptr<PacketBurst>
NrSlUePhy::PopPscchPacketBurst()
{
    NS_LOG_FUNCTION(this);
    if (m_nrSlPscchPacketBurstQueue.at(0)->GetSize() > 0)
    {
        Ptr<PacketBurst> ret = m_nrSlPscchPacketBurstQueue.at(0)->Copy();
        m_nrSlPscchPacketBurstQueue.erase(m_nrSlPscchPacketBurstQueue.begin());
        m_nrSlPscchPacketBurstQueue.push_back(CreateObject<PacketBurst>());
        return (ret);
    }
    else
    {
        m_nrSlPscchPacketBurstQueue.erase(m_nrSlPscchPacketBurstQueue.begin());
        m_nrSlPscchPacketBurstQueue.push_back(CreateObject<PacketBurst>());
        return (nullptr);
    }
}

Ptr<PacketBurst>
NrSlUePhy::PopPsschPacketBurst()
{
    NS_LOG_FUNCTION(this);
    auto it = m_nrSlPsschPacketBurstQueue.begin();
    if (it != m_nrSlPsschPacketBurstQueue.end())
    {
        if (it->second->GetSize() > 0)
        {
            Ptr<PacketBurst> ret = it->second->Copy();
            m_nrSlPsschPacketBurstQueue.erase(it);
            return (ret);
        }
    }
    return (0);
}

void
NrSlUePhy::DoSetNrSlVarTtiAllocInfo(const SfnSf& sfn, const NrSlVarTtiAllocInfo& varTtiInfo)
{
    NS_LOG_FUNCTION(this);
    // only one allocInfo should exist, which would be the slot allocation
    // info for the current slot.
    if (m_nrSlAllocInfoQueue.empty())
    {
        // new allocation
        NrSlPhySlotAlloc alloc;
        alloc.sfn = sfn;
        // each Var TTI in slvarTtiInfoList list must differ in their symbol start
        bool insertStatus = alloc.slvarTtiInfoList.emplace(varTtiInfo).second;
        NS_ASSERT_MSG(insertStatus,
                      "Insertion failed. A Var TTI starting from symbol " << varTtiInfo.symStart
                                                                          << " already exists");
        m_nrSlAllocInfoQueue.emplace_front(alloc);
    }
    else
    {
        auto it = m_nrSlAllocInfoQueue.begin();
        NS_ASSERT_MSG(it->sfn == sfn,
                      "Allocation queue must contain the allocation info of the same slot");
        // each Var TTI in slvarTtiInfoList list must differ in their symbol start
        bool insertStatus [[maybe_unused]] = it->slvarTtiInfoList.emplace(varTtiInfo).second;
    }
}

bool
NrSlUePhy::DoIsSlHarqFeedbackPending() const
{
    return m_slHarqFbList.size() > 0;
}

bool
NrSlUePhy::NrSlSlotAllocInfoExists(const SfnSf& sfn) const
{
    NS_LOG_FUNCTION(this << sfn);
    if (!m_nrSlAllocInfoQueue.empty())
    {
        return true;
    }
    for (auto it = m_nrSlPsschPacketBurstQueue.begin(); it != m_nrSlPsschPacketBurstQueue.end();
         it++)
    {
        if (it->second->GetNPackets() > 0)
        {
            return true;
        }
    }
    return false;
}

void
NrSlUePhy::StartNrSlSlot(const SfnSf& s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlCurrentAlloc = m_nrSlAllocInfoQueue.front();
    m_nrSlAllocInfoQueue.pop_front();
    NS_ASSERT_MSG(m_nrSlCurrentAlloc.sfn == GetCurrentSfnSf(),
                  "Unable to find NR SL slot allocation");
    NrSlVarTtiAllocInfo varTtiInfo = *(m_nrSlCurrentAlloc.slvarTtiInfoList.begin());
    // erase the retrieved var TTI info
    m_nrSlCurrentAlloc.slvarTtiInfoList.erase(m_nrSlCurrentAlloc.slvarTtiInfoList.begin());
    auto nextVarTtiStart = GetSymbolPeriod() * varTtiInfo.symStart;
    Simulator::Schedule(nextVarTtiStart, &NrSlUePhy::StartNrSlVarTti, this, varTtiInfo);
}

void
NrSlUePhy::StartNrSlVarTti(const NrSlVarTtiAllocInfo& varTtiInfo)
{
    NS_LOG_FUNCTION(this);

    Time varTtiDuration;

    if (varTtiInfo.SlVarTtiType == NrSlVarTtiAllocInfo::CTRL)
    {
        varTtiDuration = SlCtrl(varTtiInfo);
        NS_LOG_DEBUG("CTRL " << varTtiDuration.As(Time::MS));
    }
    else if (varTtiInfo.SlVarTtiType == NrSlVarTtiAllocInfo::DATA)
    {
        varTtiDuration = SlData(varTtiInfo);
        NS_LOG_DEBUG("DATA " << varTtiDuration.As(Time::MS));
    }
    else if (varTtiInfo.SlVarTtiType == NrSlVarTtiAllocInfo::FEEDBACK)
    {
        varTtiDuration = SlFeedback(varTtiInfo);
        NS_LOG_DEBUG("FEEDBACK " << varTtiDuration.As(Time::MS));
    }
    else
    {
        NS_FATAL_ERROR("Invalid or unknown SL VarTti type " << varTtiInfo.SlVarTtiType);
    }

    NS_LOG_DEBUG("Scheduling EndNrSlVarTti at time " << (Now() + varTtiDuration).As(Time::S));
    Simulator::Schedule(varTtiDuration, &NrSlUePhy::EndNrSlVarTti, this, varTtiInfo);
}

void
NrSlUePhy::EndNrSlVarTti(const NrSlVarTtiAllocInfo& varTtiInfo)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("NR SL var TTI started at symbol " << varTtiInfo.symStart << " which lasted for "
                                                    << varTtiInfo.symLength << " symbols");

    if (m_nrSlCurrentAlloc.slvarTtiInfoList.empty())
    {
        // end of slot
        SfnSf currentSlot = GetCurrentSfnSf();
        currentSlot.Add(1);
        SetCurrentSfnSf(currentSlot);
        // we need trigger the NR Slot start
        Simulator::Schedule(GetLastSlotStart() + GetSlotPeriod() - Simulator::Now(),
                            &NrSlUePhy::StartSlot,
                            this,
                            GetCurrentSfnSf());
    }
    else
    {
        NrSlVarTtiAllocInfo nextVarTtiInfo = *(m_nrSlCurrentAlloc.slvarTtiInfoList.begin());
        // erase the retrieved var TTI info
        m_nrSlCurrentAlloc.slvarTtiInfoList.erase(m_nrSlCurrentAlloc.slvarTtiInfoList.begin());
        auto nextVarTtiStart = GetSymbolPeriod() * nextVarTtiInfo.symStart;

        Simulator::Schedule(nextVarTtiStart + GetLastSlotStart() - Simulator::Now(),
                            &NrSlUePhy::StartNrSlVarTti,
                            this,
                            nextVarTtiInfo);
    }
}

Time
NrSlUePhy::SlCtrl(const NrSlVarTtiAllocInfo& varTtiInfo)
{
    NS_LOG_FUNCTION(this);

    Ptr<PacketBurst> pktBurst = PopPscchPacketBurst();
    if (!pktBurst || pktBurst->GetNPackets() == 0)
    {
        NS_FATAL_ERROR("No NR SL CTRL packet to transmit");
    }
    Time varTtiPeriod = GetSymbolPeriod() * varTtiInfo.symLength;
    // -1 ns ensures control ends before data period
    SendNrSlCtrlChannels(pktBurst, varTtiPeriod - NanoSeconds(1.0), varTtiInfo);

    return varTtiPeriod;
}

void
NrSlUePhy::SendNrSlCtrlChannels(const Ptr<PacketBurst>& pb,
                                const Time& varTtiDuration,
                                const NrSlVarTtiAllocInfo& varTtiInfo)
{
    NS_LOG_FUNCTION(this);

    std::vector<int> channelRbs;
    uint32_t lastRbInPlusOne = (varTtiInfo.rbStart + varTtiInfo.rbLength);
    for (uint32_t i = varTtiInfo.rbStart; i < lastRbInPlusOne; i++)
    {
        channelRbs.push_back(static_cast<int>(i));
    }

    SetSubChannelsForTransmission(channelRbs, varTtiInfo.symLength);
    NS_LOG_INFO("Sending PSCCH on SfnSf " << GetCurrentSfnSf());
    auto slSpectrumPhy = m_spectrumPhy->GetObject<NrSlSpectrumPhy>();
    NS_ASSERT_MSG(slSpectrumPhy, "Did not find NrSlSpectrumPhy object");
    slSpectrumPhy->StartTxSlCtrlFrames(pb, varTtiDuration);
}

Time
NrSlUePhy::SlData(const NrSlVarTtiAllocInfo& varTtiInfo)
{
    NS_LOG_FUNCTION(this);

    Time varTtiDuration = GetSymbolPeriod() * varTtiInfo.symLength;
    Ptr<PacketBurst> pktBurst = PopPsschPacketBurst();
    do
    {
        if (pktBurst && pktBurst->GetNPackets() > 0)
        {
            std::list<Ptr<Packet>> pkts = pktBurst->GetPackets();
            NrRadioBearerTag bearerTag;
            if (!pkts.front()->PeekPacketTag(bearerTag))
            {
                NS_FATAL_ERROR("No radio bearer tag");
            }
        }
        else
        {
            // put an error, as something is wrong. The UE should not be scheduled
            // if there is no data for it...
            NS_FATAL_ERROR("The UE " << GetRnti() << " has been scheduled without NR SL data");
        }

        NS_LOG_DEBUG("UE" << GetRnti() << " TXing NR SL DATA frame for symbols "
                          << varTtiInfo.symStart << "-"
                          << varTtiInfo.symStart + varTtiInfo.symLength - 1 << "\t start "
                          << Simulator::Now() << " end "
                          << (Simulator::Now() + varTtiDuration).GetSeconds());
        Simulator::Schedule(NanoSeconds(1.0),
                            &NrSlUePhy::SendNrSlDataChannels,
                            this,
                            pktBurst,
                            varTtiDuration - NanoSeconds(2.0),
                            varTtiInfo);
        pktBurst = PopPsschPacketBurst();
    } while (pktBurst);
    return varTtiDuration;
}

void
NrSlUePhy::SendNrSlDataChannels(const Ptr<PacketBurst>& pb,
                                const Time& varTtiDuration,
                                const NrSlVarTtiAllocInfo& varTtiInfo)
{
    NS_LOG_FUNCTION(this);

    std::vector<int> channelRbs;
    uint32_t lastRbInPlusOne = (varTtiInfo.rbStart + varTtiInfo.rbLength);
    for (uint32_t i = varTtiInfo.rbStart; i < lastRbInPlusOne; i++)
    {
        channelRbs.push_back(static_cast<int>(i));
    }

    SetSubChannelsForTransmission(channelRbs, varTtiInfo.symLength);
    NS_LOG_DEBUG("Sending PSSCH on SfnSf " << GetCurrentSfnSf());
    // Assume Sl Data channel is sent through the first stream
    auto slSpectrumPhy = m_spectrumPhy->GetObject<NrSlSpectrumPhy>();
    NS_ASSERT_MSG(slSpectrumPhy, "Did not find NrSlSpectrumPhy object");
    slSpectrumPhy->StartTxSlDataFrames(pb, varTtiDuration);
}

Time
NrSlUePhy::SlFeedback(const NrSlVarTtiAllocInfo& varTtiInfo)
{
    NS_LOG_FUNCTION(this);

    Time varTtiDuration = GetSymbolPeriod() * varTtiInfo.symLength;

    // Walk the queue and insert all eligible feedback messages to the pktBurst.
    // A message is eligible if the current slot is MinTimeGapPsfch slots or
    // greater than the slot time associated with the feedback in the queue.

    // Note:  Future revisions of this method will need to further filter
    // feedback messages beyond simply whether MinTimeGapPsfch has been
    // exceeded.  For instance, there may be two messages that require the
    // same PSFCH resource (and the higher priority must be selected), or
    // the UE may be expecting feedback on the PSFCH from a prior transmission
    // at higher priority than the feedback queued for sending (in which
    // case the sending of feedback in this slot should be suppressed).

    std::list<Ptr<NrSlHarqFeedbackMessage>> feedbackList;
    uint8_t gap =
        m_slTxPool->GetMinTimeGapPsfch(GetBwpId(), m_nrSlUePhySapUser->GetSlActiveTxPoolId());
    auto it = m_slHarqFbList.begin();
    while (it != m_slHarqFbList.end())
    {
        if (GetCurrentSfnSf().Normalize() >= gap + it->first.Normalize())
        {
            NS_LOG_DEBUG("Inserting HARQ FB to packet burst from slot "
                         << it->first.Normalize() << " for sender RNTI "
                         << it->second->GetSlHarqFeedback().m_txRnti << " dstL2Id "
                         << it->second->GetSlHarqFeedback().m_dstL2Id << " harqProcessId "
                         << +it->second->GetSlHarqFeedback().m_harqProcessId << " bwpIndex "
                         << +it->second->GetSlHarqFeedback().m_bwpIndex << " status "
                         << (it->second->GetSlHarqFeedback().IsReceivedOk() ? "ACK" : "NACK"));
            feedbackList.emplace_front(it->second);
            auto prev = it++;
            m_slHarqFbList.erase(prev);
        }
        else if (GetCurrentSfnSf().Normalize() >= it->first.Normalize())
        {
            NS_LOG_DEBUG("At slot "
                         << GetCurrentSfnSf().Normalize() << "; suppressing (processing delay "
                         << +gap << " slots from " << it->first.Normalize()
                         << ") the insertion of HARQ FB for sender RNTI "
                         << it->second->GetSlHarqFeedback().m_txRnti << " dstL2Id "
                         << it->second->GetSlHarqFeedback().m_dstL2Id << " harqProcessId "
                         << +it->second->GetSlHarqFeedback().m_harqProcessId << " bwpIndex "
                         << +it->second->GetSlHarqFeedback().m_bwpIndex << " status "
                         << (it->second->GetSlHarqFeedback().IsReceivedOk() ? "ACK" : "NACK"));
            ++it;
        }
        else
        {
            ++it;
        }
    }
    // It could be the case that among the eligible HARQ feedback messages to
    // return, we have an earlier NACK that was later overridden by an ACK
    // (possibly due to a blind retransmission).  Deliver only the latest
    // one by iterating the feedback list and using a std::set to
    // check for duplicates.  Because the previous iteration was in reverse,
    // the unique feedback that we want to return will be the first encountered.
    std::list<Ptr<NrSlHarqFeedbackMessage>> uniqueFeedbackList;
    auto it2 = feedbackList.begin();
    std::set<std::pair<uint16_t, uint8_t>> duplicateCheck;
    while (it2 != feedbackList.end())
    {
        uint16_t rnti = (*it2)->GetSlHarqFeedback().m_txRnti;
        uint8_t harqProcessId = (*it2)->GetSlHarqFeedback().m_harqProcessId;
        // If insert() returns false, the (rnti, harqProcessId) already exists
        if (duplicateCheck.insert(std::make_pair(rnti, harqProcessId)).second)
        {
            NS_LOG_DEBUG("Preparing HARQ feedback for sender RNTI " << rnti << " HARQ PID "
                                                                    << +harqProcessId);
            uniqueFeedbackList.emplace_front(*it2);
        }
        ++it2;
    }
    if (!uniqueFeedbackList.empty())
    {
        NS_LOG_DEBUG("UE" << GetRnti() << " TXing NR SL FEEDBACK frame for symbols "
                          << varTtiInfo.symStart << "-"
                          << varTtiInfo.symStart + varTtiInfo.symLength - 1 << "\t start "
                          << Simulator::Now().GetSeconds() << " end "
                          << (Simulator::Now() + varTtiDuration).GetSeconds());

        Simulator::Schedule(NanoSeconds(1.0),
                            &NrSlUePhy::SendNrSlFbChannels,
                            this,
                            uniqueFeedbackList,
                            varTtiDuration - NanoSeconds(2.0),
                            varTtiInfo);
    }
    return varTtiDuration;
}

void
NrSlUePhy::SendNrSlFbChannels(const std::list<Ptr<NrSlHarqFeedbackMessage>>& feedbackList,
                              const Time& varTtiDuration,
                              const NrSlVarTtiAllocInfo& varTtiInfo)
{
    NS_LOG_FUNCTION(this << varTtiDuration);

    std::vector<int> channelRbs;
    uint32_t lastRbInPlusOne = (varTtiInfo.rbStart + varTtiInfo.rbLength);
    for (uint32_t i = varTtiInfo.rbStart; i < lastRbInPlusOne; i++)
    {
        channelRbs.push_back(static_cast<int>(i));
    }

    SetSubChannelsForTransmission(channelRbs, varTtiInfo.symLength);
    NS_LOG_DEBUG("Sending PSFCH on SfnSf " << GetCurrentSfnSf());
    auto slSpectrumPhy = m_spectrumPhy->GetObject<NrSlSpectrumPhy>();
    NS_ASSERT_MSG(slSpectrumPhy, "Did not find NrSlSpectrumPhy object");
    slSpectrumPhy->StartTxSlFeedback(feedbackList, varTtiDuration);
}

void
NrSlUePhy::PhyPscchPduReceived(const Ptr<Packet>& p, const SpectrumValue& psd)
{
    NS_LOG_FUNCTION(this << p);
    NrSlSciF1aHeader sciF1a;
    NrSlMacPduTag tag;

    p->PeekHeader(sciF1a);
    p->PeekPacketTag(tag);

    std::unordered_set<uint32_t> destinations = m_nrSlUePhySapUser->GetSlRxDestinations();

    NS_ASSERT_MSG(m_slRxPool != nullptr, "No receiving pools configured");
    uint16_t sbChSize =
        m_slRxPool->GetNrSlSubChSize(GetBwpId(), m_nrSlUePhySapUser->GetSlActiveTxPoolId());
    uint16_t rbStart = sciF1a.GetIndexStartSubChannel() * sbChSize;
    uint16_t lastRbInPlusOne = (sciF1a.GetLengthSubChannel() * sbChSize) + rbStart;
    NS_LOG_DEBUG("PSCCH PDU received with " << sbChSize << " subchannels and "
                                            << (lastRbInPlusOne - rbStart) << " RBs starting at "
                                            << rbStart);
    std::vector<int> rbBitMap;

    for (uint16_t i = rbStart; i < lastRbInPlusOne; ++i)
    {
        rbBitMap.push_back(i);
    }

    // We use the PSCCH RSRP for sensing
    // TS 38.211 sec 8.4.1.3, 3 RE per RB carries PSCCH DMRS, i.e. Comb 4
    double rsrpDbm = GetSidelinkRsrp(psd, 3).second;

    NS_LOG_DEBUG("Sending sensing data to UE MAC. RSRP "
                 << rsrpDbm << " dBm "
                 << " Frame " << GetCurrentSfnSf().GetFrame() << " SubFrame "
                 << +GetCurrentSfnSf().GetSubframe() << " Slot " << +GetCurrentSfnSf().GetSlot());

    SensingData sensingData(GetCurrentSfnSf(),
                            sciF1a.GetSlResourceReservePeriod(),
                            sciF1a.GetLengthSubChannel(),
                            sciF1a.GetIndexStartSubChannel(),
                            sciF1a.GetPriority(),
                            rsrpDbm,
                            sciF1a.GetGapReTx1(),
                            sciF1a.GetIndexStartSbChReTx1(),
                            sciF1a.GetGapReTx2(),
                            sciF1a.GetIndexStartSbChReTx2());

    m_nrSlUePhySapUser->ReceiveSensingData(sensingData);

    auto it = destinations.find(tag.GetDstL2Id());
    if (it != destinations.end())
    {
        NS_LOG_INFO("Received first stage SCI for destination " << *it << " from RNTI "
                                                                << tag.GetRnti());
        // Assume first stream
        auto slSpectrumPhy = m_spectrumPhy->GetObject<NrSlSpectrumPhy>();
        NS_ASSERT_MSG(slSpectrumPhy, "Did not find NrSlSpectrumPhy object");
        slSpectrumPhy->AddSlExpectedTb({UINT8_MAX,
                                        tag.GetTbSize(),
                                        sciF1a.GetMcs(),
                                        UINT8_MAX,
                                        tag.GetRnti(),
                                        rbBitMap,
                                        UINT8_MAX,
                                        UINT8_MAX,
                                        false,
                                        tag.GetSymStart(),
                                        tag.GetNumSym(),
                                        tag.GetSfn()},
                                       tag.GetDstL2Id());
        SaveFutureResourceAssignments(sciF1a, tag, sbChSize);
    }
    else
    {
        NS_LOG_INFO("Ignoring PSCCH from RNTI " << tag.GetRnti() << "! Destination "
                                                << tag.GetDstL2Id() << " is not monitored by RNTI "
                                                << GetRnti());
    }
}

void
NrSlUePhy::PhyPsfchReceived(uint32_t sendingNodeId, SlHarqInfo harqInfo)
{
    NS_LOG_FUNCTION(this << sendingNodeId);
    Simulator::ScheduleWithContext(m_netDevice->GetNode()->GetId(),
                                   // Add PSFCH decode latency here
                                   // XXX provisional value of 1 slot
                                   GetSlotPeriod(),
                                   &NrSlUePhySapUser::ReceivePsfch,
                                   m_nrSlUePhySapUser,
                                   sendingNodeId,
                                   harqInfo);
}

void
NrSlUePhy::SaveFutureResourceAssignments(const NrSlSciF1aHeader& sciF1a,
                                         const NrSlMacPduTag& tag,
                                         const uint16_t sbChSize)
{
    NS_LOG_FUNCTION(this);
    if (sciF1a.GetGapReTx1() != std::numeric_limits<uint8_t>::max())
    {
        uint16_t rbStart = sciF1a.GetIndexStartSbChReTx1() * sbChSize;
        uint16_t lastRbInPlusOne = (sciF1a.GetLengthSubChannel() * sbChSize) + rbStart;
        std::vector<int> rbBitMap;
        for (uint16_t i = rbStart; i < lastRbInPlusOne; ++i)
        {
            rbBitMap.push_back(i);
        }
        SfnSf futureSfnSf = tag.GetSfn().GetFutureSfnSf(sciF1a.GetGapReTx1());
        NS_ASSERT_MSG(GetCurrentSfnSf() <= futureSfnSf, "Future SfnSf is actually in the past");
        NS_LOG_INFO("Saving expected future TB on SfnSf " << futureSfnSf.Normalize() << " with "
                                                          << +sciF1a.GetLengthSubChannel()
                                                          << " subchannels");
        auto it = m_assignedResources.find(futureSfnSf.Normalize());
        if (it != m_assignedResources.end())
        {
            it->second.emplace_back(tag.GetRnti(),
                                    tag.GetDstL2Id(),
                                    tag.GetTbSize(),
                                    sciF1a.GetMcs(),
                                    rbBitMap,
                                    tag.GetSymStart(),
                                    tag.GetNumSym(),
                                    futureSfnSf);
        }
        else
        {
            std::vector<ResourceAssignmentInfo> v;
            v.emplace_back(tag.GetRnti(),
                           tag.GetDstL2Id(),
                           tag.GetTbSize(),
                           sciF1a.GetMcs(),
                           rbBitMap,
                           tag.GetSymStart(),
                           tag.GetNumSym(),
                           futureSfnSf);
            m_assignedResources.emplace(futureSfnSf.Normalize(), v);
        }
    }
    if (sciF1a.GetGapReTx2() != std::numeric_limits<uint8_t>::max())
    {
        uint16_t rbStart = sciF1a.GetIndexStartSbChReTx2() * sbChSize;
        uint16_t lastRbInPlusOne = (sciF1a.GetLengthSubChannel() * sbChSize) + rbStart;
        std::vector<int> rbBitMap;
        for (uint16_t i = rbStart; i < lastRbInPlusOne; ++i)
        {
            rbBitMap.push_back(i);
        }
        SfnSf futureSfnSf = tag.GetSfn().GetFutureSfnSf(sciF1a.GetGapReTx2());
        NS_ASSERT_MSG(GetCurrentSfnSf() <= futureSfnSf, "Future SfnSf is actually in the past");
        NS_LOG_INFO("Saving expected future TB on SfnSf " << futureSfnSf.Normalize() << " with "
                                                          << +sciF1a.GetLengthSubChannel()
                                                          << " subchannels");
        auto it = m_assignedResources.find(futureSfnSf.Normalize());
        if (it != m_assignedResources.end())
        {
            it->second.emplace_back(tag.GetRnti(),
                                    tag.GetDstL2Id(),
                                    tag.GetTbSize(),
                                    sciF1a.GetMcs(),
                                    rbBitMap,
                                    tag.GetSymStart(),
                                    tag.GetNumSym(),
                                    futureSfnSf);
        }
        else
        {
            std::vector<ResourceAssignmentInfo> v;
            v.emplace_back(tag.GetRnti(),
                           tag.GetDstL2Id(),
                           tag.GetTbSize(),
                           sciF1a.GetMcs(),
                           rbBitMap,
                           tag.GetSymStart(),
                           tag.GetNumSym(),
                           futureSfnSf);
            m_assignedResources.emplace(futureSfnSf.Normalize(), v);
        }
    }
    NS_LOG_DEBUG("Expecting " << m_assignedResources.size()
                              << " future PSSCH slots without SCI 1-A");
}

void
NrSlUePhy::SendSlExpectedTbInfo(const SfnSf& s)
{
    NS_LOG_FUNCTION(this);
    auto it = m_assignedResources.find(s.Normalize());
    if (it != m_assignedResources.end())
    {
        for (auto it2 : it->second)
        {
            auto resourceAssignmentInfo = it2;
            auto slSpectrumPhy = m_spectrumPhy->GetObject<NrSlSpectrumPhy>();
            NS_ASSERT_MSG(slSpectrumPhy, "Did not find NrSlSpectrumPhy object");
            NS_LOG_DEBUG("Sending expected TB info from NrSlUePhy to NrSlSpectrumPhy: "
                         << resourceAssignmentInfo.tbSize << " mcs " << +resourceAssignmentInfo.mcs
                         << " RB size "
                         << std::count(resourceAssignmentInfo.rbBitmap.begin(),
                                       resourceAssignmentInfo.rbBitmap.end(),
                                       1)
                         << " sfn " << resourceAssignmentInfo.sfn.Normalize() << " dstId "
                         << resourceAssignmentInfo.dstId);
            slSpectrumPhy->AddSlExpectedTb({UINT8_MAX,
                                            resourceAssignmentInfo.tbSize,
                                            resourceAssignmentInfo.mcs,
                                            UINT8_MAX,
                                            resourceAssignmentInfo.rnti,
                                            resourceAssignmentInfo.rbBitmap,
                                            UINT8_MAX,
                                            UINT8_MAX,
                                            false,
                                            resourceAssignmentInfo.symStart,
                                            resourceAssignmentInfo.numSym,
                                            resourceAssignmentInfo.sfn},
                                           resourceAssignmentInfo.dstId);
        }
        m_assignedResources.erase(it);
    }
}

void
NrSlUePhy::PhyPsschPduReceived(const Ptr<PacketBurst>& pb, const SpectrumValue& psd)
{
    NS_LOG_FUNCTION(this);
    NrRadioBearerTag tag;
    NrSlSciF2aHeader sciF2a;
    // Separate SCI stage 2 packet from data packets
    std::list<Ptr<Packet>> dataPkts;
    bool foundSci2 = false;
    Ptr<PacketBurst> pdu = pb;
    for (auto p : pdu->GetPackets())
    {
        NrRadioBearerTag tag;
        if (!p->PeekPacketTag(tag))
        {
            // SCI stage 2 is the only packet in the packet burst, which does
            // not have the tag
            p->PeekHeader(sciF2a);
            foundSci2 = true;
        }
        else
        {
            dataPkts.push_back(p);
        }
    }

    NS_ABORT_MSG_IF(foundSci2 == false, "Did not find SCI stage 2 in PSSCH packet burst");
    NS_ASSERT_MSG(!dataPkts.empty(), "Received PHY PDU with not data packets");

    auto srcId = sciF2a.GetSrcId();
    auto dstId = sciF2a.GetDstId();
    for (auto& pktIt : dataPkts)
    {
        uint32_t srcId = sciF2a.GetSrcId();
        Ptr<Packet> packet = pktIt->Copy();
        packet->RemovePacketTag(tag);

        // We use the PSSCH RSRP for SD-RSRP and SL-RSRP meassurement
        double rsrpWatt = GetSidelinkRsrp(psd, 6).first;
        double rsrpDbm = GetSidelinkRsrp(psd, 6).second;

        // We only monitor SD-RSRP for relay discovery messages (LCID = 4)
        if ((tag.GetLcid() == 4))
        {
            // Store SD-RSRP for L1 filtering
            std::map<uint32_t, UeRsrpMeasurementsElement>::iterator itRsrp =
                m_ueSdRsrpMeasurementsMap.find(srcId);
            if (itRsrp == m_ueSdRsrpMeasurementsMap.end())
            {
                NS_LOG_DEBUG("First SD-RSRP measurement entry " << rsrpWatt << " W; " << rsrpDbm
                                                                << " dBm");
                UeRsrpMeasurementsElement elt;
                elt.rsrpSum = rsrpWatt;
                elt.rsrpNum = 1;
                m_ueSdRsrpMeasurementsMap.insert(
                    std::pair<uint32_t, UeRsrpMeasurementsElement>(srcId, elt));
            }
            else
            {
                NS_LOG_DEBUG("SD-RSRP Measurement entry found... Adding values "
                             << rsrpWatt << " W; " << rsrpDbm << "dBm");
                itRsrp->second.rsrpSum += rsrpWatt;
                itRsrp->second.rsrpNum++;
            }
            // Trace SD-RSRP measurement
            m_sdRsrpMeasurementsTrace(GetRnti(), srcId, rsrpDbm);
        }
        else
        {
            // Store SL-RSRP for L1 filtering
            std::map<uint32_t, UeRsrpMeasurementsElement>::iterator itRsrp =
                m_ueSlRsrpMeasurementsMap.find(srcId);
            if (itRsrp == m_ueSlRsrpMeasurementsMap.end())
            {
                NS_LOG_DEBUG("First SL-RSRP measurement entry " << rsrpWatt << " W; " << rsrpDbm
                                                                << " dBm");
                UeRsrpMeasurementsElement elt;
                elt.rsrpSum = rsrpWatt;
                elt.rsrpNum = 1;
                m_ueSlRsrpMeasurementsMap.insert(
                    std::pair<uint32_t, UeRsrpMeasurementsElement>(srcId, elt));
            }
            else
            {
                NS_LOG_DEBUG("SL-RSRP Measurement entry found... Adding values "
                             << rsrpWatt << " W; " << rsrpDbm << "dBm");
                itRsrp->second.rsrpSum += rsrpWatt;
                itRsrp->second.rsrpNum++;
            }
            // Trace SL-RSRP measurement
            m_slRsrpMeasurementsTrace(GetRnti(), srcId, rsrpDbm);
        }
    }

    // It is possible in some cases for the tag-based filtering on PSCCH
    // reception to fail-- there can be two PSCCH received (one to this node,
    // one to another node) leading to two decodable TBs (one to this node,
    // one to another node) in the same slot, and if the one to another node
    // is passed up to the MAC layer here, it will cause an assert there.
    // So, perform layer-1 destination filtering here (note: it has to
    // be done in the PSSCH in practice because SCI format 1-A does not
    // contain destination information, but the ns-3 implementation uses
    // a tag to send this on a side channel to the PSCCH processing above).
    std::unordered_set<uint32_t> destinations = m_nrSlUePhySapUser->GetSlRxDestinations();
    auto it = destinations.find(dstId);
    if (it != destinations.end())
    {
        NS_LOG_INFO("Scheduling ReceivePsschPhyPdu from " << +srcId << " to " << dstId
                                                          << " after decode latency of "
                                                          << GetTbDecodeLatency().As(Time::US));
        Simulator::ScheduleWithContext(m_netDevice->GetNode()->GetId(),
                                       GetTbDecodeLatency(),
                                       &NrSlUePhySapUser::ReceivePsschPhyPdu,
                                       m_nrSlUePhySapUser,
                                       pb);
    }
    else
    {
        NS_LOG_DEBUG("Filtering TB from " << srcId << " to " << dstId
                                          << " that is not being monitored by the upper layers");
    }
}

std::pair<double, double>
NrSlUePhy::GetSidelinkRsrp(SpectrumValue psd, uint32_t nResEle)
{
    // Measure instantaneous S-RSRP...
    double sum = 0.0;
    uint16_t numRB = 0;

    for (Values::const_iterator itPi = psd.ConstValuesBegin(); itPi != psd.ConstValuesEnd(); itPi++)
    {
        if ((*itPi))
        {
            uint32_t scSpacing = 15000 * static_cast<uint32_t>(std::pow(2, GetNumerology()));
            uint32_t RbWidthInHz =
                static_cast<uint32_t>(scSpacing * NrSpectrumValueHelper::SUBCARRIERS_PER_RB);
            double powerTxWattPerRb =
                ((*itPi) * RbWidthInHz); // convert PSD [W/Hz] to linear power [W]
            double powerTxWattPerRe =
                (powerTxWattPerRb /
                 NrSpectrumValueHelper::SUBCARRIERS_PER_RB); // power of one RE per RB
            double PowerTxWattDmrsPerRb = powerTxWattPerRe * nResEle;
            sum += PowerTxWattDmrsPerRb;
            numRB++;
        }
    }

    double avrgRsrpWatt = (sum / ((double)numRB * nResEle));
    double rsrpDbm = 10 * log10(1000 * (avrgRsrpWatt));

    return std::make_pair(avrgRsrpWatt, rsrpDbm);
}

void
NrSlUePhy::DoEnableUeSdRsrpMeasurements()
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_MSG_IF(m_rsrpFilterPeriod.IsZero(),
                    "SL-/SD-RSRP filter period must be non-zero; otherwise will endlessly loop");
    Simulator::Schedule(m_rsrpFilterPeriod, &NrSlUePhy::ReportUeSdRsrpMeasurements, this);
    m_ueSdRsrpMeasurementsEnabled = true;
    // Let the RRC know the L1 measurement period
    m_nrSlUeCphySapUser->SetRsrpFilterPeriod(m_rsrpFilterPeriod);
}

void
NrSlUePhy::DoDisableUeSdRsrpMeasurements()
{
    NS_LOG_FUNCTION(this);
    m_ueSdRsrpMeasurementsEnabled = false;
}

void
NrSlUePhy::DoEnableUeSlRsrpMeasurements()
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_MSG_IF(m_rsrpFilterPeriod.IsZero(),
                    "SL-/SD-RSRP filter period must be non-zero; otherwise will endlessly loop");
    Simulator::Schedule(m_rsrpFilterPeriod, &NrSlUePhy::ReportUeSlRsrpMeasurements, this);
    m_ueSlRsrpMeasurementsEnabled = true;
    // Let the RRC know the L1 measurement period
    m_nrSlUeCphySapUser->SetRsrpFilterPeriod(m_rsrpFilterPeriod);
}

void
NrSlUePhy::DoDisableUeSlRsrpMeasurements()
{
    NS_LOG_FUNCTION(this);
    m_ueSlRsrpMeasurementsEnabled = false;
}

} // namespace ns3
