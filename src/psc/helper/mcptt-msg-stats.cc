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

#include <ns3/boolean.h>
#include <ns3/log.h>
#include <ns3/mcptt-msg.h>
#include <ns3/mcptt-ptt-app.h>
#include <ns3/object.h>
#include <ns3/type-id.h>
#include <ns3/string.h>

#include "mcptt-msg-stats.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE ("McpttMsgStats");

NS_OBJECT_ENSURE_REGISTERED (McpttMsgStats);

TypeId
McpttMsgStats::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

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
  : Object (),
    m_firstMsg (true)
{
  NS_LOG_FUNCTION (this);
}

McpttMsgStats::~McpttMsgStats (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttMsgStats::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttMsgStats::GetTypeId ();
}

void
McpttMsgStats::ReceiveRxTrace (const McpttPttApp& pttApp, const McpttMsg& msg)
{
  NS_LOG_FUNCTION (this << &pttApp << &msg);

  Trace (pttApp, msg, true);
}

void
McpttMsgStats::ReceiveTxTrace (const McpttPttApp& pttApp, const McpttMsg& msg)
{
  NS_LOG_FUNCTION (this);

  Trace (pttApp, msg, false);
}

void
McpttMsgStats::Trace (const McpttPttApp& pttApp, const McpttMsg& msg, bool rx)
{
  NS_LOG_FUNCTION (this << &pttApp << &msg);

  if ((msg.IsA (McpttCallMsg::GetTypeId ()) && m_callControl == true)
      || (msg.IsA (McpttFloorMsg::GetTypeId ()) && m_floorControl == true)
      || (msg.IsA (McpttMediaMsg::GetTypeId ()) && m_media == true))
    {

      std::ofstream outFile;
      if (m_firstMsg == true)
        {
          m_firstMsg = false;
          outFile.open (m_outputFileName.c_str ());
          outFile << "time(ms)\tuserid\trx/tx\tbytes\tmessage";
          outFile << std::endl;
        }
      else
        {
          outFile.open (m_outputFileName.c_str (),  std::ios_base::app);
        }

      outFile << Simulator::Now ().GetMilliSeconds ();
      outFile << "\t" << pttApp.GetUserId ();
      outFile << "\t" << (rx ? "RX" : "TX");
      outFile << "\t" << msg.GetSerializedSize ();
      if (m_includeMsgContent)
        {
          outFile << "\t";
          msg.Print (outFile);
        }
      else
        {
          outFile << "\t" << msg.GetInstanceTypeId ();
        }
      outFile << std::endl;

      outFile.close ();
    }
}

} // namespace ns3

