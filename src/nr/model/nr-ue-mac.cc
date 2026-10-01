// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#define NS_LOG_APPEND_CONTEXT                                                                      \
    do                                                                                             \
    {                                                                                              \
        std::clog << " [ CellId " << GetCellId() << ", bwpId " << GetBwpId() << ", rnti "          \
                  << m_rnti << "] ";                                                               \
    } while (false);

#include "nr-ue-mac.h"

#include "nr-control-messages.h"
#include "nr-mac-header-vs.h"
#include "nr-mac-short-bsr-ce.h"
#include "nr-phy-sap.h"
#include "nr-radio-bearer-tag.h"

#include <ns3/boolean.h>
#include <ns3/log.h>
#include <ns3/random-variable-stream.h>
#include <ns3/uinteger.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrUeMac");
NS_OBJECT_ENSURE_REGISTERED(NrUeMac);

uint8_t NrUeMac::g_raPreambleId = 0;

///////////////////////////////////////////////////////////
// SAP forwarders
///////////////////////////////////////////////////////////

class UeMemberNrUeCmacSapProvider : public NrUeCmacSapProvider
{
  public:
    UeMemberNrUeCmacSapProvider(NrUeMac* mac);

    // inherited from NrUeCmacSapProvider
    void ConfigureRach(RachConfig rc) override;
    void StartContentionBasedRandomAccessProcedure() override;
    void StartNonContentionBasedRandomAccessProcedure(uint16_t rnti,
                                                      uint8_t preambleId,
                                                      uint8_t prachMask) override;
    void AddLc(uint8_t lcId,
               NrUeCmacSapProvider::LogicalChannelConfig lcConfig,
               NrMacSapUser* msu) override;
    void RemoveLc(uint8_t lcId) override;
    void Reset() override;
    void SetRnti(uint16_t rnti) override;
    void NotifyConnectionSuccessful() override;
    void SetImsi(uint64_t imsi) override;

  private:
    NrUeMac* m_mac;
};

UeMemberNrUeCmacSapProvider::UeMemberNrUeCmacSapProvider(NrUeMac* mac)
    : m_mac(mac)
{
}

void
UeMemberNrUeCmacSapProvider::ConfigureRach(RachConfig rc)
{
    m_mac->DoConfigureRach(rc);
}

void
UeMemberNrUeCmacSapProvider::StartContentionBasedRandomAccessProcedure()
{
    m_mac->DoStartContentionBasedRandomAccessProcedure();
}

void
UeMemberNrUeCmacSapProvider::StartNonContentionBasedRandomAccessProcedure(uint16_t rnti,
                                                                          uint8_t preambleId,
                                                                          uint8_t prachMask)
{
    m_mac->DoStartNonContentionBasedRandomAccessProcedure(rnti, preambleId, prachMask);
}

void
UeMemberNrUeCmacSapProvider::AddLc(uint8_t lcId, LogicalChannelConfig lcConfig, NrMacSapUser* msu)
{
    m_mac->AddLc(lcId, lcConfig, msu);
}

void
UeMemberNrUeCmacSapProvider::RemoveLc(uint8_t lcid)
{
    m_mac->DoRemoveLc(lcid);
}

void
UeMemberNrUeCmacSapProvider::Reset()
{
    m_mac->DoReset();
}

void
UeMemberNrUeCmacSapProvider::SetRnti(uint16_t rnti)
{
    m_mac->SetRnti(rnti);
}

void
UeMemberNrUeCmacSapProvider::NotifyConnectionSuccessful()
{
    m_mac->DoNotifyConnectionSuccessful();
}

void
UeMemberNrUeCmacSapProvider::SetImsi(uint64_t imsi)
{
    m_mac->DoSetImsi(imsi);
}

class UeMemberNrMacSapProvider : public NrMacSapProvider
{
  public:
    UeMemberNrMacSapProvider(NrUeMac* mac);

    // inherited from NrMacSapProvider
    void TransmitPdu(TransmitPduParameters params) override;
    void ReportBufferStatus(ReportBufferStatusParameters params) override;

  private:
    NrUeMac* m_mac;
};

UeMemberNrMacSapProvider::UeMemberNrMacSapProvider(NrUeMac* mac)
    : m_mac(mac)
{
}

void
UeMemberNrMacSapProvider::TransmitPdu(TransmitPduParameters params)
{
    m_mac->DoTransmitPdu(params);
}

void
UeMemberNrMacSapProvider::ReportBufferStatus(ReportBufferStatusParameters params)
{
    m_mac->DoReportBufferStatus(params);
}

class NrUePhySapUser;

class MacUeMemberPhySapUser : public NrUePhySapUser
{
  public:
    MacUeMemberPhySapUser(NrUeMac* mac);

    void ReceivePhyPdu(Ptr<Packet> p) override;

    void ReceiveControlMessage(Ptr<NrControlMessage> msg) override;

    void SlotIndication(SfnSf sfn) override;

    // virtual void NotifyHarqDeliveryFailure (uint8_t harqId);

    uint8_t GetNumHarqProcess() const override;

  private:
    NrUeMac* m_mac;
};

MacUeMemberPhySapUser::MacUeMemberPhySapUser(NrUeMac* mac)
    : m_mac(mac)
{
}

void
MacUeMemberPhySapUser::ReceivePhyPdu(Ptr<Packet> p)
{
    m_mac->DoReceivePhyPdu(p);
}

void
MacUeMemberPhySapUser::ReceiveControlMessage(Ptr<NrControlMessage> msg)
{
    m_mac->DoReceiveControlMessage(msg);
}

void
MacUeMemberPhySapUser::SlotIndication(SfnSf sfn)
{
    m_mac->DoSlotIndication(sfn);
}

uint8_t
MacUeMemberPhySapUser::GetNumHarqProcess() const
{
    return m_mac->GetNumHarqProcess();
}

