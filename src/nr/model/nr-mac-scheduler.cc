// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler.h"

#include "nr-mac-csched-sap.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacScheduler");
NS_OBJECT_ENSURE_REGISTERED(NrMacScheduler);

class NrMacGeneralCschedSapProvider : public NrMacCschedSapProvider
{
  public:
    NrMacGeneralCschedSapProvider() = delete;

    NrMacGeneralCschedSapProvider(NrMacScheduler* scheduler)
        : m_scheduler(scheduler)
    {
    }

    ~NrMacGeneralCschedSapProvider() override = default;

    // inherited from NrMacCschedSapProvider
    void CschedCellConfigReq(
        const NrMacCschedSapProvider::CschedCellConfigReqParameters& params) override
    {
        m_scheduler->DoCschedCellConfigReq(params);
    }

    void CschedUeConfigReq(
        const NrMacCschedSapProvider::CschedUeConfigReqParameters& params) override
    {
        m_scheduler->DoCschedUeConfigReq(params);
    }

    void CschedLcConfigReq(
        const NrMacCschedSapProvider::CschedLcConfigReqParameters& params) override
    {
        m_scheduler->DoCschedLcConfigReq(params);
    }

    void CschedLcReleaseReq(
        const NrMacCschedSapProvider::CschedLcReleaseReqParameters& params) override
    {
        m_scheduler->DoCschedLcReleaseReq(params);
    }

    void CschedUeReleaseReq(
        const NrMacCschedSapProvider::CschedUeReleaseReqParameters& params) override
    {
        m_scheduler->DoCschedUeReleaseReq(params);
    }

  private:
    NrMacScheduler* m_scheduler{nullptr};
};

class NrMacGeneralSchedSapProvider : public NrMacSchedSapProvider
{
  public:
    NrMacGeneralSchedSapProvider() = delete;

    NrMacGeneralSchedSapProvider(NrMacScheduler* sched)
        : m_scheduler(sched)
    {
    }

    void SchedDlRlcBufferReq(
        const NrMacSchedSapProvider::SchedDlRlcBufferReqParameters& params) override
    {
        m_scheduler->DoSchedDlRlcBufferReq(params);
    }

    void SchedDlTriggerReq(
        const NrMacSchedSapProvider::SchedDlTriggerReqParameters& params) override
    {
        m_scheduler->DoSchedDlTriggerReq(params);
    }

    void SchedUlTriggerReq(
        const NrMacSchedSapProvider::SchedUlTriggerReqParameters& params) override
    {
        m_scheduler->DoSchedUlTriggerReq(params);
    }

    void SchedDlCqiInfoReq(
        const NrMacSchedSapProvider::SchedDlCqiInfoReqParameters& params) override
    {
        m_scheduler->DoSchedDlCqiInfoReq(params);
    }

    void SchedUlCqiInfoReq(
        const NrMacSchedSapProvider::SchedUlCqiInfoReqParameters& params) override
    {
        m_scheduler->DoSchedUlCqiInfoReq(params);
    }

    void SchedUlMacCtrlInfoReq(
        const NrMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params) override
    {
        m_scheduler->DoSchedUlMacCtrlInfoReq(params);
    }

    void SchedUlSrInfoReq(const SchedUlSrInfoReqParameters& params) override
    {
        m_scheduler->DoSchedUlSrInfoReq(params);
    }

    void SchedSetMcs(uint32_t mcs) override
    {
        m_scheduler->DoSchedSetMcs(mcs);
    }

    void SchedDlRachInfoReq(const SchedDlRachInfoReqParameters& params) override
    {
        m_scheduler->DoSchedDlRachInfoReq(params);
    }

    uint8_t GetDlCtrlSyms() const override
    {
        return m_scheduler->GetDlCtrlSyms();
    }

    uint8_t GetUlCtrlSyms() const override
    {
        return m_scheduler->GetUlCtrlSyms();
    };

  private:
    NrMacScheduler* m_scheduler{nullptr};
};

TypeId
NrMacScheduler::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrMacScheduler").SetParent<Object>();

    return tid;
}

NrMacScheduler::NrMacScheduler()
{
    NS_LOG_FUNCTION_NOARGS();
    m_macSchedSapProvider = new NrMacGeneralSchedSapProvider(this);
    m_macCschedSapProvider = new NrMacGeneralCschedSapProvider(this);
}

NrMacScheduler::~NrMacScheduler()
{
    NS_LOG_FUNCTION_NOARGS();
    delete m_macSchedSapProvider;
    m_macSchedSapProvider = nullptr;

    delete m_macCschedSapProvider;
    m_macCschedSapProvider = nullptr;
}

} // namespace ns3
