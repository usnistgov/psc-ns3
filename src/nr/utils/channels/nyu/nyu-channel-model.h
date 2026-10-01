// Copyright (c) 2023 New York University and NYU WIRELESS
// Users are encouraged to cite NYU WIRELESS publications regarding this work.
// Original source code is available in https://github.com/hiteshPoddar/NYUSIM_in_ns3
//
// SPDX-License-Identifier: MIT

#ifndef NYU_CHANNEL_H
#define NYU_CHANNEL_H

#include "nyu-channel-condition-model.h"

#include "ns3/angles.h"
#include <ns3/boolean.h>
#include <ns3/matrix-based-channel-model.h>
#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/random-variable-stream.h>

#include <complex.h>
#include <unordered_map>

namespace ns3
{

class MobilityModel;

/**
 * \ingroup spectrum
 * \brief Channel Matrix Generation following NYUChannelModel
 *
 * The class implements the channel matrix generation procedure
 *
 * \see GetChannel
 */
class NYUChannelModel : public MatrixBasedChannelModel
{
  public:
    /**
     * Constructor
     */
    NYUChannelModel();
    /**
     * Destructor
     */
    ~NYUChannelModel() override;

    void DoDispose() override;

    /**
     * Get the type ID
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Set the channel condition model
     * \param model a pointer to the ChannelConditionModel object
     */
    void SetChannelConditionModel(Ptr<ChannelConditionModel> model);

    /**
     * Get the associated channel condition model
     * \return a pointer to the ChannelConditionModel object
     */
    Ptr<ChannelConditionModel> GetChannelConditionModel() const;

    /**
     * Sets the center frequency of the model
     * \param freq the center frequency in Hz
     */
    void SetFrequency(double freq);

    /**
     * Returns the center frequency
     * \return the center frequency in Hz
     */
    double GetFrequency() const;

    /**
     * Sets the RF Bandwidth of the model
     * \param rfBandwidth the RF Bandwidth in Hz
     */
    void SetRfBandwidth(double rfBandwidth);

    /**
     * Returns the RF Bandwidth of the model
     * \return the RF Bandwidth in Hz
     */
    double GetRfBandwidth() const;

    /**
     * Sets the propagation scenario
     * \param scenario the propagation scenario
     */
    void SetScenario(const std::string& scenario);

    /**
     * Returns the propagation scenario
     * \return the propagation scenario
     */
    std::string GetScenario() const;

    /**
     * Looks for the channel matrix associated to the aMob and bMob pair in m_channelMatrixMap.
     * If found, it checks if it has to be updated. If not found or if it has to
     * be updated, it generates a new uncorrelated channel matrix using the
     * method GetNewChannel and updates m_channelMap.
     *
     * \param aMob mobility model of the a device
     * \param bMob mobility model of the b device
     * \param aAntenna antenna of the a device
     * \param bAntenna antenna of the b device
     * \return the channel matrix
     */
    Ptr<const ChannelMatrix> GetChannel(Ptr<const MobilityModel> aMob,
                                        Ptr<const MobilityModel> bMob,
                                        Ptr<const PhasedArrayModel> aAntenna,
                                        Ptr<const PhasedArrayModel> bAntenna) override;

    /**
     * Looks for the channel params associated to the aMob and bMob pair in
     * m_channelParamsMap. If not found it will return a nullptr.
     *
     * \param aMob mobility model of the a device
     * \param bMob mobility model of the b device
     * \return the channel params
     */
    Ptr<const ChannelParams> GetParams(Ptr<const MobilityModel> aMob,
                                       Ptr<const MobilityModel> bMob) const override;

