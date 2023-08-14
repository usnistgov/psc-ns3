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

/*
 * This scenario illustrates MCPTT operation in three network contexts:
 * 1) on-network operation
 * 2) off-network operation
 * 3) on-network operation using a UE-to-Network relay to connect
 *    off-network participants
 *
 * The off-network communications shows how D2D/Sidelink can connect UEs
 * in a building where the signal quality from the eNodeB is poor.
 *
 * This scenario is related to the mcptt-operational-modes-mobility program except
 * that it is simplified, without mobility, and allowing the simulation
 * (and call) duration to be extended arbitrarily long.
 *
 * The topology (assuming the default of 4 UEs per team):
 *
 *                                                              |--20m--|
 *  |---------------------------------------|--3m--||----------||----------------|
 *                  400 m                                10 m          40 m
 *  -.-                    -.-                                  .----------------. -.-
 *   |                      |                                   |    Building    |  |
 *   |                      |                                   |                |  |
 *   |                     20m                                  |                |  |
 *   |                .-    |    -+-               UE1          |                |  |
 *   |                |     |     3m                            |                |  |
 *   |        Team 1 <     -+-   -+-        UE0    UE2         d_1               |  |
 *  60 m              |     |     3m                            |                |  |
 *   |                '-    |    -+-               UE3          |                |  |
 *   |                      |                                   |                |  |
 *   |                      |                                   |                |  |
 *   |                     40m                                  |                |  |
 *   |                      |                                   |                |  |
 *   |                      |                                   |                |  |
 *   |                      |                    .-             |         UE5    |  |
 *  -'-                     |                    |              |                | 120 m
 * eNodeB                  -+-           Team 2 <              d_2   UE4  UE6    |  |
 *                          |                    |              |                |  |
 *                          |                    '-             |         UE7    |  |
 *                          |                                   |                |  |
 *                          |                                   |                |  |
 *                         40m                                  |                |  |
 *                          |                                   |                |  |
 *                          |                                   |                |  |
 *                          |                    .-             |        UE9     |  |
 *                          |                    |              |                |  |
 *                         -+-           Team 3 <         UE8  d_3       UE10    |  |
 *                          |                    |              |                |  |
 *                          |                    '-             |        UE11    |  |
 *                          |                                   |                |  |
 *                         20m                                  :                :  :
 *                          |                                   |                |  |
 *                         -+-                                  '----------------' -'-
 *
 *
 * Each team is composed of a minimum of 4 (team size can be expanded)
 * stationary teams of UEs.  By default, team size is 4 UEs.
 * Team 1 is positioned outside the building, on-network, and is the baseline
 * with no D2D/Sidelink communication.
 * Team 2 is positioned inside the building, off-network over D2D/Sidelink.
 * Team 3 has three UEs inside the building and one UE in the doorway of the building,
 * serving as a gateway
 *
 * Each team starts an MCPTT call at time 10 seconds, which runs for 'callDuration'
 * seconds (default 1000 seconds; can be modified as a command line option).
 *
 * Floor control queueing is disabled by default, but can be enabled via another
 * command line option.
 *
 * Useful information is logged during the scenario configuration and can be
 * displayed in a debug build using NS_LOG="McpttOperationalModesStatic".
 *
 * The total number of transmitted and received packets are displayed using
 * the standard output upon completion of the simulation.
 *
 * Additionally, users can inspect the generated output files to gain more
 * insights about the simulation:
 *  - mcptt-operational-modes-static-access-time.dat  (access time logs)
 *  - mcptt-operational-modes-static-m2e-latency.dat  (mouth-to-ear latency logs)
 *  - mcptt-operational-modes-static-message.dat    (trace of all messages)
 *  - mcptt-operational-modes-static-pc5-signaling.dat  (trace of PC5 signaling)
 *  - mcptt-operational-modes-static-positions.dat     (log of all UE positions)
 *  - mcptt-operational-modes-static-rsrp-measurement.dat (RSRP measurement log)
 *  - mcptt-operational-modes-static-state-machine.dat  (state machine trace)
 */

#include <cfloat>
#include <sstream>
#include <iomanip>
#include "ns3/lte-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/psc-module.h"
#include "ns3/buildings-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;
using namespace psc;

NS_LOG_COMPONENT_DEFINE ("McpttOperationalModesStatic");

uint32_t RELAY_SERVICE_CODE = 33;

/**
 * Structure to store the number of received messages
 */
struct MessagesTracer
{
  void RxTrace (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType); ///< Trace sink function
  void TxTrace (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType); ///< Trace sink function

