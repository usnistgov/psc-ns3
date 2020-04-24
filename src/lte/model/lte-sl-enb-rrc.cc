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

#include "lte-sl-enb-rrc.h"

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/abort.h>

#include <ns3/pointer.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/simulator.h>

#include <ns3/lte-rrc-sap.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlEnbRrc");
NS_OBJECT_ENSURE_REGISTERED (LteSlEnbRrc);

TypeId LteSlEnbRrc::GetTypeId (void)
{
  static TypeId  tid = TypeId ("ns3::LteSlEnbRrc")
    .SetParent<Object> ()
    .AddConstructor<LteSlEnbRrc> ()

  ;
  return tid;
}

LteSlEnbRrc::LteSlEnbRrc ()
  : m_slEnabled (false),
  m_discEnabled (false)
{
  m_preconfigCommTxPoolExceptional.nbPools = 0;
  m_defaultPoolInfo.m_isSet = false;
}

void
LteSlEnbRrc::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
}

LteSlEnbRrc::~LteSlEnbRrc (void)
{
  NS_LOG_FUNCTION (this);
}

void
LteSlEnbRrc::DoDispose ()
{
  NS_LOG_FUNCTION (this);
}

/**
 * \brief makes a copy of the sidelink configuration
 * \return a copy of the sidelink configuration
 */
Ptr<LteSlEnbRrc>
LteSlEnbRrc::Copy ()
{
  NS_LOG_FUNCTION (this);
  Ptr<LteSlEnbRrc> copy = CreateObject<LteSlEnbRrc> ();
  //copy internal values
  copy->m_slEnabled = m_slEnabled;
  copy->m_sib18 = m_sib18;
  copy->m_preconfigDedicatedPoolMap = m_preconfigDedicatedPoolMap;
  copy->m_preconfigCommTxPoolExceptional = m_preconfigCommTxPoolExceptional;
  copy->m_sib19 = m_sib19;
  copy->m_discEnabled = m_discEnabled;
  copy->m_discPoolList = m_discPoolList;
  copy->m_preconfigDiscPool = m_preconfigDiscPool;
  copy->m_defaultPoolInfo = m_defaultPoolInfo;
  copy->m_discConfigRelay = m_discConfigRelay;

  return copy;
}


// void
// LteSlEnbRrc::SetSystemInformationBlockType18 (LteRrcSap::SystemInformationBlockType18 sib18)
// {
//   NS_LOG_FUNCTION (this);
//   m_sib18 = sib18;
// }

/**
 * Gets the SIB 18 information
 * \return The SIB 18
 */
LteRrcSap::SystemInformationBlockType18
LteSlEnbRrc::GetSystemInformationType18 ()
{
  NS_LOG_FUNCTION (this);

  //build the content of SIB 18 by looking at pools allocated
  LteRrcSap::SystemInformationBlockType18 sib18;
  sib18.commConfig.commRxPool.nbPools = 0;
  sib18.commConfig.commTxPoolNormalCommon.nbPools = 0;
  sib18.commConfig.commTxPoolExceptional.nbPools = 0;

  std::map <uint32_t, LteSlEnbRrc::ActivePoolInfo>::iterator it;
  for (it = m_activePoolMap.begin (); it != m_activePoolMap.end (); it++)
    {
      //check if pool already listed
      if (it->second.m_poolSetup.setup == LteRrcSap::SlCommTxResourcesSetup::SCHEDULED)
        {
          //check single pool
          if (!IsPoolInList (it->second.m_poolSetup.scheduled.commTxConfig, sib18.commConfig.commRxPool.pools, sib18.commConfig.commRxPool.nbPools))
            {
              //add pool
              sib18.commConfig.commRxPool.pools[sib18.commConfig.commRxPool.nbPools] = it->second.m_poolSetup.scheduled.commTxConfig;
              sib18.commConfig.commRxPool.nbPools++;
            }
        }
      else if (it->second.m_poolSetup.setup == LteRrcSap::SlCommTxResourcesSetup::UE_SELECTED)
        {
          if (it->second.m_poolSetup.ueSelected.havePoolToAdd)
            {
              //check all pools
              for (int i = 0; i < it->second.m_poolSetup.ueSelected.poolToAddModList.nbPools; i++)
                {
                  if (!IsPoolInList (it->second.m_poolSetup.ueSelected.poolToAddModList.pools[i].pool, sib18.commConfig.commRxPool.pools, sib18.commConfig.commRxPool.nbPools))
                    {
                      //add pool
                      sib18.commConfig.commRxPool.pools[sib18.commConfig.commRxPool.nbPools] = it->second.m_poolSetup.ueSelected.poolToAddModList.pools[i].pool;
                      sib18.commConfig.commRxPool.nbPools++;
                    }
                }
            }
        }
      else
        {
          NS_FATAL_ERROR ("Unable to find resource allocation type while building SIB 18."
                          " Please make sure to properly configure the resource allocation type while configuring the pool");
        }
    }
  //Adding the default pool (if set) to the SIB18
  if(m_defaultPoolInfo.m_isSet){
	  if (m_defaultPoolInfo.m_poolSetup.setup == LteRrcSap::SlCommTxResourcesSetup::SCHEDULED)
	  {
		  sib18.commConfig.commRxPool.pools[sib18.commConfig.commRxPool.nbPools] = m_defaultPoolInfo.m_poolSetup.scheduled.commTxConfig;
		  sib18.commConfig.commRxPool.nbPools++;
	  }
	  else if (m_defaultPoolInfo.m_poolSetup.setup == LteRrcSap::SlCommTxResourcesSetup::UE_SELECTED)
	  {
		  sib18.commConfig.commRxPool.pools[sib18.commConfig.commRxPool.nbPools] = m_defaultPoolInfo.m_poolSetup.ueSelected.poolToAddModList.pools[0].pool;
		  sib18.commConfig.commRxPool.nbPools++;
	  }
  }

  sib18.commConfig.commTxPoolExceptional = m_preconfigCommTxPoolExceptional;

  return sib18;
}

