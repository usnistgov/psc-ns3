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

#include "mcptt-test-case.h"

namespace ns3 {
namespace psc {
namespace tests {

McpttTestCase::McpttTestCase (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : TestCase (name),
    m_config (0)
{
  SetConfig (config);
}

Ptr<McpttPttApp>
McpttTestCase::GetApp (uint32_t index)
{
  ApplicationContainer clientApps = GetClientApps ();
  Ptr<Application> app = clientApps.Get (index);
  Ptr<McpttPttApp> pttApp = DynamicCast<McpttPttApp, Application> (app);

  return pttApp;
}

Ptr<McpttCallMachine>
McpttTestCase::GetCallMachine (uint32_t index)
{
  Ptr<McpttPttApp> pttApp = GetApp (index);
  Ptr<McpttCall> call = pttApp->GetSelectedCall ();
  Ptr<McpttCallMachine> machine = call->GetCallMachine ();

  return machine;
}

Ptr<McpttFloorParticipant>
McpttTestCase::GetFloorMachine (uint32_t index)
{
  Ptr<McpttPttApp> pttApp = GetApp (index);
  Ptr<McpttCall> call = pttApp->GetSelectedCall ();
  Ptr<McpttFloorParticipant> machine = call->GetFloorMachine ();

  return machine;
}

void
McpttTestCase::Configure (void)
{
  Ptr<McpttTestCaseConfig> configuration = GetConfig ();

  ApplicationContainer clientApps = configuration->Configure ();

  SetClientApps (clientApps);
}

void
McpttTestCase::Stop (void)
{
  Simulator::Stop ();
}

void
McpttTestCase::DoRun (void)
{
  Configure ();
  Execute ();
}

ApplicationContainer
McpttTestCase::GetClientApps (void) const
{
  return m_clientApps;
}

Ptr<McpttTestCaseConfig>
McpttTestCase::GetConfig (void) const
{
  return m_config;
}

void
McpttTestCase::SetClientApps (const ApplicationContainer& clientApps)
{
  m_clientApps = clientApps;
}

void
McpttTestCase::SetConfig (Ptr<McpttTestCaseConfig>  config)
{
  m_config = config;
}

} // namespace tests
} // namespace psc
} // namespace ns3

