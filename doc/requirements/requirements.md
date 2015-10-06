% Parameter Framework \
High level requirements

<!--
Copyright (c) 2015, Intel Corporation
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
-->

<article class="markdown-body">

<note>Some requirements are only motivated by the fact that the reference
implementation implements them. Search for "reference implementation".</note>

# Introduction

The Parameter Framework is abreviated as PF in the rest of the document.

## Philosophy

The Parameter Framework aims to be a hardware control abstraction layer.
Specificaly the PF contains 3 stacked abstraction layers:

 1) hardware api abstraction
 2) hardware parameter abstraction
 3) high level hardware independent abstraction

The fundamental constraint on the underlined hardware is to be representable
by independent parameters. Ie: When changing one parameter it must not change
an other.

### Hardware api abstraction
The goal of this layer is to abstract the apis of the underline hardwares.
Each abstracted hardware usualy have different apis, this layer responsibility
is to set and get parameters using the underlined native api.

See the [syncer] chapter.
<!--Fixme why are pandoc auto references not working ? -->

### Hardware parameter abstraction
The goal if this layer is to name and organize and describing
the hardware parameter properties (domain of validity, size, human representation...).

See the "Parameters" chapter.

### High level hardware independent abstraction
The goal of this layer is to abstract the hardware parameters behind abstract parameters
(called criterion in the reference implementation).

This is done by linking those abstract parameters and the hardware parameters with
arbitrary rules.

See the "Rule based dynamic abstraction".

## Requirements

### Reusability
The PF **SHOULD** be reusable between components.
<why>To be reused in different components.</why>

### Instances independence
PF instances **MUST NOT** mutate each others.
<note>This may be implemented by not sharing any mutable data between PF instances.</note>
<why>Different PF instances are expected to be completely independent thus accessing one should not impact any others.</why>

# Parameters

<note>TODO: add a paragraph/requirement about parameter independences.
Ie parameter set order should not impact the final state.
Need to find justification for this. Maybe it is only a convention?
Maybe it is a consequences of the domains ?</note>


## Definitions

<dl>
<dt>Parameter</dt>
<dd>TODO</dd>
<dt>Hardware</dt>
<dd>System controlled by the PF. Not necessary material system. This term was
chosen because:

 - historically the PF reference implementation was used to abstract hardware
 - the subsystem term would arguably fit best is already used.

(FIXME: choose "subsystem" instead of "hardware" ?)
</dd>
</dl>

## Requirements
A PF **MUST** be able to handle parameters.
<why>because the PF aims to abstract hardware and model it by parameters.</why>

## Value

A parameter **MUST** have a value.
<why>because a parameter without value would not abstract any hardware.</why>

### Mutability
A PF **MUST** support mutable parameters.
<why>To control the underlined hardware.</why>

### Set ability
This value **MUST** be settable for a mutable parameter.
<why>By definition, a mutable parameter that can not be mutated it a immutable parameter.</why>

### Get ability
This value **SHOULD** be gettable for a mutable parameter.
<why>To dump all parameter value, debug a hardware state,
save parameters values, display the current hardware state,
for coherency with the immutable parameter...</why>

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

##### Supported types
A PF **SHOULD** support the following types.
If a type is chosen to be supported, it **MUST** respect all MUST clause,
**SHOULD** respect all SHOULD clause, **MAY** respect all MAY clause of the type.
<why>All type are not necessary to use the PF. For example any parameter could
be represented as an array of char (string). But this would not permit to
check parameter validity (invariants) nor a pretty display of the values.</why>

##### Typed API
Implementation **MAY** add another API to access a parameter value.
<why>For example a C++ implementation may give access to a string as an
std::string object.</why>

##### Integers

###### Signed and unsigned support
PF **SHOULD** support signed and unsigned integer parameters
<why>The reference implementation supports it.</why>

###### Size immutability
PF **MUST** support integer with invariant size.
<why>It is common in C API to expect numbers to have a fixed maximum size.</why>

