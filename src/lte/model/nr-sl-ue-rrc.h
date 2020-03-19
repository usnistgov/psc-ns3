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
#ifndef NR_SL_UE_RRC_H
#define NR_SL_UE_RRC_H


#include <ns3/object.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/nr-sl-rrc-sap.h>


namespace ns3 {

/**
 * \ingroup lte
 * Manages Sidelink information for this UE
 */
class NrSlUeRrc : public Object
{

  /// allow MemberNrSlUeRrcSapUser<LteUeRrc> class friend access
  friend class MemberNrSlUeRrcSapUser<NrSlUeRrc>;

public:
  NrSlUeRrc ();
  virtual ~NrSlUeRrc (void);

/// inherited from Object
protected:
  virtual void DoDispose ();
public:
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Get the pointer for the NR sidelink UE RRC SAP User interface
   *        offered to UE RRC by this class
   *
   * \return the pointer of type NrSlUeRrcSapUser
   */
  NrSlUeRrcSapUser* GetNrSlUeRrcSapUser ();
  /**
   * \brief Set the pointer for the NR sidelink UE RRC SAP Provider interface
   *        offered to this class by UE RRC class
   *
   * \param s the pointer of type NrSlUeRrcSapProvider
   */
   void SetNrSlUeRrcSapProvider (NrSlUeRrcSapProvider* s);
  /**
   * \brief Set the NR Sidelink communication enabled flag
   * \param status True to enable. False to disable
   */
  void SetNrSlEnabled (bool status);
  /**
   * \brief Is NR Sidelink enabled function
   * \return True if NR Sidelink communication is enabled, false otherwise
   */
  bool IsNrSlEnabled ();
  /**
   * \brief Set NR Sidelink pre-configuration function
   * \param preconfiguration The NR LteRrcSap::SidelinkPreconfigNr struct
   */
  void SetNrSlPreconfiguration (const LteRrcSap::SidelinkPreconfigNr &preconfiguration);
  /**
   * \brief Set Sidelink pre-configuration function
   * \return The NR LteRrcSap::SidelinkPreconfigNr struct
   */
  const LteRrcSap::SidelinkPreconfigNr DoGetNrSlPreconfiguration () const;
private:
  // NR sidelink UE RRC SAP
  NrSlUeRrcSapUser* m_nrSlRrcSapUser {nullptr}; ///< NR SL UE RRC SAP user
  NrSlUeRrcSapProvider* m_nrSlUeRrcSapProvider {nullptr}; ///< NR SL UE RRC SAP provider
  /**
   * Indicates if sidelink is enabled
   */
  bool m_slEnabled {false};
  /**
   * The preconfiguration for out of coverage scenarios
   */
  LteRrcSap::SidelinkPreconfigNr m_preconfiguration;
};     //end of NrSlUeRrc'class

} // namespace ns3

#endif // NR_SL_UE_RRC_H
