// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#include "nr-epc-x2.h"

#include "nr-epc-gtpu-header.h"
#include "nr-epc-x2-header.h"

#include "ns3/inet-socket-address.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/packet.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEpcX2");

NrX2IfaceInfo::NrX2IfaceInfo(Ipv4Address remoteIpAddr,
                             Ptr<Socket> localCtrlPlaneSocket,
                             Ptr<Socket> localUserPlaneSocket)
{
    m_remoteIpAddr = remoteIpAddr;
    m_localCtrlPlaneSocket = localCtrlPlaneSocket;
    m_localUserPlaneSocket = localUserPlaneSocket;
}

NrX2IfaceInfo::~NrX2IfaceInfo()
{
    m_localCtrlPlaneSocket = nullptr;
    m_localUserPlaneSocket = nullptr;
}

NrX2IfaceInfo&
NrX2IfaceInfo::operator=(const NrX2IfaceInfo& value)
{
    NS_LOG_FUNCTION(this);
    m_remoteIpAddr = value.m_remoteIpAddr;
    m_localCtrlPlaneSocket = value.m_localCtrlPlaneSocket;
    m_localUserPlaneSocket = value.m_localUserPlaneSocket;
    return *this;
}

///////////////////////////////////////////

NrX2CellInfo::NrX2CellInfo(std::vector<uint16_t> localCellIds, std::vector<uint16_t> remoteCellIds)
    : m_localCellIds{localCellIds},
      m_remoteCellIds{remoteCellIds}
{
}

NrX2CellInfo::~NrX2CellInfo()
{
}

NrX2CellInfo&
NrX2CellInfo::operator=(const NrX2CellInfo& value)
{
    NS_LOG_FUNCTION(this);
    m_localCellIds = value.m_localCellIds;
    m_remoteCellIds = value.m_remoteCellIds;
    return *this;
}

///////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(NrEpcX2);

NrEpcX2::NrEpcX2()
    : m_x2cUdpPort(4444),
      m_x2uUdpPort(2152)
{
    NS_LOG_FUNCTION(this);

    m_x2SapProvider = new NrEpcX2SpecificEpcX2SapProvider<NrEpcX2>(this);
}

NrEpcX2::~NrEpcX2()
{
    NS_LOG_FUNCTION(this);
}

void
NrEpcX2::DoDispose()
{
    NS_LOG_FUNCTION(this);

    m_x2InterfaceSockets.clear();
    m_x2InterfaceCellIds.clear();
    delete m_x2SapProvider;
}

TypeId
NrEpcX2::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcX2").SetParent<Object>().SetGroupName("Nr");
    return tid;
}

void
NrEpcX2::SetEpcX2SapUser(NrEpcX2SapUser* s)
{
    NS_LOG_FUNCTION(this << s);
    m_x2SapUser = s;
}

NrEpcX2SapProvider*
NrEpcX2::GetEpcX2SapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_x2SapProvider;
}

void
NrEpcX2::AddX2Interface(uint16_t localCellId,
                        Ipv4Address localX2Address,
                        std::vector<uint16_t> remoteCellIds,
                        Ipv4Address remoteX2Address)
{
    uint16_t remoteCellId = remoteCellIds.at(0);
    NS_LOG_FUNCTION(this << localCellId << localX2Address << remoteCellId << remoteX2Address);

    int retval;

    // Get local gNB where this X2 entity belongs to
    Ptr<Node> localGnb = GetObject<Node>();

    // Create X2-C socket for the local gNB
    Ptr<Socket> localX2cSocket =
        Socket::CreateSocket(localGnb, TypeId::LookupByName("ns3::UdpSocketFactory"));
    retval = localX2cSocket->Bind(InetSocketAddress(localX2Address, m_x2cUdpPort));
    NS_ASSERT(retval == 0);
    localX2cSocket->SetRecvCallback(MakeCallback(&NrEpcX2::RecvFromX2cSocket, this));

    // Create X2-U socket for the local gNB
    Ptr<Socket> localX2uSocket =
        Socket::CreateSocket(localGnb, TypeId::LookupByName("ns3::UdpSocketFactory"));
    retval = localX2uSocket->Bind(InetSocketAddress(localX2Address, m_x2uUdpPort));
    NS_ASSERT(retval == 0);
    localX2uSocket->SetRecvCallback(MakeCallback(&NrEpcX2::RecvFromX2uSocket, this));

    std::vector<uint16_t> localCellIds;
    localCellIds.push_back(localCellId);

    NS_ASSERT_MSG(m_x2InterfaceSockets.find(remoteCellId) == m_x2InterfaceSockets.end(),
                  "Mapping for remoteCellId = " << remoteCellId << " is already known");
    for (uint16_t remoteCellId : remoteCellIds)
    {
        m_x2InterfaceSockets[remoteCellId] =
            Create<NrX2IfaceInfo>(remoteX2Address, localX2cSocket, localX2uSocket);
    }

    NS_ASSERT_MSG(m_x2InterfaceCellIds.find(localX2cSocket) == m_x2InterfaceCellIds.end(),
                  "Mapping for control plane localSocket = " << localX2cSocket
                                                             << " is already known");
    m_x2InterfaceCellIds[localX2cSocket] = Create<NrX2CellInfo>(localCellIds, remoteCellIds);

    NS_ASSERT_MSG(m_x2InterfaceCellIds.find(localX2uSocket) == m_x2InterfaceCellIds.end(),
                  "Mapping for data plane localSocket = " << localX2uSocket << " is already known");
    m_x2InterfaceCellIds[localX2uSocket] = Create<NrX2CellInfo>(localCellIds, remoteCellIds);
}

