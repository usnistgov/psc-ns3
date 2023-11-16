/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */


#include "nr-sl-ue-prose.h"

#include <ns3/lte-ue-rrc.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/lte-radio-bearer-info.h>

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/pointer.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/simulator.h>

#include <algorithm>

#include <ns3/nr-sl-pc5-signalling-header.h>
#include <ns3/nr-point-to-point-epc-helper.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrSlUeProse");
NS_OBJECT_ENSURE_REGISTERED (NrSlUeProse);


NrSlUeProseDirLinkContext::NrSlUeProseDirLinkContext (void)
{
  NS_LOG_FUNCTION (this);
  m_hasActiveSlDrb = false;
  m_hasPendingSlDrb = false;
  m_relayServiceCode = 0;
}

NrSlUeProseDirLinkContext::~NrSlUeProseDirLinkContext (void)
{}

TypeId
NrSlUeProseDirLinkContext::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::NrSlUeProseDirLinkContext")
    .SetParent<Object> ()
    .AddConstructor<NrSlUeProseDirLinkContext> ()
  ;
  return tid;
}

TypeId NrSlUeProse::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::NrSlUeProse")
    .SetParent<NrSlUeService> ()
    .AddConstructor<NrSlUeProse> ()
    .AddAttribute ("DiscoveryInterval",
                   "Discovery interval in seconds",
                   TimeValue (Seconds (1)),
                   MakeTimeAccessor (&NrSlUeProse::m_discoveryInterval),
                   MakeTimeChecker ())
    .AddTraceSource ("PC5SignallingPacketTrace",
                     "Trace fired upon transmission and reception of PC5 Signalling messages",
                     MakeTraceSourceAccessor (&NrSlUeProse::m_pc5SignallingPacketTrace),
                     "ns3::NrSlUeProse::PC5SignallingPacketTracedCallback")
    .AddTraceSource ("DiscoveryTrace",
                     "Trace to track the transmission/reception of discovery messages",
                     MakeTraceSourceAccessor (&NrSlUeProse::m_discoveryTrace),
                     "ns3::NrSlUeProse::DiscoveryTraceTracedCallback")
  ;
  return tid;
}

NrSlUeProse::NrSlUeProse ()
{
  NS_LOG_FUNCTION (this);
  m_nrSlUeSvcRrcSapUser = new MemberNrSlUeSvcRrcSapUser<NrSlUeProse> (this);
  m_nrSlUeSvcNasSapUser = new MemberNrSlUeSvcNasSapUser<NrSlUeProse> (this);
  m_nrSlUeProseDirLnkSapUser = new MemberNrSlUeProseDirLnkSapUser<NrSlUeProse> (this);
  m_imsi = 0;
  m_l2Id = 0;
}


NrSlUeProse::~NrSlUeProse (void)
{
  NS_LOG_FUNCTION (this);
}

void
NrSlUeProse::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_nrSlUeSvcRrcSapUser;
  delete m_nrSlUeSvcNasSapUser;
  delete m_nrSlUeProseDirLnkSapUser;
}

NrSlUeSvcRrcSapUser*
NrSlUeProse::GetNrSlUeSvcRrcSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_nrSlUeSvcRrcSapUser;
}

void
NrSlUeProse::SetNrSlUeSvcRrcSapProvider (NrSlUeSvcRrcSapProvider* s)
{
  NS_LOG_FUNCTION (this);
  m_nrSlUeSvcRrcSapProvider = s;
}


NrSlUeSvcNasSapUser*
NrSlUeProse::GetNrSlUeSvcNasSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_nrSlUeSvcNasSapUser;
}

void
NrSlUeProse::SetNrSlUeSvcNasSapProvider (NrSlUeSvcNasSapProvider* s)
{
  NS_LOG_FUNCTION (this);
  m_nrSlUeSvcNasSapProvider = s;
}


