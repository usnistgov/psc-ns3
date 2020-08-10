/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public service. You may use,
 * copy and distribute copies of the software in any medium, provided that you
 * keep intact this entire notice. You may improve,modify and create derivative
 * works of the software or any portion of the software, and you may copy and
 * distribute such modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and nature of
 * any such change. Please explicitly acknowledge the National Institute of
 * Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
 * AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
 * OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
 * ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF,
 * INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY,
 * OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#ifndef PSC_VIDEO_STREAMING_H
#define PSC_VIDEO_STREAMING_H

#include <ns3/string.h>
#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/application.h>
#include <ns3/random-variable-stream.h>

namespace ns3 {
namespace psc {

/**
 * \ingroup psc
 *
 * Class providing video streaming model based on CDF curves
 */
class PscVideoStreaming : public Application
{
public:
  /**
   * Get the TypeId for this class
   *
   * \return The TypeId for this class
   */
  static TypeId GetTypeId (void);

  /**
   * Default constructor
   */
  PscVideoStreaming ();

  /**
   * Default destructor
   */
  virtual ~PscVideoStreaming ();

  /**
   * Set the receiver information (address and port) for sending packets.
   * The address type must be either Ipv4Address or Ipv6Address
   *
   * \param recvAddress The receiver address (must be Ipv4Address or Ipv6Address)
   * \param recvPort The receiver port
   */
  void SetReceiver (Address recvAddress, uint16_t recvPort);

  /**
   * Reads a custom data distribution for size and interval from the files passed as parameters.
   * Note that these distributions will not be actually used by the application until the
   * LoadCdfs () method is called.
   *
   * \param sizeCdfFilePath Path to the file with the packet size CDF
   * \param intervalCdfFilePath Path to the file with the packet interval CDF
   */
  void ReadCustomDistribution (std::string sizeCdfFilePath, std::string intervalCdfFilePath);

  /**
   * Loads the CDFs files
   */
  void LoadCdfs (void);

protected:
  /**
   * Clean up the instance being removed
   */
  void DoDispose (void);

private:
  /**
   * Variable with the last packet ID generated
   */
  uint32_t m_sequenceNumber;

  /**
   * Starts the application
   */
  virtual void StartApplication (void);

  /**
   * Stops the application
   */
  virtual void StopApplication (void);

  /**
   * Sends the next video codec packet. If the packet size exceeds m_maxUdpPayloadSize bytes, the
   * packet is segmented in multiple UDP packets.
   */
  void Send (void);

  /**
   * Setter for the distribution attribute. This will allow to swap custom and
   * default distributions easily
   *
   * \param distributionName The name of the data distribution for the model
   */
  void SetDistributionName (std::string distributionName);

  /**
   * Getter for the distribution attribute.
   *
   * \return The name of the data distribution
   */
  std::string GetDistributionName (void) const;

  /**
   * Video Streaming Model data distribution,
   * from psc-video-streaming-distributions
   */
  std::string m_distributionName;

  /**
   * CDF values of packet size
   */
  Ptr<EmpiricalRandomVariable> m_sizeErv;

  /**
   * CDF values of packet interval
   */
  Ptr<EmpiricalRandomVariable> m_intervalErv;

  /**
   * CDF values of packet size during boost
   */
  Ptr<EmpiricalRandomVariable> m_sizeErvBoost;

  /**
   * CDF values of packet interval during boost
   */
  Ptr<EmpiricalRandomVariable> m_intervalErvBoost;

  /**
   * Address of the destination node
   */
  Address m_receiverAddress;

  /**
   * Port of the destination node
   */
  uint16_t m_receiverPort;

  /**
   * Socket for sending packets
   */
  Ptr<Socket> m_socket;

  /**
   * Length of the 'boost' period, expressed as the amount of
   * packets that will be transmitted under 'boost' conditions
   */
  uint32_t m_boostPacketCount;

  /**
   * Amount of packets pending to transmit in order to end the
   * 'boost' period
   */
  uint32_t m_boostPacketsLeft;

  /**
   * CDF percentile (scale from 0 to 100) from which the values are
   * generated during 'boost' time. A value of '99' means that during
   * 'boost' time the size and interval generated will be from the
   * top 1 % for size and bottom 1 % for inter-packet interval
   */
  double m_boostPercentile;

  /**
   * Maximum size of generated UDP payloads. This value has to be greater
   * than the serialized size of the SeqTsSizeHeader (12 Bytes), and smaller
   * or equal to 65500 Bytes.
   *
   * If the application generates a packet larger than this size, it will be
   * fragmented at the application level in fragments of up to this size
   * (including the SeqTsSizeHeader)
   *
   * By default this value is 1500 Bytes, to mimic the default behavior observed
   * in ffmpeg for RTP streams. When generating new streaming models based on 
   * ffmpeg, the value can be adjusted to take into account the underlying
   * network. (https://lists.ffmpeg.org/pipermail/ffmpeg-user/2014-September/023233.html)
   */
  uint16_t m_maxUdpPayloadSize;

  /**
   * Map with the data distribution for the packet sizes
   * These values have been loaded from the source (built-in distributions
   * or custom files), but are not used until LoadDistributions () is called
   */
  std::map <double, uint32_t> m_sizeDistribution;

  /**
   * Map with the data distribution for the packet intervals (double type in ms)
   * These values have been loaded from the source (built-in distributions
   * or custom files), but are not used until LoadDistributions () is called
   */
  std::map <double, double> m_intervalDistribution;

  /**
   * Trace for packets sent
   */
  TracedCallback <Ptr <const Packet> > m_txTrace;

  /**
   * Callback format for packets sent
   */
  typedef void (* TxTracedCallback) (Ptr <const Packet>);
}; // class PscVideoStreaming

} // namespace psc
} // namespace ns3

#endif // PSC_VIDEO_STREAMING_H
