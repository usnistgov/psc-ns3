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

#include "lte-sl-ue-controller-camad-2019.h"
#include "ns3/epc-ue-nas.h"
#include "ns3/abort.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlUeControllerCamad2019");
NS_OBJECT_ENSURE_REGISTERED (LteSlUeControllerCamad2019);

LteSlUeControllerCamad2019::LteSlUeControllerCamad2019 ()
  : m_connectingRelayUeId (std::numeric_limits<uint32_t>::max ())
{
  NS_LOG_FUNCTION (this);
  m_slUeCtrlSapProvider = new MemberLteSlUeCtrlSapProvider<LteSlUeControllerCamad2019> (this);

  m_relayDiscProbRndVar = CreateObject<UniformRandomVariable> ();
  m_relayDiscProbRndVar->SetAttribute ("Min", DoubleValue (0.0));
  m_relayDiscProbRndVar->SetAttribute ("Max", DoubleValue (1.0));
  m_connectionCount = 0;
  m_haveConnected = false;
}

LteSlUeControllerCamad2019::~LteSlUeControllerCamad2019 ()
{
  NS_LOG_FUNCTION (this);
  delete m_slUeCtrlSapProvider;
}

TypeId
LteSlUeControllerCamad2019::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::LteSlUeControllerCamad2019")
    .SetParent<LteSlUeController> ()
    .SetGroupName ("Lte")
    .AddConstructor<LteSlUeControllerCamad2019> ()
    .AddAttribute ("NetDevice",
                   "The Lte NetDevice",
                   PointerValue (),
                   MakePointerAccessor (&LteSlUeControllerCamad2019::m_netDevice),
                   MakePointerChecker <LteUeNetDevice> ())
    .AddAttribute ("ProseHelper",
                   "The Lte Sidelink Helper",
                   PointerValue (),
                   MakePointerAccessor (&LteSlUeControllerCamad2019::m_lteSidelinkHelper),
                   MakePointerChecker <LteSidelinkHelper> ())
    .AddAttribute ("RelayDiscProb",
                   "The Selection Probability of a Relay for Communication once it is discovered",
                   DoubleValue (100),
                   MakeDoubleAccessor (&LteSlUeControllerCamad2019::m_relayDiscProb),
                   MakeDoubleChecker <double> ())
    .AddAttribute ("Campaign",
                   "The campaign that will be simulated (Discovery/Connection/Communication)",
                   StringValue ("Discovery"),
                   MakeStringAccessor (&LteSlUeControllerCamad2019::SetCampaign),
                   MakeStringChecker ())
    .AddTraceSource ("DiscoversAllRelays",
                     "The Remote UE discover all the Relay UEs.",
                     MakeTraceSourceAccessor (&LteSlUeControllerCamad2019::m_traceDiscoversAllRelays),
                     "ns3::LteSlUeControllerCamad2019::DiscoveryEventTracedCallback")
    .AddAttribute ("MaxConnectionAttempts",
                   "How many times the Remote UE should try to connect",
                   DoubleValue (1),
                   MakeDoubleAccessor (&LteSlUeControllerCamad2019::m_maxConnectionAttempts),
                   MakeDoubleChecker <double> ())
    .AddTraceSource ("RemoteConnectionStart",
                     "Traces when the Remote UE starts the one-to-one connection establishment with a Relay UE.",
                     MakeTraceSourceAccessor (&LteSlUeControllerCamad2019::m_traceRemoteConnectionStart),
                     "ns3::LteSlUeControllerCamad2019::RemoteConnectionStartTracedCallback")
    .AddTraceSource ("RemoteConnectionEstablished",
                     "Traces when the Remote UE completes the one-to-one connection establishment with a Relay UE.",
                     MakeTraceSourceAccessor (&LteSlUeControllerCamad2019::m_traceRemoteConnectionEstablished),
                     "ns3::LteSlUeControllerCamad2019::RemoteConnectionEstablishedTracedCallback")
  ;
  return tid;
}

void
LteSlUeControllerCamad2019::DoDispose ()
{
}

void
LteSlUeControllerCamad2019::SetCampaign (std::string campaign)
{
  NS_LOG_FUNCTION (this << campaign);
  if (campaign.compare ("Discovery") != 0
      && campaign.compare ("Connection") != 0
      && campaign.compare ("Communication") != 0 )
    {
      NS_FATAL_ERROR ("The campaign " << campaign << " does not exist." );
    }
  m_campaign = campaign;
}

void
LteSlUeControllerCamad2019::SetStartRelayServiceTime (Time t)
{
  NS_LOG_FUNCTION (this <<  t.GetSeconds ());

  m_startRelayServiceTime = t;
}

