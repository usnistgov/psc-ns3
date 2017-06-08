/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "ns3/log.h"
#include "ns3/inet-socket-address.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/nist-epc-gtpu-header.h"

#include "ns3/nist-epc-x2-header.h"
#include "ns3/nist-epc-x2.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistEpcX2");

NistX2IfaceInfo::NistX2IfaceInfo (Ipv4Address remoteIpAddr, Ptr<Socket> localCtrlPlaneSocket, Ptr<Socket> localUserPlaneSocket)
{
  m_remoteIpAddr = remoteIpAddr;
  m_localCtrlPlaneSocket = localCtrlPlaneSocket;
  m_localUserPlaneSocket = localUserPlaneSocket;
}

NistX2IfaceInfo::~NistX2IfaceInfo (void)
{
  m_localCtrlPlaneSocket = 0;
  m_localUserPlaneSocket = 0;
}

NistX2IfaceInfo& 
NistX2IfaceInfo::operator= (const NistX2IfaceInfo& value)
{
  NS_LOG_FUNCTION (this);
  m_remoteIpAddr = value.m_remoteIpAddr;
  m_localCtrlPlaneSocket = value.m_localCtrlPlaneSocket;
  m_localUserPlaneSocket = value.m_localUserPlaneSocket;
  return *this;
}

///////////////////////////////////////////

NistX2CellInfo::NistX2CellInfo (uint16_t localCellId, uint16_t remoteCellId)
{
  m_localCellId = localCellId;
  m_remoteCellId = remoteCellId;
}

NistX2CellInfo::~NistX2CellInfo (void)
{
  m_localCellId = 0;
  m_remoteCellId = 0;
}

NistX2CellInfo& 
NistX2CellInfo::operator= (const NistX2CellInfo& value)
{
  NS_LOG_FUNCTION (this);
  m_localCellId = value.m_localCellId;
  m_remoteCellId = value.m_remoteCellId;
  return *this;
}

///////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (NistEpcX2);

NistEpcX2::NistEpcX2 ()
  : m_x2cUdpPort (4444),
    m_x2uUdpPort (2152)
{
  NS_LOG_FUNCTION (this);

  m_x2SapProvider = new NistEpcX2SpecificNistEpcX2SapProvider<NistEpcX2> (this);
}

NistEpcX2::~NistEpcX2 ()
{
  NS_LOG_FUNCTION (this);
}

void
NistEpcX2::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_x2InterfaceSockets.clear ();
  m_x2InterfaceCellIds.clear ();
  delete m_x2SapProvider;
}

TypeId
NistEpcX2::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcX2")
    .SetParent<Object> ();
  return tid;
}

void
NistEpcX2::SetNistEpcX2SapUser (NistEpcX2SapUser * s)
{
  NS_LOG_FUNCTION (this << s);
  m_x2SapUser = s;
}

NistEpcX2SapProvider*
NistEpcX2::GetNistEpcX2SapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_x2SapProvider;
}


void
NistEpcX2::AddX2Interface (uint16_t localCellId, Ipv4Address localX2Address, uint16_t remoteCellId, Ipv4Address remoteX2Address)
{
  NS_LOG_FUNCTION (this << localCellId << localX2Address << remoteCellId << remoteX2Address);

  int retval;

  // Get local eNB where this X2 entity belongs to
  Ptr<Node> localEnb = GetObject<Node> ();

  // Create X2-C socket for the local eNB
  Ptr<Socket> localX2cSocket = Socket::CreateSocket (localEnb, TypeId::LookupByName ("ns3::UdpSocketFactory"));
  retval = localX2cSocket->Bind (InetSocketAddress (localX2Address, m_x2cUdpPort));
  NS_ASSERT (retval == 0);
  localX2cSocket->SetRecvCallback (MakeCallback (&NistEpcX2::RecvFromX2cSocket, this));

  // Create X2-U socket for the local eNB
  Ptr<Socket> localX2uSocket = Socket::CreateSocket (localEnb, TypeId::LookupByName ("ns3::UdpSocketFactory"));
  retval = localX2uSocket->Bind (InetSocketAddress (localX2Address, m_x2uUdpPort));
  NS_ASSERT (retval == 0);
  localX2uSocket->SetRecvCallback (MakeCallback (&NistEpcX2::RecvFromX2uSocket, this));


  NS_ASSERT_MSG (m_x2InterfaceSockets.find (remoteCellId) == m_x2InterfaceSockets.end (),
                 "Mapping for remoteCellId = " << remoteCellId << " is already known");
  m_x2InterfaceSockets [remoteCellId] = Create<NistX2IfaceInfo> (remoteX2Address, localX2cSocket, localX2uSocket);

  NS_ASSERT_MSG (m_x2InterfaceCellIds.find (localX2cSocket) == m_x2InterfaceCellIds.end (),
                 "Mapping for control plane localSocket = " << localX2cSocket << " is already known");
  m_x2InterfaceCellIds [localX2cSocket] = Create<NistX2CellInfo> (localCellId, remoteCellId);

  NS_ASSERT_MSG (m_x2InterfaceCellIds.find (localX2uSocket) == m_x2InterfaceCellIds.end (),
                 "Mapping for data plane localSocket = " << localX2uSocket << " is already known");
  m_x2InterfaceCellIds [localX2uSocket] = Create<NistX2CellInfo> (localCellId, remoteCellId);
}


