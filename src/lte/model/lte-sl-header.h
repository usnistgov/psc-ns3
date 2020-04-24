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

#ifndef LTE_SL_HEADER_H
#define LTE_SL_HEADER_H

#include "ns3/header.h"
#include <ns3/ff-mac-common.h>

#include <list>

namespace ns3 {

/**
 * \ingroup lte
 * \brief The packet header for the discovery packets
 *
 */
class LteSlDiscHeader : public Header
{
public:
  /**
   * The various types of discovery message currently supported
   */
  enum DiscoveryMsgType : uint8_t
  {
    DISC_OPEN_ANNOUNCEMENT = 65, /* Open discovery announce model A */
    DISC_RESTRICTED_RESPONSE = 130, /* Restricted discovery response model B */
    DISC_RESTRICTED_QUERY = 134, /* Restricted discovery request model B */
    DISC_RELAY_ANNOUNCEMENT = 145, /* Relay Discovery Announcement in model A */
    DISC_RELAY_SOLICITATION = 150, /* Relay Discovery Announcement in model B */
    DISC_RELAY_RESPONSE = 146, /* UE-to-Network Relay Discovery Response in model B */
  };

  /**
   * \brief Constructor
   *
   * Creates a null header
   */
  LteSlDiscHeader ();
  ~LteSlDiscHeader ();

  /**
   * \brief Set the MIC
   *
   * \param mic The MIC attribute
   */
  void SetMic (uint32_t mic);

  /**
   * \brief Set the UTC base counter
   *
   * \param counter the UTC base counter
   */
  void SetUtcBaseCounter (uint8_t counter);

  /**
   * \brief Builds and return the discovery message type value
   *
   * \return the discovery message type
   */
  uint8_t BuildDiscoveryMsgType ();

  /**
   * \brief Get the discovery message type
   *
   * \return the discovery message type
   */
  uint8_t GetDiscoveryMsgType () const;

  /**
   * \brief Get the discovery type
   *
   * \return the discovery type
   */
  uint8_t GetDiscoveryType () const;

  /**
   * Get the discovery content type
   *
   * \return the discovery content type
   */
  uint8_t GetDiscoveryContentType () const;

  /**
   * \brief Get the discovery model
   *
   * \return the discovery model
   */
  uint8_t GetDiscoveryModel () const;

  /**
   * \brief Get the MIC
   *
   * \return the MIC
   */
  uint32_t GetMic () const;

  /**
   * \brief Get the UTC base counter
   *
   * \return the UTC base counter
   */
  uint8_t GetUtcBaseCounter () const;

  /**
   * \brief Get the application code
   *
   * \return the application code
   */
  uint32_t GetApplicationCode () const;

  /**
   * \brief Get the relay service code
   *
   * \return the relay service code
   */
  uint32_t GetRelayServiceCode () const;

  /**
   * \brief Get the user info
   *
   * \return the user info
   */
  uint64_t GetInfo () const;

  /**
   * \brief Get the relay UE ID
   *
   * \return the relay UE ID
   */
  uint32_t GetRelayUeId () const;

  /**
   * \brief Get the status indicator
   *
   * \return the status indicator
   */
  uint8_t GetStatusIndicator () const;

  /**
   * \brief Get the URDS composition
   *
   * \return the URDS composition
   */
  uint8_t GetURDSComposition () const;

  /**
   * \brief Get the group
   *
   * \return the group
   */
  uint32_t GetGroup () const;

  /**
   * \brief Get the group info
   *
   * \return the group info
   */
  uint32_t GetGroupInfo () const;

  /**
   * \brief Set the parameters for open discovery announcement
   *
   * \param appCode The application code
   */
  void SetOpenDiscoveryAnnounceParameters (uint32_t appCode);

  /**
   * \brief Set the parameters for the restricted discovery request
   *
   * \param appCode The application code
   */
  void SetRestrictedDiscoveryQueryParameters (uint32_t appCode);

  /**
   * \brief Set the parameters for the restricted discovery response
   *
   * \param appCode The application code
   */
  void SetRestrictedDiscoveryResponseParameters (uint32_t appCode);

  /**
   * \brief Set the parameters for the UE-to-Network relay announcement
   *
   * \param serviceCode the service code
   * \param announcerInfo the announcer information
   * \param relayUeId the layer 2 ID of the relay node
   * \param status the status code
   */
  void SetRelayAnnouncementParameters (uint32_t serviceCode, uint64_t announcerInfo, uint32_t relayUeId, uint32_t status);

