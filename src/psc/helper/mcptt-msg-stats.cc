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

#include <fstream>

#include <ns3/abort.h>
#include <ns3/boolean.h>
#include <ns3/log.h>
#include <ns3/mcptt-msg.h>
#include <ns3/mcptt-ptt-app.h>
#include <ns3/mcptt-server-app.h>
#include <ns3/mcptt-on-network-floor-arbitrator.h>
#include <ns3/object.h>
#include <ns3/type-id.h>
#include <ns3/string.h>
#include <ns3/sip-header.h>

#include "mcptt-msg-stats.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE ("McpttMsgStats");

NS_OBJECT_ENSURE_REGISTERED (McpttMsgStats);

TypeId
McpttMsgStats::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttMsgStats")
    .SetParent<Object> ()
    .AddConstructor<McpttMsgStats> ()
    .AddAttribute ("CallControl",
                   "Indicates if call control messages should be included.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttMsgStats::m_callControl),
                   MakeBooleanChecker ())
    .AddAttribute ("FloorControl",
                   "Indicates if floor control messages should be included.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttMsgStats::m_floorControl),
                   MakeBooleanChecker ())
    .AddAttribute ("IncludeMessageContent",
                   "Indicates if the content of a traced message should be included.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttMsgStats::m_includeMsgContent),
                   MakeBooleanChecker ())
    .AddAttribute ("Media",
                   "Indicates if media messages should be included.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&McpttMsgStats::m_media),
                   MakeBooleanChecker ())
    .AddAttribute ("OutputFileName",
                   "The name to use for the trace file.",
                   StringValue ("mcptt_msg_stats.txt"),
                   MakeStringAccessor (&McpttMsgStats::m_outputFileName),
                   MakeStringChecker ())
    ;
  return tid;
}

McpttMsgStats::McpttMsgStats (void)
  : m_firstMsg (true)
{
  NS_LOG_FUNCTION (this);
}

McpttMsgStats::~McpttMsgStats (void)
{
  NS_LOG_FUNCTION (this);
  if (m_outputFile.is_open ())
    {
      m_outputFile.close ();
    }
}

void
McpttMsgStats::ReceiveRxTrace (Ptr<const Application> app, uint16_t callId, const Header& msg)
{
  NS_LOG_FUNCTION (this << app << callId << &msg);

  Trace (app, callId, msg, true);
}

void
McpttMsgStats::ReceiveTxTrace (Ptr<const Application> app, uint16_t callId, const Header& msg)
{
  NS_LOG_FUNCTION (this << app << callId << &msg);

  Trace (app, callId, msg, false);
}

void
McpttMsgStats::Trace (Ptr<const Application> app, uint16_t callId, const Header& msg, bool rx)
{
  NS_LOG_FUNCTION (this << app << callId << &msg);
  if (m_firstMsg == true)
    {
      m_firstMsg = false;
      m_outputFile.open (m_outputFileName.c_str ());
      m_outputFile << "time(ms) nodeid\tcallid\tssrc\trx/tx\tbytes\tmessage";
      m_outputFile << std::endl;
    }
  if (msg.GetInstanceTypeId () == SipHeader::GetTypeId ())
    {
      m_outputFile << Simulator::Now ().GetMilliSeconds ();
      m_outputFile << "\t" << app->GetNode ()->GetId ();
      m_outputFile << "\t" << callId;
      m_outputFile << "\t" << "N/A";  // ssrc not applicable
      m_outputFile << "\t" << (rx ? "RX" : "TX");
      m_outputFile << "\t" << msg.GetSerializedSize ();
      m_outputFile << "\t" << "SIP " << static_cast<const SipHeader&> (msg).GetMessageTypeName ();
      if (static_cast<const SipHeader&> (msg).GetMessageType () == SipHeader::SIP_REQUEST)
        {
          m_outputFile << "  " << static_cast<const SipHeader&> (msg).GetMethodName ();
        }
      else if (static_cast<const SipHeader&> (msg).GetMessageType () == SipHeader::SIP_RESPONSE)
        {
          m_outputFile << " " << static_cast<const SipHeader&> (msg).GetStatusCode ();
        }
      else
        {
          m_outputFile << "\tUNKNOWN";
        }
      m_outputFile << std::endl;
    }
  else if ((msg.GetInstanceTypeId ().IsChildOf (McpttCallMsg::GetTypeId ()) && m_callControl == true)
      || (msg.GetInstanceTypeId ().IsChildOf (McpttFloorMsg::GetTypeId ()) && m_floorControl == true)
      || (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId () && m_media == true))
    {
      m_outputFile << Simulator::Now ().GetMilliSeconds ();
      m_outputFile << "\t" << app->GetNode ()->GetId ();
      m_outputFile << "\t" << callId;

      if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
        {
          m_outputFile << "\t" << static_cast<const McpttMediaMsg&> (msg).GetSsrc ();
        }
      else if (msg.GetInstanceTypeId ().IsChildOf (McpttFloorMsg::GetTypeId ()))
        {
          m_outputFile << "\t" << static_cast<const McpttFloorMsg&> (msg).GetSsrc ();
        }
      else
        {
          m_outputFile << "\t" << "N/A";  // not applicable
        }

      m_outputFile << "\t" << (rx ? "RX" : "TX");
      m_outputFile << "\t" << msg.GetSerializedSize ();

      if (m_includeMsgContent)
        {
          m_outputFile << "\t";
          msg.Print (m_outputFile);
        }
      else
        {
          m_outputFile << "\t" << msg.GetInstanceTypeId ();
        }
      m_outputFile << std::endl;
    }
  else
    {
      m_outputFile << Simulator::Now ().GetMilliSeconds ();
      m_outputFile << "\t" << app->GetNode ()->GetId ();
      m_outputFile << "\t" << "Unknown header type: " << msg.GetInstanceTypeId ().GetName ();
      m_outputFile << std::endl;
    }
}

} // namespace ns3

