/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 TELEMATICS LAB, DEE - Politecnico di Bari
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Giuseppe Piro <g.piro@poliba.it>
 *         Nicola Baldo  <nbaldo@cttc.es>
 */

#ifndef NIST_LTE_NET_DEVICE_H
#define NIST_LTE_NET_DEVICE_H

#include <ns3/net-device.h>
#include <ns3/event-id.h>
#include <ns3/mac48-address.h>
#include <ns3/traced-callback.h>
#include <ns3/nstime.h>
#include <ns3/nist-lte-phy.h>
#include <ns3/nist-lte-control-messages.h>

namespace ns3 {

class Node;
class Packet;

/**
 * \defgroup lte LTE Models
 *
 */

/**
 * \ingroup lte
 *
 * NistLteNetDevice provides  basic implementation for all LTE network devices
 */
class NistLteNetDevice : public NetDevice
{
public:
  static TypeId GetTypeId (void);

  NistLteNetDevice (void);
  virtual ~NistLteNetDevice (void);

  virtual void DoDispose (void);

  // inherited from NetDevice
  virtual void SetIfIndex (const uint32_t index);
  virtual uint32_t GetIfIndex (void) const;
  virtual Ptr<Channel> GetChannel (void) const;
  virtual bool SetMtu (const uint16_t mtu);
  virtual uint16_t GetMtu (void) const;
  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;
  virtual bool IsLinkUp (void) const;
  virtual void AddLinkChangeCallback (Callback<void> callback);
  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;
  virtual bool IsMulticast (void) const;
  virtual bool IsPointToPoint (void) const;
  virtual bool IsBridge (void) const;
  virtual Ptr<Node> GetNode (void) const;
  virtual void SetNode (Ptr<Node> node);
  virtual bool NeedsArp (void) const;
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);
  virtual Address GetMulticast (Ipv4Address addr) const;
  virtual Address GetMulticast (Ipv6Address addr) const;
  virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb); 
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  virtual bool SupportsSendFrom (void) const;

  /** 
   * receive a packet from the lower layers in order to forward it to the upper layers
   * 
   * \param p the packet
   */
  void Receive (Ptr<Packet> p);
  
protected:
  
  NetDevice::ReceiveCallback m_rxCallback;
  
private:
  NistLteNetDevice (const NistLteNetDevice &);
  NistLteNetDevice & operator= (const NistLteNetDevice &);

  Ptr<Node> m_node;

  TracedCallback<> m_linkChangeCallbacks;

  uint32_t m_ifIndex;
  bool m_linkUp;
  mutable uint16_t m_mtu;

  Mac48Address m_address;
};


} // namespace ns3

#endif /* NIST_LTE_NET_DEVICE_H */
