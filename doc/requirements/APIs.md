<article class="markdown-body">

Link from APIs to requirements
==============================

# `ParameterMgrPlatformConnector`

This class is the main client interface.

See
<http://01org.github.io/parameter-framework/doc/classCParameterMgrPlatformConnector.html>
for the complete documentation of each API.

## `createSelectionCriterionType` and `createSelectionCriterion`

Implement the **Selection Criterion** creation requirements. See section **5.4**
of the requirements documentation.

## `getSelectionCriterion`

There is no requirement to retreive a criterion by it's name.

## `setLogger`

Implement [req-post-mortem-debug].

Implements a way for the user to provide a log backend.
This is not a requirentment currently.
Maybe it is a miss?

## `start` and `isStarted`

**???**

There is no "two-step-creation" requirement. This is an implementation detail.

## `applyConfigurations`

Deffered application requirement; see [req-multiple-criterion-change-atomicity]
See also the Selection Criterion APIs below.

## `createParameterHandle`

Introspection and direct parameter read/write access;
see [req-introspection] especialy the point about parameter properties.
See also the Parameter Handle APIs below.

## `setForceNoRemoteInterface` and `getForceNoRemoteInterface`

Tuning capability activation control; see section [req-disabling] in [req-tuning].

## `setFailureOnMissingSubsystem` and `getFailureOnMissingSubsystem`

**???**

Currently only used for XML generation. There is no requirement associated.

## `setFailureOnFailedSettingsLoad` and `getFailureOnFailedSettingsLoad`

**???**

Currently only used for XML generaton. There is no requirement associated.

## `setSchemaUri` and `getSchemaUri`; `setValidateSchemasOnStart` and `getValidateSchemasOnStart`

**???**

Currently only used for XML generation. There is no requirement associated.

# `ParameterMgrFullConnector`

This is another client interface that covers the same purposes and APIs than
`ParameterMgrPlatformConnector` and extands them. See this class' description.
Added APIs are described in this chapter.

## `setTuningMode` and `isTuningModeOn`

Tuning capability activation control; see section [req-parameter-overwriting]. 
It adds another level of flexibility over `setForceNoRemoteInterface`.
The semantic of these two APIs overlap but **neither one fully contains the other**.

**TODO**: fix the semantics.

## `setValueSpace` and `isValueSpaceRaw`; `setOutputRawFormat` and `isOutputRawFormatHex`

Serialization control (FIXME: what about deserialization ?). Seems **UNUSED**.
There is no requirement associated.

## `setAutoSync`, `isAutoSyncOn` and `sync`

Synchronization on client request; see section [req-explicit-sync].

## `accessParameterValue`

Parameter value direct access. **This overlaps with ParameterHandle.**
Implements the same requirement: [req-parameter-overwriting]

## `getParameterMapping`

Part of the Introspection requirements. See [req-introspection] especialy implement
intropsepction of the mapped syncer.

## Tuning APIs

All of the following APIs implement the Tuning requirement; see section [req-tuning]

- `accessConfigurationValue`

- `createDomain`
- `deleteDomain`
- `renameDomain`
- `deleteAllDomains`

- `setSequenceAwareness`
- `getSequenceAwareness`
- `setElementSequence`

- `createConfiguration`
- `deleteConfiguration`
- `renameConfiguration`
- `restoreConfiguration`
- `saveConfiguration`

- `addConfigurableElementToDomain`
- `removeConfigurableElementFromDomain`
- `split`

- `setApplicationRule`
- `getApplicationRule`
- `clearApplicationRule`

## Persistance APIs

### `exportDomainsXml`

Exports the "Domains" (aka "Settings") which is the inference engine's data.
See section [req-serializable].

### `importDomainsXml`

Imports previously-exported data into the inference engine. See [req-deserializable].

### `exportSingleDomainXml`

Exports a given part of the inference engine data. See [Serialization of individual data].

### `importSingleDomainXml`

Imports a partial inference engine data as previously exported. See section
[req-deserialization-of-individual-data].

# `ISelectionCriterionTypeInterface`

Implementation detail of the criterion requirement [req-selection-criterion].

## `addValuePair`

Implementation of [req-criterion-changes].

## `getNumericalValue`

Implementation of [req-criterion-changes].

## `getLiteralValue`

Implementation of [req-criterion-changes].

## `isTypeInclusive`

Get how the criterion possible states were specified,
see [req-state-domain-specification]

## `getFormattedState`

Pretty print criterion state [req-pretty-print]

# `ISelectionCriterionInterface`

## `setCriterionState` and `getCriterionState`

Allow Introspection of a criterion; see [req-introspection].
## `getCriterionName`

Allow Introspection of a criterion; see [req-introspection].

## `getCriterionType`

Allow Introspection of a criterion; see [req-introspection].

# `CParameterHandle`

This class implements the requirements related to direct read/write access to
parameters.

## `isRogue`

Relative to setting rogue parameters; see section [req-introspection].

## `isArray`

Allow Introspection of a parameter metadata; see [req-introspection].

## `getArrayLength`

Allow Introspection of a parameter metadata; see [req-introspection].

## `getPath` and `getKind`

Allow Introspection of a parameter identifier; see [req-introspection].

## Setters and getters

The following APIs allow reading/writing parameters.
Implements [req-introspection] and [req-parameter-overwriting]

- `setAsBoolean` 
- `getAsBoolean` 
- `setAsBooleanArray` 
- `getAsBooleanArray` 
- 
- `setAsInteger` 
- `getAsInteger` 
- `setAsIntegerArray` 
- `getAsIntegerArray` 
- `setAsSignedInteger` 
- `getAsSignedInteger` 
- `setAsSignedIntegerArray` 
- `getAsSignedIntegerArray` 
- 
- `setAsDouble` 
- `getAsDouble` 
- `setAsDoubleArray` 
- `getAsDoubleArray` 
- 
- `setAsString` 
- `getAsString` 
- `setAsStringArray` 
- `getAsStringArray` 

</article>
