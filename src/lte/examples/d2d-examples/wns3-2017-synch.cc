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

void
PacketSrcDstAddrsTrace (Ptr<OutputStreamWrapper> stream, const Ipv4Address &localAddrs, std::string contex, Ptr<const Packet> p, const Address &src_addrs, const Address &dst_addrs)
{
  std::ostringstream oss;
  *stream->GetStream () << Simulator::Now ().GetMilliSeconds () << "\t" << contex << "\t" << p->GetSize () << "\t";

  if (InetSocketAddress::IsMatchingType (src_addrs)) //assumes dst_addrs is same type!
    {
      oss << InetSocketAddress::ConvertFrom (src_addrs).GetIpv4 ();
      if (!oss.str ().compare ("0.0.0.0")) //src_addrs not set
        {
          *stream->GetStream () << localAddrs << ":" << InetSocketAddress::ConvertFrom (src_addrs).GetPort () << "\t" << InetSocketAddress::ConvertFrom (dst_addrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (dst_addrs).GetPort () << std::endl;
        }
      else
        {
          oss.str ("");
          oss << InetSocketAddress::ConvertFrom (dst_addrs).GetIpv4 ();
          if (!oss.str ().compare ("0.0.0.0")) //dst_addrs not set
            {
              *stream->GetStream () << InetSocketAddress::ConvertFrom (src_addrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (src_addrs).GetPort () << "\t" << localAddrs << ":" << InetSocketAddress::ConvertFrom (dst_addrs).GetPort () << std::endl;
            }
          else
            {
              *stream->GetStream () << InetSocketAddress::ConvertFrom (src_addrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (src_addrs).GetPort () << "\t" << InetSocketAddress::ConvertFrom (dst_addrs).GetIpv4 () << ":" << InetSocketAddress::ConvertFrom (dst_addrs).GetPort () << std::endl;
            }
        }
    }
  else if (Inet6SocketAddress::IsMatchingType (src_addrs)) //assumes dst_addrs is same type!
    {
      *stream->GetStream () << Inet6SocketAddress::ConvertFrom (src_addrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (src_addrs).GetPort () << "\t" << Inet6SocketAddress::ConvertFrom (dst_addrs).GetIpv6 () << ":" << Inet6SocketAddress::ConvertFrom (dst_addrs).GetPort () << std::endl;
    }
  else
    {
      *stream->GetStream () << "Unknown address type!" << std::endl;
    }
}

/*Synchronization traces*/
void
NotifyChangeOfSyncRef (Ptr<OutputStreamWrapper> stream, LteUeRrc::SlChangeOfSyncRefStatParameters param)
{
  *stream->GetStream () << Simulator::Now ().GetMilliSeconds () << "\t" << param.imsi << "\t" << param.prevSlssid << "\t" << param.prevRxOffset << "\t" << param.prevFrameNo << "\t" << param.prevSubframeNo <<
    "\t" << param.currSlssid << "\t" << param.currRxOffset << "\t" << param.currFrameNo << "\t" << param.currSubframeNo << std::endl;
}

void
NotifySendOfSlss (Ptr<OutputStreamWrapper> stream, uint64_t imsi, uint64_t slssid, uint16_t txOffset, bool inCoverage, uint16_t frame, uint16_t subframe)
{
  *stream->GetStream () << Simulator::Now ().GetMilliSeconds () << "\t" << imsi << "\t"  << slssid << "\t" << txOffset << "\t" << inCoverage << "\t" << frame <<  "\t" << subframe << std::endl;
}


NS_LOG_COMPONENT_DEFINE ("wns3-2017-synch");
int
main (int argc, char *argv[])
{

  //Set queue values
  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (100000));

  // Set the frequency to use the Public Safety case (band 14 : 700 MHz)
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (23330));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

  // Set the UEs power in dBm
  double ueTxPower = 31.0; // 31dBm for PS, 23.0 for non-PS
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (ueTxPower));
  Config::SetDefault ("ns3::LteUePowerControl::Pcmax", DoubleValue (ueTxPower));
  Config::SetDefault ("ns3::LteUePowerControl::PoNominalPusch", IntegerValue (-106));
  Config::SetDefault ("ns3::LteUePowerControl::PsschTxPower", DoubleValue (ueTxPower));
  Config::SetDefault ("ns3::LteUePowerControl::PscchTxPower", DoubleValue (ueTxPower));

  // Set data error model
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::LteSpectrumPhy::FadingModel", StringValue ("AWGN"));


  // Set the SRS periodicity in ms (increase the capability of handling more UEs per cell)
  // Allowed values : 2, 5, 10, 20, 40, 80, 160 and 320
  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (160));

  //Default parameter values
  double simTime = 50.001;

  uint32_t numRings = 1;
  double isd = 500;

  uint32_t ueRespondersPerSector = 1;
  int numGroups = 1;

  bool onoff = false;
  double respondersStart = 20.0;
  double responderPktIntvl = 0.005;
  uint32_t responderMaxPack = 0xffffffff;
  uint32_t responderPktSize = 40;

  uint32_t mcs = 10;
  uint32_t rbSize = 2;
  uint32_t ktrp = 2;
  uint32_t pscchRbs = 22;
  std::string pscchTrpHexstring = "0x00000000FF";
  uint32_t slPeriod = 40;
  bool ctrlErrorModelEnabled = true;
  bool dropOnCollisionEnabled = false;

  /*Synchronization*/
  int16_t syncTxThreshOoC = -60; //dBm
  uint16_t filterCoefficient = 0;  //k = 4 ==> a = 0.5, k = 0 ==> a = 1 No filter
  uint16_t syncRefMinHyst = 0; //dB
  uint16_t syncRefDiffHyst = 0; //dB
  uint32_t interScanTimeMin = 2000; //ms
  uint32_t interScanTimeMax = 2000; //ms
  uint32_t scanTime = 40; //ms
  uint32_t measTime = 400; //ms
  uint32_t evalTime = 400; //ms
  uint32_t firstScanTimeMin = 2000; //ms
  uint32_t firstScanTimeMax = 4000; //ms
  bool unsyncSl = true;
  bool slSyncActive = true;
  bool  enableNsLogs = false; // If enabled will output NS LOGs
  /*END Synchronization*/

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue ("simTime", "Simulation time", simTime);
  cmd.AddValue ("responders", "Number of Responders per sector", ueRespondersPerSector);
  cmd.AddValue ("ring", "Number of rings in hexagon", numRings);
  cmd.AddValue ("isd", "Inter Site Distance", isd);
  cmd.AddValue ("groups", "Number of groups", numGroups);
  cmd.AddValue ("respPktSize", "Packets size in bytes", responderPktSize);
  cmd.AddValue ("respMaxPkt", "maximum number of packets per UE", responderMaxPack);
  cmd.AddValue ("respPktIntvl", "interval between packets", responderPktIntvl);
  cmd.AddValue ("mcs", "MCS", mcs);
  cmd.AddValue ("rbSize", "Allocation size", rbSize);
  cmd.AddValue ("ktrp", "Repetition", ktrp);
  cmd.AddValue ("pscchRbs", "PSCCH RBs", pscchRbs);
  cmd.AddValue ("pscchTrp", "PSCCH trp bitmap", pscchTrpHexstring);
  cmd.AddValue ("slPeriod", "Length of SL period", slPeriod );
  cmd.AddValue ("ctrlError", "Enables PSCCH error model", ctrlErrorModelEnabled);
  cmd.AddValue ("dropOnCol", "Drop PSSCH and PSCCH messages on collisions", dropOnCollisionEnabled);
  cmd.AddValue ("onoff", "Type of traffic", onoff);

  /*Synchronization*/
  cmd.AddValue ("syncTxThreshOoC", "SL Sync: Threshold defining the “inner part” of the SyncRef UE cell", syncTxThreshOoC );
  cmd.AddValue ("filterCoefficient", "SL Sync: Weight of the last measurement => k ", filterCoefficient);
  cmd.AddValue ("syncRefMinHyst", "SL Sync: How stronger than the minimum required the signal of a SyncRef UE should be to consider it", syncRefMinHyst);
  cmd.AddValue ("syncRefDiffHyst", "SL Sync: How stronger than the current selected SyncRef UE the signal of another SyncRef UE should be to consider it", syncRefDiffHyst);
  cmd.AddValue ("interScanTimeMin", "SL Sync: Min value of the uniform dist of the inter syncRef-selection-processes time", interScanTimeMin);
  cmd.AddValue ("interScanTimeMax", "SL Sync: Max value of the uniform dist of the inter syncRef-selection-processes time", interScanTimeMax);
  cmd.AddValue ("scanTime", "SL Sync: Length of the SyncRef search period", scanTime);
  cmd.AddValue ("measTime", "SL Sync: Length of the L1 measurement time", measTime);
  cmd.AddValue ("evalTime", "SL Sync: Length of the SyncRef evaluation period for cease/initiation of SLSS transmission", evalTime);
  cmd.AddValue ("firstScanTimeMin", "SL Sync: Min value of the uniform dist of the initial measurement", firstScanTimeMin);
  cmd.AddValue ("firstScanTimeMax", "SL Sync: Max value of the uniform dist of the initial measurement", firstScanTimeMax);
  cmd.AddValue ("unsyncSl", "SL Sync: unsynchronized scenario (random frame/subframe indication, and random SLSSID", unsyncSl);
  cmd.AddValue ("slSyncActive", "SL Sync: activate the SL synchronization protocol", slSyncActive);
  cmd.AddValue ("enableNsLogs", "Enable NS logs", enableNsLogs);
  /*END Synchronization*/

  cmd.Parse (argc, argv);

  if (enableNsLogs)
    {
      LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_ALL);
      LogComponentEnable ("wns3-2017-synch", logLevel);

      LogComponentEnable ("LteSpectrumPhy", logLevel);
      LogComponentEnable ("LteUePhy", logLevel);
      LogComponentEnable ("LteUeRrc", logLevel);
      LogComponentEnable ("LteEnbPhy", logLevel);
      LogComponentEnable ("LteUeMac", logLevel);
      LogComponentEnable ("LteSlUeRrc", logLevel);
      LogComponentEnable ("LteSidelinkHelper", logLevel);
      LogComponentEnable ("LteHelper", logLevel);
    }

  /* Synchronization*/
  //Configure synchronization protocol
  Config::SetDefault ("ns3::LteUePhy::UeSlssInterScanningPeriodMax", TimeValue (MilliSeconds (interScanTimeMax)));
  Config::SetDefault ("ns3::LteUePhy::UeSlssInterScanningPeriodMin", TimeValue (MilliSeconds (interScanTimeMin)));
  Config::SetDefault ("ns3::LteUePhy::UeSlssScanningPeriod", TimeValue (MilliSeconds (scanTime)));
  Config::SetDefault ("ns3::LteUePhy::UeSlssMeasurementPeriod", TimeValue (MilliSeconds (measTime)));
  Config::SetDefault ("ns3::LteUePhy::UeSlssEvaluationPeriod", TimeValue (MilliSeconds (evalTime)));
  Config::SetDefault ("ns3::LteUePhy::UeRandomInitialSubframeIndication", BooleanValue (unsyncSl));
  if (slSyncActive)
    {
      Config::SetDefault ("ns3::LteUeRrc::UeSlssTransmissionEnabled", BooleanValue (true));
    }
  Config::SetDefault ("ns3::LteUeRrc::MinSrsrp", DoubleValue (-125));
  Config::SetDefault ("ns3::LteUePhy::MinSrsrp", DoubleValue (-125));
  Config::SetDefault ("ns3::LteUePhy::NSamplesSrsrpMeas", UintegerValue (4));
  /* END Synchronization*/

  // Configure scheduler for UE selected
  Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (rbSize));
  Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (mcs));
  Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (ktrp));

  Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled", BooleanValue (ctrlErrorModelEnabled));
  Config::SetDefault ("ns3::LteSpectrumPhy::CtrlFullDuplexEnabled", BooleanValue (!ctrlErrorModelEnabled));
  //If true, drop PSSCH and PSCCH when collision occurs, regardless of SINR
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (dropOnCollisionEnabled));

  //Convert pscchTrp string representation to decimal.
  std::stringstream ssHex;
  ssHex << std::hex << pscchTrpHexstring;
  uint64_t pscchTrpNumber;
  ssHex >> pscchTrpNumber;
  uint32_t pscchLength = std::ceil (std::log2l (pscchTrpNumber));
  std::cout << "Data offset : " << pscchLength << std::endl;

  // Create the helpers
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
  proseHelper->SetLteHelper (lteHelper);

  Ptr<Lte3gppHexGridEnbTopologyHelper> topoHelper = CreateObject<Lte3gppHexGridEnbTopologyHelper> ();
  topoHelper->SetLteHelper (lteHelper);
  //Fix the random number stream
  uint16_t randomStream = 1;
  randomStream += topoHelper->AssignStreams(randomStream);


  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  //Disable eNBs for out-of-coverage modeling
  lteHelper->DisableEnbPhy (true);

  // Configure antenna parameters not handled by the topology helper
  lteHelper->SetEnbAntennaModelType ("ns3::Parabolic3dAntennaModel");
  lteHelper->SetEnbAntennaModelAttribute ("MechanicalTilt", DoubleValue (15));

  // Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Hybrid3gppPropagationLossModel"));

  //Configure general values of the topology
  topoHelper->SetNumRings (numRings);
  topoHelper->SetInterSiteDistance (isd);
  topoHelper->SetMinimumDistance (10);
  topoHelper->SetSiteHeight (32); // 3GPP specs TS36.814 Table A.2.1.1-2 indicates 32m
  // Create eNbs
  NodeContainer sectorNodes;
  sectorNodes.Create (topoHelper->GetNumNodes ());
  // Install mobility (eNB)
  MobilityHelper mobilityeNodeB;
  mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB.Install (sectorNodes);
  // Compute the position of each site and antenna orientation
  NetDeviceContainer enbDevs = topoHelper->SetPositionAndInstallEnbDevice (sectorNodes);

  // Responder users
  NodeContainer ueResponders;
  if (ueRespondersPerSector > 0)
    {
      // Create nodes (responders)
      ueResponders.Create (ueRespondersPerSector * sectorNodes.GetN ());
      // Install mobility (responders)
      MobilityHelper mobilityResponders;
      mobilityResponders.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
      //Do not set position here, the topology helper takes care of deploying the nodes when called ahead!
      mobilityResponders.Install (ueResponders);
    }

  //Install LTE devices to all responders and deploy them in the sectors.
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));
  NetDeviceContainer ueRespondersDevs = topoHelper->DropUEsUniformlyPerSector (ueResponders);

  //Fix the random number stream for LTE stack
  randomStream += lteHelper->AssignStreams (ueRespondersDevs, randomStream);


  // Save the nodes positions
  std::ofstream m_outFile;
  if (std::ifstream ("nPositions.txt"))
    {
      remove ("nPositions.txt");
    }
  if (ueResponders.GetN () != 0)
    {
      m_outFile.open ("nPositions.txt", std::ofstream::out | std::ofstream::app);
      m_outFile << "Node\tID\tX\tY " << std::endl;
      for (uint32_t i = 0; i < sectorNodes.GetN (); ++i)
        {
          m_outFile << "eNB\t" << sectorNodes.Get (i)->GetId () << "\t" << sectorNodes.Get (i)->GetObject<MobilityModel> ()->GetPosition ().x << "\t" << sectorNodes.Get (i)->GetObject<MobilityModel> ()->GetPosition ().y << std::endl;
        }
      Ptr<Object> ueNetDevice;
      for (uint32_t i = 0; i < ueResponders.GetN (); ++i)
        {
          ueNetDevice = ueResponders.Get (i)->GetDevice (0);

          m_outFile << "UE\t" << ueNetDevice->GetObject<LteUeNetDevice> ()->GetImsi () << "\t" << ueResponders.Get (i)->GetObject<MobilityModel> ()->GetPosition ().x << "\t" << ueResponders.Get (i)->GetObject<MobilityModel> ()->GetPosition ().y << std::endl;
        }
      m_outFile.close ();
    }

  // Add X2 inteface
  lteHelper->AddX2Interface (sectorNodes);

  // Install the IP stack on the UEs
  InternetStackHelper internet;
  internet.Install (ueResponders);

  // Assign IP address to UEs
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (ueRespondersDevs);
  std::cout << "UE IPs:" << std::endl;
  for (uint32_t u = 0; u < ueIpIface.GetN (); ++u)
    {
      ueIpIface.GetAddress (u).Print (std::cout);
      std::cout << std::endl;
    }

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  for (uint32_t u = 0; u < ueResponders.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueResponders.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Attach each UE to the best available eNB
  lteHelper->Attach (ueRespondersDevs);

  // Create responders groups and applications
  double ulEarfcn = enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ();
  double ulBandwidth = enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlBandwidth ();
  std::vector < NetDeviceContainer > createdgroups;

  //createdgroups = proseHelper->AssociateForBroadcastWithTxEnabledToReceive (ueTxPower, ulEarfcn, ulBandwidth, ueRespondersDevs, -1000, numGroups, LteProseHelper::SLRSRP_TX_PW);
  createdgroups = proseHelper->AssociateForBroadcast (ueTxPower, ulEarfcn, ulBandwidth, ueRespondersDevs, -112, numGroups, LteSidelinkHelper::SLRSRP_PSBCH);

  //print groups created
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("prose-connections.txt");
  proseHelper->PrintGroups (createdgroups, stream);

  // Application Setup for Responders
  // Set Exponential Random Variables: ON(75% with mean 2.5s), OFF(25%)
  double meanOnTime = 2.5;
  double meanOffTime = meanOnTime / 3;

  Ptr<ExponentialRandomVariable> onRv = CreateObject<ExponentialRandomVariable> ();
  onRv->SetAttribute ("Mean", DoubleValue (meanOnTime));
  Ptr<ExponentialRandomVariable> offRv = CreateObject<ExponentialRandomVariable> ();
  offRv->SetAttribute ("Mean", DoubleValue (meanOffTime));

  // VoIP model defined in 36.843 Table A.2.1.3-1
  double encoderFrameLength = responderPktIntvl; //0.020 s for VoIP
  double pktRate = 1 / encoderFrameLength; //100
  uint16_t pktSizeBytes = responderPktSize;
  double BitRate = pktSizeBytes * 8 * pktRate;

  //Packet trace file
  stream = ascii.CreateFileStream ("pscr-ue-pck.tr");
  *stream->GetStream () << "time\ttx/rx\tNID\tIMSI\tUEtype\tsize\tIP[src]\tIP[dst]" << std::endl;
  std::ostringstream oss;

  std::vector<uint32_t> groupL2Addresses;
  uint32_t groupL2Address = 200;

  Ipv4AddressGenerator::Init (Ipv4Address ("225.0.0.0"), Ipv4Mask ("255.0.0.0"));
  Ipv4Address groupRespondersIpv4Address = Ipv4AddressGenerator::NextAddress (Ipv4Mask ("255.0.0.0"));

  uint16_t echoPort = 8000; //where to listen
  uint16_t grpEchoServerPort = 8000; //where to send

  std::vector < NetDeviceContainer >::iterator gIt;
  for (gIt = createdgroups.begin (); gIt != createdgroups.end (); gIt++)
    {
      //Create sidelink bearers
      //Use Tx for the group transmitter and Rx for all the receivers
      //split Tx/Rx
      NetDeviceContainer txUe ((*gIt).Get (0));
      NetDeviceContainer rxUes = proseHelper->RemoveNetDevice ((*gIt), (*gIt).Get (0));
      Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::TRANSMIT, groupRespondersIpv4Address, groupL2Address);
      proseHelper->ActivateSidelinkBearer (Seconds (1.0), txUe, tft);
      tft = Create<LteSlTft> (LteSlTft::RECEIVE, groupRespondersIpv4Address, groupL2Address);
      proseHelper->ActivateSidelinkBearer (Seconds (1.0), rxUes, tft);

      //Deploy applications
      // Application to generate traffic (installed only in transmitter)
      ApplicationContainer clientRespondersApps;
      if (onoff)
        {
          OnOffHelper clientOnOffHelper ("ns3::UdpSocketFactory",
                                         Address ( InetSocketAddress (groupRespondersIpv4Address, grpEchoServerPort)));
          clientOnOffHelper.SetAttribute ("PacketSize", UintegerValue (pktSizeBytes));
          clientOnOffHelper.SetAttribute ("DataRate", DataRateValue (BitRate));
          clientOnOffHelper.SetAttribute ("OnTime", PointerValue (onRv));
          clientOnOffHelper.SetAttribute ("OffTime", PointerValue (offRv));

          clientRespondersApps = clientOnOffHelper.Install ((*gIt).Get (0)->GetNode ());
          clientRespondersApps.Start (Seconds (respondersStart));
          clientRespondersApps.Stop (Seconds (simTime));
        }
      else
        {
          // UDP application
          UdpEchoClientHelper echoClientHelper (groupRespondersIpv4Address, grpEchoServerPort);
          echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (responderMaxPack));
          echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (responderPktIntvl)));
          echoClientHelper.SetAttribute ("PacketSize", UintegerValue (responderPktSize)); //1280

          clientRespondersApps = echoClientHelper.Install ((*gIt).Get (0)->GetNode ());
          clientRespondersApps.Start (Seconds (respondersStart));
          clientRespondersApps.Stop (Seconds (simTime));
        }

      // Set responders Tx traces
      for (uint16_t ac = 0; ac < clientRespondersApps.GetN (); ac++)
        {
          oss << "t\t" << (*gIt).Get (ac)->GetNode ()->GetId () << "\t" << (*gIt).Get (ac)->GetObject<LteUeNetDevice> ()->GetImsi () << "\tresp";
          Ipv4Address localAddrs = (*gIt).Get (ac)->GetNode ()->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
          std::cout << "Tx address: " << localAddrs << std::endl;
          clientRespondersApps.Get (ac)->TraceConnect ("TxWithAddresses", oss.str (), MakeBoundCallback (&PacketSrcDstAddrsTrace, stream, localAddrs));
          oss.str ("");
        }

      //store and increment addresses
      groupL2Addresses.push_back (groupL2Address);
      groupL2Address++;
      groupRespondersIpv4Address = Ipv4AddressGenerator::NextAddress (Ipv4Mask ("255.0.0.0"));
    }

  // Application to receive traffic (Installed in all responders)

  PacketSinkHelper clientPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), echoPort));
  ApplicationContainer clientRespondersSrvApps = clientPacketSinkHelper.Install (ueResponders);
  clientRespondersSrvApps.Start (Seconds (respondersStart));
  clientRespondersSrvApps.Stop (Seconds (simTime + 1));

  // Set responders Rx traces
  for (uint16_t ac = 0; ac < clientRespondersSrvApps.GetN (); ac++)
    {
      Ipv4Address localAddrs =  ueRespondersDevs.Get (ac)->GetNode ()->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
      std::cout << "Rx address: " << localAddrs << std::endl;
      oss << "r\t" << ueResponders.Get (ac)->GetId () << "\t" << ueResponders.Get (ac)->GetDevice (0)->GetObject<LteUeNetDevice> ()->GetImsi () << "\tresp";
      clientRespondersSrvApps.Get (ac)->TraceConnect ("RxWithAddresses", oss.str (), MakeBoundCallback (&PacketSrcDstAddrsTrace, stream, localAddrs));
      oss.str ("");
    }


  //Sidelink configuration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);

  LteRrcSap::SlPreconfiguration preconfiguration;

  preconfiguration.preconfigGeneral.carrierFreq = 23330;
  preconfiguration.preconfigGeneral.slBandwidth = 50;
  preconfiguration.preconfigComm.nbPools = 1;

  //control
  preconfiguration.preconfigComm.pools[0].scCpLen.cplen = LteRrcSap::SlCpLen::NORMAL;
  preconfiguration.preconfigComm.pools[0].scPeriod.period = LteRrcSap::PeriodAsEnum (slPeriod).period;
  preconfiguration.preconfigComm.pools[0].scTfResourceConfig.prbNum = pscchRbs;
  preconfiguration.preconfigComm.pools[0].scTfResourceConfig.prbStart = 3;
  preconfiguration.preconfigComm.pools[0].scTfResourceConfig.prbEnd = 46;
  preconfiguration.preconfigComm.pools[0].scTfResourceConfig.offsetIndicator.offset = 0;
  preconfiguration.preconfigComm.pools[0].scTfResourceConfig.subframeBitmap.bitmap = pscchTrpNumber;
  //data
  preconfiguration.preconfigComm.pools[0].dataCpLen.cplen = LteRrcSap::SlCpLen::NORMAL;
  preconfiguration.preconfigComm.pools[0].dataHoppingConfig.hoppingParameter = 0;
  preconfiguration.preconfigComm.pools[0].dataHoppingConfig.numSubbands = LteRrcSap::SlHoppingConfigComm::ns4;
  preconfiguration.preconfigComm.pools[0].dataHoppingConfig.rbOffset = 0;
  //UE selected parameters
  preconfiguration.preconfigComm.pools[0].trptSubset.subset = std::bitset<3> (0x7); //ktrp1=1, ktrp2=1, ktrp4=1 All T-RPT set available
  preconfiguration.preconfigComm.pools[0].dataTfResourceConfig.prbNum = 25;
  preconfiguration.preconfigComm.pools[0].dataTfResourceConfig.prbStart = 0;
  preconfiguration.preconfigComm.pools[0].dataTfResourceConfig.prbEnd = 49;
  preconfiguration.preconfigComm.pools[0].dataTfResourceConfig.offsetIndicator.offset = pscchLength; //allow for control frames
  preconfiguration.preconfigComm.pools[0].dataTfResourceConfig.subframeBitmap.bitmap = std::bitset<40> (0xFFFFFFFFFF);

  preconfiguration.preconfigComm.pools[0].scTxParameters.alpha = LteRrcSap::SlTxParameters::al09;
  preconfiguration.preconfigComm.pools[0].scTxParameters.p0 = -40;
  preconfiguration.preconfigComm.pools[0].dataTxParameters.alpha = LteRrcSap::SlTxParameters::al09;
  preconfiguration.preconfigComm.pools[0].dataTxParameters.p0 = -40;

  /* Synchronization*/
  //Synchronization parameters
  preconfiguration.preconfigSync.syncOffsetIndicator1 = 18;
  preconfiguration.preconfigSync.syncOffsetIndicator2 = 29;
  preconfiguration.preconfigSync.syncTxThreshOoC = syncTxThreshOoC;
  preconfiguration.preconfigSync.syncRefDiffHyst = syncRefDiffHyst;
  preconfiguration.preconfigSync.syncRefMinHyst = syncRefMinHyst;
  preconfiguration.preconfigSync.filterCoefficient = filterCoefficient;
  /* END Synchronization*/

  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
  lteHelper->InstallSidelinkConfiguration (ueRespondersDevs, ueSidelinkConfiguration);


  /*Synchronization*/
  //Tracing synchronization stuffs
  Ptr<OutputStreamWrapper> streamSyncRef = ascii.CreateFileStream ("SyncRef.txt");
  *streamSyncRef->GetStream () << "Time\tIMSI\tprevSLSSID\tprevRxOffset\tprevFrameNo\tprevSframeNo\tcurrSLSSID\tcurrRxOffset\tcurrFrameNo\tcurrSframeNo" << std::endl;
  Ptr<OutputStreamWrapper> streamSendOfSlss = ascii.CreateFileStream ("TxSlss.txt");
  *streamSendOfSlss->GetStream () << "Time\tIMSI\tSLSSID\ttxOffset\tinCoverage\tFrameNo\tSframeNo" << std::endl;
  Ptr<OutputStreamWrapper> streamFirstScan = ascii.CreateFileStream ("FirstScan.txt");
  *streamFirstScan->GetStream () << "Time\tIMSI" << std::endl;

  //Set initial SLSSID and start of the first scanning for all UEs
  Ptr<UniformRandomVariable> rndSlssid = CreateObject<UniformRandomVariable> ();
  rndSlssid->SetAttribute ("Min", DoubleValue (100000.0));
  if (unsyncSl)
    {
      rndSlssid->SetAttribute ("Max", DoubleValue (200000.0));
    }
  else
    {
      rndSlssid->SetAttribute ("Max", DoubleValue (100000.0));
    }

  Ptr<UniformRandomVariable> rndStartScanning = CreateObject<UniformRandomVariable> ();
  rndStartScanning->SetAttribute ("Min", DoubleValue (firstScanTimeMin));
  rndStartScanning->SetAttribute ("Max", DoubleValue (firstScanTimeMax));

  std::vector < NetDeviceContainer >::iterator groupIt;
  for (groupIt = createdgroups.begin (); groupIt != createdgroups.end (); groupIt++)
    {
      for (uint32_t i = 0; i < (*groupIt).GetN (); ++i)
        {
          uint32_t slssid = rndSlssid->GetInteger ();
          (*groupIt).Get (i)->GetObject<LteUeNetDevice> ()->GetRrc ()->SetSlssid (slssid);

          if ((*groupIt).Get (i)->GetObject<LteUeNetDevice> ()->GetPhy ()->GetFirstScanningTime () == MilliSeconds (0))
            {
              uint32_t t = 0;
              if (slSyncActive)
                {
                  t = rndStartScanning->GetInteger ();
                  (*groupIt).Get (i)->GetObject<LteUeNetDevice> ()->GetPhy ()->SetFirstScanningTime (MilliSeconds (t));
                }
              *streamFirstScan->GetStream () << t << "\t" << (*groupIt).Get (i)->GetObject<LteUeNetDevice> ()->GetRrc ()->GetImsi () << std::endl;
            }
        }
    }

  //Tracing the change of synchronization reference
  for (uint32_t i = 0; i < ueRespondersDevs.GetN (); ++i)
    {
      Ptr<LteUeRrc> ueRrc =  ueRespondersDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetRrc ();
      ueRrc->TraceConnectWithoutContext ("ChangeOfSyncRef", MakeBoundCallback (&NotifyChangeOfSyncRef, streamSyncRef));
      ueRrc->TraceConnectWithoutContext ("SendSLSS", MakeBoundCallback (&NotifySendOfSlss, streamSendOfSlss));
    }
  for (uint32_t i = 0; i < ueRespondersDevs.GetN (); ++i)
    {
      Ptr<LteUeRrc> ueRrc =  ueRespondersDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetRrc ();
      LteUeRrc::SlChangeOfSyncRefStatParameters param;
      param.imsi = ueRrc->GetImsi ();
      param.prevSlssid = 0;
      param.prevRxOffset = 0;
      param.prevFrameNo = 0;
      param.prevSubframeNo = 0;
      param.currSlssid = ueRrc->GetSlssid ();
      param.currRxOffset = 0;
      param.currFrameNo = ueRrc->GetFrameNumber ();
      param.currSubframeNo = ueRrc->GetSubFrameNumber ();
      NotifyChangeOfSyncRef (streamSyncRef,param );
    }
  /*END Synchronization*/

  NS_LOG_INFO ("Enabling Sidelink traces...");

  lteHelper->EnableSlPscchMacTraces ();
  lteHelper->EnableSlPsschMacTraces ();

  lteHelper->EnableSlRxPhyTraces ();
  lteHelper->EnableSlPscchRxPhyTraces ();



  Simulator::Stop (Seconds (simTime));

  std::cout << "Simulation running..." << std::endl;
  Simulator::Run ();

  /*Synchronization*/

  for (uint32_t i = 0; i < ueRespondersDevs.GetN (); ++i)
    {
      Ptr<LteUeRrc> ueRrc =  ueRespondersDevs.Get (i)->GetObject<LteUeNetDevice> ()->GetRrc ();

      LteUeRrc::SlChangeOfSyncRefStatParameters param;
      param.imsi = ueRrc->GetImsi ();
      param.prevSlssid = ueRrc->GetSlssid ();
      param.prevFrameNo = ueRrc->GetFrameNumber ();
      param.prevSubframeNo = ueRrc->GetSubFrameNumber ();
      param.currSlssid = 0;
      param.currRxOffset = 0;
      param.currFrameNo = 0;
      param.currSubframeNo = 0;

      NotifyChangeOfSyncRef (streamSyncRef,param);
    }
  /*END Synchronization*/


  Simulator::Destroy ();
  return 0;

}

