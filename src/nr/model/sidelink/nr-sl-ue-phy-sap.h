// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_UE_PHY_SAP_H
#define NR_SL_UE_PHY_SAP_H

#include "nr-sl-phy-mac-common.h"

#include <ns3/nstime.h>
#include <ns3/packet-burst.h>
#include <ns3/ptr.h>

#include <stdint.h>
#include <unordered_set>

namespace ns3
{

/**
 * \ingroup nr
 *
 * Service Access Point (SAP) offered by the UE PHY to the UE MAC
 * for NR Sidelink
 *
 * This is the PHY SAP Provider, i.e., the part of the SAP that contains
 * the UE PHY methods called by the UE MAC
 */
class NrSlUePhySapProvider
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUePhySapProvider();

    // Sidelink Communication
    /**
     * \brief Ask the PHY the bandwidth in RBs
     *
     * \return the bandwidth in RBs
     */
    virtual uint32_t GetBwInRbs() const = 0;
    /**
     * \brief Get the slot period
     * \return the slot period (depend on the numerology)
     */
    virtual Time GetSlotPeriod() const = 0;
    /**
     * \brief Send NR Sidelink PSCCH MAC PDU
     * \param p The packet
     */
    virtual void SendPscchMacPdu(Ptr<Packet> p) = 0;
    /**
     * \brief Send NR Sidelink PSSCH MAC PDU
     * \param p The packet
     * \param dstL2Id The destination L2 ID
     */
    virtual void SendPsschMacPdu(Ptr<Packet> p, uint32_t dstL2Id) = 0;
    /**
     * \brief Set the allocation info for NR SL slot in PHY
     * \param sfn The SfnSf
     * \param varTtiInfo The Variable TTI allocation info
     */
    virtual void SetNrSlVarTtiAllocInfo(const SfnSf& sfn,
                                        const NrSlVarTtiAllocInfo& varTtiInfo) = 0;
    /**
     * \brief Indicates if the PHY has pending HARQ feedback to send
     * \returns True, if there is pending HARQ feedback to send; otherwise,
     *          false
     */
    virtual bool IsSlHarqFeedbackPending() const = 0;
    /**
     * \brief Get the numerology
     * \return the numerology
     */
    virtual uint16_t GetNumerology() const = 0;
};

/**
 * \ingroup nr
 *
 * Service Access Point (SAP) offered by the UE MAC to the UE PHY
 * for NR Sidelink
 *
 * This is the PHY SAP User, i.e., the part of the SAP that contains the UE
 * MAC methods called by the UE PHY
 */
class NrSlUePhySapUser
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUePhySapUser();

    /**
     * \brief Gets the active Sidelink pool id used for transmission and reception
     *
     * \return The active TX pool id
     */
    virtual uint8_t GetSlActiveTxPoolId() = 0;
    /**
     * \brief Get the list of Sidelink destination for transmission from UE MAC
     * \return A vector holding Sidelink communication destinations for transmission and the highest
     * priority value among its LCs
     */
    virtual std::vector<std::pair<uint32_t, uint8_t>> GetSlTxDestinations() = 0;
    /**
     * \brief Get the list of Sidelink destination for reception from UE MAC
     * \return A vector holding Sidelink communication destinations for reception and the highest
     * priority value among its LCs
     */
    virtual std::unordered_set<uint32_t> GetSlRxDestinations() = 0;

    /**
     * \brief Receive NR SL PSSCH PHY PDU
     * \param pdu The NR SL PSSCH PHY PDU
     */
    virtual void ReceivePsschPhyPdu(Ptr<PacketBurst> pdu) = 0;
    /**
     * \brief Receive the sensing information from PHY to MAC
     * \param sensingData The sensing data
     */
    virtual void ReceiveSensingData(SensingData sensingData) = 0;
    /**
     * \brief Receive the PSFCH from PHY to MAC
     * \param sendingNodeId The sending nodeId
     * \param harqInfo The HARQ info
     */
    virtual void ReceivePsfch(uint32_t sendingNodeId, SlHarqInfo harqInfo) = 0;
};

