# HELICS Simulink interface

This repository contains the tools and files required for using HELICS with Simulink.
For HELICS documentation see [HELICS.org](www.helics.org)

## Installation and Requirements

For other platforms the installation is as simple as downloading this repository and inside matlab executing the `buildSimHELICS` Matlab script.

This should download the required HELICS binary files and compile the platform specific mex file.  This does require a compiler be installed on the system and executing inside matlab

``` matlab
>> mex -setup
```

This setup is only once on any given Matlab installation and does not need to be repeated when upgrading HELICS

```matlab
>> cd simHELICS
>> buildSimHELICS('../helics');
```

this will generate the helics interface in a folder 1 level above the simHELICS directory.  If desired it can also be built in the simHELICS directory for example:

```matlabs
>> cd simHELICS
>> buildSimHELICS();
```

The `buildSimHELICS` function also has an option to generate the package file.  This can be used to generate a package that can be transfered to a similar system.

This is a new process for building a simulink interface, it is tested but not extensively and not on a large variety of platforms if you run into issues please let us know.

### Requirements

Matlab 2018a or greater.
intalled compatible compiler to build the mex file

### building with custom HELICS version

Just using with an updated HELICS library only requires changing the helics[dll|so|dylib] file and no other modification is necessary.
Updating a version with modified simulink code files would require updating the simHelics repository via git and rerunning buildSimHELICS.

## Using the library

The library consists of 3 blocks.

- helics_federate: for creating the federate
- helics_input: for getting data from HELICS
- helics_publication: for sending data to HELICS

These simulink blocks are in a simHELICS.slx library object and can be loaded or dragged as necessary

### federate block

The federate block takes 1 parameter which is the name of a config file.   This config file is typically json, but other formats including a direct string input is allowed.   This configuration json is the only means of configuring the federate in simulink.  All interfaces should be specified in this configuraiton file.

### input block

The input block is used to retreive data from HELICS.  It takes two parameters.  The first is an integer or string with the input index(zero based) or name resepectively, and some arguments as a string as the second argument.   Allowed arguments are 'output_size=X',  and 'output_type=XXXX'.  These define the size and type of the simulink output on the block.  Default for an empty string is 1, and double. 'double','int', 'bool', and 'complex' are supported along with 'vector' and 'complex_vector'.   The HELICS interface type is defined in the configuration.  This is defining the desired output type in simulink. 

The input_block is wired to the helics_federate block.  

### Publication block

The publication block is used to send data via HELICS.  The first is an integer or string with the publication index(zero based) or name resepectively, and some arguments as a string as the second argument.   Allowed arguments are 'input_size=X',  and 'input_type=XXXX'.  These define the size and type of the second simulink input on the block.  Default for an empty string argument is 1, and double. 'double','int', 'bool', and 'complex' are supported along with 'vector' and 'complex_vector'.   The HELICS interface type is defined in the configuration.  This is defining the desired input type and size in simulink.

The publication is block is wired to the helics_federate block and the data wishing to be published.

### Example

A trivial example federate (test_fed.slx) is located in the example folder, along with a simple config file.   This federate sends data to itself using a inproc federate and plots the result.

## Building with older versions of Matlab

If it is necessary to run on an older version of Matlab prior to 2018a, some minor modifications to the helicsMex.cpp may be possible to allow things to work fine, please contact the developers for specific instructions.

Versions prior to Matlab 2015 will require more extensive modifications.  And may be better served by using the Swig based matlab build from Helics version 3.2 which should be compatible with other HELICS versions in the 3.X series.

## Source Repo

The simHelics source code is hosted on GitHub: [https://github.com/GMLC-TDC/simHelics](https://github.com/GMLC-TDC/simHELICS) \[This may not be accurate\]

## Citation

General citation for HELICS:
T. Hardy, B. Palmintier, P. Top, D. Krishnamurthy and J. Fuller, "HELICS: A Co-Simulation Framework for Scalable Multi-Domain Modeling and Analysis," in IEEE Access, doi: 10.1109/ACCESS.2024.3363615, available at [https://ieeexplore.ieee.org/document/10424422](https://ieeexplore.ieee.org/document/10424422/

## Release

HELICS, simHELICS and many associated repositories are distributed under the terms of the BSD-3 clause license. All new
contributions must be made under this license. [LICENSE](LICENSE)