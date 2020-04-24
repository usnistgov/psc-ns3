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

#include "lte-sl-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlHeader");

NS_OBJECT_ENSURE_REGISTERED (LteSlDiscHeader);

LteSlDiscHeader::LteSlDiscHeader ()
  : m_discoveryMsgType (0),
  m_discoveryType (0),
  m_discoveryContentType (0),
  m_discoveryModel (0),
  m_appCode (0),
  m_relayServiceCode (0),
  m_info (0),
  m_relayUeId (0),
  m_statusIndicator (0),
  m_group (0),
  m_groupInfo (0),
  m_mic (0),
  m_utcBasedCounter (0)
{


}

LteSlDiscHeader::~LteSlDiscHeader ()
{
  m_discoveryMsgType = 0;
}

void
LteSlDiscHeader::SetOpenDiscoveryAnnounceParameters (uint32_t appCode)
{
  //DISC_OPEN_ANNOUNCEMENT
  m_discoveryType = 1;
  m_discoveryContentType = 0;
  m_discoveryModel = 1;
  m_discoveryMsgType = BuildDiscoveryMsgType ();
  m_appCode = appCode;
}
void
LteSlDiscHeader::SetRestrictedDiscoveryQueryParameters (uint32_t appCode)
{
  //DISC_RESTRICTED_REQUEST;
  m_discoveryType = 2;
  m_discoveryContentType = 1;
  m_discoveryModel = 2;
  m_discoveryMsgType = BuildDiscoveryMsgType ();
  m_appCode = appCode;
}

void
LteSlDiscHeader::SetRestrictedDiscoveryResponseParameters (uint32_t appCode)
{
  //DISC_RESTRICTED_RESPONSE;
  m_discoveryType = 2;
  m_discoveryContentType = 0;
  m_discoveryModel = 2;
  m_discoveryMsgType = BuildDiscoveryMsgType ();
  m_appCode = appCode;
}

void
LteSlDiscHeader::SetRelayAnnouncementParameters (uint32_t serviceCode, uint64_t announcerInfo, uint32_t relayUeId, uint32_t status)
{
  //DISC_RELAY_ANNOUNCEMENT;
  m_discoveryType = 2;
  m_discoveryContentType = 4;
  m_discoveryModel = 1;
  m_discoveryMsgType = BuildDiscoveryMsgType ();
  m_relayServiceCode = serviceCode;
  m_info = announcerInfo;
  m_relayUeId = relayUeId;
  m_statusIndicator = status;
}
void
LteSlDiscHeader::SetRelaySoliciationParameters (uint32_t serviceCode, uint64_t discovererInfo, uint32_t relayUeId)
{
  //DISC_RELAY_SOLICITATION;
  m_discoveryType = 2;
  m_discoveryContentType = 5;
  m_discoveryModel = 2;
  m_discoveryMsgType = BuildDiscoveryMsgType ();
  m_relayServiceCode = serviceCode;
  m_info = discovererInfo;
  m_relayUeId = relayUeId;
}
void
LteSlDiscHeader::SetRelayResponseParameters (uint32_t serviceCode, uint64_t discovereeInfo, uint32_t relayUeId, uint32_t status)
{
  //DISC_RELAY_RESPONSE;
  m_discoveryType = 2;
  m_discoveryContentType = 4;
  m_discoveryModel = 2;
  m_discoveryMsgType = BuildDiscoveryMsgType ();
  m_relayServiceCode = serviceCode;
  m_info = discovereeInfo;
  m_relayUeId = relayUeId;
  m_statusIndicator = status;
}

uint8_t
LteSlDiscHeader::BuildDiscoveryMsgType ()
{
  uint8_t msgType = ((m_discoveryType & 0x03) << 6) | ((m_discoveryContentType & 0x0F) << 2) | (m_discoveryModel & 0x03);

  NS_ABORT_MSG_IF (msgType != DISC_OPEN_ANNOUNCEMENT && msgType != DISC_RESTRICTED_QUERY &&
                   msgType != DISC_RESTRICTED_RESPONSE && msgType != DISC_RELAY_ANNOUNCEMENT &&
                   msgType != DISC_RELAY_SOLICITATION && msgType != DISC_RELAY_RESPONSE,
                   "unknown discovery message type " << (uint16_t) msgType);
  return msgType;
}



void
LteSlDiscHeader::SetMic (uint32_t mic)
{
  m_mic = mic;
}

