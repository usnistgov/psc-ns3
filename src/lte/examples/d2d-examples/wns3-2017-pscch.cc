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
#include <ns3/buildings-module.h>
#include <cfloat>
#include <sstream>



using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("wns3-2017-pscch");

void
PacketSrcDstAddrsTrace (Ptr<OutputStreamWrapper> stream, const Ipv4Address &localAddrs, std::string contex, Ptr<const Packet> p, const Address &srcAddrs, const Address &dstAddrs)
{
  std::ostringstream oss;

  *stream->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t" << contex << "\t" << p->GetSize () << "\t";
  if (InetSocketAddress::IsMatchingType (srcAddrs))
    {
      oss << InetSocketAddress::ConvertFrom (srcAddrs).GetIpv4 ();
      if (!oss.str ().compare ("0.0.0.0")) //srcAddrs not set
        {
          *stream->GetStream () << localAddrs << ":" << InetSocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << InetSocketAddress::ConvertFrom (dstAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
        }
      else
        {
          oss.str ("");
          oss << InetSocketAddress::ConvertFrom (dstAddrs).GetIpv4 ();
          if (!oss.str ().compare ("0.0.0.0")) //dstAddrs not set
            {
              *stream->GetStream () << InetSocketAddress::ConvertFrom (srcAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << localAddrs << ":" << InetSocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
            }
          else
            {
              *stream->GetStream () << InetSocketAddress::ConvertFrom (srcAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << InetSocketAddress::ConvertFrom (dstAddrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
            }
        }
    }
  else if (Inet6SocketAddress::IsMatchingType (srcAddrs))
    {
      *stream->GetStream () << Inet6SocketAddress::ConvertFrom (srcAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (srcAddrs).GetPort () << "\t" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (dstAddrs).GetPort () << std::endl;
    }
  else
    {
      *stream->GetStream () << "Unknown address type!" << std::endl;
    }
}

void
PrintStatus (uint32_t sPeriod)
{
  std::cout << Simulator::Now ().GetSeconds () << std::endl;
  Simulator::Schedule (Seconds (sPeriod),&PrintStatus, sPeriod);
}


int
main (int argc, char *argv[])
{
  // Initialize some values
  uint32_t simTime = 6;            // Simulation time in seconds
  double ueTxPower = 23.0;         // Transmission power in dBm
  uint32_t ueRespondersPerSector = 1; // Responder UEs per sectors
  uint32_t respondersStart = 1;    // Responders' applications start time
  bool verbose = false;            // Print time progress
  bool onoff = false;              // Use on-off applications
  double responderPktIntvl = 0.020; // Responders' application packet interval in seconds
  uint32_t responderMaxPack = 10;  // Responders' maximum number of packets
  uint32_t responderPktSize = 7;  // Number of payload bytes in packets
  uint32_t numRings = 1;            // Number of rings in hexagon cell topology
  double isd = 10;                 // Inter Site Distance
  double minCenterDist = 1;        // Minimum deploy distance to center of cell site
  uint32_t numGroups = 3;           // Number of D2D groups
  uint32_t numReceivers = 0;        // Number of receivers per group. If zero, then group size is not limited.
  uint32_t mcs = 10;               // Modulation and Coding Scheme
  uint32_t rbSize = 2;             // PSSCH subchannel allocation size in RBs
  uint32_t ktrp = 1;               // Transmissions opportunities in a Time Repetition Pattern(number of subframes to transmit on)
  uint32_t pscchRbs = 22;         // PSCCH pool size in RBs. Note, the PSCCH occupied bandwidth will be at least twice this amount.
  std::string pscchBitmapHexstring = "0x00000000FF"; // PSCCH time bitmap [40 bits]
  uint32_t slPeriod = 40;         // Length of Sidelink period in milliseconds
  bool ctrlErrorModelEnabled = true; // Enable error model in the PSCCH
  bool dropOnCollisionEnabled = false; // Drop PSCCH PSSCH messages on conflicting scheduled resources
  bool  enableNsLogs = false; // If enabled will output NS LOGs

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue ("time", "Simulation time", simTime);
  cmd.AddValue ("responders", "Number of Responders per sector", ueRespondersPerSector);
  cmd.AddValue ("verbose", "Print time progress", verbose);
  cmd.AddValue ("ring", "Number of rings in hexagon cell topology", numRings);
  cmd.AddValue ("isd", "Inter Site Distance", isd);
  cmd.AddValue ("minDist", "Minimum Center Distance for UEs", minCenterDist);
  cmd.AddValue ("groups", "Number of groups", numGroups);
  cmd.AddValue ("receivers", "Number of receivers per group", numReceivers);
  cmd.AddValue ("respPktSize", "Packets size in bytes", responderPktSize);
  cmd.AddValue ("respMaxPkt", "maximum number of packets per UE", responderMaxPack);
  cmd.AddValue ("respPktIntvl", "interval between packets", responderPktIntvl);
  cmd.AddValue ("mcs", "MCS", mcs);
  cmd.AddValue ("rbSize", "PSSCH allocation size", rbSize);
  cmd.AddValue ("ktrp", "Repetition", ktrp);
  cmd.AddValue ("pscchRbs", "PSCCH RBs", pscchRbs);
  cmd.AddValue ("pscchTrp", "PSCCH time bitmap", pscchBitmapHexstring);
  cmd.AddValue ("slPeriod", "Length of SL period", slPeriod );
  cmd.AddValue ("ctrlErrorModel", "Enables PSCCH error model", ctrlErrorModelEnabled);
  cmd.AddValue ("dropOnCol", "Drop PSCCH and PSSCH messages on collisions", dropOnCollisionEnabled);
  cmd.AddValue ("enableNsLogs", "Enable NS logs", enableNsLogs);
  cmd.Parse (argc, argv);

  if (enableNsLogs)
    {
      LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_ALL);
      LogComponentEnable ("wns3-2017-pscch", logLevel);

      LogComponentEnable ("LteSpectrumPhy", logLevel);
      LogComponentEnable ("LteUePhy", logLevel);
      LogComponentEnable ("LteUeRrc", logLevel);
      LogComponentEnable ("LteEnbPhy", logLevel);
      LogComponentEnable ("LteUeMac", logLevel);
      LogComponentEnable ("LteSlUeRrc", logLevel);
      LogComponentEnable ("LteSidelinkHelper", logLevel);
      LogComponentEnable ("LteHelper", logLevel);
    }


  NS_LOG_INFO ("Starting network configuration...");

  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (100000));


  // Set the frequency to use for the Public Safety case (band 14 : 788 - 798 MHz for Uplink)
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (23330));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

  // Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (ueTxPower));
  Config::SetDefault ("ns3::LteUePhy::RsrpUeMeasThreshold", DoubleValue (-10.0));

  // Set power control
  Config::SetDefault ("ns3::LteUePowerControl::Pcmax", DoubleValue (ueTxPower));
  Config::SetDefault ("ns3::LteUePowerControl::PoNominalPusch", IntegerValue (-106));
  Config::SetDefault ("ns3::LteUePowerControl::PsschTxPower", DoubleValue (ueTxPower));
  Config::SetDefault ("ns3::LteUePowerControl::PscchTxPower", DoubleValue (ueTxPower));

  // Configure error model
  Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled", BooleanValue (ctrlErrorModelEnabled));
  Config::SetDefault ("ns3::LteSpectrumPhy::CtrlFullDuplexEnabled", BooleanValue (!ctrlErrorModelEnabled));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (dropOnCollisionEnabled));

  std::cout << "CtrlErrorModel: " << ((ctrlErrorModelEnabled) ? "Enabled" : "Disabled") << std::endl;
  std::cout << "DropOnCollision: " << ((dropOnCollisionEnabled) ? "Enabled" : "Disabled") << std::endl;

  // Configure for UE selected
  Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (rbSize));
  Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (mcs));
  Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (ktrp));

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  NS_LOG_INFO ("Creating helpers...");
  // EPC
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  Ptr<Node> pgw = epcHelper->GetPgwNode ();
  std::cout << "PGW id=" << pgw->GetId () << std::endl;

  // LTE Helper
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  lteHelper->DisableEnbPhy (true); //Disable eNBs for out-of-coverage modeling

  // ProSe
  Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
  proseHelper->SetLteHelper (lteHelper);

  // Topology (Hex Grid)
  Ptr<Lte3gppHexGridEnbTopologyHelper> topoHelper = CreateObject<Lte3gppHexGridEnbTopologyHelper> ();
  topoHelper->SetLteHelper (lteHelper);
  topoHelper->SetNumRings (numRings);
  topoHelper->SetInterSiteDistance (isd);
  topoHelper->SetMinimumDistance (minCenterDist);
  topoHelper->AssignStreams (1);

  // Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Cost231PropagationLossModel"));

  // Create eNbs
  NodeContainer sectorNodes;
  sectorNodes.Create (topoHelper->GetNumNodes ());

  std::cout << "eNb IDs=[";
  for (NodeContainer::Iterator it = sectorNodes.Begin (); it != sectorNodes.End (); it++)
    {
      if (it + 1 != sectorNodes.End ())
        {
          std::cout << (*it)->GetId () << ",";
        }
      else
        {
          std::cout << (*it)->GetId () << "]" << std::endl;
        }
    }

  // Install mobility (eNB)
  MobilityHelper mobilityeNodeB;
  mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB.Install (sectorNodes);

  // Needed to set the orientation of eNB antenna to from sectors
  lteHelper->SetEnbAntennaModelType ("ns3::Parabolic3dAntennaModel");

  // Compute the position of each site and antenna orientation
  NetDeviceContainer enbDevs = topoHelper->SetPositionAndInstallEnbDevice (sectorNodes);


  // Create node container to hold all UEs
  NodeContainer ueAllNodes;

  // Responder users
  NodeContainer ueResponders;
  if (ueRespondersPerSector > 0)
    {
      // Create nodes (responders)
      ueResponders.Create (ueRespondersPerSector * sectorNodes.GetN ());
      ueAllNodes.Add (ueResponders);

      std::cout << "Responders IDs=[";
      for (NodeContainer::Iterator it = ueResponders.Begin (); it != ueResponders.End (); it++)
        {
          if (it + 1 != ueResponders.End ())
            {
              std::cout << (*it)->GetId () << ",";
            }
          else
            {
              std::cout << (*it)->GetId () << "]" << std::endl;
            }
        }

      // Install mobility (responders)
      MobilityHelper mobilityResponders;
      mobilityResponders.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
      //Do not set position here, the topology helper takes care of deploying the nodes when called ahead!
      mobilityResponders.Install (ueResponders);
    }


  // Install LTE devices to all UEs and deploy them in the sectors.
  NS_LOG_INFO ("Installing UE's network devices and Deploying...");
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));
  NetDeviceContainer ueRespondersDevs = topoHelper->DropUEsUniformlyPerSector (ueResponders);
  NetDeviceContainer ueDevs;
  ueDevs.Add (ueRespondersDevs);

  //Fix the random number stream
  int64_t randomStream = 1;
  randomStream += lteHelper->AssignStreams (ueDevs, randomStream);

  // Save nodes' positions
  std::ofstream outFile;
  if (ueAllNodes.GetN () != 0)
    {
      outFile.open ("nPositions.txt", std::ofstream::out);

      outFile << "Node\tID\tX\tY " << std::endl;
      for (uint32_t i = 0; i < sectorNodes.GetN (); ++i)
        {
          outFile << "eNB\t" << sectorNodes.Get (i)->GetId () << "\t" << sectorNodes.Get (i)->GetObject<MobilityModel> ()->GetPosition ().x << "\t" << sectorNodes.Get (i)->GetObject<MobilityModel> ()->GetPosition ().y << std::endl;
        }
      Ptr<Object> ueNetDevice;
      for (uint32_t i = 0; i < ueAllNodes.GetN (); ++i)
        {
          ueNetDevice = ueAllNodes.Get (i)->GetDevice (0);

          outFile << " UE\t" << ueNetDevice->GetObject<LteUeNetDevice> ()->GetImsi () << "\t" << ueAllNodes.Get (i)->GetObject<MobilityModel> ()->GetPosition ().x << "\t" << ueAllNodes.Get (i)->GetObject<MobilityModel> ()->GetPosition ().y << std::endl;
        }
      outFile.close ();
    }

  // Install the IP stack on the UEs
  NS_LOG_INFO ("Installing IP stack...");
  InternetStackHelper internet;
  internet.Install (ueAllNodes);

  // Assign IP address to UEs
  NS_LOG_INFO ("Allocating IP addresses and setting up network route...");
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (ueDevs);
  Ipv4StaticRoutingHelper ipv4RoutingHelper;

  for (uint32_t u = 0; u < ueAllNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueAllNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  NS_LOG_INFO ("Attaching UE's to LTE network...");
  // Attach each UE to the best available eNB
  lteHelper->Attach (ueDevs);


  NS_LOG_INFO ("Creating Sidelink groups...");
  // Create groups
  double ulEarfcn = enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ();
  double ulBandwidth = enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlBandwidth ();
  std::vector < NetDeviceContainer > createdgroups;
  if (numReceivers > 0)
    {
      //groupcast
      createdgroups = proseHelper->AssociateForGroupcast (ueTxPower, ulEarfcn, ulBandwidth, ueRespondersDevs, -112, numGroups, numReceivers, LteSidelinkHelper::SLRSRP_TX_PW);
    }
  else
    {
      //broadcast
      createdgroups = proseHelper->AssociateForBroadcastWithTxEnabledToReceive (ueTxPower, ulEarfcn, ulBandwidth, ueRespondersDevs, -112, numGroups, LteSidelinkHelper::SLRSRP_TX_PW);
    }
  //print groups created
  proseHelper->PrintGroups (createdgroups);

  //compute average number of receivers associated per transmitter and vice versa
  double totalRxs = 0;
  std::map < uint32_t, uint32_t> txPerUeMap;
  std::map < uint32_t, uint32_t> groupsPerUe;

  std::vector < NetDeviceContainer >::iterator gIt;
  for (gIt = createdgroups.begin (); gIt != createdgroups.end (); gIt++)
    {
      if (gIt->GetN () < numReceivers)
        {
          // Unable to satisfy group size requested!
          std::cerr << "Unable to satisfy group size requested for groupcast. Either the UEs are out of reach or there are not enough UEs!" << std::endl;
          return 1;
        }
      totalRxs += gIt->GetN () - 1;
      uint32_t nDevices = gIt->GetN ();
      for (uint32_t i = 1; i < nDevices; ++i)
        {
          uint32_t nId = gIt->Get (i)->GetNode ()->GetId ();
          txPerUeMap[nId]++;
        }
    }

  double totalTxPerUe = 0;
  std::map < uint32_t, uint32_t>::iterator mIt;
  std::cout << "Tx per UE:" << std::endl;
  for (mIt = txPerUeMap.begin (); mIt != txPerUeMap.end (); mIt++)
    {
      totalTxPerUe += mIt->second;
      std::cout << mIt->first << " " << mIt->second << std::endl;
      groupsPerUe [mIt->second]++;
    }
  std::cout << "Average number of receivers per transmitter = " << (totalRxs / numGroups) << std::endl;
  std::cout << "Average number of transmitters per receiver = " << (totalTxPerUe / txPerUeMap.size ()) << std::endl;
  std::cout << "Associated Groups per Rx UE" << std::endl;
  for (mIt = groupsPerUe.begin (); mIt != groupsPerUe.end (); mIt++)
    {
      std::cout << mIt->first << " " << mIt->second << std::endl;
    }

  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("prose_connections.txt");
  proseHelper->PrintGroups (createdgroups, stream);

  NS_LOG_INFO ("Installing applications...");
  // Application Setup for Responders
  // Set Exponential Random Variables: ON(75% with mean 2.5s), OFF(25%)
  double onTimeMean = 2.5;
  double offTimeMean = onTimeMean / 3;
  Ptr<ExponentialRandomVariable> onRv = CreateObject<ExponentialRandomVariable> ();
  onRv->SetAttribute ("Mean", DoubleValue (onTimeMean));
  Ptr<ExponentialRandomVariable> offRv = CreateObject<ExponentialRandomVariable> ();
  offRv->SetAttribute ("Mean", DoubleValue (offTimeMean));

  // VoIP model defined in 36.843 Table A.2.1.3-1
  double encoderFrameLength = responderPktIntvl; //0.020 s for VoIP
  double pktRate = 1 / encoderFrameLength; //100
  uint16_t pktSize = responderPktSize;
  double dataRateBitsPerSec = pktSize * 8 * pktRate;


  stream = ascii.CreateFileStream ("ue_pck.tr");
  //Trace file table header
  *stream->GetStream () << "time\ttx/rx\tNID\tIMSI\tUEtype\tsize\tIP[src]\tIP[dst]" << std::endl;

  std::vector<uint32_t> groupL2Addresses;
  uint32_t groupL2Address = 200;
  Ipv4AddressGenerator::Init (Ipv4Address ("225.0.0.0"), Ipv4Mask ("255.0.0.0"));
  Ipv4Address clientRespondersAddress = Ipv4AddressGenerator::NextAddress (Ipv4Mask ("255.0.0.0"));

  uint16_t applicationPort = 8000; //Application port to TX/RX
  ApplicationContainer clientRespondersApps;
  NetDeviceContainer activeTxUes;

  for (gIt = createdgroups.begin (); gIt != createdgroups.end (); gIt++)
    {
      //Create Sidelink bearers
      //Use Tx for the group transmitter and Rx for all the receivers
      //split Tx/Rx

      NetDeviceContainer txUe ((*gIt).Get (0));
      activeTxUes.Add (txUe);
      NetDeviceContainer rxUes = proseHelper->RemoveNetDevice ((*gIt), txUe.Get (0));
      Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::TRANSMIT, clientRespondersAddress, groupL2Address);
      proseHelper->ActivateSidelinkBearer (Seconds (1.0), txUe, tft);
      tft = Create<LteSlTft> (LteSlTft::RECEIVE, clientRespondersAddress, groupL2Address);
      proseHelper->ActivateSidelinkBearer (Seconds (1.0), rxUes, tft);

      std::cout << "Created group L2Address=" << groupL2Address << " IPAddress=";
      clientRespondersAddress.Print (std::cout);
      std::cout << std::endl;

      // Install Application in the Responders' UEs
      if (onoff)
        {
          std::cout << "Responder's OnOff App. bitrate " << dataRateBitsPerSec << std::endl;
          OnOffHelper clientOnOffHelper ("ns3::UdpSocketFactory",
                                         Address ( InetSocketAddress (clientRespondersAddress, applicationPort)));
          clientOnOffHelper.SetAttribute ("PacketSize", UintegerValue (pktSize));
          clientOnOffHelper.SetAttribute ("DataRate", DataRateValue (dataRateBitsPerSec));
          clientOnOffHelper.SetAttribute ("OnTime", PointerValue (onRv));
          clientOnOffHelper.SetAttribute ("OffTime", PointerValue (offRv));

          clientRespondersApps.Add (clientOnOffHelper.Install (txUe.Get (0)->GetNode ()));
        }
      else
        {
          // UDP application
          UdpEchoClientHelper echoClientHelper (clientRespondersAddress, applicationPort);
          echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (responderMaxPack));
          echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (responderPktIntvl)));
          echoClientHelper.SetAttribute ("PacketSize", UintegerValue (responderPktSize));

          // Install application to group transmitter
          clientRespondersApps.Add (echoClientHelper.Install (txUe.Get (0)->GetNode ()));
        }

      //store and increment addresses
      groupL2Addresses.push_back (groupL2Address);
      groupL2Address++;
      clientRespondersAddress = Ipv4AddressGenerator::NextAddress (Ipv4Mask ("255.0.0.0"));
    } // end for each group in created groups

  clientRespondersApps.Start (Seconds (respondersStart));
  clientRespondersApps.Stop (Seconds (simTime));

  // Application to receive traffic
  PacketSinkHelper clientPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), applicationPort));
  ApplicationContainer clientRespondersSrvApps = clientPacketSinkHelper.Install (ueResponders);
  clientRespondersSrvApps.Start (Seconds (respondersStart));
  clientRespondersSrvApps.Stop (Seconds (simTime + 1));


  NS_LOG_INFO ("Setting application traces...");
  std::ostringstream oss;
  // Set responders Tx traces
  for (uint16_t ac = 0; ac < clientRespondersApps.GetN (); ac++)
    {
      oss << "t\t" << activeTxUes.Get (ac)->GetNode ()->GetId () << "\t" << activeTxUes.Get (ac)->GetObject<LteUeNetDevice> ()->GetImsi () << "\tresp";
      Ipv4Address localAddrs = activeTxUes.Get (ac)->GetNode ()->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
      std::cout << "Tx address: " << localAddrs << std::endl;
      clientRespondersApps.Get (ac)->TraceConnect ("TxWithAddresses", oss.str (), MakeBoundCallback (&PacketSrcDstAddrsTrace, stream, localAddrs));
      oss.str ("");
    }

  // Set responders Rx traces
  for (uint16_t ac = 0; ac < clientRespondersSrvApps.GetN (); ac++)
    {
      Ipv4Address localAddrs =  ueRespondersDevs.Get (ac)->GetNode ()->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
      std::cout << "Rx address: " << localAddrs << std::endl;
      oss << "r\t" << ueResponders.Get (ac)->GetId () << "\t" << ueResponders.Get (ac)->GetDevice (0)->GetObject<LteUeNetDevice> ()->GetImsi () << "\tresp";
      clientRespondersSrvApps.Get (ac)->TraceConnect ("RxWithAddresses", oss.str (), MakeBoundCallback (&PacketSrcDstAddrsTrace, stream, localAddrs));
      oss.str ("");
    }


  NS_LOG_INFO ("Creating Sidelink configuration...");
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);

  LteRrcSap::SlPreconfiguration preconfiguration;
  preconfiguration.preconfigGeneral.carrierFreq = 23330;
  preconfiguration.preconfigGeneral.slBandwidth = 50;
  preconfiguration.preconfigComm.nbPools = 1;

  //Convert pscchTrp hex string representation to decimal.
  std::stringstream ssHex;
  ssHex << std::hex << pscchBitmapHexstring;
  uint64_t pscchTrpNumber;
  ssHex >> pscchTrpNumber;
  std::cout << "PSCCH RBS: " << pscchRbs << ", PSCCH hexstring: " << pscchBitmapHexstring << std::endl;
  std::cout << "Decimal representation of pscchTrpNumber : " << pscchTrpNumber << std::endl;
  uint32_t pscchLength = std::ceil (std::log2l (pscchTrpNumber));
  std::cout << "Data offset : " << pscchLength << std::endl;

  LteSlPreconfigPoolFactory pfactory;
  pfactory.SetControlBitmap (pscchTrpNumber);
  std::stringstream pstream;
  pstream << "sf" << slPeriod;
  pfactory.SetControlPeriod (pstream.str ());
  pfactory.SetControlPrbNum (pscchRbs);
  pfactory.SetDataOffset (pscchLength);

  preconfiguration.preconfigComm.pools[0] = pfactory.CreatePool ();
  preconfiguration.preconfigComm.nbPools = 1;
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);

  NS_LOG_INFO ("Installing Sidelink configuration...");
  lteHelper->InstallSidelinkConfiguration (ueRespondersDevs, ueSidelinkConfiguration);

  NS_LOG_INFO ("Enabling Sidelink traces...");

  lteHelper->EnableSlPscchMacTraces ();
  lteHelper->EnableSlPsschMacTraces ();

  lteHelper->EnableSlRxPhyTraces ();
  lteHelper->EnableSlPscchRxPhyTraces ();

  // Display simulator time progress
  if (verbose)
    {
      uint32_t statusPeriod = 1;
      Simulator::Schedule (Seconds (1),&PrintStatus, statusPeriod);
    }

  NS_LOG_INFO ("Starting simulation...");
  Simulator::Stop (MilliSeconds (simTime * 1000 + 40));
  Simulator::Run ();

  Simulator::Destroy ();

  NS_LOG_INFO ("Done.");
  return 0;

}

