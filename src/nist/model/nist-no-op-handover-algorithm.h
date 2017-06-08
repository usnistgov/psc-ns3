/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Budiarto Herman
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
 * Author: Budiarto Herman <budiarto.herman@magister.fi>
 *
 */

#ifndef NIST_NO_OP_HANDOVER_ALGORITHM_H
#define NIST_NO_OP_HANDOVER_ALGORITHM_H

#include <ns3/nist-lte-handover-algorithm.h>
#include <ns3/nist-lte-handover-management-sap.h>
#include <ns3/nist-lte-rrc-sap.h>

namespace ns3 {


/**
 * \brief Handover algorithm implementation which simply does nothing.
 *
 * Selecting this handover algorithm is equivalent to disabling automatic
 * triggering of handover. This is the default choice.
 *
 * To enable automatic handover, please select another handover algorithm, i.e.,
 * another child class of NistLteHandoverAlgorithm.
 */
class NistNoOpHandoverAlgorithm : public NistLteHandoverAlgorithm
{
public:
  /// Creates a No-op handover algorithm instance.
  NistNoOpHandoverAlgorithm ();

  virtual ~NistNoOpHandoverAlgorithm ();

  // inherited from Object
  static TypeId GetTypeId ();

  // inherited from NistLteHandoverAlgorithm
  virtual void SetNistLteHandoverManagementSapUser (NistLteHandoverManagementSapUser* s);
  virtual NistLteHandoverManagementSapProvider* GetNistLteHandoverManagementSapProvider ();

  // let the forwarder class access the protected and private members
  friend class MemberNistLteHandoverManagementSapProvider<NistNoOpHandoverAlgorithm>;

protected:
  // inherited from Object
  virtual void DoInitialize ();
  virtual void DoDispose ();

  // inherited from NistLteHandoverAlgorithm as a Handover Management SAP implementation
  void DoReportUeMeas (uint16_t rnti, NistLteRrcSap::NistMeasResults measResults);

private:
  /// Interface to the eNodeB RRC instance.
  NistLteHandoverManagementSapUser* m_handoverManagementSapUser;
  /// Receive API calls from the eNodeB RRC instance.
  NistLteHandoverManagementSapProvider* m_handoverManagementSapProvider;

}; // end of class NistNoOpHandoverAlgorithm


} // end of namespace ns3


#endif /* NIST_NO_OP_HANDOVER_ALGORITHM_H */
