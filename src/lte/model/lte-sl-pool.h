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

#ifndef LTE_SL_POOL_H
#define LTE_SL_POOL_H

#include <map>
#include <set>
#include "lte-rrc-sap.h"
#include <ns3/traced-callback.h>

namespace ns3 {

/*
 * Class describing a resource pool for Sidelink communication
 */
class SidelinkCommResourcePool : public Object
{
public:
  /** types of Sidelink pool */
  enum SlPoolType
  {
    UNKNOWN,
    SCHEDULED,
    UE_SELECTED
  };

  /** Identify the location of a subframe by its frame number and subframe number */
  struct SubframeInfo
  {
    uint32_t frameNo {std::numeric_limits <uint32_t>::max ()}; ///< The frame number
    uint32_t subframeNo {std::numeric_limits <uint32_t>::max ()}; ///< The subframe number

    /**
     * Adds two subframe locations and return the new location
     * This is used for computing the absolute subframe location from a starting point
     * \param lhs One of the subframe location
     * \param rhs The other subframe location
     * \return The new subframe location
     */
    friend SubframeInfo operator+ (const SubframeInfo& lhs, const SubframeInfo& rhs)
    {
      SubframeInfo res;
      uint32_t tmp1 = 10 * (lhs.frameNo % 1024) + lhs.subframeNo % 10;
      tmp1 += 10 * (rhs.frameNo % 1024) + rhs.subframeNo % 10;
      res.frameNo = (tmp1 / 10) % 1024;
      res.subframeNo = tmp1 % 10;
      return res;
    }

    /**
     * Adds two subframe locations and return the new location
     * This is used for computing the absolute subframe location from a starting point
     * \param lhs One of the subframe location
     * \param rhs The other subframe location
     * \return The new subframe location
     */
    friend SubframeInfo operator- (const SubframeInfo& lhs, const SubframeInfo& rhs)
    {
      SubframeInfo res;
      uint32_t tmp1 = 10 * (lhs.frameNo % 1024) + lhs.subframeNo % 10;
      if (lhs < rhs)
        {
          tmp1 += 10240;
        }
      tmp1 -= 10 * (rhs.frameNo % 1024) + rhs.subframeNo % 10;
      res.frameNo = (tmp1 / 10) % 1024;
      res.subframeNo = tmp1 % 10;
      return res;
    }

    /**
     * Checks if two subframe locations are identical
     * \param lhs One of the subframe location
     * \param rhs The other subframe location
     * \return true if the locations represent the same subframe
     */
    friend bool operator== (const SubframeInfo& lhs, const SubframeInfo& rhs)
    {
      return lhs.frameNo == rhs.frameNo && lhs.subframeNo == rhs.subframeNo;
    }

    /**
     * Checks if a subframe location is smaller that another subframe location
     * \param lhs One of the subframe location
     * \param rhs The other subframe location
     * \return true if the first argument represent a subframe that will happen sooner
     */
    friend bool operator< (const SubframeInfo& lhs, const SubframeInfo& rhs)
    {
      return lhs.frameNo < rhs.frameNo || (lhs.frameNo == rhs.frameNo && lhs.subframeNo < rhs.subframeNo);
    }
  };

  /** Indicates the location of a transmission on the Sidelink in time and frequency **/
  struct SidelinkTransmissionInfo
  {
    SubframeInfo subframe; ///< The time of the transmission
    uint8_t rbStart; ///< The index of the PRB where the transmission occurs
    uint8_t nbRb; ///< The number of PRBs used by the transmission
  };

  SidelinkCommResourcePool (void);
  virtual ~SidelinkCommResourcePool (void);
  /**
   * \brief Get the type ID.
   * \return The object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * TracedCallback signature for reporting next Sidelink Control (SC) period .
   *
   * \param [in] imsi
   * \param [in] cellId
   * \param [in] rnti
   * \param [in] oldState
   * \param [in] newState
   */
  typedef void (*ScPeriodTracedCallback)(const uint32_t frameNo, const uint32_t subframeNo, const uint32_t nextScPeriod);

