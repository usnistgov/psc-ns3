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
 * \file nr-mcptt-operational-modes-static.cc
 *
 * This scenario illustrates the operation of the Mission Critical Push to Talk
 * (MCPTT) application over a NR system in three network contexts:
 * 1) on-network operation
 * 2) off-network operation
 * 3) on-network operation using a UE-to-Network relay to connect
 *    off-network participants
 * 
 * Topology:
 * Each team is composed of 4 UEs.
 * Team 1 is positioned outside the building, on-network, and is the baseline
 * without NR sidelink communication.
 * Team 2 is positioned inside the building, off-network over NR sidelink.
 * Team 3 has three UEs inside the building and one UE in the doorway of the
 * building, serving as UE-to-Network relay to the indoor teammates. 
 * 
 * The following is an illustration of the default topology:
 *
 *                                                            |--20m--|
 *  |-----------------------------------|-3m-||---------||--------------|
 *              400 m                             10 m          40 m
 *  -.-                -.-                               .--------------. -.-
 *   |                  |                                |  Building    |  |
 *   |                  |                                |              |  |
 *   |                 20m                               |              |  |
 *   |             .-   |    -+-             UE1         |              |  |
 *   |             |    |     3m                         |              |  |
 *   |     Team 1 <    -+-   -+-        UE0  UE2        d_1             |  |
 *  60 m           |    |     3m                         |              |  |
 *   |             '-   |    -+-             UE3         |              |  |
 *   |                  |                                |              |  |
 *   |                  |                                |              |  |
 *   |                 40m                               |              |  |
 *   |                  |                                |              |  |
 *   |                  |                                |              |  |
 *   |                  |                  .-            |       UE5    |  |
 *  -'-                 |                  |             |              | 120 m
 * eNodeB              -+-         Team 3 <             d_2 UE4  UE6    |  |
 *                      |                  |             |              |  |
 *                      |                  '-            |       UE7    |  |
 *                      |                                |              |  |
 *                      |                                |              |  |
 *                     40m                               |              |  |
 *                      |                                |              |  |
 *                      |                                |              |  |
 *                      |                  .-            |      UE9     |  |
 *                      |                  |             |              |  |
 *                     -+-         Team 2 <        UE8  d_3     UE10    |  |
 *                      |                  |             |              |  |
 *                      |                  '-            |      UE11    |  |
 *                      |                                |              |  |
 *                     20m                               :              :  :
 *                      |                                |              |  |
 *                     -+-                               '--------------' -'-
 *
 * The position of the UE-to-Network relay UE (UE 8) can be configured with the
 * parameter 'relayXInit'
 * 
 * 
 * Channel configuration:
 * This example setups a simulation using the 3GPP channel model from TR 37.885
 * and it uses the default configuration of its implementation.
 *
 * System configuration:
 * The scenario uses one operational band, containing one component carrier,
 * and two bandwidth parts. One bandwidth part is used for in-network
 * communication, i.e., UL and DL between in-network UEs and gNBs, and the
 * other bandwidth part is used for NR SL communication between UEs using SL.
 * 
 * ProSe L3 UE-to-Network relay
 * The remote UEs will start the establishment of the L3 U2N relay connection
 * before the start of the in-network traffic. This will internally start the
 * establishment of the corresponding ProSe unicast direct links with their
 * corresponding relay UEs.
 *
 * Traffic:
 * Each team starts an MCPTT call of the corresponding type (on-network for 
 * teams 1 and 2, and off-network for team 3) at time 10 seconds, which runs
 * for the rest of the simulation.
 * The simulation time can be controlled in two ways: 
 *  1. The simulation runs until a given number of accesses are reached by the
 * three calls. This is controlled by the 'minNumAccess' parameter, by setting
 * it to a value greater than zero.
 *  2. The simulation runs for a given simulation time, controlled by the 
 * 'simTime' parameter. The parameter 'minNumAccess' must be zero to use this
 * option. 
 * 
 * 
 * Output:
 * Simulation parameters and IP configurations are shown in the standard output
 * before the simulation starts. 
 * Once the simulation finish, the final traffic flows statistics are displayed
 * in the standard output for media, call control and floor control packets of 
 * the on-network calls. Finally, the number of packets relayed by the 
 * UE-to-Network relay during the simulation is displayed. 
 * Additionally, the simulation produce the following output files: 
 *  - mcptt--access-time.txt (MCPTT access time logs for all calls)
 *  - mcptt-m2e-latency.txt  (MCPTT mouth-to-ear latency logs for all calls)
 *  - mcptt-msg-stats.txt    (trace of all MCPTT messages exchanged)
 *  - mcptt-state-machine-stats.txt (trace of the state machine for all calls)
 *  - NrSlPc5SignallingPacketTrace.txt (trace of PC5 signaling used for relay 
 *                                      connection)
 *  - NrSlRelayNasRxPacketTrace.txt    (trace of all packets relayed by the 
 *                                      relay UE)
 *  - flowMonitorOutput.txt (log of the on-network traffic flows statistics)
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
#include "ns3/gnuplot.h"
#include "ns3/psc-module.h"
#include "ns3/sip-module.h"
#include <sqlite3.h>
#include "ns3/buildings-module.h"

using namespace ns3;
using namespace psc;

NS_LOG_COMPONENT_DEFINE ("NrMcpttOperationalModesStatic");

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

std::map<std::string, uint32_t> g_relayNasPacketCounter;


/*
 * \brief Trace sink function for logging reception of data packets in the NAS
 *        layer by UE(s) acting as relay UE
 *
 * \param stream the output stream wrapper where the trace will be written
 * \param node the pointer to the UE node
 * \param nodeIp the IP of the relay UE
 * \param srcIp the IP of the node sending the packet
 * \param dstIp the IP of the node that would be receiving the packet
 * \param srcLink the link from which the relay UE received the packet (UL, DL, or SL)
 * \param dstLink the link towards which the relay routed the packet (UL, DL, or SL)
 * \param p the packet
 */
void
TraceSinkRelayNasRxPacketTrace (Ptr<OutputStreamWrapper> stream,
                                Ptr<Node> node,
                                Ipv4Address nodeIp, Ipv4Address srcIp, Ipv4Address dstIp,
                                std::string srcLink, std::string dstLink, Ptr<Packet> p)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds ()
                        << "\t" << node->GetId ()
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

std::map<uint16_t, uint16_t> g_accessCounterPerCallId;

void
CountAccessTimeTrace (uint16_t minNumAccess, Time t, uint32_t userId, uint16_t callId, std::string result, Time latency)
{
  //Off network call ID is 3 in the scenario, but higher numbers in the traces.
  //This workaround works ATM
  if (callId > 2)
  {
    callId = 3;
  }

  if (result == "I" or result == "Q")
  {
    g_accessCounterPerCallId [callId] ++;
  }
  if (minNumAccess > 0)
  {
    bool reached = true;
    for (auto& a:g_accessCounterPerCallId)
    {
      if (a.second < minNumAccess)
      {
        reached = false;
      }
    }
    if (reached)
    {
      for (auto& a:g_accessCounterPerCallId)
      {
        std::cout<<"Call ID " << a.first << " Number of I or Q accesses: " << a.second << std::endl;
      }
      Simulator::Stop ();
    }
  }
}

