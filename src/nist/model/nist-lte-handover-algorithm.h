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

#ifndef NIST_LTE_HANDOVER_ALGORITHM_H
#define NIST_LTE_HANDOVER_ALGORITHM_H

#include <ns3/object.h>
#include <ns3/nist-lte-rrc-sap.h>

namespace ns3 {


class NistLteHandoverManagementSapUser;
class NistLteHandoverManagementSapProvider;


/**
 * \brief The abstract base class of a handover algorithm that operates using
 *        the Handover Management SAP interface.
 *
 * Handover algorithm receives measurement reports from an eNode RRC instance
 * and tells the eNodeB RRC instance when to do a handover.
 *
 * This class is an abstract class intended to be inherited by subclasses that
 * implement its virtual methods. By inheriting from this abstract class, the
 * subclasses gain the benefits of being compatible with the NistLteEnbNetDevice
 * class, being accessible using namespace-based access through ns-3 Config
 * subsystem, and being installed and configured by NistLteHelper class (see
 * NistLteHelper::SetHandoverAlgorithmType and
 * NistLteHelper::SetHandoverAlgorithmAttribute methods).
 *
 * The communication with the eNodeB RRC instance is done through the *Handover
 * Management SAP* interface. The handover algorithm instance corresponds to the
 * "provider" part of this interface, while the eNodeB RRC instance takes the
 * role of the "user" part. The following code skeleton establishes the
 * connection between both instances:
 *
 *     Ptr<NistLteEnbRrc> u = ...;
 *     Ptr<NistLteHandoverAlgorithm> p = ...;
 *     u->SetNistLteHandoverManagementSapProvider (p->GetNistLteHandoverManagementSapProvider ());
 *     p->SetNistLteHandoverManagementSapUser (u->GetNistLteHandoverManagementSapUser ());
 *
 * However, user rarely needs to use the above code, since it has already been
 * taken care by NistLteHelper::InstallEnbDevice.
 *
 * \sa NistLteHandoverManagementSapProvider, NistLteHandoverManagementSapUser
 */
class NistLteHandoverAlgorithm : public Object
{
public:
  NistLteHandoverAlgorithm ();
  virtual ~NistLteHandoverAlgorithm ();

  // inherited from Object
  static TypeId GetTypeId ();

  /**
   * \brief Set the "user" part of the Handover Management SAP interface that
   *        this handover algorithm instance will interact with.
   * \param s a reference to the "user" part of the interface, typically a
   *          member of an NistLteEnbRrc instance
   */
  virtual void SetNistLteHandoverManagementSapUser (NistLteHandoverManagementSapUser* s) = 0;

  /**
   * \brief Export the "provider" part of the Handover Management SAP interface.
   * \return the reference to the "provider" part of the interface, typically to
   *         be kept by an NistLteEnbRrc instance
   */
  virtual NistLteHandoverManagementSapProvider* GetNistLteHandoverManagementSapProvider () = 0;

protected:

  // inherited from Object
  virtual void DoDispose ();

  // HANDOVER MANAGEMENT SAP PROVIDER IMPLEMENTATION

  /**
   * \brief Implementation of NistLteHandoverManagementSapProvider::ReportUeMeas.
   * \param rnti Radio Network Temporary Identity, an integer identifying the UE
   *             where the report originates from
   * \param measResults a single report of one measurement identity
   */
  virtual void DoReportUeMeas (uint16_t rnti, NistLteRrcSap::NistMeasResults measResults) = 0;

}; // end of class NistLteHandoverAlgorithm


} // end of namespace ns3


#endif /* NIST_LTE_HANDOVER_ALGORITHM_H */
