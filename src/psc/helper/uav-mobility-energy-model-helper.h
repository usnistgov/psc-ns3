/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 * 
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 * 
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */


#ifndef NS3_UAV_MOBILITY_ENERGY_MODEL_HELPER_H
#define NS3_UAV_MOBILITY_ENERGY_MODEL_HELPER_H

#include <ns3/attribute.h>
#include <ns3/device-energy-model.h>
#include <ns3/device-energy-model-container.h>
#include <ns3/energy-source.h>
#include <ns3/energy-source-container.h>
#include <ns3/energy-model-helper.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>
#include <ns3/ptr.h>
#include <ns3/uav-mobility-energy-model.h>

namespace ns3 {

namespace psc {

/**
 * \ingroup psc
 * \brief Creates UavMobilityEnergyModels
 * 
 */
class UavMobilityEnergyModelHelper 
{
public:
  UavMobilityEnergyModelHelper (void);

  /**
   * Sets one of the attributes of underlying UavMobilityEnergyModel
   * 
   * \param name Name of attribute to set.
   * \param v Value of the attribute.
   */
  void Set (const std::string &name, const AttributeValue &v);

  /**
   * \brief Install the configured UavMobilityEnergyModel on node
   * 
   * Installs & Aggregates the configured UavMobilityEnergyModel on node.
   * 
   * If no mobility model is present on node, then one will
   * be produced from the configuration set from SetMobilityModel
   * & aggregated onto node
   * 
   * If no energy source is present on node, then one will
   * be produced from the configuration set from SetEnergySource
   * & aggregated onto node
   * 
   * If SetMobilityModel has not been called,
   * and no MobilityModel has been installed on node
   * then this method will abort
   * 
   * If SetEnergySource has not been called,
   * and no EnergySource has been installed on node
   * then this method will abort
   * 
   * \param node Pointer to the Node to install the energy model on.
   * \return DeviceEnergyModelContainer
   *    A container containing the installed model
   */
  DeviceEnergyModelContainer Install (Ptr<Node> node) const;

  /**
   * \brief Install the configured UavMobilityEnergyModel on node
   * 
   * Installs & Aggregates the configured UavMobilityEnergyModel on node.
   * 
   * If no mobility model is present on node, then one will
   * be produced from the configuration set from SetMobilityModel
   * & aggregated onto node
   * 
   * If SetMobilityModel has not been called,
   * and no MobilityModel has been installed on node
   * then this method will abort
   * 
   * \param node Pointer to the Node to install the energy model on.
   * \param source The energy source the model will draw from
   * \return DeviceEnergyModelContainer
   *    A container containing the installed model
   */
  DeviceEnergyModelContainer Install (Ptr<Node> node,
                                      Ptr<EnergySource> source) const;

  /**
   * \brief Install the configured UavMobilityEnergyModel
   * on a collection of nodes
   * 
   * Installs & Aggregates the configured UavMobilityEnergyModel on each node.
   * 
   * If no mobility model is present on any given node, then one will
   * be produced from the configuration set from SetMobilityModel
   * & aggregated onto each node
   * 
   * If no energy source is present on any given node, then one will
   * be produced from the configuration set from SetEnergySource
   * & aggregated onto each node
   * 
   * If SetMobilityModel has not been called,
   * and no MobilityModel has been installed on any given node
   * then this method will abort
   * 
   * If SetEnergySource has not been called,
   * and no EnergySource has been installed on any given node
   * then this method will abort
   * 
   * \param nodes The collection of nodes to install the EnergyModel on
   * 
   * \return DeviceEnergyModelContainer
   *    A container containing the installed models
   */
  DeviceEnergyModelContainer Install (NodeContainer &nodes) const;

  /**
   * \brief Install the configured UavMobilityEnergyModel on each node
   * 
   * Installs & Aggregates the configured UavMobilityEnergyModel on each node.
   * 
   * There must be at least as many EnergySource(s) in sources
   * as there are Node(s) in nodes! Otherwise this method will
   * abort.
   * 
   * If no mobility model is present on any given node, then one will
   * be produced from the configuration set from SetMobilityModel
   * & aggregated onto each node
   * 
   * If SetMobilityModel has not been called,
   * and no MobilityModel has been installed on any given node
   * then this method will abort
   * 
   * \param nodes The collection of Node(s) to install the energy model on.
   *    There must be at least as many Node(s)Nodes in nodes as there are
   *    EnergySource(s) in sources.
   *   
   * \param sources The collection of EnergySource(s) to install
   *    on the Node(s) in nodes. There must be at least as many EnergySource(s)
   *    as there are Node(s) in nodes.
   *    Any extra EnergySource(s) will be ignored,
   *    and a warning will be logged.
   * 
   * \return DeviceEnergyModelContainer
   *    A container containing the installed model
   */
  DeviceEnergyModelContainer Install (NodeContainer &nodes,
                                      EnergySourceContainer &sources) const;


