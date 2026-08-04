// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-sl-phy-mac-common.h"

namespace ns3
{

bool
SlResourceInfo::operator<(const SlResourceInfo& rhs) const
{
    return (sfn < rhs.sfn || (sfn == rhs.sfn && slSubchannelStart < rhs.slSubchannelStart));
}

void
SlResourceInfo::SetSlotBusy(bool isBusy)
{
    slotBusy = isBusy;
}

bool
SlResourceInfo::GetSlotBusy() const
{
    return slotBusy;
}

bool
SlGrantResource::operator<(const SlGrantResource& rhs) const
{
    return (sfn < rhs.sfn);
}

bool
NrSlVarTtiAllocInfo::operator<(const NrSlVarTtiAllocInfo& rhs) const
{
    return (symStart < rhs.symStart);
}

std::ostream&
operator<<(std::ostream& os, const SensingData& p)
{
    os << "SfnSf: " << p.sfn << " rsvp: " << p.rsvp << " sbChLength: " << +p.sbChLength
       << " sbChStart: " << +p.sbChStart << " prio: " << +p.prio << " slRsrp: " << p.slRsrp
       << " gapReTx1: " << +p.gapReTx1 << " sbChStartReTx1: " << +p.sbChStartReTx1
       << " gapReTx2: " << +p.gapReTx2 << " sbChStartReTx2: " << +p.sbChStartReTx2;
    return os;
}

std::ostream&
operator<<(std::ostream& os, const ReservedResource& p)
{
    os << "SfnSf: " << p.sfn << " rsvp: " << p.rsvp << " sbChLength: " << +p.sbChLength
       << " sbChStart: " << +p.sbChStart << " prio: " << +p.prio << " slRsrp: " << p.slRsrp;
    return os;
}

std::ostream&
operator<<(std::ostream& os, const SlGrantResource& p)
{
    os << "SfnSf: " << p.sfn << " dstL2Id: " << p.dstL2Id
       << " ndi: " << static_cast<uint16_t>(p.ndi) << " rv: " << static_cast<uint16_t>(p.rv)
       << " priority: " << static_cast<uint16_t>(p.priority) << " mcs: " << p.mcs
       << " numSlPscchRbs: " << p.numSlPscchRbs << " slPscchSymStart: " << p.slPscchSymStart
       << " slPscchSymLength: " << p.slPscchSymLength << " slPsschSymStart: " << p.slPsschSymStart
       << " slPsschSymLength: " << p.slPsschSymLength
       << " slPsschSubChStart: " << p.slPsschSubChStart
       << " slPsschSubChLength: " << p.slPsschSubChLength
       << " maxNumPerReserve: " << p.maxNumPerReserve << " txSci1A: " << p.txSci1A
       << " slotNumInd: " << static_cast<uint16_t>(p.slotNumInd);
    return os;
}

std::ostream&
operator<<(std::ostream& os, const SlHarqInfo& item)
{
    if (item.IsReceivedOk())
    {
        os << "ACK feedback ";
    }
    else
    {
        os << "NACK feedback ";
    }
    os << "for ProcessID: " << static_cast<uint32_t>(item.m_harqProcessId) << " of UE "
       << static_cast<uint32_t>(item.m_rnti) << " Tx RNTI: " << static_cast<uint32_t>(item.m_txRnti)
       << " BWP index: " << static_cast<uint32_t>(item.m_bwpIndex);
    return os;
}

} // namespace ns3
