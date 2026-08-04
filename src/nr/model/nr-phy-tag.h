// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Marco Miozzo  <marco.miozzo@cttc.es>
//         Nicola Baldo  <nbaldo@cttc.es>

#ifndef NR_PHY_TAG_H
#define NR_PHY_TAG_H

#include "ns3/tag.h"

namespace ns3
{

/**
 * Tag used to define PHY parameters
 */
class NrPhyTag : public Tag
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    /**
     * Create an empty NrPhyTag
     */
    NrPhyTag();

    /**
     * Create a NrPhyTag with the given RNTI and LC id
     * \param cellId the cell ID
     */
    NrPhyTag(uint16_t cellId);

    ~NrPhyTag() override;

    void Serialize(TagBuffer i) const override;
    void Deserialize(TagBuffer i) override;
    uint32_t GetSerializedSize() const override;
    void Print(std::ostream& os) const override;

    /**
     * Get cell ID
     *
     * \returns cell ID
     */
    uint16_t GetCellId() const;

  private:
    uint16_t m_cellId; ///< the cell ID
};

} // namespace ns3

#endif /* NR_PHY_TAG_H */
