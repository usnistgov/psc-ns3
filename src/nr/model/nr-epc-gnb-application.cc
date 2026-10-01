// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Jaume Nin <jnin@cttc.cat>
//         Nicola Baldo <nbaldo@cttc.cat>

#include "nr-epc-gnb-application.h"

#include "nr-epc-gtpu-header.h"
#include "nr-eps-bearer-tag.h"

#include "ns3/inet-socket-address.h"
#include "ns3/ipv4.h"
#include "ns3/log.h"
#include "ns3/mac48-address.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEpcGnbApplication");

NrEpcGnbApplication::EpsFlowId_t::EpsFlowId_t()
{
}

NrEpcGnbApplication::EpsFlowId_t::EpsFlowId_t(const uint16_t a, const uint8_t b)
    : m_rnti(a),
      m_bid(b)
{
}

bool
operator==(const NrEpcGnbApplication::EpsFlowId_t& a, const NrEpcGnbApplication::EpsFlowId_t& b)
{
    return ((a.m_rnti == b.m_rnti) && (a.m_bid == b.m_bid));
}

bool
operator<(const NrEpcGnbApplication::EpsFlowId_t& a, const NrEpcGnbApplication::EpsFlowId_t& b)
{
    return ((a.m_rnti < b.m_rnti) || ((a.m_rnti == b.m_rnti) && (a.m_bid < b.m_bid)));
}

TypeId
NrEpcGnbApplication::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrEpcGnbApplication")
            .SetParent<Object>()
            .SetGroupName("Nr")
            .AddTraceSource("RxFromGnb",
                            "Receive data packets from NR Gnb Net Device",
                            MakeTraceSourceAccessor(&NrEpcGnbApplication::m_rxNrSocketPktTrace),
                            "ns3::NrEpcGnbApplication::RxTracedCallback")
            .AddTraceSource("RxFromS1u",
                            "Receive data packets from S1-U Net Device",
                            MakeTraceSourceAccessor(&NrEpcGnbApplication::m_rxS1uSocketPktTrace),
                            "ns3::NrEpcGnbApplication::RxTracedCallback");
    return tid;
}

void
NrEpcGnbApplication::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_nrSocket = nullptr;
    m_nrSocket6 = nullptr;
    m_s1uSocket = nullptr;
    delete m_s1SapProvider;
    delete m_s1apSapGnb;
}

NrEpcGnbApplication::NrEpcGnbApplication(Ptr<Socket> nrSocket,
                                         Ptr<Socket> nrSocket6,
                                         uint16_t cellId)
    : m_nrSocket(nrSocket),
      m_nrSocket6(nrSocket6),
      m_gtpuUdpPort(2152), // fixed by the standard
      m_s1SapUser(nullptr),
      m_s1apSapMme(nullptr),
      m_cellId(cellId)
{
    NS_LOG_FUNCTION(this << nrSocket << nrSocket6 << cellId);

    m_nrSocket->SetRecvCallback(MakeCallback(&NrEpcGnbApplication::RecvFromNrSocket, this));
    m_nrSocket6->SetRecvCallback(MakeCallback(&NrEpcGnbApplication::RecvFromNrSocket, this));
    m_s1SapProvider = new NrMemberEpcGnbS1SapProvider<NrEpcGnbApplication>(this);
    m_s1apSapGnb = new NrMemberEpcS1apSapGnb<NrEpcGnbApplication>(this);
}

void
NrEpcGnbApplication::AddS1Interface(Ptr<Socket> s1uSocket,
                                    Ipv4Address gnbAddress,
                                    Ipv4Address sgwAddress)
{
    NS_LOG_FUNCTION(this << s1uSocket << gnbAddress << sgwAddress);

    m_s1uSocket = s1uSocket;
    m_s1uSocket->SetRecvCallback(MakeCallback(&NrEpcGnbApplication::RecvFromS1uSocket, this));
    m_gnbS1uAddress = gnbAddress;
    m_sgwS1uAddress = sgwAddress;
}

NrEpcGnbApplication::~NrEpcGnbApplication()
{
    NS_LOG_FUNCTION(this);
}

void
NrEpcGnbApplication::SetS1SapUser(NrEpcGnbS1SapUser* s)
{
    m_s1SapUser = s;
}

NrEpcGnbS1SapProvider*
NrEpcGnbApplication::GetS1SapProvider()
{
    return m_s1SapProvider;
}

void
NrEpcGnbApplication::SetS1apSapMme(NrEpcS1apSapMme* s)
{
    m_s1apSapMme = s;
}

NrEpcS1apSapGnb*
NrEpcGnbApplication::GetS1apSapGnb()
{
    return m_s1apSapGnb;
}

