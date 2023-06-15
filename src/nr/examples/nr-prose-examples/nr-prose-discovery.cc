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


/**
 * \ingroup examples
 * \file nr-prose-discovery.cc
 * \brief A simple example using ProSe Discovery, with a sidelink
 *        configuration based on cttc-nr-v2x-demo-simple.cc example
 *
 * Channel configuration:
 * This example setups an NR sidelink out-of-coverage simulation using the 3GPP
 * channel model from TR 37.885.
 *
 * System configuration:
 * Sidelink will use one operational band, containing one component carrier,
 * and a single bandwidth part centered at the frequency specified by the
 * corresponding input parameter. The system bandwidth, and the numerology to
 * be used and the transmission power can be setup as well.
 *
 *
 * Topology:
 * There are 2 UEs in the topology located with an inter-UE distance specified
 * by the corresponding input parameter. Default configuration:
 *          UE1..........(20 m)..........UE2
 *   (0.0, 0.0, 1.5)               (20, 0.0, 1.5)
 *
 */


#include "ns3/core-module.h"
#include "ns3/config-store.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/nr-module.h"
#include "ns3/lte-module.h"
#include "ns3/stats-module.h"
#include "ns3/config-store-module.h"
#include "ns3/log.h"
#include "ns3/antenna-module.h"
#include <iomanip>

//Dependency of these nr-v2x-examples classes for statistics
#include "../nr-v2x-examples/ue-mac-pscch-tx-output-stats.h"
#include "../nr-v2x-examples/ue-mac-pssch-tx-output-stats.h"
#include "../nr-v2x-examples/ue-phy-pscch-rx-output-stats.h"
#include "../nr-v2x-examples/ue-phy-pssch-rx-output-stats.h"
#include "../nr-v2x-examples/ue-to-ue-pkt-txrx-output-stats.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrProseDiscovery");


/*
 * Global methods and variable to hook trace sources from different layers of
 * the protocol stack.
 */

/**
 * \brief Method to listen to the trace SlPscchScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 1-A from UE MAC.
 *
 * \param pscchStats Pointer to the \link UeMacPscchTxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * \param pscchStatsParams Parameters of the trace source.
 */
void NotifySlPscchScheduling (UeMacPscchTxOutputStats *pscchStats, const SlPscchUeMacStatParameters pscchStatsParams)
{
  pscchStats->Save (pscchStatsParams);
}

/**
 * \brief Method to listen to the trace SlPsschScheduling of NrUeMac, which gets
 *        triggered upon the transmission of SCI format 2-A and data from UE MAC.
 *
 * \param psschStats Pointer to the \link UeMacPsschTxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * \param psschStatsParams Parameters of the trace source.
 */
void NotifySlPsschScheduling (UeMacPsschTxOutputStats *psschStats, const SlPsschUeMacStatParameters psschStatsParams)
{
  psschStats->Save (psschStatsParams);
}

/**
 * \brief Method to listen to the trace RxPscchTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 1-A.
 *
 * \param pscchStats Pointer to the \link UePhyPscchRxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * \param pscchStatsParams Parameters of the trace source.
 */
void NotifySlPscchRx (UePhyPscchRxOutputStats *pscchStats, const SlRxCtrlPacketTraceParams pscchStatsParams)
{
  pscchStats->Save (pscchStatsParams);
}

/**
 * \brief Method to listen to the trace RxPsschTraceUe of NrSpectrumPhy, which gets
 *        triggered upon the reception of SCI format 2-A and data.
 *
 * \param psschStats Pointer to the \link UePhyPsschRxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database.
 * \param psschStatsParams Parameters of the trace source.
 */
void NotifySlPsschRx (UePhyPsschRxOutputStats *psschStats, const SlRxDataPacketTraceParams psschStatsParams)
{
  psschStats->Save (psschStatsParams);
}