void
LteSlUeControllerCamad2019::SetDiscoveryPeriodDuration (uint32_t period)
{
  NS_LOG_FUNCTION (this << period);

  m_discPeriodDuration = period;
}

void
LteSlUeControllerCamad2019::SetNumberProseRelaysToDiscover (uint32_t nRelays)
{
  NS_LOG_FUNCTION (this << nRelays);

  m_nProseRelaysToDiscover = nRelays;
}

void LteSlUeControllerCamad2019::DoRecvRelayServiceDiscovery (uint32_t serviceCode, uint64_t announcerInfo, uint32_t proseRelayUeId, uint8_t statusIndicator)
{
  NS_LOG_FUNCTION (this << serviceCode << announcerInfo << proseRelayUeId << statusIndicator);

  if (m_campaign.compare ("Discovery") == 0)
    {

      std::map <uint32_t, Time>::iterator it = m_dicoveredRelayUeIds.find (proseRelayUeId);
      if (it == m_dicoveredRelayUeIds.end ())
        {
          //First discovery of this proseRelayId

          //Store
          m_dicoveredRelayUeIds.insert (std::pair <uint32_t, Time> (proseRelayUeId, Simulator::Now ()));

          //Calculate time taken for discover this proseRelayId
          Time discoverTime = Simulator::Now () - m_startRelayServiceTime;

          //Calculate number of periods taken to discover this proseRelayId
          uint32_t nDiscPeriods = std::ceil ((double)discoverTime.GetMilliSeconds () / m_discPeriodDuration);

          //Get my Ids
          uint32_t myImsi = m_netDevice->GetImsi ();
          Ptr<LteUeRrc> rrc = m_netDevice->GetRrc ();
          PointerValue ptrOne;
          rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
          Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();
          uint32_t myL2Id = slUeRrc->GetSourceL2Id ();

          bool allRelaysDiscovered = (m_nProseRelaysToDiscover == m_dicoveredRelayUeIds.size ()) ? true : false;
          NS_LOG_INFO (" UE Imsi: " << myImsi
                                    << " UE L2Id: " << myL2Id
                                    << " discovered proseRelayId " << proseRelayUeId
                                    << " nDiscoveredRelays " << m_dicoveredRelayUeIds.size ()
                                    << " discoverTime (s) " << discoverTime.GetSeconds ()
                                    << " nDiscPeriods " << nDiscPeriods
                                    << " allRelaysDiscovered " << allRelaysDiscovered
                       );

          if (allRelaysDiscovered)
            {
              m_traceDiscoversAllRelays (myL2Id, proseRelayUeId, discoverTime.GetSeconds (), nDiscPeriods);
            }
        }
      else
        {
          //Already discovered this relay, do nothing
          NS_LOG_INFO ("Already know this Relay UE");
        }
    }
}

void
LteSlUeControllerCamad2019::DoPc5ConnectionStarted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role)
{
  NS_LOG_FUNCTION (this << peerUeId << selfUeId << role);
}

