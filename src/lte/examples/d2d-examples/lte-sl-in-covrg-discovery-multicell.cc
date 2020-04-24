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

NS_LOG_COMPONENT_DEFINE ("LteSlInCovrgDiscoveryMulticell");

/**
 * Structure to store the number of received discovery messages
 */
struct DiscoveryMonitoringTracer
{
  void Trace (uint64_t imsi, uint16_t cellId, uint16_t rnti, LteSlDiscHeader discHeader); ///< Callback function
  uint32_t  m_nRxMsgs {
    0
  };                       ///< Number of messages received
};

void
DiscoveryMonitoringTracer::Trace (uint64_t imsi, uint16_t cellId, uint16_t rnti, LteSlDiscHeader discHeader)
{
  m_nRxMsgs++;
}

/*
 * The topology is the following:
 *
 *       |------10 m------|------------20 m------------|------10 m------|
 *     eNB1      <--     UE1                          UE2     -->     eNB2
 *(0.0, 0.0, 30.0) (10.0, 0.0, 1.5)           (30.0, 0.0, 1.5)   (40.0, 0.0, 30.0)
 *
 * UE1 is attached to eNB1, and UE2 is attached to eNB2.
 *
 * This scenario illustrates two in-coverage UEs that are able to perform
 * direct discovery using the sidelink even though they are attached to
 * different eNBs. To do so, they use a common sidelink discovery pool.
 * This pool is configured in the scenario and installed in both eNBs,
 * which broadcast it to their respective attached UE in simulation time.
 * Each UE is interested in (monitoring) the application the other UE is
 * providing (announcing).
 * With Model A, each UE receives announcements ('ContentType' = 0) from the other UE.
 * With Model B, each UE receives queries (ContentType = 1) and responses (ContentType = 0)
 * from the other UE.
 *
 */

