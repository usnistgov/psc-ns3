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
 * 
 * Author: Evan Black <evan.black@nist.gov>
 */


/**
 * \file uav-mobility-energy-model-helper-test.cc
 * \ingroup psc-tests
 * 
 * UavMobilityEnergyModelHelper test suite
 * 
 */

#include <iostream>
#include <cmath>
#include <ns3/core-module.h>
#include <ns3/energy-module.h>
#include <ns3/log.h>
#include <ns3/mobility-module.h>
#include <ns3/node.h>
#include <ns3/psc-module.h>
#include <ns3/ptr.h>
#include <ns3/simulator.h>
#include <ns3/test.h>


/**
 * \ingroup psc-tests
 * \defgroup uav-mobility-energy-model-helper-tests Tests for the UavMobilityEnergyModelHelper
 */

namespace ns3 {

  namespace tests {

using psc::UavMobilityEnergyModel;
using psc::UavMobilityEnergyModelHelper;

/**
 * \ingroup uav-mobility-energy-model-helper-tests
 * Test setting up everything outside the helper,
 * and only use the most basic install
 */
class UavMobilityEnergyModelHelperTestOneNode : public TestCase
{
public:
  UavMobilityEnergyModelHelperTestOneNode (void);
  void DoRun (void) override;
};

UavMobilityEnergyModelHelperTestOneNode::UavMobilityEnergyModelHelperTestOneNode (void)
  : TestCase ("UAV Mobility Energy Model Helper Test Case - Basic Use")
{

}

void
UavMobilityEnergyModelHelperTestOneNode::DoRun (void)
{
  Ptr<Node> node = CreateObject<Node> ();
  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  mobility.Install (node);

  UavMobilityEnergyModelHelper helper;
  // Return Value
  auto modelCollection = helper.Install (node, energySource);
  NS_TEST_ASSERT_MSG_EQ (modelCollection.GetN (), 1, 
                        "Collection should have one element");

  // Aggregation
  auto model = node->GetObject<UavMobilityEnergyModel> ();
  NS_TEST_ASSERT_MSG_NE (model, 0, "Model should not be null");
  auto aggregateSource = node->GetObject<BasicEnergySource> ();
  NS_TEST_ASSERT_MSG_NE (aggregateSource, 0,
                        "Energy Source should not be null");
  Simulator::Destroy ();
}

/**
 * \ingroup uav-mobility-energy-model-helper-tests
 * Test having the helper setup everything
 */
class UavMobilityEnergyModelHelperTestFullSetup : public TestCase
{
public:
  UavMobilityEnergyModelHelperTestFullSetup (void);
  void DoRun (void) override;
};

UavMobilityEnergyModelHelperTestFullSetup::UavMobilityEnergyModelHelperTestFullSetup (void)
  : TestCase ("UAV Mobility Energy Model Helper Test Case - Full Setup")
{

}

void
UavMobilityEnergyModelHelperTestFullSetup::DoRun (void)
{
  Ptr<Node> node = CreateObject<Node> ();

  UavMobilityEnergyModelHelper helper;
  helper.SetEnergySource ("ns3::BasicEnergySource");
  helper.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  auto modelCollection = helper.Install (node);
  NS_TEST_ASSERT_MSG_EQ (modelCollection.GetN (), 1, 
                        "Collection should have one element");



  // Every component should be installed/aggregated
  auto mobilityModel = node->GetObject<ConstantVelocityMobilityModel> ();
  NS_TEST_ASSERT_MSG_NE (mobilityModel, 0,
                        "Mobility Model should not be null");

  auto energySource = node->GetObject<BasicEnergySource> ();
  NS_TEST_ASSERT_MSG_NE (energySource, 0,
                        "Energy Source should not be null");
      
  auto energyModel = node->GetObject<UavMobilityEnergyModel> ();
  NS_TEST_ASSERT_MSG_NE (energyModel, 0,
                        "Energy Model should not be null");
  Simulator::Destroy ();
}


/**
 * \ingroup uav-mobility-energy-model-helper-tests
 * Test collections with the helper
 */
class UavMobilityEnergyModelHelperTestCollections : public TestCase
{
  // # of nodes to create in each test
  const uint32_t NODES = 5;