  /**
   * Checks if two Sidelink pool configurations are identical
   * \param other The configuration of the other resource pool
   * \return true if this configuration is the same as the other one
   */
  bool operator== (const SidelinkCommResourcePool& other);

  /**
   * Configure the pool using the content of the RRC message
   * Parsing the message will indicate whether it is a scheduled or UE selected pool
   * \param pool The message containing pool information
   */
  void SetPool (LteRrcSap::SlCommResourcePool pool);

  /**
   * Configure the pool using the content of the preconfigured pool
   * Parsing the message will indicate whether it is a scheduled or UE selected pool
   * \param pool The message containing pool information
   */
  void SetPool (LteRrcSap::SlPreconfigCommPool pool);

  /**
   * Returns the type of scheduling
   * \return the type of scheduling
   */
  SlPoolType GetSchedulingType ();

  /**
   * Determines the start of the current SC period
   * \param frameNo The frame number
   * \param subframeNo The subframe number
   * \return The start of the current Sidelink period where the given subframe is located
   */
  SubframeInfo GetCurrentScPeriod (uint32_t frameNo, uint32_t subframeNo);

  /**
   * Determines the start of the next SC period
   * \param frameNo The frame number
   * \param subframeNo The subframe number
   * \return The start of the next Sidelink period where the given subframe is located
   */
  SubframeInfo GetNextScPeriod (uint32_t frameNo, uint32_t subframeNo);

  /**
   * Returns the subframe and resource block allocations of the transmissions in
   * PSCCH for the given resource
   * \param n The selected resource within the pool
   * \return The list of transmission information
   */
  std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> GetPscchTransmissions (uint32_t n);

  /**
   * Returns the list RBs that contains PSCCH opportunities in the current given subframe
   * \param frameNo The frame number
   * \param subframeNo The subframe number
   * \return The list of RBs used by the control channel on the given subframe
   */
  std::list<uint8_t> GetPscchOpportunities (uint32_t frameNo, uint32_t subframeNo);

  /**
   * Returns the list of RBs to be used in the given subframe for the given opportunity
   * \param frameNo The frame number
   * \param subframeNo The subframe number
   * \param n resource selected in the pool
   * \return The list of RBs useed by the control channel on the given subframe and resource index
   */
  std::vector<int> GetPscchRbs (uint32_t frameNo, uint32_t subframeNo, uint32_t n);

  /**
   * Returns the number of PSCCH resources from the pool
   * \return The number of PSCCH resources from the pool
   */
  uint32_t GetNPscch ();

  
  /**
   * Returns the number of transport blocks for each enabled K_TRP
   * in trpt-Subset [TS36.213, 14.1.1.3]. For values of i in 0<=i<4, 
   * v[i] = (TB for k_i), where k_i = [1, 2, 4, 8].
   * \return A vector of number of transport blocks (TB) for each enabled trpt.
   */
  std::vector<uint32_t> GetTbPerSlPeriod ();

  /**
   * Returns the subframes and RBs associated with the transmission on PSSCH
   * \param periodStart The first subframe in the Sidelink period
   * \param itrp The repetition pattern from the SCI format 0 message
   * \param rbStart The index of the PRB where the transmission occurs
   * \param rbLen The length of the transmission
   * \return The subframes and RBs associated with the transmission on PSSCH
   */
  std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> GetPsschTransmissions (SubframeInfo periodStart, uint8_t itrp, uint8_t rbStart, uint8_t rbLen);

  /**
   * Returns all PSSCH subframe index relative to the start of SL period
   * \return A vector of subframe index relative to the start of SL period
   */
  std::vector<uint32_t> GetAllPsschSubframes ();

  /**
  * Returns the hopping configuration
  * \return The hopping configuration
  */
  LteRrcSap::SlHoppingConfigComm GetDataHoppingConfig ();

  /**
   * Computes all valid rbStart values for a single Lcrb (length of contiguous RBs) value from the range [0,10]
   * \param rbLen the size of the contiguous resource block
   * \return A vector of all valid rbStart index positions for that Lcrb value
   */
  std::vector<uint8_t> GetValidRBstart (uint8_t rbLen);

