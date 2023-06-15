/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
*   Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
*   Copyright (c) 2015, NYU WIRELESS, Tandon School of Engineering, New York University
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation;
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   Author: Marco Miozzo <marco.miozzo@cttc.es>
*           Nicola Baldo  <nbaldo@cttc.es>
*
*   Modified by: Marco Mezzavilla < mezzavilla@nyu.edu>
*                         Sourjya Dutta <sdutta@nyu.edu>
*                         Russell Ford <russell.ford@nyu.edu>
*                         Menglei Zhang <menglei@nyu.edu>
*/


#include "nr-bearer-stats-calculator.h"
#include "ns3/string.h"
#include "ns3/nstime.h"
#include <ns3/log.h>
#include <vector>
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrBearerStatsCalculator");

NS_OBJECT_ENSURE_REGISTERED ( NrBearerStatsCalculator);

NrBearerStatsCalculator::NrBearerStatsCalculator ()
  : m_firstWrite (true),
  m_pendingOutput (false),
  m_protocolType ("RLC")
{
  NS_LOG_FUNCTION (this);
}

NrBearerStatsCalculator::NrBearerStatsCalculator (std::string protocolType)
  : m_firstWrite (true),
  m_pendingOutput (false)
{
  NS_LOG_FUNCTION (this);
  m_protocolType = protocolType;
}

