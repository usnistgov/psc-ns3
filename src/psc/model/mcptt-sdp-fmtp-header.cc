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

#include "mcptt-sdp-fmtp-header.h"

namespace ns3 {

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (McpttSdpFmtpHeader);


McpttSdpFmtpHeader::McpttSdpFmtpHeader ()
{}

McpttSdpFmtpHeader::~McpttSdpFmtpHeader ()
{}

TypeId
McpttSdpFmtpHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::psc::McpttSdpFmtpHeader")
    .SetParent<Header> ()
    .SetGroupName ("Mcptt")
    .AddConstructor<McpttSdpFmtpHeader> ()
  ;
  return tid;
}

void
McpttSdpFmtpHeader::SetMcQueueing (bool mcQueueing)
{
  m_mcQueueing = mcQueueing;
}

bool
McpttSdpFmtpHeader::GetMcQueueing (void) const
{
  return m_mcQueueing;
}

void
McpttSdpFmtpHeader::SetMcPriority (uint8_t mcPriority)
{
  m_mcPriority = mcPriority;
}

uint8_t
McpttSdpFmtpHeader::GetMcPriority (void) const
{
  return m_mcPriority;
}

void
McpttSdpFmtpHeader::SetMcGranted (bool mcGranted)
{
  m_mcGranted = mcGranted;
}

bool
McpttSdpFmtpHeader::GetMcGranted (void) const
{
  return m_mcGranted;
}

void
McpttSdpFmtpHeader::SetMcImplicitRequest (bool mcImplicitRequest)
{
  m_mcImplicitRequest = mcImplicitRequest;
}

bool
McpttSdpFmtpHeader::GetMcImplicitRequest (void) const
{
  return m_mcImplicitRequest;
}

TypeId
McpttSdpFmtpHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
McpttSdpFmtpHeader::Print (std::ostream &os) const
{
  os << "McQueueing: " << (m_mcQueueing ? "true " : "false ");
  os << "McPriority: " << +m_mcPriority << " ";
  os << "McGranted: " << (m_mcGranted ? "true " : "false ");
  os << "McImplicitRequest: " << (m_mcImplicitRequest ? "true " : "false ");
}

uint32_t
McpttSdpFmtpHeader::GetSerializedSize (void) const
{
  return 1 + 1 + 1 + 1;
}

void
McpttSdpFmtpHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (m_mcQueueing);
  i.WriteU8 (m_mcPriority);
  i.WriteU8 (m_mcGranted);
  i.WriteU8 (m_mcImplicitRequest);
}

uint32_t
McpttSdpFmtpHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_mcQueueing = i.ReadU8 ();
  m_mcPriority = i.ReadU8 ();
  m_mcGranted = i.ReadU8 ();
  m_mcImplicitRequest = i.ReadU8 ();
  return GetSerializedSize ();
}

} // namespace psc
} // namespace ns3
