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

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttMediaMsg);

TypeId
McpttMediaMsg::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttMediaMsg")
    .SetParent<McpttMsg> ()
    .AddConstructor<McpttMediaMsg> ()
  ;

  return tid;
}

McpttMediaMsg::McpttMediaMsg (void)
  : McpttMsg (),
    m_payloadSize (0),
    m_talkSpurtStart (Seconds (0))
{
  NS_LOG_FUNCTION (this);
}

McpttMediaMsg::McpttMediaMsg (const McpttRtpHeader& header)
  : McpttMsg (),
    m_header (header),
    m_payloadSize (0),
    m_talkSpurtStart (Seconds (0))
{
  NS_LOG_FUNCTION (this);
}

McpttMediaMsg::McpttMediaMsg (uint16_t payloadSize)
  : McpttMsg (),
    m_payloadSize (payloadSize),
    m_talkSpurtStart (Seconds (0))
{
  NS_LOG_FUNCTION (this << payloadSize);
}

McpttMediaMsg::McpttMediaMsg (const McpttRtpHeader& header, uint16_t payloadSize)
  : McpttMsg (),
    m_header (header),
    m_payloadSize (payloadSize),
    m_talkSpurtStart (Seconds (0))
{
  NS_LOG_FUNCTION (this << payloadSize);
}

McpttMediaMsg::McpttMediaMsg (const McpttRtpHeader& header, uint16_t payloadSize, Time talkSpurtStart)
  : McpttMsg (),
    m_header (header),
    m_payloadSize (payloadSize),
    m_talkSpurtStart (talkSpurtStart)
{
  NS_LOG_FUNCTION (this << payloadSize);
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

  uint32_t bytesRead = m_header.Deserialize (start);

  uint16_t index = 0;
  start.Next (bytesRead);

  uint16_t payloadSize = start.ReadNtohU16 ();
  bytesRead += 2;
  index += 2;
  NS_ABORT_MSG_UNLESS (payloadSize >= 6, "Payload size must be at least six bytes");
  m_talkSpurtStart = MicroSeconds (10 * start.ReadNtohU32 ());
  bytesRead += 4;
  index += 4;
  for (; index < payloadSize; index++)
    {
      start.ReadU8 (); // null bytes
      bytesRead += 1;
    }

  m_payloadSize = payloadSize;

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
  uint32_t ssrc = m_header.GetSsrc ();
  return ssrc;
}

uint32_t
McpttMediaMsg::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);

  uint32_t size = 0;
  size += m_header.GetSerializedSize ();
  size += m_payloadSize; // (payloadSize) dummy bytes after the header.

  return size;
}

void
McpttMediaMsg::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this << &os);

  os << "McpttMediaMsg (";
  os << "header=" << m_header;
  os << ";payloadSize=" << m_payloadSize;
  os << ")";
}

void
McpttMediaMsg::SetSsrc (const uint32_t ssrc)
{
  m_header.SetSsrc (ssrc);
}

void
McpttMediaMsg::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttMediaMsg serializing...");
  m_header.Serialize (start);
  uint32_t headerSize = m_header.GetSerializedSize ();

  start.Next (headerSize);

  NS_ABORT_MSG_UNLESS (m_payloadSize >= 6, "Payload size must be at least six bytes");
  uint16_t index = 0;
  start.WriteHtonU16 (m_payloadSize);
  index += 2;

  // Similar to RTP timestamp, we try to make best use of 32 bits. If units
  // are in tens of microseconds,  we can run for 12 hours without rolling over.
  start.WriteHtonU32 (static_cast<uint32_t> (m_talkSpurtStart.GetMicroSeconds () / 10));
  index += 4;

  // Zero-fill the remaining payload
  for ( ; index < m_payloadSize; index++)
    {
      start.WriteU8 (0);
    }

  NS_LOG_LOGIC ("McpttMediaMsg wrote " << m_payloadSize << " payload bytes.");
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
McpttMediaMsg::GetHeader (void) const
{
  NS_LOG_FUNCTION (this);

  return m_header;
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
McpttMediaMsg::SetHeader (const McpttRtpHeader& header)
{
  NS_LOG_FUNCTION (this << header);

  m_header = header;
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

} // namespace psc
} // namespace ns3