//-----------------------------------------------------------------------

TypeId
NrUeMac::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrUeMac")
            .SetParent<Object>()
            .AddConstructor<NrUeMac>()
            .AddAttribute(
                "NumHarqProcess",
                "Number of concurrent stop-and-wait Hybrid ARQ processes per user",
                UintegerValue(16),
                MakeUintegerAccessor(&NrUeMac::SetNumHarqProcess, &NrUeMac::GetNumHarqProcess),
                MakeUintegerChecker<uint8_t>())
            .AddTraceSource("UeMacRxedCtrlMsgsTrace",
                            "Ue MAC Control Messages Traces.",
                            MakeTraceSourceAccessor(&NrUeMac::m_macRxedCtrlMsgsTrace),
                            "ns3::NrMacRxTrace::RxedUeMacCtrlMsgsTracedCallback")
            .AddTraceSource("UeMacTxedCtrlMsgsTrace",
                            "Ue MAC Control Messages Traces.",
                            MakeTraceSourceAccessor(&NrUeMac::m_macTxedCtrlMsgsTrace),
                            "ns3::NrMacRxTrace::TxedUeMacCtrlMsgsTracedCallback")
            .AddTraceSource("RaResponseTimeout",
                            "Trace fired upon RA response timeout",
                            MakeTraceSourceAccessor(&NrUeMac::m_raResponseTimeoutTrace),
                            "ns3::NrUeMac::RaResponseTimeoutTracedCallback");
    return tid;
}

NrUeMac::NrUeMac()
    : Object()
{
    NS_LOG_FUNCTION(this);
    m_cmacSapProvider = new UeMemberNrUeCmacSapProvider(this);
    m_macSapProvider = new UeMemberNrMacSapProvider(this);
    m_phySapUser = new MacUeMemberPhySapUser(this);
    m_raPreambleUniformVariable = CreateObject<UniformRandomVariable>();
}

NrUeMac::~NrUeMac()
{
}

void
NrUeMac::DoDispose()
{
    m_miUlHarqProcessesPacket.clear();
    m_miUlHarqProcessesPacketTimer.clear();
    m_ulBsrReceived.clear();
    m_lcInfoMap.clear();
    m_raPreambleUniformVariable = nullptr;
    delete m_macSapProvider;
    delete m_cmacSapProvider;
    delete m_phySapUser;
}

void
NrUeMac::SetRnti(uint16_t rnti)
{
    NS_LOG_FUNCTION(this);
    m_rnti = rnti;
}

void
NrUeMac::DoNotifyConnectionSuccessful()
{
    NS_LOG_FUNCTION(this);
    m_phySapProvider->NotifyConnectionSuccessful();
}

void
NrUeMac::DoSetImsi(uint64_t imsi)
{
    NS_LOG_FUNCTION(this);
    m_imsi = imsi;
}

uint16_t
NrUeMac::GetBwpId() const
{
    if (m_phySapProvider)
    {
        return m_phySapProvider->GetBwpId();
    }
    else
    {
        return UINT16_MAX;
    }
}

uint16_t
NrUeMac::GetCellId() const
{
    if (m_phySapProvider)
    {
        return m_phySapProvider->GetCellId();
    }
    else
    {
        return UINT16_MAX;
    }
}

uint16_t
NrUeMac::GetRnti() const
{
    return m_rnti;
}

uint64_t
NrUeMac::GetImsi() const
{
    return m_imsi;
}

void
NrUeMac::SetCurrentSlot(const SfnSf& sfn)
{
    m_currentSlot = sfn;
}

uint32_t
NrUeMac::GetTotalBufSize() const
{
    uint32_t ret = 0;
    for (const auto& it : m_ulBsrReceived)
    {
        ret += (it.second.txQueueSize + it.second.retxQueueSize + it.second.statusPduSize);
    }
    return ret;
}

/**
 * \brief Sets the number of HARQ processes
 * \param numHarqProcesses the maximum number of harq processes
 */
void
NrUeMac::SetNumHarqProcess(uint8_t numHarqProcess)
{
    m_numHarqProcess = numHarqProcess;

    m_miUlHarqProcessesPacket.resize(GetNumHarqProcess());
    for (auto& i : m_miUlHarqProcessesPacket)
    {
        if (i.m_pktBurst == nullptr)
        {
            Ptr<PacketBurst> pb = CreateObject<PacketBurst>();
            i.m_pktBurst = pb;
        }
    }
    m_miUlHarqProcessesPacketTimer.resize(GetNumHarqProcess(), 0);
}

/**
 * \return number of HARQ processes
 */
uint8_t
NrUeMac::GetNumHarqProcess() const
{
    return m_numHarqProcess;
}

// forwarded from MAC SAP
void
NrUeMac::DoTransmitPdu(NrMacSapProvider::TransmitPduParameters params)
{
    NS_LOG_FUNCTION(this);
    if (m_ulDci == nullptr)
    {
        return;
    }
    NS_ASSERT(m_ulDci);
    NS_ASSERT(m_ulDci->m_harqProcess == params.harqProcessId);

    m_miUlHarqProcessesPacket.at(params.harqProcessId).m_lcidList.push_back(params.lcid);

    NrMacHeaderVs header;
    header.SetLcId(params.lcid);
    header.SetSize(params.pdu->GetSize());

    params.pdu->AddHeader(header);

    NrRadioBearerTag bearerTag(params.rnti, params.lcid, 0);
    params.pdu->AddPacketTag(bearerTag);

    if (!m_miUlHarqProcessesPacket.at(params.harqProcessId).m_pktBurst)
    {
        m_miUlHarqProcessesPacket.at(params.harqProcessId).m_pktBurst = CreateObject<PacketBurst>();
    }
    m_miUlHarqProcessesPacket.at(params.harqProcessId).m_pktBurst->AddPacket(params.pdu);
    m_miUlHarqProcessesPacketTimer.at(params.harqProcessId) = GetNumHarqProcess();

    m_ulDciTotalUsed += params.pdu->GetSize();

    NS_ASSERT_MSG(m_ulDciTotalUsed <= m_ulDci->m_tbSize,
                  "We used more data than the DCI allowed us.");

    m_phySapProvider->SendMacPdu(params.pdu, m_ulDciSfnsf, m_ulDci->m_symStart, m_ulDci->m_rnti);
}

