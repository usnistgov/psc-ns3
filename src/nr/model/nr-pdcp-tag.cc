// Copyright (c) 2011 CTTC
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Jaume Nin <jaume.nin@cttc.es>
//         Nicola Baldo <nbaldo@cttc.es>

#include "nr-pdcp-tag.h"

#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrPdcpTag);

NrPdcpTag::NrPdcpTag()
    : m_senderTimestamp(Seconds(0))
{
    // Nothing to do here
}

NrPdcpTag::NrPdcpTag(Time senderTimestamp)
    : m_senderTimestamp(senderTimestamp)

{
    // Nothing to do here
}

TypeId
NrPdcpTag::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrPdcpTag").SetParent<Tag>().SetGroupName("Nr").AddConstructor<NrPdcpTag>();
    return tid;
}

TypeId
NrPdcpTag::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrPdcpTag::GetSerializedSize() const
{
    return sizeof(Time);
}

void
NrPdcpTag::Serialize(TagBuffer i) const
{
    int64_t senderTimestamp = m_senderTimestamp.GetNanoSeconds();
    i.Write((const uint8_t*)&senderTimestamp, sizeof(int64_t));
}

void
NrPdcpTag::Deserialize(TagBuffer i)
{
    int64_t senderTimestamp;
    i.Read((uint8_t*)&senderTimestamp, 8);
    m_senderTimestamp = NanoSeconds(senderTimestamp);
}

void
NrPdcpTag::Print(std::ostream& os) const
{
    os << m_senderTimestamp;
}

Time
NrPdcpTag::GetSenderTimestamp() const
{
    return m_senderTimestamp;
}

void
NrPdcpTag::SetSenderTimestamp(Time senderTimestamp)
{
    this->m_senderTimestamp = senderTimestamp;
}

} // namespace ns3
