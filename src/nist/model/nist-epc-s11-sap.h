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

#ifndef NIST_EPC_S11_SAP_H
#define NIST_EPC_S11_SAP_H

#include <ns3/address.h>
#include <ns3/ptr.h>
#include <ns3/object.h>
#include <ns3/nist-eps-bearer.h>
#include <ns3/nist-epc-tft.h>
#include <list>

namespace ns3 {

class NistEpcS11Sap
{
public:

  virtual ~NistEpcS11Sap ();

  struct NistGtpcMessage
  {
    uint32_t teid;
  };

  /**
   * Fully-qualified TEID, see 3GPP TS 29.274 section 8.22
   * 
   */
  struct NistFteid 
  {
    uint32_t teid;
    Ipv4Address address;
  };

  /**
   * TS 29.274 8.21  User Location Information (ULI)
   * 
   */
  struct NistUli
  {
    uint16_t gci;
  };
  
 
};

/**
 * \ingroup lte
 *
 * MME side of the S11 Service Access Point (SAP), provides the MME
 * methods to be called when an S11 message is received by the MME. 
 */
class NistEpcS11SapMme : public NistEpcS11Sap
{
public:
  
 /**
   * 3GPP TS 29.274 version 8.3.1 Release 8 section 8.28
   * 
   */
  struct NistBearerContextCreated
  {

    NistEpcS11Sap::NistFteid sgwNistFteid;
    uint8_t epsBearerId; 
    NistEpsBearer bearerLevelQos; 
    Ptr<NistEpcTft> tft;
  };


  /**     
   * Create Session Response message, see 3GPP TS 29.274 7.2.2
   */
  struct NistCreateSessionResponseMessage : public NistGtpcMessage
  {
    std::list<NistBearerContextCreated> bearerContextsCreated;
  };


  /** 
   * send a Create Session Response message
   * 
   * \param msg the message
   */
  virtual void CreateSessionResponse (NistCreateSessionResponseMessage msg) = 0;

  struct NistBearerContextRemoved
  {
    uint8_t epsBearerId;
  };

  /**
   * Delete Bearer Request message, see 3GPP TS 29.274 Release 9 V9.3.0 section 7.2.9.2
   */
  struct NistDeleteBearerRequestMessage : public NistGtpcMessage
  {
    std::list<NistBearerContextRemoved> bearerContextsRemoved;
  };

  /**
    * \brief As per 3GPP TS 29.274 Release 9 V9.3.0, a Delete Bearer Request message shall be sent on the S11 interface by PGW to SGW and from SGW to MME
    * \param msg the message
    */
  virtual void DeleteBearerRequest (NistDeleteBearerRequestMessage msg) = 0;




  /**     
   * Modify Bearer Response message, see 3GPP TS 29.274 7.2.7
   */
  struct NistModifyBearerResponseMessage : public NistGtpcMessage
  {
    enum Cause {
      REQUEST_ACCEPTED = 0,
      REQUEST_ACCEPTED_PARTIALLY,
      REQUEST_REJECTED,
      CONTEXT_NOT_FOUND
    } cause;
  };

  /** 
   * send a Modify Bearer Response message
   * 
   * \param msg the message
   */
  virtual void ModifyBearerResponse (NistModifyBearerResponseMessage msg) = 0;

};

/**
 * \ingroup lte
 *
 * SGW side of the S11 Service Access Point (SAP), provides the SGW
 * methods to be called when an S11 message is received by the SGW. 
 */
class NistEpcS11SapSgw : public NistEpcS11Sap
{
public:

  struct NistBearerContextToBeCreated
  {    
    NistEpcS11Sap::NistFteid sgwNistFteid;
    uint8_t epsBearerId; 
    NistEpsBearer bearerLevelQos; 
    Ptr<NistEpcTft> tft;
  };

  
  /**     
   * Create Session Request message, see 3GPP TS 29.274 7.2.1
   */
  struct NistCreateSessionRequestMessage : public NistGtpcMessage
  {
    uint64_t imsi; 
    NistUli uli; 
    std::list<NistBearerContextToBeCreated> bearerContextsToBeCreated;    
  };

  /** 
   * send a Create Session Request message
   * 
   * \param msg the message
   */
  virtual void CreateSessionRequest (NistCreateSessionRequestMessage msg) = 0;

  struct NistBearerContextToBeRemoved
  {
    uint8_t epsBearerId;
  };

  /**
   * Delete Bearer Command message, see 3GPP TS 29.274 Release 9 V9.3.0 section 7.2.17.1
   */
  struct NistDeleteBearerCommandMessage : public NistGtpcMessage
  {
    std::list<NistBearerContextToBeRemoved> bearerContextsToBeRemoved;
  };

