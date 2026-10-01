/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 * (Based on nr-helper.h)
 */

#ifndef NR_SIMPLE_HELPER_H
#define NR_SIMPLE_HELPER_H

#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/nr-bearer-stats-calculator.h"
#include "ns3/nr-pdcp.h"
#include "ns3/nr-rlc-am.h"
#include "ns3/nr-rlc-um.h"
#include "ns3/nr-rlc.h"
#include "ns3/simple-channel.h"

namespace ns3
{

class NrTestRrc;
class NrTestMac;

/**
 * \ingroup nr-test
 *
 * \brief A simplified version of NrHelper, that
 * is used for creation and configuration of NR entities for testing purposes
 * when just a limited NrHelper functionality is wanted.
 *
 */
class NrSimpleHelper : public Object
{
  public:
    NrSimpleHelper();
    ~NrSimpleHelper() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    void DoDispose() override;

    /**
     * create a set of gNB devices
     *
     * \param c the node container where the devices are to be installed
     *
     * \return the NetDeviceContainer with the newly created devices
     */
    NetDeviceContainer InstallGnbDevice(NodeContainer c);

    /**
     * create a set of UE devices
     *
     * \param c the node container where the devices are to be installed
     *
     * \return the NetDeviceContainer with the newly created devices
     */
    NetDeviceContainer InstallUeDevice(NodeContainer c);

    /**
     * Enables logging for all components of the LENA architecture
     *
     */
    void EnableLogComponents();

    /**
     * Enables trace sinks for MAC, RLC and PDCP
     */
    void EnableTraces();

    /**
     * Enable trace sinks for RLC layer
     */
    void EnableRlcTraces();

    /**
     * Enable trace sinks for DL RLC layer
     */
    void EnableDlRlcTraces();

    /**
     * Enable trace sinks for UL RLC layer
     */
    void EnableUlRlcTraces();

    /**
     * Enable trace sinks for PDCP layer
     */
    void EnablePdcpTraces();

    /**
     * Enable trace sinks for DL PDCP layer
     */
    void EnableDlPdcpTraces();

    /**
     * Enable trace sinks for UL PDCP layer
     */
    void EnableUlPdcpTraces();

  protected:
    // inherited from Object
    void DoInitialize() override;

  private:
    /**
     * Install single gNB device
     *
     * \param n the node
     * \returns the device
     */
    Ptr<NetDevice> InstallSingleGnbDevice(Ptr<Node> n);
    /**
     * Install single UE device
     *
     * \param n the node
     * \returns the device
     */
    Ptr<NetDevice> InstallSingleUeDevice(Ptr<Node> n);

    Ptr<SimpleChannel> m_phyChannel; ///< the physical channel

  public:
    Ptr<NrTestRrc> m_gnbRrc; ///< gNB RRC
    Ptr<NrTestRrc> m_ueRrc;  ///< UE RRC

    Ptr<NrTestMac> m_gnbMac; ///< gNB MAC
    Ptr<NrTestMac> m_ueMac;  ///< UE MAC

  private:
    Ptr<NrPdcp> m_gnbPdcp; ///< gNB PDCP
    Ptr<NrRlc> m_gnbRlc;   ///< gNB RLC

    Ptr<NrPdcp> m_uePdcp; ///< UE PDCP
    Ptr<NrRlc> m_ueRlc;   ///< UE RLC

    ObjectFactory m_gnbDeviceFactory; ///< gNB device factory
    ObjectFactory m_ueDeviceFactory;  ///< UE device factory

    /// NrRlcEntityType_t enumeration
    enum NrRlcEntityType_t
    {
        RLC_UM = 1,
        RLC_AM = 2
    } m_nrRlcEntityType; ///< RLC entity type
};

} // namespace ns3

#endif // NR_SIMPLE_HELPER_H
