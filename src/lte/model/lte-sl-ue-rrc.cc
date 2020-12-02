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

#include "lte-sl-ue-rrc.h"
#include "lte-sl-ue-controller.h"
#include "lte-sl-o2o-comm-params.h"
#include <ns3/lte-ue-rrc.h>

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/abort.h>

#include <ns3/pointer.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/simulator.h>

#include <ns3/lte-rrc-sap.h>

#include <algorithm>
#include <ns3/boolean.h>

#include "lte-sl-ue-ctrl-sap.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlUeRrc");
NS_OBJECT_ENSURE_REGISTERED (LteSlUeRrc);

TypeId LteSlUeRrc::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::LteSlUeRrc")
    .SetParent<Object> ()
    .AddConstructor<LteSlUeRrc> ()
    .AddAttribute ("Rrc",
                   "The rrc layer",
                   PointerValue (),
                   MakePointerAccessor (&LteSlUeRrc::m_rrc),
                   MakePointerChecker <LteUeRrc> ())
    .AddAttribute ("SlController",
                   "The sidelink controller",
                   PointerValue (),
                   MakePointerAccessor (&LteSlUeRrc::m_controller),
                   MakePointerChecker <LteSlUeController> ())
    .AddAttribute ("RelaySolFreq",
                   "Frequency of Model B Relay Discovery Solicitation messages (in number of discovery periods)",
                   UintegerValue (2),
                   MakeUintegerAccessor (&LteSlUeRrc::m_relaySolFreq),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("RuirqEnabled",
                   "Activate or deactivate Remote UE information request procedure",
                   BooleanValue (false),
                   MakeBooleanAccessor (&LteSlUeRrc::m_isRuirqEnabled),
                   MakeBooleanChecker ())
    .AddTraceSource ("PC5SignalingPacketTrace",
                     "The PC5 Signaling Packet Trace",
                     MakeTraceSourceAccessor (&LteSlUeRrc::m_pc5SignalingPacketTrace),
                     "ns3::LteSlUeRrc::PC5SignalingPacketTracedCallback")
  ;
  return tid;
}
LteSlUeRrc::LteSlUeRrc ()
  : m_slEnabled (false),
  m_discEnabled (false),
  m_discTxResources (1),
  m_pc5SignalingSeqNum (0)
{
  NS_LOG_FUNCTION (this);
  m_slUeCtrlSapUser = new MemberLteSlUeCtrlSapUser<LteSlUeRrc> (this);
  m_rand = CreateObject<UniformRandomVariable> ();
}

void
LteSlUeRrc::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
}

LteSlUeRrc::~LteSlUeRrc (void)
{
  NS_LOG_FUNCTION (this);
}

void
LteSlUeRrc::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_activeDiscTxPool = 0;
  m_rrc = 0;
  rxPools.clear ();
  monitorPools.clear ();
  delete m_slUeCtrlSapUser;
}
/**
 * \brief makes a copy of the sidelink configuration
 * \return a copy of the sidelink configuration
 */
Ptr<LteSlUeRrc>
LteSlUeRrc::Copy ()
{
  NS_LOG_FUNCTION (this);
  Ptr<LteSlUeRrc> copy = CreateObject<LteSlUeRrc> ();
  //copy internal values
  copy->m_slEnabled = m_slEnabled;
  copy->m_slMap = m_slMap;
  return copy;
}

void
LteSlUeRrc::SetSlEnabled (bool status)
{
  NS_LOG_FUNCTION (this << status);
  m_slEnabled = status;
}

bool
LteSlUeRrc::IsSlEnabled ()
{
  return m_slEnabled;
}

void
LteSlUeRrc::SetDiscEnabled (bool status)
{
  NS_LOG_FUNCTION (this << status);
  m_discEnabled = status;
}

bool
LteSlUeRrc::IsDiscEnabled ()
{
  return m_discEnabled;
}

void
LteSlUeRrc::SetSlPreconfiguration (LteRrcSap::SlPreconfiguration preconfiguration)
{
  NS_LOG_FUNCTION (this);
  m_preconfiguration = preconfiguration;
}

LteRrcSap::SlPreconfiguration
LteSlUeRrc::GetSlPreconfiguration ()
{
  NS_LOG_FUNCTION (this);
  return m_preconfiguration;
}

void
LteSlUeRrc::SetSourceL2Id (uint32_t src)
{
  NS_LOG_FUNCTION (this);
  m_sourceL2Id = src;
}

uint32_t
LteSlUeRrc::GetSourceL2Id ()
{
  NS_LOG_FUNCTION (this);
  return m_sourceL2Id;
}

bool
LteSlUeRrc::IsTxInterested ()
{
  //Loop through each bearer to see if one is interested to transmit
  std::map <uint32_t, std::map <uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > > >::iterator srcIt = m_slrbMap.find (m_sourceL2Id);
  if (srcIt == m_slrbMap.end ())
    {
      return false;
    }
  return m_slrbMap[m_sourceL2Id].size () > 0;
}

bool
LteSlUeRrc::IsRxInterested ()
{
  return m_rxGroup.size () > 0;
}

bool
LteSlUeRrc::IsMonitoringInterested ()
{
  //Device is monitoring if model A/monitoring OR model B (since it needs to monitor for requests/responses)
  bool monitoring = false;
  //check applications
  for (std::map <uint32_t, AppServiceInfo>::iterator itInfo = m_appServicesMap.begin (); itInfo != m_appServicesMap.end (); ++itInfo)
    {
      if ( itInfo->second.role != Announcing )
        {
          monitoring = true;
          break;
        }
    }

  if (!monitoring)
    {

      //check relays
      for (std::map<uint32_t, RelayServiceInfo>::iterator itInfo = m_relayServicesMap.begin (); itInfo != m_relayServicesMap.end (); ++itInfo)
        {
          if ( itInfo->second.role == RemoteUE || (itInfo->second.role == RelayUE && itInfo->second.model == ModelB) )
            {
              monitoring = true;
              break;
            }
        }
    }
  return monitoring;
}

bool
LteSlUeRrc::IsAnnouncingInterested ()
{
  //device is announcing if model A/announcing OR model B (since it needs to send requests/responses)
  //maybe we can use a variable that is update when calling start/stop applications and relays
  bool announcing = false;
  //check applications
  for (std::map <uint32_t, AppServiceInfo>::iterator itInfo = m_appServicesMap.begin (); itInfo != m_appServicesMap.end (); ++itInfo)
    {
      if ( itInfo->second.role != Monitoring )
        {
          announcing = true;
          break;
        }
    }
  if (!announcing)
    { //check relays
      for (std::map<uint32_t, RelayServiceInfo>::iterator itInfo = m_relayServicesMap.begin (); itInfo != m_relayServicesMap.end (); ++itInfo)
        {
          if ( itInfo->second.role == RelayUE || (itInfo->second.role == RemoteUE && itInfo->second.model == ModelB) )
            {
              announcing = true;
              break;
            }
        }
    }
  return announcing;
}


std::list<uint32_t>
LteSlUeRrc::GetTxDestinations ()
{
  std::list<uint32_t> destinations;

  //Loop through each bearer to see if one is interested to transmit
  std::map <uint32_t, std::map <uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > > >::iterator srcIt = m_slrbMap.find (m_sourceL2Id);
  if (srcIt != m_slrbMap.end ())
    {
      //Loop through each bearer to see if one is interested to transmit
      std::map <uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > >::iterator it;
      for (it = m_slrbMap[m_sourceL2Id].begin (); it != m_slrbMap[m_sourceL2Id].end (); it++)
        {
          destinations.push_back (it->first);
        }
    }
  return destinations;
}

uint8_t
LteSlUeRrc::GetDiscTxResources ()
{
  return m_discTxResources;
}

void
LteSlUeRrc::SetDiscTxResources (uint8_t numDiscTxRes)
{
  m_discTxResources = numDiscTxRes;
}

uint16_t
LteSlUeRrc::GetDiscInterFreq ()
{
  return m_discInterFreq;
}
void
LteSlUeRrc::SetDiscInterFreq (uint16_t ulEarfcn)
{
  m_discInterFreq = ulEarfcn;
}

bool
LteSlUeRrc::AddSidelinkRadioBearer (Ptr<LteSidelinkRadioBearerInfo> slb)
{
  std::map <uint32_t, std::map <uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > > >::iterator srcIt = m_slrbMap.find (slb->m_sourceL2Id);
  if (srcIt == m_slrbMap.end ())
    {
      //must insert map
      std::map <uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > > empty;

      m_slrbMap.insert (std::pair <uint32_t, std::map <uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > > > (slb->m_sourceL2Id, empty));
      NS_LOG_LOGIC ("First SLRB for source " << slb->m_sourceL2Id);
    }

  std::map <uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > >::iterator groupIt = m_slrbMap[slb->m_sourceL2Id].find (slb->m_destinationL2Id);
  if (groupIt == m_slrbMap[slb->m_sourceL2Id].end ())
    {
      NS_LOG_LOGIC ("First SLRB for destination " << slb->m_destinationL2Id);
      std::list < Ptr<LteSidelinkRadioBearerInfo> > emptyList;
      m_slrbMap[slb->m_sourceL2Id].insert (std::pair<uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > > (slb->m_destinationL2Id, emptyList));
    }

  NS_LOG_LOGIC ("Adding SLRB " << slb->m_sourceL2Id << "->" << slb->m_destinationL2Id << " LCID=" << (uint16_t)slb->m_logicalChannelIdentity);
  m_slrbMap[slb->m_sourceL2Id][slb->m_destinationL2Id].push_back (slb);
  return true;
}

bool
LteSlUeRrc::DeleteSidelinkRadioBearer (uint32_t src, uint32_t group)
{
  NS_ASSERT_MSG (m_slrbMap[src][group].size () <= 1, "Currently supporting only one SLRB per pair of source/destination");
  bool deleted = m_slrbMap[src].erase (group) > 0;
  if (m_slrbMap[src].size () == 0)
    {
      //can delete the source as well
      m_slrbMap.erase (src);
    }
  //remove from receiving group if present
  m_rxGroup.remove (group);

  return deleted;
}

