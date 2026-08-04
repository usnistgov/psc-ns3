//
// SPDX-License-Identifier: NIST-Software
//

#ifndef NR_SL_RLC_UM_PDB_TEST_H
#define NR_SL_RLC_UM_PDB_TEST_H

#include "nr-sl-test-mock-objects.h"

#include "ns3/nr-sl-rlc.h"
#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include "ns3/test.h"

using namespace ns3;

/**
 * \ingroup nr-sl-test
 *
 * \brief TestSuite 4.1.1 for RLC UM: Only transmitter part.
 */
class NrSlRlcUmTestSuite : public TestSuite
{
  public:
    NrSlRlcUmTestSuite();
};

/**
 * \ingroup nr-sl-test
 *
 * \brief Test case used by NrSlRlcUmOneSduTestCase to create topology
 * and to implement functionalities and check if data received corresponds to
 * data sent.
 */
class NrSlRlcUmTestCase : public TestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the test name
     */
    NrSlRlcUmTestCase(std::string name);

    /**
     * Check data received function
     * \param time the time to check
     * \param shouldReceived should have received indicator
     * \param assertMsg the assert message
     */
    void CheckDataReceived(Time time, std::string shouldReceived, std::string assertMsg);

  protected:
    void DoSetup() override;

    Ptr<NrSlTestPdcp> m_txPdcp; ///< the transmit PDCP
    Ptr<NrSlRlc> m_txRlc;       ///< the RLC
    Ptr<NrSlTestMac> m_txMac;   ///< the MAC

  private:
    /**
     * Check data received function
     * \param shouldReceived should have received indicator
     * \param assertMsg the assert message
     */
    void DoCheckDataReceived(std::string shouldReceived, std::string assertMsg);
};

/**
 * \ingroup nr-sl-test
 *
 * \brief Test TX and RX of one PDU without segmentation
 */
class NrSlRlcUmOneSduTestCase : public NrSlRlcUmTestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the test name
     */
    NrSlRlcUmOneSduTestCase(std::string name);

  private:
    void DoRun() override;
};

/**
 * \ingroup nr-sl-test
 *
 * \brief Test Discard Timer / PDB enforcement
 */
class NrSlRlcUmPdbTestCase : public NrSlRlcUmTestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the test name
     */
    NrSlRlcUmPdbTestCase(std::string name);

  private:
    void DoRun() override;
    /**
     * Callback function to receive dropped packets
     * \param p The packet that was dropped.
     */
    void TxDropCallback(Ptr<const Packet> p);

    bool m_pktDrop{false}; /// Flag to check that a packet was dropped
};

#endif /* NR_SL_RLC_UM_PDB_TEST_H */
