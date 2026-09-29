<?php

declare(strict_types=1);

beforeEach(function (): void {
    $this->kq = kqueue();
    expect($this->kq)->toBeGreaterThanOrEqual(0);
});

it('reports read readiness and byte count on a socket', function (): void {
    [$a, $b, $fdA] = socketPairWithFds();
    $read = new kevent();
    EV_SET($read, $fdA, EVFILT_READ, EV_ADD, 0, 0, 1234);
    expect(kevent($this->kq, [$read], 1, $none, 0, zeroTimeout()))->toBe(0)
        ->and(kevent($this->kq, [], 0, $idle, 4, zeroTimeout()))->toBe(0);

    fwrite($b, 'hello');

    expect(kevent($this->kq, [], 0, $events, 4, zeroTimeout()))->toBe(1)
        ->and($events[0])->toBeInstanceOf(kevent::class)
        ->and($events[0]->ident)->toBe($fdA)
        ->and($events[0]->filter)->toBe(EVFILT_READ)
        ->and($events[0]->data)->toBe(5)
        ->and($events[0]->udata)->toBe(1234)
        ->and(fread($a, 5))->toBe('hello');
});

it('reports EV_EOF when the peer closes', function (): void {
    [$a, $b, $fdA] = socketPairWithFds();
    $read = new kevent();
    EV_SET($read, $fdA, EVFILT_READ, EV_ADD, 0, 0, 0);
    kevent($this->kq, [$read], 1, $none, 0, zeroTimeout());

    fclose($b);

    expect(kevent($this->kq, [], 0, $events, 1, zeroTimeout()))->toBe(1)
        ->and($events[0]->flags & EV_EOF)->toBe(EV_EOF);
});

it('fires a oneshot timer and round-trips udata', function (int $udata): void {
    $timer = new kevent();
    EV_SET($timer, 1, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, 1, $udata);
    $wait = new timespec();
    $wait->tv_sec = 1;

    $n = kevent($this->kq, [$timer], 1, $events, 1, $wait);

    expect($n)->toBe(1)
        ->and($events[0]->filter)->toBe(EVFILT_TIMER)
        ->and($events[0]->udata)->toBe($udata);
})->with([0xdeadbeef, -1, PHP_INT_MIN, PHP_INT_MAX]);

it('delivers a user event triggered with NOTE_TRIGGER', function (): void {
    $add = new kevent();
    EV_SET($add, 9, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0, 0);
    kevent($this->kq, [$add], 1, $none, 0, zeroTimeout());

    $trigger = new kevent();
    EV_SET($trigger, 9, EVFILT_USER, 0, NOTE_TRIGGER | NOTE_FFCOPY | 0x1234, 0, 0);

    expect(kevent($this->kq, [$trigger], 1, $events, 1, zeroTimeout()))->toBe(1)
        ->and($events[0]->ident)->toBe(9)
        ->and($events[0]->fflags & NOTE_FFLAGSMASK)->toBe(0x1234);
});

it('returns 0 and an empty eventlist when the timeout expires', function (): void {
    $events = null;

    expect(kevent($this->kq, [], 0, $events, 8, zeroTimeout()))->toBe(0)
        ->and($events)->toBe([]);
});

it('uses only the first nchanges entries of changelist', function (): void {
    $keep = new kevent();
    EV_SET($keep, 1, EVFILT_USER, EV_ADD, NOTE_TRIGGER, 0, 0);
    $skip = new kevent();
    EV_SET($skip, 2, EVFILT_USER, EV_ADD, NOTE_TRIGGER, 0, 0);

    expect(kevent($this->kq, [$keep, $skip], 1, $events, 4, zeroTimeout()))->toBe(1)
        ->and($events[0]->ident)->toBe(1);
});

it('returns -1, leaves eventlist untouched and records errno on failure', function (): void {
    $delete = new kevent();
    EV_SET($delete, 404, EVFILT_USER, EV_DELETE, 0, 0, 0);
    $events = ['untouched'];

    expect(kevent($this->kq, [$delete], 1, $events, 0, zeroTimeout()))->toBe(-1)
        ->and(kqueue_errno())->toBe(ENOENT)
        ->and($events)->toBe(['untouched']);
});

it('reports change errors as EV_ERROR events under EV_RECEIPT', function (): void {
    $delete = new kevent();
    EV_SET($delete, 404, EVFILT_USER, EV_DELETE | EV_RECEIPT, 0, 0, 77);

    expect(kevent($this->kq, [$delete], 1, $events, 1, zeroTimeout()))->toBe(1)
        ->and($events[0]->flags & EV_ERROR)->toBe(EV_ERROR)
        ->and($events[0]->data)->toBe(ENOENT)
        ->and($events[0]->udata)->toBe(77);
});

it('records EBADF for a descriptor that is not a kqueue', function (): void {
    expect(kevent(-1, [], 0, $events, 0, zeroTimeout()))->toBe(-1)
        ->and(kqueue_errno())->toBe(EBADF);
});

it('rejects nchanges outside the changelist', function (int $nchanges): void {
    expect(fn () => kevent($this->kq, [new kevent()], $nchanges, $events, 0, zeroTimeout()))
        ->toThrow(ValueError::class, 'Argument #3 ($nchanges)');
})->with([-1, 2]);

it('rejects a negative nevents', function (): void {
    expect(fn () => kevent($this->kq, [], 0, $events, -1, zeroTimeout()))
        ->toThrow(ValueError::class, 'Argument #5 ($nevents)');
});

it('rejects changelist entries that are not kevent', function (): void {
    expect(fn () => kevent($this->kq, [new kevent64_s()], 1, $events, 0, zeroTimeout()))
        ->toThrow(TypeError::class, 'must contain only kevent, kevent64_s given at position 0');
});

it('rejects an eventlist variable that is not ?array', function (): void {
    $events = 'string';

    expect(fn () => kevent($this->kq, [], 0, $events, 0, zeroTimeout()))
        ->toThrow(TypeError::class, 'Argument #4 ($eventlist) must be of type ?array, string given');
});

it('rejects struct fields wider than the C field', function (): void {
    $kev = new kevent();
    $kev->filter = 0x10000;

    expect(fn () => kevent($this->kq, [$kev], 1, $events, 0, zeroTimeout()))
        ->toThrow(ValueError::class, 'kevent::$filter must be between -32768 and 65535');
});

it('rejects an unset struct field', function (): void {
    $kev = new kevent();
    unset($kev->udata);

    expect(fn () => kevent($this->kq, [$kev], 1, $events, 0, zeroTimeout()))
        ->toThrow(Error::class, 'Typed property kevent::$udata must not be accessed before initialization');
});

it('returns -1 with EINTR when a signal interrupts a blocking wait', function (): void {
    pcntl_async_signals(true);
    pcntl_signal(SIGALRM, static fn (): null => null);
    pcntl_alarm(1);
    $wait = new timespec();
    $wait->tv_sec = 3;

    try {
        expect(kevent($this->kq, [], 0, $events, 1, $wait))->toBe(-1)
            ->and(kqueue_errno())->toBe(EINTR);
    } finally {
        pcntl_signal(SIGALRM, SIG_DFL);
    }
})->skip(! extension_loaded('pcntl'), 'needs pcntl to deliver a signal');
