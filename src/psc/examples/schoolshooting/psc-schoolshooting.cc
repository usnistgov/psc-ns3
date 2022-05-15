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

#include "ns3/psc-module.h"
#include "ns3/lte-module.h"
#include "ns3/core-module.h"

#ifdef HAS_NETSIMULYZER
#include "ns3/netsimulyzer-module.h"
#include "schoolshooting-lte-vis-helper.h"
#endif

#include "schoolshooting-application-helper.h"
#include "schoolshooting-lte-helper.h"
#include <iostream>


using namespace ns3;
using namespace psc;

/**
 * This example shows how to instantiate the school shooting incident scenario
 * over an LTE network deployment using the various helpers.
 * The simulation is composed of 5 steps:
 * 1- Create the scenario definition (i.e. creates the nodes, mobility, events)
 * 2- Deploy the LTE network topology (if enabled)
 * 3- Configure traces of interest
 * 4- Enable visualization module (if enabled)
 * 5- Run the simulation
 *
 * Optional parameters for this examples are:
 * - duration: The simulation duration (default = 14700 (i.e. 245 min)).
 *   Note that the scenario definition defines key events that will influence
 *   the start/stop of the applications. Changing the simulation time will
 *   simply truncate the scenario.
 * - enableLte: Enable the deployment of an LTE network (default = true).
 * - enableLteTraces: Enable LTE traces (default = true).
 * - enableGuiTraces: enable traces for visualization (default = false)
 * - guiResolution: granularity of the visualization traces (default = 1000 ms).
 *   Larger values will reduce file size but lower resolution.
 * - reportTime: Time interval to print simulation time for monitoring progress 
 *   (default = 0 s - disabled)
 * - testing: Enable testing by using modified application start/stop time for 100 s simulation 
 *   (default = false)
 * - appId: The ID for the application to use. Allows for testing a single application 
 *   (default = -1, i.e., all applications are enabled)
 */

