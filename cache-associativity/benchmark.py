import os
import subprocess
import sys
import itertools


COUNTS = [1, 2, 4, 6, 8, 10, 12, 16]
MODES = [0]
INCREMENTS = ["0x4", "0x1000", "0x10000", "0x100000"]

INPUTS = itertools.product(COUNTS, MODES, INCREMENTS)

COMPILER = "gcc"
EXECUTABLE = os.path.join(os.path.dirname(__file__), sys.argv[1])

for (count, mode, increment) in INPUTS:
    args = ["taskset", "0x1", EXECUTABLE, str(count), str(mode), str(int(increment, 16))]
    times = []

    for i in range(4):
        res = subprocess.run(args,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
        times.append(float(res.stderr.decode().strip()))
    output = sum(times) / len(times)

    print("Count: {}, mode: {}, increment: {}, time: {} us".format(count, mode, increment, output))
