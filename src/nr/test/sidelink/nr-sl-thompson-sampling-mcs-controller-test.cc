//
// SPDX-License-Identifier: NIST-Software

#include <ns3/config.h>
#include <ns3/double.h>
#include <ns3/log.h>
#include <ns3/nr-sl-epa-error-model.h>
#include <ns3/nr-sl-ideal-mcs-controller.h>
#include <ns3/nr-sl-thompson-sampling-mcs-controller.h>
#include <ns3/random-variable-stream.h>
#include <ns3/rng-seed-manager.h>
#include <ns3/test.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrSlThompsonSamplingMcsControllerTest");

/**
 * \brief Two-state Markov channel simulator with Gaussian fading.
 *
 * For unit testing, approximates a pedestrian urban channel with
 * small scale SNR fluctuations and larger step LOS/NLOS (the two
 * Markov states) transitions.
 *
 * Each state has a configurable mean SNR, and Gaussian small-scale
 * variation is added within each state.  When the transition probability
 * is zero, the channel stays in the initial state (LOS).  If both
 * state SNRs are equal, the model degenerates to effectively single-state.
 */
struct ChannelSimulator
{
    double m_losSnrDb;       //!< Mean SNR in LOS state
    double m_nlosSnrDb;      //!< Mean SNR in NLOS state
    double m_fadingStdDevDb; //!< Fading standard deviation
    double m_transitionProb; //!< Per-trial state transition probability
    bool m_isLos{true};      //!< Current LOS/NLOS state

    Ptr<NormalRandomVariable> m_fadingRng;      //!< RNG for small-scale fading
    Ptr<UniformRandomVariable> m_transitionRng; //!< RNG for state transitions

    /**
     * \brief Constructor
     * \param losSnrDb mean SNR in LOS state (dB)
     * \param nlosSnrDb mean SNR in NLOS state (dB)
     * \param fadingStdDevDb fading standard deviation (dB)
     * \param transitionProb per-trial state transition probability
     */
    ChannelSimulator(double losSnrDb,
                     double nlosSnrDb,
                     double fadingStdDevDb,
                     double transitionProb)
        : m_losSnrDb(losSnrDb),
          m_nlosSnrDb(nlosSnrDb),
          m_fadingStdDevDb(fadingStdDevDb),
          m_transitionProb(transitionProb)
    {
        m_fadingRng = CreateObject<NormalRandomVariable>();
        m_fadingRng->SetAttribute("Mean", DoubleValue(0.0));
        m_fadingRng->SetAttribute("Variance", DoubleValue(fadingStdDevDb * fadingStdDevDb));
        m_transitionRng = CreateObject<UniformRandomVariable>();
    }

    /**
     * \brief Generate the next SNR sample.
     *
     * Performs a possible state transition, then returns the state
     * mean SNR plus a Gaussian fading sample.
     *
     * \return the instantaneous SNR in dB
     */
    double GenerateSnr()
    {
        if (m_transitionProb > 0 && m_transitionRng->GetValue() < m_transitionProb)
        {
            m_isLos = !m_isLos;
        }
        double stateSnrDb = m_isLos ? m_losSnrDb : m_nlosSnrDb;
        if (m_fadingStdDevDb > 0)
        {
            stateSnrDb += m_fadingRng->GetValue();
        }
        return stateSnrDb;
    }

    /**
     * \brief Assign deterministic RNG streams.
     * \param stream the first stream index to use
     * \return the number of streams assigned
     */
    int64_t AssignStreams(int64_t stream)
    {
        m_fadingRng->SetStream(stream);
        m_transitionRng->SetStream(stream + 1);
        return 2;
    }
};

/**
 * \brief Test case for the Thompson Sampling MCS controller test framework.
 *
 * Runs a lightweight test loop that bypasses full ns-3
 * simulation.  A channel simulator generates SNR values, an oracle
 * selects the optimal MCS via binary search on the error model, and
 * the test evaluates success/failure using Bernoulli draws against
 * the BLER and collision probabilities.
 */
