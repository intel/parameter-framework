#Functional test

#What ?
Create a test suite for all tests about SET/GET commands.
Types,functions and Domains are tested.

#How ?
We set environment variables in the cmake.
All temporary file are stored in the build directory.
XML files depends on cmake variables so they are configured at build time and stored in {build_dir}/tmp.

We launch the functional tests with ACTCampaignEngine.py. This script launch every test present in the PfwTestCase directory. Note that functional tests cannot be launched using directly the script.
We finalize the environment setting in this script. isAlive and needResync are needed by the subsystem.
To avoid dependancies between to consecutive test, we remove all the temporary files except XML files at the end of the tests.

#Practical
By default, the BUILD_TESTING flag is set to true.
Once the makefile is created, we can launch the test by running :

    'make && make test'

Note that you can also use 'ctest -V' if you want to have the logs details.
