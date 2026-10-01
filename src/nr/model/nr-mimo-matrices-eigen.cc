// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mimo-matrices.h"

#include <Eigen/Dense>

namespace ns3
{

template <class T>
using EigenMatrix = Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>;
template <class T>
using ConstEigenMatrix = Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>;

NrIntfNormChanMat
NrCovMat::CalcIntfNormChannelMimo(const ComplexMatrixArray& chanMat) const
{
    auto res = NrIntfNormChanMat{
        ComplexMatrixArray{chanMat.GetNumRows(), chanMat.GetNumCols(), chanMat.GetNumPages()}};
    for (size_t iRb = 0; iRb < chanMat.GetNumPages(); iRb++)
    {
        ConstEigenMatrix<std::complex<double>> covMatEigen(GetPagePtr(iRb),
                                                           GetNumRows(),
                                                           GetNumCols());
        ConstEigenMatrix<std::complex<double>> chanMatEigen(chanMat.GetPagePtr(iRb),
                                                            chanMat.GetNumRows(),
                                                            chanMat.GetNumCols());
        EigenMatrix<std::complex<double>> resEigen(res.GetPagePtr(iRb),
                                                   res.GetNumRows(),
                                                   res.GetNumCols());
        auto covMat = covMatEigen.selfadjointView<Eigen::Upper>();
        resEigen = covMat.llt().matrixL().solve(chanMatEigen);
    }
    return res;
}

ComplexMatrixArray
NrIntfNormChanMat::ComputeMseMimo(const ComplexMatrixArray& precMats) const
{
    auto nDims = precMats.GetNumCols();
    auto identity = Eigen::MatrixXcd::Identity(nDims, nDims);
    auto res = ComplexMatrixArray{nDims, nDims, precMats.GetNumPages()};
    auto chanPrec = (*this) * precMats;
    auto chanCov = chanPrec.HermitianTranspose() * chanPrec;
    for (size_t iRb = 0; iRb < res.GetNumPages(); iRb++)
    {
        ConstEigenMatrix<std::complex<double>> chanCovEigen(chanCov.GetPagePtr(iRb),
                                                            chanCov.GetNumRows(),
                                                            chanCov.GetNumCols());
        EigenMatrix<std::complex<double>> resEigen(res.GetPagePtr(iRb),
                                                   res.GetNumRows(),
                                                   res.GetNumCols());
        Eigen::MatrixXcd temp = chanCovEigen + identity;
        resEigen = temp.selfadjointView<Eigen::Lower>().llt().solve(identity);
    }
    return res;
}

} // namespace ns3
