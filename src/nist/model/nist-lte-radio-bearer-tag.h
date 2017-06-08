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
 * Modified by: NIST
 */
#ifndef NIST_LTE_RADIO_BEARER_TAG_H
#define NIST_LTE_RADIO_BEARER_TAG_H

#include "ns3/tag.h"

namespace ns3 {

class Tag;


/**
 * Tag used to define the RNTI and LC id for each MAC packet trasmitted
 */

class NistLteRadioBearerTag : public Tag
{
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * Create an empty NistLteRadioBearerTag
   */
  NistLteRadioBearerTag ();

  /**
   * Create a NistLteRadioBearerTag with the given RNTI and LC id
   */
  NistLteRadioBearerTag (uint16_t  rnti, uint8_t lcId);

  /**
   * Create a NistLteRadioBearerTag with the given RNTI, LC id, srcL2Id and dstL2Id
   * Used for sidelink communication
   */
  NistLteRadioBearerTag (uint16_t  rnti, uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id);

  /**
  * Create a NistLteRadioBearerTag with the given RNTI, LC id and layer
  */
  NistLteRadioBearerTag (uint16_t  rnti, uint8_t lcId, uint8_t layer);

  /**
   * Set the RNTI to the given value.
   *
   * @param rnti the value of the RNTI to set
   */
  void SetRnti (uint16_t rnti);

  /**
   * Set the LC id to the given value.
   *
   * @param lcid the value of the RNTI to set
   */
  void SetLcid (uint8_t lcid);
  
  /**
  * Set the layer id to the given value.
  *
  * @param layer the value of the layer to set
  */
  void SetLayer (uint8_t layer);

  /**
   * Sets the source L2 Id for sidelink identification of the PDCP entity
   * \param src
   */
  void SetSourceL2Id (uint32_t src);
  
  /**
   * Sets the source L2 Id for sidelink identification of the PDCP entity
   * \param src
   */
  void SetDestinationL2Id (uint32_t dst);


  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual uint32_t GetSerializedSize () const;
  virtual void Print (std::ostream &os) const;

  uint16_t GetRnti (void) const;
  uint8_t GetLcid (void) const;
  uint8_t GetLayer (void) const;
  uint32_t GetSourceL2Id (void) const;
  uint32_t GetDestinationL2Id (void) const;

private:
  uint16_t m_rnti;
  uint8_t m_lcid;
  uint8_t m_layer;
  /* Additional identifier for sidelink */
  uint32_t m_srcL2Id;  /**< Source L2 ID (24 bits) */
  uint32_t m_dstL2Id;  /**< Destination L2 ID (24 bits) */

};



} // namespace ns3

#endif /* NIST_LTE_RADIO_BEARER_TAG_H */
