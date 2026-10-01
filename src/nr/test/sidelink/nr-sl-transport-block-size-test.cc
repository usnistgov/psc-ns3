//
// SPDX-License-Identifier: NIST-Software

/**
 * \file nr-sl-transport-block-size-test.cc
 * \ingroup test
 *
 * Test case for NrSlCommResourcePool transport block size calculations.
 * Validates GetTransportBlockSize() and GetMinSubchannels() against expected values
 * derived from TS 38.214 Section 8.1.3.2.
 */

#include <ns3/log.h>
#include <ns3/nr-mcs-tables.h>
#include <ns3/nr-sl-comm-resource-pool.h>
#include <ns3/test.h>

#include <sstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrSlTransportBlockSizeTest");

/**
 * Test case for NrSlCommResourcePool::GetTransportBlockSize()
 */
class NrSlTransportBlockSizeTest : public TestCase
{
  public:
    NrSlTransportBlockSizeTest();

  private:
    void DoRun() override;

    /**
     * Create a typical SlotInfo for testing
     * \param numPscchRbs Number of PSCCH RBs
     * \param pscchSymbols Number of PSCCH symbols
     * \param psschSymbols Number of PSSCH symbols
     * \param subchannelSize Subchannel size in RBs
     * \param hasPsfch Whether slot has PSFCH
     * \return SlotInfo with the specified parameters
     */
    NrSlCommResourcePool::SlotInfo CreateSlotInfo(uint16_t numPscchRbs,
                                                  uint16_t pscchSymbols,
                                                  uint16_t psschSymbols,
                                                  uint16_t subchannelSize,
                                                  bool hasPsfch = false);
};

NrSlTransportBlockSizeTest::NrSlTransportBlockSizeTest()
    : TestCase("Check NrSlCommResourcePool TBS calculations")
{
}

NrSlCommResourcePool::SlotInfo
NrSlTransportBlockSizeTest::CreateSlotInfo(uint16_t numPscchRbs,
                                           uint16_t pscchSymbols,
                                           uint16_t psschSymbols,
                                           uint16_t subchannelSize,
                                           bool hasPsfch)
{
    // SlotInfo constructor:
    // numSlPscchRbs, slPscchSymStart, slPscchSymLength,
    // slPsschSymStart, slPsschSymLength, slHasPsfch,
    // slSubchannelSize, slMaxNumPerReserve, absSlotIndex, slotOffset
    return NrSlCommResourcePool::SlotInfo(numPscchRbs,    // numSlPscchRbs
                                          0,              // slPscchSymStart
                                          pscchSymbols,   // slPscchSymLength
                                          pscchSymbols,   // slPsschSymStart
                                          psschSymbols,   // slPsschSymLength
                                          hasPsfch,       // slHasPsfch
                                          subchannelSize, // slSubchannelSize
                                          2,              // slMaxNumPerReserve
                                          0,              // absSlotIndex
                                          0);             // slotOffset
}

