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
 * \file nr-sl-simple-multi-lc.cc
 *
 * This example is based on cttc-nr-v2x-demo-simple and simulates a simple
 * topology consisting of 2 out-of-coverage UEs, where UE-1 transmits, and UE-2
 * receives.
 * The purpose of this scenario is to show how different sidelink (SL) traffic
 * profiles can be configured in the simulation and associated to different
 * traffic flows. The scheduler then will take into consideration those traffic
 * profiles and grant resources accordingly during the simulation.
 *
 * If interested in learning how to configure the other aspects of a NR SL
 * simulation please refer to cttc-nr-v2x-demo-simple for a nice tutorial.
 *
 * Traffic generation:
 * UE-1 has three traffic flows directed towards UE-2. Each traffic flow is
 * implemented by an OnOffApplication with constant rate that generates CBR
 * traffic for the duration of the 'trafficTime' simulation parameter. The
 * packet size and the data rate can be adjusted with the simulation
 * parameters 'udpPacketSize' and 'dataRate', respectively.
 * Please note that the traffic profiles described below were selected to
 * support the default traffic configuration of 200 bytes packets and a data
 * rate of 16 kbps. Changing those values without adjusting the traffic profile
 * accordingly may result in poor performance.
 * All traffic flows are sent to the multicast address, but each of them to a
 * different port. UE-2 listens to that address and those ports and consumes
 * the packets with a PacketSink application
 *
 *
 * Traffic profiles:
 * Three different traffic profiles are created, one for each traffic flow.
 * The structure SidelinkInfo contains the traffic profile parameters to be
 * propagated in the stack. A SidelinkInfo is created for each traffic flow.
 * A Traffic Flow Template (TFT) is created for each traffic flow associating
 * the corresponding SidelinkInfo, multicast address and port. Three SL data
 * radio bearer are created, each configured with one of the above described
 * TFTs, which will result in the creation of a logical channel per traffic
 * flow on the stack. The scheduler (NrSlUeMacSchedulerDefault) will grant
 * resources for each flow according to the traffic profiles.
 *
 * In this scenario we configure four parameters of the traffic profiles:
 *  - m_dynamic: flag for whether the logical channel scheduling is dynamic
 *               scheduling (i.e., per PDU) or Semi-persistent scheduling (SPS)
 *  - m_dstL2Id: L2 id to which the traffic will be sent
 *  - m_priority: priority (the higher the value, the higher the priority of
 *                the logical channel)
 *  - m_rri: Resource Reservation Interval for SPS scheduling
 *
 *  We created different configurations for the three traffic profiles. The
 *  table below shows the simulation parameters that control which
 *  configuration is used and the resulting values for the traffic profile
 *  parameters of each traffic flow.
 *
 *  -------------------------|-----------------------------
 * |      Simulation         | Resulting traffic profile   |
 * |    configuration        | configuration per flow      |
 * |-------------------------|-----------------------------|
 * | Parameter       | Value | Flow 1  | Flow 2  | Flow 3  |
 * |-----------------|-------|---------|---------|---------|
 * | schedTypeConfig |   1   | Dynamic | Dynamic | Dynamic |
 * |-----------------|-------|---------|---------|---------|
 * | schedTypeConfig |   2   |   SPS   |   SPS   |   SPS   |
 * |-----------------|-------|---------|---------|---------|
 * | schedTypeConfig |   3   | Dynamic | Dynamic |   SPS   |
 * |-----------------|-------|---------|---------|---------|
 * | schedTypeConfig |   4   |   SPS   |   SPS   | Dynamic |
 * |-----------------|-------|---------|---------|---------|
 * | dstL2IdConfig   |   1   |   255   |   255   |   255   |
 * |-----------------|-------|---------|---------|---------|
 * | dstL2IdConfig   |   2   |   255   |   254   |   255   |
 * |-----------------|-------|---------|---------|---------|
 * | dstL2IdConfig   |   3   |   254   |   254   |   255   |
 * |-----------------|-------|---------|---------|---------|
 * | priorityConfig  |   1   |    1    |    1    |    1    |
 * |-----------------|-------|---------|---------|---------|
 * | priorityConfig  |   2   |    1    |    2    |    3    |
 * |-----------------|-------|---------|---------|---------|
 * | priorityConfig  |   3   |    2    |    2    |    1    |
 * |-----------------|-------|---------|---------|---------|
 * | priorityConfig  |   4   |    1    |    1    |    2    |
 * |-----------------|-------|---------|---------|---------|
 * |-----------------|-------|---------|---------|---------|
 * | rriConfig       |   1   |   100   |   100   |   100   |
 * |-----------------|-------|---------|---------|---------|
 * | rriConfig       |   2   |   100   |    50   |   100   |
 *  -----------------|-------|---------|---------|---------
 *
 *  The other parameter related to the scheduling that we configure is
 *  'prioToSps' which sets the attribute 'PriorityToSps' in the scheduler and
 *  it is flag to give scheduling priority to logical channels that are
 *  configured with SPS scheduling in case of priority tie.
 *
 * The default configuration is:
 * --schedTypeConfig=1 (i.e., all LCs will have dynamic (per-packet) scheduling)
 * --dstL2IdConfig=1   (i.e., all LCs send to the same L2 ID of value 255)
 * --priorityConfig=1  (i.e., all LCs have the same priority of value 1)
 * --rriConfig=1       (i.e., all LCs have the same RRI, which doesn't matter in
 *                      this configuration as schedTypeConfig=1)
 * --prioToSps=false   (i.e., no priority to SPS, which doesn't matter in this
 *                      configuration as schedTypeConfig=1)
 *
 * The example will print on-screen the number of transmitted and received
 * packets during the simulation and the average packet delay.
 *
 * \code{.unparsed}
