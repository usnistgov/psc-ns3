// Copyright (c) 2011 CTTC
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>

#include "nr-epc-tft.h"

#include "ns3/abort.h"
#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEpcTft");

std::ostream&
operator<<(std::ostream& os, const NrEpcTft::Direction& d)
{
    switch (d)
    {
    case NrEpcTft::DOWNLINK:
        os << "DOWNLINK";
        break;
    case NrEpcTft::UPLINK:
        os << "UPLINK";
        break;
    default:
        os << "BIDIRECTIONAL";
        break;
    }
    return os;
}

bool
operator==(const NrEpcTft::PacketFilter& lhs, const NrEpcTft::PacketFilter& rhs)
{
    return (lhs.direction == rhs.direction && lhs.remoteAddress == rhs.remoteAddress &&
            lhs.remoteMask == rhs.remoteMask && lhs.remoteIpv6Address == rhs.remoteIpv6Address &&
            lhs.remoteIpv6Prefix == rhs.remoteIpv6Prefix && lhs.localAddress == rhs.localAddress &&
            lhs.localMask == rhs.localMask && lhs.localIpv6Address == rhs.localIpv6Address &&
            lhs.localIpv6Prefix == rhs.localIpv6Prefix &&
            lhs.remotePortStart == rhs.remotePortStart && lhs.remotePortEnd == rhs.remotePortEnd &&
            lhs.localPortStart == rhs.localPortStart && lhs.localPortEnd == rhs.localPortEnd &&
            lhs.typeOfService == rhs.typeOfService);
}

std::ostream&
operator<<(std::ostream& os, const NrEpcTft::PacketFilter& f)
{
    os << " direction: " << f.direction << " remoteAddress: " << f.remoteAddress
       << " remoteMask: " << f.remoteMask << " remoteIpv6Address: " << f.remoteIpv6Address
       << " remoteIpv6Prefix: " << f.remoteIpv6Prefix << " localAddress: " << f.localAddress
       << " localMask: " << f.localMask << " localIpv6Address: " << f.localIpv6Address
       << " localIpv6Prefix: " << f.localIpv6Prefix << " remotePortStart: " << f.remotePortStart
       << " remotePortEnd: " << f.remotePortEnd << " localPortStart: " << f.localPortStart
       << " localPortEnd: " << f.localPortEnd << " typeOfService: 0x" << std::hex
       << (uint16_t)f.typeOfService << std::dec << " typeOfServiceMask: 0x" << std::hex
       << (uint16_t)f.typeOfServiceMask << std::dec;
    return os;
}

NrEpcTft::PacketFilter::PacketFilter()
{
    NS_LOG_FUNCTION(this);
}

bool
NrEpcTft::PacketFilter::Matches(Direction d,
                                Ipv4Address ra,
                                Ipv4Address la,
                                uint16_t rp,
                                uint16_t lp,
                                uint8_t tos) const
{
    NS_LOG_FUNCTION(this << d << ra << la << rp << lp << (uint16_t)tos);
    if (d & direction)
    {
        NS_LOG_LOGIC("d matches");
        if (remoteMask.IsMatch(remoteAddress, ra))
        {
            NS_LOG_LOGIC("ra matches remote address " << remoteAddress << " with remoteMask "
                                                      << remoteMask);
            if (localMask.IsMatch(localAddress, la))
            {
                NS_LOG_LOGIC("la matches");
                if (remotePortStart <= rp && rp <= remotePortEnd)
                {
                    NS_LOG_LOGIC("rp matches");
                    if (localPortStart <= lp && lp <= localPortEnd)
                    {
                        NS_LOG_LOGIC("lp matches");
                        if ((tos & typeOfServiceMask) == (typeOfService & typeOfServiceMask))
                        {
                            NS_LOG_LOGIC("tos matches --> have match!");
                            return true;
                        }
                        else
                        {
                            NS_LOG_LOGIC("tos doesn't match: tos="
                                         << tos << " f.tos=" << typeOfService
                                         << " f.tosmask=" << typeOfServiceMask);
                        }
                    }
                    else
                    {
                        NS_LOG_LOGIC("lp doesn't match: lp=" << lp << " f.lps=" << localPortStart
                                                             << " f.lpe=" << localPortEnd);
                    }
                }
                else
                {
                    NS_LOG_LOGIC("rp doesn't match: rp=" << rp << " f.rps=" << remotePortStart
                                                         << " f.lpe=" << remotePortEnd);
                }
            }
            else
            {
                NS_LOG_LOGIC("la doesn't match: la=" << la << " f.la=" << localAddress
                                                     << " f.lmask=" << localMask);
            }
        }
        else
        {
            NS_LOG_LOGIC("ra doesn't match: ra=" << ra << " f.ra=" << remoteAddress
                                                 << " f.rmask=" << remoteMask);
        }
    }
    else
    {
        NS_LOG_LOGIC("d doesn't match: d=0x" << std::hex << d << " f.d=0x" << std::hex << direction
                                             << std::dec);
    }
    return false;
}

