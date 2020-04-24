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
 * The topology is the following:
 *
 *                                   RemoteHost
 *                                       |
 *                                      EPC
 *                                       |
 *       .-------------------------------'----------------------------------.
 *       |                                                                  |
 *     eNB1 ---(UL/DL)--- Relay ---------(SL)--------- Remote ---(UL/DL)--- eNB2
 *                         UE                           UE
 *
 *       |-------10 m------|------------20 m-------------|-------10 m-------|
 *(0.0, 0.0, 30.0) (10.0, 0.0, 1.5)              (30.0, 0.0, 1.5)   (40.0, 0.0, 30.0)
 *
 * The Relay UE is attached to eNB1, and the Remote UE is attached to eNB2.
 *
 * This scenario illustrates two in-coverage UEs that are able to use the
 * UE-to-Network Relay functionality even though they are attached to different
 * eNBs. To enable this, both eNBs are configured with the same Sidelink
 * communication and Sidelink discovery pools.
 *
 * The UEs select autonomously the Sidelink resources to use from the pools indicated
 * by the eNBs, as the pools are configured to be UE_SELECTED (Mode 2) for both
 * discovery and communication.
 * The Remote UE sends packets towards a RemoteHost in the network, which then
 * echoes back them to the Remote UE.
 * Before starting the relay service, the Remote UE packets follow the path
 *   Remote UE --> eNB2 --> EPC --> RemoteHost--.
 *   Remote UE <-- eNB2 <-- EPC <---------------'
 * After connecting to the Relay UE, the Remote UE packets follow the path:
 *   Remote UE --> Relay UE --> eNB1 --> EPC --> RemoteHost--.
 *   Remote UE <-- Relay UE <-- eNB1 <-- EPC <---------------'
 *
 * The scenario produces 3 output files:
 * - AppPacketTrace.txt: Log of Remote UE received packets
 * - DlPdcpStats.txt: PDCP layer statistics for the DL
 * - UlPdcpStats.txt: PDCP layer statistics for the UL
 * In 'UlPdcpStats.txt' and 'DlPdcpStats.txt' users can verify that the
 * Remote UE is no longer using the UL/DL with eNB2 to transmit and receive its
 * packets after it connects to the Relay UE. Instead, it is the Relay UE who
 * uses the UL/DL with eNB1 to transmit and receive the packets that it is
 * relaying for the Remote UE after UE-to-Network Relay connection.
 */

/*
 * Trace sink function for logging when a packet is received
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
          *stream->GetStream () << Ipv6Address::ConvertFrom (localAddrs) << ":" << Inet6SocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
        }
      else
        {
          oss.str ("");
          oss << Inet6SocketAddress::ConvertFrom (dstAddrs).GetIpv6 ();
          if (!oss.str ().compare ("::")) //dstAddrs not set
            {
              *stream->GetStream () << Inet6SocketAddress::ConvertFrom (srcAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << Ipv6Address::ConvertFrom (localAddrs) << ":" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
            }
          else
            {
              *stream->GetStream () << Inet6SocketAddress::ConvertFrom (srcAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
            }
        }
    }
  else
    {
      *stream->GetStream () << "Unknown address type!" << std::endl;
    }
}

/**
 * Structure to store the number of received messages
 */
struct NumberOfRxMessagesTracer
{
  void Trace (Ptr<const Packet> p, const Address &srcAddrs, const Address &dstAddrs); ///< Trace sink function
  Ptr<Node> m_node; ///< Node
  Ipv6Address m_network; ///< UE-to-Network Relay IPv6 network address
  Ipv6Prefix m_prefix; ///< UE-to-Network Relay IPv6 prefix
  uint32_t  m_nRxMsgs {0}; ///< Number of messages received
  uint32_t  m_nRxMsgsRelay {0}; ///< Number of messages received while connected to Relay UE
};

/*
 * Trace sink function to count the number of received packets at the
 * application layer. Two metrics are tracked: the total number of received
 * packets, and how many of them were received while connected to the Relay UE
 */
void
NumberOfRxMessagesTracer::Trace (Ptr<const Packet> p, const Address &srcAddrs, const Address &dstAddrs)
{
  m_nRxMsgs++;

  Ptr<Ipv6> ipv6 = m_node->GetObject<Ipv6> ();
  //Get the interface used for UE-to-Network Relay (LteSlUeNetDevices)
  int32_t ipInterfaceIndex = ipv6->GetInterfaceForPrefix (m_network, m_prefix);
  if (ipInterfaceIndex != -1)
    {
      //We found an address within the UE-to-Network Relay network,
      //i.e., the Remote UE is connected to the Relay UE
      m_nRxMsgsRelay++;
    }
}

NS_LOG_COMPONENT_DEFINE ("LteSlInCovrgRelayMode2Multicell");