int main (int argc, char *argv[])
{
  Time simTime = Seconds (6);
  bool enableNsLogs = false;
  bool useModelB = false;
  bool test = false;

  CommandLine cmd;
  cmd.AddValue ("simTime", "Total duration of the simulation", simTime);
  cmd.AddValue ("enableNsLogs", "Enable ns-3 logging (debug builds)", enableNsLogs);
  cmd.AddValue ("useModelB", "Use discovery model B instead of model A", useModelB);
  cmd.AddValue ("test", "Test if the scenario behaves as expected", test);

  cmd.Parse (argc, argv);

  //Set the frequency
  Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (100));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (18100));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

  // Set error models
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDiscoveryErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (false));

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
      LogComponentEnable ("LteUePhy", logLevel);
      LogComponentEnable ("LteEnbPhy", logLevel);
    }

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
  //Set the eNBs power in dBm
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (30.0));

  //Sidelink bearers activation time
  Time slBearersActivationTime = Seconds (2.0);

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

  //Install LTE devices to the nodes and fix the random number stream
  int64_t randomStream = 1;
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNodes);
  randomStream += lteHelper->AssignStreams (enbDevs, randomStream);
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);
  randomStream += lteHelper->AssignStreams (ueDevs, randomStream);

  //Configure Sidelink
  Ptr<LteSlEnbRrc> enbSidelinkConfiguration = CreateObject<LteSlEnbRrc> ();
  enbSidelinkConfiguration->SetDiscEnabled (true);

  //Configure discovery pool
  LteRrcSap::SlDiscTxResourcesSetup pool;

  LteSlDiscResourcePoolFactory pfactory;
  pfactory.SetDiscCpLen ("NORMAL");
  pfactory.SetDiscPeriod ("rf32");
  pfactory.SetNumRetx (0);
  pfactory.SetNumRepetition (1);
  pfactory.SetDiscPrbNum (10);
  pfactory.SetDiscPrbStart (10);
  pfactory.SetDiscPrbEnd (40);
  pfactory.SetDiscOffset (0);
  pfactory.SetDiscBitmap (0x11111);
  pfactory.SetDiscTxProbability ("p100");

  pool.setup = LteRrcSap::SlDiscTxResourcesSetup::UE_SELECTED;

  pool.ueSelected.havePoolToRelease = false;
  pool.ueSelected.havePoolToAdd = true;
  pool.ueSelected.poolToAddModList.nbPools = 1;
  pool.ueSelected.poolToAddModList.pools[0].poolIdentity = 1;

  pool.ueSelected.poolToAddModList.pools[0].pool =  pfactory.CreatePool ();

  enbSidelinkConfiguration->AddDiscPool (pool);

  //Install Sidelink configuration for eNBs
  lteHelper->InstallSidelinkConfiguration (enbDevs.Get (0), enbSidelinkConfiguration);
  lteHelper->InstallSidelinkConfiguration (enbDevs.Get (1), enbSidelinkConfiguration);

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

  //Attach each UE to the closest eNB
  lteHelper->Attach (ueDevs.Get (0), enbDevs.Get (0));
  lteHelper->Attach (ueDevs.Get (1), enbDevs.Get (1));

  NS_LOG_INFO ("Configuring discovery applications");
  std::map<Ptr<NetDevice>, std::list<uint32_t> > announceApps;
  std::map<Ptr<NetDevice>, std::list<uint32_t> > monitorApps;
  for (uint32_t i = 1; i <= ueNodes.GetN (); ++i)
    {
      announceApps[ueDevs.Get (i - 1)].push_back (i);

      NS_LOG_DEBUG ("Ue Device " << i - 1 << " announce app = " << i);
      for (uint32_t j = 1; j <= ueNodes.GetN (); ++j)
        {
          if (i != j)
            {
              monitorApps[ueDevs.Get (i - 1)].push_back (j);
              NS_LOG_DEBUG ("Ue Device " << i - 1 << " monitor app = " << j);
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

      if (!useModelB)
        {
          Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartDiscoveryApps, proseHelper, ueNetDevice, apps, LteSlUeRrc::Announcing);
        }
      else
        {
          Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartDiscoveryApps, proseHelper, ueNetDevice, apps, LteSlUeRrc::Discoveree);
        }
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

      if (!useModelB)
        {
          Simulator::Schedule (Seconds (2.0),&LteSidelinkHelper::StartDiscoveryApps, proseHelper, ueNetDevice, apps, LteSlUeRrc::Monitoring);
        }
      else
        {
          Simulator::Schedule (Seconds (2.0),&LteSidelinkHelper::StartDiscoveryApps, proseHelper, ueNetDevice, apps, LteSlUeRrc::Discoverer);
        }
    }

  ///*** End of application configuration ***///

  //Tracing the number of received discovery messages by each UE
  DiscoveryMonitoringTracer tracer0;
  Ptr<LteUeRrc> ueRrc0 = ueDevs.Get (0)->GetObject<LteUeNetDevice> ()->GetRrc ();
  ueRrc0->TraceConnectWithoutContext ("DiscoveryMonitoring", MakeCallback (&DiscoveryMonitoringTracer::Trace,
                                                                           &tracer0));
  DiscoveryMonitoringTracer tracer1;
  Ptr<LteUeRrc> ueRrc1 = ueDevs.Get (1)->GetObject<LteUeNetDevice> ()->GetRrc ();
  ueRrc1->TraceConnectWithoutContext ("DiscoveryMonitoring", MakeCallback (&DiscoveryMonitoringTracer::Trace,
                                                                           &tracer1));

  NS_LOG_INFO ("Enabling Sidelink discovery reception trace...");

  lteHelper->EnableSlRxPhyTraces ();
  lteHelper->EnableSlPsdchMacTraces ();
  lteHelper->EnableDiscoveryMonitoringRrcTraces ();

  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (simTime);

  Simulator::Run ();

  if (test)
    {
      NS_LOG_INFO ("Verifying scenario...");

      //In this scenario, with simTime = Seconds (6), each UE should have
      //successfully received at least 10 discovery messages
      NS_LOG_INFO ("UE 0 received " << tracer0.m_nRxMsgs << " discovery messages");
      NS_LOG_INFO ("UE 1 received " << tracer1.m_nRxMsgs << " discovery messages");

      if (tracer0.m_nRxMsgs >= 10 &&  tracer1.m_nRxMsgs >= 10)
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
