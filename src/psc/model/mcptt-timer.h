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

#ifndef MCPTT_TIMER_H
#define MCPTT_TIMER_H

#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/timer.h>
#include <ns3/ptr.h>

#include "mcptt-entity-id.h"

namespace ns3 {
namespace psc {

/**
 * \ingroup mcptt
 *
 * This class is a wrapper around the ns3::Timer to be used more like what is
 * requested of timers in 3GPP standards TS 24.379 and TS 24.380. This
 * includes functions like "Start", "Stop", and "Restart". It also provides
 * the logic to make the timers behave more like what is expected of timers in
 * the standard.
 */
class McpttTimer : public Object
{
  friend std::ostream& operator<< (std::ostream& os, const McpttTimer& timer);

public:
  /**
   * Gets the type ID of the McpttTimer class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttTimer class.
   */
  McpttTimer (void);
  /**
   * Creates an instance of the McpttTimer class.
   * \param id The ID.
   */
  McpttTimer (const McpttEntityId& id);
  /**
   * The destructor of the McpttTimer class.
   */
  virtual ~McpttTimer (void);
  /**
   * Gets the amount of time that the timer will run before expiring.
   * \returns The delay.
   */
  virtual Time GetDelay (void) const;
  /**
   * Gets the type ID of the most derived instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the amount of time left before the timer expires.
   * \returns The amount of time left.
   */
  virtual Time GetTimeLeft (void) const;
  /**
   * Indicates whether or not the timer has expired.
   * \returns True, if the timer has expired.
   */
  virtual bool IsExpired (void) const;
  /**
   * Indicates whether or not the timer is running.
   * \returns True, if the timer is running.
   */
  virtual bool IsRunning (void) const;
  /**
   * Links the timer experiation to a function.
   * \tparam MEM_PTR The type of the function pointer
   * \tparam OBJ_PTR The type of the object pointer.
   * \param memPtr The member function pointer.
   * \param objPtr The pointer to the object.
   */
  template <typename MEM_PTR, typename OBJ_PTR>
  void Link (const MEM_PTR& memPtr, const OBJ_PTR& objPtr);
  /**
   * Prints the timer to the given output stream.
   * \param os The output stream to print to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Restarts the timer.
   */
  virtual void Restart (void);
  /**
   * Sets the argument to pass to the function called when the timer expires.
   * \tparam T The type of the argument ot pass to the function.
   * \param arg The argument to pass.
   */
  template<typename T>
  void SetArgument (const T& arg);
  /**
   * Sets the delay.
   * \param delay The amount of time the timer should run before expiring.
   */
  virtual void SetDelay (const Time& delay);
  /**
   * Starts the timer.
   */
  virtual void Start (void);
  /**
   * Starts the timer with the given delay but does not preserve the delay.
   * \param delay The amont of time the timer should run before expiring.
   */
  virtual void StartWith (const Time& delay);
  /**
   * Stops the timer.
   */
  virtual void Stop (void);

protected:
  /**
   * Disposes of this McpttTimer instance.
   */
  virtual void DoDispose (void);

private:
  McpttEntityId m_id; //!< The ID of the timer.
  Timer* m_rawTimer; //!< The raw timer.

public:
  /**
   * Gets the ID of the timer.
   * \returns The ID.
   */
  virtual McpttEntityId GetId (void) const;
  /**
   * Gets the raw timer.
   * \returns The raw timer.
   */
  virtual Timer* GetRawTimer (void) const;
  /**
   * Sets the ID of the timer.
   * \param id The ID.
   */
  virtual void SetId (const McpttEntityId& id);
  /**
   * Sets the raw timer.
   * \param rawTimer The raw timer.
   */
  virtual void SetRawTimer (Timer* const& rawTimer);
};
/**
 * Prints the given timer to the given output stream.
 * \param os The output stream to print to.
 * \param timer The timer to print to the output stream.
 * \returns The output stream that the timer was printed to.
 */
std::ostream& operator<< (std::ostream& os, const McpttTimer& timer);

} // namespace psc
} // namespace ns3

/********************************************************************
 *  Implementation of the templates declared above.
 ********************************************************************/

namespace ns3 {
namespace psc {

template <typename MEM_PTR, typename OBJ_PTR>
void
McpttTimer::Link (const MEM_PTR& memPtr, const OBJ_PTR& objPtr)
{
  Timer* rawTimer = new Timer (Timer::REMOVE_ON_DESTROY);
  rawTimer->SetFunction (memPtr, objPtr);

  if (GetRawTimer () != 0)
    {
      delete GetRawTimer ();
    }

  SetRawTimer (rawTimer);
}

template <typename T>
void
McpttTimer::SetArgument (const T& arg)
{
  Timer* rawTimer = GetRawTimer ();

  rawTimer->SetArguments (arg);
}

} // namespace psc
} // namespace ns3

#endif /* MCPTT_TIMER_H */
