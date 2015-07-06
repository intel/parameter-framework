High level requirements
=======================

(Note: some requirements are only motivated by the fact that the reference
       implementation implements them. Search for "reference implementation".)

# Introduction
The PF MUST be a library.
(Why: To be reused in different components.)

PF instances MUST not share any mutable data.
(Why: to guarantee that different PF instances will not impact each others.)

# Parameters

A PF MUST be able to handle parameters.
(Why: because the PF aims to abstract hardware and model it by parameters.)

## Definitions

Hardware: system controlled by the PF. Not necessary material system. This
term was chosen because:
 - historically the PF reference implementation was used to abstract hardware
 - the subsystem term would arguably fit best is already used.
(FIXME: choose subsystem instead, it is not used in fact.)

## Value

A parameter MUST have a value.
(Why: because a parameter without value would not abstract any hardware.)

A PF MUST support mutable parameters.
(Why: To control the underlined hardware.)

A PF MAY support immutable parameters, i.e. parameters which value is
determined on start then read only.
(Why: To permit hardware read only value reflection.)
(Note: This is not implemented in the PF reference implementation.)

This value MUST be gettable and settable (except if immutable).
(Why: A parameter that can not be accessed is of no use.)

A value MUST be convertible to a string.
(Why: To display a parameter value to the human user for debugging purpose.)

A bijective conversion string <-> value SHOULD exist.
(Why: To support serialization and deserialization of the parameter values.
      It is not a MUST because a PF implementation could offer serialization to
      custom -not string based- format.)

### Data type

#### Definition
All parameters have a data type.
A data type designates parameter invariants.
A data type is the meaning of the data and the way values of that type can be
stored.


#### Philosophy

A data type defines the value properties:
 - memory layout
 - value constrains

A value type is mostly used to:
 - pretty display parameter values (not just a as an array of bits)
 - check for user error when setting it (out of bound, invalid...)
 - offer a type safe API

#### Requirements

A PF SHOULD support the following types.
If a type is chosen to be supported, it MUST respect all MUST clause,
SHOULD respect all SHOULD clause, MAY respect all MAY clause of the type.
(Why: All type are not necessary to use the PF. For example any parameter could
      be represented as an array of char (string). But this would not permit to
      check parameter validity (invariants) nor a pretty display of the values.)

Implementation MAY add another API to access a parameter value.
(Why: For example a C++ implementation may give access to a string as an
      std::string object.)

##### Integers
PF SHOULD support signed and unsigned integer parameters
(Why: The reference implementation supports it.)

PF MUST support integer with invariant size.
(Why: It is common in C API to expect numbers to have a fixed maximum size.)

The API to access it MUST respect C integer ABI.
(Why: For easy access from C code.)

Supported integer size SHOULD be at least 8, 16 and 32 bits.
(Why: The reference implementation supports it.)

PF MAY support constraining the parameter minimum and maximum value.
(Why: To catch user out of valid range errors when changing the parameter
      value.)

##### String
PF SHOULD support array of characters.
(Why: Everything that a computer can store fits in an array of characters.
      It can be used as a fallback type if no other matches the parameter.)

The array maximum size MAY be invariant (immutable).
(Note: This is what the reference implementation does.)

The API to access the string value SHOULD support null terminated character
array. As it is commonly done in C.
(Why: For easy access from C code.)

##### Fix point parameter
PF SHOULD support fix point parameters. I.e. integers divided by a fixed power
of two.
(Why: The reference implementation supports it.)

The API to access the values SHOULD respect the Qm.n and UQm.n standards.
(Why: It is the main standard for fix point parameters.)

PF SHOULD support at least `0 <= m + n <= 31` for a Signed Qm.n and
`0 <= m + n <= 32` for an Unsigned Qm.n (or "UQm.n").
(Why: The reference implementation supports it.)
(Note: The reference implementation only supports Signed Qn.m)

PF MAY support constraining the parameter minimum and maximum value.
(Why: To catch user out of valid range errors when changing the parameter
      value.)
(Note: The reference implementation does not support it)

