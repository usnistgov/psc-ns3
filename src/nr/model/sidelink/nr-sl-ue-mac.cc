// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only AND NIST-Software

#define NS_LOG_APPEND_CONTEXT                                                                      \
    do                                                                                             \
    {                                                                                              \
        std::clog << " [ CellId " << GetCellId() << ", bwpId " << GetBwpId() << ", rnti "          \
                  << GetRnti() << "] ";                                                            \
    } while (false);

#include "nr-sl-ue-mac.h"

#include "nr-sl-mac-pdu-tag.h"
#include "nr-sl-sci-f1a-header.h"
#include "nr-sl-sci-f2a-header.h"
#include "nr-sl-ue-mac-harq.h"
#include "nr-sl-ue-mac-scheduler.h"

#include "ns3/nr-radio-bearer-tag.h"
#include "ns3/nr-rlc-tag.h"
#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/integer.h>
#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/random-variable-stream.h>
#include <ns3/uinteger.h>

#include <algorithm>
#include <bitset>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlUeMac");

NS_OBJECT_ENSURE_REGISTERED(NrSlUeMac);

// Constants defined in TS 38.321 Section 5.22.1.3
// Values are restricted to be <= 16, due to a 4-bit protocol field
constexpr uint8_t MAX_SIDELINK_PROCESS_MULTIPLE_PDU = 4;
constexpr uint8_t MAX_SIDELINK_PROCESS = 16;

TypeId
NrSlUeMac::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlUeMac")
            .SetParent<NrUeMac>()
            .AddConstructor<NrSlUeMac>()
            .AddAttribute("EnableSensing",
                          "Flag to enable NR Sidelink resource selection based on sensing; "
                          "otherwise, use random selection",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrSlUeMac::EnableSensing),
                          MakeBooleanChecker())
            .AddAttribute("Tproc0",
                          "t_proc0 in slots",
                          UintegerValue(1),
                          MakeUintegerAccessor(&NrSlUeMac::SetTproc0, &NrSlUeMac::GetTproc0),
                          MakeUintegerChecker<uint8_t>())
            .AddAttribute("T1",
                          "The start of the selection window in physical slots, accounting for "
                          "physical layer processing delay.  Must be less than 3, 5, 9, or 17 "
                          "slots for numerologies 0, 1, 2, 3.",
                          UintegerValue(2),
                          MakeUintegerAccessor(&NrSlUeMac::SetT1, &NrSlUeMac::GetT1),
                          MakeUintegerChecker<uint8_t>())
            .AddAttribute("T2",
                          "The end of the selection window in physical slots; the "
                          "value used is min(T2, packet delay budget) if PDB is set",
                          UintegerValue(33),
                          MakeUintegerAccessor(&NrSlUeMac::m_t2),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute(
                "ActivePoolId",
                "The pool id of the active pool used for TX and RX",
                UintegerValue(0),
                MakeUintegerAccessor(&NrSlUeMac::SetSlActivePoolId, &NrSlUeMac::GetSlActivePoolId),
                MakeUintegerChecker<uint16_t>())
            .AddAttribute(
                "MinTimeGapProcessing",
                "Minimum time (in slots) for processing PSFCH and preparing retransmission",
                UintegerValue(2),
                MakeUintegerAccessor(&NrSlUeMac::m_minTimeGapProcessing),
                MakeUintegerChecker<uint8_t>())
            .AddAttribute(
                "SlThresPsschRsrp",
                "A threshold in dBm used for sensing based UE autonomous resource selection",
                IntegerValue(-128),
                MakeIntegerAccessor(&NrSlUeMac::SetSlThresPsschRsrp,
                                    &NrSlUeMac::GetSlThresPsschRsrp),
                MakeIntegerChecker<int>())
            .AddAttribute("NrSlUeMacScheduler",
                          "The scheduler for this MAC instance",
                          PointerValue(),
                          MakePointerAccessor(&NrSlUeMac::m_nrSlUeMacScheduler),
                          MakePointerChecker<NrSlUeMacScheduler>())
            .AddAttribute("ResourcePercentage",
                          "The percentage threshold to indicate the minimum number of"
                          "candidate single-slot resources to be selected using sensing"
                          "procedure",
                          UintegerValue(20),
                          MakeUintegerAccessor(&NrSlUeMac::SetResourcePercentage,
                                               &NrSlUeMac::GetResourcePercentage),
                          MakeUintegerChecker<uint8_t>(1, 100))
            .AddAttribute("SlMaxNumConsecutiveDtx",
                          "The maximum number of consecutive PSFCH receptions that can"
                          "be absent (when HARQ feedack is enabled) before declaring"
                          "RLF or a value of 0 to disable this RLF detection mechanism",
                          UintegerValue(0),
                          MakeUintegerAccessor(&NrSlUeMac::SetSlMaxNumConsecutiveDtx,
                                               &NrSlUeMac::GetSlMaxNumConsecutiveDtx),
                          MakeUintegerChecker<uint32_t>(0, 32))
            .AddAttribute("NrSlUeMacHarq",
                          "Pointer accessor to the NrSlUeMacHarq object",
                          PointerValue(),
                          MakePointerAccessor(&NrSlUeMac::m_nrSlHarq),
                          MakePointerChecker<NrSlUeMacHarq>())
            .AddAttribute("MaxSidelinkProcess",
                          "The maximum number of NR sidelink HARQ processes",
                          UintegerValue(MAX_SIDELINK_PROCESS),
                          MakeUintegerAccessor(&NrSlUeMac::m_maxSlProcess),
                          MakeUintegerChecker<uint8_t>(1, 16))
            .AddAttribute("MaxSidelinkProcessMultiplePdu",
                          "The maximum number of NR sidelink HARQ processes reserved for "
                          "multiple-PDU (SPS) grants. Must not exceed MaxSidelinkProcess.",
                          UintegerValue(MAX_SIDELINK_PROCESS_MULTIPLE_PDU),
                          MakeUintegerAccessor(&NrSlUeMac::m_maxSlProcessMultiplePdu),
                          MakeUintegerChecker<uint8_t>(1, 16))
            .AddTraceSource("SlPscchScheduling",
                            "Information regarding NR SL PSCCH UE scheduling",
                            MakeTraceSourceAccessor(&NrSlUeMac::m_slPscchScheduling),
                            "ns3::SlPscchUeMacStatParameters::TracedCallback")
            .AddTraceSource("SlPsschScheduling",
                            "Information regarding NR SL PSSCH UE scheduling",
                            MakeTraceSourceAccessor(&NrSlUeMac::m_slPsschScheduling),
                            "ns3::SlPsschUeMacStatParameters::TracedCallback")
            .AddTraceSource("RxRlcPduWithTxRnti",
                            "PDU received trace also exporting TX UE RNTI in SL.",
                            MakeTraceSourceAccessor(&NrSlUeMac::m_rxRlcPduWithTxRnti),
                            "ns3::NrSlUeMac::ReceiveWithTxRntiTracedCallback")
            .AddTraceSource("SensingAlgorithm",
                            "Candidates selected by the mode 2 sensing algorithm",
                            MakeTraceSourceAccessor(&NrSlUeMac::m_tracedSensingAlgorithm),
                            "ns3::NrSlUeMac::SensingAlgorithmTracedCallback");
    return tid;
}

NrSlUeMac::NrSlUeMac()
    : NrUeMac()
{
    NS_LOG_FUNCTION(this);
    m_nrSlMacSapProvider = new MemberNrSlMacSapProvider<NrSlUeMac>(this);
    m_nrSlUeCmacSapProvider = new MemberNrSlUeCmacSapProvider<NrSlUeMac>(this);
    m_nrSlUePhySapUser = new MemberNrSlUePhySapUser<NrSlUeMac>(this);
    m_ueSelectedUniformVariable = CreateObject<UniformRandomVariable>();
    m_nrSlHarq = CreateObject<NrSlUeMacHarq>();
}

void
NrSlUeMac::NotifyConstructionCompleted()
{
    NS_LOG_FUNCTION(this);
    NrUeMac::NotifyConstructionCompleted();
    NS_ASSERT_MSG(m_maxSlProcessMultiplePdu <= m_maxSlProcess,
                  "MaxSidelinkProcessMultiplePdu must not exceed MaxSidelinkProcess");
    m_nrSlHarq->InitHarqBuffer(m_maxSlProcessMultiplePdu, m_maxSlProcess);
}

void
NrSlUeMac::SchedNrSlConfigInd(uint32_t dstL2Id, const NrSlGrant& grant)
{
    NS_LOG_FUNCTION(this << dstL2Id);

    NS_LOG_INFO("Received grant to dstL2Id " << dstL2Id << " on HARQ ID " << +grant.harqId
                                             << " containing " << grant.slotAllocations.size()
                                             << " slots and RRI " << grant.rri.As(Time::MS));
    auto it = m_slGrants.find(dstL2Id);
    if (it == m_slGrants.end())
    {
        NS_LOG_DEBUG("Adding new grant structure for " << dstL2Id);
        std::deque<NrSlGrant> q;
        q.push_back(grant);
        m_slGrants.emplace(dstL2Id, q);
    }
    else
    {
        it->second.push_back(grant);
        NS_LOG_DEBUG("Inserting new grant to " << dstL2Id << "; new size " << it->second.size());
    }
    // Notify the HARQ entity of the maximum number of transmissions granted
    // for the TB, whether HARQ FB is enabled, and the TB size
    m_nrSlHarq->UpdateHarqProcess(grant.harqId,
                                  grant.slotAllocations.size(),
                                  grant.harqEnabled,
                                  grant.tbSize);

    // The grant has a set of SlGrantResource.  One of these slots will be for
    // new data and some for retransmissions.  For the new data slots, notify
    // the RLC layer of transmission opportunities.
    for (const auto& itSlotAlloc : grant.slotAllocations)
    {
        if (itSlotAlloc.ndi == 0)
        {
            continue;
        }
        for (const auto& itLcRlcPduInfo : itSlotAlloc.slRlcPduInfo)
        {
            SidelinkLcIdentifier slLcId;
            slLcId.lcId = itLcRlcPduInfo.lcid;
            slLcId.srcL2Id = m_srcL2Id;
            slLcId.dstL2Id = dstL2Id;
            const auto& itLc = m_nrSlLcInfoMap.find(slLcId);
            if (itLc != m_nrSlLcInfoMap.end())
            {
                NS_LOG_INFO("Notifying NR SL RLC of TX opportunity for LC id "
                            << +itLcRlcPduInfo.lcid << " for TB size " << itLcRlcPduInfo.size);
                itLc->second.macSapUser->NotifyNrSlTxOpportunity(
                    NrSlMacSapUser::NrSlTxOpportunityParameters(itLcRlcPduInfo.size,
                                                                GetRnti(),
                                                                itLcRlcPduInfo.lcid,
                                                                0,
                                                                grant.harqId,
                                                                GetBwpId(),
                                                                m_srcL2Id,
                                                                dstL2Id));
            }
            else
            {
                // It is possible that bearers have been removed by the RRC layer
                NS_LOG_DEBUG("No LC with id " << +itLcRlcPduInfo.lcid << " found for destination "
                                              << itSlotAlloc.dstL2Id);
            }
        }
    }
}

