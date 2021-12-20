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

#include <string>
#include <iomanip>

#include <ns3/callback.h>
#include <ns3/config.h>
#include <ns3/log.h>
#include <ns3/ptr.h>
#include <ns3/mcptt-media-msg.h>
#include "mcptt-msg-stats.h"
#include "mcptt-state-machine-stats.h"
#include "mcptt-trace-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttTraceHelper");

namespace psc {

TypeId
McpttTraceHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttTraceHelper")
    .SetParent<Object> ()
    .AddConstructor<McpttTraceHelper>()
    .AddTraceSource ("AccessTimeTrace",
                     "Outputs an access time trace sample",
                     MakeTraceSourceAccessor (&McpttTraceHelper::m_accessTimeTrace),
                     "ns3::psc::McpttTraceHelper::AccessTimeTracedCallback")
    .AddTraceSource ("MouthToEarLatencyTrace",
                     "Outputs a mouth-to-ear latency trace sample",
                     MakeTraceSourceAccessor (&McpttTraceHelper::m_mouthToEarLatencyTrace),
                     "ns3::psc::McpttTraceHelper::MouthToEarLatencyTracedCallback")
  ;
  return tid;
}
McpttTraceHelper::McpttTraceHelper (void)
  : m_msgTracer (nullptr),
    m_stateMachineTracer (nullptr)
{
  NS_LOG_FUNCTION (this);
}

McpttTraceHelper::~McpttTraceHelper ()
{
  NS_LOG_FUNCTION (this);

  if (m_mouthToEarLatencyTraceFile.is_open ())
    {
      m_mouthToEarLatencyTraceFile.close ();
    }

  if (m_accessTimeTraceFile.is_open ())
    {
      m_accessTimeTraceFile.close ();
    }
}

void
McpttTraceHelper::EnableMsgTraces (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_msgTracer)
    {
      m_msgTracer = CreateObject<McpttMsgStats> ();
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/RxTrace", MakeCallback (&McpttMsgStats::ReceiveRxTrace, m_msgTracer));
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/TxTrace", MakeCallback (&McpttMsgStats::ReceiveTxTrace, m_msgTracer));
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/RxTrace", MakeCallback (&McpttMsgStats::ReceiveRxTrace, m_msgTracer));
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/TxTrace", MakeCallback (&McpttMsgStats::ReceiveTxTrace, m_msgTracer));
    }
}

void
McpttTraceHelper::DisableMsgTraces (void)
{
  NS_LOG_FUNCTION (this);
  if (m_msgTracer != nullptr)
    {
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/RxTrace", MakeCallback (&McpttMsgStats::ReceiveRxTrace, m_msgTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/TxTrace", MakeCallback (&McpttMsgStats::ReceiveTxTrace, m_msgTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/RxTrace", MakeCallback (&McpttMsgStats::ReceiveRxTrace, m_msgTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/TxTrace", MakeCallback (&McpttMsgStats::ReceiveTxTrace, m_msgTracer));
    }
}


void
McpttTraceHelper::EnableStateMachineTraces (void)
{
  NS_LOG_FUNCTION (this);
  if (!m_stateMachineTracer)
    {
      m_stateMachineTracer = CreateObject<McpttStateMachineStats> ();
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/Calls/*/CallMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/Calls/*/CallMachine/CallTypeMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/Calls/*/CallMachine/EmergAlertMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/Calls/*/FloorMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/Calls/*/FloorArbitrator/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/Calls/*/FloorArbitrator/FloorParticipants/*/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/Calls/*/FloorArbitrator/DualFloorControl/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/Calls/*/CallMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
    }
}

void
McpttTraceHelper::DisableStateMachineTraces (void)
{
  NS_LOG_FUNCTION (this);
  if (m_stateMachineTracer != nullptr)
    {
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/Calls/*/CallMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/Calls/*/CallMachine/CallTypeMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/Calls/*/CallMachine/EmergAlertMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/Calls/*/FloorMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/Calls/*/FloorArbitrator/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/Calls/*/FloorArbitrator/FloorParticipants/*/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/Calls/*/FloorArbitrator/DualFloorControl/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttServerApp/Calls/*/CallMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
    }
}

