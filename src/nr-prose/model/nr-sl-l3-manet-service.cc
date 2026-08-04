//
// SPDX-License-Identifier: NIST-Software
//

#include "nr-sl-l3-manet-service.h"

#include "nr-sl-pc5-signalling-header.h"
#include "nr-sl-prose-tag.h"
#include "nr-sl-ue-prose-relay-selection-algorithm.h"
#include "nr-sl-ue-prose.h"

#include "ns3/abort.h"
#include "ns3/boolean.h"
#include "ns3/fatal-error.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv4-list-routing.h"
#include "ns3/log.h"
#include "ns3/nhdp-client.h"
#include "ns3/node.h"
#include "ns3/nr-point-to-point-epc-helper.h"
#include "ns3/nr-radio-bearer-info.h"
#include "ns3/nr-rrc-sap.h"
#include "ns3/nr-sl-epc-ue-nas.h"
#include "ns3/nr-ue-net-device.h"
#include "ns3/nr-ue-rrc.h"
#include "ns3/object-factory.h"
#include "ns3/object-map.h"
#include "ns3/olsr-routing-protocol.h"
#include "ns3/olsrv2-routing-protocol.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"

#include <algorithm>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlL3ManetService");
NS_OBJECT_ENSURE_REGISTERED(NrSlL3ManetService);

static const uint32_t L3_MANET_APP_CODE = 1000; //!< Arbitrary app code
static const uint32_t L3_MANET_DST_L2_ID = 255; //!< Destination L2 ID
static const bool INITIATING = true;            //!< Initiating direct link

TypeId
NrSlL3ManetService::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::NrSlL3ManetService")
            .SetParent<NrSlUeService>()
            .AddConstructor<NrSlL3ManetService>()
            .AddAttribute("DiscoveryInterval",
                          "Discovery interval",
                          TimeValue(Seconds(1)),
                          MakeTimeAccessor(&NrSlL3ManetService::m_discoveryInterval),
                          MakeTimeChecker())
            .AddAttribute("DiscoveryStart",
                          "Discovery start time",
                          TimeValue(),
                          MakeTimeAccessor(&NrSlL3ManetService::m_discoveryStartTime),
                          MakeTimeChecker())
            .AddAttribute("UseSdRsrpForRlf",
                          "Indicates if SD-RSRP should be used for RLF detection",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrSlL3ManetService::m_useSdRsrpForRlf),
                          MakeBooleanChecker())
            .AddAttribute("UseSlRsrpForRlf",
                          "Indicates if SL-RSRP should be used for RLF detection",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrSlL3ManetService::m_useSlRsrpForRlf),
                          MakeBooleanChecker())
            .AddTraceSource(
                "PC5SignallingPacketTrace",
                "Trace fired upon transmission and reception of PC5 Signalling messages",
                MakeTraceSourceAccessor(&NrSlL3ManetService::m_pc5SignallingPacketTrace),
                "ns3::NrSlL3ManetService::PC5SignallingPacketTracedCallback")
            .AddTraceSource("DiscoveryTrace",
                            "Trace to track the transmission/reception of discovery messages",
                            MakeTraceSourceAccessor(&NrSlL3ManetService::m_discoveryTrace),
                            "ns3::NrSlL3ManetService::DiscoveryTracedCallback")
            .AddTraceSource(
                "DirectLinkEstablished",
                "Traces when a direct link is established.",
                MakeTraceSourceAccessor(&NrSlL3ManetService::m_directLinkEstablishedTrace),
                "ns3::NrSlL3ManetService::DirectLinkTracedCallback")
            .AddTraceSource(
                "DirectLinkReleasing",
                "Traces when a direct link is released.",
                MakeTraceSourceAccessor(&NrSlL3ManetService::m_directLinkReleasingTrace),
                "ns3::NrSlL3ManetService::DirectLinkTracedCallback")
            .AddTraceSource("RadioLinkFailure",
                            "Traces when RLF is detected.",
                            MakeTraceSourceAccessor(&NrSlL3ManetService::m_rlfTrace),
                            "ns3::NrSlL3ManetService::RadioLinkFailureTracedCallback")
            .AddTraceSource("L3SdRsrpReport",
                            "Trace the report of L3 SD-RSRP measurements from the RRC",
                            MakeTraceSourceAccessor(&NrSlL3ManetService::m_l3SdRsrpReportTrace),
                            "ns3::NrSlL3ManetService::L3SdRsrpReportTracedCallback");
    return tid;
}

NrSlL3ManetService::NrSlL3ManetService()
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeSvcRrcSapUser = new MemberNrSlUeSvcRrcSapUser<NrSlL3ManetService>(this);
    m_nrSlUeSvcNasSapUser = new MemberNrSlUeSvcNasSapUser<NrSlL3ManetService>(this);
    m_nrSlUeProseDirLnkSapUser = new MemberNrSlUeProseDirLnkSapUser<NrSlL3ManetService>(this);
    m_imsi = 0;
    m_l2Id = 0;

    m_slSrbSlInfo = {
        SidelinkInfo::CastType::Unicast, // m_castType
        0,                               // m_srcL2Id
        0,                               // m_dstL2Id
        false,                           // m_harqEnabled
        MilliSeconds(20),                // m_pdb
        true,                            // m_dynamic
        MilliSeconds(0),                 // m_rri
        0,                               // m_lcId
        1                                // m_priority
    };
    m_slDrbSlInfo = {
        SidelinkInfo::CastType::Unicast, // m_castType
        0,                               // m_srcL2Id
        0,                               // m_dstL2Id
        false,                           // m_harqEnabled
        MilliSeconds(20),                // m_pdb
        true,                            // m_dynamic
        MilliSeconds(0),                 // m_rri
        0,                               // m_lcId
        0                                // m_priority
    };
}

NrSlL3ManetService::~NrSlL3ManetService(void)
{
    NS_LOG_FUNCTION(this);
}

void
NrSlL3ManetService::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_nrSlUeSvcRrcSapUser;
    delete m_nrSlUeSvcNasSapUser;
    delete m_nrSlUeProseDirLnkSapUser;
    m_ueDevice = nullptr;
}

