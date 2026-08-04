// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>

#include "nr-sl-epc-ue-nas.h"

#include "ns3/ipv4-header.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv6-header.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/nr-epc-tft.h"
#include "ns3/tcp-header.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/udp-header.h"
#include "ns3/udp-l4-protocol.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlEpcUeNas");

NS_OBJECT_ENSURE_REGISTERED(NrSlEpcUeNas);

NrSlEpcUeNas::NrSlEpcUeNas()
{
    NS_LOG_FUNCTION(this);
    m_nrSlAsSapUser = new MemberNrSlAsSapUser<NrSlEpcUeNas>(this);
    m_nrSlUeSvcNasSapProvider = new MemberNrSlUeSvcNasSapProvider<NrSlEpcUeNas>(this);
}

void
NrSlEpcUeNas::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_nrSlAsSapUser;
    delete m_nrSlUeSvcNasSapProvider;
    NrEpcUeNas::DoDispose();
}

TypeId
NrSlEpcUeNas::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlEpcUeNas")
            .SetParent<NrEpcUeNas>()
            .SetGroupName("Nr")
            .AddConstructor<NrSlEpcUeNas>()
            .AddTraceSource("NrSlRelayRxPacketTrace",
                            "Trace fired upon reception of data packet by a UE acting as "
                            "UE-to-Network relay UE",
                            MakeTraceSourceAccessor(&NrSlEpcUeNas::m_relayRxPacketTrace),
                            "ns3::NrSlEpcUeNas::NrSlRelayNasRxPacketTracedCallback");
    return tid;
}

