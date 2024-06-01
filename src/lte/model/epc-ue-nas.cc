/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Nicola Baldo <nbaldo@cttc.es>
 */

#include "epc-ue-nas.h"

#include "lte-as-sap.h"
#include "lte-sl-tft.h"

#include "ns3/tcp-header.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/udp-header.h"
#include "ns3/udp-l4-protocol.h"
#include <ns3/epc-helper.h>
#include <ns3/fatal-error.h>
#include <ns3/ipv4-header.h>
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/ipv6-header.h>
#include <ns3/ipv6-l3-protocol.h>
#include <ns3/log.h>
#include <ns3/simulator.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("EpcUeNas");

/// Map each of UE NAS states to its string representation.
static const std::string g_ueNasStateName[EpcUeNas::NUM_STATES] = {
    "OFF",
    "ATTACHING",
    "IDLE_REGISTERED",
    "CONNECTING_TO_EPC",
    "ACTIVE",
};

/**
 * \param s The UE NAS state.
 * \return The string representation of the given state.
 */
static inline const std::string&
ToString(EpcUeNas::State s)
{
    return g_ueNasStateName[s];
}

NS_OBJECT_ENSURE_REGISTERED(EpcUeNas);

EpcUeNas::EpcUeNas()
    : m_state(OFF),
      m_csgId(0),
      m_asSapProvider(nullptr),
      m_bidCounter(0)
{
    NS_LOG_FUNCTION(this);
    m_asSapUser = new MemberLteAsSapUser<EpcUeNas>(this);
    m_nrSlUeSvcNasSapProvider = new MemberNrSlUeSvcNasSapProvider<EpcUeNas>(this);
}

EpcUeNas::~EpcUeNas()
{
    NS_LOG_FUNCTION(this);
}

void
EpcUeNas::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_asSapUser;
    delete m_nrSlUeSvcNasSapProvider;
}

TypeId
EpcUeNas::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::EpcUeNas")
            .SetParent<Object>()
            .SetGroupName("Lte")
            .AddConstructor<EpcUeNas>()
            .AddTraceSource("StateTransition",
                            "fired upon every UE NAS state transition",
                            MakeTraceSourceAccessor(&EpcUeNas::m_stateTransitionCallback),
                            "ns3::EpcUeNas::StateTracedCallback")
            .AddTraceSource("NrSlRelayRxPacketTrace",
                            "Trace fired upon reception of data packet by a UE acting as "
                            "UE-to-Network relay UE",
                            MakeTraceSourceAccessor(&EpcUeNas::m_relayRxPacketTrace),
                            "ns3::EpcUeNas::NrSlRelayNasRxPacketTracedCallback");
    return tid;
}

void
EpcUeNas::SetDevice(Ptr<NetDevice> dev)
{
    NS_LOG_FUNCTION(this << dev);
    m_device = dev;
}

void
EpcUeNas::SetImsi(uint64_t imsi)
{
    NS_LOG_FUNCTION(this << imsi);
    m_imsi = imsi;
}

void
EpcUeNas::SetCsgId(uint32_t csgId)
{
    NS_LOG_FUNCTION(this << csgId);
    m_csgId = csgId;
    m_asSapProvider->SetCsgWhiteList(csgId);
}

uint32_t
EpcUeNas::GetCsgId() const
{
    NS_LOG_FUNCTION(this);
    return m_csgId;
}

void
EpcUeNas::SetAsSapProvider(LteAsSapProvider* s)
{
    NS_LOG_FUNCTION(this << s);
    m_asSapProvider = s;
}

LteAsSapUser*
EpcUeNas::GetAsSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_asSapUser;
}

void
EpcUeNas::SetForwardUpCallback(Callback<void, Ptr<Packet>> cb)
{
    NS_LOG_FUNCTION(this);
    m_forwardUpCallback = cb;
}

void
EpcUeNas::StartCellSelection(uint32_t dlEarfcn)
{
    NS_LOG_FUNCTION(this << dlEarfcn);
    m_asSapProvider->StartCellSelection(dlEarfcn);
}

void
EpcUeNas::Connect()
{
    NS_LOG_FUNCTION(this);

    // tell RRC to go into connected mode
    m_asSapProvider->Connect();
}

