Sidelink
********

Similarly, for the Sidelink the LTE model currently supports the output to file of PHY and MAC level KPIs. You can enable them in the following way::

	Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

	// configure all the simulation scenario here...

	lteHelper->EnableSlPscchMacTraces ();
	lteHelper->EnableSlPsschMacTraces ();

	lteHelper->EnableSlRxPhyTraces ();
	lteHelper->EnableSlPscchRxPhyTraces ();

	Simulator::Run ();

The Sidelink MAC level KPIs are gathered at UE MAC and are written into two files, first one for PSCCH and the second for PSSCH. These KPIs are basically the trace of resource allocation for the Sidelink control and data transmissions for every SC period in MODE 1 and MODE 2. For PSCCH KPIs the format is the following:

  1. Simulation time in milliseconds at which the SC period starts (**Note : PSCCH transmission start time will depend on the PSCCH resource index**)
  2. Cell ID
  3. unique UE ID (IMSI)
  4. Sidelink-specific UE ID (RNTI)
  5. Current frame number
  6. Current subframe number
  7. PSCCH resource index
  8. Frame number for the first PSCCH transmission
  9. Subframe number for the first PSCCH transmission
  10. Frame number for the second PSCCH transmission
  11. Subframe number for the second PSCCH transmission
  12. MCS
  13. Transport block size
  14. Index of the first RB used for PSSCH (i.e., Data)
  15. Total number of RBs allocated for PSSCH
  16. Time Resource Pattern Index (iTRP) used for PSSCH

while for PSSCH MAC KPIs the format is:

  1. Simulation time in milliseconds at which the PSSCH transmission starts
  2. Cell ID
  3. unique UE ID (IMSI)
  4. Sidelink-specific UE ID (RNTI)
  5. Frame number at which the SC period starts
  6. Subframe number at which the SC period starts
  7. Frame number at which the PSSCH transmission starts
  8. Subframe number at which the PSSCH transmission starts
  9. Current frame number
  10. Current subframe number
  11. MCS
  12. Transport block size
  13. Index of the first RB used for PSSCH
  14. Total number of RBs allocated for PSSCH

The names of the files used for Sidelink MAC KPI output can be customized via the ns-3 attributes ``ns3::MacStatsCalculator::SlUeCchOutputFilename`` and ``ns3::MacStatsCalculator::SlUeSchOutputFilename``.

Sidelink PHY KPIs are distributed in two different files whose names are configurable through the following attributes

 1. ns3::PhyRxStatsCalculator::SlPscchRxOutputFilename
 2. ns3::PhyRxStatsCalculator::SlRxOutputFilename

In the SlPscchRx file, the following content is available:

  1. Simulation time in milliseconds
  2. Cell ID (Fixed to 0 for Sidelink)
  3. unique UE ID (IMSI)
  4. Sidelink-specific UE ID (RNTI)
  5. MCS
  6. Transport block size in bytes
  7. PSCCH resource index
  8. Total number of RBs for PSSCH
  9. Index of the first RB used for PSSCH
  10. Time Resource Pattern Index (iTRP) used for PSSCH
  11. Hopping value
  12. Layer 2 group destination id
  13. Correctness in the reception of the TB (correct = 1 : error = 0)

And Finally, in SlRx file the parameters included are:

  1. Simulation time in milliseconds
  2. Cell ID (Fixed to 0 for Sidelink)
  3. unique UE ID (IMSI)
  4. Sidelink-specific UE ID (RNTI) **Note: For PSDCH reception, it is the RNTI of the UE transmitting discovery messages.**
  5. Layer of transmission
  6. MCS
  7. Transport block size in bytes
  8. Redundancy version
  9. New Data Indicator flag
  10. Correctness in the reception of the TB (correct = 1 : error = 0)
  11. Average perceived SINR per RB in linear units

**Note: This file is used to store the above parameters for both PSSCH and PSDCH reception.**
