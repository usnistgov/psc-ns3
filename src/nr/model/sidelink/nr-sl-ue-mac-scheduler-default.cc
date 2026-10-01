// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#include "nr-sl-ue-mac-scheduler-default.h"

#include "nr-sl-mcs-controller.h"
#include "nr-sl-ue-mac-harq.h"

#include "ns3/nr-ue-mac.h"
#include <ns3/boolean.h>
#include <ns3/enum.h>
#include <ns3/log.h>
#include <ns3/node-list.h>
#include <ns3/node.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/pointer.h>
#include <ns3/uinteger.h>

#include <cmath>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <vector>

#undef NS_LOG_APPEND_CONTEXT
#define NS_LOG_APPEND_CONTEXT                                                                      \
    if (GetMac())                                                                                  \
    {                                                                                              \
        std::clog << "[imsi=" << GetMac()->GetImsi() << "] ";                                      \
    }

namespace ns3
{

/**
 * A MAC header consists of a two-byte subheader for each RLC PDU, MAC control element (CE),
 * and padding.  This model does not actually append the MAC subheader bytes but we assume
 * two bytes per RLC PDU.
 */
constexpr uint16_t SUBHEADER_SIZE = 2; // bytes

/**
 * Minimum RLC UM PDU header overhead (the Sequence Number field).
 * When data is segmented across multiple TBs, each TB carries an RLC
 * PDU whose header consumes part of the TX opportunity.  The scheduler
 * uses this constant to avoid underestimating the number of TBs needed.
 */
constexpr uint16_t RLC_HEADER_SIZE = 2; // bytes

//!< Table 1 (64 QAM) for sidelink mode 2
constexpr uint8_t MCS_TABLE = 1;

NS_LOG_COMPONENT_DEFINE("NrSlUeMacSchedulerDefault");
NS_OBJECT_ENSURE_REGISTERED(NrSlUeMacSchedulerDefault);

TypeId
NrSlUeMacSchedulerDefault::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::NrSlUeMacSchedulerDefault")
            .SetParent<NrSlUeMacScheduler>()
            .AddConstructor<NrSlUeMacSchedulerDefault>()
            .SetGroupName("nr")
            .AddAttribute("DefaultMcs",
                          "The default value of the MCS used by this scheduler",
                          UintegerValue(0),
                          MakeUintegerAccessor(&NrSlUeMacSchedulerDefault::m_defaultMcs),
                          MakeUintegerChecker<uint8_t>())
            .AddAttribute("PriorityToSps",
                          "Flag to give scheduling priority to logical channels that are "
                          "configured with SPS in case of priority tie",
                          BooleanValue(true),
                          MakeBooleanAccessor(&NrSlUeMacSchedulerDefault::m_prioToSps),
                          MakeBooleanChecker())
            .AddAttribute("WholeSlotExclusion",
                          "Whether to exclude use of candidate resources when other resources "
                          "in same slot are sensed",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrSlUeMacSchedulerDefault::m_wholeSlotExclusion),
                          MakeBooleanChecker())
            .AddAttribute(
                "AllowMultipleDestinationsPerSlot",
                "Allow scheduling of multiple destinations in same slot",
                BooleanValue(false),
                MakeBooleanAccessor(&NrSlUeMacSchedulerDefault::m_allowMultipleDestinationsPerSlot),
                MakeBooleanChecker())
            .AddAttribute(
                "IdealSchedLevel",
                "Level of knowledge from external nodes used to filter candidate resources. "
                "None: no external grants considered, "
                "Global: consider external grants from all other nodes, "
                "LocalUnicast: consider external grants only from peers with at least one unicast "
                "sidelink logical channel.",
                EnumValue(NrSlUeMacSchedulerDefault::IDEAL_SCHED_NONE),
                MakeEnumAccessor<NrSlUeMacSchedulerDefault::IdealSchedLevel>(
                    &NrSlUeMacSchedulerDefault::m_idealSchedLevel),
                MakeEnumChecker(NrSlUeMacSchedulerDefault::IDEAL_SCHED_NONE,
                                "None",
                                NrSlUeMacSchedulerDefault::IDEAL_SCHED_GLOBAL,
                                "Global",
                                NrSlUeMacSchedulerDefault::IDEAL_SCHED_LOCAL_UNICAST,
                                "LocalUnicast"))
            .AddAttribute("MinimumSpsGrantSize",
                          "The minimum SPS grant size, in bytes",
                          UintegerValue(0),
                          MakeUintegerAccessor(&NrSlUeMacSchedulerDefault::m_minimumSpsGrantSize),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute(
                "SpsReselectionThreshold",
                "The SPS grant reselection threshold, in bytes",
                UintegerValue(std::numeric_limits<uint16_t>::max()),
                MakeUintegerAccessor(&NrSlUeMacSchedulerDefault::m_spsReselectionThreshold),
                MakeUintegerChecker<uint16_t>())
            .AddAttribute(
                "AllowSupplementalDynamicGrants",
                "Flag to enable/disable supplemental dynamic grants for overloaded SPS grants",
                BooleanValue(true),
                MakeBooleanAccessor(&NrSlUeMacSchedulerDefault::m_allowSupplementalGrants),
                MakeBooleanChecker())
            .AddAttribute("McsController",
                          "The (optional) MCS controller for the scheduler",
                          PointerValue(),
                          MakePointerAccessor(&NrSlUeMacSchedulerDefault::m_mcsController),
                          MakePointerChecker<NrSlMcsController>())
            .AddTraceSource("SchedulingReport",
                            "Report on the execution of the scheduler",
                            MakeTraceSourceAccessor(&NrSlUeMacSchedulerDefault::m_schedulingTrace),
                            "ns3::NrSlUeMacSchedulerDefault::SchedulingReportCallback")
            .AddTraceSource("McsChange",
                            "Report on a dynamic MCS change",
                            MakeTraceSourceAccessor(&NrSlUeMacSchedulerDefault::m_mcsChangeTrace),
                            "ns3::NrSlUeMacSchedulerDefault::McsChangeCallback");
    return tid;
}

NrSlUeMacSchedulerDefault::NrSlUeMacSchedulerDefault()
{
    NS_LOG_FUNCTION(this);
    m_grantSelectionUniformVariable = CreateObject<UniformRandomVariable>();
    m_destinationUniformVariable = CreateObject<UniformRandomVariable>();
    m_ueSelectedUniformVariable = CreateObject<UniformRandomVariable>();
}

NrSlUeMacSchedulerDefault::~NrSlUeMacSchedulerDefault()
{
    // just to make sure
    m_dstMap.clear();
}

void
NrSlUeMacSchedulerDefault::DoCschedNrSlLcConfigReq(
    const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& params)
{
    NS_LOG_FUNCTION(this << params.dstL2Id << +params.lcId);

    auto dstInfo = CreateDstInfo(params);
    const auto& lcgMap = dstInfo->GetNrSlLCG(); // Map of unique_ptr should not copy
    auto itLcg = lcgMap.find(params.lcGroup);
    auto itLcgEnd = lcgMap.end();
    if (itLcg == itLcgEnd)
    {
        NS_LOG_INFO("Created new NR SL LCG for destination "
                    << dstInfo->GetDstL2Id()
                    << " LCG ID =" << static_cast<uint32_t>(params.lcGroup));
        itLcg = dstInfo->Insert(CreateLCG(params.lcGroup));
    }

    itLcg->second->Insert(CreateLC(params));
    NS_LOG_INFO("Added LC id " << +params.lcId << " in LCG " << +params.lcGroup);
    // send confirmation to UE MAC
    GetMac()->CschedNrSlLcConfigCnf(params.lcGroup, params.lcId);
}

std::shared_ptr<NrSlUeMacSchedulerDstInfo>
NrSlUeMacSchedulerDefault::CreateDstInfo(
    const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& params)
{
    std::shared_ptr<NrSlUeMacSchedulerDstInfo> dstInfo = nullptr;
    auto itDst = m_dstMap.find(params.dstL2Id);
    if (itDst == m_dstMap.end())
    {
        NS_LOG_INFO("Creating destination info. Destination L2 id " << params.dstL2Id);

        dstInfo = std::make_shared<NrSlUeMacSchedulerDstInfo>(params.dstL2Id);
        // mcsCache stores any previous calls to SetMcs() on dstL2Id that
        // may have occurred before the dstInfo entry has been created
        auto it = m_mcsCache.find(params.dstL2Id);
        if (it != m_mcsCache.end())
        {
            NS_LOG_DEBUG("Configuring non-default MCS " << +it->second << " for destination "
                                                        << params.dstL2Id);
            dstInfo->SetDstMcs(it->second);
        }
        else
        {
            NS_LOG_DEBUG("Configuring default MCS " << +m_defaultMcs << " for destination "
                                                    << params.dstL2Id);
            dstInfo->SetDstMcs(m_defaultMcs);
        }

        itDst = m_dstMap.insert(std::make_pair(params.dstL2Id, dstInfo)).first;
    }
    else
    {
        NS_LOG_DEBUG("Doing nothing. You are seeing this because we are adding new LC "
                     << +params.lcId << " for Dst " << params.dstL2Id);
        dstInfo = itDst->second;
    }

    return dstInfo;
}

void
NrSlUeMacSchedulerDefault::DoRemoveNrSlLcConfigReq(uint8_t lcid, uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << lcid << dstL2Id);
    RemoveDstInfo(lcid, dstL2Id);
    // Send confirmation to MAC
    GetMac()->RemoveNrSlLcConfigCnf(lcid);
    RemoveUnpublishedGrants(lcid, dstL2Id);
}

void
NrSlUeMacSchedulerDefault::RemoveDstInfo(uint8_t lcid, uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << lcid << dstL2Id);
    auto itDst = m_dstMap.find(dstL2Id);
    if (itDst != m_dstMap.end())
    {
        NS_LOG_INFO("Found Destination L2 ID " << dstL2Id);
        // find LCID in available LCGIDs and remove it
        const auto& lcgMap = itDst->second->GetNrSlLCG();
        for (auto it = lcgMap.begin(); it != lcgMap.end(); it++)
        {
            it->second->Remove(lcid);
        }
    }
    else
    {
        NS_LOG_DEBUG("Already removed! Nothing to do!");
    }
}

void
NrSlUeMacSchedulerDefault::RemoveUnpublishedGrants(uint8_t lcid, uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << lcid << dstL2Id);
    auto itGrantInfo = m_grantInfo.find(dstL2Id);
    if (itGrantInfo != m_grantInfo.end())
    {
        for (auto itGrantVector = itGrantInfo->second.begin();
             itGrantVector != itGrantInfo->second.end();)
        {
            uint32_t foundBytes = 0;
            [[maybe_unused]] uint32_t foundSlots = 0;
            for (auto allocIt = itGrantVector->slotAllocations.begin();
                 allocIt != itGrantVector->slotAllocations.end();
                 ++allocIt)
            {
                for (auto pduInfoIt : allocIt->slRlcPduInfo)
                {
                    if (pduInfoIt.lcid == lcid)
                    {
                        foundBytes += pduInfoIt.size;
                        foundSlots++;
                    }
                }
            }
            if (foundBytes > 0)
            {
                NS_LOG_INFO("Removing unpublished grant for dstL2Id "
                            << dstL2Id << " lcid " << lcid << " slots " << foundSlots << " bytes "
                            << foundBytes);
                itGrantVector = itGrantInfo->second.erase(itGrantVector);
            }
            else
            {
                ++itGrantVector;
            }
        }
    }
    else
    {
        NS_LOG_DEBUG("No unpublished grants for dstL2Id " << dstL2Id << " lcid " << lcid);
    }
}

NrSlLCGPtr
NrSlUeMacSchedulerDefault::CreateLCG(uint8_t lcGroup) const
{
    NS_LOG_FUNCTION(this << +lcGroup);
    return std::unique_ptr<NrSlUeMacSchedulerLCG>(new NrSlUeMacSchedulerLCG(lcGroup));
}

NrSlLCPtr
NrSlUeMacSchedulerDefault::CreateLC(
    const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& params) const
{
    NS_LOG_FUNCTION(this << params.dstL2Id << +params.lcId);
    return std::unique_ptr<NrSlUeMacSchedulerLC>(new NrSlUeMacSchedulerLC(params));
}

void
NrSlUeMacSchedulerDefault::DoSchedNrSlRlcBufferReq(
    const struct NrSlMacSapProvider::NrSlReportBufferStatusParameters& params)
{
    NS_LOG_FUNCTION(this << params.dstL2Id << +params.lcid);

    GetSecond DstInfoOf;
    auto itDst = m_dstMap.find(params.dstL2Id);
    NS_ABORT_MSG_IF(itDst == m_dstMap.end(), "Destination " << params.dstL2Id << " info not found");

    for (const auto& lcg : DstInfoOf(*itDst)->GetNrSlLCG())
    {
        if (lcg.second->Contains(params.lcid))
        {
            NS_LOG_INFO("Updating buffer status for LC in LCG: "
                        << +lcg.first << " LC: " << +params.lcid << " dstL2Id: " << params.dstL2Id
                        << " queue size: " << params.txQueueSize);
            lcg.second->UpdateInfo(params);
            return;
        }
    }
    // Fail miserably because we didn't find any LC
    NS_FATAL_ERROR("The LC does not exist. Can't update");
}

uint8_t
NrSlUeMacSchedulerDefault::GetRandomReselectionCounter(Time rri) const
{
    uint8_t min;
    uint8_t max;
    uint16_t periodInt = static_cast<uint16_t>(rri.GetMilliSeconds());

    switch (periodInt)
    {
    case 100:
    case 150:
    case 200:
    case 250:
    case 300:
    case 350:
    case 400:
    case 450:
    case 500:
    case 550:
    case 600:
    case 700:
    case 750:
    case 800:
    case 850:
    case 900:
    case 950:
    case 1000:
        min = 5;
        max = 15;
        break;
    default:
        if (periodInt < 100)
        {
            min = GetLowerBoundReselCounter(periodInt);
            max = GetUpperBoundReselCounter(periodInt);
        }
        else
        {
            NS_FATAL_ERROR("VALUE NOT SUPPORTED!");
        }
        break;
    }

    NS_LOG_DEBUG("Range to choose random reselection counter. min: " << +min << " max: " << +max);
    return m_ueSelectedUniformVariable->GetInteger(min, max);
}

uint8_t
NrSlUeMacSchedulerDefault::GetLowerBoundReselCounter(uint16_t pRsrv) const
{
    NS_ASSERT_MSG(pRsrv < 100, "Resource reservation must be less than 100 ms");
    uint8_t lBound = (5 * std::ceil(100 / (std::max(static_cast<uint16_t>(20), pRsrv))));
    return lBound;
}

uint8_t
NrSlUeMacSchedulerDefault::GetUpperBoundReselCounter(uint16_t pRsrv) const
{
    NS_ASSERT_MSG(pRsrv < 100, "Resource reservation must be less than 100 ms");
    uint8_t uBound = (15 * std::ceil(100 / (std::max(static_cast<uint16_t>(20), pRsrv))));
    return uBound;
}