int
main (int argc, char *argv [])
{
  //defines default values for input parameters
  Time duration = Seconds (14700);
  bool enableLte = true;
  bool enableLteTraces = true;
  bool enableNetsimulyzer = false;
  Time reportTime = Seconds (1);
  int guiResolution = 1000; //refresh time in ms
  bool testing = false; //indicate if the scenario will use modified time to run shorter simulation
  int applicationId = -1; //all applications by default

  CommandLine cmd;

  cmd.AddValue ("duration", "Duration (in Seconds) of the simulation", duration);
  cmd.AddValue ("enableLte", "Flag to enable LTE deployment", enableLte);
  cmd.AddValue ("enableLteTraces", "Flag to enable LTE traces", enableLteTraces);
  cmd.AddValue ("enableGuiTraces", "Flag to enable the visualization traces", enableNetsimulyzer);
  cmd.AddValue ("guiResolution", "Granularity of the visualization", guiResolution);
  cmd.AddValue ("reportTime", "Indicates whether or not the simulation time is reported periodically", reportTime);
  cmd.AddValue ("testing", "Indicates if using modified application times for 100 s simulation", testing);
  cmd.AddValue ("appId", "The ID for the application (see schoolshooting-application-helper.h::SchoolSchootingApplicationId", applicationId);
  cmd.Parse (argc, argv);

  //validate input
  if (applicationId != -1)
  {
    NS_ABORT_MSG_IF (applicationId < 0 || applicationId > SchoolShootingApplicationHelper::SCHOOL_SHOOTING_VIDEO_CONFERENCE, "Invalid range for appId");
  }

  //1. Create a scenario definition
  Ptr<SchoolShootingDefinitionHelper> scenarioDefinitionHelper = CreateObject<SchoolShootingDefinitionHelper> ();
  //Do configuration of the scenario if needed
  //Classes are primarily used for visualization purposes as currently no propagation model would use them properly
  scenarioDefinitionHelper->SetAttribute ("CreateClassrooms", BooleanValue (false));

  //2. Deploy a network
  Ptr<SchoolShootingLteHelper> lteScenarioHelper = nullptr;
  if (enableLte)
    {
      lteScenarioHelper = CreateObject<SchoolShootingLteHelper> ();
      lteScenarioHelper->SetScenarioDefinitionHelper (scenarioDefinitionHelper);

      //Initialize the network, i.e., deploy eNodeBs and UEs
      lteScenarioHelper->Initialize ();
    }
  else
    {
      scenarioDefinitionHelper->CreateScenario ("SchoolShootingNoLte");
    }


  //3. Deploy applications
  //This helper allows to enable specific applications, which can be useful to verify their behaviors
  Ptr<SchoolShootingApplicationHelper> scenarioApplicationHelper = CreateObject<SchoolShootingApplicationHelper> ();

  if (enableLte)
    {
      scenarioApplicationHelper->SetTechnologyHelper (lteScenarioHelper);
      scenarioApplicationHelper->SetAttribute ("Testing", BooleanValue(testing));
      scenarioApplicationHelper->SetScenarioDefinitionHelper (scenarioDefinitionHelper);

      //Enable selected application if specified, otherwise all applications will be enabled.
      if (applicationId != -1)
      {
        scenarioApplicationHelper->EnableApplication ((SchoolShootingApplicationHelper::SchoolSchootingApplicationId) applicationId);
      }

      scenarioApplicationHelper->Initialize ();

    }

  //4. Enable traces
  //we use the standard helper to write scenario information
  Ptr<PscScenarioTraceHelper> scenarioTraceHelper = CreateObject<PscScenarioTraceHelper> (scenarioDefinitionHelper->GetScenarioDefinition ());
  scenarioTraceHelper->EnableScenarioTraces ();
  scenarioTraceHelper->EnableTimeTrace (reportTime);

  //we can also enable traces specific the network deployment
  if (enableLte)
    {
      if (enableLteTraces)
      {
        lteScenarioHelper->EnableLteTraces ();
      }
      //enable traces for all active applications
      scenarioApplicationHelper->EnableApplicationTraces ();
    }

  //5. Enable visualization
#ifdef HAS_NETSIMULYZER
  Ptr<SchoolShootingLteVisHelper> guiHelper;
  if (enableNetsimulyzer)
    {
      if (enableLte)
        {
          guiHelper = CreateObject <SchoolShootingLteVisHelper> (scenarioApplicationHelper);
          //enable graphs for all active applications
          guiHelper->EnableApplicationGraphs ();

          /** Need to delay enabling eNodeB throughput graphs until after ALL UEs are attached **/
          Simulator::Schedule (Seconds (1), &SchoolShootingLteVisHelper::EnableEnbThroughputGraphs, guiHelper);

          std::cout << "App graphs configured" << std::endl;
        }
      else
        {
          guiHelper = CreateObject <SchoolShootingLteVisHelper> (scenarioDefinitionHelper->GetScenarioDefinition ());
        }

      guiHelper->GetOrchestrator()->SetTimeStep (MilliSeconds (guiResolution), Time::Unit::MS);
      guiHelper->SetOrchestratorAttribute ("MobilityPollInterval", TimeValue (MilliSeconds (guiResolution)));
      guiHelper->EnableKeyEventLog ();
    }
#else
  if (enableNetsimulyzer)
    {
      std::cerr << "GUI Traces enabled, but 'netsimulyzer' module not found! Ignoring.\n";
    }
#endif

  //6. Run the simulation
  if (testing && duration > Seconds (100))
  {
    Simulator::Stop (Seconds (100));  
  }
  else 
  {
    Simulator::Stop (duration);
  }

  std::cout << "Running Simulation" << std::endl;

  Simulator::Run ();

  // End simulation and shutdown
  Simulator::Destroy ();

  return 0;
}

