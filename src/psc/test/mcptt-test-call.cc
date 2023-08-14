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

#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/sip-header.h"
#include "ns3/mcptt-on-network-call-machine-client.h"
#include "ns3/mcptt-ptt-app.h"
#include "mcptt-test-call.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttTestCall");

namespace psc {
namespace tests {

NS_OBJECT_ENSURE_REGISTERED (McpttTestCall);

TypeId
McpttTestCall::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttTestCall")
    .SetParent<McpttCall> ()
  ;
  return tid;
}

McpttTestCall::McpttTestCall (McpttCall::NetworkCallType callType)
  : McpttCall (callType)
{
  NS_LOG_FUNCTION (this);
}

McpttTestCall::~McpttTestCall (void)
{
  NS_LOG_FUNCTION (this);
}

bool
McpttTestCall::ShouldDrop (const McpttMsg& msg)
{
  bool drop = false;
  for (std::vector<Ptr<McpttMsgDropper> >::iterator it = m_droppers.begin (); it != m_droppers.end () && drop == false; it++)
    {
      drop = (*it)->ShouldDropMsg (msg);
    }
  return drop;
}

void
McpttTestCall::Receive (Ptr<Packet> pkt, const sip::SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << pkt << hdr);
  NS_ASSERT_MSG (hdr.GetCallId () == GetCallId (), "Received message for wrong call ID");
  NS_LOG_DEBUG ("Received SIP packet for call ID " << GetCallId ());
  Ptr<Packet> pktCopy = pkt->Copy ();
  pktCopy->AddHeader (hdr);
  GetOwner ()->TraceMessageReceive (GetCallId (), pktCopy, hdr.GetInstanceTypeId ());
  GetCallMachine ()->GetObject<McpttOnNetworkCallMachineClient> ()->ReceiveCallPacket (pkt, hdr);
}

void
McpttTestCall::Receive (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (ShouldDrop (msg))
    {
      NS_LOG_DEBUG ("Dropping McpttCallMsg");
      return;
    }
  GetCallMachine ()->Receive (msg);
}

void
McpttTestCall::Receive (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (ShouldDrop (msg))
    {
      NS_LOG_DEBUG ("Dropping McpttFloorMsg");
      return;
    }

  GetFloorMachine ()->Receive (msg);
}

void
McpttTestCall::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  if (ShouldDrop (msg))
    {
      NS_LOG_DEBUG ("Dropping McpttMediaMsg");
      return;
    }

  GetCallMachine ()->Receive (msg);
  GetFloorMachine ()->Receive (msg);
}

void
McpttTestCall::DoDispose (void)
{
  m_droppers.clear ();
  McpttCall::DoDispose ();
}

void
McpttTestCall::AddDropper (Ptr<McpttMsgDropper>  dropper)
{
  m_droppers.push_back (dropper);
}

} // namespace tests
} // namespace psc
} // namespace ns3

