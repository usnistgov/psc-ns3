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
 * Modified by: Danilo Abrignani <danilo.abrignani@unibo.it> (Carrier Aggregation - GSoC 2015)
 *              Biljana Bojovic <biljana.bojovic@cttc.es> (Carrier Aggregation)
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#include "mac-stats-calculator.h"
#include "ns3/string.h"
#include <ns3/simulator.h>
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MacStatsCalculator");

NS_OBJECT_ENSURE_REGISTERED (MacStatsCalculator);

MacStatsCalculator::MacStatsCalculator ()
  : m_dlFirstWrite (true),
    m_ulFirstWrite (true),
    m_slUeCchFirstWrite (true),
    m_slUeSchFirstWrite (true)
{
  NS_LOG_FUNCTION (this);

}

MacStatsCalculator::~MacStatsCalculator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
MacStatsCalculator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MacStatsCalculator")
    .SetParent<LteStatsCalculator> ()
    .SetGroupName("Lte")
    .AddConstructor<MacStatsCalculator> ()
    .AddAttribute ("DlOutputFilename",
                   "Name of the file where the downlink results will be saved.",
                   StringValue ("DlMacStats.txt"),
                   MakeStringAccessor (&MacStatsCalculator::SetDlOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("UlOutputFilename",
                   "Name of the file where the uplink results will be saved.",
                   StringValue ("UlMacStats.txt"),
                   MakeStringAccessor (&MacStatsCalculator::SetUlOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("SlUeCchOutputFilename",
                   "Name of the file where the Sidelink results will be saved.",
                   StringValue ("SlUeMacStats.txt"),
                   MakeStringAccessor (&MacStatsCalculator::SetSlUeCchOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("SlUeSchOutputFilename",
                   "Name of the file where the Sidelink results will be saved.",
                   StringValue ("SlSchUeMacStats.txt"),
                   MakeStringAccessor (&MacStatsCalculator::SetSlUeSchOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
MacStatsCalculator::SetUlOutputFilename (std::string outputFilename)
{
  LteStatsCalculator::SetUlOutputFilename (outputFilename);
}

std::string
MacStatsCalculator::GetUlOutputFilename (void)
{
  return LteStatsCalculator::GetUlOutputFilename ();
}

void
MacStatsCalculator::SetDlOutputFilename (std::string outputFilename)
{
  LteStatsCalculator::SetDlOutputFilename (outputFilename);
}

std::string
MacStatsCalculator::GetDlOutputFilename (void)
{
  return LteStatsCalculator::GetDlOutputFilename ();
}

void
MacStatsCalculator::SetSlUeCchOutputFilename (std::string outputFilename)
{
  LteStatsCalculator::SetSlPscchOutputFilename (outputFilename);
}

std::string
MacStatsCalculator::GetSlUeCchOutputFilename (void)
{
  return LteStatsCalculator::GetSlPscchOutputFilename ();
}

void
MacStatsCalculator::SetSlUeSchOutputFilename (std::string outputFilename)
{
  LteStatsCalculator::SetSlOutputFilename (outputFilename);
}

std::string
MacStatsCalculator::GetSlUeSchOutputFilename (void)
{
  return LteStatsCalculator::GetSlOutputFilename ();
}

void
MacStatsCalculator::DlScheduling (uint16_t cellId, uint64_t imsi, DlSchedulingCallbackInfo dlSchedulingCallbackInfo)
{
  NS_LOG_FUNCTION (this << cellId << imsi << dlSchedulingCallbackInfo.frameNo << dlSchedulingCallbackInfo.subframeNo <<
		  dlSchedulingCallbackInfo.rnti << (uint32_t) dlSchedulingCallbackInfo.mcsTb1 << dlSchedulingCallbackInfo.sizeTb1 << (uint32_t) dlSchedulingCallbackInfo.mcsTb2 << dlSchedulingCallbackInfo.sizeTb2);
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
      outFile << "% time\tcellId\tIMSI\tframe\tsframe\tRNTI\tmcsTb1\tsizeTb1\tmcsTb2\tsizeTb2\tccId";
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
  outFile << dlSchedulingCallbackInfo.frameNo << "\t";
  outFile << dlSchedulingCallbackInfo.subframeNo << "\t";
  outFile << dlSchedulingCallbackInfo.rnti << "\t";
  outFile << (uint32_t) dlSchedulingCallbackInfo.mcsTb1 << "\t";
  outFile << dlSchedulingCallbackInfo.sizeTb1 << "\t";
  outFile << (uint32_t) dlSchedulingCallbackInfo.mcsTb2 << "\t";
  outFile << dlSchedulingCallbackInfo.sizeTb2 << "\t";
  outFile << (uint32_t) dlSchedulingCallbackInfo.componentCarrierId << std::endl;
  outFile.close ();
}

void
MacStatsCalculator::UlScheduling (uint16_t cellId, uint64_t imsi, uint32_t frameNo,
                                  uint32_t subframeNo, uint16_t rnti,uint8_t mcsTb, uint16_t size, uint8_t componentCarrierId)
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
      outFile << "% time\tcellId\tIMSI\tframe\tsframe\tRNTI\tmcs\tsize\tccId";
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
  outFile << size << "\t";
  outFile << (uint32_t) componentCarrierId << std::endl;
  outFile.close ();
}

void
MacStatsCalculator::SlUeCchScheduling (SlUeMacStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_frameNo << params.m_subframeNo << params.m_rnti << (uint32_t) params.m_mcs << params.m_pscchRi << params.m_pscchFrame1 << params.m_pscchSubframe1 << params.m_pscchFrame2 << params.m_pscchSubframe2 << params.m_psschTxStartRB << params.m_psschTxLengthRB << params.m_psschItrp);
  NS_LOG_INFO ("Write SL UE Mac Stats in " << GetSlUeCchOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_slUeCchFirstWrite == true )
    {
      outFile.open (GetSlUeCchOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlUeCchOutputFilename ().c_str ());
          return;
        }
      m_slUeCchFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tRNTI\tframe\tsframe\tresPscch\tpscchFr1\tpscchSf1\tpscchFr2\tpscchSf2\tmcs\tTBS\tpsschRB\tpsschLen\tpsschItrp";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetSlUeCchOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlUeCchOutputFilename ().c_str ());
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
MacStatsCalculator::SlUeSchScheduling (SlUeMacStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_rnti << params.m_frameNo << params.m_subframeNo << (uint32_t) params.m_mcs << params.m_tbSize << params.m_psschTxStartRB << params.m_psschTxLengthRB);
  NS_LOG_INFO ("Write SL Shared Channel UE Mac Stats in " << GetSlUeSchOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_slUeSchFirstWrite == true )
    {
      outFile.open (GetSlUeSchOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlUeSchOutputFilename ().c_str ());
          return;
        }
      m_slUeSchFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tRNTI\tscPrdStartFr\tscPrdStartSf\tschStartFr\tschStartSf\tcurrFr\tcurrSf\tmcs\tTBS\tpsschRB\tpsschLen";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetSlUeSchOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlUeSchOutputFilename ().c_str ());
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
MacStatsCalculator::DlSchedulingCallback (Ptr<MacStatsCalculator> macStats, std::string path, DlSchedulingCallbackInfo dlSchedulingCallbackInfo)
{
  NS_LOG_FUNCTION (macStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  std::string pathEnb  = path.substr (0, path.find ("/ComponentCarrierMap"));
  pathAndRnti << pathEnb << "/LteEnbRrc/UeMap/" << dlSchedulingCallbackInfo.rnti;
  if (macStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = macStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiFromEnbRlcPath (pathAndRnti.str ());
      macStats->SetImsiPath (pathAndRnti.str (), imsi);
    }
  uint16_t cellId = 0;
  if (macStats->ExistsCellIdPath (pathAndRnti.str ()) == true)
    {
      cellId = macStats->GetCellIdPath (pathAndRnti.str ());
    }
  else
    {
      cellId = FindCellIdFromEnbRlcPath (pathAndRnti.str ());
      macStats->SetCellIdPath (pathAndRnti.str (), cellId);
    }

  macStats->DlScheduling (cellId, imsi, dlSchedulingCallbackInfo);
}

void
MacStatsCalculator::UlSchedulingCallback (Ptr<MacStatsCalculator> macStats, std::string path,
                      uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                      uint8_t mcs, uint16_t size, uint8_t componentCarrierId)
{
  NS_LOG_FUNCTION (macStats << path);

  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  std::string pathEnb  = path.substr (0, path.find ("/ComponentCarrierMap"));
  pathAndRnti << pathEnb << "/LteEnbRrc/UeMap/" << rnti;
  if (macStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = macStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiFromEnbRlcPath (pathAndRnti.str ());
      macStats->SetImsiPath (pathAndRnti.str (), imsi);
    }
  uint16_t cellId = 0;
  if (macStats->ExistsCellIdPath (pathAndRnti.str ()) == true)
    {
      cellId = macStats->GetCellIdPath (pathAndRnti.str ());
    }
  else
    {
      cellId = FindCellIdFromEnbRlcPath (pathAndRnti.str ());
      macStats->SetCellIdPath (pathAndRnti.str (), cellId);
    }

  macStats->UlScheduling (cellId, imsi, frameNo, subframeNo, rnti, mcs, size, componentCarrierId);
}

void
MacStatsCalculator::SlUeCchSchedulingCallback (Ptr<MacStatsCalculator> macStats, std::string path, SlUeMacStatParameters params)
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
      std::string ueNetDevicePath = path.substr (0, path.find("/ComponentCarrierMapUe"));
      params.m_imsi = FindImsiFromLteNetDevice (ueNetDevicePath);
      NS_LOG_LOGIC("Found IMSI= " << params.m_imsi);
      macStats->SetImsiPath (pathAndRnti.str (), params.m_imsi);
    }
  params.m_cellId = 0;

  macStats->SlUeCchScheduling (params);
}

void
MacStatsCalculator::SlUeSchSchedulingCallback (Ptr<MacStatsCalculator> macStats, std::string path, SlUeMacStatParameters params)
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
      std::string ueNetDevicePath = path.substr (0, path.find("/ComponentCarrierMapUe"));
      params.m_imsi = FindImsiFromLteNetDevice (ueNetDevicePath);
      NS_LOG_LOGIC("Found IMSI= " << params.m_imsi);
      macStats->SetImsiPath (pathAndRnti.str (), params.m_imsi);
    }
  params.m_cellId = 0;

  macStats->SlUeSchScheduling (params);
}

} // namespace ns3
