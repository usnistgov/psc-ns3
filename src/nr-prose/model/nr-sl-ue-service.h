//
// SPDX-License-Identifier: NIST-Software
//

#ifndef NR_SL_UE_SERVICE_H
#define NR_SL_UE_SERVICE_H

#include <ns3/object.h>

namespace ns3
{

/**
 * \ingroup lte
 *
 * This is the base class for the implementation of services that use the
 * sidelink (e.g., Proximity Services)
 */

class NrSlUeService : public Object
{
  public:
    NrSlUeService();
    virtual ~NrSlUeService();

    // inherited from Object
    virtual void DoDispose(void);
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId(void);
};

} // namespace ns3

#endif /* NR_SL_UE_SERVICE */
