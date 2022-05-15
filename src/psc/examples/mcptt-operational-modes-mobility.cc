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
 * This scenario is related to the mcptt-operational-modes-static program
 * except that it is contains mobility, is limited to four UEs per team,
 * and is not defined for a fixed simulation time (but the simulation time
 * is flexible-- it is derived indirectly by other configurable constants
 * in the scenario).  The program also has support for
 * a separate ns-3 visualizer (not compiled in by default).
 *
 * The topology and motion is displayed below (view in a wide terminal):
 *
 *                                                              |--20m--|
 *  |---------------------------------------|--3m--||----------||----------------|
 *                  400 m                                10 m          40 m
 *  -.-                    -.-                                  .----------------. -.-
 *   |                      |                                   |    Building    |  |
 *   |                      |                                   |                |  |
 *   |                     20m                                  |       p_1      |  | -.-
 *   |                .-    |    -+-               UE1 --->     |                |  |  |
 *   |                |     |     3m                            |                |  | 10m
 *   |        Team 1 <     -+-   -+-      >UE0<    UE2 --->    d_1      p_2      |  | -+-
 *  60 m              |     |     3m                            |                |  |  |
 *   |                '-    |    -+-               UE3 --->     |                |  | 10m
 *   |                      |                                   |       p_3      |  | -'-
 *   |                      |                                   |                |  |
 *   |                     40m                                  |                |  |
 *   |                      |                                   |                |  |
 *   |                      |                                   |       p_5      |  |
 *   |                .-    |    -+-               UE5 --->     |                |  |
 *  -'-               |     |     |                             |                | 120 m
 * eNodeB     Team 2 <     -+-   -+-     >UE4<-    UE6 --->    d_2      p_6      |  |
 *                    |     |     |                             |                |  |
 *                    '-    |    -+-               UE7 --->     |                |  |
 *                          |                                   |       p_7      |  |
 *                          |                                   |                |  |
 *                         40m                                  |                |  |
 *                          |                                   |                |  |
 *                          |                                   |       p_9      |  |
 *                    .-    |    -+-               UE9  --->    |                |  |
 *                    |     |     |                             |                |  |
 *            Team 3 <     -+-   -+-    ->UE8<-    UE10 --->   d_3       p_10    |  |
 *                    |     |     |                             |                |  |
 *                    '-    |    -+-               UE11 --->    |                |  |
 *                          |                                   |       p_11     |  |
 *                         20m                                  :                :  :
 *                          |                                   |                |  |
 *                         -+-                                  '----------------' -'-

 *
 *
 * Each team is composed of 4 UEs and 3 of them will move inside the building.
 * Each team starts their motion (intervention) at different times,
 * configurable in the scenario.  At a given (configurable) time, the moving
 * UEs of team 'i' move towards the point 'd_i' on the edge of the building
 * at constant speed, crossing the 'edge' of the building after a given
 * (configurable) time in the scenario. Then, each
 * UE 'j' move towards its corresponding point 'p_j' inside the building
 * The diagram above shows an example with the default
 * values currently in the scenario. The distance to the building
 * can be configured in the scenario and the timelines of the applications
 * adapt accordingly.  The timelines are visualized by additional ASCII
 * art embedded below in the program.
 *
 * Team 1 is the baseline scenario with no D2D/Sidelink communication. Packet
 * loss is expected for the moving UEs (UE1, UE2, and UE3) when entering
 * into the building.
 *
 * Team 2 moving UEs (UE5, UE6, and UE7) stop their in-network application when
 * reaching the building, and start a groupcast out-of-network application
 * which goes over the D2D/Sidelink.
 *
 * Team 3 static UE (UE8) starts UE-to-Network Relay services acting as a
 * Relay UE at a given time defined in the scenario. The moving UEs of the team
 * (UE9, UE10, UE11) monitor the RSRP of the primary cell of the eNodeB and
 * activate the Relay service as Remote UEs when the RSRP falls below a
 * 'threshold' defined in the scenario. Then, they proceed to discover the
 * Relay UE (UE8), connect to it, and continue their in-network application via
 * the Relay UE.
 *
 * Useful information is logged during the scenario configuration and can be
 * displayed in a debug build using NS_LOG="McpttOperationalModesMobility".
 *
 * The total number of transmitted and received application packets are
 * displayed using the standard output upon completion of the simulation.
 *
 * Additionally, users can inspect the generated output files to gain more
 * insight about the simulation.  The below output filenames are all prefixed
 * by 'mcptt-operationl-modes-mobility':
 * - access-time.dat:  MCPTT floor control access time and outcomes
 * - course-change.dat:  Mobility trace (course changes)
 * - m2e-latency.dat:  Mouth-to-ear latency trace
 * - message.dat:  Message trace
 * - pc5-signaling.dat:  Log of received PC5 signaling by the team 3 UEs
 * - rsrp-measurement.dat:  Log of primary cell RSRP monitoring by team 3 UEs
 * - state-machine.dat:  State machine trace
 * and the following file names are not prefixed (standard ns-3 LTE traces):
 * - UlMacStats.txt: MAC layer statistics for the UL
 * - UlPdcpStats.txt PDCP layer statistics for the UL
 * - DlMacStats.txt: MAC layer statistics for the DL
 * - DlPdcpStats.txt: PDCP layer statistics for the DL
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

#ifdef HAS_NETSIMULYZER
#include "ns3/netsimulyzer-module.h"
#endif

using namespace ns3;
using namespace psc;

NS_LOG_COMPONENT_DEFINE ("McpttOperationalModesMobility");

uint32_t RELAY_SERVICE_CODE = 33;

#ifdef HAS_NETSIMULYZER
/**
 * Global variables for the visualizer manager
 */
Ptr<netsimulyzer::Orchestrator> orchestrator = nullptr;
Ptr<netsimulyzer::LogStream> applicationLog = nullptr;
Ptr<netsimulyzer::LogStream> mobilityLog = nullptr;
Ptr<netsimulyzer::SeriesCollection> rsrpCollection = nullptr;
Ptr<netsimulyzer::SeriesCollection> m2eCollection = nullptr;
Ptr<netsimulyzer::SeriesCollection> accessTimeCollection = nullptr;
std::vector< Ptr<netsimulyzer::Decoration> > eNodebAntennas;
#endif

/**
 * Structure to store the number of received messages
 */
struct MessagesTracer
{
  void RxTrace (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType); ///< Trace sink function
  void TxTrace (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType); ///< Trace sink function
  void RsrpMeasurementTrace (uint64_t imsi, uint16_t cellId, double rsrp); ///< Trace sink function
  void FloorControlStateTrace (uint32_t mcpttUserId, uint16_t callId, const std::string& selected, const std::string& typeIdName, const std::string& oldStateName, const std::string& newStateName); ///< Trace sink for floor participant state change
  void PusherStateTrace (bool oldValue, bool newValue); ///< The pusher state trace.

  uint32_t m_ueIdx; ///< UE index
  uint32_t m_ueNodeId; ///< Node ID
  uint32_t m_ueImsi; ///< UE IMSI
  uint32_t m_team; ///< Team
  uint32_t m_nRxMsgsOff {0}; ///< Number of messages received off-network
  uint32_t m_nTxMsgsOff {0}; ///< Number of messages transmitted off-network
  uint32_t m_nRxMsgsOn {0}; ///< Number of messages received off-network
  uint32_t m_nTxMsgsOn {0}; ///< Number of messages transmitted on-network
  std::map<std::string, int> m_stateToId;

#ifdef HAS_NETSIMULYZER
  Ptr<netsimulyzer::XYSeries> m_nRxSeries {nullptr}; ///< Plots for number of messages received
  Ptr<netsimulyzer::XYSeries> m_mediaDelaySeries {nullptr}; ///< Plots for MCPTT media delay
  Ptr<netsimulyzer::XYSeries> m_rsrpSeries {nullptr}; ///< Plots for RSRP measurements

  Ptr<netsimulyzer::StateTransitionSink> m_floorParticipant {nullptr}; ///< Plot for in network floor control
  Ptr<netsimulyzer::StateTransitionSink> m_pusher {nullptr}; ///< Plot for pusher

  std::pair<Time, std::string> m_accessTimeInfo; ///< Information for computing access time
  Ptr<netsimulyzer::XYSeries> m_accessTimeSeries {nullptr}; ///< Plots for access time
#endif
};

/**
 * Structure for capturing server activity.
 */
