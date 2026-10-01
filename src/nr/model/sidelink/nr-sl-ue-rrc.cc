/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   SPDX-License-Identifier: GPL-2.0-only
 */

#include "nr-sl-ue-rrc.h"

#include "nr-sl-comm-resource-pool.h"
#include "nr-sl-mac-sap.h"
#include "nr-sl-pdcp.h"
#include "nr-sl-radio-bearer-info.h"
#include "nr-sl-rlc-um.h"

#include "ns3/abort.h"
#include "ns3/fatal-error.h"
#include "ns3/log.h"
#include "ns3/nr-ue-rrc.h"
#include "ns3/object-factory.h"
#include "ns3/object-map.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <limits>
#include <unordered_map>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlUeRrc");
NS_OBJECT_ENSURE_REGISTERED(NrSlUeRrc);

// candidate for use of std::optional when SAP objects are eventually removed
static constexpr double INVALID_RSRP_DBM = -200;

TypeId
NrSlUeRrc::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlUeRrc")
            .SetParent<NrUeRrc>()
            .SetGroupName("Nr")
            .AddConstructor<NrSlUeRrc>()
            .AddAttribute("SignallingPacketDelayBudget",
                          "Packet Delay Budget to use for signalling LC",
                          TimeValue(MilliSeconds(20)), // Magic number; not in standard
                          MakeTimeAccessor(&NrSlUeRrc::m_signallingPdb),
                          MakeTimeChecker())
            .AddTraceSource("Drop",
                            // TODO: Add field to indicate drop cause
                            "Trace for packet send failure",
                            MakeTraceSourceAccessor(&NrSlUeRrc::m_dropTrace),
                            "ns3::NrSlUeRrc::DropTraceCallback");
    return tid;
}

NrSlUeRrc::NrSlUeRrc()
{
    NS_LOG_FUNCTION(this);
    m_nrSlAsSapProvider = new MemberNrSlAsSapProvider<NrSlUeRrc>(this);
    m_nrSlUeBwpmRrcSapUser = new MemberNrSlUeBwpmRrcSapUser<NrSlUeRrc>(this);
    m_nrSlUeCmacSapUser = new MemberNrSlUeCmacSapUser<NrSlUeRrc>(this);
    m_nrSlUeCphySapUser = new MemberNrSlUeCphySapUser<NrSlUeRrc>(this);
    m_nrSlPdcpSapUser = new MemberNrSlPdcpSapUser<NrSlUeRrc>(this);
    m_nrSlUeSvcRrcSapProvider = new MemberNrSlUeSvcRrcSapProvider<NrSlUeRrc>(this);
}

void
NrSlUeRrc::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_nrSlAsSapProvider;
    delete m_nrSlUeBwpmRrcSapUser;
    delete m_nrSlUeCmacSapUser;
    delete m_nrSlUeCphySapUser;
    delete m_nrSlPdcpSapUser;
    m_nrSlUeCmacSapProvider.erase(m_nrSlUeCmacSapProvider.begin(), m_nrSlUeCmacSapProvider.end());
    m_nrSlUeCmacSapProvider.clear();
    m_nrSlUeCphySapProvider.erase(m_nrSlUeCphySapProvider.begin(), m_nrSlUeCphySapProvider.end());
    m_nrSlUeCphySapProvider.clear();
    delete m_nrSlUeSvcRrcSapProvider;
    NrUeRrc::DoDispose();
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
    for (const auto it : m_slBwpIds)
    {
        m_nrSlUeCmacSapProvider.at(it)->SetSourceL2Id(srcL2Id);
    }
}

void
NrSlUeRrc::SetNrSlPreconfiguration(const NrSlRrcSap::SidelinkPreconfigNr& preconfiguration)
{
    NS_LOG_FUNCTION(this);
    m_preconfiguration = preconfiguration;
    m_tddPattern = ConvertTddPattern(m_preconfiguration.slPreconfigGeneral.slTddConfig.tddPattern);
    // Tell RRC to populate pools
    PopulateNrSlPools();
}

void
NrSlUeRrc::SetNrSlDiscoveryRelayConfiguration(const NrSlRrcSap::SlRelayUeConfig relayConfig)
{
    NS_LOG_FUNCTION(this);
    m_relayConfig = relayConfig;
}

