//
// SPDX-License-Identifier: NIST-Software
//

#ifndef NR_SL_UE_SVC_RRC_SAP_H
#define NR_SL_UE_SVC_RRC_SAP_H

#include "ns3/nr-rrc-sap.h"

namespace ns3
{

/**
 * \ingroup nr
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
     * \brief The RRC passes the SD-RSRP measurements (after L3 filtering and comparison with
     * threshold/hysteresis) related to a peer UE to the service layer.
     *
     * \param peerL2Id the peer layer 2 ID
     * \param value SD-RSRP value associated with peer UE
     * \param eligible confirms whether or not the relay passed the RSRP threshold/hysteresis
     * criteria
     */
    virtual void ReceiveNrSdRsrpMeasurements(uint32_t peerL2Id, double value, bool eligible) = 0;

    /**
     * \brief The RRC passes the SL-RSRP measurements (after L3 filtering and comparison with
     * threshold/hysteresis) related to a peer UE to the service layer.
     *
     * \param peerL2Id the peer layer 2 ID
     * \param value SL-RSRP value associated with peer UE
     * \param eligible confirms whether or not the relay passed the RSRP threshold/hysteresis
     * criteria
     */
    virtual void ReceiveNrSlRsrpMeasurements(uint32_t peerL2Id, double value, bool eligible) = 0;

    /**
     * \brief The RRC indicates to service layer that data has been received from a given
     *        source
     *
     * \param srcL2Id the source layer 2 ID that data was received from
     * \return whether the packet should be received
     */
    virtual bool NotifyDataReceived(uint32_t srcL2Id) = 0;

    /**
     * \brief The RRC indicates to service layer that a packet is available to send
     *
     * \param packet the packet to send
     * \param dstL2Id the intended destination L2 ID
     * \param lcId the intended logical channel ID
     * \return whether the packet may be sent
     */
    virtual bool ConfirmSendRequest(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId) = 0;
    /*
     * \brief Notify the service layer that a HARQ process has timed out
     *
     * \param peerL2Id the layer 2 ID of the peer UE
     */
    virtual void NotifyNrSlHarqProcessMaxTransmissionsWithNoFeedback(uint32_t peerL2Id) = 0;
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
    /**
     * \brief Indicates if SD-RSRP measurements are being collected
     *
     * \returns True, if SD-RSRP measurements are being collected; otherwise,
     *          false
     */
    virtual bool IsUeSdRsrpMeasurementsEnabled() const = 0;
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
    virtual void ReceiveNrSdRsrpMeasurements(uint32_t peerL2Id, double value, bool eligible);
    virtual void ReceiveNrSlRsrpMeasurements(uint32_t peerL2Id, double value, bool eligible);
    virtual bool NotifyDataReceived(uint32_t srcL2Id);
    virtual bool ConfirmSendRequest(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId);
    virtual void NotifyNrSlHarqProcessMaxTransmissionsWithNoFeedback(uint32_t peerL2Id);

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
MemberNrSlUeSvcRrcSapUser<C>::ReceiveNrSdRsrpMeasurements(uint32_t peerL2Id,
                                                          double value,
                                                          bool eligible)
{
    m_owner->DoReceiveNrSdRsrpMeasurements(peerL2Id, value, eligible);
}

template <class C>
void
MemberNrSlUeSvcRrcSapUser<C>::ReceiveNrSlRsrpMeasurements(uint32_t peerL2Id,
                                                          double value,
                                                          bool eligible)
{
    m_owner->DoReceiveNrSlRsrpMeasurements(peerL2Id, value, eligible);
}

template <class C>
bool
MemberNrSlUeSvcRrcSapUser<C>::NotifyDataReceived(uint32_t srcL2Id)
{
    return m_owner->DoNotifyDataReceived(srcL2Id);
}

template <class C>
bool
MemberNrSlUeSvcRrcSapUser<C>::ConfirmSendRequest(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId)
{
    return m_owner->DoConfirmSendRequest(packet, dstL2Id, lcId);
}

template <class C>
void
MemberNrSlUeSvcRrcSapUser<C>::NotifyNrSlHarqProcessMaxTransmissionsWithNoFeedback(uint32_t peerL2Id)
{
    m_owner->DoNotifyNrSlHarqProcessMaxTransmissionsWithNoFeedback(peerL2Id);
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
    virtual bool IsUeSdRsrpMeasurementsEnabled() const;

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

template <class C>
bool
MemberNrSlUeSvcRrcSapProvider<C>::IsUeSdRsrpMeasurementsEnabled() const
{
    return m_owner->DoIsUeSdRsrpMeasurementsEnabled();
}

} // namespace ns3

#endif /* NR_SL_UE_SVC_RRC_SAP_H */
