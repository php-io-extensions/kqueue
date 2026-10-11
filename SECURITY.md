# Security Policy

## Supported versions

ext-kqueue is pre-1.0. No 0.x release receives security fixes or advisories; fixes land in the
next release line. Security support starts with 1.0.

| Version | Security fixes |
|---------|----------------|
| < 1.0   | No             |

## Reporting a vulnerability

Please don't open a public issue for a security problem.

Report it privately through GitHub: the **Report a vulnerability** button on the
[php-io-extensions/kqueue](https://github.com/php-io-extensions/kqueue) repository's **Security** tab.
If that isn't available, email **info@projectsaturnstudios.com**.

Include what you found, the affected version, your macOS version and PHP build (NTS or ZTS), and
steps to reproduce. Reports are read and weighed for the release line in development; before 1.0
there is no response-time commitment.

## Security model

ext-kqueue binds `kqueue(2)` one to one. Each function performs the syscall with the privileges of
the PHP process, the same as C code linked into that process. The syscalls only observe events and
never read, write or close a descriptor, so the extension cannot leak data from an fd it is handed;
it can only tell the caller that something happened on it.

- **Idents.** `EV_SET()` accepts any integer as `$ident` and passes it to the kernel. Watching a
  descriptor, pid or signal the script did not create is allowed wherever the kernel allows it; the
  extension adds no check. Streams and `Socket` objects are resolved to their descriptor through
  the engine, and a closed one is rejected with `TypeError` or `ValueError` before the syscall.
- **Memory.** The only allocations are the change and event buffers of `kevent()` and `kevent64()`,
  sized from `$nchanges` and `$nevents` through `safe_emalloc()` after both are checked against
  `0..INT_MAX` and `$nchanges` against the array it indexes, and freed before the call returns.
  Struct fields are read by property slot and range-checked against their C width before the copy.
- **Timers and signals.** `EVFILT_TIMER` and `EVFILT_SIGNAL` registrations are ordinary kernel
  state owned by the kqueue; a signal interrupting a wait returns `-1` / `EINTR` and nothing else.
- **Arguments.** Every `int` argument is checked against the C type it lands in. Out-of-range
  values throw `ValueError`; nothing is truncated into a different value.

A report is in scope when ext-kqueue itself reads or writes memory it should not: a buffer size
that can be made to disagree with `$nchanges` or `$nevents`, a struct field copied without its range
check, a crash reachable from well-formed PHP arguments, a descriptor resolved from a stream or
`Socket` after it was freed. Effects that follow from a valid syscall the script asked for, such as
watching a process it was handed the pid of, are the documented behaviour.
