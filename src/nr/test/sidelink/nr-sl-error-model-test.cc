//
// SPDX-License-Identifier: NIST-Software
//

#include <ns3/log.h>
#include <ns3/nr-sl-effective-bler-calculator.h>
#include <ns3/nr-sl-epa-error-model.h>
#include <ns3/nr-sl-static-error-model.h>
#include <ns3/test.h>

#include <cmath>
#include <sstream>

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

    // Check that the correct tables are consulted, and that GetTbler()
    // returns the TBLER of the current attempt conditioned on the previous
    // failed attempts.  The reference curves are unconditional (joint)
    // error probabilities, so the conditional value is the quotient of
    // the curves for n and n-1 transmissions.  GetUnconditionalTbler()
    // composes the conditional values back into the joint probability, so
    // it must reproduce the reference curve value; if GetTbler() failed
    // to form the quotient, the composition would instead yield the
    // product of the joint probabilities and these checks would fail.
    //
    // Ntx=2 MCS 0 table point (BLER 0.98747, SNR -8.591); the Ntx=1
    // curve interpolates to 0.996536 at that SNR, so the conditional
    // TBLER of the second attempt is 0.98747 / 0.996536 = 0.990903.
    // Use the typical RV pattern of [0, 2, 3, 1]
    sinrDb = -8.591;
    tbler = staticEm->GetUnconditionalTbler(mcs, numerology, 2, sinrDb, tbSize, numRbs);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.98747, 1e-6, "test for use of 2Tx table");
    auto emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 0);
    history.push_back(emOutput);
    // Check the error model with this history of size 1
    rv = 2;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.990903, 1e-5, "test conditional TBLER, second attempt");
    history.clear();
    // Ntx=3 MCS 1 table point (BLER 0.96415, SNR -7.4191); the Ntx=2
    // curve interpolates to 0.986085, so the conditional TBLER of the
    // third attempt is 0.96415 / 0.986085 = 0.977755
    mcs = 1;
    sinrDb = -7.4191;
    tbler = staticEm->GetUnconditionalTbler(mcs, numerology, 3, sinrDb, tbSize, numRbs);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.96415, 1e-6, "test for use of 3Tx table");
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 0);
    history.push_back(emOutput);
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 2);
    history.push_back(emOutput);
    rv = 3;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.977755, 1e-5, "test conditional TBLER, third attempt");
    history.clear();
    // Ntx=4 MCS 0 table point (BLER 0.65332, SNR -8.2004); the Ntx=3
    // curve interpolates to 0.789280, so the conditional TBLER of the
    // fourth attempt is 0.65332 / 0.789280 = 0.827742
    mcs = 0;
    sinrDb = -8.2004;
    tbler = staticEm->GetUnconditionalTbler(mcs, numerology, 4, sinrDb, tbSize, numRbs);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.65332, 1e-6, "test for use of 4Tx table");
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 0);
    history.push_back(emOutput);
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 2);
    history.push_back(emOutput);
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 3);
    history.push_back(emOutput);
    rv = 1;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.827742, 1e-5, "test conditional TBLER, fourth attempt");
    history.clear();
    // Ntx=5 MCS 0 table point (BLER 0.93057, SNR -8.9816); the Ntx=4
    // curve interpolates to 0.965480, so the conditional TBLER of the
    // fifth attempt is 0.93057 / 0.965480 = 0.963842
    mcs = 0;
    sinrDb = -8.9816;
    tbler = staticEm->GetUnconditionalTbler(mcs, numerology, 5, sinrDb, tbSize, numRbs);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.93057, 1e-6, "test for use of 5Tx table");
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
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.963842, 1e-5, "test conditional TBLER, fifth attempt");
    // Do not clear the history; add another entry and check the sixth
    // attempt.  The 5Tx curve is reused beyond five transmissions, so at
    // unchanged SNR the joint probabilities for six and five
    // transmissions are equal and the conditional TBLER is 1 (no
    // additional combining gain is modeled beyond five transmissions)
    emOutput = Create<NrSlErrorModelOutput>(tbler, sinrDb, 0);
    history.push_back(emOutput);
    rv = 2;
    tbler = staticEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler,
                              1.0,
                              1e-6,
                              "test conditional TBLER, sixth attempt (5Tx table)");
    history.clear();

    // If retransmissions occur, the SNRs are averaged to create an
    // effective SNR that is used in the table.  Check here that averaging
    // occurs.  Averaging occurs on linear SNR values.  Consider a point
    // used above for the Ntx=3, MCS 1 table (BLER 0.96415, SNR -7.4191).
    // We want to find three different linear SNR values that, when
    // averaged together and converted to decibels, will equal -7.4191.
    // Then, we want to construct the history with two of these values and
    // pass the third one to the GetTbler() method.  The numerator (Ntx=3
    // curve) is evaluated at the average of all three SNRs, and the
    // denominator (Ntx=2 curve) at the average of the two history SNRs;
    // both averages equal -7.4191 (the history values are 0.5x and 1.5x
    // the target linear SNR), so the expected conditional TBLER matches
    // the constant-SNR third-attempt value 0.96415 / 0.986085 = 0.977755
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
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.977755, 1e-5, "test for averaging of varying SNRs");
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

/**
 * \brief Test the EPA error model
 *
 * This test verifies the Extended Pedestrian A (EPA) error model, which
 * uses link simulation data for two transport block sizes (10 RB and 50 RB
 * for numerology 0) and interpolates between them based on the actual TB size.
 *
 * Phase 1 tests verify basic table lookups without TB size interpolation:
 * - TB size 20 bytes maps to ceiling 23 bytes (1 subchannel), using 10 RB data
 * - TB size 185 bytes maps to ceiling 185 bytes (5 subchannels), using 50 RB data
 *
 * Phase 2 tests verify the logarithmic TB size interpolation/extrapolation:
 * - TB size 100 bytes (interpolation between curves)
 * - TB size 500 bytes (extrapolation beyond available data)
 */
class NrSlEpaErrorModelTest : public TestCase
{
  public:
    /**
     * \brief Create NrSlEpaErrorModelTest
     * \param name Name of the test
     */
    NrSlEpaErrorModelTest(const std::string& name)
        : TestCase(name)
    {
    }

  private:
    void DoRun() override;
};

