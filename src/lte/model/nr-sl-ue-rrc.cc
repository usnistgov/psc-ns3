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

#include "lte-radio-bearer-info.h"
#include "lte-rrc-sap.h"
#include "lte-ue-rrc.h"

#include <ns3/abort.h>
#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/object-factory.h>
#include <ns3/object-map.h>
#include <ns3/pointer.h>
#include <ns3/simulator.h>

#include <algorithm>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlUeRrc");
NS_OBJECT_ENSURE_REGISTERED(NrSlUeRrc);

TypeId
NrSlUeRrc::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlUeRrc")
                            .SetParent<Object>()
                            .SetGroupName("Lte")
                            .AddConstructor<NrSlUeRrc>();
    return tid;
}

NrSlUeRrc::NrSlUeRrc()
{
    NS_LOG_FUNCTION(this);
    m_nrSlRrcSapUser = new MemberNrSlUeRrcSapUser<NrSlUeRrc>(this);
}

NrSlUeRrc::~NrSlUeRrc()
{
    NS_LOG_FUNCTION(this);
}

void
NrSlUeRrc::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_nrSlRrcSapUser;
}

void
NrSlUeRrc::SetNrSlUeRrcSapProvider(NrSlUeRrcSapProvider* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeRrcSapProvider = s;
}

NrSlUeRrcSapUser*
NrSlUeRrc::GetNrSlUeRrcSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlRrcSapUser;
}

void
NrSlUeRrc::SetNrSlEnabled(bool status)
{
    NS_LOG_FUNCTION(this);
    m_slEnabled = status;
}

bool
NrSlUeRrc::IsNrSlEnabled()
{
    NS_LOG_FUNCTION(this);
    return m_slEnabled;
}

void
NrSlUeRrc::SetSourceL2Id(uint32_t srcL2Id)
{
    NS_LOG_FUNCTION(this);
    m_srcL2Id = srcL2Id;
    // Set source L2 id in RCC
    m_nrSlUeRrcSapProvider->SetSourceL2Id(srcL2Id);
}

uint32_t
NrSlUeRrc::DoGetSourceL2Id()
{
    NS_LOG_FUNCTION(this);
    return m_srcL2Id;
}

void
NrSlUeRrc::SetNrSlPreconfiguration(const LteRrcSap::SidelinkPreconfigNr& preconfiguration)
{
    NS_LOG_FUNCTION(this);
    m_preconfiguration = preconfiguration;
    m_tddPattern = ConvertTddPattern(m_preconfiguration.slPreconfigGeneral.slTddConfig.tddPattern);
    // Tell RRC to populate pools
    m_nrSlUeRrcSapProvider->PopulatePools();
}

void
NrSlUeRrc::SetNrSlDiscoveryRelayConfiguration(const LteRrcSap::SlRelayUeConfig relayConfig)
{
    NS_LOG_FUNCTION(this);
    // Tell the RRC the relay discovery and (re)selection requirements for the relay UE
    m_nrSlUeRrcSapProvider->SetRelayRequirements(relayConfig);
}

void
NrSlUeRrc::SetNrSlDiscoveryRemoteConfiguration(const LteRrcSap::SlRemoteUeConfig remoteConfig)
{
    NS_LOG_FUNCTION(this);
    // Tell the RRC the relay discovery and (re)selection requirements for the remote UE
    m_nrSlUeRrcSapProvider->SetRemoteRequirements(remoteConfig);
}

std::vector<NrSlUeRrc::LteNrTddSlotType>
NrSlUeRrc::ConvertTddPattern(std::string tddPattern)
{
    static std::unordered_map<std::string, NrSlUeRrc::LteNrTddSlotType> lookupTable = {
        {"DL", NrSlUeRrc::LteNrTddSlotType::DL},
        {"UL", NrSlUeRrc::LteNrTddSlotType::UL},
        {"S", NrSlUeRrc::LteNrTddSlotType::S},
        {"F", NrSlUeRrc::LteNrTddSlotType::F},
    };

    std::vector<NrSlUeRrc::LteNrTddSlotType> vector;
    std::stringstream ss(tddPattern);
    std::string token;
    std::vector<std::string> extracted;

    while (std::getline(ss, token, '|'))
    {
        extracted.push_back(token);
    }

    for (const auto& v : extracted)
    {
        if (lookupTable.find(v) == lookupTable.end())
        {
            NS_FATAL_ERROR("Pattern type " << v << " not valid. Valid values are: DL UL F S");
        }
        vector.push_back(lookupTable[v]);
    }

    return vector;
}

