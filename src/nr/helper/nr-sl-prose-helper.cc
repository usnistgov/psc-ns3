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

#include "nr-sl-prose-helper.h"

#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-sl-ue-rrc.h>
#include <ns3/lte-ue-rrc.h>
#include <ns3/nr-sl-ue-service.h>
#include <ns3/nr-sl-ue-prose.h>
#include <ns3/epc-ue-nas.h>

#include <ns3/fatal-error.h>
#include <ns3/log.h>

#include <ns3/pointer.h>
#include <ns3/simulator.h>
#include <ns3/nr-point-to-point-epc-helper.h>
#include <ns3/config.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrSlProseHelper");

NS_OBJECT_ENSURE_REGISTERED (NrSlProseHelper);

NrSlProseHelper::NrSlProseHelper (void)

{
  NS_LOG_FUNCTION (this);
  m_discoveryTrace = CreateObject<NrSlDiscoveryTrace> ();
}

NrSlProseHelper::~NrSlProseHelper (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
NrSlProseHelper::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::NrSlProseHelper")
    .SetParent<Object> ()
    .SetGroupName ("nr")
    .AddConstructor<NrSlProseHelper> ();
  return tid;
}

void
NrSlProseHelper::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Object::DoDispose ();
}

void
NrSlProseHelper::SetEpcHelper (const Ptr<NrPointToPointEpcHelper> &epcHelper)
{
  NS_LOG_FUNCTION (this);
  m_epcHelper = epcHelper;

}

void
NrSlProseHelper::PrepareUesForProse (NetDeviceContainer c)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<NetDevice> netDev = *i;
      Ptr<NrUeNetDevice> nrUeDev = netDev->GetObject <NrUeNetDevice>();
      PrepareSingleUeForProse (nrUeDev);
    }
}

void
NrSlProseHelper::PrepareSingleUeForProse (Ptr<NrUeNetDevice> nrUeDev)
{
  NS_LOG_FUNCTION (this);

  //Create ProSe layer
  Ptr<NrSlUeProse> nrSlUeProse = CreateObject<NrSlUeProse> ();

  //Connect ProSe layer SAPs
  Ptr<LteUeRrc> lteUeRrc = nrUeDev->GetRrc ();
  nrSlUeProse->SetNrSlUeSvcRrcSapProvider (lteUeRrc->GetNrSlUeSvcRrcSapProvider ());
  lteUeRrc->SetNrSlUeSvcRrcSapUser (nrSlUeProse->GetNrSlUeSvcRrcSapUser ());

  Ptr<EpcUeNas> epcUeNas = nrUeDev->GetNas ();
  nrSlUeProse->SetNrSlUeSvcNasSapProvider (epcUeNas->GetNrSlUeSvcNasSapProvider ());
  epcUeNas->SetNrSlUeSvcNasSapUser (nrSlUeProse->GetNrSlUeSvcNasSapUser ());

  //Keep the ProSe layer accessible in the net device
  nrUeDev->SetAttribute ("NrSlService", PointerValue (nrSlUeProse));

}

void
NrSlProseHelper::PrepareUesForUnicast (NetDeviceContainer c)
{
  NS_LOG_FUNCTION (this);
  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<NetDevice> netDev = *i;
      Ptr<NrUeNetDevice> nrUeDev = netDev->GetObject <NrUeNetDevice>();
      PrepareSingleUeForUnicast (nrUeDev);
    }
}

void
NrSlProseHelper::PrepareSingleUeForUnicast (Ptr<NrUeNetDevice> nrUeDev)
{
  NS_LOG_FUNCTION (this);

  Ptr<NrSlUeProse> nrSlUeProse = nrUeDev->GetSlUeService ()->GetObject <NrSlUeProse> ();
  nrSlUeProse->ConfigureUnicast ();
}