void 
NistEpcX2::RecvFromX2cSocket (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  NS_LOG_LOGIC ("Recv X2 message: from Socket");
  Ptr<Packet> packet = socket->Recv ();
  NS_LOG_LOGIC ("packetLen = " << packet->GetSize ());

  NS_ASSERT_MSG (m_x2InterfaceCellIds.find (socket) != m_x2InterfaceCellIds.end (),
                 "Missing infos of local and remote CellId");
  Ptr<NistX2CellInfo> cellsInfo = m_x2InterfaceCellIds [socket];

  NistEpcX2Header x2Header;
  packet->RemoveHeader (x2Header);

  NS_LOG_LOGIC ("X2 header: " << x2Header);

  uint8_t messageType = x2Header.GetMessageType ();
  uint8_t procedureCode = x2Header.GetProcedureCode ();

  if (procedureCode == NistEpcX2Header::HandoverPreparation)
    {
      if (messageType == NistEpcX2Header::InitiatingMessage)
        {
          NS_LOG_LOGIC ("Recv X2 message: HANDOVER REQUEST");

          NistEpcX2HandoverRequestHeader x2HoReqHeader;
          packet->RemoveHeader (x2HoReqHeader);

          NS_LOG_INFO ("X2 HandoverRequest header: " << x2HoReqHeader);

          NistEpcX2SapUser::NistHandoverRequestParams params;
          params.oldEnbUeX2apId = x2HoReqHeader.GetOldEnbUeX2apId ();
          params.cause          = x2HoReqHeader.GetCause ();
          params.sourceCellId   = cellsInfo->m_remoteCellId;
          params.targetCellId   = x2HoReqHeader.GetTargetCellId ();
          params.mmeUeS1apId    = x2HoReqHeader.GetMmeUeS1apId ();
          params.ueAggregateMaxBitRateDownlink = x2HoReqHeader.GetUeAggregateMaxBitRateDownlink ();
          params.ueAggregateMaxBitRateUplink   = x2HoReqHeader.GetUeAggregateMaxBitRateUplink ();
          params.bearers        = x2HoReqHeader.GetBearers ();
          params.rrcContext     = packet;

          NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
          NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
          NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
          NS_LOG_LOGIC ("mmeUeS1apId = " << params.mmeUeS1apId);
          NS_LOG_LOGIC ("cellsInfo->m_localCellId = " << cellsInfo->m_localCellId);
          NS_ASSERT_MSG (params.targetCellId == cellsInfo->m_localCellId,
                         "TargetCellId mismatches with localCellId");

          m_x2SapUser->RecvHandoverRequest (params);
        }
      else if (messageType == NistEpcX2Header::SuccessfulOutcome)
        {
          NS_LOG_LOGIC ("Recv X2 message: HANDOVER REQUEST ACK");

          NistEpcX2HandoverRequestAckHeader x2HoReqAckHeader;
          packet->RemoveHeader (x2HoReqAckHeader);

          NS_LOG_INFO ("X2 HandoverRequestAck header: " << x2HoReqAckHeader);

          NistEpcX2SapUser::NistHandoverRequestAckParams params;          
          params.oldEnbUeX2apId = x2HoReqAckHeader.GetOldEnbUeX2apId ();
          params.newEnbUeX2apId = x2HoReqAckHeader.GetNewEnbUeX2apId ();
          params.sourceCellId   = cellsInfo->m_localCellId;
          params.targetCellId   = cellsInfo->m_remoteCellId;
          params.admittedBearers = x2HoReqAckHeader.GetAdmittedBearers ();
          params.notAdmittedBearers = x2HoReqAckHeader.GetNotAdmittedBearers ();
          params.rrcContext     = packet;

          NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
          NS_LOG_LOGIC ("newEnbUeX2apId = " << params.newEnbUeX2apId);
          NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
          NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);

          m_x2SapUser->RecvHandoverRequestAck (params);
        }
      else // messageType == NistEpcX2Header::UnsuccessfulOutcome
        {
          NS_LOG_LOGIC ("Recv X2 message: HANDOVER PREPARATION FAILURE");

          NistEpcX2HandoverPreparationFailureHeader x2HoPrepFailHeader;
          packet->RemoveHeader (x2HoPrepFailHeader);

          NS_LOG_INFO ("X2 HandoverPreparationFailure header: " << x2HoPrepFailHeader);

          NistEpcX2SapUser::NistHandoverPreparationFailureParams params;          
          params.oldEnbUeX2apId = x2HoPrepFailHeader.GetOldEnbUeX2apId ();
          params.sourceCellId   = cellsInfo->m_localCellId;
          params.targetCellId   = cellsInfo->m_remoteCellId;
          params.cause          = x2HoPrepFailHeader.GetCause ();
          params.criticalityDiagnostics = x2HoPrepFailHeader.GetCriticalityDiagnostics ();

          NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
          NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
          NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
          NS_LOG_LOGIC ("cause = " << params.cause);
          NS_LOG_LOGIC ("criticalityDiagnostics = " << params.criticalityDiagnostics);

          m_x2SapUser->RecvHandoverPreparationFailure (params);
        }
    }
  else if (procedureCode == NistEpcX2Header::LoadIndication)
    {
      if (messageType == NistEpcX2Header::InitiatingMessage)
        {
          NS_LOG_LOGIC ("Recv X2 message: LOAD INFORMATION");

          NistEpcX2LoadInformationHeader x2LoadInfoHeader;
          packet->RemoveHeader (x2LoadInfoHeader);

          NS_LOG_INFO ("X2 LoadInformation header: " << x2LoadInfoHeader);

          NistEpcX2SapUser::NistLoadInformationParams params;
          params.cellInformationList = x2LoadInfoHeader.GetCellInformationList ();

          NS_LOG_LOGIC ("cellInformationList size = " << params.cellInformationList.size ());

          m_x2SapUser->RecvLoadInformation (params);
        }
    }
  else if (procedureCode == NistEpcX2Header::SnNistStatusTransfer)
    {
      if (messageType == NistEpcX2Header::InitiatingMessage)
        {
          NS_LOG_LOGIC ("Recv X2 message: SN STATUS TRANSFER");

            NistEpcX2SnNistStatusTransferHeader x2SnNistStatusXferHeader;
            packet->RemoveHeader (x2SnNistStatusXferHeader);

            NS_LOG_INFO ("X2 SnNistStatusTransfer header: " << x2SnNistStatusXferHeader);

            NistEpcX2SapUser::NistSnNistStatusTransferParams params;
            params.oldEnbUeX2apId = x2SnNistStatusXferHeader.GetOldEnbUeX2apId ();
            params.newEnbUeX2apId = x2SnNistStatusXferHeader.GetNewEnbUeX2apId ();
            params.sourceCellId   = cellsInfo->m_remoteCellId;
            params.targetCellId   = cellsInfo->m_localCellId;
            params.erabsSubjectToNistStatusTransferList = x2SnNistStatusXferHeader.GetErabsSubjectToNistStatusTransferList ();

            NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
            NS_LOG_LOGIC ("newEnbUeX2apId = " << params.newEnbUeX2apId);
            NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
            NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
            NS_LOG_LOGIC ("erabsList size = " << params.erabsSubjectToNistStatusTransferList.size ());

            m_x2SapUser->RecvSnNistStatusTransfer (params);
        }
    }
  else if (procedureCode == NistEpcX2Header::UeContextRelease)
    {
      if (messageType == NistEpcX2Header::InitiatingMessage)
        {
          NS_LOG_LOGIC ("Recv X2 message: UE CONTEXT RELEASE");

          NistEpcX2UeContextReleaseHeader x2UeCtxReleaseHeader;
          packet->RemoveHeader (x2UeCtxReleaseHeader);

          NS_LOG_INFO ("X2 UeContextRelease header: " << x2UeCtxReleaseHeader);

          NistEpcX2SapUser::NistUeContextReleaseParams params;
          params.oldEnbUeX2apId = x2UeCtxReleaseHeader.GetOldEnbUeX2apId ();
          params.newEnbUeX2apId = x2UeCtxReleaseHeader.GetNewEnbUeX2apId ();

          NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
          NS_LOG_LOGIC ("newEnbUeX2apId = " << params.newEnbUeX2apId);

          m_x2SapUser->RecvUeContextRelease (params);
        }
    }
  else if (procedureCode == NistEpcX2Header::ResourceNistStatusReporting)
    {
      if (messageType == NistEpcX2Header::InitiatingMessage)
        {
          NS_LOG_LOGIC ("Recv X2 message: RESOURCE STATUS UPDATE");

          NistEpcX2ResourceNistStatusUpdateHeader x2ResStatUpdHeader;
          packet->RemoveHeader (x2ResStatUpdHeader);

          NS_LOG_INFO ("X2 ResourceNistStatusUpdate header: " << x2ResStatUpdHeader);

          NistEpcX2SapUser::NistResourceNistStatusUpdateParams params;
          params.enb1MeasurementId = x2ResStatUpdHeader.GetEnb1MeasurementId ();
          params.enb2MeasurementId = x2ResStatUpdHeader.GetEnb2MeasurementId ();
          params.cellMeasurementResultList = x2ResStatUpdHeader.GetCellMeasurementResultList ();

          NS_LOG_LOGIC ("enb1MeasurementId = " << params.enb1MeasurementId);
          NS_LOG_LOGIC ("enb2MeasurementId = " << params.enb2MeasurementId);
          NS_LOG_LOGIC ("cellMeasurementResultList size = " << params.cellMeasurementResultList.size ());

          m_x2SapUser->RecvResourceNistStatusUpdate (params);
        }
    }
  else
    {
      NS_ASSERT_MSG (false, "ProcedureCode NOT SUPPORTED!!!");
    }
}


