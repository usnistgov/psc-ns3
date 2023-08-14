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
#include <iomanip>

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

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttMsgStats");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttMsgStats);

TypeId
McpttMsgStats::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttMsgStats")
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
                   StringValue ("mcptt-msg-stats.txt"),
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
McpttMsgStats::ReceiveRxTrace (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  NS_LOG_FUNCTION (this << app << callId << pkt << headerType);

  Trace (app, callId, pkt, headerType, true);
}

void
McpttMsgStats::ReceiveTxTrace (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  NS_LOG_FUNCTION (this << app << callId << pkt << headerType);

  Trace (app, callId, pkt, headerType, false);
}

McpttCallMsg*
McpttMsgStats::ResolveCallMsgType (Ptr<const Packet> pkt)
{
  McpttCallMsg* temp = new McpttCallMsg;
  pkt->PeekHeader (*temp);
  McpttCallMsgFieldMsgType msgType = temp->GetMsgType ();
  uint8_t code = msgType.GetType ();
  delete temp;

  if (code == McpttCallMsgGrpProbe::CODE)
    {
      McpttCallMsgGrpProbe* probeMsg = new McpttCallMsgGrpProbe;
      pkt->PeekHeader (*probeMsg);
      return probeMsg;
    }
  else if (code == McpttCallMsgGrpAnnoun::CODE)
    {
      McpttCallMsgGrpAnnoun* grpAnnounMsg = new McpttCallMsgGrpAnnoun;
      pkt->PeekHeader (*grpAnnounMsg);
      return grpAnnounMsg;
    }
  else if (code == McpttCallMsgGrpAccept::CODE)
    {
      McpttCallMsgGrpAccept* grpAcceptMsg = new McpttCallMsgGrpAccept;
      pkt->PeekHeader (*grpAcceptMsg);
      return grpAcceptMsg;
    }
  else if (code == McpttCallMsgGrpImmPerilEnd::CODE)
    {
      McpttCallMsgGrpImmPerilEnd* grpImmPerilEndMsg = new McpttCallMsgGrpImmPerilEnd;
      pkt->PeekHeader (*grpImmPerilEndMsg);
      return grpImmPerilEndMsg;
    }
  else if (code == McpttCallMsgGrpEmergEnd::CODE)
    {
      McpttCallMsgGrpEmergEnd* grpEmergEndMsg = new McpttCallMsgGrpEmergEnd;
      pkt->PeekHeader (*grpEmergEndMsg);
      return grpEmergEndMsg;
    }
  else if (code == McpttCallMsgGrpEmergAlert::CODE)
    {
      McpttCallMsgGrpEmergAlert* grpEmergAlertMsg = new McpttCallMsgGrpEmergAlert;
      pkt->PeekHeader (*grpEmergAlertMsg);
      return grpEmergAlertMsg;
    }
  else if (code == McpttCallMsgGrpEmergAlertAck::CODE)
    {
      McpttCallMsgGrpEmergAlertAck* grpEmergAlertAckMsg = new McpttCallMsgGrpEmergAlertAck;
      pkt->PeekHeader (*grpEmergAlertAckMsg);
      return grpEmergAlertAckMsg;
    }
  else if (code == McpttCallMsgGrpEmergAlertCancel::CODE)
    {
      McpttCallMsgGrpEmergAlertCancel* grpEmergAlertCancelMsg = new McpttCallMsgGrpEmergAlertCancel;
      pkt->PeekHeader (*grpEmergAlertCancelMsg);
      return grpEmergAlertCancelMsg;
    }
  else if (code == McpttCallMsgGrpEmergAlertCancelAck::CODE)
    {
      McpttCallMsgGrpEmergAlertCancelAck* emergAlertCancelAckMsg = new McpttCallMsgGrpEmergAlertCancelAck;
      pkt->PeekHeader (*emergAlertCancelAckMsg);
      return emergAlertCancelAckMsg;
    }
  else if (code == McpttCallMsgGrpBroadcast::CODE)
    {
      McpttCallMsgGrpBroadcast* grpBroadcastMsg = new McpttCallMsgGrpBroadcast;
      pkt->PeekHeader (*grpBroadcastMsg);
      return grpBroadcastMsg;
    }
  else if (code == McpttCallMsgGrpBroadcastEnd::CODE)
    {
      McpttCallMsgGrpBroadcastEnd* grpBroadcastEndMsg = new McpttCallMsgGrpBroadcastEnd;
      pkt->PeekHeader (*grpBroadcastEndMsg);
      return grpBroadcastEndMsg;
    }
  else if (code == McpttCallMsgPrivateSetupReq::CODE)
    {
      McpttCallMsgPrivateSetupReq* privateSetupReqMsg = new McpttCallMsgPrivateSetupReq;
      pkt->PeekHeader (*privateSetupReqMsg);
      return privateSetupReqMsg;
    }
  else if (code == McpttCallMsgPrivateRinging::CODE)
    {
      McpttCallMsgPrivateRinging* privateRingingMsg = new McpttCallMsgPrivateRinging;
      pkt->PeekHeader (*privateRingingMsg);
      return privateRingingMsg;
    }
  else if (code == McpttCallMsgPrivateAccept::CODE)
    {
      McpttCallMsgPrivateAccept* privateAcceptMsg = new McpttCallMsgPrivateAccept;
      pkt->PeekHeader (*privateAcceptMsg);
      return privateAcceptMsg;
    }
  else if (code == McpttCallMsgPrivateReject::CODE)
    {
      McpttCallMsgPrivateReject* privateRejectMsg = new McpttCallMsgPrivateReject;
      pkt->PeekHeader (*privateRejectMsg);
      return privateRejectMsg;
    }
  else if (code == McpttCallMsgPrivateRelease::CODE)
    {
      McpttCallMsgPrivateRelease* privateReleaseMsg = new McpttCallMsgPrivateRelease;
      pkt->PeekHeader (*privateReleaseMsg);
      return privateReleaseMsg;
    }
  else if (code == McpttCallMsgPrivateReleaseAck::CODE)
    {
      McpttCallMsgPrivateReleaseAck* privateReleaseAckMsg = new McpttCallMsgPrivateReleaseAck;
      pkt->PeekHeader (*privateReleaseAckMsg);
      return privateReleaseAckMsg;
    }
  else if (code == McpttCallMsgPrivateAcceptAck::CODE)
    {
      McpttCallMsgPrivateAcceptAck* privateAcceptAckMsg = new McpttCallMsgPrivateAcceptAck;
      pkt->PeekHeader (*privateAcceptAckMsg);
      return privateAcceptAckMsg;
    }
  else if (code == McpttCallMsgPrivateEmergCancel::CODE)
    {
      McpttCallMsgPrivateEmergCancel* privateEmergCancelMsg = new McpttCallMsgPrivateEmergCancel;
      pkt->PeekHeader (*privateEmergCancelMsg);
      return privateEmergCancelMsg;
    }
  else if (code == McpttCallMsgPrivateEmergCancelAck::CODE)
    {
      McpttCallMsgPrivateEmergCancelAck* privateEmergCancelAckMsg = new McpttCallMsgPrivateEmergCancelAck;
      pkt->PeekHeader (*privateEmergCancelAckMsg);
      return privateEmergCancelAckMsg;
    }
  NS_FATAL_ERROR ("Could not resolve message code = " << (uint32_t)code << ".");
  return nullptr;
}

