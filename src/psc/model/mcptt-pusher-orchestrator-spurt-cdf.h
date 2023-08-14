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

#ifndef MCPTT_PUSHER_ORCHESTRATOR_SPURT_CDF_H
#define MCPTT_PUSHER_ORCHESTRATOR_SPURT_CDF_H

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>
#include <ns3/random-variable-stream.h>

#include "mcptt-pusher.h"
#include "mcptt-pusher-orchestrator.h"

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class is used to orchestrate the behavior of a collection of
 * McpttPusher objects. This particular orchestrator uses a CDF
 * generated from call public safety call logs in addition to an
 * activity factor to generate push and release events for a group
 * of McpttPusher objects.
 */
class McpttPusherOrchestratorSpurtCdf : public McpttPusherOrchestratorInterface
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
   * \brief Creates an instance of the McpttPusherOrchestratorSpurtCdf class.
   */
  McpttPusherOrchestratorSpurtCdf (void);
  /**
   * \brief The destructor of the McpttPusherOrchestratorSpurtCdf class.
   */
  virtual ~McpttPusherOrchestratorSpurtCdf (void);
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
   * \brief Changes the ECDF and updates the average obtained with that CDF.
   * \param pttDurationVariable The new variable to use for selecting PTT durations.
   * \param avgPttDuration The average value of the PTT durations obtained from the ECDF being provided.
   */
  virtual void ChangeCdf (Ptr<EmpiricalRandomVariable> pttDurationVariable, double avgPttDuration);
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
   * \brief Generates an interarrival time.
   * \returns The interarrival time.
   */
  virtual Time NextPttIat (void);
  /**
   * \brief Generates a PTT duration.
   * \returns The PTT duration.
   */
  virtual Time NextPttDuration (void);
  /**
   * \brief Starts generating push and release events.
   */
  virtual void Start (void);
  /**
   * \brief Stops generating push and release events.
   * This method may be called multiple times consecutively without side effect.
   */
  virtual void Stop (void);

protected:
  /**
   * Destructor implementation.
   */
  virtual void DoDispose (void);
  /**
   * \brief The PTT duration callback for the underlying orchestrator.
   * \param userId The MCPTT user ID of the pusher.
   * \param duration The PTT duration.
   */
  virtual void PttDurationTrace (uint32_t userId, Time duration);
  /**
   * \brief The PTT interarrival time callback for the underlying orchestrator.
   * \param userId The MCPTT user ID of the pusher.
   * \param iat The PTT interarrival time.
   */
  virtual void PttIatTrace (uint32_t userId, Time iat);
  /**
   * Updates the PTT interarrival variable.
   */
  virtual void UpdatePttIatVariable (void);

private:
  double m_af; //!< The activity factor per user.
  double m_avgPttDuration; //!< The average duration of PTT event.
  Ptr<McpttPusherOrchestrator> m_orchestrator; //!< The underlying orchestrator.

public:
  /**
   * Gets the activity factor.
   * \returns The activity factor.
   */
  virtual double GetActivityFactor (void) const;
  /**
   * Sets the activity factory.
   * \param af The activity factor.
   */
  virtual void SetActivityFactor (double af);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_PUSHER_ORCHESTRATOR_SPURT_CDF_H */
