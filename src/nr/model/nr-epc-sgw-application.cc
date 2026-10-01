// Copyright (c) 2017-2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#include "nr-epc-sgw-application.h"

#include "nr-epc-gtpu-header.h"

#include "ns3/log.h"

#include <map>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEpcSgwApplication");

NS_OBJECT_ENSURE_REGISTERED(NrEpcSgwApplication);

NrEpcSgwApplication::NrEpcSgwApplication(const Ptr<Socket> s1uSocket,
                                         Ipv4Address s5Addr,
                                         const Ptr<Socket> s5uSocket,
                                         const Ptr<Socket> s5cSocket)
    : m_s5Addr(s5Addr),
      m_s5uSocket(s5uSocket),
      m_s5cSocket(s5cSocket),
      m_s1uSocket(s1uSocket),
      m_gtpuUdpPort(2152), // fixed by the standard
      m_gtpcUdpPort(2123), // fixed by the standard
      m_teidCount(0)
{
    NS_LOG_FUNCTION(this << s1uSocket << s5Addr << s5uSocket << s5cSocket);
    m_s1uSocket->SetRecvCallback(MakeCallback(&NrEpcSgwApplication::RecvFromS1uSocket, this));
    m_s5uSocket->SetRecvCallback(MakeCallback(&NrEpcSgwApplication::RecvFromS5uSocket, this));
    m_s5cSocket->SetRecvCallback(MakeCallback(&NrEpcSgwApplication::RecvFromS5cSocket, this));
}

NrEpcSgwApplication::~NrEpcSgwApplication()
{
    NS_LOG_FUNCTION(this);
}

void
NrEpcSgwApplication::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_s1uSocket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
    m_s1uSocket = nullptr;
    m_s5uSocket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
    m_s5uSocket = nullptr;
    m_s5cSocket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
    m_s5cSocket = nullptr;
}

TypeId
NrEpcSgwApplication::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcSgwApplication").SetParent<Object>().SetGroupName("Nr");
    return tid;
}

void
NrEpcSgwApplication::AddMme(Ipv4Address mmeS11Addr, Ptr<Socket> s11Socket)
{
    NS_LOG_FUNCTION(this << mmeS11Addr << s11Socket);
    m_mmeS11Addr = mmeS11Addr;
    m_s11Socket = s11Socket;
    m_s11Socket->SetRecvCallback(MakeCallback(&NrEpcSgwApplication::RecvFromS11Socket, this));
}

void
NrEpcSgwApplication::AddPgw(Ipv4Address pgwAddr)
{
    NS_LOG_FUNCTION(this << pgwAddr);
    m_pgwAddr = pgwAddr;
}

void
NrEpcSgwApplication::AddGnb(uint16_t cellId, Ipv4Address gnbAddr, Ipv4Address sgwAddr)
{
    NS_LOG_FUNCTION(this << cellId << gnbAddr << sgwAddr);
    GnbInfo gnbInfo;
    gnbInfo.gnbAddr = gnbAddr;
    gnbInfo.sgwAddr = sgwAddr;
    m_gnbInfoByCellId[cellId] = gnbInfo;
}

void
NrEpcSgwApplication::RecvFromS11Socket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    NS_ASSERT(socket == m_s11Socket);
    Ptr<Packet> packet = socket->Recv();
    NrGtpcHeader header;
    packet->PeekHeader(header);
    uint16_t msgType = header.GetMessageType();

    switch (msgType)
    {
    case NrGtpcHeader::CreateSessionRequest:
        DoRecvCreateSessionRequest(packet);
        break;

    case NrGtpcHeader::ModifyBearerRequest:
        DoRecvModifyBearerRequest(packet);
        break;

    case NrGtpcHeader::DeleteBearerCommand:
        DoRecvDeleteBearerCommand(packet);
        break;

    case NrGtpcHeader::DeleteBearerResponse:
        DoRecvDeleteBearerResponse(packet);
        break;

    default:
        NS_FATAL_ERROR("GTP-C message not supported");
        break;
    }
}

