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

#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-call-machine-grp-basic.h"
#include "mcptt-call-msg.h"
#include "mcptt-counter.h"
#include "mcptt-ptt-app.h"
#include "mcptt-timer.h"

#include "mcptt-emerg-alert-machine.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttEmergAlertMachine");

namespace psc {

/** McpttEmergAlertMachine - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttEmergAlertMachine);

TypeId
McpttEmergAlertMachine::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttEmergAlertMachine")
    .SetParent<Object> ()
  ;

  return tid;
}

McpttEmergAlertMachine::McpttEmergAlertMachine (void)
  : Object ()
{
  NS_LOG_FUNCTION (this);
}

McpttEmergAlertMachine::~McpttEmergAlertMachine (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttEmergAlertMachine::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttEmergAlertMachine::GetTypeId ();
}
/** McpttEmergAlertMachine - end **/

/** McpttCallMachineNull - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttEmergAlertMachineNull);

TypeId
McpttEmergAlertMachineNull::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::psc::McpttEmergAlertMachineNull")
    .SetParent<McpttEmergAlertMachine> ()
    .AddConstructor<McpttEmergAlertMachineNull> ()
  ;

  return tid;
}

McpttEmergAlertMachineNull::McpttEmergAlertMachineNull (void)
  : McpttEmergAlertMachine ()
{
  NS_LOG_FUNCTION (this);
}

McpttEmergAlertMachineNull::~McpttEmergAlertMachineNull (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttEmergAlertMachineNull::CancelEmergAlert (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttEmergAlertMachineNull::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttEmergAlertMachineNull::GetTypeId ();
}

bool
McpttEmergAlertMachineNull::IsInEmergState (void) const
{
  NS_LOG_FUNCTION (this);

  return false;
}

bool
McpttEmergAlertMachineNull::IsStarted (void) const
{
  NS_LOG_FUNCTION (this);

  return true;
}

void
McpttEmergAlertMachineNull::ReceiveGrpCallEmergAlert (const McpttCallMsgGrpEmergAlert& msg)
{
  NS_LOG_FUNCTION (this << msg);
}

void
McpttEmergAlertMachineNull::ReceiveGrpCallEmergAlertCancel (const McpttCallMsgGrpEmergAlertCancel& msg)
{
  NS_LOG_FUNCTION (this << msg);
}

void
McpttEmergAlertMachineNull::SendEmergAlert (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttEmergAlertMachineNull::Start (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttEmergAlertMachineNull::Stop (void)
{
  NS_LOG_FUNCTION (this);
}
/** McpttCallMachineNull - end **/

} // namespace psc
} // namespace ns3

