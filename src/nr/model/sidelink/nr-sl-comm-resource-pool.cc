/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   SPDX-License-Identifier: GPL-2.0-only
 *
 *
 *
 */

#include "nr-sl-comm-resource-pool.h"

#include "ns3/log.h"
#include "ns3/nr-mcs-tables.h"

#include <algorithm>
#include <math.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlCommResourcePool");
NS_OBJECT_ENSURE_REGISTERED(NrSlCommResourcePool);

TypeId
NrSlCommResourcePool::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlCommResourcePool")
                            .SetParent<Object>()
                            .SetGroupName("lte")
                            .AddConstructor<NrSlCommResourcePool>();
    return tid;
}

NrSlCommResourcePool::NrSlCommResourcePool()
{
    NS_LOG_FUNCTION(this);
}

NrSlCommResourcePool::~NrSlCommResourcePool()
{
    NS_LOG_FUNCTION(this);
}

void
NrSlCommResourcePool::SetNrSlPreConfigFreqInfoList(
    const std::array<NrSlRrcSap::SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL>& slPreconfigFreqInfoList)
{
    NS_LOG_FUNCTION(this);
    m_slPreconfigFreqInfoList = slPreconfigFreqInfoList;
}

bool
NrSlCommResourcePool::operator==(const NrSlCommResourcePool& other) const
{
    // std::array <NrSlRrcSap::SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL> m_slPreconfigFreqInfoList
    // if Physical SL pool is equal that means SL Bitmap and TDD pattern are
    // also equal.
    bool equal = m_phySlPoolMap == other.m_phySlPoolMap;
    if (!equal)
    {
        return equal;
    }
    else
    {
        NS_ASSERT_MSG(m_phySlPoolMap.begin()->first == other.m_phySlPoolMap.begin()->first,
                      "BWP id mismatched");
        uint8_t bwpId = m_phySlPoolMap.begin()->first;
        NrSlRrcSap::SlFreqConfigCommonNr slfreqConfigLocal = m_slPreconfigFreqInfoList.at(0);
        NrSlRrcSap::SlFreqConfigCommonNr slfreqConfigOther = other.m_slPreconfigFreqInfoList.at(0);

        NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigLocal = slfreqConfigLocal.slBwpList.at(bwpId);
        NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigOther = slfreqConfigOther.slBwpList.at(bwpId);

        std::array<NrSlRrcSap::SlResourcePoolConfigNr, MAX_NUM_OF_TX_POOL> slTxPoolLocal =
            slBwpConfigLocal.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal;
        std::array<NrSlRrcSap::SlResourcePoolConfigNr, MAX_NUM_OF_TX_POOL> slTxPoolOther =
            slBwpConfigOther.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal;

        for (uint32_t poolIndex = 0; poolIndex < slTxPoolLocal.size(); ++poolIndex)
        {
            equal = slTxPoolLocal.at(poolIndex).haveSlResourcePoolConfigNr ==
                        slTxPoolOther.at(poolIndex).haveSlResourcePoolConfigNr &&
                    slTxPoolLocal.at(poolIndex).slResourcePoolId.id ==
                        slTxPoolOther.at(poolIndex).slResourcePoolId.id &&
                    slTxPoolLocal.at(poolIndex).slResourcePool.slPscchConfig.setupRelease ==
                        slTxPoolOther.at(poolIndex).slResourcePool.slPscchConfig.setupRelease &&
                    slTxPoolLocal.at(poolIndex)
                            .slResourcePool.slPscchConfig.slFreqResourcePscch.resources ==
                        slTxPoolOther.at(poolIndex)
                            .slResourcePool.slPscchConfig.slFreqResourcePscch.resources &&
                    slTxPoolLocal.at(poolIndex)
                            .slResourcePool.slPscchConfig.slTimeResourcePscch.resources ==
                        slTxPoolOther.at(poolIndex)
                            .slResourcePool.slPscchConfig.slTimeResourcePscch.resources &&
                    slTxPoolLocal.at(poolIndex).slResourcePool.slSubchannelSize.numPrbs ==
                        slTxPoolOther.at(poolIndex).slResourcePool.slSubchannelSize.numPrbs &&
                    slTxPoolLocal.at(poolIndex).slResourcePool.slTimeResource ==
                        slTxPoolOther.at(poolIndex).slResourcePool.slTimeResource &&
                    slTxPoolLocal.at(poolIndex)
                            .slResourcePool.slUeSelectedConfigRp.slSelectionWindow.windSize ==
                        slTxPoolOther.at(poolIndex)
                            .slResourcePool.slUeSelectedConfigRp.slSelectionWindow.windSize &&
                    slTxPoolLocal.at(poolIndex)
                            .slResourcePool.slUeSelectedConfigRp.slSensingWindow.windSize ==
                        slTxPoolOther.at(poolIndex)
                            .slResourcePool.slUeSelectedConfigRp.slSensingWindow.windSize &&
                    slTxPoolLocal.at(poolIndex)
                            .slResourcePool.slUeSelectedConfigRp.slMultiReserveResource ==
                        slTxPoolOther.at(poolIndex)
                            .slResourcePool.slUeSelectedConfigRp.slMultiReserveResource;

            std::list<NrSlRrcSap::SlResourceReservePeriod> listLocal =
                slTxPoolLocal.at(poolIndex)
                    .slResourcePool.slUeSelectedConfigRp.slResourceReservePeriodList;
            std::list<NrSlRrcSap::SlResourceReservePeriod> listOther =
                slTxPoolOther.at(poolIndex)
                    .slResourcePool.slUeSelectedConfigRp.slResourceReservePeriodList;
            bool listEquality = true;
            auto localIt = listLocal.cbegin();
            auto otherIt = listOther.cbegin();
            while (localIt != listLocal.cend() && otherIt != listOther.cend())
            {
                if ((*localIt).period == (*otherIt).period)
                {
                    listEquality = false;
                    break;
                }
                localIt++;
                otherIt++;
            }

            equal = equal && listEquality;

            if (!equal)
            {
                break;
            }
        }

        return equal;
    }
}

