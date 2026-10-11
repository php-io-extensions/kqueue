---
type: API
title: Functions
description: kqueue, kevent/kevent64 call shape and result, EV_SET/EV_SET64 ident forms, the kevent-vs-kevent64 queue rule, timeouts, EV_RECEIPT.
resource: kqueue.stub.php
tags: [kqueue, api, functions]
status: draft
generated: { by: claude-fable/5.1, at: 2026-10-11T20:34:38Z }
sources:
  - id: stub
    resource: kqueue.stub.php
    title: kqueue.stub.php
  - id: kqueue-c
    resource: kqueue.c
    title: kqueue.c
  - id: tests
    resource: tests/KeventTest.php
    title: tests/KeventTest.php
  - id: tests64
    resource: tests/Kevent64Test.php
    title: tests/Kevent64Test.php
  - id: evset
    resource: tests/EvSetTest.php
    title: tests/EvSetTest.php
---

# Create

```php
function kqueue(): int {}
```

Returns the kqueue fd or `-1`. A raw `int`; core PHP cannot close it (use `posix_close()` from ext-posi or let process exit reclaim it). kqueue descriptors are not inherited across `fork`.[^kqueue-c]

# kevent / kevent64

```php
function kevent(int $kq, array $changelist, int $nchanges, ?array &$eventlist, int $nevents, ?timespec $timeout): int {}
function kevent64(int $kq, array $changelist, int $nchanges, ?array &$eventlist, int $nevents, int $flags, ?timespec $timeout): int {}
```

One call registers and waits. The first `$nchanges` entries of `$changelist` (objects of `kevent`, or `kevent64_s` for `kevent64`) are copied into a C array sized with `safe_emalloc`; extra entries are ignored, fewer is a `ValueError`. Then the kernel waits up to `$timeout` for at most `$nevents` events into a second buffer, and the result is assigned by reference into `$eventlist` as a list of fresh objects. Both buffers are freed before return.[^kqueue-c][^tests]

Return: the count, `0` on timeout (with `$eventlist` set to `[]`), `-1` on error with `$eventlist` untouched and errno in `kqueue_errno()`.[^tests]

`$nevents` of `0`: apply changes, return immediately; `$eventlist` becomes `[]`. `$timeout` `null`: block; a `timespec` of `0/0`: poll. `kevent64()` with `KEVENT_FLAG_IMMEDIATE` returns at once even with a `null` timeout.[^tests][^tests64]

Queue rule: a kqueue that has been through `kevent()` answers `kevent64()` with `EINVAL`, and the reverse. The kernel enforces it; the extension passes it through. Pick one call per queue.[^tests64]

Error reporting on a change: without `EV_RECEIPT`, a failed change makes the whole call return `-1` (`ENOENT` for deleting an unregistered ident). With `EV_RECEIPT` on the change, the kernel instead delivers an event with `EV_ERROR` in `flags` and the errno in `data`, `udata` preserved, and the call returns the count.[^tests]

Signals interrupt a blocking wait with `-1` / `EINTR`.[^tests]

# EV_SET / EV_SET64

```php
/** @param resource|Socket|int $ident */
function EV_SET(kevent $kevp, mixed $ident, int $filter, int $flags, int $fflags, int $data, int $udata): void {}
function EV_SET64(kevent64_s $kevp, mixed $ident, int $filter, int $flags, int $fflags, int $data, int $udata, int $ext0, int $ext1): void {}
```

Fills the object in place through the C macro, so the stored values are what the struct holds (narrow fields as their C type). `$ident` forms: a raw `int` (fd, pid, signal number, timer or user id), a stream resource backed by a descriptor, or an ext/sockets `Socket`; the last two resolve to the descriptor number. Delivered events report the number, never the resource. Writes go through `ZEND_TRY_ASSIGN_*`, so a reference held on a property is assigned through.[^kqueue-c][^evset]

`$udata` is the tag: all 64 bits round-trip (`PHP_INT_MIN` survives) and come back in the delivered event.[^tests]

# Errors

```php
function kqueue_errno(): int {}
```

Last errno saved by a failed `kqueue()`, `kevent()` or `kevent64()` in this request (RINIT resets it). Compare against the extension's own `E*` constants.[^kqueue-c]

# Triggering

Level-triggered by default: a read filter fires on every wait while bytes remain (`data` = bytes available). `EV_CLEAR` makes it edge-like, resetting state after delivery; `EV_ONESHOT` deletes the registration after one delivery; `EV_DISPATCH` disables it after one delivery until `EV_ENABLE`. `EV_EOF` is set in `flags` when the peer closes.[^tests]

[^stub]: kqueue.stub.php
[^kqueue-c]: kqueue.c
[^tests]: tests/KeventTest.php
[^tests64]: tests/Kevent64Test.php
[^evset]: tests/EvSetTest.php
