//
// SPDX-License-Identifier: NIST-Software
//

#include "nr-sl-tft.h"

#include "ns3/abort.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlTft");

NrSlTft::NrSlTft(BearerType bearerType, const struct SidelinkInfo& slInfo)
    : m_bearerType(bearerType),
      m_sidelinkInfo(slInfo)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("Adding SL TFT to destination L2 ID " << slInfo.m_dstL2Id);
    NS_ASSERT_MSG((slInfo.m_dstL2Id & 0xFF000000) == 0, "Destination L2 id must be 24 bits");
}

NrSlTft::NrSlTft(BearerType bearerType, Ipv4Address remoteAddr, const struct SidelinkInfo& slInfo)
    : m_bearerType(bearerType),
      m_sidelinkInfo(slInfo)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("Adding SL TFT to " << remoteAddr << " destination L2 ID " << slInfo.m_dstL2Id);
    NS_ASSERT_MSG((slInfo.m_dstL2Id & 0xFF000000) == 0, "Destination L2 id must be 24 bits");
    PacketFilter f;
    f.remoteAddress = remoteAddr;
    f.remoteMask = Ipv4Mask("255.255.255.255");
    Add(f);
}

NrSlTft::NrSlTft(BearerType bearerType, Ipv6Address remoteAddr, const struct SidelinkInfo& slInfo)
    : m_bearerType(bearerType),
      m_sidelinkInfo(slInfo)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("Adding SL TFT to " << remoteAddr << " destination L2 ID " << slInfo.m_dstL2Id);
    NS_ASSERT_MSG((slInfo.m_dstL2Id & 0xFF000000) == 0, "Destination L2 id must be 24 bits");
    PacketFilter f;
    f.remoteIpv6Address = remoteAddr;
    f.remoteIpv6Prefix = Ipv6Prefix(64);
    Add(f);
}

NrSlTft::NrSlTft(BearerType bearerType,
                 Ipv4Address remoteAddr,
                 uint16_t remotePort,
                 const struct SidelinkInfo& slInfo)
    : m_bearerType(bearerType),
      m_sidelinkInfo(slInfo)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("Adding SL TFT to " << remoteAddr << " port " << remotePort
                                     << " destination L2 ID " << slInfo.m_dstL2Id);
    NS_ASSERT_MSG((slInfo.m_dstL2Id & 0xFF000000) == 0, "Destination L2 id must be 24 bits");
    PacketFilter f;
    f.remoteAddress = remoteAddr;
    f.remoteMask = Ipv4Mask("255.255.255.255");
    f.remotePortStart = remotePort;
    f.remotePortEnd = remotePort;
    Add(f);
}

NrSlTft::NrSlTft(BearerType bearerType,
                 Ipv6Address remoteAddr,
                 uint16_t remotePort,
                 const struct SidelinkInfo& slInfo)
    : m_bearerType(bearerType),
      m_sidelinkInfo(slInfo)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("Adding SL TFT to " << remoteAddr << " port " << remotePort
                                     << " destination L2 ID " << slInfo.m_dstL2Id);
    NS_ASSERT_MSG((slInfo.m_dstL2Id & 0xFF000000) == 0, "Destination L2 id must be 24 bits");
    PacketFilter f;
    f.remoteIpv6Address = remoteAddr;
    f.remoteIpv6Prefix = Ipv6Prefix(64);
    f.remotePortStart = remotePort;
    f.remotePortEnd = remotePort;
    Add(f);
}

NrSlTft::NrSlTft(Ptr<NrSlTft> tft)
{
    NS_LOG_FUNCTION(this << tft);
    m_bearerType = tft->m_bearerType;
    m_sidelinkInfo = tft->m_sidelinkInfo;
    m_harqEnabled = tft->m_harqEnabled;
    m_delayBudget = tft->m_delayBudget;
    m_filters = tft->m_filters;
    m_numFilters = tft->m_numFilters;
}

NrSlTft::~NrSlTft()
{
    NS_LOG_FUNCTION(this);
}

bool
NrSlTft::Matches(Ipv4Address ra) const
{
    NS_LOG_FUNCTION(this << ra);
    return NrEpcTft::Matches(NrEpcTft::Direction::BIDIRECTIONAL, ra, Ipv4Address(), 0, 0, 0);
}

bool
NrSlTft::Matches(Ipv6Address ra) const
{
    NS_LOG_FUNCTION(this << ra);
    return NrEpcTft::Matches(Direction::BIDIRECTIONAL, ra, Ipv6Address(), 0, 0, 0);
}

bool
NrSlTft::Matches(Ipv6Address ra, uint16_t rp)
{
    NS_LOG_FUNCTION(this << ra << rp);
    return NrEpcTft::Matches(Direction::BIDIRECTIONAL, ra, Ipv6Address(), rp, 0, 0);
}

bool
NrSlTft::Matches(Ipv4Address ra, uint16_t rp)
{
    NS_LOG_FUNCTION(this << ra << rp);
    return NrEpcTft::Matches(Direction::BIDIRECTIONAL, ra, Ipv4Address(), rp, 0, 0);
}

struct SidelinkInfo
NrSlTft::GetSidelinkInfo() const
{
    return m_sidelinkInfo;
}

void
NrSlTft::SetSidelinkInfoLcId(uint8_t lcId)
{
    m_sidelinkInfo.m_lcId = lcId;
}

bool
NrSlTft::IsReceive() const
{
    // receiving if RECEIVE or BIDIRECTIONAL
    NS_ASSERT_MSG(m_bearerType != NrSlTft::BearerType::INVALID, "Invalid TFT direction");
    return m_bearerType != NrSlTft::BearerType::TRANSMIT;
}

bool
NrSlTft::IsTransmit() const
{
    // transmitting if TRANSMIT or BIDIRECTIONAL
    NS_ASSERT_MSG(m_bearerType != NrSlTft::BearerType::INVALID, "Invalid TFT direction");
    return m_bearerType != NrSlTft::BearerType::RECEIVE;
}

bool
NrSlTft::IsUnicast() const
{
    NS_ASSERT_MSG(m_sidelinkInfo.m_castType != SidelinkInfo::CastType::Invalid,
                  "Invalid TFT communication type");
    return (m_sidelinkInfo.m_castType == SidelinkInfo::CastType::Unicast);
}

bool
NrSlTft::IsHarqEnabled() const
{
    return m_harqEnabled;
}

Time
NrSlTft::GetDelayBudget() const
{
    return m_delayBudget;
}

bool
operator==(const SidelinkInfo a, const SidelinkInfo b)
{
    return (a.m_castType == b.m_castType && a.m_srcL2Id == b.m_srcL2Id &&
            a.m_dstL2Id == b.m_dstL2Id && a.m_harqEnabled == b.m_harqEnabled &&
            a.m_pdb == b.m_pdb && a.m_dynamic == b.m_dynamic && a.m_rri == b.m_rri &&
            a.m_lcId == b.m_lcId && a.m_priority == b.m_priority);
}

} // namespace ns3
