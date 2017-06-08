/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 */

#include <ns3/fatal-error.h>
#include <ns3/log.h>

#include <ns3/nist-epc-helper.h>

#include "nist-lte-enb-net-device.h"
#include "nist-epc-ue-nas.h"
#include "nist-lte-as-sap.h"
#include "ns3/ipv4-header.h"
#include "ns3/packet.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistEpcUeNas");



/// Map each of UE NAS states to its string representation.
static const std::string g_ueNasStateName[NistEpcUeNas::NUM_STATES] =
{
  "OFF",
  "ATTACHING",
  "IDLE_REGISTERED",
  "CONNECTING_TO_EPC",
  "ACTIVE"
};

/**
 * \param s The UE NAS state.
 * \return The string representation of the given state.
 */
static inline const std::string & ToString (NistEpcUeNas::State s)
{
  return g_ueNasStateName[s];
}




NS_OBJECT_ENSURE_REGISTERED (NistEpcUeNas);

NistEpcUeNas::NistEpcUeNas ()
  : m_state (OFF),
    m_csgId (0),
    m_asSapProvider (0),
    m_bidCounter (0)
{
  NS_LOG_FUNCTION (this);
  m_asSapUser = new NistMemberLteAsSapUser<NistEpcUeNas> (this);
}


NistEpcUeNas::~NistEpcUeNas ()
{
  NS_LOG_FUNCTION (this);
}

void
NistEpcUeNas::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_asSapUser;
}

TypeId
NistEpcUeNas::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistEpcUeNas")
    .SetParent<Object> ()
    .AddConstructor<NistEpcUeNas> ()
    .AddTraceSource ("StateTransition",
                     "fired upon every UE NAS state transition",
                     MakeTraceSourceAccessor (&NistEpcUeNas::m_stateTransitionCallback),
                     "ns3::NistEpcUeNas::StateTracedCallback")
  ;
  return tid;
}

void 
NistEpcUeNas::SetDevice (Ptr<NetDevice> dev)
{
  NS_LOG_FUNCTION (this << dev);
  m_device = dev;
}

void 
NistEpcUeNas::SetImsi (uint64_t imsi)
{
  NS_LOG_FUNCTION (this << imsi);
  m_imsi = imsi;
}

void
NistEpcUeNas::SetCsgId (uint32_t csgId)
{
  NS_LOG_FUNCTION (this << csgId);
  m_csgId = csgId;
  m_asSapProvider->SetCsgWhiteList (csgId);
}

uint32_t
NistEpcUeNas::GetCsgId () const
{
  NS_LOG_FUNCTION (this);
  return m_csgId;
}

void
NistEpcUeNas::SetAsSapProvider (NistLteAsSapProvider* s)
{
  NS_LOG_FUNCTION (this << s);
  m_asSapProvider = s;
}

NistLteAsSapUser*
NistEpcUeNas::GetAsSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_asSapUser;
}

void
NistEpcUeNas::SetForwardUpCallback (Callback <void, Ptr<Packet> > cb)
{
  NS_LOG_FUNCTION (this);
  m_forwardUpCallback = cb;
}

void
NistEpcUeNas::StartCellSelection (uint16_t dlEarfcn)
{
  NS_LOG_FUNCTION (this << dlEarfcn);
  m_asSapProvider->StartCellSelection (dlEarfcn);
}

void 
NistEpcUeNas::Connect ()
{
  NS_LOG_FUNCTION (this);

  // tell RRC to go into connected mode
  m_asSapProvider->Connect ();
}

void
NistEpcUeNas::Connect (uint16_t cellId, uint16_t dlEarfcn)
{
  NS_LOG_FUNCTION (this << cellId << dlEarfcn);

  // force the UE RRC to be camped on a specific eNB
  m_asSapProvider->ForceCampedOnEnb (cellId, dlEarfcn);

  // tell RRC to go into connected mode
  m_asSapProvider->Connect ();
}


void 
NistEpcUeNas::Disconnect ()
{
  NS_LOG_FUNCTION (this);
  m_asSapProvider->Disconnect ();
  SwitchToState (OFF);
}


void 
NistEpcUeNas::ActivateNistEpsBearer (NistEpsBearer bearer, Ptr<NistEpcTft> tft)
{
  NS_LOG_FUNCTION (this);
  switch (m_state)
    {
    case ACTIVE:
      NS_FATAL_ERROR ("the necessary NAS signaling to activate a bearer after the initial context has already been setup is not implemented");
      break;

    default:
      NistBearerToBeActivated btba;
      btba.bearer = bearer;
      btba.tft = tft;
      m_bearersToBeActivatedList.push_back (btba);
      break;
    }
}