void
NrSlUeMac::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_nrSlMacSapProvider;
    delete m_nrSlUeCmacSapProvider;
    delete m_nrSlUePhySapUser;
    if (m_nrSlHarq)
    {
        m_nrSlHarq->Dispose();
    }
    m_nrSlHarq = nullptr;
    if (m_nrSlUeMacScheduler)
    {
        m_nrSlUeMacScheduler->Dispose();
    }
    m_nrSlUeMacScheduler = nullptr;
    m_slTxPool = nullptr;
    m_slRxPool = nullptr;
    NrUeMac::DoDispose();
}

int64_t
NrSlUeMac::DoAssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    int64_t currentStream = stream;
    currentStream += NrUeMac::DoAssignStreams(currentStream);
    m_ueSelectedUniformVariable->SetStream(currentStream);
    currentStream++;
    if (m_nrSlUeMacScheduler)
    {
        currentStream += m_nrSlUeMacScheduler->AssignStreams(currentStream);
    }
    return (currentStream - stream);
}

void
NrSlUeMac::DoSlotIndication(const SfnSf& sfn)
{
    NS_LOG_FUNCTION(this << sfn);
    SetCurrentSlot(sfn);

    if (m_enableSensing)
    {
        RemoveOldSensingData(
            sfn,
            m_slTxPool->GetNrSlSensWindInSlots(GetBwpId(),
                                               m_poolId,
                                               m_nrSlUePhySapProvider->GetSlotPeriod()),
            m_sensingData,
            GetImsi());
        RemoveOldTransmitHistory(
            sfn,
            m_slTxPool->GetNrSlSensWindInSlots(GetBwpId(),
                                               m_poolId,
                                               m_nrSlUePhySapProvider->GetSlotPeriod()),
            m_transmitHistory,
            GetImsi());
    }

    // Scheduling can occur on any slot boundary
    m_nrSlUeMacScheduler->SchedNrSlTriggerReq(sfn);

    // trigger SL only when it is a SL slot
    if (m_slTxPool->IsSidelinkSlot(GetBwpId(), m_poolId, sfn.Normalize()))
    {
        DoNrSlSlotIndication(sfn);
    }
}

std::list<SlResourceInfo>
NrSlUeMac::GetCandidateResources(const SfnSf& sfn,
                                 const NrSlTransmissionParams& params,
                                 NrSlSelectionParams& selectionParams)
{
    return GetCandidateResourcesPrivate(sfn,
                                        params,
                                        selectionParams,
                                        m_slTxPool,
                                        m_nrSlUePhySapProvider->GetSlotPeriod(),
                                        GetImsi(),
                                        GetBwpId(),
                                        m_poolId,
                                        GetTotalSubCh(),
                                        m_sensingData,
                                        m_transmitHistory);
}

void
NrSlUeMac::AddNumConsecutiveDtx(uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this << dstL2Id << +lcId);

    auto ncdIt = m_numConsecutiveDtx.find(dstL2Id);
    if (ncdIt == m_numConsecutiveDtx.end())
    {
        NS_LOG_DEBUG("Adding numConsecutiveDtx for dstL2Id " << dstL2Id << " lcId " << +lcId);

        NumConsecutiveDtx ncd;
        ncd.m_count = 1;
        ncd.m_lcIds.insert(lcId);
        m_numConsecutiveDtx[dstL2Id] = ncd;
    }
    else
    {
        NS_LOG_DEBUG("dstL2Id " << dstL2Id << " found");

        if (!ncdIt->second.m_lcIds.contains(lcId))
        {
            NS_LOG_INFO("Adding lcId " << +lcId << " for dstL2Id " << dstL2Id);

            ncdIt->second.m_lcIds.insert(lcId);
        }
    }
}

void
NrSlUeMac::RemoveNumConsecutiveDtx(uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this << dstL2Id << +lcId);

    auto ncdIt = m_numConsecutiveDtx.find(dstL2Id);
    if (ncdIt != m_numConsecutiveDtx.end())
    {
        NS_LOG_DEBUG("dstL2Id " << dstL2Id << " found");

        auto lcIdIt = std::find(ncdIt->second.m_lcIds.begin(), ncdIt->second.m_lcIds.end(), lcId);

        if (lcIdIt != ncdIt->second.m_lcIds.end())
        {
            NS_LOG_INFO("Removing lcId " << +lcId << " for dstL2Id " << dstL2Id);

            lcIdIt = ncdIt->second.m_lcIds.erase(lcIdIt);
        }

        if (ncdIt->second.m_lcIds.empty())
        {
            NS_LOG_INFO("Clearing numConsecutiveDTX for dstL2Id "
                        << dstL2Id << " as there are no more LCs with outstanding HARQ FB");
            ncdIt = m_numConsecutiveDtx.end();
            ClearNumConsecutiveDtx(dstL2Id);
        }
    }
}

void
NrSlUeMac::IncrementNumConsecutiveDtx()
{
    NS_LOG_FUNCTION(this);

    auto it = m_numConsecutiveDtx.begin();
    while (it != m_numConsecutiveDtx.end())
    {
        NS_LOG_DEBUG("numConsecutiveDTX = " << it->second.m_count << "; slMaxNumConsecutiveDTX = "
                                            << +m_slMaxNumConsecutiveDtx << " for dstL2Id "
                                            << it->first);

        if (m_slMaxNumConsecutiveDtx && it->second.m_count >= m_slMaxNumConsecutiveDtx)
        {
            NS_LOG_DEBUG("sl-MaxNumConsecutiveDTX(" << m_slMaxNumConsecutiveDtx
                                                    << ") reached for dstL2Id " << it->first);

            NS_LOG_INFO("Erasing entry for dstL2Id " << it->first
                                                     << " and notifying upper layer of HARQ RLF");
            GetNrSlUeCmacSapUser()->NotifySlHarqProcessMaxTxWithNoFeedback(it->first);
            it = m_numConsecutiveDtx.erase(it);
        }
        else
        {
            it->second.m_count++;
            NS_LOG_DEBUG("numConsecutiveDTX incremented to " << it->second.m_count
                                                             << " for dstL2Id " << it->first);
            it++;
        }
    }
}

void
NrSlUeMac::ClearNumConsecutiveDtx(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);

    auto it = m_numConsecutiveDtx.find(dstL2Id);
    if (it != m_numConsecutiveDtx.end())
    {
        m_numConsecutiveDtx.erase(it);
        NS_LOG_INFO("numConsecutiveDTX cleared for dstL2Id " << dstL2Id);
    }
}

bool
NrSlUeMac::CheckT1WithinTproc1(const SfnSf& sfn, uint16_t t1Slots) const
{
    if ((sfn.GetNumerology() == 0 && t1Slots <= 3) || (sfn.GetNumerology() == 1 && t1Slots <= 5) ||
        (sfn.GetNumerology() == 2 && t1Slots <= 9) || (sfn.GetNumerology() == 3 && t1Slots <= 17))
    {
        return true;
    }
    return false;
}

uint16_t
NrSlUeMac::TimeToSlots(const SfnSf& sfn, Time timeVal) const
{
    NS_ASSERT_MSG(timeVal.GetMilliSeconds() <= 4000,
                  "Overflow check failed on input time " << timeVal.As(Time::MS));
    uint16_t timeInSlots =
        static_cast<uint16_t>((timeVal.GetMicroSeconds() << sfn.GetNumerology()) / 1000);
    return timeInSlots;
}

