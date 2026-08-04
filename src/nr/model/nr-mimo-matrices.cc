// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mimo-matrices.h"

namespace ns3
{

void
NrCovMat::AddInterferenceSignal(const ComplexMatrixArray& rhs)
{
    *this += rhs * rhs.HermitianTranspose();
}

void
NrCovMat::SubtractInterferenceSignal(const ComplexMatrixArray& rhs)
{
    *this -= rhs * rhs.HermitianTranspose();
}

NrIntfNormChanMat
NrCovMat::CalcIntfNormChannel(const ComplexMatrixArray& chanMat) const
{
    /// Compute inv(L) * chanMat, where L is the Cholesky decomposition of this covariance matrix.
    /// For SISO, the computation simplifies to 1/sqrt(covMat) * chanMat
    /// This normalizes the received signal such that the interference has an identity covariance

    if ((chanMat.GetNumRows() == 1) && (chanMat.GetNumCols() == 1)) // SISO
    {
        auto res = NrIntfNormChanMat{ComplexMatrixArray{1, 1, chanMat.GetNumPages()}};
        for (size_t iRb = 0; iRb < chanMat.GetNumPages(); iRb++)
        {
            res(0, 0, iRb) = 1.0 / std::sqrt(std::real(Elem(0, 0, iRb))) * chanMat.Elem(0, 0, iRb);
        }
        return res;
    }
    else // MIMO
    {
        return CalcIntfNormChannelMimo(chanMat);
    }
}

NrSinrMatrix
NrIntfNormChanMat::ComputeSinrForPrecoding(const ComplexMatrixArray& precMats) const
{
    auto mseMat = ComputeMse(precMats);

    // Compute the SINR values from the diagonal elements of the mseMat.
    // Result is a 2D Matrix, size rank x nRbs.
    auto res = DoubleMatrixArray{mseMat.GetNumRows(), mseMat.GetNumPages()};
    for (size_t iRb = 0; iRb < mseMat.GetNumPages(); iRb++)
    {
        for (size_t layer = 0; layer < mseMat.GetNumRows(); layer++)
        {
            auto denominator = std::real(mseMat.Elem(layer, layer, iRb));
            res(layer, iRb) = 1.0 / denominator - 1.0;
        }
    }
    return NrSinrMatrix{res};
}

ComplexMatrixArray
NrIntfNormChanMat::ComputeMse(const ComplexMatrixArray& precMats) const
{
    // Compute the MSE of an MMSE receiver: inv(I + precMats' * this' * this * precMats)

    if ((GetNumRows() == 1) && (GetNumCols() == 1)) // SISO
    {
        auto res = ComplexMatrixArray{1, 1, GetNumPages()};
        auto chanPrec = (*this) * precMats;
        for (size_t iRb = 0; iRb < GetNumPages(); iRb++)
        {
            res(0, 0, iRb) = 1.0 / (1.0 + std::norm(chanPrec.Elem(0, 0, iRb)));
        }
        return res;
    }
    else // MIMO
    {
        return ComputeMseMimo(precMats);
    }
}

uint8_t
NrSinrMatrix::GetRank() const
{
    return static_cast<uint8_t>(GetNumRows());
}

size_t
NrSinrMatrix::GetNumRbs() const
{
    return GetNumCols();
}

SpectrumValue
NrSinrMatrix::GetVectorizedSpecVal() const
{
    // Convert the 2D SINR matrix into a one-dimensional SpectrumValue
    auto bands = std::vector<BandInfo>(GetNumRows() * GetNumCols());
    auto specModel = Create<SpectrumModel>(bands);
    auto vectorizedSinr = SpectrumValue{specModel};
    auto idx = size_t{0};
    for (auto it = vectorizedSinr.ValuesBegin(); it != vectorizedSinr.ValuesEnd(); it++)
    {
        auto& itVal = *it;
        itVal = m_values[idx];
        idx++;
    }
    return vectorizedSinr;
}
} // namespace ns3
