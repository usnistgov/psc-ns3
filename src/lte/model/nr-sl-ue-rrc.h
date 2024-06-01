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

#include "lte-rrc-sap.h"
#include "nr-sl-rrc-sap.h"

#include <ns3/object.h>

#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ns3
{

class NrSlDataRadioBearerInfo;
class NrSlSignallingRadioBearerInfo;
class NrSlDiscoveryRadioBearerInfo;

/**
 * \ingroup lte
 * Manages NR Sidelink information for this UE
 */
class NrSlUeRrc : public Object
{
    /// allow MemberNrSlUeRrcSapUser<LteUeRrc> class friend access
    friend class MemberNrSlUeRrcSapUser<NrSlUeRrc>;

  public:
    NrSlUeRrc();
    ~NrSlUeRrc() override;

    /// inherited from Object
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
     * \brief Get the pointer for the NR sidelink UE RRC SAP User interface
     *        offered to UE RRC by this class
     *
     * \return the pointer of type NrSlUeRrcSapUser
     */
    NrSlUeRrcSapUser* GetNrSlUeRrcSapUser();
    /**
     * \brief Set the pointer for the NR sidelink UE RRC SAP Provider interface
     *        offered to this class by UE RRC class
     *
     * \param s the pointer of type NrSlUeRrcSapProvider
     */
    void SetNrSlUeRrcSapProvider(NrSlUeRrcSapProvider* s);
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
     * \param preconfiguration The NR LteRrcSap::SidelinkPreconfigNr struct
     */
    void SetNrSlPreconfiguration(const LteRrcSap::SidelinkPreconfigNr& preconfiguration);
    /**
     * \brief Set NR sidelink relay discovery/(re)selection configuration for relay UE
     * \param discConfig the NR LteRrcSap::SlRelayUeConfig struct
     */
    void SetNrSlDiscoveryRelayConfiguration(const LteRrcSap::SlRelayUeConfig relayConfig);
    /**
     * \brief Set NR sidelink remote discovery/(re)selection configuration for remote UE
     * \param discConfig the NR LteRrcSap::SlRemoteUeConfig struct
     */
    void SetNrSlDiscoveryRemoteConfiguration(const LteRrcSap::SlRemoteUeConfig remoteConfig);
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

  private:
    // NrSlUeRrcSapUser methods
    /**
     * \brief Get NR sidelink preconfiguration
     *
     * \return The sidelink preconfiguration
     */
    const LteRrcSap::SidelinkPreconfigNr DoGetNrSlPreconfiguration();
    /**
     * \brief Get the physical sidelink pool based on SL bitmap and the TDD pattern
     * \param slBitMap slBitMap The sidelink bitmap
     * \return A vector representing the physical sidelink pool
     */
    const std::vector<std::bitset<1>> DoGetPhysicalSlPool(
        const std::vector<std::bitset<1>>& slBitMap);
    /**
     * \brief Get Bwp Id Container
     * \return The container of SL BWP ids
     */
    const std::set<uint8_t> DoGetBwpIdContainer();
    /**
     * \brief Add NR transmission sidelink data radio bearer
     * \param slTxDrb LteSidelinkRadioBearerInfo pointer
     */
    void DoAddNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb);
    /**
     * \brief Add NR Reception sidelink data radio bearer
     * \param slRxDrb LteSidelinkRadioBearerInfo pointer
     */
    void DoAddNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb);
    /**
     * \brief Get NR Sidelink Tx data radio bearer
     * \param dstL2Id The remote/destination layer 2 id
     * \param lcId The logical channel id
     * \return The NrSlDataRadioBearerInfo
     */
    Ptr<NrSlDataRadioBearerInfo> DoGetSidelinkTxDataRadioBearer(uint32_t dstL2Id, uint8_t lcId);
    /**
     * \brief Get all NR Sidelink Tx data radio bearers to a destination L2 ID
     * \param dstL2Id The remote/destination layer 2 id
     * \return a map containing all of the matching NrSlDataRadioBearerInfo, by logical channel ID
     */
    std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>> DoGetAllSidelinkTxDataRadioBearers(
        uint32_t dstL2Id);
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
     * \brief Get all NR Sidelink data radio bearers from a source L2 Id
     * Returns an empty map if there is no match
     *
     * \param srcL2Id The source layer 2 id
     * \return a map containing all of the matching NrSlDataRadioBearerInfo, by logical channel ID
     */
    std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>> DoGetAllSidelinkRxDataRadioBearers(
        uint32_t srcL2Id);

    /**
     * brief Remove NR Transmission sidelink data radio bearer
     * \param slTxDrb LteSidelinkRadioBearerInfo pointer
     */
    void DoRemoveNrSlTxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slTxDrb);
    /**
     * \brief Remove NR Reception sidelink data radio bearer
     * \param slRxDrb LteSidelinkRadioBearerInfo pointer
     */
    void DoRemoveNrSlRxDataRadioBearer(Ptr<NrSlDataRadioBearerInfo> slRxDrb);

    /**
     * \brief Get Source layer 2 id
     * \return source layer 2 id
     */
    uint32_t DoGetSourceL2Id();
    /**
     * \brief Get next LCID for setting up NR SL DRB towards the given destination
     * \param dstL2Id The destination layer 2 ID
     * \return the next available NR SL DRB LCID
     */
    uint8_t DoGetNextLcid(uint32_t dstL2Id);

    void DoAddTxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb);
    void DoAddRxNrSlSignallingRadioBearer(Ptr<NrSlSignallingRadioBearerInfo> slSrb);
    Ptr<NrSlSignallingRadioBearerInfo> DoGetTxNrSlSignallingRadioBearer(uint32_t dstL2Id,
                                                                        uint8_t lcId);
    void DoAddTxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slTxDiscRb);
    void DoAddRxNrSlDiscoveryRadioBearer(Ptr<NrSlDiscoveryRadioBearerInfo> slRxDiscRb);
    Ptr<NrSlDiscoveryRadioBearerInfo> DoGetTxNrSlDiscoveryRadioBearer(uint32_t dstL2Id);

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
     * \brief Get all NR Sidelink Tx data radio bearers to a given destination L2 Id
     *
     * \param dstL2Id The remote/destination layer 2 id
     *
     * \return A map containing all matching NrSlDataRadioBearerInfo, by logical channel ID
     */
    std::unordered_map<uint8_t, Ptr<NrSlDataRadioBearerInfo>> GetAllSidelinkTxDataRadioBearers(
        uint32_t dstL2Id);

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

    // NR sidelink UE RRC SAP
    NrSlUeRrcSapUser* m_nrSlRrcSapUser{nullptr};           ///< NR SL UE RRC SAP user
    NrSlUeRrcSapProvider* m_nrSlUeRrcSapProvider{nullptr}; ///< NR SL UE RRC SAP provider
    /**
     * Indicates if sidelink is enabled
     */
    bool m_slEnabled{false};
    /**
     * The preconfiguration for out of coverage scenarios
     */
    LteRrcSap::SidelinkPreconfigNr m_preconfiguration;
    std::vector<NrSlUeRrc::LteNrTddSlotType> m_tddPattern; //!< TDD pattern
    uint32_t m_srcL2Id{0};                                 //!< source layer 2 id
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

}; // end of NrSlUeRrc'class

} // namespace ns3

#endif // NR_SL_UE_RRC_H