    /**
     * \brief Assign a fixed random variable stream number to the random variables
     * used by this model.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream);

    /**
     * The measurements conducted by NYU are at 28,73 and 140 GHz. For other
     * frequencies a linear intrerpolation is done.
     * \param val1 the value of a parameter at 28 GHz
     * \param val2 the value of a parameter at 140 GHz
     * \param frequency the centrer frequency of operation in GHz
     * \return the value of a parameter at any frequency between 28-150 GHz
     */
    double GetCalibratedParameter(double val1, double val2, double frequency) const;

    /**
     * Find maximum value between two given values
     * \param val1 the first number
     * \param val2 the second number
     * \return the Maximum number between first and second number
     */
    double GetMaximumValue(double val1, double val2) const;

    /**
     * Find minimum value among two given values
     * \param val1 the first number
     * \param val2 the second number
     * \return the Minimum number between first and second number
     */
    double GetMinimumValue(double val1, double val2) const;

    /**
     * Generate a value as per signum function
     * \param value the input value of signum function
     * \return 1 if value > 0, 0 if value 0 and -1 if value is < 0
     */
    int GetSignum(double value) const;

    /**
     * Generate a random value following a poisson distribution
     * \param lambda the mean of the poisson distribution
     * \return a random integer value from a poisson distribution
     */

    int GetPoissionDist(double lambda) const;

    /**
     * Generate a random value following a binomial distribution
     * \param trials number of trials of the binomial distribution
     * \param success probability of success in binomial distribution
     * \return a random integer value from a binomial distribution
     */
    int GetBinomialDist(double trials, double success) const;

    /**
     * Generate a random value following a discrete uniform distribution
     * \param min the lower bound of the discrete uniform distribution
     * \param max the upper bound of the discrete uniform distribution
     * \return a random integer value from a discrete uniform distribution
     */
    int GetDiscreteUniformDist(const double min, const double max) const;

    /**
     * Generate a random value following a uniform distribution
     * \param min the lower bound of the uniform distribution
     * \param max the upper bound of the uniform distribution
     * \return a random value from a uniform distribution
     */
    double GetUniformDist(const double min, const double max) const;

    /**
     * Generate a random value following an exponential distribution
     * \param lambda the mean of the exponential distribution
     * \return a random value from an exponential distribution
     */
    double GetExponentialDist(double lambda) const;

    /**
     * Generate a random value following a gamma distribution
     * \param alpha the shape parameter of the gamma distribution
     * \param beta the rate parameter of the gamma distribution
     * \return a random value from a gamma distribution
     */
    double GetGammaDist(double alpha, double beta) const;

    /**
     * Get the number of Time Clusters
     * \param maxNumberOfTimeCluster the maximum number of Time Cluster for UMi,UMa and RMa
     * \param lambdaC mean value of the number of time cluster for InH
     * \return the number of Time Clusters
     */
    int GetNumberOfTimeClusters(double maxNumberOfTimeCluster, double lambdaC) const;

    /**
     * Get the number of Angle of Arrival (AOA) Spatial Lobes i.e. the Rx Spatial Lobes
     * \param muAoa the mean value of the number of Angle of Arrival (AOA) Spatial Lobes
     * \return the number of Angle of Arrival (AOA) Spatial Lobes
     */
    int GetNumberOfAoaSpatialLobes(double muAoa) const;

    /**
     * Get the number of Angle of Departure (AOD) Spatial Lobes i.e. the Tx Spatial Lobes
     * \param muAod the mean value of the number of Angle of Departure (AOD) Spatial Lobes
     * \return the number of Angle of Departure (AOD) Spatial Lobes
     */
    int GetNumberOfAodSpatialLobes(double muAod) const;

    /**
     * Get the number of Subpaths/Multipaths/rays in each Time Cluster which is frequency dependent
     * \param numberOfTimeClusters the number of Time Clusters in UMi,UMa and RMa
     * \param maxNumberOfSubpaths the maximum number of Subpaths in UMi,UMa and RMa
     * \param betaS the scaling factor for InH and InF
     * \param muS the mean of the exponential distribution for InH and InF
     * \param frequency the frequency of operation
     * \return the number of Subpaths in each Time Cluster
     */
    MatrixBasedChannelModel::DoubleVector GetNumberOfSubpathsInTimeCluster(
        int numberOfTimeClusters,
        double maxNumberOfSubpaths,
        double betaS,
        double muS,
        double frequency) const;

