// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_CB_TWO_PORT_H
#define NR_CB_TWO_PORT_H

#include "nr-cb-type-one.h"

namespace ns3
{
/// \brief Implementation of the two-port codebook in 3GPP TS 38.214
/// This class implements a codebook for a gNB with at most 2 antenna ports. For a single port, it
/// returns single-element matrix with value 1.0. For two ports, it implements Table 5.2.2.2.1-1:
/// Codebooks for 1-layer and 2-layer CSI reporting using antenna ports 3000 to 3001.
/// There is a slight abuse of notation: For the "codebook index", the i2 index of other Type-I
/// codebooks is used, and the i1 index remains unused.
class NrCbTwoPort : public NrCbTypeOne
{
  public:
    /// \brief Get TypeId
    /// \return the TypeId
    static TypeId GetTypeId();

    /// \brief Initialize the codebook parameters after construction, based on attribute values.
    void Init() override;

    /// \brief Get the 2D precoding matrix.
    /// \param i1 the index of the wideband precoding (always 0 for this codebook)
    /// \param i2 the index of the subband precoding (the "codebook index" in Table 5.2.2.2.1-1)
    /// \return the precoding matrix of size m_nPorts x m_rank
    ComplexMatrixArray GetBasePrecMat(size_t i1, size_t i2) const override;
};

} // namespace ns3

#endif // NR_CB_TWO_PORT_H