void
NrEpcX2::RecvFromX2cSocket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);

    NS_LOG_LOGIC("Recv X2 message: from Socket");
    Ptr<Packet> packet = socket->Recv();
    NS_LOG_LOGIC("packetLen = " << packet->GetSize());

    NS_ASSERT_MSG(m_x2InterfaceCellIds.find(socket) != m_x2InterfaceCellIds.end(),
                  "Missing infos of local and remote CellId");
    Ptr<NrX2CellInfo> cellsInfo = m_x2InterfaceCellIds[socket];

    NrEpcX2Header x2Header;
    packet->RemoveHeader(x2Header);

    NS_LOG_LOGIC("X2 header: " << x2Header);

    uint8_t messageType = x2Header.GetMessageType();
    uint8_t procedureCode = x2Header.GetProcedureCode();

    if (procedureCode == NrEpcX2Header::HandoverPreparation)
    {
        if (messageType == NrEpcX2Header::InitiatingMessage)
        {
            NS_LOG_LOGIC("Recv X2 message: HANDOVER REQUEST");

            NrEpcX2HandoverRequestHeader x2HoReqHeader;
            packet->RemoveHeader(x2HoReqHeader);

            NS_LOG_INFO("X2 HandoverRequest header: " << x2HoReqHeader);

            NrEpcX2SapUser::HandoverRequestParams params;
            params.oldGnbUeX2apId = x2HoReqHeader.GetOldGnbUeX2apId();
            params.cause = x2HoReqHeader.GetCause();
            params.sourceCellId = cellsInfo->m_remoteCellIds.at(0);
            params.targetCellId = x2HoReqHeader.GetTargetCellId();
            params.mmeUeS1apId = x2HoReqHeader.GetMmeUeS1apId();
            params.ueAggregateMaxBitRateDownlink = x2HoReqHeader.GetUeAggregateMaxBitRateDownlink();
            params.ueAggregateMaxBitRateUplink = x2HoReqHeader.GetUeAggregateMaxBitRateUplink();
            params.bearers = x2HoReqHeader.GetBearers();
            params.rrcContext = packet;

            NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
            NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);
            NS_LOG_LOGIC("targetCellId = " << params.targetCellId);
            NS_LOG_LOGIC("mmeUeS1apId = " << params.mmeUeS1apId);
            NS_LOG_LOGIC("cellsInfo->m_localCellId = " << cellsInfo->m_localCellIds.at(0));

            m_x2SapUser->RecvHandoverRequest(params);
        }
        else if (messageType == NrEpcX2Header::SuccessfulOutcome)
        {
            NS_LOG_LOGIC("Recv X2 message: HANDOVER REQUEST ACK");

            NrEpcX2HandoverRequestAckHeader x2HoReqAckHeader;
            packet->RemoveHeader(x2HoReqAckHeader);

            NS_LOG_INFO("X2 HandoverRequestAck header: " << x2HoReqAckHeader);

            NrEpcX2SapUser::HandoverRequestAckParams params;
            params.oldGnbUeX2apId = x2HoReqAckHeader.GetOldGnbUeX2apId();
            params.newGnbUeX2apId = x2HoReqAckHeader.GetNewGnbUeX2apId();
            params.sourceCellId = cellsInfo->m_localCellIds.at(0);
            params.targetCellId = cellsInfo->m_remoteCellIds.at(0);
            params.admittedBearers = x2HoReqAckHeader.GetAdmittedBearers();
            params.notAdmittedBearers = x2HoReqAckHeader.GetNotAdmittedBearers();
            params.rrcContext = packet;

            NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
            NS_LOG_LOGIC("newGnbUeX2apId = " << params.newGnbUeX2apId);
            NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);
            NS_LOG_LOGIC("targetCellId = " << params.targetCellId);

            m_x2SapUser->RecvHandoverRequestAck(params);
        }
        else // messageType == NrEpcX2Header::UnsuccessfulOutcome
        {
            NS_LOG_LOGIC("Recv X2 message: HANDOVER PREPARATION FAILURE");

            NrEpcX2HandoverPreparationFailureHeader x2HoPrepFailHeader;
            packet->RemoveHeader(x2HoPrepFailHeader);

            NS_LOG_INFO("X2 HandoverPreparationFailure header: " << x2HoPrepFailHeader);

            NrEpcX2SapUser::HandoverPreparationFailureParams params;
            params.oldGnbUeX2apId = x2HoPrepFailHeader.GetOldGnbUeX2apId();
            params.sourceCellId = cellsInfo->m_localCellIds.at(0);
            params.targetCellId = cellsInfo->m_remoteCellIds.at(0);
            params.cause = x2HoPrepFailHeader.GetCause();
            params.criticalityDiagnostics = x2HoPrepFailHeader.GetCriticalityDiagnostics();

            NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
            NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);
            NS_LOG_LOGIC("targetCellId = " << params.targetCellId);
            NS_LOG_LOGIC("cause = " << params.cause);
            NS_LOG_LOGIC("criticalityDiagnostics = " << params.criticalityDiagnostics);

            m_x2SapUser->RecvHandoverPreparationFailure(params);
        }
    }
    else if (procedureCode == NrEpcX2Header::LoadIndication)
    {
        if (messageType == NrEpcX2Header::InitiatingMessage)
        {
            NS_LOG_LOGIC("Recv X2 message: LOAD INFORMATION");

            NrEpcX2LoadInformationHeader x2LoadInfoHeader;
            packet->RemoveHeader(x2LoadInfoHeader);

            NS_LOG_INFO("X2 LoadInformation header: " << x2LoadInfoHeader);

            NrEpcX2SapUser::LoadInformationParams params;
            params.cellInformationList = x2LoadInfoHeader.GetCellInformationList();

            NS_LOG_LOGIC("cellInformationList size = " << params.cellInformationList.size());

            m_x2SapUser->RecvLoadInformation(params);
        }
    }
    else if (procedureCode == NrEpcX2Header::SnStatusTransfer)
    {
        if (messageType == NrEpcX2Header::InitiatingMessage)
        {
            NS_LOG_LOGIC("Recv X2 message: SN STATUS TRANSFER");

            NrEpcX2SnStatusTransferHeader x2SnStatusXferHeader;
            packet->RemoveHeader(x2SnStatusXferHeader);

            NS_LOG_INFO("X2 SnStatusTransfer header: " << x2SnStatusXferHeader);

            NrEpcX2SapUser::SnStatusTransferParams params;
            params.oldGnbUeX2apId = x2SnStatusXferHeader.GetOldGnbUeX2apId();
            params.newGnbUeX2apId = x2SnStatusXferHeader.GetNewGnbUeX2apId();
            params.sourceCellId = cellsInfo->m_remoteCellIds.at(0);
            params.targetCellId = cellsInfo->m_localCellIds.at(0);
            params.erabsSubjectToStatusTransferList =
                x2SnStatusXferHeader.GetErabsSubjectToStatusTransferList();

            NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
            NS_LOG_LOGIC("newGnbUeX2apId = " << params.newGnbUeX2apId);
            NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);
            NS_LOG_LOGIC("targetCellId = " << params.targetCellId);
            NS_LOG_LOGIC("erabsList size = " << params.erabsSubjectToStatusTransferList.size());

            m_x2SapUser->RecvSnStatusTransfer(params);
        }
    }
    else if (procedureCode == NrEpcX2Header::UeContextRelease)
    {
        if (messageType == NrEpcX2Header::InitiatingMessage)
        {
            NS_LOG_LOGIC("Recv X2 message: UE CONTEXT RELEASE");

            NrEpcX2UeContextReleaseHeader x2UeCtxReleaseHeader;
            packet->RemoveHeader(x2UeCtxReleaseHeader);

            NS_LOG_INFO("X2 UeContextRelease header: " << x2UeCtxReleaseHeader);

            NrEpcX2SapUser::UeContextReleaseParams params;
            params.oldGnbUeX2apId = x2UeCtxReleaseHeader.GetOldGnbUeX2apId();
            params.newGnbUeX2apId = x2UeCtxReleaseHeader.GetNewGnbUeX2apId();

            NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
            NS_LOG_LOGIC("newGnbUeX2apId = " << params.newGnbUeX2apId);

            m_x2SapUser->RecvUeContextRelease(params);
        }
    }
    else if (procedureCode == NrEpcX2Header::ResourceStatusReporting)
    {
        if (messageType == NrEpcX2Header::InitiatingMessage)
        {
            NS_LOG_LOGIC("Recv X2 message: RESOURCE STATUS UPDATE");

            NrEpcX2ResourceStatusUpdateHeader x2ResStatUpdHeader;
            packet->RemoveHeader(x2ResStatUpdHeader);

            NS_LOG_INFO("X2 ResourceStatusUpdate header: " << x2ResStatUpdHeader);

            NrEpcX2SapUser::ResourceStatusUpdateParams params;
            params.targetCellId = 0;
            params.gnb1MeasurementId = x2ResStatUpdHeader.GetGnb1MeasurementId();
            params.gnb2MeasurementId = x2ResStatUpdHeader.GetGnb2MeasurementId();
            params.cellMeasurementResultList = x2ResStatUpdHeader.GetCellMeasurementResultList();

            NS_LOG_LOGIC("gnb1MeasurementId = " << params.gnb1MeasurementId);
            NS_LOG_LOGIC("gnb2MeasurementId = " << params.gnb2MeasurementId);
            NS_LOG_LOGIC(
                "cellMeasurementResultList size = " << params.cellMeasurementResultList.size());

            m_x2SapUser->RecvResourceStatusUpdate(params);
        }
    }
    else if (procedureCode == NrEpcX2Header::HandoverCancel)
    {
        if (messageType == NrEpcX2Header::SuccessfulOutcome)
        {
            NS_LOG_LOGIC("Recv X2 message: HANDOVER CANCEL");

            NrEpcX2HandoverCancelHeader x2HoCancelHeader;
            packet->RemoveHeader(x2HoCancelHeader);

            NS_LOG_INFO("X2 HandoverCancel header: " << x2HoCancelHeader);

            NrEpcX2SapUser::HandoverCancelParams params;
            params.oldGnbUeX2apId = x2HoCancelHeader.GetOldGnbUeX2apId();
            params.newGnbUeX2apId = x2HoCancelHeader.GetNewGnbUeX2apId();
            params.sourceCellId = cellsInfo->m_localCellIds.at(0);
            params.targetCellId = cellsInfo->m_remoteCellIds.at(0);
            params.cause = x2HoCancelHeader.GetCause();

            NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
            NS_LOG_LOGIC("newGnbUeX2apId = " << params.newGnbUeX2apId);
            NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);
            NS_LOG_LOGIC("targetCellId = " << params.targetCellId);
            NS_LOG_LOGIC("cause = " << params.cause);

            m_x2SapUser->RecvHandoverCancel(params);
        }
    }
    else
    {
        NS_ASSERT_MSG(false, "ProcedureCode NOT SUPPORTED!!!");
    }
}

