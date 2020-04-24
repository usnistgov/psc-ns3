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
 *
 *                         (0.0, 17.32, 1.5)
 *                               UE3
 *                               /\
 *                              /  \
 *                             /    \
 *                            /      \
 *                   (20 m)  /        \  (20 m)
 *                          /          \
 *                         /            \
 *                        /              \
 *                       /                \
 *                     UE2.....(20 m).....UE1
 *             (-10.0, 0.0, 1.5)   (10.0, 0.0, 1.5)
 *
 * This example simulates an out-of-coverage discovery. There are three UEs; UE1 and UE2 are
 * configured to announce the discovery messages, while UE3 is configured to monitor/receive
 * these announcements. The discovery pool is set in a way that both the announcing UEs
 * have only 2 similar RBs and one similar subframe to transmit on. By doing this, a user
 * can use the attribute DropRbOnCollisionEnabled of LteSpectrumPhy class to receive or
 * discard the collided TBs. In this example, if DropRbOnCollisionEnabled is false, the
 * UE3 will be able to decode only the TBs from UE1 (RNTI 1, App code 1). This is because
 * the SINR, in spite of the collision is high enough that the UE can still decode them.
 * Moreover, in this particular example the TBs from UE1 and UE2 would have similar
 * SINR, therefore, only the TBs which would arrive first (here TBs from UE1) will be decoded.
 *
 *
 * Hint: A user should pay extra attention to the error model configuration, as it
 * influences the reception of the discovery signals, e.g., dropping the collided TBs,
 * use of PSDCH BLER curves to compute the BLER.
 *
 * The example can be run as follows:
 *
 * ./waf --run "lte-sl-out-of-covrg-discovery-collision --simTime=3"
 *
 */

NS_LOG_COMPONENT_DEFINE ("LteSlOutOfCovrgDiscoveryCollision");

int main (int argc, char *argv[])
{
  Time simTime = Seconds (3);
  bool enableNsLogs = false;

  CommandLine cmd;
  cmd.AddValue ("simTime", "Total duration of the simulation", simTime);
  cmd.AddValue ("enableNsLogs", "Enable ns-3 logging (debug builds)", enableNsLogs);
  cmd.Parse (argc, argv);

  // Set error models
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDiscoveryErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (true));

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));

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

  NodeContainer ueNodes;
  ueNodes.Create (3);
  NS_LOG_INFO ("UE 1 node id = [" << ueNodes.Get (0)->GetId () << "]");
  NS_LOG_INFO ("UE 2 node id = [" << ueNodes.Get (1)->GetId () << "]");
  NS_LOG_INFO ("UE 3 node id = [" << ueNodes.Get (2)->GetId () << "]");

  //Position of the UE nodes
  Ptr<ListPositionAllocator> positionAllocUes = CreateObject<ListPositionAllocator> ();
  positionAllocUes->Add (Vector (10.0, 0.0, 1.5));
  positionAllocUes->Add (Vector (-10.0, 0.0, 1.5));
  positionAllocUes->Add (Vector (0.0, 17.32, 1.5));

  //Install mobility
  MobilityHelper mobilityUes;
  mobilityUes.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUes.SetPositionAllocator (positionAllocUes);

  for (uint32_t i = 0; i < ueNodes.GetN (); ++i)
    {
      mobilityUes.Install (ueNodes.Get (i));
    }

  //Install LTE devices to the nodes
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

  //Fix the random number stream
  uint16_t randomStream = 1;
  randomStream += lteHelper->AssignStreams (ueDevs, randomStream);

  // Install Sidelink configuration for the UEs
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetDiscEnabled (true);

  LteRrcSap::SlPreconfiguration preconfiguration;

  preconfiguration.preconfigGeneral.carrierFreq = 23330;
  preconfiguration.preconfigGeneral.slBandwidth = 50;
  preconfiguration.preconfigDisc.nbPools = 1;

  LteSlDiscPreconfigPoolFactory pfactory;

  pfactory.SetDiscCpLen ("NORMAL");
  pfactory.SetDiscPeriod ("rf32");
  pfactory.SetNumRetx (0);
  pfactory.SetNumRepetition (1);

  pfactory.SetDiscPrbNum (2);
  pfactory.SetDiscPrbStart (1);
  pfactory.SetDiscPrbEnd (2);
  pfactory.SetDiscOffset (0);
  pfactory.SetDiscBitmap (0x00001);

  preconfiguration.preconfigDisc.pools[0] = pfactory.CreatePool ();
  preconfiguration.preconfigDisc.nbPools = 1;

  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
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


  NS_LOG_INFO ("Configuring discovery applications");
   std::map<Ptr<NetDevice>, std::list<uint32_t> > announceApps;
   std::map<Ptr<NetDevice>, std::list<uint32_t> > monitorApps;
   
   for (uint32_t i = 1; i <= ueNodes.GetN (); ++i)
   {
      if (i != ueNodes.GetN ())
      {
         announceApps[ueDevs.Get (i - 1)].push_back(i);
      }
      for (uint32_t j = 1; j <= ueNodes.GetN (); ++j)
      {
         if (i == ueNodes.GetN () && j < ueNodes.GetN ())
         {
            monitorApps[ueDevs.Get (i - 1)].push_back(j);
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
