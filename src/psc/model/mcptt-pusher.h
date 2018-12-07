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

#ifndef MCPTT_PUSHER_H
#define MCPTT_PUSHER_H

#include <ns3/event-id.h>
#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/random-variable-stream.h>
#include <ns3/traced-value.h>
#include <ns3/type-id.h>
#include <set>

#include "mcptt-pushable.h"

namespace ns3 {
 
/**
 * \ingroup mcptt
 *
 * This class is used to simulate the action of pushing and releasing a
 * button. An instance of this class schedules events using a random number
 * generator to simulate "pushing" and "releasing" the button of an
 * ns3::McpttPushable.
 */
class McpttPusher : public Object 
{
public:
 /**
  * \brief Get the type ID.
  * \return the object TypeId
  */
 static TypeId GetTypeId (void);
 /**
  * \brief Creates an instance of the McpttPusher class.
  */
 McpttPusher (void);
 /**
  * \brief The destructor of the McpttPusher class.
  */
 virtual ~McpttPusher (void);
 /**
  * \brief Indicates whether or not the pusher is in the "push" state.
  * \returns True, if the pusher is pushing, or false otherwise.
  */
 virtual bool IsPushing (void) const;
 /**
  * Pushes the pushable's button and enters the pushed state.
  */
 virtual void Push (void);
 /**
  * Notify (callback) that the pushable has successfully pushed.
  */
 virtual void NotifyPushed (void);
 /**
  * \brief Releases the pushable's button and enters the released state.
  */
 virtual void Release (void);
 /**
  * Notify (callback) that the pushable has successfully released.
  */
 virtual void NotifyReleased (void);
 /**
  * Schedules when the pusher will perform a push.
  */
 virtual void SchedulePush (void);
 /**
  * Schedules when the pusher will perform a push.
  * \param delay The amount of time from now that the pusher will perform the push.
  */
 virtual void SchedulePush (const Time& delay);
 /**
  * Schedules when the pusher will perform a release.
  */
 virtual void ScheduleRelease (void);
 /**
  * Scheuldes when the pusher will perform a release.
  * \param delay The amount of time from now that the pusher will perform the release.
  */
 virtual void ScheduleRelease (const Time& delay);
 /**
  * \brief Starts pushing and releasing the pushables button.
  */
 virtual void Start (void);
 /**
  * \brief Stops pushing and releasing the pushables button.
  */
 virtual void Stop (void);
protected:
 /**
  * \brief Disposes of the McpttPttApp instance.
  */
 virtual void DoDispose (void);
 /**
  * Actually calls the pushable's "TakePushNotification" function.
  */
 virtual void PushPushable (void);
 /**
  * Actually calls the pushable's "TakeReleaseNotification" function.
  */
 virtual void ReleasePushable (void);
private:
 /**
  * The flag that indicates if the button should automatically be pushed/released.
  */
 bool m_automatic;
 Ptr<RandomVariableStream> m_pushVariable; //!< The random variable used for automatic pushes.
 Ptr<RandomVariableStream> m_releaseVariable; //!< The random variable used for automatic releases.
 /* NOTE:
  * Using a raw pointer here because this type represents
  * an interface that does not inherit from any of the ns3
  * base classes. Therefore, this class should not allocate
  * or dispose of the memory referenced by this pointer, and
  * it is up to the user to provide a valid object and manage
  * its lifecycle. This pointer's sole purpose is to provide access to the
  * object that the user has provided.
  */
 McpttPushable* m_pushable; //!< A pointer to the pushable.
 TracedValue <bool> m_pushing; //!< A flag used to indicate if the pusher is in the "pushing" state.
 std::set<EventId> m_pushReleaseEvents;   //!< EventIds of push/release events
public:
 /**
  * \brief Gets the object with the button to push.
  * \return The object with the button to push.
  */
 virtual McpttPushable* GetPushable (void) const;
 /**
  * Sets the object with the button to push.
  * \param pushable The object with the button to push.
  */
 virtual void SetPushable (McpttPushable* pushable);
protected:
 /**
  * \brief Sets the flag that indicates if the object is in the "push" state.
  * \param pushing The flag.
  */
 virtual void SetPushing (bool pushing);
 /**
  * \brief Cancels all push-release events.
  */
 virtual void CancelEvents (void);
};
 
} // namespace ns3

#endif /* MCPTT_PUSHER_H */
