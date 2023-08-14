/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#ifndef MCPTT_ON_NETWORK_CALL_MACHINE_CLIENT_STATE_H
#define MCPTT_ON_NETWORK_CALL_MACHINE_CLIENT_STATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg.h"
#include "mcptt-entity-id.h"

namespace ns3 {

namespace psc {

class McpttOnNetworkCallMachineClient;

/**
 * \ingroup mcptt
 *
 * This class represents the on-network client state machine.  In on-network
 * operation, call control is supported by SIP as described in 3GPP TS24.229.
 * SIP conducts transactions to initiate and leave calls, and uses timers
 * to protect such messages.  This state machine logically sits on top of
 * SIP and delegates the signaling transactions to SIP.
 *
 * S1: start-stop
 *  - This state is for when the call is inactive
 *
 * S2: initiating
 *  - This state exists for the UE when it is originating the call, during
 *    the period in which the SIP INVITE transaction is occurring.
 *
 * S3: part of ongoing call
 *  - This state is for when the UE is part of an ongoing call
 *
 * S4: releasing
 *  - This state exists for the UE when it is releasing the call, during
 *    the period in which the SIP BYE transaction is occurring.
 */
class McpttOnNetworkCallMachineClientState : public SimpleRefCount<McpttOnNetworkCallMachineClientState>
{
public:
  /**
   * \brief The destructor of the McpttOnNetworkCallMachineClientState class.
   */
  virtual ~McpttOnNetworkCallMachineClientState (void);
  /**
   * Gets the state ID of this McpttOnNetworkCallMachineClientState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates if the call is active.
   * \param machine The machine that the action is for.
   * \returns True, if the call is active.
   */
  virtual bool IsCallOngoing (const McpttOnNetworkCallMachineClient& machine) const;
  /**
   * Reception of a SIP INVITE
   * \param machine The state machine that the action is for.
   * \param from The MCPTT User ID of the originator
   * \param pkt The packet containing the body of the SIP INVITE
   * \param hdr The SIP header
   */
  virtual void ReceiveInvite (McpttOnNetworkCallMachineClient& machine, uint32_t from, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  /**
   * Reception of a SIP BYE
   * \param machine The state machine that the action is for.
   * \param from The MCPTT User ID of the originator
   * \param pkt The packet containing the body of the SIP BYE
   * \param hdr The SIP header
   */
  virtual void ReceiveBye (McpttOnNetworkCallMachineClient& machine, uint32_t from, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  /**
   * Reception of a SIP response message
   * \param machine The state machine that the action is for.
   * \param from The MCPTT User ID of the originator
   * \param pkt The packet containing the body of the SIP response
   * \param hdr The SIP header
   */
  virtual void ReceiveResponse (McpttOnNetworkCallMachineClient& machine, uint32_t from, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  /**
   * Indicates that the call should be initiated
   * \param machine The state machine that the action is for.
   */
  virtual void InitiateCall (McpttOnNetworkCallMachineClient& machine);
  /**
   * Indicates that the call should be released.
   * \param machine The state machine that the action is for.
   */
  virtual void ReleaseCall (McpttOnNetworkCallMachineClient& machine);

protected:
  /**
   * Creates an instance of the McpttOnNetworkCallMachineClientState class.
   */
  McpttOnNetworkCallMachineClientState (void);
};
/**
 * The output operator of the McpttOnNetworkCallMachineClientState class.
 * \param os The output stream to write to.
 * \param inst The instance to write to the output stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttOnNetworkCallMachineClientState& inst);
/**
 * \ingroup mcptt
 *
 * This class represents The 'S1: start-stop' state of a basic call machine.
 */
class McpttOnNetworkCallMachineClientStateS1 : public McpttOnNetworkCallMachineClientState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkCallMachineClientStateS1 class.
   * \returns The S1 instance.
   */
  static Ptr<McpttOnNetworkCallMachineClientStateS1> GetInstance (void);
  /**
   * Gets the state ID of the McpttOnNetworkCallMachineClientStateS1 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttOnNetworkCallMachineClientStateS1 class.
   */
  McpttOnNetworkCallMachineClientStateS1 (void);
  // Documented in base class
  virtual ~McpttOnNetworkCallMachineClientStateS1 (void);
  virtual McpttEntityId GetInstanceStateId (void) const;
  virtual void InitiateCall (McpttOnNetworkCallMachineClient& machine);
  virtual void ReceiveInvite (McpttOnNetworkCallMachineClient& machine, uint32_t from, Ptr<Packet> pkt, const sip::SipHeader& hdr);
};
/**
 * \ingroup mcptt
 *
 * This class represents the state for the UE when it is originating the
 * call, during the period in which the SIP INVITE transaction is occurring.
 */
class McpttOnNetworkCallMachineClientStateS2 : public McpttOnNetworkCallMachineClientState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkCallMachineClientStateS2 class.
   * \returns The S2 instance.
   */
  static Ptr<McpttOnNetworkCallMachineClientStateS2> GetInstance (void);
  /**
   * Gets the state ID of the McpttOnNetworkCallMachineClientStateS2 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttOnNetworkCallMachineClientStateS2 class.
   */
  McpttOnNetworkCallMachineClientStateS2 (void);
  // Documented in base class
  virtual ~McpttOnNetworkCallMachineClientStateS2 (void);
  virtual McpttEntityId GetInstanceStateId (void) const;
  virtual void ReceiveInvite (McpttOnNetworkCallMachineClient& machine, uint32_t from, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  virtual void ReceiveResponse (McpttOnNetworkCallMachineClient& machine, uint32_t from, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  virtual void ReceiveBye (McpttOnNetworkCallMachineClient& machine, uint32_t from, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  virtual void ReleaseCall (McpttOnNetworkCallMachineClient& machine);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'S3: part of ongoing call' state of a basic call
 * machine.
 */
class McpttOnNetworkCallMachineClientStateS3 : public McpttOnNetworkCallMachineClientState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkCallMachineClientStateS3 class.
   * \returns An instance of the S3 state.
   */
  static Ptr<McpttOnNetworkCallMachineClientStateS3> GetInstance (void);
  /**
   * Gets the state ID of the McpttOnNetworkCallMachineClientStateS3 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttOnNetworkCallMachineClientStateS3 class.
   */
  McpttOnNetworkCallMachineClientStateS3 (void);
  /**
   * \brief The destructor of the McpttOnNetworkCallMachineClientStateS3 class.
   */
  virtual ~McpttOnNetworkCallMachineClientStateS3 (void);
  // Documented in base class
  virtual McpttEntityId GetInstanceStateId (void) const;
  virtual bool IsCallOngoing (const McpttOnNetworkCallMachineClient& machine) const;
  virtual void ReceiveResponse (McpttOnNetworkCallMachineClient& machine, uint32_t from, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  virtual void ReceiveBye (McpttOnNetworkCallMachineClient& machine, uint32_t from, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  virtual void ReleaseCall (McpttOnNetworkCallMachineClient& machine);
};
/**
 * \ingroup mcptt
 *
 * This class represents the state for the UE when it is releasing the call,
 * during the period in which the SIP BYE transaction is occurring.
 */
class McpttOnNetworkCallMachineClientStateS4 : public McpttOnNetworkCallMachineClientState
{
public:
  /**
   * Gets an instance of the McpttOnNetworkCallMachineClientStateS4 class.
   * \returns An instance of the S4 state.
   */
  static Ptr<McpttOnNetworkCallMachineClientStateS4> GetInstance (void);
  /**
   * Gets the state ID of the McpttOnNetworkCallMachineClientStateS4 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttOnNetworkCallMachineClientStateS4 class.
   */
  McpttOnNetworkCallMachineClientStateS4 (void);
  /**
   * \brief The destructor of the McpttOnNetworkCallMachineClientStateS4 class.
   */
  virtual ~McpttOnNetworkCallMachineClientStateS4 (void);
  // Documented in base class
  virtual McpttEntityId GetInstanceStateId (void) const;
  virtual void ReceiveBye (McpttOnNetworkCallMachineClient& machine, uint32_t from, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  virtual void ReceiveResponse (McpttOnNetworkCallMachineClient& machine, uint32_t from, Ptr<Packet> pkt, const sip::SipHeader& hdr);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_ON_NETWORK_CALL_MACHINE_CLIENT_STATE_H */