void
NrUeMac::DoReportBufferStatus(NrMacSapProvider::ReportBufferStatusParameters params)
{
    NS_LOG_FUNCTION(this << static_cast<uint32_t>(params.lcid));

    auto it = m_ulBsrReceived.find(params.lcid);

    NS_LOG_INFO("Received BSR for LC Id" << static_cast<uint32_t>(params.lcid));

    if (it != m_ulBsrReceived.end())
    {
        // update entry
        (*it).second = params;
    }
    else
    {
        it = m_ulBsrReceived.insert(std::make_pair(params.lcid, params)).first;
    }

    if (m_srState == INACTIVE || (params.expRbsTimer && m_srState == ACTIVE))
    {
        NS_LOG_INFO("INACTIVE -> TO_SEND, bufSize " << GetTotalBufSize());
        m_srState = TO_SEND;
    }
}

void
NrUeMac::SendReportBufferStatus(const SfnSf& dataSfn, uint8_t symStart)
{
    NS_LOG_FUNCTION(this);

    if (m_rnti == 0)
    {
        NS_LOG_INFO("MAC not initialized, BSR deferred");
        return;
    }

    if (m_ulBsrReceived.empty())
    {
        NS_LOG_INFO("No BSR report to transmit");
        return;
    }
    MacCeElement bsr = MacCeElement();
    bsr.m_rnti = m_rnti;
    bsr.m_macCeType = MacCeElement::BSR;

    // BSR is reported for each LCG
    std::unordered_map<uint8_t, NrMacSapProvider::ReportBufferStatusParameters>::iterator it;
    std::vector<uint32_t> queue(4, 0); // one value per each of the 4 LCGs, initialized to 0
    for (it = m_ulBsrReceived.begin(); it != m_ulBsrReceived.end(); it++)
    {
        uint8_t lcid = it->first;
        auto lcInfoMapIt = m_lcInfoMap.find(lcid);
        NS_ASSERT(lcInfoMapIt != m_lcInfoMap.end());
        NS_ASSERT_MSG((lcid != 0) ||
                          (((*it).second.txQueueSize == 0) && ((*it).second.retxQueueSize == 0) &&
                           ((*it).second.statusPduSize == 0)),
                      "BSR should not be used for LCID 0");
        uint8_t lcg = lcInfoMapIt->second.lcConfig.logicalChannelGroup;
        queue.at(lcg) +=
            ((*it).second.txQueueSize + (*it).second.retxQueueSize + (*it).second.statusPduSize);

        if (queue.at(lcg) != 0)
        {
            NS_LOG_DEBUG("Adding 5 bytes for SHORT_BSR.");
            queue.at(lcg) += 5;
        }
        if ((*it).second.txQueueSize > 0)
        {
            NS_LOG_DEBUG("Adding 3 bytes for TX subheader.");
            queue.at(lcg) += 3;
        }
        if ((*it).second.retxQueueSize > 0)
        {
            NS_LOG_DEBUG("Adding 3 bytes for RX subheader.");
            queue.at(lcg) += 3;
        }
    }

    NS_LOG_INFO("Sending BSR with this info for the LCG: "
                << queue.at(0) << " " << queue.at(1) << " " << queue.at(2) << " " << queue.at(3));
    // FF API says that all 4 LCGs are always present
    bsr.m_macCeValue.m_bufferStatus.push_back(NrMacShortBsrCe::FromBytesToLevel(queue.at(0)));
    bsr.m_macCeValue.m_bufferStatus.push_back(NrMacShortBsrCe::FromBytesToLevel(queue.at(1)));
    bsr.m_macCeValue.m_bufferStatus.push_back(NrMacShortBsrCe::FromBytesToLevel(queue.at(2)));
    bsr.m_macCeValue.m_bufferStatus.push_back(NrMacShortBsrCe::FromBytesToLevel(queue.at(3)));

    // create the message. It is used only for tracing, but we don't send it...
    Ptr<NrBsrMessage> msg = Create<NrBsrMessage>();
    msg->SetSourceBwp(GetBwpId());
    msg->SetBsr(bsr);

    m_macTxedCtrlMsgsTrace(m_currentSlot, GetCellId(), bsr.m_rnti, GetBwpId(), msg);

    // Here we send the real SHORT_BSR, as a subpdu.
    Ptr<Packet> p = Create<Packet>();

    // Please note that the levels are defined from the standard. In this case,
    // we have 5 bit available, so use such standard levels. In the future,
    // when LONG BSR will be implemented, this have to change.
    NrMacShortBsrCe header;
    header.m_bufferSizeLevel_0 = NrMacShortBsrCe::FromBytesToLevel(queue.at(0));
    header.m_bufferSizeLevel_1 = NrMacShortBsrCe::FromBytesToLevel(queue.at(1));
    header.m_bufferSizeLevel_2 = NrMacShortBsrCe::FromBytesToLevel(queue.at(2));
    header.m_bufferSizeLevel_3 = NrMacShortBsrCe::FromBytesToLevel(queue.at(3));

    p->AddHeader(header);

    NrRadioBearerTag bearerTag(m_rnti, NrMacHeaderFsUl::SHORT_BSR, 0);
    p->AddPacketTag(bearerTag);

    m_ulDciTotalUsed += p->GetSize();
    NS_ASSERT_MSG(m_ulDciTotalUsed <= m_ulDci->m_tbSize,
                  "We used more data than the DCI allowed us.");

    m_phySapProvider->SendMacPdu(p, dataSfn, symStart, m_ulDci->m_rnti);
}

