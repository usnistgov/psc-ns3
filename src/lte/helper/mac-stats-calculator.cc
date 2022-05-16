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
    m_slUeSchFirstWrite (true),
    m_slUeDchFirstWrite (true)
{
  NS_LOG_FUNCTION (this);

}

MacStatsCalculator::~MacStatsCalculator ()
{
  NS_LOG_FUNCTION (this);
  if (m_dlOutFile.is_open())
    {
      m_dlOutFile.close();
    }

  if (m_ulOutFile.is_open())
    {
      m_ulOutFile.close();
    }
}

TypeId
MacStatsCalculator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MacStatsCalculator")
    .SetParent<LteStatsCalculator> ()
    .SetGroupName ("Lte")
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
    .AddAttribute ("SlCchOutputFilename",
                   "Name of the file where the Sidelink results will be saved.",
                   StringValue ("SlCchMacStats.txt"),
                   MakeStringAccessor (&MacStatsCalculator::SetSlUeCchOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("SlSchOutputFilename",
                   "Name of the file where the Sidelink results will be saved.",
                   StringValue ("SlSchMacStats.txt"),
                   MakeStringAccessor (&MacStatsCalculator::SetSlUeSchOutputFilename),
                   MakeStringChecker ())
    .AddAttribute ("SlDchOutputFilename",
                   "Name of the file where the Sidelink results will be saved.",
                   StringValue ("SlDchMacStats.txt"),
                   MakeStringAccessor (&MacStatsCalculator::SetSlUeDchOutputFilename),
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
MacStatsCalculator::SetSlUeDchOutputFilename (std::string outputFilename)
{
  LteStatsCalculator::SetSlPsdchOutputFilename (outputFilename);
}

std::string
MacStatsCalculator::GetSlUeDchOutputFilename (void)
{
  return LteStatsCalculator::GetSlPsdchOutputFilename ();
}

void
MacStatsCalculator::DlScheduling (uint16_t cellId, uint64_t imsi, DlSchedulingCallbackInfo dlSchedulingCallbackInfo)
{
  NS_LOG_FUNCTION (this << cellId << imsi << dlSchedulingCallbackInfo.frameNo << dlSchedulingCallbackInfo.subframeNo <<
                   dlSchedulingCallbackInfo.rnti << (uint32_t) dlSchedulingCallbackInfo.mcsTb1 << dlSchedulingCallbackInfo.sizeTb1 << (uint32_t) dlSchedulingCallbackInfo.mcsTb2 << dlSchedulingCallbackInfo.sizeTb2);
  NS_LOG_INFO ("Write DL Mac Stats in " << GetDlOutputFilename ().c_str ());

  if ( m_dlFirstWrite == true )
    {
      m_dlOutFile.open (GetDlOutputFilename ().c_str ());
      if (!m_dlOutFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetDlOutputFilename ().c_str ());
          return;
        }
      m_dlFirstWrite = false;
      m_dlOutFile << "% time\tcellId\tIMSI\tframe\tsframe\tRNTI\tmcsTb1\tsizeTb1\tmcsTb2\tsizeTb2\tccId";
      m_dlOutFile << "\n";
    }

  m_dlOutFile << Simulator::Now ().GetSeconds () << "\t";
  m_dlOutFile << (uint32_t) cellId << "\t";
  m_dlOutFile << imsi << "\t";
  m_dlOutFile << dlSchedulingCallbackInfo.frameNo << "\t";
  m_dlOutFile << dlSchedulingCallbackInfo.subframeNo << "\t";
  m_dlOutFile << dlSchedulingCallbackInfo.rnti << "\t";
  m_dlOutFile << (uint32_t) dlSchedulingCallbackInfo.mcsTb1 << "\t";
  m_dlOutFile << dlSchedulingCallbackInfo.sizeTb1 << "\t";
  m_dlOutFile << (uint32_t) dlSchedulingCallbackInfo.mcsTb2 << "\t";
  m_dlOutFile << dlSchedulingCallbackInfo.sizeTb2 << "\t";
  m_dlOutFile << (uint32_t) dlSchedulingCallbackInfo.componentCarrierId << std::endl;
}

void
MacStatsCalculator::UlScheduling (uint16_t cellId, uint64_t imsi, uint32_t frameNo,
                                  uint32_t subframeNo, uint16_t rnti,uint8_t mcsTb, uint16_t size, uint8_t componentCarrierId)
{
  NS_LOG_FUNCTION (this << cellId << imsi << frameNo << subframeNo << rnti << (uint32_t) mcsTb << size);
  NS_LOG_INFO ("Write UL Mac Stats in " << GetUlOutputFilename ().c_str ());

  if ( m_ulFirstWrite == true )
    {
      m_ulOutFile.open (GetUlOutputFilename ().c_str ());
      if (!m_ulOutFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetUlOutputFilename ().c_str ());
          return;
        }
      m_ulFirstWrite = false;
      m_ulOutFile << "% time\tcellId\tIMSI\tframe\tsframe\tRNTI\tmcs\tsize\tccId";
      m_ulOutFile << "\n";
    }

  m_ulOutFile << Simulator::Now ().GetSeconds () << "\t";
  m_ulOutFile << (uint32_t) cellId << "\t";
  m_ulOutFile << imsi << "\t";
  m_ulOutFile << frameNo << "\t";
  m_ulOutFile << subframeNo << "\t";
  m_ulOutFile << rnti << "\t";
  m_ulOutFile << (uint32_t) mcsTb << "\t";
  m_ulOutFile << size << "\t";
  m_ulOutFile << (uint32_t) componentCarrierId << std::endl;
}

void
MacStatsCalculator::SlUeCchScheduling (SlUeMacStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_frameNo << params.m_subframeNo
                   << params.m_rnti << (uint32_t) params.m_mcs << params.m_resIndex << params.m_txStartRB
                   << params.m_txLengthRB << params.m_psschItrp << params.m_sidelinkDropped);
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
      outFile << "% time\tcellId\tIMSI\tRNTI\tframe\tsframe\tscPrdStartFr\tscPrdStartSf\tresPscch\tsizeTb\tpscchRbLen\tpscchStartRb\thopping\thoppingInfo\tpsschRbLen\tpsschStartRb\tiTrp\tmcs\tl1GroupDstId\tdropped";
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

  outFile << params.m_timestamp << "\t";
  outFile << params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  outFile << params.m_frameNo << "\t";
  outFile << params.m_subframeNo << "\t";
  outFile << params.m_periodStartFrame << "\t";
  outFile << params.m_periodStartSubframe << "\t";
  outFile << params.m_resIndex << "\t";
  outFile << params.m_tbSize << "\t";
  outFile << (uint16_t) params.m_pscchTxLengthRB << "\t";
  outFile << (uint16_t) params.m_pscchTxStartRB << "\t";
  outFile << (uint16_t) params.m_hopping << "\t";
  outFile << (uint16_t) params.m_hoppingInfo << "\t";
  outFile << (uint16_t) params.m_txLengthRB << "\t";
  outFile << (uint16_t) params.m_txStartRB << "\t";
  outFile << (uint16_t) params.m_psschItrp << "\t";
  outFile << (uint16_t) params.m_mcs << "\t";
  outFile << (uint16_t) params.m_groupDstId << "\t";
  outFile << (uint16_t) params.m_sidelinkDropped << std::endl;

  outFile.close ();
}

