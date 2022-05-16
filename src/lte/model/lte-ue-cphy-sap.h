/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Nicola Baldo <nbaldo@cttc.es>,
 *         Marco Miozzo <mmiozzo@cttc.es>
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#ifndef LTE_UE_CPHY_SAP_H
#define LTE_UE_CPHY_SAP_H

#include <stdint.h>
#include <ns3/ptr.h>

#include <ns3/lte-rrc-sap.h>
#include <ns3/lte-sl-pool.h>

namespace ns3 {


class LteEnbNetDevice;

/**
  * Configuration needed for the timely change of subframe indication upon synchronization to
  * a different SyncRef
  */
struct LteSlSyncParams
{
  SidelinkCommResourcePool::SubframeInfo rxSubframe;  ///< Subframe where MIB was received
  SidelinkCommResourcePool::SubframeInfo newSubframe;  ///< Subframe upon synchronization
  uint16_t slssid;  ///< SLSS ID
  uint16_t offset;  ///< synchronization offset
  LteRrcSap::MasterInformationBlockSL syncRefMib;  ///< MIB
};

/**
 * Service Access Point (SAP) offered by the UE PHY to the UE RRC for control purposes
 *
 * This is the PHY SAP Provider, i.e., the part of the SAP that contains
 * the PHY methods called by the RRC
 */
class LteUeCphySapProvider
{
public:
  /**
   * destructor
   */
  virtual ~LteUeCphySapProvider ();

  /**
   * reset the PHY
   *
   */
  virtual void Reset () = 0;

  /**
   * \brief Tell the PHY entity to listen to PSS from surrounding cells and
   *        measure the RSRP.
   * \param dlEarfcn The downlink carrier frequency (EARFCN) to listen to
   *
   * This function will instruct this PHY instance to listen to the DL channel
   * over the bandwidth of 6 RB at the frequency associated with the given
   * EARFCN.
   *
   * After this, it will start receiving Primary Synchronization Signal (PSS)
   * and periodically returning measurement reports to RRC via
   * LteUeCphySapUser::ReportUeMeasurements function.
   */
  virtual void StartCellSearch (uint32_t dlEarfcn) = 0;

  /**
   * \brief Tell the PHY entity to synchronize with a given eNodeB over the
   *        currently active EARFCN for communication purposes.
   * \param cellId The ID of the eNodeB to synchronize with
   *
   * By synchronizing, the PHY will start receiving various information
   * transmitted by the eNodeB. For instance, when receiving system information,
   * the message will be relayed to RRC via
   * LteUeCphySapUser::RecvMasterInformationBlock and
   * LteUeCphySapUser::RecvSystemInformationBlockType1 functions.
   *
   * Initially, the PHY will be configured to listen to 6 RBs of BCH.
   * LteUeCphySapProvider::SetDlBandwidth can be called afterwards to increase
   * the bandwidth.
   */
  virtual void SynchronizeWithEnb (uint16_t cellId) = 0;

  /**
   * \brief Tell the PHY entity to align to the given EARFCN and synchronize
   *        with a given eNodeB for communication purposes.
   * \param cellId The ID of the eNodeB to synchronize with
   * \param dlEarfcn The downlink carrier frequency (EARFCN)
   *
   * By synchronizing, the PHY will start receiving various information
   * transmitted by the eNodeB. For instance, when receiving system information,
   * the message will be relayed to RRC via
   * LteUeCphySapUser::RecvMasterInformationBlock and
   * LteUeCphySapUser::RecvSystemInformationBlockType1 functions.
   *
   * Initially, the PHY will be configured to listen to 6 RBs of BCH.
   * LteUeCphySapProvider::SetDlBandwidth can be called afterwards to increase
   * the bandwidth.
   */
  virtual void SynchronizeWithEnb (uint16_t cellId, uint32_t dlEarfcn) = 0;

  /**
   * \brief Get PHY cell ID
   * \return cell ID this PHY is synchronized to
   */
  virtual uint16_t GetCellId () = 0;

  /**
   * \brief Get PHY DL EARFCN
   * \return DL EARFCN this PHY is synchronized to
   */
  virtual uint32_t GetDlEarfcn () = 0;

  /**
   * \param dlBandwidth the DL bandwidth in number of PRBs
   */
  virtual void SetDlBandwidth (uint16_t dlBandwidth) = 0;

