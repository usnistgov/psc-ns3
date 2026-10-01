// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#define NS_LOG_APPEND_CONTEXT                                                                      \
    if (m_getCellId)                                                                               \
    {                                                                                              \
        std::clog << " [ CellId " << GetCellId() << ", bwpId " << GetBwpId() << "] ";              \
    }
#include "nr-mac-scheduler-cqi-management.h"

#include "nr-amc.h"

#include <ns3/log.h>
#include <ns3/nr-spectrum-value-helper.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerCQIManagement");

void
NrMacSchedulerCQIManagement::DlSBCQIReported(
    [[maybe_unused]] const DlCqiInfo& info,
    [[maybe_unused]] const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo) const
{
    NS_LOG_FUNCTION(this);
    // TODO
    NS_ABORT_MSG("SB CQI Type is not supported");
}

void
NrMacSchedulerCQIManagement::UlSBCQIReported(
    uint32_t expirationTime,
    [[maybe_unused]] uint32_t tbs,
    const NrMacSchedSapProvider::SchedUlCqiInfoReqParameters& params,
    const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
    const std::vector<bool>& rbgMask,
    uint32_t numRbPerRbg,
    const Ptr<const SpectrumModel>& model) const
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(!rbgMask.empty());

    NS_LOG_INFO("Computing SB CQI for UE " << ueInfo->m_rnti);

    ueInfo->m_ulCqi.m_sinr = params.m_ulCqi.m_sinr;
    ueInfo->m_ulCqi.m_cqiType = NrMacSchedulerUeInfo::CqiInfo::SB;
    ueInfo->m_ulCqi.m_timer = expirationTime;

    std::vector<int> rbAssignment(params.m_ulCqi.m_sinr.size(), 0);

    for (uint32_t i = 0; i < rbgMask.size(); ++i)
    {
        if (rbgMask.at(i) == 1)
        {
            for (uint32_t k = 0; k < numRbPerRbg; ++k)
            {
                rbAssignment[i * numRbPerRbg + k] = 1;
            }
        }
    }

    SpectrumValue specVals(model);
    Values::iterator specIt = specVals.ValuesBegin();

    std::stringstream out;

    for (uint32_t ichunk = 0; ichunk < model->GetNumBands(); ichunk++)
    {
        NS_ASSERT(specIt != specVals.ValuesEnd());
        if (rbAssignment[ichunk] == 1)
        {
            *specIt = ueInfo->m_ulCqi.m_sinr.at(ichunk);
            out << ueInfo->m_ulCqi.m_sinr.at(ichunk) << " ";
        }
        else
        {
            out << "0.0 ";
            *specIt = 0.0;
        }

        specIt++;
    }

    NS_LOG_INFO("Values of SINR to pass to the AMC: " << out.str());

    // MCS updated inside the function; crappy API... but we can't fix everything
    ueInfo->m_ulCqi.m_wbCqi = GetAmcUl()->CreateCqiFeedbackWbTdma(specVals, ueInfo->m_ulMcs);
    NS_LOG_DEBUG("Calculated MCS for RNTI " << ueInfo->m_rnti << " is " << ueInfo->m_ulMcs);
}

void
NrMacSchedulerCQIManagement::InstallGetBwpIdFn(const std::function<uint16_t()>& fn)
{
    NS_LOG_FUNCTION(this);
    m_getBwpId = fn;
}

void
NrMacSchedulerCQIManagement::InstallGetCellIdFn(const std::function<uint16_t()>& fn)
{
    NS_LOG_FUNCTION(this);
    m_getCellId = fn;
}

void
NrMacSchedulerCQIManagement::InstallGetStartMcsDlFn(const std::function<uint8_t()>& fn)
{
    NS_LOG_FUNCTION(this);
    m_getStartMcsDl = fn;
}

void
NrMacSchedulerCQIManagement::InstallGetStartMcsUlFn(const std::function<uint8_t()>& fn)
{
    NS_LOG_FUNCTION(this);
    m_getStartMcsUl = fn;
}

void
NrMacSchedulerCQIManagement::InstallGetNrAmcDlFn(const std::function<Ptr<const NrAmc>()>& fn)
{
    NS_LOG_FUNCTION(this);
    m_getAmcDl = fn;
}

