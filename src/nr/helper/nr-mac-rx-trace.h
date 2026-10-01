// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef SRC_NR_HELPER_NR_MAC_RX_TRACE_H_
#define SRC_NR_HELPER_NR_MAC_RX_TRACE_H_

#include <ns3/nr-control-messages.h>
#include <ns3/nr-phy-mac-common.h>
#include <ns3/object.h>
#include <ns3/spectrum-value.h>

#include <iostream>

namespace ns3
{

class NrMacRxTrace : public Object
{
  public:
    NrMacRxTrace();
    ~NrMacRxTrace() override;
    static TypeId GetTypeId();

    /**
     *  Trace sink for Gnb Mac Received Control Messages.
     *
     * \param [in] frame Frame number.
     * \param [in] subframe Subframe number.
     * \param [in] slot number.
     * \param [in] VarTti
     * \param [in] nodeId
     * \param [in] rnti
     * \param [in] bwpId
     * \param [in] pointer to msg to get the msg type
     */
    static void RxedGnbMacCtrlMsgsCallback(Ptr<NrMacRxTrace> macStats,
                                           std::string path,
                                           SfnSf sfn,
                                           uint16_t nodeId,
                                           uint16_t rnti,
                                           uint8_t bwpId,
                                           Ptr<const NrControlMessage> msg);

    /**
     *  Trace sink for Gnb Mac Transmitted Control Messages.
     *
     * \param [in] frame Frame number.
     * \param [in] subframe Subframe number.
     * \param [in] slot number.
     * \param [in] VarTti
     * \param [in] nodeId
     * \param [in] rnti
     * \param [in] bwpId
     * \param [in] pointer to msg to get the msg type
     */
    static void TxedGnbMacCtrlMsgsCallback(Ptr<NrMacRxTrace> macStats,
                                           std::string path,
                                           SfnSf sfn,
                                           uint16_t nodeId,
                                           uint16_t rnti,
                                           uint8_t bwpId,
                                           Ptr<const NrControlMessage> msg);

    /**
     *  Trace sink for Ue Mac Received Control Messages.
     *
     * \param [in] frame Frame number.
     * \param [in] subframe Subframe number.
     * \param [in] slot number.
     * \param [in] VarTti
     * \param [in] nodeId
     * \param [in] rnti
     * \param [in] bwpId
     * \param [in] pointer to msg to get the msg type
     */
    static void RxedUeMacCtrlMsgsCallback(Ptr<NrMacRxTrace> macStats,
                                          std::string path,
                                          SfnSf sfn,
                                          uint16_t nodeId,
                                          uint16_t rnti,
                                          uint8_t bwpId,
                                          Ptr<const NrControlMessage> msg);

    /**
     *  Trace sink for Ue Mac Transmitted Control Messages.
     *
     * \param [in] frame Frame number.
     * \param [in] subframe Subframe number.
     * \param [in] slot number.
     * \param [in] VarTti
     * \param [in] nodeId
     * \param [in] rnti
     * \param [in] bwpId
     * \param [in] pointer to msg to get the msg type
     */
    static void TxedUeMacCtrlMsgsCallback(Ptr<NrMacRxTrace> macStats,
                                          std::string path,
                                          SfnSf sfn,
                                          uint16_t nodeId,
                                          uint16_t rnti,
                                          uint8_t bwpId,
                                          Ptr<const NrControlMessage> msg);

  private:
    static std::ofstream m_rxedGnbMacCtrlMsgsFile;
    static std::string m_rxedGnbMacCtrlMsgsFileName;
    static std::ofstream m_txedGnbMacCtrlMsgsFile;
    static std::string m_txedGnbMacCtrlMsgsFileName;

    static std::ofstream m_rxedUeMacCtrlMsgsFile;
    static std::string m_rxedUeMacCtrlMsgsFileName;
    static std::ofstream m_txedUeMacCtrlMsgsFile;
    static std::string m_txedUeMacCtrlMsgsFileName;
};

} /* namespace ns3 */

#endif /* SRC_NR_HELPER_NR_MAC_RX_TRACE_H_ */
