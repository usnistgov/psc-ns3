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

#include <iostream>
#include <ns3/core-module.h>
#include <ns3/energy-module.h>
#include <ns3/mobility-module.h>
#include <ns3/psc-module.h>

// Sample Scenario showing the setup of the UavMobilityEnergyModel
// and manual movement of a Node (here representing
// a UAV[Unmanned Aerial Vehicle]).
//
// Walks thought movement on the X axis, stopped
// ascending, and descending. The simulation will then exit naturally
// after 11 seconds have passed.

using namespace ns3;
using namespace psc;

NS_LOG_COMPONENT_DEFINE ("UavEnergyMobility");


void
RemainingEnergy (double oldValue, double remainingEnergy)
{
  std::cout << Simulator::Now ().GetSeconds ()
            << "s Current remaining energy = " << remainingEnergy << "J\n";
}

// This Callback is unused with the default configuration of this example
// and is intended to demonstrate how Callback(s) are connected, and an
// expected implementation of the EnergyDepleted Callback
void
EnergyDepleted (Ptr<ConstantVelocityMobilityModel> mobilityModel, Ptr<const UavMobilityEnergyModel> energyModel)
{
  std::cout << Simulator::Now ().GetSeconds () << "s ENERGY DEPLETED\n";
  auto currentPosition = mobilityModel->GetPosition ();
  // Drop the UAV to the ground at its current position
  mobilityModel->SetPosition ({currentPosition.x, currentPosition.y, 0});
  Simulator::Stop ();
}

const Vector MOVE_X_VELOCITY (1, 0, 0);
const Vector STOPPED_VELOCITY (0, 0, 0);
const Vector ASCEND_VELOCITY (0, 0, 1);
const Vector DESCEND_VELOCITY (0, 0, -1);
const double INITIAL_ENERGY = 10000;

void
helperApiExample ()
{
  std::cout << "Using Helper\n";
  UavMobilityEnergyModelHelper helper;

  helper.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  helper.SetEnergySource("ns3::BasicEnergySource",
                         "BasicEnergySourceInitialEnergyJ",
                         DoubleValue (INITIAL_ENERGY));

  Ptr<Node> node = CreateObject<Node> ();
  DeviceEnergyModelContainer model = helper.Install (node);

  Ptr<ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();

  Ptr<BasicEnergySource> source = node->GetObject<BasicEnergySource> ();
  source->TraceConnectWithoutContext ("RemainingEnergy",
                                      MakeCallback (&RemainingEnergy));

  model.Get(0)->TraceConnectWithoutContext ("EnergyDepleted",
                                           MakeBoundCallback (&EnergyDepleted,
                                                              mobilityModel));

  Simulator::Schedule (Seconds (2),
                        &ConstantVelocityMobilityModel::SetVelocity,
                        mobilityModel, MOVE_X_VELOCITY);

  Simulator::Schedule (Seconds (5),
                        &ConstantVelocityMobilityModel::SetVelocity,
                        mobilityModel, STOPPED_VELOCITY);

  Simulator::Schedule (Seconds (6),
                        &ConstantVelocityMobilityModel::SetVelocity,
                        mobilityModel, ASCEND_VELOCITY);

  Simulator::Schedule (Seconds (8),
                        &ConstantVelocityMobilityModel::SetVelocity,
                        mobilityModel, DESCEND_VELOCITY);

  Simulator::Schedule (Seconds (10),
                       &ConstantVelocityMobilityModel::SetVelocity,
                       mobilityModel, STOPPED_VELOCITY);

  Simulator::Stop (Seconds (11));
  Simulator::Run ();

  // Explicitly disconnect the trace to resolve a valgrind warning
  // not necessary in your own scenarios
  model.Get(0)->TraceDisconnectWithoutContext ("EnergyDepleted",
                                               MakeBoundCallback (&EnergyDepleted,
                                                                  mobilityModel));

  Simulator::Destroy ();
}

void
noHelperApiExample ()
{
  std::cout << "Not Using Helper\n";
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  Ptr<Node> node = CreateObject<Node> ();

  mobility.Install (node);
  Ptr<ConstantVelocityMobilityModel> mobilityModel =
    node->GetObject<ConstantVelocityMobilityModel> ();

  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource> ();
  energySource->SetNode (node);
  node->AggregateObject (energySource);
  energySource->SetInitialEnergy (INITIAL_ENERGY);
  energySource->TraceConnectWithoutContext ("RemainingEnergy",
                                            MakeCallback (&RemainingEnergy));

  Ptr<UavMobilityEnergyModel> model = CreateObject<UavMobilityEnergyModel> ();
  model->Init (node, energySource);
  node->AggregateObject (model);
  

  model->TraceConnectWithoutContext ("EnergyDepleted",
                                    MakeBoundCallback (&EnergyDepleted, 
                                                       mobilityModel));

  Simulator::Schedule (Seconds (2),
                        &ConstantVelocityMobilityModel::SetVelocity,
                        mobilityModel, MOVE_X_VELOCITY);

  Simulator::Schedule (Seconds (5),
                        &ConstantVelocityMobilityModel::SetVelocity,
                        mobilityModel, STOPPED_VELOCITY);

  Simulator::Schedule (Seconds (6),
                        &ConstantVelocityMobilityModel::SetVelocity,
                        mobilityModel, ASCEND_VELOCITY);

  Simulator::Schedule (Seconds (8),
                        &ConstantVelocityMobilityModel::SetVelocity,
                        mobilityModel, DESCEND_VELOCITY);

  Simulator::Schedule (Seconds (10),
                       &ConstantVelocityMobilityModel::SetVelocity,
                       mobilityModel, STOPPED_VELOCITY);

  Simulator::Stop (Seconds (11));
  Simulator::Run ();

  // Explicitly disconnect the trace to resolve a valgrind warning
  // not necessary in your own scenarios
  model->TraceDisconnectWithoutContext ("EnergyDepleted",
                                        MakeBoundCallback (&EnergyDepleted,
                                                            mobilityModel));
  Simulator::Destroy ();
}

int
main (int argc, char *argv[])
{
  bool useHelperApi = false;

  CommandLine cmd;
  cmd.AddValue ("useHelperApi", "Run the example using the helper API",
                useHelperApi);
  
  cmd.Parse (argc,argv);

  if (useHelperApi)
    helperApiExample ();
  else
    noHelperApiExample ();

}
