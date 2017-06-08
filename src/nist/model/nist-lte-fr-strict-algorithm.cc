/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Piotr Gawlowicz
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
 * Author: Piotr Gawlowicz <gawlowicz.p@gmail.com>
 *
 */

#include "nist-lte-fr-strict-algorithm.h"
#include <ns3/log.h>
#include "ns3/boolean.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteFrStrictAlgorithm");

NS_OBJECT_ENSURE_REGISTERED (NistLteFrStrictAlgorithm);

static const struct FrStrictDownlinkDefaultConfiguration
{
  uint8_t cellId;
  uint8_t dlBandwidth;
  uint8_t dlCommonSubBandwidth;
  uint8_t dlEgdeSubBandOffset;
  uint8_t dlEdgeSubBandwidth;
} g_frStrictDownlinkDefaultConfiguration[] = {
  { 1, 15, 2, 0, 4},
  { 2, 15, 2, 4, 4},
  { 3, 15, 2, 8, 4},
  { 1, 25, 6, 0, 6},
  { 2, 25, 6, 6, 6},
  { 3, 25, 6, 12, 6},
  { 1, 50, 21, 0, 9},
  { 2, 50, 21, 9, 9},
  { 3, 50, 21, 18, 11},
  { 1, 75, 36, 0, 12},
  { 2, 75, 36, 12, 12},
  { 3, 75, 36, 24, 15},
  { 1, 100, 28, 0, 24},
  { 2, 100, 28, 24, 24},
  { 3, 100, 28, 48, 24}
};

static const struct FrStrictUplinkDefaultConfiguration
{
  uint8_t cellId;
  uint8_t ulBandwidth;
  uint8_t ulCommonSubBandwidth;
  uint8_t ulEgdeSubBandOffset;
  uint8_t ulEdgeSubBandwidth;
} g_frStrictUplinkDefaultConfiguration[] = {
  { 1, 15, 3, 0, 4},
  { 2, 15, 3, 4, 4},
  { 3, 15, 3, 8, 4},
  { 1, 25, 6, 0, 6},
  { 2, 25, 6, 6, 6},
  { 3, 25, 6, 12, 6},
  { 1, 50, 21, 0, 9},
  { 2, 50, 21, 9, 9},
  { 3, 50, 21, 18, 11},
  { 1, 75, 36, 0, 12},
  { 2, 75, 36, 12, 12},
  { 3, 75, 36, 24, 15},
  { 1, 100, 28, 0, 24},
  { 2, 100, 28, 24, 24},
  { 3, 100, 28, 48, 24}
};

const uint16_t NUM_DOWNLINK_CONFS (sizeof (g_frStrictDownlinkDefaultConfiguration) / sizeof (FrStrictDownlinkDefaultConfiguration));
const uint16_t NUM_UPLINK_CONFS (sizeof (g_frStrictUplinkDefaultConfiguration) / sizeof (FrStrictUplinkDefaultConfiguration));


NistLteFrStrictAlgorithm::NistLteFrStrictAlgorithm ()
  : m_ffrSapUser (0),
    m_ffrRrcSapUser (0),
    m_dlEgdeSubBandOffset (0),
    m_dlEdgeSubBandwidth (0),
    m_ulEgdeSubBandOffset (0),
    m_ulEdgeSubBandwidth (0),
    m_measId (0)
{
  NS_LOG_FUNCTION (this);
  m_ffrSapProvider = new MemberNistLteFfrSapProvider<NistLteFrStrictAlgorithm> (this);
  m_ffrRrcSapProvider = new MemberNistLteFfrRrcSapProvider<NistLteFrStrictAlgorithm> (this);
}


NistLteFrStrictAlgorithm::~NistLteFrStrictAlgorithm ()
{
  NS_LOG_FUNCTION (this);
}


void
NistLteFrStrictAlgorithm::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_ffrSapProvider;
  delete m_ffrRrcSapProvider;
}


