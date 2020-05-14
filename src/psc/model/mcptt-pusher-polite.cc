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

#include <ns3/boolean.h>
#include <ns3/log.h>
#include <ns3/ptr.h>

#include "mcptt-floor-participant.h"
#include "mcptt-off-network-floor-participant.h"
#include "mcptt-on-network-floor-participant.h"
#include "mcptt-ptt-app.h"

#include "mcptt-pusher-polite.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttPusherPolite");

NS_OBJECT_ENSURE_REGISTERED (McpttPusherPolite);

TypeId
McpttPusherPolite::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McpttPusherPolite")
    .SetParent<McpttPusher> ()
    .AddConstructor<McpttPusherPolite>()
    ;

  return tid;
}

McpttPusherPolite::McpttPusherPolite (void)
  : McpttPusher ()
{
  NS_LOG_FUNCTION (this);
}

McpttPusherPolite::~McpttPusherPolite (void)
{
  NS_LOG_FUNCTION (this);
}

void
McpttPusherPolite::Push (void)
{
  NS_LOG_FUNCTION (this);

  bool shouldPush = false;
  if (McpttPttApp* pttApp = dynamic_cast<McpttPttApp*> (GetPushable ())) // Try to get the McpttPttApp
    {
      Ptr<McpttFloorParticipant> participant = pttApp->GetSelectedCall ()->GetFloorMachine (); // The floor machine can tell if someone is talking
      if (Ptr<McpttOffNetworkFloorParticipant> offNetworkParticipant = DynamicCast<McpttOffNetworkFloorParticipant, McpttFloorParticipant> (participant))
        { // This is off-network
          shouldPush = !offNetworkParticipant->GetT203 ()->IsRunning (); // If T203 is running then someone is talking.
        }
      else if (Ptr<McpttOnNetworkFloorParticipant> onNetworkParticipant = DynamicCast<McpttOnNetworkFloorParticipant, McpttFloorParticipant> (participant))
        { // This is on-network
          shouldPush = !onNetworkParticipant->GetT103 ()->IsRunning (); // If T103 is running then someone is talking.
        }
      else
        {
          NS_ABORT_MSG ("This floor participant is not supported.");
        }
    }
  else
    {
      NS_ABORT_MSG ("This pushable is not supported.");
    }

    if (shouldPush == true) // No one is talking so push the button
      {
        NS_LOG_LOGIC ("Floor is idle so allow PTT request.");
        McpttPusher::Push ();
      }
    else
      {
        NS_LOG_LOGIC ("Floor is NOT idle so do NOT allow PTT request.");
        BooleanValue automatic;
        GetAttribute ("Automatic", automatic);

        if (automatic.Get () == true) // Someone is talking so try again later
        {
          SchedulePush ();
        }
      }
}

} // namespace ns3