##### Floating point
PF SHOULD support floating point parameters .
(Why: The reference implementation supports it.)

The API to access the values SHOULD respect the IEEE 754 standard.

PF SHOULD support at least 32 and 64 bit size floats.
(Why: The reference implementation supports it.)
(Note: The reference implementation only supports 32bits)

PF MAY support constraining the parameter minimum and maximum value.
(Why: To catch user out of valid range errors when changing the parameter
      value.)

##### Bit field following the C ABI
PF SHOULD support 1 or more bit sized integers.
(Why: The reference implementation supports it.)

Such parameters called are regrouped in a so called bit parameter block.
The API to access a bit parameter block SHOULD give access to a packed bit
field.
(Why: The reference implementation supports it.)

### Parameter adaptation

#### Philosophy
Parameters exposed by hardware sometimes need to transform a parameter value.
For example an hardware integer parameter could have a range 64-128 but it might
be necessary for upper layer to access in a range 0-100.

This transformation (called parameter adaptation in the rest of the document)
could be done by the syncer. Nevertheless syncers are supposed to contain only
business logic and should not be impacted by upper layer needs.

#### Definition
Parameter adaptation is a bijective pure function converting a parameter value
between the syncer and other parameter reader/writer (including the inference
engine).

(Why: It must be bijective in order to a) scale the user value to the hardware
      value and b) convert the hardware value to the user's value space.)

#### Requirements
The following parameter adaptation SHOULD be supported

 - Affine adaptation: `affAd(value) = slope * value + offset` where slope and
   offset and user-defined constants
(Why: The reference implementation supports it.)

 - Logarithm adaptation: `logAd(base, value) = ln(value) / ln(base)` where
   `ln` is the natural logarithm and base is a user-defined constant.
(Why: The reference application supports it.)
(Note: The reference implementation also supports passing a floor value to
       be applied after conversion.)

A PF MAY offer Parameter adaptation composition. I.e. combine multiple parameter
adaptation
E.g.: composing the affine and logarithm adaptation to
`compAd(value) = slope * logAd(base, value) + offset`.
(Why: To avoid combination explosion of parameter adaptations.
      The idea is to builtin basic function and let the user compose them to
      meet its need.)
(Note: The reference application supports in a tricky way: the logarithm
 adaptation is always combined with the affine adaptation)

## Identifiers
Every parameter MUST have an identifier that uniquely identifies it.
(Why: to identify a parameter outside the framework)

This identifier SHOULD be a string.
(Why: so that the (human) user can identify a parameter with ease.)

Two PF instances with the same parameters MUST have the same identifier for
those parameters.
I.e. this identifier should be the same across all instances with the same
configuration.
(Why: Persistence of parameter identifier across PF instances with the same
      configuration. To identify parameters independently of the host machine
      and PF instance)

### Parameter tree
A parameter MUST be structured in a tree. Each parameter being a distinct tree
leaf.
(Why: Tree is a simple data structure that can be easily represented and is
      enough to map underlined layers.)

Each node of the tree SHOULD have its own identifier with the same
characteristics (type, independence...) than a parameter.
(Why: To represent the tree without treating the leaf nodes specifically.)

The identifier of each node of the tree SHOULD be a combination of its parents.
More specifically, if the identifier is a string it should be formated the same
way as a file system path. E.g. the parameter named `/root/child1/4/parameter1`.
(Why: Usual syntax to address trees.)


# Syncer

The PF philosophy is to map the hardware characteristics to parameters.

A syncer MUST be mapped to one or more parameters.
(Why: The hardware minimal access may be bigger than one parameter.)

One parameter MUST NOT be mapped to two or more syncer.
(Why: Which syncer should be responsible to retrieve the initial parameter
      value if they are multiple per parameter?)

A syncer MUST support retrieving the mapped parameters value from the mapped
hardware.
(Why: to retrieve a parameter value at the start of the PF.)

A syncer MUST support setting the mapped parameters value to the mapped
hardware.
(Why: to synchronise hardware on parameter change.)