/**
 * \ingroup nr
 *
 * Template for the implementation of the NrSlUePhySapProvider as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to UE PHY class, which are called by UE MAC
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlUePhySapProvider : public NrSlUePhySapProvider
{
  public:
    /**
     * \brief Constructor
     *
     * \param owner The owner class
     */
    MemberNrSlUePhySapProvider(C* owner);
    MemberNrSlUePhySapProvider() = delete;

    uint32_t GetBwInRbs() const override;
    Time GetSlotPeriod() const override;
    void SendPscchMacPdu(Ptr<Packet> p) override;
    void SendPsschMacPdu(Ptr<Packet> p, uint32_t dstL2Id) override;
    void SetNrSlVarTtiAllocInfo(const SfnSf& sfn, const NrSlVarTtiAllocInfo& varTtiInfo) override;
    bool IsSlHarqFeedbackPending() const override;
    uint16_t GetNumerology() const override;

    // methods inherited from NrSlUePhySapProvider go here
    // NR Sidelink communication

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUePhySapProvider<C>::MemberNrSlUePhySapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
uint32_t
MemberNrSlUePhySapProvider<C>::GetBwInRbs() const
{
    return m_owner->DoGetBwInRbs();
}

template <class C>
Time
MemberNrSlUePhySapProvider<C>::GetSlotPeriod() const
{
    return m_owner->DoGetSlotPeriod();
}

template <class C>
void
MemberNrSlUePhySapProvider<C>::SendPscchMacPdu(Ptr<Packet> p)
{
    m_owner->DoSendPscchMacPdu(p);
}

template <class C>
void
MemberNrSlUePhySapProvider<C>::SendPsschMacPdu(Ptr<Packet> p, uint32_t dstL2Id)
{
    m_owner->DoSendPsschMacPdu(p, dstL2Id);
}

template <class C>
void
MemberNrSlUePhySapProvider<C>::SetNrSlVarTtiAllocInfo(const SfnSf& sfn,
                                                      const NrSlVarTtiAllocInfo& varTtiInfo)
{
    m_owner->DoSetNrSlVarTtiAllocInfo(sfn, varTtiInfo);
}

template <class C>
bool
MemberNrSlUePhySapProvider<C>::IsSlHarqFeedbackPending() const
{
    return m_owner->DoIsSlHarqFeedbackPending();
}

template <class C>
uint16_t
MemberNrSlUePhySapProvider<C>::GetNumerology() const
{
    return m_owner->DoGetNumerology();
}

/**
 * \ingroup nr
 *
 * Template for the implementation of the NrSlUePhySapUser as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to UE MAC class, which are called by UE PHY
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlUePhySapUser : public NrSlUePhySapUser
{
  public:
    /**
     * \brief Constructor
     *
     * \param owner The owner class
     */
    MemberNrSlUePhySapUser(C* owner);

    // methods inherited from NrSlUePhySapUser go here
    uint8_t GetSlActiveTxPoolId() override;
    std::vector<std::pair<uint32_t, uint8_t>> GetSlTxDestinations() override;
    std::unordered_set<uint32_t> GetSlRxDestinations() override;
    void ReceivePsschPhyPdu(Ptr<PacketBurst> pdu) override;
    void ReceiveSensingData(SensingData sensingData) override;
    void ReceivePsfch(uint32_t sendingNodeId, SlHarqInfo harqInfo) override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUePhySapUser<C>::MemberNrSlUePhySapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
uint8_t
MemberNrSlUePhySapUser<C>::GetSlActiveTxPoolId()
{
    return m_owner->DoGetSlActiveTxPoolId();
}

template <class C>
std::vector<std::pair<uint32_t, uint8_t>>
MemberNrSlUePhySapUser<C>::GetSlTxDestinations()
{
    return m_owner->DoGetSlTxDestinations();
}

template <class C>
std::unordered_set<uint32_t>
MemberNrSlUePhySapUser<C>::GetSlRxDestinations()
{
    return m_owner->DoGetSlRxDestinations();
}

template <class C>
void
MemberNrSlUePhySapUser<C>::ReceivePsschPhyPdu(Ptr<PacketBurst> pdu)
{
    m_owner->DoReceivePsschPhyPdu(pdu);
}

template <class C>
void
MemberNrSlUePhySapUser<C>::ReceiveSensingData(SensingData sensingData)
{
    m_owner->DoReceiveSensingData(sensingData);
}

template <class C>
void
MemberNrSlUePhySapUser<C>::ReceivePsfch(uint32_t sendingNodeId, SlHarqInfo harqInfo)
{
    m_owner->DoReceivePsfch(sendingNodeId, harqInfo);
}

} // namespace ns3

#endif // NR_SL_UE_PHY_SAP_H