Ptr<LteSidelinkRadioBearerInfo>
LteSlUeRrc::GetSidelinkRadioBearer (uint32_t src, uint32_t group)
{
  Ptr<LteSidelinkRadioBearerInfo> slrb = nullptr;

  NS_LOG_LOGIC ("Searching SLRB " << src << "->" << group);

  std::map <uint32_t, std::map <uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > > >::iterator srcIt = m_slrbMap.find (src);
  if (srcIt != m_slrbMap.end ())
    {
      std::map <uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > >::iterator srcIt2 = (*srcIt).second.find (group);
      if (srcIt2 != (*srcIt).second.end ())
        {
          slrb = *(m_slrbMap[src][group].begin ());
        }
    }
  return slrb;
}

Ptr<LteSidelinkRadioBearerInfo>
LteSlUeRrc::GetSidelinkRadioBearer (uint32_t group)
{
  return GetSidelinkRadioBearer (m_sourceL2Id, group);
}

void
LteSlUeRrc::StartDiscoveryApps (std::list<uint32_t> appCodes, DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);
  for (std::list<uint32_t>::iterator it = appCodes.begin (); it != appCodes.end (); ++it)
    {
      AppServiceInfo info;
      info.role = role;
      info.appCode = *it;
      info.lastRspTimestamp = Seconds (0);

      NS_LOG_DEBUG ("Adding app code");
      m_appServicesMap.insert ( std::pair <uint32_t, AppServiceInfo> (*it, info) );
    }
}


void
LteSlUeRrc::StopDiscoveryApps (std::list<uint32_t> appCodes, DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);
  std::map <uint32_t, AppServiceInfo>::iterator itInfo;

  for (std::list<uint32_t>::iterator it = appCodes.begin (); it != appCodes.end (); ++it)
    {
      itInfo = m_appServicesMap.find (*it);
      if (itInfo != m_appServicesMap.end ())
        {
          //found app to remove
          m_appServicesMap.erase (itInfo);
          break;
        }
    }
}

bool
LteSlUeRrc::IsMonitoringApp (uint8_t msgType, uint32_t appCode)
{
  NS_LOG_FUNCTION (this);
  //the device is interested in announcement if monitoring, in request if acting as discoveree, and in response if acting as discoverer
  std::map <uint32_t, AppServiceInfo>::iterator itInfo = m_appServicesMap.find (appCode);
  if (itInfo != m_appServicesMap.end ())
    {
      return ( (msgType == LteSlDiscHeader::DISC_OPEN_ANNOUNCEMENT && itInfo->second.role == Monitoring)
               || (msgType == LteSlDiscHeader::DISC_RESTRICTED_QUERY && itInfo->second.role == Discoveree)
               || (msgType == LteSlDiscHeader::DISC_RESTRICTED_RESPONSE && itInfo->second.role == Discoverer) );
    }
  return false;
}

bool
LteSlUeRrc::IsAnnouncingApp (uint32_t appCode)
{
  NS_LOG_FUNCTION (this);
  //the device is monitoring for message for this app if modelA/announcing or model B (discoveree or discoverer)
  //which covers all the cases that the device is not simply announcing
  std::map <uint32_t, AppServiceInfo>::iterator itInfo = m_appServicesMap.find (appCode);
  if (itInfo != m_appServicesMap.end ())
    {
      return itInfo->second.role != Monitoring;
    }
  return false;
}

void
LteSlUeRrc::RecvApplicationServiceDiscovery (uint8_t msgType, uint32_t appCode)
{
  NS_LOG_FUNCTION (this);
  //Check if this is a request I am interested in
  std::map <uint32_t, AppServiceInfo>::iterator itInfo = m_appServicesMap.find (appCode);
  if (itInfo != m_appServicesMap.end ())
    {
      //check if this is a request message for an app for which this UE is a Discoveree
      //also check if a response was already sent in this sidelink period
      if (msgType == LteSlDiscHeader::DISC_RESTRICTED_QUERY && itInfo->second.role == Discoveree)
        {
          if (Simulator::Now ().GetMilliSeconds () > itInfo->second.lastRspTimestamp.GetMilliSeconds () + m_activeDiscTxPool->GetDiscPeriod ())
            {
              TransmitApp (appCode);
              itInfo->second.lastRspTimestamp = Simulator::Now ();
            }
          else
            {
              NS_LOG_DEBUG ("Response already sent within sidelink period duration");
            }
        }
    }
}

void
LteSlUeRrc::StartAnnouncing ()
{
  NS_LOG_FUNCTION (this);
  Time period = MilliSeconds (m_activeDiscTxPool->GetDiscPeriod ());
  //Applications
  for (std::map<uint32_t, AppServiceInfo>::iterator itInfo = m_appServicesMap.begin (); itInfo != m_appServicesMap.end (); ++itInfo)
    {
      if ( itInfo->second.role == Announcing || itInfo->second.role == Discoverer)
        {
          if (!itInfo->second.txTimer.IsRunning ())
            {
              TransmitApp (itInfo->first);
            }
        }
    }

  //Relay
  for (std::map<uint32_t, RelayServiceInfo>::iterator itInfo = m_relayServicesMap.begin (); itInfo != m_relayServicesMap.end (); ++itInfo)
    {
      if ( (itInfo->second.role == RemoteUE && itInfo->second.model == ModelB) || (itInfo->second.role == RelayUE && itInfo->second.model == ModelA) )
        {
          if (!itInfo->second.txTimer.IsRunning ())
            {
              TransmitRelayMessage (itInfo->first);
            }
        }
    }

}

void
LteSlUeRrc::RecordTransmissionOfSidelinkUeInformation ()
{
  m_lastSidelinkUeInformationTime = Simulator::Now ();
}

double
LteSlUeRrc::GetTimeSinceLastTransmissionOfSidelinkUeInformation ()
{
  return (Simulator::Now () - m_lastSidelinkUeInformationTime).GetSeconds ();
}

uint8_t
LteSlUeRrc::GetNextLcid (uint32_t dstL2Id)
{
  //Note: This function supports the fact that multiple bearers can exist between
  //a source and destination. However, the rest of the code currently work
  //with only one LC per destination.

  //find unused the LCID
  uint8_t lcid = 0; //initialize with invalid value

  std::map <uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > >::iterator itDst;
  itDst = m_slrbMap[m_sourceL2Id].find (dstL2Id);
  if (itDst == m_slrbMap[m_sourceL2Id].end ())
    {
      //first time creating a LC for this destination
      lcid = 1;
    }
  else
    {
      //find an id not being used
      for (uint8_t lcidTmp = 1; lcidTmp < 11; lcidTmp++)
        {
          bool found = false;
          std::list < Ptr<LteSidelinkRadioBearerInfo> >::iterator it;
          for (it = itDst->second.begin (); it != itDst->second.end (); it++)
            {
              if ((*it)->m_logicalChannelIdentity == lcid)
                {
                  found = true;
                  break;
                }
            }
          if (!found)
            {
              lcid = lcidTmp;
              break; //avoid increasing lcid
            }
        }
    }
  NS_ASSERT (lcid != 0);
  return lcid;
}

bool
LteSlUeRrc::IsCellBroadcastingSIB18 (uint16_t cellId)
{
  std::map <uint16_t, LteSlCellConfiguration>::iterator it = m_slMap.find (cellId);
  return (it != m_slMap.end () && it->second.haveSib18);
}

bool
LteSlUeRrc::IsCellBroadcastingSIB19 (uint16_t cellId)
{
  std::map <uint16_t, LteSlCellConfiguration>::iterator it = m_slMap.find (cellId);
  return (it != m_slMap.end () && it->second.haveSib19);
}

void
LteSlUeRrc::SetActiveTxDiscoveryPool (Ptr<SidelinkTxDiscResourcePool> pool)
{
  NS_LOG_FUNCTION (this);
  m_activeDiscTxPool = pool;
}

Ptr<SidelinkTxDiscResourcePool>
LteSlUeRrc::GetActiveTxDiscoveryPool ()
{
  NS_LOG_FUNCTION (this);
  return m_activeDiscTxPool;
}

void
LteSlUeRrc::TransmitApp (uint32_t appCode)
{
  NS_LOG_FUNCTION (this);

  std::map <uint32_t, AppServiceInfo>::iterator it;

  it = m_appServicesMap.find (appCode);
  NS_ASSERT_MSG (it != m_appServicesMap.end (), "Unknown application code");
  //build message to transmit
  LteSlDiscHeader discHeader;

  Time period;
  if (it->second.role == Announcing)
    {
      discHeader.SetOpenDiscoveryAnnounceParameters (appCode);
      //time to retransmit
      period = MilliSeconds (m_activeDiscTxPool->GetDiscPeriod ());
      //reschedule
      NS_LOG_DEBUG ("period " << m_activeDiscTxPool->GetDiscPeriod () << ", " << period);
      it->second.txTimer = Simulator::Schedule (period, &LteSlUeRrc::TransmitApp, this, appCode);
    }
  else if (it->second.role == Discoverer)
    {
      discHeader.SetRestrictedDiscoveryQueryParameters (appCode);
      //time to retransmit, includes 2 periods to leave opportunity for a response
      period = MilliSeconds (2 * m_activeDiscTxPool->GetDiscPeriod ());
      //reschedule
      NS_LOG_DEBUG ("2x period " << 2 * m_activeDiscTxPool->GetDiscPeriod () << ", " << period);
      it->second.txTimer = Simulator::Schedule (period, &LteSlUeRrc::TransmitApp, this, appCode);
    }
  else
    {
      NS_ASSERT (it->second.role == Discoveree);
      discHeader.SetRestrictedDiscoveryResponseParameters (appCode);
      //no reschedule
    }
  m_rrc->TransmitDiscoveryMessage (discHeader);
}

void
LteSlUeRrc::StartRelayService (uint32_t serviceCode, LteSlUeRrc::DiscoveryModel model, LteSlUeRrc::RelayRole role)
{
  NS_LOG_FUNCTION (this << serviceCode << model << role);
  NS_ASSERT_MSG (m_relayServicesMap.find (serviceCode) == m_relayServicesMap.end (), "Cannot add already existing service " << serviceCode);

  RelayServiceInfo info;
  info.model = model;
  info.role = role;
  info.serviceCode = serviceCode;
  info.lastRspTimestamp = Seconds (0);

  m_relayServicesMap.insert ( std::pair <uint32_t, RelayServiceInfo> (serviceCode, info) );

}

