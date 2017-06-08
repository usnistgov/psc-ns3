/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 TELEMATICS LAB, DEE - Politecnico di Bari
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Giuseppe Piro  <g.piro@poliba.it>
 * Author: Marco Miozzo <marco.miozzo@cttc.es>
 * Modified by: NIST
 */

#ifndef NIST_LTE_CONTROL_MESSAGES_H
#define NIST_LTE_CONTROL_MESSAGES_H

#include <ns3/ptr.h>
#include <ns3/simple-ref-count.h>
#include <ns3/nist-ff-mac-common.h>
#include <ns3/nist-lte-rrc-sap.h>
#include <list>

namespace ns3 {

class NistLteNetDevice;


/**
 * \ingroup lte
 *
 * The NistLteControlMessage provides a basic implementations for
 * control messages (such as PDCCH allocation map, CQI feedbacks)
 * that are exchanged among eNodeB and UEs.
 */
class NistLteControlMessage : public SimpleRefCount<NistLteControlMessage>
{
public:
  /**
   * The type of the message
   * NOTE: The messages sent by UE are filtered by the
   *  NistLteEnbPhy::ReceiveNistLteControlMessageList in order to remove the ones 
   *  that has been already handoff by the eNB for avoiding propagation of
   *  spurious messages. When new messaged have to been added, consider to
   *  update the switch statement implementing teh filtering.
   */
  enum MessageType
  {
    DL_DCI, UL_DCI, // Downlink/Uplink Data Control Indicator
    DL_CQI, UL_CQI, // Downlink/Uplink Channel Quality Indicator
    BSR, // Buffer NistStatus Report, including sidelink bsr
    DL_HARQ, // UL HARQ feedback
    RACH_PREAMBLE, // Random Access Preamble
    RAR, // Random Access Response
    MIB, // Master Information Block
    SIB1, // System Information Block Type 1
    SL_DCI, SCI, //Sidelink Data Control Indicator and Sidelink Control Information
    MIB_SL, // Master Information Block Sidelink
    SL_DISC_MSG // sidelink dicovery message
  };

  NistLteControlMessage (void);
  virtual ~NistLteControlMessage (void);

  /**
   * \brief Set the type of the message
   * \param type the type of the message
   */
  void SetMessageType (MessageType type);
  /**
   * \brief Get the type of the message
   * \return the type of the message
   */
  MessageType GetMessageType (void);

private:
  MessageType m_type;
};


// -----------------------------------------------------------------------

/**
 * \ingroup lte
 * The Downlink Data Control Indicator messages defines the RB allocation for the
 * users in the downlink
 */
class NistDlDciLteControlMessage : public NistLteControlMessage
{
public:
  NistDlDciLteControlMessage (void);
  virtual ~NistDlDciLteControlMessage (void);

  /**
  * \brief add a DCI into the message
  * \param dci the dci
  */
  void SetDci (NistDlDciListElement_s dci);

  /**
  * \brief Get dic informations
  * \return dci messages
  */
  NistDlDciListElement_s GetDci (void);

private:
  NistDlDciListElement_s m_dci;
};


// ---------------------------------------------------------------------------

/**
 * \ingroup lte
 * The Uplink Data Control Indicator messages defines the RB allocation for the
 * users in the uplink
 */
class NistUlDciLteControlMessage : public NistLteControlMessage
{
public:
  NistUlDciLteControlMessage (void);
  virtual ~NistUlDciLteControlMessage (void);

  /**
  * \brief add a DCI into the message
  * \param dci the dci
  */
  void SetDci (NistUlDciListElement_s dci);

  /**
  * \brief Get dic informations
  * \return dci messages
  */
  NistUlDciListElement_s GetDci (void);

private:
  NistUlDciListElement_s m_dci;
};


// ---------------------------------------------------------------------------

/**
 * \ingroup lte
 * The Sidelink Data Control Indicator messages defines the RB allocation for the
 * users in the sidelink
 */
class NistSlDciLteControlMessage : public NistLteControlMessage
{
public:
  NistSlDciLteControlMessage (void);
  virtual ~NistSlDciLteControlMessage (void);

  /**
  * \brief add a DCI into the message
  * \param dci the dci
  */
  void SetDci (NistSlDciListElement_s dci);