void
NrSlCommResourcePool::SetNrSlPhysicalPoolMap(NrSlCommResourcePool::PhySlPoolMap phySlPoolMap)
{
    NS_LOG_FUNCTION(this);
    m_phySlPoolMap = phySlPoolMap;
}

const std::vector<std::bitset<1>>
NrSlCommResourcePool::GetNrSlPhyPool(uint8_t bwpId, uint16_t poolId) const
{
    NS_LOG_FUNCTION(this);
    NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId(bwpId, poolId);
    return ret.itPool->second;
}

const NrSlRrcSap::SlResourcePoolNr
NrSlCommResourcePool::GetSlResourcePoolNr(uint8_t bwpId, uint16_t poolId) const
{
    NS_LOG_FUNCTION(this);
    NrSlRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at(0);
    NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at(bwpId);
    std::array<NrSlRrcSap::SlResourcePoolConfigNr, MAX_NUM_OF_TX_POOL> slTxPoolSelectedNormal =
        slBwpConfigCommon.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal;
    NrSlRrcSap::SlResourcePoolNr pool;
    bool found = false;
    for (const auto& it : slTxPoolSelectedNormal)
    {
        if (it.slResourcePoolId.id == poolId)
        {
            found = true;
            pool = it.slResourcePool;
            break;
        }
    }
    NS_ASSERT_MSG(found == true, "unable to find pool id " << poolId);
    return pool;
}

bool
NrSlCommResourcePool::SlotHasPsfch(uint64_t absIndexCurrentSlot,
                                   uint8_t bwpId,
                                   uint16_t poolId) const
{
    std::vector<std::bitset<1>> phyPool = GetNrSlPhyPool(bwpId, poolId);
    NrSlRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr(bwpId, poolId);
    uint8_t psfchPeriod = NrSlRrcSap::GetSlPsfchPeriodValue(pool.slPsfchConfig.slPsfchPeriod);
    return SlotHasPsfch(absIndexCurrentSlot, phyPool, psfchPeriod);
}

bool
NrSlCommResourcePool::SlotHasPsfch(uint64_t absIndexCurrentSlot,
                                   std::vector<std::bitset<1>>& phyPool,
                                   uint8_t psfchPeriod) const
{
    if (!psfchPeriod)
    {
        return false;
    }

    // Determine number of SL slots from absIndexCurrentSlot, and if the slot
    // is a SL slot, we should return true if the number of SL slots is a
    // multiple of PsfchPeriod slots.
    // The size of phyPool (calculated elsewhere) is large enough to repeat the
    // SL pattern (i.e., the overall SL pattern is repeated each phyPool slots).
    // The PSFCH pattern repeats at least every (psfchPeriod * phyPool.size ())
    // slots (i.e., psfchPeriod * phyPool.size () is the modulus).  We'll
    // call this modulus value the 'period' below.
    uint64_t numSlSlots = 0; // number of SL slots before absIndexCurrentSlot
    uint16_t period = psfchPeriod * phyPool.size();
    // The number of periods before the current period is
    // absIndexCurrentSlot / modulus.  We do not need to count SL slots in
    // these earlier periods because the number of SL slots will always be
    // a multiple of psfchPeriod.  We only need to look at the remainder
    // (modulus) of the division absIndexCurrentSlot / period;
    uint64_t numSlotsIntoCurrentPeriod = absIndexCurrentSlot % period;
    bool reachedLimit = false; // Used to break out of outer for loop
    bool currentSlotIsSlSlot = false;
    for (uint16_t i = 0; i < psfchPeriod && !reachedLimit; i++)
    {
        for (uint32_t j = 0; j < phyPool.size(); j++)
        {
            numSlSlots += (phyPool[j] == 1 ? 1 : 0);
            currentSlotIsSlSlot = (phyPool[j] == 1);
            if ((i * phyPool.size()) + j == numSlotsIntoCurrentPeriod)
            {
                reachedLimit = true;
                break;
            }
        }
    }
    bool hasPsfch = currentSlotIsSlSlot && ((numSlSlots % psfchPeriod) == 0);
    NS_LOG_DEBUG("NumSlSlots " << numSlSlots << " Absolute slot number " << absIndexCurrentSlot
                               << " hasPsfch: " << hasPsfch);
    return hasPsfch;
}