  /**
   * \brief Configure uplink (normally done after reception of SIB2)
   *
   * \param ulEarfcn The uplink carrier frequency (EARFCN)
   * \param ulBandwidth The UL bandwidth in number of PRBs
   */
  virtual void ConfigureUplink (uint32_t ulEarfcn, uint16_t ulBandwidth) = 0;

  /**
   * \brief Configure referenceSignalPower
   *
   * \param referenceSignalPower The received from eNB in SIB2
   */
  virtual void ConfigureReferenceSignalPower (int8_t referenceSignalPower) = 0;

  /**
   * \brief Set Rnti function
   *
   * \param rnti The cell-specific UE identifier
   */
  virtual void SetRnti (uint16_t rnti) = 0;

  /**
   * \brief Set transmission mode
   *
   * \param txMode the transmissionMode of the user
   */
  virtual void SetTransmissionMode (uint8_t txMode) = 0;

  /**
   * \brief Set SRS configuration index
   *
   * \param srcCi the SRS configuration index
   */
  virtual void SetSrsConfigurationIndex (uint16_t srcCi) = 0;

  /**
   * \brief Set P_A value for UE power control
   *
   * \param pa the P_A value
   */
  virtual void SetPa (double pa) = 0;

  //Sidelink discovery
  /**
   * set the discovery receiving pools
   * \param pools The receiving pools
   */
  virtual void SetSlDiscRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools) = 0;

  //Sidelink Communication
  /**
   * set the current sidelink transmit pool
   * \param pool The transmission pool
   */
  virtual void SetSlCommTxPool (Ptr<SidelinkTxCommResourcePool> pool) = 0;

  /**
   * set the sidelink receiving pools
   * \param pools The sidelink receiving pools
   */
  virtual void SetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools) = 0;

  /**
   * Remove Sidelink Communication Tx Pool function
   */
  virtual void RemoveSlCommTxPool () = 0;

  /**
   * add a new destination to listen for
   * \param destination The destination (L2 ID) to listen for
   */
  virtual void AddSlDestination (uint32_t destination) = 0;

  /**
   * remove a destination to listen for
   * \param destination The destination that is no longer of interest
   */
  virtual void RemoveSlDestination (uint32_t destination) = 0;

  /**
   * Pass to the PHY entity the SLSSID to be set
   * \param slssid The SLSSID
   */
  virtual void SetSlssId (uint64_t slssid) = 0;
  /**
   * Notify the PHY entity that a SyncRef has been selected and that it should apply
   * the corresponding change of timing when appropriate
   * \param synchParams The parameters containing the information of the selected SyncRef
   */
  virtual void SynchronizeToSyncRef (LteSlSyncParams synchParams) = 0;

  /**
   * \brief Set RSRP filter coefficient.
   *
   * Determines the strength of smoothing effect induced by layer 3
   * filtering of RSRP used for uplink power control in all attached UE.
   * If equals to 0, no layer 3 filtering is applicable.
   *
   * \param rsrpFilterCoefficient value.
   */
  virtual void SetRsrpFilterCoefficient (uint8_t rsrpFilterCoefficient) = 0;

  /**
   * \brief Reset the PHY after radio link failure function
   * It resets the physical layer parameters of the
   * UE after RLF.
   *
   */
  virtual void ResetPhyAfterRlf () = 0;

  /**
   * \brief Reset radio link failure parameters
   *
   * Upon receiving N311 in-sync indications from the UE
   * PHY the UE RRC instructs the UE PHY to reset the
   * RLF parameters so, it can start RLF detection again.
   *
   */
  virtual void ResetRlfParams () = 0;

  /**
   * \brief Start in-sync detection function
   * When T310 timer is started, it indicates that physical layer
   * problems are detected at the UE and the recovery process is
   * started by checking if the radio frames are in-sync for N311
   * consecutive times.
   *
   */
  virtual void StartInSnycDetection () = 0;

  /**
   * \brief A method call by UE RRC to communicate the IMSI to the UE PHY
   * \param imsi the IMSI of the UE
   */
  virtual void SetImsi (uint64_t imsi) = 0;
  /**
   * \brief Tell the PHY entity to measure the SD-RSRP of the sidelink
   *        relay discovery messages.
   *
   * This function will instruct this PHY instance to start reporting
   * SD-RSRP measurements from the relay discovery messages in the sidelink.
   * It will schedule the first report and after this it will periodically
   * return measurement reports to RRC via
   * LteUeCphySapUser::ReportUeSdRsrpMeasurements function.
   */
  virtual void EnableUeSdRsrpMeasurements () = 0;

  /**
   * \brief Tell the PHY entity to stop measuring the SD-RSRP of the sidelink
   *        relay discovery messages if enabled.
   */
  virtual void DisableUeSdRsrpMeasurements () = 0;

};