void
NrEpcX2::RecvFromX2uSocket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);

    NS_LOG_LOGIC("Recv UE DATA through X2-U interface from Socket");
    Ptr<Packet> packet = socket->Recv();
    NS_LOG_LOGIC("packetLen = " << packet->GetSize());

    NS_ASSERT_MSG(m_x2InterfaceCellIds.find(socket) != m_x2InterfaceCellIds.end(),
                  "Missing infos of local and remote CellId");
    Ptr<NrX2CellInfo> cellsInfo = m_x2InterfaceCellIds[socket];

    NrGtpuHeader gtpu;
    packet->RemoveHeader(gtpu);

    NS_LOG_LOGIC("GTP-U header: " << gtpu);

    NrEpcX2SapUser::UeDataParams params;
    params.sourceCellId = cellsInfo->m_remoteCellIds.at(0);
    params.targetCellId = cellsInfo->m_localCellIds.at(0);
    params.gtpTeid = gtpu.GetTeid();
    params.ueData = packet;

    m_x2SapUser->RecvUeData(params);
}

//
// Implementation of the X2 SAP Provider
//
void
NrEpcX2::DoSendHandoverRequest(NrEpcX2SapProvider::HandoverRequestParams params)
{
    NS_LOG_FUNCTION(this);

    NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
    NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);
    NS_LOG_LOGIC("targetCellId = " << params.targetCellId);
    NS_LOG_LOGIC("mmeUeS1apId  = " << params.mmeUeS1apId);

    NS_ASSERT_MSG(m_x2InterfaceSockets.find(params.targetCellId) != m_x2InterfaceSockets.end(),
                  "Missing infos for targetCellId = " << params.targetCellId);
    Ptr<NrX2IfaceInfo> socketInfo = m_x2InterfaceSockets[params.targetCellId];
    Ptr<Socket> sourceSocket = socketInfo->m_localCtrlPlaneSocket;
    Ipv4Address targetIpAddr = socketInfo->m_remoteIpAddr;

    NS_LOG_LOGIC("sourceSocket = " << sourceSocket);
    NS_LOG_LOGIC("targetIpAddr = " << targetIpAddr);

    NS_LOG_INFO("Send X2 message: HANDOVER REQUEST");

    // Build the X2 message
    NrEpcX2HandoverRequestHeader x2HoReqHeader;
    x2HoReqHeader.SetOldGnbUeX2apId(params.oldGnbUeX2apId);
    x2HoReqHeader.SetCause(params.cause);
    x2HoReqHeader.SetTargetCellId(params.targetCellId);
    x2HoReqHeader.SetMmeUeS1apId(params.mmeUeS1apId);
    x2HoReqHeader.SetUeAggregateMaxBitRateDownlink(params.ueAggregateMaxBitRateDownlink);
    x2HoReqHeader.SetUeAggregateMaxBitRateUplink(params.ueAggregateMaxBitRateUplink);
    x2HoReqHeader.SetBearers(params.bearers);

    NrEpcX2Header x2Header;
    x2Header.SetMessageType(NrEpcX2Header::InitiatingMessage);
    x2Header.SetProcedureCode(NrEpcX2Header::HandoverPreparation);
    x2Header.SetLengthOfIes(x2HoReqHeader.GetLengthOfIes());
    x2Header.SetNumberOfIes(x2HoReqHeader.GetNumberOfIes());

    NS_LOG_INFO("X2 header: " << x2Header);
    NS_LOG_INFO("X2 HandoverRequest header: " << x2HoReqHeader);

    // Build the X2 packet
    Ptr<Packet> packet = (params.rrcContext) ? (params.rrcContext) : (Create<Packet>());
    packet->AddHeader(x2HoReqHeader);
    packet->AddHeader(x2Header);
    NS_LOG_INFO("packetLen = " << packet->GetSize());

    // Send the X2 message through the socket
    sourceSocket->SendTo(packet, 0, InetSocketAddress(targetIpAddr, m_x2cUdpPort));
}

