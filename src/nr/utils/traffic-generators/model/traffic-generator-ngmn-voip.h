// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef TRAFFIC_GENERATOR_NGMN_VOIP
#define TRAFFIC_GENERATOR_NGMN_VOIP

#include "traffic-generator.h"

#include <ns3/random-variable-stream.h>

namespace ns3
{

class Address;
class Socket;

/**
 * This class implements a traffic generator for the VoIP traffic.
 * Follows the traffic gaming model for VOIP described in the Annex B of
 * White Paper by the NGMN Alliance.
 *
 * Basically, according to the NGMN document, the VOIP traffic can be
 * modeled as a simple 2-state voice activity model. The states are:
 *      - Inactive State
 *      - Active State
 *
 * In the model, the probability of transitioning from state 1 (the active
 * speech state) to state 0 (the inactive or silent state) while in state
 * 1 is equal to "a", while the probability of transitioning from state 0 to
 * state 1 while in state 0 is "c".
 * The model is assumed updated at the speech encoder frame rate R=1/T,
 * where T is the encoder frame duration (typically, 20ms).
 *
 * Clearly, a 2-state model is extremely simplistic, and many more complex
 * models are available. However, it is amenable to rapid analysis and
 * initial estimation of talk spurt arrival statistics and hence reservation activity/
 * The main purpose of this traffic model is not to favour any codec but to specify a
 * model to obtain results which are comparable.
 */

class TrafficGeneratorNgmnVoip : public TrafficGenerator
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    TrafficGeneratorNgmnVoip();

    ~TrafficGeneratorNgmnVoip() override;

    enum VoipState
    {
        INACTIVE_STATE,
        ACTIVE_STATE
    };

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model. Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream) override;

  protected:
    void DoDispose() override;
    void DoInitialize() override;

  private:
    /*
     * \brief Inherited from Application base class.
     *  In its implementation it calculates the transition probabilities between
     *  active and inactive VOIP states
     *  // Called at time specified by Start by the DoInitialize method
     */
    void StartApplication() override;
    void StopApplication() override;

    /*
     * \brief Updates the model state (ACTIVE/INACTIVE).
     * The model is assumed updated at the speech encoder frame rate R=1/T,
     * where T is the encoder frame duration (typically, 20ms)
     */
    void UpdateState();

    /**
     * \brief Generates the packet burst size in bytes
     */
    void GenerateNextPacketBurstSize() override;

    /**
     * \brief Get the amount of data to transfer
     * \return the amount of data to transfer
     */
    uint32_t GetNextPacketSize() const override;

    /**
     * \brief Get the relative time when the next packet should be sent
     * \return the relative time when the next packet will be sent
     */
    Time GetNextPacketTime() const override;

    Ptr<UniformRandomVariable> m_fromActiveToInactive;
    Ptr<UniformRandomVariable> m_fromInactiveToActive;

    uint32_t m_encoderFrameLength{0};    //!< The encoder frame length in ms
    uint32_t m_meanTalkSpurtDuration{0}; //!< A mean talk spurt duration in ms
    double m_voiceActivityFactor{0.0};   //!< The voice activity factor [0,1]
    uint32_t m_activePayload{0};         //!< Active payload size in bytes
    uint32_t m_SIDPeriodicity{0};        //!< SID periodicity in milliseconds
    uint32_t m_SIDPayload{0};            //!<  The SID payload size in the number of bytes
    VoipState m_state{INACTIVE_STATE};   //!< Voip application state
    EventId m_updateState;               //! saves the event for the next update of the state
    double m_a{0.0}; //!< The probability of transitioning from the active to the inactive state
    double m_c{0.0}; //!< The probability of transitioning from the inactive to the active state
    bool m_sidEnabled{true}; //!< Flag to enable or disable sending SIDs during inactive state
};

} // namespace ns3

#endif /* TRAFFIC_GENERATOR_NGMN_VOIP */
