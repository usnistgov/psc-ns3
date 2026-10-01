<!--
Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)

SPDX-License-Identifier: GPL-2.0-only
-->

[![REUSE status](https://api.reuse.software/badge/gitlab.com/cttc-lena/nr)](https://api.reuse.software/info/gitlab.com/cttc-lena/nr)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7780747.svg)](https://doi.org/10.5281/zenodo.7780747)

# 3GPP NR ns-3 module #

This is the [ns-3](https://www.nsnam.org "ns-3 Website") nr module for the
simulation of 3GPP NR non-standalone cellular networks. ns-3 is used as a base
simulator, on top of which can be added our nr module as plug-in.

## License ##

This software is licensed under the terms of the GNU GPLv2, as like as ns-3.
See the LICENSE file for more details.

## Features

To see the features, please go to the [official webpage](https://5g-lena.cttc.es/features/).

## Papers

An updated list of published papers that are based on the outcome of this
module is available
[here](https://5g-lena.cttc.es/papers/).

## About

The [OpenSim](https://www.cttc.cat/open-simulations-opensim/) research group in CTTC is a group of highly skilled researchers,
with expertise in the area of cellular networks, O-RAN, radio resource management,
ML/AI based network management, with a focus on the following research lines:

- Developing models, algorithms, and architectures for next-generation virtualized open radio access networks
- Designing, implementing, validating, and evaluating 5G and beyond extensions in ns-3, including licensed/unlicensed/shared-based access and vehicular communications
- Deriving 3GPP/IEEE technologies coexistence and spectrum sharing strategies
- Designing radio resource, interference, and spectrum management techniques.

## Collaborations

Contact us if you think that we could collaborate! We are interested in research
projects with companies or other research institutions. Our [OpenSim](https://www.cttc.cat/open-simulations-opensim/)
research group has gained a vast experience in industrial research projects
through many successful projects with some of the top companies in the telecom
industry. We also organize secondments for excellent and motivated M.Sc./PhD students.
We can organize tutorials for the academy or industry.

## Authors ##

In alphabetical order:

- Zoraze Ali
- Amir Ashtari
- Biljana Bojovic
- Gabriel Ferreira
- Lorenza Giupponi
- Katerina Koutlia
- Sandra Lagen
- Ana Larranaga
- Natale Patriciello

Contributors: Tom Henderson, Giovanni Grieco, Carlos Herranz, André Apitzsch

Inspired by [mmWave module by NYU/UniPD](https://github.com/nyuwireless-unipd/ns3-mmwave)

## Contributing to 5G-LENA

We would be very happy if you would contribute to 5G-LENA!

If you identify a bug, please let us know through the
[Gitlab issue page](https://gitlab.com/cttc-lena/nr/-/issues/).

If you do some of the following with 5G-LENA, please open
a [merge request](https://gitlab.com/cttc-lena/nr/-/merge_requests)
or contact us to guide you through the contribution process:
- solve a bug;
- add new parameters for ease of use and flexibility;
- create a completely new and different example or test;
- develop a new feature;
- extend the tracing system through files or databases;
- improve visualization of scenarios through the python scripts;
- or whatever else you think is relevant and want to contribute.

When submitting patches, please follow our guide about
[NR Coding style and best practices](https://gitlab.com/cttc-lena/nr/-/blob/master/doc/development-guidelines.md)
which you should follow to keep the project as consistent and maintainable as possible. You can use the ns-3's `check-style-clang-format` utility for automatic C++ code formatting.

## Community

For users Q&A about 5G-LENA module you can join our 5G-LENA users group:
https://groups.google.com/g/5g-lena-users/

# Getting started {#getting-started}

In order to start with the nr module you should have some confidence with
the ns-3 environment. You can find on the ns-3 website all the necessary information.

If it is the first time you work with the ns-3 environment, we recommend to take
things slowly (but steady) and going forward through simple steps.
The ns-3 documentation <https://www.nsnam.org/documentation/> is divided into
three categories tutorial, manual and documentation (describing models):

- The ns-3 tutorial: <https://www.nsnam.org/docs/tutorial/html/index.html>
- The ns-3 manual: <https://www.nsnam.org/docs/manual/html/index.html>
- The LTE documentation: <https://www.nsnam.org/docs/models/html/lte.html>

After you get familiarized with ns-3, proceed to download and build the 5G-LENA
project, and we will point out to ns-3 and 5G-LENA documentation to help you
enter the ns-3 and the nr world.

## ns-3 + nr prerequisites

### ns-3 prerequisites:

Make sure to install all [ns-3 prerequisites](https://www.nsnam.org/docs/installation/html/quick-start.html#prerequisites)

### nr prerequisites: {#prerequisites}

Install libc6-dev (it provides `semaphore.h` header file).  The Ubuntu
package name is:

```
sudo apt-get install libc6-dev
```

Install sqlite (enables optional examples `lena-lte-comparison`,
`cttc-nr-3gpp-calibration` and `cttc-realistic-beamforming`):

```
apt-get install sqlite sqlite3 libsqlite3-dev
```

Install eigen3 (enables optional MIMO features):

```
apt-get install libeigen3-dev
```

Notice that ns-3 and nr prerequisites are required to use all nr features.
Otherwise, you will get a warning at configuration time
and/or an error message during compilation or execution.

### gsoc-nr-rl-based-sched Prerequisites

To run this example with AI mode enabled, you need to install the `ns3-gym` module.
You can find the installation guide for `ns3-gym` here: [ns3-gym Github](https://github.com/tkn-tub/ns3-gym)

For managing Python packages in a virtual environment, install `./model/ns3gym` without using the `--user` option.

```
python3 -m venv ./myenv
source ./myenv/bin/activate
pip install ns3-gym/model/ns3gym
```

To run `test-ppo.py`, install the following additional Python packages:

```
pip install numpy torch
```

## ns-3 + nr installation

Check in the [nr RELEASE_NOTES.md Supported platforms](https://gitlab.com/cttc-lena/nr/-/blob/master/RELEASE_NOTES.md#supported-platforms) which is the
recommended ns-3 release for each nr release.

For a quicker reference we provide a table with the supported versions of ns-3-dev
for each nr release.

| nr version     | ns-3 version | Build system  | Release date       |
| :------------: | :-----------:| :-----------: |--------------------|
| 5g-lena-v3.3.y | ns-3.42      | cmake         | October 15, 2024   |
| 5g-lena-v3.2.y | ns-3.42      | cmake         | September 25, 2024 |
| 5g-lena-v3.1.y | ns-3.42      | cmake         | July 19, 2024      |
| 5g-lena-v3.0.y | ns-3.41      | cmake         | February 16, 2024  |
| 5g-lena-v2.6.y | ns-3.40      | cmake         | November 30, 2023  |
| 5g-lena-v2.5.y | ns-3.39      | cmake         | July 26, 2023      |
| 5g-lena-v2.4.y | ns-3.38      | cmake         | April 5, 2023      |
| 5g-lena-v2.3.y | ns-3.37      | cmake         | November 23, 2022  |
| 5g-lena-v2.2.y | ns-3.36.1    | cmake         | June 03, 2022      |
| 5g-lena-v2.1.y | ns-3.36      | cmake         | May 06, 2022       |
| 5g-lena-v2.0.y | ns-3.36      | cmake         | April 21, 2022     |
| 5g-lena-v1.3.y | ns-3.35      | waf           | April 7, 2022      |
| 5g-lena-v1.2.y | ns-3-dev     | waf           | June 4, 2021       |
| 5g-lena-v1.1.y | ns-3-dev     | waf           | March 2, 2021      |
| 5g-lena-v1.0.y | ns-3-dev     | waf           | September 16, 2020 |
| 5g-lena-v0.4.y | ns-3-dev     | waf           | February 13 2020   |
| 5g-lena-v0.3.y | ns-3-dev     | waf           | August 27 2019     |
| 5g-lena-v0.2.y | ns-3-dev     | waf           | February 1 2019    |

###  Download ns-3:

Download then checkout the compatible version of ns-3:

```
git clone https://gitlab.com/nsnam/ns-3-dev.git
cd ns-3-dev
git checkout -b ns-3.42 ns-3.42
```

To make sure everything is working properly, run the ns-3 tests:

```
$ ./ns3 configure --enable-examples --enable-tests
$ ./test.py
```

A success for both previous commands indicates an overall success, and you are
ready to install the nr module.

### Download the nr module:

Download then checkout the compatible version of nr:

```
cd contrib
git clone https://gitlab.com/cttc-lena/nr.git
cd nr
git checkout -b 5g-lena-v3.3.y origin/5g-lena-v3.3.y
```

Notice that since these are two independent git repositories, when you run
`git status` inside of the ns-3, you will notice that the contrib/nr
directory will be listed as "Untracked files". This is normal.

### Test ns-3 + nr installation:

Let's configure the ns-3 + nr project:

```
./ns3 configure --enable-examples --enable-tests
```

In the output you should see:

```
SQLite support                : ON
Eigen3 support                : ON
```

If that is not the case, return to "ns-3 and nr prerequisites" section, and install all prerequisites.
After the installation of the missing packages run again `./ns3 configure --enable-tests --enable-examples`.

To compile the ns-3 with nr you can run the following command:

```
./ns3 build
```

If the nr module is recognized correctly, you should see "nr" in the list of
built modules. If that is the case, _Welcome to the nr world !_

## Running examples

Now, you can list the available nr examples.

Note that some of the examples may be missing if
the dependencies listed in [nr Prerequisites](#prerequisites)
section haven't been installed.

Also, the nr target listed below is the module library for nr,
so it **cannot** be used with `./ns3 run`, and only `./ns3 build nr`.
```
$ ./ns3 show targets | grep nr
nix-vector-routing            nr
cttc-lte-ca-demo              cttc-nr-3gpp-calibration-user
cttc-nr-cc-bwp-demo           cttc-nr-demo
cttc-nr-mimo-demo             cttc-nr-multi-flow-qos-sched
cttc-nr-notching              cttc-nr-simple-qos-sched
cttc-nr-traffic-3gpp-xr       cttc-nr-traffic-ngmn-mixed
```

Now, you can run one of these examples
(see the nr/examples folder for their source code):

```
$ ./ns3 run cttc-nr-demo
```

The `cttc-nr-demo` example has a complete tutorial explaining the
example in details available [here](https://cttc-lena.gitlab.io/nr/cttc-nr-demo-tutorial.pdf).

Note: when writing or modifying examples, be aware that
our code is stored under the ns3 namespace. You will either
need to use such namespace or add the prefix "ns3::" for
classes, structs and enums.

## Upgrading 5G-LENA {#upgrade}

We assume that your work lives in a separate branch, and that the 'master'
branch of the nr repository is left untouched as the first time you downloaded
it. If it is not the case, then please move all your work in a separate branch.

A vanilla 'master' branch can be updated by simply running:

```
cd ns-3-dev/contrib/nr    # or src/nr if the module lives under src/
git checkout master
git pull
```

## Documentation

We maintain three sources of documentation:

1. [The user manual](https://cttc-lena.gitlab.io/nr/nrmodule.pdf) describes the models and their assumptions; as
we developed the module while the 3GPP standard was not fully available, some parts
are not modeling precisely the procedures as indicated by the standard.

2. [The user tutorial](https://cttc-lena.gitlab.io/nr/cttc-nr-demo-tutorial.pdf) describes the internal
functionality of the NR RAN by providing a detailed, layer-by-layer insights on the packet lifecycle as
they traverse the RAN.

3. [The Doxygen](https://cttc-lena.gitlab.io/nr/html/), you will find details about design
and user usage of any class of the module, as well as description and
images for the examples and the tests.



### Building documentation

If you would like to build the documentation on your own, you can follow the
instructions from this section.

- To build the user manual, navigate to the nr folder and then:

```
cd doc
make latexpdf
```

And you will find the PDF user manual in the directory build/latex. Please note
that you may have to install some requirements to build the documentation; you
can find the list of packages for any Ubuntu-based distribution in the file
`.gitlab-ci.yml`.

- To build the doxygen documentation, please do from the nr folder:

```
cd doc
doxygen doxygen.conf
```

You will find the doxygen documentation inside `doc/doc/html/`.
