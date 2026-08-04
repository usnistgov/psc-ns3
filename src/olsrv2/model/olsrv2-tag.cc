//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software
//

#include "olsrv2-tag.h"

#include "olsrv2-header.h"

namespace ns3
{

namespace olsrv2
{

NS_OBJECT_ENSURE_REGISTERED(MessageTag);

NS_LOG_COMPONENT_DEFINE("Olsrv2MessageTag");

TypeId
MessageTag::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::olsrv2::MessageTag").SetParent<ns3::Tag>().AddConstructor<MessageTag>();
    return tid;
}

MessageTag::MessageTag()
{
}

TypeId
MessageTag::GetInstanceTypeId() const
{
    return MessageTag::GetTypeId();
}

void
MessageTag::Deserialize(TagBuffer i)
{
    m_type = i.ReadU8();
}

uint32_t
MessageTag::GetSerializedSize() const
{
    return 1;
}

void
MessageTag::Print(std::ostream& os) const
{
    switch (m_type)
    {
    case MessageHeader::HELLO_MESSAGE:
        os << "Olsrv2Hello";
        break;
    case MessageHeader::TC_MESSAGE:
        os << "Olsrv2Tc";
        break;
    case MessageHeader::MID_MESSAGE:
        os << "Olsrv2Mid";
        break;
    case MessageHeader::HNA_MESSAGE:
        os << "Olsrv2Hna";
        break;
    default:
        NS_ABORT_MSG("Uknown OLSRv2 message type");
    }
}

void
MessageTag::Serialize(TagBuffer i) const
{
    i.WriteU8(m_type);
}

uint8_t
MessageTag::GetType() const
{
    return m_type;
}

void
MessageTag::SetType(uint8_t type)
{
    m_type = type;
}

} // namespace olsrv2

} // namespace ns3