void
NrSlEpaErrorModelTest::DoRun()
{
    NS_LOG_FUNCTION(this);

    // Reference data for MCS 0, Numerology 0 (SCS 15), EPA channel
    //
    // 10 RB table (TB size 23 bytes, 1 subchannel):
    // SNR: {-15.622, -9.3722, -7.8097, -7.0285, -6.2472, -5.466, -4.6847, ...}
    // BLER: {0.99995, 0.93152, 0.84553, 0.79281, 0.72397, 0.66604, 0.5962, ...}
    //
    // 50 RB table (TB size 185 bytes, 5 subchannels):
    // SNR: {-22.612, -10.112, -8.5494, -6.9869, -6.2057, -5.4244, -4.6432, ...}
    // BLER: {1, 0.98564, 0.94586, 0.85077, 0.78093, 0.70571, 0.62239, ...}

    auto epaEm = CreateObject<NrSlEpaErrorModel>();
    uint8_t mcs = 0;
    uint8_t numerology = 0;
    uint8_t rv = 0;
    NrErrorModel::NrErrorModelHistory history;

    // ========================================================================
    // Phase 1: Test without TB size interpolation
    // ========================================================================
    NS_LOG_DEBUG("Phase 1: Testing direct table lookups without TB size interpolation");

    // ---- Test with TB size 20 bytes (uses 10 RB table directly) ----
    // TB size 20 <= 23 (1 subchannel ceiling), so tblerOneSubch is returned
    uint32_t tbSize = 20;
    uint32_t numRbs = 10;

    // Test left edge of plot (very low SNR should give TBLER near 1;
    // erfc saturates near 1.0 but does not reach it exactly)
    double sinrDb = -20;
    auto tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_GT_OR_EQ(tbler, 0.5, "test left edge of 10 RB plot with 20-byte TB");

    // Test right edge of plot (high SNR should give TBLER = 0)
    sinrDb = 50;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 0, "test right edge of 10 RB plot with 20-byte TB");

    // Test erfc model at SNR -9.3722 (MCS 0, 10 RBs)
    sinrDb = -9.3722;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.8049, 1e-3, "test 10 RB erfc value with 20-byte TB");

    // Test erfc model at SNR 0.0027674 (MCS 0, 10 RBs)
    sinrDb = 0.0027674;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.2746, 1e-3, "test second 10 RB erfc value with 20-byte TB");

    // Test erfc model at SNR -6 (MCS 0, 10 RBs)
    sinrDb = -6;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.6311, 1e-3, "test 10 RB erfc value at SNR -6");

    // ---- Test with TB size 185 bytes (uses 50 RB table directly) ----
    // TB size 185 > 63 (2 subch) but ceiling is 185 (5 subch)
    // Weight w = (ln(185) - ln(23)) / (ln(185) - ln(23)) = 1.0
    // So tblerAllSubch is returned directly
    tbSize = 185;
    numRbs = 50;

    // Test left edge of plot (erfc saturates near 1.0 but does not reach it exactly)
    sinrDb = -25;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_GT_OR_EQ(tbler, 0.5, "test left edge of 50 RB plot with 185-byte TB");

    // Test right edge of plot
    sinrDb = 50;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 0, "test right edge of 50 RB plot with 185-byte TB");

    // Test erfc model at SNR -6.2057 (MCS 0, 50 RBs)
    sinrDb = -6.2057;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.7141, 1e-3, "test 50 RB erfc value with 185-byte TB");

    // Test erfc model at SNR 0.82557 (MCS 0, 50 RBs)
    sinrDb = 0.82557;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.1382, 1e-3, "test second 50 RB erfc value with 185-byte TB");

    // ========================================================================
    // Phase 2: Test TB size interpolation and extrapolation
    // ========================================================================
    NS_LOG_DEBUG("Phase 2: Testing TB size interpolation/extrapolation");

    // ---- Test with TB size 100 bytes (interpolation, 0 < w < 1) ----
    // Weight w = (ln(tbCeiling) - ln(23)) / (ln(185) - ln(23)) = 0.7190
    // TBLER interpolated in log space between 10 RB and 50 RB erfc values
    tbSize = 100;
    numRbs = 30;
    sinrDb = -5.466;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.6350, 1e-2, "test TB size interpolation at 100 bytes");

    // ---- Test with TB size 500 bytes (extrapolation, w > 1) ----
    // Weight w = (ln(510) - ln(23)) / (ln(185) - ln(23)) = 1.4866
    tbSize = 500;
    numRbs = 130;
    sinrDb = -5.466;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.6859, 1e-2, "test TB size extrapolation at 500 bytes");

    // Verify that extrapolated TBLER is capped at 1.0
    // At very low SNR with large TB, TBLER should approach but not exceed 1
    sinrDb = -15;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_LT(tbler, 1.01, "extrapolated TBLER should be capped at 1.0");

    // ========================================================================
    // Conditional TBLER for retransmissions
    // ========================================================================
    NS_LOG_DEBUG("Testing conditional TBLER for retransmissions");

    // The reference curves are unconditional (joint) error probabilities;
    // GetTbler() must return the quotient of the curves for n and n-1
    // transmissions.  At MCS 0, SNR -6 dB, 10 RBs, the Ntx=1 erfc curve
    // gives 0.631093 (tested above) and the Ntx=2 curve gives 0.443599,
    // so the conditional TBLER of the second attempt is
    // 0.443599 / 0.631093 = 0.702906
    tbSize = 20;
    numRbs = 10;
    sinrDb = -6;
    tbler = epaEm->GetUnconditionalTbler(mcs, numerology, 2, sinrDb, tbSize, numRbs);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.443599, 1e-5, "test Ntx=2 erfc value");
    history.push_back(Create<NrSlErrorModelOutput>(tbler, sinrDb, 0));
    rv = 2;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.702906, 1e-5, "test conditional TBLER, second attempt");
    history.clear();
    // Composing the conditional values over five attempts telescopes back
    // to the unconditional five-transmission erfc curve value
    tbler = epaEm->GetUnconditionalTbler(mcs, numerology, 5, sinrDb, tbSize, numRbs);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 0.092559, 1e-5, "test composition of five attempts");
    // Independently fitted curves for adjacent numbers of transmissions
    // may cross where both are near 1, and the conditional quotient is
    // clamped to 1 there.  At MCS 9, SNR -10 dB, 10 RBs, the Ntx=3 curve
    // (0.997930) exceeds the Ntx=2 curve (0.972255)
    history.push_back(Create<NrSlErrorModelOutput>(0.5, -10, 0));
    history.push_back(Create<NrSlErrorModelOutput>(0.5, -10, 2));
    rv = 3;
    tbler = epaEm->GetTbler(9, numerology, rv, -10, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ_TOL(tbler, 1.0, 1e-9, "test clamp of conditional TBLER to 1");
    history.clear();

    // ========================================================================
    // Additional tests: RV constraints (same as static model)
    // ========================================================================
    NS_LOG_DEBUG("Testing RV constraints");

    // If RV 0 or 3 is not received, then the TB cannot be decoded
    // Note: EPA model may return very small non-zero values at high SNR due to
    // its interpolation behavior, so we check for "essentially zero" (< 1e-3)
    tbSize = 20;
    numRbs = 10;
    rv = 0;
    sinrDb = 20; // strong SINR
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_LT(tbler, 1e-3, "If RV = 0, decode should succeed (TBLER near zero)");
    rv = 1;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 1, "If RV = 1, should fail to decode");
    rv = 2;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_EQ(tbler, 1, "If RV = 2, should fail to decode");
    rv = 3;
    tbler = epaEm->GetTbler(mcs, numerology, rv, sinrDb, tbSize, numRbs, history);
    NS_TEST_ASSERT_MSG_LT(tbler, 1e-3, "If RV = 3, decode should succeed (TBLER near zero)");
}

