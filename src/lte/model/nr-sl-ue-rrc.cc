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
NrSlUeRrc::SetNrSlPreconfiguration (const LteRrcSap::SidelinkPreconfigNr &preconfiguration)
{
  NS_LOG_FUNCTION (this);
  m_preconfiguration = preconfiguration;
  SetTddPattern ();
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
NrSlUeRrc::DoGetNrSlPreconfiguration () const
{
  NS_LOG_FUNCTION (this);
  return m_preconfiguration;
}

const std::vector <std::bitset<1>>
NrSlUeRrc::GetPhysicalSlPool (const std::vector <std::bitset<1>> &slBitMap)
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
