//
// SPDX-License-Identifier: NIST-Software
//

#include "nr-sl-pdcp-header.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlPdcpHeader");

NS_OBJECT_ENSURE_REGISTERED(NrSlPdcpHeader);

NrSlPdcpHeader::NrSlPdcpHeader()
    : m_sduType(0xff),
      m_pgkIndex(0x0),
      m_secIdentity(0x0),
      m_sequenceNumber(0xfffa)
{
}

NrSlPdcpHeader::~NrSlPdcpHeader()
{
    m_sduType = 0xff;
    m_pgkIndex = 0x0;
    m_secIdentity = 0x0;
    m_sequenceNumber = 0xfffb;
}

void
NrSlPdcpHeader::SetSduType(uint8_t sduType)
{
    m_sduType = sduType & 0x07;
}

void
NrSlPdcpHeader::SetPgkIndex(uint8_t pgkIndex)
{
    m_pgkIndex = pgkIndex & 0x1F;
}

void
NrSlPdcpHeader::SetSecurityIdentity(uint16_t secIdentity)
{
    m_secIdentity = secIdentity;
}

void
NrSlPdcpHeader::SetSequenceNumber(uint16_t sequenceNumber)
{
    m_sequenceNumber = sequenceNumber;
}

uint8_t
NrSlPdcpHeader::GetSduType() const
{
    return m_sduType;
}

uint8_t
NrSlPdcpHeader::GetPgkIndex() const
{
    return m_pgkIndex;
}

uint16_t
NrSlPdcpHeader::GetSecurityIdentity() const
{
    return m_secIdentity;
}

uint16_t
NrSlPdcpHeader::GetSequenceNumber() const
{
    return m_sequenceNumber;
}

TypeId
NrSlPdcpHeader::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlPdcpHeader").SetParent<Header>().AddConstructor<NrSlPdcpHeader>();
    return tid;
}

TypeId
NrSlPdcpHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
NrSlPdcpHeader::Print(std::ostream& os) const
{
    os << "SDU type=" << (uint16_t)m_sduType;
    os << " PGK Index=" << (uint16_t)m_pgkIndex;
    os << " PTK/KD_SESS=" << m_secIdentity;
    os << " SN=" << m_sequenceNumber;
}

uint32_t
NrSlPdcpHeader::GetSerializedSize() const
{
    return 5;
}

void
NrSlPdcpHeader::Serialize(Buffer::Iterator start) const
{
    Buffer::Iterator i = start;

    i.WriteU8((m_sduType << 5) | m_pgkIndex);
    i.WriteU8((m_secIdentity & 0xFF00) >> 8);
    i.WriteU8((m_secIdentity & 0x00FF));
    i.WriteU8((m_sequenceNumber & 0xFF00) >> 8);
    i.WriteU8((m_sequenceNumber & 0x00FF));
}

uint32_t
NrSlPdcpHeader::Deserialize(Buffer::Iterator start)
{
    Buffer::Iterator i = start;
    uint8_t bytes[5];

    for (uint8_t index = 0; index < 5; index++)
    {
        bytes[index] = i.ReadU8();
    }

    m_sduType = (bytes[0] & 0xE0) >> 5;
    m_pgkIndex = bytes[0] & 0x1F;
    m_secIdentity = (bytes[1] << 8) | bytes[2];
    m_sequenceNumber = (bytes[3] << 8) | bytes[4];

    return GetSerializedSize();
}

}; // namespace ns3