void
NrSlL3ManetService::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    m_ueDevice = GetObject<NetDevice>();
    NS_ASSERT_MSG(m_ueDevice, "Not aggregated to a NetDevice");
    m_imsi = m_ueDevice->GetNode()->GetId();
    m_l2Id = m_ueDevice->GetNode()->GetId();
    // XXX make the below into a jitter variable
    Time start = m_discoveryStartTime + MilliSeconds(100 * m_imsi);
    uint32_t appCode = L3_MANET_APP_CODE;
    uint32_t dstL2Id = L3_MANET_DST_L2_ID;
    Simulator::ScheduleWithContext(m_imsi,
                                   start,
                                   &NrSlL3ManetService::AddDiscoveryApp,
                                   this,
                                   appCode,
                                   dstL2Id,
                                   NrSlL3ManetService::DiscoveryRole::Announcing);
    Simulator::ScheduleWithContext(m_imsi,
                                   start,
                                   &NrSlL3ManetService::AddDiscoveryApp,
                                   this,
                                   appCode,
                                   dstL2Id,
                                   NrSlL3ManetService::DiscoveryRole::Monitoring);
    // Hook the OLSR routing protocol traces
    auto routingProtocol = m_ueDevice->GetNode()->GetObject<Ipv4>()->GetRoutingProtocol();
    NS_ASSERT_MSG(routingProtocol, "Routing protocol not found");
    Ptr<olsrv2::RoutingProtocol> olsr = routingProtocol->GetObject<olsrv2::RoutingProtocol>();
    // OLSR can either be by itself, or a member of the Ipv4ListRoutingProtocol
    if (!olsr)
    {
        auto listRouting = routingProtocol->GetObject<Ipv4ListRouting>();
        if (listRouting)
        {
            for (uint32_t i = 0; i < listRouting->GetNRoutingProtocols(); i++)
            {
                int16_t priority;
                olsr = listRouting->GetRoutingProtocol(i, priority)
                           ->GetObject<olsrv2::RoutingProtocol>();
                if (olsr)
                {
                    break;
                }
            }
        }
    }
    NS_ABORT_MSG_UNLESS(olsr, "OLSRv2 routing protocol not found");
    olsr->TraceConnectWithoutContext("AddRoute",
                                     MakeCallback(&NrSlL3ManetService::OlsrAddRoute, this));
    olsr->TraceConnectWithoutContext("RemoveRoute",
                                     MakeCallback(&NrSlL3ManetService::OlsrRemoveRoute, this));

    for (uint32_t i = 0; i < m_ueDevice->GetNode()->GetNApplications(); i++)
    {
        auto nhdpClient = m_ueDevice->GetNode()->GetApplication(i)->GetObject<nhdp::NhdpClient>();
        if (nhdpClient)
        {
            m_nhdp = nhdpClient;
            TraceConnectWithoutContext(
                "DirectLinkEstablished",
                MakeCallback(&nhdp::NhdpClient::HandleDirectLinkEstablishedTrace, m_nhdp));
            TraceConnectWithoutContext(
                "DirectLinkReleasing",
                MakeCallback(&nhdp::NhdpClient::HandleDirectLinkReleasingTrace, m_nhdp));
        }
    }
}

NrSlUeSvcRrcSapUser*
NrSlL3ManetService::GetNrSlUeSvcRrcSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlUeSvcRrcSapUser;
}

void
NrSlL3ManetService::SetNrSlUeSvcRrcSapProvider(NrSlUeSvcRrcSapProvider* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeSvcRrcSapProvider = s;
}

NrSlUeSvcNasSapUser*
NrSlL3ManetService::GetNrSlUeSvcNasSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlUeSvcNasSapUser;
}

void
NrSlL3ManetService::SetNrSlUeSvcNasSapProvider(NrSlUeSvcNasSapProvider* s)
{
    NS_LOG_FUNCTION(this);
    m_nrSlUeSvcNasSapProvider = s;
}

void
NrSlL3ManetService::SetDiscoveryInterval(Time val)
{
    NS_LOG_FUNCTION(this << val);
    m_discoveryInterval = val;
}

void
NrSlL3ManetService::DoNotifySvcNrSlDataRadioBearerActivated(uint32_t peerL2Id)
{
    NS_LOG_FUNCTION(this << peerL2Id);

    // Get link
    auto it = m_unicastDirectLinks.find(peerL2Id);
    if (it == m_unicastDirectLinks.end())
    {
        NS_LOG_DEBUG("Could not find the direct link");
    }
    else
    {
        NS_LOG_INFO("Moving unicast direct link to " << it->first << " from pending to active");
        it->second->m_hasPendingSlDrb = false;
        it->second->m_hasActiveSlDrb = true;
    }
}

void
NrSlL3ManetService::DoNotifySvcNrSlDataRadioBearerRemoved(uint32_t peerL2Id)
{
    NS_LOG_FUNCTION(this << peerL2Id);

    // Get link
    auto it = m_unicastDirectLinks.find(peerL2Id);
    if (it == m_unicastDirectLinks.end())
    {
        NS_FATAL_ERROR("Could not find the direct link");
    }
    else
    {
        it->second->m_hasActiveSlDrb = false;
    }
}

void
NrSlL3ManetService::DoNotifyNrSlHarqProcessMaxTransmissionsWithNoFeedback(uint32_t peerL2Id)
{
    NS_LOG_FUNCTION(this << peerL2Id);

    auto it = m_unicastDirectLinks.find(peerL2Id);

    NS_ABORT_MSG_UNLESS(it != m_unicastDirectLinks.end(),
                        "Could not find direct link to " << peerL2Id);

    // Initiate release procedure with current relay/link
    // According to 3GPP TS 24.554, the cause of link release can be:
    //   #1 direct communication to the target UE not allowed;
    //   #2 direct communication to the target UE no longer needed;
    //   #4 direct connection is not available anymore;
    //   #5 lack of resources for PC5 unicast link;
    //   #13 congestion situation; or
    //   #111   protocol error, unspecified.
    // In this case, we will go with cause #4

    if (it->second->m_link->GetState() == NrSlUeProseDirectLink::ESTABLISHED)
    {
        it->second->m_link->StartConnectionRelease(4);
    }

    m_nhdp->HandleLinkFailure(it->second->m_ipInfo.peerIpv4Addr);

    m_rlfTrace(m_l2Id, peerL2Id, RlfEvent::Harq);
}

