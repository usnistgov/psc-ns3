// Copyright (c) 2011 CTTC
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Jaume Nin <jaume.nin@cttc.es>

#ifndef NR_RLC_TAG_H
#define NR_RLC_TAG_H

#include "ns3/nstime.h"
#include "ns3/packet.h"

namespace ns3
{

class Tag;

/**
 * Tag to calculate the per-PDU delay from gNB RLC to UE RLC
 */

class NrRlcTag : public Tag
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    /**
     * Create an empty RLC tag
     */
    NrRlcTag();
    /**
     * Create an RLC tag with the given senderTimestamp
     * \param senderTimestamp the time
     */
    NrRlcTag(Time senderTimestamp);

    void Serialize(TagBuffer i) const override;
    void Deserialize(TagBuffer i) override;
    uint32_t GetSerializedSize() const override;
    void Print(std::ostream& os) const override;

    /**
     * Get the instant when the RLC delivers the PDU to the MAC SAP provider
     * @return the sender timestamp
     */
    Time GetSenderTimestamp() const
    {
        return m_senderTimestamp;
    }

    /**
     * Set the sender timestamp
     * @param senderTimestamp time stamp of the instant when the RLC delivers the PDU to the MAC SAP
     * provider
     */
    void SetSenderTimestamp(Time senderTimestamp)
    {
        this->m_senderTimestamp = senderTimestamp;
    }

    /**
     * Get the RNTI of the transmitting UE
     * @return the RNTI of the transmitting UE
     */
    uint16_t GetTxRnti() const
    {
        return m_txRnti;
    }

    /**
     * Set the RNTI of the transmitting UE
     * @param rnti the RNTI of the transmitting UE
     */
    void SetTxRnti(uint16_t rnti)
    {
        this->m_txRnti = rnti;
    }

  private:
    Time m_senderTimestamp;                                  ///< sender timestamp
    uint16_t m_txRnti{std::numeric_limits<uint16_t>::max()}; ///< rnti of transmitting UE
};

} // namespace ns3

#endif /* NR_RLC_TAG_H */
