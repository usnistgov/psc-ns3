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

#ifndef MCPTT_PUSHER_ORCHESTRATOR_CONTENTION_H
#define MCPTT_PUSHER_ORCHESTRATOR_CONTENTION_H

#include "mcptt-pusher-orchestrator-interface.h"
#include "mcptt-pusher.h"

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/random-variable-stream.h>
#include <ns3/type-id.h>

namespace ns3
{

namespace psc
{

/**
 * \ingroup mcptt
 *
 * This class is used to create contention in addition with a concrete
 * orchestrator. This class connects to the "PttDurationTrace" of another
 * orchestrator and uses the information provided from it and a probability to
 * determine when there will be an additional active pusher. If there is to be
 * contention then then the additional PTT request will be generated during
 * the original PTT regardless of interarrival time, but the duration of the
 * additional PTT will be based on the duration of the underlying
 * orchestrator. Note that in order to have contention there must be at least
 * two pushers being orchestrated, and one of the pushers must not already be
 * actively pushing the PTT button.
 */
class McpttPusherOrchestratorContention : public McpttPusherOrchestratorInterface
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    /**
     * \brief Creates an instance of the McpttPusherOrchestratorContention class.
     */
    McpttPusherOrchestratorContention();
    /**
     * \brief The destructor of the McpttPusherOrchestratorContention class.
     */
    ~McpttPusherOrchestratorContention() override;
    /**
     * \brief Adds a pusher to the orchestrated set.
     * \param pusher The pusher to add.
     */
    void AddPusher(Ptr<McpttPusher> pusher) override;
    /**
     * \brief Set the stream for each random variable.
     * \param stream The starting stream number.
     * \returns The number of streams that were set.
     */
    int64_t AssignStreams(int64_t stream) override;
    /**
     * \brief Gets the set of orchestrated pushers.
     * \returns The set of pushers.
     */
    std::vector<Ptr<McpttPusher>> GetPushers() const override;
    /**
     * \brief Gets the set of pushers that are currently pushing the button.
     * \returns The set of pushers.
     */
    std::vector<Ptr<McpttPusher>> GetActivePushers() const override;
    /**
     * \brief Generates an interarrival time.
     * \returns The interarrival time.
     */
    Time NextPttIat() override;
    /**
     * \brief Generates a PTT duration.
     * \returns The PTT duration.
     */
    Time NextPttDuration() override;
    /**
     * \brief Starts generating push and release events.
     */
    void Start() override;
    /**
     * \brief Stops generating push and release events.
     * This method may be called multiple times consecutively without side effect.
     */
    void Stop() override;

  protected:
    /**
     * Activates the given pusher.
     * \param pusher The given pusher.
     */
    virtual void ActivatePusher(Ptr<McpttPusher> pusher);
    /**
     * Deactivates the current pusher.
     */
    virtual void DeactivatePusher();
    /**
     * Disposes of Object's resources.
     */
    void DoDispose() override;
    /**
     * Initiates a PTT push.
     */
    virtual void PttPush();
    /**
     * Initiates a PTT release.
     */
    virtual void PttRelease();
    /**
     * The trace to keep track of the underlying orchestrator PTT durations.
     * \param userId The MCPTT user ID of the pusher.
     * \param pttDuration the PTT duration.
     */
    virtual void PttDurationTraceCallback(uint32_t userId, Time pttDuration);

  private:
    Ptr<McpttPusher> m_activePusher;                      //!< The active pusher.
    double m_cp;                                          //!< The probability of contention.
    EventId m_nextEvent;                                  //!< The next event.
    Ptr<McpttPusher> m_nextPusher;                        //!< The next pusher to activate.
    Ptr<McpttPusherOrchestratorInterface> m_orchestrator; //!< The underlying orchestrator.
    Ptr<UniformRandomVariable> m_rv;                      //!< A random variable.

  public:
    /**
     * Gets the underlying orchestrator.
     * \returns The underlying orchestrator.
     */
    virtual Ptr<McpttPusherOrchestratorInterface> GetOrchestrator() const;
    /**
     * Sets the underlying orchestrator.
     * \param orchestrator The underlying orchestrator.
     */
    virtual void SetOrchestrator(Ptr<McpttPusherOrchestratorInterface> orchestrator);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_PUSHER_ORCHESTRATOR_CONTENTION_H */
