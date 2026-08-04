// Copyright (c) 2010 TELEMATICS LAB, DEE - Politecnico di Bari
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Original Author: Giuseppe Piro  <g.piro@poliba.it>
// Modified by:     Nicola Baldo   <nbaldo@cttc.es>
// Modified by:     Marco Miozzo   <mmiozzo@cttc.es>

#ifndef NR_LTE_AMC_H
#define NR_LTE_AMC_H

#include <ns3/object.h>
#include <ns3/ptr.h>

#include <vector>

namespace ns3
{

class SpectrumValue;

/**
 * \ingroup lte
 * Implements the Adaptive Modulation And Coding Scheme. As proposed in 3GPP
 * TSG-RAN WG1 [R1-081483 Conveying MCS and TB size via PDCCH]
 * (http://www.3gpp.org/ftp/tsg_ran/WG1_RL1/TSGR1_52b/Docs/R1-081483.zip).
 */
class NrLteAmc : public Object
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    NrLteAmc();
    ~NrLteAmc() override;

    /// Types of AMC model.
    enum AmcModel
    {
        /**
         * \details
         * An AMC model based on Piro, G.; Grieco, L.A; Boggia, G.; Camarda, P.,
         * "A two-level scheduling algorithm for QoS support in the downlink of
         * LTE cellular networks," _Wireless Conference (EW), 2010 European_,
         * pp.246,253, 12-15 April 2010.
         */
        PiroEW2010,
        /**
         * An AMC model based on 10% of BER according to NrLteMiErrorModel.
         */
        MiErrorModel
    };

    /**
     * \brief Get the Modulation and Coding Scheme for
     * a CQI value
     * \param cqi the cqi value
     * \return the MCS value
     */
    int GetMcsFromCqi(int cqi);

    /**
     * \brief Get the Transport Block Size for a selected MCS and number of PRB (table 7.1.7.2.1-1
     * of 36.213)
     * \param mcs the MCS index
     * \param nprb the no. of PRB
     * \return the Transport Block Size in bits
     */
    int GetDlTbSizeFromMcs(int mcs, int nprb);

    /**
     * \brief Get the Transport Block Size for a selected MCS and number of PRB (table 8.6.1-1
     * of 36.213)
     * \param mcs the MCS index
     * \param nprb the no. of PRB
     * \return the Transport Block Size in bits
     */
    int GetUlTbSizeFromMcs(int mcs, int nprb);

    /**
     * \brief Get a proper CQI for the spectral efficiency value.
     * In order to assure a lower block error rate, the AMC chooses the lower CQI value
     * for a given spectral efficiency
     * \param s the spectral efficiency
     * \return the CQI value
     */
    int GetCqiFromSpectralEfficiency(double s);

  private:
    /**
     * The `Ber` attribute.
     *
     * The requested BER in assigning MCS (default is 0.00005).
     */
    double m_ber;

    /**
     * The `AmcModel` attribute.
     *
     * AMC model used to assign CQI.
     */
    AmcModel m_amcModel;

}; // end of `class NrLteAmc`

} // namespace ns3

#endif /* NR_LTE_AMC_H */
