/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "nr-simple-net-device.h"

#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/queue.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSimpleNetDevice");

NS_OBJECT_ENSURE_REGISTERED(NrSimpleNetDevice);

TypeId
NrSimpleNetDevice::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSimpleNetDevice")
                            .SetParent<SimpleNetDevice>()
                            .AddConstructor<NrSimpleNetDevice>();

    return tid;
}

NrSimpleNetDevice::NrSimpleNetDevice()
{
    NS_LOG_FUNCTION(this);
}

NrSimpleNetDevice::NrSimpleNetDevice(Ptr<Node> node)
{
    NS_LOG_FUNCTION(this);
    SetNode(node);
}

NrSimpleNetDevice::~NrSimpleNetDevice()
{
    NS_LOG_FUNCTION(this);
}

void
NrSimpleNetDevice::DoDispose()
{
    NS_LOG_FUNCTION(this);
    SimpleNetDevice::DoDispose();
}

void
NrSimpleNetDevice::DoInitialize()
{
    NS_LOG_FUNCTION(this);
}

bool
NrSimpleNetDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << dest << protocolNumber);
    return SimpleNetDevice::Send(packet, dest, protocolNumber);
}

} // namespace ns3
