/*
 * Copyright (c) 2009 Drexel University
 *
 * SPDX-License-Identifier: GPL-2.0-only and NIST-Software
 *
 * Author: Tom Wambold <tom5760@gmail.com>
 */

#ifndef NHDP_INFO_BASE_H
#define NHDP_INFO_BASE_H

#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"

#include <stdint.h>
#include <vector>

namespace ns3
{

namespace nhdp
{

enum class NeighborStatus
{
    NEW,
    MODIFIED,
    REMOVED
};

enum class LinkStatus
{
    PENDING,
    HEARD,
    SYMMETRIC,
    LOST
};

enum class TwoHopStatus
{
    NEW,
    REMOVED
};

struct NeighborTuple
{
    NeighborTuple(Ipv4Address addr)
    {
        m_neighborAddrList.push_back(addr);
    }

    std::vector<Ipv4Address> m_neighborAddrList;
    bool m_symmetric{false};
};

struct LostNeighborTuple
{
    Ipv4Address m_neighborAddr;
    Time m_expirationTime;
};

struct LinkTuple
{
    LinkTuple(Ipv4Address addr)
    {
        m_neighborAddrList.push_back(addr);
    }

    LinkStatus GetLinkStatus() const;

    std::vector<Ipv4Address> m_neighborAddrList;
    Time m_heardTime;
    Time m_symTime;
    double m_quality{0};
    bool m_pending{false};
    bool m_lost{false};
    Time m_expirationTime;
};

struct TwoHopTuple
{
    TwoHopTuple(Ipv4Address addr, Ipv4Address twoHopAddr)
    {
        m_neighborAddrList.push_back(addr);
        m_twoHopAddr = twoHopAddr;
    }

    std::vector<Ipv4Address> m_neighborAddrList;
    Ipv4Address m_twoHopAddr;
    Time m_expirationTime;
};

std::ostream& operator<<(std::ostream& os, const LinkStatus& status);
std::ostream& operator<<(std::ostream& os, const NeighborStatus& status);
std::ostream& operator<<(std::ostream& os, const TwoHopStatus& status);

} // namespace nhdp

} // namespace ns3

#endif /* NHDP_INFO_BASE_H */