void
NrSlUeProse::DoNotifySvcNrSlDataRadioBearerActivated (uint32_t peerL2Id)
{
  NS_LOG_FUNCTION (this << peerL2Id);

  //Get link
  auto it = m_unicastDirectLinks.find (peerL2Id);
  if (it == m_unicastDirectLinks.end ())
    {
      NS_LOG_DEBUG ("Could not find the direct link");
    }
  else
    {
      it->second->m_hasPendingSlDrb = false;
      it->second->m_hasActiveSlDrb = true;
    }
}


void NrSlUeProse::ConfigureUnicast ()
{
  NS_LOG_FUNCTION (this);

  //Tell the RRC to inform the MAC to monitor the UE's own L2Id
  m_nrSlUeSvcRrcSapProvider->MonitorSelfL2Id ();

  //Configure traffic profile for signalind radio bearers
  m_slSrbSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
  m_slSrbSlInfo.m_dynamic = true;
  m_slSrbSlInfo.m_priority = 1;

}

void NrSlUeProse::ConfigureL2IdMonitoringForDiscovery (uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this << dstL2Id);

  //Tell the RRC to inform the MAC to monitor the UE's own L2Id
  m_nrSlUeSvcRrcSapProvider->MonitorSelfL2Id ();

  //Tell the RRC to inform the MAC to monitor the UE's L2Id
  m_nrSlUeSvcRrcSapProvider->MonitorL2Id (dstL2Id);
}

void
NrSlUeProse::AddDirectLinkConnection (uint32_t selfL2Id, Ipv4Address selfIp,
                                      uint32_t peerL2Id, bool isInitiating,
                                      uint32_t relayServiceCode,
									  const struct SidelinkInfo& slInfo)
{
  NS_LOG_FUNCTION (this << selfL2Id << selfIp << peerL2Id << isInitiating << relayServiceCode);

  NS_ASSERT_MSG (selfL2Id == m_l2Id, "L2Id mismatch.");

  auto it = m_unicastDirectLinks.find (peerL2Id);
  NS_ASSERT_MSG (it == m_unicastDirectLinks.end (), "Direct link " << selfL2Id << "<-->" << peerL2Id << "already exist. "
                 "We currently support only one direct link connection, which should be configured from the scenario once.");

  bool isRelayConn = false;
  if (relayServiceCode > 0)
  {
	  isRelayConn = true;
  }

  if (isRelayConn && !isInitiating)  //Relay UE
    {
      auto it = m_l3U2nRelayProvidedSvcs.find (relayServiceCode);
      if (it == m_l3U2nRelayProvidedSvcs.end ())
        {
          NS_FATAL_ERROR ("Could not find the U2N relay service configuration for relay service code: " <<
                          relayServiceCode << ". " <<
                          "Did you configure the service for the relay UEs in the scenario?");
        }
    }

  //Create context to maintain here in the ProSe layer
  Ptr<NrSlUeProseDirLinkContext> context = CreateObject <NrSlUeProseDirLinkContext> ();

  //Create Direct Link instance and set parameters
  Ptr<NrSlUeProseDirectLink> link = CreateObject <NrSlUeProseDirectLink> ();
  link->SetParameters (selfL2Id, peerL2Id, isInitiating, isRelayConn, relayServiceCode, selfIp);

  //Connect SAPs
  link->SetNrSlUeProseDirLnkSapUser (GetNrSlUeProseDirLnkSapUser ());

  context->m_link = link;
  context->m_nrSlUeProseDirLnkSapProvider = link->GetNrSlUeProseDirLnkSapProvider ();
  context->m_ipInfo.selfIpv4Addr = selfIp;
  context->m_slInfo = slInfo;

  if (isRelayConn)
    {
      context->m_relayServiceCode = relayServiceCode;
    }

  //Store context
  m_unicastDirectLinks.insert (std::pair <uint32_t, Ptr<NrSlUeProseDirLinkContext> > (peerL2Id, context));

  //Initiate connection establishment procedure if this UE is the initiating UE
  if (isInitiating)
    {
      context->m_link->StartConnectionEstablishment ();
    }
}


NrSlUeProseDirLnkSapUser*
NrSlUeProse::GetNrSlUeProseDirLnkSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_nrSlUeProseDirLnkSapUser;
}


