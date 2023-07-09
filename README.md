# Project Verona Artifacts

This repo contains the artifacts associated with the research papers from Project Verona. Here is a summary:

* [Wait free weak reference counting](./WFWeakRC/README.md) - The Verona Runtime uses strong and weak reference counts to manage the cowns lifetimes.  This directory contains the ISMM'23 paper that explains the algorithm to make this wait-free.  The classic algorithms used elsewhere are not wait-free.

* [When Concurrency Strikes](./WhenConcurrencyStrikes/README.md) - This is the artifact associated with the paper "When Concurrency Strikes: Behaviour Oriented Concurrency", which is currently under review. It contains a Docker script to build the benchmarks and the scripts to run them.