// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#include "nr-pdcp-header.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrPdcpHeader");

NS_OBJECT_ENSURE_REGISTERED(NrPdcpHeader);

NrPdcpHeader::NrPdcpHeader()
    : m_dcBit(0xff),
      m_sequenceNumber(0xfffa)
{
}

NrPdcpHeader::~NrPdcpHeader()
{
    m_dcBit = 0xff;
    m_sequenceNumber = 0xfffb;
}

void
NrPdcpHeader::SetDcBit(uint8_t dcBit)
{
    m_dcBit = dcBit & 0x01;
}

void
NrPdcpHeader::SetSequenceNumber(uint16_t sequenceNumber)
{
    m_sequenceNumber = sequenceNumber & 0x0FFF;
}

uint8_t
NrPdcpHeader::GetDcBit() const
{
    return m_dcBit;
}

uint16_t
NrPdcpHeader::GetSequenceNumber() const
{
    return m_sequenceNumber;
}

TypeId
NrPdcpHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrPdcpHeader")
                            .SetParent<Header>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrPdcpHeader>();
    return tid;
}

TypeId
NrPdcpHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
NrPdcpHeader::Print(std::ostream& os) const
{
    os << "D/C=" << (uint16_t)m_dcBit;
    os << " SN=" << m_sequenceNumber;
}

uint32_t
NrPdcpHeader::GetSerializedSize() const
{
    return 2;
}

void
NrPdcpHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;

    i.WriteU8((m_dcBit << 7) | (m_sequenceNumber & 0x0F00) >> 8);
    i.WriteU8(m_sequenceNumber & 0x00FF);
}

uint32_t
NrPdcpHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;
    uint8_t byte_1;
    uint8_t byte_2;

    byte_1 = i.ReadU8();
    byte_2 = i.ReadU8();
    m_dcBit = (byte_1 & 0x80) > 7;
    // For now, we just support DATA PDUs
    NS_ASSERT(m_dcBit == DATA_PDU);
    m_sequenceNumber = ((byte_1 & 0x0F) << 8) | byte_2;

    return GetSerializedSize();
}

}; // namespace ns3