    /**
     * Get the Subpath delay in each Time Cluster (in ns) which is frequency dependent
     * \param numberOfSubpathInTimeCluster the number of subpaths in each time cluster
     * \param Xmax the mean subpath delay in each time cluster (in ns) for frequency < 100 GHz in
     * Umi,Uma and Rma
     * \param muRho the mean subpath delay in each time cluster (in ns)
     * \param alphaRho alpha of gamma distribution for subpath delay in each time cluster (in ns)
     * for InF
     * \param betaRho beta of gamma distribution for subpath delay in each time cluster (in
     * ns) for InF
     * \param frequency the frequency of operation
     * \return the delay of each Subpath in each Time Cluster (in ns)
     */
    MatrixBasedChannelModel::Double2DVector GetIntraClusterDelays(
        MatrixBasedChannelModel::DoubleVector numberOfSubpathInTimeCluster,
        double Xmax,
        double muRho,
        double alphaRho,
        double betaRho,
        double frequency) const;

    /**
     * Get the Subpath phases of each Subapath in each Time Cluster
     * \param numberOfSubpathInTimeCluster the number of subpath in each Time Cluster
     * \return the phases of each subpath in each Time Cluster
     */
    MatrixBasedChannelModel::Double2DVector GetSubpathPhases(
        MatrixBasedChannelModel::DoubleVector numberOfSubpathInTimeCluster) const;

    /**
     * Get the Delay of each Time Cluster (in ns)
     * \param muTau the mean excess delay of each Time Cluster (in ns) for UMi,UMa, RMa and InH
     * \param subpathDelayInTimeCluster the subpath delay in each Time Cluster (in ns)
     * \param minimumVoidInterval the mimumum time in ns by which two Time Clusters are separated
     * (in ns)
     * \param alphaTau the alpha value of the gamma distribution for Time Cluster delay (in
     * ns) for InF
     * \param betaTau the beta value of the gamma distribution for Time Cluster delay
     * (in ns) for InF
     * \return the delay of each Time Cluster (in ns)
     */
    MatrixBasedChannelModel::DoubleVector GetClusterExcessTimeDelays(
        double muTau,
        MatrixBasedChannelModel::Double2DVector subpathDelayInTimeCluster,
        double minimumVoidInterval,
        double alphaTau,
        double betaTau) const;

    /**
     * Get the Normalized Power of each Time Cluster (in Watts)
     * \param getClusterExcessTimeDelays the mean excess delay of each Time Cluster (in ns)
     * \param sigmaCluster the shadowing value in each Time Cluster (in dB)
     * \param timeClusterGamma the Time Cluster decay constant (in ns)
     * \return the Normalized Power in each Time Cluster (in Watts)
     */
    MatrixBasedChannelModel::DoubleVector GetClusterPowers(
        MatrixBasedChannelModel::DoubleVector getClusterExcessTimeDelays,
        double sigmaCluster,
        double timeClusterGamma) const;

    /**
     * Get the Normalized Power of each Subpath in a Time Cluster (in Watts)
     * \param subpathDelayInTimeCluster number of SubPaths in each Time Cluster (in ns)
     * \param timeClusterPowers Normalized Power of the Time Clusters (in Watts)
     * \param sigmaSubpath shadowing of each Subpath (in dB)
     * \param subpathGamma the decay constant of each Subpath (in ns)
     * \param los the value holding if the channel condition is Los or Nlos
     * \return the Normalized Power of each Subpath in a Time Cluster (in Watts)
     */
    MatrixBasedChannelModel::Double2DVector GetSubpathPowers(
        MatrixBasedChannelModel::Double2DVector subpathDelayInTimeCluster,
        MatrixBasedChannelModel::DoubleVector timeClusterPowers,
        double sigmaSubpath,
        double subpathGamma,
        bool los) const;

