// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-pm-search-full.h"

#include <ns3/boolean.h>
#include <ns3/simulator.h>
#include <ns3/uinteger.h>

#include <numeric>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrPmSearchFull");
NS_OBJECT_ENSURE_REGISTERED(NrPmSearchFull);

TypeId
NrPmSearchFull::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrPmSearchFull")
                            .SetParent<NrPmSearch>()
                            .AddConstructor<NrPmSearchFull>()
                            .AddAttribute("CodebookType",
                                          "Codebook class to be used",
                                          TypeIdValue(NrCbTwoPort::GetTypeId()),
                                          MakeTypeIdAccessor(&NrPmSearchFull::SetCodebookTypeId),
                                          MakeTypeIdChecker());
    return tid;
}

void
NrPmSearchFull::SetCodebookTypeId(const TypeId& typeId)
{
    m_cbFactory.SetTypeId(typeId);
}

void
NrPmSearchFull::SetCodebookAttribute(const std::string& attrName, const AttributeValue& attrVal)
{
    NS_LOG_FUNCTION(this);
    m_cbFactory.Set(attrName, attrVal);
}

void
NrPmSearchFull::InitCodebooks()
{
    m_cbFactory.Set("N1", UintegerValue(m_nGnbHPorts));
    m_cbFactory.Set("N2", UintegerValue(m_nGnbVPorts));
    m_cbFactory.Set("IsDualPol", BooleanValue(m_isGnbDualPol));

    auto maxRank = std::min({m_nRxPorts, m_nGnbPorts, static_cast<size_t>(m_rankLimit)});
    m_ranks.resize(maxRank);
    std::iota(m_ranks.begin(), m_ranks.end(), 1); // Fill ranks vector starting at 1
    m_rankParams.resize(maxRank + 1);
    for (auto rank : m_ranks)
    {
        m_cbFactory.Set("Rank", UintegerValue(rank));
        m_rankParams[rank].cb = m_cbFactory.Create<NrCbTypeOne>();
        m_rankParams[rank].cb->Init();
    }
}

PmCqiInfo
NrPmSearchFull::CreateCqiFeedbackMimo(const NrMimoSignal& rxSignalRb, PmiUpdate pmiUpdate)
{
    NS_LOG_FUNCTION(this);

    // Extract parameters from received signal
    auto nRows = rxSignalRb.m_chanMat.GetNumRows();
    auto nCols = rxSignalRb.m_chanMat.GetNumCols();
    NS_ASSERT_MSG(nRows == m_nRxPorts, "Channel mat has {} rows but UE has {} ports");
    NS_ASSERT_MSG(nCols == m_nGnbPorts, "Channel mat has {} cols but gNB has {} ports");

    // Compute the interference-normalized channel matrix
    auto rbNormChanMat = rxSignalRb.m_covMat.CalcIntfNormChannel(rxSignalRb.m_chanMat);

    // Update optimal precoding matrices based on received signal, if update is requested
    ConditionallyUpdatePrecoding(rbNormChanMat, pmiUpdate);

    // Iterate over the ranks, apply the optimal precoding matrix, create CQI message with TB size
    auto optPrecForRanks = std::vector<PmCqiInfo>{};
    for (auto rank : m_ranks)
    {
        auto cqiMsg = CreateCqiForRank(rank, rbNormChanMat);
        optPrecForRanks.emplace_back(std::move(cqiMsg));
        // Skip higher ranks when the current is incapable of maintaining the connection
        if (optPrecForRanks.back().m_wbCqi == 0)
        {
            if (optPrecForRanks.size() >= 2)
            {
                optPrecForRanks.pop_back();
            }
            break;
        }
    }

    // Find the rank which results in largest expected TB size, and return corresponding CQI/PMI
    auto optRankCqiMsg = *std::max_element(
        optPrecForRanks.begin(),
        optPrecForRanks.end(),
        [](const PmCqiInfo& a, const PmCqiInfo& b) { return a.m_tbSize < b.m_tbSize; });
    return optRankCqiMsg;
}

void
NrPmSearchFull::ConditionallyUpdatePrecoding(const NrIntfNormChanMat& rbNormChanMat,
                                             PmiUpdate pmiUpdate)
{
    if (pmiUpdate.updateWb)
    {
        UpdateAllPrecoding(rbNormChanMat);
    }
    else if (pmiUpdate.updateSb)
    {
        UpdateSubbandPrecoding(rbNormChanMat);
    }
}