bool
NistEpcUeNas::Send (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  switch (m_state)
    {
    case ACTIVE:
      {
        //First Check if there is any sidelink bearer for the destination
        //otherwise it may use the default bearer 
        Ptr<Packet> pCopy = packet->Copy ();
        Ipv4Header ipv4Header;
        pCopy->RemoveHeader (ipv4Header);
        for (std::list<Ptr<NistSlTft> >::iterator it = m_slBearersActivatedList.begin ();
             it != m_slBearersActivatedList.end ();
             it++)
          {
            if ((*it)->Matches(ipv4Header.GetDestination ())) {
              //Found sidelink
              m_asSapProvider->SendData (packet, (*it)->GetGroupL2Address());
              return true;
            }
          }
        //check if pending
        for (std::list<Ptr<NistSlTft> >::iterator it = m_pendingSlBearersList.begin ();
             it != m_pendingSlBearersList.end ();
             it++)
          {
            if ((*it)->Matches(ipv4Header.GetDestination ()))
              {
                NS_LOG_WARN (this << "Matching sidelink bearer still pending, discarding packet");
                return false;
              }
          }
        //No sidelink found
        uint32_t id = m_tftClassifier.Classify (packet, NistEpcTft::UPLINK);
        NS_ASSERT ((id & 0xFFFFFF00) == 0);
        uint8_t bid = (uint8_t) (id & 0x000000FF);
        if (bid == 0)
          {
            return false;
          }
        else
          {
            m_asSapProvider->SendData (packet, bid); 
            return true;
          }
      }
      break;
    case OFF:
      {
        //Check if there is any sidelink bearer for the destination
        Ptr<Packet> pCopy = packet->Copy ();
        Ipv4Header ipv4Header;
        pCopy->RemoveHeader (ipv4Header);
        for (std::list<Ptr<NistSlTft> >::iterator it = m_slBearersActivatedList.begin ();
             it != m_slBearersActivatedList.end ();
             it++)
          {
            if ((*it)->Matches(ipv4Header.GetDestination ())) {
              //Found sidelink
              m_asSapProvider->SendData (packet, (*it)->GetGroupL2Address());
              return true;
            }
          } 
      }
    default:
      NS_LOG_WARN (this << " NAS NOT OFF or ACTIVE, or sidelink bearer not found, discarding packet");
      return false;
      break;
    }
}

void 
NistEpcUeNas::DoNotifyConnectionSuccessful ()
{
  NS_LOG_FUNCTION (this);

  SwitchToState (ACTIVE); // will eventually activate dedicated bearers
}

void
NistEpcUeNas::DoNotifyConnectionFailed ()
{
  NS_LOG_FUNCTION (this);

  // immediately retry the connection
  Simulator::ScheduleNow (&NistLteAsSapProvider::Connect, m_asSapProvider);
}

void
NistEpcUeNas::DoRecvData (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);
  m_forwardUpCallback (packet);
}

void 
NistEpcUeNas::DoNotifyConnectionReleased ()
{
  NS_LOG_FUNCTION (this);
  SwitchToState (OFF);
}

void 
NistEpcUeNas::DoActivateNistEpsBearer (NistEpsBearer bearer, Ptr<NistEpcTft> tft)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (m_bidCounter < 11, "cannot have more than 11 EPS bearers");
  uint8_t bid = ++m_bidCounter;
  m_tftClassifier.Add (tft, bid);
}

NistEpcUeNas::State
NistEpcUeNas::GetState () const
{
  NS_LOG_FUNCTION (this);
  return m_state;
}

void 
NistEpcUeNas::SwitchToState (State newState)
{
  NS_LOG_FUNCTION (this << ToString (newState));
  State oldState = m_state;
  m_state = newState;
  NS_LOG_INFO ("IMSI " << m_imsi << " NAS " << ToString (oldState) << " --> " << ToString (newState));
  m_stateTransitionCallback (oldState, newState);

  // actions to be done when entering a new state:
  switch (m_state)
    {
    case ACTIVE:
      for (std::list<NistBearerToBeActivated>::iterator it = m_bearersToBeActivatedList.begin ();
           it != m_bearersToBeActivatedList.end ();
           m_bearersToBeActivatedList.erase (it++))
        {
          DoActivateNistEpsBearer (it->bearer, it->tft);
        }
      break;

    default:
      break;
    }

}

void 
NistEpcUeNas::ActivateSidelinkBearer (Ptr<NistSlTft> tft)
{
  NS_LOG_FUNCTION (this);
  //regardless of the state we need to request RRC to setup the bearer
  //for in coverage case, it will trigger communication with the eNodeb
  //for out of coverage, it will trigger the use of preconfiguration
  m_pendingSlBearersList.push_back (tft);
  m_asSapProvider->ActivateSidelinkRadioBearer (tft->GetGroupL2Address(), tft->isTransmit(), tft->isReceive()); 
}

void 
NistEpcUeNas::DeactivateSidelinkBearer (Ptr<NistSlTft> tft)
{
  NS_LOG_FUNCTION (this);
  for (std::list<Ptr<NistSlTft> >::iterator it = m_slBearersActivatedList.begin ();
       it != m_slBearersActivatedList.end ();
       it++)
    {
      if (*it==tft) {
        NS_LOG_LOGIC ("Found tft to remove for group " << tft->GetGroupL2Address());
        //found the sidelink to remove
        m_asSapProvider->DeactivateSidelinkRadioBearer (tft->GetGroupL2Address());
        m_slBearersActivatedList.erase (it);
        break;
      }
    } 
}
  
void 
NistEpcUeNas::DoNotifySidelinkRadioBearerActivated (uint32_t group)
{
  NS_LOG_FUNCTION (this);
  
  std::list<Ptr<NistSlTft> >::iterator it = m_pendingSlBearersList.begin ();
  while (it != m_pendingSlBearersList.end ())
    {
      if ((*it)->GetGroupL2Address()==group) {
        //Found sidelink
        m_slBearersActivatedList.push_back (*it);
        it = m_pendingSlBearersList.erase (it);
      } else {
        it++; 
      }
    }
}
  
void 
NistEpcUeNas::AddDiscoveryApps (std::list<uint32_t> apps, bool rxtx)
{
  m_asSapProvider->AddDiscoveryApps (apps, rxtx);
}
void 
NistEpcUeNas::RemoveDiscoveryApps (std::list<uint32_t> apps, bool rxtx)
{
  m_asSapProvider->RemoveDiscoveryApps (apps, rxtx);
}

} // namespace ns3