void LteSlUeControllerCamad2019::DoPc5SecuredEstablished (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role)
{
  NS_LOG_FUNCTION (this << peerUeId << selfUeId << role);

  if (m_campaign.compare ("Discovery") == 0)
    {
      NS_LOG_ERROR ( "This controller should not initiate relay communication when running the Discovery Campaign");
      return;
    }

  //Make sure we don't already have a connection
  std::map < uint32_t, Ptr<LteSlUeNetDevice> >::iterator it = m_lteSlUeNetDeviceMap.find (peerUeId);
  if (m_connectingRelayUeId != std::numeric_limits<uint32_t>::max ())
    {
      NS_ASSERT_MSG (it == m_lteSlUeNetDeviceMap.end (), "PC5 connection between " << selfUeId << " and " << peerUeId << " already established");
      NS_ASSERT_MSG (m_connectingRelayUeId == peerUeId, "PC5 connection established to a relay it was not trying to connect");
      m_connectingRelayUeId = std::numeric_limits<uint32_t>::max ();
    }

  // If the UE is a Relay UE and has already a connection with that Remote UE,
  // tear it down and proceed to start a new one
  // This happens e.g., when the DirectCommunicationAccept is lost: the Relay established the connection
  // but not the Remote, and the Remote try to connect again before the local release (in the relay) of the previous connection

  // TS 24.334 10.4.2.6.2 Abnormal cases at the target UE
  // "... if the target UE already has an existing link established to the UE known to use this Layer 2 ID
  // and the new request contains an identical User Info as the known user, the UE shall process the new request. However, the target UE shall only delete
  // the existing link context after the new link setup procedure succeeds,
  // or the link keepalive procedure as described in subclause 10.4.3 fails.
  if (it != m_lteSlUeNetDeviceMap.end () && role == LteSlUeRrc::RelayUE)
    {
      //Terminate the current connection
      DoPc5ConnectionTerminated (peerUeId, selfUeId, role);

      //Proceed to start the new one
      it = m_lteSlUeNetDeviceMap.end ();
    }

  if (it == m_lteSlUeNetDeviceMap.end ())
    {
      NS_LOG_DEBUG (" " << bool(m_campaign.compare ("Connection") == 0));

      if (m_campaign.compare ("Connection") == 0)
        {
          NS_LOG_DEBUG (" " <<  role << " " << LteSlUeRrc::RemoteUE << " " <<  bool(role == LteSlUeRrc::RemoteUE));
          //Tracing Remote connection time
          if (role == LteSlUeRrc::RemoteUE)
            {
              m_haveConnected = true;
              double connTimeSeconds = Simulator::Now ().GetSeconds () - m_remoteStartedConnectionTime.GetSeconds ();
              m_traceRemoteConnectionEstablished (selfUeId, peerUeId, m_connectionCount, connTimeSeconds);
              NS_LOG_DEBUG ("selfUeId " << selfUeId << " peerUeId " << peerUeId
                                        << " m_connectionCount " << m_connectionCount << " connTimeSeconds " << connTimeSeconds);
            }
        }

      //Create a new UeNetDevice for sidelink communication
      Ptr<LteSlUeNetDevice> slNetDev = CreateObject<LteSlUeNetDevice> ();
      slNetDev->SetNas (m_netDevice->GetNas ());
      Ptr<Node> node = m_netDevice->GetNode ();
      slNetDev->SetNode (node);
      Mac64Address slNetDevAddress = Mac64Address::Allocate ();
      NS_LOG_DEBUG ("LteNetDevice address " << m_netDevice->GetAddress () << " Ipv6: " << node->GetObject<Ipv6L3Protocol> ()->GetAddress (1,1).GetAddress ());
      NS_LOG_DEBUG ("Created new LteSlNetDevice with address " << slNetDevAddress);
      slNetDev->SetAddress (slNetDevAddress);
      slNetDev->Initialize ();
      node->AddDevice (slNetDev);
      m_lteSlUeNetDeviceMap.insert (std::pair<uint32_t, Ptr<LteSlUeNetDevice> > (peerUeId, slNetDev));

      //Configure IP addresses as well as ingress and egress TFTs
      Ptr<Ipv6> ipv6 = node->GetObject<Ipv6> ();

      if (role == LteSlUeRrc::RemoteUE)
        {
          Ipv6InterfaceContainer ifUp = m_lteSidelinkHelper->AssignIpv6AddressForRelayCommunication (slNetDev, peerUeId, selfUeId, role);
          uint32_t ipInterfaceIndex = ipv6->GetInterfaceForDevice (slNetDev);
          NS_LOG_DEBUG ("New LteSlNetDevice in remote node has interface index " << ipInterfaceIndex);

          //add route
          Ipv6Address ipv6a_relay = m_lteSidelinkHelper->GetRelayIpv6AddressFromMap (peerUeId, selfUeId, role);
          Ipv6StaticRoutingHelper ipv6RoutingHelper;
          Ptr<Ipv6StaticRouting> ue2ueStaticRouting = ipv6RoutingHelper.GetStaticRouting (ipv6);
          ue2ueStaticRouting->SetDefaultRoute (ipv6a_relay, ipInterfaceIndex);

          //add ingress filter so the NAS can send packets up via the right netdevice
          //accepts packets where the destination is his local address
          Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::RECEIVE, LteSlTft::LOCAL, ifUp.GetAddress (0,1), peerUeId);
          m_netDevice->GetNas ()->SetForwardUpCallback (tft, MakeCallback (&LteSlUeNetDevice::Receive, slNetDev));

          //configure TFT to be able to send packets through the sidelink radio bearer
          tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, LteSlTft::LOCAL, ifUp.GetAddress (0,1), peerUeId);
          m_netDevice->GetNas ()->ActivateSidelinkBearer (tft);
        }

      if (role == LteSlUeRrc::RelayUE)
        {
          Ipv6InterfaceContainer ifUp = m_lteSidelinkHelper->AssignIpv6AddressForRelayCommunication (slNetDev, selfUeId, peerUeId, role);
          uint32_t ipInterfaceIndex = ipv6->GetInterfaceForDevice (slNetDev);
          NS_LOG_DEBUG ("New LteSlNetDevice in relay node has interface index " << ipInterfaceIndex);
          //Since it is a relay, it needs to forward packets between sidelink and uplink/downlink netdevices
          ipv6->SetForwarding (ipInterfaceIndex, true);

          //Also the interface connecting to the network need to forward packets
          uint32_t ipInterfaceNetDevIndex = ipv6->GetInterfaceForDevice (m_netDevice);
          NS_LOG_DEBUG ("NetDevice in relay node has interface index " << ipInterfaceNetDevIndex);
          ipv6->SetForwarding (ipInterfaceNetDevIndex, true);


          //Ipv6StaticRoutingHelper ipv6RoutingHelper;
          //Ptr<Ipv6StaticRouting> ue2ueStaticRouting = ipv6RoutingHelper.GetStaticRouting (node->GetObject<Ipv6> ());
          //ue2ueStaticRouting->AddNetworkRouteTo (ifUp.GetAddress (0,1), Ipv6Prefix(64), "::",2, 0);

          //add ingress filter so the NAS can send packets up via the right netdevice
          //accepts packets coming from any address within the prefix used for relay
          Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::RECEIVE, LteSlTft::REMOTE, ifUp.GetAddress (0,1), Ipv6Prefix (64), peerUeId);
          m_netDevice->GetNas ()->SetForwardUpCallback (tft, MakeCallback (&LteSlUeNetDevice::Receive, slNetDev));

          //configure TFT to be able to send packets through the sidelink radio bearer
          tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, LteSlTft::REMOTE, ifUp.GetAddress (0,1), Ipv6Prefix (64), peerUeId);
          m_netDevice->GetNas ()->ActivateSidelinkBearer (tft);
        }
    }
}

