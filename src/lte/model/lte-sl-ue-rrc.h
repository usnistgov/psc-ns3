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

#ifndef LTE_SL_UE_RRC_H
#define LTE_SL_UE_RRC_H

#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/traced-callback.h>
#include <ns3/event-id.h>

#include <ns3/lte-rrc-sap.h>
#include <ns3/lte-sl-pool.h>
#include <ns3/lte-radio-bearer-info.h>
#include <ns3/random-variable-stream.h>

#include <map>
#include <set>
#include <vector>

namespace ns3 {

/**
 * \ingroup lte
 * Manages Sidelink information for this UE
 */
class LteSlUeRrc : public Object
{
  friend class LteUeRrc;

  // Structure to store Sidelink configuration for a given plmnid
  struct LteSlCellConfiguration
  {
    uint16_t cellId;
    bool haveSib18; //may need to be replaced with another struct containing frame information
    LteRrcSap::SystemInformationBlockType18 sib18;
    //Add sib19 and others as needed
    bool haveSib19;
    LteRrcSap::SystemInformationBlockType19 sib19;
  };
public:
  LteSlUeRrc ();
  virtual ~LteSlUeRrc (void);
  /**
   * \brief makes a copy of the sidelink configuration
   * \return a copy of the sidelink configuration
   */
  Ptr<LteSlUeRrc> Copy ();
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

  void SetSlEnabled (bool status);
  bool IsSlEnabled ();
  void SetDiscEnabled (bool status);
  bool IsDiscEnabled ();
  void SetSlPreconfiguration (LteRrcSap::SlPreconfiguration preconfiguration);
  LteRrcSap::SlPreconfiguration GetSlPreconfiguration ();
  void SetSourceL2Id (uint32_t src);
  /**
   * Set discTxResourceReq
   *
   */
  void SetDiscTxResources (uint8_t nb);
  /**
   * Set discInterFreq
   *
   */
  void SetDiscInterFreq (uint16_t nb);
  /**
   * Returns discTxResourceReq
   * \return discTxResourceReq
   */
  uint8_t GetDiscTxResources ();
  /***
   * Retuens discInterFreq
   * \return discInterFreq
   *
   */
  uint16_t GetDiscInterFreq ();
protected:
  /**
   * Indicates if the UE is interested in sidelink transmission
   * \return True if the UE is interested in sidelink transmission
   */
  bool IsTxInterested ();
  /**
   * Indicates if the UE is interested in sidelink transmission
   * \return True if the UE is interested in sidelink transmission
   */
  bool IsRxInterested ();
  /**
   * Indicates if the UE is interested in discovery Monitoring
   * \return True if the UE is interested in discovery Monitoring
   */
  bool IsMonitoringInterested ();
  /**
   * Indicates if the UE is interested in discovery Announcing
   * \return True if the UE is interested in discovery Announcing
   */
  bool IsAnnouncingInterested ();
  /**
   * Returns the list of destinations this UE is interested in transmitting
   * \return the list of destinations
   */
  std::list <uint32_t> GetTxDestinations ();
  /**
   * Attempts to add a sidelink radio bearer
   * \return True if the sidelink was successfully added, else false such as when
   * an identical bearer already exists
   */
  bool AddSidelinkRadioBearer (Ptr<LteSidelinkRadioBearerInfo> slb);
  /**
   * Deletes a sidelink radio bearer
   * \param group The group identifying the radio bearer to delete
   * \return true if the bearer was successfully deleted
   */
  bool DeleteSidelinkRadioBearer (uint32_t src, uint32_t group);
  /**
   * Return the radio bearer for the given group
   * \return The radio bearer for the given group
   */
  Ptr<LteSidelinkRadioBearerInfo> GetSidelinkRadioBearer (uint32_t src, uint32_t group);
  /**
   * Return the transmit radio bearer for the given group
   * \return The transmit radio bearer for the given group
   */
  Ptr<LteSidelinkRadioBearerInfo> GetSidelinkRadioBearer (uint32_t group);
  /**
   * Add applications depending on the interest (monitoring or announcing)
   * \param apps applicatiosn IDs to be added
   * \param rxtx 0 to monitor and 1 to announce
   */
  void AddDiscoveryApps (std::list<uint32_t> apps, bool rxtx);
  /**
   * Remove applications depending on the interest (monitoring or announcing)
   *
   * \param apps applicatiosn IDs to be removed
   * \param rxtx 0 to monitor and 1 to announce
   */
  void RemoveDiscoveryApps (std::list<uint32_t> apps, bool rxtx);
  /**
   * Records the time of transmission
   */
  void RecordTransmissionOfSidelinkUeInformation ();
  /**
   * Returns the time of last transmission
   * \return The time of last transmission
   */
  double GetTimeSinceLastTransmissionOfSidelinkUeInformation ();
  /**
   * Returns the next available LCID
   */
  uint8_t GetNextLcid ();
  /**
   * Indicates if cell is broadcasting SIB 18
   * \param The cell ID
   * \return true if the SIB 18 was received from the cell
   */
  bool IsCellBroadcastingSIB18 (uint16_t cellId);
  /**
   * Indicates if cell is broadcasting SIB 19
   * \param The cell ID
   * \return true if the SIB 19 was received from the cell
   */
  bool IsCellBroadcastingSIB19 (uint16_t cellId);
  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this class. Return the number of stream indices assigned.
   *
   * \param stream first stream index to use
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);
private:
  /**
   * Indicates if sidelink is enabled
   */
  bool m_slEnabled;
  /**
   * Indicates if discovery is enabled
   */
  bool m_discEnabled;
  /**
   * The preconfiguration for out of coverage scenarios
   */
  LteRrcSap::SlPreconfiguration m_preconfiguration;
  /**
   * Map between cell ID and sidelink configuration
   */
  std::map <uint16_t, LteSlCellConfiguration> m_slMap;
  /**
   * Map between source, group, and radio bearer for transmissions
   */
  std::map <uint32_t, std::map <uint32_t, Ptr<LteSidelinkRadioBearerInfo> > > m_slrbMap;
  /**
   * The time the last SidelinkUeInformation was sent
   */
  Time m_lastSidelinkUeInformationTime;
  /**
   * The source layer 2 ID to be used
   */
  uint32_t m_sourceL2Id;
  /*
   * List of groups for which the UE is interested in receiving and may or may not have
   * radio bearers yet
   */
  std::list<uint32_t> m_rxGroup;

  std::vector< std::pair <LteRrcSap::SlCommResourcePool, Ptr<SidelinkRxCommResourcePool> > > rxPools;
  /**
   * list of IDs of applications to monitor
   */
  std::list<uint32_t> m_monitorApps;
  /**
   * list of IDs of applications to announce
   */
  std::list<uint32_t> m_announceApps;
  /**
   * List of receiving pools
   */
  std::vector< std::pair <LteRrcSap::SlDiscResourcePool, Ptr<SidelinkRxDiscResourcePool> > > monitorPools;
  /*
   * discTxResourceReq : number of resources the UE requires every discovery period
   * for transmitting sidelink direct discovery announcement
   * i.e. number of separate discovery message(s) the UE wants to transmit every discovery period
   * (to be set in the scenario)
   */
  uint8_t m_discTxResources;
  /*
   * frequency that the UE is supposed to monitor for discovery announcements
   */
  uint16_t m_discInterFreq;

  Ptr<UniformRandomVariable> m_rand;
};     //end of LteSlUeRrc'class

} // namespace ns3

#endif // LTE_SL_UE_RRC_H
