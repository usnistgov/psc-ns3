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

#ifndef RR_SL_FF_MAC_SCHEDULER_H
#define RR_SL_FF_MAC_SCHEDULER_H

#include <ns3/ff-mac-csched-sap.h>
#include <ns3/ff-mac-sched-sap.h>
#include <ns3/ff-mac-scheduler.h>
#include <vector>
#include <map>
#include <ns3/lte-common.h>
#include <ns3/lte-amc.h>
#include <ns3/lte-ffr-sap.h>

#define HARQ_PROC_NUM 8
#define HARQ_DL_TIMEOUT 11

namespace ns3 {


typedef std::vector < uint8_t > DlHarqProcessesStatus_t;
typedef std::vector < uint8_t > DlHarqProcessesTimer_t;
typedef std::vector < DlDciListElement_s > DlHarqProcessesDciBuffer_t;
typedef std::vector < std::vector <struct RlcPduListElement_s> > RlcPduList_t; // vector of the LCs and layers per UE
typedef std::vector < RlcPduList_t > DlHarqRlcPduListBuffer_t; // vector of the 8 HARQ processes per UE

typedef std::vector < UlDciListElement_s > UlHarqProcessesDciBuffer_t;
typedef std::vector < uint8_t > UlHarqProcessesStatus_t;




/**
 * \ingroup ff-api
 * \brief Implements the SCHED SAP and CSCHED SAP for a Sidelink Round Robin scheduler
 *
 * This class implements the interface defined by the FfMacScheduler abstract class
 */

class RrSlFfMacScheduler : public FfMacScheduler
{
public:
  /**
   * \brief Constructor
   *
   * Creates the MAC Scheduler interface implementation
   */
  RrSlFfMacScheduler ();

  /**
   * Destructor
   */
  virtual ~RrSlFfMacScheduler ();

  // inherited from Object
  virtual void DoDispose (void);
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  // inherited from FfMacScheduler
  virtual void SetFfMacCschedSapUser (FfMacCschedSapUser* s);
  virtual void SetFfMacSchedSapUser (FfMacSchedSapUser* s);
  virtual FfMacCschedSapProvider* GetFfMacCschedSapProvider ();
  virtual FfMacSchedSapProvider* GetFfMacSchedSapProvider ();

  // FFR SAPs
  virtual void SetLteFfrSapProvider (LteFfrSapProvider* s);
  virtual LteFfrSapUser* GetLteFfrSapUser ();

  /// allow MemberCschedSapProvider<RrSlFfMacScheduler> class friend access
  friend class MemberCschedSlSapProvider<RrSlFfMacScheduler>;
  /// allow MemberSchedSapProvider<RrSlFfMacScheduler> class friend access
  friend class MemberSchedSapProvider<RrSlFfMacScheduler>;

  /**
   * \brief Transmission mode configuration update function
   * \param rnti the RNTI
   * \param txMode the transmission mode
   */
  void TransmissionModeConfigurationUpdate (uint16_t rnti, uint8_t txMode);

private:
  //
  // Implementation of the CSCHED API primitives
  // (See 4.1 for description of the primitives)
  //

  /**
   * \brief CSched cell config request
   * \param params FfMacCschedSapProvider::CschedCellConfigReqParameters
   */
  void DoCschedCellConfigReq (const struct FfMacCschedSapProvider::CschedCellConfigReqParameters& params);

  /**
   * \brief CSched UE config request
   * \param params FfMacCschedSapProvider::CschedUeConfigReqParameters
   */
  void DoCschedUeConfigReq (const struct FfMacCschedSapProvider::CschedUeConfigReqParameters& params);

  /**
   * \brief CSched LC config request
   * \param params FfMacCschedSapProvider::CschedLcConfigReqParameters
   */
  void DoCschedLcConfigReq (const struct FfMacCschedSapProvider::CschedLcConfigReqParameters& params);

  /**
   * \brief CSched LC release request
   * \param params FfMacCschedSapProvider::CschedLcReleaseReqParameters
   */
  void DoCschedLcReleaseReq (const struct FfMacCschedSapProvider::CschedLcReleaseReqParameters& params);

