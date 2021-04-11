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

#include <ns3/data-rate.h>
#include <ns3/event-id.h>
#include <ns3/log.h>
#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/simulator.h>
#include <ns3/type-id.h>
#include <ns3/uinteger.h>

#include "mcptt-media-msg.h"
#include "mcptt-media-sink.h"
#include "mcptt-media-src.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttMediaSrc");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttMediaSrc);

TypeId
McpttMediaSrc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttMediaSrc")
    .SetParent<Object> ()
    .AddConstructor<McpttMediaSrc> ()
    .AddAttribute ("Bytes", "The number of data bytes to send per packet.",
                   UintegerValue (8),
                   MakeUintegerAccessor (&McpttMediaSrc::m_bytes),
                   MakeUintegerChecker <uint16_t> ())
    .AddAttribute ("DataRate", "The data rate at which data should be sent.",
                   DataRateValue (DataRate ("68.75b/s")),
                   MakeDataRateAccessor (&McpttMediaSrc::m_dataRate),
                   MakeDataRateChecker ())
  ;
  return tid;
}

McpttMediaSrc::McpttMediaSrc (void)
  : Object (),
    m_lastReq (Seconds (0)),
    m_reqEvent (EventId ()),
    m_sink (0),
    m_started (false),
    m_totalBytes (0),
    m_nextSeqNum (0)
{
  NS_LOG_FUNCTION (this);
}

McpttMediaSrc::~McpttMediaSrc (void)
{
  NS_LOG_FUNCTION (this);
}

bool
McpttMediaSrc::HasSink (void)
{
  NS_LOG_FUNCTION (this);

  McpttMediaSink* sink = GetSink ();

  bool hasSink = (sink != 0);

  if (hasSink)
    {
      NS_LOG_LOGIC ("Requeseter has sink (" << sink << ").");
    }
  else
    {
      NS_LOG_LOGIC ("Requester has NO sink.");
    }

  return hasSink;
}

bool
McpttMediaSrc::IsMakingReq (void) const
{
  NS_LOG_FUNCTION (this);

  return m_started;
}

void
McpttMediaSrc::StartMakingReq (void)
{
  NS_LOG_FUNCTION (this);

  m_started = true;
  m_startTime = Simulator::Now ();

  NS_LOG_LOGIC ("Requester starting to make request.");

  MakeRequest ();
}

void
McpttMediaSrc::StopMakingReq (void)
{
  NS_LOG_FUNCTION (this);

  m_started = false;

  NS_LOG_LOGIC ("Requester stopping request.");

  CancelRequest ();
}

void
McpttMediaSrc::AddToTotalBytes (uint16_t numBytes)
{
  NS_LOG_FUNCTION (this << numBytes);

  uint16_t totalBytes = GetTotalBytes ();

  NS_LOG_LOGIC ("Requester requested to send a total of " << (totalBytes + numBytes) << " byte(s).");

  totalBytes += numBytes;

  SetTotalBytes (totalBytes);
}

void
McpttMediaSrc::CancelRequest (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Requester canceling pending request.");

  EventId reqEvent = GetReqEvent ();

  Simulator::Cancel (reqEvent);
}

void
McpttMediaSrc::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  SetSink (0);

  Object::DoDispose ();
}

void
McpttMediaSrc::MakeRequest (void)
{
  NS_LOG_FUNCTION (this);

  McpttMediaSink* sink = GetSink ();

  McpttRtpHeader hdr;
  // RTP timestamp semantics are application-specific.  Here, we will try
  // to make best use of 32 bits.  If units are in tens of microseconds,
  // we can run for 42949 seconds (almost 12 hours) without rolling over.
  hdr.SetTimestamp (static_cast<uint32_t> (Simulator::Now ().GetMicroSeconds () / 10));
  hdr.SetSeqNum (m_nextSeqNum++);
  McpttMediaMsg msg (hdr, m_bytes, m_startTime);

  uint32_t size = msg.GetSerializedSize ();

  NS_LOG_LOGIC ("Requester making request to send " << m_bytes << " data byte(s), for a total of " << size << " byte(s).");

  if (sink->TakeSendReq (msg))
    {
      NS_LOG_LOGIC ("Request was taken");

      AddToTotalBytes (size);
    }
  else
    {
      NS_LOG_LOGIC ("Request was NOT taken.");
    }

  ScheduleNextReq ();
}

void
McpttMediaSrc::ScheduleNextReq (void)
{
  NS_LOG_FUNCTION (this);

  uint16_t bits = m_bytes * 8;

  Time nextReq (Seconds (bits / static_cast<double> (m_dataRate.GetBitRate ())));

  NS_LOG_LOGIC ("Requester scheduling to make request in " << nextReq.As (Time::S) << ".");

  EventId reqEvent = Simulator::Schedule (nextReq, &McpttMediaSrc::MakeRequest, this);

  SetReqEvent (reqEvent);
}

McpttMediaSink*
McpttMediaSrc::GetSink (void) const
{
  NS_LOG_FUNCTION (this);

  return m_sink;
}

uint16_t
McpttMediaSrc::GetTotalBytes (void) const
{
  NS_LOG_FUNCTION (this);

  return m_totalBytes;
}

void
McpttMediaSrc::SetSink (McpttMediaSink* const& sink)
{
  NS_LOG_FUNCTION (this << sink);

  m_sink = sink;
}

Time
McpttMediaSrc::GetLastReq (void) const
{
  NS_LOG_FUNCTION (this);

  return m_lastReq;
}

EventId
McpttMediaSrc::GetReqEvent (void) const
{
  NS_LOG_FUNCTION (this);

  return m_reqEvent;
}

void
McpttMediaSrc::SetLastReq (const Time& lastReq)
{
  NS_LOG_FUNCTION (this << lastReq);

  m_lastReq = lastReq;
}

void
McpttMediaSrc::SetReqEvent (const EventId& reqEvent)
{
  NS_LOG_FUNCTION (this);

  m_reqEvent = reqEvent;
}

void
McpttMediaSrc::SetTotalBytes (uint16_t totalBytes)
{
  NS_LOG_FUNCTION (this << totalBytes);

  m_totalBytes = totalBytes;
}

} // namespace psc
} // namespace ns3

