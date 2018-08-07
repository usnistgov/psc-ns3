/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Author: Marco Miozzo  <marco.miozzo@cttc.es>
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */
#ifndef LTE_RADIO_BEARER_TAG_H
#define LTE_RADIO_BEARER_TAG_H

#include "ns3/tag.h"

namespace ns3 {

class Tag;


/**
 * Tag used to define the RNTI and LC id for each MAC packet trasmitted
 */

class LteRadioBearerTag : public Tag
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * Create an empty LteRadioBearerTag
   */
  LteRadioBearerTag ();

  /**
   * Create a LteRadioBearerTag with the given RNTI and LC id
   * \param rnti the RNTI
   * \param lcId the LCID
   */
  LteRadioBearerTag (uint16_t  rnti, uint8_t lcId);
  
  /**
   * Create a LteRadioBearerTag with the given RNTI, LC id and layer
   * \param rnti the RNTI
   * \param lcId the LCID
   * \param layer the layer
  */
  LteRadioBearerTag (uint16_t  rnti, uint8_t lcId, uint8_t layer);
  
  /**
   * Create a LteRadioBearerTag with the given RNTI, LC id, layer, srcL2Id and dstL2Id use for Sidelink
   * \param rnti the RNTI
   * \param lcId the LCID
   * \param srcL2Id the layer2 id of the source
   * \param dstL2Id the layer2 id of the destination
  */
  LteRadioBearerTag (uint16_t rnti, uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id);

  /**
   * Set the RNTI to the given value.
   *
   * \param rnti the value of the RNTI to set
   */
  void SetRnti (uint16_t rnti);

  /**
   * Set the LC id to the given value.
   *
   * \param lcid the value of the RNTI to set
   */
  void SetLcid (uint8_t lcid);
  
  /**
  * Set the layer id to the given value.
  *
  * \param layer the value of the layer to set
  */
  void SetLayer (uint8_t layer);

  /**
   * Sets the source L2 Id for sidelink identification of the RLC UM and PDCP entity
   * \param src The Sidelink source layer 2 id
   */
  void SetSourceL2Id (uint32_t src);
  
  /**
   * Sets the destination L2 Id for sidelink identification of the RLC UM and PDCP entity
   * \param dst The Sidelink destination layer 2 id
   */
  void SetDestinationL2Id (uint32_t dst);


  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual uint32_t GetSerializedSize () const;
  virtual void Print (std::ostream &os) const;

  /**
   * Get RNTI function
   *
   * \returns RNTI
   */
  uint16_t GetRnti (void) const;
  /**
   * Get LCID function
   *
   * \returns LCID
   */
  uint8_t GetLcid (void) const;
  /**
   * Get layer function
   *
   * \returns layer
   */
  uint8_t GetLayer (void) const;
  /**
   * Get source layer2 id function
   *
   * \returns source layer2 id
   */
  uint32_t GetSourceL2Id (void) const;
  /**
   * Get destination layer2 id function
   *
   * \returns destination layer2 id
   */
  uint32_t GetDestinationL2Id (void) const;

private:
  uint16_t m_rnti; ///< RNTI
  uint8_t m_lcid; ///< LCID
  uint8_t m_layer; ///< layer
  uint32_t m_srcL2Id; ///< Source L2 ID (24 bits)
  uint32_t m_dstL2Id; ///< Destination L2 ID (24 bits)

};



} // namespace ns3

#endif /* LTE_RADIO_BEARER_TAG_H */
