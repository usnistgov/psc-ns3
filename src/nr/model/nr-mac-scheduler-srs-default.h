// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_MAC_SCHEDULER_SRS_DEFAULT_H
#define NR_MAC_SCHEDULER_SRS_DEFAULT_H

#include "nr-mac-scheduler-srs.h"

#include <ns3/object.h>
#include <ns3/random-variable-stream.h>

namespace ns3
{

/**
 * \brief Default algorithm for assigning offset and periodicity
 *
 * The algorithm assign the same periodicity to all the UEs. When a new periodicity
 * is asked, it is returned a value between 1 and the configured periodicity (minus 1).
 *
 * The returned values will never be the same; instead, when this must happen,
 * an invalid value is returned and (hopefully) an increase of periodicity is invoked.
 */
class NrMacSchedulerSrsDefault : public NrMacSchedulerSrs, public Object
{
  public:
    /**
     * \brief NrMacSchedulerSrsDefault
     */
    NrMacSchedulerSrsDefault();
    /**
     * \brief ~NrMacSchedulerSrsDefault
     */
    ~NrMacSchedulerSrsDefault() override;

    /**
     * \brief GetTypeId
     * \return the object type id
     */
    static TypeId GetTypeId();

    // inherited from NrMacSchedulerSrs
    SrsPeriodicityAndOffset AddUe() override;
    void RemoveUe(uint32_t offset) override;
    bool IncreasePeriodicity(
        std::unordered_map<uint16_t, std::shared_ptr<NrMacSchedulerUeInfo>>* ueMap) override;
    bool DecreasePeriodicity(
        std::unordered_map<uint16_t, std::shared_ptr<NrMacSchedulerUeInfo>>* ueMap) override;

    /**
     * \brief Set the Periodicity for all the UEs
     * \param start the periodicity
     */
    void SetStartingPeriodicity(uint32_t start);

    /**
     * \brief Get the periodicity
     * \return the periodicity
     */
    uint32_t GetStartingPeriodicity() const;

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model.  Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream);

  private:
    /**
     * \brief Reassign offset/periodicity to all the UEs
     * \param ueMap the UE map of the scheduler
     */
    void ReassignSrsValue(
        std::unordered_map<uint16_t, std::shared_ptr<NrMacSchedulerUeInfo>>* ueMap);
    /**
     * \brief Randomly shuffle the available offset values
     */
    void ShuffleOffsets();
    static std::vector<uint32_t> StandardPeriodicity; //!< Standard periodicity of SRS

  private:
    uint32_t m_periodicity{0};                     //!< Configured periodicity
    std::vector<uint32_t> m_availableOffsetValues; //!< Available offset values
    Ptr<UniformRandomVariable> m_random;           //!< Random variable
    EventId m_shuffleEventId;                      //!< Event ID for offset shuffling
};

} // namespace ns3

#endif // NR_MAC_SCHEDULER_SRS_DEFAULT_H