int
main (int argc, char *argv[])
{
  //Common configuration
  double centralFrequencyBand = 5.89e9; // band n47 (From SL examples)
  double bandwidthBand = 40e6; //40 MHz
  double centralFrequencyCc0 = 5.89e9;
  double bandwidthCc0 = bandwidthBand;
  std::string pattern = "DL|DL|DL|F|UL|UL|UL|UL|UL|UL|"; // From SL examples
  double bandwidthCc0Bpw0 = bandwidthCc0 / 2;
  double bandwidthCc0Bpw1 = bandwidthCc0 / 2;
  double ueTxPower = 23; //dBm

  //In-network devices configuration
  uint16_t numerologyCc0Bwp0 = 2; // BWP0 will be used for the in-network
  double gNBtotalTxPower = 32; // dBm
  bool cellScan = false;  // Beamforming method.
  double beamSearchAngleStep = 10.0; // Beamforming parameter.

  //Sidelink configuration
  uint16_t numerologyCc0Bwp1 = 2; //(From SL examples)  BWP1 will be used for SL
  Time startRelayConnTime = Seconds (2.0); //Time to start the U2N relay connection establishment
  bool enableSensing = false;
  uint16_t rri = 0;
  uint16_t slMcs = 14;
  uint16_t slMaxTxTransNumPssch = 5;
  bool harq = false;
  uint16_t psfchPeriod = 0;

  //Topology
  uint16_t nUEsPerTeam = 4;

  double gNbHeight = 30; //meters
  double ueHeight = 1.5; //meters

  double relayXInit = 397; //X-axis position, in meters, of initial relay node

  //Traffic
  Time timeStartTraffic = Seconds (10.0);

  //Simulation configuration
  uint16_t minNumAccess = 0;
  bool showProgress = false;
  bool offNetMediaSps = false;
  bool relayMediaSps = false;

  Time simTime = Seconds (40.0); 

  CommandLine cmd;
  cmd.AddValue ("simTime", "Total duration of the simulation (s)", simTime);
  cmd.AddValue ("minNumAccess", "Minimum number of accesses the calls should have before stoping the simulation (Zero disables this mechanism)", minNumAccess);
  cmd.AddValue ("showProgress", "Show simulation progress every 10s", showProgress);
  cmd.AddValue ("enableSensing", "True if sensing is activated", enableSensing);
  cmd.AddValue ("slNumerology", "Numerology to use in SL", numerologyCc0Bwp1);
  cmd.AddValue ("rri", "Resource reservation interval for SL SPS scheduling (0 means dynamic scheduling) in miliseconds", rri);
  cmd.AddValue ("slMcs", "MCS to use in SL", slMcs);
  cmd.AddValue ("maxNTx", "Maximum number of PSSCH transmissions", slMaxTxTransNumPssch);
  cmd.AddValue ("harq", "True if HARQ is activated", harq);
  cmd.AddValue ("psfchPeriod", "PSFCH period (0: blind retransmissions, 1: PSFCH in every slot, 2: PSFCH in every other slot, 4: PSFCH in every 4 slots)", psfchPeriod);
  cmd.AddValue ("offNetMediaSps", "True if SPS scheduling for off-netwrok group media packets ", offNetMediaSps);
  cmd.AddValue ("relayMediaSps", "True if SPS scheduling for relay group media packets ", relayMediaSps);
  cmd.AddValue ("relayXInit", "X-axis position, in meters, of initial relay", relayXInit);

  cmd.Parse (argc, argv);

    if (minNumAccess > 0)
  {
    simTime = Seconds (30000.0);
  }

  if (!harq)
  {
    psfchPeriod = 0;
  }

  std::cout<<"Simulation parameters: \n"
  << "simTime: " << simTime << " nUEsPerTeam: " << nUEsPerTeam 
  << " minNumAccess: " <<minNumAccess 
  << " showProgress: " << showProgress
  << " enableSensing: " << enableSensing
  << " slNumerology: " << numerologyCc0Bwp1 
  << " rri: " << rri
  << " slMcs: " << slMcs 
  << " maxNTx: " << slMaxTxTransNumPssch
  << " harq: " << harq 
  << " psfchPeriod: " << psfchPeriod 
  << " offNetMediaSps: " << offNetMediaSps 
  << " relayMediaSps: " << relayMediaSps 
  << std::endl;

  //Setup large enough buffer size to avoid overflow
  Config::SetDefault ("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue (999999999));
  //Setup -t-Reordering to 0ms to avoid RLC reordering delay
  Config::SetDefault ("ns3::LteRlcUm::ReorderingTimer", TimeValue (MilliSeconds (0)));

  // create gNBs and UEs, configure positions
  NodeContainer gNbNodes;
  NodeContainer inNetUeNodes;
  NodeContainer relayUeNodes;
  NodeContainer remoteUeNodes;
  NodeContainer offNetUeNodes;

  MobilityHelper mobility;

  gNbNodes.Create (1);

  uint16_t nInNetUes = nUEsPerTeam;
  uint16_t nRelayUes = 1;
  uint16_t nRemoteUesPerRelay = nUEsPerTeam - 1;
  uint16_t nRemoteUes = nRelayUes * nRemoteUesPerRelay;
  uint16_t nOffNetUes = nUEsPerTeam; 

  inNetUeNodes.Create (nInNetUes);
  NS_LOG_DEBUG ("In network UE nodes; ID range: " << inNetUeNodes.Get (0)->GetId () << " to " << inNetUeNodes.Get (nInNetUes - 1)->GetId ());
  relayUeNodes.Create (nRelayUes);
  NS_LOG_DEBUG ("Relay UEs; ID range: " << relayUeNodes.Get (0)->GetId () << " to " << relayUeNodes.Get (nRelayUes - 1)->GetId ());
  remoteUeNodes.Create (nRemoteUes);
  NS_LOG_DEBUG ("Remote UEs; ID range: " << remoteUeNodes.Get (0)->GetId () << " to " << remoteUeNodes.Get (nRemoteUes - 1)->GetId ());
  offNetUeNodes.Create (nOffNetUes);
  NS_LOG_DEBUG ("Off net UEs; ID range: " << offNetUeNodes.Get (0)->GetId () << " to " << offNetUeNodes.Get (nOffNetUes - 1)->GetId ());

  //Create the building
  double uesDistanceToGnb_x = 400.0; //m
  double uesDistanceToBuilding_x = 10; //m
  double xMin = uesDistanceToGnb_x + uesDistanceToBuilding_x;
  BuildingContainer buildings;
  Ptr<Building> building1 = CreateObject<Building> ();
  building1->SetBoundaries (Box (xMin, xMin + 40, -120, 0, 0.0, 20.0));
  building1->SetBuildingType (Building::Commercial);
  building1->SetExtWallsType (Building::ConcreteWithWindows);
  NS_LOG_DEBUG ("Building bounding box (" << xMin << ", " << (xMin + 40) << ", -120, 0, 0.0, 20.0)");
  buildings.Add (building1);

  //gNB
  Ptr<ListPositionAllocator> gNbPositionAlloc = CreateObject<ListPositionAllocator> ();
  gNbPositionAlloc->Add (Vector (0.0, -60, gNbHeight));

  double xInc = 3.0 / (nUEsPerTeam - 1);
  double yInc = 6.0 / (nUEsPerTeam - 1);
  double xInit = 397;
  double yInit = -17;

  //inNet UEs
  Ptr<ListPositionAllocator> posAllocInNet = CreateObject<ListPositionAllocator> ();

  for (uint32_t inNetIdx = 0; inNetIdx < inNetUeNodes.GetN (); ++inNetIdx)
    {
      posAllocInNet->Add (Vector (xInit + inNetIdx * xInc, yInit - inNetIdx * yInc, ueHeight));
      NS_LOG_INFO ("InNet UE " << inNetIdx + 1 << " node id = [" << inNetUeNodes.Get (inNetIdx)->GetId () << "]"
                   " x " << xInit + inNetIdx * xInc << " y " << yInit - inNetIdx * yInc);
    }

  //Relay UE
  yInit = -100;
  Ptr<ListPositionAllocator> posAllocRelays = CreateObject<ListPositionAllocator> ();
  for (uint32_t ryIdx = 0; ryIdx < relayUeNodes.GetN (); ++ryIdx)
    {
      posAllocRelays->Add (Vector (relayXInit + ryIdx * xInc, yInit - ryIdx * yInc, ueHeight));

      NS_LOG_INFO ("Relay UE " << ryIdx + 1 << " node id = [" << relayUeNodes.Get (ryIdx)->GetId () << "]"
                   " x " << relayXInit + ryIdx * xInc << " y " << yInit - ryIdx * yInc);
    }
  
  //Remote UEs
  xInit = 430;
  yInit = -90;
  Ptr<ListPositionAllocator> posAllocRemotes = CreateObject<ListPositionAllocator> ();

  for (uint32_t rmIdx = 0; rmIdx < remoteUeNodes.GetN (); ++rmIdx)
    {
      posAllocRemotes->Add (Vector (xInit + rmIdx * xInc, yInit - rmIdx * yInc, ueHeight));
      NS_LOG_INFO ("Remote UE " << rmIdx << " node id = [" << remoteUeNodes.Get (rmIdx)->GetId () << "]"
                    " x " << xInit + rmIdx * xInc << " y " << yInit - rmIdx * yInc);
    }

  //offNet UEs  
  xInit = 427;
  yInit = -50;
  Ptr<ListPositionAllocator> posAllocOffNet = CreateObject<ListPositionAllocator> ();
  for (uint32_t offNetIdx = 0; offNetIdx < offNetUeNodes.GetN (); ++offNetIdx)
    {
      posAllocOffNet->Add (Vector (xInit + offNetIdx * xInc, yInit - offNetIdx * yInc, ueHeight));
      NS_LOG_INFO ("OffNet UE " << offNetIdx + 1 << " node id = [" << offNetUeNodes.Get (offNetIdx)->GetId () << "]"
                   " x " << xInit + offNetIdx * xInc << " y " << yInit - offNetIdx * yInc);
    }

  //Install mobility
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  mobility.SetPositionAllocator (gNbPositionAlloc);
  mobility.Install (gNbNodes);
  BuildingsHelper::Install (gNbNodes);

  mobility.SetPositionAllocator (posAllocInNet);
  mobility.Install (inNetUeNodes);
  BuildingsHelper::Install (inNetUeNodes);

  mobility.SetPositionAllocator (posAllocRelays);
  mobility.Install (relayUeNodes);
  BuildingsHelper::Install (relayUeNodes);

  mobility.SetPositionAllocator (posAllocRemotes);
  mobility.Install (remoteUeNodes);
  BuildingsHelper::Install (remoteUeNodes);

  mobility.SetPositionAllocator (posAllocOffNet);
  mobility.Install (offNetUeNodes);
  BuildingsHelper::Install (offNetUeNodes);

  Config::SetDefault ("ns3::NoBackhaulEpcHelper::S5LinkDelay", TimeValue (MilliSeconds (10)));
  Config::SetDefault ("ns3::NoBackhaulEpcHelper::S5LinkDataRate", DataRateValue (DataRate ("10Gb/s")));

  //Setup Helpers
  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper> ();
  epcHelper->SetAttribute ("S1uLinkMtu", UintegerValue (15000));
  epcHelper->SetAttribute ("S1uLinkDataRate", DataRateValue (DataRate ("10Gb/s")));
  epcHelper->SetAttribute ("S1uLinkDelay", TimeValue (MilliSeconds (10)));
  epcHelper->SetAttribute ("X2LinkMtu", UintegerValue (15000));
  Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
  nrHelper->SetBeamformingHelper (idealBeamformingHelper);
  nrHelper->SetEpcHelper (epcHelper);

  /*************************Spectrum division ****************************/
  BandwidthPartInfoPtrVector allBwps;
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
  bwp0->m_scenario = BandwidthPartInfo::Scenario::RMa_LoS;
  bwp1->m_scenario = BandwidthPartInfo::Scenario::RMa_LoS;

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

  //Set gNB scheduler
  nrHelper->SetSchedulerTypeId (TypeId::LookupByName ("ns3::NrMacSchedulerTdmaRR"));

  //gNB Beamforming method
  if (cellScan)
    {
      idealBeamformingHelper->SetAttribute ("BeamformingMethod", TypeIdValue (CellScanBeamforming::GetTypeId ()));
      idealBeamformingHelper->SetBeamformingAlgorithmAttribute ("BeamSearchAngleStep", DoubleValue (beamSearchAngleStep));
    }
  else
    {
      idealBeamformingHelper->SetAttribute ("BeamformingMethod", TypeIdValue (DirectPathBeamforming::GetTypeId ()));
    }

  nrHelper->InitializeOperationBand (&band);
  allBwps = CcBwpCreator::GetAllBwps ({band});

  // Antennas for all the UEs
  nrHelper->SetUeAntennaAttribute ("NumRows", UintegerValue (1)); // From SL examples
  nrHelper->SetUeAntennaAttribute ("NumColumns", UintegerValue (2)); // From SL examples
  nrHelper->SetUeAntennaAttribute ("AntennaElement", PointerValue (CreateObject<IsotropicAntennaModel> ()));

  nrHelper->SetUePhyAttribute ("TxPower", DoubleValue (ueTxPower));

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
  inNetBwp.insert (inNetBwp.end (), band.GetBwpAt (/*CC*/ 0,bwpIdInNet));
  NetDeviceContainer inNetUeNetDev = nrHelper->InstallUeDevice (inNetUeNodes, inNetBwp);
  NetDeviceContainer enbNetDev = nrHelper->InstallGnbDevice (gNbNodes, inNetBwp);

  //SL UE MAC configuration
  nrHelper->SetUeMacAttribute ("EnableSensing", BooleanValue (enableSensing));

 //Resource selection window length (given by T1 and T2) depend on the numerology
  uint16_t t2;
  switch (numerologyCc0Bwp1)
  {
    case 0:
      //t2 = 33; // with T1 = 2, this gives a window of 32 slots with mu = 0 => 32 ms
      t2 = 17; // with T1 = 2, this gives a window of 16 slots with mu = 0 => 16 ms

      break;
    case 1:
      t2 = 33; // with T1 = 2, this gives a window of 32 slots with mu = 1 => 16 ms

      break;
    case 2:
      //t2 = 33; // with T1 = 2, this gives a window of 32 slots with mu = 2 => 8 ms
      t2 = 65; // with T1 = 2, this gives a window of 64 slots with mu = 2 => 16 ms
      break;

    default:
      NS_FATAL_ERROR ("Numerology for sidelink not recognized");
      break;
  }
  NS_LOG_DEBUG ("T2: " << t2 );
  nrHelper->SetUeMacAttribute ("T1", UintegerValue (2));
  nrHelper->SetUeMacAttribute ("T2", UintegerValue (t2));
  nrHelper->SetUeMacAttribute ("ActivePoolId", UintegerValue (0));
  nrHelper->SetUeMacAttribute ("SlThresPsschRsrp", IntegerValue (-128));

  //SL BWP manager configuration
  uint8_t bwpIdSl = 1;
  nrHelper->SetBwpManagerTypeId (TypeId::LookupByName ("ns3::NrSlBwpManagerUe"));
  nrHelper->SetUeBwpManagerAlgorithmAttribute ("GBR_MC_PUSH_TO_TALK", UintegerValue (bwpIdSl));

  //Install both BWPs on U2N relays
  NetDeviceContainer relayUeNetDev = nrHelper->InstallUeDevice (relayUeNodes, allBwps);

  //Install both BWPs on SL-only UEs
  //This was needed to avoid errors with bwpId and vector indexes during device installation
  NetDeviceContainer remoteUeNetDev = nrHelper->InstallUeDevice (remoteUeNodes, allBwps );
  NetDeviceContainer offNetUeNetDev = nrHelper->InstallUeDevice (offNetUeNodes, allBwps );

  std::set<uint8_t> slBwpIdContainer;
  slBwpIdContainer.insert (bwpIdInNet);
  slBwpIdContainer.insert (bwpIdSl);

  //Setup BWPs numerology, Tx Power and pattern
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 0)->SetAttribute ("Numerology", UintegerValue (numerologyCc0Bwp0));
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 0)->SetAttribute ("Pattern", StringValue (pattern));
  nrHelper->GetGnbPhy (enbNetDev.Get (0), 0)->SetAttribute ("TxPower", DoubleValue (gNBtotalTxPower));

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
  for (auto it = offNetUeNetDev.Begin (); it != offNetUeNetDev.End (); ++it)
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
  nrSlHelper->SetNrSlSchedulerTypeId (NrSlUeMacSchedulerDefault::GetTypeId ());
  nrSlHelper->SetUeSlSchedulerAttribute ("FixNrSlMcs", BooleanValue (true));
  nrSlHelper->SetUeSlSchedulerAttribute ("InitialNrSlMcs", UintegerValue (slMcs));

  //Configure U2N relay UEs for SL
  std::set<uint8_t> slBwpIdContainerRelay;
  slBwpIdContainerRelay.insert (bwpIdSl);   //Only in the SL BWP for the relay UEs
  nrSlHelper->PrepareUeForSidelink (relayUeNetDev, slBwpIdContainerRelay);

  //Configure SL-only UEs for SL
  nrSlHelper->PrepareUeForSidelink (remoteUeNetDev, slBwpIdContainer);
  nrSlHelper->PrepareUeForSidelink (offNetUeNetDev, slBwpIdContainer);


  /***SL IEs configuration **/

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
  ptrFactory->SetSlPsfchPeriod (psfchPeriod); //0 (blind),1 (every slot),2 (every 2 slots), 4
  ptrFactory->SetSlMinTimeGapPsfch (3);
  std::list<uint16_t> resourceReservePeriodList;
  if ((offNetMediaSps || relayMediaSps) && rri != 20)
  {
    resourceReservePeriodList = {0, rri, 20}; // in ms
  }
  else
  {
    resourceReservePeriodList = {0, rri}; // in ms

  }
  ptrFactory->SetSlResourceReservePeriodList (resourceReservePeriodList);
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
  for (const auto &it:slBwpIdContainer)
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
  psschParams.slMaxTxTransNumPssch = slMaxTxTransNumPssch;
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
  //For SL-only UEs
  nrSlHelper->InstallNrSlPreConfiguration (remoteUeNetDev, slPreConfigNr);
  nrSlHelper->InstallNrSlPreConfiguration (offNetUeNetDev, slPreConfigNr);

  //For U2N relay UEs
  //We need to modify some parameters to configure *only* BWP1 on the relay for
  // SL and avoid MAC problems
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
  randomStream += nrHelper->AssignStreams (offNetUeNetDev, randomStream);
  randomStream += nrSlHelper->AssignStreams (offNetUeNetDev, randomStream);

  // create the internet and install the IP stack on the UEs
  // get SGW/PGW
  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // Create a single RemoteHost to host the MCPTT server
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  //Create the link between the Remote Host and the PGW
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);

  Ipv4AddressHelper ipv4h;
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");

  std::cout << "IP configuration: " << std::endl;

  // Configure remote host
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);
  std::cout << " Remote host (MCPTT Server): " << remoteHostAddr << std::endl;

  // Configure in-network only UEs
  internet.Install (inNetUeNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (inNetUeNetDev));
  std::vector<Ipv4Address> inNetIpv4AddressVector (nInNetUes);

  // Set the default gateway for the in-network UEs
  for (uint32_t j = 0; j < inNetUeNodes.GetN (); ++j)
    {
      Ptr<Ipv4StaticRouting> ueStaticRouting =
        ipv4RoutingHelper.GetStaticRouting (inNetUeNodes.Get (j)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
      inNetIpv4AddressVector [j] = inNetUeNodes.Get (j)->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
      std::cout << " In-network UE: " << inNetIpv4AddressVector [j] << std::endl;

    }

  //Attach in-network UEs to the closest gNB
  nrHelper->AttachToClosestEnb (inNetUeNetDev, enbNetDev);

  // Configure U2N relay UEs
  internet.Install (relayUeNodes);
  Ipv4InterfaceContainer ueIpIfaceRelays;
  ueIpIfaceRelays = epcHelper->AssignUeIpv4Address (NetDeviceContainer (relayUeNetDev));
  std::vector<Ipv4Address> relaysIpv4AddressVector (nRelayUes);

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
  //Remote UEs
  internet.Install (remoteUeNodes);
  Ipv4InterfaceContainer ueIpIfaceRemotes;
  ueIpIfaceRemotes = epcHelper->AssignUeIpv4Address (NetDeviceContainer (remoteUeNetDev));
  std::vector<Ipv4Address> remotesIpv4AddressVector (nRemoteUes);

  for (uint32_t u = 0; u < remoteUeNodes.GetN (); ++u)
    {
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting =
        ipv4RoutingHelper.GetStaticRouting (remoteUeNodes.Get (u)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

      //Obtain local IPv4 addresses that will be used to route the unicast traffic upon setup of the direct link
      remotesIpv4AddressVector [u] = remoteUeNodes.Get (u)->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
      std::cout << " Remote UE: " << remotesIpv4AddressVector [u] << std::endl;
    }

  //OffNetwork UEs
  internet.Install (offNetUeNodes);
  Ipv4InterfaceContainer ueIpIfaceOffNet;
  ueIpIfaceOffNet = epcHelper->AssignUeIpv4Address (NetDeviceContainer (offNetUeNetDev));
  std::vector<Ipv4Address> offNetIpv4AddressVector (nOffNetUes);

  for (uint32_t u = 0; u < offNetUeNodes.GetN (); ++u)
    {
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting =
        ipv4RoutingHelper.GetStaticRouting (offNetUeNodes.Get (u)->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

      //Obtain local IPv4 addresses that will be used to route the unicast traffic upon setup of the direct link
      offNetIpv4AddressVector [u] = offNetUeNodes.Get (u)->GetObject<Ipv4L3Protocol> ()->GetAddress (1,0).GetLocal ();
      std::cout << " Off-Network UE: " << offNetIpv4AddressVector [u] << std::endl;
    }
  


  /******** Configure ProSe layer in the UEs that will do Relay  **********/
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
  //Provision L3 U2N configuration on the relay UEs

  //-Configure relay service codes
  // Only one relay service per relay UE can currently be provided
  uint32_t relayServiceCode = 5;
  std::set<uint32_t> providedRelaySCs;
  providedRelaySCs.insert (relayServiceCode);

  //-Configure the UL data radio bearer that the relay UE will use for U2N relaying traffic
  Ptr<EpcTft> tftRelay = Create<EpcTft> ();
  EpcTft::PacketFilter pfRelay;
  tftRelay->Add (pfRelay);
  enum EpsBearer::Qci qciRelay;
  qciRelay = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearerRelay (qciRelay);

  //Apply the configuration on the devices acting as relay UEs
  nrSlProseHelper->ConfigureL3UeToNetworkRelay (relayUeNetDev, providedRelaySCs, bearerRelay, tftRelay);

  //Configure direct link connection between remote UEs and relay UEs
  NS_LOG_INFO ("Configuring remote UE - relay UE connection..." );
  SidelinkInfo remoteUeSlInfo;
  remoteUeSlInfo.m_castType = SidelinkInfo::CastType::Unicast;

  if (rri > 0)
  {
    remoteUeSlInfo.m_dynamic = false;
  }
  else
  {
    remoteUeSlInfo.m_dynamic = true;
  }
  remoteUeSlInfo.m_harqEnabled = harq;
  remoteUeSlInfo.m_priority = 0;
  remoteUeSlInfo.m_rri = MilliSeconds (rri);

  SidelinkInfo relayUeSlInfo;
  relayUeSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
  if (rri > 0)
  {
    relayUeSlInfo.m_dynamic = false;
  }
  else
  {
    relayUeSlInfo.m_dynamic = true;
  }
  relayUeSlInfo.m_harqEnabled = harq;
  relayUeSlInfo.m_priority = 0;
  relayUeSlInfo.m_rri = MilliSeconds (rri);
  uint32_t i = 0;
  for (uint32_t j = 0; j < relayUeNetDev.GetN (); ++j)
    {
      for (uint32_t count = 0; count < nRemoteUesPerRelay ; ++count)
        {
          nrSlProseHelper->EstablishL3UeToNetworkRelayConnection (startRelayConnTime,
                                                                  remoteUeNetDev.Get (i), remotesIpv4AddressVector [i], remoteUeSlInfo, // Remote UE
                                                                  relayUeNetDev.Get (j), relaysIpv4AddressVector [j], relayUeSlInfo, // Relay UE
                                                                  relayServiceCode);
          NS_LOG_INFO ("Remote UE nodeId " << remoteUeNodes.Get (i)->GetId () <<
                       " Relay UE nodeId " << relayUeNodes.Get (j)->GetId ());
          ++i;        
        }
    }
  /******************** END L3 U2N Relay configuration ***********************/

  //Setup SL bearers on the relay for the media traffic 
  if (relayMediaSps)
  {

    uint32_t ryDstL2Id = 5;  //5 in this scenario
    uint32_t rmDstL2Id = 6;  //Starts at 6 in this scenario
    uint16_t relayGroupMediaPort = 9004; //Starts at 4 in this scenario

    for (uint32_t rm = 0; rm < remoteUeNetDev.GetN (); ++rm)
      {
        SidelinkInfo relayUeSlInfo_media;
        relayUeSlInfo_media.m_castType = SidelinkInfo::CastType::Unicast;
        relayUeSlInfo_media.m_dstL2Id = rmDstL2Id;
        relayUeSlInfo_media.m_dynamic = false;
        relayUeSlInfo_media.m_harqEnabled = harq;
        relayUeSlInfo_media.m_priority = 0;
        relayUeSlInfo_media.m_rri = MilliSeconds (20);

        Ptr<LteSlTft> tft_media_ry;
        tft_media_ry = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT, remotesIpv4AddressVector [rm], relayGroupMediaPort, relayUeSlInfo_media);
        nrSlHelper->ActivateNrSlBearer (startRelayConnTime - MilliSeconds (50), relayUeNetDev.Get (0), tft_media_ry);

        SidelinkInfo rmUeSlInfo_media;
        rmUeSlInfo_media.m_castType = SidelinkInfo::CastType::Unicast;
        rmUeSlInfo_media.m_dstL2Id = ryDstL2Id;
        rmUeSlInfo_media.m_dynamic = false;
        rmUeSlInfo_media.m_harqEnabled = harq;
        rmUeSlInfo_media.m_priority = 0;
        rmUeSlInfo_media.m_rri = MilliSeconds (20);

        Ptr<LteSlTft> tft_media_rm;
        tft_media_rm = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT, relaysIpv4AddressVector [0], relayGroupMediaPort, rmUeSlInfo_media);
        nrSlHelper->ActivateNrSlBearer (startRelayConnTime - MilliSeconds (50), remoteUeNetDev.Get (rm), tft_media_rm);

        rmDstL2Id ++;
        relayGroupMediaPort ++;
      }
  }


  /************************ Application configuration ************************/
  
  /* In-network MCPPT application comprises McpttPttApp and McpttServerApp
   * - McpttPttApps are installed in each UE
   * - The McpttServerApp is installed in the Remote Host.
   * There are two groups of UEs in the scenario:
   * - Group 1 composed of the In-network UEs (In-network UEs and Relay UEs) and,
   * - Group 2 composed of the Remote UEs.
   * The UEs of a group participate in the same MCPTT call, hence there are two
   * in-Network calls ongoing in the scenario.
  */
  //MCPTT server configuration
  ApplicationContainer serverApps;
  psc::McpttServerHelper mcpttServerHelper;

  //Install MCPTT sever in the Remote host node
  serverApps.Add (mcpttServerHelper.Install (remoteHost));

  //Configure IP address of the MCPTT server
  Ptr<psc::McpttServerApp> serverApp = DynamicCast<psc::McpttServerApp> (serverApps.Get (0));
  Ipv4Address serverAddress = remoteHostAddr;
  serverApp->SetLocalAddress (serverAddress);

  //MCPTT clients configuration
  psc::McpttHelper mcpttClientHelper;
  mcpttClientHelper.SetPttApp ("ns3::psc::McpttPttApp");
  mcpttClientHelper.SetMediaSrc ("ns3::psc::McpttMediaSrc",
                                 "Bytes", UintegerValue (60),
                                 "DataRate", DataRateValue (DataRate ("24kb/s")));
  mcpttClientHelper.SetPusher ("ns3::psc::McpttPusher",
                               "Automatic", BooleanValue (true));
  mcpttClientHelper.SetPusherPttInterarrivalTimeVariable ("ns3::NormalRandomVariable",
                                                          "Mean", DoubleValue (10.0),
                                                          "Variance", DoubleValue (5.0));
  mcpttClientHelper.SetPusherPttDurationVariable ("ns3::NormalRandomVariable",
                                                  "Mean", DoubleValue (1.0),
                                                  "Variance", DoubleValue (1.0));

  std::cout << "InNetwork UEs traffic flows: " << std::endl;
  uint32_t groupId = 1;
  ApplicationContainer inNetClientApps;
  for (uint32_t inIdx = 0; inIdx < inNetUeNodes.GetN (); ++inIdx)
    {
      //Install MCPTT client the UE
      ApplicationContainer pttAppContainer = mcpttClientHelper.Install (inNetUeNodes.Get (inIdx));
      Ptr<psc::McpttPttApp> pttApp = pttAppContainer.Get (0)->GetObject<psc::McpttPttApp> ();
      pttApp->SetLocalAddress (inNetIpv4AddressVector[inIdx]);

      inNetClientApps.Add (pttApp);

      std::cout << " UL: " << inNetIpv4AddressVector [inIdx] << " -> " << serverAddress  << std::endl;
      std::cout << " DL: " << serverAddress << " -> " << inNetIpv4AddressVector [inIdx] << std::endl;

      //DL bearer configuration
      Ptr<EpcTft> tftDl = Create<EpcTft> ();
      EpcTft::PacketFilter pfDl;
      tftDl->Add (pfDl);
      enum EpsBearer::Qci qDl;
      qDl = EpsBearer::GBR_CONV_VOICE;
      EpsBearer bearerDl (qDl);
      nrHelper->ActivateDedicatedEpsBearer (inNetUeNetDev.Get (inIdx), bearerDl, tftDl);

      //UL bearer configuration
      Ptr<EpcTft> tftUl = Create<EpcTft> ();
      EpcTft::PacketFilter pfUl;
      pfUl.remoteAddress = serverAddress; //IMPORTANT!!!
      tftUl->Add (pfUl);
      enum EpsBearer::Qci qUl;
      qUl = EpsBearer::GBR_CONV_VOICE;
      EpsBearer bearerUl (qUl);
      nrHelper->ActivateDedicatedEpsBearer (inNetUeNetDev.Get (inIdx), bearerUl, tftUl);
    }

  ApplicationContainer relayClientApps;
  //Remote UEs
  std::cout << "Remote UEs traffic flows: " << std::endl;
  groupId = 2;
  for (uint32_t rmIdx = 0; rmIdx < remoteUeNodes.GetN (); ++rmIdx)
    {
      //Install MCPTT client the UE
      ApplicationContainer pttAppContainer = mcpttClientHelper.Install (remoteUeNodes.Get (rmIdx));
      Ptr<psc::McpttPttApp> pttApp = pttAppContainer.Get (0)->GetObject<psc::McpttPttApp> ();
      pttApp->SetLocalAddress (remotesIpv4AddressVector[rmIdx]);

      relayClientApps.Add (pttApp);

      std::cout << " UL: " << remotesIpv4AddressVector [rmIdx] << " -> " << serverAddress  << std::endl;
      std::cout << " DL: " << serverAddress << " -> " << remotesIpv4AddressVector [rmIdx] << std::endl;

      //DL bearer configuration
      Ptr<EpcTft> tftDl = Create<EpcTft> ();
      EpcTft::PacketFilter pfDl;
      tftDl->Add (pfDl);
      enum EpsBearer::Qci qDl;
      qDl = EpsBearer::GBR_CONV_VOICE;
      EpsBearer bearerDl (qDl);
      nrHelper->ActivateDedicatedEpsBearer (remoteUeNetDev.Get (rmIdx), bearerDl, tftDl);

      //UL bearer configuration
      Ptr<EpcTft> tftUl = Create<EpcTft> ();
      EpcTft::PacketFilter pfUl;
      pfUl.remoteAddress = serverAddress; //IMPORTANT!!!
      tftUl->Add (pfUl);
      enum EpsBearer::Qci qUl;
      qUl = EpsBearer::GBR_CONV_VOICE;
      EpsBearer bearerUl (qUl);
      nrHelper->ActivateDedicatedEpsBearer (remoteUeNetDev.Get (rmIdx), bearerUl, tftUl);
    }

  //Relay UEs
  std::cout << "Relay UEs traffic flows: " << std::endl;
  groupId = 2;
  for (uint32_t ryIdx = 0; ryIdx < relayUeNodes.GetN (); ++ryIdx)
    {
      //Install MCPTT client the UE
      ApplicationContainer pttAppContainer = mcpttClientHelper.Install (relayUeNodes.Get (ryIdx));
      Ptr<psc::McpttPttApp> pttApp = pttAppContainer.Get (0)->GetObject<psc::McpttPttApp> ();
      pttApp->SetLocalAddress (relaysIpv4AddressVector[ryIdx]);

      relayClientApps.Add (pttApp);

      std::cout << " UL: " << relaysIpv4AddressVector [ryIdx] << " -> " << serverAddress  << std::endl;
      std::cout << " DL: " << serverAddress << " -> " << relaysIpv4AddressVector [ryIdx] << std::endl;

      //DL bearer configuration
      Ptr<EpcTft> tftDl = Create<EpcTft> ();
      EpcTft::PacketFilter pfDl;
      tftDl->Add (pfDl);
      enum EpsBearer::Qci qDl;
      qDl = EpsBearer::GBR_CONV_VOICE;
      EpsBearer bearerDl (qDl);
      nrHelper->ActivateDedicatedEpsBearer (relayUeNetDev.Get (ryIdx), bearerDl, tftDl);

      //UL bearer configuration
      Ptr<EpcTft> tftUl = Create<EpcTft> ();
      EpcTft::PacketFilter pfUl;
      pfUl.remoteAddress = serverAddress; //IMPORTANT!!!
      tftUl->Add (pfUl);
      enum EpsBearer::Qci qUl;
      qUl = EpsBearer::GBR_CONV_VOICE;
      EpsBearer bearerUl (qUl);
      nrHelper->ActivateDedicatedEpsBearer (relayUeNetDev.Get (ryIdx), bearerUl, tftUl);
    }

  //OffNet UEs
  Ipv4Address groupAddress ("225.0.0.0");
  uint32_t groupL2Id = 255;
  std::cout << "Off-Network UEs traffic flows: " << std::endl;
  groupId = 3;
  ApplicationContainer offNetClientApps;
  for (uint32_t offIdx = 0; offIdx < offNetUeNodes.GetN (); ++offIdx)
    {
      //Install MCPTT client the UE
      ApplicationContainer pttAppContainer = mcpttClientHelper.Install (offNetUeNodes.Get (offIdx));
      Ptr<psc::McpttPttApp> pttApp = pttAppContainer.Get (0)->GetObject<psc::McpttPttApp> ();
      pttApp->SetLocalAddress (offNetIpv4AddressVector[offIdx]);

      offNetClientApps.Add (pttApp);
      for (uint32_t i = 0; i < offNetUeNodes.GetN (); ++i)
        {
          if (i == offIdx)
          {
            continue;
          }
          std::cout << " SL: " << offNetIpv4AddressVector [offIdx] << " -> " << offNetIpv4AddressVector [i]  << std::endl;
        }
      
      //SL bearer config
      Time slBearerActivationTime = Seconds (2.0); 

      //Before the other bearer activation otherwise it doesn't work
      if (offNetMediaSps)
      {
        uint16_t mediaPort = 9009;
        SidelinkInfo offNetUeSlInfo_media;
        offNetUeSlInfo_media.m_castType = SidelinkInfo::CastType::Groupcast;
        offNetUeSlInfo_media.m_dstL2Id = groupL2Id;
        offNetUeSlInfo_media.m_dynamic = false;
        offNetUeSlInfo_media.m_harqEnabled = harq;
        offNetUeSlInfo_media.m_priority = 0;
        offNetUeSlInfo_media.m_rri = MilliSeconds (20);

        Ptr<LteSlTft> tft_media;
        tft_media = Create<LteSlTft> (LteSlTft::Direction::BIDIRECTIONAL, groupAddress, mediaPort, offNetUeSlInfo_media);
        nrSlHelper->ActivateNrSlBearer (slBearerActivationTime, offNetUeNetDev, tft_media);
      }


      SidelinkInfo offNetUeSlInfo;
      offNetUeSlInfo.m_castType = SidelinkInfo::CastType::Groupcast;
      offNetUeSlInfo.m_dstL2Id = groupL2Id;
      if (rri > 0)
      {
        offNetUeSlInfo.m_dynamic = false;
      }
      else
      {
        offNetUeSlInfo.m_dynamic = true;
      }
      offNetUeSlInfo.m_harqEnabled = harq;
      offNetUeSlInfo.m_priority = 0;
      offNetUeSlInfo.m_rri = MilliSeconds (rri);

      Ptr<LteSlTft> tft;
      tft = Create<LteSlTft> (LteSlTft::Direction::BIDIRECTIONAL, groupAddress, offNetUeSlInfo);
      nrSlHelper->ActivateNrSlBearer (slBearerActivationTime, offNetUeNetDev, tft);
}

  //Configure MCPTT calls
  psc::McpttCallHelper callHelper;
  callHelper.SetArbitrator ("ns3::psc::McpttOnNetworkFloorArbitrator",
                            "AckRequired", BooleanValue (false),
                            "AudioCutIn", BooleanValue (false),
                            "DualFloorSupported", BooleanValue (false),
                            "QueueingSupported", BooleanValue (false));
  callHelper.SetTowardsParticipant ("ns3::psc::McpttOnNetworkFloorTowardsParticipant",
                                    "ReceiveOnly", BooleanValue (false));
  callHelper.SetParticipant ("ns3::psc::McpttOnNetworkFloorParticipant",
                             "AckRequired", BooleanValue (false),
                             "GenMedia", BooleanValue (true));
  callHelper.SetServerCall ("ns3::psc::McpttServerCall",
                            "AmbientListening", BooleanValue (false),
                            "TemporaryGroup", BooleanValue (false));

  std::map<uint32_t, std::vector<Ipv4Address> > ipv4addressPerGroup;
  groupId = 1;
  callHelper.AddCall (inNetClientApps, 
                      serverApp, 
                      groupId,
                      psc::McpttCallMsgFieldCallType::BASIC_GROUP,
                      timeStartTraffic, 
                      simTime - Seconds (1.0));
  
  ipv4addressPerGroup.insert (std::pair < uint32_t, std::vector<Ipv4Address> > (groupId, inNetIpv4AddressVector));

  groupId = 2;
  callHelper.AddCall (relayClientApps, 
                      serverApp, 
                      groupId,
                      psc::McpttCallMsgFieldCallType::BASIC_GROUP,
                      timeStartTraffic, 
                      simTime - Seconds (1.0));

  std::vector<Ipv4Address> group2Ipv4AddressVector (nRemoteUes + nRelayUes);
  group2Ipv4AddressVector.insert (group2Ipv4AddressVector.begin (), remotesIpv4AddressVector.begin (), remotesIpv4AddressVector. end() );
  group2Ipv4AddressVector.insert (group2Ipv4AddressVector.end (), relaysIpv4AddressVector.begin (), relaysIpv4AddressVector. end() );
  ipv4addressPerGroup.insert (std::pair < uint32_t, std::vector<Ipv4Address> > (groupId, group2Ipv4AddressVector));

  groupId = 3;
  callHelper.AddCallOffNetwork (offNetClientApps,
                                serverApp->AllocateCallId (),
                                groupAddress,
                                groupId,
                                psc::McpttCallMsgFieldCallType::BASIC_GROUP,
                                timeStartTraffic,
                                simTime - Seconds (1.0));

  ipv4addressPerGroup.insert (std::pair < uint32_t, std::vector<Ipv4Address> > (groupId, offNetIpv4AddressVector));

  ApplicationContainer allClientApps;
  allClientApps.Add (inNetClientApps);
  allClientApps.Add (relayClientApps);
  allClientApps.Add (offNetClientApps);

  allClientApps.Start (timeStartTraffic - Seconds (0.4));
  allClientApps.Stop (simTime - Seconds (1.0));
  serverApps.Start (timeStartTraffic - Seconds (0.6));
  serverApps.Stop (simTime - Seconds (1.0));

  /******************** End Application configuration ************************/

  /**************** MCPTT metrics tracing ************************************/

Ptr<McpttTraceHelper> mcpttTraceHelper = CreateObject<McpttTraceHelper> ();
  mcpttTraceHelper->EnableMsgTraces ();
  mcpttTraceHelper->EnableStateMachineTraces ();
  mcpttTraceHelper->EnableMouthToEarLatencyTrace ("mcptt-m2e-latency.txt");
  mcpttTraceHelper->EnableAccessTimeTrace ("mcptt-access-time.txt");

  //Trace to count the number of accesses and stop the simulation after all calls have at least minNumAccess accesses
  mcpttTraceHelper->TraceConnectWithoutContext ("AccessTimeTrace", MakeBoundCallback (&CountAccessTimeTrace, minNumAccess));

  /**************** END MCPTT metrics tracing ************************************/

  AsciiTraceHelper ascii;
  /******************* PC5-S messages tracing ********************************/
  Ptr<OutputStreamWrapper> Pc5SignallingPacketTraceStream = ascii.CreateFileStream ("NrSlPc5SignallingPacketTrace.txt");
  *Pc5SignallingPacketTraceStream->GetStream () << "time(s)\tnodeId\tTX/RX\tsrcL2Id\tdstL2Id\tmsgType" << std::endl;
  for (uint32_t i = 0; i < remoteUeNetDev.GetN (); ++i)
    {
      Ptr<NrSlUeProse> prose = remoteUeNetDev.Get (i)->GetObject<NrUeNetDevice> ()->GetSlUeService ()->GetObject <NrSlUeProse> ();
      prose->TraceConnectWithoutContext ("PC5SignallingPacketTrace",
                                         MakeBoundCallback (&TraceSinkPC5SignallingPacketTrace,
                                                            Pc5SignallingPacketTraceStream,
                                                            remoteUeNetDev.Get (i)->GetNode ()));
    }
  for (uint32_t i = 0; i < relayUeNetDev.GetN (); ++i)
    {
      Ptr<NrSlUeProse> prose = relayUeNetDev.Get (i)->GetObject<NrUeNetDevice> ()->GetSlUeService ()->GetObject <NrSlUeProse> ();
      prose->TraceConnectWithoutContext ("PC5SignallingPacketTrace",
                                         MakeBoundCallback (&TraceSinkPC5SignallingPacketTrace,
                                                            Pc5SignallingPacketTraceStream,
                                                            relayUeNetDev.Get (i)->GetNode ()));
    }
  /******************* END PC5-S messages tracing **************************/

  /******************* Received messages by the relay tracing **************/
  Ptr<OutputStreamWrapper> RelayNasRxPacketTraceStream = ascii.CreateFileStream ("NrSlRelayNasRxPacketTrace.txt");
  *RelayNasRxPacketTraceStream->GetStream () << "time(s)\tnodeId\tnodeIp\tsrcIp\tdstIp\tsrcLink\tdstLink" << std::endl;
  for (uint32_t i = 0; i < relayUeNetDev.GetN (); ++i)
    {
      Ptr<EpcUeNas> epcUeNas = relayUeNetDev.Get (i)->GetObject<NrUeNetDevice> ()->GetNas ();

      epcUeNas->TraceConnectWithoutContext ("NrSlRelayRxPacketTrace",
                                            MakeBoundCallback (&TraceSinkRelayNasRxPacketTrace,
                                                               RelayNasRxPacketTraceStream,
                                                               relayUeNetDev.Get (i)->GetNode ()));
    }
  /*************** END Received messages by the relay tracing **************/

  //Configure FlowMonitor to get traffic flow statistics
  FlowMonitorHelper flowmonHelper;
  NodeContainer endpointNodes;
  endpointNodes.Add (inNetUeNodes);
  endpointNodes.Add (remoteUeNodes);
  endpointNodes.Add (relayUeNodes);
  endpointNodes.Add (offNetUeNodes);
  endpointNodes.Add (remoteHost);

  Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install (endpointNodes);
  monitor->SetAttribute ("DelayBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("JitterBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("PacketSizeBinWidth", DoubleValue (20));

  //Run simulation
  if (showProgress)
    {
      ShowProgress progress (Seconds (10));
      Simulator::Stop (simTime);
      Simulator::Run ();
    }
  else
    {
      Simulator::Stop (simTime);
      Simulator::Run ();
    }
 
  //Print per-network-flow statistics
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

  std::ofstream outFile;
  std::string filename = "flowMonitorOutput.txt";
  outFile.open (filename.c_str (), std::ofstream::out | std::ofstream::trunc);
  if (!outFile.is_open ())
    {
      std::cerr << "Can't open file " << filename << std::endl;
      return 1;
    }

  outFile.setf (std::ios_base::fixed);
  for (std::map<uint32_t, std::vector<Ipv4Address> >::const_iterator itGroupAddr = ipv4addressPerGroup.begin (); itGroupAddr != ipv4addressPerGroup.end (); ++itGroupAddr)
    {
      outFile << "Group ID: " << itGroupAddr->first << "\n";
      if (itGroupAddr->first == 3)
      {
        outFile << "#Flow monitor doesn't support broadcast / multicast at the moment \n";
        continue;
      }
      outFile << "#Type\tcallId\tsrcIpd\tdstIp\tnTxPackets\tnRxPackets\tLossRatio\tMeanDelay(ms)\tMeanJitter(ms)\n";

      for (std::vector<Ipv4Address>::const_iterator itAddr = itGroupAddr->second.begin (); itAddr != itGroupAddr->second.end (); itAddr++)
        {
          //Media packets first
          for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
            {
              Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

              if (t.sourceAddress == *itAddr || t.destinationAddress == *itAddr)
                {
                  if (t.sourcePort >= 9000 && t.sourcePort < 10000 )
                    {
                      outFile << "Media" << "\t";
                    }
                  else
                    {
                      continue; //We are looking for Media packets, continue to next flow.
                    }

                  outFile << itGroupAddr->first << "\t"; //groupId
                  outFile << t.sourceAddress << ":" << t.sourcePort << "\t"
                          << t.destinationAddress << ":" << t.destinationPort << "\t";

                  outFile << i->second.txPackets << "\t";
                  outFile << i->second.rxPackets << "\t";
                  outFile << (double) (i->second.txPackets - i->second.rxPackets) / i->second.txPackets << "\t";
                  if (i->second.rxPackets > 0)
                    {
                      outFile << 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets << "\t";
                      outFile << 1000 * i->second.jitterSum.GetSeconds () / i->second.rxPackets  << "\n";
                    }
                  else
                    {
                      outFile << "0" << "\t";
                      outFile << "0" << "\n";
                    }
                }
            }
        }
      for (std::vector<Ipv4Address>::const_iterator itAddr = itGroupAddr->second.begin (); itAddr != itGroupAddr->second.end (); itAddr++)
        {
          //Floor packets next
          for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
            {
              Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

              if (t.sourceAddress == *itAddr || t.destinationAddress == *itAddr)
                {
                  if (t.sourcePort >= 11000 && t.sourcePort < 12000 )
                    {
                      outFile << "Floor" << "\t";
                    }
                  else
                    {
                      continue; //We are looking for floor control packets, continue to next flow.
                    }

                  outFile << itGroupAddr->first << "\t"; //groupId
                  outFile << t.sourceAddress << ":" << t.sourcePort << "\t"
                          << t.destinationAddress << ":" << t.destinationPort << "\t";

                  outFile << i->second.txPackets << "\t";
                  outFile << i->second.rxPackets << "\t";
                  outFile << (double) (i->second.txPackets - i->second.rxPackets) / i->second.txPackets << "\t";
 
                  if (i->second.rxPackets > 0)
                    {
                      outFile << 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets << "\t";
                      outFile << 1000 * i->second.jitterSum.GetSeconds () / i->second.rxPackets  << "\n";
                    }
                  else
                    {
                      outFile << "0" << "\t";
                      outFile << "0" << "\n";
                    }
                }
            }
        }
      for (std::vector<Ipv4Address>::const_iterator itAddr = itGroupAddr->second.begin (); itAddr != itGroupAddr->second.end (); itAddr++)
        {
          //Call packets last
          for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
            {
              Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

              if (t.sourceAddress == *itAddr || t.destinationAddress == *itAddr)
                {
                  if (t.sourcePort == 5060)
                    {
                      outFile << "Call" << "\t";
                    }
                  else
                    {
                      continue; //We are looking for call control packets, continue to next flow.
                    }

                  outFile << itGroupAddr->first << "\t"; //groupId
                  outFile << t.sourceAddress << ":" << t.sourcePort << "\t"
                          << t.destinationAddress << ":" << t.destinationPort << "\t";

                  outFile << i->second.txPackets << "\t";
                  outFile << i->second.rxPackets << "\t";
                  outFile << (double) (i->second.txPackets - i->second.rxPackets) / i->second.txPackets << "\t";
 
                  if (i->second.rxPackets > 0)
                    {
                      outFile << 1000 * i->second.delaySum.GetSeconds () / i->second.rxPackets << "\t";
                      outFile << 1000 * i->second.jitterSum.GetSeconds () / i->second.rxPackets  << "\n";
                    }
                  else
                    {
                      outFile << "0" << "\t";
                      outFile << "0" << "\n";
                    }
                }
            }
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