bool
NrSlEpcUeNas::Send(Ptr<Packet> packet, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << packet << protocolNumber);
    uint32_t count = 0;
    switch (m_state)
    {
    case OFF: {
        // Check if there is any sidelink bearer for the destination
        Ptr<Packet> pCopy = packet->Copy();
        if (protocolNumber == Ipv4L3Protocol::PROT_NUMBER)
        {
            Ipv4Header ipv4Header;
            pCopy->RemoveHeader(ipv4Header);
            uint8_t protocol = ipv4Header.GetProtocol();
            uint16_t remotePort = 0;
            if (protocol == UdpL4Protocol::PROT_NUMBER)
            {
                UdpHeader udpHeader;
                pCopy->RemoveHeader(udpHeader);
                remotePort = udpHeader.GetDestinationPort();
            }
            else if (protocol == TcpL4Protocol::PROT_NUMBER)
            {
                TcpHeader tcpHeader;
                pCopy->RemoveHeader(tcpHeader);
                remotePort = tcpHeader.GetDestinationPort();
            }
            bool found = false;
            // In the following:
            // 1. Multicast packets will be copied and sent to all matching bearers.
            // 2. Non-multicast packets will be sent to the first matching bearer. This assumes that
            // the most specialized matching bearer was activated last and pushed to the front of
            // the list.
            for (auto it = m_slBearersActivatedList.begin(); it != m_slBearersActivatedList.end();
                 it++)
            {
                if ((*it)->Matches(ipv4Header.GetDestination(), remotePort))
                {
                    if ((*it)->GetSidelinkInfo().m_lcId != 0)
                    {
                        // Found sidelink bearer
                        NS_LOG_INFO("Found matching TFT for "
                                    << ipv4Header.GetDestination() << ":" << remotePort
                                    << " with dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                    << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                        m_nrSlAsSapProvider->SendSidelinkData(packet->Copy(),
                                                              (*it)->GetSidelinkInfo().m_dstL2Id,
                                                              (*it)->GetSidelinkInfo().m_lcId);
                        count++;
                        found = true;
                        if (!ipv4Header.GetDestination().IsMulticast())
                        {
                            break;
                        }
                    }
                    else if (!found && (*it)->GetSidelinkInfo().m_lcId == 0)
                    {
                        NS_LOG_INFO("Found matching TFT on default LC-ID for "
                                    << ipv4Header.GetDestination() << ":" << remotePort
                                    << " with dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                    << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                        m_nrSlAsSapProvider->SendSidelinkData(packet->Copy(),
                                                              (*it)->GetSidelinkInfo().m_dstL2Id,
                                                              (*it)->GetSidelinkInfo().m_lcId);
                        found = true;
                        count++;
                    }
                }
                else
                {
                    NS_LOG_DEBUG("Did not find matching TFT for "
                                 << ipv4Header.GetDestination() << ":" << remotePort
                                 << " bearer dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                 << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                }
            }
            if (found)
            {
                NS_LOG_DEBUG("Copied packet to " << count << " matching bearers");
                return true;
            }
        }
        if (protocolNumber == Ipv6L3Protocol::PROT_NUMBER)
        {
            Ipv6Header ipv6Header;
            pCopy->RemoveHeader(ipv6Header);
            uint8_t protocol = ipv6Header.GetNextHeader();
            uint16_t remotePort = 0;
            if (protocol == UdpL4Protocol::PROT_NUMBER)
            {
                UdpHeader udpHeader;
                pCopy->RemoveHeader(udpHeader);
                remotePort = udpHeader.GetDestinationPort();
            }
            else if (protocol == TcpL4Protocol::PROT_NUMBER)
            {
                TcpHeader tcpHeader;
                pCopy->RemoveHeader(tcpHeader);
                remotePort = tcpHeader.GetDestinationPort();
            }
            bool found = false;
            for (auto it = m_slBearersActivatedList.begin(); it != m_slBearersActivatedList.end();
                 it++)
            {
                if ((*it)->Matches(ipv6Header.GetDestination(), remotePort))
                {
                    if ((*it)->GetSidelinkInfo().m_lcId != 0)
                    {
                        // Found sidelink bearer
                        NS_LOG_INFO("Found matching TFT for "
                                    << ipv6Header.GetDestination() << ":" << remotePort
                                    << " with dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                    << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                        m_nrSlAsSapProvider->SendSidelinkData(packet->Copy(),
                                                              (*it)->GetSidelinkInfo().m_dstL2Id,
                                                              (*it)->GetSidelinkInfo().m_lcId);
                        found = true;
                    }
                    else if (!found && (*it)->GetSidelinkInfo().m_lcId == 0)
                    {
                        NS_LOG_INFO("Found matching TFT on default LC-ID for "
                                    << ipv6Header.GetDestination() << ":" << remotePort
                                    << " with dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                    << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                        // Found sidelink
                        m_nrSlAsSapProvider->SendSidelinkData(packet->Copy(),
                                                              (*it)->GetSidelinkInfo().m_dstL2Id,
                                                              (*it)->GetSidelinkInfo().m_lcId);
                        found = true;
                    }
                }
                else
                {
                    NS_LOG_DEBUG("Did not find matching TFT for "
                                 << ipv6Header.GetDestination() << ":" << remotePort
                                 << " bearer dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                 << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                }
            }
            if (found)
            {
                return true;
            }
        }
    }
    default:
        // Consider to check with an assert if NAS is not in OFF state, since this is sidelink
        NS_LOG_WARN(
            "NAS not in OFF state, or Sidelink bearer not found, discarding packet (state = "
            << m_state << ")");
        // TODO:  Add drop trace?
        return false;
    }
    // TODO: Check that all paths that return false also hit a drop trace somehow
}

void
NrSlEpcUeNas::DoRecvData(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);
    if (m_u2nRelayConfig.relaying)
    {
        ClassifyRecvPacketForU2nRelay(packet);
    }
    else
    {
        m_forwardUpCallback(packet);
    }
}

void
NrSlEpcUeNas::SetNrSlAsSapProvider(NrSlAsSapProvider* s)
{
    NS_LOG_FUNCTION(this << s);
    m_nrSlAsSapProvider = s;
}

NrSlAsSapUser*
NrSlEpcUeNas::GetNrSlAsSapUser()
{
    return m_nrSlAsSapUser;
}

NrSlUeSvcNasSapProvider*
NrSlEpcUeNas::GetNrSlUeSvcNasSapProvider()
{
    return m_nrSlUeSvcNasSapProvider;
}

void
NrSlEpcUeNas::SetNrSlUeSvcNasSapUser(NrSlUeSvcNasSapUser* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeSvcNasSapUser = s;
}

void
NrSlEpcUeNas::ActivateNrSlBearer(Ptr<NrSlTft> tft)
{
    NS_LOG_FUNCTION(this << tft->GetSidelinkInfo().m_lcId << tft->GetSidelinkInfo().m_dynamic);
    // regardless of the state we need to request RRC to setup the bearer
    // for in coverage case, it will trigger communication with the gNodeb
    // for out of coverage, it will trigger the use of preconfiguration
    m_pendingSlBearersList.push_back(tft);
    m_nrSlAsSapProvider->ActivateNrSlRadioBearer(tft->IsTransmit(),
                                                 tft->IsReceive(),
                                                 tft->GetSidelinkInfo());
    NS_LOG_DEBUG("Activating NR SL bearer to " << tft->GetSidelinkInfo().m_dstL2Id << " LCID "
                                               << +tft->GetSidelinkInfo().m_lcId);
}

void
NrSlEpcUeNas::DoNotifyNrSlRadioBearerActivated(const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this << slInfo.m_dstL2Id << +slInfo.m_lcId);

    auto it = m_pendingSlBearersList.begin();
    while (it != m_pendingSlBearersList.end())
    {
        if ((*it)->GetSidelinkInfo().m_dstL2Id == slInfo.m_dstL2Id)
        {
            // Found sidelink
            NS_LOG_LOGIC("Found pending SL bearer for dstL2Id " << slInfo.m_dstL2Id << " lcId "
                                                                << +slInfo.m_lcId);
            for (const auto& pfIt : (*it)->GetPacketFilters())
            {
                NS_LOG_LOGIC("SL bearer packet filter: " << pfIt);
            }
            (*it)->SetSidelinkInfoLcId(slInfo.m_lcId);

            // Only keep in the NAS bearers used for packet sending
            if ((*it)->IsTransmit())
            {
                m_slBearersActivatedList.push_front(*it);
                NS_LOG_DEBUG("SL TX bearer activated");
            }
            else
            {
                NS_LOG_DEBUG("SL bearer activated");
            }
            if (m_nrSlUeSvcNasSapUser)
            {
                m_nrSlUeSvcNasSapUser->NotifySvcNrSlDataRadioBearerActivated(slInfo.m_dstL2Id);
            }
            it = m_pendingSlBearersList.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void
NrSlEpcUeNas::DeleteNrSlBearer(Ptr<NrSlTft> tft)
{
    NS_LOG_FUNCTION(this);
    m_nrSlAsSapProvider->DeleteNrSlRadioBearer(tft->IsTransmit(),
                                               tft->IsReceive(),
                                               tft->GetSidelinkInfo());
}

void
NrSlEpcUeNas::DoNotifyNrSlRadioBearerRemoved(const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this << slInfo.m_dstL2Id << +slInfo.m_lcId);

    std::list<Ptr<NrSlTft>>::iterator it = m_slBearersActivatedList.begin();
    while (it != m_slBearersActivatedList.end())
    {
        if ((*it)->GetSidelinkInfo().m_dstL2Id == slInfo.m_dstL2Id)
        {
            // Found sidelink
            it = m_slBearersActivatedList.erase(it);

            // Notify the service layer if present
            if (m_nrSlUeSvcNasSapUser != nullptr)
            {
                m_nrSlUeSvcNasSapUser->NotifySvcNrSlDataRadioBearerRemoved(slInfo.m_dstL2Id);
            }
        }
        else
        {
            it++;
        }
    }

    // Remove from inactive too
    it = m_slBearersInactiveList.begin();
    while (it != m_slBearersInactiveList.end())
    {
        if ((*it)->GetSidelinkInfo().m_dstL2Id == slInfo.m_dstL2Id)
        {
            it = m_slBearersInactiveList.erase(it);
            if (m_nrSlUeSvcNasSapUser != nullptr)
            {
                m_nrSlUeSvcNasSapUser->NotifySvcNrSlDataRadioBearerRemoved(slInfo.m_dstL2Id);
            }
        }
        else
        {
            it++;
        }
    }
}

void
NrSlEpcUeNas::DoActivateSvcNrSlDataRadioBearer(Ptr<NrSlTft> tft)
{
    NS_LOG_FUNCTION(this << tft);

    // Move to active if inactive
    auto it = m_slBearersInactiveList.begin();
    while (it != m_slBearersInactiveList.end())
    {
        if (((*it)->GetSidelinkInfo().m_dstL2Id == tft->GetSidelinkInfo().m_dstL2Id) &&
            CheckTftFilterMatch(*it, tft))
        {
            m_slBearersActivatedList.push_front(*it);
            m_slBearersInactiveList.erase(it);
            return;
        }
        it++;
    }

    // Activate it otherwise
    m_pendingSlBearersList.push_back(tft);
    m_nrSlAsSapProvider->ActivateNrSlRadioBearer(tft->IsTransmit(),
                                                 tft->IsReceive(),
                                                 tft->GetSidelinkInfo());
}

void
NrSlEpcUeNas::DoDeactivateSvcNrSlDataRadioBearer(Ptr<NrSlTft> tft)
{
    NS_LOG_FUNCTION(this << tft);

    // Move to inactive
    auto it = m_slBearersActivatedList.begin();
    while (it != m_slBearersActivatedList.end())
    {
        if (((*it)->GetSidelinkInfo().m_dstL2Id == tft->GetSidelinkInfo().m_dstL2Id) &&
            CheckTftFilterMatch(*it, tft))
        {
            NS_LOG_INFO("Deactivating bearer with LCID " << +(*it)->GetSidelinkInfo().m_lcId);
            m_slBearersInactiveList.push_back(*it);
            m_slBearersActivatedList.erase(it);
            return;
        }
        it++;
    }
    NS_LOG_WARN("Could not find bearer to deactivate");
}

bool
NrSlEpcUeNas::CheckTftFilterMatch(Ptr<NrSlTft> tft1, Ptr<NrSlTft> tft2)
{
    NS_LOG_FUNCTION(this << tft1 << tft2);

    const auto& filters1 = tft1->GetPacketFilters();
    const auto& filters2 = tft2->GetPacketFilters();

    for (const auto& pf1 : filters1)
    {
        for (const auto& pf2 : filters2)
        {
            if (pf1 == pf2)
            {
                NS_LOG_INFO("Found exact matching packet filter:" << pf2);
                return true;
            }
        }
    }
    return false;
}

void
NrSlEpcUeNas::DoDeleteSvcNrSlDataRadioBearer(Ptr<NrSlTft> tft)
{
    NS_LOG_FUNCTION(this << tft);
    m_nrSlAsSapProvider->DeleteNrSlRadioBearer(tft->IsTransmit(),
                                               tft->IsReceive(),
                                               tft->GetSidelinkInfo());
}

void
NrSlEpcUeNas::DoConfigureNrSlDataRadioBearersForU2nRelay(
    uint32_t peerL2Id,
    enum NrSlUeProseDirLnkSapUser::U2nRole role,
    NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo,
    uint8_t relayDrbId,
    const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this << peerL2Id << role << ipInfo.peerIpv4Addr << +relayDrbId
                         << slInfo.m_srcL2Id << slInfo.m_dstL2Id);
    uint16_t nReconfigPfs = 0;

    switch (role)
    {
    case NrSlUeProseDirLnkSapUser::RemoteUe:
        NS_LOG_INFO("Role: Remote UE. Peer UE (Relay UE) L2ID: " << peerL2Id);

        // For each active UL bearer
        //(The active UL bearers info is stored in m_bearersToBeActivatedListForReconnection)
        for (std::list<BearerToBeActivated>::iterator itBe =
                 m_bearersToBeActivatedListForReconnection.begin();
             itBe != m_bearersToBeActivatedListForReconnection.end();
             itBe++)
        {
            std::list<NrEpcTft::PacketFilter> pfList = itBe->tft->GetPacketFilters();
            for (std::list<NrEpcTft::PacketFilter>::iterator itPf = pfList.begin();
                 itPf != pfList.end();
                 ++itPf)
            {
                // Currently, we can only reconfigure UL DRBs with configured remoteAddress
                // in the packet filter (as NrSlTft do the match by destination address)
                // It is important to configure this in the scenario
                if (itPf->remoteAddress.IsInitialized())
                {
                    nReconfigPfs++;
                    if (nReconfigPfs > 1)
                    {
                        NS_FATAL_ERROR(
                            "Currently supporting only one SL data radio bearer per Remote UE. "
                            "Hence only one bearer/tft/packet filter/remoteAddress should be "
                            "configured in the scenario for the Remote UEs.");
                    }

                    // Create an SL bearer for this traffic
                    auto slTft =
                        Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, itPf->remoteAddress, slInfo);
                    DoActivateSvcNrSlDataRadioBearer(slTft);

                    NS_LOG_INFO(
                        "Reconfigured UL data bearer with remoteAddress: " << itPf->remoteAddress);
                }
            }
        }
        if (nReconfigPfs == 0)
        {
            NS_FATAL_ERROR(
                "No bearer/tft/packet filter was reconfigured. Check Remote UE UL data bearer "
                "configuration. "
                "Did you forget to configure the remoteAddress on the UL NrEpcTft::PacketFilter?");
        }
        break;
    case NrSlUeProseDirLnkSapUser::RelayUe:
        NS_LOG_INFO("Role: Relay UE. Peer UE (Remote UE) L2ID: " << peerL2Id);

        // Configure U2N Relay values used for packet classification upon reception
        if (m_u2nRelayConfig.relaying == false)
        {
            // First time configuration
            m_u2nRelayConfig.relaying = true;
            m_u2nRelayConfig.selfIpv4Addr = ipInfo.selfIpv4Addr;
            m_u2nRelayConfig.relayDrbId = relayDrbId;
        }

        // Ask RRC to create and activate an SL data bearer to transmit to the Remote UE
        {
            auto slTft =
                Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, ipInfo.peerIpv4Addr, slInfo);
            DoActivateSvcNrSlDataRadioBearer(slTft);
        }
        break;
    default:
        NS_FATAL_ERROR("Invalid role " << role);
    }
}

void
NrSlEpcUeNas::DoRemoveNrSlDataRadioBearersForU2nRelay(
    uint32_t peerL2Id,
    enum NrSlUeProseDirLnkSapUser::U2nRole role,
    NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo,
    uint8_t relayDrbId)
{
    NS_LOG_FUNCTION(this << peerL2Id << role << ipInfo.peerIpv4Addr << +relayDrbId);

    switch (role)
    {
    case NrSlUeProseDirLnkSapUser::RemoteUe:
        NS_LOG_INFO("Role: Remote UE. Peer UE (Relay UE) L2ID: " << peerL2Id);

        break;
    case NrSlUeProseDirLnkSapUser::RelayUe:
        NS_LOG_INFO("Role: Relay UE. Peer UE (Remote UE) L2ID: " << peerL2Id);

        // Delete U2N relay values to help with packet classification upon reception
        if (m_u2nRelayConfig.relaying == true)
        {
            m_u2nRelayConfig.relaying = false;
            m_u2nRelayConfig.selfIpv4Addr = Ipv4Address::GetZero();
            m_u2nRelayConfig.relayDrbId = 0;
        }

        break;
    default:
        NS_FATAL_ERROR("Invalid role " << role);
    }

    SidelinkInfo slInfo;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_dstL2Id = peerL2Id;
    auto slTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, ipInfo.peerIpv4Addr, slInfo);
    DoDeleteSvcNrSlDataRadioBearer(slTft);
}

