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

#include "lte-sl-basic-ue-controller.h"
#include "ns3/epc-ue-nas.h"
#include "ns3/abort.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlBasicUeController");
NS_OBJECT_ENSURE_REGISTERED (LteSlBasicUeController);

LteSlBasicUeController::LteSlBasicUeController ()
  : m_connectingRelayUeId (std::numeric_limits<uint32_t>::max ())
{
  NS_LOG_FUNCTION (this);
  m_slUeCtrlSapProvider = new MemberLteSlUeCtrlSapProvider<LteSlBasicUeController> (this);
  m_relayDiscProbRndVar = CreateObject<UniformRandomVariable> ();
}

LteSlBasicUeController::~LteSlBasicUeController ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
LteSlBasicUeController::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::LteSlBasicUeController")
    .SetParent<LteSlUeController> ()
    .SetGroupName ("Lte")
    .AddConstructor<LteSlBasicUeController> ()
    .AddAttribute ("NetDevice",
                   "The Lte NetDevice",
                   PointerValue (),
                   MakePointerAccessor (&LteSlBasicUeController::m_netDevice),
                   MakePointerChecker <LteUeNetDevice> ())
    .AddAttribute ("ProseHelper",
                   "The Lte Sidelink Helper",
                   PointerValue (),
                   MakePointerAccessor (&LteSlBasicUeController::m_lteSidelinkHelper),
                   MakePointerChecker <LteSidelinkHelper> ())
    .AddAttribute ("RelayUeSelectionAlgorithm",
                   "The algorithm to be used to evaluate Relay UE (re)selection upon measurements report (RandomNoReselection/MaxSDRsrpNoReselection/MaxSDRsrp)",
                   EnumValue (LteSlBasicUeController::RelayUeSelectionAlgorithm::RANDOM_NO_RESELECTION),
                   MakeEnumAccessor (&LteSlBasicUeController::m_relayUeSelectionAlgorithm),
                   MakeEnumChecker (LteSlBasicUeController::RelayUeSelectionAlgorithm::RANDOM_NO_RESELECTION, "RandomNoReselection",
                                    LteSlBasicUeController::RelayUeSelectionAlgorithm::MAX_SDRSRP_NO_RESELECTION, "MaxSDRsrpNoReselection",
                                    LteSlBasicUeController::RelayUeSelectionAlgorithm::MAX_SDRSRP, "MaxSDRsrp"))
    .AddTraceSource ("RelayUeSelection",
                     "Traces when the Remote UE selects a new Relay UE .",
                     MakeTraceSourceAccessor (&LteSlBasicUeController::m_traceRelayUeSelection),
                     "ns3::LteSlBasicUeController::RelayUeSelectionTracedCallback")
    .AddTraceSource ("Pc5ConnectionStatus",
                     "Traces when there is a change in the status of a one-to-one connection.",
                     MakeTraceSourceAccessor (&LteSlBasicUeController::m_tracePc5ConnectionStatus),
                     "ns3::LteSlBasicUeController::Pc5ConnectionStatusTracedCallback")

  ;
  return tid;
}

void
LteSlBasicUeController::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_slUeCtrlSapProvider;
}

void LteSlBasicUeController::DoRecvRelayServiceDiscovery (uint32_t serviceCode, uint64_t announcerInfo, uint32_t proseRelayUeId, uint8_t statusIndicator)
{
  NS_LOG_FUNCTION (this << serviceCode << announcerInfo << proseRelayUeId << statusIndicator);
}

void
LteSlBasicUeController::DoPc5ConnectionStarted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role)
{
  NS_LOG_FUNCTION (this << peerUeId << selfUeId << role);
  m_tracePc5ConnectionStatus (selfUeId, peerUeId, role, LteSlBasicUeController::Pc5ConnectionStatus::STARTED, 0);
}

void LteSlBasicUeController::DoPc5SecuredEstablished (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role)
{
  NS_LOG_FUNCTION (this << peerUeId << selfUeId << role);

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
      uint32_t ipInterfaceIndex = UINT32_MAX;

      if (role == LteSlUeRrc::RemoteUE)
        {
          Ipv6InterfaceContainer ifUp = m_lteSidelinkHelper->AssignIpv6AddressForRelayCommunication (slNetDev, peerUeId, selfUeId, role);
          ipInterfaceIndex = ipv6->GetInterfaceForDevice (slNetDev);
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
          ipInterfaceIndex = ipv6->GetInterfaceForDevice (slNetDev);
          NS_LOG_DEBUG ("New LteSlNetDevice in relay node has interface index " << ipInterfaceIndex);
          //Since it is a relay, it needs to forward packets between sidelink and uplink/downlink netdevices
          ipv6->SetForwarding (ipInterfaceIndex, true);

          //Also the interface connecting to the network need to forward packets
          uint32_t ipInterfaceNetDevIndex = ipv6->GetInterfaceForDevice (m_netDevice);
          NS_LOG_DEBUG ("NetDevice in relay node has interface index " << ipInterfaceNetDevIndex);
          ipv6->SetForwarding (ipInterfaceNetDevIndex, true);

          //add ingress filter so the NAS can send packets up via the right netdevice
          //accepts packets coming from any address within the prefix used for relay
          Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::RECEIVE, LteSlTft::REMOTE, ifUp.GetAddress (0,1), Ipv6Prefix (64), peerUeId);
          m_netDevice->GetNas ()->SetForwardUpCallback (tft, MakeCallback (&LteSlUeNetDevice::Receive, slNetDev));

          //configure TFT to be able to send packets through the sidelink radio bearer
          tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, LteSlTft::REMOTE, ifUp.GetAddress (0,1), Ipv6Prefix (64), peerUeId);
          m_netDevice->GetNas ()->ActivateSidelinkBearer (tft);
        }
        m_tracePc5ConnectionStatus (selfUeId, peerUeId, role, LteSlBasicUeController::Pc5ConnectionStatus::SECURE_ESTABLISHED, ipInterfaceIndex);
    }
}

