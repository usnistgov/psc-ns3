// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef TRAFFIC_GENERATOR_3GPP_POSE_CONTROL
#define TRAFFIC_GENERATOR_3GPP_POSE_CONTROL

#include "traffic-generator.h"

#include <ns3/random-variable-stream.h>

namespace ns3
{

/**
 * This class implements the 3GPP pose/control traffic model generator according to
 *  3GPP TR 38.838 V17.0.0 (2021-12) document, sec 5.2.
 */

class TrafficGenerator3gppPoseControl : public TrafficGenerator
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TrafficGenerator3gppPoseControl();
    ~TrafficGenerator3gppPoseControl() override;

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model. Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream) override;

  private:
    // inherited from Application base class.
    //  Called at time specified by Start by the DoInitialize method
    void StartApplication() override;
    /**
     * \brief Should not be called for pose/control traffic model, no specific event to be handled
     */
    void PacketBurstSent() override;
    /**
     * \brief Generates the packet burst size in packets
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

    uint32_t m_packetSize{0};  //!< packet size
    uint32_t m_periodicity{0}; //!< the periodicity in milliseconds
};

} // namespace ns3

#endif /* TRAFFIC_GENERATOR_3GPP_POSE_CONTROL */