void
NrPmSearchFull::UpdateAllPrecoding(const NrIntfNormChanMat& rbNormChanMat)
{
    // Compute downsampled channel per subband
    auto sbNormChanMat = SubbandDownsampling(rbNormChanMat);

    for (auto rank : m_ranks)
    {
        // Loop over wideband precoding matrices W1 (index i1).
        std::vector<Ptr<PrecMatParams>> optSubbandPrecoders{};
        auto numI1 = m_rankParams[rank].cb->GetNumI1();
        for (auto i1 = size_t{0}; i1 < numI1; i1++)
        {
            // Find the optimal subband PMI values (i2) for this particular i1
            auto subbandParams = FindOptSubbandPrecoding(sbNormChanMat, i1, rank);

            // Store the parameters for this wideband index i1
            optSubbandPrecoders.emplace_back(subbandParams);
        }

        // Find the optimal wideband PMI i1
        m_rankParams[rank].precParams =
            *std::max_element(optSubbandPrecoders.begin(),
                              optSubbandPrecoders.end(),
                              [](const Ptr<PrecMatParams>& a, const Ptr<PrecMatParams>& b) {
                                  return a->perfMetric < b->perfMetric;
                              });
    }
}

void
NrPmSearchFull::UpdateSubbandPrecoding(const NrIntfNormChanMat& rbNormChanMat)
{
    // Compute downsampled channel per subband
    auto sbNormChanMat = SubbandDownsampling(rbNormChanMat);
    for (auto rank : m_ranks)
    {
        // Recompute the best subband precoding (W2) for previously found W1 and store results
        auto& optPrec = m_rankParams[rank].precParams;
        NS_ASSERT(optPrec);
        auto wbPmi = optPrec->wbPmi;
        optPrec = FindOptSubbandPrecoding(sbNormChanMat, wbPmi, rank);
    }
}

PmCqiInfo
NrPmSearchFull::CreateCqiForRank(uint8_t rank, const NrIntfNormChanMat& rbNormChanMat) const
{
    // Get the previously computed optimal precoding matrix for this rank
    auto optPrec = m_rankParams[rank].precParams;
    NS_ASSERT_MSG(optPrec, "Tried to create a CQI message but precoding matrix does not exist");

    // Upsample/convert subband precoding matrix to full RB size (size of rbNormChanMat)
    auto rbPrecMat = SubbandUpsampling(optPrec->sbPrecMat, rbNormChanMat.GetNumPages());

    // Recompute SINR value for current channel (for all RBs)
    auto sinrMat = rbNormChanMat.ComputeSinrForPrecoding(rbPrecMat);

    // For the optimal precoding matrix, determine the achievable TB size and TBLER.
    auto mcsParams = m_amc->GetMaxMcsParams(sinrMat, m_subbandSize);

    // Clamp sub-band CQI according to 3GPP 2-bit overhead limit
    if (m_subbandCqiClamping)
    {
        for (auto& sbCqi : mcsParams.sbCqis)
        {
            auto diff = (int)sbCqi - (int)mcsParams.wbCqi;
            if (diff > 2)
            {
                sbCqi = mcsParams.wbCqi + 2;
            }
            else if (diff < -1)
            {
                sbCqi = mcsParams.wbCqi - 1;
            }
        }
    }

    // Store parameters for this rank
    auto cqiMsg = PmCqiInfo{
        .m_mcs = mcsParams.mcs,
        .m_rank = rank,
        .m_wbPmi = optPrec->wbPmi,
        .m_wbCqi = mcsParams.wbCqi,
        .m_sbCqis = mcsParams.sbCqis,
        .m_sbPmis = optPrec->sbPmis,
        .m_optPrecMat = Create<const ComplexMatrixArray>(std::move(rbPrecMat)),
        .m_tbSize = mcsParams.tbSize,
    };
    return cqiMsg;
}

