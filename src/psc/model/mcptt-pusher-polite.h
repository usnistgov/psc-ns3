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

#ifndef MCPTT_PUSHER_POLITE_H
#define MCPTT_PUSHER_POLITE_H

#include <ns3/type-id.h>

#include "mcptt-pusher.h"

namespace ns3 {
 
/**
 * \ingroup mcptt
 *
 * This class is used to simulate the action of pushing and releasing a
 * button. This instance will only push the button if no one is talking.
 */
class McpttPusherPolite : public McpttPusher 
{
public:
 /**
  * \brief Get the type ID.
  * \return the object TypeId
  */
 static TypeId GetTypeId (void);
 /**
  * \brief Creates an instance of the McpttPusherPolite class.
  */
 McpttPusherPolite (void);
 /**
  * \brief The destructor of the McpttPusherPolite class.
  */
 virtual ~McpttPusherPolite (void);
 /**
  * Pushes the pushable's button and enters the pushed state.
  */
 virtual void Push (void);
};
 
} // namespace ns3

#endif /* MCPTT_PUSHER_POLITE_H */
