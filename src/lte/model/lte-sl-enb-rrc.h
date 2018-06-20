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
    friend class LteUeRrc;
    friend class UeManager;
    
   public:
    LteSlEnbRrc ();

    virtual ~LteSlEnbRrc (void);

    /**
     * \brief makes a copy of the sidelink configuration
     * \return a copy of the sidelink configuration
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

    // /**
    //  * Sets the SIB 18 information
    //  * \param sib18 The content of the system information block to broadcast
    //  */
    // void SetSystemInformationBlockType18 (LteRrcSap::SystemInformationBlockType18 sib18);

    /**
     * Gets the SIB 18 information
     * \return The SIB 18
     */
    LteRrcSap::SystemInformationBlockType18 GetSystemInformationType18 ();

    /**
     * Sets the sidelink status, i.e., True for Enabled and False for Disabled
     */
    void SetSlEnabled (bool status);

    /**
     * Gets the sidelink status
     * \return the sidelink status, i.e., True for Enabled and False for Disabled 
     */    
    bool IsSlEnabled ();

    /**
     * Utility function to preconfigure dedicated pools for UEs
     * \param group The group associated with the pool
     * \param pool The pool information
     */
    void AddPreconfiguredDedicatedPool (uint32_t group, LteRrcSap::SlCommTxResourcesSetup pool);

    /**
     * Sets the pool to be used in exceptional cases
     * \param pool The pool information
     */
    void SetCommTxPoolExceptional (LteRrcSap::SlCommTxPoolList pool);
    
  private:

    bool IsPoolInList (LteRrcSap::SlCommResourcePool pool, LteRrcSap::SlCommResourcePool *pools, int nbPool);
      
    //Add information about pools allocated for communication and discovery

    //std::map <uint16_t, LteRrcSap::SlCommTxResourcesSetup> m_dedicatedPoolMap; ///< Maps the UE's RNTI to its associated dedicated resources

    std::map <uint32_t, LteRrcSap::SlCommTxResourcesSetup> m_preconfigDedicatedPoolMap; ///< Pre-provisioned dedicated pools using the group L2 address
    LteRrcSap::SlCommTxPoolList m_preconfigCommTxPoolExceptional; ///< Pre-provisioned pool for exceptional cases
    LteRrcSap::SystemInformationBlockType18 m_sib18; ///< System Information Block Type 18 currently broadcasted
    bool m_slEnabled; ///< Indicates if Sidelink is enabled

    struct ActivePoolInfo {
      Ptr<SidelinkCommResourcePool> m_pool; ///< Pointer to the pool
      LteRrcSap::SlCommTxResourcesSetup m_poolSetup; ///< Pool in a different format
      std::set<uint16_t> m_rntiSet; ///< List of UEs assigned to the pool
    };

    std::map <uint32_t, LteSlEnbRrc::ActivePoolInfo> m_activePoolMap; ///< Map of Active pools
    
  }; //end of 'class LteSlEnbRrc


} // namespace ns3

#endif // LTE_SL_ENB_RRC_H
