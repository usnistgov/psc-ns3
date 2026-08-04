// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#define NS_LOG_APPEND_CONTEXT                                                                      \
    do                                                                                             \
    {                                                                                              \
        std::clog << " [ CellId " << GetCellId() << ", bwpId " << GetBwpId() << "] ";              \
    } while (false);

#include "nr-mac-scheduler-tdma.h"

#include <ns3/log.h>

#include <algorithm>
#include <functional>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerTdma");
NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulerTdma);

TypeId
NrMacSchedulerTdma::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrMacSchedulerTdma").SetParent<NrMacSchedulerNs3>();
    return tid;
}

NrMacSchedulerTdma::NrMacSchedulerTdma()
{
}

NrMacSchedulerTdma::~NrMacSchedulerTdma()
{
}

std::vector<NrMacSchedulerNs3::UePtrAndBufferReq>
NrMacSchedulerTdma::GetUeVectorFromActiveUeMap(const NrMacSchedulerNs3::ActiveUeMap& activeUes)
{
    std::vector<UePtrAndBufferReq> ueVector;
    for (const auto& el : activeUes)
    {
        uint64_t size = ueVector.size();
        GetSecond GetUeVector;
        for (const auto& ue : GetUeVector(el))
        {
            ueVector.emplace_back(ue);
        }
        NS_ASSERT(size + GetUeVector(el).size() == ueVector.size());
    }
    return ueVector;
}

/**
 * \brief Assign the available RBG in a TDMA fashion
 * \param symAvail Number of available symbols
 * \param activeUe active flows and UE
 * \param type String representing the type of allocation currently in act (DL or UL)
 * \param BeforeSchedFn Function to call before any scheduling is started
 * \param GetCompareFn Function to call to compare UEs during assignment
 * \param GetTBSFn Function to call to get a reference of the UL or DL TBS
 * \param GetRBGFn Function to call to get a reference of the UL or DL RBG
 * \param GetSymFn Function to call to get a reference of the UL or DL symbols
 * \param SuccessfulAssignmentFn Function to call one time for the UE that got the resources
 * assigned in one iteration \param UnSuccessfulAssignmentFn Function to call for the UEs that did
 * not get anything in one iteration
 *
 * \return a map between the beam and the symbols assigned to each one
 *
 * The algorithm redistributes the number of symbols to all the UEs. The
 * pseudocode is the following:
 * <pre>
 * for (ue : activeUe):
 *    BeforeSchedFn (ue);
 *
 * while symbols > 0:
 *    sort (ueVector);
 *    GetRBGFn(ueVector.first()) += BandwidthInRBG();
 *    symbols--;
 *    SuccessfulAssignmentFn (ueVector.first());
 *    for each ue that did not get anything assigned:
 *        UnSuccessfulAssignmentFn (ue);
 * </pre>
 *
 * To sort the UEs, the method uses the function returned by GetUeCompareDlFn().
 * Two fairness helper are hard-coded in the method: the first one is avoid
 * to assign resources to UEs that already have their buffer requirement covered,
 * and the other one is avoid to assign symbols when all the UEs have their
 * requirements covered.
 *
 * The distribution of each symbol is called 'iteration' in other part of the
 * class documentation.
 *
 * The function, thanks to the callback parameters, can be adapted to do
 * a UL or DL allocation. Please make sure the callbacks return references
 * (or no effects will be seen on the caller).
 *
 * \see BeforeDlSched
 */