void
NrMacSchedulerCQIManagement::InstallGetNrAmcUlFn(const std::function<Ptr<const NrAmc>()>& fn)
{
    NS_LOG_FUNCTION(this);
    m_getAmcUl = fn;
}

void
NrMacSchedulerCQIManagement::DlWBCQIReported(const DlCqiInfo& info,
                                             const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
                                             uint32_t expirationTime,
                                             int8_t maxDlMcs) const
{
    NS_LOG_FUNCTION(this);

    ueInfo->m_dlCqi.m_cqiType = NrMacSchedulerUeInfo::CqiInfo::WB;
    ueInfo->m_dlCqi.m_wbCqi = info.m_wbCqi;
    ueInfo->m_dlCqi.m_timer = expirationTime;
    ueInfo->m_dlCqi.m_wbCqi = info.m_wbCqi;
    ueInfo->m_dlMcs =
        std::min(static_cast<uint8_t>(GetAmcDl()->GetMcsFromCqi(ueInfo->m_dlCqi.m_wbCqi)),
                 static_cast<uint8_t>(maxDlMcs));
    NS_LOG_INFO("Calculated MCS for UE " << ueInfo->m_rnti << " is "
                                         << static_cast<uint32_t>(ueInfo->m_dlMcs));

    NS_LOG_INFO("Updated WB CQI of UE "
                << ueInfo->m_rnti << " to " << static_cast<uint32_t>(ueInfo->m_dlCqi.m_wbCqi)
                << ". It will expire in " << ueInfo->m_dlCqi.m_timer << " slots.");

    if (info.m_optPrecMat)
    {
        // Set the number of layers (rank) directly to m_ri (do not decode m_ri)
        NS_ASSERT(info.m_ri > 0);
        ueInfo->m_dlRank = info.m_ri;

        // Set the precoding matrix
        ueInfo->m_dlPrecMats = info.m_optPrecMat;
    }
}

void
NrMacSchedulerCQIManagement::RefreshDlCqiMaps(
    const std::unordered_map<uint16_t, std::shared_ptr<NrMacSchedulerUeInfo>>& ueMap) const
{
    NS_LOG_FUNCTION(this);

    for (const auto& itUe : ueMap)
    {
        const std::shared_ptr<NrMacSchedulerUeInfo>& ue = itUe.second;

        if (ue->m_dlCqi.m_timer == 0)
        {
            ue->m_dlCqi.m_wbCqi = 1; // lowest value for trying a transmission
            ue->m_dlCqi.m_cqiType = NrMacSchedulerUeInfo::CqiInfo::WB;
            ue->m_dlMcs = GetStartMcsDl();
        }
        else
        {
            ue->m_dlCqi.m_timer -= 1;
        }
    }
}

void
NrMacSchedulerCQIManagement::RefreshUlCqiMaps(
    const std::unordered_map<uint16_t, std::shared_ptr<NrMacSchedulerUeInfo>>& ueMap) const
{
    NS_LOG_FUNCTION(this);

    for (const auto& itUe : ueMap)
    {
        const std::shared_ptr<NrMacSchedulerUeInfo>& ue = itUe.second;

        if (ue->m_ulCqi.m_timer == 0)
        {
            ue->m_ulCqi.m_wbCqi = 1; // lowest value for trying a transmission
            ue->m_ulCqi.m_cqiType = NrMacSchedulerUeInfo::CqiInfo::WB;
            ue->m_ulMcs = GetStartMcsUl();
        }
        else
        {
            ue->m_ulCqi.m_timer -= 1;
        }
    }
}

uint16_t
NrMacSchedulerCQIManagement::GetBwpId() const
{
    return m_getBwpId();
}

uint16_t
NrMacSchedulerCQIManagement::GetCellId() const
{
    return m_getCellId();
}

uint8_t
NrMacSchedulerCQIManagement::GetStartMcsDl() const
{
    return m_getStartMcsDl();
}

uint8_t
NrMacSchedulerCQIManagement::GetStartMcsUl() const
{
    return m_getStartMcsUl();
}

Ptr<const NrAmc>
NrMacSchedulerCQIManagement::GetAmcDl() const
{
    return m_getAmcDl();
}

Ptr<const NrAmc>
NrMacSchedulerCQIManagement::GetAmcUl() const
{
    return m_getAmcUl();
}

} // namespace ns3
