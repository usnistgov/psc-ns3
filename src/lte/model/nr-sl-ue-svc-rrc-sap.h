/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#ifndef NR_SL_UE_SVC_RRC_SAP_H
#define NR_SL_UE_SVC_RRC_SAP_H

#include "lte-rrc-sap.h"

namespace ns3
{

/**
 * \ingroup lte
 *
 * \brief User part of the Service Access Point (SAP) between the UE service
 *        layer (e.g., the ProSe layer) and the UE RRC
 *
 * This class implements the service Access Point (SAP) for the UE service
 * layer (e.g., the ProSe layer)  and the UE RRC. In particular, this class
 * implements the User part of the SAP, i.e., the methods exported by the
 * service layer and called by the UE RRC.
 */
class NrSlUeSvcRrcSapUser
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeSvcRrcSapUser();

    /**
     * \brief The RRC passes an NR SL signalling message from a given source
     *        to the service layer.
     *
     * \param packet the NR SL signaling message
     * \param srcL2Id the source layer 2 ID that sent the message
     */
    virtual void ReceiveNrSlSignalling(Ptr<Packet> packet, uint32_t srcL2Id) = 0;

    /**
     * \brief The RRC passes an NR SL discovery message from a given source
     *        to the service layer.
     *
     * \param packet the NR SL discovery message
     * \param srcL2Id the source layer 2 ID that sent the message
     */
    virtual void ReceiveNrSlDiscovery(Ptr<Packet> packet, uint32_t srcL2Id) = 0;

    /**
     * \brief The RRC passes the RSRP measurements (after L3 filtering and comparison with
     * threshold/hysteresis) related to a peer UE to the service layer.
     *
     * \param peerL2Id the peer layer 2 ID
     * \param value RSRP value associated with peer UE
     * \param eligible confirms whether or not the relay passed the RSRP threshold/hysteresis
     * criteria
     */
    virtual void ReceiveNrSlRsrpMeasurements(uint32_t peerL2Id, double value, bool eligible) = 0;
};

/**
 * \brief Provider part of the Service Access Point (SAP) between the UE
 *        service layer (e.g., the ProSe layer) and the UE RRC
 *
 * This class implements the service Access Point (SAP) for the UE service
 * layer (e.g., the ProSe layer)  and the UE RRC. In particular, this class
 * implements the Provider part of the SAP, i.e., the methods exported by the
 * UE RRC and called by the service layer.
 *
 */
class NrSlUeSvcRrcSapProvider
{
  public:
    /**
     * \brief Destructor
     */
    virtual ~NrSlUeSvcRrcSapProvider();

    /**
     * \brief Function to instruct the RRC that the UE should monitor messages
     *        directed to its own layer 2 ID
     *
     * This function is used when the UE expects to receive messages directed to
     * his own L2 ID, e.g., when participating in ProSe Unicast communication.
     * It instructs the RRC and lower layers to monitor messages with the UE's
     * layer 2 ID as destination.
     */
    virtual void MonitorSelfL2Id() = 0;

    /**
     * \brief Function to instruct the RRC that the UE should monitor messages
     *        directed to a given layer 2 ID
     *
     * This function is used when the UE expects to receive messages directed to
     * a specific L2 ID, e.g., when participating in ProSe direct discovery.
     * It instructs the RRC and lower layers to monitor messages with the given
     * layer 2 ID as destination.
     *
     * \param dstL2Id destination Layer 2 ID
     */
    virtual void MonitorL2Id(uint32_t dstL2Id) = 0;

    /**
     * \brief The service layer passes an NR SL signalling message to the RRC for
     *        transmission to a given destination using a given logical channel
     *
     * \param packet the NR SL signaling message
     * \param dstL2Id the destination layer 2 ID
     * \param lcId the logical channel id of the logical channel where the
     *             message should be sent
     */
    virtual void SendNrSlSignalling(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId) = 0;

    /**
     * \brief The service layer instructs the RRC to activate an NR SL signalling
     *        radio bearer for a given destination and logical channel
     *
     * \param slInfo The SidelinkInfo for the bearer
     */
    virtual void ActivateNrSlSignallingRadioBearer(const struct SidelinkInfo& slInfo) = 0;

    /**
     * \brief The service layer passes a NR SL discovery message to the RRC for
     *        transmission to a given destination
     *
     * \param packet the NR SL discovery message
     * \param dstL2Id the destination layer 2 ID
     */
    virtual void SendNrSlDiscovery(Ptr<Packet> packet, uint32_t dstL2Id) = 0;

    /**
     * \brief The service layer instructs the RRC to activate a NR SL discovery
     *        radio bearer for a given destination
     *
     * \param dstL2Id the destination layer 2 ID
     */
    virtual void ActivateNrSlDiscoveryRadioBearer(uint32_t dstL2Id) = 0;

    /**
     * \brief Notify the RRC that the following connection was released
     *
     * \param srcL2Id Sidelink source L2 id
     * \param dstL2Id Sidelink destination L2 id
     * \param lcId Logical channel id
     */
    virtual void NotifySidelinkConnectionRelease(uint32_t srcL2Id,
                                                 uint32_t dstL2Id,
                                                 uint8_t lcId) = 0;
};