struct ServerCallTracer
{
  void FloorControlStateTrace (uint32_t mcpttUserId, uint16_t callId, const std::string& selected, const std::string& typeIdName, const std::string& oldStateName, const std::string& newStateName); //!< Trace sink for floor control server state change

  uint16_t m_callId; //!< The ID of the call.
  Ptr<McpttOnNetworkFloorArbitrator> m_arbitrator {nullptr}; //!< The floor arbitrator.

#ifdef HAS_NETSIMULYZER
  Ptr<netsimulyzer::StateTransitionSink> m_floorArbitrator {nullptr}; //!< Plot for on network floor control server
#endif
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
      // CallIDs 1 and 3 are allocated to the on-network and relay calls
      // CallID for the off network will be initially 2 (on network), then a randomly generated one
      if (callId <= 3)
        {
          m_nRxMsgsOn++;
        }
      else
        {
          m_nRxMsgsOff++;
        }

#ifdef HAS_NETSIMULYZER
      McpttMediaMsg msg;
      pkt->PeekHeader (msg);
      if (m_mediaDelaySeries != nullptr)
        {
          Time timestamp = MicroSeconds (msg.GetHeader ().GetTimestamp () * 10);
          m_mediaDelaySeries->Append (Simulator::Now ().GetSeconds (), (Simulator::Now () - timestamp).GetMilliSeconds ());
        }
#endif
    }

#ifdef HAS_NETSIMULYZER
  if (m_nRxSeries != nullptr)
    {
      m_nRxSeries->Append (Simulator::Now ().GetSeconds (), (m_nRxMsgsOn + m_nRxMsgsOff));
    }
#endif
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
      // CallIDs 1 and 3 are allocated to the on-network and relay calls
      // CallID for the off network will be initially 2 (on network), then a randomly generated one
      if (callId <= 3)
        {
          m_nTxMsgsOn++;
        }
      else
        {
          m_nTxMsgsOff++;
        }
    }
}

/*
 * Trace sink function to monitor primary cell RSRP
 */
void
MessagesTracer::RsrpMeasurementTrace (uint64_t imsi, uint16_t cellId, double rsrp)
{
#ifdef HAS_NETSIMULYZER
  if (m_rsrpSeries != nullptr)
    {
      m_rsrpSeries->Append (Simulator::Now ().GetSeconds (), rsrp);
    }
#endif
}

void
MessagesTracer::FloorControlStateTrace (uint32_t mcpttUserId, uint16_t callId, const std::string& /* selected */, const std::string& typeIdName, const std::string& oldStateName, const std::string& newStateName)
{
#ifdef HAS_NETSIMULYZER
  if (m_floorParticipant != nullptr)
    {
      m_floorParticipant->StateChangedId (m_stateToId.find (newStateName)->second);

      if (m_accessTimeSeries != nullptr)
        {
          std::stringstream sstream;
          sstream.setf (std::ios::fixed);
          sstream.precision (3);
          sstream << Simulator::Now ().GetSeconds ();
          //*applicationLog << sstream.str () << " s : CallId " << callId << " UserId " << mcpttUserId << " state: " << newStateName << "\n";
          //calculate access time
          if (((oldStateName == "'Start-stop'" || oldStateName == "'U: has no permission'") && newStateName == "'U: pending Request'")
              ||(oldStateName == "'O: silence'" && newStateName == "'O: pending request'"))
            {
              m_accessTimeInfo = std::make_pair (Simulator::Now (), newStateName);
            }
          if (m_accessTimeInfo.second == "'U: pending Request'")
            {
              if (newStateName == "'U: queued'")
                {
                  m_accessTimeInfo.second = newStateName;
                  return;
                }
              else if (newStateName == "'U: has permission'")
                {
                  m_accessTimeSeries->Append (Simulator::Now ().GetSeconds (), (Simulator::Now () - m_accessTimeInfo.first).GetMilliSeconds ());
                  return;
                }
            }
          else if (m_accessTimeInfo.second == "'O: pending request'")
            {
              if (newStateName == "'O: queued'")
                {
                  m_accessTimeInfo.second = newStateName;
                  return;
                }
              else if (newStateName == "'O: has permission'")
                {
                  m_accessTimeSeries->Append (Simulator::Now ().GetSeconds (), (Simulator::Now () - m_accessTimeInfo.first).GetMilliSeconds ());
                  return;
                }
            }
          else if (m_accessTimeInfo.second == "'U: queued'")
            {
              if (newStateName == "'U: has permission'")
                {
                  m_accessTimeSeries->Append (Simulator::Now ().GetSeconds (), (Simulator::Now () - m_accessTimeInfo.first).GetMilliSeconds ());
                  return;
                }
            }
          else if (m_accessTimeInfo.second == "'O: queued'")
            {
              if (newStateName == "'O: has permission'")
                {
                  m_accessTimeSeries->Append (Simulator::Now ().GetSeconds (), (Simulator::Now () - m_accessTimeInfo.first).GetMilliSeconds ());
                  return;
                }
            }
        }
    }
#endif
}

void
MessagesTracer::PusherStateTrace (bool oldValue, bool newValue)
{
#ifdef HAS_NETSIMULYZER
  if (m_pusher != nullptr)
    {
      m_pusher->StateChangedId (newValue == false ? 0 : 1);
    }
#endif
}

void
ServerCallTracer::FloorControlStateTrace (uint32_t mcpttUserId, uint16_t callId, const std::string& /* selected */, const std::string& typeIdName, const std::string& oldStateName, const std::string& newStateName)
{
#ifdef HAS_NETSIMULYZER
  if (m_floorArbitrator == nullptr)
    return;

  int stateId;
  if (newStateName == McpttOnNetworkFloorArbitratorStateStartStop::GetStateId ().GetName ())
    {
      stateId = McpttOnNetworkFloorArbitratorStateStartStop::GetStateId ().GetNumber ();
    }
  else if (newStateName == McpttOnNetworkFloorArbitratorStateIdle::GetStateId ().GetName ())
    {
      stateId = McpttOnNetworkFloorArbitratorStateIdle::GetStateId ().GetNumber ();
    }
  else if (newStateName == McpttOnNetworkFloorArbitratorStateTaken::GetStateId ().GetName ())
    {
      stateId = McpttOnNetworkFloorArbitratorStateTaken::GetStateId ().GetNumber ();
    }
  else if (newStateName == McpttOnNetworkFloorArbitratorStateRevoke::GetStateId ().GetName ())
    {
      stateId = McpttOnNetworkFloorArbitratorStateRevoke::GetStateId ().GetNumber ();
    }
  else if (newStateName == McpttOnNetworkFloorArbitratorStateReleasing::GetStateId ().GetName ())
    {
      stateId = McpttOnNetworkFloorArbitratorStateReleasing::GetStateId ().GetNumber ();
    }
  else if (newStateName == McpttOnNetworkFloorArbitratorStateInitialising::GetStateId ().GetName ())
    {
      stateId = McpttOnNetworkFloorArbitratorStateInitialising::GetStateId ().GetNumber ();
    }
  else
    {
      NS_ABORT_MSG ("Not able to recognize floor arbitrator state.");
    }

  m_floorArbitrator->StateChangedId (stateId);

#endif
}

/*
 * Trace sink function to monitor primary cell RSRP and activate UE-to-Network
 * Relay service as Remote UE when the RSRP falls below the parameter 'threshold'
 */
