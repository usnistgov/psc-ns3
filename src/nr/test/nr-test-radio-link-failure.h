//
// Copyright (c) 2018 Fraunhofer ESK
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Vignesh Babu <ns3-dev@esk.fraunhofer.de>
//

#ifndef NR_TEST_RADIO_LINK_FAILURE_H
#define NR_TEST_RADIO_LINK_FAILURE_H

#include <ns3/mobility-model.h>
#include <ns3/net-device-container.h>
#include <ns3/node-container.h>
#include <ns3/nr-ue-rrc.h>
#include <ns3/nstime.h>
#include <ns3/test.h>
#include <ns3/vector.h>

#include <vector>

namespace ns3
{

class NrUeNetDevice;

}

using namespace ns3;

/**
 * \brief Test suite for
 *
 * \sa ns3::NrRadioLinkFailureTestCase
 */
class NrRadioLinkFailureTestSuite : public TestSuite
{
  public:
    NrRadioLinkFailureTestSuite();
};

/**
 * \ingroup lte
 *
 * \brief Testing the cell reselection procedure by UE at IDLE state
 */
class NrRadioLinkFailureTestCase : public TestCase
{
  public:
    /**
     * \brief Creates an instance of the radio link failure test case.
     *
     * \param numGnbs number of eNodeBs
     * \param numUes number of UEs
     * \param simTime the simulation time
     * \param isIdealRrc if true, simulation uses Ideal RRC protocol, otherwise
     *                   simulation uses Real RRC protocol
     * \param uePositionList Position of the UEs
     * \param gnbPositionList Position of the eNodeBs
     * \param ueJumpAwayPosition Vector holding the UE jump away coordinates
     * \param checkConnectedList the time at which UEs should have an active RRC connection
     */
    NrRadioLinkFailureTestCase(uint32_t numGnbs,
                               uint32_t numUes,
                               Time simTime,
                               bool isIdealRrc,
                               std::vector<Vector> uePositionList,
                               std::vector<Vector> gnbPositionList,
                               Vector ueJumpAwayPosition,
                               std::vector<Time> checkConnectedList);

    ~NrRadioLinkFailureTestCase() override;

  private:
    /**
     * Builds the test name string based on provided parameter values
     * \param numGnbs the number of gNB nodes
     * \param numUes the number of UE nodes
     * \param isIdealRrc True if the Ideal RRC protocol is used
     * \returns the name string
     */
    std::string BuildNameString(uint32_t numGnbs, uint32_t numUes, bool isIdealRrc);
    /**
     * \brief Setup the simulation according to the configuration set by the
     *        class constructor, run it, and verify the result.
     */
    void DoRun() override;

    /**
     * Check connected function
     * \param ueDevice the UE device
     * \param gnbDevices the gNB devices
     */
    void CheckConnected(Ptr<NetDevice> ueDevice, NetDeviceContainer gnbDevices);

    /**
     * Check if the UE is in idle state
     * \param ueDevice the UE device
     * \param gnbDevices the gNB devices
     */
    void CheckIdle(Ptr<NetDevice> ueDevice, NetDeviceContainer gnbDevices);

    /**
     * \brief Check if the UE exist at the gNB
     * \param rnti the RNTI of the UE
     * \param gnbDevice the gNB device
     * \return true if the UE exist at the eNB, otherwise false
     */
    bool CheckUeExistAtGnb(uint16_t rnti, Ptr<NetDevice> gnbDevice);

    /**
     * \brief State transition callback function
     * \param context the context string
     * \param imsi the IMSI
     * \param cellId the cell ID
     * \param rnti the RNTI
     * \param oldState the old state
     * \param newState the new state
     */
    void UeStateTransitionCallback(std::string context,
                                   uint64_t imsi,
                                   uint16_t cellId,
                                   uint16_t rnti,
                                   NrUeRrc::State oldState,
                                   NrUeRrc::State newState);

    /**
     * \brief Connection established at UE callback function
     * \param context the context string
     * \param imsi the IMSI
     * \param cellId the cell ID
     * \param rnti the RNTI
     */
    void ConnectionEstablishedUeCallback(std::string context,
                                         uint64_t imsi,
                                         uint16_t cellId,
                                         uint16_t rnti);

    /**
     * \brief Connection established at eNodeB callback function
     * \param context the context string
     * \param imsi the IMSI
     * \param cellId the cell ID
     * \param rnti the RNTI
     */
    void ConnectionEstablishedGnbCallback(std::string context,
                                          uint64_t imsi,
                                          uint16_t cellId,
                                          uint16_t rnti);

    /**
     * \brief This callback function is executed when UE context is removed at eNodeB
     * \param context the context string
     * \param imsi the IMSI
     * \param cellId the cell ID
     * \param rnti the RNTI
     */
    void ConnectionReleaseAtGnbCallback(std::string context,
                                        uint64_t imsi,
                                        uint16_t cellId,
                                        uint16_t rnti);

    /**
     * \brief This callback function is executed when UE RRC receives an in-sync or out-of-sync
     * indication
     * \param context the context string
     * \param imsi the IMSI
     * \param rnti the RNTI
     * \param cellId the cell ID
     * \param type in-sync or out-of-sync indication
     * \param count the number of in-sync or out-of-sync indications
     */
    void PhySyncDetectionCallback(std::string context,
                                  uint64_t imsi,
                                  uint16_t rnti,
                                  uint16_t cellId,
                                  std::string type,
                                  uint8_t count);

    /**
     * \brief This callback function is executed when radio link failure is detected
     * \param context the context string
     * \param imsi the IMSI
     * \param rnti the RNTI
     * \param cellId the cell ID
     */
    void RadioLinkFailureCallback(std::string context,
                                  uint64_t imsi,
                                  uint16_t cellId,
                                  uint16_t rnti);

    /**
     * \brief Jump away function
     *
     * \param UeJumpAwayPositionList A list of positions where UE would jump
     */
    void JumpAway(Vector UeJumpAwayPositionList);

    uint32_t m_numGnbs;                    ///< number of eNodeBs
    uint32_t m_numUes;                     ///< number of UEs
    Time m_simTime;                        ///< simulation time
    bool m_isIdealRrc;                     ///< whether the NR is configured to use ideal RRC
    std::vector<Vector> m_uePositionList;  ///< Position of the UEs
    std::vector<Vector> m_gnbPositionList; ///< Position of the eNodeBs
    std::vector<Time>
        m_checkConnectedList;    ///< the time at which UEs should have an active RRC connection
    Vector m_ueJumpAwayPosition; ///< Position where the UE(s) would jump

    /// The current UE RRC state.
    NrUeRrc::State m_lastState;

    bool m_radioLinkFailureDetected;      ///< true if radio link fails
    uint32_t m_numOfInSyncIndications;    ///< number of in-sync indications detected
    uint32_t m_numOfOutOfSyncIndications; ///< number of out-of-sync indications detected
    Ptr<MobilityModel> m_ueMobility;      ///< UE mobility model

}; // end of class NrRadioLinkFailureTestCase

#endif /* NR_TEST_RADIO_LINK_FAILURE_H */