  uint32_t m_ueIdx; ///< UE index
  uint32_t m_ueNodeId; ///< Node ID
  uint32_t m_ueImsi; ///< UE IMSI
  uint32_t m_team; ///< Team
  uint32_t m_nRxMsgsOff {0}; ///< Number of messages received off-network
  uint32_t m_nTxMsgsOff {0}; ///< Number of messages transmitted off-network
  uint32_t m_nRxMsgsOn {0}; ///< Number of messages received off-network
  uint32_t m_nTxMsgsOn {0}; ///< Number of messages transmitted on-network
  std::map<std::string, int> m_stateToId;
};

void
DoPrintPositions (Ptr<OutputStreamWrapper> stream)
{
  std::ostream* os = stream->GetStream ();
  NodeContainer n = NodeContainer::GetGlobal ();
  for (uint32_t i = 0; i < n.GetN (); i++)
    {
      Ptr<MobilityModel> m = n.Get (i)->GetObject<MobilityModel> ();
      if (m)
        {
          *os << Simulator::Now ().As (Time::S) << " " << std::setw (2) << n.Get (i)->GetId ()
              << " (" << std::setw (3) << m->GetPosition ().x << ","  << std::setw (4)
              << m->GetPosition ().y << "," << std::setw (4) << m->GetPosition ().z << ")" << std::endl;
        }
    }
}

void
PrintPositionsAt (Ptr<OutputStreamWrapper> stream, Time printTime)
{
  Simulator::Schedule (printTime, &DoPrintPositions, stream);
}

/**
 * Structure for capturing server activity.
 */
struct ServerCallTracer
{
  uint16_t m_callId; //!< The ID of the call.
  Ptr<McpttOnNetworkFloorArbitrator> m_arbitrator {nullptr}; //!< The floor arbitrator.
};

/*
 * Trace sink function to count the number of received packets at the
 * application layer.
 */
void
MessagesTracer::RxTrace (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttMediaMsg::GetTypeId ())
    {
      // CallIDs 1 and 2 are allocated to the on-network and relay calls
      // CallID for the off network will be a randomly generated one
      if (callId < 3)
        {
          m_nRxMsgsOn++;
        }
      else
        {
          m_nRxMsgsOff++;
        }
    }
}

/*
 * Trace sink function to count the number of transmitted packets at the
 * application layer.
 */
void
MessagesTracer::TxTrace (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType)
{
  if (headerType == McpttMediaMsg::GetTypeId ())
    {
      // CallIDs 1 and 2 are allocated to the on-network and relay calls
      // CallID for the off network will be a randomly generated one
      if (callId < 3)
        {
          m_nTxMsgsOn++;
        }
      else
        {
          m_nTxMsgsOff++;
        }
    }
}

void
ConnectRelayService (Ptr<NetDevice> ueDevice)
{
  Ptr<LteUeRrc> rrc = ueDevice->GetObject<LteUeNetDevice> ()->GetRrc ();
  PointerValue ptrOne;
  rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
  Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();
  // If the UE is not in Remote UE role already (i.e., already monitoring
  // Discovery Relay Announcements (we are using discovery Model A))
  // then start UE-to-Network Relay Service as Remote UE
  if (!slUeRrc->IsMonitoringRelayServiceCode (LteSlDiscHeader::DISC_RELAY_ANNOUNCEMENT, RELAY_SERVICE_CODE))
    {
      NS_LOG_INFO ("Starting Relay Service as Remote UE for UE Node ID: " << ueDevice->GetNode ()->GetId ());
      rrc->StartRelayService (RELAY_SERVICE_CODE, LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);
    }
}

/*
 * Trace sink function for logging when PC5 signaling messages are received
 */
void
TraceSinkPC5SignalingPacketTrace (Ptr<OutputStreamWrapper> stream, uint32_t srcL2Id, uint32_t dstL2Id, Ptr<Packet> p)
{
  LteSlPc5SignallingMessageType lpc5smt;
  p->PeekHeader (lpc5smt);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << srcL2Id << "\t" << dstL2Id << "\t" << lpc5smt.GetMessageName () << std::endl;
}

/*
 * Trace sink for monitoring change of interface when remote/relay UEs connect
 */
