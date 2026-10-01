// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-sl-rlc-am.h"

#include "ns3/fatal-error.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlRlcAm");

NS_OBJECT_ENSURE_REGISTERED(NrSlRlcAm);

NrSlRlcAm::NrSlRlcAm()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrSlRlcAm::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlRlcAm")
                            .SetParent<NrSlRlc>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrSlRlcAm>();
    return tid;
}

void
NrSlRlcAm::DoTransmitPdcpPdu(Ptr<Packet> p)
{
    NS_FATAL_ERROR("RLC AM does not support NR Sidelink");
}

void
NrSlRlcAm::DoNotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams)
{
    NS_FATAL_ERROR("RLC AM does not support NR Sidelink");
}

void
NrSlRlcAm::DoNotifyHarqDeliveryFailure()
{
    NS_FATAL_ERROR("RLC AM does not support NR Sidelink");
}

void
NrSlRlcAm::DoReceivePdu(NrMacSapUser::ReceivePduParameters rxPduParams)
{
    NS_FATAL_ERROR("RLC AM does not support NR Sidelink");
}

void
NrSlRlcAm::DoTransmitNrSlPdcpPdu(const NrSlRlcSapProvider::NrSlTransmitPdcpPduParameters& params)
{
    NS_FATAL_ERROR("RLC AM does not support NR Sidelink");
}

void
NrSlRlcAm::DoNotifyNrSlTxOpportunity(const NrSlMacSapUser::NrSlTxOpportunityParameters& params)
{
    NS_FATAL_ERROR("RLC AM does not support NR Sidelink");
}

void
NrSlRlcAm::DoReceiveNrSlRlcPdu(NrSlMacSapUser::NrSlReceiveRlcPduParameters rxPduParams)
{
    NS_FATAL_ERROR("RLC AM does not support NR Sidelink");
}

} // namespace ns3