void 
NistEpcX2::RecvFromX2uSocket (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  NS_LOG_LOGIC ("Recv UE DATA through X2-U interface from Socket");
  Ptr<Packet> packet = socket->Recv ();
  NS_LOG_LOGIC ("packetLen = " << packet->GetSize ());

  NS_ASSERT_MSG (m_x2InterfaceCellIds.find (socket) != m_x2InterfaceCellIds.end (),
                 "Missing infos of local and remote CellId");
  Ptr<NistX2CellInfo> cellsInfo = m_x2InterfaceCellIds [socket];

  NistGtpuHeader gtpu;
  packet->RemoveHeader (gtpu);

  NS_LOG_LOGIC ("GTP-U header: " << gtpu);

  NistEpcX2SapUser::NistUeDataParams params;
  params.sourceCellId = cellsInfo->m_remoteCellId;
  params.targetCellId = cellsInfo->m_localCellId;
  params.gtpTeid = gtpu.GetTeid ();
  params.ueData = packet;

  m_x2SapUser->RecvUeData (params);
}


//
// Implementation of the X2 SAP Provider
//
void
NistEpcX2::DoSendHandoverRequest (NistEpcX2SapProvider::NistHandoverRequestParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
  NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
  NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
  NS_LOG_LOGIC ("mmeUeS1apId  = " << params.mmeUeS1apId);

  NS_ASSERT_MSG (m_x2InterfaceSockets.find (params.targetCellId) != m_x2InterfaceSockets.end (),
                 "Missing infos for targetCellId = " << params.targetCellId);
  Ptr<NistX2IfaceInfo> socketInfo = m_x2InterfaceSockets [params.targetCellId];
  Ptr<Socket> sourceSocket = socketInfo->m_localCtrlPlaneSocket;
  Ipv4Address targetIpAddr = socketInfo->m_remoteIpAddr;

  NS_LOG_LOGIC ("sourceSocket = " << sourceSocket);
  NS_LOG_LOGIC ("targetIpAddr = " << targetIpAddr);

  NS_LOG_INFO ("Send X2 message: HANDOVER REQUEST");

  // Build the X2 message
  NistEpcX2HandoverRequestHeader x2HoReqHeader;
  x2HoReqHeader.SetOldEnbUeX2apId (params.oldEnbUeX2apId);
  x2HoReqHeader.SetCause (params.cause);
  x2HoReqHeader.SetTargetCellId (params.targetCellId);
  x2HoReqHeader.SetMmeUeS1apId (params.mmeUeS1apId);
  x2HoReqHeader.SetUeAggregateMaxBitRateDownlink (params.ueAggregateMaxBitRateDownlink);
  x2HoReqHeader.SetUeAggregateMaxBitRateUplink (params.ueAggregateMaxBitRateUplink);
  x2HoReqHeader.SetBearers (params.bearers);

  NistEpcX2Header x2Header;
  x2Header.SetMessageType (NistEpcX2Header::InitiatingMessage);
  x2Header.SetProcedureCode (NistEpcX2Header::HandoverPreparation);
  x2Header.SetLengthOfIes (x2HoReqHeader.GetLengthOfIes ());
  x2Header.SetNumberOfIes (x2HoReqHeader.GetNumberOfIes ());

  NS_LOG_INFO ("X2 header: " << x2Header);
  NS_LOG_INFO ("X2 HandoverRequest header: " << x2HoReqHeader);

  // Build the X2 packet
  Ptr<Packet> packet = (params.rrcContext != 0) ? (params.rrcContext) : (Create <Packet> ());
  packet->AddHeader (x2HoReqHeader);
  packet->AddHeader (x2Header);
  NS_LOG_INFO ("packetLen = " << packet->GetSize ());

  // Send the X2 message through the socket
  sourceSocket->SendTo (packet, 0, InetSocketAddress (targetIpAddr, m_x2cUdpPort));
}


