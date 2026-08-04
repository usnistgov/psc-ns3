// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/nr-module.h"
#include "ns3/nr-point-to-point-epc-helper.h"
#include "ns3/point-to-point-helper.h"

/**
 * \file cttc-error-model-comparison.cc
 * \ingroup examples
 * \brief Error model example comparison: TBS for all MCSs.
 *
 * This example allows the user to compare the Transport Block Size that is obtained
 * for each MCS index under different error models (NR and LTE) and different MCS Tables.
 *
 * The NR error model can be set as "--errorModel=ns3::NrEesmCcT1", for HARQ-CC and MCS Table1,
 * while "--errorModel=ns3::NrLteMiErrorModel" configures the LTE error model.
 * For NR, you can choose between different types of error model, which use
 * different tables and different methods to process the HARQ history, e.g.,
 * "--errorModel=ns3::NrEesmIrT1", for HARQ-IR and MCS Table2.
 *
 * There is no deployment scenario configured, the example directly computes the TBS
 * for all MCSs of the configured error model and MCS Table, assuming numerology 4
 * and 100 MHz of channel bandwidth.
 *
 * This simulation prints the output to the terminal, showing for each MCS: 1)
 * the TBS that fits in 1 OFDM symbol (whole bandwidth) and 2) the TBS that fits
 * in 1 OFDM symbol and a single RB.
 *
 * To run the simulation with the default configuration one shall run the
 * following in the command line:
 *
 * ./ns3 run cttc-error-model-comparison
 *
 */

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("CttcErrorModelComparisonExample");

int
main(int argc, char* argv[])
{
    std::string errorModel = "ns3::NrEesmCcT1";

    CommandLine cmd(__FILE__);

    cmd.AddValue("errorModelType",
                 "Error model type: ns3::NrEesmCcT1, ns3::NrEesmCcT2, ns3::NrEesmIrT1, "
                 "ns3::NrEesmIrT2, ns3::NrLteMiErrorModel",
                 errorModel);

    cmd.Parse(argc, argv);

    /*
     * TODO: remove all the instances of SetDefault, NrEesmErrorModel, NrAmc
     */
    Config::SetDefault("ns3::NrAmc::ErrorModelType", TypeIdValue(TypeId::LookupByName(errorModel)));
    Config::SetDefault("ns3::NrAmc::AmcModel", EnumValue(NrAmc::ShannonModel));

    // Compute number of RBs that fit in 100 MHz channel bandwidth with numerology 4 (240 kHz SCS)
    const uint8_t numerology = 4;
    const uint32_t bandwidth = 100e6;
    const uint32_t numRbsInBandwidth = bandwidth / (15e3 * std::pow(2, numerology) * 12);

    Ptr<NrAmc> amc = CreateObject<NrAmc>();
    amc->SetDlMode();

    std::string tbs;
    for (uint32_t mcs = 0; mcs <= amc->GetMaxMcs(); ++mcs)
    {
        uint8_t rank{1};
        std::stringstream ss;
        ss << "\nResults for DL (UL only in NR case): MCS " << mcs << ". TBS in 1 RB: ["
           << amc->CalculateTbSize(mcs, rank, 1) << "] bytes. TBS in 1 sym: ["
           << amc->CalculateTbSize(mcs, rank, numRbsInBandwidth) << "] bytes.";
        tbs += ss.str();
    }

    std::cout << "NUMEROLOGY 4, 100e6 BANDWIDTH, Error Model: ";
    std::cout << errorModel << ". Results: " << std::endl;
    std::cout << tbs << std::endl;

    return 0;
}
