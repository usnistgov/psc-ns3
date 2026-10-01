// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_RLC_TM_H
#define NR_SL_RLC_TM_H

#include "nr-sl-rlc.h"

#include "ns3/nr-rlc-sequence-number.h"
#include <ns3/event-id.h>

#include <map>
#include <vector>

namespace ns3
{

/**
 * LTE RLC Transparent Mode (TM), see 3GPP TS 36.322
 */
class NrSlRlcTm : public NrSlRlc
{
  public:
    NrSlRlcTm();
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    void DoTransmitPdcpPdu(Ptr<Packet> p) override;
    void DoNotifyTxOpportunity(NrMacSapUser::TxOpportunityParameters txOpParams) override;
    void DoNotifyHarqDeliveryFailure() override;
    void DoReceivePdu(NrMacSapUser::ReceivePduParameters rxPduParams) override;

  protected:
    void DoTransmitNrSlPdcpPdu(
        const NrSlRlcSapProvider::NrSlTransmitPdcpPduParameters& params) override;
    void DoNotifyNrSlTxOpportunity(
        const NrSlMacSapUser::NrSlTxOpportunityParameters& params) override;
    void DoReceiveNrSlRlcPdu(NrSlMacSapUser::NrSlReceiveRlcPduParameters rxPduParams) override;
};

} // namespace ns3

#endif // NR_SL_RLC_TM_H
