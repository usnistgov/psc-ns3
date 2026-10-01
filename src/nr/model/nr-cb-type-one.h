// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_CB_TYPE_ONE_H
#define NR_CB_TYPE_ONE_H

#include <ns3/matrix-array.h>
#include <ns3/object.h>

namespace ns3
{

// Default initializer values, do not modify
constexpr size_t NR_CB_TYPE_ONE_INIT_N1 = 1;
constexpr size_t NR_CB_TYPE_ONE_INIT_N2 = 1;
constexpr bool NR_CB_TYPE_ONE_INIT_POL = false;
constexpr uint8_t NR_CB_TYPE_ONE_INIT_RANK = 1;
constexpr size_t NR_CB_TYPE_ONE_INIT_NI1 = 1;
constexpr size_t NR_CB_TYPE_ONE_INIT_NI2 = 1;
constexpr size_t NR_CB_TYPE_ONE_INIT_NPORTS = 1;

/// \brief Wrapper class for implementations of Type-I precoding matrices in 3GPP TS 38.214.
/// A separate object must be instantiated for each MIMO rank.
class NrCbTypeOne : public Object
{
  public:
    /// \brief Get TypeId
    /// \return the TypeId
    static TypeId GetTypeId();

    /// \brief Initialize the codebook parameters after construction, based on attribute values.
    virtual void Init() = 0;

    /// \brief Get number of i1 values.
    /// \return the number of wideband precoding indices i1
    size_t GetNumI1() const;

    /// \brief Get number of i2 values.
    /// \return the number of subband precoding indices i2
    size_t GetNumI2() const;

    /// \brief Get the 2D precoding matrix.
    /// \param i1 the index of the wideband precoding
    /// \param i2 the index of the subband precoding
    /// \return the precoding matrix of size m_nPorts x m_rank
    virtual ComplexMatrixArray GetBasePrecMat(size_t i1, size_t i2) const = 0;

  protected:
    // Constituting attributes
    size_t m_n1{NR_CB_TYPE_ONE_INIT_N1};       /// 3GPP n1-n2 config (num horiz gNB ports)
    size_t m_n2{NR_CB_TYPE_ONE_INIT_N2};       /// 3GPP n1-n2 config (num vert gNB ports)
    bool m_isDualPol{NR_CB_TYPE_ONE_INIT_POL}; /// Defines if gNB antennas are dual-polarized
    uint8_t m_rank{NR_CB_TYPE_ONE_INIT_RANK};  /// Number of MIMO layers

    // Derived attributes
    size_t m_numI1{NR_CB_TYPE_ONE_INIT_NI1};     /// Number of possible wideband indices (i1)
    size_t m_numI2{NR_CB_TYPE_ONE_INIT_NI2};     /// Number of possible subband indices (i2)
    size_t m_nPorts{NR_CB_TYPE_ONE_INIT_NPORTS}; /// Total number of gNB ports
};

} // namespace ns3

#endif // NR_CB_TYPE_ONE_H
