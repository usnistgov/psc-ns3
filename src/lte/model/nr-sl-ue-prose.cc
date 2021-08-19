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


#include "nr-sl-ue-prose.h"

#include <ns3/lte-ue-rrc.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/lte-radio-bearer-info.h>

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/pointer.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/simulator.h>

#include <algorithm>

#include <ns3/nr-sl-pc5-signalling-header.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrSlUeProse");
NS_OBJECT_ENSURE_REGISTERED (NrSlUeProse);


NrSlUeProseDirLinkContext::NrSlUeProseDirLinkContext (void)
{
  NS_LOG_FUNCTION (this);
  m_hasActiveSlDrb = false;
  m_hasPendingSlDrb = false;
}

NrSlUeProseDirLinkContext::~NrSlUeProseDirLinkContext (void)
{}

TypeId
NrSlUeProseDirLinkContext::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::NrSlUeProseDirLinkContext")
    .SetParent<Object> ()
    .AddConstructor<NrSlUeProseDirLinkContext> ()
  ;
  return tid;
}

TypeId NrSlUeProse::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::NrSlUeProse")
    .SetParent<NrSlUeService> ()
    .AddConstructor<NrSlUeProse> ()
  ;
  return tid;
}

NrSlUeProse::NrSlUeProse ()
{
  NS_LOG_FUNCTION (this);
  m_nrSlUeSvcRrcSapUser = new MemberNrSlUeSvcRrcSapUser<NrSlUeProse> (this);
  m_nrSlUeSvcNasSapUser = new MemberNrSlUeSvcNasSapUser<NrSlUeProse> (this);
  m_nrSlUeProseDirLnkSapUser = new MemberNrSlUeProseDirLnkSapUser<NrSlUeProse> (this);
}


NrSlUeProse::~NrSlUeProse (void)
{
  NS_LOG_FUNCTION (this);
}

void
NrSlUeProse::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_nrSlUeSvcRrcSapUser;
  delete m_nrSlUeSvcNasSapUser;
  delete m_nrSlUeProseDirLnkSapUser;
}

NrSlUeSvcRrcSapUser*
NrSlUeProse::GetNrSlUeSvcRrcSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_nrSlUeSvcRrcSapUser;
}

void
NrSlUeProse::SetNrSlUeSvcRrcSapProvider (NrSlUeSvcRrcSapProvider* s)
{
  NS_LOG_FUNCTION (this);
  m_nrSlUeSvcRrcSapProvider = s;
}


NrSlUeSvcNasSapUser*
NrSlUeProse::GetNrSlUeSvcNasSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_nrSlUeSvcNasSapUser;
}

void
NrSlUeProse::SetNrSlUeSvcNasSapProvider (NrSlUeSvcNasSapProvider* s)
{
  NS_LOG_FUNCTION (this);
  m_nrSlUeSvcNasSapProvider = s;
}


void
NrSlUeProse::DoNotifySvcNrSlDataRadioBearerActivated (uint32_t peerL2Id)
{
  NS_LOG_FUNCTION (this << peerL2Id);

  //Get link
  auto it = m_unicastDirectLinks.find (peerL2Id);
  if (it == m_unicastDirectLinks.end ())
    {
      //TODO: Can this happen and should be handled? e.g., Release link before bearer setup completion?
      //      To verify when release procedure is implemented
      NS_FATAL_ERROR ("Could not find the direct link");
    }
  else
    {
      it->second->m_hasPendingSlDrb = false;
      it->second->m_hasActiveSlDrb = true;
    }
}


void NrSlUeProse::ConfigureUnicast ()
{
  NS_LOG_FUNCTION (this);

  //Tell the RRC to inform the MAC to monitor the UE's own L2Id
  m_nrSlUeSvcRrcSapProvider->MonitorSelfL2Id ();

}


void
NrSlUeProse::AddDirectLinkConnection (uint32_t selfL2Id, Ipv4Address selfIp, uint32_t peerL2Id, bool isInitiating)
{
  NS_LOG_FUNCTION (this << selfL2Id << selfIp << peerL2Id << isInitiating);

  bool ideal = false;

  //TODO: Verifications
  //Is it possible to create this link?
  //No. Errors. Return.

  //Yes. Continue

  //Create context to maintain here in the ProSe layer
  Ptr<NrSlUeProseDirLinkContext> context = CreateObject <NrSlUeProseDirLinkContext> ();

  //Create Direct Link instance
  Ptr<NrSlUeProseDirectLink> link = CreateObject <NrSlUeProseDirectLink> (selfL2Id, peerL2Id, isInitiating, ideal, selfIp);

  //Connect SAPs
  link->SetNrSlUeProseDirLnkSapUser (GetNrSlUeProseDirLnkSapUser ());

  context->m_link = link;
  context->m_nrSlUeProseDirLnkSapProvider = link->GetNrSlUeProseDirLnkSapProvider ();
  context->m_ipInfo.selfIpv4Addr = selfIp;

  //Store context
  m_unicastDirectLinks.insert (std::pair <uint32_t, Ptr<NrSlUeProseDirLinkContext> > (peerL2Id, context));


  //Initiate connection establishment procedure if this UE is the initiating UE
  if (isInitiating)
    {
      context->m_link->StartConnectionEstablishment ();
    }

}