TypeId
NistLteFrStrictAlgorithm::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::NistLteFrStrictAlgorithm")
    .SetParent<NistLteFfrAlgorithm> ()
    .AddConstructor<NistLteFrStrictAlgorithm> ()
    .AddAttribute ("UlCommonSubBandwidth",
                   "Uplink Common SubBandwidth Configuration in number of Resource Block Groups",
                   UintegerValue (25),
                   MakeUintegerAccessor (&NistLteFrStrictAlgorithm::m_ulCommonSubBandwidth),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("UlEdgeSubBandOffset",
                   "Uplink Edge SubBand Offset in number of Resource Block Groups",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NistLteFrStrictAlgorithm::m_ulEgdeSubBandOffset),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("UlEdgeSubBandwidth",
                   "Uplink Edge SubBandwidth Configuration in number of Resource Block Groups",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NistLteFrStrictAlgorithm::m_ulEdgeSubBandwidth),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("DlCommonSubBandwidth",
                   "Downlink Common SubBandwidth Configuration in number of Resource Block Groups",
                   UintegerValue (25),
                   MakeUintegerAccessor (&NistLteFrStrictAlgorithm::m_dlCommonSubBandwidth),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("DlEdgeSubBandOffset",
                   "Downlink Edge SubBand Offset in number of Resource Block Groups",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NistLteFrStrictAlgorithm::m_dlEgdeSubBandOffset),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("DlEdgeSubBandwidth",
                   "Downlink Edge SubBandwidth Configuration in number of Resource Block Groups",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NistLteFrStrictAlgorithm::m_dlEdgeSubBandwidth),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("RsrqThreshold",
                   "If the RSRQ of is worse than this threshold, UE should be served in Edge sub-band",
                   UintegerValue (20),
                   MakeUintegerAccessor (&NistLteFrStrictAlgorithm::m_egdeSubBandThreshold),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("CenterPowerOffset",
                   "NistPdschConfigDedicated::Pa value for Edge Sub-band, default value dB0",
                   UintegerValue (5),
                   MakeUintegerAccessor (&NistLteFrStrictAlgorithm::m_centerAreaPowerOffset),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("EdgePowerOffset",
                   "NistPdschConfigDedicated::Pa value for Edge Sub-band, default value dB0",
                   UintegerValue (5),
                   MakeUintegerAccessor (&NistLteFrStrictAlgorithm::m_edgeAreaPowerOffset),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("CenterAreaTpc",
                   "TPC value which will be set in DL-DCI for UEs in center area"
                   "Absolute mode is used, default value 1 is mapped to -1 according to"
                   "TS36.213 Table 5.1.1.1-2",
                   UintegerValue (1),
                   MakeUintegerAccessor (&NistLteFrStrictAlgorithm::m_centerAreaTpc),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("EdgeAreaTpc",
                   "TPC value which will be set in DL-DCI for UEs in edge area"
                   "Absolute mode is used, default value 1 is mapped to -1 according to"
                   "TS36.213 Table 5.1.1.1-2",
                   UintegerValue (1),
                   MakeUintegerAccessor (&NistLteFrStrictAlgorithm::m_edgeAreaTpc),
                   MakeUintegerChecker<uint8_t> ())
  ;
  return tid;
}


void
NistLteFrStrictAlgorithm::SetNistLteFfrSapUser (NistLteFfrSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_ffrSapUser = s;
}


NistLteFfrSapProvider*
NistLteFrStrictAlgorithm::GetNistLteFfrSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_ffrSapProvider;
}

void
NistLteFrStrictAlgorithm::SetNistLteFfrRrcSapUser (NistLteFfrRrcSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_ffrRrcSapUser = s;
}


NistLteFfrRrcSapProvider*
NistLteFrStrictAlgorithm::GetNistLteFfrRrcSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_ffrRrcSapProvider;
}


void
NistLteFrStrictAlgorithm::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  NistLteFfrAlgorithm::DoInitialize ();

  NS_ASSERT_MSG (m_dlBandwidth > 14,"DlBandwidth must be at least 15 to use FFR algorithms");
  NS_ASSERT_MSG (m_ulBandwidth > 14,"UlBandwidth must be at least 15 to use FFR algorithms");

  if (m_frCellTypeId != 0)
    {
      SetDownlinkConfiguration (m_frCellTypeId, m_dlBandwidth);
      SetUplinkConfiguration (m_frCellTypeId, m_ulBandwidth);
    }

  NS_LOG_LOGIC (this << " requesting Event A1 measurements"
                     << " (threshold = 0" << ")");
  NistLteRrcSap::NistReportConfigEutra reportConfig;
  reportConfig.eventId = NistLteRrcSap::NistReportConfigEutra::EVENT_A1;
  reportConfig.threshold1.choice = NistLteRrcSap::NistThresholdEutra::THRESHOLD_RSRQ;
  reportConfig.threshold1.range = 0;
  reportConfig.triggerQuantity = NistLteRrcSap::NistReportConfigEutra::RSRQ;
  reportConfig.reportInterval = NistLteRrcSap::NistReportConfigEutra::MS120;
  m_measId = m_ffrRrcSapUser->AddUeMeasReportConfigForFfr (reportConfig);
}

void
NistLteFrStrictAlgorithm::Reconfigure ()
{
  NS_LOG_FUNCTION (this);
  if (m_frCellTypeId != 0)
    {
      SetDownlinkConfiguration (m_frCellTypeId, m_dlBandwidth);
      SetUplinkConfiguration (m_frCellTypeId, m_ulBandwidth);
    }
  InitializeDownlinkRbgMaps ();
  InitializeUplinkRbgMaps ();
  m_needReconfiguration = false;
}

void
NistLteFrStrictAlgorithm::SetDownlinkConfiguration (uint16_t cellId, uint8_t bandwidth)
{
  NS_LOG_FUNCTION (this);
  for (uint16_t i = 0; i < NUM_DOWNLINK_CONFS; ++i)
    {
      if ((g_frStrictDownlinkDefaultConfiguration[i].cellId == cellId)
          && g_frStrictDownlinkDefaultConfiguration[i].dlBandwidth == m_dlBandwidth)
        {
          m_dlCommonSubBandwidth = g_frStrictDownlinkDefaultConfiguration[i].dlCommonSubBandwidth;
          m_dlEgdeSubBandOffset = g_frStrictDownlinkDefaultConfiguration[i].dlEgdeSubBandOffset;
          m_dlEdgeSubBandwidth = g_frStrictDownlinkDefaultConfiguration[i].dlEdgeSubBandwidth;
        }
    }
}

void
NistLteFrStrictAlgorithm::SetUplinkConfiguration (uint16_t cellId, uint8_t bandwidth)
{
  NS_LOG_FUNCTION (this);
  for (uint16_t i = 0; i < NUM_UPLINK_CONFS; ++i)
    {
      if ((g_frStrictUplinkDefaultConfiguration[i].cellId == cellId)
          && g_frStrictUplinkDefaultConfiguration[i].ulBandwidth == m_ulBandwidth)
        {
          m_ulCommonSubBandwidth = g_frStrictUplinkDefaultConfiguration[i].ulCommonSubBandwidth;
          m_ulEgdeSubBandOffset = g_frStrictUplinkDefaultConfiguration[i].ulEgdeSubBandOffset;
          m_ulEdgeSubBandwidth = g_frStrictUplinkDefaultConfiguration[i].ulEdgeSubBandwidth;
        }
    }
}

void
NistLteFrStrictAlgorithm::InitializeDownlinkRbgMaps ()
{
  m_dlRbgMap.clear ();
  m_dlEdgeRbgMap.clear ();

  int rbgSize = GetRbgSize (m_dlBandwidth);
  m_dlRbgMap.resize (m_dlBandwidth / rbgSize, true);
  m_dlEdgeRbgMap.resize (m_dlBandwidth / rbgSize, false);

  NS_ASSERT_MSG (m_dlCommonSubBandwidth <= m_dlBandwidth,"DlCommonSubBandwidth higher than DlBandwidth");
  NS_ASSERT_MSG (m_dlEgdeSubBandOffset <= m_dlBandwidth,"DlEgdeSubBandOffset higher than DlBandwidth");
  NS_ASSERT_MSG (m_dlEdgeSubBandwidth <= m_dlBandwidth,"DlEdgeSubBandwidth higher than DlBandwidth");
  NS_ASSERT_MSG ((m_dlCommonSubBandwidth + m_dlEgdeSubBandOffset + m_dlEdgeSubBandwidth) <= m_dlBandwidth,
                 "(DlCommonSubBandwidth+DlEgdeSubBandOffset+DlEdgeSubBandwidth) higher than DlBandwidth");

  for (uint8_t i = 0; i < m_dlCommonSubBandwidth / rbgSize; i++)
    {
      m_dlRbgMap[i] = false;

    }

  for (uint8_t i = m_dlCommonSubBandwidth / rbgSize + m_dlEgdeSubBandOffset / rbgSize;
       i < (m_dlCommonSubBandwidth / rbgSize + m_dlEgdeSubBandOffset / rbgSize
            + m_dlEdgeSubBandwidth / rbgSize); i++)
    {
      m_dlRbgMap[i] = false;
      m_dlEdgeRbgMap[i] = true;
    }
}

void
NistLteFrStrictAlgorithm::InitializeUplinkRbgMaps ()
{
  m_ulRbgMap.clear ();
  m_ulEdgeRbgMap.clear ();

  if (!m_enabledInUplink)
    {
      m_ulRbgMap.resize (m_ulBandwidth, false);
      return;
    }

  m_ulRbgMap.resize (m_ulBandwidth, true);
  m_ulEdgeRbgMap.resize (m_ulBandwidth, false);

  NS_ASSERT_MSG (m_ulCommonSubBandwidth <= m_ulBandwidth,"UlCommonSubBandwidth higher than UlBandwidth");
  NS_ASSERT_MSG (m_ulEgdeSubBandOffset <= m_ulBandwidth,"UlEgdeSubBandOffset higher than UlBandwidth");
  NS_ASSERT_MSG (m_ulEdgeSubBandwidth <= m_ulBandwidth,"UlEdgeSubBandwidth higher than UlBandwidth");
  NS_ASSERT_MSG ((m_ulCommonSubBandwidth + m_ulEgdeSubBandOffset + m_ulEdgeSubBandwidth) <= m_dlBandwidth,
                 "(UlCommonSubBandwidth+UlEgdeSubBandOffset+UlEdgeSubBandwidth) higher than UlBandwidth");

  for (uint8_t i = 0; i < m_ulCommonSubBandwidth; i++)
    {
      m_ulRbgMap[i] = false;

    }

  for (uint8_t i = m_ulCommonSubBandwidth + m_ulEgdeSubBandOffset;
       i < (m_ulCommonSubBandwidth + m_ulEgdeSubBandOffset
            + m_ulEdgeSubBandwidth); i++)
    {
      m_ulRbgMap[i] = false;
      m_ulEdgeRbgMap[i] = true;
    }
}


std::vector <bool>
NistLteFrStrictAlgorithm::DoGetAvailableDlRbg ()
{
  NS_LOG_FUNCTION (this);

  if (m_needReconfiguration)
    {
      Reconfigure ();
    }

  if (m_dlRbgMap.empty ())
    {
      InitializeDownlinkRbgMaps ();
    }

  return m_dlRbgMap;
}

bool
NistLteFrStrictAlgorithm::DoIsDlRbgAvailableForUe (int rbgId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this);

  bool edgeRbg = m_dlEdgeRbgMap[rbgId];

  std::map< uint16_t, uint8_t >::iterator it = m_ues.find (rnti);
  if (it == m_ues.end ())
    {
      m_ues.insert (std::pair< uint16_t, uint8_t > (rnti, AreaUnset));
      return !edgeRbg;
    }

  bool edgeUe = false;
  if (it->second == CellEdge )
    {
      edgeUe = true;
    }

  return (edgeRbg && edgeUe) || (!edgeRbg && !edgeUe);
}

std::vector <bool>
NistLteFrStrictAlgorithm::DoGetAvailableUlRbg ()
{
  NS_LOG_FUNCTION (this);

  if (m_ulRbgMap.empty ())
    {
      InitializeUplinkRbgMaps ();
    }

  return m_ulRbgMap;
}

bool
NistLteFrStrictAlgorithm::DoIsUlRbgAvailableForUe (int rbgId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this);

  if (!m_enabledInUplink)
    {
      return true;
    }

  bool edgeRbg = m_ulEdgeRbgMap[rbgId];

  std::map< uint16_t, uint8_t >::iterator it = m_ues.find (rnti);
  if (it == m_ues.end ())
    {
      m_ues.insert (std::pair< uint16_t, uint8_t > (rnti, AreaUnset));
      return !edgeRbg;
    }

  bool edgeUe = false;
  if (it->second == CellEdge )
    {
      edgeUe = true;
    }

  return (edgeRbg && edgeUe) || (!edgeRbg && !edgeUe);
}

