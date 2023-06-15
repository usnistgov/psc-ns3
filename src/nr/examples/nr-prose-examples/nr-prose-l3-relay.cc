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
 * \file nr-prose-l3-relay.cc
 * \ingroup examples
 *
 * \brief Basic scenario with some in-network UEs doing in-network 
 *        communication and some out-of-network UEs doing in-network
 *        communication through a L3 UE-to-Network (U2N) relay UE.
 *
 * Channel configuration:
 * This example setups a simulation using the 3GPP channel model from TR 37.885
 * and it uses the default configuration of its implementation.
 *
 * System configuration:
 * The scenario uses one operational band, containing one component carrier,
 * and two bandwidth parts. One bandwidth part is used for in-network
 * communication, i.e., UL and DL between in-network UEs and gNBs, and the
 * other bandwidth part is used for SL communication between UEs using SL.
 *
 * Topology:
 * The scenario is composed of one gNB and four UEs. Two of the UEs (UE0 and
 * UE1) are attached to the gNB and the other two UEs (UE2 and UE3) are
 * out-of-network. UE1 is configured to serve as a L3 UE-to-Network (U2N)
 * relay.
 *
 *        -  gNB              (0.0, 30.0, 10.0)
 *        |
 *   20 m |
 *        -  UE0 UE1          (0.0, 10.0, 1.5) (1.0, 10.0, 1.5)
 *   10 m |       |
 *        -  UE2  |  UE3      (0.0, 0.0, 1.5) (2.0, 0.0, 1.5)
 *            |---|---|
 *             1 m 1 m
 *            |-------|
 *               2 m
 *
 *
 * L3 UE-to-Network relay:
 * UE2 and UE3 will start the establishment of the L3 U2N relay connection
 * before the start of the in-network traffic. This will internally start the
 * establishment of the corresponding ProSe unicast direct links. The
 * configuration is the following:
 *              |   Remote UE    |  Relay UE
 *     Link     |(Initiating UE) |(Target UE)
 * ---------------------------------------------
 * UE2 <-> UE1  |       UE2      |    UE1
 * UE3 <-> UE1  |       UE3      |    UE1
 *
 *
 * Traffic:
 * There are two CBR traffic flows concerning the in-network UEs (UE0 and UE1),
 * one from a Remote Host in the internet towards each in-network UE (DL) and
 * one from the in-network UEs towards the Remote Host (UL). Additionally, two
 * CBR traffic flows with the same configuration are configured for each
 * out-of-network UE (UE2, and UE3) to be served when they connect to the U2N
 * relay UE (UE1).
 *
 * Output:
 * The example will print on-screen the traffic flows configuration and the
 * end-to-end statistics of each of them after the simulation finishes together
 * with the number of packets relayed by the L3 UE-to-Network relay.
 * The example also produces three output files:
 * 1. default-nr-prose-l3-relay-flowMonitorOutput.txt" Contains the
 * end-to-end statistics of each traffic flow.
 * 2. default-nr-prose-l3-relay.db: contains PHY layer traces in a sqlite3 
 * database created using ns-3 stats module.
 * 3. default-nr-prose-l3-relay-NrSlPc5SignallingPacketTrace.txt: log of the
 * transmitted and received PC5 signaling messages used for the establishment 
 * of each ProSe unicast direct link.
 * 4. default-nr-prose-l3-relay-NrSlRelayNasRxPacketTrace.txt: log of the 
 packets received and routed by the NAS of the UE acting as L3 UE-to-Network UE.
 *
 * \code{.unparsed}
$ ./ns3 run "nr-prose-l3-relay --Help"
    \endcode
 */


#include "ns3/core-module.h"
#include "ns3/config-store.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/applications-module.h"
#include "ns3/log.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/nr-helper.h"
#include "ns3/nr-point-to-point-epc-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/config-store-module.h"
#include "ns3/nr-mac-scheduler-tdma-rr.h"
#include "ns3/nr-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ideal-beamforming-algorithm.h"
#include "ns3/antenna-module.h"
#include <ns3/nr-sl-pc5-signalling-header.h>
#include <ns3/nr-sl-ue-prose.h>
#include <ns3/epc-ue-nas.h>
#include <sqlite3.h>

//Dependency of these nr-v2x-examples classes for SL statistics
#include "../nr-v2x-examples/ue-mac-pscch-tx-output-stats.h"
#include "../nr-v2x-examples/ue-mac-pssch-tx-output-stats.h"
#include "../nr-v2x-examples/ue-phy-pscch-rx-output-stats.h"
#include "../nr-v2x-examples/ue-phy-pssch-rx-output-stats.h"
#include "../nr-v2x-examples/ue-to-ue-pkt-txrx-output-stats.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrProseL3Relay");