void
LteSlDiscHeader::SetUtcBaseCounter (uint8_t counter)
{
  m_utcBasedCounter = counter;
}

uint32_t
LteSlDiscHeader::GetApplicationCode () const
{
  return m_appCode;
}
uint32_t
LteSlDiscHeader::GetRelayServiceCode () const
{
  return m_relayServiceCode;
}
uint64_t
LteSlDiscHeader::GetInfo () const
{
  return m_info;
}
uint32_t
LteSlDiscHeader::GetRelayUeId () const
{
  return m_relayUeId;
}
uint8_t
LteSlDiscHeader::GetStatusIndicator () const
{
  return m_statusIndicator;
}

uint8_t
LteSlDiscHeader::GetURDSComposition () const
{
  if (m_relayUeId != 0)
    {
      return 1;
    }
  return 0;
}

uint32_t
LteSlDiscHeader::GetGroup () const
{
  return m_group;
}
uint32_t
LteSlDiscHeader::GetGroupInfo () const
{
  return m_groupInfo;
}


uint32_t
LteSlDiscHeader::GetMic () const
{
  return m_mic;
}

uint8_t
LteSlDiscHeader::GetUtcBaseCounter () const
{
  return m_utcBasedCounter;
}


uint8_t
LteSlDiscHeader::GetDiscoveryMsgType () const
{
  return m_discoveryMsgType;
}

uint8_t
LteSlDiscHeader::GetDiscoveryType () const
{
  return m_discoveryType;
}

uint8_t
LteSlDiscHeader::GetDiscoveryContentType () const
{
  return m_discoveryContentType;
}

uint8_t
LteSlDiscHeader::GetDiscoveryModel () const
{
  return m_discoveryModel;
}


TypeId
LteSlDiscHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteSlDiscHeader")
    .SetParent<Header> ()
    .AddConstructor<LteSlDiscHeader> ()
  ;
  return tid;
}

TypeId
LteSlDiscHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
LteSlDiscHeader::Print (std::ostream &os)  const
{
  //todo
}

uint32_t
LteSlDiscHeader::GetSerializedSize (void) const
{
  return 29;
}

void
LteSlDiscHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (m_discoveryMsgType);

  uint8_t padding = 0;

  switch (m_discoveryMsgType)
    {
    case DISC_OPEN_ANNOUNCEMENT:
    case DISC_RESTRICTED_QUERY:
    case DISC_RESTRICTED_RESPONSE:  
      i.WriteU32 (m_appCode);
      i.WriteU8 (padding, 19);
      break;
    case DISC_RELAY_ANNOUNCEMENT:
    case DISC_RELAY_RESPONSE:
      i.WriteU16 (m_relayServiceCode & 0xFFFF);
      i.WriteU8 ((m_relayServiceCode >> 16) & 0xFF);
      i.WriteU32 (m_info & 0xFFFFFFFF);
      i.WriteU16 ((m_info >> 32) & 0xFFFF);
      i.WriteU16 (m_relayUeId & 0xFFFF);
      i.WriteU8 ((m_relayUeId >> 16) & 0xFF);
      i.WriteU8 (m_statusIndicator);
      i.WriteU8 ( padding, 10);
      break;
    case DISC_RELAY_SOLICITATION:
      i.WriteU16 (m_relayServiceCode & 0xFFFF);
      i.WriteU8 ((m_relayServiceCode >> 16) & 0xFF);
      i.WriteU32 (m_info & 0xFFFFFFFF);
      i.WriteU16 ((m_info >> 32) & 0xFFFF);
      if (m_relayUeId == 0)
        {
          i.WriteU8 (0);
          i.WriteU8 ( padding, 13);
        }
      else
        {
          i.WriteU8 (1);
          i.WriteU16 (m_relayUeId & 0xFFFF);
          i.WriteU8 ((m_relayUeId >> 16) & 0xFF);
          i.WriteU8 ( padding, 10);
        }

    default:
      break;
    }

  i.WriteU32 (m_mic);
  i.WriteU8 (m_utcBasedCounter);
}