void
NrEpcGnbApplication::DoInitialUeMessage(uint64_t imsi, uint16_t rnti)
{
    NS_LOG_FUNCTION(this);
    // side effect: create entry if not exist
    m_imsiRntiMap[imsi] = rnti;
    m_s1apSapMme->InitialUeMessage(imsi, rnti, imsi, m_cellId);
}

void
NrEpcGnbApplication::DoPathSwitchRequest(NrEpcGnbS1SapProvider::PathSwitchRequestParameters params)
{
    NS_LOG_FUNCTION(this);
    uint16_t gnbUeS1Id = params.rnti;
    uint64_t mmeUeS1Id = params.mmeUeS1Id;
    uint64_t imsi = mmeUeS1Id;
    // side effect: create entry if not exist
    m_imsiRntiMap[imsi] = params.rnti;

    uint16_t gci = params.cellId;
    std::list<NrEpcS1apSapMme::ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList;
    for (auto bit = params.bearersToBeSwitched.begin(); bit != params.bearersToBeSwitched.end();
         ++bit)
    {
        EpsFlowId_t flowId;
        flowId.m_rnti = params.rnti;
        flowId.m_bid = bit->epsBearerId;
        uint32_t teid = bit->teid;

        EpsFlowId_t rbid(params.rnti, bit->epsBearerId);
        // side effect: create entries if not exist
        m_rbidTeidMap[params.rnti][bit->epsBearerId] = teid;
        m_teidRbidMap[teid] = rbid;

        NrEpcS1apSapMme::ErabSwitchedInDownlinkItem erab;
        erab.erabId = bit->epsBearerId;
        erab.gnbTransportLayerAddress = m_gnbS1uAddress;
        erab.gnbTeid = bit->teid;

        erabToBeSwitchedInDownlinkList.push_back(erab);
    }
    m_s1apSapMme->PathSwitchRequest(gnbUeS1Id, mmeUeS1Id, gci, erabToBeSwitchedInDownlinkList);
}

void
NrEpcGnbApplication::DoUeContextRelease(uint16_t rnti)
{
    NS_LOG_FUNCTION(this << rnti);
    auto rntiIt = m_rbidTeidMap.find(rnti);
    if (rntiIt != m_rbidTeidMap.end())
    {
        for (auto bidIt = rntiIt->second.begin(); bidIt != rntiIt->second.end(); ++bidIt)
        {
            uint32_t teid = bidIt->second;
            m_teidRbidMap.erase(teid);
            NS_LOG_INFO("TEID: " << teid << " erased");
        }
        m_rbidTeidMap.erase(rntiIt);
        NS_LOG_INFO("RNTI: " << rntiIt->first << " erased");
    }
}

void
NrEpcGnbApplication::DoInitialContextSetupRequest(
    uint64_t mmeUeS1Id,
    uint16_t gnbUeS1Id,
    std::list<NrEpcS1apSapGnb::ErabToBeSetupItem> erabToBeSetupList)
{
    NS_LOG_FUNCTION(this);

    uint64_t imsi = mmeUeS1Id;
    auto imsiIt = m_imsiRntiMap.find(imsi);
    NS_ASSERT_MSG(imsiIt != m_imsiRntiMap.end(), "unknown IMSI");
    uint16_t rnti = imsiIt->second;

    for (auto erabIt = erabToBeSetupList.begin(); erabIt != erabToBeSetupList.end(); ++erabIt)
    {
        // request the RRC to setup a radio bearer
        NrEpcGnbS1SapUser::DataRadioBearerSetupRequestParameters params;
        params.rnti = rnti;
        params.bearer = erabIt->erabLevelQosParameters;
        params.bearerId = erabIt->erabId;
        params.gtpTeid = erabIt->sgwTeid;
        m_s1SapUser->DataRadioBearerSetupRequest(params);

        EpsFlowId_t rbid(rnti, erabIt->erabId);
        // side effect: create entries if not exist
        m_rbidTeidMap[rnti][erabIt->erabId] = params.gtpTeid;
        m_teidRbidMap[params.gtpTeid] = rbid;
    }

    // Send Initial Context Setup Request to RRC
    NrEpcGnbS1SapUser::InitialContextSetupRequestParameters params;
    params.rnti = rnti;
    m_s1SapUser->InitialContextSetupRequest(params);
}

void
NrEpcGnbApplication::DoPathSwitchRequestAcknowledge(
    uint64_t gnbUeS1Id,
    uint64_t mmeUeS1Id,
    uint16_t gci,
    std::list<NrEpcS1apSapGnb::ErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList)
{
    NS_LOG_FUNCTION(this);

    uint64_t imsi = mmeUeS1Id;
    auto imsiIt = m_imsiRntiMap.find(imsi);
    NS_ASSERT_MSG(imsiIt != m_imsiRntiMap.end(), "unknown IMSI");
    uint16_t rnti = imsiIt->second;
    NrEpcGnbS1SapUser::PathSwitchRequestAcknowledgeParameters params;
    params.rnti = rnti;
    m_s1SapUser->PathSwitchRequestAcknowledge(params);
}

