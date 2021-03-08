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

#ifndef MCPTT_SERVER_CALL_MACHINE_GROUP_PREARRANGED_STATE_H
#define MCPTT_SERVER_CALL_MACHINE_GROUP_PREARRANGED_STATE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-call-msg.h"
#include "mcptt-entity-id.h"

namespace ns3 {

class McpttServerCallMachineGroupPrearranged;

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class represents the on-network server state machine.  In on-network
 * operation, call control is supported by SIP as described in 3GPP TS24.229.
 * SIP conducts transactions to initiate and leave calls, and uses timers
 * to protect such messages.  This state machine logically sits on top of
 * SIP and delegates the signaling transactions to SIP.
 *
 * S1: start-stop
 *  - This state is for when the call is inactive
 *
 * S2: initiating
 *  - This state exists for the server when it is originating the call, during
 *    the period in which the SIP INVITE transactions are occurring.
 *
 * S3: part of ongoing call
 *  - This state is for when the server is managing an ongoing call
 *
 * S4: releasing
 *  - This state exists for the server when it is releasing the call, during
 *    the period in which the SIP BYE transactions are occurring.
 *
 * Typically, an initiating UE will send an INVITE which will lead to
 * the INVITE being sent to all other UEs in the call.  With automatic
 * commencement, the exchange will look like:
 *
 * Initiating UE A ---- INVITE ---> Server (moves to state S2)
 *                                  Server---->INVITE --> UE B
 *                                  Server---->INVITE --> UE C
 *                                  Server<--- 200 OK --  UE B (moves to S3)
 *                                  Server<--- 200 OK --  UE C (moves to S3)
 * Initiating UE A <---- 200 OK --- Server (moves to state S3)
 *
 * At this point, the server will have moved to state S3.  UE A typically has floor (implicit floor control) and is in
 * state S3.  UE B and C will be in state S3 without the floor.
 *
 * A similar exchange is used for release.
 *
 * The server delegates to SIP to manage the transactions, and keeps track
 * of which clients have completed the transaction.
 */
class McpttServerCallMachineGroupPrearrangedState : public SimpleRefCount<McpttServerCallMachineGroupPrearrangedState>
{
public:
  /**
   * \brief The destructor of the McpttServerCallMachineGroupPrearrangedState class.
   */
  virtual ~McpttServerCallMachineGroupPrearrangedState (void);
  /**
   * Gets the state ID of this McpttServerCallMachineGroupPrearrangedState instance.
   * \returns The state ID.
   */
  virtual McpttEntityId GetInstanceStateId (void) const;
  /**
   * Indicates if the call is active.
   * \param machine The machine that the action is for.
   * \returns True, if the call is active.
   */
  virtual bool IsCallOngoing (const McpttServerCallMachineGroupPrearranged& machine) const;
  /**
   * Reception of a SIP INVITE
   * \param machine The state machine that the action is for.
   * \param pkt The packet containing SDP information
   * \param hdr The deserialized SIP header
   */
  virtual void ReceiveInvite (McpttServerCallMachineGroupPrearranged& machine, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  /**
   * Reception of a SIP BYE
   * \param machine The state machine that the action is for.
   * \param pkt The packet containing SDP information
   * \param hdr The deserialized SIP header
   */
  virtual void ReceiveBye (McpttServerCallMachineGroupPrearranged& machine, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  /**
   * Reception of a SIP response message
   * \param machine The state machine that the action is for.
   * \param pkt The packet containing SDP information
   * \param hdr The deserialized SIP header
   */
  virtual void ReceiveResponse (McpttServerCallMachineGroupPrearranged& machine, Ptr<Packet> pkt, const sip::SipHeader& hdr);

protected:
  /**
   * Creates an instance of the McpttServerCallMachineGroupPrearrangedState class.
   */
  McpttServerCallMachineGroupPrearrangedState (void);
};
/**
 * The output operator of the McpttServerCallMachineGroupPrearrangedState class.
 * \param os The output stream to write to.
 * \param inst The instance to write to the output stream.
 * \returns The output stream that was written to.
 */
std::ostream& operator<< (std::ostream& os, const McpttServerCallMachineGroupPrearrangedState& inst);
/**
 * \ingroup mcptt
 *
 * This class represents The 'S1: start-stop' state of a basic call machine.
 */
class McpttServerCallMachineGroupPrearrangedStateS1 : public McpttServerCallMachineGroupPrearrangedState
{
public:
  /**
   * Gets an instance of the McpttServerCallMachineGroupPrearrangedStateS1 class.
   * \returns The S1 instance.
   */
  static Ptr<McpttServerCallMachineGroupPrearrangedStateS1> GetInstance (void);
  /**
   * Gets the state ID of the McpttServerCallMachineGroupPrearrangedStateS1 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttServerCallMachineGroupPrearrangedStateS1 class.
   */
  McpttServerCallMachineGroupPrearrangedStateS1 (void);
  // Documented in base class
  virtual ~McpttServerCallMachineGroupPrearrangedStateS1 (void);
  virtual McpttEntityId GetInstanceStateId (void) const;
  virtual void ReceiveInvite (McpttServerCallMachineGroupPrearranged& machine, Ptr<Packet> pkt, const sip::SipHeader& hdr);
};
/**
 * \ingroup mcptt
 *
 * This class represents the state for the UE when it is originating the
 * call, during the period in which the SIP INVITE transaction is occurring.
 */
class McpttServerCallMachineGroupPrearrangedStateS2 : public McpttServerCallMachineGroupPrearrangedState
{
public:
  /**
   * Gets an instance of the McpttServerCallMachineGroupPrearrangedStateS2 class.
   * \returns The S2 instance.
   */
  static Ptr<McpttServerCallMachineGroupPrearrangedStateS2> GetInstance (void);
  /**
   * Gets the state ID of the McpttServerCallMachineGroupPrearrangedStateS2 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttServerCallMachineGroupPrearrangedStateS2 class.
   */
  McpttServerCallMachineGroupPrearrangedStateS2 (void);
  // Documented in base class
  virtual ~McpttServerCallMachineGroupPrearrangedStateS2 (void);
  virtual McpttEntityId GetInstanceStateId (void) const;
  virtual void ReceiveInvite (McpttServerCallMachineGroupPrearranged& machine, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  virtual void ReceiveResponse (McpttServerCallMachineGroupPrearranged& machine, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  virtual void ReceiveBye (McpttServerCallMachineGroupPrearranged& machine, Ptr<Packet> pkt, const sip::SipHeader& hdr);
};
/**
 * \ingroup mcptt
 *
 * This class represents The 'S3: part of ongoing call' state of a basic call
 * machine.
 */
class McpttServerCallMachineGroupPrearrangedStateS3 : public McpttServerCallMachineGroupPrearrangedState
{
public:
  /**
   * Gets an instance of the McpttServerCallMachineGroupPrearrangedStateS3 class.
   * \returns An instance of the S3 state.
   */
  static Ptr<McpttServerCallMachineGroupPrearrangedStateS3> GetInstance (void);
  /**
   * Gets the state ID of the McpttServerCallMachineGroupPrearrangedStateS3 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttServerCallMachineGroupPrearrangedStateS3 class.
   */
  McpttServerCallMachineGroupPrearrangedStateS3 (void);
  /**
   * \brief The destructor of the McpttServerCallMachineGroupPrearrangedStateS3 class.
   */
  virtual ~McpttServerCallMachineGroupPrearrangedStateS3 (void);
  // Documented in base class
  virtual McpttEntityId GetInstanceStateId (void) const;
  virtual bool IsCallOngoing (const McpttServerCallMachineGroupPrearranged& machine) const;
  virtual void ReceiveResponse (McpttServerCallMachineGroupPrearranged& machine, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  virtual void ReceiveBye (McpttServerCallMachineGroupPrearranged& machine, Ptr<Packet> pkt, const sip::SipHeader& hdr);
};
/**
 * \ingroup mcptt
 *
 * This class represents the state for the UE when it is releasing the call,
 * during the period in which the SIP BYE transaction is occurring.
 */
class McpttServerCallMachineGroupPrearrangedStateS4 : public McpttServerCallMachineGroupPrearrangedState
{
public:
  /**
   * Gets an instance of the McpttServerCallMachineGroupPrearrangedStateS4 class.
   * \returns An instance of the S4 state.
   */
  static Ptr<McpttServerCallMachineGroupPrearrangedStateS4> GetInstance (void);
  /**
   * Gets the state ID of the McpttServerCallMachineGroupPrearrangedStateS4 class.
   * \returns The state ID.
   */
  static McpttEntityId GetStateId (void);
  /**
   * Creates an instance of the McpttServerCallMachineGroupPrearrangedStateS4 class.
   */
  McpttServerCallMachineGroupPrearrangedStateS4 (void);
  /**
   * \brief The destructor of the McpttServerCallMachineGroupPrearrangedStateS4 class.
   */
  virtual ~McpttServerCallMachineGroupPrearrangedStateS4 (void);
  // Documented in base class
  virtual McpttEntityId GetInstanceStateId (void) const;
  virtual void ReceiveBye (McpttServerCallMachineGroupPrearranged& machine, Ptr<Packet> pkt, const sip::SipHeader& hdr);
  virtual void ReceiveResponse (McpttServerCallMachineGroupPrearranged& machine, Ptr<Packet> pkt, const sip::SipHeader& hdr);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_SERVER_CALL_MACHINE_GROUP_PREARRANGED_STATE_H */
