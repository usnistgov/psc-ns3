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

#ifndef NIST_RTP_HEADER_H
#define NIST_RTP_HEADER_H

#include <iostream>

#include <ns3/buffer.h>
#include <ns3/header.h>
#include <ns3/type-id.h>

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * A class used to represent an RTP header which is necessary for MCPTT media
 * messages.
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | V |P|X|   CC  |M|      PT     |        Sequence Number        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                           Timestamp                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                              SSRC                             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                         CSRC [0...15]                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
class McpttRtpHeader : public Header
{
public:
  /**
   * Gets the type ID of the McpttRtpHeader.
   * \returns The type ID.
   */
  static TypeId GetTypeId (void);
  /**
  * Creates an instance of the McpttRtpHeader class.
  */
  McpttRtpHeader (void);
  /**
   * Creates an instance of the McpttRtpHeader class.
   * \param ssrc They synchronization source (SSRC).
   */
  McpttRtpHeader (uint32_t ssrc);
  /**
   * Creates an instance of the McpttRtpHeader class.
   * \param ssrc The synchronization source (SSRC).
   * \param payloadType The payload type (PT).
   */
  McpttRtpHeader (uint32_t ssrc, uint8_t payloadType);
  /**
   * The destructor of the McpttRtpHeader class.
   */
  virtual ~McpttRtpHeader (void);
  /**
   * Adds a CSRC.
   * \param csrc The CSRC to add.
   * \returns True, if the CSRC was added.
   */
  virtual bool AddCsrc (uint32_t csrc);
  /**
   * Removes all CSRCs.
   */
  virtual void ClearCsrcs (void);
  /**
   * Reads the contents of the message from the byte stream.
   * \param start The buffer to read from.
   * \returns The number of bytes that were read.
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);
  /**
   * Gets the type ID of this McpttRtpHeader instance.
   * \returns The type ID.
   */
  virtual TypeId GetInstanceTypeId (void) const;
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
   * Removes a CSRC.
   * \param csrc The CSRC to remove.
   * \returns True, if the CSRC was found and removed.
   */
  virtual bool RemoveCsrc (uint32_t csrc);
  /**
   * Writes the contents of the message to the byte stream.
   * \param start The byte stream to write to.
   */
  virtual void Serialize (Buffer::Iterator start) const;

protected:

private:
  uint8_t m_csrcCount; //!< The number of CSRC identifiers included in the header (CC).
  std::vector<uint32_t> m_csrcs; //!< The collection of contributing sources (CSRC).
  uint8_t m_extension; //!< Flag to indicate if header is followed by exactly one header extension (X).
  uint8_t m_marker; //!< A marker bit (M).
  uint8_t m_padding; //!< The padding bit value (P).
  uint8_t m_payloadType; //!< The payload type (PT).
  uint16_t m_seqNum; //!< The RTP data packet sequence number (Sequence Number).
  uint32_t m_ssrc; //!< The synchronization source (SSRC).
  uint32_t m_timestamp; //!< The sampling instant of the first octent in the RTP packet (Timestamp).
  uint8_t m_version; //!< The version of the RTCP (V).

protected:
  /**
   * Sets the number of CSRC identifiers (CC).
   * \param csrcCount The number of CSRC identifiers (CC).
   */
  virtual void SetCsrcCount (uint8_t csrcCount);
  /**
   * Sets the collection of contributing sources (CSRC).
   * \param csrcs The collection of contributing sources (CSRC).
   */
  virtual void SetCsrcs (const std::vector<uint32_t>  csrcs);

public:
  /**
   * Gets the number of CSRC identifiers (CC).
   * \returns The number of CSRC identifiers (CC).
   */
  virtual uint8_t GetCsrcCount (void) const;
  /**
   * Gets the collection of contributing sources (CSRC).
   * \returns The collection of contributing sources (CSRC).
   */
  virtual std::vector<uint32_t> GetCsrcs (void) const;
  /**
   * Gets the extension flag (X).
   * \returns The extension flag (X).
   */
  virtual uint8_t GetExtension (void) const;
  /**
   * Gets the marker bit (M).
   * \returns The marker bit (M).
   */
  virtual uint8_t GetMarker (void) const;
  /**
   * Gets the padding bit value (P).
   * \returns The padding bit value.
   */
  virtual uint8_t GetPadding (void) const;
  /**
   * Gets the value of the payload type (PT) value.
   * \returns The payload type (PT) value.
   */
  virtual uint8_t GetPayloadType (void) const;
  /**
   * Gets the sequence number (Sequence Number).
   * \returns The sequence number (Sequence Number).
   */
  virtual uint16_t GetSeqNum (void) const;
  /**
   * Gets the synchronization source (SSRC).
   * \returns The synchronization source.
   */
  virtual uint32_t GetSsrc (void) const;
  /**
   * Gets the timestamp (Timestamp).
   * \returns The timestamp (Timestamp).
   */
  virtual uint32_t GetTimestamp (void) const;
  /**
   * Gets the version number (V).
   * \returns The version number.
   */
  virtual uint8_t GetVersion (void) const;
  /**
   * Sets the extension flag (X).
   * \param extension The extension flag.
   */
  virtual void SetExtension (uint8_t extension);
  /**
   * Sets the marker bit (M).
   * \param marker The marker bit (M).
   */
  virtual void SetMarker (uint8_t marker);
  /**
   * Sets the padding bit value (P).
   * \param padding The paddinb bit value.
   */
  virtual void SetPadding (uint8_t padding);
  /**
   * Sets the value of the payload type (PT).
   * \param payloadType The payload type (PT) value.
   */
  virtual void SetPayloadType (uint8_t payloadType);
  /**
   * Sets the sequence number (Sequence Number).
   * \param seqNum The sequence number.
   */
  virtual void SetSeqNum (uint16_t seqNum);
  /**
   * Sets the synchronization source (SSRC).
   * \param ssrc The synchronization source.
   */
  virtual void SetSsrc (uint32_t ssrc);
  /**
   * Sets the timestamp (Timestamp).
   * \param timestamp The timestamp (Timestamp).
   */
  virtual void SetTimestamp (uint32_t timestamp);
  /**
   * Sets the value of the version number (V).
   * \param version The version number.
   */
  virtual void SetVersion (uint8_t version);
};

} // namepace psc
} // namepace ns3

#endif // NIST_RTP_HEADER_H

