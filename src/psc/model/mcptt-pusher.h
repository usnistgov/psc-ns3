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

#include "mcptt-ptt-app.h"

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class is used to simulate the action of pushing and releasing a
 * button. An instance of this class schedules events using a random number
 * generator to simulate "pushing" and "releasing" the button of an
 * McpttPttApp.
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
   * Pushes the PTT app's button and enters the pushed state.
   */
  virtual void Push (void);
  /**
   * Enter the pushed state based on a callback.
   */
  virtual void NotifyPushed (void);
  /**
   * Releases the PTT app's button and enters the released state.
   */
  virtual void Release (void);
  /**
   * Enter the released state based on a callback.
   */
  virtual void NotifyReleased (void);
  /**
   * Schedules when the pusher will perform a push, based on random variable.
   * This method will return false if there is already a scheduled push event.
   * \return true if schedule was successful, false if not
   */
  virtual bool SchedulePush (void);
  /**
   * Schedules a specific delay after which the pusher will perform a push.
   * This method will return false if there is already a scheduled push event.
   * \param delay The amount of time from now that the pusher will perform the push.
   * \return true if schedule was successful, false if not
   */
  virtual bool SchedulePush (const Time& delay);
  /**
   * Cancels any pending push event
   * This method will return false if there was not a scheduled push event.
   * \return true if cancel was successful, false if not
   */
  virtual bool CancelPush (void);
  /**
   * Schedules when the pusher will perform a release.
   * This method will return false if there is already a scheduled release event.
   * \return true if schedule was successful, false if not
   */
  virtual bool ScheduleRelease (void);
  /**
   * Schedules a specific delay after which the pusher will perform a release.
   * This method will return false if there is already a scheduled release event.
   * \param delay The amount of time from now that the pusher will perform the release.
   * \return true if schedule was successful, false if not
   */
  virtual bool ScheduleRelease (const Time& delay);
  /**
   * Cancels any pending release event
   * This method will return false if there was not a scheduled release event.
   * \return true if cancel was successful, false if not
   */
  virtual bool CancelRelease (void);
  /**
   * \brief Starts pushing and releasing the PTT app's button.
   */
  virtual void Start (void);
  /**
   * \brief Stops pushing and releasing the PTT app's button.
   * This method may be called multiple times consecutively without side effect.
   */
  virtual void Stop (void);

protected:
  /**
   * \brief Disposes of the McpttPttApp instance.
   */
  virtual void DoDispose (void);

private:
  /**
   * The flag that indicates if the button should automatically be pushed/released.
   */
  bool m_automatic;
  Ptr<RandomVariableStream> m_pttIatVariable; //!< The random variable used for automatic pushes.
  Ptr<RandomVariableStream> m_pttDurationVariable; //!< The random variable used for automatic releases.
  Ptr<McpttPttApp> m_pttApp; //!< A pointer to the PTT app.
  TracedValue <bool> m_pushing; //!< A flag used to indicate if the pusher is in the "pushing" state.
  EventId m_pushEvent; //!< EventId of most recently scheduled push event
  EventId m_releaseEvent; //!< EventId of most recently scheduled release event
  TracedCallback<uint32_t, Time> m_pttIatTrace; //!< The interarrival time trace.
  TracedCallback<uint32_t, Time> m_pttDurationTrace; //!< The duration trace.

public:
  /**
   * \brief Gets the object with the button to push.
   * \return The object with the button to push.
   */
  virtual Ptr<McpttPttApp> GetPttApp (void) const;
  /**
   * Sets the object with the button to push.
   * \param pttApp The object with the button to push.
   */
  virtual void SetPttApp (Ptr<McpttPttApp> pttApp);

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

} // namespace psc
} // namespace ns3

#endif /* MCPTT_PUSHER_H */