void
NrEpcSgwApplication::RecvFromS5uSocket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    NS_ASSERT(socket == m_s5uSocket);
    Ptr<Packet> packet = socket->Recv();
    NrGtpuHeader gtpu;
    packet->RemoveHeader(gtpu);
    uint32_t teid = gtpu.GetTeid();

    Ipv4Address gnbAddr = m_gnbByTeidMap[teid];
    NS_LOG_DEBUG("eNB " << gnbAddr << " TEID " << teid);
    SendToS1uSocket(packet, gnbAddr, teid);
}

void
NrEpcSgwApplication::RecvFromS5cSocket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    NS_ASSERT(socket == m_s5cSocket);
    Ptr<Packet> packet = socket->Recv();
    NrGtpcHeader header;
    packet->PeekHeader(header);
    uint16_t msgType = header.GetMessageType();

    switch (msgType)
    {
    case NrGtpcHeader::CreateSessionResponse:
        DoRecvCreateSessionResponse(packet);
        break;

    case NrGtpcHeader::ModifyBearerResponse:
        DoRecvModifyBearerResponse(packet);
        break;

    case NrGtpcHeader::DeleteBearerRequest:
        DoRecvDeleteBearerRequest(packet);
        break;

    default:
        NS_FATAL_ERROR("GTP-C message not supported");
        break;
    }
}

void
NrEpcSgwApplication::RecvFromS1uSocket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    NS_ASSERT(socket == m_s1uSocket);
    Ptr<Packet> packet = socket->Recv();
    NrGtpuHeader gtpu;
    packet->RemoveHeader(gtpu);
    uint32_t teid = gtpu.GetTeid();

    SendToS5uSocket(packet, m_pgwAddr, teid);
}

void
NrEpcSgwApplication::SendToS1uSocket(Ptr<Packet> packet, Ipv4Address gnbAddr, uint32_t teid)
{
    NS_LOG_FUNCTION(this << packet << gnbAddr << teid);

    NrGtpuHeader gtpu;
    gtpu.SetTeid(teid);
    // From 3GPP TS 29.281 v10.0.0 Section 5.1
    // Length of the payload + the non obligatory GTP-U header
    gtpu.SetLength(packet->GetSize() + gtpu.GetSerializedSize() - 8);
    packet->AddHeader(gtpu);
    m_s1uSocket->SendTo(packet, 0, InetSocketAddress(gnbAddr, m_gtpuUdpPort));
}

void
NrEpcSgwApplication::SendToS5uSocket(Ptr<Packet> packet, Ipv4Address pgwAddr, uint32_t teid)
{
    NS_LOG_FUNCTION(this << packet << pgwAddr << teid);

    NrGtpuHeader gtpu;
    gtpu.SetTeid(teid);
    // From 3GPP TS 29.281 v10.0.0 Section 5.1
    // Length of the payload + the non obligatory GTP-U header
    gtpu.SetLength(packet->GetSize() + gtpu.GetSerializedSize() - 8);
    packet->AddHeader(gtpu);
    m_s5uSocket->SendTo(packet, 0, InetSocketAddress(pgwAddr, m_gtpuUdpPort));
}

///////////////////////////////////
// Process messages from the MME
///////////////////////////////////

