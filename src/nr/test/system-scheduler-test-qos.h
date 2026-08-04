// Copyright (c) 2022 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef SYSTEM_SCHEDULER_TEST_QOS_H
#define SYSTEM_SCHEDULER_TEST_QOS_H

#include <ns3/ptr.h>
#include <ns3/test.h>

namespace ns3
{

class Packet;

/**
 * \file system-scheduler-test-qos.h
 * \ingroup test
 *
 * \brief This test case checks if the throughput obtained is as expected for
 * the QoS scheduling logic.
 *
 * For the testing of the QoS scheduler we consider two different types of traffic,
 * each one assigned a QCI with different priority. Based on this priority, we test
 * if the ratio of the throughput obtained is equal to the ratio of the priorities
 * for the case that the load of the higher priority UEs is in saturation:
 *
 * \f$P=frac{100-P_1}{100-P_2}=frac{Th_1}{Th_2}\f$
 *
 * Notice that for the UL case, due to a restriction of the scheduler for the
 * case of non-GBR QCIs, we consider the default QCI 9 with priority 90. Therefore,
 * in the check we consider hardcoded P_2 = 90.
 *
 * To execute this test suite run:
 *
 *  * \code{.unparsed}
$ ./ns3 run "test-runner --suite=nr-system-test-schedulers-qos"
    \endcode
 *
 */

/**
 * \ingroup test
 * \brief Main class for testing a scheduler, system-wise
 */
class SystemSchedulerTestQos : public TestCase
{
  public:
    /**
     * \brief SystemSchedulerTest is a test constructor which is used to initialise the test
     * parameters. \param name The "normal" scheduler under test \param ueNumPergNb The number of
     * UEs (per gNB for test case 1) \param numerology The numerology to be used in the simulation
     * \param bw1 The system bandwidth (Hz)
     * \param isDownlnk Is the downlink traffic going to be present in the test case
     * \param isUplink Is the uplink traffic going to be present in the test case
     * \param priorityTrafficScenario The type of traffic to be assign to flow with QCI 1
     * \param schedulerType Which scheduler is going to be used in the test case
     *        Ofdma/Tdma" and the scheduling logic "QoS"
     */
    SystemSchedulerTestQos(uint32_t ueNumPergNb,
                           uint32_t numerology,
                           double bw1,
                           bool isDownlink,
                           bool isUplink,
                           double p1,
                           double p2,
                           uint32_t priorityTrafficScenario,
                           const std::string& schedulerType);
    /**
     * \brief ~SystemSchedulerTestQos
     */
    ~SystemSchedulerTestQos() override;

  private:
    void DoRun() override;
    void CountPkts(Ptr<const Packet> pkt);

    uint32_t m_ueNumPergNb;             //!< number of users
    uint32_t m_numerology;              //!< the numerology to be used
    double m_bw1;                       //!< bandwidth of bandwidth part 1
    bool m_isDownlink;                  //!< whether to generate the downlink traffic
    bool m_isUplink;                    //!< whether to generate the uplink traffic
    double m_p1;                        //!< The priority of QCI for Low Lat
    double m_p2;                        //!< The priority of QCI for Voice
    uint32_t m_priorityTrafficScenario; //!< traffic Type (saturation/mediumLoad)
    std::string m_schedulerType;        //!< Sched type
    bool verbose{false};
};

/**
 * \brief The QoS scheduler system test suite
 * \ingroup test
 *
 * This test will check Tdma/Ofdma QoS with:
 *
 * - DL, UL
 * - number of UEs: 2, 4, 6
 * - numerologies: 0
 * - currently the priorities are hardcoded, but the test can be extended to
 *   support additional QCIs
 *
 * \see SystemSchedulerTestQos
 */
class NrSystemTestSchedulerQosSuite : public TestSuite
{
  public:
    /**
     * \brief constructor
     */
    NrSystemTestSchedulerQosSuite();
};

NrSystemTestSchedulerQosSuite::NrSystemTestSchedulerQosSuite()
    : TestSuite("nr-system-test-schedulers-qos", Type::SYSTEM)
{
    std::list<std::string> subdivision = {"Tdma", "Ofdma"};
    std::string type = {"Qos"};
    std::list<std::string> mode = {"DL", "UL"};
    std::list<uint32_t> numUesPerGnbList = {2, 4};
    std::list<uint32_t> numerologies = {0, 1};
    double qciP1 = {20};
    double qciP2 = {68};
    std::list<uint32_t> priorityTrafficScenarioList = {0};

    for (const auto& modeType : mode)
    {
        for (const auto& subType : subdivision)
        {
            for (const auto& num : numerologies)
            {
                for (const auto& uesPerGnb : numUesPerGnbList)
                {
                    for (const auto& priorityTrafficScenario : priorityTrafficScenarioList)
                    {
                        const bool isDl = modeType == "DL" || modeType == "DL_UL";
                        const bool isUl = modeType == "UL" || modeType == "DL_UL";

                        std::stringstream schedName;
                        schedName << "ns3::NrMacScheduler" << subType << type;

                        AddTestCase(new SystemSchedulerTestQos(uesPerGnb,
                                                               num,
                                                               5e6,
                                                               isDl,
                                                               isUl,
                                                               qciP1,
                                                               qciP2,
                                                               priorityTrafficScenario,
                                                               schedName.str()),
                                    Duration::QUICK);
                    }
                }
            }
        }
    }
}

static NrSystemTestSchedulerQosSuite nrSystemTestSchedulerQosSuite;

} // namespace ns3
#endif // SYSTEM_SCHEDULER_TEST_QOS_H
