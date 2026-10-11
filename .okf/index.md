---
okf_version: "0.2"
---

# ext-kqueue

* [Module](architecture/module.md) - Scope, file layout, module globals, the three struct classes, optional sockets support, errno and argument conventions.

# API

* [Functions](api/functions.md) - kqueue, kevent/kevent64 call shape and result, EV_SET/EV_SET64 ident forms, the kevent-vs-kevent64 queue rule, timeouts, EV_RECEIPT.
* [Structs](api/structs.md) - kevent, kevent64_s and timespec: property slots, narrow-field ranges, ext, reference-safe writes.
* [Constants](api/constants.md) - The <sys/event.h> set by group, errno constants, deprecated NOTE_ values.

# Runbooks

* [Build, install, test](runbooks/build.md) - gen_stub after a stub edit, scratch builds per PHP, the Mac installer, Pest on php84 and zhp, CI matrix, PIE.
