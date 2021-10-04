/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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

#ifndef SCHOOLSHOOTING_LTE_VIS_HELPER
#define SCHOOLSHOOTING_LTE_VIS_HELPER

#ifdef HAS_NETSIMULYZER

#include "map"
#include "ns3/packet.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/netsimulyzer-module.h"

#include "schoolshooting-application-helper.h"

namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 *
 * Helper class to provide the trace sinks for the incident modeling
 * in a single place.
 */

class SchoolShootingLteVisHelper : public Object
{
public:

  struct McpttCallInfo 
  {
    uint16_t callId; //!< call Id
    std::map<uint32_t, std::pair<Time, std::string> > accessTimeMap; //!< state tracker
    std::map<uint32_t, Time> m2eLatencyMap; //!< latency tracker
    Ptr<netsimulyzer::XYSeries> accessTimeGraph; //!< access time graph
    Ptr<netsimulyzer::XYSeries> m2eLatencyGraph; //!< m2e latency graph
  };

  /**
   * Default constructor
   */
  SchoolShootingLteVisHelper ();

  /**
   * Constructor for a given scenario
   * \param scenarioHelper The scenario helper
   */
  SchoolShootingLteVisHelper (Ptr<PscScenarioDefinition> scenarioDefinition);

  /**
   * Constructor for a given scenario
   * \param scenarioHelper The scenario helper
   */
  SchoolShootingLteVisHelper (Ptr<SchoolShootingApplicationHelper> scenarioHelper);

  /**
   * Default destructor
   */
  ~SchoolShootingLteVisHelper ();

  /**
   * Get the TypeId for this class.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Returns the orchestrator
   * \return The orchestrator
   */
  Ptr<netsimulyzer::Orchestrator> GetOrchestrator ();

  /**
   * Set an attribute for the orchestrator to be created.
   *
   * \param n The name of the attribute.
   * \param v The value of the attribute
   */
  void SetOrchestratorAttribute (std::string n, const AttributeValue &v);

  /**
   * Set the interval between data points in the XY series
   * \param interval The interval between data points in the XY series
   */
  void SetInterval (Time interval);

  /**
   * Enable traces for the active applications
   */
  void EnableApplicationGraphs ();

  /**
   * Enable traces for the given application
   * \param appName The name of the application to trace
   */
  void EnableApplicationGraphs (SchoolShootingApplicationHelper::SchoolSchootingApplicationId appName);

  /**
   * Enable logging of key simulation events
   */
  void EnableKeyEventLog ();

  /**
   * Enables the traffic graphs for eNodeBs.
   */
  void EnableEnbThroughputGraphs (void);

protected:
  /**
   * Initialize buildings
   */
  void InitBuildings ();

  /**
   * Initialize areas
   */
  void InitAreas ();

  /**
   * Initialize node models
   */
  void InitNodeModels ();

  /**
   * Enables graphs for the SWAT team camera application
   */
  void EnableSwatHelmetCameraGraphs ();

  /**
   * Trace sink for the transmission of packets by the SwatHelmetCamera application
   * 
   * \param sink The sink collecting data
   * \param p The packet sent
   */
  static void TraceTxSwatHelmetCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the SwatHelmetCamera application
   * 
   * \param sink The sink collecting data
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxSwatHelmetCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables graphs for the deployable camera application
   */
  void EnableDeployableCameraGraphs ();

  /**
   * Trace sink for the transmission of packets by the deployable camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet sent
   */
  static void TraceTxDeployableCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the deployable camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxDeployableCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables graphs for the throw phone camera application
   */
  void EnableThrowPhoneCameraGraphs ();

  /**
   * Trace sink for the transmission of packets by the throw phone camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet sent
   */
  static void TraceTxThrowPhoneCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the throw phone camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxThrowPhoneCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables graphs for the helicopter camera application
   */
  void EnableHelicopterCameraGraphs ();

  /**
   * Trace sink for the transmission of packets by the helicopter camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet sent
   */
  static void TraceTxHelicopterCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the helicopter camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxHelicopterCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables graphs for the EMS vehicle camera application
   */
  void EnableEmsVehicleCameraGraphs ();

  /**
   * Trace sink for the transmission of packets by the EMS vehicle camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet sent
   */
  static void TraceTxEmsVehicleCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the EMS vehicle camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxEmsVehicleCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables graphs for the video conference video application
   */
  void EnableVideoConferenceVideoGraphs ();

