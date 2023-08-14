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

#ifndef MCPTT_MSG_DROPPER_H
#define MCPTT_MSG_DROPPER_H

#include <ns3/mcptt-msg.h>
#include <ns3/simple-ref-count.h>
#include <ns3/type-id.h>

namespace ns3 {
namespace psc {
namespace tests {

class McpttMsgDropper : public SimpleRefCount<McpttMsgDropper>
{
public:
  McpttMsgDropper (void);
  virtual ~McpttMsgDropper (void);
  virtual bool ShouldDropMsg (const McpttMsg& msg) = 0;
};

class McpttMsgDropperImpl : public McpttMsgDropper
{
public:
  McpttMsgDropperImpl (void);
  McpttMsgDropperImpl (const uint32_t& limit, const TypeId& msgTypeId);
  virtual ~McpttMsgDropperImpl (void);
  virtual bool ShouldDropMsg (const McpttMsg& msg);

protected:
  virtual void IncrementReceived (const uint32_t& amount = 1);
  virtual bool IsLimitReached (void) const;

private:
  uint32_t m_limit;
  TypeId m_msgTypeId;
  uint32_t m_received;

protected:
  virtual uint32_t GetLimit (void) const;
  virtual TypeId GetMsgTypeId (void) const;
  virtual uint32_t GetReceived (void) const;
  virtual void SetLimit (const uint32_t& limit);
  virtual void SetMsgTypeId (const TypeId& msgTypeId);
  virtual void SetReceived (const uint32_t& receieved);
};

} // namespace tests
} // namespace psc
} // namespace ns3

#endif