void
McpttTraceHelper::TraceMcpttMediaMsg (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  NS_LOG_FUNCTION (this << app << callId << pkt << headerType);
  if (headerType == McpttMediaMsg::GetTypeId ())
    {
      McpttMediaMsg msg;
      pkt->PeekHeader (msg);
      std::pair<uint32_t, uint16_t> key = std::make_pair (app->GetNode ()->GetId (), callId);
      Time talkSpurtStart = msg.GetTalkSpurtStart ();
      uint32_t ssrc = msg.GetSsrc ();
      auto it = m_mouthToEarLatencyMap.find (key);
      if (it == m_mouthToEarLatencyMap.end ())
        {
          m_mouthToEarLatencyMap.insert (std::make_pair (key, talkSpurtStart));
          NS_LOG_DEBUG ("First talk spurt for node: " << app->GetNode ()->GetId () << " callId " << callId);
          RecordMouthToEarLatency (Simulator::Now (), ssrc, app->GetNode ()->GetId (), callId, Simulator::Now () - talkSpurtStart);
        }
      else
        {
          if (it->second < talkSpurtStart)
            {
              it->second = talkSpurtStart;
              NS_LOG_DEBUG ("New talk spurt for node: " << app->GetNode ()->GetId () << " callId " << callId);
              RecordMouthToEarLatency (Simulator::Now (), ssrc, app->GetNode ()->GetId (), callId, Simulator::Now () - talkSpurtStart);
            }
        }
    }
}

void
McpttTraceHelper::EnableMouthToEarLatencyTrace (void)
{
  NS_LOG_FUNCTION (this);

  Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/RxTrace", MakeCallback (&McpttTraceHelper::TraceMcpttMediaMsg, this));
}

void
McpttTraceHelper::EnableMouthToEarLatencyTrace (std::string filename)
{
  NS_LOG_FUNCTION (this << filename);

  if (! m_mouthToEarLatencyTraceFile.is_open ())
    {
      m_mouthToEarLatencyTraceFile.open (filename.c_str ());
      m_mouthToEarLatencyTraceFile << "#";
      m_mouthToEarLatencyTraceFile << std::setw (9) << "time(s)";
      m_mouthToEarLatencyTraceFile << std::setw (5) << "ssrc";
      m_mouthToEarLatencyTraceFile << std::setw (7) << "nodeid";
      m_mouthToEarLatencyTraceFile << std::setw (7) << "callid";
      m_mouthToEarLatencyTraceFile << " latency(s)" << std::endl;
    }

  EnableMouthToEarLatencyTrace ();
}

void
McpttTraceHelper::DisableMouthToEarLatencyTrace (void)
{
  NS_LOG_FUNCTION (this);

  Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/RxTrace", MakeCallback (&McpttTraceHelper::TraceMcpttMediaMsg, this));

  if (m_mouthToEarLatencyTraceFile.is_open ())
    {
      m_mouthToEarLatencyTraceFile.close ();
    }
}

