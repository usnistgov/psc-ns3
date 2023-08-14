/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public service. You may use,
 * copy and distribute copies of the software in any medium, provided that you
 * keep intact this entire notice. You may improve,modify and create derivative
 * works of the software or any portion of the software, and you may copy and
 * distribute such modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and nature of
 * any such change. Please explicitly acknowledge the National Institute of
 * Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
 * AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
 * OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
 * ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF,
 * INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY,
 * OR USEFULNESS OF THE SOFTWARE.
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
 * \file uav-mobility-energy-model-test.cc
 * \ingroup psc-tests
 * 
 * UavMobilityEnergyModel test suite
 */

#include <iostream>
#include <cmath>
#include <ns3/core-module.h>
#include <ns3/mobility-module.h>
#include <ns3/psc-module.h>
#include <ns3/simulator.h>
#include <ns3/test.h>
#include <ns3/energy-module.h>
#include <ns3/log.h>

NS_LOG_COMPONENT_DEFINE ("UavMobilityEnergyModelTestSuite");

/**
 * \ingroup psc-tests
 * \defgroup uav-mobility-energy-model-tests Tests for the UavMobilityEnergyModel
 */

namespace ns3 {

  namespace tests {

using psc::UavMobilityEnergyModel;

      namespace { // Internal Use Items
// Create & Attach mock MobilityHelpers
void
AttachMockMobility (Ptr<Node> &node)
{
  MobilityHelper mobility;

  Ptr<ns3::ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (node);
}

} // Anonymous Namespace

/**
 * \ingroup uav-mobility-energy-model-tests
 * Test basic energy consumption
 */
class UavMobilityEnergyModelTestCaseEnergyConsumption : public TestCase
{
public:
  UavMobilityEnergyModelTestCaseEnergyConsumption (void);
  void DoRun (void) override;
};


UavMobilityEnergyModelTestCaseEnergyConsumption::UavMobilityEnergyModelTestCaseEnergyConsumption (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Energy Consumption")
{

}

void
UavMobilityEnergyModelTestCaseEnergyConsumption::DoRun (void)
{
  const double HOVER_COST = 210; // Set This In Case The Default Changes
  const double INITIAL_ENERGY = 5000; // Should Leave Enough Leeway For 2 secs.
  const double VOLTAGE = 11;

  Ptr<Node> node = CreateObject<Node> ();

  // Be sure to attach the mobility model first
  AttachMockMobility (node);

  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  energySource->SetNode (node);
  energySource->SetSupplyVoltage (VOLTAGE);
  energySource->SetInitialEnergy (INITIAL_ENERGY);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  model->Init (node, energySource);
  node->AggregateObject (model);
  
  model->SetHoverCurrent (HOVER_COST);
  
  // Hover for one second
  Simulator::Schedule (Seconds(1), &UavMobilityEnergyModel::Hover, model);
  Simulator::Schedule (Seconds(2), &UavMobilityEnergyModel::Stop, model);
  
  Simulator::Stop (Seconds (3.0));
  Simulator::Run ();


  NS_TEST_ASSERT_MSG_EQ (energySource->GetRemainingEnergy (),
                        INITIAL_ENERGY - VOLTAGE * HOVER_COST,
                        "Hover cost should be correctly deducted from total");
  Simulator::Destroy ();
}

/**
 * \ingroup uav-mobility-energy-model-tests
 * Test that we can still disable all energy costs
 */
class UavMobilityEnergyModelTestCaseZeroEnergyCost : public TestCase
{
public:
  UavMobilityEnergyModelTestCaseZeroEnergyCost (void);
  void DoRun (void) override;
};

UavMobilityEnergyModelTestCaseZeroEnergyCost::UavMobilityEnergyModelTestCaseZeroEnergyCost (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Zero Energy Consumption")
{

}

void
UavMobilityEnergyModelTestCaseZeroEnergyCost::DoRun (void)
{
  // Make everything free (not realistic, but a supported feature)
  const double ZERO_ENERGY_COST = 0;

  // Give some semantic names to what the simulation will be doing
  // these only serve for clarity since everything costs no energy
  const double MOVEMENT_SPEED = 2;
  const Vector MOVE_VECTOR {MOVEMENT_SPEED, 0, 0};

  Ptr<Node> node = CreateObject<Node> ();

  // Be sure to attach the mobility model first
  AttachMockMobility (node);

  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  energySource->SetNode (node);
  node->AggregateObject (energySource);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  model->Init (node, energySource);
  node->AggregateObject (model);

  model->SetAscendEnergyConversionFactor(ZERO_ENERGY_COST);
  model->SetDescendEnergyConversionFactor(ZERO_ENERGY_COST);
  model->SetMoveEnergyConversionFactor(ZERO_ENERGY_COST);
  model->SetHoverCurrent (ZERO_ENERGY_COST);
  
  // Go through all of the operations in no particular order
  Simulator::Schedule (Seconds (1), &UavMobilityEnergyModel::Ascend,
                      model, MOVEMENT_SPEED);
  Simulator::Schedule (Seconds (2), &UavMobilityEnergyModel::Descend,
                      model, MOVEMENT_SPEED);
  Simulator::Schedule (Seconds (3), &UavMobilityEnergyModel::Move,
                      model, MOVE_VECTOR);
  Simulator::Schedule (Seconds (4), &UavMobilityEnergyModel::Hover, model);
  Simulator::Schedule (Seconds (5), &UavMobilityEnergyModel::Stop, model);
  
  Simulator::Stop (Seconds (6.0));
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (energySource->GetRemainingEnergy (),
                        energySource->GetInitialEnergy (),
                        "No energy should have been lost");

  Simulator::Destroy ();
}


/**
 * \ingroup uav-mobility-energy-model-tests
 * Test move energy calculations
 */
class UavMobilityEnergyModelTestCaseMoveEnergy : public TestCase
{
public:
  UavMobilityEnergyModelTestCaseMoveEnergy (void);
  void DoRun (void) override;
};

UavMobilityEnergyModelTestCaseMoveEnergy::UavMobilityEnergyModelTestCaseMoveEnergy (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Move Energy")
{

}

void
UavMobilityEnergyModelTestCaseMoveEnergy::DoRun (void)
{
  const double MOVE_CONVERSION_FACTOR = 220;
  const double ASCEND_CONVERSION_FACTOR = 250;
  const Vector MOVE_X (1, 0, 0);
  const Vector MOVE_XZ (1, 0, 1);
  const Vector2D MOVE_XZ_2D (MOVE_XZ.x, MOVE_XZ.y);

  Ptr<Node> node = CreateObject<Node> ();

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  mobility.Install (node);
  Ptr <ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();

  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  energySource->SetNode (node);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  model->SetAttribute ("MoveEnergyConversionFactor",
                      DoubleValue (MOVE_CONVERSION_FACTOR));
  model->SetAttribute ("AscendEnergyConversionFactor",
                      DoubleValue (ASCEND_CONVERSION_FACTOR));
  model->Init (node, energySource);
  node->AggregateObject (model);


  const double MOVE_X_AMPERAGE = MOVE_CONVERSION_FACTOR * MOVE_X.GetLength ();
  const double MOVE_XZ_AMPERAGE = 
    MOVE_CONVERSION_FACTOR * MOVE_XZ_2D.GetLength () + 
    ASCEND_CONVERSION_FACTOR * MOVE_XZ.z;

  mobilityModel->SetVelocity (MOVE_X);
  bool correctA = TestDoubleIsEqual (model->GetCurrentA (), MOVE_X_AMPERAGE);
  NS_TEST_ASSERT_MSG_EQ (correctA, true, "Model has incorrect current");

  mobilityModel->SetVelocity (MOVE_XZ);
  correctA = TestDoubleIsEqual (model->GetCurrentA (), MOVE_XZ_AMPERAGE);
  NS_TEST_ASSERT_MSG_EQ (correctA, true, "Model has incorrect current");

  Simulator::Destroy ();
}


/**
 * \ingroup uav-mobility-energy-model-tests
 * Test if the low energy callback works
 */
class UavMobilityEnergyModelTestCaseLowEnergyTrace : public TestCase
{
  int m_lowEnergyCount = 0;
  int m_highEnergyCount = 0;
public:
  UavMobilityEnergyModelTestCaseLowEnergyTrace (void);
  void DoRun (void) override;
  void LowEnergy(Ptr<const UavMobilityEnergyModel> model);
  void HighEnergy(Ptr<const UavMobilityEnergyModel> model);

};

UavMobilityEnergyModelTestCaseLowEnergyTrace::UavMobilityEnergyModelTestCaseLowEnergyTrace (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Low Energy Callback")
{

}

void
UavMobilityEnergyModelTestCaseLowEnergyTrace::DoRun (void)
{
  // Make sure defaults are sane
  const double HOVER_COST = 210;
  const double INITIAL_ENERGY = 10000;
  const double VOLTAGE = 11;

  // Make sure we don't consume more then the possible energy
  const double MAX_RUN_TIME = 
                          std::floor (INITIAL_ENERGY / (VOLTAGE * HOVER_COST));
  
  // Make sure the test itself makes sense
  NS_TEST_ASSERT_MSG_GT (MAX_RUN_TIME, 1,
                        "Make sure we run for more than one second");

  Ptr<Node> node = CreateObject<Node> ();

  // Be sure to attach the mobility model first
  AttachMockMobility (node);

  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  node->AggregateObject (energySource);
  energySource->SetNode (node);
  energySource->SetSupplyVoltage (VOLTAGE);
  energySource->SetInitialEnergy (INITIAL_ENERGY);

  // Set the Low Energy Threshold very high, so we're sure to hit it.
  energySource->SetAttribute ("BasicEnergyLowBatteryThreshold",
                              DoubleValue (.5));
  
  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  model->Init (node, energySource);
  node->AggregateObject (model);

  model->TraceConnectWithoutContext ("EnergyDepleted",
                                    MakeCallback (&UavMobilityEnergyModelTestCaseLowEnergyTrace::LowEnergy,
                                                  this));

  // We should not have a recharged event
  model->TraceConnectWithoutContext ("EnergyRecharged",
                                    MakeCallback (&UavMobilityEnergyModelTestCaseLowEnergyTrace::HighEnergy,
                                                  this));

  Simulator::Schedule (Seconds (1), &UavMobilityEnergyModel::Hover, model);
  // Be sure we stop before we go over the energy source's capacity
  Simulator::Schedule (Seconds (MAX_RUN_TIME),
                      &UavMobilityEnergyModel::Stop, model);
  Simulator::Stop (Seconds (MAX_RUN_TIME));
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (m_lowEnergyCount, 1,
                        "Low Energy callback should be called once");
  NS_TEST_ASSERT_MSG_EQ (m_highEnergyCount, 0,
                        "Recharge event should not occur");
  
  Simulator::Destroy ();
}

void
UavMobilityEnergyModelTestCaseLowEnergyTrace::LowEnergy (Ptr<const UavMobilityEnergyModel> model)
{
  m_lowEnergyCount++;
}

void
UavMobilityEnergyModelTestCaseLowEnergyTrace::HighEnergy (Ptr<const UavMobilityEnergyModel> model)
{
  m_highEnergyCount++;
}


/**
 * \ingroup uav-mobility-energy-model-tests
 * Test the energy model with a different type of energy source
 * 
 * This application is almost certainly not what this battery is for
 * This should only demonstrait that models other than BasicEnergySource
 * will work
 */
class UavMobilityEnergyModelTestCaseLiIonEnergySource : public TestCase
{
public:
  UavMobilityEnergyModelTestCaseLiIonEnergySource (void);
  void DoRun (void) override;
};

UavMobilityEnergyModelTestCaseLiIonEnergySource::UavMobilityEnergyModelTestCaseLiIonEnergySource (void)
  : TestCase ("UAV Mobility Energy Model Test Case - LiIonEnergySource")
{

}

void UavMobilityEnergyModelTestCaseLiIonEnergySource::DoRun (void)
{
  Ptr<Node> node = CreateObject<Node> ();

  // Be sure to attach the mobility model first
  AttachMockMobility (node);

  Ptr<LiIonEnergySource> liIon = CreateObject<LiIonEnergySource> ();
  node->AggregateObject (liIon);
  liIon->SetNode (node);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  model->Init (node, liIon);
  node->AggregateObject (model);
    
  // Hover for one second
  Simulator::Schedule (Seconds(1), &UavMobilityEnergyModel::Hover, model);
  Simulator::Schedule (Seconds(2), &UavMobilityEnergyModel::Stop, model);

  Simulator::Run ();

  // Test that energy is reduced with the fewest assumptions about the
  // Energy Source
  NS_TEST_ASSERT_MSG_LT (liIon->GetRemainingEnergy(), liIon->GetInitialEnergy(),
                        "Energy in the cell should have been reduced");
  
  Simulator::Destroy ();
}

/**
 * \ingroup uav-mobility-energy-model-tests
 * Tests the model's integration with a MobilityModel
 */
class UavMobilityEnergyModelTestCaseMobilityModel : public TestCase
{
public:
  UavMobilityEnergyModelTestCaseMobilityModel (void);
  void DoRun (void) override;
};

UavMobilityEnergyModelTestCaseMobilityModel::UavMobilityEnergyModelTestCaseMobilityModel (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Mobility Model")
{

}

void
UavMobilityEnergyModelTestCaseMobilityModel::DoRun (void)
{
  const double DURATION = 3;
  const Vector STOPPED_VELOCITY (0, 0, 0);
  const Vector MOVE_X_VELOCITY (1, 0, 0);
  const double INITIAL_ENERGY = 10000;
  const double VOLTAGE = 11;
  const double MOVE_COST = 220;

  // Make sure we don't consume more then the possible energy
  const double MAX_RUN_TIME = 
                          std::floor (INITIAL_ENERGY / (VOLTAGE * MOVE_COST));

  // Make sure the test itself makes sense
  NS_TEST_ASSERT_MSG_GT (MAX_RUN_TIME, 1,
                        "Test must run for at least one second");

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator");

  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  Ptr<Node> node = CreateObject<Node> ();

  mobility.Install (node);
  Ptr <ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();
  
  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  node->AggregateObject (energySource);
  energySource->SetInitialEnergy (INITIAL_ENERGY);
  energySource->SetSupplyVoltage (VOLTAGE);
  energySource->SetNode (node);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  node->AggregateObject (model);
  model->Init (node, energySource);
  
  model->SetMoveEnergyConversionFactor (MOVE_COST);

  Simulator::Schedule (Seconds (0),
                      &ConstantVelocityMobilityModel::SetVelocity,
                      mobilityModel, MOVE_X_VELOCITY);
  Simulator::Schedule (Seconds (DURATION),
                      &ConstantVelocityMobilityModel::SetVelocity,
                      mobilityModel, STOPPED_VELOCITY);
                    
  Simulator::Stop (Seconds (DURATION + 1.0));
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (energySource->GetRemainingEnergy (),
                        INITIAL_ENERGY - VOLTAGE * MOVE_COST * DURATION,
                        "Energy model should determine that we are moving");

  Simulator::Destroy ();
}


/**
 * \ingroup uav-mobility-energy-model-tests
 * Tests changing the model's energy costs after it has been created
 */
class UavMobilityEnergyModelTestCaseChangeCostsRuntime : public TestCase
{
public:
  UavMobilityEnergyModelTestCaseChangeCostsRuntime (void);
  void DoRun (void) override;
};

UavMobilityEnergyModelTestCaseChangeCostsRuntime::UavMobilityEnergyModelTestCaseChangeCostsRuntime (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Change Costs At Runtime")
{

}

void
UavMobilityEnergyModelTestCaseChangeCostsRuntime::DoRun (void)
{
  const double DURATION = 2;
  const Vector STOPPED_VELOCITY (0, 0, 0);
  const Vector MOVE_X_VELOCITY (1, 0, 0);
  const double INITIAL_ENERGY = 10000;
  const double VOLTAGE = 11;
  const double MOVE_COST_INITIAL = 220;
  const double MOVE_COST_FINAL = 110;

  // Make sure we don't consume more then the possible energy
  const double MAX_RUN_TIME =
                          std::floor (INITIAL_ENERGY / 
                                      (VOLTAGE * MOVE_COST_INITIAL));

  // Make sure the test itself makes sense
  NS_TEST_ASSERT_MSG_GT (MAX_RUN_TIME, DURATION,
                        "Test must run for at least the duration");

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  Ptr<Node> node = CreateObject<Node> ();

  mobility.Install (node);
  Ptr <ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();
  
  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  node->AggregateObject (energySource);
  energySource->SetInitialEnergy (INITIAL_ENERGY);
  energySource->SetSupplyVoltage (VOLTAGE);
  energySource->SetNode (node);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  model->Init (node, energySource);
  node->AggregateObject (model);
  
  model->SetAttribute ("AscendEnergyConversionFactor",
                      DoubleValue (MOVE_COST_INITIAL));
  

  Simulator::Schedule (Seconds (0),
                      &ConstantVelocityMobilityModel::SetVelocity,
                      mobilityModel, MOVE_X_VELOCITY);
  Simulator::Schedule (Seconds (1),
                      &UavMobilityEnergyModel::SetMoveEnergyConversionFactor,
                      model, MOVE_COST_FINAL);

  Simulator::Schedule (Seconds (DURATION),
                      &ConstantVelocityMobilityModel::SetVelocity,
                      mobilityModel, STOPPED_VELOCITY);
                    
  Simulator::Stop (Seconds (DURATION + 1));
  Simulator::Run ();

  // Both values are held for one second, so duration is not included
  const double INITIAL_COST_CONSUMPTION = VOLTAGE * MOVE_COST_INITIAL;
  const double FINAL_COST_CONSUMPTION = VOLTAGE * MOVE_COST_FINAL;

  NS_TEST_ASSERT_MSG_EQ (energySource->GetRemainingEnergy (),
                        INITIAL_ENERGY -
                          (INITIAL_COST_CONSUMPTION + FINAL_COST_CONSUMPTION),
                        "Energy model should detect cost change");
  Simulator::Destroy ();
}


/**
 * \ingroup uav-mobility-energy-model-tests
 * Tests if the measured energy consumed is correct
 */
class UavMobilityEnergyModelTestCaseTotalEnergyConsumption : public TestCase
{
  double m_tracedEnergy = 0;
public:
  UavMobilityEnergyModelTestCaseTotalEnergyConsumption (void);
  void DoRun (void) override;
  void TraceEnergy (double oldValue, double newValue);
};

UavMobilityEnergyModelTestCaseTotalEnergyConsumption::UavMobilityEnergyModelTestCaseTotalEnergyConsumption (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Total Energy Consumption")
{

}

void UavMobilityEnergyModelTestCaseTotalEnergyConsumption::DoRun (void)
{
  const double DURATION = 3;
  const Vector STOPPED_VELOCITY (0, 0, 0);
  const Vector MOVE_X_VELOCITY (1, 0, 0);
  const double INITIAL_ENERGY = 10000;
  const double VOLTAGE = 11;
  const double MOVE_COST = 220;

  // Make sure we don't consume more then the possible energy
  const double MAX_RUN_TIME = 
                          std::floor (INITIAL_ENERGY / (VOLTAGE * MOVE_COST));

  // Make sure the test itself makes sense
  NS_TEST_ASSERT_MSG_GT (MAX_RUN_TIME, DURATION,
                        "Test must run for the complete duration");

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  Ptr<Node> node = CreateObject<Node> ();

  mobility.Install (node);
  Ptr <ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();
  
  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  node->AggregateObject (energySource);
  energySource->SetInitialEnergy (INITIAL_ENERGY);
  energySource->SetSupplyVoltage (VOLTAGE);
  energySource->SetNode (node);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  node->AggregateObject (model);
  model->TraceConnectWithoutContext ("TotalEnergyConsumption",
                                    MakeCallback (&UavMobilityEnergyModelTestCaseTotalEnergyConsumption::TraceEnergy,
                                                  this));
  model->Init (node, energySource);
  
  model->SetMoveEnergyConversionFactor (MOVE_COST);

  Simulator::Schedule (Seconds (0),
                      &ConstantVelocityMobilityModel::SetVelocity,
                      mobilityModel, MOVE_X_VELOCITY);
  Simulator::Schedule (Seconds (DURATION),
                      &ConstantVelocityMobilityModel::SetVelocity,
                      mobilityModel, STOPPED_VELOCITY);
                    
  Simulator::Stop (Seconds (DURATION + 1));
  Simulator::Run ();
  

  const double TOTAL_ENERGY_COST = VOLTAGE * MOVE_COST * DURATION;
  NS_TEST_ASSERT_MSG_EQ (energySource->GetRemainingEnergy (),
                        INITIAL_ENERGY - TOTAL_ENERGY_COST,
                        "Energy cost from the model should be correct");

  NS_TEST_ASSERT_MSG_EQ (model->GetTotalEnergyConsumption (),
                        TOTAL_ENERGY_COST,
                        "Measured energy cost should match actual");

  NS_TEST_ASSERT_MSG_EQ (m_tracedEnergy,
                        TOTAL_ENERGY_COST,
                        "Traced energy cost should match actual");

  Simulator::Destroy ();
}

void
UavMobilityEnergyModelTestCaseTotalEnergyConsumption::TraceEnergy (double oldValue, double newValue)
{
  m_tracedEnergy = newValue;
}


/**
 * \ingroup uav-mobility-energy-model-tests
 * Tests if a velocity set before the simulation starts functions correctly
 */
class UavMobilityEnergyModelTestCaseInitialVelocity : public TestCase
{
public:
  UavMobilityEnergyModelTestCaseInitialVelocity (void);
  void DoRun (void) override;
};

UavMobilityEnergyModelTestCaseInitialVelocity::UavMobilityEnergyModelTestCaseInitialVelocity (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Total Energy Consumption")
{

}

void UavMobilityEnergyModelTestCaseInitialVelocity::DoRun (void)
{
  const double DURATION = 3;
  const Vector MOVE_X_VELOCITY (1, 0, 0);
  const double INITIAL_ENERGY = 10000;
  const double VOLTAGE = 11;
  const double MOVE_COST = 220;

  // Make sure we don't consume more then the possible energy
  const double MAX_RUN_TIME =
                          std::floor (INITIAL_ENERGY / (VOLTAGE * MOVE_COST));

  // Make sure the test itself makes sense
  NS_TEST_ASSERT_MSG_GT (MAX_RUN_TIME, DURATION,
                        "Test must run for the complete duration");

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  Ptr<Node> node = CreateObject<Node> ();

  mobility.Install (node);
  Ptr <ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();
  
  mobilityModel->SetVelocity (MOVE_X_VELOCITY);
  
  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  node->AggregateObject (energySource);
  energySource->SetInitialEnergy (INITIAL_ENERGY);
  energySource->SetSupplyVoltage (VOLTAGE);
  energySource->SetNode (node);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  model->Init (node, energySource);
  node->AggregateObject (model);
  
  model->SetMoveEnergyConversionFactor (MOVE_COST);


  // The energy source will not update once the simulation stops
  //  so, we only consume energy until DURATION - 1 Seconds
  Simulator::Stop (Seconds (DURATION));                    
  Simulator::Run ();
 
  NS_TEST_ASSERT_MSG_EQ (energySource->GetRemainingEnergy (),
                        INITIAL_ENERGY - VOLTAGE * MOVE_COST * DURATION,
                        "Energy cost from the model should be correct");
  Simulator::Destroy ();
}

/**
 * \ingroup uav-mobility-energy-model-tests
 * Walk through each possible state to see if they are set correctly
 */
class UavMobilityEnergyModelTestCaseState : public TestCase
{
  UavMobilityEnergyModel::State m_state = UavMobilityEnergyModel::State::STOP;

  /**
   * Validate against the state determind by the model & our traced value
   */
  void ValidateState(Ptr<UavMobilityEnergyModel> energyModel, UavMobilityEnergyModel::State expected);
public:
  UavMobilityEnergyModelTestCaseState (void);
  void DoRun (void) override;
  void TraceState(UavMobilityEnergyModel::State oldState, UavMobilityEnergyModel::State newState, Vector velocity);
};

UavMobilityEnergyModelTestCaseState::UavMobilityEnergyModelTestCaseState (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Get State")
{

}

void UavMobilityEnergyModelTestCaseState::DoRun (void)
{
  using State = UavMobilityEnergyModel::State;

  const Vector MOVE_X_VELOCITY (1, 0, 0);
  const Vector MOVE_Y_VELOCITY (0, 1, 0);
  const Vector MOVE_ASCEND_Z_VELOCITY (0, 0, 1);
  const Vector MOVE_DESCEND_Z_VELOCITY (0, 0, -1);
  const Vector STOPPED_VELOCITY (0, 0, 0);


  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  Ptr<Node> node = CreateObject<Node> ();

  mobility.Install (node);
  Ptr <ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();

  // No energy should be consumed during this test,
  // so we don't worry about the voltage/energy level
  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  node->AggregateObject (energySource);
  energySource->SetNode (node);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  node->AggregateObject (model);
  model->TraceConnectWithoutContext ("State", MakeCallback (&UavMobilityEnergyModelTestCaseState::TraceState,
                                                           this));
  model->Init (node, energySource);

  mobilityModel->SetVelocity (STOPPED_VELOCITY);
  ValidateState (model, State::STOP);

  mobilityModel->SetVelocity (MOVE_X_VELOCITY);
  ValidateState (model, State::MOVE);

  mobilityModel->SetVelocity (MOVE_ASCEND_Z_VELOCITY);
  ValidateState (model, State::ASCEND);

  mobilityModel->SetVelocity (MOVE_DESCEND_Z_VELOCITY);
  ValidateState (model, State::DESCEND);
  
  mobilityModel->SetVelocity (MOVE_Y_VELOCITY);
  ValidateState (model, State::MOVE);

  // Add a +Z position for hover
  // Setting a position on a ConstantVelocityMobilityModel
  // Zeros the velocity, but set this explicitly in case that's
  // not an intended feature
  mobilityModel->SetVelocity (STOPPED_VELOCITY);
  mobilityModel->SetPosition ({0, 0, 1});
  ValidateState (model, State::HOVER);

  // Stop Hovering
  mobilityModel->SetPosition ({0, 0, 0});
  ValidateState (model, State::STOP);

  Simulator::Destroy ();
}

void
UavMobilityEnergyModelTestCaseState::TraceState (UavMobilityEnergyModel::State oldState, UavMobilityEnergyModel::State newState, Vector velocity)
{
  m_state = newState;
}

void
UavMobilityEnergyModelTestCaseState::ValidateState (Ptr<UavMobilityEnergyModel> energyModel, UavMobilityEnergyModel::State expected)
{
  NS_TEST_ASSERT_MSG_EQ (energyModel->GetState (), expected,
                        "Model state does not match expected state");
  
  NS_TEST_ASSERT_MSG_EQ (m_state, expected,
                        "Traced state does not match expected state");
}


/**
 * \ingroup uav-mobility-energy-model-tests
 * Walk through each possible state to see if the current is set properly
 */
class UavMobilityEnergyModelTestCaseTraceCurrent : public TestCase
{
  double m_current = 0;
public:
  UavMobilityEnergyModelTestCaseTraceCurrent (void);
  void DoRun (void) override;
  void TraceCurrent(double oldCurrent, double newCurrent, Vector velocity);
};

UavMobilityEnergyModelTestCaseTraceCurrent::UavMobilityEnergyModelTestCaseTraceCurrent (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Trace Current")
{

}

void UavMobilityEnergyModelTestCaseTraceCurrent::DoRun (void)
{
  // Keep velocities at 1 so, the current is wholly
  // determined by the cost
  const Vector MOVE_X_VELOCITY (1, 0, 0);
  const Vector MOVE_Y_VELOCITY (0, 1, 0);
  const Vector MOVE_ASCEND_Z_VELOCITY (0, 0, 1);
  const Vector MOVE_DESCEND_Z_VELOCITY (0, 0, -1);
  const Vector STOPPED_VELOCITY (0, 0, 0);

  const double ASCEND_COST = 2;
  const double DESCEND_COST = 3;
  const double MOVE_COST = 4;
  const double HOVER_COST = 5;


  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  Ptr<Node> node = CreateObject<Node> ();

  mobility.Install (node);
  Ptr <ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();

  // No energy should be consumed during this test,
  // so we don't worry about the voltage/energy level
  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  node->AggregateObject (energySource);
  energySource->SetNode (node);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  node->AggregateObject (model);

  model->SetAttribute("AscendEnergyConversionFactor", DoubleValue (ASCEND_COST));
  model->SetAttribute("DescendEnergyConversionFactor", DoubleValue (DESCEND_COST));
  model->SetAttribute("MoveEnergyConversionFactor", DoubleValue (MOVE_COST));
  model->SetAttribute("HoverCurrent", DoubleValue (HOVER_COST));

  model->TraceConnectWithoutContext ("Current", MakeCallback (&UavMobilityEnergyModelTestCaseTraceCurrent::TraceCurrent,
                                                             this));
  model->Init (node, energySource);

  mobilityModel->SetVelocity (STOPPED_VELOCITY);
  NS_TEST_ASSERT_MSG_EQ (m_current, 0, 
                        "Traced current is incorrect for stop");

  mobilityModel->SetVelocity (MOVE_X_VELOCITY);
  NS_TEST_ASSERT_MSG_EQ (m_current, MOVE_COST,
                        "Traced current is incorrect for move X");

  mobilityModel->SetVelocity (MOVE_ASCEND_Z_VELOCITY);
  NS_TEST_ASSERT_MSG_EQ (m_current, ASCEND_COST,
                        "Traced current is incorrect for ascend");

  mobilityModel->SetVelocity (MOVE_DESCEND_Z_VELOCITY);
  NS_TEST_ASSERT_MSG_EQ (m_current, DESCEND_COST,
                        "Traced current is incorrect for descend");
  
  mobilityModel->SetVelocity (MOVE_Y_VELOCITY);
  NS_TEST_ASSERT_MSG_EQ (m_current, MOVE_COST,
                        "Traced current is incorrect for move Y");

  // Add a +Z position for hover
  // Setting a position on a ConstantVelocityMobilityModel
  // Zeros the velocity, but set this explicitly in case that's
  // not an intended feature
  mobilityModel->SetVelocity (STOPPED_VELOCITY);
  mobilityModel->SetPosition ({0, 0, 1});
  NS_TEST_ASSERT_MSG_EQ (m_current, HOVER_COST,
                        "Traced current is incorrect for hover");

  // Stop Hovering
  mobilityModel->SetPosition ({0, 0, 0});
  NS_TEST_ASSERT_MSG_EQ (m_current, 0,
                          "Traced current is incorrect for stop after hover");

  Simulator::Destroy ();
}

void
UavMobilityEnergyModelTestCaseTraceCurrent::TraceCurrent (double oldCurrent, double newCurrent, Vector velocity)
{
  m_current = newCurrent;
}


/**
 * \ingroup uav-mobility-energy-model-tests
 * Test setting the EnergySource through an attribute
 */
class UavMobilityEnergyModelTestCaseAttributeEnergySource : public TestCase
{
public:
  UavMobilityEnergyModelTestCaseAttributeEnergySource (void);
  void DoRun (void) override;
};

UavMobilityEnergyModelTestCaseAttributeEnergySource::UavMobilityEnergyModelTestCaseAttributeEnergySource (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Attribute Energy Source")
{

}

void UavMobilityEnergyModelTestCaseAttributeEnergySource::DoRun (void)
{
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  Ptr<Node> node = CreateObject<Node> ();

  mobility.Install (node);
  Ptr <ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();
  
  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  node->AggregateObject (energySource);
  energySource->SetNode (node);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  node->AggregateObject (model);

  // We don't need to completely init to test the attribute

  model->SetAttribute ("EnergySource", PointerValue (energySource));

  NS_TEST_ASSERT_MSG_NE (model->GetEnergySource (), 0,
                         "Energy source should be set");

  PointerValue attributeValue;
  model->GetAttribute ("EnergySource", attributeValue);
  Ptr<EnergySource> retrievedEnergySource = attributeValue.Get<EnergySource> ();
  NS_TEST_ASSERT_MSG_NE (retrievedEnergySource, 0, 
                        "Retrieved Energy Source should not be null");
  Simulator::Destroy ();
}

/**
 * \ingroup uav-mobility-energy-model-tests
 * Test if we maintain state, but change speed,
 * (i.e. Ascend at 1 m/s, then Ascend at 2 m/s)
 * We should have the current updated, but the speed not updated
 */
class UavMobilityEnergyModelTestCaseFixedStateChangeSpeed : public TestCase
{
  int m_stateChangedCount = 0;
  int m_currentChangeCount = 0;
public:
  UavMobilityEnergyModelTestCaseFixedStateChangeSpeed (void);
  void DoRun (void) override;
  void StateTrace (UavMobilityEnergyModel::State oldState, UavMobilityEnergyModel::State newState, Vector velocity);
  void CurrentTrace (double oldCurrent, double newCurrent, Vector velocity);
};

UavMobilityEnergyModelTestCaseFixedStateChangeSpeed::UavMobilityEnergyModelTestCaseFixedStateChangeSpeed (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Fixed State, Change Speed")
{

}

void
UavMobilityEnergyModelTestCaseFixedStateChangeSpeed::DoRun (void)
{
  const Vector ASCEND_SLOW (0, 0, 1);
  const Vector ASCEND_FAST (0, 0, 2);
  const double ASCEND_CONVERSION_FACTOR = 250;

  Ptr<Node> node = CreateObject<Node> ();

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  mobility.Install (node);
  Ptr <ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();
  
  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  energySource->SetNode (node);
  node->AggregateObject (energySource);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  node->AggregateObject (model);
  model->Init (node, energySource);
  model->SetAttribute ("AscendEnergyConversionFactor",
                      DoubleValue (ASCEND_CONVERSION_FACTOR));
  model->TraceConnectWithoutContext ("State", MakeCallback (&UavMobilityEnergyModelTestCaseFixedStateChangeSpeed::StateTrace,
                                                            this));
  model->TraceConnectWithoutContext ("Current", MakeCallback (&UavMobilityEnergyModelTestCaseFixedStateChangeSpeed::CurrentTrace,
                                                            this));
  mobilityModel->SetVelocity (ASCEND_SLOW);
  mobilityModel->SetVelocity (ASCEND_FAST);

  NS_TEST_ASSERT_MSG_EQ (m_stateChangedCount, 1, "State should change once");
  NS_TEST_ASSERT_MSG_EQ (m_currentChangeCount, 2, "Current should update twice");
  
  const double ASCEND_SLOW_AMPERAGE = ASCEND_SLOW.z * ASCEND_CONVERSION_FACTOR;
  bool matchesSlowA = TestDoubleIsEqual (model->GetCurrentA (),
                                        ASCEND_SLOW_AMPERAGE);
  NS_TEST_ASSERT_MSG_EQ (matchesSlowA, false,
                        "Should not match the slow speeds amperage");
  
  const double ASCEND_FAST_AMPERAGE = ASCEND_FAST.z * ASCEND_CONVERSION_FACTOR;
  bool matchesFastA = TestDoubleIsEqual (model->GetCurrentA (),
                                        ASCEND_FAST_AMPERAGE);
  NS_TEST_ASSERT_MSG_EQ (matchesFastA, true,
                        "Should match the fast speeds amperage");
  
  Simulator::Destroy ();
}

void
UavMobilityEnergyModelTestCaseFixedStateChangeSpeed::StateTrace (UavMobilityEnergyModel::State oldState, UavMobilityEnergyModel::State newState, Vector velocity)
{
  m_stateChangedCount++;
}

void
UavMobilityEnergyModelTestCaseFixedStateChangeSpeed::CurrentTrace (double oldCurrent, double newCurrent, Vector velocity)
{
  m_currentChangeCount++;
}

/**
 * \ingroup uav-mobility-energy-model-tests
 * Test if the velocity value passed with other traces is correct
 */
class UavMobilityEnergyModelTestCaseVelocityTraces : public TestCase
{
  Vector m_currentVelocity;
  Vector m_stateVelocity;
  inline bool CompareVectors (const Vector &lhs, const Vector &rhs);
public:
  UavMobilityEnergyModelTestCaseVelocityTraces (void);
  void DoRun (void) override;
  void StateTrace (UavMobilityEnergyModel::State oldState, UavMobilityEnergyModel::State newState, Vector velocity);
  void CurrentTrace (double oldCurrent, double newCurrent, Vector velocity);
};

inline bool
UavMobilityEnergyModelTestCaseVelocityTraces::CompareVectors (const Vector &lhs, const Vector &rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z; 
}

UavMobilityEnergyModelTestCaseVelocityTraces::UavMobilityEnergyModelTestCaseVelocityTraces (void)
  : TestCase ("UAV Mobility Energy Model Test Case - Velocity Traces")
{

}

void
UavMobilityEnergyModelTestCaseVelocityTraces::DoRun (void)
{
  // Make sure we use state where both current and state change
  const Vector MOVE (1, 0, 0);
  const Vector ASCEND (0, 0, 1);
  const Vector DESCEND (0, 0, -1);
  const Vector STOP (0, 0, 0);

  Ptr<Node> node = CreateObject<Node> ();

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  mobility.Install (node);
  Ptr <ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();
  
  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  energySource->SetNode (node);
  node->AggregateObject (energySource);

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  node->AggregateObject (model);
  model->Init (node, energySource);
  
  model->TraceConnectWithoutContext ("State", MakeCallback (&UavMobilityEnergyModelTestCaseVelocityTraces::StateTrace,
                                                            this));
  model->TraceConnectWithoutContext ("Current", MakeCallback (&UavMobilityEnergyModelTestCaseVelocityTraces::CurrentTrace,
                                                              this));

  // ----- MOVE -----
  mobilityModel->SetVelocity (MOVE);
  bool isStateVelocityCorrect = CompareVectors (MOVE, m_stateVelocity);
  NS_TEST_ASSERT_MSG_EQ (isStateVelocityCorrect, true,
                        "State velocity should match actual velocity");

  bool isCurrentVelocityCorrect = CompareVectors (MOVE, m_currentVelocity);
  NS_TEST_ASSERT_MSG_EQ (isCurrentVelocityCorrect, true,
                        "Current velocity should match actual velocity");
  
  // ----- ASCEND -----
  mobilityModel->SetVelocity (ASCEND);
  isStateVelocityCorrect = CompareVectors (ASCEND, m_stateVelocity);
  NS_TEST_ASSERT_MSG_EQ (isStateVelocityCorrect, true,
                        "State velocity should match actual velocity");

  isCurrentVelocityCorrect = CompareVectors (ASCEND, m_currentVelocity);
  NS_TEST_ASSERT_MSG_EQ (isCurrentVelocityCorrect, true,
                        "Current velocity should match actual velocity");
  
  // ----- DESCEND -----
  mobilityModel->SetVelocity (DESCEND);
  isStateVelocityCorrect = CompareVectors (DESCEND, m_stateVelocity);
  NS_TEST_ASSERT_MSG_EQ (isStateVelocityCorrect, true,
                        "State velocity should match actual velocity");

  isCurrentVelocityCorrect = CompareVectors (DESCEND, m_currentVelocity);
  NS_TEST_ASSERT_MSG_EQ (isCurrentVelocityCorrect, true,
                        "Current velocity should match actual velocity");
  
  // ----- STOP -----
  mobilityModel->SetVelocity (STOP);
  isStateVelocityCorrect = CompareVectors (STOP, m_stateVelocity);
  NS_TEST_ASSERT_MSG_EQ (isStateVelocityCorrect, true,
                        "State velocity should match actual velocity");

  isCurrentVelocityCorrect = CompareVectors (STOP, m_currentVelocity);
  NS_TEST_ASSERT_MSG_EQ (isCurrentVelocityCorrect, true,
                        "Current velocity should match actual velocity");

  Simulator::Destroy ();
}

void
UavMobilityEnergyModelTestCaseVelocityTraces::StateTrace (UavMobilityEnergyModel::State oldState, UavMobilityEnergyModel::State newState, Vector velocity)
{
  m_stateVelocity = velocity;
}

void
UavMobilityEnergyModelTestCaseVelocityTraces::CurrentTrace (double oldCurrent, double newCurrent, Vector velocity)
{
  m_currentVelocity = velocity;
}

/**
 * \ingroup uav-mobility-energy-model-tests
 */
class UavMobilityEnergyModelTestSuite : public TestSuite
{
public:
  UavMobilityEnergyModelTestSuite();
};

UavMobilityEnergyModelTestSuite::UavMobilityEnergyModelTestSuite (void)
  : TestSuite ("uav-mobility-energy-model")
{
  AddTestCase (new UavMobilityEnergyModelTestCaseEnergyConsumption ());
  AddTestCase (new UavMobilityEnergyModelTestCaseZeroEnergyCost ());
  AddTestCase (new UavMobilityEnergyModelTestCaseMoveEnergy ());
  AddTestCase (new UavMobilityEnergyModelTestCaseLowEnergyTrace ());
  AddTestCase (new UavMobilityEnergyModelTestCaseLiIonEnergySource ());
  AddTestCase (new UavMobilityEnergyModelTestCaseMobilityModel ());
  AddTestCase (new UavMobilityEnergyModelTestCaseChangeCostsRuntime ());
  AddTestCase (new UavMobilityEnergyModelTestCaseTotalEnergyConsumption ());
  AddTestCase (new UavMobilityEnergyModelTestCaseInitialVelocity ());
  AddTestCase (new UavMobilityEnergyModelTestCaseState ());
  AddTestCase (new UavMobilityEnergyModelTestCaseTraceCurrent ());
  AddTestCase (new UavMobilityEnergyModelTestCaseAttributeEnergySource ());
  AddTestCase (new UavMobilityEnergyModelTestCaseFixedStateChangeSpeed ());
  AddTestCase (new UavMobilityEnergyModelTestCaseVelocityTraces ());
}

/**
 * \ingroup uav-mobility-energy-model-tests
 */
static UavMobilityEnergyModelTestSuite g_uavMobilityEnergyModelTestSuite;

  } // namespace tests

} // namespace ns3
