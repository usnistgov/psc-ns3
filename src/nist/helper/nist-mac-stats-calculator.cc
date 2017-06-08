/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Jaume Nin <jnin@cttc.es>
 * Modified by: NIST
 */

#include "nist-mac-stats-calculator.h"
#include "ns3/string.h"
#include <ns3/simulator.h>
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistMacStatsCalculator");

NS_OBJECT_ENSURE_REGISTERED (NistMacStatsCalculator);

NistMacStatsCalculator::NistMacStatsCalculator ()
  : m_dlFirstWrite (true),
    m_ulFirstWrite (true),
    m_slUeFirstWrite (true),
    m_slSchUeFirstWrite (true)
{
  NS_LOG_FUNCTION (this);

}

NistMacStatsCalculator::~NistMacStatsCalculator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
NistMacStatsCalculator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistMacStatsCalculator")
    .SetParent<NistLteStatsCalculator> ()
    .AddConstructor<NistMacStatsCalculator> ()
    .AddAttribute ("DlOutputFilename",
                   "Name of the file where the downlink results will be saved.",
                   StringValue ("DlMacStats.txt"),
                   MakeStringAccessor (&NistMacStatsCalculator::SetDlOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("UlOutputFilename",
                   "Name of the file where the uplink results will be saved.",
                   StringValue ("UlMacStats.txt"),
                   MakeStringAccessor (&NistMacStatsCalculator::SetUlOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("SlUeOutputFilename",
                   "Name of the file where the sidelink results will be saved.",
                   StringValue ("SlUeMacStats.txt"),
                   MakeStringAccessor (&NistMacStatsCalculator::SetSlUeOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("SlSchUeOutputFilename",
                   "Name of the file where the sidelink results will be saved.",
                   StringValue ("SlSchUeMacStats.txt"),
                   MakeStringAccessor (&NistMacStatsCalculator::SetSlSchUeOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}
void
NistMacStatsCalculator::SetSlUeOutputFilename (std::string outputFilename)
{
  NistLteStatsCalculator::SetSlUeOutputFilename (outputFilename);
}

std::string
NistMacStatsCalculator::GetSlUeOutputFilename (void)
{
  return NistLteStatsCalculator::GetSlUeOutputFilename ();
}

void
NistMacStatsCalculator::SetSlSchUeOutputFilename (std::string outputFilename)
{
  NistLteStatsCalculator::SetSlSchUeOutputFilename (outputFilename);
}

std::string
NistMacStatsCalculator::GetSlSchUeOutputFilename (void)
{
  return NistLteStatsCalculator::GetSlSchUeOutputFilename ();
}

void
NistMacStatsCalculator::SetUlOutputFilename (std::string outputFilename)
{
  NistLteStatsCalculator::SetUlOutputFilename (outputFilename);
}

std::string
NistMacStatsCalculator::GetUlOutputFilename (void)
{
  return NistLteStatsCalculator::GetUlOutputFilename ();
}

void
NistMacStatsCalculator::SetDlOutputFilename (std::string outputFilename)
{
  NistLteStatsCalculator::SetDlOutputFilename (outputFilename);
}

std::string
NistMacStatsCalculator::GetDlOutputFilename (void)
{
  return NistLteStatsCalculator::GetDlOutputFilename ();
}

void
NistMacStatsCalculator::DlScheduling (uint16_t cellId, uint64_t imsi, uint32_t frameNo, uint32_t subframeNo,
                                  uint16_t rnti, uint8_t mcsTb1, uint16_t sizeTb1, uint8_t mcsTb2, uint16_t sizeTb2)
{
  NS_LOG_FUNCTION (this << cellId << imsi << frameNo << subframeNo << rnti << (uint32_t) mcsTb1 << sizeTb1 << (uint32_t) mcsTb2 << sizeTb2);
  NS_LOG_INFO ("Write DL Mac Stats in " << GetDlOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_dlFirstWrite == true )
    {
      outFile.open (GetDlOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetDlOutputFilename ().c_str ());
          return;
        }
      m_dlFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tframe\tsframe\tRNTI\tmcsTb1\tsizeTb1\tmcsTb2\tsizeTb2";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetDlOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetDlOutputFilename ().c_str ());
          return;
        }
    }

  outFile << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t";
  outFile << (uint32_t) cellId << "\t";
  outFile << imsi << "\t";
  outFile << frameNo << "\t";
  outFile << subframeNo << "\t";
  outFile << rnti << "\t";
  outFile << (uint32_t) mcsTb1 << "\t";
  outFile << sizeTb1 << "\t";
  outFile << (uint32_t) mcsTb2 << "\t";
  outFile << sizeTb2 << std::endl;
  outFile.close ();
}

void
NistMacStatsCalculator::UlScheduling (uint16_t cellId, uint64_t imsi, uint32_t frameNo,
                                  uint32_t subframeNo, uint16_t rnti,uint8_t mcsTb, uint16_t size)
{
  NS_LOG_FUNCTION (this << cellId << imsi << frameNo << subframeNo << rnti << (uint32_t) mcsTb << size);
  NS_LOG_INFO ("Write UL Mac Stats in " << GetUlOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_ulFirstWrite == true )
    {
      outFile.open (GetUlOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetUlOutputFilename ().c_str ());
          return;
        }
      m_ulFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tframe\tsframe\tRNTI\tmcs\tsize";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetUlOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetUlOutputFilename ().c_str ());
          return;
        }
    }

  outFile << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t";
  outFile << (uint32_t) cellId << "\t";
  outFile << imsi << "\t";
  outFile << frameNo << "\t";
  outFile << subframeNo << "\t";
  outFile << rnti << "\t";
  outFile << (uint32_t) mcsTb << "\t";
  outFile << size << std::endl;
  outFile.close ();
}

void
NistMacStatsCalculator::SlUeScheduling (NistSlUeMacStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_frameNo << params.m_subframeNo << params.m_rnti << (uint32_t) params.m_mcs << params.m_pscchRi << params.m_pscchFrame1 << params.m_pscchSubframe1 << params.m_pscchFrame2 << params.m_pscchSubframe2 << params.m_psschTxStartRB << params.m_psschTxLengthRB << params.m_psschItrp);
  NS_LOG_INFO ("Write SL UE Mac Stats in " << GetSlUeOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_slUeFirstWrite == true )
    {
      outFile.open (GetSlUeOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlUeOutputFilename ().c_str ());
          return;
        }
      m_slUeFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tRNTI\tframe\tsframe\tpscchRi\tpscchF1\tpscchSF1\tpscchF2\tpscchSF2\tmcs\tTBS\tpsschRB\tpsschLen\tpssch_itrp";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetSlUeOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlUeOutputFilename ().c_str ());
          return;
        }
    }

  outFile << (uint32_t) params.m_timestamp << "\t";
  outFile << (uint32_t) params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  outFile << params.m_frameNo << "\t";
  outFile << params.m_subframeNo << "\t";
  outFile << params.m_pscchRi << "\t";
  outFile << params.m_pscchFrame1 << "\t";
  outFile << params.m_pscchSubframe1 << "\t";
  outFile << params.m_pscchFrame2 << "\t";
  outFile << params.m_pscchSubframe2 << "\t";
  outFile << (uint32_t) params.m_mcs << "\t";
  outFile << params.m_tbSize << "\t";
  outFile << params.m_psschTxStartRB << "\t";
  outFile << params.m_psschTxLengthRB << "\t";
  outFile << params.m_psschItrp << std::endl;
  outFile.close ();
}

