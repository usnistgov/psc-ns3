// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef SYSTEM_SCHEDULER_TEST_H
#define SYSTEM_SCHEDULER_TEST_H

#include <ns3/ptr.h>
#include <ns3/test.h>

namespace ns3
{

class Packet;

/**
 * \file system-scheduler-test.h
 * \ingroup test
 *
 * \brief This test case checks if the throughput obtained per UE is as expected for
 * the specified scheduling logic.
 * The test scenario consists of a scenario in which various UEs are attached to a single gNB.
 * UEs transmit a fixed amount of packets, at a certain rate, and the test
 * checks that all the packets are delivered correctly. gNB is configured to
 * have 1 bandwidth part. UEs can belong to the same or different beams.
 * This examples uses beam search beamforming method.
 */

/**
 * \ingroup test
 * \brief Main class for testing a scheduler, system-wise
 */
class SystemSchedulerTest : public TestCase
{
  public:
    /**
     * \brief SystemSchedulerTest is a test constructor which is used to initialise
     *        the test parameters.
     * \param name A unique test configuration name
     * \param usersPerBeamNum How many users will be installed per beam
     * \param numOfBeams Into how many beams of gNB will be distributed UEs attached to it.
     *        The maximum for this test case is 4.
     * \param numerology The numerology to be used in the simulation
     * \param bw1 The system bandwidth (Hz)
     * \param isDownlink Is the downlink traffic going to be present in the test case
     * \param isUplink Is the uplink traffic going to be present in the test case
     * \param schedulerType Which scheduler is going to be used in the test case
     *        Ofdma/Tdma" and the scheduling logic RR, PF, of MR
     */
    SystemSchedulerTest(const std::string& name,
                        uint32_t usersPerNumOfBeams,
                        uint32_t numOfBeams,
                        uint32_t numerology,
                        double bw1,
                        bool isDownlink,
                        bool isUplink,
                        const std::string& schedulerType);
    /**
     * \brief ~SystemSchedulerTest
     */
    ~SystemSchedulerTest() override;

  private:
    void DoRun() override;
    void CountPkts(Ptr<const Packet> pkt);

    uint32_t m_numerology;      //!< the numerology to be used
    double m_bw1;               //!< bandwidth of bandwidth part 1
    bool m_isDownlink;          //!< whether to generate the downlink traffic
    bool m_isUplink;            //!< whether to generate the uplink traffic
    uint32_t m_usersPerBeamNum; //!< number of users
    uint32_t m_numOfBeams; //!< currently the test is supposed to work with maximum 4 beams per gNb
    std::string m_schedulerType; //!< Sched type
    std::string m_name;          //!< Name of the test
    uint32_t m_packets{0};       //!< Packets received correctly
    uint32_t m_limit{0}; //!< Total amount of packets, depending on the parameters of the test
};

} // namespace ns3
#endif // SYSTEM_SCHEDULER_TEST_H