void
NrEpcX2::DoSendHandoverRequestAck(NrEpcX2SapProvider::HandoverRequestAckParams params)
{
    NS_LOG_FUNCTION(this);

    NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
    NS_LOG_LOGIC("newGnbUeX2apId = " << params.newGnbUeX2apId);
    NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);
    NS_LOG_LOGIC("targetCellId = " << params.targetCellId);

    NS_ASSERT_MSG(m_x2InterfaceSockets.find(params.sourceCellId) != m_x2InterfaceSockets.end(),
                  "Socket infos not defined for sourceCellId = " << params.sourceCellId);

    Ptr<Socket> localSocket = m_x2InterfaceSockets[params.sourceCellId]->m_localCtrlPlaneSocket;
    Ipv4Address remoteIpAddr = m_x2InterfaceSockets[params.sourceCellId]->m_remoteIpAddr;

    NS_LOG_LOGIC("localSocket = " << localSocket);
    NS_LOG_LOGIC("remoteIpAddr = " << remoteIpAddr);

    NS_LOG_INFO("Send X2 message: HANDOVER REQUEST ACK");

    // Build the X2 message
    NrEpcX2HandoverRequestAckHeader x2HoAckHeader;
    x2HoAckHeader.SetOldGnbUeX2apId(params.oldGnbUeX2apId);
    x2HoAckHeader.SetNewGnbUeX2apId(params.newGnbUeX2apId);
    x2HoAckHeader.SetAdmittedBearers(params.admittedBearers);
    x2HoAckHeader.SetNotAdmittedBearers(params.notAdmittedBearers);

    NrEpcX2Header x2Header;
    x2Header.SetMessageType(NrEpcX2Header::SuccessfulOutcome);
    x2Header.SetProcedureCode(NrEpcX2Header::HandoverPreparation);
    x2Header.SetLengthOfIes(x2HoAckHeader.GetLengthOfIes());
    x2Header.SetNumberOfIes(x2HoAckHeader.GetNumberOfIes());

    NS_LOG_INFO("X2 header: " << x2Header);
    NS_LOG_INFO("X2 HandoverAck header: " << x2HoAckHeader);
    NS_LOG_INFO("RRC context: " << params.rrcContext);

    // Build the X2 packet
    Ptr<Packet> packet = (params.rrcContext) ? (params.rrcContext) : (Create<Packet>());
    packet->AddHeader(x2HoAckHeader);
    packet->AddHeader(x2Header);
    NS_LOG_INFO("packetLen = " << packet->GetSize());

    // Send the X2 message through the socket
    localSocket->SendTo(packet, 0, InetSocketAddress(remoteIpAddr, m_x2cUdpPort));
}

