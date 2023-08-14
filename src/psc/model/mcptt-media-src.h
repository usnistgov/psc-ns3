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

#ifndef MCPTT_MEDIA_SRC_H
#define MCPTT_MEDIA_SRC_H

#include <ns3/data-rate.h>
#include <ns3/event-id.h>
#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/type-id.h>
#include <ns3/uinteger.h>

#include "mcptt-media-sink.h"

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class is cable of generating MCPTT media messages that will then be
 * requested to be sent by a McpttMediaSink for sending. This class was
 * created to provide a simple traffic model of MCPTT media for an MCPTT
 * application.
 */
class McpttMediaSrc : public Object
{
public:
  /**
   * \brief Gets the identifier of the McpttMediaSrc class.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Creates an instance of the McpttMediaSrc class.
   */
  McpttMediaSrc (void);
  /**
   * \brief The destructor of the McpttMediaSrc class.
   */
  virtual ~McpttMediaSrc (void);
  /**
   * \brief Indicates whether or not the requester has a sink.
   * \return True, if the requester has a sink or false otherwise.
   */
  virtual bool HasSink (void);
  /**
   * Indicates whether or not the requester is making request.
   * \returns True, if the requester is making requests.
   */
  virtual bool IsMakingReq (void) const;
  /**
   * \brief Starts making send requests.
   */
  virtual void StartMakingReq (void);
  /**
   * \brief Stops making send requests.
   */
  virtual void StopMakingReq (void);

protected:
  /**
   * \brief Adds the given number of bytes to the total count.
   * \param numBytes The number of bytes to add to the total.
   */
  virtual void AddToTotalBytes (uint16_t numBytes);
  /**
   * \brief Cancels the current request event.
   */
  virtual void CancelRequest (void);
  /**
   * \brief Disposes of all dynamically allocated resources.
   */
  virtual void DoDispose (void);
  /**
   * \brief Makes a request to send the data.
   */
  virtual void MakeRequest (void);
  /**
   * Schedules the next time at which MakeRequest () should be executed.
   */
  virtual void ScheduleNextReq (void);

private:
  uint16_t m_bytes; //!< The number of data bytes that should be sent.
  DataRate m_dataRate; //!< The rate at which data should be sent.
  Time m_lastReq; //!< The last time at which a request was made.
  EventId m_reqEvent; //!< The event used used for scheduling the requests
  /* NOTE:
   * Using a raw pointer here because this type represents
   * an interface that does not inheret from any of the ns3
   * base classes. Therefore, this class should not allocate
   * or dispose of the memory referenced by this pointer, and
   * it is up to the user to provide a valid object. This
   * pointer's sole purpose is to provide access to the
   * object that the user has provided.
   */
  McpttMediaSink* m_sink; //!< A pointer to the sink.
  bool m_started; //!< Indicates if the requester has started making request.
  Time m_startTime; //!< Start time of talk spurt
  uint16_t m_totalBytes; //!< The total number of bytes that have been sent.
  uint16_t m_nextSeqNum; //!< Next RTP sequence number value

public:
  /**
   * \brief Gets a pointer to the sink.
   * \return A pointer to the sink.
   */
  virtual McpttMediaSink* GetSink (void) const;
  /**
   * \brief Gets the total number of bytes that have been requested to send.
   * \return The total number of bytes.
   */
  virtual uint16_t GetTotalBytes (void) const;
  /**
   * \brief Sets the pointer to the sink.
   * \param sink A pointer to the sink.
   */
  virtual void SetSink (McpttMediaSink* const& sink);

protected:
  /**
   * \brief Gets the last time at which a request was made.
   * \return The last time.
   */
  virtual Time GetLastReq (void) const;
  /**
   * \brief Sets the last time at which a request was made.
   * \param lastReq The time.
   */
  virtual void SetLastReq (const Time& lastReq);
  /**
   * \brief Gets the event that describes when a send request will be made.
   * \return The event.
   */
  virtual EventId GetReqEvent (void) const;
  /**
   * \brief Sets the event that describes when a send request will be made.
   * \param reqEvent The event.
   */
  virtual void SetReqEvent (const EventId& reqEvent);
  /**
   * \brief Sets the total number of bytes that have been requested to send.
   * \param totalBytes The total number of bytes.
   */
  virtual void SetTotalBytes (uint16_t totalBytes);
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_MEDIA_SRC_H */

