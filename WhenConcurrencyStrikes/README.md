# When Concurrency Strikes: Behaviour Oriented Concurrency

This contains the supporting artifact to the paper "When Concurrency Strikes."  The artifact is a Docker script that compares the performance of Pony with the BoC C++ library using the Savina benchmarks suite.

## Running the benchmarks

The instructions assume a working Docker installation:

First build the Docker image:
```
docker build -t verona-when .
```
[This step should take about 3 minutes.]

Note that the Docker build step will install all the necessary packages, and build the various benchmarks.  It does not run any benchmarks.

Then run the docker image:
```
mkdir artifact
cd artifact
docker run -v $PWD:/artifact/outputs -it verona-when
```

From inside the docker image, to run the Savina benchmarks for both Pony and Verona, run the following command:
```
python3 verona-benchmarks/scripts/run_savina.py -o outputs \
  --verona-path verona-benchmarks/build/savina \
  --pony-path pony-savina/savina-pony \
  --repeats 2
```
You can increase the number of repeats to get more accurate results, but it will take longer to run. This configuration should take 4-6 minutes to run.

This script will generate a series of `csv` files in the `outputs` directory.

To run the dining philosophers benchmark for both Verona and `std::lock`, run the following command:
```
python3 verona-benchmarks/scripts/run_dining.py -o outputs \
  --verona-path verona-rt/build/test \
  --repeats 2 \
  --fast
```
The `--fast` option runs the benchmarks with fewer iterations, to make it faster to run.  To get more accurate results as used in the paper, remove this option.

[TODO 2PC pony benchmark]

## Generating the graphs and tables

The artifact contains already run benchmarks results, that can be directly used to generate the graphs and tables.  To use the results from the previous section:
```
export RESULTS_DIR=/artifact/outputs
```
To use the included results:
```
export RESULTS_DIR=/artifact/verona-benchmarks/results
```

From inside the docker image, to generate the graphs and tables, run the following commands:
```
python3 verona-benchmarks/scripts/produce_table_actor.py -i $RESULTS_DIR > outputs/table1_pony_boc.tex
```

The second table requires line numbers to be generated for the BoC C++ library. To do this, run the following commands:
```
cloc verona-benchmarks/savina --match-d="actors|boc" --by-file --quiet --csv --out=outputs/cloc_raw.csv  
```

Then run the following command to generate the second table:
```
python3 verona-benchmarks/scripts/produce_table_boc_full.py -i $RESULTS_DIR > outputs/table2_boc_full.tex
```

To generate the graphs, run the following commands:
```
python3 verona-benchmarks/scripts/produce_graph_dining.py -i $RESULTS_DIR -o outputs
python3 verona-benchmarks/scripts/produce_graph_banking_scale.py -i $RESULTS_DIR -o outputs
```


Note that if you have used `--fast` on the `run_dining.py` script, you will need to run the following command to generate the graph for the dining philosophers benchmark:
```
python3 verona-benchmarks/scripts/produce_graph_dining.py -i $RESULTS_DIR -o outputs --fast
```
This should take about 4 minutes to run.

## Finding components

Inside the Docker image you can find

* verona-rt: a checkout of the runtime implementation
* verona-benchmarks: a checkout of the Savina benchmark suite written in against the C++ BoC library.
* pony-savina: a checkout of the Savina benchmark suite written in Pony.

### Verona-rt

The most useful components in

* `verona-rt/test/func/simp*` contains a series of variable simple tests to illustrate the use of the C++ API for BoC.
* `verona-rt/docs/internal/concurrency/modelimpl/` contains the C# model implementation that is presented in the paper.
* `verona-rt/src/rt/sched/behaviourcore.h` contains the core C++ algorithms as described in the paper.

### Verona-benchmarks

* `verona-benchmarks/savina/actors/` contains the implementations of the Savina benchmarks using only single cown `when`.
* `verona-benchmarks/savina/boc` contains the implementation of the Savina benchmarks using multi-cown `when`.
* `verona-benchmarks/results/` contains the results used to generate the graphs and tables in the paper.
* `verona-benchmarks/scripts/` contains the scripts used to generate the graphs and tables in the paper.

### Pony Savina

* `pony-savina/savina-pony` contains the implementation of the Savina benchmarks in Pony.

## Running individual benchmarks

## Profiling benchmarks

It is useful to be able to run individual benchmarks for profiling.

Firstly, to run `perf` inside the Docker image it needs to be started with `--privileged`:
```
docker run --privileged -v .:/artifact/outputs -it verona-when
```

Then you can run the profiling for a particular benchmark as follows:
```
sudo /usr/lib/linux-tools-5.15.0-76/perf record verona-benchmarks/build/savina/savina --actor
 --benchmark "Banking"
```
The results can then be viewed with:
```
sudo /usr/lib/linux-tools-5.15.0-76/perf report
```

This can be used to verify the various comments about
that causes of slowdown in the paper.