class NrSlThompsonSamplingMcsControllerTest : public TestCase
{
  public:
    /**
     * \brief Constructor
     * \param name test case name
     * \param losSnrDb mean SNR in LOS state
     * \param nlosSnrDb mean SNR in NLOS state
     * \param fadingStdDevDb fading standard deviation
     * \param transitionProb per-trial LOS/NLOS transition probability
     * \param collisionRate per-trial collision probability
     * \param targetBler target block error rate for oracle MCS selection
     * \param numTrials number of trials to run
     */
    NrSlThompsonSamplingMcsControllerTest(const std::string& name,
                                          double losSnrDb,
                                          double nlosSnrDb,
                                          double fadingStdDevDb,
                                          double transitionProb,
                                          double collisionRate,
                                          double targetBler,
                                          uint32_t numTrials)
        : TestCase(name),
          m_losSnrDb(losSnrDb),
          m_nlosSnrDb(nlosSnrDb),
          m_fadingStdDevDb(fadingStdDevDb),
          m_transitionProb(transitionProb),
          m_collisionRate(collisionRate),
          m_targetBler(targetBler),
          m_numTrials(numTrials)
    {
    }

  private:
    void DoRun() override;
    void DoSetup() override;
    void DoTeardown() override;

    uint32_t m_rngSeedCache{1}; //!< Cached value of RngSeed
    uint64_t m_rngRunCache{1};  //!< Cached value of RngRun

    double m_losSnrDb;       //!< Mean SNR in LOS state
    double m_nlosSnrDb;      //!< Mean SNR in NLOS state
    double m_fadingStdDevDb; //!< Fading standard deviation
    double m_transitionProb; //!< Per-trial state transition probability
    double m_collisionRate;  //!< Per-trial collision probability
    double m_targetBler;     //!< Target BLER for oracle
    uint32_t m_numTrials;    //!< Number of trials
};

void
NrSlThompsonSamplingMcsControllerTest::DoSetup()
{
    NS_LOG_FUNCTION(this);
    // Lock down the RngSeed and RngRun values for this test; cache previous values
    m_rngSeedCache = RngSeedManager::GetSeed();
    m_rngRunCache = RngSeedManager::GetRun();
    Config::SetGlobal("RngSeed", UintegerValue(1));
    Config::SetGlobal("RngRun", UintegerValue(1));
}

void
NrSlThompsonSamplingMcsControllerTest::DoTeardown()
{
    NS_LOG_FUNCTION(this);
    // Restore the seed and run number that were in effect before this test
    Config::SetGlobal("RngSeed", UintegerValue(m_rngSeedCache));
    Config::SetGlobal("RngRun", UintegerValue(m_rngRunCache));
}