std::list<SlResourceInfo>
NrSlUeMac::GetCandidateResourcesPrivate(const SfnSf& sfn,
                                        const NrSlTransmissionParams& params,
                                        NrSlSelectionParams& selectionParams,
                                        Ptr<const NrSlCommResourcePool> txPool,
                                        Time slotPeriod,
                                        uint64_t imsi,
                                        uint8_t bwpId,
                                        uint16_t poolId,
                                        uint8_t totalSubCh,
                                        const std::list<SensingData>& sensingData,
                                        const std::list<SfnSf>& transmitHistory) const
{
    NS_LOG_FUNCTION(this << sfn.GetFrame() << +sfn.GetSubframe() << sfn.GetSlot() << params
                         << txPool << slotPeriod << imsi << +bwpId << poolId << +totalSubCh);

    // Following TS 38.214 and R1-2003807:
    // - if packet delay budget is unset (has value 0), use NrSlUeMac::T2
    //   - T2 >= T2min, a value set in the resource pool depending on numerology
    // - else if packet delay budget set, use min(packet delay budget, NrSlUeMac::T2)
    uint16_t t2;
    if (!params.m_packetDelayBudget.IsZero())
    {
        // Packet delay budget is known, so use it
        uint16_t t2pdb = TimeToSlots(sfn, params.m_packetDelayBudget);
        if (t2pdb > m_t2)
        {
            NS_LOG_DEBUG("Using T2 value from attribute "
                         << m_t2 << " less than packet delay budget " << t2pdb);
            t2 = m_t2;
        }
        else
        {
            NS_LOG_DEBUG("Using T2 value from packet delay budget: " << t2pdb);
            t2 = t2pdb;
        }
    }
    else
    {
        // Packet delay budget is not known, so use max(NrSlUeMac::T2, T2min)
        uint16_t t2min = txPool->GetT2Min(bwpId, poolId, sfn.GetNumerology());
        if (m_t2 < t2min)
        {
            NS_LOG_DEBUG("Using T2min value " << t2min);
            t2 = t2min;
        }
        else
        {
            NS_LOG_DEBUG("Using T2 value from attribute " << m_t2);
            t2 = m_t2;
        }
    }
    NS_ABORT_MSG_UNLESS(CheckT1WithinTproc1(sfn, m_t1),
                        "Configured T1 " << m_t1 << " is greater than Tproc1 for this numerology");
    selectionParams.m_t1 = m_t1;
    selectionParams.m_t2 = t2;
    SensingTraceReport report; // for tracing
    report.m_sfn = sfn;
    report.m_t0 = txPool->GetNrSlSensWindInSlots(bwpId, poolId, slotPeriod);
    report.m_tProc0 = m_tproc0;
    report.m_t1 = m_t1;
    report.m_t2 = t2;
    report.m_subchannels = totalSubCh;
    report.m_lSubch = params.m_lSubch;
    report.m_resourcePercentage = GetResourcePercentage();

    NS_LOG_DEBUG("Transmit  size: " << transmitHistory.size()
                                    << "; sensing data size: " << sensingData.size());

    // Input parameters (from params) are the priority, packet delay budget,
    // number of subchannels, the RRI, and the C_resel
    // - params.m_priority
    // - params.m_packetDelayBudget
    // - params.m_lSubch
    // - params.m_pRsvpTx
    // - params.m_cResel

    // TR 38.214 Section 8.1.4, return the set 'S_A' (candidate single slot
    // resources).  The size of this list is the algorithm parameter 'M_total'.

    // In this code, the list of candidateSlots is first obtained from the resource pool;
    // however, each SlotInfo doesn't have a list of subchannel (indices).
    // The NrUeMac copies each resource to the candidateResources list containing
    // SlResourceInfo which contains resource (slot and subchannel) information.

    std::list<NrSlCommResourcePool::SlotInfo> candidateSlots; // candidate single slots
    std::list<SlResourceInfo> candidateResources;             // S_A as per TS 38.214

    uint64_t absSlotIndex = sfn.Normalize();
    uint16_t numerology = sfn.GetNumerology();

    // Check the validity of the resource selection window configuration (T1 and T2)
    // and the following parameters: numerology and reservation period.
    uint16_t nsMs =
        (t2 - m_t1 + 1) *
        (1 / pow(2, numerology)); // number of slots multiplied by the slot duration in ms
    uint16_t rsvpMs =
        static_cast<uint16_t>(params.m_pRsvpTx.GetMilliSeconds()); // reservation period in ms
    NS_ABORT_MSG_IF(rsvpMs && nsMs > rsvpMs,
                    "An error may be generated due to the fact that the resource selection window "
                    "size is higher than the resource reservation period value. Make sure that "
                    "(T2-T1+1) x (1/(2^numerology)) < reservation period. Modify the values of T1, "
                    "T2, numerology, and reservation period accordingly.");

    // step 4 as per TS 38.214 sec 8.1.4
    candidateSlots =
        txPool->GetNrSlCommOpportunities(absSlotIndex, bwpId, numerology, poolId, m_t1, t2);
    report.m_initialCandidateSlotsSize = candidateSlots.size();
    if (candidateSlots.empty())
    {
        // Since, all the parameters (i.e., T1, T2min, and T2) of the selection
        // window are in terms of physical slots, it may happen that there are no
        // slots available for Sidelink, which depends on the TDD pattern and the
        // Sidelink bitmap.
        return std::list<SlResourceInfo>();
    }
    uint8_t psfchPeriod = txPool->GetPsfchPeriod(bwpId, poolId);
    uint8_t minTimeGapPsfch = txPool->GetMinTimeGapPsfch(bwpId, poolId);
    candidateResources = GetCandidateResourcesFromSlots(sfn,
                                                        psfchPeriod,
                                                        minTimeGapPsfch,
                                                        params.m_lSubch,
                                                        totalSubCh,
                                                        candidateSlots);
    uint32_t mTotal = candidateResources.size(); // total number of candidate single-slot resources
    report.m_initialCandidateResourcesSize = mTotal;
    if (!m_enableSensing)
    {
        NS_LOG_DEBUG("No sensing: Total slots selected " << mTotal);
        return candidateResources;
    }

    // This is an optimization to skip further null processing below
    if (m_enableSensing && sensingData.empty() && transmitHistory.empty())
    {
        NS_LOG_DEBUG("No sensing or data found: Total slots selected " << mTotal);
        m_tracedSensingAlgorithm(report, candidateResources, sensingData, transmitHistory);
        return candidateResources;
    }

    // Copy the buffer so we can trim the buffer as per Tproc0.
    // Note, we do not need to delete the latest measurement
    // from the original buffer because it will be deleted
    // by RemoveOldSensingData method once it is outdated.

    auto updatedSensingData = sensingData;

    // latest sensing data is at the end of the list
    // now remove the latest sensing data as per the value of Tproc0. This would
    // keep the size of the buffer equal to [n – T0 , n – Tproc0)

    auto rvIt = updatedSensingData.crbegin();
    if (rvIt != updatedSensingData.crend())
    {
        while (sfn.Normalize() - rvIt->sfn.Normalize() <= GetTproc0())
        {
            NS_LOG_DEBUG("IMSI " << GetImsi() << " ignoring sensed SCI at sfn " << sfn
                                 << " received at " << rvIt->sfn);
            updatedSensingData.pop_back();
            rvIt = updatedSensingData.crbegin();
        }
    }

    // Perform a similar operation on the transmit history.
    // latest is at the end of the list
    // keep the size of the buffer equal to [n – T0 , n – Tproc0)
    auto updatedHistory = transmitHistory;

    auto rvIt2 = updatedHistory.crbegin();
    while ((rvIt2 != updatedHistory.crend()) &&
           (sfn.Normalize() - rvIt2->Normalize() <= GetTproc0()))
    {
        NS_LOG_DEBUG("IMSI " << GetImsi() << " ignoring  at sfn " << sfn << " received at "
                             << *rvIt2);
        updatedHistory.pop_back();
        rvIt2 = updatedHistory.crbegin();
    }

    // step 5: filter candidateResources based on transmit history, if threshold
    // defined in step 5a) is met
    auto remainingCandidates = candidateResources;
    ExcludeResourcesBasedOnHistory(sfn,
                                   updatedHistory,
                                   remainingCandidates,
                                   txPool->GetSlResourceReservePeriodList(bwpId, poolId));
    if (remainingCandidates.size() >= (GetResourcePercentage() / 100.0) * mTotal)
    {
        NS_LOG_DEBUG("Step 5a check allows step 5 to pass: original: "
                     << candidateResources.size() << " remaining: " << remainingCandidates.size()
                     << " X: " << GetResourcePercentage() / 100.0);
    }
    else
    {
        NS_LOG_DEBUG("Step 5a fails-- too few remaining candidates: original: "
                     << candidateResources.size() << " updated: " << remainingCandidates.size()
                     << " X: " << GetResourcePercentage() / 100.0);
        remainingCandidates = candidateResources;
    }
    report.m_candidateResourcesSizeAfterStep5 = remainingCandidates.size();

    // step 6

    // calculate all possible transmissions based on sensed SCIs,
    // with past transmissions projected into the selection window.
    // Using a vector of ReservedResource, since we need to check all the SCIs
    // and their possible future transmission that are received during the
    // above trimmed sensing window. Each element of the vector holds a
    // list that holds the info of each received SCI and its possible
    // future transmissions.
    std::vector<std::list<ReservedResource>> sensingDataProjections;
    for (const auto& itSensedSlot : updatedSensingData)
    {
        uint16_t resvPeriodSlots = txPool->GetResvPeriodInSlots(bwpId,
                                                                poolId,
                                                                MilliSeconds(itSensedSlot.rsvp),
                                                                slotPeriod);
        std::list<ReservedResource> resourceList =
            ExcludeReservedResources(itSensedSlot, slotPeriod, resvPeriodSlots, m_t1, t2);
        sensingDataProjections.push_back(resourceList);
    }

    NS_LOG_DEBUG("Size of sensingDataProjections outer vector: " << sensingDataProjections.size());

    int rsrpThreshold = m_thresRsrp;
    report.m_initialRsrpThreshold = m_thresRsrp;
    auto candidateResourcesAfterStep5 = remainingCandidates;
    do
    {
        // following assignment is needed since we might have to perform
        // multiple do-while over the same list by increasing the rsrpThreshold
        remainingCandidates = candidateResourcesAfterStep5;
        NS_LOG_DEBUG("Step 6 loop iteration checking " << remainingCandidates.size()
                                                       << " resources against threshold "
                                                       << rsrpThreshold);
        auto itCandidate = remainingCandidates.begin();
        // itCandidate is the candidate single-slot resource R_x,y
        while (itCandidate != remainingCandidates.end())
        {
            bool erased = false;
            itCandidate->SetSlotBusy(false);
            // calculate all proposed transmissions of current candidate resource within selection
            // window
            std::list<SlResourceInfo> resourceInfoList;
            uint16_t pPrimeRsvpTx =
                txPool->GetResvPeriodInSlots(bwpId, poolId, params.m_pRsvpTx, slotPeriod);
            for (uint16_t i = 0; i < params.m_cResel; i++)
            {
                auto slResourceInfo = *itCandidate;
                slResourceInfo.sfn.Add(i * pPrimeRsvpTx);
                resourceInfoList.emplace_back(slResourceInfo);
            }
            // Traverse over all the possible transmissions derived from each sensed SCI
            for (const auto& itSensingDataProjections : sensingDataProjections)
            {
                // for all proposed transmissions of current candidate resource
                for (auto& itFutureCand : resourceInfoList)
                {
                    // Traverse the list of future projected transmissions for the given sensed SCI
                    for (const auto& itReservedResourceProjection : itSensingDataProjections)
                    {
                        // If overlapped in time ...
                        if (itFutureCand.sfn.Normalize() ==
                            itReservedResourceProjection.sfn.Normalize())
                        {
                            // And above the current threshold ...
                            if (itReservedResourceProjection.slRsrp > rsrpThreshold)
                            {
                                // And overlapped in frequency ...
                                if (OverlappedResource(itReservedResourceProjection.sbChStart,
                                                       itReservedResourceProjection.sbChLength,
                                                       itCandidate->slSubchannelStart,
                                                       itCandidate->slSubchannelLength))
                                {
                                    NS_LOG_DEBUG("Overlapped resource "
                                                 << itCandidate->sfn.Normalize() << " occupied "
                                                 << +itReservedResourceProjection.sbChLength
                                                 << " subchannels index "
                                                 << +itReservedResourceProjection.sbChStart);
                                    NS_LOG_DEBUG("Resource "
                                                 << itCandidate->sfn.Normalize() << ":["
                                                 << itCandidate->slSubchannelStart << ","
                                                 << (itCandidate->slSubchannelStart +
                                                     itCandidate->slSubchannelLength - 1)
                                                 << "] erased. Its rsrp : "
                                                 << itReservedResourceProjection.slRsrp
                                                 << " Threshold : " << rsrpThreshold);
                                    itCandidate = remainingCandidates.erase(itCandidate);
                                    erased = true; // Used to break out of outer for loop of sensed
                                                   // data projections
                                    break; // Stop further evaluation because candidate is erased
                                }
                                else
                                {
                                    // Although not overlapping in frequency, overlapped in time
                                    itCandidate->SetSlotBusy(true);
                                }
                            }
                        }
                    }
                }
                if (erased)
                {
                    break; // break for itSensingDataProjections
                }
            }
            if (!erased)
            {
                // Only need to increment if not erased above; if erased, the erase()
                // action will point itCandidate to the next item
                itCandidate++;
            }
        }
        // step 7. If the following while will not break, start over do-while
        // loop with rsrpThreshold increased by 3dB
        rsrpThreshold += 3;
        if (rsrpThreshold > 0)
        {
            // 0 dBm is the maximum RSRP threshold level so if we reach
            // it, that means all the available slots are overlapping
            // in time and frequency with the sensed slots, and the
            // RSRP of the sensed slots is very high.
            NS_LOG_DEBUG("Reached maximum RSRP threshold, unable to select resources");
            remainingCandidates.erase(remainingCandidates.begin(), remainingCandidates.end());
            break; // break do while
        }
    } while (remainingCandidates.size() < (GetResourcePercentage() / 100.0) * mTotal);

    NS_LOG_DEBUG(remainingCandidates.size()
                 << " resources selected after sensing resource selection from " << mTotal
                 << " slots");

    report.m_finalRsrpThreshold = (rsrpThreshold - 3); // undo the last increment
    m_tracedSensingAlgorithm(report, remainingCandidates, updatedSensingData, updatedHistory);
    return remainingCandidates;
}

