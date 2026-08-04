/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 */

#ifndef NR_TEST_RLC_AM_E2E_H
#define NR_TEST_RLC_AM_E2E_H

#include "ns3/ptr.h"
#include "ns3/test.h"

namespace ns3
{
class Packet;
}

using namespace ns3;

/**
 * \ingroup nr-test
 *
 * \brief Test suite for RlcAmE2e test case.
 */
class NrRlcAmE2eTestSuite : public TestSuite
{
  public:
    NrRlcAmE2eTestSuite();
};

/**
 * \ingroup nr-test
 *
 * Test cases used for the test suite lte-rlc-am-e2e. See the testing section of
 * the NR module documentation for details.
 */
class NrRlcAmE2eTestCase : public TestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the reference name
     * \param seed the random variable seed
     * \param losses the error rate
     * \param bulkSduArrival true if bulk SDU arrival
     */
    NrRlcAmE2eTestCase(std::string name, uint32_t seed, double losses, bool bulkSduArrival);
    NrRlcAmE2eTestCase();
    ~NrRlcAmE2eTestCase() override;

  private:
    void DoRun() override;

    /**
     * DL drop event
     * \param p the packet
     */
    void DlDropEvent(Ptr<const Packet> p);
    /**
     * UL drop event
     * \param p the packet
     */
    void UlDropEvent(Ptr<const Packet> p);

    uint32_t m_run;        ///< rng run
    double m_losses;       ///< error rate
    bool m_bulkSduArrival; ///< bulk SDU arrival

    uint32_t m_dlDrops; ///< number of Dl drops
    uint32_t m_ulDrops; ///< number of UL drops
};

#endif // NR_TEST_RLC_AM_E2E_H
