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

#ifndef LTE_SL_ENB_RRC_H
#define LTE_SL_ENB_RRC_H

#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/traced-callback.h>
#include <ns3/event-id.h>

#include <ns3/lte-rrc-sap.h>
#include <ns3/lte-sl-pool.h>

#include <map>
#include <set>
#include <vector>

namespace ns3 {

/**
  * \ingroup lte
  * Manages Sidelink information for this eNodeB
  */
class LteSlEnbRrc : public Object
{
  /// allow UeManager class friend access
  friend class UeManager;

public:
  LteSlEnbRrc ();

  virtual ~LteSlEnbRrc (void);

  /**
   * \brief makes a copy of the Sidelink configuration
   * \return A copy of the Sidelink configuration
   */
  Ptr<LteSlEnbRrc> Copy ();


  // inherited from Object
protected:
  virtual void DoInitialize ();
  virtual void DoDispose ();

public:
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  /**
   * Gets the SIB 18 information
   * \return The SIB 18
   */
  LteRrcSap::SystemInformationBlockType18 GetSystemInformationType18 ();

  /**
   * Sets the Sidelink status, i.e., True for Enabled and False for Disabled
   * \param status The Sidelink Communication status True for Enabled and False for Disabled
   */
  void SetSlEnabled (bool status);

  /**
   * Gets the Sidelink status
   * \return The Sidelink status, i.e., True for Enabled and False for Disabled
   */
  bool IsSlEnabled ();

  /**
   * Sets the Sidelink Discovery status, i.e., True for Enabled and False for Disabled
   * \param status The Sidelink Discovery status True for Enabled and False for Disabled
   */
  void SetDiscEnabled (bool status);

  /**
   * Gets the Sidelink Discovery status
   * \return The Sidelink Discovery status, i.e., True for Enabled and False for Disabled
   */
  bool IsDiscEnabled ();


  /**
   * Add Preconfigured Dedicated Pool function
   * Utility function to preconfigure Sidelink Communication
   * dedicated pools for UEs
   *
   * \param group The group associated with the pool
   * \param pool The pool information
   */
  void AddPreconfiguredDedicatedPool (uint32_t group, LteRrcSap::SlCommTxResourcesSetup pool);

  /**
   * Add Preconfigured Dedicated Pool function
   * Utility function to preconfigure Sidelink Discovery
   * dedicated pools for UEs
   *
   * \param pool The Sidelink Discovery pool information
   */
  void AddPreconfiguredDedicatedPool (LteRrcSap::SlPreconfigDiscPool pool);

  /**
   * Add discovery pool to the pools list function
   * \param pool The Sidelink Discovery pool information
   */
  void AddDiscPool (LteRrcSap::SlDiscTxResourcesSetup pool);

  /**
   * Set Communication Tx pool exceptional function
   * Sets the pool to be used in exceptional cases
   *
   * \param pool The pool information
   */
  void SetCommTxPoolExceptional (LteRrcSap::SlCommTxPoolList pool);

  /**
   * Get System information type 19 function
   *
   * \return The SIB 19
   */
  LteRrcSap::SystemInformationBlockType19 GetSystemInformationType19 ();

  /**
   * Utility function to preconfigure a default Sl communication pool for UEs
   *
   * \param pool The pool information
   */
  void SetDefaultPool (LteRrcSap::SlCommTxResourcesSetup pool);

  /**
   * Set the Sidelink relay discovery configuration to be used in SIB19
   *
   * \param config sidelink relay discovery configuration
   */
  void SetDiscConfigRelay (LteRrcSap::Sib19DiscConfigRelay config);


private:
  /**
   * Is pool in list function
   * \param pool Sidelink communication pool to check
   * \param *pools Pointer to the list of communication pools
   * \param nbPool Total number of pools
   * \return True if Sidelink communication pool is in the list
   */
  bool IsPoolInList (LteRrcSap::SlCommResourcePool pool, LteRrcSap::SlCommResourcePool *pools, int nbPool);
  /**
   * Is pool in list function
   * \param pool Sidelink discovery pool to check
   * \param *pools Pointer to the list of discovery pools
   * \param nbPool Total number of pools
   * \return True if Sidelink discovery pool is in the list
   */
  bool IsPoolInList (LteRrcSap::SlDiscResourcePool pool, LteRrcSap::SlDiscResourcePool *pools, int nbPool);

  //Add information about pools allocated for communication and discovery
  std::map <uint32_t, LteRrcSap::SlCommTxResourcesSetup> m_preconfigDedicatedPoolMap;   ///< Pre-provisioned dedicated pools using the group L2 address
  // discovery pools
  // 1 pool (m_discPoolList.scheduled.discTxConfig) if scheduled
  // 1 or more pools (m_discPoolList.ueSelected.poolToAddModList.pools[i].pool
  // with i < m_discPoolList.ueSelected.poolToAddModList.nbPools) if ue-selected
  LteRrcSap::SlDiscTxResourcesSetup m_discPoolList;   ///< Sidelink Discovery pool list

  LteRrcSap::SlPreconfigDiscPool m_preconfigDiscPool;   ///< Pre-configured Sidelink Discovery pool

  LteRrcSap::SlCommTxPoolList m_preconfigCommTxPoolExceptional;   ///< Pre-provisioned pool for exceptional cases
  LteRrcSap::SystemInformationBlockType18 m_sib18;   ///< System Information Block Type 18 currently broadcasted
  LteRrcSap::SystemInformationBlockType19 m_sib19;   ///< System Information Block Type 19 currently broadcasted
  bool m_slEnabled;   ///< Indicates if Sidelink is enabled
  bool m_discEnabled;   ///< Indicates if Sidelink Discovery is enabled

  /// ActivePoolInfo structure
  struct ActivePoolInfo
  {
    Ptr<SidelinkCommResourcePool> m_pool;   ///< Pointer to the pool
    LteRrcSap::SlCommTxResourcesSetup m_poolSetup;   ///< Pool in a different format
    std::set<uint16_t> m_rntiSet;   ///< List of UEs assigned to the pool
  };

  std::map <uint32_t, LteSlEnbRrc::ActivePoolInfo> m_activePoolMap;   ///< Map of Active pools

  /// Default Sidelink communication pool structure
  struct DefaultPoolInfo
  {
	  bool m_isSet; ///< Indicates if the default pool has been configured
	  LteRrcSap::SlCommTxResourcesSetup m_poolSetup; ///< Default pool configuration
  };

  LteSlEnbRrc::DefaultPoolInfo m_defaultPoolInfo; ///< Default Sidelink communication pool

  /// Sidelink relay discovery configuration
  LteRrcSap::Sib19DiscConfigRelay m_discConfigRelay;

};   //end of 'class LteSlEnbRrc


} // namespace ns3

#endif // LTE_SL_ENB_RRC_H
