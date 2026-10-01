// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "bwp-manager-gnb.h"

#include "bwp-manager-algorithm.h"
#include "nr-control-messages.h"

#include <ns3/log.h>
#include <ns3/object-map.h>
#include <ns3/pointer.h>
#include <ns3/uinteger.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("BwpManagerGnb");
NS_OBJECT_ENSURE_REGISTERED(BwpManagerGnb);

BwpManagerGnb::BwpManagerGnb()
    : NrRrComponentCarrierManager()
{
    NS_LOG_FUNCTION(this);
}

BwpManagerGnb::~BwpManagerGnb()
{
    NS_LOG_FUNCTION(this);
}

TypeId
BwpManagerGnb::GetTypeId()
{
    static TypeId tid = TypeId("ns3::BwpManagerGnb")
                            .SetParent<NrNoOpComponentCarrierManager>()
                            .SetGroupName("nr")
                            .AddConstructor<BwpManagerGnb>()
                            .AddAttribute("BwpManagerAlgorithm",
                                          "The algorithm pointer",
                                          PointerValue(),
                                          MakePointerAccessor(&BwpManagerGnb::m_algorithm),
                                          MakePointerChecker<BwpManagerAlgorithm>());
    return tid;
}

void
BwpManagerGnb::SetBwpManagerAlgorithm(const Ptr<BwpManagerAlgorithm>& algorithm)
{
    NS_LOG_FUNCTION(this);
    m_algorithm = algorithm;
}

uint8_t
BwpManagerGnb::GetResourceType(NrMacSapProvider::ReportBufferStatusParameters params)
{
    NS_ASSERT_MSG(m_ueInfo.find(params.rnti) != m_ueInfo.end(),
                  "Trying to check the QoS of unknown UE");
    NS_ASSERT_MSG(m_ueInfo.at(params.rnti).m_rlcLcInstantiated.find(params.lcid) !=
                      m_ueInfo.at(params.rnti).m_rlcLcInstantiated.end(),
                  "Trying to check the QoS of unknown logical channel");
    return (m_ueInfo[params.rnti].m_rlcLcInstantiated[params.lcid]).resourceType;
}

std::vector<NrCcmRrcSapProvider::LcsConfig>
BwpManagerGnb::DoSetupDataRadioBearer(NrEpsBearer bearer,
                                      uint8_t bearerId,
                                      uint16_t rnti,
                                      uint8_t lcid,
                                      uint8_t lcGroup,
                                      NrMacSapUser* msu)
{
    NS_LOG_FUNCTION(this);

    std::vector<NrCcmRrcSapProvider::LcsConfig> lcsConfig =
        NrRrComponentCarrierManager::DoSetupDataRadioBearer(bearer,
                                                            bearerId,
                                                            rnti,
                                                            lcid,
                                                            lcGroup,
                                                            msu);
    return lcsConfig;
}

uint8_t
BwpManagerGnb::GetBwpIndex(uint16_t rnti, uint8_t lcid)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_algorithm != nullptr);
    NS_ASSERT_MSG(m_ueInfo.find(rnti) != m_ueInfo.end(), "Unknown UE");
    NS_ASSERT_MSG(m_ueInfo.at(rnti).m_rlcLcInstantiated.find(lcid) !=
                      m_ueInfo.at(rnti).m_rlcLcInstantiated.end(),
                  "Unknown logical channel of UE");

    uint8_t qci = m_ueInfo[rnti].m_rlcLcInstantiated[lcid].qci;

    // Force a conversion between the uint8_t type that comes from the LcInfo
    // struct (yeah, using the NrEpsBearer::Qci type was too hard ...)
    return m_algorithm->GetBwpForEpsBearer(static_cast<NrEpsBearer::Qci>(qci));
}

uint8_t
BwpManagerGnb::PeekBwpIndex(uint16_t rnti, uint8_t lcid) const
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_algorithm != nullptr);
    // For the moment, Get and Peek are the same, but they'll change
    NS_ASSERT_MSG(m_ueInfo.find(rnti) != m_ueInfo.end(), "Unknown UE");
    NS_ASSERT_MSG(m_ueInfo.at(rnti).m_rlcLcInstantiated.find(lcid) !=
                      m_ueInfo.at(rnti).m_rlcLcInstantiated.end(),
                  "Unknown logical channel of UE");

    uint8_t qci = m_ueInfo.at(rnti).m_rlcLcInstantiated.at(lcid).qci;

    // Force a conversion between the uint8_t type that comes from the LcInfo
    // struct (yeah, using the NrEpsBearer::Qci type was too hard ...)
    return m_algorithm->GetBwpForEpsBearer(static_cast<NrEpsBearer::Qci>(qci));
}

