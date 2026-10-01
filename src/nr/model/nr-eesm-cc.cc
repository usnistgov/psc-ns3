// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-eesm-cc.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEesmCc");
NS_OBJECT_ENSURE_REGISTERED(NrEesmCc);

NrEesmCc::NrEesmCc()
{
    NS_LOG_FUNCTION(this);
}

NrEesmCc::~NrEesmCc()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrEesmCc::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEesmCc").SetParent<NrEesmErrorModel>();
    return tid;
}

double
NrEesmCc::ComputeSINR(const SpectrumValue& sinr,
                      const std::vector<int>& map,
                      uint8_t mcs,
                      [[maybe_unused]] uint32_t sizeBit,
                      const NrErrorModel::NrErrorModelHistory& sinrHistory) const
{
    NS_LOG_FUNCTION(this);

    // HARQ CHASE COMBINING: update SINReff, but not ECR after retx
    // repetition of coded bits

    // make a vector of history that contains the last tx (but without modifying
    // sinrHistory, as it will be modified by the caller when it will be the time)
    Ptr<NrEesmErrorModelOutput> last = Create<NrEesmErrorModelOutput>(0.0);
    last->m_map = map;
    last->m_sinr = sinr;

    NrErrorModel::NrErrorModelHistory total = sinrHistory;
    total.emplace_back(last);

    // evaluate SINR_eff over "total", as per Chase Combining

    NS_ASSERT(sinr.GetSpectrumModel()->GetNumBands() == sinr.GetValuesN());

    SpectrumValue sinr_sum(sinr.GetSpectrumModel());
    uint32_t historySize = static_cast<uint32_t>(total.size());
    uint32_t maxRBUsed = 0;
    for (uint32_t i = 0; i < historySize; ++i)
    {
        Ptr<NrEesmErrorModelOutput> output = DynamicCast<NrEesmErrorModelOutput>(total.at(i));
        maxRBUsed = std::max(maxRBUsed, static_cast<uint32_t>(output->m_map.size()));
    }

    std::vector<int> map_sum;
    map_sum.reserve(maxRBUsed);

    for (uint32_t i = 0; i < maxRBUsed; ++i)
    {
        sinr_sum[i] = 0;
        map_sum.push_back(static_cast<int>(i));
    }

    /* combine at the bit level. Example:
     * SINR{1}=[0 0 10 20 10 0 0];
     * SINR{2}=[1 2 1 2 1 0 3];
     * SINR{3}=[5 0 0 0 0 0 0];
     *
     * map{1}=[2 3 4];
     * map{2}=[0 1 2 3 4 6];
     * map{3}=[0];
     *
     * MAP_SUM = [0 1 2 3 4 5]
     * SINR_SUM = [16 27 16 17 26 18]
     *
     * (the value at SINR_SUM[0] is SINR{1}[2] + SINR{2}[0] + SINR{3}[0])
     */
    for (uint32_t i = 0; i < historySize; ++i)
    {
        Ptr<NrEesmErrorModelOutput> output = DynamicCast<NrEesmErrorModelOutput>(total.at(i));
        uint32_t size = output->m_map.size();
        for (uint32_t j = 0; j < maxRBUsed; ++j)
        {
            sinr_sum[j] += output->m_sinr[output->m_map[j % size]];
        }
    }

    NS_LOG_INFO("\tHISTORY:");
    for (const auto& element : total)
    {
        Ptr<NrEesmErrorModelOutput> output = DynamicCast<NrEesmErrorModelOutput>(element);
        NS_LOG_INFO("\tMAP:" << PrintMap(output->m_map));
        NS_LOG_INFO("\tSINR: " << output->m_sinr);
    }

    NS_LOG_INFO("MAP_SUM: " << PrintMap(map_sum));
    NS_LOG_INFO("SINR_SUM: " << sinr_sum);

    // compute effective SINR with the sinr_sum vector and map_sum RB map
    return SinrEff(sinr_sum, map_sum, mcs, 0.0, map_sum.size());
}

double
NrEesmCc::GetMcsEq(uint8_t mcsTx) const
{
    NS_LOG_FUNCTION(this);
    return mcsTx;
}

} // namespace ns3
