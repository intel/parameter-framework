Copyright (c) 2014-2015, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Client simulator

## Project

This project allows users to test Parameter Framework configurations. It
allows writing scenario, mixing criterion changes and arbitrary script
execution. The goal of such tests are to recreate, under a test harness, the
real use-cases that will be executed by a Parameter Framework.

In short: it eases the development of automated Parameter Framework
configuration functional tests.

## How to run tests

You can run tests using pfClientSimulator.py. `test-platform` and
`remote-process` need to be in the PATH (e.g. by installing the Parameter
Framework - see the main README file).

You have to run the script with at least the test directory argument:

* `test_directory`: path to the test directory

Different options are available:

* `-s`, `--scenario`    : precise the scenario number to launch
* `-i`, `--interactive` : run in interactive mode to test new vectors
* `-v`, `--verbose`     : make the script talk on stdin
* `-c`, `--coverage`    : generate the html coverage report at the end of the script
* `-h`, `--help`        : show options recap

To see available scenarios in the test directory, you can run:

    ./pfClientSimulator.py path/to/tests/

The script will wait for a choice and the desired scenario will be
launched. You can also directly select a scenario with -`s` option.

Regarding coverage, you might want to read
[this README](https://github.com/01org/parameter-framework/blob/master/tools/coverage/README.md)
to learn what it is about.

## How to create a test environment

### Test Directory

A test directory should contains a `conf.json` file containing:

- The desired command prefix (e.g. `adb shell` in order to execute tests on an
  android board or empty to execute locally).
- The port on which the test-platform should be started.
- The criterion file path (see
  [this README](https://github.com/01org/parameter-framework/tree/master/tools/xmlGenerator#domaingeneratorpy)).
- The path to the Parameter Framework toplevel configuration file.
- The path to the directory containing the scenario files.
- The path to the scripts definitions file (optional) (see below).
- The path to the actions definitions (aka "ActionGatherer") file (optional)
  (see below).
- The path to the log output file (optional but needed for coverage).
- The path to the directory containing the coverage generation tool
  (optional; for coverage only).
- The path to the html coverage output file (optional; for coverage only).
- The path to the Parameter Framework domain configuration file (optional; for
  coverage only).

Relative paths in `conf.json` will be evaluated *relative to the test
directory*.

## Example Client Simulator configuration file

```{.json}
{
    "PrefixCommand" : "adb shell",
    "TestPlatformPort" : "5001",

    "CriterionFile" : "MyCriteria.txt",
    "PfwConfFile" : "/home/user/tests/TopLevel.xml",

    "ScenariosDirectory" : "test-vectors",
    "ScriptsFile" : "my-test-scripts.json",
    "ActionGathererFile" : "my-test-actions.json",

    "LogFile" : "tests.log",

    "CoverageDir" : "/home/user/parameter-framework/tools/coverage",
    "CoverageFile" : "coverage.html",
    "PfwDomainConfFile" : "MyConfigurableDomains.xml"
}
```

### Scenario

All scenario files need to be put in the directory mentioned by the
`ScenariosDirectory` configuration item.

A scenario file contains all the actions you want to take. *Note*: it is an
ordered list. There are two possible kind of actions: `setCriterion` and
`script`.
For example:

```{.json}
[
    {"setCriterion" :
        {
            "Mood" : "mad"
        }
    },
    {"script" : "myFirstScript"},
    {"setCriterion" :
        {
            "Mood" : "glad",
            "SmokeGenerator" : "On",
            "Colors" : "red blue"
        }
    },
    {"script" : "mySecondScript"}
]
```

This scenario file sets a criterion, then runs a script, then sets three
criteria and finally runs another script.

The `setCriterion` type allows user to offer a dictionary describing changing
criteria (configurations are automatically applied after using this action
type). Other criteria keep there old values.

The other type, `script` allows users to launch a script when he wants.

### Action definitions (aka ActionGatherer)

You can also define your own types based on the system ones, defined ones. You
have to edit the `ActionGathererFile` specified in the `conf.json` file to do
that. This file has this pattern :

```{.json}
{
    "getMad":
        {"setCriterion" :
            {
                "Mood" : "mad"
            }
        },
    "party" :
        {"setCriterion" :
            {
                "SmokeGenerator" : "On",
                "Colors":"red blue"
            }
        }
}
```

Here we define five new types based on `setCriterion`. When writing a scenario,
we can now use those types as basis and add some criteria to set in the same
time.

**For now, defining `script` actions are not supported**; only `setCriterion`
action definitions are supported.

Here is the example scenario, rewritten using the above definitions:

```{.json}
[
    {"getMad" : {}},
    {"script" : "myFirstScript"},
    {"party":
        {
            "Mood":"glad",
        }
    },
    {"script" : "mySecondScript"},
]
```

During the `party` step, the `SmokeGenerator` and `Colors` criteria are set as
defined in the `ActionGathererFile` but also the `Mood` criteria.

### Scripts

Scripts are defined in the `ScriptsFile` specified in the `conf.json` file *or*
they can be an inline shell script.

This `ScriptsFile` file should look like this :

```{.json}
{
    "myFirstScript" : ["test-scripts/first.sh","asynchronous"]
    "mySecondScript" : ["test-scripts/second.sh","synchronous"],
}
```

This dictionary is composed as such:

```{.json}
name : ["path/to/script",synchronousness]
```

The path to the script is relative *to the path of the `ScriptsFile`*.

The synchronousness can be either `"synchronous"` or `"asynchronous"` and
determines whether the Client Simulator waits for the script to return before
executing the next scenario step. To be clear:

* asynchronous : The script will run concurrently to the execution of the
  Client Simulator;
* synchronous : The Client Simulator will wait the end of the script before
  resuming execution and thus, the rest of the scenario won't be executed until
  the script returns.