void
NrSlEpcUeNas::ClassifyRecvPacketForU2nRelay(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this);

    bool classified = false;
    Ptr<Packet> pCopy = packet->Copy();
    Ipv4Header ipv4Header;
    pCopy->RemoveHeader(ipv4Header);

    NS_LOG_DEBUG("My IPv4 address: " << m_u2nRelayConfig.selfIpv4Addr
                                     << ", Packet IPv4 destination address : "
                                     << ipv4Header.GetDestination()
                                     << " Packet IPv4 source address: " << ipv4Header.GetSource());

    // Check for SL involvement - iterate over the active SL data radio bearers
    for (std::list<Ptr<NrSlTft>>::iterator it = m_slBearersActivatedList.begin();
         it != m_slBearersActivatedList.end();
         it++)
    {
        if ((*it)->Matches(ipv4Header.GetDestination()))
        {
            // U2N relay case - Downward packet - From network to the Remote UE - Send on the SL
            NS_LOG_INFO("Relaying packet to SL");
            m_relayRxPacketTrace(m_u2nRelayConfig.selfIpv4Addr,
                                 ipv4Header.GetSource(),
                                 ipv4Header.GetDestination(),
                                 "DL",
                                 "SL",
                                 packet);
            m_nrSlAsSapProvider->SendSidelinkData(packet,
                                                  (*it)->GetSidelinkInfo().m_dstL2Id,
                                                  (*it)->GetSidelinkInfo().m_lcId);
            return;
        }
        if ((*it)->Matches(ipv4Header.GetSource()))
        {
            if (ipv4Header.GetDestination() != m_u2nRelayConfig.selfIpv4Addr)
            {
                // U2N relay case - Upward packet -> From the Remote UE to the network - Send on the
                // UL
                if (m_u2nRelayConfig.relayDrbId != 0)
                {
                    NS_LOG_INFO("Relaying packet to UL");
                    m_relayRxPacketTrace(m_u2nRelayConfig.selfIpv4Addr,
                                         ipv4Header.GetSource(),
                                         ipv4Header.GetDestination(),
                                         "SL",
                                         "UL",
                                         packet);
                    m_asSapProvider->SendData(packet, m_u2nRelayConfig.relayDrbId);
                    return;
                }
                else
                {
                    NS_FATAL_ERROR(
                        "UL data bearer ID of bearer used for U2N relay has not been configured");
                }
            }
            else
            {
                // SL Unicast only case - Packet directed to this UE - Pass to upper layer
                NS_LOG_INFO("Passing packet to upper layer");
                m_forwardUpCallback(packet);
                return;
            }
        }
    }

    // At this point SL is not involved, should be DL
    if (ipv4Header.GetDestination() == m_u2nRelayConfig.selfIpv4Addr)
    {
        // DL case - Packet directed to this UE - Pass to upper layer
        NS_LOG_INFO("Passing packet to upper layer");
        m_forwardUpCallback(packet);
        return;
    }

    if (!classified)
    {
        NS_FATAL_ERROR("Unable to classify packet for U2N relay. Missing a case to handle? ");
    }
}

