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

#include "nist-lte-stats-calculator.h"

#include <ns3/log.h>
#include <ns3/config.h>
#include <ns3/nist-lte-enb-rrc.h>
#include <ns3/nist-lte-ue-rrc.h>
#include <ns3/nist-lte-enb-net-device.h>
#include <ns3/nist-lte-ue-net-device.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistLteStatsCalculator");

NS_OBJECT_ENSURE_REGISTERED (NistLteStatsCalculator);

NistLteStatsCalculator::NistLteStatsCalculator ()
  : m_dlOutputFilename (""),
    m_ulOutputFilename ("")
{
  // Nothing to do here

}

NistLteStatsCalculator::~NistLteStatsCalculator ()
{
  // Nothing to do here
}


TypeId
NistLteStatsCalculator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistLteStatsCalculator")
    .SetParent<Object> ()
    .AddConstructor<NistLteStatsCalculator> ()
  ;
  return tid;
}

void
NistLteStatsCalculator::SetSlUeOutputFilename (std::string outputFilename)
{
  m_slUeOutputFilename = outputFilename;
}

std::string
NistLteStatsCalculator::GetSlUeOutputFilename (void)
{
  return m_slUeOutputFilename;
}

void
NistLteStatsCalculator::SetSlPscchOutputFilename (std::string outputFilename)
{
  m_slPscchOutputFilename = outputFilename;
}

std::string
NistLteStatsCalculator::GetSlPscchOutputFilename (void)
{
  return m_slPscchOutputFilename;
}

void
NistLteStatsCalculator::SetSlSchUeOutputFilename (std::string outputFilename)
{
  m_slSchUeOutputFilename = outputFilename;
}

std::string
NistLteStatsCalculator::GetSlSchUeOutputFilename (void)
{
  return m_slSchUeOutputFilename;
}

void
NistLteStatsCalculator::SetUlOutputFilename (std::string outputFilename)
{
  m_ulOutputFilename = outputFilename;
}

std::string
NistLteStatsCalculator::GetUlOutputFilename (void)
{
  return m_ulOutputFilename;
}

void
NistLteStatsCalculator::SetDlOutputFilename (std::string outputFilename)
{
  m_dlOutputFilename = outputFilename;
}

std::string
NistLteStatsCalculator::GetDlOutputFilename (void)
{
  return m_dlOutputFilename;
}

void
NistLteStatsCalculator::SetSlOutputFilename (std::string outputFilename)
{
  m_slOutputFilename = outputFilename;
}

std::string
NistLteStatsCalculator::GetSlOutputFilename (void)
{
  return m_slOutputFilename;
}


bool
NistLteStatsCalculator::ExistsImsiPath (std::string path)
{
  if (m_pathImsiMap.find (path) == m_pathImsiMap.end () )
    {
      return false;
    }
  else
    {
      return true;
    }
}

void
NistLteStatsCalculator::SetImsiPath (std::string path, uint64_t imsi)
{
  NS_LOG_FUNCTION (this << path << imsi);
  m_pathImsiMap[path] = imsi;
}

uint64_t
NistLteStatsCalculator::GetImsiPath (std::string path)
{
  return m_pathImsiMap.find (path)->second;
}

bool
NistLteStatsCalculator::ExistsCellIdPath (std::string path)
{
  if (m_pathCellIdMap.find (path) == m_pathCellIdMap.end () )
    {
      return false;
    }
  else
    {
      return true;
    }
}

void
NistLteStatsCalculator::SetCellIdPath (std::string path, uint16_t cellId)
{
  NS_LOG_FUNCTION (this << path << cellId);
  m_pathCellIdMap[path] = cellId;
}

uint16_t
NistLteStatsCalculator::GetCellIdPath (std::string path)
{
  return m_pathCellIdMap.find (path)->second;
}


uint64_t
NistLteStatsCalculator::FindImsiFromEnbRlcPath (std::string path)
{
  NS_LOG_FUNCTION (path);
  // Sample path input:
  // /NodeList/#NodeId/DeviceList/#DeviceId/NistLteEnbRrc/UeMap/#C-RNTI/DataRadioBearerMap/#LCID/NistLteRlc/RxPDU

  // We retrieve the NistUeManager associated to the C-RNTI and perform the IMSI lookup
  std::string ueMapPath = path.substr (0, path.find ("/DataRadioBearerMap"));
  Config::MatchContainer match = Config::LookupMatches (ueMapPath);

  if (match.GetN () != 0)
    {
      Ptr<Object> ueInfo = match.Get (0);
      NS_LOG_LOGIC ("FindImsiFromEnbRlcPath: " << path << ", " << ueInfo->GetObject<NistUeManager> ()->GetImsi ());
      return ueInfo->GetObject<NistUeManager> ()->GetImsi ();
    }
  else
    {
      NS_FATAL_ERROR ("Lookup " << ueMapPath << " got no matches");
    }
}

uint64_t
NistLteStatsCalculator::FindImsiFromUePhy (std::string path)
{
  NS_LOG_FUNCTION (path);
  // Sample path input:
  // /NodeList/#NodeId/DeviceList/#DeviceId/NistLteUePhy

  // We retrieve the NistUeInfo associated to the C-RNTI and perform the IMSI lookup
  std::string ueRlcPath = path.substr (0, path.find ("/NistLteUePhy"));
  ueRlcPath += "/NistLteUeRrc";
  Config::MatchContainer match = Config::LookupMatches (ueRlcPath);

  if (match.GetN () != 0)
    {
      Ptr<Object> ueRrc = match.Get (0);
      return ueRrc->GetObject<NistLteUeRrc> ()->GetImsi ();
    }
  else
    {
      NS_FATAL_ERROR ("Lookup " << ueRlcPath << " got no matches");
    }
  return 0;
}


