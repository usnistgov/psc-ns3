// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mimo-signal.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMimoSignal");

NrMimoSignal::NrMimoSignal(const std::vector<MimoSignalChunk>& mimoChunks)
{
    NS_ASSERT_MSG(!mimoChunks.empty(), "mimoChunks cannot be empty");
    m_chanMat = ConsolidateChanSpctMimo(mimoChunks);
    m_covMat = ComputeAvgCovMatMimo(mimoChunks);
}

ComplexMatrixArray
NrMimoSignal::ConsolidateChanSpctMimo(const std::vector<MimoSignalChunk>& mimoChunks)
{
    NS_ASSERT(!mimoChunks.empty());

    // Create a consolidated chanSpct that combines all non-zero pages of the different chanSpcts.
    auto chanSpct = mimoChunks[0].chanSpct;
    for (const auto& chunk : mimoChunks)
    {
        for (auto iRb = size_t{0}; iRb < chunk.chanSpct.GetNumPages(); iRb++)
        {
            if (chunk.chanSpct(0, 0, iRb) == 0.0)
            {
                continue;
            }
            // Replace the sub-matrix/page of this RB with the non-zero page from other chunk
            for (size_t i = 0; i < chanSpct.GetNumRows(); i++)
            {
                for (size_t j = 0; j < chanSpct.GetNumCols(); j++)
                {
                    chanSpct(i, j, iRb) = chunk.chanSpct(i, j, iRb);
                }
            }
        }
    }
    return chanSpct;
}

NrCovMat
NrMimoSignal::ComputeAvgCovMatMimo(const std::vector<MimoSignalChunk>& mimoChunks)
{
    NS_ASSERT(!mimoChunks.empty());
    auto nRx = mimoChunks[0].interfNoiseCov.GetNumRows();
    auto avgMat =
        NrCovMat{ComplexMatrixArray{nRx, nRx, mimoChunks[0].interfNoiseCov.GetNumPages()}};
    auto totDur = double{0.0};
    for (const auto& chunk : mimoChunks)
    {
        avgMat += chunk.interfNoiseCov * std::complex<double>{chunk.dur.GetDouble()};
        totDur += chunk.dur.GetDouble();
    }
    return avgMat * std::complex<double>{1.0 / totDur};
}

} // namespace ns3