void
NrSlThompsonSamplingMcsControllerTest::DoRun()
{
    NS_LOG_FUNCTION(this);

    auto errorModel = CreateObject<NrSlEpaErrorModel>();
    ChannelSimulator channel(m_losSnrDb, m_nlosSnrDb, m_fadingStdDevDb, m_transitionProb);

    // Create RNGs for BLER and collision evaluation
    auto blerRng = CreateObject<UniformRandomVariable>();
    auto collisionRng = CreateObject<UniformRandomVariable>();

    // Assign deterministic RNG streams
    int64_t stream = 1;
    stream += channel.AssignStreams(stream);
    blerRng->SetStream(stream++);
    collisionRng->SetStream(stream++);

    // Fixed parameters for GetTbler (EPA error model ignores tbSize)
    const uint8_t numerology = 0;
    const uint8_t rv = 0;
    const uint32_t tbSize = 100;
    const uint32_t numRbs = 10;
    NrErrorModel::NrErrorModelHistory history;

    // Metrics
    uint32_t decodeSuccesses = 0;
    uint32_t finalSuccesses = 0;
    uint32_t matchesWithOracle = 0;
    double mcsSum = 0;

    for (uint32_t i = 0; i < m_numTrials; i++)
    {
        auto snrDb = channel.GenerateSnr();
        auto oracleResult = NrSlIdealMcsController::BinarySearch(errorModel,
                                                                 numerology,
                                                                 rv,
                                                                 snrDb,
                                                                 m_targetBler,
                                                                 tbSize,
                                                                 numRbs);
        // If no MCS meets the target, fall back to MCS 0
        uint8_t oracleMcs = oracleResult.value_or(0);

        // This step to be changed in eventual test of ThompsonSampling
        uint8_t selectedMcs = oracleMcs;

        if (selectedMcs == oracleMcs)
        {
            matchesWithOracle++;
        }
        mcsSum += selectedMcs;

        // Evaluate BLER for the selected MCS at the instantaneous SNR
        double tbler =
            errorModel->GetTbler(selectedMcs, numerology, rv, snrDb, tbSize, numRbs, history);

        NS_LOG_DEBUG("Trial " << i << " channel SNR (dB) = " << snrDb << " MCS = " << +selectedMcs
                              << " TBLER " << tbler);
        // Decoding check: Bernoulli draw against BLER
        bool decoded = blerRng->GetValue() > tbler;
        if (decoded)
        {
            decodeSuccesses++;
        }

        // Collision check: Bernoulli draw against collision rate
        bool collided = false;
        if (decoded && m_collisionRate > 0)
        {
            collided = collisionRng->GetValue() < m_collisionRate;
        }

        if (decoded && !collided)
        {
            finalSuccesses++;
        }

        // Eventually, feed result back to controller here.
    }

    // Compute metrics
    double pdr = static_cast<double>(finalSuccesses) / m_numTrials;
    double decodePdr = static_cast<double>(decodeSuccesses) / m_numTrials;
    double meanMcs = mcsSum / m_numTrials;
    double oracleMatchFraction = static_cast<double>(matchesWithOracle) / m_numTrials;

    NS_LOG_INFO("Trials: " << m_numTrials << " decoded: " << decodeSuccesses
                           << " final count: " << finalSuccesses << " decode PDR: " << decodePdr
                           << " final PDR: " << pdr << " Mean MCS: " << meanMcs
                           << " fraction of oracle matches: " << oracleMatchFraction);

    // The oracle selects the highest MCS with BLER <= targetBler, so
    // the actual BLER is at most targetBler.  The expected PDR is therefore
    // at least (1 - targetBler) * (1 - collisionRate).  We allow a tolerance
    // for statistical variation.
    double expectedMinPdr = (1.0 - m_targetBler) * (1.0 - m_collisionRate);
    double tolerance = 0.05;
    NS_TEST_ASSERT_MSG_GT_OR_EQ(pdr,
                                expectedMinPdr - tolerance,
                                "PDR " << pdr << " below expected minimum "
                                       << (expectedMinPdr - tolerance));
    NS_TEST_ASSERT_MSG_EQ_TOL(oracleMatchFraction, 1.0, 1e-9, "Oracle should always match itself");
}

/**
 * \brief Test case for the Thompson Sampling algorithm.
 *
 * Uses the same test loop as the oracle test, but replaces the oracle
 * MCS selection with the Thompson Sampling controller.  The controller
 * receives decode feedback (HARQ ACK/NACK equivalent) and updates its
 * Beta posteriors accordingly.  The oracle MCS is still computed for
 * comparison metrics.
 */
class NrSlThompsonSamplingAlgorithmTest : public TestCase
{
  public:
    /**
     * \brief Constructor
     * \param name test case name
     * \param losSnrDb mean SNR in LOS state
     * \param nlosSnrDb mean SNR in NLOS state
     * \param fadingStdDevDb fading standard deviation
     * \param transitionProb per-trial LOS/NLOS transition probability
     * \param collisionRate per-trial collision probability
     * \param targetBler target block error rate for oracle MCS selection
     * \param numTrials number of trials to run
     */
    NrSlThompsonSamplingAlgorithmTest(const std::string& name,
                                      double losSnrDb,
                                      double nlosSnrDb,
                                      double fadingStdDevDb,
                                      double transitionProb,
                                      double collisionRate,
                                      double targetBler,
                                      uint32_t numTrials)
        : TestCase(name),
          m_losSnrDb(losSnrDb),
          m_nlosSnrDb(nlosSnrDb),
          m_fadingStdDevDb(fadingStdDevDb),
          m_transitionProb(transitionProb),
          m_collisionRate(collisionRate),
          m_targetBler(targetBler),
          m_numTrials(numTrials)
    {
    }

