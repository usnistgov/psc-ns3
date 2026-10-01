// Copyright (c) 2011,2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Marco Miozzo <marco.miozzo@cttc.es>
//         Nicola Baldo <nbaldo@cttc.es>

#include "nr-eps-bearer-tag.h"

#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrEpsBearerTag);

TypeId
NrEpsBearerTag::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrEpsBearerTag")
            .SetParent<Tag>()
            .SetGroupName("Nr")
            .AddConstructor<NrEpsBearerTag>()
            .AddAttribute("rnti",
                          "The rnti that indicates the UE which packet belongs",
                          UintegerValue(0),
                          MakeUintegerAccessor(&NrEpsBearerTag::GetRnti),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("bid",
                          "The EPS bearer id within the UE to which the packet belongs",
                          UintegerValue(0),
                          MakeUintegerAccessor(&NrEpsBearerTag::GetBid),
                          MakeUintegerChecker<uint8_t>());
    return tid;
}

TypeId
NrEpsBearerTag::GetInstanceTypeId() const
{
    return GetTypeId();
}

NrEpsBearerTag::NrEpsBearerTag()
    : m_rnti(0),
      m_bid(0)
{
}

NrEpsBearerTag::NrEpsBearerTag(uint16_t rnti, uint8_t bid)
    : m_rnti(rnti),
      m_bid(bid)
{
}

void
NrEpsBearerTag::SetRnti(uint16_t rnti)
{
    m_rnti = rnti;
}

void
NrEpsBearerTag::SetBid(uint8_t bid)
{
    m_bid = bid;
}

uint32_t
NrEpsBearerTag::GetSerializedSize() const
{
    return 3;
}

void
NrEpsBearerTag::Serialize(TagBuffer i) const
{
    i.WriteU16(m_rnti);
    i.WriteU8(m_bid);
}

void
NrEpsBearerTag::Deserialize(TagBuffer i)
{
    m_rnti = (uint16_t)i.ReadU16();
    m_bid = (uint8_t)i.ReadU8();
}

uint16_t
NrEpsBearerTag::GetRnti() const
{
    return m_rnti;
}

uint8_t
NrEpsBearerTag::GetBid() const
{
    return m_bid;
}

void
NrEpsBearerTag::Print(std::ostream& os) const
{
    os << "rnti=" << m_rnti << ", bid=" << (uint16_t)m_bid;
}

} // namespace ns3