    /**
     * Get the Absolute propagation time of each subpath
     * \param distance2D the 2D distance between Tx and Rx nodes
     * \param delayOfTimeCluster the delay of each Time Cluster (in ns)
     * \param subpathDelayInTimeCluster the subpath delay in each Time Cluster (in ns)
     * \return the absolute propagation time of each subpath in a Time Cluster (in ns)
     */
    MatrixBasedChannelModel::Double2DVector GetAbsolutePropagationTimes(
        double distance2D,
        MatrixBasedChannelModel::DoubleVector delayOfTimeCluster,
        MatrixBasedChannelModel::Double2DVector subpathDelayInTimeCluster) const;

    /**
     * Get the Mapping of each Subpath and the Azimuth and Elevation angles w.r.t to the Spatial
     * Lobe
     * \param numberOfSpatialLobes the number of Spatial Lobes
     * \param numberOfSubpathInTimeCluster the number of subpaths in each Time Cluster
     * \param mean the mean angle of the Spatial Lobe (in degrees)
     * \param sigma the standard deviation of the mean of the Spatial Lobe (in degrees)
     * \param stdRMSLobeElevationSpread the standard deviation of the elevation offset from the lobe
     * centroid (in degrees)
     * \param stdRMSLobeAzimuthSpread the standard deviation of the azimuth offset from the lobe
     * centroid (in degrees) \param azimuthDistributionType the distribution of azimuth angles of
     * Subpaths \param elevationDistributionType the distribution of elevations angles of Subpaths
     * \return the Time Cluster ID, Subpath ID, Spatial Lobe ID, Azimuth angle of
     * the Subpaths, Elevation angle of the Subpath
     */
    MatrixBasedChannelModel::Double2DVector GetSubpathMappingAndAngles(
        int numberOfSpatialLobes,
        MatrixBasedChannelModel::DoubleVector numberOfSubpathInTimeCluster,
        double mean,
        double sigma,
        double stdRMSLobeElevationSpread,
        double stdRMSLobeAzimuthSpread,
        std::string azimuthDistributionType,
        std::string elevationDistributionType) const;
    /**
     * Create a database for the Subpath characteristics :- Time (in ns), Phase (in degrees), Power
     * (in Watts), AOD (in degree), ZOD (in degree), AOA (in degree) and ZOA (in degree)
     * \param numberOfSubpathInTimeCluster the number of Subpaths in each Time Cluster
     * \param absoluteSubpathdelayinTimeCluster the absolute delay of each Subpath (in ns)
     * \param subpathPower the normalized subpath power (in Watts)
     * \param subpathPhases the subpath phases
     * \param subpathAodZod the AOD and ZOD of the subpath
     * \param subpathAoaZoa the AOA and ZOA of the subpath
     * \return SP Absolute Delay(in ns), Power (rel to 1mW), Phase (radians), AOD, ZOD, AOA, ZOA
     * (all in degrees), AOD Spatial Lobe, AOA Spatial Lobe
     */
    MatrixBasedChannelModel::Double2DVector GetPowerSpectrum(
        MatrixBasedChannelModel::DoubleVector numberOfSubpathInTimeCluster,
        MatrixBasedChannelModel::Double2DVector absoluteSubpathdelayinTimeCluster,
        MatrixBasedChannelModel::Double2DVector subpathPower,
        MatrixBasedChannelModel::Double2DVector subpathPhases,
        MatrixBasedChannelModel::Double2DVector subpathAodZod,
        MatrixBasedChannelModel::Double2DVector subpathAoaZoa) const;

