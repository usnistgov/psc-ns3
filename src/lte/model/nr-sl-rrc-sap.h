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

class NrSlDataRadioBearerInfo;

/**
 * \ingroup lte
 *
 * \brief User part of the Service Access Point (SAP) between UE RRC and NR
 *        sidelink UE RRC.
 *
 * This class implements the service Access Point (SAP) for NR sidelink
 * UE RRC, i.e., the interface between the LteUeRrc and the NrSlUeRrc. In
 * particular, this class implements the User part of the
 * SAP, i.e., the sidelink related methods exported by the
 * NrSlUeRrc and called by the LteUeRrc.
 */
class NrSlUeRrcSapUser
{
public:
  /**
   * \brief Destructor
   */
  virtual ~NrSlUeRrcSapUser ();
  /**
   * \brief Get NR sidelink preconfiguration
   *
   * \return The sidelink preconfiguration
   */
  virtual const LteRrcSap::SidelinkPreconfigNr GetNrSlPreconfiguration () = 0;
  /**
   * \brief Get the physical sidelink pool based on SL bitmap and the TDD pattern
   *
   * \param slBitMap The sidelink bitmap
   * \return A vector representing the physical sidelink pool
   */
  virtual const std::vector <std::bitset<1>>
  GetPhysicalSlPool (const std::vector <std::bitset<1>> &slBitMap) = 0;
  /**
   * \brief Get Bwp Id Container
   *
   * \return The container of SL BWP ids
   */
  virtual const std::set<uint8_t> GetBwpIdContainer () = 0;
  /**
   * \brief Add NR sidelink data radio bearer
   *
   * Attempts to add a sidelink radio bearer
   *
   * \param slDrb LteSidelinkRadioBearerInfo pointer
   */
  virtual void AddNrSlDataRadioBearer (Ptr<NrSlDataRadioBearerInfo> slDrb) = 0;
  /**
   * \brief Add NR Reception sidelink data radio bearer
   *
   * Attempts to add a sidelink radio bearer for RX
   *
   * \param slRxDrb LteSidelinkRadioBearerInfo pointer
   */
  virtual void AddNrSlRxDataRadioBearer (Ptr<NrSlDataRadioBearerInfo> slRxDrb) = 0;
  /**
   * \brief Get NR Sidelink data radio bearer
   *
   * \param dstL2Id The remote/destination layer 2 id
   * \return The NrSlDataRadioBearerInfo
   */
  virtual Ptr<NrSlDataRadioBearerInfo> GetSidelinkDataRadioBearer (uint32_t dstL2Id) = 0;
  /**
   * \brief Get next LCID for setting up NR SL DRB towards the given destination
   *
   * As per, table 6.2.4-1 of 38.321 LCID for SL-SCH range from 4-19, i.e.,
   * total 16 LCIDs
   *
   * \param dstL2Id The destination layer 2 ID
   * \return the next available NR SL DRB LCID
   */
  virtual uint8_t GetNextLcid (uint32_t dstL2Id) = 0;
};

/**
 * \brief Provider part of the Service Access Point (SAP) between UE RRC and NR
 *        sidelink UE RRC.
 *
 * This class implements the service Access Point (SAP) for NR sidelink
 * UE RRC, i.e., the interface between the LteUeRrc and the NrSlUeRrc. In
 * particular, this class implements the Provider part of the
 * SAP, i.e., the sidelink related methods exported by the
 * LteUeRrc and called by the NrSlUeRrc.
 *
 */
class NrSlUeRrcSapProvider
{
public:
  /**
   * \brief Destructor
   */
  virtual ~NrSlUeRrcSapProvider ();
  /**
   * \brief Populate NR Sidelink pools
   *
   * After getting the pre-configuration
   * NrSlUeRrc instruct the LteUeRrc to
   * populate the pools.
   *
   */
  virtual void PopulatePools () = 0 ;
  /**
   * \brief Set Sidelink source layer 2 id
   *
   * \param srcL2Id The Sidelink layer 2 id of the source
   */
  virtual void SetSourceL2Id (uint32_t srcL2Id) = 0;


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
  virtual const std::vector <std::bitset<1>>
  GetPhysicalSlPool (const std::vector <std::bitset<1>> &slBitMap);
  virtual const std::set<uint8_t> GetBwpIdContainer ();
  virtual void AddNrSlDataRadioBearer (Ptr<NrSlDataRadioBearerInfo> slDrb);
  virtual void AddNrSlRxDataRadioBearer (Ptr<NrSlDataRadioBearerInfo> slRxDrb);
  virtual Ptr<NrSlDataRadioBearerInfo> GetSidelinkDataRadioBearer (uint32_t dstL2Id);
  virtual uint8_t GetNextLcid (uint32_t dstL2Id);


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

template <class C>
const std::vector <std::bitset<1>>
MemberNrSlUeRrcSapUser<C>::GetPhysicalSlPool (const std::vector <std::bitset<1>> &slBitMap)
{
  return m_owner->DoGetPhysicalSlPool (slBitMap);
}

template <class C>
const std::set<uint8_t>
MemberNrSlUeRrcSapUser<C>::GetBwpIdContainer ()
{
  return m_owner->DoGetBwpIdContainer ();
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddNrSlDataRadioBearer (Ptr<NrSlDataRadioBearerInfo> slDrb)
{
  m_owner->DoAddNrSlDataRadioBearer (slDrb);
}

template <class C>
void
MemberNrSlUeRrcSapUser<C>::AddNrSlRxDataRadioBearer (Ptr<NrSlDataRadioBearerInfo> slRxDrb)
{
  m_owner->DoAddNrSlRxDataRadioBearer (slRxDrb);
}


template <class C>
Ptr<NrSlDataRadioBearerInfo>
MemberNrSlUeRrcSapUser<C>::GetSidelinkDataRadioBearer (uint32_t dstL2Id)
{
  return m_owner->DoGetSidelinkDataRadioBearer (dstL2Id);
}

template <class C>
uint8_t
MemberNrSlUeRrcSapUser<C>::GetNextLcid (uint32_t dstL2Id)
{
  return m_owner->DoGetNextLcid (dstL2Id);
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

  // inherited from NRSlUeRrcSapProvider
  virtual void PopulatePools ();
  virtual void SetSourceL2Id (uint32_t srcL2Id);


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


template <class C>
void
MemberNrSlUeRrcSapProvider<C>::PopulatePools ()
{
  m_owner->DoPopulatePools ();
}

template <class C>
void
MemberNrSlUeRrcSapProvider<C>::SetSourceL2Id (uint32_t srsL2Id)
{
  m_owner->DoSetSourceL2Id (srsL2Id);
}




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
