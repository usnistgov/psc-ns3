//
// SPDX-License-Identifier: NIST-Software

#ifndef NR_SL_MCS_CONTROLLER_H
#define NR_SL_MCS_CONTROLLER_H

#include "nr-sl-ue-mac.h"

#include <ns3/object.h>
#include <ns3/ptr.h>

namespace ns3
{

class NrSlUeMacSchedulerDefault;

/**
 * \ingroup scheduler
 *
 * This object is compatible with NrSlUeMacSchedulerDefault and any subclasses
 *
 * \brief Interface and null implementation for all of the MCS controllers
 */
class NrSlMcsController : public Object
{
  public:
    /**
     * \brief Get the type id
     * \return the type id of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrSlMcsController constructor
     */
    NrSlMcsController();

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model.  Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    virtual int64_t AssignStreams(int64_t stream);

    /**
     * Set the pointer to the NrSlUeMac
     * \param mac The pointer to the NrSlUeMac
     */
    void SetNrSlUeMac(Ptr<NrSlUeMac> mac);

    /**
     * Set the pointer to the NrSlUeMacScheduler
     * \param scheduler The pointer to the NrSlUeMacScheduler
     */
    void SetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler);

  protected:
    void DoDispose() override;

    Ptr<NrSlUeMac> m_mac;                       //!< Pointer to NrSlUeMac
    Ptr<NrSlUeMacSchedulerDefault> m_scheduler; //!< Pointer to NrSlUeMacScheduler

  private:
    /**
     * Virtual method to allow subclasses to apply post-conditions on setting the scheduler
     * pointer
     * \param scheduler The pointer to the NrSlUeMacScheduler
     */
    virtual void DoSetNrSlUeMacScheduler(Ptr<NrSlUeMacSchedulerDefault> scheduler);
};

} // namespace ns3

#endif /* NR_SL_MCS_CONTROLLER_H */