void
NrSlL3ManetService::ConfigureUnicast()
{
    NS_LOG_FUNCTION(this);

    // Tell the RRC to inform the MAC to monitor the UE's own L2Id
    m_nrSlUeSvcRrcSapProvider->MonitorSelfL2Id();
}

void
NrSlL3ManetService::ConfigureL2IdMonitoringForDiscovery(uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << dstL2Id);

    // Tell the RRC to inform the MAC to monitor the UE's own L2Id
    m_nrSlUeSvcRrcSapProvider->MonitorSelfL2Id();

    // Tell the RRC to inform the MAC to monitor the UE's L2Id
    m_nrSlUeSvcRrcSapProvider->MonitorL2Id(dstL2Id);
}

Ptr<NrSlUeProseDirectLink>
NrSlL3ManetService::GetDirectLink(uint32_t appCode, uint32_t peerL2Id)
{
    auto it = m_unicastDirectLinks.find(peerL2Id);
    if (it != m_unicastDirectLinks.end())
    {
        return it->second->m_link;
    }
    else
    {
        return nullptr;
    }
}

void
NrSlL3ManetService::AddDirectLinkConnection(uint32_t selfL2Id,
                                            Ipv4Address selfIp,
                                            uint32_t peerL2Id,
                                            Ipv4Address peerIp,
                                            bool isInitiating,
                                            uint32_t applicationCode,
                                            const struct SidelinkInfo& slInfo)
{
    NS_LOG_FUNCTION(this << selfL2Id << selfIp << peerL2Id << peerIp << isInitiating
                         << applicationCode);
    NS_ASSERT_MSG(selfL2Id == m_l2Id, "L2Id mismatch.");
    bool isRelayConn = false;

    auto it = m_unicastDirectLinks.find(peerL2Id);

    // Check if a direct link connection is ongoing
    // If a direct link between the pair of UEs already exists, it gets overwritten with the new
    // parameters.
    //  - On the remote side: This may happen in certain cases where the ReleaseRequest or
    //  ReleaseAccept message gets lost
    // and eventually retransmitted (once the related timer runs out). However, before this process
    // concludes, the same relay is available again and the relay selection procedure is triggered,
    // which calls for the reset of the existing (incomplete) link and the starting of a new
    // connection.
    //  - On the relay side: The relay does not get an acknowledgement of the ReleaseAccept. So the
    //  link does
    // not get deleted from the m_unicastDirectLinks map. If the remote reconsiders connecting to
    // this relay after a while, a reset and an update of the pre-existing old link would take
    // place.
    if (it != m_unicastDirectLinks.end())
    {
        NS_LOG_INFO("Direct link " << selfL2Id << " <--> " << peerL2Id
                                   << " already exists in state " << it->second->m_link->GetState()
                                   << "; updating");

        // update link parameters
        it->second->m_link
            ->SetParameters(selfL2Id, peerL2Id, isInitiating, isRelayConn, applicationCode, selfIp);
        // update map element parameters
        it->second->m_ipInfo.selfIpv4Addr = selfIp;
        it->second->m_ipInfo.peerIpv4Addr = peerIp;
    }
    else
    {
        NS_LOG_INFO("New direct link from " << selfL2Id << " to " << peerL2Id);

        // Create context to maintain here in the ProSe layer
        Ptr<NrSlUeProseDirLinkContext> context = CreateObject<NrSlUeProseDirLinkContext>();

        // Create Direct Link instance and set parameters
        Ptr<NrSlUeProseDirectLink> link = CreateObject<NrSlUeProseDirectLink>();
        link->SetParameters(selfL2Id, peerL2Id, isInitiating, isRelayConn, applicationCode, selfIp);
        link->TraceConnectWithoutContext(
            "KeepAliveFailure",
            MakeCallback(&NrSlL3ManetService::NotifyKeepAliveFailure, this));

        // Connect SAPs
        link->SetNrSlUeProseDirLnkSapUser(GetNrSlUeProseDirLnkSapUser());

        context->m_link = link;
        context->m_nrSlUeProseDirLnkSapProvider = link->GetNrSlUeProseDirLnkSapProvider();
        context->m_ipInfo.selfIpv4Addr = selfIp;
        context->m_ipInfo.peerIpv4Addr = peerIp;
        context->m_slInfo = slInfo;

        // Store context
        m_unicastDirectLinks.insert(
            std::pair<uint32_t, Ptr<NrSlUeProseDirLinkContext>>(peerL2Id, context));

        // Initiate connection establishment procedure if this UE is the initiating UE
        if (isInitiating)
        {
            context->m_link->StartConnectionEstablishment();
        }
    }
}

NrSlUeProseDirLnkSapUser*
NrSlL3ManetService::GetNrSlUeProseDirLnkSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_nrSlUeProseDirLnkSapUser;
}

void
NrSlL3ManetService::DoSendNrSlPc5SMessage(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this << packet << dstL2Id << lcId);

    // Activate the corresponding SL-SRB for the logical channel, if not active
    SidelinkInfo slSrbSlInfo = m_slSrbSlInfo;
    slSrbSlInfo.m_srcL2Id = m_l2Id;
    slSrbSlInfo.m_dstL2Id = dstL2Id;
    slSrbSlInfo.m_lcId = lcId;
    auto it = m_activeSlSrbs.find(dstL2Id);
    if (it == m_activeSlSrbs.end()) // First SL-SRB for this destination
    {
        NS_LOG_INFO(
            "First SL-SRB for this destination; instruct the RRC to activate the SL-SRB to dstL2Id "
            << dstL2Id);
        m_nrSlUeSvcRrcSapProvider->ActivateNrSlSignallingRadioBearer(slSrbSlInfo);

        // Keep track of it
        std::bitset<4> activeLcs;
        activeLcs[lcId] = true;
        m_activeSlSrbs.insert(std::pair<uint32_t, std::bitset<4>>(dstL2Id, activeLcs));
    }
    else
    {
        if (it->second[lcId] == false) // First SL-SRB for this lcId
        {
            NS_LOG_INFO(
                "First SL-SRB for this lcId; instruct the RRC to activate the SL-SRB to dstL2Id "
                << dstL2Id << " lcId " << +lcId);
            m_nrSlUeSvcRrcSapProvider->ActivateNrSlSignallingRadioBearer(slSrbSlInfo);
            // Keep track of it
            it->second[lcId] = true;
        }
    }
    m_pc5SignallingPacketTrace(m_l2Id, dstL2Id, true, packet);

    // Pass the message to the RRC
    m_nrSlUeSvcRrcSapProvider->SendNrSlSignalling(packet, dstL2Id, lcId);
}

