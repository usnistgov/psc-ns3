/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef NR_SL_RRC_SAP_H
#define NR_SL_RRC_SAP_H

#include<ns3/lte-rrc-sap.h>

namespace ns3 {

/**
 * \brief User part of the Service Access Point (SAP) between UE RRC and NR
 *        sidelink UE RRC.
 */
class NrSlUeRrcSapUser
{
public:
  /**
   * \brief Destructor
   */
  virtual ~NrSlUeRrcSapUser ();

  virtual const LteRrcSap::SidelinkPreconfigNr GetNrSlPreconfiguration () = 0;

};

/**
 * \brief Provider part of the Service Access Point (SAP) between UE RRC and NR
 *        sidelink UE RRC.
 */
class NrSlUeRrcSapProvider
{
public:
  /**
   * \brief Destructor
   */
  virtual ~NrSlUeRrcSapProvider ();


};

////////////////////////////////////
//   templates UE RRC side
////////////////////////////////////


/**
 * Template for the implementation of the NrSlUeRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeRrcSapUser : public NrSlUeRrcSapUser
{
public:
  /**
   * Constructor
   *
   * \param owner the owner class
   */
  MemberNrSlUeRrcSapUser (C* owner);

  // inherited from NRSlUeRrcSapUser
  virtual const LteRrcSap::SidelinkPreconfigNr GetNrSlPreconfiguration ();


private:
  MemberNrSlUeRrcSapUser ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeRrcSapUser<C>::MemberNrSlUeRrcSapUser (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberNrSlUeRrcSapUser<C>::MemberNrSlUeRrcSapUser ()
{
}


template <class C>
const LteRrcSap::SidelinkPreconfigNr
MemberNrSlUeRrcSapUser<C>::GetNrSlPreconfiguration ()
{
  return m_owner->DoGetNrSlPreconfiguration ();
}



/**
 * Template for the implementation of the NrSlUeRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlUeRrcSapProvider : public NrSlUeRrcSapProvider
{
public:
  /**
   * Constructor
   *
   * \param owner the owner class
   */
  MemberNrSlUeRrcSapProvider (C* owner);

  // inherited from NRSlUeRrcSapUser
//  virtual void Setup (SetupParameters params);


private:
  MemberNrSlUeRrcSapProvider ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeRrcSapProvider<C>::MemberNrSlUeRrcSapProvider (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberNrSlUeRrcSapProvider<C>::MemberNrSlUeRrcSapProvider ()
{
}

/*
template <class C>
void
MemberNrSlUeRrcSapProvider<C>::Setup (SetupParameters params)
{
  m_owner->DoSetup (params);
}
*/



//eNB side SAPs



/**
 * \brief User part of the Service Access Point (SAP) between eNB RRC and NR
 *        sidelink eNB RRC.
 */
class NrSlEnbRrcSapUser
{
public:
  /**
   * \brief Destructor
   */
  virtual ~NrSlEnbRrcSapUser ();


};

/**
 * \brief Provider part of the Service Access Point (SAP) between eNB RRC and NR
 *        sidelink eNB RRC.
 */
class NrSlEnbRrcSapProvider
{
public:
  /**
   * \brief Destructor
   */
  virtual ~NrSlEnbRrcSapProvider ();


};


////////////////////////////////////
//   templates eNB RRC side
////////////////////////////////////


/**
 * Template for the implementation of the NrSlEnbRrcSapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlEnbRrcSapUser : public NrSlEnbRrcSapUser
{
public:
  /**
   * Constructor
   *
   * \param owner the owner class
   */
  MemberNrSlEnbRrcSapUser (C* owner);

  // inherited from NRSlUeRrcSapUser
//  virtual void Setup (SetupParameters params);


private:
  MemberNrSlEnbRrcSapUser ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlEnbRrcSapUser<C>::MemberNrSlEnbRrcSapUser (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberNrSlEnbRrcSapUser<C>::MemberNrSlEnbRrcSapUser ()
{
}

/*
template <class C>
void
MemberNrSlEnbRrcSapUser<C>::Setup (SetupParameters params)
{
  m_owner->DoSetup (params);
}
*/


/**
 * Template for the implementation of the NrSlEnbRrcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberNrSlEnbRrcSapProvider : public NrSlEnbRrcSapProvider
{
public:
  /**
   * Constructor
   *
   * \param owner the owner class
   */
  MemberNrSlEnbRrcSapProvider (C* owner);

  // inherited from NRSlUeRrcSapUser
//  virtual void Setup (SetupParameters params);


private:
  MemberNrSlEnbRrcSapProvider ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlEnbRrcSapProvider<C>::MemberNrSlEnbRrcSapProvider (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberNrSlEnbRrcSapProvider<C>::MemberNrSlEnbRrcSapProvider ()
{
}

/*
template <class C>
void
MemberNrSlEnbRrcSapProvider<C>::Setup (SetupParameters params)
{
  m_owner->DoSetup (params);
}
*/



}

#endif /* NR_SL_RRC_SAP_H */
