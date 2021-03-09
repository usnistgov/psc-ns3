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

#ifndef MCPTT_TEST_CALL_H
#define MCPTT_TEST_CALL_H

#include <ns3/mcptt-call.h>
#include <ns3/sip-header.h>
#include "mcptt-msg-dropper.h"

namespace ns3 {
namespace psc {
namespace tests {

/**
 * \ingroup mcptt
 *
 * This class subclasses ns3::psc::McpttCall and allows the ability to insert
 * McpttMsgDroppers to the receive path for testing purposes.
 */
class McpttTestCall : public McpttCall
{
public:
  /**
   * Gets the type ID of the McpttTestCall class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttTestCall class.
   * \param callType network type for call
   */
  McpttTestCall (McpttCall::NetworkCallType callType);
  /**
   * The destructor of the McpttTestCall class.
   */
  virtual ~McpttTestCall (void);
  /**
   * Receives a call message.
   * \param pkt The packet (serialized with SIP header)
   * \param hdr A reference to the SIP header that has been serialized
   */
  virtual void Receive (Ptr<Packet> pkt, const sip::SipHeader& hdr);
  /**
   * Receives a call message.
   * \param msg The message that was received.
   */
  virtual void Receive (const McpttCallMsg& msg);
  /**
   * Receives a floor message.
   * \param msg The message that was received.
   */
  virtual void Receive (const McpttFloorMsg& msg);
  /**
   * Receive a media message.
   * \param msg The message that was received.
   */
  virtual void Receive (const McpttMediaMsg& msg);
  /**
   * Add a message dropper
   * \param dropper The message dropper to add.
   */
  virtual void AddDropper (Ptr<McpttMsgDropper>  dropper);

protected:
  void DoDispose (void);

private:
  bool ShouldDrop (const McpttMsg& msg);
  std::vector<Ptr<McpttMsgDropper> > m_droppers;
};

} // namespace tests
} // namespace psc
} // namespace ns3

#endif /* MCPTT_TEST_CALL_H */

