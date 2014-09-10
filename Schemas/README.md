# parameter-framework configuration file XML Schemas

These are W3C Schemas for the various configuration files.

`xmllint` may be used to check for correctness, e.g:

    xmllint --xinclude --noout --schema ParameterFrameworkConfiguration.xsd /path/to/your/ParameterFrameworkConfiguration.xml

See `tools/xmlValidator` for a custom alternative tool.

Only `ParameterFrameworkConfiguration.xsd`, `SystemClass.xsd`, `Subsystem.xsd` and
`ConfigurableDomains.xsd` are relevant for use with xmllint: the others are
included by these 4 XSDs.

You may refer to samples at
<https://github.com/01org/parameter-framework-samples>.

## ParameterFrameworkConfiguration.xsd

Schema for the top-level configuration.  It contains:

- A reference to the `SystemClass` (aka StructureDescription) XML file (see
  below);
- The list of plugins to be used;
- Optionally, a reference to the `Settings`.

Attributes of `ParameterFrameworkConfiguration` are:

- The `SystemClass` name (for consistency check)
- The `TuningMode` (whether the parameter-framework listens for commands)
- The `ServerPort` on which the parameter-framework listens if
  `TuningMode=true`.

## SystemClass.xsd

Schema for the SystemClass associated with the top-level configuration.  It
points to all the "Subsystem" files (see below).

## Subsystem.xsd

Schema for all Subsystem files (aka Structure files).  These files describe the
content and structure of the system to be managed by the parameter-framework
and also indicate which plugin is to be used.

## ConfigurableDomains.xsd

Schema for the ConfigurableDomains (aka Settings files).  These files contain
the rules for applying values to parameters.
