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

#ifndef SCHOOLSHOOTING_LTE_TRACE_HELPER
#define SCHOOLSHOOTING_LTE_TRACE_HELPER

#include "map"
#include "ns3/packet.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/output-stream-wrapper.h"

#include "schoolshooting-application-helper.h"

namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 *
 * Helper class to provide the trace sinks for the incident modeling
 * in a single place.
 */

class SchoolShootingApplicationTraceHelper : public Object
{
public:
  /**
   * Default constructor
   */
  SchoolShootingApplicationTraceHelper ();

  /**
   * Constructor for a given application
   * \param applicationHelper The application helper
   */
  SchoolShootingApplicationTraceHelper (Ptr<SchoolShootingApplicationHelper> applicationHelper);

  /**
   * Default destructor
   */
  ~SchoolShootingApplicationTraceHelper ();

  /**
   * Get the TypeId for this class.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Connect the PscApplication trace source with a trace sink that will print
   * the start and stop time of the application to a file
   * \param name Name of application to trace
   */
  void EnablePscAppsTimeTrace (SchoolShootingApplicationHelper::SchoolSchootingApplicationId name);

  /**
   * Trace sink for the applications start and stop time.
   *
   * \param appOutputStreamWrapper The trace wrapper
   * \param appName The name of the application that is being traced.
   * \param startTime The time at which this application starts.
   * \param stopTime The time at which this application stops.
   */
  static void TraceAppTime (Ptr<OutputStreamWrapper> appOutputStreamWrapper, std::string appName, Time startTime, Time stopTime);

  /**
   * Connect the PscApplication trace source with a trace sink that will print
   * the packets sent and received to a file
   * \param name Name of application to trace
   */
  void EnablePscAppsTrafficTrace (SchoolShootingApplicationHelper::SchoolSchootingApplicationId name);

  /**
   * Trace sink for the application RX trace.
   *
   * \param appOutputStreamWrapper The trace wrapper
   * \param appName The name of the application that is being traced.
   * \param header The header of the packet being traced
   */
  static void TraceAppRxTraffic (Ptr<OutputStreamWrapper> appOutputStreamWrapper, std::string appName, SeqTsSizeHeader header);

  /**
   * Trace sink for the application TX trace.
   *
   * \param appOutputStreamWrapper The trace wrapper
   * \param appName The name of the application that is being traced.
   * \param header The header of the packet being traced
   */
  static void TraceAppTxTraffic (Ptr<OutputStreamWrapper> appOutputStreamWrapper, std::string appName, SeqTsSizeHeader header);