/**
 * \brief Test alignment of the EPA error model with the LLS reference data
 *
 * The EPA error model represents each TBLER curve by an erfc fit to the
 * NIST LLS reference data (contrib/nr/model/sidelink/error-model-data/epa/).
 * This test verifies two properties for one (numerology, ntx, RB anchor)
 * configuration per test case:
 *
 * 1. Accuracy. At reference points sampled from the LLS data (the
 *    measured points nearest TBLER 0.5, 0.1, and 0.05 for MCS 0, 9, 10,
 *    16, 17, and 28), the model TBLER must match the measured value
 *    within 0.15 in log10 (a factor of ~1.4). The largest residual
 *    over these points is below 0.08 in log10, so accurate fits pass with
 *    margin while systematic distortions of the curves fail.
 *
 * 2. Preserved inversions. The LLS data contains TBLER-vs-MCS
 *    inversions at the modulation boundaries (MCS 9->10, QPSK to 16QAM,
 *    and MCS 16->17, 16QAM to 64QAM), which deepen with the number of
 *    HARQ transmissions. At the SNR of maximum inversion depth,
 *    the higher MCS must report a strictly lower TBLER than the lower MCS.
 *
 * TBLER values are probed through GetUnconditionalTbler(), which
 * composes the per-attempt conditional values returned by GetTbler()
 * into the unconditional TBLER after ntx equal-SNR transmissions,
 * matching the semantics of the LLS reference curves.  The comparison
 * therefore also regression-tests the conditional quotient in
 * GetTbler(): if the quotient were missing, the composition would
 * return the product of the unconditional curves and fail the accuracy
 * checks for ntx >= 2.
 */
class NrSlEpaAlignmentTest : public TestCase
{
  public:
    /**
     * \brief Create NrSlEpaAlignmentTest
     * \param name Name of the test
     * \param numerology Numerology (0 or 1)
     * \param ntx Number of transmissions (1-5)
     * \param numRbs Number of resource blocks (an anchor value)
     */
    NrSlEpaAlignmentTest(const std::string& name, uint8_t numerology, uint8_t ntx, uint32_t numRbs)
        : TestCase(name),
          m_numerology(numerology),
          m_ntx(ntx),
          m_numRbs(numRbs)
    {
    }

  private:
    void DoRun() override;

    /**
     * \brief Compute TBLER at a probe SNR through the public API
     * \param errorModel The error model
     * \param mcs MCS index
     * \param snrDb Probe SNR in dB
     * \return The TBLER value
     */
    double GetTblerAt(Ptr<NrSlEpaErrorModel> errorModel, uint8_t mcs, double snrDb) const;

    uint8_t m_numerology; //!< Numerology (0 or 1)
    uint8_t m_ntx;        //!< Number of transmissions (1-5)
    uint32_t m_numRbs;    //!< Number of resource blocks
};

/**
 * A reference point sampled from the LLS data: the model TBLER at snrDb
 * must match tbler within ALIGNMENT_LOG10_TOL in log10.
 */
struct EpaReferencePoint
{
    uint8_t num;  //!< Numerology
    uint8_t ntx;  //!< Number of transmissions
    uint32_t rbs; //!< Number of resource blocks (anchor)
    uint8_t mcs;  //!< MCS index
    double snrDb; //!< SNR (dB) of the measured LLS point
    double tbler; //!< Measured TBLER of the LLS point
};

/**
 * A modulation-boundary inversion probe: at snrDb, the model TBLER for
 * MCS (mcsLow + 1) must be strictly lower than for mcsLow. The depth
 * field records the fitted inversion depth (in TBLER units) for
 * documentation; it is not asserted.
 */
struct EpaInversionProbe
{
    uint8_t num;    //!< Numerology
    uint8_t ntx;    //!< Number of transmissions
    uint32_t rbs;   //!< Number of resource blocks (anchor)
    uint8_t mcsLow; //!< Lower MCS of the inverted pair (9 or 16)
    double snrDb;   //!< SNR (dB) of maximum inversion depth
    double depth;   //!< Fitted inversion depth (TBLER units), informational
};

/// Tolerance for |log10(model) - log10(measured)| at reference points
static constexpr double ALIGNMENT_LOG10_TOL = 0.15;