  /**
    * \brief As per 3GPP TS 29.274 Release 9 V9.3.0, a Delete Bearer Command message shall be sent on the S11 interface by the MME to the SGW
    */
  virtual void DeleteBearerCommand (NistDeleteBearerCommandMessage msg) = 0;


  struct NistBearerContextRemovedSgwPgw
  {
    uint8_t epsBearerId;
  };

  /**
   * Delete Bearer Response message, see 3GPP TS 29.274 Release 9 V9.3.0 section 7.2.10.2
   */
  struct NistDeleteBearerResponseMessage : public NistGtpcMessage
  {
    std::list<NistBearerContextRemovedSgwPgw> bearerContextsRemoved;
  };

  /**
    * \brief As per 3GPP TS 29.274 Release 9 V9.3.0, a Delete Bearer Command message shall be sent on the S11 interface by the MME to the SGW
    * \param msg the message
    */
  virtual void DeleteBearerResponse (NistDeleteBearerResponseMessage msg) = 0;

  /**     
   * Modify Bearer Request message, see 3GPP TS 29.274 7.2.7
   */
  struct NistModifyBearerRequestMessage : public NistGtpcMessage
  {
    NistUli uli;
  };

  /** 
   * send a Modify Bearer Request message
   * 
   * \param msg the message
   */
  virtual void ModifyBearerRequest (NistModifyBearerRequestMessage msg) = 0;

};







/**
 * Template for the implementation of the NistEpcS11SapMme as a member
 * of an owner class of type C to which all methods are forwarded
 * 
 */
template <class C>
class MemberNistEpcS11SapMme : public NistEpcS11SapMme
{
public:
  MemberNistEpcS11SapMme (C* owner);

  // inherited from NistEpcS11SapMme
  virtual void CreateSessionResponse (NistCreateSessionResponseMessage msg);
  virtual void ModifyBearerResponse (NistModifyBearerResponseMessage msg);
  virtual void DeleteBearerRequest (NistDeleteBearerRequestMessage msg);

private:
  MemberNistEpcS11SapMme ();
  C* m_owner;
};

template <class C>
MemberNistEpcS11SapMme<C>::MemberNistEpcS11SapMme (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberNistEpcS11SapMme<C>::MemberNistEpcS11SapMme ()
{
}

template <class C>
void MemberNistEpcS11SapMme<C>::CreateSessionResponse (NistCreateSessionResponseMessage msg)
{
  m_owner->DoCreateSessionResponse (msg);
}

template <class C>
void MemberNistEpcS11SapMme<C>::DeleteBearerRequest (NistDeleteBearerRequestMessage msg)
{
  m_owner->DoDeleteBearerRequest (msg);
}

template <class C>
void MemberNistEpcS11SapMme<C>::ModifyBearerResponse (NistModifyBearerResponseMessage msg)
{
  m_owner->DoModifyBearerResponse (msg);
}





/**
 * Template for the implementation of the NistEpcS11SapSgw as a member
 * of an owner class of type C to which all methods are forwarded
 * 
 */
template <class C>
class MemberNistEpcS11SapSgw : public NistEpcS11SapSgw
{
public:
  MemberNistEpcS11SapSgw (C* owner);

  // inherited from NistEpcS11SapSgw
  virtual void CreateSessionRequest (NistCreateSessionRequestMessage msg);
  virtual void ModifyBearerRequest (NistModifyBearerRequestMessage msg);
  virtual void DeleteBearerCommand (NistDeleteBearerCommandMessage msg);
  virtual void DeleteBearerResponse (NistDeleteBearerResponseMessage msg);

private:
  MemberNistEpcS11SapSgw ();
  C* m_owner;
};

template <class C>
MemberNistEpcS11SapSgw<C>::MemberNistEpcS11SapSgw (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberNistEpcS11SapSgw<C>::MemberNistEpcS11SapSgw ()
{
}

template <class C>
void MemberNistEpcS11SapSgw<C>::CreateSessionRequest (NistCreateSessionRequestMessage msg)
{
  m_owner->DoCreateSessionRequest (msg);
}

template <class C>
void MemberNistEpcS11SapSgw<C>::ModifyBearerRequest (NistModifyBearerRequestMessage msg)
{
  m_owner->DoModifyBearerRequest (msg);
}

template <class C>
void MemberNistEpcS11SapSgw<C>::DeleteBearerCommand (NistDeleteBearerCommandMessage msg)
{
  m_owner->DoDeleteBearerCommand (msg);
}

template <class C>
void MemberNistEpcS11SapSgw<C>::DeleteBearerResponse (NistDeleteBearerResponseMessage msg)
{
  m_owner->DoDeleteBearerResponse (msg);
}




} //namespace ns3

#endif /* NIST_EPC_S11_SAP_H */

