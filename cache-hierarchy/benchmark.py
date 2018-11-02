import os
import subprocess
import sys

sizes = [
    (16 * 1024, 10),
    (32 * 1024, 10),
    (128 * 1024, 10),
    (256 * 1024, 10),
    (1 * 1024 * 1024, 10),
    (2 * 1024 * 1024, 10),
    (3 * 1024 * 1024, 10),
    (4 * 1024 * 1024, 10),
    (6 * 1024 * 1024, 10),
    (10 * 1024 * 1024, 4),
    (20 * 1024 * 1024, 4),
    (30 * 1024 * 1024, 4),
    (50 * 1024 * 1024, 2)
]

COMPILER = "gcc"
EXECUTABLE = os.path.join(os.path.dirname(__file__), sys.argv[1]) # os.path.join("build" if COMPILER == "gcc" else "build-icc", "stream")

for (size, repeat) in sizes:
    args = ["taskset", "0x1", EXECUTABLE, str(size), str(repeat)]
    res = subprocess.run(args,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)
    output = res.stderr.decode().strip()
    #print(res.stdout.decode().strip())
    results = [float(n) for n in output.split()]
    print("Size: {}, bandwidth: {}".format(size, results[0]))
