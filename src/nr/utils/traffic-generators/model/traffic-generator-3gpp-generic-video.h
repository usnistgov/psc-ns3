// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef TRAFFIC_GENERATOR_3GPP_GENERIC_VIDEO
#define TRAFFIC_GENERATOR_3GPP_GENERIC_VIDEO

#include "traffic-generator.h"

#include <ns3/random-variable-stream.h>

namespace ns3
{

/**
 * This class implements the 3GPP 2 stream traffic model composed of video
 * according to 3GPP TR 38.838 V17.0.0 (2021-12) document, 5.1.1.
 */

class TrafficGenerator3gppGenericVideo : public TrafficGenerator
{
  public:
    /**
     * \brief Different loopback algorithm types
     */
    enum LoopbackAlgType
    {
        ADJUST_IPA_TIME,
        ADJUST_PACKET_SIZE,
        ADJUST_PACKET_SIZE_UP_AGG,
        ADJUST_FPS,
        WO
    };

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    TrafficGenerator3gppGenericVideo();
    ~TrafficGenerator3gppGenericVideo() override;

    /*
     * \brief Get loopback algorithm type
     */
    TrafficGenerator3gppGenericVideo::LoopbackAlgType GetLoopbackAlgType() const;

    /**
     *\brief Set loopback algorithm type
     */
    void SetLoopbackAlgType(LoopbackAlgType loopbackAlgType);

    void ReceiveLoopbackInformation(double packetLoss,
                                    uint32_t packetReceived,
                                    double windowInSeconds,
                                    Time packetDelay,
                                    Time packetDelayJitter);

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

    /// Traced Callback Updated traffic parameters after the loopback adaptation
    // time, peer port, data rate, fps, mean packet size, reported packet loss, delay, delay jitter
    typedef TracedCallback<Time, uint16_t, double, uint32_t, double, double, Time, Time>
        ParamsTracedCallback;
    ParamsTracedCallback m_paramsTrace;

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

    LoopbackAlgType m_loopbackAlgType;      //!< the loopback algorithm type
    double m_dataRate{0.0};                 //!< the data rate in Mbps
    double m_fps{0.0};                      //!< the frame rate per second
    double m_minDataRate{0.0};              //!<  the min data rate in Mbps
    double m_maxDataRate{0.0};              //!<  the max data rate in Mbps
    double m_minFps{0.0};                   //!< the min frame rate per second
    double m_maxFps{0.0};                   //!< the max frame rate per second
    Ptr<NormalRandomVariable> m_packetSize; //!< the packet size random variable that is configured
                                            //!< based on the desired frame rate and data rate
    Ptr<NormalRandomVariable> m_packetJitter; //!< the packet jitter random variable
    double m_meanPacketSize{
        0.0}; //!< the mean value See Table 5.1.1.1-1 of 3GPP TR 38.838 V17.0.0 (2021-12)
    double m_stdRatioPacketSize{0.0}; //!< STD ratio wrt to the mean value. See Table 5.1.1.1-1 of
                                      //!< 3GPP TR 38.838 V17.0.0 (2021-12)
    double m_minRatioPacketSize{0.0}; //!< min value ratio wrt to the mean value. See
                                      //!< Table 5.1.1.1-1 of 3GPP TR 38.838 V17.0.0 (2021-12)
    double m_maxRatioPacketSize{0.0}; //!< max value ratio wrt to the max value. See Table 5.1.1.1-1
                                      //!< of 3GPP TR 38.838 V17.0.0 (2021-12)
    double m_meanJitter{0.0}; //!< the mean value of the packet arrival jitter. See Table 5.1.1.2-1
                              //!< of 3GPP TR 38.838 V17.0.0 (2021-12)
    double m_stdJitter{0.0}; //!< the STD value of the packet arrival jitter. See Table 5.1.1.2-1 of
                             //!< 3GPP TR 38.838 V17.0.0 (2021-12)
    double m_boundJitter{0.0}; //!< the bound value of the packet arrival jitter. See
                               //!< Table 5.1.1.2-1 of 3GPP TR 38.838 V17.0.0 (2021-12)
    // adjust data rate (packet size) parameters:
    double m_lowerThresholdForDecreasingSlowly{
        0.0}; //< the lower bound for decreasing the video traffic volume slowly
    double m_lowerThresholdForDecreasingQuickly{
        0.0}; //!< the lower bound for decreasing the video traffic volume quickly
    double m_upperThresholdForIncreasing{
        0.0}; //!< until this value the video traffic volume can be increased
    double m_increaseDataRateMultiplier{
        0.0}; //!< the multiplier when increasing the data rate, e.g., 3 to increase 3 times
    double m_decreaseDataRateSlowlyMultiplier{
        0.0}; //!< the multiplier when decreasing the data rate slowly, e.g, 0.75, to decrease 25%
    double m_decreaseDataRateQuicklyMultiplier{0.0}; //!< the multiplier when decreaseing the data
                                                     //!< rate quickly, e.g. 0.2 to decrease 5 times
    uint16_t m_port{0};                              //!< the port of the peer node
};

} // namespace ns3

#endif /* TRAFFIC_GENERATOR_3GPP_GENERIC_VIDEO */