void
EpcUeNas::Connect(uint16_t cellId, uint32_t dlEarfcn)
{
    NS_LOG_FUNCTION(this << cellId << dlEarfcn);

    // force the UE RRC to be camped on a specific eNB
    m_asSapProvider->ForceCampedOnEnb(cellId, dlEarfcn);

    // tell RRC to go into connected mode
    m_asSapProvider->Connect();
}

void
EpcUeNas::Disconnect()
{
    NS_LOG_FUNCTION(this);
    SwitchToState(OFF);
    m_asSapProvider->Disconnect();
}

void
EpcUeNas::ActivateEpsBearer(EpsBearer bearer, Ptr<EpcTft> tft)
{
    NS_LOG_FUNCTION(this);
    switch (m_state)
    {
    case ACTIVE:
        NS_FATAL_ERROR("the necessary NAS signaling to activate a bearer after the initial context "
                       "has already been setup is not implemented");
        break;

    default:
        BearerToBeActivated btba;
        btba.bearer = bearer;
        btba.tft = tft;
        m_bearersToBeActivatedList.push_back(btba);
        m_bearersToBeActivatedListForReconnection.push_back(btba);
        break;
    }
}

bool
EpcUeNas::Send(Ptr<Packet> packet, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << packet << protocolNumber);

    switch (m_state)
    {
    case ACTIVE: {
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
            // First Check if there is any sidelink bearer for the destination
            // otherwise it may use the default bearer, if exists
            for (auto it = m_slBearersActivatedList.begin(); it != m_slBearersActivatedList.end();
                 it++)
            {
                if ((*it)->Matches(ipv4Header.GetDestination(), remotePort))
                {
                    // Found sidelink
                    NS_LOG_INFO("Found matching TFT for "
                                << ipv4Header.GetDestination() << ":" << remotePort
                                << " with dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                    m_asSapProvider->SendSidelinkData(packet,
                                                      (*it)->GetSidelinkInfo().m_dstL2Id,
                                                      (*it)->GetSidelinkInfo().m_lcId);
                    return true;
                }
                else
                {
                    NS_LOG_DEBUG("TFT is not a match for "
                                 << ipv4Header.GetDestination() << ":" << remotePort
                                 << " ; bearer has dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                 << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                }
            }
            // check if pending
            for (auto it = m_pendingSlBearersList.begin(); it != m_pendingSlBearersList.end(); it++)
            {
                if ((*it)->Matches(ipv4Header.GetDestination(), remotePort))
                {
                    NS_LOG_WARN("Matching sidelink bearer still pending, discarding packet");
                    // TODO: Add drop trace?
                    return false;
                }
                else
                {
                    NS_LOG_DEBUG("Pending bearer is not a match for "
                                 << ipv4Header.GetDestination() << ":" << remotePort
                                 << " ; bearer has dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                 << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                }
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

            for (auto it = m_slBearersActivatedList.begin(); it != m_slBearersActivatedList.end();
                 it++)
            {
                if ((*it)->Matches(ipv6Header.GetDestination(), remotePort))
                {
                    // Found sidelink
                    NS_LOG_INFO("Found matching TFT for "
                                << ipv6Header.GetDestination() << ":" << remotePort
                                << " with dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                    m_asSapProvider->SendSidelinkData(packet,
                                                      (*it)->GetSidelinkInfo().m_dstL2Id,
                                                      (*it)->GetSidelinkInfo().m_lcId);
                    return true;
                }
                else
                {
                    NS_LOG_DEBUG("TFT is not a match for "
                                 << ipv6Header.GetDestination() << ":" << remotePort
                                 << " with dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                 << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                }
            }
            // check if pending
            for (auto it = m_pendingSlBearersList.begin(); it != m_pendingSlBearersList.end(); it++)
            {
                if ((*it)->Matches(ipv6Header.GetDestination(), remotePort))
                {
                    NS_LOG_WARN("Matching sidelink bearer still pending, discarding packet");
                    // TODO: Add drop trace?
                    return false;
                }
            }
        }
        // No sidelink found
        NS_LOG_DEBUG("No sidelink bearer found. Use uplink");
        uint32_t id = m_tftClassifier.Classify(packet, EpcTft::UPLINK, protocolNumber);
        NS_ASSERT((id & 0xFFFFFF00) == 0);
        auto bid = (uint8_t)(id & 0x000000FF);
        if (bid == 0)
        {
            // TODO: Add drop trace?
            return false;
        }
        else
        {
            m_asSapProvider->SendData(packet, bid);
            return true;
        }
    }
    break;
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
            for (auto it = m_slBearersActivatedList.begin(); it != m_slBearersActivatedList.end();
                 it++)
            {
                if ((*it)->Matches(ipv4Header.GetDestination(), remotePort))
                {
                    // Found sidelink
                    NS_LOG_INFO("Found matching TFT for "
                                << ipv4Header.GetDestination() << ":" << remotePort
                                << " with dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                    m_asSapProvider->SendSidelinkData(packet,
                                                      (*it)->GetSidelinkInfo().m_dstL2Id,
                                                      (*it)->GetSidelinkInfo().m_lcId);
                    return true;
                }
                else
                {
                    NS_LOG_DEBUG("Did not find matching TFT for "
                                 << ipv4Header.GetDestination() << ":" << remotePort
                                 << " bearer dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                 << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                }
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

            for (auto it = m_slBearersActivatedList.begin(); it != m_slBearersActivatedList.end();
                 it++)
            {
                if ((*it)->Matches(ipv6Header.GetDestination(), remotePort))
                {
                    NS_LOG_INFO("Found matching TFT for "
                                << ipv6Header.GetDestination() << ":" << remotePort
                                << " with dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                    // Found sidelink
                    m_asSapProvider->SendSidelinkData(packet,
                                                      (*it)->GetSidelinkInfo().m_dstL2Id,
                                                      (*it)->GetSidelinkInfo().m_lcId);
                    return true;
                }
                else
                {
                    NS_LOG_DEBUG("Did not find matching TFT for "
                                 << ipv6Header.GetDestination() << ":" << remotePort
                                 << " bearer dstL2Id " << (*it)->GetSidelinkInfo().m_dstL2Id
                                 << " LC ID " << +(*it)->GetSidelinkInfo().m_lcId);
                }
            }
        }
    }
    default:
        NS_LOG_WARN("NAS neither OFF nor ACTIVE, or Sidelink bearer not found, discarding packet");
        // TODO:  Add drop trace?
        return false;
    }
    // TODO: Check that all paths that return false also hit a drop trace somehow
}

void
EpcUeNas::DoNotifyConnectionSuccessful()
{
    NS_LOG_FUNCTION(this);

    SwitchToState(ACTIVE); // will eventually activate dedicated bearers
}

void
EpcUeNas::DoNotifyConnectionFailed()
{
    NS_LOG_FUNCTION(this);

    // immediately retry the connection
    Simulator::ScheduleNow(&LteAsSapProvider::Connect, m_asSapProvider);
}

void
EpcUeNas::DoRecvData(Ptr<Packet> packet)
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
EpcUeNas::DoNotifyConnectionReleased()
{
    NS_LOG_FUNCTION(this);
    // remove tfts
    while (m_bidCounter > 0)
    {
        m_tftClassifier.Delete(m_bidCounter);
        m_bidCounter--;
    }
    // restore the bearer list to be activated for the next RRC connection
    m_bearersToBeActivatedList = m_bearersToBeActivatedListForReconnection;

    Disconnect();
}

void
EpcUeNas::DoActivateEpsBearer(EpsBearer bearer, Ptr<EpcTft> tft)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(m_bidCounter < 11, "cannot have more than 11 EPS bearers");
    uint8_t bid = ++m_bidCounter;
    m_tftClassifier.Add(tft, bid);
}

EpcUeNas::State
EpcUeNas::GetState() const
{
    NS_LOG_FUNCTION(this);
    return m_state;
}

void
EpcUeNas::SwitchToState(State newState)
{
    NS_LOG_FUNCTION(this << ToString(newState));
    State oldState = m_state;
    m_state = newState;
    NS_LOG_INFO("IMSI " << m_imsi << " NAS " << ToString(oldState) << " --> "
                        << ToString(newState));
    m_stateTransitionCallback(oldState, newState);

    // actions to be done when entering a new state:
    switch (m_state)
    {
    case ACTIVE:
        for (auto it = m_bearersToBeActivatedList.begin(); it != m_bearersToBeActivatedList.end();
             m_bearersToBeActivatedList.erase(it++))
        {
            DoActivateEpsBearer(it->bearer, it->tft);
        }
        break;

    default:
        break;
    }
}

void
EpcUeNas::ActivateNrSlBearer(Ptr<LteSlTft> tft)
{
    NS_LOG_FUNCTION(this << tft->GetSidelinkInfo().m_lcId << tft->GetSidelinkInfo().m_dynamic);
    // regardless of the state we need to request RRC to setup the bearer
    // for in coverage case, it will trigger communication with the gNodeb
    // for out of coverage, it will trigger the use of preconfiguration
    m_pendingSlBearersList.push_back(tft);
    m_asSapProvider->ActivateNrSlRadioBearer(tft->IsTransmit(),
                                             tft->IsReceive(),
                                             tft->GetSidelinkInfo());
}

void
EpcUeNas::DoNotifyNrSlRadioBearerActivated(const struct SidelinkInfo& slInfo)
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
            (*it)->SetSidelinkInfoLcId(slInfo.m_lcId);
            m_slBearersActivatedList.push_back(*it);
            it = m_pendingSlBearersList.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void
EpcUeNas::DeleteNrSlBearer(Ptr<LteSlTft> tft)
{
    NS_LOG_FUNCTION(this);
    m_asSapProvider->DeleteNrSlRadioBearer(tft->IsTransmit(),
                                           tft->IsReceive(),
                                           tft->GetSidelinkInfo());
}

void
EpcUeNas::DoNotifyNrSlRadioBearerRemoved(const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this << slInfo.m_dstL2Id << +slInfo.m_lcId);

    std::list<Ptr<LteSlTft>>::iterator it = m_slBearersActivatedList.begin();
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
}

NrSlUeSvcNasSapProvider*
EpcUeNas::GetNrSlUeSvcNasSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlUeSvcNasSapProvider;
}

void
EpcUeNas::SetNrSlUeSvcNasSapUser(NrSlUeSvcNasSapUser* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeSvcNasSapUser = s;
}

void
EpcUeNas::DoActivateSvcNrSlDataRadioBearer(Ptr<LteSlTft> tft)
{
    NS_LOG_FUNCTION(this);

    m_pendingSlBearersList.push_back(tft);
    m_asSapProvider->ActivateNrSlRadioBearer(tft->IsTransmit(),
                                             tft->IsReceive(),
                                             tft->GetSidelinkInfo());
}

void
EpcUeNas::DoDeleteSvcNrSlDataRadioBearer(Ptr<LteSlTft> tft)
{
    NS_LOG_FUNCTION(this);
    m_asSapProvider->DeleteNrSlRadioBearer(tft->IsTransmit(),
                                           tft->IsReceive(),
                                           tft->GetSidelinkInfo());
}

void
EpcUeNas::DoConfigureNrSlDataRadioBearersForU2nRelay(
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
            std::list<EpcTft::PacketFilter> pfList = itBe->tft->GetPacketFilters();
            for (std::list<EpcTft::PacketFilter>::iterator itPf = pfList.begin();
                 itPf != pfList.end();
                 ++itPf)
            {
                // Currently, we can only reconfigure UL DRBs with configured remoteAddress
                // in the packet filter (as LteSlTft do the match by destination address)
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
                    auto slTft = Create<LteSlTft>(LteSlTft::Direction::TRANSMIT,
                                                  itPf->remoteAddress,
                                                  slInfo);
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
                "Did you forget to configure the remoteAddress on the UL EpcTft::PacketFilter?");
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
                Create<LteSlTft>(LteSlTft::Direction::TRANSMIT, ipInfo.peerIpv4Addr, slInfo);
            DoActivateSvcNrSlDataRadioBearer(slTft);
        }
        break;
    default:
        NS_FATAL_ERROR("Invalid role " << role);
    }
}

void
EpcUeNas::DoRemoveNrSlDataRadioBearersForU2nRelay(uint32_t peerL2Id,
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
    auto slTft = Create<LteSlTft>(LteSlTft::Direction::TRANSMIT, ipInfo.peerIpv4Addr, slInfo);
    DoDeleteSvcNrSlDataRadioBearer(slTft);
}

void
EpcUeNas::ClassifyRecvPacketForU2nRelay(Ptr<Packet> packet)
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
    for (std::list<Ptr<LteSlTft>>::iterator it = m_slBearersActivatedList.begin();
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
            m_asSapProvider->SendSidelinkData(packet,
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

} // namespace ns3
