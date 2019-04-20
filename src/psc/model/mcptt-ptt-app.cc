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

#include <ns3/abort.h>
#include <ns3/application.h>
#include <ns3/boolean.h>
#include <ns3/callback.h>
#include <ns3/log.h>
#include <ns3/mobility-model.h>
#include <ns3/node.h>
#include <ns3/nstime.h>
#include <ns3/object-vector.h>
#include <ns3/pointer.h>
#include <ns3/random-variable-stream.h>
#include <ns3/simulator.h>
#include <ns3/string.h>
#include <ns3/traced-callback.h>
#include <ns3/udp-socket-factory.h>
#include <ns3/uinteger.h>
#include <ns3/vector.h>

#include "mcptt-call-msg.h"
#include "mcptt-call-machine-grp-basic.h"
#include "mcptt-call-machine-grp-broadcast.h"
#include "mcptt-chan.h"
#include "mcptt-floor-machine.h"
#include "mcptt-floor-machine-basic.h"
#include "mcptt-floor-msg.h"
#include "mcptt-media-msg.h"
#include "mcptt-ptt-app.h"
#include "mcptt-pusher.h"
#include "mcptt-media-src.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("McpttPttApp");

NS_OBJECT_ENSURE_REGISTERED (McpttPttApp);

uint16_t McpttPttApp::s_portNumber = 49154;

uint16_t
McpttPttApp::GetCurrentPortNumber (void)
{
  return s_portNumber;
}

uint16_t
McpttPttApp::AllocateNextPortNumber (void)
{
  return s_portNumber++;
}

TypeId
McpttPttApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttPttApp")
    .SetParent<Application> ()
    .AddConstructor<McpttPttApp>()
    .AddAttribute ("CallPort", "The port that the application will use for call control messages.",
                   UintegerValue (49153),
                   MakeUintegerAccessor (&McpttPttApp::m_callPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Calls", "The list of all calls created during the simulation.",
                   ObjectVectorValue (),
                   MakeObjectVectorAccessor (&McpttPttApp::m_calls),
                   MakeObjectVectorChecker<McpttCall> ())
    .AddAttribute ("PeerAddress", "The Ipv4 address of the node that the peer application is on.",
                   Ipv4AddressValue (Ipv4Address ("255.255.255.255")),
                   MakeIpv4AddressAccessor (&McpttPttApp::m_peerAddress),
                   MakeIpv4AddressChecker ())
    .AddAttribute ("PushOnStart", "The flag that indicates if the pusher should be started with the application.",
                   BooleanValue (false),
                   MakeBooleanAccessor (&McpttPttApp::m_pushOnStart),
                   MakeBooleanChecker ())
    .AddTraceSource ("RxTrace", "The trace for capturing received messages",
                     MakeTraceSourceAccessor (&McpttPttApp::m_rxTrace),
                     "ns3::McpttPttApp::RxTrace")
    .AddTraceSource ("TxTrace", "The trace for capturing sent messages",
                     MakeTraceSourceAccessor (&McpttPttApp::m_txTrace),
                     "ns3::McpttPttApp::TxTrace")
   ;

    return tid;
}  

McpttPttApp::McpttPttApp (void)
  : Application (),
    m_callChan (0),
    m_floorGrantedCb (MakeNullCallback<void> ()),
    m_localAddress (Ipv4Address::GetAny ()),
    m_mediaSrc (0),
    m_newCallCb (MakeNullCallback<void, uint16_t> ()),
    m_pusher (0),
    m_selectedCall (0),
    m_selectedCallChangeCb (MakeNullCallback<void, Ptr<McpttCall> , Ptr<McpttCall> > ()),
    m_userId (0)
{  
  NS_LOG_FUNCTION (this);
}

McpttPttApp::~McpttPttApp (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttPttApp::AcceptCall (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();
  
  if (call != 0)
    {
      callMachine = call->GetCallMachine ();

      callMachine->AcceptCall ();
    }
}

void
McpttPttApp::AcceptFloorGrant (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttFloorMachine> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();
  
  if (call != 0)
    {
      floorMachine = call->GetFloorMachine ();

      floorMachine->AcceptGrant ();
    }
}

void
McpttPttApp::BeginEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttFloorMachine> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();
  
  if (call != 0)
    {
      callMachine = call->GetCallMachine ();
      floorMachine = call->GetFloorMachine ();
      floorMachine->SetPriority (7);
      callMachine->BeginEmergAlert ();
    }
}

