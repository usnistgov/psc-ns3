// Copyright (c) 2021 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduling-stats.h"

#include "ns3/string.h"
#include <ns3/log.h>
#include <ns3/simulator.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulingStats");

NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulingStats);

NrMacSchedulingStats::NrMacSchedulingStats()
{
    NS_LOG_FUNCTION(this);
    SetDlOutputFilename(GetDlOutputFilename());
    SetUlOutputFilename(GetUlOutputFilename());
}

NrMacSchedulingStats::~NrMacSchedulingStats()
{
    NS_LOG_FUNCTION(this);
    if (outDlFile.is_open())
    {
        outDlFile.close();
    }
    if (outUlFile.is_open())
    {
        outUlFile.close();
    }
}

TypeId
NrMacSchedulingStats::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrMacSchedulingStats")
            .SetParent<NrStatsCalculator>()
            .SetGroupName("nr")
            .AddConstructor<NrMacSchedulingStats>()
            .AddAttribute("DlOutputFilename",
                          "Name of the file where the downlink results will be saved.",
                          StringValue("NrDlMacStats.txt"),
                          MakeStringAccessor(&NrMacSchedulingStats::SetDlOutputFilename),
                          MakeStringChecker())
            .AddAttribute("UlOutputFilename",
                          "Name of the file where the uplink results will be saved.",
                          StringValue("NrUlMacStats.txt"),
                          MakeStringAccessor(&NrMacSchedulingStats::SetUlOutputFilename),
                          MakeStringChecker());
    return tid;
}

void
NrMacSchedulingStats::SetUlOutputFilename(std::string outputFilename)
{
    NrStatsCalculator::SetUlOutputFilename(outputFilename);
    if (outUlFile.is_open())
    {
        outUlFile.close();
    }
    outUlFile.open(GetUlOutputFilename().c_str());
    if (!outUlFile.is_open())
    {
        NS_LOG_ERROR("Can't open file " << GetUlOutputFilename().c_str());
        return;
    }
    outUlFile << "% "
                 "time(s)"
                 "\tcellId\tbwpId\tIMSI\tRNTI\tframe\tsframe\tslot\tsymStart\tnumSym\thar"
                 "qId\tndi\trv\tmcs\ttbSize";
    outUlFile << std::endl;
}

std::string
NrMacSchedulingStats::GetUlOutputFilename()
{
    return NrStatsCalculator::GetUlOutputFilename();
}

void
NrMacSchedulingStats::SetDlOutputFilename(std::string outputFilename)
{
    NrStatsCalculator::SetDlOutputFilename(outputFilename);
    if (outDlFile.is_open())
    {
        outDlFile.close();
    }
    outDlFile.open(GetDlOutputFilename().c_str());
    if (!outDlFile.is_open())
    {
        NS_LOG_ERROR("Can't open file " << GetDlOutputFilename().c_str());
        return;
    }
    outDlFile << "% "
                 "time(s)"
                 "\tcellId\tbwpId\tIMSI\tRNTI\tframe\tsframe\tslot\tsymStart\tnumSym\thar"
                 "qId\tndi\trv\tmcs\ttbSize";
    outDlFile << std::endl;
}

std::string
NrMacSchedulingStats::GetDlOutputFilename()
{
    return NrStatsCalculator::GetDlOutputFilename();
}

void
NrMacSchedulingStats::DlScheduling(uint16_t cellId,
                                   uint64_t imsi,
                                   const NrSchedulingCallbackInfo& traceInfo)
{
    NS_LOG_FUNCTION(this << cellId << imsi << traceInfo.m_frameNum << traceInfo.m_subframeNum
                         << traceInfo.m_rnti << (uint32_t)traceInfo.m_mcs << traceInfo.m_tbSize);
    NS_LOG_INFO("Write DL Mac Stats in " << GetDlOutputFilename().c_str());

    outDlFile << Simulator::Now().GetSeconds() << "\t";
    outDlFile << (uint32_t)cellId << "\t";
    outDlFile << (uint32_t)traceInfo.m_bwpId << "\t";
    outDlFile << imsi << "\t";
    outDlFile << traceInfo.m_rnti << "\t";
    outDlFile << traceInfo.m_frameNum << "\t";
    outDlFile << (uint32_t)traceInfo.m_subframeNum << "\t";
    outDlFile << traceInfo.m_slotNum << "\t";
    outDlFile << (uint32_t)traceInfo.m_symStart << "\t";
    outDlFile << (uint32_t)traceInfo.m_numSym << "\t";
    outDlFile << (uint32_t)traceInfo.m_harqId << "\t";
    outDlFile << (uint32_t)traceInfo.m_ndi << "\t";
    outDlFile << (uint32_t)traceInfo.m_rv << "\t";
    outDlFile << (uint32_t)traceInfo.m_mcs << "\t";
    outDlFile << traceInfo.m_tbSize << std::endl;
}