  /**
   * \brief CSched UE release request
   * \param params FfMacCschedSapProvider::CschedUeReleaseReqParameters
   */
  void DoCschedUeReleaseReq (const struct FfMacCschedSapProvider::CschedUeReleaseReqParameters& params);

  /**
   * \brief CSched pool config request
   * \param params FfMacCschedSapProvider::CschedPoolConfigReqParameters
   */
  void DoCschedPoolConfigReq (const struct FfMacCschedSapProvider::CschedPoolConfigReqParameters& params);

  /**
   * \brief CSched pool release request
   * \param params FfMacCschedSapProvider::CschedPoolReleaseReqParameters
   */
  void DoCschedPoolReleaseReq (const struct FfMacCschedSapProvider::CschedPoolReleaseReqParameters& params);

  /**
   * \brief CSched discovery pool config request
   * \param params FfMacCschedSapProvider::CschedDiscPoolConfigReqParameters
   */
  void DoCschedDiscPoolConfigReq (const struct FfMacCschedSapProvider::CschedDiscPoolConfigReqParameters& params);

  /**
   * \brief CSched discovery pool release request
   * \param params FfMacCschedSapProvider::CschedDiscPoolReleaseReqParameters
   */
  void DoCschedDiscPoolReleaseReq (const struct FfMacCschedSapProvider::CschedDiscPoolReleaseReqParameters& params);

  //
  // Implementation of the SCHED API primitives
  // (See 4.2 for description of the primitives)
  //

  /**
   * \brief Sched DL RLC buffer request
   * \param params FfMacSchedSapProvider::SchedDlRlcBufferReqParameters
   */
  void DoSchedDlRlcBufferReq (const struct FfMacSchedSapProvider::SchedDlRlcBufferReqParameters& params);

  /**
   * \brief Sched DL paging buffer request
   * \param params FfMacSchedSapProvider::SchedDlPagingBufferReqParameters
   */
  void DoSchedDlPagingBufferReq (const struct FfMacSchedSapProvider::SchedDlPagingBufferReqParameters& params);

  /**
   * \brief Sched DL MAC buffer request
   * \param params FfMacSchedSapProvider::SchedDlMacBufferReqParameters
   */
  void DoSchedDlMacBufferReq (const struct FfMacSchedSapProvider::SchedDlMacBufferReqParameters& params);

  /**
   * \brief Sched DL trigger request
   * \param params FfMacSchedSapProvider::SchedDlTriggerReqParameters
   */
  void DoSchedDlTriggerReq (const struct FfMacSchedSapProvider::SchedDlTriggerReqParameters& params);

  /**
   * \brief Sched DL RACH info request
   * \param params FfMacSchedSapProvider::SchedDlRachInfoReqParameters
   */
  void DoSchedDlRachInfoReq (const struct FfMacSchedSapProvider::SchedDlRachInfoReqParameters& params);

  /**
   * \brief Sched DL CQI info request
   * \param params FfMacSchedSapProvider::SchedDlCqiInfoReqParameters
   */
  void DoSchedDlCqiInfoReq (const struct FfMacSchedSapProvider::SchedDlCqiInfoReqParameters& params);

  /**
   * \brief Sched UL trigger request
   * \param params FfMacSchedSapProvider::SchedUlTriggerReqParameters
   */
  void DoSchedUlTriggerReq (const struct FfMacSchedSapProvider::SchedUlTriggerReqParameters& params);

  /**
   * \brief Sched UL noise interference request
   * \param params FfMacSchedSapProvider::SchedUlNoiseInterferenceReqParameters
   */
  void DoSchedUlNoiseInterferenceReq (const struct FfMacSchedSapProvider::SchedUlNoiseInterferenceReqParameters& params);

  /**
   * \brief Sched UL SRS info request
   * \param params FfMacSchedSapProvider::SchedUlSrInfoReqParameters
   */
  void DoSchedUlSrInfoReq (const struct FfMacSchedSapProvider::SchedUlSrInfoReqParameters& params);

  /**
   * \brief Sched UL MAC control info request
   * \param params FfMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters
   */
  void DoSchedUlMacCtrlInfoReq (const struct FfMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params);