void
NrSlUeMacSchedulerDefault::DoSchedNrSlTriggerReq(const SfnSf& sfn)
{
    NS_LOG_FUNCTION(this << sfn);

    MakeSpsReselectionDecisions();

    if (!GetMacHarq()->GetNumAvailableHarqIds())
    {
        // Cannot create new grants at this time but there may be existing
        // ones to publish
        CheckForGrantsToPublish(sfn);
        return;
    }
    m_supplementalLcIds.clear();

    // 1. Obtain which destinations and logical channels are in need of scheduling
    std::map<uint32_t, std::vector<uint8_t>> dstsAndLcsToSched;
    GetDstsAndLcsNeedingScheduling(sfn, dstsAndLcsToSched);
    if (dstsAndLcsToSched.size() > 0)
    {
        NS_LOG_DEBUG("There are " << dstsAndLcsToSched.size()
                                  << " destinations needing scheduling");

        // 2. Allocate as much of the destinations and logical channels as possible,
        //    following the Logical Channel Prioritization (LCP) procedure
        while (dstsAndLcsToSched.size() > 0)
        {
            AllocationInfo allocationInfo;
            std::list<SlResourceInfo> candResources;
            auto dstL2IdToServe =
                LogicalChannelPrioritization(sfn, dstsAndLcsToSched, allocationInfo, candResources);

            NS_LOG_DEBUG(
                "Destination L2 Id to allocate: "
                << (dstL2IdToServe.has_value() ? std::to_string(dstL2IdToServe.value()) : " NONE")
                << " Number of LCs: " << allocationInfo.m_allocatedRlcPdus.size() << " Priority: "
                << +allocationInfo.m_priority << " Is dynamic: " << allocationInfo.m_isDynamic
                << " TB size: " << allocationInfo.m_tbSize
                << " HARQ enabled: " << allocationInfo.m_harqEnabled);
            NS_LOG_DEBUG("Resources available (" << candResources.size() << "):");
            for (auto itCandResou : candResources)
            {
                NS_LOG_DEBUG(itCandResou.sfn
                             << " slSubchannelStart: " << +itCandResou.slSubchannelStart
                             << " slSubchannelSize:" << itCandResou.slSubchannelSize);
            }
            if (dstL2IdToServe.has_value())
            {
                uint32_t dstL2Id = dstL2IdToServe.value();
                if (candResources.size() > 0 && allocationInfo.m_allocatedRlcPdus.size() > 0)
                {
                    auto allocationMade =
                        AttemptGrantAllocation(sfn, dstL2Id, candResources, allocationInfo);
                    if (allocationMade)
                    {
                        RemoveServedLcsFromSchedulingMap(dstL2Id,
                                                         allocationInfo,
                                                         dstsAndLcsToSched);
                    }
                    else
                    {
                        NS_LOG_DEBUG(
                            "Unable to allocate destination; AttemptGrantAllocation failed to "
                            << dstL2Id);
                        break;
                    }
                }
                else
                {
                    NS_LOG_DEBUG("Unable to allocate destination " << dstL2Id);
                    // It could happen that we are not able to serve this destination
                    // but could serve any of the other destinations needing scheduling.
                    // This case is not currently considered and we stop trying to allocate
                    // destinations at the first one we are not able to serve.
                    break;
                }
            }
            else
            {
                NS_LOG_DEBUG("No destination found to serve");
                break;
            }
        }
    }
    else
    {
        NS_LOG_DEBUG("No destination needing scheduling");
    }
    // Do not immediately publish grants, because when using global knowledge, a grant could
    // be created and immediately published before another scheduler has a chance to look at it.
    // ScheduleNow() causes an event to be appended to the end of the event list for this time
    Simulator::ScheduleNow(&NrSlUeMacSchedulerDefault::CheckForGrantsToPublish, this, sfn);
}

void
NrSlUeMacSchedulerDefault::DoNotifyNrSlRlcPduDequeue(uint32_t dstL2Id, uint8_t lcId, uint32_t size)
{
    NS_LOG_FUNCTION(this << dstL2Id << +lcId << size);

    const auto itDstInfo = m_dstMap.find(dstL2Id);
    const auto& lcgMap = itDstInfo->second->GetNrSlLCG();
    lcgMap.begin()->second->AssignedData(lcId, size);

    return;
}

bool
NrSlUeMacSchedulerDefault::TxResourceReselectionCheck(const SfnSf& sfn,
                                                      uint32_t dstL2Id,
                                                      uint8_t lcId)
{
    NS_LOG_FUNCTION(this << sfn << dstL2Id << +lcId);
    const auto itDstInfo = m_dstMap.find(dstL2Id);
    const auto& lcgMap = itDstInfo->second->GetNrSlLCG();

    bool isLcDynamic = lcgMap.begin()->second->IsLcDynamic(lcId);
    uint32_t lcBufferSize = lcgMap.begin()->second->GetTotalSizeOfLC(lcId);
    NS_LOG_DEBUG("LcId " << +lcId << " buffer size " << lcBufferSize);
    if (lcBufferSize == 0)
    {
        NS_LOG_DEBUG("Didn't pass, Empty buffer");
        return false;
    }
    auto grantSearch = FindGrantForLc(sfn, dstL2Id, lcId);
    bool pass = false;
    if (isLcDynamic)
    {
        pass = CheckDynamicLcNeedsScheduling(dstL2Id, lcId, lcBufferSize, grantSearch.foundForDest);
    }
    else // SPS
    {
        auto itGrantInfo = m_grantInfo.find(dstL2Id);
        auto itGrantFoundLc = grantSearch.grant;
        bool grantFoundForLc = grantSearch.found;
        if (grantFoundForLc && !itGrantFoundLc->isDynamic &&
            itGrantFoundLc->slotAllocations.empty() && itGrantFoundLc->slResoReselCounter > 0)
        {
            // All slot allocations are over but the counter > 0; some
            // transmissions must have been missed due to an empty buffer.
            // Strictly following TS 38.321 means that resources cannot be kept.
            // Shorten the HARQ timer to free the process ID after in-flight
            // transmissions or feedback is complete.
            NS_LOG_INFO("Erasing SPS grant: slots exhausted with counter "
                        << +itGrantFoundLc->slResoReselCounter);
            auto harqId = itGrantFoundLc->harqId;
            Time deallocTime = CalculateProcessTimeout(sfn,
                                                       itGrantFoundLc->lastGrantedSlotSfn,
                                                       itGrantFoundLc->harqEnabled,
                                                       GetMac()->GetPsfchPeriod());
            // The ID may have been recycled if the HARQ timer expired naturally
            // after all slots were consumed; skip renewal to avoid corrupting a new grant.
            if (deallocTime > GetMac()->GetSlotPeriod())
            {
                GetMacHarq()->RenewHarqProcessIdTimer(harqId, deallocTime);
            }
            itGrantInfo->second.erase(itGrantFoundLc);
            if (lcBufferSize > 0)
            {
                pass = true;
            }
        }
        else if (grantSearch.mcsChanged)
        {
            NS_LOG_DEBUG("MCS has changed to " << +itDstInfo->second->GetDstMcs()
                                               << "; forcing a reselection");
            // Give time to the already granted slots to be transmitted and their feedback
            // (if any) to be received before deallocating the HARQ process ID
            Time deallocTime = CalculateProcessTimeout(sfn,
                                                       itGrantFoundLc->lastGrantedSlotSfn,
                                                       itGrantFoundLc->harqEnabled,
                                                       GetMac()->GetPsfchPeriod());
            // The last granted slot may already be in the past if slots were exhausted
            // before MCS change was detected; skip renewal to avoid corrupting a recycled ID.
            if (deallocTime > GetMac()->GetSlotPeriod())
            {
                GetMacHarq()->RenewHarqProcessIdTimer(itGrantFoundLc->harqId, deallocTime);
            }
            RemoveUnpublishedGrants(lcId, dstL2Id);
            pass = true;
        }
        else if (grantFoundForLc && itGrantFoundLc->slResoReselCounter == 0)
        {
            // The keep/reselect decision was made at counter == 1
            // (see below) and stored in pendingReselection.
            // This runs regardless of buffer status because SPS grant
            // lifecycle is independent of buffer.
            if (itGrantFoundLc->pendingReselection)
            {
                // Reselect: Clear the grant, and also conditionally shorten
                // the HARQ timer to still allow granted, but not yet transmitted,
                // slots, and any pending feedback, to complete, then allow the
                // HARQ process timeout to deallocate the ID.
                //
                // The condition on the timer modification is to only call
                // RenewHarqProcessIdTimer() when lastGrantedSlotSfn
                // is still in the future (deallocTime > one slot period).
                // If lastGrantedSlotSfn is in the past, CalculateProcessTimeout()
                // returns one slot period, meaning the HARQ timer for this grant's
                // ID may have expired and the ID may have been reallocated to a
                // different grant.  Calling RenewHarqProcessIdTimer() in that
                // case would overwrite that grant's timer with a near-zero
                // timeout, causing its HARQ process ID to be prematurely released.
                auto harqId = itGrantFoundLc->harqId;
                Time deallocTime = CalculateProcessTimeout(sfn,
                                                           itGrantFoundLc->lastGrantedSlotSfn,
                                                           itGrantFoundLc->harqEnabled,
                                                           GetMac()->GetPsfchPeriod());
                if (deallocTime > GetMac()->GetSlotPeriod())
                {
                    NS_LOG_INFO("Shortening HARQ timer for possibly stale HARQ process "
                                << +harqId);
                    GetMacHarq()->RenewHarqProcessIdTimer(harqId, deallocTime);
                }
                itGrantInfo->second.erase(itGrantFoundLc);
                NS_LOG_INFO("Clearing SPS grant for reselection");
                if (lcBufferSize > 0)
                {
                    pass = true;
                }
            }
            else
            {
                // Keep: regenerate slots with same resource pattern,
                // redraw counter, renew HARQ timer
                itGrantFoundLc->slResoReselCounter =
                    GetRandomReselectionCounter(itGrantFoundLc->rri);
                itGrantFoundLc->cReselCounter = 10 * itGrantFoundLc->slResoReselCounter;
                RegenerateSpsSlotAllocations(sfn, *itGrantFoundLc);
                itGrantFoundLc->allocationTime = Simulator::Now();
                itGrantFoundLc->pendingReselection = false;
                auto timeout = CalculateSpsProcessTimeout(sfn,
                                                          itGrantFoundLc->slResoReselCounter,
                                                          itGrantFoundLc->rri);
                bool renewed =
                    GetMacHarq()->RenewHarqProcessIdTimer(itGrantFoundLc->harqId, timeout);
                if (!renewed)
                {
                    // HARQ process timer expired while waiting; fall back
                    // to reselection so a fresh HARQ ID can be allocated
                    NS_LOG_INFO("HARQ process expired during keep; "
                                "falling back to reselection");
                    itGrantInfo->second.erase(itGrantFoundLc);
                    if (lcBufferSize > 0)
                    {
                        pass = true;
                    }
                }
                else
                {
                    NS_LOG_INFO("Keeping SPS grant, new slResoReselCounter: "
                                << +itGrantFoundLc->slResoReselCounter);
                }
            }
        }
        else if (grantFoundForLc && itGrantFoundLc->slResoReselCounter == 1 &&
                 !itGrantFoundLc->pendingReselection)
        {
            // Per TS 38.321, when counter == 1, decide whether to keep or
            // reselect resources. This runs regardless of buffer status.
            // The guard on pendingReselection prevents re-drawing across
            // multiple slots while counter stays at 1.
            double randomVariate = m_ueSelectedUniformVariable->GetValue(0, 1);
            double slProbResourceKeep = GetMac()->GetSlProbResourceKeep();
            if (slProbResourceKeep <= randomVariate)
            {
                itGrantFoundLc->pendingReselection = true;
                // Signal no further reservations in last SCI 1A
                itGrantFoundLc->rri = Time(0);
                NS_LOG_INFO("Reselection decided at counter=1: "
                            "slProbResourceKeep ("
                            << slProbResourceKeep << ") <= randomVariate (" << randomVariate
                            << ")");
            }
            else
            {
                NS_LOG_INFO("Keep decided at counter=1: "
                            "slProbResourceKeep ("
                            << slProbResourceKeep << ") > randomVariate (" << randomVariate << ")");
                NotifyGrantReused(slProbResourceKeep, randomVariate);
            }
            // After the decision, also check for supplemental grants
            // if buffer has data and supplemental grants are enabled
            if (m_allowSupplementalGrants && lcBufferSize > 0)
            {
                uint32_t allocatedSize = CalculateEffectiveAllocatedCapacityForLc(dstL2Id, lcId);
                if (lcBufferSize > allocatedSize)
                {
                    NS_LOG_INFO("Supplemental dynamic grant needed: buffer "
                                << lcBufferSize << " > allocated " << allocatedSize);
                    m_supplementalLcIds.insert(lcId);
                    pass = true;
                }
                else
                {
                    NS_LOG_DEBUG("Current buffer " << lcBufferSize << " covered by grants "
                                                   << allocatedSize);
                }
            }
        }
        else if (lcBufferSize > 0)
        {
            if (!grantFoundForLc)
            {
                NS_LOG_DEBUG("Passed check, fresh SPS grant required");
                pass = true;
            }
            else if (lcBufferSize >= m_spsReselectionThreshold &&
                     (itGrantFoundLc->allocationTime + itGrantFoundLc->rri) < Simulator::Now())
            {
                NS_LOG_INFO("LC buffer size " << lcBufferSize
                                              << " is at least the reselection threshold "
                                              << m_spsReselectionThreshold
                                              << ", clearing grant, fresh SPS grant required");
                // Give time to the already granted slots to be transmitted
                // and their feedback (if any) to be received before
                // deallocating the HARQ process ID
                Time deallocTime = CalculateProcessTimeout(sfn,
                                                           itGrantFoundLc->lastGrantedSlotSfn,
                                                           itGrantFoundLc->harqEnabled,
                                                           GetMac()->GetPsfchPeriod());
                // The last granted slot may already be in the past if slots were consumed
                // before the threshold triggered; skip renewal to avoid corrupting a recycled ID.
                if (deallocTime > GetMac()->GetSlotPeriod())
                {
                    GetMacHarq()->RenewHarqProcessIdTimer(itGrantFoundLc->harqId, deallocTime);
                }
                RemoveUnpublishedGrants(lcId, dstL2Id);
                pass = true;
            }
            else
            {
                NS_LOG_DEBUG("slResoReselCounter " << +itGrantFoundLc->slResoReselCounter);
                uint32_t allocatedSize = CalculateEffectiveAllocatedCapacityForLc(dstL2Id, lcId);
                if (m_allowSupplementalGrants && lcBufferSize > allocatedSize)
                {
                    NS_LOG_INFO("Supplemental dynamic grant needed: buffer "
                                << lcBufferSize << " > allocated " << allocatedSize);
                    m_supplementalLcIds.insert(lcId);
                    pass = true;
                }
                else
                {
                    NS_LOG_DEBUG("slResoReselCounter != 0, buffer covered by grants");
                }
            }
        }
    }
    if (!pass)
    {
        NS_LOG_DEBUG("Didn't pass the check");
    }

    return pass;
}

std::optional<uint32_t>
NrSlUeMacSchedulerDefault::LogicalChannelPrioritization(
    const SfnSf& sfn,
    std::map<uint32_t, std::vector<uint8_t>> dstsAndLcsToSched,
    AllocationInfo& allocationInfo,
    std::list<SlResourceInfo>& candResources)

