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
 * \file nr-prose-unicast-multi-link.cc
 * \brief A simple example using ProSe Unicast communication, with a sidelink
 *        configuration based on cttc-nr-v2x-demo-simple.cc example
 *
 * Channel configuration:
 * This example setups an NR sidelink out-of-coverage simulation using the 3GPP
 * channel model from TR 37.885.
 *
 * System configuration:
 * Sidelink will use one operational band, containing one component carrier,
 * and a single bandwidth part centered at the frequency specified by the
 * corresponding input parameter. The system bandwidth, the numerology to
 * be used and the transmission power can be setup as well.
 * Sensing based resource allocation can be enabled by setting the
 * corresponding input parameter to true. The default configuration is with
 * no sensing.
 *
 *
 * Topology:
 * There are 3 UEs in the topology located with an inter-UE distance specified
 * by the corresponding input parameter. Default configuration:
 *          UE0..........(20 m)..........UE1..........(20 m)..........UE2
 *   (0.0, 0.0, 1.5)               (20, 0.0, 1.5)               (40, 0.0, 1.5)
 *
 * ProSe Unicast:
 * Each pair of UEs in the system will establish a ProSe unicast direct link at
 * a given simulation time specified by the corresponding input parameter.
 * The selection of the initiating UE is done by increasing node ID, i.e.,
 * UE i is in the initiating UE of the link with UE i+1, UE i+2, etc. In the
 * default configuration of 3 UEs this means:
 *     Link     | Initiating UE  |   Target UE
 * ---------------------------------------------
 * UE0 <-> UE1  |      UE0       |     UE1
 * UE0 <-> UE2  |      UE0       |     UE2
 * UE1 <-> UE2  |      UE1       |     UE2
 *
 *
 * Traffic:
 * A CBR traffic flow that goes from the initiating UE of each link towards
 * the target UE of the link is configured by default. If the 'bidirectional'
 * parameter is set to True, another CBR flow is installed in the target UE
 * towards the initiating UE. The packet size, data rate and general starting
 * time of the flows can be specified in the input parameters. The actual
 * starting time of the flow is randomized in a interval of 100 ms after the
 * general starting time to avoid all flows starting at the same time.
 *
 *
 * Output:
 * The example will print on the standard output the total transmitted and
 * received bits in the system and the corresponding number of packets. It
 * also prints the traffic statistics per traffic flow.
 * The example also produces three output files:
 * 1. default-nr-prose-unicast-multi-link.db: contains PHY layer traces in a 
 * sqlite3 database created using ns-3 stats module.
 * 2. default-nr-prose-unicast-multi-link-flowMonitorOutput.txt: statistics per
 * traffic flow (the same that are printed in the standard output)
 * 3. default-nr-prose-unicast-multi-link-NrSlPc5SignallingPacketTrace.txt: log
 * of the transmitted and received PC5 signaling messages used for the 
 * establishment of each ProSe unicast direct link.
 * 
 * Please refer to the cttc-nr-v2x-demo-simple.cc example for a nice tutorial
 * of scenario setup and NR sidelink configuration.
 *
 * \code{.unparsed}
$ ./ns3 run "nr-prose-unicast-multi-link --help"
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
#include <sqlite3.h>
#include "ns3/flow-monitor-module.h"


//Dependency of these nr-v2x-examples classes for statistics
#include "../nr-v2x-examples/ue-mac-pscch-tx-output-stats.h"
#include "../nr-v2x-examples/ue-mac-pssch-tx-output-stats.h"
#include "../nr-v2x-examples/ue-phy-pscch-rx-output-stats.h"
#include "../nr-v2x-examples/ue-phy-pssch-rx-output-stats.h"
#include "../nr-v2x-examples/ue-to-ue-pkt-txrx-output-stats.h"
#include "../nr-v2x-examples/v2x-kpi.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrProseUnicastMultiLink");


/*
 * Global methods and variable to hook trace sources from different layers of
 * the protocol stack.
 */

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

/**
 * \brief Method to listen to the application level traces of type TxWithAddresses
 *        and RxWithAddresses.
 * \param stats Pointer to the \link UeToUePktTxRxOutputStats \endlink class,
 *        which is responsible for writing the trace source parameters to a database. *
 * \param nodeId The node id of the TX or RX node
 * \param localAddrs The local IPV4 address of the node
 * \param txRx The string indicating the type of node, i.e., TX or RX
 * \param p The packet
 * \param srcAddrs The source address from the trace
 * \param dstAddrs The destination address from the trace
 * \param seqTsSizeHeader The SeqTsSizeHeader
 */