std::list<SlResourceInfo>
NrSlUeMac::GetCandidateResourcesFromSlots(const SfnSf& sfn,
                                          uint8_t psfchPeriod,
                                          uint8_t minTimeGapPsfch,
                                          uint16_t lSubCh,
                                          uint16_t totalSubCh,
                                          std::list<NrSlCommResourcePool::SlotInfo> slotInfo) const
{
    NS_LOG_FUNCTION(this << sfn.Normalize() << psfchPeriod << minTimeGapPsfch << lSubCh
                         << totalSubCh << slotInfo.size());

    std::list<SlResourceInfo> nrResourceList;
    for (const auto& it : slotInfo)
    {
        for (uint16_t i = 0; i + lSubCh <= totalSubCh; i++)
        {
            SlResourceInfo info(it.numSlPscchRbs,
                                it.slPscchSymStart,
                                it.slPscchSymLength,
                                it.slPsschSymStart,
                                it.slPsschSymLength,
                                it.slSubchannelSize,
                                it.slMaxNumPerReserve,
                                psfchPeriod,
                                minTimeGapPsfch,
                                m_minTimeGapProcessing,
                                sfn.GetFutureSfnSf(it.slotOffset),
                                i,
                                lSubCh);
            nrResourceList.emplace_back(info);
        }
    }

    return nrResourceList;
}

void
NrSlUeMac::ExcludeResourcesBasedOnHistory(
    const SfnSf& sfn,
    const std::list<SfnSf>& transmitHistory,
    std::list<SlResourceInfo>& candidateList,
    const std::list<uint16_t>& slResourceReservePeriodList) const
{
    NS_LOG_FUNCTION(this << sfn.Normalize() << transmitHistory.size() << candidateList.size()
                         << slResourceReservePeriodList.size());

    std::set<uint64_t> sfnToExclude; // SFN slot numbers (normalized) to exclude
    uint64_t firstSfnNorm =
        candidateList.front().sfn.Normalize(); // lowest candidate SFN slot number
    uint64_t lastSfnNorm =
        candidateList.back().sfn.Normalize(); // highest candidate SFN slot number
    NS_LOG_DEBUG("Excluding resources between normalized SFNs (" << firstSfnNorm << ":"
                                                                 << lastSfnNorm << ")");

    // Iterate the resource reserve period list and the transmit history to
    // find all slot numbers such that multiples of the reserve period, when
    // added to the history's slot number, are within the candidate resource
    // slots lowest and highest numbers
    for (auto listIt : slResourceReservePeriodList)
    {
        if (listIt == 0)
        {
            continue; // 0ms value is ignored
        }
        listIt *= (1 << sfn.GetNumerology()); // Convert from ms to slots
        for (const auto& historyIt : transmitHistory)
        {
            uint16_t i = 1;
            uint64_t sfnToCheck = (historyIt).Normalize() + (listIt);
            while (sfnToCheck <= lastSfnNorm)
            {
                if (sfnToCheck >= firstSfnNorm)
                {
                    sfnToExclude.insert(sfnToCheck);
                }
                i++;
                sfnToCheck = (historyIt).Normalize() + (i) * (listIt);
            }
        }
    }
    // sfnToExclude is a set of SFN normalized slot numbers for which we need
    // to exclude (erase) any candidate resources that match
    for (auto i : sfnToExclude)
    {
        auto itCand = candidateList.begin();
        while (itCand != candidateList.end())
        {
            if ((*itCand).sfn.Normalize() == i)
            {
                NS_LOG_DEBUG("Erasing candidate resource at " << i);
                itCand = candidateList.erase(itCand);
            }
            else
            {
                itCand++;
            }
        }
    }
}

// Calculates parameters including Q for step 6(c) of sensing algorithm
std::list<ReservedResource>
NrSlUeMac::ExcludeReservedResources(SensingData sensedData,
                                    Time slotPeriod,
                                    uint16_t resvPeriodSlots,
                                    uint16_t t1,
                                    uint16_t t2) const
{
    NS_LOG_FUNCTION(this << sensedData.sfn.Normalize() << slotPeriod << resvPeriodSlots);
    std::list<ReservedResource> resourceList;

    double slotDurationMs = slotPeriod.GetSeconds() * 1000.0;
    NS_ABORT_MSG_IF(slotDurationMs > 1, "Slot length can not exceed 1 ms");
    // slot range is [n + T1, n + T2] (both endpoints included)
    uint16_t windowSlots = (t2 - t1) + 1;          // selection window length in physical slots
    double tScalMs = windowSlots * slotDurationMs; // Parameter T_scal in the algorithm
    double pRsvpMs = static_cast<double>(sensedData.rsvp); // Parameter Pprime_rsvp_rx in algorithm
    uint16_t q = 0;                                        // Parameter Q in the algorithm
    if (sensedData.rsvp != 0)
    {
        if (pRsvpMs < tScalMs)
        {
            q = static_cast<uint16_t>(std::ceil(tScalMs / pRsvpMs));
        }
        else
        {
            q = 1;
        }
        NS_LOG_DEBUG("tScalMs: " << tScalMs << " pRsvpMs: " << pRsvpMs);
    }
    uint16_t pPrimeRsvpRx = resvPeriodSlots;

    for (uint16_t i = 1; i <= q; i++)
    {
        ReservedResource resource(sensedData.sfn,
                                  sensedData.rsvp,
                                  sensedData.sbChLength,
                                  sensedData.sbChStart,
                                  sensedData.prio,
                                  sensedData.slRsrp);
        resource.sfn.Add(i * pPrimeRsvpRx);
        resourceList.emplace_back(resource);

        if (sensedData.gapReTx1 != std::numeric_limits<uint8_t>::max())
        {
            auto reTx1Slot = resource;
            reTx1Slot.sfn = resource.sfn.GetFutureSfnSf(sensedData.gapReTx1);
            reTx1Slot.sbChLength = sensedData.sbChLength;
            reTx1Slot.sbChStart = sensedData.sbChStartReTx1;
            resourceList.emplace_back(reTx1Slot);
        }
        if (sensedData.gapReTx2 != std::numeric_limits<uint8_t>::max())
        {
            auto reTx2Slot = resource;
            reTx2Slot.sfn = resource.sfn.GetFutureSfnSf(sensedData.gapReTx2);
            reTx2Slot.sbChLength = sensedData.sbChLength;
            reTx2Slot.sbChStart = sensedData.sbChStartReTx2;
            resourceList.emplace_back(reTx2Slot);
        }
    }
    NS_LOG_DEBUG("q: " << q << " Size of resourceList: " << resourceList.size());

    return resourceList;
}

void
NrSlUeMac::RemoveOldSensingData(const SfnSf& sfn,
                                uint16_t sensingWindow,
                                std::list<SensingData>& sensingData,
                                [[maybe_unused]] uint64_t imsi)
{
    NS_LOG_FUNCTION(this << sfn << sensingWindow << sensingData.size() << imsi);
    // oldest sensing data is on the top of the list
    auto it = sensingData.cbegin();
    while (it != sensingData.cend() && it->sfn.Normalize() < sfn.Normalize() - sensingWindow)
    {
        NS_LOG_DEBUG("IMSI " << imsi << " erasing SCI at sfn " << sfn << " received at "
                             << it->sfn);
        it = sensingData.erase(it);
    }
}

void
NrSlUeMac::RemoveOldTransmitHistory(const SfnSf& sfn,
                                    uint16_t sensingWindow,
                                    std::list<SfnSf>& history,
                                    [[maybe_unused]] uint64_t imsi)
{
    NS_LOG_FUNCTION(this << sfn << sensingWindow << history.size() << imsi);

    auto it = history.cbegin();
    while (it != history.cend())
    {
        if (it->Normalize() < sfn.Normalize() - sensingWindow)
        {
            NS_LOG_DEBUG("IMSI " << imsi << " erasing SFN history at sfn " << sfn << " sent at "
                                 << *it);
            it = history.erase(it);
        }
        else
        {
            // break upon reaching the edge of the sensing window
            break;
        }
        ++it;
    }
}

bool
NrSlUeMac::OverlappedResource(uint8_t firstStart,
                              uint8_t firstLength,
                              uint8_t secondStart,
                              uint8_t secondLength) const
{
    NS_ASSERT_MSG(firstLength && secondLength, "Length should not be zero");
    return (std::max(firstStart, secondStart) <
            std::min(firstStart + firstLength, secondStart + secondLength));
}

void
NrSlUeMac::DoReceiveSensingData(SensingData sensingData)
{
    NS_LOG_FUNCTION(this);

    if (m_enableSensing)
    {
        // oldest data will be at the front of the queue
        m_sensingData.push_back(sensingData);
    }
}

