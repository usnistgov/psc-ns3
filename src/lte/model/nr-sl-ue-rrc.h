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

#include <unordered_map>
#include <vector>


namespace ns3 {

class NrSlDataRadioBearerInfo;

/**
 * \ingroup lte
 * Manages NR Sidelink information for this UE
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
   * \brief Available TDD slot types. Ordering is important.
   */
  enum LteNrTddSlotType : uint8_t
  {
    DL = 0,  //!< DL CTRL + DL DATA
    S  = 1,  //!< DL CTRL + DL DATA + UL CTRL
    F  = 2,  //!< DL CTRL + DL DATA + UL DATA + UL CTRL
    UL = 3,  //!< UL DATA + UL CTRL
  };

  /**
   * \brief Stream output operator
   * \param os output stream
   * \param item LteNrTddSlotType whose string equivalent to output
   * \return updated stream
   */
  friend std::ostream & operator<< (std::ostream & os, LteNrTddSlotType const & item);
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);
  /**
   * Map between logical channel id and data radio bearer for transmissions
   */
  typedef std::unordered_map <uint8_t, Ptr<NrSlDataRadioBearerInfo> > NrSlDrbMapPerLcId;
  /**
   * Map between L2 id, logical channel id and data radio bearer for transmissions.
   */
  typedef std::unordered_map <uint32_t, NrSlDrbMapPerLcId> NrSlDrbMapPerL2Id;
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
   * \brief Set Sidelink source layer 2 id
   *
   * \param srcL2Id The Sidelink layer 2 id of the source
   */
  void SetSourceL2Id (uint32_t srcL2Id);
  /**
   * \brief Store Sidelink BWP id
   *
   * \param bwpId The active sidelink BWP id
   */
  void StoreSlBwpId (uint8_t bwpId);

private:
  //NrSlUeRrcSapUser methods
  /**
   * \brief Get NR sidelink preconfiguration
   *
   * \return The sidelink preconfiguration
   */
  const LteRrcSap::SidelinkPreconfigNr DoGetNrSlPreconfiguration ();
  /**
   * \brief Get the physical sidelink pool based on SL bitmap and the TDD pattern
   * \param slBitMap slBitMap The sidelink bitmap
   * \return A vector representing the physical sidelink pool
   */
  const std::vector <std::bitset<1>>
  DoGetPhysicalSlPool (const std::vector <std::bitset<1>> &slBitMap);
  /**
   * \brief Get Bwp Id Container
   * \return The container of SL BWP ids
   */
  const std::set <uint8_t> DoGetBwpIdContainer ();
  /**
   * \brief Add NR sidelink data radio bearer
   * \param slDrb LteSidelinkRadioBearerInfo pointer
   */
  void DoAddNrSlDataRadioBearer (Ptr<NrSlDataRadioBearerInfo> slDrb);
  /**
   * \brief Add NR Reception sidelink data radio bearer
   * \param slRxDrb LteSidelinkRadioBearerInfo pointer
   */
  void DoAddNrSlRxDataRadioBearer (Ptr<NrSlDataRadioBearerInfo> slRxDrb);
  /**
   * \brief Get NR Sidelink data radio bearer
   * \param dstL2Id The remote/destination layer 2 id
   * \return The NrSlDataRadioBearerInfo
   */
  Ptr<NrSlDataRadioBearerInfo> DoGetSidelinkDataRadioBearer (uint32_t dstL2Id);
  /**
   * \brief Get Source layer 2 id
   * \return source layer 2 id
   */
  uint32_t DoGetSourceL2Id ();
  /**
   * \brief Get next LCID for setting up NR SL DRB towards the given destination
   * \param dstL2Id The destination layer 2 ID
   * \return the next available NR SL DRB LCID
   */
  uint8_t DoGetNextLcid (uint32_t dstL2Id);

  //Class internal private methods and member variables

  /**
   * \brief Set the TDD pattern that the this UE RRC will utilize to compute
   *        physical SL pool.
   *
   * For example, a valid pattern would be "DL|DL|UL|UL|DL|DL|UL|UL|". The slot
   * types allowed are:
   *
   * - "DL" for downlink only
   * - "UL" for uplink only
   * - "F" for flexible (dl and ul)
   * - "S" for special slot (LTE-compatibility)
   *
   * This function is copied from mmwave-enb-phy class
   */
  void SetTddPattern ();
  /**
   * \brief Get NR Sidelink data radio bearer
   *
   * \param srcL2Id The source layer 2 id
   * \param dstL2Id The remote/destination layer 2 id
   *
   * \return The NrSlDataRadioBearerInfo
   */
  Ptr<NrSlDataRadioBearerInfo> GetSidelinkDataRadioBearer (uint32_t srcL2Id, uint32_t dstL2Id);
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
  std::vector<NrSlUeRrc::LteNrTddSlotType> m_tddPattern; //!< TDD pattern
  uint32_t m_srcL2Id {0}; //!< source layer 2 id
  //I am using std::set here instead of std::unordered_set for 2 reason:
  //1. Python bindings does not support std::unordered_set
  //2. I do not see this container to pass max 2 elements
  std::set <uint8_t> m_slBwpIds; //!< A container to store SL BWP ids
  NrSlDrbMapPerL2Id m_slDrbMap; /**< NR sidelink data radio bearer map per
                                 * destination layer 2 id. For Group-Cast
                                 * it will only hold the tx bearer info.
                                 * We use another map to store rx bearer
                                 * info.
                                 */
  NrSlDrbMapPerL2Id m_slRxDrbMap; /**< NR sidelink rx data radio bearer map
                                   * per source layer 2 id of the sender
                                   * for Group-Cast.
                                   */

};     //end of NrSlUeRrc'class

} // namespace ns3

#endif // NR_SL_UE_RRC_H