void
NrEpcX2::DoSendHandoverPreparationFailure(
    NrEpcX2SapProvider::HandoverPreparationFailureParams params)
{
    NS_LOG_FUNCTION(this);

    NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
    NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);
    NS_LOG_LOGIC("targetCellId = " << params.targetCellId);
    NS_LOG_LOGIC("cause = " << params.cause);
    NS_LOG_LOGIC("criticalityDiagnostics = " << params.criticalityDiagnostics);

    NS_ASSERT_MSG(m_x2InterfaceSockets.find(params.sourceCellId) != m_x2InterfaceSockets.end(),
                  "Socket infos not defined for sourceCellId = " << params.sourceCellId);

    Ptr<Socket> localSocket = m_x2InterfaceSockets[params.sourceCellId]->m_localCtrlPlaneSocket;
    Ipv4Address remoteIpAddr = m_x2InterfaceSockets[params.sourceCellId]->m_remoteIpAddr;

    NS_LOG_LOGIC("localSocket = " << localSocket);
    NS_LOG_LOGIC("remoteIpAddr = " << remoteIpAddr);

    NS_LOG_INFO("Send X2 message: HANDOVER PREPARATION FAILURE");

    // Build the X2 message
    NrEpcX2HandoverPreparationFailureHeader x2HoPrepFailHeader;
    x2HoPrepFailHeader.SetOldGnbUeX2apId(params.oldGnbUeX2apId);
    x2HoPrepFailHeader.SetCause(params.cause);
    x2HoPrepFailHeader.SetCriticalityDiagnostics(params.criticalityDiagnostics);

    NrEpcX2Header x2Header;
    x2Header.SetMessageType(NrEpcX2Header::UnsuccessfulOutcome);
    x2Header.SetProcedureCode(NrEpcX2Header::HandoverPreparation);
    x2Header.SetLengthOfIes(x2HoPrepFailHeader.GetLengthOfIes());
    x2Header.SetNumberOfIes(x2HoPrepFailHeader.GetNumberOfIes());

    NS_LOG_INFO("X2 header: " << x2Header);
    NS_LOG_INFO("X2 HandoverPrepFail header: " << x2HoPrepFailHeader);

    // Build the X2 packet
    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(x2HoPrepFailHeader);
    packet->AddHeader(x2Header);
    NS_LOG_INFO("packetLen = " << packet->GetSize());

    // Send the X2 message through the socket
    localSocket->SendTo(packet, 0, InetSocketAddress(remoteIpAddr, m_x2cUdpPort));
}