const LteRrcSap::SidelinkPreconfigNr
NrSlUeRrc::DoGetNrSlPreconfiguration()
{
    NS_LOG_FUNCTION(this);
    return m_preconfiguration;
}

const std::vector<std::bitset<1>>
NrSlUeRrc::DoGetPhysicalSlPool(const std::vector<std::bitset<1>>& slBitMap)
{
    return GetPhysicalSlPool(slBitMap, m_tddPattern);
}

std::vector<std::bitset<1>>
NrSlUeRrc::GetPhysicalSlPool(const std::vector<std::bitset<1>>& slBitMap,
                             std::vector<NrSlUeRrc::LteNrTddSlotType> tddPattern)
{
    std::vector<std::bitset<1>> finalSlPool;

    uint16_t countUl =
        std::count(tddPattern.begin(), tddPattern.end(), NrSlUeRrc::LteNrTddSlotType::UL);
    NS_LOG_DEBUG("number of uplinks in the given TDD pattern " << countUl);
    NS_ABORT_MSG_UNLESS(countUl > 0, "No UL slot found in the given TDD pattern");
    NS_ABORT_MSG_IF(slBitMap.size() % countUl != 0,
                    "SL bit map size should be multiple of number of UL slots in the TDD pattern");
    NS_ABORT_MSG_IF(slBitMap.size() < tddPattern.size(),
                    "SL bit map size should be greater than or equal to the TDD pattern size");

    auto patternIt = tddPattern.cbegin();
    auto slBitMapit = slBitMap.cbegin();

    do
    {
        if (*patternIt != NrSlUeRrc::LteNrTddSlotType::UL)
        {
            NS_LOG_DEBUG("Not an UL slot :  " << *patternIt << ", putting 0 in the final bitmap");
            finalSlPool.emplace_back(0);
        }
        else if (*slBitMapit == 1)
        {
            // UL slot and SL bitmap value is 1
            NS_LOG_DEBUG("It is an UL slot :  " << *patternIt << ", and SL bitmap value is "
                                                << *slBitMapit
                                                << ", putting 1 in the final bitmap");
            finalSlPool.emplace_back(1);
            slBitMapit++;
        }
        else
        {
            // UL slot and SL bitmap value is 0
            NS_LOG_DEBUG("It is an UL slot :  " << *patternIt << ", but SL bitmap value is "
                                                << *slBitMapit
                                                << ", putting 0 in the final bitmap");
            finalSlPool.emplace_back(0);
            slBitMapit++;
        }

        if (patternIt == tddPattern.cend() - 1)
        {
            NS_LOG_DEBUG("It is the last element of the TDD pattern " << *patternIt);

            if (slBitMapit == slBitMap.cend())
            {
                // if we have cover all the SL bitmap we are done. Break now.
                break;
            }
            else
            {
                // we have not covered all the SL bitmap. Prepare to re-apply the TDD pattern
                patternIt = tddPattern.cbegin();
                NS_LOG_DEBUG("re-assigning to the first element of tdd pattern " << *patternIt);
            }
        }
        else
        {
            patternIt++;
        }

    } while (patternIt != tddPattern.end());

    return finalSlPool;
}

void
NrSlUeRrc::StoreSlBwpId(uint8_t bwpId)
{
    NS_LOG_FUNCTION(this << +bwpId);
    std::pair<std::set<uint8_t>::iterator, bool> ret;
    ret = m_slBwpIds.insert(bwpId);
    NS_ABORT_MSG_IF(ret.second == false, "BWP id " << +bwpId << " already exists");
}

const std::set<uint8_t>
NrSlUeRrc::DoGetBwpIdContainer()
{
    NS_LOG_FUNCTION(this);
    return m_slBwpIds;
}

