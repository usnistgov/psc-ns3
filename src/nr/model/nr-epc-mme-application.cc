// Copyright (c) 2017-2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#include "nr-epc-mme-application.h"

#include "nr-epc-gtpc-header.h"

#include "ns3/log.h"

#include <map>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEpcMmeApplication");

NS_OBJECT_ENSURE_REGISTERED(NrEpcMmeApplication);

NrEpcMmeApplication::NrEpcMmeApplication()
    : m_gtpcUdpPort(2123) // fixed by the standard
{
    NS_LOG_FUNCTION(this);
    m_s1apSapMme = new NrMemberEpcS1apSapMme<NrEpcMmeApplication>(this);
}

NrEpcMmeApplication::~NrEpcMmeApplication()
{
    NS_LOG_FUNCTION(this);
}

void
NrEpcMmeApplication::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_s1apSapMme;
}

TypeId
NrEpcMmeApplication::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrEpcMmeApplication")
                            .SetParent<Object>()
                            .SetGroupName("Nr")
                            .AddConstructor<NrEpcMmeApplication>();
    return tid;
}

NrEpcS1apSapMme*
NrEpcMmeApplication::GetS1apSapMme()
{
    return m_s1apSapMme;
}

void
NrEpcMmeApplication::AddSgw(Ipv4Address sgwS11Addr,
                            Ipv4Address mmeS11Addr,
                            Ptr<Socket> mmeS11Socket)
{
    NS_LOG_FUNCTION(this << sgwS11Addr << mmeS11Addr << mmeS11Socket);
    m_sgwS11Addr = sgwS11Addr;
    m_mmeS11Addr = mmeS11Addr;
    m_s11Socket = mmeS11Socket;
    m_s11Socket->SetRecvCallback(MakeCallback(&NrEpcMmeApplication::RecvFromS11Socket, this));
}

void
NrEpcMmeApplication::AddGnb(uint16_t gci, Ipv4Address gnbS1uAddr, NrEpcS1apSapGnb* gnbS1apSap)
{
    NS_LOG_FUNCTION(this << gci << gnbS1uAddr << gnbS1apSap);
    Ptr<GnbInfo> gnbInfo = Create<GnbInfo>();
    gnbInfo->gci = gci;
    gnbInfo->s1uAddr = gnbS1uAddr;
    gnbInfo->s1apSapGnb = gnbS1apSap;
    m_gnbInfoMap[gci] = gnbInfo;
}

void
NrEpcMmeApplication::AddUe(uint64_t imsi)
{
    NS_LOG_FUNCTION(this << imsi);
    Ptr<NrUeInfo> ueInfo = Create<NrUeInfo>();
    ueInfo->imsi = imsi;
    ueInfo->mmeUeS1Id = imsi;
    ueInfo->bearerCounter = 0;
    m_ueInfoMap[imsi] = ueInfo;
}

uint8_t
NrEpcMmeApplication::AddBearer(uint64_t imsi, Ptr<NrEpcTft> tft, NrEpsBearer bearer)
{
    NS_LOG_FUNCTION(this << imsi);
    auto it = m_ueInfoMap.find(imsi);
    NS_ASSERT_MSG(it != m_ueInfoMap.end(), "could not find any UE with IMSI " << imsi);
    NS_ASSERT_MSG(it->second->bearerCounter < 11,
                  "too many bearers already! " << it->second->bearerCounter);
    BearerInfo bearerInfo;
    bearerInfo.bearerId = ++(it->second->bearerCounter);
    bearerInfo.tft = tft;
    bearerInfo.bearer = bearer;
    it->second->bearersToBeActivated.push_back(bearerInfo);
    return bearerInfo.bearerId;
}

// S1-AP SAP MME forwarded methods