###### ABI
The API to access it **MUST** respect C integer ABI.
<why>For easy access from C code.</why>

###### Supported size
Supported integer size **SHOULD** be at least 8, 16 and 32 bits.
<why>The reference implementation supports it.</why>

###### Min max support
PF **MAY** support constraining the parameter minimum and maximum value.
<why>To catch user out of valid range errors when changing the parameter
value.</why>

##### String
###### Support
PF **SHOULD** support array of characters.
<why>Everything that a computer can store fits in an array of characters. It can
be used as a fallback type if no other matches the parameter.</why>

###### String max size
The array maximum size **MAY** be invariant (immutable).
<unknown>This is what the reference implementation does.</unknown>

###### API
The API to access the string value **SHOULD** support null terminated character
array. As it is commonly done in C.
<why>For easy access from C code.</why>

##### Fix point parameter

###### Support
PF **SHOULD** support fix point parameters. I.e. integers divided by a fixed power
of two.
<unknown>The reference implementation supports it.</unknown>

###### API
The API to access the values **SHOULD** respect the Qm.n and UQm.n standards.
<why>It is the main standard for fix point parameters.</why>

###### Size
PF **SHOULD** support at least `0 <= m + n <= 31` for a Signed Qm.n and
`0 <= m + n <= 32` for an Unsigned Qm.n (or "UQm.n").
<unknown>The reference implementation supports it.</unknown>
<ko>The reference implementation only supports Signed Qn.m</ko>

###### Min and max support
PF **MAY** support constraining the parameter minimum and maximum value.
<why>To catch user out of valid range errors when changing the parameter
value.</why>
<unknown>The reference implementation does not support it</unknown>

##### Floating point
###### Support
PF **SHOULD** support floating point parameters .
<unknown>The reference implementation supports it.</unknown>

###### API
The API to access the values **SHOULD** respect C platform float abi.
<note>Usually the IEEE 754 standard.</note>

###### Size
PF **SHOULD** support at least 32 and 64 bit size floats.
<why>The reference implementation supports it.</why>
<unknown>The reference implementation only supports 32bits</unknown>

###### Min and max support
PF **MAY** support constraining the parameter minimum and maximum value.
<why>To catch user out of valid range errors when changing the parameter
value.</why>

##### Bit field

###### Support
PF **SHOULD** support 1 or more bit sized integers.
<unknown>The reference implementation supports it.</unknown>

###### Single bit access API
The API to access a bit parameter is implementation defined.
<why>C has no way to point to a single (or more) bits. Thus there is no </why>

###### Bit field access API
Such bit parameters **SHOULD** be grouped in a bit field.
A bit field is an ordered set of bit parameter.
The API to access a bit filed **SHOULD** give access to a packed bit
field following the C abi.
<note>This bit field may contain only bit parameter.</note>
<why>To offer a C compatible api to fit field.</why>


### Parameter adaptation

#### Definition
<dl>
<dt>Parameter adaptation<dt>
<dd>
A bijective pure function converting a parameter value between the syncer
and other parameter reader/writer (including the inference engine).

The adaptation function maps the syncer and client space. It:

 - scales the user value to the hardware value (client => syncer)
 - converts the hardware value to the user's value space. (syncer => client)

<why>For coherency a client getting a previously set parameter should return the setted value,
thus the transformation must be bijective.
</why>
</dd>
</dl>

#### Philosophy

Parameters exposed by hardware sometimes need to be normalized.
<note>For example a hardware integer parameter could have a range 64-128 but it might
be necessary for upper layer to access in a range 0-100.</note>\

This transformation can also permits to change the unit of a parameter.
<note>For example the hardware could expose a parameter in cm but it might better
to expose it in mm. </note>\

Parameters types offer a way to abstract underlined implementation.
<note>For example a Q2,2 (see [fix-point-parameter]) when setting 1
will be translated to 0100. </note>\

With parameter adaptation, types can be even further parameterised.
<note>For example, Qn,m Fix point parameter could be emulated with a $*2^n$
adaptation over an n + m integer. </note>\

