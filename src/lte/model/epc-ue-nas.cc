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
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#include <ns3/fatal-error.h>
#include <ns3/log.h>

#include <ns3/epc-helper.h>

#include "lte-enb-net-device.h"
#include "epc-ue-nas.h"
#include "lte-as-sap.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv6-header.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/packet.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("EpcUeNas");



/// Map each of UE NAS states to its string representation.
static const std::string g_ueNasStateName[EpcUeNas::NUM_STATES] =
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
static inline const std::string & ToString (EpcUeNas::State s)
{
  return g_ueNasStateName[s];
}




NS_OBJECT_ENSURE_REGISTERED (EpcUeNas);

EpcUeNas::EpcUeNas ()
  : m_state (OFF),
  m_csgId (0),
  m_asSapProvider (0),
  m_bidCounter (0)
{
  NS_LOG_FUNCTION (this);
  m_asSapUser = new MemberLteAsSapUser<EpcUeNas> (this);
}


EpcUeNas::~EpcUeNas ()
{
  NS_LOG_FUNCTION (this);
}

void
EpcUeNas::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_asSapUser;
}

TypeId
EpcUeNas::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EpcUeNas")
    .SetParent<Object> ()
    .SetGroupName ("Lte")
    .AddConstructor<EpcUeNas> ()
    .AddTraceSource ("StateTransition",
                     "fired upon every UE NAS state transition",
                     MakeTraceSourceAccessor (&EpcUeNas::m_stateTransitionCallback),
                     "ns3::EpcUeNas::StateTracedCallback")
  ;
  return tid;
}

void
EpcUeNas::SetDevice (Ptr<NetDevice> dev)
{
  NS_LOG_FUNCTION (this << dev);
  m_device = dev;
}

void
EpcUeNas::SetImsi (uint64_t imsi)
{
  NS_LOG_FUNCTION (this << imsi);
  m_imsi = imsi;
}

void
EpcUeNas::SetCsgId (uint32_t csgId)
{
  NS_LOG_FUNCTION (this << csgId);
  m_csgId = csgId;
  m_asSapProvider->SetCsgWhiteList (csgId);
}

uint32_t
EpcUeNas::GetCsgId () const
{
  NS_LOG_FUNCTION (this);
  return m_csgId;
}

void
EpcUeNas::SetAsSapProvider (LteAsSapProvider* s)
{
  NS_LOG_FUNCTION (this << s);
  m_asSapProvider = s;
}

LteAsSapUser*
EpcUeNas::GetAsSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_asSapUser;
}

void
EpcUeNas::SetForwardUpCallback (Callback <void, Ptr<Packet> > cb)
{
  NS_LOG_FUNCTION (this);
  m_forwardUpCallback = cb;
}

void
EpcUeNas::SetForwardUpCallback (Ptr<LteSlTft> tft, Callback <void, Ptr<Packet> > cb)
{
  NS_LOG_FUNCTION (this);
  SlCallbackInfo info;
  info.tft = tft;
  info.cb = cb;
  m_slForwardUpCallbackList.push_back (info);
}

void
EpcUeNas::RemoveForwardUpCallback (Ptr<LteSlTft> tft)
{
  NS_LOG_FUNCTION (this);
  
  for (std::list<SlCallbackInfo>::iterator it = m_slForwardUpCallbackList.begin ();
       it != m_slForwardUpCallbackList.end ();
       it++)
    {
      if ((*it).tft->Equals(tft)) 
        { 
          m_slForwardUpCallbackList.erase(it); return; 
        }
    }
  
  NS_ABORT_MSG ("TFT and associated callback not present in list!");
}

void
EpcUeNas::StartCellSelection (uint32_t dlEarfcn)
{
  NS_LOG_FUNCTION (this << dlEarfcn);
  m_asSapProvider->StartCellSelection (dlEarfcn);
}

void
EpcUeNas::Connect ()
{
  NS_LOG_FUNCTION (this);

  // tell RRC to go into connected mode
  m_asSapProvider->Connect ();
}

void
EpcUeNas::Connect (uint16_t cellId, uint32_t dlEarfcn)
{
  NS_LOG_FUNCTION (this << cellId << dlEarfcn);

  // force the UE RRC to be camped on a specific eNB
  m_asSapProvider->ForceCampedOnEnb (cellId, dlEarfcn);

  // tell RRC to go into connected mode
  m_asSapProvider->Connect ();
}


void
EpcUeNas::Disconnect ()
{
  NS_LOG_FUNCTION (this);
  SwitchToState (OFF);
  m_asSapProvider->Disconnect ();
}