uint8_t
NrSlCommResourcePool::GetMinTimeGapPsfch(uint8_t bwpId, uint16_t poolId) const
{
    NrSlRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr(bwpId, poolId);
    return NrSlRrcSap::GetSlMinTimeGapPsfchValue(pool.slPsfchConfig.slMinTimeGapPsfch);
}

uint8_t
NrSlCommResourcePool::GetPsfchPeriod(uint8_t bwpId, uint16_t poolId) const
{
    NrSlRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr(bwpId, poolId);
    return NrSlRrcSap::GetSlPsfchPeriodValue(pool.slPsfchConfig.slPsfchPeriod);
}

uint16_t
NrSlCommResourcePool::GetT2Min(uint8_t bwpId, uint16_t poolId, uint16_t numerology) const
{
    std::vector<std::bitset<1>> phyPool = GetNrSlPhyPool(bwpId, poolId);
    const NrSlRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr(bwpId, poolId);
    // t2_min as a function of numerology. Discussed in 3GPP meeting R1-2003807
    // also in TS 38.331 in SL-UE-SelectedConfigRP field descriptions
    uint16_t t2min = NrSlRrcSap::GetSlSelWindowValue(pool.slUeSelectedConfigRp.slSelectionWindow);
    t2min = t2min * static_cast<uint16_t>(std::pow(2, numerology));
    return t2min;
}