Parameter adaptation could be implemented by the syncer.
Nevertheless syncers are supposed to contain only
business logic and should not be impacted by upper layer needs.

#### Requirements

##### Support
The following parameter adaptation **SHOULD** be supported

 - Affine adaptation: `affAd(value) = slope * value + offset` where slope and
   offset and user-defined constants
   <unknown>The reference implementation supports it.</unknown>

 - Logarithm adaptation: `logAd(base, value) = ln(value) / ln(base)` where
   `ln` is the natural logarithm and base is a user-defined constant.
   <unknown>The reference application supports it.</unknown>
   <note>The reference implementation also supports passing a floor value to be
   applied after conversion.</note>

##### Composition
A PF **MAY** offer Parameter adaptation composition. I.e. combine multiple parameter
adaptation
<note>E.g.: composing the affine and logarithm adaptation to
`compAd(value) = slope * logAd(base, value) + offset`.</note>
<why>To avoid combination explosion of parameter adaptations. The idea is to
builtin basic function and let the user compose them to meet its need.</why>
<ko>The reference application supports in a tricky way: the logarithm
adaptation is always combined with the affine adaptation</ko>

### Parameter tree
A parameter **SHOULD** be structured in a tree. Each parameter being a distinct
tree leaf.
<why>Tree is a simple data structure that can be easily represented and is
enough to map underlined layers.</why>

#### Identifier
Each node of the tree **SHOULD** have its own identifier with the same
characteristics (type, independence...) than a parameter.
<why>To represent the tree without treating the leaf nodes specifically.</why>


# Syncer

## Philosophy

The PF philosophy is to map the hardware characteristics to parameters.
In order to impact the hardware when parameters are modified, a hardware specific
code must be used.

Syncers are responsible for synchronizing the values of parameters to the underlined hardware.
Ie, it is the glue between hardware and parameters. It contains the code specific
to access an hardware.

The aim of the PF is to keep this hardware specific code as light as possible.

## Definition

<dl>
<dt>Syncer<dt>
<dd>
Entity that keeps synchronised PF parameters and their associated hardware.
</dd>
</dl>

## Requirements

### Mapping
A syncer **MUST** be mapped to one or more parameters.
<why>The hardware minimal access may be bigger than one parameter.</why>

### Uniqueness
One parameter **MUST NOT** be mapped to two or more syncer.
Ie: a parameter MUST be mapped to zero or one syncer.
<why>Which syncer should be responsible to retrieve the initial parameter value
if they are multiple per parameter?</why>

### Read hardware
A syncer **MUST** support retrieving the mapped parameters value from the mapped
hardware.
<why>to retrieve a parameter value at the start of the PF.</why>

#### Write hardware
A syncer **MUST** support setting the mapped parameters value to the mapped
hardware.
<why>to synchronise hardware on parameter change.</why>

#### API
This API **MAY** be a packed parameter structure, following the C ABI without
padding.
<note>This is what the reference implementation does.</note>
<unknown>TODO</unknown>

## Parameter introspection
The syncer API **SHOULD** allow introspection of the mapped parameters.
<why>the parameter structure may be useful for the syncer to communicate with
the hardware. For example a syncer might need each to know each associated
parameter type to send it to the hardware.</why>

## Plugins

 - This formation is object oriented. Requirements should not require any programing paradigm.
 - Is this section about syncer creation and builders too close to implementation ?

### Definition
The PF creates syncer using syncer builder.

### Requirements
The PF **MUST** be able to create syncers.
<why>To bind on the corresponding parameters.</why>

### Identifier

#### Syncer library
All syncers mapping to the same hardware **SHOULD** have their builders regrouped
in a syncer library.
<note>FIXME:

 - Is this syncer library concept not a definition ? Ie a syncer builder set.
 - The concept is needed by other requirement but it does not stand by itself.
 - Why is there a requirement of "same hardware" ?
      Is this not more a convention than a requirement ?

