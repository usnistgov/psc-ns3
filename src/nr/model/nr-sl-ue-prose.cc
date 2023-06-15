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
#include <ns3/node.h>
#include <ns3/ipv4-l3-protocol.h>

#include <ns3/nr-sl-pc5-signalling-header.h>
#include <ns3/nr-point-to-point-epc-helper.h>

#include "nr-sl-ue-prose-relay-selection-algorithm.h"

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

TypeId 
NrSlUeProse::GetTypeId (void)
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
                     "ns3::NrSlUeProse::DiscoveryTracedCallback")
    .AddTraceSource ("RelayDiscoveryTrace",
                     "Traces when the Remote UE discovers a new Relay UE.",
                     MakeTraceSourceAccessor (&NrSlUeProse::m_relayDiscoveryTrace),
                     "ns3::NrSlUeProse::RelayDiscoveryTracedCallback")
    .AddTraceSource ("RelaySelectionTrace",
                     "Traces when the Remote UE selects a new Relay UE.",
                     MakeTraceSourceAccessor (&NrSlUeProse::m_relaySelectionTrace),
                     "ns3::NrSlUeProse::RelaySelectionTracedCallback")
    .AddTraceSource ("RelayRsrpTrace",
                     "Traces when an RRSP measurement is received.",
                     MakeTraceSourceAccessor (&NrSlUeProse::m_relayRsrpTrace),
                     "ns3::NrSlUeProse::RelayRsrpTracedCallback")

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
  m_connectingRelay.l2Id = 0;
  m_currentSelectedRelay.l2Id = 0;
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
NrSlUeProse::SetDiscoveryInterval (Time val)
{
  NS_LOG_FUNCTION (this << val);
  m_discoveryInterval = val;
}

void
NrSlUeProse::DoNotifySvcNrSlDataRadioBearerActivated (uint32_t peerL2Id)
{
  NS_LOG_FUNCTION (this << peerL2Id);

  //Get link
  auto it = m_unicastDirectLinks.find (peerL2Id);
  if (it == m_unicastDirectLinks.end ())
    {
      NS_FATAL_ERROR ("Could not find the direct link");
    }
  else
    {
      it->second->m_hasPendingSlDrb = false;
      it->second->m_hasActiveSlDrb = true;
    }
}

void
NrSlUeProse::DoNotifySvcNrSlDataRadioBearerRemoved (uint32_t peerL2Id)
{
  NS_LOG_FUNCTION (this << peerL2Id);

  //Get link
  auto it = m_unicastDirectLinks.find (peerL2Id);
  if (it == m_unicastDirectLinks.end ())
    {
      NS_FATAL_ERROR ("Could not find the direct link");
    }
  else
    {
      it->second->m_hasActiveSlDrb = false;
    }
}