void
LteSlUeRrc::StopRelayService (uint32_t serviceCode)
{
  NS_LOG_FUNCTION (this << serviceCode);

  std::map <uint32_t, RelayServiceInfo>::iterator it = m_relayServicesMap.find (serviceCode);
  if (it != m_relayServicesMap.end ())
    {
      //stop timer and remove from list
      if (it->second.txTimer.IsRunning ())
        {
          it->second.txTimer.Cancel ();
        }
      m_relayServicesMap.erase (it);
    }

  //The following code is used to implement "Rx Upper, Tx DCR" type scenarios in the One-to-One Communication State Machine
  //When the Remote or the Relay decides to stop its Relay Service, each context in the map would be accessed to send a DirectCommunicationRelease message provided the UE is in the appropriate state to send DCR
  Ptr<LteSlO2oCommParams> o2ocp = CreateObject<LteSlO2oCommParams> ();
  for (std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = m_o2oCommContexts.begin (); it != m_o2oCommContexts.end (); ++it)
    {
      ReleaseO2OConnection (it->second, LteSlO2oCommParams::COMM_NO_LONGER_NEEDED);
    }
}

void
LteSlUeRrc::TransmitRelayMessage (uint32_t serviceCode)
{
  NS_LOG_FUNCTION (this << serviceCode);

  std::map <uint32_t, RelayServiceInfo>::iterator it;

  it = m_relayServicesMap.find (serviceCode);
  NS_ASSERT (it != m_relayServicesMap.end ());
  //build message to transmit
  LteSlDiscHeader discHeader;

  //Right now we only have model A, but additional cases will be added for model B
  if (it->second.model == ModelA && it->second.role == RelayUE)
    {
      discHeader.SetRelayAnnouncementParameters (serviceCode, 0, m_sourceL2Id, 1);

      //reschedule
      Time period = MilliSeconds (m_activeDiscTxPool->GetDiscPeriod ());
      NS_LOG_DEBUG ("period " << m_activeDiscTxPool->GetDiscPeriod () << ", " << period);
      it->second.txTimer = Simulator::Schedule (period, &LteSlUeRrc::TransmitRelayMessage, this, serviceCode);
    }
  else if (it->second.model == ModelB && it->second.role == RemoteUE)
    {
      discHeader.SetRelaySoliciationParameters (serviceCode, 0, m_sourceL2Id);

      //reschedule
      Time period = MilliSeconds (m_relaySolFreq * m_activeDiscTxPool->GetDiscPeriod ());
      NS_LOG_DEBUG (m_relaySolFreq << "x period " << m_relaySolFreq * m_activeDiscTxPool->GetDiscPeriod () << ", " << period);
      it->second.txTimer = Simulator::Schedule (period, &LteSlUeRrc::TransmitRelayMessage, this, serviceCode);
    }
  else
    {
      NS_ASSERT (it->second.model == ModelB && it->second.role == RelayUE);

      discHeader.SetRelayResponseParameters (serviceCode, 0, m_sourceL2Id, 1);

      // no reschedule
    }

  m_rrc->TransmitDiscoveryMessage (discHeader);
}

bool
LteSlUeRrc::IsMonitoringRelayServiceCode (uint8_t msgType, uint32_t serviceCode)
{
  NS_LOG_FUNCTION (this << serviceCode);

  //This function will probably have to be updated to include the role or message type we are looking for
  //for example in model B both devices are monitoring but not for the same type of messages
  std::map <uint32_t, RelayServiceInfo>::iterator it;
  it = m_relayServicesMap.find (serviceCode);
  if (it != m_relayServicesMap.end ())
    {
      return ( (msgType == 145 && it->second.model == ModelA && it->second.role == RemoteUE)
               || (msgType == 146 && it->second.model == ModelB && it->second.role == RemoteUE)
               || (msgType == 150 && it->second.model == ModelB && it->second.role == RelayUE) );
    }
  return false;
}

void
LteSlUeRrc::RecvRelayServiceDiscovery (uint32_t serviceCode, uint64_t announcerInfo, uint32_t proseRelayUeId, uint8_t statusIndicator)
{
  NS_LOG_FUNCTION (this << serviceCode << announcerInfo << proseRelayUeId << (uint16_t)statusIndicator);

  //Here we can have a complex logic that decides if the remote UE needs to connect
  //for now, let's just connect on first discovery received

  //StartRelayDirectCommunication (serviceCode, proseRelayUeId);
  m_slUeCtrlSapProvider->RecvRelayServiceDiscovery (serviceCode, announcerInfo, proseRelayUeId, statusIndicator);
}

void
LteSlUeRrc::RecvRelayServiceDiscovery (uint32_t serviceCode, uint64_t discovererInfo,uint8_t urdsComposition, uint32_t proseRelayUeId)
{
  NS_LOG_FUNCTION (this << serviceCode << discovererInfo << (uint16_t) urdsComposition << proseRelayUeId);

  //Check if this is a request I am interested in
  std::map <uint32_t, RelayServiceInfo>::iterator itInfo = m_relayServicesMap.find (serviceCode);
  if (itInfo != m_relayServicesMap.end ())
    {
      NS_ASSERT (itInfo->second.model == ModelB && itInfo->second.role == RelayUE);
      if (Simulator::Now ().GetMilliSeconds () > itInfo->second.lastRspTimestamp.GetMilliSeconds () + m_activeDiscTxPool->GetDiscPeriod ())
        {
          TransmitRelayMessage (serviceCode);
          itInfo->second.lastRspTimestamp = Simulator::Now ();
        }
      else
        {
          NS_LOG_DEBUG ("Response already sent within sidelink period duration");
        }
    }

}

void
LteSlUeRrc::StartRelayDirectCommunication (uint32_t serviceCode, uint32_t proseRelayUeId)
{
  NS_LOG_FUNCTION (this << proseRelayUeId);

  //Need to create a Rx bearer to listen to messages to itself
  m_rrc->ActivateSidelinkRadioBearer (m_sourceL2Id, false, true);

  //Check that there is no existing link
  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator cIt;
  for (cIt = m_o2oCommContexts.begin (); cIt != m_o2oCommContexts.end (); cIt++)
    {
      if (cIt->first.peerL2Id == proseRelayUeId)
        {
          //found existing context for this relay
          break;
        }
    }
  NS_ASSERT_MSG (cIt == m_o2oCommContexts.end (), "Cannot start relay communication when existing link exists");

  DirectCommunicationRequest dcrq;
  dcrq.SetRelayServiceCode (serviceCode);
  dcrq.SetSequenceNumber (++m_pc5SignalingSeqNum);
  dcrq.SetImsi (m_rrc->GetImsi ());

  LteSlO2oCommParams::LteSlPc5ContextId pc5Context;
  pc5Context.peerL2Id = proseRelayUeId;
  pc5Context.contextId = dcrq.GetSequenceNumber ();

  Ptr<LteSlO2oCommParams> o2ocp = CreateObject<LteSlO2oCommParams>();
  o2ocp->SetState (LteSlO2oCommParams::REMOTE_IDLE);
  o2ocp->SetContextId (pc5Context);
  o2ocp->SetDcrqRetrans (dcrq);

  m_o2oCommContexts.insert (std::pair< LteSlO2oCommParams::LteSlPc5ContextId,Ptr<LteSlO2oCommParams> > (pc5Context,o2ocp));
  m_slUeCtrlSapProvider->Pc5ConnectionStarted (proseRelayUeId, m_sourceL2Id, LteSlUeRrc::RemoteUE);

  //Create a sidelink bearer to transmit messages if not already available
  if (GetSidelinkRadioBearer (m_sourceL2Id, proseRelayUeId) == NULL)
    {
      m_oneToOnePeer.push_back (proseRelayUeId);
      m_rrc->ActivateSidelinkRadioBearer (proseRelayUeId, true, false);
    }
  else
    {
      o2ocp->SetSecurityModeState (LteSlO2oCommParams::EMPTY);

      Ptr<Packet> p = Create<Packet> ();
      p->AddHeader (dcrq);
      m_rrc->SendPc5Signaling (p, proseRelayUeId);
      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationRequest to " << proseRelayUeId);

      o2ocp->GetRemoteT4100 ()->SetFunction (&LteSlUeRrc::Timer4100Expiry,this);
      o2ocp->GetRemoteT4100 ()->SetArguments (o2ocp->GetContextId ());
      o2ocp->GetRemoteT4100 ()->Schedule ();
      o2ocp->SetState (LteSlO2oCommParams::REMOTE_INIT_SETUP);
    }
}

void
LteSlUeRrc::RecvPc5DataMessage (uint32_t srcL2Id, uint32_t dstL2Id, Ptr<Packet> pdcpSdu)
{
  NS_LOG_FUNCTION (this << srcL2Id << dstL2Id);

  if (dstL2Id == m_sourceL2Id)
    {
      std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it;
      for (it = m_o2oCommContexts.begin (); it != m_o2oCommContexts.end (); it++)
        {
          Ptr<LteSlO2oCommParams> o2ocp = it->second;
          if (o2ocp->GetContextId ().peerL2Id == srcL2Id && (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED || o2ocp->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED))
            {
              NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received PC5 User data from " << srcL2Id);
              UpdateKeepAlive (o2ocp);
            }
        }
    } //else this message is probably for a group ID
}

