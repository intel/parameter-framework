# XML Generator

This set of tools is used to transform files written in the pseudo language
(referred to as "Extended Domain Description") into XML parameter-framework
Settings files.  The extension of such files are usually `.edd` or `.pfw`.

## EDD Syntax

### Introduction

The Extended Domain Description (EDD) has been designed to help describing
multiple and complex PFW settings. It is a recursive structured language with
tabulation indentation (inspired from python).

It has several advantages :

- Easy to write
- Easy to read
- Nearly twice as compact as it's equivalent in XML
- Less merge conflicts and easy solving
- Can be split in multiple files
- Intuitive syntax and comprehension when you know the PFW concepts

But has a couple of drawbacks:

- it is not supported natively by the PFW. It needs to be compiled into XML.
- it supports only tabulation indentation

### Concepts

The EDD adds a couple of concepts over the PFW ones in order to extend the
concepts used in the Settings files.

#### DomainGroup
A domain group can contain other domain groups and domains. Those inner domains
will be prefixed with the name of the domain group.

*The tag for domain groups is `domainGroup`.*

*Example*

```
domainGroup: Codec

	domain: Flow
		conf: Unmute
			/Audio/codec/playback/master_mute = 0
		conf: Mute
			/Audio/codec/playback/master_mute = 1

	domainGroup: Configure
		RoutageState Includes Configure

		domain: IHF
		[...]
```

will create the domains :

- Codec.Flow (containing the Unmute and Mute configurations)
- Codec.Configure.IHF (The `RoutageState Includes Configure` rule will apply to
  all configurations inside the `Codec.Configure.*` domains)

#### ConfigurationGroup
A configuration group can contain other configuration groups and
configurations.   Those inner configurations will be prefixed with the name of
the configuration group.

*The tag for configuration groups is `confGroup`.*

*Example*

```
domain: ExternalDSP

	conf: TTY
		[...]

	confGroup: CsvCall
		Mode Is InCsvCall

		confGroup: Output
			conf: IHF
			[...]
			conf: Earpiece
			[...]
```

will create the following configurations in the `ExternalDSP` domain:

- TTY
- CsvCall.Output.IHF
- CsvCall.Outout.Earpiece

As with domainGroup, the `Mode Is InCsvCall` rule applies to all
`CsvCall.Output.*` configurations in the `ExternalDSP` domain.

#### ConfigurationType
A configuration type is the specialization concept. When defining a
configuration type, any configuration in the containing domain (or domain
group) with the same name will inherit the configuration type rule.

*The tag for configuration types is `confType`.*

*Example*

```
domain: ExternalDSP
	confType: Bind
		Mode Is InCsvCall

	confGroup: Modem

		conf: Bind
			BandRinging is Modem
			[...]
		conf: Unbind
			[...]
```

will create the following configurations in the `ExternalDSP` domain:

- Modem.Bind (applicable if `Mode Is InCsvCall` and `BandRinging is Modem`)
- Modem.Unbind (no rule, i.e. applicable by default)

#### Component
A component can be used to factorize parameter names in configurations.

*The tag for components is `component`.*

```
domain: Foo
	conf: Bar
		component: /System/some_element
			parameter1 = "egg"
			parameter2 = "spam"
		/System/another_element/parameter3 = 42
```

will create a domain Foo containing a configuration Bar (no rule, i.e.
applicable by default) that will set these 3 parameters:

- `/System/some_element/parameter1` to "egg"
- `/System/some_element/parameter2` to "spam"
- `/System/another_element/parameter3` to 42

## Preprocessor

The xmlGenerator uses m4 to preprocess the files before parsing them.  You may
use any macro implemented by m4, such as `define` and `include`.  This is
deprecated and we do not recommend using it.

## Style

Here are a few recommendations to follow when writing Settings using EDD:

### Rules

- if you need to modify a rule, do not hesitate to rework it globally.
- keep rule depth under 3-4.
- factorize the rules, taking 3 minute to write a Karnaugh map is worth it.
- comment, comment, comment !

### Enum Parameters

When setting an enum parameter value, use its lexical space, not its numerical
space.  E.g. don't write

	/Subsystem/frequency = 5

Write instead:

	/Subsystem/frequency = f48kHz

### String Parameters

In an EDD file, string parameters may not contain newlines.  Apart from that,
all characters are supported.  Also, leading spaces are ignored.  Do *not*
surround a string parameter's value with quotes. Don't write:

	/Subsystem/string_parameter = "some string value"

Write instead:

	/Subsystem/string_parameter = some string value

## XML Generation

Once a `.edd` file is ready to be tested, it is possible to generate the
corresponding XML file.

### domainGenerator.py

This python tool is self-documented: you may run `domainGenerator.py -h` to get
its full usage.

It prints the resulting XML on the standard output. Its syntax is:

    domainGenerator.py [-h] --toplevel-config TOPLEVEL_CONFIG_FILE
                             --criteria CRITERIA_FILE
                             [--initial-settings XML_SETTINGS_FILE]
                             [--add-domains XML_DOMAIN_FILE [XML_DOMAIN_FILE ...]]
                             [--add-edds EDD_FILE [EDD_FILE ...]]
                             [--schemas-dir SCHEMAS_DIR]
                             [--target-schemas-dir TARGET_SCHEMAS_DIR]
                             [--validate] [--verbose]

*Explanation:*

- The "top-level configuration file" is the same as the one provided by the
  parameter-framework client to instantiate it.  The plugins referenced in that
  file are not used.
- The "criteria file" lists all criteria and possible values used in the EDD
  files.
- The initial settings file is an XML file containing a single
  `<ConfigurableDomains>` (plural) tag; it may not overlap with the other
  sources below. It will be imported into the settings.
- Domain files are XML files, each containing a single `<ConfigurableDomain>`
  (singular) tag. They all will be imported in the order of the command line
  into the settings.
- EDD files are all the files in EDD syntax you want to add to your Settings.
- The optional `--schemas-dir` argument lets you change the directory
  containing the XML Schemas in the context of the XML generation only (see the
  `--validate` option).
- The optional `--target-schemas-dir` argument lets you change the directory
  containing the XML Schemas on the target device (the one the
  parameter-framework will run on) if it is using Schema validation and if
  different than the default.
- The optional `--validate` option check the validity of all XML files involved
  in the process.

*Regarding XML Domain files and EDD files:*

In theory, the order doesn't matter but since files are parsed in the order of
the command line, you'll get different (although equivalent) files if you
change the order, which makes it more difficult to compare versions.


*The "criteria file" must look something like this:*

```
ExclusiveCriterion  Criterion1Name : Criterion1Value1 Criterion1Value2
InclusiveCriterion  Criterion2Name : Criterion2Value1 Criterion2Value2
```

I.e. One criterion by line, starting by its kind, then its name, followed by a
semicolon and then all possible values separated by spaces.

### hostDomainGenerator.sh

**This script is now deprecated and replaced by domainGenerator.py
(see above).**

It prints the resulting XML on the standard output. Its syntax is:

    hostDomainGenerator.sh [--validate] <top-level configuration file> <criteria file> <EDD files...>

See domainGenerator.py above for the explanation of the arguments.


#### How it works
TODO