    /**
     * Combine generated subpaths depending on the RF Bandwidth. Wider bands have greater subpath
     * resolution when compared to narrow bands.
     * \param powerSpectrumOld the subpath charactersitcs
     * - Absolute Delay(in ns), Power (rel to 1mW), Phase (radians), AOD, ZOD, AOA, ZOA (all in
     * degrees), AOD Spatial Lobe, AOA Spatial Lobe
     * \param rfBandwidth the RF Bandwidth of operation
     * \param los the channel condition is either Los/Nlos
     * \return the final number of resolvable subpaths
     */
    MatrixBasedChannelModel::Double2DVector GetBWAdjustedtedPowerSpectrum(
        MatrixBasedChannelModel::Double2DVector powerSpectrumOld,
        double rfBandwidth,
        bool los) const;

    /**
     * The first subpath in LOS is aligned - this implies that AOD and AOA are aligned , ZOD and ZOA
     * are aligned.
     * \param powerSpectrum the subpath charactersitcs after bandwidth adjustment
     * \param los the value indicating if channel is Los/Nlos
     * \return the PowerSpectrum aligned for LOS
     */
    MatrixBasedChannelModel::Double2DVector GetLosAlignedPowerSpectrum(
        MatrixBasedChannelModel::Double2DVector& powerSpectrum,
        bool los) const;

    /**
     * Remove the subpaths with weak power
     * \param powerSpectrum the subpath charactersitcs adjusted as per RF Bandwidth
     * \param pwrthreshold the minimum detectable subpath power
     * \return PowerSpectrum having only the strong subpaths
     */
    MatrixBasedChannelModel::Double2DVector GetValidSubapths(
        MatrixBasedChannelModel::Double2DVector powerSpectrum,
        double pwrthreshold) const;

    /**
     * Get the XPD for each ray in the final PowerSpectrum
     * \param totalNumberOfSubpaths the number of subpath in each Time Cluster
     * \param xpdMean the mean value of the XPD
     * \param xpdSd the standard deviation of the XPD
     * \return the XPD value of each subpath in each Time Cluster
     */
    MatrixBasedChannelModel::Double2DVector GetXpdPerSubpath(double totalNumberOfSubpaths,
                                                             double xpdMean,
                                                             double xpdSd) const;

    /**
     * Convert Power in dB scale to linear scale
     * \param pwrdB the power in dB scale
     * \return the power in linear scale
     */
    double GetDbToPow(double pwrdB) const;

    /**
     * Convert the Subpath AOD,ZOD,AOA,ZOA generated in degrees using the NYU Coordinate System
     * (NYUCS) to Global Coordinate System (GCS) in degrees and transform the subpath
     * AOD,ZOD,AOA,ZOA from degrees to radians. \param powerSpectrum the database used to fetch the
     * AOD,ZOD,AOA,ZOA in degrees for each Subpath \return SP AOD,ZOD,AOA,ZOA in radians w.r.t GCS
     */
    MatrixBasedChannelModel::Double2DVector NYUCoordinateSystemToGlobalCoordinateSystem(
        MatrixBasedChannelModel::Double2DVector powerSpectrum) const;

    /**
     * Fetch the minimum detectable power in dB
     * \param distance2D the 2d distance between the TX and RX
     * \return the minimum power that can be detected by the NYU Channel Sounder
     */
    double DynamicRange(double distance2D) const;

