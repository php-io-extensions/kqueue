<?php

declare(strict_types=1);

it('fills every kevent field', function (): void {
    $kev = new kevent();
    EV_SET($kev, 42, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, 5, 0xdeadbeef);

    expect(get_object_vars($kev))->toBe([
        'ident' => 42, 'filter' => EVFILT_TIMER, 'flags' => EV_ADD | EV_ONESHOT,
        'fflags' => NOTE_SECONDS, 'data' => 5, 'udata' => 0xdeadbeef,
    ]);
});

it('fills every kevent64_s field including ext', function (): void {
    $kev = new kevent64_s();
    EV_SET64($kev, 7, EVFILT_USER, EV_ADD, NOTE_TRIGGER, -3, PHP_INT_MIN, -1, 3);

    expect(get_object_vars($kev))->toBe([
        'ident' => 7, 'filter' => EVFILT_USER, 'flags' => EV_ADD, 'fflags' => NOTE_TRIGGER,
        'data' => -3, 'udata' => PHP_INT_MIN, 'ext' => [-1, 3],
    ]);
});

it('stores narrow fields as their C type holds them', function (): void {
    $kev = new kevent();
    EV_SET($kev, 0, 0xffff, -1, NOTE_PCTRLMASK, 0, 0);

    expect($kev->filter)->toBe(-1)
        ->and($kev->flags)->toBe(0xffff)
        ->and($kev->fflags)->toBe(0xfff00000);
});

it('rejects narrow values wider than the C field', function (int $filter, int $flags, int $fflags, string $argument): void {
    expect(fn () => EV_SET(new kevent(), 0, $filter, $flags, $fflags, 0, 0))
        ->toThrow(ValueError::class, $argument);
})->with([
    'filter' => [0x10000, 0, 0, 'Argument #3 ($filter)'],
    'flags' => [0, -0x8001, 0, 'Argument #4 ($flags)'],
    'fflags' => [0, 0, 0x100000000, 'Argument #5 ($fflags)'],
]);

it('assigns through references held on a property', function (): void {
    $kev = new kevent();
    $flags = &$kev->flags;
    EV_SET($kev, 1, EVFILT_USER, EV_ADD, 0, 0, 0);

    expect($flags)->toBe(EV_ADD);
});

it('takes the descriptor behind a stream as ident', function (): void {
    [$a, , $fdA] = socketPairWithFds();
    $kev = new kevent();
    EV_SET($kev, $a, EVFILT_READ, EV_ADD, 0, 0, 0);

    expect($kev->ident)->toBe($fdA);
});

it('takes the descriptor behind a stream as ident in EV_SET64', function (): void {
    [$a, , $fdA] = socketPairWithFds();
    $kev = new kevent64_s();
    EV_SET64($kev, $a, EVFILT_READ, EV_ADD, 0, 0, 0, 0, 0);

    expect($kev->ident)->toBe($fdA);
});

it('takes the descriptor behind a Socket as ident', function (): void {
    socket_create_pair(AF_UNIX, SOCK_STREAM, 0, $pair);
    $kev = new kevent();
    EV_SET($kev, $pair[0], EVFILT_READ, EV_ADD, 0, 0, 0);

    expect($kev->ident)->toBe(fdOf(socket_export_stream($pair[0])));
})->skip(! extension_loaded('sockets'), 'needs ext-sockets');

it('registers a stream by resource and reports it by descriptor', function (): void {
    $kq = kqueue();
    [$a, $b, $fdA] = socketPairWithFds();
    $read = new kevent();
    EV_SET($read, $a, EVFILT_READ, EV_ADD, 0, 0, 0);
    expect(kevent($kq, [$read], 1, $none, 0, zeroTimeout()))->toBe(0);

    fwrite($b, 'x');

    expect(kevent($kq, [], 0, $events, 1, zeroTimeout()))->toBe(1)
        ->and($events[0]->ident)->toBe($fdA);
});

it('rejects an ident that is not an int, stream or Socket', function (): void {
    expect(fn () => EV_SET(new kevent(), 'x', EVFILT_READ, EV_ADD, 0, 0, 0))
        ->toThrow(TypeError::class, 'Argument #2 ($ident) must be of type Socket|resource|int, string given');
});

it('rejects a closed stream as ident', function (): void {
    [$a] = socketPairWithFds();
    fclose($a);

    expect(fn () => EV_SET(new kevent(), $a, EVFILT_READ, EV_ADD, 0, 0, 0))
        ->toThrow(TypeError::class, 'Argument #2 ($ident) must be a valid stream resource');
});
