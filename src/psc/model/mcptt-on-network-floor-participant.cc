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
#include "mcptt-sdp-fmtp-header.h"
#include "mcptt-server-call.h"
#include "mcptt-on-network-floor-participant-state.h"
#include "mcptt-ptt-app.h"
#include "mcptt-timer.h"

#include "mcptt-on-network-floor-participant.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttOnNetworkFloorParticipant");

namespace psc {

/** McpttOnNetworkFloorParticipant - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttOnNetworkFloorParticipant);

TypeId
McpttOnNetworkFloorParticipant::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttOnNetworkFloorParticipant")
    .SetParent<McpttFloorParticipant> ()
    .AddConstructor<McpttOnNetworkFloorParticipant>()
    .AddAttribute ("AckRequired", "The flag that indicates if acknowledgement is required.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttOnNetworkFloorParticipant::m_ackRequired),
                   MakeBooleanChecker ())
    .AddAttribute ("C100", "The initial limit of counter C100.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttOnNetworkFloorParticipant::SetLimitC100),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("C101", "The initial limit of counter C101.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttOnNetworkFloorParticipant::SetLimitC101),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("C104", "The initial limit of counter C104.",
                   UintegerValue (3),
                   MakeUintegerAccessor (&McpttOnNetworkFloorParticipant::SetLimitC104),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("GenMedia", "The flag that indicates if the floor machine should generate media when it has permission.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttOnNetworkFloorParticipant::m_genMedia),
                   MakeBooleanChecker ())
    .AddAttribute ("ImplicitRequest", "The flag that indicates if the SIP INVITE includes an implicit Floor Request.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttOnNetworkFloorParticipant::m_implicitRequest),
                   MakeBooleanChecker ())
    .AddAttribute ("Priority", "The priority of the floor participant.",
                   UintegerValue (1),
                   MakeUintegerAccessor (&McpttOnNetworkFloorParticipant::GetPriority,
                                         &McpttOnNetworkFloorParticipant::SetPriority),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("T100", "Timeout value to wait for response to Floor Release",
                   TimeValue (Seconds (1)),
                   MakeTimeAccessor (&McpttOnNetworkFloorParticipant::SetDelayT100),
                   MakeTimeChecker ())
    .AddAttribute ("T101", "Timeout value to wait for response to Floor Request",
                   TimeValue (Seconds (1)),
                   MakeTimeAccessor (&McpttOnNetworkFloorParticipant::SetDelayT101),
                   MakeTimeChecker ())
    .AddAttribute ("T103", "Timeout value to wait for Floor Idle",
                   TimeValue (Seconds (4)),
                   MakeTimeAccessor (&McpttOnNetworkFloorParticipant::SetDelayT103),
                   MakeTimeChecker ())
    .AddAttribute ("T104", "Timeout value to wait for response to Floor Queue Position Request",
                   TimeValue (MilliSeconds (1)),
                   MakeTimeAccessor (&McpttOnNetworkFloorParticipant::SetDelayT104),
                   MakeTimeChecker ())
    .AddAttribute ("T132", "Timeout to wait for user action to a Floor Granted message",
                   TimeValue (Seconds (2)),
                   MakeTimeAccessor (&McpttOnNetworkFloorParticipant::SetDelayT132),
                   MakeTimeChecker ())
    .AddTraceSource ("StateChangeTrace",
                     "The trace for capturing state changes.",
                     MakeTraceSourceAccessor (&McpttOnNetworkFloorParticipant::m_stateChangeTrace),
                     "ns3::psc::McpttFloorParticipant::StateChangeTracedCallback")
  ;

  return tid;
}

McpttOnNetworkFloorParticipant::McpttOnNetworkFloorParticipant (void)
  : McpttFloorParticipant (),
    m_c100 (CreateObject<McpttCounter> (McpttEntityId (0, "C100"))),
    m_c101 (CreateObject<McpttCounter> (McpttEntityId (1, "C101"))),
    m_c104 (CreateObject<McpttCounter> (McpttEntityId (1, "C104"))),
    m_dualFloor (false),
    m_floorGrantedCb (MakeNullCallback<void> ()),
    m_originator (false),
    m_overridden (false),
    m_overriding (false),
    m_call (0),
    m_priority (1),
    m_state (McpttOnNetworkFloorParticipantStateStartStop::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const McpttEntityId&, const McpttEntityId&> ()),
    m_storedMsgs (Create<Packet> ()),
    m_t100 (CreateObject<McpttTimer> (McpttEntityId (0, "T100"))),
    m_t101 (CreateObject<McpttTimer> (McpttEntityId (1, "T101"))),
    m_t103 (CreateObject<McpttTimer> (McpttEntityId (3, "T103"))),
    m_t104 (CreateObject<McpttTimer> (McpttEntityId (3, "T104"))),
    m_t132 (CreateObject<McpttTimer> (McpttEntityId (3, "T132")))
{
  NS_LOG_FUNCTION (this);

  m_t100->Link (&McpttOnNetworkFloorParticipant::ExpiryOfT100, this);
  m_t101->Link (&McpttOnNetworkFloorParticipant::ExpiryOfT101, this);
  m_t103->Link (&McpttOnNetworkFloorParticipant::ExpiryOfT103, this);
  m_t104->Link (&McpttOnNetworkFloorParticipant::ExpiryOfT104, this);
  m_t132->Link (&McpttOnNetworkFloorParticipant::ExpiryOfT132, this);
}

McpttOnNetworkFloorParticipant::~McpttOnNetworkFloorParticipant (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttOnNetworkFloorParticipant::AcceptGrant (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << " accepting grant" << ".");

  m_state->AcceptGrant (*this);
}

void
McpttOnNetworkFloorParticipant::CallInitiated (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << "'s call initiated.");

  m_state->CallInitiated (*this);
}

void
McpttOnNetworkFloorParticipant::CallEstablished (const McpttSdpFmtpHeader& sdpHeader)
{
  NS_LOG_FUNCTION (this << sdpHeader);

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << "'s call established.");

  SetPriority (sdpHeader.GetMcPriority ());
  m_granted = sdpHeader.GetMcGranted ();
  m_implicitRequest = sdpHeader.GetMcImplicitRequest ();
  m_state->CallEstablished (*this);
}

void
McpttOnNetworkFloorParticipant::CallRelease1 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << "'s call release (part I).");
  m_state->CallRelease1 (*this);
}

void
McpttOnNetworkFloorParticipant::CallRelease2 (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << "'s call release (part II).");
  m_state->CallRelease2 (*this);
}

void
McpttOnNetworkFloorParticipant::ChangeState (Ptr<McpttOnNetworkFloorParticipantState>  state)
{
  NS_LOG_FUNCTION (this << state->GetInstanceStateId ());

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();
  NS_LOG_LOGIC ("UserId " << userId << " moving from state " << m_state->GetName () << " to state " << state->GetName () << ".");
  std::string selected = "False";
  if (GetCall ()->GetCallId () == GetCall ()->GetOwner ()->GetSelectedCall ()->GetCallId ())
    {
      selected = "True";
    }
  if (GetStateId () != state->GetInstanceStateId ())
    {
      McpttEntityId previousStateId = GetStateId ();
      m_state->Unselected (*this);
      SetState (state);
      state->Selected (*this);
      m_stateChangeTrace (GetCall ()->GetOwner ()->GetUserId (), GetCall ()->GetCallId (), selected, GetInstanceTypeId ().GetName (), previousStateId.GetName (), state->GetName ());
    }
}

void
McpttOnNetworkFloorParticipant::FloorGranted (void)
{
  NS_LOG_FUNCTION (this);

  if (!m_floorGrantedCb.IsNull ())
    {
      m_floorGrantedCb ();
    }
}

uint8_t
McpttOnNetworkFloorParticipant::GetCallTypeId (void) const
{
  Ptr<McpttCallMachine> callMachine = GetCall ()->GetCallMachine ();
  McpttCallMsgFieldCallType callType = callMachine->GetCallType ();
  uint8_t callTypeId = callType.GetType ();

  return callTypeId;
}


McpttFloorMsgFieldIndic
McpttOnNetworkFloorParticipant::GetIndicator (void) const
{
  McpttFloorMsgFieldIndic indicator;
  uint8_t callTypeId = GetCallTypeId ();

  if (IsDualFloor ())
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::DUAL_FLOOR);
    }

  if (callTypeId == McpttCallMsgFieldCallType::BASIC_GROUP)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::NORMAL_CALL);
    }
  else if (callTypeId == McpttCallMsgFieldCallType::BROADCAST_GROUP)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::BROADCAST_CALL);
    }
  else if (callTypeId == McpttCallMsgFieldCallType::EMERGENCY_GROUP)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::EMERGENCY_CALL);
    }
  else if (callTypeId == McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::IMMINENT_CALL);
    }
  else if (callTypeId == McpttCallMsgFieldCallType::PRIVATE)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::NORMAL_CALL);
    }
  else if (callTypeId == McpttCallMsgFieldCallType::EMERGENCY_PRIVATE)
    {
      indicator.Indicate (McpttFloorMsgFieldIndic::EMERGENCY_CALL);
    }

  return indicator;
}

TypeId
McpttOnNetworkFloorParticipant::GetInstanceTypeId (void) const
{
  return McpttOnNetworkFloorParticipant::GetTypeId ();
}

McpttEntityId
McpttOnNetworkFloorParticipant::GetStateId (void) const
{
  McpttEntityId stateId = m_state->GetInstanceStateId ();

  return stateId;
}

Ptr<McpttOnNetworkFloorParticipantState>
McpttOnNetworkFloorParticipant::GetState (void) const
{
  return m_state;
}

uint32_t
McpttOnNetworkFloorParticipant::GetTxSsrc () const
{
  uint32_t txSsrc = GetCall ()->GetOwner ()->GetUserId ();

  return txSsrc;
}

bool
McpttOnNetworkFloorParticipant::HasFloor (void) const
{
  bool hasFloor = m_state->HasFloor (*this);

  return hasFloor;
}

bool
McpttOnNetworkFloorParticipant::IsAckRequired (void) const
{
  return m_ackRequired;
}

bool
McpttOnNetworkFloorParticipant::IsDualFloor (void) const
{
  return m_dualFloor;
}

bool
McpttOnNetworkFloorParticipant::IsImplicitRequest (void) const
{
  return m_implicitRequest;
}

bool
McpttOnNetworkFloorParticipant::IsGranted (void) const
{
  return m_granted;
}

bool
McpttOnNetworkFloorParticipant::IsOriginator (void) const
{
  return m_originator;
}

bool
McpttOnNetworkFloorParticipant::IsOverridden (void) const
{
  return m_overridden;
}

bool
McpttOnNetworkFloorParticipant::IsOverriding (void) const
{
  return m_overriding;
}

bool
McpttOnNetworkFloorParticipant::IsStarted (void) const
{
  return McpttOnNetworkFloorParticipant::GetStateId () != McpttOnNetworkFloorParticipantStateStartStop::GetStateId ();
}

void
McpttOnNetworkFloorParticipant::MediaReady (McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this);

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << myUserId << "'s client is about to send media.");

  msg.SetSsrc (GetTxSsrc ());

  m_state->MediaReady (*this, msg);
}

void
McpttOnNetworkFloorParticipant::Receive (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  msg.Visit (*this);
}

void
McpttOnNetworkFloorParticipant::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  msg.Visit (*this);
}

void
McpttOnNetworkFloorParticipant::ReceiveFloorAck (const McpttFloorMsgAck& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorAck (*this, msg);
}

void
McpttOnNetworkFloorParticipant::ReceiveFloorDeny (const McpttFloorMsgDeny& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorDeny (*this, msg);
}

void
McpttOnNetworkFloorParticipant::ReceiveFloorGranted (const McpttFloorMsgGranted& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorGranted (*this, msg);
}

void
McpttOnNetworkFloorParticipant::ReceiveFloorIdle (const McpttFloorMsgIdle& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorIdle (*this, msg);
}

void
McpttOnNetworkFloorParticipant::ReceiveFloorQueuePositionInfo (const McpttFloorMsgQueuePositionInfo& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorQueuePositionInfo (*this, msg);
}

void
McpttOnNetworkFloorParticipant::ReceiveFloorRevoke (const McpttFloorMsgRevoke& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorRevoke (*this, msg);
}

void
McpttOnNetworkFloorParticipant::ReceiveFloorTaken (const McpttFloorMsgTaken& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveFloorTaken (*this, msg);
}

void
McpttOnNetworkFloorParticipant::ReceiveMedia (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << userId << " received " << msg.GetInstanceTypeId () << ".");

  m_state->ReceiveMedia (*this, msg);
}

void
McpttOnNetworkFloorParticipant::ReleaseRequest (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << " releasing request" << ".");

  m_state->ReleaseRequest (*this);
}

void
McpttOnNetworkFloorParticipant::Send (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << " sending " << msg << ".");

  GetCall ()->Send (msg);
}

void
McpttOnNetworkFloorParticipant::SendFloorQueuePositionRequest (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << GetCall ()->GetOwner ()->GetUserId () << " requesting queue position" << ".");

  m_state->SendFloorQueuePositionRequest (*this);
}

void
McpttOnNetworkFloorParticipant::SetDelayT100 (const Time& delayT100)
{
  NS_LOG_FUNCTION (this << delayT100);

  GetT100 ()->SetDelay (delayT100);
}

void
McpttOnNetworkFloorParticipant::SetDelayT101 (const Time& delayT101)
{
  NS_LOG_FUNCTION (this << delayT101);

  GetT101 ()->SetDelay (delayT101);
}

void
McpttOnNetworkFloorParticipant::SetDelayT103 (const Time& delayT103)
{
  NS_LOG_FUNCTION (this << delayT103);

  GetT103 ()->SetDelay (delayT103);
}

void
McpttOnNetworkFloorParticipant::SetDelayT104 (const Time& delayT104)
{
  NS_LOG_FUNCTION (this << delayT104);

  GetT104 ()->SetDelay (delayT104);
}

void
McpttOnNetworkFloorParticipant::SetDelayT132 (const Time& delayT132)
{
  NS_LOG_FUNCTION (this << delayT132);

  GetT132 ()->SetDelay (delayT132);
}

void
McpttOnNetworkFloorParticipant::SetLimitC100 (uint32_t limitC100)
{
  NS_LOG_FUNCTION (this << limitC100);

  GetC100 ()->SetLimit (limitC100);
}

void
McpttOnNetworkFloorParticipant::SetLimitC101 (uint32_t limitC101)
{
  NS_LOG_FUNCTION (this << limitC101);

  GetC101 ()->SetLimit (limitC101);
}

void
McpttOnNetworkFloorParticipant::SetLimitC104 (uint32_t limitC104)
{
  NS_LOG_FUNCTION (this << limitC104);

  GetC104 ()->SetLimit (limitC104);
}

bool
McpttOnNetworkFloorParticipant::ShouldGenMedia (void) const
{
  return m_genMedia;
}

void
McpttOnNetworkFloorParticipant::PttPush (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttPttApp> pttApp = GetCall ()->GetOwner ();
  uint8_t callTypeId = GetCallTypeId ();
  uint32_t userId = pttApp->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << userId << " taking push notification.");

  if (callTypeId == McpttCallMsgFieldCallType::BROADCAST_GROUP)
    {
      //Provide local floor deny because PTT request are not allowed
      //from a terminaing user when part of a 'BROADCAST GROUP CALL'.
      //The originating user (or the user that started the call)
      //should have been given an implicit grant and thus should not
      //being making PTT request.
      NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << userId << " denied locally since terminating users can't make PTT request when part of a 'BROADCAST GROUP CALL'.");

      if (pttApp->IsPushed ())
        {
          pttApp->NotifyReleased ();
        }
    }
  else
    {
      m_state->PttPush (*this);
    }
}

void
McpttOnNetworkFloorParticipant::PttRelease (void)
{
  NS_LOG_FUNCTION (this);

  uint32_t userId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << userId << " taking release notification.");

  m_state->PttRelease (*this);
}

void
McpttOnNetworkFloorParticipant::Start (void)
{
  NS_LOG_FUNCTION (this);

  CallInitiated ();
}

void
McpttOnNetworkFloorParticipant::Stop (void)
{
  NS_LOG_FUNCTION (this);

  if (GetT100 ()->IsRunning ())
    {
      GetT100 ()->Stop ();
    }

  if (GetT101 ()->IsRunning ())
    {
      GetT101 ()->Stop ();
    }

  if (GetT103 ()->IsRunning ())
    {
      GetT103 ()->Stop ();
    }

  if (GetT104 ()->IsRunning ())
    {
      GetT104 ()->Stop ();
    }

  if (GetT132 ()->IsRunning ())
    {
      GetT132 ()->Stop ();
    }
}

void
McpttOnNetworkFloorParticipant::ReportEvent (const char* reason) const
{
  GetCall ()->GetOwner ()->ReportEvent (GetCall ()->GetCallId (), reason);
}

void
McpttOnNetworkFloorParticipant::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_c100 = 0;
  m_c101 = 0;
  m_c104 = 0;
  m_call = 0;
  m_state = 0;
  m_t100 = 0;
  m_t101 = 0;
  m_t103 = 0;
  m_t104 = 0;
  m_t132 = 0;
}

void
McpttOnNetworkFloorParticipant::ExpiryOfT100 (void)
{
  NS_LOG_FUNCTION (this);

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << myUserId << " T100 expired " << GetC100 ()->GetValue () << " times.");

  m_state->ExpiryOfT100 (*this);
}

void
McpttOnNetworkFloorParticipant::ExpiryOfT101 (void)
{
  NS_LOG_FUNCTION (this);

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << myUserId << " T101 expired.");

  m_state->ExpiryOfT101 (*this);
}

void
McpttOnNetworkFloorParticipant::ExpiryOfT103 (void)
{
  NS_LOG_FUNCTION (this);

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << myUserId << " T103 has expired.");

  m_state->ExpiryOfT103 (*this);
}

void
McpttOnNetworkFloorParticipant::ExpiryOfT104 (void)
{
  NS_LOG_FUNCTION (this);

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << myUserId << " T104 has expired.");

  m_state->ExpiryOfT104 (*this);
}

void
McpttOnNetworkFloorParticipant::ExpiryOfT132 (void)
{
  NS_LOG_FUNCTION (this);

  uint32_t myUserId = GetCall ()->GetOwner ()->GetUserId ();

  NS_LOG_LOGIC ("McpttOnNetworkFloorParticipant " << myUserId << " T132 has expired.");

  m_state->ExpiryOfT132 (*this);
}

Ptr<McpttCounter>
McpttOnNetworkFloorParticipant::GetC100 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_c100;
}

Ptr<McpttCounter>
McpttOnNetworkFloorParticipant::GetC101 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_c101;
}

Ptr<McpttCounter>
McpttOnNetworkFloorParticipant::GetC104 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_c104;
}

Ptr<McpttCall>
McpttOnNetworkFloorParticipant::GetCall (void) const
{
  return m_call;
}

uint8_t
McpttOnNetworkFloorParticipant::GetPriority (void) const
{
  NS_LOG_FUNCTION (this);

  return m_priority;
}

Ptr<Packet>
McpttOnNetworkFloorParticipant::GetStoredMsgs (void) const
{
  NS_LOG_FUNCTION (this);

  return m_storedMsgs;
}

Ptr<McpttTimer>
McpttOnNetworkFloorParticipant::GetT100 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_t100;
}

Ptr<McpttTimer>
McpttOnNetworkFloorParticipant::GetT101 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_t101;
}

Ptr<McpttTimer>
McpttOnNetworkFloorParticipant::GetT103 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_t103;
}

Ptr<McpttTimer>
McpttOnNetworkFloorParticipant::GetT104 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_t104;
}

Ptr<McpttTimer>
McpttOnNetworkFloorParticipant::GetT132 (void) const
{
  NS_LOG_FUNCTION (this);

  return m_t132;
}

void
McpttOnNetworkFloorParticipant::SetDualFloor (const bool& dualFloor)
{
  NS_LOG_FUNCTION (this);

  m_dualFloor = dualFloor;
}

void
McpttOnNetworkFloorParticipant::SetFloorGrantedCb (const Callback<void>  floorGrantedCb)
{
  NS_LOG_FUNCTION (this);

  m_floorGrantedCb = floorGrantedCb;
}

void
McpttOnNetworkFloorParticipant::SetOriginator (const bool& originator)
{
  NS_LOG_FUNCTION (this << originator);

  m_originator = originator;
}

void
McpttOnNetworkFloorParticipant::SetOverridden (const bool& overridden)
{
  NS_LOG_FUNCTION (this << overridden);

  m_overridden = overridden;
}

void
McpttOnNetworkFloorParticipant::SetOverriding (const bool& overriding)
{
  NS_LOG_FUNCTION (this << overriding);

  m_overriding = overriding;
}

void
McpttOnNetworkFloorParticipant::SetCall (Ptr<McpttCall> call)
{
  NS_LOG_FUNCTION (this);

  m_call = call;
}

void
McpttOnNetworkFloorParticipant::SetPriority (uint8_t priority)
{
  NS_LOG_FUNCTION (this << +priority);

  m_priority = priority;
}

void
McpttOnNetworkFloorParticipant::SetState (Ptr<McpttOnNetworkFloorParticipantState>  state)
{
  NS_LOG_FUNCTION (this << state->GetInstanceStateId ());

  m_state = state;
}

void
McpttOnNetworkFloorParticipant::SetStateChangeCb (const Callback<void, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  NS_LOG_FUNCTION (this);

  m_stateChangeCb = stateChangeCb;
}
/** McpttOnNetworkFloorParticipant - end **/

} // namespace psc
} // namespace ns3

