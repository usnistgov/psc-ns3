// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#ifndef NR_SL_AS_SAP_H
#define NR_SL_AS_SAP_H

#include "nr-sl-tft.h"

#include <ns3/packet.h>
#include <ns3/ptr.h>

namespace ns3
{

/**
 * This class implements the Access Stratum (AS) Service Access Point
 * (SAP), i.e., the interface between the NrSlEpcUeNas and the NrSlUeRrc.
 * In particular, this class implements the
 * Provider part of the SAP, i.e., the methods exported by the
 * NrUeRrc and called by the NrSlEpcUeNas.
 */
class NrSlAsSapProvider
{
  public:
    virtual ~NrSlAsSapProvider() = default;

    /**
     * \brief Activate NR sidelink radio bearer
     *
     * Tells the RRC to activate NR Sidelink Bearer
     *
     * \param isTransmit True if the bearer is for transmission
     * \param isReceive True if the bearer is for reception
     * \param slInfo The SidelinkInfo for the bearer
     */
    virtual void ActivateNrSlRadioBearer(bool isTransmit,
                                         bool isReceive,
                                         const SidelinkInfo& slInfo) = 0;

    /**
     * \brief Delete existing NR SL radio bearer
     *
     * \param isTransmit True if the bearer is for transmission
     * \param isReceive True if the bearer is for reception
     * \param slInfo The SidelinkInfo for the bearer
     */
    virtual void DeleteNrSlRadioBearer(bool isTransmit,
                                       bool isReceive,
                                       const SidelinkInfo& slInfo) = 0;

    /**
     * \brief Send sidelink data packet to RRC.
     *
     * \param packet The packet
     * \param dstL2Id The destination layer 2 id
     * \param lcId The logical channel id
     */
    virtual void SendSidelinkData(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId) = 0;
};

/**
 * This class implements the Access Stratum (AS) Service Access Point
 * (SAP), i.e., the interface between the NrSlEpcUeNas and the NrSlUeRrc
 * In particular, this class implements the
 * User part of the SAP, i.e., the methods exported by the
 * NrSlEpcUeNas and called by the NrSlUeRrc.
 */
class NrSlAsSapUser
{
  public:
    virtual ~NrSlAsSapUser() = default;

    /**
     * \brief Notify the NAS that the NR sidelink has been setup
     *
     * \param slInfo The SidelinkInfo for the bearer
     */
    virtual void NotifyNrSlRadioBearerActivated(const struct SidelinkInfo& slInfo) = 0;

    /**
     * \brief Notify the NAS that the NR sidelink has been removed
     *
     * \param slInfo The SidelinkInfo for the bearer
     */
    virtual void NotifyNrSlRadioBearerRemoved(const struct SidelinkInfo& slInfo) = 0;
};

/**
 * Template for the implementation of the NrSlAsSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 */
template <class C>
class MemberNrSlAsSapProvider : public NrSlAsSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlAsSapProvider(C* owner);

    // Delete default constructor to avoid misuse
    MemberNrSlAsSapProvider() = delete;

    // inherited from NrSlAsSapProvider
    void ActivateNrSlRadioBearer(bool isTransmit,
                                 bool isReceive,
                                 const struct SidelinkInfo& slInfo) override;
    void DeleteNrSlRadioBearer(bool isTransmit,
                               bool isReceive,
                               const struct SidelinkInfo& slInfo) override;
    void SendSidelinkData(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId) override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlAsSapProvider<C>::MemberNrSlAsSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
void
MemberNrSlAsSapProvider<C>::ActivateNrSlRadioBearer(bool isTransmit,
                                                    bool isReceive,
                                                    const struct SidelinkInfo& slInfo)
{
    m_owner->DoActivateNrSlRadioBearer(isTransmit, isReceive, slInfo);
}

template <class C>
void
MemberNrSlAsSapProvider<C>::DeleteNrSlRadioBearer(bool isTransmit,
                                                  bool isReceive,
                                                  const struct SidelinkInfo& slInfo)
{
    m_owner->DoDeleteNrSlDataRadioBearer(isTransmit, isReceive, slInfo);
}

template <class C>
void
MemberNrSlAsSapProvider<C>::SendSidelinkData(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId)
{
    m_owner->DoSendSidelinkData(packet, dstL2Id, lcId);
}

/**
 * Template for the implementation of the NrSlAsSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 */
template <class C>
class MemberNrSlAsSapUser : public NrSlAsSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlAsSapUser(C* owner);

    // Delete default constructor to avoid misuse
    MemberNrSlAsSapUser() = delete;

    // inherited from NrAsSapUser
    void NotifyNrSlRadioBearerActivated(const struct SidelinkInfo& slInfo) override;
    void NotifyNrSlRadioBearerRemoved(const struct SidelinkInfo& slInfo) override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlAsSapUser<C>::MemberNrSlAsSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
void
MemberNrSlAsSapUser<C>::NotifyNrSlRadioBearerActivated(const struct SidelinkInfo& slInfo)
{
    m_owner->DoNotifyNrSlRadioBearerActivated(slInfo);
}

template <class C>
void
MemberNrSlAsSapUser<C>::NotifyNrSlRadioBearerRemoved(const struct SidelinkInfo& slInfo)
{
    m_owner->DoNotifyNrSlRadioBearerRemoved(slInfo);
}

} // namespace ns3

#endif // NR_SL_AS_SAP_H
