/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Piotr Gawlowicz
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
 * Author: Piotr Gawlowicz <gawlowicz.p@gmail.com>
 *
 */

#ifndef NIST_LTE_FR_HARD_ALGORITHM_H
#define NIST_LTE_FR_HARD_ALGORITHM_H

#include <ns3/nist-lte-ffr-algorithm.h>
#include <ns3/nist-lte-ffr-sap.h>
#include <ns3/nist-lte-ffr-rrc-sap.h>
#include <ns3/nist-lte-rrc-sap.h>

namespace ns3 {


/**
 * \brief Hard Frequency Reuse algorithm implementation which uses only 1 sub-band.
 */
class NistLteFrHardAlgorithm : public NistLteFfrAlgorithm
{
public:
  /**
   * \brief Creates a trivial ffr algorithm instance.
   */
  NistLteFrHardAlgorithm ();

  virtual ~NistLteFrHardAlgorithm ();

  // inherited from Object
  static TypeId GetTypeId ();

  // inherited from NistLteFfrAlgorithm
  virtual void SetNistLteFfrSapUser (NistLteFfrSapUser* s);
  virtual NistLteFfrSapProvider* GetNistLteFfrSapProvider ();

  virtual void SetNistLteFfrRrcSapUser (NistLteFfrRrcSapUser* s);
  virtual NistLteFfrRrcSapProvider* GetNistLteFfrRrcSapProvider ();

  // let the forwarder class access the protected and private members
  friend class MemberNistLteFfrSapProvider<NistLteFrHardAlgorithm>;
  friend class MemberNistLteFfrRrcSapProvider<NistLteFrHardAlgorithm>;

protected:
  // inherited from Object
  virtual void DoInitialize ();
  virtual void DoDispose ();

  virtual void Reconfigure ();

  // FFR SAP PROVIDER IMPLEMENTATION
  virtual std::vector <bool> DoGetAvailableDlRbg ();
  virtual bool DoIsDlRbgAvailableForUe (int i, uint16_t rnti);
  virtual std::vector <bool> DoGetAvailableUlRbg ();
  virtual bool DoIsUlRbgAvailableForUe (int i, uint16_t rnti);
  virtual void DoReportDlCqiInfo (const struct NistFfMacSchedSapProvider::NistSchedDlCqiInfoReqParameters& params);
  virtual void DoReportUlCqiInfo (const struct NistFfMacSchedSapProvider::NistSchedUlCqiInfoReqParameters& params);
  virtual void DoReportUlCqiInfo ( std::map <uint16_t, std::vector <double> > ulCqiMap );
  virtual uint8_t DoGetTpc (uint16_t rnti);
  virtual uint8_t DoGetMinContinuousUlBandwidth ();

  // FFR SAP RRC PROVIDER IMPLEMENTATION
  virtual void DoReportUeMeas (uint16_t rnti, NistLteRrcSap::NistMeasResults measResults);
  virtual void DoRecvLoadInformation (NistEpcX2Sap::NistLoadInformationParams params);

private:
  void SetDownlinkConfiguration (uint16_t cellId, uint8_t bandwidth);
  void SetUplinkConfiguration (uint16_t cellId, uint8_t bandwidth);
  void InitializeDownlinkRbgMaps ();
  void InitializeUplinkRbgMaps ();

  // FFR SAP
  NistLteFfrSapUser* m_ffrSapUser;
  NistLteFfrSapProvider* m_ffrSapProvider;

  // FFR RRF SAP
  NistLteFfrRrcSapUser* m_ffrRrcSapUser;
  NistLteFfrRrcSapProvider* m_ffrRrcSapProvider;

  uint8_t m_dlOffset;
  uint8_t m_dlSubBand;

  uint8_t m_ulOffset;
  uint8_t m_ulSubBand;

  std::vector <bool> m_dlRbgMap;
  std::vector <bool> m_ulRbgMap;

}; // end of class NistLteFrHardAlgorithm


} // end of namespace ns3


#endif /* NIST_LTE_FR_HARD_ALGORITHM_H */