  protected:
    struct NYUChannelParams : public MatrixBasedChannelModel::ChannelParams
    {
        ChannelCondition::LosConditionValue m_losCondition;
        ChannelCondition::O2iConditionValue m_o2iCondition;
        int numberOfTimeClusters = 0;    //!< value containing the number of Time Clusters
        int numberOfAoaSpatialLobes = 0; //!< value containing the number of AOA Spatial Lobes
        int numberOfAodSpatialLobes = 0; //!< value containing the number of AOD Spatial Lobes
        int totalSubpaths = 0;           //!< value containing the total number of Subpaths
        MatrixBasedChannelModel::DoubleVector
            numberOfSubpathInTimeCluster; //!< value containing the number of Subpaths in each time
                                          //!< cluster
        MatrixBasedChannelModel::DoubleVector
            delayOfTimeCluster; //!< value containing the delay of each time cluster
        MatrixBasedChannelModel::DoubleVector
            timeClusterPowers; //!< value containing the power of each time cluster
        MatrixBasedChannelModel::DoubleVector rayAodRadian; //!< the vector containing AOD angles
        MatrixBasedChannelModel::DoubleVector rayAoaRadian; //!< the vector containing AOA angles
        MatrixBasedChannelModel::DoubleVector rayZodRadian; //!< the vector containing ZOD angles
        MatrixBasedChannelModel::DoubleVector rayZoaRadian; //!< the vector containing ZOA angles
        MatrixBasedChannelModel::Double2DVector
            subpathDelayInTimeCluster; //!< value containing delay of each subpath in each time
                                       //!< cluster
        MatrixBasedChannelModel::Double2DVector
            subpathPhases; //!< value containing the Subpath phases of each each SP in each time
                           //!< cluster
        MatrixBasedChannelModel::Double2DVector
            subpathPowers; //!< value containing the power of each Subpath in each time cluster
        MatrixBasedChannelModel::Double2DVector
            absoluteSubpathDelayinTimeCluster; //!< value containing the absolute delay of each
                                               //!< subpath in each time cluster
        MatrixBasedChannelModel::Double2DVector
            subpathAodZod; //!< value containing the mapping(SP,TC,Lobe) and Subpath
                           //!< angles(Azimuth,Elevation) of AOD Lobe
        MatrixBasedChannelModel::Double2DVector
            subpathAoaZoa; //!< value containing the mapping(SP,TC,Lobe) and Subpath
                           //!< angles(Azimuth,Elevation) of AOA Lobe
        MatrixBasedChannelModel::Double2DVector
            powerSpectrumOld; //!< value containing SP characteristics: AbsoluteDelay(in ns),Power
                              //!< (relative to 1mW),Phases (radians),AOD (in degrees),ZOD (in
                              //!< degrees),AOA (in degrees),ZOA (in degrees)
        MatrixBasedChannelModel::Double2DVector
            powerSpectrum; //!< value containing SP characteristics - Adjusted according to RF
                           //!< bandwidth
        MatrixBasedChannelModel::Double2DVector
            xpd; //!< value containing the XPD (Cross Polarization Discriminator) in dB for each Ray
    };

