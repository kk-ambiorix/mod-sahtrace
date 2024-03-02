# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]


## Release v1.1.0 - 2023-10-13(09:32:32 +0000)

### New

- Implement a global Logging policy.

## Release v1.0.7 - 2023-10-06(09:48:27 +0000)

### Changes

- Only save trace-zones

## Release v1.0.6 - 2023-03-15(10:45:47 +0000)

### Fixes

- No traces in syslog when plug-in is started deamonized from ssh session

## Release v1.0.5 - 2022-09-23(11:16:10 +0000)

### Fixes

- [SAHTrace] Use new syntax to prevent ubus config overwrite

## Release v1.0.4 - 2022-08-16(10:36:44 +0000)

### Other

- amx/mod_sahtrace does not select dynamically stderr or syslog

## Release v1.0.3 - 2022-07-15(12:27:36 +0000)

### Fixes

- Valgrind complains about uninitialized values

## Release v1.0.2 - 2022-06-28(06:39:53 +0000)

### Fixes

- Should not pass NULL pointer to stat function

## Release v1.0.1 - 2022-06-09(06:16:14 +0000)

### Other

- Add functions to make the tracelevel and zones runtime configurable

## Release v1.0.0 - 2022-06-03(13:29:11 +0000)

### Other

- Add functions to make the tracelevel and zones runtime configurable

## Release v0.0.11 - 2022-05-23(07:41:01 +0000)

### Fixes

- [Gitlab CI][Unit tests][valgrind] Pipeline doesn't stop when memory leaks are detected

## Release v0.0.10 - 2022-05-04(12:44:53 +0000)

### Fixes

- Doc generation for mod-sahtrace not working

### Other

- Move component from ambiorix to core on gitlab.com

## Release v0.0.9 - 2021-08-03(15:03:43 +0000)

### Changes

- Use global sahtrace level for zones if no zone level is defined

## Release v0.0.8 - 2021-06-25(07:50:32 +0000)

### Fixes

- [tr181 plugins][makefile] Dangerous clean target for all tr181 components
- Use dyncast to interprete the trace lvl for trace zones

## Release v0.0.7 - 2021-05-04(09:10:07 +0000)

### Fixes

- When plug-in names changes mod_sahtrace causes a segfault

### Changes

- Add FAKEROOT variable to packages makefile
- Prepare for opensource

### Other

- Enable auto opensourcing

## Release 0.0.6 - 2021-04-13(09:21:40 +0000)

### Changed

- The version suffix should be removed from the shared object [change]

## Release 0.0.5 - 2021-03-11(16:19:04 +0000)

### Changed

- Missing quote in mod-sahtrace baf.yml

## Release 0.0.4 - 2021-03-08(08:14:32 +0000)

### Changes

- After introducing BAF mod_sahtrace.odl is no longer installed
- Not handled reasons in an entry point function should not cause a failure
- Update test

## Release 0.0.3 - 2020-11-30(16:57:17 +0000)

### Changes

- Update makefile
- Adds license and copyright to source file

### Fixes

- Linking is not done correctly when building the package

## Release 0.0.2 - 2020-10-27(12:15:49 +0000)

- Correct install & release paths

## Release 0.0.1 - 2020-10-23(05:30:31 +0000)

### Changes

- Extends tests
- Updates readme

### Fixes

- Level of zones can be set
- trace-zones can be used

## Release 0.0.0 - 2020-10-23(05:28:19 +0000)

- Initial version