void
NrSlUeRrc::SetNrSlDiscoveryRemoteConfiguration(const NrSlRrcSap::SlRemoteUeConfig remoteConfig)
{
    NS_LOG_FUNCTION(this);
    m_remoteConfig = remoteConfig;
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

void
NrSlUeRrc::AddNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb)
{
    NS_LOG_FUNCTION(this);
    auto destIt = m_slTxDrbMap.find(slTxDrb->m_destinationL2Id);
    if (destIt == m_slTxDrbMap.end())
    {
        NS_LOG_DEBUG("First SL DRB for destination " << slTxDrb->m_destinationL2Id);
        NrSlDrbMapPerLcId mapPerLcId;
        mapPerLcId.insert(
            std::pair<uint8_t, Ptr<NrSlDataRadioBearerInfo>>(slTxDrb->m_logicalChannelIdentity,
                                                             slTxDrb));
        NS_LOG_INFO("Add Tx DRB to " << slTxDrb->m_destinationL2Id << " LCID "
                                     << +slTxDrb->m_logicalChannelIdentity);
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
            NS_LOG_INFO("Add Tx DRB to " << slTxDrb->m_destinationL2Id << " LCID "
                                         << +slTxDrb->m_logicalChannelIdentity);
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
NrSlUeRrc::AddNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb)
{
    NS_LOG_FUNCTION(this);
    std::pair<uint32_t, uint32_t> key =
        std::make_pair(slRxDrb->m_sourceL2Id, slRxDrb->m_destinationL2Id);
    auto srcIt = m_slRxDrbMap.find(key);
    if (srcIt == m_slRxDrbMap.end())
    {
        NS_LOG_DEBUG("First SL RX DRB for this UE. Source L2 id " << slRxDrb->m_sourceL2Id
                                                                  << " Destination L2 id "
                                                                  << slRxDrb->m_destinationL2Id);
        NrSlDrbMapPerLcId mapPerRxLcId;
        NS_LOG_INFO("Add Rx DRB from " << slRxDrb->m_sourceL2Id << " to "
                                       << slRxDrb->m_destinationL2Id << " LCID "
                                       << +slRxDrb->m_logicalChannelIdentity);
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
            NS_LOG_INFO("Add Rx DRB from " << slRxDrb->m_sourceL2Id << " to "
                                           << slRxDrb->m_destinationL2Id << " LCID "
                                           << +slRxDrb->m_logicalChannelIdentity);
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
    auto destIt = m_slTxDrbMap.find(dstL2Id);
    if (destIt != m_slTxDrbMap.end())
    {
        return destIt->second;
    }
    else
    {
        NS_LOG_DEBUG("Unable to find DRB for destination L2 Id "
                     << dstL2Id << " size " << m_slTxDrbMap.size() << " src " << m_srcL2Id);
        return std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>>();
    }
}

void
NrSlUeRrc::RemoveNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb)
{
    NS_LOG_FUNCTION(this);

    NrSlDrbMapPerL2Id::iterator destIt = m_slTxDrbMap.find(slTxDrb->m_destinationL2Id);
    if (destIt != m_slTxDrbMap.end())
    {
        NS_LOG_DEBUG("SL TX DRB found for this destination ID " << slTxDrb->m_destinationL2Id);
        m_slTxDrbMap.erase(slTxDrb->m_destinationL2Id);
    }
}

void
NrSlUeRrc::RemoveNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb)
{
    NS_LOG_FUNCTION(this);
    std::pair<uint32_t, uint32_t> key =
        std::make_pair(slRxDrb->m_sourceL2Id, slRxDrb->m_destinationL2Id);
    auto srcIt = m_slRxDrbMap.find(key);
    if (srcIt != m_slRxDrbMap.end())
    {
        NS_LOG_DEBUG("SL RX DRB found for remote UE with source L2 id "
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
NrSlUeRrc::GetNextLcid(uint32_t dstL2Id)
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
NrSlUeRrc::AddTxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb)
{
    NS_LOG_FUNCTION(this);
    NrSlSrbMapPerL2Id::iterator destIt = m_slTxSrbMap.find(slSrb->m_destinationL2Id);
    if (destIt == m_slTxSrbMap.end())
    {
        NS_LOG_DEBUG("First SL-SRB for destination " << slSrb->m_destinationL2Id);
        NrSlSrbMapPerLcId mapPerLcId;
        mapPerLcId.insert(
            std::pair<uint8_t, Ptr<NrSlSignallingRadioBearerInfo>>(slSrb->m_logicalChannelIdentity,
                                                                   slSrb));
        m_slTxSrbMap.insert(
            std::pair<uint32_t, NrSlSrbMapPerLcId>(slSrb->m_destinationL2Id, mapPerLcId));
        NS_LOG_DEBUG("Added SL-SRB with LCID " << (uint16_t)slSrb->m_logicalChannelIdentity
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
            NS_LOG_DEBUG("Added SL-SRB with LCID " << slSrb->m_logicalChannelIdentity
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
NrSlUeRrc::AddRxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slRxSrb)
{
    NS_LOG_FUNCTION(this);
    NrSlSrbMapPerL2Id::iterator srcIt = m_slRxSrbMap.find(slRxSrb->m_sourceL2Id);
    if (srcIt == m_slRxSrbMap.end())
    {
        NS_LOG_DEBUG("First SL RX SRB for peer UE with source L2 id "
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
            NS_LOG_DEBUG("Added RX SL-SRB with LCID " << (uint16_t)slRxSrb->m_logicalChannelIdentity
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
NrSlUeRrc::GetTxNrSlSignallingRadioBearer(uint32_t dstL2Id, uint8_t lcId)
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
        NS_LOG_DEBUG("Searching SL-SRB with LCID " << (uint16_t)lcId << " for destination L2 Id "
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
            NS_LOG_DEBUG("Found SL-SRB with LCID " << (uint16_t)slSrb->m_logicalChannelIdentity
                                                   << " for destination "
                                                   << slSrb->m_destinationL2Id);
        }
    }
    return slSrb;
}

void
NrSlUeRrc::AddTxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slTxDiscRb)
{
    NS_LOG_FUNCTION(this);
    NrSlDiscoveryRbMapPerL2Id::iterator destIt =
        m_slTxDiscoveryRbMap.find(slTxDiscRb->m_destinationL2Id);

    if (destIt == m_slTxDiscoveryRbMap.end())
    {
        NS_LOG_DEBUG("First SL Discovery RB for destination " << slTxDiscRb->m_destinationL2Id);
        NrSlDiscRbMap mapPerDestL2Id;
        mapPerDestL2Id.insert(
            std::pair<uint32_t, Ptr<NrSlDiscoveryRadioBearerInfo>>(slTxDiscRb->m_sourceL2Id,
                                                                   slTxDiscRb));
        m_slTxDiscoveryRbMap.insert(
            std::pair<uint32_t, NrSlDiscRbMap>(slTxDiscRb->m_destinationL2Id, mapPerDestL2Id));
        NS_LOG_DEBUG("Added SL Discovery RB with source " << slTxDiscRb->m_sourceL2Id
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
            NS_LOG_DEBUG("Added SL Discovery RB with source " << slTxDiscRb->m_sourceL2Id
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
NrSlUeRrc::AddRxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slRxDiscRb)
{
    NS_LOG_FUNCTION(this);
    NrSlDiscoveryRbMapPerL2Id::iterator srcIt = m_slRxDiscoveryRbMap.find(slRxDiscRb->m_sourceL2Id);

    if (srcIt == m_slRxDiscoveryRbMap.end())
    {
        NS_LOG_DEBUG("First SL RX Discovery RB for peer UE with source L2 id "
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
            NS_LOG_DEBUG("Added RX SL-SRB with destination L2 ID " << slRxDiscRb->m_destinationL2Id
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
NrSlUeRrc::GetTxNrSlDiscoveryRadioBearer(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this);
    Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRb = nullptr;
    NrSlDiscoveryRbMapPerL2Id::iterator destIt = m_slTxDiscoveryRbMap.find(dstL2Id);
    uint32_t srcL2Id = m_srcL2Id;

    if (destIt == m_slTxDiscoveryRbMap.end())
    {
        NS_FATAL_ERROR("Unable to find any SL discovery RB for destination L2 Id " << dstL2Id);
    }
    else
    {
        NS_LOG_DEBUG("Searching SL Discovery RB with source "
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
            NS_LOG_DEBUG("Found SL discovery RB with source " << slDiscRb->m_sourceL2Id
                                                              << " for destination "
                                                              << slDiscRb->m_destinationL2Id);
        }
    }
    return slDiscRb;
}

void
NrSlUeRrc::SetNrSlAsSapUser(NrSlAsSapUser* s)
{
    m_nrSlAsSapUser = s;
}

NrSlAsSapProvider*
NrSlUeRrc::GetNrSlAsSapProvider()
{
    return m_nrSlAsSapProvider;
}

const std::set<uint8_t>
NrSlUeRrc::GetNrSlBwpIdContainer()
{
    return m_slBwpIds;
}

void
NrSlUeRrc::SetNrSlBwpIdContainerInBwpm()
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeBwpmRrcSapProvider->SetBwpIdContainer(m_slBwpIds);
}

void
NrSlUeRrc::SetNrSlUeBwpmRrcSapProvider(NrSlUeBwpmRrcSapProvider* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeBwpmRrcSapProvider = s;
}

NrSlUeBwpmRrcSapUser*
NrSlUeRrc::GetNrSlUeBwpmRrcSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlUeBwpmRrcSapUser;
}

void
NrSlUeRrc::SetNrSlUeCmacSapProvider(uint8_t bwpId, NrSlUeCmacSapProvider* s)
{
    NS_LOG_FUNCTION(this);
    if (m_nrSlUeCmacSapProvider.empty())
    {
        for (uint16_t n = 0; n < m_numberOfComponentCarriers; n++)
        {
            m_nrSlUeCmacSapProvider.push_back(nullptr);
        }
    }
    m_nrSlUeCmacSapProvider.at(bwpId) = s;
}

NrSlUeCmacSapUser*
NrSlUeRrc::GetNrSlUeCmacSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlUeCmacSapUser;
}

void
NrSlUeRrc::SetNrSlUeCphySapProvider(uint8_t bwpId, NrSlUeCphySapProvider* s)
{
    NS_LOG_FUNCTION(this);
    if (m_nrSlUeCphySapProvider.empty())
    {
        for (uint16_t n = 0; n < m_numberOfComponentCarriers; n++)
        {
            m_nrSlUeCphySapProvider.push_back(nullptr);
        }
    }
    m_nrSlUeCphySapProvider.at(bwpId) = s;
}

NrSlUeCphySapUser*
NrSlUeRrc::GetNrSlUeCphySapUser()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlUeCphySapUser;
}

void
NrSlUeRrc::SetNrSlPdcpSapProvider(NrSlPdcpSapProvider* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlPdcpSapProvider = s;
}

NrSlPdcpSapUser*
NrSlUeRrc::GetNrSlPdcpSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlPdcpSapUser;
}

void
NrSlUeRrc::SetNrSlMacSapProvider(NrSlMacSapProvider* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlMacSapProvider = s;
}

NrSlUeSvcRrcSapProvider*
NrSlUeRrc::GetNrSlUeSvcRrcSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlUeSvcRrcSapProvider;
}

void
NrSlUeRrc::SetNrSlUeSvcRrcSapUser(NrSlUeSvcRrcSapUser* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeSvcRrcSapUser = s;
}

void
NrSlUeRrc::DoReceiveNrSlPdcpSdu(const NrSlPdcpSapUser::NrSlReceivePdcpSduParameters& params)
{
    NS_LOG_FUNCTION(this);
    if (params.lcId < 4)
    {
        // Signalling, pass it to the service layer
        m_nrSlUeSvcRrcSapUser->ReceiveNrSlSignalling(params.pdcpSdu, params.srcL2Id);
    }
    else if (params.lcId == 4)
    {
        // Discovery, pass it to the service layer
        m_nrSlUeSvcRrcSapUser->ReceiveNrSlDiscovery(params.pdcpSdu, params.srcL2Id);
    }
    else
    {
        // Data, pass it to the AS layer
        if (m_nrSlUeSvcRrcSapUser)
        {
            if (m_nrSlUeSvcRrcSapUser->NotifyDataReceived(params.srcL2Id))
            {
                m_asSapUser->RecvData(params.pdcpSdu);
            }
        }
        else
        {
            m_asSapUser->RecvData(params.pdcpSdu);
        }
    }
}

void
NrSlUeRrc::DoSendSidelinkData(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this << packet << " for NR Sidelink. destination layer 2 id " << dstL2Id
                         << " lcId " << +lcId);
    if (m_nrSlUeSvcRrcSapUser)
    {
        auto confirmSendRequest = m_nrSlUeSvcRrcSapUser->ConfirmSendRequest(packet, dstL2Id, lcId);
        if (!confirmSendRequest)
        {
            NS_LOG_INFO("Dropping packet (lack of direct link) to " << dstL2Id << " for LC ID "
                                                                    << +lcId);
            m_dropTrace(packet, dstL2Id, lcId);
            return;
        }
    }
    // Find the PDCP for NR Sidelink transmission
    Ptr<NrSlDataRadioBearerInfo> slDrb = GetSidelinkTxDataRadioBearer(m_srcL2Id, dstL2Id, lcId);

    // If there are multiple bearers, hence, multiple LCs, for a destination the
    // the NAS layer should be aware about this. That is, it should give RRC a
    // bearer id, and RRC should map it to a right LC.
    NS_ASSERT_MSG(slDrb, "could not find Sidelink data radio bearer for remote = " << dstL2Id);

    auto params =
        NrSlPdcpSapProvider::NrSlTransmitPdcpSduParameters(packet,
                                                           m_rnti,
                                                           slDrb->m_logicalChannelIdentity,
                                                           NrSlPdcpSapUser::IP_SDU);

    NS_LOG_DEBUG("RNTI=" << m_rnti << " sending packet " << packet
                         << " on NR SL DRB for destination " << dstL2Id << " (LCID "
                         << (uint32_t)params.lcId << ")"
                         << " (" << packet->GetSize() << " bytes)");

    slDrb->m_pdcp->GetObject<NrSlPdcp>()->GetNrSlPdcpSapProvider()->TransmitNrSlPdcpSdu(params);
}

void
NrSlUeRrc::DoActivateNrSlRadioBearer(bool isTransmit,
                                     bool isReceive,
                                     const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this << isTransmit << isReceive);
    ActivateNrSlDrb(isTransmit, isReceive, slInfo);
}

void
NrSlUeRrc::ActivateNrSlDrb(bool isTransmit, bool isReceive, const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this << isTransmit << isReceive << slInfo.m_srcL2Id << slInfo.m_dstL2Id
                         << slInfo.m_lcId);

    // Associate this RRC entity's source L2 ID with the sidelink info that
    // was passed in (possibly with an uninitialized srcL2Id field)
    struct SidelinkInfo slInfoWithSrcId(slInfo);
    slInfoWithSrcId.m_srcL2Id = m_srcL2Id;

    switch (m_state)
    {
    case IDLE_START:
    case IDLE_CELL_SEARCH:
    case IDLE_WAIT_MIB_SIB1:
    case IDLE_WAIT_MIB:
    case IDLE_WAIT_SIB1:
    case IDLE_CAMPED_NORMALLY:

        NS_LOG_INFO("Considering IMSI " << m_imsi << " out of network");

        if (m_rnti == RNTI_UNASSIGNED)
        {
            SetOutofCovrgUeRnti();
        }

        // Table 6.2.4-1 Values of LCID for SL-SCH of 38.321 and LCG from 36.331

        if (isTransmit)
        {
            Ptr<NrSlDataRadioBearerInfo> slDrbInfo =
                AddNrSlTxDrb(slInfoWithSrcId.m_dstL2Id,
                             GetNextLcid(slInfoWithSrcId.m_dstL2Id),
                             slInfoWithSrcId);
            slInfoWithSrcId.m_lcId = slDrbInfo->m_logicalChannelIdentity;
            NS_LOG_INFO("Created new TX SLRB for remote id "
                        << slInfoWithSrcId.m_dstL2Id
                        << " LCID = " << +slDrbInfo->m_logicalChannelIdentity);
        }

        if ((isTransmit && isReceive) || isReceive)
        {
            // Do not call AddNrSlRxDrb() here; receive DRB will be added upon packet reception
            for (const auto& it : m_slBwpIds)
            {
                NS_LOG_INFO("Communicating Rx destination to the MAC of SL BWP "
                            << static_cast<uint16_t>(it));
                m_nrSlUeCmacSapProvider.at(it)->AddNrSlRxDstL2Id(slInfoWithSrcId.m_dstL2Id);
            }
        }

        // Notify NAS
        m_nrSlAsSapUser->NotifyNrSlRadioBearerActivated(slInfoWithSrcId);
        break;

    case IDLE_WAIT_SIB2:
    case IDLE_CONNECTING:
        NS_LOG_INFO("Connecting, must wait to send message");
        break;

    case CONNECTED_NORMALLY:
    case CONNECTED_HANDOVER:
    case CONNECTED_PHY_PROBLEM:
    case CONNECTED_REESTABLISHING:
        NS_LOG_INFO("Considering IMSI " << m_imsi << "RNTI " << m_rnti << " in coverage");
        /*
         * Currently, in-network UEs doing SL rely on preconfiguration.
         * This part of the function should be modified/extended when the
         * network will be involved in SL configuration and protocols.
         */

        // We are going to reuse RNTI assigned by the network for SL.
        {
            for (const auto& it : m_slBwpIds)
            {
                NS_LOG_INFO("Communicating RNTI " << m_rnti << " to PHY and MAC in  in BWP "
                                                  << +it);
                m_cphySapProvider.at(it)->SetRnti(m_rnti);
                m_cmacSapProvider.at(it)->SetRnti(m_rnti);
            }
        }
        // We use same SL-DRB creation and configuration logic than OOC
        if (isTransmit)
        {
            Ptr<NrSlDataRadioBearerInfo> slDrbInfo =
                AddNrSlTxDrb(slInfoWithSrcId.m_dstL2Id,
                             GetNextLcid(slInfoWithSrcId.m_dstL2Id),
                             slInfoWithSrcId);
            slInfoWithSrcId.m_lcId = slDrbInfo->m_logicalChannelIdentity;
            NS_LOG_INFO("Created new TX SL-DRB for dstL2id "
                        << slInfo.m_dstL2Id << " LCID = " << +slDrbInfo->m_logicalChannelIdentity);
        }

        if ((isTransmit && isReceive) || isReceive)
        {
            Ptr<NrSlDataRadioBearerInfo> slDrbInfo = AddNrSlRxDrb(slInfoWithSrcId.m_srcL2Id,
                                                                  slInfoWithSrcId.m_dstL2Id,
                                                                  slInfoWithSrcId.m_lcId);
            for (const auto& it : m_slBwpIds)
            {
                NS_LOG_INFO("Communicating Rx destination to the MAC of SL BWP "
                            << static_cast<uint16_t>(it));
                m_nrSlUeCmacSapProvider.at(it)->AddNrSlRxDstL2Id(slInfoWithSrcId.m_dstL2Id);
            }
        }

        // Notify NAS
        m_nrSlAsSapUser->NotifyNrSlRadioBearerActivated(slInfoWithSrcId);
        break;

    default: // i.e. IDLE_RANDOM_ACCESS
        NS_FATAL_ERROR("method unexpected in state " << ToString(m_state));
        break;
    }
}

void
NrSlUeRrc::SetOutofCovrgUeRnti()
{
    NS_LOG_FUNCTION(this);
    // preconfigure the RNTI to the IMSI's 16 LSB for uniqueness
    auto rnti = static_cast<uint16_t>(m_imsi & 0xFFFF);
    m_rnti = rnti;
    NS_LOG_DEBUG("Assigning RNTI to an out-of-coverage SL UE " << m_rnti);
    for (const auto& it : m_slBwpIds)
    {
        m_cphySapProvider.at(it)->SetRnti(m_rnti);
        m_cmacSapProvider.at(it)->SetRnti(m_rnti);
    }
}

void
NrSlUeRrc::DoNotifySidelinkReception(uint8_t lcId,
                                     uint32_t srcL2Id,
                                     uint32_t dstL2Id,
                                     uint8_t castType,
                                     bool harqEnabled)
{
    NS_LOG_FUNCTION(this << (uint16_t)lcId << srcL2Id << dstL2Id << static_cast<uint16_t>(castType)
                         << harqEnabled);
    // add LC
    if (lcId < 4)
    {
        // SL-SRB
        struct SidelinkInfo slInfo;
        slInfo.m_srcL2Id = srcL2Id;
        slInfo.m_dstL2Id = dstL2Id;
        slInfo.m_lcId = lcId;
        slInfo.m_castType = static_cast<SidelinkInfo::CastType>(castType);
        slInfo.m_dynamic = true;
        slInfo.m_pdb = m_signallingPdb;
        slInfo.m_priority = 1; // SL-SRBs have priority 1 (TS 38.331 9.1.1.4)
        Ptr<NrSlSignallingRadioBearerInfo> slbInfo = AddNrSlSrb(srcL2Id, slInfo);
        NS_LOG_INFO("Created new RX SL-SRB for dstL2Id "
                    << dstL2Id << " LCID=" << (slbInfo->m_logicalChannelIdentity & 0xF));
    }
    else if (lcId == 4)
    {
        // Discovery
        Ptr<NrSlDiscoveryRadioBearerInfo> slbInfo = AddNrSlDiscoveryRb(srcL2Id, dstL2Id);
        NS_LOG_INFO("Created new RX SL Discovery RB for dstL2Id "
                    << dstL2Id << " LCID=" << (slbInfo->m_logicalChannelIdentity & 0xF));
    }
    else
    {
        // SL-DRB
        Ptr<NrSlDataRadioBearerInfo> slbInfo = AddNrSlRxDrb(srcL2Id, dstL2Id, lcId);
        NS_LOG_INFO("Created new RX SLRB for group "
                    << dstL2Id << " LCID=" << (slbInfo->m_logicalChannelIdentity & 0xF));
    }
}

void
NrSlUeRrc::DoNotifySlHarqProcessMaxTxWithNoFeedback(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);

    m_nrSlUeSvcRrcSapUser->NotifyNrSlHarqProcessMaxTransmissionsWithNoFeedback(dstL2Id);
}

Ptr<NrSlDataRadioBearerInfo>
NrSlUeRrc::AddNrSlTxDrb(uint32_t dstL2Id, uint8_t lcid, const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this << dstL2Id << +lcid);

    NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo lcInfo;
    lcInfo.srcL2Id = m_srcL2Id;
    lcInfo.dstL2Id = dstL2Id;
    lcInfo.lcId = lcid;
    lcInfo.lcGroup = 0; // Current scheduler handles only one LCG, all LCGs across different types
                        // of bearers should be the same
    lcInfo.priority = slInfo.m_priority;
    lcInfo.castType = slInfo.m_castType;
    lcInfo.harqEnabled = slInfo.m_harqEnabled;
    lcInfo.pdb = slInfo.m_pdb;
    lcInfo.dynamic = slInfo.m_dynamic;
    lcInfo.rri = slInfo.m_rri;
    // following parameters have no impact at the moment
    // GBR Mission Critical User Plane Push To Talk voice TS 23.501 Table 5.7.4-1
    lcInfo.pqi = 65;
    lcInfo.isGbr = true;
    lcInfo.gbr = 65535; // bits/s random value
    lcInfo.mbr = lcInfo.gbr;

    Ptr<NrSlDataRadioBearerInfo> slDrbInfo = CreateObject<NrSlDataRadioBearerInfo>();
    slDrbInfo->m_sourceL2Id = lcInfo.srcL2Id;
    slDrbInfo->m_destinationL2Id = lcInfo.dstL2Id;
    slDrbInfo->m_logicalChannelIdentity = lcInfo.lcId;
    slDrbInfo->m_logicalChannelConfig.logicalChannelGroup = lcInfo.lcGroup;
    slDrbInfo->m_logicalChannelConfig.priority = lcInfo.priority;
    slDrbInfo->m_logicalChannelConfig.prioritizedBitRateKbps = lcInfo.gbr;
    slDrbInfo->m_logicalChannelConfig.bucketSizeDurationMs = 1000; // Check this value \todo

    AddNrSlTxDataRadioBearer(slDrbInfo);
    return (FinishSlDrbConfiguration(slDrbInfo, lcInfo));
}

Ptr<NrSlDataRadioBearerInfo>
NrSlUeRrc::AddNrSlRxDrb(uint32_t srcL2Id, uint32_t dstL2Id, uint8_t lcid)
{
    NS_LOG_FUNCTION(this << srcL2Id << dstL2Id << +lcid);

    NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo lcInfo;
    lcInfo.srcL2Id = srcL2Id;
    lcInfo.dstL2Id = dstL2Id;
    lcInfo.lcId = lcid;
    lcInfo.lcGroup = 0; // Current scheduler handles only one LCG, all LCGs across different
                        // types of bearers should be the same
    // following parameters have no impact at the moment
    // GBR Mission Critical User Plane Push To Talk voice TS 23.501 Table 5.7.4-1
    lcInfo.priority = 7;
    lcInfo.pqi = 65;
    lcInfo.isGbr = true;
    lcInfo.gbr = 65535; // bits/s random value
    lcInfo.mbr = lcInfo.gbr;
    lcInfo.pdb = m_signallingPdb;
    lcInfo.dynamic = true;

    Ptr<NrSlDataRadioBearerInfo> slDrbInfo = CreateObject<NrSlDataRadioBearerInfo>();
    slDrbInfo->m_sourceL2Id = lcInfo.srcL2Id;
    slDrbInfo->m_destinationL2Id = lcInfo.dstL2Id;
    slDrbInfo->m_logicalChannelIdentity = lcInfo.lcId;
    slDrbInfo->m_logicalChannelConfig.logicalChannelGroup = lcInfo.lcGroup;
    slDrbInfo->m_logicalChannelConfig.priority = lcInfo.priority;
    slDrbInfo->m_logicalChannelConfig.prioritizedBitRateKbps = lcInfo.gbr;
    slDrbInfo->m_logicalChannelConfig.bucketSizeDurationMs = 1000; // Check this value \todo
    AddNrSlRxDataRadioBearer(slDrbInfo);
    return (FinishSlDrbConfiguration(slDrbInfo, lcInfo));
}

Ptr<NrSlDataRadioBearerInfo>
NrSlUeRrc::FinishSlDrbConfiguration(Ptr<NrSlDataRadioBearerInfo> slDrbInfo,
                                    const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& lcInfo)
{
    NS_LOG_FUNCTION(this << slDrbInfo << lcInfo);

    // create PDCP/RLC stack
    ObjectFactory rlcObjectFactory;
    rlcObjectFactory.SetTypeId(NrSlRlcUm::GetTypeId());
    Ptr<NrSlRlc> rlc = rlcObjectFactory.Create()->GetObject<NrSlRlc>();
    rlc->SetNrSlMacSapProvider(m_nrSlMacSapProvider);
    rlc->SetRnti(m_rnti);
    rlc->SetLcId(slDrbInfo->m_logicalChannelIdentity);
    rlc->SetSourceL2Id(slDrbInfo->m_sourceL2Id);
    rlc->SetDestinationL2Id(slDrbInfo->m_destinationL2Id);
    rlc->SetRlcChannelType(NrSlRlc::STCH);
    rlc->SetPacketDelayBudgetMs(lcInfo.pdb.GetMilliSeconds());

    slDrbInfo->m_rlc = rlc;

    Ptr<NrSlPdcp> pdcp = CreateObject<NrSlPdcp>();
    pdcp->SetRnti(m_rnti);
    pdcp->SetLcId(slDrbInfo->m_logicalChannelIdentity);
    pdcp->SetSourceL2Id(slDrbInfo->m_sourceL2Id);
    pdcp->SetDestinationL2Id(slDrbInfo->m_destinationL2Id);
    pdcp->SetNrSlPdcpSapUser(m_nrSlPdcpSapUser);
    pdcp->SetNrSlRlcSapProvider(rlc->GetNrSlRlcSapProvider());
    rlc->SetNrSlRlcSapUser(pdcp->GetNrSlRlcSapUser());

    slDrbInfo->m_pdcp = pdcp;

    // Call AddNrSlDrbLc of NR sidelink BWP manager
    std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm> slLcOnBwpMapping;
    slLcOnBwpMapping = m_nrSlUeBwpmRrcSapProvider->AddNrSlDrbLc(lcInfo, rlc->GetNrSlMacSapUser());

    uint8_t numOfBwpsByBwpm = slLcOnBwpMapping.size();

    NS_ASSERT_MSG(numOfBwpsByBwpm != 0, "SL BWP manager failed to add SL LC for SL radio bearer");

    uint8_t numOfSlBwps = m_slBwpIds.size();
    NS_ASSERT_MSG(
        numOfSlBwps == numOfBwpsByBwpm,
        " Bwp manager configured SL LC for incorrect number of SL BWPs : " << numOfBwpsByBwpm);

    NS_LOG_DEBUG("Size of slLcOnBwpMapping vector " << slLcOnBwpMapping.size());

    std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm>::iterator itSlLcOnBwpMapping;

    for (itSlLcOnBwpMapping = slLcOnBwpMapping.begin();
         itSlLcOnBwpMapping != slLcOnBwpMapping.end();
         ++itSlLcOnBwpMapping)
    {
        NS_LOG_DEBUG("RNTI " << m_rnti << " LCG id " << +itSlLcOnBwpMapping->lcInfo.lcGroup
                             << " BWP Id " << +itSlLcOnBwpMapping->bwpId);
        uint8_t bwpId = itSlLcOnBwpMapping->bwpId;
        m_nrSlUeCmacSapProvider.at(bwpId)->AddNrSlLc(itSlLcOnBwpMapping->lcInfo,
                                                     itSlLcOnBwpMapping->msu);
    }

    return slDrbInfo;
}

void
NrSlUeRrc::DoNotifySidelinkConnectionRelease(uint32_t srcL2Id, uint32_t dstL2Id, uint8_t lcid)
{
    NS_LOG_FUNCTION(this << srcL2Id << dstL2Id << +lcid);

    // Removing Rx Sl-DRB
    RemoveNrSlDataRadioBearer(false, srcL2Id, dstL2Id, lcid);
    NS_LOG_INFO("Removed Rx SL-DRB related to srcL2Id " << srcL2Id << " lcid " << +lcid);
}

void
NrSlUeRrc::DoDeleteNrSlDataRadioBearer(bool isTransmit,
                                       bool isReceive,
                                       const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this << isTransmit << isReceive);
    if (isTransmit)
    {
        // Only one bearer per source/destinationm
        RemoveNrSlDataRadioBearer(true, m_srcL2Id, slInfo.m_dstL2Id, slInfo.m_lcId);
        NS_LOG_INFO("Removed existing TX SL-DRB for dstL2Id " << slInfo.m_dstL2Id << " lcid "
                                                              << +slInfo.m_lcId);
    }

    if ((isTransmit && isReceive) || isReceive)
    {
        for (const auto& it : m_slBwpIds)
        {
            NS_LOG_INFO("Removing Rx destination from the MAC of SL BWP "
                        << static_cast<uint16_t>(it));
            m_nrSlUeCmacSapProvider.at(it)->RemoveNrSlRxDstL2Id(slInfo.m_dstL2Id);
        }
    }

    // Notify NAS
    m_nrSlAsSapUser->NotifyNrSlRadioBearerRemoved(slInfo);
}