NrBearerStatsCalculator::~NrBearerStatsCalculator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
NrBearerStatsCalculator::GetTypeId (void)
{
  static TypeId tid =
    TypeId ("ns3::NrBearerStatsCalculator")
    .SetParent<NrBearerStatsBase> ()
    .AddConstructor<NrBearerStatsCalculator> ()
    .SetGroupName ("nr")
    .AddAttribute ("StartTime", "Start time of the on going epoch.",
                   TimeValue (Seconds (0.)),
                   MakeTimeAccessor (&NrBearerStatsCalculator::SetStartTime,
                                     &NrBearerStatsCalculator::GetStartTime),
                   MakeTimeChecker ())
    .AddAttribute ("EpochDuration", "Epoch duration.",
                   TimeValue (Seconds (0.25)),
                   MakeTimeAccessor (&NrBearerStatsCalculator::GetEpoch,
                                     &NrBearerStatsCalculator::SetEpoch),
                   MakeTimeChecker ())
    .AddAttribute ("DlRlcOutputFilename",
                   "Name of the file where the downlink results will be saved.",
                   StringValue ("NrDlRlcStatsE2E.txt"),
                   MakeStringAccessor (&NrBearerStatsCalculator::m_dlRlcOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("UlRlcOutputFilename",
                   "Name of the file where the uplink results will be saved.",
                   StringValue ("NrUlRlcStatsE2E.txt"),
                   MakeStringAccessor (&NrBearerStatsCalculator::m_ulRlcOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("DlPdcpOutputFilename",
                   "Name of the file where the downlink results will be saved.",
                   StringValue ("NrDlPdcpStatsE2E.txt"),
                   MakeStringAccessor (&NrBearerStatsCalculator::m_dlPdcpOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("UlPdcpOutputFilename",
                   "Name of the file where the uplink results will be saved.",
                   StringValue ("NrUlPdcpStatsE2E.txt"),
                   MakeStringAccessor (&NrBearerStatsCalculator::m_ulPdcpOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
NrBearerStatsCalculator::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  if (m_pendingOutput)
    {
      ShowResults ();
    }
}

void
NrBearerStatsCalculator::SetStartTime (Time t)
{
  m_startTime = t;
  //RescheduleEndEpoch ();
}

Time
NrBearerStatsCalculator::GetStartTime () const
{
  return m_startTime;
}

void
NrBearerStatsCalculator::SetEpoch (Time e)
{
  m_epochDuration = e;
  //RescheduleEndEpoch ();
}

Time
NrBearerStatsCalculator::GetEpoch () const
{
  return m_epochDuration;
}

void
NrBearerStatsCalculator::UlTxPdu (uint16_t cellId, uint64_t imsi, uint16_t rnti, uint8_t lcid, uint32_t packetSize)
{
  NS_LOG_FUNCTION (this);

  ImsiLcidPair_t p (imsi, lcid);
  if (Simulator::Now () >= m_startTime)
    {
      m_ulCellId[p] = cellId;
      m_flowId[p] = LteFlowId_t (rnti, lcid);
      m_ulTxPackets[p]++;
      m_ulTxData[p] += packetSize;
    }
  m_pendingOutput = true;
}

void
NrBearerStatsCalculator::DlTxPdu (uint16_t cellId, uint64_t imsi, uint16_t rnti, uint8_t lcid, uint32_t packetSize)
{
  NS_LOG_FUNCTION (this);

  ImsiLcidPair_t p (imsi, lcid);
  if (Simulator::Now () >= m_startTime)
    {
      m_dlCellId[p] = cellId;
      m_flowId[p] = LteFlowId_t (rnti, lcid);
      m_dlTxPackets[p]++;
      m_dlTxData[p] += packetSize;
    }
  m_pendingOutput = true;
}

void
NrBearerStatsCalculator::UlRxPdu (uint16_t cellId, uint64_t imsi, uint16_t rnti, uint8_t lcid, uint32_t packetSize,
                                      uint64_t delay)
{
  NS_LOG_FUNCTION (this);

  ImsiLcidPair_t p (imsi, lcid);
  if (Simulator::Now () >= m_startTime)
    {
      m_ulCellId[p] = cellId;
      m_ulRxPackets[p]++;
      m_ulRxData[p] += packetSize;

      Uint64StatsMap::iterator it = m_ulDelay.find (p);
      if (it == m_ulDelay.end ())
        {
          NS_LOG_DEBUG (this << " Creating UL stats calculators for IMSI " << p.m_imsi << " and LCID " << (uint32_t) p.m_lcId);
          m_ulDelay[p] = CreateObject<MinMaxAvgTotalCalculator<uint64_t> > ();
          m_ulPduSize[p] = CreateObject<MinMaxAvgTotalCalculator<uint32_t> > ();
        }
      m_ulDelay[p]->Update (delay);
      m_ulPduSize[p]->Update (packetSize);
    }
  m_pendingOutput = true;
}

void
NrBearerStatsCalculator::DlRxPdu (uint16_t cellId, uint64_t imsi, uint16_t rnti, uint8_t lcid, uint32_t packetSize, uint64_t delay)
{
  NS_LOG_FUNCTION (this);

  ImsiLcidPair_t p (imsi, lcid);
  if (Simulator::Now () >= m_startTime)
    {
      m_dlCellId[p] = cellId;
      m_dlRxPackets[p]++;
      m_dlRxData[p] += packetSize;

      Uint64StatsMap::iterator it = m_dlDelay.find (p);
      if (it == m_dlDelay.end ())
        {
          NS_LOG_DEBUG (this << " Creating DL stats calculators for IMSI " << p.m_imsi << " and LCID " << (uint32_t) p.m_lcId);
          m_dlDelay[p] = CreateObject<MinMaxAvgTotalCalculator<uint64_t> > ();
          m_dlPduSize[p] = CreateObject<MinMaxAvgTotalCalculator<uint32_t> > ();
        }
      m_dlDelay[p]->Update (delay);
      m_dlPduSize[p]->Update (packetSize);
    }
  m_pendingOutput = true;
}

void
NrBearerStatsCalculator::ShowResults (void)
{
  NS_LOG_FUNCTION (this << GetUlOutputFilename ().c_str () << GetDlOutputFilename ().c_str ());
  NS_LOG_INFO ("Write bearer stats to " << GetUlOutputFilename ().c_str () << " and in " << GetDlOutputFilename ().c_str ());

  std::ofstream ulOutFile;
  std::ofstream dlOutFile;

  if (m_firstWrite == true)
    {
      ulOutFile.open (GetUlOutputFilename ().c_str ());
      if (!ulOutFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetUlOutputFilename ().c_str ());
          return;
        }

      dlOutFile.open (GetDlOutputFilename ().c_str ());
      if (!dlOutFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetDlOutputFilename ().c_str ());
          return;
        }
      m_firstWrite = false;
      ulOutFile << "% start(s)\tend(s)\tCellId\tIMSI\tRNTI\tLCID\tnTxPDUs\tTxBytes\tnRxPDUs\tRxBytes\t";
      ulOutFile << "delay(s)\tstdDev(s)\tmin(s)\tmax(s)\t";
      ulOutFile << "PduSize\tstdDev\tmin\tmax";
      ulOutFile << std::endl;
      dlOutFile << "% start(s)\tend(s)\tCellId\tIMSI\tRNTI\tLCID\tnTxPDUs\tTxBytes\tnRxPDUs\tRxBytes\t";
      dlOutFile << "delay(s)\tstdDev(s)\tmin(s)\tmax(s)\t";
      dlOutFile << "PduSize\tstdDev\tmin\tmax";
      dlOutFile << std::endl;
    }
  else
    {
      ulOutFile.open (GetUlOutputFilename ().c_str (), std::ios_base::app);
      if (!ulOutFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetUlOutputFilename ().c_str ());
          return;
        }

      dlOutFile.open (GetDlOutputFilename ().c_str (), std::ios_base::app);
      if (!dlOutFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetDlOutputFilename ().c_str ());
          return;
        }
    }

  WriteUlResults (ulOutFile);
  WriteDlResults (dlOutFile);
  m_pendingOutput = false;

}

void
NrBearerStatsCalculator::WriteUlResults (std::ofstream& outFile)
{
  NS_LOG_FUNCTION (this);

  // Get the unique IMSI / LCID list

  std::vector < ImsiLcidPair_t > pairVector;
  for (Uint32Map::iterator it = m_ulTxPackets.begin (); it != m_ulTxPackets.end (); ++it)
    {
      if (find (pairVector.begin (), pairVector.end (), (*it).first) == pairVector.end ())
        {
          pairVector.push_back ((*it).first);
        }
    }

  Time endTime = m_startTime + m_epochDuration;
  for (std::vector<ImsiLcidPair_t>::iterator it = pairVector.begin (); it != pairVector.end (); ++it)
    {
      ImsiLcidPair_t p = *it;
      outFile << m_startTime.GetSeconds () << "\t";
      outFile << endTime.GetSeconds () << "\t";
      outFile << GetUlCellId (p.m_imsi, p.m_lcId) << "\t";
      outFile << p.m_imsi << "\t";
      outFile << m_flowId[p].m_rnti << "\t";
      outFile << (uint32_t) m_flowId[p].m_lcId << "\t";
      outFile << GetUlTxPackets (p.m_imsi, p.m_lcId) << "\t";
      outFile << GetUlTxData (p.m_imsi, p.m_lcId) << "\t";
      outFile << GetUlRxPackets (p.m_imsi, p.m_lcId) << "\t";
      outFile << GetUlRxData (p.m_imsi, p.m_lcId) << "\t";
      std::vector<double> stats = GetUlDelayStats (p.m_imsi, p.m_lcId);
      for (std::vector<double>::iterator it = stats.begin (); it != stats.end (); ++it)
        {
          outFile << (*it) * 1e-9 << "\t";
        }
      stats = GetUlPduSizeStats (p.m_imsi, p.m_lcId);
      for (std::vector<double>::iterator it = stats.begin (); it != stats.end (); ++it)
        {
          outFile << (*it) << "\t";
        }
      outFile << std::endl;
    }

  outFile.close ();
}

void
NrBearerStatsCalculator::WriteDlResults (std::ofstream& outFile)
{
  NS_LOG_FUNCTION (this);

  // Get the unique IMSI list
  std::vector < ImsiLcidPair_t > pairVector;
  for (Uint32Map::iterator it = m_dlTxPackets.begin (); it != m_dlTxPackets.end (); ++it)
    {
      if (find (pairVector.begin (), pairVector.end (), (*it).first) == pairVector.end ())
        {
          pairVector.push_back ((*it).first);
        }
    }

  Time endTime = m_startTime + m_epochDuration;
  for (std::vector<ImsiLcidPair_t>::iterator pair = pairVector.begin (); pair != pairVector.end (); ++pair)
    {
      ImsiLcidPair_t p = *pair;
      outFile << m_startTime.GetSeconds () << "\t";
      outFile << endTime.GetSeconds () << "\t";
      outFile << GetDlCellId (p.m_imsi, p.m_lcId) << "\t";
      outFile << p.m_imsi << "\t";
      outFile << m_flowId[p].m_rnti << "\t";
      outFile << (uint32_t) m_flowId[p].m_lcId << "\t";
      outFile << GetDlTxPackets (p.m_imsi, p.m_lcId) << "\t";
      outFile << GetDlTxData (p.m_imsi, p.m_lcId) << "\t";
      outFile << GetDlRxPackets (p.m_imsi, p.m_lcId) << "\t";
      outFile << GetDlRxData (p.m_imsi, p.m_lcId) << "\t";
      std::vector<double> stats = GetDlDelayStats (p.m_imsi, p.m_lcId);
      for (std::vector<double>::iterator it = stats.begin (); it != stats.end (); ++it)
        {
          outFile << (*it) * 1e-9 << "\t";
        }
      stats = GetDlPduSizeStats (p.m_imsi, p.m_lcId);
      for (std::vector<double>::iterator it = stats.begin (); it != stats.end (); ++it)
        {
          outFile << (*it) << "\t";
        }
      outFile << std::endl;
    }

  outFile.close ();
}

void
NrBearerStatsCalculator::ResetResults (void)
{
  NS_LOG_FUNCTION (this);

  m_ulTxPackets.erase (m_ulTxPackets.begin (), m_ulTxPackets.end ());
  m_ulRxPackets.erase (m_ulRxPackets.begin (), m_ulRxPackets.end ());
  m_ulRxData.erase (m_ulRxData.begin (), m_ulRxData.end ());
  m_ulTxData.erase (m_ulTxData.begin (), m_ulTxData.end ());
  m_ulDelay.erase (m_ulDelay.begin (), m_ulDelay.end ());
  m_ulPduSize.erase (m_ulPduSize.begin (), m_ulPduSize.end ());

  m_dlTxPackets.erase (m_dlTxPackets.begin (), m_dlTxPackets.end ());
  m_dlRxPackets.erase (m_dlRxPackets.begin (), m_dlRxPackets.end ());
  m_dlRxData.erase (m_dlRxData.begin (), m_dlRxData.end ());
  m_dlTxData.erase (m_dlTxData.begin (), m_dlTxData.end ());
  m_dlDelay.erase (m_dlDelay.begin (), m_dlDelay.end ());
  m_dlPduSize.erase (m_dlPduSize.begin (), m_dlPduSize.end ());
}

void
NrBearerStatsCalculator::RescheduleEndEpoch (void)
{
  NS_LOG_FUNCTION (this);
  m_endEpochEvent.Cancel ();
  NS_ASSERT (Simulator::Now ().GetMilliSeconds () == 0); // below event time assumes this
  m_endEpochEvent = Simulator::Schedule (m_startTime + m_epochDuration, &NrBearerStatsCalculator::EndEpoch, this);
}

void
NrBearerStatsCalculator::EndEpoch (void)
{
  NS_LOG_FUNCTION (this);
  ShowResults ();
  ResetResults ();
  m_startTime += m_epochDuration;
  m_endEpochEvent = Simulator::Schedule (m_epochDuration, &NrBearerStatsCalculator::EndEpoch, this);
}

uint32_t
NrBearerStatsCalculator::GetUlTxPackets (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  return m_ulTxPackets[p];
}

uint32_t
NrBearerStatsCalculator::GetUlRxPackets (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  return m_ulRxPackets[p];
}

uint64_t
NrBearerStatsCalculator::GetUlTxData (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  return m_ulTxData[p];
}

uint64_t
NrBearerStatsCalculator::GetUlRxData (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  return m_ulRxData[p];
}

double
NrBearerStatsCalculator::GetUlDelay (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  Uint64StatsMap::iterator it = m_ulDelay.find (p);
  if (it == m_ulDelay.end ())
    {
      NS_LOG_ERROR ("UL delay for " << imsi << " - " << (uint16_t) lcid << " not found");
      return 0;

    }
  return m_ulDelay[p]->getMean ();
}

std::vector<double>
NrBearerStatsCalculator::GetUlDelayStats (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  std::vector<double> stats;
  Uint64StatsMap::iterator it = m_ulDelay.find (p);
  if (it == m_ulDelay.end ())
    {
      stats.push_back (0.0);
      stats.push_back (0.0);
      stats.push_back (0.0);
      stats.push_back (0.0);
      return stats;

    }
  stats.push_back (m_ulDelay[p]->getMean ());
  stats.push_back (m_ulDelay[p]->getStddev ());
  stats.push_back (m_ulDelay[p]->getMin ());
  stats.push_back (m_ulDelay[p]->getMax ());
  return stats;
}

std::vector<double>
NrBearerStatsCalculator::GetUlPduSizeStats (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  std::vector<double> stats;
  Uint32StatsMap::iterator it = m_ulPduSize.find (p);
  if (it == m_ulPduSize.end ())
    {
      stats.push_back (0.0);
      stats.push_back (0.0);
      stats.push_back (0.0);
      stats.push_back (0.0);
      return stats;

    }
  stats.push_back (m_ulPduSize[p]->getMean ());
  stats.push_back (m_ulPduSize[p]->getStddev ());
  stats.push_back (m_ulPduSize[p]->getMin ());
  stats.push_back (m_ulPduSize[p]->getMax ());
  return stats;
}

uint32_t
NrBearerStatsCalculator::GetDlTxPackets (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  return m_dlTxPackets[p];
}

uint32_t
NrBearerStatsCalculator::GetDlRxPackets (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  return m_dlRxPackets[p];
}

uint64_t
NrBearerStatsCalculator::GetDlTxData (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  return m_dlTxData[p];
}

uint64_t
NrBearerStatsCalculator::GetDlRxData (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  return m_dlRxData[p];
}

uint32_t
NrBearerStatsCalculator::GetUlCellId (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  return m_ulCellId[p];
}

uint32_t
NrBearerStatsCalculator::GetDlCellId (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  return m_dlCellId[p];
}

double
NrBearerStatsCalculator::GetDlDelay (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  Uint64StatsMap::iterator it = m_dlDelay.find (p);
  if (it == m_dlDelay.end ())
    {
      NS_LOG_ERROR ("DL delay for " << imsi << " not found");
      return 0;
    }
  return m_dlDelay[p]->getMean ();
}

std::vector<double>
NrBearerStatsCalculator::GetDlDelayStats (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  std::vector<double> stats;
  Uint64StatsMap::iterator it = m_dlDelay.find (p);
  if (it == m_dlDelay.end ())
    {
      stats.push_back (0.0);
      stats.push_back (0.0);
      stats.push_back (0.0);
      stats.push_back (0.0);
      return stats;

    }
  stats.push_back (m_dlDelay[p]->getMean ());
  stats.push_back (m_dlDelay[p]->getStddev ());
  stats.push_back (m_dlDelay[p]->getMin ());
  stats.push_back (m_dlDelay[p]->getMax ());
  return stats;
}

std::vector<double>
NrBearerStatsCalculator::GetDlPduSizeStats (uint64_t imsi, uint8_t lcid)
{
  NS_LOG_FUNCTION (this << imsi << (uint16_t) lcid);
  ImsiLcidPair_t p (imsi, lcid);
  std::vector<double> stats;
  Uint32StatsMap::iterator it = m_dlPduSize.find (p);
  if (it == m_dlPduSize.end ())
    {
      stats.push_back (0.0);
      stats.push_back (0.0);
      stats.push_back (0.0);
      stats.push_back (0.0);
      return stats;

    }
  stats.push_back (m_dlPduSize[p]->getMean ());
  stats.push_back (m_dlPduSize[p]->getStddev ());
  stats.push_back (m_dlPduSize[p]->getMin ());
  stats.push_back (m_dlPduSize[p]->getMax ());
  return stats;
}


std::string
NrBearerStatsCalculator::GetUlOutputFilename (void)
{
  if (m_protocolType == "RLC")
    {
      return m_ulRlcOutputFilename;
    }
  else
    {
      return m_ulPdcpOutputFilename;
    }
}

std::string
NrBearerStatsCalculator::GetDlOutputFilename (void)
{
  if (m_protocolType == "RLC")
    {
      return m_dlRlcOutputFilename;
    }
  else
    {
      return m_dlPdcpOutputFilename;
    }
}

} // namespace ns3