std::list<NrSlCommResourcePool::SlotInfo>
NrSlCommResourcePool::GetNrSlCommOpportunities(uint64_t absIndexCurrentSlot,
                                               uint8_t bwpId,
                                               uint16_t numerology,
                                               uint16_t poolId,
                                               uint8_t t1,
                                               uint16_t t2) const
{
    NS_LOG_FUNCTION(this << absIndexCurrentSlot << +bwpId << numerology << poolId << +t1 << t2);
    std::vector<std::bitset<1>> phyPool = GetNrSlPhyPool(bwpId, poolId);
    const NrSlRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr(bwpId, poolId);

    NrSlRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at(0);
    NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at(bwpId);
    uint16_t totalSlSymbols =
        NrSlRrcSap::GetSlLengthSymbolsValue(slBwpConfigCommon.slBwpGeneric.slLengthSymbols);
    uint16_t slSymbolStart =
        NrSlRrcSap::GetSlStartSymbolValue(slBwpConfigCommon.slBwpGeneric.slStartSymbol);

    // t2_min as a function of numerology. Discussed in 3GPP meeting R1-2003807
    // also in TS 38.331 in SL-UE-SelectedConfigRP field descriptions
    uint16_t t2min = NrSlRrcSap::GetSlSelWindowValue(pool.slUeSelectedConfigRp.slSelectionWindow);
    auto multiplier = static_cast<uint16_t>(std::pow(2, numerology));
    t2min = t2min * multiplier;
    NS_ABORT_MSG_IF(t2min > t2,
                    "T2min(" << t2min << ")"
                             << " should be less than or equal to T2(" << t2
                             << ") in physical slots");
    // 38.214 sec 8.1.4 says, if T2min is shorter than the remaining packet delay
    // budget (in slots) then T2 is up to UE implementation subject to
    // T2min <= T2 <= remaining packet budget (in slots); otherwise T2 is set
    // to the remaining packet delay budget (in slots).
    // In the current implementation, we do not consider packet budget, thus, we
    // take t2 as it is without checking if it is lower than packet budget or not
    uint16_t t2Final = t2;

    uint64_t firstAbsSlotIndex = absIndexCurrentSlot + t1;
    uint64_t lastAbsSlotIndex = absIndexCurrentSlot + t2Final;

    NS_LOG_DEBUG("Starting absolute slot number of the selection window = " << firstAbsSlotIndex);
    NS_LOG_DEBUG("Last absolute slot number of the selection window =  " << lastAbsSlotIndex);
    NS_LOG_DEBUG("Final selection Window Length in physical slots = "
                 << (lastAbsSlotIndex - firstAbsSlotIndex) + 1);

    std::list<NrSlCommResourcePool::SlotInfo> list;
    uint16_t absPoolIndex = firstAbsSlotIndex % phyPool.size();
    uint8_t psfchPeriod = NrSlRrcSap::GetSlPsfchPeriodValue(pool.slPsfchConfig.slPsfchPeriod);
    NS_LOG_DEBUG("Absolute pool index = " << absPoolIndex);

    for (uint64_t i = firstAbsSlotIndex; i <= lastAbsSlotIndex; ++i)
    {
        if (phyPool[absPoolIndex] == 1) // slot is a sidelink slot
        {
            // PSCCH
            uint16_t numSlPscchRbs =
                NrSlRrcSap::GetSlFResoPscchValue(pool.slPscchConfig.slFreqResourcePscch);
            uint16_t slPscchSymStart = slSymbolStart;
            uint16_t slPscchSymLength =
                NrSlRrcSap::GetSlTResoPscchValue(pool.slPscchConfig.slTimeResourcePscch);
            // PSSCH
            uint16_t slPsschSymStart = slPscchSymStart + slPscchSymLength;
            bool slHasPsfch = SlotHasPsfch(i, phyPool, psfchPeriod);
            uint16_t slPsschSymLength;
            if (slHasPsfch)
            {
                // PSFCH requires an additional 3 symbols
                slPsschSymLength = (totalSlSymbols - slPscchSymLength) - 1 - 3;
            }
            else
            {
                slPsschSymLength = (totalSlSymbols - slPscchSymLength) - 1;
            }
            uint16_t slSubchannelSize = NrSlRrcSap::GetNrSlSubChSizeValue(pool.slSubchannelSize);
            uint16_t slMaxNumPerReserve = NrSlRrcSap::GetSlMaxNumPerReserveValue(
                pool.slUeSelectedConfigRp.slMaxNumPerReserve);
            uint64_t absSlotIndex = i;
            auto slotOffset = static_cast<uint32_t>(i - absIndexCurrentSlot);

            NrSlCommResourcePool::SlotInfo info(numSlPscchRbs,
                                                slPscchSymStart,
                                                slPscchSymLength,
                                                slPsschSymStart,
                                                slPsschSymLength,
                                                slHasPsfch,
                                                slSubchannelSize,
                                                slMaxNumPerReserve,
                                                absSlotIndex,
                                                slotOffset);
            list.emplace_back(info);
        }
        absPoolIndex = (absPoolIndex + 1) % phyPool.size();
    }

    NS_LOG_DEBUG(
        "Total number of slots available for Sidelink in the selection window = " << list.size());

    for (const auto& it : list)
    {
        NS_LOG_DEBUG("Absolute slot number of the Sidelink slot in the selection window = "
                     << it.slotOffset + absIndexCurrentSlot);
    }

    return list;
}

uint16_t
NrSlCommResourcePool::GetNrSlSensWindInSlots(uint8_t bwpId, uint16_t poolId, Time slotLength) const
{
    NS_LOG_FUNCTION(this << +bwpId << poolId << slotLength);

    [[maybe_unused]] NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId(bwpId, poolId);
    NrSlRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at(0);
    NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at(bwpId);
    NrSlRrcSap::SlResourcePoolConfigNr slResourcePoolConfig =
        slBwpConfigCommon.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal.at(poolId);
    NrSlRrcSap::SlSensingWindow slSensingWindowLen =
        slResourcePoolConfig.slResourcePool.slUeSelectedConfigRp.slSensingWindow;
    NS_ASSERT_MSG(slSensingWindowLen.windSize != NrSlRrcSap::SlSensingWindow::INVALID,
                  "Sensing window not set for BWP id " << +bwpId << " pool id " << poolId);

    uint16_t windLenInMs = NrSlRrcSap::GetSlSensWindowValue(slSensingWindowLen);

    double numSlots = (windLenInMs / static_cast<double>(1000)) / slotLength.GetSeconds();

    return static_cast<uint16_t>(numSlots);
}

void
NrSlCommResourcePool::SetNrSlSchedulingType(NrSlCommResourcePool::SchedulingType type)
{
    NS_LOG_FUNCTION(this << type);
    m_schType = type;
}

NrSlCommResourcePool::SchedulingType
NrSlCommResourcePool::GetNrSlSchedulingType() const
{
    return m_schType;
}