$ ./ns3 run "nr-sl-simple-multi-lc --help"
    \endcode
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


using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("NrSlSimpleMultiLc");


uint32_t g_rxPktCounter = 0; //!< Global variable to count RX packets
uint32_t g_txPktCounter = 0; //!< Global variable to count TX packets
std::list <double> g_delays; //!< Global list to store packet delays upon RX

/*
 * Structure to keep track of the transmission time of the packets at the
 * application layer. Used to calculate packet delay.
 */
struct PacketWithRxTimestamp
{
  Ptr<const Packet> p;
  Time txTimestamp;
};

/*
 * Map to store received packets and reception timestamps at the application
 * layer. Used to calculate packet delay at the application layer.
 */
std::map<std::string, PacketWithRxTimestamp> g_rxPacketsForDelayCalc;

/*
 * \brief Trace sink function to count and logging the transmitted data packets
 *        and their corresponding transmission timestamp at the application layer
 *
 * \param p the packet
 * \param srcAddrs the source IP address in the packet
 * \param dstAddrs the destination IP address in the packet
 * \param seqTsSizeHeader the header containing the sequence number of the packet
 */
void
TxPacketTraceForDelay (Ptr<const Packet> p, const Address &srcAddrs,
                       const Address &dstAddrs, const SeqTsSizeHeader &seqTsSizeHeader)
{
  g_txPktCounter++;
  std::ostringstream  oss;
  oss << InetSocketAddress::ConvertFrom (dstAddrs).GetPort()
      << "("
      << seqTsSizeHeader.GetSeq ()
      << ")";
  std::string mapKey = oss.str ();
  PacketWithRxTimestamp mapValue;
  mapValue.p = p;
  mapValue.txTimestamp = Simulator::Now ();
  g_rxPacketsForDelayCalc.insert (std::pair < std::string, PacketWithRxTimestamp> (mapKey, mapValue));
  NS_LOG_DEBUG (" TX: " << mapKey);


}

/*
 * \brief Trace sink function to count and calculate the delay upon reception
 *        of a packet at the application layer
 *
 * \param p the packet
 * \param srcAddrs the source IP address in the packet
 * \param dstAddrs the destination IP address in the packet
 * \param seqTsSizeHeader the header containing the sequence number of the packet
 */