void
NistEpcX2::DoSendHandoverRequestAck (NistEpcX2SapProvider::NistHandoverRequestAckParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
  NS_LOG_LOGIC ("newEnbUeX2apId = " << params.newEnbUeX2apId);
  NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
  NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);

  NS_ASSERT_MSG (m_x2InterfaceSockets.find (params.sourceCellId) != m_x2InterfaceSockets.end (),
                 "Socket infos not defined for sourceCellId = " << params.sourceCellId);

  Ptr<Socket> localSocket = m_x2InterfaceSockets [params.sourceCellId]->m_localCtrlPlaneSocket;
  Ipv4Address remoteIpAddr = m_x2InterfaceSockets [params.sourceCellId]->m_remoteIpAddr;

  NS_LOG_LOGIC ("localSocket = " << localSocket);
  NS_LOG_LOGIC ("remoteIpAddr = " << remoteIpAddr);

  NS_LOG_INFO ("Send X2 message: HANDOVER REQUEST ACK");

  // Build the X2 message
  NistEpcX2HandoverRequestAckHeader x2HoAckHeader;
  x2HoAckHeader.SetOldEnbUeX2apId (params.oldEnbUeX2apId);
  x2HoAckHeader.SetNewEnbUeX2apId (params.newEnbUeX2apId);
  x2HoAckHeader.SetAdmittedBearers (params.admittedBearers);
  x2HoAckHeader.SetNotAdmittedBearers (params.notAdmittedBearers);

  NistEpcX2Header x2Header;
  x2Header.SetMessageType (NistEpcX2Header::SuccessfulOutcome);
  x2Header.SetProcedureCode (NistEpcX2Header::HandoverPreparation);
  x2Header.SetLengthOfIes (x2HoAckHeader.GetLengthOfIes ());
  x2Header.SetNumberOfIes (x2HoAckHeader.GetNumberOfIes ());

  NS_LOG_INFO ("X2 header: " << x2Header);
  NS_LOG_INFO ("X2 HandoverAck header: " << x2HoAckHeader);
  NS_LOG_INFO ("RRC context: " << params.rrcContext);

  // Build the X2 packet
  Ptr<Packet> packet = (params.rrcContext != 0) ? (params.rrcContext) : (Create <Packet> ());
  packet->AddHeader (x2HoAckHeader);
  packet->AddHeader (x2Header);
  NS_LOG_INFO ("packetLen = " << packet->GetSize ());

  // Send the X2 message through the socket
  localSocket->SendTo (packet, 0, InetSocketAddress (remoteIpAddr, m_x2cUdpPort));
}