void
NistMacStatsCalculator::SlSharedChUeScheduling (NistSlUeMacStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_rnti << params.m_frameNo << params.m_subframeNo << (uint32_t) params.m_mcs << params.m_tbSize << params.m_psschTxStartRB << params.m_psschTxLengthRB);
  NS_LOG_INFO ("Write SL Shared Channel UE Mac Stats in " << GetSlSchUeOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_slSchUeFirstWrite == true )
    {
      outFile.open (GetSlSchUeOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlSchUeOutputFilename ().c_str ());
          return;
        }
      m_slSchUeFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tRNTI\tSlPframe\tSlPsframe\tSchStartframe\tSchSsframe\tframe\tsframe\tmcs\tTBS\tpsschRB\tpsschLen";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetSlSchUeOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlSchUeOutputFilename ().c_str ());
          return;
        }
    }

  outFile << (uint32_t) params.m_timestamp << "\t";
  outFile << (uint32_t) params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  outFile << params.m_frameNo << "\t";
  outFile << params.m_subframeNo << "\t";
  outFile << params.m_psschFrameStart << "\t";
  outFile << params.m_psschSubframeStart << "\t";
  outFile << params.m_psschFrame << "\t";
  outFile << params.m_psschSubframe << "\t";
  outFile << (uint32_t) params.m_mcs << "\t";
  outFile << params.m_tbSize << "\t";
  outFile << params.m_psschTxStartRB << "\t";
  outFile << params.m_psschTxLengthRB << std::endl;
  outFile.close ();
}