void
McpttPttApp::CancelEmergAlert (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttFloorMachine> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();
   
  if (call != 0)
    {
      callMachine = call->GetCallMachine ();
      floorMachine = call->GetFloorMachine ();
      floorMachine->SetPriority (1);
      callMachine->CancelEmergAlert ();
    }
}

void
McpttPttApp::CreateCall (ObjectFactory& callFac, ObjectFactory& floorFac)
{
  NS_LOG_FUNCTION (this);

  std::vector<Ptr<McpttCall> > calls = GetCalls ();

  Ptr<McpttCall> call = CreateObject<McpttCall> ();
  Ptr<McpttChan> floorChan = CreateObject<McpttChan> ();
  Ptr<McpttChan> mediaChan = CreateObject<McpttChan> ();
  Ptr<McpttCallMachine> callMachine = callFac.Create<McpttCallMachine> ();
  Ptr<McpttFloorMachine> floorMachine = floorFac.Create<McpttFloorMachine> ();

  call->SetCallMachine (callMachine);
  call->SetFloorChan (floorChan);
  call->SetFloorMachine (floorMachine);
  call->SetMediaChan (mediaChan);
  call->SetOwner (this);

  call->SetRxCb (MakeCallback (&McpttPttApp::RxCb, this));
  call->SetTxCb (MakeCallback (&McpttPttApp::TxCb, this));

  calls.push_back (call);

  SetCalls (calls);
}

void
McpttPttApp::DowngradeCallType (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      callMachine = call->GetCallMachine ();

      callMachine->DowngradeCallType ();
    }
}

TypeId
McpttPttApp::GetInstanceTypeId (void) const
{
  return McpttPttApp::GetTypeId ();
}

Vector
McpttPttApp::GetNodeLoc (void) const
{
  Ptr<Node> node = GetNode ();
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  Vector current = mobility->GetPosition();

  NS_LOG_LOGIC ("McpttPttApp located at " << current << ".");
  
  return current;
}

bool
McpttPttApp::HasGrpCallFor (uint32_t grpId)
{
  NS_LOG_FUNCTION (this << grpId);

  bool found = false;
  Ptr<McpttCallMachine> callMachine = 0;
  std::vector<Ptr<McpttCall> > calls = GetCalls ();

  for (std::vector<Ptr<McpttCall> >::iterator it = calls.begin (); it != calls.end () && !found; it++)
    {
      callMachine = (*it)->GetCallMachine ();
      found = (callMachine->IsGrpCall (grpId));
    }

  return found;
}

bool
McpttPttApp::HasPrivateCallFor (uint32_t userId)
{
  NS_LOG_FUNCTION (this << userId);

  bool found = false;
  Ptr<McpttCallMachine> callMachine = 0;
  std::vector<Ptr<McpttCall> > calls = GetCalls ();

  for (std::vector<Ptr<McpttCall> >::iterator it = calls.begin (); it != calls.end () && !found; it++)
    {
      callMachine = (*it)->GetCallMachine ();
      found = (callMachine->IsPrivateCall (userId));
    }

  return found;
}

void
McpttPttApp::InitiateCall (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();
  
  if (call != 0)
    {
      callMachine = call->GetCallMachine ();

      callMachine->InitiateCall ();
    }
}


bool
McpttPttApp::IsPushed (void) const
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttPusher> pusher = GetPusher ();

  bool isPushed = pusher->IsPushing ();

  return isPushed;
}

void
McpttPttApp::Pushed (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttPusher> pusher = GetPusher ();

  if (!IsPushed ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: PttApp button pushed.");

      pusher->NotifyPushed ();
    }
}

void
McpttPttApp::RejectCall (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();
  
  if (call != 0)
    {
      callMachine = call->GetCallMachine ();

      callMachine->RejectCall ();
    }
}

void
McpttPttApp::ReleaseCall (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCall> call = GetSelectedCall ();
  Ptr<McpttCallMachine> callMachine = 0;
  
  if (call != 0)
    {
      callMachine = call->GetCallMachine ();

      callMachine->ReleaseCall ();
    }
}