void
NistEpcX2::DoSendHandoverPreparationFailure (NistEpcX2SapProvider::NistHandoverPreparationFailureParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
  NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
  NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
  NS_LOG_LOGIC ("cause = " << params.cause);
  NS_LOG_LOGIC ("criticalityDiagnostics = " << params.criticalityDiagnostics);

  NS_ASSERT_MSG (m_x2InterfaceSockets.find (params.sourceCellId) != m_x2InterfaceSockets.end (),
                 "Socket infos not defined for sourceCellId = " << params.sourceCellId);

  Ptr<Socket> localSocket = m_x2InterfaceSockets [params.sourceCellId]->m_localCtrlPlaneSocket;
  Ipv4Address remoteIpAddr = m_x2InterfaceSockets [params.sourceCellId]->m_remoteIpAddr;

  NS_LOG_LOGIC ("localSocket = " << localSocket);
  NS_LOG_LOGIC ("remoteIpAddr = " << remoteIpAddr);

  NS_LOG_INFO ("Send X2 message: HANDOVER PREPARATION FAILURE");

  // Build the X2 message
  NistEpcX2HandoverPreparationFailureHeader x2HoPrepFailHeader;
  x2HoPrepFailHeader.SetOldEnbUeX2apId (params.oldEnbUeX2apId);
  x2HoPrepFailHeader.SetCause (params.cause);
  x2HoPrepFailHeader.SetCriticalityDiagnostics (params.criticalityDiagnostics);

  NistEpcX2Header x2Header;
  x2Header.SetMessageType (NistEpcX2Header::UnsuccessfulOutcome);
  x2Header.SetProcedureCode (NistEpcX2Header::HandoverPreparation);
  x2Header.SetLengthOfIes (x2HoPrepFailHeader.GetLengthOfIes ());
  x2Header.SetNumberOfIes (x2HoPrepFailHeader.GetNumberOfIes ());

  NS_LOG_INFO ("X2 header: " << x2Header);
  NS_LOG_INFO ("X2 HandoverPrepFail header: " << x2HoPrepFailHeader);

  // Build the X2 packet
  Ptr<Packet> packet = Create <Packet> ();
  packet->AddHeader (x2HoPrepFailHeader);
  packet->AddHeader (x2Header);
  NS_LOG_INFO ("packetLen = " << packet->GetSize ());

  // Send the X2 message through the socket
  localSocket->SendTo (packet, 0, InetSocketAddress (remoteIpAddr, m_x2cUdpPort));
}