McpttFloorMsg*
McpttMsgStats::ResolveFloorMsgType (Ptr<const Packet> pkt)
{
  McpttFloorMsg* temp = new McpttFloorMsg;

  pkt->PeekHeader (*temp);
  uint8_t subtype = temp->GetSubtype ();
  delete temp;

  if (subtype == McpttFloorMsgRequest::SUBTYPE)
    {
      McpttFloorMsgRequest* reqMsg = new McpttFloorMsgRequest;
      pkt->PeekHeader (*reqMsg);
      return reqMsg;
    }
  else if (subtype == McpttFloorMsgGranted::SUBTYPE
           || subtype == McpttFloorMsgGranted::SUBTYPE_ACK)
    {
      McpttFloorMsgGranted* grantedMsg = new McpttFloorMsgGranted;
      pkt->PeekHeader (*grantedMsg);
      return grantedMsg;
    }
  else if (subtype == McpttFloorMsgDeny::SUBTYPE
           || subtype == McpttFloorMsgDeny::SUBTYPE_ACK)
    {
      McpttFloorMsgDeny* denyMsg = new McpttFloorMsgDeny;
      pkt->PeekHeader (*denyMsg);
      return denyMsg;
    }
  else if (subtype == McpttFloorMsgRelease::SUBTYPE
           || subtype == McpttFloorMsgRelease::SUBTYPE_ACK)
    {
      McpttFloorMsgRelease* releaseMsg = new McpttFloorMsgRelease;
      pkt->PeekHeader (*releaseMsg);
      return releaseMsg;
    }
  else if (subtype == McpttFloorMsgIdle::SUBTYPE
           || subtype == McpttFloorMsgIdle::SUBTYPE_ACK)
    {
      McpttFloorMsgIdle* idleMsg = new McpttFloorMsgIdle;
      pkt->PeekHeader (*idleMsg);
      return idleMsg;
    }
  else if (subtype == McpttFloorMsgTaken::SUBTYPE
           || subtype == McpttFloorMsgTaken::SUBTYPE_ACK)
    {
      McpttFloorMsgTaken* takenMsg = new McpttFloorMsgTaken;
      pkt->PeekHeader (*takenMsg);
      return takenMsg;
    }
  else if (subtype == McpttFloorMsgRevoke::SUBTYPE)
    {
      McpttFloorMsgRevoke* revokeMsg = new McpttFloorMsgRevoke;
      pkt->PeekHeader (*revokeMsg);
      return revokeMsg;
    }
  else if (subtype == McpttFloorMsgQueuePositionRequest::SUBTYPE)
    {
      McpttFloorMsgQueuePositionRequest* queuePositionRequestMsg = new McpttFloorMsgQueuePositionRequest;
      pkt->PeekHeader (*queuePositionRequestMsg);
      return queuePositionRequestMsg;
    }
  else if (subtype == McpttFloorMsgQueuePositionInfo::SUBTYPE
           || subtype == McpttFloorMsgQueuePositionInfo::SUBTYPE_ACK)
    {
      McpttFloorMsgQueuePositionInfo* queueInfoMsg = new McpttFloorMsgQueuePositionInfo;
      pkt->PeekHeader (*queueInfoMsg);
      return queueInfoMsg;
    }
  else if (subtype == McpttFloorMsgAck::SUBTYPE)
    {
      McpttFloorMsgAck* ackMsg = new McpttFloorMsgAck;
      pkt->PeekHeader (*ackMsg);
      return ackMsg;
    }
  NS_FATAL_ERROR ("Could not resolve message subtype = " << (uint32_t)subtype << ".");
  return nullptr;
}