NrSlUeProseDirLnkSapUser*
NrSlUeProse::GetNrSlUeProseDirLnkSapUser ()
{
  NS_LOG_FUNCTION (this);
  return m_nrSlUeProseDirLnkSapUser;
}


void
NrSlUeProse::DoSendNrSlPc5SMessage (Ptr<Packet> packet, uint32_t dstL2Id,  uint8_t lcId)
{
  NS_LOG_FUNCTION (this);

  //Activate the corresponding SL-SRB for the logical channel, if not active
  auto it = m_activeSlSrbs.find (dstL2Id);
  if (it == m_activeSlSrbs.end ()) //First SL-SRB for this destination
    {
      //Instruct the RRC to activate the SL-SRB
      m_nrSlUeSvcRrcSapProvider->ActivateNrSlSignallingRadioBearer (dstL2Id, lcId);

      //Keep track of it
      std::bitset<4> activeLcs;
      activeLcs[lcId] = true;
      m_activeSlSrbs.insert (std::pair<uint32_t, std::bitset<4> > (dstL2Id, activeLcs));
    }
  else
    {
      if (it->second [lcId] == false) //First SL-SRB for this lcId
        {
          //Instruct the RRC to activate the SL-SRB
          m_nrSlUeSvcRrcSapProvider->ActivateNrSlSignallingRadioBearer (dstL2Id, lcId);
          //Keep track of it
          it->second [lcId] = true;
        }
    }

  //Pass the message to the RRC
  m_nrSlUeSvcRrcSapProvider->SendNrSlSignalling (packet, dstL2Id, lcId);

}

void
NrSlUeProse::DoReceiveNrSlSignalling (Ptr<Packet> packet, uint32_t srcL2Id)
{
  NS_LOG_FUNCTION (this);

  //This can be PC5-S or a discovery message
  //TODO: How to differentiate them here?

  //If PC5-S for unicast communication:
  auto it = m_unicastDirectLinks.find (srcL2Id);
  if (it == m_unicastDirectLinks.end ())
    {
      NS_LOG_INFO ("Unrecognized peer L2 ID...");
      NS_FATAL_ERROR ("This case is not handled yet. You should configure both UEs in the "
                      "link from the scenario.");
      //This will be the case when the Relay UE receives a request from the Remote UE.
      //The Relay UE should:
      //1. Create the corresponding context and link.
      //    TODO: provision selfL2Id,  selfIp in the class
      //    AddDirectLinkConnection (selfL2Id, selfIp, srcL2Id, false);
      //2. Pass the packet to the corresponding direct link instance
    }
  else
    {
      NS_LOG_INFO ("Context found!");
      //Pass the packet to the corresponding direct link instance
      it->second->m_nrSlUeProseDirLnkSapProvider->ReceiveNrSlPc5Message (packet);
    }
}


void
NrSlUeProse::DoNotifyChangeOfDirectLinkState (uint32_t peerL2Id, NrSlUeProseDirLnkSapUser::ChangeOfStateNotification info)
{
  NS_LOG_FUNCTION (this << peerL2Id << info.newStateStr << info.newStateEnum);

  //Get link
  auto it = m_unicastDirectLinks.find (peerL2Id);
  if (it == m_unicastDirectLinks.end ())
    {
      NS_FATAL_ERROR ("Could not find the direct link");
    }

  //Perform action depending on state
  switch (info.newStateEnum)
    {
      case NrSlUeProseDirectLink::INIT:
        NS_LOG_INFO ("INIT");
        break;
      case NrSlUeProseDirectLink::ESTABLISHING:
        NS_LOG_INFO ("ESTABLISHING");
        break;
      case NrSlUeProseDirectLink::ESTABLISHED:
        NS_LOG_INFO ("ESTABLISHED");

        if (!it->second->m_hasActiveSlDrb && !it->second->m_hasPendingSlDrb)
          {
            NS_LOG_INFO ("Instructing activation of SL-DRB with peer Ipv4 address " << info.ipInfo.peerIpv4Addr);
            ActivateDirectLinkDataRadioBearer (peerL2Id, info.ipInfo);
          }

        break;
      case NrSlUeProseDirectLink::RELEASING:
        NS_LOG_INFO ("RELEASING");
        break;
      case NrSlUeProseDirectLink::RELEASED:
        NS_LOG_INFO ("RELEASED");
        break;
      default:
        NS_FATAL_ERROR ("Invalid state " << info.newStateStr);
    }


}

void
NrSlUeProse::ActivateDirectLinkDataRadioBearer (uint32_t peerL2Id, NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo)
{
  NS_LOG_FUNCTION (this << peerL2Id << ipInfo.peerIpv4Addr);

  //Get link
  auto it = m_unicastDirectLinks.find (peerL2Id);
  if (it == m_unicastDirectLinks.end ())
    {
      NS_FATAL_ERROR ("Could not find the direct link");
    }
  else
    {
      //Create unicast TFT to be able to transmit to peer UE
      Ptr<LteSlTft> tft;
      tft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT,
                              LteSlTft::CommType::Unicast,
                              ipInfo.peerIpv4Addr, peerL2Id);
      m_nrSlUeSvcNasSapProvider->ActivateSvcNrSlDataRadioBearer (tft);
      it->second->m_hasPendingSlDrb = true;
    }
}



} // namespace ns3
