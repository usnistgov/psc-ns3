//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software
//

#ifndef NR_SL_PROSE_TAG_H
#define NR_SL_PROSE_TAG_H

#include "ns3/tag.h"

namespace ns3
{

/**
 * Tag for detecting application layer packets at lower layers.
 */
class NrSlProseAppTag : public Tag
{
  public:
    /**
     * @brief Gets the TypeId for the NrSlProseAppTag class.
     * @returns The TypeId.
     */
    static TypeId GetTypeId(void);
    /**
     * @brief Creates an instance of the NrSlProseAppTag class.
     */
    NrSlProseAppTag();

    // Inherited from ObjectBase
    TypeId GetInstanceTypeId(void) const override;

    // Inherited from PacketTag
    void Deserialize(TagBuffer i) override;
    uint32_t GetSerializedSize() const override;
    void Print(std::ostream& os) const override;
    void Serialize(TagBuffer i) const override;
};

/**
 * Tag for detecting ProSe discovery messages at lower layers.
 */
class NrSlProseDiscoveryTag : public Tag
{
  public:
    /**
     * @brief Gets the TypeId for the NrSlProseDiscoveryTag class.
     * @returns The TypeId.
     */
    static TypeId GetTypeId(void);
    /**
     * @brief Creates an instance of the NrSlProseDiscoveryTag class.
     */
    NrSlProseDiscoveryTag();

    // Inherited from ObjectBase
    TypeId GetInstanceTypeId(void) const override;

    // Inherited from PacketTag
    void Deserialize(TagBuffer i) override;
    uint32_t GetSerializedSize() const override;
    void Print(std::ostream& os) const override;
    void Serialize(TagBuffer i) const override;
};

/**
 * Tag for detecting ProSe PC5 signalling messages at lower layers.
 */
class NrSlProsePc5SignallingTag : public Tag
{
  public:
    /**
     * @brief Gets the TypeId for the NrSlProsePc5SignallingTag class.
     * @returns The TypeId.
     */
    static TypeId GetTypeId(void);
    /**
     * @brief Creates an instance of the NrSlProsePc5SignallingTag class.
     */
    NrSlProsePc5SignallingTag();

    // Inherited from ObjectBase
    TypeId GetInstanceTypeId(void) const override;

    // Inherited from PacketTag
    void Deserialize(TagBuffer i) override;
    uint32_t GetSerializedSize() const override;
    void Print(std::ostream& os) const override;
    void Serialize(TagBuffer i) const override;

    /**
     * Gets the PC5 signalling message type
     * @returns The PC5 signalling message type.
     */
    uint8_t GetType() const;
    /**
     * Sets the PC5 signalling message type.
     * @param type The PC5 signalling message type.
     */
    void SetType(const uint8_t type);

  private:
    uint8_t m_type{0}; //!< The PC5 signalling message type.
};

} // namespace ns3

#endif /* NR_SL_PROSE_TAG_H */
