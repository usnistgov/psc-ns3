//
// SPDX-License-Identifier: NIST-Software
//

// This example generates block error rate vs. SNR curves for NR sidelink
// error rate models.
//
// Program Options:
//    --table:       Error model table [static]
//    --numerology:  Numerology []
//    --numTx:       Number of transmissions [1]
//
// The output is a gnuplot file with all of the relevant curves of the
// error model.  The 'static' and 'epa' tables print out all MCS curves,
// while the 'static-sci2' and 'epa-sci2' tables print out only a single
// curve (because the performance is the same for all MCS)
//
// The naming convention for files are as follows:
// - static-tbler-#.pdf (# = number of transmissions)
// - static-sci2.pdf
// - epa-#-##.pdf (# = numerology, ## = number of transmissions)
// - epa-sci2-#.pdf (# = numerology)

#include "ns3/core-module.h"
#include "ns3/nr-module.h"
#include "ns3/stats-module.h"

#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrSlErrorModelExample");

double GetErrorRate(std::string table,
                    uint32_t numTx,
                    Ptr<NrSlErrorModel> errorModel,
                    uint8_t mcs,
                    uint8_t numerology,
                    double snrDb,
                    uint32_t numRb);

int
main(int argc, char* argv[])
{
    std::string table{"static"};
    uint32_t numTx{1};
    // The input is a uint16_t so that the default prints out in the command
    // line argument help statements.  Below, it is narrowed to a uint8_t.
    uint16_t numerologyInput{0};
    uint32_t numRb{10}; // Number of RBs
    uint32_t snrMaxDb{0};

    CommandLine cmd(__FILE__);
    cmd.Usage("Example program to generate plots of TBLER vs. SNR");
    cmd.AddValue("table", "Error model table", table);
    cmd.AddValue("numerology", "Numerology", numerologyInput);
    cmd.AddValue("numTx", "Number of transmissions", numTx);
    cmd.Parse(argc, argv);

    NS_ABORT_MSG_IF(numTx < 1, "Error, numTx must be >= 1");
    NS_ABORT_MSG_IF(numerologyInput > 2, "Error, numerology must be <= 2");
    uint8_t numerology = static_cast<uint8_t>(numerologyInput);

    std::ofstream plotfile;
    Gnuplot plot;
    Ptr<NrSlErrorModel> errorModel;
    uint8_t lastMcs{0};
    if (table == "static")
    {
        std::string prefix = "static-tbler-" + std::to_string(numTx);
        plotfile.open(prefix + ".plt", std::ofstream::out);
        plot = Gnuplot(prefix + ".eps");
        errorModel = CreateObject<NrSlStaticErrorModel>();
        plot.SetTitle("AWGN TBLER vs. SNR, numTx=" + std::to_string(numTx));
        plot.SetLegend("SNR (dB)", "TBLER");
        lastMcs = 28;
        snrMaxDb = 30;
    }
    else if (table == "epa")
    {
        std::string prefix =
            "epa-tbler-" + std::to_string(numerology) + "-" + std::to_string(numTx);
        plotfile.open(prefix + ".plt", std::ofstream::out);
        plot = Gnuplot(prefix + ".eps");
        errorModel = CreateObject<NrSlEpaErrorModel>();
        plot.SetTitle("EPA TBLER vs. SNR, numerology=" + std::to_string(numerology) +
                      " numTx=" + std::to_string(numTx));
        plot.SetLegend("SNR (dB)", "TBLER");
        lastMcs = 28;
        snrMaxDb = 70;
    }
    else if (table == "static-sci2")
    {
        plotfile.open("static-sci2.plt", std::ofstream::out);
        plot = Gnuplot("static-sci2.eps");
        errorModel = CreateObject<NrSlStaticErrorModel>();
        plot.SetTitle("AWGN SCI-2 error rate vs. SNR");
        plot.SetLegend("SNR (dB)", "error rate");
        lastMcs = 0;
        snrMaxDb = 30;
    }
    else if (table == "epa-sci2")
    {
        std::string prefix = "epa-sci2-" + std::to_string(numerology);
        plotfile.open(prefix + ".plt", std::ofstream::out);
        plot = Gnuplot(prefix + ".eps");
        errorModel = CreateObject<NrSlEpaErrorModel>();
        plot.SetTitle("EPA SCI-2 vs. SNR, numerology=" + std::to_string(numerology));
        plot.SetLegend("SNR (dB)", "error rate");
        lastMcs = 0;
        snrMaxDb = 70;
    }
    else
    {
        std::cerr << "Error, table type " << table << " unknown" << std::endl;
    }
    for (uint8_t mcs = 0; mcs <= lastMcs; mcs++)
    {
        Gnuplot2dDataset dataset;
        NS_LOG_INFO("Checking MCS " << +mcs);
        // Find left and right edge of data points for this MCS
        // right edge is first observed non-zero value, plus 1 dB
        // left edge is first observed value of 1, minus 1 dB
        double rightEdge = 30;
        double leftEdge = -10;
        double lastErrorRate = 0;
        for (double snrDb = snrMaxDb; snrDb >= -10; snrDb -= 1)
        {
            auto errorRate = GetErrorRate(table, numTx, errorModel, mcs, numerology, snrDb, numRb);
            if (lastErrorRate == 0 && errorRate > 0)
            {
                rightEdge = snrDb + 1;
                NS_LOG_INFO("Setting right edge to " << rightEdge << " dB for mcs " << +mcs);
            }
            else if (lastErrorRate < 1 && errorRate == 1)
            {
                leftEdge = snrDb - 0.5;
                NS_LOG_INFO("Setting left edge to " << leftEdge << " dB for mcs " << +mcs);
            }
            lastErrorRate = errorRate;
        }
        // Now plot at 0.01 dB increments in the region close to the curve edge
        for (double snrDb = rightEdge; snrDb >= rightEdge - 2; snrDb -= 0.001)
        {
            auto errorRate = GetErrorRate(table, numTx, errorModel, mcs, numerology, snrDb, numRb);
            dataset.Add(snrDb, errorRate);
        }
        // Now plot at 0.1 dB increments the rest of the way
        for (double snrDb = rightEdge - 2; snrDb >= leftEdge; snrDb -= 0.1)
        {
            auto errorRate = GetErrorRate(table, numTx, errorModel, mcs, numerology, snrDb, numRb);
            dataset.Add(snrDb, errorRate);
        }
        dataset.SetTitle("MCS " + std::to_string(mcs));
        plot.AddDataset(dataset);
    }

    plot.SetTerminal("postscript eps color enh \"Times-BoldItalic\"");
    std::string xrangeStr = "set xrange [-10:" + std::to_string(snrMaxDb) + "]";
    plot.SetExtra(xrangeStr + "\n\
set logscale y 10\n\
set yrange [0.00001:5]\n\
set format y \"%h\"\n\
set style line 1 linewidth 5\n\
set style line 2 linewidth 5\n\
set style line 3 linewidth 5\n\
set style line 4 linewidth 5\n\
set style line 5 linewidth 5\n\
set style line 6 linewidth 5\n\
set style line 7 linewidth 5\n\
set style line 8 linewidth 5\n\
set style line 9 linewidth 5\n\
set style line 10 linewidth 5\n\
set style line 11 linewidth 5\n\
set style line 12 linewidth 5\n\
set style line 13 linewidth 5\n\
set style line 14 linewidth 5\n\
set style line 15 linewidth 5\n\
set style line 16 linewidth 5\n\
set style increment user");

    plot.GenerateOutput(plotfile);
    plotfile.close();

    return 0;
}

double
GetErrorRate(std::string table,
             uint32_t numTx,
             Ptr<NrSlErrorModel> errorModel,
             uint8_t mcs,
             uint8_t numerology,
             double snrDb,
             uint32_t numRb)
{
    double errorRate{0};
    uint8_t rv{0};
    uint32_t tbSizeUnused{0}; // TB size is unused by these error models
    if (table == "static" || table == "epa")
    {
        NrErrorModel::NrErrorModelHistory history;
        for (uint32_t i = 1; i < numTx; i++)
        {
            history.emplace_back(Create<NrSlErrorModelOutput>(0, snrDb, 0));
        }
        errorRate = errorModel->GetTbler(mcs, numerology, rv, snrDb, tbSizeUnused, numRb, history);
    }
    else if (table == "static-sci2" || table == "epa-sci2")
    {
        errorRate = errorModel->GetSci2ErrorRate(mcs, numerology, snrDb);
    }
    return errorRate;
}
