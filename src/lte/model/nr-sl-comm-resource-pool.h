/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef NR_SL_COMM_RESOURCE_POOL
#define NR_SL_COMM_RESOURCE_POOL

#include <ns3/object.h>
#include "lte-rrc-sap.h"
#include "nr-sl-ue-rrc.h"

#include <stdint.h>
#include <list>
#include <array>
#include <bitset>
#include <unordered_map>

namespace ns3 {


class NrSlCommResourcePool : public Object
{

public:
  /**
   * \brief NR Sidelink slot info.
   */
  struct SlotInfo
  {
    /**
     * \brief NrSlSlotInfo constructor
     * \param numSlPscchRbs Indicates the number of PRBs for PSCCH in a resource pool where it is not greater than the number PRBs of the subchannel.
     * \param slPscchSymStart Indicates the starting symbol used for sidelink PSCCH in a slot
     * \param slPscchSymLength Indicates the total number of symbols available for sidelink PSCCH
     * \param slPsschSymStart Indicates the starting symbol used for sidelink PSSCH in a slot
     * \param slPsschSymLength Indicates the total number of symbols available for sidelink PSSCH
     * \param slSubchannelSize Indicates the subchannel size in number of RBs
     * \param slMaxNumPerReserve Indicates the maximum number of reserved PSCCH/PSSCH resources that can be indicated by an SCI.
     * \param absSlotIndex Indicates the the absolute slot index
     * \param slotOffset Indicates the positive offset between two slots
     */
    SlotInfo (uint16_t numSlPscchRbs, uint16_t slPscchSymStart, uint16_t slPscchSymLength,
              uint16_t slPsschSymStart, uint16_t slPsschSymLength, uint16_t slSubchannelSize,
              uint16_t slMaxNumPerReserve, uint64_t absSlotIndex, uint32_t slotOffset)
    {
      this->numSlPscchRbs = numSlPscchRbs;
      this->slPscchSymStart = slPscchSymStart;
      this->slPscchSymLength = slPscchSymLength;
      this->slPsschSymStart = slPsschSymStart;
      this->slPsschSymLength = slPsschSymLength;
      this->slSubchannelSize = slSubchannelSize;
      this->slMaxNumPerReserve = slMaxNumPerReserve;
      this->absSlotIndex = absSlotIndex;
      this->slotOffset = slotOffset;
    }
    //PSCCH
    uint16_t numSlPscchRbs {0}; //!< Indicates the number of PRBs for PSCCH in a resource pool where it is not greater than the number PRBs of the subchannel.
    uint16_t slPscchSymStart {0}; //!< Indicates the starting symbol used for sidelink PSCCH in a slot
    uint16_t slPscchSymLength {0}; //!< Indicates the total number of symbols available for sidelink PSCCH
    //PSSCH
    uint16_t slPsschSymStart {0}; //!< Indicates the starting symbol used for sidelink PSSCH in a slot
    uint16_t slPsschSymLength {0}; //!< Indicates the total number of symbols available for sidelink PSSCH
    //subchannel size in RBs
    uint16_t slSubchannelSize {0}; //!< Indicates the subchannel size in number of RBs
    uint16_t slMaxNumPerReserve {0}; //!< The maximum number of reserved PSCCH/PSSCH resources that can be indicated by an SCI.
    uint64_t absSlotIndex {0}; //!< Indicates the the absolute slot index
    uint32_t slotOffset {0}; //!< Indicates the positive offset between two slots
  };

  /**
   * Scheduling types of sidelink pools hold by this class. At one time,
   * either all the pools will be used for UE_SELECTED scheduling or
   * network SCHEDULED. There can not be mix-up.
   */
  enum SchedulingType
  {
    UNKNOWN = 0,
    SCHEDULED,
    UE_SELECTED
  };
  /**
   * \brief Map to store the physical SL pool per BWP and per SL pool
   *
   * Key of the first map is the BWP id
   * Value of the first map is an unordered map storing physical SL pool per SL pool
   * Key of the second map is the pool id
   * Value of the second map is a vector, which is a physical SL pool
   */
  typedef std::unordered_map<uint8_t, std::unordered_map <uint16_t, std::vector <std::bitset<1>>> > PhySlPoolMap;