{
    std::optional<uint32_t> retVal;
    NS_LOG_FUNCTION(this << dstsAndLcsToSched.size() << candResources.size());

    // The below code checks whether PSFCH is configured for this resource pool.
    // This information is needed later when calculating L_subCH for candidate selection.
    const auto pool = GetMac()->GetTxPool();
    const auto bwpId = GetMac()->GetBwpId();
    const auto poolId = GetMac()->GetSlActivePoolId();
    const auto psfchPeriod = pool->GetPsfchPeriod(bwpId, poolId);
    bool hasPsfch = false;
    if (psfchPeriod)
    {
        NS_LOG_DEBUG("PSFCH enabled on BWP ID " << +bwpId << " pool ID " << poolId);
        hasPsfch = true;
    }
    else
    {
        NS_LOG_DEBUG("PSFCH disbled on BWP ID " << +bwpId << " pool ID " << poolId);
    }
    // Next, calculate the minimum symbols per slot, used for TB size estimation.
    // This scheduler, when requesting candidate resources, must calculate the
    // required number of subchannels, L_subCH, and in doing so, needs to estimate
    // the transport block size that would corresopnd to each possible value of the
    // number of subchannels. If PSFCH is enabled, and the PSFCH period is either 2 or 4,
    // some slots will have more available PSSCH symbols than others.  This scheduler will
    // conservatively estimate, during resource selection, the minimum number of symbols
    // to calculate L_subCH.  Later, once the resource candidates are selected, the
    // scheduler can take into consideration the actual PSSCH size of each slot when
    // selecting from among the candidates.
    uint16_t minSymbolsPerSlot = pool->GetPsschSymLength(bwpId, poolId, hasPsfch);
    uint16_t subChannelSizeInRbs = GetMac()->GetNrSlSubChSize();

    // 1. Selection of destination and logical channels to allocate
    auto dstSelected = SelectDestinationByPriority(dstsAndLcsToSched);
    if (!dstSelected.has_value())
    {
        return retVal;
    }
    uint32_t dstIdSelected = dstSelected.value();

    auto lcSelection = SelectAndFilterLcs(dstIdSelected,
                                          dstsAndLcsToSched,
                                          allocationInfo,
                                          minSymbolsPerSlot,
                                          hasPsfch);
    auto itDstInfo = m_dstMap.find(dstIdSelected);
    const auto& lcgMap = itDstInfo->second->GetNrSlLCG();
    uint8_t lcIdOfRef = lcSelection.lcIdOfRef;
    bool isSupplemental = lcSelection.isSupplemental;

    // 2. Allocation of sidelink resources
    NS_LOG_DEBUG("Getting resources");
    // 2.1 Select which logical channels can be allocated
    std::map<uint8_t, std::vector<uint8_t>> selectedLcs = lcSelection.lcIdsByPrio;
    // Collect all LC IDs being scheduled (selectedLcs is erased during the loop below)
    std::set<uint8_t> scheduledLcIds;
    for (const auto& entry : selectedLcs)
    {
        for (const auto& lcId : entry.second)
        {
            scheduledLcIds.insert(lcId);
        }
    }
    std::queue<std::vector<uint8_t>> allocQueue;
    uint32_t bufferSize = 0;
    uint32_t nLcsInQueue = 0;
    uint32_t candResoTbSize = 0;
    auto rItSelectedLcs = selectedLcs.rbegin(); // reverse iterator
    while (selectedLcs.size() > 0)
    {
        allocQueue.emplace(rItSelectedLcs->second);
        // Calculate buffer size of LCs just pushed in the queue
        uint32_t currBufferSize = 0;
        for (auto& itLc : rItSelectedLcs->second)
        {
            currBufferSize = currBufferSize + lcgMap.begin()->second->GetTotalSizeOfLC(itLc);
        }
        nLcsInQueue = nLcsInQueue + rItSelectedLcs->second.size();
        // Calculate buffer size of all LCs currently in the queue
        bufferSize = bufferSize + currBufferSize;

        // For dynamic grants (including supplemental), subtract data already
        // allocated to existing grants for the same LCs.  This prevents
        // over-allocation when re-triggered by TxResourceReselectionCheck.
        if (allocationInfo.m_isDynamic)
        {
            uint32_t allocatedSize = 0;
            auto itGrants = m_grantInfo.find(dstIdSelected);
            if (itGrants != m_grantInfo.end())
            {
                for (const auto& grant : itGrants->second)
                {
                    if (!grant.slotAllocations.empty())
                    {
                        for (const auto& rlcPdu : grant.slotAllocations.begin()->slRlcPduInfo)
                        {
                            if (scheduledLcIds.count(rlcPdu.lcid))
                            {
                                // Reduce effective drain per grant by RLC_HEADER_SIZE
                                // because segmentation across multiple TBs introduces
                                // per-segment RLC headers that were not accounted for
                                // in the original buffer status report from the RLC.
                                uint32_t effectiveSize = rlcPdu.size > RLC_HEADER_SIZE
                                                             ? rlcPdu.size - RLC_HEADER_SIZE
                                                             : 0;
                                allocatedSize += effectiveSize;
                            }
                        }
                    }
                }
            }
            NS_LOG_DEBUG("Dynamic: adjusting buffer from " << bufferSize << " by subtracting "
                                                           << allocatedSize
                                                           << " already allocated");
            bufferSize = bufferSize > allocatedSize ? bufferSize - allocatedSize : 0;
        }

        // Enforce SPS minimum grant size
        if (!allocationInfo.m_isDynamic && bufferSize < m_minimumSpsGrantSize)
        {
            NS_LOG_DEBUG("Increasing calculated buffer size " << bufferSize << " to minimum "
                                                              << m_minimumSpsGrantSize);
            bufferSize = m_minimumSpsGrantSize;
        }

        // Notify any MCS controllers that the MCS for a destination is about to be fetched
        if (!m_mcsQueryIndicationCb.IsNull())
        {
            // We don't know yet what TB size is needed, but conservatively estimate it
            // based on the maximum TB size that could be selected below
            // Assume SUBHEADER_SIZE per LC
            // Conservatively assume the minimum number of symbols per slot
            uint32_t maxTbSize =
                std::min<uint32_t>(bufferSize + SUBHEADER_SIZE,
                                   subChannelSizeInRbs * GetTotalSubCh() * minSymbolsPerSlot);
            m_mcsQueryIndicationCb(sfn,
                                   dstIdSelected,
                                   maxTbSize,
                                   subChannelSizeInRbs * GetTotalSubCh());
        }
        // Calculate the number of subchannels needed to serve (bufferSize + SUBHEADER_SIZE) bytes
        uint8_t dstMcs = itDstInfo->second->GetDstMcs();
        auto slotInfo = CreateSlotInfo(minSymbolsPerSlot, hasPsfch);
        auto minSubchannels = GetMac()->GetTxPool()->GetMinSubchannels(slotInfo,
                                                                       dstMcs,
                                                                       bufferSize + SUBHEADER_SIZE,
                                                                       GetTotalSubCh(),
                                                                       MCS_TABLE);
        uint16_t lSubch = minSubchannels.has_value() ? minSubchannels.value() : GetTotalSubCh();
        // Resolve the number of subchannels to a transport block size
        // This tbSize will be greater than or equal to (buffer size + SUBHEADER_SIZE)
        uint16_t tbSize =
            GetMac()->GetTxPool()->GetTransportBlockSize(slotInfo, dstMcs, lSubch, MCS_TABLE);

        NS_LOG_DEBUG("Trying " << nLcsInQueue << " LCs with total buffer size of " << bufferSize
                               << " bytes in " << lSubch << " subchannels for a TB size of "
                               << tbSize);

        // All LCs in the set should have the same attributes as the lcIdOfRef
        m_transmissionParams.m_priority = lcgMap.begin()->second->GetLcPriority(lcIdOfRef);
        m_transmissionParams.m_packetDelayBudget = lcgMap.begin()->second->GetLcPdb(lcIdOfRef);
        m_transmissionParams.m_lSubch = lSubch;
        if (isSupplemental)
        {
            m_transmissionParams.m_pRsvpTx = Time(0); // One-shot, no reservation
            m_transmissionParams.m_cResel = 0;
        }
        else
        {
            m_transmissionParams.m_pRsvpTx = lcgMap.begin()->second->GetLcRri(lcIdOfRef);
            m_transmissionParams.m_cResel = 10 * allocationInfo.m_reselCounter;
        }
        // GetCandidateResources() will return the set S_A defined in
        // sec. 8.1.4 of TS 38.214.  The scheduler is responsible for
        // further filtering out any candidates that overlap with already
        // scheduled grants within the selection window.
        m_candidateResources =
            GetMac()->GetCandidateResources(sfn, m_transmissionParams, m_selectionParams);
        Time filterRri = isSupplemental ? Time(0) : lcgMap.begin()->second->GetLcRri(lcIdOfRef);
        uint16_t filterCResel = isSupplemental ? 0 : 10 * allocationInfo.m_reselCounter;
        candResources = FilterTxOpportunities(sfn, m_candidateResources, filterRri, filterCResel);
        if (!candResources.size())
        {
            NS_LOG_DEBUG("Resources not found");
            break;
        }
        else
        {
            NS_LOG_DEBUG("Resources found");
            candResoTbSize = tbSize;
        }
        rItSelectedLcs = std::reverse_iterator(selectedLcs.erase(--rItSelectedLcs.base()));
    }
    if (!candResources.size())
    {
        NS_LOG_DEBUG("Unable to find resources");
        return retVal;
    }
    allocationInfo.m_tbSize = candResoTbSize;

    // For dynamic grants, calculate number of TBs needed to drain the buffer
    if (allocationInfo.m_isDynamic)
    {
        allocationInfo.m_numTBs = CalculateNumTbNeeded(bufferSize, candResoTbSize);
        NS_LOG_DEBUG("Dynamic grant needs " << allocationInfo.m_numTBs << " TBs for buffer of "
                                            << bufferSize << " bytes");
    }
    else
    {
        allocationInfo.m_numTBs = 1; // SPS grants use single TB
    }

    NS_LOG_DEBUG("Destination L2 ID " << dstIdSelected << " got " << candResources.size()
                                      << " resources (of TB size " << candResoTbSize << ")"
                                      << " available to allocate " << nLcsInQueue
                                      << " LCs with total buffer size of " << bufferSize
                                      << " bytes");

    // 2.2 Allocate the resources to logical channels
    AllocateLcsToResources(allocQueue, candResoTbSize, dstIdSelected, allocationInfo);

    return dstIdSelected;
}

void
NrSlUeMacSchedulerDefault::GetDstsAndLcsNeedingScheduling(
    const SfnSf& sfn,
    std::map<uint32_t, std::vector<uint8_t>>& dstsAndLcsToSched)
{
    NS_LOG_FUNCTION(this << sfn);
    for (auto& itDstInfo : m_dstMap)
    {
        const auto& lcgMap = itDstInfo.second->GetNrSlLCG(); // Map of unique_ptr should not copy
        std::vector<uint8_t> lcVector = lcgMap.begin()->second->GetLCId();
        std::vector<uint8_t> passedLcsVector;
        for (auto& itLcId : lcVector)
        {
            if (TxResourceReselectionCheck(sfn, itDstInfo.first, itLcId))
            {
                passedLcsVector.emplace_back(itLcId);
            }
        }
        if (passedLcsVector.size() > 0)
        {
            dstsAndLcsToSched.emplace(itDstInfo.first, passedLcsVector);
        }
        NS_LOG_DEBUG("Destination L2 ID " << itDstInfo.first << " has " << passedLcsVector.size()
                                          << " LCs needing scheduling");
    }
}

bool
NrSlUeMacSchedulerDefault::AttemptGrantAllocation(const SfnSf& sfn,
                                                  uint32_t dstL2Id,
                                                  const std::list<SlResourceInfo>& candResources,
                                                  const AllocationInfo& allocationInfo)
{
    NS_LOG_FUNCTION(this << sfn << dstL2Id);

    const auto itDstInfo = m_dstMap.find(dstL2Id);

    // For SPS grants, create a single grant
    if (!allocationInfo.m_isDynamic)
    {
        std::set<SlGrantResource> allocList;
        bool allocated =
            DoNrSlAllocation(candResources, itDstInfo->second, allocList, allocationInfo);
        if (!allocated)
        {
            return false;
        }
        CreateSpsGrant(sfn, allocList, allocationInfo);
        return true;
    }

    // For dynamic grants, potentially create multiple grants
    uint32_t numGrantsToCreate = allocationInfo.m_numTBs;
    uint32_t availableHarqIds = GetMacHarq()->GetNumAvailableHarqIds();
    NS_LOG_DEBUG("Number of dynamic grants to create: "
                 << numGrantsToCreate << " available HARQ IDs: " << availableHarqIds);

    if (numGrantsToCreate > availableHarqIds)
    {
        NS_LOG_INFO("Partial allocation: need " << numGrantsToCreate << " grants but only "
                                                << availableHarqIds << " HARQ IDs available");
        numGrantsToCreate = availableHarqIds;
    }

    if (!numGrantsToCreate)
    {
        return false;
    }

    std::list<SlResourceInfo> remainingResources = candResources;
    uint32_t grantsCreated = 0;

    for (uint32_t i = 0; i < numGrantsToCreate && !remainingResources.empty(); ++i)
    {
        std::set<SlGrantResource> allocList;
        bool allocated =
            DoNrSlAllocation(remainingResources, itDstInfo->second, allocList, allocationInfo);
        if (!allocated)
        {
            NS_LOG_DEBUG("Could not allocate grant " << (i + 1) << " of " << numGrantsToCreate);
            break;
        }

        // Update m_candidateResources so that the SchedulingReport trace reflects the
        // current candidate set for this grant
        m_candidateResources = remainingResources;

        CreateSinglePduGrant(sfn, allocList, allocationInfo);
        grantsCreated++;

        // Remove used resources from candidates for next iteration
        remainingResources = FilterUsedResources(remainingResources, allocList);
    }

    NS_LOG_INFO("Created " << grantsCreated << " of " << allocationInfo.m_numTBs
                           << " needed grants for destination " << dstL2Id);

    return grantsCreated > 0;
}

Time
NrSlUeMacSchedulerDefault::CalculateSpsProcessTimeout(const SfnSf& sfn,
                                                      uint8_t resoReselCounter,
                                                      Time rri) const
{
    NS_LOG_FUNCTION(this << sfn << +resoReselCounter << rri.As(Time::MS));
    auto timePerSlot = MicroSeconds(1000 >> sfn.GetNumerology());
    // Set a conservative timeout value.  The grant will be reselected
    // at (resoReselCounter * RRI) in the future, and add one more
    // RRI to this value to prevent cases where the HARQ process ID timer
    // expires just before the scheduler was about to renew it.
    auto timeout = rri * (resoReselCounter + 1);
    return timeout;
}

void
NrSlUeMacSchedulerDefault::MakeSpsReselectionDecisions()
{
    NS_LOG_FUNCTION(this);
    for (auto& [dstL2Id, grants] : m_grantInfo)
    {
        for (auto& grant : grants)
        {
            if (grant.isDynamic || grant.slResoReselCounter != 1 || grant.pendingReselection)
            {
                continue;
            }
            double randomVariate = m_ueSelectedUniformVariable->GetValue(0, 1);
            double slProbResourceKeep = GetMac()->GetSlProbResourceKeep();
            if (slProbResourceKeep <= randomVariate)
            {
                grant.pendingReselection = true;
                grant.rri = Time(0);
                NS_LOG_INFO("Reselection decided at counter=1: slProbResourceKeep ("
                            << slProbResourceKeep << ") <= randomVariate (" << randomVariate
                            << ")");
            }
            else
            {
                NS_LOG_INFO("Keep decided at counter=1: slProbResourceKeep ("
                            << slProbResourceKeep << ") > randomVariate (" << randomVariate << ")");
                NotifyGrantReused(slProbResourceKeep, randomVariate);
            }
        }
    }
}

GrantSearchResult
NrSlUeMacSchedulerDefault::FindGrantForLc(const SfnSf& sfn, uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this << sfn << dstL2Id << +lcId);
    GrantSearchResult result;

    const auto itGrantInfo = m_grantInfo.find(dstL2Id);
    result.foundForDest = itGrantInfo != m_grantInfo.end();
    if (!result.foundForDest)
    {
        return result;
    }

    const auto itDstInfo = m_dstMap.find(dstL2Id);
    bool isLcDynamic = itDstInfo->second->GetNrSlLCG().begin()->second->IsLcDynamic(lcId);

    for (auto itGrantVector = itGrantInfo->second.begin();
         itGrantVector != itGrantInfo->second.end();
         ++itGrantVector)
    {
        if (itGrantVector->slotAllocations.size() == 0)
        {
            // For SPS grants at counter == 0, slotAllocations is empty
            // but the grant still exists awaiting keep/reselect processing.
            // Use baseSlotPattern to find the LC match.
            if (!itGrantVector->isDynamic && !itGrantVector->baseSlotPattern.empty())
            {
                for (const auto& it : itGrantVector->baseSlotPattern.begin()->slRlcPduInfo)
                {
                    if (it.lcid == lcId)
                    {
                        result.found = true;
                        break;
                    }
                }
                if (result.found)
                {
                    result.grant = itGrantVector;
                    break;
                }
            }
            continue;
        }
        // Look if any of the RLC PDUs correspond to the LCID
        for (const auto& it : itGrantVector->slotAllocations.begin()->slRlcPduInfo)
        {
            if (it.lcid == lcId)
            {
                NS_LOG_DEBUG("LcId " << +lcId << " already has a grant ");
                result.found = true;
                if (!m_mcsQueryIndicationCb.IsNull() && !isLcDynamic)
                {
                    // Check if MCS to the destination needs to be updated
                    // which would force an SPS grant reselection
                    m_mcsQueryIndicationCb(
                        sfn,
                        dstL2Id,
                        itGrantVector->slotAllocations.begin()->slRlcPduInfo.size(),
                        GetMac()->GetNrSlSubChSize() *
                            itGrantVector->slotAllocations.begin()->slPsschSubChLength);
                }
                if (itGrantVector->slotAllocations.begin()->mcs != itDstInfo->second->GetDstMcs())
                {
                    result.mcsChanged = true;
                }
                break;
            }
        }
        if (result.found)
        {
            result.grant = itGrantVector;
            break;
        }
    }
    return result;
}

