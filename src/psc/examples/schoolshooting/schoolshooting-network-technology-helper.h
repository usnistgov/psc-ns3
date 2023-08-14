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

#include "ns3/pointer.h"
#include "ns3/node.h"
#include "ns3/net-device-container.h"
#include "ns3/psc-scenario-definition.h"
#include "ns3/object.h"

#ifndef SCHOOLSHOOTING_NETWORK_TECHNOLOGY_HELPER_H
#define SCHOOLSHOOTING_NETWORK_TECHNOLOGY_HELPER_H

namespace ns3 {
namespace psc {

/**
 * \ingroup pscincident
 *
 * Helper class to configure the network technology of the school shooting
 * incident.
 */
class SchoolShootingNetworkTechnologyHelper : public Object
{
public:
  /**
   * Gets the type ID.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Default destructor.
   */
  ~SchoolShootingNetworkTechnologyHelper ();
  /**
   * Get public safety application server
   * \return The public safety application server
   */
  virtual Ptr<Node> GetPsServer (void) = 0;
  /**
   * Get school application server
   * \return The school application server
   */
  virtual Ptr<Node> GetSchoolServer (void) = 0;
  /**
   * Get city application server
   * \return The city application server
   */
  virtual Ptr<Node> GetCityServer (void) = 0;
  /**
   * Get public application server
   * \return The public application server
   */
  virtual Ptr<Node> GetPublicServer (void) = 0;
  /**
   * Gets the NetDevices belonging to the given group
   * \param id The ID of the group
   * \return The NetDevice container
   */
  virtual NetDeviceContainer GetNetDevicesForGroup (PscScenarioDefinition::GroupId id) = 0;
protected:
  /**
   * Default constructor.
   */
  SchoolShootingNetworkTechnologyHelper ();
};

} // namespace psc
} // namespace ns3

#endif // SCHOOLSHOOTING_NETWORK_TECHNOLOGY_HELPER_H
