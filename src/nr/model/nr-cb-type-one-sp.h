// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_CB_TYPE_ONE_SP_H
#define NR_CB_TYPE_ONE_SP_H

#include "nr-cb-type-one.h"

namespace ns3
{
/// @brief Implementation of Type-I Single-Panel Codebook 3GPP TS 38.214, Rel. 15, Sec. 5.2.2.2.1
/// Supports codebook mode 1 only, and is limited to rank 4.
/// Codebook mode 1 means the per-subband i2 beam index is used only for the phase shift of the
/// second polarization, while codebook mode 2 would use i2 also for beam refinement.
/// Plain references like 5.2.2.2.1-x refer to 3GPP TS 38.214, Rel. 15, Table 5.2.2.2.1-x
class NrCbTypeOneSp : public NrCbTypeOne
{
  public:
    /// @brief Get ns-3 TypeId
    /// @return the TypeId
    static TypeId GetTypeId();

    /// @brief Initialize the codebook parameters after construction, based on attribute values.
    void Init() override;

    /// @brief Get the 2D precoding matrix.
    /// @param i1 the composite index of the wideband precoding
    /// @param i2 the index of the subband precoding
    /// @return the precoding matrix of size m_nPorts x m_rank
    ComplexMatrixArray GetBasePrecMat(size_t i1, size_t i2) const override;

    /// @brief Get the 2D precoding matrix.
    /// @param i11 the horizontal beam index of the wideband precoding
    /// @param i12 the vertical beam index of the wideband precoding
    /// @param i13 the secondary beam index of the wideband precoding (or 0)
    /// @param i2 the index of the subband precoding
    /// @return the precoding matrix of size m_nPorts x m_rank
    virtual ComplexMatrixArray GetBasePrecMatFromIndex(size_t i11,
                                                       size_t i12,
                                                       size_t i13,
                                                       size_t i2) const;

    /// @brief Get num i11
    /// @return the number of i11 indices (horizontal beam directions)
    size_t GetNumI11() const;

    /// @brief Get num i12
    /// @return the number of i12 indices (vertical beam directions)
    size_t GetNumI12() const;

    /// @brief Get num i13
    /// @return the number of i13 indices (co-phasing shifts for a secondary beam)
    /// @note if i13 is not defined and there is no secondary beam (e.g., in rank 1), this returns 1
    size_t GetNumI13() const;

  protected:
    /// @brief Init the number of i11 indices (horizontal beams)
    void InitNumI11();

    /// @brief Init the number of i12 indices (vertical beams)
    void InitNumI12();

    /// @brief Init the number of i13 indices (co-phasing indices for a secondary beam)
    void InitNumI13();

    /// @brief Init the mapping tables from i13 to k1-k2 for any rank
    void InitK1K2();

    /// @brief Init the mapping tables from i13 to k1-k2 for rank 2
    void DoInitK1K2Rank2();

    /// @brief Init the mapping tables from i13 to k1-k2 for rank 3 or 4 when numPorts < 16
    void DoInitK1K2Rank34();

    /// @brief Define the columns in the full precoding matrix W.
    /// @note Specifically, define for each column/layer the index of the vector to be selected from
    /// uniqueBfvs, and the sign for the second polarization (sign in the lower half before phi_n).
    void InitWParams();

    /// @brief Init the number of composite i1 indices (number of unique i11, i12, i13 combinations)
    void InitNumI1();

    /// @brief Init the number of i2 indices (phase offset for the second polarization)
    void InitNumI2();

    /// @brief Map a composite i1 index to a i11 index (horizontal beam direction)
    /// @param i1 the composite index of i11, i12, i13
    /// @return the corresponding i11
    /// @note i1 is defined as a vector in TS 38.214. This vector is mapped to a unique integer to
    /// reduce the number of loops and parameters.
    /// The mapping is as follows: i1 is created from i11, i12, i13 as
    /// i1 = i11 + numI11 * (i12 + numI12 * i13)
    /// If i13 is not defined: i13 = 0.
    /// This is an arbitrary choice. The details (ordering of the indices) of this mapping do not
    /// matter when performing a full search over all indices.
    size_t MapToI11(size_t i1) const;

    /// @brief Map a composite i1 index to a i12 index (vertical beam direction)
    /// @param i1 the composite index of i11, i12, i13
    /// @return the corresponding i12
    size_t MapToI12(size_t i1) const;

    /// @brief Map a composite i1 index to a i13 index (co-phasing of a secondary beam)
    /// @param i1 the composite index of i11, i12, i13
    /// @return the corresponding i13 (this is 0 when there is no i13 / no secondary beam)
    size_t MapToI13(size_t i1) const;

    /// @brief Map an i13 index to a k1 index (horizontal offset of the secondary beam)
    /// @param i13 the i13 index
    /// @return the corresponding k1
    size_t MapToK1(size_t i13) const;

