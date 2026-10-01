// Copyright (c) 2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>
//         (based on epc-sgw-pgw-application.cc)

#include "nr-epc-pgw-application.h"

#include "nr-epc-gtpu-header.h"

#include "ns3/abort.h"
#include "ns3/inet-socket-address.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6-header.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/ipv6.h"
#include "ns3/log.h"
#include "ns3/mac48-address.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEpcPgwApplication");

/////////////////////////
// NrUeInfo
/////////////////////////

NrEpcPgwApplication::NrUeInfo::NrUeInfo()
{
    NS_LOG_FUNCTION(this);
}

void
NrEpcPgwApplication::NrUeInfo::AddBearer(uint8_t bearerId, uint32_t teid, Ptr<NrEpcTft> tft)
{
    NS_LOG_FUNCTION(this << (uint16_t)bearerId << teid << tft);
    m_teidByBearerIdMap[bearerId] = teid;
    return m_tftClassifier.Add(tft, teid);
}

void
NrEpcPgwApplication::NrUeInfo::RemoveBearer(uint8_t bearerId)
{
    NS_LOG_FUNCTION(this << (uint16_t)bearerId);
    auto it = m_teidByBearerIdMap.find(bearerId);
    m_tftClassifier.Delete(it->second); // delete tft
    m_teidByBearerIdMap.erase(bearerId);
}

uint32_t
NrEpcPgwApplication::NrUeInfo::Classify(Ptr<Packet> p, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << p);
    // we hardcode DOWNLINK direction since the PGW is expected to
    // classify only downlink packets (uplink packets will go to the
    // internet without any classification).
    return m_tftClassifier.Classify(p, NrEpcTft::DOWNLINK, protocolNumber);
}

Ipv4Address
NrEpcPgwApplication::NrUeInfo::GetSgwAddr()
{
    return m_sgwAddr;
}

void
NrEpcPgwApplication::NrUeInfo::SetSgwAddr(Ipv4Address sgwAddr)
{
    m_sgwAddr = sgwAddr;
}

Ipv4Address
NrEpcPgwApplication::NrUeInfo::GetUeAddr()
{
    return m_ueAddr;
}

void
NrEpcPgwApplication::NrUeInfo::SetUeAddr(Ipv4Address ueAddr)
{
    m_ueAddr = ueAddr;
}

Ipv6Address
NrEpcPgwApplication::NrUeInfo::GetUeAddr6()
{
    return m_ueAddr6;
}

void
NrEpcPgwApplication::NrUeInfo::SetUeAddr6(Ipv6Address ueAddr)
{
    m_ueAddr6 = ueAddr;
}

/////////////////////////
// NrEpcPgwApplication
/////////////////////////

TypeId
NrEpcPgwApplication::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrEpcPgwApplication")
            .SetParent<Object>()
            .SetGroupName("Nr")
            .AddTraceSource("RxFromTun",
                            "Receive data packets from internet in Tunnel NetDevice",
                            MakeTraceSourceAccessor(&NrEpcPgwApplication::m_rxTunPktTrace),
                            "ns3::NrEpcPgwApplication::RxTracedCallback")
            .AddTraceSource("RxFromS1u",
                            "Receive data packets from S5 Socket",
                            MakeTraceSourceAccessor(&NrEpcPgwApplication::m_rxS5PktTrace),
                            "ns3::NrEpcPgwApplication::RxTracedCallback");
    return tid;
}

void
NrEpcPgwApplication::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_s5uSocket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
    m_s5uSocket = nullptr;
    m_s5cSocket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
    m_s5cSocket = nullptr;
}

NrEpcPgwApplication::NrEpcPgwApplication(const Ptr<VirtualNetDevice> tunDevice,
                                         Ipv4Address s5Addr,
                                         const Ptr<Socket> s5uSocket,
                                         const Ptr<Socket> s5cSocket)
    : m_pgwS5Addr(s5Addr),
      m_s5uSocket(s5uSocket),
      m_s5cSocket(s5cSocket),
      m_tunDevice(tunDevice),
      m_gtpuUdpPort(2152), // fixed by the standard
      m_gtpcUdpPort(2123)  // fixed by the standard
{
    NS_LOG_FUNCTION(this << tunDevice << s5Addr << s5uSocket << s5cSocket);
    m_s5uSocket->SetRecvCallback(MakeCallback(&NrEpcPgwApplication::RecvFromS5uSocket, this));
    m_s5cSocket->SetRecvCallback(MakeCallback(&NrEpcPgwApplication::RecvFromS5cSocket, this));
}

