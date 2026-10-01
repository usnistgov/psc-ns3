/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   SPDX-License-Identifier: GPL-2.0-only
 */
#ifndef NR_SL_UE_RRC_H
#define NR_SL_UE_RRC_H

#include "nr-sl-as-sap.h"
#include "nr-sl-pdcp-sap.h"
#include "nr-sl-rrc-sap.h"
#include "nr-sl-ue-bwpm-rrc-sap.h"
#include "nr-sl-ue-cmac-sap.h"
#include "nr-sl-ue-cphy-sap.h"
#include "nr-sl-ue-svc-rrc-sap.h"

#include "ns3/nr-rrc-sap.h"
#include "ns3/nr-ue-rrc.h"
#include <ns3/object.h>
#include <ns3/traced-callback.h>

#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ns3
{

class NrSlDataRadioBearerInfo;
class NrSlSignallingRadioBearerInfo;
class NrSlDiscoveryRadioBearerInfo;
class NrSlMacSapProvider;

/**
 * \ingroup nr
 * Manages NR Sidelink information for this UE
 */
class NrSlUeRrc : public NrUeRrc
{
    /// allow MemberNrSlUeRrcSapUser<NrSlUeRrc> class friend access
    friend class MemberNrSlUeRrcSapUser<NrSlUeRrc>;
    /// allow MemberNrSlAsSapProvider<NrSlUeRrc> class friend access
    friend class MemberNrSlAsSapProvider<NrSlUeRrc>;
    /// allow MemberNrSlUeRrcSapProvider<NrSlUeRrc> class friend access
    friend class MemberNrSlUeRrcSapProvider<NrSlUeRrc>;
    /// allow MemberNrSlUeBwpmRrcSapUser<NrSlUeRrc> class friend access
    friend class MemberNrSlUeBwpmRrcSapUser<NrSlUeRrc>;
    /// allow MemberNrSlUeCmacSapUser<NrSlUeRrc> class friend access
    friend class MemberNrSlUeCmacSapUser<NrSlUeRrc>;
    /// allow MemberNrSlUeCphySapUser<NrSlUeRrc> class friend access
    friend class MemberNrSlUeCphySapUser<NrSlUeRrc>;
    /// allow MemberNrSlPdcpSapUser<NrSlUeRrc> class friend access
    friend class MemberNrSlPdcpSapUser<NrSlUeRrc>;
    /// allow MemberNrSlUeSvcRrcSapProvider<NrSlUeRrc> class friend access
    friend class MemberNrSlUeSvcRrcSapProvider<NrSlUeRrc>;

  public:
    NrSlUeRrc();

    /**
     * Special value to detect an unassigned IMSI
     */
    static const uint64_t IMSI_UNASSIGNED = std::numeric_limits<uint64_t>::max();
    /**
     * Special value to detect an unassigned RNTI
     */
    static const uint16_t RNTI_UNASSIGNED = std::numeric_limits<uint16_t>::max();

    /**
     * TracedCallback signature for packet drop events.
     *
     * \param [in] packet the packet being dropped
     * \param [in] dstL2Id the intended dstL2Id
     * \param [in] lcId the intended lcId
     */
    typedef void (*DropTraceCallback)(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId);

  protected:
    void DoDispose() override;

  public:
    /**
     * \brief Available TDD slot types. Ordering is important.
     */
    enum LteNrTddSlotType : uint8_t
    {
        DL = 0, //!< DL CTRL + DL DATA
        S = 1,  //!< DL CTRL + DL DATA + UL CTRL
        F = 2,  //!< DL CTRL + DL DATA + UL DATA + UL CTRL
        UL = 3, //!< UL DATA + UL CTRL
    };

    /**
     * \brief Stream output operator
     * \param os output stream
     * \param item LteNrTddSlotType whose string equivalent to output
     * \return updated stream
     */
    friend std::ostream& operator<<(std::ostream& os, const LteNrTddSlotType& item);
    /**
     *  Register this type.
     *  \return The object TypeId.
     */
    static TypeId GetTypeId();
    /**
     * Map between logical channel id and data radio bearer for transmissions
     */
    typedef std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>> NrSlDrbMapPerLcId;
    /**
     * Map between L2 id, logical channel id and data radio bearer for transmissions.
     */
    typedef std::unordered_map<uint32_t, NrSlDrbMapPerLcId> NrSlDrbMapPerL2Id;
    /**
     * Map between a pair of L2 id (typically source and destination), logical channel id and data
     * radio bearer for transmissions.
     */
    typedef std::map<std::pair<uint32_t, uint32_t>, NrSlDrbMapPerLcId> NrSlDrbMapPerPairL2Id;
    /**
     * \brief Get the pointer of the UE service layer SAP Provider interface
     *        offered to the service layer by this class
     *
     * \return the pointer of type NrSlUeSvcRrcSapProvider
     */
    NrSlUeSvcRrcSapProvider* GetNrSlUeSvcRrcSapProvider();
    /**
     * \brief Set the pointer for the UE service layer SAP User interface
     *        offered to this class by service layer class
     *
     * \param s the pointer of type NrSlUeSvcRrcSapUser
     */
    void SetNrSlUeSvcRrcSapUser(NrSlUeSvcRrcSapUser* s);

    /**
     * \brief Set the NR Sidelink communication enabled flag
     * \param status True to enable. False to disable
     */
    void SetNrSlEnabled(bool status);
    /**
     * \brief Is NR Sidelink enabled function
     * \return True if NR Sidelink communication is enabled, false otherwise
     */
    bool IsNrSlEnabled();
    /**
     * \brief Set NR Sidelink pre-configuration function
     * \param preconfiguration The NR NrSlRrcSap::SidelinkPreconfigNr struct
     */
    void SetNrSlPreconfiguration(const NrSlRrcSap::SidelinkPreconfigNr& preconfiguration);
    /**
     * \brief Set NR sidelink relay discovery/(re)selection configuration for relay UE
     * \param discConfig the NR NrSlRrcSap::SlRelayUeConfig struct
     */
    void SetNrSlDiscoveryRelayConfiguration(const NrSlRrcSap::SlRelayUeConfig relayConfig);
    /**
     * \brief Set NR sidelink remote discovery/(re)selection configuration for remote UE
     * \param discConfig the NR NrSlRrcSap::SlRemoteUeConfig struct
     */
    void SetNrSlDiscoveryRemoteConfiguration(const NrSlRrcSap::SlRemoteUeConfig remoteConfig);
    /**
     * \brief Set Sidelink source layer 2 id
     *
     * \param srcL2Id The Sidelink layer 2 id of the source
     */
    void SetSourceL2Id(uint32_t srcL2Id);
    /**
     * \brief Store Sidelink BWP id
     *
     * \param bwpId The active sidelink BWP id
     */
    void StoreSlBwpId(uint8_t bwpId);

    /**
     * \brief Convert a string representation of a TDD pattern to a vector of
     *        enum type
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
     *
     * \param tddPattern The TDD pattern specified such as "DL|UL|F|S"
     * \return the vector representation of the TDD pattern
     */
    static std::vector<NrSlUeRrc::LteNrTddSlotType> ConvertTddPattern(std::string tddPattern);

    /**
     * Map between logical channel id and signalling radio bearer
     */
    typedef std::unordered_map<uint8_t, Ptr<NrSlSignallingRadioBearerInfo>> NrSlSrbMapPerLcId;
    /**
     * Map between L2 id, logical channel id and signalling radio bearer
     */
    typedef std::unordered_map<uint32_t, NrSlSrbMapPerLcId> NrSlSrbMapPerL2Id;

    /**
     * Map between destination L2 id and discovery radio bearer
     * if TX: sourceL2Id, DiscoveryBearer
     * if RX: destinationL2Id, DiscoveryBearer
     */
    typedef std::unordered_map<uint32_t, Ptr<NrSlDiscoveryRadioBearerInfo>> NrSlDiscRbMap;

    /**
     * Map between L2 IDs and discovery radio bearer
     * if TX: [destinationL2Id, [sourceL2Id, DiscoveryBearer]]
     * if RX: [sourceL2Id, [destinationL2Id, DiscoveryBearer]]
     */
    typedef std::unordered_map<uint32_t, NrSlDiscRbMap> NrSlDiscoveryRbMapPerL2Id;

    /**
     * \brief Get the physical sidelink pool based on SL bitmap and the TDD pattern
     * \param slBitMap slBitMap The sidelink bitmap
     * \param tddPattern The TDD pattern
     * \return A vector representing the physical sidelink pool
     */
    static std::vector<std::bitset<1>> GetPhysicalSlPool(
        const std::vector<std::bitset<1>>& slBitMap,
        std::vector<NrSlUeRrc::LteNrTddSlotType> tddPattern);

    /**
     * \brief Get all NR Sidelink Tx data radio bearers to a destination L2 ID
     * \param dstL2Id The remote/destination layer 2 id
     * \return a map containing all of the matching NrSlDataRadioBearerInfo, by logical channel ID
     */
    std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>> GetAllSidelinkTxDataRadioBearers(
        uint32_t dstL2Id);

  private:
    /**
     * \brief Add NR transmission sidelink data radio bearer
     * \param slTxDrb NrSlDataRadioBearerInfo pointer
     */
    void AddNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb);
    /**
     * \brief Add NR Reception sidelink data radio bearer
     * \param slRxDrb NrSlDataRadioBearerInfo pointer
     */
    void AddNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb);
    /**
     * \brief Get NR Sidelink Rx data radio bearer
     * Returns a null pointer if there is no match
     *
     * \param srcL2Id The source layer 2 id
     * \param lcId The logical channel id
     * \return The NrSlDataRadioBearerInfo, or a null pointer if there is no match
     */
    Ptr<NrSlDataRadioBearerInfo> DoGetSidelinkRxDataRadioBearer(uint32_t srcL2Id, uint8_t lcId);

    /**
     * brief Remove NR Transmission sidelink data radio bearer
     * \param slTxDrb NrSlDataRadioBearerInfo pointer
     */
    void RemoveNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb);
    /**
     * \brief Remove NR Reception sidelink data radio bearer
     * \param slRxDrb NrSlDataRadioBearerInfo pointer
     */
    void RemoveNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb);

    /**
     * \brief Get next LCID for setting up NR SL DRB towards the given destination
     * \param dstL2Id The destination layer 2 ID
     * \return the next available NR SL DRB LCID
     */
    uint8_t GetNextLcid(uint32_t dstL2Id);

    void AddTxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb);
    void AddRxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb);
    Ptr<NrSlSignallingRadioBearerInfo> GetTxNrSlSignallingRadioBearer(uint32_t dstL2Id,
                                                                      uint8_t lcId);
    void AddTxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slTxDiscRb);
    void AddRxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slRxDiscRb);
    Ptr<NrSlDiscoveryRadioBearerInfo> GetTxNrSlDiscoveryRadioBearer(uint32_t dstL2Id);

    // Class internal private methods and member variables

    /**
     * \brief Get NR Sidelink Tx data radio bearer
     *
     * \param srcL2Id The source layer 2 id
     * \param dstL2Id The remote/destination layer 2 id
     * \param lcId The logical channel id
     *
     * \return The NrSlDataRadioBearerInfo
     */
    Ptr<NrSlDataRadioBearerInfo> GetSidelinkTxDataRadioBearer(uint32_t srcL2Id,
                                                              uint32_t dstL2Id,
                                                              uint8_t lcId);

    /**
     * \brief Get NR Sidelink Rx data radio bearer
     * Returns a null pointer if there is no match
     *
     * \param srcL2Id The source layer 2 id
     * \param dstL2Id The destination layer 2 id
     * \param lcId The logical channel id
     *
     * \return The NrSlDataRadioBearerInfo
     */
    Ptr<NrSlDataRadioBearerInfo> GetSidelinkRxDataRadioBearer(uint32_t srcL2Id,
                                                              uint32_t dstL2Id,
                                                              uint8_t lcid);

    /**
     * \brief Get all NR Sidelink Rx data radio bearers from a given source L2 Id
     * Returns an empty map if there is no match
     *
     * \param srcL2Id The source layer 2 id
     *
     * \return A map containing all matching NrSlDataRadioBearerInfo, by logical channel ID
     */
    std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>> GetAllSidelinkRxDataRadioBearers(
        uint32_t srcL2Id);

    /**
     * Indicates if sidelink is enabled
     */
    bool m_slEnabled{false};
    /**
     * The preconfiguration for out of coverage scenarios
     */
    NrSlRrcSap::SidelinkPreconfigNr m_preconfiguration;
    std::vector<NrSlUeRrc::LteNrTddSlotType> m_tddPattern; //!< TDD pattern

    Time m_signallingPdb; //!< Packet delay budget for signalling LCs */

    // I am using std::set here instead of std::unordered_set for 2 reason:
    // 1. Python bindings does not support std::unordered_set
    // 2. I do not see this container to pass max 2 elements
    std::set<uint8_t> m_slBwpIds;       //!< A container to store SL BWP ids
    NrSlDrbMapPerL2Id m_slTxDrbMap;     /**< NR sidelink data radio bearer map per
                                         * destination layer 2 id. For Group-Cast
                                         * it will only hold the tx bearer info.
                                         * We use another map to store rx bearer
                                         * info.
                                         */
    NrSlDrbMapPerPairL2Id m_slRxDrbMap; /**< NR sidelink rx data radio bearer map
                                         * per source layer 2 id of the sender
                                         * for Group-Cast.
                                         */

    NrSlSrbMapPerL2Id m_slTxSrbMap; /**< NR SL transmission signalling radio
                                     * bearer map per destination layer 2 id.
                                     */
    NrSlSrbMapPerL2Id m_slRxSrbMap; /**< NR SL reception signalling radio
                                     * bearer map per peer (source) layer 2 id.
                                     */

    NrSlDiscoveryRbMapPerL2Id m_slTxDiscoveryRbMap; /**< NR SL transmission discovery radio
                                                     * bearer map per destination layer 2 id.
                                                     */
    NrSlDiscoveryRbMapPerL2Id m_slRxDiscoveryRbMap; /**< NR SL reception discovery radio
                                                     * bearer map per peer (source) layer 2 id.
                                                     */
  public:
    /**
     * Set the AS SAP user to interact with the NAS entity
     *
     * \param s the AS SAP user
     */
    void SetNrSlAsSapUser(NrSlAsSapUser* s);

    /**
     *
     *
     * \return the AS SAP provider exported by this RRC
     */
    NrSlAsSapProvider* GetNrSlAsSapProvider();

    /**
     * \brief Get NR SL BWP Id Container
     *
     * \return The container of SL BWP ids
     */
    const std::set<uint8_t> GetNrSlBwpIdContainer();

    /**
     * \brief Set NR SL BWP Id Container in NR SL UE BWP manager
     */
    void SetNrSlBwpIdContainerInBwpm();

    /**
     * \brief Called by the PDCP entity to notify the RRC entity of the reception
     * of a new NR SL RRC PDU
     *
     * \param params
     */
    void DoReceiveNrSlPdcpSdu(const NrSlPdcpSapUser::NrSlReceivePdcpSduParameters& params);

    /**
     * \brief set the NR Sidelik BWP Manager SAP this RRC should use to
     *        interact with the NR sidelink BWP manager
     *
     * \param s the NR Sidelik UE BWP Manager SAP Provider to be used by this RRC
     */
    void SetNrSlUeBwpmRrcSapProvider(NrSlUeBwpmRrcSapProvider* s);

    /**
     * \brief Get the NR Sidelik BWP Manager SAP offered by this RRC
     *
     * \return s the NR Sidelik BWP Manager SAP User interface offered to the
     *           NR SL UE BWP manager by this RRC
     */
    NrSlUeBwpmRrcSapUser* GetNrSlUeBwpmRrcSapUser();

    /**
     * \brief the NR Sidelik UE Control MAC SAP offered by MAC to RRC
     *
     * \param bwpId The BWP id of the MAC to which the SAP belongs
     * \param s the NR Sidelik UE Control MAC SAP provider interface offered to the
     *          RRC by MAC
     */
    void SetNrSlUeCmacSapProvider(uint8_t bwpId, NrSlUeCmacSapProvider* s);

    /**
     * \brief Get the NR Sidelik UE Control MAC SAP offered by RRC to MAC
     *
     * \return the NR Sidelik UE Control MAC SAP user interface offered by
     *         by RRC to MAC
     */
    NrSlUeCmacSapUser* GetNrSlUeCmacSapUser();

    /**
     * \brief the NR Sidelik UE Control PHY SAP offered by MAC to RRC
     *
     *\param bwpId The BWP id of the PHY to which the SAP belongs
     * \param s the NR Sidelik UE Control PHY SAP provider interface offered to the
     *          RRC by MAC
     */
    void SetNrSlUeCphySapProvider(uint8_t bwpId, NrSlUeCphySapProvider* s);

    /**
     * \brief Get the NR Sidelik UE Control PHY SAP offered by RRC to MAC
     *
     * \return the NR Sidelik UE Control PHY SAP user interface offered by
     *         by RRC to MAC
     */
    NrSlUeCphySapUser* GetNrSlUeCphySapUser();

    /**
     * \brief Get the NR Sidelik SAP offered by RRC to PDCP
     *
     * \return the NR Sidelik UE PHY SAP user interface offered by RRC to PDCP
     */
    NrSlPdcpSapUser* GetNrSlPdcpSapUser();

    /**
     * \brief Set the NR Sidelik SAP offered by PDCP to RRC
     *
     * \param s the NR Sidelik PDCP SAP provider interface offered by PDCO to RLC
     */
    void SetNrSlPdcpSapProvider(NrSlPdcpSapProvider* s);

    /**
     * \brief set the NR SL MAC SAP provider. The UE RRC does not use this
     * directly, but it needs to provide it to newly created RLC instances.
     *
     * \param s the NR SL MAC SAP provider that will be used by all
     * newly created RLC instances
     */
    void SetNrSlMacSapProvider(NrSlMacSapProvider* s);

    /**
     * \brief Get Sidelink source layer 2 id
     *
     * \return srcL2Id The Sidelink layer 2
     */
    uint32_t GetSourceL2Id();

    /**
     *  Tell the PHY to enable SD-RSRP measurements for relay selection
     */
    void EnableUeSdRsrpMeasurements();

    /**
     * Tell the PHY to disable SD-RSRP measurements for relay selection
     */
    void DisableUeSdRsrpMeasurements();

    /**
     *  Tell the PHY to enable SL-RSRP measurements for relay selection
     */
    void EnableUeSlRsrpMeasurements();

    /**
     * Tell the PHY to disable SL-RSRP measurements for relay selection
     */
    void DisableUeSlRsrpMeasurements();

    /**
     * Structure to save RSRP meausrement and its related timestamp
     */
    struct RsrpMeasurement
    {
        double value;
        Time timestamp;
    };

    /**
     * \brief Ask MAC if insterested in receiving packets directed to a given destination L2 id
     *
     * \return true if MAC is interested in the destination L2 id, false otherwise
     */
    bool IsInterested(uint32_t dstL2Id) const;

  private:
    // NR Sidelink AS SAP Provider methods
    /**
     * \brief Activate NR sidelink radio bearer
     *
     * \param isTransmit True if the bearer is for transmission
     * \param isReceive True if the bearer is for reception
     * \param slInfo The SidelinkInfo information
     */
    void DoActivateNrSlRadioBearer(bool isTransmit,
                                   bool isReceive,
                                   const struct SidelinkInfo& slInfo);

    /**
     * \brief Implement the method called bu the ProSe layer to instruct
     *        the RRC to delete the NS SL data bearer
     *
     * \param isTransmit True if the bearer is for transmission
     * \param isReceive True if the bearer is for reception
     * \param slInfo The SidelinkInfo information

     */
    void DoDeleteNrSlDataRadioBearer(bool isTransmit,
                                     bool isReceive,
                                     const struct SidelinkInfo& slInfo);

    /**
     * \brief Send sidelink data packet to RRC.
     *
     * \param packet The packet
     * \param dstL2Id The destination layer 2 id
     * \param lcId The logical channel ID
     */
    void DoSendSidelinkData(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId);

    // Internal private methods and member variables
  private:
    /**
     * \brief Activate NR sidelink data radio bearer
     *
     * \param isTransmit True if the bearer is for transmission
     * \param isReceive True if the bearer is for reception
     * \param slInfo The SidelinkInfo information
     */
    void ActivateNrSlDrb(bool isTransmit, bool isReceive, const struct SidelinkInfo& slInfo);

    /**
     * \brief set out-of-coverage UE RNTI
     *
     * Normally, RNTI is set by UE MAC, however, for SL
     * out-of-coverage case we set it via UE RRC. For uniqueness,
     * it is the lower 16 bits of the IMSI.
     */
    void SetOutofCovrgUeRnti();

    /**
     * \brief Add Nr sidelink receive data radio bearer
     *
     * \param srcL2Id The sidelink source layer 2 id
     * \param lcid The logical channel id
     * \param slInfo The SidelinkInfo information
     * \return The Sidelink radio bearer information
     */
    Ptr<NrSlDataRadioBearerInfo> AddNrSlTxDrb(uint32_t srcL2Id,
                                              uint8_t lcid,
                                              const struct SidelinkInfo& slInfo);

    /**
     * \brief Add Nr sidelink receive data radio bearer
     *
     * \param srcL2Id The sidelink source layer 2 id
     * \param dstL2Id The sidelink destination layer 2 id
     * \param lcid The logical channel id
     * \param castType The type of communication (tx only)
     * \param harqEnabled Whether HARQ is enabled (tx only)
     * \param delayBudget Packet delay budget (tx only)
     * \return The Sidelink radio bearer information
     */
    Ptr<NrSlDataRadioBearerInfo> AddNrSlRxDrb(uint32_t srcL2Id, uint32_t dstL2Id, uint8_t lcid);

    /**
     * \brief Remove NR SIdelink Data Radio Bearer
     *
     * \param isTransmit Whether the bearer to remove is a transmit bearer
     * \param srcL2Id The sidelink source layer 2 id
     * \param dstL2Id The sidelink destination layer 2 id
     * \param lcid The logical channel id
     */
    void RemoveNrSlDataRadioBearer(bool isTransmit,
                                   uint32_t srcL2Id,
                                   uint32_t dstL2Id,
                                   uint8_t lcid);

    /**
     * \brief Populate NR SL Pool to lower layers
     *
     * This methods populates the NR SL pools
     * specifically to MAC and PHY of the UE.
     *
     */
    void PopulateNrSlPools();

    /**
     * \brief Notify Sidelink reception function
     *
     * This function is called by UE MAC to notify UE RRC about the reception of
     * Sidelink communication data upon which UE RRC creates a new Sidelink
     * data radio bearer for reception.
     *
     * \param lcId logical channel id
     * \param srcL2Id source layer 2 id
     * \param dstL2Id destination layer 2 id
     * \param castType Cast type
     * \param harqEnabled whether HARQ was enabled
     */
    void DoNotifySidelinkReception(uint8_t lcId,
                                   uint32_t srcL2Id,
                                   uint32_t dstL2Id,
                                   uint8_t castType,
                                   bool harqEnabled);
    /**
     * \brief Notify that a sidelink HARQ process has timed out
     *
     * \param dstL2Id The destination L2 ID associated with the HARQ process
     */
    void DoNotifySlHarqProcessMaxTxWithNoFeedback(uint32_t dstL2Id);

    /**
     * \brief Finish configuration after adding NR sidelink data radio bearer
     *
     * \param slDrbInfo sidelink data radio bearer information
     * \param lcInfo Logical channel information
     * \return The Sidelink radio bearer information
     */
    Ptr<NrSlDataRadioBearerInfo> FinishSlDrbConfiguration(
        Ptr<NrSlDataRadioBearerInfo> slDrbInfo,
        const NrSlUeCmacSapProvider::SidelinkLogicalChannelInfo& lcInfo);

    /**
     * \brief Notify the RRC that a sidelink connection was released
     *
     * \param srcL2Id source layer 2 id
     * \param dstL2Id destination layer 2 id
     * \param lcId logical channel id
     */
    void DoNotifySidelinkConnectionRelease(uint32_t srcL2Id, uint32_t dstL2Id, uint8_t lcId);

    /**
     * \brief Activate NR sidelink signalling radio bearer (SL-SRB)
     *
     * \param slInfo The SidelinkInfo containing the peer dstL2Id and the
     *        logical channel ID of the bearer to be activated
     */
    void ActivateNrSlSrb(const struct SidelinkInfo& slInfo);

    /**
     * \brief Create and store an NR sidelink signalling radio bearer (SL-SRB)
     *
     * \param srcL2Id The sidelink source layer 2 id
     * \param slInfo The SidelinkInfo containing the peer dstL2Id and the
     *        logical channel ID of the bearer to be activated
     * \return The Sidelink radio bearer information
     */
    Ptr<NrSlSignallingRadioBearerInfo> AddNrSlSrb(uint32_t srcL2Id,
                                                  const struct SidelinkInfo& slInfo);

    // NrSlUeSvcRrcSapProvider methods
    /**
     * \brief Implementation of the method called by the service layer (e.g.,
     *        ProSe layer) asking the RRC layer to instruct lower layers
     *        to monitor messages directed to the layer 2 ID used by this UE
     */
    void DoMonitorSelfL2Id();

    /**
     * \brief Implementation of the method called by the service layer (e.g.,
     *        ProSe layer) asking the RRC layer to instruct lower layers
     *        to monitor messages directed to the specified layer 2 ID
     *
     * \param dstL2Id destination layer 2 ID
     */
    void DoMonitorL2Id(uint32_t dstL2Id);

    /**
     * \brief Implementation of the method called by the service layer (e.g.,
     *        ProSe layer) to instruct the RRC layer to pass an SL signalling
     *        message (e.g., PC5-S message) to lower layers for transmission
     *
     * \param packet the signalling message
     * \param dstL2Id the destination layer 2 ID
     * \param lcId the logical channel ID of the logical channel where the
     *             message should be sent
     */
    void DoSendNrSlSignalling(Ptr<Packet> packet, uint32_t dstL2Id, uint8_t lcId);
    /**
     * \brief Implementation of the method called by the service layer (e.g.,
     *        ProSe layer) to instruct the RRC layer to activate a NR SL
     *        signaling radio bearer (SL-SRB).
     *
     * \param slInfo The SidelinkInfo with the peer layer 2 ID and lcId
     */
    void DoActivateNrSlSignallingRadioBearer(const struct SidelinkInfo& slInfo);

    /**
     * \brief Implementation of the method called by the service layer (e.g.,
     *        ProSe layer) to instruct the RRC layer to pass an SL discovery
     *        message to lower layers for transmission
     *
     * \param packet the discovery message
     * \param dstL2Id the destination layer 2 ID
     */
    void DoSendNrSlDiscoveryMessage(Ptr<Packet> packet, uint32_t dstL2Id);
    /**
     * \brief Implementation of the method called by the service layer (e.g.,
     *        ProSe layer) to instruct the RRC layer to activate an NR SL
     *        discovery radio bearer (SL-SRB).
     *
     * \param dstL2Id the peer layer 2 ID
     */
    void DoActivateNrSlDiscoveryRadioBearer(uint32_t dstL2Id);

    /**
     * \brief Activate NR sidelink discovery radio bearer (SL-SRB)
     *
     * \param dstL2Id the peer layer 2 id
     */
    void ActivateNrSlDiscoveryRb(uint32_t dstL2Id);

    /**
     * \brief Create and store an NR sidelink discovery radio bearer (SL-SRB4)
     *
     * \param srcL2Id The sidelink source layer 2 id
     * \param dstL2Id The sidelink destination layer 2 id
     * \return The Sidelink discovery radio bearer information
     */
    Ptr<NrSlDiscoveryRadioBearerInfo> AddNrSlDiscoveryRb(uint32_t srcL2Id, uint32_t dstL2Id);

    /**
     * \brief Notify reception of SD-RSRP measurements
     *
     * \param l list of SD-RSRP measurements in dBm and correspoding l2Id L2 ID of the peer UE
     */
    void DoReceiveUeSdRsrpMeasurements(NrSlUeCphySapUser::RsrpElementsList l);

    /**
     * \brief Notify reception of SL-RSRP measurements
     *
     * \param l list of SL-RSRP measurements in dBm and correspoding l2Id L2 ID of the peer UE
     */
    void DoReceiveUeSlRsrpMeasurements(NrSlUeCphySapUser::RsrpElementsList l);

    /**
     * \brief Set the filter period from L1 measurement period
     *
     * \param period L1 SD-/SL-RSRP filter period
     *
     */
    void DoSetRsrpFilterPeriod(Time period);

    /**
     * \brief Indicates if SD-RSRP measurements are being collected
     *
     * \returns True, if SD-RSRP measurements are being collected; otherwise,
     *          false
     */
    bool DoIsUeSdRsrpMeasurementsEnabled() const;

    // NR sidelink SAP
    NrSlAsSapProvider* m_nrSlAsSapProvider{nullptr}; ///< NR SL AS SAP provider
    NrSlAsSapUser* m_nrSlAsSapUser{nullptr};         ///< NR SL AS SAP user
    // NrUeRrc<->NrSlUeRrc
    NrSlUeRrcSapProvider* m_nrSlRrcSapProvider{nullptr}; //!< NR SL UE RRC SAP provider
    NrSlUeRrcSapUser* m_nrSlRrcSapUser{nullptr};         ///< NR SL UE RRC SAP user
    // NrUeRrc <-> NrSlUeBwpManager
    NrSlUeBwpmRrcSapProvider* m_nrSlUeBwpmRrcSapProvider{
        nullptr};                                 //!< NR SL UE BWP manager RRC SAP provider
    NrSlUeBwpmRrcSapUser* m_nrSlUeBwpmRrcSapUser; //!< NR SL UE BWP manager RRC SAP user

    std::vector<NrSlUeCmacSapProvider*> m_nrSlUeCmacSapProvider; //!< NR SL UE CMAC SAP provider
    NrSlUeCmacSapUser* m_nrSlUeCmacSapUser;                      //!< NR SL UE CMAC SAP user
    std::vector<NrSlUeCphySapProvider*> m_nrSlUeCphySapProvider; //!< NR SL UE CPHY SAP provider
    NrSlUeCphySapUser* m_nrSlUeCphySapUser;                      //!< NR SL UE CPHY SAP user

    NrSlPdcpSapProvider* m_nrSlPdcpSapProvider{
        nullptr};                       //!< SAP interface to call methods of PDCP instance
    NrSlPdcpSapUser* m_nrSlPdcpSapUser; //!< SAP interface to receive calls from PDCP instance
    NrSlMacSapProvider* m_nrSlMacSapProvider{
        nullptr}; //!< SAP interface to be given to newly created RLC instance of RLC
    uint32_t m_srcL2Id{std::numeric_limits<uint32_t>::max()}; //!< The NR Sidelink Source L2 id;

    NrSlRrcSap::SlRelayUeConfig m_relayConfig;   //!< SlRelayUeConfig
    NrSlRrcSap::SlRemoteUeConfig m_remoteConfig; //!< SlRemoteUeConfig
    std::map<uint32_t, RsrpMeasurement>
        m_sdRsrpMeasurementsMap; //!< SD-RSRP measurements indexed by L2 ID
    std::map<uint32_t, RsrpMeasurement>
        m_slRsrpMeasurementsMap;               //!< SL-RSRP measurements indexed by L2 ID
    Time m_rsrpFilterPeriod;                   //!< L1 measurement period
    bool m_sdUeRsrpMeasurementsEnabled{false}; //!< Indicates if SD-RSRP measurement collection is
                                               //!< enabled

    // NrUeRrc<->Service layer (e.g., NrSlUeProse)
    NrSlUeSvcRrcSapProvider* m_nrSlUeSvcRrcSapProvider; //!< SAP interface to receive calls from the
                                                        //!< service layer instance
    NrSlUeSvcRrcSapUser* m_nrSlUeSvcRrcSapUser{
        nullptr}; //!< SAP interface to call methods of the service layer instance
    TracedCallback<Ptr<Packet>, uint32_t, uint8_t> m_dropTrace; //!< drop trace
};

} // namespace ns3

#endif // NR_SL_UE_RRC_H