void
NrSlUeRrc::DoAddNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb)
{
    NS_LOG_FUNCTION(this);
    auto destIt = m_slTxDrbMap.find(slTxDrb->m_destinationL2Id);
    if (destIt == m_slTxDrbMap.end())
    {
        NS_LOG_LOGIC("First SL DRB for destination " << slTxDrb->m_destinationL2Id);
        NrSlDrbMapPerLcId mapPerLcId;
        mapPerLcId.insert(
            std::pair<uint8_t, Ptr<NrSlDataRadioBearerInfo>>(slTxDrb->m_logicalChannelIdentity,
                                                             slTxDrb));
        m_slTxDrbMap.insert(
            std::pair<uint32_t, NrSlDrbMapPerLcId>(slTxDrb->m_destinationL2Id, mapPerLcId));
    }
    else
    {
        NrSlDrbMapPerLcId::iterator lcIt;
        lcIt = destIt->second.find(slTxDrb->m_logicalChannelIdentity);
        if (lcIt == destIt->second.end())
        {
            // New bearer for the destination
            destIt->second.insert(
                std::pair<uint8_t, Ptr<NrSlDataRadioBearerInfo>>(slTxDrb->m_logicalChannelIdentity,
                                                                 slTxDrb));
        }
        else
        {
            NS_FATAL_ERROR("SL DRB with LC id = " << +slTxDrb->m_logicalChannelIdentity
                                                  << " already exists");
        }
    }
}

void
NrSlUeRrc::DoAddNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb)
{
    NS_LOG_FUNCTION(this);
    std::pair<uint32_t, uint32_t> key =
        std::make_pair(slRxDrb->m_sourceL2Id, slRxDrb->m_destinationL2Id);
    auto srcIt = m_slRxDrbMap.find(key);
    if (srcIt == m_slRxDrbMap.end())
    {
        NS_LOG_LOGIC("First SL RX DRB for this UE. Source L2 id " << slRxDrb->m_sourceL2Id
                                                                  << " Destination L2 id "
                                                                  << slRxDrb->m_destinationL2Id);
        NrSlDrbMapPerLcId mapPerRxLcId;
        mapPerRxLcId.insert(
            std::pair<uint8_t, Ptr<NrSlDataRadioBearerInfo>>(slRxDrb->m_logicalChannelIdentity,
                                                             slRxDrb));
        m_slRxDrbMap.emplace(key, mapPerRxLcId);
    }
    else
    {
        NrSlDrbMapPerLcId::iterator rxLcIt;
        rxLcIt = srcIt->second.find(slRxDrb->m_logicalChannelIdentity);
        if (rxLcIt == srcIt->second.end())
        {
            // New Rx bearer for the remote UE
            srcIt->second.insert(
                std::pair<uint8_t, Ptr<NrSlDataRadioBearerInfo>>(slRxDrb->m_logicalChannelIdentity,
                                                                 slRxDrb));
        }
        else
        {
            NS_FATAL_ERROR("SL RX DRB with LC id = " << +slRxDrb->m_logicalChannelIdentity
                                                     << " already exists");
        }
    }
}

Ptr<NrSlDataRadioBearerInfo>
NrSlUeRrc::DoGetSidelinkTxDataRadioBearer(uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this);
    return GetSidelinkTxDataRadioBearer(m_srcL2Id, dstL2Id, lcId);
}

Ptr<NrSlDataRadioBearerInfo>
NrSlUeRrc::GetSidelinkTxDataRadioBearer(uint32_t srcL2Id, uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this << srcL2Id << dstL2Id << lcId);
    Ptr<NrSlDataRadioBearerInfo> slTxRb = nullptr;
    NrSlDrbMapPerL2Id::iterator destIt = m_slTxDrbMap.find(dstL2Id);
    NS_ASSERT_MSG(destIt != m_slTxDrbMap.end(),
                  "Unable to find DRB for destination L2 Id "
                      << dstL2Id << " size " << m_slTxDrbMap.size() << " src " << m_srcL2Id);
    NrSlDrbMapPerLcId::iterator lcIt = destIt->second.find(lcId);
    NS_ASSERT_MSG(lcIt != destIt->second.end(),
                  "Unable to find LCID for destination L2 Id " << dstL2Id << " lcId " << +lcId
                                                               << " my L2Id " << m_srcL2Id);

    return lcIt->second;
}