void
RxPacketTraceForDelay (Ptr<const Packet> p, const Address &srcAddrs,
                       const Address &dstAddrs, const SeqTsSizeHeader &seqTsSizeHeader)
{
  g_rxPktCounter++;

  double delay = 0.0;
  std::ostringstream  oss;
  oss << InetSocketAddress::ConvertFrom (dstAddrs).GetPort ()
      << "("
      << seqTsSizeHeader.GetSeq ()
      << ")";
  std::string mapKey = oss.str ();

  auto it = g_rxPacketsForDelayCalc.find (mapKey);
  if (it == g_rxPacketsForDelayCalc.end ())
    {
      NS_FATAL_ERROR ("Rx packet not found?!");
    }
  else
    {
      delay = Simulator::Now ().GetSeconds () * 1000.0 - it->second.txTimestamp.GetSeconds () * 1000.0;
      g_delays.push_back (delay);
      g_rxPacketsForDelayCalc.erase (mapKey);
    }
    NS_LOG_DEBUG (" RX: " << mapKey << delay);
}

int 
main (int argc, char *argv[])
{
  // Scenario parameters
  uint16_t interUeDistance = 20; //meters

  // Traffic parameters
  uint32_t udpPacketSize = 200;
  double dataRate = 16; //16 kilobits per second

  //Traffic profile parameters
  uint16_t schedTypeConfig = 1;
  uint16_t dstL2IdConfig = 1;
  uint16_t priorityConfig = 1;
  uint16_t rriConfig = 1;
  bool prioToSps = false;

  // Simulation parameters.
  Time trafficTime = Seconds (2.0);

  //Testing flag
  bool testing = false;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("trafficTime",
                "The time traffic will be active in seconds",
                trafficTime);
  cmd.AddValue ("interUeDistance",
                "The distance among the UEs in the topology",
                interUeDistance);
  cmd.AddValue ("packetSize",
                "packet size in bytes to be used by best effort traffic",
                udpPacketSize);
  cmd.AddValue ("dataRate",
                "The data rate in kilobits per second for best effort traffic",
                dataRate);
  cmd.AddValue ("schedTypeConfig",
                "The scheduling configuration for the three traffic profiles",
                schedTypeConfig);
  cmd.AddValue ("dstL2IdConfig",
                "The dstL2Id configuration for the three traffic profiles",
                dstL2IdConfig);
  cmd.AddValue ("priorityConfig",
                "The priority configuration for the three traffic profiles",
                priorityConfig);
  cmd.AddValue ("rriConfig",
                "The RRI configuration for the three traffic profiles",
                rriConfig);
  cmd.AddValue ("prioToSps",
                 "Give scheduling priority to SPS logical channels in case of a tie (if set to True)",
                 prioToSps);
  cmd.AddValue ("testing",
                 "Testing flag to do verification that the example is working as expected (if set to True)",
                 testing);

  // Parse the command line
  cmd.Parse (argc, argv);

  // NR parameters
  uint16_t numerologyBwpSl = 2;
  double centralFrequencyBandSl = 5.89e9; // band n47  TDD //Here band is analogous to channel
  uint16_t bandwidthBandSl = 400; //Multiple of 100 KHz; 400 = 40 MHz
  double txPower = 23; //dBm

  // Final simulation time
  Time slBearersActivationTime = Seconds (2.0);
  Time finalSlBearersActivationTime = slBearersActivationTime;
  Time finalSimTime = trafficTime + finalSlBearersActivationTime + Seconds (0.05); //Give some extra simulation time for packets to be Rx

  /*
   * Default values for the simulation. We are progressively removing all
   * the instances of SetDefault, but we need it for legacy code (LTE)
   */
  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue(999999999));


  //Create UE nodes
  NodeContainer ueNodeContainer;
  uint16_t ueNum = 2;
  ueNodeContainer.Create (ueNum);

  //Assign position and mobility to the UEs
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  Ptr<ListPositionAllocator> positionAllocUe = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < ueNum; i++)
    {
      positionAllocUe->Add (Vector (interUeDistance * i, 0.0, 1.5));
    }
  mobility.SetPositionAllocator (positionAllocUe);
  mobility.Install (ueNodeContainer);

  /* The default topology is the following:
   *
   *         UE1..........(20 m)..........UE2
   *   (0.0, 0.0, 1.5)               (20, 0.0, 1.5)
   */
  Packet::EnableChecking ();
  Packet::EnablePrinting ();


  //Configure NR module
  Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper> ();
  epcHelper->SetAttribute ("S1uLinkDelay", TimeValue (MilliSeconds (0)));
  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  nrHelper->SetEpcHelper (epcHelper);

  /*
   * Spectrum division. We create one operational band, containing
   * one component carrier, and a single bandwidth part
   * centered at the frequency specified by centralFrequencyBandSl.
   * We will use the StreetCanyon channel modeling.
   */
  BandwidthPartInfoPtrVector allBwps;
  CcBwpCreator ccBwpCreator;
  const uint8_t numCcPerBand = 1;

  CcBwpCreator::SimpleOperationBandConf bandConfSl (centralFrequencyBandSl, bandwidthBandSl, numCcPerBand, BandwidthPartInfo::V2V_Highway);
  OperationBandInfo bandSl = ccBwpCreator.CreateOperationBandContiguousCc (bandConfSl);

  Config::SetDefault ("ns3::ThreeGppChannelModel::UpdatePeriod",TimeValue (MilliSeconds(100)));
  nrHelper->SetChannelConditionModelAttribute ("UpdatePeriod", TimeValue (MilliSeconds (0)));
  nrHelper->SetPathlossAttribute ("ShadowingEnabled", BooleanValue (false));
  nrHelper->InitializeOperationBand (&bandSl);

  allBwps = CcBwpCreator::GetAllBwps ({bandSl});

  nrHelper->SetUeAntennaAttribute ("NumRows", UintegerValue (1));
  nrHelper->SetUeAntennaAttribute ("NumColumns", UintegerValue (2));
  nrHelper->SetUeAntennaAttribute ("AntennaElement", PointerValue (CreateObject<IsotropicAntennaModel> ()));
  nrHelper->SetUePhyAttribute ("TxPower", DoubleValue (txPower));

  //NR Sidelink attribute of UE MAC, which are common for all the UEs
  nrHelper->SetUeMacAttribute ("EnableSensing", BooleanValue (false));
  nrHelper->SetUeMacAttribute ("T1", UintegerValue (2));
  nrHelper->SetUeMacAttribute ("T2", UintegerValue (33));
  nrHelper->SetUeMacAttribute ("ActivePoolId", UintegerValue (0));
  nrHelper->SetUeMacAttribute ("NumSidelinkProcess", UintegerValue (4));

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


  //Install UE NetDevices
  NetDeviceContainer ueNetDev = nrHelper->InstallUeDevice (ueNodeContainer, allBwps);
  for (auto it = ueNetDev.Begin (); it != ueNetDev.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }

  /**************************** SL configuration *****************************/
  Ptr<NrSlHelper> nrSlHelper = CreateObject <NrSlHelper> ();
  nrSlHelper->SetEpcHelper (epcHelper);

  //SL error model
  std::string errorModel = "ns3::NrEesmIrT1";
  nrSlHelper->SetSlErrorModel (errorModel);
  nrSlHelper->SetUeSlAmcAttribute ("AmcModel", EnumValue (NrAmc::ErrorModel));

  //SL scheduler
  nrSlHelper->SetNrSlSchedulerTypeId (NrSlUeMacSchedulerDefault::GetTypeId());
  nrSlHelper->SetUeSlSchedulerAttribute ("FixNrSlMcs", BooleanValue (true));
  nrSlHelper->SetUeSlSchedulerAttribute ("InitialNrSlMcs", UintegerValue (14));
  nrSlHelper->SetUeSlSchedulerAttribute ("PriorityToSps", BooleanValue (prioToSps));

  nrSlHelper->PrepareUeForSidelink (ueNetDev, bwpIdContainer);

  //SlResourcePoolNr IE
  LteRrcSap::SlResourcePoolNr slResourcePoolNr;
  Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory> ();
  std::vector <std::bitset<1> > slBitmap = {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1};
  ptrFactory->SetSlTimeResources (slBitmap);
  ptrFactory->SetSlSensingWindow (100); // T0 in ms
  ptrFactory->SetSlSelectionWindow (5);
  ptrFactory->SetSlFreqResourcePscch (10); // PSCCH RBs
  ptrFactory->SetSlSubchannelSize (50);
  ptrFactory->SetSlMaxNumPerReserve (3);
  ptrFactory->SetSlPsfchPeriod (0); // for blind retransmissions
  std::list<uint16_t> resourceReservePeriodList = {0, 50, 100}; // in ms
  ptrFactory->SetSlResourceReservePeriodList (resourceReservePeriodList);
  //Once parameters are configured, we can create the pool
  LteRrcSap::SlResourcePoolNr pool = ptrFactory->CreatePool ();
  slResourcePoolNr = pool;

  //Configure the SlResourcePoolConfigNr IE, which hold a pool and its id
  LteRrcSap::SlResourcePoolConfigNr slresoPoolConfigNr;
  slresoPoolConfigNr.haveSlResourcePoolConfigNr = true;
  uint16_t poolId = 0;
  LteRrcSap::SlResourcePoolIdNr slResourcePoolIdNr;
  slResourcePoolIdNr.id = poolId;
  slresoPoolConfigNr.slResourcePoolId = slResourcePoolIdNr;
  slresoPoolConfigNr.slResourcePool = slResourcePoolNr;

  //Configure the SlBwpPoolConfigCommonNr IE, which hold an array of pools
  LteRrcSap::SlBwpPoolConfigCommonNr slBwpPoolConfigCommonNr;
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

  //Configure the SlFreqConfigCommonNr IE, which hold the array to store
  //the configuration of all Sidelink BWP (s).
  LteRrcSap::SlFreqConfigCommonNr slFreConfigCommonNr;
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
  nrSlHelper->InstallNrSlPreConfiguration (ueNetDev, slPreConfigNr);

  /****************************** End SL Configuration ***********************/

  //Fix random streams
  int64_t stream = 1;
  stream += nrHelper->AssignStreams (ueNetDev, stream);
  stream += nrSlHelper->AssignStreams (ueNetDev, stream);

  //Configure internet
  InternetStackHelper internet;
  internet.Install (ueNodeContainer);
  stream += internet.AssignStreams (ueNodeContainer, stream);

  //Target IP
  Ipv4Address groupAddress4 ("225.0.0.0");     //use multicast address as destination

  /************************** Traffic flows configuration ********************/
  /*
   * All traffic flows will be directed to the target IP (groupAddress) but each to a
   * different port. For each flow:
   * - we create a traffic profile (SlInfo),
   * - we create a traffic flow template (LteSlTft) using the traffic profile, and
   * - we activate a SL data radio bearer that will steer the traffic according to the TFT
   */

  //Create the traffic profiles
  uint32_t dstL2Id1 = 255;
  uint32_t dstL2Id2 = 254;

  SidelinkInfo slInfo1;
  slInfo1.m_castType = SidelinkInfo::CastType::Groupcast;
  slInfo1.m_harqEnabled = true;
  SidelinkInfo slInfo2;
  slInfo2.m_castType = SidelinkInfo::CastType::Groupcast;
  slInfo2.m_harqEnabled = true;
  SidelinkInfo slInfo3;
  slInfo3.m_castType = SidelinkInfo::CastType::Groupcast;
  slInfo3.m_harqEnabled = true;

  //Assign the three traffic profile parameter values depending on the configuration
  switch (schedTypeConfig)
  {
    case 1:
      slInfo1.m_dynamic = true;
      slInfo2.m_dynamic = true;
      slInfo3.m_dynamic = true;
      break;
    case 2:
      slInfo1.m_dynamic = false;
      slInfo2.m_dynamic = false;
      slInfo3.m_dynamic = false;
      break;
    case 3:
      slInfo1.m_dynamic = true;
      slInfo2.m_dynamic = true;
      slInfo3.m_dynamic = false;
      break;
    case 4:
      slInfo1.m_dynamic = false;
      slInfo2.m_dynamic = false;
      slInfo3.m_dynamic = true;
      break;
  }

  switch (dstL2IdConfig)
  {
    case 1:
      slInfo1.m_dstL2Id = dstL2Id1;
      slInfo2.m_dstL2Id = dstL2Id1;
      slInfo3.m_dstL2Id = dstL2Id1;
      break;
    case 2:
      slInfo1.m_dstL2Id = dstL2Id1;
      slInfo2.m_dstL2Id = dstL2Id2;
      slInfo3.m_dstL2Id = dstL2Id1;
      break;
    case 3:
      slInfo1.m_dstL2Id = dstL2Id2;
      slInfo2.m_dstL2Id = dstL2Id2;
      slInfo3.m_dstL2Id = dstL2Id1;
      break;
  }

  switch (priorityConfig)
  {
    case 1:
      slInfo1.m_priority = 1;
      slInfo2.m_priority = 1;
      slInfo3.m_priority = 1;
      break;
    case 2:
      slInfo1.m_priority = 1;
      slInfo2.m_priority = 2;
      slInfo3.m_priority = 3;
      break;
    case 3:
      slInfo1.m_priority = 2;
      slInfo2.m_priority = 2;
      slInfo3.m_priority = 1;
      break;
    case 4:
      slInfo1.m_priority = 1;
      slInfo2.m_priority = 1;
      slInfo3.m_priority = 2;
      break;
  }

  switch (rriConfig)
  {
    case 1:
      slInfo1.m_rri = MilliSeconds (100);
      slInfo2.m_rri = MilliSeconds (100);
      slInfo3.m_rri = MilliSeconds (100);
      break;
    case 2:
      slInfo1.m_rri = MilliSeconds (100);
      slInfo2.m_rri = MilliSeconds (50);
      slInfo3.m_rri = MilliSeconds (100);
      break;
  }

  //Create TFTs and activate SL-DRBs
  Address remoteAddress1;
  Address remoteAddress2;
  Address remoteAddress3;
  Address localAddress1;
  Address localAddress2;
  Address localAddress3;
  uint16_t port1 = 8001;
  uint16_t port2 = 8002;
  uint16_t port3 = 8003;
  Ptr<LteSlTft> tft1;
  Ptr<LteSlTft> tft2;
  Ptr<LteSlTft> tft3;


  //Assign IP address for the UEs
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (ueNetDev);

  // Set the default gateway for the UEs
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  for (uint32_t u = 0; u < ueNodeContainer.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodeContainer.Get (u);
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  //Create TFTs for each traffic profile and corresponding addresses/port
  remoteAddress1 = InetSocketAddress (groupAddress4, port1);
  localAddress1 = InetSocketAddress (Ipv4Address::GetAny (), port1);
  tft1 = Create<LteSlTft> (LteSlTft::Direction::BIDIRECTIONAL, groupAddress4, port1, slInfo1);

  remoteAddress2 = InetSocketAddress (groupAddress4, port2);
  localAddress2 = InetSocketAddress (Ipv4Address::GetAny (), port2);
  tft2 = Create<LteSlTft> (LteSlTft::Direction::BIDIRECTIONAL, groupAddress4, port2, slInfo2);

  remoteAddress3 = InetSocketAddress (groupAddress4, port3);
  localAddress3 = InetSocketAddress (Ipv4Address::GetAny (), port3);
  tft3 = Create<LteSlTft> (LteSlTft::Direction::BIDIRECTIONAL, groupAddress4, port3, slInfo3);

  //Activate SL data radio bearers for each traffic flow template and profile
  nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, ueNetDev, tft1);
  nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, ueNetDev, tft2);
  nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, ueNetDev, tft3);

  /*
   * Configure the applications:
   * Client app: OnOff application configure to generate CBR traffic
   * Server app: PacketSink application.
   */

  //Configure client applications
  std::string dataRateString  = std::to_string (dataRate) + "kb/s";
  OnOffHelper sidelinkClient1 ("ns3::UdpSocketFactory", remoteAddress1);
  sidelinkClient1.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
  sidelinkClient1.SetConstantRate (DataRate (dataRateString), udpPacketSize);

  OnOffHelper sidelinkClient2 ("ns3::UdpSocketFactory", remoteAddress2);
  sidelinkClient2.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
  sidelinkClient2.SetConstantRate (DataRate (dataRateString), udpPacketSize);

  OnOffHelper sidelinkClient3 ("ns3::UdpSocketFactory", remoteAddress3);
  sidelinkClient3.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
  sidelinkClient3.SetConstantRate (DataRate (dataRateString), udpPacketSize);


  ApplicationContainer allClientApps;
  ApplicationContainer allServerApps;

  //Install client applications on the first UE (Tx)
  ApplicationContainer clientApps1 = sidelinkClient1.Install (ueNodeContainer.Get (0));
  clientApps1.Start (finalSlBearersActivationTime);
  clientApps1.Stop (finalSimTime);
  allClientApps.Add (clientApps1);

  ApplicationContainer clientApps2 = sidelinkClient2.Install (ueNodeContainer.Get (0));
  clientApps2.Start (finalSlBearersActivationTime);
  clientApps2.Stop (finalSimTime);
  allClientApps.Add (clientApps2);

  ApplicationContainer clientApps3 = sidelinkClient3.Install (ueNodeContainer.Get (0));
  clientApps3.Start (finalSlBearersActivationTime);
  clientApps3.Stop (finalSimTime);
  allClientApps.Add (clientApps3);

  //Configure server applications and install them in the second UE (Rx)
  ApplicationContainer serverApps1;
  PacketSinkHelper sidelinkSink1 ("ns3::UdpSocketFactory", localAddress1);
  sidelinkSink1.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
  serverApps1 = sidelinkSink1.Install (ueNodeContainer.Get (1));
  serverApps1.Start (Seconds (2.0));
  allServerApps.Add (serverApps1);

  ApplicationContainer serverApps2;
  PacketSinkHelper sidelinkSink2 ("ns3::UdpSocketFactory", localAddress2);
  sidelinkSink2.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
  serverApps2 = sidelinkSink2.Install (ueNodeContainer.Get (1));
  serverApps2.Start (Seconds (2.0));
  allServerApps.Add (serverApps2);

  ApplicationContainer serverApps3;
  PacketSinkHelper sidelinkSink3 ("ns3::UdpSocketFactory", localAddress3);
  sidelinkSink3.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
  serverApps3 = sidelinkSink3.Install (ueNodeContainer.Get (1));
  serverApps3.Start (Seconds (2.0));
  allServerApps.Add (serverApps3);

  /************************ END Traffic flows configuration ******************/


  /******************** Application packet tracing ***************************/
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> PacketTraceForDelayStream = ascii.CreateFileStream ("NrSlAppRxPacketDelayTrace.txt");
  *PacketTraceForDelayStream->GetStream () << "time(s)\trxNodeId\tsrcIp\tdstIp\tseqNum\tdelay(ms)" << std::endl;

  for (uint16_t ac = 0; ac < allClientApps.GetN (); ac++)
    {
      allClientApps.Get (ac)->TraceConnectWithoutContext ("TxWithSeqTsSize", MakeCallback (&TxPacketTraceForDelay));
    }
  for (uint16_t ac = 0; ac < allServerApps.GetN (); ac++)
    {
      allServerApps.Get (ac)->TraceConnectWithoutContext ("RxWithSeqTsSize", MakeCallback (&RxPacketTraceForDelay));
    }
  /******************** END Application packet  tracing **********************/

  Simulator::Stop (finalSimTime);
  Simulator::Run ();

  std::cout << "Total Tx packets = " << g_txPktCounter << std:: endl;
  std::cout << "Total Rx packets = " << g_rxPktCounter << std:: endl;
  double delaySum = 0;
  for (auto it = g_delays.begin () ; it != g_delays.end (); it ++ )
    {
      delaySum += *it;
    }
  std::cout << "Average packet delay = " << delaySum / g_delays.size () << " ms" << std:: endl;

  Simulator::Destroy ();
  if (testing)
    {
      if (g_rxPktCounter < 0.95 * g_txPktCounter)
        {
          std::cout << "Not enough packets received" << std:: endl;
          return 1;
        }
    }
  return 0;
}


