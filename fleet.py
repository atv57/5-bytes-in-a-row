import math
import sys

GiB = 1024 ** 3


def expected_runs(num_bytes, run_len=5):
    return num_bytes / 256 ** (run_len - 1)


def chance_at_least_one(num_bytes, run_len=5):
    return 1 - math.exp(-expected_runs(num_bytes, run_len))


if len(sys.argv) == 3:
    n, k = int(sys.argv[1]), int(sys.argv[2])
    print(f"Expected runs: {expected_runs(n, k):.3g}, "
          f"{chance_at_least_one(n, k) * 100:.1f}% chance of at least one")
    sys.exit()

print(f"One machine, 4 GiB:       {chance_at_least_one(4 * GiB) * 100:.0f}% chance")
print(f"2000 VMs, 2 MiB each:     {chance_at_least_one(2000 * 2 * 1024 ** 2) * 100:.0f}% chance")
print(f"  Any single VM:          {chance_at_least_one(2 * 1024 ** 2) * 100:.2f}% chance")