void
NrUeMac::SetUeCmacSapUser(NrUeCmacSapUser* s)
{
    m_cmacSapUser = s;
}

NrUeCmacSapProvider*
NrUeMac::GetUeCmacSapProvider()
{
    return m_cmacSapProvider;
}

void
NrUeMac::RefreshHarqProcessesPacketBuffer()
{
    NS_LOG_FUNCTION(this);

    for (std::size_t i = 0; i < m_miUlHarqProcessesPacketTimer.size(); i++)
    {
        if (m_miUlHarqProcessesPacketTimer.at(i) == 0 && m_miUlHarqProcessesPacket.at(i).m_pktBurst)
        {
            if (m_miUlHarqProcessesPacket.at(i).m_pktBurst->GetSize() > 0)
            {
                // timer expired: drop packets in buffer for this process
                NS_LOG_INFO("HARQ Proc Id " << i << " packets buffer expired");
                Ptr<PacketBurst> emptyPb = CreateObject<PacketBurst>();
                m_miUlHarqProcessesPacket.at(i).m_pktBurst = emptyPb;
                m_miUlHarqProcessesPacket.at(i).m_lcidList.clear();
            }
        }
        else
        {
            // m_miUlHarqProcessesPacketTimer.at (i)--;  // ignore HARQ timeout
        }
    }
}

void
NrUeMac::DoSlotIndication(const SfnSf& sfn)
{
    NS_LOG_FUNCTION(this);
    m_currentSlot = sfn;
    NS_LOG_INFO("Slot " << m_currentSlot);

    RefreshHarqProcessesPacketBuffer();

    if (m_srState == TO_SEND)
    {
        NS_LOG_INFO("Sending SR to PHY in slot " << sfn);
        SendSR();
        m_srState = ACTIVE;
    }

    // Feedback missing
}

void
NrUeMac::SendSR() const
{
    NS_LOG_FUNCTION(this);

    if (m_rnti == 0)
    {
        NS_LOG_INFO("MAC not initialized, SR deferred");
        return;
    }

    // create the SR to send to the gNB
    Ptr<NrSRMessage> msg = Create<NrSRMessage>();
    msg->SetSourceBwp(GetBwpId());
    msg->SetRNTI(m_rnti);

    m_macTxedCtrlMsgsTrace(m_currentSlot, GetCellId(), m_rnti, GetBwpId(), msg);
    m_phySapProvider->SendControlMessage(msg);
}

void
NrUeMac::DoReceivePhyPdu(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this);

    NrRadioBearerTag tag;
    p->RemovePacketTag(tag);

    if (tag.GetRnti() != m_rnti) // Packet is for another user
    {
        return;
    }

    NrMacHeaderVs header;
    p->RemoveHeader(header);

    NrMacSapUser::ReceivePduParameters rxParams;
    rxParams.p = p;
    rxParams.rnti = m_rnti;
    rxParams.lcid = header.GetLcId();

    auto it = m_lcInfoMap.find(header.GetLcId());
    // Ignore non-existing lcids
    if (it == m_lcInfoMap.end())
    {
        return;
    }

    // p can be empty. Well, right now no, but when someone will add CE in downlink,
    // then p can be empty.
    if (rxParams.p->GetSize() > 0)
    {
        it->second.macSapUser->ReceivePdu(rxParams);
    }
}

void
NrUeMac::RecvRaResponse(NrBuildRarListElement_s raResponse)
{
    NS_LOG_FUNCTION(this);
    m_waitingForRaResponse = false;
    m_noRaResponseReceivedEvent.Cancel();
    NS_LOG_INFO(" IMSI " << m_imsi << " RNTI " << m_rnti << " received RAR for RA preamble ID "
                         << +m_raPreambleId
                         << ", setting T-C-RNTI = " << raResponse.ulMsg3Dci->m_rnti
                         << " at: " << Simulator::Now().As(Time::MS));
    m_rnti = raResponse.ulMsg3Dci->m_rnti;
    m_cmacSapUser->SetTemporaryCellRnti(m_rnti);
    // in principle we should wait for contention resolution,
    // but in the current NR model when two or more identical
    // preambles are sent no one is received, so there is no need
    // for contention resolution
    m_cmacSapUser->NotifyRandomAccessSuccessful();
    // Trigger Tx opportunity for Message 3 over LC 0
    // this is needed since Message 3's UL GRANT is in the RAR, not in UL-DCIs
    const uint8_t lc0Lcid = 0;
    auto lc0InfoIt = m_lcInfoMap.find(lc0Lcid);
    NS_ASSERT_MSG(lc0InfoIt != m_lcInfoMap.end(),
                  "LC0 not mapped to this UE MAC with bwpId:" << GetBwpId());
    auto lc0BsrIt = m_ulBsrReceived.find(lc0Lcid);
    if ((lc0BsrIt != m_ulBsrReceived.end()) && (lc0BsrIt->second.txQueueSize > 0))
    {
        NS_LOG_INFO(
            "Notify RLC about transmission opportunity for sending RRC CONNECTION REQUEST.");
        NS_ASSERT_MSG(raResponse.ulMsg3Dci->m_tbSize > lc0BsrIt->second.txQueueSize,
                      "segmentation of Message 3 is not allowed");
        NrMacSapUser::TxOpportunityParameters txOpParams;
        txOpParams.lcid = lc0Lcid;
        txOpParams.rnti = m_rnti;
        txOpParams.bytes = raResponse.ulMsg3Dci->m_tbSize;
        txOpParams.layer = 0;
        txOpParams.harqId = 0;
        txOpParams.componentCarrierId = GetBwpId();

        lc0InfoIt->second.macSapUser->NotifyTxOpportunity(txOpParams);
        lc0BsrIt->second.txQueueSize = 0;
        lc0BsrIt->second.retxQueueSize = 0;
        lc0BsrIt->second.statusPduSize = 0;
        m_ulBsrReceived.erase(lc0BsrIt);
    }
}

