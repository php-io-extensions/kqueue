<?php

declare(strict_types=1);

it('registers filter values from the SDK header', function (string $name, int $value): void {
    expect(constant($name))->toBe($value);
})->with([
    ['EVFILT_READ', -1],
    ['EVFILT_WRITE', -2],
    ['EVFILT_TIMER', -7],
    ['EVFILT_USER', -10],
    ['EVFILT_EXCEPT', -15],
]);

it('registers flag values with C expression semantics', function (string $name, int $value): void {
    expect(constant($name))->toBe($value);
})->with([
    ['EV_ADD', 0x0001],
    ['EV_DISPATCH2', 0x0180],
    ['EV_ERROR', 0x4000],
    ['NOTE_FFOR', 0x80000000],
    ['NOTE_PCTRLMASK', ~0x000fffff],
    ['NOTE_REAP', 0x10000000],
    ['NOTE_EXIT_REPARENTED', 0x00080000],
    ['KEVENT_FLAG_IMMEDIATE', 0x000001],
]);

it('registers the errno values kqueue(2) documents', function (string $name): void {
    expect(defined($name))->toBeTrue()->and(constant($name))->toBeInt()->toBeGreaterThan(0);
})->with(['EACCES', 'EBADF', 'EFAULT', 'EINTR', 'EINVAL', 'EMFILE', 'ENFILE', 'ENOENT', 'ENOMEM', 'ESRCH']);
