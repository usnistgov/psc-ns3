/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Budiarto Herman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Budiarto Herman <budiarto.herman@magister.fi>
 *
 */

#include "nist-a3-rsrp-handover-algorithm.h"
#include <ns3/log.h>
#include <ns3/double.h>
#include <ns3/nist-lte-common.h>
#include <list>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistA3RsrpHandoverAlgorithm");

NS_OBJECT_ENSURE_REGISTERED (NistA3RsrpHandoverAlgorithm);


NistA3RsrpHandoverAlgorithm::NistA3RsrpHandoverAlgorithm ()
  : m_handoverManagementSapUser (0)
{
  NS_LOG_FUNCTION (this);
  m_handoverManagementSapProvider = new MemberNistLteHandoverManagementSapProvider<NistA3RsrpHandoverAlgorithm> (this);
}


NistA3RsrpHandoverAlgorithm::~NistA3RsrpHandoverAlgorithm ()
{
  NS_LOG_FUNCTION (this);
}


TypeId
NistA3RsrpHandoverAlgorithm::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::NistA3RsrpHandoverAlgorithm")
    .SetParent<NistLteHandoverAlgorithm> ()
    .AddConstructor<NistA3RsrpHandoverAlgorithm> ()
    .AddAttribute ("Hysteresis",
                   "Handover margin (hysteresis) in dB "
                   "(rounded to the nearest multiple of 0.5 dB)",
                   DoubleValue (3.0),
                   MakeDoubleAccessor (&NistA3RsrpHandoverAlgorithm::m_hysteresisDb),
                   MakeDoubleChecker<uint8_t> (0.0, 15.0)) // Hysteresis IE value range is [0..30] as per Section 6.3.5 of 3GPP TS 36.331
    .AddAttribute ("TimeToTrigger",
                   "Time during which neighbour cell's RSRP "
                   "must continuously higher than serving cell's RSRP "
                   "in order to trigger a handover",
                   TimeValue (MilliSeconds (256)), // 3GPP time-to-trigger median value as per Section 6.3.5 of 3GPP TS 36.331
                   MakeTimeAccessor (&NistA3RsrpHandoverAlgorithm::m_timeToTrigger),
                   MakeTimeChecker ())
  ;
  return tid;
}


void
NistA3RsrpHandoverAlgorithm::SetNistLteHandoverManagementSapUser (NistLteHandoverManagementSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_handoverManagementSapUser = s;
}


NistLteHandoverManagementSapProvider*
NistA3RsrpHandoverAlgorithm::GetNistLteHandoverManagementSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_handoverManagementSapProvider;
}


void
NistA3RsrpHandoverAlgorithm::DoInitialize ()
{
  NS_LOG_FUNCTION (this);

  uint8_t hysteresisIeValue = NistEutranMeasurementMapping::ActualHysteresis2IeValue (m_hysteresisDb);
  NS_LOG_LOGIC (this << " requesting Event A3 measurements"
                     << " (hysteresis=" << (uint16_t) hysteresisIeValue << ")"
                     << " (ttt=" << m_timeToTrigger.GetMilliSeconds () << ")");

  NistLteRrcSap::NistReportConfigEutra reportConfig;
  reportConfig.eventId = NistLteRrcSap::NistReportConfigEutra::EVENT_A3;
  reportConfig.a3Offset = 0;
  reportConfig.hysteresis = hysteresisIeValue;
  reportConfig.timeToTrigger = m_timeToTrigger.GetMilliSeconds ();
  reportConfig.reportOnLeave = false;
  reportConfig.triggerQuantity = NistLteRrcSap::NistReportConfigEutra::RSRP;
  reportConfig.reportInterval = NistLteRrcSap::NistReportConfigEutra::MS1024;
  m_measId = m_handoverManagementSapUser->AddUeMeasReportConfigForHandover (reportConfig);

  NistLteHandoverAlgorithm::DoInitialize ();
}


void
NistA3RsrpHandoverAlgorithm::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_handoverManagementSapProvider;
}


void
NistA3RsrpHandoverAlgorithm::DoReportUeMeas (uint16_t rnti,
                                         NistLteRrcSap::NistMeasResults measResults)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t) measResults.measId);

  if (measResults.measId != m_measId)
    {
      NS_LOG_WARN ("Ignoring measId " << (uint16_t) measResults.measId);
    }
  else
    {
      if (measResults.haveMeasResultNeighCells
          && !measResults.measResultListEutra.empty ())
        {
          uint16_t bestNeighbourCellId = 0;
          uint8_t bestNeighbourRsrp = 0;

          for (std::list <NistLteRrcSap::NistMeasResultEutra>::iterator it = measResults.measResultListEutra.begin ();
               it != measResults.measResultListEutra.end ();
               ++it)
            {
              if (it->haveRsrpResult)
                {
                  if ((bestNeighbourRsrp < it->rsrpResult)
                      && IsValidNeighbour (it->physCellId))
                    {
                      bestNeighbourCellId = it->physCellId;
                      bestNeighbourRsrp = it->rsrpResult;
                    }
                }
              else
                {
                  NS_LOG_WARN ("RSRP measurement is missing from cell ID " << it->physCellId);
                }
            }

          if (bestNeighbourCellId > 0)
            {
              NS_LOG_LOGIC ("Trigger Handover to cellId " << bestNeighbourCellId);
              NS_LOG_LOGIC ("target cell RSRP " << (uint16_t) bestNeighbourRsrp);
              NS_LOG_LOGIC ("serving cell RSRP " << (uint16_t) measResults.rsrpResult);

              // Inform eNodeB RRC about handover
              m_handoverManagementSapUser->TriggerHandover (rnti,
                                                            bestNeighbourCellId);
            }
        }
      else
        {
          NS_LOG_WARN (this << " Event A3 received without measurement results from neighbouring cells");
        }
    } // end of else of if (measResults.measId != m_measId)

} // end of DoReportUeMeas


bool
NistA3RsrpHandoverAlgorithm::IsValidNeighbour (uint16_t cellId)
{
  NS_LOG_FUNCTION (this << cellId);

  /**
   * \todo In the future, this function can be expanded to validate whether the
   *       neighbour cell is a valid target cell, e.g., taking into account the
   *       NRT in ANR and whether it is a CSG cell with closed access.
   */

  return true;
}


} // end of namespace ns3
