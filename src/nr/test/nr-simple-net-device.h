/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#ifndef NR_SIMPLE_NET_DEVICE_H
#define NR_SIMPLE_NET_DEVICE_H

#include "ns3/error-model.h"
#include "ns3/event-id.h"
#include "ns3/node.h"
#include "ns3/nr-rlc.h"
#include "ns3/simple-channel.h"
#include "ns3/simple-net-device.h"

namespace ns3
{

/**
 * \ingroup nr
 * The NrSimpleNetDevice class implements the NR simple net device.
 * This class is used to provide a limited NrNetDevice functionalities that
 * are necessary for testing purposes.
 */
class NrSimpleNetDevice : public SimpleNetDevice
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    NrSimpleNetDevice();
    /**
     * Constructor
     *
     * \param node the Node
     */
    NrSimpleNetDevice(Ptr<Node> node);

    ~NrSimpleNetDevice() override;
    void DoDispose() override;

    // inherited from NetDevice
    bool Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) override;

  protected:
    // inherited from Object
    void DoInitialize() override;
};

} // namespace ns3

#endif // NR_SIMPLE_NET_DEVICE_H