  /**
   * Computes all valid rbStart values for every Lcrb value from range [0,10]
   * \return A vector of vectors which contains valid rbStart indexes for corresponding Lcrb value
   */
  std::vector< std::vector<uint8_t> > GetValidAllocations ();

    
  /**
   * Returns the PSSCH bandwidth in the pool.
   * \return Total number of PRBs in PSSCH.
   */
  uint32_t GetPsschBandwidth ();

protected:
  /**
   * Initialize the Sidelink communication pool
   */
  void Initialize ();

  /**
   * Translates the filtered subframes to subframe numbers relative to the start
   * of the SC period
   * \param info The Sidelink transmission information
   * \return The SidelinkTransmissionInfo
   */
  SidelinkCommResourcePool::SidelinkTransmissionInfo TranslatePscch (SidelinkCommResourcePool::SidelinkTransmissionInfo info);

  SlPoolType m_type; ///< The type of pool

  //Common fields for all types of pools
  LteRrcSap::SlCpLen m_scCpLen; ///< cyclic prefix length of control channel
  LteRrcSap::SlPeriodComm m_scPeriod; ///< duration of the Sidelink period
  LteRrcSap::SlTfResourceConfig m_scTfResourceConfig; ///< control pool information (subframes and PRBs)
  LteRrcSap::SlCpLen m_dataCpLen; ///< cyclic prefix for the shared channel
  LteRrcSap::SlHoppingConfigComm m_dataHoppingConfig; ///< frequency hopping parameters
  LteRrcSap::SlTrptSubset m_trptSubset; ///< normally only used for UE selected pools because it is assumed that all subframes are available in scheduled pools

  //Fields for UE selected pools
  LteRrcSap::SlTfResourceConfig m_dataTfResourceConfig; ///< shared channel pool information

private:
  /**
   * Checks if a resource with a given rbStart and length is within the valid pool range
   * \param rbStart The starting position of the contiguous resource blocks
   * \param rbLen The size of the contiguous resource blocks
   * \param rbOffset The total offset considering both
   * \return true if the resources are within the pool and false if they are not
   */
  bool IsInPool (uint8_t rbStart, uint8_t rbLen, uint8_t rbOffset = 0);

  /**
   * Computes the nPRB for Type 1 Frequency Hopping used in PSSCH
   * \param rbStart the starting position of the contiguous resource blocks
   * \return The computed Nprb value for Type 1 Frequency Hopping
   */
  uint32_t GetNprbType1 (uint8_t rbStart);

  /**
   * Computes the nPRB for Type 2 Frequency Hopping used in PSSCH
   * \param rbStart The starting position of the contiguous resource blocks
   * \param rbLen The size of the contiguous resource blocks
   * \param psschTimeIndexes The absolute subframe or slot number vector in the PSSCH.
   * \return The computed nPRB values for each nVRB on all time indexes.
   */
  std::vector <std::vector <uint32_t> > GetNprbType2 (uint8_t rbStart, uint8_t rbLen, std::vector <uint32_t> psschTimeIndexes);

  /**
   * Computes the hopping sub-band used for the given subframe when type 2 frequency hopping is enabled
   * \param i The subframe number
   * \return The sub-band index
   */
  uint8_t FHopFunction (uint32_t i);

  /**
   * Generates the a hop sequence for every subframe as described in TS 36.211 Section 5.3.4
   */
  void GenerateHopSequence ();

  /**
   * Generates the complete pseudo-random sequence as described in TS 36.211 Section 7.2
   */
  void GenerateGoldSequence ();

  /**
   * Determines if mirroring is enabled for the given subframe number
   * \param i Represents either, the subframe number, or transmission number
   * \return 0 for disabled, 1 for enabled.
   */
  uint8_t FMirroringFunction (uint32_t i);

  /**
   * Computes the frames/RBs that are part of the PSCCH pool
   */
  void ComputeNumberOfPscchResources ();

  /**
   * Computes the frame/RBS that are part of the PSSCH pool
   */
  void ComputeNumberOfPsschResources ();