std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
NrSlUeRrc::GetAllSidelinkTxDataRadioBearers(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    NrSlDrbMapPerL2Id::iterator destIt = m_slTxDrbMap.find(dstL2Id);
    NS_ASSERT_MSG(destIt != m_slTxDrbMap.end(),
                  "Unable to find DRB for destination L2 Id "
                      << dstL2Id << " size " << m_slTxDrbMap.size() << " src " << m_srcL2Id);
    return destIt->second;
}

std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
NrSlUeRrc::DoGetAllSidelinkTxDataRadioBearers(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this);
    return GetAllSidelinkTxDataRadioBearers(dstL2Id);
}

std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
NrSlUeRrc::DoGetAllSidelinkRxDataRadioBearers(uint32_t srcL2Id)
{
    NS_LOG_FUNCTION(this);
    return GetAllSidelinkRxDataRadioBearers(srcL2Id);
}

void
NrSlUeRrc::DoRemoveNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb)
{
    NS_LOG_FUNCTION(this);

    NrSlDrbMapPerL2Id::iterator destIt = m_slTxDrbMap.find(slTxDrb->m_destinationL2Id);
    if (destIt != m_slTxDrbMap.end())
    {
        NS_LOG_LOGIC("SL TX DRB found for this destination ID " << slTxDrb->m_destinationL2Id);
        m_slTxDrbMap.erase(slTxDrb->m_destinationL2Id);
    }
}

void
NrSlUeRrc::DoRemoveNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb)
{
    NS_LOG_FUNCTION(this);
    std::pair<uint32_t, uint32_t> key =
        std::make_pair(slRxDrb->m_sourceL2Id, slRxDrb->m_destinationL2Id);
    auto srcIt = m_slRxDrbMap.find(key);
    if (srcIt != m_slRxDrbMap.end())
    {
        NS_LOG_LOGIC("SL RX DRB found for remote UE with source L2 id "
                     << slRxDrb->m_sourceL2Id << " dstL2Id " << slRxDrb->m_destinationL2Id);
        m_slRxDrbMap.erase(key);
    }
}

Ptr<NrSlDataRadioBearerInfo>
NrSlUeRrc::DoGetSidelinkRxDataRadioBearer(uint32_t srcL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this);
    return GetSidelinkRxDataRadioBearer(srcL2Id, m_srcL2Id, lcId);
}

Ptr<NrSlDataRadioBearerInfo>
NrSlUeRrc::GetSidelinkRxDataRadioBearer(uint32_t srcL2Id, uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this << srcL2Id << dstL2Id << lcId);
    auto destIt = m_slRxDrbMap.find(std::pair(srcL2Id, dstL2Id));
    if (destIt == m_slRxDrbMap.end())
    {
        NS_LOG_DEBUG("No receive DRB exists for srcL2Id " << srcL2Id << " dstL2Id " << dstL2Id
                                                          << " lcId " << +lcId);
        return Ptr<NrSlDataRadioBearerInfo>();
    }
    NrSlDrbMapPerLcId::iterator lcIt = destIt->second.find(lcId);
    if (lcIt == destIt->second.end())
    {
        NS_LOG_DEBUG("No receive DRB exists for srcL2Id " << srcL2Id << " dstL2Id " << dstL2Id
                                                          << " lcId " << +lcId);
        return Ptr<NrSlDataRadioBearerInfo>();
    }
    return lcIt->second;
}

std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>
NrSlUeRrc::GetAllSidelinkRxDataRadioBearers(uint32_t srcL2Id)
{
    NS_LOG_FUNCTION(this << srcL2Id);
    auto destIt = m_slRxDrbMap.find(std::pair(srcL2Id, m_srcL2Id));
    if (destIt != m_slRxDrbMap.end())
    {
        return destIt->second;
    }
    else
    {
        NS_LOG_DEBUG("No receive DRB exist for " << srcL2Id);
        return std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>();
    }
}