/// LLS reference points (measured points nearest TBLER 0.5, 0.1, 0.05)
// clang-format off
static const EpaReferencePoint g_epaReferencePoints[] = {
    {0, 1, 10, 0, -3.9035, 0.53281},
    {0, 1, 10, 0, 4.1043, 0.10325},
    {0, 1, 10, 0, 6.6434, 0.050314},
    {0, 1, 10, 9, 5.4715, 0.52133},
    {0, 1, 10, 9, 14.065, 0.099698},
    {0, 1, 10, 9, 17.19, 0.052704},
    {0, 1, 10, 10, 6.2528, 0.48513},
    {0, 1, 10, 10, 14.065, 0.10055},
    {0, 1, 10, 10, 17.19, 0.04634},
    {0, 1, 10, 16, 10.94, 0.51457},
    {0, 1, 10, 16, 19.339, 0.10003},
    {0, 1, 10, 16, 22.659, 0.046692},
    {0, 1, 10, 17, 11.722, 0.50642},
    {0, 1, 10, 17, 19.925, 0.10209},
    {0, 1, 10, 17, 22.659, 0.053208},
    {0, 1, 10, 28, 21.878, 0.47321},
    {0, 1, 10, 28, 30.472, 0.10164},
    {0, 1, 10, 28, 34.378, 0.047623},
    {0, 1, 50, 0, -3.8619, 0.53275},
    {0, 1, 50, 0, 1.4115, 0.10189},
    {0, 1, 50, 0, 3.1693, 0.049434},
    {0, 1, 50, 9, 7.0756, 0.53031},
    {0, 1, 50, 9, 14.107, 0.098491},
    {0, 1, 50, 9, 16.06, 0.054667},
    {0, 1, 50, 10, 6.2943, 0.47509},
    {0, 1, 50, 10, 11.372, 0.10403},
    {0, 1, 50, 10, 13.326, 0.047296},
    {0, 1, 50, 16, 11.763, 0.52757},
    {0, 1, 50, 16, 18.013, 0.10098},
    {0, 1, 50, 16, 19.966, 0.048755},
    {0, 1, 50, 17, 12.544, 0.47663},
    {0, 1, 50, 17, 17.622, 0.10314},
    {0, 1, 50, 17, 19.576, 0.049057},
    {0, 1, 50, 28, 24.263, 0.49721},
    {0, 1, 50, 28, 33.052, 0.10151},
    {0, 1, 50, 28, 35.591, 0.053384},
    {0, 2, 10, 0, -6.2472, 0.4955},
    {0, 2, 10, 0, -1.3644, 0.10153},
    {0, 2, 10, 0, 0.19808, 0.050063},
    {0, 2, 10, 9, 3.909, 0.48196},
    {0, 2, 10, 9, 10.745, 0.10164},
    {0, 2, 10, 9, 13.675, 0.046692},
    {0, 2, 10, 10, 2.3465, 0.49454},
    {0, 2, 10, 10, 7.2293, 0.096906},
    {0, 2, 10, 10, 8.9871, 0.047748},
    {0, 2, 10, 16, 6.2528, 0.51628},
    {0, 2, 10, 16, 12.307, 0.099572},
    {0, 2, 10, 16, 14.456, 0.052579},
    {0, 2, 10, 17, 7.034, 0.48116},
    {0, 2, 10, 17, 12.112, 0.10239},
    {0, 2, 10, 17, 13.87, 0.050868},
    {0, 2, 10, 28, 13.284, 0.50828},
    {0, 2, 10, 28, 19.339, 0.09927},
    {0, 2, 10, 28, 21.487, 0.046491},
    {0, 2, 50, 0, -6.2057, 0.50058},
    {0, 2, 50, 0, -2.6901, 0.10078},
    {0, 2, 50, 0, -1.7135, 0.05434},
    {0, 2, 50, 9, 4.7318, 0.48166},
    {0, 2, 50, 9, 11.177, 0.098918},
    {0, 2, 50, 9, 13.716, 0.053031},
    {0, 2, 50, 10, 2.7787, 0.48659},
    {0, 2, 50, 10, 6.4896, 0.1042},
    {0, 2, 50, 10, 7.8568, 0.050516},
    {0, 2, 50, 16, 7.0756, 0.52362},
    {0, 2, 50, 16, 12.349, 0.098013},
    {0, 2, 50, 16, 14.107, 0.052377},
    {0, 2, 50, 17, 7.4662, 0.47225},
    {0, 2, 50, 17, 11.177, 0.10239},
    {0, 2, 50, 17, 12.349, 0.051522},
    {0, 2, 50, 28, 14.107, 0.4967},
    {0, 2, 50, 28, 18.794, 0.10108},
    {0, 2, 50, 28, 20.747, 0.048403},
    {0, 3, 10, 0, -7.8097, 0.51766},
    {0, 3, 10, 0, -3.7082, 0.093962},
    {0, 3, 10, 0, -2.341, 0.046038},
    {0, 3, 10, 9, 1.5653, 0.51472},
    {0, 3, 10, 9, 6.2528, 0.098113},
    {0, 3, 10, 9, 7.8153, 0.049132},
    {0, 3, 10, 10, 0.39339, 0.47683},
    {0, 3, 10, 10, 4.1043, 0.098717},
    {0, 3, 10, 10, 5.4715, 0.046943},
    {0, 3, 10, 16, 4.2996, 0.50468},
    {0, 3, 10, 16, 8.4012, 0.097887},
    {0, 3, 10, 16, 9.7684, 0.052302},
    {0, 3, 10, 17, 4.2996, 0.5234},
    {0, 3, 10, 17, 8.4012, 0.10408},
    {0, 3, 10, 17, 9.7684, 0.050491},
    {0, 3, 10, 28, 10.55, 0.4874},
    {0, 3, 10, 28, 15.042, 0.094642},
    {0, 3, 10, 28, 16.409, 0.052},
    {0, 3, 50, 0, -7.3776, 0.47532},
    {0, 3, 50, 0, -4.6432, 0.10521},
    {0, 3, 50, 0, -3.6666, 0.049283},
    {0, 3, 50, 9, 2.3881, 0.49842},
    {0, 3, 50, 9, 6.4896, 0.10158},
    {0, 3, 50, 9, 7.8568, 0.049057},
    {0, 3, 50, 10, 0.82557, 0.50958},
    {0, 3, 50, 10, 3.9506, 0.099019},
    {0, 3, 50, 10, 5.1224, 0.049509},
    {0, 3, 50, 16, 5.1224, 0.49555},
    {0, 3, 50, 16, 8.6381, 0.10294},
    {0, 3, 50, 16, 10.005, 0.047547},
    {0, 3, 50, 17, 5.1224, 0.48566},
    {0, 3, 50, 17, 8.2474, 0.094943},
    {0, 3, 50, 17, 9.224, 0.052075},
    {0, 3, 50, 28, 11.372, 0.50362},
    {0, 3, 50, 28, 15.279, 0.099019},
    {0, 3, 50, 28, 16.451, 0.054792},
    {0, 4, 10, 0, -8.591, 0.51615},
    {0, 4, 10, 0, -5.0754, 0.094294},
    {0, 4, 10, 0, -4.0988, 0.046086},
    {0, 4, 10, 9, 0.78402, 0.54342},
    {0, 4, 10, 9, 5.4715, 0.10032},
    {0, 4, 10, 9, 7.034, 0.050614},
    {0, 4, 10, 10, -0.77848, 0.52128},
    {0, 4, 10, 10, 2.7371, 0.10174},
    {0, 4, 10, 10, 4.1043, 0.048103},
    {0, 4, 10, 16, 3.5184, 0.48395},
    {0, 4, 10, 16, 7.2293, 0.10466},
    {0, 4, 10, 16, 8.5965, 0.051827},
    {0, 4, 10, 17, 3.5184, 0.47992},
    {0, 4, 10, 17, 7.034, 0.09831},
    {0, 4, 10, 17, 8.2059, 0.052526},
    {0, 4, 10, 28, 8.5965, 0.48063},
    {0, 4, 10, 28, 12.503, 0.0968},
    {0, 4, 10, 28, 13.675, 0.048304},
    {0, 4, 50, 0, -8.1588, 0.4914},
    {0, 4, 50, 0, -5.6197, 0.096206},
    {0, 4, 50, 0, -4.8385, 0.053129},
    {0, 4, 50, 9, 1.6068, 0.51152},
    {0, 4, 50, 9, 5.7084, 0.095995},
    {0, 4, 50, 9, 7.0756, 0.049004},
    {0, 4, 50, 10, -0.34631, 0.53537},
    {0, 4, 50, 10, 2.5834, 0.10867},
    {0, 4, 50, 10, 3.5599, 0.047902},
    {0, 4, 50, 16, 3.9506, 0.52108},
    {0, 4, 50, 16, 7.2709, 0.10284},
    {0, 4, 50, 16, 8.2474, 0.054639},
    {0, 4, 50, 17, 3.5599, 0.53235},
    {0, 4, 50, 17, 6.6849, 0.092574},
    {0, 4, 50, 17, 7.4662, 0.05404},
    {0, 4, 50, 28, 9.4193, 0.50639},
    {0, 4, 50, 28, 12.74, 0.098621},
    {0, 4, 50, 28, 13.716, 0.052526},
    {0, 5, 10, 0, -8.9816, 0.46969},
    {0, 5, 10, 0, -6.2472, 0.099245},
    {0, 5, 10, 0, -5.2707, 0.048302},
    {0, 5, 10, 9, 0.0027674, 0.50868},
    {0, 5, 10, 9, 3.5184, 0.093836},
    {0, 5, 10, 9, 4.495, 0.053082},
    {0, 5, 10, 10, -1.5597, 0.46893},
    {0, 5, 10, 10, 1.1746, 0.10692},
    {0, 5, 10, 10, 2.3465, 0.048176},
    {0, 5, 10, 16, 2.3465, 0.47547},
    {0, 5, 10, 16, 5.2762, 0.10377},
    {0, 5, 10, 16, 6.4481, 0.047044},
    {0, 5, 10, 17, 1.9559, 0.50616},
    {0, 5, 10, 17, 5.0809, 0.097987},
    {0, 5, 10, 17, 5.8621, 0.054465},
    {0, 5, 10, 28, 7.034, 0.51547},
    {0, 5, 10, 28, 10.354, 0.097233},
    {0, 5, 10, 28, 11.331, 0.05195},
    {0, 5, 50, 0, -8.9401, 0.54642},
    {0, 5, 50, 0, -6.401, 0.093082},
    {0, 5, 50, 0, -5.8151, 0.052704},
    {0, 5, 50, 9, 0.82557, 0.48679},
    {0, 5, 50, 9, 3.9506, 0.098616},
    {0, 5, 50, 9, 4.9271, 0.046667},
    {0, 5, 50, 10, -1.1276, 0.52164},
    {0, 5, 50, 10, 1.6068, 0.097736},
    {0, 5, 50, 10, 2.3881, 0.046792},
    {0, 5, 50, 16, 3.1693, 0.46843},
    {0, 5, 50, 16, 5.7084, 0.096981},
    {0, 5, 50, 16, 6.6849, 0.046918},
    {0, 5, 50, 17, 2.7787, 0.45887},
    {0, 5, 50, 17, 5.1224, 0.10164},
    {0, 5, 50, 17, 6.099, 0.044528},
    {0, 5, 50, 28, 8.2474, 0.47836},
    {0, 5, 50, 28, 10.982, 0.10063},
    {0, 5, 50, 28, 11.958, 0.048931},
    {1, 1, 10, 0, -3.7888, 0.5194},
    {1, 1, 10, 0, 3.2425, 0.09753},
    {1, 1, 10, 0, 5.1956, 0.05131},
    {1, 1, 10, 9, 5.5862, 0.52905},
    {1, 1, 10, 9, 14.18, 0.10272},
    {1, 1, 10, 9, 16.914, 0.049204},
    {1, 1, 10, 10, 6.3675, 0.48537},
    {1, 1, 10, 10, 13.399, 0.10066},
    {1, 1, 10, 10, 15.742, 0.048752},
    {1, 1, 10, 16, 11.055, 0.5195},
    {1, 1, 10, 16, 19.258, 0.10066},
    {1, 1, 10, 16, 21.992, 0.048627},
    {1, 1, 10, 17, 11.836, 0.50944},
    {1, 1, 10, 17, 19.453, 0.099536},
    {1, 1, 10, 17, 21.602, 0.052414},
    {1, 1, 10, 28, 21.992, 0.50641},
    {1, 1, 10, 28, 31.563, 0.1034},
    {1, 1, 10, 28, 35.274, 0.050458},
    {1, 1, 20, 0, -3.6741, 0.48429},
    {1, 1, 20, 0, 1.404, 0.098533},
    {1, 1, 20, 0, 3.1619, 0.048226},
    {1, 1, 20, 9, 6.4822, 0.51172},
    {1, 1, 20, 9, 13.318, 0.095824},
    {1, 1, 20, 9, 15.076, 0.053693},
    {1, 1, 20, 10, 6.4822, 0.48645},
    {1, 1, 20, 10, 11.756, 0.10167},
    {1, 1, 20, 10, 13.513, 0.050984},
    {1, 1, 20, 16, 11.951, 0.47601},
    {1, 1, 20, 16, 17.81, 0.10265},
    {1, 1, 20, 16, 19.763, 0.05111},
    {1, 1, 20, 17, 11.951, 0.50046},
    {1, 1, 20, 17, 17.615, 0.10051},
    {1, 1, 20, 17, 19.373, 0.050357},
    {1, 1, 20, 28, 23.67, 0.51534},
    {1, 1, 20, 28, 33.045, 0.098959},
    {1, 1, 20, 28, 35.779, 0.048226},
    {1, 2, 10, 0, -6.1325, 0.48407},
    {1, 2, 10, 0, -1.6403, 0.09986},
    {1, 2, 10, 0, -0.077845, 0.047397},
    {1, 2, 10, 9, 4.0237, 0.48129},
    {1, 2, 10, 9, 10.469, 0.10107},
    {1, 2, 10, 9, 12.617, 0.050206},
    {1, 2, 10, 10, 2.4612, 0.47791},
    {1, 2, 10, 10, 6.9534, 0.097904},
    {1, 2, 10, 10, 8.3206, 0.049453},
    {1, 2, 10, 16, 6.3675, 0.51718},
    {1, 2, 10, 16, 11.836, 0.10021},
    {1, 2, 10, 16, 13.789, 0.048902},
    {1, 2, 10, 17, 7.1487, 0.46506},
    {1, 2, 10, 17, 11.641, 0.098856},
    {1, 2, 10, 17, 13.203, 0.046645},
    {1, 2, 10, 28, 13.399, 0.50895},
    {1, 2, 10, 28, 18.867, 0.10232},
    {1, 2, 10, 28, 20.821, 0.047748},
    {1, 2, 20, 0, -6.4085, 0.52305},
    {1, 2, 20, 0, -2.5022, 0.0969},
    {1, 2, 20, 0, -1.3303, 0.047046},
    {1, 2, 20, 9, 4.1384, 0.507},
    {1, 2, 20, 9, 10.388, 0.10056},
    {1, 2, 20, 9, 12.732, 0.047397},
    {1, 2, 20, 10, 2.1853, 0.50218},
    {1, 2, 20, 10, 5.8962, 0.10628},
    {1, 2, 20, 10, 7.0681, 0.050456},
    {1, 2, 20, 16, 7.2634, 0.45794},
    {1, 2, 20, 16, 11.56, 0.10151},
    {1, 2, 20, 16, 13.513, 0.048199},
    {1, 2, 20, 17, 6.8728, 0.49039},
    {1, 2, 20, 17, 10.584, 0.10477},
    {1, 2, 20, 17, 11.756, 0.051763},
    {1, 2, 20, 28, 13.513, 0.53524},
    {1, 2, 20, 28, 18.396, 0.10448},
    {1, 2, 20, 28, 20.154, 0.049707},
    {1, 3, 10, 0, -7.695, 0.50508},
    {1, 3, 10, 0, -3.9841, 0.10217},
    {1, 3, 10, 0, -2.8122, 0.049278},
    {1, 3, 10, 9, 1.68, 0.51949},
    {1, 3, 10, 9, 6.1722, 0.10067},
    {1, 3, 10, 9, 7.7347, 0.049579},
    {1, 3, 10, 10, 0.11747, 0.5211},
    {1, 3, 10, 10, 4.0237, 0.097803},
    {1, 3, 10, 10, 5.1956, 0.048981},
    {1, 3, 10, 16, 4.4143, 0.4971},
    {1, 3, 10, 16, 8.3206, 0.10082},
    {1, 3, 10, 16, 9.4925, 0.05334},
    {1, 3, 10, 17, 4.4143, 0.51147},
    {1, 3, 10, 17, 8.1253, 0.10164},
    {1, 3, 10, 17, 9.2972, 0.05161},
    {1, 3, 10, 28, 10.664, 0.4953},
    {1, 3, 10, 28, 14.961, 0.099759},
    {1, 3, 10, 28, 16.133, 0.053645},
    {1, 3, 20, 0, -7.5803, 0.50655},
    {1, 3, 20, 0, -4.4553, 0.09833},
    {1, 3, 20, 0, -3.4788, 0.049203},
    {1, 3, 20, 9, 2.1853, 0.49748},
    {1, 3, 20, 9, 6.2869, 0.10269},
    {1, 3, 20, 9, 7.654, 0.054168},
    {1, 3, 20, 10, 0.23217, 0.53288},
    {1, 3, 20, 10, 3.3572, 0.10797},
    {1, 3, 20, 10, 4.3337, 0.049278},
    {1, 3, 20, 16, 4.9197, 0.48033},
    {1, 3, 20, 16, 8.4353, 0.096983},
    {1, 3, 20, 16, 9.6072, 0.051685},
    {1, 3, 20, 17, 4.529, 0.49221},
    {1, 3, 20, 17, 7.4587, 0.10834},
    {1, 3, 20, 17, 8.4353, 0.052738},
    {1, 3, 20, 28, 11.17, 0.49959},
    {1, 3, 20, 28, 15.271, 0.096374},
    {1, 3, 20, 28, 16.443, 0.049658},
    {1, 4, 10, 0, -8.4763, 0.50391},
    {1, 4, 10, 0, -5.156, 0.095797},
    {1, 4, 10, 0, -4.1794, 0.050055},
    {1, 4, 10, 9, 1.2893, 0.49373},
    {1, 4, 10, 9, 5.3909, 0.10091},
    {1, 4, 10, 9, 7.1487, 0.046243},
    {1, 4, 10, 10, -0.66378, 0.51831},
    {1, 4, 10, 10, 2.6565, 0.10051},
    {1, 4, 10, 10, 3.8284, 0.047648},
    {1, 4, 10, 16, 3.6331, 0.47668},
    {1, 4, 10, 16, 7.1487, 0.099107},
    {1, 4, 10, 16, 8.3206, 0.050858},
    {1, 4, 10, 17, 3.2425, 0.52162},
    {1, 4, 10, 17, 6.9534, 0.092186},
    {1, 4, 10, 17, 7.93, 0.049955},
    {1, 4, 10, 28, 8.7112, 0.48636},
    {1, 4, 10, 28, 12.422, 0.10122},
    {1, 4, 10, 28, 13.789, 0.050657},
    {1, 4, 20, 0, -8.3616, 0.52402},
    {1, 4, 20, 0, -5.4319, 0.093299},
    {1, 4, 20, 0, -4.846, 0.051159},
    {1, 4, 20, 9, 1.404, 0.52402},
    {1, 4, 20, 9, 5.5056, 0.10382},
    {1, 4, 20, 9, 6.8728, 0.051159},
    {1, 4, 20, 10, -0.54908, 0.52683},
    {1, 4, 20, 10, 2.1853, 0.10121},
    {1, 4, 20, 10, 2.9665, 0.050557},
    {1, 4, 20, 16, 3.7478, 0.50642},
    {1, 4, 20, 16, 7.0681, 0.099609},
    {1, 4, 20, 16, 8.0447, 0.046444},
    {1, 4, 20, 17, 3.3572, 0.45912},
    {1, 4, 20, 17, 5.8962, 0.10182},
    {1, 4, 20, 17, 6.8728, 0.046946},
    {1, 4, 20, 28, 9.2165, 0.50943},
    {1, 4, 20, 28, 12.732, 0.095897},
    {1, 4, 20, 28, 13.904, 0.048049},
    {1, 5, 10, 0, -9.2575, 0.53354},
    {1, 5, 10, 0, -6.1325, 0.090533},
    {1, 5, 10, 0, -5.3513, 0.050909},
    {1, 5, 10, 9, 0.11747, 0.50997},
    {1, 5, 10, 9, 3.6331, 0.097931},
    {1, 5, 10, 9, 4.6097, 0.050282},
    {1, 5, 10, 10, -1.445, 0.48188},
    {1, 5, 10, 10, 1.2893, 0.099687},
    {1, 5, 10, 10, 2.2659, 0.046019},
    {1, 5, 10, 16, 2.4612, 0.47561},
    {1, 5, 10, 16, 5.3909, 0.10307},
    {1, 5, 10, 16, 6.3675, 0.046897},
    {1, 5, 10, 17, 2.0706, 0.49668},
    {1, 5, 10, 17, 5.0003, 0.096301},
    {1, 5, 10, 17, 5.7815, 0.052038},
    {1, 5, 10, 28, 7.1487, 0.52539},
    {1, 5, 10, 28, 10.664, 0.093918},
    {1, 5, 10, 28, 11.641, 0.049781},
    {1, 5, 20, 0, -8.7522, 0.48777},
    {1, 5, 20, 0, -6.4085, 0.10282},
    {1, 5, 20, 0, -5.6272, 0.050784},
    {1, 5, 20, 9, 0.62279, 0.49292},
    {1, 5, 20, 9, 3.7478, 0.10395},
    {1, 5, 20, 9, 4.9197, 0.049028},
    {1, 5, 20, 10, -1.3303, 0.49429},
    {1, 5, 20, 10, 1.0134, 0.10583},
    {1, 5, 20, 10, 1.7947, 0.045893},
    {1, 5, 20, 16, 2.5759, 0.5294},
    {1, 5, 20, 16, 5.5056, 0.10169},
    {1, 5, 20, 16, 6.2869, 0.051411},
    {1, 5, 20, 17, 2.1853, 0.48665},
    {1, 5, 20, 17, 4.529, 0.093668},
    {1, 5, 20, 17, 5.3103, 0.045016},
    {1, 5, 20, 28, 8.0447, 0.50094},
    {1, 5, 20, 28, 10.974, 0.094044},
    {1, 5, 20, 28, 11.951, 0.047774},
};
// clang-format on

