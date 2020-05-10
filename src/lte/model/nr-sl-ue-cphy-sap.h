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

#ifndef NR_SL_UE_CPHY_SAP_H
#define NR_SL_UE_CPHY_SAP_H

#include <stdint.h>
#include <ns3/ptr.h>


namespace ns3 {

/**
 * \ingroup lte
 *
 * Service Access Point (SAP) offered by the UE PHY to the UE RRC
 * for control purposes of NR Sidelink
 *
 * This is the PHY SAP Provider, i.e., the part of the SAP that contains
 * the PHY methods called by the RRC
 */
class NrSlUeCphySapProvider
{
public:
  /**
   * \brief Destructor
   */
  virtual ~NrSlUeCphySapProvider ();

  //Sidelink Communication
  /**
   * set the current sidelink transmit pool
   * \param pool The transmission pool
   */
 // virtual void SetSlCommTxPool (Ptr<SidelinkTxCommResourcePool> pool) = 0;

  /**
   * set the sidelink receiving pools
   * \param pools The sidelink receiving pools
   */
  //virtual void SetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools) = 0;

  /**
   * Remove Sidelink Communication Tx Pool function
   */
  //virtual void RemoveSlCommTxPool () = 0;

  /**
   * add a new destination to listen for
   * \param destination The destination (L2 ID) to listen for
   */
  //virtual void AddSlDestination (uint32_t destination) = 0;

  /**
   * remove a destination to listen for
   * \param destination The destination that is no longer of interest
   */
  //virtual void RemoveSlDestination (uint32_t destination) = 0;

};


/**
 * \ingroup lte
 *
 * Service Access Point (SAP) offered by the UE PHY to the UE RRC
 * for control purposes of NR Sidelink
 *
 * This is the CPHY SAP User, i.e., the part of the SAP that contains the RRC
 * methods called by the PHY
*/
class NrSlUeCphySapUser
{
public:
  /**
   * \brief Destructor
   */
  virtual ~NrSlUeCphySapUser ();

};




/**
 * \ingroup lte
 *
 * Template for the implementation of the NrSlUeCphySapProvider as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to UE PHY class, which are called by UE RRC
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlUeCphySapProvider : public NrSlUeCphySapProvider
{
public:
  /**
   * \brief Constructor
   *
   * \param owner The owner class
   */
  MemberNrSlUeCphySapProvider (C* owner);

  // methods inherited from NrSlUeCphySapProvider go here
  //NR Sidelink communication
 // virtual void SetSlCommTxPool (Ptr<SidelinkTxCommResourcePool> pool);
 // virtual void SetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools);
 // virtual void RemoveSlCommTxPool ();
 // virtual void AddSlDestination (uint32_t destination);
 // virtual void RemoveSlDestination (uint32_t destination);

private:
  MemberNrSlUeCphySapProvider ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeCphySapProvider<C>::MemberNrSlUeCphySapProvider (C* owner)
  : m_owner (owner)
{
}

//Sidelink communication
/*
template <class C>
void
MemberNrSlUeCphySapProvider<C>::SetSlCommTxPool (Ptr<SidelinkTxCommResourcePool> pool)
{
  m_owner->DoSetSlCommTxPool (pool);
}

template <class C>
void MemberNrSlUeCphySapProvider<C>::SetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools)
{
  m_owner->DoSetSlCommRxPools (pools);
}

template <class C>
void MemberNrSlUeCphySapProvider<C>::RemoveSlCommTxPool ()
{
  m_owner->DoRemoveSlCommTxPool ();
}

template <class C>
void MemberNrSlUeCphySapProvider<C>::AddSlDestination (uint32_t destination)
{
  m_owner->DoAddSlDestination (destination);
}

template <class C>
void MemberNrSlUeCphySapProvider<C>::RemoveSlDestination (uint32_t destination)
{
  m_owner->DoRemoveSlDestination (destination);
}
*/



/**
 * \ingroup lte
 *
 * Template for the implementation of the NrSlUeCphySapUser as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to UE RRC class, which are called by UE PHY
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlUeCphySapUser : public NrSlUeCphySapUser
{
public:
  /**
   * \brief Constructor
   *
   * \param owner The owner class
   */
  MemberNrSlUeCphySapUser (C* owner);

  // methods inherited from NrSlUeCphySapUser go here

private:
  C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUeCphySapUser<C>::MemberNrSlUeCphySapUser (C* owner)
  : m_owner (owner)
{
}



} // namespace ns3


#endif // NR_SL_UE_CPHY_SAP_H
