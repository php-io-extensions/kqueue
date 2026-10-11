---
type: Runbook
title: Build, install, test
description: gen_stub after a stub edit, scratch builds per PHP, the Mac installer, Pest on php84 and zhp, CI matrix, PIE.
resource: install-macos.sh
tags: [kqueue, build, macos, pest, pie, ci]
status: draft
generated: { by: claude-fable/5.1, at: 2026-10-11T20:34:38Z }
sources:
  - id: mac
    resource: install-macos.sh
    title: install-macos.sh
  - id: ci
    resource: .github/workflows/ci.yml
    title: ci.yml
  - id: composer
    resource: composer.json
    title: composer.json
  - id: clone-run
    resource: "scope: fresh clone built and tested on the Mac, 2026-10-11"
    title: Clone-and-test run, php84 8.4.25 NTS and zhp 8.4.25 ZTS
---

# Needs

macOS with the Xcode Command Line Tools. php-dev (phpize, php-config) for the target PHP: Homebrew `php@8.4` and `php@8.4-zts` ship them. No library. ext/sockets optional.

# Stub → arginfo

Edit `kqueue.stub.php`, never `kqueue_arginfo.h`. Regenerate with the target PHP's gen_stub:

```bash
php "$(php-config --prefix)/lib/php/build/gen_stub.php" kqueue.stub.php
```

Commit stub and arginfo together; the arginfo header carries the stub hash. Classes are `@strict-properties`; the property order in the stub must match the `KQ_*` slot enums in `kqueue.c`.

# Scratch build

phpize builds in the repo root; `.gitignore` covers every artifact. A module only loads in the PHP whose `phpize` built it, so build once per target:

```bash
phpize && ./configure --enable-kqueue && make -j"$(sysctl -n hw.ncpu)"
Z=/opt/homebrew/opt/php@8.4-zts/bin; $Z/phpize && ./configure --enable-kqueue --with-php-config=$Z/php-config && make -j"$(sysctl -n hw.ncpu)"
```

`--enable-kqueue` is required by hand: `config.m4` defaults the flag to `no`.

# Test

Pest v4, hardware-free: `stream_socket_pair`, `socket_create_pair`, timers, user events, `pcntl_alarm` for `EINTR`. `tests/Pest.php` refuses to run without the extension loaded and resolves a stream's fd through `/dev/fd` (inode + mode match), since PHP has no public fd accessor.

```bash
composer install
php -n -d extension="$PWD/modules/kqueue.so" -d memory_limit=128M vendor/bin/pest
```

`-n` keeps an installed `kqueue.so` from loading first. Proven from a fresh clone of the published repo on both `php84` (8.4.25 NTS) and `zhp` (8.4.25 ZTS): 74 passed, 209 assertions each.[^clone-run]

# CI

`macos-15` × PHP `8.4`, `8.5` × `nts`, `zts` with `shivammathur/setup-php`. Steps: phpize/configure/make, load check with `php -n -d extension=…`, `--ri kqueue`, then `composer install` and Pest.[^ci]

# Installer

`install-macos.sh [php …]`: default targets Homebrew `php@8.4` and `php@8.4-zts`. Per PHP: build in a temp copy with that PHP's `phpize`/`php-config`, copy `kqueue.so` into `extension_dir`, `codesign --force --sign -` so amfid accepts it on first load, write `30-kqueue.ini` into the conf.d scan dir, verify `extension_loaded('kqueue')`. Refuses to run on anything but Darwin.[^mac]

# PIE

`composer.json` `type: php-ext`, `extension-name: kqueue`, priority 30, NTS + ZTS, `os-families: [darwin]`, configure option `--enable-kqueue`. No `version` key: git tags drive Packagist. `.gitattributes` export-ignores tests, `phpunit.xml`, `.okf`, `AGENTS.md`, `CLAUDE.md`. Not yet exercised with `pie install` on this package; ext-epoll's identical manifest shape was, on the Pi.[^composer]

# Version

`PHP_KQUEUE_VERSION` in `php_kqueue.h` is the only version field in the repo; bump it only past a published tag, and tag to publish.

[^mac]: install-macos.sh
[^ci]: .github/workflows/ci.yml
[^composer]: composer.json
[^clone-run]: Clone-and-test run, 2026-10-11