/// Modulation-boundary inversion probes (fitted depth >= 0.01)
// clang-format off
static const EpaInversionProbe g_epaInversionProbes[] = {
    {0, 1, 50, 9, 9.1, 0.1289},
    {0, 1, 50, 16, 17.95, 0.0119},
    {0, 2, 10, 9, 6.6, 0.1464},
    {0, 2, 10, 16, 13.6, 0.0142},
    {0, 2, 50, 9, 6.55, 0.2119},
    {0, 2, 50, 16, 11.8, 0.0610},
    {0, 3, 10, 9, 2.1, 0.1608},
    {0, 3, 50, 9, 2.85, 0.2020},
    {0, 3, 50, 16, 7.7, 0.0391},
    {0, 4, 10, 9, 1.35, 0.2122},
    {0, 4, 10, 16, 7.05, 0.0161},
    {0, 4, 50, 9, 2.15, 0.2440},
    {0, 4, 50, 16, 5.65, 0.0646},
    {0, 5, 10, 9, -0.4, 0.2459},
    {0, 5, 10, 16, 2.9, 0.0329},
    {0, 5, 50, 9, 0.45, 0.2772},
    {0, 5, 50, 16, 3.95, 0.0741},
    {1, 1, 10, 9, 13.2, 0.0257},
    {1, 1, 20, 9, 10.25, 0.0555},
    {1, 1, 20, 16, 18.3, 0.0113},
    {1, 2, 10, 9, 5.65, 0.1766},
    {1, 2, 10, 16, 12.65, 0.0137},
    {1, 2, 20, 9, 5.5, 0.2402},
    {1, 2, 20, 16, 10.5, 0.0575},
    {1, 3, 10, 9, 2.2, 0.1821},
    {1, 3, 10, 16, 8.15, 0.0118},
    {1, 3, 20, 9, 2.6, 0.2430},
    {1, 3, 20, 16, 6.4, 0.0725},
    {1, 4, 10, 9, 1.5, 0.2256},
    {1, 4, 10, 16, 5.95, 0.0259},
    {1, 4, 20, 9, 1.75, 0.2841},
    {1, 4, 20, 16, 5.3, 0.0956},
    {1, 5, 10, 9, -0.2, 0.2589},
    {1, 5, 10, 16, 3.9, 0.0452},
    {1, 5, 20, 9, 0.3, 0.3266},
    {1, 5, 20, 16, 3.3, 0.1266},
};
// clang-format on

