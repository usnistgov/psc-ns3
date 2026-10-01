// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_MIMO_MATRICES_H
#define NR_MIMO_MATRICES_H

#include <ns3/matrix-array.h>
#include <ns3/spectrum-value.h>

namespace ns3
{

class NrIntfNormChanMat;
class NrSinrMatrix;

/// \ingroup Matrices
/// NrCovMat stores the interference-plus-noise covariance matrices of a MIMO signal, with one
/// matrix page for each frequency bin. Operations for efficient computation, addition, and
/// subtraction of covariance matrices of interfering MIMO signals are implemented.
class NrCovMat : public ComplexMatrixArray
{
  public:
    NrCovMat() = default;
    NrCovMat(ComplexMatrixArray arr)
        : ComplexMatrixArray(arr) {};

    /// Add an interference signal: this += rhs * rhs.HermitianTranspose()
    /// \param rhs the full channel matrix (including precoding)
    virtual void AddInterferenceSignal(const ComplexMatrixArray& rhs);

    /// Subtract an interference signal: this -= rhs * rhs.HermitianTranspose()
    /// \param rhs the full channel matrix (including precoding)
    virtual void SubtractInterferenceSignal(const ComplexMatrixArray& rhs);

    /// \brief Calculate the interference-normalized channel matrix for SISO and MIMO.
    /// See NrIntfNormChanMat for details.
    /// \param chanMat the frequency-domain channel matrix without precoding
    /// \return the channel matrix after applying interference-normalization/whitening
    virtual NrIntfNormChanMat CalcIntfNormChannel(const ComplexMatrixArray& chanMat) const;

  private:
    /// \brief Calculate the interference-normalized channel matrix for MIMO.
    /// When the simulation is SISO only, this method will not be called.
    /// \param chanMat the frequency-domain channel matrix without precoding
    /// \return the channel matrix after applying interference-normalization/whitening
    virtual NrIntfNormChanMat CalcIntfNormChannelMimo(const ComplexMatrixArray& chanMat) const;
};

/// \ingroup Matrices
/// NrIntfNormChanMat stores the channel matrix after normalizing/whitening the interference.
/// See https://en.wikipedia.org/wiki/Whitening_transformation
/// Specifically, H_intfNorm = inv(L) * H, where
/// L is the lower triangular Cholesky decomposition of the interference covariance matrix R, and
/// H is the channel matrix.
/// Assume the receive signal is originally modeled as Y = H * P * S + W
/// where P is the precoding matrix, S is the transmit codeword and W is interference-and-noise term
/// which is assumed to be Gaussian with covariance matrix R. An equivalent signal representation is
/// Y_e = inv(L) * Y = H_intfNorm * P * S + W_e
/// where W_e is Gaussian with an identity covariance matrix.
/// For SISO, H_intfNorm is equivalent to 1 / sqrt(interfPlusNoisePower) * H, and the SISO SINR is
/// equal to |H_intfNorm|^2
class NrIntfNormChanMat : public ComplexMatrixArray
{
  public:
    NrIntfNormChanMat() = default;
    NrIntfNormChanMat(ComplexMatrixArray arr)
        : ComplexMatrixArray(arr) {};

    /// \brief Compute the MIMO SINR when a specific precoder is applied.
    /// \param precMats the precoding matrices (dim: nTxPorts * rank * nRbs)
    /// \returns the SINR values for each layer and RB (dim: rank x nRbs)
    virtual NrSinrMatrix ComputeSinrForPrecoding(const ComplexMatrixArray& precMats) const;

  private:
    /// \brief Compute the MSE (mean square error) for an MMSE receiver, for SISO and MIMO.
    /// \param precMats the precoding matrices (dim: nTxPorts * rank * nRbs)
    /// \returns the MSE value or matrix
    virtual ComplexMatrixArray ComputeMse(const ComplexMatrixArray& precMats) const;

    /// \brief Compute the MSE (mean square error) matrix for a MIMO MMSE receiver
    /// When the simulation is SISO only, this method will not be called.
    /// \param precMats the precoding matrices (dim: nTxPorts * rank * nRbs)
    /// \returns the MSE matrix as inv(I + precMats' * this' * this * precMats).
    virtual ComplexMatrixArray ComputeMseMimo(const ComplexMatrixArray& precMats) const;
};

/// \brief NrSinrMatrix stores the MIMO SINR matrix, with dimension rank x nRbs
class NrSinrMatrix : public DoubleMatrixArray
{
  public:
    NrSinrMatrix() = default;
    NrSinrMatrix(DoubleMatrixArray arr)
        : DoubleMatrixArray(arr) {};
    NrSinrMatrix(const std::valarray<double>& values)
        : DoubleMatrixArray(values) {};
    NrSinrMatrix(uint8_t rank, size_t nRbs)
        : DoubleMatrixArray(rank, nRbs) {};

    uint8_t GetRank() const;
    size_t GetNumRbs() const;
    /// \brief Linearize a 2D matrix into a vector, and convert that vector to a SpectrumValue
    /// Matches layer-to-codeword mapping in TR 38.211, Table 7.3.1.3-1
    /// \return A SpectrumValue with the (nRB * nMimoLayers) SINR values
    SpectrumValue GetVectorizedSpecVal() const;
};

} // namespace ns3

#endif // NR_MIMO_MATRICES_H
