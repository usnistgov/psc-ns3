// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-ue-net-device.h"

#include "bandwidth-part-ue.h"
#include "bwp-manager-ue.h"
#include "nr-epc-ue-nas.h"
#include "nr-gnb-net-device.h"
#include "nr-initial-association.h"
#include "nr-ue-component-carrier-manager.h"
#include "nr-ue-mac.h"
#include "nr-ue-phy.h"
#include "nr-ue-rrc.h"

#include <ns3/ipv4-l3-protocol.h>
#include <ns3/ipv6-l3-protocol.h>
#include <ns3/object-map.h>
#include <ns3/pointer.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrUeNetDevice");

NS_OBJECT_ENSURE_REGISTERED(NrUeNetDevice);

TypeId
NrUeNetDevice::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrUeNetDevice")
            .SetParent<NrNetDevice>()
            .AddConstructor<NrUeNetDevice>()
            .AddAttribute("NrEpcUeNas",
                          "The NAS associated to this UeNetDevice",
                          PointerValue(),
                          MakePointerAccessor(&NrUeNetDevice::m_nas),
                          MakePointerChecker<NrEpcUeNas>())
            .AddAttribute("nrUeRrc",
                          "The RRC associated to this UeNetDevice",
                          PointerValue(),
                          MakePointerAccessor(&NrUeNetDevice::m_rrc),
                          MakePointerChecker<NrUeRrc>())
            .AddAttribute("Imsi",
                          "International Mobile Subscriber Identity assigned to this UE",
                          UintegerValue(0),
                          MakeUintegerAccessor(&NrUeNetDevice::SetImsi, &NrUeNetDevice::GetImsi),
                          MakeUintegerChecker<uint64_t>())
            .AddAttribute(
                "PrimaryDlIndex",
                "The index of DL PHY/MAC that will be used as the primary DL PHY/MAC."
                "This is needed because UE RRC needs to know which DL PHY/MAC pair is primary.",
                UintegerValue(0),
                MakeUintegerAccessor(&NrUeNetDevice::m_primaryDlIndex),
                MakeUintegerChecker<uint16_t>())
            .AddAttribute(
                "PrimaryUlIndex",
                "The index of UL PHY/MAC that will be used as the primary UL PHY/MAC."
                "This is needed because UE RRC needs to know which UL PHY/MAC pair is primary.",
                UintegerValue(0),
                MakeUintegerAccessor(&NrUeNetDevice::m_primaryUlIndex),
                MakeUintegerChecker<uint16_t>())
            .AddAttribute("NrUeRrc",
                          "The RRC layer associated with the gNB",
                          PointerValue(),
                          MakePointerAccessor(&NrUeNetDevice::m_rrc),
                          MakePointerChecker<NrUeRrc>())
            .AddAttribute("NrUeComponentCarrierManager",
                          "The ComponentCarrierManager associated to this UeNetDevice",
                          PointerValue(),
                          MakePointerAccessor(&NrUeNetDevice::m_componentCarrierManager),
                          MakePointerChecker<NrUeComponentCarrierManager>())
            .AddAttribute("ComponentCarrierMapUe",
                          "List of all component Carrier.",
                          ObjectMapValue(),
                          MakeObjectMapAccessor(&NrUeNetDevice::m_ccMap),
                          MakeObjectMapChecker<BandwidthPartUe>())
            .AddAttribute("InitAssoc",
                          "Pointer to nr Initial Accos",
                          PointerValue(),
                          MakePointerAccessor(&NrUeNetDevice::m_nrInitAcc),
                          MakePointerChecker<NrInitialAssociation>());
    return tid;
}

NrUeNetDevice::NrUeNetDevice()
{
    NS_LOG_FUNCTION(this);
}

NrUeNetDevice::~NrUeNetDevice()
{
}

void
NrUeNetDevice::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    // While these may have been previously set, the values may not
    // have propagated to the other objects depending on whether they
    // had been created upon the previous setting time.
    m_nas->SetImsi(m_imsi);
    m_rrc->SetImsi(m_imsi);
    m_nas->SetCsgId(m_csgId); // this also handles propagation to RRC
}

void
NrUeNetDevice::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_rrc->Dispose();
    m_rrc = nullptr;

    m_targetGnb = nullptr;
    m_nas->Dispose();
    m_nas = nullptr;
    for (const auto& it : m_ccMap)
    {
        it.second->Dispose();
    }
    m_ccMap.clear();
    m_componentCarrierManager->Dispose();
    m_componentCarrierManager = nullptr;
    NrNetDevice::DoDispose();
}

std::map<uint8_t, Ptr<BandwidthPartUe>>
NrUeNetDevice::GetCcMap()
{
    NS_LOG_FUNCTION(this);
    return m_ccMap;
}

uint32_t
NrUeNetDevice::GetCcMapSize() const
{
    NS_LOG_FUNCTION(this);
    return m_ccMap.size();
}

void
NrUeNetDevice::EnqueueDlHarqFeedback(const DlHarqInfo& m) const
{
    NS_LOG_FUNCTION(this);

    auto ccManager = DynamicCast<BwpManagerUe>(m_componentCarrierManager);
    NS_ASSERT(ccManager != nullptr);
    uint8_t index = ccManager->RouteDlHarqFeedback(m);
    m_ccMap.at(index)->GetPhy()->EnqueueDlHarqFeedback(m);
}

