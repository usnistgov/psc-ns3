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

#ifndef NIST_A3_RSRP_HANDOVER_ALGORITHM_H
#define NIST_A3_RSRP_HANDOVER_ALGORITHM_H

#include <ns3/nist-lte-handover-algorithm.h>
#include <ns3/nist-lte-handover-management-sap.h>
#include <ns3/nist-lte-rrc-sap.h>
#include <ns3/nstime.h>

namespace ns3 {


/**
 * \brief Implementation of the strongest cell handover algorithm, based on RSRP
 *        measurements and Event A3.
 *
 * The algorithm utilizes Event A3 (Section 5.5.4.4 of 3GPP TS 36.331) UE
 * measurements and the Reference Signal Reference Power (RSRP). It is defined
 * as the event when the UE perceives that a neighbour cell's RSRP is better
 * than the serving cell's RSRP.
 *
 * Handover margin (a.k.a. hysteresis) and time-to-trigger (TTT) can be
 * configured to delay the event triggering. The values of these parameters
 * apply to all attached UEs.
 *
 * The following code snippet is an example of using and configuring the
 * handover algorithm in a simulation program:
 *
 *     Ptr<NistLteHelper> lteHelper = CreateObject<NistLteHelper> ();
 *
 *     NodeContainer enbNodes;
 *     // configure the nodes here...
 *
 *     lteHelper->SetHandoverAlgorithmType ("ns3::NistA3RsrpHandoverAlgorithm");
 *     lteHelper->SetHandoverAlgorithmAttribute ("Hysteresis",
 *                                               DoubleValue (3.0));
 *     lteHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger",
 *                                               TimeValue (MilliSeconds (256)));
 *     NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
 *
 * \note Setting the handover algorithm type and attributes after the call to
 *       NistLteHelper::InstallEnbDevice does not have any effect to the devices
 *       that have already been installed.
 */
class NistA3RsrpHandoverAlgorithm : public NistLteHandoverAlgorithm
{
public:
  /// Creates a strongest cell handover algorithm instance.
  NistA3RsrpHandoverAlgorithm ();

  virtual ~NistA3RsrpHandoverAlgorithm ();

  // inherited from Object
  static TypeId GetTypeId ();

  // inherited from NistLteHandoverAlgorithm
  virtual void SetNistLteHandoverManagementSapUser (NistLteHandoverManagementSapUser* s);
  virtual NistLteHandoverManagementSapProvider* GetNistLteHandoverManagementSapProvider ();

  // let the forwarder class access the protected and private members
  friend class MemberNistLteHandoverManagementSapProvider<NistA3RsrpHandoverAlgorithm>;

protected:
  // inherited from Object
  virtual void DoInitialize ();
  virtual void DoDispose ();

  // inherited from NistLteHandoverAlgorithm as a Handover Management SAP implementation
  void DoReportUeMeas (uint16_t rnti, NistLteRrcSap::NistMeasResults measResults);

private:
  /**
   * Determines if a neighbour cell is a valid destination for handover.
   * Currently always return true.
   *
   * \param cellId The cell ID of the neighbour cell.
   * \return True if the cell is a valid destination for handover.
   */
  bool IsValidNeighbour (uint16_t cellId);

  /// The expected measurement identity for A3 measurements.
  uint8_t m_measId;

  /**
   * The `Hysteresis` attribute. Handover margin (hysteresis) in dB (rounded to
   * the nearest multiple of 0.5 dB).
   */
  double m_hysteresisDb;
  /**
   * The `TimeToTrigger` attribute. Time during which neighbour cell's RSRP
   * must continuously higher than serving cell's RSRP "
   */
  Time m_timeToTrigger;

  /// Interface to the eNodeB RRC instance.
  NistLteHandoverManagementSapUser* m_handoverManagementSapUser;
  /// Receive API calls from the eNodeB RRC instance.
  NistLteHandoverManagementSapProvider* m_handoverManagementSapProvider;

}; // end of class NistA3RsrpHandoverAlgorithm


} // end of namespace ns3


#endif /* NIST_A3_RSRP_HANDOVER_ALGORITHM_H */
