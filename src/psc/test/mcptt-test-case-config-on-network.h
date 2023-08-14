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

#ifndef MCPTT_TEST_CASE_CONFIG_ON_NETWORK_H
#define MCPTT_TEST_CASE_CONFIG_ON_NETWORK_H


#include <ns3/application-container.h>
#include <ns3/mcptt-call-helper.h>
#include <ns3/mcptt-helper.h>
#include <ns3/mcptt-server-app.h>
#include <ns3/mcptt-server-helper.h>
#include <ns3/ptr.h>
#include <ns3/simple-ref-count.h>
#include <ns3/type-id.h>
#include <ns3/test.h>

#include "mcptt-test-case-config.h"

namespace ns3 {
namespace psc {
namespace tests {

/**
 * Test case configuration framework for on-network MCPTT tests
 */
class McpttTestCaseConfigOnNetwork : public McpttTestCaseConfig
{
public:
  McpttTestCaseConfigOnNetwork (void);
  virtual ~McpttTestCaseConfigOnNetwork (void);
  virtual ApplicationContainer Configure (void);

private:
  McpttCallHelper m_callHelper;
  McpttHelper m_clientHelper;
  McpttServerHelper m_serverHelper;
  Ptr<McpttServerApp> m_serverApp {nullptr};

public:
  virtual void SetCallHelper (const McpttCallHelper& callHelper);
  virtual void SetClientHelper (const McpttHelper& clientHelper);
  virtual void SetServerHelper (const McpttServerHelper& serverHelper);
  Ptr<McpttServerApp> GetServerApp (void) const;
};

} // namespace tests
} // namespace psc
} // namespace ns3

#endif

