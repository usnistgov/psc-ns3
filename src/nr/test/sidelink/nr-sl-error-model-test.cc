//
// SPDX-License-Identifier: NIST-Software
//

#include <ns3/log.h>
#include <ns3/nr-sl-static-error-model.h>
#include <ns3/test.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrSlErrorModelTest");

/**
 * \brief Test the static error model
 */
class NrSlStaticErrorModelTest : public TestCase
{
  public:
    /**
     * \brief Create NrSlStaticErrorModelTest
     * \param name Name of the test
     */
    NrSlStaticErrorModelTest(const std::string& name)
        : TestCase(name)
    {
    }

  private:
    void DoRun() override;
};

void
NrSlStaticErrorModelTest::DoRun()
{
    NS_LOG_FUNCTION(this);
    // TBLER vector for MCS 0, Band 14 (793 MHz), SCS 15, static channel, single transmission
    // {1,         1,          0.98148,    0.91884, 0.85248,  0.75112,  0.62971,
    //  0.4838,    0.34465,    0.22287,    0.12528, 0.063849, 0.030365, 0.010742,
    //  0.0011069, 0.00042767, 7.5472e-05, 0,       0,        0},
    // SNR vector for MCS 0, Band 14 (793 MHz), SCS 15, static channel, single transmission
    // {-15.622, -9.3722, -6.2472, -5.8566, -5.6613, -5.466,  -5.2707, -5.0754, -4.88,  -4.6847,
    //  -4.4894, -4.2941, -4.0988, -3.9035, -3.5129, -3.3175, -3.1222, 9.3778,  34.378, 84.378},
    auto staticEm = CreateObject<NrSlStaticErrorModel>();
    uint8_t mcs = 0;
    uint8_t numerology = 0;
    uint8_t rv = 0;
    uint32_t tbSize = 100;
    uint32_t numRbs = 50;
    NrErrorModel::NrErrorModelHistory history;

    double sinrDb = -20;
    auto tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 1, "test left edge of plot");

    sinrDb = 20;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 0, "test right edge of plot");

    sinrDb = -6.2472;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.98148, 1e-6, "test specific point in plot");

    // Interpolate between the SNR points of -6.2472 and -5.8566
    // to find the value at SNR -6.  The BLER points are 0.98148 and 0.91884.
    // The linear SNR point values are 0.23729 and 0.259621.  The
    // requested linear SNR is 0.251189 (-6 dB).
    // 0.259621 - 0.251189 = 0.008432.  0.25962 - 0.23729 = 0.02233
    // 0.00843/0.02233 = 0.37761.
    // The expected BLER value is then 37.76% of the difference between 0.98148
    // and 0.91884; i.e. 0.91884 + (0.98148 - 0.91884) * 0.3776 = 0.94249
    sinrDb = -6;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.94249, 1e-5, "test interpolation");

    // Extrapolate using default values for Threshold and ExtrapolationLimit
    // The last valid point for a TBLER threshold of 0.01 is the point at
    // 0.010742 at an SNR of -3.9035
    // With the default ExtrapolationLimit of 1 dB, this means that
    // the TBLER for -2.9034 dB should be zero, and the TBLER for -3 dB should
    // be extrapolated
    sinrDb = -2.9034;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 0, "test extrapolation beyond limit");
    // At SNR of -4.0988 dB, TBLER is 0.030365 (roughly 10^-1.5)
    // At SNR -3.9035 dB, TBLER is 0.010742 (roughly 10^-2)
    // An SNR increase of roughly 0.2 dB led to TBLER dropping by roughly 0.5
    // on the log(TBLER) scale.  Therefore, to further drop to 10^-4 should
    // require a SNR increase of 0.2 * 4 = 0.8.  Consequently, we can check
    // that a SNR increase of 0.6 is above the extrapolation limit, while
    // an increase of 1 dB should be below the limit
    sinrDb = -3.9035 + 0.6;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_GT(tbler, 0, "test extrapolation within limit");
    sinrDb = -3.9035 + 1;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 0, "test extrapolation beyond limit");

    // Check that the correct tables are being consulted.  If we have a
    // SNR history of one entry (2nd transmission), the model should use
    // the Ntx=2 table.  First, create an error model history entry at the
    // SNR of interest.  SNR -8.591 dB yields TBLER of 0.98747
    // Use the typical RV pattern of [0, 2, 3, 1]
    sinrDb = -8.591;
    auto emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 0);
    history.push_back(emOutput);
    // Check the error model with this history of size 1
    rv = 2;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.98747, 1e-6, "test for use of 2Tx table");
    history.clear();
    // Check a point in the Ntx=3 MCS 1 table (BLER 0.96415, SNR -7.4191)
    mcs = 1;
    sinrDb = -7.4191;
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 0);
    history.push_back(emOutput);
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 2);
    history.push_back(emOutput);
    rv = 3;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.96415, 1e-6, "test for use of 3Tx table");
    history.clear();
    // Check a point in the Ntx=4 MCS 0 table (BLER 0.65332, SNR -8.2004)
    mcs = 0;
    sinrDb = -8.2004;
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 0);
    history.push_back(emOutput);
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 2);
    history.push_back(emOutput);
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 3);
    history.push_back(emOutput);
    rv = 1;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.65332, 1e-6, "test for use of 4Tx table");
    history.clear();
    // Check a point in the Ntx=5 MCS 0 table (BLER 0.93057, SNR -8.9816)
    mcs = 0;
    sinrDb = -8.9816;
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 0);
    history.push_back(emOutput);
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 2);
    history.push_back(emOutput);
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 3);
    history.push_back(emOutput);
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 1);
    history.push_back(emOutput);
    rv = 0;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.93057, 1e-6, "test for use of 5Tx table");
    // Do not clear the table; add another retransmission and check that
    // the result of Ntx=6 equals the result of Ntx=5 (due to the limitation
    // in the table data).
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 0);
    history.push_back(emOutput);
    rv = 2;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.93057, 1e-6, "test for use of 5Tx table with 6Tx");
    history.clear();

    // If retransmissions occur, the SNRs are averaged to create an
    // effective SNR that is used in the table.  Check here that averaging
    // occurs.  Averaging occurs on linear SNR values.  Consider a point
    // used above for the Ntx=3, MCS 1 table (BLER 0.96415, SNR -7.4191).
    // We want to find three different linear SNR values that, when
    // averaged together and converted to decibels, will equal -7.4191.
    // Then, we want to construct the history with two of these values and
    // pass the third one to the GetTbler() method, and then check that
    // tbler equals 0.96415
    mcs = 1;
    double snrTargetAverage = -7.4191; // dB
    double snrLinear = pow(10, snrTargetAverage / 10);
    double snrLowDb = 10 * log10(0.5 * snrLinear);
    double snrHighDb = 10 * log10(1.5 * snrLinear);
    emOutput = Create<NrSlErrorModelOutput>(tbler, snrLowDb, 0);
    history.push_back(emOutput);
    emOutput = Create<NrSlErrorModelOutput>(tbler, snrHighDb, 2);
    history.push_back(emOutput);
    rv = 3;
    tbler = staticEm->GetTbler(mcs, numerology, rv, snrTargetAverage, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.96415, 1e-6, "test for averaging of varying SNRs");
    history.clear();
    mcs = 0;

    // If RV 0 or 3 is not received, then the TB cannot be decoded due to
    // lack of systematic bits, even if the SNR is high.  Check that this
    // is enforced.
    rv = 0;
    sinrDb = 20; // strong SINR
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 0, "If RV = 0, decode should succeed");
    rv = 1;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 1, "If RV = 1, should fail to decode");
    rv = 2;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 1, "If RV = 2, should fail to decode");
    rv = 3;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 0, "If RV = 3, decode should succeed");
}

class NrSlErrorModelTestSuite : public TestSuite
{
  public:
    NrSlErrorModelTestSuite()
        : TestSuite("nr-sl-error-model", Type::UNIT)
    {
        AddTestCase(new NrSlStaticErrorModelTest("Unit test on static error model"),
                    Duration::QUICK);
    }
};

static NrSlErrorModelTestSuite nrSlErrorModelTestSuite; //!< Nr error model test suite
