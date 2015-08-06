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

**???**

## `setLogger`

**???**

There is no logging requirements in the documentation; it might be a miss.

## `start` and `isStarted`

**???**

There is no "two-step-creation" requirement. This is an implementation detail.

## `applyConfigurations`

Deffered application requirement; see section **5.4.2.** See also the Selection
Criterion APIs below.

## `createParameterHandle`

Introspection and direct parameter read/write access; see section **7.2**. See
also the Parameter Handle APIs below.

## `setForceNoRemoteInterface` and `getForceNoRemoteInterface`

Tuning capability activation control; see section **8.2**.

## `setFailureOnMissingSubsystem` and `getFailureOnMissingSubsystem`

**???**

Currently only used for XML generation.

## `setFailureOnFailedSettingsLoad` and `getFailureOnFailedSettingsLoad`

**???**

Currently only used for XML generaton.

## `setSchemaFolderLocation` and `getSchemaFolderLocation`; `setValidateSchemasOnStart` and `getValidateSchemasOnStart`

**???**

Currently only used for XML generation.

# `ParameterMgrFullConnector`

This is another client interface that covers the same purposes and APIs than
`ParameterMgrPlatformConnector` and extands them. See this class' description.
Added APIs are described in this chapter.

## `setTuningMode` and `isTuningModeOn`

Tuning capability activation control; see section **8.2**. It adds another
level of flexibility over `setForceNoRemoteInterface`. The semantic of these
two APIs overlap but **neither one fully contains the other**.

**TODO**: fix the semantics.

## `setValueSpace` and `isValueSpaceRaw`; `setOutputRawFormat` and `isOutputRawFormatHex`

Serialization control (FIXME: what about deserialization ?). Seems **UNUSED**.

## `setAutoSync`, `isAutoSyncOn` and `sync`

Synchronization on client request; see section **4.4**.

## `accessParameterValue`

Parameter value direct access. **This overlaps with ParameterHandle.**

## `getParameterMapping`

Part of the Introspection requirements. See section **7**.

## Tuning APIs

All of the following APIs implement the Tuning requirement; see section **8**

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
See section **6.3**.

### `importDomainsXml`

Imports previously-exported data into the inference engine. See section
**6.3**.

### `exportSingleDomainXml`

Exports a given part of the inference engine data. See section **6.3**.

### `importSingleDomainXml`

Imports a partial inference engine data as previously exported. See section
**6.3**.

# `ISelectionCriterionTypeInterface`

Plays a part in implementing the selection criterion requirements.

## `addValuePair`

Used to implement some requirements in section **5.4**.

## `getNumericalValue`

**???**

## `getLiteralValue`

**???**

## `isTypeInclusive`

**???**

## `getFormattedState`

**???**

# `ISelectionCriterionInterface`

## `setCriterionState` and `getCriterionState`

See section **5.4** and **5.4.2**.

## `getCriterionName`

**???**

## `getCriterionType`

**???**

# `CParameterHandle`

This class implements the requirements related to direct read/write access to
parameters.

## `isRogue`

Relative to setting rogue parameters; see section **7.2**.

## `isArray`

**???**

## `getArrayLength`

## `getPath` and `getKind`

For introspection purposes; see section **7.2**.

## Setters and getters

The following APIs allow reading/writing parameters:

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