bool
NrSlUeMacSchedulerDefault::CheckDynamicLcNeedsScheduling(uint32_t dstL2Id,
                                                         uint8_t lcId,
                                                         uint32_t lcBufferSize,
                                                         bool grantFoundForDest) const
{
    NS_LOG_FUNCTION(this << dstL2Id << +lcId << lcBufferSize << grantFoundForDest);
    if (lcBufferSize == 0)
    {
        return false;
    }
    if (!grantFoundForDest)
    {
        NS_LOG_DEBUG("Passed check, new dynamic grant required (no grants for destination)");
        return true;
    }
    // Calculate how much data is already allocated to existing dynamic grants
    uint32_t allocatedSize = 0;
    auto itGrantInfo = m_grantInfo.find(dstL2Id);
    for (const auto& grant : itGrantInfo->second)
    {
        if (grant.isDynamic && !grant.slotAllocations.empty())
        {
            for (const auto& rlcPdu : grant.slotAllocations.begin()->slRlcPduInfo)
            {
                if (rlcPdu.lcid == lcId)
                {
                    allocatedSize += rlcPdu.size;
                }
            }
        }
    }
    if (lcBufferSize > allocatedSize)
    {
        NS_LOG_DEBUG("Passed check, dynamic grant required for unallocated data ("
                     << lcBufferSize << " bytes in buffer, " << allocatedSize
                     << " bytes allocated)");
        return true;
    }
    NS_LOG_DEBUG("Buffer (" << lcBufferSize << " bytes) fully allocated to grants ("
                            << allocatedSize << " bytes)");
    return false;
}

uint32_t
NrSlUeMacSchedulerDefault::CalculateEffectiveAllocatedCapacityForLc(uint32_t dstL2Id,
                                                                    uint8_t lcId) const
{
    NS_LOG_FUNCTION(this << dstL2Id << +lcId);
    uint32_t allocatedSize = 0;
    auto itGrants = m_grantInfo.find(dstL2Id);
    if (itGrants == m_grantInfo.end())
    {
        return 0;
    }
    for (const auto& grant : itGrants->second)
    {
        if (!grant.slotAllocations.empty())
        {
            const auto& rlcPdus = grant.slotAllocations.begin()->slRlcPduInfo;
            uint32_t numLcs = rlcPdus.size();
            uint32_t macHeaderOverhead = SUBHEADER_SIZE * numLcs;
            uint32_t totalPayload =
                grant.tbSize > macHeaderOverhead ? grant.tbSize - macHeaderOverhead : 0;
            uint32_t otherLcTotal = 0;
            for (const auto& rlcPdu : rlcPdus)
            {
                if (rlcPdu.lcid != lcId)
                {
                    otherLcTotal += rlcPdu.size;
                }
            }
            allocatedSize += totalPayload > otherLcTotal ? totalPayload - otherLcTotal : 0;
        }
    }
    return allocatedSize;
}

std::optional<uint32_t>
NrSlUeMacSchedulerDefault::SelectDestinationByPriority(
    const std::map<uint32_t, std::vector<uint8_t>>& dstsAndLcsToSched)
{
    NS_LOG_FUNCTION(this << dstsAndLcsToSched.size());
    if (dstsAndLcsToSched.empty())
    {
        return std::nullopt;
    }

    std::map<uint8_t, std::vector<uint32_t>> dstL2IdsByPrio;
    for (const auto& itDst : dstsAndLcsToSched)
    {
        uint8_t lcHighestPrio = 0;
        auto itDstInfo = m_dstMap.find(itDst.first);
        const auto& lcgMap = itDstInfo->second->GetNrSlLCG();
        for (const auto& itLc : itDst.second)
        {
            uint8_t lcPriority = lcgMap.begin()->second->GetLcPriority(itLc);
            NS_LOG_DEBUG("Destination L2 ID "
                         << itDst.first << " LCID " << +itLc << " priority " << +lcPriority
                         << " buffer size " << lcgMap.begin()->second->GetTotalSizeOfLC(itLc)
                         << " dynamic scheduling " << lcgMap.begin()->second->IsLcDynamic(itLc)
                         << " RRI " << (lcgMap.begin()->second->GetLcRri(itLc)).GetMilliSeconds()
                         << " ms");
            if (lcPriority > lcHighestPrio)
            {
                lcHighestPrio = lcPriority;
            }
        }
        auto itDstL2IdsByPrio = dstL2IdsByPrio.find(lcHighestPrio);
        if (itDstL2IdsByPrio == dstL2IdsByPrio.end())
        {
            std::vector<uint32_t> dstIds;
            dstIds.emplace_back(itDst.first);
            dstL2IdsByPrio.emplace(lcHighestPrio, dstIds);
        }
        else
        {
            itDstL2IdsByPrio->second.emplace_back(itDst.first);
        }
    }
    // std::map sorts by key, so rbegin() gives the highest priority value
    uint8_t dstHighestPrio = dstL2IdsByPrio.rbegin()->first;
    NS_ASSERT_MSG(dstL2IdsByPrio.rbegin()->second.size(), "Unexpected empty vector");
    // Select a dstL2Id randomly
    uint32_t randomIndex =
        m_destinationUniformVariable->GetInteger(0, dstL2IdsByPrio.rbegin()->second.size() - 1);
    uint32_t dstIdSelected = dstL2IdsByPrio.rbegin()->second.at(randomIndex);
    NS_LOG_INFO("Selected dstL2ID "
                << dstIdSelected << " (" << dstL2IdsByPrio.rbegin()->second.size() << "/"
                << dstsAndLcsToSched.size() << " destinations with highest LC priority of "
                << +dstHighestPrio << ")");
    return dstIdSelected;
}

void
NrSlUeMacSchedulerDefault::AllocateLcsToResources(std::queue<std::vector<uint8_t>>& allocQueue,
                                                  uint32_t tbPayloadCapacity,
                                                  uint32_t dstL2Id,
                                                  AllocationInfo& allocationInfo)
{
    NS_LOG_FUNCTION(this << tbPayloadCapacity << dstL2Id);
    auto itDstInfo = m_dstMap.find(dstL2Id);
    const auto& lcgMap = itDstInfo->second->GetNrSlLCG();

    uint32_t allocatedSize = 0;
    while (allocQueue.size() > 0)
    {
        // All LCs of the same priority are served equally
        // Find how much to allocate to each
        uint32_t minBufferSize = std::numeric_limits<uint32_t>::max();
        uint32_t toServeBufferSize = 0;
        for (auto itLc : allocQueue.front())
        {
            if (lcgMap.begin()->second->GetTotalSizeOfLC(itLc) < minBufferSize)
            {
                minBufferSize = lcgMap.begin()->second->GetTotalSizeOfLC(itLc);
            }
        }
        // Cap the per-LC allocation so that the total across all LCs of this
        // priority level does not exceed the remaining TB payload capacity.
        // - tbPayloadCapacity: raw TB size from MCS and subchannel selection
        // - SUBHEADER_SIZE: Assumed MAC subheader overhead (not available for RLC data)
        // - allocatedSize: bytes already allocated to higher-priority LCs
        // - allocQueue.front().size(): number of LCs at this priority level
        toServeBufferSize = minBufferSize;
        if (allocQueue.front().size() * toServeBufferSize >
            tbPayloadCapacity - SUBHEADER_SIZE - allocatedSize)
        {
            // If the total would exceed capacity, toServeBufferSize is reduced
            // so that the remaining capacity is divided equally among the
            // allocQueue.front().size() LCs at this priority level.
            toServeBufferSize = std::floor((tbPayloadCapacity - SUBHEADER_SIZE - allocatedSize) /
                                           allocQueue.front().size());
        }
        if (toServeBufferSize > 0)
        {
            for (auto itLc : allocQueue.front())
            {
                SlRlcPduInfo slRlcPduInfo(itLc, toServeBufferSize);
                allocationInfo.m_allocatedRlcPdus.push_back(slRlcPduInfo);
                NS_LOG_INFO("LC ID " << +itLc << " Dst L2ID " << dstL2Id << " allocated "
                                     << toServeBufferSize << " bytes");
                allocatedSize = allocatedSize + toServeBufferSize;
            }
        }
        else
        {
            break;
        }

        allocQueue.pop();
    }
}

void
NrSlUeMacSchedulerDefault::RemoveServedLcsFromSchedulingMap(
    uint32_t dstL2Id,
    const AllocationInfo& allocationInfo,
    std::map<uint32_t, std::vector<uint8_t>>& dstsAndLcsToSched)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    auto itDstsAndLcsToSched = dstsAndLcsToSched.find(dstL2Id);
    if (itDstsAndLcsToSched == dstsAndLcsToSched.end())
    {
        return;
    }
    if (allocationInfo.m_allocatedRlcPdus.size() == itDstsAndLcsToSched->second.size())
    {
        NS_LOG_DEBUG("All logical channels of destination " << dstL2Id << " were allocated");
        dstsAndLcsToSched.erase(dstL2Id);
    }
    else
    {
        NS_LOG_DEBUG("Only " << allocationInfo.m_allocatedRlcPdus.size() << "/"
                             << itDstsAndLcsToSched->second.size()
                             << " logical channels of destination " << dstL2Id
                             << " were allocated");
        for (const auto& slRlcPduInfo : allocationInfo.m_allocatedRlcPdus)
        {
            auto itLcs = itDstsAndLcsToSched->second.begin();
            while (itLcs != itDstsAndLcsToSched->second.end())
            {
                if (*itLcs == slRlcPduInfo.lcid)
                {
                    NS_LOG_DEBUG("Erasing LCID " << slRlcPduInfo.lcid);
                    itLcs = itDstsAndLcsToSched->second.erase(itLcs);
                }
                else
                {
                    ++itLcs;
                }
            }
        }
    }
}

LcSelectionResult
NrSlUeMacSchedulerDefault::SelectAndFilterLcs(
    uint32_t dstL2Id,
    const std::map<uint32_t, std::vector<uint8_t>>& dstsAndLcsToSched,
    AllocationInfo& allocationInfo,
    uint16_t minSymbolsPerSlot,
    bool hasPsfch)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    LcSelectionResult result;

    auto itDstInfo = m_dstMap.find(dstL2Id);
    uint8_t dstMcs = itDstInfo->second->GetDstMcs();
    const auto& lcgMap = itDstInfo->second->GetNrSlLCG();
    const auto& itDst = dstsAndLcsToSched.find(dstL2Id);
    for (const auto& itLc : itDst->second)
    {
        uint8_t lcPriority = lcgMap.begin()->second->GetLcPriority(itLc);
        auto itLcIdsByPrio = result.lcIdsByPrio.find(lcPriority);
        if (itLcIdsByPrio == result.lcIdsByPrio.end())
        {
            std::vector<uint8_t> lcIds;
            lcIds.emplace_back(itLc);
            result.lcIdsByPrio.emplace(lcPriority, lcIds);
        }
        else
        {
            itLcIdsByPrio->second.emplace_back(itLc);
        }
    }

    // Verify type of scheduling of LCs with highest priority (the one at the rear of the map)
    bool dynamicGrant = true;
    uint16_t nDynLcs = 0;
    uint16_t nSpsLcs = 0;
    if (result.lcIdsByPrio.rbegin()->second.size() > 1)
    {
        for (const auto& itLcsHighestPrio : result.lcIdsByPrio.rbegin()->second)
        {
            if (lcgMap.begin()->second->IsLcDynamic(itLcsHighestPrio))
            {
                nDynLcs++;
            }
            else
            {
                nSpsLcs++;
            }
        }
        if ((m_prioToSps && nSpsLcs > 0) || (!m_prioToSps && nDynLcs == 0 && nSpsLcs > 0))
        {
            dynamicGrant = false;
        }
    }
    else
    {
        dynamicGrant =
            lcgMap.begin()->second->IsLcDynamic(result.lcIdsByPrio.rbegin()->second.front());
    }
    if (dynamicGrant)
    {
        allocationInfo.m_isDynamic = true;
        NS_LOG_DEBUG("Selected scheduling type: dynamic grant / per-PDU ");
    }
    else
    {
        allocationInfo.m_isDynamic = false;
        NS_LOG_DEBUG("Selected scheduling type: SPS");
    }

    // Check if this is a supplemental dynamic grant for an SPS LC
    result.isSupplemental = false;
    if (!dynamicGrant)
    {
        for (const auto& itlcIdsByPrio : result.lcIdsByPrio)
        {
            for (const auto& lcId : itlcIdsByPrio.second)
            {
                if (m_supplementalLcIds.count(lcId))
                {
                    result.isSupplemental = true;
                }
            }
        }
        if (result.isSupplemental)
        {
            dynamicGrant = true;
            allocationInfo.m_isDynamic = true;
            NS_LOG_INFO("Overriding to dynamic for supplemental grant");
        }
    }

    allocationInfo.m_harqEnabled =
        lcgMap.begin()->second->IsHarqEnabled(result.lcIdsByPrio.rbegin()->second.front());

    // Remove all LCs that do not have the selected scheduling type.
    // Find LcId of reference belonging to the LC with selected scheduling type, highest priority,
    // and smallest LcId.
    uint16_t nLcs = 0;
    uint16_t nRemainingLcs = 0;
    for (auto itlcIdsByPrio = result.lcIdsByPrio.rbegin();
         itlcIdsByPrio != result.lcIdsByPrio.rend();
         ++itlcIdsByPrio)
    {
        uint8_t lowestLcId = std::numeric_limits<uint8_t>::max();
        for (auto itLcs = itlcIdsByPrio->second.begin(); itLcs != itlcIdsByPrio->second.end();)
        {
            nLcs++;
            if (lcgMap.begin()->second->IsLcDynamic(*itLcs) != dynamicGrant &&
                !(result.isSupplemental && m_supplementalLcIds.count(*itLcs)))
            {
                itLcs = itlcIdsByPrio->second.erase(itLcs);
            }
            else
            {
                if (*itLcs < lowestLcId)
                {
                    lowestLcId = *itLcs;
                }
                ++itLcs;
                nRemainingLcs++;
            }
        }
        if (itlcIdsByPrio->second.size() == 0)
        {
            itlcIdsByPrio = std::reverse_iterator(result.lcIdsByPrio.erase(--itlcIdsByPrio.base()));
        }

        if (lowestLcId != std::numeric_limits<uint8_t>::max() && result.lcIdOfRef == 0)
        {
            result.lcIdOfRef = lowestLcId;
        }
    }

    // If SPS, remove all LCs with RRI different than the lcIdOfRef, limit the number of LCs
    // to those we can allocate within one slot given the destination MCS, and assign re-selection
    // counters
    if (!dynamicGrant)
    {
        for (auto itlcIdsByPrio = result.lcIdsByPrio.begin();
             itlcIdsByPrio != result.lcIdsByPrio.end();
             ++itlcIdsByPrio)
        {
            for (auto itLcs = itlcIdsByPrio->second.begin(); itLcs != itlcIdsByPrio->second.end();)
            {
                if (lcgMap.begin()->second->GetLcRri(*itLcs) !=
                    lcgMap.begin()->second->GetLcRri(result.lcIdOfRef))
                {
                    itLcs = itlcIdsByPrio->second.erase(itLcs);
                    nRemainingLcs--;
                }
                else
                {
                    ++itLcs;
                }
            }
            if (itlcIdsByPrio->second.size() == 0)
            {
                itlcIdsByPrio = result.lcIdsByPrio.erase(itlcIdsByPrio);
            }
        }

        if (nRemainingLcs > 1)
        {
            auto slotInfo = CreateSlotInfo(minSymbolsPerSlot, hasPsfch);
            auto maxTbs = GetMac()->GetTxPool()->GetTransportBlockSize(slotInfo,
                                                                       dstMcs,
                                                                       GetTotalSubCh(),
                                                                       MCS_TABLE);

            // Always reserve space for lcIdOfRef (highest priority LC with smallest LcId)
            uint32_t currBufferSize = lcgMap.begin()->second->GetTotalSizeOfLC(result.lcIdOfRef);

            // Check all other LCs and keep only those that fit within maxTbs
            for (auto itlcIdsByPrio = result.lcIdsByPrio.begin();
                 itlcIdsByPrio != result.lcIdsByPrio.end();
                 ++itlcIdsByPrio)
            {
                for (auto itLcs = itlcIdsByPrio->second.begin();
                     itLcs != itlcIdsByPrio->second.end();)
                {
                    if (*itLcs == result.lcIdOfRef)
                    {
                        ++itLcs; // lcIdOfRef is already included in the budget
                        continue;
                    }

                    uint32_t lcSize = lcgMap.begin()->second->GetTotalSizeOfLC(*itLcs);

                    if (currBufferSize + lcSize + 5 > maxTbs)
                    {
                        // Drop LC if it does not fit
                        itLcs = itlcIdsByPrio->second.erase(itLcs);
                        nRemainingLcs--;
                        NS_LOG_DEBUG("Dropping SPS LC ID " << +(*itLcs)
                                                           << " because it does not fit in the TB");
                    }
                    else
                    {
                        // Keep LC and update budget
                        currBufferSize += lcSize;
                        ++itLcs;
                    }
                }
            }
        }
        allocationInfo.m_rri = lcgMap.begin()->second->GetLcRri(result.lcIdOfRef);
        // Calculate reselection counter here because C_resel is needed for candidate resource
        // selection
        allocationInfo.m_reselCounter = GetRandomReselectionCounter(allocationInfo.m_rri);
        NS_LOG_INFO("SPS Reselection counter: " << +allocationInfo.m_reselCounter);
    }
    allocationInfo.m_priority = lcgMap.begin()->second->GetLcPriority(result.lcIdOfRef);
    allocationInfo.m_castType = lcgMap.begin()->second->GetLcCastType(result.lcIdOfRef);
    NS_LOG_DEBUG("Number of LCs to attempt allocation for the selected destination: "
                 << nRemainingLcs << "/" << nLcs << ". LcId of reference " << +result.lcIdOfRef);

    result.isDynamic = dynamicGrant;
    return result;
}

