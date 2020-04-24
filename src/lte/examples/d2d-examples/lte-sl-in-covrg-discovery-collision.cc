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
 *                                   (0.0, 17.32, 1.5)
 *                                         UE3
 *                                          |
 *                                          |
 *                                          |
 *                                          |
 *                                          |
 *    UE2 (-10.0, 0.0, 1.5).....(10 m).....eNB (0.0, 0.0, 30.0).....(10 m).....UE1 (10.0, 0.0, 1.5)
 *
 * This example simulates an in-coverage discovery. There are three UEs, which are attached
 * to an eNB; UE1 and UE2 are configured to announce the discovery messages, while UE3 is
 * configured to monitor/receive these announcements. The discovery pool is set in a way that
 * both the announcing UEs have only 2 similar RBs and one similar subframe to transmit on.
 * By doing this, a user can use the attribute DropRbOnCollisionEnabled of LteSpectrumPhy
 * class to receive or discard the collided TBs. In this example, if DropRbOnCollisionEnabled
 * is false, the UE3 will be able to decode only the TBs from UE1 (RNTI 3, App code 1). This
 * is because the SINR, in spite of the collision is high enough that the UE can still decode
 * them. Moreover, in this particular example the TBs from UE1 and UE2 would have similar
 * SINR, therefore, only the TBs which would arrive first (here TBs from UE1) will be decoded.
 *
 * Hint: A user should pay extra attention to the error model configuration, as it
 * influences the reception of the discovery signals, e.g., dropping the collided TBs,
 * use of PSDCH BLER curves to compute the BLER.
 *
 * The example can be run as follows:
 *
 * ./waf --run "lte-sl-in-covrg-discovery-collision --simTime=3"
 *
 */

NS_LOG_COMPONENT_DEFINE ("LteSlInCovrgDiscoveryCollision");