void
NistMacStatsCalculator::DlSchedulingCallback (Ptr<NistMacStatsCalculator> macStats,
                      std::string path, uint32_t frameNo, uint32_t subframeNo,
                      uint16_t rnti, uint8_t mcsTb1, uint16_t sizeTb1,
                      uint8_t mcsTb2, uint16_t sizeTb2)
{
  NS_LOG_FUNCTION (macStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << rnti;
  if (macStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = macStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiFromEnbMac (path, rnti);
      macStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  uint16_t cellId = 0;
  if (macStats->ExistsCellIdPath (pathAndRnti.str ()) == true)
    {
      cellId = macStats->GetCellIdPath (pathAndRnti.str ());
    }
  else
    {
      cellId = FindCellIdFromEnbMac (path, rnti);
      macStats->SetCellIdPath (pathAndRnti.str (), cellId);
    }

  macStats->DlScheduling (cellId, imsi, frameNo, subframeNo, rnti, mcsTb1, sizeTb1, mcsTb2, sizeTb2);
}

void
NistMacStatsCalculator::UlSchedulingCallback (Ptr<NistMacStatsCalculator> macStats, std::string path,
                      uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                      uint8_t mcs, uint16_t size)
{
  NS_LOG_FUNCTION (macStats << path);

  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << rnti;
  if (macStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = macStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiFromEnbMac (path, rnti);
      macStats->SetImsiPath (pathAndRnti.str (), imsi);
    }
  uint16_t cellId = 0;
  if (macStats->ExistsCellIdPath (pathAndRnti.str ()) == true)
    {
      cellId = macStats->GetCellIdPath (pathAndRnti.str ());
    }
  else
    {
      cellId = FindCellIdFromEnbMac (path, rnti);
      macStats->SetCellIdPath (pathAndRnti.str (), cellId);
    }

  macStats->UlScheduling (cellId, imsi, frameNo, subframeNo, rnti, mcs, size);
}

void
NistMacStatsCalculator::SlUeSchedulingCallback (Ptr<NistMacStatsCalculator> macStats, std::string path, NistSlUeMacStatParameters params)
  // uint32_t frameNo, uint32_t subframeNo, uint16_t rnti, uint8_t mcs, uint16_t pscch_ri, uint16_t pssch_rb, uint16_t pssch_length, uint16_t pssch_itrp)
{
  NS_LOG_FUNCTION (macStats << path);

  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  if (macStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      NS_LOG_LOGIC("Existing IMSI path. Getting IMSI..."); 
      params.m_imsi = macStats->GetImsiPath (pathAndRnti.str ());
      NS_LOG_LOGIC("IMSI= " << params.m_imsi);
    }
  else
    {
      NS_LOG_LOGIC("NON-existing IMSI path. Finding IMSI from UE LteNetDevice...");
      std::string ueNetDevicePath = path.substr (0, path.find("/NistLteUeMac"));
      params.m_imsi = FindImsiFromNistLteNetDevice (ueNetDevicePath);
      NS_LOG_LOGIC("Found IMSI= " << params.m_imsi);
      macStats->SetImsiPath (pathAndRnti.str (), params.m_imsi);
    }
  params.m_cellId = 0;
 /* if (macStats->ExistsCellIdPath (pathAndRnti.str ()) == true)
    {
      cellId = macStats->GetCellIdPath (pathAndRnti.str ());
    }
  else
    {
      cellId = FindCellIdFromEnbMac (path, rnti);
      macStats->SetCellIdPath (pathAndRnti.str (), cellId);
    }
    */

  macStats->SlUeScheduling (params);
}

void
NistMacStatsCalculator::SlSharedChUeSchedulingCallback (Ptr<NistMacStatsCalculator> macStats, std::string path, NistSlUeMacStatParameters params)
{
  NS_LOG_FUNCTION (macStats << path);

  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  if (macStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      NS_LOG_LOGIC("Existing IMSI path. Getting IMSI..."); 
      params.m_imsi = macStats->GetImsiPath (pathAndRnti.str ());
      NS_LOG_LOGIC("IMSI= " << params.m_imsi);
    }
  else
    {
      NS_LOG_LOGIC("NON-existing IMSI path. Finding IMSI from UE LteNetDevice...");
      std::string ueNetDevicePath = path.substr (0, path.find("/NistLteUeMac"));
      params.m_imsi = FindImsiFromNistLteNetDevice (ueNetDevicePath);
      NS_LOG_LOGIC("Found IMSI= " << params.m_imsi);
      macStats->SetImsiPath (pathAndRnti.str (), params.m_imsi);
    }
  params.m_cellId = 0;
 /* if (macStats->ExistsCellIdPath (pathAndRnti.str ()) == true)
    {
      cellId = macStats->GetCellIdPath (pathAndRnti.str ());
    }
  else
    {
      cellId = FindCellIdFromEnbMac (path, rnti);
      macStats->SetCellIdPath (pathAndRnti.str (), cellId);
    }
    */

  macStats->SlSharedChUeScheduling (params);
}

} // namespace ns3