void
NrSlUeRrc::RemoveNrSlDataRadioBearer(bool isTransmit,
                                     uint32_t srcL2Id,
                                     uint32_t dstL2Id,
                                     uint8_t lcid)
{
    NS_LOG_FUNCTION(this << isTransmit << srcL2Id << dstL2Id << lcid);

    if (isTransmit)
    {
        NS_LOG_DEBUG("Transmission bearer");
        auto bearers = GetAllSidelinkTxDataRadioBearers(dstL2Id);
        for (auto i : bearers)
        {
            Ptr<NrSlDataRadioBearerInfo> slTxDrb = i.second;
            lcid = slTxDrb->m_logicalChannelIdentity;
            NS_LOG_DEBUG("Found txDrb with lcId " << +lcid);
            RemoveNrSlTxDataRadioBearer(slTxDrb);

            // Inform BWP manager and MAC logical channel
            std::vector<uint8_t> bwpIds;
            bwpIds = m_nrSlUeBwpmRrcSapProvider->RemoveNrSlDrbLc(lcid, srcL2Id, dstL2Id);
            for (std::vector<uint8_t>::iterator it = bwpIds.begin(); it != bwpIds.end(); ++it)
            {
                m_nrSlUeCmacSapProvider.at(*it)->RemoveNrSlLc(lcid, srcL2Id, dstL2Id);
            }
        }
    }
    else
    {
        auto bearers = GetAllSidelinkRxDataRadioBearers(srcL2Id);
        for (auto i : bearers)
        {
            Ptr<NrSlDataRadioBearerInfo> slRxDrb = i.second;
            lcid = slRxDrb->m_logicalChannelIdentity;
            NS_LOG_DEBUG("Found rxDrb with lcId " << +lcid);
            RemoveNrSlRxDataRadioBearer(slRxDrb);

            // Inform BWP manager and MAC logical channel
            std::vector<uint8_t> bwpIds;
            bwpIds = m_nrSlUeBwpmRrcSapProvider->RemoveNrSlDrbLc(lcid, srcL2Id, dstL2Id);
            for (std::vector<uint8_t>::iterator it = bwpIds.begin(); it != bwpIds.end(); ++it)
            {
                m_nrSlUeCmacSapProvider.at(*it)->RemoveNrSlLc(lcid, srcL2Id, dstL2Id);
            }
        }
    }
}

