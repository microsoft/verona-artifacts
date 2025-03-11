#!/bin/bash

set -e

for file in /artifact/frankenscript/examples/*.frank; do
    echo "Processing $file ..."
    ./fs-html.bash "$file" > /dev/null
    echo "... done"
done