NrMacSchedulerTdma::BeamSymbolMap
NrMacSchedulerTdma::AssignRBGTDMA(uint32_t symAvail,
                                  const ActiveUeMap& activeUe,
                                  const std::string& type,
                                  const BeforeSchedFn& BeforeSchedFn,
                                  const GetCompareUeFn& GetCompareFn,
                                  const GetTBSFn& GetTBSFn,
                                  const GetRBGFn& GetRBGFn,
                                  const GetSymFn& GetSymFn,
                                  const AfterSuccessfulAssignmentFn& SuccessfulAssignmentFn,
                                  const AfterUnsuccessfulAssignmentFn& UnSuccessfulAssignmentFn,
                                  const CallNotifyFn& callNotifyFn) const
{
    NS_LOG_FUNCTION(this);
    NS_LOG_DEBUG("Assigning RBG in " << type << ", # beams active flows: " << activeUe.size()
                                     << ", # sym: " << symAvail);

    // Create vector of UE (without considering the beam)
    std::vector<UePtrAndBufferReq> ueVector = GetUeVectorFromActiveUeMap(activeUe);

    // Distribute the symbols following the selected behaviour among UEs
    uint32_t resources = symAvail;
    FTResources assigned(0, 0);

    const std::vector<bool> notchedRBGsMask =
        type == "DL" ? GetDlNotchedRbgMask() : GetUlNotchedRbgMask();
    int zeroes = std::count(notchedRBGsMask.begin(), notchedRBGsMask.end(), 0);
    uint32_t numOfAssignableRbgs = GetBandwidthInRbg() - zeroes;
    NS_ASSERT(numOfAssignableRbgs > 0);

    for (auto& ue : ueVector)
    {
        BeforeSchedFn(ue, FTResources(numOfAssignableRbgs, 1));
    }

    while (resources > 0)
    {
        if (m_activeDlAi || m_activeUlAi)
        {
            callNotifyFn(ueVector);
        }
        GetFirst GetUe;

        auto schedInfoIt = ueVector.begin();

        std::stable_sort(ueVector.begin(), ueVector.end(), GetCompareFn());

        // Ensure fairness: pass over UEs which already has enough resources to transmit
        while (schedInfoIt != ueVector.end())
        {
            uint32_t bufQueueSize = schedInfoIt->second;

            if (GetTBSFn(GetUe(*schedInfoIt)) >= std::max(bufQueueSize, 10U))
            {
                NS_LOG_INFO("UE " << GetUe(*schedInfoIt)->m_rnti << " TBS "
                                  << GetTBSFn(GetUe(*schedInfoIt)) << " queue " << bufQueueSize
                                  << ", passing");
                schedInfoIt++;
            }
            else
            {
                break;
            }
        }

        // In the case that all the UE already have their requirements fulfilled,
        // then stop the assignment
        if (schedInfoIt == ueVector.end())
        {
            NS_LOG_INFO("All the UE already have their resources allocated. Skipping the beam");
            break;
        }

        // Assign 1 entire symbol (full RBG) to the selected UE and to the total
        // resources assigned count
        GetRBGFn(GetUe(*schedInfoIt)) += numOfAssignableRbgs;
        assigned.m_rbg += numOfAssignableRbgs;

        GetSymFn(GetUe(*schedInfoIt)) += 1;
        assigned.m_sym += 1;

        // subtract 1 SYM from the number of sym available for the while loop
        resources -= 1;

        // Update metrics for the successful UE
        NS_LOG_DEBUG("Assigned " << numOfAssignableRbgs << " " << type << " RBG (= 1 SYM) to UE "
                                 << GetUe(*schedInfoIt)->m_rnti
                                 << " total assigned up to now: " << GetRBGFn(GetUe(*schedInfoIt))
                                 << " that corresponds to " << assigned.m_rbg);
        SuccessfulAssignmentFn(*schedInfoIt, FTResources(numOfAssignableRbgs, 1), assigned);

        // Update metrics for the unsuccessful UEs (who did not get any resource in this iteration)
        for (auto& ue : ueVector)
        {
            if (GetUe(ue)->m_rnti != GetUe(*schedInfoIt)->m_rnti)
            {
                UnSuccessfulAssignmentFn(ue, FTResources(numOfAssignableRbgs, 1), assigned);
            }
        }
    }

    // Count the number of assigned symbol of each beam.
    NrMacSchedulerTdma::BeamSymbolMap ret;
    for (const auto& el : activeUe)
    {
        uint32_t symOfBeam = 0;
        for (const auto& ue : el.second)
        {
            symOfBeam += GetRBGFn(ue.first) / numOfAssignableRbgs;
        }
        ret.insert(std::make_pair(el.first, symOfBeam));
    }
    return ret;
}