  /**
   * \brief Sched UL CQI info request
   * \param params FfMacSchedSapProvider::SchedUlCqiInfoReqParameters
   */
  void DoSchedUlCqiInfoReq (const struct FfMacSchedSapProvider::SchedUlCqiInfoReqParameters& params);

  /**
   * \brief Get RBG size function
   * \param dlbandwidth the DL bandwidth
   * \returns RBG size
   */
  int GetRbgSize (int dlbandwidth);

  /**
   * \brief Sort RLC buffer request function
   * \param i FfMacSchedSapProvider::SchedDlRlcBufferReqParameters
   * \param j FfMacSchedSapProvider::SchedDlRlcBufferReqParameters
   * \returns true if
   */
  static bool SortRlcBufferReq (FfMacSchedSapProvider::SchedDlRlcBufferReqParameters i,FfMacSchedSapProvider::SchedDlRlcBufferReqParameters j);

  /// Refresh DL CQI maps function
  void RefreshDlCqiMaps (void);
  /// Refresh UL CQI maps function
  void RefreshUlCqiMaps (void);

  /**
   * \brief Update DL RLC buffer info function
   * \param rnti the RNTI
   * \param lcid the LCID
   * \param size the size
   */
  void UpdateDlRlcBufferInfo (uint16_t rnti, uint8_t lcid, uint16_t size);
  /**
   * \brief Update UL RLC buffer info function
   * \param rnti the RNTI
   * \param size the size
   */
  void UpdateUlRlcBufferInfo (uint16_t rnti, uint16_t size);

  /**
   * \brief Update and return a new process Id for the RNTI specified
   *
   * \param rnti the RNTI of the UE to be updated
   * \return the process id  value
   */
  uint8_t UpdateHarqProcessId (uint16_t rnti);

  /**
   * \brief Return the availability of free process for the RNTI specified
   *
   * \param rnti the RNTI of the UE to be updated
   * \return the process id  value
   */
  uint8_t HarqProcessAvailability (uint16_t rnti);

  /**
   * \brief Refresh HARQ processes according to the timers
   *
   */
  void RefreshHarqProcesses ();

  Ptr<LteAmc> m_amc; ///< AMC

  /**
   * Vectors of UE's RLC info
   */
  std::list <FfMacSchedSapProvider::SchedDlRlcBufferReqParameters> m_rlcBufferReq;

  /**
   * Map of UE's DL CQI P01 received
   */
  std::map <uint16_t,uint8_t> m_p10CqiRxed;
  /**
   * Map of UE's timers on DL CQI P01 received
   */
  std::map <uint16_t,uint32_t> m_p10CqiTimers;

  /**
   * Map of previous allocated UE per RBG
   * (used to retrieve info from UL-CQI)
   */
  std::map <uint16_t, std::vector <uint16_t> > m_allocationMaps;

  /**
   * Map of UEs' UL-CQI per RBG
   */
  std::map <uint16_t, std::vector <double> > m_ueCqi;
  /**
   * Map of UEs' timers on UL-CQI per RBG
   */
  std::map <uint16_t, uint32_t> m_ueCqiTimers;



  /**
   * Map of UE's buffer status reports received
   */
  std::map <uint16_t,uint32_t> m_ceBsrRxed;

  // MAC SAPs
  FfMacCschedSapUser* m_cschedSapUser; ///< CSched SAP user
  FfMacSchedSapUser* m_schedSapUser; ///< Sched SAP user
  FfMacCschedSapProvider* m_cschedSapProvider; ///< CSched SAP provider
  FfMacSchedSapProvider* m_schedSapProvider; ///< Sched SAP provider

  // FFR SAPs
  LteFfrSapUser* m_ffrSapUser; ///< FFR SAP user
  LteFfrSapProvider* m_ffrSapProvider; ///< FFR SAP provider

  // Internal parameters
  FfMacCschedSapProvider::CschedCellConfigReqParameters m_cschedCellConfig; ///< CSched cell config

