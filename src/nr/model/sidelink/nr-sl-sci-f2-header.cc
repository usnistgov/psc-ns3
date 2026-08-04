// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-sl-sci-f2-header.h"

#include <ns3/log.h>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrSlSciF2Header);
NS_LOG_COMPONENT_DEFINE("NrSlSciF2Header");

NrSlSciF2Header::NrSlSciF2Header()
{
}

NrSlSciF2Header::~NrSlSciF2Header()
{
}

TypeId
NrSlSciF2Header::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlSciF2Header").SetParent<Header>().AddConstructor<NrSlSciF2Header>();
    return tid;
}

TypeId
NrSlSciF2Header::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
NrSlSciF2Header::SetHarqId(uint8_t harqId)
{
    m_harqId = harqId;
}

void
NrSlSciF2Header::SetNdi(uint8_t ndi)
{
    m_ndi = ndi;
}

void
NrSlSciF2Header::SetRv(uint8_t rv)
{
    m_rv = rv;
}

void
NrSlSciF2Header::SetSrcId(uint32_t srcId)
{
    m_srcId = srcId;
}

void
NrSlSciF2Header::SetDstId(uint32_t dstId)
{
    m_dstId = dstId;
}

void
NrSlSciF2Header::SetHarqFbIndicator(uint8_t harqFb)
{
    m_harqFbIndicator = harqFb;
}

uint8_t
NrSlSciF2Header::GetHarqId() const
{
    return m_harqId;
}

uint8_t
NrSlSciF2Header::GetNdi() const
{
    return m_ndi;
}

uint8_t
NrSlSciF2Header::GetRv() const
{
    return m_rv;
}

uint8_t
NrSlSciF2Header::GetSrcId() const
{
    return m_srcId;
}

uint16_t
NrSlSciF2Header::GetDstId() const
{
    return m_dstId;
}

uint8_t
NrSlSciF2Header::GetHarqFbIndicator() const
{
    return m_harqFbIndicator;
}

bool
NrSlSciF2Header::EnsureMandConfig() const
{
    return m_harqId != std::numeric_limits<uint8_t>::max() &&
           m_ndi != std::numeric_limits<uint8_t>::max() &&
           m_rv != std::numeric_limits<uint8_t>::max() &&
           m_srcId != std::numeric_limits<uint32_t>::max() &&
           m_dstId != std::numeric_limits<uint32_t>::max();
}

void
NrSlSciF2Header::Print(std::ostream& os) const
{
    NS_LOG_FUNCTION(this);
    os << "HARQ process id " << +m_harqId << ", New data indicator " << +m_ndi
       << ", Redundancy version " << +m_rv << ", Source layer 2 Id " << +m_srcId
       << ", Destination layer 2 id " << m_dstId << ", HARQ feedback indicator "
       << +m_harqFbIndicator;
}

uint32_t
NrSlSciF2Header::GetSerializedSize() const
{
    return 4;
}

void
NrSlSciF2Header::Serialize(Buffer::Iterator start) const
{
    NS_FATAL_ERROR("Call to NrSlSciF2Header::Serialize is forbidden");
}

void
NrSlSciF2Header::PreSerialize(Buffer::Iterator& i) const
{
    NS_ASSERT_MSG(EnsureMandConfig(), "All the mandatory fields must be set before serializing");

    uint32_t scif2 = 0;

    scif2 = (m_harqId & 0xF);
    scif2 = (m_ndi & 0x1) | (scif2 << 1);
    scif2 = (m_rv & 0x3) | (scif2 << 2);
    scif2 = (m_srcId & 0xFF) | (scif2 << 8);
    scif2 = (m_dstId & 0xFFFF) | (scif2 << 16);
    scif2 = (m_harqFbIndicator & 0x1) | (scif2 << 1);

    i.WriteHtonU32(scif2);
}

uint32_t
NrSlSciF2Header::PreDeserialize(Buffer::Iterator& i)
{
    uint32_t scif2 = i.ReadNtohU32();

    m_harqId = (scif2 >> 28) & 0xF;
    m_ndi = (scif2 >> 27) & 0x1;
    m_rv = (scif2 >> 25) & 0x3;
    m_srcId = (scif2 >> 17) & 0xFF;
    m_dstId = (scif2 >> 1) & 0xFFFF;
    m_harqFbIndicator = (scif2 & 0x1);

    return GetSerializedSize();
}

uint32_t
NrSlSciF2Header::Deserialize(Buffer::Iterator start)
{
    return PreDeserialize(start);
}

} // namespace ns3
