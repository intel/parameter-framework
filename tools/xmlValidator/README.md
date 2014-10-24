# xmlValidator tool

This tool can be used to check if the `.xml` files you have written are
following the `.xsd` schemas provided by the PFW.  By doing so, you are *ensured*
that your configuration is *fully compatible* with the `parameter-framework`.

It scans all directories and subdirectories for `.xml` files and checks them
with `.xsd` from a *schemas* directory you specified for the script.

## Usage

To run xmlValidator, just start it from the commandline with:

    python xmlValidator.py <xmlRootDirectory> <xsdDirectory>

where:

* `<xmlRootDirectory>` is a path to a directory containing:
    - `.xml` files
    - subdirectories containing `.xml` files
* `<xsdDirectory>` is a path to a directory containing:
    - `.xsd` files (also called *schemas*)

## Example of usage

### File structure

In the example, we have the following files:

    |-- ParameterFrameworkConfiguration.xml
    |-- Schemas
    |   |-- ComponentLibrary.xsd
    |   |-- ComponentTypeSet.xsd
    |   |-- ConfigurableDomains.xsd
    |   |-- FileIncluder.xsd
    |   |-- ParameterFrameworkConfiguration.xsd
    |   |-- ParameterSettings.xsd
    |   |-- Parameter.xsd
    |   |-- Subsystem.xsd
    |   `-- SystemClass.xsd
    |-- Settings
    |   `-- FS
    |       `-- Genres.xml
    `--- Structure
         `-- FS
            |-- MusicLibraries.xml
            `-- my_music.xml

### Command
We are in the directory which contains the structure detailed previously.
To check the validity, we just run:

    ../../tools/xmlValidator/xmlValidator.py . Schemas

### Results
And we will get the following output on the commandline:

    [*] Validate xml files in /home/lab/MusicLibrary/ with /home/lab/MusicLibrary/Schemas
    Attempt to validate ParameterFrameworkConfiguration.xml with ParameterFrameworkConfiguration.xsd
    ParameterFrameworkConfiguration.xml is valid
    Attempt to validate my_music.xml with Subsystem.xsd
    my_music.xml is valid
    Attempt to validate MusicLibraries.xml with SystemClass.xsd
    MusicLibraries.xml is valid
    Attempt to validate Genres.xml with ConfigurableDomains.xsd
    Genres.xml is valid


## Install requirements

In order to use this tool, you must have the following packages (these
are the names on a debian-based distribution):

* `python` (2.7 or later)
* `python-lxml`
* `libpython2.7` or later
