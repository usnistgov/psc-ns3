
.. _sec-sidelink-simulation-output:

--------------------------
Sidelink simulation output
--------------------------

Similarly, for the Sidelink the LTE model currently supports the output to file
of PHY, MAC and RRC level KPIs. You can enable them in the following way::

        Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

        // configure all the simulation scenario here...

        //Transmission traces
        lteHelper->EnableSlPscchMacTraces ();
        lteHelper->EnableSlPsschMacTraces ();
        lteHelper->EnableSlPsdchMacTraces ();

        //Reception traces
        lteHelper->EnableSlRxPhyTraces ();
        lteHelper->EnableSlPscchRxPhyTraces ();
        lteHelper->EnableDiscoveryMonitoringRrcTraces ();

        Simulator::Run ();

A helper method exists to enable all six traces above in one statement::

        Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

        // configure all the simulation scenario here...

        lteHelper->EnableSidelinkTraces ();

        Simulator::Run ();

The Sidelink PSCCH and PSSCH MAC level KPIs are gathered at UE MAC and are
written into two files, first one for PSCCH and the second for PSSCH.
These KPIs are basically the trace of resource allocation for the Sidelink
control and data transmissions for every SC period in MODE 1 and MODE 2.
For PSCCH KPIs the format is the following:

  1. Simulation time in milliseconds
  2. Cell ID
  3. unique UE ID (IMSI)
  4. Sidelink-specific UE ID (RNTI)
  5. Current frame number
  6. Current subframe number
  7. Sidelink Control (SC) period starting frame number
  8. Sidelink Control (SC) period starting subframe number
  9. PSCCH resource index
  10. Transport block size (PSCCH)
  11. Total number of RBs allocated for PSCCH
  12. Index of the first RB used for PSCCH
  13. Hopping flag
  14. Hopping value (value of 255 is assigned when hopping is not enabled)
  15. Total number of RBs allocated for PSSCH
  16. Index of the first RB used for PSSCH
  17. Time Resource Pattern Index (iTRP) used for PSSCH
  18. MCS
  19. Layer 1 group destination id (8 LSBs of the L2 group destination id at UE
      MAC)
  20. Drop flag (dropped = 1 : not dropped = 0)

while for PSSCH MAC KPIs the format is:

  1. Simulation time in milliseconds
  2. Cell ID
  3. unique UE ID (IMSI)
  4. Sidelink-specific UE ID (RNTI)
  5. Current frame number
  6. Current subframe number
  7. Frame number at which the SC period starts
  8. Subframe number at which the SC period starts
  9. Total number of RBs allocated for PSSCH
  10. Index of the first RB used for PSSCH
  11. MCS
  12. Transport block size
  13. Redundancy version
  14. Drop flag (dropped = 1 : not dropped = 0)

Similarly, the file containing the PSDCH MAC KPIs have the following content:

  1. Simulation time in milliseconds
  2. Cell ID
  3. unique UE ID (IMSI)
  4. Sidelink-specific UE ID (RNTI)
  5. Current frame number
  6. Current subframe number
  7. Frame number at which the discovery period starts
  8. Subframe number at which the discovery period starts
  9. PSDCH resource index
  10. Total number of RBs allocated for PSSCH
  11. Index of the first RB used for PSSCH
  12. MCS
  13. Transport block size
  14. Redundancy version
  15. Discovery message type
  16. Discovery content type
  17. Discovery model
  18. Content. The information in this field vary depending on the type of UE:

      * ``Normal UE:`` Application code
      * ``Relay UE:`` Relay Service Code; Announcer info; Relay UE Id;
        Status indicator; Spare

  19. Drop flag (dropped = 1 : not dropped = 0)

The names of the files used for Sidelink MAC KPI output can be customized via
the ns-3 attributes:

 * ``ns3::MacStatsCalculator::SlCchOutputFilename``
 * ``ns3::MacStatsCalculator::SlSchOutputFilename``
 * ``ns3::MacStatsCalculator::SlDchOutputFilename``

Sidelink PHY KPIs are distributed in two different files whose names are
configurable through the following attributes:

 * ``ns3::PhyRxStatsCalculator::SlCchRxOutputFilename``
 * ``ns3::PhyRxStatsCalculator::SlRxOutputFilename``

In the SlPscchRx file, the following content is available:

  1. Simulation time in milliseconds
  2. Cell ID (Fixed to 0 for Sidelink)
  3. Unique UE ID (IMSI)
  4. Sidelink-specific UE ID (RNTI) **Note: It is the RNTI of transmitting UE**
  5. PSCCH resource index
  6. Transport block size (PSCCH) in bytes
  7. Hopping flag
  8. Hopping value (value of 255 is assigned when hopping is not enabled)
  9. Total number of RBs for PSSCH
  10. Index of the first RB used for PSSCH
  11. Time Resource Pattern Index (iTRP) used for PSSCH
  12. MCS
  13. Layer 1 group destination id (8 LSBs of the L2 group destination id at UE
      MAC)
  14. Correctness in the reception of the TB (correct = 1 : error = 0)

The parameters of SlRx file, which stores the PSSCH, PSDCH and PSBCH PHY
reception results are:

  1. Simulation time in milliseconds
  2. Cell ID (Fixed to 0 for Sidelink)
  3. Unique UE ID (IMSI)
  4. Sidelink-specific UE ID (RNTI) **Note: It is the RNTI of transmitting UE**
  5. Layer of transmission
  6. MCS
  7. Transport block size in bytes
  8. Redundancy version
  9. New Data Indicator flag
  10. Correctness in the reception of the TB (correct = 1 : error = 0)
  11. Average perceived SINR per RB in linear units

**Note: At the time of writing this documentation, the RNTI for the PSBCH
signals is not reported, thus, is set to zero.**

And Finally, the name of the file, which stores the RRC level KPIs for PSDCH
monitoring is configurable through the attribute
``ns3::RrcStatsCalculator::SlDchRxRrcOutputFilename``. The content of this
file is the following:

  1. Simulation time in milliseconds
  2. Cell ID
  3. unique UE ID (IMSI)
  4. Sidelink-specific UE ID (RNTI)
  5. Discovery message type
  6. Discovery content type
  7. Discovery model
  8. Content. The information in this field vary depending on the type of UE:

     * ``Normal UE:`` Application code
     * ``Relay UE:`` Relay Service Code; Announcer info; Relay UE Id;
       Status indicator; Spare