void
NistLteFrStrictAlgorithm::DoReportDlCqiInfo (const struct NistFfMacSchedSapProvider::NistSchedDlCqiInfoReqParameters& params)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_WARN ("Method should not be called, because it is empty");
}

void
NistLteFrStrictAlgorithm::DoReportUlCqiInfo (const struct NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters& params)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_WARN ("Method should not be called, because it is empty");
}

void
NistLteFrStrictAlgorithm::DoReportUlCqiInfo (std::map <uint16_t, std::vector <double> > ulCqiMap)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_WARN ("Method should not be called, because it is empty");
}

uint8_t
NistLteFrStrictAlgorithm::DoGetTpc (uint16_t rnti)
{
  NS_LOG_FUNCTION (this);

  if (!m_enabledInUplink)
    {
      return 1;     // 1 is mapped to 0 for Accumulated mode, and to -1 in Absolute mode TS36.213 Table 5.1.1.1-2
    }

  //TS36.213 Table 5.1.1.1-2
  //   TPC   |   Accumulated Mode  |  Absolute Mode
  //------------------------------------------------
  //    0    |         -1          |      -4
  //    1    |          0          |      -1
  //    2    |          1          |       1
  //    3    |          3          |       4
  //------------------------------------------------
  // here Absolute mode is used

  std::map< uint16_t, uint8_t >::iterator it = m_ues.find (rnti);
  if (it == m_ues.end ())
    {
      return 1;
    }

  if (it->second == CellEdge )
    {
      return m_edgeAreaTpc;
    }
  else if (it->second == CellCenter )
    {
      return m_centerAreaTpc;
    }

  return 1;
}

