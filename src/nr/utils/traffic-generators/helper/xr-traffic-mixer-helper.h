// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef XR_TRAFFIC_MIXER_HELPER_H
#define XR_TRAFFIC_MIXER_HELPER_H

#include "traffic-generator-helper.h"

#include "ns3/address.h"
#include "ns3/application-container.h"
#include "ns3/application.h"
#include "ns3/node-container.h"
#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/traffic-generator-3gpp-audio-data.h"
#include "ns3/traffic-generator-3gpp-generic-video.h"
#include "ns3/traffic-generator-3gpp-pose-control.h"
#include "ns3/traffic-generator-ngmn-video.h"
#include "ns3/traffic-generator-ngmn-voip.h"
#include "ns3/traffic-generator.h"

#include <list>
#include <map>

namespace ns3
{

/**
 * \brief Enum that is used to configure the traffic type
 */
enum NrXrConfig
{
    AR_M3,    // AR Model 3
    AR_M3_V2, // AR Model 3 that is using NGMN video instead of 3GPP video
    VR_DL1,   // VR 1 stream
    VR_DL2,   // VR 2 streams
    VR_UL,    // VR uplink
    CG_DL1,   // CG DL 1 stream
    CG_DL2,   // CG UL 1 stream
    CG_UL,    // CG UL 1 stream
    NGMN_VOICE
}; // NGMN voip
/**
 * \brief Return NrXrConfig enum for a corresponding string
 */
enum NrXrConfig GetXrTrafficType(const std::string& item);
/**
 * \brief Returns a string representing NrXrConfig enum
 */
std::string GetXrTrafficName(const NrXrConfig& item);

/**
 * \brief Operator for input from stream to NrXrConfig
 */
static inline std::istream&
operator>>(std::istream& is, NrXrConfig& item)
{
    std::string inputValue;
    is >> inputValue;
    item = GetXrTrafficType(inputValue);
    return is;
}

/**
 * \brief Operator to output to the stream from NrXrConfig enum
 */
static inline std::ostream&
operator<<(std::ostream& os, const NrXrConfig& item)
{
    os << GetXrTrafficName(item);
    return os;
}

extern const std::map<NrXrConfig, std::list<TypeId>> XrPreconfig;

/**
 * \ingroup applications
 * \defgroup traffic TrafficGenerator
 *
 * This traffic mixer can mix various types of traffics.
 */
class XrTrafficMixerHelper : public Object
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    XrTrafficMixerHelper();
    ~XrTrafficMixerHelper() override;
    /*
     * \brief Adds a stream of provided TypeId
     * \param trafficGenerator specifies the type ID of a stream to be added to a mixed traffic flow
     */
    void AddStream(TypeId trafficGenerator);

    /*
     * \brief Configures the configured XR traffic
     * \param xrTrafficType the XR traffic type to be configured
     */
    void ConfigureXr(NrXrConfig xrTrafficType);

    /*
     * \brief Configures the stream mixtures of the traffic types added by
     * function AddStream.
     * \param transportProtocol the transport protocol to be configured
     * \param remoteAddresses the vector of the remote address values toward which will be sent the
     * generated data \param trafficGeneratorNode the traffic generator node container \return the
     * container of the newly created traffic generator applications
     */
    ApplicationContainer Install(std::string transportProtocol,
                                 std::vector<Address>& remoteAddresses,
                                 Ptr<Node> trafficGeneratorNode);

  private:
    std::list<TypeId> m_trafficStreams; //!< the list of traffic stream types to be mixed
};

} // namespace ns3

#endif /* XR_TRAFFIC_MIXER_HELPER_H */
