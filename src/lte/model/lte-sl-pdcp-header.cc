/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#include "ns3/log.h"

#include "ns3/lte-sl-pdcp-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlPdcpHeader");

NS_OBJECT_ENSURE_REGISTERED (LteSlPdcpHeader);

LteSlPdcpHeader::LteSlPdcpHeader ()
  : m_sduType (0xff),
    m_pgkIndex (0x0),
    m_secIdentity (0x0),
    m_sequenceNumber (0xfffa)
{
}

LteSlPdcpHeader::~LteSlPdcpHeader ()
{
  m_sduType = 0xff;
  m_pgkIndex = 0x0;
  m_secIdentity = 0x0;
  m_sequenceNumber = 0xfffb;
}

void
LteSlPdcpHeader::SetSduType (uint8_t sduType)
{
  m_sduType = sduType & 0x07;
}

void
LteSlPdcpHeader::SetPgkIndex (uint8_t pgkIndex)
{
  m_pgkIndex = pgkIndex & 0x1F;
}

void
LteSlPdcpHeader::SetSecurityIdentity (uint16_t secIdentity)
{
  m_secIdentity = secIdentity;
}
  
void
LteSlPdcpHeader::SetSequenceNumber (uint16_t sequenceNumber)
{
  m_sequenceNumber = sequenceNumber;
}

uint8_t
LteSlPdcpHeader::GetSduType () const
{
  return m_sduType;
}

uint8_t
LteSlPdcpHeader::GetPgkIndex () const
{
  return m_pgkIndex;
}

uint16_t
LteSlPdcpHeader::GetSecurityIdentity () const
{
  return m_secIdentity;
}
  
uint16_t
LteSlPdcpHeader::GetSequenceNumber () const
{
  return m_sequenceNumber;
}


TypeId
LteSlPdcpHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteSlPdcpHeader")
    .SetParent<Header> ()
    .AddConstructor<LteSlPdcpHeader> ()
  ;
  return tid;
}

TypeId
LteSlPdcpHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void LteSlPdcpHeader::Print (std::ostream &os)  const
{
  os << "SDU type=" << (uint16_t)m_sduType;
  os << " PGK Index=" << (uint16_t)m_pgkIndex;
  os << " PTK/KD_SESS=" << m_secIdentity;
  os << " SN=" << m_sequenceNumber;
}

uint32_t LteSlPdcpHeader::GetSerializedSize (void) const
{
  return 5;
}

void LteSlPdcpHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 ( (m_sduType << 5) | m_pgkIndex );
  i.WriteU8 ( (m_secIdentity & 0xFF00) >> 8);
  i.WriteU8 ( (m_secIdentity & 0x00FF) );
  i.WriteU8 ( (m_sequenceNumber & 0xFF00) >> 8);
  i.WriteU8 ( (m_sequenceNumber & 0x00FF) );
}

uint32_t LteSlPdcpHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint8_t bytes[5];

  for (uint8_t index = 0; index < 5; index++)
    {
      bytes[index] = i.ReadU8 ();
    }

  m_sduType = (bytes[0] & 0xE0) >> 5;
  m_pgkIndex = bytes[0] & 0x1F;
  m_secIdentity = (bytes[1] << 8) | bytes[2];
  m_sequenceNumber = (bytes[3] << 8) | bytes[4];

  return GetSerializedSize ();
}

}; // namespace ns3