void
NrSlProseHelper::EstablishRealDirectLink (Time time,
		                                  Ptr<NetDevice> initUe, Ipv4Address initUeIp, struct SidelinkInfo& initSlInfo,
										  Ptr<NetDevice> trgtUe, Ipv4Address trgtUeIp, struct SidelinkInfo& trgtSlInfo)
{
  NS_LOG_FUNCTION (this);
  Ptr<NrUeNetDevice> initUeNetDev = initUe->GetObject <NrUeNetDevice>();
  Ptr<NrUeNetDevice> trgtUeNetDev = trgtUe->GetObject <NrUeNetDevice>();
  Ptr<NrSlUeProse> initUeProse = initUeNetDev->GetSlUeService ()->GetObject <NrSlUeProse> ();
  Ptr<NrSlUeProse> trgtUeProse = trgtUeNetDev->GetSlUeService ()->GetObject <NrSlUeProse> ();
  Ptr<LteUeRrc> initUeRrc = initUeNetDev->GetRrc ();
  Ptr<LteUeRrc> trgtUeRrc = trgtUeNetDev->GetRrc ();

  initUeProse->SetImsi (initUeRrc->GetImsi ());
  trgtUeProse->SetImsi (trgtUeRrc->GetImsi ());

  uint32_t initUeL2Id = initUeRrc->GetSourceL2Id ();
  uint32_t trgtUeL2Id = trgtUeRrc->GetSourceL2Id ();

  initUeProse->SetL2Id (initUeL2Id);
  trgtUeProse->SetL2Id (trgtUeL2Id);

  NS_LOG_INFO ("initUeL2Id " << initUeL2Id << " trgtUeL2Id " << trgtUeL2Id);

  initSlInfo.m_dstL2Id = trgtUeL2Id;
  initSlInfo.m_srcL2Id = initUeL2Id;
  trgtSlInfo.m_dstL2Id = initUeL2Id;
  trgtSlInfo.m_srcL2Id = trgtUeL2Id;

  //Initiating UE
  Simulator::Schedule (time, &NrSlUeProse::AddDirectLinkConnection, initUeProse, initUeL2Id, initUeIp,
		               trgtUeL2Id, true, 0, initSlInfo);

  //Target UE
  Simulator::Schedule (time, &NrSlUeProse::AddDirectLinkConnection, trgtUeProse, trgtUeL2Id, trgtUeIp,
		               initUeL2Id, false, 0, trgtSlInfo);

}

void
NrSlProseHelper::EstablishL3UeToNetworkRelayConnection (Time t,
                                                        Ptr<NetDevice> remoteUe, Ipv4Address remoteUeIp, struct SidelinkInfo& remoteUeSlInfo,
                                                        Ptr<NetDevice> relayUe, Ipv4Address relayUeIp, struct SidelinkInfo& relayUeSlInfo,
                                                        uint32_t relayServiceCode)
{
  NS_LOG_FUNCTION (this);

  if (relayServiceCode <= 0)
  {
	  NS_FATAL_ERROR ("Please provide a relay service code greater than zero for U2N relay connection.");
  }
  Ptr<NrUeNetDevice> remoteUeNetDev = remoteUe->GetObject <NrUeNetDevice>();
  Ptr<NrUeNetDevice> relayUeNetDev = relayUe->GetObject <NrUeNetDevice>();
  Ptr<NrSlUeProse> remoteUeProse = remoteUeNetDev->GetSlUeService ()->GetObject <NrSlUeProse> ();
  Ptr<NrSlUeProse> relayUeProse = relayUeNetDev->GetSlUeService ()->GetObject <NrSlUeProse> ();
  Ptr<LteUeRrc> remoteUeRrc = remoteUeNetDev->GetRrc ();
  Ptr<LteUeRrc> relayUeRrc = relayUeNetDev->GetRrc ();

  remoteUeProse->SetImsi (remoteUeRrc->GetImsi ());
  relayUeProse->SetImsi (relayUeRrc->GetImsi ());

  uint32_t remoteUeL2Id = remoteUeRrc->GetSourceL2Id ();
  uint32_t relayUeL2Id = relayUeRrc->GetSourceL2Id ();

  remoteUeProse->SetL2Id (remoteUeL2Id);
  relayUeProse->SetL2Id (relayUeL2Id);

  NS_LOG_DEBUG ("remote UE L2Id: " << remoteUeL2Id << " relay UE L2Id: " << relayUeL2Id);

  remoteUeSlInfo.m_dstL2Id = relayUeL2Id;
  remoteUeSlInfo.m_srcL2Id = remoteUeL2Id;
  relayUeSlInfo.m_dstL2Id = remoteUeL2Id;
  relayUeSlInfo.m_srcL2Id = relayUeL2Id;

  //Remote UE (Initiating UE)
  Simulator::Schedule (t, &NrSlUeProse::AddDirectLinkConnection,
                       remoteUeProse, remoteUeL2Id, remoteUeIp, relayUeL2Id,
                       true, relayServiceCode, remoteUeSlInfo);

  //Relay UE (Target UE)
  Simulator::Schedule (t, &NrSlUeProse::AddDirectLinkConnection,
                       relayUeProse, relayUeL2Id, relayUeIp, remoteUeL2Id,
                       false, relayServiceCode, relayUeSlInfo);

}