void
NrEpcSgwApplication::DoRecvCreateSessionRequest(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrGtpcCreateSessionRequestMessage msg;
    packet->RemoveHeader(msg);
    uint64_t imsi = msg.GetImsi();
    uint16_t cellId = msg.GetUliEcgi();
    NS_LOG_DEBUG("cellId " << cellId << " IMSI " << imsi);

    auto gnbit = m_gnbInfoByCellId.find(cellId);
    NS_ASSERT_MSG(gnbit != m_gnbInfoByCellId.end(), "unknown CellId " << cellId);
    Ipv4Address gnbAddr = gnbit->second.gnbAddr;
    NS_LOG_DEBUG("eNB " << gnbAddr);

    NrGtpcHeader::Fteid_t mmeS11Fteid = msg.GetSenderCpFteid();
    NS_ASSERT_MSG(mmeS11Fteid.interfaceType == NrGtpcHeader::S11_MME_GTPC, "wrong interface type");

    NrGtpcCreateSessionRequestMessage msgOut;
    msgOut.SetImsi(imsi);
    msgOut.SetUliEcgi(cellId);

    NrGtpcHeader::Fteid_t sgwS5cFteid;
    sgwS5cFteid.interfaceType = NrGtpcHeader::S5_SGW_GTPC;
    sgwS5cFteid.teid = imsi;
    m_mmeS11FteidBySgwS5cTeid[sgwS5cFteid.teid] = mmeS11Fteid;
    sgwS5cFteid.addr = m_s5Addr;
    msgOut.SetSenderCpFteid(sgwS5cFteid); // S5 SGW GTP-C TEID

    std::list<NrGtpcCreateSessionRequestMessage::BearerContextToBeCreated> bearerContexts =
        msg.GetBearerContextsToBeCreated();
    NS_LOG_DEBUG("BearerContextToBeCreated size = " << bearerContexts.size());
    std::list<NrGtpcCreateSessionRequestMessage::BearerContextToBeCreated> bearerContextsOut;
    for (auto& bearerContext : bearerContexts)
    {
        // simple sanity check. If you ever need more than 4M teids
        // throughout your simulation, you'll need to implement a smarter teid
        // management algorithm.
        NS_ABORT_IF(m_teidCount == 0xFFFFFFFF);
        uint32_t teid = ++m_teidCount;

        NS_LOG_DEBUG("  TEID " << teid);
        m_gnbByTeidMap[teid] = gnbAddr;

        NrGtpcCreateSessionRequestMessage::BearerContextToBeCreated bearerContextOut;
        bearerContextOut.sgwS5uFteid.interfaceType = NrGtpcHeader::S5_SGW_GTPU;
        bearerContextOut.sgwS5uFteid.teid = teid; // S5U SGW FTEID
        bearerContextOut.sgwS5uFteid.addr = gnbit->second.sgwAddr;
        bearerContextOut.epsBearerId = bearerContext.epsBearerId;
        bearerContextOut.bearerLevelQos = bearerContext.bearerLevelQos;
        bearerContextOut.tft = bearerContext.tft;
        bearerContextsOut.push_back(bearerContextOut);
    }

    msgOut.SetBearerContextsToBeCreated(bearerContextsOut);

    msgOut.SetTeid(0);
    msgOut.ComputeMessageLength();

    Ptr<Packet> packetOut = Create<Packet>();
    packetOut->AddHeader(msgOut);
    NS_LOG_DEBUG("Send CreateSessionRequest to PGW " << m_pgwAddr);
    m_s5cSocket->SendTo(packetOut, 0, InetSocketAddress(m_pgwAddr, m_gtpcUdpPort));
}