uint16_t
NrSlCommResourcePool::GetNrSlSubChSize(uint8_t bwpId, uint16_t poolId) const
{
    NS_LOG_FUNCTION(this << +bwpId << poolId);

    [[maybe_unused]] NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId(bwpId, poolId);
    NrSlRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at(0);
    NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at(bwpId);
    NrSlRrcSap::SlResourcePoolConfigNr slResourcePoolConfig =
        slBwpConfigCommon.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal.at(poolId);
    NrSlRrcSap::SlSubchannelSize slSubChSize = slResourcePoolConfig.slResourcePool.slSubchannelSize;
    NS_ASSERT_MSG(slSubChSize.numPrbs != NrSlRrcSap::SlSubchannelSize::INVALID,
                  "Subchannel is not set for BWP id " << +bwpId << " pool id " << poolId);

    uint16_t slSubChSizeInt = NrSlRrcSap::GetNrSlSubChSizeValue(slSubChSize);

    return slSubChSizeInt;
}

NrSlCommResourcePool::BwpAndPoolIt
NrSlCommResourcePool::ValidateBwpAndPoolId(uint8_t bwpId, uint16_t poolId) const
{
    NS_LOG_FUNCTION(this << +bwpId << poolId);
    auto itBwp = m_phySlPoolMap.find(bwpId);
    NS_ABORT_MSG_IF(itBwp == m_phySlPoolMap.end(), "Unable to find bandwidth part id " << +bwpId);
    auto itPool = itBwp->second.find(poolId);
    NS_ABORT_MSG_IF(itPool == itBwp->second.end(), "Unable to find pool id " << poolId);

    BwpAndPoolIt ret;
    ret.itBwp = itBwp;
    ret.itPool = itPool;
    return ret;
}

void
NrSlCommResourcePool::ValidateResvPeriod(uint8_t bwpId,
                                         uint16_t poolId,
                                         Time resvPeriod,
                                         Time slotLength) const
{
    NS_LOG_FUNCTION(this << +bwpId << poolId << resvPeriod.GetMilliSeconds());
    [[maybe_unused]] NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId(bwpId, poolId);
    const NrSlRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr(bwpId, poolId);
    std::list<NrSlRrcSap::SlResourceReservePeriod> resvPeriodList =
        pool.slUeSelectedConfigRp.slResourceReservePeriodList;
    auto periodInt = static_cast<uint16_t>(resvPeriod.GetMilliSeconds());
    NrSlRrcSap::SlResourceReservePeriod resvPeriodEnum =
        NrSlRrcSap::GetSlResoResvPrdEnum(periodInt);
    bool found = false;
    for (const auto& it : resvPeriodList)
    {
        if (it.period == resvPeriodEnum.period)
        {
            found = true;
            uint16_t rsvpInSlots = GetResvPeriodInSlots(bwpId, poolId, resvPeriod, slotLength);
            IsRsvpMultipOfPoolLen(bwpId, poolId, rsvpInSlots);
        }
    }
    NS_ABORT_MSG_IF(!found, "The given reservation period is not in the user specified list");
}

std::list<uint16_t>
NrSlCommResourcePool::GetSlResourceReservePeriodList(uint8_t bwpId, uint16_t poolId) const
{
    NS_LOG_FUNCTION(this << +bwpId << poolId);
    std::list<uint16_t> rpList;
    const NrSlRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr(bwpId, poolId);
    std::list<NrSlRrcSap::SlResourceReservePeriod> resvPeriodList =
        pool.slUeSelectedConfigRp.slResourceReservePeriodList;
    for (const auto& it : resvPeriodList)
    {
        uint16_t value = NrSlRrcSap::GetSlResoResvPrdValue(it);
        rpList.push_back(value);
    }
    return rpList;
}

uint16_t
NrSlCommResourcePool::GetResvPeriodInSlots(uint8_t bwpId,
                                           uint16_t poolId,
                                           Time resvPeriod,
                                           Time slotLength) const
{
    NS_LOG_FUNCTION(this << +bwpId << poolId << resvPeriod.GetMilliSeconds() << slotLength);
    [[maybe_unused]] NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId(bwpId, poolId);
    auto periodInt = static_cast<uint16_t>(resvPeriod.GetMilliSeconds());

    double numResvSlots = (periodInt / static_cast<double>(1000)) / slotLength.GetSeconds();

    auto rsvpInSlots = static_cast<uint16_t>(numResvSlots);
    IsRsvpMultipOfPoolLen(bwpId, poolId, rsvpInSlots);
    return rsvpInSlots;
}

bool
NrSlCommResourcePool::IsSidelinkSlot(uint8_t bwpId, uint16_t poolId, uint64_t absSlotIndex) const
{
    NS_LOG_FUNCTION(this << +bwpId << poolId << absSlotIndex);

    uint16_t absPoolIndex = GetAbsPoolIndex(bwpId, poolId, absSlotIndex);
    std::vector<std::bitset<1>> phyPool = GetNrSlPhyPool(bwpId, poolId);
    // trigger SL only when it is a SL slot
    return phyPool.at(absPoolIndex) == 1;
}

