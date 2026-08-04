/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

// SPDX-License-Identifier: NIST-Software

#include "nr-sl-trace-helper.h"

#include <ns3/log.h>
#include <ns3/mobility-model.h>
#include <ns3/nr-sl-ue-mac.h>
#include <ns3/nr-sl-ue-phy.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-ue-rrc.h>
#include <ns3/output-stream-wrapper.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>
#include <ns3/trace-helper.h>

#include <set>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlTraceHelper");

NrSlTraceHelper::NrSlTraceHelper()
{
    NS_LOG_FUNCTION(this);
}

void
NrSlTraceHelper::TraceUePositions(NodeContainer n, Time interval, std::string traceFileName)
{
    NS_LOG_FUNCTION(this << &n << interval.As(Time::S) << traceFileName);
    AsciiTraceHelper ascii;
    if (traceFileName.empty())
    {
        m_uePositionStream = ascii.CreateFileStream("NrSlUePositionTrace.txt");
    }
    else
    {
        m_uePositionStream = ascii.CreateFileStream(traceFileName);
    }
    // UE positions tracing
    *m_uePositionStream->GetStream() << "Time (s)\tnodeId\tx\ty\tz" << std::endl;
    for (uint32_t i = 0; i < n.GetN(); ++i)
    {
        if (interval != Seconds(0))
        {
            // Reschedule if user set a non-zero recurrence interval
            TraceUePosition(n.Get(i), interval);
        }
    }
}

void
NrSlTraceHelper::TraceUePosition(Ptr<Node> n, Time interval)
{
    NS_LOG_FUNCTION(this);
    const auto mobility = n->GetObject<MobilityModel>();
    if (mobility)
    {
        const auto position = mobility->GetPosition();
        *m_uePositionStream->GetStream()
            << "\t" << Simulator::Now().GetSeconds() << "\t" << n->GetId() << "\t" << position.x
            << "\t" << position.y << "\t" << position.z << std::endl;
        Simulator::Schedule(interval, &NrSlTraceHelper::TraceUePosition, this, n, interval);
    }
}

void
NrSlTraceHelper::TraceSensingAlgorithm(Ptr<NrSlUeMac> mac, std::string traceFileName)
{
    NS_LOG_FUNCTION(this << mac << traceFileName);
    // Each Mac can be uniquely identified by the tuple of its IMSI and its BwpId
    std::string context =
        std::to_string(mac->GetImsi()) + std::string{":"} + std::to_string(mac->GetBwpId());
    NS_ABORT_MSG_IF(m_traceSensingStreams.contains(context),
                    "This Mac already enabled for sensing trace");
    AsciiTraceHelper ascii;
    m_traceSensingStreams.emplace(context, ascii.CreateFileStream(traceFileName));
    mac->TraceConnect("SensingAlgorithm",
                      context,
                      MakeCallback(&NrSlTraceHelper::TraceSensing, this));
}

void
NrSlTraceHelper::TraceSensingAlgorithm(Ptr<Node> node, std::string traceFileName)
{
    NS_LOG_FUNCTION(this << node << traceFileName);
    TraceSensingAlgorithm(GetNrSlUeMac(node), traceFileName);
}

void
NrSlTraceHelper::TraceSchedulingAlgorithm(Ptr<NrSlUeMac> mac, std::string traceFileName)
{
    NS_LOG_FUNCTION(this << mac << traceFileName);
    // Each Mac can be uniquely identified by the tuple of its IMSI and its BwpId
    std::string context =
        std::to_string(mac->GetImsi()) + std::string{":"} + std::to_string(mac->GetBwpId());
    NS_ABORT_MSG_IF(m_traceSchedulingStreams.contains(context),
                    "This Mac already enabled for scheduling trace");
    AsciiTraceHelper ascii;
    m_traceSchedulingStreams.emplace(context, ascii.CreateFileStream(traceFileName));
    PointerValue schedPtr;
    mac->GetAttribute("NrSlUeMacScheduler", schedPtr);
    auto sched = schedPtr.GetObject()->GetObject<NrSlUeMacSchedulerDefault>();
    sched->TraceConnect("SchedulingReport",
                        context,
                        MakeCallback(&NrSlTraceHelper::TraceScheduling, this));
}

