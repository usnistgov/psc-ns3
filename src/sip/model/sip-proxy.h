/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright 2020 University of Washington
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
 */

#ifndef SIP_PROXY_H
#define SIP_PROXY_H

#include <unordered_map>
#include <functional>
#include <tuple>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/packet.h>
#include <ns3/type-id.h>
#include <ns3/traced-callback.h>
#include <ns3/callback.h>
#include "sip-element.h"

namespace ns3 {

namespace sip {

class SipHeader;

/**
 * \ingroup sip
 *
 * A SipProxy notionally represents a SIP Proxy on a server.  The model
 * does not distinguish between different variants of SIP proxies.
 * The SipProxy is the peer to the client-based SipAgent, and exists
 * primarily to manage transactions and dialogs for one or more calls.
 */
class SipProxy : public SipElement
{
public:
  /**
   * \brief Construct a SIP proxy
   */
  SipProxy ();
  /**
   * \brief Destructor
   */
  virtual ~SipProxy ();
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Send a SIP response with status code
   *
   * \param p the packet to add a SIP header to
   * \param addr the address to send the packet to
   * \param statusCode the status code
   * \param from the from field
   * \param to the to field
   * \param callId the callId
   * \param sendCallback the callback to use to send the messages
   */
  virtual void SendResponse (Ptr<Packet> p, const Address& addr, uint16_t statusCode, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback);

  /**
   * Schedule Timer C (proxy INVITE transaction timeout)
   * \param id Transaction ID
   */
  virtual void ScheduleTimerC (TransactionId id);
  /**
   * \brief Cancel Timer C
   * \param id Transaction ID
   */
  virtual void CancelTimerC (TransactionId id);
  /**
   * Handle Timer C expiry (proxy INVITE transaction timeout)
   * \param id Transaction ID
   */
  virtual void HandleTimerC (TransactionId id);

protected:
  void DoDispose (void);
private:
  Time m_proxyInviteTransactionTimeout; //!< Timeout value for Timer C
};

} // namespace sip

} // namespace ns3

#endif /* SIP_PROXY_H */
