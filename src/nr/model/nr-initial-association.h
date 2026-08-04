// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_INITIAL_ASSOC_H
#define NR_INITIAL_ASSOC_H
#include "ns3/nr-module.h"
#include "ns3/object.h"

namespace ns3
{
const uint16_t NR_NUM_BANDS_FOR_SSB = 20;      ///< Number of bands used for the SSB
const double TRANSMIT_POWER_INIT_ASSOC = 30.0; ///< Transmit power in dBm

/// Angle pair in degrees for the row and column angle of beam direction for uniform planar array
struct NrAnglePair
{
    double rowAng = 90; ///< degrees
    double colAng = 90; ///< degrees
};

///< @brief NrInitialAssociation class
///< To set a initial association using SSB based approach where in the UE is associated with a gNB
///< from which the received RSRP is within handoff margin of the max received RSRP. It also
///< generates the main interfering set for a UE.
///<  Need to perform the following steps
///< Set UE Device first and then  assign a possible set of gNB devices.
///< Do FindAssociatedGnb() to get the associated gNB
///< Do InitializeInterSet to get major interferer

class NrInitialAssociation : public Object
{
  public:
    NrInitialAssociation() = default;

    /// @brief ChannelParams struct
    ///< to set channel model, pathloss model, spectral model and spectrumprop model to extract from
    ///< Ue and pass to attachment process
    struct ChannelParams
    {
        Ptr<ThreeGppChannelModel> channelModel{nullptr};
        Ptr<ThreeGppPropagationLossModel> pathLossModel{nullptr};
        Ptr<const SpectrumModel> spectralModel{nullptr};
        Ptr<ThreeGppSpectrumPropagationLossModel> spectrumPropModel{nullptr};
    };

    /// @brief Mobilities struct
    ///< to keep mobility model of ue and gNB
    struct Mobilities
    {
        Ptr<MobilityModel> ueMobility{nullptr};
        Ptr<MobilityModel> gnbMobility{nullptr};
    };

    /// @brief AntennaArrayModels struct
    ///< to store copy of antenna array of gNB and UE to do beamforming later.
    struct AntennaArrayModels
    {
        Ptr<UniformPlanarArray> gnbArrayModel{
            nullptr}; ///< Copy of gNB antenna array model. Modified to reduce complexity
        Ptr<UniformPlanarArray> ueArrayModel{nullptr}; ///< Copy of UE antenna array model
    };

    /// @brief LocalSearchParams struct
    ///< format to keep ChannelParams, Mobilities, and AntennaArrayModels of UE and gNB
    struct LocalSearchParams
    {
        ChannelParams chParams;
        Mobilities mobility;
        AntennaArrayModels antennaArrays;
    };

    /// @brief Check whether number of beams is corresponds to standard
    /// @return true if the number of beams conforms to standard
    bool CheckNumBeamsAllowed() const;

    /// @brief Get the type ID.
    /// @return the object TypeId
    static TypeId GetTypeId();

    /// @brief Set number of main interferer gNBs
    /// @param numInterfere Number of main interferer gNBs
    void SetNumMainInterfererGnb(uint8_t numInterfere);

    /// @brief Get number of main interferer gNBs
    /// @return Number of main interferer gNBs
    uint8_t GetNumMainInterfererGnb() const;

    /// @brief Set hand off margin in dB
    /// @param margin handoff margin
    /// @note Set m_handOffMargin in dB. UE attaches to any gNB whose RSRP is within hand off margin
    void SetHandoffMargin(double margin);

    /// @brief Get carrier frequency
    /// @return carrier frequency
    double GetCarrierFrequency() const;

    /// @brief Get handoff margin
    /// @return handoff margin
    double GetHandoffMargin() const;

    /// @brief Get row angles of the beam used
    /// @return row angles
    std::vector<double> GetRowBeamAngles() const;

    /// @brief Set row beam angles
    /// @param rowVect vector of row angles
    void SetRowBeamAngles(std::vector<double> rowVect);

    /// @brief Get col angles of the beam used
    /// @return col angles
    std::vector<double> GetColBeamAngles() const;

