/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 CTTC
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
 * This class, authored by NIST, is derived from lte-harq-phy.h
 * originally authored by Marco Miozzo  <marco.miozzo@cttc.es>.
 */

#ifndef LTE_SL_HARQ_PHY_MODULE_H
#define LTE_SL_HARQ_PHY_MODULE_H

#include <ns3/log.h>
#include <ns3/assert.h>
#include <math.h>
#include <vector>
#include <map>
#include <set>
#include <ns3/simple-ref-count.h>
#include <ns3/lte-harq-phy.h>


namespace ns3 {


/**
 * \ingroup lte
 * \brief The LteSlHarqPhy class implements the Sidelink HARQ functionalities related to PHY layer
 *(i.e., decodification buffers for incremental redundancy management). We use the same global data
 *structures struct HarqProcessInfoElement_t and std::vector < HarqProcessInfoElement_t > HarqProcessInfoElement_t
 *structures from LteHarqPhy so we do not break LteNistErrorModel.
 *
 */
class LteSlHarqPhy : public SimpleRefCount<LteSlHarqPhy>
{
public:
  LteSlHarqPhy ();
  ~LteSlHarqPhy ();

  /**
   * Set the number of retransmissions for Sidelink Discovery messages
   * \param retx The number of retransmissions
   */
  void SetDiscNumRetx (uint8_t retx);
  /**
   * Gets the current TB index.
   * \param rnti The UE identifier
   * \param l1dst The group identifier
   * \returns The TB index.
   */
  uint32_t GetTbIdx (uint16_t rnti, uint8_t l1dst);
  /**
   * Increases the TB index by one.
   * \param rnti The UE identifier
   * \param l1dst The group identifier
   */
  void IncreaseTbIdx (uint16_t rnti, uint8_t l1dst);
  /**
   * Updates the last created harq information to indicate that the TB of this
   * harq information has been decoded.
   * \param rnti The UE identifier
   * \param l1dst The group identifier
   */
  void IndicatePrevDecoded (uint16_t rnti, uint8_t l1dst);
  /**
   * Gets the flag that indicates whether or not one of the TB's regarding this
   * harq information has already been decoded.
   * \param rnti The UE identifier
   * \param l1dst The group identifier
   * \returns True, if one of the TBs has been decoded; false, otherwise.
   */
  bool IsPrevDecoded (uint16_t rnti, uint8_t l1dst);
  /**
   * Resets the flag that indicates whether or not one of the TB's regarding
   * this harq information has already been decoded.
   * \param rnti The UE identifier
   * \param l1dst The group identifier
   */
  void ResetPrevDecoded (uint16_t rnti, uint8_t l1dst);
  /**
   * Updates the last created harq information to indicate that the TB of this
   * harq information has been decoded.
   * \param rnti The UE identifier
   * \param resPsdch The discovery resource index
   */
  void IndicateDiscTbPrevDecoded (uint16_t rnti, uint8_t resPsdch);
  /**
   * Gets the flag that indicates whether or not one of the TB's regarding this
   * harq information has already been decoded.
   * \param rnti The UE identifier
   * \param resPsdch The discovery resource index
   * \returns True, if one of the TBs has been decoded; false, otherwise.
   */
  bool IsDiscTbPrevDecoded (uint16_t rnti, uint8_t resPsdch);
  /**
   * Resets the flag that indicates whether or not one of the TB's regarding
   * this harq information has already been decoded.
   * \param rnti The UE identifier
   * \param resPsdch The discovery resource index
   */
  void ResetDiscTbPrevDecoded (uint16_t rnti, uint8_t resPsdch);
  /**
   * Resets the TB count
   * \param rnti The UE identifier
   * \param l1dst The group identifier
   */
  void ResetTbIdx (uint16_t rnti, uint8_t l1dst);

  /**
   * \brief Return the cumulated MI of the HARQ procId in case of retransmissions
   * for SL
   * \param rnti The UE identifier
   * \param l1dst The layer 1 destination ID
   * \return the MI accumulated
   */
  double GetAccumulatedMiSl (uint16_t rnti, uint8_t l1dst);

  /**
   * \brief Return the info of the HARQ procId in case of retransmissions
   * for SL
   * \param rnti the RNTI of the transmitter
   * \param l1dst The layer 1 destination ID
   * \return the vector of the info related to HARQ proc Id
   */
  HarqProcessInfoList_t GetHarqProcessInfoSl (uint16_t rnti, uint8_t l1dst);

  /**
   * \brief Return the info of the HARQ procId in case of retransmissions
   * for d2d discovery
   * \param rnti the RNTI of the transmitter
   * \param resPsdch The resource used
   * \return the vector of the info related to HARQ proc Id
   */
  HarqProcessInfoList_t GetHarqProcessInfoDisc (uint16_t rnti, uint8_t resPsdch);

  /**
   * \brief Update the MI value associated to the decodification of an HARQ process
   * for SL
   * \param rnti the RNTI of the transmitter
   * \param l1dst the layer 1 destination ID
   * \param mi the new MI
   * \param infoBytes the number of bytes of info
   * \param codeBytes the total number of bytes txed
   */
  void UpdateSlHarqProcessStatus (uint16_t rnti, uint8_t l1dst, double mi, uint16_t infoBytes, uint16_t codeBytes);

  /**
   * \brief Update the SINR value associated to the decodification of an HARQ process
   * for SL
   * \param rnti the RNTI of the transmitter
   * \param l1dst the layer 1 destination ID
   * \param sinr the new SINR
   */
  void UpdateSlHarqProcessStatus (uint16_t rnti, uint8_t l1dst, double sinr);

  /**
   * \brief Update the MI value associated to the decodification of an HARQ process
   * for DL (asynchronous)
   * \param rnti the RNTI of the transmitter
   * \param resPsdch resource used
   * \param sinr the new SINR
   */
  void UpdateDiscHarqProcessStatus (uint16_t rnti, uint8_t resPsdch, double sinr);

  /**
   * \brief Reset  the info associated to the decodification of an HARQ process
   * for SL
   * \param rnti the RNTI
   * \param l1dst the layer 1 destination ID
   */
  void ResetSlHarqProcessStatus (uint16_t rnti, uint8_t l1dst);

  /**
   * \brief Reset  the info associated to the decodification of an HARQ process
   * \param rnti rnti of the transmitter
   * \param resPsdch resource used
   */
  void ResetDiscHarqProcessStatus (uint16_t rnti, uint8_t resPsdch);

private:
  std::set<uint32_t> m_decoded; ///< Container to track Sidelink communication decoded TBs
  std::set<uint32_t> m_discDecoded; ///< Container to track Sidelink discovery decoded TBs
  std::map<uint32_t, uint32_t> m_tbIdx; ///< map the TB id (formed by (rnti << 8) + l1dst) and the count of TB received for a particular TB id
  std::map <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> > m_miSlHarqProcessesInfoMap; ///< Sidelink communication HARQ processes info map
  std::map <uint16_t, std::map <uint8_t, HarqProcessInfoList_t> > m_miDiscHarqProcessesInfoMap; ///< Sidelink discovery HARQ process info map

  uint8_t m_discNumRetx; ///< Total number retransmissions configured for Sidelink discovery transmissions

};


}

#endif /* LTE_SL_HARQ_PHY_MODULE_H */