void
NrEpcMmeApplication::DoInitialUeMessage(uint64_t mmeUeS1Id,
                                        uint16_t gnbUeS1Id,
                                        uint64_t imsi,
                                        uint16_t gci)
{
    NS_LOG_FUNCTION(this << mmeUeS1Id << gnbUeS1Id << imsi << gci);
    auto it = m_ueInfoMap.find(imsi);
    NS_ASSERT_MSG(it != m_ueInfoMap.end(), "could not find any UE with IMSI " << imsi);
    it->second->cellId = gci;

    NrGtpcCreateSessionRequestMessage msg;
    msg.SetImsi(imsi);
    msg.SetUliEcgi(gci);

    NrGtpcHeader::Fteid_t mmeS11Fteid;
    mmeS11Fteid.interfaceType = NrGtpcHeader::S11_MME_GTPC;
    mmeS11Fteid.teid = imsi;
    mmeS11Fteid.addr = m_mmeS11Addr;
    msg.SetSenderCpFteid(mmeS11Fteid); // S11 MME GTP-C F-TEID

    std::list<NrGtpcCreateSessionRequestMessage::BearerContextToBeCreated> bearerContexts;
    for (auto bit = it->second->bearersToBeActivated.begin();
         bit != it->second->bearersToBeActivated.end();
         ++bit)
    {
        NrGtpcCreateSessionRequestMessage::BearerContextToBeCreated bearerContext{};
        bearerContext.epsBearerId = bit->bearerId;
        bearerContext.tft = bit->tft;
        bearerContext.bearerLevelQos = bit->bearer;
        bearerContexts.push_back(bearerContext);
    }
    NS_LOG_DEBUG("BearerContextToBeCreated size = " << bearerContexts.size());
    msg.SetBearerContextsToBeCreated(bearerContexts);

    msg.SetTeid(0);
    msg.ComputeMessageLength();

    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(msg);
    NS_LOG_DEBUG("Send CreateSessionRequest to SGW " << m_sgwS11Addr);
    m_s11Socket->SendTo(packet, 0, InetSocketAddress(m_sgwS11Addr, m_gtpcUdpPort));
}

void
NrEpcMmeApplication::DoInitialContextSetupResponse(
    uint64_t mmeUeS1Id,
    uint16_t gnbUeS1Id,
    std::list<NrEpcS1apSapMme::ErabSetupItem> erabSetupList)
{
    NS_LOG_FUNCTION(this << mmeUeS1Id << gnbUeS1Id);
    NS_FATAL_ERROR("unimplemented");
}

void
NrEpcMmeApplication::DoPathSwitchRequest(
    uint64_t gnbUeS1Id,
    uint64_t mmeUeS1Id,
    uint16_t gci,
    std::list<NrEpcS1apSapMme::ErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList)
{
    NS_LOG_FUNCTION(this << mmeUeS1Id << gnbUeS1Id << gci);
    uint64_t imsi = mmeUeS1Id;
    auto it = m_ueInfoMap.find(imsi);
    NS_ASSERT_MSG(it != m_ueInfoMap.end(), "could not find any UE with IMSI " << imsi);
    NS_LOG_INFO("IMSI " << imsi << " old gNB: " << it->second->cellId << ", new gNB: " << gci);
    it->second->cellId = gci;
    it->second->gnbUeS1Id = gnbUeS1Id;

    NrGtpcModifyBearerRequestMessage msg;
    msg.SetImsi(imsi);
    msg.SetUliEcgi(gci);

    std::list<NrGtpcModifyBearerRequestMessage::BearerContextToBeModified> bearerContexts;
    for (auto& erab : erabToBeSwitchedInDownlinkList)
    {
        NS_LOG_DEBUG("erabId " << erab.erabId << " gNB " << erab.gnbTransportLayerAddress
                               << " TEID " << erab.gnbTeid);

        NrGtpcModifyBearerRequestMessage::BearerContextToBeModified bearerContext;
        bearerContext.epsBearerId = erab.erabId;
        bearerContext.fteid.interfaceType = NrGtpcHeader::S1U_GNB_GTPU;
        bearerContext.fteid.addr = erab.gnbTransportLayerAddress;
        bearerContext.fteid.teid = erab.gnbTeid;
        bearerContexts.push_back(bearerContext);
    }
    msg.SetBearerContextsToBeModified(bearerContexts);
    msg.SetTeid(imsi);
    msg.ComputeMessageLength();

    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(msg);
    NS_LOG_DEBUG("Send ModifyBearerRequest to SGW " << m_sgwS11Addr);
    m_s11Socket->SendTo(packet, 0, InetSocketAddress(m_sgwS11Addr, m_gtpcUdpPort));
}

