# test-platform

Test-platform is a little executable that can instantiate a parameter-framework
with custom criteria and full control over criteria states.  It also serves
as an example of a parameter-framework client.

It offers the possibility to :

- Give the parameter-framework configuration xml
- Create criteria from a name, a type (exclusive or inclusive), states (the
  state number or a state name list)
- Start a parameter-framework instance
- Change criteria states and apply configurations

## Usage

Start it with:

    test-platform [-d] </path/to/ParameterFrameworkConfiguration.xml> [port, defaults to 5001]

(The optional `-d` option daemonizes test-platform).

Then, you may send commands to the test-platform using remote-process; e.g:

    remote-process localhost 5001 help
    remote-process localhost 5001 start

## Known issues

- The path to the configuration file must contain at least a `/`.  Thus, if you
want to refer to a file in the current directory, you should write:

    test-platform ./ParameterFrameworkConfiguration.xml
