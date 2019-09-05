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

/*
This example is used to validate the maximum achievable data rate on the Physical Sidelink Shared
Channel (PSSCH) between 2 UEs given a particular resource pool configuration.
Parameters include:
- period: duration of the Sidelink period
- pscchLength: length of the physical Sidelink control channel (PSCCH)
- ktrp: repetition pattern defining how many subframes can be used for Sidelink
- mcs: modulation and coding scheme for transmission on the Sidelink shared channel
- rbSize: allocation size in resource block (RB)
- simTime: Simulation time in seconds
- SlRxOutputFilename: Name of the file where the Sidelink RxPhy results will be saved
*/

NS_LOG_COMPONENT_DEFINE ("wns3-2017-pssch");

using namespace ns3;

int
main (int argc, char *argv[])
{
  uint32_t mcs = 10; // Modulation and Coding Scheme
  uint32_t rbSize = 2; // PSSCH subchannel allocation size in RBs
  uint32_t ktrp = 2; // Transmissions opportunities in a Time Repetition Pattern(number of subframes to transmit on)
  uint32_t pscchLength = 8; // Number of PSCCH subframes
  std::string period = "sf40"; // Length of Sidelink period in milliseconds
  double dist = 10; // Distance between the UEs
  double simTime = 10.0; // Simulation time in seconds
  uint32_t ueCount = 2; // Number of UEs
  bool  enableNsLogs = false; // If enabled will output NS LOGs
  std::string SlRxOutputFilename = "SlRxPhyStats.txt";

  // Set error models
  Config::SetDefault ("ns3::LteSpectrumPhy::SlCtrlErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDataErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (false));


  // Command line arguments
  CommandLine cmd;
  cmd.AddValue ("period", "Sidelink period", period);
  cmd.AddValue ("pscchLength", "Length of PSCCH.", pscchLength);
  cmd.AddValue ("ktrp", "Repetition.", ktrp);
  cmd.AddValue ("mcs", "MCS.", mcs);
  cmd.AddValue ("rbSize", "Allocation size.", rbSize);
  cmd.AddValue ("simTime", "Simulation time", simTime);
  cmd.AddValue ("SlRxOutputFilename", "Name of the file where the Sidelink results will be saved.", SlRxOutputFilename);
  cmd.AddValue ("enableNsLogs", "Enable NS logs", enableNsLogs);

  cmd.Parse (argc, argv);

  NS_LOG_INFO ("Configuring UE settings...");
  Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (mcs));
  Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (rbSize));
  Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (ktrp));
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
  Config::SetDefault ("ns3::LteUePowerControl::Pcmax", DoubleValue (23.0));
  Config::SetDefault ("ns3::LteUePowerControl::PscchTxPower", DoubleValue (23.0));
  Config::SetDefault ("ns3::LteUePowerControl::PsschTxPower", DoubleValue (23.0));
  Config::SetDefault ("ns3::PhyRxStatsCalculator::SlRxOutputFilename", StringValue (SlRxOutputFilename));

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  if (enableNsLogs)
    {
      LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_ALL);
      LogComponentEnable ("wns3-2017-pssch", logLevel);

      LogComponentEnable ("LteSpectrumPhy", logLevel);
      LogComponentEnable ("LteUePhy", logLevel);
      LogComponentEnable ("LteUeRrc", logLevel);
      LogComponentEnable ("LteEnbPhy", logLevel);
      LogComponentEnable ("LteUeMac", logLevel);
      LogComponentEnable ("LteSlUeRrc", logLevel);
      LogComponentEnable ("LteSidelinkHelper", logLevel);
      LogComponentEnable ("LteHelper", logLevel);
    }

  NS_LOG_INFO ("Creating helpers...");
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));
  // Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::FriisPropagationLossModel"));
  lteHelper->DisableEnbPhy (true);
  lteHelper->Initialize ();

  // Since we are not installing eNB, we need to set the frequency attribute of pathloss model here
  // Frequency for Public Safety use case (band 14 : 788 - 798 MHz for Uplink)
  double ulFreq = LteSpectrumValueHelper::GetCarrierFrequency (23330);
  NS_LOG_LOGIC ("UL freq: " << ulFreq);
  Ptr<Object> uplinkPathlossModel = lteHelper->GetUplinkPathlossModel ();
  Ptr<PropagationLossModel> lossModel = uplinkPathlossModel->GetObject<PropagationLossModel> ();
  NS_ABORT_MSG_IF (lossModel == nullptr, "No PathLossModel");
  bool ulFreqOk = uplinkPathlossModel->SetAttributeFailSafe ("Frequency", DoubleValue (ulFreq));
  if (!ulFreqOk)
    {
      NS_LOG_WARN ("UL propagation model does not have a Frequency attribute");
    }

  Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
  proseHelper->SetLteHelper (lteHelper);

  NS_LOG_INFO ("Deploying UE's...");
  NodeContainer ueResponders;
  ueResponders.Create (ueCount);

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0),
                                 "MinY", DoubleValue (-5.0),
                                 "DeltaX", DoubleValue (dist),
                                 "DeltaY", DoubleValue (dist),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (ueResponders);

  NS_LOG_INFO ("Installing UE network devices...");
  NetDeviceContainer ueDevs;
  NetDeviceContainer ueRespondersDevs = lteHelper->InstallUeDevice (ueResponders);
  ueDevs.Add (ueRespondersDevs);

  //Fix the random number stream
  uint16_t randomStream = 1;
  randomStream += lteHelper->AssignStreams (ueDevs, randomStream);

  NS_LOG_INFO ("Installing IP stack...");
  InternetStackHelper internet;
  internet.Install (ueResponders);

  NS_LOG_INFO ("Allocating IP addresses and setting up network route...");
  Ipv4InterfaceContainer ueIpIface;
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  ueIpIface = epcHelper->AssignUeIpv4Address (ueDevs);
  for (uint32_t u = 0; u < ueResponders.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueResponders.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  NS_LOG_INFO ("Attaching UE's to LTE network...");
  lteHelper->Attach (ueDevs);

  NS_LOG_INFO ("Installing applications...");
  // UDP application
  Ipv4Address groupAddress ("225.0.0.0"); //use multicast address as destination
  UdpClientHelper udpClient (groupAddress, 8000);
  udpClient.SetAttribute ("MaxPackets", UintegerValue (5000000));
  udpClient.SetAttribute ("Interval", TimeValue (Seconds (0.001)));
  udpClient.SetAttribute ("PacketSize", UintegerValue (1000));

  ApplicationContainer clientApps = udpClient.Install (ueResponders);
  clientApps.Get (0)->SetStartTime (Seconds (3.0));
  clientApps.Get (1)->SetStartTime (Seconds (1000.0));


  NS_LOG_INFO ("Creating Sidelink configuration...");
  uint32_t groupL2Address = 0xFF;
  Ptr<LteSlTft> tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, groupAddress, groupL2Address);
  proseHelper->ActivateSidelinkBearer (Seconds (1.0), ueDevs, tft);

  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);

  LteRrcSap::SlPreconfiguration preconfiguration;
  preconfiguration.preconfigGeneral.carrierFreq = 23330;
  preconfiguration.preconfigGeneral.slBandwidth = 50;
  preconfiguration.preconfigComm.nbPools = 1;
  LteSlPreconfigPoolFactory pfactory;
  //build PSCCH bitmap value
  uint64_t pscchBitmapValue = 0x0;
  for (uint32_t i = 0; i < pscchLength; i++)
    {
      pscchBitmapValue = pscchBitmapValue << 1 | 0x1;
    }
  std::cout << "bitmap=" << std::hex << pscchBitmapValue << '\n';
  std::cout << std::dec;
  pfactory.SetControlBitmap (pscchBitmapValue);
  pfactory.SetControlPeriod (period);
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

  NS_LOG_INFO ("Starting simulation...");
  Simulator::Stop (Seconds (simTime + 2));
  Simulator::Run ();
  Simulator::Destroy ();

  NS_LOG_INFO ("Done.");

  return 0;

}

