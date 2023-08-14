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

#include "mcptt-ptt-app.h"
#include "mcptt-pusher.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttPusher");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttPusher);

TypeId
McpttPusher::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttPusher")
    .SetParent<Object> ()
    .AddConstructor<McpttPusher>()
    .AddAttribute ("Automatic", "Indicates if pushes/releases should occur automatically.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttPusher::m_automatic),
                   MakeBooleanChecker ())
    .AddAttribute ("PttInterarrivalTimeVariable", "The variable used for automatic pushes.",
                   StringValue ("ns3::NormalRandomVariable[Mean=5.0|Variance=2.0]"),
                   MakePointerAccessor (&McpttPusher::m_pttIatVariable),
                   MakePointerChecker<RandomVariableStream> ())
    .AddAttribute ("PttDurationVariable", "The variable used for automatic releases.",
                   StringValue ("ns3::NormalRandomVariable[Mean=5.0|Variance=2.0]"),
                   MakePointerAccessor (&McpttPusher::m_pttDurationVariable),
                   MakePointerChecker<RandomVariableStream> ())
    .AddTraceSource ("PushingState", "Trace state changes of 'pushing' state",
                     MakeTraceSourceAccessor (&McpttPusher::m_pushing),
                     "ns3::TracedValueCallback::Boolean")
    .AddTraceSource ("PttInterarrivalTimeTrace",
                     "The trace for capturing PTT interarrival times.",
                     MakeTraceSourceAccessor (&McpttPusher::m_pttIatTrace),
                     "ns3::Time::TracedCallback")
    .AddTraceSource ("PttDurationTrace",
                     "The trace for capturing PTT durations.",
                     MakeTraceSourceAccessor (&McpttPusher::m_pttDurationTrace),
                     "ns3::Time::TracedCallback")
  ;

  return tid;
}

McpttPusher::McpttPusher (void)
  : m_pttApp (0),
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
  m_pushEvent.Cancel ();
  m_releaseEvent.Cancel ();
}

void
McpttPusher::Push (void)
{
  NS_LOG_FUNCTION (this);
  NS_ABORT_MSG_UNLESS (m_pttApp, "There is no PTT app.");
  NS_LOG_LOGIC ("Pusher about to push PTT button.");
  SetPushing (true);
  m_pttApp->TakePushNotification ();
  if (m_automatic == true)
    {
      ScheduleRelease ();
    }
}

void
McpttPusher::NotifyPushed (void)
{
  NS_LOG_FUNCTION (this);
  if (!IsPushing ())
    {
      SetPushing (true);
      if (m_automatic == true)
        {
          ScheduleRelease ();
        }
    }
}

void
McpttPusher::Release (void)
{
  NS_LOG_FUNCTION (this);
  NS_ABORT_MSG_UNLESS (m_pttApp, "There is no PTT app.");
  NS_LOG_LOGIC ("Pusher about to release PTT button.");
  SetPushing (false);
  m_pttApp->TakeReleaseNotification ();
  if (m_automatic == true)
    {
      SchedulePush ();
    }
}

void
McpttPusher::NotifyReleased (void)
{
  NS_LOG_FUNCTION (this);
  if (IsPushing ())
    {
      SetPushing (false);
      if (m_automatic == true)
        {
          SchedulePush ();
        }
    }
}

bool
McpttPusher::SchedulePush (void)
{
  NS_LOG_FUNCTION (this);
  double rv = m_pttIatVariable->GetValue ();
  if (rv < 0)
    {
      rv = 0;
    }
  Time offInterval = Seconds (rv);
  return SchedulePush (offInterval);
}

bool
McpttPusher::SchedulePush (const Time& delay)
{
  NS_LOG_FUNCTION (this << delay);
  if (!m_pushEvent.IsRunning ())
    {
      NS_LOG_LOGIC ("Pusher scheduling to push button in " << delay.GetSeconds () << "s.");
      m_pushEvent = Simulator::Schedule (delay, &McpttPusher::Push, this);
      m_pttIatTrace (GetPttApp ()->GetUserId (), delay);
      return true;
    }
  else
    {
      NS_LOG_LOGIC ("Suppressing new push event (already running)");
      return false;
    }
}

bool
McpttPusher::CancelPush (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_pushEvent.IsRunning ())
    {
      NS_LOG_LOGIC ("No push event to cancel");
      return false;
    }
  else
    {
      NS_LOG_LOGIC ("Cancelling scheduled push event");
      m_pushEvent.Cancel ();
      return true;
    }
}

bool
McpttPusher::ScheduleRelease (void)
{
  NS_LOG_FUNCTION (this);
  double rv = m_pttDurationVariable->GetValue ();
  if (rv < 0)
    {
      rv = 0;
    }
  Time onInterval = Seconds (rv);
  return ScheduleRelease (onInterval);
}

bool
McpttPusher::ScheduleRelease (const Time& delay)
{
  NS_LOG_FUNCTION (this << delay);
  if (!m_releaseEvent.IsRunning ())
    {
      NS_LOG_LOGIC ("Pusher scheduling to release button in " << delay.GetSeconds () << "s.");
      m_releaseEvent = Simulator::Schedule (delay, &McpttPusher::Release, this);
      m_pttDurationTrace (GetPttApp ()->GetUserId (), delay);
      return true;
    }
  else
    {
      NS_LOG_LOGIC ("Suppressing new release event (already running)");
      return false;
    }
}

bool
McpttPusher::CancelRelease (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_releaseEvent.IsRunning ())
    {
      NS_LOG_LOGIC ("No release event to cancel");
      return false;
    }
  else
    {
      NS_LOG_LOGIC ("Cancelling scheduled release event");
      m_releaseEvent.Cancel ();
      return true;
    }
}

void
McpttPusher::Start (void)
{
  NS_LOG_FUNCTION (this);
  if (m_automatic)
    {
      NS_LOG_LOGIC ("Pusher starting pushes and releases.");
      SchedulePush ();
    }
  else
    {
      NS_LOG_LOGIC ("Pusher starting but waiting for manual push event.");
    }
}

void
McpttPusher::Stop (void)
{
  NS_LOG_FUNCTION (this);
  if (m_releaseEvent.IsRunning () || m_pushEvent.IsRunning ())
    {
      NS_LOG_LOGIC ("Pusher stopping pushes and releases.");
      if (IsPushing ())
        {
          SetPushing (false);
        }
      CancelEvents ();
    }
}

void
McpttPusher::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  SetPttApp (0);
  m_pttIatVariable = 0;
  m_pttDurationVariable = 0;
  Object::DoDispose ();
}

Ptr<McpttPttApp>
McpttPusher::GetPttApp (void) const
{
  NS_LOG_FUNCTION (this);
  return m_pttApp;
}

void
McpttPusher::SetPttApp (Ptr<McpttPttApp> pttApp)
{
  NS_LOG_FUNCTION (this << pttApp);
  m_pttApp = pttApp;
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

} // namespace psc
} // namespace ns3
