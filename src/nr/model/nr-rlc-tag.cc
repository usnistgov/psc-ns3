// Copyright (c) 2011 CTTC
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Jaume Nin <jaume.nin@cttc.es>

#include "nr-rlc-tag.h"

#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrRlcTag);

NrRlcTag::NrRlcTag()
    : m_senderTimestamp(Seconds(0))
{
    // Nothing to do here
}

NrRlcTag::NrRlcTag(Time senderTimestamp)
    : m_senderTimestamp(senderTimestamp)

{
    // Nothing to do here
}

TypeId
NrRlcTag::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrRlcTag").SetParent<Tag>().SetGroupName("Nr").AddConstructor<NrRlcTag>();
    return tid;
}

TypeId
NrRlcTag::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrRlcTag::GetSerializedSize() const
{
    return sizeof(Time) + sizeof(uint16_t);
}

void
NrRlcTag::Serialize(TagBuffer i) const
{
    int64_t senderTimestamp = m_senderTimestamp.GetNanoSeconds();
    i.Write((const uint8_t*)&senderTimestamp, sizeof(int64_t));
    i.WriteU16(m_txRnti);
}

void
NrRlcTag::Deserialize(TagBuffer i)
{
    int64_t senderTimestamp;
    i.Read((uint8_t*)&senderTimestamp, 8);
    m_senderTimestamp = NanoSeconds(senderTimestamp);
    m_txRnti = i.ReadU16();
}

void
NrRlcTag::Print(std::ostream& os) const
{
    os << m_senderTimestamp;
    os << " " << m_txRnti;
}

} // namespace ns3