void
NrSlUeMac::DoReceivePsschPhyPdu(Ptr<PacketBurst> pdu)
{
    NS_LOG_FUNCTION(this << "Received Sidelink PDU from PHY");

    NrRadioBearerTag tag;
    NrSlSciF2aHeader sciF2a;
    // Separate SCI stage 2 packet from data packets
    std::list<Ptr<Packet>> dataPkts;
    bool foundSci2 = false;
    for (auto packet : pdu->GetPackets())
    {
        NrRadioBearerTag tag;
        if (!packet->PeekPacketTag(tag))
        {
            // SCI stage 2 is the only packet in the packet burst, which does
            // not have the tag
            packet->RemoveHeader(sciF2a);
            foundSci2 = true;
        }
        else
        {
            dataPkts.push_back(packet);
        }
    }

    NS_ABORT_MSG_IF(foundSci2 == false, "Did not find SCI stage 2 in PSSCH packet burst");
    NS_ASSERT_MSG(!dataPkts.empty(), "Received PHY PDU with not data packets");

    // Perform L2 filtering.
    // Remember, all the packets in the packet burst are for the same
    // destination, therefore it is safe to do the following.
    auto it = m_sidelinkRxDestinations.find(sciF2a.GetDstId());
    if (it == m_sidelinkRxDestinations.end())
    {
        // if we hit this assert that means SCI 1 reception code in NrUePhy
        // is not filtering the SCI 1 correctly.
        NS_FATAL_ERROR("Received PHY PDU with unknown destination " << sciF2a.GetDstId());
    }

    for (auto& pktIt : dataPkts)
    {
        pktIt->RemovePacketTag(tag);
        // Even though all the packets in the packet burst are for the same
        // destination, they can belong to different Logical Channels (LC),
        // therefore, we have to build the identifier and find the LC of the
        // packet.
        SidelinkLcIdentifier identifier;
        identifier.lcId = tag.GetLcid();
        identifier.srcL2Id = sciF2a.GetSrcId();
        identifier.dstL2Id = sciF2a.GetDstId();

        std::map<SidelinkLcIdentifier, SlLcInfoUeMac>::iterator it =
            m_nrSlLcInfoMap.find(identifier);
        if (it == m_nrSlLcInfoMap.end())
        {
            // notify RRC to setup bearer
            m_nrSlUeCmacSapUser->NotifySidelinkReception(tag.GetLcid(),
                                                         identifier.srcL2Id,
                                                         identifier.dstL2Id,
                                                         sciF2a.GetCastType(),
                                                         sciF2a.GetHarqFeedbackIndicator());

            // should be setup now
            it = m_nrSlLcInfoMap.find(identifier);
            if (it == m_nrSlLcInfoMap.end())
            {
                NS_FATAL_ERROR("Failure to setup Sidelink radio bearer for reception");
            }
        }
        NS_LOG_INFO("SL PDU reception on LC " << +tag.GetLcid()
                                              << " from src: " << identifier.srcL2Id
                                              << " to dst: " << identifier.dstL2Id);
        NrSlMacSapUser::NrSlReceiveRlcPduParameters rxPduParams(pktIt,
                                                                GetRnti(),
                                                                tag.GetLcid(),
                                                                identifier.srcL2Id,
                                                                identifier.dstL2Id);

        FireTraceSlRlcRxPduWithTxRnti(pktIt->Copy(), tag.GetLcid());

        it->second.macSapUser->ReceiveNrSlRlcPdu(rxPduParams);
    }
}