// Possible events (implied state transitions) traced here
//
// PendingRequest -> R: Floor Deny : Denied ('D')
// PendingRequest -> R: Floor Revoke : Failed ('F')
// PendingRequest -> Timer T101 exp. N times : Failed ('F')
// PendingRequest -> PTT button released : Abandoned ('A')
// Queued -> R: Floor Deny : Denied ('D')
// Queued -> R: Floor Revoke : Failed ('F')
// Queued -> R: Floor Idle : Failed ('F')
// Queued -> Timer T104 exp. N times : Failed ('F')
// Queued -> Timer T132 expired : Not applicable (floor previously granted)
// Queued -> PTT button released : Abandoned ('A')
//
void
McpttTraceHelper::TraceEventsForAccessTime (uint32_t userId, uint16_t callId, const std::string& selected, const char* description)
{
  NS_LOG_FUNCTION (userId << callId << selected << description);

  std::pair<uint32_t, uint16_t> key = std::make_pair (userId, callId);
  auto it = m_accessTimeMap.find (key);
  if (it == m_accessTimeMap.end ())
    {
      if (strcmp (description, McpttCallMachine::CALL_INITIATED) == 0)
        {
          std::pair<Time, std::string> item = std::make_pair (Simulator::Now (), description);
          m_accessTimeMap.insert (std::make_pair (key, item));
          return;
        }
      else
        {
          NS_LOG_DEBUG ("Event key not found for " << userId  << " " << callId);
          return;
        }
    }
  if (it->second.second == "'U: pending Request'"
      || it->second.second == "'O: pending request'")
    {
      if (strcmp (description, McpttFloorParticipant::RECEIVED_FLOOR_DENY) == 0)
        {
          NS_LOG_DEBUG ("pending request, received event RECEIVED_FLOOR_DENY " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "D", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (strcmp (description, McpttFloorParticipant::RECEIVED_FLOOR_REVOKED) == 0)
        {
          NS_LOG_DEBUG ("pending request, received event RECEIVED_FLOOR_REVOKED " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "F", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (strcmp (description, McpttFloorParticipant::TIMER_T101_EXPIRED_N_TIMES) == 0)
        {
          NS_LOG_DEBUG ("pending request, received event TIMER_T101_EXPIRED_N_TIMES " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "F", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (strcmp (description, McpttFloorParticipant::PTT_BUTTON_RELEASED) == 0)
        {
          NS_LOG_DEBUG ("pending request, received event PTT_BUTTON_RELEASED " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "A", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
    }
  else if (it->second.second == "'U: queued'"
           || it->second.second == "'O: queued'")
    {
      if (strcmp (description, McpttFloorParticipant::RECEIVED_FLOOR_GRANTED) == 0)
        {
          NS_LOG_DEBUG ("queued, received event RECEIVED_FLOOR_GRANTED " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "Q", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (strcmp (description, McpttFloorParticipant::RECEIVED_FLOOR_DENY) == 0)
        {
          NS_LOG_DEBUG ("queued, received event RECEIVED_FLOOR_DENY " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "D", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (strcmp (description, McpttFloorParticipant::RECEIVED_FLOOR_REVOKED) == 0)
        {
          NS_LOG_DEBUG ("queued, received event RECEIVED_FLOOR_REVOKED " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "F", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (strcmp (description, McpttFloorParticipant::RECEIVED_FLOOR_IDLE) == 0)
        {
          NS_LOG_DEBUG ("queued, received event RECEIVED_FLOOR_IDLE " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "F", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (strcmp (description, McpttFloorParticipant::TIMER_T101_EXPIRED_N_TIMES) == 0)
        {
          NS_LOG_DEBUG ("queued, received event TIMER_T101_EXPIRED_N_TIMES " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "F", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (strcmp (description, McpttFloorParticipant::TIMER_T204_EXPIRED_N_TIMES) == 0)
        {
          NS_LOG_DEBUG ("queued, received event TIMER_T204_EXPIRED_N_TIMES " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "F", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (strcmp (description, McpttFloorParticipant::TIMER_T203_EXPIRED) == 0)
        {
          NS_LOG_DEBUG ("queued, received event TIMER_T203_EXPIRED " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "F", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (strcmp (description, McpttFloorParticipant::TIMER_T233_EXPIRED) == 0)
        {
          NS_LOG_DEBUG ("queued, received event TIMER_T233_EXPIRED " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "F", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (strcmp (description, McpttFloorParticipant::TIMER_T132_EXPIRED) == 0)
        {
          NS_FATAL_ERROR ("Floor should have been previously granted and entry removed");
        }
      else if (strcmp (description, McpttFloorParticipant::PTT_BUTTON_RELEASED) == 0)
        {
          NS_LOG_DEBUG ("queued, received event PTT_BUTTON_RELEASED " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "A", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
    }
}

// Possible state transitions traced here:
// StartStop or HasNoPermission -> PendingRequest (start of an access trace)
// PendingRequest -> HasPermission : Immediate ('I') outcome
//
// Regarding the following outcome:
// PendingRequest -> Queued -> HasPermission : Queued ('Q') outcome
// This path could be traced here, but because the transition from Queued
// to HasPermission requires the user or PttApp to accept the grant before
// changing state, we handle it instead in TraceEventsForAccessTime
//
// Other transitions from PendingRequest or Queued (to PendingRelease or
// HasNoPermission) are handled in TraceEventsForAccessTime
//
// Note:  StartStop -> HasPermission : Immediate ('I') outcome described in
// Section 6.2.4.2.2 (NOTE) is not traced for access time
//
void
McpttTraceHelper::TraceStatesForAccessTime (uint32_t userId, uint16_t callId, const std::string& selected, const std::string& typeId, const std::string& oldStateName, const std::string& newStateName)
{
  NS_LOG_FUNCTION (userId << callId << selected << typeId << oldStateName << newStateName);

  // Note: 'selected' field is not used by this method
  std::pair<uint32_t, uint16_t> key = std::make_pair (userId, callId);
  auto it = m_accessTimeMap.find (key);
  if (it == m_accessTimeMap.end ())
    {
      if ((oldStateName == "'Start-stop'"
           || oldStateName == "'U: has no permission'"
           || oldStateName == "'O: has no permission'"
           || oldStateName == "'O: silence'"
           || oldStateName == "'O: queued'")
          && (newStateName == "'U: pending Request'"
              || newStateName == "'O: pending request'"))
        {
          std::pair<Time, std::string> item = std::make_pair (Simulator::Now (), newStateName);
          m_accessTimeMap.insert (std::make_pair (key, item));
        }
      else if (oldStateName == "'Start-stop'"
               && newStateName == "'U: has permission'")
        {
          NS_LOG_DEBUG ("start stop, received event CALL_ORIGINATOR " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "I", Seconds (0));
        }
      return;
    }

  if (it->second.second == McpttCallMachine::CALL_INITIATED)
    {
      if (oldStateName == "'Start-stop'"
          && newStateName == "'O: has permission'")
        {
          NS_LOG_DEBUG ("start stop, received event CALL_ORIGINATOR " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "I", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else
        {
          NS_LOG_DEBUG ("call initiated, received event CALL_ORIGINATOR " << userId << " " << callId);
          RecordAccessTime (Simulator::Now (), userId, callId, "F", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
    }
  else if (it->second.second == "'U: pending Request'"
      || it->second.second == "'O: pending request'")
    {
      if (newStateName == "'U: has permission'"
          || newStateName == "'O: has permission'")
        {
          RecordAccessTime (Simulator::Now (), userId, callId, "I", Simulator::Now () - it->second.first);
          m_accessTimeMap.erase (key);
          return;
        }
      else if (newStateName == "'U: queued'"
               || newStateName == "'O: queued'")
        {
          it->second.second = newStateName;
          return;
        }
      else if (newStateName == "'U: has no permission'"
               || newStateName == "'O: has no permission'")
        {
          // Transition can either be due to Floor Deny or T101 expiry
          // Both cases are handled by the event trace, which should
          // catch them before this state transition and erase the entry
          NS_FATAL_ERROR ("Invalid transition to U: has no permission");
        }
      else if (newStateName == "'U: pending release'"
               || newStateName == "'O: silence'")
        {
          // Transition can either be due to Floor Revoke or PTT button release
          // Both cases are handled by the event trace, which should
          // catch them before this state transition and erase the entry
          // However, end of call may trigger this also, so just erase here
          m_accessTimeMap.erase (key);
          return;
        }
      else if (newStateName == "'Releasing'" || newStateName == "'Start-stop'")
        {
          // No such transitions listed in TS 24.380 Figure 6.2.4.1-1
          // However, end of call may trigger this also, so just erase here
          m_accessTimeMap.erase (key);
          return; 
        }
      else
        {
          // Unknown state transition
          NS_FATAL_ERROR ("Unknown state transition from U:  pending request");
        }
    }
  else if (it->second.second == "'U: queued'"
      || it->second.second == "'O: queued'")
    {
      if (newStateName == "'U: has permission'"
          || newStateName == "'O: has permission'")
        {
          NS_FATAL_ERROR ("This transition should be traced as an event");
        }
      else if (newStateName == "'U: has no permission'"
               || newStateName == "'O: has no permission'")
        {
          // Transition can either be due to Floor Deny or Floor Idle or
          // T132 expiry
          // These cases are handled by the event trace, which should
          // catch them before this state transition and erase the entry
          NS_FATAL_ERROR ("Invalid transition to U: has no permission");
        }
      else if (newStateName == "'U: pending release'"
               || newStateName == "'O: silence'")
        {
          // Transition can either be due to T104 expiry or PTT button release
          // Both cases are handled by the event trace, which should
          // catch them before this state transition and erase the entry
          // However, end of call may trigger this also, so just erase here
          m_accessTimeMap.erase (key);
          return;
        }
      else if (newStateName == "'Releasing'" || newStateName == "'Start-stop'")
        {
          // No such transitions listed in TS 24.380 Figure 6.2.4.1-1
          // However, end of call may trigger this also, so just erase here
          m_accessTimeMap.erase (key);
          return;
        }
      else
        {
          // Unknown state transition
          NS_FATAL_ERROR ("Unknown state transition from U:  queued");
        }
    }
}

void
McpttTraceHelper::EnableAccessTimeTrace (void)
{
  NS_LOG_FUNCTION (this);

  Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/Calls/*/FloorMachine/StateChangeTrace", MakeCallback (&McpttTraceHelper::TraceStatesForAccessTime, this));
  Config::ConnectWithoutContextFailSafe ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/EventTrace", MakeCallback (&McpttTraceHelper::TraceEventsForAccessTime, this));
}

void
McpttTraceHelper::EnableAccessTimeTrace (std::string filename)
{
  NS_LOG_FUNCTION (this << filename);

  if (! m_accessTimeTraceFile.is_open ())
    {
      m_accessTimeTraceFile.open (filename.c_str ());
      m_accessTimeTraceFile << "#";
      m_accessTimeTraceFile << std::setw (9) << "time(s)";
      m_accessTimeTraceFile << std::setw (7) << "userid";
      m_accessTimeTraceFile << std::setw (7) << "callid";
      m_accessTimeTraceFile << std::setw (7) << "result";
      m_accessTimeTraceFile << " latency(s)" << std::endl;
    }

  EnableAccessTimeTrace ();
}

void
McpttTraceHelper::DisableAccessTimeTrace (void)
{
  NS_LOG_FUNCTION (this);

  Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/Calls/*/FloorMachine/StateChangeTrace", MakeCallback (&McpttTraceHelper::TraceStatesForAccessTime, this));
  Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::psc::McpttPttApp/EventTrace", MakeCallback (&McpttTraceHelper::TraceEventsForAccessTime, this));

  if (m_accessTimeTraceFile.is_open ())
    {
      m_accessTimeTraceFile.close ();
    }
}

void
McpttTraceHelper::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_msgTracer = nullptr;
  m_stateMachineTracer = nullptr;
}

void
McpttTraceHelper::RecordAccessTime (Time ts, uint32_t userId, uint16_t callId, std::string result, Time latency)
{
  NS_LOG_FUNCTION (this);

  if (m_accessTimeTraceFile.is_open ())
    {
      m_accessTimeTraceFile << std::fixed << std::setw (10) << ts.GetSeconds ();
      m_accessTimeTraceFile << std::setw (7) << userId;
      m_accessTimeTraceFile << std::setw (7) << callId;
      m_accessTimeTraceFile << std::setw (6) << result;
      m_accessTimeTraceFile << std::fixed << std::setw (11) << latency.GetSeconds () << std::endl;
    }

  m_accessTimeTrace (ts, userId, callId, result, latency);
}

void
McpttTraceHelper::RecordMouthToEarLatency (Time ts, uint32_t ssrc, uint64_t nodeId, uint16_t callId, Time latency)
{
  NS_LOG_FUNCTION (this);

  if (m_mouthToEarLatencyTraceFile.is_open ())
    {
      m_mouthToEarLatencyTraceFile << std::fixed << std::setw (10) << ts.GetSeconds ();
      m_mouthToEarLatencyTraceFile << std::setw (5) << ssrc;
      m_mouthToEarLatencyTraceFile << std::setw (7) << nodeId;
      m_mouthToEarLatencyTraceFile << std::setw (7) << callId;
      m_mouthToEarLatencyTraceFile << std::fixed << std::setw (11) << latency.GetSeconds () << std::endl;
    }

  m_mouthToEarLatencyTrace (ts, ssrc, nodeId, callId, latency);
}

} // namespace psc
} // namespace ns3