  private:
    void DoRun() override;
    void DoSetup() override;
    void DoTeardown() override;

    uint32_t m_rngSeedCache{1}; //!< Cached value of RngSeed
    uint64_t m_rngRunCache{1};  //!< Cached value of RngRun

    double m_losSnrDb;       //!< Mean SNR in LOS state
    double m_nlosSnrDb;      //!< Mean SNR in NLOS state
    double m_fadingStdDevDb; //!< Fading standard deviation
    double m_transitionProb; //!< Per-trial state transition probability
    double m_collisionRate;  //!< Per-trial collision probability
    double m_targetBler;     //!< Target BLER for oracle
    uint32_t m_numTrials;    //!< Number of trials
};

void
NrSlThompsonSamplingAlgorithmTest::DoSetup()
{
    NS_LOG_FUNCTION(this);
    m_rngSeedCache = RngSeedManager::GetSeed();
    m_rngRunCache = RngSeedManager::GetRun();
    Config::SetGlobal("RngSeed", UintegerValue(1));
    Config::SetGlobal("RngRun", UintegerValue(1));
}

void
NrSlThompsonSamplingAlgorithmTest::DoTeardown()
{
    NS_LOG_FUNCTION(this);
    Config::SetGlobal("RngSeed", UintegerValue(m_rngSeedCache));
    Config::SetGlobal("RngRun", UintegerValue(m_rngRunCache));
}

void
NrSlThompsonSamplingAlgorithmTest::DoRun()
{
    NS_LOG_FUNCTION(this);

    auto errorModel = CreateObject<NrSlEpaErrorModel>();
    ChannelSimulator channel(m_losSnrDb, m_nlosSnrDb, m_fadingStdDevDb, m_transitionProb);

    auto controller = CreateObject<NrSlThompsonSamplingMcsController>();
    controller->SetNumerology(0);
    controller->SetAttribute("DiscountFactor", DoubleValue(1.0));
    controller->SetAttribute("TargetBler", DoubleValue(m_targetBler));

    // Create RNGs for BLER and collision evaluation
    auto blerRng = CreateObject<UniformRandomVariable>();
    auto collisionRng = CreateObject<UniformRandomVariable>();

    // Assign deterministic RNG streams
    int64_t stream = 1;
    stream += channel.AssignStreams(stream);
    blerRng->SetStream(stream++);
    collisionRng->SetStream(stream++);
    stream += controller->AssignStreams(stream);

    // Fixed parameters for GetTbler (EPA error model ignores tbSize)
    const uint8_t numerology = 0;
    const uint8_t rv = 0;
    const uint32_t tbSize = 100;
    const uint32_t numRbs = 10;
    NrErrorModel::NrErrorModelHistory history;
    const uint32_t dstL2Id = 1;

    // Metrics
    uint32_t decodeSuccesses = 0;
    uint32_t finalSuccesses = 0;
    uint32_t matchesWithOracle = 0;
    double mcsSum = 0;

    for (uint32_t i = 0; i < m_numTrials; i++)
    {
        auto snrDb = channel.GenerateSnr();
        auto oracleResult = NrSlIdealMcsController::BinarySearch(errorModel,
                                                                 numerology,
                                                                 rv,
                                                                 snrDb,
                                                                 m_targetBler,
                                                                 tbSize,
                                                                 numRbs);
        uint8_t oracleMcs = oracleResult.value_or(0);

        auto selectedParams = controller->GetGrantParams(dstL2Id, SidelinkInfo::CastType::Unicast);
        NS_ASSERT_MSG(selectedParams.mcs.has_value(), "GetGrantParams returned nullopt for mcs");
        uint8_t selectedMcs = selectedParams.mcs.value();

        if (selectedMcs == oracleMcs)
        {
            matchesWithOracle++;
        }
        mcsSum += selectedMcs;

        // Evaluate BLER for the selected MCS at the instantaneous SNR
        double tbler =
            errorModel->GetTbler(selectedMcs, numerology, rv, snrDb, tbSize, numRbs, history);

        NS_LOG_DEBUG("Trial " << i << " channel SNR (dB) = " << snrDb << " MCS = " << +selectedMcs
                              << " TBLER " << tbler);
        // Decoding check: Bernoulli draw against BLER
        bool decoded = blerRng->GetValue() > tbler;
        if (decoded)
        {
            decodeSuccesses++;
        }

        // Collision check: Bernoulli draw against collision rate
        bool collided = false;
        if (decoded && m_collisionRate > 0)
        {
            collided = collisionRng->GetValue() < m_collisionRate;
        }

        if (decoded && !collided)
        {
            finalSuccesses++;
        }

        // Feed decode outcome back to the controller (HARQ ACK/NACK equivalent)
        controller->RecordOutcome(dstL2Id, selectedMcs, decoded);
    }

    // Compute metrics
    double pdr = static_cast<double>(finalSuccesses) / m_numTrials;
    double decodePdr = static_cast<double>(decodeSuccesses) / m_numTrials;
    double meanMcs = mcsSum / m_numTrials;
    double oracleMatchFraction = static_cast<double>(matchesWithOracle) / m_numTrials;

    NS_LOG_INFO("TS Trials: " << m_numTrials << " decoded: " << decodeSuccesses
                              << " final count: " << finalSuccesses << " decode PDR: " << decodePdr
                              << " final PDR: " << pdr << " Mean MCS: " << meanMcs
                              << " fraction of oracle matches: " << oracleMatchFraction);

    // Thompson Sampling should achieve reasonable PDR; the algorithm explores so it
    // will not match the oracle, but it should be well above random MCS selection
    NS_TEST_ASSERT_MSG_GT_OR_EQ(pdr, 0.4, "PDR " << pdr << " below minimum threshold 0.4");
    NS_TEST_ASSERT_MSG_GT(meanMcs, 0.0, "Mean MCS should be above zero");
}

