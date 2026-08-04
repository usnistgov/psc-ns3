// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-eesm-cc-t1.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEesmCcT1");
NS_OBJECT_ENSURE_REGISTERED(NrEesmCcT1);

NrEesmCcT1::NrEesmCcT1()
{
}

NrEesmCcT1::~NrEesmCcT1()
{
}

TypeId
NrEesmCcT1::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrEesmCcT1").SetParent<NrEesmCc>().AddConstructor<NrEesmCcT1>();
    return tid;
}

const std::vector<double>*
NrEesmCcT1::GetBetaTable() const
{
    return m_t1.m_betaTable;
}

const std::vector<double>*
NrEesmCcT1::GetMcsEcrTable() const
{
    return m_t1.m_mcsEcrTable;
}

const NrEesmErrorModel::SimulatedBlerFromSINR*
NrEesmCcT1::GetSimulatedBlerFromSINR() const
{
    return m_t1.m_simulatedBlerFromSINR;
}

const std::vector<uint8_t>*
NrEesmCcT1::GetMcsMTable() const
{
    return m_t1.m_mcsMTable;
}

const std::vector<double>*
NrEesmCcT1::GetSpectralEfficiencyForMcs() const
{
    return m_t1.m_spectralEfficiencyForMcs;
}

const std::vector<double>*
NrEesmCcT1::GetSpectralEfficiencyForCqi() const
{
    return m_t1.m_spectralEfficiencyForCqi;
}

} // namespace ns3