void
NrEpcX2::DoSendSnStatusTransfer(NrEpcX2SapProvider::SnStatusTransferParams params)
{
    NS_LOG_FUNCTION(this);

    NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
    NS_LOG_LOGIC("newGnbUeX2apId = " << params.newGnbUeX2apId);
    NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);
    NS_LOG_LOGIC("targetCellId = " << params.targetCellId);
    NS_LOG_LOGIC("erabsList size = " << params.erabsSubjectToStatusTransferList.size());

    NS_ASSERT_MSG(m_x2InterfaceSockets.find(params.targetCellId) != m_x2InterfaceSockets.end(),
                  "Socket infos not defined for targetCellId = " << params.targetCellId);

    Ptr<Socket> localSocket = m_x2InterfaceSockets[params.targetCellId]->m_localCtrlPlaneSocket;
    Ipv4Address remoteIpAddr = m_x2InterfaceSockets[params.targetCellId]->m_remoteIpAddr;

    NS_LOG_LOGIC("localSocket = " << localSocket);
    NS_LOG_LOGIC("remoteIpAddr = " << remoteIpAddr);

    NS_LOG_INFO("Send X2 message: SN STATUS TRANSFER");

    // Build the X2 message
    NrEpcX2SnStatusTransferHeader x2SnStatusXferHeader;
    x2SnStatusXferHeader.SetOldGnbUeX2apId(params.oldGnbUeX2apId);
    x2SnStatusXferHeader.SetNewGnbUeX2apId(params.newGnbUeX2apId);
    x2SnStatusXferHeader.SetErabsSubjectToStatusTransferList(
        params.erabsSubjectToStatusTransferList);

    NrEpcX2Header x2Header;
    x2Header.SetMessageType(NrEpcX2Header::InitiatingMessage);
    x2Header.SetProcedureCode(NrEpcX2Header::SnStatusTransfer);
    x2Header.SetLengthOfIes(x2SnStatusXferHeader.GetLengthOfIes());
    x2Header.SetNumberOfIes(x2SnStatusXferHeader.GetNumberOfIes());

    NS_LOG_INFO("X2 header: " << x2Header);
    NS_LOG_INFO("X2 SnStatusTransfer header: " << x2SnStatusXferHeader);

    // Build the X2 packet
    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(x2SnStatusXferHeader);
    packet->AddHeader(x2Header);
    NS_LOG_INFO("packetLen = " << packet->GetSize());

    // Send the X2 message through the socket
    localSocket->SendTo(packet, 0, InetSocketAddress(remoteIpAddr, m_x2cUdpPort));
}

void
NrEpcX2::DoSendUeContextRelease(NrEpcX2SapProvider::UeContextReleaseParams params)
{
    NS_LOG_FUNCTION(this);

    NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
    NS_LOG_LOGIC("newGnbUeX2apId = " << params.newGnbUeX2apId);
    NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);

    NS_ASSERT_MSG(m_x2InterfaceSockets.find(params.sourceCellId) != m_x2InterfaceSockets.end(),
                  "Socket infos not defined for sourceCellId = " << params.sourceCellId);

    Ptr<Socket> localSocket = m_x2InterfaceSockets[params.sourceCellId]->m_localCtrlPlaneSocket;
    Ipv4Address remoteIpAddr = m_x2InterfaceSockets[params.sourceCellId]->m_remoteIpAddr;

    NS_LOG_LOGIC("localSocket = " << localSocket);
    NS_LOG_LOGIC("remoteIpAddr = " << remoteIpAddr);

    NS_LOG_INFO("Send X2 message: UE CONTEXT RELEASE");

    // Build the X2 message
    NrEpcX2UeContextReleaseHeader x2UeCtxReleaseHeader;
    x2UeCtxReleaseHeader.SetOldGnbUeX2apId(params.oldGnbUeX2apId);
    x2UeCtxReleaseHeader.SetNewGnbUeX2apId(params.newGnbUeX2apId);

    NrEpcX2Header x2Header;
    x2Header.SetMessageType(NrEpcX2Header::InitiatingMessage);
    x2Header.SetProcedureCode(NrEpcX2Header::UeContextRelease);
    x2Header.SetLengthOfIes(x2UeCtxReleaseHeader.GetLengthOfIes());
    x2Header.SetNumberOfIes(x2UeCtxReleaseHeader.GetNumberOfIes());

    NS_LOG_INFO("X2 header: " << x2Header);
    NS_LOG_INFO("X2 UeContextRelease header: " << x2UeCtxReleaseHeader);

    // Build the X2 packet
    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(x2UeCtxReleaseHeader);
    packet->AddHeader(x2Header);
    NS_LOG_INFO("packetLen = " << packet->GetSize());

    // Send the X2 message through the socket
    localSocket->SendTo(packet, 0, InetSocketAddress(remoteIpAddr, m_x2cUdpPort));
}