void
NrSlUeProse::DoSendNrSlPc5SMessage (Ptr<Packet> packet, uint32_t dstL2Id,  uint8_t lcId)
{
  NS_LOG_FUNCTION (this);

  //Activate the corresponding SL-SRB for the logical channel, if not active
  SidelinkInfo slSrbSlInfo;
  slSrbSlInfo.m_castType = m_slSrbSlInfo.m_castType;
  slSrbSlInfo.m_dynamic = m_slSrbSlInfo.m_dynamic;
  slSrbSlInfo.m_priority = m_slSrbSlInfo.m_priority;
  slSrbSlInfo.m_srcL2Id = m_l2Id;
  slSrbSlInfo.m_dstL2Id = dstL2Id;
  slSrbSlInfo.m_lcId = lcId;
  auto it = m_activeSlSrbs.find (dstL2Id);
  if (it == m_activeSlSrbs.end ()) //First SL-SRB for this destination
    {
      //Instruct the RRC to activate the SL-SRB
      m_nrSlUeSvcRrcSapProvider->ActivateNrSlSignallingRadioBearer (slSrbSlInfo);

      //Keep track of it
      std::bitset<4> activeLcs;
      activeLcs[lcId] = true;
      m_activeSlSrbs.insert (std::pair<uint32_t, std::bitset<4> > (dstL2Id, activeLcs));
    }
  else
    {
      if (it->second [lcId] == false) //First SL-SRB for this lcId
        {
          //Instruct the RRC to activate the SL-SRB
          m_nrSlUeSvcRrcSapProvider->ActivateNrSlSignallingRadioBearer (slSrbSlInfo);
          //Keep track of it
          it->second [lcId] = true;
        }
    }
  m_pc5SignallingPacketTrace (m_l2Id, dstL2Id, true, packet);

  //Pass the message to the RRC
  m_nrSlUeSvcRrcSapProvider->SendNrSlSignalling (packet, dstL2Id, lcId);

}

void 
NrSlUeProse::AddDiscoveryApp (uint32_t appCode, uint32_t dstL2Id, DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);
    
  DiscoveryInfo info;
  info.role = role;
  info.appCode = appCode;
  info.dstL2Id = dstL2Id;

  NS_LOG_DEBUG ("Adding app code");
  m_discoveryMap.insert ( std::pair <uint32_t, DiscoveryInfo> (appCode, info) );

  if (role == Announcing || role == Discoverer)
  {
    SendDiscovery (appCode, dstL2Id);
  }
  //It instructs the MAC layer (and PHY therefore) to monitor packets directed the UE's own and other Layer 2 IDs
  ConfigureL2IdMonitoringForDiscovery (dstL2Id);
}

void
NrSlUeProse::RemoveDiscoveryApp (uint32_t appCode, DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);
  std::map <uint32_t, DiscoveryInfo>::iterator itInfo;
  itInfo = m_discoveryMap.find (appCode);
  if (itInfo != m_discoveryMap.end ())
    {
       NS_ASSERT_MSG (itInfo->second.role == role, "Wrong role.");
      //found app to remove
      NS_LOG_DEBUG ("Removing app code");
      m_discoveryMap.erase (itInfo);
    }
}

bool
NrSlUeProse::IsMonitoringApp (uint8_t msgType, uint32_t appCode)
{
  NS_LOG_FUNCTION (this);
  //the device is interested in announcement if monitoring, in request if acting as discoveree, and in response if acting as discoverer
  std::map <uint32_t, DiscoveryInfo>::iterator itInfo = m_discoveryMap.find (appCode);
  if (itInfo != m_discoveryMap.end ())
    {
      return ( (msgType == NrSlDiscoveryHeader::DISC_OPEN_ANNOUNCEMENT && itInfo->second.role == Monitoring)
               || (msgType == NrSlDiscoveryHeader::DISC_RESTRICTED_QUERY && itInfo->second.role == Discoveree)
               || (msgType == NrSlDiscoveryHeader::DISC_RESTRICTED_RESPONSE && itInfo->second.role == Discoverer) );
    }
  return false;
}

