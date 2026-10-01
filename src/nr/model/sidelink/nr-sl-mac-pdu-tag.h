// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_MAC_PDU_TAG_H
#define NR_SL_MAC_PDU_TAG_H

#include "ns3/nr-phy-mac-common.h"
#include "ns3/nstime.h"
#include "ns3/packet.h"
#include "ns3/sfnsf.h"

namespace ns3
{

/**
 * \ingroup ue-mac
 *
 * \brief The NrSlMacPduTag class
 */
class NrSlMacPduTag : public Tag
{
  public:
    /**
     * \brief GetTypeId
     * \return the type id object
     */
    static TypeId GetTypeId();

    /**
     * \brief GetInstanceTypeId
     * \return the instance of the object
     */
    TypeId GetInstanceTypeId() const override;

    /**
     * Create an empty MacP PDU tag
     */
    NrSlMacPduTag() = default;

    /**
     * \brief NrSlMacPduTag constructor
     * \param rnti The RNTI
     * \param sfn the SfnSf
     * \param symStart the sym start
     * \param numSym the number of symbol
     * \param tbSize the TB size
     * \param dstL2Id The destination layer 2 id
     */
    NrSlMacPduTag(uint16_t rnti,
                  SfnSf sfn,
                  uint8_t symStart,
                  uint8_t numSym,
                  uint32_t tbSize,
                  uint32_t dstL2Id);

    void Serialize(TagBuffer i) const override;
    void Deserialize(TagBuffer i) override;
    uint32_t GetSerializedSize() const override;
    void Print(std::ostream& os) const override;

    /**
     * \brief Get RNTI
     * \return The RNTI
     */
    uint16_t GetRnti() const;
    /**
     * \brief Set RNTI
     * \param rnti The RNTI
     */
    void SetRnti(uint16_t rnti);

    /**
     * \brief GetSfn
     * \return the SfnSf installed in this object
     */
    SfnSf GetSfn() const;

    /**
     * \brief SetSfn
     * \param sfn the SfnSf to install
     */
    void SetSfn(SfnSf sfn);

    /**
     * \brief GetSymStart
     * \return the symStart variable installed in this object
     */
    uint8_t GetSymStart() const;

    /**
     * \brief GetNumSym
     * \return the numSym variable installed in this object
     */
    uint8_t GetNumSym() const;

    /**
     * \brief SetSymStart
     * \param symStart the symStart value to install
     */
    void SetSymStart(uint8_t symStart);

    /**
     * \brief SetNumSym
     * \param numSym the numSym value to install
     */
    void SetNumSym(uint8_t numSym);

    /**
     * \brief Get Transport Block size
     * \return The Transport Block Size
     */
    uint32_t GetTbSize() const;

    /**
     * \brief Set transport block size
     * \param tbSize The transport block size
     */
    void SetTbSize(uint32_t tbSize);

    /**
     * \brief Get destination layer 2 id
     * \return The destination layer 2 id
     */
    uint32_t GetDstL2Id() const;

    /**
     * \brief Set destination layer 2 id
     * \param dstL2Id The destination layer 2 id
     */
    void SetDstL2Id(uint32_t dstL2Id);

  protected:
    uint16_t m_rnti{0};    //!< RNTI
    SfnSf m_sfnSf;         //!< SfnSf
    uint8_t m_symStart{0}; //!< Symstart
    uint8_t m_numSym{0};   //!< Num sym
    uint32_t m_tbSize{0};  //!< The transport block size
    uint32_t m_dstL2Id{0}; //!< The destination layer 2 id
};

} // namespace ns3

#endif /* NR_SL_MAC_PDU_TAG_H */
