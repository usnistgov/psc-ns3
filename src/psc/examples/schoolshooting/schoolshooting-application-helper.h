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

#include "ns3/point-to-point-epc-helper.h"
#include "ns3/psc-application-helper.h"
#include "ns3/mcptt-trace-helper.h"
#include "ns3/mcptt-pusher-orchestrator.h"

#include "schoolshooting-network-technology-helper.h"
#include "schoolshooting-definition-helper.h"

#ifndef SCHOOLSHOOTING_APPLICATION_HELPER_H
#define SCHOOLSHOOTING_APPLICATION_HELPER_H

namespace ns3 {
namespace psc {

class SchoolShootingApplicationTraceHelper;

/**
 * \ingroup pscincident
 *
 * Helper class to configure the school shooting incident using an LTE macro
 * cell deployment.
 */
class SchoolShootingApplicationHelper : public Object
{
public:
  /**
    * Enumeration with the type of applications defined and
    * with a default configuration stored in the PscApplicationHelper class
    */
  enum SchoolSchootingApplicationId : PscScenarioDefinition::ApplicationId
  {
    SCHOOL_SHOOTING_MCPTT,
    SCHOOL_SHOOTING_TACTICAL_TELEMETRY,
    SCHOOL_SHOOTING_BIOMETRICS,
    SCHOOL_SHOOTING_SWAT_HELMET_CAMERA,
    SCHOOL_SHOOTING_THROW_PHONE,
    SCHOOL_SHOOTING_AVL,
    SCHOOL_SHOOTING_WEB,
    SCHOOL_SHOOTING_RMS_BCA,
    SCHOOL_SHOOTING_SATELLITE,
    SCHOOL_SHOOTING_BUILDING_PLANS,
    SCHOOL_SHOOTING_INCIDENT_MANAGEMENT,
    SCHOOL_SHOOTING_NG911,
    SCHOOL_SHOOTING_SCHOOL_VIDEO,
    SCHOOL_SHOOTING_TRAFFIC_VIDEO,
    SCHOOL_SHOOTING_DEPLOYABLE_CAMERA,
    SCHOOL_SHOOTING_HELICOPTER,
    SCHOOL_SHOOTING_EMS_VEHICLES,
    SCHOOL_SHOOTING_VIDEO_CONFERENCE,
    SCHOOL_SHOOTING_LAST,
    SCHOOL_SHOOTING_THROW_PHONE_VIDEO, // Used only to identify throw phone video trace output stream
    SCHOOL_SHOOTING_VIDEO_CONFERENCE_VIDEO, // Used only to identify audio conference video trace output stream
    SCHOOL_SHOOTING_VIDEO_CONFERENCE_AUDIO // Used only to identify audio conference audio trace output stream
  };
  /**
   * Default constructor.
   */
  SchoolShootingApplicationHelper ();
  /**
   * Default destructor.
   */
  ~SchoolShootingApplicationHelper ();
  /**
   * Get the TypeId for this class.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * Initialize scenario
   */
  void Initialize ();
  /**
   * Enable a particular application.
   * By default all the applications are enabled. By using this function, the
   * user can enable particular applications, while all others will not be
   * deployed. The function can be call multiple times to enable multiple
   * applications
   * \param appName The name of the application to trace
   */
  void EnableApplication (SchoolSchootingApplicationId appName);
  /**
   * Enable traces for all the active applications
   */
  void EnableApplicationTraces ();
  /**
   * Enable traces for the given application
   * \param appName The name of the application to trace
   */
  void EnableApplicationTraces (SchoolSchootingApplicationId appName);
  /**
   * Gets the list of enabled applications
   * \return The set of enabled applications
   */
  std::set <SchoolSchootingApplicationId> GetEnabledApplications ();
  /**
   * Gets the list of PscApplication configured in this scenario
   * \param appName The name of the application to trace
   * \return The list of PscApplication configured in this scenario
   */
  ApplicationContainer GetPscApplications (SchoolSchootingApplicationId appName);
  /**
   * Gets the list of application transmitting SWAT Helmet cameras
   * \return The list of application transmitting SWAT Helmet cameras
   */
  ApplicationContainer GetSwatHelmetCamerasTxApplications ();
  /**
   * Gets the list of application receiving SWAT Helmet cameras
   * \return The list of application receiving SWAT Helmet cameras
   */
  ApplicationContainer GetSwatHelmetCamerasRxApplications ();
  /**
   * Gets the list of application transmitting deployable cameras
   * \return The list of application transmitting deployable cameras
   */
  ApplicationContainer GetDeployableCamerasTxApplications ();
  /**
   * Gets the list of application receiving deployable Helmet cameras
   * \return The list of application receiving deployable Helmet cameras
   */
  ApplicationContainer GetDeployableCamerasRxApplications ();
  /**
   * Gets the list of application transmitting throw phone cameras
   * \return The list of application transmitting throw phone cameras
   */
  ApplicationContainer GetThrowPhoneCamerasTxApplications ();
  /**
   * Gets the list of application receiving throw phone cameras
   * \return The list of application receiving throw phone cameras
   */
  ApplicationContainer GetThrowPhoneCamerasRxApplications ();
  /**
   * Gets the list of application transmitting helicopter cameras
   * \return The list of application transmitting helicopter cameras
   */
  ApplicationContainer GetHelicopterCamerasTxApplications ();
  /**
   * Gets the list of application receiving helicopter cameras
   * \return The list of application receiving helicopter cameras
   */
  ApplicationContainer GetHelicopterCamerasRxApplications ();
  /**
   * Gets the list of application transmitting EMS vehicle cameras
   * \return The list of application transmitting EMS vehicle cameras
   */
  ApplicationContainer GetEmsVehicleCamerasTxApplications ();
  /**
   * Gets the list of application receiving EMS vehicle cameras
   * \return The list of application receiving EMS vehicle cameras
   */
  ApplicationContainer GetEmsVehicleCamerasRxApplications ();
  /**
   * Gets the list of application transmitting video conference video
   * \return The list of application transmitting video conference video
   */
  ApplicationContainer GetVideoConferenceVideoTxApplications ();
  /**
   * Gets the list of application receiving video conference video
   * \return The list of application receiving video conference video
   */
  ApplicationContainer GetVideoConferenceVideoRxApplications ();
  /**
   * Gets the list of application transmitting school cameras
   * \return The list of application transmitting school cameras
   */
  ApplicationContainer GetSchoolCamerasTxApplications ();
  /**
   * Gets the list of application receiving school cameras
   * \return The list of application receiving school cameras
   */
  ApplicationContainer GetSchoolCamerasRxApplications ();
  /**
   * Gets the list of application transmitting traffic cameras
   * \return The list of application transmitting traffic cameras
   */
  ApplicationContainer GetTrafficCamerasTxApplications ();
  /**
   * Gets the list of application receiving traffic cameras
   * \return The list of application receiving traffic cameras
   */
  ApplicationContainer GetTrafficCamerasRxApplications ();
  /**
   * Gets the list of application transmitting web content
   * \return The list of application transmitting web content
   */
  ApplicationContainer GetWebTxApplications ();
  /**
   * Gets the list of application receiving web content
   * \return The list of application receiving web content
   */
  ApplicationContainer GetWebRxApplications ();
  /**
   * Gets the list of application transmitting NG911 content
   * \return The list of application transmitting NG911 content
   */
  ApplicationContainer GetNg911TxApplications ();
  /**
   * Gets the list of application receiving NG911 content
   * \return The list of application receiving NG911 content
   */
  ApplicationContainer GetNg911RxApplications ();
  /**
   * Print traces related to MCPTT
   */
  void EnableMcpttTraces ();
  /**
   * Sets the underlying technology helper.
   * \param techHelper The technology helper.
   */
  void SetTechnologyHelper (Ptr<SchoolShootingNetworkTechnologyHelper> techHelper);
  /**
   * Gets the underlying technology helper.
   * \returns The technology helper.
   */
  Ptr<SchoolShootingNetworkTechnologyHelper> GetTechnologyHelper (void);
  /**
   * Set the scenario definition helper
   * \param definitionHelper The scenario definition helper
   */
  void SetScenarioDefinitionHelper (Ptr<SchoolShootingDefinitionHelper> definitionHelper);
  /**
   * Returns the scenario definition
   * \return The scenario definition
   */
  Ptr<PscScenarioDefinition> GetScenarioDefinition ();
protected:
  /**
   * Initialize the enabled applications
   *
   * This function calls the individual initialization functions to configure
   * and schedule the start and stop times of each enabled application (e.g.
   * InitMcpttApp). Therefore, only this function should be called to setup
   * the applications so that only those that are enabled are configured and
   * active during the simulation, and thus the individual functions should
   * not be called. If the enabled application list is empty, then all
   * applications will be enabled, otherwise only those applications that
   * were specified through the EnableApplication function will be enabled.
   */
  void InitApplications ();
  /**
   * Initialize the MCPTT application
   */
  void InitMcpttApp ();
  /**
   * Initialize the Tactical telemetry application
   */
  void InitTacticalTelemetryApp ();
  /**
   * Initialize the biometric application used by the SWAT teams and victims' monitoring
   */
  void InitBiometricApp ();
  /**
   * Initialize the helmet camera application
   */
  void InitSwatHelmetCameraApp ();
  /**
   * Initialize the helmet camera application
   */
  void InitDeployableCameraApp ();
  /**
   * Initialize the throw phone application
   */
  void InitThrowPhoneApp ();
  /**
   * Initialize the helicopter application
   */
  void InitHelicopterApp ();
  /**
   * Initialize the EMS vehicle application
   */
  void InitEmsVehicleApp ();
  /**
   * Initialize the video conference application
   */
  void InitVideoConferenceApp ();
  /**
   * Initialize the AVL application
   */
  void InitAvlApp ();
  /**
   * Initialize the Web applications
   */
  void InitWebApp ();
  /**
   * Initialize the Records Management System (RMS) and Bureau of Criminal
   * Apprehension (BCA) data bases applications
   */
  void InitRmsBcaApp ();
  /**
   * Initialize the satellite images application
   */
  void InitSatelliteApp ();
  /**
   * Initialize the building plan application
   */
  void InitBuildingPlanApp ();
  /**
   * Initialize the incident management application
   */
  void InitIncidentManagementApp ();
  /**
   * Initialize the NG911 application
   */
  void InitNg911App ();
  /**
   * Initialize the school video application
   */
  void InitSchoolVideoApp ();
  /**
   * Initialize the traffic video application
   */
  void InitTrafficVideoApp ();
  /**
   * Add a new PscApplication
   * \param name The application name
   * \param application The new application
   */
  void AddPscApplication (SchoolSchootingApplicationId name, Ptr<PscApplication> application);
  /**
   * Add new PscApplications
   * \param name The application name
   * \param applications Container with PscApplications
   */
  void AddPscApplications (SchoolSchootingApplicationId name, ApplicationContainer applications);
private:
  /**
   * Indicates if the scenario has been initialized
   */
  bool m_initialized;
  /**
   * The MCPTT trace helper
   */
  Ptr<McpttTraceHelper> m_mcpttTraceHelper; 

