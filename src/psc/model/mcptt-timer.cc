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
#include <ns3/object.h>
#include <ns3/timer.h>
#include <ns3/pointer.h>

#include "mcptt-entity-id.h"

#include "mcptt-timer.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttTimer");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttTimer);

TypeId
McpttTimer::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttTimer")
    .SetParent<Object> ()
    .AddConstructor<McpttTimer> ()
  ;

  return tid;
}

McpttTimer::McpttTimer (void)
  : Object (),
    m_id (McpttEntityId (0, "Timer")),
    m_rawTimer (0)
{
  NS_LOG_FUNCTION (this);
}

McpttTimer::McpttTimer (const McpttEntityId& id)
  : Object (),
    m_id (id),
    m_rawTimer (0)
{
  NS_LOG_FUNCTION (this << id);
}

McpttTimer::~McpttTimer (void)
{
  NS_LOG_FUNCTION (this);
}

Time
McpttTimer::GetDelay (void) const
{
  Timer* rawTimer = GetRawTimer ();

  Time delay = rawTimer->GetDelay ();

  return delay;
}

TypeId
McpttTimer::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttTimer::GetTypeId ();
}

Time
McpttTimer::GetTimeLeft (void) const
{
  NS_LOG_FUNCTION (this);

  Timer* rawTimer = GetRawTimer ();

  Time timeLeft = rawTimer->GetDelayLeft ();

  NS_LOG_LOGIC ("Timer " << GetId () << " has " << timeLeft << " left.");

  return timeLeft;
}

bool
McpttTimer::IsExpired (void) const
{
  NS_LOG_FUNCTION (this);

  Timer* rawTimer = GetRawTimer ();

  bool isExpired = rawTimer->IsExpired ();

  NS_LOG_LOGIC ("Timer " << GetId () << " is" << (isExpired ? " " : " not ") << "expired.");

  return isExpired;
}

bool
McpttTimer::IsRunning (void) const
{
  NS_LOG_FUNCTION (this);

  Timer* rawTimer = GetRawTimer ();

  bool isRunning = rawTimer->IsRunning ();

  NS_LOG_LOGIC ("Timer " << GetId () << " is" << (isRunning ? " " : " not ") << "running.");

  if (isRunning)
    {
      GetTimeLeft ();
    }

  return isRunning;
}

void
McpttTimer::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this << &os);

  Time delay = GetDelay ();
  McpttEntityId id = GetId ();
  bool isRunning = IsRunning ();
  bool isExpired = IsExpired ();

  os << id << "(";
  os << "delay=" << delay.GetSeconds () << "s" << ";";
  os << "running=" << (isRunning ? "true" : "false") << ";";
  os << "expired=" << (isExpired ? "true" : "false") << ";";
  os << "time-left=";

  if (isRunning)
    {
      os << GetTimeLeft ().GetSeconds () << "s";
    }
  else
    {
      os << "N/A";
    }

  os << ")";
}

void
McpttTimer::Restart (void)
{
  NS_LOG_FUNCTION (this);

  if (IsRunning ())
    {
      Stop ();
    }

  Start ();
}

void
McpttTimer::SetDelay (const Time& delay)
{
  NS_LOG_FUNCTION (this << delay);

  Timer* rawTimer = GetRawTimer ();

  NS_ASSERT_MSG (!IsRunning (), "Timer " << GetId () << " has already been started.");

  NS_LOG_LOGIC ("Setting timer " << GetId () << " delay to " << delay << ".");

  rawTimer->SetDelay (delay);
}

void
McpttTimer::Start (void)
{
  NS_LOG_FUNCTION (this);

  Timer* rawTimer = GetRawTimer ();

  NS_ASSERT_MSG (!IsRunning (), "Timer " << GetId () << " has already been started.");

  NS_LOG_LOGIC ("Starting timer " << GetId () << ".");

  rawTimer->Schedule ();
}

void
McpttTimer::StartWith (const Time& delay)
{
  NS_LOG_FUNCTION (this);

  Time permDelay = GetDelay ();
  SetDelay (delay);
  Start ();
  GetRawTimer ()->SetDelay (permDelay);
}

void
McpttTimer::Stop (void)
{
  NS_LOG_FUNCTION (this);

  Timer* rawTimer = GetRawTimer ();

  NS_ASSERT_MSG (IsRunning (), "Timer " << GetId () << " is already stopped.");

  NS_LOG_LOGIC ("Stopping timer " << GetId () << ".");

  rawTimer->Cancel ();
}

void
McpttTimer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  Timer* rawTimer = GetRawTimer ();

  delete rawTimer;

  SetRawTimer (0);
}

McpttEntityId
McpttTimer::GetId (void) const
{
  NS_LOG_FUNCTION (this);

  return m_id;
}

Timer*
McpttTimer::GetRawTimer (void) const
{
  NS_LOG_FUNCTION (this);

  return m_rawTimer;
}

void
McpttTimer::SetId (const McpttEntityId& id)
{
  NS_LOG_FUNCTION (this << id);

  m_id = id;
}

void
McpttTimer::SetRawTimer (Timer* const& rawTimer)
{
  NS_LOG_FUNCTION (this << rawTimer);

  m_rawTimer = rawTimer;
}

std::ostream&
operator<< (std::ostream& os, const McpttTimer& timer)
{
  timer.Print (os);

  return os;
}

} // namespace psc
} // namespace ns3