void
Pc5ConnectionStatusTrace (Ptr<McpttServerApp> serverApp, uint16_t callId, Ptr<Node> node, uint32_t srcL2Id, uint32_t dstL2Id, LteSlUeRrc::RelayRole role, LteSlBasicUeController::Pc5ConnectionStatus status, uint32_t iface)
{
  NS_LOG_DEBUG ("PC5 status changed for srcL2Id " << srcL2Id << " and dstL2Id " << dstL2Id << " to " << status);
  if (status == LteSlBasicUeController::SECURE_ESTABLISHED && role == LteSlUeRrc::RelayRole::RemoteUE)
    {
      //Retrieve new IP address information
      Ptr<Ipv6> ipv6 = node->GetObject<Ipv6> ();
      Ipv6Address ipv6Addr = ipv6->GetAddress (iface, 1).GetAddress ();
      Ptr<McpttServerCall> call = serverApp->GetCall (callId);
      NS_LOG_DEBUG ("Call " << callId << " user " << node->GetApplication (0)->GetObject<McpttPttApp> ()->GetUserId () << " address " << ipv6Addr);
      if (call != nullptr)
        {
          Ptr<McpttOnNetworkFloorArbitrator> arbitrator = call->GetArbitrator ();
          Ptr<McpttOnNetworkFloorTowardsParticipant> participantFloorCtrl = arbitrator->GetParticipantByUserId (node->GetApplication (0)->GetObject<McpttPttApp> ()->GetUserId ());
          participantFloorCtrl->SetPeerAddress (ipv6Addr);
          participantFloorCtrl->Stop ();
          participantFloorCtrl->Start ();
        }
      else
        {
          NS_LOG_DEBUG ("Could not find call with ID " << callId);
        }
    }
}

