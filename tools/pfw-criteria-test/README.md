
**INTEL CONFIDENTIAL
Copyright  2014 Intel
Corporation All Rights Reserved.**

**The source code contained or described herein and all documents related to
the source code ("Material") are owned by Intel Corporation or its suppliers
or licensors. Title to the Material remains with Intel Corporation or its
suppliers and licensors. The Material contains trade secrets and proprietary
and confidential information of Intel or its suppliers and licensors. The
Material is protected by worldwide copyright and trade secret laws and
treaty provisions. No part of the Material may be used, copied, reproduced,
modified, published, uploaded, posted, transmitted, distributed, or
disclosed in any way without Intels prior express written permission.**

**No license under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or delivery
of the Materials, either expressly, by implication, inducement, estoppel or
otherwise. Any license under such intellectual property rights must be
express and approved by Intel in writing.**

# Pfw Criteria Tests Project Readme

## Project
This project allows user to Test pfw configuration. For now,
*audio* test vectors and configuration are available in the
`vendor/intel/hardware/PRIVATE/audiocomms/tests` project,
in the `test_suites/audio-hal/functional_tests/pfw-criteria-test`
directory.
As they can be platform related, they can be moved in device
directory. See dedicated README file for more information.

The project should be in the parameter-framework core repo
once mature.

## How to

You can run tests using pfwCriteriaTest.py file.

You have to run the script with at least the test directory argument:

* test\_directory: path to the test directory

Different options are available:

* -s, --scenario    : precise the scenario number to launch
* -i, --interactive : run in interactive mode to test new vectors
* -v, --verbose     : make the script talk on stdin
* -c, --coverage    : generate the html coverage report at the end of the script
* -h, --help        : show options recap

To see available scenarios in the test directory, you can run:

``` ./pfwCriteriaTest.py path/to/tests/ ```

The script will wait for a choice and the desired scenario will be
launched. You can also precise a scenario with -s option.

