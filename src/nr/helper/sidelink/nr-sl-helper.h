// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_HELPER_H
#define NR_SL_HELPER_H

#include "ns3/nr-helper.h"
#include <ns3/net-device-container.h>
#include <ns3/nr-sl-rrc-sap.h>
#include <ns3/object.h>

#include <optional>

namespace ns3
{

class NrUeNetDevice;
class NrSlTft;
class NrSlUeMacScheduler;

class NrSlHelper : public NrHelper
{
  public:
    /**
     * \brief Constructor
     */
    NrSlHelper();
    /**
     * \brief Destructor
     */
    ~NrSlHelper() override;
    /**
     * \brief \c GetTypeId, inherited from Object
     *
     * \returns The \c TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief Configure a sidelink (mode 2) network of UEs
     *
     * This helper method encapsulates a lot of low-level configuration for a common configuration
     * of a sidelink Mode 2 network.  It is for a single carrier, single bandwidth part network.
     *
     * Several attributes can be configured by NrHelper::Set...Attribute() methods.  One that
     * is called internally to this method is to set the antenna factory configuration to a
     * single element IsotropicAntennaModel instead of the default UE planar array.
     * Several additional attributes of this class are available to set values
     * such as central frequency, bandwidth, numerology, etc.
     *
     * The UE transmit power should be set externally to this method; note that the default
     * is only 2 dBm.
     *
     * This method sets the maximum number of sidelink HARQ processes to 4.  Other attributes
     * such as enabling sensing should be set externally to this method.
     *
     * \param n The NodeContainer
     * \param config optional configuration structure to override selected default values
     * \param channel The SpectrumChannel instance to use
     * \return A NetDeviceContainer with the created NetDevices
     */
    NetDeviceContainer ConfigureSlNetwork(NodeContainer n, Ptr<SpectrumChannel> channel);
    /**
     * \brief Prepare UE for Sidelink
     *
     * \param c The \c NetDeviceContainer
     * \param slBwpIds The container of Sidelink BWP ids
     */
    void PrepareUeForSidelink(NetDeviceContainer c, const std::set<uint8_t>& slBwpIds);

    /**
     * \brief Install NR sidelink pre-configuration in the UEs expected to use
     *        sidelink.
     *
     * \param c The \c NetDeviceContainer
     * \param preConfig The <tt> struct NrSlRrcSap::SidelinkPreconfigNr </tt>
     */
    void InstallNrSlPreConfiguration(NetDeviceContainer c,
                                     const NrSlRrcSap::SidelinkPreconfigNr preConfig);
    /**
     * \brief Schedule the activation of a NR sidelink bearer
     *
     * \param activationTime The time to setup the sidelink bearer
     * \param ues The list of UEs where the bearer must be activated
     * \param tft The traffic flow template for the bearer (i.e. multicast address and group)
     */
    void ActivateNrSlBearer(Time activationTime, NetDeviceContainer ues, const Ptr<NrSlTft> tft);
    /**
     * \brief Activation of a sidelink bearer
     *
     * \param ues The list of UEs where the bearer must be activated
     * \param tft The traffic flow template for the bearer (i.e. multicast address and group)
     */
    void DoActivateNrSlBearer(NetDeviceContainer ues, const Ptr<NrSlTft> tft);
    /**
     * \brief Set the SL error model TypeId.  Works only before Install is called.
     * \param typeId The SL error model type
     */
    void SetSlErrorModelTypeId(const TypeId& typeId);
    /**
     * \brief Set the NR SL Scheduler TypeId. Works only before it is created.
     * \param typeId The NR SL scheduler type
     *
     * \see NrSlUeMacSchedulerSimple
     */
    void SetNrSlSchedulerTypeId(const TypeId& typeId);
    /**
     * \brief Set UE sidelink scheduler attribute
     * \param n The attribute name
     * \param v The attribute value
     */
    void SetUeSlSchedulerAttribute(const std::string& n, const AttributeValue& v);
    /**
     * \brief Set the MCS controller TypeId. This method only works prior to UE stack installation.
     * \param typeId The McsController type
     */
    void SetMcsControllerTypeId(const TypeId& typeId);
    /**
     * \brief Set the list of possible resource reservation periods in the resource pool
     * \param periodList The list of possible periods (in ms)
     */
    void SetSlResourceReservePeriodList(const std::list<uint16_t>& periodList);
    /**
     * \brief Set the probability of keeping SPS resources at reselection time
     * \param probability The probability value (0.0 to 0.8 per TS 38.321)
     */
    void SetSlProbResourceKeep(double probability);
    /**
     * \brief Set the maximum number of PSSCH resources per reservation
     * The possible values are 1, 2, or 3 (default).
     * \param maxNumPerReserve The maximum (1 = no retransmissions, 2 or 3 = retransmissions)
     */
    void SetSlMaxNumPerReserve(uint16_t maxNumPerReserve);
    /**
     * \brief Set the maximum number of NR sidelink HARQ processes
     *
     * Configures the MaxSidelinkProcess and MaxSidelinkProcessMultiplePdu
     * attributes on the UE MAC before device installation.
     *
     * \param total Total number of HARQ process IDs (default 16, max 16)
     * \param multiplePdu Number reserved for multiple-PDU (SPS) grants (default 4)
     */
    void SetSlMaxHarqProcesses(uint8_t total, uint8_t multiplePdu);
    /**
     * \brief Get the length of the physical Sidelink pool based on
     *        SL bitmap length, TDD pattern length, and the number of UL slots
     *        in the TDD pattern.
     * \param slBitmapLen The SL bitmap length
     * \param tddPatternLen The TDD pattern length
     * \param numUlTddPattern the number of UL slots in the TDD pattern
     * \return The resultant length of the physical Sidelink pool in slots
     */
    static uint16_t GetPhySlPoolLength(uint16_t slBitmapLen,
                                       uint16_t tddPatternLen,
                                       uint16_t numUlTddPattern);
    /**
     * \brief Activate NR sidelink bearer
     *
     * \param ueDevice The device of the UE
     * \param slTft The sidelink traffic flow template for the new bearer
     */
    void ActivateNrSlBearerForUe(const Ptr<NetDevice>& ueDevice, const Ptr<NrSlTft>& slTft) const;

