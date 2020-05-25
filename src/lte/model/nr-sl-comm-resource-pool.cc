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
NrSlCommResourcePool::SetSlPreConfigFreqInfoList (const std::array <LteRrcSap::SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL> &slPreconfigFreqInfoList)
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
NrSlCommResourcePool::SetPhysicalSlPoolMap (NrSlCommResourcePool::PhySlPoolMap phySlPoolMap)
{
  NS_LOG_FUNCTION (this);
  m_phySlPoolMap = phySlPoolMap;
}

const std::vector <std::bitset<1>>
NrSlCommResourcePool::GetPhySlPool (uint16_t bwpId, uint16_t poolId)
{
  NS_LOG_FUNCTION (this);
  NrSlCommResourcePool::PhySlPoolMap::iterator itBwp = m_phySlPoolMap.find (bwpId);
  NS_ABORT_MSG_IF (itBwp == m_phySlPoolMap.end (), "Unable to find BWP id " << bwpId);
  std::unordered_map <uint16_t, std::vector <std::bitset<1>>>::iterator itPool = itBwp->second.find (poolId);
  NS_ABORT_MSG_IF (itPool == itBwp->second.end (), "Unable to find pool id " << poolId);
  return itPool->second;
}

const LteRrcSap::SlResourcePoolNr
NrSlCommResourcePool::GetSlResourcePoolNr (uint16_t bwpId, uint16_t poolId)
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

std::unordered_map <uint16_t, std::vector <std::bitset<1>>>::const_iterator
NrSlCommResourcePool::GetIteratorToPhySlPool (uint16_t bwpId, uint16_t poolId)
{
  NS_LOG_FUNCTION (this);
  NrSlCommResourcePool::PhySlPoolMap::iterator itBwp = m_phySlPoolMap.find (bwpId);
  NS_ABORT_MSG_IF (itBwp == m_phySlPoolMap.end (), "Unable to find bandwidth part id " << bwpId);
  std::unordered_map <uint16_t, std::vector <std::bitset<1>>>::const_iterator itPool = itBwp->second.find (poolId);
  NS_ABORT_MSG_IF (itPool == itBwp->second.end (), "Unable to find pool id " << poolId);
  return itPool;
}

std::list <NrSlCommResourcePool::SlotInfo>
NrSlCommResourcePool::GetSlCommOpportunities (uint16_t absIndexCurretSlot, uint16_t bwpId, uint16_t poolId, uint16_t t1)
{
  NS_LOG_FUNCTION (this);
  std::unordered_map <uint16_t, std::vector <std::bitset<1>>>::const_iterator itPhyPool;
  itPhyPool = GetIteratorToPhySlPool (bwpId, poolId);
  LteRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at (0);
  LteRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at (bwpId);
  uint16_t totalSlSymbols = LteRrcSap::GetSlLengthSymbolsValue (slBwpConfigCommon.slBwpGeneric.slLengthSymbols);
  uint16_t slSymbolStart =  LteRrcSap::GetSlStartSymbolValue(slBwpConfigCommon.slBwpGeneric.slStartSymbol);

  const LteRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr (bwpId, poolId);
  uint16_t t2 = LteRrcSap::GetSlSelWindowValue (pool.slUeSelectedConfigRp.slSelectionWindow);
  uint16_t firstAbsSlotIndex = absIndexCurretSlot + t1;
  uint16_t lastAbsSlotIndex =  absIndexCurretSlot + t2;

  NS_LOG_DEBUG ("Starting absolute slot number of the selection window = " << firstAbsSlotIndex);
  NS_LOG_DEBUG ("Last absolute slot number of the selection window =  " << lastAbsSlotIndex);
  NS_LOG_DEBUG ("Final selection Window Length = " << lastAbsSlotIndex - firstAbsSlotIndex);

  std::list <NrSlCommResourcePool::SlotInfo> list;
  uint16_t absPoolIndex = firstAbsSlotIndex % itPhyPool->second.size ();
  NS_LOG_DEBUG ("Absolute pool index = " << absPoolIndex);
  for (uint16_t i = firstAbsSlotIndex; i < lastAbsSlotIndex; ++i)
    {

      if (itPhyPool->second [absPoolIndex] == 1)
        {
          NrSlCommResourcePool::SlotInfo info;
          //PSCCH
          info.numSlPscchRbs = LteRrcSap::GetSlFResoPscchValue (pool.slPscchConfig.slFreqResourcePscch);
          info.slPscchSymStart = slSymbolStart;
          info.slPscchSymlength = LteRrcSap::GetSlTResoPscchValue (pool.slPscchConfig.slTimeResourcePscch);
          //PSSCH
          info.slSubchannelSize = LteRrcSap::GetSlSubChSizeValue (pool.slSubchannelSize);
          info.slPsschSymStart = info.slPscchSymStart + info.slPscchSymlength;
          info.slPsschSymlength = (totalSlSymbols - info.slPscchSymlength) - 1;
          info.absSlotIndex = i;
          list.emplace_back (info);
        }
      ++absPoolIndex;
    }

  return list;
}

uint16_t
NrSlCommResourcePool::GetNumSlotsSensWind (uint16_t bwpId, uint16_t poolId, Time slotLength)
{
NS_LOG_FUNCTION (this << bwpId << poolId << slotLength);

NrSlCommResourcePool::PhySlPoolMap::iterator itBwp = m_phySlPoolMap.find (bwpId);
NS_ASSERT_MSG (itBwp != m_phySlPoolMap.end (), "Unable to find physical pool for bandwidth part id " << bwpId);
//If there is no physical pool set for the given BWP id, avoid accessing
//the slBwpList array index.

LteRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at (0);
LteRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at (bwpId);
LteRrcSap::SlResourcePoolConfigNr slResourcePoolConfig = slBwpConfigCommon.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal.at (poolId);
LteRrcSap::SlSensingWindow slSensingWindowLen = slResourcePoolConfig.slResourcePool.slUeSelectedConfigRp.slSensingWindow;
NS_ASSERT_MSG (slSensingWindowLen.windSize != LteRrcSap::SlSensingWindow::INVALID, "Sensing window not set for BWP id " << bwpId << " pool id " << poolId);

uint16_t windLenInMs = LteRrcSap::GetSlSensWindowValue (slSensingWindowLen);

double numSlots = (windLenInMs / static_cast <double>(1000)) / slotLength.GetSeconds ();

return static_cast <uint16_t> (numSlots);
}


}


