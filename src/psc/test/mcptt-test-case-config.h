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

#ifndef MCPTT_TEST_CASE_CONFIG_H
#define MCPTT_TEST_CASE_CONFIG_H


#include <ns3/nstime.h>
#include <ns3/application-container.h>
#include <ns3/mcptt-ptt-app.h>
#include <ns3/ptr.h>
#include <ns3/simple-ref-count.h>
#include <ns3/type-id.h>
#include <ns3/test.h>

namespace ns3 {
namespace psc {
namespace tests {

class McpttTestCall;

/**
 * Test case configuration framework for off-network MCPTT tests
 */
class McpttTestCaseConfig : public SimpleRefCount<McpttTestCaseConfig>
{
public:
  McpttTestCaseConfig (void);
  McpttTestCaseConfig (const uint32_t& appCount, const TypeId& appTid, const TypeId& callTid, const TypeId& floorTid, const Time& start, const Time& stop);
  virtual ~McpttTestCaseConfig (void);
  virtual ApplicationContainer Configure (void);

private:
  uint32_t m_appCount;
  TypeId m_appTid;
  TypeId m_callTid;
  TypeId m_floorTid;
  Time m_start;
  Time m_stop;

public:
  virtual uint32_t GetAppCount (void) const;
  virtual TypeId GetAppTid (void) const;
  virtual TypeId GetCallTid (void) const;
  virtual TypeId GetFloorTid (void) const;
  virtual Time GetStart (void) const;
  virtual Time GetStop (void) const;
  virtual void SetAppCount (const uint32_t& appCount);
  virtual void SetAppTid (const TypeId& appTid);
  virtual void SetCallTid (const TypeId& callTid);
  virtual void SetFloorTid (const TypeId& floorTid);
  virtual void SetStart (const Time& start);
  virtual void SetStop (const Time& stop);
  virtual Ptr<McpttTestCall> CreateTestCall (ObjectFactory& callFac, ObjectFactory& floorFac, uint16_t callId) const;
};

} //namespace tests
} //namespace psc
} //namespace ns3

#endif

