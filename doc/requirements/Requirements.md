<article class="markdown-body">

High level requirements
=======================

<nb>Some requirements are only motivated by the fact that the reference
implementation implements them. Search for "reference implementation".</nb>

# Introduction
The PF **MUST** be a library.
<why>To be reused in different components.</why>

PF instances **MUST NOT** share any mutable data.
<why>to guarantee that different PF instances will not impact each others.</why>

# Parameters

A PF **MUST** be able to handle parameters.
<why>because the PF aims to abstract hardware and model it by parameters.</why>

## Definitions

<dl>
<dt>Hardware</dt>
<dd>System controlled by the PF. Not necessary material system. This term was
chosen because:

 - historically the PF reference implementation was used to abstract hardware
 - the subsystem term would arguably fit best is already used.

(FIXME: choose subsystem instead, it is not used in fact.)
</dd>
</dl>

## Value

A parameter **MUST** have a value.
<why>because a parameter without value would not abstract any hardware.</why>

A PF **MUST** support mutable parameters.
<why>To control the underlined hardware.</why>

A PF **MAY** support immutable parameters, i.e. parameters which value is determined
on start then read only.
<why>To permit hardware read only value reflection.</why>
<ko>This is not implemented in the PF reference implementation.</ko>

This value **MUST** be gettable and settable (except if immutable).
<why>A parameter that can not be accessed is of no use.</why>

A value **MUST** be convertible to a string.
<why>To display a parameter value to the human user for debugging purpose.</why>

A bijective conversion string <-> value **SHOULD** exist.
<why>To support serialization and deserialization of the parameter values. It is
not a MUST because a PF implementation could offer serialization to custom --
not string based -- format.</why>

### Data type

#### Definition

<dl>
<dt>Data type</dt>
<dd>
All parameters have a data type. A data type designates parameter invariants.

A data type is the meaning of the data and the way values of that type can be
stored.
</dd>
</dl>


#### Philosophy

A data type defines the value properties:

 - memory layout
 - value constrains

A value type is mostly used to:

 - pretty display parameter values (not just a as an array of bits)
 - check for user error when setting it (out of bound, invalid...)
 - offer a type safe API

#### Requirements

A PF **SHOULD** support the following types.
If a type is chosen to be supported, it **MUST** respect all MUST clause,
**SHOULD** respect all SHOULD clause, **MAY** respect all MAY clause of the type.
<why>All type are not necessary to use the PF. For example any parameter could
be represented as an array of char (string). But this would not permit to
check parameter validity (invariants) nor a pretty display of the values.</why>

Implementation **MAY** add another API to access a parameter value.
<why>For example a C++ implementation may give access to a string as an
std::string object.</why>

##### Integers
PF **SHOULD** support signed and unsigned integer parameters
<why>The reference implementation supports it.</why>

PF **MUST** support integer with invariant size.
<why>It is common in C API to expect numbers to have a fixed maximum size.</why>

The API to access it **MUST** respect C integer ABI.
<why>For easy access from C code.</why>

Supported integer size **SHOULD** be at least 8, 16 and 32 bits.
<why>The reference implementation supports it.</why>

PF **MAY** support constraining the parameter minimum and maximum value.
<why>To catch user out of valid range errors when changing the parameter
value.</why>

##### String
PF **SHOULD** support array of characters.
<why>Everything that a computer can store fits in an array of characters. It can
be used as a fallback type if no other matches the parameter.</why>

The array maximum size **MAY** be invariant (immutable).
<nb>This is what the reference implementation does.</nb>

The API to access the string value **SHOULD** support null terminated character
array. As it is commonly done in C.
<why>For easy access from C code.</why>

##### Fix point parameter
PF **SHOULD** support fix point parameters. I.e. integers divided by a fixed power
of two.
<why>The reference implementation supports it.</why>

The API to access the values **SHOULD** respect the Qm.n and UQm.n standards.
<why>It is the main standard for fix point parameters.</why>

PF **SHOULD** support at least `0 <= m + n <= 31` for a Signed Qm.n and
`0 <= m + n <= 32` for an Unsigned Qm.n (or "UQm.n").
<why>The reference implementation supports it.</why>
<ko>The reference implementation only supports Signed Qn.m</ko>

