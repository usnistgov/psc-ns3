// Copyright (c) 2021 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_MAC_SCHEDULING_STATS_H_
#define NR_MAC_SCHEDULING_STATS_H_

#include "nr-stats-calculator.h"

#include "ns3/nr-gnb-mac.h"
#include "ns3/nstime.h"
#include "ns3/uinteger.h"

#include <fstream>
#include <string>

namespace ns3
{

/**
 * \ingroup nr
 *
 * Takes care of storing the information generated at MAC layer. Metrics saved are:
 *   - Timestamp (in MilliSeconds)
 *   - Cell id
 *   - BWP id
 *   - IMSI
 *   - RNTI
 *   - Frame number
 *   - Subframe number
 *   - Slot number
 *   - MCS
 *   - Size of transport block
 */
class NrMacSchedulingStats : public NrStatsCalculator
{
  public:
    /**
     * Constructor
     */
    NrMacSchedulingStats();

    /**
     * Destructor
     */
    ~NrMacSchedulingStats() override;

    // Inherited from ns3::Object
    /**
     *  Register this type.
     *  \return The object TypeId.
     */
    static TypeId GetTypeId();

    /**
     * Set the name of the file where the uplink statistics will be stored.
     *
     * \param outputFilename string with the name of the file
     */
    void SetUlOutputFilename(std::string outputFilename);

    /**
     * Get the name of the file where the uplink statistics will be stored.
     * @return the name of the file where the uplink statistics will be stored
     */
    std::string GetUlOutputFilename();

    /**
     * Set the name of the file where the downlink statistics will be stored.
     *
     * \param outputFilename string with the name of the file
     */
    void SetDlOutputFilename(std::string outputFilename);

    /**
     * Get the name of the file where the downlink statistics will be stored.
     * @return the name of the file where the downlink statistics will be stored
     */
    std::string GetDlOutputFilename();

    /**
     * Notifies the stats calculator that an downlink scheduling has occurred.
     * \param cellId Cell ID of the attached gNb
     * \param imsi IMSI of the scheduled UE
     * \param traceInfo NrSchedulingCallbackInfo structure containing all downlink
     *        information that is generated when DlScheduling trace is fired
     */
    void DlScheduling(uint16_t cellId, uint64_t imsi, const NrSchedulingCallbackInfo& traceInfo);

    /**
     * Notifies the stats calculator that an uplink scheduling has occurred.
     * \param cellId Cell ID of the attached gNB
     * \param imsi IMSI of the scheduled UE
     * \param traceInfo NrSchedulingCallbackInfo structure containing all uplink
     *        information that is generated when DlScheduling trace is fired
     */
    void UlScheduling(uint16_t cellId, uint64_t imsi, const NrSchedulingCallbackInfo& traceInfo);

    /**
     * Trace sink for the ns3::NrGnbMac::DlScheduling trace source
     *
     * \param macStats
     * \param path
     * \param traceInfo NrSchedulingCallbackInfo structure containing all downlink
     *        information that is generated when DlScheduling trace is fired
     */
    static void DlSchedulingCallback(Ptr<NrMacSchedulingStats> macStats,
                                     std::string path,
                                     NrSchedulingCallbackInfo traceInfo);

    /**
     * Trace sink for the ns3::NrGnbMac::UlScheduling trace source
     *
     * \param macStats the pointer to the MAC stats
     * \param path the trace source path
     * \param traceInfo - all the traces information in a single structure
     */
    static void UlSchedulingCallback(Ptr<NrMacSchedulingStats> macStats,
                                     std::string path,
                                     NrSchedulingCallbackInfo traceInfo);

  private:
    /**
     * DL MAC statistics file stream. When the filename
     * is changed, columns description are added. Then
     * next lines are appended to file.
     */
    std::ofstream outDlFile;
    /**
     * UL MAC statistics file stream. When the filename
     * is changed, columns description are added. Then
     * next lines are appended to file.
     */
    std::ofstream outUlFile;
};

} // namespace ns3

#endif /* NR_MAC_SCHEDULING_STATS_H_ */