uint16_t
NrSlCommResourcePool::GetAbsPoolIndex(uint8_t bwpId, uint16_t poolId, uint64_t absSlotIndex) const
{
    NS_LOG_FUNCTION(this << +bwpId << poolId << absSlotIndex);
    NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId(bwpId, poolId);
    std::vector<std::bitset<1>> phyPool = ret.itPool->second;
    uint16_t absPoolIndex = absSlotIndex % phyPool.size();
    return absPoolIndex;
}

void
NrSlCommResourcePool::SetTddPattern(std::vector<NrSlUeRrc::LteNrTddSlotType> tddPattern)
{
    NS_LOG_FUNCTION(this);
    m_tddPattern = tddPattern;
}

uint16_t
NrSlCommResourcePool::GetSlSubChSize(uint8_t bwpId, uint16_t poolId) const
{
    NS_LOG_FUNCTION(this << +bwpId << poolId);
    [[maybe_unused]] NrSlCommResourcePool::BwpAndPoolIt ret = ValidateBwpAndPoolId(bwpId, poolId);
    const NrSlRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr(bwpId, poolId);
    return NrSlRrcSap::GetNrSlSubChSizeValue(pool.slSubchannelSize);
}

void
NrSlCommResourcePool::IsRsvpMultipOfPoolLen(uint8_t bwpId,
                                            uint16_t poolId,
                                            uint16_t rsvpInSlots) const
{
    NS_LOG_FUNCTION(this << +bwpId << poolId << rsvpInSlots);
    std::vector<std::bitset<1>> phyPool = GetNrSlPhyPool(bwpId, poolId);
    NS_ABORT_MSG_IF(rsvpInSlots % phyPool.size() != 0,
                    "Resource reservation period in slots "
                        << rsvpInSlots << " should be multiple of physical sidelink pool length of "
                        << phyPool.size());
}

uint32_t
NrSlCommResourcePool::GetNReDmrs(uint16_t nDmrsSymSlot) const
{
    NS_ASSERT_MSG(nDmrsSymSlot >= 2 && nDmrsSymSlot <= 4, "nDmrsSymSlot must be between 2 and 4");
    // The constant 6 below derives from TS 38.214 Table 8.1.3.2-1
    return 6 * nDmrsSymSlot;
}

uint32_t
NrSlCommResourcePool::GetNReSci1(const SlotInfo& slotInfo) const
{
    NS_LOG_FUNCTION(this);
    // SCI1 REs = PSCCH symbols * PSCCH PRBs * 12 subcarriers per PRB
    return slotInfo.slPscchSymLength * slotInfo.numSlPscchRbs * 12;
}

uint32_t
NrSlCommResourcePool::GetNReSci2(double codeRate) const
{
    NS_LOG_FUNCTION(this << codeRate);
    // The equation for number of REs is expressed in TS 38.212 Section 8.4.4
    // as "Q_prime_sci2 = min(part1, part2) + gamma", where Q_prime_sci2 is
    // the number of REs
    //
    // part1 = ceil((O_SCI2 + L_SCI2) * BETA_OFFSET_SCI2 / (Q_M_SCI2 * R_SCI2))
    // part2 = ceil(alpha * M_SC_SCI2_total)
    //
    // Per TS 38.212 Section 8.4.4, R_SCI2 is the code rate indicated by the
    // "Modulation and coding scheme" field in SCI format 1-A (the data MCS
    // code rate), not a fixed SCI2-specific rate. Q_M_SCI2 is 2 (QPSK)
    // because SCI2 is always QPSK-modulated regardless of the data modulation.
    //
    // Fixed parameters per TS 38.212 Section 8.4.1.1:
    constexpr uint32_t O_SCI2 = 35;            // SCI format 2-A payload size (bits)
    constexpr uint32_t L_SCI2 = 24;            // CRC length for 2nd-stage SCI
    constexpr double BETA_OFFSET_SCI2 = 1.125; // From sl-BetaOffsets2ndSCI index 1
    constexpr uint8_t Q_M_SCI2 = 2;            // Modulation order for SCI2 (QPSK)

    double part1 =
        std::ceil(static_cast<double>(O_SCI2 + L_SCI2) * BETA_OFFSET_SCI2 / (Q_M_SCI2 * codeRate));

    // For typical configurations, part1 < part2, so min(part1, part2) = part1
    // gamma = 0 per NIST New Radio Sidelink LLS implementation (citing 3GPP R1-2007161)
    return static_cast<uint32_t>(part1);
}

