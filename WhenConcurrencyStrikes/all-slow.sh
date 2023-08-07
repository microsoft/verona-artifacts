#!/bin/bash

echo Started at $(date)

set -e

pushd /artifact

echo "Running Savina (expected duration ~1hr 10mins)"

python3 verona-benchmarks/scripts/run_savina.py -o outputs \
  --verona-path verona-benchmarks/build/savina \
  --pony-path pony-savina/savina-pony \
  --repeats 50

echo Completed Savina at $(date)

echo "Running Dining Philosophers (expected duration ~4hr 10mins)"

python3 verona-benchmarks/scripts/run_dining.py -o outputs \
  --verona-path verona-rt/build/test \
  --repeats 7

echo Completed Dining Philosophers at $(date)

./pony-savina/savina-pony/savina-pony --benchmark "Banking 2PC" --reps 50 --ponymaxthreads 1 > outputs/pony-banking-2pc-1.txt
./pony-savina/savina-pony/savina-pony --benchmark "Banking 2PC" --reps 50 --ponymaxthreads 8 > outputs/pony-banking-2pc-8.txt

echo Completed Banking2PC in Pony at $(date)

RESULTS_DIR=/artifact/outputs

python3 verona-benchmarks/scripts/produce_table_actor.py -i $RESULTS_DIR > outputs/table1_pony_boc.tex

echo Completed Table 1 at $(date)

pushd verona-benchmarks/savina
cloc . --match-d="actors|boc" --by-file --quiet --csv --out=/artifact/outputs/cloc_raw.csv
popd # verona-benchmarks/savina

python3 verona-benchmarks/scripts/produce_table_boc_full.py -i $RESULTS_DIR > outputs/table2_boc_full.tex

echo Completed Table 2 at $(date)

python3 verona-benchmarks/scripts/produce_graph_dining.py -i $RESULTS_DIR -o outputs

echo Completed Dining Graph at $(date)

python3 verona-benchmarks/scripts/produce_graph_banking_scale.py -i $RESULTS_DIR -o outputs

popd # /artifact

echo Completed everything at $(date)