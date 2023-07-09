# When Concurrency Strikes: Behaviour Oriented Concurrency

This contains the supporting artifact to the paper "When Concurrency Strikes."  The artifact is a Docker script that compares the performance of Pony with the BoC C++ library using the Savina benchmarks suite.

## Running the benchmarks

The instructions assume a working Docker installation:

First build the Docker image:
```
docker build -t verona-when
```

Then run the docker image:
```
mkdir verona-artifact
cd verona-artifact
docker run -v .:/artifact/outputs -it verona-when
```

From inside the docker image, to run the Savina benchmarks for both Pony and Verona, run the following command:
```
python3 verona-benchmarks/scripts/run_savina.py -o outputs \
  --verona-path verona-benchmarks/build/savina \
  --pony-path pony-savina/savina-pony \
  --repeats 2
```
[You can increase the number of repeats to get more accurate results, but it will take longer to run.]

To run the dining philosophers benchmark for both Verona and std::lock, run the following command:
```
python3 verona-benchmarks/scripts/run_dining.py -o outputs \
  --verona-path verona-rt/build/test \
  --repeats 2 \
  --fast
```
The `--fast` option runs the benchmarks with fewer iterations, to make it faster to run.  To get more accurate results as used in teh paper, remove this option.

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