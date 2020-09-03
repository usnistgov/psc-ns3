/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 */

#include "nr-sl-ue-rrc.h"

#include <ns3/lte-ue-rrc.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/lte-radio-bearer-info.h>

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/pointer.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/simulator.h>

#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrSlUeRrc");
NS_OBJECT_ENSURE_REGISTERED (NrSlUeRrc);

TypeId NrSlUeRrc::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::NrSlUeRrc")
    .SetParent<Object> ()
    .SetGroupName ("Lte")
    .AddConstructor<NrSlUeRrc> ()
  ;
  return tid;
}
NrSlUeRrc::NrSlUeRrc ()
{
  NS_LOG_FUNCTION (this);
  m_nrSlRrcSapUser = new MemberNrSlUeRrcSapUser<NrSlUeRrc> (this);
}

NrSlUeRrc::~NrSlUeRrc (void)
{
  NS_LOG_FUNCTION (this);
}

void
NrSlUeRrc::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_nrSlRrcSapUser;
}

void
NrSlUeRrc::SetNrSlUeRrcSapProvider (NrSlUeRrcSapProvider* s)
{
  NS_LOG_FUNCTION (this);
  m_nrSlUeRrcSapProvider = s;
}

NrSlUeRrcSapUser*
NrSlUeRrc::GetNrSlUeRrcSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_nrSlRrcSapUser;
}

void
NrSlUeRrc::SetNrSlEnabled (bool status)
{
  NS_LOG_FUNCTION (this);
  m_slEnabled = status;
}

bool
NrSlUeRrc::IsNrSlEnabled ()
{
  NS_LOG_FUNCTION (this);
  return m_slEnabled;
}

void
NrSlUeRrc::SetSourceL2Id (uint32_t srcL2Id)
{
  NS_LOG_FUNCTION (this);
  m_srcL2Id = srcL2Id;
  //Set source L2 id in RCC
  m_nrSlUeRrcSapProvider->SetSourceL2Id (srcL2Id);
}

uint32_t
NrSlUeRrc::DoGetSourceL2Id ()
{
  NS_LOG_FUNCTION (this);
  return m_srcL2Id;
}

void
NrSlUeRrc::SetNrSlPreconfiguration (const LteRrcSap::SidelinkPreconfigNr &preconfiguration)
{
  NS_LOG_FUNCTION (this);
  m_preconfiguration = preconfiguration;
  SetTddPattern ();
  //Tell RRC to populate pools
  m_nrSlUeRrcSapProvider->PopulatePools ();
}

void
NrSlUeRrc::SetTddPattern ()
{
  NS_LOG_FUNCTION (this);
  std::string tddPattern = m_preconfiguration.slPreconfigGeneral.slTddConfig.tddPattern;
  NS_LOG_INFO (this << " Setting TDD pattern " << tddPattern);
  static std::unordered_map<std::string, NrSlUeRrc::LteNrTddSlotType> lookupTable =
  {
    { "DL", NrSlUeRrc::LteNrTddSlotType::DL },
    { "UL", NrSlUeRrc::LteNrTddSlotType::UL },
    { "S",  NrSlUeRrc::LteNrTddSlotType::S },
    { "F",  NrSlUeRrc::LteNrTddSlotType::F },
  };

  std::vector<NrSlUeRrc::LteNrTddSlotType> vector;
  std::stringstream ss (tddPattern);
  std::string token;
  std::vector<std::string> extracted;

   while (std::getline(ss, token, '|'))
     {
       extracted.push_back(token);
     }

   for (const auto & v : extracted)
     {
       if (lookupTable.find (v) == lookupTable.end())
         {
           NS_FATAL_ERROR ("Pattern type " << v << " not valid. Valid values are: DL UL F S");
         }
       vector.push_back (lookupTable[v]);
     }

   m_tddPattern = vector;
}

const LteRrcSap::SidelinkPreconfigNr
NrSlUeRrc::DoGetNrSlPreconfiguration ()
{
  NS_LOG_FUNCTION (this);
  return m_preconfiguration;
}