void
NrSlUeMacSchedulerDefault::RegenerateSpsSlotAllocations(const SfnSf& sfn, GrantInfo& grant)
{
    NS_LOG_FUNCTION(this << sfn);
    NS_ASSERT_MSG(!grant.baseSlotPattern.empty(), "Base slot pattern not stored");
    NS_ASSERT_MSG(grant.slotAllocations.empty(),
                  "Slot allocations should be empty when regenerating");

    uint16_t resPeriodSlots = GetMac()->GetResvPeriodInSlots(grant.rri);

    // New start is one RRI after the last published NDI
    NS_ASSERT_MSG(grant.lastPublishedNdiSfn.has_value(),
                  "Cannot regenerate SPS slots without a prior published NDI");
    SfnSf newStartSfn = *grant.lastPublishedNdiSfn;
    newStartSfn.Add(resPeriodSlots);
    uint64_t baseNdiNormalized = grant.baseSlotPattern.begin()->sfn.Normalize();
    uint64_t newStartNormalized = newStartSfn.Normalize();
    uint64_t delta = newStartNormalized - baseNdiNormalized;

    for (uint16_t i = 0; i < grant.slResoReselCounter; i++)
    {
        for (const auto& baseSlot : grant.baseSlotPattern)
        {
            auto slAlloc = baseSlot;
            slAlloc.sfn = baseSlot.sfn;
            slAlloc.sfn.Add(static_cast<uint32_t>(delta + i * resPeriodSlots));
            // Future slot may not have the same PSFCH status as the original
            slAlloc.slHasPsfch = GetMac()->SlotHasPsfch(slAlloc.sfn);
            slAlloc.slPsschSymLength = slAlloc.slHasPsfch ? 9 : 12;
            if (slAlloc.ndi == 1)
            {
                NS_LOG_INFO("  SPS keep NDI at: Frame = "
                            << slAlloc.sfn.GetFrame() << " SF = " << +slAlloc.sfn.GetSubframe()
                            << " slot = " << +slAlloc.sfn.GetSlot()
                            << " normalized = " << slAlloc.sfn.Normalize());
            }
            bool insertStatus = grant.slotAllocations.emplace(slAlloc).second;
            NS_ASSERT_MSG(insertStatus, "Slot allocation already exists");
        }
    }
    NS_LOG_INFO("Regenerated " << grant.slotAllocations.size()
                               << " slot allocations for kept SPS grant");
}

void
NrSlUeMacSchedulerDefault::CreateSpsGrant(const SfnSf& sfn,
                                          const std::set<SlGrantResource>& slotAllocList,
                                          const AllocationInfo& allocationInfo)
{
    NS_LOG_FUNCTION(this << sfn);
    // m_grantInfo is a map with key dstL2Id and value std::vector<GrantInfo>
    auto itVecGrantInfo = m_grantInfo.find(slotAllocList.begin()->dstL2Id);
    if (itVecGrantInfo == m_grantInfo.end())
    {
        NS_LOG_DEBUG("New destination " << slotAllocList.begin()->dstL2Id);
        GrantInfo grant = CreateSpsGrantInfo(slotAllocList, allocationInfo);
        auto timeout =
            CalculateSpsProcessTimeout(sfn, grant.slResoReselCounter, allocationInfo.m_rri);
        auto harqId =
            GetMacHarq()->AllocateHarqProcessId(slotAllocList.begin()->dstL2Id, true, timeout);
        if (!harqId.has_value())
        {
            NS_LOG_WARN("Unable to create grant, HARQ Id not available");
            return;
        }
        grant.harqId = harqId.value();
        // To this point, the 'harqEnabled' flag means that either blind or
        // HARQ feedback transmissions are enabled.  However, the semantics
        // of this flag for a published grant are that harqEnabled refers
        // only to whether HARQ feedback is enabled
        grant.harqEnabled = allocationInfo.m_harqEnabled && GetMac()->GetPsfchPeriod();
        grant.castType = allocationInfo.m_castType;
        std::vector<GrantInfo> grantVector;
        grantVector.push_back(grant);
        NotifyGrantCreated(grant);
        // Call a more detailed scheduling trace report before adding grant
        struct SchedulingReport report;
        report.m_sfn = sfn;
        report.m_subchannels = GetMac()->GetTotalSubCh();
        report.m_psfchPeriod = GetMac()->GetPsfchPeriod();
        report.m_t1 = m_selectionParams.m_t1;
        report.m_t2 = m_selectionParams.m_t2;
        m_schedulingTrace(report,
                          m_candidateResources,
                          m_transmissionParams,
                          m_publishedGrants,
                          m_grantInfo,
                          grant);
        itVecGrantInfo =
            m_grantInfo.emplace(std::make_pair(slotAllocList.begin()->dstL2Id, grantVector)).first;
        NS_LOG_INFO("New SPS grant created to new destination "
                    << slotAllocList.begin()->dstL2Id << " with HARQ ID " << +grant.harqId
                    << " HARQ enabled " << +grant.harqEnabled);
    }
    else
    {
        NS_LOG_DEBUG("Destination " << slotAllocList.begin()->dstL2Id << " found");
        // Destination exists
        bool grantFound = false;
        auto itGrantVector = itVecGrantInfo->second.begin();
        for (; itGrantVector != itVecGrantInfo->second.end(); ++itGrantVector)
        {
            // Skip SPS grants with empty slot allocations (counter == 0,
            // awaiting reselection decision)
            if (itGrantVector->slotAllocations.empty())
            {
                continue;
            }
            if (itGrantVector->rri == allocationInfo.m_rri &&
                itGrantVector->slotAllocations.begin()->slRlcPduInfo.size() ==
                    slotAllocList.begin()->slRlcPduInfo.size())
            {
                uint16_t foundLcs = 0;
                for (auto itGrantRlcPdu : itGrantVector->slotAllocations.begin()->slRlcPduInfo)
                {
                    for (auto itNewRlcPdu : slotAllocList.begin()->slRlcPduInfo)
                    {
                        if (itGrantRlcPdu.lcid == itNewRlcPdu.lcid)
                        {
                            NS_LOG_DEBUG("Found matching logical channel ID "
                                         << +itGrantRlcPdu.lcid << " in existing grant");
                            foundLcs++;
                            break;
                        }
                    }
                }
                NS_LOG_DEBUG("Checking if the found LCs "
                             << foundLcs << " matches the slRlcPduInfo.size() "
                             << itGrantVector->slotAllocations.begin()->slRlcPduInfo.size());
                if (foundLcs == itGrantVector->slotAllocations.begin()->slRlcPduInfo.size())
                {
                    grantFound = true;
                    break;
                    // itGrantVector normally points to the found grant at this point
                }
            }
        }
        if (grantFound)
        {
            // This case corresponds to slResoReselCounter going to zero but
            // the grant still existing-- can it happen?
            // If this is reachable code, the below needs to be reworked
            // to avoid copying harq ID to a new grant without updating the timer
            NS_FATAL_ERROR("Check whether this code is unreachable");
            // Update
            NS_ASSERT_MSG(
                itGrantVector->slResoReselCounter == 0,
                "Sidelink resource counter must be zero before assigning new grant for dst "
                    << slotAllocList.begin()->dstL2Id);
            uint8_t prevHarqId = itGrantVector->harqId;
            GrantInfo grant = CreateSpsGrantInfo(slotAllocList, allocationInfo);
            *itGrantVector = grant;
            itGrantVector->harqId = prevHarqId; // Preserve previous ID
            NS_LOG_INFO("Updated SPS grant to destination "
                        << slotAllocList.begin()->dstL2Id << " with HARQ ID "
                        << itGrantVector->harqId << " HARQ enabled " << +grant.harqEnabled);
        }
        else
        {
            // Insert
            GrantInfo grant = CreateSpsGrantInfo(slotAllocList, allocationInfo);
            auto timeout =
                CalculateSpsProcessTimeout(sfn, grant.slResoReselCounter, allocationInfo.m_rri);
            auto harqId =
                GetMacHarq()->AllocateHarqProcessId(slotAllocList.begin()->dstL2Id, true, timeout);
            if (!harqId.has_value())
            {
                NS_LOG_WARN("Unable to create grant, HARQ Id not available");
                return;
            }
            grant.harqId = harqId.value();
            // To this point, the 'harqEnabled' flag means that either blind or
            // HARQ feedback transmissions are enabled.  However, the semantics
            // of this flag for a published grant are that harqEnabled refers
            // only to whether HARQ feedback is enabled
            grant.harqEnabled = allocationInfo.m_harqEnabled && GetMac()->GetPsfchPeriod();
            grant.castType = allocationInfo.m_castType;
            NotifyGrantCreated(grant);
            // Call a more detailed scheduling trace report before adding grant
            struct SchedulingReport report;
            report.m_sfn = sfn;
            report.m_subchannels = GetMac()->GetTotalSubCh();
            report.m_psfchPeriod = GetMac()->GetPsfchPeriod();
            report.m_t1 = m_selectionParams.m_t1;
            report.m_t2 = m_selectionParams.m_t2;
            m_schedulingTrace(report,
                              m_candidateResources,
                              m_transmissionParams,
                              m_publishedGrants,
                              m_grantInfo,
                              grant);
            itVecGrantInfo->second.push_back(grant);
            NS_LOG_INFO("New SPS grant created to existing destination "
                        << slotAllocList.begin()->dstL2Id << " with HARQ ID " << +grant.harqId
                        << " HARQ enabled " << +grant.harqEnabled);
        }
    }
}

Time
NrSlUeMacSchedulerDefault::CalculateProcessTimeout(const SfnSf& sfn,
                                                   const std::optional<SfnSf>& lastTxSlotSfn,
                                                   bool harqEnabled,
                                                   uint16_t psfchPeriod) const
{
    NS_LOG_FUNCTION(this << sfn << lastTxSlotSfn.has_value() << harqEnabled << psfchPeriod);
    auto timePerSlot = MicroSeconds(1000 >> sfn.GetNumerology());
    if (!lastTxSlotSfn.has_value())
    {
        // No slots have been granted; return a minimum timeout
        NS_LOG_DEBUG("Timeout (no granted slots): " << timePerSlot.As(Time::US));
        return timePerSlot;
    }
    // Current time is (sfn.Normalize() * timePerSlot)
    // The last grant transmission time will be at time (lastTxSlotSfn->Normalize() * timePerSlot)
    if (!(harqEnabled && psfchPeriod))
    {
        // If there is no HARQ feedback, set the time to one slot beyond
        // the last grant transmission time
        if (lastTxSlotSfn->Normalize() < sfn.Normalize())
        {
            NS_LOG_DEBUG("Timeout (without HARQ FB, past slot): " << timePerSlot.As(Time::US));
            return timePerSlot;
        }
        auto timeout = timePerSlot * (lastTxSlotSfn->Normalize() + 1 - sfn.Normalize());
        NS_LOG_DEBUG("Timeout (without HARQ FB): " << timeout.As(Time::US));
        return timeout;
    }
    // PSFCH feedback will usually be delivered in the first PSFCH-enabled slot after the
    // MinTimeGapPsfch has elapsed. Find this PSFCH-enabled slot, and set the timeout
    // value to (PSFCH-enabled slot + 1 - current slot) * timePerSlot.
    // Note: even if lastTxSlotSfn is in the past, the PSFCH feedback slot may
    // still be in the future, so do not short-circuit here.
    SfnSf futureSlot = *lastTxSlotSfn;
    futureSlot.Add(1);
    while (!GetMac()->SlotHasPsfch(futureSlot))
    {
        futureSlot.Add(1);
    }
    if (futureSlot.Normalize() < sfn.Normalize())
    {
        NS_LOG_DEBUG("Timeout (with HARQ FB, past PSFCH slot): " << timePerSlot.As(Time::US));
        return timePerSlot;
    }
    auto timeout = timePerSlot * (futureSlot.Normalize() + 1 - sfn.Normalize());
    NS_LOG_DEBUG("Timeout (with HARQ FB): " << timeout.As(Time::US));
    return timeout;
}

void
NrSlUeMacSchedulerDefault::CreateSinglePduGrant(const SfnSf& sfn,
                                                const std::set<SlGrantResource>& slotAllocList,
                                                const AllocationInfo& allocationInfo)
{
    NS_LOG_FUNCTION(this << sfn);
    auto itGrantInfo = m_grantInfo.find(slotAllocList.begin()->dstL2Id);

    if (itGrantInfo == m_grantInfo.end())
    {
        // New destination
        NS_LOG_DEBUG("New destination " << slotAllocList.begin()->dstL2Id);
        auto timeout = CalculateProcessTimeout(sfn,
                                               std::prev(slotAllocList.end())->sfn,
                                               allocationInfo.m_harqEnabled,
                                               GetMac()->GetPsfchPeriod());
        auto harqId =
            GetMacHarq()->AllocateHarqProcessId(slotAllocList.begin()->dstL2Id, false, timeout);
        if (!harqId.has_value())
        {
            NS_LOG_WARN("Unable to create grant, HARQ Id not available");
            return;
        }
        GrantInfo grant = CreateSinglePduGrantInfo(slotAllocList, allocationInfo);
        grant.harqId = harqId.value();
        // To this point, the 'harqEnabled' flag means that either blind or
        // HARQ feedback transmissions are enabled.  However, the semantics
        // of this flag for a published grant are that harqEnabled refers
        // only to whether HARQ feedback is enabled
        grant.harqEnabled = allocationInfo.m_harqEnabled && GetMac()->GetPsfchPeriod();
        grant.castType = allocationInfo.m_castType;
        NotifyGrantCreated(grant);
        // Call a more detailed scheduling trace report before adding grant
        struct SchedulingReport report;
        report.m_sfn = sfn;
        report.m_subchannels = GetMac()->GetTotalSubCh();
        report.m_psfchPeriod = GetMac()->GetPsfchPeriod();
        report.m_t1 = m_selectionParams.m_t1;
        report.m_t2 = m_selectionParams.m_t2;
        m_schedulingTrace(report,
                          m_candidateResources,
                          m_transmissionParams,
                          m_publishedGrants,
                          m_grantInfo,
                          grant);
        std::vector<GrantInfo> grantVector;
        grantVector.push_back(grant);
        itGrantInfo =
            m_grantInfo.emplace(std::make_pair(slotAllocList.begin()->dstL2Id, grantVector)).first;
        NS_LOG_INFO("New dynamic grant created to new destination "
                    << slotAllocList.begin()->dstL2Id << " with HARQ ID " << +grant.harqId
                    << " HARQ enabled " << +grant.harqEnabled);
    }
    else
    {
        // Destination exists; insert a new grant (multiple grants may be
        // created for the same LCs when RLC segmentation requires multiple TBs
        // or if more data is queued than fits in a single TB)
        NS_LOG_DEBUG("Destination " << slotAllocList.begin()->dstL2Id << " found");
        {
            auto timeout = CalculateProcessTimeout(sfn,
                                                   std::prev(slotAllocList.end())->sfn,
                                                   allocationInfo.m_harqEnabled,
                                                   GetMac()->GetPsfchPeriod());
            NS_LOG_INFO("Inserting dynamic grant with timeout of " << timeout.As(Time::MS));
            auto harqId =
                GetMacHarq()->AllocateHarqProcessId(slotAllocList.begin()->dstL2Id, false, timeout);
            if (!harqId.has_value())
            {
                NS_LOG_WARN("Unable to create grant, HARQ Id not available");
                return;
            }
            GrantInfo grant = CreateSinglePduGrantInfo(slotAllocList, allocationInfo);
            grant.harqId = harqId.value();
            // To this point, the 'harqEnabled' flag means that either blind or
            // HARQ feedback transmissions are enabled.  However, the semantics
            // of this flag for a published grant are that harqEnabled refers
            // only to whether HARQ feedback is enabled
            grant.harqEnabled = allocationInfo.m_harqEnabled && GetMac()->GetPsfchPeriod();
            grant.castType = allocationInfo.m_castType;
            NotifyGrantCreated(grant);
            // Call a more detailed scheduling trace report before adding grant
            struct SchedulingReport report;
            report.m_sfn = sfn;
            report.m_subchannels = GetMac()->GetTotalSubCh();
            report.m_psfchPeriod = GetMac()->GetPsfchPeriod();
            report.m_t1 = m_selectionParams.m_t1;
            report.m_t2 = m_selectionParams.m_t2;
            m_schedulingTrace(report,
                              m_candidateResources,
                              m_transmissionParams,
                              m_publishedGrants,
                              m_grantInfo,
                              grant);
            itGrantInfo->second.push_back(grant);
            NS_LOG_INFO("New dynamic grant created to existing destination "
                        << slotAllocList.begin()->dstL2Id << " with HARQ ID " << +grant.harqId
                        << " HARQ enabled " << +grant.harqEnabled);
        }
    }
}

