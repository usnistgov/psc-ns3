// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-short-bsr-ce.h"

#include <ns3/log.h>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrMacShortBsrCe);
NS_LOG_COMPONENT_DEFINE("NrMacShortBsrCe");

TypeId
NrMacShortBsrCe::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrMacShortBsrCe").SetParent<Header>().AddConstructor<NrMacShortBsrCe>();
    return tid;
}

TypeId
NrMacShortBsrCe::GetInstanceTypeId() const
{
    return GetTypeId();
}

NrMacShortBsrCe::NrMacShortBsrCe()
{
    NS_LOG_FUNCTION(this);
    m_header.SetLcId(NrMacHeaderFsUl::SHORT_BSR);
}

void
NrMacShortBsrCe::Serialize(Buffer::Iterator start) const
{
    NS_LOG_FUNCTION(this);

    NS_ASSERT(m_bufferSizeLevel_0 <= 31);
    NS_ASSERT(m_bufferSizeLevel_1 <= 31);
    NS_ASSERT(m_bufferSizeLevel_2 <= 31);
    NS_ASSERT(m_bufferSizeLevel_3 <= 31);

    m_header.Serialize(start);
    start.Next(m_header.GetSerializedSize());

    start.WriteU8(m_bufferSizeLevel_0);
    start.WriteU8(m_bufferSizeLevel_1);
    start.WriteU8(m_bufferSizeLevel_2);
    start.WriteU8(m_bufferSizeLevel_3);
}

uint32_t
NrMacShortBsrCe::Deserialize(Buffer::Iterator start)
{
    NS_LOG_FUNCTION(this);

    auto readBytes = m_header.Deserialize(start);
    start.Next(readBytes);
    NS_ASSERT(m_header.GetLcId() == NrMacHeaderFsUl::SHORT_BSR);

    m_bufferSizeLevel_0 = start.ReadU8();
    m_bufferSizeLevel_1 = start.ReadU8();
    m_bufferSizeLevel_2 = start.ReadU8();
    m_bufferSizeLevel_3 = start.ReadU8();

    return GetSerializedSize();
}

uint32_t
NrMacShortBsrCe::GetSerializedSize() const
{
    NS_LOG_FUNCTION(this);
    return m_header.GetSerializedSize() + 4;
}

void
NrMacShortBsrCe::Print(std::ostream& os) const
{
    NS_LOG_FUNCTION(this);
    os << "LCG0: " << m_bufferSizeLevel_0;
    os << "LCG1: " << m_bufferSizeLevel_1;
    os << "LCG2: " << m_bufferSizeLevel_2;
    os << "LCG3: " << m_bufferSizeLevel_3;
}

bool
NrMacShortBsrCe::operator==(const NrMacShortBsrCe& o) const
{
    return m_bufferSizeLevel_0 == o.m_bufferSizeLevel_0 &&
           m_bufferSizeLevel_1 == o.m_bufferSizeLevel_1 &&
           m_bufferSizeLevel_2 == o.m_bufferSizeLevel_2 &&
           m_bufferSizeLevel_3 == o.m_bufferSizeLevel_3;
}

uint8_t
NrMacShortBsrCe::FromBytesToLevel(uint64_t bufferSize)
{
    // Table 6.1.3-1 TS 38.321 V15.3.0
    static std::vector<uint64_t> lookupVector = {
        0,     10,    14,    20,    28,    38,    53,    74,     102,   142,  198,
        276,   384,   535,   745,   1038,  1446,  2014,  2806,   3909,  5446, 7587,
        10570, 14726, 20516, 28581, 39818, 55474, 77284, 107669, 150000};

    uint8_t index = 0;
    NS_ASSERT(lookupVector.back() == 150000);
    if (bufferSize > lookupVector.back())
    {
        index = 31;
    }
    else if (bufferSize == 0)
    {
        index = 0;
    }
    else
    {
        while (lookupVector[index] < bufferSize)
        {
            index++;
        }
    }

    NS_ASSERT(index <= 31);

    return index;
}

uint64_t
NrMacShortBsrCe::FromLevelToBytes(uint8_t bufferLevel)
{
    // Table 6.1.3-1 TS 38.321 V15.3.0
    static std::vector<uint64_t> lookupVector = {
        0,     10,    14,    20,    28,    38,    53,    74,     102,   142,  198,
        276,   384,   535,   745,   1038,  1446,  2014,  2806,   3909,  5446, 7587,
        10570, 14726, 20516, 28581, 39818, 55474, 77284, 107669, 150000};

    if (bufferLevel > lookupVector.size() - 1)
    {
        // The value is > 150000. We cannot return exactly 150000, otherwise the
        // value would have been 30. So we return something big...
        return 150000 * 8;
    }

    return lookupVector[bufferLevel];
}

} // namespace ns3
