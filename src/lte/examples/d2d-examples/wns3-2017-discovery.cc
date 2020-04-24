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

NS_LOG_COMPONENT_DEFINE ("wns3-2017-discovery");

int
main (int argc, char *argv[])
{
  // Initialize some values
  double simTime = 10;
  uint32_t nbUes = 10;
  uint16_t txProb = 100;
  bool useRecovery = false;
  bool  enableNsLogs = false; // If enabled will output NS LOGs

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue ("simTime", "Simulation time", simTime);
  cmd.AddValue ("numUe", "Number of UEs", nbUes);
  cmd.AddValue ("txProb", "initial transmission probability", txProb);
  cmd.AddValue ("enableRecovery", "error model and HARQ for D2D Discovery", useRecovery);
  cmd.AddValue ("enableNsLogs", "Enable NS logs", enableNsLogs);

  cmd.Parse (argc, argv);

  if (enableNsLogs)
    {
      LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_ALL);
      LogComponentEnable ("wns3-2017-discovery", logLevel);

      LogComponentEnable ("LteSpectrumPhy", logLevel);
      LogComponentEnable ("LteUePhy", logLevel);
      LogComponentEnable ("LteUeRrc", logLevel);
      LogComponentEnable ("LteEnbPhy", logLevel);
      LogComponentEnable ("LteUeMac", logLevel);
      LogComponentEnable ("LteSlUeRrc", logLevel);
      LogComponentEnable ("LteSidelinkHelper", logLevel);
      LogComponentEnable ("LteHelper", logLevel);
    }

  // Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
  // Use error model and HARQ for D2D Discovery (recovery process)
  Config::SetDefault ("ns3::LteSpectrumPhy::SlDiscoveryErrorModelEnabled", BooleanValue (useRecovery));
  Config::SetDefault ("ns3::LteSpectrumPhy::DropRbOnCollisionEnabled", BooleanValue (true));

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  NS_LOG_INFO ("Creating helpers...");
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  Ptr<LteSidelinkHelper> sidelinkHelper = CreateObject<LteSidelinkHelper> ();
  sidelinkHelper->SetLteHelper (lteHelper);

  // Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::FriisPropagationLossModel"));

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

  NS_LOG_INFO ("Deploying UE's...");
  NodeContainer ues;
  ues.Create (nbUes);

  //Position of the nodes
  Ptr<ListPositionAllocator> positionAllocUe = CreateObject<ListPositionAllocator> ();

  for (uint32_t u = 0; u < ues.GetN (); ++u)
    {
      Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable> ();
      double x = rand->GetValue (-100,100);
      double y = rand->GetValue (-100,100);
      double z = 1.5;
      positionAllocUe->Add (Vector (x, y, z));
    }

  // Install mobility

  MobilityHelper mobilityUe;
  mobilityUe.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityUe.SetPositionAllocator (positionAllocUe);
  mobilityUe.Install (ues);

  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ues);

  //Fix the random number stream
  uint16_t randomStream = 1;
  randomStream += lteHelper->AssignStreams (ueDevs, randomStream);

  AsciiTraceHelper asc;
  Ptr<OutputStreamWrapper> st = asc.CreateFileStream ("discovery_nodes.txt");
  *st->GetStream () << "id\tx\ty" << std::endl;

  for (uint32_t i = 0; i < ueDevs.GetN (); ++i)
    {
      Ptr<LteUeNetDevice> ueNetDevice = DynamicCast<LteUeNetDevice> (ueDevs.Get (i));
      uint64_t imsi = ueNetDevice->GetImsi ();
      Vector pos = ues.Get (i)->GetObject<MobilityModel> ()->GetPosition ();
      std::cout << "UE " << i << " id = " << ues.Get (i)->GetId () << " / imsi = " << imsi << " / position = [" << pos.x << "," << pos.y << "," << pos.z << "]" << std::endl;
      *st->GetStream () << imsi << "\t" << pos.x << "\t" << pos.y << std::endl;
    }

  NS_LOG_INFO ("Configuring discovery pool for the UEs...");
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetDiscEnabled (true);

  //todo: specify parameters before installing in UEs if needed
  LteRrcSap::SlPreconfiguration preconfiguration;

  preconfiguration.preconfigGeneral.carrierFreq = 23330;
  preconfiguration.preconfigGeneral.slBandwidth = 50;
  preconfiguration.preconfigDisc.nbPools = 1;
  preconfiguration.preconfigDisc.pools[0].cpLen.cplen = LteRrcSap::SlCpLen::NORMAL;
  preconfiguration.preconfigDisc.pools[0].discPeriod.period = LteRrcSap::SlPeriodDisc::rf32;
  preconfiguration.preconfigDisc.pools[0].numRetx = 0;
  preconfiguration.preconfigDisc.pools[0].numRepetition = 1;
  preconfiguration.preconfigDisc.pools[0].tfResourceConfig.prbNum = 10;
  preconfiguration.preconfigDisc.pools[0].tfResourceConfig.prbStart = 10;
  preconfiguration.preconfigDisc.pools[0].tfResourceConfig.prbEnd = 49;
  preconfiguration.preconfigDisc.pools[0].tfResourceConfig.offsetIndicator.offset = 0;
  preconfiguration.preconfigDisc.pools[0].tfResourceConfig.subframeBitmap.bitmap = std::bitset<40> (0x11111);
  preconfiguration.preconfigDisc.pools[0].txParameters.txParametersGeneral.alpha = LteRrcSap::SlTxParameters::al09;
  preconfiguration.preconfigDisc.pools[0].txParameters.txParametersGeneral.p0 = -40;
  preconfiguration.preconfigDisc.pools[0].txParameters.txProbability = SidelinkDiscResourcePool::TxProbabilityFromInt (txProb);

  NS_LOG_INFO ("Install Sidelink discovery configuration in the UEs...");
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfiguration);
  lteHelper->InstallSidelinkConfiguration (ueDevs, ueSidelinkConfiguration);

  NS_LOG_INFO ("Configuring discovery applications");
  std::map<Ptr<NetDevice>, std::list<uint32_t> > announcePayloads; 
  std::map<Ptr<NetDevice>, std::list<uint32_t> > monitorPayloads; 

  for (uint32_t i = 1; i <= nbUes; ++i)
    {
      announcePayloads[ueDevs.Get (i - 1)].push_back (i);

      for (uint32_t j = 1; j <= nbUes; ++j)
        {
          if (i != j)
            {
              monitorPayloads[ueDevs.Get (i - 1)].push_back (j);
            }
        }
    }

  for (uint32_t i = 0; i < nbUes; i++)
    {
      Simulator::Schedule (Seconds (2.0), &LteSidelinkHelper::StartDiscoveryApps, sidelinkHelper, ueDevs.Get (i), announcePayloads[ueDevs.Get (i)], LteSlUeRrc::Announcing);
      Simulator::Schedule (Seconds (2.0),&LteSidelinkHelper::StartDiscoveryApps, sidelinkHelper, ueDevs.Get (i), monitorPayloads[ueDevs.Get (i)], LteSlUeRrc::Monitoring);
    }

  ///*** End of application configuration ***///

  // Set Discovery Traces
  lteHelper->EnableSlRxPhyTraces ();
  lteHelper->EnableSlPsdchMacTraces ();
  lteHelper->EnableDiscoveryMonitoringRrcTraces ();
  
  NS_LOG_INFO ("Starting simulation...");
  Simulator::Stop (Seconds (simTime));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;

}