Syncers MUST retrieve and set the parameters value from the PF core.
(Why: to be able to synchronise with hardware.)

This API MAY be a packed parameter structure, following the C ABI without
padding.
(Note: This is what the reference implementation does.)
(Why: ???)

## Introspection
The syncer API SHOULD allow introspection of the mapped parameters.
(Why: the parameter structure may be useful for the syncer to
      communicate with the hardware.)

## Plugins
The PF MUST be able to create syncers.
(Why: to bind on the corresponding parameters.)

### Definition
The PF creates syncer using syncer builder.

### Identifier
All syncers mapping to the same hardware SHOULD have their builders regrouped in
a syncer library.
(Why: to be able to link a group of parameters and a given hardware.)

A syncer builder MUST have a unique identifier in its containing syncer library.
(Why: To uniquely identify the syncer that should bind on parameters.
      Given that the syncer library has already been specified.)

A syncer library MUST have a unique identifier in the host system.
(Why: To identify the library associated to parameters.)

Syncer build and syncer library identifiers SHOULD be strings.
(Why: The reference application does so.)

### Loading
Syncer library or/and builder MAY be loaded from dynamically linked libraries
(called syncer plugins).
(Why: The reference implementation supports it.)

Such syncer plugins SHOULD have an unique entry point that -when called- should
register its payload (syncer library/builder) in the provided gatherer.
(Note: This permit to merge multiple syncer libraries in one shared library.)
(Why: The reference implementation supports it.)

Multiple syncer plugins, may depend on each other. The PF should appropriately
handle the case and not fail.
(Why: The reference implementation supports it.)

## Mapping
### Definition
A parameter not bound to a syncer is called a "virtual parameter".
(Todo: remove if not used in the requirements.)

### Requirements
**TODO**:
 - Plugins
 - association builder <-> parameters

## Sync

Syncer SHOULD synchronise the mapped hardware on parameter change.
(Why: To always keep synchronise the underlined hardware and the PF parameters.)

Syncer SHOULD retrieve parameter value from the hardware if no value has be set
since the PF start.
(Note: This is usually implemented on PF start, initialize the parameter
       values with the mapped hardware current state.)
(Why: To allow introspection of the hardware.)

A mode with synchronisation on client request MAY be supported.
(Why: The reference implementation supports it.)

Syncers MAY report an 'out-of-sync' condition indicating that the hardware
parameter values are not (or no longer) reflecting the last values set by the
Parameter Framework.
(Why: This can happen when the underlying hardware subsystem
      crashes/reboots/...)

When a syncer reports an out-of-sync condition, the PF MUST try to resync the
hardware values.

# Rule based dynamic abstraction

## Philosophy

The PF offers parameters mapped on hardware. This is a good but weak
abstraction. There is often a 1/1 relation between a parameter and the hardware
it maps.

A PF offers a mechanism to abstract the parameters to more high level concept.

The goal is to hide numerous parameters and their dynamic value behind simple
and human friendly API.

It works by regrouping parameters with similar management and defining
configurations for each "scenario". These "scenario" are then given a priority
and a detection predicate. Configuration are applied when their associated
"scenario" is detected.

"Scenario" are detected through arbitrary criterion provided by the PF host
(see below).

## Definition

Configurations: set of values for different parameters. A configuration MUST not
contain 2 values of the same parameters.

For example, given a PF with 3 integer parameters A,B,C, a configuration can
contain
 - 1 value for (A), (B) or (C)
 - 2 value for (A,B), (A,C) or (B,C)
 - 3 value for (A,B,C)

## Configuration

A PF MUST offer configurations as described in the Definition chapter.
(Note: rule based parameter engine does not manipulate directly values,
      it applies configuration on the parameters.)
(Why: This is what the reference implementation does.)

Each configuration MUST be associated with a predicate that condition its
eligibility. A configuration with a predicate that evaluates to `true` is called
an "eligible configuration"
(Why: This is what the reference implementation does.)

It SHOULD be possible to express a predicate to always evaluates to `true`.
(Why: in order to have parameters set to constant values or have a fallback
      configuration in a domain - see below.)

