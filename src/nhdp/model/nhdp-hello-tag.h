//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software
//

#ifndef NHDP_TAG_HELLO_H
#define NHDP_TAG_HELLO_H

#include "ns3/tag.h"

namespace ns3
{

namespace nhdp
{

/**
 * Tag for detecting NHDP hello messages at lower layers.
 */
class HelloTag : public Tag
{
  public:
    /**
     * @brief Gets the TypeId for the HelloTag class.
     * @returns The TypeId.
     */
    static TypeId GetTypeId(void);
    /**
     * @brief Creates an instance of the HelloTag class.
     */
    HelloTag();

    // Inherited from ObjectBase
    TypeId GetInstanceTypeId(void) const override;

    // Inherited from PacketTag
    void Deserialize(TagBuffer i) override;
    uint32_t GetSerializedSize() const override;
    void Print(std::ostream& os) const override;
    void Serialize(TagBuffer i) const override;
};

} // namespace nhdp

} // namespace ns3

#endif /* NHDP_TAG_HELLO_H */