void
McpttMsgStats::Trace (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType, bool rx)
{
  NS_LOG_FUNCTION (this << app << callId << pkt << headerType << rx);
  if (m_firstMsg == true)
    {
      m_firstMsg = false;
      m_outputFile.open (m_outputFileName.c_str ());
      m_outputFile << "#";
      m_outputFile << std::setw (9) << "time(s)";
      m_outputFile << std::setw (7) << "nodeid";
      m_outputFile << std::setw (7) << "callid";
      m_outputFile << std::setw (5) << "ssrc";
      m_outputFile << std::setw (9) << "selected";
      m_outputFile << std::setw (7) << "rx/tx";
      m_outputFile << std::setw (6) << "bytes";
      m_outputFile << "  message";
      m_outputFile << std::endl;
    }
  // Determine if the message corresponds to the client's selected call
  Ptr<const McpttPttApp> pttApp = DynamicCast<const McpttPttApp> (app);
  std::string selected = "N/A";
  if (pttApp)
    {
      if (callId == pttApp->GetSelectedCall ()->GetCallId ())
        {
          selected = "True";
        }
      else
        {
          selected = "False";
        }
    }
  if (headerType == sip::SipHeader::GetTypeId () && m_callControl == true)
    {
      sip::SipHeader sipHeader;
      pkt->PeekHeader (sipHeader);
      m_outputFile << std::fixed << std::setw (10) << Simulator::Now ().GetSeconds ();
      m_outputFile << std::setw (6) << app->GetNode ()->GetId ();
      m_outputFile << std::setw (6) << callId;
      m_outputFile << std::setw (6) << "N/A";  // ssrc not applicable
      m_outputFile << std::setw (9) << selected;
      m_outputFile << std::setw (6) << (rx ? "RX" : "TX");
      m_outputFile << std::setw (6) << sipHeader.GetSerializedSize ();
      m_outputFile << "    SIP " << sipHeader.GetMessageTypeName ();
      if (sipHeader.GetMessageType () == sip::SipHeader::SIP_REQUEST)
        {
          m_outputFile << "  " << sipHeader.GetMethodName ();
        }
      else if (sipHeader.GetMessageType () == sip::SipHeader::SIP_RESPONSE)
        {
          uint16_t statusCode = sipHeader.GetStatusCode ();
          if (statusCode == 100)
            {
              m_outputFile << " 100 Trying";
            }
          else if (statusCode == 200)
            {
              m_outputFile << " 200 OK";
            }
          else
            {
              m_outputFile << " " << statusCode;
            }
        }
      else
        {
          m_outputFile << " UNKNOWN";
        }
      m_outputFile << std::endl;
    }
  else if (headerType.IsChildOf (McpttCallMsg::GetTypeId ()) && m_callControl == true)
    {
      McpttCallMsg* callMsg = ResolveCallMsgType (pkt);
      m_outputFile << std::fixed << std::setw (10) << Simulator::Now ().GetSeconds ();
      m_outputFile << std::setw (6) << app->GetNode ()->GetId ();
      m_outputFile << std::setw (6) << callId;
      m_outputFile << "  N/A";  // not applicable
      m_outputFile << std::setw (9) << selected;
      m_outputFile << std::setw (6) << (rx ? "RX" : "TX");
      m_outputFile << std::setw (6) << callMsg->GetSerializedSize ();
      if (m_includeMsgContent)
        {
          m_outputFile << "  ";
          callMsg->Print (m_outputFile);
        }
      else
        {
          // substr (10):  trims leading 'ns3::psc::'
          m_outputFile << std::left << "    " << headerType.GetName ().substr (10) << std::right;
        }
      m_outputFile << std::endl;
      delete callMsg;
    }
  else if (headerType.IsChildOf (McpttFloorMsg::GetTypeId ()) && m_floorControl == true)
    {
      McpttFloorMsg* floorMsg = ResolveFloorMsgType (pkt);
      m_outputFile << std::fixed << std::setw (10) << Simulator::Now ().GetSeconds ();
      m_outputFile << std::setw (6) << app->GetNode ()->GetId ();
      m_outputFile << std::setw (6) << callId;
      m_outputFile << std::setw (6) << floorMsg->GetSsrc ();
      m_outputFile << std::setw (9) << selected;
      m_outputFile << std::setw (6) << (rx ? "RX" : "TX");
      m_outputFile << std::setw (6) << floorMsg->GetSerializedSize ();
      if (m_includeMsgContent)
        {
          m_outputFile << "  ";
          floorMsg->Print (m_outputFile);
        }
      else
        {
          // substr (10):  trims leading 'ns3::psc::'
          m_outputFile << std::left << "    " << headerType.GetName ().substr (10) << std::right;
        }
      m_outputFile << std::endl;
      delete floorMsg;
    }
  else if (headerType == McpttMediaMsg::GetTypeId () && m_media == true)
    {
      McpttMediaMsg mediaMsg;
      pkt->PeekHeader (mediaMsg);
      m_outputFile << std::fixed << std::setw (10) << Simulator::Now ().GetSeconds ();
      m_outputFile << std::setw (6) << app->GetNode ()->GetId ();
      m_outputFile << std::setw (6) << callId;
      m_outputFile << std::setw (6) << mediaMsg.GetSsrc ();
      m_outputFile << std::setw (9) << selected;
      m_outputFile << std::setw (6) << (rx ? "RX" : "TX");
      m_outputFile << std::setw (6) << mediaMsg.GetSerializedSize ();
      if (m_includeMsgContent)
        {
          m_outputFile << "  ";
          mediaMsg.Print (m_outputFile);
        }
      else
        {
          // substr (10):  trims leading 'ns3::psc::'
          m_outputFile << std::left << "    " << headerType.GetName ().substr (10) << std::right;
        }
      m_outputFile << std::endl;
    }
}

} // namespace psc
} // namespace ns3