NrEpcPgwApplication::~NrEpcPgwApplication()
{
    NS_LOG_FUNCTION(this);
}

bool
NrEpcPgwApplication::RecvFromTunDevice(Ptr<Packet> packet,
                                       const Address& source,
                                       const Address& dest,
                                       uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << source << dest << protocolNumber << packet << packet->GetSize());
    m_rxTunPktTrace(packet->Copy());

    // get IP address of UE
    if (protocolNumber == Ipv4L3Protocol::PROT_NUMBER)
    {
        Ipv4Header ipv4Header;
        packet->PeekHeader(ipv4Header);
        Ipv4Address ueAddr = ipv4Header.GetDestination();
        NS_LOG_LOGIC("packet addressed to UE " << ueAddr);

        // find corresponding NrUeInfo address
        bool foundRemoteUe = false;
        bool foundSingleHopUe = false;
        // check if it is a remote UE connected to a L3 relay UE
        auto it = m_ueInfoByRemoteUeAddrMap.find(ueAddr);
        foundRemoteUe = (it != m_ueInfoByRemoteUeAddrMap.end());
        if (foundRemoteUe)
        {
            NS_LOG_DEBUG("Found a remote UE connected to a L3 relay UE with IPv4 address: "
                         << it->second->GetUeAddr());
        }
        else
        {
            // check if it is a regular UE
            it = m_ueInfoByAddrMap.find(ueAddr);
            foundSingleHopUe = (it != m_ueInfoByAddrMap.end());
            if (!foundSingleHopUe)
            {
                NS_LOG_WARN("unknown UE address " << ueAddr);
            }
        }
        if (foundRemoteUe || foundSingleHopUe)
        {
            Ipv4Address sgwAddr = it->second->GetSgwAddr();
            uint32_t teid = it->second->Classify(packet, protocolNumber);
            if (teid == 0)
            {
                NS_LOG_WARN("no matching bearer for this packet");
            }
            else
            {
                SendToS5uSocket(packet, sgwAddr, teid);
            }
        }
    }
    else if (protocolNumber == Ipv6L3Protocol::PROT_NUMBER)
    {
        Ipv6Header ipv6Header;
        packet->PeekHeader(ipv6Header);
        Ipv6Address ueAddr = ipv6Header.GetDestination();
        NS_LOG_LOGIC("packet addressed to UE " << ueAddr);

        // find corresponding NrUeInfo address
        auto it = m_ueInfoByAddrMap6.find(ueAddr);
        if (it == m_ueInfoByAddrMap6.end())
        {
            NS_LOG_WARN("unknown UE address " << ueAddr);
        }
        else
        {
            Ipv4Address sgwAddr = it->second->GetSgwAddr();
            uint32_t teid = it->second->Classify(packet, protocolNumber);
            if (teid == 0)
            {
                NS_LOG_WARN("no matching bearer for this packet");
            }
            else
            {
                SendToS5uSocket(packet, sgwAddr, teid);
            }
        }
    }
    else
    {
        NS_ABORT_MSG("Unknown IP type");
    }

    // there is no reason why we should notify the TUN
    // VirtualNetDevice that he failed to send the packet: if we receive
    // any bogus packet, it will just be silently discarded.
    const bool succeeded = true;
    return succeeded;
}

void
NrEpcPgwApplication::RecvFromS5uSocket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    NS_ASSERT(socket == m_s5uSocket);
    Ptr<Packet> packet = socket->Recv();
    m_rxS5PktTrace(packet->Copy());

    NrGtpuHeader gtpu;
    packet->RemoveHeader(gtpu);
    uint32_t teid = gtpu.GetTeid();

    SendToTunDevice(packet, teid);
}