void
McpttPttApp::ReleaseRequest (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttFloorMachine> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();
  
  if (call != 0)
    {
      floorMachine = call->GetFloorMachine ();

      floorMachine->ReleaseRequest ();
    }
 }

void
McpttPttApp::Released (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttPusher> pusher = GetPusher ();

  if (IsPushed ())
    {
      NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: PttApp button released.");

      pusher->NotifyReleased ();
    }
}

void
McpttPttApp::Receive (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  std::vector<Ptr<McpttCall> > calls = GetCalls ();

  for (std::vector<Ptr<McpttCall> >::iterator it = calls.begin (); it != calls.end (); it++)
    {
      (*it)->Receive (msg);
    }
}

void
McpttPttApp::SelectCall (uint32_t callIdx)
{
  NS_LOG_FUNCTION (this << callIdx);

  uint16_t oldCallId = 0;
  uint16_t newCallId = 0;
  Ptr<McpttCall> newCall = 0;
  Ptr<McpttCallMachine> oldCallMachine = 0;
  Ptr<McpttFloorMachine> oldFloorMachine = 0;
  Ptr<McpttCallMachine> newCallMachine = 0;
  Ptr<McpttFloorMachine> newFloorMachine = 0;
  Ptr<McpttCall> oldCall = GetSelectedCall ();
  std::vector<Ptr<McpttCall> > calls = GetCalls ();

  if (callIdx < calls.size ())
    {
      newCall = calls[callIdx];
    }

  if (oldCall != 0)
    {
      oldCallId = oldCall->GetCallId ();
      oldCallMachine = oldCall->GetCallMachine ();
      oldFloorMachine = oldCall->GetFloorMachine ();
      oldCallMachine->SetNewCallCb (MakeNullCallback<void, uint16_t> ());
      oldFloorMachine->SetFloorGrantedCb (MakeNullCallback<void> ());
    }

  if (newCall != 0)
    {
      newCallId = newCall->GetCallId ();
      newCallMachine = newCall->GetCallMachine ();
      newFloorMachine = newCall->GetFloorMachine ();
      newCallMachine->SetNewCallCb (MakeCallback (&McpttPttApp::NewCallCb, this));
      newFloorMachine->SetFloorGrantedCb (MakeCallback (&McpttPttApp::FloorGrantedCb, this));
    }

  SetSelectedCall (newCall);

  if (!m_selectedCallChangeCb.IsNull ())
    {
      m_selectedCallChangeCb (oldCall, newCall);
    }

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: PttApp switching from call " << (uint32_t)oldCallId << " to " << (uint32_t)newCallId << ".");
}

void
McpttPttApp::SelectLastCall (void)
{
  SelectCall (GetCalls ().size () - 1);
}

void
McpttPttApp::Send (const McpttCallMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  Ptr<McpttChan> callChan = GetCallChan ();

  Ptr<Packet> pkt = Create<Packet> ();

  pkt->AddHeader (msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: PttApp sending " << msg << ".");

  callChan->Send (pkt);
}

void
McpttPttApp::TakePushNotification (void)
{
  NS_LOG_FUNCTION (this);
  Ptr<McpttCall> call = GetSelectedCall ();
  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttFloorMachine> floorMachine = 0;
  if (call != 0)
    {
      callMachine = call->GetCallMachine ();
      floorMachine = call->GetFloorMachine ();

      Pushed ();

      if (callMachine->IsCallOngoing ())
        {
          floorMachine->TakePushNotification ();
        }
      else
        {
          InitiateCall ();
        }
    }
}

void
McpttPttApp::TakeReleaseNotification (void)
{
  NS_LOG_FUNCTION (this);

  uint32_t myUserId = GetUserId ();
  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttFloorMachine> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();
  
  if (call != 0)
    {
      callMachine = call->GetCallMachine ();
      floorMachine = call->GetFloorMachine ();

      Released ();

      floorMachine->TakeReleaseNotification ();

      //Broadcast calls should be released when the originator is done talking.
      if (callMachine->GetInstanceTypeId () == McpttCallMachineGrpBroadcast::GetTypeId ()
          && callMachine->GetCallerUserId () == myUserId)
        {
          ReleaseCall ();
        }
    }
}

bool
McpttPttApp::TakeSendReq (McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << &msg);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: PttApp taking request to send " << msg << ".");

  bool sent = false;
  Ptr<McpttFloorMachine> floorMachine = 0;
  Ptr<McpttCall> call = GetSelectedCall ();

  if (call != 0)
    {
      floorMachine = call->GetFloorMachine ();

      floorMachine->MediaReady (msg);

      if (floorMachine->HasFloor ())
        {
          sent = true;
          call->Send (msg);
        }
    }

  return sent;
}