/*************************** Methods for tracing using database **************/
/*Please reffer to nr-prose-unicast-multi-link.cc for function documentation */
void NotifySlPscchRx (UePhyPscchRxOutputStats *pscchStats, const SlRxCtrlPacketTraceParams pscchStatsParams)
{
  pscchStats->Save (pscchStatsParams);
}
void NotifySlPsschRx (UePhyPsschRxOutputStats *psschStats, const SlRxDataPacketTraceParams psschStatsParams)
{
  psschStats->Save (psschStatsParams);
}
/************************END Methods for tracing using database **************/


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
                                   uint32_t srcL2Id, uint32_t dstL2Id, bool isTx, Ptr<Packet> p)
{
  NrSlPc5SignallingMessageType pc5smt;
  p->PeekHeader (pc5smt);
  *stream->GetStream () << Simulator::Now ().GetSeconds ();
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

std::map<std::string, uint32_t> g_relayNasPacketCounter;

/*
 * \brief Trace sink function for logging reception of data packets in the NAS
 *        layer by UE(s) acting as relay UE
 *
 * \param stream the output stream wrapper where the trace will be written
 * \param nodeIp the IP of the relay UE
 * \param srcIp the IP of the node sending the packet
 * \param dstIp the IP of the node that would be receiving the packet
 * \param srcLink the link from which the relay UE received the packet (UL, DL, or SL)
 * \param dstLink the link towards which the relay routed the packet (UL, DL, or SL)
 * \param p the packet
 */
void
TraceSinkRelayNasRxPacketTrace (Ptr<OutputStreamWrapper> stream,
                                Ipv4Address nodeIp, Ipv4Address srcIp, Ipv4Address dstIp,
                                std::string srcLink, std::string dstLink, Ptr<Packet> p)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds ()
                        << "\t" << nodeIp
                        << "\t" << srcIp
                        << "\t" << dstIp
                        << "\t" << srcLink
                        << "\t" << dstLink
                        << std::endl;
  std::ostringstream  oss;
  oss << nodeIp << "      " << srcIp << "->" << dstIp << "      " << srcLink << "->" << dstLink;
  std::string mapKey = oss.str ();
  auto it = g_relayNasPacketCounter.find (mapKey);
  if (it == g_relayNasPacketCounter.end ())
    {
      g_relayNasPacketCounter.insert (std::pair < std::string, uint32_t> (mapKey, 1));
    }
  else
    {
      it->second += 1;
    }
}