/**
 * Template for the implementation of the NrSlUeSvcRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeSvcRrcSapUser : public NrSlUeSvcRrcSapUser
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeSvcRrcSapUser(C* owner);

    // inherited from NrSlUeSvcRrcSapUser
    virtual void ReceiveNrSlSignalling(Ptr<Packet> packet, uint32_t srcL2Id);
    virtual void ReceiveNrSlDiscovery(Ptr<Packet> packet, uint32_t srcL2Id);
    virtual void ReceiveNrSlRsrpMeasurements(uint32_t peerL2Id, double value, bool eligible);

  private:
    MemberNrSlUeSvcRrcSapUser();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeSvcRrcSapUser<C>::MemberNrSlUeSvcRrcSapUser(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlUeSvcRrcSapUser<C>::MemberNrSlUeSvcRrcSapUser()
{
}

template <class C>
void
MemberNrSlUeSvcRrcSapUser<C>::ReceiveNrSlSignalling(Ptr<Packet> packet, uint32_t srcL2Id)
{
    m_owner->DoReceiveNrSlSignalling(packet, srcL2Id);
}

template <class C>
void
MemberNrSlUeSvcRrcSapUser<C>::ReceiveNrSlDiscovery(Ptr<Packet> packet, uint32_t srcL2Id)
{
    m_owner->DoReceiveNrSlDiscovery(packet, srcL2Id);
}

template <class C>
void
MemberNrSlUeSvcRrcSapUser<C>::ReceiveNrSlRsrpMeasurements(uint32_t peerL2Id,
                                                          double value,
                                                          bool eligible)
{
    m_owner->DoReceiveNrSlRsrpMeasurements(peerL2Id, value, eligible);
}

/**
 * Template for the implementation of the NrSlUeSvcRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeSvcRrcSapProvider : public NrSlUeSvcRrcSapProvider
{
  public:
    /**
     * Constructor
     *
     * \param owner the owner class
     */
    MemberNrSlUeSvcRrcSapProvider(C* owner);

    // inherited from NrSlUeSvcRrcSapProvider
    virtual void MonitorSelfL2Id();
    virtual void MonitorL2Id(uint32_t dstL2Id);
    virtual void SendNrSlSignalling(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId);
    virtual void ActivateNrSlSignallingRadioBearer(const struct SidelinkInfo& slInfo);
    virtual void SendNrSlDiscovery(Ptr<Packet> packet, uint32_t dstL2Id);
    virtual void ActivateNrSlDiscoveryRadioBearer(uint32_t dstL2Id);
    virtual void NotifySidelinkConnectionRelease(uint32_t srcL2Id, uint32_t dstL2Id, uint8_t lcId);

  private:
    MemberNrSlUeSvcRrcSapProvider();
    C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeSvcRrcSapProvider<C>::MemberNrSlUeSvcRrcSapProvider(C* owner)
    : m_owner(owner)
{
}

template <class C>
MemberNrSlUeSvcRrcSapProvider<C>::MemberNrSlUeSvcRrcSapProvider()
{
}

template <class C>
void
MemberNrSlUeSvcRrcSapProvider<C>::MonitorSelfL2Id()
{
    m_owner->DoMonitorSelfL2Id();
}

template <class C>
void
MemberNrSlUeSvcRrcSapProvider<C>::MonitorL2Id(uint32_t dstL2Id)
{
    m_owner->DoMonitorL2Id(dstL2Id);
}

template <class C>
void
MemberNrSlUeSvcRrcSapProvider<C>::SendNrSlSignalling(Ptr<Packet> packet,
                                                     uint32_t dstL2Id,
                                                     uint8_t lcId)
{
    m_owner->DoSendNrSlSignalling(packet, dstL2Id, lcId);
}

template <class C>
void
MemberNrSlUeSvcRrcSapProvider<C>::ActivateNrSlSignallingRadioBearer(
    const struct SidelinkInfo& slInfo)
{
    m_owner->DoActivateNrSlSignallingRadioBearer(slInfo);
}

template <class C>
void
MemberNrSlUeSvcRrcSapProvider<C>::SendNrSlDiscovery(Ptr<Packet> packet, uint32_t dstL2Id)
{
    m_owner->DoSendNrSlDiscoveryMessage(packet, dstL2Id);
}

template <class C>
void
MemberNrSlUeSvcRrcSapProvider<C>::ActivateNrSlDiscoveryRadioBearer(uint32_t dstL2Id)
{
    m_owner->DoActivateNrSlDiscoveryRadioBearer(dstL2Id);
}

template <class C>
void
MemberNrSlUeSvcRrcSapProvider<C>::NotifySidelinkConnectionRelease(uint32_t srcL2Id,
                                                                  uint32_t dstL2Id,
                                                                  uint8_t lcId)
{
    m_owner->DoNotifySidelinkConnectionRelease(srcL2Id, dstL2Id, lcId);
}

} // namespace ns3

#endif /* NR_SL_UE_SVC_RRC_SAP_H */
