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
 *        Convert NistMacStatsCalculator in NistPhyTxStatsCalculator
 * Modified by: NIST
 */

#include "nist-phy-tx-stats-calculator.h"
#include "ns3/string.h"
#include <ns3/simulator.h>
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistPhyTxStatsCalculator");

NS_OBJECT_ENSURE_REGISTERED (NistPhyTxStatsCalculator);

NistPhyTxStatsCalculator::NistPhyTxStatsCalculator ()
  : m_dlTxFirstWrite (true),
    m_ulTxFirstWrite (true),
    m_slTxFirstWrite (true)
{
  NS_LOG_FUNCTION (this);

}

NistPhyTxStatsCalculator::~NistPhyTxStatsCalculator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
NistPhyTxStatsCalculator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistPhyTxStatsCalculator")
    .SetParent<NistLteStatsCalculator> ()
    .AddConstructor<NistPhyTxStatsCalculator> ()
    .AddAttribute ("DlTxOutputFilename",
                   "Name of the file where the downlink results will be saved.",
                   StringValue ("DlTxPhyStats.txt"),
                   MakeStringAccessor (&NistPhyTxStatsCalculator::SetDlTxOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("UlTxOutputFilename",
                   "Name of the file where the uplink results will be saved.",
                   StringValue ("UlTxPhyStats.txt"),
                   MakeStringAccessor (&NistPhyTxStatsCalculator::SetUlTxOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("SlTxOutputFilename",
                   "Name of the file where the sidelink results will be saved.",
                   StringValue ("SlTxPhyStats.txt"),
                   MakeStringAccessor (&NistPhyTxStatsCalculator::SetSlTxOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
NistPhyTxStatsCalculator::SetUlTxOutputFilename (std::string outputFilename)
{
  NistLteStatsCalculator::SetUlOutputFilename (outputFilename);
}

std::string
NistPhyTxStatsCalculator::GetUlTxOutputFilename (void)
{
  return NistLteStatsCalculator::GetUlOutputFilename ();
}

void
NistPhyTxStatsCalculator::SetDlTxOutputFilename (std::string outputFilename)
{
  NistLteStatsCalculator::SetDlOutputFilename (outputFilename);
}

std::string
NistPhyTxStatsCalculator::GetDlTxOutputFilename (void)
{
  return NistLteStatsCalculator::GetDlOutputFilename ();
}

void
NistPhyTxStatsCalculator::SetSlTxOutputFilename (std::string outputFilename)
{
  NistLteStatsCalculator::SetSlOutputFilename (outputFilename);
}

std::string
NistPhyTxStatsCalculator::GetSlTxOutputFilename (void)
{
  return NistLteStatsCalculator::GetSlOutputFilename ();
}

void
NistPhyTxStatsCalculator::DlPhyTransmission (NistPhyTransmissionStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_timestamp << params.m_rnti << params.m_layer << params.m_mcs << params.m_size << params.m_rv << params.m_ndi);
  NS_LOG_INFO ("Write DL Tx Phy Stats in " << GetDlTxOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_dlTxFirstWrite == true )
    {
      outFile.open (GetDlOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetDlTxOutputFilename ().c_str ());
          return;
        }
      m_dlTxFirstWrite = false;
      //outFile << "% time\tcellId\tIMSI\tRNTI\tlayer\tmcs\tsize\trv\tndi"; // txMode is not available at dl tx side
      outFile << "% time\tcellId\tIMSI\tRNTI\tlayer\tmcs\tsize\trv\tndi";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetDlTxOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetDlTxOutputFilename ().c_str ());
          return;
        }
    }

//   outFile << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t";
  outFile << params.m_timestamp << "\t";
  outFile << (uint32_t) params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  //outFile << (uint32_t) params.m_txMode << "\t"; // txMode is not available at dl tx side
  outFile << (uint32_t) params.m_layer << "\t";
  outFile << (uint32_t) params.m_mcs << "\t";
  outFile << params.m_size << "\t";
  outFile << (uint32_t) params.m_rv << "\t";
  outFile << (uint32_t) params.m_ndi << std::endl;
  outFile.close ();
}

void
NistPhyTxStatsCalculator::UlPhyTransmission (NistPhyTransmissionStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_timestamp << params.m_rnti << params.m_layer << params.m_mcs << params.m_size << params.m_rv << params.m_ndi);
  NS_LOG_INFO ("Write UL Tx Phy Stats in " << GetUlTxOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_ulTxFirstWrite == true )
    {
      outFile.open (GetUlTxOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetUlTxOutputFilename ().c_str ());
          return;
        }
      m_ulTxFirstWrite = false;
//       outFile << "% time\tcellId\tIMSI\tRNTI\ttxMode\tlayer\tmcs\tsize\trv\tndi";
      outFile << "% time\tcellId\tIMSI\tRNTI\tlayer\tmcs\tsize\trv\tndi";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetUlTxOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetUlTxOutputFilename ().c_str ());
          return;
        }
    }