uint32_t
LteSlDiscHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_discoveryMsgType = i.ReadU8 ();

  m_discoveryType = (m_discoveryMsgType >> 6) & 0x03;
  m_discoveryContentType = (m_discoveryMsgType >> 2) & 0x0F;
  m_discoveryModel = m_discoveryMsgType & 0x03;

  uint8_t padding[19];
  uint64_t tmp;
  switch (m_discoveryMsgType)
    {
    case DISC_OPEN_ANNOUNCEMENT:
    case DISC_RESTRICTED_QUERY:
    case DISC_RESTRICTED_RESPONSE:
      m_appCode = i.ReadU32 ();
      i.Read (padding, 19);
      break;
    case DISC_RELAY_ANNOUNCEMENT:
    case DISC_RELAY_RESPONSE:
      m_relayServiceCode = i.ReadU16 ();
      m_relayServiceCode += i.ReadU8 () << 16;
      m_info = i.ReadU32 ();
      tmp = i.ReadU16 ();
      m_info += tmp << 32;
      m_relayUeId = i.ReadU16 ();
      m_relayUeId += i.ReadU8 () << 16;
      m_statusIndicator = i.ReadU8 ();
      i.Read (padding, 10);
      break;
    case DISC_RELAY_SOLICITATION:
      m_relayServiceCode = i.ReadU16 ();
      m_relayServiceCode += i.ReadU8 () << 16;
      m_info = i.ReadU32 ();
      tmp = i.ReadU16 ();
      m_info += tmp << 32;

      if (i.ReadU8 () == 0)
        {
          i.Read (padding, 13);
        }
      else
        {
          m_relayUeId = i.ReadU16 ();
          m_relayUeId += i.ReadU8 () << 16;
          i.Read (padding, 10);
        }

    default:
      break;
    }


  m_mic = i.ReadU32 ();
  m_utcBasedCounter = i.ReadU8 ();

  return GetSerializedSize ();
}

NS_OBJECT_ENSURE_REGISTERED (LteSlSciHeader);

LteSlSciHeader::LteSlSciHeader ()
  : m_hopping (false),
  m_rbStart (0),
  m_rbLen (0),
  m_hoppingInfo (0),
  m_trp (0),
  m_mcs (0),
  //m_timing (0),
  m_groupDstId (0)
{

}

LteSlSciHeader::~LteSlSciHeader ()
{

}

void LteSlSciHeader::SetSciFormat0Params (bool hopping, uint8_t rbStart, uint8_t rbLen, uint8_t hoppingInfo, uint8_t trp, uint8_t mcs, uint8_t groupId)
{
  m_hopping = hopping;
  m_rbStart = rbStart;
  m_rbLen = rbLen;
  m_hoppingInfo = hoppingInfo;
  m_trp = trp;
  m_mcs = mcs;
  m_groupDstId = groupId;
}

bool LteSlSciHeader::IsHopping () const
{
  return m_hopping;
}
uint8_t LteSlSciHeader::GetRbStart () const
{
  return m_rbStart;
}
uint8_t LteSlSciHeader::GetRbLen () const
{
  return m_rbLen;
}
uint8_t LteSlSciHeader::GetHoppingInfo () const
{
  return m_hoppingInfo;
}
uint8_t LteSlSciHeader::GetTrp () const
{
  return m_trp;
}
uint8_t LteSlSciHeader::GetMcs () const
{
  return m_mcs;
}
uint8_t LteSlSciHeader::GetGroupDstId () const
{
  return m_groupDstId;
}

TypeId
LteSlSciHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteSlSciHeader")
    .SetParent<Header> ()
    .AddConstructor<LteSlSciHeader> ()
  ;
  return tid;
}

TypeId
LteSlSciHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
LteSlSciHeader::Print (std::ostream &os)  const
{
  //todo
}

uint32_t
LteSlSciHeader::GetSerializedSize (void) const
{
  return 6;
}

void
LteSlSciHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  //we change the order so the serialize/deserialize is easier
  //write flag and TRP
  uint8_t hopFlagAndTrp = m_hopping ? 1 << 7 : 0;
  hopFlagAndTrp = hopFlagAndTrp | (m_trp & 0x7F);
  i.WriteU8 (hopFlagAndTrp);
  i.WriteU8 (m_mcs);
  i.WriteU8 (m_groupDstId);
  i.WriteU8 (m_rbStart);
  i.WriteU8 (m_rbLen);
  i.WriteU8 (m_hoppingInfo);
}

uint32_t
LteSlSciHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  uint8_t tmp = i.ReadU8 ();
  m_hopping = (tmp & 0x80) != 0;
  m_trp = tmp & 0x7F;
  m_mcs = i.ReadU8 ();
  m_groupDstId = i.ReadU8 ();
  m_rbStart = i.ReadU8 ();
  m_rbLen = i.ReadU8 ();
  m_hoppingInfo = i.ReadU8 ();

  return GetSerializedSize ();
}

}  // namespace ns3
