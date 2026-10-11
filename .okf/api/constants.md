---
type: API
title: Constants
description: The <sys/event.h> set by group, errno constants, deprecated NOTE_ values.
resource: kqueue.stub.php
tags: [kqueue, constants]
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
    resource: tests/ConstantsTest.php
    title: tests/ConstantsTest.php
---

# Rule

Every constant is `@cvalue` from `<sys/event.h>` (or `<errno.h>` for the `E*` group) at build time, never a literal, declared unconditionally: every name exists in the macOS SDK the extension requires.[^stub]

# Groups

95 constants: 14 `EVFILT_*`, 3 `KEVENT_FLAG_*`, 18 `EV_*`, 50 `NOTE_*` (user, read/write, vnode, proc, vm, timer, track), 10 `E*` errno. The README's API section lists each group.[^stub]

# Deprecated SDK values

`NOTE_REAP` and `NOTE_EXIT_REPARENTED` are marked deprecated in the SDK header. They are still part of `<sys/event.h>`, so they are registered like every other constant; `kqueue.c` wraps the arginfo include in `#pragma GCC diagnostic ignored "-Wdeprecated-declarations"` so the build stays warning-free.[^kqueue-c]

# Errno

`EACCES EBADF EFAULT EINTR EINVAL EMFILE ENFILE ENOENT ENOMEM ESRCH`: the values `kqueue()`/`kevent()` can set and `EV_ERROR` events carry in `data`. Registered here because no other extension in a plain CLI PHP provides them under these names (ext/sockets uses `SOCKET_E*`; ext-posi defines none). A PHP that also loads another extension registering the same names would conflict at MINIT; none in the php-io-extensions family does.[^stub]

[^stub]: kqueue.stub.php
[^kqueue-c]: kqueue.c
[^tests]: tests/ConstantsTest.php
