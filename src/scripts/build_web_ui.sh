#!/bin/bash

# cd to dir where this script is held so relative paths work
cd "$(dirname "$(readlink -f "$0")")"

fileToProgmem() {
    local file=$1
    local varname=$2

    # use xxd to convert file to C header format
    # need to manually replace types, add PROGMEM, etc:w
    echo "#pragma once"
    xxd -i "$file" \
        | sed "s/.*\[\].*/const char $varname\[\] PROGMEM = {/g" \
        | sed -E "s/unsigned int.*( = .*)/const int ${varname}_len\1/g" \
        | sed -E 's/(.*0x..$.*)/\1, 0x00/g' # add null terminator
}

for file in $(find ../RubberNugget/webUI -type f); do
    [ -e "$file" ] || { echo "invalid file $file" && exit 1; }
    fileName=$(basename $file)
    fileName="${fileName%%.*}"
    outFile="$(dirname ${file})/${fileName}.h"
    varname=${fileName^^}

    # TODO: gzip the file first. The below will work but the server needs to add
    # the "Content-Encoding: gzip" header
    #gzipFile="$file.gz"
    #gzip --keep "$file"
    #./file_to_progmem.sh "$gzipFile" > "$outFile"

    echo "converting $file. Importable as '$fileName.h', variable '$varname'"
    fileToProgmem "$file" "$varname" > "$outFile"
done
