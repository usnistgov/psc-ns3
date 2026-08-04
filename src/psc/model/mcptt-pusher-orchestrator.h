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

#ifndef MCPTT_PUSHER_ORCHESTRATOR_H
#define MCPTT_PUSHER_ORCHESTRATOR_H

#include "mcptt-pusher-orchestrator-interface.h"
#include "mcptt-pusher.h"

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

namespace ns3
{

namespace psc
{

/**
 * \ingroup mcptt
 *
 * This class is used to orchestrate the behavior of a collection of
 * McpttPusher objects. This class uses a random variable for
 * determining when a PTT event will occur, and another random variable for
 * determining for how long. When pushers are orchestrated by this object
 * each pusher has equal opportunity for being selected go generate a PTT
 * push and release.
 */
class McpttPusherOrchestrator : public McpttPusherOrchestratorInterface
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    /**
     * \brief Creates an instance of the McpttPusherOrchestrator class.
     */
    McpttPusherOrchestrator();
    /**
     * \brief The destructor of the McpttPusherOrchestrator class.
     */
    ~McpttPusherOrchestrator() override;
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
     * \brief Indicates if the orchestrator is active.
     * \return True, if the orchestrator is scheduling pushes and releases.
     */
    virtual bool IsActive() const;
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
     * Deactivates the pusher.
     */
    virtual void DeactivatePusher();
    /**
     * Disposes of Object resources.
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

  private:
    Time m_minIat;                                 //!< The minimum amount of time between consecutive PTT events
    bool m_active;                                   //!< Indicates if the orchestrator is active.
    Ptr<McpttPusher> m_activePusher;                 //!< Currently selected pusher.
    EventId m_nextEvent;                             //!< The next event.
    Ptr<McpttPusher> m_nextPusher;                   //!< The next pusher to be selected.
    std::vector<Ptr<McpttPusher>> m_pushers;         //!< Set of pushers to orchestrate.
    Ptr<RandomVariableStream> m_pttDurationVariable; //!< For switching pushers.
    Ptr<RandomVariableStream> m_pttIatVariable;      //!< PTT interarrival time variable.
    Ptr<UniformRandomVariable> m_selectionVariable;  //!< For selecting pushers.
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_PUSHER_ORCHESTRATOR_H */