/**
 * \brief Assign the available DL RBG to the UEs
 * \param symAvail Number of available symbols
 * \param activeDl active DL flows and UE
 * \return a map between the beam and the symbols assigned to each one
 *
 * The function will prepare all the needed callbacks to return UE DL parameters
 * (e.g., the DL TBS, the DL RBG) and then will call NrMacSchedulerTdma::AssignRBGTDMA.
 */
NrMacSchedulerTdma::BeamSymbolMap
NrMacSchedulerTdma::AssignDLRBG(uint32_t symAvail, const ActiveUeMap& activeDl) const
{
    NS_LOG_FUNCTION(this);

    BeforeSchedFn beforeSched = std::bind(&NrMacSchedulerTdma::BeforeDlSched,
                                          this,
                                          std::placeholders::_1,
                                          std::placeholders::_2);
    AfterSuccessfulAssignmentFn SuccFn = std::bind(&NrMacSchedulerTdma::AssignedDlResources,
                                                   this,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2,
                                                   std::placeholders::_3);
    AfterUnsuccessfulAssignmentFn UnSuccFn = std::bind(&NrMacSchedulerTdma::NotAssignedDlResources,
                                                       this,
                                                       std::placeholders::_1,
                                                       std::placeholders::_2,
                                                       std::placeholders::_3);
    GetCompareUeFn compareFn = std::bind(&NrMacSchedulerTdma::GetUeCompareDlFn, this);

    GetTBSFn GetTbs = &NrMacSchedulerUeInfo::GetDlTBS;
    GetRBGFn GetRBG = &NrMacSchedulerUeInfo::GetDlRBG;
    GetSymFn GetSym = &NrMacSchedulerUeInfo::GetDlSym;

    CallNotifyFn callNotifyFn =
        std::bind(&NrMacSchedulerTdma::CallNotifyDlFn, this, std::placeholders::_1);

    return AssignRBGTDMA(symAvail,
                         activeDl,
                         "DL",
                         beforeSched,
                         compareFn,
                         GetTbs,
                         GetRBG,
                         GetSym,
                         SuccFn,
                         UnSuccFn,
                         callNotifyFn);
}

/**
 * \brief Assign the available UL RBG to the UEs
 * \param symAvail Number of available symbols
 * \param activeUl active DL flows and UE
 * \return a map between the beam and the symbols assigned to each one
 *
 * The function will prepare all the needed callbacks to return UE UL parameters
 * (e.g., the UL TBS, the UL RBG) and then will call NrMacSchedulerTdma::AssignRBGTDMA.
 */
NrMacSchedulerTdma::BeamSymbolMap
NrMacSchedulerTdma::AssignULRBG(uint32_t symAvail, const ActiveUeMap& activeUl) const
{
    NS_LOG_FUNCTION(this);
    BeforeSchedFn beforeSched = std::bind(&NrMacSchedulerTdma::BeforeUlSched,
                                          this,
                                          std::placeholders::_1,
                                          std::placeholders::_2);
    AfterSuccessfulAssignmentFn SuccFn = std::bind(&NrMacSchedulerTdma::AssignedUlResources,
                                                   this,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2,
                                                   std::placeholders::_3);
    GetCompareUeFn compareFn = std::bind(&NrMacSchedulerTdma::GetUeCompareUlFn, this);
    AfterUnsuccessfulAssignmentFn UnSuccFn = std::bind(&NrMacSchedulerTdma::NotAssignedUlResources,
                                                       this,
                                                       std::placeholders::_1,
                                                       std::placeholders::_2,
                                                       std::placeholders::_3);
    GetTBSFn GetTbs = &NrMacSchedulerUeInfo::GetUlTBS;
    GetRBGFn GetRBG = &NrMacSchedulerUeInfo::GetUlRBG;
    GetSymFn GetSym = &NrMacSchedulerUeInfo::GetUlSym;

    CallNotifyFn callNotifyFn =
        std::bind(&NrMacSchedulerTdma::CallNotifyUlFn, this, std::placeholders::_1);

    return AssignRBGTDMA(symAvail,
                         activeUl,
                         "UL",
                         beforeSched,
                         compareFn,
                         GetTbs,
                         GetRBG,
                         GetSym,
                         SuccFn,
                         UnSuccFn,
                         callNotifyFn);
}