void
NrMacSchedulingStats::UlScheduling(uint16_t cellId,
                                   uint64_t imsi,
                                   const NrSchedulingCallbackInfo& traceInfo)
{
    NS_LOG_FUNCTION(this << cellId << imsi << traceInfo.m_frameNum << traceInfo.m_subframeNum
                         << traceInfo.m_rnti << (uint32_t)traceInfo.m_mcs << traceInfo.m_tbSize);
    NS_LOG_INFO("Write UL Mac Stats in " << GetUlOutputFilename().c_str());

    outUlFile << Simulator::Now().GetSeconds() << "\t";
    outUlFile << (uint32_t)cellId << "\t";
    outUlFile << (uint32_t)traceInfo.m_bwpId << "\t";
    outUlFile << imsi << "\t";
    outUlFile << traceInfo.m_rnti << "\t";
    outUlFile << traceInfo.m_frameNum << "\t";
    outUlFile << (uint32_t)traceInfo.m_subframeNum << "\t";
    outUlFile << traceInfo.m_slotNum << "\t";
    outUlFile << (uint32_t)traceInfo.m_symStart << "\t";
    outUlFile << (uint32_t)traceInfo.m_numSym << "\t";
    outUlFile << (uint32_t)traceInfo.m_harqId << "\t";
    outUlFile << (uint32_t)traceInfo.m_ndi << "\t";
    outUlFile << (uint32_t)traceInfo.m_rv << "\t";
    outUlFile << (uint32_t)traceInfo.m_mcs << "\t";
    outUlFile << traceInfo.m_tbSize << std::endl;
}

void
NrMacSchedulingStats::DlSchedulingCallback(Ptr<NrMacSchedulingStats> macStats,
                                           std::string path,
                                           NrSchedulingCallbackInfo traceInfo)
{
    NS_LOG_FUNCTION(macStats << path);
    uint64_t imsi = 0;
    std::ostringstream pathAndRnti;
    std::string pathGnb = path.substr(0, path.find("/BandwidthPartMap"));
    pathAndRnti << pathGnb << "/NrGnbRrc/UeMap/" << traceInfo.m_rnti;
    if (macStats->ExistsImsiPath(pathAndRnti.str()))
    {
        imsi = macStats->GetImsiPath(pathAndRnti.str());
    }
    else
    {
        imsi = FindImsiFromGnbRlcPath(pathAndRnti.str());
        macStats->SetImsiPath(pathAndRnti.str(), imsi);
    }
    uint16_t cellId = 0;
    if (macStats->ExistsCellIdPath(pathAndRnti.str()))
    {
        cellId = macStats->GetCellIdPath(pathAndRnti.str());
    }
    else
    {
        cellId = FindCellIdFromGnbRlcPath(pathAndRnti.str());
        macStats->SetCellIdPath(pathAndRnti.str(), cellId);
    }

    macStats->DlScheduling(cellId, imsi, traceInfo);
}

void
NrMacSchedulingStats::UlSchedulingCallback(Ptr<NrMacSchedulingStats> macStats,
                                           std::string path,
                                           NrSchedulingCallbackInfo traceInfo)
{
    NS_LOG_FUNCTION(macStats << path);

    uint64_t imsi = 0;
    std::ostringstream pathAndRnti;
    std::string pathGnb = path.substr(0, path.find("/BandwidthPartMap"));
    pathAndRnti << pathGnb << "/NrGnbRrc/UeMap/" << traceInfo.m_rnti;
    if (macStats->ExistsImsiPath(pathAndRnti.str()))
    {
        imsi = macStats->GetImsiPath(pathAndRnti.str());
    }
    else
    {
        imsi = FindImsiFromGnbRlcPath(pathAndRnti.str());
        macStats->SetImsiPath(pathAndRnti.str(), imsi);
    }
    uint16_t cellId = 0;
    if (macStats->ExistsCellIdPath(pathAndRnti.str()))
    {
        cellId = macStats->GetCellIdPath(pathAndRnti.str());
    }
    else
    {
        cellId = FindCellIdFromGnbRlcPath(pathAndRnti.str());
        macStats->SetCellIdPath(pathAndRnti.str(), cellId);
    }

    macStats->UlScheduling(cellId, imsi, traceInfo);
}

} // namespace ns3