void
NrSlUeMac::DoNrSlSlotIndication(const SfnSf& sfn)
{
    NS_LOG_FUNCTION(this << " Frame " << sfn.GetFrame() << " Subframe " << +sfn.GetSubframe()
                         << " slot " << sfn.GetSlot() << " Normalized slot number "
                         << sfn.Normalize());

    bool atLeastOneTransmissionInSlot = false;
    // If a grant is scheduled for this slot, code below will provide the
    // TTI indication to the PHY (for either PSCCH or PSSCH).  However,
    // PSFCH may be scheduled in slots independent of the grants.  Provide
    // those TTI indications to the PHY here so that it can determine whether
    // to check its buffer for HARQ messages to send in this slot.
    if (m_slTxPool->SlotHasPsfch(sfn.Normalize(), GetBwpId(), m_poolId))
    {
        NS_LOG_DEBUG("Slot " << sfn.Normalize() << " has PSFCH");
        NrSlVarTtiAllocInfo feedbackVarTtiInfo;
        feedbackVarTtiInfo.SlVarTtiType = NrSlVarTtiAllocInfo::FEEDBACK;
        feedbackVarTtiInfo.symStart = 12; // PSFCH is always in slot 12
        feedbackVarTtiInfo.symLength = 1;
        // Current NR sidelink code assumes that all of the RBs in a BWP
        // are used; so the rbStart will always be zero and the rbLength
        // will be the number of RBs in the BWP
        feedbackVarTtiInfo.rbStart = 0;
        feedbackVarTtiInfo.rbLength =
            GetTotalSubCh() * m_slTxPool->GetNrSlSubChSize(GetBwpId(), m_poolId);
        m_nrSlUePhySapProvider->SetNrSlVarTtiAllocInfo(sfn, feedbackVarTtiInfo);
        NS_LOG_DEBUG("PSFCH at : Frame = " << sfn.GetFrame() << " SF = " << +sfn.GetSubframe()
                                           << " slot = " << +sfn.GetSlot());
        if (!m_nrSlUePhySapProvider->IsSlHarqFeedbackPending())
        {
            IncrementNumConsecutiveDtx();
        }
    }

    // check if we need to transmit PSCCH + PSSCH
    // We are starting with the transmission of data packets because if the buffer
    // at the RLC would be empty we just erase the grant of the current slot
    // without transmitting SCI 1 and SCI 2 message, and data. Therefore,
    // even we had the grant we will not put anything in the queues at the PHY.
    for (auto& itGrantMap : m_slGrants)
    {
        if (itGrantMap.second.empty())
        {
            continue;
        }
        for (auto itGrant = itGrantMap.second.begin(); itGrant != itGrantMap.second.end();)
        {
            bool removeGrant = false;
            NrSlGrant currentGrant = *itGrant;
            // Rename use of *itGrant (and currentGrant) below with "nrSlGrant"
            NS_ASSERT_MSG(
                itGrant->slotAllocations.size() > 0,
                "Empty grant in m_slGrants when iterated in NrUeMac::DoNrSlSlotIndication, rnti: "
                    << GetRnti() << " harqId: " << +itGrant->harqId);
            auto currentSlotIt = itGrant->slotAllocations.begin();
            if (currentSlotIt == itGrant->slotAllocations.end())
            {
                removeGrant = true;
            }
            else
            {
                // Find the first slot that is either at Now() or in the future
                while (currentSlotIt->sfn < sfn)
                {
                    currentSlotIt++;
                    if (currentSlotIt == itGrant->slotAllocations.end())
                    {
                        removeGrant = true;
                    }
                }
            }
            SlGrantResource currentSlot = *currentSlotIt;
            // Rename use of currentSlot below with "nrSlSlotAlloc"
            if (!removeGrant && currentGrant.slotAllocations.begin()->sfn == sfn)
            {
                NS_LOG_INFO("Grant on HARQ ID " << +currentGrant.harqId
                                                << " at : Frame = " << currentSlot.sfn.GetFrame()
                                                << " SF = " << +currentSlot.sfn.GetSubframe()
                                                << " slot = " << +currentSlot.sfn.GetSlot());
                if (currentSlot.ndi)
                {
                    auto pb = m_nrSlHarq->GetPacketBurst(currentSlot.dstL2Id, currentGrant.harqId);
                    if (pb && pb->GetNPackets() > 0)
                    {
                        m_nrSlMacPduTxed = true;
                        for (const auto& itPkt : pb->GetPackets())
                        {
                            NS_LOG_INFO("Sending PSSCH MAC PDU (1st Tx) dstL2Id: "
                                        << currentSlot.dstL2Id
                                        << " harqId: " << +currentGrant.harqId
                                        << " Packet Size: " << itPkt->GetSize());
                            m_nrSlUePhySapProvider->SendPsschMacPdu(itPkt, currentSlot.dstL2Id);
                        }
                        if (currentGrant.harqEnabled)
                        {
                            AddNumConsecutiveDtx(currentSlot.dstL2Id,
                                                 currentSlot.slRlcPduInfo.front().lcid);
                        }
                    }
                    else
                    {
                        // A grant with NDI has been published but there is
                        // no data in the HARQ buffer.  This can occur if the
                        // application supported by SPS has stopped.
                        m_nrSlMacPduTxed = false;
                        NS_LOG_DEBUG("Wasted grant opportunity ");
                        itGrant = itGrantMap.second.erase(itGrant);
                        continue;
                    }
                    itGrant->tbTxCounter++;
                }
                else
                {
                    // retx from MAC HARQ buffer
                    // we might want to match the LC ids in currentGrant.slRlcPduInfo and
                    // the LC ids whose packets are in the packet burst in the HARQ
                    // buffer. I am not doing it at the moment as it might slow down
                    // the simulation.
                    itGrant->tbTxCounter++;
                    auto pb = m_nrSlHarq->GetPacketBurst(currentSlot.dstL2Id, currentGrant.harqId);
                    if (pb && pb->GetNPackets() > 0)
                    {
                        m_nrSlMacPduTxed = true;
                        for (const auto& itPkt : pb->GetPackets())
                        {
                            NS_LOG_DEBUG("Sending PSSCH MAC PDU (Rtx) dstL2Id: "
                                         << currentSlot.dstL2Id
                                         << " harqId: " << +currentGrant.harqId
                                         << " Packet Size: " << itPkt->GetSize());
                            m_nrSlUePhySapProvider->SendPsschMacPdu(itPkt, currentSlot.dstL2Id);
                        }
                    }
                    else
                    {
                        NS_LOG_DEBUG("Wasted Retx opportunity");
                    }
                }
                // Remove current slot allocation from this grant
                if (currentGrant.tbTxCounter == currentGrant.nSelected)
                {
                    // Remove this grant from the queue before continuing to next grant
                    removeGrant = true;
                    NS_LOG_DEBUG("No slot allocations remain for grant to " << currentSlot.dstL2Id);
                }
                itGrant->slotAllocations.erase(currentSlotIt);
                // Ensure that the grant is removed if no slot allocations remain, to avoid
                // re-entry into the loop body with an empty slotAllocations
                if (itGrant->slotAllocations.empty())
                {
                    removeGrant = true;
                }
                if (!m_nrSlMacPduTxed)
                {
                    // NR SL MAC PDU was not txed. It can happen if RLC buffer was empty
                    NS_LOG_DEBUG("Slot wasted at : Frame = "
                                 << currentSlot.sfn.GetFrame()
                                 << " SF = " << +currentSlot.sfn.GetSubframe()
                                 << " slot = " << currentSlot.sfn.GetSlot());
                    if (removeGrant)
                    {
                        itGrant = itGrantMap.second.erase(itGrant);
                    }
                    else
                    {
                        ++itGrant;
                    }
                    m_nrSlMacPduTxed = false;
                    continue;
                }
                atLeastOneTransmissionInSlot = true;

                // prepare and send SCI format 2A message
                NrSlSciF2aHeader sciF2a;
                sciF2a.SetHarqId(currentGrant.harqId);
                sciF2a.SetNdi(currentSlot.ndi);
                sciF2a.SetRv(currentSlot.rv);
                sciF2a.SetSrcId(m_srcL2Id);
                sciF2a.SetDstId(currentSlot.dstL2Id);
                // fields which are not used yet that is why we set them to 0
                sciF2a.SetCsiReq(0);
                SidelinkLcIdentifier slLcId;
                // If multiple TB and LC are handled by this grant, they should
                // all share the same cast type, so it should suffice to fetch the
                // cast type from the first LC ID associated with the grant
                NS_ASSERT_MSG(currentSlot.slRlcPduInfo.size() > 0, "No SlRlcPduInfo available");
                slLcId.lcId = currentSlot.slRlcPduInfo.front().lcid;
                slLcId.srcL2Id = m_srcL2Id;
                slLcId.dstL2Id = currentSlot.dstL2Id;
                NS_ASSERT_MSG(currentGrant.castType != SidelinkInfo::CastType::Invalid,
                              "Invalid cast type for LC " << +slLcId.lcId << " dstL2Id "
                                                          << currentSlot.dstL2Id);
                sciF2a.SetCastType(static_cast<uint8_t>(currentGrant.castType));
                // Request HARQ feedback if HARQ enabled and PSFCH period > 0
                if (currentGrant.harqEnabled &&
                    (m_slTxPool->GetPsfchPeriod(GetBwpId(), m_poolId) > 0))
                {
                    sciF2a.SetHarqFbIndicator(1);
                }
                else
                {
                    sciF2a.SetHarqFbIndicator(0);
                }

                Ptr<Packet> pktSciF02 = Create<Packet>();
                pktSciF02->AddHeader(sciF2a);
                // put SCI stage 2 in PSSCH queue
                m_nrSlUePhySapProvider->SendPsschMacPdu(pktSciF02, currentSlot.dstL2Id);

                // set the VarTti allocation info for PSSCH
                NrSlVarTtiAllocInfo dataVarTtiInfo;
                dataVarTtiInfo.SlVarTtiType = NrSlVarTtiAllocInfo::DATA;
                dataVarTtiInfo.symStart = currentSlot.slPsschSymStart;
                dataVarTtiInfo.symLength = currentSlot.slPsschSymLength;
                dataVarTtiInfo.rbStart = currentSlot.slPsschSubChStart *
                                         m_slTxPool->GetNrSlSubChSize(GetBwpId(), m_poolId);
                dataVarTtiInfo.rbLength = currentSlot.slPsschSubChLength *
                                          m_slTxPool->GetNrSlSubChSize(GetBwpId(), m_poolId);
                m_nrSlUePhySapProvider->SetNrSlVarTtiAllocInfo(sfn, dataVarTtiInfo);

                // Collect statistics for NR SL PSCCH UE MAC scheduling trace
                SlPsschUeMacStatParameters psschStatsParams;
                psschStatsParams.timeMs = Simulator::Now().GetSeconds() * 1000.0;
                psschStatsParams.imsi = GetImsi();
                psschStatsParams.rnti = GetRnti();
                psschStatsParams.frameNum = currentSlot.sfn.GetFrame();
                psschStatsParams.subframeNum = currentSlot.sfn.GetSubframe();
                psschStatsParams.slotNum = currentSlot.sfn.GetSlot();
                psschStatsParams.symStart = currentSlot.slPsschSymStart;
                psschStatsParams.symLength = currentSlot.slPsschSymLength;
                psschStatsParams.rbStart = currentSlot.slPsschSubChStart *
                                           m_slTxPool->GetNrSlSubChSize(GetBwpId(), m_poolId);
                psschStatsParams.subChannelSize =
                    m_slTxPool->GetNrSlSubChSize(GetBwpId(), m_poolId);
                psschStatsParams.rbLength = currentSlot.slPsschSubChLength *
                                            m_slTxPool->GetNrSlSubChSize(GetBwpId(), m_poolId);
                psschStatsParams.harqId = currentGrant.harqId;
                psschStatsParams.ndi = currentSlot.ndi;
                psschStatsParams.rv = currentSlot.rv;
                psschStatsParams.srcL2Id = m_srcL2Id;
                psschStatsParams.dstL2Id = currentSlot.dstL2Id;
                psschStatsParams.csiReq = sciF2a.GetCsiReq();
                psschStatsParams.castType = sciF2a.GetCastType();
#ifdef NOTYET
                psschStatsParams.resoReselCounter = itGrantInfo.second.slResoReselCounter;
                psschStatsParams.cReselCounter = itGrantInfo.second.cReselCounter;
#endif

                m_slPsschScheduling(psschStatsParams); // Trace

                if (currentSlot.txSci1A)
                {
                    // prepare and send SCI format 1A message
                    NrSlSciF1aHeader sciF1a;
                    sciF1a.SetPriority(currentSlot.priority);
                    sciF1a.SetMcs(currentSlot.mcs);
                    sciF1a.SetSciStage2Format(NrSlSciF1aHeader::SciFormat2A);
                    sciF1a.SetSlResourceReservePeriod(
                        static_cast<uint16_t>(currentGrant.rri.GetMilliSeconds()));
                    sciF1a.SetTotalSubChannels(GetTotalSubCh());
                    sciF1a.SetIndexStartSubChannel(currentSlot.slPsschSubChStart);
                    sciF1a.SetLengthSubChannel(currentSlot.slPsschSubChLength);
                    sciF1a.SetSlMaxNumPerReserve(currentSlot.maxNumPerReserve);
                    if (currentSlot.slotNumInd > 1)
                    {
                        // itGrant->slotAllocations.cbegin () points to
                        // the next slot allocation this slot has to indicate
                        std::vector<uint8_t> gaps = ComputeGaps(currentSlot.sfn,
                                                                itGrant->slotAllocations.cbegin(),
                                                                currentSlot.slotNumInd);
                        std::vector<uint8_t> sbChIndex =
                            GetStartSbChOfReTx(itGrant->slotAllocations.cbegin(),
                                               currentSlot.slotNumInd);
                        sciF1a.SetGapReTx1(gaps.at(0));
                        sciF1a.SetIndexStartSbChReTx1(sbChIndex.at(0));
                        if (gaps.size() > 1)
                        {
                            sciF1a.SetGapReTx2(gaps.at(1));
                            NS_ASSERT_MSG(gaps.at(0) < gaps.at(1),
                                          "Incorrect computation of ReTx slot gaps");
                            sciF1a.SetIndexStartSbChReTx2(sbChIndex.at(1));
                        }
                    }

                    Ptr<Packet> pktSciF1a = Create<Packet>();
                    pktSciF1a->AddHeader(sciF1a);
                    NrSlMacPduTag tag(GetRnti(),
                                      currentSlot.sfn,
                                      currentSlot.slPsschSymStart,
                                      currentSlot.slPsschSymLength,
                                      currentGrant.tbSize,
                                      currentSlot.dstL2Id);
                    NS_LOG_DEBUG("NrSlMacPduTag contents "
                                 << tag.GetRnti() << " " << tag.GetSfn() << " "
                                 << +tag.GetSymStart() << " " << +tag.GetNumSym() << " "
                                 << tag.GetTbSize() << " " << tag.GetDstL2Id());

                    pktSciF1a->AddPacketTag(tag);

                    NS_LOG_INFO("Sending PSCCH MAC PDU to dstL2Id: "
                                << currentSlot.dstL2Id << " harqId: " << +currentGrant.harqId);
                    m_nrSlUePhySapProvider->SendPscchMacPdu(pktSciF1a);

                    // set the VarTti allocation info for PSCCH
                    NrSlVarTtiAllocInfo ctrlVarTtiInfo;
                    ctrlVarTtiInfo.SlVarTtiType = NrSlVarTtiAllocInfo::CTRL;
                    ctrlVarTtiInfo.symStart = currentSlot.slPscchSymStart;
                    ctrlVarTtiInfo.symLength = currentSlot.slPscchSymLength;
                    ctrlVarTtiInfo.rbStart = currentSlot.slPsschSubChStart *
                                             m_slTxPool->GetNrSlSubChSize(GetBwpId(), m_poolId);
                    ctrlVarTtiInfo.rbLength = currentSlot.numSlPscchRbs;
                    m_nrSlUePhySapProvider->SetNrSlVarTtiAllocInfo(sfn, ctrlVarTtiInfo);

                    // Collect statistics for NR SL PSCCH UE MAC scheduling trace
                    SlPscchUeMacStatParameters pscchStatsParams;
                    pscchStatsParams.timeMs = Simulator::Now().GetSeconds() * 1000.0;
                    pscchStatsParams.imsi = GetImsi();
                    pscchStatsParams.rnti = GetRnti();
                    pscchStatsParams.frameNum = currentSlot.sfn.GetFrame();
                    pscchStatsParams.subframeNum = currentSlot.sfn.GetSubframe();
                    pscchStatsParams.slotNum = currentSlot.sfn.GetSlot();
                    pscchStatsParams.symStart = currentSlot.slPscchSymStart;
                    pscchStatsParams.symLength = currentSlot.slPscchSymLength;
                    pscchStatsParams.rbStart = currentSlot.slPsschSubChStart *
                                               m_slTxPool->GetNrSlSubChSize(GetBwpId(), m_poolId);
                    pscchStatsParams.rbLength = currentSlot.numSlPscchRbs;
                    pscchStatsParams.priority = currentSlot.priority;
                    pscchStatsParams.mcs = currentSlot.mcs;
                    pscchStatsParams.tbSize = currentGrant.tbSize;
                    pscchStatsParams.slResourceReservePeriod =
                        static_cast<uint16_t>(currentGrant.rri.GetMilliSeconds());
                    pscchStatsParams.totalSubChannels = GetTotalSubCh();
                    pscchStatsParams.slPsschSubChStart = currentSlot.slPsschSubChStart;
                    pscchStatsParams.slPsschSubChLength = currentSlot.slPsschSubChLength;
                    pscchStatsParams.slMaxNumPerReserve = currentSlot.maxNumPerReserve;
                    pscchStatsParams.gapReTx1 = sciF1a.GetGapReTx1();
                    pscchStatsParams.gapReTx2 = sciF1a.GetGapReTx2();
                    m_slPscchScheduling(pscchStatsParams); // Trace
                }
            }
            else
            {
                // When there are no resources it may happen that the re-selection
                // counter of already existing destination remains zero. In this case,
                // we just go the next destination, if any.
            }

            if (removeGrant)
            {
                // The grant may be removed either when all slot allocations
                // have been used, or the TB has been positively acknowledged
                itGrant = itGrantMap.second.erase(itGrant);
            }
            else
            {
                ++itGrant;
            }

            // make this false before processing the grant for next destination
            m_nrSlMacPduTxed = false;
        }
    }
    if (atLeastOneTransmissionInSlot)
    {
        NS_LOG_DEBUG("IMSI " << GetImsi() << " adding SFN history at sfn " << sfn);
        m_transmitHistory.push_back(sfn);
    }
}

std::vector<uint8_t>
NrSlUeMac::ComputeGaps(const SfnSf& sfn,
                       std::set<SlGrantResource>::const_iterator it,
                       uint8_t slotNumInd)
{
    NS_LOG_FUNCTION(this);
    std::vector<uint8_t> gaps;
    // slotNumInd is the number including the first TX. Gaps are computed only for
    // the ReTxs
    for (uint8_t i = 0; i < slotNumInd - 1; i++)
    {
        std::advance(it, i);
        gaps.push_back(static_cast<uint8_t>(it->sfn.Normalize() - sfn.Normalize()));
    }

    return gaps;
}

