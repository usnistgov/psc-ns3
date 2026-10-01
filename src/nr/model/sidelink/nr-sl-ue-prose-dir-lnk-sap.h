//
// SPDX-License-Identifier: NIST-Software
//

#ifndef NR_SL_UE_PROSE_DIR_LNK_SAP_H
#define NR_SL_UE_PROSE_DIR_LNK_SAP_H

#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/simulator.h>

namespace ns3
{

/**
 * \ingroup nr
 *
 * \brief User part of the Service Access Point (SAP) between ProSe Direct Link
 *        and ProSe layer. This is the part of the SAP exported by the ProSe
 *        layer and called from the ProSe direct link class
 *
 */
class NrSlUeProseDirLnkSapUser
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeProseDirLnkSapUser();

    /**
     * Structure to store the IP addresses corresponding to both UEs in the
     * direct link
     */
    struct DirectLinkIpInfo
    {
        Ipv4Address selfIpv4Addr;
        Ipv4Address peerIpv4Addr;
    };

    /**
     * The UE-to-Network relay role of the UE
     */
    enum U2nRole
    {
        RemoteUe = 0,
        RelayUe
    };

    /**
     * Structure to store the UE-to-Network Relay configuration of the UE if
     * the direct link is part of a relay connection
     */
    struct DirectLinkRelayInfo
    {
        bool isRelayConn =
            false;    ///< flag set to true if the direct link is part of a relay connection
        U2nRole role; ///< role of the UE in the direct link for U2N relay (remote UE or relay UE)
        uint32_t relayServiceCode; ///< the service code of the associated relay service
    };

    /**
     * Structure to exchange relevant information during the change of state
     * notification function
     */
    struct ChangeOfStateNotification
    {
        uint16_t oldStateEnum;         ///< old state
        uint16_t newStateEnum;         ///< new state
        std::string newStateStr;       ///< new state
        DirectLinkIpInfo ipInfo;       ///< IP addresses of the link
        DirectLinkRelayInfo relayInfo; ///< UE-to-Network relay configuration
    };

    // Methods
    /**
     * \brief The direct link instructs the ProSe layer to send an NR SL PC5-S
     * message to a given destination in a given logical channel
     *
     * \param packet the NR SL PC5-S message
     * \param dstL2Id the destination layer 2 ID
     * \param lcId the logical channel ID of the logical channel where the message should be sent
     */
    virtual void SendNrSlPc5SMessage(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId) = 0;
    /**
     * \brief The direct link notifies the ProSe layer that the direct link changed
     *        of state and provides relevant information if any
     *
     * \param peerL2Id the layer 2 ID of the peer UE in the link
     * \param info structure containing information relevant to the change of state
     */
    virtual void NotifyChangeOfDirectLinkState(
        uint32_t peerL2Id,
        NrSlUeProseDirLnkSapUser::ChangeOfStateNotification info) = 0;
};

/**
 * \brief Provider part of the Service Access Point (SAP) between ProSe Direct Link
 * and ProSe layer. This is the part of the SAP that is exported by the direct link
 * and called by the ProSe layer.
 *
 */
class NrSlUeProseDirLnkSapProvider
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeProseDirLnkSapProvider();
    // Methods
    /**
     * \brief The ProSe layer passes an NR SL PC5-S message to the direct link
     *        instance
     *
     * \param packet the NR SL PC5-S message
     */
    virtual void ReceiveNrSlPc5Message(Ptr<Packet> packet) = 0;
};

////////////////////////////////////
//   templates NrSlUeProse side
////////////////////////////////////

/**
 * Template for the implementation of the NrSlUeProseDirLnkSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeProseDirLnkSapUser : public NrSlUeProseDirLnkSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeProseDirLnkSapUser(C* owner);

    // inherited from NRSlUeRrcSapUser
    virtual void SendNrSlPc5SMessage(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId);
    virtual void NotifyChangeOfDirectLinkState(
        uint32_t peerL2Id,
        NrSlUeProseDirLnkSapUser::ChangeOfStateNotification info);

  private:
    MemberNrSlUeProseDirLnkSapUser();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeProseDirLnkSapUser<C>::MemberNrSlUeProseDirLnkSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlUeProseDirLnkSapUser<C>::MemberNrSlUeProseDirLnkSapUser()
{
}

template <class C>
void
MemberNrSlUeProseDirLnkSapUser<C>::SendNrSlPc5SMessage(Ptr<Packet> packet,
                                                       uint32_t dstL2Id,
                                                       uint8_t lcId)
{
    m_owner->DoSendNrSlPc5SMessage(packet, dstL2Id, lcId);
}

template <class C>
void
MemberNrSlUeProseDirLnkSapUser<C>::NotifyChangeOfDirectLinkState(
    uint32_t peerL2Id,
    NrSlUeProseDirLnkSapUser::ChangeOfStateNotification info)
{
    m_owner->DoNotifyChangeOfDirectLinkState(peerL2Id, info);
}

//////////////////////////////////////////////
//   templates NrSlUeProseDirectLink side
/////////////////////////////////////////////

/**
 * Template for the implementation of the NrSlUeProseDirLnkSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeProseDirLnkSapProvider : public NrSlUeProseDirLnkSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeProseDirLnkSapProvider(C* owner);

    // inherited from NRSlUeRrcSapProvider
    virtual void ReceiveNrSlPc5Message(Ptr<Packet> packet);

  private:
    MemberNrSlUeProseDirLnkSapProvider();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeProseDirLnkSapProvider<C>::MemberNrSlUeProseDirLnkSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlUeProseDirLnkSapProvider<C>::MemberNrSlUeProseDirLnkSapProvider()
{
}

template <class C>
void
MemberNrSlUeProseDirLnkSapProvider<C>::ReceiveNrSlPc5Message(Ptr<Packet> packet)
{
    m_owner->DoReceiveNrSlPc5Message(packet);
}

} // namespace ns3

#endif /* NR_SL_RRC_SAP_H */