    /// @brief Set column beam angles
    /// @param colVect vector of column angles
    void SetColBeamAngles(std::vector<double> colVect);

    /// @brief Set UE device for which initial association is required
    /// @param ueDev UE device
    void SetUeDevice(const Ptr<NetDevice>& ueDev);

    /// @brief Set gnb devices among which association is done
    /// @param gnbDevices gnb devices
    void SetGnbDevices(const NetDeviceContainer& gnbDevices);

    /// @brief Get UE device for which initial association is required
    /// @return UE device
    Ptr<const NetDevice> GetUeDevice() const;

    /// @brief Create a container of gNBs forming a main interfering set with the UE
    /// @param numIntfs Number of interferer in the interfering set
    /// @param useRelRsrp If true use relative RSRP to form interfering set
    /// @param relRsrpThreshold Threshold of the ratio RSRP of the remaining interferers to main
    /// interferers, i.e., interferer whose power sum is below the threshold are not main
    /// interferers
    void InitializeIntfSet(uint16_t numIntfs, bool useRelRsrp, double relRsrpThreshold);

    /// @brief Calculate the cumulative sum of RSRP values from gNBs
    /// @param idxVal Index of the gNBs in increasing order of received power to the UE
    /// @return Cumulative sum of received RSRP from gNB wherein RSRP are in increasing order
    std::vector<double> GetInterference(const std::vector<uint16_t>& idxVal) const;

    /// @brief Calculate total interference based on RSRP values from gNBs
    /// @param cumSumIntf Cumulative sum of received RSRP from gNB wherein RSRP are in increasing
    /// order
    /// @return Total interference based on RSRP values from gNBs
    double GetTotalInterference(const std::vector<double>& cumSumIntf) const;

    /// @brief Get the number of interference gNB based on Relative RSRP value
    /// @param cumSumIntf Cumulative sum of received RSRP from gNB wherein RSRP are in increasing
    /// order
    /// @param relRsrpThreshold Threshold of the ratio RSRP of the remaining interferers to main
    /// interferers,
    /// @param  totalInterference Total interference based on RSRP values from gNBs
    /// @return Number of main interfering gNBs
    size_t GetNumIntfGnbsByRelRsrp(const std::vector<double> cumSumIntf,
                                   const double relRsrpThreshold,
                                   const double totalInterference) const;

    /// @brief Find the gNB associated with the UE
    /// @return pair of gNB and RSRP of associated gNB and
    /// @note Calculate the RSRP at this UE for all gNBs in the system. Association is not
    /// necessarily done with the gNB with max-RSRP. Instead, the UE randomly associates with one of
    /// the gNBs where the RSRP is within the handover margin of the max-RSRP
    std::pair<Ptr<NetDevice>, double> FindAssociatedGnb();

    /// @brief Get the gNB associated gNB with the UE
    /// @return gNB associated with the UE device
    Ptr<NetDevice> GetAssociatedGnb() const;

    /// @brief Get the gNBs which are main interferer with the UE
    /// @return A container having the set of main interfering gNBs
    NetDeviceContainer GetInterferingGnbs() const;

    /// @brief Get the max RSRP from a given gNB
    /// @param gnbId node ID of gNB
    /// @return Max RSRP received from gNB
    double GetMaxRsrp(uint64_t gnbId) const;

    /// @brief Get the best beam from a given gNB to the UE
    /// @param gnbId ID of gNB
    /// @return Best beam from a given gNB
    NrAnglePair GetBestBfv(uint64_t gnbId) const;

    /// @brief Get relative RSRP of remaining gNBs to that of the main one
    /// @return The relative RSRP ratio
    double GetRelativeRsrpRatio() const;

    /// @brief Set start RB for SSB
    /// @param startSsb
    void SetStartSsbRb(uint16_t startSsb);

    /// @brief Set number of RBs for association
    /// @param numSsbRb
    void SetNumSsbRb(uint16_t numSsbRb);

    /// @brief Get RSRP of associated gNB
    /// @return RSRP
    double GetAssociatedRsrp() const;

    /// @brief Set the primary BWP or carrier
    /// @param index
    void SetPrimaryCarrier(double index);