void
NrSlUeRrc::PopulateNrSlPools()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_DEBUG("Adding pool for IMSI " << m_imsi << " srcL2 Id " << m_srcL2Id);

    std::array<NrSlRrcSap::SlBwpConfigCommonNr, MAX_NUM_OF_SL_BWPs> slBwpList;
    slBwpList = m_preconfiguration.slPreconfigFreqInfoList[0].slBwpList;

    Ptr<NrSlCommResourcePool> slPool; // this pointer would be communicated to the MAC and PHY

    // UE RRC will set these maps in NrSlCommResourcePool
    std::unordered_map<uint8_t, std::unordered_map<uint16_t, std::vector<std::bitset<1>>>>
        mapPerBwp;
    std::unordered_map<uint16_t, std::vector<std::bitset<1>>> mapPerPool;

    for (uint32_t index = 0; index < slBwpList.size(); ++index)
    {
        // index of slBwpList is used as BWP id
        // send SL pool to only that BWP for which SlBwpGeneric and SlBwpPoolConfigCommonNr are
        // configured.
        if (slBwpList[index].haveSlBwpGeneric && slBwpList[index].haveSlBwpPoolConfigCommonNr)
        {
            auto it = m_slBwpIds.find(index);
            NS_ASSERT_MSG(it != m_slBwpIds.end(),
                          "UE is not prepared to use BWP id " << +index << " for SL");

            std::array<NrSlRrcSap::SlResourcePoolConfigNr, MAX_NUM_OF_TX_POOL> txPoolList;
            txPoolList = slBwpList[index].slBwpPoolConfigCommonNr.slTxPoolSelectedNormal;
            for (const auto& it : txPoolList) // fill the map per pool
            {
                if (it.haveSlResourcePoolConfigNr) // if this true, it means pools are set
                {
                    // check if subchannel size in RBs is less or equal to the total
                    // available BW is RBs
                    uint16_t sbChSizeInRbs =
                        NrSlRrcSap::GetNrSlSubChSizeValue(it.slResourcePool.slSubchannelSize);
                    uint32_t bwInRbs = m_nrSlUeCphySapProvider.at(index)->GetBwInRbs();
                    NS_ASSERT_MSG(sbChSizeInRbs <= bwInRbs,
                                  "Incorrect Subchannel size of "
                                      << sbChSizeInRbs
                                      << " RBs. Must be less or equal to the available BW of "
                                      << bwInRbs << " RBs");
                    uint16_t numPscchRbs = NrSlRrcSap::GetSlFResoPscchValue(
                        it.slResourcePool.slPscchConfig.slFreqResourcePscch);
                    NS_ASSERT_MSG(numPscchRbs <= sbChSizeInRbs,
                                  "Incorrect number of PSCCH RBs : "
                                      << numPscchRbs
                                      << " . Must be less or equal to the SL subchannel size of "
                                      << sbChSizeInRbs << " RBs");
                    std::vector<std::bitset<1>> physicalPool =
                        GetPhysicalSlPool(it.slResourcePool.slTimeResource, m_tddPattern);
                    mapPerPool.emplace(it.slResourcePoolId.id, physicalPool);
                }
            }

            NS_ASSERT_MSG(!mapPerPool.empty(), "No SL pool set for BWP " << +index);

            mapPerBwp.emplace(index, mapPerPool);
        }

        if (!mapPerBwp.empty()) // we found SL BWP
        {
            slPool = CreateObject<NrSlCommResourcePool>();
            // set the slPreconfigFreqInfoList
            slPool->SetNrSlPreConfigFreqInfoList(m_preconfiguration.slPreconfigFreqInfoList);
            // set the PhysicalSlPoolMap
            slPool->SetNrSlPhysicalPoolMap(mapPerBwp);
            slPool->SetNrSlSchedulingType(NrSlCommResourcePool::UE_SELECTED);

            NS_LOG_INFO("Configuring TX pool for BWP " << +index << " IMSI " << m_imsi);
            m_nrSlUeCmacSapProvider.at(index)->AddNrSlCommTxPool(slPool);
            m_nrSlUeCmacSapProvider.at(index)->SetSlProbResourceKeep(
                m_preconfiguration.slUeSelectedPreConfig.slProbResourceKeep);
            m_nrSlUeCmacSapProvider.at(index)->SetSlMaxTxTransNumPssch(
                m_preconfiguration.slUeSelectedPreConfig.slPsschTxConfigList.slPsschTxParameters
                    .at(0)
                    .slMaxTxTransNumPssch);
            m_nrSlUeCphySapProvider.at(index)->AddNrSlCommTxPool(slPool);

            NS_LOG_INFO("Configuring RX pool for BWP " << +index << " IMSI " << m_imsi);
            m_nrSlUeCmacSapProvider.at(index)->AddNrSlCommRxPool(slPool);
            m_nrSlUeCphySapProvider.at(index)->AddNrSlCommRxPool(slPool);
            mapPerPool.clear();
            mapPerBwp.clear();
        }
    }
}

