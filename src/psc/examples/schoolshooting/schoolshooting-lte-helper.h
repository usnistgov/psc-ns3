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

#ifndef SCHOOLSHOOTING_LTE_HELPER_H
#define SCHOOLSHOOTING_LTE_HELPER_H

#include "ns3/lte-helper.h"
#include "ns3/point-to-point-epc-helper.h"

#include "schoolshooting-definition-helper.h"
#include "schoolshooting-network-technology-helper.h"

namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 *
 * Helper class to configure the school shooting incident using an LTE macro
 * cell deployment.
 */
class SchoolShootingLteHelper : public SchoolShootingNetworkTechnologyHelper
{
public:
  /**
   * Default constructor.
   */
  SchoolShootingLteHelper (void);
  /**
   * Default destructor.
   */
  ~SchoolShootingLteHelper (void);
  /**
   * Get the TypeId for this class.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * Set the scenario definition helper
   * \param definitionHelper The scenario definition helper
   */
  void SetScenarioDefinitionHelper (Ptr<SchoolShootingDefinitionHelper> definitionHelper);
  
  /**
   * Returns the scenario definition
   * \return The scenario definition
   */
  Ptr<PscScenarioDefinition> GetScenarioDefinition (void) const;
  /**
   * Initialize scenario
   */
  void Initialize (void);
  /**
   * Get eNobeB nodes
   * \return The eNodeBs
   */
  NodeContainer GetEnodeBs (void);
  /**
   * Get SGW node
   * \return The SGW node
   */
  Ptr<Node> GetSgw (void);
  /**
   * Get PGW node
   * \return The PGW node
   */
  Ptr<Node> GetPgw (void);
  /**
   * Get public safety application server
   * \return The public safety application server
   */
  Ptr<Node> GetPsServer (void) override;
  /**
   * Get school application server
   * \return The school application server
   */
  Ptr<Node> GetSchoolServer (void) override;
  /**
   * Get city application server
   * \return The city application server
   */
  Ptr<Node> GetCityServer (void) override;
  /**
   * Get public application server
   * \return The public application server
   */
  Ptr<Node> GetPublicServer (void) override;
  /**
   * Enable LTE traces
   */
  void EnableLteTraces (void);
protected:
  /**
   * Configure the eNodeB, core network, and servers
   */
  void InitNetwork (void);
  /**
   * Configure LteNetDevices to the various nodes
   */
  void InitUes (void);
  /**
   * Gets the NetDevices belonging to the given group
   * \param id The ID of the group
   * \return The NetDevice container
   */
  NetDeviceContainer GetNetDevicesForGroup (PscScenarioDefinition::GroupId id) override;
private:
    
  /**
   * School shooting scenario definition helper
   */  
  Ptr<SchoolShootingDefinitionHelper> m_scenarioDefinitionHelper;
    
  /**
   * LteHelper to configure the LTE parameters, connections, etc.
   */
  Ptr<LteHelper> m_lteHelper;
  /**
   * EpcHelper to configure core network
   */
  Ptr<PointToPointEpcHelper> m_epcHelper;
  /**
   * Indicates if the scenario has been initialized
   */
  bool m_initialized;
  /**
   * Antenna height for the UEs.
   */
  double m_ueAntennaHeight;
  /**
   * Antenna height for the eNBs.
   */
  double m_eNbAntennaHeight;
  /**
   * Default LTE transmission mode (for MIMO).
   */
  uint8_t m_defaultTransmissionMode;
  /**
   * UE TX power.
   */
  double m_ueTxPower;
  /**
   * eNB TX power.
   */
  double m_eNbTxPower;
  /**
   * Pathloss model to use, as a string with the ns3 path to the class.
   */
  std::string m_pathLossModelType;
  /**
   * LTE scheduler to use, as a string with the ns3 path to the class.
   */
  std::string m_schedulerType;
  /**
   * Number of resource blocks to use in the DL channel.
   */
  uint8_t m_dlBandwidth;
  /**
   * Number of resource blocks to use in the UL channel.
   */
  uint8_t m_ulBandwidth;
  /**
   * Downlink EARFCN as per TS 36.101 Section 5.7.3. This determines the frequency band.
   */
  uint16_t m_dlEarfcn;
  /**
   * Uplink EARFCN as per TS 36.101 Section 5.7.3. This determines the frequency band.
   */
  uint16_t m_ulEarfcn;
  /**
   * Datarate for the S1U link
   */
  DataRate m_linkDataRate;
  /**
   * Delay for the S1U link
   */
  Time m_linkDelay;
  /**
   * Container for the eNodeB nodes
   */
  NodeContainer m_enbNodesContainer;
  /**
   * Public safety application server
   */
  Ptr<Node> m_psServer;
  /**
   * School server (where videos from school cameras are stored/streamed)
   */
  Ptr<Node> m_schoolServer;
  /**
   * City server (where videos from traffic cameras are stored/streamed and
   * building plans)
   */
  Ptr<Node> m_cityServer;
  /**
   * Public server for accessing information such as satellite images and other
   * online searches
   */
  Ptr<Node> m_publicServer;
  /**
   * Maps user groups with LteUeNetDevice
   */
  std::map<PscScenarioDefinition::GroupId, NetDeviceContainer> m_netDeviceGroups;
}; // class SchoolShootingLteHelper

} // namespace psc
} // namespace ns3

#endif // SCHOOLSHOOTING_LTE_HELPER_H
