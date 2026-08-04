// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_FH_SCHED_SAP_H
#define NR_FH_SCHED_SAP_H

#include "nr-mac-scheduler-ns3.h"
#include "nr-mac-scheduler-ue-info.h"

// #include "nr-fh-control.h"

namespace ns3
{

/**
 * \brief Service Access Point (SAP) offered by the FhControl instance
 *        to the MAC Scheduler instance.
 *
 * This is the *NrFhSchedSapProvider*, i.e., the part of the SAP that
 * contains the FhControl methods called by the MAC Scheduler instance.
 */

class NrFhSchedSapProvider
{
  public:
    virtual ~NrFhSchedSapProvider();

    virtual bool DoesAllocationFit(uint16_t bwpId,
                                   uint32_t mcs,
                                   uint32_t nRegs,
                                   uint8_t dlRank) = 0;
    virtual uint8_t GetFhControlMethod() = 0;
    virtual uint16_t GetNrFhPhysicalCellId() = 0;
    virtual void SetActiveUe(uint16_t bwpId, uint16_t rnti, uint32_t bytes) = 0;
    virtual void SetActiveHarqUes(uint16_t bwpId, uint16_t rnti) = 0;
    virtual void UpdateActiveUesMap(
        uint16_t bwpId,
        const std::deque<VarTtiAllocInfo>& allocation,
        const std::unordered_map<uint16_t, std::shared_ptr<NrMacSchedulerUeInfo>>& ueMap) = 0;
    virtual uint8_t GetMaxMcsAssignable(uint16_t bwpId,
                                        uint32_t reg,
                                        uint32_t rnti,
                                        uint8_t dlRank) = 0;
    virtual uint32_t GetMaxRegAssignable(uint16_t bwpId,
                                         uint32_t mcs,
                                         uint32_t rnti,
                                         uint8_t dlRank) = 0;
};

/**
 * \brief Service Access Point (SAP) offered by the MAC Scheduler instance
 * to the FhControl instance.
 *
 * This is the *NrFhSchedSapUser*, i.e., the part of the SAP that contains the
 * gnb PHY methods called by the FhControl instance.
 */

class NrFhSchedSapUser
{
  public:
    virtual ~NrFhSchedSapUser();

    virtual uint64_t GetNumRbPerRbgFromSched() = 0;
};

/**
 * \brief Template for the implementation of the NrFhSchedSapProvider as a member of
 *        an owner class of type C to which all methods are forwarded.
 */

template <class C>
class MemberNrFhSchedSapProvider : public NrFhSchedSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrFhSchedSapProvider(C* owner);

    MemberNrFhSchedSapProvider() = delete;

    bool DoesAllocationFit(uint16_t bwpId, uint32_t mcs, uint32_t nRegs, uint8_t dlRank) override;
    uint8_t GetFhControlMethod() override;
    uint16_t GetNrFhPhysicalCellId() override;
    void SetActiveUe(uint16_t bwpId, uint16_t rnti, uint32_t bytes) override;
    void SetActiveHarqUes(uint16_t bwpId, uint16_t rnti) override;
    void UpdateActiveUesMap(
        uint16_t bwpId,
        const std::deque<VarTtiAllocInfo>& allocation,
        const std::unordered_map<uint16_t, std::shared_ptr<NrMacSchedulerUeInfo>>& ueMap) override;
    uint8_t GetMaxMcsAssignable(uint16_t bwpId,
                                uint32_t reg,
                                uint32_t rnti,
                                uint8_t dlRank) override;
    uint32_t GetMaxRegAssignable(uint16_t bwpId,
                                 uint32_t mcs,
                                 uint32_t rnti,
                                 uint8_t dlRank) override;

  private:
    C* m_owner; ///< the owner class

}; // end of class MemberNrFhSchedSapProvider

template <class C>
MemberNrFhSchedSapProvider<C>::MemberNrFhSchedSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
bool
MemberNrFhSchedSapProvider<C>::DoesAllocationFit(uint16_t bwpId,
                                                 uint32_t mcs,
                                                 uint32_t nRegs,
                                                 uint8_t dlRank)
{
    return m_owner->DoGetDoesAllocationFit(bwpId, mcs, nRegs, dlRank);
}

template <class C>
uint8_t
MemberNrFhSchedSapProvider<C>::GetFhControlMethod()
{
    return m_owner->DoGetFhControlMethod();
}

template <class C>
uint16_t
MemberNrFhSchedSapProvider<C>::GetNrFhPhysicalCellId()
{
    return m_owner->DoGetPhysicalCellId();
}

template <class C>
void
MemberNrFhSchedSapProvider<C>::SetActiveUe(uint16_t bwpId, uint16_t rnti, uint32_t bytes)
{
    return m_owner->DoSetActiveUe(bwpId, rnti, bytes);
}

template <class C>
void
MemberNrFhSchedSapProvider<C>::SetActiveHarqUes(uint16_t bwpId, uint16_t rnti)
{
    return m_owner->DoSetActiveHarqUes(bwpId, rnti);
}

template <class C>
void
MemberNrFhSchedSapProvider<C>::UpdateActiveUesMap(
    uint16_t bwpId,
    const std::deque<VarTtiAllocInfo>& allocation,
    const std::unordered_map<uint16_t, std::shared_ptr<NrMacSchedulerUeInfo>>& ueMap)
{
    return m_owner->DoUpdateActiveUesMap(bwpId, allocation, ueMap);
}

template <class C>
uint8_t
MemberNrFhSchedSapProvider<C>::GetMaxMcsAssignable(uint16_t bwpId,
                                                   uint32_t reg,
                                                   uint32_t rnti,
                                                   uint8_t dlRank)
{
    return m_owner->DoGetMaxMcsAssignable(bwpId, reg, rnti, dlRank);
}

template <class C>
uint32_t
MemberNrFhSchedSapProvider<C>::GetMaxRegAssignable(uint16_t bwpId,
                                                   uint32_t mcs,
                                                   uint32_t rnti,
                                                   uint8_t dlRank)
{
    return m_owner->DoGetMaxRegAssignable(bwpId, mcs, rnti, dlRank);
}

/**
 * \brief Template for the implementation of the NrFhSchedSapUser as a member of an
 *        owner class of type C to which all methods are forwarded.
 */

template <class C>
class MemberNrFhSchedSapUser : public NrFhSchedSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrFhSchedSapUser(C* owner);

    MemberNrFhSchedSapUser() = delete;

    uint64_t GetNumRbPerRbgFromSched() override;

  private:
    C* m_owner; ///< the owner class

}; // end of class MemberNrFhSchedSapUser

template <class C>
MemberNrFhSchedSapUser<C>::MemberNrFhSchedSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
uint64_t
MemberNrFhSchedSapUser<C>::GetNumRbPerRbgFromSched()
{
    return m_owner->GetNumRbPerRbg();
}

} // namespace ns3
#endif // NR_FH_SCHED_SAP_H