void
NrSlTraceHelper::TraceSchedulingAlgorithm(Ptr<Node> node, std::string traceFileName)
{
    NS_LOG_FUNCTION(this << node << traceFileName);
    TraceSchedulingAlgorithm(GetNrSlUeMac(node), traceFileName);
}

void
NrSlTraceHelper::TraceSensing(std::string context,
                              const struct NrSlUeMac::SensingTraceReport& report,
                              const std::list<SlResourceInfo>& candidateResources,
                              const std::list<SensingData>& sensingData,
                              const std::list<SfnSf>& transmitHistory)
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_MSG_UNLESS(m_traceSensingStreams.contains(context), "Context not found");
    auto stream = m_traceSensingStreams[context]->GetStream();
    std::string::size_type pos = context.find(':');
    NS_ABORT_MSG_UNLESS(pos != std::string::npos, "Error in context string: " << context);
    std::string nodeStr = context.substr(0, pos);
    std::string bwpIdStr = context.substr(pos + 1);

    struct ResourceRecord
    {
        bool m_transmitted{false};
        bool m_sensed{false};
        double m_rsrp{-200};
        bool m_candidate{false};
    };

    // The zeroth entry is at slot (report.m_sfn - report.m_t0)
    // TS 38.214 Section 8.1.4 sensing window definition: [n-T0, n-Tproc0)
    // selection window definition: [n+T1, n+T2].  Therefore, there are
    // T0 + T2 + 1 slots in the whole span (the 1 is the current slot).
    uint32_t zerothSlot = report.m_sfn.Normalize() - report.m_t0;
    uint32_t window = report.m_t0 + report.m_t2 + 1;
    std::vector<std::vector<ResourceRecord>> resources(
        window,
        std::vector<ResourceRecord>(report.m_subchannels));
    NS_LOG_DEBUG("Time " << Now().GetSeconds() << " Node " << nodeStr << " BwpId " << bwpIdStr);
    NS_LOG_DEBUG("    Sensing report");
    NS_LOG_DEBUG("      Sfn " << report.m_sfn.Normalize());
    NS_LOG_DEBUG("      zeroth " << zerothSlot);
    NS_LOG_DEBUG("      Subch " << report.m_subchannels);
    NS_LOG_DEBUG("      T0 " << report.m_t0);
    NS_LOG_DEBUG("      T2 " << report.m_t2);
    NS_LOG_DEBUG("      Tproc0 " << +report.m_tProc0);
    NS_LOG_DEBUG("    Candidates");
    for (const auto& it : candidateResources)
    {
        uint32_t index = it.sfn.Normalize() - zerothSlot;
        NS_ASSERT_MSG(index < window,
                      "Index out of bounds error candidates: index "
                          << index << " window " << window << " sfn " << it.sfn.Normalize()
                          << " zerothSlot " << zerothSlot);
        NS_LOG_DEBUG("      " << it.sfn.Normalize() << " " << index << " " << +it.slSubchannelStart
                              << " " << +it.slSubchannelLength);
        for (uint32_t j = it.slSubchannelStart; j < it.slSubchannelStart + it.slSubchannelLength;
             j++)
        {
            resources[index][j].m_candidate = true;
        }
    }
    NS_LOG_DEBUG("    Sensing data");
    for (const auto& it : sensingData)
    {
        NS_LOG_DEBUG("      " << it.sfn.Normalize() << " " << it.slRsrp << " " << +it.sbChStart
                              << " " << +it.sbChLength);
        uint32_t index = it.sfn.Normalize() - zerothSlot;
        NS_ASSERT_MSG(index < window,
                      "Index out of bounds error sensing data: index "
                          << index << " window " << window << " sfn " << it.sfn.Normalize()
                          << " zerothSlot " << zerothSlot);
        for (uint32_t j = it.sbChStart; j < (it.sbChStart + it.sbChLength); j++)
        {
            resources[index][j].m_sensed = true;
            resources[index][j].m_rsrp = it.slRsrp;
        }
    }
    NS_LOG_DEBUG("    Tx history");
    for (const auto& it : transmitHistory)
    {
        NS_LOG_DEBUG("      " << it.Normalize());
        uint32_t index = it.Normalize() - zerothSlot;
        NS_ASSERT_MSG(index < window,
                      "Index out of bounds error transmit history: " << index << " window "
                                                                     << window);
        for (uint32_t j = 0; j < report.m_subchannels; j++)
        {
            resources[index][j].m_transmitted = true;
        }
    }
    *stream << "#Trace,time=" << Simulator::Now().GetSeconds()
            << ",slot=" << report.m_sfn.Normalize() << ",node=" << nodeStr << ",bwpId=" << bwpIdStr
            << ",len=" << window << ",subch=" << report.m_subchannels
            << ",LsubCH=" << report.m_lSubch << ",t0=" << report.m_t0 << ",t1=" << +report.m_t1
            << ",t2=" << report.m_t2 << std::endl;
    for (uint32_t subch = report.m_subchannels; subch > 0; subch--)
    {
        for (uint32_t slot = 0; slot < window; slot++)
        {
            if (slot > 0)
            {
                *stream << ",";
            }
            if (resources[slot][subch - 1].m_sensed)
            {
                *stream << resources[slot][subch - 1].m_rsrp;
            }
            else if (resources[slot][subch - 1].m_transmitted)
            {
                *stream << "-200";
            }
            else if (resources[slot][subch - 1].m_candidate)
            {
                *stream << "200";
            }
            else
            {
                *stream << "0";
            }
        }
        *stream << std::endl;
    }
}