  /**
   * The `ReportNextScPeriod` trace source. Fired upon when the next
   * Sidelink Control (SC) period is computed. Exporting FrameNo, SubframeNo
   * and nextStart (i.e an absolute subframe number starting from the current
   * subframe, in which the next SC period will start.
   */
  TracedCallback<uint32_t, uint32_t, uint32_t> m_nextScPeriod;

  uint32_t m_lpscch; ///< Total number of subframes that belong to a PSCCH pool
  std::vector <uint32_t> m_lpscchVector; ///< List of subframes that belong to PSCCH pool
  uint32_t m_rbpscch; ///< Total number of RBs that belong to a PSCCH pool
  std::vector <uint32_t> m_rbpscchVector; ///< List of RBs that belong to PSCCH pool
  uint32_t m_nPscchResources; ///< Number of resources in the PSCCH pools

  uint32_t m_lpssch; ///< Total number of subframes that belong to PSSCH pool
  std::vector <uint32_t> m_lpsschVector; ///< List of subframes that belong to PSSCH pool
  uint32_t m_rbpssch; ///< Total number of RBs that belong to PSSCH pool
  std::vector <uint32_t> m_rbpsschVector; ///< List of RBs that belong to PSSCH pool
  std::map <uint32_t, uint32_t> m_rbpsschPoolPrbToVrbIndexMap;  ///< RB_index, Pool_index>
  std::vector <uint8_t> m_hopSequence; ///< Hop sequence, holds hop distance for every subframe
  std::vector <uint8_t> m_goldSequence; ///< Pseudo random sequence used for frequency hopping Type 2.


  bool m_preconfigured; ///< Indicates if the pool is preconfigured
};

/**
 * Class describing a resource pool for receiving Sidelink communication
 */
class SidelinkRxCommResourcePool :  public SidelinkCommResourcePool
{
public:
  /**
   * Register this type.
   * \return The object TypeId.
   */
  static TypeId GetTypeId (void);

protected:
};

/**
 * Class describing a resource pool for transmitting Sidelink communication
 */
class SidelinkTxCommResourcePool :  public SidelinkCommResourcePool
{
public:
  /**
   * Register this type.
   * \return The object TypeId.
   */
  static TypeId GetTypeId (void);


  /**
   * Configure the pool using the content of the RRC message
   * \param pool The message containing the pool information
   */
  void SetPool (LteRrcSap::SlCommResourcePool pool);

  /**
   * Configure the pool using the content of the preconfigured pool
   * \param pool The message containing the pool information
   */
  void SetPool (LteRrcSap::SlPreconfigCommPool pool);

  /**
   * Set parameters for a scheduled pool
   * \param slrnti The Sidelink RNTI assigned by the eNodeB
   * \param macMainConfig The MAC layer configuration
   * \param commTxConfig The pool configuration
   * \param index An index associated with the resource pool
   */
  void SetScheduledTxParameters (uint16_t slrnti, LteRrcSap::SlMacMainConfigSl macMainConfig, LteRrcSap::SlCommResourcePool commTxConfig, uint8_t index);

  /**
   * Set parameters for a scheduled pool
   * \param slrnti The Sidelink RNTI assigned by the eNodeB
   * \param macMainConfig The MAC layer configuration
   * \param commTxConfig The pool configuration
   * \param index An index associated with the resource pool
   * \param mcs The MCS to be used for transmitting data
   */
  void SetScheduledTxParameters (uint16_t slrnti, LteRrcSap::SlMacMainConfigSl macMainConfig, LteRrcSap::SlCommResourcePool commTxConfig, uint8_t index, uint8_t mcs);

  /**
   * Set parameters for UE selected pool
   * \param identity The pool identity
   */
  void SetUeSelectedTxParameters (uint8_t identity);

  /**
   * Returns the index of the resource pool
   * \return the index of the resource pool
   */
  uint8_t GetIndex ();

  /**
   * Returns the MCS to use
   * \return The MCS to use
   */
  uint8_t GetMcs ();

  /**
   * Returns the pool identity
   * \return The pool identity
   */
  uint8_t GetPoolIdentity ();