//   outFile << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t";
  outFile << params.m_timestamp << "\t";
  outFile << (uint32_t) params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  //outFile << (uint32_t) params.m_txMode << "\t";
  outFile << (uint32_t) params.m_layer << "\t";
  outFile << (uint32_t) params.m_mcs << "\t";
  outFile << params.m_size << "\t";
  outFile << (uint32_t) params.m_rv << "\t";
  outFile << (uint32_t) params.m_ndi << std::endl;
  outFile.close ();
}

void
NistPhyTxStatsCalculator::SlPhyTransmission (NistPhyTransmissionStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_timestamp << params.m_rnti << params.m_layer << params.m_mcs << params.m_size << params.m_rv << params.m_ndi);
  NS_LOG_INFO ("Write SL Tx Phy Stats in " << GetSlTxOutputFilename ().c_str ());

  std::ofstream outFile;
  if ( m_slTxFirstWrite == true )
    {
      outFile.open (GetSlTxOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlTxOutputFilename ().c_str ());
          return;
        }
      m_slTxFirstWrite = false;
//       outFile << "% time\tcellId\tIMSI\tRNTI\ttxMode\tlayer\tmcs\tsize\trv\tndi";
      outFile << "% time\tcellId\tIMSI\tRNTI\tlayer\tmcs\tsize\trv\tndi";
      outFile << std::endl;
    }
  else
    {
      outFile.open (GetSlTxOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlTxOutputFilename ().c_str ());
          return;
        }
    }

//   outFile << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t";
  outFile << params.m_timestamp << "\t";
  outFile << (uint32_t) params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  //outFile << (uint32_t) params.m_txMode << "\t";
  outFile << (uint32_t) params.m_layer << "\t";
  outFile << (uint32_t) params.m_mcs << "\t";
  outFile << params.m_size << "\t";
  outFile << (uint32_t) params.m_rv << "\t";
  outFile << (uint32_t) params.m_ndi << std::endl;
  outFile.close ();
}


void
NistPhyTxStatsCalculator::DlPhyTransmissionCallback (Ptr<NistPhyTxStatsCalculator> phyTxStats,
                      std::string path, NistPhyTransmissionStatParameters params)
{
  NS_LOG_FUNCTION (phyTxStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  if (phyTxStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = phyTxStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiForEnb (path, params.m_rnti);
      phyTxStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  params.m_imsi = imsi;
  phyTxStats->DlPhyTransmission (params);
}

void
NistPhyTxStatsCalculator::UlPhyTransmissionCallback (Ptr<NistPhyTxStatsCalculator> phyTxStats,
                      std::string path, NistPhyTransmissionStatParameters params)
{
  NS_LOG_FUNCTION (phyTxStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  if (phyTxStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = phyTxStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiForUe (path, params.m_rnti);
      phyTxStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  params.m_imsi = imsi;
  phyTxStats->UlPhyTransmission (params);
}

void
NistPhyTxStatsCalculator::SlPhyTransmissionCallback (Ptr<NistPhyTxStatsCalculator> phyTxStats,
                      std::string path, NistPhyTransmissionStatParameters params)
{
  NS_LOG_FUNCTION (phyTxStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  if (phyTxStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = phyTxStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiForUe (path, params.m_rnti);
      phyTxStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  params.m_imsi = imsi;
  phyTxStats->UlPhyTransmission (params);
}

} // namespace ns3

