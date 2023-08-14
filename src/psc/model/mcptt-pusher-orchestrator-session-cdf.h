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

#ifndef MCPTT_PUSHER_ORCHESTRATOR_SESSION_CDF_H
#define MCPTT_PUSHER_ORCHESTRATOR_SESSION_CDF_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/random-variable-stream.h>

#include "mcptt-pusher.h"
#include "mcptt-pusher-orchestrator-interface.h"

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class is used to create sessions for MCPTT pushers, meaning that it
 * determines when an underlying orchestrator will be started and will it be
 * stopped. Therefore, a session is simply a timespan when pushers are active
 * while outside of that timespan they are not active. The data for
 * determining the duration of sessions was taken from a CDF that was computed
 * from real public safety call logs. An activity factor can also be updated
 * so that sessions are activite for the desired ratio of total simulation
 * time.
 */
class McpttPusherOrchestratorSessionCdf : public McpttPusherOrchestratorInterface
{
public:
  /**
   * \brief The collection of CDF points.
   */
  static const std::vector<std::pair<double, double> > CDF_POINTS;
  /**
   * The average of the CDF points.
   */
  static const double CDF_POINTS_AVG;
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Creates an instance of the McpttPusherOrchestratorSessionCdf class.
   */
  McpttPusherOrchestratorSessionCdf (void);
  /**
   * \brief The destructor of the McpttPusherOrchestratorSessionCdf class.
   */
  virtual ~McpttPusherOrchestratorSessionCdf (void);
  /**
   * \brief Adds a pusher to the orchestrated set.
   * \param pusher The pusher to add.
   */
  virtual void AddPusher (Ptr<McpttPusher> pusher);
  /**
   * \brief Set the stream for each random variable.
   * \param stream The starting stream number.
   * \returns The number of streams that were set.
   */
  virtual int64_t AssignStreams (int64_t stream);
  /**
   * Changes the CDF used for the call session.
   * \param sessionDurationVariable The ECDF to use for session durations.
   * \param avgSessionDuration The average produced by the ECDF.
   */
  virtual void ChangeCdf (Ptr<EmpiricalRandomVariable> sessionDurationVariable, double avgSessionDuration);
  /**
   * \brief Gets the set of orchestrated pushers.
   * \returns The set of pushers.
   */
  virtual std::vector<Ptr<McpttPusher> > GetPushers (void) const;
  /**
   * \brief Gets the set of pushers that are currently pushing the button.
   * \returns The set of pushers.
   */
  virtual std::vector<Ptr<McpttPusher> > GetActivePushers (void) const;
  /**
   * \brief Generates a PTT interarrival time.
   * \returns The interarrival time.
   */
  virtual Time NextPttIat (void);
  /**
   * \brief Generates a PTT duration.
   * \returns The spurt duration.
   */
  virtual Time NextPttDuration (void);
  /**
   * \brief Starts orchestrating.
   */
  virtual void Start (void);
  /**
   * \brief Stops orchestrating.
   */
  virtual void Stop (void);

protected:
  /**
   * Disposes of Object's resources.
   */
  virtual void DoDispose (void);
  /**
   * Starts a sessions.
   */
  virtual void StartSession (void);
  /**
   * Stops a session.
   */
  virtual void StopSession (void);
  /**
   * Updates the session interarrival time variable.
   */
  virtual void UpdateSessionIatVariable (void);

private:
  double m_af; //!< The session activity factor.
  double m_avgSessionDuration; //!< The average duration of a session.
  EventId m_nextEvent; //!< The next event.
  Ptr<McpttPusherOrchestratorInterface> m_orchestrator; //!< The underlying orchestrator.
  Ptr<EmpiricalRandomVariable> m_sessionDurationVariable; //!< Duration of a session.
  Ptr<ExponentialRandomVariable> m_sessionIatVariable; //!< Interarrival time of sesssions.
  TracedCallback<Time> m_sessionIatTrace; //!< The session interarrival time trace.
  TracedCallback<Time> m_sessionDurationTrace; //!< The session duration trace.
  TracedValue <bool> m_active; //!< A flag used to indicate if a session is active.

public:
  /**
   * Gets the activity factor.
   * \returns The activity factor.
   */
  virtual double GetActivityFactor (void) const;
  /**
   * Gets the underlying orchestrator.
   * \returns The underlying orchestrator.
   */
  virtual Ptr<McpttPusherOrchestratorInterface> GetOrchestrator (void) const;
  /**
   * Sets the activity factory.
   * \param af The activity factor.
   */
  virtual void SetActivityFactor (double af);
  /**
  * Sets the underlying orchestrator.
  * \param orchestrator The underlying orchestrator.
  */
  virtual void SetOrchestrator (Ptr<McpttPusherOrchestratorInterface> orchestrator);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_PUSHER_ORCHESTRATOR_SESSION_CDF_H */