    struct ParamsTable : public SimpleRefCount<ParamsTable>
    {
        /******** NYU Channel Parameters ************/
        // common parameters for UMi,UMa,RMa,InH and InF
        double muAod = 0;               //!< Max num of AOD Spatial Lobes
        double muAoa = 0;               //!< Max num of AOA Spatial Lobes
        double minimumVoidInterval = 0; //!< minVoidInterval Time in ns
        double sigmaCluster = 0;        //!< Per-cluster shadowing in dB
        double timeClusterGamma = 0;    //!< Time cluster decay constant in ns
        double sigmaSubpath = 0;        //!< per subpath shadowing in dB
        double subpathGamma = 0;        //!< subpath decay constant in ns
        double meanZod = 0;             //!< Mean zenith angle of departure (ZOD) in degrees
        double sigmaZod = 0;            //!< Standard deviation of the ZOD distribution in degrees
        double sdOfAodRmsLobeAzimuthSpread =
            0; //!< Standard deviation of the azimuth offset from the lobe centroid in degrees
        double sdOfAodRmsLobeElevationSpread =
            0; //!< Standard deviation of the elevation offset from the lobe centroid in degrees
        std::string aodRmsLobeAzimuthSpread;   //!< string specifying which distribution to use:
                                               //!< 'Gaussian' or 'Laplacian
        std::string aodRmsLobeElevationSpread; //!< string specifying which distribution to use:
                                               //!< 'Gaussian' or 'Laplacian
        double meanZoa = 0;                    //!< Mean zenith angle of arrival (ZOA) in degrees
        double sigmaZoa = 0; //!< Standard deviation of the ZOA distribution in degrees
        double sdOfAoaRmsLobeAzimuthSpread =
            0; //!< Standard deviation of the azimuth offset from the lobe centroid in degrees
        double sdOfAoaRmsLobeElevationSpread =
            0; //!< Standard deviation of the elevation offset from the lobe centroid
        std::string aoaRmsLobeAzimuthSpread;   //!< A string specifying which distribution to use:
                                               //!< 'Gaussian' or 'Laplacian
        std::string aoaRmsLobeElevationSpread; //!< A string specifying which distribution to use:
                                               //!< 'Gaussian' or 'Laplacian
        bool los; //!< boolean value indicating whether the channel condition is LOS or NLOS
        double xpdMean = 0; //!< Mean of XPD value
        double xpdSd = 0;   //!< standard deviation of XPD value
        // common parameters for for UMi,UMa and RMa
        double maxNumberOfTimeCluster = 0; //!< Max number of Time Clusters
        double maxNumberOfSubpaths = 0;    //!< Max number of Subpaths
        // common parameters for for UMi,UMa,RMa and InH
        double muTau = 0; //!< Mean excess Delay in ns
        double muRho = 0; //!< Intra cluster Delay in ns
        double Xmax = 0;  //!< Intra cluster Delay in ns for frequency less than 100 GHz
        // common parameters for for InH, InF
        double lambdaC = 0; //!< Mean number of time clusters
        double betaS = 0;   //!< Scaling factor for mean number of cluster sub-paths
        // parameters specific to InF
        double kS = 0;       //!< the shape of the number of cluster sub-paths for InF
        double sigmaS = 0;   //!< the scale factor for the number of cluster sub-paths for InF
        double thethaS = 0;  //!< the bound for the number of cluster sub-paths for InF
        double alphaTau = 0; //!< the alpha value for the gamma distribution for inter cluster delay
                             //!< (in ns) for InF
        double betaTau = 0;  //!< the beta value for the gamma distribution for inter cluster delay
                             //!< (in ns) for InF
        double alphaRho = 0; //!< the alpha value for the gamma distribution for intra cluster
                             //!< subpath delay (in ns) for InF
        double betaRho = 0; //!< the beta value for the gamma distribution for intra cluster subpath
                            //!< delay (in ns) for InF
        // parameters specific to InH
        double muS = 0; //!< Mean number of cluster sub-paths for InH
    };

    /**
     * Get the parameters needed to apply the channel generation procedure
     * \param channelCondition the channel condition
     * \return the parameters table
     */
    virtual Ptr<const ParamsTable> GetNYUTable(Ptr<const ChannelCondition> channelCondition) const;

    /**
     * Prepare NYU channel parameters among the nodes a and b.
     * The function does the following steps described in :
     *
     * Step 1: Generate number of time clusters N, spatial AOD Lobes and spatial AOA Lobes, and
     * subpaths in each time cluster Step 2: Generate the intra-cluster subpath delays rho_mn (ns)
     * Step 3: Generate the phases (rad) for each subpath
     * Step 4: Generate the cluster excess time delays tau_n (ns)
     * Step 5: Generate temporal cluster powers (mW)
     * Step 6: Generate the cluster subpath powers (mW)
     * step 7: Recover absolute propagation times t_mn (ns) of each subpath component
     * Step 8: Recover AODs and AOAs of the multipath components
     * Step 9: Construct the multipath parameters (AOA,ZOD,AOA,ZOA)
     * Step 10: combine SP which cannot be resolved and align the Subpath AOD,ZOD,AOA,ZOA if channel
     * is LOS Step 11: Generate the XPD values for each subpath All relevant generated parameters
     * are added then to NYUChannelParams which is the return value of this function.
     * \param channelCondition the channel condition
     * \param tablenyu the nyu parameters from the table
     * \param aMob the a node mobility model
     * \param bMob the b node mobility model
     * \return NYUChannelParams structure with all the channel parameters generated according to
     * steps from 1 to 9.
     */
    // To generate the MP characteristics. MP power, Phase, AOD,ZOD,AOA,ZOA
    Ptr<NYUChannelParams> GenerateChannelParameters(
        const Ptr<const ChannelCondition> channelCondition,
        const Ptr<const ParamsTable> tablenyu,
        const Ptr<const MobilityModel> aMob,
        const Ptr<const MobilityModel> bMob) const;

