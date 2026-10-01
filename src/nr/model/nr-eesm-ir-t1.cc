// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-eesm-ir-t1.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEesmIrT1");
NS_OBJECT_ENSURE_REGISTERED(NrEesmIrT1);

NrEesmIrT1::NrEesmIrT1()
{
}

NrEesmIrT1::~NrEesmIrT1()
{
}

TypeId
NrEesmIrT1::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrEesmIrT1").SetParent<NrEesmIr>().AddConstructor<NrEesmIrT1>();
    return tid;
}

const std::vector<double>*
NrEesmIrT1::GetBetaTable() const
{
    return m_t1.m_betaTable;
}

const std::vector<double>*
NrEesmIrT1::GetMcsEcrTable() const
{
    return m_t1.m_mcsEcrTable;
}

const NrEesmErrorModel::SimulatedBlerFromSINR*
NrEesmIrT1::GetSimulatedBlerFromSINR() const
{
    return m_t1.m_simulatedBlerFromSINR;
}

const std::vector<uint8_t>*
NrEesmIrT1::GetMcsMTable() const
{
    return m_t1.m_mcsMTable;
}

const std::vector<double>*
NrEesmIrT1::GetSpectralEfficiencyForMcs() const
{
    return m_t1.m_spectralEfficiencyForMcs;
}

const std::vector<double>*
NrEesmIrT1::GetSpectralEfficiencyForCqi() const
{
    return m_t1.m_spectralEfficiencyForCqi;
}

} // namespace ns3
