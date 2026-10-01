// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-lc-rr.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerLcRR");
NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulerLcRR);

NrMacSchedulerLcRR::NrMacSchedulerLcRR()
    : NrMacSchedulerLcAlgorithm()
{
    NS_LOG_FUNCTION(this);
}

NrMacSchedulerLcRR::~NrMacSchedulerLcRR()
{
}

TypeId
NrMacSchedulerLcRR::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrMacSchedulerLcRR")
                            .SetParent<NrMacSchedulerLcAlgorithm>()
                            .AddConstructor<NrMacSchedulerLcRR>();
    return tid;
}

std::vector<NrMacSchedulerLcAlgorithm::Assignation>
NrMacSchedulerLcRR::AssignBytesToDlLC(const std::unordered_map<uint8_t, LCGPtr>& ueLCG,
                                      uint32_t tbs,
                                      [[maybe_unused]] Time slotPeriod) const
{
    return AssignBytesToLC(ueLCG, tbs);
}

std::vector<NrMacSchedulerLcAlgorithm::Assignation>
NrMacSchedulerLcRR::AssignBytesToUlLC(const std::unordered_map<uint8_t, LCGPtr>& ueLCG,
                                      uint32_t tbs) const
{
    return AssignBytesToLC(ueLCG, tbs);
}

std::vector<NrMacSchedulerLcAlgorithm::Assignation>
NrMacSchedulerLcRR::AssignBytesToLC(const std::unordered_map<uint8_t, LCGPtr>& ueLCG,
                                    uint32_t tbs) const
{
    NS_LOG_FUNCTION(this);
    GetFirst GetLCGID;
    GetSecond GetLCG;

    std::vector<NrMacSchedulerLcAlgorithm::Assignation> ret;

    NS_LOG_INFO("To distribute: " << tbs << " bytes over " << ueLCG.size() << " LCG");

    uint32_t activeLc = 0;
    for (const auto& lcg : ueLCG)
    {
        std::vector<uint8_t> lcs = GetLCG(lcg)->GetLCId();
        for (const auto& lcId : lcs)
        {
            if (GetLCG(lcg)->GetTotalSizeOfLC(lcId) > 0)
            {
                ++activeLc;
            }
        }
    }

    if (activeLc == 0)
    {
        return ret;
    }

    uint32_t amountPerLC = tbs / activeLc;
    NS_LOG_INFO("Total LC: " << activeLc << " each one will receive " << amountPerLC << " bytes");

    for (const auto& lcg : ueLCG)
    {
        std::vector<uint8_t> lcs = GetLCG(lcg)->GetLCId();
        for (const auto& lcId : lcs)
        {
            if (GetLCG(lcg)->GetTotalSizeOfLC(lcId) > 0)
            {
                NS_LOG_INFO("Assigned to LCID " << static_cast<uint32_t>(lcId) << " inside LCG "
                                                << static_cast<uint32_t>(GetLCGID(lcg))
                                                << " an amount of " << amountPerLC << " B");
                ret.emplace_back(GetLCGID(lcg), lcId, amountPerLC);
            }
        }
    }

    return ret;
}

}; // namespace ns3
