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

#include "nr-sl-comm-resource-pool.h"
#include "ns3/log.h"
#include <math.h>
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrSlCommResourcePool");
NS_OBJECT_ENSURE_REGISTERED (NrSlCommResourcePool);

TypeId
NrSlCommResourcePool::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::NrSlCommResourcePool")
    .SetParent<Object> ()
    .SetGroupName("lte")
    .AddConstructor<NrSlCommResourcePool> ()
    ;
  return tid;
}

NrSlCommResourcePool::NrSlCommResourcePool ()
{
  NS_LOG_FUNCTION (this);
}
NrSlCommResourcePool::~NrSlCommResourcePool ()
{
  NS_LOG_FUNCTION (this);
}

void
NrSlCommResourcePool::SetNrSlPreConfigFreqInfoList (const std::array <LteRrcSap::SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL> &slPreconfigFreqInfoList)
{
  NS_LOG_FUNCTION (this);
  m_slPreconfigFreqInfoList = slPreconfigFreqInfoList;
}

bool
NrSlCommResourcePool::operator== (const NrSlCommResourcePool& other) const
{
  //std::array <LteRrcSap::SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL> m_slPreconfigFreqInfoList
  //if Physical SL pool is equal that means SL Bitmap and TDD pattern are
  //also equal.
  bool equal = m_phySlPoolMap == other.m_phySlPoolMap;
  if (equal == false)
    {
      return equal;
    }
  else
    {
      NS_ASSERT_MSG (m_phySlPoolMap.begin ()->first == other.m_phySlPoolMap.begin ()->first, "BWP id mismatched");
      uint8_t bwpId = m_phySlPoolMap.begin ()->first;
      LteRrcSap::SlFreqConfigCommonNr slfreqConfigLocal = m_slPreconfigFreqInfoList.at (0);
      LteRrcSap::SlFreqConfigCommonNr slfreqConfigOther = other.m_slPreconfigFreqInfoList.at (0);

      LteRrcSap::SlBwpConfigCommonNr slBwpConfigLocal = slfreqConfigLocal.slBwpList.at (bwpId);
      LteRrcSap::SlBwpConfigCommonNr slBwpConfigOther = slfreqConfigOther.slBwpList.at (bwpId);


      std::array <LteRrcSap::SlResourcePoolConfigNr, MAX_NUM_OF_TX_POOL> slTxPoolLocal = slBwpConfigLocal.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal;
      std::array <LteRrcSap::SlResourcePoolConfigNr, MAX_NUM_OF_TX_POOL> slTxPoolOther = slBwpConfigOther.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal;

      for (uint16_t poolIndex = 0; poolIndex < slTxPoolLocal.size (); ++poolIndex)
        {
          equal = slTxPoolLocal.at (poolIndex).haveSlResourcePoolConfigNr == slTxPoolOther.at (poolIndex).haveSlResourcePoolConfigNr
               && slTxPoolLocal.at (poolIndex).slResourcePoolId.id == slTxPoolOther.at (poolIndex).slResourcePoolId.id
               && slTxPoolLocal.at (poolIndex).slResourcePool.slPscchConfig.setupRelease == slTxPoolOther.at (poolIndex).slResourcePool.slPscchConfig.setupRelease
               && slTxPoolLocal.at (poolIndex).slResourcePool.slPscchConfig.slFreqResourcePscch.resources == slTxPoolOther.at (poolIndex).slResourcePool.slPscchConfig.slFreqResourcePscch.resources
               && slTxPoolLocal.at (poolIndex).slResourcePool.slPscchConfig.slTimeResourcePscch.resources == slTxPoolOther.at (poolIndex).slResourcePool.slPscchConfig.slTimeResourcePscch.resources
               && slTxPoolLocal.at (poolIndex).slResourcePool.slSubchannelSize.numPrbs == slTxPoolOther.at (poolIndex).slResourcePool.slSubchannelSize.numPrbs
               && slTxPoolLocal.at (poolIndex).slResourcePool.slTimeResource == slTxPoolOther.at (poolIndex).slResourcePool.slTimeResource
               && slTxPoolLocal.at (poolIndex).slResourcePool.slUeSelectedConfigRp.slSelectionWindow.windSize == slTxPoolOther.at (poolIndex).slResourcePool.slUeSelectedConfigRp.slSelectionWindow.windSize
               && slTxPoolLocal.at (poolIndex).slResourcePool.slUeSelectedConfigRp.slSensingWindow.windSize == slTxPoolOther.at (poolIndex).slResourcePool.slUeSelectedConfigRp.slSensingWindow.windSize
               && slTxPoolLocal.at (poolIndex).slResourcePool.slUeSelectedConfigRp.slMultiReserveResource == slTxPoolOther.at (poolIndex).slResourcePool.slUeSelectedConfigRp.slMultiReserveResource;

          std::array <LteRrcSap::SlResourceReservePeriod, 16> listLocal = slTxPoolLocal.at (poolIndex).slResourcePool.slUeSelectedConfigRp.slResourceReservePeriodList;
          std::array <LteRrcSap::SlResourceReservePeriod, 16> listOther = slTxPoolOther.at (poolIndex).slResourcePool.slUeSelectedConfigRp.slResourceReservePeriodList;
          bool listEquality = false;
          for (uint16_t resPListIndex = 0; resPListIndex < listLocal.size (); ++resPListIndex)
            {
              listEquality = listLocal.at (resPListIndex).period == listOther.at (resPListIndex).period;
            }

          equal = equal && listEquality;

          if (equal == false)
            {
              break;
            }
        }

      return equal;
    }
}

