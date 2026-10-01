// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "system-scheduler-test.h"

#include "ns3/test.h"

using namespace ns3;

/**
 * \file nr-system-test-schedulers-tdma-rr.cc
 * \ingroup test
 *
 * \brief System test for TDMA - Round Robin scheduler. It checks that all the
 * packets sent are delivered correctly.
 */

/**
 * \brief The TDMA RR scheduler system test suite
 * \ingroup test
 *
 * It will check Tdma RR with:
 *
 * - DL
 * - UEs per beam: 1, 2, 4, 8
 * - beams: 1, 2
 * - numerologies: 0, 1
 */
class NrSystemTestSchedulerTdmaRrDlSuite : public TestSuite
{
  public:
    /**
     * \brief constructor
     */
    NrSystemTestSchedulerTdmaRrDlSuite();
};

NrSystemTestSchedulerTdmaRrDlSuite::NrSystemTestSchedulerTdmaRrDlSuite()
    : TestSuite("nr-system-test-schedulers-tdma-rr-dl", Type::SYSTEM)
{
    std::list<std::string> subdivision = {
        "Tdma",
    };
    std::list<std::string> scheds = {"RR"};
    std::list<std::string> mode = {
        "DL",
    };
    std::list<uint32_t> uesPerBeamList = {1, 2, 4, 8};
    std::list<uint32_t> beams = {1, 2};
    std::list<uint32_t> numerologies = {
        0,
        1,
    }; // Test only num 0 and 1

    for (const auto& num : numerologies)
    {
        for (const auto& subType : subdivision)
        {
            for (const auto& sched : scheds)
            {
                for (const auto& modeType : mode)
                {
                    for (const auto& uesPerBeam : uesPerBeamList)
                    {
                        for (const auto& beam : beams)
                        {
                            std::stringstream ss;
                            std::stringstream schedName;
                            ss << ", Num " << num << ", " << modeType << ", " << subType << " "
                               << sched << ", " << uesPerBeam << " UE per beam, " << beam
                               << " beam";
                            const bool isDl = modeType == "DL" || modeType == "DL_UL";
                            const bool isUl = modeType == "UL" || modeType == "DL_UL";

                            schedName << "ns3::NrMacScheduler" << subType << sched;

                            AddTestCase(new SystemSchedulerTest(ss.str(),
                                                                uesPerBeam,
                                                                beam,
                                                                num,
                                                                20e6,
                                                                isDl,
                                                                isUl,
                                                                schedName.str()),
                                        Duration::QUICK);
                        }
                    }
                }
            }
        }
    }
}

static NrSystemTestSchedulerTdmaRrDlSuite nrSystemTestSchedulerTdmaRrDlSuite;

// ----------------------------------------------------------------------------

/**
 * \brief The TDMA RR scheduler system test suite
 *
 * It will check Tdma RR with:
 *
 * - UL
 * - UEs per beam: 1, 2, 4, 8
 * - beams: 1, 2
 * - numerologies: 0, 1
 */
class NrSystemTestSchedulerTdmaRrUlSuite : public TestSuite
{
  public:
    /**
     * \brief constructor
     */
    NrSystemTestSchedulerTdmaRrUlSuite();
};

NrSystemTestSchedulerTdmaRrUlSuite::NrSystemTestSchedulerTdmaRrUlSuite()
    : TestSuite("nr-system-test-schedulers-tdma-rr-ul", Type::SYSTEM)
{
    std::list<std::string> subdivision = {
        "Tdma",
    };
    std::list<std::string> scheds = {"RR"};
    std::list<std::string> mode = {
        "UL",
    };
    std::list<uint32_t> uesPerBeamList = {1, 2, 4, 8};
    std::list<uint32_t> beams = {1, 2};
    std::list<uint32_t> numerologies = {
        0,
        1,
    }; // Test only num 0 and 1

    for (const auto& num : numerologies)
    {
        for (const auto& subType : subdivision)
        {
            for (const auto& sched : scheds)
            {
                for (const auto& modeType : mode)
                {
                    for (const auto& uesPerBeam : uesPerBeamList)
                    {
                        for (const auto& beam : beams)
                        {
                            std::stringstream ss;
                            std::stringstream schedName;
                            ss << ", Num " << num << ", " << modeType << ", " << subType << " "
                               << sched << ", " << uesPerBeam << " UE per beam, " << beam
                               << " beam";
                            const bool isDl = modeType == "DL" || modeType == "DL_UL";
                            const bool isUl = modeType == "UL" || modeType == "DL_UL";

                            schedName << "ns3::NrMacScheduler" << subType << sched;

                            AddTestCase(new SystemSchedulerTest(ss.str(),
                                                                uesPerBeam,
                                                                beam,
                                                                num,
                                                                20e6,
                                                                isDl,
                                                                isUl,
                                                                schedName.str()),
                                        Duration::QUICK);
                        }
                    }
                }
            }
        }
    }
}

static NrSystemTestSchedulerTdmaRrUlSuite nrSystemTestSchedulerTdmaRrUlSuite;

// ----------------------------------------------------------------------------

/**
 * \brief The TDMA RR scheduler system test suite
 *
 * It will check Tdma RR with:
 *
 * - DL/UL
 * - UEs per beam: 1, 2, 4, 8
 * - beams: 1, 2
 * - numerologies: 0, 1
 */
class NrSystemTestSchedulerTdmaRrDlUlSuite : public TestSuite
{
  public:
    /**
     * \brief constructor
     */
    NrSystemTestSchedulerTdmaRrDlUlSuite();
};

NrSystemTestSchedulerTdmaRrDlUlSuite::NrSystemTestSchedulerTdmaRrDlUlSuite()
    : TestSuite("nr-system-test-schedulers-tdma-rr-dl-ul", Type::SYSTEM)
{
    std::list<std::string> subdivision = {
        "Tdma",
    };
    std::list<std::string> scheds = {"RR"};
    std::list<std::string> mode = {
        "DL_UL",
    };
    std::list<uint32_t> uesPerBeamList = {8};
    std::list<uint32_t> beams = {
        2,
    };
    std::list<uint32_t> numerologies = {
        0,
    }; // Test only num 0 and 1

    for (const auto& num : numerologies)
    {
        for (const auto& subType : subdivision)
        {
            for (const auto& sched : scheds)
            {
                for (const auto& modeType : mode)
                {
                    for (const auto& uesPerBeam : uesPerBeamList)
                    {
                        for (const auto& beam : beams)
                        {
                            std::stringstream ss;
                            std::stringstream schedName;
                            ss << ", Num " << num << ", " << modeType << ", " << subType << " "
                               << sched << ", " << uesPerBeam << " UE per beam, " << beam
                               << " beam";
                            const bool isDl = modeType == "DL" || modeType == "DL_UL";
                            const bool isUl = modeType == "UL" || modeType == "DL_UL";

                            schedName << "ns3::NrMacScheduler" << subType << sched;

                            AddTestCase(new SystemSchedulerTest(ss.str(),
                                                                uesPerBeam,
                                                                beam,
                                                                num,
                                                                20e6,
                                                                isDl,
                                                                isUl,
                                                                schedName.str()),
                                        Duration::QUICK);
                        }
                    }
                }
            }
        }
    }
}

static NrSystemTestSchedulerTdmaRrDlUlSuite nrSystemTestSchedulerTdmaRrDlUlSuite;

// ----------------------------------------------------------------------------
