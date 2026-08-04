// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef TRAFFIC_GENERATOR_3GPP_AUDIO_DATA
#define TRAFFIC_GENERATOR_3GPP_AUDIO_DATA

#include "traffic-generator.h"

#include <ns3/random-variable-stream.h>

namespace ns3
{

/**
 * This class implements the 3GPP 2 stream traffic model composed of video + audio/data streams
 * according to 3GPP TR 38.838 V17.0.0 (2021-12) document, 5.1.2.2 audio/data.
 */

class TrafficGenerator3gppAudioData : public TrafficGenerator
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TrafficGenerator3gppAudioData();
    ~TrafficGenerator3gppAudioData() override;
    /*
     * \brief Function that configures the
     * data rate.
     */
    void SetDataRate(double dataRate);

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
    void DoInitialize() override;

  private:
    // inherited from Application base class.
    // Called at time specified by Start by the DoInitialize method
    void StartApplication() override;
    /**
     * \brief Creates the next event
     */
    void PacketBurstSent() override;
    /**
     * \brief Generates the packet burst size in number of packets
     */
    void GenerateNextPacketBurstSize() override;
    /**
     * \brief Get the size of the next packet in bytes
     * \return the size of the next packet in bytes
     */
    uint32_t GetNextPacketSize() const override;
    /**
     * \brief Get the relative time when the next packet should be sent
     * \return the relative time when the next packet will be sent
     */
    Time GetNextPacketTime() const override;

    double m_dataRate{0.0}; //!< the data rate of audio/data application
    uint32_t m_packetSize{
        0}; //!< the packet size that is calculated based on the configured data rate
    uint32_t m_periodicity{0}; //!< the periodicity in milliseconds
};

} // namespace ns3

#endif /* TRAFFIC_GENERATOR_3GPP_AUDIO_DATA */
