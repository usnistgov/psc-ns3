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

#ifndef NIST_LTE_FR_NO_OP_ALGORITHM_H
#define NIST_LTE_FR_NO_OP_ALGORITHM_H

#include <ns3/nist-lte-ffr-algorithm.h>
#include <ns3/nist-lte-ffr-sap.h>
#include <ns3/nist-lte-ffr-rrc-sap.h>
#include <ns3/nist-lte-rrc-sap.h>

namespace ns3 {


/**
 * \brief FR algorithm implementation which simply does nothing.
 *
 * Selecting this FR algorithm is equivalent to disabling FFR.
 * This is the default choice.
 *
 * To enable FR, please select another FR algorithm, i.e.,
 * another child class of NistLteFfrAlgorithm.
 */
class NistLteFrNoOpAlgorithm : public NistLteFfrAlgorithm
{
public:
  /**
   * \brief Creates a NoOP FR algorithm instance.
   */
  NistLteFrNoOpAlgorithm ();

  virtual ~NistLteFrNoOpAlgorithm ();

  // inherited from Object
  static TypeId GetTypeId ();

  // inherited from NistLteFfrAlgorithm
  virtual void SetNistLteFfrSapUser (NistLteFfrSapUser* s);
  virtual NistLteFfrSapProvider* GetNistLteFfrSapProvider ();

  virtual void SetNistLteFfrRrcSapUser (NistLteFfrRrcSapUser* s);
  virtual NistLteFfrRrcSapProvider* GetNistLteFfrRrcSapProvider ();

  // let the forwarder class access the protected and private members
  friend class MemberNistLteFfrSapProvider<NistLteFrNoOpAlgorithm>;
  friend class MemberNistLteFfrRrcSapProvider<NistLteFrNoOpAlgorithm>;

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
  // FFR SAP
  NistLteFfrSapUser* m_ffrSapUser;
  NistLteFfrSapProvider* m_ffrSapProvider;

  // FFR RRF SAP
  NistLteFfrRrcSapUser* m_ffrRrcSapUser;
  NistLteFfrRrcSapProvider* m_ffrRrcSapProvider;

}; // end of class NistLteFrNoOpAlgorithm


} // end of namespace ns3


#endif /* LTE_FFR_NO_OP_ALGORITHM_H */