void NrSlUeProse::ConfigureUnicast ()
{
  NS_LOG_FUNCTION (this);

  //Tell the RRC to inform the MAC to monitor the UE's own L2Id
  m_nrSlUeSvcRrcSapProvider->MonitorSelfL2Id ();

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
                                      bool isRelayConn, uint32_t relayServiceCode)
{
  NS_LOG_FUNCTION (this << selfL2Id << selfIp << peerL2Id << isInitiating << isRelayConn << relayServiceCode);
  NS_ASSERT_MSG (selfL2Id == m_l2Id, "L2Id mismatch.");

  auto it = m_unicastDirectLinks.find (peerL2Id);
   
  //Check if a direct link connection is ongoing
  //If a direct link between the pair of UEs already exists, it gets overwritten with the new parameters.
  // - On the remote side: This may happen in certain cases where the ReleaseRequest or ReleaseAccept message gets lost 
  //and eventually retransmitted (once the related timer runs out). However, before this process concludes, 
  //the same relay is available again and the relay selection procedure is triggered, 
  //which calls for the reset of the existing (incomplete) link and the starting of a new connection.
  // - On the relay side: The relay does not get an acknowledgement of the ReleaseAccept. So the link does
  //not get deleted from the m_unicastDirectLinks map. If the remote reconsiders connecting to this relay after a while, 
  //a reset and an update of the pre-existing old link would take place.
  if (it != m_unicastDirectLinks.end ())
    {
      NS_LOG_INFO ("Direct link " << selfL2Id << " <--> " << peerL2Id << " already exists.");
    
      //update link parameters
      it->second->m_link->SetParameters (selfL2Id, peerL2Id, isInitiating, isRelayConn, relayServiceCode, selfIp);
      //update map element parameters 
      it->second->m_ipInfo.selfIpv4Addr = selfIp;
      if (isRelayConn)
        {
          it->second->m_relayServiceCode = relayServiceCode;
        }
      //reset link
      it->second->m_link->ResetCurrentLink ();
    }
  else
    {
      NS_LOG_INFO ("New direct link " << selfL2Id << " <--> " << peerL2Id );
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
  auto it = m_activeSlSrbs.find (dstL2Id);
  if (it == m_activeSlSrbs.end ()) //First SL-SRB for this destination
    {
      //Instruct the RRC to activate the SL-SRB
      m_nrSlUeSvcRrcSapProvider->ActivateNrSlSignallingRadioBearer (dstL2Id, lcId);

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
          m_nrSlUeSvcRrcSapProvider->ActivateNrSlSignallingRadioBearer (dstL2Id, lcId);
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
  std::map <uint32_t, DiscoveryInfo>::iterator itCode = m_relayMap.find (relayCode);
  
  if (itCode != m_relayMap.end ())
    {
      NS_ASSERT_MSG (itCode->second.role == role, "Wrong role.");
      m_relayMap.erase (itCode);
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
  NS_LOG_INFO ("Discovery message sent by " << m_l2Id << " to " << dstL2Id);
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
                  //Check availability of RSRP measurement for this relay to add to the discovery trace
                  std::pair <double, bool> relayMeas = FindRsrpMeasurement (srcL2Id);
                  //Discovery trace
                  m_relayDiscoveryTrace (m_l2Id, srcL2Id, relayCode, relayMeas.first);

                  //Update List of discovered relays
                  UpdateDiscoveredRelaysList (srcL2Id, relayCode);

                  //Initiate relay selection procedure
                  if (m_relaySelectionAlgorithm)
                    {
                      SelectRelay ();
                    }
                }
            }
        }
    }
}

void
NrSlUeProse::DoReceiveNrSlRsrpMeasurements (uint32_t peerId, double value, bool eligible)
{
  NS_LOG_FUNCTION (this << peerId << value);

  m_relayRsrpTrace (m_l2Id, peerId, value);
  
  std::pair<double, bool> rsrp (value, eligible);

  std::map <uint32_t, std::pair <double, bool> >::iterator it = m_rsrpMeasurementsMap.find (peerId);
  if (it == m_rsrpMeasurementsMap.end())
    {
      //First time adding this UE
      m_rsrpMeasurementsMap.insert (std::pair <uint32_t, std::pair <double, bool> > (peerId, rsrp));
    }
  else
    {
      //updating existing values
      it->second = rsrp;
    }

  //Update m_discoveredRelaysList with new RSRP information (value and eligibility)
  uint32_t index = 0;
  bool found = false;
  RelayInfo relay;
  while (index < m_discoveredRelaysList.size () && !found)
    {
      relay = m_discoveredRelaysList.at (index);
      if (relay.l2Id == peerId)
        {
          found = true;
          m_discoveredRelaysList.erase (m_discoveredRelaysList.begin () + index);
          break;
        }
      index++;
    }
  if (found)
    {
      relay.rsrp = value;
      relay.eligible = eligible;
      m_discoveredRelaysList.push_back (relay);
    }
  //Trigger relay selection (given the new RSRP update)
  if (m_relaySelectionAlgorithm)
    {
      SelectRelay ();
    }
}

std::map <uint32_t, std::pair <double, bool> >
NrSlUeProse::GetRsrpMeasurementsMap ()
{
  NS_LOG_FUNCTION (this);
  return m_rsrpMeasurementsMap;
}

