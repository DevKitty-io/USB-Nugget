#!/bin/bash

# cd to dir where this script is held so relative paths work
cd "$(dirname "$(readlink -f "$0")")"

rm ../RubberNugget/webUI/*.h
#rm ../RubberNugget/webUI/*.gz

for file in $(find ../RubberNugget/webUI -type f); do
    [ -e "$file" ] || { echo "invalid file $file" && exit 1; }
    fileName=$(basename $file)
    fileName="${fileName%%.*}"
    outFile="$(dirname ${file})/${fileName}.h"

    # TODO: gzip the file first. The below will work but the server needs to add
    # the "Content-Encoding: gzip" header
    #gzipFile="$file.gz"
    #gzip --keep "$file"
    #./file_to_progmem.sh "$gzipFile" > "$outFile"

    ./file_to_progmem.sh "$file" > "$outFile"
done
