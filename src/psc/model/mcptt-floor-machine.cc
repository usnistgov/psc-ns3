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

#include "mcptt-entity-id.h"
#include "mcptt-floor-queue.h"

#include "mcptt-floor-machine.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttFloorMachine");

/** McpttFloorMachine - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorMachine);

TypeId
McpttFloorMachine::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttFloorMachine")
    .SetParent<Object> ()
    ;
  
  return tid;
}

McpttFloorMachine::McpttFloorMachine (void)
  : Object ()
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMachine::~McpttFloorMachine (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttFloorMachine::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMachine::GetTypeId ();
}
/** McpttFloorMachine - end **/

/** McpttFloorMachineNull - begin **/

McpttEntityId
McpttFloorMachineNull::GetNullStateId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static McpttEntityId stateId = McpttEntityId (0, "Null");

  return stateId;
}

TypeId
McpttFloorMachineNull::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static TypeId tid = TypeId ("ns3::McpttFloorMachineNull")
    .SetParent<McpttFloorMachine> ()
    .AddConstructor<McpttFloorMachineNull> ()
  ;

  return tid;
}

McpttFloorMachineNull::McpttFloorMachineNull (void)
  : McpttFloorMachine ()
{
  NS_LOG_FUNCTION (this);
}

McpttFloorMachineNull::~McpttFloorMachineNull (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineNull::AcceptGrant (void)
{
  NS_LOG_FUNCTION (this);
}

uint32_t
McpttFloorMachineNull::GetCurrentSsrc (void) const
{
  NS_LOG_FUNCTION (this);

  return 0;
}

TypeId
McpttFloorMachineNull::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION (this);

  return McpttFloorMachineNull::GetTypeId ();
}

uint8_t
McpttFloorMachineNull::GetPriority (void) const
{
  return 0;
}

Ptr<McpttFloorQueue>
McpttFloorMachineNull::GetQueue (void) const
{
  Ptr<McpttFloorQueue> queue = CreateObject<McpttFloorQueue> (0);

  return queue;
}

Time
McpttFloorMachineNull::GetSetupDelayStartTime (void) const
{
  return Seconds (0);
}

McpttEntityId
McpttFloorMachineNull::GetStateId (void) const
{
  return McpttFloorMachineNull::GetNullStateId ();
}

uint32_t
McpttFloorMachineNull::GetTxSsrc (void) const
{
  return 0;
}

bool
McpttFloorMachineNull::HasCurrentSsrc (void) const
{
  return false;
}

bool
McpttFloorMachineNull::HasFloor (void) const
{
  return false;
}

bool
McpttFloorMachineNull::IsStarted (void) const
{
  return true;
}

void
McpttFloorMachineNull::MediaReady (McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);
}

void
McpttFloorMachineNull::Receive (const McpttFloorMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);
}

void
McpttFloorMachineNull::Receive (const McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineNull::ReleaseRequest (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineNull::SendFloorQueuePosReq (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineNull::SetFloorGrantedCb (const Callback<void>  floorGrantedCb)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineNull::SetOriginator (const bool& originator)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineNull::SetOwner (McpttCall* const& owner)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineNull::SetPriority (uint8_t priority)
{
  NS_LOG_FUNCTION (this << +priority);
}

void
McpttFloorMachineNull::Start (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineNull::Stop (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineNull::TakePushNotification (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorMachineNull::TakeReleaseNotification (void)
{
  NS_LOG_FUNCTION (this);
}
/** McpttFloorMachineNull - end **/

} // namespace ns3

