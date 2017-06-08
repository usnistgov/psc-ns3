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
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#ifndef NIST_LTE_RLC_SDU_STATUS_TAG_H
#define NIST_LTE_RLC_SDU_STATUS_TAG_H

#include "ns3/tag.h"

namespace ns3 {


/**
 * \brief This class implements a tag that carries the status of a RLC SDU
 * for the fragmentation process
 * NistStatus of RLC SDU
 */
class NistLteRlcSduNistStatusTag : public Tag
{
public:
  NistLteRlcSduNistStatusTag ();

  void SetNistStatus (uint8_t status);
  uint8_t GetNistStatus (void) const;

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  typedef enum { FULL_SDU       = 1,
                 FIRST_SEGMENT  = 2,
                 MIDDLE_SEGMENT = 3,
                 LAST_SEGMENT   = 4,
                 ANY_SEGMENT    = 5 } SduNistStatus_t;

private:
  uint8_t m_sduNistStatus;
};


}; // namespace ns3

#endif // NIST_LTE_RLC_SDU_STATUS_TAG_H
