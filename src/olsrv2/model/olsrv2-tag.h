//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software
//

#ifndef OLSRV2_TAG_H
#define OLSRV2_TAG_H

#include "ns3/tag.h"

namespace ns3
{

namespace olsrv2
{

/**
 * Tag for detecting OLSRv2 messages at lower layers.
 */
class MessageTag : public ns3::Tag
{
  public:
    /**
     * @brief Gets the TypeId for the MessageTag class.
     * @returns The TypeId.
     */
    static TypeId GetTypeId(void);
    /**
     * @brief Creates an instance of the MessageTag class.
     */
    MessageTag();

    // Inherited from ObjectBase
    TypeId GetInstanceTypeId(void) const override;

    // Inherited from PacketTag
    void Deserialize(TagBuffer i) override;
    uint32_t GetSerializedSize() const override;
    void Print(std::ostream& os) const override;
    void Serialize(TagBuffer i) const override;

    /**
     * Gets the OLSRv2 message type
     * @returns The OLSRv2 message type.
     */
    uint8_t GetType() const;
    /**
     * Sets the OLSRv2 message type.
     * @param type The OLSRv2 message type.
     */
    void SetType(const uint8_t type);

  private:
    uint8_t m_type{0}; //!< The OLSRv2 message type.
};

} // namespace olsrv2

} // namespace ns3

#endif /* OLSRV2_TAG_H */