    /**
     * Compute the channel matrix between two nodes a and b, and their
     * antenna arrays aAntenna and bAntenna using the procedure
     * described in 3GPP TR 38.901
     * \param channelParams the channel parameters previously generated for the pair of nodes a and
     * b
     * \param tablenyu the NYU parameters table
     * \param sMob the mobility model of node s
     * \param uMob the mobility model of node u
     * \param sAntenna the antenna array of node s
     * \param uAntenna the antenna array of node u
     * \return the channel realization
     */

    virtual Ptr<ChannelMatrix> GetNewChannel(Ptr<const NYUChannelParams> channelParams,
                                             Ptr<const ParamsTable> tablenyu,
                                             const Ptr<const MobilityModel> sMob,
                                             const Ptr<const MobilityModel> uMob,
                                             Ptr<const PhasedArrayModel> sAntenna,
                                             Ptr<const PhasedArrayModel> uAntenna) const;
    /**
     * Check if the channel params has to be updated
     * \param channelParams channel params
     * \param channelCondition the channel condition
     * \return true if the channel params has to be updated, false otherwise
     */
    bool ChannelParamsNeedsUpdate(Ptr<const NYUChannelParams> channelParams,
                                  Ptr<const ChannelCondition> channelCondition) const;
    /**
     * Check if the channel matrix has to be updated (it needs update when the channel params
     * generation time is more recent than channel matrix generation time
     * \param channelParams channel params structure
     * \param channelMatrix channel matrix structure
     * \return true if the channel matrix has to be updated, false otherwise
     */
    bool ChannelMatrixNeedsUpdate(Ptr<const NYUChannelParams> channelParams,
                                  Ptr<const ChannelMatrix> channelMatrix);

    std::unordered_map<uint64_t, Ptr<ChannelMatrix>>
        m_channelMatrixMap; //!< map containing the channel realizations per pair of
                            //!< PhasedAntennaArray instances, the key of this map is reciprocal
                            //!< uniquely identifies a pair of PhasedAntennaArrays
    std::unordered_map<uint64_t, Ptr<NYUChannelParams>>
        m_channelParamsMap; //!< map containing the common channel parameters per pair of nodes, the
                            //!< key of this map is reciprocal and uniquely identifies a pair of
                            //!< nodes
    Time m_updatePeriod;    //!< the channel update period in ms
    double m_frequency;     //!< the operating frequency in Hz
    double m_rfBandwidth;   //!< the operating rf bandwidth in Hz
    std::string m_scenario; //!< the NYU scenario
    Ptr<ChannelConditionModel> m_channelConditionModel; //!< the channel condition model
    Ptr<UniformRandomVariable> m_uniformRv;             //!< uniform random variable
    Ptr<NormalRandomVariable> m_normalRv;               //!< normal random variable
    Ptr<ExponentialRandomVariable> m_expRv;             //!< exponential random variable
    Ptr<GammaRandomVariable> m_gammaRv;                 //!< gamma random variable
    // parameters for the blockage model
    bool m_blockage; //!< enables the blockage
};
} // namespace ns3

#endif /* NYU_CHANNEL_H */
