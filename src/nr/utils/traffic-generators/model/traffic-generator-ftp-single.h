// Copyright (c) 2010 Georgia Institute of Technology
// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef TRAFFIC_GENERATOR_FTP_SINGLE_H
#define TRAFFIC_GENERATOR_FTP_SINGLE_H

#include "traffic-generator.h"

namespace ns3
{

/**
 * File transfer application used to send a single file
 */

class TrafficGeneratorFtpSingle : public TrafficGenerator
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    TrafficGeneratorFtpSingle();

    ~TrafficGeneratorFtpSingle() override;

    /**
     * \brief Set the file size to try to transfer
     *
     * \param fileSize the size of a file to try to transfer
     */
    void SetFileSize(uint32_t fileSize);
    /**
     * \brief Sets the packet size
     */
    void SetPacketSize(uint32_t packetSize);

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
    // overrides
    void DoDispose() override;
    void DoInitialize() override;

  private:
    /**
     * \brief Sets the file size to transfer
     */
    void GenerateNextPacketBurstSize() override;
    /**
     * \brief Get the amount of data to transfer
     * \return the amount of data to transfer
     */
    uint32_t GetNextPacketSize() const override;

    uint32_t m_fileSize{0};   //!< Limit total number of bytes sent
    uint32_t m_packetSize{0}; //!< Size of data to send each time
};

} // namespace ns3

#endif /* TRAFFIC_GENERATOR_FTP_SINGLE_H */