const std::vector <std::bitset<1>>
NrSlUeRrc::DoGetPhysicalSlPool (const std::vector <std::bitset<1>> &slBitMap)
{
  NS_LOG_FUNCTION (this);
  std::vector <std::bitset<1>> finalSlPool;

  uint16_t countUl = std::count (m_tddPattern.begin (), m_tddPattern.end (), NrSlUeRrc::LteNrTddSlotType::UL);
  NS_LOG_DEBUG ("number of uplinks in the given TDD pattern " << countUl);
  //If you will remove the following assert you will burn in the
  //do while loop below till the end of time!
  NS_ASSERT_MSG (countUl > 0, "No UL slot found in the given TDD pattern");

  NS_ABORT_MSG_IF (slBitMap.size () % countUl != 0, "SL bit map size should be multiple of number of UL slots in the TDD pattern");
  NS_ABORT_MSG_IF (slBitMap.size () < m_tddPattern.size (), "SL bit map size should be greater than or equal to the TDD pattern size");

  auto patternIt = m_tddPattern.cbegin ();
  auto slBitMapit = slBitMap.cbegin();

  do
    {
      if (*patternIt != NrSlUeRrc::LteNrTddSlotType::UL)
        {
          NS_LOG_DEBUG ("Not an UL slot :  " << *patternIt << ", putting 0 in the final bitmap");
          finalSlPool.push_back (0);
        }
      else if (*slBitMapit == 1)
        {
          // UL slot and SL bitmap value is 1
          NS_LOG_DEBUG ("It is an UL slot :  " << *patternIt << ", and SL bitmap value is " << *slBitMapit << ", putting 1 in the final bitmap");
          finalSlPool.push_back (1);
          slBitMapit++;
        }
      else
        {
          // UL slot and SL bitmap value is 0
          NS_LOG_DEBUG ("It is an UL slot :  " << *patternIt << ", but SL bitmap value is " << *slBitMapit << ", putting 0 in the final bitmap");
          finalSlPool.push_back (0);
          slBitMapit++;
        }

      if (patternIt == m_tddPattern.cend () - 1)
        {
          NS_LOG_DEBUG ("It is the last element of the TDD pattern " << *patternIt);

          if (slBitMapit == slBitMap.cend ())
            {
              //if we have cover all the SL bitmap we are done. Break now.
              break;
            }
          else
            {
              // we have not covered all the SL bitmap. Prepare to re-apply the TDD pattern
              patternIt = m_tddPattern.cbegin ();
              NS_LOG_DEBUG ("re-assigning to the first element of tdd pattern " << *patternIt);
            }
        }
      else
        {
          patternIt++;
        }

    }
  while (patternIt != m_tddPattern.end ());

  return finalSlPool;
}

void
NrSlUeRrc::StoreSlBwpId (uint8_t bwpId)
{
  NS_LOG_FUNCTION (this << +bwpId);
  std::pair<std::set<uint8_t>::iterator,bool> ret;
  ret = m_slBwpIds.insert (bwpId);
  NS_ABORT_MSG_IF (ret.second == false, "BWP id " << +bwpId << " already exists");
}

const std::set <uint8_t>
NrSlUeRrc::DoGetBwpIdContainer ()
{
  NS_LOG_FUNCTION (this);
  return m_slBwpIds;
}

void
NrSlUeRrc::DoAddNrSlDataRadioBearer (Ptr<NrSlDataRadioBearerInfo> slDrb)
{
  NS_LOG_FUNCTION (this);
  NrSlDrbMapPerL2Id::iterator destIt = m_slDrbMap.find (slDrb->m_destinationL2Id);
  if (destIt == m_slDrbMap.end ())
    {
      NS_LOG_LOGIC ("First SL DRB for destination " << slDrb->m_destinationL2Id);
      NrSlDrbMapPerLcId mapPerLcId;
      mapPerLcId.insert (std::pair<uint8_t, Ptr<NrSlDataRadioBearerInfo> > (slDrb->m_logicalChannelIdentity, slDrb));
      m_slDrbMap.insert (std::pair<uint32_t, NrSlDrbMapPerLcId> (slDrb->m_destinationL2Id, mapPerLcId));
    }
  else
    {
      NS_FATAL_ERROR ("Currently supporting only one SL DRB per pair of source/destination");
      // once we support multiple SL DRB per pair of source/destination
      // above assert can be removed, and following code will be used
      NrSlDrbMapPerLcId::iterator lcIt;
      lcIt = destIt->second.find (slDrb->m_logicalChannelIdentity);
      if (lcIt == destIt->second.end ())
        {
          //New bearer for the destination
          destIt->second.insert (std::pair<uint8_t, Ptr<NrSlDataRadioBearerInfo> > (slDrb->m_logicalChannelIdentity, slDrb));
        }
      else
        {
          NS_FATAL_ERROR ("SL DRB with LC id = " << +slDrb->m_logicalChannelIdentity << " already exists");
        }

    }
}

