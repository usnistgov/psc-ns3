//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software
//

#include "nhdp-hello-tag.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NhdpHelloTag");

namespace nhdp
{

NS_OBJECT_ENSURE_REGISTERED(HelloTag);

TypeId
HelloTag::GetTypeId()
{
    static TypeId tid = TypeId("ns3::nhdp::HelloTag").SetParent<Tag>().AddConstructor<HelloTag>();
    return tid;
}

HelloTag::HelloTag()
{
}

TypeId
HelloTag::GetInstanceTypeId() const
{
    return HelloTag::GetTypeId();
}

void
HelloTag::Deserialize(TagBuffer i)
{
}

uint32_t
HelloTag::GetSerializedSize() const
{
    return 0;
}

void
HelloTag::Print(std::ostream& os) const
{
    os << "NhdpHello";
}

void
HelloTag::Serialize(TagBuffer i) const
{
}

} // namespace nhdp

} // namespace ns3
