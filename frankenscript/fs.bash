#!/bin/bash

# Check that a file was given
if [ $# -eq 0 ]; then
    echo "Please specify the .frank file to run"
    exit 1
fi

# Check the file ending
if [[ ! "$1" == *.frank ]]; then
    echo "Invalid argument. The given file must end with .frank"
    exit 1
fi

# Check that the file exists
if [[ ! -f "$1" ]]; then
    echo "The given file name doesn't exist"
    exit 1
fi


frankpath=$1
base=$(basename "$frankpath" .frank)
tmpmd="/tmp/$base.md"

output="/artifact/output/$base.md"
if [[ ! -z "$2" ]]; then
    output=$2
fi

# Make sure the file isn't empty, this is a requirement of frankenscript
if [ ! -s "$frankpath" ]; then
    echo " " > $tmpmd
else
    # Run FrankenScript
    /artifact/frankenscript/build/frankenscript build $frankpath --out $tmpmd
fi


# Check if FrankenScript created an output file.
# If not it probably segfaulted or had a parsing error.
# We'll use that output instead:
if [[ ! -f "$tmpmd" ]]; then
    touch $tmpmd
    echo "**FrankenScript Failed**" >> $tmpmd
    echo " " >> $tmpmd
    echo '```' >> $tmpmd
    /artifact/frankenscript/build/frankenscript build $frankpath --out $tmpmd &>> $tmpmd
    echo '```' >> $tmpmd
fi

echo "### $frankpath" > $output
echo '```' >> $output
cat $frankpath >> $output
echo " " >> $output
echo '```' >> $output
echo "### Output" >> $output
cat $tmpmd >> $output

# Only print the output if no output file was specified
if [[ -z "$2" ]]; then
    echo " "
    echo "Output:"
    echo "  - Local:  $base.md"
    echo "  - Docker: $output"
fi

rm *.trieste
rm $tmpmd