/**
 * Service Access Point (SAP) offered by the UE PHY to the UE RRC for control purposes
 *
 * This is the CPHY SAP User, i.e., the part of the SAP that contains the RRC
 * methods called by the PHY
*/
class LteUeCphySapUser
{
public:
  /**
   * destructor
   */
  virtual ~LteUeCphySapUser ();


  /**
   * Parameters of the ReportUeMeasurements primitive: RSRP [dBm] and RSRQ [dB]
   * See section 5.1.1 and 5.1.3 of TS 36.214
   */
  struct UeMeasurementsElement
  {
    uint16_t m_cellId; ///< cell ID
    double m_rsrp;  ///< [dBm]
    double m_rsrq;  ///< [dB]
  };

  /// UeMeasurementsParameters structure
  struct UeMeasurementsParameters
  {
    std::vector <struct UeMeasurementsElement> m_ueMeasurementsList; ///< UE measurement list
    uint8_t m_componentCarrierId; ///< component carrier ID
  };

  /**
   * Parameters for reporting S-RSRP measurements to the RRC by the PHY
   */
  struct UeSlssMeasurementsElement
  {
    uint16_t m_slssid; ///< SLSSID of the measured SyncRef
    double m_srsrp;  ///< Measured S-RSRP [dBm]
    uint16_t m_offset; ///< Reception offset
  };

  /**
   * List of SLSS measurements to be reported to the RRC by the PHY
   */
  struct UeSlssMeasurementsParameters
  {
    std::vector <struct UeSlssMeasurementsElement> m_ueSlssMeasurementsList; ///< List of SLSS measurements to be reported to the RRC by the PHY
  };

  /**
   * Parameters used by the PHY to report SD-RSRP measurements to the RRC
   */
  struct UeSdRsrpMeasurementsElement
  {
    uint64_t m_relayId; ///< Id of the measured Relay UE
    uint32_t m_serviceCode; ///< Service code offered by the Relay UE
    double m_sdRsrp;  ///< Measured SD-RSRP [dBm]
  };

  /**
   * Structure to store SD-RSRP measurements to be reported to the RRC by the PHY
   */
  struct UeSdRsrpMeasurementsParameters
  {
    std::vector <struct UeSdRsrpMeasurementsElement> m_ueSdRsrpMeasurementsList; ///< List of SD-RSRP measurements to be reported to the RRC by the PHY
  };

  /**
   * \brief Relay an MIB message from the PHY entity to the RRC layer.
   * 
   * This function is typically called after PHY receives an MIB message over
   * the BCH.
   *
   * \param cellId the ID of the eNodeB where the message originates from
   * \param mib the Master Information Block message.
   */
  virtual void RecvMasterInformationBlock (uint16_t cellId,
                                           LteRrcSap::MasterInformationBlock mib) = 0;

  /**
   * \brief Relay an SIB1 message from the PHY entity to the RRC layer.
   *
   * This function is typically called after PHY receives an SIB1 message over
   * the BCH.
   *
   * \param cellId the ID of the eNodeB where the message originates from
   * \param sib1 the System Information Block Type 1 message
   */
  virtual void RecvSystemInformationBlockType1 (uint16_t cellId,
                                                LteRrcSap::SystemInformationBlockType1 sib1) = 0;

  /**
   * \brief Send a report of RSRP and RSRQ values perceived from PSS by the PHY
   *        entity (after applying layer-1 filtering) to the RRC layer.
   *
   * \param params the structure containing a vector of cellId, RSRP and RSRQ
   */
  virtual void ReportUeMeasurements (UeMeasurementsParameters params) = 0;

  /**
   * \brief Send an out of sync indication to UE RRC.
   *
   * When the number of out-of-sync indications
   * are equal to N310, RRC starts the T310 timer.
   */
  virtual void NotifyOutOfSync () = 0;

