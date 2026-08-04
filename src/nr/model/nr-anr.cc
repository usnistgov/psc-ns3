// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2013 Budiarto Herman
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Original work authors (from lte-enb-rrc.cc):
//   Nicola Baldo <nbaldo@cttc.es>
//   Marco Miozzo <mmiozzo@cttc.es>
//   Manuel Requena <manuel.requena@cttc.es>
//
// Converted to ANR interface by:
//   Budiarto Herman <budiarto.herman@magister.fi>

#include "nr-anr.h"

#include <ns3/log.h>
#include <ns3/uinteger.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrAnr");

NS_OBJECT_ENSURE_REGISTERED(NrAnr);

NrAnr::NrAnr(uint16_t servingCellId)
    : m_anrSapUser(nullptr),
      m_threshold(0),
      m_measId(0),
      m_servingCellId(servingCellId)
{
    NS_LOG_FUNCTION(this << servingCellId);
    m_anrSapProvider = new MemberNrAnrSapProvider<NrAnr>(this);
}

NrAnr::~NrAnr()
{
    NS_LOG_FUNCTION(this << m_servingCellId);
}

TypeId
NrAnr::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrAnr")
            .SetParent<Object>()
            .SetGroupName("Nr")
            .AddAttribute("Threshold",
                          "Minimum RSRQ range value required for detecting a neighbour cell",
                          UintegerValue(0),
                          MakeUintegerAccessor(&NrAnr::m_threshold),
                          MakeUintegerChecker<uint8_t>(
                              0,
                              34)) // RSRQ range is [0..34] as per Section 9.1.7 of 3GPP TS 36.133
        ;
    return tid;
}

void
NrAnr::AddNeighbourRelation(uint16_t cellId)
{
    NS_LOG_FUNCTION(this << m_servingCellId << cellId);

    if (cellId == m_servingCellId)
    {
        NS_FATAL_ERROR("Serving cell ID " << cellId << " may not be added into NRT");
    }

    if (m_neighbourRelationTable.find(cellId) != m_neighbourRelationTable.end())
    {
        NS_FATAL_ERROR("There is already an entry in the NRT for cell ID " << cellId);
    }

    NeighbourRelation_t neighbourRelation;
    neighbourRelation.noRemove = true;
    neighbourRelation.noHo = true;
    neighbourRelation.noX2 = false;
    neighbourRelation.detectedAsNeighbour = false;
    m_neighbourRelationTable[cellId] = neighbourRelation;
}

void
NrAnr::RemoveNeighbourRelation(uint16_t cellId)
{
    NS_LOG_FUNCTION(this << m_servingCellId << cellId);

    auto it = m_neighbourRelationTable.find(cellId);
    if (it != m_neighbourRelationTable.end())
    {
        NS_FATAL_ERROR("Cell ID " << cellId << " cannot be found in NRT");
    }

    m_neighbourRelationTable.erase(it);
}

void
NrAnr::SetNrAnrSapUser(NrAnrSapUser* s)
{
    NS_LOG_FUNCTION(this << s);
    m_anrSapUser = s;
}

NrAnrSapProvider*
NrAnr::GetNrAnrSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_anrSapProvider;
}

void
NrAnr::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    NS_LOG_LOGIC(this << " requesting Event A4 measurements"
                      << " (threshold=" << (uint16_t)m_threshold << ")");
    NrRrcSap::ReportConfigEutra reportConfig;
    reportConfig.eventId = NrRrcSap::ReportConfigEutra::EVENT_A4;
    reportConfig.threshold1.choice = NrRrcSap::ThresholdEutra::THRESHOLD_RSRQ;
    reportConfig.threshold1.range = m_threshold;
    reportConfig.triggerQuantity = NrRrcSap::ReportConfigEutra::RSRQ;
    reportConfig.reportInterval = NrRrcSap::ReportConfigEutra::MS480;
    m_measId = m_anrSapUser->AddUeMeasReportConfigForAnr(reportConfig);
}

void
NrAnr::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_anrSapProvider;
    m_neighbourRelationTable.clear();
}

void
NrAnr::DoReportUeMeas(NrRrcSap::MeasResults measResults)
{
    uint8_t measId = measResults.measId;
    NS_LOG_FUNCTION(this << m_servingCellId << (uint16_t)measId);

    if (measId != m_measId)
    {
        NS_LOG_WARN(this << " Skipping unexpected measurement identity " << (uint16_t)measId);
    }
    else
    {
        if (measResults.haveMeasResultNeighCells && !(measResults.measResultListEutra.empty()))
        {
            for (auto it = measResults.measResultListEutra.begin();
                 it != measResults.measResultListEutra.end();
                 ++it)
            {
                // Keep new RSRQ value reported for the neighbour cell
                NS_ASSERT_MSG(it->haveRsrqResult == true,
                              "RSRQ measure missing for cellId " << it->physCellId);

                // Update Neighbour Relation Table
                auto itNrt = m_neighbourRelationTable.find(it->physCellId);
                if (itNrt != m_neighbourRelationTable.end())
                {
                    // Update neighbour relation entry
                    NS_LOG_LOGIC(this << " updating NRT of cell " << m_servingCellId
                                      << " with entry of cell " << it->physCellId);
                    if (!itNrt->second.noX2)
                    {
                        NS_LOG_LOGIC(this << " enabling handover"
                                          << " from cell " << m_servingCellId << " to cell "
                                          << it->physCellId);
                        itNrt->second.noHo = false;
                    }
                    itNrt->second.detectedAsNeighbour = true;
                }
                else
                {
                    // Discovered new neighbour
                    NS_LOG_LOGIC(this << " inserting NRT of cell " << m_servingCellId
                                      << " with newly discovered neighbouring cell "
                                      << it->physCellId);
                    NeighbourRelation_t neighbourRelation;
                    neighbourRelation.noRemove = false;
                    neighbourRelation.noHo = true;
                    neighbourRelation.noX2 = true;
                    neighbourRelation.detectedAsNeighbour = true;
                    m_neighbourRelationTable[it->physCellId] = neighbourRelation;
                }

            } // end of for (it = measResults.measResultListEutra.begin ())

        } // end of if (measResults.haveMeasResultNeighCells &&
          // !(measResults.measResultListEutra.empty ()))
        else
        {
            NS_LOG_WARN(
                this << " Event A4 received without measurement results from neighbouring cells");
            /// \todo Remove neighbours in the NRT.
        }

    } // end of else of if (measId != m_measId)

} // end of DoReportUeMeas

void
NrAnr::DoAddNeighbourRelation(uint16_t cellId)
{
    NS_LOG_FUNCTION(this << cellId);
    AddNeighbourRelation(cellId);
}

bool
NrAnr::DoGetNoRemove(uint16_t cellId) const
{
    NS_LOG_FUNCTION(this << m_servingCellId << cellId);
    return Find(cellId)->noRemove;
}

bool
NrAnr::DoGetNoHo(uint16_t cellId) const
{
    NS_LOG_FUNCTION(this << m_servingCellId << cellId);
    return Find(cellId)->noHo;
}

bool
NrAnr::DoGetNoX2(uint16_t cellId) const
{
    NS_LOG_FUNCTION(this << m_servingCellId << cellId);
    return Find(cellId)->noX2;
}

const NrAnr::NeighbourRelation_t*
NrAnr::Find(uint16_t cellId) const
{
    auto it = m_neighbourRelationTable.find(cellId);
    if (it == m_neighbourRelationTable.end())
    {
        NS_FATAL_ERROR("Cell ID " << cellId << " cannot be found in NRT");
    }
    return &(it->second);
}

} // end of namespace ns3
