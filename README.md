# Module SAHTRACE

## Using mod_sahtrace

In the main odl file include `mod_sahtrace.odl`, preferable as an optional include.

```odl
#include "mod_sahtrace.odl";
```

mod_sahtrace uses the following global options:

- `default_log_level`: default trace level used when no level is specified
- `default_trace_zone_level`: default trace zone level used when no level is specified

## Configuration

### config options

Optionally sahtrace config options can be provided in the `%config` section of the main odl file.

The following config options are supported:

- `sahtrace`: a table that can contain

  - `type`: must be a string and can be set to `syslog`, `stdout`, `stderr`, `file`, `auto`
  - `level`: an integer, set the default trace level
  - `file`: only when `type` is set to `file`, and must contain the file name to which the trace output is written.

  When type is set to `auto` log messages will be printed to `sterr` when the application is started in foreground. When the application is daemonized (amxrt otion `-D`), the messages will be written to the system log.

- `trace-zones`: an array of trace zones that must be activated

#### Example

```odl
%config {
    sahtrace = {
        type = "syslog",
        level = 500
    };

    trace-zones = ["imtp", "usp"];
}
```

### Alternative config options

The configuration options can be set in an alternative way as well. This is usefull when using the `amxrt` command line option `-o`.

- `log-type`: must be a string and can be set to `syslog`, `stdout`, `stderr`, `file`, `auto`.
- `log-level`: an integer, set the default trace level
- `log-file`: only when `log-type` is set to `file`, and must contain the file name to which the trace output is written.
- `log-zones`: an array of trace zones that must be activated

#### Example

```odl
%config {
    log-type = "syslog";
    log-level = 500;
    log-zones = ["imtp", "usp"];
}
```

### Defaults

When no configuration options are provided the default settings are:

- `log-type` = "syslog"
- `log-level` = `default_log_level`
- `log-zones` = [];

### Trace Zones

Trace zones can be defined as a table with key - value pairs. The key must contain the name of the zone and the value is the trace level for the zone.

#### Example

```odl
%config {
    sahtrace = {
        type = "syslog",
        level = 500
    };

    trace-zones = {imtp = 500, usp = 200};
}
```

### set_trace_zone

Trace zones can be dynamically added and changed during runtime for a specific root object. The zone parameter is obligatory and the level parameter is optional with a default level defined by `default_trace_zone_level`.

#### Example

Using `ubus-cli` or `pcb-cli` (other flavours are available as well and depends on the bus system you are using).

```
RootObject.set_trace_zone(zone = "zone1", level = 300)
```
### list_trace_zones

Lists all trace zones set at that moment. Not only those set by set_trace_zone()

#### Example

Using `ubus-cli` or `pcb-cli` (other flavours are available as well and depends on the bus system you are using).

```
RootObject.list_trace_zones()
```