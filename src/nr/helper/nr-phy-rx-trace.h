// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2015, NYU WIRELESS, Tandon School of Engineering, New York University
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef SRC_NR_HELPER_NR_PHY_RX_TRACE_H_
#define SRC_NR_HELPER_NR_PHY_RX_TRACE_H_

#include <ns3/nr-control-messages.h>
#include <ns3/nr-phy-mac-common.h>
#include <ns3/nr-spectrum-phy.h>
#include <ns3/object.h>
#include <ns3/spectrum-phy.h>
#include <ns3/spectrum-value.h>

#include <fstream>
#include <iostream>

namespace ns3
{

class NrPhyRxTrace : public Object
{
  public:
    NrPhyRxTrace();
    ~NrPhyRxTrace() override;
    static TypeId GetTypeId();

    /**
     * \brief Set simTag that will be concatenated to
     * output file names
     * \param simTag string to be used as simulation tag
     */
    void SetSimTag(const std::string& simTag);

    /**
     * \brief Set results folder
     * \param resultsFolder string to be used as a path to results folder
     */
    void SetResultsFolder(const std::string& resultsFolder);

    /**
     * \brief Trace sink for DL Average SINR of DATA (in dB).
     * \param [in] phyStats NrPhyRxTrace object
     * \param [in] path context path
     * \param [in] cellId the cell ID
     * \param [in] rnti the RNTI
     * \param [in] avgSinr the average SINR
     * \param [in] bwpId the BWP ID
     */
    static void DlDataSinrCallback(Ptr<NrPhyRxTrace> phyStats,
                                   std::string path,
                                   uint16_t cellId,
                                   uint16_t rnti,
                                   double avgSinr,
                                   uint16_t bwpId);

    /**
     * \brief Trace sink for DL Average SINR of CTRL (in dB).
     * \param [in] phyStats NrPhyRxTrace object
     * \param [in] path context path
     * \param [in] cellId the cell ID
     * \param [in] rnti the RNTI
     * \param [in] avgSinr the average SINR
     * \param [in] bwpId the BWP ID
     */
    static void DlCtrlSinrCallback(Ptr<NrPhyRxTrace> phyStats,
                                   std::string path,
                                   uint16_t cellId,
                                   uint16_t rnti,
                                   double avgSinr,
                                   uint16_t bwpId);

    static void UlSinrTraceCallback(Ptr<NrPhyRxTrace> phyStats,
                                    std::string path,
                                    uint64_t imsi,
                                    SpectrumValue& sinr,
                                    SpectrumValue& power);
    static void ReportPacketCountUeCallback(Ptr<NrPhyRxTrace> phyStats,
                                            std::string path,
                                            UePhyPacketCountParameter param);
    static void ReportPacketCountGnbCallback(Ptr<NrPhyRxTrace> phyStats,
                                             std::string path,
                                             GnbPhyPacketCountParameter param);
    static void ReportDownLinkTBSize(Ptr<NrPhyRxTrace> phyStats,
                                     std::string path,
                                     uint64_t imsi,
                                     uint64_t tbSize);
    static void RxPacketTraceUeCallback(Ptr<NrPhyRxTrace> phyStats,
                                        std::string path,
                                        RxPacketTraceParams param);
    static void RxPacketTraceGnbCallback(Ptr<NrPhyRxTrace> phyStats,
                                         std::string path,
                                         RxPacketTraceParams param);

    /**
     *  Trace sink for Gnb Phy Received Control Messages.
     *
     * \param [in] phyStats Physical layer statistics.
     * \param [in] path Path of the file where the traces will be written
     * \param [in] sfn frame, subframe and slot number
     * \param [in] nodeId
     * \param [in] rnti
     * \param [in] bwpId
     * \param [in] pointer to msg to get the msg type
     */
    static void RxedGnbPhyCtrlMsgsCallback(Ptr<NrPhyRxTrace> phyStats,
                                           std::string path,
                                           SfnSf sfn,
                                           uint16_t nodeId,
                                           uint16_t rnti,
                                           uint8_t bwpId,
                                           Ptr<const NrControlMessage> msg);

