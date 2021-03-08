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

#include "mcptt-floor-participant.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttFloorParticipant");

namespace psc {

/** McpttFloorParticipant - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorParticipant);

TypeId
McpttFloorParticipant::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttFloorParticipant")
    .SetParent<Object> ()
  ;

  return tid;
}

McpttFloorParticipant::McpttFloorParticipant (void)
{
  NS_LOG_FUNCTION (this);
}

McpttFloorParticipant::~McpttFloorParticipant (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttFloorParticipant::GetInstanceTypeId (void) const
{
  return McpttFloorParticipant::GetTypeId ();
}
/** McpttFloorParticipant - end **/

/** McpttFloorParticipantNull - begin **/
NS_OBJECT_ENSURE_REGISTERED (McpttFloorParticipantNull);

TypeId
McpttFloorParticipantNull::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttFloorParticipantNull")
    .SetParent<Object> ()
    .AddConstructor<McpttFloorParticipantNull> ()
  ;

  return tid;
}

McpttFloorParticipantNull::McpttFloorParticipantNull (void)
  : McpttFloorParticipant ()
{
  NS_LOG_FUNCTION (this);
}

McpttFloorParticipantNull::~McpttFloorParticipantNull (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorParticipantNull::AcceptGrant (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
McpttFloorParticipantNull::GetInstanceTypeId (void) const
{
  return McpttFloorParticipantNull::GetTypeId ();
}

bool
McpttFloorParticipantNull::HasFloor (void) const
{
  return false;
}

bool
McpttFloorParticipantNull::IsStarted (void) const
{
  return true;
}

void
McpttFloorParticipantNull::MediaReady (McpttMediaMsg& msg)
{
  NS_LOG_FUNCTION (this << msg);
}

void
McpttFloorParticipantNull::ReleaseRequest (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorParticipantNull::SendFloorQueuePositionRequest (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorParticipantNull::SetFloorGrantedCb (const Callback<void>  floorGrantedCb)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorParticipantNull::SetOriginator (const bool& originator)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorParticipantNull::SetCall (Ptr<McpttCall> call)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorParticipantNull::SetPriority (uint8_t priority)
{
  NS_LOG_FUNCTION (this << +priority);
}

void
McpttFloorParticipantNull::Start (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorParticipantNull::Stop (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorParticipantNull::PttPush (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttFloorParticipantNull::PttRelease (void)
{
  NS_LOG_FUNCTION (this);
}
/** McpttFloorParticipantNull - end **/

} // namespace psc
} // namespace ns3