void
LteSlUeRrc::RecvPc5SignalingMessage (uint32_t srcL2Id, uint32_t dstL2Id, Ptr<Packet> pdcpSdu)
{
  NS_LOG_FUNCTION (this << srcL2Id << dstL2Id);

  if (!IsOneToOnePeer (srcL2Id))
    {
      m_oneToOnePeer.push_back (srcL2Id);
    }

  m_pc5SignalingPacketTrace (srcL2Id, dstL2Id, pdcpSdu);

  LteSlPc5SignallingMessageType lpc5smt;
  pdcpSdu->PeekHeader (lpc5smt);

  switch (lpc5smt.GetMessageType ())
    {
    case LteSlPc5SignallingMessageType::DirectCommunicationRequest:
      ProcessDirectCommunicationRequest (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectCommunicationAccept:
      ProcessDirectCommunicationAccept (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectCommunicationReject:
      ProcessDirectCommunicationReject (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectCommunicationKeepalive:
      ProcessDirectCommunicationKeepalive (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectCommunicationKeepaliveAck:
      ProcessDirectCommunicationKeepaliveAck (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectCommunicationRelease:
      ProcessDirectCommunicationRelease (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectCommunicationReleaseAccept:
      ProcessDirectCommunicationReleaseAccept (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectSecurityModeCommand:
      ProcessDirectSecurityModeCommand (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectSecurityModeComplete:
      ProcessDirectSecurityModeComplete (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectSecurityModeReject:
      ProcessDirectSecurityModeReject (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectRekeyingRequest:
      ProcessDirectRekeyingRequest (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectRekeyingResponse:
      ProcessDirectRekeyingResponse (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::DirectRekeyingTrigger:
      ProcessDirectRekeyingTrigger (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::RemoteUeInfoRequest:
      ProcessRemoteUeInfoRequest (srcL2Id, pdcpSdu);
      break;


    case LteSlPc5SignallingMessageType::RemoteUeInfoResponse:
      ProcessRemoteUeInfoResponse (srcL2Id, pdcpSdu);
      break;

    default:
      break;
    }

  //Condition currently not implemented: Fails direct link authentication / Tx DCRj (#2), Stop T4111
  //Condition currently not implemented: Rx CELL_ID_ANNOUNCEMENT_REQUEST / Start T4107, Tx CELL_ID_ANNOUNCEMENT_RESPONSE
  //Condition currently not implemented: T4107 expires / Stop cell ID announcement in PC5_DISCOVERY message
  //Condition currently not implemented: T4105 expires / Remove TMGI and MBMS SAIs
  //Condition currently not implemented: Rx TMGI_MONITORING_REQUEST / Tx TMGI_MONITORING_RESPONSE, Start T4105

  //Condition currently not implemented: Rx DSMCm (cannot be accepted) / Tx DSMRj (#7, #8 or #9), Stop T4100
}

void
LteSlUeRrc::ProcessDirectCommunicationRequest (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectCommunicationRequest dcrq;
  pdcpSdu->PeekHeader (dcrq);

  if (m_relayServicesMap.find (dcrq.GetRelayServiceCode ()) == m_relayServicesMap.end ())
    {
      //This node is not supporting this relay service code
      NS_LOG_INFO ("Unsupported relay service code " << dcrq.GetRelayServiceCode ());
      return;
    }

  LteSlO2oCommParams::LteSlPc5ContextId cId;
  cId.peerL2Id = L2Id;
  cId.contextId = dcrq.GetSequenceNumber ();
  Ptr<LteSlO2oCommParams> o2ocp = nullptr;
  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = m_o2oCommContexts.find (cId);
  if (it != m_o2oCommContexts.end ())
    {
      o2ocp = it->second;
    }

  if ( o2ocp == nullptr )
    {
      NS_LOG_DEBUG ("Creating new state machine");
      //this is a new request
      o2ocp = CreateObject<LteSlO2oCommParams> ();

      o2ocp->SetState (LteSlO2oCommParams::RELAY_IDLE);
      o2ocp->SetContextId (cId);
      m_o2oCommContexts.insert (std::pair< LteSlO2oCommParams::LteSlPc5ContextId,Ptr<LteSlO2oCommParams> > (cId,o2ocp));
      m_slUeCtrlSapProvider->Pc5ConnectionStarted (L2Id, m_sourceL2Id, LteSlUeRrc::RelayUE);
    }

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received DirectCommunicationRequest from " << L2Id << " context " << dcrq.GetSequenceNumber ());

  NS_ASSERT_MSG (dcrq.GetSequenceNumber () == o2ocp->GetContextId ().contextId, "Message is not a retransmission");

  //Store IMSI
  uint64_t imsi = dcrq.GetImsi ();
  m_l2Id2ImsiMap.insert ( std::pair< uint32_t,uint64_t > (L2Id,imsi) );

  switch (o2ocp->GetState ())
    {
    case LteSlO2oCommParams::RELAY_IDLE:
      break;
    case LteSlO2oCommParams::RELAY_SETUP_REQUEST:
      //We basically have to restart the procedure from the beginning
      o2ocp->ClearTimers ();
      o2ocp->SetState (LteSlO2oCommParams::RELAY_IDLE);
      break;
    case LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED:
      // This can happen if the message is late and the Relay UE has changed of state in the meantime
      NS_LOG_DEBUG ("In State " << o2ocp->GetStateString (o2ocp->GetState ()) << " for " << L2Id << " context " << dcrq.GetSequenceNumber ()
                                << ", ignoring DirectCommunicationRequest");
      return;
    case LteSlO2oCommParams::RELAY_INIT_RELEASE:
      NS_LOG_DEBUG ("In State " << o2ocp->GetStateString (o2ocp->GetState ()) << " for " << L2Id << " context " << dcrq.GetSequenceNumber ()
                                << ", ignoring DirectCommunicationRequest");
      return;
    default:
      NS_LOG_DEBUG ("Invalid state");
      return;
    }

  DirectSecurityModeCommand dsmcm;
  DirectCommunicationReject dcrj;

  //Check if we should accept or reject the request
  //for now, always accepts
  dsmcm.SetSequenceNumber (o2ocp->GetContextId ().contextId);
  o2ocp->SetDsmcmRetrans (dsmcm);

  //Create a sidelink bearer to transmit messages if not already available
  if (GetSidelinkRadioBearer (m_sourceL2Id, L2Id) == NULL)
    {
      m_rrc->ActivateSidelinkRadioBearer (L2Id, true, false);
    }
  else
    {
      o2ocp->SetSecurityModeId (o2ocp->GetSecurityModeId () + 1);
      dsmcm.SetLsb (o2ocp->GetSecurityModeId ());
      Ptr<Packet> p = Create<Packet>();
      p->AddHeader (dsmcm);
      m_rrc->SendPc5Signaling (p, L2Id);
      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent  DirectSecurityModeCommand to " << L2Id << " context " << dsmcm.GetSequenceNumber ());
      o2ocp->SetSecurityModeState (LteSlO2oCommParams::COMMANDED);
      o2ocp->GetRelayT4111 ()->SetFunction (&LteSlUeRrc::Timer4111Expiry,this);
      o2ocp->GetRelayT4111 ()->SetArguments (o2ocp->GetContextId ());
      o2ocp->GetRelayT4111 ()->Schedule ();

      o2ocp->SetState (LteSlO2oCommParams::RELAY_SETUP_REQUEST);
    }
}

void
LteSlUeRrc::ProcessDirectCommunicationAccept (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectCommunicationAccept dca;
  pdcpSdu->PeekHeader (dca);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, dca.GetSequenceNumber ());

  if (it == m_o2oCommContexts.end ())
    {
      // This can happen if the message is late and the Remote UE has erased the context in the meantime
      // e.g., due to Timer4100Expiry and the Remote UE has reached max number of direct communication request retransmissions,
      // or due to the reception of a DirectCommunicationReject due to Timer4111Expiry in the Relay UE side
      NS_LOG_DEBUG ("Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " context " << dca.GetSequenceNumber ());
      return;
    }

  Ptr<LteSlO2oCommParams> o2ocp = it->second;
  UpdateKeepAlive (o2ocp);

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received Message: DirectCommunicationAccept from " << L2Id << " context " << dca.GetSequenceNumber ());
  NS_LOG_DEBUG ("security_mode_state " << o2ocp->GetSecurityModeState ());

  if (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED)
    {
      // This can happen if the message is late and the Relay UE has changed of state in the meantime
      NS_LOG_DEBUG ("In State " << o2ocp->GetStateString (o2ocp->GetState ()) << " for " << L2Id << " context " << dca.GetSequenceNumber ()
                                << ", ignoring DirectCommunicationAccept");
      return;
    }
  if (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_INIT_RELEASE)
    {
      // This should not happen
      NS_LOG_DEBUG ("In State " << o2ocp->GetStateString (o2ocp->GetState ()) << " for " << L2Id << " context " << dca.GetSequenceNumber ()
                                << ", ignoring DirectCommunicationAccept");
      return;
    }

  if (o2ocp->GetSecurityModeState () != LteSlO2oCommParams::COMPLETED)
    {
      NS_LOG_DEBUG ("Direct security mode not completed, ignoring DirectCommunicationAccept");
      return;
    }

  o2ocp->GetRemoteT4100 ()->Remove ();

  o2ocp->GetRemoteT4102 ()->SetFunction (&LteSlUeRrc::Timer4102Expiry,this);
  o2ocp->GetRemoteT4102 ()->SetArguments (o2ocp->GetContextId ());
  o2ocp->GetRemoteT4102 ()->Schedule ();

  o2ocp->SetRemoteKaCount (0);

  o2ocp->SetState (LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED);
  m_slUeCtrlSapProvider->Pc5SecuredEstablished (L2Id, m_sourceL2Id, LteSlUeRrc::RemoteUE);
}

void
LteSlUeRrc::ProcessDirectCommunicationReject (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectCommunicationReject dcrj;
  pdcpSdu->PeekHeader (dcrj);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, dcrj.GetSequenceNumber ());
  if (it == m_o2oCommContexts.end ())
    {
      NS_LOG_DEBUG ("Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << dcrj.GetSequenceNumber ());
    }
  else
    {
      Ptr<LteSlO2oCommParams> o2ocp = it->second;
      UpdateKeepAlive (o2ocp);

      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received DirectCommunicationReject (cause=" << (uint16_t) dcrj.GetPc5SignallingCauseValue () << ") from " << L2Id);

      NS_ASSERT_MSG (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_INIT_SETUP, "Invalid state");
      o2ocp->SetRemoteDcrqCount (0);
      o2ocp->GetRemoteT4100 ()->Remove ();
      //remove context
      o2ocp->ClearTimers ();
      m_o2oCommContexts.erase (it);
      m_slUeCtrlSapProvider->Pc5ConnectionAborted (L2Id, m_sourceL2Id, LteSlUeRrc::RemoteUE, (LteSlO2oCommParams::UeO2ORejectReason)dcrj.GetPc5SignallingCauseValue ());
    }
}

void
LteSlUeRrc::ProcessDirectCommunicationKeepalive (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectCommunicationKeepalive dck;
  pdcpSdu->PeekHeader (dck);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, dck.GetSequenceNumber ());
  if (it == m_o2oCommContexts.end ())
    {
      NS_LOG_DEBUG ("Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << dck.GetSequenceNumber ());
    }
  else
    {
      Ptr<LteSlO2oCommParams> o2ocp = it->second;
      UpdateKeepAlive (o2ocp);

      DirectCommunicationKeepaliveAck dcka;
      dcka.SetSequenceNumber (o2ocp->GetContextId ().contextId);


      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received DirectCommunicationKeepalive from " << L2Id);

      if (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED)
        {
          Ptr<Packet> p = Create<Packet> ();
          p->AddHeader (dcka);
          m_rrc->SendPc5Signaling (p, L2Id);
          NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationKeepaliveAck to " << L2Id);

          o2ocp->GetRelayT4108 ()->Cancel ();
          o2ocp->GetRelayT4108 ()->Schedule ();

          o2ocp->SetState (LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED);
        }
    }
}

void
LteSlUeRrc::ProcessDirectCommunicationKeepaliveAck (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectCommunicationKeepaliveAck dcka;
  pdcpSdu->PeekHeader (dcka);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, dcka.GetSequenceNumber ());
  if (it == m_o2oCommContexts.end ())
    {
      NS_LOG_DEBUG ("Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << dcka.GetSequenceNumber ());
    }
  else
    {
      Ptr<LteSlO2oCommParams> o2ocp = it->second;
      UpdateKeepAlive (o2ocp);

      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << " ] Received DirectCommunicationKeepaliveAck from " << L2Id);

      NS_ASSERT_MSG (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED || o2ocp->GetState () == LteSlO2oCommParams::REMOTE_INIT_RELEASE, "Invalid state");

      if (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED)
        {
          //cancel retransmissions of keep alive messages
          o2ocp->GetRemoteT4101 ()->Cancel ();

          o2ocp->GetRemoteT4102 ()->Cancel ();
          o2ocp->GetRemoteT4102 ()->Schedule ();

          o2ocp->SetRemoteKaCount (o2ocp->GetRemoteKaCount () + 1);
        }
    }
}

void
LteSlUeRrc::ProcessDirectCommunicationRelease (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectCommunicationRelease dcr;
  pdcpSdu->PeekHeader (dcr);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, dcr.GetSequenceNumber ());

  if (it == m_o2oCommContexts.end ())
    {
      //This can happen if the peer UE has retransmitted the release message
      NS_LOG_DEBUG ("Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " context " << dcr.GetSequenceNumber ());
    }
  else
    {
      Ptr<LteSlO2oCommParams> o2ocp = it->second;
      UpdateKeepAlive (o2ocp);

      DirectCommunicationReleaseAccept dcra;
      dcra.SetSequenceNumber (o2ocp->GetContextId ().contextId);

      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received DirectCommunicationRelease from " << L2Id);

      if (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED
          || o2ocp->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED
          || o2ocp->GetState () == LteSlO2oCommParams::RELAY_INIT_RELEASE
          || o2ocp->GetState () == LteSlO2oCommParams::REMOTE_INIT_RELEASE)
        {
          NS_ASSERT_MSG (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED
                         || o2ocp->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED
                         || o2ocp->GetState () == LteSlO2oCommParams::RELAY_INIT_RELEASE
                         || o2ocp->GetState () == LteSlO2oCommParams::REMOTE_INIT_RELEASE, "Invalid state");

          Ptr<Packet> p = Create<Packet> ();
          p->AddHeader (dcra);
          m_rrc->SendPc5Signaling (p, L2Id);
          NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationReleaseAccept to " << L2Id);

          if (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED
              || o2ocp->GetState () == LteSlO2oCommParams::RELAY_INIT_RELEASE)
            {
              m_slUeCtrlSapProvider->Pc5ConnectionTerminated (L2Id, m_sourceL2Id, LteSlUeRrc::RelayUE);
            }
          else
            {
              m_slUeCtrlSapProvider->Pc5ConnectionTerminated (L2Id, m_sourceL2Id, LteSlUeRrc::RemoteUE);
            }

          //delete context
          o2ocp->ClearTimers ();
          m_o2oCommContexts.erase (it);
        }
    }
}

void
LteSlUeRrc::ProcessDirectCommunicationReleaseAccept (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectCommunicationReleaseAccept dcra;
  pdcpSdu->PeekHeader (dcra);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, dcra.GetSequenceNumber ());
  Ptr<LteSlO2oCommParams> o2ocp;
  if (it != m_o2oCommContexts.end ())
    {
      o2ocp = it->second;
    }
  NS_ASSERT_MSG (it == m_o2oCommContexts.end () || o2ocp->GetState () == LteSlO2oCommParams::RELAY_INIT_RELEASE
                 || o2ocp->GetState () == LteSlO2oCommParams::REMOTE_INIT_RELEASE, "Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << dcra.GetSequenceNumber ());

  if (it != m_o2oCommContexts.end ())
    {
      Ptr<LteSlO2oCommParams> o2ocp = it->second;

      NS_LOG_DEBUG ("Received Message: DirectCommunicationReleaseAccept" << " At: " << m_sourceL2Id << " In State: " << o2ocp->GetStateString (o2ocp->GetState ()) << " From: " << L2Id);

      if (o2ocp->GetState () == LteSlO2oCommParams::RELAY_INIT_RELEASE)
        {
          m_slUeCtrlSapProvider->Pc5ConnectionTerminated (L2Id, m_sourceL2Id, LteSlUeRrc::RelayUE);
        }
      else
        {
          m_slUeCtrlSapProvider->Pc5ConnectionTerminated (L2Id, m_sourceL2Id, LteSlUeRrc::RemoteUE);
        }
      o2ocp->ClearTimers ();
      m_o2oCommContexts.erase (it);
    }
}

void
LteSlUeRrc::ProcessDirectSecurityModeCommand (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectSecurityModeCommand dsmcm;
  pdcpSdu->PeekHeader (dsmcm);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, dsmcm.GetSequenceNumber ());
  if (it == m_o2oCommContexts.end ())
    {
      // This can happen if the message is late and the Remote UE has erased the context in the meantime
      // e.g., due to Timer4100Expiry and the Remote UE has reached max number of direct communication request retransmissions,
      // or due to the reception of a DirectCommunicationReject due to Timer4111Expiry in the Relay UE side
      NS_LOG_DEBUG ("Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << dsmcm.GetSequenceNumber ());
      return;
    }

  Ptr<LteSlO2oCommParams> o2ocp = it->second;
  UpdateKeepAlive (o2ocp);

  DirectSecurityModeComplete dsmcp;
  dsmcp.SetSequenceNumber (o2ocp->GetContextId ().contextId);

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received DirectSecurityModeCommand from "
                      << L2Id << " context " << dsmcm.GetSequenceNumber () << " lsb " << (uint16_t) dsmcm.GetLsb ());

  if (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED)
    {
      // This can happen if the message is late and the Relay UE has changed of state in the meantime
      NS_LOG_DEBUG ("In State " << o2ocp->GetStateString (o2ocp->GetState ()) << " for " << L2Id << " context " << dsmcm.GetSequenceNumber ()
                                << ", ignoring DirectSecurityModeCommand");
      return;
    }

  if (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_INIT_SETUP
      && o2ocp->GetSecurityModeState () == LteSlO2oCommParams::EMPTY)
    {
      o2ocp->SetSecurityModeId (dsmcm.GetLsb ());
      dsmcp.SetLsb (o2ocp->GetSecurityModeId ());
      Ptr<Packet> p = Create<Packet>();
      p->AddHeader (dsmcp);
      o2ocp->SetSecurityModeState (LteSlO2oCommParams::COMPLETED);
      m_rrc->SendPc5Signaling (p, L2Id);
      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectSecurityModeComplete to " << L2Id);
    }

}

void
LteSlUeRrc::ProcessDirectSecurityModeComplete (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectSecurityModeComplete dsmcp;
  pdcpSdu->PeekHeader (dsmcp);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, dsmcp.GetSequenceNumber ());

  if (it == m_o2oCommContexts.end ())
    {
      // This can happen if the message is late and the Relay UE has erased the context in the meantime
      // e.g., due to Timer4111Expiry
      NS_LOG_DEBUG ("Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << dsmcp.GetSequenceNumber ());
      return;
    }

  Ptr<LteSlO2oCommParams> o2ocp = it->second;
  UpdateKeepAlive (o2ocp);

  DirectCommunicationAccept dca;
  dca.SetSequenceNumber (o2ocp->GetContextId ().contextId);

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received DirectSecurityModeComplete from "
                      << L2Id << " context " << dsmcp.GetSequenceNumber () << " Lsb " << (uint16_t) dsmcp.GetLsb ());

  if (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED)
    {
      // This can happen if the message is late and the Relay UE has changed of state in the meantime
      NS_LOG_DEBUG ("In State " << o2ocp->GetStateString (o2ocp->GetState ()) << " for " << L2Id << " context " << dsmcp.GetSequenceNumber ()
                                << ", ignoring DirectSecurityModeComplete");
      return;
    }

  if (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SETUP_REQUEST
      && o2ocp->GetSecurityModeState () == LteSlO2oCommParams::COMMANDED
      && o2ocp->GetSecurityModeId () == dsmcp.GetLsb ())
    {
      o2ocp->GetRelayT4111 ()->Remove ();

      Ptr<Packet> p = Create<Packet> ();
      p->AddHeader (dca);
      m_rrc->SendPc5Signaling (p, L2Id);
      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationAccept to " << L2Id);
      o2ocp->SetSecurityModeState (LteSlO2oCommParams::COMPLETED);
      o2ocp->GetRelayT4108 ()->SetFunction (&LteSlUeRrc::Timer4108Expiry,this);
      o2ocp->GetRelayT4108 ()->SetArguments (o2ocp->GetContextId ());
      o2ocp->GetRelayT4108 ()->Schedule ();

      o2ocp->SetState (LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED);

      // Erase all other active contexts the Relay UE has for this Remote L2Id
      // TS 24.334 10.4.2.6.2	Abnormal cases at the target UE
      // "... if the target UE already has an existing link established to the UE known to use this Layer 2 ID
      // and the new request contains an identical User Info as the known user, the UE shall process the new request. However, the target UE shall only delete
      // the existing link context after the new link setup procedure succeeds,
      // or the link keepalive procedure as described in subclause 10.4.3 fails.
      std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it2;
      for (it2 = m_o2oCommContexts.begin (); it2 != m_o2oCommContexts.end (); it2++)
        {
          Ptr<LteSlO2oCommParams> o2ocp2 = it2->second;
          LteSlO2oCommParams::LteSlPc5ContextId cId2 = it2->first;

          if (cId2.peerL2Id == L2Id &&  cId2.contextId != dsmcp.GetSequenceNumber ())
            {
              NS_LOG_DEBUG ("Removing previous context for peerL2Id " << cId2.peerL2Id << " contextId " << cId2.contextId );
              o2ocp2->ClearTimers ();
              m_o2oCommContexts.erase (cId2);
            }
        }

      //Proceed to connect
      m_slUeCtrlSapProvider->Pc5SecuredEstablished (L2Id, m_sourceL2Id, LteSlUeRrc::RelayUE);

      //Report connected Remote UE to the network
      std::map< uint32_t, uint64_t >::iterator it = m_l2Id2ImsiMap.find (L2Id);
      if ( it != m_l2Id2ImsiMap.end () )
        {
          m_slUeCtrlSapProvider->RecvRemoteUeReport (m_rrc->GetImsi (), it->second, L2Id);
        }

      //Start Remote UE information request procedure if enabled
      if (m_isRuirqEnabled)
        {
          RemoteUeInfoRequest ruirq;
          ruirq.SetSequenceNumber (o2ocp->GetContextId ().contextId);

          Ptr<Packet> p = Create<Packet>();
          o2ocp->SetRuirqRetrans (ruirq);
          p->AddHeader (ruirq);
          m_rrc->SendPc5Signaling (p, L2Id);
          NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent RemoteUeInfoRequest to " << L2Id);

          o2ocp->GetRelayTRUIR ()->SetFunction (&LteSlUeRrc::TimerRUIRExpiry,this);
          o2ocp->GetRelayTRUIR ()->SetArguments (o2ocp->GetContextId ());
          o2ocp->GetRelayTRUIR ()->Schedule ();

          o2ocp->SetRelayRuirCount (0);
        }
    }

}

void
LteSlUeRrc::ProcessDirectSecurityModeReject (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectSecurityModeReject dsmrj;
  pdcpSdu->PeekHeader (dsmrj);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, dsmrj.GetSequenceNumber ());
  NS_ASSERT_MSG (it != m_o2oCommContexts.end (), "Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << dsmrj.GetSequenceNumber ());
  Ptr<LteSlO2oCommParams> o2ocp = it->second;

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received DirectSecurityModeReject from " << L2Id);

  if (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SETUP_REQUEST)
    {
      if (dsmrj.GetPc5SignallingCauseValue () == 7 || dsmrj.GetPc5SignallingCauseValue () == 8 || dsmrj.GetPc5SignallingCauseValue () == 9)
        {
          o2ocp->GetRelayT4111 ()->Remove ();

          o2ocp->SetState (LteSlO2oCommParams::RELAY_IDLE);
        }
    }
}

void
LteSlUeRrc::ProcessDirectRekeyingRequest (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectRekeyingRequest drrq;
  pdcpSdu->PeekHeader (drrq);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, drrq.GetSequenceNumber ());
  NS_ASSERT_MSG (it != m_o2oCommContexts.end (), "Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << drrq.GetSequenceNumber ());
  Ptr<LteSlO2oCommParams> o2ocp = it->second;
  UpdateKeepAlive (o2ocp);

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received DirectRekeyingRequest from " << L2Id);
}

void
LteSlUeRrc::ProcessDirectRekeyingResponse (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectRekeyingResponse drrs;
  pdcpSdu->PeekHeader (drrs);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, drrs.GetSequenceNumber ());
  NS_ASSERT_MSG (it != m_o2oCommContexts.end (), "Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << drrs.GetSequenceNumber ());
  Ptr<LteSlO2oCommParams> o2ocp = it->second;
  UpdateKeepAlive (o2ocp);

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received DirectRekeyingResponse from " << L2Id);
}

void
LteSlUeRrc::ProcessDirectRekeyingTrigger (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  DirectRekeyingTrigger drt;
  pdcpSdu->PeekHeader (drt);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, drt.GetSequenceNumber ());
  NS_ASSERT_MSG (it != m_o2oCommContexts.end (), "Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << drt.GetSequenceNumber ());
  Ptr<LteSlO2oCommParams> o2ocp = it->second;
  UpdateKeepAlive (o2ocp);

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received DirectRekeyingTrigger from " << L2Id);
}

void
LteSlUeRrc::ProcessRemoteUeInfoRequest (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  RemoteUeInfoRequest ruirq;
  pdcpSdu->PeekHeader (ruirq);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, ruirq.GetSequenceNumber ());

  if (it == m_o2oCommContexts.end ())
    {
      // This can happen if the message is late and the Remote UE has erased the context in the meantime
      // e.g., due to Timer4100Expiry and the Remote UE has reached max number of direct communication request retransmissions,
      // or due to the reception of a DirectCommunicationReject due to Timer4111Expiry in the Relay UE side
      NS_LOG_DEBUG ("Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << ruirq.GetSequenceNumber ());
      return;
    }
  Ptr<LteSlO2oCommParams> o2ocp = it->second;
  UpdateKeepAlive (o2ocp);

  RemoteUeInfoResponse ruirs;
  ruirs.SetSequenceNumber (o2ocp->GetContextId ().contextId);

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received RemoteUeInfoRequest from " << L2Id);

  if (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED)
    {
      Ptr<Packet> p = Create<Packet> ();
      ruirs.SetImei (m_rrc->GetImsi ());
      p->AddHeader (ruirs);
      m_rrc->SendPc5Signaling (p, L2Id);
      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent RemoteUeInfoResponseto " << L2Id);

      //no change of state
    }
}

void
LteSlUeRrc::ProcessRemoteUeInfoResponse (uint32_t L2Id, Ptr<Packet> pdcpSdu)
{
  RemoteUeInfoResponse ruirs;
  pdcpSdu->PeekHeader (ruirs);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = GetO2OContext (L2Id, ruirs.GetSequenceNumber ());
  if (it == m_o2oCommContexts.end ())
    {
      // This can happen if the message is late and the Relay UE has erased the context in the meantime
      // e.g., due to Timer4111Expiry
      NS_LOG_DEBUG ("Could not find the appropriate One-to-One Communication Context for the Layer-2 ID " << L2Id << " and context " << ruirs.GetSequenceNumber ());
      return;
    }
  Ptr<LteSlO2oCommParams> o2ocp = it->second;
  UpdateKeepAlive (o2ocp);

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Received RemoteUeInfoResponse from " << L2Id);

  if (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED && o2ocp->GetRelayTRUIR ()->IsRunning ())
    {
      uint64_t imei = ruirs.GetImei ();
      NS_LOG_DEBUG ("Received IMEI " << imei << " from L2Id" << L2Id);
      o2ocp->GetRelayTRUIR ()->Remove ();
      //no change of state
    }
}

void
LteSlUeRrc::Timer4100Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId)
{
  NS_LOG_FUNCTION (this << cId.peerL2Id << cId.contextId);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = m_o2oCommContexts.find (cId);
  NS_ASSERT_MSG (it != m_o2oCommContexts.end (), "Could not find the appropriate One-to-One Communication Context for the Layer-2 ID");
  Ptr<LteSlO2oCommParams> o2ocp = it->second;

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Timer4100Expiry");

  NS_ASSERT_MSG (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_INIT_SETUP, "Invalid state");

  if (o2ocp->GetRemoteDcrqCount () < o2ocp->GetRemoteDcrqMax ())
    {
      o2ocp->SetSecurityModeState (LteSlO2oCommParams::EMPTY);
      Ptr<Packet> p = Create<Packet>();
      p->AddHeader (o2ocp->GetDcrqRetrans ());
      m_rrc->SendPc5Signaling (p, cId.peerL2Id);
      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationRequest (Retrans " << (o2ocp->GetRemoteDcrqCount () + 1) << "/" << o2ocp->GetRemoteDcrqMax () << ") to " << cId.peerL2Id);

      o2ocp->SetRemoteDcrqCount (o2ocp->GetRemoteDcrqCount () + 1);

      o2ocp->GetRemoteT4100 ()->Cancel ();
      o2ocp->GetRemoteT4100 ()->Schedule ();

      o2ocp->SetState (LteSlO2oCommParams::REMOTE_INIT_SETUP);
    }
  else
    {
      o2ocp->SetRemoteDcrqCount (0);
      o2ocp->GetRemoteT4100 ()->Remove ();                   //Aborting timer this time, Relay Reselection is also possible

      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Max DirectCommunicationRequest Retransmissions reached to " << cId.peerL2Id);

      //inform controller that connection aborted. We used OTHER_ERRORS even though it was not sent by the relay UE
      m_slUeCtrlSapProvider->Pc5ConnectionAborted (cId.peerL2Id, m_sourceL2Id, LteSlUeRrc::RemoteUE, LteSlO2oCommParams::OTHER_ERRORS);

      //remote context
      o2ocp->ClearTimers ();
      m_o2oCommContexts.erase (cId);
    }

}

void
LteSlUeRrc::Timer4111Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId)
{
  NS_LOG_FUNCTION (this << cId.peerL2Id << cId.contextId);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = m_o2oCommContexts.find (cId);
  NS_ASSERT_MSG (it != m_o2oCommContexts.end (), "Could not find the appropriate One-to-One Communication Context for the Layer-2 ID");
  Ptr<LteSlO2oCommParams> o2ocp = it->second;

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Timer4111Expiry");

  DirectCommunicationReject dcrj;
  dcrj.SetSequenceNumber (o2ocp->GetContextId ().contextId);

  if (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SETUP_REQUEST)
    {
      Ptr<Packet> p = Create<Packet> ();
      dcrj.SetPc5SignallingCauseValue (LteSlO2oCommParams::AUTH_FAILURE);
      p->AddHeader (dcrj);
      m_rrc->SendPc5Signaling (p, cId.peerL2Id);
      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationReject (Reason=Auth Failure) to " << cId.peerL2Id);

      //remote context
      o2ocp->ClearTimers ();
      m_o2oCommContexts.erase (cId);
    }
}

void
LteSlUeRrc::Timer4108Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId)
{
  NS_LOG_FUNCTION (this << cId.peerL2Id << cId.contextId);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = m_o2oCommContexts.find (cId);
  NS_ASSERT_MSG (it != m_o2oCommContexts.end (), "Could not find the appropriate One-to-One Communication Context for the Layer-2 ID");
  Ptr<LteSlO2oCommParams> o2ocp = it->second;

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Timer4108Expiry");

  DirectCommunicationRelease dcr;
  dcr.SetSequenceNumber (o2ocp->GetContextId ().contextId);

  NS_ASSERT (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED);
  Ptr<Packet> p = Create<Packet>();
  dcr.SetReleaseReason (LteSlO2oCommParams::COMM_NO_LONGER_AVAILABLE);
  o2ocp->SetDcrRetrans (dcr);
  p->AddHeader (dcr);
  m_rrc->SendPc5Signaling (p, cId.peerL2Id);
  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationRelease to " << cId.peerL2Id);

  o2ocp->GetRelayT4103 ()->SetFunction (&LteSlUeRrc::Timer4103Expiry,this);
  o2ocp->GetRelayT4103 ()->SetArguments (cId);
  o2ocp->GetRelayT4103 ()->Schedule ();

  o2ocp->SetRelayDcrCount (0);

  o2ocp->SetState (LteSlO2oCommParams::RELAY_INIT_RELEASE);
}

void
LteSlUeRrc::Timer4103Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId)
{
  NS_LOG_FUNCTION (this << cId.peerL2Id << cId.contextId);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = m_o2oCommContexts.find (cId);
  NS_ASSERT_MSG (it != m_o2oCommContexts.end (), "Could not find the appropriate One-to-One Communication Context for the Layer-2 ID");
  Ptr<LteSlO2oCommParams> o2ocp = it->second;

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Timer4103Expiry for peerL2Id " << cId.peerL2Id << " context " << cId.contextId);

  if (o2ocp->GetState () == LteSlO2oCommParams::RELAY_INIT_RELEASE)
    {
      if (o2ocp->GetRelayDcrCount () < o2ocp->GetRelayDcrMax ())
        {
          Ptr<Packet> p = Create<Packet>();
          p->AddHeader (o2ocp->GetDcrRetrans ());
          m_rrc->SendPc5Signaling (p, cId.peerL2Id);
          NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationRelease (" << (o2ocp->GetRelayDcrCount () + 1) << "/" << o2ocp->GetRelayDcrMax () << ") to " << cId.peerL2Id);

          o2ocp->SetRelayDcrCount ( o2ocp->GetRelayDcrCount () + 1 );

          o2ocp->GetRelayT4103 ()->Cancel ();
          o2ocp->GetRelayT4103 ()->Schedule ();

          o2ocp->SetState (LteSlO2oCommParams::RELAY_INIT_RELEASE);
        }
      else
        {
          o2ocp->ClearTimers ();
          m_o2oCommContexts.erase (cId);
          NS_LOG_DEBUG ("RelayDcrMaximum reached for peerL2Id " << cId.peerL2Id << " context " << cId.contextId << ", releasing locally ");
          m_slUeCtrlSapProvider->Pc5ConnectionTerminated (cId.peerL2Id, m_sourceL2Id, LteSlUeRrc::RelayUE);         //Release locally
        }
    }

  if (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_INIT_RELEASE)
    {
      if (o2ocp->GetRemoteDcrCount () < o2ocp->GetRemoteDcrMax ())
        {
          Ptr<Packet> p = Create<Packet>();
          p->AddHeader (o2ocp->GetDcrRetrans ());
          m_rrc->SendPc5Signaling (p, cId.peerL2Id);
          NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationRelease (" << (o2ocp->GetRemoteDcrCount () + 1) << "/" << o2ocp->GetRemoteDcrMax () << ") to " << cId.peerL2Id);

          o2ocp->SetRemoteDcrCount (o2ocp->GetRemoteDcrCount () + 1);

          o2ocp->GetRemoteT4103 ()->Cancel ();
          o2ocp->GetRemoteT4103 ()->Schedule ();

          o2ocp->SetState (LteSlO2oCommParams::REMOTE_INIT_RELEASE);
        }
      else
        {
          o2ocp->ClearTimers ();
          m_o2oCommContexts.erase (cId);
          m_slUeCtrlSapProvider->Pc5ConnectionTerminated (cId.peerL2Id, m_sourceL2Id, LteSlUeRrc::RemoteUE);         //Release locally
        }
    }
}

void
LteSlUeRrc::Timer4102Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId)
{
  NS_LOG_FUNCTION (this << cId.peerL2Id << cId.contextId);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = m_o2oCommContexts.find (cId);
  NS_ASSERT_MSG (it != m_o2oCommContexts.end (), "Could not find the appropriate One-to-One Communication Context for the Layer-2 ID");
  Ptr<LteSlO2oCommParams> o2ocp = it->second;

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Timer4102Expiry");

  DirectCommunicationKeepalive dck;
  dck.SetSequenceNumber (o2ocp->GetContextId ().contextId);

  if (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED)
    {
      Ptr<Packet> p = Create<Packet>();
      o2ocp->SetDckRetrans (dck);
      p->AddHeader (dck);
      m_rrc->SendPc5Signaling (p, cId.peerL2Id);
      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationKeepalive to " << cId.peerL2Id);

      o2ocp->GetRemoteT4101 ()->SetFunction (&LteSlUeRrc::Timer4101Expiry,this);
      o2ocp->GetRemoteT4101 ()->SetArguments (cId);
      o2ocp->GetRemoteT4101 ()->Schedule ();

      o2ocp->SetRemoteDckCount (0);

      //no change in state
    }
}

void
LteSlUeRrc::Timer4101Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId)
{
  NS_LOG_FUNCTION (this << cId.peerL2Id << cId.contextId);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = m_o2oCommContexts.find (cId);
  NS_ASSERT_MSG (it != m_o2oCommContexts.end (), "Could not find the appropriate One-to-One Communication Context for the Layer-2 ID");
  Ptr<LteSlO2oCommParams> o2ocp = it->second;

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Timer4101Expiry");

  DirectCommunicationRelease dcr;
  dcr.SetSequenceNumber (o2ocp->GetContextId ().contextId);

  if (o2ocp->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED)
    {
      if (o2ocp->GetRemoteDckCount () < o2ocp->GetRemoteDckMax ())
        {
          Ptr<Packet> p = Create<Packet>();
          p->AddHeader (o2ocp->GetDckRetrans ());
          m_rrc->SendPc5Signaling (p, cId.peerL2Id);
          NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationKeepalive (" << (o2ocp->GetRemoteDckCount () + 1) << "/" << o2ocp->GetRemoteDckMax () << ") to " << cId.peerL2Id);

          o2ocp->SetRemoteDckCount (o2ocp->GetRemoteDckCount () + 1);
          o2ocp->GetRemoteT4101 ()->Schedule ();

          //no change in state
        }
      else
        {
          Ptr<Packet> p = Create<Packet> ();
          dcr.SetReleaseReason (LteSlO2oCommParams::COMM_NO_LONGER_AVAILABLE);              //Release Reason #3 added to the header
          o2ocp->SetDcrRetrans (dcr);
          p->AddHeader (dcr);
          m_rrc->SendPc5Signaling (p, cId.peerL2Id);
          NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationRelease to " << cId.peerL2Id);

          o2ocp->GetRemoteT4103 ()->SetFunction (&LteSlUeRrc::Timer4103Expiry,this);
          o2ocp->GetRemoteT4103 ()->SetArguments (cId);
          o2ocp->GetRemoteT4103 ()->Schedule ();

          o2ocp->SetRemoteDcrCount (0);

          o2ocp->SetState (LteSlO2oCommParams::REMOTE_INIT_RELEASE);
        }
    }
}

void
LteSlUeRrc::TimerRUIRExpiry (LteSlO2oCommParams::LteSlPc5ContextId cId)
{
  NS_LOG_FUNCTION (this << cId.peerL2Id << cId.contextId);

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it = m_o2oCommContexts.find (cId);
  NS_ASSERT (it != m_o2oCommContexts.end ());
  Ptr<LteSlO2oCommParams> o2ocp = it->second;

  NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] TimerRUIRExpiry");

  if (o2ocp->GetState () == LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED)
    {
      NS_ASSERT_MSG (o2ocp->GetRelayRuirCount () != o2ocp->GetRelayRuirMax (), "Remote UE Info Request count = Remote UE Info Request maximum is currently not handled in the standards");          //See TS 24.334
      if (o2ocp->GetRelayRuirCount () < o2ocp->GetRelayRuirMax ())
        {
          Ptr<Packet> p = Create<Packet>();
          p->AddHeader (o2ocp->GetRuirqRetrans ());
          m_rrc->SendPc5Signaling (p, cId.peerL2Id);
          NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent RemoteUeInfoRequest (" << (o2ocp->GetRelayRuirCount () + 1) << "/" << o2ocp->GetRelayRuirMax () << ") to " << cId.peerL2Id);

          o2ocp->SetRelayRuirCount (o2ocp->GetRelayRuirCount () + 1);

          //no change in state
        }
    }
}

bool
LteSlUeRrc::IsOneToOnePeer (uint32_t proseUeId)
{
  NS_LOG_FUNCTION (this << proseUeId);

  return std::find (m_oneToOnePeer.begin (), m_oneToOnePeer.end (), proseUeId) != m_oneToOnePeer.end ();
}

void
LteSlUeRrc::NotifySidelinkRadioBearerActivated (uint32_t peerUeId)
{
  NS_LOG_FUNCTION (this << peerUeId);

  //check in which state we are
  //if this is a remote UE trying to establish a link to a relay, initiate request
  //Send direct communication request

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it;
  for (it = m_o2oCommContexts.begin (); it != m_o2oCommContexts.end (); it++)
    {
      Ptr<LteSlO2oCommParams> o2ocp = it->second;
      if (o2ocp->GetContextId ().peerL2Id == peerUeId)
        {
          NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] with peer " << o2ocp->GetContextId ().peerL2Id << " context " << o2ocp->GetContextId ().contextId);

          Ptr<Packet> p = Create<Packet> ();
          //DirectSecurityModeCommand dsmcm;
          //DirectCommunicationReject dcrj;

          //check the state to see if we had a message pending
          switch (o2ocp->GetState ())
            {
            case LteSlO2oCommParams::REMOTE_IDLE:
              o2ocp->SetSecurityModeState (LteSlO2oCommParams::EMPTY);
              p->AddHeader (o2ocp->GetDcrqRetrans ());
              m_rrc->SendPc5Signaling (p, peerUeId);
              NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectCommunicationRequest to " << peerUeId);

              o2ocp->GetRemoteT4100 ()->SetFunction (&LteSlUeRrc::Timer4100Expiry,this);
              o2ocp->GetRemoteT4100 ()->SetArguments (o2ocp->GetContextId ());
              o2ocp->GetRemoteT4100 ()->Schedule ();

              o2ocp->SetState (LteSlO2oCommParams::REMOTE_INIT_SETUP);
              break;
            case LteSlO2oCommParams::RELAY_IDLE:
              //send DirectCommunicationReject or DirectSecurityModeCommand depending on checks
              //for now send only DirectSecurityModeCommand
              o2ocp->SetSecurityModeId (o2ocp->GetSecurityModeId () + 1);
              o2ocp->SetDsmcmRetransLsb (o2ocp->GetSecurityModeId ());
              p->AddHeader (o2ocp->GetDsmcmRetrans ());
              m_rrc->SendPc5Signaling (p, peerUeId);
              NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << o2ocp->GetStateString (o2ocp->GetState ()) << "] Sent DirectSecurityModeCommand to " << peerUeId << " context " << o2ocp->GetDsmcmRetrans ().GetSequenceNumber () );
              o2ocp->SetSecurityModeState (LteSlO2oCommParams::COMMANDED);

              o2ocp->GetRelayT4111 ()->SetFunction (&LteSlUeRrc::Timer4111Expiry,this);
              o2ocp->GetRelayT4111 ()->SetArguments (o2ocp->GetContextId ());
              o2ocp->GetRelayT4111 ()->Schedule ();

              o2ocp->SetState (LteSlO2oCommParams::RELAY_SETUP_REQUEST);
              break;
            default:
              break;
            }
        }
    }
}

void
LteSlUeRrc::UpdateKeepAlive (Ptr<LteSlO2oCommParams> context)
{
  NS_LOG_FUNCTION (this);

  if (context->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED)
    {
      if (!context->GetRemoteT4102 ()->IsRunning ())
        {
          //if T4102 is not running, we must be doing a keep alive procedure
          NS_ASSERT_MSG (context->GetRemoteT4101 ()->IsRunning (), "No ongoing keep alive while T4102 has expired");
          context->GetRemoteT4101 ()->Cancel ();
        }
      context->GetRemoteT4102 ()->Cancel ();
      context->GetRemoteT4102 ()->Schedule ();
    }
  else if (context->GetState () == LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED)
    {
      context->GetRelayT4108 ()->Cancel ();
      context->GetRelayT4108 ()->Schedule ();
    }
}

void
LteSlUeRrc::ReleaseO2OConnection (Ptr<LteSlO2oCommParams> context, LteSlO2oCommParams::UeO2OReleaseReason reason)
{
  NS_LOG_FUNCTION (this << reason);

  if (context->GetState () == LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED
      || context->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED)
    {
      //Common procedure
      DirectCommunicationRelease dcr;
      dcr.SetSequenceNumber (context->GetContextId ().contextId);
      Ptr<Packet> p = Create<Packet> ();
      dcr.SetReleaseReason (reason);
      context->SetDcrRetrans (dcr);
      p->AddHeader (dcr);
      m_rrc->SendPc5Signaling (p, context->GetContextId ().peerL2Id);
      NS_LOG_DEBUG ("In " << m_sourceL2Id << " [" << context->GetStateString (context->GetState ()) << "] Sent DirectCommunicationRelease to " << context->GetContextId ().peerL2Id);
    }

  switch (context->GetState ())
    {
    case LteSlO2oCommParams::RELAY_SECURE_ESTABLISHED:
      context->GetRelayT4108 ()->Remove ();

      context->GetRelayT4103 ()->SetFunction (&LteSlUeRrc::Timer4103Expiry,this);
      context->GetRelayT4103 ()->SetArguments (context->GetContextId ());
      context->GetRelayT4103 ()->Schedule ();
      context->SetRelayDcrCount (0);

      context->SetState (LteSlO2oCommParams::RELAY_INIT_RELEASE);
      break;
    case LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED:
      context->GetRemoteT4101 ()->Remove ();
      context->GetRemoteT4102 ()->Remove ();

      context->GetRemoteT4103 ()->SetFunction (&LteSlUeRrc::Timer4103Expiry,this);
      context->GetRemoteT4103 ()->SetArguments (context->GetContextId ());
      context->GetRemoteT4103 ()->Schedule ();
      context->SetRemoteDcrCount (0);

      context->SetState ( LteSlO2oCommParams::REMOTE_INIT_RELEASE);
      break;
    case LteSlO2oCommParams::RELAY_INIT_RELEASE:
    case LteSlO2oCommParams::REMOTE_INIT_RELEASE:
      //already initiated the release
      break;
    default:
      //simply delete the context
      //TODO: release timers based on state
      NS_LOG_DEBUG ("Deleting context");
      context->ClearTimers ();
      m_o2oCommContexts.erase (context->GetContextId ());
    }
}

std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator
LteSlUeRrc::GetO2OContext (uint32_t peerUeId, uint32_t contextId)
{
  LteSlO2oCommParams::LteSlPc5ContextId cId;
  cId.peerL2Id = peerUeId;
  cId.contextId = contextId;

  return m_o2oCommContexts.find (cId);
}

int64_t
LteSlUeRrc::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_rand->SetStream (stream);
  return 1;
}


