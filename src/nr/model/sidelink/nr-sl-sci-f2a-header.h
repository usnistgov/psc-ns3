// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_SCI_F2A_HEADER_H
#define NR_SL_SCI_F2A_HEADER_H

#include "nr-sl-sci-f2-header.h"

#include <iostream>

namespace ns3
{

/**
 * \ingroup nr
 * \brief The packet header for the NR Sidelink Control Information (SCI)
 * format 2A (TS 38.212 Sec 8.3 Rel 16). The following includes two fields.
 *
 * - m_castTypeIndicator [2 bits]
 * - m_csiReq [1 bit]
 *
 * The total size of this header is 5 bytes. 4 bytes of \link NrSlSciF2aHeader \endlink
 * , 3 bits of above fields plus 5 bits of zero padding.
 *
 * The use of this header is only possible if:
 * - All the mandatory fields in \link NrSlSciF2aHeader \endlink are set using
 *   their respective setter methods. Otherwise, serialization will hit an assert.
 *
 * \see NrSlSciF2Header
 */
class NrSlSciF2aHeader : public NrSlSciF2Header
{
  public:
    /**
     * \brief Constructor
     *
     * Creates an SCI header
     */
    NrSlSciF2aHeader();
    ~NrSlSciF2aHeader() override;

    /// HARQ feedback enabled/disabled indicator enumeration
    enum HarqFeedbackIndicator_t : uint8_t
    {
        Disabled = 0,
        Enabled = 1
    };

    /// Cast type indicator enumeration
    enum CastTypeIndicator_t : uint8_t
    {
        Broadcast = 0,
        Groupcast = 1,
        Unicast = 2,
        GroupcastOnlyNack = 3,
    };

    /**
     * \brief Set the HARQ feedback enabled/disabled indicator field
     *
     * \param harqFeedbackIndicator  Whether HARQ feedback is enabled
     */
    void SetHarqFeedbackIndicator(uint8_t harqFeedbackIndicator);
    /**
     * \brief Set the cast type indicator field
     *
     * \param castType The cast type indicator value
     */
    void SetCastType(uint8_t castType);
    /**
     * \brief Set the Channel State Information request flag
     *
     * \param csiReq The channel state information request flag
     */
    void SetCsiReq(uint8_t csiReq);

    /**
     * \brief Get the HARQ feedback enabled/disabled indicator field value
     *
     * \return The HARQ feedback enabled/disabled indicator field value
     */
    uint8_t GetHarqFeedbackIndicator() const;
    /**
     * \brief Get the cast type indicator field value
     *
     * \return The cast type indicator value
     */
    uint8_t GetCastType() const;
    /**
     * \brief Get the Channel State Information request flag
     *
     * \return The channel state information request flag
     */
    uint8_t GetCsiReq() const;

    /**
     * \brief Equality operator
     *
     * \param [in] b the NrSlSciF2aHeader to compare
     * \returns \c true if \pname{b} is equal
     */
    bool operator==(const NrSlSciF2aHeader& b) const;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    void Print(std::ostream& os) const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;

  private:
    uint8_t m_harqFeedbackIndicator{Disabled}; //!< Whether HARQ feedback is enabled or disabled
    uint8_t m_castType{Broadcast}; //!< The type of communication this NrSlSciF2aHeader is used for
    uint8_t m_csiReq{0};           //!< The channel state information request flag

    static std::vector<CastTypeIndicator_t>
        m_allowedCastType; //!< Vector of allowed Stage 2 SCI formats, to speed up checking
};

} // namespace ns3

#endif // NR_SL_SCI_F2A_HEADER_H
