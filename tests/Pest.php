<?php

declare(strict_types=1);

if (! extension_loaded('kqueue')) {
    throw new RuntimeException('The kqueue extension is not loaded; run pest with -d extension=/path/to/kqueue.so');
}

function zeroTimeout(): timespec
{
    return new timespec();
}

/**
 * The descriptor number behind a stream: the /dev/fd entry whose inode and
 * mode match the stream's fstat().
 *
 * @param resource $stream
 */
function fdOf($stream): int
{
    // /dev/fd entries are reused across descriptors; cached stats go stale.
    clearstatcache();
    $want = fstat($stream);
    // Entries that cannot be stat()ed through /dev/fd (a kqueue descriptor, or
    // the one scandir() itself used) cannot be the stream and are skipped.
    set_error_handler(static fn (): bool => true);
    try {
        foreach (scandir('/dev/fd') as $entry) {
            $st = ctype_digit($entry) ? stat("/dev/fd/{$entry}") : false;
            if ($st !== false && $st['ino'] === $want['ino'] && $st['mode'] === $want['mode']) {
                return (int) $entry;
            }
        }
    } finally {
        restore_error_handler();
    }

    throw new RuntimeException('No /dev/fd entry matches the stream');
}

/**
 * A connected UNIX socket pair plus each end's descriptor number.
 *
 * @return array{0: resource, 1: resource, 2: int, 3: int}
 */
function socketPairWithFds(): array
{
    [$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

    return [$a, $b, fdOf($a), fdOf($b)];
}