bool
NrSlEpcUeNas::AddDestinationToPacketFilters(uint32_t dstL2Id, const Ipv4Address& dest)
{
    NS_LOG_FUNCTION(this << dstL2Id << dest);
    bool found = false;
    for (const auto& it : m_slBearersActivatedList)
    {
        if (it->GetSidelinkInfo().m_dstL2Id == dstL2Id)
        {
            NS_LOG_DEBUG("Found TFT for bearer to next hop L2 ID "
                         << dstL2Id << " LC ID " << +it->GetSidelinkInfo().m_lcId << "; add "
                         << dest << " to packet filter");
            bool foundInBearer = false;
            for (auto& itPf : it->GetPacketFilters())
            {
                if (itPf.remoteAddress == dest)
                {
                    NS_LOG_DEBUG("Found existing PacketFilter entry for "
                                 << dest << " to next hop L2Id " << dstL2Id);
                    foundInBearer = true;
                    found = true;
                }
            }
            if (!foundInBearer)
            {
                NS_LOG_INFO("Adding new PacketFilter entry towards "
                            << dest << " to TFT for the bearer with dstL2Id "
                            << it->GetSidelinkInfo().m_dstL2Id << " LC ID "
                            << +it->GetSidelinkInfo().m_lcId);
                NrEpcTft::PacketFilter f;
                f.remoteAddress = dest;
                f.remoteMask = Ipv4Mask("255.255.255.255");
                f.precedence = 0;
                it->Add(f);
            }
        }
    }
    return !found;
}

