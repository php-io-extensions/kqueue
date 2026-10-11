# Agent guidance — php-io-extensions/kqueue

1. **Read [`.okf/index.md`](.okf/index.md) first** before changing the API, the C, or packaging. Open only the concepts the change touches.
2. **kqueue(2), 1:1.** One PHP function = one syscall or macro, named as `<sys/event.h>` names it. No helpers that aren't in the header, no event loop, no wrapper class beyond the three structs, no throwing on a failed call: failure is C's `-1` with errno kept for `kqueue_errno()`. See [`.okf/architecture/module.md`](.okf/architecture/module.md).
3. **macOS only, 64-bit only.** `kevent64()` is Darwin's; `kevent64_s` needs a 64-bit `zend_long`. epoll and POSIX calls live in ext-epoll and ext-posi.
4. **Plain C on the Zend API.** No Zephir, no FFI. Sources: `kqueue.c`, `php_kqueue.h`.
5. **The stub is the declaration.** Edit `kqueue.stub.php`, regenerate `kqueue_arginfo.h` with gen_stub, commit both. Never hand-edit arginfo. Property order in the three classes must match the `KQ_*` slot enums in `kqueue.c`. See [`.okf/api/structs.md`](.okf/api/structs.md).
6. **Constants come from the header** via `@cvalue`, never a literal; deprecated SDK names stay registered under the diagnostic pragma. See [`.okf/api/constants.md`](.okf/api/constants.md).
7. **Arguments are checked, never truncated.** Narrow struct fields accept the signed or unsigned reading of their C width and nothing wider; counts are checked against the arrays they index; out of range → `ValueError` naming the argument or property. See [`.okf/api/functions.md`](.okf/api/functions.md).
8. **Never link ext/sockets.** `Socket` is resolved by class name at call time behind `KQ_HAVE_SOCKETS`.
9. **Property writes go through `ZEND_TRY_ASSIGN_*`** so references held on a property are assigned through.
10. **NTS and ZTS.** State lives only in module globals. Every change builds and passes Pest on `php84` and `zhp` (Homebrew NTS and ZTS) and in CI; a module built with one PHP's `phpize` does not load in the other.
11. **Tests are Pest v4 and hardware-free**: socket pairs, timers, user events, `/dev/fd`, a child signal. Expected values come from the constants.
12. **Durable facts go in `.okf`.** Update the matching concept, bump its `generated.at`, append `.okf/log.md`. The bundle documents the package, never a session.
13. **Version**: `PHP_KQUEUE_VERSION` moves only past a published tag; tags drive Packagist.
