/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
*   Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
*/
#ifndef NR_SL_UE_MAC_CSCHED_SAP_PROVIDER_H
#define NR_SL_UE_MAC_CSCHED_SAP_PROVIDER_H

#include "sfnsf.h"
#include <ns3/lte-sl-tft.h>

#include <iostream>
#include <list>
#include <vector>
#include <ns3/lte-sl-tft.h>

namespace ns3 {

/**
 * \ingroup scheduler
 * \brief The SAP interface between NR UE MAC and NR SL UE scheduler
 */
class NrSlUeMacCschedSapProvider
{
public:
  /**
   * ~NrSlUeMacCschedSapProvider
   */
  virtual ~NrSlUeMacCschedSapProvider () = default;

  /**
   * NR Sidelink Logical Channel information
   */
  struct SidelinkLogicalChannelInfo
  {
    /**
     * \brief SidelinkLogicalChannelInfo constructor
     * \param dstL2Id L2 destination id
     * \param lcId logical channel identifier
     * \param lcGroup logical channel group
     * \param pqi PC5 QoS Class Identifier
     * \param priority priority
     * \param isGbr true if the bearer is GBR, false if the bearer is NON-GBR
     * \param mbr maximum bitrate
     * \param gbr guaranteed bitrate
     * \param castType the Cast type
     * \param harqEnabled whether HARQ is enabled
     * \param pdb Packet Delay Budget
     * \param dynamic flag for whether it is dynamic or SPS
     * \param rri Resource Reservation Interval
     */
    SidelinkLogicalChannelInfo (uint32_t dstL2Id, uint8_t lcId, uint8_t lcGroup,
                                uint8_t pqi, uint8_t priority, bool isGbr,
                                uint64_t mbr, uint64_t gbr,
                                SidelinkInfo::CastType castType,
                                bool harqEnabled, Time pdb, bool dynamic,
                                Time rri)
    {
      this->dstL2Id = dstL2Id;
      this->lcId = lcId;
      this->lcGroup = lcGroup;
      this->pqi = pqi;
      this->priority = priority;
      this->isGbr = isGbr;
      this->mbr = mbr;
      this->gbr = gbr;
      this->castType = castType;
      this->harqEnabled = harqEnabled;
      this->pdb = pdb;
      this->dynamic = dynamic;
      this->rri = rri;
    }
    uint32_t dstL2Id {std::numeric_limits <uint32_t>::max ()}; //!< L2 destination id
    uint8_t  lcId {std::numeric_limits <uint8_t>::max ()};     //!< logical channel identifier
    uint8_t  lcGroup {std::numeric_limits <uint8_t>::max ()};  //!< logical channel group
    uint8_t  pqi {std::numeric_limits <uint8_t>::max ()};      //!< PC5 QoS Class Identifier
    uint8_t  priority {std::numeric_limits <uint8_t>::max ()};  //!< priority
    bool     isGbr {false};   //!< true if the bearer is GBR, false if the bearer is NON-GBR
    uint64_t mbr {0};   //!< maximum bitrate
    uint64_t gbr {0};   //!< guaranteed bitrate
    SidelinkInfo::CastType castType {SidelinkInfo::CastType::Invalid}; //!< cast type
    bool harqEnabled {false};  //!< Whether HARQ is enabled
    Time pdb;   //!< Packet Delay Budget
    bool dynamic {false};   //!< flag for whether it is dynamic or SPS
    Time rri;  //!< Resource Reservation Interval
  };

  /**
   * \brief Send the NR Sidelink logical channel configuration from UE MAC to the UE scheduler
   *
   * \param params NrSlUeMacCschedSapProvider::SidelinkLogicalChannelInfo
   */
  virtual void CschedUeNrSlLcConfigReq (const struct NrSlUeMacCschedSapProvider::SidelinkLogicalChannelInfo& params) = 0;
};

/**
 * \ingroup scheduler
 *
 * \brief The Interface between NR SL UE Scheduler and NR UE MAC
 */
class NrSlUeMacCschedSapUser
{
public:
  /**
   * \brief ~NrSlUeMacCschedSapUser
   */
  virtual ~NrSlUeMacCschedSapUser () = default;
  /**
   * \brief Send the confirmation about the successful configuration of LC
   *        to the UE MAC.
   * \param lcg The logical group
   * \param lcId The Logical Channel id
   */
  virtual void  CschedUeNrSlLcConfigCnf (uint8_t lcg, uint8_t lcId) = 0;

};

/**
 * \brief Stream output operator for SidelinkLogicalChannelInfo
 * \param os output stream
 * \param p struct whose parameter to output
 * \return updated stream
 */
std::ostream & operator<< (std::ostream & os, NrSlUeMacCschedSapProvider::SidelinkLogicalChannelInfo const & p);

} // namespace ns3

#endif /* NR_SL_UE_MAC_CSCHED_SAP_PROVIDER_H */
