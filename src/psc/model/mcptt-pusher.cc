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

#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/log.h>
#include <ns3/nstime.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/random-variable-stream.h>
#include <ns3/simulator.h>
#include <ns3/string.h>
#include <ns3/type-id.h>
#include <ns3/uinteger.h>

#include "mcptt-pushable.h"
#include "mcptt-pusher.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttPusher");

NS_OBJECT_ENSURE_REGISTERED (McpttPusher);

TypeId
McpttPusher::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttPusher")
    .SetParent<Object> ()
    .AddConstructor<McpttPusher>()
    .AddAttribute ("Automatic", "Indicates if pushes/releases should occur automatically.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttPusher::m_automatic),
                   MakeBooleanChecker ())
    .AddAttribute ("PushVariable", "The variable used for automatic pushes.",
                   StringValue ("ns3::NormalRandomVariable[Mean=5.0|Variance=2.0]"),
                   MakePointerAccessor (&McpttPusher::m_pushVariable),
                   MakePointerChecker<RandomVariableStream> ())
    .AddAttribute ("ReleaseVariable", "The variable used for automatic releases.",
                   StringValue ("ns3::NormalRandomVariable[Mean=5.0|Variance=2.0]"),
                   MakePointerAccessor (&McpttPusher::m_releaseVariable),
                   MakePointerChecker<RandomVariableStream> ())
    .AddTraceSource ("PushingState", "Trace state changes of 'pushing' state",
                     MakeTraceSourceAccessor (&McpttPusher::m_pushing),
                     "ns3::TracedValueCallback::Boolean")
    ;

  return tid;
}

McpttPusher::McpttPusher (void)
  : m_pushable (0),
    m_pushing (false)
{
  NS_LOG_FUNCTION (this);
}

McpttPusher::~McpttPusher (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttPusher::CancelEvents (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: Pusher canceling events.");
  for (auto it = m_pushReleaseEvents.begin(); it != m_pushReleaseEvents.end(); ++it)
    {
      Simulator::Cancel (*it);
      m_pushReleaseEvents.erase (*it);
    }
}

void
McpttPusher::Push (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: Pusher about to push pushable.");
  PushPushable ();
  NotifyPushed ();
}

void
McpttPusher::NotifyPushed (void)
{
  NS_LOG_FUNCTION (this);
  SetPushing (true);
  if (m_automatic == true)
    {
      ScheduleRelease ();
    }
}

void
McpttPusher::Release (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: Pusher about to release pushable.");
  ReleasePushable ();
  NotifyReleased ();
}

void
McpttPusher::NotifyReleased (void)
{
  NS_LOG_FUNCTION (this);
  SetPushing (false);
  if (m_automatic == true)
    {
      SchedulePush ();
    }
}

void
McpttPusher::SchedulePush (void)
{
  NS_LOG_FUNCTION (this);
  double rv = m_pushVariable->GetValue ();
  if (rv < 0)
    {
      rv = 0;
    }
  Time offInterval = Seconds (rv);
  SchedulePush (offInterval);
}

void
McpttPusher::SchedulePush (const Time& delay)
{
  NS_LOG_FUNCTION (this << delay);
  NS_LOG_LOGIC ("Pusher scheduling to push button at " << delay.GetSeconds () << "s.");
  EventId eventId = Simulator::Schedule (delay, &McpttPusher::Push, this);
  m_pushReleaseEvents.insert (eventId);
}

void
McpttPusher::ScheduleRelease (void)
{
  NS_LOG_FUNCTION (this);
  double rv = m_releaseVariable->GetValue ();
  if (rv < 0)
    {
      rv = 0;
    }
  Time onInterval = Seconds (rv);
  ScheduleRelease (onInterval);
}

void
McpttPusher::ScheduleRelease (const Time& delay)
{
  NS_LOG_FUNCTION (this << delay);
  NS_LOG_LOGIC ("Pusher scheduling to release button at " << delay.GetSeconds() << "s.");
  EventId eventId = Simulator::Schedule (delay, &McpttPusher::Release, this);
  m_pushReleaseEvents.insert (eventId);
}

void
McpttPusher::Start (void)
{
  NS_LOG_FUNCTION (this);
  if (m_automatic)
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: Pusher starting pushes and releases.");
      SchedulePush ();
    }
  else
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: Pusher starting but waiting for manual push event.");
    }
}

void
McpttPusher::Stop (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: Pusher stopping pushes and releases.");
  if (IsPushing ())
    {
      SetPushing (false);
    }
  CancelEvents ();
}

void
McpttPusher::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  SetPushable (0);
  m_pushVariable = 0;
  m_releaseVariable = 0;
  Object::DoDispose ();
}

void
McpttPusher::PushPushable (void)
{
  NS_LOG_FUNCTION (this);
  NS_ABORT_MSG_UNLESS (m_pushable, "There is no pushable.");
  m_pushable->TakePushNotification ();
}

void
McpttPusher::ReleasePushable (void)
{
  NS_LOG_FUNCTION (this);
  NS_ABORT_MSG_UNLESS (m_pushable, "There is no pushable.");
  m_pushable->TakeReleaseNotification ();
}

McpttPushable*
McpttPusher::GetPushable (void) const
{
  NS_LOG_FUNCTION (this);
  return m_pushable;
}

void
McpttPusher::SetPushable (McpttPushable* pushable)
{
  NS_LOG_FUNCTION (this << pushable);
  m_pushable = pushable;
}

bool
McpttPusher::IsPushing (void) const
{
  return m_pushing;
}

void
McpttPusher::SetPushing (bool pushing)
{
  m_pushing = pushing;
}

} // namespace ns3
