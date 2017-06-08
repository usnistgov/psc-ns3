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

#ifndef NIST_LTE_ANR_SAP_H
#define NIST_LTE_ANR_SAP_H

#include <ns3/nist-lte-rrc-sap.h>

namespace ns3 {


/**
 * \brief Service Access Point (SAP) offered by the ANR instance to the eNodeB
 *        RRC instance.
 *
 * This is the *ANR SAP Provider*, i.e., the part of the SAP that contains the
 * ANR (Automatic Neighbour Relation) methods called by the eNodeB RRC instance.
 */
class NistLteAnrSapProvider
{
public:
  virtual ~NistLteAnrSapProvider ();

  /**
   * \brief Send a UE measurement report to the ANC instance.
   * \param measResults a single report of one measurement identity
   *
   * The received measurement report is a result of the UE measurement
   * configuration previously configured by calling
   * NistLteAnrSapUser::AddUeMeasReportConfigForAnr. The report may be stored and
   * utilized for the purpose of maintaining Neighbour Relation Table (NRT).
   */
  virtual void ReportUeMeas (NistLteRrcSap::NistMeasResults measResults) = 0;

  /**
   * \brief Add a new Neighbour Relation entry.
   * \param cellId the Physical Cell ID of the new neighbouring cell
   */
  virtual void AddNeighbourRelation (uint16_t cellId) = 0;

  /**
   * \brief Get the value of *No Remove* field of a neighbouring cell from the
   *        Neighbour Relation Table (NRT).
   * \param cellId the Physical Cell ID of the neighbouring cell of interest
   * \return if true, the Neighbour Relation shall *not* be removed from the NRT
   */
  virtual bool GetNoRemove (uint16_t cellId) const = 0;

  /**
   * \brief Get the value of *No HO* field of a neighbouring cell from the
   *        Neighbour Relation Table (NRT).
   * \param cellId the Physical Cell ID of the neighbouring cell of interest
   * \return if true, the Neighbour Relation shall *not* be used by the eNodeB
   *         for handover reasons
   */
  virtual bool GetNoHo (uint16_t cellId) const = 0;

  /**
   * \brief Get the value of *No X2* field of a neighbouring cell from the
   *        Neighbour Relation Table (NRT).
   * \param cellId the Physical Cell ID of the neighbouring cell of interest
   * \return if true, the Neighbour Relation shall *not* use an X2 interface in
   *         order to initiate procedures towards the eNodeB parenting the
   *         target cell
   */
  virtual bool GetNoX2 (uint16_t cellId) const = 0;

}; // end of class NistLteAnrSapProvider



/**
 * \brief Service Access Point (SAP) offered by the eNodeB RRC instance to the
 *        ANR instance.
 *
 * This is the *ANR SAP User*, i.e., the part of the SAP that contains the
 * eNodeB RRC methods called by the ANR (Automatic Neighbour Relation) instance.
 */
class NistLteAnrSapUser
{
public:
  virtual ~NistLteAnrSapUser ();

  /**
   * \brief Request a certain reporting configuration to be fulfilled by the UEs
   *        attached to the eNodeB entity.
   * \param reportConfig the UE measurement reporting configuration
   * \return the measurement identity associated with this newly added
   *         reporting configuration
   *
   * The eNodeB RRC entity is expected to configure the same reporting
   * configuration in each of the attached UEs. When later in the simulation a
   * UE measurement report is received from a UE as a result of this
   * configuration, the eNodeB RRC entity shall forward this report to the ANC
   * instance through the NistLteAnrSapProvider::ReportUeMeas SAP function.
   *
   * \note This function is only valid before the simulation begins.
   */
  virtual uint8_t AddUeMeasReportConfigForAnr (NistLteRrcSap::NistReportConfigEutra reportConfig) = 0;

}; // end of class NistLteAnrSapUser



/**
 * \brief Template for the implementation of the NistLteAnrSapProvider as a member
 *        of an owner class of type C to which all methods are forwarded.
 */
template <class C>
class MemberNistLteAnrSapProvider : public NistLteAnrSapProvider
{
public:
  MemberNistLteAnrSapProvider (C* owner);

  // inherited from NistLteAnrSapProvider
  virtual void ReportUeMeas (NistLteRrcSap::NistMeasResults measResults);
  virtual void AddNeighbourRelation (uint16_t cellId);
  virtual bool GetNoRemove (uint16_t cellId) const;
  virtual bool GetNoHo (uint16_t cellId) const;
  virtual bool GetNoX2 (uint16_t cellId) const;

private:
  MemberNistLteAnrSapProvider ();
  C* m_owner;

}; // end of class MemberNistLteAnrSapProvider


template <class C>
MemberNistLteAnrSapProvider<C>::MemberNistLteAnrSapProvider (C* owner)
  : m_owner (owner)
{
}


template <class C>
void
MemberNistLteAnrSapProvider<C>::ReportUeMeas (NistLteRrcSap::NistMeasResults measResults)
{
  m_owner->DoReportUeMeas (measResults);
}


template <class C>
void
MemberNistLteAnrSapProvider<C>::AddNeighbourRelation (uint16_t cellId)
{
  m_owner->DoAddNeighbourRelation (cellId);
}


template <class C>
bool
MemberNistLteAnrSapProvider<C>::GetNoRemove (uint16_t cellId) const
{
  return m_owner->DoGetNoRemove (cellId);
}


template <class C>
bool
MemberNistLteAnrSapProvider<C>::GetNoHo (uint16_t cellId) const
{
  return m_owner->DoGetNoHo (cellId);
}


template <class C>
bool
MemberNistLteAnrSapProvider<C>::GetNoX2 (uint16_t cellId) const
{
  return m_owner->DoGetNoX2 (cellId);
}



/**
 * \brief Template for the implementation of the NistLteAnrSapUser as a member of an
 *        owner class of type C to which all methods are forwarded.
 */
template <class C>
class MemberNistLteAnrSapUser : public NistLteAnrSapUser
{
public:
  MemberNistLteAnrSapUser (C* owner);

  // inherited from NistLteAnrSapUser
  virtual uint8_t AddUeMeasReportConfigForAnr (NistLteRrcSap::NistReportConfigEutra reportConfig);

private:
  MemberNistLteAnrSapUser ();
  C* m_owner;

}; // end of class MemberNistLteAnrSapUser


template <class C>
MemberNistLteAnrSapUser<C>::MemberNistLteAnrSapUser (C* owner)
  : m_owner (owner)
{
}


template <class C>
uint8_t
MemberNistLteAnrSapUser<C>::AddUeMeasReportConfigForAnr (NistLteRrcSap::NistReportConfigEutra reportConfig)
{
  return m_owner->DoAddUeMeasReportConfigForAnr (reportConfig);
}


} // end of namespace ns3


#endif /* NIST_LTE_ANR_SAP_H */