bool
NrEpcTft::PacketFilter::Matches(Direction d,
                                Ipv6Address ra,
                                Ipv6Address la,
                                uint16_t rp,
                                uint16_t lp,
                                uint8_t tos) const
{
    NS_LOG_FUNCTION(this << d << ra << la << rp << lp << (uint16_t)tos);
    if (d & direction)
    {
        NS_LOG_LOGIC("d matches");
        if (remoteIpv6Prefix.IsMatch(remoteIpv6Address, ra))
        {
            NS_LOG_LOGIC("ra matches remote address " << remoteIpv6Address << " with remotePrefix "
                                                      << remoteIpv6Prefix);
            if (localIpv6Prefix.IsMatch(localIpv6Address, la))
            {
                NS_LOG_LOGIC("la matches");
                if (remotePortStart <= rp && rp <= remotePortEnd)
                {
                    NS_LOG_LOGIC("rp matches");
                    if (localPortStart <= lp && lp <= localPortEnd)
                    {
                        NS_LOG_LOGIC("lp matches");
                        if ((tos & typeOfServiceMask) == (typeOfService & typeOfServiceMask))
                        {
                            NS_LOG_LOGIC("tos matches --> have match!");
                            return true;
                        }
                        else
                        {
                            NS_LOG_LOGIC("tos doesn't match: tos="
                                         << tos << " f.tos=" << typeOfService
                                         << " f.tosmask=" << typeOfServiceMask);
                        }
                    }
                    else
                    {
                        NS_LOG_LOGIC("lp doesn't match: lp=" << lp << " f.lps=" << localPortStart
                                                             << " f.lpe=" << localPortEnd);
                    }
                }
                else
                {
                    NS_LOG_LOGIC("rp doesn't match: rp=" << rp << " f.rps=" << remotePortStart
                                                         << " f.lpe=" << remotePortEnd);
                }
            }
            else
            {
                NS_LOG_LOGIC("la doesn't match: la=" << la << " f.la=" << localIpv6Address
                                                     << " f.lprefix=" << localIpv6Prefix);
            }
        }
        else
        {
            NS_LOG_LOGIC("ra doesn't match: ra=" << ra << " f.ra=" << remoteIpv6Address
                                                 << " f.rprefix=" << remoteIpv6Prefix);
        }
    }
    else
    {
        NS_LOG_LOGIC("d doesn't match: d=0x" << std::hex << d << " f.d=0x" << std::hex << direction
                                             << std::dec);
    }
    return false;
}

Ptr<NrEpcTft>
NrEpcTft::Default()
{
    Ptr<NrEpcTft> tft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter defaultPacketFilter;
    tft->Add(defaultPacketFilter);
    return tft;
}

NrEpcTft::NrEpcTft()
    : m_numFilters(0)
{
    NS_LOG_FUNCTION(this);
}

NrEpcTft::~NrEpcTft()
{
    NS_LOG_FUNCTION(this);
}

uint8_t
NrEpcTft::Add(PacketFilter f)
{
    NS_LOG_FUNCTION(this << f);

    NS_ABORT_MSG_IF(m_numFilters == 255,
                    "Reached limit of 255 packet filters for the 8-bit integer");

    std::list<PacketFilter>::iterator it;
    for (it = m_filters.begin(); (it != m_filters.end()) && (it->precedence <= f.precedence); ++it)
    {
    }
    m_filters.insert(it, f);
    ++m_numFilters;
    return (m_numFilters - 1);
}

bool
NrEpcTft::Remove(PacketFilter f)
{
    NS_LOG_FUNCTION(this << f);
    NS_ABORT_MSG_IF(m_numFilters == 0, "Trying to remove a packet filter but none exist");

    bool found = false;
    std::list<PacketFilter>::iterator it;
    for (it = m_filters.begin(); it != m_filters.end(); ++it)
    {
        if (*it == f)
        {
            m_filters.erase(it);
            --m_numFilters;
            found = true;
            break;
        }
    }
    return found;
}

bool
NrEpcTft::Matches(Direction direction,
                  Ipv4Address remoteAddress,
                  Ipv4Address localAddress,
                  uint16_t remotePort,
                  uint16_t localPort,
                  uint8_t typeOfService) const
{
    NS_LOG_FUNCTION(this << direction << remoteAddress << localAddress << remotePort << localPort
                         << (uint16_t)typeOfService);
    for (auto it = m_filters.begin(); it != m_filters.end(); ++it)
    {
        if (it->Matches(direction,
                        remoteAddress,
                        localAddress,
                        remotePort,
                        localPort,
                        typeOfService))
        {
            return true;
        }
    }
    return false;
}

bool
NrEpcTft::Matches(Direction direction,
                  Ipv6Address remoteAddress,
                  Ipv6Address localAddress,
                  uint16_t remotePort,
                  uint16_t localPort,
                  uint8_t typeOfService) const
{
    NS_LOG_FUNCTION(this << direction << std::hex << remoteAddress << localAddress << std::dec
                         << remotePort << localPort << (uint16_t)typeOfService);
    for (auto it = m_filters.begin(); it != m_filters.end(); ++it)
    {
        if (it->Matches(direction,
                        remoteAddress,
                        localAddress,
                        remotePort,
                        localPort,
                        typeOfService))
        {
            return true;
        }
    }
    return false;
}

std::list<NrEpcTft::PacketFilter>
NrEpcTft::GetPacketFilters() const
{
    NS_LOG_FUNCTION(this);
    return m_filters;
}

} // namespace ns3
