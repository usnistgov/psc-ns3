// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "flow-monitor-output-stats.h"

#include <ns3/abort.h>
#include <ns3/flow-monitor-module.h>
#include <ns3/rng-seed-manager.h>

#include <fstream>

namespace ns3
{

FlowMonitorOutputStats::FlowMonitorOutputStats()
{
}

void
FlowMonitorOutputStats::SetDb(SQLiteOutput* db, const std::string& tableName)
{
    m_db = db;
    m_tableName = tableName;

    bool ret;

    ret = db->SpinExec("CREATE TABLE IF NOT EXISTS " + tableName +
                       " ("
                       "FlowId INTEGER NOT NULL, "
                       "TxPackets INTEGER NOT NULL,"
                       "TxBytes INTEGER NOT NULL,"
                       "TxOfferedMbps DOUBLE NOT NULL,"
                       "RxBytes INTEGER NOT NULL,"
                       "ThroughputMbps DOUBLE NOT NULL, "
                       "MeanDelayMs DOUBLE NOT NULL, "
                       "MeanJitterMs DOUBLE NOT NULL, "
                       "RxPackets INTEGER NOT NULL, "
                       "SEED INTEGER NOT NULL,"
                       "RUN INTEGER NOT NULL,"
                       "PRIMARY KEY(FlowId,SEED,RUN)"
                       ");");
    NS_ABORT_UNLESS(ret);

    FlowMonitorOutputStats::DeleteWhere(m_db,
                                        RngSeedManager::GetSeed(),
                                        RngSeedManager::GetRun(),
                                        tableName);
}

void
FlowMonitorOutputStats::Save(const Ptr<FlowMonitor>& monitor,
                             FlowMonitorHelper& flowmonHelper,
                             const std::string& filename)
{
    bool ret;
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer flowStats = monitor->GetFlowStats();

    double averageFlowThroughput = 0.0;
    double averageFlowDelay = 0.0;

    std::ofstream outFile;
    outFile.open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!outFile.is_open())
    {
        std::cerr << "Can't open file " << filename << std::endl;
        return;
    }

    outFile.setf(std::ios_base::fixed);

    for (const auto& flowStat : flowStats)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(flowStat.first);
        std::stringstream protoStream;
        protoStream << (uint16_t)t.protocol;
        if (t.protocol == 6)
        {
            protoStream.str("TCP");
        }
        if (t.protocol == 17)
        {
            protoStream.str("UDP");
        }

        sqlite3_stmt* stmt;

        m_db->SpinPrepare(&stmt, "INSERT INTO " + m_tableName + " VALUES (?,?,?,?,?,?,?,?,?,?,?);");

        // Measure the duration of the flow from sender's perspective
        double rxDuration = flowStat.second.timeLastTxPacket.GetSeconds() -
                            flowStat.second.timeFirstTxPacket.GetSeconds();
        double txOffered = flowStat.second.txBytes * 8.0 / rxDuration / 1000.0 / 1000.0;

        outFile << "Flow " << flowStat.first << " (" << t.sourceAddress << ":" << t.sourcePort
                << " -> " << t.destinationAddress << ":" << t.destinationPort << ") proto "
                << protoStream.str() << "\n";
        outFile << "  Tx Packets: " << flowStat.second.txPackets << "\n";
        outFile << "  Tx Bytes:   " << flowStat.second.txBytes << "\n";
        outFile << "  TxOffered:  " << txOffered << " Mbps\n";
        outFile << "  Rx Bytes:   " << flowStat.second.rxBytes << "\n";

        ret = m_db->Bind(stmt, 1, flowStat.first);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 2, flowStat.second.txPackets);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 3, static_cast<uint32_t>(flowStat.second.txBytes));
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 4, txOffered);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 5, static_cast<uint32_t>(flowStat.second.rxBytes));
        NS_ABORT_UNLESS(ret);

        if (flowStat.second.rxPackets > 0)
        {
            double th = flowStat.second.rxBytes * 8.0 / rxDuration / 1000 / 1000;
            double delay = 1000 * flowStat.second.delaySum.GetSeconds() / flowStat.second.rxPackets;
            double jitter =
                1000 * flowStat.second.jitterSum.GetSeconds() / flowStat.second.rxPackets;

            averageFlowThroughput += th;
            averageFlowDelay += delay;

            ret = m_db->Bind(stmt, 6, th);
            NS_ABORT_UNLESS(ret);
            ret = m_db->Bind(stmt, 7, delay);
            NS_ABORT_UNLESS(ret);
            ret = m_db->Bind(stmt, 8, jitter);
            NS_ABORT_UNLESS(ret);

            outFile << "  Throughput: " << th << " Mbps\n";
            outFile << "  Mean delay:  " << delay << " ms\n";
            outFile << "  Mean jitter:  " << jitter << " ms\n";
        }
        else
        {
            outFile << "  Throughput:  0 Mbps\n";
            outFile << "  Mean delay:  0 ms (NOT VALID)\n";
            outFile << "  Mean jitter: 0 ms (NOT VALID)\n";
            ret = m_db->Bind(stmt, 6, 0.0);
            NS_ABORT_UNLESS(ret);
            ret = m_db->Bind(stmt, 7, 0.0);
            NS_ABORT_UNLESS(ret);
            ret = m_db->Bind(stmt, 8, 0.0);
            NS_ABORT_UNLESS(ret);
        }
        outFile << "  Rx Packets: " << flowStat.second.rxPackets << "\n";
        ret = m_db->Bind(stmt, 9, flowStat.second.rxPackets);
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 10, RngSeedManager::GetSeed());
        NS_ABORT_UNLESS(ret);
        ret = m_db->Bind(stmt, 11, static_cast<uint32_t>(RngSeedManager::GetRun()));
        NS_ABORT_UNLESS(ret);
        ret = m_db->SpinExec(stmt);
        NS_ABORT_UNLESS(ret);
    }

    outFile << "\n\n  Mean flow throughput: " << averageFlowThroughput / flowStats.size() << "\n";
    outFile << "  Mean flow delay: " << averageFlowDelay / flowStats.size() << "\n";

    outFile.close();
}

void
FlowMonitorOutputStats::DeleteWhere(SQLiteOutput* p,
                                    uint32_t seed,
                                    uint32_t run,
                                    const std::string& table)
{
    bool ret;
    sqlite3_stmt* stmt;
    ret = p->SpinPrepare(&stmt, "DELETE FROM \"" + table + "\" WHERE SEED = ? AND RUN = ?;");
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 1, seed);
    NS_ABORT_IF(ret == false);
    ret = p->Bind(stmt, 2, run);

    ret = p->SpinExec(stmt);
    NS_ABORT_IF(ret == false);
}

} // namespace ns3
