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

enum class LinkStatus
{
    PENDING,
    HEARD,
    SYMMETRIC,
    LOST
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

struct LinkTuple
{
    LinkTuple(Ipv4Address addr, double quality)
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
    std::vector<Ipv4Address> m_neighborAddrList;
    Ipv4Address m_twoHopAddr;
    Time m_expirationTime;
};

} // namespace nhdp

} // namespace ns3

#endif /* NHDP_INFO_BASE_H */