int
main (int argc, char *argv[])
{
  //System configuration
  double centralFrequencyBand = 5.89e9; // band n47 
  double bandwidthBand = 40e6; //40 MHz
  double centralFrequencyCc0 = 5.89e9;
  double bandwidthCc0 = bandwidthBand;
  std::string pattern = "DL|DL|DL|F|UL|UL|UL|UL|UL|UL|"; 
  double bandwidthCc0Bpw0 = bandwidthCc0 / 2;
  double bandwidthCc0Bpw1 = bandwidthCc0 / 2;
  double ueHeight = 1.5;

  //In-network devices configuration
  uint16_t numerologyCc0Bwp0 = 3; // BWP0 will be used for the in-network
  double gNBtotalTxPower = 32; // dBm

  //Applications configuration
  uint32_t packetSizeDlUl = 100; //bytes
  uint32_t lambdaDl = 50; // packets per second
  uint32_t lambdaUl = 50; // packets per second
  double trafficStartTime = 5.0; //seconds

  //Sidelink configuration
  uint16_t numerologyCc0Bwp1 = 2; // BWP1 will be used for SL
  Time startRelayConnTime = Seconds (2.0); //Time to start the U2N relay connection establishment

  //Simulation configuration
  std::string simTag = "default";
  double simTime = 15; // seconds

  CommandLine cmd;
  cmd.AddValue ("simTime", "Simulation time", simTime);
  cmd.Parse (argc, argv);

  //Setup large enough buffer size to avoid overflow
  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (999999999));

  //Create gNBs and in-network UEs, configure positions
  uint16_t gNbNum = 1;
  uint16_t inNetUeNum = 1;
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  NodeContainer gNbNodes;
  gNbNodes.Create (gNbNum);
  Ptr<ListPositionAllocator> gNbPositionAlloc = CreateObject<ListPositionAllocator> ();
  gNbPositionAlloc->Add (Vector (0.0, 30.0, 10));
  mobility.SetPositionAllocator (gNbPositionAlloc);
  mobility.Install (gNbNodes);

  NodeContainer inNetUeNodes;
  inNetUeNodes.Create (inNetUeNum);
  Ptr<ListPositionAllocator> inNetUePositionAlloc = CreateObject<ListPositionAllocator> ();
  inNetUePositionAlloc->Add (Vector (0.0, 10.0, ueHeight));
  mobility.SetPositionAllocator (inNetUePositionAlloc);
  mobility.Install (inNetUeNodes);

  //Create U2N relay nodes, configure positions
  uint16_t relayUeNum = 1;
  NodeContainer relayUeNodes;
  relayUeNodes.Create (relayUeNum);
  Ptr<ListPositionAllocator> relayUesPositionAlloc = CreateObject<ListPositionAllocator> ();
  relayUesPositionAlloc->Add (Vector (1.0, 10.0, 1.5));
  mobility.SetPositionAllocator (relayUesPositionAlloc);
  mobility.Install (relayUeNodes);

  //Create Remote UE nodes, configure positions
  uint16_t remoteUeNum = 2;
  uint16_t remoteInterUeDistance = 2; //m

  NodeContainer remoteUeNodes;
  remoteUeNodes.Create (remoteUeNum);
  Ptr<ListPositionAllocator> remoteUesPositionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < remoteUeNum; i++)
    {
      remoteUesPositionAlloc->Add (Vector (remoteInterUeDistance * i, 0.0, 1.5));
    }
  mobility.SetPositionAllocator (remoteUesPositionAlloc);
  mobility.Install (remoteUeNodes);

  //Setup Helpers
  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper> ();
  Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
  nrHelper->SetBeamformingHelper (idealBeamformingHelper);
  nrHelper->SetEpcHelper (epcHelper);


  /*************************Spectrum division ****************************/

  BandwidthPartInfoPtrVector allBwps;
  CcBwpCreator ccBwpCreator;
  OperationBandInfo band;

  /*
   * The configured spectrum division is:
   * |-------------- Band ------------|
   * |---------------CC0--------------|
   * |------BWP0------|------BWP1-----|
   */
  std::unique_ptr<ComponentCarrierInfo> cc0 (new ComponentCarrierInfo ());
  std::unique_ptr<BandwidthPartInfo> bwp0 (new BandwidthPartInfo ());
  std::unique_ptr<BandwidthPartInfo> bwp1 (new BandwidthPartInfo ());

  band.m_centralFrequency  = centralFrequencyBand;
  band.m_channelBandwidth = bandwidthBand;
  band.m_lowerFrequency = band.m_centralFrequency - band.m_channelBandwidth / 2;
  band.m_higherFrequency = band.m_centralFrequency + band.m_channelBandwidth / 2;

  // Component Carrier 0
  cc0->m_ccId = 0;
  cc0->m_centralFrequency = centralFrequencyCc0;
  cc0->m_channelBandwidth = bandwidthCc0;
  cc0->m_lowerFrequency = cc0->m_centralFrequency - cc0->m_channelBandwidth / 2;
  cc0->m_higherFrequency = cc0->m_centralFrequency + cc0->m_channelBandwidth / 2;

  // BWP 0
  bwp0->m_bwpId = 0;
  bwp0->m_centralFrequency = cc0->m_lowerFrequency + cc0->m_channelBandwidth / 4;
  bwp0->m_channelBandwidth = bandwidthCc0Bpw0;
  bwp0->m_lowerFrequency = bwp0->m_centralFrequency - bwp0->m_channelBandwidth / 2;
  bwp0->m_higherFrequency = bwp0->m_centralFrequency + bwp0->m_channelBandwidth / 2;

  cc0->AddBwp (std::move (bwp0));

  // BWP 1
  bwp1->m_bwpId = 1;
  bwp1->m_centralFrequency = cc0->m_higherFrequency - cc0->m_channelBandwidth / 4;
  bwp1->m_channelBandwidth = bandwidthCc0Bpw1;
  bwp1->m_lowerFrequency = bwp1->m_centralFrequency - bwp1->m_channelBandwidth / 2;
  bwp1->m_higherFrequency = bwp1->m_centralFrequency + bwp1->m_channelBandwidth / 2;

  cc0->AddBwp (std::move (bwp1));

  // Add CC to the corresponding operation band.
  band.AddCc (std::move (cc0));

  /********************* END Spectrum division ****************************/

  nrHelper->SetPathlossAttribute ("ShadowingEnabled", BooleanValue (false));
  epcHelper->SetAttribute ("S1uLinkDelay", TimeValue (MilliSeconds (0)));

  //Set gNB scheduler
  nrHelper->SetSchedulerTypeId (TypeId::LookupByName ("ns3::NrMacSchedulerTdmaRR"));

  //gNB Beamforming method
  idealBeamformingHelper->SetAttribute ("BeamformingMethod", TypeIdValue (DirectPathBeamforming::GetTypeId ()));
  
  nrHelper->InitializeOperationBand (&band);
  allBwps = CcBwpCreator::GetAllBwps ({band});

  // Antennas for all the UEs
  nrHelper->SetUeAntennaAttribute ("NumRows", UintegerValue (1)); // From SL examples
  nrHelper->SetUeAntennaAttribute ("NumColumns", UintegerValue (2)); // From SL examples
  nrHelper->SetUeAntennaAttribute ("AntennaElement", PointerValue (CreateObject<IsotropicAntennaModel> ()));

  // Antennas for all the gNbs
  nrHelper->SetGnbAntennaAttribute ("NumRows", UintegerValue (4));
  nrHelper->SetGnbAntennaAttribute ("NumColumns", UintegerValue (8));
  nrHelper->SetGnbAntennaAttribute ("AntennaElement", PointerValue (CreateObject<IsotropicAntennaModel> ()));

  //gNB bandwidth part manager setup.
  //The current algorithm multiplexes BWPs depending on the associated bearer QCI
  nrHelper->SetGnbBwpManagerAlgorithmAttribute ("GBR_CONV_VOICE", UintegerValue (0)); // The BWP index is 0 because only one BWP will be installed in the eNB

  //Install only in the BWP that will be used for in-network
  uint8_t bwpIdInNet = 0;
  BandwidthPartInfoPtrVector inNetBwp;
  inNetBwp.insert (inNetBwp.end (), band.GetBwpAt (/*CC*/ 0, bwpIdInNet));
  NetDeviceContainer inNetUeNetDev = nrHelper->InstallUeDevice (inNetUeNodes, inNetBwp);
  NetDeviceContainer enbNetDev = nrHelper->InstallGnbDevice (gNbNodes, inNetBwp);

  //Setup BWPs numerology, Tx Power and pattern
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 0)->SetAttribute ("Numerology", UintegerValue (numerologyCc0Bwp0));
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 0)->SetAttribute ("Pattern", StringValue (pattern));
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 0)->SetAttribute ("TxPower", DoubleValue (gNBtotalTxPower));

  //SL UE MAC configuration
  nrHelper->SetUeMacAttribute ("EnableSensing", BooleanValue (false));
  nrHelper->SetUeMacAttribute ("T1", UintegerValue (2));
  nrHelper->SetUeMacAttribute ("T2", UintegerValue (65));
  nrHelper->SetUeMacAttribute ("ActivePoolId", UintegerValue (0));
  nrHelper->SetUeMacAttribute ("ReservationPeriod", TimeValue (MilliSeconds (20)));
  nrHelper->SetUeMacAttribute ("NumSidelinkProcess", UintegerValue (255));
  nrHelper->SetUeMacAttribute ("EnableBlindReTx", BooleanValue (true));
  nrHelper->SetUeMacAttribute ("SlThresPsschRsrp", IntegerValue (-128));

  //SL BWP manager configuration
  uint8_t bwpIdSl = 1;
  nrHelper->SetBwpManagerTypeId (TypeId::LookupByName ("ns3::NrSlBwpManagerUe"));
  nrHelper->SetUeBwpManagerAlgorithmAttribute ("GBR_MC_PUSH_TO_TALK", UintegerValue (bwpIdSl));

  //Install both BWPs on U2N relays
  NetDeviceContainer relayUeNetDev = nrHelper->InstallUeDevice (relayUeNodes, allBwps);

  //Install both BWPs on remote UEs
  //This was needed to avoid errors with bwpId and vector indexes during device installation
  NetDeviceContainer remoteUeNetDev = nrHelper->InstallUeDevice (remoteUeNodes, allBwps );
  std::set<uint8_t> remoteUesBwpIdContainer;
  remoteUesBwpIdContainer.insert (bwpIdInNet);
  remoteUesBwpIdContainer.insert (bwpIdSl);

  //Force update configurations
  for (auto it = enbNetDev.Begin (); it != enbNetDev.End (); ++it)
    {
      DynamicCast<NrGnbNetDevice> (*it)->UpdateConfig ();
    }
  for (auto it = inNetUeNetDev.Begin (); it != inNetUeNetDev.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }
  for (auto it = relayUeNetDev.Begin (); it != relayUeNetDev.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }
  for (auto it = remoteUeNetDev.Begin (); it != remoteUeNetDev.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }

  /* Create NrSlHelper which will configure the UEs protocol stack to be ready to
   * perform Sidelink related procedures
   */
  Ptr<NrSlHelper> nrSlHelper = CreateObject <NrSlHelper> ();
  nrSlHelper->SetEpcHelper (epcHelper);

  //Set the SL error model and AMC
  std::string errorModel = "ns3::NrEesmIrT1";
  nrSlHelper->SetSlErrorModel (errorModel);
  nrSlHelper->SetUeSlAmcAttribute ("AmcModel", EnumValue (NrAmc::ErrorModel));

  //Set the SL scheduler attributes
  nrSlHelper->SetNrSlSchedulerTypeId (NrSlUeMacSchedulerSimple::GetTypeId ());
  nrSlHelper->SetUeSlSchedulerAttribute ("FixNrSlMcs", BooleanValue (true));
  nrSlHelper->SetUeSlSchedulerAttribute ("InitialNrSlMcs", UintegerValue (14));

  //Configure U2N relay UEs for SL
  std::set<uint8_t> slBwpIdContainerRelay;
  slBwpIdContainerRelay.insert (bwpIdSl);   //Only in the SL BWP for the relay UEs
  nrSlHelper->PrepareUeForSidelink (relayUeNetDev, slBwpIdContainerRelay);

  //Configure remote UEs for SL
  nrSlHelper->PrepareUeForSidelink (remoteUeNetDev, remoteUesBwpIdContainer);

  /***SL IEs configuration **/

  //SlResourcePoolNr IE
  LteRrcSap::SlResourcePoolNr slResourcePoolNr;
  //get it from pool factory
  Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory> ();
  //Configure specific parameters of interest:
  std::vector <std::bitset<1> > slBitmap = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
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
  bwp.numerology = numerologyCc0Bwp1;
  bwp.symbolsPerSlots = 14;
  bwp.rbPerRbg = 1;
  bwp.bandwidth = bandwidthCc0Bpw1/1000/100; // SL configuration requires BW in Multiple of 100 KHz

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
  for (const auto &it:remoteUesBwpIdContainer)
    {
      // it is the BWP id
      slFreConfigCommonNr.slBwpList [it] = slBwpConfigCommonNr;
    }

  //Configure the TddUlDlConfigCommon IE
  LteRrcSap::TddUlDlConfigCommon tddUlDlConfigCommon;
  tddUlDlConfigCommon.tddPattern = pattern;

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
  //For remote UEs
  nrSlHelper->InstallNrSlPreConfiguration (remoteUeNetDev, slPreConfigNr);

  //For U2N relay UEs we need to modify some parameters to configure *only*
  //BWP1 on the relay for SL and avoid MAC problems
  LteRrcSap::SlFreqConfigCommonNr slFreConfigCommonNrRelay;
  slFreConfigCommonNrRelay.slBwpList [bwpIdSl] = slBwpConfigCommonNr;

  LteRrcSap::SidelinkPreconfigNr slPreConfigNrRelay;
  slPreConfigNrRelay.slPreconfigGeneral = slPreconfigGeneralNr;
  slPreConfigNrRelay.slUeSelectedPreConfig = slUeSelectedPreConfig;
  slPreConfigNrRelay.slPreconfigFreqInfoList [0] = slFreConfigCommonNrRelay;

  nrSlHelper->InstallNrSlPreConfiguration (relayUeNetDev, slPreConfigNrRelay);

  /***END SL IEs configuration **/

  //Set random streams
  int64_t randomStream = 1;
  randomStream += nrHelper->AssignStreams (enbNetDev, randomStream);
  randomStream += nrHelper->AssignStreams (inNetUeNetDev, randomStream);
  randomStream += nrHelper->AssignStreams (relayUeNetDev, randomStream);
  randomStream += nrSlHelper->AssignStreams (relayUeNetDev, randomStream);
  randomStream += nrHelper->AssignStreams (remoteUeNetDev, randomStream);
  randomStream += nrSlHelper->AssignStreams (remoteUeNetDev, randomStream);

  // create the internet and install the IP stack on the UEs
  // get SGW/PGW and create a single RemoteHost
  Ptr<Node> pgw = epcHelper->GetPgwNode ();
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // connect a remoteHost to pgw. Setup routing too
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (2500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.000)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  std::cout << "IP configuration: " << std::endl;
  std::cout << " Remote Host: " << remoteHostAddr << std::endl;

  // Configure in-network only UEs
  internet.Install (inNetUeNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (inNetUeNetDev));
  // Set the default gateway for the in-network UEs
  for (uint32_t j = 0; j < inNetUeNodes.GetN (); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting =
        ipv4RoutingHelper.GetStaticRouting (inNetUeNodes.Get (j)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
      std::cout << " In-network only UE: " << inNetUeNodes.Get (j)->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal () << std::endl;
    }

  //Attach in-network UEs to the closest gNB
  nrHelper->AttachToClosestEnb (inNetUeNetDev, enbNetDev);

  // Configure U2N relay UEs
  internet.Install (relayUeNodes);
  Ipv4InterfaceContainer ueIpIfaceRelays;
  ueIpIfaceRelays = epcHelper->AssignUeIpv4Address (NetDeviceContainer (relayUeNetDev));
  std::vector<Ipv4Address> relaysIpv4AddressVector (relayUeNum);

  for (uint32_t u = 0; u < relayUeNodes.GetN (); ++u)
    {
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting =
        ipv4RoutingHelper.GetStaticRouting (relayUeNodes.Get (u)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

      //Obtain local IPv4 addresses that will be used to route the unicast traffic upon setup of the direct link
      relaysIpv4AddressVector [u] = relayUeNodes.Get (u)->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
      std::cout << " Relay UE: " << relaysIpv4AddressVector [u] << std::endl;
    }

  //Attach U2N relay UEs to the closest gNB
  nrHelper->AttachToClosestEnb (relayUeNetDev, enbNetDev);

  // Configure out-of-network UEs
  internet.Install (remoteUeNodes);
  Ipv4InterfaceContainer ueIpIfaceSl;
  ueIpIfaceSl = epcHelper->AssignUeIpv4Address (NetDeviceContainer (remoteUeNetDev));
  std::vector<Ipv4Address> slIpv4AddressVector (remoteUeNum);

  for (uint32_t u = 0; u < remoteUeNodes.GetN (); ++u)
    {
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting =
        ipv4RoutingHelper.GetStaticRouting (remoteUeNodes.Get (u)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

      //Obtain local IPv4 addresses that will be used to route the unicast traffic upon setup of the direct link
      slIpv4AddressVector [u] = remoteUeNodes.Get (u)->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
      std::cout << " Out-of-network UE: " << slIpv4AddressVector [u] << std::endl;
    }


  /******** Configure ProSe layer in the UEs that will do SL  **********/
  //Create ProSe helper
  Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject <NrSlProseHelper> ();
  nrSlProseHelper->SetEpcHelper (epcHelper);

  // Install ProSe layer and corresponding SAPs in the UEs
  nrSlProseHelper->PrepareUesForProse (relayUeNetDev);
  nrSlProseHelper->PrepareUesForProse (remoteUeNetDev);

  //Configure ProSe Unicast parameters. At the moment it only instruct the MAC
  //layer (and PHY therefore) to monitor packets directed the UE's own Layer 2 ID
  nrSlProseHelper->PrepareUesForUnicast (relayUeNetDev);
  nrSlProseHelper->PrepareUesForUnicast (remoteUeNetDev);

  //Configure the value of timer Timer T5080 (Prose Direct Link Establishment Request Retransmission)
  //to a lower value than the standard (8.0 s) to speed connection in shorter simulation time
  Config::SetDefault ("ns3::NrSlUeProseDirectLink::T5080", TimeValue (Seconds (2.0)));
  /******** END Configure ProSe layer in the UEs that will do SL  **********/


  /******************** L3 U2N Relay configuration ***************************/
  //-Configure relay service codes
  // Only one relay service per relay UE is currently supported
  uint32_t relayServiceCode = 5;
  std::set<uint32_t> relaySCs;
  relaySCs.insert (relayServiceCode);

  //-Configure the UL data radio bearer that the relay UE will use for U2N relaying traffic
  Ptr<EpcTft> tftRelay = Create<EpcTft> ();
  EpcTft::PacketFilter pfRelay;
  tftRelay->Add (pfRelay);
  enum EpsBearer::Qci qciRelay;
  qciRelay = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearerRelay (qciRelay);

  //Apply the configuration on the devices acting as relay UEs
  nrSlProseHelper->ConfigureL3UeToNetworkRelay (relayUeNetDev, relaySCs, bearerRelay, tftRelay);

  //Configure direct link connection between remote UEs and relay UEs
  NS_LOG_INFO ("Configuring remote UE - relay UE connection..." );
  uint32_t j = 0; //We have only one relay UE
  for (uint32_t i = 0; i < remoteUeNodes.GetN (); ++i)
    {
      nrSlProseHelper->EstablishL3UeToNetworkRelayConnection (startRelayConnTime,
                                                              remoteUeNetDev.Get (i), slIpv4AddressVector [i], // Remote UE
                                                              relayUeNetDev.Get (j), relaysIpv4AddressVector [j], // Relay UE
                                                              relayServiceCode);

      NS_LOG_INFO ("Remote UE nodeId " << remoteUeNodes.Get (i)->GetId () << 
                   " Relay UE nodeId " << relayUeNodes.Get (j)->GetId ());
    }
  /******************** END L3 U2N Relay configuration ***********************/

  /********* In-network only applications configuration ******/
  // install UDP applications
  uint16_t dlPort = 100;
  uint16_t ulPort = 200;
  ApplicationContainer clientApps, serverApps;
  // Random variable to randomize a bit start times of the client applications
  //to avoid simulation artifacts of all the TX UEs transmitting at the same time.
  Ptr<UniformRandomVariable> startTimeRnd = CreateObject<UniformRandomVariable> ();
  startTimeRnd->SetAttribute ("Min", DoubleValue (0));
  startTimeRnd->SetAttribute ("Max", DoubleValue (0.1)); //seconds
  
  Time appStartTime;

  //IN-NETWORK ONLY UEs TRAFFIC
  std::cout << "Traffic flows: " << std::endl;
  for (uint32_t u = 0; u < inNetUeNodes.GetN (); ++u)
    {
      //DL traffic
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
      serverApps.Add (dlPacketSinkHelper.Install (inNetUeNodes.Get (u)));

      UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
      dlClient.SetAttribute ("PacketSize", UintegerValue (packetSizeDlUl));
      dlClient.SetAttribute ("Interval", TimeValue (Seconds (1.0 / lambdaDl)));
      dlClient.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
      ApplicationContainer dlApp = dlClient.Install (remoteHost);
      appStartTime = Seconds (trafficStartTime + startTimeRnd->GetValue ());
      dlApp.Start (appStartTime);
      clientApps.Add (dlApp);

      std::cout << " DL: " << remoteHostAddr << " -> " << ueIpIface.GetAddress (u) << ":" << dlPort <<
        " start time: " << appStartTime.GetSeconds ()  << " s, end time: " << simTime << " s" << std::endl;

      Ptr<EpcTft> tftDl = Create<EpcTft> ();
      EpcTft::PacketFilter pfDl;
      pfDl.localPortStart = dlPort;
      pfDl.localPortEnd = dlPort;
      ++dlPort;
      tftDl->Add (pfDl);

      enum EpsBearer::Qci qDl;
      qDl = EpsBearer::GBR_CONV_VOICE;

      EpsBearer bearerDl (qDl);
      nrHelper->ActivateDedicatedEpsBearer (inNetUeNetDev.Get (u), bearerDl, tftDl);

      // UL traffic
      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
      serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

      UdpClientHelper ulClient (remoteHostAddr, ulPort);
      ulClient.SetAttribute ("PacketSize", UintegerValue (packetSizeDlUl));
      ulClient.SetAttribute ("Interval", TimeValue (Seconds (1.0 / lambdaUl)));
      ulClient.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
      ApplicationContainer ulApp = ulClient.Install (inNetUeNodes.Get (u));
      appStartTime = Seconds (trafficStartTime + startTimeRnd->GetValue ());
      ulApp.Start (appStartTime);
      clientApps.Add (ulApp);

      std::cout << " UL: " << ueIpIface.GetAddress (u)  << " -> " << remoteHostAddr << ":"  << ulPort  <<
        " start time: " << appStartTime.GetSeconds ()  << " s, end time: " << simTime << " s" << std::endl;

      Ptr<EpcTft> tftUl = Create<EpcTft> ();
      EpcTft::PacketFilter pfUl;
      pfUl.remotePortStart = ulPort;
      pfUl.remotePortEnd = ulPort;
      ++ulPort;
      tftUl->Add (pfUl);

      enum EpsBearer::Qci qUl;

      qUl = EpsBearer::GBR_CONV_VOICE;
      EpsBearer bearerUl (qUl);
      nrHelper->ActivateDedicatedEpsBearer (inNetUeNetDev.Get (u), bearerUl, tftUl);
    }

  //RELAY UE's OWN IN-NETWORK TRAFFIC
  for (uint32_t u = 0; u < relayUeNodes.GetN (); ++u)
    {
      //DL traffic
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
      serverApps.Add (dlPacketSinkHelper.Install (relayUeNodes.Get (u)));

      UdpClientHelper dlClient (ueIpIfaceRelays.GetAddress (u), dlPort);
      dlClient.SetAttribute ("PacketSize", UintegerValue (packetSizeDlUl));
      dlClient.SetAttribute ("Interval", TimeValue (Seconds (1.0 / lambdaDl)));
      dlClient.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
      ApplicationContainer dlApp = dlClient.Install (remoteHost);
      appStartTime = Seconds (trafficStartTime + startTimeRnd->GetValue ());
      dlApp.Start (appStartTime);
      clientApps.Add (dlApp);

      std::cout << " DL: " << remoteHostAddr << " -> " << ueIpIfaceRelays.GetAddress (u) << ":" << dlPort <<
        " start time: " << appStartTime.GetSeconds ()  << " s, end time: " << simTime << " s" << std::endl;

      Ptr<EpcTft> tftDl = Create<EpcTft> ();
      EpcTft::PacketFilter pfDl;
      pfDl.localPortStart = dlPort;
      pfDl.localPortEnd = dlPort;
      ++dlPort;
      tftDl->Add (pfDl);

      enum EpsBearer::Qci qDl;
      qDl = EpsBearer::GBR_CONV_VOICE;

      EpsBearer bearerDl (qDl);
      nrHelper->ActivateDedicatedEpsBearer (relayUeNetDev.Get (u), bearerDl, tftDl);

      // UL traffic
      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
      serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

      UdpClientHelper ulClient (remoteHostAddr, ulPort);
      ulClient.SetAttribute ("PacketSize", UintegerValue (packetSizeDlUl));
      ulClient.SetAttribute ("Interval", TimeValue (Seconds (1.0 / lambdaUl)));
      ulClient.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
      ApplicationContainer ulApp = ulClient.Install (relayUeNodes.Get (u));
      appStartTime = Seconds (trafficStartTime + startTimeRnd->GetValue ());
      ulApp.Start (appStartTime);
      clientApps.Add (ulApp);
      
      std::cout << " UL: " << ueIpIfaceRelays.GetAddress (u)  << " -> " << remoteHostAddr << ":"  << ulPort  <<
        " start time: " << appStartTime.GetSeconds ()  << " s, end time: " << simTime << " s" << std::endl;

      Ptr<EpcTft> tftUl = Create<EpcTft> ();
      EpcTft::PacketFilter pfUl;
      pfUl.remoteAddress = remoteHostAddr;
      pfUl.remotePortStart = ulPort;
      pfUl.remotePortEnd = ulPort;
      ++ulPort;
      tftUl->Add (pfUl);

      enum EpsBearer::Qci qUl;

      qUl = EpsBearer::GBR_CONV_VOICE;
      EpsBearer bearerUl (qUl);
      nrHelper->ActivateDedicatedEpsBearer (relayUeNetDev.Get (u), bearerUl, tftUl);
    }

  // REMOTE UEs TRAFFIC
  for (uint32_t u = 0; u < remoteUeNodes.GetN (); ++u)
    {
      //DL traffic
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
      serverApps.Add (dlPacketSinkHelper.Install (remoteUeNodes.Get (u)));

      UdpClientHelper dlClient (ueIpIfaceSl.GetAddress (u), dlPort);
      dlClient.SetAttribute ("PacketSize", UintegerValue (packetSizeDlUl));
      dlClient.SetAttribute ("Interval", TimeValue (Seconds (1.0 / lambdaDl)));
      dlClient.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
      ApplicationContainer dlApp = dlClient.Install (remoteHost);
      appStartTime = Seconds (trafficStartTime + startTimeRnd->GetValue ());
      dlApp.Start (appStartTime);
      clientApps.Add (dlApp);
      std::cout << " DL: " << remoteHostAddr << " -> " << ueIpIfaceSl.GetAddress (u) << ":" << dlPort <<
        " start time: " << appStartTime.GetSeconds ()  << " s, end time: " << simTime << " s" << std::endl;

      Ptr<EpcTft> tftDl = Create<EpcTft> ();
      EpcTft::PacketFilter pfDl;
      pfDl.localPortStart = dlPort;
      pfDl.localPortEnd = dlPort;
      ++dlPort;
      tftDl->Add (pfDl);

      enum EpsBearer::Qci qDl;
      qDl = EpsBearer::GBR_CONV_VOICE;

      EpsBearer bearerDl (qDl);
      nrHelper->ActivateDedicatedEpsBearer (remoteUeNetDev.Get (u), bearerDl, tftDl);

      // UL traffic
      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
      serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

      UdpClientHelper ulClient (remoteHostAddr, ulPort);
      ulClient.SetAttribute ("PacketSize", UintegerValue (packetSizeDlUl));
      ulClient.SetAttribute ("Interval", TimeValue (Seconds (1.0 / lambdaUl)));
      ulClient.SetAttribute ("MaxPackets", UintegerValue (0xFFFFFFFF));
      ApplicationContainer ulApp = ulClient.Install (remoteUeNodes.Get (u));
      appStartTime = Seconds (trafficStartTime + startTimeRnd->GetValue ());
      ulApp.Start (appStartTime);
      clientApps.Add (ulApp);

      std::cout << " UL: " << ueIpIfaceSl.GetAddress (u)  << " -> " << remoteHostAddr << ":"  << ulPort  <<
        " start time: " << appStartTime.GetSeconds ()  << " s, end time: " << simTime << " s" << std::endl;

      Ptr<EpcTft> tftUl = Create<EpcTft> ();
      EpcTft::PacketFilter pfUl;
      pfUl.remoteAddress = remoteHostAddr; //IMPORTANT!!!
      pfUl.remotePortStart = ulPort;
      pfUl.remotePortEnd = ulPort;
      ++ulPort;
      tftUl->Add (pfUl);

      enum EpsBearer::Qci qUl;

      qUl = EpsBearer::GBR_CONV_VOICE;
      EpsBearer bearerUl (qUl);
      nrHelper->ActivateDedicatedEpsBearer (remoteUeNetDev.Get (u), bearerUl, tftUl);
    }


  serverApps.Start (Seconds (trafficStartTime));
  serverApps.Stop (Seconds (simTime));
  clientApps.Stop (Seconds (simTime));
  /********* END In-network only applications configuration ******/

  /************ SL traces database setup *************************************/
  std::string exampleName = simTag + "-" + "nr-prose-l3-relay";
  SQLiteOutput db (exampleName + ".db");

  UePhyPscchRxOutputStats pscchPhyStats;
  pscchPhyStats.SetDb (&db, "pscchRxUePhy");
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/NrUePhy/NrSpectrumPhyList/*/RxPscchTraceUe",
                                 MakeBoundCallback (&NotifySlPscchRx, &pscchPhyStats));

  UePhyPsschRxOutputStats psschPhyStats;
  psschPhyStats.SetDb (&db, "psschRxUePhy");
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/NrUePhy/NrSpectrumPhyList/*/RxPsschTraceUe",
                                 MakeBoundCallback (&NotifySlPsschRx, &psschPhyStats));

  /************ END SL traces database setup *************************************/

  /******************* PC5-S messages tracing ********************************/
  AsciiTraceHelper ascii;
  std::string pc5s_filename = exampleName  + "-NrSlPc5SignallingPacketTrace.txt";
  Ptr<OutputStreamWrapper> Pc5SignallingPacketTraceStream = ascii.CreateFileStream (pc5s_filename.c_str ());
  *Pc5SignallingPacketTraceStream->GetStream () << "time(s)\tTX/RX\tsrcL2Id\tdstL2Id\tmsgType" << std::endl;
  for (uint32_t i = 0; i < remoteUeNetDev.GetN (); ++i)
    {
      Ptr<NrSlUeProse> prose = remoteUeNetDev.Get (i)->GetObject<NrUeNetDevice> ()->GetSlUeService ()->GetObject <NrSlUeProse> ();
      prose->TraceConnectWithoutContext ("PC5SignallingPacketTrace",
                                         MakeBoundCallback (&TraceSinkPC5SignallingPacketTrace,
                                                            Pc5SignallingPacketTraceStream));
    }
  for (uint32_t i = 0; i < relayUeNetDev.GetN (); ++i)
    {
      Ptr<NrSlUeProse> prose = relayUeNetDev.Get (i)->GetObject<NrUeNetDevice> ()->GetSlUeService ()->GetObject <NrSlUeProse> ();
      prose->TraceConnectWithoutContext ("PC5SignallingPacketTrace",
                                         MakeBoundCallback (&TraceSinkPC5SignallingPacketTrace,
                                                            Pc5SignallingPacketTraceStream));
    }
  /******************* END PC5-S messages tracing ****************************/
 
  /******************** NAS forwarding tracing *******************************/
  std::string nasRx_filename = exampleName  + "-NrSlRelayNasRxPacketTrace.txt";
  Ptr<OutputStreamWrapper> RelayNasRxPacketTraceStream = ascii.CreateFileStream (nasRx_filename.c_str ());
  *RelayNasRxPacketTraceStream->GetStream () << "time(s)\tnodeIp\tsrcIp\tdstIp\tsrcLink\tdstLink" << std::endl;
  for (uint32_t i = 0; i < relayUeNetDev.GetN (); ++i)
    {
      Ptr<EpcUeNas> epcUeNas = relayUeNetDev.Get (i)->GetObject<NrUeNetDevice> ()->GetNas ();

      epcUeNas->TraceConnectWithoutContext ("NrSlRelayRxPacketTrace",
                                            MakeBoundCallback (&TraceSinkRelayNasRxPacketTrace,
                                                               RelayNasRxPacketTraceStream));
    }
  /******************** END NAS forwarding tracing ***************************/


  //Configure FlowMonitor to get traffic flow statistics
  FlowMonitorHelper flowmonHelper;
  NodeContainer endpointNodes;
  endpointNodes.Add (remoteHost);
  endpointNodes.Add (inNetUeNodes);
  endpointNodes.Add (remoteUeNodes);
  endpointNodes.Add (relayUeNodes);

  Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install (endpointNodes);
  monitor->SetAttribute ("DelayBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("JitterBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("PacketSizeBinWidth", DoubleValue (20));

  //Run simulation
  Simulator::Stop (Seconds (simTime));
  Simulator::Run ();

  //SL database dump
  pscchPhyStats.EmptyCache ();
  psschPhyStats.EmptyCache ();

  //Print per-flow statistics
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

  std::ofstream outFile;
  std::string filename = exampleName + "-flowMonitorOutput.txt";
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

      double appDuration = simTime - trafficStartTime;

      outFile << "  Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ") " << protoStream.str () << "\n";
      outFile << "    Tx Packets: " << i->second.txPackets << "\n";
      outFile << "    Tx Bytes:   " << i->second.txBytes << "\n";
      outFile << "    TxOffered:  " << i->second.txBytes * 8.0 / appDuration / 1000 / 1000  << " Mbps\n";
      outFile << "    Rx Packets: " << i->second.rxPackets << "\n";
      outFile << "    Rx Bytes:   " << i->second.rxBytes << "\n";
      if (i->second.rxPackets > 0)
        {
          outFile << "    Throughput: " << i->second.rxBytes * 8.0 / appDuration / 1000 / 1000  << " Mbps\n";
          outFile << "    Mean delay:  " << 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets << " ms\n";
        }
      else
        {
          outFile << "    Throughput:  0 Mbps\n";
          outFile << "    Mean delay:  0 ms\n";
        }
    }
  outFile.close ();

  std::cout << "Simulation done!"  << std::endl << "Traffic flows statistics: " << std::endl;
  std::ifstream f (filename.c_str ());
  if (f.is_open ())
    {
      std::cout << f.rdbuf ();
    }
  std::cout << "Number of packets relayed by the L3 UE-to-Network relays:"  << std::endl;
  std::cout << "relayIp      srcIp->dstIp      srcLink->dstLink\t\tnPackets"  << std::endl;
  for (auto it = g_relayNasPacketCounter.begin (); it != g_relayNasPacketCounter.end (); ++it)
    {
      std::cout << it->first << "\t\t" << it->second << std::endl;
    }

  Simulator::Destroy ();
  return 0;
}