uint8_t
BwpManagerGnb::RouteIngoingCtrlMsgs(const Ptr<NrControlMessage>& msg, uint8_t sourceBwpId) const
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("Msg type " << msg->GetMessageType() << " from bwp " << +sourceBwpId
                            << " that wants to go in the gnb, goes in BWP " << msg->GetSourceBwp());
    return msg->GetSourceBwp();
}

uint8_t
BwpManagerGnb::RouteOutgoingCtrlMsg(const Ptr<NrControlMessage>& msg, uint8_t sourceBwpId) const
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("Msg type " << msg->GetMessageType() << " from bwp " << +sourceBwpId
                            << " that wants to go out from gnb");

    if (m_outputLinks.empty())
    {
        NS_LOG_INFO("No linked BWP, routing outgoing msg to the source: " << +sourceBwpId);
        return sourceBwpId;
    }

    auto it = m_outputLinks.find(sourceBwpId);
    if (it == m_outputLinks.end())
    {
        NS_LOG_INFO("Source BWP not in the map, routing outgoing msg to itself: " << +sourceBwpId);
        return sourceBwpId;
    }

    NS_LOG_INFO("routing outgoing msg to bwp: " << +it->second);
    return it->second;
}

void
BwpManagerGnb::SetOutputLink(uint32_t sourceBwp, uint32_t outputBwp)
{
    NS_LOG_FUNCTION(this);
    m_outputLinks.insert(std::make_pair(sourceBwp, outputBwp));
}

void
BwpManagerGnb::DoReportBufferStatus(NrMacSapProvider::ReportBufferStatusParameters params)
{
    NS_LOG_FUNCTION(this);

    uint8_t bwpIndex = GetBwpIndex(params.rnti, params.lcid);

    if (m_macSapProvidersMap.find(bwpIndex) != m_macSapProvidersMap.end())
    {
        m_macSapProvidersMap.find(bwpIndex)->second->ReportBufferStatus(params);
    }
    else
    {
        NS_ABORT_MSG("Bwp index " << +bwpIndex << " not valid.");
    }
}

void
BwpManagerGnb::DoNotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams)
{
    NS_LOG_FUNCTION(this);
    std::map<uint16_t, NrUeInfo>::iterator rntiIt = m_ueInfo.find(txOpParams.rnti);
    NS_ASSERT_MSG(rntiIt != m_ueInfo.end(), "could not find RNTI" << txOpParams.rnti);

    std::map<uint8_t, NrMacSapUser*>::iterator lcidIt =
        rntiIt->second.m_ueAttached.find(txOpParams.lcid);
    NS_ASSERT_MSG(lcidIt != rntiIt->second.m_ueAttached.end(),
                  "could not find LCID " << (uint16_t)txOpParams.lcid);

    (*lcidIt).second->NotifyTxOpportunity(txOpParams);
}

void
BwpManagerGnb::DoUlReceiveMacCe(nr::MacCeListElement_s bsr, uint8_t componentCarrierId)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_algorithm != nullptr);
    NS_ASSERT_MSG(bsr.m_macCeType == nr::MacCeListElement_s::BSR,
                  "Received a Control Message not allowed " << bsr.m_macCeType);
    NS_ASSERT_MSG(m_ccmMacSapProviderMap.find(componentCarrierId) != m_ccmMacSapProviderMap.end(),
                  "Mac sap provider does not exist.");

    NS_LOG_DEBUG("Routing BSR for UE " << bsr.m_rnti << " to source CC id "
                                       << static_cast<uint32_t>(componentCarrierId));

    if (m_ccmMacSapProviderMap.find(componentCarrierId) != m_ccmMacSapProviderMap.end())
    {
        m_ccmMacSapProviderMap.find(componentCarrierId)->second->ReportMacCeToScheduler(bsr);
    }
    else
    {
        NS_ABORT_MSG("Bwp index not valid.");
    }
}

void
BwpManagerGnb::DoUlReceiveSr(uint16_t rnti, uint8_t componentCarrierId)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(m_algorithm != nullptr);

    NS_LOG_DEBUG("Routing SR for UE " << rnti << " to source CC id "
                                      << static_cast<uint32_t>(componentCarrierId));

    auto it = m_ccmMacSapProviderMap.find(componentCarrierId);
    NS_ABORT_IF(it == m_ccmMacSapProviderMap.end());

    m_ccmMacSapProviderMap.find(componentCarrierId)->second->ReportSrToScheduler(rnti);
}

} // end of namespace ns3
