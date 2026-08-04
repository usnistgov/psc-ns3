// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-harq-vector.h"

namespace ns3
{

bool
NrMacHarqVector::Erase(uint8_t id)
{
    at(id).Erase();
    --m_usedSize;

    uint32_t count = 0;
    for (const auto& v : *this)
    {
        if (v.second.m_active)
        {
            ++count;
        }
    }
    NS_ASSERT(count == m_usedSize);
    return true;
}

bool
NrMacHarqVector::Insert(uint8_t* id, const HarqProcess& element)
{
    if (m_usedSize >= m_maxSize)
    {
        return false;
    }

    NS_ABORT_IF(element.m_active == false);

    *id = FirstAvailableId();
    if (*id == 255)
    {
        return false;
    }

    NS_ABORT_IF(at(*id).m_active == true);
    at(*id) = element;

    NS_ABORT_IF(at(*id).m_active == false);
    NS_ABORT_IF(this->FirstAvailableId() == *id);

    ++m_usedSize;
    return true;
}

std::ostream&
operator<<(std::ostream& os, const NrMacHarqVector& item)
{
    for (const auto& p : item)
    {
        os << "Process ID " << static_cast<uint32_t>(p.first) << ": " << p.second << std::endl;
    }
    return os;
}

} // namespace ns3