std::vector<uint8_t>
NrSlUeMac::GetStartSbChOfReTx(std::set<SlGrantResource>::const_iterator it, uint8_t slotNumInd)
{
    NS_LOG_FUNCTION(this);
    std::vector<uint8_t> startSbChIndex;
    // slotNumInd is the number including the first TX. Start sub-channel index or
    // indices are retrieved only for the ReTxs
    for (uint8_t i = 0; i < slotNumInd - 1; i++)
    {
        std::advance(it, i);
        startSbChIndex.push_back(it->slPsschSubChStart);
    }

    return startSbChIndex;
}

NrSlMacSapProvider*
NrSlUeMac::GetNrSlMacSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlMacSapProvider;
}

void
NrSlUeMac::SetNrSlMacSapUser(NrSlMacSapUser* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlMacSapUser = s;
}

NrSlUeCmacSapUser*
NrSlUeMac::GetNrSlUeCmacSapUser() const
{
    return m_nrSlUeCmacSapUser;
}

NrSlUeCmacSapProvider*
NrSlUeMac::GetNrSlUeCmacSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlUeCmacSapProvider;
}

void
NrSlUeMac::SetNrSlUeCmacSapUser(NrSlUeCmacSapUser* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeCmacSapUser = s;
}

NrSlUePhySapUser*
NrSlUeMac::GetNrSlUePhySapUser()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlUePhySapUser;
}

void
NrSlUeMac::SetNrSlUePhySapProvider(NrSlUePhySapProvider* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUePhySapProvider = s;
}

void
NrSlUeMac::DoTransmitNrSlRlcPdu(const NrSlMacSapProvider::NrSlRlcPduParameters& params)
{
    NS_LOG_FUNCTION(this << +params.lcid << +params.harqProcessId);
    NrRadioBearerTag bearerTag(params.rnti, params.lcid, 0);
    params.pdu->AddPacketTag(bearerTag);
    NS_LOG_DEBUG("Adding packet in HARQ buffer for HARQ id "
                 << +params.harqProcessId << " pkt size " << params.pdu->GetSize());
    m_nrSlHarq->AddPacket(params.dstL2Id, params.lcid, params.harqProcessId, params.pdu);
    m_nrSlUeMacScheduler->NotifyNrSlRlcPduDequeue(params.dstL2Id,
                                                  params.lcid,
                                                  params.pdu->GetSize());
}

void
NrSlUeMac::DoReportNrSlBufferStatus(
    const NrSlMacSapProvider::NrSlReportBufferStatusParameters& params)
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("Reporting for Sidelink. Tx Queue size = " << params.txQueueSize);
    // Sidelink BSR
    std::map<SidelinkLcIdentifier, NrSlMacSapProvider::NrSlReportBufferStatusParameters>::iterator
        it;

    SidelinkLcIdentifier slLcId;
    slLcId.lcId = params.lcid;
    slLcId.srcL2Id = params.srcL2Id;
    slLcId.dstL2Id = params.dstL2Id;

    it = m_nrSlBsrReceived.find(slLcId);
    if (it != m_nrSlBsrReceived.end())
    {
        // update entry
        (*it).second = params;
    }
    else
    {
        m_nrSlBsrReceived.insert(std::make_pair(slLcId, params));
    }

    m_nrSlUeMacScheduler->SchedNrSlRlcBufferReq(params);
}

void
NrSlUeMac::DoAddNrSlLc(const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& slLcInfo,
                       NrSlMacSapUser* msu)
{
    NS_LOG_FUNCTION(this << +slLcInfo.lcId << slLcInfo.srcL2Id << slLcInfo.dstL2Id);
    NS_LOG_INFO("IMSI " << GetImsi() << " adding LC from " << slLcInfo.srcL2Id << " to "
                        << slLcInfo.dstL2Id << " lcId " << +slLcInfo.lcId << " dynamic "
                        << slLcInfo.dynamic << " pdb " << slLcInfo.pdb.As(Time::MS));
    SidelinkLcIdentifier slLcIdentifier;
    slLcIdentifier.lcId = slLcInfo.lcId;
    slLcIdentifier.srcL2Id = slLcInfo.srcL2Id;
    slLcIdentifier.dstL2Id = slLcInfo.dstL2Id;

    NS_ASSERT_MSG(m_nrSlLcInfoMap.find(slLcIdentifier) == m_nrSlLcInfoMap.end(),
                  "cannot add LCID " << +slLcInfo.lcId << ", srcL2Id " << slLcInfo.srcL2Id
                                     << ", dstL2Id " << slLcInfo.dstL2Id << " is already present");

    SlLcInfoUeMac slLcInfoUeMac;
    slLcInfoUeMac.lcInfo = slLcInfo;
    slLcInfoUeMac.macSapUser = msu;
    m_nrSlLcInfoMap.insert(std::make_pair(slLcIdentifier, slLcInfoUeMac));

    // Following if is needed because this method is called for both
    // TX and RX LCs addition into m_nrSlLcInfoMap. In case of RX LC, the
    // destination is this UE MAC.
    if (slLcInfo.srcL2Id == m_srcL2Id)
    {
        NS_LOG_DEBUG("UE MAC with src id " << m_srcL2Id << " giving info of LC to the scheduler");
        m_nrSlUeMacScheduler->CschedNrSlLcConfigReq(slLcInfo);
        AddNrSlDstL2Id(slLcInfo.dstL2Id, slLcInfo.priority);
    }
}

void
NrSlUeMac::DoRemoveNrSlLc(uint8_t slLcId, uint32_t srcL2Id, uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << +slLcId << srcL2Id << dstL2Id);
    NS_ASSERT_MSG(slLcId > 3, "Hey! I can delete only the LC for data radio bearers.");
    SidelinkLcIdentifier slLcIdentifier;
    slLcIdentifier.lcId = slLcId;
    slLcIdentifier.srcL2Id = srcL2Id;
    slLcIdentifier.dstL2Id = dstL2Id;
    NS_ASSERT_MSG(m_nrSlLcInfoMap.find(slLcIdentifier) != m_nrSlLcInfoMap.end(),
                  "could not find Sidelink LCID " << slLcId);
    if (srcL2Id == m_srcL2Id)
    {
        m_nrSlUeMacScheduler->RemoveNrSlLcConfigReq(slLcId, dstL2Id);
    }
    RemoveNumConsecutiveDtx(dstL2Id, slLcId);
    m_nrSlLcInfoMap.erase(slLcIdentifier);
    // Search for and remove grants
    auto itNrSlGrantMap = m_slGrants.find(dstL2Id);
    if (itNrSlGrantMap != m_slGrants.end())
    {
        for (auto itNrSlGrant = itNrSlGrantMap->second.begin();
             itNrSlGrant != itNrSlGrantMap->second.end();)
        {
            uint32_t foundBytes = 0;
            for (auto itPduInfos = itNrSlGrant->slotAllocations.begin()->slRlcPduInfo.begin();
                 itPduInfos != itNrSlGrant->slotAllocations.begin()->slRlcPduInfo.end();
                 itPduInfos++)
            {
                if (itPduInfos->lcid == slLcId)
                {
                    foundBytes += itPduInfos->size;
                }
            }
            if (foundBytes > 0)
            {
                NS_LOG_INFO("Erasing grant to " << dstL2Id << " LC ID " << +slLcId);
                itNrSlGrant = itNrSlGrantMap->second.erase(itNrSlGrant);
            }
            else
            {
                ++itNrSlGrant;
            }
        }
    }
}