  /**
   * \param name the name of the model to set
   * \param n0 the name of the attribute to set
   * \param v0 the value of the attribute to set
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   * \param n2 the name of the attribute to set
   * \param v2 the value of the attribute to set
   * \param n3 the name of the attribute to set
   * \param v3 the value of the attribute to set
   * \param n4 the name of the attribute to set
   * \param v4 the value of the attribute to set
   * \param n5 the name of the attribute to set
   * \param v5 the value of the attribute to set
   * \param n6 the name of the attribute to set
   * \param v6 the value of the attribute to set
   * \param n7 the name of the attribute to set
   * \param v7 the value of the attribute to set
   *
   * Configure a Mobility Model to attach to each node
   * If not called, then provided nodes must have a
   * MobilityModel already attached
   */
  void SetMobilityModel (std::string name,
                         std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                         std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                         std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                         std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                         std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                         std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                         std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                         std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
  
  /**
   * \param name the name of the model to set
   * \param n0 the name of the attribute to set
   * \param v0 the value of the attribute to set
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   * \param n2 the name of the attribute to set
   * \param v2 the value of the attribute to set
   * \param n3 the name of the attribute to set
   * \param v3 the value of the attribute to set
   * \param n4 the name of the attribute to set
   * \param v4 the value of the attribute to set
   * \param n5 the name of the attribute to set
   * \param v5 the value of the attribute to set
   * \param n6 the name of the attribute to set
   * \param v6 the value of the attribute to set
   * \param n7 the name of the attribute to set
   * \param v7 the value of the attribute to set
   *
   * Configure a EnergySource to attach to each node
   * If not called, then provided nodes must have an
   * EnergySource already attached
   */
  void SetEnergySource (std::string name,
                         std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                         std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                         std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                         std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                         std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                         std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                         std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                         std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());


private:
  ObjectFactory m_energyModel; //!< object factory for creating energy model
  ObjectFactory m_energySource; //!< object factory for creating energy source
  ObjectFactory m_mobilityModel; //!< object factory for creating mobility model

  /**
   * Checks if a MobilityModel is installed on node
   * If so, returns that model
   * If not, Tries to create one (with m_mobilityModel),
   * installs it, aggregates it to node, and returns it
   * 
   * \param node The Node to check for a MobilityModel.
   *    Will also have a created MobilityModel aggregated onto it,
   *    if one hasn't been already.
   * 
   * \return Ptr<MobilityModel> The existing MobilityModel on node
   *    (if one was already aggregated), or the newly created
   *    MobilityModel, with the configuration from m_mobilityModel.
   */
  Ptr<MobilityModel> CheckInstallMobilityModel (Ptr<Node> node) const;

  /**
   * Checks if an EnergySource is installed on node
   * If so, returns that EnergySource
   * If not, Tries to create one (with m_energySource),
   * installs it, aggregates it to node, and returns it
   * 
   * Note: if no MobilityModel is aggregated onto node and
   * m_mobilityModel has not been configured (through SetMobilityModel ())
   * then this method will abort
   * 
   * \param node The Node to check for an EnergySource.
   *    Will also have a created EnergySource aggregated onto it,
   *    if one hasn't been already.
   * 
   * \return Ptr<EnergySource> The existing EnergySource on node
   *    (if one was already aggregated), or the newly created
   *    EnergySource, with the configuration from m_energySource.
   */
  Ptr<EnergySource> CheckInstallEnergySource (Ptr<Node> node) const;

  /**
   * Sets node as the Node on source,
   * Creates a model using m_energyModel,
   * Init ()'s  it using node & source,
   * and aggregates the created model to node.
   * 
   * Note: if no EnergySource is aggregated onto node and
   * m_energySource has not been configured (through SetEnergySource ())
   * then this method will abort
   * 
   * \param node The Node to connect to source
   *    and the created UavMobilityEnergyModel.
   *    A MobilityModel must already have been
   *    aggregated onto node.
   * 
   * \param source The EnergySource for the created UavMobilityEnergyModel
   *    to draw from.
   * 
   * \return Ptr<DeviceEnergyModel> An Init ()'ed UavMobilityEnergyModel.
   */
  Ptr<DeviceEnergyModel> DoInstall (Ptr<Node> node,
                                    Ptr<EnergySource> source) const;
};


} // namespace psc
} // namespace ns3

#endif //NS3_UAV_MOBILITY_ENERGY_MODEL_HELPER_H