void
NrSlUeProse::SendDiscovery (uint32_t appCode, uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this);

  std::map <uint32_t, DiscoveryInfo>::iterator it;

  it = m_discoveryMap.find (appCode);

  if (it != m_discoveryMap.end ())
  {
    
    NrSlDiscoveryHeader discHeader;

    if (it->second.role == Announcing)
    {
      discHeader.SetOpenDiscoveryAnnounceParameters (appCode);
    
      //reschedule
      Simulator::Schedule (m_discoveryInterval, &NrSlUeProse::SendDiscovery, this, appCode, dstL2Id);

    }
    else if (it->second.role == Discoverer)
    {
      discHeader.SetRestrictedDiscoveryQueryParameters (appCode);
    
      //reschedule
      Simulator::Schedule (m_discoveryInterval, &NrSlUeProse::SendDiscovery, this, appCode, dstL2Id);
    }

    else if (it->second.role == Discoveree)
    {
      discHeader.SetRestrictedDiscoveryResponseParameters (appCode);

      //no reschedule
    }
  
    //build message to transmit
    Ptr<Packet> discoveryPacket = Create<Packet>();
    discoveryPacket->AddHeader (discHeader);

    DoSendNrSlDiscovery (discoveryPacket, dstL2Id);   
    m_discoveryTrace (m_l2Id, dstL2Id, true, discHeader); 
  }
}

void
NrSlUeProse::AddRelayDiscovery (uint32_t relayCode, uint32_t dstL2Id, DiscoveryModel model, DiscoveryRole role)
{
  NS_LOG_FUNCTION (this << relayCode << dstL2Id << model << role);
  NS_ASSERT_MSG (m_relayMap.find (relayCode) == m_relayMap.end (), "Cannot add already existing service " << relayCode);

  DiscoveryInfo info;
  info.model = model;
  info.role = role;
  info.appCode = relayCode;
  info.dstL2Id = dstL2Id;
  
  m_relayMap.insert ( std::pair <uint32_t, DiscoveryInfo> (relayCode, info) );

  if ((model == ModelA && role == RelayUE) || (model == ModelB && role == RemoteUE))
  {
    SendRelayDiscovery (relayCode, dstL2Id);
  }
 
  //It instructs the MAC layer (and PHY therefore) to monitor packets directed the UE's own and other Layer 2 IDs
  ConfigureL2IdMonitoringForDiscovery (dstL2Id);
}

void
NrSlUeProse::RemoveRelayDiscovery (uint32_t relayCode, DiscoveryRole role)
{
  NS_LOG_FUNCTION (this << relayCode);

  std::map <uint32_t, DiscoveryInfo>::iterator it = m_relayMap.find (relayCode);
  if (it != m_relayMap.end ())
    {
      NS_ASSERT_MSG (it->second.role == role, "Wrong role.");
      m_relayMap.erase (it);
    }
}

bool
NrSlUeProse::IsMonitoringRelay (uint8_t msgType, uint32_t relayCode)
{
  NS_LOG_FUNCTION (this << relayCode);
  std::map <uint32_t, DiscoveryInfo>::iterator it;
  it = m_relayMap.find (relayCode);

  if (it != m_relayMap.end())
  {
    return ( (msgType == NrSlDiscoveryHeader::DISC_RELAY_ANNOUNCEMENT && it->second.model == ModelA && it->second.role == RemoteUE)
              || (msgType == NrSlDiscoveryHeader::DISC_RELAY_SOLICITATION && it->second.model == ModelB && it->second.role == RelayUE)
              || (msgType == NrSlDiscoveryHeader::DISC_RELAY_RESPONSE && it->second.model == ModelB && it->second.role == RemoteUE) );
  }
  return false;
}