void
NrUeMac::ProcessUlDci(const Ptr<NrUlDciMessage>& dciMsg)
{
    NS_LOG_FUNCTION(this);

    SfnSf dataSfn = m_currentSlot;
    dataSfn.Add(dciMsg->GetKDelay());

    // Saving the data we need in DoTransmitPdu
    m_ulDciSfnsf = dataSfn;
    m_ulDciTotalUsed = 0;
    m_ulDci = dciMsg->GetDciInfoElement();

    m_macRxedCtrlMsgsTrace(m_currentSlot, GetCellId(), m_rnti, GetBwpId(), dciMsg);

    NS_LOG_INFO("UL DCI received, transmit data in slot "
                << dataSfn << " Harq Process " << +m_ulDci->m_harqProcess << " TBS "
                << m_ulDci->m_tbSize << " total queue " << GetTotalBufSize());

    if (m_ulDci->m_ndi == 0)
    {
        // This method will retransmit the data saved in the harq buffer
        TransmitRetx();

        // This method will transmit a new BSR.
        SendReportBufferStatus(dataSfn, m_ulDci->m_symStart);
    }
    else if (m_ulDci->m_ndi == 1)
    {
        SendNewData();

        NS_LOG_INFO("After sending NewData, bufSize " << GetTotalBufSize());

        // Send a new BSR. SendNewData() already took into account the size of
        // the BSR.
        SendReportBufferStatus(dataSfn, m_ulDci->m_symStart);

        NS_LOG_INFO("UL DCI processing done, sent to PHY a total of "
                    << m_ulDciTotalUsed << " B out of " << m_ulDci->m_tbSize
                    << " allocated bytes ");

        if (GetTotalBufSize() == 0)
        {
            m_srState = INACTIVE;
            NS_LOG_INFO("ACTIVE -> INACTIVE, bufSize " << GetTotalBufSize());

            // the UE may have been scheduled, but we didn't use a single byte
            // of the allocation. So send an empty PDU. This happens because the
            // byte reporting in the BSR is not accurate, due to RLC and/or
            // BSR quantization.
            if (m_ulDciTotalUsed == 0)
            {
                NS_LOG_WARN("No byte used for this UL-DCI, sending empty PDU");

                NrMacSapProvider::TransmitPduParameters txParams;

                txParams.pdu = Create<Packet>();
                txParams.lcid = 3;
                txParams.rnti = m_rnti;
                txParams.layer = 0;
                txParams.harqProcessId = m_ulDci->m_harqProcess;
                txParams.componentCarrierId = GetBwpId();

                DoTransmitPdu(txParams);
            }
        }
    }
}

void
NrUeMac::TransmitRetx()
{
    NS_LOG_FUNCTION(this);

    Ptr<PacketBurst> pb = m_miUlHarqProcessesPacket.at(m_ulDci->m_harqProcess).m_pktBurst;

    if (pb == nullptr)
    {
        NS_LOG_WARN(
            "The previous transmission did not contain any new data; "
            "probably it was BSR only. To not send an old BSR to the scheduler, "
            "we don't send anything back in this allocation. Eventually, "
            "the Harq timer at gnb will expire, and soon this allocation will be forgotten.");
        return;
    }

    NS_LOG_DEBUG("UE MAC RETX HARQ " << +m_ulDci->m_harqProcess);

    NS_ASSERT(pb->GetNPackets() > 0);

    for (std::list<Ptr<Packet>>::const_iterator j = pb->Begin(); j != pb->End(); ++j)
    {
        Ptr<Packet> pkt = (*j)->Copy();
        NrRadioBearerTag bearerTag;
        if (!pkt->PeekPacketTag(bearerTag))
        {
            NS_FATAL_ERROR("No radio bearer tag");
        }
        m_phySapProvider->SendMacPdu(pkt, m_ulDciSfnsf, m_ulDci->m_symStart, m_ulDci->m_rnti);
    }

    m_miUlHarqProcessesPacketTimer.at(m_ulDci->m_harqProcess) = GetNumHarqProcess();
}

void
NrUeMac::SendRetxData(uint32_t usefulTbs, uint32_t activeLcsRetx)
{
    NS_LOG_FUNCTION(this);

    if (activeLcsRetx == 0)
    {
        return;
    }

    uint32_t bytesPerLcId = usefulTbs / activeLcsRetx;

    for (auto& itBsr : m_ulBsrReceived)
    {
        auto& bsr = itBsr.second;

        if (m_ulDciTotalUsed + bytesPerLcId <= usefulTbs)
        {
            NrMacSapUser::TxOpportunityParameters txParams;
            txParams.lcid = bsr.lcid;
            txParams.rnti = m_rnti;
            txParams.bytes = bytesPerLcId;
            txParams.layer = 0;
            txParams.harqId = m_ulDci->m_harqProcess;
            txParams.componentCarrierId = GetBwpId();

            NS_LOG_INFO("Notifying RLC of LCID " << +bsr.lcid
                                                 << " of a TxOpp "
                                                    "of "
                                                 << bytesPerLcId << " B for a RETX PDU");

            m_lcInfoMap.at(bsr.lcid).macSapUser->NotifyTxOpportunity(txParams);
            // After this call, m_ulDciTotalUsed has been updated with the
            // correct amount of bytes... but it is up to us in updating the BSR
            // value, subtracting the amount of bytes transmitted

            // We need to use std::min here because bytesPerLcId can be
            // greater than bsr.txQueueSize because scheduler can assign
            // more bytes than needed due to how TB size is computed.
            bsr.retxQueueSize -= std::min(bytesPerLcId, bsr.retxQueueSize);
        }
        else
        {
            NS_LOG_DEBUG("Something wrong with the calculation of overhead."
                         "Active LCS Retx: "
                         << activeLcsRetx << " assigned to this: " << bytesPerLcId
                         << ", with TBS of " << m_ulDci->m_tbSize << " usefulTbs " << usefulTbs
                         << " and total used " << m_ulDciTotalUsed);
        }
    }
}

