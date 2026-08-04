// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>

#include "nr-epc-ue-nas.h"

#include "nr-as-sap.h"

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/nr-epc-helper.h>
#include <ns3/simulator.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrEpcUeNas");

/// Map each of UE NAS states to its string representation.
static const std::string g_ueNasStateName[NrEpcUeNas::NUM_STATES] = {
    "OFF",
    "ATTACHING",
    "IDLE_REGISTERED",
    "CONNECTING_TO_EPC",
    "ACTIVE",
};

/**
 * \param s The UE NAS state.
 * \return The string representation of the given state.
 */
static inline const std::string&
ToString(NrEpcUeNas::State s)
{
    return g_ueNasStateName[s];
}

NS_OBJECT_ENSURE_REGISTERED(NrEpcUeNas);

NrEpcUeNas::NrEpcUeNas()
{
    NS_LOG_FUNCTION(this);
    m_asSapUser = new MemberNrAsSapUser<NrEpcUeNas>(this);
}

NrEpcUeNas::~NrEpcUeNas()
{
    NS_LOG_FUNCTION(this);
}

void
NrEpcUeNas::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_asSapUser;
}

TypeId
NrEpcUeNas::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrEpcUeNas")
            .SetParent<Object>()
            .SetGroupName("Nr")
            .AddConstructor<NrEpcUeNas>()
            .AddTraceSource("StateTransition",
                            "fired upon every UE NAS state transition",
                            MakeTraceSourceAccessor(&NrEpcUeNas::m_stateTransitionCallback),
                            "ns3::NrEpcUeNas::StateTracedCallback");
    return tid;
}

void
NrEpcUeNas::SetDevice(Ptr<NetDevice> dev)
{
    NS_LOG_FUNCTION(this << dev);
    m_device = dev;
}

void
NrEpcUeNas::SetImsi(uint64_t imsi)
{
    NS_LOG_FUNCTION(this << imsi);
    m_imsi = imsi;
}

void
NrEpcUeNas::SetCsgId(uint32_t csgId)
{
    NS_LOG_FUNCTION(this << csgId);
    m_csgId = csgId;
    m_asSapProvider->SetCsgWhiteList(csgId);
}

uint32_t
NrEpcUeNas::GetCsgId() const
{
    NS_LOG_FUNCTION(this);
    return m_csgId;
}

void
NrEpcUeNas::SetAsSapProvider(NrAsSapProvider* s)
{
    NS_LOG_FUNCTION(this << s);
    m_asSapProvider = s;
}

NrAsSapUser*
NrEpcUeNas::GetAsSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_asSapUser;
}

void
NrEpcUeNas::SetForwardUpCallback(Callback<void, Ptr<Packet>> cb)
{
    NS_LOG_FUNCTION(this);
    m_forwardUpCallback = cb;
}

void
NrEpcUeNas::StartCellSelection(uint32_t dlEarfcn)
{
    NS_LOG_FUNCTION(this << dlEarfcn);
    m_asSapProvider->StartCellSelection(dlEarfcn);
}

void
NrEpcUeNas::Connect()
{
    NS_LOG_FUNCTION(this);

    // tell RRC to go into connected mode
    m_asSapProvider->Connect();
}

void
NrEpcUeNas::Connect(uint16_t cellId, uint32_t dlEarfcn)
{
    NS_LOG_FUNCTION(this << cellId << dlEarfcn);

    // force the UE RRC to be camped on a specific eNB
    m_asSapProvider->ForceCampedOnGnb(cellId, dlEarfcn);

    // tell RRC to go into connected mode
    m_asSapProvider->Connect();
}

void
NrEpcUeNas::Disconnect()
{
    NS_LOG_FUNCTION(this);
    SwitchToState(OFF);
    m_asSapProvider->Disconnect();
}

void
NrEpcUeNas::ActivateEpsBearer(NrEpsBearer bearer, Ptr<NrEpcTft> tft)
{
    NS_LOG_FUNCTION(this);
    switch (m_state)
    {
    case ACTIVE:
        NS_FATAL_ERROR("the necessary NAS signaling to activate a bearer after the initial context "
                       "has already been setup is not implemented");
        break;

    default:
        BearerToBeActivated btba;
        btba.bearer = bearer;
        btba.tft = tft;
        m_bearersToBeActivatedList.push_back(btba);
        m_bearersToBeActivatedListForReconnection.push_back(btba);
        break;
    }
}

bool
NrEpcUeNas::Send(Ptr<Packet> packet, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << packet << protocolNumber);

    switch (m_state)
    {
    case ACTIVE: {
        uint32_t id = m_tftClassifier.Classify(packet, NrEpcTft::UPLINK, protocolNumber);
        NS_ASSERT((id & 0xFFFFFF00) == 0);
        auto bid = (uint8_t)(id & 0x000000FF);
        if (bid == 0)
        {
            NS_LOG_DEBUG("No bearer ID found; returning false");
            return false;
        }
        else
        {
            NS_LOG_DEBUG("Bearer ID " << +bid << " found; sending packet");
            m_asSapProvider->SendData(packet, bid);
            return true;
        }
    }
    break;

    default:
        NS_LOG_WARN(this << " NAS OFF, discarding packet");
        return false;
    }
}

void
NrEpcUeNas::DoNotifyConnectionSuccessful()
{
    NS_LOG_FUNCTION(this);

    SwitchToState(ACTIVE); // will eventually activate dedicated bearers
}

void
NrEpcUeNas::DoNotifyConnectionFailed()
{
    NS_LOG_FUNCTION(this);

    // immediately retry the connection
    Simulator::ScheduleNow(&NrAsSapProvider::Connect, m_asSapProvider);
}

void
NrEpcUeNas::DoRecvData(Ptr<Packet> packet)
{
    NS_LOG_FUNCTION(this << packet);
    m_forwardUpCallback(packet);
}

void
NrEpcUeNas::DoNotifyConnectionReleased()
{
    NS_LOG_FUNCTION(this);
    // remove tfts
    while (m_bidCounter > 0)
    {
        m_tftClassifier.Delete(m_bidCounter);
        m_bidCounter--;
    }
    // restore the bearer list to be activated for the next RRC connection
    m_bearersToBeActivatedList = m_bearersToBeActivatedListForReconnection;

    Disconnect();
}

void
NrEpcUeNas::DoActivateEpsBearer(NrEpsBearer bearer, Ptr<NrEpcTft> tft)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(m_bidCounter < 11, "cannot have more than 11 EPS bearers");
    uint8_t bid = ++m_bidCounter;
    NS_LOG_INFO("Adding bearer id " << +bid << " for Tft with " << tft->GetPacketFilters().size()
                                    << " filters");
    m_tftClassifier.Add(tft, bid);
}

NrEpcUeNas::State
NrEpcUeNas::GetState() const
{
    NS_LOG_FUNCTION(this);
    return m_state;
}

void
NrEpcUeNas::SwitchToState(State newState)
{
    NS_LOG_FUNCTION(this << ToString(newState));
    State oldState = m_state;
    m_state = newState;
    NS_LOG_INFO("IMSI " << m_imsi << " NAS " << ToString(oldState) << " --> "
                        << ToString(newState));
    m_stateTransitionCallback(oldState, newState);

    // actions to be done when entering a new state:
    switch (m_state)
    {
    case ACTIVE:
        for (auto it = m_bearersToBeActivatedList.begin(); it != m_bearersToBeActivatedList.end();
             m_bearersToBeActivatedList.erase(it++))
        {
            DoActivateEpsBearer(it->bearer, it->tft);
        }
        break;

    default:
        break;
    }
}

} // namespace ns3