void
NrEpcPgwApplication::RecvFromS5cSocket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    NS_ASSERT(socket == m_s5cSocket);
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
NrEpcPgwApplication::DoRecvCreateSessionRequest(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrGtpcCreateSessionRequestMessage msg;
    packet->RemoveHeader(msg);
    uint64_t imsi = msg.GetImsi();
    uint16_t cellId = msg.GetUliEcgi();
    NS_LOG_DEBUG("cellId " << cellId << " IMSI " << imsi);

    auto ueit = m_ueInfoByImsiMap.find(imsi);
    NS_ASSERT_MSG(ueit != m_ueInfoByImsiMap.end(), "unknown IMSI " << imsi);
    ueit->second->SetSgwAddr(m_sgwS5Addr);

    NrGtpcHeader::Fteid_t sgwS5cFteid = msg.GetSenderCpFteid();
    NS_ASSERT_MSG(sgwS5cFteid.interfaceType == NrGtpcHeader::S5_SGW_GTPC, "Wrong interface type");

    NrGtpcCreateSessionResponseMessage msgOut;
    msgOut.SetTeid(sgwS5cFteid.teid);
    msgOut.SetCause(NrGtpcCreateSessionResponseMessage::REQUEST_ACCEPTED);

    NrGtpcHeader::Fteid_t pgwS5cFteid;
    pgwS5cFteid.interfaceType = NrGtpcHeader::S5_PGW_GTPC;
    pgwS5cFteid.teid = sgwS5cFteid.teid;
    pgwS5cFteid.addr = m_pgwS5Addr;
    msgOut.SetSenderCpFteid(pgwS5cFteid);

    std::list<NrGtpcCreateSessionRequestMessage::BearerContextToBeCreated> bearerContexts =
        msg.GetBearerContextsToBeCreated();
    NS_LOG_DEBUG("BearerContextsToBeCreated size = " << bearerContexts.size());

    std::list<NrGtpcCreateSessionResponseMessage::BearerContextCreated> bearerContextsCreated;
    for (auto& bearerContext : bearerContexts)
    {
        uint32_t teid = bearerContext.sgwS5uFteid.teid;
        NS_LOG_DEBUG("bearerId " << (uint16_t)bearerContext.epsBearerId << " SGW "
                                 << bearerContext.sgwS5uFteid.addr << " TEID " << teid);

        ueit->second->AddBearer(bearerContext.epsBearerId, teid, bearerContext.tft);

        NrGtpcCreateSessionResponseMessage::BearerContextCreated bearerContextOut;
        bearerContextOut.fteid.interfaceType = NrGtpcHeader::S5_PGW_GTPU;
        bearerContextOut.fteid.teid = teid;
        bearerContextOut.fteid.addr = m_pgwS5Addr;
        bearerContextOut.epsBearerId = bearerContext.epsBearerId;
        bearerContextOut.bearerLevelQos = bearerContext.bearerLevelQos;
        bearerContextOut.tft = bearerContext.tft;
        bearerContextsCreated.push_back(bearerContextOut);
    }

    NS_LOG_DEBUG("BearerContextsCreated size = " << bearerContextsCreated.size());
    msgOut.SetBearerContextsCreated(bearerContextsCreated);
    msgOut.SetTeid(sgwS5cFteid.teid);
    msgOut.ComputeMessageLength();

    Ptr<Packet> packetOut = Create<Packet>();
    packetOut->AddHeader(msgOut);
    NS_LOG_DEBUG("Send CreateSessionResponse to SGW " << sgwS5cFteid.addr);
    m_s5cSocket->SendTo(packetOut, 0, InetSocketAddress(sgwS5cFteid.addr, m_gtpcUdpPort));
}

