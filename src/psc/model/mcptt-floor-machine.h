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

#ifndef MCPTT_FLOOR_MACHINE_H
#define MCPTT_FLOOR_MACHINE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/nstime.h>
#include <ns3/type-id.h>

#include "mcptt-entity-id.h"
#include "mcptt-floor-msg.h"
#include "mcptt-floor-queue.h"
#include "mcptt-media-msg.h"

namespace ns3 {

class McpttCall;

/**
 * \ingroup mcptt
 *
 * This interface exists to create a common API for the MCPTT floor control
 * state machine described in TS 24.380 v14.4.0.
 */
class McpttFloorMachine : public Object
{
public:
 /**
  * \brief Gets the ID of the McpttFloorMachine type.
  * \returns The type ID.
  */
 static TypeId GetTypeId (void);
 /**
  * \brief Creates an instance of the McpttFloorMachine class.
  */
 McpttFloorMachine (void);
 /**
  * \brief The destructor of the McpttFloorMachine class.
  */
 virtual ~McpttFloorMachine (void);
 /**
  * Accepts the floor grant.
  */
 virtual void AcceptGrant (void) = 0;
 /**
  * Gets the SSRC stored for the current arbitrator.
  * \returns The SSRC.
  */
 virtual uint32_t GetCurrentSsrc (void) const = 0;
 /**
  * Gets the type ID of this McpttFloorMachine instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
 /**
  * Gets the current priority of the floor.
  * \returns The current priority.
  */
 virtual uint8_t GetPriority (void) const = 0;
/**
  * Gets the queue.
  * \returns The queue.
  */
 virtual Ptr<McpttFloorQueue> GetQueue (void) const = 0;
 /**
  * Gets the setup delay.
  * \returns The setup delay time.
  */
 virtual Time GetSetupDelayStartTime (void) const = 0;
 /**
  * Gets the ID of the state.
  * \returns The state ID.
  */
 virtual McpttEntityId GetStateId (void) const = 0;
 /**
  * Gets the SSRC to use when sending a message.
  * \returns The SSRC to use when sending a message.
  */
 virtual uint32_t GetTxSsrc (void) const = 0;
 /**
  * Indicates if an SSRC is stored for the current arbitrator.
  * \returns True, if an SSRC is stored; otherwise false.
  */
 virtual bool HasCurrentSsrc (void) const = 0;
 /**
  * Indicates whether or not the current participant is the acting arbitrator.
  * \returns True, if the current participant is the acting arbitrator.
  */
 virtual bool HasFloor (void) const = 0;
 /**
  * Indicates whether or not the floor machine has been started.
  * \returns True, if the floor machine has been started.
  */
 virtual bool IsStarted (void) const = 0;
 /**
  * Indicates that media is ready to be sent.
  * \param msg The media that is ready to be sent.
  */
 virtual void MediaReady (McpttMediaMsg& msg) = 0;
  /**
  * Receives a message.
  * \param msg The message that was received.
  */
 virtual void Receive (const McpttFloorMsg& msg) = 0;
 /**
  * Receives a message.
  * \param msg The message that was received.
  */
 virtual void Receive (const McpttMediaMsg& msg) = 0;
  /**
  * Releases a request.
  */
 virtual void ReleaseRequest (void) = 0;
 /**
  * Sends a floor queue position request message.
  */
 virtual void SendFloorQueuePosReq (void) = 0;
 /**
  * Sets the callback used to indicate that the floor has been granted.
  * \param floorGrantedCb The callback to set
  */
 virtual void SetFloorGrantedCb (const Callback<void>  floorGrantedCb) = 0;
 /**
  * Sets the flag that indicates if this floor machine is the originator of the call.
  * \param originator True, if this participant is the originator of the call.
  */
 virtual void SetOriginator (const bool& originator) = 0;
 /**
  * Sets the owner of the floor machine.
  * \param owner The owner.
  */
 virtual void SetOwner (McpttCall* const& owner) = 0;
  /**
  * Sets the current priority of the floor.
  * \param priority The current priority.
  */
 virtual void SetPriority (uint8_t priority) = 0;
 /**
  * Starts the floor machine.
  */
 virtual void Start (void) = 0;
 /**
  * Stops the floor machine.
  */
 virtual void Stop (void) = 0;
 /**
  * Notifies this machine that the button has been pushed.
  */
 virtual void TakePushNotification (void) = 0;
 /**
  * Notifies this machine that the button has been released.
  */
 virtual void TakeReleaseNotification (void) = 0;
};

/**
 * \ingroup mcptt
 * \brief A class that represents an MCPTT floor machine.
 */
class McpttFloorMachineNull : public McpttFloorMachine
{
public:
 /**
  * Gets the state ID of the null state.
  * \returns The null state ID.
  */
 static McpttEntityId GetNullStateId (void);
 /**
  * \brief Gets the ID of the McpttFloorMachineNull type.
  * \returns The type ID.
  */
 static TypeId GetTypeId (void);
 /**
  * \brief Creates an instance of the McpttFloorMachineNull class.
  */
 McpttFloorMachineNull (void);
 /**
  * \brief The destructor of the McpttFloorMachineNull class.
  */
 virtual ~McpttFloorMachineNull (void);
 /**
  * Accepts the floor grant.
  */
 virtual void AcceptGrant (void);
 /**
  * Gets the SSRC stored for the current arbitrator.
  * \returns The SSRC.
  */
 virtual uint32_t GetCurrentSsrc (void) const;
 /**
  * Gets the type ID of this McpttFloorMachine instance.
  * \returns The type ID.
  */
 virtual TypeId GetInstanceTypeId (void) const;
 /**
  * Gets the current priority of the floor.
  * \returns The current priority.
  */
 virtual uint8_t GetPriority (void) const;
/**
  * Gets the queue.
  * \returns The queue.
  */
 virtual Ptr<McpttFloorQueue> GetQueue (void) const;
 /**
  * Gets the setup delay.
  * \returns The setup delay time.
  */
 virtual Time GetSetupDelayStartTime (void) const;
 /**
  * Gets the ID of the state.
  * \returns The state ID.
  */
 virtual McpttEntityId GetStateId (void) const;
 /**
  * Gets the SSRC to use when sending a message.
  * \returns The SSRC to use when sending a message.
  */
 virtual uint32_t GetTxSsrc (void) const;
 /**
  * Indicates if an SSRC is stored for the current arbitrator.
  * \returns True, if an SSRC is stored; otherwise false.
  */
 virtual bool HasCurrentSsrc (void) const;
 /**
  * Indicates whether or not the current participant is the acting arbitrator.
  * \returns True, if the current participant is the acting arbitrator.
  */
 virtual bool HasFloor (void) const;
 /**
  * Indicates whether or not the floor machine has been started.
  * \returns True, if the floor machine has been started.
  */
 virtual bool IsStarted (void) const;
 /**
  * Indicates that media is ready to be sent.
  * \param msg The media that is ready to be sent.
  */
 virtual void MediaReady (McpttMediaMsg& msg);
 /**
  * Receives a floor control message.
  * \param msg The received message.
  */
 virtual void Receive (const McpttFloorMsg& msg);
 /**
  * Receives a media message.
  * \param msg The received message.
  */
 virtual void Receive (const McpttMediaMsg& msg);
 /**
  * Releases a request.
  */
 virtual void ReleaseRequest (void);
 /**
  * Sends a floor queue position request message.
  */
 virtual void SendFloorQueuePosReq (void);
 /**
  * Sets the callback used to indicate that the floor has been granted.
  * \param floorGrantedCb The callback to set
  */
 virtual void SetFloorGrantedCb (const Callback<void>  floorGrantedCb);
  /**
  * Sets the flag that indicates if this floor machine is the originator of the call.
  * \param originator True, if this participant is the originator of the call.
  */
 virtual void SetOriginator (const bool& originator);
 /**
  * Sets the owner.
  * \param owner The owner.
  */
 virtual void SetOwner (McpttCall* const& owner);
 /**
  * Sets the current priority of the floor.
  * \param priority The current priority.
  */
 virtual void SetPriority (uint8_t priority);
 /**
  * Starts the floor machine.
  */
 virtual void Start (void);
 /**
  * Stops the floor machine.
  */
 virtual void Stop (void);
 /**
  * Notifies this machine that the button has been pushed.
  */
 virtual void TakePushNotification (void);
 /**
  * Notifies this machine that the button has been released.
  */
 virtual void TakeReleaseNotification (void);
};

} // namespace ns3

#endif /* MCPTT_FLOOR_MACHINE_H */

