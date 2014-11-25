# parameter-framework configuration file XML Schemas

These are W3C Schemas for the various configuration files.

`xmllint` may be used to check for correctness, e.g:

    xmllint --xinclude --noout --schema ParameterFrameworkConfiguration.xsd /path/to/your/ParameterFrameworkConfiguration.xml

See `tools/xmlValidator` for a custom alternative tool.

Only `ParameterFrameworkConfiguration.xsd`, `SystemClass.xsd`, `Subsystem.xsd` and
`ConfigurableDomains.xsd` are relevant for use with xmllint: the others are
included by these 4 XSDs.

**You may refer to samples at
<https://github.com/01org/parameter-framework-samples>.**

## ParameterFrameworkConfiguration.xsd

Schema for the **top-level configuration**.  It contains:

- A reference to the `SystemClass` (aka StructureDescription) XML file (see
  below);
- The list of plugins (libraries) to be used. They may be split according to
the folder they reside in. The `Folder` attribute can either be:
    
    - an absolute path,
    - a relative path (relative to the execution directory),
    - empty.
    
    In the first two cases, the runtime loader will be asked to explicitely load
    the libraries found in the specified folder; in the last case (empty string)
    the runtime loader will search for the library on its own (e.g. on Linux
    distribution this is usually `/lib`, `/usr/lib` - see `man ld.so`)
- Optionally, a reference to the `Settings`.

Attributes of `ParameterFrameworkConfiguration` are:

- The `SystemClass` name (for consistency check)
- `TuningAllowed` (whether the parameter-framework listens for commands)
- The `ServerPort` on which the parameter-framework listens if
  `TuningAllowed=true`.

## SystemClass.xsd

Schema for the **SystemClass associated with the top-level configuration**.  It
points to all the "Subsystem" files (see below).

The `Name` attribute of the SystemClass must match the `SystemClass` attribute
of the top-level configuration file. This name will be the first component of
all parameters in it, i.e. if its name is "FooBar", its path is `/FooBar`. We
will use this name in examples below.

## Subsystem.xsd

Schema for all **Subsystem files** (aka Structure files).  These files describe the
content and structure of the system to be managed by the parameter-framework
and also indicate which plugin is to be used.

A Subsystem has the following attribute:

- `Name` (self-explanatory); again it is the base component of all parameters
  inside it; i.e. if its name is "Spam", its path is `/FooBar/Spam`;
- `Type`, which indicates which SubsystemBuilder is to be used (each plugin can
  declare one or more SubsystemBuilders); it may be defined as `Virtual`, in
  which case, no plugin will be used and the parameters won't be synchronized.
  This is useful for debugging but may also be used for the parameter-framework
  to act as a configurable settings database;
- `Endianness`: `Little` or `Big`;
- `Mapping` (optional), defines a Mapping to be inherited by all Components in
  the Subsystem.

A Subsystem *must* contain:

- A `ComponentLibrary`, which may include (using `<xi:include href="xyz.xml"/>`)
  other files containing a `<ComponentLibrary>` or a `<ComponentTypeSet>` tag.
- An `InstanceDefinition` which instantiates the parameters and may use
  ComponentTypes defined in the ComponentLibrary.

## ConfigurableDomains.xsd

Schema for the ConfigurableDomains (aka Settings files).  These files contain
the rules for applying values to parameters.

Writing this file by hand is painful but it is not intended to be dealt
with directly: instead, you may use the command-line interface (see
`remote-process/README.md`) to set the settings and export the resulting
Settings with the `getDomainsWithSettingsXML` command.
