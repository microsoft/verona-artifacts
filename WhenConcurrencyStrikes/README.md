# When Concurrency Strikes: Behaviour Oriented Concurrency

This contains the supporting artifact to the paper "When Concurrency Strikes."  The artifact is a Docker script that compares the performance of Pony with the BoC C++ library using the Savina benchmarks suite.

## Running the benchmarks
The instructions assume a working Docker installation and local installation of Git.

First, checkout the artifact repo from Github:
```
git clone https://github.com/microsoft/verona-artifacts
cd verona-artifacts/WhenConcurrencyStrikes
```

Next, build the Docker image:
```
docker build -t verona-when .
```
This step should take about 3-6 minutes depending on your internet connection.

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

The final benchmark run is done manually.  This is the Pony Reasonable Banking benchmark. To run this benchmark, run the following command:
```
./pony-savina/savina-pony/savina-pony --benchmark "Banking 2PC" --reps 5
```
There is no corresponding requirement for the BoC benchmark, as the Banking example as implemented in BoC automatically satisfies all the requirements. 


## Generating the graphs and tables

The artifact contains already run benchmarks results, that can be directly used to generate the graphs and tables.  To use the results from the previous section:
```
export RESULTS_DIR=/artifact/outputs
```
To use the included results:
```
export RESULTS_DIR=/artifact/results
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

## Running individual benchmarks

Both the Pony and Verona benchmarks can be run individually. 
```
verona-benchmarks/build/savina/savina --list
```
provides a list of the available benchmarks.

To run a particular benchmark, use the `--benchmark` option using the single cown `when` versions of the benchmarks:
```
verona-benchmarks/build/savina/savina --actor --benchmark "Recursive Matrix Multiplication"
```
[--actor specifies to use Actor like concurrency]

To use the full power of BoC, and the multi-cown `when` construct, use the following:
```
verona-benchmarks/build/savina/savina --full --benchmark "Recursive Matrix Multiplication"
```
You can increase the number of runs with the `--reps [n]` option. You can output in a machine-readable format with the `--csv` option.


## Profiling benchmarks

It is useful to be able to run individual benchmarks for profiling.

Firstly, to run `perf` inside the Docker image it needs to be started with `--privileged`:
```
docker run --privileged -v .:/artifact/outputs -it verona-when
```

Then you can run the profiling for a particular benchmark as follows our runtime as follows:
```
sudo /usr/lib/linux-tools-5.15.0-76/perf record verona-benchmarks/build/savina/savina --actor --benchmark "Banking"
```
and for Pony as follows:
```
sudo /usr/lib/linux-tools-5.15.0-76/perf record pony-savina/savina-pony/savina-pony --benchmark "Banking"
```

The results can then be viewed with:
```
sudo /usr/lib/linux-tools-5.15.0-76/perf report
```

## Relationship to the paper

Now we will outline the various claims made in the paper and how they relate to the artifact.  The following assumes you have already completed sections "Running the benchmarks" and "Generating the graphs and tables".

* Table 1. This table is generated by running the `produce_table_actor.py` script.  The results are in the `outputs/table1_pony_boc.tex` file.
* Table 3. This table is generated by running the `produce_table_boc_full.py` script.  The results are in the `outputs/table2_boc_full.tex` file.
* Table 4. This table is manually created by running the "Banking 2PC" benchmark on Pony, and using the existing results for BoC (Full).
* Figure 5a. This graph is generated by running the `produce_graph_banking_scale.py` script.  The results are in the `outputs/graph_banking_scale.pdf` file.
* Figure 5b. This graph is generated by running the `produce_graph_dining.py` script.  The results are in the `outputs/graph_dining.pdf` file.
* Claims about reasons for difference, Line 765-789, were done by manually profiling individual examples as explained in the "Profiling benchmarks" section.
* Figure 3. This a compressed version of the the code in `verona-rt/docs/internal/concurrency/modelimpl/`.
* Section 4.3. The core of the C++ implementation sheduling is `verona-rt/src/rt/sched/behaviourcore.h`. 

Inside the Docker image you can find

* `verona-rt`: a checkout of the runtime implementation
* `verona-benchmarks`: a checkout of the Savina benchmark suite written in against the C++ BoC library.
* `pony-savina`: a checkout of the Savina benchmark suite written in Pony.
* `results`: a directory containing the results of the benchmarks that was used to generate the paper.
* `outputs`: a directory where the scripts above will write their output.

### Verona-rt

The most useful components in

* `verona-rt/test/func/simp*` contains a series of variable simple tests to illustrate the use of the C++ API for BoC.
* `verona-rt/docs/internal/concurrency/modelimpl/` contains the C# model implementation that is presented in the paper.
* `verona-rt/src/rt/sched/behaviourcore.h` contains the core C++ algorithms as described in the paper.

### Verona-benchmarks

* `verona-benchmarks/savina/actors/` contains the implementations of the Savina benchmarks using only single cown `when`.
* `verona-benchmarks/savina/boc` contains the implementation of the Savina benchmarks using multi-cown `when`.
* `verona-benchmarks/scripts/` contains the scripts used to generate the graphs and tables in the paper.

### Pony Savina

* `pony-savina/savina-pony` contains the implementation of the Savina benchmarks in Pony.