double
NrSlEpaAlignmentTest::GetTblerAt(Ptr<NrSlEpaErrorModel> errorModel, uint8_t mcs, double snrDb) const
{
    uint32_t tbSizeUnused = 0;
    return errorModel
        ->GetUnconditionalTbler(mcs, m_numerology, m_ntx, snrDb, tbSizeUnused, m_numRbs);
}

void
NrSlEpaAlignmentTest::DoRun()
{
    NS_LOG_FUNCTION(this);

    auto epaEm = CreateObject<NrSlEpaErrorModel>();

    for (const auto& point : g_epaReferencePoints)
    {
        if (point.num != m_numerology || point.ntx != m_ntx || point.rbs != m_numRbs)
        {
            continue;
        }
        double tbler = GetTblerAt(epaEm, point.mcs, point.snrDb);
        std::ostringstream msg;
        msg << "Model TBLER " << tbler << " deviates from LLS reference " << point.tbler
            << " at MCS " << +point.mcs << " SNR=" << point.snrDb << " dB (num=" << +m_numerology
            << " ntx=" << +m_ntx << " rbs=" << m_numRbs << ")";
        NS_TEST_ASSERT_MSG_GT(tbler, 0.0, msg.str());
        NS_TEST_ASSERT_MSG_LT_OR_EQ(std::abs(std::log10(tbler / point.tbler)),
                                    ALIGNMENT_LOG10_TOL,
                                    msg.str());
    }

    for (const auto& probe : g_epaInversionProbes)
    {
        if (probe.num != m_numerology || probe.ntx != m_ntx || probe.rbs != m_numRbs)
        {
            continue;
        }
        double tblerLow = GetTblerAt(epaEm, probe.mcsLow, probe.snrDb);
        double tblerHigh = GetTblerAt(epaEm, probe.mcsLow + 1, probe.snrDb);
        std::ostringstream msg;
        msg << "Boundary inversion MCS " << +probe.mcsLow << "->" << +(probe.mcsLow + 1)
            << " not preserved at SNR=" << probe.snrDb << " dB: TBLER " << tblerHigh << " !< "
            << tblerLow << " (num=" << +m_numerology << " ntx=" << +m_ntx << " rbs=" << m_numRbs
            << ")";
        NS_TEST_ASSERT_MSG_LT(tblerHigh, tblerLow, msg.str());
    }
}