void
NrSlL3ManetService::AddDiscoveryApp(uint32_t appCode, uint32_t dstL2Id, DiscoveryRole role)
{
    NS_LOG_FUNCTION(this << appCode << dstL2Id << role);

    NS_LOG_INFO("Adding app code " << appCode << " dstL2Id " << dstL2Id << " role " << role);
    DiscoveryInfo info;
    info.model = DiscoveryModel::ModelA;
    info.role = role;
    info.appCode = appCode;
    info.dstL2Id = dstL2Id;
    m_discoverySet.insert(info);
    for (auto it = m_discoverySet.begin(); it != m_discoverySet.end(); ++it)

        if (role == Announcing || role == Discoverer)
        {
            SendDiscovery(appCode, dstL2Id);
        }
    // It instructs the MAC layer (and PHY therefore) to monitor packets directed the UE's own and
    // other Layer 2 IDs
    ConfigureL2IdMonitoringForDiscovery(dstL2Id);
}

void
NrSlL3ManetService::RemoveDiscoveryApp(uint32_t appCode, DiscoveryRole role)
{
    NS_LOG_FUNCTION(this << appCode << role);
    for (auto it = m_discoverySet.begin(); it != m_discoverySet.end();)
    {
        if ((*it).role == role && (*it).appCode == appCode)
        {
            NS_LOG_DEBUG("Removing app code " << appCode << " role " << role);
            it = m_discoverySet.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

bool
NrSlL3ManetService::IsMonitoringApp(uint8_t msgType, uint32_t appCode)
{
    NS_LOG_FUNCTION(this << msgType << appCode);
    // the device is interested in announcement if monitoring, in request if acting as discoveree,
    // and in response if acting as discoverer
    for (auto it = m_discoverySet.begin(); it != m_discoverySet.end(); ++it)
    {
        if ((*it).appCode == appCode && (*it).role == Monitoring &&
            msgType == NrSlDiscoveryHeader::DISC_OPEN_ANNOUNCEMENT)
        {
            return true;
        }
    }
    return false;
}

void
NrSlL3ManetService::SendDiscovery(uint32_t appCode, uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << appCode << dstL2Id);

    for (auto it = m_discoverySet.begin(); it != m_discoverySet.end(); ++it)
    {
        if ((*it).role != Announcing && (*it).role != Discoverer)
        {
            continue;
        }
        if ((*it).appCode == appCode)
        {
            NrSlDiscoveryHeader discHeader;

            if ((*it).role == Announcing)
            {
                discHeader.SetOpenDiscoveryAnnounceParameters(appCode);

                // reschedule
                Simulator::Schedule(m_discoveryInterval,
                                    &NrSlL3ManetService::SendDiscovery,
                                    this,
                                    appCode,
                                    dstL2Id);
            }
            else if ((*it).role == Discoverer)
            {
                discHeader.SetRestrictedDiscoveryQueryParameters(appCode);

                // reschedule
                Simulator::Schedule(m_discoveryInterval,
                                    &NrSlL3ManetService::SendDiscovery,
                                    this,
                                    appCode,
                                    dstL2Id);
            }

            else if ((*it).role == Discoveree)
            {
                discHeader.SetRestrictedDiscoveryResponseParameters(appCode);

                // no reschedule
            }
            else
            {
                NS_FATAL_ERROR("Invalid role " << +(*it).role);
            }

            // build message to transmit
            Ptr<Packet> discoveryPacket = Create<Packet>();
            discoveryPacket->AddHeader(discHeader);

            NrSlProseDiscoveryTag discoveryTag;
            discoveryPacket->AddByteTag(discoveryTag);

            DoSendNrSlDiscovery(discoveryPacket, dstL2Id);
            m_discoveryTrace(m_l2Id, dstL2Id, true, discHeader);
        }
    }
}

void
NrSlL3ManetService::DoSendNrSlDiscovery(Ptr<Packet> packet, uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << packet << dstL2Id);

    // Activate the corresponding SL Discovery RB for the logical channel, if not active
    auto it = std::find(m_activeSlDiscoveryRbs.begin(), m_activeSlDiscoveryRbs.end(), dstL2Id);
    if (it == m_activeSlDiscoveryRbs.end()) // First SL Discovery RB for this destination
    {
        // Instruct the RRC to activate the SL Disocvery RB
        NS_LOG_INFO("Activating SL Discovery RB from my L2ID " << m_l2Id << " to dstL2Id "
                                                               << dstL2Id);
        m_nrSlUeSvcRrcSapProvider->ActivateNrSlDiscoveryRadioBearer(dstL2Id);

        // Keep track of it
        m_activeSlDiscoveryRbs.push_front(dstL2Id);
    }

    // Pass the message to the RRC
    NS_LOG_INFO("Discovery message sent from my L2ID " << m_l2Id << " to " << dstL2Id);
    m_nrSlUeSvcRrcSapProvider->SendNrSlDiscovery(packet, dstL2Id);
}

void
NrSlL3ManetService::DoReceiveNrSlDiscovery(Ptr<Packet> packet, uint32_t srcL2Id)
{
    NS_LOG_FUNCTION(this << packet << srcL2Id);
    NrSlDiscoveryHeader discHeader;
    packet->RemoveHeader(discHeader);

    uint8_t msgType = discHeader.GetDiscoveryMsgType();

    // Discovery
    if (msgType == NrSlDiscoveryHeader::DISC_OPEN_ANNOUNCEMENT ||
        msgType == NrSlDiscoveryHeader::DISC_RESTRICTED_QUERY ||
        msgType == NrSlDiscoveryHeader::DISC_RESTRICTED_RESPONSE)
    {
        uint32_t appCode = discHeader.GetApplicationCode();

        // open or restricted announcement
        if (IsMonitoringApp(msgType, appCode))
        {
            // Check if this is a request I am interested in
            for (auto it = m_discoverySet.begin(); it != m_discoverySet.end(); ++it)
            {
                if ((*it).role != DiscoveryRole::Monitoring)
                {
                    continue;
                }
                if ((*it).appCode == appCode)
                {
                    NS_LOG_INFO("Discovery message received by " << m_l2Id << " from " << srcL2Id);
                    m_discoveryTrace(srcL2Id, m_l2Id, false, discHeader);
                    // Check if SD-RSRP measurements are being collected.
                    // If they are, do nothing because measurements will
                    // be assessed prior to link establishment. If they
                    // are NOT, initate the link now.
                    if (!m_nrSlUeSvcRrcSapProvider->IsUeSdRsrpMeasurementsEnabled())
                    {
                        InitiateLink(srcL2Id);
                    }
                    // check if this is a request message for an app for which this UE is a
                    // Discoveree
                    if (msgType == NrSlDiscoveryHeader::DISC_RESTRICTED_QUERY &&
                        (*it).role == Discoveree)
                    {
                        SendDiscovery(appCode, srcL2Id);
                    }
                }
            }
        }
    }
}

void
NrSlL3ManetService::DoReceiveNrSdRsrpMeasurements(uint32_t peerId, double value, bool eligible)
{
    NS_LOG_FUNCTION(this << peerId << value << eligible);
    m_l3SdRsrpReportTrace(m_l2Id, peerId, value, eligible);
    m_countNrSdRsrpMeas[peerId]++;

    // Evaluate threshold condition every four L3 SD-RSRP measurement reports.
    // This corresponds to T_evaluate = 16 in TS 38.133 Section 12.10, table 12.10.2-1 assuming that
    // the SD-RSRP measurement reports are received every 4 discovery periods (i.e., T_measure = 4)
    if (m_countNrSdRsrpMeas[peerId] == 4)
    {
        if (m_useSdRsrpForRlf && !eligible)
        {
            auto it = m_unicastDirectLinks.find(peerId);
            if (it != m_unicastDirectLinks.end())
            {
                if (it->second->m_link->GetState() == NrSlUeProseDirectLink::ESTABLISHED)
                {
                    NS_LOG_INFO("dstL2Id " << peerId << " with SD-RSRP " << value
                                           << "dBm is no longer eligible");
                    it->second->m_link->StartConnectionRelease(4);
                    m_nhdp->HandleLinkFailure(it->second->m_ipInfo.peerIpv4Addr);
                    m_rlfTrace(m_l2Id, peerId, RlfEvent::SdRsrp);
                }
            }
        }

        if (eligible)
        {
            InitiateLink(peerId);
        }
        m_countNrSdRsrpMeas[peerId] = 0; // Reset
    }
}

void
NrSlL3ManetService::DoReceiveNrSlRsrpMeasurements(uint32_t peerId, double value, bool eligible)
{
    NS_LOG_FUNCTION(this << peerId << value << eligible);

    if (m_useSlRsrpForRlf && !eligible)
    {
        auto it = m_unicastDirectLinks.find(peerId);
        if (it != m_unicastDirectLinks.end())
        {
            if (it->second->m_link->GetState() == NrSlUeProseDirectLink::ESTABLISHED)
            {
                NS_LOG_INFO("dstL2Id " << peerId << " with SL-RSRP " << value
                                       << "dBm is no longer eligible");
                it->second->m_link->StartConnectionRelease(4);
                m_nhdp->HandleLinkFailure(it->second->m_ipInfo.peerIpv4Addr);
                m_rlfTrace(m_l2Id, peerId, RlfEvent::SlRsrp);
            }
        }
    }
}

void
NrSlL3ManetService::DoReceiveNrSlSignalling(Ptr<Packet> packet, uint32_t srcL2Id)
{
    NS_LOG_FUNCTION(this);

    // If PC5-S for unicast communication:
    auto it = m_unicastDirectLinks.find(srcL2Id);
    if (it == m_unicastDirectLinks.end())
    {
        NS_LOG_DEBUG("Unrecognized peer L2 ID " << srcL2Id);

        // This will be the case e.g. when the Relay UE receives a request from the Remote UE
        // after the relay discovery procedure.
        // In that case, the Relay UE should:
        // 1. Create the corresponding context and direct link instance.
        // 2. Pass the packet to the corresponding direct link instance

        NrSlPc5SignallingMessageType header;
        packet->PeekHeader(header);
        uint8_t msgType = header.GetMessageType();

        if (msgType == NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentRequest)
        {
            ProseDirectLinkEstablishmentRequest reqHeader;
            packet->PeekHeader(reqHeader);
            auto applicationIds = reqHeader.GetProseApplicationIds();
            NS_ASSERT_MSG(applicationIds.size() == 1, "Expected application ID size to be 1");
            auto selfIp = GetIpv4Address();
            auto srcIp = GetIpv4AddressFromPeerL2Id(srcL2Id);
            uint32_t selfL2Id = m_imsi;
            NS_LOG_INFO("Initiating direct link to IP " << srcIp << " L2ID " << srcL2Id
                                                        << " from my IP " << selfIp);
            SidelinkInfo initSlInfo = m_slDrbSlInfo;
            initSlInfo.m_srcL2Id = m_l2Id;
            initSlInfo.m_dstL2Id = srcL2Id;
            AddDirectLinkConnection(selfL2Id, selfIp, srcL2Id, srcIp, INITIATING, 0, initSlInfo);
        }
    }
    else
    {
        NS_LOG_DEBUG("Direct link context found between " << m_l2Id << " and " << srcL2Id
                                                          << "; state "
                                                          << it->second->m_link->GetState());
        m_pc5SignallingPacketTrace(srcL2Id, m_l2Id, false, packet);

        // Pass the packet to the corresponding direct link instance
        it->second->m_nrSlUeProseDirLnkSapProvider->ReceiveNrSlPc5Message(packet);
    }
}

void
NrSlL3ManetService::DoNotifyDataReceived(uint32_t srcL2Id)
{
    NS_LOG_FUNCTION(this << srcL2Id);

    auto it = m_unicastDirectLinks.find(srcL2Id);
    NS_ABORT_MSG_IF(it == m_unicastDirectLinks.end(), "Could not find direct link");

    it->second->m_link->NotifyDataReceived();
}

bool
NrSlL3ManetService::DoConfirmSendRequest(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId)
{
    NS_LOG_FUNCTION(this << packet << dstL2Id << lcId);
    auto it = m_unicastDirectLinks.find(dstL2Id);
    if (it == m_unicastDirectLinks.end())
    {
        NS_LOG_DEBUG("No direct link to send data from " << m_l2Id << " to " << dstL2Id);
        return false;
    }
    else if (!it->second->m_hasActiveSlDrb)
    {
        NS_LOG_DEBUG("No active data radio bearer to send data from " << m_l2Id << " to "
                                                                      << dstL2Id);
        return false;
    }
    else if (!it->second->m_hasActiveSlDrb)
    {
        NS_LOG_DEBUG("No active data radio bearer to send data from " << m_l2Id << " to "
                                                                      << dstL2Id);
        return false;
    }
    if (it->second->m_link && it->second->m_link->GetState() != NrSlUeProseDirectLink::ESTABLISHED)
    {
        NS_LOG_DEBUG("Direct link to send data from " << m_l2Id << " to " << dstL2Id
                                                      << " not in ESTABLISHED state");
        return false;
    }
    NS_LOG_DEBUG("Confirming request to send data from " << m_l2Id << " to " << dstL2Id);
    return true;
}

void
NrSlL3ManetService::DoNotifyChangeOfDirectLinkState(
    uint32_t peerL2Id,
    NrSlUeProseDirLnkSapUser::ChangeOfStateNotification info)
{
    NS_LOG_FUNCTION(this << peerL2Id << info.newStateStr << info.newStateEnum);

    // Get link
    auto it = m_unicastDirectLinks.find(peerL2Id);
    if (info.newStateEnum == NrSlUeProseDirectLink::INIT)
    {
        // There is no direct link context yet, so perform an early return
        NS_LOG_INFO("Direct link to " << peerL2Id << " changing to " << info.newStateEnum
                                      << " state");
        return;
    }
    else if (it == m_unicastDirectLinks.end())
    {
        NS_FATAL_ERROR("Could not find the direct link");
    }

    // Perform action depending on state
    switch (info.newStateEnum)
    {
    case NrSlUeProseDirectLink::INIT:
        NS_FATAL_ERROR("Should have returned early above");
        break;
    case NrSlUeProseDirectLink::ESTABLISHING:
        NS_LOG_INFO("Direct link to " << peerL2Id << " changing to " << info.newStateEnum
                                      << " state");
        break;
    case NrSlUeProseDirectLink::ESTABLISHED:
        NS_LOG_INFO("Direct link to " << peerL2Id << " changing to ESTABLISHED state");
        NS_LOG_INFO("Bearer state:  active: " << std::boolalpha << it->second->m_hasActiveSlDrb
                                              << " pending: " << it->second->m_hasPendingSlDrb);

        if (!it->second->m_hasActiveSlDrb && !it->second->m_hasPendingSlDrb)
        {
            NS_LOG_INFO("Requesting activation of SL-DRB to dstL2Id "
                        << peerL2Id << " with peer Ipv4 address " << info.ipInfo.peerIpv4Addr);
            ActivateDirectLinkDataRadioBearer(peerL2Id, info.ipInfo);
            m_directLinkEstablishedTrace(m_l2Id,
                                         info.ipInfo.selfIpv4Addr,
                                         peerL2Id,
                                         info.ipInfo.peerIpv4Addr);
        }
        else
        {
            NS_LOG_INFO("Using active SL-DRB to dstL2Id " << peerL2Id << " with peer Ipv4 address "
                                                          << info.ipInfo.peerIpv4Addr);
            m_directLinkEstablishedTrace(m_l2Id,
                                         info.ipInfo.selfIpv4Addr,
                                         peerL2Id,
                                         info.ipInfo.peerIpv4Addr);
        }

        break;
    case NrSlUeProseDirectLink::RELEASING:
        NS_LOG_INFO("Direct link to " << peerL2Id << " changing to RELEASING state");
        // Suppress calling the releasing trace if the established trace wasn't previously
        // called.  RELEASING can occur from ESTABLISHING state (an attempt that tried to
        // establish a direct link but timed out).  We can check whether this dstL2Id made it
        // to ESTABLISHED state by looking for pending or active data radio bearers
        if (it->second->m_hasPendingSlDrb || it->second->m_hasActiveSlDrb)
        {
            NS_LOG_INFO("Tracing the release of direct link to " << peerL2Id);
            m_directLinkReleasingTrace(m_l2Id,
                                       info.ipInfo.selfIpv4Addr,
                                       peerL2Id,
                                       info.ipInfo.peerIpv4Addr);
        }
        break;
    case NrSlUeProseDirectLink::RELEASED:
        NS_LOG_INFO("Direct link to " << peerL2Id << " changing to RELEASED state");
        NS_LOG_INFO("Requesting the removal of SL-DRB to " << peerL2Id);
        // Suppress calling the releasing trace if the established trace wasn't previously
        // called.  RELEASING can occur from ESTABLISHING state (an attempt that tried to
        // establish a direct link but timed out).  We can check whether this dstL2Id made it
        // to ESTABLISHED state by looking for pending or active data radio bearers
        if (it->second->m_hasPendingSlDrb || it->second->m_hasActiveSlDrb)
        {
            // Check first that we haven't traced this release earlier
            if (info.oldStateEnum != NrSlUeProseDirectLink::RELEASING)
            {
                NS_LOG_INFO("Tracing the release of direct link to " << peerL2Id);
                m_directLinkReleasingTrace(m_l2Id,
                                           info.ipInfo.selfIpv4Addr,
                                           peerL2Id,
                                           info.ipInfo.peerIpv4Addr);
            }
            // Tx bearers
            DeleteDirectLinkDataRadioBearer(peerL2Id, info.ipInfo);
        }
        // Rx bearers
        // Pass the maximum value of lcId to remove bearers for all LCs
        m_nrSlUeSvcRrcSapProvider->NotifySidelinkConnectionRelease(
            peerL2Id,
            m_l2Id,
            std::numeric_limits<uint8_t>::max());
        NS_LOG_INFO("Remove NrSlUeProseDirectLink object to " << peerL2Id);
        m_unicastDirectLinks.erase(peerL2Id);
        break;
    default:
        NS_FATAL_ERROR("Invalid state " << info.newStateStr);
    }
}

void
NrSlL3ManetService::ActivateDirectLinkDataRadioBearer(
    uint32_t peerL2Id,
    NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo)
{
    NS_LOG_FUNCTION(this << peerL2Id << ipInfo.peerIpv4Addr);

    // Get link
    auto itDirLinkCtxt = m_unicastDirectLinks.find(peerL2Id);
    if (itDirLinkCtxt == m_unicastDirectLinks.end())
    {
        NS_FATAL_ERROR("Could not find the direct link");
    }
    else
    {
        // Create unicast TFT to be able to transmit to peer UE
        Ptr<NrSlTft> tft;
        tft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, itDirLinkCtxt->second->m_slInfo);
        NrEpcTft::PacketFilter f;
        // Add all multicast to the packet filters
        f.remoteAddress = Ipv4Address("224.0.0.0");
        f.remoteMask = Ipv4Mask("/4");
        f.remotePortStart = 0;
        f.remotePortEnd = 65535;
        f.precedence = 0;
        tft->Add(f);
        NS_LOG_INFO("Activate bearer for multicast towards " << ipInfo.peerIpv4Addr);
        m_nrSlUeSvcNasSapProvider->ActivateSvcNrSlDataRadioBearer(tft);
        itDirLinkCtxt->second->m_hasPendingSlDrb = true;
    }
}