  /**
    * Comparison operator
    *
    * \param rhs second pool
    * \returns true if the pools are equal
    */
    bool operator == (const SidelinkTxCommResourcePool& rhs);
    
protected:
  //Fields for UE selected pools
  LteRrcSap::SlTxParameters m_scTxParameters; ///< configuration of the control channel
  LteRrcSap::SlTxParameters m_dataTxParameters; ///< configuration of the shared channel
  uint8_t m_poolIdentity; ///< identity of the individual pool entry configured for sidelink transmission

  //Fields for scheduled pools
  uint16_t m_slrnti; ///< Sidelink RNTI
  LteRrcSap::SlMacMainConfigSl m_macMainConfig; ///< MAC layer configuration
  LteRrcSap::SlCommResourcePool m_commTxConfig; ///< resource pool configuration
  bool m_haveMcs; ///< indicates if MCS is being set
  uint8_t m_mcs; ///< the MCS value to use if set (0..28)
  uint8_t m_index; ///< index to be used in BSR

};

/**
 * Class describing a resource pool for Sidelink discovery
 */
class SidelinkDiscResourcePool : public Object
{
public:
  /** types of Sidelink pool */
  enum SlPoolType
  {
    UNKNOWN,
    SCHEDULED,
    UE_SELECTED
  };

  /** Identify the location of a subframe by its frame number and subframe number */
  struct SubframeInfo
  {
    uint32_t frameNo; ///< The frame number
    uint32_t subframeNo; ///< The subframe number

    /**
     * Adds two subframe locations and return the new location
     * This is used for computing the absolute subframe location from a starting point
     * \param lhs One of the subframe location
     * \param rhs The other subframe location
     * \return The new subframe location
     */
    friend SubframeInfo operator+ (const SubframeInfo& lhs, const SubframeInfo& rhs)
    {
      SubframeInfo res;
      uint32_t tmp1 = 10 * (lhs.frameNo % 1024) + lhs.subframeNo % 10;
      tmp1 += 10 * (rhs.frameNo % 1024) + rhs.subframeNo % 10;
      res.frameNo = tmp1 / 10;
      res.subframeNo = tmp1 % 10;
      return res;
    }

    /**
     * Checks if two subframe locations are identical
     * \param lhs One of the subframe location
     * \param rhs The other subframe location
     * \return true if the locations represent the same subframe
     */
    friend bool operator== (const SubframeInfo& lhs, const SubframeInfo& rhs)
    {
      return lhs.frameNo == rhs.frameNo && lhs.subframeNo == rhs.subframeNo;
    }

    /**
     * Checks if a subframe location is smaller that another subframe location
     * \param lhs One of the subframe location
     * \param rhs The other subframe location
     * \return true if the first argument represent a subframe that will happen sooner
     */
    friend bool operator< (const SubframeInfo& lhs, const SubframeInfo& rhs)
    {
      return lhs.frameNo < rhs.frameNo || (lhs.frameNo == rhs.frameNo && lhs.subframeNo < rhs.subframeNo);
    }
  };

  /** indicates the location of a discovery transmission on the Sidelink */
  struct SidelinkTransmissionInfo
  {
    SubframeInfo subframe; ///< The time of the transmission
    uint8_t rbStart; ///< The index of the PRB where the transmission occurs
    uint8_t nbRb; ///< The number of PRBs used by the transmission
  };

  SidelinkDiscResourcePool (void);
  virtual ~SidelinkDiscResourcePool (void);
  /**
   * Register this type.
   * \return The object TypeId.
   */
  static TypeId GetTypeId (void);

  /**
   * Configure the pool using the content of the RRC message
   * Parsing the message will indicate whether it is a scheduled or UE selected pool
   * \param pool discovery pool
   */
  void SetPool (LteRrcSap::SlDiscResourcePool pool);

  /**
   * Configure the pool using the content of the preconfigured pool
   * Parsing the message will indicate
   * \param pool The preconfigured discovery pool
   */
  void SetPool (LteRrcSap::SlPreconfigDiscPool pool);