  /**
   * \brief Send an in sync indication to UE RRC.
   *
   * When the number of in-sync indications
   * are equal to N311, RRC stops the T310 timer.
   */
  virtual void NotifyInSync () = 0;

  /**
   * \brief Reset the sync indication counter.
   *
   * Resets the sync indication counter of RRC if the Qin or Qout condition
   * is not fulfilled for the number of consecutive frames.
   */
  virtual void ResetSyncIndicationCounter () = 0;
  
  /**
   * \brief Send a report of S-RSRP values perceived from SLSSs by the PHY
   *        entity (after applying layer-1 filtering) to the RRC layer.
   * \param params tThe structure containing a list of
   *        (SyncRef SLSSID, SyncRef offset and S-RSRP value)
   * \param slssid The SLSSID of the evaluated SyncRef if corresponding
   * \param offset The offset of the evaluated SyncRef if corresponding
   */
  virtual void ReportSlssMeasurements (LteUeCphySapUser::UeSlssMeasurementsParameters params, uint64_t slssid, uint16_t offset) = 0;

  /**
   * The PHY indicated to the RRC the current subframe indication
   * \param frameNo The current frameNo
   * \param subFrameNo The current subframeNo
   */
  virtual void ReportSubframeIndication (uint16_t frameNo, uint16_t subFrameNo) = 0;
  
  /**
   * The PHY pass a received MIB-SL to the RRC
   * \param p The packet containing the MIB-SL
   * \param slssid The SLSSID associated with the received packet
   */
  virtual void ReceiveMibSL (Ptr<Packet> p, uint16_t slssid) = 0;
  
  /**
   * Notify the successful change of timing/SyncRef, and store the selected
   * (current) SyncRef information
   * \param params The resynchronization information
   */
  virtual void ReportChangeOfSyncRef (LteSlSyncParams params) = 0;

  /**
   * Report the SD-RSRP measurements of the Relay UEs in proximity made by the PHY
   * entity (after applying layer-1 filtering) to the RRC layer.
   *
   * \param params The structure containing a list of SD-RSRP measurement elements
   *               (SD-RSRP values and corresponding Relay UE Id and Service Code).
   */
  virtual void ReportUeSdRsrpMeasurements (LteUeCphySapUser::UeSdRsrpMeasurementsParameters params) = 0;
};




/**
 * Template for the implementation of the LteUeCphySapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberLteUeCphySapProvider : public LteUeCphySapProvider
{
public:
  /**
   * Constructor
   *
   * \param owner The owner class
   */
  MemberLteUeCphySapProvider (C* owner);

  // inherited from LteUeCphySapProvider
  virtual void Reset ();
  virtual void StartCellSearch (uint32_t dlEarfcn);
  virtual void SynchronizeWithEnb (uint16_t cellId);
  virtual void SynchronizeWithEnb (uint16_t cellId, uint32_t dlEarfcn);
  virtual uint16_t GetCellId ();
  virtual uint32_t GetDlEarfcn ();
  virtual void SetDlBandwidth (uint16_t dlBandwidth);
  virtual void ConfigureUplink (uint32_t ulEarfcn, uint16_t ulBandwidth);
  virtual void ConfigureReferenceSignalPower (int8_t referenceSignalPower);
  virtual void SetRnti (uint16_t rnti);
  virtual void SetTransmissionMode (uint8_t txMode);
  virtual void SetSrsConfigurationIndex (uint16_t srcCi);
  virtual void SetPa (double pa);
  //Sidelink discovery
  virtual void SetSlDiscRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools);
  //Sidelink communication
  virtual void SetSlCommTxPool (Ptr<SidelinkTxCommResourcePool> pool);
  virtual void SetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools);
  virtual void RemoveSlCommTxPool ();
  virtual void AddSlDestination (uint32_t destination);
  virtual void RemoveSlDestination (uint32_t destination);
  virtual void SetSlssId (uint64_t slssid);
  virtual void SynchronizeToSyncRef (LteSlSyncParams synchParams);

  virtual void SetRsrpFilterCoefficient (uint8_t rsrpFilterCoefficient);
  virtual void ResetPhyAfterRlf ();
  virtual void ResetRlfParams ();
  virtual void StartInSnycDetection ();
  virtual void SetImsi (uint64_t imsi);
  virtual void EnableUeSdRsrpMeasurements ();
  virtual void DisableUeSdRsrpMeasurements ();

