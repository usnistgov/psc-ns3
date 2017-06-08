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
 * Author: Nicola Baldo <nbaldo@cttc.cat>
 */

#ifndef NIST_EPC_S1AP_SAP_H
#define NIST_EPC_S1AP_SAP_H

#include <ns3/address.h>
#include <ns3/ptr.h>
#include <ns3/object.h>
#include <ns3/nist-eps-bearer.h>
#include <ns3/nist-epc-tft.h>
#include <list>


namespace ns3 {

class NistEpcS1apSap
{
public:
  virtual ~NistEpcS1apSap ();

};


/**
 * \ingroup lte
 *
 * MME side of the S1-AP Service Access Point (SAP), provides the MME
 * methods to be called when an S1-AP message is received by the MME. 
 */
class NistEpcS1apSapMme : public NistEpcS1apSap
{
public:

  /** 
   * 
   * \param mmeUeS1Id in practice, we use the IMSI
   * \param enbUeS1Id in practice, we use the RNTI
   * \param stmsi in practice, the imsi
   * \param ecgi in practice, the cell Id
   * 
   */
  virtual void InitialUeMessage (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, uint64_t stmsi, uint16_t ecgi) = 0;


  /**
   *  E-RAB Release Indication Item IEs, 3GPP TS 36.413 version 9.8.0 section 9.1.3.7
   *
   */
  struct NistErabToBeReleasedIndication
  {
    uint8_t erabId;
  };

  /**
    * \brief As per 3GPP TS 23.401 Release 9 V9.5.0 Figure 5.4.4.2-1  eNB sends indication of Bearer Release to MME
    * \brief As per 3GPP TS version 9.8.0 section 8.2.3.2.2, the eNB initiates the procedure by sending an E-RAB RELEASE INDICATION message towards MME
    * \param mmeUeS1Id in practice, we use the IMSI
    * \param enbUeS1Id in practice, we use the RNTI
    * \param erabToBeReleaseIndication, List of bearers to be deactivated
    *
    */
  virtual void ErabReleaseIndication (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<NistErabToBeReleasedIndication> erabToBeReleaseIndication ) = 0;

  /**
   *  E-RAB Setup Item IEs, see 3GPP TS 36.413 9.1.4.2 
   * 
   */
  struct NistErabSetupItem
  {
    uint16_t    erabId;
    Ipv4Address enbTransportLayerAddress;
    uint32_t    enbTeid;    
  };

  /** 
   * INITIAL CONTEXT SETUP RESPONSE message,  see 3GPP TS 36.413 9.1.4.2 
   * 
   * \param mmeUeS1Id in practice, we use the IMSI
   * \param enbUeS1Id in practice, we use the RNTI
   * \param erabSetupList
   * 
   */
  virtual void InitialContextSetupResponse (uint64_t mmeUeS1Id,
                                            uint16_t enbUeS1Id,
                                            std::list<NistErabSetupItem> erabSetupList) = 0;


  /**
   * E-RABs Switched in Downlink Item IE, see 3GPP TS 36.413 9.1.5.8
   * 
   */
  struct NistErabSwitchedInDownlinkItem
  {
    uint16_t    erabId;
    Ipv4Address enbTransportLayerAddress;
    uint32_t    enbTeid;    
  };

  /**
   * PATH SWITCH REQUEST message, see 3GPP TS 36.413 9.1.5.8
   * 
   */
  virtual void PathSwitchRequest (uint64_t enbUeS1Id, uint64_t mmeUeS1Id, uint16_t gci, std::list<NistErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList) = 0;
};



/**
 * \ingroup lte
 *
 * eNB side of the S1-AP Service Access Point (SAP), provides the eNB
 * methods to be called when an S1-AP message is received by the eNB. 
 */
class NistEpcS1apSapEnb : public NistEpcS1apSap
{
public:


  struct NistErabToBeSetupItem
  {
    uint8_t    erabId;
    NistEpsBearer   erabLevelQosParameters;
    Ipv4Address transportLayerAddress;
    uint32_t    sgwTeid;    
  };

  /** 
   * 
   * 
   * \param mmeUeS1Id in practice, we use the IMSI
   * \param enbUeS1Id in practice, we use the RNTI
   * \param erabToBeSetupList
   * 
   */
  virtual void InitialContextSetupRequest (uint64_t mmeUeS1Id,
                                           uint16_t enbUeS1Id,
                                           std::list<NistErabToBeSetupItem> erabToBeSetupList) = 0;


  /**
   * E-RABs Switched in Uplink Item IE, see 3GPP TS 36.413 9.1.5.9
   * 
   */
  struct NistErabSwitchedInUplinkItem
  {
    uint8_t    erabId;
    Ipv4Address transportLayerAddress;
    uint32_t    enbTeid;    
  };

