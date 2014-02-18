# parameter-framework

tl;dr: compilation instructions are at the end

## Introduction

WIP

## Compiling

Run `cmake .` then `make`.  You may then install libraries, headers and
binaries with `make install`.  By default, they are installed under
`/usr/local` on unix OSes; if you want to install them under a custom
directory, you may do so by passing it to the `cmake .` command; e.g.
`cmake -DCMAKE_INSTALL_PREFIX=/path/to/custom/install .`.