void
NrSlCommResourcePool::SetNrSlPhysicalPoolMap (NrSlCommResourcePool::PhySlPoolMap phySlPoolMap)
{
  NS_LOG_FUNCTION (this);
  m_phySlPoolMap = phySlPoolMap;
}

const std::vector <std::bitset<1>>
NrSlCommResourcePool::GetNrSlPhyPool (uint8_t bwpId, uint16_t poolId) const
{
  NS_LOG_FUNCTION (this);
  NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId (bwpId, poolId);
  return ret.itPool->second;
}

const LteRrcSap::SlResourcePoolNr
NrSlCommResourcePool::GetSlResourcePoolNr (uint8_t bwpId, uint16_t poolId) const
{
  NS_LOG_FUNCTION (this);
  LteRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at (0);
  LteRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at (bwpId);
  std::array <LteRrcSap::SlResourcePoolConfigNr, MAX_NUM_OF_TX_POOL> slTxPoolSelectedNormal = slBwpConfigCommon.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal;
  LteRrcSap::SlResourcePoolNr pool;
  bool found = false;
  for (const auto& it: slTxPoolSelectedNormal)
    {
      if (it.slResourcePoolId.id == poolId)
        {
          found = true;
          pool = it.slResourcePool;
          break;
        }
    }
  NS_ASSERT_MSG (found == true, "unable to find pool id " << poolId);
  return pool;
}

