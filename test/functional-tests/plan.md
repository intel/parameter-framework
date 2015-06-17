# Parameter Framework (PF) test plan

## Starting the Parameter Framework

- set a logger
- set/get various properties before startup:
    - "force no remote interface"
    - "failure on missing subsystem"
    - "failure on failed settings load"
    - "schema folder location"
    - "validate schema on start"
- criteria: see below
- start the Parameter Framework

### Error cases/special cases

- invalid configuration or a file can't be read:
    - toplevel
    - structure
    - settings (unless silenced by the user)
- plugins can't be found (unless silenced by the user)
- usage of `<xi:include .../>`
- plugins:
    - non-empty "Location" attribute
    - empty "Location" attribute
        - relative path
        - absolute path

* * *

## Logging

- info
- warning

* * *

## Basic Domains & Configurations management

- create a domain; create a configuration
- delete a configuration; delete a domain; delete all domains
- list domains; list configurations for a domain
- rename a domain; rename a configuration

### Error cases/special cases

- create a domain (or a configuration) with an already-existing name
- delete a non-existent domain (or configuration)
- list configuration of a unknown domain
- rename a domain (or a configuration) to an already existing name
- rename a domain (or a configuration) to its own name: should be a no-op and succeed

* * *

## Domain modification

- set/get sequence awareness
- add/list/remove elements to a domain
- *(split domain ?)*
- *(listBelongingDomains ?)*
- *(listAssociatedDomains ?)*
- *(listAssociatedElements ?)*
- *(listConflictingElements ?)*
- *(listRogueElements ?)*

### Error cases/special cases

- add (or remove) a non-existent element
- add (or remove) a parameter to (or from) a non-existent domain

* * *

## Configurations modification

- set a configuration rule; get it back
    - trivial rules, complex rules (fuzzing ?)
    - clear a rule
- save/restore a configuration
- set/get element sequence
- set/get a parameter for a given configuration
- set a parameter belonging to a domain and save a configuration

### Error cases

- set a wrongly-formatted rule
- set a rule using an unknown criterion (or unknown criterion value)
- set/get a rule to a non-existent domain (or configuration)

* * *

## Criteria

- create a criterion
    - exclusive
    - inclusive
- *(list criteria)*
- set a criterion; get a criterion value
    - exclusive criterion
    - inclusive criterion
        - special case: "none" value

### Error cases/special cases

- duplicated criterion name (with the same type/with a different type)
- duplicated criterion value
- max supported number of criterion values

* * *

## Settings Import/Export

- import xml domains
    - w/ or w/o settings
    - from a file/from a string
    - all/single
- export xml domains
    - w/ or w/o settings
    - to a file/from a string
    - all/single
- *(import binary settings ?)*
- *(export binary settings ?)*

### Error case/special cases

- invalid settings
    - invalid xml (should we use a fuzzing framework ?)
    - valid xml but not matching the XSD (ditto)
    - valid but unusable data:
        - unknown criterion/criterion values
        - unknown parameters
- unreadable/unwritable file
- single domain import overwrite
    - fail if overwrite not requested
    - succeed if overwrite requested

* * *

## Parameters

- structures are correctly instantiated
    - parameters in the "instance definition"
    - component types instantiation
    - parameter blocks
    - bit blocks
    - arrays
- check parameter mappings are correctly applied
    - at all levels possible (subsystem, component type, component instance,
            block, parameter...)
    - context mappings, amends, instantiation mappings
- set/get a parameter
- parameter handles
- test on parameter values
    - booleans
    - bit blocks
    - numeric types ranges
    - fixed-point tests
    - string length

* * *

## Configuration application

- apply configurations
    - sequence aware domains
- forced sync

* * *

## Misc/to be classified

- tuning mode
- auto sync
- remote interface instantiation
- value space (raw/real)/format (dec/hex)

### Error cases

