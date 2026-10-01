// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-cb-two-port.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrCbTwoPort");
NS_OBJECT_ENSURE_REGISTERED(NrCbTwoPort);

TypeId
NrCbTwoPort::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrCbTwoPort").SetParent<NrCbTypeOne>().AddConstructor<NrCbTwoPort>();
    return tid;
}

void
NrCbTwoPort::Init()
{
    m_nPorts = m_isDualPol ? (2 * m_n1 * m_n2) : (m_n1 * m_n2);
    NS_ASSERT_MSG(m_nPorts >= 1, "This codebook requires at least 1 port");
    NS_ASSERT_MSG(m_nPorts <= 2, "This codebook supports at most 2 ports");
    NS_ASSERT_MSG(m_rank <= m_nPorts, "Number of MIMO layers cannot exceed the number of ports");

    m_numI1 = 1;
    m_numI2 = (m_rank == 1) ? 4 : 2; // Number of rows in TS 38.214, Table 5.2.2.2.1-1
};

ComplexMatrixArray
NrCbTwoPort::GetBasePrecMat(size_t i1, size_t i2) const
{
    NS_ASSERT_MSG(i1 < m_numI1, "Wideband index i1 exceeds size");
    NS_ASSERT_MSG(i2 < m_numI2, "Subband index i2 exceeds size");

    auto precMat = ComplexMatrixArray(m_nPorts, m_rank);
    if (m_nPorts == 1)
    {
        precMat(0, 0) = 1.0;
    }
    else
    {
        // Convert index i2 (0, 1, 2, 3) to phase shift multiplier phi (1, j, -1, -j)
        auto phase = (M_PI * i2 / 2.0);
        auto phi = std::complex<double>{cos(phase), sin(phase)};

        // Implement TS 38.214, Table 5.2.2.2.1-1
        auto normalizer = 1 / sqrt(m_nPorts * m_rank);
        if (m_rank == 1)
        {
            precMat(0, 0) = normalizer * 1.0;
            precMat(1, 0) = normalizer * phi;
        }
        else
        {
            precMat(0, 0) = normalizer * 1.0;
            precMat(0, 1) = normalizer * 1.0;
            precMat(1, 0) = normalizer * phi;
            precMat(1, 1) = -normalizer * phi;
        }
    }
    return precMat;
}

} // namespace ns3
