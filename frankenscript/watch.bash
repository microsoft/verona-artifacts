#!/bin/bash

# Check that a file was given
if [[ $# -ne 1 ]]; then
    echo "Usage: $0 ./output/watch.frank"
    exit 1
fi

script="/artifact/fs.bash"
watch_file="$1"

# Check if the script file exists and is executable
if [[ ! -f "$script" || ! -x "$script" ]]; then
    echo "Error: $script does not exist or is not executable."
    exit 1
fi

# Check if the watch file exists
if [[ ! -f "$watch_file" ]]; then
    echo "Error: $watch_file does not exist."
    exit 1
fi

bash "$script" "$watch_file" > /dev/null

echo "Watching $watch_file for changes..."

while inotifywait -e modify "$watch_file"; do
    echo "Change detected in $watch_file. Running $script..."
    bash "$script" "$watch_file" > /dev/null
done
