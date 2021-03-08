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

#include "mcptt-pusher-orchestrator-contention.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttPusherOrchestratorContention");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttPusherOrchestratorContention);

TypeId
McpttPusherOrchestratorContention::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttPusherOrchestratorContention")
    .SetParent<McpttPusherOrchestratorInterface> ()
    .AddConstructor<McpttPusherOrchestratorContention>()
    .AddAttribute ("ContentionProbability", "Probability that there will be contention.",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&McpttPusherOrchestratorContention::m_cp),
                   MakeDoubleChecker<double> (0, 1))
    .AddAttribute ("Orchestrator", "The underlying orchestrator to contend with.",
                   PointerValue (0),
                   MakePointerAccessor (&McpttPusherOrchestratorContention::GetOrchestrator,
                                        &McpttPusherOrchestratorContention::SetOrchestrator),
                   MakePointerChecker<McpttPusherOrchestratorInterface> ())
  ;

  return tid;
}

McpttPusherOrchestratorContention::McpttPusherOrchestratorContention (void)
  : McpttPusherOrchestratorInterface (),
    m_activePusher (0),
    m_nextEvent (EventId ()),
    m_rv (CreateObject<UniformRandomVariable> ())
{
  NS_LOG_FUNCTION (this);
}

McpttPusherOrchestratorContention::~McpttPusherOrchestratorContention (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttPusherOrchestratorContention::AddPusher (Ptr<McpttPusher> pusher)
{
  NS_LOG_FUNCTION (this << pusher);

  m_orchestrator->AddPusher (pusher);
}

int64_t
McpttPusherOrchestratorContention::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);

  int64_t streams = m_orchestrator->AssignStreams (stream);
  streams++;
  m_rv->SetStream (stream + streams);

  return streams;
}

std::vector<Ptr<McpttPusher> >
McpttPusherOrchestratorContention::GetPushers (void) const
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator->GetPushers ();
}

std::vector<Ptr<McpttPusher> >
McpttPusherOrchestratorContention::GetActivePushers (void) const
{
  NS_LOG_FUNCTION (this);

  std::vector<Ptr<McpttPusher> > activePushers = m_orchestrator->GetActivePushers ();

  if (m_activePusher)
    {
      activePushers.push_back (m_activePusher);
    }

  return activePushers;
}

Time
McpttPusherOrchestratorContention::NextPttIat (void)
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator->NextPttIat ();
}

Time
McpttPusherOrchestratorContention::NextPttDuration (void)
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator->NextPttDuration ();
}

void
McpttPusherOrchestratorContention::Start (void)
{
  NS_LOG_FUNCTION (this);

  NS_ABORT_MSG_IF (m_orchestrator == 0, "No underlying orchestrator.");

  m_orchestrator->Start ();
}

void
McpttPusherOrchestratorContention::Stop (void)
{
  NS_LOG_FUNCTION (this);

  m_nextEvent.Cancel ();

  DeactivatePusher ();

  m_orchestrator->Stop ();
}

void
McpttPusherOrchestratorContention::ActivatePusher (Ptr<McpttPusher> pusher)
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
McpttPusherOrchestratorContention::DeactivatePusher (void)
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
McpttPusherOrchestratorContention::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_nextEvent.Cancel ();
  m_nextPusher = 0;
  m_orchestrator = 0;
  m_rv = 0;

  DeactivatePusher ();
}

void
McpttPusherOrchestratorContention::PttPush (void)
{
  NS_LOG_FUNCTION (this);

  if (m_nextPusher)
    {
      ActivatePusher (m_nextPusher);
      m_nextPusher = 0;
    }

  Time pttDuration = NextPttDuration ();

  m_nextEvent = Simulator::Schedule (pttDuration, &McpttPusherOrchestratorContention::PttRelease, this);

  TracePttDuration (m_activePusher ? m_activePusher->GetPttApp ()->GetUserId () : 0, pttDuration);
}

void
McpttPusherOrchestratorContention::PttRelease (void)
{
  NS_LOG_FUNCTION (this);

  DeactivatePusher ();
}

void
McpttPusherOrchestratorContention::PttDurationTraceCallback (uint32_t userId, Time pttDuration)
{
  NS_LOG_FUNCTION (this);

  if (m_rv->GetValue (0.0, 1.0) < m_cp)
    {
      std::vector<Ptr<McpttPusher> > contenders;
      std::vector<Ptr<McpttPusher> > pushers = GetPushers ();
      std::vector<Ptr<McpttPusher> > activePushers = GetActivePushers ();

      for (std::vector<Ptr<McpttPusher> >::iterator pit = pushers.begin ();
           pit != pushers.end ();
           pit++)
        {
          bool found = false;
          for (std::vector<Ptr<McpttPusher> >::iterator apit = activePushers.begin ();
               apit != activePushers.end ();
               apit++)
            {
              if (*pit == *apit)
                {
                  found = true;
                }
            }

          if (!found)
            {
              contenders.push_back (*pit);
            }
        }

      if (contenders.size () > 0)
        {
          uint32_t rv = m_rv->GetInteger (0, contenders.size () - 1);
          m_nextPusher = contenders[rv];
        }

      double rv = m_rv->GetValue (0.0, pttDuration.GetSeconds ());
      Time pttIat = Seconds (rv);
      m_nextEvent = Simulator::Schedule (pttIat, &McpttPusherOrchestratorContention::PttPush, this);
      TracePttIat (m_nextPusher ? m_nextPusher->GetPttApp ()->GetUserId () : 0, pttIat);
    }
}

Ptr<McpttPusherOrchestratorInterface>
McpttPusherOrchestratorContention::GetOrchestrator (void) const
{
  NS_LOG_FUNCTION (this);

  return m_orchestrator;
}

void
McpttPusherOrchestratorContention::SetOrchestrator (Ptr<McpttPusherOrchestratorInterface> orchestrator)
{
  NS_LOG_FUNCTION (this << orchestrator);

  if (m_orchestrator)
    {
      orchestrator->TraceDisconnectWithoutContext ("PttDurationTrace", MakeCallback (&McpttPusherOrchestratorContention::PttDurationTraceCallback, this));
    }

  if (orchestrator)
    {
      orchestrator->TraceConnectWithoutContext ("PttDurationTrace", MakeCallback (&McpttPusherOrchestratorContention::PttDurationTraceCallback, this));
    }

  m_orchestrator = orchestrator;
}

} // namespace psc
} // namespace ns3