  uint16_t m_nextRntiDl; ///< RNTI of the next user to be served next scheduling in DL
  uint16_t m_nextRntiUl; ///< RNTI of the next user to be served next scheduling in UL

  uint32_t m_cqiTimersThreshold; ///< # of TTIs for which a CQI can be considered valid

  std::map <uint16_t,uint8_t> m_uesTxMode; ///< txMode of the UEs



  // HARQ attributes
  /**
   * m_harqOn when false inhibit te HARQ mechanisms (by default active)
   */
  bool m_harqOn;
  std::map <uint16_t, uint8_t> m_dlHarqCurrentProcessId; ///< DL HARQ current process ID
  //HARQ status
  // 0: process Id available
  // x>0: process Id equal to `x` transmission count
  std::map <uint16_t, DlHarqProcessesStatus_t> m_dlHarqProcessesStatus; ///< DL HARQ process status
  std::map <uint16_t, DlHarqProcessesTimer_t> m_dlHarqProcessesTimer; ///< DL HARQ process timer
  std::map <uint16_t, DlHarqProcessesDciBuffer_t> m_dlHarqProcessesDciBuffer; ///< DL HARQ process DCI buffer
  std::map <uint16_t, DlHarqRlcPduListBuffer_t> m_dlHarqProcessesRlcPduListBuffer; ///< DL HARQ process RLC PDU list buffer
  std::vector <DlInfoListElement_s> m_dlInfoListBuffered; ///< HARQ retx buffered

  std::map <uint16_t, uint8_t> m_ulHarqCurrentProcessId; ///< UL HARQ current process ID
  //HARQ status
  // 0: process Id available
  // x>0: process Id equal to `x` transmission count
  std::map <uint16_t, UlHarqProcessesStatus_t> m_ulHarqProcessesStatus; ///< UL HARQ process status
  std::map <uint16_t, UlHarqProcessesDciBuffer_t> m_ulHarqProcessesDciBuffer; ///< UL HARQ process DCI buffer


  // RACH attributes
  std::vector <struct RachListElement_s> m_rachList; ///< RACH list
  std::vector <uint16_t> m_rachAllocationMap; ///< RACH allocation map
  uint8_t m_ulGrantMcs; ///< MCS for UL grant (default 0)

  // Sidelink attributes
  /// PoolUserAllocation structure
  struct PoolUserAllocation
  {
    uint16_t m_rnti; ///< the RNTI of the UE
    //PSCCH
    uint8_t m_resPscch; ///< the PSCCH resource allocated to the UE

    //PSSCH
    uint8_t m_slItrp; ///< the Sidelink TRP index
    uint8_t m_rbStart; ///< the index of the first RB allocated
    uint8_t m_rbLen; ///< total number of RBs allocated

    std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> m_psschTx; ///< list of PSSCH transmissions within the pool
  };

  /// PoolInfo structure
  struct PoolInfo
  {
    Ptr<SidelinkCommResourcePool> m_pool; ///< the pool
    SidelinkCommResourcePool::SubframeInfo m_currentScPeriod; ///< start of current period
    SidelinkCommResourcePool::SubframeInfo m_nextScPeriod; ///< start of next period
    uint32_t m_npscch = 0; ///< number of PSCCH available in the pool

    std::map <uint16_t,uint32_t> m_ceSlBsrRxed; ///< map the RNTI and Sidelink BSR request

    std::map <uint16_t, PoolUserAllocation> m_currentAllocation; ///< map the RNTI and current allocation
    std::map <uint16_t, PoolUserAllocation> m_nextAllocation; ///< map the RNTI and next allocation
    bool m_init; ///< Flag to indicate whether the pool is initialized
  };

  std::map <uint32_t, PoolInfo> m_poolAllocations; ///< map the group id and pool info

  std::map <uint16_t,std::vector <uint32_t> > m_uesDestinations; ///< Destinations of the UEs

  // temporary variables for fixed allocation
  uint8_t m_slItrp; ///< Sidelink TRP index
  uint8_t m_slGrantSize; ///< Sidelink grant size in RBs
};
} // namespace ns3

#endif /* RR_SL_FF_MAC_SCHEDULER_H */