void
EpcUeNas::ActivateEpsBearer (EpsBearer bearer, Ptr<EpcTft> tft)
{
  NS_LOG_FUNCTION (this);
  switch (m_state)
    {
    case ACTIVE:
      NS_FATAL_ERROR ("the necessary NAS signaling to activate a bearer after the initial context has already been setup is not implemented");
      break;

    default:
      BearerToBeActivated btba;
      btba.bearer = bearer;
      btba.tft = tft;
      m_bearersToBeActivatedList.push_back (btba);
      m_bearersToBeActivatedListForReconnection.push_back (btba);
      break;
    }
}

bool
EpcUeNas::Send (Ptr<Packet> packet, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << packet << protocolNumber);

  switch (m_state)
    {
    case ACTIVE:
      {
        //First Check if there is any sidelink bearer for the destination
        //otherwise it may use the default bearer
        Ptr<Packet> pCopy = packet->Copy ();
        if (protocolNumber == Ipv4L3Protocol::PROT_NUMBER)
          {
            Ipv4Header ipv4Header;
            pCopy->RemoveHeader (ipv4Header);
            for (std::list<Ptr<LteSlTft> >::iterator it = m_slBearersActivatedList.begin ();
                 it != m_slBearersActivatedList.end ();
                 it++)
              {
                if ((*it)->Matches (ipv4Header.GetSource (), ipv4Header.GetDestination ()))
                  {
                    //Found sidelink
                    m_asSapProvider->SendDataToGroup (packet, (*it)->GetRemoteL2Address ());
                    return true;
                  }
              }
            //check if pending
            for (std::list<Ptr<LteSlTft> >::iterator it = m_pendingSlBearersList.begin ();
                 it != m_pendingSlBearersList.end ();
                 it++)
              {
                if ((*it)->Matches (ipv4Header.GetSource (), ipv4Header.GetDestination ()))
                  {
                    NS_LOG_WARN (this << "Matching sidelink bearer still pending, discarding packet");
                    return false;
                  }
              }
          }
        if (protocolNumber == Ipv6L3Protocol::PROT_NUMBER)
          {
            Ipv6Header ipv6Header;
            pCopy->RemoveHeader (ipv6Header);
            for (std::list<Ptr<LteSlTft> >::iterator it = m_slBearersActivatedList.begin ();
                 it != m_slBearersActivatedList.end ();
                 it++)
              {
                if ((*it)->Matches (ipv6Header.GetSource (), ipv6Header.GetDestination ()))
                  {
                    //Found sidelink
                    m_asSapProvider->SendDataToGroup (packet, (*it)->GetRemoteL2Address ());
                    return true;
                  }
              }
            //check if pending
            for (std::list<Ptr<LteSlTft> >::iterator it = m_pendingSlBearersList.begin ();
                 it != m_pendingSlBearersList.end ();
                 it++)
              {
                if ((*it)->Matches (ipv6Header.GetSource (), ipv6Header.GetDestination ()))
                  {
                    NS_LOG_WARN (this << "Matching sidelink bearer still pending, discarding packet");
                    return false;
                  }
              }
          }
        //No sidelink found
        NS_LOG_DEBUG ("No sidelink bearer found. Use uplink");
        uint32_t id = m_tftClassifier.Classify (packet, EpcTft::UPLINK, protocolNumber);
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
        if (protocolNumber == Ipv4L3Protocol::PROT_NUMBER)
          {
            Ipv4Header ipv4Header;
            pCopy->RemoveHeader (ipv4Header);
            for (std::list<Ptr<LteSlTft> >::iterator it = m_slBearersActivatedList.begin ();
                 it != m_slBearersActivatedList.end ();
                 it++)
              {
                if ((*it)->Matches (ipv4Header.GetSource (), ipv4Header.GetDestination ()))
                  {
                    //Found sidelink
                    m_asSapProvider->SendDataToGroup (packet, (*it)->GetRemoteL2Address ());
                    return true;
                  }
              }
          }
        if (protocolNumber == Ipv6L3Protocol::PROT_NUMBER)
          {
            Ipv6Header ipv6Header;
            pCopy->RemoveHeader (ipv6Header);
            for (std::list<Ptr<LteSlTft> >::iterator it = m_slBearersActivatedList.begin ();
                 it != m_slBearersActivatedList.end ();
                 it++)
              {
                if ((*it)->Matches (ipv6Header.GetSource (), ipv6Header.GetDestination ()))
                  {
                    //Found sidelink
                    m_asSapProvider->SendDataToGroup (packet, (*it)->GetRemoteL2Address ());
                    return true;
                  }
              }
          }
      }
    default:
      NS_LOG_WARN (this << " NAS neither OFF nor ACTIVE, or Sidelink bearer not found, discarding packet");
      return false;
      break;
    }
}