void
NrEpcSgwApplication::DoRecvModifyBearerRequest(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrGtpcModifyBearerRequestMessage msg;
    packet->RemoveHeader(msg);
    uint64_t imsi = msg.GetImsi();
    uint16_t cellId = msg.GetUliEcgi();
    NS_LOG_DEBUG("cellId " << cellId << " IMSI " << imsi);

    auto gnbit = m_gnbInfoByCellId.find(cellId);
    NS_ASSERT_MSG(gnbit != m_gnbInfoByCellId.end(), "unknown CellId " << cellId);
    Ipv4Address gnbAddr = gnbit->second.gnbAddr;
    NS_LOG_DEBUG("eNB " << gnbAddr);

    NrGtpcModifyBearerRequestMessage msgOut;
    msgOut.SetImsi(imsi);
    msgOut.SetUliEcgi(cellId);

    std::list<NrGtpcModifyBearerRequestMessage::BearerContextToBeModified> bearerContextsOut;
    std::list<NrGtpcModifyBearerRequestMessage::BearerContextToBeModified> bearerContexts =
        msg.GetBearerContextsToBeModified();
    NS_LOG_DEBUG("BearerContextsToBeModified size = " << bearerContexts.size());
    for (auto& bearerContext : bearerContexts)
    {
        NS_ASSERT_MSG(bearerContext.fteid.interfaceType == NrGtpcHeader::S1U_GNB_GTPU,
                      "Wrong FTEID in ModifyBearerRequest msg");
        uint32_t teid = bearerContext.fteid.teid;
        Ipv4Address gnbAddr = bearerContext.fteid.addr;
        NS_LOG_DEBUG("bearerId " << (uint16_t)bearerContext.epsBearerId << " TEID " << teid);
        auto addrit = m_gnbByTeidMap.find(teid);
        NS_ASSERT_MSG(addrit != m_gnbByTeidMap.end(), "unknown TEID " << teid);
        addrit->second = gnbAddr;
        NrGtpcModifyBearerRequestMessage::BearerContextToBeModified bearerContextOut;
        bearerContextOut.epsBearerId = bearerContext.epsBearerId;
        bearerContextOut.fteid.interfaceType = NrGtpcHeader::S5_SGW_GTPU;
        bearerContextOut.fteid.addr = m_s5Addr;
        bearerContextOut.fteid.teid = bearerContext.fteid.teid;

        bearerContextsOut.push_back(bearerContextOut);
    }

    msgOut.SetTeid(imsi);
    msgOut.ComputeMessageLength();

    Ptr<Packet> packetOut = Create<Packet>();
    packetOut->AddHeader(msgOut);
    NS_LOG_DEBUG("Send ModifyBearerRequest to PGW " << m_pgwAddr);
    m_s5cSocket->SendTo(packetOut, 0, InetSocketAddress(m_pgwAddr, m_gtpcUdpPort));
}

void
NrEpcSgwApplication::DoRecvDeleteBearerCommand(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrGtpcDeleteBearerCommandMessage msg;
    packet->RemoveHeader(msg);

    std::list<NrGtpcDeleteBearerCommandMessage::BearerContext> bearerContextsOut;
    for (auto& bearerContext : msg.GetBearerContexts())
    {
        NS_LOG_DEBUG("ebid " << (uint16_t)bearerContext.m_epsBearerId);
        NrGtpcDeleteBearerCommandMessage::BearerContext bearerContextOut;
        bearerContextOut.m_epsBearerId = bearerContext.m_epsBearerId;
        bearerContextsOut.push_back(bearerContextOut);
    }

    NrGtpcDeleteBearerCommandMessage msgOut;
    msgOut.SetBearerContexts(bearerContextsOut);
    msgOut.SetTeid(msg.GetTeid());
    msgOut.ComputeMessageLength();

    Ptr<Packet> packetOut = Create<Packet>();
    packetOut->AddHeader(msgOut);
    NS_LOG_DEBUG("Send DeleteBearerCommand to PGW " << m_pgwAddr);
    m_s5cSocket->SendTo(packetOut, 0, InetSocketAddress(m_pgwAddr, m_gtpcUdpPort));
}

void
NrEpcSgwApplication::DoRecvDeleteBearerResponse(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrGtpcDeleteBearerResponseMessage msg;
    packet->RemoveHeader(msg);
    NrGtpcDeleteBearerResponseMessage msgOut;
    msgOut.SetEpsBearerIds(msg.GetEpsBearerIds());
    msgOut.SetTeid(msg.GetTeid());
    msgOut.ComputeMessageLength();

    Ptr<Packet> packetOut = Create<Packet>();
    packetOut->AddHeader(msgOut);
    NS_LOG_DEBUG("Send DeleteBearerResponse to PGW " << m_pgwAddr);
    m_s5cSocket->SendTo(packetOut, 0, InetSocketAddress(m_pgwAddr, m_gtpcUdpPort));
}

////////////////////////////////////////////
// Process messages received from the PGW
////////////////////////////////////////////