void
NrSlL3ManetService::DeleteDirectLinkDataRadioBearer(
    uint32_t dstL2Id,
    NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo)
{
    NS_LOG_FUNCTION(this << dstL2Id << ipInfo.peerIpv4Addr);

    // Get link
    auto it = m_unicastDirectLinks.find(dstL2Id);
    if (it == m_unicastDirectLinks.end())
    {
        NS_FATAL_ERROR("Could not find the direct link");
    }
    else
    {
        NS_LOG_LOGIC("Direct link found for destination " << dstL2Id);

        // Create unicast TFT
        Ptr<NrSlTft> tft;
        SidelinkInfo slInfo;
        slInfo.m_dstL2Id = dstL2Id;
        tft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, slInfo);

        // Instruct the NAS to delete SL-DRB
        m_nrSlUeSvcNasSapProvider->DeleteSvcNrSlDataRadioBearer(tft);
    }
}

void
NrSlL3ManetService::SetImsi(uint64_t imsi)
{
    NS_LOG_FUNCTION(this << imsi);
    m_imsi = imsi;
}

void
NrSlL3ManetService::SetL2Id(uint32_t l2Id)
{
    NS_LOG_FUNCTION(this << l2Id);
    m_l2Id = l2Id;
}

uint32_t
NrSlL3ManetService::GetL2Id() const
{
    return m_l2Id;
}

