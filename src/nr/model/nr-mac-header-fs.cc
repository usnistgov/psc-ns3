// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-header-fs.h"

#include <ns3/log.h>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrMacHeaderFs);
NS_LOG_COMPONENT_DEFINE("NrMacHeaderFs");

TypeId
NrMacHeaderFs::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrMacHeaderFs").SetParent<Header>().AddConstructor<NrMacHeaderFs>();
    return tid;
}

TypeId
NrMacHeaderFs::GetInstanceTypeId() const
{
    return GetTypeId();
}

NrMacHeaderFs::NrMacHeaderFs()
{
    NS_LOG_FUNCTION(this);
}

void
NrMacHeaderFs::Serialize(Buffer::Iterator start) const
{
    NS_LOG_FUNCTION(this);

    // 0x3F: 0 0 1 1 1 1 1 1
    uint8_t firstByte = m_lcid & 0x3F; // R, R bit set to 0, the rest equal to lcId
    start.WriteU8(firstByte);
}

uint32_t
NrMacHeaderFs::Deserialize(Buffer::Iterator start)
{
    NS_LOG_FUNCTION(this);

    m_lcid = start.ReadU8();

    return 1;
}

uint32_t
NrMacHeaderFs::GetSerializedSize() const
{
    NS_LOG_FUNCTION(this);
    return 1;
}

void
NrMacHeaderFs::Print(std::ostream& os) const
{
    NS_LOG_FUNCTION(this);
    os << "LCid " << +m_lcid;
}

void
NrMacHeaderFs::SetLcId(uint8_t lcId)
{
    NS_ASSERT(lcId == PADDING);
    m_lcid = PADDING;
}

uint8_t
NrMacHeaderFs::GetLcId() const
{
    return m_lcid;
}

bool
NrMacHeaderFs::operator==(const NrMacHeaderFs& o) const
{
    return m_lcid == o.m_lcid;
}

} // namespace ns3
