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

#include "uav-mobility-energy-model-helper.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("UavMobilityEnergyModelHelper");

namespace psc
{

UavMobilityEnergyModelHelper::UavMobilityEnergyModelHelper()
{
    NS_LOG_FUNCTION(this);
    m_energyModel.SetTypeId("ns3::psc::UavMobilityEnergyModel");
}

void
UavMobilityEnergyModelHelper::Set(const std::string& name, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_energyModel.Set(name, v);
}

energy::DeviceEnergyModelContainer
UavMobilityEnergyModelHelper::Install(Ptr<Node> node) const
{
    NS_LOG_FUNCTION(this << node);
    CheckInstallMobilityModel(node);
    auto energySource = CheckInstallEnergySource(node);

    energy::DeviceEnergyModelContainer container = DoInstall(node, energySource);
    return container;
}

energy::DeviceEnergyModelContainer
UavMobilityEnergyModelHelper::Install(Ptr<Node> node, Ptr<energy::EnergySource> source) const
{
    NS_LOG_FUNCTION(this << node << source);
    CheckInstallMobilityModel(node);
    node->AggregateObject(source);

    energy::DeviceEnergyModelContainer container = DoInstall(node, source);
    return container;
}

energy::DeviceEnergyModelContainer
UavMobilityEnergyModelHelper::Install(NodeContainer& nodes) const
{
    NS_LOG_FUNCTION(this);
    energy::DeviceEnergyModelContainer results;
    for (auto node = nodes.Begin(); node != nodes.End(); node++)
    {
        results.Add(Install(*node));
    }

    return results;
}

energy::DeviceEnergyModelContainer
UavMobilityEnergyModelHelper::Install(NodeContainer& nodes,
                                      energy::EnergySourceContainer& sources) const
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_MSG_IF(nodes.GetN() > sources.GetN(),
                    "Number of Nodes greater than number of sources");

    if (sources.GetN() > nodes.GetN())
    {
        NS_LOG_WARN("Number of EnergySource(s) exceeds Nodes. "
                    "Only "
                    << nodes.GetN()
                    << " EnergySource(s) "
                       "will be used.");
    }
    energy::DeviceEnergyModelContainer results;

    auto node = nodes.Begin();
    auto energySource = sources.Begin();
    for (; node != nodes.End(); node++, energySource++)
    {
        results.Add(Install(*node, *energySource));
    }

    return results;
}

void
UavMobilityEnergyModelHelper::SetMobilityModel(std::string name,
                                               std::string n0,
                                               const AttributeValue& v0,
                                               std::string n1,
                                               const AttributeValue& v1,
                                               std::string n2,
                                               const AttributeValue& v2,
                                               std::string n3,
                                               const AttributeValue& v3,
                                               std::string n4,
                                               const AttributeValue& v4,
                                               std::string n5,
                                               const AttributeValue& v5,
                                               std::string n6,
                                               const AttributeValue& v6,
                                               std::string n7,
                                               const AttributeValue& v7)
{
    NS_LOG_FUNCTION(this);
    ObjectFactory factory;
    factory.SetTypeId(name);
    factory.Set(n0, v0);
    factory.Set(n1, v1);
    factory.Set(n2, v2);
    factory.Set(n3, v3);
    factory.Set(n4, v4);
    factory.Set(n5, v5);
    factory.Set(n6, v6);
    factory.Set(n7, v7);

    m_mobilityModel = factory;
}

void
UavMobilityEnergyModelHelper::SetEnergySource(std::string name,
                                              std::string n0,
                                              const AttributeValue& v0,
                                              std::string n1,
                                              const AttributeValue& v1,
                                              std::string n2,
                                              const AttributeValue& v2,
                                              std::string n3,
                                              const AttributeValue& v3,
                                              std::string n4,
                                              const AttributeValue& v4,
                                              std::string n5,
                                              const AttributeValue& v5,
                                              std::string n6,
                                              const AttributeValue& v6,
                                              std::string n7,
                                              const AttributeValue& v7)
{
    NS_LOG_FUNCTION(this);
    ObjectFactory factory;
    factory.SetTypeId(name);
    factory.Set(n0, v0);
    factory.Set(n1, v1);
    factory.Set(n2, v2);
    factory.Set(n3, v3);
    factory.Set(n4, v4);
    factory.Set(n5, v5);
    factory.Set(n6, v6);
    factory.Set(n7, v7);

    m_energySource = factory;
}

Ptr<MobilityModel>
UavMobilityEnergyModelHelper::CheckInstallMobilityModel(Ptr<Node> node) const
{
    NS_LOG_FUNCTION(this << node);
    auto mobility = node->GetObject<MobilityModel>();
    if (!mobility)
    {
        if (m_mobilityModel.GetTypeId().GetName().empty())
        {
            NS_ABORT_MSG("No MobilityModel installed on node, and "
                         "SetMobilityModel not called");
        }
        mobility = m_mobilityModel.Create()->GetObject<MobilityModel>();
        if (!mobility)
        {
            NS_ABORT_MSG("Failed to create MobilityModel "
                         << m_mobilityModel.GetTypeId().GetName());
        }

        node->AggregateObject(mobility);
    }

    return mobility;
}

Ptr<energy::EnergySource>
UavMobilityEnergyModelHelper::CheckInstallEnergySource(Ptr<Node> node) const
{
    NS_LOG_FUNCTION(this << node);
    auto energySource = node->GetObject<energy::EnergySource>();
    if (!energySource)
    {
        if (m_energySource.GetTypeId().GetName().empty())
        {
            NS_ABORT_MSG("No EnergySource installed on node, and "
                         "SetEnergySource not called");
        }

        energySource = m_energySource.Create()->GetObject<energy::EnergySource>();
        if (!energySource)
        {
            NS_ABORT_MSG("Failed to create EnergySource " << m_energySource.GetTypeId().GetName());
        }

        node->AggregateObject(energySource);
    }

    return energySource;
}

Ptr<energy::DeviceEnergyModel>
UavMobilityEnergyModelHelper::DoInstall(Ptr<Node> node, Ptr<energy::EnergySource> source) const
{
    NS_LOG_FUNCTION(this << node << source);
    NS_ABORT_MSG_UNLESS(node, "Cannot install on a NULL node");
    NS_ABORT_MSG_UNLESS(source, "Cannot install with a NULL source");
    source->SetNode(node);
    auto mobility = node->GetObject<MobilityModel>();
    NS_ABORT_MSG_UNLESS(mobility, "node Must have a MobilityModel installed");

    auto createdModel = m_energyModel.Create()->GetObject<UavMobilityEnergyModel>();

    createdModel->Init(node, source);
    node->AggregateObject(createdModel);
    return createdModel;
}

} // namespace psc
} // namespace ns3
