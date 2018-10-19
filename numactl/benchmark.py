import os
import subprocess
import sys
sys.path.append("../snailwatch/client")

from swclient.client import Client

benchmarks = [
    #{"threads": "1", "numactl": None},
    #{"threads": "1", "numactl": "-l"},
    #{"threads": "1", "numactl": "--cpubind=0 --membind=0"},
    #{"threads": "1", "numactl": "--cpubind=0 --membind=1"},
    #{"threads": "1", "numactl": "--interleave=all"},
    {"threads": "12", "numactl": None},
    {"threads": "12", "numactl": "-l"},
    {"threads": "12", "numactl": "--cpubind=0 --membind=0"},
    {"threads": "12", "numactl": "--cpubind=0 --membind=1"},
    {"threads": "12", "numactl": "--interleave=all"},
    {"threads": "12", "numactl": "--cpubind=0 --interleave=all"},
    {"threads": "24", "numactl": None},
    {"threads": "24", "numactl": "-l"},
    {"threads": "24", "numactl": "--interleave=all"},
]

COMPILER = "icc"
EXECUTABLE = os.path.join("build" if COMPILER == "gcc" else "build-icc", "stream")

measurements = []
client = Client(
    'https://snailwatch.it4i.cz/dev/api',
    'abf89eefe41140a18820055f698174bb'
)

for benchmark in benchmarks:
    threads = benchmark["threads"]
    numactl = benchmark["numactl"]

    args = []
    if numactl:
        args = ["numactl", *numactl.split()]

    args.append(EXECUTABLE)
    environment = os.environ.copy()
    environment.update({
        "OMP_NUM_THREADS": threads,
        "OMP_PROC_BIND": "true"
    })

    res = subprocess.run(args,
                         stdout=subprocess.DEVNULL,
                         stderr=subprocess.PIPE,
                         env=environment)
    output = res.stderr.decode().strip()
    results = [float(n) for n in output.split()]
    assert len(results) == 4
    functions = ["copy", "mul", "add", "triad"]

    for (i, result) in enumerate(results):
        env = {
            "threads": threads,
            "compiler": COMPILER,
            "numactl": numactl if numactl else "(no numactl)"
        }
        bench_name = functions[i] + "_" + threads
        print("Threads: {}, numactl: {}, function: {} = {}".format(threads, numactl, functions[i], result))
        measurements.append((bench_name, env, {
            "result": {
                "value": result,
                "type": "time"
            }
        }))


print("Sending measurements")
#client.upload_measurements(measurements)