    /**
     *  Trace sink for Gnb Phy Transmitted Control Messages.
     *
     * \param [in] frame Frame number
     * \param [in] subframe Subframe number
     * \param [in] slot number
     * \param [in] nodeId
     * \param [in] rnti
     * \param [in] bwpId
     * \param [in] pointer to msg to get the msg type
     */
    static void TxedGnbPhyCtrlMsgsCallback(Ptr<NrPhyRxTrace> phyStats,
                                           std::string path,
                                           SfnSf sfn,
                                           uint16_t nodeId,
                                           uint16_t rnti,
                                           uint8_t bwpId,
                                           Ptr<const NrControlMessage> msg);

    /**
     *  Trace sink for Ue Phy Received Control Messages.
     *
     * \param [in] frame Frame number
     * \param [in] subframe Subframe number
     * \param [in] slot number
     * \param [in] nodeId
     * \param [in] rnti
     * \param [in] bwpId
     * \param [in] pointer to msg to get the msg type
     */
    static void RxedUePhyCtrlMsgsCallback(Ptr<NrPhyRxTrace> phyStats,
                                          std::string path,
                                          SfnSf sfn,
                                          uint16_t nodeId,
                                          uint16_t rnti,
                                          uint8_t bwpId,
                                          Ptr<const NrControlMessage> msg);

    /**
     *  Trace sink for Ue Phy Transmitted Control Messages.
     *
     * \param [in] frame Frame number
     * \param [in] subframe Subframe number
     * \param [in] slot number
     * \param [in] nodeId
     * \param [in] rnti
     * \param [in] bwpId
     * \param [in] pointer to msg to get the msg type
     */
    static void TxedUePhyCtrlMsgsCallback(Ptr<NrPhyRxTrace> phyStats,
                                          std::string path,
                                          SfnSf sfn,
                                          uint16_t nodeId,
                                          uint16_t rnti,
                                          uint8_t bwpId,
                                          Ptr<const NrControlMessage> msg);

    /**
     *  Trace sink for Ue Phy Received Control Messages.
     *
     * \param [in] frame Frame number
     * \param [in] subframe Subframe number
     * \param [in] slot number
     * \param [in] nodeId
     * \param [in] rnti
     * \param [in] bwpId
     * \param [in] harq Id
     * \param [in] k1 delay
     */
    static void RxedUePhyDlDciCallback(Ptr<NrPhyRxTrace> phyStats,
                                       std::string path,
                                       SfnSf sfn,
                                       uint16_t nodeId,
                                       uint16_t rnti,
                                       uint8_t bwpId,
                                       uint8_t harqId,
                                       uint32_t k1Delay);
    /**
     *  Trace sink for Ue Phy Received Control Messages.
     *
     * \param [in] phyStats Physical layer statistics
     * \param [in] path Where to write PHY layer statistics
     * \param [in] sfn Frame, subframe, slot number.
     * \param [in] nodeId
     * \param [in] rnti
     * \param [in] bwpId
     * \param [in] harq Id
     * \param [in] k1 delay
     */
    static void TxedUePhyHarqFeedbackCallback(Ptr<NrPhyRxTrace> phyStats,
                                              std::string path,
                                              SfnSf sfn,
                                              uint16_t nodeId,
                                              uint16_t rnti,
                                              uint8_t bwpId,
                                              uint8_t harqId,
                                              uint32_t k1Delay);
    /**
     * \brief Trace sink for spectrum channel pathloss trace
     *
     * \param [in] phyStats Pointer to NrPhyRxTrace API
     * \param [in] path The context of the trace path
     * \param [in] txPhy The TX SpectrumPhy instance
     * \param [in] rxPhy The RX SpectrumPhy instance
     * \param [in] lossDb The loss value in dB
     */
    static void PathlossTraceCallback(Ptr<NrPhyRxTrace> phyStats,
                                      std::string path,
                                      Ptr<const SpectrumPhy> txPhy,
                                      Ptr<const SpectrumPhy> rxPhy,
                                      double lossDb);

