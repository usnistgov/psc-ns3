/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
*   Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
*
*/


#include "nr-sl-phy-mac-common.h"

namespace ns3 {

bool
NrSlSlotAlloc::operator < (const NrSlSlotAlloc &rhs) const
{
  return (sfn < rhs.sfn || (sfn == rhs.sfn && slPsschSubChStart < rhs.slPsschSubChStart));
}

bool
NrSlVarTtiAllocInfo::operator < (const NrSlVarTtiAllocInfo &rhs) const
{
  return (symStart < rhs.symStart);
}

std::ostream &operator<< (std::ostream &os, const SensingData& p)
{
  os << "SfnSf: " << p.sfn
     << " rsvp: " << p.rsvp
     << " sbChLength: " << +p.sbChLength
     << " sbChStart: " << +p.sbChStart
     << " prio: " << +p.prio
     << " slRsrp: " << p.slRsrp
     << " gapReTx1: " << +p.gapReTx1
     << " sbChStartReTx1: " << +p.sbChStartReTx1
     << " gapReTx2: " << +p.gapReTx2
     << " sbChStartReTx2: " << +p.sbChStartReTx2;
  return os;
}

std::ostream &operator<< (std::ostream &os, const SlotSensingData& p)
{
  os << "SfnSf: " << p.sfn
     << " rsvp: " << p.rsvp
     << " sbChLength: " << +p.sbChLength
     << " sbChStart: " << +p.sbChStart
     << " prio: " << +p.prio
     << " slRsrp: " << p.slRsrp;
  return os;
}

std::ostream &operator<< (std::ostream &os, const NrSlSlotAlloc& p)
{
  os << "SfnSf: " << p.sfn
     << " dstL2Id: " << p.dstL2Id
     << " ndi: " << static_cast<uint16_t> (p.ndi)
     << " rv: " << static_cast<uint16_t> (p.rv)
     << " priority: " << static_cast<uint16_t> (p.priority)
     << " mcs: " << p.mcs
     << " numSlPscchRbs: " << p.numSlPscchRbs
     << " slPscchSymStart: " << p.slPscchSymStart
     << " slPscchSymLength: " << p.slPscchSymLength
     << " slPsschSymStart: " << p.slPsschSymStart
     << " slPsschSymLength: " << p.slPsschSymLength
     << " slPsschSubChStart: " << p.slPsschSubChStart
     << " slPsschSubChLength: " << p.slPsschSubChLength
     << " maxNumPerReserve: " << p.maxNumPerReserve
     << " txSci1A: " << p.txSci1A
     << " slotNumInd: " << static_cast<uint16_t> (p.slotNumInd);
  return os;
}

}
