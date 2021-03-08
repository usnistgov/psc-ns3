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

#ifndef MCPTT_MEDIA_MSG_H
#define MCPTT_MEDIA_MSG_H

#include <ns3/type-id.h>
#include <ns3/nstime.h>

#include "mcptt-msg.h"
#include "mcptt-rtp-header.h"

namespace ns3 {

namespace psc {

class McpttFloorMsgSink;
class McpttCallMachinePrivate;

/**
 * \ingroup mcptt
 *
 * A class used to represent an RTP media message described in TS 24.380 for
 * floor control.
 *
 * The McpttMediaMsg is an ns3::Header object that may be serialized in
 * the usual way.  It starts with an instance of McpttRtpHeader, followed
 * by a notional payload (of size GetPayloadSize ()).  The actual bytes
 * encoded in the buffer following the McpttRtpHeader are the size of the
 * payload field (encoded as a 16-bit unsigned integer), and a 32-bit
 * timestamp encoding the start of talk spurt time.  Any further
 * notional payload bytes are encoded as zero.
 *
 * The payload size of the media message must be >= 6 bytes;
 */
class McpttMediaMsg : public McpttMsg
{
public:
  /**
   * Gets the type ID of the McpttMediaMsg class.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
   * Creates an instance of the McpttMediaMsg class.
   */
  McpttMediaMsg (void);
  /**
   * Creates an instance of the McpttMediaMsg class.
   * \param header The header of the message.
   */
  McpttMediaMsg (const McpttRtpHeader& header);
  /**
   * Creates an instance of the McpttMediaMsg class.
   * \param payloadSize The number of bytes to include in the payload.
   */
  McpttMediaMsg (uint16_t payloadSize);
  /**
   * Creates an instance of the McpttMediaMsg class.
   * \param header The header of the message.
   * \param payloadSize The number of bytes to include in the payload.
   */
  McpttMediaMsg (const McpttRtpHeader& header, uint16_t payloadSize);
  /**
   * Creates an instance of the McpttMediaMsg class.
   * \param header The header of the message.
   * \param payloadSize The number of bytes to include in the payload.
   * \param talkSpurtStart Talk spurt start time
   */
  McpttMediaMsg (const McpttRtpHeader& header, uint16_t payloadSize, Time talkSpurtStart);
  /**
   * The destructor of the McpttMediaMsg class.
   */
  virtual ~McpttMediaMsg (void);
  /**
   * Reads the contents of the message from the byte stream.
   * \param start The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);
  /**
   * Gets the type ID of this McpttFloorMsg instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
  /**
   * Gets the SSRC of this message.
   * \returns The SSRC.
   */
  virtual uint32_t GetSsrc (void) const;
  /**
   * Gets the size (in bytes) of the message when it is serialized.
   * \returns The size (in bytes).
   */
  virtual uint32_t GetSerializedSize (void) const;
  /**
   * Prints the contents of the header to the given output stream.
   * \param os The output stream to write to.
   */
  virtual void Print (std::ostream& os) const;
  /**
   * Writes the contents of the message to the byte stream.
   * \param start The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator start) const;
  /**
   * Sets the SSRC of the media message.
   * \param ssrc The SSRC.
   */
  virtual void SetSsrc (const uint32_t ssrc);
  /**
   * Visits the given privat call machine.
   * \param callMachine The private call machine to visit.
   */
  virtual void Visit (McpttCallMachinePrivate& callMachine) const;
  /**
   * Visits the given floor machine.
   * \param floorMachine The floor machine to visit.
   */
  virtual void Visit (McpttFloorMsgSink& floorMachine) const;

private:
  McpttRtpHeader m_header; //!< The header of the message.
  uint16_t m_payloadSize; //!< The size (in bytes) of the payload of the message.
  Time m_talkSpurtStart; //!< Start of talk spurt

public:
  /**
   * Gets the header of the message.
   * \returns The header of the message.
   */
  virtual McpttRtpHeader GetHeader (void) const;
  /**
   * Gets the size (in bytes) of the payload.
   * \returns The size (in bytes).
   */
  virtual uint16_t GetPayloadSize (void) const;
  /**
   * Gets the time of the talk spurt start, if set (will return zero if unset)
   * \returns The stored value of the talk spurt start time
   */
  virtual Time GetTalkSpurtStart (void) const;
  /**
   * Sets the header of the message.
   * \param header The header of the message.
   */
  virtual void SetHeader (const McpttRtpHeader& header);
  /**
   * Sets the size (in bytes) of the payload.
   * \param payloadSize The size (in bytes).
   *
   * Payload size must be at least two bytes.
   */
  virtual void SetPayloadSize (uint16_t payloadSize);
  /**
   * Sets the time of the talk spurt start
   * \param talkSpurtStart Talk spurt start time
   */
  virtual void SetTalkSpurtStart (Time talkSpurtStart);
};

} // namespace psc
} // namespace ns3

#endif // MCPTT_MEDIA_H

