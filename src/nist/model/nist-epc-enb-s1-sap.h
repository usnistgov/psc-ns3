/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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

#ifndef NIST_EPC_ENB_S1_SAP_H
#define NIST_EPC_ENB_S1_SAP_H

#include <list>
#include <stdint.h>
#include <ns3/nist-eps-bearer.h>
#include <ns3/ipv4-address.h>

namespace ns3 {


/**
 * This class implements the Service Access Point (SAP) between the
 * NistLteEnbRrc and the NistEpcEnbApplication. In particular, this class implements the
 * Provider part of the SAP, i.e., the methods exported by the
 * NistEpcEnbApplication and called by the NistLteEnbRrc.
 * 
 */
class NistEpcEnbS1SapProvider
{
public:
  virtual ~NistEpcEnbS1SapProvider ();  

  /** 
   * 
   * 
   * \param imsi 
   * \param rnti 
   */
  virtual void InitialUeMessage (uint64_t imsi, uint16_t rnti) = 0;

  /**
   *  \brief Triggers epc-enb-application to send ERAB Release Indication message towards MME
   *  \param imsi the UE IMSI
   *  \param rnti the UE RNTI
   *  \param bearerId Bearer Identity which is to be de-activated
   */
  virtual void DoSendReleaseIndication (uint64_t imsi, uint16_t rnti, uint8_t bearerId) = 0;

  struct NistBearerToBeSwitched
  {
    uint8_t epsBearerId;
    uint32_t teid;
  };
  
  struct NistPathSwitchRequestParameters
  {
    uint16_t rnti;
    uint16_t cellId;
    uint32_t mmeUeS1Id;
    std::list<NistBearerToBeSwitched> bearersToBeSwitched;
  };

  virtual void PathSwitchRequest (NistPathSwitchRequestParameters params) = 0;


  /** 
   * release UE context at the S1 Application of the source eNB after
   * reception of the UE CONTEXT RELEASE X2 message from the target eNB
   * during X2-based handover 
   * 
   * \param rnti 
   */
  virtual void UeContextRelease (uint16_t rnti) = 0;
    
};
  


/**
 * This class implements the Service Access Point (SAP) between the
 * NistLteEnbRrc and the NistEpcEnbApplication. In particular, this class implements the
 * User part of the SAP, i.e., the methods exported by the NistLteEnbRrc
 * and called by the NistEpcEnbApplication.
 * 
 */
class NistEpcEnbS1SapUser
{
public:
  virtual ~NistEpcEnbS1SapUser ();
  
  /**
   * Parameters passed to DataRadioBearerSetupRequest ()
   * 
   */
  struct NistDataRadioBearerSetupRequestParameters
  {
    uint16_t rnti;   /**< the RNTI identifying the UE for which the
			DataRadioBearer is to be created */ 
    NistEpsBearer bearer; /**< the characteristics of the bearer to be set
                         up */
    uint8_t bearerId; /**< the EPS Bearer Identifier */
    uint32_t    gtpTeid; /**< S1-bearer GTP tunnel endpoint identifier, see 36.423 9.2.1 */
    Ipv4Address transportLayerAddress; /**< IP Address of the SGW, see 36.423 9.2.1 */
  };

  /**
   * request the setup of a DataRadioBearer
   * 
   */
  virtual void DataRadioBearerSetupRequest (NistDataRadioBearerSetupRequestParameters params) = 0;

  
  struct NistPathSwitchRequestAcknowledgeParameters
  {
    uint16_t rnti;
  };

  virtual void PathSwitchRequestAcknowledge (NistPathSwitchRequestAcknowledgeParameters params) = 0;
  
};
  



/**
 * Template for the implementation of the NistEpcEnbS1SapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 * 
 */
template <class C>
class MemberNistEpcEnbS1SapProvider : public NistEpcEnbS1SapProvider
{
public:
  MemberNistEpcEnbS1SapProvider (C* owner);

  // inherited from NistEpcEnbS1SapProvider
  virtual void InitialUeMessage (uint64_t imsi, uint16_t rnti);
  virtual void DoSendReleaseIndication (uint64_t imsi, uint16_t rnti, uint8_t bearerId);

  virtual void PathSwitchRequest (NistPathSwitchRequestParameters params);
  virtual void UeContextRelease (uint16_t rnti);

private:
  MemberNistEpcEnbS1SapProvider ();
  C* m_owner;
};

template <class C>
MemberNistEpcEnbS1SapProvider<C>::MemberNistEpcEnbS1SapProvider (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberNistEpcEnbS1SapProvider<C>::MemberNistEpcEnbS1SapProvider ()
{
}


template <class C>
void MemberNistEpcEnbS1SapProvider<C>::InitialUeMessage (uint64_t imsi, uint16_t rnti)
{
  m_owner->DoInitialUeMessage (imsi, rnti);
}

template <class C>
void MemberNistEpcEnbS1SapProvider<C>::DoSendReleaseIndication (uint64_t imsi, uint16_t rnti, uint8_t bearerId)
{
  m_owner->DoReleaseIndication (imsi, rnti, bearerId);
}

template <class C>
void MemberNistEpcEnbS1SapProvider<C>::PathSwitchRequest (NistPathSwitchRequestParameters params)
{
  m_owner->DoPathSwitchRequest (params);
}

template <class C>
void MemberNistEpcEnbS1SapProvider<C>::UeContextRelease (uint16_t rnti)
{
  m_owner->DoUeContextRelease (rnti);
}

/**
 * Template for the implementation of the NistEpcEnbS1SapUser as a member
 * of an owner class of type C to which all methods are forwarded
 * 
 */
template <class C>
class MemberNistEpcEnbS1SapUser : public NistEpcEnbS1SapUser
{
public:
  MemberNistEpcEnbS1SapUser (C* owner);

  // inherited from NistEpcEnbS1SapUser
  virtual void DataRadioBearerSetupRequest (NistDataRadioBearerSetupRequestParameters params);
  virtual void PathSwitchRequestAcknowledge (NistPathSwitchRequestAcknowledgeParameters params);

private:
  MemberNistEpcEnbS1SapUser ();
  C* m_owner;
};

template <class C>
MemberNistEpcEnbS1SapUser<C>::MemberNistEpcEnbS1SapUser (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberNistEpcEnbS1SapUser<C>::MemberNistEpcEnbS1SapUser ()
{
}

template <class C>
void MemberNistEpcEnbS1SapUser<C>::DataRadioBearerSetupRequest (NistDataRadioBearerSetupRequestParameters params)
{
  m_owner->DoDataRadioBearerSetupRequest (params);
}

template <class C>
void MemberNistEpcEnbS1SapUser<C>::PathSwitchRequestAcknowledge (NistPathSwitchRequestAcknowledgeParameters params)
{
  m_owner->DoPathSwitchRequestAcknowledge (params);
}

} // namespace ns3

#endif // NIST_EPC_ENB_S1_SAP_H