bool
NrSlL3ManetService::CompareDiscoveryInfo(const DiscoveryInfo& a, const DiscoveryInfo& b) const
{
    if (a.role != b.role)
    {
        return a.role < b.role;
    }
    else if (a.appCode != b.appCode)
    {
        return a.appCode < b.appCode;
    }
    else if (a.dstL2Id != b.dstL2Id)
    {
        return a.dstL2Id < b.dstL2Id;
    }
    return a.model < b.model;
}

void
NrSlL3ManetService::OlsrAddRoute(const Ipv4Address& dest,
                                 const Ipv4Address& nextHop,
                                 uint32_t interface,
                                 uint32_t distance)
{
    NS_LOG_FUNCTION(this << dest << nextHop << interface << distance);
    NS_LOG_INFO(dest << " " << nextHop << " " << interface << " " << distance);
    auto nrSlEpcUeNas = m_ueDevice->GetObject<NrUeNetDevice>()->GetNas()->GetObject<NrSlEpcUeNas>();
    NS_ASSERT_MSG(nrSlEpcUeNas, "NrSlEpcUeNas not found");
    const auto dstL2Id = FindL2IdForNextHopIpv4Address(nextHop);
    if (!dstL2Id.has_value())
    {
        NS_LOG_WARN("Unable to add OLSR destination " << dest << " to packet filter for bearer to "
                                                      << nextHop);
        return;
    }

    for (auto it = m_qosRules.begin(); it != m_qosRules.end(); it++)
    {
        for (const auto& filter : (*it)->GetPacketFilters())
        {
            if (filter.remoteAddress == dest)
            {
                NS_LOG_INFO("Activating bearer for custom QoS rule for destination IP "
                            << dest << " using dstL2Id " << dstL2Id.value());
                Ptr<NrSlTft> copy = Create<NrSlTft>(*it);
                // Update SlInfo with current dstL2Id to next hop
                copy->m_sidelinkInfo.m_dstL2Id = dstL2Id.value();
                // Instruct the NAS to activate the corresponding SL-DRB
                m_nrSlUeSvcNasSapProvider->ActivateSvcNrSlDataRadioBearer(copy);
            }
        }
    }

    bool added = nrSlEpcUeNas->AddDestinationToPacketFilters(dstL2Id.value(), dest);
    if (added)
    {
        NS_LOG_INFO("Added OLSR destination " << dest << " to packet filter for bearer to "
                                              << nextHop);
    }
}