  /**
   * Whether MCPTT traces are enabled
   */
  bool m_mcpttTracesEnabled {false};
  /**
   * Indicates if we are testing the scenario with modified start/stop of applications
   */
  bool m_testing;
  /**
   * Helper for configuration applications based on generic traffic
   */
  Ptr<PscApplicationHelper> m_genericApplicationHelper;
  /**
   * Map with the base ports used for each application
   */
  std::map <SchoolSchootingApplicationId, uint32_t> m_basePortsMap;
  /**
   * List of applications to enable. If empty, all the applications will be
   * enabled.
   */
  std::set <SchoolSchootingApplicationId> m_enabledApps;
  /**
   * The underlying technology helper.
   */
  Ptr<SchoolShootingNetworkTechnologyHelper> m_techHelper;
  /**
   * The underlying definition helper.
   */
  Ptr<SchoolShootingDefinitionHelper> m_scenarioDefinitionHelper;
  /**
   * Helper for traces
   */
  Ptr<SchoolShootingApplicationTraceHelper> m_traceHelper;
  /**
   * Container for SWAT team helmet cameras applications (sender)
   */
  ApplicationContainer m_swatHelmetCamerasTx;
  /**
   * Container for SWAT team helmet cameras applications (UC receiver)
   */
  ApplicationContainer m_swatHelmetCamerasRx;
  /**
   * Container for deployable cameras applications (sender)
   */
  ApplicationContainer m_deployableCamerasTx;
  /**
   * Container for deployable cameras applications (UC receiver)
   */
  ApplicationContainer m_deployableCamerasRx;
  /**
   * Container for throw phone cameras applications (sender)
   */
  ApplicationContainer m_throwPhoneCamerasTx;
  /**
   * Container for throw phone cameras applications (UC receiver)
   */
  ApplicationContainer m_throwPhoneCamerasRx;
  /**
   * Container for helicopter cameras applications (sender)
   */
  ApplicationContainer m_helicopterCamerasTx;
  /**
   * Container for helicopter cameras applications (UC receiver)
   */
  ApplicationContainer m_helicopterCamerasRx;
  /**
   * Container for EMS vehicle cameras applications (sender)
   */
  ApplicationContainer m_emsVehicleCamerasTx;
  /**
   * Container for EMS vehicle cameras applications (UC receiver)
   */
  ApplicationContainer m_emsVehicleCamerasRx;
  /**
   * Container for video conference video applications
   */
  ApplicationContainer m_videoConferenceVideoTx;
  /**
   * Container for video conference video applications
   */
  ApplicationContainer m_videoConferenceVideoRx;
  /**
   * Container for school cameras applications (sender)
   */
  ApplicationContainer m_schoolCamerasTx;
  /**
   * Container for school cameras applications (UC receiver)
   */
  ApplicationContainer m_schoolCamerasRx;
  /**
   * Container for traffic cameras applications (sender)
   */
  ApplicationContainer m_trafficCamerasTx;
  /**
   * Container for traffic cameras applications (UC receiver)
   */
  ApplicationContainer m_trafficCamerasRx;
  /**
   * Container for web applications (sender)
   */
  ApplicationContainer m_webTx;
  /**
   * Container for web applications (receiver)
   */
  ApplicationContainer m_webRx;
  /**
   * Container for NG911 applications (sender)
   */
  ApplicationContainer m_ng911Tx;
  /**
   * Container for NG911 applications (receiver)
   */
  ApplicationContainer m_ng911Rx;
  /**
   * Map PSC application name to application container with instances
   */
  std::map <SchoolSchootingApplicationId, ApplicationContainer> m_pscApplicationsMap;
  /**
   * Container for MCPTT pusher orchestrators
   */
  std::vector<Ptr<McpttPusherOrchestratorInterface> > m_pttOrchestrators;
}; // class SchoolShootingApplicationHelper

} // namespace psc
} // namespace ns3

#endif // SCHOOLSHOOTING_APPLICATION_HELPER_H
