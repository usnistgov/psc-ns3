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
 * modified by: Marco Miozzo <mmiozzo@cttc.es>
 *        Convert NistMacStatsCalculator in NistPhyRxStatsCalculator
 * Modified by: NIST
 */

#include "nist-phy-rx-stats-calculator.h"
#include "ns3/string.h"
#include <ns3/simulator.h>
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistPhyRxStatsCalculator");

NS_OBJECT_ENSURE_REGISTERED (NistPhyRxStatsCalculator);

NistPhyRxStatsCalculator::NistPhyRxStatsCalculator ()
  : m_dlRxFirstWrite (true),
    m_ulRxFirstWrite (true),
    m_slRxFirstWrite (true),
    m_slPscchRxFirstWrite (true)
{
  NS_LOG_FUNCTION (this);

}

NistPhyRxStatsCalculator::~NistPhyRxStatsCalculator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
NistPhyRxStatsCalculator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistPhyRxStatsCalculator")
    .SetParent<NistLteStatsCalculator> ()
    .AddConstructor<NistPhyRxStatsCalculator> ()
    .AddAttribute ("DlRxOutputFilename",
                   "Name of the file where the downlink results will be saved.",
                   StringValue ("DlRxPhyStats.txt"),
                   MakeStringAccessor (&NistPhyRxStatsCalculator::SetDlRxOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("UlRxOutputFilename",
                   "Name of the file where the uplink results will be saved.",
                   StringValue ("UlRxPhyStats.txt"),
                   MakeStringAccessor (&NistPhyRxStatsCalculator::SetUlRxOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("SlRxOutputFilename",
                   "Name of the file where the sidelink results will be saved.",
                   StringValue ("SlRxPhyStats.txt"),
                   MakeStringAccessor (&NistPhyRxStatsCalculator::SetSlRxOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("SlPscchRxOutputFilename",
                   "Name of the file where the sidelink PSCHCH results will be saved.",
                   StringValue ("SlPscchRxPhyStats.txt"),
                   MakeStringAccessor (&NistPhyRxStatsCalculator::SetSlPscchRxOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
NistPhyRxStatsCalculator::SetUlRxOutputFilename (std::string outputFilename)
{
  NistLteStatsCalculator::SetUlOutputFilename (outputFilename);
}

std::string
NistPhyRxStatsCalculator::GetUlRxOutputFilename (void)
{
  return NistLteStatsCalculator::GetUlOutputFilename ();
}

void
NistPhyRxStatsCalculator::SetDlRxOutputFilename (std::string outputFilename)
{
  NistLteStatsCalculator::SetDlOutputFilename (outputFilename);
}

std::string
NistPhyRxStatsCalculator::GetDlRxOutputFilename (void)
{
  return NistLteStatsCalculator::GetDlOutputFilename ();
}

void
NistPhyRxStatsCalculator::SetSlRxOutputFilename (std::string outputFilename)
{
  NistLteStatsCalculator::SetSlOutputFilename (outputFilename);
}

std::string
NistPhyRxStatsCalculator::GetSlRxOutputFilename (void)
{
  return NistLteStatsCalculator::GetSlOutputFilename ();
}

void
NistPhyRxStatsCalculator::SetSlPscchRxOutputFilename (std::string outputFilename)
{
  NistLteStatsCalculator::SetSlPscchOutputFilename (outputFilename);
}

std::string
NistPhyRxStatsCalculator::GetSlPscchRxOutputFilename (void)
{
  return NistLteStatsCalculator::GetSlPscchOutputFilename ();
}

void
NistPhyRxStatsCalculator::DlPhyReception (NistPhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_timestamp << params.m_rnti << params.m_layer << params.m_mcs << params.m_size << params.m_rv << params.m_ndi << params.m_correctness);
  NS_LOG_INFO ("Write DL Rx Phy Stats in " << GetDlRxOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_dlRxFirstWrite == true )
    {
      outFile.open (GetDlRxOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetDlRxOutputFilename ().c_str ());
          return;
        }
      m_dlRxFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tRNTI\ttxMode\tlayer\tmcs\tsize\trv\tndi\tcorrect\tsinrPerRb";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetDlRxOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetDlRxOutputFilename ().c_str ());
          return;
        }
    }

//   outFile << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t";
  outFile << params.m_timestamp << "\t";
  outFile << (uint32_t) params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  outFile << (uint32_t) params.m_txMode << "\t";
  outFile << (uint32_t) params.m_layer << "\t";
  outFile << (uint32_t) params.m_mcs << "\t";
  outFile << params.m_size << "\t";
  outFile << (uint32_t) params.m_rv << "\t";
  outFile << (uint32_t) params.m_ndi << "\t";
  outFile << (uint32_t) params.m_correctness << "\t"; 
  outFile << (double) params.m_sinrPerRb  << std::endl;
  outFile.close ();
}

void
NistPhyRxStatsCalculator::UlPhyReception (NistPhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_timestamp << params.m_rnti << params.m_layer << params.m_mcs << params.m_size << params.m_rv << params.m_ndi << params.m_correctness);
  NS_LOG_INFO ("Write UL Rx Phy Stats in " << GetUlRxOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_ulRxFirstWrite == true )
    {
      outFile.open (GetUlRxOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetUlRxOutputFilename ().c_str ());
          return;
        }
      m_ulRxFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tRNTI\tlayer\tmcs\tsize\trv\tndi\tcorrect\tsinrPerRb";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetUlRxOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetUlRxOutputFilename ().c_str ());
          return;
        }
    }

//   outFile << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t";
  outFile << params.m_timestamp << "\t";
  outFile << (uint32_t) params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  outFile << (uint32_t) params.m_layer << "\t";
  outFile << (uint32_t) params.m_mcs << "\t";
  outFile << params.m_size << "\t";
  outFile << (uint32_t) params.m_rv << "\t";
  outFile << (uint32_t) params.m_ndi << "\t";
  outFile << (uint32_t) params.m_correctness << "\t"; 
  outFile << (double) params.m_sinrPerRb  << std::endl;
  outFile.close ();
}

void
NistPhyRxStatsCalculator::SlPhyReception (NistPhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_timestamp << params.m_rnti << params.m_layer << params.m_mcs << params.m_size << params.m_rv << params.m_ndi << params.m_correctness);
  NS_LOG_INFO ("Write SL Rx Phy Stats in " << GetSlRxOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_slRxFirstWrite == true )
    {
      outFile.open (GetSlRxOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlRxOutputFilename ().c_str ());
          return;
        }
      m_slRxFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tRNTI\tlayer\tmcs\tsize\trv\tndi\tcorrect";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetSlRxOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlRxOutputFilename ().c_str ());
          return;
        }
    }