std::list <NrSlCommResourcePool::SlotInfo>
NrSlCommResourcePool::GetNrSlCommOpportunities (uint64_t absIndexCurretSlot, uint8_t bwpId, uint16_t numerology, uint16_t poolId, uint8_t t1, uint16_t t2) const
{
  NS_LOG_FUNCTION (this);
  std::vector <std::bitset<1>> phyPool = GetNrSlPhyPool (bwpId, poolId);
  const LteRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr (bwpId, poolId);

  LteRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at (0);
  LteRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at (bwpId);
  uint16_t totalSlSymbols = LteRrcSap::GetSlLengthSymbolsValue (slBwpConfigCommon.slBwpGeneric.slLengthSymbols);
  uint16_t slSymbolStart =  LteRrcSap::GetSlStartSymbolValue(slBwpConfigCommon.slBwpGeneric.slStartSymbol);


  //t2_min as a function of numerology. Discussed in 3GPP meeting R1-2003807
  //also in TS 38.331 in SL-UE-SelectedConfigRP field descriptions
  uint16_t t2min = LteRrcSap::GetSlSelWindowValue (pool.slUeSelectedConfigRp.slSelectionWindow);
  uint16_t multiplier = static_cast <uint16_t> (std::pow (2, numerology));
  t2min = t2min * multiplier;
  NS_ABORT_MSG_IF (t2min > t2, "T2min(" << t2min << ")" << " should be less than or equal to T2(" << t2 << ") in physical slots");
  //38.214 sec 8.1.4 says, if T2min is shorter than the remaining packet delay
  //budget (in slots) then T2 is up to UE implementation subject to
  //T2min ≤ T 2 ≤ remaining packet budget (in slots); otherwise T 2 is set
  //to the remaining packet delay budget (in slots).
  //In the current implementation, we do not consider packet budget, thus, we
  //take t2 as it is without checking if it is lower than packet budget or not
  uint16_t t2Final = t2;

  uint64_t firstAbsSlotIndex = absIndexCurretSlot + t1;
  uint64_t lastAbsSlotIndex =  absIndexCurretSlot + t2Final;

  NS_LOG_DEBUG ("Starting absolute slot number of the selection window = " << firstAbsSlotIndex);
  NS_LOG_DEBUG ("Last absolute slot number of the selection window =  " << lastAbsSlotIndex);
  NS_LOG_DEBUG ("Final selection Window Length in physical slots = " << (lastAbsSlotIndex - firstAbsSlotIndex) + 1);

  std::list <NrSlCommResourcePool::SlotInfo> list;
  uint16_t absPoolIndex = firstAbsSlotIndex % phyPool.size ();
  NS_LOG_DEBUG ("Absolute pool index = " << absPoolIndex);

  for (uint64_t i = firstAbsSlotIndex; i <= lastAbsSlotIndex; ++i)
    {
      if (phyPool [absPoolIndex] == 1)
        {
          //PSCCH
          uint16_t numSlPscchRbs = LteRrcSap::GetSlFResoPscchValue (pool.slPscchConfig.slFreqResourcePscch);
          uint16_t slPscchSymStart = slSymbolStart;
          uint16_t slPscchSymLength = LteRrcSap::GetSlTResoPscchValue (pool.slPscchConfig.slTimeResourcePscch);
          //PSSCH
          uint16_t slPsschSymStart = slPscchSymStart + slPscchSymLength;
          uint16_t slPsschSymLength = (totalSlSymbols - slPscchSymLength) - 1;
          uint16_t slSubchannelSize = LteRrcSap::GetNrSlSubChSizeValue (pool.slSubchannelSize);
          uint16_t slMaxNumPerReserve = LteRrcSap::GetSlMaxNumPerReserveValue (pool.slUeSelectedConfigRp.slMaxNumPerReserve);
          uint64_t absSlotIndex = i;
          uint32_t slotOffset = static_cast <uint32_t> (i - absIndexCurretSlot);

          NrSlCommResourcePool::SlotInfo info (numSlPscchRbs, slPscchSymStart,
                                               slPscchSymLength, slPsschSymStart,
                                               slPsschSymLength, slSubchannelSize,
                                               slMaxNumPerReserve, absSlotIndex, slotOffset);
          list.emplace_back (info);
        }
      absPoolIndex = (absPoolIndex + 1) % phyPool.size ();
    }

  NS_LOG_DEBUG ("Total number of slots available for Sidelink in the selection window = " << list.size ());

  for (const auto & it : list)
    {
      NS_LOG_DEBUG ("Absolute slot number of the Sidelink slot in the selection window = " << it.slotOffset + absIndexCurretSlot);
    }

  return list;
}