void
UePacketTraceDb (UeToUePktTxRxOutputStats *stats, Ptr<Node> node, const Address &localAddrs,
                 std::string txRx, Ptr<const Packet> p, const Address &srcAddrs,
                 const Address &dstAddrs, const SeqTsSizeHeader &seqTsSizeHeader)
{
  uint32_t nodeId = node->GetId ();
  uint64_t imsi = node->GetDevice (0)->GetObject<NrUeNetDevice> ()->GetImsi ();
  uint32_t seq = seqTsSizeHeader.GetSeq ();
  uint32_t pktSize = p->GetSize () + seqTsSizeHeader.GetSerializedSize ();

  stats->Save (txRx, localAddrs, nodeId, imsi, pktSize, srcAddrs, dstAddrs, seq);
}

/*
 * Global variables to count TX/RX packets and bytes.
 */

uint32_t rxByteCounter = 0;
uint32_t txByteCounter = 0;
uint32_t rxPktCounter = 0;
uint32_t txPktCounter = 0;

/**
 * \brief Method to listen to the packet sink application trace Rx.
 * \param packet The packet
 * \param The address of the transmitter
 */
void ReceivePacket (Ptr<const Packet> packet, const Address & from [[maybe_unused]])
{
  rxByteCounter += packet->GetSize ();
  rxPktCounter++;
}

/**
 * \brief Method to listen to the transmitting application trace Tx.
 * \param packet The packet
 */
void TransmitPacket (Ptr<const Packet> packet)
{
  txByteCounter += packet->GetSize ();
  txPktCounter++;
}

/*
 * \brief Trace sink function for logging transmission and reception of PC5
 *        signaling (PC5-S) messages
 *
 * \param stream the output stream wrapper where the trace will be written
 * \param node the pointer to the UE node
 * \param srcL2Id the L2 ID of the UE sending the PC5-S packet
 * \param dstL2Id the L2 ID of the UE receiving the PC5-S packet
 * \param isTx flag that indicates if the UE is transmitting the PC5-S packet
 * \param p the PC5-S packet
 */
void
TraceSinkPC5SignallingPacketTrace (Ptr<OutputStreamWrapper> stream,
                                   Ptr<Node> node,
                                   uint32_t srcL2Id, uint32_t dstL2Id, bool isTx, Ptr<Packet> p)
{
  NrSlPc5SignallingMessageType pc5smt;
  p->PeekHeader (pc5smt);
  *stream->GetStream () << Simulator::Now ().GetSeconds ()
                        << "\t" << node->GetId ();
  if (isTx)
    {
      *stream->GetStream () << "\t" << "TX";
    }
  else
    {
      *stream->GetStream () << "\t" << "RX";
    }
  *stream->GetStream () << "\t" << srcL2Id << "\t" << dstL2Id << "\t" << pc5smt.GetMessageName ();
  *stream->GetStream () << std::endl;
}

