<?php

declare(strict_types=1);

beforeEach(function (): void {
    $this->kq = kqueue();
    expect($this->kq)->toBeGreaterThanOrEqual(0);
});

it('round-trips every 64-bit field including ext', function (): void {
    $kev = new kevent64_s();
    EV_SET64($kev, PHP_INT_MAX, EVFILT_USER, EV_ADD, NOTE_TRIGGER, 0, PHP_INT_MIN, -1, 3);

    expect(kevent64($this->kq, [$kev], 1, $events, 2, 0, zeroTimeout()))->toBe(1)
        ->and($events[0])->toBeInstanceOf(kevent64_s::class)
        ->and($events[0]->ident)->toBe(PHP_INT_MAX)
        ->and($events[0]->filter)->toBe(EVFILT_USER)
        ->and($events[0]->udata)->toBe(PHP_INT_MIN)
        ->and($events[0]->ext)->toBe([-1, 3]);
});

it('honours KEVENT_FLAG_IMMEDIATE without a timeout', function (): void {
    expect(kevent64($this->kq, [], 0, $events, 4, KEVENT_FLAG_IMMEDIATE, null))->toBe(0)
        ->and($events)->toBe([]);
});

it('passes the kernel EINVAL through when a kqueue already used by kevent() meets kevent64()', function (): void {
    kevent($this->kq, [], 0, $none, 0, zeroTimeout());

    expect(kevent64($this->kq, [], 0, $events, 1, 0, zeroTimeout()))->toBe(-1)
        ->and(kqueue_errno())->toBe(EINVAL);
});

it('passes the kernel EINVAL through when a kqueue already used by kevent64() meets kevent()', function (): void {
    kevent64($this->kq, [], 0, $none, 0, 0, zeroTimeout());

    expect(kevent($this->kq, [], 0, $events, 1, zeroTimeout()))->toBe(-1)
        ->and(kqueue_errno())->toBe(EINVAL);
});

it('rejects flags wider than unsigned int', function (): void {
    expect(fn () => kevent64($this->kq, [], 0, $events, 0, 0x100000000, zeroTimeout()))
        ->toThrow(ValueError::class, 'Argument #6 ($flags)');
});

it('rejects changelist entries that are not kevent64_s', function (): void {
    expect(fn () => kevent64($this->kq, [new kevent()], 1, $events, 0, 0, zeroTimeout()))
        ->toThrow(TypeError::class, 'must contain only kevent64_s, kevent given at position 0');
});

it('rejects an ext that is not two ints at keys 0 and 1', function (array $ext, string $class, string $message): void {
    $kev = new kevent64_s();
    $kev->ext = $ext;

    expect(fn () => kevent64($this->kq, [$kev], 1, $events, 0, 0, zeroTimeout()))
        ->toThrow($class, $message);
})->with([
    'too few' => [[0], ValueError::class, 'kevent64_s::$ext must hold exactly 2 elements, 1 held'],
    'wrong keys' => [['a' => 0, 'b' => 0], ValueError::class, 'kevent64_s::$ext must be a list with keys 0 and 1'],
    'not int' => [[0, '1'], TypeError::class, 'kevent64_s::$ext[1] must be of type int, string given'],
]);