void
NrUeMac::SendTxData(uint32_t usefulTbs, uint32_t activeTx)
{
    NS_LOG_FUNCTION(this);

    if (activeTx == 0)
    {
        return;
    }

    uint32_t bytesPerLcId = usefulTbs / activeTx;

    for (auto& itBsr : m_ulBsrReceived)
    {
        auto& bsr = itBsr.second;

        if (m_ulDciTotalUsed + bytesPerLcId <= usefulTbs)
        {
            NrMacSapUser::TxOpportunityParameters txParams;
            txParams.lcid = bsr.lcid;
            txParams.rnti = m_rnti;
            txParams.bytes = bytesPerLcId;
            txParams.layer = 0;
            txParams.harqId = m_ulDci->m_harqProcess;
            txParams.componentCarrierId = GetBwpId();

            NS_LOG_INFO("Notifying RLC of LCID " << +bsr.lcid
                                                 << " of a TxOpp "
                                                    "of "
                                                 << bytesPerLcId << " B for a TX PDU");

            m_lcInfoMap.at(bsr.lcid).macSapUser->NotifyTxOpportunity(txParams);
            // After this call, m_ulDciTotalUsed has been updated with the
            // correct amount of bytes... but it is up to us in updating the BSR
            // value, subtracting the amount of bytes transmitted

            // We need to use std::min here because bytesPerLcId can be
            // greater than bsr.txQueueSize because scheduler can assign
            // more bytes than needed due to how TB size is computed.
            bsr.txQueueSize -= std::min(bytesPerLcId, bsr.txQueueSize);
        }
        else
        {
            NS_LOG_DEBUG("Something wrong with the calculation of overhead."
                         "Active LCS TX: "
                         << activeTx << " assigned to this: " << bytesPerLcId << ", with TBS of "
                         << m_ulDci->m_tbSize << " usefulTbs " << usefulTbs << " and total used "
                         << m_ulDciTotalUsed);
        }
    }
}

void
NrUeMac::SendNewData()
{
    NS_LOG_FUNCTION(this);
    // New transmission -> empty pkt buffer queue (for deleting eventual pkts not acked )
    Ptr<PacketBurst> pb = CreateObject<PacketBurst>();
    m_miUlHarqProcessesPacket.at(m_ulDci->m_harqProcess).m_pktBurst = pb;
    m_miUlHarqProcessesPacket.at(m_ulDci->m_harqProcess).m_lcidList.clear();
    NS_LOG_INFO("Reset HARQP " << +m_ulDci->m_harqProcess);

    // Sending the status data has no boundary: let's try to send the ACK as
    // soon as possible, filling the TBS, if necessary.
    SendNewStatusData();

    // Let's count how many LC we have, that are waiting with some data
    uint16_t activeLcsRetx = 0;
    uint16_t activeLcsTx = 0;
    for (const auto& itBsr : m_ulBsrReceived)
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
    NS_ASSERT_MSG(m_ulDciTotalUsed + 5 <= m_ulDci->m_tbSize,
                  "The StatusPDU used " << m_ulDciTotalUsed
                                        << " B, we don't have any for the SHORT_BSR.");
    // reserve some data for the SHORT_BSR
    uint32_t usefulTbs = m_ulDci->m_tbSize - m_ulDciTotalUsed - 5;

    // Now, we have 3 bytes of overhead for each subPDU. Let's try to serve all
    // the queues with some RETX data.
    if (activeLcsRetx == 0 && activeLcsTx == 0 && usefulTbs > 0)
    {
        NS_LOG_LOGIC("This UE tx opportunity will be wasted: " << usefulTbs << " bytes.");
    }

    // this check is needed, because if there are no active LCS we should not
    // enter into else and call the function SendRetxData
    if (activeLcsRetx > 0 && usefulTbs > 0) // the queues with some RETX data.
    {
        // 10 because 3 bytes will go for MAC subheader
        // and we should ensure to pass to RLC AM at least 7 bytes
        if (activeLcsRetx * 10 > usefulTbs)
        {
            NS_LOG_DEBUG("The overhead for transmitting retx data is greater than the space for "
                         "transmitting it."
                         "Ignore the TBS of "
                         << usefulTbs << " B.");
        }
        else
        {
            usefulTbs -= activeLcsRetx * 3;
            SendRetxData(usefulTbs, activeLcsRetx);
        }
    }

    // Now we have to update our useful TBS for the next transmission.
    // Remember that m_ulDciTotalUsed keep count of data and overhead that we
    // used till now.
    NS_ASSERT_MSG(
        m_ulDciTotalUsed + 5 <= m_ulDci->m_tbSize,
        "The StatusPDU and RETX sending required all space, we don't have any for the SHORT_BSR.");
    usefulTbs = m_ulDci->m_tbSize - m_ulDciTotalUsed - 5; // Update the usefulTbs.

    // The last part is for the queues with some non-RETX data. If there is no space left,
    // then nothing.
    if (activeLcsTx > 0 && usefulTbs > 0) // the queues with some TX data.
    {
        // 10 because 3 bytes will go for MAC subheader
        // and we should ensure to pass to RLC AM at least 7 bytes
        if (activeLcsTx * 10 > usefulTbs)
        {
            NS_LOG_DEBUG("The overhead for transmitting new data is greater than the space for "
                         "transmitting it."
                         "Ignore the TBS of "
                         << usefulTbs << " B.");
        }
        else
        {
            usefulTbs -= activeLcsTx * 3;
            SendTxData(usefulTbs, activeLcsTx);
        }
    }

    // If we did not used the packet burst, explicitly signal it to the HARQ
    // retx, if any.
    if (m_ulDciTotalUsed == 0)
    {
        m_miUlHarqProcessesPacket.at(m_ulDci->m_harqProcess).m_pktBurst = nullptr;
        m_miUlHarqProcessesPacket.at(m_ulDci->m_harqProcess).m_lcidList.clear();
    }
}