void
NrSlTraceHelper::TraceScheduling(
    std::string context,
    const struct NrSlUeMacSchedulerDefault::SchedulingReport& report,
    const std::list<SlResourceInfo>& candidateResources,
    const struct NrSlUeMac::NrSlTransmissionParams& params,
    const std::vector<SlGrantResource>& publishedResources,
    const std::map<uint32_t, std::vector<NrSlUeMacScheduler::GrantInfo>>& existingGrants,
    const struct NrSlUeMacScheduler::GrantInfo& newGrant)
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_MSG_UNLESS(m_traceSchedulingStreams.contains(context), "Context not found");
    auto stream = m_traceSchedulingStreams[context]->GetStream();
    std::string::size_type pos = context.find(':');
    NS_ABORT_MSG_UNLESS(pos != std::string::npos, "Error in context string: " << context);
    std::string nodeStr = context.substr(0, pos);
    std::string bwpIdStr = context.substr(pos + 1);

    struct ResourceRecord
    {
        bool m_existingGrant{false};
        bool m_newGrant{false};
        bool m_unusedCandidate{false};
    };

    // window to plot ranges from [report.m_sfn to (report.m_sfn + report.m_t2)]
    uint32_t window = report.m_t2 + 1;
    std::vector<std::vector<ResourceRecord>> resources(
        window,
        std::vector<ResourceRecord>(report.m_subchannels));
    NS_LOG_DEBUG("Time " << Now().GetSeconds() << " Node " << nodeStr << " BwpId " << bwpIdStr);
    NS_LOG_DEBUG("    Scheduling report");
    NS_LOG_DEBUG("      Sfn " << report.m_sfn.Normalize());
    NS_LOG_DEBUG("      subch " << report.m_subchannels);
    NS_LOG_DEBUG("      psfchPeriod " << report.m_psfchPeriod);
    NS_LOG_DEBUG("      cReselCounter " << newGrant.cReselCounter);
    NS_LOG_DEBUG("      slResoReselCounter " << +newGrant.slResoReselCounter);
    NS_LOG_DEBUG("      harqId " << +newGrant.harqId);
    NS_LOG_DEBUG("      harqEnabled " << newGrant.harqEnabled);
    NS_LOG_DEBUG("      nSelected " << +newGrant.nSelected);
    NS_LOG_DEBUG("      rri " << newGrant.rri.As(Time::MS));
    NS_LOG_DEBUG("    Candidates");
    for (const auto& it : candidateResources)
    {
        NS_ASSERT_MSG(report.m_sfn.Normalize() <= it.sfn.Normalize(), "Index out of bounds error");
        uint32_t index = it.sfn.Normalize() - report.m_sfn.Normalize();
        NS_ASSERT_MSG(index < window, "Index out of bounds error");

        NS_LOG_DEBUG("      " << it.sfn.Normalize() << " " << index << " " << +it.slSubchannelStart
                              << " " << +it.slSubchannelLength);
        for (uint32_t j = it.slSubchannelStart; j < it.slSubchannelStart + it.slSubchannelLength;
             j++)
        {
            resources[index][j].m_unusedCandidate = true;
        }
    }
    NS_LOG_DEBUG("    Published grants");
    for (const auto& it : publishedResources)
    {
        NS_ASSERT_MSG(report.m_sfn.Normalize() <= it.sfn.Normalize(), "Index out of bounds error");
        uint32_t index = it.sfn.Normalize() - report.m_sfn.Normalize();
        NS_LOG_DEBUG("      " << it.sfn.Normalize() << " " << index << " " << +it.slPsschSubChStart
                              << " " << +it.slPsschSubChLength);
        for (uint16_t j = it.slPsschSubChStart; j < it.slPsschSubChStart + it.slPsschSubChLength;
             j++)
        {
            resources[index][j].m_existingGrant = true;
        }
    }
    // Each item in published grants map is a dstL2Id and a grant.  The grant is a std::set of
    // SlGrantResources (slot allocations)
    NS_LOG_DEBUG("    Unpublished grants");
    for (const auto& it : existingGrants)
    {
        for (const auto& it2 : it.second)
        {
            for (const auto& it3 : it2.slotAllocations)
            {
                uint32_t index = it3.sfn.Normalize() - report.m_sfn.Normalize();
                if (index >= window)
                {
                    // dimension of vector is 'window'; i.e. [0,(window - 1)].
                    // prevent indexing out of bounds (if index >= window)
                    continue;
                }
                NS_LOG_DEBUG("      " << it3.sfn.Normalize() << " " << index << " "
                                      << +it3.slPsschSubChStart << " " << +it3.slPsschSubChLength);
                for (uint16_t j = it3.slPsschSubChStart;
                     j < it3.slPsschSubChStart + it3.slPsschSubChLength;
                     j++)
                {
                    NS_ASSERT_MSG(!resources[index][j].m_existingGrant,
                                  "Overlap between published/unpublished grant");
                    resources[index][j].m_existingGrant = true;
                }
            }
        }
    }
    NS_LOG_DEBUG("    New grant");
    uint16_t lSubCh{65535}; // Initialized to an operationally invalid value
    for (const auto& it : newGrant.slotAllocations)
    {
        uint32_t index = it.sfn.Normalize() - report.m_sfn.Normalize();
        if (index >= window)
        {
            // dimension of vector is 'window'; i.e. [0,(window - 1)].
            // prevent indexing out of bounds (if index >= window)
            continue;
        }
        NS_LOG_DEBUG("      " << it.sfn.Normalize() << " " << index << " " << +it.slPsschSubChStart
                              << " " << +it.slPsschSubChLength);
        for (uint16_t j = it.slPsschSubChStart; j < it.slPsschSubChStart + it.slPsschSubChLength;
             j++)
        {
            NS_ASSERT_MSG(!resources[index][j].m_existingGrant,
                          "Overlap between existing and new grant");
            lSubCh = it.slPsschSubChLength;
            resources[index][j].m_newGrant = true;
        }
    }
    *stream << "#Trace,time=" << Simulator::Now().GetSeconds()
            << ",slot=" << report.m_sfn.Normalize() << ",node=" << nodeStr << ",bwpId=" << bwpIdStr
            << ",len=" << window << ",subch=" << report.m_subchannels << ",lSubCh=" << lSubCh
            << ",PsfchPeriod=" << report.m_psfchPeriod << ",nSelected=" << +newGrant.nSelected
            << ",rri=" << newGrant.rri.GetMilliSeconds() << ",harqEnabled=" << newGrant.harqEnabled
            << std::endl;
    for (uint32_t subch = report.m_subchannels; subch > 0; subch--)
    {
        for (uint32_t slot = 0; slot < window; slot++)
        {
            if (slot > 0)
            {
                *stream << ",";
            }
            if (resources[slot][subch - 1].m_newGrant)
            {
                *stream << "75";
            }
            else if (resources[slot][subch - 1].m_existingGrant)
            {
                *stream << "-75";
            }
            else if (resources[slot][subch - 1].m_unusedCandidate)
            {
                *stream << "-175";
            }
            else
            {
                *stream << "0";
            }
        }
        *stream << std::endl;
    }
}

