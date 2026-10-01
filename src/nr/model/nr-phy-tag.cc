// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Marco Miozzo <marco.miozzo@cttc.es>
//         Nicola Baldo <nbaldo@cttc.es>

#include "nr-phy-tag.h"

#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrPhyTag);

TypeId
NrPhyTag::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrPhyTag").SetParent<Tag>().SetGroupName("Nr").AddConstructor<NrPhyTag>();
    return tid;
}

TypeId
NrPhyTag::GetInstanceTypeId() const
{
    return GetTypeId();
}

NrPhyTag::NrPhyTag()
{
}

NrPhyTag::NrPhyTag(uint16_t cellId)
    : m_cellId(cellId)
{
}

NrPhyTag::~NrPhyTag()
{
}

uint32_t
NrPhyTag::GetSerializedSize() const
{
    return 2;
}

void
NrPhyTag::Serialize(TagBuffer i) const
{
    i.WriteU16(m_cellId);
}

void
NrPhyTag::Deserialize(TagBuffer i)
{
    m_cellId = i.ReadU16();
}

void
NrPhyTag::Print(std::ostream& os) const
{
    os << m_cellId;
}

uint16_t
NrPhyTag::GetCellId() const
{
    return m_cellId;
}

} // namespace ns3
