# php-io-extensions/kqueue

1:1 PHP bindings of macOS `<sys/event.h>`, in plain C. Global functions, same names, same argument order as C. No wrapper package, no policy.

Platform: macOS (Darwin). Binds `kevent64()`, which only macOS has. PHP ≥ 8.4, 64-bit, NTS + ZTS.

## Install

PIE:

```sh
pie install php-io-extensions/kqueue
```

Source checkout, Homebrew `php@8.4` + `php@8.4-zts` (or pass PHP binaries as args):

```sh
./install-macos.sh
./install-macos.sh /path/to/bin/php
```

Script builds in temp copy, installs `kqueue.so` into each PHP's `extension_dir`, ad-hoc signs it, writes `30-kqueue.ini` into its conf.d.

Manual:

```sh
phpize && ./configure --enable-kqueue && make && make install
```

## API

```php
function kqueue(): int;
function kevent(int $kq, array $changelist, int $nchanges, ?array &$eventlist, int $nevents, ?timespec $timeout): int;
function kevent64(int $kq, array $changelist, int $nchanges, ?array &$eventlist, int $nevents, int $flags, ?timespec $timeout): int;
function EV_SET(kevent $kevp, mixed $ident, int $filter, int $flags, int $fflags, int $data, int $udata): void;
function EV_SET64(kevent64_s $kevp, mixed $ident, int $filter, int $flags, int $fflags, int $data, int $udata, int $ext0, int $ext1): void;
function kqueue_errno(): int;

final class kevent     { int $ident, $filter, $flags, $fflags, $data, $udata; }
final class kevent64_s { int $ident, $filter, $flags, $fflags, $data, $udata; array $ext = [0, 0]; }
final class timespec   { int $tv_sec, $tv_nsec; }
```

Constants: all 87 `EVFILT_*`, `EV_*`, `NOTE_*`, `KEVENT_FLAG_*` from the SDK header, values taken at compile time, plus errno values kqueue(2) documents: `EACCES EBADF EFAULT EINTR EINVAL EMFILE ENFILE ENOENT ENOMEM ESRCH`.

```php
$kq = kqueue();

$timer = new kevent();
EV_SET($timer, 1, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, 250, 0);

$wait = new timespec();
$wait->tv_sec = 1;

$n = kevent($kq, [$timer], 1, $events, 8, $wait);   // 1, $events[0] is a kevent
```

## Mapping to C

- **Structs.** `struct kevent`, `struct kevent64_s`, `struct timespec` → final classes, typed public props named as C fields, zero-initialised, no dynamic props. Class and function share name `kevent` as in C; PHP keeps them in separate symbol tables.
- **Arrays in, arrays out.** `changelist` = PHP array of `kevent` (`kevent64_s` for `kevent64`); first `$nchanges` entries in iteration order are sent. `$eventlist` receives list of new `kevent`/`kevent64_s`, length = return value. On `-1`, `$eventlist` untouched, as C leaves its buffer.
- **`timeout`.** `null` = C `NULL` = block. `new timespec()` = poll.
- **Widths.** `filter` (`int16_t`), `flags` (`uint16_t`), `fflags` and `kevent64` `flags` (`uint32_t`) accept either signed or unsigned reading of their width, as C's implicit conversion does. Wider values throw `ValueError`. `EV_SET`/`EV_SET64` store the value as the C field holds it: `EV_SET($k, 0, 0xffff, -1, NOTE_PCTRLMASK, 0, 0)` → `filter -1`, `flags 65535`, `fflags 0xfff00000`. Constants carry C expression values: `NOTE_PCTRLMASK` = `~0x000fffff` = `-1048576`.
- **ident.** `EV_SET`/`EV_SET64` take `int`, a stream resource, or a `Socket` (when ext-sockets is loaded). Stream and `Socket` resolve to their fd, same as `epoll_ctl`'s `$fd`; the struct stores the int. Non-fd filters (`EVFILT_SIGNAL`, `EVFILT_PROC`, `EVFILT_TIMER`, `EVFILT_USER`) pass their int ident as before. Other types throw `TypeError`; a closed stream throws `TypeError`, a stream with no fd `ValueError`.
- **64-bit fields.** `ident`, `data`, `udata`, `ext[]` pass through as bit patterns. `udata` is pointer-sized int: kernel returns exactly what you set, nothing more. Unsigned 64-bit values ≥ 2^63 read back negative.
- **errno.** PHP runtime can overwrite `errno` before PHP code runs, so each binding saves it right after the syscall on `-1`. `kqueue_errno()` reads it. Reset per request.
- **Descriptor lifetime.** `close(2)` is POSIX, not kqueue: use `Posi\System::close()` from `php-io-extensions/posi`.
- **Invalid arguments** (`nchanges` outside `changelist`, negative `nevents`, wrong element class, unset prop, malformed `ext`) throw `ValueError`/`TypeError`/`Error` before any syscall.

Kernel behaviour passes through unchanged:

- A change that fails while `$nevents > 0` comes back as an event with `EV_ERROR` in `flags` and errno in `data`; with `$nevents = 0` the call returns `-1` and `kqueue_errno()` holds errno. `EV_RECEIPT` forces the event form.
- XNU binds a kqueue to the first ABI it is used with. `kevent64()` on a kqueue already used with `kevent()` (or reverse) returns `-1`, `EINVAL`.
- Signals interrupt a blocking `kevent()`: `-1`, `EINTR`.

## Tests

Pest v4. Needs extension loaded:

```sh
composer install
php -d extension=/path/to/kqueue.so vendor/bin/pest   # or plain php once installed
```

## Regenerating arginfo

`kqueue_arginfo.h` is generated from `kqueue.stub.php`; edit stub, then:

```sh
php "$(php-config --prefix)/lib/php/build/gen_stub.php" kqueue.stub.php
```

## License

MIT