void
NrEpcX2::DoSendLoadInformation(NrEpcX2SapProvider::LoadInformationParams params)
{
    NS_LOG_FUNCTION(this);

    NS_LOG_LOGIC("targetCellId = " << params.targetCellId);
    NS_LOG_LOGIC("cellInformationList size = " << params.cellInformationList.size());

    NS_ASSERT_MSG(m_x2InterfaceSockets.find(params.targetCellId) != m_x2InterfaceSockets.end(),
                  "Missing infos for targetCellId = " << params.targetCellId);
    Ptr<NrX2IfaceInfo> socketInfo = m_x2InterfaceSockets[params.targetCellId];
    Ptr<Socket> sourceSocket = socketInfo->m_localCtrlPlaneSocket;
    Ipv4Address targetIpAddr = socketInfo->m_remoteIpAddr;

    NS_LOG_LOGIC("sourceSocket = " << sourceSocket);
    NS_LOG_LOGIC("targetIpAddr = " << targetIpAddr);

    NS_LOG_INFO("Send X2 message: LOAD INFORMATION");

    // Build the X2 message
    NrEpcX2LoadInformationHeader x2LoadInfoHeader;
    x2LoadInfoHeader.SetCellInformationList(params.cellInformationList);

    NrEpcX2Header x2Header;
    x2Header.SetMessageType(NrEpcX2Header::InitiatingMessage);
    x2Header.SetProcedureCode(NrEpcX2Header::LoadIndication);
    x2Header.SetLengthOfIes(x2LoadInfoHeader.GetLengthOfIes());
    x2Header.SetNumberOfIes(x2LoadInfoHeader.GetNumberOfIes());

    NS_LOG_INFO("X2 header: " << x2Header);
    NS_LOG_INFO("X2 LoadInformation header: " << x2LoadInfoHeader);

    // Build the X2 packet
    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(x2LoadInfoHeader);
    packet->AddHeader(x2Header);
    NS_LOG_INFO("packetLen = " << packet->GetSize());

    // Send the X2 message through the socket
    sourceSocket->SendTo(packet, 0, InetSocketAddress(targetIpAddr, m_x2cUdpPort));
}

void
NrEpcX2::DoSendResourceStatusUpdate(NrEpcX2SapProvider::ResourceStatusUpdateParams params)
{
    NS_LOG_FUNCTION(this);

    NS_LOG_LOGIC("targetCellId = " << params.targetCellId);
    NS_LOG_LOGIC("gnb1MeasurementId = " << params.gnb1MeasurementId);
    NS_LOG_LOGIC("gnb2MeasurementId = " << params.gnb2MeasurementId);
    NS_LOG_LOGIC("cellMeasurementResultList size = " << params.cellMeasurementResultList.size());

    NS_ASSERT_MSG(m_x2InterfaceSockets.find(params.targetCellId) != m_x2InterfaceSockets.end(),
                  "Missing infos for targetCellId = " << params.targetCellId);
    Ptr<NrX2IfaceInfo> socketInfo = m_x2InterfaceSockets[params.targetCellId];
    Ptr<Socket> sourceSocket = socketInfo->m_localCtrlPlaneSocket;
    Ipv4Address targetIpAddr = socketInfo->m_remoteIpAddr;

    NS_LOG_LOGIC("sourceSocket = " << sourceSocket);
    NS_LOG_LOGIC("targetIpAddr = " << targetIpAddr);

    NS_LOG_INFO("Send X2 message: RESOURCE STATUS UPDATE");

    // Build the X2 message
    NrEpcX2ResourceStatusUpdateHeader x2ResourceStatUpdHeader;
    x2ResourceStatUpdHeader.SetGnb1MeasurementId(params.gnb1MeasurementId);
    x2ResourceStatUpdHeader.SetGnb2MeasurementId(params.gnb2MeasurementId);
    x2ResourceStatUpdHeader.SetCellMeasurementResultList(params.cellMeasurementResultList);

    NrEpcX2Header x2Header;
    x2Header.SetMessageType(NrEpcX2Header::InitiatingMessage);
    x2Header.SetProcedureCode(NrEpcX2Header::ResourceStatusReporting);
    x2Header.SetLengthOfIes(x2ResourceStatUpdHeader.GetLengthOfIes());
    x2Header.SetNumberOfIes(x2ResourceStatUpdHeader.GetNumberOfIes());

    NS_LOG_INFO("X2 header: " << x2Header);
    NS_LOG_INFO("X2 ResourceStatusUpdate header: " << x2ResourceStatUpdHeader);

    // Build the X2 packet
    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(x2ResourceStatUpdHeader);
    packet->AddHeader(x2Header);
    NS_LOG_INFO("packetLen = " << packet->GetSize());

    // Send the X2 message through the socket
    sourceSocket->SendTo(packet, 0, InetSocketAddress(targetIpAddr, m_x2cUdpPort));
}