void
RemoteRsrpMeasurementTrace (Ptr<OutputStreamWrapper> stream, const double threshold, Ptr<NetDevice> ueDevice, std::string context, uint64_t imsi, uint16_t cellId, double rsrp)
{
  std::ostringstream oss;
  stream->GetStream ()->precision (6);
  *stream->GetStream () << Simulator::Now ().GetNanoSeconds () / (double) 1e9 << "\t"
                        << imsi << "\t"
                        << cellId << "\t"
                        << rsrp << "\t"
                        << threshold << "\t"
                        << std::endl;

  Ptr<LteUeRrc> rrc = ueDevice->GetObject<LteUeNetDevice> ()->GetRrc ();
  PointerValue ptrOne;
  rrc->GetAttribute ("SidelinkConfiguration", ptrOne);
  Ptr<LteSlUeRrc> slUeRrc = ptrOne.Get<LteSlUeRrc> ();

  //Avoid that in-coverage Remote UE starts Relay service before cell
  //attachment (happening at the first RSRP report: 200 ms)
  if (Simulator::Now ().GetMilliSeconds () < 400)
    {
      return;
    }
  else
    {
      //If the RSRP of the primary cell is below the threshold and
      //the UE is not in Remote UE role already (i.e., already monitoring
      //Discovery Relay Announcements (we are using discovery Model A))
      //then start UE-to-Network Relay Service as Remote UE
      if (rsrp < threshold && !slUeRrc->IsMonitoringRelayServiceCode (LteSlDiscHeader::DISC_RELAY_ANNOUNCEMENT, RELAY_SERVICE_CODE))
        {
          NS_LOG_INFO ("Starting Relay Service as Remote UE for UE Node ID: " << ueDevice->GetNode ()->GetId () << " IMSI: " << imsi);
#ifdef HAS_NETSIMULYZER
          if (mobilityLog != nullptr)
            {
              std::stringstream sstream;
              sstream.setf (std::ios::fixed);
              sstream.precision (3);
              sstream << Simulator::Now ().GetSeconds ();
              *mobilityLog << sstream.str () << " s: Starting Relay Service as Remote UE for UE Node ID: " << ueDevice->GetNode ()->GetId () << "\n";
            }
#endif
          rrc->StartRelayService (RELAY_SERVICE_CODE, LteSlUeRrc::ModelA, LteSlUeRrc::RemoteUE);
        }
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
          //Address currentAddr = participantFloorCtrl->GetPeerAddress ();
          //Inet6SocketAddress socketAddr = Inet6SocketAddress::ConvertFrom (participantFloorCtrl->GetPeerAddress ());
          //socketAddr.SetIpv6 (ipv6Addr);
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
  bool mobility = true;
  uint32_t nUEsPerTeam = 4;
  double threshold = -124; //dBm
  double uesDistanceToENB_x = 400.0; //m
  double uesDistanceToBuilding_x = 10; //m
  bool queueing = false;
  bool enableVisualization = false;
  int guiResolution = 200; //refresh time in ms

  CommandLine cmd;

  cmd.AddValue ("enableGuiTraces", "Flag to enable the NetSimulyzer traces", enableVisualization);
  cmd.AddValue ("guiResolution", "Mobility granularity of the visualization", guiResolution);
  cmd.AddValue ("queueing", "Whether floor queueing is enabled", queueing);

  cmd.Parse (argc, argv);

  // Set on-network queueing behavior
  Config::SetDefault ("ns3::psc::McpttOnNetworkFloorArbitrator::QueueingSupported", BooleanValue (queueing));
  if (queueing)
    {
      Config::SetDefault ("ns3::psc::McpttFloorQueue::Capacity", UintegerValue (3));
    }

#ifndef HAS_NETSIMULYZER
  if (enableVisualization)
    {
      std::cout << "Cannot enable visualization traces because netsimulyzer module not enabled" << std::endl;
      return -1;
    }
#endif

  NS_LOG_INFO ("Calculating timelines ...");

  /*
   * EVENTS TIMELINE AND DEFINED/CALCULATED TIME QUANTITIES FOR EACH TEAM:
   *
   ********************************************************************************************************************************
   * TEAM 1:
   *********
   *                                         team1StartMoving          enteringBuildingTime    stopMoving
   *                                                 V                         V                  V
   *  --------|--------------------------|-----------|-------------------------|-----------------|~|------------|------------->
   *          ^                          ^                                                                      ^             t
   *  team1StartTime           inNetworkAppStartTime                                                   inNetworkAppStopTime
   *          |--------team1startDelayMovement-------|
   *          |--startDelayInNetworkApp--|----------------- durationTimeInNetworkApp----------------------------|
   *                                                 |-------------durationTimeInNetworkAppWhileMoving----------|
   *                                                 |--delayToReachBuilding---|
   *                                                                           |------------------|
   *                                                                        durationMovementInsideBuilding
   *
   ********************************************************************************************************************************
   * TEAM 2:
   *********
   *                                         team2StartMoving          enteringBuildingTime    stopMoving
   *                                                 V                         V                  V
   *  --------|--------------------------|-----------|-------------------------|-----------------|~|------------|------------->
   *          ^                          ^                                     ^                                ^             t
   *  team2StartTime           inNetworkAppStartTime                 inNetworkAppStopTime                       ^
   *                                                                outOfNetworkAppStartTime         outOfNetworkAppStopTime
   *          |--------team2startDelayMovement-------|
   *          |--startDelayInNetworkApp--|
   *                                     |----team2durationTimeInNetworkApp----|
   *          |-------------------startDelayOutOfNetworkApp--------------------|---durationTimeOutOfNetworkApp--|
   *                                                 |--delayToReachBuilding---|
   *                                                                           |------------------|
   *                                                                        durationMovementInsideBuilding
   *
   ********************************************************************************************************************************
   * TEAM 3:
   *********
   *                                         team3StartMoving          enteringBuildingTime    stopMoving
   *                                                 V                         V                  V
   *  --------|--------------------------|-----------|----------------|--------|--------|~|------|~|------------|------------->
   *          ^                          ^                            ^                  ^                      ^             t
   *  team3StartTime         inNetworkAppStartTime                 RelayUE           RemoteUEs          inNetworkAppStopTime
   *                                                         StartsRelayService  StartRelayService
   *          |--------team3startDelayMovement-------|
   *          |--startDelayInNetworkApp--|----------------- durationTimeInNetworkApp----------------------------|
   *                                                 |-------------durationTimeInNetworkAppWhileMoving----------|
   *          |--------------startDelayRelayServiceRelayUE------------|---1s----|
   *                                                 |--delayToReachBuilding----|
   *                                                                            |------------------|
   *                                                                        durationMovementInsideBuilding
   *
   ********************************************************************************************************************************
   *
   */

  //Set team's starting times:
  double team1StartTime = 15.0; //s
  double team2StartTime = 15.0; //s
  double team3StartTime = 15.0; //s

  //Set team's movement starting delay:
  double team1startDelayMovement = 15.0; //s
  double team2startDelayMovement = 60.0; //s
  double team3startDelayMovement = 105.0; //s

  //Define the time the UEs will take to reach the building
  double delayToReachBuilding = 15.0;  // s
  //double delayToReachBuilding = uesDistanceToBuilding_x / uesSpeed_x;  // s

  //Define for how long the UEs continue moving inside the building
  double durationMovementInsideBuilding = 30.0; //s

  //Define Application times:
  //All Teams
  double startDelayInNetworkApp = 0.0; //s
  //double durationTimeInNetworkApp = 60.0; //s (Depends on the team...)
  double durationTimeInNetworkAppWhileMoving = 45.0; //s

  //Calculate Team 2 (moving UEs) specific times and durations
  double team2durationTimeInNetworkApp = delayToReachBuilding + (team2startDelayMovement - startDelayInNetworkApp); //s
  double durationTimeOutOfNetworkApp = durationMovementInsideBuilding; //s
  double startDelayOutOfNetworkApp = team2startDelayMovement + delayToReachBuilding; //s

  //Calculate Team 3 (static UE / Relay UE) specific time
  double startDelayRelayServiceRelayUE = team3startDelayMovement + delayToReachBuilding  - 1.0; //s

  //Calculate simTime depending on last team's start time and corresponding timing
  double simTime = team3StartTime + team3startDelayMovement + delayToReachBuilding + durationMovementInsideBuilding + 1.0; //s


  NS_LOG_INFO ("Configuring default parameters...");

  //Mobility
  Config::SetDefault ("ns3::WaypointMobilityModel::InitialPositionIsWaypoint", BooleanValue (true));

  // Keep file names consistent
  Config::SetDefault ("ns3::psc::McpttStateMachineStats::OutputFileName", StringValue ("mcptt-operational-modes-mobility-state-machine-stats.dat"));
  Config::SetDefault ("ns3::psc::McpttMsgStats::OutputFileName", StringValue ("mcptt-operational-modes-mobility-msg-stats.dat"));

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
  //lteHelper->SetAttribute ("PathlossModel", StringValue ("ns3::Hybrid3gppPropagationLossModel"));

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

  //Position of the nodes
  NS_LOG_INFO ("Defining nodes starting positions...");

  //eNodeB
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  positionAllocEnb->Add (Vector (0.0, -60.0, 30.0));
  NS_LOG_INFO ("eNodeB 0 node id = [" << enbNode.Get (0)->GetId () << "]"
               " position: (" << 0.0 << " , " << -60.0 << ", " << 30.0 << ")");


  //Initial position of the UEs
  Ptr<ListPositionAllocator> positionAllocStaticUEs = CreateObject<ListPositionAllocator> ();
  Ptr<ListPositionAllocator> positionAllocMovingUEs = CreateObject<ListPositionAllocator> ();
  NodeContainer staticUEsNodes;
  NodeContainer movingUEsNodes;

  double ue_pos_x = uesDistanceToENB_x;
  double ue_pos_y = 0.0;
  double ue_pos_x_static = 0.0;
  double ue_pos_y_static = 0.0;
  double team_center_y = -20.0;

  for (uint32_t ueNodeIdx = 0; ueNodeIdx < allUeNodes.GetN (); ueNodeIdx++)
    {
      //The first member of each team will remain static
      if (ueNodeIdx % nUEsPerTeam == 0)
        {
          ue_pos_x_static = ue_pos_x - 3.0;
          ue_pos_y_static = team_center_y;
          positionAllocStaticUEs->Add (Vector (ue_pos_x_static, ue_pos_y_static, 1.5));
          staticUEsNodes.Add (allUeNodes.Get (ueNodeIdx));

          NS_LOG_INFO ("Static UE Node id = [" << allUeNodes.Get (ueNodeIdx)->GetId () << "]"
                       " UE Idx = [" << ueNodeIdx << "]"
                       " position: (" << ue_pos_x_static << " , " << ue_pos_y_static  << ", " << 1.5 << ")");

          ue_pos_y = team_center_y + 3.0;
          team_center_y =  team_center_y - 40.0;

        }
      else
        {
          positionAllocMovingUEs->Add (Vector (ue_pos_x, ue_pos_y, 1.5));
          movingUEsNodes.Add (allUeNodes.Get (ueNodeIdx));

          NS_LOG_INFO ("Moving UE Node id = [" << allUeNodes.Get (ueNodeIdx)->GetId () << "]"
                       " UE Idx = [" << ueNodeIdx << "]"
                       " initial position: (" << ue_pos_x << " , " << ue_pos_y  << ", " << 1.5 << ")");

          ue_pos_y = ue_pos_y - 3.0; // 3 meters towards the bottom

        }
    }

  //Position of the building
  double xMin = uesDistanceToENB_x + uesDistanceToBuilding_x;

  Ptr<Building> building1 = CreateObject<Building> ();
  building1->SetBoundaries (Box (xMin, xMin + 40, -120, 0, 0.0, 20.0));
  building1->SetBuildingType (Building::Commercial);
  building1->SetExtWallsType (Building::ConcreteWithWindows);
  NS_LOG_DEBUG ("Building bounding box (" << xMin << ", " << (xMin + 40) << ", -120, 0, 0.0, 20.0)");

  NS_LOG_INFO ("Configuring mobility...");

  //Install mobility
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> mobilityOutputStream = ascii.CreateFileStream ("mcptt-operational-modes-mobility-course-change.dat");
  //eNodeB
  MobilityHelper mobilityeNodeB;
  mobilityeNodeB.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityeNodeB.SetPositionAllocator (positionAllocEnb);
  mobilityeNodeB.Install (enbNode);
  BuildingsHelper::Install (enbNode);
  mobilityeNodeB.EnableAsciiAll (mobilityOutputStream);

  //Static UEs
  MobilityHelper mobilityStaticUEs;
  mobilityStaticUEs.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityStaticUEs.SetPositionAllocator (positionAllocStaticUEs);
  mobilityStaticUEs.Install (staticUEsNodes);
  BuildingsHelper::Install (staticUEsNodes);
  mobilityStaticUEs.EnableAsciiAll (mobilityOutputStream);

  //Moving UEs
  MobilityHelper mobilityMovingUEs;
  if (!mobility)
    {
      mobilityMovingUEs.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
      mobilityMovingUEs.SetPositionAllocator (positionAllocMovingUEs);
      mobilityMovingUEs.Install (movingUEsNodes);
    }
  else
    {
      mobilityMovingUEs.SetMobilityModel ("ns3::WaypointMobilityModel");
      mobilityMovingUEs.SetPositionAllocator (positionAllocMovingUEs);

      mobilityMovingUEs.Install (movingUEsNodes);
      BuildingsHelper::Install (movingUEsNodes);


      for (uint32_t movingUeNodeIdx = 0; movingUeNodeIdx < movingUEsNodes.GetN (); movingUeNodeIdx++)
        {
          Ptr<Node> refNode = movingUEsNodes.Get (movingUeNodeIdx);
          Ptr<WaypointMobilityModel> mobilityModel = refNode->GetObject<WaypointMobilityModel> ();

          //Get initial position of the moving UE for the first waypoint
          Vector initialPos = mobilityModel->GetPosition ();

          //Calculate team to which the UE belongs
          uint32_t team = (uint32_t)(movingUeNodeIdx / (nUEsPerTeam - 1)) + 1;
          //Choose movement start time depending on team
          double movementStartTime = 0.0;
          double door_pos_x = xMin;
          double door_pos_y = 0.0; //Calculated for each UE depending on team
          double inside_pos_x = xMin + 20;
          double inside_pos_y = 0.0; //Calculated for each UE depending on team

          Vector staticUEPos;
          switch (team)
            {
              case 1:
                movementStartTime = team1StartTime + team1startDelayMovement;
                staticUEPos = staticUEsNodes.Get (0)->GetObject<MobilityModel> ()->GetPosition ();
                break;
              case 2:
                movementStartTime = team2StartTime + team2startDelayMovement;
                staticUEPos = staticUEsNodes.Get (1)->GetObject<MobilityModel> ()->GetPosition ();
                break;
              case 3:
                movementStartTime = team3StartTime + team3startDelayMovement;
                staticUEPos = staticUEsNodes.Get (2)->GetObject<MobilityModel> ()->GetPosition ();
                break;
            }
          double movementStopTime = movementStartTime + delayToReachBuilding + durationMovementInsideBuilding;

          door_pos_y = staticUEPos.y;
          inside_pos_y = (door_pos_y - 10) + (movingUeNodeIdx % 3) * 10;

          Waypoint wpt0 (Seconds (movementStartTime), initialPos);
          Waypoint wpt1 (Seconds (movementStartTime + delayToReachBuilding), Vector (door_pos_x, door_pos_y, 1.5));
          Waypoint wpt2 (Seconds (movementStopTime), Vector (inside_pos_x, inside_pos_y, 1.5));
          mobilityModel->AddWaypoint (wpt0);
          mobilityModel->AddWaypoint (wpt1);
          mobilityModel->AddWaypoint (wpt2);

          NS_LOG_INFO ("Moving UE Node id = [" << allUeNodes.Get (movingUeNodeIdx)->GetId () << "]"
                       " starts moving at " <<  movementStartTime << " s"
                       " reaches the building at " <<  movementStartTime + delayToReachBuilding << " s"
                       " on position (" << door_pos_x << "m, " << door_pos_y << "m, 1.5)"
                       " and stops moving at " <<  movementStopTime << " s"
                       " on position (" << inside_pos_x << "m, " << inside_pos_y << "m, 1.5)");

        }
      mobilityMovingUEs.EnableAsciiAll (mobilityOutputStream);
    }

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
  serverApps.Start (Seconds (team1StartTime - 1.0));
  serverApps.Stop (Seconds (simTime - 1.0));

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

  clientApps.Start (Seconds (team1StartTime - 1.0));
  clientApps.Stop (Seconds (simTime - 1.0));

  DynamicCast<McpttPttApp, Application> (team1Apps.Get (1))->GetPusher ()->SetAttribute ("Automatic", BooleanValue (true));
  DynamicCast<McpttPttApp, Application> (team2Apps.Get (1))->GetPusher ()->SetAttribute ("Automatic", BooleanValue (true));
  DynamicCast<McpttPttApp, Application> (team3Apps.Get (1))->GetPusher ()->SetAttribute ("Automatic", BooleanValue (true));

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
  double callStartTime = team1StartTime + startDelayInNetworkApp;
  double callStopTime =  team1StartTime + team1startDelayMovement + durationTimeInNetworkAppWhileMoving;
  McpttCallMsgFieldCallType callType = McpttCallMsgFieldCallType::BASIC_GROUP;
  callId = callHelper.AddCall (team1Apps,
                               serverApp,
                               groupId,
                               callType,
                               Seconds (callStartTime),
                               Seconds (callStopTime));
  groupIdToOnNetworkCallId.insert (std::pair<uint32_t, uint16_t> (groupId, callId));

  NS_LOG_INFO ("Team 1 "
               "starts on-network application at " << callStartTime << " s and "
               "stops it at " << callStopTime << " s");

  groupId = 2;
  callStartTime = team2StartTime + startDelayInNetworkApp;
  callStopTime = callStartTime + team2durationTimeInNetworkApp;
  callId = callHelper.AddCall (team2Apps,
                               serverApp,
                               groupId,
                               callType,
                               Seconds (callStartTime),
                               Seconds (callStopTime));
  groupIdToOnNetworkCallId.insert (std::pair<uint32_t, uint16_t> (groupId, callId));

  NS_LOG_INFO ("Team 2 "
               "starts on-network application at " << callStartTime << " s and "
               "stops it at " << callStopTime << " s");

  groupId = 3;
  callStartTime = team3StartTime + startDelayInNetworkApp;
  callStopTime = team3StartTime + team3startDelayMovement + durationTimeInNetworkAppWhileMoving;
  callId = callHelper.AddCall (team3Apps,
                               serverApp,
                               groupId,
                               callType,
                               Seconds (callStartTime),
                               Seconds (callStopTime));
  groupIdToOnNetworkCallId.insert (std::pair<uint32_t, uint16_t> (groupId, callId));
  uint16_t team3CallId = callId;

  NS_LOG_INFO ("Team 3 "
               "starts on-network application at " << callStartTime << " s and "
               "stops it at " << callStopTime << " s");

  //*** Out-of-Network communication for TEAM 2 moving UEs only ***//

  //Define addressing
  uint32_t groupL2Address = 255;
  Ipv6Address groupAddress6 ("ff0e::1");     //use multicast address as destination

  //Add multicast address only in the Moving UEs of Team 2
  for (uint32_t ueNodeIdx = 1; ueNodeIdx < team2UeNodes.GetN (); ueNodeIdx++)
    {
      team2UeNodes.Get (ueNodeIdx)->GetObject<Ipv6L3Protocol> ()->AddMulticastAddress (groupAddress6);
    }

  //Create tft for off-network applications
  Ptr<LteSlTft> tft;
  tft = Create<LteSlTft> (LteSlTft::BIDIRECTIONAL, groupAddress6, groupL2Address);

  //Activate SL  bearer 1.0 s before application start
  Time slBearersActivationTime = Seconds (team2StartTime + startDelayOutOfNetworkApp - 1.0);
  proseHelper->ActivateSidelinkBearer (slBearersActivationTime, team2UeDevs, tft);

  groupId = 4;
  callStartTime = team2StartTime + startDelayOutOfNetworkApp;
  callStopTime = team2StartTime + startDelayOutOfNetworkApp + durationTimeOutOfNetworkApp;
  callHelper.AddCallOffNetwork (team2Apps,
                                serverApp->AllocateCallId (),
                                groupAddress6,
                                groupId,
                                callType,
                                Seconds (callStartTime),
                                Seconds (callStopTime));
  NS_LOG_INFO ("Team 2 "
               "starts off-network application at " << callStartTime << " s and "
               "stops it at " << callStopTime << " s");


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
  Simulator::Schedule (Seconds (team3StartTime + startDelayRelayServiceRelayUE), &LteSidelinkHelper::StartRelayService,
                       proseHelper, relayUeDev.Get (0), RELAY_SERVICE_CODE,
                       LteSlUeRrc::ModelA, LteSlUeRrc::RelayUE);

  NS_LOG_INFO ("Relay UE node id = [" << relayUeNode.Get (0)->GetId () << "]"
               " provides Service Code " << RELAY_SERVICE_CODE <<
               " and start service at " << team3StartTime + startDelayRelayServiceRelayUE << " s");

  //Connect trace in all Team's UEs for monitoring UE-to-Network Relay signaling messages
  Ptr<OutputStreamWrapper> PC5SignalingPacketTraceStream = ascii.CreateFileStream ("mcptt-operational-modes-mobility-pc5-signaling.dat");
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
  Ptr<OutputStreamWrapper> RsrpMeasurementsStream = ascii.CreateFileStream ("mcptt-operational-modes-mobility-rsrp-measurement.dat");
  *RsrpMeasurementsStream->GetStream () << "time(s)\timsi\tcellId\trsrp\tthreshold" << std::endl;
  std::ostringstream ctx;

  for (uint32_t ueIdx = 0; ueIdx < remoteUeNodes.GetN (); ueIdx++)
    {
      ctx << remoteUeNodes.Get (ueIdx)->GetId ();
      Ptr<LteUeRrc> rrc = remoteUeDevs.Get (ueIdx)->GetObject<LteUeNetDevice> ()->GetRrc ();
      PointerValue ptrOne;
      rrc->TraceConnect ("RsrpMeasurement",  ctx.str (),
                         MakeBoundCallback (&RemoteRsrpMeasurementTrace,
                                            RsrpMeasurementsStream,
                                            threshold,
                                            remoteUeDevs.Get (ueIdx)->GetObject<LteUeNetDevice> ()));

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

  NS_LOG_INFO ("Enabling traces...");

  lteHelper->EnablePdcpTraces ();
  lteHelper->EnableUlMacTraces ();
  lteHelper->EnableDlMacTraces ();

  Ptr<McpttTraceHelper> traceHelper = CreateObject<McpttTraceHelper> ();
  traceHelper->EnableMsgTraces ();
  traceHelper->EnableStateMachineTraces ();
  traceHelper->EnableMouthToEarLatencyTrace ("mcptt-operational-modes-mobility-m2e-latency.dat");
  traceHelper->EnableAccessTimeTrace ("mcptt-operational-modes-mobility-access-time.dat");

  //Configure visualization
#ifdef HAS_NETSIMULYZER
  using netsimulyzer::OptionalValue;
  using netsimulyzer::Color3;
  using netsimulyzer::Color3Value;
  using CategoryValuePair = netsimulyzer::CategoryAxis::ValuePair;

  if (enableVisualization)
    {
      NS_LOG_INFO ("Enabling visualization...");

      const auto teamOneColor = netsimulyzer::RED_VALUE;
      const auto teamTwoColor = netsimulyzer::GREEN_VALUE;
      const auto teamThreeColor = netsimulyzer::BLUE_VALUE;

      orchestrator = CreateObject<netsimulyzer::Orchestrator> ("mcptt-operational-modes-mobility.json");
      orchestrator->SetTimeStep (MilliSeconds (guiResolution), Time::Unit::MS);
      orchestrator->SetAttribute ("MobilityPollInterval", TimeValue (MilliSeconds (guiResolution)));

      auto team1Area = CreateObject<netsimulyzer::RectangularArea> (orchestrator, Rectangle{395, 403, -25, -15});
      team1Area->SetAttribute ("Fill", EnumValue (netsimulyzer::RectangularArea::DrawMode::Hidden));
      team1Area->SetAttribute ("Border", EnumValue (netsimulyzer::RectangularArea::DrawMode::Solid));
      // Red
      team1Area->SetAttribute ("BorderColor", teamOneColor);

      auto team2Area = CreateObject<netsimulyzer::RectangularArea> (orchestrator, Rectangle{395, 403, -65, -55});
      team2Area->SetAttribute ("Fill", EnumValue (netsimulyzer::RectangularArea::DrawMode::Hidden));
      team2Area->SetAttribute ("Border", EnumValue (netsimulyzer::RectangularArea::DrawMode::Solid));
      // Green
      team2Area->SetAttribute ("BorderColor", teamTwoColor);

      auto team3Area = CreateObject<netsimulyzer::RectangularArea> (orchestrator, Rectangle{395, 403, -105, -95});
      team3Area->SetAttribute ("Fill", EnumValue (netsimulyzer::RectangularArea::DrawMode::Hidden));
      team3Area->SetAttribute ("Border", EnumValue (netsimulyzer::RectangularArea::DrawMode::Solid));
      // Blue
      team3Area->SetAttribute ("BorderColor", teamThreeColor);

      auto firetruck1 = CreateObject<netsimulyzer::Decoration> (orchestrator);
      firetruck1->SetAttribute ("Model", StringValue ("non-distributable/models/props/vehicle/Firetruck.obj"));
      firetruck1->SetAttribute ("Orientation", Vector3DValue ({0.0, 0.0, 0.0}));
      firetruck1->SetAttribute ("Position", Vector3DValue ({390.0, -40.0, 0.0}));

      auto firetruck2 = CreateObject<netsimulyzer::Decoration> (orchestrator);
      firetruck2->SetAttribute ("Model", StringValue ("non-distributable/models/props/vehicle/Firetruck.obj"));
      firetruck2->SetAttribute ("Orientation", Vector3DValue ({0.0, 0.0, 0.0}));
      firetruck2->SetAttribute ("Position", Vector3DValue ({390.0, -80.0, 0.0}));

      //Configure buildings
      netsimulyzer::BuildingConfigurationHelper buildingConfigHelper (orchestrator);
      buildingConfigHelper.Install (building1);
      Ptr<Building> door1 = CreateObject<Building> ();
      door1->SetBoundaries (Box (409.8, 409.999, -21, -19, 0.0, 5.0));
      buildingConfigHelper.Install (door1);
      Ptr<Building> door2 = CreateObject<Building> ();
      door2->SetBoundaries (Box (409.8, 409.999, -61, -59, 0.0, 5.0));
      buildingConfigHelper.Install (door2);
      Ptr<Building> door3 = CreateObject<Building> ();
      door3->SetBoundaries (Box (409.8, 409.999, -101, -99, 0.0, 5.0));
      buildingConfigHelper.Install (door3);


      //Configure nodes
      netsimulyzer::NodeConfigurationHelper nodeConfigHelper (orchestrator);
      nodeConfigHelper.Set ("Scale", DoubleValue (1.0));
      nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/characters/rest/Firefighter.obj"));
      nodeConfigHelper.Set ("Height", OptionalValue<double> (1.8));
      nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D (0, 0, -1.5)));
      nodeConfigHelper.Set ("Orientation", Vector3DValue (Vector3D (0, 0, 90)));

      for (uint32_t ueNodeIdx = 0; ueNodeIdx < allUeNodes.GetN (); ueNodeIdx++)
        {
          uint32_t team = (uint32_t)(ueNodeIdx / (nUEsPerTeam)) + 1;
          nodeConfigHelper.Set ("Name", StringValue ("Responder " + std::to_string (ueNodeIdx) + "-team " + std::to_string (team)));
          nodeConfigHelper.Install (allUeNodes.Get (ueNodeIdx));
        }

      //Network
      nodeConfigHelper.Set ("Model", StringValue ("non-distributable/models/props/Billboard_pole.obj"));
      nodeConfigHelper.Set ("Name", StringValue ("Cell tower"));
      nodeConfigHelper.Set ("Height", OptionalValue<double> (30));
      nodeConfigHelper.Set ("Offset", Vector3DValue (Vector3D (0, 0, -30)));
      nodeConfigHelper.Set ("Orientation", Vector3DValue (Vector3D (0, 0, 0)));
      nodeConfigHelper.Install (enbNode);

      //Decorations
      // for (auto team = 0; team < 3; team++)
      // {
      // Ptr<netsimulyzer::Decoration> antenna = CreateObject<netsimulyzer::Decoration> (orchestrator);
      // antenna->SetAttribute ("Model", StringValue ("non-distributable/models/props/Cylinder.obj"));
      // antenna->SetAttribute ("Height", OptionalValue<double> (3));
      // antenna->SetAttribute ("Position", Vector3DValue (Vector3D(1, -59, 28)));
      // eNodebAntennas.push_back (antenna);
      // antenna = CreateObject<netsimulyzer::Decoration> (orchestrator);
      // antenna->SetAttribute ("Model", StringValue ("non-distributable/models/props/Cylinder.obj"));
      // antenna->SetAttribute ("Height", OptionalValue<double> (3));
      // antenna->SetAttribute ("Position", Vector3DValue (Vector3D(-1, -61, 28)));
      // eNodebAntennas.push_back (antenna);
      // antenna = CreateObject<netsimulyzer::Decoration> (orchestrator);
      // antenna->SetAttribute ("Model", StringValue ("non-distributable/models/props/Cylinder.obj"));
      // antenna->SetAttribute ("Height", OptionalValue<double> (3));
      // antenna->SetAttribute ("Position", Vector3DValue (Vector3D(-1, -61, 28)));
      // eNodebAntennas.push_back (antenna);
      // }

      //Log
      applicationLog = CreateObject<netsimulyzer::LogStream> (orchestrator);
      applicationLog->SetAttribute ("Name", StringValue ("Application log"));

      applicationLog->SetAttribute ("Color", OptionalValue<Color3> {100u, 150u, 100u});

      Simulator::Schedule (Seconds (15), &netsimulyzer::LogStream::Write, applicationLog, "15 s: Team 1 starts group call\n");
      Simulator::Schedule (Seconds (15), &netsimulyzer::LogStream::Write, applicationLog, "15 s: Team 2 starts group call\n");
      Simulator::Schedule (Seconds (15), &netsimulyzer::LogStream::Write, applicationLog, "15 s: Team 3 starts group call\n");
      Simulator::Schedule (Seconds (75), &netsimulyzer::LogStream::Write, applicationLog, "75 s: Team 1 stops group call\n");
      Simulator::Schedule (Seconds (120), &netsimulyzer::LogStream::Write, applicationLog, "120 s: Team 2 stops group call\n");
      Simulator::Schedule (Seconds (165), &netsimulyzer::LogStream::Write, applicationLog, "165 s: Team 3 stops group call\n");

      mobilityLog = CreateObject<netsimulyzer::LogStream> (orchestrator);
      mobilityLog->SetAttribute ("Name", StringValue ("Mobility log"));

      mobilityLog->SetAttribute ("Color", OptionalValue<Color3> {72u, 66u, 245u});

      Simulator::Schedule (Seconds (30), &netsimulyzer::LogStream::Write, mobilityLog, "30 s: Team 1 starts moving\n");
      Simulator::Schedule (Seconds (45), &netsimulyzer::LogStream::Write, mobilityLog, "45 s: Team 1 enters building\n");
      Simulator::Schedule (Seconds (75), &netsimulyzer::LogStream::Write, mobilityLog, "75 s: Team 1 stops moving\n");
      Simulator::Schedule (Seconds (75), &netsimulyzer::LogStream::Write, mobilityLog, "75 s: Team 2 starts moving\n");
      Simulator::Schedule (Seconds (90), &netsimulyzer::LogStream::Write, mobilityLog, "90 s: Team 2 enters building\n");
      Simulator::Schedule (Seconds (120), &netsimulyzer::LogStream::Write, mobilityLog, "120 s: Team 2 stops moving\n");
      Simulator::Schedule (Seconds (120), &netsimulyzer::LogStream::Write, mobilityLog, "135 s: Team 3 starts moving\n");
      Simulator::Schedule (Seconds (135), &netsimulyzer::LogStream::Write, mobilityLog, "120 s: Team 3 enters building\n");
      Simulator::Schedule (Seconds (165), &netsimulyzer::LogStream::Write, mobilityLog, "165 s: Team 3 enters building\n");

      //Statistics
      PointerValue xAxis;
      PointerValue yAxis;
      //Visualize MCPTT server
      ObjectMapValue serverAppCalls;
      serverApp->GetAttribute ("Calls", serverAppCalls);
      std::map<uint32_t, uint16_t>::iterator groupIdToOnNetworkCallIdIt;
      std::vector<CategoryValuePair> floorArbitratorStates;
      for (groupIdToOnNetworkCallIdIt = groupIdToOnNetworkCallId.begin (); groupIdToOnNetworkCallIdIt != groupIdToOnNetworkCallId.end (); groupIdToOnNetworkCallIdIt++)
        {
          Ptr<McpttServerCall> serverCall = serverAppCalls.Get (groupIdToOnNetworkCallIdIt->second)->GetObject<McpttServerCall> ();
          ServerCallTracer serverCallTracer;
          std::map<uint16_t, ServerCallTracer>::iterator serverIt;
          serverCallTracers.insert (std::pair<uint16_t, ServerCallTracer> (groupIdToOnNetworkCallIdIt->first, serverCallTracer));
          serverIt = serverCallTracers.find (groupIdToOnNetworkCallIdIt->first);
          CategoryValuePair floorArbitratorStateValue;
          floorArbitratorStateValue.key = McpttOnNetworkFloorArbitratorStateStartStop::GetStateId ().GetNumber ();
          floorArbitratorStateValue.value = McpttOnNetworkFloorArbitratorStateStartStop::GetStateId ().GetName ();
          floorArbitratorStates.push_back (floorArbitratorStateValue);
          floorArbitratorStateValue.key = McpttOnNetworkFloorArbitratorStateIdle::GetStateId ().GetNumber ();
          floorArbitratorStateValue.value = McpttOnNetworkFloorArbitratorStateIdle::GetStateId ().GetName ();
          floorArbitratorStates.push_back (floorArbitratorStateValue);
          floorArbitratorStateValue.key = McpttOnNetworkFloorArbitratorStateTaken::GetStateId ().GetNumber ();
          floorArbitratorStateValue.value = McpttOnNetworkFloorArbitratorStateTaken::GetStateId ().GetName ();
          floorArbitratorStates.push_back (floorArbitratorStateValue);
          floorArbitratorStateValue.key = McpttOnNetworkFloorArbitratorStateRevoke::GetStateId ().GetNumber ();
          floorArbitratorStateValue.value = McpttOnNetworkFloorArbitratorStateRevoke::GetStateId ().GetName ();
          floorArbitratorStates.push_back (floorArbitratorStateValue);
          floorArbitratorStateValue.key = McpttOnNetworkFloorArbitratorStateReleasing::GetStateId ().GetNumber ();
          floorArbitratorStateValue.value = McpttOnNetworkFloorArbitratorStateReleasing::GetStateId ().GetName ();
          floorArbitratorStates.push_back (floorArbitratorStateValue);
          floorArbitratorStateValue.key = McpttOnNetworkFloorArbitratorStateInitialising::GetStateId ().GetNumber ();
          floorArbitratorStateValue.value = McpttOnNetworkFloorArbitratorStateInitialising::GetStateId ().GetName ();
          floorArbitratorStates.push_back (floorArbitratorStateValue);

          PointerValue floorArbitrator;
          serverCall->GetAttribute ("FloorArbitrator", floorArbitrator);
          Ptr<McpttOnNetworkFloorArbitrator> arbitrator = floorArbitrator.Get<McpttOnNetworkFloorArbitrator> ();
          arbitrator->TraceConnectWithoutContext ("StateChangeTrace", MakeCallback (&ServerCallTracer::FloorControlStateTrace, &serverIt->second));

          serverIt->second.m_floorArbitrator = CreateObject <netsimulyzer::StateTransitionSink> (orchestrator, floorArbitratorStates, 0);
          serverIt->second.m_floorArbitrator->SetAttribute ("Name", StringValue ("Team " + std::to_string (groupIdToOnNetworkCallIdIt->first) + " Floor Arbitrator"));
          serverIt->second.m_floorArbitrator->SetAttribute ("LoggingMode", StringValue ("None"));
        }

      std::map <uint32_t, MessagesTracer>::iterator rxMessagesPerUeTracerMapIt;
      for (rxMessagesPerUeTracerMapIt = rxMessagesPerUeTracerMap.begin (); rxMessagesPerUeTracerMapIt != rxMessagesPerUeTracerMap.end (); rxMessagesPerUeTracerMapIt++)
        {
          //Calculate team to which the UE belongs
          uint32_t ueNodeIdx = rxMessagesPerUeTracerMapIt->first;
          uint32_t team = (uint32_t)(ueNodeIdx / (nUEsPerTeam)) + 1;

          //RSRP
          Ptr<LteUeRrc> rrc = allUeDevs.Get (ueNodeIdx)->GetObject<LteUeNetDevice> ()->GetRrc ();
          rrc->TraceConnectWithoutContext ("RsrpMeasurement",
                                           MakeCallback (&MessagesTracer::RsrpMeasurementTrace, &rxMessagesPerUeTracerMapIt->second));

          if (ueNodeIdx == 2 || ueNodeIdx == 6 || ueNodeIdx == 11)
            {
              rxMessagesPerUeTracerMapIt->second.m_rsrpSeries = CreateObject <netsimulyzer::XYSeries> (orchestrator);

              rxMessagesPerUeTracerMapIt->second.m_rsrpSeries->SetAttribute ("Name", StringValue ("UE " + std::to_string (ueNodeIdx) + " RSRP"));
              rxMessagesPerUeTracerMapIt->second.m_rsrpSeries->SetAttribute ("LabelMode", StringValue ("Hidden"));
              rxMessagesPerUeTracerMapIt->second.m_rsrpSeries->SetAttribute ("Connection", StringValue ("None"));
              switch (team)
                {
                  case 1:
                    rxMessagesPerUeTracerMapIt->second.m_rsrpSeries->SetAttribute ("Color", teamOneColor);
                    break;
                  case 2:
                    rxMessagesPerUeTracerMapIt->second.m_rsrpSeries->SetAttribute ("Color", teamTwoColor);
                    break;
                  case 3:
                    rxMessagesPerUeTracerMapIt->second.m_rsrpSeries->SetAttribute ("Color", teamThreeColor);
                  default:
                    //should not happen, keep default color
                    break;
                }

              rxMessagesPerUeTracerMapIt->second.m_rsrpSeries->GetXAxis()->SetAttribute ("Name", StringValue ("Time (s)"));
              rxMessagesPerUeTracerMapIt->second.m_rsrpSeries->GetAttribute ("YAxis", yAxis);
              yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("RSRP (dBm)"));
              yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("BoundMode", StringValue ("Fixed"));
              yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Maximum", DoubleValue (-110));
              yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Minimum", DoubleValue (-130));


              //Media delay
              rxMessagesPerUeTracerMapIt->second.m_mediaDelaySeries = CreateObject <netsimulyzer::XYSeries> (orchestrator);

              rxMessagesPerUeTracerMapIt->second.m_mediaDelaySeries->SetAttribute ("Name", StringValue ("UE " + std::to_string (ueNodeIdx) + " M2E latency"));
              rxMessagesPerUeTracerMapIt->second.m_mediaDelaySeries->SetAttribute ("LabelMode", StringValue ("Hidden"));
              rxMessagesPerUeTracerMapIt->second.m_mediaDelaySeries->SetAttribute ("Connection", StringValue ("None"));
              rxMessagesPerUeTracerMapIt->second.m_mediaDelaySeries->SetAttribute ("Color", Color3Value {204u, 111u, 4u});

              switch (team)
                {
                  case 1:
                    rxMessagesPerUeTracerMapIt->second.m_mediaDelaySeries->SetAttribute ("Color", teamOneColor);
                    break;
                  case 2:
                    rxMessagesPerUeTracerMapIt->second.m_mediaDelaySeries->SetAttribute ("Color", teamTwoColor);
                    break;
                  case 3:
                    rxMessagesPerUeTracerMapIt->second.m_mediaDelaySeries->SetAttribute ("Color", teamThreeColor);
                    break;
                  default:
                    //should not happen, keep default color
                    break;
                }

              rxMessagesPerUeTracerMapIt->second.m_mediaDelaySeries->GetXAxis()->SetAttribute ("Name", StringValue ("Time (s)"));
              rxMessagesPerUeTracerMapIt->second.m_mediaDelaySeries->GetYAxis()->SetAttribute ("Name", StringValue ("Latency (ms)"));
            }

          bool hasOnNetworkCall = false;
          bool hasOffNetworkCall = false;
          std::map<uint16_t, Ptr<McpttCall> >::iterator ueCallsIt;
          Ptr<McpttPttApp> pttApp = clientApps.Get (ueNodeIdx)->GetObject<McpttPttApp> ();
          std::map<uint16_t, Ptr<McpttCall> > ueCalls = pttApp->GetCalls ();
          for (ueCallsIt = ueCalls.begin (); ueCallsIt != ueCalls.end (); ueCallsIt++)
            {
              Ptr<McpttCall> ueCall = ueCallsIt->second;
              Ptr<McpttFloorParticipant> ueFloorParticipant = ueCall->GetFloorMachine ();

              if (ueFloorParticipant->GetInstanceTypeId () == McpttOnNetworkFloorParticipant::GetTypeId ())
                {
                  hasOnNetworkCall = true;
                  ueFloorParticipant->GetObject<McpttOnNetworkFloorParticipant> ()->TraceConnectWithoutContext ("StateChangeTrace",
                                                                                                                MakeCallback (&MessagesTracer::FloorControlStateTrace, &rxMessagesPerUeTracerMapIt->second));
                }
              else if (ueFloorParticipant->GetInstanceTypeId () == McpttOffNetworkFloorParticipant::GetTypeId ())
                {
                  hasOffNetworkCall = true;
                  ueFloorParticipant->GetObject<McpttOffNetworkFloorParticipant> ()->TraceConnectWithoutContext ("StateChangeTrace",
                                                                                                                 MakeCallback (&MessagesTracer::FloorControlStateTrace, &rxMessagesPerUeTracerMapIt->second));
                }
              else
                {
                  NS_ABORT_MSG ("Cannot identify floor participant to connect to state trace.");
                }
            }

          std::vector<CategoryValuePair> pusherStates;
          CategoryValuePair pusherStateValue;
          pusherStateValue.key = 0;
          pusherStateValue.value = "Released";
          pusherStates.push_back (pusherStateValue);
          pusherStateValue.key = 1;
          pusherStateValue.value = "Pushed";
          pusherStates.push_back (pusherStateValue);
          pttApp->GetPusher ()->TraceConnectWithoutContext ("PushingState",
                                                            MakeCallback (&MessagesTracer::PusherStateTrace, &rxMessagesPerUeTracerMapIt->second));

          //Add pusher state only for Tx nodes
          if (ueNodeIdx == 1 || ueNodeIdx == 5 || ueNodeIdx == 9)
            {
              rxMessagesPerUeTracerMapIt->second.m_pusher = CreateObject <netsimulyzer::StateTransitionSink> (orchestrator, pusherStates, 0);
              rxMessagesPerUeTracerMapIt->second.m_pusher->SetAttribute ("Name", StringValue ("UE " + std::to_string (ueNodeIdx) + " Pusher state"));
              rxMessagesPerUeTracerMapIt->second.m_pusher->SetAttribute ("LoggingMode", StringValue ("None"));
            }

          //Add state machine graphs
          std::vector<CategoryValuePair> floorCtrlStates;
          CategoryValuePair floorCtrlStateValue;
          floorCtrlStateValue.key = 0;
          floorCtrlStateValue.value = McpttOnNetworkFloorParticipantStateStartStop::GetStateId ().GetName ();
          floorCtrlStates.push_back (floorCtrlStateValue);
          rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));

          if (hasOnNetworkCall)
            {
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOnNetworkFloorParticipantStateHasNoPermission::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOnNetworkFloorParticipantStatePendingRelease::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOnNetworkFloorParticipantStatePendingRequest::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOnNetworkFloorParticipantStateHasPermission::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOnNetworkFloorParticipantStateReleasing::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOnNetworkFloorParticipantStateQueued::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
            }

          if (hasOffNetworkCall)
            {
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOffNetworkFloorParticipantStateSilence::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOffNetworkFloorParticipantStatePendGrant::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOffNetworkFloorParticipantStatePendReq::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
              floorCtrlStateValue.key++;
              floorCtrlStateValue.value = McpttOffNetworkFloorParticipantStateQueued::GetStateId ().GetName ();
              floorCtrlStates.push_back (floorCtrlStateValue);
              rxMessagesPerUeTracerMapIt->second.m_stateToId.insert (std::pair<std::string, int> (floorCtrlStateValue.value, floorCtrlStateValue.key));
            }

          //Create graph for floor control for Tx nodes only
          if (ueNodeIdx == 0 || ueNodeIdx == 1 || ueNodeIdx == 5 || ueNodeIdx == 9)
            {
              rxMessagesPerUeTracerMapIt->second.m_floorParticipant = CreateObject <netsimulyzer::StateTransitionSink> (orchestrator, floorCtrlStates, 0);
              rxMessagesPerUeTracerMapIt->second.m_floorParticipant->SetAttribute ("Name", StringValue ("UE " + std::to_string (ueNodeIdx) + " Floor control"));
              rxMessagesPerUeTracerMapIt->second.m_floorParticipant->SetAttribute ("LoggingMode", StringValue ("None"));
            }

          //Create graph for access time and pusher states for Tx nodes only
          if (ueNodeIdx == 1 || ueNodeIdx == 5 || ueNodeIdx == 9)
            {
              rxMessagesPerUeTracerMapIt->second.m_accessTimeSeries = CreateObject <netsimulyzer::XYSeries> (orchestrator);
              rxMessagesPerUeTracerMapIt->second.m_accessTimeSeries->SetAttribute ("Name", StringValue ("UE " + std::to_string (ueNodeIdx) + " Access time"));
              rxMessagesPerUeTracerMapIt->second.m_accessTimeSeries->SetAttribute ("LabelMode", StringValue ("Hidden"));
              rxMessagesPerUeTracerMapIt->second.m_accessTimeSeries->SetAttribute ("Connection", StringValue ("None"));
              rxMessagesPerUeTracerMapIt->second.m_accessTimeSeries->SetAttribute ("Color", Color3Value {204u, 111u, 4u});

              switch (team)
                {
                  case 1:
                    rxMessagesPerUeTracerMapIt->second.m_accessTimeSeries->SetAttribute ("Color", teamOneColor);
                    break;
                  case 2:
                    rxMessagesPerUeTracerMapIt->second.m_accessTimeSeries->SetAttribute ("Color", teamTwoColor);
                    break;
                  case 3:
                    rxMessagesPerUeTracerMapIt->second.m_accessTimeSeries->SetAttribute ("Color", teamThreeColor);
                    break;
                  default:
                    //should not happen, keep default color
                    break;
                }

              rxMessagesPerUeTracerMapIt->second.m_accessTimeSeries->GetXAxis()->SetAttribute ("Name", StringValue ("Time (s)"));
              rxMessagesPerUeTracerMapIt->second.m_accessTimeSeries->GetYAxis()->SetAttribute ("Name", StringValue ("Access time (ms)"));
            }
        }

      //Add graphs to collection data
      rsrpCollection = CreateObject <netsimulyzer::SeriesCollection> (orchestrator);
      rsrpCollection->SetAttribute ("Name", StringValue ("RSRP"));
      rsrpCollection->GetXAxis()->SetAttribute ("Name", StringValue ("Time (s)"));
      rsrpCollection->GetAttribute ("YAxis", yAxis);
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Name", StringValue ("RSRP (dBm)"));
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("BoundMode", StringValue ("Fixed"));
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Maximum", DoubleValue (-110));
      yAxis.Get<netsimulyzer::ValueAxis> ()->SetAttribute ("Minimum", DoubleValue (-130));

      m2eCollection = CreateObject <netsimulyzer::SeriesCollection> (orchestrator);
      m2eCollection->SetAttribute ("Name", StringValue ("Mouth-to-Ear latency"));
      m2eCollection->GetXAxis()->SetAttribute ("Name", StringValue ("Time (s)"));
      m2eCollection->GetYAxis()->SetAttribute ("Name", StringValue ("Latency (ms)"));

      accessTimeCollection = CreateObject <netsimulyzer::SeriesCollection> (orchestrator);
      accessTimeCollection->SetAttribute ("Name", StringValue ("Access Time"));
      accessTimeCollection->GetXAxis()->SetAttribute ("Name", StringValue ("Time (s)"));
      accessTimeCollection->GetYAxis()->SetAttribute ("Name", StringValue ("Access Time (ms)"));


      //std::map <uint32_t, MessagesTracer>::iterator rxMessagesPerUeTracerMapIt;
      for (rxMessagesPerUeTracerMapIt = rxMessagesPerUeTracerMap.begin (); rxMessagesPerUeTracerMapIt != rxMessagesPerUeTracerMap.end (); rxMessagesPerUeTracerMapIt++)
        {
          uint32_t ueNodeIdx = rxMessagesPerUeTracerMapIt->first;
          //Add RSRP measurements of certain nodes in a collection
          if (ueNodeIdx == 2 || ueNodeIdx == 6 || ueNodeIdx == 11)
            {
              //rxMessagesPerUeTracerMapIt->second.m_rsrpSeries->SetAttribute ("Name", StringValue("UE " + std::to_string(ueNodeIdx) + " (team " + std::to_string(team) + ")"));
              rsrpCollection->Add (rxMessagesPerUeTracerMapIt->second.m_rsrpSeries);
            }

          if (ueNodeIdx == 1 || ueNodeIdx == 5 || ueNodeIdx == 9)
            {
              accessTimeCollection->Add (rxMessagesPerUeTracerMapIt->second.m_accessTimeSeries);
            }

          //Add media measurements of certain nodes in a collection
          if (ueNodeIdx == 2 || ueNodeIdx == 6 || ueNodeIdx == 11)
            {
              m2eCollection->Add (rxMessagesPerUeTracerMapIt->second.m_mediaDelaySeries);
            }
        }
    }
#endif

  NS_LOG_INFO ("Simulation time " << simTime << " s");
  NS_LOG_INFO ("Starting simulation...");

  Simulator::Stop (Seconds (simTime));
  Simulator::Run ();

  NS_LOG_INFO ("End of simulation...");


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
