// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-sl-rlc-tm.h"

#include "ns3/fatal-error.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlRlcTm");

NS_OBJECT_ENSURE_REGISTERED(NrSlRlcTm);

NrSlRlcTm::NrSlRlcTm()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrSlRlcTm::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlRlcTm")
                            .SetParent<NrSlRlc>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrSlRlcTm>();
    return tid;
}

void
NrSlRlcTm::DoTransmitPdcpPdu(Ptr<Packet> p)
{
    NS_FATAL_ERROR("RLC TM does not support NR Sidelink");
}

void
NrSlRlcTm::DoNotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams)
{
    NS_FATAL_ERROR("RLC TM does not support NR Sidelink");
}

void
NrSlRlcTm::DoNotifyHarqDeliveryFailure()
{
    NS_FATAL_ERROR("RLC TM does not support NR Sidelink");
}

void
NrSlRlcTm::DoReceivePdu(NrMacSapUser::ReceivePduParameters rxPduParams)
{
    NS_FATAL_ERROR("RLC TM does not support NR Sidelink");
}

void
NrSlRlcTm::DoTransmitNrSlPdcpPdu(const NrSlRlcSapProvider::NrSlTransmitPdcpPduParameters& params)
{
    NS_FATAL_ERROR("RLC TM does not support NR Sidelink");
}

void
NrSlRlcTm::DoNotifyNrSlTxOpportunity(const NrSlMacSapUser::NrSlTxOpportunityParameters& params)
{
    NS_FATAL_ERROR("RLC TM does not support NR Sidelink");
}

void
NrSlRlcTm::DoReceiveNrSlRlcPdu(NrSlMacSapUser::NrSlReceiveRlcPduParameters rxPduParams)
{
    NS_FATAL_ERROR("RLC TM does not support NR Sidelink");
}

} // namespace ns3