void
LteSlUeControllerCamad2019::DoPc5ConnectionTerminated (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role)
{
  NS_LOG_FUNCTION (this << peerUeId << selfUeId << role);

  if (m_campaign.compare ("Discovery") == 0)
    {
      NS_LOG_ERROR ( "This controller should not initiate relay communication when running the Discovery Campaign");
      return;
    }

  std::map < uint32_t, Ptr<LteSlUeNetDevice> >::iterator it = m_lteSlUeNetDeviceMap.find (peerUeId);
  NS_ASSERT_MSG (it != m_lteSlUeNetDeviceMap.end (), "Could not find an associated interface");

  Ptr<LteSlUeNetDevice> slNetDev = it->second;

  Ptr<Node> node = m_netDevice->GetNode ();
  Ptr<Ipv6> ipv6 = node->GetObject<Ipv6> ();
  uint32_t ipInterfaceIndex = ipv6->GetInterfaceForDevice (slNetDev);
  ipv6->SetDown (ipInterfaceIndex);

  if (role == LteSlUeRrc::RemoteUE)
    {
      Ipv6Address ipv6a = m_lteSidelinkHelper->GetSelfIpv6AddressFromMap (peerUeId, selfUeId, role);

      Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::RECEIVE, LteSlTft::LOCAL, ipv6a, peerUeId);
      m_netDevice->GetNas ()->RemoveForwardUpCallback (tft);

      tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, LteSlTft::REMOTE, ipv6a, Ipv6Prefix (64), peerUeId);
      m_netDevice->GetNas ()->DeactivateSidelinkBearer (tft);

      ipv6->RemoveAddress (ipInterfaceIndex, ipv6a);
    }

  if (role == LteSlUeRrc::RelayUE)
    {
      Ipv6Address ipv6a = m_lteSidelinkHelper->GetSelfIpv6AddressFromMap (selfUeId, peerUeId, role);

      Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::RECEIVE, LteSlTft::REMOTE, ipv6a, Ipv6Prefix (64), peerUeId);
      m_netDevice->GetNas ()->RemoveForwardUpCallback (tft);

      tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, LteSlTft::REMOTE, ipv6a, Ipv6Prefix (64), peerUeId);
      m_netDevice->GetNas ()->DeactivateSidelinkBearer (tft);

      ipv6->RemoveAddress (ipInterfaceIndex, ipv6a);
    }
  node->RemoveDevice (slNetDev);
  m_lteSlUeNetDeviceMap.erase (peerUeId);

}

void
LteSlUeControllerCamad2019::DoPc5ConnectionAborted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role, LteSlO2oCommParams::UeO2ORejectReason reason)
{
  NS_LOG_FUNCTION (this << peerUeId << selfUeId << role << reason);

  if (m_campaign.compare ("Discovery") == 0)
    {
      NS_LOG_ERROR ( "This controller should not initiate relay communication when running the Discovery Campaign");
      return;
    }

  NS_ASSERT (role == LteSlUeRrc::RemoteUE);
  if (m_connectingRelayUeId == peerUeId)
    {
      //reset connecting UE to enable reconnection attempt
      m_connectingRelayUeId = std::numeric_limits<uint32_t>::max ();
    }
}

