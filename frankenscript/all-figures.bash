#!/bin/bash

set -e

for file in /artifact/figures/*.frank; do
    echo "Processing $file ..."
    ./fs.bash "$file" > /dev/null
    echo "... done"
done