uint32_t
NrSlCommResourcePool::ComputeTbsFromNInfo(double nInfo, double codeRate) const
{
    NS_LOG_FUNCTION(this << nInfo << codeRate);

    // TS 38.214 Section 5.1.3.2 TBS determination
    if (nInfo <= 0)
    {
        return 0;
    }
    uint32_t tbs{0};
    if (nInfo <= 3824)
    {
        // Step 3: Quantize N_info to N'_info
        uint32_t n = std::max(3u, static_cast<uint32_t>(std::floor(std::log2(nInfo))) - 6);
        uint32_t nInfoPrime =
            std::max(24u,
                     static_cast<uint32_t>(std::pow(2, n)) *
                         static_cast<uint32_t>(std::floor(nInfo / std::pow(2, n))));

        // TBS lookup table (TS 38.214 Table 5.1.3.2-1)
        // Simplified lookup: find closest TBS >= N'_info
        static constexpr std::array<uint32_t, 93> tbsTable = {
            24,   32,   40,   48,   56,   64,   72,   80,   88,   96,   104,  112,  120,  128,
            136,  144,  152,  160,  168,  176,  184,  192,  208,  224,  240,  256,  272,  288,
            304,  320,  336,  352,  368,  384,  408,  432,  456,  480,  504,  528,  552,  576,
            608,  640,  672,  704,  736,  768,  808,  848,  888,  928,  984,  1032, 1064, 1128,
            1160, 1192, 1224, 1256, 1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800,
            1864, 1928, 2024, 2088, 2152, 2216, 2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792,
            2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824};

        auto it = std::lower_bound(tbsTable.begin(), tbsTable.end(), nInfoPrime);
        if (it != tbsTable.end())
        {
            tbs = *it;
        }
        else
        {
            tbs = tbsTable.back();
        }
        NS_LOG_DEBUG("TB size calc step 3: n=" << n << " N_info_prime=" << nInfoPrime
                                               << " size (from table)=" << tbs);
    }
    else
    {
        // Step 4: For N_info > 3824
        uint32_t n = static_cast<uint32_t>(std::floor(std::log2(nInfo - 24))) - 5;
        uint32_t nInfoPrime =
            std::max(3840u,
                     static_cast<uint32_t>(std::pow(2, n)) *
                         static_cast<uint32_t>(std::round((nInfo - 24) / std::pow(2, n))));

        if (codeRate <= 0.25)
        {
            uint32_t C = std::ceil((nInfoPrime + 24) / 3816.0);
            tbs = 8 * C * std::ceil((nInfoPrime + 24) / (8.0 * C)) - 24;
        }
        else
        {
            if (nInfoPrime > 8424)
            {
                uint32_t C = std::ceil((nInfoPrime + 24) / 8424.0);
                tbs = 8 * C * std::ceil((nInfoPrime + 24) / (8.0 * C)) - 24;
            }
            else
            {
                tbs = 8 * std::ceil((nInfoPrime + 24) / 8.0) - 24;
            }
        }
        NS_LOG_DEBUG("TB size calc step 4: n=" << n << " N_info_prime=" << nInfoPrime
                                               << " size (from formula)=" << tbs);
    }
    return tbs;
}

uint32_t
NrSlCommResourcePool::GetTransportBlockSize(const SlotInfo& slotInfo,
                                            uint8_t mcs,
                                            uint16_t nSubchannels,
                                            uint8_t mcsTable) const
{
    NS_LOG_FUNCTION(this << +mcs << nSubchannels << +mcsTable);
    NS_ABORT_MSG_IF(nSubchannels == 0, "Number of subchannels must be > 0");
    NS_ABORT_MSG_IF(mcsTable != 1 && mcsTable != 2, "MCS table must be 1 or 2");
    uint8_t maxMcs = NrMcsTables::GetMaxMcs(mcsTable);
    NS_ABORT_MSG_IF(mcs > maxMcs,
                    "MCS " << +mcs << " exceeds max " << +maxMcs << " for table " << +mcsTable);

    // These are currently hardcoded values in this implementation
    constexpr uint8_t RANK = 1;             // Sidelink Release 16 uses rank 1
    constexpr uint16_t N_DMRS_SYM_SLOT = 2; // NIST LLS assumes Type 1 with 2 symbols/slot

    // The number of subcarriers per physical resource block is a constant
    constexpr uint32_t N_SC_RB = 12; // Subcarriers per RB (TS 38.211 Section 4.4.4.1)

    // Compute N_RE' per PRB
    uint32_t nSymbPssch = slotInfo.slPsschSymLength;
    uint32_t nReDmrs = GetNReDmrs(N_DMRS_SYM_SLOT);
    uint32_t nRePrime = N_SC_RB * nSymbPssch - nReDmrs; // N_OH_PRB = 0 for FR1

    // Compute total N_RE (number of resource elements)
    // Per TS 38.214 Section 8.1.3.2:
    // N_RE = N_RE' * n_PRB - N_RE_SCI1 - N_RE_SCI2
    // where N_RE_SCI1 accounts for PSCCH REs (same frequency, different symbols)
    // and N_RE_SCI2 accounts for 2nd-stage SCI multiplexed with PSSCH
    uint32_t nPrb = nSubchannels * slotInfo.slSubchannelSize;
    uint32_t nReSci1 = GetNReSci1(slotInfo);

    // Compute N_info
    uint8_t Qm = NrMcsTables::GetModulationOrder(mcs, mcsTable);
    double R = NrMcsTables::GetCodeRate(mcs, mcsTable);

    // N_RE_SCI2 depends on the data code rate per TS 38.212 Section 8.4.4
    uint32_t nReSci2 = GetNReSci2(R);
    uint32_t nRe = nRePrime * nPrb - nReSci1 - nReSci2;
    double nInfo = static_cast<double>(nRe) * Qm * R * RANK;

    NS_LOG_DEBUG("TBS calc: nSubch=" << nSubchannels << " nPrb=" << nPrb
                                     << " nSymbPssch=" << nSymbPssch << " nReDmrs=" << nReDmrs
                                     << " N_RE'=" << nRePrime << " N_RE_SCI1=" << nReSci1
                                     << " N_RE_SCI2=" << nReSci2 << " N_RE=" << nRe << " Qm=" << +Qm
                                     << " R=" << R << " N_info=" << nInfo);

    uint32_t tbsBits = ComputeTbsFromNInfo(nInfo, R);
    NS_LOG_DEBUG("TBS result: tbsBits=" << tbsBits << " tbsBytes=" << tbsBits / 8);
    return tbsBits / 8;
}