    /**
     * \brief Write DL CTRL pathloss values in a file
     * \param [in] phyStats NrPhyRxTrace object
     * \param [in] path context path
     * \param cellId cell ID
     * \param bwpId BWP ID
     * \param ueNodeId UE node ID
     * \param lossdB loss in dB
     */
    static void ReportDlCtrlPathloss(Ptr<NrPhyRxTrace> phyStats,
                                     std::string path,
                                     uint16_t cellId,
                                     uint8_t bwpId,
                                     uint32_t ueNodeId,
                                     double lossDb);

    /**
     * \brief Write DL DATA pathloss values in a file
     * \param [in] phyStats NrPhyRxTrace object
     * \param [in] path context path
     * \param cellId cell ID
     * \param bwpId BWP ID
     * \param ueNodeId UE node ID
     * \param lossdB loss in dB
     * \param cqi the CQI that corresponds to the received signal (calculated based on the received
     * SINR)
     */
    static void ReportDlDataPathloss(Ptr<NrPhyRxTrace> phyStats,
                                     std::string path,
                                     uint16_t cellId,
                                     uint8_t bwpId,
                                     uint32_t ueNodeId,
                                     double lossDb,
                                     uint8_t cqi);

  private:
    void ReportInterferenceTrace(uint64_t imsi, SpectrumValue& sinr);
    void ReportPowerTrace(uint64_t imsi, SpectrumValue& power);
    void ReportPacketCountUe(UePhyPacketCountParameter param);
    void ReportPacketCountGnb(GnbPhyPacketCountParameter param);
    void ReportDLTbSize(uint64_t imsi, uint64_t tbSize);
    /**
     * \brief Write DL pathloss values in a file
     *
     * \param [in] txNrSpectrumPhy The TX NrSpectrumPhy instance
     * \param [in] rxNrSpectrumPhy The RX NrSpectrumPhy instance
     * \param [in] lossDb The loss value in dB
     */
    void WriteDlPathlossTrace(Ptr<NrSpectrumPhy> txNrSpectrumPhy,
                              Ptr<NrSpectrumPhy> rxNrSpectrumPhy,
                              double lossDb);
    /**
     * \brief Write UL pathloss values in a file
     *
     * \param [in] txNrSpectrumPhy The TX NrSpectrumPhy instance
     * \param [in] rxNrSpectrumPhy The RX NrSpectrumPhy instance
     * \param [in] lossDb The loss value in dB
     */
    void WriteUlPathlossTrace(Ptr<NrSpectrumPhy> txNrSpectrumPhy,
                              Ptr<NrSpectrumPhy> rxNrSpectrumPhy,
                              double lossDb);

    static std::string m_simTag;        //!< The `SimTag` attribute.
    static std::string m_resultsFolder; //!< The results folder path

    static std::ofstream m_dlDataSinrFile;
    static std::string m_dlDataSinrFileName;

    static std::ofstream m_dlCtrlSinrFile;
    static std::string m_dlCtrlSinrFileName;

    static std::ofstream m_rxPacketTraceFile;
    static std::string m_rxPacketTraceFilename;

    static std::ofstream m_rxedGnbPhyCtrlMsgsFile;
    static std::string m_rxedGnbPhyCtrlMsgsFileName;
    static std::ofstream m_txedGnbPhyCtrlMsgsFile;
    static std::string m_txedGnbPhyCtrlMsgsFileName;

    static std::ofstream m_rxedUePhyCtrlMsgsFile;
    static std::string m_rxedUePhyCtrlMsgsFileName;
    static std::ofstream m_txedUePhyCtrlMsgsFile;
    static std::string m_txedUePhyCtrlMsgsFileName;
    static std::ofstream m_rxedUePhyDlDciFile;
    static std::string m_rxedUePhyDlDciFileName;
    static std::ofstream m_dlPathlossFile;
    static std::string m_dlPathlossFileName;
    static std::ofstream m_ulPathlossFile;
    static std::string m_ulPathlossFileName;

    static std::ofstream m_dlCtrlPathlossFile;
    static std::string m_dlCtrlPathlossFileName;
    static std::ofstream m_dlDataPathlossFile;
    static std::string m_dlDataPathlossFileName;
};

} /* namespace ns3 */

#endif /* SRC_NR_HELPER_NR_PHY_RX_TRACE_H_ */