  /**
   * Returns the type of scheduling
   * \return the type of scheduling
   */
  SlPoolType GetSchedulingType ();

  /**
   * Determines the start of the current discovery period
   * \param frameNo The frame number
   * \param subframeNo The subframe number
   * \return The start of the current discovery period
   */
  SubframeInfo GetCurrentDiscPeriod (uint32_t frameNo, uint32_t subframeNo);

  /**
   * Determines the start of the next discovery period
   * \param frameNo The frame number
   * \param subframeNo The subframe number
   * \return The start of the next discovery period
   */
  SubframeInfo GetNextDiscPeriod (uint32_t frameNo, uint32_t subframeNo);

  /**
   * Returns the number of PSDCH resources from the pool
   * \return The number of PSDCH resources from the pool
   */
  uint32_t GetNPsdch ();

  /**
   * Returns the number of subframes reserved to discovery
   * \return The number of subframes reserved to discovery
   */
  uint32_t GetNSubframes ();

  /**
   * Returns the number of resource block pairs
   * \return The number of resource block pairs
   */
  uint32_t GetNRbPairs ();

  /**
   * Returns the maximum number of retransmission
   * \return The maximum number of retransmission
   */
  uint8_t GetNumRetx ();

  /**
   * Returns the discovery period
   * \return The discovery period
   */
  int32_t GetDiscPeriod ();


  /**
   * Returns the subframes and RBs associated with the transmission on PSDCH
   * \param npsdch The index of the resource within the pool
   * \return The subframe and RBs information for transmission on PSDCH for the resource npsdch of the resource pool
   */
  std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo> GetPsdchTransmissions (uint32_t npsdch);

  /**
   * Returns a vector that contains PSDCH opportunities in the current given subframe
   * \param frameNo The frame number
   * \param subframeNo The subframe number
   * \return The vector that contains PSDCH opportunities in the current given subframe
   */
  std::vector<uint32_t> GetPsdchOpportunities (uint32_t frameNo, uint32_t subframeNo);

  /**
   * Converts the TxProbability enum to its corresponding integer value, i.e.,
   * p25->25, p50->50, p75->75 and p100->100.
   * \param prob The set transmit probability of type LteRrcSap::TxProbability for the discovery pool
   * \return The Tx probability value for a discovery pool as an unsigned integer
   */
  uint32_t TxProbabilityAsInt (LteRrcSap::TxProbability prob);

  /**
   * Converts an integer value to a corresponding enum of type LteRrcSap::TxProbability, i.e.,
   * 25->p25, 50->p50, 75->p75 and 100->p100
   * \param p The transmit probability for the discovery pool
   * \return The transmit probability of type LteRrcSap::TxProbability
   */
  static LteRrcSap::TxProbability TxProbabilityFromInt (uint32_t p);

  /**
   * \brief Returns the list of resources that conflict with the given resource in time, i.e.,
   * the resources using the same subframe.
   *
   * \param res The resource number
   * \return The list of resources that conflict with the given resource
   */
  std::set<uint32_t> GetConflictingResources (uint32_t res);

protected:
  /**
   * Initialize the PSDCH pool
   */
  void Initialize ();

  /**
   * Translates the filtered subframes to subframe numbers relative to the start discovery period
   * \param info The SidelinkTransmissionInfo
   * \return The updated SidelinkTransmissionInfo
   */
  SidelinkDiscResourcePool::SidelinkTransmissionInfo TranslatePsdch (SidelinkDiscResourcePool::SidelinkTransmissionInfo info);


  SlPoolType m_type; ///< The type of pool

  //Common fields for all types of pools
  LteRrcSap::SlCpLen m_discCpLen; ///< cyclic prefix length
  LteRrcSap::SlPeriodDisc m_discPeriod; ///< duration of the discovery period
  uint8_t m_numRetx;  ///< number of retransmission
  uint32_t m_numRepetition; ///< number of repetition
  LteRrcSap::SlTfResourceConfig m_discTfResourceConfig; ///< resource pool information
  bool m_haveTxParameters; ///< indicates if the Tx parameters are present or not
  LteRrcSap::SlTxParameters m_txParametersGeneral; ///< Tx parameters to use if defined

private:
  /**
   * Compute the frames/RBs that are part of the PSDCH pool
   * for ue-selected and scheduled cases
   */
  void ComputeNumberOfPsdchResources ();

