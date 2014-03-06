# Domain coverage report

## Introduction

During the PFW's life cycle, criteria states are changed and configurations are
applied.

This report gives the coverage for :

- Criterion: detect easily which criterion state aren't used.
- Configuration: detect configuration that are never applied.
- Configuration rule: part of the rules that are useless and could be removed.
- Total average coverage

Two things are needed to generate a coverage report:

1. The Settings file
2. parameter-framework logs

## How to get the Settings file

Usually, you have this file at hand.  But if getting the Settings file is
painful, you may use remote-process to print it:

    remote-process <host> <port> getDomainsWithSettingsXML > domains.xml

## How to get PFW logs

You must get the logs from the start of the parameter-framework.

On Android, you should stop the process instantiating the parameter-framework,
clear the logs, start the process and get the logs using logcat.  E.g:

    $ adb shell stop your_process
    $ adb logcat -c
    $ adb logcat > coverage.log &
    $ adb shell start your_process
    ... do your testing ...
    kill the background logcat started above

## How to generate the report

Then execute :

    $ aplog2coverage.sh -d domains.xml -e<some_regex> coverage.log -o coverageReport.html

You may pass several log files.

The regex (-e argument) is used to match relevant log lines: If you have
several PFW instances running, you need to have a way to tell which instance is
logging each line.  Besides, if accelerates the parsing by eliminating
irrelevant log lines.

You may pass the optional `-f` argument to ignore some parse errors.

### Limitations

- Having more than one dot (".") in log paths is not supported (see the sort
  implementation (sort -rn -k2 -t.) in aplog2coverage.sh).
- A PFW start log line must be at the head of the oldest log: no
  applyconfiguration or set criterion log should be found before the PFW start.