void
McpttPttApp::UpgradeCallType (uint8_t callType)
{
  NS_LOG_FUNCTION (this << (uint32_t)callType);

  Ptr<McpttCall> call = GetSelectedCall ();
  Ptr<McpttCallMachine> callMachine = 0;
  
  if (call != 0)
    {
      callMachine = call->GetCallMachine ();

      callMachine->UpgradeCallType (callType);
    }
}

void
McpttPttApp::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("Disposing of McpttPttApp app.");

  SetCallChan (0);
  SetCalls (std::vector<Ptr<McpttCall> > ());
  SetMediaSrc (0);
  SetLocalAddress (Ipv4Address ());
  SetPusher (0);
  m_pushOnStart = false;
  SetSelectedCall (0);
  SetUserId (0);

  Object::DoDispose ();
}

void
McpttPttApp::FloorGrantedCb (void)
{
  NS_LOG_FUNCTION (this);

  if (m_floorGrantedCb.IsNull ())
    {
      AcceptFloorGrant ();
    }
  else
    {
      m_floorGrantedCb ();
    }
}

void
McpttPttApp::NewCallCb (uint16_t callId)
{
  NS_LOG_FUNCTION (this);

  if (m_newCallCb.IsNull ())
    {
      AcceptCall ();
    }
  else
    {
      m_newCallCb (callId);
    }
}