private:
  MemberLteUeCphySapProvider ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberLteUeCphySapProvider<C>::MemberLteUeCphySapProvider (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberLteUeCphySapProvider<C>::MemberLteUeCphySapProvider ()
{
}

template <class C>
void
MemberLteUeCphySapProvider<C>::Reset ()
{
  m_owner->DoReset ();
}

template <class C>
void
MemberLteUeCphySapProvider<C>::StartCellSearch (uint32_t dlEarfcn)
{
  m_owner->DoStartCellSearch (dlEarfcn);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SynchronizeWithEnb (uint16_t cellId)
{
  m_owner->DoSynchronizeWithEnb (cellId);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SynchronizeWithEnb (uint16_t cellId, uint32_t dlEarfcn)
{
  m_owner->DoSynchronizeWithEnb (cellId, dlEarfcn);
}

template <class C>
uint16_t
MemberLteUeCphySapProvider<C>::GetCellId ()
{
  return m_owner->DoGetCellId ();
}

template <class C>
uint32_t
MemberLteUeCphySapProvider<C>::GetDlEarfcn ()
{
  return m_owner->DoGetDlEarfcn ();
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SetDlBandwidth (uint16_t dlBandwidth)
{
  m_owner->DoSetDlBandwidth (dlBandwidth);
}

template <class C>
void 
MemberLteUeCphySapProvider<C>::ConfigureUplink (uint32_t ulEarfcn, uint16_t ulBandwidth)
{
  m_owner->DoConfigureUplink (ulEarfcn, ulBandwidth);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::ConfigureReferenceSignalPower (int8_t referenceSignalPower)
{
  m_owner->DoConfigureReferenceSignalPower (referenceSignalPower);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SetRnti (uint16_t rnti)
{
  m_owner->DoSetRnti (rnti);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SetTransmissionMode (uint8_t txMode)
{
  m_owner->DoSetTransmissionMode (txMode);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SetSrsConfigurationIndex (uint16_t srcCi)
{
  m_owner->DoSetSrsConfigurationIndex (srcCi);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SetPa (double pa)
{
  m_owner->DoSetPa (pa);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SetRsrpFilterCoefficient (uint8_t rsrpFilterCoefficient)
{
  m_owner->DoSetRsrpFilterCoefficient (rsrpFilterCoefficient);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::ResetPhyAfterRlf ()
{
  m_owner->DoResetPhyAfterRlf ();
}

template <class C>
void MemberLteUeCphySapProvider<C>::ResetRlfParams ()
{
  m_owner->DoResetRlfParams ();
}

template <class C>
void MemberLteUeCphySapProvider<C>::StartInSnycDetection ()
{
  m_owner->DoStartInSnycDetection ();
}

template <class C>
void MemberLteUeCphySapProvider<C>::SetImsi (uint64_t imsi)
{
  m_owner->DoSetImsi (imsi);
}



//Sidelink discovery
template <class C>
void MemberLteUeCphySapProvider<C>::SetSlDiscRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools)
{
  m_owner->DoSetSlDiscRxPools (pools);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::EnableUeSdRsrpMeasurements ()
{
  m_owner->DoEnableUeSdRsrpMeasurements ();
}

template <class C>
void
MemberLteUeCphySapProvider<C>::DisableUeSdRsrpMeasurements ()
{
  m_owner->DoDisableUeSdRsrpMeasurements ();
}

//Sidelink communication
template <class C>
void
MemberLteUeCphySapProvider<C>::SetSlCommTxPool (Ptr<SidelinkTxCommResourcePool> pool)
{
  m_owner->DoSetSlCommTxPool (pool);
}

template <class C>
void MemberLteUeCphySapProvider<C>::SetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools)
{
  m_owner->DoSetSlCommRxPools (pools);
}

template <class C>
void MemberLteUeCphySapProvider<C>::RemoveSlCommTxPool ()
{
  m_owner->DoRemoveSlCommTxPool ();
}

template <class C>
void MemberLteUeCphySapProvider<C>::AddSlDestination (uint32_t destination)
{
  m_owner->DoAddSlDestination (destination);
}

template <class C>
void MemberLteUeCphySapProvider<C>::RemoveSlDestination (uint32_t destination)
{
  m_owner->DoRemoveSlDestination (destination);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SetSlssId (uint64_t slssid)
{
  m_owner->DoSetSlssId (slssid);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SynchronizeToSyncRef (LteSlSyncParams synchParams)
{
  m_owner->DoSynchronizeToSyncRef (synchParams);
}



/**
 * Template for the implementation of the LteUeCphySapUser as a member
 * of an owner class of type C to which all methods are forwarded
 *
 */
template <class C>
class MemberLteUeCphySapUser : public LteUeCphySapUser
{
public:
  /**
   * Constructor
   *
   * \param owner The owner class
   */
  MemberLteUeCphySapUser (C* owner);

  // methods inherited from LteUeCphySapUser go here
  virtual void RecvMasterInformationBlock (uint16_t cellId,
                                           LteRrcSap::MasterInformationBlock mib);
  virtual void RecvSystemInformationBlockType1 (uint16_t cellId,
                                                LteRrcSap::SystemInformationBlockType1 sib1);
  virtual void ReportUeMeasurements (LteUeCphySapUser::UeMeasurementsParameters params);
  virtual void NotifyOutOfSync ();
  virtual void NotifyInSync ();
  virtual void ResetSyncIndicationCounter ();

  virtual void ReportSlssMeasurements (LteUeCphySapUser::UeSlssMeasurementsParameters params,  uint64_t slssid, uint16_t offset);
  virtual void ReportSubframeIndication (uint16_t frameNo, uint16_t subFrameNo);
  virtual void ReceiveMibSL (Ptr<Packet> p, uint16_t slssid);
  virtual void ReportChangeOfSyncRef (LteSlSyncParams params);

  virtual void ReportUeSdRsrpMeasurements (LteUeCphySapUser::UeSdRsrpMeasurementsParameters params);

private:
  MemberLteUeCphySapUser ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberLteUeCphySapUser<C>::MemberLteUeCphySapUser (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberLteUeCphySapUser<C>::MemberLteUeCphySapUser ()
{
}

template <class C>
void
MemberLteUeCphySapUser<C>::RecvMasterInformationBlock (uint16_t cellId,
                                                       LteRrcSap::MasterInformationBlock mib)
{
  m_owner->DoRecvMasterInformationBlock (cellId, mib);
}

template <class C>
void
MemberLteUeCphySapUser<C>::RecvSystemInformationBlockType1 (uint16_t cellId,
                                                            LteRrcSap::SystemInformationBlockType1 sib1)
{
  m_owner->DoRecvSystemInformationBlockType1 (cellId, sib1);
}

template <class C>
void
MemberLteUeCphySapUser<C>::ReportUeMeasurements (LteUeCphySapUser::UeMeasurementsParameters params)
{
  m_owner->DoReportUeMeasurements (params);
}

template <class C>
void
MemberLteUeCphySapUser<C>::NotifyOutOfSync ()
{
  m_owner->DoNotifyOutOfSync ();
}

template <class C>
void
MemberLteUeCphySapUser<C>::NotifyInSync ()
{
  m_owner->DoNotifyInSync ();
}

template <class C>
void
MemberLteUeCphySapUser<C>::ResetSyncIndicationCounter ()
{
  m_owner->DoResetSyncIndicationCounter ();
}

template <class C>
void
MemberLteUeCphySapUser<C>::ReportSlssMeasurements (LteUeCphySapUser::UeSlssMeasurementsParameters params,  uint64_t slssid, uint16_t offset)
{
  m_owner->DoReportSlssMeasurements (params,  slssid, offset);
}

template <class C>
void
MemberLteUeCphySapUser<C>::ReportSubframeIndication (uint16_t frameNo, uint16_t subFrameNo)
{
  m_owner->DoReportSubframeIndication (frameNo, subFrameNo);
}

template <class C>
void
MemberLteUeCphySapUser<C>::ReceiveMibSL (Ptr<Packet> p, uint16_t slssid)
{
  m_owner->DoReceiveMibSL (p, slssid);
}

template <class C>
void
MemberLteUeCphySapUser<C>::ReportChangeOfSyncRef (LteSlSyncParams params)
{
  m_owner->DoReportChangeOfSyncRef (params);
}

template <class C>
void
MemberLteUeCphySapUser<C>::ReportUeSdRsrpMeasurements (LteUeCphySapUser::UeSdRsrpMeasurementsParameters params)
{
  m_owner->DoReportUeSdRsrpMeasurements (params);
}


} // namespace ns3


#endif // LTE_UE_CPHY_SAP_H