int main (int argc, char *argv[])
{
  Time simTime = Seconds (3);
  bool enableNsLogs = false;

  CommandLine cmd;
  cmd.AddValue ("simTime", "Total duration of the simulation", simTime);
  cmd.AddValue ("enableNsLogs", "Enable ns-3 logging (debug builds)", enableNsLogs);
  cmd.Parse (argc, argv);

  //Set the frequency

  Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (18100));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

  // Set error models
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDiscoveryErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (true));

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
  //Set the eNBs power in dBm
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (30.0));

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();
  // parse again so we can override input file default values via command line
  cmd.Parse (argc, argv);

  if (enableNsLogs)
    {
      LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_ALL);

      LogComponentEnable ("LteUeRrc", logLevel);
      LogComponentEnable ("LteUeMac", logLevel);
      LogComponentEnable ("LteSpectrumPhy", logLevel);
      LogComponentEnable ("LteSlInterference", logLevel);

      LogComponentEnable ("LteUePhy", logLevel);
      LogComponentEnable ("LteEnbPhy", logLevel);
    }

  //Create the helpers
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  //Create and set the EPC helper
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  ////Create Sidelink helper and set lteHelper
  Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
  proseHelper->SetLteHelper (lteHelper);

  //Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Cost231PropagationLossModel"));

  //Enable Sidelink
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

  //Create nodes (eNb + UEs)
  NodeContainer enbNode;
  enbNode.Create (1);
  NS_LOG_INFO ("eNb node id = [" << enbNode.Get (0)->GetId () << "]");
  NodeContainer ueNodes;
  ueNodes.Create (3);
  NS_LOG_INFO ("UE 1 node id = [" << ueNodes.Get (0)->GetId () << "]");
  NS_LOG_INFO ("UE 2 node id = [" << ueNodes.Get (1)->GetId () << "]");
  NS_LOG_INFO ("UE 3 node id = [" << ueNodes.Get (2)->GetId () << "]");

  //Position of the nodes
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  positionAllocEnb->Add (Vector (0.0, 0.0, 30.0));
  Ptr<ListPositionAllocator> positionAllocUes = CreateObject<ListPositionAllocator> ();
  positionAllocUes->Add (Vector (10.0, 0.0, 1.5));
  positionAllocUes->Add (Vector (-10.0, 0.0, 1.5));
  positionAllocUes->Add (Vector (0.0, 17.32, 1.5));

  //Install mobility
  MobilityHelper mobilityeNodeB;
  mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB.SetPositionAllocator (positionAllocEnb);
  mobilityeNodeB.Install (enbNode);

  MobilityHelper mobilityUes;
  mobilityUes.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUes.SetPositionAllocator (positionAllocUes);

  for (uint32_t i = 0; i < ueNodes.GetN (); ++i)
    {
      mobilityUes.Install (ueNodes.Get (i));
    }

  //Install LTE devices to the nodes and fix the random number stream
  int64_t randomStream = 1;
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNode);
  randomStream += lteHelper->AssignStreams (enbDevs, randomStream);
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);
  randomStream += lteHelper->AssignStreams (ueDevs, randomStream);

  //Configure Sidelink Pool
  Ptr<LteSlEnbRrc> enbSidelinkConfiguration = CreateObject<LteSlEnbRrc> ();
  enbSidelinkConfiguration->SetDiscEnabled (true);

  LteRrcSap::SlDiscTxResourcesSetup pool;
  pool.setup = LteRrcSap::SlDiscTxResourcesSetup::UE_SELECTED;
  pool.ueSelected.havePoolToRelease = false;
  pool.ueSelected.havePoolToAdd = true;
  pool.ueSelected.poolToAddModList.nbPools = 1;
  pool.ueSelected.poolToAddModList.pools[0].poolIdentity = 1;

  LteSlDiscResourcePoolFactory pfactory;
  pfactory.SetDiscCpLen ("NORMAL");
  pfactory.SetDiscPeriod ("rf32");
  pfactory.SetNumRetx (0);
  pfactory.SetNumRepetition (1);
  pfactory.SetDiscPrbNum (2);
  pfactory.SetDiscPrbStart (1);
  pfactory.SetDiscPrbEnd (2);
  pfactory.SetDiscOffset (0);
  pfactory.SetDiscBitmap (0x00001);

  pool.ueSelected.poolToAddModList.pools[0].pool =  pfactory.CreatePool ();

  //Install Sidelink configuration for eNBs
  enbSidelinkConfiguration->AddDiscPool (pool);
  lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

  // Install Sidelink configuration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  uint8_t nb = 3;
  ueSidelinkConfiguration->SetDiscTxResources (nb);
  ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());
  ueSidelinkConfiguration->SetDiscEnabled (true);
  lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);

  //Install the IP stack on the UEs and assign IP address
  InternetStackHelper internet;
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));

  // set the default gateway for the UE
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  //Attach each UE to the best available eNB
  lteHelper->Attach (ueDevs);

  NS_LOG_INFO ("Configuring discovery applications");
  std::map<Ptr<NetDevice>, std::list<uint32_t> > announceApps;
  std::map<Ptr<NetDevice>, std::list<uint32_t> > monitorApps;

  for (uint32_t i = 1; i <= ueNodes.GetN (); ++i)
    {
      if (i != ueNodes.GetN ())
        {
          announceApps[ueDevs.Get (i - 1)].push_back (i);
        }
      for (uint32_t j = 1; j <= ueNodes.GetN (); ++j)
        {
          if (i == ueNodes.GetN () && j < ueNodes.GetN ())
            {
              monitorApps[ueDevs.Get (i - 1)].push_back (j);
            }
        }
    }

  for (auto itAnnounceApps : announceApps)
    {
      Ptr<LteUeNetDevice> ueNetDevice = DynamicCast<LteUeNetDevice> (itAnnounceApps.first);
      std::list<uint32_t> apps = itAnnounceApps.second;
      std::cout << "Scheduling " << apps.size () << " announce apps for UE with IMSI = " << ueNetDevice->GetImsi () << std::endl;
      std::list<uint32_t>::iterator itAppList;
      for (auto itAppList : apps)
        {
          std::cout << "Announcing App code = " << itAppList << std::endl;
        }

      Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartDiscoveryApps, proseHelper, ueNetDevice, apps, LteSlUeRrc::Announcing);
    }

  for (auto itMonitorApps : monitorApps)
    {
      Ptr<LteUeNetDevice> ueNetDevice = DynamicCast<LteUeNetDevice> (itMonitorApps.first);
      std::list<uint32_t> apps = itMonitorApps.second;
      std::cout << "Scheduling " << apps.size () << " monitor apps for UE with IMSI = " << ueNetDevice->GetImsi () << std::endl;
      std::list<uint32_t>::iterator itAppList;
      for (auto itAppList : apps)
        {
          std::cout << "Monitoring App code = " << itAppList << std::endl;
        }

      Simulator::Schedule (Seconds (2.0),&LteSidelinkHelper::StartDiscoveryApps, proseHelper, ueNetDevice, apps, LteSlUeRrc::Monitoring);
    }

  ///*** End of application configuration ***///

  NS_LOG_INFO ("Enabling Sidelink discovery reception trace...");

  lteHelper->EnableSlRxPhyTraces ();
  lteHelper->EnableSlPsdchMacTraces ();
  lteHelper->EnableDiscoveryMonitoringRrcTraces ();

  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (simTime);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;

}