  /**
   * \brief Checks if two NR Sidelink pool configurations are identical
   *
   * \param other The configuration of the other resource pool
   * \return true if this configuration is the same as the other one
   */
  bool operator== (const NrSlCommResourcePool& other) const;

  /**
   * \brief A struct containing the iterator of the BWP map and the pool
   *        associated to the BWP.
   */
  struct BwpAndPoolIt
  {
    NrSlCommResourcePool::PhySlPoolMap::const_iterator itBwp; //!< Iterator to the first map in PhySlPoolMap
    std::unordered_map <uint16_t, std::vector <std::bitset<1>>>::const_iterator itPool; //!< Iterator to the second map in PhySlPoolMap
  };

  /**
   * \brief Constructor
   */
  NrSlCommResourcePool ();
  /**
   * \brief Destructor
   */
  virtual ~NrSlCommResourcePool ();

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();

  /**
   * \brief Set NR Sidelink preconfiguration  frequency information list
   *
   * \param slPreconfigFreqInfoList A list containing per carrier configuration for NR sidelink communication
   */
  void SetNrSlPreConfigFreqInfoList (const std::array <LteRrcSap::SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL> &slPreconfigFreqInfoList);
  /**
   * \brief Set NR Sidelink physical pool map
   *
   * \param phySlPoolMap The Map containing the physical SL pool per BWP and per SL pool
   */
  void SetNrSlPhysicalPoolMap (NrSlCommResourcePool::PhySlPoolMap phySlPoolMap);
  /**
   * \brief Get NR Sidelink physical sidelink pool
   *
   * \param bwpId The bandwidth part id
   * \param poolId The pool id
   * \return A vector representing the the physical sidelink pool
   */
  const std::vector <std::bitset<1>> GetNrSlPhyPool (uint8_t bwpId, uint16_t poolId) const;
  /**
   * \brief Get NR Sidelink communication opportunities
   *
   * TS 38.214 8.1.2.1 <b>"Within the slot, PSSCH resource allocation starts at symbol startSLsymbols+1"</b>
   * Since we are multiplexing the PSCCH and PSSCH in time, we are already
   * taking the TS 38.214 (8.1.2.1) into the consideration. That is,
   * <b>1.</b> \f$number of PSSCH symbols = Total symbols - number of PSCCH symbols\f$
   * TS 38.214 8.1.2.1 also says <b>"The UE shall not transmit PSSCH in the last symbol configured for sidelink.</b>
   * Therefore, we to subtract one more symbol from the PSSCH symbols, i.e.,
   * <b>2.</b> \f$Total number of PSSCH symbols = number of PSSCH symbols (from 1) - 1\f$
   *
   * \param absIndexCurretSlot The current absolute slot number
   * \param bwpId The bandwidth part id
   * \param numerology The numerology
   * \param poolId The pool id
   * \param t1 The start of the selection window in physical slots, accounting for physical layer processing delay
   * \param t2 The end of the selection window in physical slots
   * \return A list of sidelink communication opportunities for each available slot in a selection window
   */
  std::list <NrSlCommResourcePool::SlotInfo> GetNrSlCommOpportunities (uint64_t absIndexCurretSlot, uint8_t bwpId, uint16_t numerology, uint16_t poolId, uint8_t t1, uint16_t t2) const;
  /**
   * \brief Get NR Sidelink sensing window in slots
   *
   * \param bwpId The bandwidth part id
   * \param poolId The pool id
   * \param slotLength The slot length in time
   * \return The length of the sensing window in slots
   */
  uint16_t GetNrSlSensWindInSlots (uint8_t bwpId, uint16_t poolId, Time slotLength) const;
  /**
   * \brief Set NR Sidelink scheduling type to be used for the pools
   *
   * \param type The scheduling type [NrSlCommResourcePool::SchedulingType]
   */
  void SetNrSlSchedulingType (NrSlCommResourcePool::SchedulingType type);
  /**
   * \brief Get NR Sidelink scheduling type used for the pools
   *
   * \return The scheduling type [NrSlCommResourcePool::SchedulingType]
   */
  NrSlCommResourcePool::SchedulingType GetNrSlSchedulingType () const;
  /**
   * \brief Get NR Sidelink subchannel size
   *
   * \param bwpId The bandwidth part id
   * \param poolId The pool id
   * \return The subchannel size in RBs
   */
  uint16_t GetNrSlSubChSize (uint8_t bwpId, uint16_t poolId) const;
  /**
   * \brief Validate that the given resource reservation period is in the list
   *        of user provided configuration. If it is not in the list an assert
   *        will hit.
   * \param bwpId The bandwidth part id
   * \param poolId The poolId The pool id
   * \param resvPeriod The resource reservation period
   * \param slotLength The slot length in time
   */
  void ValidateResvPeriod (uint8_t bwpId, uint16_t poolId, Time resvPeriod, Time slotLength) const;
  /**
   * \brief Get the reservation period in slots
   * \param bwpId The bandwidth part id
   * \param poolId The poolId The pool id
   * \param resvPeriod The reservation period in ms
   * \param slotLength The slot length in time
   * \return The reservation period in slots
   */
  uint16_t GetResvPeriodInSlots (uint8_t bwpId, uint16_t poolId, Time resvPeriod, Time slotLength) const;
  /**
   * \brief Is Sidelink slot
   * \param bwpId The bandwidth part id
   * \param poolId The poolId The pool id
   * \param absSlotIndex The current absolute slot index
   * \return true if it is the Sidelink slot, false otherwise
   */
  bool IsSidelinkSlot (uint8_t bwpId, uint16_t poolId, uint64_t absSlotIndex) const;
  /**
   * \brief Get Absolute index of the pool slot as pet the current absolute slot
   * \param bwpId The bandwidth part id
   * \param poolId The poolId The pool id
   * \param absSlotIndex The current absolute slot index
   * \return The absolute index of the pool slot
   */
  uint16_t GetAbsPoolIndex (uint8_t bwpId, uint16_t poolId, uint64_t absSlotIndex) const;
  /**
   * \brief Set the TDD pattern.
   * \param tddPattern The TDD pattern
   *
   * For example, a valid pattern would be "DL|DL|UL|UL|DL|DL|UL|UL|". The slot
   * types allowed are:
   *
   * - "DL" for downlink only
   * - "UL" for uplink only
   * - "F" for flexible (dl and ul)
   * - "S" for special slot (LTE-compatibility)
   */
  void SetTddPattern (std::vector<NrSlUeRrc::LteNrTddSlotType> tddPattern);
  /**
   * \brief Get Nr Sidelink PSSCH subchannel size in RBs
   * \param bwpId bwpId The bandwidth part id
   * \param poolId poolId The poolId The pool id
   * \return PSSCH subchannel size in RBs
   */
  uint16_t GetSlSubChSize (uint8_t bwpId, uint16_t poolId) const;

private:
  /**
   * \brief Get SlResourcePoolNr
   *
   * \param bwpId The BWP id
   * \param poolId The pool id
   * \return The \link LteRrcSap::SlResourcePoolNr \endlink, which holds the
   *         SL pool related configuration.
   */
  const LteRrcSap::SlResourcePoolNr GetSlResourcePoolNr (uint8_t bwpId, uint16_t poolId) const;
  /**
   * \brief Validate if the pool id is configured for the given bandwidthpart id.
   * \param bwpId The BWP id
   * \param poolId The pool id
   * \return
   */
  BwpAndPoolIt ValidateBwpAndPoolId (uint8_t bwpId, uint16_t poolId) const;
  /**
   * \brief Is Sidelink resource reservation period multiple of physical
   *        Sidelink pool length.
   * \param bwpId The BWP id
   * \param poolId The pool id
   * \param rsvpInSlots The resource reservation period in slots
   */
  void IsRsvpMultipOfPoolLen (uint8_t bwpId, uint16_t poolId, uint16_t rsvpInSlots) const;

  std::array <LteRrcSap::SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL> m_slPreconfigFreqInfoList; //!< A list containing per carrier configuration for NR sidelink communication
  PhySlPoolMap m_phySlPoolMap; //!< A map to store the physical SL pool per BWP and per SL pool
  SchedulingType m_schType {SchedulingType::UNKNOWN}; //!< Type of the scheduling to be used for the pools hold by this class
  std::vector<NrSlUeRrc::LteNrTddSlotType> m_tddPattern; //!< TDD pattern

};



}

#endif /* NR_SL_COMM_RESOURCE_POOL */
