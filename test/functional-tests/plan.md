# Parameter Framework (PF) test plan

## Notes
- The unit test code MAY be able to list implemented scenarios
  in this same BDD format in order to check scenario coverage.
- All scenarios implicitly start by _"GIVEN a parameter framework"_.
- If a scenario does not specify a config files,
  the following minimal files SHOULD be used:

Top level file (replace `{structurePath}` by its value):
~~~ xml
<?xml version='1.0' encoding='UTF-8'?>
<ParameterFrameworkConfiguration SystemClassName='test'
                                 TuningAllowed='false' ServerPort='1'>
    <SubsystemPlugins/>
    <StructureDescriptionFileLocation Path='{structurePath}'/>
</ParameterFrameworkConfiguration>
~~~

Structure file:
~~~ xml
<?xml version='1.0' encoding='UTF-8'?>
<SystemClass Name='test'>
    <Subsystem Name='test' Type='Virtual'>
        <ComponentLibrary/>
        <InstanceDefinition>
            <BooleanParameter Name="placeholder"/>
        </InstanceDefinition>
    </Subsystem>
</SystemClass>
~~~

## Starting the Parameter Framework

- set a logger
    - [X] Scenario: Default logger
        * WHEN no logger is set
        * THEN start should succeed

    - [X] Scenario: No logger
        * WHEN a nullptr logger is set
        * THEN start should succeed

    - [X] Scenario: Logger should receive info and warnings
        * GIVEN config files that emit warnings
        * GIVEN a logger that store logs
        * WHEN the record logger is set
        * THEN start should succeed
        * AND_THEN the logger should have stored info and warning log

    - [X] Scenario: Unset logger
        * GIVEN config files that emit warnings
        * GIVEN a logger that store logs
        * WHEN the record logger is set
        * AND_WHEN a nullptr logger is set
        * THEN start should succeed
        * AND_THEN the record logger should NOT have stored any info or warning log

- set/get various properties before startup:
    - "force no remote interface"
        - [ ] Scenario: Tuning OK
            * GIVEN config files with tuning enabled on a valid port
            * WHEN the remote interface is NOT forbiden
            * THEN start should succeed

        - [ ] Scenario: Invalid tunning
            * GIVEN config files with tuning enabled on an invalid port (0?)
            * WHEN the remote interface is NOT forbiden
            * THEN start should fail

        - [ ] Scenario: Forbiden invalid tunning
            * GIVEN config files with tuning enabled on an invalid port (0?)
            * WHEN the remote interface is forbiden
            * THEN start should succeed

    - "failure on missing subsystem"
        - [ ] Scenario: Plugin OK
            * GIVEN config files with a valid plugin
            * WHEN the missing subsystem policy is set to failure
            * THEN start should succeed

        - [X] Scenario: Wrong plugin
            * GIVEN config files with a invalid plugin
            * WHEN the missing subsystem policy is set to failure
            * THEN start should fail

        - [ ] Scenario: Wrong plugin but fallback
            * GIVEN config files with a invalid plugin
            * WHEN the missing subsystem policy is set to success
            * THEN start should succeed

    - "failure on failed settings load"
        - [X] Scenario: Settings OK
            * GIVEN config files with a valid settings file
            * WHEN settings load failure is requested to abort start
            * THEN start should succeed

        - [X] Scenario: Wrong settings
            * GIVEN config files with a invalid (non existant?) settings file
            * WHEN settings load failure is requested to abort start
            * THEN start should fail

        - [X] Scenario: Wrong settings but ignore
            * GIVEN config files with a invalid (non existant?) settings file
            * WHEN settings load failure is requested to abort start
            * THEN start should succeed

    - "schema folder location" and "validate schema on start"
        - [ ] Scenario: Schema OK
            * GIVEN config files with correct default schema location
            * WHEN schema folder location is left to default
            * WHEN schema validation is enabled
            * THEN start should succeed

        - [ ] Scenario: Inexisting schemas
            * GIVEN config files with correct default schema location
            * WHEN schema folder location is set to an invalid location (/doesNotExist ?)
            * WHEN schema validation is enabled
            * THEN start should fail

        - [ ] Scenario: Inexisting ignored schemas
            * GIVEN config files with correct default schema location
            * WHEN schema folder location is set to an invalid location (/doesNotExist ?)
            * WHEN schema validation is disabled
            * THEN start should succeed

        - [ ] Scenario: Inexisting used schemas
            * GIVEN config files with custom schema location (./shemaCustomFolderName)
            * WHEN schema folder location is set to the custom shema location
            * WHEN schema validation is enabled
            * THEN start should succeed

        - [ ] Scenario: Schema OK, config KO
            * GIVEN incorect config files with correct default schema location
            * WHEN schema validation is enabled
            * THEN start should fail

### Error cases/special cases

- invalid configuration or a file can't be read:
    - [X] Scenario: invalid top level config file
        * GIVEN an incorect (empty ? wrong balisa ?) config top level file.
        * THEN start should fail

    - [X] Scenario: invalid structure
        * GIVEN an incorect (empty ? wrong balisa ?) structure file.
        * THEN start should fail

    - [X] Scenario: invalid settings
        * GIVEN an incorect (empty ? wrong balisa ?) settings file.
        * THEN start should fail

