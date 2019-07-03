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

#include <ns3/log.h>
#include <ns3/type-id.h>

#include "mcptt-call-machine-private.h"
#include "mcptt-floor-msg-sink.h"
#include "mcptt-msg.h"
#include "mcptt-rtp-header.h"

#include "mcptt-media-msg.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttMediaMsg");

NS_OBJECT_ENSURE_REGISTERED (McpttMediaMsg);

TypeId
McpttMediaMsg::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::McpttMediaMsg")
    .SetParent<McpttMsg> ()
    .AddConstructor<McpttMediaMsg> ()
  ;

  return tid;
}

McpttMediaMsg::McpttMediaMsg (void)
  : McpttMsg (),
    m_head (McpttRtpHeader ()),
    m_payloadSize (0),
    m_talkSpurtStart (Seconds (0))
{
  NS_LOG_FUNCTION (this);
}

McpttMediaMsg::McpttMediaMsg (const McpttRtpHeader& head)
  : McpttMsg (),
    m_head (head),
    m_payloadSize (0),
    m_talkSpurtStart (Seconds (0))
{
  NS_LOG_FUNCTION (this);
}

McpttMediaMsg::McpttMediaMsg (uint16_t payloadSize)
  : McpttMsg (),
    m_head (McpttRtpHeader ()),
    m_payloadSize (payloadSize),
    m_talkSpurtStart (Seconds (0))
{
  NS_LOG_FUNCTION (this);
}

McpttMediaMsg::McpttMediaMsg (const McpttRtpHeader& head, uint16_t payloadSize)
  : McpttMsg (),
    m_head (head),
    m_payloadSize (payloadSize),
    m_talkSpurtStart (Seconds (0))
{
  NS_LOG_FUNCTION (this);
}

McpttMediaMsg::McpttMediaMsg (const McpttRtpHeader& head, uint16_t payloadSize, Time talkSpurtStart)
  : McpttMsg (),
    m_head (head),
    m_payloadSize (payloadSize),
    m_talkSpurtStart (talkSpurtStart)
{
  NS_LOG_FUNCTION (this);
}

McpttMediaMsg::~McpttMediaMsg (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttMediaMsg::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttMediaMsg deserializing...");

  McpttRtpHeader head = GetHead ();
  uint32_t bytesRead = head.Deserialize (start);

  uint16_t index = 0;
  start.Next (bytesRead);

  uint16_t payloadSize = start.ReadNtohU16 ();
  NS_ASSERT_MSG (payloadSize >=2, "Payload size must be at least two bytes");
  bytesRead += 2;
  index += 2;

  if (payloadSize >= 10)
    {
      m_talkSpurtStart = Time (static_cast<int64_t> (start.ReadU64 ()));
      bytesRead += 8;
      index += 8;
    }
  for (; index < payloadSize; index++)
    {
      start.ReadU8 ();
      bytesRead += 1;
    }

  SetHead (head);
  SetPayloadSize (payloadSize);

  NS_LOG_LOGIC ("McpttMediaMsg read " << payloadSize << " payload bytes.");

  return bytesRead;
}

TypeId
McpttMediaMsg::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttMediaMsg::GetTypeId ();
}

uint32_t
McpttMediaMsg::GetSsrc (void) const
{
  NS_LOG_FUNCTION (this);

  McpttRtpHeader header = GetHead ();
  uint32_t ssrc = header.GetSsrc ();

  return ssrc;
}

uint32_t
McpttMediaMsg::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);

  McpttRtpHeader head = GetHead ();
  uint32_t payloadSize = GetPayloadSize ();

  uint32_t size = 0;
  size += head.GetSerializedSize ();
  size += payloadSize; // (payloadSize) dummy bytes after the header.

  return size;
}

void
McpttMediaMsg::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this << &os);

  McpttRtpHeader head = GetHead ();
  uint32_t payloadSize = GetPayloadSize ();

  os << "McpttMediaMsg (";
  os << "head=" << head;
  os << ";payloadSize=" << payloadSize;
  os << ")";
}

void
McpttMediaMsg::SetSsrc (const uint32_t ssrc)
{
  m_head.SetSsrc (ssrc);
}

void
McpttMediaMsg::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this);

  McpttRtpHeader head = GetHead ();
  uint16_t payloadSize = GetPayloadSize ();

  NS_LOG_LOGIC ("McpttMediaMsg serializing...");

  head.Serialize (start);
  uint32_t headSize = head.GetSerializedSize ();

  start.Next (headSize);

  NS_ASSERT_MSG (payloadSize >=2, "Payload size must be at least two bytes");
  start.WriteHtonU16 (payloadSize);

  uint16_t index = 0;
  if (payloadSize >= 10)
    {
      // There is space to encode the talkspurt timestamp
      start.WriteU64 (static_cast<uint64_t> (m_talkSpurtStart.GetTimeStep ()));
      index += 8;
    }
  else
    {
       NS_ASSERT_MSG (true, "If this assert is hit, it is a sign that the "
                      "simulation is using small McpttMediaMsg payload sizes, " 
                      "and this class should be changed to encode the "
                      "talkspurt in a Packet Tag.");
    }

  // Zero-fill the remaining payload
  for ( ; index < (payloadSize - 2); index++)
    {
      start.WriteU8 (0);
    }

  NS_LOG_LOGIC ("McpttMediaMsg wrote " << payloadSize << " payload bytes.");
}

void
McpttMediaMsg::Visit (McpttCallMachinePrivate& callMachine) const
{
  NS_LOG_FUNCTION (this << &callMachine);

  callMachine.ReceiveMedia (*this);
}

void
McpttMediaMsg::Visit (McpttFloorMsgSink& floorMachine) const
{
  NS_LOG_FUNCTION (this << &floorMachine);

  floorMachine.ReceiveMedia (*this);
}

McpttRtpHeader
McpttMediaMsg::GetHead (void) const
{
  NS_LOG_FUNCTION (this);

  return m_head;
}

uint16_t
McpttMediaMsg::GetPayloadSize (void) const
{
  NS_LOG_FUNCTION (this);

  return m_payloadSize;
}

Time
McpttMediaMsg::GetTalkSpurtStart (void) const
{
  NS_LOG_FUNCTION (this);

  return m_talkSpurtStart;
}

void
McpttMediaMsg::SetHead (const McpttRtpHeader& head)
{
  NS_LOG_FUNCTION (this << head);

  m_head = head;
}

void
McpttMediaMsg::SetPayloadSize (uint16_t payloadSize)
{
  NS_LOG_FUNCTION (this << payloadSize);

  m_payloadSize = payloadSize;
}

void
McpttMediaMsg::SetTalkSpurtStart (Time talkSpurtStart)
{
  NS_LOG_FUNCTION (this << talkSpurtStart);

  m_talkSpurtStart = talkSpurtStart;
}


} // namespace ns3
