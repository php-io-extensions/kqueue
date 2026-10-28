# Log

## 2026-10-28

* **Version**: 0.11.0 in `PHP_KQUEUE_VERSION` (`php_kqueue.h`); `branch-alias` `dev-main` is `0.11.x-dev`. Promoted with the Venusian 0.11 ecosystem. No API or behaviour change; built and loaded on Homebrew PHP 8.4 NTS and ZTS.
* **Open — Socket where ext/sockets is shared**: `kqueue.c` sets `KQ_HAVE_SOCKETS` only when `HAVE_SOCKETS` is defined. A PHP that builds ext/sockets shared and leaves the macro out of `php_config.h` would compile Socket support out and refuse `Socket` objects. Homebrew defines it; a BSD package build may not. epoll 0.10.6 defines `HAVE_SOCKETS` itself when the header is present.

## 2026-10-11

* **Creation**: Bundle created for ext-kqueue 0.10.5 from the repository history (ef7d2cc through f8f99a8): [module](/architecture/module.md), [functions](/api/functions.md), [structs](/api/structs.md), [constants](/api/constants.md), [build](/runbooks/build.md).
