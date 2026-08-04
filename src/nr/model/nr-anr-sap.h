// Copyright (c) 2013 Budiarto Herman
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Budiarto Herman <budiarto.herman@magister.fi>

#ifndef NR_ANR_SAP_H
#define NR_ANR_SAP_H

#include "nr-rrc-sap.h"

namespace ns3
{

/**
 * \brief Service Access Point (SAP) offered by the ANR instance to the eNodeB
 *        RRC instance.
 *
 * This is the *ANR SAP Provider*, i.e., the part of the SAP that contains the
 * ANR (Automatic Neighbour Relation) methods called by the eNodeB RRC instance.
 */
class NrAnrSapProvider
{
  public:
    virtual ~NrAnrSapProvider() = default;

    /**
     * \brief Send a UE measurement report to the ANC instance.
     * \param measResults a single report of one measurement identity
     *
     * The received measurement report is a result of the UE measurement
     * configuration previously configured by calling
     * NrAnrSapUser::AddUeMeasReportConfigForAnr. The report may be stored and
     * utilized for the purpose of maintaining Neighbour Relation Table (NRT).
     */
    virtual void ReportUeMeas(NrRrcSap::MeasResults measResults) = 0;

    /**
     * \brief Add a new Neighbour Relation entry.
     * \param cellId the Physical Cell ID of the new neighbouring cell
     */
    virtual void AddNeighbourRelation(uint16_t cellId) = 0;

    /**
     * \brief Get the value of *No Remove* field of a neighbouring cell from the
     *        Neighbour Relation Table (NRT).
     * \param cellId the Physical Cell ID of the neighbouring cell of interest
     * \return if true, the Neighbour Relation shall *not* be removed from the NRT
     */
    virtual bool GetNoRemove(uint16_t cellId) const = 0;

    /**
     * \brief Get the value of *No HO* field of a neighbouring cell from the
     *        Neighbour Relation Table (NRT).
     * \param cellId the Physical Cell ID of the neighbouring cell of interest
     * \return if true, the Neighbour Relation shall *not* be used by the eNodeB
     *         for handover reasons
     */
    virtual bool GetNoHo(uint16_t cellId) const = 0;

    /**
     * \brief Get the value of *No X2* field of a neighbouring cell from the
     *        Neighbour Relation Table (NRT).
     * \param cellId the Physical Cell ID of the neighbouring cell of interest
     * \return if true, the Neighbour Relation shall *not* use an X2 interface in
     *         order to initiate procedures towards the eNodeB parenting the
     *         target cell
     */
    virtual bool GetNoX2(uint16_t cellId) const = 0;

}; // end of class NrAnrSapProvider

/**
 * \brief Service Access Point (SAP) offered by the eNodeB RRC instance to the
 *        ANR instance.
 *
 * This is the *ANR SAP User*, i.e., the part of the SAP that contains the
 * eNodeB RRC methods called by the ANR (Automatic Neighbour Relation) instance.
 */
class NrAnrSapUser
{
  public:
    virtual ~NrAnrSapUser() = default;

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
     * instance through the NrAnrSapProvider::ReportUeMeas SAP function.
     *
     * \note This function is only valid before the simulation begins.
     */
    virtual uint8_t AddUeMeasReportConfigForAnr(NrRrcSap::ReportConfigEutra reportConfig) = 0;

}; // end of class NrAnrSapUser

/**
 * \brief Template for the implementation of the NrAnrSapProvider as a member
 *        of an owner class of type C to which all methods are forwarded.
 */
template <class C>
class MemberNrAnrSapProvider : public NrAnrSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrAnrSapProvider(C* owner);

    // Delete default constructor to avoid misuse
    MemberNrAnrSapProvider() = delete;

    // inherited from NrAnrSapProvider
    void ReportUeMeas(NrRrcSap::MeasResults measResults) override;
    void AddNeighbourRelation(uint16_t cellId) override;
    bool GetNoRemove(uint16_t cellId) const override;
    bool GetNoHo(uint16_t cellId) const override;
    bool GetNoX2(uint16_t cellId) const override;

  private:
    C* m_owner; ///< the owner class

}; // end of class MemberNrAnrSapProvider

template <class C>
MemberNrAnrSapProvider<C>::MemberNrAnrSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
void
MemberNrAnrSapProvider<C>::ReportUeMeas(NrRrcSap::MeasResults measResults)
{
    m_owner->DoReportUeMeas(measResults);
}

template <class C>
void
MemberNrAnrSapProvider<C>::AddNeighbourRelation(uint16_t cellId)
{
    m_owner->DoAddNeighbourRelation(cellId);
}

template <class C>
bool
MemberNrAnrSapProvider<C>::GetNoRemove(uint16_t cellId) const
{
    return m_owner->DoGetNoRemove(cellId);
}

template <class C>
bool
MemberNrAnrSapProvider<C>::GetNoHo(uint16_t cellId) const
{
    return m_owner->DoGetNoHo(cellId);
}

template <class C>
bool
MemberNrAnrSapProvider<C>::GetNoX2(uint16_t cellId) const
{
    return m_owner->DoGetNoX2(cellId);
}

/**
 * \brief Template for the implementation of the NrAnrSapUser as a member of an
 *        owner class of type C to which all methods are forwarded.
 */
template <class C>
class MemberNrAnrSapUser : public NrAnrSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrAnrSapUser(C* owner);

    // Delete default constructor to avoid misuse
    MemberNrAnrSapUser() = delete;

    // inherited from NrAnrSapUser
    uint8_t AddUeMeasReportConfigForAnr(NrRrcSap::ReportConfigEutra reportConfig) override;

  private:
    C* m_owner; ///< the owner class

}; // end of class MemberNrAnrSapUser

template <class C>
MemberNrAnrSapUser<C>::MemberNrAnrSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
uint8_t
MemberNrAnrSapUser<C>::AddUeMeasReportConfigForAnr(NrRrcSap::ReportConfigEutra reportConfig)
{
    return m_owner->DoAddUeMeasReportConfigForAnr(reportConfig);
}

} // end of namespace ns3

#endif /* NR_ANR_SAP_H */