  void TestFullSetup (void);
  void TestNodeEnergyContainers (void);
public:
  UavMobilityEnergyModelHelperTestCollections (void);
  void DoRun (void) override;
};

UavMobilityEnergyModelHelperTestCollections::UavMobilityEnergyModelHelperTestCollections (void)
  : TestCase ("UAV Mobility Energy Model Helper Test Case - Collections")
{

}

void
UavMobilityEnergyModelHelperTestCollections::TestFullSetup (void)
{
  NodeContainer nodes;
  nodes.Create (NODES);

  UavMobilityEnergyModelHelper helper;
  helper.SetEnergySource ("ns3::BasicEnergySource");
  helper.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  auto modelCollection = helper.Install (nodes);
  NS_TEST_ASSERT_MSG_EQ (modelCollection.GetN (), NODES,
                        "# Models created should match # of Nodes");
  
  for (auto node = nodes.Begin (); node != nodes.End (); node++)
    {
      NS_TEST_ASSERT_MSG_NE ((*node)->GetObject<ConstantVelocityMobilityModel> (),
                            0, "Mobility Model should not be null");

      NS_TEST_ASSERT_MSG_NE ((*node)->GetObject<BasicEnergySource> (),
                            0, "Energy Source should not be null");

      NS_TEST_ASSERT_MSG_NE ((*node)->GetObject<UavMobilityEnergyModel> (), 
                            0, "Energy Model should not be null");
    }
}

void
UavMobilityEnergyModelHelperTestCollections::TestNodeEnergyContainers (void)
{
  const uint32_t NODES = 5;

  NodeContainer nodes;
  nodes.Create (NODES);

  EnergySourceContainer sources;
  for (uint32_t i = 0; i < NODES; i++)
    {
      sources.Add (CreateObject<BasicEnergySource> ());
    }

  UavMobilityEnergyModelHelper helper;
  helper.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  
  auto models = helper.Install (nodes, sources);

  for (auto node = nodes.Begin (); node != nodes.End (); node++)
    {
      NS_TEST_ASSERT_MSG_NE ((*node)->GetObject<ConstantVelocityMobilityModel> (),
                            0, "Mobility Model should not be null");

      NS_TEST_ASSERT_MSG_NE ((*node)->GetObject<BasicEnergySource> (),
                            0, "Energy Source should not be null");

      NS_TEST_ASSERT_MSG_NE ((*node)->GetObject<UavMobilityEnergyModel> (), 
                            0, "Energy Model should not be null");
    }
}

void
UavMobilityEnergyModelHelperTestCollections::DoRun (void)
{
  TestFullSetup ();
  Simulator::Destroy ();

  TestNodeEnergyContainers ();
  Simulator::Destroy ();
}


/**
 * \ingroup uav-mobility-energy-model-helper-tests
 */
class UavMobilityEnergyModelHelperTestSuite : public TestSuite
{
  public:
    UavMobilityEnergyModelHelperTestSuite (void);
};

UavMobilityEnergyModelHelperTestSuite::UavMobilityEnergyModelHelperTestSuite (void)
  : TestSuite ("uav-mobility-energy-model-helper")
{
  AddTestCase (new UavMobilityEnergyModelHelperTestOneNode ());
  AddTestCase (new UavMobilityEnergyModelHelperTestFullSetup ());
  AddTestCase (new UavMobilityEnergyModelHelperTestCollections ());
}


/**
 * \ingroup uav-mobility-energy-model-helper-tests
 */
static UavMobilityEnergyModelHelperTestSuite g_uavMobilityEnergyModelHelperTestSuite;

  } // namespace tests

} // namespace ns3