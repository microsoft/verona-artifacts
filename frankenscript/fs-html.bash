#!/bin/bash

set -e

# Check that a file was given
if [ $# -eq 0 ]; then
    echo "Please specify the .frank file to run"
    exit 1
fi

frankpath=$1
base=$(basename "$frankpath" .frank)
tmpmd="/tmp/$base-html.md"
tmphtml="/tmp/$base.html"
output="/artifact/output/$base.html"

# Generate Markdown
/artifact/fs.bash $frankpath $tmpmd

pandoc --from=markdown --to=html --wrap=none --no-highlight $tmpmd -o $tmphtml
sed -i 's|<code>||g' $tmphtml
sed -i 's|</code>||g' $tmphtml

echo "" > $output
cat /artifact/res/head.txt >> $output
cat $tmphtml >> $output
cat /artifact/res/foot.txt >> $output

echo " "
echo "Output:"
echo "  - Local: $base.html"
echo "  - Docker: $output"

rm $tmpmd $tmphtml