void
NrEpcPgwApplication::DoRecvModifyBearerRequest(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrGtpcModifyBearerRequestMessage msg;
    packet->RemoveHeader(msg);
    uint64_t imsi = msg.GetImsi();
    uint16_t cellId = msg.GetUliEcgi();
    NS_LOG_DEBUG("cellId " << cellId << " IMSI " << imsi);

    auto ueit = m_ueInfoByImsiMap.find(imsi);
    NS_ASSERT_MSG(ueit != m_ueInfoByImsiMap.end(), "unknown IMSI " << imsi);
    ueit->second->SetSgwAddr(m_sgwS5Addr);

    std::list<NrGtpcModifyBearerRequestMessage::BearerContextToBeModified> bearerContexts =
        msg.GetBearerContextsToBeModified();
    NS_LOG_DEBUG("BearerContextsToBeModified size = " << bearerContexts.size());

    for (auto& bearerContext : bearerContexts)
    {
        Ipv4Address sgwAddr = bearerContext.fteid.addr;
        uint32_t teid = bearerContext.fteid.teid;
        NS_LOG_DEBUG("bearerId " << (uint16_t)bearerContext.epsBearerId << " SGW " << sgwAddr
                                 << " TEID " << teid);
    }

    NrGtpcModifyBearerResponseMessage msgOut;
    msgOut.SetCause(NrGtpcIes::REQUEST_ACCEPTED);
    msgOut.SetTeid(imsi);
    msgOut.ComputeMessageLength();

    Ptr<Packet> packetOut = Create<Packet>();
    packetOut->AddHeader(msgOut);
    NS_LOG_DEBUG("Send ModifyBearerResponse to SGW " << m_sgwS5Addr);
    m_s5cSocket->SendTo(packetOut, 0, InetSocketAddress(m_sgwS5Addr, m_gtpcUdpPort));
}

void
NrEpcPgwApplication::DoRecvDeleteBearerCommand(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrGtpcDeleteBearerCommandMessage msg;
    packet->RemoveHeader(msg);

    std::list<uint8_t> epsBearerIds;
    for (auto& bearerContext : msg.GetBearerContexts())
    {
        NS_LOG_DEBUG("ebid " << (uint16_t)bearerContext.m_epsBearerId);
        epsBearerIds.push_back(bearerContext.m_epsBearerId);
    }

    NrGtpcDeleteBearerRequestMessage msgOut;
    msgOut.SetEpsBearerIds(epsBearerIds);
    msgOut.SetTeid(msg.GetTeid());
    msgOut.ComputeMessageLength();

    Ptr<Packet> packetOut = Create<Packet>();
    packetOut->AddHeader(msgOut);
    NS_LOG_DEBUG("Send DeleteBearerRequest to SGW " << m_sgwS5Addr);
    m_s5cSocket->SendTo(packetOut, 0, InetSocketAddress(m_sgwS5Addr, m_gtpcUdpPort));
}

void
NrEpcPgwApplication::DoRecvDeleteBearerResponse(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    NrGtpcDeleteBearerResponseMessage msg;
    packet->RemoveHeader(msg);

    uint64_t imsi = msg.GetTeid();
    auto ueit = m_ueInfoByImsiMap.find(imsi);
    NS_ASSERT_MSG(ueit != m_ueInfoByImsiMap.end(), "unknown IMSI " << imsi);

    for (auto& epsBearerId : msg.GetEpsBearerIds())
    {
        // Remove de-activated bearer contexts from PGW side
        NS_LOG_INFO("PGW removing bearer " << (uint16_t)epsBearerId << " of IMSI " << imsi);
        ueit->second->RemoveBearer(epsBearerId);
    }
}

void
NrEpcPgwApplication::SendToTunDevice(Ptr<Packet> packet, uint32_t teid)
{
    NS_LOG_FUNCTION(this << packet << teid);
    NS_LOG_LOGIC("packet size: " << packet->GetSize() << " bytes");

    uint8_t ipType;
    packet->CopyData(&ipType, 1);
    ipType = (ipType >> 4) & 0x0f;

    uint16_t protocol = 0;
    if (ipType == 0x04)
    {
        protocol = 0x0800;
    }
    else if (ipType == 0x06)
    {
        protocol = 0x86DD;
    }
    else
    {
        NS_ABORT_MSG("Unknown IP type");
    }

    m_tunDevice->Receive(packet,
                         protocol,
                         m_tunDevice->GetAddress(),
                         m_tunDevice->GetAddress(),
                         NetDevice::PACKET_HOST);
}

