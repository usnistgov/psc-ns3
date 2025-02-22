/*
 * Copyright (c) 2009 Drexel University
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Tom Wambold <tom5760@gmail.com>
 */

#include "nhdp-info-base.h"

namespace ns3
{

namespace nhdp
{

void
TupleBase::Ref() const
{
    m_refCount++;
}

void
TupleBase::Unref() const
{
    m_refCount--;
    if (m_refCount == 0)
    {
        delete this;
    }
}

} // namespace nhdp

} // namespace ns3
