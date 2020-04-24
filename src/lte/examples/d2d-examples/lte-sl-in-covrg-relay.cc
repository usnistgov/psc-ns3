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


#include "ns3/lte-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store.h"
#include <cfloat>
#include <sstream>

using namespace ns3;

/*
 * This scenario illustrates two in-coverage UEs using the UE-to-Network Relay
 * functionality of LTE ProSe.
 *
 * The topology is the following:
 *                                 RemoteHost
 *                                     |
 *                                    EPC
 *                                     |
 *              .-----(UL/DL)---------eNB--------(UL/DL)-------.
 *              |                                              |
 *            UE2 -------------------(SL)-------------------- UE1
 *        (Remote UE)                                      (Relay UE)
 *
 *             |<---------10m--------->|<---------10m--------->|
 *    (-10.0, 0.0, 1.5)         (0.0, 0.0, 30.0)        (10.0, 0.0, 1.5)
 *

 * UE1 starts its relay service at 2.0 s with the 'Relay UE' role.
 * It uses direct discovery over the Sidelink to advertise its presence and its
 * provided service.
 * UE2 starts its relay service at 4.0 s with the 'Remote UE' role.
 * UE2 monitors the discovery messages from the Relay UEs in proximity, selects
 * UE1 as Relay UE (the only one available), and uses the one-to-one direct
 * communication link setup procedure to establish a connection with it.
 * This scenario is using the LteSlBasicUeController (default Sidelink
 * controller). Upon connection establishment, this controller configures a
 * UE-to-Network Relay IPv6 sub-network between the Relay UE and the Remote UE
 * and sets the Relay UE address as the default route for the Remote UE.
 * Thus, all packets from UE2 (Remote UE) will be sent to UE1 (Relay UE)
 * after connection (instead of to the eNodeB).
 * Signaling packets and traffic packets after the connection between UE2
 * (Remote UE) and UE1 (Relay UE) are exchanged over the Sidelink using the direct
 * communication procedures.
 *
 * UE2 (Remote UE) is the only node generating traffic in this scenario. It
 * sends packets towards RemoteHost, which then echoes back them to UE2 (Remote UE).
 *
 * Before starting the relay service, UE2 (Remote UE) packets follow the path
 *   UE2 (Remote UE) --> eNB --> EPC --> RemoteHost --.
 *   UE2 (Remote UE) <-- eNB <-- EPC <----------------'
 * After connecting to UE1 (Relay UE), UE2 (Remote UE) packets follow the path:
 *   UE2 (Remote UE) --> UE1 (Relay UE) --> eNB --> EPC --> RemoteHost --.
 *   UE2 (Remote UE) <-- UE1 (Relay UE) <-- eNB <-- EPC <----------------'
 *
 * The scenario produces 3 output files:
 * - AppPacketTrace.txt: Log of UE2 (Remote UE) and RemoteHost packet
 *                       transmissions and receptions
 * - DlPdcpStats.txt: PDCP layer statistics for the DL
 * - UlPdcpStats.txt: PDCP layer statistics for the UL
 * In 'AppPacketTrace.txt', users can verify the change of Ipv6 address of the
 * packets received by RemoteHost after UE2 (Remote UE) connects to the Relay UE.
 * Before it connects, UE2 (Remote UE) uses the IPv6 address provided by the
 * network. After connection, it uses an address belonging to the UE-to-Network
 * Relay sub-net, which is assigned upon connection establishment.
 * In 'UlPdcpStats.txt' and 'DlPdcpStats.txt' users can verify that UE2 (Remote
 * UE) is no longer using the UL/DL to transmit and receive its packets after
 * it connects to UE1 (Relay UE). Instead, it is UE1 (Relay UE) who uses the
 * UL/DL to transmit and receive the packets that it is relaying.
 *
 * Please refer to the Sidelink section of the LTE user documentation for more
 * details.
 *
 */


/*
 * Trace sink function for logging when a packet is transmitted or received
 * at the application layer
 */