    /// @brief Map an i13 index to a k2 index (vertical offset of the secondary beam)
    /// @param i13 the i13 index
    /// @return the corresponding k2
    size_t MapToK2(size_t i13) const;

    /// @brief Create a list of different beamforming vectors used for the first polarization
    /// @param i11 the index i11 (horizontal beam direction)
    /// @param i12 the index i12 (vertical beam direction)
    /// @param i13 the index i13 (co-phasing shift for secondary beam)
    /// @return a list of unique vectors that are used in the upper half of the precoding matrix.
    /// @note Vectors each have size (m_nPorts/2). This method must not be used when m_nPorts == 1.
    /// For m_nPorts == 2, this returns only a single element of value 1.0. Otherwise:
    /// For rank 1, this returns the vector v_{l,m} (Table 5.2.2.2.1-5).
    /// For rank 2, this returns the two vectors v_{l,m} and v_{l',m'} (Table 5.2.2.2.1-6).
    /// For rank 3 or 4 with less than 16 ports, this returns the two vectors v_{l,m} and v_{l',m'}
    /// (these are the unique vectors in the upper parts of Tables 5.2.2.2.1-7 and 5.2.2.2.1-8).
    /// For rank 3 or 4 with at least 16 ports, this returns the two concatenated vectors
    /// [tilde{v}_{l,m}; theta_p * tilde{v}_{l,m}] and [tilde{v}_{l,m}; -theta_p * tilde{v}_{l,m}].
    /// Note that the upper parts of Tables 5.2.2.2.1-7 and 5.2.2.2.1-8 become equal to the lower
    /// parts when replacing v_{l,m} and v_{l',m'}, respectively, by those concatenated vectors.
    std::vector<std::vector<std::complex<double>>> CreateUniqueBfvs(size_t i11,
                                                                    size_t i12,
                                                                    size_t i13) const;

    /// @brief Create the vector v_{l,m} as given in 3GPP TS 38.214, Sec. 5.2.2.2.1
    /// @param l parameter l
    /// @param m parameter m
    /// @return the vector
    std::vector<std::complex<double>> CreateVecV(size_t l, size_t m) const;

    /// @brief Create the vector tilde{v}_{l,m} as given in 3GPP TS 38.214, Sec. 5.2.2.2.1
    /// @param l parameter l
    /// @param m parameter m
    /// @return the vector
    std::vector<std::complex<double>> CreateVecVtilde(size_t l, size_t m) const;

    /// @brief Concatenate the vectors vTilde and +/- theta_p * vTilde
    /// @param vTilde the vector tilde{v}_{l,m}
    /// @param signedThetaP the multiplier used for the lower part (+theta_p or -theta_p)
    /// @return the vector [vTilde; signedThetaP * vTilde]
    std::vector<std::complex<double>> ConcatVtildeThetaVtilde(
        const std::vector<std::complex<double>>& vTilde,
        std::complex<double> signedThetaP) const;

    /// @brief Create the vector u_m as given in 3GPP TS 38.214, Sec. 5.2.2.2.1
    /// @param m parameter m
    /// @return the vector
    std::vector<std::complex<double>> CreateVecU(size_t m) const;

    /// @brief Create the Kronecker product of two vectors
    /// @param vecA the first vector
    /// @param vecB the second vector
    /// @return a vector created by multiplying each element of vecA with the vector vecB
    static std::vector<std::complex<double>> Kroneckerproduct(
        std::vector<std::complex<double>> vecA,
        std::vector<std::complex<double>> vecB);

    /// @brief Check if the rank is 3 or 4 and the total number of ports is <16
    /// @return the check result
    bool IsRank34AndBelow16Ports() const;

    /// @brief Check if the rank is 3 or 4 and the total number of ports is >=16
    /// @return the check result
    bool IsRank34AndAtLeast16Ports() const;

    // Constituting attributes
    uint8_t m_codebookMode{1}; ///< Codebook mode (1 or 2 as defined in 5.2.2.2.1)

    // Derived attributes
    size_t m_o1{}; ///< Oversampling in n1-direction (typically the horizontal direction)
    size_t m_o2{}; ///< Oversampling in n2-direction (typically the vertical direction)

    size_t m_numI11{}; ///< Number of i11 values (horizontal beam indices)
    size_t m_numI12{}; ///< Number of i12 values (vertical beam indices)
    size_t m_numI13{}; ///< Number of i13 values (secondary beam offsets)

    std::vector<size_t> m_k1Factors{}; ///< Mapping from i13 to k1
    std::vector<size_t> m_k2Factors{}; ///< Mapping from i13 to k2

    std::vector<size_t> m_uniqueBfvInds; ///< For each column in W, the beamforming vector index
    std::vector<double> m_signPhiN;      ///< For each column in W, the sign before phi_n
};

} // namespace ns3

#endif // NR_CB_TYPE_ONE_SP_H
