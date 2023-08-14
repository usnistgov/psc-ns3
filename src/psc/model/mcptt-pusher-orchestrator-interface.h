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

#ifndef MCPTT_PUSHER_ORCHESTRATOR_INTERFACE_H
#define MCPTT_PUSHER_ORCHESTRATOR_INTERFACE_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

#include "mcptt-pusher.h"

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class defines the interface of objects used to orchestrate the
 * behavior of a collection of McpttPusher objects. In general, objects
 * derived from this abstract base class will decide when the pusher objects
 * will schedule push and release events.
 */
class McpttPusherOrchestratorInterface : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Creates an instance of the McpttPusherOrchestratorInterface class.
   */
  McpttPusherOrchestratorInterface (void);
  /**
   * \brief The destructor of the McpttPusherOrchestratorInterface class.
   */
  virtual ~McpttPusherOrchestratorInterface (void);
  /**
   * \brief Adds a pusher to the orchestrated set.
   * \param pusher The pusher to add.
   */
  virtual void AddPusher (Ptr<McpttPusher> pusher) = 0;
  /**
   * \brief Set the stream for each random variable.
   * \param stream The starting stream number.
   * \returns The number of streams that were set.
   */
  virtual int64_t AssignStreams (int64_t stream) = 0;
  /**
   * \brief Gets the set of orchestrated pushers.
   * \returns The set of pushers.
   */
  virtual std::vector<Ptr<McpttPusher> > GetPushers (void) const = 0;
  /**
   * \brief Gets the set of pushers that are currently pushing the button.
   * \returns The set of pushers.
   */
  virtual std::vector<Ptr<McpttPusher> > GetActivePushers (void) const = 0;
  /**
   * \brief Generates an interarrival time.
   * \returns The interarrival time.
   */
  virtual Time NextPttIat (void) = 0;
  /**
   * \brief Generates a PTT duration.
   * \returns The PTT duration.
   */
  virtual Time NextPttDuration (void) = 0;
  /**
   * \brief Starts orchestrating the pushers.
   */
  virtual void Start (void) = 0;
  /**
   * \brief Schedules the orchestrator to start at the given time.
   * \brief t The time at which to start.
   */
  virtual void StartAt (const Time& t);
  /**
   * \brief Stops orchestrating the pushers.
   */
  virtual void Stop (void) = 0;
  /**
   * \brief Schedules the orchestrator to stop at the given time.
   * \brief t The time at which to stop the orchestrator.
   */
  virtual void StopAt (const Time& t);

protected:
  /**
   * \brief Disposes of Object resources.
   */
  virtual void DoDispose (void);
  /**
   * Traces the interarrival time.
   * \param userId The MCPTT user ID of the pusher.
   * \param iat The interarrival time.
   */
  virtual void TracePttIat (const uint32_t userId, const Time& iat);
  /**
   * Traces the duration.
   * \param userId The MCPTT user ID of the pusher.
   * \param duration The duration.
   */
  virtual void TracePttDuration (const uint32_t userId, const Time& duration);

private:
  EventId m_startEvent; //!< The start event.
  EventId m_stopEvent; //!< The stop event.
  TracedCallback<uint32_t, Time> m_pttIatTrace; //!< The interarrival time trace.
  TracedCallback<uint32_t, Time> m_pttDurationTrace; //!< The duration trace.
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_PUSHER_ORCHESTRATOR_INTERFACE_H */