/**
 * \brief Test suite for the Thompson Sampling MCS controller.
 */
class NrSlThompsonSamplingMcsControllerTestSuite : public TestSuite
{
  public:
    NrSlThompsonSamplingMcsControllerTestSuite()
        : TestSuite("nr-sl-thompson-sampling-mcs-controller", Type::UNIT)
    {
        // Constant SNR, no collision: baseline framework validation
        AddTestCase(new NrSlThompsonSamplingMcsControllerTest("OracleConstantSnr",
                                                              10,    // losSnrDb
                                                              10,    // nlosSnrDb
                                                              0,     // fadingStdDevDb
                                                              0,     // transitionProb
                                                              0,     // collisionRate
                                                              0.1,   // targetBler
                                                              1000), // numTrials
                    Duration::QUICK);

        // Constant SNR with collision: verify that collision model reduces PDR
        AddTestCase(new NrSlThompsonSamplingMcsControllerTest("OracleConstantSnrCollision",
                                                              10,    // losSnrDb
                                                              10,    // nlosSnrDb
                                                              0,     // fadingStdDevDb
                                                              0,     // transitionProb
                                                              0.1,   // collisionRate
                                                              0.1,   // targetBler
                                                              1000), // numTrials
                    Duration::QUICK);

        // Fading channel, no collision: oracle adapts per trial
        AddTestCase(new NrSlThompsonSamplingMcsControllerTest("OracleFading",
                                                              10,    // losSnrDb
                                                              10,    // nlosSnrDb
                                                              3,     // fadingStdDevDb
                                                              0,     // transitionProb
                                                              0,     // collisionRate
                                                              0.1,   // targetBler
                                                              1000), // numTrials
                    Duration::QUICK);

        // Two-state Markov channel: oracle adapts to state transitions
        AddTestCase(new NrSlThompsonSamplingMcsControllerTest("OracleMarkov",
                                                              15,    // losSnrDb
                                                              5,     // nlosSnrDb
                                                              2,     // fadingStdDevDb
                                                              0.02,  // transitionProb
                                                              0,     // collisionRate
                                                              0.1,   // targetBler
                                                              2000), // numTrials
                    Duration::QUICK);

        // Two-state Markov with collisions
        AddTestCase(new NrSlThompsonSamplingMcsControllerTest("OracleMarkovCollision",
                                                              15,    // losSnrDb
                                                              5,     // nlosSnrDb
                                                              2,     // fadingStdDevDb
                                                              0.02,  // transitionProb
                                                              0.1,   // collisionRate
                                                              0.1,   // targetBler
                                                              2000), // numTrials
                    Duration::QUICK);

        // --- Thompson Sampling algorithm tests ---

        // Initial constant SNR test.
        // All arms start with Beta(1,1) = Uniform(0,1) posteriors.
        // Early MCS selections will often be drawn from high MCS values and will
        // exceed the 0.9 success threshold (1 - targetBler) based on their naive
        // posterior distributions, biasing initial selections toward high MCS values.
        // These selections will fail, shifting their posteriors toward zero in a
        // top-down elimination pattern.  The algorithm converges around the optimal
        // MCS (5 at 10 dB SNR with this error model) because the selection always
        // takes the highest MCS whose sampled theta exceeds the threshold, so it
        // naturally prefers the most aggressive viable MCS.  Occasional exploration
        // spikes occur when a higher MCS arm produces a sample above the threshold.
        // Lower MCS arms retain flat posteriors (rarely selected, but always viable).
        AddTestCase(new NrSlThompsonSamplingAlgorithmTest("TSConstantSnr",
                                                          10,    // losSnrDb
                                                          10,    // nlosSnrDb
                                                          0,     // fadingStdDevDb
                                                          0,     // transitionProb
                                                          0,     // collisionRate
                                                          0.1,   // targetBler
                                                          2000), // numTrials
                    Duration::QUICK);

        // Constant SNR with collision losses
        AddTestCase(new NrSlThompsonSamplingAlgorithmTest("TSConstantSnrCollision",
                                                          10,    // losSnrDb
                                                          10,    // nlosSnrDb
                                                          0,     // fadingStdDevDb
                                                          0,     // transitionProb
                                                          0.1,   // collisionRate
                                                          0.1,   // targetBler
                                                          2000), // numTrials
                    Duration::QUICK);

        // Fading channel: TS adapts to varying SNR
        AddTestCase(new NrSlThompsonSamplingAlgorithmTest("TSFading",
                                                          10,    // losSnrDb
                                                          10,    // nlosSnrDb
                                                          3,     // fadingStdDevDb
                                                          0,     // transitionProb
                                                          0,     // collisionRate
                                                          0.1,   // targetBler
                                                          2000), // numTrials
                    Duration::QUICK);

        // Two-state Markov channel: TS tracks state transitions
        AddTestCase(new NrSlThompsonSamplingAlgorithmTest("TSMarkov",
                                                          15,    // losSnrDb
                                                          5,     // nlosSnrDb
                                                          2,     // fadingStdDevDb
                                                          0.02,  // transitionProb
                                                          0,     // collisionRate
                                                          0.1,   // targetBler
                                                          5000), // numTrials
                    Duration::QUICK);

        // Two-state Markov with collisions
        AddTestCase(new NrSlThompsonSamplingAlgorithmTest("TSMarkovCollision",
                                                          15,    // losSnrDb
                                                          5,     // nlosSnrDb
                                                          2,     // fadingStdDevDb
                                                          0.02,  // transitionProb
                                                          0.1,   // collisionRate
                                                          0.1,   // targetBler
                                                          5000), // numTrials
                    Duration::QUICK);
    }
};

/// Static test suite registration
static NrSlThompsonSamplingMcsControllerTestSuite
    g_nrSlThompsonSamplingMcsControllerTestSuite; //!< Test suite instance
