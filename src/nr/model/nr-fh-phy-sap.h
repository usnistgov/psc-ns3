// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_FH_PHY_SAP_H
#define NR_FH_PHY_SAP_H

#include "nr-gnb-phy.h"

namespace ns3
{

/**
 * \brief Service Access Point (SAP) offered by the FhControl instance
 *        to the gnb PHY instance.
 *
 * This is the *NrFhPhySapProvider*, i.e., the part of the SAP that contains
 * the FhControl methods called by the gnb PHY instance.
 *
 * FH Control ---> PHY
 */

class NrFhPhySapProvider
{
  public:
    virtual ~NrFhPhySapProvider();

    virtual uint8_t GetFhControlMethod() = 0;
    virtual bool DoesAllocationFit(uint16_t bwpId,
                                   uint32_t mcs,
                                   uint32_t nRegs,
                                   uint8_t dlRank) = 0;
    virtual void UpdateTracesBasedOnDroppedData(uint16_t bwpId,
                                                uint32_t mcs,
                                                uint32_t nRbgs,
                                                uint32_t nSymb,
                                                uint8_t dlRank) = 0;
    virtual void NotifyEndSlot(uint16_t bwpId, SfnSf currentSlot) = 0;
};

/**
 * \brief Service Access Point (SAP) offered by the gnb PHY instance to
 * the FhControl instance.
 *
 * This is the *NrFhPhySapUser*, i.e., the part of the SAP that contains the
 * gnb PHY methods called by the FhControl instance.
 *
 * PHY --> FH Control
 */

class NrFhPhySapUser
{
  public:
    virtual ~NrFhPhySapUser();

    virtual uint16_t GetNumerology() const = 0;
};

/**
 * \brief Template for the implementation of the NrFhPhySapProvider as a member of
 *        an owner class of type C to which all methods are forwarded.
 */
template <class C>
class MemberNrFhPhySapProvider : public NrFhPhySapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrFhPhySapProvider(C* owner);

    MemberNrFhPhySapProvider() = delete;

    uint8_t GetFhControlMethod() override;
    bool DoesAllocationFit(uint16_t bwpId, uint32_t mcs, uint32_t nRegs, uint8_t dlRank) override;
    void UpdateTracesBasedOnDroppedData(uint16_t bwpId,
                                        uint32_t mcs,
                                        uint32_t nRbgs,
                                        uint32_t nSymb,
                                        uint8_t dlRank) override;
    void NotifyEndSlot(uint16_t bwpId, SfnSf currentSlot) override;

  private:
    C* m_owner; ///< the owner class

}; // end of class MemberNrFhPhySapProvider

template <class C>
MemberNrFhPhySapProvider<C>::MemberNrFhPhySapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
uint8_t
MemberNrFhPhySapProvider<C>::GetFhControlMethod()
{
    return m_owner->DoGetFhControlMethod();
}

template <class C>
bool
MemberNrFhPhySapProvider<C>::DoesAllocationFit(uint16_t bwpId,
                                               uint32_t mcs,
                                               uint32_t nRegs,
                                               uint8_t dlRank)
{
    return m_owner->DoGetDoesAllocationFit(bwpId, mcs, nRegs, dlRank);
}

template <class C>
void
MemberNrFhPhySapProvider<C>::UpdateTracesBasedOnDroppedData(uint16_t bwpId,
                                                            uint32_t mcs,
                                                            uint32_t nRbgs,
                                                            uint32_t nSymb,
                                                            uint8_t dlRank)
{
    return m_owner->DoUpdateTracesBasedOnDroppedData(bwpId, mcs, nRbgs, nSymb, dlRank);
}

template <class C>
void
MemberNrFhPhySapProvider<C>::NotifyEndSlot(uint16_t bwpId, SfnSf currentSlot)
{
    return m_owner->DoNotifyEndSlot(bwpId, currentSlot);
}

/**
 * \brief Template for the implementation of the NrFhPhySapUser as a member of an
 *        owner class of type C to which all methods are forwarded.
 */

template <class C>
class MemberNrFhPhySapUser : public NrFhPhySapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrFhPhySapUser(C* owner);

    MemberNrFhPhySapUser() = delete;

    uint16_t GetNumerology() const override;

  private:
    C* m_owner; ///< the owner class

}; // end of class NrFhPhySapUser

template <class C>
MemberNrFhPhySapUser<C>::MemberNrFhPhySapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
uint16_t
MemberNrFhPhySapUser<C>::GetNumerology() const
{
    return m_owner->GetNumerology();
}

} // namespace ns3
#endif // NR_FH_PHY_SAP_H