void
NrSlUeRrc::DoMonitorSelfL2Id()
{
    NS_LOG_FUNCTION(this);

    for (const auto& it : m_slBwpIds)
    {
        NS_LOG_INFO("Communicating Self L2Id (" << m_srcL2Id << ") to the MAC of SL BWP "
                                                << static_cast<uint16_t>(it));
        m_nrSlUeCmacSapProvider.at(it)->AddNrSlRxDstL2Id(m_srcL2Id);
    }
}

void
NrSlUeRrc::DoMonitorL2Id(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this);

    for (const auto& it : m_slBwpIds)
    {
        NS_LOG_INFO("Communicating L2Id (" << dstL2Id << ") to the MAC of SL BWP "
                                           << static_cast<uint16_t>(it));
        m_nrSlUeCmacSapProvider.at(it)->AddNrSlRxDstL2Id(dstL2Id);
    }
}

void
NrSlUeRrc::DoSendNrSlSignalling(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this);

    Ptr<NrSlSignallingRadioBearerInfo> slSrbAf = GetTxNrSlSignallingRadioBearer(dstL2Id, lcId);

    NS_LOG_INFO("dstL2Id " << slSrbAf->m_destinationL2Id << " lcId "
                           << (uint32_t)slSrbAf->m_logicalChannelIdentity << " lcGroup "
                           << (uint32_t)slSrbAf->m_logicalChannelConfig.logicalChannelGroup);

    auto params =
        NrSlPdcpSapProvider::NrSlTransmitPdcpSduParameters(packet,
                                                           m_rnti,
                                                           slSrbAf->m_logicalChannelIdentity,
                                                           NrSlPdcpSapUser::PC5_SIGNALING_SDU);
    slSrbAf->m_pdcp->GetObject<NrSlPdcp>()->GetNrSlPdcpSapProvider()->TransmitNrSlPdcpSdu(params);
}

void
NrSlUeRrc::DoActivateNrSlSignallingRadioBearer(const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this << slInfo.m_dstL2Id << slInfo.m_lcId);

    ActivateNrSlSrb(slInfo);
}

void
NrSlUeRrc::ActivateNrSlSrb(const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this << slInfo.m_dstL2Id << slInfo.m_lcId);
    Ptr<NrSlSignallingRadioBearerInfo> slSrb;

    switch (m_state)
    {
    case IDLE_START:
    case IDLE_CELL_SEARCH:
    case IDLE_WAIT_MIB_SIB1:
    case IDLE_WAIT_MIB:
    case IDLE_WAIT_SIB1:
    case IDLE_CAMPED_NORMALLY:

        NS_LOG_INFO("Considering IMSI " << m_imsi << " out of network");

        if (m_rnti == RNTI_UNASSIGNED)
        {
            SetOutofCovrgUeRnti();
        }
        slSrb = AddNrSlSrb(m_srcL2Id, slInfo);
        NS_LOG_INFO("Inserted, dstL2Id "
                    << slSrb->m_destinationL2Id << " lcId "
                    << (uint32_t)slSrb->m_logicalChannelIdentity << " lcGroup "
                    << (uint32_t)slSrb->m_logicalChannelConfig.logicalChannelGroup);
        break;

    case IDLE_WAIT_SIB2:
    case IDLE_CONNECTING:
        NS_LOG_INFO("Connecting, must wait to send message");
        break;

    case CONNECTED_NORMALLY:
    case CONNECTED_HANDOVER:
    case CONNECTED_PHY_PROBLEM:
    case CONNECTED_REESTABLISHING:

        NS_LOG_INFO("Considering IMSI " << m_imsi << " RNTI " << m_rnti << " in coverage");
        /**
         * Currently, in-network UEs doing SL rely on preconfiguration.
         * This part of the function should be modified/extended when the
         * network will be involved in SL configuration and protocols.
         */

        // We are going to reuse RNTI assigned by the network.
        {
            for (const auto& it : m_slBwpIds)
            {
                NS_LOG_INFO("Communicating RNTI " << m_rnti << " to PHY and MAC in  in BWP "
                                                  << +it);
                m_cphySapProvider.at(it)->SetRnti(m_rnti);
                m_cmacSapProvider.at(it)->SetRnti(m_rnti);
            }
        }

        // We use same SL-SRB creation and configuration logic as OOC
        slSrb = AddNrSlSrb(m_srcL2Id, slInfo);
        NS_LOG_INFO("Inserted, dstL2Id "
                    << slSrb->m_destinationL2Id << " lcId "
                    << (uint32_t)slSrb->m_logicalChannelIdentity << " lcGroup "
                    << (uint32_t)slSrb->m_logicalChannelConfig.logicalChannelGroup);

        break;

    default: // i.e. IDLE_RANDOM_ACCESS
        NS_FATAL_ERROR("method unexpected in state " << ToString(m_state));
        break;
    }
}