int main (int argc, char *argv[])
{
  //Default values
  bool showProgress = false;
  uint32_t nUEsPerTeam = 4;
  double uesDistanceToENB_x = 400.0; //m
  double uesDistanceToBuilding_x = 10; //m
  Time relayStartTime = Seconds (5);
  Time relayConnectTime = Seconds (7);
  Time callStartTime = Seconds (10);
  Time callDuration = Seconds (1000);
  bool queueing = false;

  CommandLine cmd;
  cmd.AddValue ("nUEsPerTeam", "Number of UEs per team", nUEsPerTeam);
  cmd.AddValue ("callDuration", "Call duration", callDuration);
  cmd.AddValue ("queueing", "Whether floor queueing is enabled", queueing);
  cmd.AddValue ("showProgress", "Show simulation progress every 10s", showProgress);
  cmd.Parse (argc, argv);

  Time callStopTime = callStartTime + callDuration;
  Time simTime = callStopTime + Seconds (1);
  NS_ABORT_MSG_UNLESS (nUEsPerTeam >= 4, "At least 4 UEs per team");

  // Set on-network queueing behavior
  Config::SetDefault ("ns3::psc::McpttOnNetworkFloorArbitrator::QueueingSupported", BooleanValue (queueing));
  if (queueing)
    {
      Config::SetDefault ("ns3::psc::McpttFloorQueue::Capacity", UintegerValue (nUEsPerTeam - 1));
    }

  //Mobility
  Config::SetDefault ("ns3::WaypointMobilityModel::InitialPositionIsWaypoint", BooleanValue (true));

  // Keep file names consistent
  Config::SetDefault ("ns3::psc::McpttStateMachineStats::OutputFileName", StringValue ("mcptt-operational-modes-static-state-machine.dat"));
  Config::SetDefault ("ns3::psc::McpttMsgStats::OutputFileName", StringValue ("mcptt-operational-modes-static-message.dat"));

  //Disable control messages in MCPTT message trace
  Config::SetDefault ("ns3::psc::McpttMsgStats::CallControl", BooleanValue (false));
  Config::SetDefault ("ns3::psc::McpttMsgStats::FloorControl", BooleanValue (false));

  //TODO: We need to deactivate RLF. Otherwise we run into problems with RNTI mismatch for PC5 signaling for Remote UEs
  Config::SetDefault ("ns3::LteUePhy::EnableRlfDetection", BooleanValue (false));

  //Configure the UE for UE_SELECTED scenario
  Config::SetDefault ("ns3::LteUeMac::SlGrantMcs", UintegerValue (16));
  Config::SetDefault ("ns3::LteUeMac::SlGrantSize", UintegerValue (6)); //The number of RBs allocated per UE for Sidelink
  Config::SetDefault ("ns3::LteUeMac::Ktrp", UintegerValue (1));
  Config::SetDefault ("ns3::LteUeMac::UseSetTrp", BooleanValue (false));
  Config::SetDefault ("ns3::LteUeMac::SlScheduler", StringValue ("MaxCoverage")); //Values include Fixed, Random, MinPrb, MaxCoverage

  //Set the frequency
  Config::SetDefault ("ns3::LteEnbNetDevice::DlEarfcn", UintegerValue (5330));
  Config::SetDefault ("ns3::LteUeNetDevice::DlEarfcn", UintegerValue (5330));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlEarfcn", UintegerValue (23330));
  Config::SetDefault ("ns3::LteEnbNetDevice::DlBandwidth", UintegerValue (50));
  Config::SetDefault ("ns3::LteEnbNetDevice::UlBandwidth", UintegerValue (50));

  //Reduce frequency of CQI report to allow for sidelink transmissions
  Config::SetDefault ("ns3::LteUePhy::DownlinkCqiPeriodicity", TimeValue (MilliSeconds (79)));

  //Set the UEs power in dBm
  Config::SetDefault ("ns3::LteUePhy::TxPower", DoubleValue (23.0));
  //Set the eNBs power in dBm
  Config::SetDefault ("ns3::LteEnbPhy::TxPower", DoubleValue (30.0));

  //Timers for one-to-one communication
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4101", UintegerValue (100000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RemoteT4103", UintegerValue (100000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4103", UintegerValue (100000));
  Config::SetDefault ("ns3::LteSlO2oCommParams::RelayT4108", UintegerValue (100000));

  //Create the lte helper
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  //Create and set the EPC helper
  Config::SetDefault ("ns3::NoBackhaulEpcHelper::S5LinkDelay", TimeValue (MilliSeconds (10)));
  Config::SetDefault ("ns3::NoBackhaulEpcHelper::S5LinkDataRate", DataRateValue (DataRate ("10Gb/s")));

  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  epcHelper->SetAttribute ("S1uLinkMtu", UintegerValue (15000));
  epcHelper->SetAttribute ("S1uLinkDataRate", DataRateValue (DataRate ("10Gb/s")));
  epcHelper->SetAttribute ("S1uLinkDelay", TimeValue (MilliSeconds (10)));
  epcHelper->SetAttribute ("X2LinkMtu", UintegerValue (15000));
  lteHelper->SetEpcHelper (epcHelper);

  //Create Sidelink helper and set lteHelper
  Ptr<LteSidelinkHelper> proseHelper = CreateObject<LteSidelinkHelper> ();
  proseHelper->SetLteHelper (lteHelper);

  //Connect Sidelink controller and Sidelink Helper
  Config::SetDefault ("ns3::LteSlBasicUeController::ProseHelper", PointerValue (proseHelper));

  //Configure Relay UE (re)selection algorithm (RandomNoReselection|MaxSDRsrpNoReselection|MaxSDRsrp)
  Config::SetDefault ("ns3::LteSlBasicUeController::RelayUeSelectionAlgorithm", StringValue ("MaxSDRsrpNoReselection"));

  //Set pathloss model
  lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::HybridBuildingsPropagationLossModel"));
  lteHelper->SetPathlossModelAttribute ("ShadowSigmaExtWalls", DoubleValue (0));
  lteHelper->SetPathlossModelAttribute ("ShadowSigmaOutdoor", DoubleValue (1));
  lteHelper->SetPathlossModelAttribute ("ShadowSigmaIndoor", DoubleValue (1.5));
  lteHelper->SetSpectrumChannelType ("ns3::MultiModelSpectrumChannel");
  lteHelper->SetPathlossModelAttribute ("Frequency", DoubleValue (700e6));

  //Enable Sidelink
  lteHelper->SetAttribute ("UseSidelink", BooleanValue (true));

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);

  //Create nodes (eNb + UEs)

  NodeContainer enbNode;
  enbNode.Create (1);

  NodeContainer team1UeNodes;
  team1UeNodes.Create (nUEsPerTeam);
  NodeContainer team2UeNodes;
  team2UeNodes.Create (nUEsPerTeam);
  NodeContainer team3UeNodes;
  team3UeNodes.Create (nUEsPerTeam);

  NodeContainer allUeNodes = NodeContainer (team1UeNodes, team2UeNodes, team3UeNodes);

  //Position of the building
  double xMin = uesDistanceToENB_x + uesDistanceToBuilding_x;

  Ptr<Building> building1 = CreateObject<Building> ();
  building1->SetBoundaries (Box (xMin, xMin + 40, -120, 0, 0.0, 20.0));
  building1->SetBuildingType (Building::Commercial);
  building1->SetExtWallsType (Building::ConcreteWithWindows);
  NS_LOG_DEBUG ("Building bounding box (" << xMin << ", " << (xMin + 40) << ", -120, 0, 0.0, 20.0)");

  // Install mobility models
  Ptr<MobilityModel> mm;
  // eNodeB
  mm = CreateObject<ConstantPositionMobilityModel> ();
  mm->SetPosition (Vector (0, -60, 30));
  enbNode.Get (0)->AggregateObject (mm);
  BuildingsHelper::Install (enbNode);

  // Position UEs in small clusters; exact position is not that important
  // Team 1
  double xInc = 3.0 / (nUEsPerTeam - 1);
  double yInc = 6.0 / (nUEsPerTeam - 1);
  double xInit = 397;
  double yInit = -17;
  for (uint32_t i = 0; i < nUEsPerTeam; i++)
    {
      mm = CreateObject<ConstantPositionMobilityModel> ();
      mm->SetPosition (Vector (xInit + i * xInc, yInit - i * yInc, 1.5));
      team1UeNodes.Get (i)->AggregateObject (mm);
    }
  BuildingsHelper::Install (team1UeNodes);

  // Team 2
  xInit = 427;
  yInit = -50;
  for (uint32_t i = 0; i < nUEsPerTeam; i++)
    {
      mm = CreateObject<ConstantPositionMobilityModel> ();
      mm->SetPosition (Vector (xInit + i * xInc, yInit - i * yInc, 1.5));
      team2UeNodes.Get (i)->AggregateObject (mm);
    }
  BuildingsHelper::Install (team2UeNodes);

  // Team 3; position first node outside
  mm = CreateObject<ConstantPositionMobilityModel> ();
  mm->SetPosition (Vector (397, -100, 1.5));
  team3UeNodes.Get (0)->AggregateObject (mm);

  xInit = 430;
  yInit = -90;
  for (uint32_t i = 1; i < nUEsPerTeam; i++)
    {
      mm = CreateObject<ConstantPositionMobilityModel> ();
      mm->SetPosition (Vector (xInit + i * xInc, yInit - i * yInc, 1.5));
      team3UeNodes.Get (i)->AggregateObject (mm);
    }
  BuildingsHelper::Install (team3UeNodes);

  AsciiTraceHelper ascii;
  PrintPositionsAt (ascii.CreateFileStream ("mcptt-operational-modes-static-positions.dat"), Seconds (0));

  NS_LOG_INFO ("Configuring network parameters...");

  //Install LTE devices to the nodes
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNode);
  NetDeviceContainer team1UeDevs = lteHelper->InstallUeDevice (team1UeNodes);
  NetDeviceContainer team2UeDevs = lteHelper->InstallUeDevice (team2UeNodes);
  NetDeviceContainer team3UeDevs = lteHelper->InstallUeDevice (team3UeNodes);

  NetDeviceContainer allUeDevs = NetDeviceContainer (NetDeviceContainer (team1UeDevs, team2UeDevs), team3UeDevs);

  //Configure eNodeB for Sidelink
  Ptr<LteSlEnbRrc> enbSidelinkConfiguration = CreateObject<LteSlEnbRrc> ();
  enbSidelinkConfiguration->SetSlEnabled (true);

  //-Configure Sidelink communication pool
  enbSidelinkConfiguration->SetDefaultPool (proseHelper->GetDefaultSlCommTxResourcesSetupUeSelected ());

  //-Enable Sidelink discovery
  enbSidelinkConfiguration->SetDiscEnabled (true);

  //-Configure Sidelink discovery pool
  enbSidelinkConfiguration->AddDiscPool (proseHelper->GetDefaultSlDiscTxResourcesSetupUeSelected ());

  //-Configure UE-to-Network Relay parameters
  enbSidelinkConfiguration->SetDiscConfigRelay (proseHelper->GetDefaultSib19DiscConfigRelay ());

  //Install eNodeB Sidelink configuration on the eNodeB devices
  lteHelper->InstallSidelinkConfiguration (enbDevs, enbSidelinkConfiguration);

  //Preconfigure UEs for Sidelink
  Ptr<LteSlUeRrc> ueSidelinkConfiguration = CreateObject<LteSlUeRrc> ();
  ueSidelinkConfiguration->SetSlEnabled (true);

  //-Enable Sidelink discovery
  ueSidelinkConfiguration->SetDiscEnabled (true);
  //-Set frequency for Sidelink discovery messages monitoring
  ueSidelinkConfiguration->SetDiscInterFreq (enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ());

  //Install UE Sidelink configuration on the UE devices (only Team 2 and Team 3 will use the SL)
  //Team 3 (Empty preconf)
  LteRrcSap::SlPreconfiguration preconfigurationTeam3;
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfigurationTeam3);
  lteHelper->InstallSidelinkConfiguration (team3UeDevs, ueSidelinkConfiguration);

  //Team 2 (OOC communication preconf)
  LteRrcSap::SlPreconfiguration preconfigurationTeam2;
  //Configure general preconfiguration parameters
  preconfigurationTeam2.preconfigGeneral.carrierFreq = enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlEarfcn ();
  preconfigurationTeam2.preconfigGeneral.slBandwidth = enbDevs.Get (0)->GetObject<LteEnbNetDevice> ()->GetUlBandwidth ();
  preconfigurationTeam2.preconfigComm = proseHelper->GetDefaultSlPreconfigCommPoolList ();
  ueSidelinkConfiguration->SetSlPreconfiguration (preconfigurationTeam2);
  lteHelper->InstallSidelinkConfiguration (team2UeDevs, ueSidelinkConfiguration);

  //Install the IP stack on the UEs and assign network IP addresses
  internet.Install (allUeNodes);
  Ipv6InterfaceContainer ueIpIfaces;
  ueIpIfaces = epcHelper->AssignUeIpv6Address (allUeDevs);

  //Set the default gateway for the UEs
  Ipv6StaticRoutingHelper Ipv6RoutingHelper;
  for (uint32_t u = 0; u < allUeNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = allUeNodes.Get (u);
      Ptr<Ipv6StaticRouting> ueStaticRouting = Ipv6RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv6> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress6 (), 1);
    }

  //Configure IP for the nodes in the Internet (PGW and RemoteHost)
  Ipv6AddressHelper ipv6h;
  ipv6h.SetBase (Ipv6Address ("6001:db80::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer internetIpIfaces = ipv6h.Assign (internetDevices);
  internetIpIfaces.SetForwarding (0, true);
  internetIpIfaces.SetDefaultRouteInAllNodes (0);

  //Set route for the Remote Host to join the LTE network nodes
  Ipv6StaticRoutingHelper ipv6RoutingHelper;
  Ptr<Ipv6StaticRouting> remoteHostStaticRouting = ipv6RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv6> ());
  remoteHostStaticRouting->AddNetworkRouteTo ("7777:f000::", Ipv6Prefix (60), internetIpIfaces.GetAddress (0, 1), 1, 0);

  //Configure UE-to-Network Relay network
  proseHelper->SetIpv6BaseForRelayCommunication ("7777:f00e::", Ipv6Prefix (60));

  //Configure route between PGW and UE-to-Network Relay network
  proseHelper->ConfigurePgwToUeToNetworkRelayRoute (pgw);

  //Attach all UEs to the eNB
  lteHelper->Attach (allUeDevs);

  NS_LOG_INFO ("Configuring applications...");


  //*********** On-Network Application ************///

  //Initialize on-network MCPTT server tracer;
  std::map<uint16_t, ServerCallTracer> serverCallTracers;

  //Initialize on-network app UE Rx Packet tracer
  std::map <uint32_t, MessagesTracer> rxMessagesPerUeTracerMap;
  for (uint32_t ueNodeIdx = 0; ueNodeIdx < allUeNodes.GetN (); ueNodeIdx++)
    {
      //Calculate team to which the UE belongs
      uint32_t team = (uint32_t)(ueNodeIdx / (nUEsPerTeam)) + 1;
      MessagesTracer tracer;
      tracer.m_ueIdx = ueNodeIdx;
      tracer.m_ueNodeId = allUeNodes.Get (ueNodeIdx)->GetId ();
      tracer.m_team = team;
      tracer.m_ueImsi = allUeNodes.Get (ueNodeIdx)->GetDevice (0)->GetObject<LteUeNetDevice> ()->GetImsi ();
      rxMessagesPerUeTracerMap.insert (std::pair<uint32_t, MessagesTracer> (ueNodeIdx, tracer));
    }

  Ipv6Address serverAddr = internetIpIfaces.GetAddress (1, 1);

  ApplicationContainer serverApps;
  ApplicationContainer clientApps;

  McpttServerHelper mcpttServerHelper;
  serverApps.Add (mcpttServerHelper.Install (remoteHost));
  serverApps.Start (callStartTime - Seconds (1));
  serverApps.Stop (simTime - Seconds (1));

  Ptr<McpttServerApp> serverApp = DynamicCast<McpttServerApp> (serverApps.Get (0));
  serverApp->SetLocalAddress (serverAddr);
  NS_LOG_INFO ("server IP address = " << serverAddr);

  McpttHelper mcpttClientHelper;
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

  ApplicationContainer team1Apps = mcpttClientHelper.Install (team1UeNodes);
  ApplicationContainer team3Apps = mcpttClientHelper.Install (team3UeNodes);
  ApplicationContainer team2Apps = mcpttClientHelper.Install (team2UeNodes);

  clientApps.Add (team1Apps);
  clientApps.Add (team2Apps);
  clientApps.Add (team3Apps);

  clientApps.Start (callStartTime - Seconds (1));
  clientApps.Stop (simTime - Seconds (1));

  for (uint32_t ueNodeIdx = 0; ueNodeIdx < allUeNodes.GetN (); ueNodeIdx++)
    {
      uint32_t team = (uint32_t)(ueNodeIdx / (nUEsPerTeam)) + 1;
      Ptr<McpttPttApp> pttApp = clientApps.Get (ueNodeIdx)->GetObject<McpttPttApp> ();
      Ipv6Address clientAddress = ueIpIfaces.GetAddress (ueNodeIdx, 1);
      pttApp->SetLocalAddress (clientAddress);

      NS_LOG_INFO ("Team = " << team <<
                   " UE node id = [" << allUeNodes.Get (ueNodeIdx)->GetId () << "]"
                   " UE Idx = [" << ueNodeIdx << "]"
                   " UE IP address = " << clientAddress);

      //Set Rx trace for counting rx messages
      std::map <uint32_t, MessagesTracer>::iterator it = rxMessagesPerUeTracerMap.find (ueNodeIdx);
      pttApp->TraceConnectWithoutContext ("RxTrace", MakeCallback (&MessagesTracer::RxTrace, &it->second));
      pttApp->TraceConnectWithoutContext ("TxTrace", MakeCallback (&MessagesTracer::TxTrace, &it->second));
    }

  McpttCallHelper callHelper;
  // Optional statements to tailor the configurable attributes
  callHelper.SetArbitrator ("ns3::psc::McpttOnNetworkFloorArbitrator",
                            "AckRequired", BooleanValue (false),
                            "AudioCutIn", BooleanValue (false),
                            "DualFloorSupported", BooleanValue (false),
                            "TxSsrc", UintegerValue (100));
  callHelper.SetTowardsParticipant ("ns3::psc::McpttOnNetworkFloorTowardsParticipant",
                                    "ReceiveOnly", BooleanValue (false));
  callHelper.SetParticipant ("ns3::psc::McpttOnNetworkFloorParticipant",
                             "AckRequired", BooleanValue (false),
                             "GenMedia", BooleanValue (true));
  callHelper.SetServerCall ("ns3::psc::McpttServerCall",
                            "AmbientListening", BooleanValue (false),
                            "TemporaryGroup", BooleanValue (false));

  uint16_t callId;
  uint32_t groupId = 1;
  std::map<uint32_t, uint16_t> groupIdToOnNetworkCallId;
  McpttCallMsgFieldCallType callType = McpttCallMsgFieldCallType::BASIC_GROUP;
  callId = callHelper.AddCall (team1Apps,
                               serverApp,
                               groupId,
                               callType,
                               callStartTime,
                               callStopTime);
  groupIdToOnNetworkCallId.insert (std::pair<uint32_t, uint16_t> (groupId, callId));

  groupId = 3;
  callId = callHelper.AddCall (team3Apps,
                               serverApp,
                               groupId,
                               callType,
                               callStartTime,
                               callStopTime);
  groupIdToOnNetworkCallId.insert (std::pair<uint32_t, uint16_t> (groupId, callId));
  uint16_t team3CallId = callId;

  //*** Out-of-Network communication for TEAM 2 ***//

  //Define addressing
  uint32_t groupL2Address = 255;
  Ipv6Address groupAddress6 ("ff0e::1");     //use multicast address as destination

  for (uint32_t ueNodeIdx = 0; ueNodeIdx < team2UeNodes.GetN (); ueNodeIdx++)
    {
      team2UeNodes.Get (ueNodeIdx)->GetObject<Ipv6L3Protocol> ()->AddMulticastAddress (groupAddress6);
    }

  //Create tft for off-network applications
  Ptr<LteSlTft> tft;
  tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, groupAddress6, groupL2Address);

  //Activate SL  bearer 1.0 s before application start
  Time slBearersActivationTime = callStartTime - Seconds (1);
  proseHelper->ActivateSidelinkBearer (slBearersActivationTime, team2UeDevs, tft);

  groupId = 4;
  callHelper.AddCallOffNetwork (team2Apps,
                                serverApp->AllocateCallId (),
                                groupAddress6,
                                groupId,
                                callType,
                                callStartTime,
                                callStopTime);

  NS_LOG_INFO ("Configuring UE-to-Network Relay...");
  ///*** UE-to-Network Relay configuration for TEAM 3 only***///

  NodeContainer relayUeNode = NodeContainer (team3UeNodes.Get (0));
  NodeContainer remoteUeNodes = NodeContainer (team3UeNodes.Get (1), team3UeNodes.Get (2), team3UeNodes.Get (3));

  NetDeviceContainer relayUeDev = NetDeviceContainer (team3UeDevs.Get (0));
  NetDeviceContainer remoteUeDevs = NetDeviceContainer (NetDeviceContainer (team3UeDevs.Get (1), team3UeDevs.Get (2)), team3UeDevs.Get (3));

  //Setup dedicated bearer for the Relay UE and its (relayed) traffic
  Ptr<EpcTft> tftRelay = Create<EpcTft> ();
  EpcTft::PacketFilter dlpf;
  dlpf.localIpv6Address = proseHelper->GetIpv6NetworkForRelayCommunication ();
  dlpf.localIpv6Prefix = proseHelper->GetIpv6PrefixForRelayCommunication ();
  tftRelay->Add (dlpf);
  EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
  lteHelper->ActivateDedicatedEpsBearer (relayUeDev.Get (0), bearer, tftRelay);

  //Schedule the start of the relay service in the Relay UE
  Simulator::Schedule (relayStartTime, &LteSidelinkHelper::StartRelayService,
                       proseHelper, relayUeDev.Get (0), RELAY_SERVICE_CODE,
                       LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);

  NS_LOG_INFO ("Relay UE node id = [" << relayUeNode.Get (0)->GetId () << "]"
               " provides Service Code " << RELAY_SERVICE_CODE);

  //Connect trace in all Team's UEs for monitoring UE-to-Network Relay signaling messages
  Ptr<OutputStreamWrapper> PC5SignalingPacketTraceStream = ascii.CreateFileStream ("mcptt-operational-modes-static-pc5-signaling.dat");
  *PC5SignalingPacketTraceStream->GetStream () << "time(s)\ttxId\tRxId\tmsgType" << std::endl;
  for (uint32_t ueDevIdx = 0; ueDevIdx < team3UeDevs.GetN (); ueDevIdx++)
    {
      Ptr<LteUeRrc> rrc = team3UeDevs.Get (ueDevIdx)->GetObject<LteUeNetDevice> ()->GetRrc ();
      PointerValue ptrOne;
      rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
      Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();
      slUeRrc->TraceConnectWithoutContext ("PC5SignalingPacketTrace",
                                           MakeBoundCallback (&TraceSinkPC5SignalingPacketTrace,
                                                              PC5SignalingPacketTraceStream));
    }

  //Connect trace in the moving UEs to monitor primary cell RSRP and activate
  //relay service as Remote UE when it falls below 'threshold' (See 'RsrpMeasurementTrace' function)
  Ptr<OutputStreamWrapper> RsrpMeasurementsStream = ascii.CreateFileStream ("mcptt-operational-modes-static-rsrp-measurement.dat");
  *RsrpMeasurementsStream->GetStream () << "time(s)\timsi\tcellId\trsrp\tthreshold" << std::endl;
  std::ostringstream ctx;

  for (uint32_t ueIdx = 0; ueIdx < remoteUeNodes.GetN (); ueIdx++)
    {
      Simulator::Schedule (relayConnectTime, &ConnectRelayService, remoteUeDevs.Get (ueIdx));
      ctx << remoteUeNodes.Get (ueIdx)->GetId ();
      Ptr<LteUeRrc> rrc = remoteUeDevs.Get (ueIdx)->GetObject<LteUeNetDevice> ()->GetRrc ();
      PointerValue ptrOne;

      //connect for status change
      rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
      Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();
      PointerValue ptrTwo;
      slUeRrc->GetAttribute ("SlController", ptrTwo);
      Ptr<LteSlBasicUeController> ctlr = ptrTwo.Get<LteSlBasicUeController>();
      ctlr->TraceConnectWithoutContext ("Pc5ConnectionStatus",
                                        MakeBoundCallback (&Pc5ConnectionStatusTrace,
                                                           serverApp, team3CallId, remoteUeNodes.Get (ueIdx)));
    }

  //End scenario configuration

  Ptr<McpttTraceHelper> traceHelper = CreateObject<McpttTraceHelper> ();
  traceHelper->EnableMsgTraces ();
  traceHelper->EnableStateMachineTraces ();
  traceHelper->EnableMouthToEarLatencyTrace ("mcptt-operational-modes-static-m2e-latency.dat");
  traceHelper->EnableAccessTimeTrace ("mcptt-operational-modes-static-access-time.dat");

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

  std::map <uint32_t, MessagesTracer>::iterator it;
  std::cout << std::endl;
  std::cout << "--------------------------------------------------------------" << std::endl;
  std::cout << " Number of media packets received/transmitted by applications" << std::endl;
  std::cout << "--------------------------------------------------------------" << std::endl;
  for (it = rxMessagesPerUeTracerMap.begin (); it != rxMessagesPerUeTracerMap.end (); it++ )
    {
      std::cout << "Team: " << it->second.m_team
                << " UE idx: " << it->second.m_ueIdx
                << " UE Node ID: " << it->second.m_ueNodeId
                << " UE IMSI: " << it->second.m_ueImsi
                << " RX packets (on-network) : " << it->second.m_nRxMsgsOn
                << " TX packets (on-network) : " << it->second.m_nTxMsgsOn
                << " RX packets (off-network) : " << it->second.m_nRxMsgsOff
                << " TX packets (off-network) : " << it->second.m_nTxMsgsOff
                << std::endl;
      if (it->second.m_ueIdx % nUEsPerTeam == nUEsPerTeam - 1)
        {
          std::cout << "--------------" << std::endl;
        }
    }

  Simulator::Destroy ();

  return 0;

}
