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
 * Modified by: Marco Miozzo <mmiozzo@cttc.es>
 *        Convert MacStatsCalculator in PhyRxStatsCalculator
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#include "phy-rx-stats-calculator.h"
#include "ns3/string.h"
#include <ns3/simulator.h>
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PhyRxStatsCalculator");

NS_OBJECT_ENSURE_REGISTERED (PhyRxStatsCalculator);

PhyRxStatsCalculator::PhyRxStatsCalculator ()
  : m_dlRxFirstWrite (true),
    m_ulRxFirstWrite (true),
    m_slRxFirstWrite (true),
    m_slPscchRxFirstWrite (true)
{
  NS_LOG_FUNCTION (this);

}

PhyRxStatsCalculator::~PhyRxStatsCalculator ()
{
  NS_LOG_FUNCTION (this);
  if (m_dlRxOutFile.is_open())
    {
      m_dlRxOutFile.close();
    }

  if (m_ulRxOutFile.is_open())
    {
      m_ulRxOutFile.close();
    }
}

TypeId
PhyRxStatsCalculator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PhyRxStatsCalculator")
    .SetParent<LteStatsCalculator> ()
    .SetGroupName ("Lte")
    .AddConstructor<PhyRxStatsCalculator> ()
    .AddAttribute ("DlRxOutputFilename",
                   "Name of the file where the downlink results will be saved.",
                   StringValue ("DlRxPhyStats.txt"),
                   MakeStringAccessor (&PhyRxStatsCalculator::SetDlRxOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("UlRxOutputFilename",
                   "Name of the file where the uplink results will be saved.",
                   StringValue ("UlRxPhyStats.txt"),
                   MakeStringAccessor (&PhyRxStatsCalculator::SetUlRxOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("SlRxOutputFilename",
                   "Name of the file where the PSSCH and PSDCH phy reception results will be saved.",
                   StringValue ("SlRxPhyStats.txt"),
                   MakeStringAccessor (&PhyRxStatsCalculator::SetSlRxOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("SlCchRxOutputFilename",
                   "Name of the file where the Sidelink PSCCH results will be saved.",
                   StringValue ("SlCchRxPhyStats.txt"),
                   MakeStringAccessor (&PhyRxStatsCalculator::SetSlPscchRxOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
PhyRxStatsCalculator::SetUlRxOutputFilename (std::string outputFilename)
{
  LteStatsCalculator::SetUlOutputFilename (outputFilename);
}

std::string
PhyRxStatsCalculator::GetUlRxOutputFilename (void)
{
  return LteStatsCalculator::GetUlOutputFilename ();
}

void
PhyRxStatsCalculator::SetDlRxOutputFilename (std::string outputFilename)
{
  LteStatsCalculator::SetDlOutputFilename (outputFilename);
}

std::string
PhyRxStatsCalculator::GetDlRxOutputFilename (void)
{
  return LteStatsCalculator::GetDlOutputFilename ();
}

void
PhyRxStatsCalculator::SetSlRxOutputFilename (std::string outputFilename)
{
  LteStatsCalculator::SetSlOutputFilename (outputFilename);
}

std::string
PhyRxStatsCalculator::GetSlRxOutputFilename (void)
{
  return LteStatsCalculator::GetSlOutputFilename ();
}

void
PhyRxStatsCalculator::SetSlPscchRxOutputFilename (std::string outputFilename)
{
  LteStatsCalculator::SetSlPscchOutputFilename (outputFilename);
}

std::string
PhyRxStatsCalculator::GetSlPscchRxOutputFilename (void)
{
  return LteStatsCalculator::GetSlPscchOutputFilename ();
}

void
PhyRxStatsCalculator::DlPhyReception (PhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_timestamp << params.m_rnti << params.m_layer << params.m_mcs << params.m_size << params.m_rv << params.m_ndi << params.m_correctness);
  NS_LOG_INFO ("Write DL Rx Phy Stats in " << GetDlRxOutputFilename ().c_str ());

  if ( m_dlRxFirstWrite == true )
    {
      m_dlRxOutFile.open (GetDlRxOutputFilename ().c_str ());
      if (!m_dlRxOutFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetDlRxOutputFilename ().c_str ());
          return;
        }
      m_dlRxFirstWrite = false;
      m_dlRxOutFile << "% time\tcellId\tIMSI\tRNTI\ttxMode\tlayer\tmcs\tsize\trv\tndi\tcorrect\tccId";
      m_dlRxOutFile << "\n";
    }

  m_dlRxOutFile << params.m_timestamp << "\t";
  m_dlRxOutFile << (uint32_t) params.m_cellId << "\t";
  m_dlRxOutFile << params.m_imsi << "\t";
  m_dlRxOutFile << params.m_rnti << "\t";
  m_dlRxOutFile << (uint32_t) params.m_txMode << "\t";
  m_dlRxOutFile << (uint32_t) params.m_layer << "\t";
  m_dlRxOutFile << (uint32_t) params.m_mcs << "\t";
  m_dlRxOutFile << params.m_size << "\t";
  m_dlRxOutFile << (uint32_t) params.m_rv << "\t";
  m_dlRxOutFile << (uint32_t) params.m_ndi << "\t";
  m_dlRxOutFile << (uint32_t) params.m_correctness << "\t";
  m_dlRxOutFile << (uint32_t) params.m_ccId << std::endl;
}

void
PhyRxStatsCalculator::UlPhyReception (PhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_timestamp << params.m_rnti << params.m_layer << params.m_mcs << params.m_size << params.m_rv << params.m_ndi << params.m_correctness);
  NS_LOG_INFO ("Write UL Rx Phy Stats in " << GetUlRxOutputFilename ().c_str ());

  if ( m_ulRxFirstWrite == true )
    {
      m_ulRxOutFile.open (GetUlRxOutputFilename ().c_str ());
      if (!m_ulRxOutFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetUlRxOutputFilename ().c_str ());
          return;
        }
      m_ulRxFirstWrite = false;
      m_ulRxOutFile << "% time\tcellId\tIMSI\tRNTI\tlayer\tmcs\tsize\trv\tndi\tcorrect\tccId";
      m_ulRxOutFile << "\n";
    }

  m_ulRxOutFile << params.m_timestamp << "\t";
  m_ulRxOutFile << (uint32_t) params.m_cellId << "\t";
  m_ulRxOutFile << params.m_imsi << "\t";
  m_ulRxOutFile << params.m_rnti << "\t";
  m_ulRxOutFile << (uint32_t) params.m_layer << "\t";
  m_ulRxOutFile << (uint32_t) params.m_mcs << "\t";
  m_ulRxOutFile << params.m_size << "\t";
  m_ulRxOutFile << (uint32_t) params.m_rv << "\t";
  m_ulRxOutFile << (uint32_t) params.m_ndi << "\t";
  m_ulRxOutFile << (uint32_t) params.m_correctness << "\t";
  m_ulRxOutFile << (uint32_t) params.m_ccId << std::endl;
}

void
PhyRxStatsCalculator::SlPhyReception (PhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_timestamp << params.m_rnti << params.m_layer << params.m_mcs << params.m_size << params.m_rv << params.m_ndi << params.m_correctness);
  NS_LOG_INFO ("Write SL Rx Phy Stats in " << GetSlRxOutputFilename ().c_str ());

  std::ofstream outFile;
  if (m_slRxFirstWrite == true)
    {
      outFile.open (GetSlRxOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlRxOutputFilename ().c_str ());
          return;
        }
      m_slRxFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tRNTI\tlayer\tmcs\tsize\trv\tndi\tcorrect\tavrgSinrPerRb";
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
  outFile << (double) params.m_sinrPerRb <<std::endl;
  outFile.close ();
}

void
PhyRxStatsCalculator::SlPscchReception (SlPhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_timestamp << params.m_cellId << params.m_imsi << params.m_rnti << (uint16_t)params.m_mcs << params.m_size << params.m_resPscch << (uint16_t)params.m_rbLen << (uint16_t)params.m_rbStart<< (uint16_t)params.m_iTrp << (uint16_t)params.m_hopping << (uint16_t)params.m_groupDstId << (uint16_t)params.m_correctness);
  NS_LOG_INFO ("Write SL Rx PSCCH Stats in " << GetSlPscchRxOutputFilename ().c_str ());

  std::ofstream outFile;
  if (m_slPscchRxFirstWrite == true)
    {
      outFile.open (GetSlPscchRxOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlPscchRxOutputFilename ().c_str ());
          return;
        }
      m_slPscchRxFirstWrite = false;
      outFile << "% time\tcellId\tIMSI\tRNTI\tresPscch\tsizeTb\thopping\thoppingInfo\tpsschRbLen\tpsschStartRb\tiTrp\tmcs\tl1GroupDstId\tcorrect";
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

  outFile << params.m_timestamp << "\t";
  outFile << params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  outFile << params.m_resPscch << "\t";
  outFile << params.m_size << "\t";
  outFile << (uint32_t) params.m_hopping << "\t";
  outFile << (uint32_t) params.m_hoppingInfo << "\t";
  outFile << (uint32_t) params.m_rbLen << "\t";
  outFile << (uint32_t) params.m_rbStart << "\t";
  outFile << (uint32_t) params.m_iTrp << "\t";
  outFile << (uint32_t) params.m_mcs << "\t";
  outFile << (uint32_t) params.m_groupDstId << "\t";
  outFile << (uint32_t) params.m_correctness <<std::endl;
  outFile.close ();
}

void
PhyRxStatsCalculator::DlPhyReceptionCallback (Ptr<PhyRxStatsCalculator> phyRxStats,
                                              std::string path, PhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (phyRxStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  std::string pathUePhy  = path.substr (0, path.find ("/ComponentCarrierMapUe"));
  if (phyRxStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = phyRxStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiFromLteNetDevice (pathUePhy);
      phyRxStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  params.m_imsi = imsi;
  phyRxStats->DlPhyReception (params);
}

void
PhyRxStatsCalculator::UlPhyReceptionCallback (Ptr<PhyRxStatsCalculator> phyRxStats,
                                              std::string path, PhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (phyRxStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  std::string pathEnb  = path.substr (0, path.find ("/ComponentCarrierMap"));
  pathAndRnti << pathEnb << "/LteEnbRrc/UeMap/" << params.m_rnti;
  if (phyRxStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = phyRxStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiFromEnbRlcPath (pathAndRnti.str ());
      phyRxStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  params.m_imsi = imsi;
  phyRxStats->UlPhyReception (params);
}

void
PhyRxStatsCalculator::SlPhyReceptionCallback (Ptr<PhyRxStatsCalculator> phyRxStats,
                      std::string path, PhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (phyRxStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  std::string pathSlUePhy  = path.substr (0, path.find ("/ComponentCarrierMapUe"));
  if (phyRxStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = phyRxStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiFromLteNetDevice (pathSlUePhy);
      phyRxStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  params.m_imsi = imsi;
  phyRxStats->SlPhyReception (params);
}

void
PhyRxStatsCalculator::SlPscchReceptionCallback (Ptr<PhyRxStatsCalculator> phyRxStats,
                      std::string path, SlPhyReceptionStatParameters params)
{
  NS_LOG_FUNCTION (phyRxStats << path);
  uint64_t imsi = 0;
  std::ostringstream pathAndRnti;
  pathAndRnti << path << "/" << params.m_rnti;
  std::string pathSlUePhy  = path.substr (0, path.find ("/ComponentCarrierMapUe"));
  if (phyRxStats->ExistsImsiPath (pathAndRnti.str ()) == true)
    {
      imsi = phyRxStats->GetImsiPath (pathAndRnti.str ());
    }
  else
    {
      imsi = FindImsiFromLteNetDevice (pathSlUePhy);
      phyRxStats->SetImsiPath (pathAndRnti.str (), imsi);
    }

  params.m_imsi = imsi;
  phyRxStats->SlPscchReception (params);
}

} // namespace ns3