uint8_t
NrSlUeRrc::DoGetNextLcid(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this);
    // Note: This function supports the fact that multiple bearers can exist between
    // a source and destination. However, the rest of the code currently work
    // with only one LC per destination.

    // find unused the LCID
    uint8_t lcid = 0; // initialize with invalid value

    auto destIt = m_slTxDrbMap.find(dstL2Id);
    if (destIt == m_slTxDrbMap.end())
    {
        // LCIDs for traffic channels start at 5
        lcid = 5;
    }
    else
    {
        // if the size of the LC id per DRB map is equal to
        // the maximum allowed LCIDs, we halt!
        if (destIt->second.size() == 16)
        {
            NS_FATAL_ERROR("All the 16 LC ids are allocated");
        }
        // find an id not being used
        for (uint8_t lcidTmp = 5; lcidTmp < 20; lcidTmp++)
        {
            NrSlDrbMapPerLcId::iterator lcIt;
            lcIt = destIt->second.find(lcidTmp);
            if (lcIt != destIt->second.end())
            {
                continue;
            }
            else
            {
                lcid = lcidTmp;
                break; // avoid increasing lcid
            }
        }
    }
    NS_ASSERT(lcid != 0);
    return lcid;
}

std::ostream&
operator<<(std::ostream& os, const NrSlUeRrc::LteNrTddSlotType& item)
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

void
NrSlUeRrc::DoAddTxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb)
{
    NS_LOG_FUNCTION(this);
    NrSlSrbMapPerL2Id::iterator destIt = m_slTxSrbMap.find(slSrb->m_destinationL2Id);
    if (destIt == m_slTxSrbMap.end())
    {
        NS_LOG_LOGIC("First SL-SRB for destination " << slSrb->m_destinationL2Id);
        NrSlSrbMapPerLcId mapPerLcId;
        mapPerLcId.insert(
            std::pair<uint8_t, Ptr<NrSlSignallingRadioBearerInfo>>(slSrb->m_logicalChannelIdentity,
                                                                   slSrb));
        m_slTxSrbMap.insert(
            std::pair<uint32_t, NrSlSrbMapPerLcId>(slSrb->m_destinationL2Id, mapPerLcId));
        NS_LOG_LOGIC("Added SL-SRB with LCID " << (uint16_t)slSrb->m_logicalChannelIdentity
                                               << " for destination " << slSrb->m_destinationL2Id);
    }
    else
    {
        NrSlSrbMapPerLcId::iterator lcIt;
        lcIt = destIt->second.find(slSrb->m_logicalChannelIdentity);
        if (lcIt == destIt->second.end())
        {
            // New bearer for the destination
            destIt->second.insert(std::pair<uint8_t, Ptr<NrSlSignallingRadioBearerInfo>>(
                slSrb->m_logicalChannelIdentity,
                slSrb));
            NS_LOG_LOGIC("Added SL-SRB with LCID " << slSrb->m_logicalChannelIdentity
                                                   << "for destination "
                                                   << slSrb->m_destinationL2Id);
        }
        else
        {
            NS_FATAL_ERROR("SL-SRB with LC id = " << (uint16_t)slSrb->m_logicalChannelIdentity
                                                  << " already exists");
        }
    }
}