Ptr<NrSlSignallingRadioBearerInfo>
NrSlUeRrc::AddNrSlSrb(uint32_t srcL2Id, const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this);

    NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo lcInfo;
    lcInfo.srcL2Id = srcL2Id;
    lcInfo.dstL2Id = slInfo.m_dstL2Id;
    lcInfo.lcId = slInfo.m_lcId;
    lcInfo.lcGroup = 0; // SL-SRBs belong to lcGroup 0 (TS 38.331 9.1.1.4)
    lcInfo.pdb = slInfo.m_pdb;
    lcInfo.dynamic = slInfo.m_dynamic;
    lcInfo.rri = slInfo.m_rri;
    lcInfo.harqEnabled = slInfo.m_harqEnabled;
    lcInfo.castType = slInfo.m_castType;
    // following parameters have no impact at the moment
    lcInfo.priority = 1; // SL-SRBs have priority 1 (TS 38.331 9.1.1.4)
    lcInfo.pqi = 65;
    lcInfo.isGbr = true;
    lcInfo.gbr = 65535; // bits/s random value
    lcInfo.mbr = lcInfo.gbr;

    Ptr<NrSlSignallingRadioBearerInfo> slSrbInfo = CreateObject<NrSlSignallingRadioBearerInfo>();
    slSrbInfo->m_sourceL2Id = lcInfo.srcL2Id;
    slSrbInfo->m_destinationL2Id = lcInfo.dstL2Id;
    slSrbInfo->m_logicalChannelIdentity = lcInfo.lcId;
    slSrbInfo->m_logicalChannelConfig.logicalChannelGroup = lcInfo.lcGroup;
    slSrbInfo->m_logicalChannelConfig.priority = lcInfo.priority;
    slSrbInfo->m_logicalChannelConfig.prioritizedBitRateKbps = lcInfo.gbr;
    slSrbInfo->m_logicalChannelConfig.bucketSizeDurationMs = 1000; // Check this value \todo

    if (m_srcL2Id == srcL2Id)
    {
        // Bearer for transmission
        AddTxNrSlSignallingRadioBearer(slSrbInfo);
    }
    else
    {
        // Bearer for reception
        AddRxNrSlSignallingRadioBearer(slSrbInfo);
    }

    // create PDCP/RLC stack
    ObjectFactory rlcObjectFactory;
    rlcObjectFactory.SetTypeId(NrSlRlcUm::GetTypeId());
    Ptr<NrSlRlc> rlc = rlcObjectFactory.Create()->GetObject<NrSlRlc>();
    rlc->SetNrSlMacSapProvider(m_nrSlMacSapProvider);
    rlc->SetRnti(m_rnti);
    rlc->SetLcId(slSrbInfo->m_logicalChannelIdentity);
    rlc->SetSourceL2Id(slSrbInfo->m_sourceL2Id);
    rlc->SetDestinationL2Id(slSrbInfo->m_destinationL2Id);
    rlc->SetRlcChannelType(NrSlRlc::STCH);

    slSrbInfo->m_rlc = rlc;

    Ptr<NrSlPdcp> pdcp = CreateObject<NrSlPdcp>();
    pdcp->SetRnti(m_rnti);
    pdcp->SetLcId(slSrbInfo->m_logicalChannelIdentity);
    pdcp->SetSourceL2Id(slSrbInfo->m_sourceL2Id);
    pdcp->SetDestinationL2Id(slSrbInfo->m_destinationL2Id);
    pdcp->SetNrSlPdcpSapUser(m_nrSlPdcpSapUser);
    pdcp->SetNrSlRlcSapProvider(rlc->GetNrSlRlcSapProvider());
    rlc->SetNrSlRlcSapUser(pdcp->GetNrSlRlcSapUser());

    slSrbInfo->m_pdcp = pdcp;

    // Configure BWP manager and MAC logical channel
    std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm> slLcOnBwpMapping;
    slLcOnBwpMapping = m_nrSlUeBwpmRrcSapProvider->AddNrSlSrbLc(lcInfo, rlc->GetNrSlMacSapUser());

    uint8_t numOfBwpsByBwpm = slLcOnBwpMapping.size();

    NS_ASSERT_MSG(numOfBwpsByBwpm != 0, "SL BWP manager failed to add SL LC for SL radio bearer");

    uint8_t numOfSlBwps = m_slBwpIds.size();
    NS_ASSERT_MSG(
        numOfSlBwps == numOfBwpsByBwpm,
        " Bwp manager configured SL LC for incorrect number of SL BWPs : " << numOfBwpsByBwpm);

    NS_LOG_DEBUG("Size of slLcOnBwpMapping vector " << slLcOnBwpMapping.size());

    std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm>::iterator itSlLcOnBwpMapping;

    for (itSlLcOnBwpMapping = slLcOnBwpMapping.begin();
         itSlLcOnBwpMapping != slLcOnBwpMapping.end();
         ++itSlLcOnBwpMapping)
    {
        NS_LOG_DEBUG("RNTI " << m_rnti << " LCG id " << +itSlLcOnBwpMapping->lcInfo.lcGroup
                             << " BWP Id " << +itSlLcOnBwpMapping->bwpId);
        uint8_t bwpId = itSlLcOnBwpMapping->bwpId;
        m_nrSlUeCmacSapProvider.at(bwpId)->AddNrSlLc(itSlLcOnBwpMapping->lcInfo,
                                                     itSlLcOnBwpMapping->msu);
    }

    return slSrbInfo;
}

uint32_t
NrSlUeRrc::GetSourceL2Id()
{
    return m_srcL2Id;
}

void
NrSlUeRrc::DoSendNrSlDiscoveryMessage(Ptr<Packet> packet, uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this);
    Ptr<NrSlDiscoveryRadioBearerInfo> slSrbAf = GetTxNrSlDiscoveryRadioBearer(dstL2Id);

    NS_LOG_INFO("Send discovery to dstL2Id "
                << slSrbAf->m_destinationL2Id << " lcId "
                << (uint32_t)slSrbAf->m_logicalChannelIdentity << " lcGroup "
                << (uint32_t)slSrbAf->m_logicalChannelConfig.logicalChannelGroup);

    auto params =
        NrSlPdcpSapProvider::NrSlTransmitPdcpSduParameters(packet,
                                                           m_rnti,
                                                           slSrbAf->m_logicalChannelIdentity,
                                                           NrSlPdcpSapUser::PC5_DISCOVERY_SDU);
    slSrbAf->m_pdcp->GetObject<NrSlPdcp>()->GetNrSlPdcpSapProvider()->TransmitNrSlPdcpSdu(params);
}

void
NrSlUeRrc::DoActivateNrSlDiscoveryRadioBearer(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    ActivateNrSlDiscoveryRb(dstL2Id);
}

void
NrSlUeRrc::ActivateNrSlDiscoveryRb(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);
    Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRb;

    switch (m_state)
    {
    case IDLE_START:
    case IDLE_CELL_SEARCH:
    case IDLE_WAIT_MIB_SIB1:
    case IDLE_WAIT_MIB:
    case IDLE_WAIT_SIB1:
    case IDLE_CAMPED_NORMALLY:

        NS_LOG_INFO("Considering IMSI " << m_imsi << " out of network");

        if (m_rnti == RNTI_UNASSIGNED)
        {
            SetOutofCovrgUeRnti();
        }

        {
            for (const auto& it : m_slBwpIds)
            {
                NS_LOG_INFO("Communicating Rx destination to the MAC of SL BWP "
                            << static_cast<uint16_t>(it));
                m_nrSlUeCmacSapProvider.at(it)->AddNrSlRxDstL2Id(dstL2Id);
            }
        }

        slDiscRb = AddNrSlDiscoveryRb(m_srcL2Id, dstL2Id);
        NS_LOG_INFO("Inserted, dstL2Id "
                    << slDiscRb->m_destinationL2Id << " lcId "
                    << (uint32_t)slDiscRb->m_logicalChannelIdentity << " lcGroup "
                    << (uint32_t)slDiscRb->m_logicalChannelConfig.logicalChannelGroup);
        break;

    case IDLE_WAIT_SIB2:
    case IDLE_CONNECTING:
        NS_LOG_INFO("Connecting, must wait to send message");
        break;

    case CONNECTED_NORMALLY:
    case CONNECTED_HANDOVER:
    case CONNECTED_PHY_PROBLEM:
    case CONNECTED_REESTABLISHING:

        NS_LOG_INFO("Considering IMSI " << m_imsi << " RNTI " << m_rnti << " in coverage");
        /**
         * Currently, in-network UEs doing SL rely on preconfiguration.
         * This part of the function should be modified/extended when the
         * network will be involved in SL configuration and protocols.
         */

        // We are going to reuse RNTI assigned by the network.
        // TODO: verify downsides of this approach if any
        {
            for (const auto& it : m_slBwpIds)
            {
                NS_LOG_INFO("Communicating RNTI " << m_rnti << " to PHY and MAC in  in BWP "
                                                  << +it);
                m_cphySapProvider.at(it)->SetRnti(m_rnti);
                m_cmacSapProvider.at(it)->SetRnti(m_rnti);
            }
        }

        // We use same SL-SRB creation and configuration logic as OOC
        slDiscRb = AddNrSlDiscoveryRb(m_srcL2Id, dstL2Id);
        NS_LOG_INFO("Inserted, dstL2Id "
                    << slDiscRb->m_destinationL2Id << " lcId "
                    << (uint32_t)slDiscRb->m_logicalChannelIdentity << " lcGroup "
                    << (uint32_t)slDiscRb->m_logicalChannelConfig.logicalChannelGroup);
        break;

    default: // i.e. IDLE_RANDOM_ACCESS
        NS_FATAL_ERROR("method unexpected in state " << ToString(m_state));
        break;
    }
}