void
NrSlL3ManetService::OlsrRemoveRoute(const Ipv4Address& dest,
                                    const Ipv4Address& nextHop,
                                    uint32_t interface,
                                    uint32_t distance)
{
    NS_LOG_FUNCTION(this << dest << nextHop << interface << distance);
    auto nrSlEpcUeNas = m_ueDevice->GetObject<NrUeNetDevice>()->GetNas()->GetObject<NrSlEpcUeNas>();
    NS_ASSERT_MSG(nrSlEpcUeNas, "NrSlEpcUeNas not found");
    const auto dstL2Id = FindL2IdForNextHopIpv4Address(nextHop);
    if (!dstL2Id.has_value())
    {
        NS_LOG_WARN("Unable to remove OLSR destination "
                    << dest << " from packet filter for bearer to " << nextHop);
        return;
    }

    for (auto it = m_qosRules.begin(); it != m_qosRules.end(); it++)
    {
        for (const auto& filter : (*it)->GetPacketFilters())
        {
            if (filter.remoteAddress == dest)
            {
                NS_LOG_INFO("Deactivating bearer for custom custom QoS rule for destination IP "
                            << dest << " using dstL2Id " << dstL2Id.value());
                Ptr<NrSlTft> copy = Create<NrSlTft>(*it);
                // Update SlInfo with current dstL2Id to next hop
                copy->m_sidelinkInfo.m_dstL2Id = dstL2Id.value();
                // Instruct the NAS to deactivate the corresponding SL-DRB
                m_nrSlUeSvcNasSapProvider->DeactivateSvcNrSlDataRadioBearer(copy);
            }
        }
    }

    bool removed = nrSlEpcUeNas->RemoveDestinationFromPacketFilters(dstL2Id.value(), dest);
    if (removed)
    {
        NS_LOG_INFO("Removed OLSR destination " << dest << " from packet filter for bearer to "
                                                << nextHop);
    }
}