/**
 * \brief Create a DL DCI starting from spoint and spanning maxSym symbols
 * \param spoint Starting point of the DCI
 * \param ueInfo UE representation
 * \param maxSym Maximum number of symbols for the creation of the DCI
 * \return a pointer to the newly created DCI
 *
 * The method calculates the TBS and the real number of symbols needed, and
 * then call CreateDci().
 */
std::shared_ptr<DciInfoElementTdma>
NrMacSchedulerTdma::CreateDlDci(PointInFTPlane* spoint,
                                const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
                                [[maybe_unused]] uint32_t maxSym) const
{
    NS_LOG_FUNCTION(this);
    uint32_t tbs = m_dlAmc->CalculateTbSize(ueInfo->m_dlMcs,
                                            ueInfo->m_dlRank,
                                            ueInfo->m_dlRBG * GetNumRbPerRbg());
    // If is less than 10 (3 mac header, 2 rlc header, 5 data), then we can't
    // transmit any new data, so don't create dci.
    if (tbs < 10)
    {
        NS_LOG_DEBUG("While creating DL DCI for UE " << ueInfo->m_rnti << " assigned "
                                                     << ueInfo->m_dlRBG << " DL RBG, but TBS < 10");
        ueInfo->m_dlTbSize = 0;
        return nullptr;
    }

    const std::vector<bool> notchedRBGsMask = GetDlNotchedRbgMask();
    int zeroes = std::count(notchedRBGsMask.begin(), notchedRBGsMask.end(), 0);
    uint32_t numOfAssignableRbgs = GetBandwidthInRbg() - zeroes;

    uint8_t numSym = static_cast<uint8_t>(ueInfo->m_dlRBG / numOfAssignableRbgs);

    auto dci = CreateDci(spoint,
                         ueInfo,
                         tbs,
                         DciInfoElementTdma::DL,
                         ueInfo->m_dlMcs,
                         ueInfo->m_dlRank,
                         ueInfo->m_dlPrecMats,
                         std::max(numSym, static_cast<uint8_t>(1)));

    // The starting point must advance.
    spoint->m_rbg = 0;
    spoint->m_sym += numSym;

    return dci;
}

/**
 * \brief Create a UL DCI starting from spoint and spanning maxSym symbols
 * \param spoint Starting point of the DCI
 * \param ueInfo UE representation
 * \return a pointer to the newly created DCI
 *
 * The method calculates the TBS and the real number of symbols needed, and
 * then call CreateDci().
 * Allocate the DCI going backward from the starting point (it should be called
 * ending point maybe).
 */