void
NrEpcPgwApplication::SendToS5uSocket(Ptr<Packet> packet, Ipv4Address sgwAddr, uint32_t teid)
{
    NS_LOG_FUNCTION(this << packet << sgwAddr << teid);

    NrGtpuHeader gtpu;
    gtpu.SetTeid(teid);
    // From 3GPP TS 29.281 v10.0.0 Section 5.1
    // Length of the payload + the non obligatory GTP-U header
    gtpu.SetLength(packet->GetSize() + gtpu.GetSerializedSize() - 8);
    packet->AddHeader(gtpu);
    uint32_t flags = 0;
    m_s5uSocket->SendTo(packet, flags, InetSocketAddress(sgwAddr, m_gtpuUdpPort));
}

void
NrEpcPgwApplication::AddSgw(Ipv4Address sgwS5Addr)
{
    NS_LOG_FUNCTION(this << sgwS5Addr);
    m_sgwS5Addr = sgwS5Addr;
}

void
NrEpcPgwApplication::AddUe(uint64_t imsi)
{
    NS_LOG_FUNCTION(this << imsi);
    Ptr<NrUeInfo> ueInfo = Create<NrUeInfo>();
    m_ueInfoByImsiMap[imsi] = ueInfo;
}

void
NrEpcPgwApplication::SetUeAddress(uint64_t imsi, Ipv4Address ueAddr)
{
    NS_LOG_FUNCTION(this << imsi << ueAddr);
    auto ueit = m_ueInfoByImsiMap.find(imsi);
    NS_ASSERT_MSG(ueit != m_ueInfoByImsiMap.end(), "unknown IMSI" << imsi);
    ueit->second->SetUeAddr(ueAddr);
    m_ueInfoByAddrMap[ueAddr] = ueit->second;
}

void
NrEpcPgwApplication::SetUeAddress6(uint64_t imsi, Ipv6Address ueAddr)
{
    NS_LOG_FUNCTION(this << imsi << ueAddr);
    auto ueit = m_ueInfoByImsiMap.find(imsi);
    NS_ASSERT_MSG(ueit != m_ueInfoByImsiMap.end(), "unknown IMSI " << imsi);
    m_ueInfoByAddrMap6[ueAddr] = ueit->second;
    ueit->second->SetUeAddr6(ueAddr);
}

void
NrEpcPgwApplication::AddRemoteUe(uint64_t relayImsi, Ipv4Address ueAddr)
{
    NS_LOG_FUNCTION(this << relayImsi << ueAddr);
    // the relay UE must be known to the network
    const auto relayIt = m_ueInfoByImsiMap.find(relayImsi);
    NS_ABORT_MSG_UNLESS(relayIt != m_ueInfoByImsiMap.end(), "unknown IMSI " << relayImsi);

    m_ueInfoByRemoteUeAddrMap[ueAddr] = relayIt->second;
}

void
NrEpcPgwApplication::RemoveRemoteUe(uint64_t relayImsi, Ipv4Address ueAddr)
{
    NS_LOG_FUNCTION(this << relayImsi << ueAddr);

    // the relay UE must be known to the network
    const auto relayIt = m_ueInfoByImsiMap.find(relayImsi);
    NS_ABORT_MSG_UNLESS(relayIt != m_ueInfoByImsiMap.end(), "unknown IMSI " << relayImsi);

    // the remote must be already in the map
    const auto remoteIt = m_ueInfoByRemoteUeAddrMap.find(ueAddr);
    NS_ABORT_MSG_UNLESS(remoteIt != m_ueInfoByRemoteUeAddrMap.end(),
                        "unknown UE Address " << ueAddr);

    // Remove if it is the same relay for the remote
    if (m_ueInfoByRemoteUeAddrMap[ueAddr] == m_ueInfoByImsiMap[relayImsi])
    {
        m_ueInfoByRemoteUeAddrMap.erase(ueAddr);
    }
}

} // namespace ns3