void
NrSlProseHelper::ConfigureL3UeToNetworkRelay (NetDeviceContainer relayUeDevices,
                                              std::set<uint32_t> relayServiceCodes,
                                              EpsBearer bearer, Ptr<EpcTft> tft)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (m_epcHelper != 0, "dedicated EPS bearers cannot be set up when the EPC is not used");

  for (NetDeviceContainer::Iterator devIt = relayUeDevices.Begin (); devIt != relayUeDevices.End (); ++devIt)
    {
      uint64_t imsi = (*devIt)->GetObject<NrUeNetDevice> ()->GetImsi ();
      Ptr<NrSlUeProse> prose = (*devIt)->GetObject<NrUeNetDevice> ()->GetSlUeService ()->GetObject <NrSlUeProse> ();

      //Set the relay service codes of the services the relay UE provides and the associated configuration
      for ( auto it = relayServiceCodes.begin (); it != relayServiceCodes.end (); ++it )
        {
          //Activate Eps dedicated bearer for relaying
          uint8_t relayDrbId = m_epcHelper->ActivateEpsBearer (*devIt, imsi, tft, bearer);
          NrSlUeProse::NrSlL3U2nServiceConfiguration config;
          config.relayDrbId = relayDrbId;
          prose->AddL3U2nRelayServiceConfiguration (*it, config);
        }
      //Set EPC Helper pointer on the ProSe layer, which is used to configure
      //data path in the EpcPgwApplication when a remote UE successfully connects to this relay UE)
      prose->SetEpcHelper (m_epcHelper);
    }
}

void
NrSlProseHelper::StartDiscoveryApp (Ptr<NetDevice> ueDevice, uint32_t appCode, uint32_t dstL2Id, NrSlUeProse::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);

  Ptr<NrSlUeProse> ueProse = ueDevice->GetObject <NrUeNetDevice>()->GetSlUeService ()->GetObject <NrSlUeProse> ();
  ueProse->SetL2Id (ueDevice->GetObject <NrUeNetDevice>()->GetRrc ()->GetSourceL2Id ());
  ueProse->AddDiscoveryApp (appCode, dstL2Id, role);
}

void
NrSlProseHelper::StopDiscoveryApp (Ptr<NetDevice> ueDevice, uint32_t appCode, NrSlUeProse::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);

  Ptr<NrSlUeProse> ueProse = ueDevice->GetObject <NrUeNetDevice>()->GetSlUeService ()->GetObject <NrSlUeProse> ();
  ueProse->RemoveDiscoveryApp (appCode, role);
}

void
NrSlProseHelper::StartDiscovery (Ptr<NetDevice> ueDevice, std::list<uint32_t> appCodes, std::list<uint32_t> dstL2Ids, NrSlUeProse::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);  

  std::list<uint32_t>::iterator dst = dstL2Ids.begin ();
  for (std::list<uint32_t>::iterator it = appCodes.begin (); it != appCodes.end (); ++it)
  {
    StartDiscoveryApp (ueDevice, *it, *dst, role);
    ++dst;
  }
}

void
NrSlProseHelper::StopDiscovery (Ptr<NetDevice> ueDevice, std::list<uint32_t> appCodes, NrSlUeProse::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);

  std::map <uint32_t, NrSlUeProse::DiscoveryInfo>::iterator itInfo;
  for (std::list<uint32_t>::iterator it = appCodes.begin (); it != appCodes.end (); ++it)
  {
    StopDiscoveryApp (ueDevice, *it, role);
  }
}

void
NrSlProseHelper::StartRelayDiscovery (Ptr<NetDevice> ueDevice, uint32_t relayCode, uint32_t dstL2Id, NrSlUeProse::DiscoveryModel model, NrSlUeProse::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);
  Ptr<NrSlUeProse> ueProse = ueDevice->GetObject <NrUeNetDevice>()->GetSlUeService ()->GetObject <NrSlUeProse> ();
  ueDevice->GetObject <NrUeNetDevice>()->GetSlUeService ()->GetObject <NrSlUeProse> ()->SetL2Id (ueDevice->GetObject <NrUeNetDevice>()->GetRrc ()->GetSourceL2Id ());
  ueProse->AddRelayDiscovery (relayCode, dstL2Id, model, role);
}

void
NrSlProseHelper::StopRelayDiscovery (Ptr<NetDevice> ueDevice, uint32_t relayCode, NrSlUeProse::DiscoveryRole role)
{
  NS_LOG_FUNCTION (this);
  Ptr<NrSlUeProse> ueProse = ueDevice->GetObject <NrUeNetDevice>()->GetSlUeService ()->GetObject <NrSlUeProse> ();
  ueProse->RemoveRelayDiscovery (relayCode, role);
}

void
NrSlProseHelper::EnableDiscoveryTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/NrSlService/$ns3::NrSlUeProse/DiscoveryTrace", MakeBoundCallback (&NrSlDiscoveryTrace::DiscoveryTraceCallback, m_discoveryTrace));
}

} // namespace ns3