bool
NrSlEpcUeNas::RemoveDestinationFromPacketFilters(uint32_t dstL2Id, const Ipv4Address& dest)
{
    NS_LOG_FUNCTION(this << dstL2Id << dest);
    bool found = false;
    for (const auto& it : m_slBearersActivatedList)
    {
        if (it->GetSidelinkInfo().m_dstL2Id == dstL2Id)
        {
            NS_LOG_INFO("Found TFT for bearer to next hop L2 ID "
                        << dstL2Id << " LC ID " << +it->GetSidelinkInfo().m_lcId << "; add " << dest
                        << " to packet filter");
            for (auto& itPf : it->GetPacketFilters())
            {
                if (itPf.remoteAddress == dest)
                {
                    NS_LOG_INFO("Removing PacketFilter entry towards "
                                << dest << " to TFT for the bearer to dstL2Id "
                                << it->GetSidelinkInfo().m_dstL2Id << " LC ID "
                                << +it->GetSidelinkInfo().m_lcId);
                    NrEpcTft::PacketFilter f;
                    f.remoteAddress = dest;
                    f.remoteMask = Ipv4Mask("255.255.255.255");
                    f.precedence = 0;
                    it->Remove(f);
                    found = true;
                }
            }
        }
    }
    if (!found)
    {
        NS_LOG_DEBUG("Didn't find existing PacketFilter entry for " << dest << " to next hop L2 ID "
                                                                    << dstL2Id);
    }
    return found;
}

} // namespace ns3