void
NrEpcMmeApplication::DoErabReleaseIndication(
    uint64_t mmeUeS1Id,
    uint16_t gnbUeS1Id,
    std::list<NrEpcS1apSapMme::ErabToBeReleasedIndication> erabToBeReleaseIndication)
{
    NS_LOG_FUNCTION(this << mmeUeS1Id << gnbUeS1Id);
    uint64_t imsi = mmeUeS1Id;
    auto it = m_ueInfoMap.find(imsi);
    NS_ASSERT_MSG(it != m_ueInfoMap.end(), "could not find any UE with IMSI " << imsi);

    NrGtpcDeleteBearerCommandMessage msg;
    std::list<NrGtpcDeleteBearerCommandMessage::BearerContext> bearerContexts;
    for (auto& erab : erabToBeReleaseIndication)
    {
        NS_LOG_DEBUG("erabId " << (uint16_t)erab.erabId);
        NrGtpcDeleteBearerCommandMessage::BearerContext bearerContext;
        bearerContext.m_epsBearerId = erab.erabId;
        bearerContexts.push_back(bearerContext);
    }
    msg.SetBearerContexts(bearerContexts);
    msg.SetTeid(imsi);
    msg.ComputeMessageLength();

    Ptr<Packet> packet = Create<Packet>();
    packet->AddHeader(msg);
    NS_LOG_DEBUG("Send DeleteBearerCommand to SGW " << m_sgwS11Addr);
    m_s11Socket->SendTo(packet, 0, InetSocketAddress(m_sgwS11Addr, m_gtpcUdpPort));
}

void
NrEpcMmeApplication::RemoveBearer(Ptr<NrUeInfo> ueInfo, uint8_t epsBearerId)
{
    NS_LOG_FUNCTION(this << epsBearerId);
    auto bit = ueInfo->bearersToBeActivated.begin();
    while (bit != ueInfo->bearersToBeActivated.end())
    {
        if (bit->bearerId == epsBearerId)
        {
            ueInfo->bearersToBeActivated.erase(bit);
            ueInfo->bearerCounter = ueInfo->bearerCounter - 1;
            break;
        }
        ++bit;
    }
}

void
NrEpcMmeApplication::RecvFromS11Socket(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    NS_ASSERT(socket == m_s11Socket);
    Ptr<Packet> packet = socket->Recv();
    NrGtpcHeader header;
    packet->PeekHeader(header);
    uint16_t msgType = header.GetMessageType();

    switch (msgType)
    {
    case NrGtpcHeader::CreateSessionResponse:
        DoRecvCreateSessionResponse(header, packet);
        break;

    case NrGtpcHeader::ModifyBearerResponse:
        DoRecvModifyBearerResponse(header, packet);
        break;

    case NrGtpcHeader::DeleteBearerRequest:
        DoRecvDeleteBearerRequest(header, packet);
        break;

    default:
        NS_FATAL_ERROR("GTP-C message not supported");
        break;
    }
}

void
NrEpcMmeApplication::DoRecvCreateSessionResponse(NrGtpcHeader& header, Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << header);
    uint64_t imsi = header.GetTeid();
    NS_LOG_DEBUG("TEID/IMSI " << imsi);
    auto it = m_ueInfoMap.find(imsi);
    NS_ASSERT_MSG(it != m_ueInfoMap.end(), "could not find any UE with IMSI " << imsi);
    uint16_t cellId = it->second->cellId;
    uint16_t gnbUeS1Id = it->second->gnbUeS1Id;
    uint64_t mmeUeS1Id = it->second->mmeUeS1Id;
    NS_LOG_DEBUG("cellId " << cellId << " mmeUeS1Id " << mmeUeS1Id << " gnbUeS1Id " << gnbUeS1Id);
    auto jt = m_gnbInfoMap.find(cellId);
    NS_ASSERT_MSG(jt != m_gnbInfoMap.end(), "could not find any gNB with CellId " << cellId);

    NrGtpcCreateSessionResponseMessage msg;
    packet->RemoveHeader(msg);

    std::list<NrEpcS1apSapGnb::ErabToBeSetupItem> erabToBeSetupList;
    std::list<NrGtpcCreateSessionResponseMessage::BearerContextCreated> bearerContexts =
        msg.GetBearerContextsCreated();
    NS_LOG_DEBUG("BearerContextsCreated size = " << bearerContexts.size());
    for (auto& bearerContext : bearerContexts)
    {
        NrEpcS1apSapGnb::ErabToBeSetupItem erab;
        erab.erabId = bearerContext.epsBearerId;
        erab.erabLevelQosParameters = bearerContext.bearerLevelQos;
        erab.transportLayerAddress = bearerContext.fteid.addr; // SGW S1U address
        erab.sgwTeid = bearerContext.fteid.teid;
        NS_LOG_DEBUG("SGW " << erab.transportLayerAddress << " TEID " << erab.sgwTeid);
        erabToBeSetupList.push_back(erab);
    }

    NS_LOG_DEBUG("Send InitialContextSetupRequest to gNB " << jt->second->s1apSapGnb);
    jt->second->s1apSapGnb->InitialContextSetupRequest(mmeUeS1Id, gnbUeS1Id, erabToBeSetupList);
}