bool
LteSlEnbRrc::IsPoolInList (LteRrcSap::SlCommResourcePool pool, LteRrcSap::SlCommResourcePool *pools, int nbPool)
{
  bool found = false;
  for (int i = 0; i < nbPool && !found; i++)
    {
      found = pool == pools[i];
    }
  return found;
}


void
LteSlEnbRrc::SetSlEnabled (bool status)
{
  NS_LOG_FUNCTION (this);
  m_slEnabled = status;
}

bool
LteSlEnbRrc::IsSlEnabled ()
{
  NS_LOG_FUNCTION (this);
  return m_slEnabled;
}

void
LteSlEnbRrc::SetDiscEnabled (bool status)
{
  NS_LOG_FUNCTION (this);
  m_discEnabled = status;
}

bool
LteSlEnbRrc::IsDiscEnabled ()
{
  NS_LOG_FUNCTION (this);
  return m_discEnabled;
}

void
LteSlEnbRrc::AddPreconfiguredDedicatedPool (uint32_t group, LteRrcSap::SlCommTxResourcesSetup pool)
{
  NS_LOG_FUNCTION (this);
  m_preconfigDedicatedPoolMap.insert (std::pair<uint32_t, LteRrcSap::SlCommTxResourcesSetup> (group, pool));
}

void
LteSlEnbRrc::AddPreconfiguredDedicatedPool (LteRrcSap::SlPreconfigDiscPool pool)
{
  NS_LOG_FUNCTION (this);
  m_preconfigDiscPool = pool;
}

void
LteSlEnbRrc::AddDiscPool (LteRrcSap::SlDiscTxResourcesSetup pool)
{
  NS_LOG_FUNCTION (this);
  m_discPoolList = pool;
}

void
LteSlEnbRrc::SetCommTxPoolExceptional (LteRrcSap::SlCommTxPoolList pool)
{
  NS_LOG_FUNCTION (this);
  m_preconfigCommTxPoolExceptional = pool;
}

LteRrcSap::SystemInformationBlockType19
LteSlEnbRrc::GetSystemInformationType19 ()
{
  NS_LOG_FUNCTION (this);

  //build the content of SIB 19 by looking at pools allocated
  LteRrcSap::SystemInformationBlockType19 sib19;
  sib19.discConfig.discRxPool.nbPools = 0;
  sib19.discConfig.discTxPoolCommon.nbPools = 0;
  sib19.discConfig.discTxPowerInfo.nbPowerInfo = 0;

  //check if pool already listed
  if (m_discPoolList.setup == LteRrcSap::SlDiscTxResourcesSetup::SCHEDULED)
    {  //check single pool
      if (!IsPoolInList (m_discPoolList.scheduled.discTxConfig, sib19.discConfig.discRxPool.pools, sib19.discConfig.discRxPool.nbPools))
        {
          //add pool
          sib19.discConfig.discRxPool.pools[sib19.discConfig.discRxPool.nbPools] = m_discPoolList.scheduled.discTxConfig;
          sib19.discConfig.discRxPool.nbPools++;
        }
    }
  else if (m_discPoolList.setup == LteRrcSap::SlDiscTxResourcesSetup::UE_SELECTED)
    {
      if (m_discPoolList.ueSelected.havePoolToAdd)
        {
          //check all pools
          for (int i = 0; i < m_discPoolList.ueSelected.poolToAddModList.nbPools; i++)
            {
              if (!IsPoolInList (m_discPoolList.ueSelected.poolToAddModList.pools[i].pool, sib19.discConfig.discRxPool.pools, sib19.discConfig.discRxPool.nbPools))
                {
                  //add pool
                  sib19.discConfig.discRxPool.pools[sib19.discConfig.discRxPool.nbPools] = m_discPoolList.ueSelected.poolToAddModList.pools[i].pool;
                  sib19.discConfig.discRxPool.nbPools++;
                }
            }
        }
    }
  else
    {
      NS_FATAL_ERROR ("Unable to find resource allocation type while building SIB 19."
                      " Please make sure to properly configure the resource allocation type while configuring the pool");
    }

  //Configure UE-to-Network Relay Discovery
  sib19.discConfigRelay = m_discConfigRelay;

  return sib19;
}

bool
LteSlEnbRrc::IsPoolInList (LteRrcSap::SlDiscResourcePool pool, LteRrcSap::SlDiscResourcePool *pools, int nbPool)
{
  bool found = false;
  for (int i = 0; i < nbPool && !found; i++)
    {
      found =  pool == pools[i];
    }
  return found;
}

void LteSlEnbRrc::SetDefaultPool (LteRrcSap::SlCommTxResourcesSetup pool)
{
	NS_LOG_FUNCTION (this);
	m_defaultPoolInfo.m_poolSetup = pool;
	m_defaultPoolInfo.m_isSet = true;
}

void
LteSlEnbRrc::SetDiscConfigRelay (LteRrcSap::Sib19DiscConfigRelay config)
{
  NS_LOG_FUNCTION (this);
  m_discConfigRelay = config;
}

} // namespace ns3
