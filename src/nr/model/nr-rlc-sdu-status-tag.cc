// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#include "nr-rlc-sdu-status-tag.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrRlcSduStatusTag);

NrRlcSduStatusTag::NrRlcSduStatusTag()
{
}

void
NrRlcSduStatusTag::SetStatus(uint8_t status)
{
    m_sduStatus = status;
}

uint8_t
NrRlcSduStatusTag::GetStatus() const
{
    return m_sduStatus;
}

TypeId
NrRlcSduStatusTag::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrRlcSduStatusTag")
                            .SetParent<Tag>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrRlcSduStatusTag>();
    return tid;
}

TypeId
NrRlcSduStatusTag::GetInstanceTypeId() const
{
    return GetTypeId();
}

uint32_t
NrRlcSduStatusTag::GetSerializedSize() const
{
    return 1;
}

void
NrRlcSduStatusTag::Serialize(TagBuffer i) const
{
    i.WriteU8(m_sduStatus);
}

void
NrRlcSduStatusTag::Deserialize(TagBuffer i)
{
    m_sduStatus = i.ReadU8();
}

void
NrRlcSduStatusTag::Print(std::ostream& os) const
{
    os << "SDU Status=" << (uint32_t)m_sduStatus;
}

}; // namespace ns3
