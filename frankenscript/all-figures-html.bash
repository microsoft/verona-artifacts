#!/bin/bash

set -e

for file in /artifact/figures/*.frank; do
    echo "Processing $file ..."
    ./fs-html.bash "$file" > /dev/null
    echo "... done"
done

