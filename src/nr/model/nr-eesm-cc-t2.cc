// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-eesm-cc-t2.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEesmCcT2");
NS_OBJECT_ENSURE_REGISTERED(NrEesmCcT2);

NrEesmCcT2::NrEesmCcT2()
{
}

NrEesmCcT2::~NrEesmCcT2()
{
}

TypeId
NrEesmCcT2::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrEesmCcT2").SetParent<NrEesmCc>().AddConstructor<NrEesmCcT2>();
    return tid;
}

const std::vector<double>*
NrEesmCcT2::GetBetaTable() const
{
    return m_t2.m_betaTable;
}

const std::vector<double>*
NrEesmCcT2::GetMcsEcrTable() const
{
    return m_t2.m_mcsEcrTable;
}

const NrEesmErrorModel::SimulatedBlerFromSINR*
NrEesmCcT2::GetSimulatedBlerFromSINR() const
{
    return m_t2.m_simulatedBlerFromSINR;
}

const std::vector<uint8_t>*
NrEesmCcT2::GetMcsMTable() const
{
    return m_t2.m_mcsMTable;
}

const std::vector<double>*
NrEesmCcT2::GetSpectralEfficiencyForMcs() const
{
    return m_t2.m_spectralEfficiencyForMcs;
}

const std::vector<double>*
NrEesmCcT2::GetSpectralEfficiencyForCqi() const
{
    return m_t2.m_spectralEfficiencyForCqi;
}

} // namespace ns3
