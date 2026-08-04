// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>
// Modified by: CTTC for NR Sidelink

#include "nr-sl-rlc.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlRlc");

NS_OBJECT_ENSURE_REGISTERED(NrSlRlc);

NrSlRlc::NrSlRlc()
{
    NS_LOG_FUNCTION(this);
    m_nrSlMacSapUser = new MemberNrSlMacSapUser<NrSlRlc>(this);
    m_nrSlRlcSapProvider = new MemberNrSlRlcSapProvider<NrSlRlc>(this);
}

NrSlRlc::~NrSlRlc()
{
    NS_LOG_FUNCTION(this);
    delete m_nrSlMacSapUser;
    delete m_nrSlRlcSapProvider;
}

TypeId
NrSlRlc::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlRlc").SetParent<NrRlc>().SetGroupName("Nr");
    return tid;
}

void
NrSlRlc::DoDispose()
{
    NS_LOG_FUNCTION(this);
    NrRlc::DoDispose();
}

void
NrSlRlc::SetNrSlMacSapProvider(NrSlMacSapProvider* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlMacSapProvider = s;
}

NrSlMacSapUser*
NrSlRlc::GetNrSlMacSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlMacSapUser;
}

NrSlRlcSapProvider*
NrSlRlc::GetNrSlRlcSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlRlcSapProvider;
}

void
NrSlRlc::SetNrSlRlcSapUser(NrSlRlcSapUser* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlRlcSapUser = s;
}

void
NrSlRlc::SetSourceL2Id(uint32_t src)
{
    NS_LOG_FUNCTION(this << src);
    m_srcL2Id = src;
}

void
NrSlRlc::SetDestinationL2Id(uint32_t dst)
{
    NS_LOG_FUNCTION(this << dst);
    m_dstL2Id = dst;
}

void
NrSlRlc::SetRlcChannelType(NrSlRlc::ChannelType channelType)
{
    NS_LOG_FUNCTION(this);

    switch (channelType)
    {
    case STCH:
        m_channelType = STCH;
        break;
    default:
        m_channelType = DEFAULT;
        break;
    }
}

////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(NrSlRlcSm);

NrSlRlcSm::NrSlRlcSm()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrSlRlcSm::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlRlcSm")
                            .SetParent<NrSlRlc>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrSlRlcSm>();
    return tid;
}

void
NrSlRlcSm::DoTransmitPdcpPdu(Ptr<Packet> p)
{
    NS_FATAL_ERROR("RLC SM does not support NR Sidelink");
}

void
NrSlRlcSm::DoNotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams)
{
    NS_FATAL_ERROR("RLC SM does not support NR Sidelink");
}

void
NrSlRlcSm::DoNotifyHarqDeliveryFailure()
{
    NS_FATAL_ERROR("RLC SM does not support NR Sidelink");
}

void
NrSlRlcSm::DoReceivePdu(NrMacSapUser::ReceivePduParameters rxPduParams)
{
    NS_FATAL_ERROR("RLC SM does not support NR Sidelink");
}

void
NrSlRlcSm::DoTransmitNrSlPdcpPdu(const NrSlRlcSapProvider::NrSlTransmitPdcpPduParameters& params)
{
    NS_FATAL_ERROR("RLC SM does not support NR Sidelink");
}

void
NrSlRlcSm::DoNotifyNrSlTxOpportunity(const NrSlMacSapUser::NrSlTxOpportunityParameters& params)
{
    NS_FATAL_ERROR("RLC SM does not support NR Sidelink");
}

void
NrSlRlcSm::DoReceiveNrSlRlcPdu(NrSlMacSapUser::NrSlReceiveRlcPduParameters rxPduParams)
{
    NS_FATAL_ERROR("RLC SM does not support NR Sidelink");
}

} // namespace ns3