  /**
   * Enables traces for the SWAT team helmet cameras
   * \param txApps The list of transmitting applications to trace
   * \param rxApps The list of receiving applications to trace
   */
  void EnableSwatHelmetCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps);

  /**
   * Trace sink for the transmission of packets by the SwatHelmetCamera application
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet sent
   */
  static void TraceTxSwatHelmetCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the SwatHelmetCamera application
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxSwatHelmetCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables traces for the deployable cameras
   * \param txApps The list of transmitting applications to trace
   * \param rxApps The list of receiving applications to trace
   */
  void EnableDeployableCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps);

  /**
   * Trace sink for the transmission of packets by deployable camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet sent
   */
  static void TraceTxDeployableCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the deployable camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxDeployableCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables traces for the throw phone cameras
   * \param txApps The list of transmitting applications to trace
   * \param rxApps The list of receiving applications to trace
   */
  void EnableThrowPhoneCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps);

  /**
   * Trace sink for the transmission of packets by throw phone camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet sent
   */
  static void TraceTxThrowPhoneCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the throw phone camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxThrowPhoneCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables traces for the helicopter cameras
   * \param txApps The list of transmitting applications to trace
   * \param rxApps The list of receiving applications to trace
   */
  void EnableHelicopterCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps);

  /**
   * Trace sink for the transmission of packets by helicopter camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet sent
   */
  static void TraceTxHelicopterCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the helicopter camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxHelicopterCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables traces for the EMS vehicle cameras
   * \param txApps The list of transmitting applications to trace
   * \param rxApps The list of receiving applications to trace
   */
  void EnableEmsVehicleCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps);

  /**
   * Trace sink for the transmission of packets by EMS vehicle camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet sent
   */
  static void TraceTxEmsVehicleCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the EMS vehicle camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxEmsVehicleCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables traces for the video conference video
   * \param txApps The list of transmitting applications to trace
   * \param rxApps The list of receiving applications to trace
   */
  void EnableVideoConferenceVideoTraces (ApplicationContainer txApps, ApplicationContainer rxApps);

  /**
   * Trace sink for the transmission of packets by video conference video applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet sent
   */
  static void TraceTxVideoConferenceVideo (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the video conference video applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxVideoConferenceVideo (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables traces for the school cameras
   * \param txApps The list of transmitting applications to trace
   * \param rxApps The list of receiving applications to trace
   */
  void EnableSchoolCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps);

  /**
   * Trace sink for the transmission of packets by school camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet sent
   */
  static void TraceTxSchoolCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the school camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxSchoolCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables traces for the traffic cameras
   * \param txApps The list of transmitting applications to trace
   * \param rxApps The list of receiving applications to trace
   */
  void EnableTrafficCameraTraces (ApplicationContainer txApps, ApplicationContainer rxApps);

  /**
   * Trace sink for the transmission of packets by traffic camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet sent
   */
  static void TraceTxTrafficCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the traffic camera applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxTrafficCamera (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables traces for the web applications
   * \param txApps The list of transmitting applications to trace
   * \param rxApps The list of receiving applications to trace
   */
  void EnableWebTraces (ApplicationContainer txApps, ApplicationContainer rxApps);

  /**
   * Trace sink for the transmission of packets by web applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet sent
   */
  static void TraceTxWeb (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the web applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxWeb (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs);

  /**
   * Enables traces for the NG911 applications
   * \param txApps The list of transmitting applications to trace
   * \param rxApps The list of receiving applications to trace
   */
  void EnableNg911Traces (ApplicationContainer txApps, ApplicationContainer rxApps);

  /**
   * Trace sink for the transmission of packets by NG911 applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet sent
   */
  static void TraceTxNg911 (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p);

  /**
   * Trace sink for the reception of packets by the NG911 applications
   * 
   * \param outputStreamWrapper The trace wrapper
   * \param node The sender node
   * \param p The packet received
   * \param srcAddrs The address of the sender
   */
  static void TraceRxNg911 (Ptr<OutputStreamWrapper> outputStreamWrapper, Ptr<Node> node, Ptr<const Packet> p, const Address &srcAddrs);

private:
  /**
   * The application helper
   */
  Ptr<SchoolShootingApplicationHelper> m_applicationHelper;

  /**
   * OutputStreamWrappers for writing statistics.
   */
  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > m_outputStreamWrappers;

  /**
   * OutputStreamWrappers for writing statistics for PSC Applications.
   */
  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > m_appTimeOutputStreamWrappers;

  /**
   * OutputStreamWrappers for writing statistics for PSC Applications.
   */
  std::map<SchoolShootingApplicationHelper::SchoolSchootingApplicationId, Ptr<OutputStreamWrapper> > m_appTrafficOutputStreamWrappers;

  /**
   * Set the precision to use when printing double numbers.
   *
   * \param outputPrecision The precision to use
   */
  static void SetOutputPrecision (uint16_t outputPrecision);

  /**
   * Get the precision to use when printing double numbers.
   *
   * \return The precision.
   */
  static uint16_t GetOutputPrecision (void);

  /**
   * Precision to use when printing decimal numbers. This value includes the digits
   * of the integer part.
   */
  static uint16_t s_outputPrecision;

}; // SchoolShootingApplicationTraceHelper

} // namespace psc
} // namespace ns3

#endif // SCHOOLSHOOTING_LTE_TRACE_HELPER