int main (int argc, char *argv[])
{
  Time simTime = Seconds (10.0);
  bool enableNsLogs = false;
  bool test = false;

  CommandLine cmd;
  cmd.AddValue ("simTime", "Total duration of the simulation", simTime);
  cmd.AddValue ("enableNsLogs", "Enable ns-3 logging (debug builds)", enableNsLogs);
  cmd.AddValue ("test", "Test if the scenario behaves as expected", test);
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

  // Reduce frequency of CQI report to allow for sidelink transmissions
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

  //Create the helpers
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

  //Create nodes (eNbs + UEs)
  NodeContainer enbNodes;
  enbNodes.Create (2);
  NS_LOG_INFO ("eNb1 node id = [" << enbNodes.Get (0)->GetId () << "]");
  NS_LOG_INFO ("eNb2 node id = [" << enbNodes.Get (1)->GetId () << "]");

  NodeContainer ueNodes;
  ueNodes.Create (2);
  NS_LOG_INFO ("UE 1 node id = [" << ueNodes.Get (0)->GetId () << "]");
  NS_LOG_INFO ("UE 2 node id = [" << ueNodes.Get (1)->GetId () << "]");

  //Position of the eNodeB nodes
  Ptr<ListPositionAllocator> positionAllocEnb1 = CreateObject<ListPositionAllocator> ();
  positionAllocEnb1->Add (Vector (0.0, 0.0, 30.0));
  Ptr<ListPositionAllocator> positionAllocEnb2 = CreateObject<ListPositionAllocator> ();
  positionAllocEnb2->Add (Vector (40.0, 0.0, 30.0));
  //Position of the UE nodes
  Ptr<ListPositionAllocator> positionAllocUe1 = CreateObject<ListPositionAllocator> ();
  positionAllocUe1->Add (Vector (10.0, 0.0, 1.5));
  Ptr<ListPositionAllocator> positionAllocUe2 = CreateObject<ListPositionAllocator> ();
  positionAllocUe2->Add (Vector (30.0, 0.0, 1.5));

  //Install mobility
  MobilityHelper mobilityeNodeB1;
  mobilityeNodeB1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB1.SetPositionAllocator (positionAllocEnb1);
  mobilityeNodeB1.Install (enbNodes.Get (0));

  MobilityHelper mobilityeNodeB2;
  mobilityeNodeB2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB2.SetPositionAllocator (positionAllocEnb2);
  mobilityeNodeB2.Install (enbNodes.Get (1));

  MobilityHelper mobilityUe1;
  mobilityUe1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe1.SetPositionAllocator (positionAllocUe1);
  mobilityUe1.Install (ueNodes.Get (0));

  MobilityHelper mobilityUe2;
  mobilityUe2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe2.SetPositionAllocator (positionAllocUe2);
  mobilityUe2.Install (ueNodes.Get (1));

  //Install LTE devices to the nodes
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNodes);
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

  //Attach each UE to the closest eNB
  lteHelper->Attach (ueDevs.Get (0), enbDevs.Get (0));
  lteHelper->Attach (ueDevs.Get (1), enbDevs.Get (1));

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

  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("AppPacketTrace.txt");
  *stream->GetStream () << "time(sec)\ttx/rx\tNodeID\tIMSI\tPktSize(bytes)\tIP[src]\tIP[dst]" << std::endl;
  std::ostringstream oss;

  //Tracing packets on the UdpEchoClient (C)
  //Tracing only the packets that are Rx back.
  Ipv6Address localAddrs = Ipv6Address::GetZero ();
  oss << "rx\t" << ueNodes.Get (1)->GetId () << "\t" << ueNodes.Get (1)->GetDevice (0)->GetObject<LteUeNetDevice> ()->GetImsi ();
  clientApps.Get (0)->TraceConnect ("RxWithAddresses", oss.str (), MakeBoundCallback (&UePacketTrace, stream, localAddrs));
  oss.str ("");

  //Set Rx trace for test
  NumberOfRxMessagesTracer tracer;
  tracer.m_node = clientApps.Get (0)->GetNode ();
  tracer.m_network = proseHelper->GetIpv6NetworkForRelayCommunication ();
  tracer.m_prefix = proseHelper->GetIpv6PrefixForRelayCommunication ();
  clientApps.Get (0)->TraceConnectWithoutContext ("RxWithAddresses",  MakeCallback (&NumberOfRxMessagesTracer::Trace, &tracer));

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

  if (test)
    {
      NS_LOG_INFO ("Verifying scenario...");

      //In this scenario, with simTime = Seconds (10), the Remote UE should have
      //successfully received back at least 15 messages and at least 8 of them should
      //have been received after the connection to the Relay UE (reflected in the change of IP)
      NS_LOG_INFO ("Remote UE received back " << tracer.m_nRxMsgs << " messages");
      NS_LOG_INFO ("Remote UE received back " << tracer.m_nRxMsgsRelay << " messages while connected to the Relay UE");

      if (tracer.m_nRxMsgs >= 15 && tracer.m_nRxMsgsRelay >= 8)
        {
          NS_LOG_INFO ("Expected behavior");
        }
      else
        {
          NS_FATAL_ERROR ("Unexpected behavior!");
        }
    }


  Simulator::Destroy ();
  return 0;

}