  /**
   * \brief Compute and build a list of resources that overlap in time, i.e.,
   * the resources using the same subframe.
   */
  void BuildListOfConflictingResources ();

  uint32_t m_lpsdch; ///< Total number of subframes belong to a PSDCH pool
  std::vector <uint32_t> m_lpsdchVector;   ///< List of subframes that belong to PSDCH pool
  uint32_t m_rbpsdch; ///< Total number of RBs belong to a PSDCH pool
  std::vector <uint32_t> m_rbpsdchVector;   ///< List of RBs that belong to PSDCH pool
  uint32_t m_nPsdchResources;   ///< Number of resources in the PSDCH pools
  std::vector < std::set <uint32_t> > m_resourceConflictsVector; ///< Matrix indicating resources using the same subframes
  bool m_preconfigured;   ///< Indicates if the pool is preconfigured
};

/**
 * Class describing a resource pool for receiving Sidelink discovery (Monitor)
 */
class SidelinkRxDiscResourcePool :  public SidelinkDiscResourcePool
{
public:
  /**
   * Register this type.
   * \return The object TypeId.
   */
  static TypeId GetTypeId (void);

protected:
};

/**
 * Class describing a resource pool for transmitting Sidelink discovery (Announce)
 */
class SidelinkTxDiscResourcePool :  public SidelinkDiscResourcePool
{
public:
  /**
   * Register this type.
   * \return The object TypeId.
   */
  static TypeId GetTypeId (void);


  /**
   * Configure the pool
   * \param pool discovery pool to be configured
   */
  void SetPool (LteRrcSap::SlDiscResourcePool pool);

  /**
   * Configure the pool using the content of the preconfigured pool
   * \param pool The preconfigured pool
   */
  void SetPool (LteRrcSap::SlPreconfigDiscPool pool);

  /**
   * Set parameters for scheduled config
   * \param discPool The discovery resource pool
   * \param discResources The resources in terms of resource blocks and subframes
   * \param discHopping The frequency hopping configuration
   */
  void SetScheduledTxParameters (LteRrcSap::SlDiscResourcePool discPool, LteRrcSap::SlTfIndexPairList discResources, LteRrcSap::SlHoppingConfigDisc discHopping);

  /**
   * Set parameters for UE selected pool
   * \param identity The pool identity
   */
  void SetUeSelectedTxParameters (uint8_t identity);

  /**
   * Returns the transmission probability
   * \return The transmission probability
   */
  uint32_t GetTxProbability ();

  /**
   * Sets the transmission probability
   * \param theta The transmission probability in percentage (25, 50, 75, or 100)
   */
  void SetTxProbability (uint32_t theta);

  /**
   * Returns the pool identity
   * \return The pool identity
   */
  uint8_t GetPoolIdentity ();

protected:
  //Fields for UE selected pools
  LteRrcSap::PoolSelection m_poolSelection; ///< method for selecting the pool
  bool m_havePoolSelectionRsrpBased; ///< indicates if the pool selection is RSRP based
  LteRrcSap::PoolSelectionRsrpBased m_poolSelectionRsrpBased; ///< parameters for the RSRP based selection
  LteRrcSap::TxProbability m_txProbability;  ///< transmission probability
  bool m_txProbChanged; ///< indicates if the transmission probability has changed
  uint8_t m_poolIdentity; ///< identity of the individual pool entry configured for sidelink transmission

  //Fields for scheduled pools
  LteRrcSap::SlDiscResourcePool m_discTxConfig; ///< resource configuration
  LteRrcSap::SlTfIndexPairList m_discTfIndexList; ///< index of the resource pool
  LteRrcSap::SlHoppingConfigDisc m_discHoppingConfigDisc; ///< frequency hopping configuration

};



} // namespace ns3

#endif //LTE_SL_POOL_H