std::optional<uint32_t>
NrSlL3ManetService::FindL2IdForNextHopIpv4Address(const Ipv4Address& nextHop)
{
    for (const auto& [l2Id, context] : m_unicastDirectLinks)
    {
        NS_ASSERT_MSG(context->m_ipInfo.peerIpv4Addr != Ipv4Address(),
                      "Peer IPv4 address is uninitialized");
        if (nextHop == context->m_ipInfo.peerIpv4Addr)
        {
            return l2Id;
        }
    }
    NS_LOG_WARN("Did not find a dstL2Id for IPv4 next hop " << nextHop << " on node " << m_imsi
                                                            << "; direct link size "
                                                            << m_unicastDirectLinks.size());
    return std::nullopt;
}

Ipv4Address
NrSlL3ManetService::GetIpv4Address() const
{
    auto ipv4 = m_ueDevice->GetNode()->GetObject<Ipv4>();
    NS_ASSERT_MSG(ipv4, "Ipv4 object not found");
    auto ipv4Interface = ipv4->GetInterfaceForDevice(m_ueDevice);
    NS_ASSERT_MSG(ipv4->GetNAddresses(ipv4Interface) == 1,
                  "Ipv4Interface has more than one address");
    return ipv4->GetAddress(ipv4Interface, 0).GetLocal();
}

Ipv4Address
NrSlL3ManetService::GetIpv4AddressFromPeerL2Id(uint32_t peerL2Id) const
{
    // This implementation assumes that the IPv4 address for the peer can be built
    // by taking the IPv4 address of this device and changing the last octet from
    // the local L2 Id to the peer L2 Id.  If this assumption does not hold,
    // it is recommended to iterate the global NodeList to find the device with the
    // corresponding peer L2 Id.
    NS_ASSERT_MSG(peerL2Id < 256, "Too many nodes in this network (must be < 256)");
    auto selfIp = GetIpv4Address();
    uint8_t buf[4];
    selfIp.Serialize(buf);
    buf[3] = static_cast<uint8_t>(peerL2Id);
    Ipv4Address peerIp;
    peerIp = peerIp.Deserialize(buf);
    return peerIp;
}

void
NrSlL3ManetService::AddQosRule(Ptr<NrSlTft> tft)
{
    NS_LOG_FUNCTION(this);
    m_qosRules.push_back(tft);
}

void
NrSlL3ManetService::SetDefaultSlDrbSlInfo(SidelinkInfo slDrbSlInfo)
{
    NS_LOG_FUNCTION(this);
    m_slDrbSlInfo = slDrbSlInfo;
}

void
NrSlL3ManetService::SetDefaultSlSrbSlInfo(SidelinkInfo slSrbSlInfo)
{
    NS_LOG_FUNCTION(this);
    m_slSrbSlInfo = slSrbSlInfo;
}

void
NrSlL3ManetService::NotifyKeepAliveFailure(uint32_t srcL2Id, uint32_t dstL2Id)
{
    NS_LOG_FUNCTION(this << srcL2Id << dstL2Id);

    auto it = m_unicastDirectLinks.find(dstL2Id);

    NS_ABORT_MSG_IF(it == m_unicastDirectLinks.end(), "Could not find direct link");

    m_nhdp->HandleLinkFailure(it->second->m_ipInfo.peerIpv4Addr);

    m_rlfTrace(srcL2Id, dstL2Id, RlfEvent::KeepAlive);
}

void
NrSlL3ManetService::InitiateLink(uint32_t targetL2Id)
{
    NS_LOG_FUNCTION(this << targetL2Id);

    Ptr<NrSlUeProseDirectLink> link = nullptr;
    auto it = m_unicastDirectLinks.find(targetL2Id);
    if (it != m_unicastDirectLinks.end())
    {
        link = it->second->m_link;
    }

    if (!link)
    {
        auto selfIp = GetIpv4Address();
        auto peerIp = GetIpv4AddressFromPeerL2Id(targetL2Id);
        uint32_t selfL2Id = m_imsi;
        NS_LOG_INFO("Initiating direct link to IP " << peerIp << " L2ID " << targetL2Id
                                                    << " from my IP " << selfIp);
        SidelinkInfo initSlInfo = m_slDrbSlInfo;
        initSlInfo.m_srcL2Id = m_l2Id;
        initSlInfo.m_dstL2Id = targetL2Id;
        AddDirectLinkConnection(selfL2Id, selfIp, targetL2Id, peerIp, INITIATING, 0, initSlInfo);
    }
    else
    {
        if (link->GetState() == NrSlUeProseDirectLink::RELEASING)
        {
            // This can occur if a link was released but the release exchange
            // did not complete, so the underlying Direct Link is in RELEASING
            // state.  Force it to reset.
            NS_LOG_INFO("Resetting the RELEASING direct link to peerL2Id " << targetL2Id);
            link->ResetCurrentLink();
        }
        else
        {
            auto peerIp = GetIpv4AddressFromPeerL2Id(targetL2Id);
            NS_LOG_INFO("Link to IP " << peerIp << " in state " << link->GetState()
                                      << "; ignoring");
        }
    }
}

} // namespace ns3