void
NistEpcX2::DoSendSnNistStatusTransfer (NistEpcX2SapProvider::NistSnNistStatusTransferParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
  NS_LOG_LOGIC ("newEnbUeX2apId = " << params.newEnbUeX2apId);
  NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
  NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
  NS_LOG_LOGIC ("erabsList size = " << params.erabsSubjectToNistStatusTransferList.size ());

  NS_ASSERT_MSG (m_x2InterfaceSockets.find (params.targetCellId) != m_x2InterfaceSockets.end (),
                 "Socket infos not defined for targetCellId = " << params.targetCellId);

  Ptr<Socket> localSocket = m_x2InterfaceSockets [params.targetCellId]->m_localCtrlPlaneSocket;
  Ipv4Address remoteIpAddr = m_x2InterfaceSockets [params.targetCellId]->m_remoteIpAddr;

  NS_LOG_LOGIC ("localSocket = " << localSocket);
  NS_LOG_LOGIC ("remoteIpAddr = " << remoteIpAddr);

  NS_LOG_INFO ("Send X2 message: SN STATUS TRANSFER");

  // Build the X2 message
  NistEpcX2SnNistStatusTransferHeader x2SnNistStatusXferHeader;
  x2SnNistStatusXferHeader.SetOldEnbUeX2apId (params.oldEnbUeX2apId);
  x2SnNistStatusXferHeader.SetNewEnbUeX2apId (params.newEnbUeX2apId);
  x2SnNistStatusXferHeader.SetErabsSubjectToNistStatusTransferList (params.erabsSubjectToNistStatusTransferList);

  NistEpcX2Header x2Header;
  x2Header.SetMessageType (NistEpcX2Header::InitiatingMessage);
  x2Header.SetProcedureCode (NistEpcX2Header::SnNistStatusTransfer);
  x2Header.SetLengthOfIes (x2SnNistStatusXferHeader.GetLengthOfIes ());
  x2Header.SetNumberOfIes (x2SnNistStatusXferHeader.GetNumberOfIes ());

  NS_LOG_INFO ("X2 header: " << x2Header);
  NS_LOG_INFO ("X2 SnNistStatusTransfer header: " << x2SnNistStatusXferHeader);

  // Build the X2 packet
  Ptr<Packet> packet = Create <Packet> ();
  packet->AddHeader (x2SnNistStatusXferHeader);
  packet->AddHeader (x2Header);
  NS_LOG_INFO ("packetLen = " << packet->GetSize ());

  // Send the X2 message through the socket
  localSocket->SendTo (packet, 0, InetSocketAddress (remoteIpAddr, m_x2cUdpPort));
}


