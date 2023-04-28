# Wait-free Weak Reference Counting

Repo containing the Supporting evidence for the [Wait-free Weak Reference counting algorithm paper](  https://www.microsoft.com/en-us/research/publication/wait-free-weak-reference-counting/).

The repo is structured as follows:

*  code - contains the implementation
*  paper - contains the paper
*  scripts - contains the python script to generate the graphs for the paper
*  results - contains the benchmarking data, and generated graphs
*  verification - contains the starling proofs for some versions of the code

## Building the code

Pre-requisites: CMake, ninja-build and clang.

To build
```
cd code
mkdir build
cmake -DCMAKE_BUILD_TYPE=Release -G Ninja ..
ninja
```

This should create two binaries:

* rc_tests - runs all the benchmarks and correctness tests a small number of times.  Will build with address sanitizer. 
* rc_perf - runs all the benchmarks for the paper. 

To collect the benchmark results:
```
./rc_perf > results.txt
```
The file `results/results.zip` contains a compressed version of the test run used for the paper.

## Building the graphs

To build the graphs do
```
cd results
unzip results.zip
python3 ../scripts/graph_one.py
```

This will generate all the graphs used in the paper.
It will also generate HTML versions that can be inspected in more detail.


## Building the paper

The paper is `paper/paper.tex` use a standard latex sequence such as `mklatex` to build.  

## Verification

First you need a working installation of .NET and F# to run Starling:

https://learn.microsoft.com/en-us/dotnet/core/install/

Then checkout a version of starling
```
cd verification
git clone https://github.com/MattWindsor91/starling-tool
cd starling-tool
dotnet tool restore
dotnet build
dotnet run --no-build ../waitfree.cvf --project starling
```

This work on Windows, but I haven't got it working on Ubuntu yet.
