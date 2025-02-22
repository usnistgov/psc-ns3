/*
 * Copyright (c) 2009 Drexel University
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Tom Wambold <tom5760@gmail.com>
 */

#include "nhdp-info-base.h"

#include "ns3/simulator.h"

namespace ns3
{

namespace nhdp
{

// RFC 6130, Section 7.1
LinkStatus
LinkTuple::GetLinkStatus() const
{
    if (m_pending)
    {
        return LinkStatus::PENDING;
    }
    else if (m_lost)
    {
        return LinkStatus::LOST;
    }
    else if (m_symTime > Simulator::Now())
    {
        return LinkStatus::SYMMETRIC;
    }
    else if (m_heardTime > Simulator::Now())
    {
        return LinkStatus::HEARD;
    }
    return LinkStatus::LOST;
}

} // namespace nhdp

} // namespace ns3