void
NistEpcX2::DoSendUeContextRelease (NistEpcX2SapProvider::NistUeContextReleaseParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("oldEnbUeX2apId = " << params.oldEnbUeX2apId);
  NS_LOG_LOGIC ("newEnbUeX2apId = " << params.newEnbUeX2apId);
  NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);

  NS_ASSERT_MSG (m_x2InterfaceSockets.find (params.sourceCellId) != m_x2InterfaceSockets.end (),
                 "Socket infos not defined for sourceCellId = " << params.sourceCellId);

  Ptr<Socket> localSocket = m_x2InterfaceSockets [params.sourceCellId]->m_localCtrlPlaneSocket;
  Ipv4Address remoteIpAddr = m_x2InterfaceSockets [params.sourceCellId]->m_remoteIpAddr;

  NS_LOG_LOGIC ("localSocket = " << localSocket);
  NS_LOG_LOGIC ("remoteIpAddr = " << remoteIpAddr);

  NS_LOG_INFO ("Send X2 message: UE CONTEXT RELEASE");

  // Build the X2 message
  NistEpcX2UeContextReleaseHeader x2UeCtxReleaseHeader;
  x2UeCtxReleaseHeader.SetOldEnbUeX2apId (params.oldEnbUeX2apId);
  x2UeCtxReleaseHeader.SetNewEnbUeX2apId (params.newEnbUeX2apId);

  NistEpcX2Header x2Header;
  x2Header.SetMessageType (NistEpcX2Header::InitiatingMessage);
  x2Header.SetProcedureCode (NistEpcX2Header::UeContextRelease);
  x2Header.SetLengthOfIes (x2UeCtxReleaseHeader.GetLengthOfIes ());
  x2Header.SetNumberOfIes (x2UeCtxReleaseHeader.GetNumberOfIes ());

  NS_LOG_INFO ("X2 header: " << x2Header);
  NS_LOG_INFO ("X2 UeContextRelease header: " << x2UeCtxReleaseHeader);

  // Build the X2 packet
  Ptr<Packet> packet = Create <Packet> ();
  packet->AddHeader (x2UeCtxReleaseHeader);
  packet->AddHeader (x2Header);
  NS_LOG_INFO ("packetLen = " << packet->GetSize ());

  // Send the X2 message through the socket
  localSocket->SendTo (packet, 0, InetSocketAddress (remoteIpAddr, m_x2cUdpPort));
}


void
NistEpcX2::DoSendLoadInformation (NistEpcX2SapProvider::NistLoadInformationParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
  NS_LOG_LOGIC ("cellInformationList size = " << params.cellInformationList.size ());

  NS_ASSERT_MSG (m_x2InterfaceSockets.find (params.targetCellId) != m_x2InterfaceSockets.end (),
                 "Missing infos for targetCellId = " << params.targetCellId);
  Ptr<NistX2IfaceInfo> socketInfo = m_x2InterfaceSockets [params.targetCellId];
  Ptr<Socket> sourceSocket = socketInfo->m_localCtrlPlaneSocket;
  Ipv4Address targetIpAddr = socketInfo->m_remoteIpAddr;

  NS_LOG_LOGIC ("sourceSocket = " << sourceSocket);
  NS_LOG_LOGIC ("targetIpAddr = " << targetIpAddr);

  NS_LOG_INFO ("Send X2 message: LOAD INFORMATION");

  // Build the X2 message
  NistEpcX2LoadInformationHeader x2LoadInfoHeader;
  x2LoadInfoHeader.SetCellInformationList (params.cellInformationList);

  NistEpcX2Header x2Header;
  x2Header.SetMessageType (NistEpcX2Header::InitiatingMessage);
  x2Header.SetProcedureCode (NistEpcX2Header::LoadIndication);
  x2Header.SetLengthOfIes (x2LoadInfoHeader.GetLengthOfIes ());
  x2Header.SetNumberOfIes (x2LoadInfoHeader.GetNumberOfIes ());

  NS_LOG_INFO ("X2 header: " << x2Header);
  NS_LOG_INFO ("X2 LoadInformation header: " << x2LoadInfoHeader);

  // Build the X2 packet
  Ptr<Packet> packet = Create <Packet> ();
  packet->AddHeader (x2LoadInfoHeader);
  packet->AddHeader (x2Header);
  NS_LOG_INFO ("packetLen = " << packet->GetSize ());

  // Send the X2 message through the socket
  sourceSocket->SendTo (packet, 0, InetSocketAddress (targetIpAddr, m_x2cUdpPort));

}