</note>
<why>To be able to link a group of parameters and a given hardware.
For example all parameters that are mapped to sound card should be linked to a
sound card syncer library. (Each parameter are then individually mapped to a specific syncer.)
</why>

#### Syncer ID
A syncer builder **MUST** have a unique identifier in its containing syncer
library.
<why>To uniquely identify the syncer that should bind on parameters. Given that
the syncer library has already been specified.</why>

#### Library UID
A syncer library **MUST** have a unique identifier in the host system.
<why>To identify the library associated to parameters.</why>

### Loading

#### DLL
Syncer library or/and builder **MAY** be loaded from dynamically linked libraries
(called syncer plugins).
<unknown>The reference implementation supports it.</unknown>

#### Plugin entry point
Such syncer plugins **SHOULD** have an unique entry point that -- when called --
should register its payload (syncer library/builder) in the provided gatherer.
<note>This permit to merge multiple syncer libraries in one shared
library.</note>
<unknown>The reference implementation supports it.</unknown>

#### Plugin interdependancies
Multiple syncer plugins, may depend on each other. The PF should appropriately
handle the case and not fail.
<unknown>The reference implementation supports it.</unknown>

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

### Sync on change
Syncer **SHOULD** synchronise the mapped hardware on parameter change.
<why>To always keep synchronise the underlined hardware and the PF
parameters.</why>

### Read hardware
Syncer **SHOULD** retrieve parameter value from the hardware if no value has be
set since the PF start.
<note>This is usually implemented on PF start, initialize the parameter values
with the mapped hardware current state.</note>
<why>To allow introspection of the hardware.</why>

### Explicit sync
A mode with synchronisation on client request **SHOULD** be supported.
<why>The user may want to group the synchronization of multiple parameters --
for instance if a syncer contains more than 1 parameter -- in order to avoid
undesired intermediary states.</why>

### Out of sync
Syncers **MAY** report an 'out-of-sync' condition indicating that the hardware
parameter values are not (or no longer) reflecting the last values set by the
Parameter Framework.
<why>This can happen when the underlying hardware subsystem
crashes/reboots/...</why>

#### Recovery
When a syncer reports an out-of-sync condition, the PF **MUST** try to resync
the hardware values.

# Rule based dynamic abstraction

## Philosophy

The PF offers parameters mapped on hardware. This is a good but weak
abstraction as there is often a 1/1 relation between a parameter and the hardware
it maps. Ie: parameter abstract how to access hardware and what hardware but
are still hardware specific.

A PF offers a mechanism to abstract the parameters to a higher level concept.

The goal is to hide numerous parameters and their dynamic values behind simple
and human friendly API.

It works by grouping parameters with similar management and defining
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
values of the same parameter.

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

### Support
A PF **MUST** offer configurations as described in the Definition chapter.
<note>rule based parameter engine does not manipulate directly values, it
applies configuration on the parameters.</note>
<unknown>This is what the reference implementation does.</unknown>

### Eligibility
Each configuration **MUST** be associated with a predicate that condition its
eligibility. A configuration with a predicate that evaluates to `true` is called
an "eligible configuration"
<why>This is what the reference implementation does.</why>

### Default
It **SHOULD** be possible to express a predicate to always evaluates to `true`.
Ie: It *SHOULD* be possible to make a configuration always eligible.
<why>In order to have parameters set to constant values or have a fallback
configuration in a domain -- see below.</why>

### Predicate implementation
The predicate **SHOULD** be a "selection criterion rule". See next chapter for a
definition.
<why>The reference implementation uses a boolean expression based engine.</why>

## Selection criterion

### State uniqueness
A selection criterion **MUST** have one, and only one, state at a given time.

### State validity
A selection criterion **MUST** have a always known immutable domain of definition.
Ie All the possible state that a selection criterion can take **MUST** be known
at all time.
<why>To be able to validate:\
 - rules on start\
 - state changes
</why>

