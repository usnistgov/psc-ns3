/*
 *   Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   SPDX-License-Identifier: GPL-2.0-only
 *
 *
 *
 */

#include "nr-sl-ue-cmac-sap.h"

namespace ns3
{

NrSlUeCmacSapProvider::~NrSlUeCmacSapProvider()
{
}

NrSlUeCmacSapUser::~NrSlUeCmacSapUser()
{
}

std::ostream&
operator<<(std::ostream& os, const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& p)
{
    os << "srcL2Id: " << p.srcL2Id << " dstL2Id: " << p.dstL2Id
       << " lcId: " << static_cast<uint16_t>(p.lcId)
       << " lcGroup: " << static_cast<uint16_t>(p.lcGroup)
       << " pqi: " << static_cast<uint16_t>(p.pqi)
       << " priority: " << static_cast<uint16_t>(p.priority) << " isGbr: " << p.isGbr
       << " mbr: " << p.mbr << " gbr: " << p.gbr
       << " castType: " << static_cast<uint16_t>(p.castType) << " harqEnabled: " << p.harqEnabled
       << " pdb: " << p.pdb.As(Time::MS) << " dynamic: " << p.dynamic
       << " rri: " << p.rri.As(Time::MS);
    return os;
}

} // namespace ns3
