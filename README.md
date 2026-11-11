# 5 Bytes in a Row

Random data contains 5 identical bytes in a row about once every 4 GiB:

```
(1/256)^4 = 1 in 2^32 = 1 in 4 GiB
```

Encryption keys, TLS sessions, and passwords all depend on random numbers, so systems check their random number generators continuously. Some of these health tests treat a run like this as a sign that the source is broken. A healthy source produces these runs too.

## The Rate Is Global

The chance depends on the total number of bytes read, not on where they were read. One server reading 4 GiB and 2000 VMs reading 2 MiB each have almost the same chance of producing a run:

```
$ python3 fleet.py
One machine, 4 GiB:       63% chance
2000 VMs, 2 MiB each:     62% chance
  Any single VM:          0.05% chance
```

A check that takes a machine down on a chance event will, at a fixed rate, take down a healthy machine somewhere. A single operator cannot predict it or prevent it, and a larger fleet only makes it more frequent.

`random_blocks.c` confirms the math on real data. Reading 4 GiB from `/dev/urandom` gave 2 runs of 5 (1 expected), and the shorter runs were within 1% of the expected counts.

## Where Linux Does This

FIPS mode (`fips=1`) is required on systems that handle government, financial, and healthcare data, and most major Linux distributions and cloud providers offer it. In this mode, the kernel's jitter entropy RNG runs a permanent health test. A healthy source fails it about 1 in 2^60 times, and the kernel panics when it does.

A panic stops the whole machine immediately. Every service on it goes down, open connections and in-flight work are lost, and the machine stays down until it reboots. The log shows a health test failure, so the hardware looks faulty even though nothing is wrong with it.

## The Fix

[The patch](jitterentropy-retry.patch) replaces the panic with a retry. On a permanent failure, the kernel discards the output, swaps in a new collector that has passed the startup health test, and reads again. It repeats until the read succeeds. The caller gets valid random data, and the only trace of the failure is a warning in the kernel log.

## Testing

I booted the kernel in QEMU with `fips=1` and used a test-only hack ([`test/fake_failures_hack.py`](test/fake_failures_hack.py)) to force permanent failures:

```
                          unpatched       patched
1 failure                 kernel panic    read succeeds
5 failures in a row       -               read succeeds
```

Logs are in [`test/results/`](test/results/).

## Notes

- If the noise source is actually broken, reads now retry indefinitely instead of panicking.
- FIPS failure handling is part of a module's certification, so this patch is a proposal.