  /**
  * \brief Get dic informations
  * \return dci messages
  */
  NistSlDciListElement_s GetDci (void);

private:
  NistSlDciListElement_s m_dci;
};


// ---------------------------------------------------------------------------

/**
 * \ingroup lte
 * The downlink CqiNistLteControlMessage defines an ideal list of
 * feedback about the channel quality sent by the UE to the eNodeB.
 */
class NistDlCqiLteControlMessage : public NistLteControlMessage
{
public:
  NistDlCqiLteControlMessage (void);
  virtual ~NistDlCqiLteControlMessage (void);

  /**
  * \brief add a DL-CQI feedback record into the message.
  * \param dlcqi the DL cqi feedback
  */
  void SetDlCqi (NistCqiListElement_s dlcqi);

  /**
  * \brief Get DL cqi informations
  * \return dlcqi messages
  */
  NistCqiListElement_s GetDlCqi (void);

private:
  NistCqiListElement_s m_dlCqi;
};


// ---------------------------------------------------------------------------

/**
 * \ingroup lte
 * The uplink NistBsrLteControlMessage defines the specific
 * extension of the CE element for reporting the buffer status report
 */
class NistBsrLteControlMessage : public NistLteControlMessage
{
public:
  NistBsrLteControlMessage (void);
  virtual ~NistBsrLteControlMessage (void);

  /**
  * \brief add a BSR feedback record into the message.
  * \param bsr the BSR feedback
  */
  void SetBsr (NistMacCeListElement_s bsr);

  /**
  * \brief Get BSR informations
  * \return BSR message
  */
  NistMacCeListElement_s GetBsr (void);

private:
  NistMacCeListElement_s m_bsr;

};


// ---------------------------------------------------------------------------

/**
 * \ingroup lte
 * The downlink NistDlHarqFeedbackLteControlMessage defines the specific
 * messages for transmitting the DL HARQ feedback through PUCCH
 */
class NistDlHarqFeedbackLteControlMessage : public NistLteControlMessage
{
public:
  NistDlHarqFeedbackLteControlMessage (void);
  virtual ~NistDlHarqFeedbackLteControlMessage (void);

  /**
  * \brief add a DL HARQ feedback record into the message.
  * \param m the DL HARQ feedback
  */
  void SetDlHarqFeedback (NistDlInfoListElement_s m);

  /**
  * \brief Get DL HARQ informations
  * \return DL HARQ message
  */
  NistDlInfoListElement_s GetDlHarqFeedback (void);

private:
  NistDlInfoListElement_s m_dlInfoListElement;

};


// ---------------------------------------------------------------------------

/**
 * \ingroup lte
 *
 * abstract model for the Random Access Preamble
 */
class NistRachPreambleLteControlMessage : public NistLteControlMessage
{
public:
  NistRachPreambleLteControlMessage (void);
  
  /** 
   * Set the Random Access Preamble Identifier (RAPID), see 3GPP TS 36.321 6.2.2
   *
   * \param rapid the RAPID
   */
  void SetRapId (uint32_t rapid);
  
  /** 
   * 
   * \return the RAPID
   */
  uint32_t GetRapId () const;

private:
  uint32_t m_rapId;

};


// ---------------------------------------------------------------------------

/**
 * \ingroup lte
 *
 * abstract model for the MAC Random Access Response message
 */
class NistRarLteControlMessage : public NistLteControlMessage
{
public:
  NistRarLteControlMessage (void);

  /** 
   * 
   * \param raRnti the RA-RNTI, see 3GPP TS 36.321 5.1.4
   */
  void SetRaRnti (uint16_t raRnti);

  /** 
   * 
   * \return  the RA-RNTI, see 3GPP TS 36.321 5.1.4
   */
  uint16_t GetRaRnti () const;

  /**
   * a MAC RAR and the corresponding RAPID subheader 
   * 
   */
  struct NistRar
  {
    uint8_t rapId;
    NistBuildNistRarListElement_s rarPayload;
  };

  /** 
   * add a RAR to the MAC PDU, see 3GPP TS 36.321 6.2.3
   * 
   * \param rar the rar
   */
  void AddNistRar (NistRar rar);

  /** 
   * 
   * \return a const iterator to the beginning of the RAR list
   */
  std::list<NistRar>::const_iterator NistRarListBegin () const;
  
