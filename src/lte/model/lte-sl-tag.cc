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

#include "lte-sl-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LteSlSciTag);

TypeId
LteSlSciTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteSlSciTag")
    .SetParent<Tag> ()
    .SetGroupName ("Lte")
    .AddConstructor<LteSlSciTag> ()
    .AddAttribute ("rnti", "The rnti that indicates the UE to which packet belongs",
                   UintegerValue (0),
                   MakeUintegerAccessor (&LteSlSciTag::GetRnti),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("resNo", "The resource number within the control pool",
                   UintegerValue (0),
                   MakeUintegerAccessor (&LteSlSciTag::GetResNo),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("tbSize", "The transport block size",
                   UintegerValue (0),
                   MakeUintegerAccessor (&LteSlSciTag::GetTbSize),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}

TypeId
LteSlSciTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

LteSlSciTag::LteSlSciTag ()
  : m_rnti (0),
  m_resNo (0),
  m_tbSize (0)
{
}
LteSlSciTag::LteSlSciTag (uint16_t rnti, uint16_t resNo, uint32_t tbSize)
  : m_rnti (rnti),
  m_resNo (resNo),
  m_tbSize (tbSize)
{
}

uint32_t
LteSlSciTag::GetSerializedSize (void) const
{
  return 8;
}

void
LteSlSciTag::Serialize (TagBuffer i) const
{
  i.WriteU16 (m_rnti);
  i.WriteU16 (m_resNo);
  i.WriteU32 (m_tbSize);
}

void
LteSlSciTag::Deserialize (TagBuffer i)
{
  m_rnti = i.ReadU16 ();
  m_resNo = i.ReadU16 ();
  m_tbSize = i.ReadU32 ();
}

uint16_t
LteSlSciTag::GetRnti () const
{
  return m_rnti;
}

uint16_t
LteSlSciTag::GetResNo () const
{
  return m_resNo;
}

uint32_t
LteSlSciTag::GetTbSize () const
{
  return m_tbSize;
}

void
LteSlSciTag::Print (std::ostream &os) const
{
  os << "rnti=" << m_rnti << ", resNo=" << (uint16_t) m_resNo << ", tbSize=" << m_tbSize;
}

TypeId
LteSlPc5ContextTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteSlSciTag")
    .SetParent<Tag> ()
    .SetGroupName ("Lte")
    .AddConstructor<LteSlSciTag> ()
    .AddAttribute ("context", "The context value",
                   UintegerValue (0),
                   MakeUintegerAccessor (&LteSlPc5ContextTag::GetContext),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}

TypeId
LteSlPc5ContextTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

LteSlPc5ContextTag::LteSlPc5ContextTag ()
  : m_context (0)
{
}
LteSlPc5ContextTag::LteSlPc5ContextTag (uint32_t tag)
  : m_context (tag)
{
}

uint32_t
LteSlPc5ContextTag::GetSerializedSize (void) const
{
  return 4;
}

void
LteSlPc5ContextTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_context);
}

void
LteSlPc5ContextTag::Deserialize (TagBuffer i)
{
  m_context = (uint8_t) i.ReadU32 ();
}

uint32_t
LteSlPc5ContextTag::GetContext () const
{
  return m_context;
}

void
LteSlPc5ContextTag::Print (std::ostream &os) const
{
  os << "context=" << m_context;
}


} // namespace ns3