void
NrSlUeRrc::DoAddNrSlRxDataRadioBearer (Ptr<NrSlDataRadioBearerInfo> slRxDrb)
{
  NS_LOG_FUNCTION (this);
  NrSlDrbMapPerL2Id::iterator srcIt = m_slRxDrbMap.find (slRxDrb->m_sourceL2Id);
  if (srcIt == m_slRxDrbMap.end ())
    {
      NS_LOG_LOGIC ("First SL RX DRB for remote UE with source L2 id " << slRxDrb->m_sourceL2Id);
      NrSlDrbMapPerLcId mapPerRxLcId;
      mapPerRxLcId.insert (std::pair<uint8_t, Ptr<NrSlDataRadioBearerInfo> > (slRxDrb->m_logicalChannelIdentity, slRxDrb));
      m_slDrbMap.insert (std::pair<uint32_t, NrSlDrbMapPerLcId> (slRxDrb->m_sourceL2Id, mapPerRxLcId));
    }
  else
    {
      NS_FATAL_ERROR ("Currently supporting only one SL RX DRB per pair of source/destination");
      // once we support multiple SL DRB per pair of source/destination
      // above assert can be removed, and following code will be used
      NrSlDrbMapPerLcId::iterator rxLcIt;
      rxLcIt = srcIt->second.find (slRxDrb->m_logicalChannelIdentity);
      if (rxLcIt == srcIt->second.end ())
        {
          //New Rx bearer for the remote UE
          srcIt->second.insert (std::pair<uint8_t, Ptr<NrSlDataRadioBearerInfo> > (slRxDrb->m_logicalChannelIdentity, slRxDrb));
        }
      else
        {
          NS_FATAL_ERROR ("SL RX DRB with LC id = " << +slRxDrb->m_logicalChannelIdentity << " already exists");
        }
    }
}

Ptr<NrSlDataRadioBearerInfo>
NrSlUeRrc::GetSidelinkDataRadioBearer (uint32_t srcL2Id, uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this);
  Ptr<NrSlDataRadioBearerInfo> slrb = nullptr;
  NrSlDrbMapPerL2Id::iterator destIt = m_slDrbMap.find (dstL2Id);
  NS_ASSERT_MSG (destIt != m_slDrbMap.end (), "Unable to find DRB for destination L2 Id " << dstL2Id);
  NS_LOG_LOGIC ("Searching SL DRB " << srcL2Id << " -> " << dstL2Id);
  // Since we do not support multiple bearers for a single destination,
  // the size of the LC map should be equal to 1, thus, we can just return
  // the NrSlDataRadioBearerInfo of the LC for the destination.
  // In future, when we overcome this limitation this method would have an
  // extra parameter of LC id.
  NS_ASSERT_MSG (destIt->second.size () == 1, "There should be only one LC per destination");
  return destIt->second.begin ()->second;
}

Ptr<NrSlDataRadioBearerInfo>
NrSlUeRrc::DoGetSidelinkDataRadioBearer (uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this);
  return GetSidelinkDataRadioBearer (m_srcL2Id, dstL2Id);
}

uint8_t
NrSlUeRrc::DoGetNextLcid (uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this);
  //Note: This function supports the fact that multiple bearers can exist between
  //a source and destination. However, the rest of the code currently work
  //with only one LC per destination.

  //find unused the LCID
  uint8_t lcid = 0; //initialize with invalid value

  NrSlDrbMapPerL2Id::iterator destIt = m_slDrbMap.find (dstL2Id);
  if (destIt == m_slDrbMap.end ())
    {
      //first time creating a LC for this destination
      lcid = 4;
    }
  else
    {
      // if the size of the LC id per DRB map is equal to
      // the maximum allowed LCIDs, we halt!
      if (destIt->second.size () == 16)
        {
          NS_FATAL_ERROR ("All the 16 LC ids are allocated");
        }
      // find an id not being used
      for (uint8_t lcidTmp = 4; lcidTmp < 20; lcidTmp++)
        {
          NrSlDrbMapPerLcId::iterator lcIt;
          lcIt = destIt->second.find (lcidTmp);
          if (lcIt != destIt->second.end ())
            {
              continue;
            }
          else
            {
              lcid = lcidTmp;
              break; //avoid increasing lcid
            }
        }
    }
  NS_ASSERT (lcid != 0);
  return lcid;
}

std::ostream &
operator<< (std::ostream &os, const NrSlUeRrc::LteNrTddSlotType &item)
{
  switch (item)
    {
    case NrSlUeRrc::LteNrTddSlotType::DL:
      os << "DL";
      break;
    case NrSlUeRrc::LteNrTddSlotType::F:
      os << "F";
      break;
    case NrSlUeRrc::LteNrTddSlotType::S:
      os << "S";
      break;
    case NrSlUeRrc::LteNrTddSlotType::UL:
      os << "UL";
      break;
    }
  return os;
}


} // namespace ns3
