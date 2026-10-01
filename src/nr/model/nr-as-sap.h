// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>

#ifndef NR_AS_SAP_H
#define NR_AS_SAP_H

#include <ns3/packet.h>
#include <ns3/ptr.h>

#include <stdint.h>

namespace ns3
{

/**
 * This class implements the Access Stratum (AS) Service Access Point
 * (SAP), i.e., the interface between the NrEpcUeNas and the NrUeRrc.
 * In particular, this class implements the
 * Provider part of the SAP, i.e., the methods exported by the
 * NrUeRrc and called by the NrEpcUeNas.
 */
class NrAsSapProvider
{
  public:
    virtual ~NrAsSapProvider() = default;

    /**
     * \brief Set the selected Closed Subscriber Group subscription list to be
     *        used for cell selection.
     *
     * \param csgId identity of the subscribed CSG
     */
    virtual void SetCsgWhiteList(uint32_t csgId) = 0;

    /**
     * \brief Initiate Idle mode cell selection procedure.
     *
     * \param dlEarfcn the downlink carrier frequency (EARFCN)
     */
    virtual void StartCellSelection(uint32_t dlEarfcn) = 0;

    /**
     * \brief Force the RRC entity to stay camped on a certain eNodeB.
     *
     * \param cellId the cell ID identifying the eNodeB
     * \param dlEarfcn the downlink carrier frequency (EARFCN)
     */
    virtual void ForceCampedOnGnb(uint16_t cellId, uint32_t dlEarfcn) = 0;

    /**
     * \brief Tell the RRC entity to enter Connected mode.
     *
     * If this function is called when the UE is in a situation where connecting
     * is not possible (e.g. before the simulation begin), then the UE will
     * attempt to connect at the earliest possible time (e.g. after it camps to a
     * suitable cell).
     */
    virtual void Connect() = 0;

    /**
     * \brief Send a data packet.
     *
     * \param packet the packet
     * \param bid the EPS bearer ID
     */
    virtual void SendData(Ptr<Packet> packet, uint8_t bid) = 0;

    /**
     * \brief Tell the RRC entity to release the connection.
     */
    virtual void Disconnect() = 0;
};

/**
 * This class implements the Access Stratum (AS) Service Access Point
 * (SAP), i.e., the interface between the NrEpcUeNas and the NrUeRrc
 * In particular, this class implements the
 * User part of the SAP, i.e., the methods exported by the
 * NrEpcUeNas and called by the NrUeRrc.
 */
class NrAsSapUser
{
  public:
    virtual ~NrAsSapUser() = default;

    /**
     * \brief Notify the NAS that RRC Connection Establishment was successful.
     */
    virtual void NotifyConnectionSuccessful() = 0;

    /**
     * \brief Notify the NAS that RRC Connection Establishment failed.
     */
    virtual void NotifyConnectionFailed() = 0;

    /**
     * Notify the NAS that RRC Connection was released
     */
    virtual void NotifyConnectionReleased() = 0;

    /**
     * receive a data packet
     *
     * \param packet the packet
     */
    virtual void RecvData(Ptr<Packet> packet) = 0;
};

/**
 * Template for the implementation of the NrAsSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 */
template <class C>
class MemberNrAsSapProvider : public NrAsSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrAsSapProvider(C* owner);

    // Delete default constructor to avoid misuse
    MemberNrAsSapProvider() = delete;

    // inherited from NrAsSapProvider
    void SetCsgWhiteList(uint32_t csgId) override;
    void StartCellSelection(uint32_t dlEarfcn) override;
    void ForceCampedOnGnb(uint16_t cellId, uint32_t dlEarfcn) override;
    void Connect() override;
    void SendData(Ptr<Packet> packet, uint8_t bid) override;
    void Disconnect() override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrAsSapProvider<C>::MemberNrAsSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
void
MemberNrAsSapProvider<C>::SetCsgWhiteList(uint32_t csgId)
{
    m_owner->DoSetCsgWhiteList(csgId);
}

template <class C>
void
MemberNrAsSapProvider<C>::StartCellSelection(uint32_t dlEarfcn)
{
    m_owner->DoStartCellSelection(dlEarfcn);
}

template <class C>
void
MemberNrAsSapProvider<C>::ForceCampedOnGnb(uint16_t cellId, uint32_t dlEarfcn)
{
    m_owner->DoForceCampedOnGnb(cellId, dlEarfcn);
}

template <class C>
void
MemberNrAsSapProvider<C>::Connect()
{
    m_owner->DoConnect();
}

template <class C>
void
MemberNrAsSapProvider<C>::SendData(Ptr<Packet> packet, uint8_t bid)
{
    m_owner->DoSendData(packet, bid);
}

template <class C>
void
MemberNrAsSapProvider<C>::Disconnect()
{
    m_owner->DoDisconnect();
}

/**
 * Template for the implementation of the NrAsSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 */
template <class C>
class MemberNrAsSapUser : public NrAsSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrAsSapUser(C* owner);

    // Delete default constructor to avoid misuse
    MemberNrAsSapUser() = delete;

    // inherited from NrAsSapUser
    void NotifyConnectionSuccessful() override;
    void NotifyConnectionFailed() override;
    void RecvData(Ptr<Packet> packet) override;
    void NotifyConnectionReleased() override;

  private:
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrAsSapUser<C>::MemberNrAsSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
void
MemberNrAsSapUser<C>::NotifyConnectionSuccessful()
{
    m_owner->DoNotifyConnectionSuccessful();
}

template <class C>
void
MemberNrAsSapUser<C>::NotifyConnectionFailed()
{
    m_owner->DoNotifyConnectionFailed();
}

template <class C>
void
MemberNrAsSapUser<C>::RecvData(Ptr<Packet> packet)
{
    m_owner->DoRecvData(packet);
}

template <class C>
void
MemberNrAsSapUser<C>::NotifyConnectionReleased()
{
    m_owner->DoNotifyConnectionReleased();
}

} // namespace ns3

#endif // NR_AS_SAP_H