//   outFile << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t";
  outFile << params.m_timestamp << "\t";
  outFile << (uint32_t) params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  outFile << (uint32_t) params.m_layer << "\t";
  outFile << (uint32_t) params.m_mcs << "\t";
  outFile << params.m_size << "\t";
  outFile << (uint32_t) params.m_rv << "\t";
  outFile << (uint32_t) params.m_ndi << "\t";
  outFile << (uint32_t) params.m_correctness << std::endl;
  outFile.close ();
}

void
NistPhyRxStatsCalculator::SlPscchReception (NistPhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_timestamp << params.m_rnti << params.m_layer << params.m_mcs << params.m_size << params.m_rv << params.m_ndi << params.m_correctness);
  NS_LOG_INFO ("Write SL Rx PSCCH Stats in " << GetSlPscchRxOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_slPscchRxFirstWrite == true )
    {
      outFile.open (GetSlPscchRxOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlPscchRxOutputFilename ().c_str ());
          return;
        }
      m_slPscchRxFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tRNTI\tlayer\tcorrect";
      //outFile << "% time\tcellId\tIMSI\tRNTI\tlayer\tmcs\tsize\trbStart\trbLen\tcorrect";

      outFile << std::endl;
    }
  else
    {
      outFile.open (GetSlPscchRxOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlPscchRxOutputFilename ().c_str ());
          return;
        }
    }

//   outFile << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t";
  outFile << params.m_timestamp << "\t";
  outFile << (uint32_t) params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  outFile << (uint32_t) params.m_layer << "\t";
  //outFile << (uint32_t) params.m_mcs << "\t";
  //outFile << params.m_size << "\t";
  //outFile << (uint32_t) params.m_rv << "\t"; // This is used for the rbStart
  //outFile << (uint32_t) params.m_ndi << "\t";// This is used for the rbLen
  outFile << (uint32_t) params.m_correctness << std::endl;
  outFile.close ();
}

void
NistPhyRxStatsCalculator::DlPhyReceptionCallback (Ptr<NistPhyRxStatsCalculator> phyRxStats,
                      std::string path, NistPhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (phyRxStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  if (phyRxStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = phyRxStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiForUe (path, params.m_rnti);
      phyRxStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  params.m_imsi = imsi;
  phyRxStats->DlPhyReception (params);
}

void
NistPhyRxStatsCalculator::UlPhyReceptionCallback (Ptr<NistPhyRxStatsCalculator> phyRxStats,
                      std::string path, NistPhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (phyRxStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  if (phyRxStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = phyRxStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiForEnb (path, params.m_rnti);
      phyRxStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  params.m_imsi = imsi;
  phyRxStats->UlPhyReception (params);
}

void
NistPhyRxStatsCalculator::SlPhyReceptionCallback (Ptr<NistPhyRxStatsCalculator> phyRxStats,
                      std::string path, NistPhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (phyRxStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  if (phyRxStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = phyRxStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiForUe (path, params.m_rnti);
      phyRxStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  params.m_imsi = imsi;
  phyRxStats->SlPhyReception (params);
}

void
NistPhyRxStatsCalculator::SlPscchReceptionCallback (Ptr<NistPhyRxStatsCalculator> phyRxStats,
                      std::string path, NistPhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (phyRxStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  if (phyRxStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = phyRxStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiForUe (path, params.m_rnti);
      phyRxStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  params.m_imsi = imsi;
  phyRxStats->SlPscchReception (params);
}

} // namespace ns3