void
NrSlUeProse::SendRelayDiscovery (uint32_t relayCode, uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this << relayCode);

  std::map <uint32_t, DiscoveryInfo>::iterator it;

  it = m_relayMap.find (relayCode);
  if (it != m_relayMap.end ())
  {
    //build message to transmit
    NrSlDiscoveryHeader discHeader;

    if (it->second.model == ModelA && it->second.role == RelayUE)
      {
        discHeader.SetRelayAnnouncementParameters (relayCode, m_imsi, m_l2Id, 1);
        //reschedule
        Simulator::Schedule (m_discoveryInterval, &NrSlUeProse::SendRelayDiscovery, this, relayCode, dstL2Id);
      }

    else if (it->second.model == ModelB && it->second.role == RemoteUE)
      {
        discHeader.SetRelaySoliciationParameters (relayCode, m_imsi, m_l2Id);
        //reschedule
        Simulator::Schedule (m_discoveryInterval, &NrSlUeProse::SendRelayDiscovery, this, relayCode, dstL2Id); 
      }
        
    else if (it->second.model == ModelB && it->second.role == RelayUE)
      {
        discHeader.SetRelayResponseParameters (relayCode, m_imsi, m_l2Id, 1);
        //no reschedule
      }
      
      //send
      Ptr<Packet> discoveryPacket = Create<Packet>();
      discoveryPacket->AddHeader (discHeader);
      DoSendNrSlDiscovery (discoveryPacket, dstL2Id);   
      m_discoveryTrace (m_l2Id, dstL2Id, true, discHeader);
  }
}

void
NrSlUeProse::DoSendNrSlDiscovery (Ptr<Packet> packet, uint32_t dstL2Id)
{
  NS_LOG_FUNCTION (this);

  //Activate the corresponding SL Discovery RB for the logical channel, if not active
  auto it = std::find (m_activeSlDiscoveryRbs.begin(), m_activeSlDiscoveryRbs.end(), dstL2Id);
  if (it == m_activeSlDiscoveryRbs.end ()) //First SL Discovery RB for this destination
    {
      //Instruct the RRC to activate the SL Disocvery RB
      m_nrSlUeSvcRrcSapProvider->ActivateNrSlDiscoveryRadioBearer (dstL2Id);

      //Keep track of it
      m_activeSlDiscoveryRbs.push_front (dstL2Id);
    }
  
  //Pass the message to the RRC
  m_nrSlUeSvcRrcSapProvider->SendNrSlDiscovery (packet, dstL2Id);

}

void
NrSlUeProse::DoReceiveNrSlDiscovery (Ptr<Packet> packet, uint32_t srcL2Id)
{
  NS_LOG_FUNCTION (this);
  NrSlDiscoveryHeader discHeader;
  packet->RemoveHeader (discHeader);

  uint8_t msgType = discHeader.GetDiscoveryMsgType ();

  //Discovery
  if (msgType == NrSlDiscoveryHeader::DISC_OPEN_ANNOUNCEMENT 
      || msgType == NrSlDiscoveryHeader::DISC_RESTRICTED_QUERY 
      || msgType == NrSlDiscoveryHeader::DISC_RESTRICTED_RESPONSE)
  { 
    uint32_t appCode = discHeader.GetApplicationCode ();

    //open or restricted announcement
    if (IsMonitoringApp (msgType, appCode))
      {
        //Check if this is a request I am interested in
        std::map <uint32_t, DiscoveryInfo>::iterator itInfo = m_discoveryMap.find (appCode);
        if (itInfo != m_discoveryMap.end ())
          {
            NS_LOG_INFO ("Discovery message received by " << m_l2Id << " from " << srcL2Id);
            m_discoveryTrace (srcL2Id, m_l2Id, false, discHeader);

            //check if this is a request message for an app for which this UE is a Discoveree
            if (msgType == NrSlDiscoveryHeader::DISC_RESTRICTED_QUERY && itInfo->second.role == Discoveree)
              {
                SendDiscovery (appCode, srcL2Id);
              }
          }
      }
  }
  //Relay Discovery
  else if (msgType == NrSlDiscoveryHeader::DISC_RELAY_ANNOUNCEMENT 
      || msgType == NrSlDiscoveryHeader::DISC_RELAY_RESPONSE
      || msgType == NrSlDiscoveryHeader::DISC_RELAY_SOLICITATION)
  {
    uint32_t relayCode = discHeader.GetRelayServiceCode ();

    if (IsMonitoringRelay (msgType, relayCode))
    {
      //Check if this is a relay announcement or relay response/solicitation I am interested in
      std::map <uint32_t, DiscoveryInfo>::iterator itInfo = m_relayMap.find (relayCode);
      if (itInfo != m_relayMap.end ())
      {
        NS_LOG_INFO ("Relay message received by " << m_l2Id << " from " << srcL2Id);
        m_discoveryTrace (srcL2Id, m_l2Id, false, discHeader);
        
        if (msgType == NrSlDiscoveryHeader::DISC_RELAY_SOLICITATION && itInfo->second.role == RelayUE)
        {
          SendRelayDiscovery (relayCode, srcL2Id);
        }
        else
        {
          //StartRelayDirectCommunication
          //TODO: Stop relay discovery retransmission (for the next discovery interval)
        }
      }
    }
  }
}