PF **MAY** support constraining the parameter minimum and maximum value.
<why>To catch user out of valid range errors when changing the parameter
value.</why>
<ko>The reference implementation does not support it</ko>

##### Floating point
PF **SHOULD** support floating point parameters .
<why>The reference implementation supports it.</why>

The API to access the values **SHOULD** respect the IEEE 754 standard.

PF **SHOULD** support at least 32 and 64 bit size floats.
<why>The reference implementation supports it.</why>
<ko>The reference implementation only supports 32bits</ko>

PF **MAY** support constraining the parameter minimum and maximum value.
<why>To catch user out of valid range errors when changing the parameter
value.</why>

##### Bit field following the C ABI
PF **SHOULD** support 1 or more bit sized integers.
<why>The reference implementation supports it.</why>

Such parameters called are regrouped in a so called bit parameter block.
The API to access a bit parameter block **SHOULD** give access to a packed bit
field.
<why>The reference implementation supports it.</why>

### Parameter adaptation

#### Philosophy
Parameters exposed by hardware sometimes need to transform a parameter value.
For example an hardware integer parameter could have a range 64-128 but it might
be necessary for upper layer to access in a range 0-100.

This transformation (called parameter adaptation in the rest of the document)
could be done by the syncer. Nevertheless syncers are supposed to contain only
business logic and should not be impacted by upper layer needs.

#### Definition
<dl>
<dt>Parameter adaptation<dt>
<dd>
A bijective pure function converting a parameter value between the syncer
and other parameter reader/writer (including the inference engine).

<why>It must be bijective in order to a) scale the user value to the hardware
value and b) convert the hardware value to the user's value space.)</why>
</dd>
</dl>

#### Requirements
The following parameter adaptation **SHOULD** be supported

 - Affine adaptation: `affAd(value) = slope * value + offset` where slope and
   offset and user-defined constants
   <why>The reference implementation supports it.</why>

 - Logarithm adaptation: `logAd(base, value) = ln(value) / ln(base)` where
   `ln` is the natural logarithm and base is a user-defined constant.
   <why>The reference application supports it.</why>
   <nb>The reference implementation also supports passing a floor value to be
   applied after conversion.</nb>

A PF **MAY** offer Parameter adaptation composition. I.e. combine multiple parameter
adaptation
<nb>E.g.: composing the affine and logarithm adaptation to
`compAd(value) = slope * logAd(base, value) + offset`.</nb>
<why>To avoid combination explosion of parameter adaptations. The idea is to
builtin basic function and let the user compose them to meet its need.</why>
<ko>The reference application supports in a tricky way: the logarithm
adaptation is always combined with the affine adaptation</ko>

## Identifiers
Every parameter **MUST** have an identifier that uniquely identifies it.
<why>to identify a parameter outside the framework</why>

This identifier **SHOULD** be a string.
<why>so that the (human</why> user can identify a parameter with ease.)

Two PF instances with the same parameters **MUST** have the same identifier for
those parameters.
I.e. this identifier should be the same across all instances with the same
configuration.
<why>Persistence of parameter identifier across PF instances with the same
configuration. To identify parameters independently of the host machine and PF
instance</why>

### Parameter tree
A parameter **MUST** be structured in a tree. Each parameter being a distinct
tree leaf.
<why>Tree is a simple data structure that can be easily represented and is
enough to map underlined layers.</why>

Each node of the tree **SHOULD** have its own identifier with the same
characteristics (type, independence...) than a parameter.
<why>To represent the tree without treating the leaf nodes specifically.</why>

The identifier of each node of the tree **SHOULD** be a combination of its
parents. More specifically, if the identifier is a string it should be
formated the same way as a file system path. E.g. the parameter named
`/root/child1/4/parameter1`.
<why>Usual syntax to address trees.</why>


# Syncer

The PF philosophy is to map the hardware characteristics to parameters.

A syncer **MUST** be mapped to one or more parameters.
<why>The hardware minimal access may be bigger than one parameter.</why>

One parameter **MUST NOT** be mapped to two or more syncer.
<why>Which syncer should be responsible to retrieve the initial parameter value
if they are multiple per parameter?</why>

A syncer **MUST** support retrieving the mapped parameters value from the mapped
hardware.
<why>to retrieve a parameter value at the start of the PF.</why>

