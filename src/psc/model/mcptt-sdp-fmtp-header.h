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

#ifndef MCPTT_SDP_FMTP_HEADER_H
#define MCPTT_SDP_FMTP_HEADER_H

#include <ns3/header.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

namespace ns3 {

namespace psc {

/**
 * \ingroup mcptt
 *
 * This class implements a simplified SDP header to cover the ftmp media
 * attribute used in floor control (TS 24.380, Section 12).
 *
 * SDP is a text-based protocol; this header is just an abstraction for the
 * attributes of relevance to MCPTT.  This header may be replaced by a fuller
 * SDP implementation in the future.
 */
class McpttSdpFmtpHeader : public Header
{
public:
  /**
   * \brief Construct a null header
   */
  McpttSdpFmtpHeader ();
  /**
   * \brief Destructor
   */
  virtual ~McpttSdpFmtpHeader ();
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  // Below eight methods need doxygen
  void SetMcQueueing (bool mcQueueing);
  bool GetMcQueueing (void) const;

  void SetMcPriority (uint8_t mcPriority);
  uint8_t GetMcPriority (void) const;

  void SetMcGranted (bool mcGranted);
  bool GetMcGranted (void) const;

  void SetMcImplicitRequest (bool mcImplicitRequest);
  bool GetMcImplicitRequest (void) const;

  // Documented in ns3::Header class
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  bool m_mcQueueing {false};
  uint8_t m_mcPriority {1};
  bool m_mcGranted {false};
  bool m_mcImplicitRequest {false};
};

} // namespace psc
} // namespace ns3

#endif /* MCPTT_SDP_FMTP_HEADER_H */