### State domain specification
#### Naive
The selection criterion possible states **MUST** be specifiable by directly a
state set (`Input -> states == identity`)
<note>called **exclusive criterion**</note>
<note>An empty set is not allowed as the criterion could not have a state.</note>
<why>Any criterion can be created from this API.</why>

#### Combination
The selection criterion possible states **SHOULD** be specifiable by a combination
of values
<note>combination in the [mathematical sense](https://en.wikipedia.org/wiki/Combination)
`"ab" -> ["", "a", "b", "ab"]`</note>
<note>called **inclusive criterion**</note>
<note>An empty value set is allowed as its combination -- a set containing the
empty set -- would not be empty. The empty set would be the only possible
criteria state.</note>
<why>The reference implementation supports it.</why>

### Criteria number
The PF **SHOULD NOT** limit the number of criteria.

#### State number
The PF **SHOULD NOT** limit the number of possible states of any given criterion
<ko>The reference implementation only supports 32 values for an inclusive
criterion and 2^32 values for an exclusive criterion</ko>

### Definitions
<dl>
<dt>Selection criterion rule</dt>
<dd>
Function (in the mathematical sense) that **MUST** given selection criteria
return a Boolean. Ie, a [predicate](https://en.wikipedia.org/wiki/Predicate_%28mathematical_logic%29).
</dd>

<dt>Rule</dt>
<dd>
A Boolean expression of Selection criterion rules.
<note>implementation only allows AND and OR combination</note>
<dd>
</dl>

### Criterion changes

#### Multiple criterion change atomicity
The API to change criterion values **MUST** allow atomicity regarding
configuration application. I.e. it **MUST** be possible to change multiple
criterion values without triggering a configuration application.
<why>Two criterion might have an excluding state. If configuration application
was triggered after each criterion change this transitory incompatible state
would impact the system.
For example 2 criterion `Tx` and `Rx` with 2 values `"on"` and `"off"` may have
an incompatible state `Tx = Rx = "on"`. Ie this state is unspecified and the
inference engine would gave unknown result.
\
When going: \
 - from `Tx = "on" and Rx = "on"` (state 1) \
 - to `Tx = "off" and Rx = "off"` (state 2) \
<!-- FIXME: why are list closing the why block ?  -->
a transitory state `Tx = "on" and Rx = "on"` may be reached. Nevertheless
the inference engine must not be run on such. There must be a way to go
from one state 1 to state 2 without triggering configuration application.
</why>

### Rules

It **MUST** always be able to express a selection criterion rule from a given
selection criterion state.
I.e.: a criteria **MUST** always have a state that can be matched by a rule.
<why>If no rules can be formulated from a criterion state,
the hardware can not be abstracted in this state witch defeats the PF purpose.</why>

Parameter values change **SHOULD** be selected by Rules.
<why>A rule based inference engine has been chosen based on implementation and
configuration ease</why>

## Domains

### Definition
<dl>
<dt>Domain</dt>
<dd>
Ordered set of configuration, all of which contain the values for the
same parameters.
</dd>
</dl>

### Philosophy

When creating configurations for parameters, a pattern emerges.
Some parameters are naturally grouping together. Ie changing on the same predicates.

Without carefully crafting configuration predicates for mutual exclusivity,
multiples configuration of the same parameter could be eligible on the same
criterion state. This would lead to an ambiguity: which configuration should be applied.

Multiple solution could be imagine like:
 - ask to the client/user
 - having configuration predicate mutual exclusive
 - choose randomly
 - group configuration applicable on the same in a priority ordered set

The domains this specification recommend is this last solution.
It has been chosen as the recommended solution (just like parameter tree)
because it is a simple solution and is implemented in the reference implementation.

The constraint of this solution is that a configuration can no longer be shared
between domains. For example a global default configuration can not exist.
It must be split up for each domain.

This choice also force parameters to be independently accessible.

### Requirement

#### Configuration application ambiguity
There **MUST** be a mechanism to avoid ambiguity on multiple configuration eligibility
for the same parameter.
<why>Applying multiple configurations would leave the parameters in an unknown state.</why>

#### Domain support
Each configuration **SHOULD** be in a "domain" (see Definition chapter).
<why>Domains are mostly a way to define the priority of configuration application
for some parameters.</why>
<ko>It is not a MUST because this goal could also be achieve with (for
example) global configurations and per parameter priority. It is not a MAY
because the reference implementation uses domains.</ko>

#### Configuration priority
If multiple configuration are eligible, the first one **MUST** be applied.
<why>If multiple configuration are eligible, there must be a way to discriminate
them. The order was arbitrary chosen.
See the domain philosophy section for more information about this choice.</why>

#### Lazy application
If no configuration is eligible, no configuration **MUST** be applied.
<note>It means that if none of the configurations is eligible, none is applied.
This also mean that no function can be defined between criteria and states.
I.e.: parameter values MAY depend on previous selection criterion states.</note>
<why>This is what the reference implementation does.</why>

#### Sequence indifference
Parameter set and get order MUST not change the final state.
<why>Their is no way to order such access if the parameters are from different domains.</why>

#### Sequence aware domain
Domains **MAY** be sequence aware. Such domains update their associated
parameters in a specific, predictable and configurable order.
<ko>The reference application supports it.</ko>
<why>Some parameters might require specific ordering on set.
This is contradictory with the fact that parameters MUST be accessed independently.</why>

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

### Deserializable
The PF data **MUST** be deserializable.
<why>Otherwise a PF instance could only be created empty and then be filled by
the tuning interface. The reference implementation supports it.</why>

### Deserializable from a file
The PF data **SHOULD** be deserializable from a config file.
<why>This is usually how program configuration are stored. The reference
implementation supports it.</why>

### Serializable
The PF data **SHOULD** be serializable.
<why>In order to save a PF instance state and restore it later. This achieve
destruction recovery. The reference implementation supports it.</why>

### (De)Serialization of individual data
The PF data **SHOULD** be serializable/deserializable by parts.
<why>For easier configuration management: for versioning; for selecting only wanted parts of a
complete configuration.</why>

### Serialization format
**TODO**: XML ?

### Implementation
Syncer build and syncer library identifiers **SHOULD** be strings.
<unknown>The reference application does so.</unknown>

# Post mortem debug
A PF **MAY** save all data needed to replay it's state evolution.
<note>Eg: log criterion change, configuration application, parameter
external change.

This is implementing by logging events by the reference implementation.</note>
<why>In order for the user to debug the user configuration after a bug occurred (post mortem or rare bug).
This is kind of like the bash -x feature.
</why>

# Introspection
## Philosophy
In order to debug the user configuration, allow introspection of PF data at runtime.
As data is meant to be displayed to user, lots are requirements are towards
pretty printing PF data.

## Requirements

### Support
User **SHOULD** be able to inspect PF data.
<why>To offer run time debugging.
This includes: \
- listing \
\ \ \ \ + domains\
\ \ \ \ + configurations of a domains\
\ \ \ \ + parameters\
\ \ \ \ + a domain's associated parameters\
- getting their properties. Including:\
\ \ \ \ + parameters values, min, max, size...
</why>

### Pretty print
PF **MAY** offer pretty print of data. Including:

- printing parameter value in decimal
    <why>For human readability</why>
- pretty print parameter tree (such as the Unix tree command for files)
    <why>In order to ease runtime debug.</why>

### Rogue parameter
Users **SHOULD** be able to modify rogue parameters through the native API at
all time.
<why>Otherwise, a rogue parameter is of no use.</why>
<ko>In the reference implementation, under certain conditions, this is not
possible (tuning mode)</ko>

### Parameter Identifiers

#### Support
Every parameter **MUST** have an identifier that uniquely identifies it.
<why>to identify a parameter outside the framework</why>

#### String
This identifier **SHOULD** be a string.
<why>So that a human user can identify a parameter with ease.</why>

#### Determinism
Two PF instances with the same parameters **MUST** have the same identifier for
those parameters.
I.e. this identifier should be the same across all instances with the same
configuration.
<why>Persistence of parameter identifier across PF instances with the same
configuration. To identify parameters independently of the host machine and PF
instance</why>

#### Tree path
The identifier of each node of a parameter tree **SHOULD** be a combination of its
parents. More specifically, if the identifier is a string it **SHOULD** be
formated in a similar way as a file system path. E.g. `/root/child1/4/parameter1`.
<why>Usual syntax to address trees.</why>


# Tuning

## Definition

<dl>
<dt>Tuning</dt>
<dd>
Tuning is the ability to modify the PF data structure at runtime.
</dd>
</dl>

<note>Is this naming "Tuning" not too audio oriented.</note>

## Philosophy

As the PF might model a complex system with its dynamic parameter value engine
(rule based in the default implementation), its behaviour might be hard to
understand and should be easily modified not correct.

To address this need, a fast modify-update-test cycle should be possible.

## Requirements

### Inference engine
Users **SHOULD** be able to modify the PF inference engine behaviour (rules,
configuration...) with minimal effort.
<why>To enable a fast modify-update-test cycle during tuning.
This usually mean avoiding for the user to: \
 - recompile \
 - restart the host process/service
</why>

<note>No requirement is made on the persistence of those changes, they may or
may not disappear on PF restart. This could be implemented in several way, for
example:

- exposed in the PF API
- changing a config file and sending a signal to the PF
- providing a IPC
- directly modifying the memory

</note>

### Native api
Tuning **SHOULD** be possible from the PF native API.
<why>In order to let the host system implement its own tuning mechanism.</why>

### Parameter overwriting
Users **SHOULD** be able to modify the parameter values at any time.
This change **SHOULD NOT** be overwritten without a user action.
<note>User overwritten user action could be a log out, leaving some tuning mode,
forcing an inference engine update...</note>
<why>Even if a parameter is managed by the inference engine, it often is useful
(test, debugging) to overwrite its value temporally.</why>

### Disabling
A PF tuning capability **MAY** be disabled in a context where no tuning is needed.
<why>The reference implementation does so (phone end users can not change the
tuning).</why>

# Command line interface
<ko>Is this not an implementation detail? Does a client really needs it?</ko>

## Support
The PF **MAY** offer a command line interface that binds to its IPC.
<why>To have a reference way to interact with a PF without implementing its IPC
protocol.</why>
<note>This requirement is fulfilled by remote-processor and remote-command on the reference implementation.</note>

## Introspection & tunning
This command line interface **SHOULD** support all tuning and introspection ability.
<why>In order to be used in scripting and live tuning/debugging on an embedded
system.</why>

## Auto completion
This command line interface **MAY** offer argument auto completion.
<why>Is more user friendly.</why>

# Bindings

## C
The PF **SHOULD** expose its API in C.
<why>The PF aims to be a hardware abstraction thus middle ware which is often
written in C or a language compatible with C. Virtually all programing language
support C Foreign Procedure Call, having a C API ease integration whichever the
host language is.</why>

## Programing language
The PF **MAY** expose its API to multiple programing language.
<unknown>The reference implementation has python bindings.</unknown>

# Performance

The reference Parameter Framework implementation is mainly intended for use
in consumer electronics such as smartphones and tablets. Such platforms are
often referred to as "embedded" platforms but their capacity today is so huge in
terms of both computing and memory that they can be considered as small personal
computers.

Moreover, since one of the Parameter Framework's primary feature is to implement
storage of

 - hardware description
 - settings

its memory footprint largely depends on how many such items are stored.

For those reasons, there are no performance requirements imposed on the
architecture. Performance considerations are left to the implementation of the
Parameter Framework and/or the client and/or the build chain.

# Next

<ko>
The following requirements are not implemented in the reference implementation
and are to be considered draft.
</ko>

## Multi OS
PF **MAY** support at least:

 - Linux (and Android)
 - Windows
 - Mac OSX

<why>As the reference PF implementation leaves its original Android environment,
needs emerge to use it on other platform.</why>

## Tuning
### Get and set multiple parameter values in one request
#### Atomicity
When setting multiple parameters from one client request,
and when one or more parameter value is invalid (eg. out of range),
no parameter **SHOULD** be set.
Eg: an invalid request to change parameters **SHOULD** not impact the parameters
values nor the subsystems.
<note>This may be implemented by first checking parameters validity
before setting them, or implementing a rollback mechanism, or any other way.</note>
<why>To provide parameter mutation atomicity to the client.
This is especially important if the client wants to implement parameter consistency.
Eg: let two parameters have excluding values,
if a transaction fail after the first parameter is set but not the second,
the excluding constraint may be violated.
It also usefull for the client to know the state of the parameters
after a parameter set without having to query the PF.</why>

#### Access parameters as Xml
Getting and setting the content of one or more ([one, all]) parameters **SHOULD**
be possible in xml.
<why>For performance reason. Tools often need to update multiple parameter
and having one call per parameter is too slow. (benchmark ?).
This feature permit the client to save and restore from an external database parameter
values a la `alsa.state`.</why>

#### Access parameters as binary
The PF host API **SHOULD** expose parameter values with the same API syncer use.
<why>The current reference implementation abstracts the memory layout of
parameters. This memory layout is specified in the parameter structure thus
is known by the client.</why>

## Stage and commit Sync
Explicit sync **SHOULD** only sync parameters which values were updated since last sync.
<why>For performance reason or when an hardware does not support certain
transition state, manual parameter synchronisation is requested.

Sync request was implemented in the reference implementation by syncing all
parameters, including the one that were not changed since last sync.

For performance reason only the changed parameters should be send to hardware.</why>


## Structured api API
The PF host API **SHOULD** be structured.
I.e.: the PF, when requested for a list of domains, should return a list of
structured object, each containing configuration objects, containing their
values...
<why>The reference implementation has a string oriented API. E.g/: The list of
domains is returned as a concatenation of domains name in one big string. This
leads to hard to use API from C and C++ code. Especially for testing</why>

### Implementation language
The main implementation will transition to C++11 for
 - cross platform support of multi-threading
 - remove dependency to pthread
 - reduce the gap with the "next" branch
It will be compatible with android thank to clang's libc++"

<note>Put this in a design document.</note>

## Long term
The following requirements are not planned to be implemented any time soon as
their is not need identified but are rather a long term guidance.

### Custom parameter types
The client **MAY** inject custom parameters types.
<why>As the client creates parameters it should also be able to specify the
parameter contains ie its types. Without this possibility the client has to
choose a built-in that may not match what he wants.

For example representing a prime number with an integer would not allow to enforce primness.

For example a complex number could be represented with two float but `a+bi` format
could not be used.

For example stocking a parameter with a dynamic type, say either a string or a number
could be done with a boolean a string and a number but this could not be pretty
print and not memory efficient.
</why>

### Structure tunning
Users **MAY** be able to modify the parameters (types, identifiers, tree...) with
minimal effort (in the same way they can modify the inference engine).
<ko>The reference implementation does not support it.</ko>
<why>To enable a fast modify-update-test cycle on PF configuration.</why>

### Immutable parameters
A PF **MAY** support immutable parameters, i.e. parameters which value is determined
on start then read only.
<why>To permit hardware read only value reflection.</why>
<ko>This is not implemented in the PF reference implementation.</ko>

This value **MUST** be gettable for an immutable parameter.
<why>A parameter that can not be accessed (read or write) is of no use.</why>

### Endianess adaptation
A parameter or a block of parameters might be presented by the Parameter
Framework but only used as a passthrough to the underlying subsystem (think
"`(void *)` interfaces"). It is then possible that the endianess of the
subsystem differs from the one the Parameter Framework is running on, an
endianness adaptation would allow supporting those cases.

This can be seen as related to the "Parameter Adaptation" requirement or even
as a special case.

</article>