uint64_t
NistLteStatsCalculator::FindImsiFromNistLteNetDevice (std::string path)
{
  NS_LOG_FUNCTION (path);
  // Sample path input:
  // /NodeList/#NodeId/DeviceList/#DeviceId/

  // We retrieve the Imsi associated to the NistLteUeNetDevice
  Config::MatchContainer match = Config::LookupMatches (path);

  if (match.GetN () != 0)
    {
      Ptr<Object> ueNetDevice = match.Get (0);
      NS_LOG_LOGIC ("FindImsiFromNistLteNetDevice: " << path << ", " << ueNetDevice->GetObject<NistLteUeNetDevice> ()->GetImsi ());
      return ueNetDevice->GetObject<NistLteUeNetDevice> ()->GetImsi ();
    }
  else
    {
      NS_FATAL_ERROR ("Lookup " << path << " got no matches");
    }
}

uint16_t
NistLteStatsCalculator::FindCellIdFromEnbRlcPath (std::string path)
{
  NS_LOG_FUNCTION (path);
  // Sample path input:
  // /NodeList/#NodeId/DeviceList/#DeviceId/NistLteEnbRrc/UeMap/#C-RNTI/DataRadioBearerMap/#LCID/NistLteRlc/RxPDU

  // We retrieve the CellId associated to the Enb
  std::string enbNetDevicePath = path.substr (0, path.find ("/NistLteEnbRrc"));
  Config::MatchContainer match = Config::LookupMatches (enbNetDevicePath);
  if (match.GetN () != 0)
    {
      Ptr<Object> enbNetDevice = match.Get (0);
      NS_LOG_LOGIC ("FindCellIdFromEnbRlcPath: " << path << ", " << enbNetDevice->GetObject<NistLteEnbNetDevice> ()->GetCellId ());
      return enbNetDevice->GetObject<NistLteEnbNetDevice> ()->GetCellId ();
    }
  else
    {
      NS_FATAL_ERROR ("Lookup " << enbNetDevicePath << " got no matches");
    }
}

uint64_t
NistLteStatsCalculator::FindImsiFromEnbMac (std::string path, uint16_t rnti)
{
  NS_LOG_FUNCTION (path << rnti);

  // /NodeList/#NodeId/DeviceList/#DeviceId/NistLteEnbMac/DlScheduling
  std::ostringstream oss;
  std::string p = path.substr (0, path.find ("/NistLteEnbMac"));
  oss << rnti;
  p += "/NistLteEnbRrc/UeMap/" + oss.str ();
  uint64_t imsi = FindImsiFromEnbRlcPath (p);
  NS_LOG_LOGIC ("FindImsiFromEnbMac: " << path << ", " << rnti << ", " << imsi);
  return imsi;
}

uint16_t
NistLteStatsCalculator::FindCellIdFromEnbMac (std::string path, uint16_t rnti)
{
  NS_LOG_FUNCTION (path << rnti);
  // /NodeList/#NodeId/DeviceList/#DeviceId/NistLteEnbMac/DlScheduling
  std::ostringstream oss;
  std::string p = path.substr (0, path.find ("/NistLteEnbMac"));
  oss << rnti;
  p += "/NistLteEnbRrc/UeMap/" + oss.str ();
  uint16_t cellId = FindCellIdFromEnbRlcPath (p);
  NS_LOG_LOGIC ("FindCellIdFromEnbMac: " << path << ", "<< rnti << ", " << cellId);
  return cellId;
}


uint64_t
NistLteStatsCalculator::FindImsiForEnb (std::string path, uint16_t rnti)
{
  NS_LOG_FUNCTION (path << rnti);
  uint64_t imsi = 0;
  if (path.find ("/DlPhyTransmission"))
    {
      // /NodeList/0/DeviceList/0/NistLteEnbPhy/DlPhyTransmission/NistLteEnbRrc/UeMap/1
      std::ostringstream oss;
      std::string p = path.substr (0, path.find ("/NistLteEnbPhy"));
      oss << rnti;
      p += "/NistLteEnbRrc/UeMap/" + oss.str ();
      imsi = FindImsiFromEnbRlcPath (p);
      NS_LOG_LOGIC ("FindImsiForEnb[Tx]: " << path << ", " << rnti << ", " << imsi);
    }
  else if (path.find ("/UlPhyReception"))
    {
      std::string p = path.substr (0, path.find ("/NistLteUePhy"));
      imsi = FindImsiFromNistLteNetDevice (p);
      NS_LOG_LOGIC ("FindImsiForEnb[Rx]: " << path << ", " << rnti << ", " << imsi);
    }
  return imsi;
}


uint64_t
NistLteStatsCalculator::FindImsiForUe (std::string path, uint16_t rnti)
{
  NS_LOG_FUNCTION (path << rnti);
  uint64_t imsi = 0;
  if (path.find ("/UlPhyTransmission"))
    {
      std::string p = path.substr (0, path.find ("/NistLteUePhy"));
      imsi = FindImsiFromNistLteNetDevice (p);
      NS_LOG_LOGIC ("FindImsiForUe[Tx]: " << path << ", " << rnti << ", " << imsi);
    }
  else if (path.find ("/DlPhyReception"))
    {
      // /NodeList/0/DeviceList/0/NistLteEnbPhy/NistLteSpectrumPhy
      std::ostringstream oss;
      std::string p = path.substr (0, path.find ("/NistLteEnbPhy"));
      oss << rnti;
      p += "/NistLteEnbRrc/UeMap/" + oss.str ();
      imsi = FindImsiFromEnbRlcPath (p);
      NS_LOG_LOGIC ("FindImsiForUe[Rx]: " << path << ", " << rnti << ", " << imsi);
    }
  return imsi;
}


} // namespace ns3
