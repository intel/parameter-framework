# parameter-framework

[![Build Status](https://travis-ci.org/01org/parameter-framework.svg?branch=master)](https://travis-ci.org/01org/parameter-framework)
[![Coverage Status](https://coveralls.io/repos/01org/parameter-framework/badge.svg?branch=master)](https://coveralls.io/r/01org/parameter-framework)

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

Run `cmake .` then `make`.  You may then install libraries, headers and
binaries with `make install`.  By default, they are installed under
`/usr/local` on unix OSes; if you want to install them under a custom
directory, you may do so by passing it to the `cmake .` command; e.g.

    cmake -DCMAKE_INSTALL_PREFIX=/path/to/custom/install .

Also, CMake can build a project out-of-tree, which is the recommended method:

    mkdir /path/to/build/directory
    cd /path/to/build/directory
    cmake /path/to/parameter-framework/sources
    make

After an install you may want to run the parameter-framework tests with
`make test`.