  /** 
   * 
   * \return a const iterator to the end of the RAR list
   */
  std::list<NistRar>::const_iterator NistRarListEnd () const;

private:
  std::list<NistRar> m_rarList;
  uint16_t m_raRnti;

};


// ---------------------------------------------------------------------------

/**
 * \ingroup lte
 * \brief Abstract model for broadcasting the Master Information Block (MIB)
 *        within the control channel (BCCH).
 *
 * MIB is transmitted by eNodeB RRC and received by UE RRC at every radio frame,
 * i.e., every 10 milliseconds.
 *
 * \sa NistLteEnbRrc::ConfigureCell, NistLteEnbPhy::StartFrame,
 *     NistLteUeRrc::DoRecvNistMasterInformationBlock
 */
class NistMibLteControlMessage : public NistLteControlMessage
{
public:
  /**
   * \brief Create a new instance of MIB control message.
   */
  NistMibLteControlMessage (void);

  /**
   * \brief Replace the MIB content of this control message.
   * \param mib the desired MIB content
   */
  void SetMib (NistLteRrcSap::NistMasterInformationBlock mib);

  /**
   * \brief Retrieve the MIB content from this control message.
   * \return the current MIB content that this control message holds
   */
  NistLteRrcSap::NistMasterInformationBlock GetMib () const;

private:
  NistLteRrcSap::NistMasterInformationBlock m_mib;

}; // end of class NistMibLteControlMessage


// ---------------------------------------------------------------------------

/**
 * \ingroup lte
 * \brief Abstract model for broadcasting the System Information Block Type 1
 *        (SIB1) within the control channel (BCCH).
 *
 * SIB1 is transmitted by eNodeB RRC and received by UE RRC at the 6th subframe
 * of every odd-numbered radio frame, i.e., every 20 milliseconds.
 *
 * \sa NistLteEnbRrc::SetNistSystemInformationBlockType1, NistLteEnbPhy::StartSubFrame,
 *     NistLteUeRrc::DoRecvNistSystemInformationBlockType1
 */
class NistSib1LteControlMessage : public NistLteControlMessage
{
public:
  /**
   * \brief Create a new instance of SIB1 control message.
   */
  NistSib1LteControlMessage (void);

  /**
   * \brief Replace the SIB1 content of this control message.
   * \param sib1 the desired SIB1 content
   */
  void SetSib1 (NistLteRrcSap::NistSystemInformationBlockType1 sib1);

  /**
   * \brief Retrieve the SIB1 content from this control message.
   * \return the current SIB1 content that this control message holds
   */
  NistLteRrcSap::NistSystemInformationBlockType1 GetSib1 () const;

private:
  NistLteRrcSap::NistSystemInformationBlockType1 m_sib1;

}; // end of class NistSib1LteControlMessage

/**
 * \ingroup lte
 * The Sidelink Control Indicator messages defines the RB allocation for the
 * users in the sidelink shared channel
 */
class SciNistLteControlMessage : public NistLteControlMessage
{
public:
  SciNistLteControlMessage (void);

  /**
  * \brief add a SCI into the message
  * \param sci the sci
  */
  void SetSci (NistSciListElement_s sci);

  /**
  * \brief Get sci informations
  * \return sci messages
  */
  NistSciListElement_s GetSci (void);

private:
  NistSciListElement_s m_sci;
};

/**
 * \ingroup lte
 * Abstract model for broadcasting the Master Information Block Sidelink (MIB-SL)
 * MIB-SL is transmitted by sidelink-enabled UEs (when required) every 40 milliseconds.
 * */
class MibSLNistLteControlMessage : public NistLteControlMessage
{
public:
  MibSLNistLteControlMessage (void);

  /**
  * \brief add a MIB-SL into the message
  * \param mibSL the MIB-SL
  */
  void SetMibSL (NistLteRrcSap::MasterInformationBlockSL mibSL);

  /**
  * \brief Get MIB-SL informations
  * \return mibSL messages
  */
  NistLteRrcSap::MasterInformationBlockSL GetMibSL (void);

private:
  NistLteRrcSap::MasterInformationBlockSL m_mibSL;
};
  
/**
 * \ingroup lte
 * the discovery message 
 *
 */ 
class NistSlDiscMessage: public NistLteControlMessage
{
public:
  NistSlDiscMessage (void);

  /**
   * \brief set discovery message
   * \param discMsg discovery message
   */
  void SetSlDiscMessage (NistSlDiscMsg discMsg);

  /**
   * \brief get discovery message
   * \return discovery message
   */
  NistSlDiscMsg GetSlDiscMessage (void);

private:
NistSlDiscMsg m_discMsg;

};

} // namespace ns3

#endif  // NIST_LTE_CONTROL_MESSAGES_H