The predicate SHOULD be a "selection criterion rule". See next chapter for a
definition.
(Why: The reference implementation uses a boolean expression based engine.)

## Selection criterion

A selection criterion MUST have one, and only one, state at a given time.

A selection criterion MUST know at construction all possible states.
(Why: To be able to validate:
       - rules on start
       - state changes)

The selection criterion possible states MUST be specifiable by:
 - Directly a state set (Input -> states == identity)
   (Note: called exclusive criterion)
   (Note: An empty set is not allowed as the criterion could not have a state.)
   (Why: any criterion can be crated with this one.)

The selection criterion possible states SHOULD be specifiable by:
 - A combination of values
   (Note: combination in the mathematical sense `"ab" -> ["", "a", "b", "ab"]`)
   (Note: called inclusive criterion)
   (Note: An empty value set is allowed as its combination -a set
          containing the empty set- would not be empty. The empty set would be
          the only possible criteria state.)
   (Why: The reference implementation supports it.)

The PF MUST not limit the number of criteria.

The PF MUST not limit the number of states of any given criterion
(Note: The reference implementation only supports 32 values for an inclusive
       criterion and 2^32 values for an exclusive criterion)

### Definitions
 - Selection criterion rule: function (in the mathematical sense) that MUST
   given selection criteria return a Boolean.

 - Rule: an Boolean expression of Selection criterion rules
   (Note: implementation only allows AND and OR combination)

### Criterion change

The API to change criterion values MUST allow atomicity regarding configuration
application. I.e. it MUST be possible to change multiple criterion values
without triggering a configuration application.
(Why: Two criterion might have an excluding state.
      If configuration application was triggered after each)

### Rules

It MUST always be able to express a selection criterion rule from a given
selection criterion
I.e.: a criteria MUST always have a state that can be matched by a rule.
(Why: if no rules can be formulated from a criterion, it is useless)

Parameter values change SHOULD be selected by Rules.
(Why: A rule based inference engine has been chosen based on implementation and
configuration ease)