    /// @brief Get the primary BWP or carrier
    double GetPrimaryCarrier() const;

  private:
    /// @brief Extract information from ueDevice
    /// @return ChannelParamLocal
    LocalSearchParams ExtractUeParameters() const;

    /// @brief Extract information from gnbDevice
    /// @param searchParam
    /// @return Gnb antenna model
    /// @note For initial access beams typically have wider beams so limit the beams to first port
    /// of gNB antenna array
    Ptr<UniformPlanarArray> ExtractGnbParameters(const Ptr<NetDevice>& gnbDevice,
                                                 LocalSearchParams& searchParam) const;

    /// @brief Compute max RSRP in watts for gNB device by beamforming using LocalSearchParams
    /// @param gnbDevice gNB device
    /// @param lsps structure with parameters
    /// @return RSRP value
    double ComputeMaxRsrp(const Ptr<NetDevice>& gnbDevice, LocalSearchParams& lsps);

    /// @brief Compute sum of received power of UE antenna ports
    /// @param spectrumSigParam spectral signal parameters
    /// @return Sum of power in Watts/Hz at UE antennas ports
    double ComputeRxPsd(Ptr<const SpectrumSignalParameters> spectrumSigParam) const;

    /// @brief Generate beamforming vector of a given angle pair
    /// @param angRow Angle of beam direction in degrees for the row vector
    /// @param angCols Angle of beam direction in degree for the col vector
    /// @param gnbArrayModel array model for gNB
    /// @return beam forming vector
    PhasedArrayModel::ComplexVector GenBeamforming(double angRow,
                                                   double angCol,
                                                   Ptr<UniformPlanarArray> gnbArrayModel) const;

    /// @brief Compute the RSRP ratio
    /// @param totalRsrp total received RSRP
    /// @param idxVal index of sorted RSRP values
    double ComputeRsrpRatio(double totalRsrp, std::vector<uint16_t> idxVal);

    /// @brief  Generate and store RSRP values for a given UE to all gNB
    /// @param lsps search parameters specific to initial association
    void PopulateRsrps(LocalSearchParams& lsps);

    Ptr<const NetDevice> m_ueDevice{
        nullptr};                    ///< UE device for which associated gNB needs to be found
    NetDeviceContainer m_gnbDevices; ///< Set of gNB devices among which a UE will be associated to
    double m_freq;                   ///< Carrier frequency
    double m_handoffMargin{0.0};     ///< handoff margin (dB). See FindAssociatedGnb() for details.
    size_t m_startSsb{0};            ///< Starting resource block location of SSB
    size_t m_numBandsSsb{NR_NUM_BANDS_FOR_SSB}; ///< Number of bands used by SSB

    uint8_t m_numMainInterfererGnb{6};            ///< Number of main interferer gNBs
    size_t m_numIntfGnbs{m_numMainInterfererGnb}; ///< Number of main interfering gNBs
    double m_rsrpRatio{0.0};          ///< RSRP ratio of main interferer to remaining interferer
    NetDeviceContainer m_intfGnbDevs; ///< Set of main interfering gNB with the m_ueDevice
    std::vector<double> m_maxRsrps;   ///< Set of max RSRP values from different gNBs to UE
    double m_rsrpAsscGnb;
    Ptr<NetDevice> m_associatedGnb{nullptr}; ///< gNB with which m_ueDevice is associated

    std::vector<NrAnglePair> m_bestBfVectors; ///< vector of best BF vectors from gNBs to UE
    PhasedArrayModel::ComplexVector m_beamformingVector; ///< Beamforming vector resulting in
                                                         ///< highest RSRP with the associated gNB
    std::vector<double> m_rowBeamAngles{0, 90}; ///< Set of row angles in degrees of beamforming
                                                ///< vectors used in the initial access/association
    std::vector<double> m_colBeamAngles{0, 90}; ///< Set of column angles in degrees of beamforming
                                                ///< vectors used in the initial access/association

    double m_primaryCarrierIndex{0}; ////< Primary carrier bandwidth part index
};
} // namespace ns3
#endif // NR_INITIAL_ASSOC_H
