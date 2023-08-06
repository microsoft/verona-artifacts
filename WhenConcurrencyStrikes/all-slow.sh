#!/bin/bash

set -e

pushd /artifact

python3 verona-benchmarks/scripts/run_savina.py -o outputs \
  --verona-path verona-benchmarks/build/savina \
  --pony-path pony-savina/savina-pony \
  --repeats 50

python3 verona-benchmarks/scripts/run_dining.py -o outputs \
  --verona-path verona-rt/build/test \
  --repeats 50

./pony-savina/savina-pony/savina-pony --benchmark "Banking 2PC" --reps 50 --ponymaxthreads 1 > outputs/pony-banking-2pc-1.txt
./pony-savina/savina-pony/savina-pony --benchmark "Banking 2PC" --reps 50 --ponymaxthreads 8 > outputs/pony-banking-2pc-8.txt

RESULTS_DIR=/artifact/outputs

python3 verona-benchmarks/scripts/produce_table_actor.py -i $RESULTS_DIR > outputs/table1_pony_boc.tex

pushd verona-benchmarks/savina
cloc . --match-d="actors|boc" --by-file --quiet --csv --out=/artifact/outputs/cloc_raw.csv
popd # verona-benchmarks/savina

python3 verona-benchmarks/scripts/produce_table_boc_full.py -i $RESULTS_DIR > outputs/table2_boc_full.tex

python3 verona-benchmarks/scripts/produce_graph_dining.py -i $RESULTS_DIR -o outputs
python3 verona-benchmarks/scripts/produce_graph_banking_scale.py -i $RESULTS_DIR -o outputs

popd # /artifact