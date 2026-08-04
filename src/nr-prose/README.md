# ns-3 nr-prose module
An extension to the nr V2X code to support 5G Proximity Services (ProSe) including Discovery and Layer-3 UE-to-Network Relay.

# Table of Contents

* [About](#about)
* [Requirements](#requirements)
* [Installation](#installation)
    * [Clone (Recommended)](#clone-recommended)
    * [Download ZIP](#download-zip)
    * [Connecting the Module Quickly](#connecting-the-module-quickly)
  * [Updating](#updating)
    * [Clone](#clone)
    * [ZIP](#zip)
* [Documentation](#documentation)
  * [Building the Documentation](#building-the-documentation)

# About
This is an [ns-3](https://www.nsnam.org/) extension module for simulating
[5G Proximity Services](https://portal.3gpp.org/desktopmodules/Specifications/SpecificationDetails.aspx?specificationId=3864).

# Requirements
* A C++ 20 compliant compiler.
* A compatible version of [ns-3](https://github.com/usnistgov/psc-ns3/) that has been patched for ProSe support.
* A compatible version of the CTTC NR module, V2X branch.

# Installation
## Clone (Recommended)
Clone the project into a directory called `nr-prose` in
the `contrib` directory of a supported version of ns-3

1) `cd` into the `contrib` directory of `ns-3`

```shell
cd contrib/
```

2) Clone the project from one of the below URLs
```shell
# Pick one of the below
# HTTPS (Choose this one if you're uncertain)
git clone https://github.com/usnistgov/nr-prose-ns3-module nr-prose

# SSH
git clone git@github.com:usnistgov/nr-prose-ns3-module.git nr-prose
```

3) (Re)configure & (Re)build `ns-3`
```shell
# --enable-examples is optional, see `Running the Examples`
# for how to run them
./ns3 configure --enable-examples
./ns3
```

## Download ZIP
If, for whatever reason, `git` is not available, download the
project & unzip it into the `contrib` directory of `ns-3`.

Note that updates will have to be performed manually using this method

1) Download the ZIP of the project from the url below:

https://github.com/usnistgov/nr-prose-ns3-module/archive/master.zip

2) Unzip the file into the `ns-3` `contrib/` directory
```shell
unzip nr-prose-ns3-module-master.zip
```

3) Rename the resulting directory to `nr-prose`, as ns-3 will not accept a module named differently
than its directory.
```shell
mv nr-prose-ns3-module-master nr-prose
```

## Connecting the Module Quickly
If you are linking your module/program to the `nr-prose` module add the following to your `CMakeLists.txt`.

### CMake
For CMake, add the module's target `${libnr-prose}` to your `libraries_to_link` list
```cmake
# Example
build_lib_example(
  NAME your-example-name
  SOURCE_FILES example.cc
  LIBRARIES_TO_LINK
    ${libnr-prose}
    # ...
)

# Module
build_lib(
  LIBNAME your-module-name
  SOURCE_FILES
    # ...
  HEADER_FILES
    # ...
  LIBRARIES_TO_LINK
    ${libnr-prose}
    # ...
)
```

You may now include & use the `nr-prose` module in code:
```cpp
#include <ns3/nr-prose-module.h>
//...

int main ()
{
    // ...
    // ...
}
```

## Updating
### Clone
To update the cloned module, move to the module's root directory and perform a `git pull`
```shell
# From the ns-3 root
cd contrib/nr-prose
git pull
```

### ZIP
To update a ZIP installation, remove the old module and replace it with the updated one.

```shell
# From the ns-3 root
cd contrib
rm -Rf nr-prose

#use this command, or download manually
wget https://github.com/usnistgov/nr-prose-ns3-module/archive/refs/heads/master.zip -O nr-prose-ns3-module-master.zip
unzip nr-prose-ns3-module-master.zip

# Make sure the directory in the ns-3 contrib/ directory is
# named `nr-prose`
mv nr-prose-ns3-module-master nr-prose


```

# Documentation

## Building the Documentation
[Sphinx](https://www.sphinx-doc.org/en/master/) is required to build the documentation.

To run Sphinx to build the documentation, cd into the `doc` directory in the module
and run `make [type]` for the type of documentation you wish to build.

```shell
# From the ns-3 root directory
cd contrib/nr-prose/doc

# HTML (Several Pages)
make html

# HTML (One Page)
make singlehtml

# PDF
make latexpdf

# To list other options, just run make
make
```

The built documentation will now be found in `doc/build/[type]`.

