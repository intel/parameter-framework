# parameter-framework

[![Build Status](https://travis-ci.org/01org/parameter-framework.svg?branch=master)](https://travis-ci.org/01org/parameter-framework)
[![Windows Build Status](https://ci.appveyor.com/api/projects/status/ga24jp8tet0qimbu/branch/master)](https://ci.appveyor.com/project/parameter-framework/parameter-framework)
[![Coverage Status](https://codecov.io/github/01org/parameter-framework/coverage.svg?branch=master)](https://codecov.io/github/01org/parameter-framework?branch=master)

## Introduction

The parameter-framework is a plugin-based and rule-based framework for handling
parameters.  This means that you can:

1. Describe your system's structure and its parameters (in XML) - aka. **What**;
2. Write (in C++) or reuse a backend (aka. plugin) for accessing the parameters
that you just described - aka. **How**;
3. Define (in XML or in a domain-specific-language) conditions/rules upon which
a given parameter must take a given value - aka. **When**.

![What, How, When](https://01org.github.io/parameter-framework/hosting/what-how-when.png)

### Usage examples

#### Alsa controls on embedded platforms

The parameter-framework can be used to set the value of alsa controls
(switches, volumes, etc.) on smartphones/tablets based on parameter-framework
rules (in this example, they transcribe use-cases).  For accessing parameters
(i.e. alsa controls), you may use the
[alsa plugin](https://github.com/01org/parameter-framework-plugins-alsa).

#### Parameters in files

The [filesystem plugin](https://github.com/01org/parameter-framework-plugins-filesystem)
can be used to write parameters in files.  This is particularly useful for
files in `/sys` managing GPIOs.

### More details

The parameter-framework's core comes in the form of a shared library.  Its
client has to provide:

- configuration files describing the structure of the system to be managed by
  the parameter-framework and what plugins it must use to read/write into each
  subsystem;
- a list of criteria (representing the state of the client) and their possible
  values;
- configuration files describing the value that each part of the system (aka
  parameter) must take - this is done by writing rules based on the criteria
  described above.

At runtime, the most usual communication between the client and the
parameter-framework are:

1. The update of *criteria* (that are used in the rules introduced above) and
2. Update all relevant parameters according to the new criteria values.  The
   parameter-framework uses the appropriate backend for writing the values in
   each underlying subsystem.

The parameter-framework comes with several tools, including a command-line
interface: `remote-process`.

## Going further

See [the wiki on github](https://github.com/01org/parameter-framework/wiki).

## Compiling

**You may take a look at `.travis.yml` and `appveyor.yml` for examples on how we
build the Parameter Framework in the CI.** It will probably help if you have
troubles building the Parameter Framework even after reading the following
sections:

### Dependencies

In order to compile you'll need, at the very least:

- CMake (v3.2.2 or later) (v3.3.0 or later on Windows);
- A C/C++ compiler supporting C++11;
- libxml2 headers and libraries (Provided by the `libxml2-dev` on debian-based
distributions);

If you want to use the remote command interface (`NETWORKING=ON` by default),
you'll also need:

- Standalone ASIO (1.10.6 or later) (Provided by `libasio-dev` on debian-based
distributions) ASIO is C++ header-only ASynchronous-IO library.

If you want to compile the *Python bindings* (`PYTHON_BINDINGS=ON` by default),
you'll also need:

- SWIG 2.0 (A binding generator);
- Python2.7 development environment (Provided by `python2.7-dev` on debian-based
distributions)

If you want to *compile and run the tests* (`BUILD_TESTING=ON` by default),
you'll also need:

- Catch (Provided by `catch` on debian-based distributions). Catch is a
single-header test framework - as such you may also download it directly
[here](https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp);
- Python2.7 (Provided by `python2.7` on debian-based distribution - it is
preinstalled on most distributions).

If you want to *build the code documentation* (`DOXYGEN=OFF` by default), you'll
need `doxygen` and `graphviz`. This doc is already available to you - see the
wiki.

**To list all available configuration options, try** `cmake -L` (you may also
filter-out lines starting with `CMAKE_`).

### How-To

If you are already familiar with CMake, you know what to do.

Run `cmake .` then `make`.  You may then install libraries, headers and
binaries with `make install`.  By default, they are installed under
`/usr/local` on unix OSes; if you want to install them under a custom
directory, you may do so by passing it to the `cmake .` command; e.g.

    # Always use absolute paths in CMake "-D" options: you don't know where
    # relative paths will be evaluated from.
    cmake -DCMAKE_INSTALL_PREFIX=/path/to/custom/install .

If you want to provide your own dependencies (e.g. your own version of
libxml2), you should pass the base paths as the `CMAKE_PREFIX_PATH` variable,
e.g.:

    cmake -DCMAKE_PREFIX_PATH='/path/to/dependency1/;/path/to/dependency2/'

For more information on how to use `CMAKE_PREFIX_PATH`, see CMake's
documentation.

Also, CMake can build a project out-of-tree, which is the recommended method:

    mkdir /path/to/build/directory
    cd /path/to/build/directory
    cmake /path/to/sources/of/parameter-framework
    make

After a build you may want to run the parameter-framework tests with
`make test` or `ctest`.

### Compiling on Windows

The only supported compiler on Windows in Visual Studio 14 2015. The 2013
version does not support some C++11 features.  When running CMake's
configuration step (the first call to CMake) you must specify the build system
you want to use, i.e. `-G Visual Studio 14 2015 Win64`. Again, you may refer to
`appveyor.yml`.

If you don't already have libxml2 headers/libraries and don't want to build them
by yourself, we have a precompiled version for x86-64. *These are provided for
reference and as a convenience for development purpose only; when making a
final product, you should recompile the latest libxml2 release yourself.*

Compiled with Visual Studio 12 2013:
- [in debug configuration](https://01.org/sites/default/files/libxml2-x86_64-debug-3eaedba1b64180668fdab7ad2eba549586017bf3.zip)
- [in release configuration](https://01.org/sites/default/files/libxml2-x86_64-3eaedba1b64180668fdab7ad2eba549586017bf3.zip)

We have mirrored ASIO 1.10.6 [here](https://01.org/sites/default/files/asio-1.10.6.tar.gz).

Once you have downloaded and uncompressed these two dependencies, add the
following two entries to `CMAKE_PREFIX_PATH`:

    /path/to/libxml2-x86_64/
    /path/to/asio-1.10.6/