void
NrEpcSgwApplication::DoRecvCreateSessionResponse(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrGtpcCreateSessionResponseMessage msg;
    packet->RemoveHeader(msg);

    NrGtpcHeader::Fteid_t pgwS5cFteid = msg.GetSenderCpFteid();
    NS_ASSERT_MSG(pgwS5cFteid.interfaceType == NrGtpcHeader::S5_PGW_GTPC, "wrong interface type");

    NrGtpcCreateSessionResponseMessage msgOut;
    msgOut.SetCause(NrGtpcCreateSessionResponseMessage::REQUEST_ACCEPTED);

    uint32_t teid = msg.GetTeid();
    NrGtpcHeader::Fteid_t mmeS11Fteid = m_mmeS11FteidBySgwS5cTeid[teid];

    std::list<NrGtpcCreateSessionResponseMessage::BearerContextCreated> bearerContexts =
        msg.GetBearerContextsCreated();
    NS_LOG_DEBUG("BearerContextsCreated size = " << bearerContexts.size());
    std::list<NrGtpcCreateSessionResponseMessage::BearerContextCreated> bearerContextsOut;
    for (auto& bearerContext : bearerContexts)
    {
        NrGtpcCreateSessionResponseMessage::BearerContextCreated bearerContextOut;
        bearerContextOut.fteid.interfaceType = NrGtpcHeader::S5_SGW_GTPU;
        bearerContextOut.fteid.teid = bearerContext.fteid.teid;
        bearerContextOut.fteid.addr = m_s5Addr;
        bearerContextOut.epsBearerId = bearerContext.epsBearerId;
        bearerContextOut.bearerLevelQos = bearerContext.bearerLevelQos;
        bearerContextOut.tft = bearerContext.tft;
        bearerContextsOut.push_back(bearerContext);
    }
    msgOut.SetBearerContextsCreated(bearerContextsOut);

    msgOut.SetTeid(mmeS11Fteid.teid);
    msgOut.ComputeMessageLength();

    Ptr<Packet> packetOut = Create<Packet>();
    packetOut->AddHeader(msgOut);
    NS_LOG_DEBUG("Send CreateSessionResponse to MME " << mmeS11Fteid.addr);
    m_s11Socket->SendTo(packetOut, 0, InetSocketAddress(mmeS11Fteid.addr, m_gtpcUdpPort));
}

void
NrEpcSgwApplication::DoRecvModifyBearerResponse(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrGtpcModifyBearerResponseMessage msg;
    packet->RemoveHeader(msg);

    NrGtpcModifyBearerResponseMessage msgOut;
    msgOut.SetCause(NrGtpcIes::REQUEST_ACCEPTED);
    msgOut.SetTeid(msg.GetTeid());
    msgOut.ComputeMessageLength();

    Ptr<Packet> packetOut = Create<Packet>();
    packetOut->AddHeader(msgOut);
    NS_LOG_DEBUG("Send ModifyBearerResponse to MME " << m_mmeS11Addr);
    m_s11Socket->SendTo(packetOut, 0, InetSocketAddress(m_mmeS11Addr, m_gtpcUdpPort));
}

void
NrEpcSgwApplication::DoRecvDeleteBearerRequest(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrGtpcDeleteBearerRequestMessage msg;
    packet->RemoveHeader(msg);

    NrGtpcDeleteBearerRequestMessage msgOut;
    msgOut.SetEpsBearerIds(msg.GetEpsBearerIds());
    msgOut.SetTeid(msg.GetTeid());
    msgOut.ComputeMessageLength();

    Ptr<Packet> packetOut = Create<Packet>();
    packetOut->AddHeader(msgOut);
    NS_LOG_DEBUG("Send DeleteBearerRequest to MME " << m_mmeS11Addr);
    m_s11Socket->SendTo(packetOut, 0, InetSocketAddress(m_mmeS11Addr, m_gtpcUdpPort));
}

} // namespace ns3