std::pair <double, bool>
NrSlUeProse::FindRsrpMeasurement (uint32_t l2Id)
{
  NS_LOG_FUNCTION (this << l2Id);

  double rsrpVal;
  bool validRelay;

  //Look up the corresponding RSRP measurement
  std::map <uint32_t, std::pair <double, bool> >::iterator rsrpIt = m_rsrpMeasurementsMap.find (l2Id);
  if (rsrpIt != m_rsrpMeasurementsMap.end ())
    {
      NS_LOG_DEBUG ("RSRP measurement found for this discovered relay");
      rsrpVal = rsrpIt->second.first;
      validRelay = rsrpIt->second.second;
    }
  else
    {
      NS_LOG_DEBUG ("No RSRP measurement found for this discovered relay");
      rsrpVal = (-1) * (std::numeric_limits<double>::infinity());
      validRelay = false;
    }

  std::pair <double, bool> rsrpMeas (rsrpVal, validRelay);
  return rsrpMeas;

}

void
NrSlUeProse::UpdateDiscoveredRelaysList (uint32_t relayL2Id, uint32_t relayCode)
{
  NS_LOG_FUNCTION (this << relayL2Id << relayCode);
        
  std::pair <double, bool> relayMeas = FindRsrpMeasurement (relayL2Id);

  RelayInfo discoveredRelay;
  discoveredRelay.l2Id = relayL2Id;
  discoveredRelay.relayCode = relayCode;
  discoveredRelay.rsrp = relayMeas.first;
  discoveredRelay.eligible = relayMeas.second;

  //Update the list of discovered relays
  bool found = false;
  if (m_discoveredRelaysList.size () != 0)
    {
      uint32_t index = 0;
      RelayInfo relay;
      while (index < m_discoveredRelaysList.size () && !found)
        {
          relay = m_discoveredRelaysList.at (index);
          if (relay.l2Id == discoveredRelay.l2Id)
            {
              found = true;
              //erase old information
              m_discoveredRelaysList.erase (m_discoveredRelaysList.begin () + index);
            }
          index++;
        }
    }
  //push new information in
  m_discoveredRelaysList.push_back (discoveredRelay);
}

std::vector<NrSlUeProse::RelayInfo>
NrSlUeProse::GetDiscoveredRelaysList ()
{
  NS_LOG_FUNCTION (this);
  return m_discoveredRelaysList;
}