NrSlUeMacScheduler::GrantInfo
NrSlUeMacSchedulerDefault::CreateSpsGrantInfo(const std::set<SlGrantResource>& slotAllocList,
                                              const AllocationInfo& allocationInfo) const
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG((!allocationInfo.m_rri.IsZero()), "Can not create SPS grants with 0 RRI");

    NS_LOG_DEBUG("Creating SPS grants for dstL2Id " << slotAllocList.begin()->dstL2Id);
    NS_LOG_DEBUG("Resource reservation interval " << allocationInfo.m_rri.GetMilliSeconds()
                                                  << " ms");

    uint16_t resPeriodSlots = GetMac()->GetResvPeriodInSlots(allocationInfo.m_rri);
    GrantInfo grant;

    grant.slResoReselCounter = allocationInfo.m_reselCounter;
    grant.cReselCounter = 10 * allocationInfo.m_reselCounter;

    // if further IDs are needed and the std::deque needs to be popped from
    // front, need to copy the std::deque to remove its constness
    grant.nSelected = static_cast<uint8_t>(slotAllocList.size());
    grant.rri = allocationInfo.m_rri;
    grant.castType = allocationInfo.m_castType;
    grant.allocationTime = Simulator::Now();
    NS_LOG_DEBUG("nSelected = " << +grant.nSelected);

    for (uint16_t i = 0; i < allocationInfo.m_reselCounter; i++)
    {
        for (const auto& it : slotAllocList)
        {
            auto slAlloc = it;
            slAlloc.sfn.Add(i * resPeriodSlots);

            if (slAlloc.ndi == 1)
            {
                NS_LOG_INFO("  SPS NDI scheduled at: Frame = "
                            << slAlloc.sfn.GetFrame() << " SF = " << +slAlloc.sfn.GetSubframe()
                            << " slot = " << +slAlloc.sfn.GetSlot()
                            << " normalized = " << slAlloc.sfn.Normalize()
                            << " subchannels = " << slAlloc.slPsschSubChStart << ":"
                            << slAlloc.slPsschSubChStart + slAlloc.slPsschSubChLength - 1);
            }
            else
            {
                NS_LOG_INFO("  SPS rtx scheduled at: Frame = "
                            << slAlloc.sfn.GetFrame() << " SF = " << +slAlloc.sfn.GetSubframe()
                            << " slot = " << +slAlloc.sfn.GetSlot()
                            << " normalized = " << slAlloc.sfn.Normalize()
                            << " subchannels = " << slAlloc.slPsschSubChStart << ":"
                            << slAlloc.slPsschSubChStart + slAlloc.slPsschSubChLength - 1);
            }
            // Future slot may not have the same PSFCH status as the original slot
            slAlloc.slHasPsfch = GetMac()->SlotHasPsfch(slAlloc.sfn);
            slAlloc.slPsschSymLength = slAlloc.slHasPsfch ? 9 : 12;
            bool insertStatus = grant.slotAllocations.emplace(slAlloc).second;
            NS_ASSERT_MSG(insertStatus, "slot allocation already exist");
        }
    }

    // Store the base slot pattern (first nSelected slots) for potential regeneration
    // when resources are kept at reselection time
    grant.baseSlotPattern = slotAllocList;

    grant.tbSize = allocationInfo.m_tbSize;
    return grant;
}

NrSlUeMacScheduler::GrantInfo
NrSlUeMacSchedulerDefault::CreateSinglePduGrantInfo(const std::set<SlGrantResource>& slotAllocList,
                                                    const AllocationInfo& allocationInfo) const
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("Creating single-PDU grant for dstL2Id " << slotAllocList.begin()->dstL2Id);

    GrantInfo grant;
    grant.nSelected = static_cast<uint8_t>(slotAllocList.size());
    grant.isDynamic = true;
    grant.castType = allocationInfo.m_castType;
    grant.allocationTime = Simulator::Now();
    NS_LOG_DEBUG("nSelected = " << +grant.nSelected);

    for (const auto& it : slotAllocList)
    {
        auto slAlloc = it;
        if (slAlloc.ndi == 1)
        {
            NS_LOG_INFO("  Dynamic NDI scheduled at: Frame = "
                        << slAlloc.sfn.GetFrame() << " SF = " << +slAlloc.sfn.GetSubframe()
                        << " slot = " << +slAlloc.sfn.GetSlot() << " normalized = "
                        << slAlloc.sfn.Normalize() << " subchannels = " << slAlloc.slPsschSubChStart
                        << ":" << slAlloc.slPsschSubChStart + slAlloc.slPsschSubChLength - 1);
        }
        else
        {
            NS_LOG_INFO("  Dynamic rtx scheduled at: Frame = "
                        << slAlloc.sfn.GetFrame() << " SF = " << +slAlloc.sfn.GetSubframe()
                        << " slot = " << +slAlloc.sfn.GetSlot() << " normalized = "
                        << slAlloc.sfn.Normalize() << " subchannels = " << slAlloc.slPsschSubChStart
                        << ":" << slAlloc.slPsschSubChStart + slAlloc.slPsschSubChLength - 1);
        }
        bool insertStatus = grant.slotAllocations.emplace(slAlloc).second;
        NS_ASSERT_MSG(insertStatus, "slot allocation already exist");
    }
    grant.tbSize = allocationInfo.m_tbSize;
    return grant;
}

void
NrSlUeMacSchedulerDefault::CheckForGrantsToPublish(const SfnSf& sfn)
{
    NS_LOG_FUNCTION(this << sfn.Normalize());
    for (auto itGrantInfo = m_grantInfo.begin(); itGrantInfo != m_grantInfo.end(); itGrantInfo++)
    {
        for (auto itGrantVector = itGrantInfo->second.begin();
             itGrantVector != itGrantInfo->second.end();)
        {
            if (!itGrantVector->isDynamic && itGrantVector->slResoReselCounter == 0)
            {
                ++itGrantVector;
                continue;
            }

            if (itGrantVector->slotAllocations.empty())
            {
                // Slots exhausted while counter > 0 (likely due to slots with no data).
                // The grant cannot publish anything; skip it and let
                // TxResourceReselectionCheck clean it up when the LC has data.
                ++itGrantVector;
                continue;
            }

            if (itGrantVector->slotAllocations.begin()->sfn.Normalize() > sfn.Normalize() + m_t1)
            {
                ++itGrantVector;
                continue;
            }
            // The next set of slots (NDI + any retransmissions) should be added
            // to a grant, and deleted from m_grantInfo
            auto slotIt = itGrantVector->slotAllocations.begin();
            NS_ASSERT_MSG(slotIt->ndi == 1, "New data indication not found");
            NS_ASSERT_MSG(slotIt->sfn.Normalize() >= sfn.Normalize(), "Stale slot in m_grantInfo");
            SlGrantResource currentSlot = *slotIt;
            NS_LOG_DEBUG("Slot at : Frame = " << currentSlot.sfn.GetFrame()
                                              << " SF = " << +currentSlot.sfn.GetSubframe()
                                              << " slot = " << +currentSlot.sfn.GetSlot());
            // Per TS 38.321, the reselection counter should only decrement
            // after the transmission of a TB with New Data Indicator. For SPS
            // grants, check if any LC has buffered data before publishing.
            // If no LC has data, skip this slot without decrementing the counter.
            if (!itGrantVector->isDynamic)
            {
                uint32_t dstL2Id = itGrantInfo->first;
                const auto itDstInfo = m_dstMap.find(dstL2Id);
                bool hasData = false;
                if (itDstInfo != m_dstMap.end())
                {
                    const auto& lcgMap = itDstInfo->second->GetNrSlLCG();
                    for (const auto& rlcPdu : currentSlot.slRlcPduInfo)
                    {
                        if (lcgMap.begin()->second->GetTotalSizeOfLC(rlcPdu.lcid) > 0)
                        {
                            hasData = true;
                            break;
                        }
                    }
                }
                if (!hasData)
                {
                    NS_LOG_INFO("SPS slot skipped (no data): counter stays at "
                                << +itGrantVector->slResoReselCounter);
                    // Remove NDI slot and associated retransmission slots
                    itGrantVector->slotAllocations.erase(slotIt);
                    slotIt = itGrantVector->slotAllocations.begin();
                    while (slotIt != itGrantVector->slotAllocations.end() && slotIt->ndi == 0)
                    {
                        itGrantVector->slotAllocations.erase(slotIt);
                        slotIt = itGrantVector->slotAllocations.begin();
                    }
                    // Check if grant is exhausted with counter > 0
                    if (itGrantVector->slotAllocations.empty() &&
                        itGrantVector->slResoReselCounter > 0)
                    {
                        // Shorten the HARQ timer to free the ID after in-flight
                        // transmissions/feedback complete, then erase the grant.
                        NS_LOG_INFO("SPS grant exhausted with counter "
                                    << +itGrantVector->slResoReselCounter
                                    << "; shortening HARQ timer and erasing grant");
                        Time deallocTime =
                            CalculateProcessTimeout(sfn,
                                                    itGrantVector->lastGrantedSlotSfn,
                                                    itGrantVector->harqEnabled,
                                                    GetMac()->GetPsfchPeriod());
                        // The ID may have been recycled if the HARQ timer expired naturally
                        // after all slots were consumed; skip renewal to avoid corrupting a new
                        // grant.
                        if (deallocTime > GetMac()->GetSlotPeriod())
                        {
                            GetMacHarq()->RenewHarqProcessIdTimer(itGrantVector->harqId,
                                                                  deallocTime);
                        }
                        itGrantVector = itGrantInfo->second.erase(itGrantVector);
                    }
                    else
                    {
                        ++itGrantVector;
                    }
                    continue;
                }
            }
            // Update per-LC dequeue sizes to use full TB payload capacity.
            // The RLC will dequeue min(txOpportunity, bufferSize), so providing
            // the full capacity allows the RLC to send more data if available.
            if (itGrantVector->tbSize > 0)
            {
                uint32_t numLcs = currentSlot.slRlcPduInfo.size();
                uint32_t macHeaderOverhead = SUBHEADER_SIZE * numLcs;
                uint32_t totalPayload = itGrantVector->tbSize > macHeaderOverhead
                                            ? itGrantVector->tbSize - macHeaderOverhead
                                            : 0;
                if (numLcs == 1)
                {
                    NS_LOG_DEBUG("Expanding LC "
                                 << +currentSlot.slRlcPduInfo[0].lcid << " dequeue from "
                                 << currentSlot.slRlcPduInfo[0].size << " to " << totalPayload
                                 << " bytes (full TB payload)");
                    currentSlot.slRlcPduInfo[0].size = totalPayload;
                }
                else
                {
                    // Give remaining capacity to the first (highest priority) LC
                    uint32_t otherLcTotal = 0;
                    for (size_t i = 1; i < numLcs; i++)
                    {
                        otherLcTotal += currentSlot.slRlcPduInfo[i].size;
                    }
                    uint32_t firstLcPayload =
                        totalPayload > otherLcTotal ? totalPayload - otherLcTotal : 0;
                    NS_LOG_DEBUG("Expanding LC "
                                 << +currentSlot.slRlcPduInfo[0].lcid << " dequeue from "
                                 << currentSlot.slRlcPduInfo[0].size << " to " << firstLcPayload
                                 << " bytes");
                    currentSlot.slRlcPduInfo[0].size = firstLcPayload;
                }
            }
            // Log the assigned bytes to each LC of this destination
            for (const auto& it : currentSlot.slRlcPduInfo)
            {
                NS_LOG_DEBUG("LC " << static_cast<uint16_t>(it.lcid) << " was assigned " << it.size
                                   << " bytes");
            }
            itGrantVector->tbTxCounter = 1;
            NrSlUeMac::NrSlGrant grant;
            grant.harqId = itGrantVector->harqId;
            grant.nSelected = itGrantVector->nSelected;
            grant.tbTxCounter = itGrantVector->tbTxCounter;
            grant.tbSize = itGrantVector->tbSize;
            grant.rri = itGrantVector->rri;
            // For SPS grants: if this is the last NDI slot and counter will
            // remain > 0 after decrement (due to earlier skipped slots),
            // signal pRsvp=0 so sensing UEs know the reservation is ending.
            if (!itGrantVector->isDynamic && itGrantVector->slResoReselCounter > 1)
            {
                bool moreNdiSlots = false;
                auto peekIt = slotIt;
                ++peekIt;
                while (peekIt != itGrantVector->slotAllocations.end())
                {
                    if (peekIt->ndi == 1)
                    {
                        moreNdiSlots = true;
                        break;
                    }
                    ++peekIt;
                }
                if (!moreNdiSlots)
                {
                    NS_LOG_INFO("Last NDI slot with counter " << +itGrantVector->slResoReselCounter
                                                              << "; setting pRsvp=0");
                    grant.rri = Time(0);
                }
            }
            grant.harqEnabled = itGrantVector->harqEnabled;
            grant.castType = itGrantVector->castType;
            // Add the NDI slot and retransmissions to the set of slot allocations
            m_publishedGrants.emplace_back(currentSlot);
            grant.slotAllocations.emplace(currentSlot);
            itGrantVector->slotAllocations.erase(slotIt);
            // Add any retransmission slots and erase them
            slotIt = itGrantVector->slotAllocations.begin();
            while (slotIt != itGrantVector->slotAllocations.end() && slotIt->ndi == 0)
            {
                SlGrantResource nextSlot = *slotIt;
                m_publishedGrants.emplace_back(nextSlot);
                grant.slotAllocations.emplace(nextSlot);
                itGrantVector->slotAllocations.erase(slotIt);
                slotIt = itGrantVector->slotAllocations.begin();
            }
            GetMac()->SchedNrSlConfigInd(currentSlot.dstL2Id, grant);
            NotifyGrantPublished(grant);
            NS_LOG_INFO("Publishing grant with " << grant.slotAllocations.size()
                                                 << " slots to destination " << currentSlot.dstL2Id
                                                 << " HARQ ID " << +grant.harqId);
            // Track the last granted slot (including retx) for HARQ timer calculation
            itGrantVector->lastGrantedSlotSfn = std::prev(grant.slotAllocations.end())->sfn;
            if (itGrantVector->isDynamic)
            {
                itGrantVector = itGrantInfo->second.erase(itGrantVector);
            }
            else
            {
                // Track last published NDI slot for potential regeneration
                itGrantVector->lastPublishedNdiSfn = currentSlot.sfn;
                --itGrantVector->slResoReselCounter;
                ++itGrantVector;
            }
        }
    }
}