/**
 * \brief Test alignment of the static error model with the LLS reference data
 *
 * The static error model interpolates the NIST LLS reference curves
 * (contrib/nr/model/sidelink/error-model-data/static/) directly.  At
 * reference points sampled from the LLS data (the measured points nearest
 * TBLER 0.5 and 0.1 for MCS 0, 7, 14, 21, and 28, for each number of
 * transmissions), GetUnconditionalTbler() must reproduce the measured
 * value.  As in the EPA alignment test, the probe composes the
 * per-attempt conditional values returned by GetTbler(), so this also
 * regression-tests the conditional quotient: if the quotient were
 * missing, the composition would return the product of the unconditional
 * curves and fail for ntx >= 2.  The composition reproduces the table
 * values to machine precision at these points (the quotient clamp does
 * not engage), so a tight log10 tolerance is used.
 */
class NrSlStaticAlignmentTest : public TestCase
{
  public:
    /**
     * \brief Create NrSlStaticAlignmentTest
     * \param name Name of the test
     */
    NrSlStaticAlignmentTest(const std::string& name)
        : TestCase(name)
    {
    }

  private:
    void DoRun() override;
};

/**
 * A reference point sampled from the static LLS data: the model TBLER at
 * snrDb must match tbler within STATIC_ALIGNMENT_LOG10_TOL in log10.
 */
struct StaticReferencePoint
{
    uint8_t ntx;  //!< Number of transmissions
    uint8_t mcs;  //!< MCS index
    double snrDb; //!< SNR (dB) of the measured LLS point
    double tbler; //!< Measured TBLER of the LLS point
};

/// Tolerance for |log10(model) - log10(measured)| at static reference points
static constexpr double STATIC_ALIGNMENT_LOG10_TOL = 0.01;

/// Static LLS reference points (measured points nearest TBLER 0.5 and 0.1)
// clang-format off
static const StaticReferencePoint g_staticReferencePoints[] = {
    {1, 0, -5.0754, 0.4838},
    {1, 0, -4.4894, 0.12528},
    {2, 0, -7.4191, 0.53585},
    {2, 0, -6.4425, 0.10254},
    {3, 0, -7.8097, 0.51962},
    {3, 0, -7.0285, 0.074642},
    {4, 0, -8.005, 0.50005},
    {4, 0, -7.4191, 0.11159},
    {5, 0, -8.2004, 0.4673},
    {5, 0, -7.6144, 0.090189},
    {1, 7, 2.1512, 0.54123},
    {1, 7, 2.5418, 0.086038},
    {2, 7, -0.19255, 0.46335},
    {2, 7, 0.78402, 0.10214},
    {3, 7, -0.77848, 0.4683},
    {3, 7, 0.0027674, 0.11026},
    {4, 7, -0.9738, 0.49542},
    {4, 7, -0.19255, 0.10203},
    {5, 7, -1.1691, 0.52855},
    {5, 7, -0.58317, 0.11862},
    {1, 14, 7.8153, 0.50259},
    {1, 14, 8.2059, 0.05844},
    {2, 14, 3.3231, 0.37962},
    {2, 14, 3.5184, 0.11945},
    {3, 14, 1.1746, 0.64498},
    {3, 14, 1.5653, 0.10589},
    {4, 14, 0.39339, 0.52098},
    {4, 14, 1.1746, 0.11875},
    {5, 14, 0.19808, 0.38629},
    {5, 14, 0.5887, 0.10642},
    {1, 21, 13.675, 0.34377},
    {1, 21, 13.87, 0.1203},
    {2, 21, 7.62, 0.58687},
    {2, 21, 8.0106, 0.056352},
    {3, 21, 5.0809, 0.64974},
    {3, 21, 5.4715, 0.056755},
    {4, 21, 3.7137, 0.63862},
    {4, 21, 4.1043, 0.036731},
    {5, 21, 2.5418, 0.35333},
    {5, 21, 2.7371, 0.085283},
    {1, 28, 19.729, 0.36553},
    {1, 28, 19.925, 0.11275},
    {2, 28, 11.136, 0.42058},
    {2, 28, 11.331, 0.10103},
    {3, 28, 8.2059, 0.62755},
    {3, 28, 8.5965, 0.024453},
    {4, 28, 6.0575, 0.46211},
    {4, 28, 6.2528, 0.095401},
    {5, 28, 4.8856, 0.48201},
    {5, 28, 5.2762, 0.11799},
};
// clang-format on

