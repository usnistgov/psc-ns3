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
#include <ns3/lte-rrc-sap.h>

#include <stdint.h>
#include <list>
#include <array>
#include <bitset>
#include <unordered_map>

namespace ns3 {


class NrSlCommResourcePool : public Object
{

public:
  struct SlotInfo
  {
    //PSCCH
    uint16_t numSlPscchRbs {0};
    uint16_t slPscchSymStart {0};
    uint16_t slPscchSymlength {0};
    //PSSCH
    uint16_t slPsschSymStart {0};
    uint16_t slPsschSymlength {0};
    //subchannel size in RBs
    uint16_t slSubchannelSize {0};
    //Absolute slot number
    uint16_t absSlotIndex {0};
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
   * \brief Set sidelink preconfiguration  frequency information list
   *
   * \param slPreconfigFreqInfoList A list containing per carrier configuration for NR sidelink communication
   */
  void SetSlPreConfigFreqInfoList (const std::array <LteRrcSap::SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL> &slPreconfigFreqInfoList);
  /**
   * \brief Set physical sidelink pool map
   *
   * \param phySlPoolMap The Map containing the physical SL pool per BWP and per SL pool
   */
  void SetPhysicalSlPoolMap (NrSlCommResourcePool::PhySlPoolMap phySlPoolMap);
  /**
   * \brief Get physical sidelink pool
   *
   * \param bwpId The bandwidth part id
   * \param poolId The pool id
   * \return A vector representing the the physical sidelink pool
   */
  const std::vector <std::bitset<1>> GetPhySlPool (uint16_t bwpId, uint16_t poolId);
  /**
   * \brief Get sidelink communication opportunities
   *
   * TS 38.214 8.1.2.1 <b>"Within the slot, PSSCH resource allocation starts at symbol startSLsymbols+1"</b>
   * Since we are multiplexing the PSCCH and PSSCH in time, we are already
   * taking the TS 38.214 (8.1.2.1) into the consideration. That is,
   * <b>1.</b> \f$number of PSSCH symbols = Total symbols - number of PSCCH symbols\f$
   * TS 38.214 8.1.2.1 also says <b>"The UE shall not transmit PSSCH in the last symbol configured for sidelink.</b>
   * Therefore, we to subtract one more symbol from the PSSCH symbols, i.e.,
   * <b>2.</b> \f$Total number of PSSCH symbols = number of PSSCH symbols (from 1) - 1\f$
   *
   * \param numAbsSlot The current absolute slot number
   * \param bwpId The bandwidth part id
   * \param poolId The pool id
   * \param t1 The physical layer processing delay in slots
   * \return A list of sidelink communication opportunities for each available slot in a selection window
   */
  std::list <NrSlCommResourcePool::SlotInfo> GetSlCommOpportunities (uint16_t numAbsSlot, uint16_t bwpId, uint16_t poolId, uint16_t t1);
  /**
   * \brief Get number of slots for sensing window
   *
   * \param bwpId The bandwidth part id
   * \param poolId The pool id
   * \return The length of the sensing window in slots
   */
  uint16_t GetNumSlotsSensWind (uint16_t bwpId, uint16_t poolId, Time slotLength);



private:
  /**
   * \brief Get iterator to physical sidelink pool
   *
   * \param bwpId The bandwidth part id
   * \param poolId The pool id
   * \return An iterator to the stored physical sidelink pool
   */
  std::unordered_map <uint16_t, std::vector <std::bitset<1>>>::const_iterator GetIteratorToPhySlPool (uint16_t bwpId, uint16_t pooId);
  /*
   * \brief Get SlResourcePoolNr
   *
   * \param bwpId The BWP id
   * \param pooI The pool id
   * \return The LteRrcSap::SlResourcePoolNr, which holds the SL pool related configuration
   */
  const LteRrcSap::SlResourcePoolNr GetSlResourcePoolNr (uint16_t bwpId, uint16_t poolId);
  std::array <LteRrcSap::SlFreqConfigCommonNr, MAX_NUM_OF_FREQ_SL> m_slPreconfigFreqInfoList; //!< A list containing per carrier configuration for NR sidelink communication
  PhySlPoolMap m_phySlPoolMap; //!< A map to store the physical SL pool per BWP and per SL pool

};



}

#endif /* NR_SL_COMM_RESOURCE_POOL */
