// Copyright (c) 2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "system-scheduler-test.h"

#include "ns3/test.h"

using namespace ns3;

/**
 * \file nr-system-test-schedulers-ofdma-pf.cc
 * \ingroup test
 *
 * \brief System test for OFDMA - Proportional Fair scheduler. It checks that all the
 * packets sent are delivered correctly.
 */

/**
 * \brief The OFDMA PF scheduler system test suite
 * \ingroup test
 *
 * It will check OFDMA PF with:
 *
 * - DL, UL, DL and UL together
 * - UEs per beam: 1, 2, 4, 8
 * - beams: 1, 2
 * - numerologies: 0, 1
 */
class NrSystemTestSchedulerOfdmaPfSuite : public TestSuite
{
  public:
    /**
     * \brief constructor
     */
    NrSystemTestSchedulerOfdmaPfSuite();
};

NrSystemTestSchedulerOfdmaPfSuite::NrSystemTestSchedulerOfdmaPfSuite()
    : TestSuite("nr-system-test-schedulers-ofdma-pf", Type::SYSTEM)
{
    enum TxMode
    {
        DL,
        UL,
        DL_UL
    };

    std::list<std::string> subdivision = {
        "Ofdma",
    };
    std::list<std::string> scheds = {"PF"};
    std::list<TxMode> mode = {DL, UL, DL_UL};
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
                            if (modeType == DL)
                            {
                                ss << "DL";
                            }
                            else if (modeType == UL)
                            {
                                ss << "UL";
                            }
                            else
                            {
                                ss << "DL_UL";
                            }
                            ss << ", Num " << num << ", " << subType << " " << sched << ", "
                               << uesPerBeam << " UE per beam, " << beam << " beam";
                            const bool isDl = modeType == DL || modeType == DL_UL;
                            const bool isUl = modeType == UL || modeType == DL_UL;

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

// Do not forget to allocate an instance of this TestSuite
static NrSystemTestSchedulerOfdmaPfSuite mmwaveTestSuite;