void
NrSlStaticAlignmentTest::DoRun()
{
    NS_LOG_FUNCTION(this);

    auto staticEm = CreateObject<NrSlStaticErrorModel>();
    uint8_t numerology = 0;
    uint32_t tbSizeUnused = 0;
    uint32_t numRbsUnused = 0;

    for (const auto& point : g_staticReferencePoints)
    {
        double tbler = staticEm->GetUnconditionalTbler(point.mcs,
                                                       numerology,
                                                       point.ntx,
                                                       point.snrDb,
                                                       tbSizeUnused,
                                                       numRbsUnused);
        std::ostringstream msg;
        msg << "Model TBLER " << tbler << " deviates from LLS reference " << point.tbler
            << " at MCS " << +point.mcs << " SNR=" << point.snrDb << " dB (ntx=" << +point.ntx
            << ")";
        NS_TEST_ASSERT_MSG_GT(tbler, 0.0, msg.str());
        NS_TEST_ASSERT_MSG_LT_OR_EQ(std::abs(std::log10(tbler / point.tbler)),
                                    STATIC_ALIGNMENT_LOG10_TOL,
                                    msg.str());
    }
}

/**
 * \brief Test NrSlEffectiveBlerCalculator
 *
 * Checks with synthetic per-attempt values that the TB-only path equals
 * the plain product of conditionals, that the composed path equals the
 * explicit sum over received-attempt subsets for one to three attempts,
 * that an attempt received alone with RV 2 contributes failure
 * probability 1 (no systematic bits), and that with SCI-2a BLER zero the
 * two paths agree.
 */
class NrSlEffectiveBlerCalculatorTest : public TestCase
{
  public:
    /**
     * \brief Create NrSlEffectiveBlerCalculatorTest
     * \param name Name of the test
     */
    NrSlEffectiveBlerCalculatorTest(const std::string& name)
        : TestCase(name)
    {
    }

  private:
    void DoRun() override;
};

void
NrSlEffectiveBlerCalculatorTest::DoRun()
{
    NS_LOG_FUNCTION(this);
    constexpr double C1 = 0.5;
    constexpr double C2 = 0.3;
    constexpr double C3 = 0.2;
    constexpr double Q = 0.1;

    // TB-only path: product of conditionals, RV sequence (0, 2, 3)
    {
        NrSlEffectiveBlerCalculator calc;
        calc.AddAttempt(C1, Q, 0);
        NS_TEST_ASSERT_MSG_EQ_TOL(calc.GetEffectiveBler(false), C1, 1e-12, "TB-only, 1 attempt");
        calc.AddAttempt(C2, Q, 2);
        NS_TEST_ASSERT_MSG_EQ_TOL(calc.GetEffectiveBler(false),
                                  C1 * C2,
                                  1e-12,
                                  "TB-only, 2 attempts");
        calc.AddAttempt(C3, Q, 3);
        NS_TEST_ASSERT_MSG_EQ_TOL(calc.GetEffectiveBler(false),
                                  C1 * C2 * C3,
                                  1e-12,
                                  "TB-only, 3 attempts");
    }

    // Composed path: explicit subset sums.  J0 = 1, J1 = C1, J2 = C1*C2,
    // J3 = C1*C2*C3; a subset of received attempts decodes only if it
    // contains attempt 1 (RV 0) or attempt 3 (RV 3).
    {
        NrSlEffectiveBlerCalculator calc;
        calc.AddAttempt(C1, Q, 0);
        double expected1 = Q + (1 - Q) * C1;
        NS_TEST_ASSERT_MSG_EQ_TOL(calc.GetEffectiveBler(true),
                                  expected1,
                                  1e-12,
                                  "Composed, 1 attempt");
        calc.AddAttempt(C2, Q, 2);
        // Subsets: {} -> 1; {2} (RV 2 alone) -> 1; {1} -> J1; {1,2} -> J2
        double expected2 =
            Q * Q + Q * (1 - Q) * 1.0 + (1 - Q) * Q * C1 + (1 - Q) * (1 - Q) * (C1 * C2);
        NS_TEST_ASSERT_MSG_EQ_TOL(calc.GetEffectiveBler(true),
                                  expected2,
                                  1e-12,
                                  "Composed, 2 attempts");
        calc.AddAttempt(C3, Q, 3);
        // Subsets: {} and {2} undecodable (probability Q^3 and Q^2(1-Q));
        // {1} and {3} -> J1; {1,2}, {1,3}, {2,3} -> J2; {1,2,3} -> J3
        double expected3 = Q * Q * Q + Q * Q * (1 - Q) * (1.0 + C1 + C1) +
                           Q * (1 - Q) * (1 - Q) * 3.0 * (C1 * C2) +
                           (1 - Q) * (1 - Q) * (1 - Q) * (C1 * C2 * C3);
        NS_TEST_ASSERT_MSG_EQ_TOL(calc.GetEffectiveBler(true),
                                  expected3,
                                  1e-12,
                                  "Composed, 3 attempts");
    }

    // An attempt received alone with RV 2 carries no systematic bits, so
    // the composed effective BLER is 1 regardless of the conditional
    {
        NrSlEffectiveBlerCalculator calc;
        calc.AddAttempt(0.01, Q, 2);
        NS_TEST_ASSERT_MSG_EQ_TOL(calc.GetEffectiveBler(true),
                                  1.0,
                                  1e-12,
                                  "RV 2 alone is undecodable");
    }

    // With SCI-2a BLER zero the two paths agree
    {
        NrSlEffectiveBlerCalculator calc;
        calc.AddAttempt(C1, 0.0, 0);
        calc.AddAttempt(C2, 0.0, 2);
        calc.AddAttempt(C3, 0.0, 3);
        NS_TEST_ASSERT_MSG_EQ_TOL(calc.GetEffectiveBler(true),
                                  calc.GetEffectiveBler(false),
                                  1e-12,
                                  "Paths agree when SCI-2a BLER is zero");
    }
}

class NrSlErrorModelTestSuite : public TestSuite
{
  public:
    NrSlErrorModelTestSuite()
        : TestSuite("nr-sl-error-model", Type::UNIT)
    {
        AddTestCase(new NrSlStaticErrorModelTest("Unit test on static error model"),
                    Duration::QUICK);
        AddTestCase(new NrSlStaticAlignmentTest("Static error model alignment with LLS data"),
                    Duration::QUICK);
        AddTestCase(new NrSlEpaErrorModelTest("Unit test on EPA error model"), Duration::QUICK);
        // Alignment tests for all erfc parameter table configurations,
        // at the RB anchor values where the reference data was measured.
        // Numerology 0 (SCS 15 kHz): Rb10 and Rb50 anchors, Ntx 1-5
        // Numerology 1 (SCS 30 kHz): Rb10 and Rb20 anchors, Ntx 1-5
        for (uint8_t num = 0; num <= 1; num++)
        {
            std::vector<uint32_t> rbValues;
            if (num == 0)
            {
                rbValues = {10, 50};
            }
            else
            {
                rbValues = {10, 20};
            }
            for (uint8_t ntx = 1; ntx <= 5; ntx++)
            {
                for (auto numRbs : rbValues)
                {
                    std::ostringstream name;
                    name << "EPA alignment num=" << +num << " ntx=" << +ntx << " rbs=" << numRbs;
                    AddTestCase(new NrSlEpaAlignmentTest(name.str(), num, ntx, numRbs),
                                Duration::QUICK);
                }
            }
        }
        AddTestCase(new NrSlEffectiveBlerCalculatorTest("Unit test on effective BLER calculator"),
                    Duration::QUICK);
    }
};

static NrSlErrorModelTestSuite nrSlErrorModelTestSuite; //!< Nr error model test suite
