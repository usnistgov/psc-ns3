/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This software was developed at the National Institute of Standards and
 * Technology by employees of the Federal Government in the course of
 * their official duties. Pursuant to titleElement 17 Section 105 of the United
 * States Code this software is not subject to copyright protection and
 * is in the public domain.
 * NIST assumes no responsibility whatsoever for its use by other parties,
 * and makes no guarantees, expressed or implied, about its quality,
 * reliability, or any other characteristic.

 * We would appreciate acknowledgement if the software is used.

 * NIST ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
 * DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING
 * FROM THE USE OF THIS SOFTWARE.
 *
 * Modified by: NIST
 */

#ifndef NIST_SL_DISC_PRECONFIG_POOL_FACTORY_H
#define NIST_SL_DISC_PRECONFIG_POOL_FACTORY_H

#include "nist-lte-rrc-sap.h"
#include "nist-sl-disc-resource-pool-factory.h"

namespace ns3 {
/**
 * Sidelink discovery preconfigured pool factory
 */
class NistSlDiscPreconfigPoolFactory : public NistSlDiscResourcePoolFactory
{
public:

  /**
   * Create a preconfigured discovery pool
   */
  NistSlDiscPreconfigPoolFactory ();

  /**
   * Set the preconfigured discovery pool
   */
  NistLteRrcSap::SlPreconfigDiscPool ConfigPool ();
private:
  NistLteRrcSap::SlPreconfigDiscPool m_pool;
};

} // namespace ns3

#endif