Ptr<NrSlUeMac>
NrSlTraceHelper::GetNrSlUeMac(Ptr<Node> node) const
{
    bool found = false;
    Ptr<NrSlUeMac> mac;
    for (uint32_t i = 0; i < node->GetNDevices(); i++)
    {
        Ptr<NrUeNetDevice> nd = node->GetDevice(i)->GetObject<NrUeNetDevice>();
        if (!nd)
        {
            continue;
        }
        Ptr<NrSlUeRrc> nrUeRrc = nd->GetRrc()->GetObject<NrSlUeRrc>();
        std::set<uint8_t> bwpIds = nrUeRrc->GetNrSlBwpIdContainer();
        for (const auto& i : nrUeRrc->GetNrSlBwpIdContainer())
        {
            mac = nd->GetMac(i)->GetObject<NrSlUeMac>();
            if (mac)
            {
                if (found)
                {
                    NS_ABORT_MSG_IF(found, "More than one NrSlUeMac objects found");
                }
                found = true;
            }
        }
    }
    NS_ABORT_MSG_UNLESS(found, "No NrSlUeMac objects found");
    return mac;
}

Ptr<NrSlUeMacScheduler>
NrSlTraceHelper::GetNrSlUeMacScheduler(Ptr<Node> node) const
{
    auto mac = GetNrSlUeMac(node);
    PointerValue val;
    mac->GetAttribute("NrSlUeMacScheduler", val);
    NS_ABORT_MSG_UNLESS(val.Get<NrSlUeMacScheduler>(), "No scheduler found");
    return val.Get<NrSlUeMacScheduler>();
}

Ptr<NrSlSpectrumPhy>
NrSlTraceHelper::GetNrSlSpectrumPhy(Ptr<Node> node) const
{
    for (uint32_t j = 0; j < node->GetNDevices(); j++)
    {
        Ptr<NrUeNetDevice> ueDevice = node->GetDevice(j)->GetObject<NrUeNetDevice>();
        if (ueDevice)
        {
            auto uePhy = ueDevice->GetPhy(0)->GetObject<NrSlUePhy>();
            NS_ASSERT(uePhy);
            auto ueSpectrumPhy = uePhy->GetSpectrumPhy()->GetObject<NrSlSpectrumPhy>();
            NS_ASSERT(ueSpectrumPhy);
            return ueSpectrumPhy;
        }
    }
    return nullptr;
}

} // namespace ns3
