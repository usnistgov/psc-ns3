// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-lc-qos.h"

#include "nr-phy-mac-common.h"

#include "ns3/log.h"

#include <algorithm>
#include <iostream>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerLcQos");
NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulerLcQos);

NrMacSchedulerLcQos::NrMacSchedulerLcQos()
    : NrMacSchedulerLcAlgorithm()
{
    NS_LOG_FUNCTION(this);
}

NrMacSchedulerLcQos::~NrMacSchedulerLcQos()
{
}

TypeId
NrMacSchedulerLcQos::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrMacSchedulerLcQos")
                            .SetParent<NrMacSchedulerLcAlgorithm>()
                            .AddConstructor<NrMacSchedulerLcQos>();
    return tid;
}

std::vector<NrMacSchedulerLcAlgorithm::Assignation>
NrMacSchedulerLcQos::AssignBytesToDlLC(const std::unordered_map<uint8_t, LCGPtr>& ueLCG,
                                       uint32_t tbs,
                                       Time slotPeriod) const
{
    NS_LOG_FUNCTION(this);
    GetFirst GetLCGID;
    GetSecond GetLCG;

    std::vector<NrMacSchedulerLcAlgorithm::Assignation> ret;

    NS_LOG_INFO("To distribute: " << tbs << " bytes over " << ueLCG.size() << " LCG"
                                  << " in Qos manner");

    uint32_t bytesLeftToBeAssigned = tbs;
    uint32_t bytesAssigned = 0;
    uint64_t sumErabGueanteedBitRate = 0;

    std::vector<std::pair<uint8_t, uint8_t>>
        gbrActiveLCs; // vector that stores the LCG ID and the LC ID of the gbr active LCs
    std::vector<std::pair<uint8_t, uint8_t>>
        restActiveLCs; // vector that stores the LCG ID and the LC ID of the rest active LCs

    std::vector<std::pair<uint8_t, uint32_t>>
        AssignedBytesToGbrLCsList; // vector that stores the LC ID with the bytes assigned for the
                                   // case of GBR LCs

    for (const auto& lcg : ueLCG)
    {
        std::vector<uint8_t> ueActiveLCs = GetLCG(lcg)->GetActiveLCIds();
        for (const auto lcId : ueActiveLCs)
        {
            if ((GetLCG(lcg)->GetLC(lcId)->m_resourceType ==
                     nr::LogicalChannelConfigListElement_s::QBT_DGBR ||
                 GetLCG(lcg)->GetLC(lcId)->m_resourceType ==
                     nr::LogicalChannelConfigListElement_s::QBT_GBR) &&
                GetLCG(lcg)->GetLC(lcId)->m_eRabGuaranteedBitrateDl != UINT64_MAX)
            {
                gbrActiveLCs.emplace_back(GetLCGID(lcg), lcId);
                sumErabGueanteedBitRate +=
                    (GetLCG(lcg)->GetLC(lcId)->m_eRabGuaranteedBitrateDl / 8);
            }
            restActiveLCs.emplace_back(GetLCGID(lcg), lcId);
        }
    }

    if (gbrActiveLCs.size() > 1 && sumErabGueanteedBitRate >= tbs)
    {
        uint32_t bytesPerLcGbr;

        if (bytesLeftToBeAssigned > 0)
        {
            NS_ASSERT(!gbrActiveLCs.empty());
            bytesPerLcGbr = bytesLeftToBeAssigned / gbrActiveLCs.size();

            for (const auto& lcg : ueLCG)
            {
                for (auto& itGbrActiveLCs : gbrActiveLCs)
                {
                    if (itGbrActiveLCs.first == GetLCGID(lcg))
                    {
                        AssignedBytesToGbrLCsList.emplace_back(itGbrActiveLCs.second,
                                                               bytesPerLcGbr);
                    }
                }
            }
            bytesLeftToBeAssigned = 0;
        }
    }
    else if (!gbrActiveLCs.empty())
    {
        for (const auto& lcg : ueLCG)
        {
            for (auto& itGbrActiveLCs : gbrActiveLCs)
            {
                if (itGbrActiveLCs.first == GetLCGID(lcg))
                {
                    NS_ASSERT_MSG(
                        GetLCG(lcg)->GetLC(itGbrActiveLCs.second)->m_eRabGuaranteedBitrateDl !=
                            UINT64_MAX,
                        "LC is not guaranteed bit rate!");

                    uint32_t bytes = std::min(
                        static_cast<uint32_t>(
                            slotPeriod.GetSeconds() *
                            (GetLCG(lcg)->GetLC(itGbrActiveLCs.second)->m_eRabGuaranteedBitrateDl /
                             8)),
                        GetLCG(lcg)->GetLC(itGbrActiveLCs.second)->GetTotalSize());

                    bytesAssigned = bytes >= bytesLeftToBeAssigned ? bytesLeftToBeAssigned : bytes;

                    AssignedBytesToGbrLCsList.emplace_back(itGbrActiveLCs.second, bytesAssigned);

                    NS_ASSERT(bytesLeftToBeAssigned >= bytesAssigned); // check
                    bytesLeftToBeAssigned -= bytesAssigned;
                }
            }
        }
    }

    uint32_t bytesPerLc;

    if (!restActiveLCs.empty() && bytesLeftToBeAssigned > 0)
    {
        NS_ASSERT(!restActiveLCs.empty());
        bytesPerLc = bytesLeftToBeAssigned / restActiveLCs.size();

        for (const auto& lcg : ueLCG)
        {
            for (auto& itRestActiveLCs : restActiveLCs)
            {
                if (itRestActiveLCs.first == GetLCGID(lcg))
                {
                    bool erabGbrTrue = false;

                    for (auto& it : AssignedBytesToGbrLCsList)
                    {
                        if (it.first == itRestActiveLCs.second && GetLCGID(lcg) == 1)
                        {
                            it.second += bytesPerLc;
                            erabGbrTrue = true;
                            break;
                        }
                    }

                    if (!erabGbrTrue)
                    {
                        NS_LOG_DEBUG("LC : " << +itRestActiveLCs.second
                                             << " bytes: " << bytesPerLc);
                        ret.emplace_back(GetLCGID(lcg), itRestActiveLCs.second, bytesPerLc);
                    }
                }
            }
        }
    }

    for (auto& it : AssignedBytesToGbrLCsList)
    {
        NS_LOG_DEBUG("LC : " << +it.first << " bytes: " << it.second);
        ret.emplace_back(1, it.first, it.second);
    }

    return ret;
}

std::vector<NrMacSchedulerLcAlgorithm::Assignation>
NrMacSchedulerLcQos::AssignBytesToUlLC(const std::unordered_map<uint8_t, LCGPtr>& ueLCG,
                                       uint32_t tbs) const
{
    NS_LOG_FUNCTION(this);
    GetFirst GetLCGID;
    GetSecond GetLCG;

    std::vector<NrMacSchedulerLcAlgorithm::Assignation> ret;

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

} // namespace ns3
