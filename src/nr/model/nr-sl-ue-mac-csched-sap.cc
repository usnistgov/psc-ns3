/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
*   Copyright (c) 2022 University of Washington
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation;
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "nr-sl-ue-mac-csched-sap.h"

namespace ns3 {

std::ostream &operator<< (std::ostream &os, const NrSlUeMacCschedSapProvider::SidelinkLogicalChannelInfo &p)
{
  os << "dstL2Id: " << p.dstL2Id
     << " lcId: " << static_cast<uint16_t> (p.lcId)
     << " lcGroup: " << static_cast<uint16_t> (p.lcGroup)
     << " pqi: " << static_cast<uint16_t> (p.pqi)
     << " priority: " << static_cast<uint16_t> (p.priority)
     << " isGbr: " << p.isGbr
     << " mbr: " << p.mbr
     << " gbr: " << p.gbr;
  return os;
}

} // namespace ns3