    /**
     * \brief Assign a fixed random variable stream number to the random variables used.
     *
     * The InstallUeDevice and PrepareUeForSidelink method should have previously
     * been called by the user on the given devices.
     *
     *
     * \param c NetDeviceContainer of the NR SL UE NetDevices for which
     *          we need to fix the stream
     * \param stream first stream index to use
     * \return the number of stream indices (possibly zero) that have been assigned
     */
    int64_t AssignStreams(NetDeviceContainer c, int64_t stream) override;

  protected:
    /**
     * \brief \c DoDispose method inherited from \c Object
     */
    void DoDispose() override;

  private:
    /**
     * \brief Configure the UE parameters
     *
     * This method is used to configure the UE parameters,
     * which can not be set via RRC.
     *
     * \param dev The NrUeNetDevice
     * \param freqCommon The <tt> struct SlFreqConfigCommonNr </tt> to retrieve
     *        SL BWP related configuration
     * \param general The <tt> struct SlPreconfigGeneralNr </tt> to retrieve
     *        general parameters for a BWP, e.g., TDD pattern
     * \return true if the user indented BWP for SL is configured, false otherwise
     */
    bool ConfigUeParams(const Ptr<NrUeNetDevice>& dev,
                        const NrSlRrcSap::SlFreqConfigCommonNr& freqCommon,
                        const NrSlRrcSap::SlPreconfigGeneralNr& general);

    /**
     * \brief Prepare Single UE for Sidelink
     *
     * \param nrUeDev The Ptr to NR Ue netdevice
     * \param slBwpIds The container of Sidelink BWP ids
     */
    void PrepareSingleUeForSidelink(Ptr<NrUeNetDevice> nrUeDev, const std::set<uint8_t>& slBwpIds);

    ObjectFactory m_ueSlSchedulerFactory;  //!< UE SL scheduler Object factory
    ObjectFactory m_ueSlErrorModelFactory; //!< UE SL error model factory
    ObjectFactory m_mcsControllerFactory;  //!< MCS controller factory

    // Configurable items
    double m_centralFrequency; //!< Central frequency for ConfigureSlNetwork()
    double m_bandwidth;        //!< Bandwidth for ConfigureSlNetwork()
    uint16_t m_numerology;     //!< Numerology for ConfigureSlNetwork()
    std::list<uint16_t> m_resourceReservePeriodList{0, 20, 50, 100}; //!< Candidate periods
    double m_slProbResourceKeep{0.0}; //!< Probability of keeping SPS resources at reselection
    uint16_t m_maxNumPerReserve{3};   //!< Maximum PSSCH resources per reservation (1-3)
    uint8_t m_maxSlHarqProcesses{16}; //!< MaxSidelinkProcess attribute value
    uint8_t m_maxSlHarqProcessesMultiplePdu{4}; //!< MaxSidelinkProcessMultiplePdu attribute value
};

} // namespace ns3

#endif /* NR_SL_HELPER_H */