void
LteSlUeControllerCamad2019::DoRecvRemoteUeReport (uint64_t localImsi, uint32_t peerUeId, uint64_t remoteImsi)
{
  NS_LOG_FUNCTION (this << localImsi << peerUeId << remoteImsi);

  if (m_campaign.compare ("Discovery") == 0)
    {
      NS_LOG_ERROR ( "This controller should not initiate relay communication when running the Discovery Campaign");
      return;
    }

  //for now we let the core know about the new UE. Normally this is done by the relay node
  //by transmitting a Remote UE report (24.301 6.6.3)
  std::map < uint32_t, Ptr<LteSlUeNetDevice> >::iterator it = m_lteSlUeNetDeviceMap.find (peerUeId);
  NS_ASSERT_MSG (it != m_lteSlUeNetDeviceMap.end (), "Unknown remote UE id " << peerUeId);
  Ptr<Ipv6> ipv6 = m_netDevice->GetNode ()->GetObject<Ipv6> ();
  uint32_t ipInterfaceIndex = ipv6->GetInterfaceForDevice ((*it).second);

  //Now we can report the /64 IPv6 Prefix of the remote UE
  uint8_t relayAddr[16];
  ipv6->GetAddress (ipInterfaceIndex, 1).GetAddress ().GetBytes (relayAddr);
  uint8_t remotePrefix[8];
  std::memmove (remotePrefix, relayAddr, 8);

  m_lteSidelinkHelper->RemoteUeContextConnected (localImsi, remoteImsi, remotePrefix);

}

uint64_t
LteSlUeControllerCamad2019::DoRelayUeSelection (std::map <uint64_t, double> validRelays, uint32_t serviceCode, uint64_t currentRelayId)
{
  NS_LOG_FUNCTION (this << currentRelayId);

  if (m_campaign.compare ("Discovery") == 0)
    {
      //This controller should not do any Relay UE (re)selection.
      //This function will be called upon measurement report and it will return the currentRelayId
      //so that there is no action taken in LteSlUeRrc::RelayUeSelection
      return currentRelayId;
    }
  else
    {

      if (m_campaign.compare ("Connection") == 0)
        {
          //In the Connection campaign we are only interested in the first successful connection in simTime
          //If the Remote UE already had a connection, it should not try to connect
          //If the Remote UE has reach the maximum attempts without being successful, it should not try to connect
          if (m_haveConnected || m_connectionCount >= m_maxConnectionAttempts)
            {
              return currentRelayId;
            }
        }


      // The connection and communication scenarios have only 1 Relay UE
      // This algorithm will return
      // - Zero if the Relay UE was not detected and the UE is not connected
      //   to the Relay UE already, or
      // - The Relay UE ID of the Relay UE if not connected to it already, or
      // - The Relay UE ID of the Relay UE if already connected to it
      uint64_t selectedRelayId = 0;

      if (currentRelayId == 0)
        {
          NS_LOG_INFO (validRelays.size () << " valid Relay UEs");
          if (validRelays.size () > 0)
            {
              std::map <uint64_t, double>::iterator vaReIt;
              vaReIt = validRelays.begin ();
              selectedRelayId = vaReIt->first;

              if (m_campaign.compare ("Connection") == 0)
                {
                  //Trace the connection start event
                  m_remoteStartedConnectionTime = Simulator::Now ();
                  m_connectionCount++;

                  Ptr<LteUeRrc> rrc = m_netDevice->GetRrc ();
                  PointerValue ptrOne;
                  rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
                  Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();
                  uint32_t myL2Id = slUeRrc->GetSourceL2Id ();

                  m_traceRemoteConnectionStart (myL2Id, selectedRelayId, m_connectionCount);
                }
            }
          else
            {
              selectedRelayId = 0;
            }
        }
      if (currentRelayId != 0)
        {
          selectedRelayId = currentRelayId;
        }
      NS_LOG_INFO ("Current Relay ID " << currentRelayId << " Selected Relay ID " << selectedRelayId);

      return selectedRelayId;
    }
}

void
LteSlUeControllerCamad2019::SetLteSlUeCtrlSapUser (LteSlUeCtrlSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_slUeCtrlSapUser = s;
}

LteSlUeCtrlSapProvider*
LteSlUeControllerCamad2019::GetLteSlUeCtrlSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_slUeCtrlSapProvider;
}


} // namespace ns3
