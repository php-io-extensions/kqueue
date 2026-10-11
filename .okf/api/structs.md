---
type: API
title: Structs
description: "kevent, kevent64_s and timespec: property slots, narrow-field ranges, ext, reference-safe writes."
resource: kqueue.stub.php
tags: [kqueue, api, structs]
status: draft
generated: { by: claude-fable/5.1, at: 2026-10-11T20:34:38Z }
sources:
  - id: stub
    resource: kqueue.stub.php
    title: kqueue.stub.php
  - id: kqueue-c
    resource: kqueue.c
    title: kqueue.c
  - id: structs
    resource: tests/StructsTest.php
    title: tests/StructsTest.php
---

# Classes

```php
/** @strict-properties */ final class kevent     { public int $ident = 0; public int $filter = 0; public int $flags = 0; public int $fflags = 0; public int $data = 0; public int $udata = 0; }
/** @strict-properties */ final class kevent64_s { /* the six above */ public array $ext; }
/** @strict-properties */ final class timespec   { public int $tv_sec = 0; public int $tv_nsec = 0; }
```

Plain data holders mirroring the C structs, one property per field, in C order. No methods: `EV_SET()` fills them, `kevent()` reads and produces them, user code may set properties directly.[^stub]

# Slots

The C reads and writes properties by slot index (`OBJ_PROP_NUM`), enum `KQ_IDENT..KQ_EXT` and `KQ_TV_SEC, KQ_TV_NSEC`, matching stub declaration order. Reordering a property in the stub without reordering the enum silently corrupts every field. Reads `ZVAL_DEREF` first, so a property holding a reference is read through; an unset (uninitialised) typed property throws `Error` with the engine's own message.[^kqueue-c]

# Narrow fields

| Field | C type | Accepted PHP range |
|---|---|---|
| `filter` | `int16_t` | `-32768..65535` |
| `flags` | `uint16_t` | `-32768..65535` |
| `fflags` | `uint32_t` | `-2147483648..4294967295` |

Either signed or unsigned reading of the width is accepted, as C's implicit conversion does; wider values throw `ValueError` naming the function argument (from `EV_SET`) or the property (from `kevent`). After the cast the object holds the C value: `flags` `-1` reads back `65535`, `filter` `0xffff` reads back `-1`. `ident`, `data`, `udata`, `ext` are full 64-bit.[^kqueue-c][^structs]

# ext

`kevent64_s::$ext` must be exactly two ints at keys `0` and `1`; anything else is `ValueError` / `TypeError` naming the element. The stub cannot declare a non-empty array default, so a custom `create_object` initialises it to `[0, 0]`. Delivered events get a fresh `[ext0, ext1]`.[^kqueue-c]

# Writes

Every store goes through `ZEND_TRY_ASSIGN_LONG` / `ZEND_TRY_ASSIGN_ARR`, so a reference held on a property (`$f = &$kev->flags`) sees the new value, as a normal PHP assignment would.[^kqueue-c]

[^stub]: kqueue.stub.php
[^kqueue-c]: kqueue.c
[^structs]: tests/StructsTest.php