Ptr<NrSlDiscoveryRadioBearerInfo>
NrSlUeRrc::AddNrSlDiscoveryRb(uint32_t srcL2Id, uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this);

    NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo lcInfo;
    lcInfo.srcL2Id = srcL2Id;
    lcInfo.dstL2Id = dstL2Id;
    uint8_t lcid = 4; // discovery
    lcInfo.lcId = lcid;
    // The discovery bearer configuration is using random values since they have not being stated in
    // the standard.
    // TODO: These values need to be re-evaluated.
    lcInfo.lcGroup = 0;  // SL Discovery RBs belong to lcGroup ?
    lcInfo.priority = 2; // SL Discovery RBs have priority ?
    lcInfo.pqi = 65;
    lcInfo.isGbr = false;
    lcInfo.gbr = 65535; // bits/s
    lcInfo.mbr = lcInfo.gbr;
    lcInfo.castType = SidelinkInfo::CastType::Unicast;
    lcInfo.harqEnabled = false;
    lcInfo.dynamic = true;
    lcInfo.pdb = m_signallingPdb;

    Ptr<NrSlDiscoveryRadioBearerInfo> slDiscRbInfo = CreateObject<NrSlDiscoveryRadioBearerInfo>();
    slDiscRbInfo->m_sourceL2Id = lcInfo.srcL2Id;
    slDiscRbInfo->m_destinationL2Id = lcInfo.dstL2Id;
    slDiscRbInfo->m_logicalChannelIdentity = lcInfo.lcId;
    slDiscRbInfo->m_logicalChannelConfig.logicalChannelGroup = lcInfo.lcGroup;
    slDiscRbInfo->m_logicalChannelConfig.priority = lcInfo.priority;
    slDiscRbInfo->m_logicalChannelConfig.prioritizedBitRateKbps = lcInfo.gbr;
    slDiscRbInfo->m_logicalChannelConfig.bucketSizeDurationMs = 1000; // Check this value \todo

    if (m_srcL2Id == srcL2Id)
    {
        // Bearer for transmission
        AddTxNrSlDiscoveryRadioBearer(slDiscRbInfo);
    }
    else
    {
        // Bearer for reception
        AddRxNrSlDiscoveryRadioBearer(slDiscRbInfo);
    }

    // create PDCP/RLC stack
    ObjectFactory rlcObjectFactory;
    rlcObjectFactory.SetTypeId(NrSlRlcUm::GetTypeId());
    Ptr<NrSlRlc> rlc = rlcObjectFactory.Create()->GetObject<NrSlRlc>();
    rlc->SetNrSlMacSapProvider(m_nrSlMacSapProvider);
    rlc->SetRnti(m_rnti);
    rlc->SetLcId(slDiscRbInfo->m_logicalChannelIdentity);
    rlc->SetSourceL2Id(slDiscRbInfo->m_sourceL2Id);
    rlc->SetDestinationL2Id(slDiscRbInfo->m_destinationL2Id);
    rlc->SetRlcChannelType(NrSlRlc::STCH);

    slDiscRbInfo->m_rlc = rlc;

    Ptr<NrSlPdcp> pdcp = CreateObject<NrSlPdcp>();
    pdcp->SetRnti(m_rnti);
    pdcp->SetLcId(slDiscRbInfo->m_logicalChannelIdentity);
    pdcp->SetSourceL2Id(slDiscRbInfo->m_sourceL2Id);
    pdcp->SetDestinationL2Id(slDiscRbInfo->m_destinationL2Id);
    pdcp->SetNrSlPdcpSapUser(m_nrSlPdcpSapUser);
    pdcp->SetNrSlRlcSapProvider(rlc->GetNrSlRlcSapProvider());
    rlc->SetNrSlRlcSapUser(pdcp->GetNrSlRlcSapUser());

    slDiscRbInfo->m_pdcp = pdcp;

    // Configure BWP manager and MAC logical channel
    std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm> slLcOnBwpMapping;
    slLcOnBwpMapping =
        m_nrSlUeBwpmRrcSapProvider->AddNrSlDiscoveryRbLc(lcInfo, rlc->GetNrSlMacSapUser());

    uint8_t numOfBwpsByBwpm = slLcOnBwpMapping.size();

    NS_ASSERT_MSG(numOfBwpsByBwpm != 0, "SL BWP manager failed to add SL LC for SL radio bearer");

    uint8_t numOfSlBwps = m_slBwpIds.size();
    NS_ASSERT_MSG(
        numOfSlBwps == numOfBwpsByBwpm,
        " Bwp manager configured SL LC for incorrect number of SL BWPs : " << numOfBwpsByBwpm);

    NS_LOG_DEBUG("Size of slLcOnBwpMapping vector " << slLcOnBwpMapping.size());

    std::vector<NrSlUeBwpmRrcSapProvider::SlLcInfoBwpm>::iterator itSlLcOnBwpMapping;

    for (itSlLcOnBwpMapping = slLcOnBwpMapping.begin();
         itSlLcOnBwpMapping != slLcOnBwpMapping.end();
         ++itSlLcOnBwpMapping)
    {
        NS_LOG_DEBUG("RNTI " << m_rnti << " LCG id " << +itSlLcOnBwpMapping->lcInfo.lcGroup
                             << " BWP Id " << +itSlLcOnBwpMapping->bwpId);
        uint8_t bwpId = itSlLcOnBwpMapping->bwpId;
        m_nrSlUeCmacSapProvider.at(bwpId)->AddNrSlLc(itSlLcOnBwpMapping->lcInfo,
                                                     itSlLcOnBwpMapping->msu);
    }

    return slDiscRbInfo;
}

void
NrSlUeRrc::EnableUeSdRsrpMeasurements()
{
    NS_LOG_FUNCTION(this);
    for (const auto it : m_slBwpIds)
    {
        m_nrSlUeCphySapProvider.at(it)->EnableUeSdRsrpMeasurements();
    }

    m_sdUeRsrpMeasurementsEnabled = true;
}

void
NrSlUeRrc::DisableUeSdRsrpMeasurements()
{
    NS_LOG_FUNCTION(this);
    for (const auto it : m_slBwpIds)
    {
        m_nrSlUeCphySapProvider.at(it)->DisableUeSdRsrpMeasurements();
    }

    m_sdUeRsrpMeasurementsEnabled = false;
}

void
NrSlUeRrc::EnableUeSlRsrpMeasurements()
{
    NS_LOG_FUNCTION(this);
    for (const auto it : m_slBwpIds)
    {
        m_nrSlUeCphySapProvider.at(it)->EnableUeSlRsrpMeasurements();
    }
}

void
NrSlUeRrc::DisableUeSlRsrpMeasurements()
{
    NS_LOG_FUNCTION(this);
    for (const auto it : m_slBwpIds)
    {
        m_nrSlUeCphySapProvider.at(it)->DisableUeSlRsrpMeasurements();
    }
}

void
NrSlUeRrc::DoReceiveUeSdRsrpMeasurements(NrSlUeCphySapUser::RsrpElementsList l)
{
    NS_LOG_FUNCTION(this);

    double coeff = m_remoteConfig.slReselectionConfig.slFilterCoefficientRsrp;
    double thres = m_remoteConfig.slReselectionConfig.slRsrpThres;
    uint8_t hyst = m_remoteConfig.slReselectionConfig.slHystMin;

    NS_LOG_DEBUG("Relay UE (re)selection parameters:" << " L3 filter coefficient " << coeff
                                                      << " Threshold " << thres << " minHyst "
                                                      << +hyst);

    for (auto rsrpElt : l.rsrpMeasurementsList)
    {
        uint32_t l2Id = rsrpElt.l2Id;
        double rsrpVal = rsrpElt.rsrp;
        double rsrpToReport;

        auto it = m_sdRsrpMeasurementsMap.find(l2Id);
        if (it != m_sdRsrpMeasurementsMap.end())
        {
            auto& [l2Id, measurement] = *it;
            // 3GPP TS 38.331 Section 5.5.3.2
            if (coeff)
            {
                NS_LOG_DEBUG("Using L3 filtering with coefficient: " << coeff);

                // Converting stored and new RSRP to linear units
                double storedRsrpWatt = std::pow(10.0, measurement.value / 10.0) / 1000.0;
                double newRsrpWatt = std::pow(10.0, rsrpVal / 10.0) / 1000.0;
                // Apply L3 filtering: F_n = (1 - a) * F_{n-1} + a * M_n
                // M_n is the latest received measurement result from the physical layer
                // F_n is the updated filtered measurement result,
                // that is used for evaluation of reporting criteria or for measurement reporting
                // F_{n-1} is the old filtered measurement result,
                // where F_0 is set to M_1 when the first measurement result from the physical layer
                // is received

                // a = 1/(2^(k/4)) where k is the L3 filter coefficient
                double a = 1 / (std::pow(2, (coeff / 4)));
                // adapt the filter such that the time characteristics of the filter are preserved
                // at different input rates, observing that the filterCoefficient k assumes a sample
                // rate equal to X ms; The value of X is equivalent to one intra-frequency L1
                // measurement period
                double x =
                    ((Simulator::Now() - measurement.timestamp) / m_rsrpFilterPeriod).GetDouble();
                double rsrpWatt = std::pow((1 - a), x) * storedRsrpWatt + (a * newRsrpWatt);

                // Converting filtered value to dBm
                rsrpToReport = 10 * log10(1000 * (rsrpWatt));
                NS_LOG_DEBUG("l2Id: " << l2Id << " a: " << a << " x:" << x << " storedRsrpWatt: "
                                      << storedRsrpWatt << "  newRsrpWatt: " << newRsrpWatt
                                      << " filtered rsrpWatt: " << rsrpWatt);
            }
            else
            {
                NS_LOG_DEBUG("Not using L3 filtering, storing fresh value");
                rsrpToReport = rsrpVal;
            }
            measurement.value = rsrpToReport;
            measurement.timestamp = Simulator::Now();
            NS_LOG_INFO("SD-RSRP for l2Id " << l2Id << " changed:"
                                            << " value = " << measurement.value
                                            << " timestamp = " << measurement.timestamp);
        }
        else
        {
            NS_LOG_DEBUG("First time storing the SD-RSRP for this destination");
            RsrpMeasurement rsrp;
            rsrp.value = rsrpVal;
            rsrp.timestamp = Simulator::Now();
            m_sdRsrpMeasurementsMap.insert(std::pair<uint32_t, RsrpMeasurement>(l2Id, rsrp));
            rsrpToReport = rsrpVal;
            double rsrpWatt = std::pow(10.0, rsrpToReport / 10.0) / 1000.0;
            NS_LOG_DEBUG("l2Id: " << l2Id << " rsrpWatt: " << rsrpWatt);

            NS_LOG_INFO("New SD-RSRP for l2Id " << l2Id << " stored:"
                                                << " value = " << rsrp.value
                                                << " timestamp = " << rsrp.timestamp);
        }

        // Comparing with threshold and hysteresis
        if ((rsrpToReport - thres) > static_cast<double>(hyst))
        {
            // Pass it to the service layer for relay selection logic
            // true: the relay passes the threshold/hysteresis criteria
            m_nrSlUeSvcRrcSapUser->ReceiveNrSdRsrpMeasurements(l2Id, rsrpToReport, true);
            NS_LOG_DEBUG("SD-RSRP(" << rsrpToReport << ") - threshold(" << thres << ") > hyst("
                                    << +hyst << ") -> l2Id " << l2Id << " eligible for selection");
        }
        else
        {
            // false: the relay is not eligible to be selected due to poor RSRP
            m_nrSlUeSvcRrcSapUser->ReceiveNrSdRsrpMeasurements(l2Id, rsrpToReport, false);
            NS_LOG_DEBUG("SD-RSRP(" << rsrpToReport << ") - threshold(" << thres << ") <= hyst("
                                    << +hyst << ") -> l2Id " << l2Id
                                    << " NOT eligible for selection");
        }
    }

    // Handle L2 IDs that have a stored SD-RSRP measurement but no entry in the current
    // rsrpMeasurementsList (i.e., a known peer UE was not detected during this measurement period).
    // For these, advance the L3 filter using M_n = 0 (no new measurement), preserving the filter
    // time behavior per TS 38.331 and keeping the stored filtered value up to date. If filtering is
    // disabled, assign INVALID_RSRP_DBM to indicate that no measurement was collected. Evaluate the
    // threshold condition with the resulting value and report to upper layers.
    for (auto& entry : m_sdRsrpMeasurementsMap)
    {
        uint32_t l2Id = entry.first;
        RsrpMeasurement& measurement = entry.second;

        // Skip measurements processed above
        if (measurement.timestamp == Simulator::Now())
        {
            continue;
        }
        double rsrpToReport;
        // Apply L3 filtering if configured
        if (coeff)
        {
            NS_LOG_DEBUG("L3 filtering with M_n = 0 for l2Id " << l2Id);

            double storedRsrpWatt = std::pow(10.0, measurement.value / 10.0) / 1000.0;
            double newRsrpWatt = 0.0; // No new measurement: M_n = 0
            double a = 1 / (std::pow(2, (coeff / 4)));
            double x =
                ((Simulator::Now() - measurement.timestamp) / m_rsrpFilterPeriod).GetDouble();
            double rsrpWatt = std::pow((1 - a), x) * storedRsrpWatt + (a * newRsrpWatt);
            rsrpToReport = 10 * std::log10(1000 * rsrpWatt);

            NS_LOG_DEBUG("l2Id: " << l2Id << " a: " << a << " x: " << x << " storedRsrpWatt: "
                                  << storedRsrpWatt << " filtered rsrpWatt (M_n=0): " << rsrpWatt);
        }
        else
        {
            NS_LOG_DEBUG("Not using L3 filtering, M_n = 0 for l2Id " << l2Id);
            rsrpToReport = INVALID_RSRP_DBM;
        }

        // Store updated filtered value and timestamp
        measurement.value = rsrpToReport;
        measurement.timestamp = Simulator::Now();

        NS_LOG_INFO("SD-RSRP (no new measurement) for l2Id "
                    << l2Id << " updated:"
                    << " value = " << measurement.value
                    << " timestamp = " << measurement.timestamp);

        // Comparing with threshold and hysteresis
        if ((rsrpToReport - thres) > static_cast<double>(hyst))
        {
            m_nrSlUeSvcRrcSapUser->ReceiveNrSdRsrpMeasurements(l2Id, rsrpToReport, true);
            NS_LOG_DEBUG("SD-RSRP(" << rsrpToReport << ") - threshold(" << thres << ") > hyst("
                                    << +hyst << ") -> l2Id " << l2Id
                                    << " eligible for selection (M_n = 0, no new report)");
        }
        else
        {
            m_nrSlUeSvcRrcSapUser->ReceiveNrSdRsrpMeasurements(l2Id, rsrpToReport, false);
            NS_LOG_DEBUG("SD-RSRP(" << rsrpToReport << ") - threshold(" << thres << ") <= hyst("
                                    << +hyst << ") -> l2Id " << l2Id
                                    << " NOT eligible for selection (M_n = 0, no new report)");
        }
    }
}