void
NrSlUeProse::DoReceiveNrSlSignalling (Ptr<Packet> packet, uint32_t srcL2Id)
{
  NS_LOG_FUNCTION (this);

  //If PC5-S for unicast communication:
  auto it = m_unicastDirectLinks.find (srcL2Id);
  if (it == m_unicastDirectLinks.end ())
    {
      NS_LOG_INFO ("Unrecognized peer L2 ID...");
      NS_FATAL_ERROR ("This case is not handled yet. You should configure both UEs on the "
                      "direct link from the scenario.");
      //This will be the case e.g. when the Relay UE receives a request from the Remote UE
      //after the relay discovery procedure.
      //In that case, the Relay UE should:
      //1. Create the corresponding context and direct link instance.
      //2. Pass the packet to the corresponding direct link instance
    }
  else
    {
      NS_LOG_INFO ("Context found!");
      m_pc5SignallingPacketTrace (srcL2Id, m_l2Id, false, packet);

      //Pass the packet to the corresponding direct link instance
      it->second->m_nrSlUeProseDirLnkSapProvider->ReceiveNrSlPc5Message (packet);
    }
}

void
NrSlUeProse::DoNotifyChangeOfDirectLinkState (uint32_t peerL2Id, NrSlUeProseDirLnkSapUser::ChangeOfStateNotification info)
{
  NS_LOG_FUNCTION (this << peerL2Id << info.newStateStr << info.newStateEnum);

  //Get link
  auto it = m_unicastDirectLinks.find (peerL2Id);
  if (it == m_unicastDirectLinks.end ())
    {
      NS_FATAL_ERROR ("Could not find the direct link");
    }

  //Perform action depending on state
  switch (info.newStateEnum)
    {
      case NrSlUeProseDirectLink::INIT:
        NS_LOG_INFO ("INIT");
        break;
      case NrSlUeProseDirectLink::ESTABLISHING:
        NS_LOG_INFO ("ESTABLISHING");
        break;
      case NrSlUeProseDirectLink::ESTABLISHED:
        NS_LOG_INFO ("ESTABLISHED");

        if (!it->second->m_hasActiveSlDrb && !it->second->m_hasPendingSlDrb)
          {

            if (info.relayInfo.isRelayConn)
              {
                NS_LOG_INFO ("info.ipInfo.selfIpv4Addr: " << info.ipInfo.selfIpv4Addr);

                //Depending on the UE Role, we need to tell the NAS to (re)configure the data bearers
                //to have the data packets flowing in the appropriate path
                ConfigureDataRadioBearersForU2nRelay (peerL2Id, info.relayInfo, info.ipInfo, it->second->m_slInfo);
              }
            else
              {
                NS_LOG_INFO ("Instructing activation of SL-DRB with peer Ipv4 address " << info.ipInfo.peerIpv4Addr);
                ActivateDirectLinkDataRadioBearer (peerL2Id, info.ipInfo);
              }

          }

        break;
      case NrSlUeProseDirectLink::RELEASING:
        NS_LOG_INFO ("RELEASING");
        break;
      case NrSlUeProseDirectLink::RELEASED:
        NS_LOG_INFO ("RELEASED");
        break;
      default:
        NS_FATAL_ERROR ("Invalid state " << info.newStateStr);
    }


}