bool
NrSlUeMacSchedulerDefault::OverlappedResources(const SfnSf& firstSfn,
                                               uint16_t firstStart,
                                               uint16_t firstLength,
                                               const SfnSf& secondSfn,
                                               uint16_t secondStart,
                                               uint16_t secondLength) const
{
    NS_ASSERT_MSG(firstLength && secondLength, "Length should not be zero");
    if (firstSfn == secondSfn)
    {
        if (std::max(firstStart, secondStart) <
            std::min(firstStart + firstLength, secondStart + secondLength))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

std::list<SlResourceInfo>
NrSlUeMacSchedulerDefault::FilterTxOpportunities(const SfnSf& sfn,
                                                 std::list<SlResourceInfo> txOppr,
                                                 Time rri,
                                                 uint16_t cResel)
{
    NS_LOG_FUNCTION(this << sfn.Normalize() << txOppr.size() << rri.As(Time::MS) << cResel);

    if (txOppr.empty())
    {
        return txOppr;
    }
    NS_LOG_DEBUG("Filtering txOppr list of size " << txOppr.size() << " resources");
    std::map<uint32_t, std::vector<NrSlUeMacScheduler::GrantInfo>> externalGrants;
    if (m_idealSchedLevel == IDEAL_SCHED_GLOBAL || m_idealSchedLevel == IDEAL_SCHED_LOCAL_UNICAST)
    {
        GetExternalGrants(externalGrants);
    }

    auto itTxOppr = txOppr.begin();
    while (itTxOppr != txOppr.end())
    {
        // Filter each candidate on three possibilities:
        // 1) if candidate overlaps with a resource in the list of published grants
        // 2) if candidate overlaps with a resource in the list of unpublished grants
        // 3) if whole slot exclusion option is enabled, and candidate is marked with slotBusy
        // 4) if using global or local knowledge is enabled, and candidate overlaps with a resource
        // in the list of external grants

        bool filtered = false;
        // 1) if candidate overlaps with a resource in the list of published grants
        auto itPublished = m_publishedGrants.begin();
        while (itPublished != m_publishedGrants.end())
        {
            // Erase published records in the past
            if (itPublished->sfn < sfn)
            {
                NS_LOG_INFO("Erasing published grant from " << itPublished->sfn.Normalize());
                itPublished = m_publishedGrants.erase(itPublished);
                continue;
            }
            if (m_allowMultipleDestinationsPerSlot)
            {
                if (OverlappedResources(itPublished->sfn,
                                        itPublished->slPsschSubChStart,
                                        itPublished->slPsschSubChLength,
                                        itTxOppr->sfn,
                                        itTxOppr->slSubchannelStart,
                                        itTxOppr->slSubchannelLength))
                {
                    NS_LOG_INFO("Erasing candidate " << itTxOppr->sfn.Normalize()
                                                     << " due to published grant overlap");
                    itTxOppr = txOppr.erase(itTxOppr);
                }
                else
                {
                    ++itTxOppr;
                }
            }
            else
            {
                if (itPublished->sfn == itTxOppr->sfn)
                {
                    filtered = true;
                    NS_LOG_INFO("Erasing candidate " << itTxOppr->sfn.Normalize()
                                                     << " due to published grant overlap");
                }
            }
            ++itPublished;
        }
        if (filtered)
        {
            itTxOppr = txOppr.erase(itTxOppr);
            continue;
        }
        // 2) if candidate overlaps with a resource in the list of unpublished grants
        for (const auto& itDst : m_grantInfo)
        {
            for (auto itGrantVector = itDst.second.begin(); itGrantVector != itDst.second.end();
                 ++itGrantVector)
            {
                for (auto itGrantAlloc = itGrantVector->slotAllocations.begin();
                     itGrantAlloc != itGrantVector->slotAllocations.end();
                     itGrantAlloc++)
                {
                    // need to consider this txOppr plus its potential repetitions
                    bool foundOverlap = false;
                    for (uint16_t i = 0; i <= cResel; i++)
                    {
                        SfnSf candidateSfn = itTxOppr->sfn.GetFutureSfnSf(
                            i * rri.GetMilliSeconds() * std::pow(2, sfn.GetNumerology()));
                        if (itGrantAlloc->sfn < candidateSfn)
                        {
                            break;
                        }
                        if (m_allowMultipleDestinationsPerSlot)
                        {
                            if (OverlappedResources(itGrantAlloc->sfn,
                                                    itGrantAlloc->slPsschSubChStart,
                                                    itGrantAlloc->slPsschSubChLength,
                                                    candidateSfn,
                                                    itTxOppr->slSubchannelStart,
                                                    itTxOppr->slSubchannelLength))
                            {
                                foundOverlap = true;
                                break;
                            }
                        }
                        else
                        {
                            // Disallow scheduling again on a previously scheduled slot
                            if (itGrantAlloc->sfn == candidateSfn)
                            {
                                foundOverlap = true;
                                break;
                            }
                        }
                    }
                    if (foundOverlap)
                    {
                        NS_LOG_INFO("Erasing candidate " << itTxOppr->sfn.Normalize());
                        filtered = true;
                    }
                }
            }
        }
        if (filtered)
        {
            itTxOppr = txOppr.erase(itTxOppr);
            continue;
        }
        // 3) if whole slot exclusion option is enabled, and candidate is marked with slotBusy
        if (m_wholeSlotExclusion && itTxOppr->GetSlotBusy())
        {
            NS_LOG_INFO("Erasing slotBusy candidate " << itTxOppr->sfn.Normalize());
            itTxOppr = txOppr.erase(itTxOppr);
            continue;
        }

        // 4) if using global or localknowledge is enabled, and candidate overlaps with a resource
        // in the list of external grants
        if (m_idealSchedLevel == IDEAL_SCHED_GLOBAL ||
            m_idealSchedLevel == IDEAL_SCHED_LOCAL_UNICAST)
        {
            for (const auto& itDst : externalGrants)
            {
                for (auto itGrantVector = itDst.second.begin(); itGrantVector != itDst.second.end();
                     ++itGrantVector)
                {
                    for (auto itGrantAlloc = itGrantVector->slotAllocations.begin();
                         itGrantAlloc != itGrantVector->slotAllocations.end();
                         itGrantAlloc++)
                    {
                        // need to consider this txOppr plus its potential repetitions
                        bool foundOverlap = false;
                        for (uint16_t i = 0; i <= cResel; i++)
                        {
                            SfnSf candidateSfn = itTxOppr->sfn.GetFutureSfnSf(
                                i * rri.GetMilliSeconds() * std::pow(2, sfn.GetNumerology()));
                            if (itGrantAlloc->sfn < candidateSfn)
                            {
                                break;
                            }
                            if (m_allowMultipleDestinationsPerSlot)
                            {
                                if (OverlappedResources(itGrantAlloc->sfn,
                                                        itGrantAlloc->slPsschSubChStart,
                                                        itGrantAlloc->slPsschSubChLength,
                                                        candidateSfn,
                                                        itTxOppr->slSubchannelStart,
                                                        itTxOppr->slSubchannelLength))
                                {
                                    foundOverlap = true;
                                    break;
                                }
                            }
                            else
                            {
                                // Disallow scheduling again on a previously scheduled slot
                                if (itGrantAlloc->sfn == candidateSfn)
                                {
                                    foundOverlap = true;
                                    break;
                                }
                            }
                        }
                        if (foundOverlap)
                        {
                            NS_LOG_INFO("Erasing candidate " << itTxOppr->sfn.Normalize()
                                                             << " due to external grant overlap");
                            filtered = true;
                        }
                    }
                }
            }
            if (filtered)
            {
                itTxOppr = txOppr.erase(itTxOppr);
                continue;
            }
        }

        ++itTxOppr;
    }
    return txOppr;
}

uint8_t
NrSlUeMacSchedulerDefault::GetTotalSubCh() const
{
    return GetMac()->GetTotalSubCh();
}

uint8_t
NrSlUeMacSchedulerDefault::GetSlMaxTxTransNumPssch() const
{
    return GetMac()->GetSlMaxTxTransNumPssch();
}

uint8_t
NrSlUeMacSchedulerDefault::GetRv(uint8_t txNumTb) const
{
    NS_LOG_FUNCTION(this << +txNumTb);
    uint8_t modulo = txNumTb % 4;
    // we assume rvid = 0, so RV would take 0, 2, 3, 1
    // see TS 38.21 table 6.1.2.1-2
    uint8_t rv = 0;
    switch (modulo)
    {
    case 0:
        rv = 0;
        break;
    case 1:
        rv = 2;
        break;
    case 2:
        rv = 3;
        break;
    case 3:
        rv = 1;
        break;
    default:
        NS_ABORT_MSG("Wrong modulo result to deduce RV");
    }

    return rv;
}

int64_t
NrSlUeMacSchedulerDefault::AssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    m_grantSelectionUniformVariable->SetStream(stream);
    m_destinationUniformVariable->SetStream(stream + 1);
    m_ueSelectedUniformVariable->SetStream(stream + 2);
    return 3;
}

void
NrSlUeMacSchedulerDefault::DoDispose()
{
    NS_LOG_FUNCTION(this);
    if (m_mcsController)
    {
        m_mcsController->Dispose();
        m_mcsController = nullptr;
    }
    if (!m_mcsQueryIndicationCb.IsNull())
    {
        m_mcsQueryIndicationCb =
            MakeNullCallback<void, const SfnSf&, uint32_t, uint32_t, uint32_t>();
    }
}

NrSlCommResourcePool::SlotInfo
NrSlUeMacSchedulerDefault::CreateSlotInfo(uint16_t symbolsPerSlot, bool hasPsfch) const
{
    const auto pool = GetMac()->GetTxPool();
    const auto bwpId = GetMac()->GetBwpId();
    const auto poolId = GetMac()->GetSlActivePoolId();
    return NrSlCommResourcePool::SlotInfo{pool->GetNumSlPscchRbs(bwpId, poolId),
                                          pool->GetPscchSymStart(bwpId, poolId),
                                          pool->GetPscchSymLength(bwpId, poolId),
                                          pool->GetPsschSymStart(bwpId, poolId),
                                          symbolsPerSlot,
                                          hasPsfch,
                                          pool->GetSlSubChSize(bwpId, poolId),
                                          pool->GetMaxNumPerReserve(bwpId, poolId),
                                          0, /* absSlotIndex, unneeded here */
                                          0};
}

bool
NrSlUeMacSchedulerDefault::DoNrSlAllocation(
    const std::list<SlResourceInfo>& candResources,
    const std::shared_ptr<NrSlUeMacSchedulerDstInfo>& dstInfo,
    std::set<SlGrantResource>& slotAllocList,
    const AllocationInfo& allocationInfo)
{
    NS_LOG_FUNCTION(this);
    bool allocated = false;
    NS_ASSERT_MSG(candResources.size() > 0,
                  "Scheduler received an empty resource list from UE MAC");

    std::list<SlResourceInfo> selectedTxOpps;
    // blind retransmission corresponds to HARQ enabled AND (PSFCH period == 0)
    if (allocationInfo.m_harqEnabled && (GetMac()->GetPsfchPeriod() == 0))
    {
        // Select up to N_PSSCH_maxTx resources without regard MinTimeGapPsfch
        // i.e., for blind retransmissions
        selectedTxOpps = SelectResourcesForBlindRetransmissions(candResources);
    }
    else
    {
        selectedTxOpps = SelectResourcesWithConstraint(candResources, allocationInfo.m_harqEnabled);
    }
    NS_ASSERT_MSG(selectedTxOpps.size() > 0, "Scheduler should select at least 1 slot from txOpps");
    allocated = true;
    auto itTxOpps = selectedTxOpps.cbegin();
    for (; itTxOpps != selectedTxOpps.cend(); ++itTxOpps)
    {
        SlGrantResource slotAlloc;
        slotAlloc.sfn = itTxOpps->sfn;
        slotAlloc.dstL2Id = dstInfo->GetDstL2Id();
        slotAlloc.priority = allocationInfo.m_priority;
        slotAlloc.slRlcPduInfo = allocationInfo.m_allocatedRlcPdus;
        slotAlloc.mcs = dstInfo->GetDstMcs();
        // PSCCH
        slotAlloc.numSlPscchRbs = itTxOpps->numSlPscchRbs;
        slotAlloc.slPscchSymStart = itTxOpps->slPscchSymStart;
        slotAlloc.slPscchSymLength = itTxOpps->slPscchSymLength;
        // PSSCH
        slotAlloc.slPsschSymStart = itTxOpps->slPsschSymStart;
        slotAlloc.slPsschSymLength = itTxOpps->slPsschSymLength;
        slotAlloc.slPsschSubChStart = itTxOpps->slSubchannelStart;
        slotAlloc.slPsschSubChLength = itTxOpps->slSubchannelLength;
        slotAlloc.maxNumPerReserve = itTxOpps->slMaxNumPerReserve;
        slotAlloc.ndi = slotAllocList.empty() == true ? 1 : 0;
        slotAlloc.rv = GetRv(static_cast<uint8_t>(slotAllocList.size()));
        if (static_cast<uint16_t>(slotAllocList.size()) % itTxOpps->slMaxNumPerReserve == 0)
        {
            slotAlloc.txSci1A = true;
            if (slotAllocList.size() + itTxOpps->slMaxNumPerReserve <= selectedTxOpps.size())
            {
                slotAlloc.slotNumInd = itTxOpps->slMaxNumPerReserve;
            }
            else
            {
                slotAlloc.slotNumInd = selectedTxOpps.size() - slotAllocList.size();
            }
        }
        else
        {
            slotAlloc.txSci1A = false;
            // Slot, which does not carry SCI 1-A can not indicate future TXs
            slotAlloc.slotNumInd = 0;
        }

        slotAllocList.emplace(slotAlloc);
    }
    return allocated;
}

bool
NrSlUeMacSchedulerDefault::OverlappedSlots(const std::list<SlResourceInfo>& resources,
                                           const SlResourceInfo& candidate) const
{
    for (const auto& it : resources)
    {
        if (it.sfn == candidate.sfn)
        {
            return true;
        }
    }
    return false;
}

std::list<SlResourceInfo>
NrSlUeMacSchedulerDefault::SelectResourcesForBlindRetransmissions(std::list<SlResourceInfo> txOpps)
{
    NS_LOG_FUNCTION(this << txOpps.size());

    uint8_t totalTx = GetSlMaxTxTransNumPssch();
    std::list<SlResourceInfo> newTxOpps;

    NS_LOG_DEBUG("Attempting to select " << +totalTx << " (blind) transmissions from size of "
                                         << txOpps.size());
    if (txOpps.size() > totalTx)
    {
        while (newTxOpps.size() != totalTx && txOpps.size() > 0)
        {
            auto txOppsIt = txOpps.begin();
            // Advance to the randomly selected element
            std::advance(txOppsIt,
                         m_grantSelectionUniformVariable->GetInteger(0, txOpps.size() - 1));
            if (!OverlappedSlots(newTxOpps, *txOppsIt))
            {
                // copy the randomly selected slot info into the new list
                newTxOpps.emplace_back(*txOppsIt);
            }
            // erase the selected one from the list
            txOppsIt = txOpps.erase(txOppsIt);
        }
    }
    else
    {
        // Try to use each available slot
        auto txOppsIt = txOpps.begin();
        while (txOppsIt != txOpps.end())
        {
            if (!OverlappedSlots(newTxOpps, *txOppsIt))
            {
                // copy the slot info into the new list
                newTxOpps.emplace_back(*txOppsIt);
            }
            // erase the selected one from the list
            txOppsIt = txOpps.erase(txOppsIt);
        }
    }

    // sort the list by SfnSf before returning
    newTxOpps.sort();
    NS_ASSERT_MSG(newTxOpps.size() <= totalTx,
                  "Number of randomly selected slots exceeded total number of TX");
    return newTxOpps;
}

std::list<SlResourceInfo>
NrSlUeMacSchedulerDefault::SelectResourcesWithConstraint(std::list<SlResourceInfo> txOpps,
                                                         bool harqEnabled)
{
    NS_LOG_FUNCTION(this << txOpps.size() << harqEnabled);
    uint8_t totalTx = 1;
    if (harqEnabled)
    {
        totalTx = GetSlMaxTxTransNumPssch();
    }
    std::list<SlResourceInfo> newTxOpps;
    std::size_t originalSize [[maybe_unused]] = txOpps.size();

    // TS 38.321 states to randomly select a resource from the available
    // pool, and then to proceed to select additional resources at random
    // such that the minimum time gap between any two selected resources
    // in case that PSFCH is configured for this pool of resources and
    // that a retransmission resource can be indicated by the time resource
    // assignment of a prior SCI according to clause 8.3.1.1 of TS 38.212

    // *txOppsIt.sfn is the SfnSf
    // *txOppsIt.slHasPsfch is the SfnSf
    while (newTxOpps.size() < totalTx && txOpps.size() > 0)
    {
        auto txOppsIt = txOpps.begin();
        std::advance(txOppsIt, m_grantSelectionUniformVariable->GetInteger(0, txOpps.size() - 1));
        if (IsCandidateResourceEligible(newTxOpps, *txOppsIt))
        {
            // copy the randomly selected resource into the new list
            newTxOpps.emplace_back(*txOppsIt);
            newTxOpps.sort();
        }
        // erase the selected one from the list
        txOpps.erase(txOppsIt);
    }
    // sort the list by SfnSf before returning
    newTxOpps.sort();
    NS_LOG_INFO("Selected " << newTxOpps.size() << " resources from " << originalSize
                            << " candidates and a maximum of " << +totalTx);
    return newTxOpps;
}

// This logic implements the minimum time gap constraint check.  The time
// resource assignment constraint (which appears to be a constraint on
// assigning SCI 1-A frequently enough, not on slot selection) can be
// handled in DoNrSlAllocation
bool
NrSlUeMacSchedulerDefault::IsMinTimeGapSatisfied(const SfnSf& first,
                                                 const SfnSf& second,
                                                 uint8_t minTimeGapPsfch,
                                                 uint8_t minTimeGapProcessing) const
{
    NS_ASSERT_MSG(minTimeGapPsfch > 0, "Invalid minimum time gap");
    SfnSf sfnsf = first;
    sfnsf.Add(minTimeGapPsfch);
    while (!GetMac()->SlotHasPsfch(sfnsf))
    {
        sfnsf.Add(1);
    }
    sfnsf.Add(minTimeGapProcessing);
    return (sfnsf <= second);
}

bool
NrSlUeMacSchedulerDefault::IsCandidateResourceEligible(const std::list<SlResourceInfo>& txOpps,
                                                       const SlResourceInfo& resourceInfo) const
{
    NS_LOG_FUNCTION(txOpps.size() << resourceInfo.sfn.Normalize());
    if (txOpps.size() == 0)
    {
        NS_LOG_DEBUG("Resource " << resourceInfo.sfn.Normalize()
                                 << " is eligible as the first slot in the list");
        return true; // first slot is always eligible
    }
    auto firstElementIt = txOpps.cbegin();
    auto lastElementIt = std::prev(txOpps.cend(), 1);
    if (resourceInfo.sfn == (*firstElementIt).sfn || resourceInfo.sfn == (*lastElementIt).sfn)
    {
        NS_LOG_DEBUG("Resource " << resourceInfo.sfn.Normalize()
                                 << " overlaps with first or last on the list");
        return false;
    }
    if (resourceInfo.sfn < (*firstElementIt).sfn)
    {
        bool eligible = IsMinTimeGapSatisfied(resourceInfo.sfn,
                                              (*firstElementIt).sfn,
                                              (*firstElementIt).slMinTimeGapPsfch,
                                              (*firstElementIt).slMinTimeGapProcessing);
        if (eligible)
        {
            NS_LOG_DEBUG("Resource " << resourceInfo.sfn.Normalize()
                                     << " is eligible as a new first slot in the list");
        }
        else
        {
            NS_LOG_DEBUG("Resource "
                         << resourceInfo.sfn.Normalize()
                         << " is not outside of minimum time gap to first slot in list");
        }
        return eligible;
    }
    else if ((*lastElementIt).sfn < resourceInfo.sfn)
    {
        bool eligible = IsMinTimeGapSatisfied((*lastElementIt).sfn,
                                              resourceInfo.sfn,
                                              (*lastElementIt).slMinTimeGapPsfch,
                                              (*lastElementIt).slMinTimeGapProcessing);
        if (eligible)
        {
            NS_LOG_DEBUG("Resource " << resourceInfo.sfn.Normalize()
                                     << " is eligible as a new last slot in the list");
        }
        else
        {
            NS_LOG_DEBUG("Resource " << resourceInfo.sfn.Normalize()
                                     << " is not outside of minimum time gap to last slot in list");
        }
        return eligible;
    }
    else
    {
        // Candidate slot lies in between elements of txOpps.  Find the two
        // elements (left, right) that bound the candidate.  Test that
        // the min time gap is satisfied for both intervals (left, candidate)
        // and (candidate, right).  Also, the resource may not overlap.
        auto leftIt = firstElementIt;
        auto rightIt = std::next(leftIt, 1);
        // we have already checked firstElementIt for an SFN match, so only
        // need to check the next one (rightIt)
        if (resourceInfo.sfn == (*rightIt).sfn)
        {
            NS_LOG_DEBUG("Resource " << resourceInfo.sfn.Normalize()
                                     << " overlaps with one on the list");
            return false;
        }
        while ((*rightIt).sfn < resourceInfo.sfn)
        {
            leftIt++;
            rightIt++;
            NS_ASSERT_MSG(leftIt != lastElementIt, "Unexpectedly reached end");
        }
        bool eligible = (IsMinTimeGapSatisfied((*leftIt).sfn,
                                               resourceInfo.sfn,
                                               (*leftIt).slMinTimeGapPsfch,
                                               (*leftIt).slMinTimeGapProcessing) &&
                         IsMinTimeGapSatisfied(resourceInfo.sfn,
                                               (*rightIt).sfn,
                                               (*rightIt).slMinTimeGapPsfch,
                                               (*rightIt).slMinTimeGapProcessing));
        if (eligible)
        {
            NS_LOG_DEBUG("Resource " << resourceInfo.sfn.Normalize() << " is eligible between "
                                     << (*leftIt).sfn.Normalize() << " and "
                                     << (*rightIt).sfn.Normalize());
        }
        else
        {
            NS_LOG_DEBUG("Resource " << resourceInfo.sfn.Normalize()
                                     << " does not meet constraints");
        }
        return eligible;
    }
    return true; // unreachable, but can silence compiler warning
}

Ptr<NrSlUeMacHarq>
NrSlUeMacSchedulerDefault::GetMacHarq(void) const
{
    if (!m_nrSlUeMacHarq)
    {
        PointerValue val;
        GetMac()->GetAttribute("NrSlUeMacHarq", val);
        m_nrSlUeMacHarq = val.Get<NrSlUeMacHarq>();
    }
    return m_nrSlUeMacHarq;
}

void
NrSlUeMacSchedulerDefault::SetMcs(uint32_t dstL2Id, uint8_t mcs)
{
    NS_LOG_FUNCTION(this << dstL2Id << mcs);
    // store MCS in m_mcsCache in case a dstInfo is not yet present
    std::optional<uint8_t> oldMcs;
    auto it = m_mcsCache.find(dstL2Id);
    if (it != m_mcsCache.end())
    {
        if (m_mcsCache[dstL2Id] != mcs)
        {
            NS_LOG_INFO("Changing MCS for destination " << dstL2Id << " from "
                                                        << +m_mcsCache[dstL2Id] << " to " << +mcs);
            oldMcs = m_mcsCache[dstL2Id];
            m_mcsCache[dstL2Id] = mcs;
        }
    }
    else
    {
        NS_LOG_DEBUG("Configuring MCS " << +mcs << " for new destination " << dstL2Id);
        oldMcs = mcs; // Setting this value triggers a trace below
        m_mcsCache.emplace(dstL2Id, mcs);
    }
    auto itDstInfo = m_dstMap.find(dstL2Id);
    if (itDstInfo != m_dstMap.end())
    {
        if (itDstInfo->second->GetDstMcs() != mcs)
        {
            NS_LOG_DEBUG("Changing MCS in DstInfo to " << +mcs << " for destination " << dstL2Id);
            itDstInfo->second->SetDstMcs(mcs);
        }
    }
    if (oldMcs.has_value())
    {
        m_mcsChangeTrace(dstL2Id, oldMcs.value(), mcs);
    }
}

const std::map<uint32_t, std::vector<NrSlUeMacScheduler::GrantInfo>>
NrSlUeMacSchedulerDefault::GetUnpublishedGrants() const
{
    NS_LOG_FUNCTION(this);
    return m_grantInfo;
}

void
NrSlUeMacSchedulerDefault::GetExternalGrants(
    std::map<uint32_t, std::vector<NrSlUeMacScheduler::GrantInfo>>& externalGrants) const
{
    NS_LOG_FUNCTION(this);

    for (auto i = NodeList::Begin(); i != NodeList::End(); ++i)
    {
        Ptr<Node> node = *i;
        for (uint32_t j = 0; j < node->GetNDevices(); ++j)
        {
            Ptr<NetDevice> netDev = node->GetDevice(j);
            Ptr<NrUeNetDevice> ueNetDev = netDev->GetObject<NrUeNetDevice>();
            if (!ueNetDev)
            {
                continue;
            }

            Ptr<NrSlUeMac> ueMac = ueNetDev->GetMac(0)->GetObject<NrSlUeMac>();
            NS_ASSERT_MSG(ueMac, "NrSlUeMac not found");

            PointerValue pv;
            ueMac->GetAttribute("NrSlUeMacScheduler", pv);

            Ptr<NrSlUeMacScheduler> base = pv.Get<NrSlUeMacScheduler>();
            Ptr<NrSlUeMacSchedulerDefault> sched = DynamicCast<NrSlUeMacSchedulerDefault>(base);
            NS_ASSERT_MSG(sched, "NrSlUeMacSchedulerDefault not found");
            const auto& nodeGrants = sched->GetUnpublishedGrants();

            if (sched == this || nodeGrants.size() == 0)
            {
                continue;
            }

            if (m_idealSchedLevel == IDEAL_SCHED_LOCAL_UNICAST)
            {
                Ptr<NrSlUeRrc> ueRrc = DynamicCast<NrSlUeRrc>(ueNetDev->GetRrc());
                NS_ASSERT_MSG(ueRrc, "NrSlUeRrc not found (RRC is not a NrSlUeRrc)");
                uint32_t peerL2Id = ueRrc->GetSourceL2Id();

                // Skip if no unicast logical channel was found with the peer node
                if (!HasUnicastLogicalChannelTo(peerL2Id))
                {
                    continue;
                }
            }

            NS_LOG_DEBUG("Node ID " << node->GetId() << " has " << nodeGrants.size()
                                    << " destinations possibly with unpublished grants");
            for (const auto& [dstL2Id, grants] : nodeGrants)
            {
                auto it = externalGrants.find(dstL2Id);
                if (it == externalGrants.end())
                {
                    // Start an entry for dstL2Id
                    it = externalGrants.emplace(dstL2Id, std::vector<GrantInfo>{}).first;
                }
                // Append the discovered external grant's items to the entry for dstL2Id
                auto& externalGrantVector = it->second;
                NS_LOG_DEBUG("dstL2Id's grant vector has " << grants.size()
                                                           << " unpublished grants");
#ifdef NS3_LOG_ENABLE
                // Avoid iterating when logging is disabled
                for (const auto& grantInfo : grants)
                {
                    for (const auto& slGrantResource : grantInfo.slotAllocations)
                    {
                        NS_LOG_DEBUG("Inserting grant entry for "
                                     << slGrantResource.sfn << " subchannels "
                                     << slGrantResource.slPsschSubChStart << ":"
                                     << slGrantResource.slPsschSubChStart +
                                            slGrantResource.slPsschSubChLength);
                    }
                }
#endif
                externalGrantVector.insert(externalGrantVector.end(), grants.begin(), grants.end());
            }
        }
    }
}

void
NrSlUeMacSchedulerDefault::SetMcsQueryIndicationCallback(
    Callback<void, const SfnSf&, uint32_t, uint32_t, uint32_t> mcsQueryIndicationCb)
{
    NS_LOG_FUNCTION(this);
    m_mcsQueryIndicationCb = mcsQueryIndicationCb;
}

bool
NrSlUeMacSchedulerDefault::HasUnicastLogicalChannelTo(uint32_t peerL2Id) const
{
    NS_LOG_FUNCTION(this << peerL2Id);
    auto itDst = m_dstMap.find(peerL2Id);
    if (itDst != m_dstMap.end())
    {
        const auto& lcgMap = itDst->second->GetNrSlLCG();
        for (auto it = lcgMap.begin(); it != lcgMap.end(); it++)
        {
            const std::vector<uint8_t> lcIds = it->second->GetLCId();
            for (uint8_t lcId : lcIds)
            {
                // Data LC IDs start at 5
                if (lcId > 4 && it->second->GetLcCastType(lcId) == SidelinkInfo::CastType::Unicast)
                {
                    NS_LOG_DEBUG("Unicast LC with peer L2 ID " << peerL2Id
                                                               << " found. LCID: " << +lcId);
                    return true;
                }
            }
        }
    }
    return false;
}

uint32_t
NrSlUeMacSchedulerDefault::CalculateNumTbNeeded(uint32_t bufferSize, uint32_t tbSize) const
{
    NS_LOG_FUNCTION(this << bufferSize << tbSize);

    if (!bufferSize)
    {
        return 0;
    }
    // Guard against an (unlikely) division by zero or unsigned underflow
    NS_ASSERT_MSG(tbSize > SUBHEADER_SIZE, "tbSize is too small");

    // The RLC exposes the aggregate size of its assumed PDUs to the scheduler.  Each PDU
    // requires a MAC subheader.  We estimate that each TB will carry one MAC subheader,
    // reducing the capacity per TB accordingly.
    uint32_t capacityPerTb = tbSize - SUBHEADER_SIZE;
    uint32_t numTb = bufferSize / capacityPerTb;
    numTb += (bufferSize % capacityPerTb) ? 1 : 0; // round up by one TB if not evenly divisible

    // When segmentation across multiple TBs is needed, each RLC PDU also
    // carries a fixed header (SN field) that further reduces payload capacity.
    // Recompute with the reduced capacity to avoid underestimating TBs needed.
    // Single-TB cases are unaffected (no segmentation overhead).
    if (numTb > 1 && tbSize > SUBHEADER_SIZE + RLC_HEADER_SIZE)
    {
        capacityPerTb = tbSize - SUBHEADER_SIZE - RLC_HEADER_SIZE;
        numTb = bufferSize / capacityPerTb;
        numTb += (bufferSize % capacityPerTb) ? 1 : 0;
    }

    NS_LOG_DEBUG("Buffer " << bufferSize << " bytes needs " << numTb << " TBs of " << tbSize
                           << " bytes each (capacity " << capacityPerTb << " bytes per TB)");
    return numTb;
}

std::list<SlResourceInfo>
NrSlUeMacSchedulerDefault::FilterUsedResources(const std::list<SlResourceInfo>& candidates,
                                               const std::set<SlGrantResource>& usedSlots) const
{
    NS_LOG_FUNCTION(this);
    std::list<SlResourceInfo> remaining;

    for (const auto& cand : candidates)
    {
        bool overlaps = false;
        for (const auto& used : usedSlots)
        {
            // Check if candidate slot matches any used slot
            if (cand.sfn == used.sfn)
            {
                // Check subchannel overlap
                uint16_t candEnd = cand.slSubchannelStart + cand.slSubchannelLength;
                uint16_t usedEnd = used.slPsschSubChStart + used.slPsschSubChLength;
                if (!(candEnd <= used.slPsschSubChStart || cand.slSubchannelStart >= usedEnd))
                {
                    overlaps = true;
                    break;
                }
            }
        }
        if (!overlaps)
        {
            remaining.push_back(cand);
        }
    }

    NS_LOG_DEBUG("Filtered " << (candidates.size() - remaining.size()) << " used resources, "
                             << remaining.size() << " remaining");
    return remaining;
}

} // namespace ns3