A syncer **MUST** support setting the mapped parameters value to the mapped
hardware.
<why>to synchronise hardware on parameter change.</why>

Syncers **MUST** retrieve and set the parameters value from the PF core.
<why>to be able to synchronise with hardware.</why>

This API **MAY** be a packed parameter structure, following the C ABI without
padding.
<nb>This is what the reference implementation does.</nb>
<why>???</why>

## Introspection
The syncer API **SHOULD** allow introspection of the mapped parameters.
<why>the parameter structure may be useful for the syncer to communicate with
the hardware.</why>

## Plugins
The PF **MUST** be able to create syncers.
<why>to bind on the corresponding parameters.</why>

### Definition
The PF creates syncer using syncer builder.

### Identifier
All syncers mapping to the same hardware **SHOULD** have their builders regrouped
in a syncer library.
<why>to be able to link a group of parameters and a given hardware.</why>

A syncer builder **MUST** have a unique identifier in its containing syncer
library.
<why>To uniquely identify the syncer that should bind on parameters. Given that
the syncer library has already been specified.</why>

A syncer library **MUST** have a unique identifier in the host system.
<why>To identify the library associated to parameters.</why>

Syncer build and syncer library identifiers **SHOULD** be strings.
<why>The reference application does so.</why>

### Loading
Syncer library or/and builder **MAY** be loaded from dynamically linked libraries
(called syncer plugins).
<why>The reference implementation supports it.</why>

Such syncer plugins **SHOULD** have an unique entry point that -- when called --
should register its payload (syncer library/builder) in the provided gatherer.
<nb>This permit to merge multiple syncer libraries in one shared
library.</nb>
<why>The reference implementation supports it.</why>

Multiple syncer plugins, may depend on each other. The PF should appropriately
handle the case and not fail.
<why>The reference implementation supports it.</why>

## Mapping
### Definition

<dl>
<dt>Virtual Parameter</dt>
<dd>
A parameter not bound to a syncer.
(Todo: remove if not used in the requirements.)
</dd>
</dl>

### Requirements
**TODO**:
 - Plugins
 - association builder <-> parameters

## Sync

Syncer **SHOULD** synchronise the mapped hardware on parameter change.
<why>To always keep synchronise the underlined hardware and the PF
parameters.</why>

Syncer **SHOULD** retrieve parameter value from the hardware if no value has be
set since the PF start.
<nb>This is usually implemented on PF start, initialize the parameter values
with the mapped hardware current state.</nb>
<why>To allow introspection of the hardware.</why>

A mode with synchronisation on client request **SHOULD** be supported.
<why>The user may want to group the synchronization of multiple parameters --
for instance if a syncer contains more than 1 parameter -- in order to avoid
undesired intermediary states.</why>

Syncers **MAY** report an 'out-of-sync' condition indicating that the hardware
parameter values are not (or no longer) reflecting the last values set by the
Parameter Framework.
<why>This can happen when the underlying hardware subsystem
crashes/reboots/...</why>

When a syncer reports an out-of-sync condition, the PF **MUST** try to resync
the hardware values.

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

<dl>
<dt>Configuration</dt>
<dd>
Set of values for different parameters. A configuration **MUST NOT** contain 2
values of the same parameters.

For example, given a PF with 3 integer parameters A,B,C, a configuration can
contain:

 - 1 value: (A) or (B) or (C); or
 - 2 values: (A,B) or (A,C) or (B,C); or
 - 3 values: (A,B,C).
</dd>

<dt>Rogue Parameter</dt>
<dd>
A Parameter that is not contained by any configuration.
<dd>
</dl>

## Configuration

A PF **MUST** offer configurations as described in the Definition chapter.
<nb>rule based parameter engine does not manipulate directly values, it
applies configuration on the parameters.</nb>
<why>This is what the reference implementation does.</why>

Each configuration **MUST** be associated with a predicate that condition its
eligibility. A configuration with a predicate that evaluates to `true` is called
an "eligible configuration"
<why>This is what the reference implementation does.</why>

It **SHOULD** be possible to express a predicate to always evaluates to `true`.
<why>in order to have parameters set to constant values or have a fallback
configuration in a domain -- see below.</why>