uint16_t
NrSlCommResourcePool::GetNrSlSensWindInSlots (uint8_t bwpId, uint16_t poolId, Time slotLength) const
{
  NS_LOG_FUNCTION (this << +bwpId << poolId << slotLength);

  [[maybe_unused]] NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId (bwpId, poolId);
  LteRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at (0);
  LteRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at (bwpId);
  LteRrcSap::SlResourcePoolConfigNr slResourcePoolConfig = slBwpConfigCommon.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal.at (poolId);
  LteRrcSap::SlSensingWindow slSensingWindowLen = slResourcePoolConfig.slResourcePool.slUeSelectedConfigRp.slSensingWindow;
  NS_ASSERT_MSG (slSensingWindowLen.windSize != LteRrcSap::SlSensingWindow::INVALID, "Sensing window not set for BWP id " << +bwpId << " pool id " << poolId);

  uint16_t windLenInMs = LteRrcSap::GetSlSensWindowValue (slSensingWindowLen);

  double numSlots = (windLenInMs / static_cast <double>(1000)) / slotLength.GetSeconds ();

  return static_cast <uint16_t> (numSlots);
}

void
NrSlCommResourcePool::SetNrSlSchedulingType (NrSlCommResourcePool::SchedulingType type)
{
  NS_LOG_FUNCTION (this << type);
  m_schType = type;
}

NrSlCommResourcePool::SchedulingType
NrSlCommResourcePool::GetNrSlSchedulingType () const
{
  return m_schType;
}

uint16_t
NrSlCommResourcePool::GetNrSlSubChSize (uint8_t bwpId, uint16_t poolId) const
{
  NS_LOG_FUNCTION (this << +bwpId << poolId);

  [[maybe_unused]] NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId (bwpId, poolId);
  LteRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at (0);
  LteRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at (bwpId);
  LteRrcSap::SlResourcePoolConfigNr slResourcePoolConfig = slBwpConfigCommon.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal.at (poolId);
  LteRrcSap::SlSubchannelSize slSubChSize = slResourcePoolConfig.slResourcePool.slSubchannelSize;
  NS_ASSERT_MSG (slSubChSize.numPrbs != LteRrcSap::SlSubchannelSize::INVALID, "Subchannel is not set for BWP id " << +bwpId << " pool id " << poolId);

  uint16_t slSubChSizeInt = LteRrcSap::GetNrSlSubChSizeValue (slSubChSize);

  return slSubChSizeInt;
}

NrSlCommResourcePool::BwpAndPoolIt
NrSlCommResourcePool::ValidateBwpAndPoolId (uint8_t bwpId, uint16_t poolId) const
{
  NS_LOG_FUNCTION (this << +bwpId << poolId);
  NrSlCommResourcePool::PhySlPoolMap::const_iterator itBwp = m_phySlPoolMap.find (bwpId);
  NS_ABORT_MSG_IF (itBwp == m_phySlPoolMap.end (), "Unable to find bandwidth part id " << +bwpId);
  std::unordered_map <uint16_t, std::vector <std::bitset<1>>>::const_iterator itPool = itBwp->second.find (poolId);
  NS_ABORT_MSG_IF (itPool == itBwp->second.end (), "Unable to find pool id " << poolId);

  BwpAndPoolIt ret;
  ret.itBwp = itBwp;
  ret.itPool = itPool;
  return ret;
}