void
LteSlBasicUeController::DoPc5ConnectionTerminated (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role)
{
  NS_LOG_FUNCTION (this << peerUeId << selfUeId << role);

  std::map < uint32_t, Ptr<LteSlUeNetDevice> >::iterator it = m_lteSlUeNetDeviceMap.find (peerUeId);
  NS_ASSERT_MSG (it != m_lteSlUeNetDeviceMap.end (), "Could not find an associated interface");

  Ptr<LteSlUeNetDevice> slNetDev = it->second;

  Ptr<Node> node = m_netDevice->GetNode ();
  Ptr<Ipv6> ipv6 = node->GetObject<Ipv6> ();
  uint32_t ipInterfaceIndex = ipv6->GetInterfaceForDevice (slNetDev);
  ipv6->SetDown (ipInterfaceIndex);

  m_tracePc5ConnectionStatus (selfUeId, peerUeId, role, LteSlBasicUeController::Pc5ConnectionStatus::TERMINATED, ipInterfaceIndex);

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
LteSlBasicUeController::DoPc5ConnectionAborted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role, LteSlO2oCommParams::UeO2ORejectReason reason)
{
  NS_LOG_FUNCTION (this << peerUeId << selfUeId << role << reason);

  NS_ASSERT (role == LteSlUeRrc::RemoteUE);

  m_tracePc5ConnectionStatus (selfUeId, peerUeId, role, LteSlBasicUeController::Pc5ConnectionStatus::ABORTED, 0);

  if (m_connectingRelayUeId == peerUeId)
    {
      //reset connecting UE to enable reconnection attempt
      m_connectingRelayUeId = std::numeric_limits<uint32_t>::max ();
    }
}