- plugins can't be found (unless silenced by the user)
    - Done in "failure on missing subsystem"

- usage of `<xi:include .../>`
    - [ ] Scenario: Arbitrary xincluded node
        * GIVEN a top level file xincluding a file with the "SettingsConfiguration" balisa
        * THEN start should succeed

- plugins:
    - non-empty "Location" attribute
        - [ ] Scenario: Plugin location OK
            * GIVEN a top level file with a plugin folder location attribute
            * THEN start should succeed
    - empty "Location" attribute
        - relative path
            - [ ] Scenario: Empty Location, standard plugin install
                * GIVEN config files with an empty location attribute
                * GIVEN a plugin installed in a dlopen standard path
                * GIVEN that "Location/Plugin[name]" is the plugin name
                * THEN start should succeed

            - [ ] Scenario: Empty Location, non standard plugin install
                * GIVEN config files with an empty location attribute
                * GIVEN a plugin NOT installed in a dlopen standard path
                * GIVEN that "Location/Plugin[name]" is the plugin name
                * THEN start should fail

        - absolute path
            - [ ] Scenario: Empty Location, plugin absolute path
                * GIVEN config files with an empty location attribute
                * GIVEN a plugin
                * GIVEN that "Location/Plugin[name]" is the plugin absolute path
                * THEN start should succeed

            - [ ] Scenario: Empty Location, plugin non existing path
                * GIVEN config files with an empty location attribute
                * GIVEN that "Location/Plugin[name]" is an non existing path (/libDoesNotExist.so?)
                * THEN start should fail

* * *

## Logging

- info and warning
    - Partly covered by the "set a logger" section
    - [ ] Scenario: Plugin info and warning log
        * GIVEN a plugin that log a unique string as info and warning
        * GIVEN a logger that store logs
        * WHEN the record logger is set
        * THEN start should succeed
        * AND_THEN the logger should have stored the plugin specific info and warning log

* * *

## Basic Domains & Configurations management

- [ ] Scenario: create and delete an empty domain
    * GIVEN a started parameter framework without domains
    * THEN creating a domain with an arbitrary name ("domain1"?) should succeed
    * (AND_THEN the domain list should contain the domain name)
    * THEN deleting the domain should succeed

- [ ] Scenario: create and delete a configuration
    * GIVEN a started parameter framework with a domain
    * THEN creating a configuration in this domain with an arbitrary name ("config1") should succeed
    * (AND_THEN the domain configuration list should contain the configuration name)
    * THEN deleting the configuration should succeed
    * (AND_THEN the domain configuration list should not contain the configuration name)

- [ ] Scenario: delete a domain with configurations
    * GIVEN a started parameter framework with a domain and several configuration (3?)
    * THEN deleting the domain should succeed
    * (AND_THEN the domain list should not contain the domain name)

- [ ] Scenario: delete all domains
    * GIVEN a started parameter framework with several domains
    * THEN deleting all domains should succeed
    * (AND_THEN the domain list should be empty)

- [ ] Scenario: rename a domain
    * GIVEN a started parameter framework with a domain and several configuration (3?)
    * THEN renaming the domain to an arbitrary name should succeed


### Error cases/special cases

- [ ] Scenario: create a domain with an already-existing name
    * GIVEN a started parameter framework with a domain (arbitrary name)
    * THEN creating a second domain with the same name than the first should fail

- [ ] Scenario: create a configuration with an already-existing name
    * GIVEN a started parameter framework with a domain
            containing one configuration (arbitrary name)
    * THEN creating a second configuration in this domain
           with the same name than the fist should fail

- [ ] delete a non-existent domain
    * GIVEN a started parameter framework without domain
    * THEN deleting an arbitrary named domain should fail

- [ ] delete a non-existent configuration
    * GIVEN a started parameter framework one domain
    * THEN deleting in this domain a arbitrary named configuration should fail

- [ ] list configuration of a unknown domain
    * GIVEN a started parameter framework without domain
    * THEN listing configuration of an arbitrary named domain should fail

- [ ] Scenario: rename a domain to an already used name
    * GIVEN a started parameter framework with two domains
            with arbitrary different names ("domain1" and "domain2")
    * THEN renaming first domain ("domain1") to the name of the second one ("domain2") should fail

- [ ] Scenario: rename a configuration to an already used name
    * GIVEN a started parameter framework with one domain containing two configuration
            with arbitrary different names ("config1" and "config2")
    * THEN renaming first configuration ("config1")
           to the name of the second one ("config2") should fail

- [ ] Scenario: rename a domain to its own name
    * GIVEN a started parameter framework one arbitrary named domain
    * THEN renaming the domain to its own name should succeed

- [ ] rename a configuration to its own name
    * GIVEN a started parameter framework a domain and one configuration
    * THEN renaming the configuration to its own name should succeed

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

