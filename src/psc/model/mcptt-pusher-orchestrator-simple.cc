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
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/simulator.h>
#include <ns3/string.h>
#include <ns3/type-id.h>

#include "mcptt-pusher.h"

#include "mcptt-pusher-orchestrator.h"
#include "mcptt-pusher-orchestrator-simple.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttPusherOrchestratorSimple");

NS_OBJECT_ENSURE_REGISTERED (McpttPusherOrchestratorSimple);

TypeId
McpttPusherOrchestratorSimple::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttPusherOrchestratorSimple")
    .SetParent<McpttPusherOrchestrator> ()
    .AddConstructor<McpttPusherOrchestratorSimple>()
    .AddAttribute ("PttDurationVariable", "The variable used for PTT durations.",
                   StringValue ("ns3::NormalRandomVariable[Mean=5.0|Variance=2.0]"),
                   MakePointerAccessor (&McpttPusherOrchestratorSimple::m_pttDurationVariable),
                   MakePointerChecker<RandomVariableStream> ())
     .AddAttribute ("PttInterarrivalTimeVariable", "The variable used for PTT occurrences.",
                   StringValue ("ns3::ExponentialRandomVariable[Mean=5.0]"),
                   MakePointerAccessor (&McpttPusherOrchestratorSimple::m_pttIatVariable),
                   MakePointerChecker<RandomVariableStream> ())
      ;

  return tid;
}

McpttPusherOrchestratorSimple::McpttPusherOrchestratorSimple (void)
  : McpttPusherOrchestrator (),
    m_active (false),
    m_activePusher (0),
    m_nextEvent (EventId ()),
    m_pushers (std::vector<Ptr<McpttPusher> > ()),
    m_selectionVariable (CreateObject<UniformRandomVariable> ())
{
  NS_LOG_FUNCTION (this);
}

McpttPusherOrchestratorSimple::~McpttPusherOrchestratorSimple (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttPusherOrchestratorSimple::AddPusher (Ptr<McpttPusher> pusher)
{
  NS_LOG_FUNCTION (this << pusher);

  pusher->SetAttribute ("Automatic", BooleanValue (false));
  m_pushers.push_back (pusher);
}

int64_t 
McpttPusherOrchestratorSimple::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);

  int64_t streams = 0;
  streams++;
  m_pttDurationVariable->SetStream (stream + streams);
  streams++;
  m_pttIatVariable->SetStream (stream + streams);
  streams++;
  m_selectionVariable->SetStream (stream + streams);

  return streams;
}

std::vector<Ptr<McpttPusher> >
McpttPusherOrchestratorSimple::GetPushers (void) const
{
  NS_LOG_FUNCTION (this);

  return m_pushers;
}

std::vector<Ptr<McpttPusher> >
McpttPusherOrchestratorSimple::GetActivePushers (void) const
{
  NS_LOG_FUNCTION (this);

  return std::vector<Ptr<McpttPusher> > { m_activePusher };
}

Time
McpttPusherOrchestratorSimple::NextPttIat (void)
{
  NS_LOG_FUNCTION (this);

  return Seconds (m_pttIatVariable->GetValue ());
}

Time
McpttPusherOrchestratorSimple::NextPttDuration (void)
{
  NS_LOG_FUNCTION (this);

  return Seconds (m_pttDurationVariable->GetValue ());
}

bool
McpttPusherOrchestratorSimple::IsActive (void) const
{
  NS_LOG_FUNCTION (this);

  return m_active;
}

void
McpttPusherOrchestratorSimple::Start (void)
{
  NS_LOG_FUNCTION (this);

  if (!IsActive ())
    {
      m_active = true;
      PttRelease ();
    }
}

void
McpttPusherOrchestratorSimple::Stop (void)
{
  NS_LOG_FUNCTION (this);

  m_active = false;

  m_nextEvent.Cancel ();

  DeactivatePusher ();
}

void
McpttPusherOrchestratorSimple::ActivatePusher (Ptr<McpttPusher> pusher)
{
  NS_LOG_FUNCTION (this << pusher);

  if (m_activePusher)
    {
      DeactivatePusher ();
    }

  m_activePusher = pusher;
  m_activePusher->Push ();
}

void
McpttPusherOrchestratorSimple::DeactivatePusher (void)
{
  if (m_activePusher)
    {
      if (m_activePusher->IsPushing ())
        {
          m_activePusher->Release ();
        }
      m_activePusher = 0;
    }
}

void
McpttPusherOrchestratorSimple::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  std::vector<Ptr<McpttPusher> >::iterator it;
  for (it = m_pushers.begin (); it != m_pushers.end (); it++)
    {
      *it = 0;
    }

  m_nextEvent.Cancel ();

  m_pushers.clear ();

  DeactivatePusher ();

  m_pttIatVariable = 0;
  m_pttDurationVariable = 0;
  m_selectionVariable = 0;
}

void
McpttPusherOrchestratorSimple::PttPush (void)
{
  NS_LOG_FUNCTION (this);

  if (m_pushers.size () > 0)
    {
      uint32_t rv = m_selectionVariable->GetInteger (0, m_pushers.size () - 1);
      Ptr<McpttPusher> pusher = m_pushers[rv];
      ActivatePusher (pusher);
    }

  Time pttDuration = NextPttDuration ();
  m_nextEvent = Simulator::Schedule (pttDuration, &McpttPusherOrchestratorSimple::PttRelease, this);

  TracePttDuration (pttDuration);
}

void
McpttPusherOrchestratorSimple::PttRelease (void)
{
  DeactivatePusher ();
  
  Time pttIat = NextPttIat ();
  m_nextEvent = Simulator::Schedule (pttIat, &McpttPusherOrchestratorSimple::PttPush, this);

  TracePttIat (pttIat);
}

} // namespace ns3