It MUST be possible to express a Rule that is always True.
(Why: In order to make a configuration "always applicable"

## Domains

### Definition
Domain: ordered set of configuration, all of which contain the values for the
same parameters.

### Requirement

Each configuration SHOULD be in a "domain" (see Definition chapter).
(Why: Domains are mostly a way to define the priority of "Scenarios" for some
      parameters. It is not a MUST because this goal could also be achieve with
      (for example) global configurations and per parameter priority. It is not
      a MAY because the reference implementation uses domains.)

If multiple configuration are eligible, the first one MUST be applied.
(Why: If multiple configuration are eligible)

If no configuration is eligible, no configuration MUST be applied.
(Note: It means that if none of the configurations is eligible, none is applied.
       This also mean that no function can be defined between criteria and
       states.
       I.e.: parameter values MAY depend on previous selection criterion states.
(Why: This is what the reference implementation does.)

TODO sequence aware domains

# (de)serialization

## Philosophy
Serialization and deserialization are meant to support destruction recovery and
configuration deployment.

These are the same requirements than for a database, it needs to be able to save
its state and restore for backup, deployment, reboot...

## Definition
PF data includes:
- parameters tree
- configurations:
  - selection rule
  - parameter/value couples
- domain:
  - list of associated configurations
  - order of priority

## Requirement
The PF data MUST be deserializable.
(Why: Otherwise a PF instance could only be created empty and then be filled
      by the tuning interface.
      The reference implementation supports it.)

The PF data SHOULD be deserializable from a config file.
(Why: This is usually how program configuration are stored.
      The reference implementation supports it.)

The PF data SHOULD be serializable.
(Why: In order to save a PF instance state and restore it later.
      This achieve destruction recovery.
      The reference implementation supports it.)

**TODO**: XML ?

# Introspection
## Philosophy
In order to debug

## Requirements
User SHOULD be able to inspect PF data. Including:
 - listing
    - domains
    - configurations of a domains
    - parameters
    - a domain's associated parameters
 - displaying their properties. Including:
    - parameters values, min, max, size...
(Why: To offer run time debugging.)

PF MAY offer pretty print of data. Including:
 - printing parameter value in decimal
(Why: For human readability)
 - pretty print parameter tree (such as the Unix tree command for files)
(Why: In order to ease runtime debug.)


# Tuning
## Philosophy
Tuning is the ability to modify the PF data structure at runtime.

As the PF might model a complex system with its dynamic parameter value engine
(rule based in the default implementation), its behaviour might be hard to
understand and should be easily modified not correct.

To address this need, a fast modify-update-test cycle should be possible.

## Requirements
Users SHOULD be able to modify the PF inference engine behaviour (rules,
configuration...) with minimal effort.
This usually mean avoiding:
 - recompiling
 - restarting the host process/service
(Note: No requirement is made on the persistence of those changes,
       they may or may not disappear on PF restart.
       This could be implemented in several way, for example:
        - exposed in the PF API
        - changing a config file and sending a signal to the PF
        - providing a IPC
        - directly modifying the memory)
(Why: To enable a fast modify-update-test cycle during tuning.)

Tuning SHOULD be possible from the PF native API.
Why: In order to let the host system implement its own tuning mechanism.)

Users MAY be able to modify the parameters (types, identifiers, tree...) with
minimal effort (see previous requirement).
(Note: The reference implementation does not support it.)
(Why: To enable a fast modify-update-test cycle on PF configuration.)

Users SHOULD be able to modify the parameter values at any time.
This change SHOULD not be overwritten without a user action.
(Note: user overwritten user action could be a log out, leaving some tuning
       mode, forcing an inference engine update...)
(Why: Even if a parameter is managed by the inference engine,
      it often is useful (test, debugging) to overwrite its value temporally.).

A PF tuning capability MAY be disabled in a context where no tuning is needed.
(Why: The reference implementation does so (phone end users can not change the
      tuning).)

# Command line interface

The PF MAY offer a command line interface that binds to its IPC.
(Why: to have a reference way to interact with a PF without implementing its IPC
      protocol.)

This command line interface SHOULD support all tuning and introspection ability.
(Why: In order to be used in scripting and live tuning/debugging on an embedded
      system.)

This command line interface MAY offer argument auto completion.
(Why: Is more user friendly.)

# Bindings
The PF SHOULD expose its API in C.
(Why: The PF aims to be a hardware abstraction thus middle ware which is often
      written in C or a language compatible with C.
      Virtually all programing language support C Foreign Procedure Call,
      having a C API ease integration whichever the host language is.)

The PF MAY expose its API to multiple programing language.
(Why: The reference implementation has python bindings.)

# Performance

The reference Parameter Framework implementation is mainly intended for use in
consumer electronics such as smartphones and tablets. Such platforms are often
referred to as "embedded" platforms but their capacity today is so huge in
terms of both computing and memory that can can be considered as small personal
computers.

Moreover, since one of the Parameter Framework's primary feature is to
implement storage of a) a hardware description and b) settings, its memory
footprint largely depends on how many such items are stored.

For those reasons, there are no performance requirements imposed on the
architecture. Performance considerations are left to the implementation of the
Parameter Framework and/or the client and/or the build chain.

# Next
The following requirements are not implemented in the reference implementation
and are to be considered draft.

PF MAY support at least:
    - Linux (and Android)
    - Windows
    - Mac OSX
(Why: As the reference PF implementation leaves its original Android
      environment, needs emerge to use it on other platform.)

The PF host API SHOULD be structured.
I.e.: the PF, when requested for a list of domains, should return a list of
structured object, each containing configuration objects, containing their
values.
(Why: The reference implementation has a string oriented API.
      E.g/: The list of domains is returned as a concatenation of domains name
      in one big string.
      This leads to hard an hard to use API from C and C++ code.)

The PF host API SHOULD expose parameter values with the same API syncer use.
(Why: The current reference implementation abstracts the memory layout of
      parameters. This memory layout is specified in the parameter structure
      thus TODO)
