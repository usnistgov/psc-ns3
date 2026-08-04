// SPDX-License-Identifier: NIST-Software
//
// Offline sweep of the NR sidelink EPA error model over
// (widebandSnrDb, tbSizeBytes, MCS, N_sc), writing a CSV that can be
// post-processed to answer two questions:
//
//   (1) Is OLLA's 1-D projection (max MCS whose minimum-capacity N_sc
//       passes target BLER) ever Pareto-dominated by a point at the
//       same or smaller N_sc with lower BLER?
//   (2) Given an (MCS, N_sc) resource budget, is the BLER surface
//       monotone in MCS and non-increasing in N_sc, or are there
//       violations caused by diversity/coding interactions that the
//       1-D iteration misses?
//
// The sweep mirrors the math used inside NrSlOllaMcsController::DoGetGrantParams:
// at fixed total transmit power, the per-allocation per-RB SNR is the
// wideband SNR plus 10*log10(poolRBs / candRBs). Capacity is computed
// from NrSlCommResourcePool::GetTransportBlockSize against a hand-built
// SlotInfo so the program does not require a full pool configuration.
//
// Output CSV columns:
//   widebandSnrDb, tbSizeBytes, mcs, nSubCh, candRbs, perRbSnrDb,
//   numTx, rv, effectiveBler, sci2Bler, capacityOk
//
// tbSizeBytes is the SDU size (the user-facing sweep axis).
// capacityOk reflects whether the PHY TBS covers tbSizeBytes +
// SUBHEADER_BYTES, matching the scheduler's LC commitment.
//
// sci2Bler is the per-transmission SCI-2a decode BLER at the
// candidate's per-RB SINR, using fixed QPSK rate-0 coding
// (sci2Mcs=0). It depends only on (numerology, perRbSnrDb) and is
// the same for every numTx row of a given (snr, tb, mcs, nSubCh).
// Downstream joint-target analyses compose it with effectiveBler
// as P_total = 1 - (1 - sci2Bler) * (1 - effectiveBler).

#include "ns3/core-module.h"
#include "ns3/nr-sl-comm-resource-pool.h"
#include "ns3/nr-sl-epa-error-model.h"
#include "ns3/nr-sl-error-model.h"

#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>

using namespace ns3;

// Mirror of SUBHEADER_SIZE in nr-sl-ue-mac-scheduler-default.cc.
// The scheduler reserves this many bytes per RLC PDU as MAC overhead,
// so a TB can only carry an SDU of size (tbSize - SUBHEADER_BYTES).
constexpr uint32_t SUBHEADER_BYTES = 2;

NS_LOG_COMPONENT_DEFINE("NrProseMcsParetoSweep");

