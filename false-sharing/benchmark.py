import os
import subprocess
import sys
import itertools
import matplotlib.pyplot as plt
import pandas
import seaborn


THREADS = [1, 2, 3, 4, 5, 6, 7, 8]
INCREMENTS = [1, 8]

INPUTS = itertools.product(THREADS, INCREMENTS)

EXECUTABLE = os.path.join(os.path.dirname(__file__), sys.argv[1])

frame = pandas.DataFrame(columns=["Thread", "Increment", "Time"])

for (threads, increment) in INPUTS:
    args = EXECUTABLE, str(threads), str(increment)
    times = []

    for i in range(4):
        res = subprocess.run(args,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
        times.append(float(res.stderr.decode().strip()))
    output = sum(times) / len(times)

    frame = frame.append({
        "Thread": str(threads),
        "Increment": str(increment),
        "Time": output
    }, ignore_index=True)

    print("Threads: {}, increment: {}, time: {} us".format(threads, increment, output))

seaborn.barplot(data=frame, x="Thread", y="Time", hue="Increment")
plt.show()
