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

McpttTraceHelper::McpttTraceHelper (void)
  : m_msgTracer (0),
    m_stateMachineTracer (0)
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
}

void
McpttTraceHelper::EnableMsgTraces (void)
{
  NS_LOG_FUNCTION (this);
  if (m_msgTracer == 0)
    {
      m_msgTracer = CreateObject<McpttMsgStats> ();
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/RxTrace", MakeCallback (&McpttMsgStats::ReceiveRxTrace, m_msgTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/TxTrace", MakeCallback (&McpttMsgStats::ReceiveTxTrace, m_msgTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/RxTrace", MakeCallback (&McpttMsgStats::ReceiveRxTrace, m_msgTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/TxTrace", MakeCallback (&McpttMsgStats::ReceiveTxTrace, m_msgTracer));
    }
}

void
McpttTraceHelper::DisableMsgTraces (void)
{
  NS_LOG_FUNCTION (this);
  if (m_msgTracer)
    {
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/RxTrace", MakeCallback (&McpttMsgStats::ReceiveRxTrace, m_msgTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/TxTrace", MakeCallback (&McpttMsgStats::ReceiveTxTrace, m_msgTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/RxTrace", MakeCallback (&McpttMsgStats::ReceiveRxTrace, m_msgTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/TxTrace", MakeCallback (&McpttMsgStats::ReceiveTxTrace, m_msgTracer));
    }
}


void
McpttTraceHelper::EnableStateMachineTraces (void)
{
  NS_LOG_FUNCTION (this);
  if (m_stateMachineTracer == 0)
    {
      m_stateMachineTracer = CreateObject<McpttStateMachineStats> ();
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/CallMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/CallMachine/CallTypeMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/CallMachine/EmergAlertMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/FloorMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/Calls/*/FloorArbitrator/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/Calls/*/FloorArbitrator/FloorParticipants/*/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/Calls/*/FloorArbitrator/DualFloorControl/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/Calls/*/CallMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
    }
}

void
McpttTraceHelper::DisableStateMachineTraces (void)
{
  NS_LOG_FUNCTION (this);
  if (m_stateMachineTracer)
    {
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/CallMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/CallMachine/CallTypeMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/CallMachine/EmergAlertMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/Calls/*/FloorMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/Calls/*/FloorArbitrator/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/Calls/*/FloorArbitrator/FloorParticipants/*/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/Calls/*/FloorArbitrator/DualFloorControl/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
      Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttServerApp/Calls/*/CallMachine/StateChangeTrace", MakeCallback (&McpttStateMachineStats::StateChangeCb, m_stateMachineTracer));
    }
}

void
McpttTraceHelper::TraceMcpttMediaMsg (Ptr<const Application> app, uint16_t callId, const Header& msg)
{
  NS_LOG_FUNCTION (this << app << callId);
  if (msg.GetInstanceTypeId ().IsChildOf (McpttMediaMsg::GetTypeId ()))
    {
      std::pair<uint32_t, uint16_t> key = std::make_pair (app->GetNode ()->GetId (), callId);
      Time talkSpurtStart = dynamic_cast<const McpttMediaMsg&>(msg).GetTalkSpurtStart ();
      auto it = m_mouthToEarLatencyMap.find (key);
      if (it == m_mouthToEarLatencyMap.end ())
        {
          m_mouthToEarLatencyMap.insert (std::make_pair (key, talkSpurtStart));
          m_mouthToEarLatencyTraceFile << Simulator::Now ().GetMilliSeconds () << "\t"
            << app->GetNode ()->GetId () << "\t" << callId << "\t" 
            << (Simulator::Now () - talkSpurtStart).GetMilliSeconds ()
            << std::endl;
        }
      else
        {
          if (it->second < talkSpurtStart)
            {
              it->second = talkSpurtStart;
              m_mouthToEarLatencyTraceFile << Simulator::Now ().GetMilliSeconds () << "\t"
                << app->GetNode ()->GetId () << "\t" << callId << "\t" 
                << (Simulator::Now () - talkSpurtStart).GetMilliSeconds ()
                << std::endl;
            }
        }
    }
}

void
McpttTraceHelper::EnableMouthToEarLatencyTrace (std::string filename)
{
  NS_LOG_FUNCTION (this << filename);
  m_mouthToEarLatencyTraceFile.open (filename.c_str ());
  m_mouthToEarLatencyTraceFile << "time(ms) nodeid\tcallid\tlatency(ms)" << std::endl;
  Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/RxTrace", MakeCallback (&McpttTraceHelper::TraceMcpttMediaMsg, this));
}

void
McpttTraceHelper::DisableMouthToEarLatencyTrace (void)
{
  NS_LOG_FUNCTION (this);
  Config::DisconnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::McpttPttApp/RxTrace", MakeCallback (&McpttTraceHelper::TraceMcpttMediaMsg, this));
}

} // namespace ns3

