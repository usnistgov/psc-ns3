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

#ifndef NIST_LTE_FFR_SOFT_ALGORITHM_H
#define NIST_LTE_FFR_SOFT_ALGORITHM_H

#include <ns3/nist-lte-ffr-algorithm.h>
#include <ns3/nist-lte-ffr-sap.h>
#include <ns3/nist-lte-ffr-rrc-sap.h>
#include <ns3/nist-lte-rrc-sap.h>
#include <map>

namespace ns3 {


/**
 * \brief Soft Fractional Frequency Reuse algorithm implementation
 */
class NistLteFfrSoftAlgorithm : public NistLteFfrAlgorithm
{
public:
  /**
   * \brief Creates a trivial ffr algorithm instance.
   */
  NistLteFfrSoftAlgorithm ();

  virtual ~NistLteFfrSoftAlgorithm ();

  // inherited from Object
  static TypeId GetTypeId ();

  // inherited from NistLteFfrAlgorithm
  virtual void SetNistLteFfrSapUser (NistLteFfrSapUser* s);
  virtual NistLteFfrSapProvider* GetNistLteFfrSapProvider ();

  virtual void SetNistLteFfrRrcSapUser (NistLteFfrRrcSapUser* s);
  virtual NistLteFfrRrcSapProvider* GetNistLteFfrRrcSapProvider ();

  // let the forwarder class access the protected and private members
  friend class MemberNistLteFfrSapProvider<NistLteFfrSoftAlgorithm>;
  friend class MemberNistLteFfrRrcSapProvider<NistLteFfrSoftAlgorithm>;

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

  uint8_t m_dlCommonSubBandwidth;
  uint8_t m_dlEgdeSubBandOffset;
  uint8_t m_dlEdgeSubBandwidth;

  uint8_t m_ulCommonSubBandwidth;
  uint8_t m_ulEgdeSubBandOffset;
  uint8_t m_ulEdgeSubBandwidth;

  std::vector <bool> m_dlRbgMap;
  std::vector <bool> m_ulRbgMap;

  std::vector <bool> m_dlCenterRbgMap;
  std::vector <bool> m_ulCenterRbgMap;

  std::vector <bool> m_dlMediumRbgMap;
  std::vector <bool> m_ulMediumRbgMap;

  std::vector <bool> m_dlEdgeRbgMap;
  std::vector <bool> m_ulEdgeRbgMap;

  enum UePosition
  {
    AreaUnset,
    CenterArea,
    MediumArea,
    EdgeArea
  };

  std::map< uint16_t, uint8_t > m_ues;

  uint8_t m_centerSubBandThreshold;
  uint8_t m_egdeSubBandThreshold;

  uint8_t m_centerAreaPowerOffset;
  uint8_t m_mediumAreaPowerOffset;
  uint8_t m_edgeAreaPowerOffset;

  uint8_t m_centerAreaTpc;
  uint8_t m_mediumAreaTpc;
  uint8_t m_edgeAreaTpc;

  // The expected measurement identity
  uint8_t m_measId;

}; // end of class NistLteFfrSoftAlgorithm


} // end of namespace ns3


#endif /* NIST_LTE_FFR_SOFT_ALGORITHM_H */
