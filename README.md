-------------------------------------------------------------------------------
#      LTE Device to Device Communication Model for ns-3  V0.1 (06/13/2017)
-------------------------------------------------------------------------------

This repository host the development of an LTE D2D model for ns-3. 

The implemented model is based on the LTE module available in ns-3.22, and it 
supports the following additional features:

1. Out-of-coverage D2D synchronization
2. D2D Discovery (in and out of coverage, UE_SELECTED mode only)
3. D2D Communication (in coverage not fully tested)
4. Static (configurable) resource scheduler 
5. New physical layer error model (BLER curves)
6. Various channel models


Please refer to the following publication for further information about the
LTE D2D functionalities background, and the model implementation and validation
details:

R. Rouil, F. Cintron, A. Ben Mosbah and S. Gamboa, "Implementation and 
Validation of an LTE D2D Model for ns-3", Proceedings of the Workshop
on ns-3 (WNS3 2017), June 2017, Porto, Portugal.

Link: http://dl.acm.org/citation.cfm?id=3067668

If you use our model in your research, please cite the above-mentioned paper.

###### Future work:

The following features are intended to be added to the project in the future: 

1. In-coverage and partial coverage validation
2. UE-to-Network relay
3. Resource pool priority 
4. Discovery SCHEDULED mode


## Installation


1. Clone or download the source code from this repository 
2. Go into the LTE D2D ns-3 root directory (**~/d2d-ns-3.22/**) 
3. Configure the project using the command 
```
  ./waf configure  
```
4. Build the project using the command
```
  ./waf build
```

To build the project in optimized mode for fast execution modify the command
of step 3 as follows, and proceed afterwards with step 4. 
```
  ./waf configure -d optimized 
```  
 
## Usage


The project includes several sets of simulation scripts created to validate
the implemented functionalities, which can be found in the folder: 

**~/d2d-ns-3.22/src/nist/examples/**

Each set contains a README file with the description of the scripts, the
instructions to follow to run them, and the expected output.  

### Support


**Installation support:**

Due to limited resources, we will provide assistance only for issues
related to the installation of this code. 
We strongly recommend to install the standard version of ns-3 prior to install
our code. 

   Link: https://www.nsnam.org/docs/tutorial/html/getting-started.html

Like that, the users can get support for any issue related to ns-3 installation
from the ns-3 community. 

   Link: https://www.nsnam.org/support/mailing-list/
   
Afterwards, the user can install our project and report to us only the issues
related to our model installation.

**Bugs:**

If you find a bug when running the provided scenarios, please report it
including detailed information about the scenario and error(s) observed.

Please report issues to psc_ns3@nist.gov


## License


This software was developed at the National Institute of Standards and 
Technology by employees of the Federal Government in the course of their
official duties. Pursuant to titleElement 17 Section 105 of the United States
Code this software is not subject to copyright protection and is in the public
domain. NIST assumes no responsibility whatsoever for its use by other parties,
and makes no guarantees, expressed or implied, about its quality, reliability,
or any other characteristic. 

We would appreciate acknowledgment if the software is used.  

NIST ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND DISCLAIM ANY
LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF 
THIS SOFTWARE.                     

## About the Model

The model is developed and maintained by the Public Safety Communication 
Project Group of the Wireless Networks Division (WND) at the Communication
Thechnology Laboratory (CTL) of the National Institute of Standards and 
Technology (NIST) - U.S. Department of Commerce. 
For further information about the research conducted by the group, please refer
to the following website: 
https://www.nist.gov/programs-projects/public-safety-communications