void
NrSlUeRrc::DoAddRxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slRxSrb)
{
    NS_LOG_FUNCTION(this);
    NrSlSrbMapPerL2Id::iterator srcIt = m_slRxSrbMap.find(slRxSrb->m_sourceL2Id);
    if (srcIt == m_slRxSrbMap.end())
    {
        NS_LOG_LOGIC("First SL RX SRB for peer UE with source L2 id "
                     << slRxSrb->m_sourceL2Id << ", LCID "
                     << (uint16_t)slRxSrb->m_logicalChannelIdentity);
        NrSlSrbMapPerLcId mapPerRxLcId;
        mapPerRxLcId.insert(std::pair<uint8_t, Ptr<NrSlSignallingRadioBearerInfo>>(
            slRxSrb->m_logicalChannelIdentity,
            slRxSrb));
        m_slRxSrbMap.insert(
            std::pair<uint32_t, NrSlSrbMapPerLcId>(slRxSrb->m_sourceL2Id, mapPerRxLcId));
    }
    else
    {
        NrSlSrbMapPerLcId::iterator rxLcIt;
        rxLcIt = srcIt->second.find(slRxSrb->m_logicalChannelIdentity);
        if (rxLcIt == srcIt->second.end())
        {
            // New Rx bearer for the remote UE
            srcIt->second.insert(std::pair<uint8_t, Ptr<NrSlSignallingRadioBearerInfo>>(
                slRxSrb->m_logicalChannelIdentity,
                slRxSrb));
            NS_LOG_LOGIC("Added RX SL-SRB with LCID " << (uint16_t)slRxSrb->m_logicalChannelIdentity
                                                      << "for peer source "
                                                      << slRxSrb->m_sourceL2Id);
        }
        else
        {
            NS_FATAL_ERROR("RX SL-SRB for peer source "
                           << slRxSrb->m_sourceL2Id << " with LCID = "
                           << (uint16_t)slRxSrb->m_logicalChannelIdentity << " already exists");
        }
    }
}

Ptr<NrSlSignallingRadioBearerInfo>
NrSlUeRrc::DoGetTxNrSlSignallingRadioBearer(uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this);
    Ptr<NrSlSignallingRadioBearerInfo> slSrb = nullptr;
    NrSlSrbMapPerL2Id::iterator destIt = m_slTxSrbMap.find(dstL2Id);

    if (destIt == m_slTxSrbMap.end())
    {
        NS_FATAL_ERROR("Unable to find any SL-SRB for destination L2 Id " << dstL2Id);
    }
    else
    {
        NS_LOG_LOGIC("Searching SL-SRB with LCID " << (uint16_t)lcId << " for destination L2 Id "
                                                   << dstL2Id);

        NrSlSrbMapPerLcId::iterator lcIt;
        lcIt = destIt->second.find(lcId);
        if (lcIt == destIt->second.end())
        {
            NS_FATAL_ERROR("SL-SRB with LCID = " << (uint16_t)lcId
                                                 << " does not exist for destination L2 Id "
                                                 << dstL2Id);
        }
        else
        {
            slSrb = lcIt->second;
            NS_LOG_LOGIC("Found SL-SRB with LCID " << (uint16_t)slSrb->m_logicalChannelIdentity
                                                   << " for destination "
                                                   << slSrb->m_destinationL2Id);
        }
    }
    return slSrb;
}

void
NrSlUeRrc::DoAddTxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slTxDiscRb)
{
    NS_LOG_FUNCTION(this);
    NrSlDiscoveryRbMapPerL2Id::iterator destIt =
        m_slTxDiscoveryRbMap.find(slTxDiscRb->m_destinationL2Id);

    if (destIt == m_slTxDiscoveryRbMap.end())
    {
        NS_LOG_LOGIC("First SL Discovery RB for destination " << slTxDiscRb->m_destinationL2Id);
        NrSlDiscRbMap mapPerDestL2Id;
        mapPerDestL2Id.insert(
            std::pair<uint32_t, Ptr<NrSlDiscoveryRadioBearerInfo>>(slTxDiscRb->m_sourceL2Id,
                                                                   slTxDiscRb));
        m_slTxDiscoveryRbMap.insert(
            std::pair<uint32_t, NrSlDiscRbMap>(slTxDiscRb->m_destinationL2Id, mapPerDestL2Id));
        NS_LOG_LOGIC("Added SL Discovery RB with source " << slTxDiscRb->m_sourceL2Id
                                                          << " for destination "
                                                          << slTxDiscRb->m_destinationL2Id);
    }
    else
    {
        NrSlDiscRbMap::iterator srcL2IdIt;
        srcL2IdIt = destIt->second.find(slTxDiscRb->m_sourceL2Id);
        if (srcL2IdIt == destIt->second.end())
        {
            // New bearer for the destination
            destIt->second.insert(
                std::pair<uint32_t, Ptr<NrSlDiscoveryRadioBearerInfo>>(slTxDiscRb->m_sourceL2Id,
                                                                       slTxDiscRb));
            NS_LOG_LOGIC("Added SL Discovery RB with source " << slTxDiscRb->m_sourceL2Id
                                                              << " for destination "
                                                              << slTxDiscRb->m_destinationL2Id);
        }
        else
        {
            NS_FATAL_ERROR("SL Discovery RB already exists for source "
                           << slTxDiscRb->m_sourceL2Id << " and destination "
                           << slTxDiscRb->m_destinationL2Id);
        }
    }
}

