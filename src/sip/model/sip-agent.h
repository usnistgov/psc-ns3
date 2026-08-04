/**
 * Copyright 2020 University of Washington
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 *
 */

#ifndef SIP_AGENT_H
#define SIP_AGENT_H

#include "sip-element.h"
#include "sip-header.h"

#include "ns3/type-id.h"

namespace ns3
{

namespace sip
{

/**
 * @ingroup sip
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
     * @brief Construct a SIP agent
     */
    SipAgent();
    /**
     * @brief Destructor
     */
    ~SipAgent() override;
    /**
     * @brief Get the type ID.
     * @return the object TypeId
     */
    static TypeId GetTypeId();

  protected:
    void DoDispose() override;
};

} // namespace sip

} // namespace ns3

#endif /* SIP_AGENT_H */