std::shared_ptr<DciInfoElementTdma>
NrMacSchedulerTdma::CreateUlDci(NrMacSchedulerNs3::PointInFTPlane* spoint,
                                const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
                                uint32_t maxSym) const
{
    NS_LOG_FUNCTION(this);
    uint32_t tbs = m_ulAmc->CalculateTbSize(ueInfo->m_ulMcs,
                                            ueInfo->m_ulRank,
                                            ueInfo->m_ulRBG * GetNumRbPerRbg());

    // If is less than 12, 7 (3 mac header, 2 rlc header, 2 data) + SHORT_BSR (5),
    // then we can't transmit any new data, so don't create dci.
    if (tbs < 12)
    {
        NS_LOG_DEBUG("While creating UL DCI for UE " << ueInfo->m_rnti << " assigned "
                                                     << ueInfo->m_ulRBG << " UL RBG, but TBS "
                                                     << tbs << " < 12");
        return nullptr;
    }

    const std::vector<bool> notchedRBGsMask = GetUlNotchedRbgMask();
    int zeroes = std::count(notchedRBGsMask.begin(), notchedRBGsMask.end(), 0);
    uint32_t numOfAssignableRbgs = GetBandwidthInRbg() - zeroes;

    uint8_t numSym = static_cast<uint8_t>(std::max(ueInfo->m_ulRBG / numOfAssignableRbgs, 1U));
    numSym = std::min(numSym, static_cast<uint8_t>(maxSym));

    NS_ASSERT(spoint->m_sym >= numSym);

    // The starting point must go backward to accommodate the needed sym
    spoint->m_sym -= numSym;

    auto dci = CreateDci(spoint,
                         ueInfo,
                         tbs,
                         DciInfoElementTdma::UL,
                         ueInfo->m_ulMcs,
                         ueInfo->m_ulRank,
                         ueInfo->m_ulPrecMats,
                         numSym);

    // Reset the RBG (we are TDMA)
    spoint->m_rbg = 0;

    return dci;
}

uint8_t
NrMacSchedulerTdma::GetTpc() const
{
    NS_LOG_FUNCTION(this);
    return 1; // 1 is mapped to 0 for Accumulated mode, and to -1 in Absolute mode TS38.213 Table
              // Table 7.1.1-1
}

/**
 * \brief Create a DCI with the parameters specified as input
 * \param spoint starting point
 * \param ueInfo ue representation
 * \param tbs Transport Block Size
 * \param fmt Format of the DCI (UL or DL)
 * \param mcs MCS
 * \param numSym Number of symbols
 * \return a pointer to the newly created DCI
 *
 * Creates a TDMA DCI (a DCI with all the resource block assigned for the
 * specified number of symbols)
 */
std::shared_ptr<DciInfoElementTdma>
NrMacSchedulerTdma::CreateDci(NrMacSchedulerNs3::PointInFTPlane* spoint,
                              const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
                              uint32_t tbs,
                              DciInfoElementTdma::DciFormat fmt,
                              uint32_t mcs,
                              uint8_t rank,
                              Ptr<const ComplexMatrixArray> precMats,
                              uint8_t numSym) const
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(tbs > 0);
    NS_ASSERT(numSym > 0);

    std::shared_ptr<DciInfoElementTdma> dci =
        std::make_shared<DciInfoElementTdma>(ueInfo->m_rnti,
                                             fmt,
                                             spoint->m_sym,
                                             numSym,
                                             mcs,
                                             rank,
                                             precMats,
                                             tbs,
                                             1,
                                             0,
                                             DciInfoElementTdma::DATA,
                                             GetBwpId(),
                                             GetTpc());

    std::vector<bool> rbgAssigned =
        fmt == DciInfoElementTdma::DL ? GetDlNotchedRbgMask() : GetUlNotchedRbgMask();

    if (rbgAssigned.empty())
    {
        rbgAssigned = std::vector<bool>(GetBandwidthInRbg(), true);
    }

    NS_ASSERT(rbgAssigned.size() == GetBandwidthInRbg());

    dci->m_rbgBitmask = std::move(rbgAssigned);

    std::ostringstream oss;
    for (auto x : dci->m_rbgBitmask)
    {
        oss << std::to_string(x) << " ";
    }

    NS_LOG_INFO("UE " << ueInfo->m_rnti << " assigned RBG from " << spoint->m_rbg << " with mask "
                      << oss.str() << " for " << static_cast<uint32_t>(numSym) << " SYM ");

    NS_ASSERT(std::count(dci->m_rbgBitmask.begin(), dci->m_rbgBitmask.end(), 0) !=
              GetBandwidthInRbg());

    return dci;
}

} // namespace ns3
