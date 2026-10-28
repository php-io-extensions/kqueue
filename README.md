<div align="center" id="header">

<h1>ext-kqueue</h1>

<h3>macOS <code>kqueue(2)</code> bindings for PHP 8.4+</h3>


[![CI](https://img.shields.io/github/actions/workflow/status/php-io-extensions/kqueue/ci.yml?style=flat-square&label=CI)](https://github.com/php-io-extensions/kqueue/actions/workflows/ci.yml)
[![Packagist](https://img.shields.io/packagist/v/php-io-extensions/kqueue?style=flat-square)](https://packagist.org/packages/php-io-extensions/kqueue)
[![PHP](https://img.shields.io/packagist/dependency-v/php-io-extensions/kqueue/php?style=flat-square&label=PHP)](https://packagist.org/packages/php-io-extensions/kqueue)
[![License](https://img.shields.io/github/license/php-io-extensions/kqueue?style=flat-square)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-macos_only-blue?style=flat-square&logo=apple&logoColor=white)](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/kqueue.2.html)
[![OKF](https://img.shields.io/badge/OKF-0.2-6f42c1?style=flat-square&label=knowledge%20bundle)](.okf/index.md)
[![PIE](https://img.shields.io/badge/PIE-installable-4f5b93?style=flat-square&logo=php&logoColor=white)](https://github.com/php/pie)

</div>

A program that talks to many connections at once needs to answer one question over and over: "<b>which of my file descriptors can I read or write right now without blocking?</b>" PHP's built-in answer is `stream_select()`. It works, and it's the right answer while you only watch a handful of streams. Every call you hand it the full list, the kernel copies the list in, scans every one, copies the result out. The more fds you watch, the more expensive that becomes.

With 10 connections nothing is taxed. With 10,000, each wakeup scans 10,000 fds to find the 3 that have data. There's also a hard cap, `FD_SETSIZE`, usually 1024, baked into the C library that PHP inherits. So out of the box, resource waiting doesn't scale.

<b>kqueue</b> solves this. You register each thing you care about once with the macOS kernel into a "kqueue", which also happens to be an fd. Then you wait on it. This is the cool part: the kernel hands back only the entries with activity, each one labelled with the tag you registered it under. PHP never sorts through what's ready; it iterates through the ready ones, reads them, and goes again.

And "each thing" is more than a socket. One kqueue watches fds, timers, signals, a child process exiting, a file changing on disk, and events your own code triggers, through the same call. A loop that would need `stream_select()`, `pcntl`, `sleep()` and a polled `stat()` on Linux needs one `kevent()` here.

<b>The only problem?</b> PHP never exposes kqueue to userland. PECL has `ev`, `event` and `uv`, which pick kqueue on macOS and hide it behind a portable event loop. Every one of them gives you a loop. This extension exposes kqueue directly giving you access to the system calls, one to one, no opinionated event loop: allowing you the freedom to build the loop you need, or watch resources those loop implementations never thought of.

## Basic usage

Most ways you will interface with kqueue are in two parts -

Start a kqueue with `kqueue()`, connect to resources and describe each one in a `kevent` struct with `EV_SET()`, and collect them in `$changes`.

```php

// Create a kqueue with the kernel and get back its fd.
$kq = kqueue();

$resources = [];
$resource_tags = ['resourceA', 'resourceB', 'resourceC'];
$changes = [];

foreach ($resource_tags as $tag => $name)
{
    // Open another process on the device
    [$process, $pipes] = $this->startNewProcess();
    // Save it to your list of $resources
    $resources[$tag] = ['name' => $name, 'process' => $process, 'stdin' => $pipes[0], 'stdout' => $pipes[1]];

    // Describe what to watch: this pipe, for readability, tagged with $tag
    EV_SET($changes[] = new kevent(), $pipes[1], EVFILT_READ, EV_ADD, 0, 0, $tag);
    $this->writeToResource($pipes[0]);
}

```

Ping kqueue for activity with `kevent()` and do work. The same call hands the kernel whatever is in `$changes` before it waits, so the registrations from the first part go in on the first pass and the list is emptied. To stop watching a resource, close it: a closed descriptor drops out of the kqueue on its own.

```php
while (!empty($resources))
{
    kevent(kq: $kq, changelist: $changes, nchanges: count($changes), eventlist: $activity, nevents: 16, timeout: null);
    $changes = [];

    foreach ($activity as $event)
    {
        $resource = $resources[$event->udata];

        // Read the data out of the resource
        $line = fgets($resource[stdout]);
        $gone = ($line === false || $event->flags & EV_EOF);

        if (!$gone) {
            $message = json_decode($line, true);
            echo "{$resource[name]}: ", $message[method] ?? "reply to #{$message[id]}", "\n";
        }
        else
        {
            // There was nothing to read; closing the process closes the pipe, and the kqueue forgets it
            proc_close($resource[process]);
            unset($resources[$event->udata]);
            echo "{$resource[name]}: gone\n";
        }
    }
}
```
## Requirements

| | |
|---|---|
| OS | macOS. The extension binds `kevent64()`, which only Darwin provides; `configure` stops on any other kernel. |
| PHP | 8.4 or newer, NTS or ZTS, 64-bit. The build refuses a 32-bit `zend_long` because `kevent64_s` carries 64-bit fields. |
| Libraries | None beyond libSystem. |
| Optional | ext/sockets, if you want to pass `Socket` objects as `$ident`. Detected at build, resolved at runtime; the module loads without it. |

To build it you need the Xcode Command Line Tools and the development files of the PHP you intend to load it into. Homebrew's `php@8.4` and `php@8.4-zts` ship `phpize` and `php-config`:

```bash
xcode-select --install
brew install php@8.4
```

## Installation

### With PHP PIE (recommended)

[PIE](https://github.com/php/pie) is the PHP Foundation's installer for extensions. It downloads the tagged source from Packagist, builds it against the PHP on your `PATH`, installs the module and enables it:

```bash
pie install php-io-extensions/kqueue
```

### Manually (install script)

The repository ships `install-macos.sh`. With no arguments it targets Homebrew's `php@8.4` and `php@8.4-zts`; pass PHP binaries to target others. For each one it builds in a temporary copy, installs `kqueue.so` into that PHP's `extension_dir`, ad-hoc signs it so macOS loads it on first use, writes `30-kqueue.ini` into its `conf.d` scan directory, and checks the module loads:

```bash
git clone https://github.com/php-io-extensions/kqueue.git
cd kqueue
./install-macos.sh
./install-macos.sh /path/to/bin/php
```

### Manually (phpize)

```bash
git clone https://github.com/php-io-extensions/kqueue.git
cd kqueue
phpize
./configure --enable-kqueue
make -j"$(sysctl -n hw.ncpu)"
make install
```

Then enable it: add `extension=kqueue.so` to your `php.ini`, or drop that line into a file in the scan directory that `php --ini` reports (Homebrew: `/opt/homebrew/etc/php/8.4/conf.d`).

### Verify

```console
$ php --ri kqueue

kqueue

kqueue support => enabled
Version => 0.11.x
```

## API

| Method | Description |
|---|---|
| `kqueue(): int` | Creates a kqueue and returns its fd, or `-1`. |
| `kevent(int $kq, array $changelist, int $nchanges, ?array &$eventlist, int $nevents, ?timespec $timeout): int` | The one call that does everything. Applies the first `$nchanges` entries of `$changelist` (each a `kevent`), then waits up to `$timeout` for at most `$nevents` events, which it writes into `$eventlist` as a list of `kevent` objects. Returns the number of events, `0` on timeout, `-1` on error. `$nevents` of `0` applies changes and returns at once; `$timeout` of `null` blocks until something fires. |
| `kevent64(int $kq, array $changelist, int $nchanges, ?array &$eventlist, int $nevents, int $flags, ?timespec $timeout): int` | As `kevent()`, with `kevent64_s` structs that carry two extra 64-bit words and a `$flags` argument (`KEVENT_FLAG_IMMEDIATE` returns at once without a `timespec`). A kqueue used with one of the two calls rejects the other with `EINVAL`; pick one per queue. |
| `EV_SET(kevent $kevp, Socket\|resource\|int $ident, int $filter, int $flags, int $fflags, int $data, int $udata): void` | Fills a `kevent` in place, as the C macro does. `$ident` is the thing the filter watches: an fd (as an int, a stream resource, or a `Socket`), a pid for `EVFILT_PROC`, a signal number for `EVFILT_SIGNAL`, any number you choose for `EVFILT_TIMER` and `EVFILT_USER`. `$udata` is your 64-bit tag, handed back untouched. |
| `EV_SET64(kevent64_s $kevp, Socket\|resource\|int $ident, int $filter, int $flags, int $fflags, int $data, int $udata, int $ext0, int $ext1): void` | Same for `kevent64_s`, with the two extension words. |
| `kqueue_errno(): int` | The `errno` saved by the last call that failed. Compare it against the `E*` constants below. Bad PHP arguments never reach here: they throw `ValueError` or `TypeError` before the syscall. |

| Struct | Fields | Description |
|---|---|---|
| `kevent` | `int $ident, $filter, $flags, $fflags, $data, $udata` | One registration or one delivered event. `$filter` is 16-bit signed, `$flags` 16-bit, `$fflags` 32-bit; a wider value throws `ValueError` naming the field. |
| `kevent64_s` | the six above plus `array $ext` | The `kevent64()` form. `$ext` is exactly two ints, `[0, 0]` on a fresh object. |
| `timespec` | `int $tv_sec, $tv_nsec` | The timeout. A fresh object is `0` / `0`, which polls. |

| Constant group | Names | Description |
|---|---|---|
| Filters | `EVFILT_READ`, `EVFILT_WRITE`, `EVFILT_EXCEPT`, `EVFILT_VNODE`, `EVFILT_PROC`, `EVFILT_SIGNAL`, `EVFILT_TIMER`, `EVFILT_USER`, `EVFILT_AIO`, `EVFILT_FS`, `EVFILT_MACHPORT`, `EVFILT_VM`, `EVFILT_SYSCOUNT`, `EVFILT_THREADMARKER` | What kind of thing `$ident` is and what counts as an event on it. |
| Actions | `EV_ADD`, `EV_DELETE`, `EV_ENABLE`, `EV_DISABLE`, `EV_ONESHOT`, `EV_CLEAR`, `EV_DISPATCH`, `EV_DISPATCH2`, `EV_RECEIPT`, `EV_UDATA_SPECIFIC`, `EV_VANISHED` | `$flags` you set on a change. `EV_CLEAR` is kqueue's edge-triggered mode. `EV_RECEIPT` reports a change error as an `EV_ERROR` event instead of a `-1` return. |
| Returned flags | `EV_EOF`, `EV_ERROR`, `EV_OOBAND`, `EV_POLL`, `EV_FLAG0`, `EV_FLAG1`, `EV_SYSFLAGS` | `$flags` the kernel sets on a delivered event. `EV_EOF` means the peer closed; `EV_ERROR` means `$data` holds an errno. |
| `kevent64()` flags | `KEVENT_FLAG_NONE`, `KEVENT_FLAG_IMMEDIATE`, `KEVENT_FLAG_ERROR_EVENTS` | The `$flags` argument of `kevent64()`. |
| Read / write | `NOTE_LOWAT`, `NOTE_OOB` | `$fflags` for `EVFILT_READ` and `EVFILT_WRITE`. |
| Vnode | `NOTE_DELETE`, `NOTE_WRITE`, `NOTE_EXTEND`, `NOTE_ATTRIB`, `NOTE_LINK`, `NOTE_RENAME`, `NOTE_REVOKE`, `NOTE_FUNLOCK`, `NOTE_LEASE_DOWNGRADE`, `NOTE_LEASE_RELEASE`, `NOTE_NONE` | What to watch on an open file or directory under `EVFILT_VNODE`. |
| Process | `NOTE_EXIT`, `NOTE_FORK`, `NOTE_EXEC`, `NOTE_SIGNAL`, `NOTE_EXITSTATUS`, `NOTE_EXIT_DETAIL`, `NOTE_EXIT_DETAIL_MASK`, `NOTE_EXIT_DECRYPTFAIL`, `NOTE_EXIT_MEMORY`, `NOTE_EXIT_CSERROR`, `NOTE_EXIT_REPARENTED`, `NOTE_REAP`, `NOTE_TRACK`, `NOTE_TRACKERR`, `NOTE_CHILD`, `NOTE_PDATAMASK`, `NOTE_PCTRLMASK` | What to watch on a pid under `EVFILT_PROC`. |
| Timer | `NOTE_SECONDS`, `NOTE_USECONDS`, `NOTE_NSECONDS`, `NOTE_MACHTIME`, `NOTE_MACH_CONTINUOUS_TIME`, `NOTE_ABSOLUTE`, `NOTE_LEEWAY`, `NOTE_CRITICAL`, `NOTE_BACKGROUND` | Units and behaviour of `EVFILT_TIMER`; `$data` is the period, milliseconds unless a unit is given. |
| User | `NOTE_TRIGGER`, `NOTE_FFNOP`, `NOTE_FFAND`, `NOTE_FFOR`, `NOTE_FFCOPY`, `NOTE_FFCTRLMASK`, `NOTE_FFLAGSMASK` | Fire an `EVFILT_USER` event from your own code and control how its low 24 `$fflags` bits combine. |
| Memory | `NOTE_VM_PRESSURE`, `NOTE_VM_PRESSURE_TERMINATE`, `NOTE_VM_PRESSURE_SUDDEN_TERMINATE`, `NOTE_VM_ERROR` | `EVFILT_VM` notes. |
| Errno | `EACCES`, `EBADF`, `EFAULT`, `EINTR`, `EINVAL`, `EMFILE`, `ENFILE`, `ENOENT`, `ENOMEM`, `ESRCH` | The values `kqueue_errno()` and an `EV_ERROR` event's `$data` can hold. |

Every value comes from `<sys/event.h>` at build time.

## Extensions you might use with this one

* [ext-posi](https://github.com/php-io-extensions/posi): POSIX calls as PHP functions. Gives you `posix_open()`, `posix_read()`, `posix_close()` and `ioctl()` for the raw fds kqueue hands back, including the kqueue itself, which core PHP cannot close.
* [ext/sockets](https://www.php.net/manual/en/book.sockets.php): ships with PHP. Its `Socket` objects go straight into `EV_SET()` as `$ident`.

## AI-assisted development

This codebase was built with LLM assistance under human direction. Prompts, reviews, and architectural decisions were owned by a professional software engineer — the model accelerates drafting and exploration; it does not replace judgment on APIs, dependency direction, or what ships.

## Agentic Documentation

This repository carries an [Open Knowledge Format](https://github.com/GoogleCloudPlatform/knowledge-catalog/blob/main/okf/SPEC.md) bundle at [`.okf/`](.okf/): Markdown with YAML frontmatter, readable by you and loadable by a coding agent. It records what the README doesn't: how the module is laid out, how a `kevent` object becomes a C struct and back, why each field check exists, how the stub becomes `kqueue_arginfo.h`, and a log of every change that altered behaviour.

Start at [`.okf/index.md`](.okf/index.md). [`AGENTS.md`](AGENTS.md) is the short rule sheet an agent reads before touching the C, the stub, or packaging; each rule points into the bundle for the reason behind it.

Point your agent at the repo and it has the same context the maintainer has. Point yourself at it when the README's answer is "it does X" and you want "because".

## Resources

* [`kqueue(2)`](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/kqueue.2.html) (Apple) and [`kqueue(2)`](https://man.freebsd.org/cgi/man.cgi?kqueue) (FreeBSD): the kernel interface this extension binds. Apple's page matches the `<sys/event.h>` built against here; FreeBSD's explains the filters in more depth. The man pages are the manual; this README only adds what PHP changes.
* [PIE](https://github.com/php/pie): the PHP Foundation's extension installer.
* [`gen_stub.php`](https://github.com/php/php-src/blob/master/build/gen_stub.php): the php-src stub format. [`kqueue.stub.php`](kqueue.stub.php) is the API declaration; `kqueue_arginfo.h` is generated from it.
* [Open Knowledge Format](https://github.com/GoogleCloudPlatform/knowledge-catalog/blob/main/okf/SPEC.md): the spec the [`.okf/`](.okf/) bundle follows.

## Running the test suite

The suite is Pest v4 and needs no hardware: socket pairs, timers, user events, a child signal. It runs on macOS only, since the module does. Build the module in the clone, then point `php` at it:

```bash
git clone https://github.com/php-io-extensions/kqueue.git
cd kqueue
phpize && ./configure --enable-kqueue && make -j"$(sysctl -n hw.ncpu)"
composer install
php -n -d extension="$PWD/modules/kqueue.so" vendor/bin/pest
```

```console
Tests:    74 passed (209 assertions)
Duration: 1.12s
```

`-n` skips your `php.ini`, so the suite tests the clone and not an installed `kqueue.so`; without it, a PHP that already loads kqueue warns `Module "kqueue" is already loaded` and keeps the installed one. A module only loads in the PHP whose `phpize` built it: for Homebrew's ZTS build, run `phpize` and pass `--with-php-config` from `/opt/homebrew/opt/php@8.4-zts/bin`.

## Contributing

Open an issue first for anything beyond a typo. Pull requests change `kqueue.stub.php` and `kqueue.c` together, regenerate `kqueue_arginfo.h` with gen_stub, and add a Pest test for the behaviour. CI must be green on every cell of the matrix. Read [`AGENTS.md`](AGENTS.md) for the rules the code follows.

## Security

kqueue only reports events; it never reads, writes or closes a descriptor. See [SECURITY.md](SECURITY.md) for the model and how to report a vulnerability.

## License

MIT. See [LICENSE](LICENSE).