void
NrEpcX2::DoSendUeData(NrEpcX2SapProvider::UeDataParams params)
{
    NS_LOG_FUNCTION(this);

    NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);
    NS_LOG_LOGIC("targetCellId = " << params.targetCellId);
    NS_LOG_LOGIC("gtpTeid = " << params.gtpTeid);

    NS_ASSERT_MSG(m_x2InterfaceSockets.find(params.targetCellId) != m_x2InterfaceSockets.end(),
                  "Missing infos for targetCellId = " << params.targetCellId);
    Ptr<NrX2IfaceInfo> socketInfo = m_x2InterfaceSockets[params.targetCellId];
    Ptr<Socket> sourceSocket = socketInfo->m_localUserPlaneSocket;
    Ipv4Address targetIpAddr = socketInfo->m_remoteIpAddr;

    NS_LOG_LOGIC("sourceSocket = " << sourceSocket);
    NS_LOG_LOGIC("targetIpAddr = " << targetIpAddr);

    NrGtpuHeader gtpu;
    gtpu.SetTeid(params.gtpTeid);
    gtpu.SetLength(params.ueData->GetSize() + gtpu.GetSerializedSize() -
                   8); /// \todo This should be done in NrGtpuHeader
    NS_LOG_INFO("GTP-U header: " << gtpu);

    Ptr<Packet> packet = params.ueData;
    packet->AddHeader(gtpu);

    NS_LOG_INFO("Forward UE DATA through X2 interface");
    sourceSocket->SendTo(packet, 0, InetSocketAddress(targetIpAddr, m_x2uUdpPort));
}

void
NrEpcX2::DoSendHandoverCancel(NrEpcX2SapProvider::HandoverCancelParams params)
{
    NS_LOG_FUNCTION(this);

    NS_LOG_LOGIC("oldGnbUeX2apId = " << params.oldGnbUeX2apId);
    NS_LOG_LOGIC("newGnbUeX2apId = " << params.newGnbUeX2apId);
    NS_LOG_LOGIC("sourceCellId = " << params.sourceCellId);
    NS_LOG_LOGIC("targetCellId = " << params.targetCellId);

    NS_ASSERT_MSG(m_x2InterfaceSockets.find(params.targetCellId) != m_x2InterfaceSockets.end(),
                  "Socket infos not defined for targetCellId = " << params.targetCellId);

    Ptr<Socket> localSocket = m_x2InterfaceSockets[params.targetCellId]->m_localCtrlPlaneSocket;
    Ipv4Address remoteIpAddr = m_x2InterfaceSockets[params.targetCellId]->m_remoteIpAddr;

    NS_LOG_LOGIC("localSocket = " << localSocket);
    NS_LOG_LOGIC("remoteIpAddr = " << remoteIpAddr);

    NS_LOG_INFO("Send X2 message: HANDOVER CANCEL");

    // Build the X2 message
    NrEpcX2HandoverCancelHeader x2HandoverCancelHeader;
    x2HandoverCancelHeader.SetOldGnbUeX2apId(params.oldGnbUeX2apId);
    x2HandoverCancelHeader.SetNewGnbUeX2apId(params.newGnbUeX2apId);
    x2HandoverCancelHeader.SetCause(params.cause);

    NrEpcX2Header x2Header;
    x2Header.SetMessageType(NrEpcX2Header::SuccessfulOutcome);
    x2Header.SetProcedureCode(NrEpcX2Header::HandoverCancel);
    x2Header.SetLengthOfIes(x2HandoverCancelHeader.GetLengthOfIes());
    x2Header.SetNumberOfIes(x2HandoverCancelHeader.GetNumberOfIes());

    NS_LOG_INFO("X2 header: " << x2Header);
    NS_LOG_INFO("X2 UeContextRelease header: " << x2HandoverCancelHeader);

    // Build the X2 packet
    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(x2HandoverCancelHeader);
    packet->AddHeader(x2Header);
    NS_LOG_INFO("packetLen = " << packet->GetSize());

    // Send the X2 message through the socket
    localSocket->SendTo(packet, 0, InetSocketAddress(remoteIpAddr, m_x2cUdpPort));
}

} // namespace ns3
