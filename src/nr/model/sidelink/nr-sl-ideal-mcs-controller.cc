//
// SPDX-License-Identifier: NIST-Software

#include "nr-sl-ideal-mcs-controller.h"

#include "nr-sl-spectrum-phy.h"
#include "nr-sl-ue-mac-scheduler-default.h"
#include "nr-sl-ue-phy.h"

#include <ns3/callback.h>
#include <ns3/double.h>
#include <ns3/node-list.h>
#include <ns3/node.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/simulator.h>
#include <ns3/uinteger.h>

#include <cstdint>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlIdealMcsController");

NS_OBJECT_ENSURE_REGISTERED(NrSlIdealMcsController);

TypeId
NrSlIdealMcsController::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlIdealMcsController")
            .SetParent<NrSlMcsController>()
            .AddConstructor<NrSlIdealMcsController>()
            .SetGroupName("nr")
            .AddAttribute("Margin",
                          "A margin (in dB) subtracted from the SINR "
                          "before computing the block error rate",
                          DoubleValue(0.0),
                          MakeDoubleAccessor(&NrSlIdealMcsController::m_sinrMargin),
                          MakeDoubleChecker<double>())
            .AddAttribute("TargetBler",
                          "The highest allowed block error rate for the calculated MCS",
                          DoubleValue(0.1),
                          MakeDoubleAccessor(&NrSlIdealMcsController::m_tblerTarget),
                          MakeDoubleChecker<double>())
            .AddAttribute("MaximumL2Id",
                          "The highest destination L2 ID value to control",
                          UintegerValue(200),
                          MakeUintegerAccessor(&NrSlIdealMcsController::m_maximumL2Id),
                          MakeUintegerChecker<uint32_t>());
    return tid;
}

NrSlIdealMcsController::NrSlIdealMcsController()
{
    NS_LOG_FUNCTION(this);
}

void
NrSlIdealMcsController::DoSetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler)
{
    NS_LOG_FUNCTION(this << scheduler);
    scheduler->SetMcsQueryIndicationCallback(
        MakeCallback(&NrSlIdealMcsController::NotifyMcsQueryIndication, this));
    m_srcL2Id = scheduler->GetMac()->GetImsi(); // Helper sets srcL2Id to the IMSI
    NS_LOG_DEBUG("Source L2 Id set to " << m_srcL2Id);
    Simulator::ScheduleWithContext(m_srcL2Id,
                                   Seconds(0),
                                   &NrSlIdealMcsController::ConfigureTraces,
                                   this);
}

void
NrSlIdealMcsController::ConfigureTraces(void)
{
    NS_LOG_FUNCTION(this);
    for (auto i = NodeList::Begin(); i != NodeList::End(); i++)
    {
        Ptr<Node> node = *i;
        for (uint32_t j = 0; j < node->GetNDevices(); j++)
        {
            Ptr<NrUeNetDevice> ueDevice = node->GetDevice(j)->GetObject<NrUeNetDevice>();
            if (ueDevice)
            {
                auto uePhy = ueDevice->GetPhy(0)->GetObject<NrSlUePhy>();
                NS_ASSERT(uePhy);
                auto ueSpectrumPhy = uePhy->GetSpectrumPhy()->GetObject<NrSlSpectrumPhy>();
                NS_ASSERT(ueSpectrumPhy);
                bool found = ueSpectrumPhy->TraceConnect(
                    "RxPsschTraceUe",
                    std::to_string(node->GetId()),
                    MakeCallback(&NrSlIdealMcsController::NotifyRxPssch, this));
                NS_ASSERT(found);
                if (node->GetId() == m_srcL2Id)
                {
                    m_slErrorModel = ueSpectrumPhy->GetSlErrorModel();
                }
            }
        }
    }
    NS_ASSERT_MSG(m_slErrorModel, "Error model not found");
}

void
NrSlIdealMcsController::NotifyMcsQueryIndication(const SfnSf& sfn,
                                                 const uint32_t dstL2Id,
                                                 const uint32_t tbSize,
                                                 const uint32_t numRbs)
{
    NS_LOG_FUNCTION(this << sfn << dstL2Id << tbSize << numRbs);
    if (dstL2Id > m_maximumL2Id)
    {
        return;
    }
    const auto& it = m_traceMap.find(dstL2Id);
    if (it == m_traceMap.end())
    {
        NS_LOG_INFO("No history for destination; setting MCS 0 in scheduler for " << dstL2Id);
        m_scheduler->SetMcs(dstL2Id, 0u);
        return;
    }

    // using the m_slErrorModel, find the MCS value that meets the PER requirement
    uint8_t bestMcs = BinarySearch(sfn.GetNumerology(), it->second, tbSize, numRbs);
    NS_LOG_INFO("Returning MCS " << +bestMcs << " to dstL2Id " << dstL2Id << " for SINR "
                                 << it->second.sinrDb);
    m_scheduler->SetMcs(dstL2Id, bestMcs);
}

