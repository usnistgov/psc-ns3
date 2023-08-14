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

#ifndef PSC_VIDEO_STREAMING_DISTRIBUTIONS_H
#define PSC_VIDEO_STREAMING_DISTRIBUTIONS_H

#include <ns3/string.h>
#include <ns3/ptr.h>

namespace ns3 {
namespace psc {

/**
 * \ingroup psc
 *
 * Class providing data distributions for the video streaming model based on
 * CDF curves. These CDFs were obtained by streaming videos using ffmpeg using
 * RTP and CRF 25, and collecting information about the packets generated at the
 * application level (after encoding but before network fragmentation). The raw
 * CDFs obtained were simplified to have a constant probability step in such a
 * way that at least 95 % of the points are at most 5 % off when compared with
 * the original value.
 */
class PscVideoStreamingDistributions
{
public:
  /**
   * Get the distribution of packet sizes corresponding to the distribution
   * name passed as a parameter.
   *
   * If the model does not recognize the name provided, it will abort the simulation
   *
   * \param distributionName The name of the distribution
   * \return The distribution of packet sizes
   */
  static std::map <double, uint32_t> GetSizeDistribution (std::string distributionName);

  /**
   * Get the distribution of packet intervals corresponding to the distribution
   * name passed as a parameter.
   *
   * If the model does not recognize the name provided, it will abort the simulation
   *
   * \param distributionName The name of the distribution
   * \return The distribution of packet intervals
   */
  static std::map <double, double> GetIntervalDistribution (std::string distributionName);

}; // class PscVideoStreamingDistributions

} // namespace psc
} // namespace ns3

#endif // PSC_VIDEO_STREAMING_DISTRIBUTIONS_H
