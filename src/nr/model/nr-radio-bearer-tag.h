// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_RADIO_BEARER_TAG_H
#define NR_RADIO_BEARER_TAG_H

#include "ns3/tag.h"

namespace ns3
{

class Tag;

/**
 * \ingroup spectrum
 *
 * \brief Tag used to define the RNTI and LC id for each MAC packet transmitted
 */
class NrRadioBearerTag : public Tag
{
  public:
    /**
     * \brief Get the object TypeId
     * \return the object type id
     */
    static TypeId GetTypeId();

    /**
     * \brief Get the InstanceTypeId
     * \return the TypeId of the instance
     */
    TypeId GetInstanceTypeId() const override;

    /**
     * Create an empty NrRadioBearerTag
     */
    NrRadioBearerTag();

    /**
     * Create a NrRadioBearerTag with the given RNTI and LC id
     */
    NrRadioBearerTag(uint16_t rnti, uint8_t lcId, uint32_t size);

    /**
     * Create a NrRadioBearerTag with the given RNTI, LC id and layer
     */
    NrRadioBearerTag(uint16_t rnti, uint8_t lcId, uint32_t size, uint8_t layer);

    /**
     * Set the RNTI to the given value.
     *
     * \param rnti the value of the RNTI to set
     */
    void SetRnti(uint16_t rnti);

    /**
     * Set the LC id to the given value.
     *
     * \param lcid the value of the RNTI to set
     */
    void SetLcid(uint8_t lcid);

    /**
     * Set the layer id to the given value.
     *
     * \param layer the value of the layer to set
     */
    void SetLayer(uint8_t layer);

    /**
     * Set the size of the RLC PDU in bytes.
     *
     * \param size the size of the RLC PDU in bytes
     */
    void SetSize(uint32_t size);

    // inherited
    void Serialize(TagBuffer i) const override;
    void Deserialize(TagBuffer i) override;
    uint32_t GetSerializedSize() const override;
    void Print(std::ostream& os) const override;

    /**
     * \brief Get the Rnti
     * \return the RNTI
     */
    uint16_t GetRnti() const;
    /**
     * \brief Get the Lcid
     * \return the LCID
     */
    uint8_t GetLcid() const;
    /**
     * \brief Get the Layer
     * \return the layer (?)
     */
    uint8_t GetLayer() const;
    /**
     * \brief Get Size
     * \return size in bytes of RLC PDU
     */
    uint32_t GetSize() const;

  private:
    uint16_t m_rnti; //!< RNTI
    uint8_t m_lcid;  //!< LCID
    uint8_t m_layer; //!< Layer
    uint32_t m_size; //!< Size in bytes of RLC PDU
};

} // namespace ns3

#endif /* NR_RADIO_BEARER_TAG_H */
