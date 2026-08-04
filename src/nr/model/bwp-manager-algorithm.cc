// Copyright (c) 2018 Natale Patriciello <natale.patriciello@gmail.com>
//
// SPDX-License-Identifier: GPL-2.0-only

#include "bwp-manager-algorithm.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("BwpManagerAlgorithm");
NS_OBJECT_ENSURE_REGISTERED(BwpManagerAlgorithm);

TypeId
BwpManagerAlgorithm::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::BwpManagerAlgorithm").SetParent<ObjectBase>().SetGroupName("nr");
    return tid;
}

NS_OBJECT_ENSURE_REGISTERED(BwpManagerAlgorithmStatic);

#define DECLARE_ATTR(NAME, DESC, GETTER, SETTER)                                                   \
    .AddAttribute(NAME,                                                                            \
                  DESC,                                                                            \
                  UintegerValue(0),                                                                \
                  MakeUintegerAccessor(&BwpManagerAlgorithmStatic::GETTER,                         \
                                       &BwpManagerAlgorithmStatic::SETTER),                        \
                  MakeUintegerChecker<uint8_t>(0, 5))

TypeId
BwpManagerAlgorithmStatic::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::BwpManagerAlgorithmStatic")
            .SetParent<BwpManagerAlgorithm>()
            .SetGroupName("nr")
            .AddConstructor<BwpManagerAlgorithmStatic>() DECLARE_ATTR(
                "GBR_CONV_VOICE",
                "BWP index to which flows of this Qci type should be forwarded.",
                GetConvVoiceBwp,
                SetConvVoiceBwp) DECLARE_ATTR("GBR_CONV_VIDEO",
                                              "BWP index to which flows of GBR_CONV_VIDEO Qci type "
                                              "should be forwarded.",
                                              GetConvVideoBwp,
                                              SetConvVideoBwp) DECLARE_ATTR("GBR_GAMING",
                                                                            "BWP index to which "
                                                                            "flows of GBR_GAMING "
                                                                            "Qci type should be "
                                                                            "forwarded.",
                                                                            GetGamingBwp,
                                                                            SetGamingBwp)
                DECLARE_ATTR(
                    "GBR_NON_CONV_VIDEO",
                    "BWP index to which flows of GBR_NON_CONV_VIDEO Qci "
                    "type should be forwarded.",
                    GetNonConvVideoBwp,
                    SetNonConvVideoBwp) DECLARE_ATTR("GBR_MC_PUSH_TO_TALK",
                                                     "BWP index to which flows of "
                                                     "GBR_MC_PUSH_TO_TALK Qci type should "
                                                     "be forwarded.",
                                                     GetMcPttBwp,
                                                     SetMcPttBwp) DECLARE_ATTR("GBR_NMC_PUSH_TO_"
                                                                               "TALK",
                                                                               "BWP index to which "
                                                                               "flows of "
                                                                               "GBR_NMC_PUSH_TO_"
                                                                               "TALK Qci type "
                                                                               "should be "
                                                                               "forwarded.",
                                                                               GetNmcPttBwp,
                                                                               SetNmcPttBwp)
                    DECLARE_ATTR(
                        "GBR_MC_VIDEO",
                        "BWP index to which flows of GBR_MC_VIDEO "
                        "Qci type should be forwarded.",
                        GetMcVideoBwp,
                        SetMcVideoBwp) DECLARE_ATTR("GBR_V2X",
                                                    "BWP index to which flows of GBR_V2X Qci type "
                                                    "should be forwarded.",
                                                    GetGbrV2xBwp,
                                                    SetGbrV2xBwp) DECLARE_ATTR("NGBR_IMS",
                                                                               "BWP index to which "
                                                                               "flows of NGBR_IMS "
                                                                               "Qci type should be "
                                                                               "forwarded.",
                                                                               GetImsBwp,
                                                                               SetImsBwp)
                        DECLARE_ATTR("NGBR_VIDEO_TCP_OPERATOR",
                                     "BWP index to which flows of NGBR_VIDEO_TCP_OPERATOR Qci type "
                                     "should be forwarded.",
                                     GetVideoTcpOpBwp,
                                     SetVideoTcpOpBwp) DECLARE_ATTR("NGBR_VOICE_VIDEO_GAMING",
                                                                    "BWP index to which flows of "
                                                                    "NGBR_VOICE_VIDEO_GAMING Qci "
                                                                    "type should be forwarded.",
                                                                    GetVideoGamingBwp,
                                                                    SetVideoGamingBwp)
                            DECLARE_ATTR(
                                "NGBR_VIDEO_TCP_PREMIUM",
                                "BWP index to which flows of NGBR_VIDEO_TCP_PREMIUM Qci "
                                "type should be forwarded.",
                                GetVideoTcpPremiumBwp,
                                SetVideoTcpPremiumBwp) DECLARE_ATTR("NGBR_VIDEO_TCP_DEFAULT",
                                                                    "BWP index to which flows of "
                                                                    "NGBR_VIDEO_TCP_DEFAULT "
                                                                    "Qci type should be forwarded.",
                                                                    GetVideoTcpDefaultBwp,
                                                                    SetVideoTcpDefaultBwp)
                                DECLARE_ATTR(
                                    "NGBR_MC_DELAY_SIGNAL",
                                    "BWP index to which flows of NGBR_MC_DELAY_SIGNAL "
                                    "Qci type should be forwarded.",
                                    GetMcDelaySignalBwp,
                                    SetMcDelaySignalBwp) DECLARE_ATTR("NGBR_MC_DATA",
                                                                      "BWP index to which flows of "
                                                                      "NGBR_MC_DATA Qci "
                                                                      "type should be forwarded.",
                                                                      GetMcDataBwp,
                                                                      SetMcDataBwp)
                                    DECLARE_ATTR(
                                        "NGBR_V2X",
                                        "BWP index to which flows of NGBR_V2X "
                                        "Qci type should be forwarded.",
                                        GetNgbrV2xBwp,
                                        SetNgbrV2xBwp) DECLARE_ATTR("NGBR_LOW_LAT_EMBB",
                                                                    "BWP index to which flows of "
                                                                    "NGBR_LOW_LAT_EMBB Qci type "
                                                                    "should be forwarded.",
                                                                    GetLowLatEmbbBwp,
                                                                    SetLowLatEmbbBwp)
                                        DECLARE_ATTR(
                                            "DGBR_DISCRETE_AUT_SMALL",
                                            "BWP index to which flows of DGBR_DISCRETE_AUT_SMALL "
                                            "Qci "
                                            "type should be forwarded.",
                                            GetDiscreteAutSmallBwp,
                                            SetDiscreteAutSmallBwp) DECLARE_ATTR("DGBR_DISCRETE_"
                                                                                 "AUT_LARGE",
                                                                                 "BWP index to "
                                                                                 "which flows of "
                                                                                 "DGBR_DISCRETE_"
                                                                                 "AUT_LARGE Qci "
                                                                                 "type "
                                                                                 "should be "
                                                                                 "forwarded.",
                                                                                 GetDiscreteAutLargeBwp,
                                                                                 SetDiscreteAutLargeBwp)
                                            DECLARE_ATTR(
                                                "DGBR_ITS",
                                                "BWP index to which flows of DGBR_ITS Qci type "
                                                "should be "
                                                "forwarded.",
                                                GetItsBwp,
                                                SetItsBwp) DECLARE_ATTR("DGBR_ELECTRICITY",
                                                                        "BWP index to which flows "
                                                                        "of DGBR_ELECTRICITY Qci "
                                                                        "type should be forwarded.",
                                                                        GetElectricityBwp,
                                                                        SetElectricityBwp)
                                                DECLARE_ATTR("GBR_LIVE_UL_71",
                                                             "BWP index to which flows of "
                                                             "GBR_LIVE_UL_71 Qci "
                                                             "type should be forwarded.",
                                                             GetLiveUlStream71Bwp,
                                                             SetLiveUlStream71Bwp)
                                                    DECLARE_ATTR("GBR_LIVE_UL_72",
                                                                 "BWP index to which flows of "
                                                                 "GBR_LIVE_UL_72 Qci "
                                                                 "type should be forwarded.",
                                                                 GetLiveUlStream72Bwp,
                                                                 SetLiveUlStream72Bwp)
                                                        DECLARE_ATTR("GBR_LIVE_UL_73",
                                                                     "BWP index to which flows of "
                                                                     "GBR_LIVE_UL_73 Qci "
                                                                     "type should be forwarded.",
                                                                     GetLiveUlStream73Bwp,
                                                                     SetLiveUlStream73Bwp)
                                                            DECLARE_ATTR(
                                                                "GBR_LIVE_UL_74",
                                                                "BWP index to which flows of "
                                                                "GBR_LIVE_UL_74 Qci "
                                                                "type should be forwarded.",
                                                                GetLiveUlStream74Bwp,
                                                                SetLiveUlStream74Bwp)
                                                                DECLARE_ATTR(
                                                                    "GBR_LIVE_UL_76",
                                                                    "BWP index to which flows of "
                                                                    "GBR_LIVE_UL_76 Qci "
                                                                    "type should be forwarded.",
                                                                    GetLiveUlStream76Bwp,
                                                                    SetLiveUlStream76Bwp)
                                                                    DECLARE_ATTR(
                                                                        "DGBR_INTER_SERV_87",
                                                                        "BWP index to which flows "
                                                                        "of DGBR_INTER_SERV_87 Qci "
                                                                        "type should be forwarded.",
                                                                        GetInterService87Bwp,
                                                                        SetInterService87Bwp)
                                                                        DECLARE_ATTR(
                                                                            "DGBR_INTER_SERV_88",
                                                                            "BWP index to which "
                                                                            "flows of "
                                                                            "DGBR_INTER_SERV_88 "
                                                                            "Qci "
                                                                            "type should be "
                                                                            "forwarded.",
                                                                            GetInterService88Bwp,
                                                                            SetInterService88Bwp)
                                                                            DECLARE_ATTR(
                                                                                "DGBR_VISUAL_"
                                                                                "CONTENT_89",
                                                                                "BWP index to "
                                                                                "which flows of "
                                                                                "DGBR_VISUAL_"
                                                                                "CONTENT_89 Qci "
                                                                                "type should be "
                                                                                "forwarded.",
                                                                                GetVisualContent89Bwp,
                                                                                SetVisualContent89Bwp)
                                                                                DECLARE_ATTR(
                                                                                    "DGBR_VISUAL_"
                                                                                    "CONTENT_90",
                                                                                    "BWP index to "
                                                                                    "which flows "
                                                                                    "of "
                                                                                    "DGBR_VISUAL_"
                                                                                    "CONTENT_90 "
                                                                                    "Qci "
                                                                                    "type should "
                                                                                    "be forwarded.",
                                                                                    GetVisualContent90Bwp,
                                                                                    SetVisualContent90Bwp);
    return tid;
}

uint8_t
BwpManagerAlgorithmStatic::GetBwpForEpsBearer(const NrEpsBearer::Qci& v) const
{
    return m_qciToBwpMap.at(v == 0 ? 1 : v);
}

} // namespace ns3