The predicate **SHOULD** be a "selection criterion rule". See next chapter for a
definition.
<why>The reference implementation uses a boolean expression based engine.</why>

## Selection criterion

A selection criterion **MUST** have one, and only one, state at a given time.

A selection criterion **MUST** know at construction all possible states.
<why>To be able to validate: -- rules on start -- state changes</why>

The selection criterion possible states **MUST** be specifiable by directly a
state set (`Input -> states == identity`)
<nb>called **exclusive criterion**</nb>
<nb>An empty set is not allowed as the criterion could not have a state.</nb>
<why>any criterion can be crated with this one.</why>

The selection criterion possible states **SHOULD** be specifiable by a combination
of values
<nb>combination in the mathematical sense `"ab" -> ["", "a", "b", "ab"]`</nb>
<nb>called **inclusive criterion**</nb>
<nb>An empty value set is allowed as its combination -- a set containing the
empty set -- would not be empty. The empty set would be the only possible
criteria state.</nb>
<why>The reference implementation supports it.</why>

The PF **MUST NOT** limit the number of criteria.

The PF **MUST NOT** limit the number of states of any given criterion
<ko>The reference implementation only supports 32 values for an inclusive
criterion and 2^32 values for an exclusive criterion</ko>

### Definitions
<dl>
<dt>Selection criterion rule</dt>
<dd>
Function (in the mathematical sense) that **MUST** given selection criteria
return a Boolean.
</dd>

<dt>Rule</dt>
<dd>
A Boolean expression of Selection criterion rules.
<nb>implementation only allows AND and OR combination</nb>
<dd>
</dl>

### Criterion change

The API to change criterion values **MUST** allow atomicity regarding
configuration application. I.e. it **MUST** be possible to change multiple
criterion values without triggering a configuration application.
<why>Two criterion might have an excluding state. If configuration application
was triggered after each</why>

### Rules

It **MUST** always be able to express a selection criterion rule from a given
selection criterion
I.e.: a criteria **MUST** always have a state that can be matched by a rule.
<why>if no rules can be formulated from a criterion, it is useless</why>

Parameter values change **SHOULD** be selected by Rules.
<why>A rule based inference engine has been chosen based on implementation and
configuration ease</why>

It **MUST** be possible to express a Rule that is always True.
<why>In order to make a configuration "always applicable"</why>

## Domains

### Definition
<dl>
<dt>Domain</dt>
<dd>
Ordered set of configuration, all of which contain the values for the
same parameters.
</dd>
</dl>

### Requirement

Each configuration **SHOULD** be in a "domain" (see Definition chapter).
<why>Domains are mostly a way to define the priority of "Scenarios" for some
parameters. It is not a MUST because this goal could also be achieve with (for
example) global configurations and per parameter priority. It is not a MAY
because the reference implementation uses domains.</why>

If multiple configuration are eligible, the first one **MUST** be applied.
<why>If multiple configuration are eligible</why>

If no configuration is eligible, no configuration **MUST** be applied.
<nb>It means that if none of the configurations is eligible, none is applied.
This also mean that no function can be defined between criteria and states.
I.e.: parameter values MAY depend on previous selection criterion states.</nb>
<why>This is what the reference implementation does.</why>

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
The PF data **MUST** be deserializable.
<why>Otherwise a PF instance could only be created empty and then be filled by
the tuning interface. The reference implementation supports it.</why>

The PF data **SHOULD** be deserializable from a config file.
<why>This is usually how program configuration are stored. The reference
implementation supports it.</why>

The PF data **SHOULD** be serializable.
<why>In order to save a PF instance state and restore it later. This achieve
destruction recovery. The reference implementation supports it.</why>

The PF data **SHOULD** be serializable/deserializable by parts. <why>For easier
configuration management: for versioning; for selecting only wanted parts of a
complete configuration.</why>

**TODO**: XML ?

**TODO**: get the binary content of an element.

# Introspection
## Philosophy
In order to debug

## Requirements
User **SHOULD** be able to inspect PF data.
<why>To offer run time debugging.</why>
This includes:

- listing
    - domains
    - configurations of a domains
    - parameters
    - a domain's associated parameters
- getting their properties. Including:
    - parameters values, min, max, size...

PF **MAY** offer pretty print of data. Including:

