/*
 * Copyright (c) 2009 Drexel University
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Tom Wambold <tom5760@gmail.com>
 */

#ifndef NHDP_INFO_BASE_H
#define NHDP_INFO_BASE_H

#include "ns3/ipv4-interface-address.h"
#include "ns3/nstime.h"

#include <stdint.h>
#include <vector>

namespace ns3
{

/** Just so I don't need to duplicate all the Ptr methods */
class TupleBase
{
  public:
    void Ref() const;
    void Unref() const;

  private:
    mutable uint32_t m_refCount;
};

typedef std::vector<Ipv4InterfaceAddress>::iterator NhdpTupleAddressIterator;
typedef std::vector<Ipv4InterfaceAddress>::const_iterator ConstNhdpTupleAddressIterator;

struct NhdpLocalInterfaceTuple : public TupleBase
{
    std::vector<Ipv4InterfaceAddress> addrList;
    bool isManet;
};

struct NhdpRemovedInterfaceAddressTuple : public TupleBase
{
    Ipv4InterfaceAddress addr;
    Time time;
};

struct NhdpLinkTuple : public TupleBase
{
    std::vector<Ipv4InterfaceAddress> neighborIfaceAddrList;
    Time heardTime;
    Time symTime;
    float quality;
    bool pending;
    bool lost;
    Time time;
};

struct NhdpTwoHopTuple : public TupleBase
{
    std::vector<Ipv4InterfaceAddress> neighborIfaceAddrList;
    Ipv4InterfaceAddress twoHopAddr;
    Time time;
};

struct NhdpNeighborTuple : public TupleBase
{
    std::vector<Ipv4InterfaceAddress> neighborAddrList;
    bool symmetric;
};

struct NhdpLostNeighborTuple : public TupleBase
{
    Ipv4InterfaceAddress address;
    Time time;
};

} // namespace ns3

#endif /* NHDP_INFO_BASE_H */
