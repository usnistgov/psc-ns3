// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-eesm-ir-t2.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEesmIrT2");
NS_OBJECT_ENSURE_REGISTERED(NrEesmIrT2);

NrEesmIrT2::NrEesmIrT2()
{
}

NrEesmIrT2::~NrEesmIrT2()
{
}

TypeId
NrEesmIrT2::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrEesmIrT2").SetParent<NrEesmIr>().AddConstructor<NrEesmIrT2>();
    return tid;
}

const std::vector<double>*
NrEesmIrT2::GetBetaTable() const
{
    return m_t2.m_betaTable;
}

const std::vector<double>*
NrEesmIrT2::GetMcsEcrTable() const
{
    return m_t2.m_mcsEcrTable;
}

const NrEesmErrorModel::SimulatedBlerFromSINR*
NrEesmIrT2::GetSimulatedBlerFromSINR() const
{
    return m_t2.m_simulatedBlerFromSINR;
}

const std::vector<uint8_t>*
NrEesmIrT2::GetMcsMTable() const
{
    return m_t2.m_mcsMTable;
}

const std::vector<double>*
NrEesmIrT2::GetSpectralEfficiencyForMcs() const
{
    return m_t2.m_spectralEfficiencyForMcs;
}

const std::vector<double>*
NrEesmIrT2::GetSpectralEfficiencyForCqi() const
{
    return m_t2.m_spectralEfficiencyForCqi;
}

} // namespace ns3