void
NrSlIdealMcsController::Flush(const uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    m_traceMap.erase(dstL2Id);
}

uint8_t
NrSlIdealMcsController::BinarySearch(const uint8_t numerology,
                                     const TraceEntry& entry,
                                     const uint32_t tbSize,
                                     const uint32_t numRbs)
{
    NS_LOG_FUNCTION(numerology << entry.sinrDb << tbSize << numRbs);
    auto tbler =
        m_slErrorModel
            ->GetTbler(0, numerology, entry.rv, entry.sinrDb - m_sinrMargin, tbSize, numRbs, {});
    if (tbler > m_tblerTarget)
    {
        NS_LOG_DEBUG("No MCS meets target " << m_tblerTarget << "; MCS 0 selected for SINR(dB) "
                                            << entry.sinrDb << " margin(dB) " << m_sinrMargin
                                            << " TB(bytes) " << tbSize << " numerology "
                                            << +numerology << " TBLER " << tbler);
        return 0u;
    }
    tbler =
        m_slErrorModel
            ->GetTbler(28, numerology, entry.rv, entry.sinrDb - m_sinrMargin, tbSize, numRbs, {});
    if (tbler <= m_tblerTarget)
    {
        NS_LOG_DEBUG("Highest MCS meets target "
                     << m_tblerTarget << "; MCS 28 selected for SINR(dB) " << entry.sinrDb
                     << " margin(dB) " << m_sinrMargin << " TB(bytes) " << tbSize << " numerology "
                     << +numerology << " TBLER " << tbler);
        return 28u;
    }
    // Binary search through the remaining MCS values
    uint8_t bestMcs{0};
    double bestTbler{0};
    uint8_t low{0};
    uint8_t high{27};
    while (low <= high)
    {
        uint8_t mid = low + (high - low) / 2;
        tbler = m_slErrorModel->GetTbler(mid,
                                         numerology,
                                         entry.rv,
                                         entry.sinrDb - m_sinrMargin,
                                         tbSize,
                                         numRbs,
                                         {});
        if (tbler <= m_tblerTarget)
        {
            // tbler is within the limit, so mid could be the result.
            bestMcs = mid;
            bestTbler = tbler;
            // Search for a larger MCS.
            low = mid + 1;
        }
        else
        {
            // Search for a smaller MCS
            high = mid - 1;
        }
    }
    NS_LOG_DEBUG("MCS " << +bestMcs << " has BLER " << bestTbler << " below threshold "
                        << m_tblerTarget << " for SINR(dB) " << entry.sinrDb << " margin(dB) "
                        << m_sinrMargin << " TB(bytes) " << tbSize << " numerology "
                        << +numerology);
    return bestMcs;
}

void
NrSlIdealMcsController::NotifyRxPssch(std::string context, SlRxDataPacketTraceParams traceParams)
{
    NS_LOG_FUNCTION(this << context);
    double snrDb = 10 * std::log10(traceParams.m_sinr);
    // The reported SNR may not be a wideband SNR, but a sub-band (if the full bandwidth of
    // the resource pool was not used for that TB).  We determine the fraction of the full
    // bandwidth that this transmission used, and scale the snrDb down accordingly (if needed)
    // to make it a wideband SNR (in dB) and use that value below
    double resourceBlocksUsed = static_cast<double>(traceParams.m_rbAssignedNum);
    double resourceBlocksInPool =
        static_cast<double>(m_mac->GetTotalSubCh() * m_mac->GetNrSlSubChSize());
    if (resourceBlocksUsed < resourceBlocksInPool)
    {
        double widebandCorrectionDb = 10 * std::log10(resourceBlocksInPool / resourceBlocksUsed);
        NS_LOG_DEBUG("Reducing received " << snrDb << " dB by " << widebandCorrectionDb
                                          << " dB to calculate the wideband SNR");
        snrDb -= widebandCorrectionDb;
    }
    if (traceParams.m_srcL2Id == m_srcL2Id)
    {
        const auto [value, result] = m_traceMap.insert_or_assign(
            traceParams.m_dstL2Id,
            TraceEntry{.time = Now(), .sinrDb = snrDb, .rv = traceParams.m_rv});

        if (result)
        {
            NS_LOG_DEBUG("Controller " << m_srcL2Id << " inserted new entry for "
                                       << traceParams.m_dstL2Id << " of "
                                       << "rv: " << +value->second.rv
                                       << ", wideband snrDb: " << snrDb << " dB");
        }
        else
        {
            NS_LOG_DEBUG("Controller "
                         << m_srcL2Id << " updated entry for " << traceParams.m_dstL2Id << " to "
                         << "rv: " << +value->second.rv << ", wideband snrDb: " << snrDb << " dB");
        }
    }
}

void
NrSlIdealMcsController::DoDispose()
{
    NS_LOG_FUNCTION(this);
    NrSlMcsController::DoDispose();
}

int64_t
NrSlIdealMcsController::AssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    return NrSlMcsController::AssignStreams(stream);
}

} // namespace ns3
