<?php

declare(strict_types=1);

it('zero-initialises kevent like a zeroed struct', function (): void {
    expect(get_object_vars(new kevent()))->toBe([
        'ident' => 0, 'filter' => 0, 'flags' => 0, 'fflags' => 0, 'data' => 0, 'udata' => 0,
    ]);
});

it('zero-initialises kevent64_s including both ext slots', function (): void {
    expect(get_object_vars(new kevent64_s()))->toBe([
        'ident' => 0, 'filter' => 0, 'flags' => 0, 'fflags' => 0, 'data' => 0, 'udata' => 0, 'ext' => [0, 0],
    ]);
});

it('zero-initialises timespec', function (): void {
    expect(get_object_vars(new timespec()))->toBe(['tv_sec' => 0, 'tv_nsec' => 0]);
});

it('rejects dynamic properties on the struct classes', function (object $struct): void {
    expect(fn () => $struct->nope = 1)->toThrow(Error::class);
})->with([
    'kevent' => fn (): kevent => new kevent(),
    'kevent64_s' => fn (): kevent64_s => new kevent64_s(),
    'timespec' => fn (): timespec => new timespec(),
]);

it('declares the struct classes final', function (string $class): void {
    expect((new ReflectionClass($class))->isFinal())->toBeTrue();
})->with(['kevent', 'kevent64_s', 'timespec']);