void
McpttPttApp::ReceiveCallPkt (Ptr<Packet>  pkt)
{
  NS_LOG_FUNCTION (this << pkt);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: PttApp receieved " << pkt->GetSize () << " byte(s).");

  McpttCallMsg temp;
  pkt->PeekHeader (temp);

  McpttCallMsgFieldMsgType msgType = temp.GetMsgType ();
  uint8_t code = msgType.GetType ();

  if (code == McpttCallMsgGrpProbe::CODE)
    {
      McpttCallMsgGrpProbe probeMsg;
      pkt->RemoveHeader (probeMsg);
      Receive (probeMsg);
    }
  else if (code == McpttCallMsgGrpAnnoun::CODE)
    {
      McpttCallMsgGrpAnnoun grpAnnounMsg;
      pkt->RemoveHeader (grpAnnounMsg);
      Receive (grpAnnounMsg);
    }
  else if (code == McpttCallMsgGrpAccept::CODE)
    {
      McpttCallMsgGrpAccept grpAcceptMsg;
      pkt->RemoveHeader (grpAcceptMsg);
      Receive (grpAcceptMsg);
    }
  else if (code == McpttCallMsgGrpImmPerilEnd::CODE)
    {
      McpttCallMsgGrpImmPerilEnd grpImmPerilEndMsg;
      pkt->RemoveHeader (grpImmPerilEndMsg);
      Receive (grpImmPerilEndMsg);
    }
  else if (code == McpttCallMsgGrpEmergEnd::CODE)
    {
      McpttCallMsgGrpEmergEnd grpEmergEndMsg;
      pkt->RemoveHeader (grpEmergEndMsg);
      Receive (grpEmergEndMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlert::CODE)
    {
      McpttCallMsgGrpEmergAlert grpEmergAlertMsg;
      pkt->RemoveHeader (grpEmergAlertMsg);
      Receive (grpEmergAlertMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlertAck::CODE)
    {
      McpttCallMsgGrpEmergAlertAck grpEmergAlertAckMsg;
      pkt->RemoveHeader (grpEmergAlertAckMsg);
      Receive (grpEmergAlertAckMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlertCancel::CODE)
    {
      McpttCallMsgGrpEmergAlertCancel grpEmergAlertCancelMsg;
      pkt->RemoveHeader (grpEmergAlertCancelMsg);
      Receive (grpEmergAlertCancelMsg);
    }
  else if (code == McpttCallMsgGrpEmergAlertCancelAck::CODE)
    {
      McpttCallMsgGrpEmergAlertCancelAck emergAlertCancelAckMsg;
      pkt->RemoveHeader (emergAlertCancelAckMsg);
      Receive (emergAlertCancelAckMsg);
    }
  else if (code == McpttCallMsgGrpBroadcast::CODE)
    {
      McpttCallMsgGrpBroadcast grpBroadcastMsg;
      pkt->RemoveHeader (grpBroadcastMsg);
      Receive (grpBroadcastMsg);
    }
  else if (code == McpttCallMsgGrpBroadcastEnd::CODE)
    {
      McpttCallMsgGrpBroadcastEnd grpBroadcastEndMsg;
      pkt->RemoveHeader (grpBroadcastEndMsg);
      Receive (grpBroadcastEndMsg);
    }
  else if (code == McpttCallMsgPrivateSetupReq::CODE)
    {
      McpttCallMsgPrivateSetupReq privateSetupReqMsg;
      pkt->RemoveHeader (privateSetupReqMsg);
      Receive (privateSetupReqMsg);
    }
  else if (code == McpttCallMsgPrivateRinging::CODE)
    {
      McpttCallMsgPrivateRinging privateRingingMsg;
      pkt->RemoveHeader (privateRingingMsg);
      Receive (privateRingingMsg);
    }
  else if (code == McpttCallMsgPrivateAccept::CODE)
    {
      McpttCallMsgPrivateAccept privateAcceptMsg;
      pkt->RemoveHeader (privateAcceptMsg);
      Receive (privateAcceptMsg);
    }
  else if (code == McpttCallMsgPrivateReject::CODE)
    {
      McpttCallMsgPrivateReject privateRejectMsg;
      pkt->RemoveHeader (privateRejectMsg);
      Receive (privateRejectMsg);
    }
  else if (code == McpttCallMsgPrivateRelease::CODE)
    {
      McpttCallMsgPrivateRelease privateReleaseMsg;
      pkt->RemoveHeader (privateReleaseMsg);
      Receive (privateReleaseMsg);
    }
  else if (code == McpttCallMsgPrivateReleaseAck::CODE)
    {
      McpttCallMsgPrivateReleaseAck privateReleaseAckMsg;
      pkt->RemoveHeader (privateReleaseAckMsg);
      Receive (privateReleaseAckMsg);
    }
  else if (code == McpttCallMsgPrivateAcceptAck::CODE)
    {
      McpttCallMsgPrivateAcceptAck privateAcceptAckMsg;
      pkt->RemoveHeader (privateAcceptAckMsg);
      Receive (privateAcceptAckMsg);
    }
  else if (code == McpttCallMsgPrivateEmergCancel::CODE)
    {
      McpttCallMsgPrivateEmergCancel privateEmergCancelMsg;
      pkt->RemoveHeader (privateEmergCancelMsg);
      Receive (privateEmergCancelMsg);
    }
  else if (code == McpttCallMsgPrivateEmergCancelAck::CODE)
    {
      McpttCallMsgPrivateEmergCancelAck privateEmergCancelAckMsg;
      pkt->RemoveHeader (privateEmergCancelAckMsg);
      Receive (privateEmergCancelAckMsg);
    }
  else
    {
      NS_FATAL_ERROR ("Could not resolve message code = " << (uint32_t)code << ".");
    }
}

void
McpttPttApp::RxCb (const McpttCall& call, const McpttMsg& msg)
{
  NS_LOG_FUNCTION (this << &call << &msg);

  m_rxTrace (*this, msg);
}

void
McpttPttApp::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<Node> node = GetNode ();
  Ptr<McpttCallMachine> callMachine = 0;
  Ipv4Address localAddr = GetLocalAddress ();
  Ptr<McpttPusher> pusher = GetPusher ();
  std::vector<Ptr<McpttCall> > calls = GetCalls ();
  Ptr<McpttChan> callChan = CreateObject<McpttChan> ();

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: McpttPttApp starting application.");

  callChan->SetRxPktCb (MakeCallback (&McpttPttApp::ReceiveCallPkt, this));
  callChan->Open (node, m_callPort, localAddr, m_peerAddress);

  SetCallChan (callChan);

  for (std::vector<Ptr<McpttCall> >::iterator it = calls.begin (); it != calls.end (); it++)
    {
      callMachine = (*it)->GetCallMachine ();
      callMachine->Start ();
    }

  if (m_pushOnStart == true)
    {
      pusher->Start ();
    }
}

void
McpttPttApp::StopApplication (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC (Simulator::Now ().GetSeconds () << "s: PttApp application stopping.");

  Ptr<McpttCallMachine> callMachine = 0;
  Ptr<McpttPusher> pusher = GetPusher ();
  Ptr<McpttChan> callChan = GetCallChan ();
  std::vector<Ptr<McpttCall> > calls = GetCalls ();

  pusher->Stop();

  for (std::vector<Ptr<McpttCall> >::iterator it = calls.begin (); it != calls.end (); it++)
    {
      callMachine = (*it)->GetCallMachine ();
      callMachine->Stop ();
    }

  if (callChan->IsOpen ())
    {
      callChan->Close ();
    }

  callChan->SetRxPktCb (MakeNullCallback<void, Ptr<Packet> > ());
}

void
McpttPttApp::TxCb (const McpttCall& call, const McpttMsg& msg)
{
  NS_LOG_FUNCTION (this << &call << &msg);

  m_txTrace (*this, msg);
}

void
McpttPttApp::SetLocalAddress (const Ipv4Address& localAddress)
{
  NS_LOG_FUNCTION (this << &localAddress);

  m_localAddress = localAddress;
}

Ptr<McpttChan>
McpttPttApp::GetCallChan (void) const
{
  return m_callChan;
}

std::vector<Ptr<McpttCall> >
McpttPttApp::GetCalls (void) const
{
  return m_calls;
}

Ipv4Address
McpttPttApp::GetLocalAddress (void) const
{
  return m_localAddress;
}

Ptr<McpttMediaSrc>
McpttPttApp::GetMediaSrc (void) const
{
  return m_mediaSrc;
}

Ptr<McpttPusher>
McpttPttApp::GetPusher (void) const
{
  return m_pusher;
}

Ptr<McpttCall>
McpttPttApp::GetSelectedCall (void) const
{
  return m_selectedCall;
}

uint32_t
McpttPttApp::GetUserId (void) const
{
  return m_userId;
}

void
McpttPttApp::SetCallChan (Ptr<McpttChan>  callChan)
{
  NS_LOG_FUNCTION (this << &callChan);

  m_callChan = callChan;
}

void
McpttPttApp::SetCalls (const std::vector<Ptr<McpttCall> >  calls)
{
  NS_LOG_FUNCTION (this << &calls);

  m_calls = calls;
}

void
McpttPttApp::SetFloorGrantedCb (const Callback<void>  floorGrantedCb)
{
  NS_LOG_FUNCTION (this << &floorGrantedCb);

  m_floorGrantedCb = floorGrantedCb;
}

void
McpttPttApp::SetMediaSrc (Ptr<McpttMediaSrc>  mediaSrc)
{
  NS_LOG_FUNCTION (this << mediaSrc);

  if (mediaSrc != 0)
    {
      mediaSrc->SetSink (this);
    }

  m_mediaSrc = mediaSrc;
}

void
McpttPttApp::SetNewCallCb (const Callback<void, uint16_t>  newCallCb)
{
  NS_LOG_FUNCTION (this << &newCallCb);

  m_newCallCb = newCallCb;
}

void
McpttPttApp::SetPusher (Ptr<McpttPusher>  pusher)
{
  NS_LOG_FUNCTION (this << pusher);

  if (pusher != 0)
    {
      pusher->SetPushable (this);
    }

  m_pusher = pusher;
}

void
McpttPttApp::SetSelectedCall (Ptr<McpttCall>  selectedCall)
{
  NS_LOG_FUNCTION (this);

  m_selectedCall = selectedCall;
}

void
McpttPttApp::SetSelectedCallChangeCb (const Callback<void, Ptr<McpttCall> , Ptr<McpttCall> >  selectedCallChangeCb)
{
  NS_LOG_FUNCTION (this << &selectedCallChangeCb);

  m_selectedCallChangeCb = selectedCallChangeCb;
}

void
McpttPttApp::SetUserId (uint32_t userId)
{
  NS_LOG_FUNCTION (this << userId);

  m_userId = userId;
}

} // namespace ns3

