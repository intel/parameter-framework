# remote-process

`remote-process` is an executable used to communicate with an instance of the
parameter-framework through a TCP socket.  It is only possible if the
parameter-framework's configuration allows it (`TuningAllowed="true"` in the
ParameterFrameworkConfiguration.xml) and if the port is also defined in the
configuration (by the `ServerPort` attribute):

For instance, if your parameter-framework configuration starts with:

    <ParameterFrameworkConfiguration SystemClassName="..." ServerPort="5000" TuningAllowed="true">

then, it listens on port 5000.

remote-process may also be used to communicate with test-platform
(see test/test-platform/README.md) thanks to test-platform being a minimal
parameter-framework client.

## Syntax

    remote-process <host> <port> <command>

You can get all available commands with the `help` command.

You may also run a batch of commands by passing them on remote-process'
standard input:

    remote-process <host> <port> < file

You should not use this feature in an interactive mode, because the
communication may timeout if you're typing your commands too slowly.