  /**
   * PATH SWITCH REQUEST ACKNOWLEDGE message, see 3GPP TS 36.413 9.1.5.9
   * 
   */
  virtual void PathSwitchRequestAcknowledge (uint64_t enbUeS1Id, uint64_t mmeUeS1Id, uint16_t cgi, std::list<NistErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList) = 0;


};






/**
 * Template for the implementation of the NistEpcS1apSapMme as a member
 * of an owner class of type C to which all methods are forwarded
 * 
 */
template <class C>
class MemberNistEpcS1apSapMme : public NistEpcS1apSapMme
{
public:
  MemberNistEpcS1apSapMme (C* owner);

  // inherited from NistEpcS1apSapMme
  virtual void InitialUeMessage (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, uint64_t imsi, uint16_t ecgi);
  virtual void ErabReleaseIndication (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<NistErabToBeReleasedIndication> erabToBeReleaseIndication );

  virtual void InitialContextSetupResponse (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<NistErabSetupItem> erabSetupList);
  virtual void PathSwitchRequest (uint64_t enbUeS1Id, uint64_t mmeUeS1Id, uint16_t cgi, std::list<NistErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList);

private:
  MemberNistEpcS1apSapMme ();
  C* m_owner;
};

template <class C>
MemberNistEpcS1apSapMme<C>::MemberNistEpcS1apSapMme (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberNistEpcS1apSapMme<C>::MemberNistEpcS1apSapMme ()
{
}

template <class C>
void MemberNistEpcS1apSapMme<C>::InitialUeMessage (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, uint64_t imsi, uint16_t ecgi)
{
  m_owner->DoInitialUeMessage (mmeUeS1Id, enbUeS1Id, imsi, ecgi);
}

template <class C>
void MemberNistEpcS1apSapMme<C>::ErabReleaseIndication (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<NistErabToBeReleasedIndication> erabToBeReleaseIndication)
{
  m_owner->DoErabReleaseIndication (mmeUeS1Id, enbUeS1Id, erabToBeReleaseIndication);
}

template <class C>
void MemberNistEpcS1apSapMme<C>::InitialContextSetupResponse (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<NistErabSetupItem> erabSetupList)
{
  m_owner->DoInitialContextSetupResponse (mmeUeS1Id, enbUeS1Id, erabSetupList);
}

template <class C>
void MemberNistEpcS1apSapMme<C>::PathSwitchRequest (uint64_t enbUeS1Id, uint64_t mmeUeS1Id, uint16_t cgi, std::list<NistErabSwitchedInDownlinkItem> erabToBeSwitchedInDownlinkList)
{
  m_owner->DoPathSwitchRequest (enbUeS1Id, mmeUeS1Id, cgi, erabToBeSwitchedInDownlinkList);
}







/**
 * Template for the implementation of the NistEpcS1apSapEnb as a member
 * of an owner class of type C to which all methods are forwarded
 * 
 */
template <class C>
class MemberNistEpcS1apSapEnb : public NistEpcS1apSapEnb
{
public:
  MemberNistEpcS1apSapEnb (C* owner);

  // inherited from NistEpcS1apSapEnb
  virtual void InitialContextSetupRequest (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<NistErabToBeSetupItem> erabToBeSetupList);
  virtual void PathSwitchRequestAcknowledge (uint64_t enbUeS1Id, uint64_t mmeUeS1Id, uint16_t cgi, std::list<NistErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList);

private:
  MemberNistEpcS1apSapEnb ();
  C* m_owner;
};

template <class C>
MemberNistEpcS1apSapEnb<C>::MemberNistEpcS1apSapEnb (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberNistEpcS1apSapEnb<C>::MemberNistEpcS1apSapEnb ()
{
}

template <class C>
void MemberNistEpcS1apSapEnb<C>::InitialContextSetupRequest (uint64_t mmeUeS1Id, uint16_t enbUeS1Id, std::list<NistErabToBeSetupItem> erabToBeSetupList)
{
  m_owner->DoInitialContextSetupRequest (mmeUeS1Id, enbUeS1Id, erabToBeSetupList);
}

template <class C>
void MemberNistEpcS1apSapEnb<C>::PathSwitchRequestAcknowledge (uint64_t enbUeS1Id, uint64_t mmeUeS1Id, uint16_t cgi, std::list<NistErabSwitchedInUplinkItem> erabToBeSwitchedInUplinkList)
{
  m_owner->DoPathSwitchRequestAcknowledge (enbUeS1Id, mmeUeS1Id, cgi, erabToBeSwitchedInUplinkList);
}







} //namespace ns3

#endif /* NIST_EPC_S1AP_SAP_H */