  /**
   * \brief Set the parameters for the UE-to-Network relay solicitation
   *
   * \param serviceCode the service code
   * \param discovererInfo the discoverer information
   * \param relayUeId the layer 2 ID of the relay node
   */
  void SetRelaySoliciationParameters (uint32_t serviceCode, uint64_t discovererInfo, uint32_t relayUeId);

  /**
   * \brief Set the parameters for the UE-to-Network relay response
   *
   * \param serviceCode the service code
   * \param discovereeInfo the announcer information
   * \param relayUeId the layer 2 ID of the relay node
   * \param status the status code
   */
  void SetRelayResponseParameters (uint32_t serviceCode, uint64_t discovereeInfo, uint32_t relayUeId, uint32_t status);

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  uint8_t m_discoveryMsgType; ///< Discovery message type, built from the next three variables
  uint8_t m_discoveryType; ///< Discovery type
  uint8_t m_discoveryContentType; ///< Discovery content type
  uint8_t m_discoveryModel; ///< Discovery model

  uint32_t m_appCode; ///< ProSe Application Code (32 bits instead of 184 bits defined in standard)
  uint32_t m_relayServiceCode; ///< Relay service code
  uint64_t m_info; ///< Info for announcer, or discoverer, or discoveree, or user
  uint32_t m_relayUeId; ///< Relay layer 2 ID
  uint8_t m_statusIndicator; ///< Status indicator

  //variables for group member discovery
  uint32_t m_group; ///< Group ID
  uint32_t m_groupInfo; ///< Group information

  uint32_t m_mic; ///< Message Integrity Check
  uint8_t m_utcBasedCounter; ///< UTC time associated with the discovery transmission opportunity

};

/**
 * \ingroup lte
 * \brief The packet header for the Sidelink Control Information packets
 * Header size is 32 bits + ceil (log2(Nrb*(Nrb+1)/2))
 */
class LteSlSciHeader : public Header
{
public:
  /**
   * \brief Constructor
   *
   * Creates an SCI header
   */
  LteSlSciHeader ();
  ~LteSlSciHeader ();

  /**
   * \brief Set the SCI message information
   * @param hopping Frequency hopping flag
   * @param rbStart Resource block index of the start of the transmission
   * @param rbLen Number of resource blocks to be used by the transmission
   * @param hoppingInfo Frequency hopping configuration
   * @param trp Time Resource Pattern
   * @param mcs Modulation and coding scheme used to transmit the message
   * @param groupId The layer 1 group identity
   */
  void SetSciFormat0Params (bool hopping, uint8_t rbStart, uint8_t rbLen, uint8_t hoppingInfo, uint8_t trp, uint8_t mcs, uint8_t groupId);

  /**
   * \brief Indicate if the allocation is using frequency hopping
   *
   * \return true if the allocation is using frequency hopping
   */
  bool IsHopping () const;

  /**
   * \brief Get the start of the allocation
   * \return The start of the allocation
   */
  uint8_t GetRbStart () const;

  /**
   * \brief Get the length of the allocation
   * \return The length of the allocation
   */
  uint8_t GetRbLen () const;

  /**
   * \brief Get the frequency hopping configuration
   * \return The frequency hopping configuration
   */
  uint8_t GetHoppingInfo () const;

  /**
   * \brief Get the TRP field
   * \return The TRP field
   */
  uint8_t GetTrp () const;

  /**
   * \brief Get the MCS used for the transmission
   * \return The MCS used for the transmission
   */
  uint8_t GetMcs () const;

  /**
   * \brief Get the destination group ID
   * \return The destination group ID
   */
  uint8_t GetGroupDstId () const;

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  bool   m_hopping; ///< hopping flag
  uint8_t   m_rbStart; ///< models rb assignment
  uint8_t   m_rbLen;   ///< models rb assignment
  uint8_t   m_hoppingInfo; ///< models rb assignment when hopping is enabled
  uint8_t   m_trp; ///< Time resourse pattern (TRP)
  uint8_t   m_mcs; ///< MCS
  uint8_t   m_groupDstId; ///< Group destination ID
};


} // namespace ns3

#endif // LTE_SL_HEADER_H