int
main(int argc, char* argv[])
{
    double snrMinDb = -5.0;
    double snrMaxDb = 25.0;
    double snrStepDb = 0.5;
    uint32_t tbSizeMinBytes = 60;
    uint32_t tbSizeMaxBytes = 320;
    uint32_t tbSizeStepBytes = 20;
    uint16_t subChSize = 10;
    uint16_t maxSubCh = 5;
    uint16_t numerology = 0;
    uint8_t mcsTable = 1;
    uint8_t maxNumTx = 3; // selection window supports at most 2 retransmissions
    // SlotInfo defaults aligned with the NR SL pool used by
    // nr-prose-adaptive-mcs.cc: every slot is a PSFCH slot (Feedback1),
    // 14-symbol slot with 2 symbols reserved for PSCCH starting at
    // symbol 1, PSSCH from symbol 3 through symbol 11 (9 PSSCH symbols),
    // PSFCH in the trailing symbols.
    uint16_t numSlPscchRbs = 10;
    uint16_t pscchSymStart = 1;
    uint16_t pscchSymLength = 2;
    uint16_t psschSymStart = 3;
    uint16_t psschSymLength = 9;
    bool hasPsfch = true;
    std::string outFile = "mcs-pareto-sweep.csv";

    CommandLine cmd(__FILE__);
    cmd.AddValue("snrMinDb", "Minimum wideband SNR (dB)", snrMinDb);
    cmd.AddValue("snrMaxDb", "Maximum wideband SNR (dB)", snrMaxDb);
    cmd.AddValue("snrStepDb", "Wideband SNR step (dB)", snrStepDb);
    cmd.AddValue("tbSizeMinBytes", "Minimum TB size (bytes)", tbSizeMinBytes);
    cmd.AddValue("tbSizeMaxBytes", "Maximum TB size (bytes)", tbSizeMaxBytes);
    cmd.AddValue("tbSizeStepBytes", "TB size step (bytes)", tbSizeStepBytes);
    cmd.AddValue("subChSize", "Subchannel size in RBs", subChSize);
    cmd.AddValue("maxSubCh", "Maximum subchannels in the pool", maxSubCh);
    cmd.AddValue("numerology", "Numerology (0 or 1)", numerology);
    cmd.AddValue("mcsTable", "MCS table (1 or 2)", mcsTable);
    cmd.AddValue("maxNumTx", "Maximum transmissions per TB (1..5)", maxNumTx);
    cmd.AddValue("psschSymLength", "PSSCH symbols per slot", psschSymLength);
    cmd.AddValue("hasPsfch", "Whether PSFCH is present in the slot", hasPsfch);
    cmd.AddValue("outFile", "Output CSV path", outFile);
    cmd.Parse(argc, argv);

    NS_ABORT_MSG_IF(mcsTable != 1 && mcsTable != 2, "mcsTable must be 1 or 2");
    NS_ABORT_MSG_IF(numerology > 1, "EPA error model supports numerology 0 or 1");
    NS_ABORT_MSG_IF(maxSubCh == 0, "maxSubCh must be > 0");
    NS_ABORT_MSG_IF(subChSize == 0, "subChSize must be > 0");
    NS_ABORT_MSG_IF(maxNumTx < 1 || maxNumTx > 5, "maxNumTx must be in [1, 5]");
    // IR RV sequence used by the SL HARQ process for up to 4 retx; truncate to maxNumTx
    const std::array<uint8_t, 5> rvSeq{0, 2, 3, 1, 0};

    auto errorModel = CreateObject<NrSlEpaErrorModel>();
    auto pool = CreateObject<NrSlCommResourcePool>();

    NrSlCommResourcePool::SlotInfo slotInfo(numSlPscchRbs,
                                            pscchSymStart,
                                            pscchSymLength,
                                            psschSymStart,
                                            psschSymLength,
                                            hasPsfch,
                                            subChSize,
                                            /*slMaxNumPerReserve*/ 1,
                                            /*absSlotIndex*/ 0,
                                            /*slotOffset*/ 0);

    std::ofstream out(outFile);
    NS_ABORT_MSG_IF(!out.is_open(), "Could not open " << outFile << " for writing");
    out << "widebandSnrDb,tbSizeBytes,mcs,nSubCh,candRbs,perRbSnrDb,"
           "numTx,rv,effectiveBler,sci2Bler,capacityOk\n";
    out << std::fixed;

    const uint32_t poolRbs = static_cast<uint32_t>(maxSubCh) * subChSize;
    const uint8_t maxMcs = 28;

    for (double snrDb = snrMinDb; snrDb <= snrMaxDb + 1e-9; snrDb += snrStepDb)
    {
        for (uint32_t tb = tbSizeMinBytes; tb <= tbSizeMaxBytes; tb += tbSizeStepBytes)
        {
            for (uint8_t mcs = 0; mcs <= maxMcs; mcs++)
            {
                for (uint16_t nSc = 1; nSc <= maxSubCh; nSc++)
                {
                    const uint32_t candRbs = static_cast<uint32_t>(nSc) * subChSize;
                    const uint32_t capBytes =
                        pool->GetTransportBlockSize(slotInfo, mcs, nSc, mcsTable);
                    const bool capacityOk = (capBytes >= tb + SUBHEADER_BYTES);
                    const double perRbSnr =
                        snrDb + 10.0 * std::log10(static_cast<double>(poolRbs) / candRbs);
                    // SCI-2a uses fixed QPSK rate-0 coding (sci2Mcs=0) and does not
                    // soft-combine across retransmissions. BLER depends only on
                    // (numerology, perRbSnr), so it is the same for every numTx row
                    // of this (snr, tb, mcs, nSc) tuple.
                    const double sci2Bler =
                        errorModel->GetSci2ErrorRate(/*sci2Mcs*/ 0,
                                                     static_cast<uint8_t>(numerology),
                                                     perRbSnr);
                    // Walk the IR RV sequence, appending to the history after each
                    // query; the returned tbler is the post-combining BLER.
                    NrErrorModel::NrErrorModelHistory history;
                    for (uint8_t i = 0; i < maxNumTx; i++)
                    {
                        const uint8_t rv = rvSeq[i];
                        const double bler = errorModel->GetTbler(mcs,
                                                                 static_cast<uint8_t>(numerology),
                                                                 rv,
                                                                 perRbSnr,
                                                                 tb,
                                                                 candRbs,
                                                                 history);
                        out << std::setprecision(3) << snrDb << "," << tb << "," << +mcs << ","
                            << nSc << "," << candRbs << "," << std::setprecision(3) << perRbSnr
                            << "," << static_cast<unsigned>(i + 1) << "," << +rv << ","
                            << std::setprecision(6) << bler << "," << sci2Bler << ","
                            << (capacityOk ? 1 : 0) << "\n";
                        history.push_back(Create<NrSlErrorModelOutput>(bler, perRbSnr, rv));
                    }
                }
            }
        }
    }

    out.close();
    std::cout << "Wrote " << outFile << std::endl;
    return 0;
}