void
LteSlUeRrc::RelayUeSelection (std::map < std::pair<uint64_t,uint32_t>, double> validRelays)
{
  NS_LOG_FUNCTION (this << m_sourceL2Id);

  // Call the controller Relay UE (re)selection algorithm for each relay service code
  // the UE is interested on.
  // TODO: Currently the code only support one service per UE, but we let this portion
  // generic for future extensions.
  std::map<uint32_t, RelayServiceInfo>::iterator itInfo;
  for (itInfo = m_relayServicesMap.begin (); itInfo != m_relayServicesMap.end (); ++itInfo)
    {
      if (itInfo->second.role == RemoteUE)
        {
          uint32_t serviceCode = itInfo->second.serviceCode;

          NS_LOG_LOGIC (this << " Remote UE L2ID " << m_sourceL2Id
                             << " checking if it should (re)select Relay UE for Service Code " << serviceCode);
          // Check if the UE is connected to a Relay UE (has currently a selected sidelink Relay UE)
          // This code assumes that the Remote UE can be connected only to one Relay UE
          // per service code
          uint64_t currentRelayId = 0;
          Ptr<LteSlO2oCommParams> currentRelayIdContext;
          if (m_o2oCommContexts.size () > 0)
            {
              std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator it;
              for (it = m_o2oCommContexts.begin (); it != m_o2oCommContexts.end (); ++it)
                {
                  if (it->second->GetDcrqRetrans ().GetRelayServiceCode () == serviceCode)
                    {
                      if (it->second->GetState () == LteSlO2oCommParams::REMOTE_SECURE_ESTABLISHED)
                        {
                          currentRelayId = it->first.peerL2Id;
                          currentRelayIdContext = it->second;
                          NS_LOG_LOGIC (this << " UE L2ID " << m_sourceL2Id
                                             << " Connected to Relay UE ID " << currentRelayId
                                             << " for Service Code " << serviceCode);
                          break;
                        }
                      else if (it->second->GetState () == LteSlO2oCommParams::REMOTE_INIT_RELEASE)
                        {
                          NS_LOG_LOGIC (this << " UE L2ID " << m_sourceL2Id
                                             << " is in a direct communication release procedure with Relay UE ID " << it->first.peerL2Id
                                             << " for Service Code " << serviceCode << "."
                                             << " Skipping Selection... ");
                          return;
                        }
                    }
                }
            }
          else
            {
              NS_LOG_LOGIC (this << " UE L2ID " << m_sourceL2Id << " does not have currently any connection");
            }

          //Call Relay Selection algorithm in the controller
          //- parameter: the list of valid Relay UEs offering this service code
          //             and their (l3) SD-RSRP
          //             the serviceCode
          //             the currentRelayId
          //- return: the Relay ID of the selected Relay,
          //	      (Relay ID = Zero if not suitable relay available)
          uint64_t selectedRelayId = 0;
          std::map <uint64_t, double> validRelaysForSc;
          std::map <std::pair<uint64_t,uint32_t>, double>::iterator vaReIt;
          for (vaReIt = validRelays.begin (); vaReIt != validRelays.end (); vaReIt++)
            {
              if (vaReIt->first.second == serviceCode)
                {
                  validRelaysForSc.insert (std::pair<uint64_t, double> (vaReIt->first.first, vaReIt->second ));
                }
            }

          selectedRelayId = m_slUeCtrlSapProvider->RelayUeSelection (validRelaysForSc, serviceCode, currentRelayId);

          if (selectedRelayId != 0)
            {
              NS_LOG_LOGIC (this << " Remote UE L2ID " << m_sourceL2Id
                                 << " selected Relay UE ID: " << selectedRelayId
                                 << " for Service Code " << serviceCode );
              if (currentRelayId != 0)
                {
                  // Check if the Remote UE should stay connected to the current Relay UE
                  NS_LOG_LOGIC (this << " Currently connected to Relay UE ID: " << currentRelayId
                                     << " for Service Code " << serviceCode );
                  if (currentRelayId == selectedRelayId)
                    {
                      //the Remote UE keeps connected to the current Relay UE for this service code
                      NS_LOG_LOGIC (this << " Keeping connection" );
                      continue;
                    }
                  else
                    {
                      //The Remote UE should disconnect from the current Relay UE before
                      //connecting to the newly selected Relay UE
                      NS_LOG_LOGIC (this << " Disconnecting... ");
                      ReleaseO2OConnection (currentRelayIdContext, LteSlO2oCommParams::COMM_NO_LONGER_NEEDED);
                    }
                }

              // Now proceed to start connection procedure with the selected Relay UE
              NS_LOG_LOGIC (this << " Starting connection with Relay UE ID: " << selectedRelayId
                                 << " for Service Code " << serviceCode);
              StartRelayDirectCommunication (serviceCode, selectedRelayId);
            }
          else
            {
              NS_LOG_LOGIC (this << " Remote UE L2ID " << m_sourceL2Id
                                 << " did not find a suitable Relay UE to select"
                                 << " for Service Code " << serviceCode);
              if (currentRelayId != 0)
                {
                  NS_LOG_LOGIC (this << " Currently connected to Relay UE ID: " << currentRelayId
                                     << "for Service Code " << serviceCode );
                  NS_LOG_LOGIC (this << " Disconnecting... ");
                  ReleaseO2OConnection (currentRelayIdContext, LteSlO2oCommParams::COMM_NO_LONGER_NEEDED);
                }
            }
        } //if (itInfo->second.role == RemoteUE)
    } // for (itInfo = m_relayServicesMap.begin (); ...
}

void
LteSlUeRrc::SetLteSlUeCtrlSapProvider (LteSlUeCtrlSapProvider* s)
{
  NS_LOG_FUNCTION (this << s);
  m_slUeCtrlSapProvider = s;
}

LteSlUeCtrlSapUser*
LteSlUeRrc::GetLteSlUeCtrlSapUser ()
{
  return m_slUeCtrlSapUser;
}

} // namespace ns3