void
NrUeNetDevice::RouteIngoingCtrlMsgs(const std::list<Ptr<NrControlMessage>>& msgList,
                                    uint8_t sourceBwpId)
{
    NS_LOG_FUNCTION(this);

    for (const auto& msg : msgList)
    {
        uint8_t bwpId = DynamicCast<BwpManagerUe>(m_componentCarrierManager)
                            ->RouteIngoingCtrlMsg(msg, sourceBwpId);
        m_ccMap.at(bwpId)->GetPhy()->PhyCtrlMessagesReceived(msg);
    }
}

void
NrUeNetDevice::RouteOutgoingCtrlMsgs(const std::list<Ptr<NrControlMessage>>& msgList,
                                     uint8_t sourceBwpId)
{
    NS_LOG_FUNCTION(this);

    for (const auto& msg : msgList)
    {
        uint8_t bwpId = DynamicCast<BwpManagerUe>(m_componentCarrierManager)
                            ->RouteOutgoingCtrlMsg(msg, sourceBwpId);
        NS_ASSERT_MSG(m_ccMap.size() > bwpId,
                      "Returned bwp " << +bwpId << " is not present. Check your configuration");
        NS_ASSERT_MSG(
            m_ccMap.at(bwpId)->GetPhy()->HasUlSlot(),
            "Returned bwp "
                << +bwpId
                << " has no UL slot, so the message can't go out. Check your configuration");
        m_ccMap.at(bwpId)->GetPhy()->EncodeCtrlMsg(msg);
    }
}

void
NrUeNetDevice::SetCcMap(std::map<uint8_t, Ptr<BandwidthPartUe>> ccm)
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_IF(!m_ccMap.empty());
    m_ccMap = ccm;
}

uint32_t
NrUeNetDevice::GetCsgId() const
{
    NS_LOG_FUNCTION(this);
    return m_csgId;
}

void
NrUeNetDevice::SetCsgId(uint32_t csgId)
{
    NS_LOG_FUNCTION(this << csgId);
    m_csgId = csgId;
    if (m_nas)
    {
        m_nas->SetCsgId(m_csgId); // this also handles propagation to RRC
    }
}

Ptr<NrUeMac>
NrUeNetDevice::GetMac(uint8_t index) const
{
    NS_LOG_FUNCTION(this);
    return m_ccMap.at(index)->GetMac();
}

void
NrUeNetDevice::UpdateConfig()
{
    NS_LOG_FUNCTION(this);
}

bool
NrUeNetDevice::DoSend(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << packet << dest << protocolNumber);
    NS_ABORT_MSG_IF(protocolNumber != Ipv4L3Protocol::PROT_NUMBER &&
                        protocolNumber != Ipv6L3Protocol::PROT_NUMBER,
                    "unsupported protocol " << protocolNumber
                                            << ", only IPv4 and IPv6 are supported");
    return m_nas->Send(packet, protocolNumber);
}

Ptr<NrUePhy>
NrUeNetDevice::GetPhy(uint8_t index) const
{
    NS_LOG_FUNCTION(this);
    return m_ccMap.at(index)->GetPhy();
}

Ptr<BwpManagerUe>
NrUeNetDevice::GetBwpManager() const
{
    NS_LOG_FUNCTION(this);
    return DynamicCast<BwpManagerUe>(m_componentCarrierManager);
}

Ptr<NrEpcUeNas>
NrUeNetDevice::GetNas() const
{
    NS_LOG_FUNCTION(this);
    return m_nas;
}

Ptr<NrUeRrc>
NrUeNetDevice::GetRrc() const
{
    NS_LOG_FUNCTION(this);
    return m_rrc;
}

void
NrUeNetDevice::SetImsi(uint64_t imsi)
{
    NS_LOG_FUNCTION(this << imsi);
    m_imsi = imsi;
    if (m_nas)
    {
        m_nas->SetImsi(imsi);
    }
    if (m_rrc)
    {
        m_rrc->SetImsi(imsi);
    }
}

uint64_t
NrUeNetDevice::GetImsi() const
{
    NS_LOG_FUNCTION(this);
    return m_imsi;
}

uint16_t
NrUeNetDevice::GetCellId() const
{
    auto gnb = GetTargetGnb();
    if (gnb)
    {
        return GetTargetGnb()->GetCellId();
    }
    else
    {
        return UINT16_MAX;
    }
}

void
NrUeNetDevice::SetInitAssoc(Ptr<NrInitialAssociation> initAssoc)
{
    NS_LOG_FUNCTION(this);
    m_nrInitAcc = initAssoc;
}

void
NrUeNetDevice::SetTargetGnb(Ptr<NrGnbNetDevice> gnb)
{
    NS_LOG_FUNCTION(this);
    m_targetGnb = gnb;
}

Ptr<const NrGnbNetDevice>
NrUeNetDevice::GetTargetGnb() const
{
    NS_LOG_FUNCTION(this);
    return m_targetGnb;
}

} // namespace ns3