void
NrUeMac::SendNewStatusData()
{
    NS_LOG_FUNCTION(this);

    bool hasStatusPdu = false;
    bool sentOneStatusPdu = false;

    for (auto& bsrIt : m_ulBsrReceived)
    {
        auto& bsr = bsrIt.second;

        if (bsr.statusPduSize > 0)
        {
            hasStatusPdu = true;

            // Check if we have room to transmit the statusPdu
            if (m_ulDciTotalUsed + bsr.statusPduSize <= m_ulDci->m_tbSize)
            {
                NrMacSapUser::TxOpportunityParameters txParams;
                txParams.lcid = bsr.lcid;
                txParams.rnti = m_rnti;
                txParams.bytes = bsr.statusPduSize;
                txParams.layer = 0;
                txParams.harqId = m_ulDci->m_harqProcess;
                txParams.componentCarrierId = GetBwpId();

                NS_LOG_INFO("Notifying RLC of LCID " << +bsr.lcid
                                                     << " of a TxOpp "
                                                        "of "
                                                     << bsr.statusPduSize << " B for a status PDU");

                m_lcInfoMap.at(bsr.lcid).macSapUser->NotifyTxOpportunity(txParams);
                // After this call, m_ulDciTotalUsed has been updated with the
                // correct amount of bytes... but it is up to us in updating the BSR
                // value, subtracting the amount of bytes transmitted
                bsr.statusPduSize = 0;
                sentOneStatusPdu = true;
            }
            else
            {
                NS_LOG_INFO("Cannot send StatusPdu of " << bsr.statusPduSize
                                                        << " B, we already used all the TBS");
            }
        }
    }

    NS_ABORT_MSG_IF(hasStatusPdu && !sentOneStatusPdu,
                    "The TBS of size " << m_ulDci->m_tbSize
                                       << " doesn't allow us "
                                          "to send one status PDU...");
}

void
NrUeMac::DoReceiveControlMessage(Ptr<NrControlMessage> msg)
{
    NS_LOG_FUNCTION(this << msg);

    switch (msg->GetMessageType())
    {
    case (NrControlMessage::UL_DCI): {
        ProcessUlDci(DynamicCast<NrUlDciMessage>(msg));
        break;
    }
    case (NrControlMessage::RAR): {
        NS_LOG_INFO("Received RAR in slot " << m_currentSlot);

        m_macRxedCtrlMsgsTrace(m_currentSlot, GetCellId(), m_rnti, GetBwpId(), msg);

        if (m_waitingForRaResponse)
        {
            Ptr<NrRarMessage> rarMsg = DynamicCast<NrRarMessage>(msg);
            NS_LOG_LOGIC("got RAR with RA-RNTI " << +rarMsg->GetRaRnti() << ", expecting "
                                                 << +m_raRnti);
            for (auto it = rarMsg->RarListBegin(); it != rarMsg->RarListEnd(); ++it)
            {
                if (it->rarPayload.raPreambleId == m_raPreambleId)
                {
                    RecvRaResponse(it->rarPayload);
                }
            }
        }
        break;
    }

    default:
        NS_LOG_LOGIC("Control message not supported/expected");
    }
}

NrUePhySapUser*
NrUeMac::GetPhySapUser()
{
    return m_phySapUser;
}

void
NrUeMac::SetPhySapProvider(NrPhySapProvider* ptr)
{
    m_phySapProvider = ptr;
}

void
NrUeMac::RaResponseTimeout(bool contention)
{
    NS_LOG_FUNCTION(this << contention);
    m_waitingForRaResponse = false;
    // 3GPP 36.321 5.1.4
    ++m_preambleTransmissionCounter;
    // fire RA response timeout trace
    m_raResponseTimeoutTrace(m_imsi,
                             contention,
                             m_preambleTransmissionCounter,
                             m_rachConfig.preambleTransMax + 1);
    if (m_preambleTransmissionCounter == m_rachConfig.preambleTransMax + 1)
    {
        NS_LOG_INFO("RAR timeout, preambleTransMax reached => giving up");
        m_cmacSapUser->NotifyRandomAccessFailed();
    }
    else
    {
        NS_LOG_INFO("RAR timeout while waiting for raPreambleId " << +m_raPreambleId
                                                                  << " re-send preamble");
        if (contention)
        {
            RandomlySelectAndSendRaPreamble();
        }
        else
        {
            SendRaPreamble(contention);
        }
    }
}

void
NrUeMac::DoConfigureRach(NrUeCmacSapProvider::RachConfig rc)
{
    NS_LOG_FUNCTION(this);
    m_rachConfig = rc;
    m_rachConfigured = true;
}

void
NrUeMac::DoStartContentionBasedRandomAccessProcedure()
{
    NS_LOG_FUNCTION(this);
    RandomlySelectAndSendRaPreamble();
}