- printing parameter value in decimal
    <why>For human readability</why>
- pretty print parameter tree (such as the Unix tree command for files)
    <why>In order to ease runtime debug.</why>

Users **SHOULD** be able to modify rogue parameters through the native API at
all time.
<why>Otherwise, a rogue parameter is of no use.</why>
<ko>In the reference implementation, under certain conditions, this is not
possible (tuning mode)</ko>

# Tuning
## Philosophy
Tuning is the ability to modify the PF data structure at runtime.

As the PF might model a complex system with its dynamic parameter value engine
(rule based in the default implementation), its behaviour might be hard to
understand and should be easily modified not correct.

To address this need, a fast modify-update-test cycle should be possible.

## Requirements
Users **SHOULD** be able to modify the PF inference engine behaviour (rules,
configuration...) with minimal effort.
<why>To enable a fast modify-update-test cycle during tuning.</why>
This usually mean avoiding:

- recompiling
- restarting the host process/service

<nb>
No requirement is made on the persistence of those changes, they may or
may not disappear on PF restart. This could be implemented in several way, for
example:

- exposed in the PF API
- changing a config file and sending a signal to the PF
- providing a IPC
- directly modifying the memory
</nb>

Tuning **SHOULD** be possible from the PF native API.
<why>In order to let the host system implement its own tuning mechanism.</why>

Users **MAY** be able to modify the parameters (types, identifiers, tree...) with
minimal effort (see previous requirement).
<ko>The reference implementation does not support it.</ko>
<why>To enable a fast modify-update-test cycle on PF configuration.</why>

Users **SHOULD** be able to modify the parameter values at any time.
This change **SHOULD NOT** be overwritten without a user action.
<nb>user overwritten user action could be a log out, leaving some tuning mode,
forcing an inference engine update...</nb>
<why>Even if a parameter is managed by the inference engine, it often is useful
(test, debugging) to overwrite its value temporally.</why>

A PF tuning capability **MAY** be disabled in a context where no tuning is needed.
<why>The reference implementation does so (phone end users can not change the
tuning).</why>

# Command line interface

The PF **MAY** offer a command line interface that binds to its IPC.
<why>to have a reference way to interact with a PF without implementing its IPC
protocol.</why>

This command line interface **SHOULD** support all tuning and introspection
ability.
<why>In order to be used in scripting and live tuning/debugging on an embedded
system.</why>

This command line interface **MAY** offer argument auto completion.
<why>Is more user friendly.</why>

# Bindings
The PF **SHOULD** expose its API in C.
<why>The PF aims to be a hardware abstraction thus middle ware which is often
written in C or a language compatible with C. Virtually all programing language
support C Foreign Procedure Call, having a C API ease integration whichever the
host language is.</why>

The PF **MAY** expose its API to multiple programing language.
<why>The reference implementation has python bindings.</why>

# Performance

The reference Parameter Framework implementation is mainly intended for use
in consumer electronics such as smartphones and tablets. Such platforms are
often referred to as "embedded" platforms but their capacity today is so huge in
terms of both computing and memory that they can be considered as small personal
computers.

Moreover, since one of the Parameter Framework's primary feature is to implement
storage of a) a hardware description and b) settings, its memory footprint
largely depends on how many such items are stored.

For those reasons, there are no performance requirements imposed on the
architecture. Performance considerations are left to the implementation of the
Parameter Framework and/or the client and/or the build chain.

# Next
<ko>
The following requirements are not implemented in the reference implementation
and are to be considered draft.
</ko>

PF **MAY** support at least:

 - Linux (and Android)
 - Windows
 - Mac OSX

<why>As the reference PF implementation leaves its original Android environment,
needs emerge to use it on other platform.</why>

The PF host API **SHOULD** be structured.
I.e.: the PF, when requested for a list of domains, should return a list of
structured object, each containing configuration objects, containing their
values.
<why>The reference implementation has a string oriented API. E.g/: The list of
domains is returned as a concatenation of domains name in one big string. This
leads to hard an hard to use API from C and C++ code.</why>

The PF host API **SHOULD** expose parameter values with the same API syncer use.
<why>The current reference implementation abstracts the memory layout of
parameters. This memory layout is specified in the parameter structure thus
TODO</why>
</article>