void
NrSlTransportBlockSizeTest::DoRun()
{
    // Create resource pool for TBS calculations
    auto pool = CreateObject<NrSlCommResourcePool>();

    // Test configuration matching NIST Sidelink LLS reference implementation:
    // - 10 PRBs per subchannel
    // - 2 symbols for PSCCH, 10 PRBs for PSCCH
    // - 12 symbols for PSSCH (SL symbols = 14 minus 2 for AGC/guard)
    constexpr uint16_t subchannelSize = 10;
    constexpr uint16_t numPscchRbs = 10;
    constexpr uint16_t pscchSymbols = 2;
    constexpr uint16_t psschSymbols = 12;
    constexpr uint8_t mcsTable = 1;

    auto slotInfo = CreateSlotInfo(numPscchRbs, pscchSymbols, psschSymbols, subchannelSize);

    struct TestVector
    {
        uint16_t nSubchannels;
        uint32_t expectedTbs;
    };

    std::vector<TestVector> mcs0Tests = {
        {1, 23},  // 1 subchannel (10 PRBs)
        {2, 63},  // 2 subchannels (20 PRBs)
        {3, 101}, // 3 subchannels (30 PRBs)
        {4, 141}, // 4 subchannels (40 PRBs)
        {5, 185}, // 5 subchannels (50 PRBs)
    };

    NS_LOG_INFO("Testing GetTransportBlockSize for MCS 0");
    for (const auto& test : mcs0Tests)
    {
        uint32_t tbs = pool->GetTransportBlockSize(slotInfo, 0, test.nSubchannels, mcsTable);
        NS_LOG_INFO("  " << test.nSubchannels << " subchannels: TBS = " << tbs
                         << " bytes (expected " << test.expectedTbs << ")");
        std::ostringstream msg;
        msg << "TBS mismatch for MCS 0, " << test.nSubchannels << " subchannels";
        NS_TEST_EXPECT_MSG_EQ(tbs, test.expectedTbs, msg.str());
    }

    // Test vectors for MCS 14 without PSFCH
    std::vector<TestVector> mcs14Tests = {
        {1, 277},  // 1 subchannel (10 PRBs)
        {2, 624},  // 2 subchannels (20 PRBs)
        {3, 992},  // 3 subchannels (30 PRBs)
        {4, 1345}, // 4 subchannels (40 PRBs)
        {5, 1697}, // 5 subchannels (50 PRBs)
    };

    NS_LOG_INFO("Testing GetTransportBlockSize for MCS 14");
    for (const auto& test : mcs14Tests)
    {
        uint32_t tbs = pool->GetTransportBlockSize(slotInfo, 14, test.nSubchannels, mcsTable);
        NS_LOG_INFO("  " << test.nSubchannels << " subchannels: TBS = " << tbs
                         << " bytes (expected " << test.expectedTbs << ")");
        std::ostringstream msg;
        msg << "TBS mismatch for MCS 14, " << test.nSubchannels << " subchannels";
        NS_TEST_EXPECT_MSG_EQ(tbs, test.expectedTbs, msg.str());
    }

    // Test vectors for MCS 28 without PSFCH
    std::vector<TestVector> mcs28Tests = {
        {1, 720},  // 1 subchannel (10 PRBs)
        {2, 1633}, // 2 subchannels (20 PRBs)
        {3, 2562}, // 3 subchannels (30 PRBs)
        {4, 3457}, // 4 subchannels (40 PRBs)
        {5, 4352}, // 5 subchannels (50 PRBs)
    };

    NS_LOG_INFO("Testing GetTransportBlockSize for MCS 28");
    for (const auto& test : mcs28Tests)
    {
        uint32_t tbs = pool->GetTransportBlockSize(slotInfo, 28, test.nSubchannels, mcsTable);
        NS_LOG_INFO("  " << test.nSubchannels << " subchannels: TBS = " << tbs
                         << " bytes (expected " << test.expectedTbs << ")");
        std::ostringstream msg;
        msg << "TBS mismatch for MCS 28, " << test.nSubchannels << " subchannels";
        NS_TEST_EXPECT_MSG_EQ(tbs, test.expectedTbs, msg.str());
    }

    // Create SlotInfo for PSFCH scenarios (9 PSSCH symbols instead of 12)
    constexpr uint16_t psschSymbolsWithPsfch = 9;
    auto slotInfoPsfch =
        CreateSlotInfo(numPscchRbs, pscchSymbols, psschSymbolsWithPsfch, subchannelSize, true);

    std::vector<TestVector> mcs0TestsWithPsfch = {
        {1, 12},  // 1 subchannel (10 PRBs)
        {2, 40},  // 2 subchannels (20 PRBs)
        {3, 69},  // 3 subchannels (30 PRBs)
        {4, 101}, // 4 subchannels (40 PRBs)
        {5, 129}, // 5 subchannels (50 PRBs)
    };

    NS_LOG_INFO("Testing GetTransportBlockSize for MCS 0 with PSFCH");
    for (const auto& test : mcs0TestsWithPsfch)
    {
        uint32_t tbs = pool->GetTransportBlockSize(slotInfoPsfch, 0, test.nSubchannels, mcsTable);
        NS_LOG_INFO("  " << test.nSubchannels << " subchannels: TBS = " << tbs
                         << " bytes (expected " << test.expectedTbs << ")");
        std::ostringstream msg;
        msg << "TBS mismatch for MCS 0 with PSFCH, " << test.nSubchannels << " subchannels";
        NS_TEST_EXPECT_MSG_EQ(tbs, test.expectedTbs, msg.str());
    }

    std::vector<TestVector> mcs14TestsWithPsfch = {
        {1, 177},  // 1 subchannel (10 PRBs)
        {2, 437},  // 2 subchannels (20 PRBs)
        {3, 688},  // 3 subchannels (30 PRBs)
        {4, 960},  // 4 subchannels (40 PRBs)
        {5, 1217}, // 5 subchannels (50 PRBs)
    };

    NS_LOG_INFO("Testing GetTransportBlockSize for MCS 14 with PSFCH");
    for (const auto& test : mcs14TestsWithPsfch)
    {
        uint32_t tbs = pool->GetTransportBlockSize(slotInfoPsfch, 14, test.nSubchannels, mcsTable);
        NS_LOG_INFO("  " << test.nSubchannels << " subchannels: TBS = " << tbs
                         << " bytes (expected " << test.expectedTbs << ")");
        std::ostringstream msg;
        msg << "TBS mismatch for MCS 14 with PSFCH, " << test.nSubchannels << " subchannels";
        NS_TEST_EXPECT_MSG_EQ(tbs, test.expectedTbs, msg.str());
    }

    std::vector<TestVector> mcs28TestsWithPsfch = {
        {1, 478},  // 1 subchannel (10 PRBs)
        {2, 1153}, // 2 subchannels (20 PRBs)
        {3, 1793}, // 3 subchannels (30 PRBs)
        {4, 2496}, // 4 subchannels (40 PRBs)
        {5, 3138}, // 5 subchannels (50 PRBs)
    };

    NS_LOG_INFO("Testing GetTransportBlockSize for MCS 28 with PSFCH");
    for (const auto& test : mcs28TestsWithPsfch)
    {
        uint32_t tbs = pool->GetTransportBlockSize(slotInfoPsfch, 28, test.nSubchannels, mcsTable);
        NS_LOG_INFO("  " << test.nSubchannels << " subchannels: TBS = " << tbs
                         << " bytes (expected " << test.expectedTbs << ")");
        std::ostringstream msg;
        msg << "TBS mismatch for MCS 28 with PSFCH, " << test.nSubchannels << " subchannels";
        NS_TEST_EXPECT_MSG_EQ(tbs, test.expectedTbs, msg.str());
    }

    // Test that TBS increases with MCS within the same modulation order.
    // At modulation order transitions (e.g., QPSK to 16QAM at MCS 9->10),
    // TBS can decrease because the code rate drops and the MCS-dependent
    // N_RE_SCI2 increases (TS 38.212 Section 8.4.4).
    NS_LOG_INFO("Testing TBS increases with MCS within same modulation order");
    uint32_t prevTbs = 0;
    uint8_t prevQm = 0;
    const uint16_t sampleSubchannels = 3; // arbitrary subchannel length
    for (uint8_t mcs = 0; mcs <= 28; ++mcs)
    {
        uint32_t tbs = pool->GetTransportBlockSize(slotInfo, mcs, sampleSubchannels, mcsTable);
        uint8_t Qm = NrMcsTables::GetModulationOrder(mcs, mcsTable);
        if (Qm == prevQm)
        {
            NS_TEST_EXPECT_MSG_GT_OR_EQ(tbs, prevTbs, "TBS should increase with MCS");
        }
        prevTbs = tbs;
        prevQm = Qm;
    }

    NS_LOG_INFO("Testing that TBS increases with the number of subchannels");
    prevTbs = 0;
    const uint16_t sampleMcs = 14; // arbitrary MCS
    for (uint16_t nSubch = 1; nSubch <= 5; nSubch++)
    {
        uint32_t tbs = pool->GetTransportBlockSize(slotInfo, sampleMcs, nSubch, mcsTable);
        NS_TEST_EXPECT_MSG_GT(tbs, prevTbs, "TBS should increase with subchannels");
        prevTbs = tbs;
    }

    NS_LOG_INFO("Testing GetMinSubchannels");
    auto result = pool->GetMinSubchannels(slotInfo,
                                          0 /* MCS */,
                                          50 /* tbSize */,
                                          5 /* maxSubchannels */,
                                          mcsTable);
    NS_TEST_EXPECT_MSG_EQ(result.has_value(), true, "Should find subchannels for 50 bytes");
    if (result.has_value())
    {
        NS_LOG_INFO("  Min subchannels for 50 bytes at MCS 0: " << result.value());
        // For MCS 0, 50 bytes requires 2 subchannels (TBS = 63) since 1 subchannel only handles
        // 23 bytes
        NS_TEST_EXPECT_MSG_EQ(result.value(), 2, "50 bytes requires 2 subchannels at MCS 0");
    }

    // Test GetMinSubchannels returns nullopt when requested TB size can't be satisfied
    auto resultLarge = pool->GetMinSubchannels(slotInfo, 0, 10000, 5, mcsTable);
    NS_TEST_EXPECT_MSG_EQ(resultLarge.has_value(),
                          false,
                          "Should return nullopt for TB size that cannot be handled");
}

/**
 * Test suite for NR Sidelink TBS calculations
 */
class NrSlTbsTestSuite : public TestSuite
{
  public:
    NrSlTbsTestSuite();
};

NrSlTbsTestSuite::NrSlTbsTestSuite()
    : TestSuite("nr-sl-transport-block-size", Type::UNIT)
{
    AddTestCase(new NrSlTransportBlockSizeTest, TestCase::Duration::QUICK);
}

/**
 * Static variable for test initialization
 */
static NrSlTbsTestSuite s_nrSlTbsTestSuite;