void
UePacketTrace (Ptr<OutputStreamWrapper> stream, const Ipv6Address &localAddrs, std::string context, Ptr<const Packet> p, const Address &srcAddrs, const Address &dstAddrs)
{
  std::ostringstream oss;
  *stream->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << context << "\t" << p->GetSize () << "\t";
  if (Inet6SocketAddress::IsMatchingType (srcAddrs))
    {
      oss << Inet6SocketAddress::ConvertFrom (srcAddrs).GetIpv6 ();
      if (!oss.str ().compare ("::")) //srcAddrs not set
        {
          *stream->GetStream () << localAddrs << ":" << Inet6SocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
        }
      else
        {
          oss.str ("");
          oss << Inet6SocketAddress::ConvertFrom (dstAddrs).GetIpv6 ();
          if (!oss.str ().compare ("::")) //dstAddrs not set
            {
              *stream->GetStream () << Inet6SocketAddress::ConvertFrom (srcAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << localAddrs << ":" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
            }
          else
            {
              *stream->GetStream () << Inet6SocketAddress::ConvertFrom (srcAddrs).GetIpv6 () << ":" << InetSocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
            }
        }
    }
  else
    {
      *stream->GetStream () << "Unknown address type!" << std::endl;
    }
}

NS_LOG_COMPONENT_DEFINE ("LteSlInCovrgRelay");

int main (int argc, char *argv[])
{
  Time simTime = Seconds (10.0);
  bool enableNsLogs = false;

  CommandLine cmd;
  cmd.AddValue ("simTime", "Total duration of the simulation", simTime);
  cmd.AddValue ("enableNsLogs", "Enable ns-3 logging (debug builds)", enableNsLogs);
  cmd.Parse (argc, argv);

  //Configure the UE for UE_SELECTED scenario
  Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
  Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (5)); //The number of RBs allocated per UE for Sidelink
  Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
  Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (false)); //use default Trp index of 0

  //Set the frequency
  Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (18100));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

  //Reduce frequency of CQI report to allow for sidelink transmissions
  Config::SetDefault ("ns3::LteUePhy::DownlinkCqiPeriodicity", TimeValue (MilliSeconds (79)));

  if (enableNsLogs)
    {
      LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_ALL);

      LogComponentEnable ("LteUeRrc", logLevel);
      LogComponentEnable ("LteSlUeRrc", logLevel);
      LogComponentEnable ("LteUeMac", logLevel);
      LogComponentEnable ("LteEnbRrc", logLevel);
      LogComponentEnable ("LteSlEnbRrc", logLevel);
    }

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
  //Set the eNBs power in dBm
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (30.0));

  //Create the lte helper
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  //Create and set the EPC helper
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  //Create Sidelink helper and set lteHelper
  Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
  proseHelper->SetLteHelper (lteHelper);

  //Connect Sidelink controller and Sidelink Helper
  Config::SetDefault ("ns3::LteSlBasicUeController::ProseHelper", PointerValue (proseHelper));

  //Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Hybrid3gppPropagationLossModel"));

  //Enable Sidelink
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);

  //Create nodes (eNb + UEs)
  NodeContainer enbNode;
  enbNode.Create (1);
  NS_LOG_INFO ("eNb node id = [" << enbNode.Get (0)->GetId () << "]");
  NodeContainer ueNodes;
  ueNodes.Create (2);
  NS_LOG_INFO ("UE 1 node id = [" << ueNodes.Get (0)->GetId () << "]");
  NS_LOG_INFO ("UE 2 node id = [" << ueNodes.Get (1)->GetId () << "]");

  //Position of the nodes
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  positionAllocEnb->Add (Vector (0.0, 0.0, 30.0));
  Ptr<ListPositionAllocator> positionAllocUe1 = CreateObject<ListPositionAllocator> ();
  positionAllocUe1->Add (Vector (10.0, 0.0, 1.5));
  Ptr<ListPositionAllocator> positionAllocUe2 = CreateObject<ListPositionAllocator> ();
  positionAllocUe2->Add (Vector (-10.0, 0.0, 1.5));

  //Install mobility
  MobilityHelper mobilityeNodeB;
  mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB.SetPositionAllocator (positionAllocEnb);
  mobilityeNodeB.Install (enbNode);

  MobilityHelper mobilityUe1;
  mobilityUe1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe1.SetPositionAllocator (positionAllocUe1);
  mobilityUe1.Install (ueNodes.Get (0));

  MobilityHelper mobilityUe2;
  mobilityUe2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe2.SetPositionAllocator (positionAllocUe2);
  mobilityUe2.Install (ueNodes.Get (1));

  //Install LTE devices to the nodes
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNode);
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

  //Configure eNodeB for Sidelink
  Ptr<LteSlEnbRrc> enbSidelinkConfiguration = CreateObject<LteSlEnbRrc> ();
  enbSidelinkConfiguration->SetSlEnabled (true);

  //-Configure Sidelink communication pool
  enbSidelinkConfiguration->SetDefaultPool (proseHelper->GetDefaultSlCommTxResourcesSetupUeSelected ());

  //-Enable Sidelink discovery
  enbSidelinkConfiguration->SetDiscEnabled (true);

  //-Configure Sidelink discovery pool
  enbSidelinkConfiguration->AddDiscPool (proseHelper->GetDefaultSlDiscTxResourcesSetupUeSelected ());

  //-Configure UE-to-Network Relay parameters
  enbSidelinkConfiguration->SetDiscConfigRelay (proseHelper->GetDefaultSib19DiscConfigRelay ());

  //Install eNodeB Sidelink configuration on the eNodeB devices
  lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

  //Preconfigure UEs for Sidelink
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);
  //-Set default Sidelink preconfiguration
  LteRrcSap::SlPreconfiguration preconfiguration;
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
  //-Enable Sidelink discovery
  ueSidelinkConfiguration->SetDiscEnabled (true);
  //-Set frequency for Sidelink discovery messages monitoring
  ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());

  //Install UE Sidelink configuration on the UE devices
  lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);

  //Install the IP stack on the UEs and assign network IP addresses
  internet.Install (ueNodes);
  Ipv6InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv6Address (NetDeviceContainer (ueDevs));

  //Set the default gateway for the UEs
  Ipv6StaticRoutingHelper Ipv6RoutingHelper;
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      Ptr<Ipv6StaticRouting> ueStaticRouting = Ipv6RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv6> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress6 (), 1);
    }

  //Configure IP for the nodes in the Internet (PGW and RemoteHost)
  Ipv6AddressHelper ipv6h;
  ipv6h.SetBase (Ipv6Address ("6001:db80::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer internetIpIfaces = ipv6h.Assign (internetDevices);
  internetIpIfaces.SetForwarding (0, true);
  internetIpIfaces.SetDefaultRouteInAllNodes (0);

  //Set route for the Remote Host to join the LTE network nodes
  Ipv6StaticRoutingHelper ipv6RoutingHelper;
  Ptr<Ipv6StaticRouting> remoteHostStaticRouting = ipv6RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv6> ());
  remoteHostStaticRouting->AddNetworkRouteTo ("7777:f000::", Ipv6Prefix (60), internetIpIfaces.GetAddress (0, 1), 1, 0);

  //Configure UE-to-Network Relay network
  proseHelper->SetIpv6BaseForRelayCommunication ("7777:f00e::", Ipv6Prefix (60));

  //Configure route between PGW and UE-to-Network Relay network
  proseHelper->ConfigurePgwToUeToNetworkRelayRoute (pgw);

  //Attach UEs to the eNB
  lteHelper->Attach (ueDevs);

  ///*** Configure applications ***///

  Ipv6Address remoteHostAddr = internetIpIfaces.GetAddress (1, 1);
  uint16_t echoPort = 8000;

  //Set echo server in the remote host
  UdpEchoServerHelper echoServer (echoPort);
  ApplicationContainer serverApps = echoServer.Install (remoteHost);
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  //Set echo client in the remote UEs
  UdpEchoClientHelper echoClient (remoteHostAddr, echoPort);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (20));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.5)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (200));
  ApplicationContainer clientApps = echoClient.Install (ueNodes.Get (1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  //Packets traces
  AsciiTraceHelper ascii;

  Ptr<OutputStreamWrapper> appStream = ascii.CreateFileStream ("AppPacketTrace.txt");
  *appStream->GetStream () << "time(s)\ttx/rx\tNodeID\tIMSI\tPktSize(bytes)\tIP[src]\tIP[dst]" << std::endl;
  std::ostringstream oss;

  Ipv6Address localAddrs =  serverApps.Get (0)->GetNode ()->GetObject<Ipv6L3Protocol> ()->GetAddress (1,1).GetAddress ();

  //Upward: Tx by UE / Rx by Remote Host
  oss << "rx-RH\t" << serverApps.Get (0)->GetNode ()->GetId () << "\t" << "-";
  serverApps.Get (0)->TraceConnect ("RxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, appStream, localAddrs));
  oss.str ("");

  //Downward: Tx by Remote Host / Rx by UE
  oss << "tx-RH\t" << serverApps.Get (0)->GetNode ()->GetId () << "\t" << "-";
  serverApps.Get (0)->TraceConnect ("TxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, appStream, localAddrs));
  oss.str ("");

  localAddrs = Ipv6Address::GetZero ();
  //Downward: Tx by Remote Host / Rx by UE
  oss << "rx-UE\t" << clientApps.Get (0)->GetNode ()->GetId () << "\t" << clientApps.Get (0)->GetNode ()->GetDevice (0)->GetObject<LteUeNetDevice> ()->GetImsi ();
  clientApps.Get (0)->TraceConnect ("RxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, appStream, localAddrs));
  oss.str ("");

  //Upward: Tx by UE / Rx by Remote Host
  oss << "tx-UE\t" << clientApps.Get (0)->GetNode ()->GetId () << "\t" << clientApps.Get (0)->GetNode ()->GetDevice (0)->GetObject<LteUeNetDevice> ()->GetImsi ();
  clientApps.Get (0)->TraceConnect ("TxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, appStream, localAddrs));
  oss.str ("");

  ///*** Configure Relaying ***///

  //Setup dedicated bearer for the Relay UE
  Ptr<EpcTft> tft = Create<EpcTft> ();
  EpcTft::PacketFilter dlpf;
  dlpf.localIpv6Address = proseHelper->GetIpv6NetworkForRelayCommunication ();
  dlpf.localIpv6Prefix = proseHelper->GetIpv6PrefixForRelayCommunication ();
  tft->Add (dlpf);
  EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
  lteHelper->ActivateDedicatedEpsBearer (ueDevs.Get (0), bearer, tft);

  //Schedule the start of the relay service in each UE with their corresponding
  //roles and both interested in the same service code
  uint32_t serviceCode = 33;
  Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartRelayService,
                       proseHelper, ueDevs.Get (0), serviceCode,
                       LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);

  Simulator::Schedule (Seconds (4.0), &LteSidelinkHelper::StartRelayService,
                       proseHelper, ueDevs.Get (1), serviceCode,
                       LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);

  NS_LOG_INFO ("Enabling traces...");

  lteHelper->EnablePdcpTraces ();

  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (simTime);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;

}
