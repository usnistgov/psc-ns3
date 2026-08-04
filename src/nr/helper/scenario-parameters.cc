// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "scenario-parameters.h"

namespace ns3
{

double ScenarioParameters::MAX_ANTENNA_OFFSET = 1;

ScenarioParameters::~ScenarioParameters()
{
}

void
ScenarioParameters::SetBsHeight(double h)
{
    m_bsHeight = h;
}

void
ScenarioParameters::SetUtHeight(double h)
{
    m_utHeight = h;
}

uint32_t
ScenarioParameters::GetNumSectorsPerSite() const
{
    return static_cast<uint32_t>(m_sectorization);
}

void
ScenarioParameters::SetSectorization(SiteSectorizationType numSectors)
{
    m_sectorization = numSectors;
}

void
ScenarioParameters::SetSectorization(uint32_t numSectors)
{
    SetSectorization(static_cast<SiteSectorizationType>(numSectors));
}

void
ScenarioParameters::SetScenarioParameters(const std::string& scenario)
{
    if (scenario == "UMa")
    {
        SetUMaParameters();
    }
    else if (scenario == "UMi")
    {
        SetUMiParameters();
    }
    else if (scenario == "RMa")
    {
        SetRMaParameters();
    }
    else
    {
        NS_ABORT_MSG("Unrecognized scenario: " << scenario);
    }
}

void
ScenarioParameters::SetScenarioParameters(const ScenarioParameters& scenario)
{
    m_isd = scenario.m_isd;
    m_bsHeight = scenario.m_bsHeight;
    m_utHeight = scenario.m_utHeight;
    m_sectorization = scenario.m_sectorization;
    m_minBsUtDistance = scenario.m_minBsUtDistance;
    m_antennaOffset = scenario.m_antennaOffset;
}

void
ScenarioParameters::SetUMaParameters()
{
    m_isd = 1732;
    m_bsHeight = 30.0;
    m_utHeight = 1.5;
    m_sectorization = SiteSectorizationType::TRIPLE;
    m_minBsUtDistance = 30.203; // minimum 2D distance is 10 meters considering UE height of 1.5 m
    m_antennaOffset = 1.0;
}

void
ScenarioParameters::SetUMiParameters()
{
    m_isd = 500;
    m_bsHeight = 10.0;
    m_utHeight = 1.5;
    m_sectorization = SiteSectorizationType::TRIPLE;
    m_minBsUtDistance = 10;
    m_antennaOffset = 1.0;
}

void
ScenarioParameters::SetRMaParameters()
{
    m_isd = 7000;
    m_bsHeight = 45.0;
    m_utHeight = 1.5;
    m_sectorization = SiteSectorizationType::TRIPLE;
    m_minBsUtDistance = 44.63; // minimum 2D distance is 10 meters considering UE height of 1.5 m
    m_antennaOffset = 1.0;
}

} // namespace ns3