void
NrSlUeMac::DoResetNrSlLcMap()
{
    NS_LOG_FUNCTION(this);

    auto it = m_nrSlLcInfoMap.begin();

    while (it != m_nrSlLcInfoMap.end())
    {
        if (it->first.lcId > 3) // SL DRB LC starts from 4
        {
            m_nrSlLcInfoMap.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void
NrSlUeMac::AddNrSlDstL2Id(uint32_t dstL2Id, uint8_t lcPriority)
{
    NS_LOG_FUNCTION(this << dstL2Id << lcPriority);
    bool foundDst = false;
    for (auto& it : m_sidelinkTxDestinations)
    {
        if (it.first == dstL2Id)
        {
            foundDst = true;
            if (lcPriority < it.second)
            {
                it.second = lcPriority;
            }
            break;
        }
    }

    if (!foundDst)
    {
        NS_LOG_INFO("Adding destination " << dstL2Id << " with priority " << +lcPriority
                                          << " to list of sidelink Tx destinations");
        m_sidelinkTxDestinations.emplace_back(dstL2Id, lcPriority);
    }

    std::sort(m_sidelinkTxDestinations.begin(), m_sidelinkTxDestinations.end(), CompareSecond);
}

bool
NrSlUeMac::CompareSecond(std::pair<uint32_t, uint8_t>& a, std::pair<uint32_t, uint8_t>& b)
{
    return a.second < b.second;
}

void
NrSlUeMac::DoAddNrSlCommTxPool(Ptr<const NrSlCommResourcePool> txPool)
{
    NS_LOG_FUNCTION(this << txPool);
    m_slTxPool = txPool;
}

void
NrSlUeMac::DoAddNrSlCommRxPool(Ptr<const NrSlCommResourcePool> rxPool)
{
    NS_LOG_FUNCTION(this);
    m_slRxPool = rxPool;
}

void
NrSlUeMac::DoSetSlProbResourceKeep(double probability)
{
    NS_LOG_FUNCTION(this << probability);
    NS_ASSERT_MSG(probability <= 1.0, "Probability value must be between 0 and 1");
    m_slProbResourceKeep = probability;
}

void
NrSlUeMac::DoSetSlMaxTxTransNumPssch(uint8_t maxTxPssch)
{
    NS_LOG_FUNCTION(this << +maxTxPssch);
    NS_ASSERT_MSG(maxTxPssch <= 32, "Number of PSSCH transmissions can not exceed 32");
    m_slMaxTxTransNumPssch = maxTxPssch;
}

void
NrSlUeMac::DoSetSourceL2Id(uint32_t srcL2Id)
{
    NS_LOG_FUNCTION(this << srcL2Id);
    m_srcL2Id = srcL2Id;
}

void
NrSlUeMac::DoAddNrSlRxDstL2Id(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    NS_LOG_INFO("Adding Rx destination " << dstL2Id << " to list of sidelink Rx destinations");
    m_sidelinkRxDestinations.insert(dstL2Id);
}

void
NrSlUeMac::DoRemoveNrSlRxDstL2Id(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    NS_LOG_INFO("Removing Rx destination " << dstL2Id << " from list of sidelink Rx destinations");
    m_sidelinkRxDestinations.erase(dstL2Id);
}

uint8_t
NrSlUeMac::DoGetSlActiveTxPoolId() const
{
    return GetSlActivePoolId();
}

std::vector<std::pair<uint32_t, uint8_t>>
NrSlUeMac::DoGetSlTxDestinations()
{
    return m_sidelinkTxDestinations;
}

std::unordered_set<uint32_t>
NrSlUeMac::DoGetSlRxDestinations()
{
    return m_sidelinkRxDestinations;
}

uint8_t
NrSlUeMac::GetSlMaxTxTransNumPssch() const
{
    NS_LOG_FUNCTION(this);
    return m_slMaxTxTransNumPssch;
}

double
NrSlUeMac::GetSlProbResourceKeep() const
{
    return m_slProbResourceKeep;
}

bool
NrSlUeMac::SlotHasPsfch(const SfnSf& sfn) const
{
    if (m_slTxPool->IsSidelinkSlot(GetBwpId(), GetSlActivePoolId(), sfn.Normalize()))
    {
        return m_slTxPool->SlotHasPsfch(sfn.Normalize(), GetBwpId(), GetSlActivePoolId());
    }
    else
    {
        return false;
    }
}

uint16_t
NrSlUeMac::GetResvPeriodInSlots(Time resvPeriod) const
{
    // The following ValidateResvPeriod() method was moved here from the
    // SetReservationPeriod() method.  The code no longer configures the RRI
    // for the MAC as a whole, but sets it on a per-LC basis.  This checks that
    // only the standard compliant values, including their intermediate values
    // could be set. TS38.321 sec 5.22.1.1 instructs to select one of the
    // allowed values configured by RRC in sl-ResourceReservePeriodList and
    // set the resource reservation interval with the selected value.
    // Also, this method checks that the reservation period is a multiple of
    // the length of the physical sidelink pool (i.e., the resultant bitmap
    // after applying SL bitmap over the TDD pattern).
    m_slTxPool->ValidateResvPeriod(GetBwpId(),
                                   m_poolId,
                                   resvPeriod,
                                   m_nrSlUePhySapProvider->GetSlotPeriod());
    return m_slTxPool->GetResvPeriodInSlots(GetBwpId(),
                                            m_poolId,
                                            resvPeriod,
                                            m_nrSlUePhySapProvider->GetSlotPeriod());
}

uint16_t
NrSlUeMac::GetNrSlSubChSize() const
{
    return m_slTxPool->GetNrSlSubChSize(GetBwpId(), m_poolId);
}

uint8_t
NrSlUeMac::GetPsfchPeriod() const
{
    return m_slTxPool->GetPsfchPeriod(GetBwpId(), m_poolId);
}

void
NrSlUeMac::CschedNrSlLcConfigCnf(uint8_t lcg, uint8_t lcId)
{
    NS_LOG_FUNCTION(this << +lcg << +lcId);
    NS_LOG_INFO("SL UE scheduler successfully added LCG " << +lcg << " LC id " << +lcId);
}

void
NrSlUeMac::RemoveNrSlLcConfigCnf(uint8_t lcId)
{
    NS_LOG_FUNCTION(this << +lcId);
    NS_LOG_INFO("SL UE scheduler successfully removed LC id " << +lcId);
}

NrSlUeMac::NrSlTransmissionParams::NrSlTransmissionParams(uint8_t prio,
                                                          Time pdb,
                                                          uint16_t lSubch,
                                                          Time pRsvpTx,
                                                          uint16_t cResel)
    : m_priority(prio),
      m_packetDelayBudget(pdb),
      m_lSubch(lSubch),
      m_pRsvpTx(pRsvpTx),
      m_cResel(cResel)
{
}

void
NrSlUeMac::EnableSensing(bool enableSensing)
{
    NS_LOG_FUNCTION(this << enableSensing);
    NS_ASSERT_MSG(m_enableSensing == false,
                  " Once the sensing is enabled, it can not be enabled or disabled again");
    m_enableSensing = enableSensing;
}

void
NrSlUeMac::DoReceivePsfch(uint32_t sendingNodeId, SlHarqInfo harqInfo)
{
    NS_LOG_FUNCTION(this << sendingNodeId << harqInfo.m_rnti);
    if (harqInfo.m_txRnti == GetRnti() && harqInfo.m_bwpIndex == GetBwpId())
    {
        // This HARQ is for us.  If this is a HARQ ACK, check whether to cancel
        // a pending grant for retransmitting the associated TB.
        bool transportBlockRemoved = m_nrSlHarq->RecvHarqFeedback(harqInfo);
        if (harqInfo.IsReceivedOk())
        {
            // Indicate that HARQ feedback was received; clearing HARQ RLF
            // numConsecutiveDtx for the given destination.
            ClearNumConsecutiveDtx(harqInfo.m_dstL2Id);
            // Look for the std::deque of NrSlGrant objects corresponding to
            // this dstL2Id
            auto itNrSlGrantMap = m_slGrants.find(harqInfo.m_dstL2Id);
            if (transportBlockRemoved && itNrSlGrantMap != m_slGrants.end())
            {
                // Iterate the std::deque to find the NrSlGrant with a
                // matching HARQ process ID
                for (auto itNrSlGrant = itNrSlGrantMap->second.begin();
                     itNrSlGrant != itNrSlGrantMap->second.end();)
                {
                    if (itNrSlGrant->harqId == harqInfo.m_harqProcessId)
                    {
                        NS_LOG_INFO("HARQ ACK: erasing grant to " << harqInfo.m_dstL2Id
                                                                  << " with HARQ process ID "
                                                                  << +harqInfo.m_harqProcessId);
                        itNrSlGrant = itNrSlGrantMap->second.erase(itNrSlGrant);
                        break;
                    }
                    else
                    {
                        ++itNrSlGrant;
                    }
                }
            }
        }
    }
}

void
NrSlUeMac::SetTproc0(uint8_t tproc0)
{
    NS_LOG_FUNCTION(this << +tproc0);
    m_tproc0 = tproc0;
}

uint8_t
NrSlUeMac::GetTproc0() const
{
    return m_tproc0;
}

uint8_t
NrSlUeMac::GetT1() const
{
    return m_t1;
}

void
NrSlUeMac::SetT1(uint8_t t1)
{
    NS_LOG_FUNCTION(this << +t1);
    m_t1 = t1;
}

uint16_t
NrSlUeMac::GetSlActivePoolId() const
{
    return m_poolId;
}

void
NrSlUeMac::SetSlActivePoolId(uint16_t poolId)
{
    m_poolId = poolId;
}

uint8_t
NrSlUeMac::GetTotalSubCh() const
{
    uint16_t subChSize = m_slTxPool->GetNrSlSubChSize(static_cast<uint8_t>(GetBwpId()), m_poolId);

    uint8_t totalSubChannels =
        static_cast<uint8_t>(std::floor(m_nrSlUePhySapProvider->GetBwInRbs() / subChSize));

    return totalSubChannels;
}

std::pair<uint8_t, uint8_t>
NrSlUeMac::GetNumSidelinkProcess() const
{
    return std::make_pair(MAX_SIDELINK_PROCESS_MULTIPLE_PDU, MAX_SIDELINK_PROCESS);
}

void
NrSlUeMac::SetSlThresPsschRsrp(int thresRsrp)
{
    NS_LOG_FUNCTION(this);
    m_thresRsrp = thresRsrp;
}

int
NrSlUeMac::GetSlThresPsschRsrp() const
{
    NS_LOG_FUNCTION(this);
    return m_thresRsrp;
}

void
NrSlUeMac::SetResourcePercentage(uint8_t percentage)
{
    NS_LOG_FUNCTION(this);
    m_resPercentage = percentage;
}

uint8_t
NrSlUeMac::GetResourcePercentage() const
{
    NS_LOG_FUNCTION(this);
    return m_resPercentage;
}

void
NrSlUeMac::SetSlMaxNumConsecutiveDtx(uint32_t max)
{
    std::set<uint8_t> options = {0, 1, 2, 3, 4, 6, 8, 16, 32};
    NS_ABORT_MSG_UNLESS(options.contains(max), "Invalid sl-MaxNumConsecutiveDTX value provided");
    m_slMaxNumConsecutiveDtx = max;
}

uint32_t
NrSlUeMac::GetSlMaxNumConsecutiveDtx() const
{
    return m_slMaxNumConsecutiveDtx;
}

void
NrSlUeMac::FireTraceSlRlcRxPduWithTxRnti(const Ptr<Packet> p, uint8_t lcid)
{
    NS_LOG_FUNCTION(this);
    // Receiver timestamp
    NrRlcTag rlcTag;
    Time delay;

    bool ret = p->FindFirstMatchingByteTag(rlcTag);
    NS_ASSERT_MSG(ret, "NrRlcTag is missing for NR SL");

    delay = Simulator::Now() - rlcTag.GetSenderTimestamp();
    m_rxRlcPduWithTxRnti(GetImsi(),
                         GetRnti(),
                         rlcTag.GetTxRnti(),
                         lcid,
                         p->GetSize(),
                         delay.GetSeconds());
}

std::ostream&
operator<<(std::ostream& os, const NrSlUeMac::NrSlTransmissionParams& p)
{
    os << "Prio: " << +p.m_priority << ", PDB: " << p.m_packetDelayBudget.As(Time::MS)
       << ", subchannels: " << p.m_lSubch << ", RRI: " << p.m_pRsvpTx.As(Time::MS)
       << ", Cresel: " << p.m_cResel;
    return os;
}

std::ostream&
operator<<(std::ostream& os, const struct NrSlUeMac::SensingTraceReport& report)
{
    os << "Sfn (" << report.m_sfn.GetFrame() << ":" << +report.m_sfn.GetSubframe() << ":"
       << +report.m_sfn.GetSlot() << "):" << report.m_sfn.Normalize() << " T0 " << report.m_t0
       << " T_proc0 " << +report.m_tProc0 << " T1 " << +report.m_t1 << " T2 " << report.m_t2
       << " poolSubch " << report.m_subchannels << " lSubch " << report.m_lSubch << " resoPct "
       << +report.m_resourcePercentage << " initCandSlots " << report.m_initialCandidateSlotsSize
       << " initCandReso " << report.m_initialCandidateResourcesSize << " candResoAfterStep5 "
       << report.m_candidateResourcesSizeAfterStep5 << " initRsrp " << report.m_initialRsrpThreshold
       << " finalRsrp " << report.m_finalRsrpThreshold;
    return os;
}

Time
NrSlUeMac::GetSlotPeriod() const
{
    return m_nrSlUePhySapProvider->GetSlotPeriod();
}

Ptr<const NrSlCommResourcePool>
NrSlUeMac::GetTxPool() const
{
    return m_slTxPool;
}

} // namespace ns3