void
NrSlUeProse::SelectRelay ()
{
  NS_LOG_FUNCTION (this);
  RelayInfo newRelay;

  if (m_connectingRelay.l2Id != 0)
    {
      //Ignore request
      newRelay = m_currentSelectedRelay;
      NS_LOG_DEBUG ("This remote has an ongoing connection establishment with a Relay of L2 ID = " << m_connectingRelay.l2Id << ". Ignore relay reselection request!");
    }
  else
    {
      //Check if the list of discovered relays is empty
      if (m_discoveredRelaysList.size() != 0)
        {
          newRelay = m_relaySelectionAlgorithm->SelectRelay (m_discoveredRelaysList);
          
          //Check if it is an eligibale relay
          if (newRelay.l2Id != 0)
            {
              //Trace the attempt to change a selected relay
              m_relaySelectionTrace (m_l2Id, m_currentSelectedRelay.l2Id, newRelay.l2Id, newRelay.relayCode, newRelay.rsrp);

              //A new relay was selected
              if (newRelay.l2Id == m_currentSelectedRelay.l2Id)
                {
                  NS_LOG_LOGIC ("The remote is already connected to this relay. Do nothing!");
                }
              else
                {
                  Ipv4Address remoteIp = m_ueDevice->GetNode ()->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();  

                  if (m_currentSelectedRelay.l2Id == 0)
                    {
                      NS_LOG_LOGIC ("This is the first time selecting a relay for this remote. Continue!");
                    }
                  else
                    {
                      NS_LOG_LOGIC ("This remote is already connected to a different relay. Find the connection and release it!");
                      //Get existing link
                      auto it = m_unicastDirectLinks.find (m_currentSelectedRelay.l2Id);
                      if (it == m_unicastDirectLinks.end ())
                      {
                        NS_FATAL_ERROR ("Could not find the direct link");
                      }
                      else
                        {
                          //Initiate release procedure with current relay/link
                          //According to 3GPP TS 24.554, the cause of link release can be:
                          //  #1	direct communication to the target UE not allowed;
                          //  #2	direct communication to the target UE no longer needed;
                          //  #4	direct connection is not available anymore;
                          //  #5	lack of resources for PC5 unicast link; 
                          //  #13 congestion situation; or
                          //  #111	protocol error, unspecified.
                          //In this case, we will go with cause #2
                          uint8_t cause = 2; //Direct communication to the target UE no longer needed
                          it->second->m_link->StartConnectionRelease (cause);
                        }
                    }
                  
                  //Add the relay that this remote is trying to connect to
                  m_connectingRelay = newRelay;
                  
                  //Create new link with newly selected relay 
                  //Remote UE (Initiating UE)
                  AddDirectLinkConnection (m_l2Id, remoteIp, newRelay.l2Id, true, true, newRelay.relayCode);
                }
            }
          //No relay gets selected: check if we have an ongoing connection and release it
          else
            {
              if (m_currentSelectedRelay.l2Id != 0)
                {
                  //Get existing link
                  auto it = m_unicastDirectLinks.find (m_currentSelectedRelay.l2Id);
                  if (it != m_unicastDirectLinks.end ())
                    {
                      //Initiate release procedure with current relay/link
                      //According to 3GPP TS 24.554, the cause of link release can be:
                      //  #1	direct communication to the target UE not allowed;
                      //  #2	direct communication to the target UE no longer needed;
                      //  #4	direct connection is not available anymore;
                      //  #5	lack of resources for PC5 unicast link; 
                      //  #13 congestion situation; or
                      //  #111	protocol error, unspecified.
                      //In this case, we will go with cause #4
                      uint8_t cause = 4; //Direct communication is no longer available
                      it->second->m_link->StartConnectionRelease (cause);
 
                      //Trace the change of the previously selected relay to no relay
                      m_relaySelectionTrace (m_l2Id, m_currentSelectedRelay.l2Id, 0, 0, 0);
                    }
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
      
      //This will be the case e.g. when the Relay UE receives a request from the Remote UE
      //after the relay discovery procedure.
      //In that case, the Relay UE should:
      //1. Create the corresponding context and direct link instance.
      //2. Pass the packet to the corresponding direct link instance
            
      NrSlPc5SignallingMessageType header;
      packet->PeekHeader (header);
      uint8_t msgType = header.GetMessageType ();
      
      if (msgType == NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentRequest) 
        {
          ProseDirectLinkEstablishmentRequest reqHeader;
          packet->PeekHeader (reqHeader);

          //Relay UE (Target UE) 
          Ipv4Address relayIp = m_ueDevice->GetNode ()->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal (); 
          uint32_t relayCode = reqHeader.GetRelayServiceCode ();

          //Create the new link and related context
          AddDirectLinkConnection (m_l2Id, relayIp, srcL2Id, false, true, relayCode);

          //Add trace for received establishment request
          m_pc5SignallingPacketTrace (srcL2Id, m_l2Id, false, packet);

          //Pass the packet to the corresponding direct link instance
          auto lnk = m_unicastDirectLinks.find (srcL2Id);
          lnk->second->m_nrSlUeProseDirLnkSapProvider->ReceiveNrSlPc5Message (packet);
        }
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

                //If this is a remote, switch connectingRelay to currentSelectedRelay and reset connectingRelay
                if (info.relayInfo.role == NrSlUeProseDirLnkSapUser::RemoteUe)
                  {
                    if (m_connectingRelay.l2Id != 0)
                      {
                        NS_ASSERT_MSG (m_connectingRelay.l2Id == peerL2Id, "It is not the same relay this remote was trying to establish a one-to-one communication with!");
                        m_currentSelectedRelay = m_connectingRelay;
                        m_connectingRelay.l2Id = 0;
                      }
                  }

                //Depending on the UE Role, we need to tell the NAS to (re)configure the data bearers
                //to have the data packets flowing in the appropriate path
                ConfigureDataRadioBearersForU2nRelay (peerL2Id, info.relayInfo, info.ipInfo);
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
        
        // Remote: Remove the tx data bearer
        if (info.relayInfo.isRelayConn)
          {
            NS_LOG_INFO ("info.ipInfo.selfIpv4Addr: " << info.ipInfo.selfIpv4Addr);

            if (info.relayInfo.role == NrSlUeProseDirLnkSapUser::RemoteUe)
              {
                NS_LOG_FUNCTION ("The remote is initiating a ReleaseRequest with the relay!");
              
                //Reconfigure data bearers to take into account the release of the link
                RemoveDataRadioBearersForU2nRelay (peerL2Id, info.relayInfo, info.ipInfo);    
              }
          }
        break;
      case NrSlUeProseDirectLink::RELEASED:
        NS_LOG_INFO ("RELEASED");
 
        if (info.relayInfo.isRelayConn)
          {
            NS_LOG_INFO ("info.ipInfo.selfIpv4Addr: " << info.ipInfo.selfIpv4Addr);

            //Remote
            if (info.relayInfo.role == NrSlUeProseDirLnkSapUser::RemoteUe)
              {
                NS_LOG_FUNCTION ("This is a remote in RELEASED state. A ReleaseAccept was received from the relay!");
                
                //Reset connectingRelay in the case of an incomplete connection establishment
                if (m_connectingRelay.l2Id == peerL2Id)
                  {
                    m_connectingRelay.l2Id = 0;
                  }
                //Reset currentSelectedRelay
                if (m_currentSelectedRelay.l2Id == peerL2Id)
                  {
                    m_currentSelectedRelay.l2Id = 0;
                  }
            
                //Notify the RRC to delete the Rx sidelink data bearer for this remote in connection with the removed relay
                m_nrSlUeSvcRrcSapProvider->NotifySidelinkConnectionRelease (peerL2Id, m_l2Id);
                //Remove context
                m_unicastDirectLinks.erase (peerL2Id);
              }
            //Relay
            else
              {
                NS_LOG_FUNCTION ("This is a relay in RELEASED state. A ReleaseAccept was sent to the remote!");

                //Notify the RRC to delete the Rx sidelink data bearer for this relay in connection with the removed remote
                m_nrSlUeSvcRrcSapProvider->NotifySidelinkConnectionRelease (peerL2Id, m_l2Id);

                //Reconfigure data bearers to take into account the release of the link       
                RemoveDataRadioBearersForU2nRelay (peerL2Id, info.relayInfo, info.ipInfo);
                //We don't remove context yet (in case the ReleaseAccept message doesn't get received by the remote)
                //The context may get deleted after all possible retransmissions has ended
                //For this, we would have to keep track of the retransmission timer and the maximum number of transmissions
                //m_unicastDirectLinks.erase (peerL2Id);
              }
          }
        else
          {
            NS_LOG_INFO ("Instructing the removal of SL-DRB with peer Ipv4 address " << info.ipInfo.peerIpv4Addr);
            // Tx bearers
            DeleteDirectLinkDataRadioBearer (peerL2Id, info.ipInfo);
            // Rx bearers
            m_nrSlUeSvcRrcSapProvider->NotifySidelinkConnectionRelease (peerL2Id, m_l2Id);
          }
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
  auto it = m_unicastDirectLinks.find (peerL2Id);
  if (it == m_unicastDirectLinks.end ())
    {
      NS_FATAL_ERROR ("Could not find the direct link");
    }
  else
    {
      //Create unicast TFT to be able to transmit to peer UE
      Ptr<LteSlTft> tft;
      tft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT,
                              LteSlTft::CommType::Unicast,
                              ipInfo.peerIpv4Addr, peerL2Id);
      m_nrSlUeSvcNasSapProvider->ActivateSvcNrSlDataRadioBearer (tft);
      it->second->m_hasPendingSlDrb = true;
    }
}

void
NrSlUeProse::ConfigureDataRadioBearersForU2nRelay (uint32_t peerL2Id,
                                                   NrSlUeProseDirLnkSapUser::DirectLinkRelayInfo relayInfo,
                                                   NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo)
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
  m_nrSlUeSvcNasSapProvider->ConfigureNrSlDataRadioBearersForU2nRelay (peerL2Id, relayInfo.role, ipInfo, relayDrbId);

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
NrSlUeProse::DeleteDirectLinkDataRadioBearer (uint32_t dstL2Id, NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo)
{
  NS_LOG_FUNCTION (this << dstL2Id << ipInfo.peerIpv4Addr);

  //Get link
  auto it = m_unicastDirectLinks.find (dstL2Id);
  if (it == m_unicastDirectLinks.end ())
    {
       NS_FATAL_ERROR ("Could not find the direct link");
    }
  else
    {
      NS_LOG_LOGIC ("SL-SRB found for this destination " << dstL2Id);
     
      //Create unicast TFT 
      Ptr<LteSlTft> tft;
      tft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT,
                            LteSlTft::CommType::Unicast,
                            ipInfo.peerIpv4Addr, dstL2Id);

      //Instruct the NAS to delata SL-DRB
      m_nrSlUeSvcNasSapProvider->DeleteSvcNrSlDataRadioBearer (tft);
    }
}

void
NrSlUeProse::RemoveDataRadioBearersForU2nRelay (uint32_t peerL2Id,
                                                   NrSlUeProseDirLnkSapUser::DirectLinkRelayInfo relayInfo,
                                                   NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo)
{
  NS_LOG_FUNCTION (this << peerL2Id << relayInfo.role << ipInfo.peerIpv4Addr);

  uint8_t relayDrbId = 0;
  if (relayInfo.role == NrSlUeProseDirLnkSapUser::RelayUe)
    { 
      //Tell the EPC helper to configure the EpcPgwApplication to remove the link between the remote UE and the relay UE
      m_epcHelper->RemoveRemoteUe (m_imsi, ipInfo.peerIpv4Addr);

      //Find data relay radio bearer id for the service
      auto it = m_l3U2nRelayProvidedSvcs.find (relayInfo.relayServiceCode);
      if (it == m_l3U2nRelayProvidedSvcs.end ())
        {
          NS_FATAL_ERROR ("Could not find  and delete the U2N relay service with relay service code :" << relayInfo.relayServiceCode);
        }
      else
        {
          relayDrbId = it->second.relayDrbId;
          NS_ASSERT_MSG (relayDrbId != 0, "The relay data radio bearer is unknown!");
        }
    }

  //Tell the NAS to remove the UL and SL data bearers to have the data packets flowing in the appropriate path
  m_nrSlUeSvcNasSapProvider->RemoveNrSlDataRadioBearersForU2nRelay (peerL2Id, relayInfo.role, ipInfo, relayDrbId);

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

uint32_t 
NrSlUeProse::GetL2Id () const
{
  return m_l2Id;
}

void
NrSlUeProse::SetEpcHelper (const Ptr<NrPointToPointEpcHelper> &epcHelper)
{
  NS_LOG_FUNCTION (this);
  m_epcHelper = epcHelper;
}

void
NrSlUeProse::SetRelaySelectionAlgorithm (Ptr<NrSlUeProseRelaySelectionAlgorithm> selectionAlgorithm)
{
  NS_LOG_FUNCTION (this);
  m_relaySelectionAlgorithm = selectionAlgorithm;
}

void
NrSlUeProse::SetNetDevice (Ptr<NetDevice> dev)
{
  NS_LOG_FUNCTION (this);
  m_ueDevice = dev;
}

} // namespace ns3