void
NrUeMac::RandomlySelectAndSendRaPreamble()
{
    NS_LOG_FUNCTION(this);
    // 3GPP 36.321 5.1.1
    NS_ASSERT_MSG(m_rachConfigured, "RACH not configured");
    // assume that there is no Random Access Preambles group B
    m_raPreambleId =
        m_raPreambleUniformVariable->GetInteger(0, m_rachConfig.numberOfRaPreambles - 1);
    NS_LOG_DEBUG(m_currentSlot << " Received System Information, send to PHY the "
                                  "RA preamble: "
                               << m_raPreambleId);
    SendRaPreamble(true);
}

void
NrUeMac::SendRaPreamble(bool contention)
{
    NS_LOG_FUNCTION(this << (uint32_t)m_raPreambleId << contention);

    if (contention)
    {
        // m_raPreambleId = m_raPreambleUniformVariable->GetInteger (0, 64 - 1);
        m_raPreambleId = g_raPreambleId++;
        bool preambleOverflow = m_raPreambleId == 255;
        m_raPreambleId += preambleOverflow;
        g_raPreambleId += preambleOverflow;
    }
    /*raRnti should be subframeNo -1 */
    m_raRnti = 1;

    // 3GPP 36.321 5.1.4
    m_phySapProvider->SendRachPreamble(m_raPreambleId, m_raRnti);
    NS_LOG_INFO(" Sent preamble id " << +m_raPreambleId
                                     << " at: " << Simulator::Now().As(Time::MS));
    Time raWindowBegin = m_phySapProvider->GetSlotPeriod();
    Time raWindowEnd = m_phySapProvider->GetSlotPeriod() * (6 + m_rachConfig.raResponseWindowSize);
    Simulator::Schedule(raWindowBegin, &NrUeMac::StartWaitingForRaResponse, this);
    m_noRaResponseReceivedEvent =
        Simulator::Schedule(raWindowEnd, &NrUeMac::RaResponseTimeout, this, contention);

    // Tracing purposes
    Ptr<NrRachPreambleMessage> rachMsg = Create<NrRachPreambleMessage>();
    rachMsg->SetSourceBwp(GetBwpId());
    m_macTxedCtrlMsgsTrace(m_currentSlot, GetCellId(), m_rnti, GetBwpId(), rachMsg);
}

void
NrUeMac::StartWaitingForRaResponse()
{
    NS_LOG_FUNCTION(this);
    m_waitingForRaResponse = true;
}

void
NrUeMac::DoStartNonContentionBasedRandomAccessProcedure(uint16_t rnti,
                                                        [[maybe_unused]] uint8_t preambleId,
                                                        uint8_t prachMask)
{
    NS_LOG_FUNCTION(this << rnti << (uint16_t)preambleId << (uint16_t)prachMask);
    NS_ASSERT_MSG(prachMask == 0,
                  "requested PRACH MASK = " << (uint32_t)prachMask
                                            << ", but only PRACH MASK = 0 is supported");
    m_rnti = rnti;
    m_raPreambleId = preambleId;
    m_preambleTransmissionCounter = 0;
    bool contention = false;
    SendRaPreamble(contention);
}

void
NrUeMac::AddLc(uint8_t lcId, NrUeCmacSapProvider::LogicalChannelConfig lcConfig, NrMacSapUser* msu)
{
    NS_LOG_FUNCTION(this << " lcId" << (uint32_t)lcId);
    NS_ASSERT_MSG(m_lcInfoMap.find(lcId) == m_lcInfoMap.end(),
                  "cannot add channel because LCID " << (uint16_t)lcId << " is already present");

    LcInfo lcInfo;
    lcInfo.lcConfig = lcConfig;
    lcInfo.macSapUser = msu;
    m_lcInfoMap[lcId] = lcInfo;
}

void
NrUeMac::DoRemoveLc(uint8_t lcId)
{
    NS_LOG_FUNCTION(this << " lcId" << lcId);
    NS_ASSERT_MSG(m_lcInfoMap.find(lcId) != m_lcInfoMap.end(),
                  "could not find LCID " << (uint16_t)lcId);
    m_lcInfoMap.erase(lcId);
    m_ulBsrReceived.erase(lcId); // empty BSR buffer for this lcId
    // for (auto& harqProcess: m_miUlHarqProcessesPacket)
    //{
    //     uint32_t packets = harqProcess.m_pktBurst->GetNPackets();
    //     auto itPackets = harqProcess.m_pktBurst->GetPackets();
    //     for (uint32_t p = 0; p < packets; p++)
    //     {
    //         uint32_t i = packets-p-1;
    //         if (harqProcess.m_lcidList.at(i) == lcId)
    //         {
    //             // how to erase from packetburst?
    //             //auto it = itPackets.begin();
    //             //std::advance(it, i);
    //             //harqProcess.m_pktBurst->GetPackets().erase(it);
    //             auto it2 = harqProcess.m_lcidList.begin();
    //             std::advance(it2, i);
    //             harqProcess.m_lcidList.erase(it2);
    //         }
    //     }
    // }
}

NrMacSapProvider*
NrUeMac::GetUeMacSapProvider()
{
    return m_macSapProvider;
}

void
NrUeMac::DoReset()
{
    NS_LOG_FUNCTION(this);
    auto it = m_lcInfoMap.begin();
    while (it != m_lcInfoMap.end())
    {
        // don't delete CCCH)
        if (it->first == 0)
        {
            ++it;
        }
        else
        {
            // note: use of postfix operator preserves validity of iterator
            m_lcInfoMap.erase(it++);
        }
    }
    // note: rnti will be assigned by the gNB using RA response message
    m_rnti = 0;
    m_noRaResponseReceivedEvent.Cancel();
    m_rachConfigured = false;
    m_ulBsrReceived.clear();
}

//////////////////////////////////////////////

int64_t
NrUeMac::AssignStreams(int64_t stream)
{
    return DoAssignStreams(stream);
}

int64_t
NrUeMac::DoAssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    m_raPreambleUniformVariable->SetStream(stream);
    return 1;
}

} // namespace ns3