void
EpcUeNas::DoNotifyConnectionSuccessful ()
{
  NS_LOG_FUNCTION (this);

  SwitchToState (ACTIVE); // will eventually activate dedicated bearers
}

void
EpcUeNas::DoNotifyConnectionFailed ()
{
  NS_LOG_FUNCTION (this);

  // immediately retry the connection
  Simulator::ScheduleNow (&LteAsSapProvider::Connect, m_asSapProvider);
}

void
EpcUeNas::DoRecvData (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  //Check maps for sidelink
  Ptr<Packet> pCopy = packet->Copy ();
  Ipv6Header ipv6Header;
  pCopy->RemoveHeader (ipv6Header);

  for (std::list<SlCallbackInfo>::iterator it = m_slForwardUpCallbackList.begin ();
       it != m_slForwardUpCallbackList.end ();
       it++)
    {
      if ((*it).tft->Matches (ipv6Header.GetDestination (), ipv6Header.GetSource ()))
        {
          //Found sidelink
          NS_LOG_INFO ("Forwarding packet from " << ipv6Header.GetSource () << " to " << ipv6Header.GetDestination () << " on sidelink NetDevice");
          (*it).cb (packet);
          return;
        }
    }


  m_forwardUpCallback (packet);
}

void
EpcUeNas::DoNotifyConnectionReleased ()
{
  NS_LOG_FUNCTION (this);
  // remove tfts
  while (m_bidCounter > 0)
    {
      m_tftClassifier.Delete (m_bidCounter);
      m_bidCounter--;
    }
  //restore the bearer list to be activated for the next RRC connection
  m_bearersToBeActivatedList = m_bearersToBeActivatedListForReconnection;

  Disconnect ();
}

void
EpcUeNas::DoActivateEpsBearer (EpsBearer bearer, Ptr<EpcTft> tft)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (m_bidCounter < 11, "cannot have more than 11 EPS bearers");
  uint8_t bid = ++m_bidCounter;
  m_tftClassifier.Add (tft, bid);
}

EpcUeNas::State
EpcUeNas::GetState () const
{
  NS_LOG_FUNCTION (this);
  return m_state;
}

void
EpcUeNas::SwitchToState (State newState)
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
      for (std::list<BearerToBeActivated>::iterator it = m_bearersToBeActivatedList.begin ();
           it != m_bearersToBeActivatedList.end ();
           m_bearersToBeActivatedList.erase (it++))
        {
          DoActivateEpsBearer (it->bearer, it->tft);
        }
      break;

    default:
      break;
    }

}

void
EpcUeNas::ActivateSidelinkBearer (Ptr<LteSlTft> tft)
{
  NS_LOG_FUNCTION (this);
  //regardless of the state we need to request RRC to setup the bearer
  //for in coverage case, it will trigger communication with the eNodeb
  //for out of coverage, it will trigger the use of preconfiguration
  m_pendingSlBearersList.push_back (tft);
  m_asSapProvider->ActivateSidelinkRadioBearer (tft->GetRemoteL2Address (), tft->isTransmit (), tft->isReceive ());
}

void
EpcUeNas::DeactivateSidelinkBearer (Ptr<LteSlTft> tft)
{
  NS_LOG_FUNCTION (this);
  for (std::list<Ptr<LteSlTft> >::iterator it = m_slBearersActivatedList.begin ();
       it != m_slBearersActivatedList.end ();
       it++)
    {
      if (*it == tft)
        {
          NS_LOG_LOGIC ("Found tft to remove for group " << tft->GetRemoteL2Address ());
          //found the sidelink to remove
          m_asSapProvider->DeactivateSidelinkRadioBearer (tft->GetRemoteL2Address ());
          m_slBearersActivatedList.erase (it);
          break;
        }
    }
}

void
EpcUeNas::DoNotifySidelinkRadioBearerActivated (uint32_t group)
{
  NS_LOG_FUNCTION (this);

  std::list<Ptr<LteSlTft> >::iterator it = m_pendingSlBearersList.begin ();
  while (it != m_pendingSlBearersList.end ())
    {
      if ((*it)->GetRemoteL2Address () == group)
        {
          //Found sidelink
          m_slBearersActivatedList.push_back (*it);
          it = m_pendingSlBearersList.erase (it);
        }
      else
        {
          it++;
        }
    }
}

} // namespace ns3

