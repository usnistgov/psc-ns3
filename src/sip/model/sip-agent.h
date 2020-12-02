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

#ifndef SIP_AGENT_H
#define SIP_AGENT_H

#include <ns3/type-id.h>
#include "sip-header.h"
#include "sip-element.h"

namespace ns3 {

namespace sip {

/**
 * \ingroup sip
 *
 * A SipAgent notionally represents what RFC 3261 calls an end system.
 * or User Agent.  It implements both the User Agent Client (UAC) and User
 * Agent Server (UAS) roles.
 *
 */
class SipAgent : public SipElement
{
public:
  /**
   * \brief Construct a SIP agent
   */
  SipAgent ();
  /**
   * \brief Destructor
   */
  virtual ~SipAgent ();
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

protected:
  void DoDispose (void);

};

} // namespace sip

} // namespace ns3

#endif /* SIP_AGENT_H */
