// Copyright (c) 2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-error-model.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrErrorModel");
NS_OBJECT_ENSURE_REGISTERED(NrErrorModel);

NrErrorModel::NrErrorModel()
    : Object()
{
    NS_LOG_FUNCTION(this);
}

NrErrorModel::~NrErrorModel()
{
}

TypeId
NrErrorModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrErrorModel").SetParent<Object>();
    return tid;
}

Ptr<NrErrorModelOutput>
NrErrorModel::GetTbDecodificationStatsMimo(const std::vector<MimoSinrChunk>& sinrChunks,
                                           const std::vector<int>& map,
                                           uint32_t size,
                                           uint8_t mcs,
                                           uint8_t rank,
                                           const NrErrorModelHistory& history)
{
    NS_ASSERT_MSG(!sinrChunks.empty(), "At least one SINR value is required");

    // Compute time-domain average of the SINR matrix
    auto avgSinrMat = ComputeAvgSinrMimo(sinrChunks);
    NS_ASSERT(avgSinrMat.GetNumRows() == rank);

    // Vectorize SINR matrix and convert to SpectrumValue
    /// Linearize a 2D matrix into a vector, and convert that vector to a SpectrumValue
    /// Matches layer-to-codeword mapping in TR 38.211, Table 7.3.1.3-1
    /// avgSinrMat(NrSinrMatrix) is A 2D matrix of average SINR values, dimensions nMimoLayers x
    /// nRbs vectorizedSinr(SpectrumValue) contains the (nRB * nMimoLayers) SINR values
    auto vectorizedSinr = avgSinrMat.GetVectorizedSpecVal();

    // Create a new RB map that fits the vectorized SINR values
    auto vectorizedMap = CreateVectorizedRbMap(map, rank);

    return GetTbDecodificationStats(vectorizedSinr, vectorizedMap, size, mcs, history);
}

NrSinrMatrix
NrErrorModel::ComputeAvgSinrMimo(const std::vector<MimoSinrChunk>& sinrChunks)
{
    NS_ASSERT(!sinrChunks.empty());
    if (sinrChunks.size() == 1)
    {
        return sinrChunks[0].mimoSinr; // Single value, no need to compute an average
    }
    auto nRbs = sinrChunks[0].mimoSinr.GetNumCols();
    auto rank = sinrChunks[0].mimoSinr.GetNumRows();
    auto totDur = double{0.0};
    auto avgSinrMat = DoubleMatrixArray{rank, nRbs};
    for (const auto& chunk : sinrChunks)
    {
        const auto& sinrMat = chunk.mimoSinr;
        NS_ASSERT(sinrMat.GetNumRows() == avgSinrMat.GetNumRows());
        NS_ASSERT(sinrMat.GetNumCols() == avgSinrMat.GetNumCols());
        avgSinrMat += sinrMat * chunk.dur.GetDouble();
        totDur += chunk.dur.GetDouble();
    }
    return NrSinrMatrix{avgSinrMat * (1.0 / totDur)};
}

std::vector<int>
NrErrorModel::CreateVectorizedRbMap(std::vector<int> map, uint8_t rank)
{
    auto vectorizedMap = std::vector<int>{};
    for (int iRb : map)
    {
        for (size_t layer = 0; layer < rank; layer++)
        {
            vectorizedMap.emplace_back(iRb * rank + layer);
        }
    }
    return vectorizedMap;
}

} // namespace ns3