int
main (int argc, char *argv[])
{
  //Topology parameters
  uint16_t interUeDistance = 20; //meters

  // Traffic parameters
  uint32_t udpPacketSize = 200; //bytes
  double dataRate = 16; //16 kilobits per second
  bool bidirectional = false;

  // Simulation timeline parameters
  Time simTime = Seconds (20.0); //Total simulation time
  Time startDirLinkTime = Seconds (2.0); //Time to start the Prose Unicast Direct Link establishment procedure
  Time startTrafficTime = Seconds (3.0); //Time to start the traffic in the application layer


  // NR parameters
  uint16_t numerologyBwpSl = 2; //The numerology to be used in sidelink bandwidth part
  double centralFrequencyBandSl = 5.89e9; // band n47  TDD //Here band is analogous to channel
  uint16_t bandwidthBandSl = 400; //Multiple of 100 KHz; 400 = 40 MHz
  double txPower = 23; //dBm

  // SL parameters
  bool sensing = false;

  // Where we will store the output files.
  std::string simTag = "default";

  CommandLine cmd;

  cmd.AddValue ("interUeDistance",
                "The distance among the UEs in the topology",
                interUeDistance);
  cmd.AddValue ("packetSizeBe",
                "packet size in bytes to be used by the traffic",
                udpPacketSize);
  cmd.AddValue ("dataRate",
                "The data rate in kilobits per second",
                dataRate);
  cmd.AddValue ("bidirectional",
                "True if traffic should flow in both direction of the unicast link, "
                "False if only a flow in the initiating->target direction is desired",
                bidirectional);
  cmd.AddValue ("simTime",
                "Simulation time in seconds",
                simTime);
  cmd.AddValue ("startDirLinkTime",
                "Time to start the Prose Unicast Direct Link establishment procedure in seconds",
                startDirLinkTime);
  cmd.AddValue ("startTrafficTime",
                "Time to start the traffic in the application layer, in seconds",
                startTrafficTime);
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
  cmd.AddValue ("sensing",
                "If true, it enables the sensing based resource selection for "
                "SL, otherwise, no sensing is applied",
                sensing);
  cmd.AddValue ("simTag",
                "tag to be appended to output filenames to distinguish simulation campaigns",
                simTag);

  // Parse the command line
  cmd.Parse (argc, argv);

  //Check if the frequency is in the allowed range.
  NS_ABORT_IF (centralFrequencyBandSl > 6e9);

  //Setup large enough buffer size to avoid overflow
  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (999999999));

  //UE nodes creation
  NodeContainer ueVoiceContainer;
  uint16_t ueNum = 3;
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
  nrHelper->SetUeMacAttribute ("EnableSensing", BooleanValue (sensing));
  nrHelper->SetUeMacAttribute ("T1", UintegerValue (2));
  nrHelper->SetUeMacAttribute ("T2", UintegerValue (33));
  nrHelper->SetUeMacAttribute ("ActivePoolId", UintegerValue (0));
  nrHelper->SetUeMacAttribute ("ReservationPeriod", TimeValue (MilliSeconds (10)));
  nrHelper->SetUeMacAttribute ("NumSidelinkProcess", UintegerValue (255));
  nrHelper->SetUeMacAttribute ("EnableBlindReTx", BooleanValue (true));
  nrHelper->SetUeMacAttribute ("SlThresPsschRsrp", IntegerValue (-128));

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
  uint16_t port = 8000;
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (ueVoiceNetDev);

  std::vector<Ipv4Address> ipv4AddressVector (ueNum);

  // set the default gateway for the UE
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  for (uint32_t u = 0; u < ueVoiceContainer.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueVoiceContainer.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

      //Obtain local IPv4 addresses that will be used to route the unicast traffic upon setup of the direct link
      ipv4AddressVector [u] = ueVoiceContainer.Get (u)->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
    }

  /*
   * Configure ProSe
   */

  //Create ProSe helper
  Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject <NrSlProseHelper> ();
  // Install ProSe layer and corresponding SAPs in the UEs
  nrSlProseHelper->PrepareUesForProse (ueVoiceNetDev);
  //Configure ProSe Unicast parameters. At the moment it only instruct the MAC
  //layer (and PHY therefore) to monitor packets directed the UE's own Layer 2 ID
  nrSlProseHelper->PrepareUesForUnicast (ueVoiceNetDev);
  //Configure the value of timer Timer T5080 (Prose Direct Link Establishment Request Retransmission)
  //to a lower value than the standard (8.0 s) to speed connection in shorter simulation time
  Config::SetDefault ("ns3::NrSlUeProseDirectLink::T5080", TimeValue (Seconds (1.0)));
  /*
   * Setup the start of the ProSe direct link establishment procedure
   * (with the 'Real' protocol, PC5-S messages are exchanged over the SL)
   * First UE on the function call will be the initiating UE (UE i),
   * which starts the procedure, and it is interested in establish a direct
   * link with the following j UEs, which will be the target UEs
   */
  NS_LOG_INFO ("Configuring unicast direct links..." );

  for (uint32_t i = 0; i < ueVoiceContainer.GetN () - 1; ++i)
    {
      for (uint32_t j = i + 1; j < ueVoiceContainer.GetN (); ++j)
        {
          nrSlProseHelper->EstablishRealDirectLink (startDirLinkTime,
                                                    ueVoiceNetDev.Get (i), ipv4AddressVector [i],
                                                    ueVoiceNetDev.Get (j), ipv4AddressVector [j]);
          NS_LOG_INFO ("Initiating UE nodeId " << i << " target UE nodeId " << j );

        }
    }

  /*********************** End ProSe configuration ***************************/

  /*
   * Configure the applications:
   * - Client app: OnOff application configured to generate CBR traffic.
   *               Installed in the initiating UE of each link by default, and
   *               in target UE if 'bidirectional' flag is True.
   * - Server app: PacketSink application to consume the received traffic.
   *               Installed in all UEs
   */
  NS_LOG_INFO ("Configuring applications..." );
  // Random variable to randomize a bit start times of the client applications
  //to avoid simulation artifacts of all the TX UEs transmitting at the same time.
  Ptr<UniformRandomVariable> startTimeRnd = CreateObject<UniformRandomVariable> ();
  startTimeRnd->SetAttribute ("Min", DoubleValue (0));
  startTimeRnd->SetAttribute ("Max", DoubleValue (0.10));

  std::string dataRateString  = std::to_string (dataRate) + "kb/s";
  ApplicationContainer clientApps;
  std::cout << "Traffic flows: " << std::endl;
  for (uint32_t i = 0; i < ueVoiceContainer.GetN () - 1; ++i)
    {
      for (uint32_t j = i + 1; j < ueVoiceContainer.GetN (); ++j)
        {
          OnOffHelper sidelinkClient ("ns3::UdpSocketFactory",
                                      InetSocketAddress (ipv4AddressVector [j], port)); //Towards UE j
          sidelinkClient.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
          sidelinkClient.SetConstantRate (DataRate (dataRateString), udpPacketSize);
          ApplicationContainer app = sidelinkClient.Install (ueVoiceContainer.Get (i)); // Installed in UE i
          Time appStart = startTrafficTime + Seconds (startTimeRnd->GetValue ());
          app.Start (appStart);
          clientApps.Add (app);
          NS_LOG_INFO ("OnOff application installed in UE nodeId " << i << " srcIp " << ipv4AddressVector [i] <<
                       " towards UE nodeId " << j << " dstIp " << ipv4AddressVector [j] );
          std::cout << ipv4AddressVector [i] << " -> " << ipv4AddressVector [j] <<
            " start time: " << appStart.GetSeconds () << " s, end time: " << simTime.GetSeconds () << " s" << std::endl;

          if (bidirectional)
            {
              OnOffHelper sidelinkClient ("ns3::UdpSocketFactory",
                                          InetSocketAddress (ipv4AddressVector [i], port)); // Towards UE i
              sidelinkClient.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
              sidelinkClient.SetConstantRate (DataRate (dataRateString), udpPacketSize);
              ApplicationContainer app = sidelinkClient.Install (ueVoiceContainer.Get (j)); // Installed in UE j
              Time appStart = startTrafficTime + Seconds (startTimeRnd->GetValue ());
              app.Start (appStart);
              clientApps.Add (app);
              NS_LOG_INFO ("OnOff application installed in UE nodeId " << j << " srcIp " << ipv4AddressVector [j] <<
                           " towards UE nodeId " << i << " dstIp " << ipv4AddressVector [i] );
              std::cout << ipv4AddressVector [j] << " -> " << ipv4AddressVector [i] <<
                " start time: " << appStart.GetSeconds ()  << " s, end time: " << simTime.GetSeconds () << " s" << std::endl;
            }
        }
    }

  clientApps.Stop (simTime);

  ApplicationContainer serverApps;
  PacketSinkHelper sidelinkSink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
  sidelinkSink.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
  serverApps = sidelinkSink.Install (ueVoiceContainer);  // Installed in all UEs
  serverApps.Start (Seconds (2.0));

  /******************** End application configuration ************************/


  /*
   * Hook the traces, to be used to compute average PIR and to data to be
   * stored in a database
   */

  //Trace receptions; use the following to be robust to node ID changes
  std::ostringstream path;
  path << "/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx";
  Config::ConnectWithoutContext (path.str (), MakeCallback (&ReceivePacket));
  path.str ("");

  path << "/NodeList/*/ApplicationList/*/$ns3::OnOffApplication/Tx";
  Config::ConnectWithoutContext (path.str (), MakeCallback (&TransmitPacket));
  path.str ("");

  //Database setup
  std::string exampleName = simTag + "-" + "nr-prose-unicast-multi-link";
  SQLiteOutput db (exampleName + ".db");


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

  // Set Tx traces
  for (uint16_t ac = 0; ac < clientApps.GetN (); ac++)
    {
      Ipv4Address localAddrs =  clientApps.Get (ac)->GetNode ()->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
      clientApps.Get (ac)->TraceConnect ("TxWithSeqTsSize", "tx", MakeBoundCallback (&UePacketTraceDb, &pktStats, clientApps.Get (ac)->GetNode (), localAddrs));
    }

  // Set Rx traces
  for (uint16_t ac = 0; ac < serverApps.GetN (); ac++)
    {
      Ipv4Address localAddrs =  serverApps.Get (ac)->GetNode ()->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
      serverApps.Get (ac)->TraceConnect ("RxWithSeqTsSize", "rx", MakeBoundCallback (&UePacketTraceDb, &pktStats, serverApps.Get (ac)->GetNode (), localAddrs));
    }

  /******************* PC5-S messages tracing ********************************/
  AsciiTraceHelper ascii;
  std::string pc5s_filename = exampleName  + "-NrSlPc5SignallingPacketTrace.txt";
  Ptr<OutputStreamWrapper> Pc5SignallingPacketTraceStream = ascii.CreateFileStream (pc5s_filename.c_str ());
  *Pc5SignallingPacketTraceStream->GetStream () << "time(s)\tnodeId\tTX/RX\tsrcL2Id\tdstL2Id\tmsgType" << std::endl;
  for (uint32_t i = 0; i < ueVoiceNetDev.GetN (); ++i)
  {
	  Ptr<NrSlUeProse> prose = ueVoiceNetDev.Get (i)->GetObject<NrUeNetDevice> ()->GetSlUeService ()->GetObject <NrSlUeProse> ();
	  prose->TraceConnectWithoutContext ("PC5SignallingPacketTrace",
			  MakeBoundCallback (&TraceSinkPC5SignallingPacketTrace,
					  Pc5SignallingPacketTraceStream,
					  ueVoiceNetDev.Get (i)->GetNode ()));
  }
  /******************* END PC5-S messages tracing **************************/

  //Configure FlowMonitor to get traffic flow statistics
  FlowMonitorHelper flowmonHelper;
  NodeContainer endpointNodes;
  endpointNodes.Add (ueVoiceContainer);

  Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install (endpointNodes);
  monitor->SetAttribute ("DelayBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("JitterBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("PacketSizeBinWidth", DoubleValue (20));

  Simulator::Stop (simTime + Seconds (1.0));
  Simulator::Run ();

  std::cout << "System total Tx packets = " << txPktCounter << std:: endl;
  std::cout << "System total Tx bits = " << txByteCounter * 8 << std:: endl;
  std::cout << "System total Rx packets = " << rxPktCounter << std:: endl;
  std::cout << "System total Rx bits = " << rxByteCounter * 8 << std:: endl;
  std::cout << "System average thput = " << (rxByteCounter * 8) / (simTime - startTrafficTime).GetSeconds () / 1000.0 << " kbps" << std:: endl;


  /*
   * VERY IMPORTANT: Do not forget to empty the database cache, which would
   * dump the data store towards the end of the simulation in to a database.
   */
  pktStats.EmptyCache ();
  pscchPhyStats.EmptyCache ();
  psschPhyStats.EmptyCache ();

  //Print per-flow statistics
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

  std::ofstream outFile;
  std::string filename = exampleName  + "-flowMonitorOutput.txt";
  outFile.open (filename.c_str (), std::ofstream::out | std::ofstream::trunc);
  if (!outFile.is_open ())
    {
      std::cerr << "Can't open file " << filename << std::endl;
      return 1;
    }

  outFile.setf (std::ios_base::fixed);

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      std::stringstream protoStream;
      protoStream << (uint16_t) t.protocol;
      if (t.protocol == 6)
        {
          protoStream.str ("TCP");
        }
      if (t.protocol == 17)
        {
          protoStream.str ("UDP");
        }

      double appDuration = 0;

      appDuration = simTime.GetSeconds () - startTrafficTime.GetSeconds (); //Some inaccuracy is expected due to randomization of start time.

      outFile << "  Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ") " << protoStream.str () << "\n";
      outFile << "    Tx Packets:   " << i->second.txPackets << "\n";
      outFile << "    Tx Bytes:     " << i->second.txBytes << "\n";
      outFile << "    Tx Data rate: " << i->second.txBytes * 8.0 / appDuration / 1000  << " kbps\n";
      outFile << "    Rx Packets:   " << i->second.rxPackets << "\n";
      outFile << "    Rx Bytes:     " << i->second.rxBytes << "\n";
      if (i->second.rxPackets > 0)
        {
          outFile << "    Rx Data rate: " << i->second.rxBytes * 8.0 / appDuration / 1000   << " kbps\n";
          outFile << "    Mean delay:   " << 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets << " ms\n";
        }
      else
        {
          outFile << "    Throughput:  0 kbps\n";
          outFile << "    Mean delay:  0 ms\n";
          outFile << "    Mean jitter: 0 ms\n";
        }
    }
  outFile.close ();

  std::cout << "Traffic flows statistics: " << std::endl;
  std::ifstream f (filename.c_str ());
  if (f.is_open ())
    {
      std::cout << f.rdbuf ();
    }

  Simulator::Destroy ();
  return 0;
}


