#!/usr/bin/env bash

#numactl --membind=0 --cpubind=0 ./stream-onecore
#OMP_NUM_THREADS=12 numactl --membind=0 --cpubind=0 ./stream

#./stream
#numactl -l ./stream
#numactl --cpubind=0 --membind=0 ./stream
#numactl --cpubind=0 --membind=1 ./stream
#numactl --interleave=all ./stream

numastat > stat1.txt
OMP_NUM_THREADS=24 ./stream
numastat > stat2.txt
diff stat1.txt stat2.txt
rm stat1.txt stat2.txt