void
MacStatsCalculator::SlUeSchScheduling (SlUeMacStatParameters params)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_rnti << params.m_frameNo << params.m_subframeNo
                   << (uint32_t) params.m_mcs << params.m_tbSize << params.m_txStartRB << params.m_txLengthRB);
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
      outFile << "% time\tcellId\tIMSI\tRNTI\tcurrFr\tcurrSf\tscPrdStartFr\tscPrdStartSf\tpsschRbLen\tpsschStartRb\tmcs\tsizeTb\trv\tdropped";
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

  outFile << params.m_timestamp << "\t";
  outFile << params.m_cellId << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  outFile << params.m_frameNo << "\t";
  outFile << params.m_subframeNo << "\t";
  outFile << params.m_periodStartFrame << "\t";
  outFile << params.m_periodStartSubframe << "\t";
  outFile << (uint16_t) params.m_txLengthRB << "\t";
  outFile << (uint16_t) params.m_txStartRB << "\t";
  outFile << (uint16_t) params.m_mcs << "\t";
  outFile << params.m_tbSize << "\t";
  outFile << (uint16_t) params.m_rv << "\t";
  outFile << (uint16_t) params.m_sidelinkDropped << std::endl;
  outFile.close ();
}

void 
MacStatsCalculator::SlUeDchScheduling (SlUeMacStatParameters params, LteSlDiscHeader discMsg)
{
  NS_LOG_FUNCTION (this << params.m_cellId << params.m_imsi << params.m_rnti);
  NS_LOG_INFO ("Writing SL Discovery Channel UE Mac Stats in " << GetSlUeDchOutputFilename ().c_str ());
  	
  std::ofstream outFile;
  if ( m_slUeDchFirstWrite == true )
    {
      outFile.open (GetSlUeDchOutputFilename ().c_str ());
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlUeDchOutputFilename ().c_str ());
          return;
        }
      m_slUeDchFirstWrite = false;
      outFile << "Time\tIMSI\tRNTI\tframe\tsubframe\tdiscPrdStartFr\tdiscPrdStartSf\tresPsdch\tpsdchRbLen\tpsdchStartRb\tmcs\tsizeTb\trv\tDiscType\tContentType\tDiscModel\tContent\tdropped" << std::endl;
    }
  else
    {
      outFile.open (GetSlUeDchOutputFilename ().c_str (),  std::ios_base::app);
      if (!outFile.is_open ())
        {
          NS_LOG_ERROR ("Can't open file " << GetSlUeDchOutputFilename ().c_str ());
          return;
        }
    }

  outFile << params.m_timestamp << "\t";
  outFile << params.m_imsi << "\t";
  outFile << params.m_rnti << "\t";
  outFile << params.m_frameNo << "\t";
  outFile << params.m_subframeNo << "\t";
  outFile << params.m_periodStartFrame << "\t";
  outFile << params.m_periodStartSubframe << "\t";
  outFile << params.m_resIndex << "\t";
  outFile << (uint16_t) params.m_txLengthRB << "\t";
  outFile << (uint16_t) params.m_txStartRB << "\t";
  outFile << (uint16_t) params.m_mcs << "\t";
  outFile << params.m_tbSize << "\t";
  outFile << (uint16_t) params.m_rv << "\t";
  uint8_t msgType = discMsg.GetDiscoveryMsgType ();
  outFile << (uint16_t) discMsg.GetDiscoveryType ()  << "\t" << (uint16_t) discMsg.GetDiscoveryContentType ()
                                                                << "\t" << (uint16_t) discMsg.GetDiscoveryModel ()<< "\t";


  switch (msgType)
  {
    case LteSlDiscHeader::DISC_RELAY_ANNOUNCEMENT://UE-to-Network Relay Discovery Announcement in model A
    case LteSlDiscHeader::DISC_RELAY_RESPONSE://UE-to-Network Relay Discovery Response in model B
      {
        outFile << discMsg.GetRelayServiceCode () << ";" << discMsg.GetInfo () << ";" << discMsg.GetRelayUeId () << ";" << (uint16_t) discMsg.GetStatusIndicator () << ";" << 0 << std::endl;
      }
      break;
  case LteSlDiscHeader::DISC_RELAY_SOLICITATION:
    outFile << discMsg.GetRelayServiceCode () << ";" << discMsg.GetInfo () << ";" << (uint16_t) discMsg.GetURDSComposition () << ";" << discMsg.GetRelayUeId () << ";" << 0 << std::endl;
    break;
    case LteSlDiscHeader::DISC_OPEN_ANNOUNCEMENT:
      case LteSlDiscHeader::DISC_RESTRICTED_QUERY:
    case LteSlDiscHeader::DISC_RESTRICTED_RESPONSE:
      { //open or restricted announcement
        outFile << discMsg.GetApplicationCode () << std::endl;
      }
      break;
    default:
      NS_FATAL_ERROR ("Invalid discovery message type " << (uint16_t) msgType);
  }

  outFile << (uint16_t) params.m_sidelinkDropped << std::endl;
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

void 
MacStatsCalculator::SlUeDchSchedulingCallback (Ptr<MacStatsCalculator> macStats, std::string path, SlUeMacStatParameters params, LteSlDiscHeader discMsg)
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
  
  macStats->SlUeDchScheduling (params, discMsg);
}

} // namespace ns3
