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
#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/simulator.h>
#include <ns3/type-id.h>

#include "mcptt-counter.h"
#include "mcptt-call-machine.h"
#include "mcptt-call-type-machine.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-on-network-floor-arbitrator.h"
#include "mcptt-on-network-floor-dual-control-state.h"
#include "mcptt-ptt-app.h"
#include "mcptt-timer.h"

#include "mcptt-on-network-floor-dual-control.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttOnNetworkFloorDualControl");

namespace psc {

/** McpttOnNetworkFloorDualControl - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttOnNetworkFloorDualControl);

TypeId
McpttOnNetworkFloorDualControl::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttOnNetworkFloorDualControl")
    .SetParent<Object> ()
    .AddConstructor<McpttOnNetworkFloorDualControl>()
    .AddAttribute ("T11", "The delay to use for timer T11 (Time value)",
                   TimeValue (Seconds (4)),
                   MakeTimeAccessor (&McpttOnNetworkFloorDualControl::SetDelayT11),
                   MakeTimeChecker ())
    .AddAttribute ("T12", "The delay to use for timer T12 (Time value)",
                   TimeValue (Seconds (30)),
                   MakeTimeAccessor (&McpttOnNetworkFloorDualControl::SetDelayT12),
                   MakeTimeChecker ())
    .AddTraceSource ("StateChangeTrace",
                     "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttOnNetworkFloorDualControl::m_stateChangeTrace),
                     "ns3::psc::McpttOnNetworkFloorDualControl::StateChangeTrace")
  ;

  return tid;
}

McpttOnNetworkFloorDualControl::McpttOnNetworkFloorDualControl (void)
  : Object (),
    m_owner (0),
    m_state (McpttOnNetworkFloorDualControlStateStartStop::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ()),
    m_t11 (CreateObject<McpttTimer> (McpttEntityId (11, "T11"))),
    m_t12 (CreateObject<McpttTimer> (McpttEntityId (12, "T12")))
{
  NS_LOG_FUNCTION (this);

  m_t11->Link (&McpttOnNetworkFloorDualControl::ExpiryOfT11, this);
  m_t12->Link (&McpttOnNetworkFloorDualControl::ExpiryOfT12, this);
}

McpttOnNetworkFloorDualControl::~McpttOnNetworkFloorDualControl (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkFloorDualControl::CallRelease1 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorDualControl (" << this << ") taking call release notification (part I).");

  m_state->CallRelease1 (*this);
}

void
McpttOnNetworkFloorDualControl::CallRelease2 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorDualControl (" << this << ") taking call release notification (part II).");

  m_state->CallRelease2 (*this);
}

void
McpttOnNetworkFloorDualControl::ClientRelease (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorDualControl (" << this << ") taking client release notification.");

  m_state->ClientRelease (*this);
}

void
McpttOnNetworkFloorDualControl::ChangeState (Ptr<McpttOnNetworkFloorDualControlState>  state)
{
  NS_LOG_FUNCTION (this << state);

  McpttEntityId stateId = state->GetInstanceStateId ();

  McpttEntityId currStateId = GetStateId ();

  if (currStateId != stateId)
    {
      NS_LOG_LOGIC ("McpttOnNetworkFloorDualControl (" << this << ") moving from state " << *m_state << " to state " << *state << ".");

      m_state->Unselected (*this);
      SetState (state);
      state->Selected (*this);

      if (!m_stateChangeCb.IsNull ())
        {
          m_stateChangeCb (currStateId, stateId);
        }

      std::string selected = "N/A"; // Selected call indicator not applicable
      m_stateChangeTrace (GetOwner ()->GetTxSsrc (), GetOwner ()->GetOwner ()->GetCallId (), selected, GetInstanceTypeId ().GetName (), currStateId.GetName (), stateId.GetName ());
    }
}

TypeId
McpttOnNetworkFloorDualControl::GetInstanceTypeId (void) const
{
  return McpttOnNetworkFloorDualControl::GetTypeId ();
}

McpttEntityId
McpttOnNetworkFloorDualControl::GetStateId (void) const
{
  McpttEntityId stateId = m_state->GetInstanceStateId ();

  return stateId;
}

bool
McpttOnNetworkFloorDualControl::IsStarted (void) const
{
  return m_state->GetInstanceStateId () != McpttOnNetworkFloorDualControlStateStartStop::GetStateId ();
}

void
McpttOnNetworkFloorDualControl::ReceiveFloorRelease (const McpttFloorMsgRelease& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC ("McpttOnNetworkFloorDualControl(" << this << ") received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorRelease (*this, msg);
}

void
McpttOnNetworkFloorDualControl::ReceiveFloorRequest (const McpttFloorMsgRequest& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC ("McpttOnNetworkFloorDualControl(" << this << ") received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorRequest (*this, msg);
}

void
McpttOnNetworkFloorDualControl::ReceiveMedia (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC ("McpttOnNetworkFloorDualControl(" << this << ") received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveMedia (*this, msg);
}

void
McpttOnNetworkFloorDualControl::ReceivePreemptiveFloorRequest (const McpttFloorMsgRequest& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC ("McpttOnNetworkFloorDualControl(" << this << ") received preemptive " << msg.GetInstanceTypeId () << ".");

  m_state->ReceivePreemptiveFloorRequest (*this, msg);
}

void
McpttOnNetworkFloorDualControl::SetDelayT11 (const Time& delayT11)
{
  NS_LOG_FUNCTION (this << delayT11);

  GetT11 ()->SetDelay (delayT11);
}

void
McpttOnNetworkFloorDualControl::SetDelayT12 (const Time& delayT12)
{
  NS_LOG_FUNCTION (this << delayT12);

  GetT12 ()->SetDelay (delayT12);
}

void
McpttOnNetworkFloorDualControl::Stop (void)
{
  NS_LOG_FUNCTION (this);

  if (GetT11 ()->IsRunning ())
    {
      GetT11 ()->Stop ();
    }

  if (GetT12 ()->IsRunning ())
    {
      GetT12 ()->Stop ();
    }
}

void
McpttOnNetworkFloorDualControl::Terminate (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorDualControl (" << this << ") taking terminate notification.");

  m_state->Terminate (*this);
}

void
McpttOnNetworkFloorDualControl::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_owner = 0;
  m_state = 0;
  m_t11 = 0;
  m_t12 = 0;
  m_stateChangeCb = MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ();
}

void
McpttOnNetworkFloorDualControl::ExpiryOfT11 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorDualControl T11 expired.");

  m_state->ExpiryOfT11 (*this);
}

void
McpttOnNetworkFloorDualControl::ExpiryOfT12 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorDualControl T12 expired.");

  m_state->ExpiryOfT12 (*this);
}

Ptr<McpttOnNetworkFloorArbitrator>
McpttOnNetworkFloorDualControl::GetOwner (void) const
{
  NS_LOG_FUNCTION (this);

  return m_owner;
}

uint32_t
McpttOnNetworkFloorDualControl::GetStoredSsrc (void) const
{
  NS_LOG_FUNCTION (this);

  return m_storedSsrc;
}

uint8_t
McpttOnNetworkFloorDualControl::GetStoredPriority (void) const
{
  NS_LOG_FUNCTION (this);

  return m_storedPriority;
}

McpttFloorMsgFieldTrackInfo
McpttOnNetworkFloorDualControl::GetTrackInfo (void) const
{
  NS_LOG_FUNCTION (this);

  return m_trackInfo;
}

Ptr<McpttTimer>
McpttOnNetworkFloorDualControl::GetT11 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_t11;
}

Ptr<McpttTimer>
McpttOnNetworkFloorDualControl::GetT12 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_t12;
}

void
McpttOnNetworkFloorDualControl::SetOwner (Ptr<McpttOnNetworkFloorArbitrator> owner)
{
  NS_LOG_FUNCTION (this);

  m_owner = owner;
}

void
McpttOnNetworkFloorDualControl::SetState (Ptr<McpttOnNetworkFloorDualControlState>  state)
{
  NS_LOG_FUNCTION (this << state);

  m_state = state;
}

void
McpttOnNetworkFloorDualControl::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this);

  m_stateChangeCb = stateChangeCb;
}

void
McpttOnNetworkFloorDualControl::SetStoredSsrc (const uint32_t storedSsrc)
{
  NS_LOG_FUNCTION (this);

  m_storedSsrc = storedSsrc;
}

void
McpttOnNetworkFloorDualControl::SetStoredPriority (uint8_t storedPriority)
{
  NS_LOG_FUNCTION (this);

  m_storedPriority = storedPriority;
}

void
McpttOnNetworkFloorDualControl::SetTrackInfo (const McpttFloorMsgFieldTrackInfo& trackInfo)
{
  NS_LOG_FUNCTION (this);

  m_trackInfo = trackInfo;
}

/** McpttOnNetworkFloorDualControl - end **/

} // namespace psc
} // namespace ns3