void
NrSlCommResourcePool::ValidateResvPeriod (uint8_t bwpId, uint16_t poolId, Time resvPeriod, Time slotLength) const
{
  NS_LOG_FUNCTION (this << +bwpId << poolId << resvPeriod.GetMilliSeconds ());
  [[maybe_unused]] NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId (bwpId, poolId);
  const LteRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr (bwpId, poolId);
  std::array <LteRrcSap::SlResourceReservePeriod, 16> resvPeriodList = pool.slUeSelectedConfigRp.slResourceReservePeriodList;
  uint16_t periodInt = static_cast <uint16_t> (resvPeriod.GetMilliSeconds ());
  LteRrcSap::SlResourceReservePeriod resvPeriodEnum =  LteRrcSap::GetSlResoResvPrdEnum (periodInt);
  bool found = false;
  for (const auto& it:resvPeriodList)
    {
      if (it.period == resvPeriodEnum.period)
        {
          found = true;
          uint16_t rsvpInSlots = GetResvPeriodInSlots (bwpId, poolId, resvPeriod, slotLength);
          IsRsvpMultipOfPoolLen (bwpId, poolId, rsvpInSlots);
        }
    }
  NS_ABORT_MSG_IF (!found, "The given reservation period is not in the user specified list");
}

uint16_t
NrSlCommResourcePool::GetResvPeriodInSlots (uint8_t bwpId, uint16_t poolId, Time resvPeriod, Time slotLength) const
{
  NS_LOG_FUNCTION (this << +bwpId << poolId << resvPeriod.GetMilliSeconds () << slotLength);
  [[maybe_unused]] NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId (bwpId, poolId);
  uint16_t periodInt = static_cast <uint16_t> (resvPeriod.GetMilliSeconds ());

  double numResvSlots = (periodInt / static_cast <double>(1000)) / slotLength.GetSeconds ();

  uint16_t rsvpInSlots = static_cast <uint16_t> (numResvSlots);
  IsRsvpMultipOfPoolLen (bwpId, poolId, rsvpInSlots);
  return rsvpInSlots;
}

bool
NrSlCommResourcePool::IsSidelinkSlot (uint8_t bwpId, uint16_t poolId, uint64_t absSlotIndex) const
{
  NS_LOG_FUNCTION (this << +bwpId << poolId << absSlotIndex);

  uint16_t absPoolIndex = GetAbsPoolIndex (bwpId, poolId, absSlotIndex);
  std::vector <std::bitset<1>> phyPool = GetNrSlPhyPool (bwpId, poolId);
  //trigger SL only when it is a SL slot
  if (phyPool.at (absPoolIndex) == 1)
    {
      return true;
    }

  return false;
}

uint16_t
NrSlCommResourcePool::GetAbsPoolIndex (uint8_t bwpId, uint16_t poolId, uint64_t absSlotIndex) const
{
  NS_LOG_FUNCTION (this << +bwpId << poolId << absSlotIndex);
  NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId (bwpId, poolId);
  std::vector <std::bitset<1>> phyPool = ret.itPool->second;
  uint16_t absPoolIndex = absSlotIndex % phyPool.size ();
  return absPoolIndex;
}

void
NrSlCommResourcePool::SetTddPattern (std::vector<NrSlUeRrc::LteNrTddSlotType> tddPattern)
{
  NS_LOG_FUNCTION (this);
  m_tddPattern = tddPattern;
}

uint16_t
NrSlCommResourcePool::GetSlSubChSize (uint8_t bwpId, uint16_t poolId) const
{
  NS_LOG_FUNCTION (this << +bwpId << poolId);
  [[maybe_unused]] NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId (bwpId, poolId);
  const LteRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr (bwpId, poolId);
  return LteRrcSap::GetNrSlSubChSizeValue (pool.slSubchannelSize);
}

void
NrSlCommResourcePool::IsRsvpMultipOfPoolLen (uint8_t bwpId, uint16_t poolId, uint16_t rsvpInSlots) const
{
  NS_LOG_FUNCTION (this << +bwpId << poolId << rsvpInSlots);
  std::vector <std::bitset<1>> phyPool = GetNrSlPhyPool (bwpId, poolId);
  NS_ABORT_MSG_IF (rsvpInSlots % phyPool.size () != 0, "Resource reservation period in slots "
                   << rsvpInSlots << " should be multiple of physical sidelink pool length of "
                   << phyPool.size ());
}

}