void
LteSlBasicUeController::DoRecvRemoteUeReport (uint64_t localImsi, uint32_t peerUeId, uint64_t remoteImsi)
{
  NS_LOG_FUNCTION (this << localImsi << peerUeId << remoteImsi);

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
LteSlBasicUeController::DoRelayUeSelection (std::map <uint64_t, double> validRelays, uint32_t serviceCode, uint64_t currentRelayId)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("UE IMSI " << m_netDevice->GetRrc ()->GetImsi ()
                           << " Current Relay UE ID " << currentRelayId << " for SC " << serviceCode);
  NS_LOG_DEBUG ("Number of valid Relay UEs available: " << validRelays.size ());

  uint64_t selectedRelayId = 0;

  //Are we in the middle of a connection procedure?
  if (m_connectingRelayUeId != std::numeric_limits<uint32_t>::max ())
    {
      //Yes: do nothing.
      selectedRelayId = currentRelayId;
      NS_LOG_DEBUG ("Ongoing connection procedure with Relay UE ID " << m_connectingRelayUeId
                                                                     << ". Skip (re)selection.");
    }
  else
    {
      //No: evaluate Relay UE (re)selection

      std::map <uint64_t, double>::iterator vaReIt;
      switch (m_relayUeSelectionAlgorithm)
        {
          case LteSlBasicUeController::RelayUeSelectionAlgorithm::RANDOM_NO_RESELECTION:
            // This algorithm will return
            // - The Relay UE ID of a randomly selected valid Relay UE if not connected
            //   to any Relay UE, or
            // - The Relay UE ID of the Relay UE to which the UE is already connected
            {
              NS_LOG_DEBUG ("Selection algorithm: RandomNoReselection");

              if (currentRelayId == 0 && validRelays.size () > 0)
                {
                  //Select a random entry in the validRelays map
                  //(advance iterator a random number of positions)
                  vaReIt = validRelays.begin ();
                  uint32_t pos = m_relayDiscProbRndVar->GetInteger (0, (validRelays.size () - 1));
                  std::advance (vaReIt, pos);

                  NS_ASSERT_MSG (vaReIt->first != 0, "Unable to find a valid Relay UE ID");

                  selectedRelayId = vaReIt->first;
                  NS_LOG_DEBUG ("UE IMSI " << m_netDevice->GetRrc ()->GetImsi ()
                                           << " selected Relay UE ID " << selectedRelayId << " for SC " << serviceCode);
                }
              else
                {
                  selectedRelayId = currentRelayId;
                  NS_LOG_DEBUG ("UE IMSI " << m_netDevice->GetRrc ()->GetImsi ()
                                           << " did not select a new Relay UE");
                }

              break;
            }
          case LteSlBasicUeController::RelayUeSelectionAlgorithm::MAX_SDRSRP_NO_RESELECTION:
            // This algorithm will return
            // - The Relay UE ID of the valid Relay UE with the strongest SD-RSRP
            //   if not connected to any Relay UE, or
            // - The Relay UE ID of the Relay UE to which the UE is already connected
            {
              NS_LOG_DEBUG ("Selection algorithm: MaxSDRsrpNoReselection");

              if (currentRelayId == 0 && validRelays.size () > 0)
                {
                  uint64_t maxSDRsrpRelayId = 0;
                  double maxSDRsrp = -std::numeric_limits<double>::infinity ();

                  for (vaReIt = validRelays.begin (); vaReIt != validRelays.end (); vaReIt++)
                    {
                      NS_LOG_DEBUG ("  Relay UE ID " << vaReIt->first << " SD-RSRP " << vaReIt->second);

                      if (maxSDRsrp < vaReIt->second)
                        {
                          maxSDRsrpRelayId = vaReIt->first;
                          maxSDRsrp = vaReIt->second;
                        }
                    }
                  NS_ASSERT_MSG (maxSDRsrpRelayId != 0, "Unable to find a valid Relay UE ID");

                  selectedRelayId = maxSDRsrpRelayId;
                  NS_LOG_DEBUG ("UE IMSI " << m_netDevice->GetRrc ()->GetImsi ()
                                           << " selected Relay UE ID " << selectedRelayId << " for SC " << serviceCode);
                }
              else
                {
                  selectedRelayId = currentRelayId;
                  NS_LOG_DEBUG ("UE IMSI " << m_netDevice->GetRrc ()->GetImsi ()
                                           << " did not select a new Relay UE");
                }
              break;
            }
          case LteSlBasicUeController::RelayUeSelectionAlgorithm::MAX_SDRSRP:

            // This algorithm will return
            // - The Relay UE ID of the valid Relay UE with the strongest SD-RSRP
            {
              NS_LOG_DEBUG ("Selection algorithm: MaxSDRsrp");

              if (validRelays.size () == 0)
                {
                  NS_LOG_DEBUG ("No valid Relay UEs available.");
                  selectedRelayId = 0;
                  break;
                }
              else
                {
                  uint64_t maxSDRsrpRelayId = 0;
                  double maxSDRsrp = -std::numeric_limits<double>::infinity ();
                  for (vaReIt = validRelays.begin (); vaReIt != validRelays.end (); vaReIt++)
                    {
                      NS_LOG_DEBUG ("  Relay UE ID " << vaReIt->first << " SD-RSRP " << vaReIt->second);

                      if (maxSDRsrp < vaReIt->second)
                        {
                          maxSDRsrpRelayId = vaReIt->first;
                          maxSDRsrp = vaReIt->second;
                        }
                    }

                  NS_ASSERT_MSG (maxSDRsrpRelayId != 0, "Unable to find a valid Relay UE ID");

                  selectedRelayId = maxSDRsrpRelayId;
                  NS_LOG_DEBUG ("UE IMSI " << m_netDevice->GetRrc ()->GetImsi ()
                                           << " selected Relay UE ID " << selectedRelayId << " for SC " << serviceCode);
                  break;
                }
            }
          default:
            NS_FATAL_ERROR (" Relay UE (re)selection algorithm not recognized ");
        }

      if (selectedRelayId != currentRelayId)
        {
          if (selectedRelayId != 0)
            {
              // This implies a new Relay UE was selected (the first time, or for reselection)
              // and a one-to-one communication connection procedure will start with
              // the selectedRelayId in the LteSlUeRrc
              m_connectingRelayUeId = selectedRelayId;
              NS_LOG_DEBUG ("Selected Relay UE for SC " << serviceCode << " changed");
            }
          //Trace only when a change is actually made
          m_traceRelayUeSelection (m_netDevice->GetRrc ()->GetImsi (), serviceCode, currentRelayId, selectedRelayId );
        }
      else
        {
          NS_LOG_DEBUG ("Selected Relay UE for SC " << serviceCode << " did not change");
        }
    } // we are not in the middle of a connection procedure

  return selectedRelayId;

}

void
LteSlBasicUeController::SetLteSlUeCtrlSapUser (LteSlUeCtrlSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_slUeCtrlSapUser = s;
}

LteSlUeCtrlSapProvider*
LteSlBasicUeController::GetLteSlUeCtrlSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_slUeCtrlSapProvider;
}

} // namespace ns3