int
main (int argc, char *argv[])
{

  //Topology parameters
  uint16_t ueNum = 2; // Number of UEs in the simulation
  uint16_t interUeDistance = 20; //meters

  // Simulation timeline parameters
  Time simTime = Seconds (10.0); //Total simulation time
  Time startDiscTime = Seconds (2.0); //Time to start the Prose discovery procedure in seconds
  Time stopDiscTime = Seconds (50.0); //Time to stop the Prose discovery procedure in seconds
  Time discInterval = Seconds (2.0); // Interval between two discovery announcements


  // NR parameters
  uint16_t numerologyBwpSl = 2; //The numerology to be used in sidelink bandwidth part
  double centralFrequencyBandSl = 5.89e9; // band n47  TDD //Here band is analogous to channel
  uint16_t bandwidthBandSl = 400; //Multiple of 100 KHz; 400 = 40 MHz
  double txPower = 23; //dBm

  // Where we will store the output files.
  std::string simTag = "default";
  std::string outputDir = "./";


  CommandLine cmd;
  cmd.AddValue ("ueNum",
                "Number of UEs in the simulation",
                ueNum);
  cmd.AddValue ("interUeDistance",
                "The distance among the UEs in the topology",
                interUeDistance);
  cmd.AddValue ("simTime",
                "Simulation time in seconds",
                simTime);
  cmd.AddValue ("startDiscTime",
                "Time to start the Prose discovery procedure in seconds",
                startDiscTime);
  cmd.AddValue ("stopDiscTime",
                "Time to stop the Prose discovery procedure in seconds",
                stopDiscTime);
  cmd.AddValue ("discInterval",
                "Interval between two Prose discovery announcements in seconds",
                discInterval);
  cmd.AddValue ("numerologyBwpSl",
                "The numerology to be used in sidelink bandwidth part",
                numerologyBwpSl);
  cmd.AddValue ("centralFrequencyBandSl",
                "The central frequency to be used for sidelink band/channel",
                centralFrequencyBandSl);
  cmd.AddValue ("bandwidthBandSl",
                "The system bandwidth to be used for sidelink",
                bandwidthBandSl);
  cmd.AddValue ("txPower",
                "total tx power in dBm",
                txPower);
  cmd.AddValue ("simTag",
                "tag to be appended to output filenames to distinguish simulation campaigns",
                simTag);
  cmd.AddValue ("outputDir",
                "directory where to store simulation results",
                outputDir);


  // Parse the command line
  cmd.Parse (argc, argv);

  //ConfigStore
  ConfigStore config;
  config.ConfigureDefaults ();

  //Check if the frequency is in the allowed range.
  NS_ABORT_IF (centralFrequencyBandSl > 6e9);

  //Setup large enough buffer size to avoid overflow
  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (999999999));
  
  // Discovery Frequency
  Config::SetDefault ("ns3::NrSlUeProse::DiscoveryInterval",TimeValue (discInterval));

  //UE nodes creation
  NodeContainer ueVoiceContainer;
  ueVoiceContainer.Create (ueNum);

  //UE nodes mobility setup
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  Ptr<ListPositionAllocator> positionAllocUe = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < ueNum; i++)
    {
      positionAllocUe->Add (Vector (interUeDistance * i, 0.0, 1.5));
    }
  mobility.SetPositionAllocator (positionAllocUe);
  mobility.Install (ueVoiceContainer);

  /*
   * Setup the NR module. We create the various helpers needed for the
   * NR simulation:
   * - EpcHelper, which will setup the core network entities
   * - NrHelper, which takes care of creating and connecting the various
   * part of the NR stack
   */
  Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper> ();
  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();

  // Put the pointers inside nrHelper
  nrHelper->SetEpcHelper (epcHelper);

  /*
   * Spectrum division. We create one operational band, containing
   * one component carrier, and a single bandwidth part
   * centered at the frequency specified by the input parameters.
   */
  BandwidthPartInfoPtrVector allBwps;
  CcBwpCreator ccBwpCreator;
  const uint8_t numCcPerBand = 1;

  //Create the configuration for the CcBwpHelper. SimpleOperationBandConfcreates a single BWP per CC
  CcBwpCreator::SimpleOperationBandConf bandConfSl (centralFrequencyBandSl, bandwidthBandSl, numCcPerBand, BandwidthPartInfo::V2V_Highway);

  // By using the configuration created, it is time to make the operation bands
  OperationBandInfo bandSl = ccBwpCreator.CreateOperationBandContiguousCc (bandConfSl);

  //Configure 3GPP channel model
  Config::SetDefault ("ns3::ThreeGppChannelModel::UpdatePeriod",TimeValue (MilliSeconds (100)));
  nrHelper->SetChannelConditionModelAttribute ("UpdatePeriod", TimeValue (MilliSeconds (0)));
  nrHelper->SetPathlossAttribute ("ShadowingEnabled", BooleanValue (false));

  /*
   * Initialize channel and pathloss, plus other things inside bandSl. If needed,
   * the band configuration can be done manually, but we leave it for more
   * sophisticated examples. For the moment, this method will take care
   * of all the spectrum initialization needs.
   */
  nrHelper->InitializeOperationBand (&bandSl);
  allBwps = CcBwpCreator::GetAllBwps ({bandSl});


  Packet::EnableChecking ();
  Packet::EnablePrinting ();


  // Core latency
  epcHelper->SetAttribute ("S1uLinkDelay", TimeValue (MilliSeconds (0)));

  /*
   * Antennas for all the UEs
   * We are not using beamforming in SL, rather we are using
   * quasi-omnidirectional transmission and reception, which is the default
   * configuration of the beams.
   */
  nrHelper->SetUeAntennaAttribute ("NumRows", UintegerValue (1));
  nrHelper->SetUeAntennaAttribute ("NumColumns", UintegerValue (2));
  nrHelper->SetUeAntennaAttribute ("AntennaElement", PointerValue (CreateObject<IsotropicAntennaModel> ()));

  nrHelper->SetUePhyAttribute ("TxPower", DoubleValue (txPower));

  //NR Sidelink attribute of UE MAC, which are would be common for all the UEs
  nrHelper->SetUeMacAttribute ("EnableSensing", BooleanValue (false));
  nrHelper->SetUeMacAttribute ("T1", UintegerValue (2));
  nrHelper->SetUeMacAttribute ("T2", UintegerValue (33));
  nrHelper->SetUeMacAttribute ("ActivePoolId", UintegerValue (0));
  nrHelper->SetUeMacAttribute ("ReservationPeriod", TimeValue (MilliSeconds (10)));
  nrHelper->SetUeMacAttribute ("NumSidelinkProcess", UintegerValue (4));
  nrHelper->SetUeMacAttribute ("EnableBlindReTx", BooleanValue (true));

  uint8_t bwpIdForGbrMcptt = 0;

  nrHelper->SetBwpManagerTypeId (TypeId::LookupByName ("ns3::NrSlBwpManagerUe"));
  //following parameter has no impact at the moment because:
  //1. No support for PQI based mapping between the application and the LCs
  //2. No scheduler to consider PQI
  //However, till such time all the NR SL examples should use GBR_MC_PUSH_TO_TALK
  //because we hard coded the PQI 65 in UE RRC.
  nrHelper->SetUeBwpManagerAlgorithmAttribute ("GBR_MC_PUSH_TO_TALK", UintegerValue (bwpIdForGbrMcptt));

  std::set<uint8_t> bwpIdContainer;
  bwpIdContainer.insert (bwpIdForGbrMcptt);

  NetDeviceContainer ueVoiceNetDev = nrHelper->InstallUeDevice (ueVoiceContainer, allBwps);

  // When all the configuration is done, explicitly call UpdateConfig ()
  for (auto it = ueVoiceNetDev.Begin (); it != ueVoiceNetDev.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }


  /*Create NrSlHelper which will configure the UEs protocol stack to be ready to
   *perform Sidelink related procedures
   */
  Ptr<NrSlHelper> nrSlHelper = CreateObject <NrSlHelper> ();
  // Put the pointers inside NrSlHelper
  nrSlHelper->SetEpcHelper (epcHelper);

  /*
   * Set the SL error model and AMC
   * Error model type: ns3::NrEesmCcT1, ns3::NrEesmCcT2, ns3::NrEesmIrT1,
   *                   ns3::NrEesmIrT2, ns3::NrLteMiErrorModel
   * AMC type: NrAmc::ShannonModel or NrAmc::ErrorModel
   */
  std::string errorModel = "ns3::NrEesmIrT1";
  nrSlHelper->SetSlErrorModel (errorModel);
  nrSlHelper->SetUeSlAmcAttribute ("AmcModel", EnumValue (NrAmc::ErrorModel));

  /*
   * Set the SL scheduler attributes
   * In this example we use NrSlUeMacSchedulerSimple scheduler, which uses
   * fix MCS value and schedules logical channels by priority order first and
   * then by creation order
   */
  nrSlHelper->SetNrSlSchedulerTypeId (NrSlUeMacSchedulerSimple::GetTypeId ());
  nrSlHelper->SetUeSlSchedulerAttribute ("FixNrSlMcs", BooleanValue (true));
  nrSlHelper->SetUeSlSchedulerAttribute ("InitialNrSlMcs", UintegerValue (14));

  /*
   * Very important method to configure UE protocol stack, i.e., it would
   * configure all the SAPs among the layers, setup callbacks, configure
   * error model, configure AMC, and configure ChunkProcessor in Interference
   * API.
   */
  nrSlHelper->PrepareUeForSidelink (ueVoiceNetDev, bwpIdContainer);


  /*
   * Start preparing for all the sub Structs/RRC Information Element (IEs)
   * of LteRrcSap::SidelinkPreconfigNr. This is the main structure, which would
   * hold all the pre-configuration related to Sidelink.
   */

  //SlResourcePoolNr IE
  LteRrcSap::SlResourcePoolNr slResourcePoolNr;
  //get it from pool factory
  Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory> ();
  //Configure specific parameters of interest:
  std::vector <std::bitset<1> > slBitmap = {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1};
  ptrFactory->SetSlTimeResources (slBitmap);
  ptrFactory->SetSlSensingWindow (100); // T0 in ms
  ptrFactory->SetSlSelectionWindow (5);
  ptrFactory->SetSlFreqResourcePscch (10); // PSCCH RBs
  ptrFactory->SetSlSubchannelSize (10);
  ptrFactory->SetSlMaxNumPerReserve (3);
  //Once parameters are configured, we can create the pool
  LteRrcSap::SlResourcePoolNr pool = ptrFactory->CreatePool ();
  slResourcePoolNr = pool;

  //Configure the SlResourcePoolConfigNr IE, which holds a pool and its id
  LteRrcSap::SlResourcePoolConfigNr slresoPoolConfigNr;
  slresoPoolConfigNr.haveSlResourcePoolConfigNr = true;
  //Pool id, ranges from 0 to 15
  uint16_t poolId = 0;
  LteRrcSap::SlResourcePoolIdNr slResourcePoolIdNr;
  slResourcePoolIdNr.id = poolId;
  slresoPoolConfigNr.slResourcePoolId = slResourcePoolIdNr;
  slresoPoolConfigNr.slResourcePool = slResourcePoolNr;

  //Configure the SlBwpPoolConfigCommonNr IE, which holds an array of pools
  LteRrcSap::SlBwpPoolConfigCommonNr slBwpPoolConfigCommonNr;
  //Array for pools, we insert the pool in the array as per its poolId
  slBwpPoolConfigCommonNr.slTxPoolSelectedNormal [slResourcePoolIdNr.id] = slresoPoolConfigNr;

  //Configure the BWP IE
  LteRrcSap::Bwp bwp;
  bwp.numerology = numerologyBwpSl;
  bwp.symbolsPerSlots = 14;
  bwp.rbPerRbg = 1;
  bwp.bandwidth = bandwidthBandSl;

  //Configure the SlBwpGeneric IE
  LteRrcSap::SlBwpGeneric slBwpGeneric;
  slBwpGeneric.bwp = bwp;
  slBwpGeneric.slLengthSymbols = LteRrcSap::GetSlLengthSymbolsEnum (14);
  slBwpGeneric.slStartSymbol = LteRrcSap::GetSlStartSymbolEnum (0);

  //Configure the SlBwpConfigCommonNr IE
  LteRrcSap::SlBwpConfigCommonNr slBwpConfigCommonNr;
  slBwpConfigCommonNr.haveSlBwpGeneric = true;
  slBwpConfigCommonNr.slBwpGeneric = slBwpGeneric;
  slBwpConfigCommonNr.haveSlBwpPoolConfigCommonNr = true;
  slBwpConfigCommonNr.slBwpPoolConfigCommonNr = slBwpPoolConfigCommonNr;

  //Configure the SlFreqConfigCommonNr IE, which holds the array to store
  //the configuration of all Sidelink BWP (s).
  LteRrcSap::SlFreqConfigCommonNr slFreConfigCommonNr;
  //Array for BWPs. Here we will iterate over the BWPs, which
  //we want to use for SL.
  for (const auto &it:bwpIdContainer)
    {
      // it is the BWP id
      slFreConfigCommonNr.slBwpList [it] = slBwpConfigCommonNr;
    }

  //Configure the TddUlDlConfigCommon IE
  LteRrcSap::TddUlDlConfigCommon tddUlDlConfigCommon;
  tddUlDlConfigCommon.tddPattern = "DL|DL|DL|F|UL|UL|UL|UL|UL|UL|";

  //Configure the SlPreconfigGeneralNr IE
  LteRrcSap::SlPreconfigGeneralNr slPreconfigGeneralNr;
  slPreconfigGeneralNr.slTddConfig = tddUlDlConfigCommon;

  //Configure the SlUeSelectedConfig IE
  LteRrcSap::SlUeSelectedConfig slUeSelectedPreConfig;
  slUeSelectedPreConfig.slProbResourceKeep = 0;
  //Configure the SlPsschTxParameters IE
  LteRrcSap::SlPsschTxParameters psschParams;
  psschParams.slMaxTxTransNumPssch = 5;
  //Configure the SlPsschTxConfigList IE
  LteRrcSap::SlPsschTxConfigList pscchTxConfigList;
  pscchTxConfigList.slPsschTxParameters [0] = psschParams;
  slUeSelectedPreConfig.slPsschTxConfigList = pscchTxConfigList;

  /*
   * Finally, configure the SidelinkPreconfigNr This is the main structure
   * that needs to be communicated to NrSlUeRrc class
   */
  LteRrcSap::SidelinkPreconfigNr slPreConfigNr;
  slPreConfigNr.slPreconfigGeneral = slPreconfigGeneralNr;
  slPreConfigNr.slUeSelectedPreConfig = slUeSelectedPreConfig;
  slPreConfigNr.slPreconfigFreqInfoList [0] = slFreConfigCommonNr;

  //Communicate the above pre-configuration to the NrSlHelper
  nrSlHelper->InstallNrSlPreConfiguration (ueVoiceNetDev, slPreConfigNr);

  /****************************** End SL Configuration ***********************/

  /*
   * Fix the random streams
   */
  int64_t stream = 1;
  stream += nrHelper->AssignStreams (ueVoiceNetDev, stream);
  stream += nrSlHelper->AssignStreams (ueVoiceNetDev, stream);

  /*
   * Configure the IPv4 stack
   */
  InternetStackHelper internet;
  internet.Install (ueVoiceContainer);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (ueVoiceNetDev);

  // set the default gateway for the UE
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  for (uint32_t u = 0; u < ueVoiceContainer.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueVoiceContainer.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }
  
  /*
   * Configure ProSe
   */

  //Create ProSe helper
  Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject <NrSlProseHelper> ();
  // Install ProSe layer and corresponding SAPs in the UEs
  nrSlProseHelper->PrepareUesForProse (ueVoiceNetDev);

  /*
   * Setup discovery applications 
   */
  NS_LOG_INFO ("Configuring discovery applications"); 

  
  // Application Codes to announce/monitor
  std::map<Ptr<NetDevice>, std::list<uint32_t> > announcePayloads; 
  std::map<Ptr<NetDevice>, std::list<uint32_t> > monitorPayloads;

  // Destination L2 IDs for each application code
  std::map<Ptr<NetDevice>, std::list<uint32_t> > announceDstL2IdsMap;
  std::map<Ptr<NetDevice>, std::list<uint32_t> > monitorDstL2IdsMap;

  for (uint32_t i = 1; i <= ueVoiceNetDev.GetN (); ++i)
    {
      //For each UE, announce one appCode and monitor all the others appCode
      announcePayloads[ueVoiceNetDev.Get (i - 1)].push_back (i);
      announceDstL2IdsMap[ueVoiceNetDev.Get (i - 1)].push_back (100*i);

      for (uint32_t j = 1; j <= ueVoiceNetDev.GetN (); ++j)
        {
          if (i != j)
            {
              monitorPayloads[ueVoiceNetDev.Get (i - 1)].push_back (j);
              monitorDstL2IdsMap[ueVoiceNetDev.Get (i - 1)].push_back (100*j);
            }
        }
    }

  for (uint32_t i = 0; i < ueVoiceNetDev.GetN (); ++i)
    {
      //Start announcing/monitoring
      Simulator::Schedule (startDiscTime, &NrSlProseHelper::StartDiscovery, nrSlProseHelper, ueVoiceNetDev.Get (i), 
          announcePayloads[ueVoiceNetDev.Get (i)], announceDstL2IdsMap[ueVoiceNetDev.Get (i)], NrSlUeProse::Announcing);
      Simulator::Schedule (startDiscTime, &NrSlProseHelper::StartDiscovery, nrSlProseHelper, ueVoiceNetDev.Get (i), 
          monitorPayloads[ueVoiceNetDev.Get (i)], monitorDstL2IdsMap[ueVoiceNetDev.Get (i)], NrSlUeProse::Monitoring);

      //Stop announcing
      Simulator::Schedule (stopDiscTime,&NrSlProseHelper::StopDiscovery, nrSlProseHelper, ueVoiceNetDev.Get (i), 
                            announcePayloads[ueVoiceNetDev.Get (i)], NrSlUeProse::Announcing);
      //Stop monitoring
      Simulator::Schedule (stopDiscTime,&NrSlProseHelper::StopDiscovery, nrSlProseHelper, ueVoiceNetDev.Get (i), 
                            monitorPayloads[ueVoiceNetDev.Get (i)], NrSlUeProse::Monitoring);
    }
 
  /*********************** End ProSe configuration ***************************/

  //Datebase setup
  std::string exampleName = simTag + "-" + "nr-prose-discovery";
  SQLiteOutput db (outputDir + exampleName + ".db");

  UeMacPscchTxOutputStats pscchStats;
  pscchStats.SetDb (&db, "pscchTxUeMac");
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/NrUeMac/SlPscchScheduling",
                                 MakeBoundCallback (&NotifySlPscchScheduling, &pscchStats));

  UeMacPsschTxOutputStats psschStats;
  psschStats.SetDb (&db, "psschTxUeMac");
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/NrUeMac/SlPsschScheduling",
                                 MakeBoundCallback (&NotifySlPsschScheduling, &psschStats));


  UePhyPscchRxOutputStats pscchPhyStats;
  pscchPhyStats.SetDb (&db, "pscchRxUePhy");
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/NrUePhy/NrSpectrumPhyList/*/RxPscchTraceUe",
                                 MakeBoundCallback (&NotifySlPscchRx, &pscchPhyStats));

  UePhyPsschRxOutputStats psschPhyStats;
  psschPhyStats.SetDb (&db, "psschRxUePhy");
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/NrUePhy/NrSpectrumPhyList/*/RxPsschTraceUe",
                                 MakeBoundCallback (&NotifySlPsschRx, &psschPhyStats));

  UeToUePktTxRxOutputStats pktStats;
  pktStats.SetDb (&db, "pktTxRx");

  //Enable discovery traces
  nrSlProseHelper->EnableDiscoveryTraces ();
  
  //ConfigStore
  config.ConfigureAttributes ();
  
  Simulator::Stop (simTime);
  Simulator::Run ();

  /*
   * VERY IMPORTANT: Do not forget to empty the database cache, which would
   * dump the data store towards the end of the simulation in to a database.
   */
  pktStats.EmptyCache ();
  pscchStats.EmptyCache ();
  psschStats.EmptyCache ();
  pscchPhyStats.EmptyCache ();
  psschPhyStats.EmptyCache ();



  Simulator::Destroy ();
  return 0;
}