  /**
   * Trace sink for the transmission of packets by the video conference video application
   * 
   * \param sink The sink collecting data
   * \param p The packet sent
   */
  static void TraceTxVideoConferenceVideo (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the video conference video application
   * 
   * \param sink The sink collecting data
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxVideoConferenceVideo (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables graphs for the school camera application
   */
  void EnableSchoolCameraGraphs ();

  /**
   * Trace sink for the transmission of packets by the school camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet sent
   */
  static void TraceTxSchoolCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the school camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxSchoolCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables graphs for the traffic camera application
   */
  void EnableTrafficCameraGraphs ();

  /**
   * Trace sink for the transmission of packets by the traffic camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet sent
   */
  static void TraceTxTrafficCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the traffic camera application
   * 
   * \param sink The sink collecting data
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxTrafficCamera (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables graphs for the web application
   */
  void EnableWebGraphs ();

  /**
   * Trace sink for the transmission of packets by the web application
   * 
   * \param sink The sink collecting data
   * \param p The packet sent
   */
  static void TraceTxWeb (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the web application
   * 
   * \param sink The sink collecting data
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxWeb (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables graphs for the NG911 application
   */
  void EnableNg911Graphs ();

  /**
   * Trace sink for the transmission of packets by the NG911 application
   * 
   * \param sink The sink collecting data
   * \param p The packet sent
   */
  static void TraceTxNg911 (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the NG911 application
   * 
   * \param sink The sink collecting data
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxNg911 (Ptr<netsimulyzer::ThroughputSink> sink, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables graphs for the given PscApplication application
   */
  void EnablePscAppsTrafficGraphs (SchoolShootingApplicationHelper::SchoolSchootingApplicationId name);

  /**
   * Trace sink for the transmission of packets by a PscApplication application
   * 
   * \param sink The sink collecting data
   * \param appName The application name
   * \param header The PSC header of the packet received
   */
  static void TraceAppTxTraffic (Ptr<netsimulyzer::ThroughputSink> sink, std::string appName, SeqTsSizeHeader header);

  /**
   * Trace sink for the reception of packets by a PscApplication application
   * 
   * \param sink The sink collecting data
   * \param appName The application name
   * \param header The PSC header of the packet received
   */
  static void TraceAppRxTraffic (Ptr<netsimulyzer::ThroughputSink> sink, std::string appName, SeqTsSizeHeader header);

  /**
   * Enables graphs for MCPTT application
   */
  void EnableMcpttGraphs ();

  /**
   * Trace sink for the access time
   * \param userId The user Id
   * \param callId The call Id
   * \param selected Indicates if this is the currently selected call
   * \param typeId The message type Id
   * \param oldStateName The name of the state before transition
   * \param newStateName The name of the state after transition
   */
  void TraceAccessTime (uint32_t userId, uint16_t callId, const std::string& selected, const std::string& typeId, const std::string& oldStateName, const std::string& newStateName);

  /**
   * Trace sink for the M2E latency
   * \param app The application
   * \param callId the ID of the call
   * \param pkt The packet received
   * \param headerType The header type
   */
  void TraceMouthToEarLatency (Ptr<const Application> app, uint16_t callId, Ptr<const Packet> pkt, const TypeId& headerType);

  /**
   * Trace sink for the transmission of packets by a PscApplication application
   * 
   * \param sink The sink collecting data
   * \param rnti C-RNTI scheduled.
   * \param lcid The logical chanel id corresponding to the sending RLC instance.
   */
  static void TraceEnbThroughputTx (Ptr<netsimulyzer::ThroughputSink> sink, uint16_t rnti, uint8_t lcid, uint32_t bytes);

  /**
   * Trace sink for the reception of packets by a PscApplication application
   * 
   * \param sink The sink collecting data
   * \param rnti C-RNTI scheduled.
   * \param lcid The logical chanel id corresponding to the sending RLC instance.
   * \param delay Delay since sender temestamp, in ns.
   */
  static void TraceEnbThroughputRx (Ptr<netsimulyzer::ThroughputSink> sink, uint16_t rnti, uint8_t lcid, uint32_t bytes, uint64_t delay);

private:
  /**
   * The scenario definition
   */
  Ptr<PscScenarioDefinition> m_scenarioDefinition;

  /**
   * The scenario helper
   */
  Ptr<SchoolShootingApplicationHelper> m_scenarioHelper;
  
  /**
   * Orchestrator for writing visualization information
   */
  Ptr<netsimulyzer::Orchestrator> m_orchestrator;

  /**
   * Default time for reporting graph values
   */
  Time m_interval;
  
  /**
   * List of throughput sinks collecting application information
   */
  std::list< Ptr<netsimulyzer::ThroughputSink> > m_sinks;

  /**
   * Map call ID to call information
   */
  std::map<uint16_t,  McpttCallInfo> m_callInfoByCallId; 

  /**
   * Collection for access times
   */
  Ptr<netsimulyzer::SeriesCollection> m_accessTimesCollection;

  /**
   * Collection for M2E latencies
   */
  Ptr<netsimulyzer::SeriesCollection> m_m2eLatencyCollection;
  /**
   * Collections for Helmet cameras
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_helmetCameraCollections;

  /**
   * Collections for deployable cameras
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_deployableCameraCollections;

  /**
   * Collections for throw phone cameras
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_throwPhoneCameraCollections;

  /**
   * Collections for helicopter cameras
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_helicopterCameraCollections;

  /**
   * Collections for EMS vehicle cameras
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_emsVehicleCameraCollections;

  /**
   * Collections for video conference video
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_videoConferenceVideoCollections;

  /**
   * Collections for video conference audio
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_videoConferenceAudioCollections;

  /**
   * Collections for school cameras
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_schoolCameraCollections;

  /**
   * Collections for traffic cameras
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_trafficCameraCollections;

  /**
   * Collections for web content
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_webCollections;

  /**
   * Collections for NG911 video
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_ng911Collections;

  /**
   * Collections for PscApplications
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_pscApplicationCollections;

  /**
   * Collections for eNodeBs
   */
  std::vector < Ptr<netsimulyzer::SeriesCollection> > m_enbThroughputCollections;

}; // SchoolShootingLteVisHelper

} // namespace psc
} // namespace ns3

#endif // HAS_NETSIMULYZER

#endif // SCHOOLSHOOTING_LTE_VIS_HELPER