void
NistEpcX2::DoSendResourceNistStatusUpdate (NistEpcX2SapProvider::NistResourceNistStatusUpdateParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
  NS_LOG_LOGIC ("enb1MeasurementId = " << params.enb1MeasurementId);
  NS_LOG_LOGIC ("enb2MeasurementId = " << params.enb2MeasurementId);
  NS_LOG_LOGIC ("cellMeasurementResultList size = " << params.cellMeasurementResultList.size ());

  NS_ASSERT_MSG (m_x2InterfaceSockets.find (params.targetCellId) != m_x2InterfaceSockets.end (),
                 "Missing infos for targetCellId = " << params.targetCellId);
  Ptr<NistX2IfaceInfo> socketInfo = m_x2InterfaceSockets [params.targetCellId];
  Ptr<Socket> sourceSocket = socketInfo->m_localCtrlPlaneSocket;
  Ipv4Address targetIpAddr = socketInfo->m_remoteIpAddr;

  NS_LOG_LOGIC ("sourceSocket = " << sourceSocket);
  NS_LOG_LOGIC ("targetIpAddr = " << targetIpAddr);

  NS_LOG_INFO ("Send X2 message: RESOURCE STATUS UPDATE");

  // Build the X2 message
  NistEpcX2ResourceNistStatusUpdateHeader x2ResourceStatUpdHeader;
  x2ResourceStatUpdHeader.SetEnb1MeasurementId (params.enb1MeasurementId);
  x2ResourceStatUpdHeader.SetEnb2MeasurementId (params.enb2MeasurementId);
  x2ResourceStatUpdHeader.SetCellMeasurementResultList (params.cellMeasurementResultList);

  NistEpcX2Header x2Header;
  x2Header.SetMessageType (NistEpcX2Header::InitiatingMessage);
  x2Header.SetProcedureCode (NistEpcX2Header::ResourceNistStatusReporting);
  x2Header.SetLengthOfIes (x2ResourceStatUpdHeader.GetLengthOfIes ());
  x2Header.SetNumberOfIes (x2ResourceStatUpdHeader.GetNumberOfIes ());

  NS_LOG_INFO ("X2 header: " << x2Header);
  NS_LOG_INFO ("X2 ResourceNistStatusUpdate header: " << x2ResourceStatUpdHeader);

  // Build the X2 packet
  Ptr<Packet> packet = Create <Packet> ();
  packet->AddHeader (x2ResourceStatUpdHeader);
  packet->AddHeader (x2Header);
  NS_LOG_INFO ("packetLen = " << packet->GetSize ());

  // Send the X2 message through the socket
  sourceSocket->SendTo (packet, 0, InetSocketAddress (targetIpAddr, m_x2cUdpPort));

}


void
NistEpcX2::DoSendUeData (NistEpcX2SapProvider::NistUeDataParams params)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_LOGIC ("sourceCellId = " << params.sourceCellId);
  NS_LOG_LOGIC ("targetCellId = " << params.targetCellId);
  NS_LOG_LOGIC ("gtpTeid = " << params.gtpTeid);

  NS_ASSERT_MSG (m_x2InterfaceSockets.find (params.targetCellId) != m_x2InterfaceSockets.end (),
                 "Missing infos for targetCellId = " << params.targetCellId);
  Ptr<NistX2IfaceInfo> socketInfo = m_x2InterfaceSockets [params.targetCellId];
  Ptr<Socket> sourceSocket = socketInfo->m_localUserPlaneSocket;
  Ipv4Address targetIpAddr = socketInfo->m_remoteIpAddr;

  NS_LOG_LOGIC ("sourceSocket = " << sourceSocket);
  NS_LOG_LOGIC ("targetIpAddr = " << targetIpAddr);

  NistGtpuHeader gtpu;
  gtpu.SetTeid (params.gtpTeid);
  gtpu.SetLength (params.ueData->GetSize () + gtpu.GetSerializedSize () - 8); /// \todo This should be done in NistGtpuHeader
  NS_LOG_INFO ("GTP-U header: " << gtpu);

  Ptr<Packet> packet = params.ueData;
  packet->AddHeader (gtpu);

  NS_LOG_INFO ("Forward UE DATA through X2 interface");
  sourceSocket->SendTo (packet, 0, InetSocketAddress (targetIpAddr, m_x2uUdpPort));
}

} // namespace ns3