void
NrEpcGnbApplication::RecvFromNrSocket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this);
    if (m_nrSocket6)
    {
        NS_ASSERT(socket == m_nrSocket || socket == m_nrSocket6);
    }
    else
    {
        NS_ASSERT(socket == m_nrSocket);
    }
    Ptr<Packet> packet = socket->Recv();

    NrEpsBearerTag tag;
    bool found = packet->RemovePacketTag(tag);
    NS_ASSERT(found);
    uint16_t rnti = tag.GetRnti();
    uint8_t bid = tag.GetBid();
    NS_LOG_INFO("Received packet with RNTI: " << rnti << ", BID: " << +bid);
    auto rntiIt = m_rbidTeidMap.find(rnti);
    if (rntiIt == m_rbidTeidMap.end())
    {
        NS_LOG_WARN("UE context not found, discarding packet");
    }
    else
    {
        auto bidIt = rntiIt->second.find(bid);
        NS_ASSERT(bidIt != rntiIt->second.end());
        uint32_t teid = bidIt->second;
        m_rxNrSocketPktTrace(packet->Copy());
        SendToS1uSocket(packet, teid);
    }
}

void
NrEpcGnbApplication::RecvFromS1uSocket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    NS_ASSERT(socket == m_s1uSocket);
    Ptr<Packet> packet = socket->Recv();
    NrGtpuHeader gtpu;
    packet->RemoveHeader(gtpu);
    uint32_t teid = gtpu.GetTeid();
    NS_LOG_INFO("Received packet from S1-U interface with GTP TEID: " << teid);
    auto it = m_teidRbidMap.find(teid);
    if (it == m_teidRbidMap.end())
    {
        NS_LOG_WARN("UE context at cell id " << m_cellId << " not found, discarding packet");
    }
    else
    {
        m_rxS1uSocketPktTrace(packet->Copy());
        SendToNrSocket(packet, it->second.m_rnti, it->second.m_bid);
    }
}

void
NrEpcGnbApplication::SendToNrSocket(Ptr<Packet> packet, uint16_t rnti, uint8_t bid)
{
    NS_LOG_FUNCTION(this << packet << rnti << bid << packet->GetSize());
    NrEpsBearerTag tag(rnti, bid);
    packet->AddPacketTag(tag);
    NS_LOG_INFO("Add NrEpsBearerTag with RNTI " << rnti << " and bearer ID " << +bid);
    uint8_t ipType;

    packet->CopyData(&ipType, 1);
    ipType = (ipType >> 4) & 0x0f;

    int sentBytes;
    if (ipType == 0x04)
    {
        NS_LOG_INFO("Forward packet from gNB's S1-U to NR stack via IPv4 socket.");
        sentBytes = m_nrSocket->Send(packet);
    }
    else if (ipType == 0x06)
    {
        NS_LOG_INFO("Forward packet from gNB's S1-U to NR stack via IPv6 socket.");
        sentBytes = m_nrSocket6->Send(packet);
    }
    else
    {
        NS_ABORT_MSG("NrEpcGnbApplication::SendToNrSocket - Unknown IP type...");
    }

    NS_ASSERT(sentBytes > 0);
}

void
NrEpcGnbApplication::SendToS1uSocket(Ptr<Packet> packet, uint32_t teid)
{
    NS_LOG_FUNCTION(this << packet << teid << packet->GetSize());
    NrGtpuHeader gtpu;
    gtpu.SetTeid(teid);
    // From 3GPP TS 29.281 v10.0.0 Section 5.1
    // Length of the payload + the non obligatory GTP-U header
    gtpu.SetLength(packet->GetSize() + gtpu.GetSerializedSize() - 8);
    packet->AddHeader(gtpu);
    uint32_t flags = 0;
    NS_LOG_INFO("Forward packet from gNB's NR to S1-U stack with TEID: " << teid);
    m_s1uSocket->SendTo(packet, flags, InetSocketAddress(m_sgwS1uAddress, m_gtpuUdpPort));
}

void
NrEpcGnbApplication::DoReleaseIndication(uint64_t imsi, uint16_t rnti, uint8_t bearerId)
{
    NS_LOG_FUNCTION(this << bearerId);
    std::list<NrEpcS1apSapMme::ErabToBeReleasedIndication> erabToBeReleaseIndication;
    NrEpcS1apSapMme::ErabToBeReleasedIndication erab;
    erab.erabId = bearerId;
    erabToBeReleaseIndication.push_back(erab);
    // From 3GPP TS 23401-950 Section 5.4.4.2, gNB sends EPS bearer Identity in Bearer Release
    // Indication message to MME
    m_s1apSapMme->ErabReleaseIndication(imsi, rnti, erabToBeReleaseIndication);
}

} // namespace ns3