Ptr<NrPmSearchFull::PrecMatParams>
NrPmSearchFull::FindOptSubbandPrecoding(const NrIntfNormChanMat& sbNormChanMat,
                                        size_t i1,
                                        uint8_t rank) const
{
    // Create the possible subband precoding matrices for each value of i2, and compute the
    // corresponding performance metric (channel capacity) for each subband and each i2.
    auto nSubbands = sbNormChanMat.GetNumPages();
    auto allPrecMats = CreateSubbandPrecoders(i1, rank, nSubbands);
    auto subbandMetricForPrec = ComputeCapacityForPrecoders(sbNormChanMat, allPrecMats);
    auto numI2 = allPrecMats.size();

    // For each subband, find the optimal value of i2 (subband PMI value)
    auto sbPmis = std::vector<size_t>(nSubbands);
    auto optSubbandMetric = DoubleMatrixArray{nSubbands};
    auto optPrecMat = allPrecMats[0]; // Initialize optimal precoding matrix
    for (auto iSb = size_t{0}; iSb < nSubbands; iSb++)
    {
        // Find the optimal value of i2 (subband PMI value) for the current subband
        for (auto i2 = size_t{0}; i2 < numI2; i2++)
        {
            if (subbandMetricForPrec(iSb, i2) > optSubbandMetric(iSb))
            {
                sbPmis[iSb] = i2;
                optSubbandMetric(iSb) = subbandMetricForPrec(iSb, i2);
            }
        }
        // Store the optimal precoding matrix for this subband
        for (size_t i = 0; i < optPrecMat.GetNumRows(); i++)
        {
            for (size_t j = 0; j < optPrecMat.GetNumCols(); j++)
            {
                optPrecMat(i, j, iSb) = allPrecMats[sbPmis[iSb]](i, j, iSb);
            }
        }
    }
    auto widebandMetric = optSubbandMetric.GetValues().sum();

    auto res = Create<NrPmSearchFull::PrecMatParams>();
    res->wbPmi = i1;
    res->sbPmis = sbPmis;
    res->sbPrecMat = optPrecMat;
    res->perfMetric = widebandMetric;
    return res;
}

std::vector<ComplexMatrixArray>
NrPmSearchFull::CreateSubbandPrecoders(size_t i1, uint8_t rank, size_t nSubbands) const
{
    const auto& cb = m_rankParams[rank].cb;
    auto numI2 = cb->GetNumI2();

    std::vector<ComplexMatrixArray> allPrecMats;

    for (auto i2 = size_t{0}; i2 < numI2; i2++)
    {
        auto basePrecMat = cb->GetBasePrecMat(i1, i2);
        auto sbPrecMat = ExpandPrecodingMatrix(basePrecMat, nSubbands);
        allPrecMats.emplace_back(sbPrecMat);
    }
    return allPrecMats;
}

ComplexMatrixArray
NrPmSearchFull::ExpandPrecodingMatrix(ComplexMatrixArray basePrecMat, size_t nSubbands)
{
    NS_ASSERT_MSG(basePrecMat.GetNumPages() == 1, "Expanding to 3D requires a 2D input");
    auto nRows = basePrecMat.GetNumRows();
    auto nCols = basePrecMat.GetNumCols();
    ComplexMatrixArray res{nRows, nCols, nSubbands};
    for (size_t p = 0; p < nSubbands; p++)
    {
        for (size_t i = 0; i < nRows; i++)
        {
            for (size_t j = 0; j < nCols; j++)
            {
                res(i, j, p) = basePrecMat(i, j);
            }
        }
    }
    return res;
}

DoubleMatrixArray
NrPmSearchFull::ComputeCapacityForPrecoders(const NrIntfNormChanMat& sbNormChanMat,
                                            std::vector<ComplexMatrixArray> allPrecMats) const
{
    auto nSubbands = sbNormChanMat.GetNumPages();
    auto numI2 = allPrecMats.size();
    // Loop over subband PMI value i2 and store the capacity for each subband and each i2
    DoubleMatrixArray subbandCap{nSubbands, numI2};
    for (auto i2 = size_t{0}; i2 < numI2; i2++)
    {
        const auto& sbPrecMat = allPrecMats[i2];
        auto sinr = sbNormChanMat.ComputeSinrForPrecoding(sbPrecMat);
        for (auto iSb = size_t{0}; iSb < nSubbands; iSb++)
        {
            double currCap = 0;
            for (size_t iLayer = 0; iLayer < sinr.GetNumRows(); iLayer++)
            {
                currCap += log2(1.0 + sinr(iLayer, iSb));
            }
            subbandCap(iSb, i2) = currCap;
        }
    }
    return subbandCap;
}

} // namespace ns3