void
NrEpcMmeApplication::DoRecvModifyBearerResponse(NrGtpcHeader& header, Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << header);
    NrGtpcModifyBearerResponseMessage msg;
    packet->RemoveHeader(msg);
    NS_ASSERT(msg.GetCause() == NrGtpcModifyBearerResponseMessage::REQUEST_ACCEPTED);

    uint64_t imsi = header.GetTeid();
    NS_LOG_DEBUG("TEID/IMSI " << imsi);
    auto it = m_ueInfoMap.find(imsi);
    NS_ASSERT_MSG(it != m_ueInfoMap.end(), "could not find any UE with IMSI " << imsi);
    uint16_t cellId = it->second->cellId;
    uint16_t gnbUeS1Id = it->second->gnbUeS1Id;
    uint64_t mmeUeS1Id = it->second->mmeUeS1Id;
    NS_LOG_DEBUG("cellId " << cellId << " mmeUeS1Id " << mmeUeS1Id << " gnbUeS1Id " << gnbUeS1Id);
    std::list<NrEpcS1apSapGnb::ErabSwitchedInUplinkItem>
        erabToBeSwitchedInUplinkList; // unused for now
    auto jt = m_gnbInfoMap.find(cellId);
    NS_ASSERT_MSG(jt != m_gnbInfoMap.end(), "could not find any gNB with CellId " << cellId);

    NS_LOG_DEBUG("Send PathSwitchRequestAcknowledge to gNB " << jt->second->s1apSapGnb);
    jt->second->s1apSapGnb->PathSwitchRequestAcknowledge(gnbUeS1Id,
                                                         mmeUeS1Id,
                                                         cellId,
                                                         erabToBeSwitchedInUplinkList);
}

void
NrEpcMmeApplication::DoRecvDeleteBearerRequest(NrGtpcHeader& header, Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << header);
    uint64_t imsi = header.GetTeid();
    NS_LOG_DEBUG("TEID/IMSI " << imsi);
    auto it = m_ueInfoMap.find(imsi);
    NS_ASSERT_MSG(it != m_ueInfoMap.end(), "could not find any UE with IMSI " << imsi);

    NrGtpcDeleteBearerRequestMessage msg;
    packet->RemoveHeader(msg);

    NrGtpcDeleteBearerResponseMessage msgOut;

    std::list<uint8_t> epsBearerIds;
    for (auto& ebid : msg.GetEpsBearerIds())
    {
        epsBearerIds.push_back(ebid);
        /*
         * This condition is added to not remove bearer info at MME
         * when UE gets disconnected since the bearers are only added
         * at beginning of simulation at MME and if it is removed the
         * bearers cannot be activated again unless scheduled for
         * addition of the bearer during simulation
         *
         */
        if (it->second->cellId == 0)
        {
            RemoveBearer(it->second,
                         ebid); // schedules function to erase, context of de-activated bearer
        }
    }
    msgOut.SetEpsBearerIds(epsBearerIds);
    msgOut.SetTeid(imsi);
    msgOut.ComputeMessageLength();

    Ptr<Packet> packetOut = Create<Packet>();
    packetOut->AddHeader(msgOut);
    NS_LOG_DEBUG("Send DeleteBearerResponse to SGW " << m_sgwS11Addr);
    m_s11Socket->SendTo(packetOut, 0, InetSocketAddress(m_sgwS11Addr, m_gtpcUdpPort));
}

} // namespace ns3