void
NrSlUeProse::ActivateDirectLinkDataRadioBearer (uint32_t peerL2Id, NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo)
{
  NS_LOG_FUNCTION (this << peerL2Id << ipInfo.peerIpv4Addr);

  //Get link
  auto itDirLinkCtxt = m_unicastDirectLinks.find (peerL2Id);
  if (itDirLinkCtxt == m_unicastDirectLinks.end ())
    {
      NS_FATAL_ERROR ("Could not find the direct link");
    }
  else
    {
      //Create unicast TFT to be able to transmit to peer UE
      Ptr<LteSlTft> tft;
      tft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT,
                              ipInfo.peerIpv4Addr, itDirLinkCtxt->second->m_slInfo);
      m_nrSlUeSvcNasSapProvider->ActivateSvcNrSlDataRadioBearer (tft);
      itDirLinkCtxt->second->m_hasPendingSlDrb = true;
    }
}

void
NrSlUeProse::ConfigureDataRadioBearersForU2nRelay (uint32_t peerL2Id,
                                                   NrSlUeProseDirLnkSapUser::DirectLinkRelayInfo relayInfo,
                                                   NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo,
                                                   const struct SidelinkInfo& slInfo)
{
  NS_LOG_FUNCTION (this << peerL2Id << relayInfo.role << ipInfo.peerIpv4Addr);

  uint8_t relayDrbId = 0;
  if (relayInfo.role == NrSlUeProseDirLnkSapUser::RelayUe)
    {
      //Tell the EPC helper to configure the EpcPgwApplication to route the packets
      //directed to the remote UE towards the relay UE
      m_epcHelper->AddRemoteUe (m_imsi, ipInfo.peerIpv4Addr);

      //Find data relay radio bearer id for the service
      auto it = m_l3U2nRelayProvidedSvcs.find (relayInfo.relayServiceCode);
      if (it == m_l3U2nRelayProvidedSvcs.end ())
        {
          NS_FATAL_ERROR ("Could not find the U2N relay service with relay service code :" << relayInfo.relayServiceCode);
        }
      else
        {
          relayDrbId = it->second.relayDrbId;
        }
    }

  //Tell the NAS to (re)configure the UL and SL data bearers to have the data packets
  //flowing in the appropriate path
  m_nrSlUeSvcNasSapProvider->ConfigureNrSlDataRadioBearersForU2nRelay (peerL2Id, relayInfo.role, ipInfo, relayDrbId, slInfo);

}

void
NrSlUeProse::AddL3U2nRelayServiceConfiguration (uint32_t relayServiceCode, NrSlL3U2nServiceConfiguration config)
{
  NS_LOG_FUNCTION (this << relayServiceCode << config.relayDrbId);

  auto it = m_l3U2nRelayProvidedSvcs.find (relayServiceCode);
  if (it == m_l3U2nRelayProvidedSvcs.end ())
    {
      //First time
      m_l3U2nRelayProvidedSvcs.insert (std::pair<uint32_t, NrSlL3U2nServiceConfiguration> (relayServiceCode, config));
    }
  else
    {
      //Update
      it->second.relayDrbId = config.relayDrbId;
    }
}


void
NrSlUeProse::SetImsi (uint64_t imsi)
{
  NS_LOG_FUNCTION (this << imsi);
  m_imsi = imsi;
}

void
NrSlUeProse::SetL2Id (uint32_t l2Id)
{
  NS_LOG_FUNCTION (this << l2Id);
  m_l2Id = l2Id;
}

void
NrSlUeProse::SetEpcHelper (const Ptr<NrPointToPointEpcHelper> &epcHelper)
{
  NS_LOG_FUNCTION (this);
  m_epcHelper = epcHelper;
}

} // namespace ns3