uint8_t
NistLteFrStrictAlgorithm::DoGetMinContinuousUlBandwidth ()
{
  NS_LOG_FUNCTION (this);

  if (!m_enabledInUplink)
    {
      return m_ulBandwidth;
    }

  uint8_t minContinuousUlBandwidth = m_ulCommonSubBandwidth < m_ulEdgeSubBandwidth ? m_ulCommonSubBandwidth : m_ulEdgeSubBandwidth;
  NS_LOG_INFO ("minContinuousUlBandwidth: " << (int)minContinuousUlBandwidth);

  return minContinuousUlBandwidth;
}

void
NistLteFrStrictAlgorithm::DoReportUeMeas (uint16_t rnti,
                                      NistLteRrcSap::NistMeasResults measResults)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t) measResults.measId);
  NS_LOG_INFO ("RNTI :" << rnti << " MeasId: " << (uint16_t) measResults.measId
                        << " RSRP: " << (uint16_t)measResults.rsrpResult
                        << " RSRQ: " << (uint16_t)measResults.rsrqResult);

  if (measResults.measId != m_measId)
    {
      NS_LOG_WARN ("Ignoring measId " << (uint16_t) measResults.measId);
    }
  else
    {
      std::map< uint16_t, uint8_t >::iterator it = m_ues.find (rnti);
      if (it == m_ues.end ())
        {
          m_ues.insert (std::pair< uint16_t, uint8_t > (rnti, AreaUnset));
        }
      it = m_ues.find (rnti);

      if (measResults.rsrqResult < m_egdeSubBandThreshold)
        {
          if (it->second != CellEdge )
            {
              NS_LOG_INFO ("UE RNTI: " << rnti << " will be served in Edge sub-band");
              it->second = CellEdge;

              NistLteRrcSap::NistPdschConfigDedicated pdschConfigDedicated;
              pdschConfigDedicated.pa = m_edgeAreaPowerOffset;
              m_ffrRrcSapUser->SetNistPdschConfigDedicated (rnti, pdschConfigDedicated);
            }
        }
      else
        {
          if (it->second != CellCenter)
            {
              NS_LOG_INFO ("UE RNTI: " << rnti << " will be served in Center sub-band");
              it->second = CellCenter;

              NistLteRrcSap::NistPdschConfigDedicated pdschConfigDedicated;
              pdschConfigDedicated.pa = m_centerAreaPowerOffset;
              m_ffrRrcSapUser->SetNistPdschConfigDedicated (rnti, pdschConfigDedicated);
            }
        }
    }
}

void
NistLteFrStrictAlgorithm::DoRecvLoadInformation (NistEpcX2Sap::NistLoadInformationParams params)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_WARN ("Method should not be called, since it is empty");

}

} // end of namespace ns3