void
NrSlUeRrc::DoReceiveUeSlRsrpMeasurements(NrSlUeCphySapUser::RsrpElementsList l)
{
    NS_LOG_FUNCTION(this);

    double coeff = m_remoteConfig.slReselectionConfig.slFilterCoefficientRsrp;
    double thres = m_remoteConfig.slReselectionConfig.slRsrpThres;
    uint8_t hyst = m_remoteConfig.slReselectionConfig.slHystMin;

    NS_LOG_DEBUG("Relay UE (re)selection parameters:" << " L3 filter coefficient " << coeff
                                                      << " Threshold " << thres << " minHyst "
                                                      << +hyst);

    for (auto rsrpElt : l.rsrpMeasurementsList)
    {
        uint32_t l2Id = rsrpElt.l2Id;
        double rsrpVal = rsrpElt.rsrp;
        double rsrpToReport;

        auto it = m_slRsrpMeasurementsMap.find(l2Id);
        if (it != m_slRsrpMeasurementsMap.end())
        {
            auto& [l2Id, measurement] = *it;
            // 3GPP TS 38.331 Section 5.5.3.2
            if (coeff != 0)
            {
                NS_LOG_DEBUG("Using L3 filtering with coefficient: " << coeff);

                // Converting stored and new RSRP to linear units
                double storedRsrpWatt = std::pow(10.0, measurement.value / 10.0) / 1000.0;
                double newRsrpWatt = std::pow(10.0, rsrpVal / 10.0) / 1000.0;
                // Apply L3 filtering: F_n = (1 - a) * F_{n-1} + a * M_n
                // M_n is the latest received measurement result from the physical layer
                // F_n is the updated filtered measurement result,
                // that is used for evaluation of reporting criteria or for measurement reporting
                // F_{n-1} is the old filtered measurement result,
                // where F_0 is set to M_1 when the first measurement result from the physical layer
                // is received

                // a = 1/(2^(k/4)) where k is the L3 filter coefficient
                double a = 1 / (std::pow(2, (coeff / 4)));
                // adapt the filter such that the time characteristics of the filter are preserved
                // at different input rates, observing that the filterCoefficient k assumes a sample
                // rate equal to X ms; The value of X is equivalent to one intra-frequency L1
                // measurement period
                double x =
                    ((Simulator::Now() - measurement.timestamp) / m_rsrpFilterPeriod).GetDouble();
                double rsrpWatt = std::pow((1 - a), x) * storedRsrpWatt + (a * newRsrpWatt);

                // Converting filtered value to dBm
                rsrpToReport = 10 * log10(1000 * (rsrpWatt));
                NS_LOG_DEBUG("l2Id: " << l2Id << " a: " << a << " x:" << x << " storedRsrpWatt: "
                                      << storedRsrpWatt << "  newRsrpWatt: " << newRsrpWatt
                                      << " filtered rsrpWatt: " << rsrpWatt);
            }
            else
            {
                NS_LOG_DEBUG("Not using L3 filtering, storing fresh value");
                rsrpToReport = rsrpVal;
            }
            measurement.value = rsrpToReport;
            measurement.timestamp = Simulator::Now();
            NS_LOG_INFO("SL-RSRP for l2Id " << l2Id << " changed:"
                                            << " value = " << measurement.value
                                            << " timestamp = " << measurement.timestamp);
        }
        else
        {
            NS_LOG_DEBUG("First time storing the SD-RSRP for this destination");
            RsrpMeasurement rsrp;
            rsrp.value = rsrpVal;
            rsrp.timestamp = Simulator::Now();
            m_slRsrpMeasurementsMap.insert(std::pair<uint32_t, RsrpMeasurement>(l2Id, rsrp));
            rsrpToReport = rsrpVal;
            double rsrpWatt = std::pow(10.0, rsrpToReport / 10.0) / 1000.0;
            NS_LOG_DEBUG("l2Id: " << l2Id << " rsrpWatt: " << rsrpWatt);

            NS_LOG_INFO("New SL-RSRP for l2Id " << l2Id << " stored:"
                                                << " value = " << rsrp.value
                                                << " timestamp = " << rsrp.timestamp);
        }

        // Comparing with threshold and hysteresis
        if ((rsrpToReport - thres) > static_cast<double>(hyst))
        {
            // Pass it to the service layer for relay selection logic
            // true: the relay passes the threshold/hysteresis criteria
            m_nrSlUeSvcRrcSapUser->ReceiveNrSlRsrpMeasurements(l2Id, rsrpToReport, true);
            NS_LOG_DEBUG("SL-RSRP(" << rsrpToReport << ") - threshold(" << thres << ") > hyst("
                                    << +hyst << ") -> l2Id " << l2Id << " eligible for selection");
        }
        else
        {
            // false: the relay is not eligible to be selected due to poor RSRP
            m_nrSlUeSvcRrcSapUser->ReceiveNrSlRsrpMeasurements(l2Id, rsrpToReport, false);
            NS_LOG_DEBUG("SL-RSRP(" << rsrpToReport << ") - threshold(" << thres << ") <= hyst("
                                    << +hyst << ") -> l2Id " << l2Id
                                    << " NOT eligible for selection");
        }
    }
}

void
NrSlUeRrc::DoSetRsrpFilterPeriod(Time period)
{
    NS_LOG_FUNCTION(this << period);
    m_rsrpFilterPeriod = period;
}

bool
NrSlUeRrc::DoIsUeSdRsrpMeasurementsEnabled() const
{
    return m_sdUeRsrpMeasurementsEnabled;
}

bool
NrSlUeRrc::IsInterested(uint32_t dstL2Id) const
{
    for (const auto& it : m_slBwpIds)
    {
        auto dstL2IdSet = m_nrSlUeCmacSapProvider.at(it)->GetSlRxDestinations();
        if (dstL2IdSet.find(dstL2Id) != dstL2IdSet.end())
        {
            return true;
        }
    }
    return false;
}

} // namespace ns3