std::optional<uint16_t>
NrSlCommResourcePool::GetMinSubchannels(const SlotInfo& slotInfo,
                                        uint8_t mcs,
                                        uint32_t transportBlockSize,
                                        uint16_t maxSubchannels,
                                        uint8_t mcsTable) const
{
    NS_LOG_FUNCTION(this << +mcs << transportBlockSize << maxSubchannels << +mcsTable);
    NS_ABORT_MSG_IF(maxSubchannels == 0, "Maximum subchannels must be > 0");
    for (uint16_t nSubch = 1; nSubch <= maxSubchannels; nSubch++)
    {
        uint32_t tbs = GetTransportBlockSize(slotInfo, mcs, nSubch, mcsTable);
        if (tbs >= transportBlockSize)
        {
            return nSubch;
        }
    }
    NS_LOG_DEBUG("Transport block size " << transportBlockSize << " cannot fit in max subchannels "
                                         << maxSubchannels);
    return std::nullopt;
}

uint16_t
NrSlCommResourcePool::GetNumSlPscchRbs(uint8_t bwpId, uint16_t poolId) const
{
    const NrSlRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr(bwpId, poolId);
    return NrSlRrcSap::GetSlFResoPscchValue(pool.slPscchConfig.slFreqResourcePscch);
}

uint16_t
NrSlCommResourcePool::GetPscchSymStart(uint8_t bwpId, uint16_t poolId) const
{
    NrSlRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at(0);
    NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at(bwpId);
    return NrSlRrcSap::GetSlStartSymbolValue(slBwpConfigCommon.slBwpGeneric.slStartSymbol);
}

uint16_t
NrSlCommResourcePool::GetPscchSymLength(uint8_t bwpId, uint16_t poolId) const
{
    const NrSlRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr(bwpId, poolId);
    return NrSlRrcSap::GetSlTResoPscchValue(pool.slPscchConfig.slTimeResourcePscch);
}

uint16_t
NrSlCommResourcePool::GetPsschSymStart(uint8_t bwpId, uint16_t poolId) const
{
    return GetPscchSymStart(bwpId, poolId) + GetPscchSymLength(bwpId, poolId);
}

uint16_t
NrSlCommResourcePool::GetPsschSymLength(uint8_t bwpId, uint16_t poolId, bool hasPsfch) const
{
    NrSlRrcSap::SlFreqConfigCommonNr slfreqConfigCommon = m_slPreconfigFreqInfoList.at(0);
    NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigCommon = slfreqConfigCommon.slBwpList.at(bwpId);
    uint16_t totalSlSymbols =
        NrSlRrcSap::GetSlLengthSymbolsValue(slBwpConfigCommon.slBwpGeneric.slLengthSymbols);
    uint16_t slPsschSymLength = (totalSlSymbols - GetPscchSymLength(bwpId, poolId)) - 1;
    if (hasPsfch)
    {
        // PSFCH requires an additional 3 symbols
        slPsschSymLength -= 3;
    }
    return slPsschSymLength;
}

uint16_t
NrSlCommResourcePool::GetMaxNumPerReserve(uint8_t bwpId, uint16_t poolId) const
{
    const NrSlRrcSap::SlResourcePoolNr pool = GetSlResourcePoolNr(bwpId, poolId);
    return NrSlRrcSap::GetSlMaxNumPerReserveValue(pool.slUeSelectedConfigRp.slMaxNumPerReserve);
}

} // namespace ns3
