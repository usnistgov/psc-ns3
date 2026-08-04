// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef TRAFFIC_GENERATOR_NGMN_GAMING
#define TRAFFIC_GENERATOR_NGMN_GAMING

#include "traffic-generator.h"

#include <ns3/random-variable-stream.h>

namespace ns3
{

class Address;
class Socket;

/**
 * This class implements a traffic generator for the gaming traffic, and can
 * be configured to generate either donwlink or uplink traffic. Follows traffic
 * gaming models for DL and UL explained in the Annex A of
 * White Paper by the NGMN Alliance.
 */

class TrafficGeneratorNgmnGamingTestCase;

class TrafficGeneratorNgmnGaming : public TrafficGenerator
{
    friend TrafficGeneratorNgmnGamingTestCase;

  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    TrafficGeneratorNgmnGaming();

    ~TrafficGeneratorNgmnGaming() override;

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
    // inherited from Application base class.
    // Called at time specified by Start by the DoInitialize method
    void StartApplication() override;
    /**
     * \brief Generates reading time using exponential distribution
     *
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
     * \brief Generate the initial packet arrival time
     * \return initial packet arrival time
     */
    Time GetInitialPacketArrivalTime() const;
    /**
     * \brief Get the relative time when the next packet should be sent
     * \return the relative time when the next packet will be sent
     */
    Time GetNextPacketTime() const override;

    Ptr<UniformRandomVariable>
        m_initPacketArrivalVariable; //!< Uniform packet arrival random variable for the initial
                                     //!< packet arrive time for downlink
    Ptr<UniformRandomVariable>
        m_packetSizeRandomVariable; //!< Uniform packet size random variable for the packet size
                                    //!< generation for both, downlink and uplink
    Ptr<UniformRandomVariable>
        m_packetArrivalVariable; //!< Uniform packet arrival random variable for the packet arrival
                                 //!< time for downlink
    bool m_isDownlink{
        true}; //!< whether this application will generate downlink or uplink gaming traffic
    uint32_t m_aParamPacketSizeUl{
        0}; //!< "a" parameter used for the packet size generation in uplink
    double m_bParamPacketSizeUl{
        0.0}; //!< "b" parameter used for the packet size generation in uplink
    uint32_t m_aParamPacketSizeDl{
        0}; //!< "a" parameter used for the packet size generation in downlink
    double m_bParamPacketSizeDl{
        0.0}; //!< "b" parameter used for the packet size generation in downlink
    double m_aParamPacketArrivalDl{
        0.0}; //!< "a" parameter used for the packet arrival generation in downlink
    double m_bParamPacketArrivalDl{
        0.0}; //!< "a" parameter used for the packet arrival generation in downlink
    uint32_t m_initialPacketArrivalMin{0}; //!< the minimum value for the initial packet arrival
                                           //!< generation for both, downlink and uplink
    uint32_t m_initialPacketArrivalMax{0}; //!< the maximum value for the initial packet arrival
                                           //!< generation for both, downlink and uplink
    uint32_t m_packetArrivalUl{0}; //!< the packet arrival in uplink in number of milliseconds
};

} // namespace ns3

#endif /* TRAFFIC_GENERATOR_NGMN_GAMING */