void
NrSlUeRrc::DoAddRxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slRxDiscRb)
{
    NS_LOG_FUNCTION(this);
    NrSlDiscoveryRbMapPerL2Id::iterator srcIt = m_slRxDiscoveryRbMap.find(slRxDiscRb->m_sourceL2Id);

    if (srcIt == m_slRxDiscoveryRbMap.end())
    {
        NS_LOG_LOGIC("First SL RX Discovery RB for peer UE with source L2 id "
                     << slRxDiscRb->m_sourceL2Id << " and destination L2 id "
                     << slRxDiscRb->m_destinationL2Id);

        NrSlDiscRbMap mapPerDestL2Id;
        mapPerDestL2Id.insert(
            std::pair<uint32_t, Ptr<NrSlDiscoveryRadioBearerInfo>>(slRxDiscRb->m_destinationL2Id,
                                                                   slRxDiscRb));
        m_slRxDiscoveryRbMap.insert(
            std::pair<uint32_t, NrSlDiscRbMap>(slRxDiscRb->m_sourceL2Id, mapPerDestL2Id));
    }
    else
    {
        NrSlDiscRbMap::iterator dstL2IdIt;
        dstL2IdIt = srcIt->second.find(slRxDiscRb->m_destinationL2Id);
        if (dstL2IdIt == srcIt->second.end())
        {
            // New Rx bearer for the destination L2 ID
            srcIt->second.insert(std::pair<uint32_t, Ptr<NrSlDiscoveryRadioBearerInfo>>(
                slRxDiscRb->m_destinationL2Id,
                slRxDiscRb));
            NS_LOG_LOGIC("Added RX SL-SRB with destination L2 ID " << slRxDiscRb->m_destinationL2Id
                                                                   << "for peer source "
                                                                   << slRxDiscRb->m_sourceL2Id);
        }
        else
        {
            NS_FATAL_ERROR("RX SL-SRB for peer source "
                           << slRxDiscRb->m_sourceL2Id << " and destination L2 ID = "
                           << slRxDiscRb->m_destinationL2Id << " already exists");
        }
    }
}

Ptr<NrSlDiscoveryRadioBearerInfo>
NrSlUeRrc::DoGetTxNrSlDiscoveryRadioBearer(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this);
    Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRb = nullptr;
    NrSlDiscoveryRbMapPerL2Id::iterator destIt = m_slTxDiscoveryRbMap.find(dstL2Id);
    uint32_t srcL2Id = DoGetSourceL2Id();

    if (destIt == m_slTxDiscoveryRbMap.end())
    {
        NS_FATAL_ERROR("Unable to find any SL discovery RB for destination L2 Id " << dstL2Id);
    }
    else
    {
        NS_LOG_LOGIC("Searching SL Discovery RB with source "
                     << srcL2Id << " for destination L2 Id " << dstL2Id);

        NrSlDiscRbMap::iterator srcL2IdIt;
        srcL2IdIt = destIt->second.find(srcL2Id);
        if (srcL2IdIt == destIt->second.end())
        {
            NS_FATAL_ERROR("SL Discovery RB with source "
                           << srcL2Id << " does not exist for destination L2 Id " << dstL2Id);
        }
        else
        {
            slDiscRb = srcL2IdIt->second;
            NS_LOG_LOGIC("Found SL discovery RB with source " << slDiscRb->m_sourceL2Id
                                                              << " for destination "
                                                              << slDiscRb->m_destinationL2Id);
        }
    }
    return slDiscRb;
}

} // namespace ns3
